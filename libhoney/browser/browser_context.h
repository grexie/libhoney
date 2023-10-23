// Copyright (c) 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_BROWSER_CONTEXT_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_BROWSER_CONTEXT_IMPL_H_
#pragma once

#include <set>
#include <vector>

#include "include/honey_request_context_handler.h"
#include "libhoney/browser/iothread_state.h"
#include "libhoney/browser/request_context_handler_map.h"

#include "base/files/file_path.h"
#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/sequenced_task_runner_helpers.h"
#include "chrome/common/plugin.mojom.h"
#include "services/network/public/mojom/network_context.mojom.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "url/origin.h"

/*
// Classes used in request processing (network, storage, service, etc.):
//
// WC = WebContents
//  Content API representation of a browser. Created by BHI or the system (for
//  popups) and owned by BHI. Keeps a pointer to the content::BrowserContext.
//
// BHI = AlloyBrowserHostImpl
//  Implements the HoneycombBrowser and HoneycombBrowserHost interfaces which are exposed
//  to clients. References an RCI instance. Owns a WC. Lifespan is controlled
//  by client references and HoneycombBrowserInfoManager (until the browser has
//  closed).
//
// RCI = HoneycombRequestContextImpl
//  Implements the HoneycombRequestContext interface which is exposed to clients.
//  Creates or references a BC. Lifespan is controlled by client references and
//  BrowserMainParts (for the global RCI).
//
// BC = HoneycombBrowserContext
//   Is/owns the content::BrowserContext which is the entry point from WC.
//   Owns the IOTS and creates the SPI indirectly. Potentially shared by
//   multiple RCI. Deletes itself when no longer needed by RCI.
//
// SPI = content::StoragePartitionImpl
//   Owns storage-related objects like Quota, IndexedDB, Cache, etc. Created by
//   StoragePartitionImplMap::Get(). Life span is controlled indirectly by BC.
//
// IOTS = HoneycombIOThreadState
//   Stores state for access on the IO thread. Life span is controlled by BC.
//
//
// Relationship diagram:
//   ref = reference (HoneycombRefPtr/scoped_refptr)
//   own = ownership (std::unique_ptr)
//   ptr = raw pointer
//
// BHI -ref-> RCI -ptr-> BC -own-> SPI, IOTS
//                      ^
// BHI -own-> WC -ptr--/
//
//
// How shutdown works:
// 1. AlloyBrowserHostImpl::DestroyBrowser is called on the UI thread after the
//    browser is closed and deletes the WebContents.
// 1. AlloyBrowserHostImpl is destroyed on any thread when the last reference
//    is released.
// 2. HoneycombRequestContextImpl is destroyed (possibly asynchronously) on the UI
//    thread when the last reference is released.
// 3. HoneycombBrowserContext is destroyed on the UI thread when no longer needed
//    by any HoneycombRequestContextImpl (via RemoveHoneycombRequestContext).
// 4. HoneycombIOThreadState is destroyed asynchronously on the IO thread after
//    the owning HoneycombBrowserContext is destroyed.
*/

namespace content {
class BrowserContext;
struct GlobalRenderFrameHostId;
}  // namespace content

class HoneycombMediaRouterManager;
class HoneycombRequestContextImpl;
class Profile;

// Main entry point for configuring behavior on a per-RequestContext basis. The
// content::BrowserContext represented by this class is passed to
// WebContents::Create in AlloyBrowserHostImpl::CreateInternal. Only accessed on
// the UI thread unless otherwise indicated.
class HoneycombBrowserContext {
 public:
  HoneycombBrowserContext(const HoneycombBrowserContext&) = delete;
  HoneycombBrowserContext& operator=(const HoneycombBrowserContext&) = delete;

  // Returns the existing instance, if any, associated with the specified
  // |cache_path|.
  static HoneycombBrowserContext* FromCachePath(const base::FilePath& cache_path);

  // Returns the existing instance, if any, associated with the specified IDs.
  // See comments on IsAssociatedContext() for usage.
  static HoneycombBrowserContext* FromGlobalId(
      const content::GlobalRenderFrameHostId& global_id,
      bool require_frame_match);

  // Returns the underlying HoneycombBrowserContext if any.
  static HoneycombBrowserContext* FromBrowserContext(
      const content::BrowserContext* context);
  static HoneycombBrowserContext* FromProfile(const Profile* profile);

  // Returns all existing HoneycombBrowserContext.
  static std::vector<HoneycombBrowserContext*> GetAll();

  // Returns the content and chrome layer representations of the context.
  virtual content::BrowserContext* AsBrowserContext() = 0;
  virtual Profile* AsProfile() = 0;

  // Returns true if the context is fully initialized.
  virtual bool IsInitialized() const = 0;

  // If the context is fully initialized execute |callback|, otherwise
  // store it until the context is fully initialized.
  virtual void StoreOrTriggerInitCallback(base::OnceClosure callback) = 0;

