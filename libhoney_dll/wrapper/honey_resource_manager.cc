// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_resource_manager.h"

#include <algorithm>
#include <vector>

#include "include/base/honey_callback.h"
#include "include/base/honey_weak_ptr.h"
#include "include/honey_parser.h"
#include "include/wrapper/honey_stream_resource_handler.h"
#include "include/wrapper/honey_zip_archive.h"

namespace {

#if defined(OS_WIN)
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

// Returns |url| without the query or fragment components, if any.
std::string GetUrlWithoutQueryOrFragment(const std::string& url) {
  // Find the first instance of '?' or '#'.
  const size_t pos = std::min(url.find('?'), url.find('#'));
  if (pos != std::string::npos) {
    return url.substr(0, pos);
  }

  return url;
}

// Determine the mime type based on the |url| file extension.
std::string GetMimeType(const std::string& url) {
  std::string mime_type;
  const std::string& url_without_query = GetUrlWithoutQueryOrFragment(url);
  size_t sep = url_without_query.find_last_of(".");
  if (sep != std::string::npos) {
    mime_type = HoneycombGetMimeType(url_without_query.substr(sep + 1));
    if (!mime_type.empty()) {
      return mime_type;
    }
  }
  return "text/html";
}

// Default no-op filter.
std::string GetFilteredUrl(const std::string& url) {
  return url;
}

// Provider of fixed contents.
class ContentProvider : public HoneycombResourceManager::Provider {
 public:
  ContentProvider(const std::string& url,
                  const std::string& content,
                  const std::string& mime_type)
      : url_(url), content_(content), mime_type_(mime_type) {
    DCHECK(!url.empty());
    DCHECK(!content.empty());
  }

  ContentProvider(const ContentProvider&) = delete;
  ContentProvider& operator=(const ContentProvider&) = delete;

  bool OnRequest(scoped_refptr<HoneycombResourceManager::Request> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url != url_) {
      // Not handled by this provider.
      return false;
    }

    HoneycombRefPtr<HoneycombStreamReader> stream = HoneycombStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(content_.data())),
        content_.length());

    // Determine the mime type a single time if it isn't already set.
    if (mime_type_.empty()) {
      mime_type_ = request->mime_type_resolver().Run(url);
    }

    request->Continue(new HoneycombStreamResourceHandler(mime_type_, stream));
    return true;
  }

 private:
  std::string url_;
  std::string content_;
  std::string mime_type_;
};

// Provider of contents loaded from a directory on the file system.
class DirectoryProvider : public HoneycombResourceManager::Provider {
 public:
  DirectoryProvider(const std::string& url_path,
                    const std::string& directory_path)
      : url_path_(url_path), directory_path_(directory_path) {
    DCHECK(!url_path_.empty());
    DCHECK(!directory_path_.empty());

    // Normalize the path values.
    if (url_path_[url_path_.size() - 1] != '/') {
      url_path_ += '/';
    }
    if (directory_path_[directory_path_.size() - 1] != PATH_SEP) {
      directory_path_ += PATH_SEP;
    }
  }

  DirectoryProvider(const DirectoryProvider&) = delete;
  DirectoryProvider& operator=(const DirectoryProvider&) = delete;

  bool OnRequest(scoped_refptr<HoneycombResourceManager::Request> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url.find(url_path_) != 0U) {
      return false;
    }

    const std::string& file_path = GetFilePath(url);

    // Open |file_path| on the FILE thread.
    HoneycombPostTask(TID_FILE_USER_BLOCKING,
                base::BindOnce(&DirectoryProvider::OpenOnFileThread, file_path,
                               request));

