// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "libhoneycomb/browser/frame_host_impl.h"

#include "include/honey_request.h"
#include "include/honey_stream.h"
#include "include/honey_v8.h"
#include "include/test/honey_test_helpers.h"
#include "libhoneycomb/browser/browser_host_base.h"
#include "libhoneycomb/browser/net_service/browser_urlrequest_impl.h"
#include "libhoneycomb/common/frame_util.h"
#include "libhoneycomb/common/net/url_util.h"
#include "libhoneycomb/common/process_message_impl.h"
#include "libhoneycomb/common/process_message_smr_impl.h"
#include "libhoneycomb/common/request_impl.h"
#include "libhoneycomb/common/string_util.h"
#include "libhoneycomb/common/task_runner_impl.h"

#include "content/browser/renderer_host/frame_tree_node.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "services/service_manager/public/cpp/interface_provider.h"

namespace {

void StringVisitCallback(HoneycombRefPtr<HoneycombStringVisitor> visitor,
                         base::ReadOnlySharedMemoryRegion response) {
  string_util::ExecuteWithScopedHoneycombString(
      std::move(response),
      base::BindOnce([](HoneycombRefPtr<HoneycombStringVisitor> visitor,
                        const HoneycombString& str) { visitor->Visit(str); },
                     visitor));
}

void ViewTextCallback(HoneycombRefPtr<HoneycombFrameHostImpl> frame,
                      base::ReadOnlySharedMemoryRegion response) {
  if (auto browser = frame->GetBrowser()) {
    string_util::ExecuteWithScopedHoneycombString(
        std::move(response),
        base::BindOnce(
            [](HoneycombRefPtr<HoneycombBrowser> browser, const HoneycombString& str) {
              static_cast<HoneycombBrowserHostBase*>(browser.get())->ViewText(str);
            },
            browser));
  }
}

using HoneycombFrameHostImplCommand = void (HoneycombFrameHostImpl::*)();
using WebContentsCommand = void (content::WebContents::*)();

void ExecWebContentsCommand(HoneycombFrameHostImpl* fh,
                            HoneycombFrameHostImplCommand fh_func,
                            WebContentsCommand wc_func,
                            const std::string& command) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(fh_func, fh));
    return;
  }
  auto rfh = fh->GetRenderFrameHost();
  if (rfh) {
    auto web_contents = content::WebContents::FromRenderFrameHost(rfh);
    if (web_contents) {
      std::invoke(wc_func, web_contents);
      return;
    }
  }
  fh->SendCommand(command);
}

#define EXEC_WEBCONTENTS_COMMAND(name)                  \
  ExecWebContentsCommand(this, &HoneycombFrameHostImpl::name, \
                         &content::WebContents::name, #name);

}  // namespace

HoneycombFrameHostImpl::HoneycombFrameHostImpl(scoped_refptr<HoneycombBrowserInfo> browser_info,
                                   int64_t parent_frame_id)
    : is_main_frame_(false),
      frame_id_(kInvalidFrameId),
      browser_info_(browser_info),
      is_focused_(is_main_frame_),  // The main frame always starts focused.
      parent_frame_id_(parent_frame_id) {
#if DCHECK_IS_ON()
  DCHECK(browser_info_);
  if (is_main_frame_) {
    DCHECK_EQ(parent_frame_id_, kInvalidFrameId);
  } else {
    DCHECK_GT(parent_frame_id_, 0);
  }
#endif
}

HoneycombFrameHostImpl::HoneycombFrameHostImpl(scoped_refptr<HoneycombBrowserInfo> browser_info,
                                   content::RenderFrameHost* render_frame_host)
    : is_main_frame_(render_frame_host->GetParent() == nullptr),
      frame_id_(frame_util::MakeFrameId(render_frame_host->GetGlobalId())),
      browser_info_(browser_info),
      is_focused_(is_main_frame_),  // The main frame always starts focused.
      url_(render_frame_host->GetLastCommittedURL().spec()),
      name_(render_frame_host->GetFrameName()),
      parent_frame_id_(
          is_main_frame_ ? kInvalidFrameId
                         : frame_util::MakeFrameId(
                               render_frame_host->GetParent()->GetGlobalId())),
      render_frame_host_(render_frame_host) {
  DCHECK(browser_info_);
}

