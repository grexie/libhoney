// Copyright (c) 2015 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file are only available to applications that link
// against the libhoney_dll_wrapper target.
//

#ifndef HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_RESOURCE_MANAGER_H_
#define HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_RESOURCE_MANAGER_H_
#pragma once

#include <list>
#include <memory>

#include "include/base/honey_callback.h"
#include "include/base/honey_ref_counted.h"
#include "include/base/honey_weak_ptr.h"
#include "include/honey_request_handler.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"

///
/// Class for managing multiple resource providers. For each resource request
/// providers will be called in order and have the option to (a) handle the
/// request by returning a HoneycombResourceHandler, (b) pass the request to the next
/// provider in order, or (c) stop handling the request. See comments on the
/// Request object for additional usage information. The methods of this class
/// may be called on any browser process thread unless otherwise indicated.
///
class HoneycombResourceManager
    : public base::RefCountedThreadSafe<HoneycombResourceManager,
                                        HoneycombDeleteOnIOThread> {
 public:
  ///
  /// Provides an opportunity to modify |url| before it is passed to a provider.
  /// For example, the implementation could rewrite |url| to include a default
  /// file extension. |url| will be fully qualified and may contain query or
  /// fragment components.
  ///
  using UrlFilter =
      base::RepeatingCallback<std::string(const std::string& /*url*/)>;

  ///
  /// Used to resolve mime types for URLs, usually based on the file extension.
  /// |url| will be fully qualified and may contain query or fragment
  /// components.
  ///
  using MimeTypeResolver =
      base::RepeatingCallback<std::string(const std::string& /*url*/)>;

 private:
  // Values that stay with a request as it moves between providers.
  struct RequestParams {
    std::string url_;
    HoneycombRefPtr<HoneycombBrowser> browser_;
    HoneycombRefPtr<HoneycombFrame> frame_;
    HoneycombRefPtr<HoneycombRequest> request_;
    UrlFilter url_filter_;
    MimeTypeResolver mime_type_resolver_;
  };

  // Values that are associated with the pending request only.
  struct RequestState;

 public:
  ///
  /// Object representing a request. Each request object is used for a single
  /// call to Provider::OnRequest and will become detached (meaning the
  /// callbacks will no longer trigger) after Request::Continue or Request::Stop
  /// is called. A request passed to Provider::OnRequestCanceled will already
  /// have been detached. The methods of this class may be called on any browser
  /// process thread.
  ///
  class Request : public base::RefCountedThreadSafe<Request> {
   public:
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;

    ///
    /// Returns the URL associated with this request. The returned value will be
    /// fully qualified but will not contain query or fragment components. It
    /// will already have been passed through the URL filter.
    ///
    std::string url() const { return params_.url_; }

    ///
    /// Returns the HoneycombBrowser associated with this request.
    ///
    HoneycombRefPtr<HoneycombBrowser> browser() const { return params_.browser_; }

    ///
    /// Returns the HoneycombFrame associated with this request.
    ///
    HoneycombRefPtr<HoneycombFrame> frame() const { return params_.frame_; }

    ///
    /// Returns the HoneycombRequest associated with this request.
    ///
    HoneycombRefPtr<HoneycombRequest> request() const { return params_.request_; }

    ///
    /// Returns the current URL filter.
    ///
    const HoneycombResourceManager::UrlFilter& url_filter() const {
      return params_.url_filter_;
    }

    ///
    /// Returns the current mime type resolver.
    ///
    const HoneycombResourceManager::MimeTypeResolver& mime_type_resolver() const {
      return params_.mime_type_resolver_;
    }

    ///
    /// Continue handling the request. If |handler| is non-NULL then no
    /// additional providers will be called and the |handler| value will be
    /// returned via HoneycombResourceManager::GetResourceHandler. If |handler| is
    /// NULL then the next provider in order, if any, will be called. If there
    /// are no additional providers then NULL will be returned via
    /// HoneycombResourceManager:: GetResourceHandler.
    ///
    void Continue(HoneycombRefPtr<HoneycombResourceHandler> handler);

    ///
    /// Stop handling the request. No additional providers will be called and
    /// NULL will be returned via HoneycombResourceManager::GetResourceHandler.
    ///
    void Stop();

   private:
    // Only allow deletion via scoped_refptr.
    friend class base::RefCountedThreadSafe<Request>;

    friend class HoneycombResourceManager;

    // The below methods are called on the browser process IO thread.

    explicit Request(std::unique_ptr<RequestState> state);

    std::unique_ptr<RequestState> SendRequest();
    bool HasState();

    static void ContinueOnIOThread(std::unique_ptr<RequestState> state,
                                   HoneycombRefPtr<HoneycombResourceHandler> handler);
    static void StopOnIOThread(std::unique_ptr<RequestState> state);

    // Will be non-NULL while the request is pending. Only accessed on the
    // browser process IO thread.
    std::unique_ptr<RequestState> state_;

    // Params that stay with this request object. Safe to access on any thread.
    RequestParams params_;
  };

  using RequestList = std::list<scoped_refptr<Request>>;

  ///
  /// Interface implemented by resource providers. A provider may be created on
  /// any thread but the methods will be called on, and the object will be
  /// destroyed on, the browser process IO thread.
  ///
  class Provider {
   public:
    ///
    /// Called to handle a request. If the provider knows immediately that it
    /// will not handle the request return false. Otherwise, return true and
    /// call Request::Continue or Request::Stop either in this method or
    /// asynchronously to indicate completion. See comments on Request for
    /// additional usage information.
    ///
    virtual bool OnRequest(scoped_refptr<Request> request) = 0;

    ///
    /// Called when a request has been canceled. It is still safe to dereference
    /// |request| but any calls to Request::Continue or Request::Stop will be
    /// ignored.
    ///
    virtual void OnRequestCanceled(scoped_refptr<Request> request) {}

    virtual ~Provider() {}
  };

  HoneycombResourceManager();

  HoneycombResourceManager(const HoneycombResourceManager&) = delete;
  HoneycombResourceManager& operator=(const HoneycombResourceManager&) = delete;

  ///
  /// Add a provider that maps requests for |url| to |content|. |url| should be
  /// fully qualified but not include a query or fragment component. If
  /// |mime_type| is empty the MimeTypeResolver will be used. See comments on
  /// AddProvider for usage of the |order| and |identifier| parameters.
  ///
  void AddContentProvider(const std::string& url,
                          const std::string& content,
                          const std::string& mime_type,
                          int order,
                          const std::string& identifier);

  ///
  /// Add a provider that maps requests that start with |url_path| to files
  /// under |directory_path|. |url_path| should include an origin and optional
  /// path component only. Files will be loaded when a matching URL is
  /// requested. See comments on AddProvider for usage of the |order| and
  /// |identifier| parameters.
  ///
  void AddDirectoryProvider(const std::string& url_path,
                            const std::string& directory_path,
                            int order,
                            const std::string& identifier);

  ///
  /// Add a provider that maps requests that start with |url_path| to files
  /// stored in the archive file at |archive_path|. |url_path| should include an
  /// origin and optional path component only. The archive file will be loaded
  /// when a matching URL is requested for the first time. See comments on
  /// AddProvider for usage of the |order| and |identifier| parameters.
  ///
  void AddArchiveProvider(const std::string& url_path,
                          const std::string& archive_path,
                          const std::string& password,
                          int order,
                          const std::string& identifier);

  ///
  /// Add a provider. This object takes ownership of |provider|. Providers will
  /// be called in ascending order based on the |order| value. Multiple
  /// providers sharing the same |order| value will be called in the order that
  /// they were added. The |identifier| value, which does not need to be unique,
  /// can be used to remove the provider at a later time.
  ///
  void AddProvider(Provider* provider,
                   int order,
                   const std::string& identifier);

  ///
  /// Remove all providers with the specified |identifier| value. If any removed
  /// providers have pending requests the Provider::OnRequestCancel method will
  /// be called. The removed providers may be deleted immediately or at a later
  /// time.
  ///
  void RemoveProviders(const std::string& identifier);

  ///
  /// Remove all providers. If any removed providers have pending requests the
  /// Provider::OnRequestCancel method will be called. The removed providers may
  /// be deleted immediately or at a later time.
  ///
  void RemoveAllProviders();

  ///
  /// Set the url filter. If not set the default no-op filter will be used.
  /// Changes to this value will not affect currently pending requests.
  ///
  void SetUrlFilter(const UrlFilter& filter);

  ///
  /// Set the mime type resolver. If not set the default resolver will be used.
  /// Changes to this value will not affect currently pending requests.
  ///
  void SetMimeTypeResolver(const MimeTypeResolver& resolver);

  /// The below methods should be called from other Honeycomb handlers. They must be
  /// called exactly as documented for the manager to function correctly.

  ///
  /// Called from HoneycombRequestHandler::OnBeforeResourceLoad on the browser process
  /// IO thread.
  ///
  honey_return_value_t OnBeforeResourceLoad(HoneycombRefPtr<HoneycombBrowser> browser,
                                          HoneycombRefPtr<HoneycombFrame> frame,
                                          HoneycombRefPtr<HoneycombRequest> request,
                                          HoneycombRefPtr<HoneycombCallback> callback);

  ///
  /// Called from HoneycombRequestHandler::GetResourceHandler on the browser process
  /// IO thread.
  ///
  HoneycombRefPtr<HoneycombResourceHandler> GetResourceHandler(
      HoneycombRefPtr<HoneycombBrowser> browser,
      HoneycombRefPtr<HoneycombFrame> frame,
      HoneycombRefPtr<HoneycombRequest> request);

 private:
  // Only allow deletion via scoped_refptr.
  friend struct HoneycombDeleteOnThread<TID_IO>;
  friend class base::RefCountedThreadSafe<HoneycombResourceManager,
                                          HoneycombDeleteOnIOThread>;

  ~HoneycombResourceManager();

  // Provider and associated information.
  struct ProviderEntry;
  using ProviderEntryList = std::list<ProviderEntry*>;

  // Values associated with the pending request only. Ownership will be passed
  // between requests and the resource manager as request handling proceeds.
  struct RequestState {
    ~RequestState();

    base::WeakPtr<HoneycombResourceManager> manager_;

    // Callback to execute once request handling is complete.
    HoneycombRefPtr<HoneycombCallback> callback_;

    // Position of the currently associated ProviderEntry in the |providers_|
    // list.
    ProviderEntryList::iterator current_entry_pos_;

    // Position of this request object in the currently associated
    // ProviderEntry's |pending_requests_| list.
    RequestList::iterator current_request_pos_;

    // Params that will be copied to each request object.
    RequestParams params_;
  };

  // Methods that manage request state between requests. Called on the browser
  // process IO thread.
  bool SendRequest(std::unique_ptr<RequestState> state);
  void ContinueRequest(std::unique_ptr<RequestState> state,
                       HoneycombRefPtr<HoneycombResourceHandler> handler);
  void StopRequest(std::unique_ptr<RequestState> state);
  bool IncrementProvider(RequestState* state);
  void DetachRequestFromProvider(RequestState* state);
  void GetNextValidProvider(ProviderEntryList::iterator& iterator);
  void DeleteProvider(ProviderEntryList::iterator& iterator, bool stop);

  // The below members are only accessed on the browser process IO thread.

  // List of providers including additional associated information.
  ProviderEntryList providers_;

  // Map of response ID to pending HoneycombResourceHandler object.
  using PendingHandlersMap = std::map<uint64_t, HoneycombRefPtr<HoneycombResourceHandler>>;
  PendingHandlersMap pending_handlers_;

  UrlFilter url_filter_;
  MimeTypeResolver mime_type_resolver_;

  // Must be the last member. Created and accessed on the IO thread.
  std::unique_ptr<base::WeakPtrFactory<HoneycombResourceManager>> weak_ptr_factory_;
};

#endif  // HONEYCOMB_INCLUDE_WRAPPER_HONEYCOMB_RESOURCE_MANAGER_H_