    return true;
  }

 private:
  std::string GetFilePath(const std::string& url) {
    std::string path_part = url.substr(url_path_.length());
#if defined(OS_WIN)
    std::replace(path_part.begin(), path_part.end(), '/', '\\');
#endif
    return directory_path_ + path_part;
  }

  static void OpenOnFileThread(
      const std::string& file_path,
      scoped_refptr<HoneycombResourceManager::Request> request) {
    HONEYCOMB_REQUIRE_FILE_USER_BLOCKING_THREAD();

    HoneycombRefPtr<HoneycombStreamReader> stream =
        HoneycombStreamReader::CreateForFile(file_path);

    // Continue loading on the IO thread.
    HoneycombPostTask(TID_IO,
                base::BindOnce(&DirectoryProvider::ContinueOpenOnIOThread,
                               request, stream));
  }

  static void ContinueOpenOnIOThread(
      scoped_refptr<HoneycombResourceManager::Request> request,
      HoneycombRefPtr<HoneycombStreamReader> stream) {
    HONEYCOMB_REQUIRE_IO_THREAD();

    HoneycombRefPtr<HoneycombStreamResourceHandler> handler;
    if (stream.get()) {
      handler = new HoneycombStreamResourceHandler(
          request->mime_type_resolver().Run(request->url()), stream);
    }
    request->Continue(handler);
  }

  std::string url_path_;
  std::string directory_path_;
};

// Provider of contents loaded from an archive file.
class ArchiveProvider : public HoneycombResourceManager::Provider {
 public:
  ArchiveProvider(const std::string& url_path,
                  const std::string& archive_path,
                  const std::string& password)
      : url_path_(url_path),
        archive_path_(archive_path),
        password_(password),
        archive_load_started_(false),
        archive_load_ended_(false),
        weak_ptr_factory_(this) {
    DCHECK(!url_path_.empty());
    DCHECK(!archive_path_.empty());

    // Normalize the path values.
    if (url_path_[url_path_.size() - 1] != '/') {
      url_path_ += '/';
    }
  }

  ArchiveProvider(const ArchiveProvider&) = delete;
  ArchiveProvider& operator=(const ArchiveProvider&) = delete;

  bool OnRequest(scoped_refptr<HoneycombResourceManager::Request> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url.find(url_path_) != 0U) {
      // Not handled by this provider.
      return false;
    }

    if (!archive_load_started_) {
      // Initiate archive loading and queue the pending request.
      archive_load_started_ = true;
      pending_requests_.push_back(request);

      // Load the archive file on the FILE thread.
      HoneycombPostTask(TID_FILE_USER_BLOCKING,
                  base::BindOnce(&ArchiveProvider::LoadOnFileThread,
                                 weak_ptr_factory_.GetWeakPtr(), archive_path_,
                                 password_));
      return true;
    }

    if (archive_load_started_ && !archive_load_ended_) {
      // The archive load has already started. Queue the pending request.
      pending_requests_.push_back(request);
      return true;
    }

