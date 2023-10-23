// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/extensions/extension_system.h"

#include <string>

#include "libhoneycomb/browser/extension_impl.h"
#include "libhoneycomb/browser/extensions/value_store/honey_value_store_factory.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/common/extensions/extensions_util.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/json/json_string_value_serializer.h"
#include "base/path_service.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "chrome/browser/pdf/pdf_extension_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/common/chrome_paths.h"
#include "components/crx_file/id_util.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"
#include "content/public/browser/plugin_service.h"
#include "content/public/browser/render_process_host.h"
#include "extensions/browser/api/app_runtime/app_runtime_api.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/null_app_sorting.h"
#include "extensions/browser/quota_service.h"
#include "extensions/browser/renderer_startup_helper.h"
#include "extensions/browser/service_worker_manager.h"
#include "extensions/browser/state_store.h"
#include "extensions/browser/unloaded_extension_reason.h"
#include "extensions/common/constants.h"
#include "extensions/common/extension_messages.h"
#include "extensions/common/file_util.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_handlers/mime_types_handler.h"
#include "extensions/common/switches.h"
#include "net/base/mime_util.h"

using content::BrowserContext;

namespace extensions {

namespace {

// Implementation based on ComponentLoader::ParseManifest.
absl::optional<base::Value::Dict> ParseManifest(
    base::StringPiece manifest_contents) {
  JSONStringValueDeserializer deserializer(manifest_contents);
  std::unique_ptr<base::Value> manifest =
      deserializer.Deserialize(nullptr, nullptr);

  if (!manifest.get() || !manifest->is_dict()) {
    LOG(ERROR) << "Failed to parse extension manifest.";
    return absl::nullopt;
  }

  return std::move(*manifest).TakeDict();
}

void ExecuteLoadFailure(HoneycombRefPtr<HoneycombExtensionHandler> handler,
                        honey_errorcode_t result) {
  if (!handler) {
    return;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(ExecuteLoadFailure, handler, result));
    return;
  }

  handler->OnExtensionLoadFailed(result);
}

void LoadExtensionOnUIThread(base::WeakPtr<HoneycombExtensionSystem> context,
                             base::Value::Dict manifest,
                             const base::FilePath& root_directory,
                             bool internal,
                             HoneycombRefPtr<HoneycombRequestContext> loader_context,
                             HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(LoadExtensionOnUIThread, context,
                                          std::move(manifest), root_directory,
                                          internal, loader_context, handler));
    return;
  }

  if (context) {
    context->LoadExtension(std::move(manifest), root_directory, internal,
                           loader_context, handler);
  }
}

void LoadExtensionWithManifest(base::WeakPtr<HoneycombExtensionSystem> context,
                               const std::string& manifest_contents,
                               const base::FilePath& root_directory,
                               bool internal,
                               HoneycombRefPtr<HoneycombRequestContext> loader_context,
                               HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  HONEYCOMB_REQUIRE_BLOCKING();

  auto manifest = ParseManifest(manifest_contents);
  if (!manifest) {
    LOG(WARNING) << "Failed to parse extension manifest";
    ExecuteLoadFailure(handler, ERR_INVALID_ARGUMENT);
    return;
  }

  LoadExtensionOnUIThread(context, std::move(*manifest), root_directory,
                          internal, loader_context, handler);
}

void LoadExtensionFromDisk(base::WeakPtr<HoneycombExtensionSystem> context,
                           const base::FilePath& root_directory,
                           bool internal,
                           HoneycombRefPtr<HoneycombRequestContext> loader_context,
                           HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  HONEYCOMB_REQUIRE_BLOCKING();

  base::FilePath manifest_path = root_directory.AppendASCII("manifest.json");
  std::string manifest_contents;
  if (!base::ReadFileToString(manifest_path, &manifest_contents)) {
    LOG(WARNING) << "Failed to read extension manifest from "
                 << manifest_path.MaybeAsASCII();
    ExecuteLoadFailure(handler, ERR_FILE_NOT_FOUND);
    return;
  }

  LoadExtensionWithManifest(context, manifest_contents, root_directory,
                            internal, loader_context, handler);
}

}  // namespace

HoneycombExtensionSystem::HoneycombExtensionSystem(BrowserContext* browser_context)
    : browser_context_(browser_context),
      initialized_(false),
      registry_(ExtensionRegistry::Get(browser_context)),
      renderer_helper_(
          extensions::RendererStartupHelperFactory::GetForBrowserContext(
              browser_context)),
      weak_ptr_factory_(this) {
  InitPrefs();
}

