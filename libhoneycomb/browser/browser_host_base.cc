// Copyright 2020 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/browser_host_base.h"

#include <tuple>

#include "libhoneycomb/browser/browser_info_manager.h"
#include "libhoneycomb/browser/browser_platform_delegate.h"
#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/image_impl.h"
#include "libhoneycomb/browser/navigation_entry_impl.h"
#include "libhoneycomb/browser/printing/print_util.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/common/frame_util.h"
#include "libhoneycomb/common/net/url_util.h"

#include "base/logging.h"
#include "chrome/browser/platform_util.h"
#include "chrome/browser/spellchecker/spellcheck_factory.h"
#include "chrome/browser/spellchecker/spellcheck_service.h"
#include "chrome/browser/ui/browser_commands.h"
#include "components/favicon/core/favicon_url.h"
#include "components/spellcheck/common/spellcheck_features.h"
#include "components/zoom/page_zoom.h"
#include "components/zoom/zoom_controller.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/download_request_utils.h"
#include "content/public/browser/file_select_listener.h"
#include "content/public/browser/navigation_entry.h"
#include "ui/base/resource/resource_scale_factor.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/shell_dialogs/select_file_policy.h"

#if BUILDFLAG(IS_MAC)
#include "components/spellcheck/browser/spellcheck_platform.h"
#endif

namespace {

// Associates a HoneycombBrowserHostBase instance with a WebContents. This object will
// be deleted automatically when the WebContents is destroyed.
class WebContentsUserDataAdapter : public base::SupportsUserData::Data {
 public:
  static void Register(HoneycombRefPtr<HoneycombBrowserHostBase> browser) {
    new WebContentsUserDataAdapter(browser);
  }

  static HoneycombRefPtr<HoneycombBrowserHostBase> Get(
      const content::WebContents* web_contents) {
    WebContentsUserDataAdapter* adapter =
        static_cast<WebContentsUserDataAdapter*>(
            web_contents->GetUserData(UserDataKey()));
    if (adapter) {
      return adapter->browser_;
    }
    return nullptr;
  }

 private:
  WebContentsUserDataAdapter(HoneycombRefPtr<HoneycombBrowserHostBase> browser)
      : browser_(browser) {
    auto web_contents = browser->GetWebContents();
    DCHECK(web_contents);
    web_contents->SetUserData(UserDataKey(), base::WrapUnique(this));
  }

  static void* UserDataKey() {
    // We just need a unique constant. Use the address of a static that
    // COMDAT folding won't touch in an optimizing linker.
    static int data_key = 0;
    return reinterpret_cast<void*>(&data_key);
  }

