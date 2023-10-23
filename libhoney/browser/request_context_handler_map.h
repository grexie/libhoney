// Copyright (c) 2019 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_HANDLER_MAP_
#define HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_HANDLER_MAP_
#pragma once

#include <map>

#include "include/honey_request_context.h"
#include "include/honey_request_context_handler.h"

#include "content/public/browser/global_routing_id.h"

// Tracks HoneycombRequestContextHandler associations on a single thread.
class HoneycombRequestContextHandlerMap {
 public:
  HoneycombRequestContextHandlerMap();

  HoneycombRequestContextHandlerMap(const HoneycombRequestContextHandlerMap&) = delete;
  HoneycombRequestContextHandlerMap& operator=(const HoneycombRequestContextHandlerMap&) =
      delete;

  ~HoneycombRequestContextHandlerMap();

  // Keep track of handlers associated with specific frames. This information
  // originates from frame create/delete notifications in
  // HoneycombBrowserContentsDelegate or HoneycombMimeHandlerViewGuestDelegate which are
  // forwarded via HoneycombRequestContextImpl and HoneycombBrowserContext.
  void AddHandler(const content::GlobalRenderFrameHostId& global_id,
                  HoneycombRefPtr<HoneycombRequestContextHandler> handler);
  void RemoveHandler(const content::GlobalRenderFrameHostId& global_id);

  // Returns the handler that matches the specified IDs. If
  // |require_frame_match| is true only exact matches will be returned. If
  // |require_frame_match| is false, and there is not an exact match, then the
  // first handler for the same |global_id.child_id| will be returned.
  HoneycombRefPtr<HoneycombRequestContextHandler> GetHandler(
      const content::GlobalRenderFrameHostId& global_id,
      bool require_frame_match) const;

 private:
  // Map of global ID to handler. These IDs are guaranteed to uniquely
  // identify a RFH for its complete lifespan. See documentation on
  // RenderFrameHost::GetFrameTreeNodeId() for background.
  using RenderIdHandlerMap = std::map<content::GlobalRenderFrameHostId,
                                      HoneycombRefPtr<HoneycombRequestContextHandler>>;
  RenderIdHandlerMap render_id_handler_map_;
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_REQUEST_CONTEXT_HANDLER_MAP_