    // Archive loading is done.
    return ContinueRequest(request);
  }

 private:
  static void LoadOnFileThread(base::WeakPtr<ArchiveProvider> ptr,
                               const std::string& archive_path,
                               const std::string& password) {
    HONEYCOMB_REQUIRE_FILE_USER_BLOCKING_THREAD();

    HoneycombRefPtr<HoneycombZipArchive> archive;

    HoneycombRefPtr<HoneycombStreamReader> stream =
        HoneycombStreamReader::CreateForFile(archive_path);
    if (stream.get()) {
      archive = new HoneycombZipArchive;
      if (archive->Load(stream, password, true) == 0) {
        DLOG(WARNING) << "Empty archive file: " << archive_path;
        archive = nullptr;
      }
    } else {
      DLOG(WARNING) << "Failed to load archive file: " << archive_path;
    }

    HoneycombPostTask(TID_IO, base::BindOnce(&ArchiveProvider::ContinueOnIOThread,
                                       ptr, archive));
  }

  void ContinueOnIOThread(HoneycombRefPtr<HoneycombZipArchive> archive) {
    HONEYCOMB_REQUIRE_IO_THREAD();

    archive_load_ended_ = true;
    archive_ = archive;

    if (!pending_requests_.empty()) {
      // Continue all pending requests.
      PendingRequests::const_iterator it = pending_requests_.begin();
      for (; it != pending_requests_.end(); ++it) {
        ContinueRequest(*it);
      }
      pending_requests_.clear();
    }
  }

  bool ContinueRequest(scoped_refptr<HoneycombResourceManager::Request> request) {
    HoneycombRefPtr<HoneycombResourceHandler> handler;

    // |archive_| will be NULL if the archive file failed to load or was empty.
    if (archive_.get()) {
      const std::string& url = request->url();
      const std::string& relative_path = url.substr(url_path_.length());
      HoneycombRefPtr<HoneycombZipArchive::File> file = archive_->GetFile(relative_path);
      if (file.get()) {
        handler = new HoneycombStreamResourceHandler(
            request->mime_type_resolver().Run(url), file->GetStreamReader());
      }
    }

    if (!handler.get()) {
      return false;
    }

    request->Continue(handler);
    return true;
  }

  std::string url_path_;
  std::string archive_path_;
  std::string password_;

  bool archive_load_started_;
  bool archive_load_ended_;
  HoneycombRefPtr<HoneycombZipArchive> archive_;

  // List of requests that are pending while the archive is being loaded.
  using PendingRequests =
      std::vector<scoped_refptr<HoneycombResourceManager::Request>>;
  PendingRequests pending_requests_;

  // Must be the last member.
  base::WeakPtrFactory<ArchiveProvider> weak_ptr_factory_;
};

}  // namespace

// HoneycombResourceManager::ProviderEntry implementation.

struct HoneycombResourceManager::ProviderEntry {
  ProviderEntry(Provider* provider, int order, const std::string& identifier)
      : provider_(provider),
        order_(order),
        identifier_(identifier),
        deletion_pending_(false) {}

  std::unique_ptr<Provider> provider_;
  int order_;
  std::string identifier_;

  // List of pending requests currently associated with this provider.
  RequestList pending_requests_;

  // True if deletion of this provider is pending.
  bool deletion_pending_;
};

// HoneycombResourceManager::RequestState implementation.

HoneycombResourceManager::RequestState::~RequestState() {
  // Always execute the callback.
  if (callback_.get()) {
    callback_->Continue();
  }
}

// HoneycombResourceManager::Request implementation.

void HoneycombResourceManager::Request::Continue(
    HoneycombRefPtr<HoneycombResourceHandler> handler) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&HoneycombResourceManager::Request::Continue,
                                       this, handler));
    return;
  }

  if (!state_.get()) {
    return;
  }

  // Disassociate |state_| immediately so that Provider::OnRequestCanceled is
  // not called unexpectedly if Provider::OnRequest calls this method and then
  // calls HoneycombResourceManager::Remove*.
  HoneycombPostTask(TID_IO,
              base::BindOnce(&HoneycombResourceManager::Request::ContinueOnIOThread,
                             std::move(state_), handler));
}

void HoneycombResourceManager::Request::Stop() {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO,
                base::BindOnce(&HoneycombResourceManager::Request::Stop, this));
    return;
  }

  if (!state_.get()) {
    return;
  }

  // Disassociate |state_| immediately so that Provider::OnRequestCanceled is
  // not called unexpectedly if Provider::OnRequest calls this method and then
  // calls HoneycombResourceManager::Remove*.
  HoneycombPostTask(TID_IO,
              base::BindOnce(&HoneycombResourceManager::Request::StopOnIOThread,
                             std::move(state_)));
}

HoneycombResourceManager::Request::Request(std::unique_ptr<RequestState> state)
    : state_(std::move(state)), params_(state_->params_) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  ProviderEntry* entry = *(state_->current_entry_pos_);
  // Should not be on a deleted entry.
  DCHECK(!entry->deletion_pending_);

  // Add this request to the entry's pending request list.
  entry->pending_requests_.push_back(this);
  state_->current_request_pos_ = --entry->pending_requests_.end();
}