  HoneycombRefPtr<HoneycombBrowserHostBase> browser_;
};

}  // namespace

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::GetBrowserForHost(
    const content::RenderViewHost* host) {
  DCHECK(host);
  HONEYCOMB_REQUIRE_UIT();
  content::WebContents* web_contents = content::WebContents::FromRenderViewHost(
      const_cast<content::RenderViewHost*>(host));
  if (web_contents) {
    return GetBrowserForContents(web_contents);
  }
  return nullptr;
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::GetBrowserForHost(
    const content::RenderFrameHost* host) {
  DCHECK(host);
  HONEYCOMB_REQUIRE_UIT();
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(
          const_cast<content::RenderFrameHost*>(host));
  if (web_contents) {
    return GetBrowserForContents(web_contents);
  }
  return nullptr;
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::GetBrowserForContents(
    const content::WebContents* contents) {
  DCHECK(contents);
  HONEYCOMB_REQUIRE_UIT();
  return WebContentsUserDataAdapter::Get(contents);
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::GetBrowserForGlobalId(
    const content::GlobalRenderFrameHostId& global_id) {
  if (!frame_util::IsValidGlobalId(global_id)) {
    return nullptr;
  }

  if (HONEYCOMB_CURRENTLY_ON_UIT()) {
    // Use the non-thread-safe but potentially faster approach.
    content::RenderFrameHost* render_frame_host =
        content::RenderFrameHost::FromID(global_id);
    if (!render_frame_host) {
      return nullptr;
    }
    return GetBrowserForHost(render_frame_host);
  } else {
    // Use the thread-safe approach.
    bool is_guest_view = false;
    auto info = HoneycombBrowserInfoManager::GetInstance()->GetBrowserInfo(
        global_id, &is_guest_view);
    if (info && !is_guest_view) {
      auto browser = info->browser();
      if (!browser) {
        LOG(WARNING) << "Found browser id " << info->browser_id()
                     << " but no browser object matching frame "
                     << frame_util::GetFrameDebugString(global_id);
      }
      return browser;
    }
    return nullptr;
  }
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase>
HoneycombBrowserHostBase::GetBrowserForTopLevelNativeWindow(
    gfx::NativeWindow owning_window) {
  DCHECK(owning_window);
  HONEYCOMB_REQUIRE_UIT();

  for (const auto& browser_info :
       HoneycombBrowserInfoManager::GetInstance()->GetBrowserInfoList()) {
    if (auto browser = browser_info->browser()) {
      if (browser->GetTopLevelNativeWindow() == owning_window) {
        return browser;
      }
    }
  }

  return nullptr;
}

// static
HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombBrowserHostBase::GetLikelyFocusedBrowser() {
  HONEYCOMB_REQUIRE_UIT();

  for (const auto& browser_info :
       HoneycombBrowserInfoManager::GetInstance()->GetBrowserInfoList()) {
    if (auto browser = browser_info->browser()) {
      if (browser->IsFocused()) {
        return browser;
      }
    }
  }

  return nullptr;
}

HoneycombBrowserHostBase::HoneycombBrowserHostBase(
    const HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombClient> client,
    std::unique_ptr<HoneycombBrowserPlatformDelegate> platform_delegate,
    scoped_refptr<HoneycombBrowserInfo> browser_info,
    HoneycombRefPtr<HoneycombRequestContextImpl> request_context)
    : settings_(settings),
      client_(client),
      platform_delegate_(std::move(platform_delegate)),
      browser_info_(browser_info),
      request_context_(request_context),
      is_views_hosted_(platform_delegate_->IsViewsHosted()) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(!browser_info_->browser().get());
  browser_info_->SetBrowser(this);

  contents_delegate_ =
      std::make_unique<HoneycombBrowserContentsDelegate>(browser_info_);
  contents_delegate_->AddObserver(this);
}

void HoneycombBrowserHostBase::InitializeBrowser() {
  HONEYCOMB_REQUIRE_UIT();

  // Associate the WebContents with this browser object.
  DCHECK(GetWebContents());
  WebContentsUserDataAdapter::Register(this);
}

void HoneycombBrowserHostBase::DestroyBrowser() {
  HONEYCOMB_REQUIRE_UIT();

  devtools_manager_.reset();
  media_stream_registrar_.reset();

  platform_delegate_.reset();

  contents_delegate_->RemoveObserver(this);
  contents_delegate_->ObserveWebContents(nullptr);

  HoneycombBrowserInfoManager::GetInstance()->RemoveBrowserInfo(browser_info_);
  browser_info_->SetBrowser(nullptr);
}

HoneycombRefPtr<HoneycombBrowser> HoneycombBrowserHostBase::GetBrowser() {
  return this;
}

HoneycombRefPtr<HoneycombClient> HoneycombBrowserHostBase::GetClient() {
  return client_;
}

HoneycombRefPtr<HoneycombRequestContext> HoneycombBrowserHostBase::GetRequestContext() {
  return request_context_;
}

bool HoneycombBrowserHostBase::CanZoom(honey_zoom_command_t command) {
  // Verify that this method is being called on the UI thread.
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  if (auto web_contents = GetWebContents()) {
    switch (command) {
      case HONEYCOMB_ZOOM_COMMAND_OUT:
        return chrome::CanZoomOut(web_contents);
      case HONEYCOMB_ZOOM_COMMAND_RESET:
        return chrome::CanResetZoom(web_contents);
      case HONEYCOMB_ZOOM_COMMAND_IN:
        return chrome::CanZoomIn(web_contents);
    }
  }

  return false;
}

void HoneycombBrowserHostBase::Zoom(honey_zoom_command_t command) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::Zoom, this, command));
    return;
  }

  if (auto web_contents = GetWebContents()) {
    const content::PageZoom page_zoom = [command]() {
      switch (command) {
        case HONEYCOMB_ZOOM_COMMAND_OUT:
          return content::PAGE_ZOOM_OUT;
        case HONEYCOMB_ZOOM_COMMAND_RESET:
          return content::PAGE_ZOOM_RESET;
        case HONEYCOMB_ZOOM_COMMAND_IN:
          return content::PAGE_ZOOM_IN;
      }
    }();

    // Same implementation as chrome::Zoom(), but explicitly specifying the
    // WebContents.
    zoom::PageZoom::Zoom(web_contents, page_zoom);
  }
}

