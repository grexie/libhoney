// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/wrapper/honey_message_router.h"

#include <limits>
#include <map>
#include <set>

#include "include/base/honey_callback.h"
#include "include/honey_shared_process_message_builder.h"
#include "include/honey_task.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_helpers.h"
#include "libhoneycomb_dll/wrapper/honey_browser_info_map.h"

namespace {

// ID value reserved for internal use.
const int kReservedId = 0;

// Appended to the JS function name for related IPC messages.
const char kMessageSuffix[] = "Msg";

// JS object member argument names for honeyQuery.
const char kMemberRequest[] = "request";
const char kMemberOnSuccess[] = "onSuccess";
const char kMemberOnFailure[] = "onFailure";
const char kMemberPersistent[] = "persistent";

// Default error information when a query is canceled.
const int kCanceledErrorCode = -1;
const char kCanceledErrorMessage[] = "The query has been canceled";

// Value of 16KB is chosen as a result of performance tests available at
// http://tests/ipc_performance
constexpr size_t kResponseSizeThreshold = 16384;

// Validate configuration settings.
bool ValidateConfig(HoneycombMessageRouterConfig& config) {
  // Must specify function names.
  if (config.js_cancel_function.empty() || config.js_query_function.empty()) {
    return false;
  }

  return true;
}

struct MessageHeader {
  int context_id;
  int request_id;
  bool is_success;
};

struct ParsedMessage {
  int context_id;
  int request_id;
  bool success;
  int error_code;
  HoneycombString message;
};

size_t GetByteLength(const HoneycombString& value) {
  return value.size() * sizeof(HoneycombString::char_type);
}

size_t GetMessageSize(const HoneycombString& response) {
  return sizeof(MessageHeader) + GetByteLength(response);
}

void CopyResponseIntoMemory(void* memory, const HoneycombString& response) {
  const size_t bytes = response.size() * sizeof(HoneycombString::char_type);
  void* dest = static_cast<uint8_t*>(memory) + sizeof(MessageHeader);
  memcpy(dest, response.c_str(), bytes);
}

HoneycombString GetStringFromMemory(const void* memory, size_t size) {
  const size_t bytes = size - sizeof(MessageHeader);
  const size_t string_len = bytes / sizeof(HoneycombString::char_type);
  const HoneycombString::char_type* src =
      reinterpret_cast<const HoneycombString::char_type*>(
          static_cast<const uint8_t*>(memory) + sizeof(MessageHeader));
  constexpr bool copy = true;
  HoneycombString result;
  result.FromString(src, string_len, copy);
  return result;
}

HoneycombRefPtr<HoneycombProcessMessage> BuildListMessage(const std::string& message_name,
                                              int context_id,
                                              int request_id,
                                              const HoneycombString& response) {
  auto message = HoneycombProcessMessage::Create(message_name);
  HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
  args->SetInt(0, context_id);
  args->SetInt(1, request_id);
  args->SetBool(2, true);  // Indicates a success result.
  args->SetString(3, response);
  return message;
}

HoneycombRefPtr<HoneycombProcessMessage> BuildBinaryMessage(const std::string& message_name,
                                                int context_id,
                                                int request_id,
                                                const HoneycombString& response) {
  const size_t message_size = GetMessageSize(response);
  auto builder =
      HoneycombSharedProcessMessageBuilder::Create(message_name, message_size);
  if (!builder->IsValid()) {
    LOG(ERROR) << "Failed to allocate shared memory region of size "
               << message_size;
    // Use list message as a fallback
    return BuildListMessage(message_name, context_id, request_id, response);
  }

  auto header = static_cast<MessageHeader*>(builder->Memory());
  header->context_id = context_id;
  header->request_id = request_id;
  header->is_success = true;

  CopyResponseIntoMemory(builder->Memory(), response);

  return builder->Build();
}

HoneycombRefPtr<HoneycombProcessMessage> BuildMessage(size_t threshold,
                                          const std::string& message_name,
                                          int context_id,
                                          int request_id,
                                          const HoneycombString& response) {
  if (GetByteLength(response) <= threshold) {
    return BuildListMessage(message_name, context_id, request_id, response);
  } else {
    return BuildBinaryMessage(message_name, context_id, request_id, response);
  }
}

ParsedMessage ParseMessage(const HoneycombRefPtr<HoneycombProcessMessage>& message) {
  if (auto args = message->GetArgumentList()) {
    DCHECK_GT(args->GetSize(), 3U);

    const int context_id = args->GetInt(0);
    const int request_id = args->GetInt(1);
    const bool is_success = args->GetBool(2);

    if (is_success) {
      return ParsedMessage{context_id, request_id, is_success, 0,
                           args->GetString(3)};
    }

    DCHECK_EQ(args->GetSize(), 5U);
    return ParsedMessage{context_id, request_id, is_success, args->GetInt(3),
                         args->GetString(4)};
  }

  if (const auto region = message->GetSharedMemoryRegion()) {
    if (region->IsValid()) {
      DCHECK_GE(region->Size(), sizeof(MessageHeader));
      auto header = static_cast<const MessageHeader*>(region->Memory());
      DCHECK(header->is_success);
      return ParsedMessage{
          header->context_id, header->request_id, header->is_success, 0,
          GetStringFromMemory(region->Memory(), region->Size())};
    }
  }

  return ParsedMessage{};
}

/**
 * @brief A helper template for generating ID values.
 *
 * This class generates monotonically increasing ID values within the interval
 * [kReservedId + 1, numeric_limits<T>::max()].
 *
 * @tparam T The data type for the ID values.
 */
template <typename T>
class IdGenerator {
 public:
  IdGenerator() : next_id_(kReservedId) {}