// Detaches and returns |state_| if the provider indicates that it will not
// handle the request. Note that |state_| may already be NULL if OnRequest
// executes a callback before returning, in which case execution will continue
// asynchronously in any case.
std::unique_ptr<HoneycombResourceManager::RequestState>
HoneycombResourceManager::Request::SendRequest() {
  HONEYCOMB_REQUIRE_IO_THREAD();
  Provider* provider = (*state_->current_entry_pos_)->provider_.get();
  if (!provider->OnRequest(this)) {
    return std::move(state_);
  }
  return std::unique_ptr<RequestState>();
}

bool HoneycombResourceManager::Request::HasState() {
  HONEYCOMB_REQUIRE_IO_THREAD();
  return (state_.get() != nullptr);
}

// static
void HoneycombResourceManager::Request::ContinueOnIOThread(
    std::unique_ptr<RequestState> state,
    HoneycombRefPtr<HoneycombResourceHandler> handler) {
  HONEYCOMB_REQUIRE_IO_THREAD();
  // The manager may already have been deleted.
  base::WeakPtr<HoneycombResourceManager> manager = state->manager_;
  if (manager) {
    manager->ContinueRequest(std::move(state), handler);
  }
}

// static
void HoneycombResourceManager::Request::StopOnIOThread(
    std::unique_ptr<RequestState> state) {
  HONEYCOMB_REQUIRE_IO_THREAD();
  // The manager may already have been deleted.
  base::WeakPtr<HoneycombResourceManager> manager = state->manager_;
  if (manager) {
    manager->StopRequest(std::move(state));
  }
}

// HoneycombResourceManager implementation.

HoneycombResourceManager::HoneycombResourceManager()
    : url_filter_(base::BindRepeating(GetFilteredUrl)),
      mime_type_resolver_(base::BindRepeating(GetMimeType)) {}

HoneycombResourceManager::~HoneycombResourceManager() {
  HONEYCOMB_REQUIRE_IO_THREAD();
  RemoveAllProviders();

  // Delete all entryies now. Requests may still be pending but they will not
  // call back into this manager due to the use of WeakPtr.
  if (!providers_.empty()) {
    ProviderEntryList::iterator it = providers_.begin();
    for (; it != providers_.end(); ++it) {
      delete *it;
    }
    providers_.clear();
  }
}

void HoneycombResourceManager::AddContentProvider(const std::string& url,
                                            const std::string& content,
                                            const std::string& mime_type,
                                            int order,
                                            const std::string& identifier) {
  AddProvider(new ContentProvider(url, content, mime_type), order, identifier);
}

void HoneycombResourceManager::AddDirectoryProvider(const std::string& url_path,
                                              const std::string& directory_path,
                                              int order,
                                              const std::string& identifier) {
  AddProvider(new DirectoryProvider(url_path, directory_path), order,
              identifier);
}

void HoneycombResourceManager::AddArchiveProvider(const std::string& url_path,
                                            const std::string& archive_path,
                                            const std::string& password,
                                            int order,
                                            const std::string& identifier) {
  AddProvider(new ArchiveProvider(url_path, archive_path, password), order,
              identifier);
}

void HoneycombResourceManager::AddProvider(Provider* provider,
                                     int order,
                                     const std::string& identifier) {
  DCHECK(provider);
  if (!provider) {
    return;
  }

  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&HoneycombResourceManager::AddProvider, this,
                                       provider, order, identifier));
    return;
  }

  std::unique_ptr<ProviderEntry> new_entry(
      new ProviderEntry(provider, order, identifier));

  if (providers_.empty()) {
    providers_.push_back(new_entry.release());
    return;
  }

  // Insert before the first entry with a higher |order| value.
  ProviderEntryList::iterator it = providers_.begin();
  for (; it != providers_.end(); ++it) {
    if ((*it)->order_ > order) {
      break;
    }
  }

  providers_.insert(it, new_entry.release());
}