double HoneycombBrowserHostBase::GetDefaultZoomLevel() {
  // Verify that this method is being called on the UI thread.
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return 0.0;
  }

  if (auto web_contents = GetWebContents()) {
    zoom::ZoomController* zoom_controller =
        zoom::ZoomController::FromWebContents(web_contents);
    if (zoom_controller) {
      return zoom_controller->GetDefaultZoomLevel();
    }
  }

  return 0.0;
}

double HoneycombBrowserHostBase::GetZoomLevel() {
  // Verify that this method is being called on the UI thread.
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return 0.0;
  }

  if (auto web_contents = GetWebContents()) {
    zoom::ZoomController* zoom_controller =
        zoom::ZoomController::FromWebContents(web_contents);
    if (zoom_controller) {
      return zoom_controller->GetZoomLevel();
    }
  }

  return 0.0;
}

void HoneycombBrowserHostBase::SetZoomLevel(double zoomLevel) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::SetZoomLevel,
                                          this, zoomLevel));
    return;
  }

  if (auto web_contents = GetWebContents()) {
    zoom::ZoomController* zoom_controller =
        zoom::ZoomController::FromWebContents(web_contents);
    if (zoom_controller) {
      if (zoomLevel == 0.0) {
        // Same logic as PageZoom::Zoom(PAGE_ZOOM_RESET).
        zoomLevel = zoom_controller->GetDefaultZoomLevel();
        web_contents->SetPageScale(1.f);
      }
      zoom_controller->SetZoomLevel(zoomLevel);
    }
  }
}

bool HoneycombBrowserHostBase::HasView() {
  return is_views_hosted_;
}

void HoneycombBrowserHostBase::SetFocus(bool focus) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::SetFocus, this, focus));
    return;
  }

  if (focus) {
    OnSetFocus(FOCUS_SOURCE_SYSTEM);
  } else if (platform_delegate_) {
    platform_delegate_->SetFocus(false);
  }
}

void HoneycombBrowserHostBase::RunFileDialog(
    FileDialogMode mode,
    const HoneycombString& title,
    const HoneycombString& default_file_path,
    const std::vector<HoneycombString>& accept_filters,
    HoneycombRefPtr<HoneycombRunFileDialogCallback> callback) {
  DCHECK(callback);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::RunFileDialog,
                                          this, mode, title, default_file_path,
                                          accept_filters, callback));
    return;
  }

  if (!callback || !EnsureFileDialogManager()) {
    LOG(ERROR) << "File dialog canceled due to invalid state.";
    if (callback) {
      callback->OnFileDialogDismissed({});
    }
    return;
  }

  file_dialog_manager_->RunFileDialog(mode, title, default_file_path,
                                      accept_filters, callback);
}

void HoneycombBrowserHostBase::StartDownload(const HoneycombString& url) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::StartDownload, this, url));
    return;
  }

  GURL gurl = GURL(url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  auto browser_context = web_contents->GetBrowserContext();
  if (!browser_context) {
    return;
  }

  content::DownloadManager* manager = browser_context->GetDownloadManager();
  if (!manager) {
    return;
  }

  std::unique_ptr<download::DownloadUrlParameters> params(
      content::DownloadRequestUtils::CreateDownloadForWebContentsMainFrame(
          web_contents, gurl, MISSING_TRAFFIC_ANNOTATION));
  manager->DownloadUrl(std::move(params));
}

void HoneycombBrowserHostBase::DownloadImage(
    const HoneycombString& image_url,
    bool is_favicon,
    uint32_t max_image_size,
    bool bypass_cache,
    HoneycombRefPtr<HoneycombDownloadImageCallback> callback) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBrowserHostBase::DownloadImage, this, image_url,
                       is_favicon, max_image_size, bypass_cache, callback));
    return;
  }

  if (!callback) {
    return;
  }

  GURL gurl = GURL(image_url.ToString());
  if (gurl.is_empty() || !gurl.is_valid()) {
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  const float scale = ui::GetScaleForMaxSupportedResourceScaleFactor();
  web_contents->DownloadImage(
      gurl, is_favicon, gfx::Size(max_image_size, max_image_size),
      max_image_size * scale, bypass_cache,
      base::BindOnce(
          [](uint32_t max_image_size,
             HoneycombRefPtr<HoneycombDownloadImageCallback> callback, int id,
             int http_status_code, const GURL& image_url,
             const std::vector<SkBitmap>& bitmaps,
             const std::vector<gfx::Size>& sizes) {
            HONEYCOMB_REQUIRE_UIT();

            HoneycombRefPtr<HoneycombImageImpl> image_impl;

            if (!bitmaps.empty()) {
              image_impl = new HoneycombImageImpl();
              image_impl->AddBitmaps(max_image_size, bitmaps);
            }

            callback->OnDownloadImageFinished(
                image_url.spec(), http_status_code, image_impl.get());
          },
          max_image_size, callback));
}