  IdGenerator(const IdGenerator&) = delete;
  IdGenerator& operator=(const IdGenerator&) = delete;

  T GetNextId() {
    if (next_id_ == std::numeric_limits<T>::max()) {
      next_id_ = kReservedId;
    }
    return ++next_id_;
  }

 private:
  T next_id_;
};

// Browser-side router implementation.
class HoneycombMessageRouterBrowserSideImpl : public HoneycombMessageRouterBrowserSide {
 public:
  // Implementation of the Callback interface.
  class CallbackImpl : public HoneycombMessageRouterBrowserSide::Callback {
   public:
    CallbackImpl(HoneycombRefPtr<HoneycombMessageRouterBrowserSideImpl> router,
                 int browser_id,
                 int64_t query_id,
                 bool persistent)
        : router_(router),
          browser_id_(browser_id),
          query_id_(query_id),
          persistent_(persistent) {}

    CallbackImpl(const CallbackImpl&) = delete;
    CallbackImpl& operator=(const CallbackImpl&) = delete;

    virtual ~CallbackImpl() {
      // Hitting this DCHECK means that you didn't call Success or Failure
      // on the Callback after returning true from Handler::OnQuery. You must
      // call Failure to terminate persistent queries.
      DCHECK(!router_);
    }

    void Success(const HoneycombString& response) override {
      if (!HoneycombCurrentlyOn(TID_UI)) {
        // Must execute on the UI thread to access member variables.
        HoneycombPostTask(TID_UI,
                    base::BindOnce(&CallbackImpl::Success, this, response));
        return;
      }

      if (router_) {
        HoneycombPostTask(
            TID_UI,
            base::BindOnce(&HoneycombMessageRouterBrowserSideImpl::OnCallbackSuccess,
                           router_.get(), browser_id_, query_id_, response));

        if (!persistent_) {
          // Non-persistent callbacks are only good for a single use.
          router_ = nullptr;
        }
      }
    }

    void Failure(int error_code, const HoneycombString& error_message) override {
      if (!HoneycombCurrentlyOn(TID_UI)) {
        // Must execute on the UI thread to access member variables.
        HoneycombPostTask(TID_UI, base::BindOnce(&CallbackImpl::Failure, this,
                                           error_code, error_message));
        return;
      }

      if (router_) {
        HoneycombPostTask(
            TID_UI,
            base::BindOnce(&HoneycombMessageRouterBrowserSideImpl::OnCallbackFailure,
                           router_.get(), browser_id_, query_id_, error_code,
                           error_message));

        // Failure always invalidates the callback.
        router_ = nullptr;
      }
    }

    void Detach() {
      HONEYCOMB_REQUIRE_UI_THREAD();
      router_ = nullptr;
    }

   private:
    HoneycombRefPtr<HoneycombMessageRouterBrowserSideImpl> router_;
    const int browser_id_;
    const int64_t query_id_;
    const bool persistent_;

    IMPLEMENT_REFCOUNTING(CallbackImpl);
  };

  explicit HoneycombMessageRouterBrowserSideImpl(const HoneycombMessageRouterConfig& config)
      : config_(config),
        query_message_name_(config.js_query_function.ToString() +
                            kMessageSuffix),
        cancel_message_name_(config.js_cancel_function.ToString() +
                             kMessageSuffix) {}

  HoneycombMessageRouterBrowserSideImpl(const HoneycombMessageRouterBrowserSideImpl&) =
      delete;
  HoneycombMessageRouterBrowserSideImpl& operator=(
      const HoneycombMessageRouterBrowserSideImpl&) = delete;

  virtual ~HoneycombMessageRouterBrowserSideImpl() {
    // There should be no pending queries when the router is deleted.
    DCHECK(browser_query_info_map_.empty());
  }

  bool AddHandler(Handler* handler, bool first) override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    if (handler_set_.find(handler) == handler_set_.end()) {
      handler_set_.insert(first ? handler_set_.begin() : handler_set_.end(),
                          handler);
      return true;
    }
    return false;
  }

  bool RemoveHandler(Handler* handler) override {
    HONEYCOMB_REQUIRE_UI_THREAD();
    if (handler_set_.erase(handler) > 0) {
      CancelPendingFor(nullptr, handler, true);
      return true;
    }
    return false;
  }

  void CancelPending(HoneycombRefPtr<HoneycombBrowser> browser, Handler* handler) override {
    CancelPendingFor(browser, handler, true);
  }