void HoneycombResourceManager::RemoveProviders(const std::string& identifier) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&HoneycombResourceManager::RemoveProviders,
                                       this, identifier));
    return;
  }

  if (providers_.empty()) {
    return;
  }

  ProviderEntryList::iterator it = providers_.begin();
  while (it != providers_.end()) {
    if ((*it)->identifier_ == identifier) {
      DeleteProvider(it, false);
    } else {
      ++it;
    }
  }
}

void HoneycombResourceManager::RemoveAllProviders() {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO,
                base::BindOnce(&HoneycombResourceManager::RemoveAllProviders, this));
    return;
  }

  if (providers_.empty()) {
    return;
  }

  ProviderEntryList::iterator it = providers_.begin();
  while (it != providers_.end()) {
    DeleteProvider(it, true);
  }
}

void HoneycombResourceManager::SetMimeTypeResolver(const MimeTypeResolver& resolver) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&HoneycombResourceManager::SetMimeTypeResolver,
                                       this, resolver));
    return;
  }

  if (!resolver.is_null()) {
    mime_type_resolver_ = resolver;
  } else {
    mime_type_resolver_ = base::BindRepeating(GetMimeType);
  }
}

void HoneycombResourceManager::SetUrlFilter(const UrlFilter& filter) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&HoneycombResourceManager::SetUrlFilter, this,
                                       filter));
    return;
  }

  if (!filter.is_null()) {
    url_filter_ = filter;
  } else {
    url_filter_ = base::BindRepeating(GetFilteredUrl);
  }
}

honey_return_value_t HoneycombResourceManager::OnBeforeResourceLoad(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombCallback> callback) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  // Find the first provider that is not pending deletion.
  ProviderEntryList::iterator current_entry_pos = providers_.begin();
  GetNextValidProvider(current_entry_pos);

  if (current_entry_pos == providers_.end()) {
    // No providers so continue the request immediately.
    return RV_CONTINUE;
  }

  std::unique_ptr<RequestState> state(new RequestState);

  if (!weak_ptr_factory_.get()) {
    // WeakPtrFactory instances need to be created and destroyed on the same
    // thread. This object performs most of its work on the IO thread and will
    // be destroyed on the IO thread so, now that we're on the IO thread,
    // properly initialize the WeakPtrFactory.
    weak_ptr_factory_.reset(new base::WeakPtrFactory<HoneycombResourceManager>(this));
  }

  state->manager_ = weak_ptr_factory_->GetWeakPtr();
  state->callback_ = callback;

  state->params_.url_ =
      GetUrlWithoutQueryOrFragment(url_filter_.Run(request->GetURL()));
  state->params_.browser_ = browser;
  state->params_.frame_ = frame;
  state->params_.request_ = request;
  state->params_.url_filter_ = url_filter_;
  state->params_.mime_type_resolver_ = mime_type_resolver_;

  state->current_entry_pos_ = current_entry_pos;

  // If the request is potentially handled we need to continue asynchronously.
  return SendRequest(std::move(state)) ? RV_CONTINUE_ASYNC : RV_CONTINUE;
}

HoneycombRefPtr<HoneycombResourceHandler> HoneycombResourceManager::GetResourceHandler(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  if (pending_handlers_.empty()) {
    return nullptr;
  }

  HoneycombRefPtr<HoneycombResourceHandler> handler;

  PendingHandlersMap::iterator it =
      pending_handlers_.find(request->GetIdentifier());
  if (it != pending_handlers_.end()) {
    handler = it->second;
    pending_handlers_.erase(it);
  }

  return handler;
}