HoneycombFrameHostImpl::~HoneycombFrameHostImpl() {
  // Should have been Detached if not temporary.
  DCHECK(is_temporary() || !browser_info_);
  DCHECK(!render_frame_host_);
}

bool HoneycombFrameHostImpl::IsValid() {
  return !!GetBrowserHostBase();
}

void HoneycombFrameHostImpl::Undo() {
  EXEC_WEBCONTENTS_COMMAND(Undo);
}

void HoneycombFrameHostImpl::Redo() {
  EXEC_WEBCONTENTS_COMMAND(Redo);
}

void HoneycombFrameHostImpl::Cut() {
  EXEC_WEBCONTENTS_COMMAND(Cut);
}

void HoneycombFrameHostImpl::Copy() {
  EXEC_WEBCONTENTS_COMMAND(Copy);
}

void HoneycombFrameHostImpl::Paste() {
  EXEC_WEBCONTENTS_COMMAND(Paste);
}

void HoneycombFrameHostImpl::Delete() {
  EXEC_WEBCONTENTS_COMMAND(Delete);
}

void HoneycombFrameHostImpl::SelectAll() {
  EXEC_WEBCONTENTS_COMMAND(SelectAll);
}

void HoneycombFrameHostImpl::ViewSource() {
  SendCommandWithResponse(
      "GetSource",
      base::BindOnce(&ViewTextCallback, HoneycombRefPtr<HoneycombFrameHostImpl>(this)));
}

void HoneycombFrameHostImpl::GetSource(HoneycombRefPtr<HoneycombStringVisitor> visitor) {
  SendCommandWithResponse("GetSource",
                          base::BindOnce(&StringVisitCallback, visitor));
}

void HoneycombFrameHostImpl::GetText(HoneycombRefPtr<HoneycombStringVisitor> visitor) {
  SendCommandWithResponse("GetText",
                          base::BindOnce(&StringVisitCallback, visitor));
}

void HoneycombFrameHostImpl::LoadRequest(HoneycombRefPtr<HoneycombRequest> request) {
  auto params = honey::mojom::RequestParams::New();
  static_cast<HoneycombRequestImpl*>(request.get())->Get(params);
  LoadRequest(std::move(params));
}

void HoneycombFrameHostImpl::LoadURL(const HoneycombString& url) {
  LoadURLWithExtras(url, content::Referrer(), kPageTransitionExplicit,
                    std::string());
}

void HoneycombFrameHostImpl::ExecuteJavaScript(const HoneycombString& jsCode,
                                         const HoneycombString& scriptUrl,
                                         int startLine) {
  SendJavaScript(jsCode, scriptUrl, startLine);
}

bool HoneycombFrameHostImpl::IsMain() {
  return is_main_frame_;
}

bool HoneycombFrameHostImpl::IsFocused() {
  base::AutoLock lock_scope(state_lock_);
  return is_focused_;
}

HoneycombString HoneycombFrameHostImpl::GetName() {
  base::AutoLock lock_scope(state_lock_);
  return name_;
}

int64_t HoneycombFrameHostImpl::GetIdentifier() {
  base::AutoLock lock_scope(state_lock_);
  return frame_id_;
}

HoneycombRefPtr<HoneycombFrame> HoneycombFrameHostImpl::GetParent() {
  int64_t parent_frame_id;

  {
    base::AutoLock lock_scope(state_lock_);
    if (is_main_frame_ || parent_frame_id_ == kInvalidFrameId) {
      return nullptr;
    }
    parent_frame_id = parent_frame_id_;
  }

  auto browser = GetBrowserHostBase();
  if (browser) {
    return browser->GetFrame(parent_frame_id);
  }

  return nullptr;
}

HoneycombString HoneycombFrameHostImpl::GetURL() {
  base::AutoLock lock_scope(state_lock_);
  return url_;
}

HoneycombRefPtr<HoneycombBrowser> HoneycombFrameHostImpl::GetBrowser() {
  return GetBrowserHostBase().get();
}

HoneycombRefPtr<HoneycombV8Context> HoneycombFrameHostImpl::GetV8Context() {
  DCHECK(false) << "GetV8Context cannot be called from the browser process";
  return nullptr;
}

