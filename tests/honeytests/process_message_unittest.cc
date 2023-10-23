// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_process_message.h"
#include "include/honey_task.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/test_util.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/renderer/client_app_renderer.h"

using client::ClientAppRenderer;

namespace {

// Unique values for the SendRecv test.
const char kSendRecvUrl[] = "https://tests/ProcessMessageTest.SendRecv";
const char kSendRecvMsg[] = "ProcessMessageTest.SendRecv";

// Creates a test message.
HoneycombRefPtr<HoneycombProcessMessage> CreateTestMessage() {
  HoneycombRefPtr<HoneycombProcessMessage> msg = HoneycombProcessMessage::Create(kSendRecvMsg);
  EXPECT_TRUE(msg.get());
  EXPECT_TRUE(msg->IsValid());
  EXPECT_FALSE(msg->IsReadOnly());

  HoneycombRefPtr<HoneycombListValue> args = msg->GetArgumentList();
  EXPECT_TRUE(args.get());
  EXPECT_TRUE(args->IsValid());
  EXPECT_FALSE(args->IsReadOnly());

  size_t index = 0;
  args->SetNull(index++);
  args->SetInt(index++, 5);
  args->SetDouble(index++, 10.543);
  args->SetBool(index++, true);
  args->SetString(index++, "test string");
  args->SetList(index++, args->Copy());

  EXPECT_EQ(index, args->GetSize());

  return msg;
}

// Renderer side.
class SendRecvRendererTest : public ClientAppRenderer::Delegate {
 public:
  SendRecvRendererTest() {}

  bool OnProcessMessageReceived(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    if (message->GetName() == kSendRecvMsg) {
      EXPECT_TRUE(browser.get());
      EXPECT_TRUE(frame.get());
      EXPECT_EQ(PID_BROWSER, source_process);
      EXPECT_TRUE(message.get());
      EXPECT_TRUE(message->IsValid());
      EXPECT_TRUE(message->IsReadOnly());

      const std::string& url = frame->GetURL();
      if (url == kSendRecvUrl) {
        // Echo the message back to the sender natively.
        frame->SendProcessMessage(PID_BROWSER, message);
        EXPECT_FALSE(message->IsValid());
        return true;
      }
    }

    // Message not handled.
    return false;
  }

  IMPLEMENT_REFCOUNTING(SendRecvRendererTest);
};

// Browser side.
class SendRecvTestHandler : public TestHandler {
 public:
  explicit SendRecvTestHandler(honey_thread_id_t send_thread)
      : send_thread_(send_thread) {}

  void RunTest() override {
    AddResource(kSendRecvUrl, "<html><body>TEST</body></html>", "text/html");
    CreateBrowser(kSendRecvUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(HoneycombRefPtr<HoneycombBrowser> browser,
                 HoneycombRefPtr<HoneycombFrame> frame,
                 int httpStatusCode) override {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));

    // Send the message to the renderer process.
    if (!HoneycombCurrentlyOn(send_thread_)) {
      HoneycombPostTask(send_thread_,
                  base::BindOnce(&SendRecvTestHandler::SendMessage, this,
                                 browser, frame));
    } else {
      SendMessage(browser, frame);
    }
  }

  bool OnProcessMessageReceived(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    EXPECT_TRUE(HoneycombCurrentlyOn(TID_UI));
    EXPECT_TRUE(browser.get());
    EXPECT_TRUE(frame.get());
    EXPECT_EQ(PID_RENDERER, source_process);
    EXPECT_TRUE(message.get());
    EXPECT_TRUE(message->IsValid());
    EXPECT_TRUE(message->IsReadOnly());

    // Verify that the recieved message is the same as the sent message.
    TestProcessMessageEqual(CreateTestMessage(), message);

    got_message_.yes();

    // Test is complete.
    DestroyTest();

    return true;
  }

 protected:
  void DestroyTest() override {
    EXPECT_TRUE(got_message_);
    TestHandler::DestroyTest();
  }

 private:
  void SendMessage(HoneycombRefPtr<HoneycombBrowser> browser, HoneycombRefPtr<HoneycombFrame> frame) {
    EXPECT_TRUE(HoneycombCurrentlyOn(send_thread_));
    auto message = CreateTestMessage();
    frame->SendProcessMessage(PID_RENDERER, message);

    // The message will be invalidated immediately, no matter what thread we
    // send from.
    EXPECT_FALSE(message->IsValid());
  }

  honey_thread_id_t send_thread_;
  TrackCallback got_message_;

  IMPLEMENT_REFCOUNTING(SendRecvTestHandler);
};

}  // namespace

// Verify send from the UI thread and recieve.
TEST(ProcessMessageTest, SendRecvUI) {
  HoneycombRefPtr<SendRecvTestHandler> handler = new SendRecvTestHandler(TID_UI);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Verify send from the IO thread and recieve.
TEST(ProcessMessageTest, SendRecvIO) {
  HoneycombRefPtr<SendRecvTestHandler> handler = new SendRecvTestHandler(TID_IO);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Verify create.
TEST(ProcessMessageTest, Create) {
  HoneycombRefPtr<HoneycombProcessMessage> message =
      HoneycombProcessMessage::Create(kSendRecvMsg);
  EXPECT_TRUE(message.get());
  EXPECT_TRUE(message->IsValid());
  EXPECT_FALSE(message->IsReadOnly());
  EXPECT_STREQ(kSendRecvMsg, message->GetName().ToString().c_str());

  HoneycombRefPtr<HoneycombListValue> args = message->GetArgumentList();
  EXPECT_TRUE(args.get());
  EXPECT_TRUE(args->IsValid());
  EXPECT_FALSE(args->IsOwned());
  EXPECT_FALSE(args->IsReadOnly());
}

// Verify copy.
TEST(ProcessMessageTest, Copy) {
  HoneycombRefPtr<HoneycombProcessMessage> message = CreateTestMessage();
  HoneycombRefPtr<HoneycombProcessMessage> message2 = message->Copy();
  TestProcessMessageEqual(message, message2);
}

// Entry point for creating process message renderer test objects.
// Called from client_app_delegates.cc.
void CreateProcessMessageRendererTests(
    ClientAppRenderer::DelegateSet& delegates) {
  // For ProcessMessageTest.SendRecv
  delegates.insert(new SendRecvRendererTest);
}