void HoneycombBrowserHostBase::Print() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::Print, this));
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  const bool print_preview_disabled =
      !platform_delegate_ || !platform_delegate_->IsPrintPreviewSupported();
  print_util::Print(web_contents, print_preview_disabled);
}

void HoneycombBrowserHostBase::PrintToPDF(const HoneycombString& path,
                                    const HoneycombPdfPrintSettings& settings,
                                    HoneycombRefPtr<HoneycombPdfPrintCallback> callback) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::PrintToPDF, this,
                                          path, settings, callback));
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  print_util::PrintToPDF(web_contents, path, settings, callback);
}

bool HoneycombBrowserHostBase::SendDevToolsMessage(const void* message,
                                             size_t message_size) {
  if (!message || message_size == 0) {
    return false;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    std::string message_str(static_cast<const char*>(message), message_size);
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(
            [](HoneycombRefPtr<HoneycombBrowserHostBase> self, std::string message_str) {
              self->SendDevToolsMessage(message_str.data(), message_str.size());
            },
            HoneycombRefPtr<HoneycombBrowserHostBase>(this), std::move(message_str)));
    return false;
  }

  if (!EnsureDevToolsManager()) {
    return false;
  }
  return devtools_manager_->SendDevToolsMessage(message, message_size);
}

int HoneycombBrowserHostBase::ExecuteDevToolsMethod(
    int message_id,
    const HoneycombString& method,
    HoneycombRefPtr<HoneycombDictionaryValue> params) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT, base::BindOnce(base::IgnoreResult(
                                    &HoneycombBrowserHostBase::ExecuteDevToolsMethod),
                                this, message_id, method, params));
    return 0;
  }

  if (!EnsureDevToolsManager()) {
    return 0;
  }
  return devtools_manager_->ExecuteDevToolsMethod(message_id, method, params);
}

HoneycombRefPtr<HoneycombRegistration> HoneycombBrowserHostBase::AddDevToolsMessageObserver(
    HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer) {
  if (!observer) {
    return nullptr;
  }
  auto registration = HoneycombDevToolsManager::CreateRegistration(observer);
  InitializeDevToolsRegistrationOnUIThread(registration);
  return registration.get();
}

void HoneycombBrowserHostBase::GetNavigationEntries(
    HoneycombRefPtr<HoneycombNavigationEntryVisitor> visitor,
    bool current_only) {
  DCHECK(visitor.get());
  if (!visitor.get()) {
    return;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::GetNavigationEntries, this,
                                visitor, current_only));
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  content::NavigationController& controller = web_contents->GetController();
  const int total = controller.GetEntryCount();
  const int current = controller.GetCurrentEntryIndex();

  if (current_only) {
    // Visit only the current entry.
    HoneycombRefPtr<HoneycombNavigationEntryImpl> entry =
        new HoneycombNavigationEntryImpl(controller.GetEntryAtIndex(current));
    visitor->Visit(entry.get(), true, current, total);
    std::ignore = entry->Detach(nullptr);
  } else {
    // Visit all entries.
    bool cont = true;
    for (int i = 0; i < total && cont; ++i) {
      HoneycombRefPtr<HoneycombNavigationEntryImpl> entry =
          new HoneycombNavigationEntryImpl(controller.GetEntryAtIndex(i));
      cont = visitor->Visit(entry.get(), (i == current), i, total);
      std::ignore = entry->Detach(nullptr);
    }
  }
}

HoneycombRefPtr<HoneycombNavigationEntry> HoneycombBrowserHostBase::GetVisibleNavigationEntry() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return nullptr;
  }

  content::NavigationEntry* entry = nullptr;
  auto web_contents = GetWebContents();
  if (web_contents) {
    entry = web_contents->GetController().GetVisibleEntry();
  }

  if (!entry) {
    return nullptr;
  }

  return new HoneycombNavigationEntryImpl(entry);
}