void HoneycombFrameHostImpl::VisitDOM(HoneycombRefPtr<HoneycombDOMVisitor> visitor) {
  DCHECK(false) << "VisitDOM cannot be called from the browser process";
}

HoneycombRefPtr<HoneycombURLRequest> HoneycombFrameHostImpl::CreateURLRequest(
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombURLRequestClient> client) {
  if (!request || !client) {
    return nullptr;
  }

  if (!HoneycombTaskRunnerImpl::GetCurrentTaskRunner()) {
    DCHECK(false) << "called on invalid thread";
    return nullptr;
  }

  auto browser = GetBrowserHostBase();
  if (!browser) {
    return nullptr;
  }

  auto request_context = browser->request_context();

  HoneycombRefPtr<HoneycombBrowserURLRequest> impl =
      new HoneycombBrowserURLRequest(this, request, client, request_context);
  if (impl->Start()) {
    return impl.get();
  }
  return nullptr;
}

void HoneycombFrameHostImpl::SendProcessMessage(
    HoneycombProcessId target_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  DCHECK_EQ(PID_RENDERER, target_process);
  DCHECK(message && message->IsValid());
  if (!message || !message->IsValid()) {
    return;
  }

  if (message->GetArgumentList() != nullptr) {
    // Invalidate the message object immediately by taking the argument list.
    auto argument_list =
        static_cast<HoneycombProcessMessageImpl*>(message.get())->TakeArgumentList();
    SendToRenderFrame(
        __FUNCTION__,
        base::BindOnce(
            [](const HoneycombString& name, base::Value::List argument_list,
               const RenderFrameType& render_frame) {
              render_frame->SendMessage(name, std::move(argument_list));
            },
            message->GetName(), std::move(argument_list)));
  } else {
    auto region =
        static_cast<HoneycombProcessMessageSMRImpl*>(message.get())->TakeRegion();
    SendToRenderFrame(
        __FUNCTION__,
        base::BindOnce(
            [](const HoneycombString& name, base::WritableSharedMemoryRegion region,
               const RenderFrameType& render_frame) {
              render_frame->SendSharedMemoryRegion(name, std::move(region));
            },
            message->GetName(), std::move(region)));
  }
}

void HoneycombFrameHostImpl::SetFocused(bool focused) {
  base::AutoLock lock_scope(state_lock_);
  is_focused_ = focused;
}

void HoneycombFrameHostImpl::RefreshAttributes() {
  HONEYCOMB_REQUIRE_UIT();

  base::AutoLock lock_scope(state_lock_);
  if (!render_frame_host_) {
    return;
  }
  url_ = render_frame_host_->GetLastCommittedURL().spec();

  // Use the assigned name if it is non-empty. This represents the name property
  // on the frame DOM element. If the assigned name is empty, revert to the
  // internal unique name. This matches the logic in render_frame_util::GetName.
  name_ = render_frame_host_->GetFrameName();
  if (name_.empty()) {
    const auto node = content::FrameTreeNode::GloballyFindByID(
        render_frame_host_->GetFrameTreeNodeId());
    if (node) {
      name_ = node->unique_name();
    }
  }

  if (!is_main_frame_) {
    parent_frame_id_ =
        frame_util::MakeFrameId(render_frame_host_->GetParent()->GetGlobalId());
  }
}

void HoneycombFrameHostImpl::NotifyMoveOrResizeStarted() {
  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce([](const RenderFrameType& render_frame) {
                      render_frame->MoveOrResizeStarted();
                    }));
}

void HoneycombFrameHostImpl::LoadRequest(honey::mojom::RequestParamsPtr params) {
  if (!url_util::FixupGURL(params->url)) {
    return;
  }

  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce(
                        [](honey::mojom::RequestParamsPtr params,
                           const RenderFrameType& render_frame) {
                          render_frame->LoadRequest(std::move(params));
                        },
                        std::move(params)));

  auto browser = GetBrowserHostBase();
  if (browser) {
    browser->OnSetFocus(FOCUS_SOURCE_NAVIGATION);
  }
}

