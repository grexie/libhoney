// Copyright (c) 2019 The Honeycomb Authors. Portions
// Copyright (c) 2018 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this
// source code is governed by a BSD-style license that can be found in the
// LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_URL_LOADER_FACTORY_GETTER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_URL_LOADER_FACTORY_GETTER_H_

#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/sequenced_task_runner_helpers.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"

namespace content {
class BrowserContext;
class RenderFrameHost;
}  // namespace content

namespace network {
class SharedURLLoaderFactory;
class PendingSharedURLLoaderFactory;
}  // namespace network

namespace net_service {

struct URLLoaderFactoryGetterDeleter;

// Helper class for retrieving a URLLoaderFactory that can be bound on any
// thread, and that correctly handles proxied requests.
class URLLoaderFactoryGetter
    : public base::RefCountedThreadSafe<URLLoaderFactoryGetter,
                                        URLLoaderFactoryGetterDeleter> {
 public:
  URLLoaderFactoryGetter(const URLLoaderFactoryGetter&) = delete;
  URLLoaderFactoryGetter& operator=(const URLLoaderFactoryGetter&) = delete;

  // Create a URLLoaderFactoryGetter on the UI thread.
  // |render_frame_host| may be nullptr.
  static scoped_refptr<URLLoaderFactoryGetter> Create(
      content::RenderFrameHost* render_frame_host,
      content::BrowserContext* browser_context);

  // Create a SharedURLLoaderFactory on the current thread. All future calls
  // to this method must be on the same thread.
  scoped_refptr<network::SharedURLLoaderFactory> GetURLLoaderFactory();

 private:
  friend class base::DeleteHelper<URLLoaderFactoryGetter>;
  friend class base::RefCountedThreadSafe<URLLoaderFactoryGetter,
                                          URLLoaderFactoryGetterDeleter>;
  friend struct URLLoaderFactoryGetterDeleter;

  URLLoaderFactoryGetter(std::unique_ptr<network::PendingSharedURLLoaderFactory>
                             loader_factory_info,
                         mojo::PendingRemote<network::mojom::URLLoaderFactory>
                             proxy_factory_remote,
                         mojo::PendingReceiver<network::mojom::URLLoaderFactory>
                             proxy_factory_receiver);
  ~URLLoaderFactoryGetter();

  void DeleteOnCorrectThread() const;

  std::unique_ptr<network::PendingSharedURLLoaderFactory> loader_factory_info_;
  scoped_refptr<network::SharedURLLoaderFactory> lazy_factory_;
  mojo::PendingRemote<network::mojom::URLLoaderFactory> proxy_factory_remote_;
  mojo::PendingReceiver<network::mojom::URLLoaderFactory>
      proxy_factory_receiver_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
};

struct URLLoaderFactoryGetterDeleter {
  static void Destruct(const URLLoaderFactoryGetter* factory_getter) {
    factory_getter->DeleteOnCorrectThread();
  }
};

}  // namespace net_service

#endif  // HONEYCOMB_LIBHONEY_BROWSER_NET_SERVICE_URL_LOADER_FACTORY_GETTER_H_
