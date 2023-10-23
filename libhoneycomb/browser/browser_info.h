// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_INFO_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_INFO_H_
#pragma once

#include <queue>
#include <set>
#include <unordered_map>

#include "include/internal/honey_ptr.h"
#include "libhoneycomb/common/values_impl.h"

#include "base/containers/unique_ptr_adapters.h"
#include "base/functional/callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "base/values.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/render_frame_host.h"

class HoneycombBrowserHostBase;
class HoneycombFrameHandler;
class HoneycombFrameHostImpl;

// HoneycombBrowserInfo is used to associate a browser ID and render view/process
// IDs with a particular HoneycombBrowserHostBase. Render view/process IDs may change
// during the lifetime of a single HoneycombBrowserHostBase.
//
// HoneycombBrowserInfo objects are managed by HoneycombBrowserInfoManager and should not be
// created directly.
class HoneycombBrowserInfo : public base::RefCountedThreadSafe<HoneycombBrowserInfo> {
 public:
  HoneycombBrowserInfo(int browser_id,
                 bool is_popup,
                 bool is_windowless,
                 HoneycombRefPtr<HoneycombDictionaryValue> extra_info);

  HoneycombBrowserInfo(const HoneycombBrowserInfo&) = delete;
  HoneycombBrowserInfo& operator=(const HoneycombBrowserInfo&) = delete;

  int browser_id() const { return browser_id_; }
  bool is_popup() const { return is_popup_; }
  bool is_windowless() const { return is_windowless_; }
  HoneycombRefPtr<HoneycombDictionaryValue> extra_info() const { return extra_info_; }

  // May return NULL if the browser has not yet been created or if the browser
  // has been destroyed.
  HoneycombRefPtr<HoneycombBrowserHostBase> browser() const;

  // Set or clear the browser. Called from HoneycombBrowserHostBase InitializeBrowser
  // (to set) and DestroyBrowser (to clear).
  void SetBrowser(HoneycombRefPtr<HoneycombBrowserHostBase> browser);

  // Called after OnBeforeClose and before SetBrowser(nullptr). This will cause
  // browser() and GetMainFrame() to return nullptr as expected by
  // HoneycombFrameHandler callbacks. Note that this differs from calling
  // SetBrowser(nullptr) because the WebContents has not yet been destroyed and
  // further frame-related callbacks are expected.
  void SetClosing();

  // Ensure that a frame record exists for |host|. Called for the main frame
  // when the RenderView is created, or for a sub-frame when the associated
  // RenderFrame is created in the renderer process.
  // Called from HoneycombBrowserContentsDelegate::RenderFrameCreated (is_guest_view =
  // false) or HoneycombMimeHandlerViewGuestDelegate::OnGuestAttached (is_guest_view =
  // true).
  void MaybeCreateFrame(content::RenderFrameHost* host, bool is_guest_view);

  // Used to track state changes such as entering/exiting the BackForwardCache.
  // Called from HoneycombBrowserContentsDelegate::RenderFrameHostStateChanged.
  void FrameHostStateChanged(
      content::RenderFrameHost* host,
      content::RenderFrameHost::LifecycleState old_state,
      content::RenderFrameHost::LifecycleState new_state);

  // Remove the frame record for |host|. Called for the main frame when the
  // RenderView is destroyed, or for a sub-frame when the associated RenderFrame
  // is destroyed in the renderer process.
  // Called from HoneycombBrowserContentsDelegate::RenderFrameDeleted or
  // HoneycombMimeHandlerViewGuestDelegate::OnGuestDetached.
  void RemoveFrame(content::RenderFrameHost* host);

  // Returns the main frame object. This object will remain valid until the
  // browser is destroyed even though the indentifier may change with cross-
  // origin navigations. Furthermore, calling LoadURL on this object will always
  // behave as expected because the call is routed through the browser's
  // NavigationController.
  HoneycombRefPtr<HoneycombFrameHostImpl> GetMainFrame();

  // Creates a temporary sub-frame object for situations during navigation or
  // resource loading where a RFH does not yet exist. If |parent_frame_id|
  // is invalid the current main frame will be specified as the parent.
  // Temporary frame objects are not tracked but will be implicitly detached
  // on browser destruction.
  HoneycombRefPtr<HoneycombFrameHostImpl> CreateTempSubFrame(
      const content::GlobalRenderFrameHostId& parent_global_id);

  // Returns the frame object matching the specified host or nullptr if no match
  // is found. Nullptr will also be returned if a guest view match is found
  // because we don't create frame objects for guest views. If |is_guest_view|
  // is non-nullptr it will be set to true in this case. Must be called on the
  // UI thread.
  HoneycombRefPtr<HoneycombFrameHostImpl> GetFrameForHost(
      const content::RenderFrameHost* host,
      bool* is_guest_view = nullptr,
      bool prefer_speculative = false) const;

  // Returns the frame object matching the specified ID or nullptr if no match
  // is found. Nullptr will also be returned if a guest view match is found
  // because we don't create frame objects for guest views. If |is_guest_view|
  // is non-nullptr it will be set to true in this case. Safe to call from any
  // thread.
  HoneycombRefPtr<HoneycombFrameHostImpl> GetFrameForGlobalId(
      const content::GlobalRenderFrameHostId& global_id,
      bool* is_guest_view = nullptr,
      bool prefer_speculative = false) const;