void HoneycombFrameHostImpl::LoadURLWithExtras(const std::string& url,
                                         const content::Referrer& referrer,
                                         ui::PageTransition transition,
                                         const std::string& extra_headers) {
  // Only known frame ids or kMainFrameId are supported.
  const auto frame_id = GetFrameId();
  if (frame_id < HoneycombFrameHostImpl::kMainFrameId) {
    return;
  }

  // Any necessary fixup will occur in LoadRequest.
  GURL gurl = url_util::MakeGURL(url, /*fixup=*/false);

  if (frame_id == HoneycombFrameHostImpl::kMainFrameId) {
    // Load via the browser using NavigationController.
    auto browser = GetBrowserHostBase();
    if (browser) {
      content::OpenURLParams params(
          gurl, referrer, WindowOpenDisposition::CURRENT_TAB, transition,
          /*is_renderer_initiated=*/false);
      params.extra_headers = extra_headers;

      browser->LoadMainFrameURL(params);
    }
  } else {
    auto params = honey::mojom::RequestParams::New();
    params->url = gurl;
    params->referrer =
        blink::mojom::Referrer::New(referrer.url, referrer.policy);
    params->headers = extra_headers;
    LoadRequest(std::move(params));
  }
}

void HoneycombFrameHostImpl::SendCommand(const std::string& command) {
  DCHECK(!command.empty());
  SendToRenderFrame(__FUNCTION__, base::BindOnce(
                                      [](const std::string& command,
                                         const RenderFrameType& render_frame) {
                                        render_frame->SendCommand(command);
                                      },
                                      command));
}

void HoneycombFrameHostImpl::SendCommandWithResponse(
    const std::string& command,
    honey::mojom::RenderFrame::SendCommandWithResponseCallback
        response_callback) {
  DCHECK(!command.empty());
  SendToRenderFrame(
      __FUNCTION__,
      base::BindOnce(
          [](const std::string& command,
             honey::mojom::RenderFrame::SendCommandWithResponseCallback
                 response_callback,
             const RenderFrameType& render_frame) {
            render_frame->SendCommandWithResponse(command,
                                                  std::move(response_callback));
          },
          command, std::move(response_callback)));
}

void HoneycombFrameHostImpl::SendJavaScript(const std::u16string& jsCode,
                                      const std::string& scriptUrl,
                                      int startLine) {
  if (jsCode.empty()) {
    return;
  }
  if (startLine <= 0) {
    // A value of 0 is v8::Message::kNoLineNumberInfo in V8. There is code in
    // V8 that will assert on that value (e.g. V8StackTraceImpl::Frame::Frame
    // if a JS exception is thrown) so make sure |startLine| > 0.
    startLine = 1;
  }

  SendToRenderFrame(
      __FUNCTION__,
      base::BindOnce(
          [](const std::u16string& jsCode, const std::string& scriptUrl,
             int startLine, const RenderFrameType& render_frame) {
            render_frame->SendJavaScript(jsCode, scriptUrl, startLine);
          },
          jsCode, scriptUrl, startLine));
}

void HoneycombFrameHostImpl::MaybeSendDidStopLoading() {
  auto rfh = GetRenderFrameHost();
  if (!rfh) {
    return;
  }

  // We only want to notify for the highest-level LocalFrame in this frame's
  // renderer process subtree. If this frame has a parent in the same process
  // then the notification will be sent via the parent instead.
  auto rfh_parent = rfh->GetParent();
  if (rfh_parent && rfh_parent->GetProcess() == rfh->GetProcess()) {
    return;
  }

  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce([](const RenderFrameType& render_frame) {
                      render_frame->DidStopLoading();
                    }));
}

void HoneycombFrameHostImpl::ExecuteJavaScriptWithUserGestureForTests(
    const HoneycombString& javascript) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(
            &HoneycombFrameHostImpl::ExecuteJavaScriptWithUserGestureForTests, this,
            javascript));
    return;
  }

  content::RenderFrameHost* rfh = GetRenderFrameHost();
  if (rfh) {
    rfh->ExecuteJavaScriptWithUserGestureForTests(javascript,
                                                  base::NullCallback());
  }
}

content::RenderFrameHost* HoneycombFrameHostImpl::GetRenderFrameHost() const {
  HONEYCOMB_REQUIRE_UIT();
  return render_frame_host_;
}