  int GetPendingCount(HoneycombRefPtr<HoneycombBrowser> browser,
                      Handler* handler) override {
    HONEYCOMB_REQUIRE_UI_THREAD();

    if (browser_query_info_map_.empty()) {
      return 0;
    }

    if (handler) {
      // Need to iterate over each QueryInfo object to test the handler.
      class Visitor : public BrowserQueryInfoMap::Visitor {
       public:
        explicit Visitor(Handler* handler) : handler_(handler), count_(0) {}

        bool OnNextInfo(int browser_id,
                        InfoIdType info_id,
                        InfoObjectType info,
                        bool* remove) override {
          if (info->handler == handler_) {
            count_++;
          }
          return true;
        }

        int count() const { return count_; }

       private:
        Handler* handler_;
        int count_;
      };

      Visitor visitor(handler);

      if (browser.get()) {
        // Count queries associated with the specified browser.
        browser_query_info_map_.FindAll(browser->GetIdentifier(), &visitor);
      } else {
        // Count all queries for all browsers.
        browser_query_info_map_.FindAll(&visitor);
      }

      return visitor.count();
    } else if (browser.get()) {
      return static_cast<int>(
          browser_query_info_map_.size(browser->GetIdentifier()));
    }

    return static_cast<int>(browser_query_info_map_.size());
  }

  void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override {
    CancelPendingFor(browser, nullptr, false);
  }