// Send the request to providers in order until one potentially handles it or we
// run out of providers. Returns true if the request is potentially handled.
bool HoneycombResourceManager::SendRequest(std::unique_ptr<RequestState> state) {
  bool potentially_handled = false;

  do {
    // Should not be on the last provider entry.
    DCHECK(state->current_entry_pos_ != providers_.end());
    scoped_refptr<Request> request = new Request(std::move(state));

    // Give the provider an opportunity to handle the request.
    state = request->SendRequest();
    if (state.get()) {
      // The provider will not handle the request. Move to the next provider if
      // any.
      if (!IncrementProvider(state.get())) {
        StopRequest(std::move(state));
      }
    } else {
      potentially_handled = true;
    }
  } while (state.get());

  return potentially_handled;
}

void HoneycombResourceManager::ContinueRequest(
    std::unique_ptr<RequestState> state,
    HoneycombRefPtr<HoneycombResourceHandler> handler) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  if (handler.get()) {
    // The request has been handled. Associate the request ID with the handler.
    pending_handlers_.insert(
        std::make_pair(state->params_.request_->GetIdentifier(), handler));
    StopRequest(std::move(state));
  } else {
    // Move to the next provider if any.
    if (IncrementProvider(state.get())) {
      SendRequest(std::move(state));
    } else {
      StopRequest(std::move(state));
    }
  }
}

void HoneycombResourceManager::StopRequest(std::unique_ptr<RequestState> state) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  // Detach from the current provider.
  DetachRequestFromProvider(state.get());

  // Delete the state object and execute the callback.
  state.reset();
}

// Move state to the next provider if any and return true if there are more
// providers.
bool HoneycombResourceManager::IncrementProvider(RequestState* state) {
  // Identify the next provider.
  ProviderEntryList::iterator next_entry_pos = state->current_entry_pos_;
  GetNextValidProvider(++next_entry_pos);

  // Detach from the current provider.
  DetachRequestFromProvider(state);

  if (next_entry_pos != providers_.end()) {
    // Update the state to reference the new provider entry.
    state->current_entry_pos_ = next_entry_pos;
    return true;
  }

  return false;
}

// The new provider, if any, should be determined before calling this method.
void HoneycombResourceManager::DetachRequestFromProvider(RequestState* state) {
  if (state->current_entry_pos_ != providers_.end()) {
    // Remove the association from the current provider entry.
    ProviderEntryList::iterator current_entry_pos = state->current_entry_pos_;
    ProviderEntry* current_entry = *(current_entry_pos);
    current_entry->pending_requests_.erase(state->current_request_pos_);

    if (current_entry->deletion_pending_ &&
        current_entry->pending_requests_.empty()) {
      // Delete the current provider entry now.
      providers_.erase(current_entry_pos);
      delete current_entry;
    }

    // Set to the end for error checking purposes.
    state->current_entry_pos_ = providers_.end();
  }
}

// Move to the next provider that is not pending deletion.
void HoneycombResourceManager::GetNextValidProvider(
    ProviderEntryList::iterator& iterator) {
  while (iterator != providers_.end() && (*iterator)->deletion_pending_) {
    ++iterator;
  }
}

void HoneycombResourceManager::DeleteProvider(ProviderEntryList::iterator& iterator,
                                        bool stop) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  ProviderEntry* current_entry = *(iterator);

  if (current_entry->deletion_pending_) {
    return;
  }

  if (!current_entry->pending_requests_.empty()) {
    // Don't delete the provider entry until all pending requests have cleared.
    current_entry->deletion_pending_ = true;

    // Continue pending requests immediately.
    RequestList::iterator it = current_entry->pending_requests_.begin();
    for (; it != current_entry->pending_requests_.end(); ++it) {
      const scoped_refptr<Request>& request = *it;
      if (request->HasState()) {
        if (stop) {
          request->Stop();
        } else {
          request->Continue(nullptr);
        }
        current_entry->provider_->OnRequestCanceled(request);
      }
    }

    ++iterator;
  } else {
    // Delete the provider entry now.
    iterator = providers_.erase(iterator);
    delete current_entry;
  }
}