void HoneycombBrowserHostBase::NotifyMoveOrResizeStarted() {
#if BUILDFLAG(IS_WIN) || (BUILDFLAG(IS_POSIX) && !BUILDFLAG(IS_MAC))
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBrowserHostBase::NotifyMoveOrResizeStarted, this));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->NotifyMoveOrResizeStarted();
  }
#endif
}

void HoneycombBrowserHostBase::ReplaceMisspelling(const HoneycombString& word) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBrowserHostBase::ReplaceMisspelling, this, word));
    return;
  }

  auto web_contents = GetWebContents();
  if (web_contents) {
    web_contents->ReplaceMisspelling(word);
  }
}

void HoneycombBrowserHostBase::AddWordToDictionary(const HoneycombString& word) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBrowserHostBase::AddWordToDictionary, this, word));
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    return;
  }

  SpellcheckService* spellcheck = nullptr;
  content::BrowserContext* browser_context = web_contents->GetBrowserContext();
  if (browser_context) {
    spellcheck = SpellcheckServiceFactory::GetForContext(browser_context);
    if (spellcheck) {
      spellcheck->GetCustomDictionary()->AddWord(word);
    }
  }
#if BUILDFLAG(IS_MAC)
  if (spellcheck && spellcheck::UseBrowserSpellChecker()) {
    spellcheck_platform::AddWord(spellcheck->platform_spell_checker(), word);
  }
#endif
}

void HoneycombBrowserHostBase::SendKeyEvent(const HoneycombKeyEvent& event) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombBrowserHostBase::SendKeyEvent,
                                          this, event));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendKeyEvent(event);
  }
}

void HoneycombBrowserHostBase::SendMouseClickEvent(const HoneycombMouseEvent& event,
                                             MouseButtonType type,
                                             bool mouseUp,
                                             int clickCount) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::SendMouseClickEvent, this,
                                 event, type, mouseUp, clickCount));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendMouseClickEvent(event, type, mouseUp, clickCount);
  }
}

void HoneycombBrowserHostBase::SendMouseMoveEvent(const HoneycombMouseEvent& event,
                                            bool mouseLeave) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::SendMouseMoveEvent, this,
                                 event, mouseLeave));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendMouseMoveEvent(event, mouseLeave);
  }
}

void HoneycombBrowserHostBase::SendMouseWheelEvent(const HoneycombMouseEvent& event,
                                             int deltaX,
                                             int deltaY) {
  if (deltaX == 0 && deltaY == 0) {
    // Nothing to do.
    return;
  }

  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::SendMouseWheelEvent, this,
                                 event, deltaX, deltaY));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendMouseWheelEvent(event, deltaX, deltaY);
  }
}

bool HoneycombBrowserHostBase::IsValid() {
  return browser_info_->browser() == this;
}

HoneycombRefPtr<HoneycombBrowserHost> HoneycombBrowserHostBase::GetHost() {
  return this;
}

bool HoneycombBrowserHostBase::CanGoBack() {
  base::AutoLock lock_scope(state_lock_);
  return can_go_back_;
}

void HoneycombBrowserHostBase::GoBack() {
  auto callback = base::BindOnce(&HoneycombBrowserHostBase::GoBack, this);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc && wc->GetController().CanGoBack()) {
    wc->GetController().GoBack();
  }
}

bool HoneycombBrowserHostBase::CanGoForward() {
  base::AutoLock lock_scope(state_lock_);
  return can_go_forward_;
}

void HoneycombBrowserHostBase::GoForward() {
  auto callback = base::BindOnce(&HoneycombBrowserHostBase::GoForward, this);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc && wc->GetController().CanGoForward()) {
    wc->GetController().GoForward();
  }
}

bool HoneycombBrowserHostBase::IsLoading() {
  base::AutoLock lock_scope(state_lock_);
  return is_loading_;
}

void HoneycombBrowserHostBase::Reload() {
  auto callback = base::BindOnce(&HoneycombBrowserHostBase::Reload, this);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc) {
    wc->GetController().Reload(content::ReloadType::NORMAL, true);
  }
}

void HoneycombBrowserHostBase::ReloadIgnoreCache() {
  auto callback = base::BindOnce(&HoneycombBrowserHostBase::ReloadIgnoreCache, this);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc) {
    wc->GetController().Reload(content::ReloadType::BYPASSING_CACHE, true);
  }
}