HoneycombExtensionSystem::~HoneycombExtensionSystem() {}

void HoneycombExtensionSystem::Init() {
  DCHECK(!initialized_);

  // There's complexity here related to the ordering of message delivery. For
  // an extension to load correctly both the ExtensionMsg_Loaded and
  // ExtensionMsg_ActivateExtension messages must be sent. These messages are
  // currently sent by RendererStartupHelper, ExtensionWebContentsObserver, and
  // this class. ExtensionMsg_Loaded is handled by Dispatcher::OnLoaded and adds
  // the extension to |extensions_|. ExtensionMsg_ActivateExtension is handled
  // by Dispatcher::OnActivateExtension and adds the extension to
  // |active_extension_ids_|. If these messages are not sent correctly then
  // ScriptContextSet::Register called from Dispatcher::DidCreateScriptContext
  // will classify the extension incorrectly and API bindings will not be added.

  // Inform the rest of the extensions system to start.
  ready_.Signal();

  // Add the internal PDF extension. PDF loading works as follows:
  // 1. The PDF plugin is registered in libhoneycomb/common/content_client.cc
  //    ComputeBuiltInPlugins to handle the pdf::kInternalPluginMimeType.
  // 2. The PDF extension is registered by the below call to AddExtension and
  //    associated with the "application/pdf" mime type.
  // 3. Web content running in the owner HoneycombBrowser requests to load a PDF file
  //    resource with the "application/pdf" mime type. This can be via a frame
  //    (main frame/iframe) or object/embed tag.
  // 4. PluginResponseInterceptorURLLoaderThrottle intercepts the PDF resource
  //    load in the browser process and registers the PDF resource as a stream
  //    via MimeHandlerStreamManager::AddStream.
  // 5. PluginResponseInterceptorURLLoaderThrottle::WillProcessResponse triggers
  //    creation of a MimeHandlerViewEmbedder in the browser process via
  //    MimeHandlerViewAttachHelper::OverrideBodyForInterceptedResponse.
  // 6. MimeHandlerViewEmbedder::ReadyToCommitNavigation is called and sends a
  //    Mojo message to MimeHandlerViewContainerManager::SetInternalId in the
  //    owner renderer process.
  // 7. The MimeHandlerViewContainerManager is created in the owner renderer
  //    process via MimeHandlerViewContainerManager::BindReceiver and the
  //    SetInternalId call arrives.
  // 8. HTMLPlugInElement::RequestObject is called in the owner renderer process
  //    to handle the PDF file frame/object/embed tag. This results in calls to
  //    ContentBrowserClient::GetPluginMimeTypesWithExternalHandlers (browser
  //    process) and ContentRendererClient::IsPluginHandledExternally (owner
  //    renderer process), and determines that the plugin should be handled
  //    externally (handled_externally=true).
  // 9. MimeHandlerViewContainerManager::IsManagedByContainerManager sends a
  //    Mojo message to MimeHandlerViewEmbedder::ReadyToCreateMimeHandlerView
  //    in the browser process.
  // 10.MimeHandlerViewEmbedder::RenderFrameCreated triggers creation of a
  //    MimeHandlerViewGuest and HoneycombMimeHandlerViewGuestDelegate in the browser
  //    process.
  // 11.MimeHandlerViewGuest::CreateWebContents creates a new guest WebContents
  //    (is_guest_view=true) to host the PDF extension and the PDF resource
  //    stream is retrieved via MimeHandlerStreamManager::ReleaseStream.
  // 12.MimeHandlerViewGuest::DidAttachToEmbedder calls
  //    HoneycombMimeHandlerViewGuestDelegate::OnGuestAttached to associate the guest
  //    WebContents routing IDs with the owner HoneycombBrowser. MimeHandlerViewGuest
  //    then loads the extension URL (index.html) in the guest WebContents.
  // 13.Creation of the RenderFrame in the guest renderer process triggers a
  //    sync IPC call from AlloyContentRendererClient::MaybeCreateBrowser to
  //    HoneycombBrowserInfoManager::GetBrowserInfo in the browser process to retrieve
  //    the HoneycombBrowser information, which will be immediately available due to
  //    step 12.
  // 14.The PDF extension begins to load. Extension resource requests are
  //    handled via ExtensionURLLoaderFactory::CreateLoaderAndStart in the
  //    browser process. Access to PDF extension resources is checked by
  //    HoneycombExtensionsBrowserClient::AllowCrossRendererResourceLoad and
  //    PDF extension resources are provided from bundle via
  //    HoneycombExtensionsBrowserClient::LoadResourceFromResourceBundle
  //    and HoneycombComponentExtensionResourceManager. Access to chrome://resources
  //    is granted via HoneycombExtensionWebContentsObserver::RenderViewCreated.
  // 15.The PDF extension requests the PDF plugin to handle
  //    pdf::kInternalPluginMimeType. Approval arrives in the guest renderer
  //    process via ExtensionFrameHelper::OnExtensionResponse which calls
  //    NativeExtensionBindingsSystem::HandleResponse. This triggers creation of
  //    an HTMLPlugInElement via native V8 bindings to host the PDF plugin.
  // 16.- With the old PPAPI plugin:
  //      The PDF extension calls chrome.mimeHandlerPrivate.getStreamInfo
  //      (chrome/browser/resources/pdf/browser_api.js) to retrieve the PDF
  //      resource stream. This API is implemented using Mojo as described in
  //      libhoneycomb/common/extensions/api/README.txt.
  //    - With the new PdfUnseasoned plugin:
  //      The PDF resource navigation is redirected by PdfNavigationThrottle and
  //      the stream contents are replaced by PdfURLLoaderRequestInterceptor.
  // 17.HTMLPlugInElement::RequestObject is called in the guest renderer process
  //    and determines that the PDF plugin should be handled internally
  //    (handled_externally=false). A PluginDocument is created and
  //    AlloyContentRendererClient::OverrideCreatePlugin is called to create a
  //    WebPlugin.
  // 18.- With the old PPAPI plugin:
  //      The PDF plugin is loaded by ChromeContentRendererClient::CreatePlugin
  //      calling RenderFrameImpl::CreatePlugin.
  //    - With the new PdfUnseasoned plugin:
  //      The PDF plugin is loaded by ChromeContentRendererClient::CreatePlugin
  //      calling pdf::CreateInternalPlugin.
  // 19.The PDF extension and PDF plugin are now loaded. Print commands, if
  //    any, are handled in the guest renderer process by ChromePDFPrintClient
  //    and HoneycombPrintRenderFrameHelperDelegate.
  // 20.When navigating away from the PDF file or closing the owner HoneycombBrowser
  //    the guest WebContents will be destroyed. This triggers a call to
  //    HoneycombMimeHandlerViewGuestDelegate::OnGuestDetached which removes the
  //    routing ID association with the owner HoneycombBrowser.
  if (PdfExtensionEnabled()) {
    if (auto manifest = ParseManifest(pdf_extension_util::GetManifest())) {
      LoadExtension(std::move(*manifest),
                    base::FilePath(FILE_PATH_LITERAL("pdf")),
                    true /* internal */, nullptr, nullptr);
    }
  }

  initialized_ = true;
}