  void OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser) override {
    CancelPendingFor(browser, nullptr, false);
  }

  void OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombRefPtr<HoneycombFrame> frame) override {
    if (frame->IsMain()) {
      CancelPendingFor(browser, nullptr, false);
    }
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    HONEYCOMB_REQUIRE_UI_THREAD();

    const std::string& message_name = message->GetName();
    if (message_name == query_message_name_) {
      HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
      DCHECK_EQ(args->GetSize(), 4U);

      const int context_id = args->GetInt(0);
      const int request_id = args->GetInt(1);
      const HoneycombString& request = args->GetString(2);
      const bool persistent = args->GetBool(3);

      if (handler_set_.empty()) {
        // No handlers so cancel the query.
        CancelUnhandledQuery(browser, frame, context_id, request_id);
        return true;
      }

      const int browser_id = browser->GetIdentifier();
      const int64_t query_id = query_id_generator_.GetNextId();

      HoneycombRefPtr<CallbackImpl> callback(
          new CallbackImpl(this, browser_id, query_id, persistent));

      // Make a copy of the handler list in case the user adds or removes a
      // handler while we're iterating.
      HandlerSet handler_set = handler_set_;

      bool handled = false;
      HandlerSet::const_iterator it_handler = handler_set.begin();
      for (; it_handler != handler_set.end(); ++it_handler) {
        handled = (*it_handler)
                      ->OnQuery(browser, frame, query_id, request, persistent,
                                callback.get());
        if (handled) {
          break;
        }
      }

      // If the query isn't handled nothing should be keeping a reference to
      // the callback.
      DCHECK(handled || callback->HasOneRef());

      if (handled) {
        // Persist the query information until the callback executes.
        // It's safe to do this here because the callback will execute
        // asynchronously.
        QueryInfo* info = new QueryInfo;
        info->browser = browser;
        info->frame = frame;
        info->context_id = context_id;
        info->request_id = request_id;
        info->persistent = persistent;
        info->callback = callback;
        info->handler = *(it_handler);
        browser_query_info_map_.Add(browser_id, query_id, info);
      } else {
        // Invalidate the callback.
        callback->Detach();

        // No one chose to handle the query so cancel it.
        CancelUnhandledQuery(browser, frame, context_id, request_id);
      }

      return true;
    } else if (message_name == cancel_message_name_) {
      HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
      DCHECK_EQ(args->GetSize(), 2U);

      const int browser_id = browser->GetIdentifier();
      const int context_id = args->GetInt(0);
      const int request_id = args->GetInt(1);

      CancelPendingRequest(browser_id, context_id, request_id);
      return true;
    }

    return false;
  }

 private:
  // Structure representing a pending query.
  struct QueryInfo {
    // Browser and frame originated the query.
    HoneycombRefPtr<HoneycombBrowser> browser;
    HoneycombRefPtr<HoneycombFrame> frame;

    // IDs that uniquely identify the query in the renderer process. These
    // values are opaque to the browser process but must be returned with the
    // response.
    int context_id;
    int request_id;

    // True if the query is persistent.
    bool persistent;

    // Callback associated with the query that must be detached when the query
    // is canceled.
    HoneycombRefPtr<CallbackImpl> callback;

    // Handler that should be notified if the query is automatically canceled.
    Handler* handler;
  };

  // Retrieve a QueryInfo object from the map based on the browser-side query
  // ID. If |always_remove| is true then the QueryInfo object will always be
  // removed from the map. Othewise, the QueryInfo object will only be removed
  // if the query is non-persistent. If |removed| is true the caller is
  // responsible for deleting the returned QueryInfo object.
  QueryInfo* GetQueryInfo(int browser_id,
                          int64_t query_id,
                          bool always_remove,
                          bool* removed) {
    class Visitor : public BrowserQueryInfoMap::Visitor {
     public:
      explicit Visitor(bool always_remove)
          : always_remove_(always_remove), removed_(false) {}

      bool OnNextInfo(int browser_id,
                      InfoIdType info_id,
                      InfoObjectType info,
                      bool* remove) override {
        *remove = removed_ = (always_remove_ || !info->persistent);
        return true;
      }

      bool removed() const { return removed_; }

     private:
      const bool always_remove_;
      bool removed_;
    };

    Visitor visitor(always_remove);
    QueryInfo* info =
        browser_query_info_map_.Find(browser_id, query_id, &visitor);
    if (info) {
      *removed = visitor.removed();
    }
    return info;
  }

  // Called by CallbackImpl on success.
  void OnCallbackSuccess(int browser_id,
                         int64_t query_id,
                         const HoneycombString& response) {
    HONEYCOMB_REQUIRE_UI_THREAD();

    bool removed;
    QueryInfo* info = GetQueryInfo(browser_id, query_id, false, &removed);
    if (info) {
      SendQuerySuccess(info, response);
      if (removed) {
        delete info;
      }
    }
  }

  // Called by CallbackImpl on failure.
  void OnCallbackFailure(int browser_id,
                         int64_t query_id,
                         int error_code,
                         const HoneycombString& error_message) {
    HONEYCOMB_REQUIRE_UI_THREAD();

    bool removed;
    QueryInfo* info = GetQueryInfo(browser_id, query_id, true, &removed);
    if (info) {
      SendQueryFailure(info, error_code, error_message);
      DCHECK(removed);
      delete info;
    }
  }

  void SendQuerySuccess(QueryInfo* info, const HoneycombString& response) {
    SendQuerySuccess(info->browser, info->frame, info->context_id,
                     info->request_id, response);
  }

  void SendQuerySuccess(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        int context_id,
                        int request_id,
                        const HoneycombString& response) {
    if (auto message =
            BuildMessage(config_.message_size_threshold, query_message_name_,
                         context_id, request_id, response)) {
      frame->SendProcessMessage(PID_RENDERER, message);
    }
  }

  void SendQueryFailure(QueryInfo* info,
                        int error_code,
                        const HoneycombString& error_message) {
    SendQueryFailure(info->browser, info->frame, info->context_id,
                     info->request_id, error_code, error_message);
  }

  void SendQueryFailure(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        int context_id,
                        int request_id,
                        int error_code,
                        const HoneycombString& error_message) {
    HoneycombRefPtr<HoneycombProcessMessage> message =
        HoneycombProcessMessage::Create(query_message_name_);
    HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
    args->SetInt(0, context_id);
    args->SetInt(1, request_id);
    args->SetBool(2, false);  // Indicates a failure result.
    args->SetInt(3, error_code);
    args->SetString(4, error_message);
    frame->SendProcessMessage(PID_RENDERER, message);
  }

  // Cancel a query that has not been sent to a handler.
  void CancelUnhandledQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                            HoneycombRefPtr<HoneycombFrame> frame,
                            int context_id,
                            int request_id) {
    SendQueryFailure(browser, frame, context_id, request_id, kCanceledErrorCode,
                     kCanceledErrorMessage);
  }

  // Cancel a query that has already been sent to a handler.
  void CancelQuery(int64_t query_id, QueryInfo* info, bool notify_renderer) {
    if (notify_renderer) {
      SendQueryFailure(info, kCanceledErrorCode, kCanceledErrorMessage);
    }

    info->handler->OnQueryCanceled(info->browser, info->frame, query_id);

    // Invalidate the callback.
    info->callback->Detach();
  }

  // Cancel all pending queries associated with either |browser| or |handler|.
  // If both |browser| and |handler| are NULL all pending queries will be
  // canceled. Set |notify_renderer| to true if the renderer should be notified.
  void CancelPendingFor(HoneycombRefPtr<HoneycombBrowser> browser,
                        Handler* handler,
                        bool notify_renderer) {
    if (!HoneycombCurrentlyOn(TID_UI)) {
      // Must execute on the UI thread.
      HoneycombPostTask(
          TID_UI,
          base::BindOnce(&HoneycombMessageRouterBrowserSideImpl::CancelPendingFor,
                         this, browser, handler, notify_renderer));
      return;
    }

    if (browser_query_info_map_.empty()) {
      return;
    }

    class Visitor : public BrowserQueryInfoMap::Visitor {
     public:
      Visitor(HoneycombMessageRouterBrowserSideImpl* router,
              Handler* handler,
              bool notify_renderer)
          : router_(router),
            handler_(handler),
            notify_renderer_(notify_renderer) {}

      bool OnNextInfo(int browser_id,
                      InfoIdType info_id,
                      InfoObjectType info,
                      bool* remove) override {
        if (!handler_ || info->handler == handler_) {
          *remove = true;
          router_->CancelQuery(info_id, info, notify_renderer_);
          delete info;
        }
        return true;
      }

     private:
      HoneycombMessageRouterBrowserSideImpl* router_;
      Handler* handler_;
      const bool notify_renderer_;
    };

    Visitor visitor(this, handler, notify_renderer);

    if (browser.get()) {
      // Cancel all queries associated with the specified browser.
      browser_query_info_map_.FindAll(browser->GetIdentifier(), &visitor);
    } else {
      // Cancel all queries for all browsers.
      browser_query_info_map_.FindAll(&visitor);
    }
  }

  // Cancel a query based on the renderer-side IDs. If |request_id| is
  // kReservedId all requests associated with |context_id| will be canceled.
  void CancelPendingRequest(int browser_id, int context_id, int request_id) {
    class Visitor : public BrowserQueryInfoMap::Visitor {
     public:
      Visitor(HoneycombMessageRouterBrowserSideImpl* router,
              int context_id,
              int request_id)
          : router_(router), context_id_(context_id), request_id_(request_id) {}

      bool OnNextInfo(int browser_id,
                      InfoIdType info_id,
                      InfoObjectType info,
                      bool* remove) override {
        if (info->context_id == context_id_ &&
            (request_id_ == kReservedId || info->request_id == request_id_)) {
          *remove = true;
          router_->CancelQuery(info_id, info, false);
          delete info;

          // Stop iterating if only canceling a single request.
          return (request_id_ == kReservedId);
        }
        return true;
      }

     private:
      HoneycombMessageRouterBrowserSideImpl* router_;
      const int context_id_;
      const int request_id_;
    };

    Visitor visitor(this, context_id, request_id);
    browser_query_info_map_.FindAll(browser_id, &visitor);
  }

  const HoneycombMessageRouterConfig config_;
  const std::string query_message_name_;
  const std::string cancel_message_name_;

  IdGenerator<int64_t> query_id_generator_;

  // Set of currently registered handlers. An entry is added when a handler is
  // registered and removed when a handler is unregistered.
  using HandlerSet = std::set<Handler*>;
  HandlerSet handler_set_;

  // Map of query ID to QueryInfo instance. An entry is added when a Handler
  // indicates that it will handle the query and removed when either the query
  // is completed via the Callback, the query is explicitly canceled from the
  // renderer process, or the associated context is (or will be) released.
  using BrowserQueryInfoMap = HoneycombBrowserInfoMap<int64_t, QueryInfo*>;
  BrowserQueryInfoMap browser_query_info_map_;
};

