// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/extensions/extensions_browser_client.h"

#include <utility>

#include "libhoney/browser/alloy/alloy_browser_host_impl.h"
#include "libhoney/browser/browser_context.h"
#include "libhoney/browser/extensions/component_extension_resource_manager.h"
#include "libhoney/browser/extensions/extension_system.h"
#include "libhoney/browser/extensions/extension_system_factory.h"
#include "libhoney/browser/extensions/extension_web_contents_observer.h"
#include "libhoney/browser/extensions/extensions_api_client.h"
#include "libhoney/browser/extensions/extensions_browser_api_provider.h"
#include "libhoney/browser/request_context_impl.h"

#include "chrome/browser/browser_process.h"
#include "chrome/browser/extensions/chrome_url_request_util.h"
#include "chrome/browser/extensions/event_router_forwarder.h"
#include "chrome/browser/media/webrtc/media_device_salt_service_factory.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_frame_host.h"
#include "extensions/browser/api/core_extensions_browser_api_provider.h"
#include "extensions/browser/api/extensions_api_client.h"
#include "extensions/browser/api/mime_handler_private/mime_handler_private.h"
#include "extensions/browser/api/runtime/runtime_api_delegate.h"
#include "extensions/browser/app_sorting.h"
#include "extensions/browser/event_router.h"
#include "extensions/browser/extension_host_delegate.h"
#include "extensions/browser/extensions_browser_interface_binders.h"
#include "extensions/browser/guest_view/mime_handler_view/mime_handler_view_guest.h"
#include "extensions/browser/kiosk/kiosk_delegate.h"
#include "extensions/browser/url_request_util.h"
#include "extensions/common/api/mime_handler.mojom.h"
#include "extensions/common/constants.h"

using content::BrowserContext;
using content::BrowserThread;

namespace extensions {

namespace {

void BindMimeHandlerService(
    content::RenderFrameHost* frame_host,
    mojo::PendingReceiver<extensions::mime_handler::MimeHandlerService>
        receiver) {
  auto* web_contents = content::WebContents::FromRenderFrameHost(frame_host);
  if (!web_contents) {
    return;
  }

  auto* guest_view =
      extensions::MimeHandlerViewGuest::FromWebContents(web_contents);
  if (!guest_view) {
    return;
  }
  extensions::MimeHandlerServiceImpl::Create(guest_view->GetStreamWeakPtr(),
                                             std::move(receiver));
}

void BindBeforeUnloadControl(
    content::RenderFrameHost* frame_host,
    mojo::PendingReceiver<extensions::mime_handler::BeforeUnloadControl>
        receiver) {
  auto* web_contents = content::WebContents::FromRenderFrameHost(frame_host);
  if (!web_contents) {
    return;
  }

  auto* guest_view =
      extensions::MimeHandlerViewGuest::FromWebContents(web_contents);
  if (!guest_view) {
    return;
  }
  guest_view->FuseBeforeUnloadControl(std::move(receiver));
}

// Dummy KiosDelegate that always returns false
class HoneycombKioskDelegate : public extensions::KioskDelegate {
 public:
  HoneycombKioskDelegate() = default;
  ~HoneycombKioskDelegate() override = default;