bool HoneycombFrameHostImpl::Detach(DetachReason reason) {
  HONEYCOMB_REQUIRE_UIT();

  if (VLOG_IS_ON(1)) {
    std::string reason_str;
    switch (reason) {
      case DetachReason::RENDER_FRAME_DELETED:
        reason_str = "RENDER_FRAME_DELETED";
        break;
      case DetachReason::NEW_MAIN_FRAME:
        reason_str = "NEW_MAIN_FRAME";
        break;
      case DetachReason::BROWSER_DESTROYED:
        reason_str = "BROWSER_DESTROYED";
        break;
    };

    VLOG(1) << GetDebugString() << " detached (reason=" << reason_str
            << ", is_connected=" << render_frame_.is_bound() << ")";
  }

  // May be called multiple times (e.g. from HoneycombBrowserInfo SetMainFrame and
  // RemoveFrame).
  bool first_detach = false;

  // Should not be called for temporary frames.
  DCHECK(!is_temporary());

  {
    base::AutoLock lock_scope(state_lock_);
    if (browser_info_) {
      first_detach = true;
      browser_info_ = nullptr;
    }
  }

  // In case we never attached, clean up.
  while (!queued_renderer_actions_.empty()) {
    queued_renderer_actions_.pop();
  }

  if (render_frame_.is_bound()) {
    render_frame_->FrameDetached();
  }

  render_frame_.reset();
  render_frame_host_ = nullptr;

  return first_detach;
}

void HoneycombFrameHostImpl::MaybeReAttach(
    scoped_refptr<HoneycombBrowserInfo> browser_info,
    content::RenderFrameHost* render_frame_host) {
  HONEYCOMB_REQUIRE_UIT();
  if (render_frame_.is_bound() && render_frame_host_ == render_frame_host) {
    // Nothing to do here.
    return;
  }

  // We expect that Detach() was called previously.
  CHECK(!is_temporary());
  CHECK(!render_frame_.is_bound());
  CHECK(!render_frame_host_);

  // The RFH may change but the GlobalId should remain the same.
  CHECK_EQ(frame_id_,
           frame_util::MakeFrameId(render_frame_host->GetGlobalId()));

  {
    base::AutoLock lock_scope(state_lock_);
    browser_info_ = browser_info;
  }

  render_frame_host_ = render_frame_host;
  RefreshAttributes();

  // We expect a reconnect to be triggered via FrameAttached().
}

// kMainFrameId must be -1 to align with renderer expectations.
const int64_t HoneycombFrameHostImpl::kMainFrameId = -1;
const int64_t HoneycombFrameHostImpl::kFocusedFrameId = -2;
const int64_t HoneycombFrameHostImpl::kUnspecifiedFrameId = -3;
const int64_t HoneycombFrameHostImpl::kInvalidFrameId = -4;

// This equates to (TT_EXPLICIT | TT_DIRECT_LOAD_FLAG).
const ui::PageTransition HoneycombFrameHostImpl::kPageTransitionExplicit =
    static_cast<ui::PageTransition>(ui::PAGE_TRANSITION_TYPED |
                                    ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);

int64_t HoneycombFrameHostImpl::GetFrameId() const {
  base::AutoLock lock_scope(state_lock_);
  return is_main_frame_ ? kMainFrameId : frame_id_;
}

scoped_refptr<HoneycombBrowserInfo> HoneycombFrameHostImpl::GetBrowserInfo() const {
  base::AutoLock lock_scope(state_lock_);
  return browser_info_;
}

HoneycombRefPtr<HoneycombBrowserHostBase> HoneycombFrameHostImpl::GetBrowserHostBase() const {
  if (auto browser_info = GetBrowserInfo()) {
    return browser_info->browser();
  }
  return nullptr;
}

void HoneycombFrameHostImpl::SendToRenderFrame(const std::string& function_name,
                                         RenderFrameAction action) {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombFrameHostImpl::SendToRenderFrame, this,
                                 function_name, std::move(action)));
    return;
  }

  if (is_temporary()) {
    LOG(WARNING) << function_name
                 << " sent to temporary subframe will be ignored.";
    return;
  } else if (!render_frame_host_) {
    // We've been detached.
    LOG(WARNING) << function_name << " sent to detached " << GetDebugString()
                 << " will be ignored";
    return;
  }

  if (!render_frame_.is_bound()) {
    // Queue actions until we're notified by the renderer that it's ready to
    // handle them.
    queued_renderer_actions_.push(
        std::make_pair(function_name, std::move(action)));
    return;
  }

  std::move(action).Run(render_frame_);
}