// Renderer-side router implementation.
class HoneycombMessageRouterRendererSideImpl : public HoneycombMessageRouterRendererSide {
 public:
  class V8HandlerImpl : public HoneycombV8Handler {
   public:
    V8HandlerImpl(HoneycombRefPtr<HoneycombMessageRouterRendererSideImpl> router,
                  const HoneycombMessageRouterConfig& config)
        : router_(router), config_(config), context_id_(kReservedId) {}

    V8HandlerImpl(const V8HandlerImpl&) = delete;
    V8HandlerImpl& operator=(const V8HandlerImpl&) = delete;

    bool Execute(const HoneycombString& name,
                 HoneycombRefPtr<HoneycombV8Value> object,
                 const HoneycombV8ValueList& arguments,
                 HoneycombRefPtr<HoneycombV8Value>& retval,
                 HoneycombString& exception) override {
      if (name == config_.js_query_function) {
        if (arguments.size() != 1 || !arguments[0]->IsObject()) {
          exception = "Invalid arguments; expecting a single object";
          return true;
        }

        HoneycombRefPtr<HoneycombV8Value> arg = arguments[0];

        HoneycombRefPtr<HoneycombV8Value> requestVal = arg->GetValue(kMemberRequest);
        if (!requestVal.get() || !requestVal->IsString()) {
          exception = "Invalid arguments; object member '" +
                      std::string(kMemberRequest) +
                      "' is required and must have type string";
          return true;
        }

        HoneycombRefPtr<HoneycombV8Value> successVal = nullptr;
        if (arg->HasValue(kMemberOnSuccess)) {
          successVal = arg->GetValue(kMemberOnSuccess);
          if (!successVal->IsFunction()) {
            exception = "Invalid arguments; object member '" +
                        std::string(kMemberOnSuccess) +
                        "' must have type function";
            return true;
          }
        }

        HoneycombRefPtr<HoneycombV8Value> failureVal = nullptr;
        if (arg->HasValue(kMemberOnFailure)) {
          failureVal = arg->GetValue(kMemberOnFailure);
          if (!failureVal->IsFunction()) {
            exception = "Invalid arguments; object member '" +
                        std::string(kMemberOnFailure) +
                        "' must have type function";
            return true;
          }
        }

        HoneycombRefPtr<HoneycombV8Value> persistentVal = nullptr;
        if (arg->HasValue(kMemberPersistent)) {
          persistentVal = arg->GetValue(kMemberPersistent);
          if (!persistentVal->IsBool()) {
            exception = "Invalid arguments; object member '" +
                        std::string(kMemberPersistent) +
                        "' must have type boolean";
            return true;
          }
        }

        HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
        const int context_id = GetIDForContext(context);
        const bool persistent =
            (persistentVal.get() && persistentVal->GetBoolValue());

        const int request_id = router_->SendQuery(
            context->GetBrowser(), context->GetFrame(), context_id,
            requestVal->GetStringValue(), persistent, successVal, failureVal);
        retval = HoneycombV8Value::CreateInt(request_id);
        return true;
      } else if (name == config_.js_cancel_function) {
        if (arguments.size() != 1 || !arguments[0]->IsInt()) {
          exception = "Invalid arguments; expecting a single integer";
          return true;
        }

        bool result = false;
        const int request_id = arguments[0]->GetIntValue();
        if (request_id != kReservedId) {
          HoneycombRefPtr<HoneycombV8Context> context = HoneycombV8Context::GetCurrentContext();
          const int context_id = GetIDForContext(context);
          result =
              router_->SendCancel(context->GetBrowser(), context->GetFrame(),
                                  context_id, request_id);
        }
        retval = HoneycombV8Value::CreateBool(result);
        return true;
      }

      return false;
    }