void HoneycombBrowserHostBase::StopLoad() {
  auto callback = base::BindOnce(&HoneycombBrowserHostBase::StopLoad, this);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  auto wc = GetWebContents();
  if (wc) {
    wc->Stop();
  }
}

int HoneycombBrowserHostBase::GetIdentifier() {
  return browser_id();
}

bool HoneycombBrowserHostBase::IsSame(HoneycombRefPtr<HoneycombBrowser> that) {
  auto impl = static_cast<HoneycombBrowserHostBase*>(that.get());
  return (impl == this);
}

bool HoneycombBrowserHostBase::HasDocument() {
  base::AutoLock lock_scope(state_lock_);
  return has_document_;
}

bool HoneycombBrowserHostBase::IsPopup() {
  return browser_info_->is_popup();
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetMainFrame() {
  return GetFrame(HoneycombFrameHostImpl::kMainFrameId);
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetFocusedFrame() {
  return GetFrame(HoneycombFrameHostImpl::kFocusedFrameId);
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetFrame(int64_t identifier) {
  if (identifier == HoneycombFrameHostImpl::kInvalidFrameId) {
    return nullptr;
  } else if (identifier == HoneycombFrameHostImpl::kMainFrameId) {
    return browser_info_->GetMainFrame();
  } else if (identifier == HoneycombFrameHostImpl::kFocusedFrameId) {
    base::AutoLock lock_scope(state_lock_);
    if (!focused_frame_) {
      // The main frame is focused by default.
      return browser_info_->GetMainFrame();
    }
    return focused_frame_;
  }

  return browser_info_->GetFrameForGlobalId(
      frame_util::MakeGlobalId(identifier));
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetFrame(const HoneycombString& name) {
  for (const auto& frame : browser_info_->GetAllFrames()) {
    if (frame->GetName() == name) {
      return frame;
    }
  }
  return nullptr;
}

size_t HoneycombBrowserHostBase::GetFrameCount() {
  return browser_info_->GetAllFrames().size();
}

void HoneycombBrowserHostBase::GetFrameIdentifiers(
    std::vector<int64_t>& identifiers) {
  if (identifiers.size() > 0) {
    identifiers.clear();
  }

  const auto frames = browser_info_->GetAllFrames();
  if (frames.empty()) {
    return;
  }

  identifiers.reserve(frames.size());
  for (const auto& frame : frames) {
    identifiers.push_back(frame->GetIdentifier());
  }
}

void HoneycombBrowserHostBase::GetFrameNames(std::vector<HoneycombString>& names) {
  if (names.size() > 0) {
    names.clear();
  }

  const auto frames = browser_info_->GetAllFrames();
  if (frames.empty()) {
    return;
  }

  names.reserve(frames.size());
  for (const auto& frame : frames) {
    names.push_back(frame->GetName());
  }
}

void HoneycombBrowserHostBase::OnStateChanged(HoneycombBrowserContentsState state_changed) {
  // Make sure that HoneycombBrowser state is consistent before the associated
  // HoneycombClient callback is executed.
  base::AutoLock lock_scope(state_lock_);
  if ((state_changed & HoneycombBrowserContentsState::kNavigation) ==
      HoneycombBrowserContentsState::kNavigation) {
    is_loading_ = contents_delegate_->is_loading();
    can_go_back_ = contents_delegate_->can_go_back();
    can_go_forward_ = contents_delegate_->can_go_forward();
  }
  if ((state_changed & HoneycombBrowserContentsState::kDocument) ==
      HoneycombBrowserContentsState::kDocument) {
    has_document_ = contents_delegate_->has_document();
  }
  if ((state_changed & HoneycombBrowserContentsState::kFullscreen) ==
      HoneycombBrowserContentsState::kFullscreen) {
    is_fullscreen_ = contents_delegate_->is_fullscreen();
  }
  if ((state_changed & HoneycombBrowserContentsState::kFocusedFrame) ==
      HoneycombBrowserContentsState::kFocusedFrame) {
    focused_frame_ = contents_delegate_->focused_frame();
  }
}

void HoneycombBrowserHostBase::OnWebContentsDestroyed(
    content::WebContents* web_contents) {}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetFrameForHost(
    const content::RenderFrameHost* host) {
  HONEYCOMB_REQUIRE_UIT();
  if (!host) {
    return nullptr;
  }

  return browser_info_->GetFrameForHost(host);
}

HoneycombRefPtr<HoneycombFrame> HoneycombBrowserHostBase::GetFrameForGlobalId(
    const content::GlobalRenderFrameHostId& global_id) {
  return browser_info_->GetFrameForGlobalId(global_id, nullptr);
}

void HoneycombBrowserHostBase::AddObserver(Observer* observer) {
  HONEYCOMB_REQUIRE_UIT();
  observers_.AddObserver(observer);
}

void HoneycombBrowserHostBase::RemoveObserver(Observer* observer) {
  HONEYCOMB_REQUIRE_UIT();
  observers_.RemoveObserver(observer);
}

bool HoneycombBrowserHostBase::HasObserver(Observer* observer) const {
  HONEYCOMB_REQUIRE_UIT();
  return observers_.HasObserver(observer);
}

void HoneycombBrowserHostBase::LoadMainFrameURL(
    const content::OpenURLParams& params) {
  auto callback =
      base::BindOnce(&HoneycombBrowserHostBase::LoadMainFrameURL, this, params);
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, std::move(callback));
    return;
  }

  if (browser_info_->IsNavigationLocked(std::move(callback))) {
    return;
  }

  if (Navigate(params)) {
    OnSetFocus(FOCUS_SOURCE_NAVIGATION);
  }
}

bool HoneycombBrowserHostBase::Navigate(const content::OpenURLParams& params) {
  HONEYCOMB_REQUIRE_UIT();
  auto web_contents = GetWebContents();
  if (web_contents) {
    GURL gurl = params.url;
    if (!url_util::FixupGURL(gurl)) {
      return false;
    }

    web_contents->GetController().LoadURL(
        gurl, params.referrer, params.transition, params.extra_headers);
    return true;
  }
  return false;
}

void HoneycombBrowserHostBase::ViewText(const std::string& text) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombBrowserHostBase::ViewText, this, text));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->ViewText(text);
  }
}