  // KioskDelegate overrides:
  bool IsAutoLaunchedKioskApp(const ExtensionId& id) const override {
    return false;
  }
};

}  // namespace

HoneycombExtensionsBrowserClient::HoneycombExtensionsBrowserClient()
    : api_client_(new HoneycombExtensionsAPIClient) {
  AddAPIProvider(std::make_unique<CoreExtensionsBrowserAPIProvider>());
  AddAPIProvider(std::make_unique<HoneycombExtensionsBrowserAPIProvider>());
}

HoneycombExtensionsBrowserClient::~HoneycombExtensionsBrowserClient() {}

// static
HoneycombExtensionsBrowserClient* HoneycombExtensionsBrowserClient::Get() {
  return static_cast<HoneycombExtensionsBrowserClient*>(
      ExtensionsBrowserClient::Get());
}

bool HoneycombExtensionsBrowserClient::IsShuttingDown() {
  return false;
}

bool HoneycombExtensionsBrowserClient::AreExtensionsDisabled(
    const base::CommandLine& command_line,
    BrowserContext* context) {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsValidContext(void* context) {
  return GetOriginalContext(static_cast<BrowserContext*>(context)) != nullptr;
}

bool HoneycombExtensionsBrowserClient::IsSameContext(BrowserContext* first,
                                               BrowserContext* second) {
  // Returns true if |first| and |second| share the same underlying
  // HoneycombBrowserContext.
  return GetOriginalContext(first) == GetOriginalContext(second);
}

bool HoneycombExtensionsBrowserClient::HasOffTheRecordContext(
    BrowserContext* context) {
  // Honeycomb doesn't use incognito contexts.
  return false;
}

BrowserContext* HoneycombExtensionsBrowserClient::GetOffTheRecordContext(
    BrowserContext* context) {
  return nullptr;
}

BrowserContext* HoneycombExtensionsBrowserClient::GetOriginalContext(
    BrowserContext* context) {
  auto honey_browser_context = HoneycombBrowserContext::FromBrowserContext(context);
  if (honey_browser_context) {
    return honey_browser_context->AsBrowserContext();
  }
  return nullptr;
}

content::BrowserContext*
HoneycombExtensionsBrowserClient::GetContextRedirectedToOriginal(
    content::BrowserContext* context,
    bool force_guest_profile) {
  return context;
}

content::BrowserContext* HoneycombExtensionsBrowserClient::GetContextOwnInstance(
    content::BrowserContext* context,
    bool force_guest_profile) {
  return context;
}

content::BrowserContext* HoneycombExtensionsBrowserClient::GetContextForOriginalOnly(
    content::BrowserContext* context,
    bool force_guest_profile) {
  return context;
}

bool HoneycombExtensionsBrowserClient::AreExtensionsDisabledForContext(
    content::BrowserContext* context) {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsGuestSession(BrowserContext* context) const {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsExtensionIncognitoEnabled(
    const std::string& extension_id,
    content::BrowserContext* context) const {
  return false;
}

bool HoneycombExtensionsBrowserClient::CanExtensionCrossIncognito(
    const Extension* extension,
    content::BrowserContext* context) const {
  return false;
}

base::FilePath HoneycombExtensionsBrowserClient::GetBundleResourcePath(
    const network::ResourceRequest& request,
    const base::FilePath& extension_resources_path,
    int* resource_id) const {
  return chrome_url_request_util::GetBundleResourcePath(
      request, extension_resources_path, resource_id);
}

void HoneycombExtensionsBrowserClient::LoadResourceFromResourceBundle(
    const network::ResourceRequest& request,
    mojo::PendingReceiver<network::mojom::URLLoader> loader,
    const base::FilePath& resource_relative_path,
    const int resource_id,
    scoped_refptr<net::HttpResponseHeaders> headers,
    mojo::PendingRemote<network::mojom::URLLoaderClient> client) {
  chrome_url_request_util::LoadResourceFromResourceBundle(
      request, std::move(loader), resource_relative_path, resource_id,
      std::move(headers), std::move(client));
}

bool HoneycombExtensionsBrowserClient::AllowCrossRendererResourceLoad(
    const network::ResourceRequest& request,
    network::mojom::RequestDestination destination,
    ui::PageTransition page_transition,
    int child_id,
    bool is_incognito,
    const Extension* extension,
    const ExtensionSet& extensions,
    const ProcessMap& process_map) {
  bool allowed = false;
  if (url_request_util::AllowCrossRendererResourceLoad(
          request, destination, page_transition, child_id, is_incognito,
          extension, extensions, process_map, &allowed)) {
    return allowed;
  }

  // Couldn't determine if resource is allowed. Block the load.
  return false;
}

PrefService* HoneycombExtensionsBrowserClient::GetPrefServiceForContext(
    BrowserContext* context) {
  return HoneycombBrowserContext::FromBrowserContext(context)
      ->AsProfile()
      ->GetPrefs();
}

void HoneycombExtensionsBrowserClient::GetEarlyExtensionPrefsObservers(
    content::BrowserContext* context,
    std::vector<EarlyExtensionPrefsObserver*>* observers) const {}

ProcessManagerDelegate* HoneycombExtensionsBrowserClient::GetProcessManagerDelegate()
    const {
  return nullptr;
}

std::unique_ptr<ExtensionHostDelegate>
HoneycombExtensionsBrowserClient::CreateExtensionHostDelegate() {
  // Honeycomb does not use the ExtensionHost constructor that calls this method.
  DCHECK(false);
  return std::unique_ptr<ExtensionHostDelegate>();
}

bool HoneycombExtensionsBrowserClient::CreateBackgroundExtensionHost(
    const Extension* extension,
    content::BrowserContext* browser_context,
    const GURL& url,
    ExtensionHost** host) {
  auto honey_browser_context =
      HoneycombBrowserContext::FromBrowserContext(browser_context);

  // A Honeycomb representation should always exist.
  HoneycombRefPtr<HoneycombExtension> honey_extension =
      honey_browser_context->GetExtension(extension->id());
  DCHECK(honey_extension);
  if (!honey_extension) {
    // Cancel the background host creation.
    return true;
  }

  // Always use the same request context that the extension was registered with.
  // GetLoaderContext() will return NULL for internal extensions.
  HoneycombRefPtr<HoneycombRequestContext> request_context =
      honey_extension->GetLoaderContext();
  if (!request_context) {
    // Cancel the background host creation.
    return true;
  }

  HoneycombBrowserCreateParams create_params;
  create_params.url = url.spec();
  create_params.request_context = request_context;

  HoneycombRefPtr<HoneycombExtensionHandler> handler = honey_extension->GetHandler();
  if (handler.get() && handler->OnBeforeBackgroundBrowser(
                           honey_extension, create_params.url,
                           create_params.client, create_params.settings)) {
    // Cancel the background host creation.
    return true;
  }

  // This triggers creation of the background host.
  create_params.extension = extension;
  create_params.extension_host_type = mojom::ViewType::kExtensionBackgroundPage;

  // Browser creation may fail under certain rare circumstances. Fail the
  // background host creation in that case.
  HoneycombRefPtr<AlloyBrowserHostImpl> browser =
      AlloyBrowserHostImpl::Create(create_params);
  if (browser) {
    *host = browser->GetExtensionHost();
    DCHECK(*host);
  }
  return true;
}

bool HoneycombExtensionsBrowserClient::DidVersionUpdate(BrowserContext* context) {
  // TODO(jamescook): We might want to tell extensions when app_shell updates.
  return false;
}

void HoneycombExtensionsBrowserClient::PermitExternalProtocolHandler() {}

bool HoneycombExtensionsBrowserClient::IsInDemoMode() {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsScreensaverInDemoMode(
    const std::string& app_id) {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsRunningInForcedAppMode() {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsAppModeForcedForApp(
    const ExtensionId& extension_id) {
  return false;
}

bool HoneycombExtensionsBrowserClient::IsLoggedInAsPublicAccount() {
  return false;
}

ExtensionSystemProvider*
HoneycombExtensionsBrowserClient::GetExtensionSystemFactory() {
  return HoneycombExtensionSystemFactory::GetInstance();
}

void HoneycombExtensionsBrowserClient::RegisterBrowserInterfaceBindersForFrame(
    mojo::BinderMapWithContext<content::RenderFrameHost*>* map,
    content::RenderFrameHost* render_frame_host,
    const Extension* extension) const {
  PopulateExtensionFrameBinders(map, render_frame_host, extension);

  map->Add<extensions::mime_handler::MimeHandlerService>(
      base::BindRepeating(&BindMimeHandlerService));
  map->Add<extensions::mime_handler::BeforeUnloadControl>(
      base::BindRepeating(&BindBeforeUnloadControl));
}

std::unique_ptr<RuntimeAPIDelegate>
HoneycombExtensionsBrowserClient::CreateRuntimeAPIDelegate(
    content::BrowserContext* context) const {
  // TODO(extensions): Implement to support Apps.
  DCHECK(false);
  return nullptr;
}

const ComponentExtensionResourceManager*
HoneycombExtensionsBrowserClient::GetComponentExtensionResourceManager() {
  if (!resource_manager_) {
    resource_manager_ =
        std::make_unique<HoneycombComponentExtensionResourceManager>();
  }
  return resource_manager_.get();
}

void HoneycombExtensionsBrowserClient::BroadcastEventToRenderers(
    events::HistogramValue histogram_value,
    const std::string& event_name,
    base::Value::List args,
    bool dispatch_to_off_the_record_profiles) {
  g_browser_process->extension_event_router_forwarder()
      ->BroadcastEventToRenderers(histogram_value, event_name, std::move(args),
                                  GURL(), dispatch_to_off_the_record_profiles);
}

ExtensionCache* HoneycombExtensionsBrowserClient::GetExtensionCache() {
  // Only used by Chrome via ExtensionService.
  DCHECK(false);
  return nullptr;
}

bool HoneycombExtensionsBrowserClient::IsBackgroundUpdateAllowed() {
  return true;
}

bool HoneycombExtensionsBrowserClient::IsMinBrowserVersionSupported(
    const std::string& min_version) {
  return true;
}

ExtensionWebContentsObserver*
HoneycombExtensionsBrowserClient::GetExtensionWebContentsObserver(
    content::WebContents* web_contents) {
  return HoneycombExtensionWebContentsObserver::FromWebContents(web_contents);
}

KioskDelegate* HoneycombExtensionsBrowserClient::GetKioskDelegate() {
  if (!kiosk_delegate_) {
    kiosk_delegate_.reset(new HoneycombKioskDelegate());
  }
  return kiosk_delegate_.get();
}

bool HoneycombExtensionsBrowserClient::IsLockScreenContext(
    content::BrowserContext* context) {
  return false;
}

std::string HoneycombExtensionsBrowserClient::GetApplicationLocale() {
  return g_browser_process->GetApplicationLocale();
}

media_device_salt::MediaDeviceSaltService*
HoneycombExtensionsBrowserClient::GetMediaDeviceSaltService(
    content::BrowserContext* context) {
  return MediaDeviceSaltServiceFactory::GetInstance()->GetForBrowserContext(
      context);
}

}  // namespace extensions