   private:
    // Don't create the context ID until it's actually needed.
    int GetIDForContext(HoneycombRefPtr<HoneycombV8Context> context) {
      if (context_id_ == kReservedId) {
        context_id_ = router_->CreateIDForContext(context);
      }
      return context_id_;
    }

    HoneycombRefPtr<HoneycombMessageRouterRendererSideImpl> router_;
    const HoneycombMessageRouterConfig config_;
    int context_id_;

    IMPLEMENT_REFCOUNTING(V8HandlerImpl);
  };

  explicit HoneycombMessageRouterRendererSideImpl(
      const HoneycombMessageRouterConfig& config)
      : config_(config),
        query_message_name_(config.js_query_function.ToString() +
                            kMessageSuffix),
        cancel_message_name_(config.js_cancel_function.ToString() +
                             kMessageSuffix) {}

  HoneycombMessageRouterRendererSideImpl(const HoneycombMessageRouterRendererSideImpl&) =
      delete;
  HoneycombMessageRouterRendererSideImpl& operator=(
      const HoneycombMessageRouterRendererSideImpl&) = delete;

  int GetPendingCount(HoneycombRefPtr<HoneycombBrowser> browser,
                      HoneycombRefPtr<HoneycombV8Context> context) override {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    if (browser_request_info_map_.empty()) {
      return 0;
    }

    if (context.get()) {
      const int context_id = GetIDForContext(context, false);
      if (context_id == kReservedId) {
        return 0;  // Nothing associated with the specified context.
      }

      // Need to iterate over each RequestInfo object to test the context.
      class Visitor : public BrowserRequestInfoMap::Visitor {
       public:
        explicit Visitor(int context_id) : context_id_(context_id), count_(0) {}

        bool OnNextInfo(int browser_id,
                        InfoIdType info_id,
                        InfoObjectType info,
                        bool* remove) override {
          if (info_id.first == context_id_) {
            count_++;
          }
          return true;
        }

        int count() const { return count_; }

       private:
        int context_id_;
        int count_;
      };

      Visitor visitor(context_id);

      if (browser.get()) {
        // Count requests associated with the specified browser.
        browser_request_info_map_.FindAll(browser->GetIdentifier(), &visitor);
      } else {
        // Count all requests for all browsers.
        browser_request_info_map_.FindAll(&visitor);
      }

      return visitor.count();
    } else if (browser.get()) {
      return static_cast<int>(
          browser_request_info_map_.size(browser->GetIdentifier()));
    }

    return static_cast<int>(browser_request_info_map_.size());
  }

  void OnContextCreated(HoneycombRefPtr<HoneycombBrowser> browser,
                        HoneycombRefPtr<HoneycombFrame> frame,
                        HoneycombRefPtr<HoneycombV8Context> context) override {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    // Register function handlers with the 'window' object.
    HoneycombRefPtr<HoneycombV8Value> window = context->GetGlobal();

    HoneycombRefPtr<V8HandlerImpl> handler = new V8HandlerImpl(this, config_);
    HoneycombV8Value::PropertyAttribute attributes =
        static_cast<HoneycombV8Value::PropertyAttribute>(
            V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM |
            V8_PROPERTY_ATTRIBUTE_DONTDELETE);

    // Add the query function.
    HoneycombRefPtr<HoneycombV8Value> query_func =
        HoneycombV8Value::CreateFunction(config_.js_query_function, handler.get());
    window->SetValue(config_.js_query_function, query_func, attributes);

    // Add the cancel function.
    HoneycombRefPtr<HoneycombV8Value> cancel_func =
        HoneycombV8Value::CreateFunction(config_.js_cancel_function, handler.get());
    window->SetValue(config_.js_cancel_function, cancel_func, attributes);
  }