void HoneycombBrowserHostBase::RunFileChooserForBrowser(
    const blink::mojom::FileChooserParams& params,
    HoneycombFileDialogManager::RunFileChooserCallback callback) {
  if (!EnsureFileDialogManager()) {
    LOG(ERROR) << "File dialog canceled due to invalid state.";
    std::move(callback).Run({});
    return;
  }
  file_dialog_manager_->RunFileChooser(params, std::move(callback));
}

void HoneycombBrowserHostBase::RunSelectFile(
    ui::SelectFileDialog::Listener* listener,
    std::unique_ptr<ui::SelectFilePolicy> policy,
    ui::SelectFileDialog::Type type,
    const std::u16string& title,
    const base::FilePath& default_path,
    const ui::SelectFileDialog::FileTypeInfo* file_types,
    int file_type_index,
    const base::FilePath::StringType& default_extension,
    gfx::NativeWindow owning_window,
    void* params) {
  if (!EnsureFileDialogManager()) {
    LOG(ERROR) << "File dialog canceled due to invalid state.";
    listener->FileSelectionCanceled(params);
    return;
  }
  file_dialog_manager_->RunSelectFile(listener, std::move(policy), type, title,
                                      default_path, file_types, file_type_index,
                                      default_extension, owning_window, params);
}

void HoneycombBrowserHostBase::SelectFileListenerDestroyed(
    ui::SelectFileDialog::Listener* listener) {
  if (file_dialog_manager_) {
    file_dialog_manager_->SelectFileListenerDestroyed(listener);
  }
}

bool HoneycombBrowserHostBase::MaybeAllowNavigation(
    content::RenderFrameHost* opener,
    bool is_guest_view,
    const content::OpenURLParams& params) {
  return true;
}

void HoneycombBrowserHostBase::OnAfterCreated() {
  HONEYCOMB_REQUIRE_UIT();
  if (client_) {
    if (auto handler = client_->GetLifeSpanHandler()) {
      handler->OnAfterCreated(this);
    }
  }
}

void HoneycombBrowserHostBase::OnBeforeClose() {
  HONEYCOMB_REQUIRE_UIT();
  if (client_) {
    if (auto handler = client_->GetLifeSpanHandler()) {
      handler->OnBeforeClose(this);
    }
  }
  browser_info_->SetClosing();
}