void HoneycombExtensionSystem::LoadExtension(
    const base::FilePath& root_directory,
    bool internal,
    HoneycombRefPtr<HoneycombRequestContext> loader_context,
    HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  HONEYCOMB_REQUIRE_UIT();
  HONEYCOMB_POST_USER_VISIBLE_TASK(
      base::BindOnce(LoadExtensionFromDisk, weak_ptr_factory_.GetWeakPtr(),
                     root_directory, internal, loader_context, handler));
}

void HoneycombExtensionSystem::LoadExtension(
    const std::string& manifest_contents,
    const base::FilePath& root_directory,
    bool internal,
    HoneycombRefPtr<HoneycombRequestContext> loader_context,
    HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  HONEYCOMB_REQUIRE_UIT();
  HONEYCOMB_POST_USER_VISIBLE_TASK(base::BindOnce(
      LoadExtensionWithManifest, weak_ptr_factory_.GetWeakPtr(),
      manifest_contents, root_directory, internal, loader_context, handler));
}

// Implementation based on ComponentLoader::Add.
void HoneycombExtensionSystem::LoadExtension(
    base::Value::Dict manifest,
    const base::FilePath& root_directory,
    bool internal,
    HoneycombRefPtr<HoneycombRequestContext> loader_context,
    HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  HONEYCOMB_REQUIRE_UIT();

// Internal extensions don't have a loader context. External extensions should.
#if DCHECK_IS_ON()
  if (internal) {
    DCHECK(!loader_context);
  } else {
    DCHECK(loader_context);
  }
#endif

  ComponentExtensionInfo info(std::move(manifest), root_directory, internal);
  const Extension* extension = LoadExtension(info, loader_context, handler);
  if (!extension) {
    ExecuteLoadFailure(handler, ERR_FAILED);
  }
}

