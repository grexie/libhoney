// Copyright (c) 2015 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_IOTHREAD_STATE_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_IOTHREAD_STATE_H_
#pragma once

#include "include/honey_request_context.h"
#include "include/honey_request_context_handler.h"
#include "include/honey_scheme.h"

#include "libhoneycomb/browser/request_context_handler_map.h"

#include "content/public/browser/browser_thread.h"

namespace content {
struct GlobalRenderFrameHostId;
}

class GURL;

// Stores state that will be accessed on the IO thread. Life span is controlled
// by HoneycombBrowserContext. Created on the UI thread but accessed and destroyed on
// the IO thread. See browser_context.h for an object relationship diagram.
class HoneycombIOThreadState : public base::RefCountedThreadSafe<
                             HoneycombIOThreadState,
                             content::BrowserThread::DeleteOnIOThread> {
 public:
  HoneycombIOThreadState();

  HoneycombIOThreadState(const HoneycombIOThreadState&) = delete;
  HoneycombIOThreadState& operator=(const HoneycombIOThreadState&) = delete;

  // See comments in HoneycombRequestContextHandlerMap.
  void AddHandler(const content::GlobalRenderFrameHostId& global_id,
                  HoneycombRefPtr<HoneycombRequestContextHandler> handler);
  void RemoveHandler(const content::GlobalRenderFrameHostId& global_id);
  HoneycombRefPtr<HoneycombRequestContextHandler> GetHandler(
      const content::GlobalRenderFrameHostId& global_id,
      bool require_frame_match) const;

  // Manage scheme handler factories associated with this context.
  void RegisterSchemeHandlerFactory(const std::string& scheme_name,
                                    const std::string& domain_name,
                                    HoneycombRefPtr<HoneycombSchemeHandlerFactory> factory);
  void ClearSchemeHandlerFactories();
  HoneycombRefPtr<HoneycombSchemeHandlerFactory> GetSchemeHandlerFactory(const GURL& url);

 private:
  friend struct content::BrowserThread::DeleteOnThread<
      content::BrowserThread::IO>;
  friend class base::DeleteHelper<HoneycombIOThreadState>;

  ~HoneycombIOThreadState();

  void InitOnIOThread();

  // Map IDs to HoneycombRequestContextHandler objects.
  HoneycombRequestContextHandlerMap handler_map_;

  // Map (scheme, domain) to factories.
  using SchemeHandlerFactoryMap = std::map<std::pair<std::string, std::string>,
                                           HoneycombRefPtr<HoneycombSchemeHandlerFactory>>;
  SchemeHandlerFactoryMap scheme_handler_factory_map_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_IOTHREAD_STATE_H_