void HoneycombFrameHostImpl::OnRenderFrameDisconnect() {
  HONEYCOMB_REQUIRE_UIT();

  // Reconnect, if any, will be triggered via FrameAttached().
  render_frame_.reset();
}

void HoneycombFrameHostImpl::SendMessage(const std::string& name,
                                   base::Value::List arguments) {
  if (auto browser = GetBrowserHostBase()) {
    if (auto client = browser->GetClient()) {
      HoneycombRefPtr<HoneycombProcessMessageImpl> message(
          new HoneycombProcessMessageImpl(name, std::move(arguments),
                                    /*read_only=*/true));
      browser->GetClient()->OnProcessMessageReceived(
          browser.get(), this, PID_RENDERER, message.get());
    }
  }
}

void HoneycombFrameHostImpl::SendSharedMemoryRegion(
    const std::string& name,
    base::WritableSharedMemoryRegion region) {
  if (auto browser = GetBrowserHostBase()) {
    if (auto client = browser->GetClient()) {
      HoneycombRefPtr<HoneycombProcessMessage> message(
          new HoneycombProcessMessageSMRImpl(name, std::move(region)));
      browser->GetClient()->OnProcessMessageReceived(browser.get(), this,
                                                     PID_RENDERER, message);
    }
  }
}

void HoneycombFrameHostImpl::FrameAttached(
    mojo::PendingRemote<honey::mojom::RenderFrame> render_frame_remote,
    bool reattached) {
  HONEYCOMB_REQUIRE_UIT();
  CHECK(render_frame_remote);

  auto browser_info = GetBrowserInfo();
  if (!browser_info) {
    // Already Detached.
    return;
  }

  VLOG(1) << GetDebugString() << " " << (reattached ? "re" : "") << "connected";

  render_frame_.Bind(std::move(render_frame_remote));
  render_frame_.set_disconnect_handler(
      base::BindOnce(&HoneycombFrameHostImpl::OnRenderFrameDisconnect, this));

  // Notify the renderer process that it can start sending messages.
  render_frame_->FrameAttachedAck();

  while (!queued_renderer_actions_.empty()) {
    std::move(queued_renderer_actions_.front().second).Run(render_frame_);
    queued_renderer_actions_.pop();
  }

  browser_info->MaybeExecuteFrameNotification(base::BindOnce(
      [](HoneycombRefPtr<HoneycombFrameHostImpl> self, bool reattached,
         HoneycombRefPtr<HoneycombFrameHandler> handler) {
        if (auto browser = self->GetBrowserHostBase()) {
          handler->OnFrameAttached(browser, self, reattached);
        }
      },
      HoneycombRefPtr<HoneycombFrameHostImpl>(this), reattached));
}

void HoneycombFrameHostImpl::UpdateDraggableRegions(
    absl::optional<std::vector<honey::mojom::DraggableRegionEntryPtr>> regions) {
  auto browser = GetBrowserHostBase();
  if (!browser) {
    return;
  }

  std::vector<HoneycombDraggableRegion> draggable_regions;
  if (regions) {
    draggable_regions.reserve(regions->size());

    for (const auto& region : *regions) {
      const auto& rect = region->bounds;
      const HoneycombRect bounds(rect.x(), rect.y(), rect.width(), rect.height());
      draggable_regions.push_back(
          HoneycombDraggableRegion(bounds, region->draggable));
    }
  }

  // Delegate to BrowserInfo so that current state is maintained with
  // cross-origin navigation.
  browser_info_->MaybeNotifyDraggableRegionsChanged(
      browser, this, std::move(draggable_regions));
}

std::string HoneycombFrameHostImpl::GetDebugString() const {
  return "frame " + frame_util::GetFrameDebugString(frame_id_) +
         (is_main_frame_ ? " (main)" : " (sub)");
}

void HoneycombExecuteJavaScriptWithUserGestureForTests(HoneycombRefPtr<HoneycombFrame> frame,
                                                 const HoneycombString& javascript) {
  HoneycombFrameHostImpl* impl = static_cast<HoneycombFrameHostImpl*>(frame.get());
  if (impl) {
    impl->ExecuteJavaScriptWithUserGestureForTests(javascript);
  }
}
