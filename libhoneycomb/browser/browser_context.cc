// Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/browser_context.h"

#include <map>
#include <utility>

#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/media_router/media_router_manager.h"
#include "libhoneycomb/browser/request_context_impl.h"
#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/common/honey_switches.h"
#include "libhoneycomb/common/frame_util.h"
#include "libhoneycomb/features/runtime.h"

#include "base/files/file_util.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"

using content::BrowserThread;

namespace {

// Manages the global list of Impl instances.
class ImplManager {
 public:
  using Vector = std::vector<HoneycombBrowserContext*>;

  ImplManager() {}

  ImplManager(const ImplManager&) = delete;
  ImplManager& operator=(const ImplManager&) = delete;

  ~ImplManager() {
    DCHECK(all_.empty());
    DCHECK(map_.empty());
  }

  void AddImpl(HoneycombBrowserContext* impl) {
    HONEYCOMB_REQUIRE_UIT();
    DCHECK(!IsValidImpl(impl));
    all_.push_back(impl);
  }

  void RemoveImpl(HoneycombBrowserContext* impl, const base::FilePath& path) {
    HONEYCOMB_REQUIRE_UIT();

    {
      Vector::iterator it = GetImplPos(impl);
      DCHECK(it != all_.end());
      all_.erase(it);
    }

    if (!path.empty()) {
      PathMap::iterator it = map_.find(path);
      DCHECK(it != map_.end());
      if (it != map_.end()) {
        map_.erase(it);
      }
    }
  }

  bool IsValidImpl(const HoneycombBrowserContext* impl) {
    HONEYCOMB_REQUIRE_UIT();
    return GetImplPos(impl) != all_.end();
  }

  HoneycombBrowserContext* GetImplFromGlobalId(
      const content::GlobalRenderFrameHostId& global_id,
      bool require_frame_match) {
    HONEYCOMB_REQUIRE_UIT();
    for (const auto& context : all_) {
      if (context->IsAssociatedContext(global_id, require_frame_match)) {
        return context;
      }
    }
    return nullptr;
  }

  HoneycombBrowserContext* GetImplFromBrowserContext(
      const content::BrowserContext* context) {
    HONEYCOMB_REQUIRE_UIT();
    if (!context) {
      return nullptr;
    }

    for (const auto& bc : all_) {
      if (bc->AsBrowserContext() == context) {
        return bc;
      }
    }
    return nullptr;
  }

  void SetImplPath(HoneycombBrowserContext* impl, const base::FilePath& path) {
    HONEYCOMB_REQUIRE_UIT();
    DCHECK(!path.empty());
    DCHECK(IsValidImpl(impl));
    DCHECK(GetImplFromPath(path) == nullptr);
    map_.insert(std::make_pair(path, impl));
  }

  HoneycombBrowserContext* GetImplFromPath(const base::FilePath& path) {
    HONEYCOMB_REQUIRE_UIT();
    DCHECK(!path.empty());
    PathMap::const_iterator it = map_.find(path);
    if (it != map_.end()) {
      return it->second;
    }
    return nullptr;
  }

  const Vector GetAllImpl() const { return all_; }

 private:
  Vector::iterator GetImplPos(const HoneycombBrowserContext* impl) {
    Vector::iterator it = all_.begin();
    for (; it != all_.end(); ++it) {
      if (*it == impl) {
        return it;
      }
    }
    return all_.end();
  }

  using PathMap = std::map<base::FilePath, HoneycombBrowserContext*>;
  PathMap map_;