  void OnContextReleased(HoneycombRefPtr<HoneycombBrowser> browser,
                         HoneycombRefPtr<HoneycombFrame> frame,
                         HoneycombRefPtr<HoneycombV8Context> context) override {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    // Get the context ID and remove the context from the map.
    const int context_id = GetIDForContext(context, true);
    if (context_id != kReservedId) {
      // Cancel all pending requests for the context.
      SendCancel(browser, frame, context_id, kReservedId);
    }
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    const std::string& message_name = message->GetName();
    if (message_name == query_message_name_) {
      auto content = ParseMessage(message);
      if (content.success) {
        HoneycombPostTask(
            TID_RENDERER,
            base::BindOnce(
                &HoneycombMessageRouterRendererSideImpl::ExecuteSuccessCallback, this,
                browser->GetIdentifier(), content.context_id,
                content.request_id, content.message));
      } else {
        HoneycombPostTask(
            TID_RENDERER,
            base::BindOnce(
                &HoneycombMessageRouterRendererSideImpl::ExecuteFailureCallback, this,
                browser->GetIdentifier(), content.context_id,
                content.request_id, content.error_code, content.message));
      }

      return true;
    }

    return false;
  }

 private:
  // Structure representing a pending request.
  struct RequestInfo {
    // True if the request is persistent.
    bool persistent;

    // Success callback function. May be NULL.
    HoneycombRefPtr<HoneycombV8Value> success_callback;

    // Failure callback function. May be NULL.
    HoneycombRefPtr<HoneycombV8Value> failure_callback;
  };

  // Retrieve a RequestInfo object from the map based on the renderer-side
  // IDs. If |always_remove| is true then the RequestInfo object will always be
  // removed from the map. Othewise, the RequestInfo object will only be removed
  // if the query is non-persistent. If |removed| is true the caller is
  // responsible for deleting the returned QueryInfo object.
  RequestInfo* GetRequestInfo(int browser_id,
                              int context_id,
                              int request_id,
                              bool always_remove,
                              bool* removed) {
    class Visitor : public BrowserRequestInfoMap::Visitor {
     public:
      explicit Visitor(bool always_remove)
          : always_remove_(always_remove), removed_(false) {}

      bool OnNextInfo(int browser_id,
                      InfoIdType info_id,
                      InfoObjectType info,
                      bool* remove) override {
        *remove = removed_ = (always_remove_ || !info->persistent);
        return true;
      }

      bool removed() const { return removed_; }

     private:
      const bool always_remove_;
      bool removed_;
    };

    Visitor visitor(always_remove);
    RequestInfo* info = browser_request_info_map_.Find(
        browser_id, std::make_pair(context_id, request_id), &visitor);
    if (info) {
      *removed = visitor.removed();
    }
    return info;
  }