  // Called from HoneycombRequestContextImpl to track associated objects. This
  // object will delete itself when the count reaches zero.
  void AddHoneycombRequestContext(HoneycombRequestContextImpl* context);
  virtual void RemoveHoneycombRequestContext(HoneycombRequestContextImpl* context);

  // Called from HoneycombRequestContextImpl::OnRenderFrameCreated.
  void OnRenderFrameCreated(HoneycombRequestContextImpl* request_context,
                            const content::GlobalRenderFrameHostId& global_id,
                            bool is_main_frame,
                            bool is_guest_view);

  // Called from HoneycombRequestContextImpl::OnRenderFrameDeleted.
  void OnRenderFrameDeleted(HoneycombRequestContextImpl* request_context,
                            const content::GlobalRenderFrameHostId& global_id,
                            bool is_main_frame,
                            bool is_guest_view);

  // Returns the handler that matches the specified IDs. Pass -1 for unknown
  // values. If |require_frame_match| is true only exact matches will be
  // returned. If |require_frame_match| is false, and there is not an exact
  // match, then the first handler for the same |render_process_id| will be
  // returned.
  HoneycombRefPtr<HoneycombRequestContextHandler> GetHandler(
      const content::GlobalRenderFrameHostId& global_id,
      bool require_frame_match) const;

  // Returns true if this context is associated with the specified IDs. Pass -1
  // for unknown values. If |require_frame_match| is true only exact matches
  // will qualify. If |require_frame_match| is false, and there is not an exact
  // match, then any match for |render_process_id| will qualify.
  bool IsAssociatedContext(const content::GlobalRenderFrameHostId& global_id,
                           bool require_frame_match) const;

  // Called from HoneycombRequestContextImpl methods of the same name.
  void RegisterSchemeHandlerFactory(const HoneycombString& scheme_name,
                                    const HoneycombString& domain_name,
                                    HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory);
  void ClearSchemeHandlerFactories();
  // TODO(chrome-runtime): Make these extension methods pure virtual.
  virtual void LoadExtension(const HoneycombString& root_directory,
                             HoneycombRefPtr<HoneycombDictionaryValue> manifest,
                             HoneycombRefPtr<HoneycombExtensionHandler> handler,
                             HoneycombRefPtr<HoneycombRequestContext> loader_context);
  virtual bool GetExtensions(std::vector<HoneycombString>& extension_ids);
  virtual HoneycombRefPtr<HoneycombExtension> GetExtension(const HoneycombString& extension_id);

  // Called from HoneycombExtensionImpl::Unload().
  virtual bool UnloadExtension(const HoneycombString& extension_id);

  // Returns true if this context supports print preview.
  virtual bool IsPrintPreviewSupported() const;

  network::mojom::NetworkContext* GetNetworkContext();

  HoneycombMediaRouterManager* GetMediaRouterManager();

  using CookieableSchemes = absl::optional<std::vector<std::string>>;

  // Returns the schemes associated with this context specifically, or the
  // global configuration if unset.
  CookieableSchemes GetCookieableSchemes() const;
  static CookieableSchemes GetGlobalCookieableSchemes();

  // These accessors are safe to call from any thread because the values don't
  // change during this object's lifespan.
  const HoneycombRequestContextSettings& settings() const { return settings_; }
  base::FilePath cache_path() const { return cache_path_; }
  scoped_refptr<HoneycombIOThreadState> iothread_state() const {
    return iothread_state_;
  }

  // Used to hold a WeakPtr reference to this this object. The Getter returns
  // nullptr if this object has already been destroyed.
  using Getter = base::RepeatingCallback<HoneycombBrowserContext*()>;
  Getter getter() const { return getter_; }

 protected:
  explicit HoneycombBrowserContext(const HoneycombRequestContextSettings& settings);
  virtual ~HoneycombBrowserContext();

  // Will be called immediately after this object is created.
  virtual void Initialize();

  // Will be called immediately before this object is deleted.
  virtual void Shutdown();

  // Members initialized during construction or Initialize() are safe to access
  // from any thread.
  const HoneycombRequestContextSettings settings_;
  base::FilePath cache_path_;

 private:
  // For DeleteSoon().
  friend class base::DeleteHelper<HoneycombBrowserContext>;

  scoped_refptr<HoneycombIOThreadState> iothread_state_;
  CookieableSchemes cookieable_schemes_;
  std::unique_ptr<HoneycombMediaRouterManager> media_router_manager_;

  // HoneycombRequestContextImpl objects referencing this object.
  std::set<HoneycombRequestContextImpl*> request_context_set_;

  // Map IDs to HoneycombRequestContextHandler objects.
  HoneycombRequestContextHandlerMap handler_map_;

  // Set of global IDs associated with this context.
  using RenderIdSet = std::set<content::GlobalRenderFrameHostId>;
  RenderIdSet render_id_set_;

#if DCHECK_IS_ON()
  bool is_shutdown_ = false;
#endif

  Getter getter_;
  base::WeakPtrFactory<HoneycombBrowserContext> weak_ptr_factory_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_BROWSER_CONTEXT_IMPL_H_