  Vector all_;
};

#if DCHECK_IS_ON()
// Because of DCHECK()s in the object destructor.
base::LazyInstance<ImplManager>::DestructorAtExit g_manager =
    LAZY_INSTANCE_INITIALIZER;
#else
base::LazyInstance<ImplManager>::Leaky g_manager = LAZY_INSTANCE_INITIALIZER;
#endif

HoneycombBrowserContext* GetSelf(base::WeakPtr<HoneycombBrowserContext> self) {
  HONEYCOMB_REQUIRE_UIT();
  return self.get();
}

HoneycombBrowserContext::CookieableSchemes MakeSupportedSchemes(
    const HoneycombString& schemes_list,
    bool include_defaults) {
  if (schemes_list.empty() && include_defaults) {
    // No explicit registration of schemes.
    return absl::nullopt;
  }

  std::vector<std::string> all_schemes;
  if (!schemes_list.empty()) {
    all_schemes =
        base::SplitString(schemes_list.ToString(), std::string(","),
                          base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  }

  if (include_defaults) {
    // Add default schemes that should always support cookies.
    // This list should match CookieMonster::kDefaultCookieableSchemes.
    all_schemes.push_back("http");
    all_schemes.push_back("https");
    all_schemes.push_back("ws");
    all_schemes.push_back("wss");
  }

  return absl::make_optional(all_schemes);
}

template <typename T>
HoneycombBrowserContext::CookieableSchemes MakeSupportedSchemes(const T& settings) {
  return MakeSupportedSchemes(HoneycombString(&settings.cookieable_schemes_list),
                              !settings.cookieable_schemes_exclude_defaults);
}

}  // namespace

HoneycombBrowserContext::HoneycombBrowserContext(const HoneycombRequestContextSettings& settings)
    : settings_(settings), weak_ptr_factory_(this) {
  g_manager.Get().AddImpl(this);
  getter_ = base::BindRepeating(GetSelf, weak_ptr_factory_.GetWeakPtr());
}

HoneycombBrowserContext::~HoneycombBrowserContext() {
  HONEYCOMB_REQUIRE_UIT();
#if DCHECK_IS_ON()
  DCHECK(is_shutdown_);
#endif
}

void HoneycombBrowserContext::Initialize() {
  cache_path_ = base::FilePath(HoneycombString(&settings_.cache_path));

  if (!cache_path_.empty()) {
    g_manager.Get().SetImplPath(this, cache_path_);
  }

  iothread_state_ = base::MakeRefCounted<HoneycombIOThreadState>();
  cookieable_schemes_ = MakeSupportedSchemes(settings_);
}

void HoneycombBrowserContext::Shutdown() {
  HONEYCOMB_REQUIRE_UIT();

#if DCHECK_IS_ON()
  is_shutdown_ = true;
#endif

  // No HoneycombRequestContext should be referencing this object any longer.
  DCHECK(request_context_set_.empty());

  // Unregister the context first to avoid re-entrancy during shutdown.
  g_manager.Get().RemoveImpl(this, cache_path_);

  // Destroy objects that may hold references to the MediaRouter.
  media_router_manager_.reset();

  // Invalidate any Getter references to this object.
  weak_ptr_factory_.InvalidateWeakPtrs();
}

void HoneycombBrowserContext::AddHoneycombRequestContext(HoneycombRequestContextImpl* context) {
  HONEYCOMB_REQUIRE_UIT();
  request_context_set_.insert(context);
}

void HoneycombBrowserContext::RemoveHoneycombRequestContext(
    HoneycombRequestContextImpl* context) {
  HONEYCOMB_REQUIRE_UIT();

  request_context_set_.erase(context);

  // Delete ourselves when the reference count reaches zero.
  if (request_context_set_.empty()) {
    Shutdown();

    // Allow the current call stack to unwind before deleting |this|.
    content::BrowserThread::DeleteSoon(HONEYCOMB_UIT, FROM_HERE, this);
  }
}

// static
HoneycombBrowserContext* HoneycombBrowserContext::FromCachePath(
    const base::FilePath& cache_path) {
  return g_manager.Get().GetImplFromPath(cache_path);
}

// static
HoneycombBrowserContext* HoneycombBrowserContext::FromGlobalId(
    const content::GlobalRenderFrameHostId& global_id,
    bool require_frame_match) {
  return g_manager.Get().GetImplFromGlobalId(global_id, require_frame_match);
}

// static
HoneycombBrowserContext* HoneycombBrowserContext::FromBrowserContext(
    const content::BrowserContext* context) {
  return g_manager.Get().GetImplFromBrowserContext(context);
}

// static
HoneycombBrowserContext* HoneycombBrowserContext::FromProfile(const Profile* profile) {
  auto* honey_context = FromBrowserContext(profile);
  if (honey_context) {
    return honey_context;
  }

  if (honey::IsChromeRuntimeEnabled()) {
    auto* original_profile = profile->GetOriginalProfile();
    if (original_profile != profile) {
      // With the Chrome runtime if the user launches an incognito window via
      // the UI we might be associated with the original Profile instead of the
      // (current) incognito profile.
      return FromBrowserContext(original_profile);
    }
  }

  return nullptr;
}

// static
std::vector<HoneycombBrowserContext*> HoneycombBrowserContext::GetAll() {
  return g_manager.Get().GetAllImpl();
}

void HoneycombBrowserContext::OnRenderFrameCreated(
    HoneycombRequestContextImpl* request_context,
    const content::GlobalRenderFrameHostId& global_id,
    bool is_main_frame,
    bool is_guest_view) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(frame_util::IsValidGlobalId(global_id));

  render_id_set_.insert(global_id);

  HoneycombRefPtr<HoneycombRequestContextHandler> handler = request_context->GetHandler();
  if (handler) {
    handler_map_.AddHandler(global_id, handler);

    HONEYCOMB_POST_TASK(HONEYCOMB_IOT, base::BindOnce(&HoneycombIOThreadState::AddHandler,
                                          iothread_state_, global_id, handler));
  }
}