// Implementation based on ExtensionService::RemoveComponentExtension.
bool HoneycombExtensionSystem::UnloadExtension(const std::string& extension_id) {
  HONEYCOMB_REQUIRE_UIT();
  ExtensionMap::iterator it = extension_map_.find(extension_id);
  if (it == extension_map_.end()) {
    // No Honeycomb representation so we've already unloaded it.
    return false;
  }

  HoneycombRefPtr<HoneycombExtensionImpl> honey_extension =
      static_cast<HoneycombExtensionImpl*>(it->second.get());

  // Erase first so that callbacks can't retrieve the unloaded extension.
  extension_map_.erase(it);

  honey_extension->OnExtensionUnloaded();

  scoped_refptr<const Extension> extension(
      registry_->GetInstalledExtension(extension_id));
  UnloadExtension(extension_id, UnloadedExtensionReason::UNINSTALL);
  if (extension.get()) {
    registry_->TriggerOnUninstalled(
        extension.get(), extensions::UNINSTALL_REASON_COMPONENT_REMOVED);
  }

  return true;
}

bool HoneycombExtensionSystem::HasExtension(const std::string& extension_id) const {
  return !!GetExtension(extension_id);
}

HoneycombRefPtr<HoneycombExtension> HoneycombExtensionSystem::GetExtension(
    const std::string& extension_id) const {
  HONEYCOMB_REQUIRE_UIT();
  ExtensionMap::const_iterator it = extension_map_.find(extension_id);
  if (it != extension_map_.end()) {
    return it->second;
  }
  return nullptr;
}

HoneycombExtensionSystem::ExtensionMap HoneycombExtensionSystem::GetExtensions() const {
  HONEYCOMB_REQUIRE_UIT();
  return extension_map_;
}

void HoneycombExtensionSystem::OnRequestContextDeleted(HoneycombRequestContext* context) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(context);

  // Make a copy of the map because UnloadExtension will modify it.
  // Don't add any references to |context|.
  ExtensionMap map = extension_map_;
  ExtensionMap::const_iterator it = map.begin();
  for (; it != map.end(); ++it) {
    HoneycombRefPtr<HoneycombExtensionImpl> honey_extension =
        static_cast<HoneycombExtensionImpl*>(it->second.get());
    if (honey_extension->loader_context() == context) {
      UnloadExtension(it->first);
    }
  }
}

void HoneycombExtensionSystem::Shutdown() {
  HONEYCOMB_REQUIRE_UIT();
// Only internal extensions should exist at this point.
#if DCHECK_IS_ON()
  ExtensionMap::iterator it = extension_map_.begin();
  for (; it != extension_map_.end(); ++it) {
    HoneycombRefPtr<HoneycombExtensionImpl> honey_extension =
        static_cast<HoneycombExtensionImpl*>(it->second.get());
    DCHECK(!honey_extension->loader_context());
  }
#endif
  extension_map_.clear();
}

void HoneycombExtensionSystem::InitForRegularProfile(bool extensions_enabled) {
  DCHECK(!initialized_);
  service_worker_manager_.reset(new ServiceWorkerManager(browser_context_));
  quota_service_.reset(new QuotaService);
  app_sorting_.reset(new NullAppSorting);
}

ExtensionService* HoneycombExtensionSystem::extension_service() {
  return nullptr;
}

ManagementPolicy* HoneycombExtensionSystem::management_policy() {
  return nullptr;
}

ServiceWorkerManager* HoneycombExtensionSystem::service_worker_manager() {
  return service_worker_manager_.get();
}

UserScriptManager* HoneycombExtensionSystem::user_script_manager() {
  return nullptr;
}

StateStore* HoneycombExtensionSystem::state_store() {
  return state_store_.get();
}

StateStore* HoneycombExtensionSystem::rules_store() {
  return rules_store_.get();
}

StateStore* HoneycombExtensionSystem::dynamic_user_scripts_store() {
  return nullptr;
}