void HoneycombBrowserHostBase::OnBrowserDestroyed() {
  HONEYCOMB_REQUIRE_UIT();

  // Destroy any platform constructs.
  if (file_dialog_manager_) {
    file_dialog_manager_->Destroy();
    file_dialog_manager_.reset();
  }

  for (auto& observer : observers_) {
    observer.OnBrowserDestroyed(this);
  }
}

int HoneycombBrowserHostBase::browser_id() const {
  return browser_info_->browser_id();
}

SkColor HoneycombBrowserHostBase::GetBackgroundColor() const {
  // Don't use |platform_delegate_| because it's not thread-safe.
  return HoneycombContext::Get()->GetBackgroundColor(
      &settings_, IsWindowless() ? STATE_ENABLED : STATE_DISABLED);
}

bool HoneycombBrowserHostBase::IsWindowless() const {
  return false;
}

content::WebContents* HoneycombBrowserHostBase::GetWebContents() const {
  HONEYCOMB_REQUIRE_UIT();
  return contents_delegate_->web_contents();
}

content::BrowserContext* HoneycombBrowserHostBase::GetBrowserContext() const {
  HONEYCOMB_REQUIRE_UIT();
  auto web_contents = GetWebContents();
  if (web_contents) {
    return web_contents->GetBrowserContext();
  }
  return nullptr;
}

HoneycombMediaStreamRegistrar* HoneycombBrowserHostBase::GetMediaStreamRegistrar() {
  HONEYCOMB_REQUIRE_UIT();
  if (!media_stream_registrar_) {
    media_stream_registrar_ = std::make_unique<HoneycombMediaStreamRegistrar>(this);
  }
  return media_stream_registrar_.get();
}

views::Widget* HoneycombBrowserHostBase::GetWindowWidget() const {
  HONEYCOMB_REQUIRE_UIT();
  if (!platform_delegate_) {
    return nullptr;
  }
  return platform_delegate_->GetWindowWidget();
}

HoneycombRefPtr<HoneycombBrowserView> HoneycombBrowserHostBase::GetBrowserView() const {
  HONEYCOMB_REQUIRE_UIT();
  if (is_views_hosted_ && platform_delegate_) {
    return platform_delegate_->GetBrowserView();
  }
  return nullptr;
}

gfx::NativeWindow HoneycombBrowserHostBase::GetTopLevelNativeWindow() const {
  HONEYCOMB_REQUIRE_UIT();
  // Windowless browsers always return nullptr from GetTopLevelNativeWindow().
  if (!IsWindowless()) {
    auto web_contents = GetWebContents();
    if (web_contents) {
      return web_contents->GetTopLevelNativeWindow();
    }
  }
  return gfx::NativeWindow();
}

bool HoneycombBrowserHostBase::IsFocused() const {
  HONEYCOMB_REQUIRE_UIT();
  auto web_contents = GetWebContents();
  if (web_contents) {
    return static_cast<content::RenderFrameHostImpl*>(
               web_contents->GetPrimaryMainFrame())
        ->IsFocused();
  }
  return false;
}

bool HoneycombBrowserHostBase::IsVisible() const {
  HONEYCOMB_REQUIRE_UIT();
  // Windowless browsers always return nullptr from GetNativeView().
  if (!IsWindowless()) {
    auto web_contents = GetWebContents();
    if (web_contents) {
      return platform_util::IsVisible(web_contents->GetNativeView());
    }
  }
  return false;
}

bool HoneycombBrowserHostBase::EnsureDevToolsManager() {
  HONEYCOMB_REQUIRE_UIT();
  if (!contents_delegate_->web_contents()) {
    return false;
  }

  if (!devtools_manager_) {
    devtools_manager_ = std::make_unique<HoneycombDevToolsManager>(this);
  }
  return true;
}

void HoneycombBrowserHostBase::InitializeDevToolsRegistrationOnUIThread(
    HoneycombRefPtr<HoneycombRegistration> registration) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(
            &HoneycombBrowserHostBase::InitializeDevToolsRegistrationOnUIThread, this,
            registration));
    return;
  }

  if (!EnsureDevToolsManager()) {
    return;
  }
  devtools_manager_->InitializeRegistrationOnUIThread(registration);
}

bool HoneycombBrowserHostBase::EnsureFileDialogManager() {
  HONEYCOMB_REQUIRE_UIT();
  if (!contents_delegate_->web_contents()) {
    return false;
  }

  if (!file_dialog_manager_) {
    file_dialog_manager_ = std::make_unique<HoneycombFileDialogManager>(this);
  }
  return true;
}