void HoneycombBrowserContext::OnRenderFrameDeleted(
    HoneycombRequestContextImpl* request_context,
    const content::GlobalRenderFrameHostId& global_id,
    bool is_main_frame,
    bool is_guest_view) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(frame_util::IsValidGlobalId(global_id));

  auto it1 = render_id_set_.find(global_id);
  if (it1 != render_id_set_.end()) {
    render_id_set_.erase(it1);
  }

  HoneycombRefPtr<HoneycombRequestContextHandler> handler = request_context->GetHandler();
  if (handler) {
    handler_map_.RemoveHandler(global_id);

    HONEYCOMB_POST_TASK(HONEYCOMB_IOT, base::BindOnce(&HoneycombIOThreadState::RemoveHandler,
                                          iothread_state_, global_id));
  }
}

HoneycombRefPtr<HoneycombRequestContextHandler> HoneycombBrowserContext::GetHandler(
    const content::GlobalRenderFrameHostId& global_id,
    bool require_frame_match) const {
  HONEYCOMB_REQUIRE_UIT();
  return handler_map_.GetHandler(global_id, require_frame_match);
}

bool HoneycombBrowserContext::IsAssociatedContext(
    const content::GlobalRenderFrameHostId& global_id,
    bool require_frame_match) const {
  HONEYCOMB_REQUIRE_UIT();

  if (frame_util::IsValidGlobalId(global_id)) {
    const auto it1 = render_id_set_.find(global_id);
    if (it1 != render_id_set_.end()) {
      return true;
    }
  }

  if (frame_util::IsValidChildId(global_id.child_id) && !require_frame_match) {
    // Choose an arbitrary handler for the same process.
    for (const auto& render_ids : render_id_set_) {
      if (render_ids.child_id == global_id.child_id) {
        return true;
      }
    }
  }

  return false;
}

void HoneycombBrowserContext::RegisterSchemeHandlerFactory(
    const HoneycombString& scheme_name,
    const HoneycombString& domain_name,
    HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory) {
  HONEYCOMB_POST_TASK(
      HONEYCOMB_IOT,
      base::BindOnce(&HoneycombIOThreadState::RegisterSchemeHandlerFactory,
                     iothread_state_, scheme_name, domain_name, factory));
}

void HoneycombBrowserContext::ClearSchemeHandlerFactories() {
  HONEYCOMB_POST_TASK(HONEYCOMB_IOT,
                base::BindOnce(&HoneycombIOThreadState::ClearSchemeHandlerFactories,
                               iothread_state_));
}

void HoneycombBrowserContext::LoadExtension(
    const HoneycombString& root_directory,
    HoneycombRefPtr<HoneycombDictionaryValue> manifest,
    HoneycombRefPtr<HoneycombExtensionHandler> handler,
    HoneycombRefPtr<HoneycombRequestContext> loader_context) {
  NOTIMPLEMENTED();
  if (handler) {
    handler->OnExtensionLoadFailed(ERR_ABORTED);
  }
}

bool HoneycombBrowserContext::GetExtensions(std::vector<HoneycombString>& extension_ids) {
  NOTIMPLEMENTED();
  return false;
}

HoneycombRefPtr<HoneycombExtension> HoneycombBrowserContext::GetExtension(
    const HoneycombString& extension_id) {
  NOTIMPLEMENTED();
  return nullptr;
}

bool HoneycombBrowserContext::UnloadExtension(const HoneycombString& extension_id) {
  NOTIMPLEMENTED();
  return false;
}

bool HoneycombBrowserContext::IsPrintPreviewSupported() const {
  return true;
}

network::mojom::NetworkContext* HoneycombBrowserContext::GetNetworkContext() {
  HONEYCOMB_REQUIRE_UIT();
  auto browser_context = AsBrowserContext();
  return browser_context->GetDefaultStoragePartition()->GetNetworkContext();
}

HoneycombMediaRouterManager* HoneycombBrowserContext::GetMediaRouterManager() {
  HONEYCOMB_REQUIRE_UIT();
  if (!media_router_manager_) {
    media_router_manager_.reset(new HoneycombMediaRouterManager(AsBrowserContext()));
  }
  return media_router_manager_.get();
}

HoneycombBrowserContext::CookieableSchemes HoneycombBrowserContext::GetCookieableSchemes()
    const {
  HONEYCOMB_REQUIRE_UIT();
  return cookieable_schemes_;
}

// static
HoneycombBrowserContext::CookieableSchemes
HoneycombBrowserContext::GetGlobalCookieableSchemes() {
  HONEYCOMB_REQUIRE_UIT();

  static base::NoDestructor<CookieableSchemes> schemes(
      []() { return MakeSupportedSchemes(HoneycombContext::Get()->settings()); }());
  return *schemes;
}