scoped_refptr<value_store::ValueStoreFactory>
HoneycombExtensionSystem::store_factory() {
  return store_factory_;
}

QuotaService* HoneycombExtensionSystem::quota_service() {
  return quota_service_.get();
}

AppSorting* HoneycombExtensionSystem::app_sorting() {
  return app_sorting_.get();
}

const base::OneShotEvent& HoneycombExtensionSystem::ready() const {
  return ready_;
}

bool HoneycombExtensionSystem::is_ready() const {
  return ready_.is_signaled();
}

ContentVerifier* HoneycombExtensionSystem::content_verifier() {
  return nullptr;
}

std::unique_ptr<ExtensionSet> HoneycombExtensionSystem::GetDependentExtensions(
    const Extension* extension) {
  return std::make_unique<ExtensionSet>();
}

void HoneycombExtensionSystem::InstallUpdate(
    const std::string& extension_id,
    const std::string& public_key,
    const base::FilePath& temp_dir,
    bool install_immediately,
    InstallUpdateCallback install_update_callback) {
  DCHECK(false);
  base::DeletePathRecursively(temp_dir);
}

void HoneycombExtensionSystem::PerformActionBasedOnOmahaAttributes(
    const std::string& extension_id,
    const base::Value::Dict& attributes) {
  DCHECK(false);
}

bool HoneycombExtensionSystem::FinishDelayedInstallationIfReady(
    const std::string& extension_id,
    bool install_immediately) {
  DCHECK(false);
  return false;
}

HoneycombExtensionSystem::ComponentExtensionInfo::ComponentExtensionInfo(
    base::Value::Dict manifest,
    const base::FilePath& directory,
    bool internal)
    : manifest(std::move(manifest)),
      root_directory(directory),
      internal(internal) {
  if (!root_directory.IsAbsolute()) {
    // This path structure is required by
    // url_request_util::MaybeCreateURLRequestResourceBundleJob.
    CHECK(base::PathService::Get(chrome::DIR_RESOURCES, &root_directory));
    root_directory = root_directory.Append(directory);
  }
}

void HoneycombExtensionSystem::InitPrefs() {
  store_factory_ =
      new value_store::HoneycombValueStoreFactory(browser_context_->GetPath());

  Profile* profile = Profile::FromBrowserContext(browser_context_);

  // Two state stores. The latter, which contains declarative rules, must be
  // loaded immediately so that the rules are ready before we issue network
  // requests.
  state_store_ = std::make_unique<StateStore>(
      profile, store_factory_, StateStore::BackendType::STATE, true);

  rules_store_ = std::make_unique<StateStore>(
      profile, store_factory_, StateStore::BackendType::RULES, false);
}

// Implementation based on ComponentLoader::CreateExtension.
scoped_refptr<const Extension> HoneycombExtensionSystem::CreateExtension(
    const ComponentExtensionInfo& info,
    std::string* utf8_error) {
  // TODO(abarth): We should REQUIRE_MODERN_MANIFEST_VERSION once we've updated
  //               our component extensions to the new manifest version.
  int flags = 0;
  if (info.internal) {
    // Internal extensions must have kPublicKey in the manifest.
    flags |= Extension::REQUIRE_KEY;
  }
  return Extension::Create(
      info.root_directory,
      // Tests should continue to use the Manifest::COMMAND_LINE value here
      // Some Chrome APIs will cause undesired effects if this is incorrect
      // e.g.: alarms API has 1 minute minimum applied to Packed Extensions
      info.internal ? mojom::ManifestLocation::kComponent
                    : mojom::ManifestLocation::kCommandLine,
      info.manifest, flags, utf8_error);
}

// Implementation based on ComponentLoader::Load and
// ExtensionService::AddExtension.
const Extension* HoneycombExtensionSystem::LoadExtension(
    const ComponentExtensionInfo& info,
    HoneycombRefPtr<HoneycombRequestContext> loader_context,
    HoneycombRefPtr<HoneycombExtensionHandler> handler) {
  std::string error;
  scoped_refptr<const Extension> extension(CreateExtension(info, &error));
  if (!extension.get()) {
    LOG(ERROR) << error;
    return nullptr;
  }

  if (registry_->GetInstalledExtension(extension->id())) {
    LOG(ERROR) << "Extension with id " << extension->id()
               << "is already installed";
    return nullptr;
  }

  HoneycombRefPtr<HoneycombExtensionImpl> honey_extension =
      new HoneycombExtensionImpl(extension.get(), loader_context.get(), handler);

  // Insert first so that callbacks can retrieve the loaded extension.
  extension_map_.insert(std::make_pair(extension->id(), honey_extension));

  // This may trigger additional callbacks.
  registry_->AddEnabled(extension.get());
  NotifyExtensionLoaded(extension.get());

  honey_extension->OnExtensionLoaded();

  return extension.get();
}