  // Returns the new request ID.
  int SendQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                HoneycombRefPtr<HoneycombFrame> frame,
                int context_id,
                const HoneycombString& request,
                bool persistent,
                HoneycombRefPtr<HoneycombV8Value> success_callback,
                HoneycombRefPtr<HoneycombV8Value> failure_callback) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    const int request_id = request_id_generator_.GetNextId();

    auto* info =
        new RequestInfo{persistent, success_callback, failure_callback};

    browser_request_info_map_.Add(browser->GetIdentifier(),
                                  std::make_pair(context_id, request_id), info);

    HoneycombRefPtr<HoneycombProcessMessage> message =
        HoneycombProcessMessage::Create(query_message_name_);

    HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
    args->SetInt(0, context_id);
    args->SetInt(1, request_id);
    args->SetString(2, request);
    args->SetBool(3, persistent);

    frame->SendProcessMessage(PID_BROWSER, message);

    return request_id;
  }

  // If |request_id| is kReservedId all requests associated with |context_id|
  // will be canceled, otherwise only the specified |request_id| will be
  // canceled. Returns true if any request was canceled.
  bool SendCancel(HoneycombRefPtr<HoneycombBrowser> browser,
                  HoneycombRefPtr<HoneycombFrame> frame,
                  int context_id,
                  int request_id) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    const int browser_id = browser->GetIdentifier();

    int cancel_count = 0;
    if (request_id != kReservedId) {
      // Cancel a single request.
      bool removed;
      RequestInfo* info =
          GetRequestInfo(browser_id, context_id, request_id, true, &removed);
      if (info) {
        DCHECK(removed);
        delete info;
        cancel_count = 1;
      }
    } else {
      // Cancel all requests with the specified context ID.
      class Visitor : public BrowserRequestInfoMap::Visitor {
       public:
        explicit Visitor(int context_id)
            : context_id_(context_id), cancel_count_(0) {}

        bool OnNextInfo(int browser_id,
                        InfoIdType info_id,
                        InfoObjectType info,
                        bool* remove) override {
          if (info_id.first == context_id_) {
            *remove = true;
            delete info;
            cancel_count_++;
          }
          return true;
        }

        int cancel_count() const { return cancel_count_; }

       private:
        const int context_id_;
        int cancel_count_;
      };

      Visitor visitor(context_id);
      browser_request_info_map_.FindAll(browser_id, &visitor);
      cancel_count = visitor.cancel_count();
    }

    if (cancel_count > 0) {
      HoneycombRefPtr<HoneycombProcessMessage> message =
          HoneycombProcessMessage::Create(cancel_message_name_);

      HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
      args->SetInt(0, context_id);
      args->SetInt(1, request_id);

      frame->SendProcessMessage(PID_BROWSER, message);
      return true;
    }

    return false;
  }

  // Execute the onSuccess JavaScript callback.
  void ExecuteSuccessCallback(int browser_id,
                              int context_id,
                              int request_id,
                              const HoneycombString& response) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    bool removed;
    RequestInfo* info =
        GetRequestInfo(browser_id, context_id, request_id, false, &removed);
    if (!info) {
      return;
    }

    HoneycombRefPtr<HoneycombV8Context> context = GetContextByID(context_id);
    if (context && info->success_callback) {
      HoneycombV8ValueList args;
      args.push_back(HoneycombV8Value::CreateString(response));
      info->success_callback->ExecuteFunctionWithContext(context, nullptr,
                                                         args);
    }

    if (removed) {
      delete info;
    }
  }

  // Execute the onFailure JavaScript callback.
  void ExecuteFailureCallback(int browser_id,
                              int context_id,
                              int request_id,
                              int error_code,
                              const HoneycombString& error_message) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    bool removed;
    RequestInfo* info =
        GetRequestInfo(browser_id, context_id, request_id, true, &removed);
    if (!info) {
      return;
    }

    HoneycombRefPtr<HoneycombV8Context> context = GetContextByID(context_id);
    if (context && info->failure_callback) {
      HoneycombV8ValueList args;
      args.push_back(HoneycombV8Value::CreateInt(error_code));
      args.push_back(HoneycombV8Value::CreateString(error_message));
      info->failure_callback->ExecuteFunctionWithContext(context, nullptr,
                                                         args);
    }

    DCHECK(removed);
    delete info;
  }

  int CreateIDForContext(HoneycombRefPtr<HoneycombV8Context> context) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    // The context should not already have an associated ID.
    DCHECK_EQ(GetIDForContext(context, false), kReservedId);

    const int context_id = context_id_generator_.GetNextId();
    context_map_.insert(std::make_pair(context_id, context));
    return context_id;
  }

  // Retrieves the existing ID value associated with the specified |context|.
  // If |remove| is true the context will also be removed from the map.
  int GetIDForContext(HoneycombRefPtr<HoneycombV8Context> context, bool remove) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    ContextMap::iterator it = context_map_.begin();
    for (; it != context_map_.end(); ++it) {
      if (it->second->IsSame(context)) {
        int context_id = it->first;
        if (remove) {
          context_map_.erase(it);
        }
        return context_id;
      }
    }

    return kReservedId;
  }

  HoneycombRefPtr<HoneycombV8Context> GetContextByID(int context_id) {
    HONEYCOMB_REQUIRE_RENDERER_THREAD();

    ContextMap::const_iterator it = context_map_.find(context_id);
    if (it != context_map_.end()) {
      return it->second;
    }
    return nullptr;
  }

  const HoneycombMessageRouterConfig config_;
  const std::string query_message_name_;
  const std::string cancel_message_name_;

  IdGenerator<int> context_id_generator_;
  IdGenerator<int> request_id_generator_;

  // Map of (request ID, context ID) to RequestInfo for pending queries. An
  // entry is added when a request is initiated via the bound function and
  // removed when either the request completes, is canceled via the bound
  // function, or the associated context is released.
  using BrowserRequestInfoMap =
      HoneycombBrowserInfoMap<std::pair<int, int>, RequestInfo*>;
  BrowserRequestInfoMap browser_request_info_map_;

  // Map of context ID to HoneycombV8Context for existing contexts. An entry is added
  // when a bound function is executed for the first time in the context and
  // removed when the context is released.
  using ContextMap = std::map<int, HoneycombRefPtr<HoneycombV8Context>>;
  ContextMap context_map_;
};

}  // namespace

HoneycombMessageRouterConfig::HoneycombMessageRouterConfig()
    : js_query_function("honeyQuery"),
      js_cancel_function("honeyQueryCancel"),
      message_size_threshold(kResponseSizeThreshold) {}

// static
HoneycombRefPtr<HoneycombMessageRouterBrowserSide> HoneycombMessageRouterBrowserSide::Create(
    const HoneycombMessageRouterConfig& config) {
  HoneycombMessageRouterConfig validated_config = config;
  if (!ValidateConfig(validated_config)) {
    return nullptr;
  }
  return new HoneycombMessageRouterBrowserSideImpl(validated_config);
}

// static
HoneycombRefPtr<HoneycombMessageRouterRendererSide> HoneycombMessageRouterRendererSide::Create(
    const HoneycombMessageRouterConfig& config) {
  HoneycombMessageRouterConfig validated_config = config;
  if (!ValidateConfig(validated_config)) {
    return nullptr;
  }
  return new HoneycombMessageRouterRendererSideImpl(validated_config);
}