  // Returns all non-speculative frame objects that currently exist. Guest views
  // will be excluded because they don't have a frame object. Safe to call from
  // any thread.
  using FrameHostList = std::set<HoneycombRefPtr<HoneycombFrameHostImpl>>;
  FrameHostList GetAllFrames() const;

  class NavigationLock final : public base::RefCounted<NavigationLock> {
   private:
    friend class HoneycombBrowserInfo;
    friend class base::RefCounted<NavigationLock>;

    NavigationLock();
    ~NavigationLock();

    base::OnceClosure pending_action_;
    base::WeakPtrFactory<NavigationLock> weak_ptr_factory_;
  };

  // Block navigation actions on NavigationLock life span. Must be called on the
  // UI thread.
  scoped_refptr<NavigationLock> CreateNavigationLock();

  // Returns true if navigation actions are currently blocked. If this method
  // returns true the most recent |pending_action| will be executed on the UI
  // thread once the navigation lock is released. Must be called on the UI
  // thread.
  bool IsNavigationLocked(base::OnceClosure pending_action);

  using FrameNotifyOnceAction =
      base::OnceCallback<void(HoneycombRefPtr<HoneycombFrameHandler>)>;

  // Specifies a HoneycombFrameHandler notification action whose execution may need
  // to be blocked on release of a potentially held NotificationStateLock. If no
  // HoneycombFrameHandler exists then the action will be discarded without executing.
  // If the NotificationStateLock is not currently held then the action will be
  // executed immediately.
  void MaybeExecuteFrameNotification(FrameNotifyOnceAction pending_action);

  void MaybeNotifyDraggableRegionsChanged(
      HoneycombRefPtr<HoneycombBrowserHostBase> browser,
      HoneycombRefPtr<HoneycombFrameHostImpl> frame,
      std::vector<HoneycombDraggableRegion> draggable_regions);

 private:
  friend class base::RefCountedThreadSafe<HoneycombBrowserInfo>;

  virtual ~HoneycombBrowserInfo();

  struct FrameInfo {
    ~FrameInfo();

    inline bool IsCurrentMainFrame() const {
      return frame_ && is_main_frame_ && !is_speculative_ && !is_in_bfcache_;
    }

    content::RenderFrameHost* host_;
    content::GlobalRenderFrameHostId global_id_;
    bool is_guest_view_;
    bool is_main_frame_;
    bool is_speculative_;
    bool is_in_bfcache_ = false;
    HoneycombRefPtr<HoneycombFrameHostImpl> frame_;
  };

  void SetMainFrame(HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                    HoneycombRefPtr<HoneycombFrameHostImpl> frame);

  void MaybeNotifyFrameCreated(HoneycombRefPtr<HoneycombFrameHostImpl> frame);
  void MaybeNotifyFrameDetached(HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                                HoneycombRefPtr<HoneycombFrameHostImpl> frame);
  void MaybeNotifyMainFrameChanged(HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                                   HoneycombRefPtr<HoneycombFrameHostImpl> old_frame,
                                   HoneycombRefPtr<HoneycombFrameHostImpl> new_frame);

  void RemoveAllFrames(HoneycombRefPtr<HoneycombBrowserHostBase> old_browser);

  int browser_id_;
  bool is_popup_;
  bool is_windowless_;
  HoneycombRefPtr<HoneycombDictionaryValue> extra_info_;

  // Navigation will be blocked while |navigation_lock_| exists.
  // Only accessed on the UI thread.
  base::WeakPtr<NavigationLock> navigation_lock_;

  // Used instead of |base::AutoLock(lock_)| in situations that might generate
  // HoneycombFrameHandler notifications. Any notifications passed to
  // MaybeExecuteFrameNotification() will be queued until the lock is released,
  // and then executed in order. Only accessed on the UI thread.
  class NotificationStateLock final {
   public:
    explicit NotificationStateLock(HoneycombBrowserInfo* browser_info);
    ~NotificationStateLock();

   protected:
    friend class HoneycombBrowserInfo;
    HoneycombBrowserInfo* const browser_info_;
    HoneycombRefPtr<HoneycombFrameHandler> frame_handler_;
    std::unique_ptr<base::AutoLock> browser_info_lock_scope_;
    std::queue<FrameNotifyOnceAction> queue_;
  };

  mutable base::Lock notification_lock_;

  // These members must be protected by |notification_lock_|.
  NotificationStateLock* notification_state_lock_ = nullptr;
  HoneycombRefPtr<HoneycombFrameHandler> frame_handler_;

  mutable base::Lock lock_;

  // The below members must be protected by |lock_|.

  HoneycombRefPtr<HoneycombBrowserHostBase> browser_;

  // Owner of FrameInfo structs.
  using FrameInfoSet =
      std::set<std::unique_ptr<FrameInfo>, base::UniquePtrComparator>;
  FrameInfoSet frame_info_set_;

  // Map a global ID to one frame. These IDs are guaranteed to uniquely
  // identify a RFH for its complete lifespan. See documentation on
  // RenderFrameHost::GetFrameTreeNodeId() for background.
  using FrameIDMap = std::unordered_map<content::GlobalRenderFrameHostId,
                                        FrameInfo*,
                                        content::GlobalRenderFrameHostIdHasher>;
  FrameIDMap frame_id_map_;

  // The current main frame.
  HoneycombRefPtr<HoneycombFrameHostImpl> main_frame_;

  // True if the browser is currently closing.
  bool is_closing_ = false;

  // Only accessed on the UI thread.
  std::vector<HoneycombDraggableRegion> draggable_regions_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_BROWSER_INFO_H_