// Implementation based on ExtensionService::UnloadExtension.
void HoneycombExtensionSystem::UnloadExtension(const std::string& extension_id,
                                         UnloadedExtensionReason reason) {
  // Make sure the extension gets deleted after we return from this function.
  int include_mask =
      ExtensionRegistry::EVERYTHING & ~ExtensionRegistry::TERMINATED;
  scoped_refptr<const Extension> extension(
      registry_->GetExtensionById(extension_id, include_mask));

  // This method can be called via PostTask, so the extension may have been
  // unloaded by the time this runs.
  if (!extension.get()) {
    return;
  }

  if (registry_->disabled_extensions().Contains(extension->id())) {
    registry_->RemoveDisabled(extension->id());
    // Don't send the unloaded notification. It was sent when the extension
    // was disabled.
  } else {
    // Remove the extension from the enabled list.
    registry_->RemoveEnabled(extension->id());
    NotifyExtensionUnloaded(extension.get(), reason);
  }
}

// Implementation based on ExtensionService::NotifyExtensionLoaded.
void HoneycombExtensionSystem::NotifyExtensionLoaded(const Extension* extension) {
  // Tell renderers about the loaded extension.
  renderer_helper_->OnExtensionLoaded(*extension);

  // Tell subsystems that use the ExtensionRegistryObserver::OnExtensionLoaded
  // about the new extension.
  //
  // NOTE: It is important that this happen after notifying the renderers about
  // the new extensions so that if we navigate to an extension URL in
  // ExtensionRegistryObserver::OnExtensionLoaded the renderer is guaranteed to
  // know about it.
  registry_->TriggerOnLoaded(extension);

  // Register plugins included with the extension.
  // Implementation based on PluginManager::OnExtensionLoaded.
  const MimeTypesHandler* handler = MimeTypesHandler::GetHandler(extension);
  if (handler && !handler->handler_url().empty()) {
    content::WebPluginInfo info;
    info.type = content::WebPluginInfo::PLUGIN_TYPE_BROWSER_PLUGIN;
    info.name = base::UTF8ToUTF16(extension->name());
    info.path = base::FilePath::FromUTF8Unsafe(extension->url().spec());

    for (std::set<std::string>::const_iterator mime_type =
             handler->mime_type_set().begin();
         mime_type != handler->mime_type_set().end(); ++mime_type) {
      content::WebPluginMimeType mime_type_info;
      mime_type_info.mime_type = *mime_type;
      base::FilePath::StringType file_extension;
      if (net::GetPreferredExtensionForMimeType(*mime_type, &file_extension)) {
        mime_type_info.file_extensions.push_back(
            base::FilePath(file_extension).AsUTF8Unsafe());
      }
      info.mime_types.push_back(mime_type_info);
    }
    content::PluginService* plugin_service =
        content::PluginService::GetInstance();
    plugin_service->RefreshPlugins();
    plugin_service->RegisterInternalPlugin(info, true);
  }
}

// Implementation based on ExtensionService::NotifyExtensionUnloaded.
void HoneycombExtensionSystem::NotifyExtensionUnloaded(
    const Extension* extension,
    UnloadedExtensionReason reason) {
  // Unregister plugins included with the extension.
  // Implementation based on PluginManager::OnExtensionUnloaded.
  const MimeTypesHandler* handler = MimeTypesHandler::GetHandler(extension);
  if (handler && !handler->handler_url().empty()) {
    base::FilePath path =
        base::FilePath::FromUTF8Unsafe(extension->url().spec());
    content::PluginService* plugin_service =
        content::PluginService::GetInstance();
    plugin_service->UnregisterInternalPlugin(path);
    plugin_service->RefreshPlugins();
  }

  registry_->TriggerOnUnloaded(extension, reason);

  // Tell renderers about the unloaded extension.
  renderer_helper_->OnExtensionUnloaded(*extension);
}

}  // namespace extensions
