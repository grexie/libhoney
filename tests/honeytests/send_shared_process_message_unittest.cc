// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_shared_process_message_builder.h"
#include "include/honey_task.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/test_handler.h"
#include "tests/gtest/include/gtest/gtest.h"
#include "tests/shared/renderer/client_app_renderer.h"

#include <array>

using client::ClientAppRenderer;

namespace {

struct TestData {
  bool flag = true;
  int value = 1;
  double d_value = 77.77;
  std::array<size_t, 50> buffer{};
};

const char kSharedMessageUrl[] = "https://tests/SendSharedProcessMessageTest";
const char kSharedMessageName[] = "SendSharedProcessMessageTest";

HoneycombRefPtr<HoneycombProcessMessage> CreateTestMessage(const TestData& data) {
  auto builder =
      HoneycombSharedProcessMessageBuilder::Create(kSharedMessageName, sizeof(data));
  std::memcpy(builder->Memory(), reinterpret_cast<const void*>(&data),
              sizeof(data));
  return builder->Build();
}

// Renderer side.
class SharedMessageRendererTest final : public ClientAppRenderer::Delegate {
 public:
  bool OnProcessMessageReceived(HoneycombRefPtr<ClientAppRenderer> app,
                                HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                HoneycombProcessId source_process,
                                HoneycombRefPtr<HoneycombProcessMessage> message) override {
    if (message->GetName() == kSharedMessageName) {
      EXPECT_TRUE(browser.get());
      EXPECT_TRUE(frame.get());
      EXPECT_EQ(PID_BROWSER, source_process);
      EXPECT_TRUE(message.get());
      EXPECT_TRUE(message->IsValid());
      EXPECT_TRUE(message->IsReadOnly());
      EXPECT_EQ(message->GetArgumentList(), nullptr);

      const std::string& url = frame->GetURL();
      if (url == kSharedMessageUrl) {
        // Echo the message back to the sender natively.
        frame->SendProcessMessage(PID_BROWSER, message);
        EXPECT_FALSE(message->IsValid());
        return true;
      }
    }

    return false;
  }

  IMPLEMENT_REFCOUNTING(SharedMessageRendererTest);
};

// Browser side.
class SharedMessageTestHandler final : public TestHandler {
 public:
  explicit SharedMessageTestHandler(honey_thread_id_t send_thread)
      : send_thread_(send_thread) {}

  void RunTest() override {
    AddResource(kSharedMessageUrl, "<html><body>TEST</body></html>",
                "text/html");
    CreateBrowser(kSharedMessageUrl);

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
                  base::BindOnce(&SharedMessageTestHandler::SendProcessMessage,
                                 this, browser, frame));
    } else {
      SendProcessMessage(browser, frame);
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
    EXPECT_EQ(message->GetArgumentList(), nullptr);

    // Verify that the recieved message is the same as the sent message.
    auto region = message->GetSharedMemoryRegion();
    const TestData* received = static_cast<const TestData*>(region->Memory());
    EXPECT_EQ(data_.flag, received->flag);
    EXPECT_EQ(data_.value, received->value);
    EXPECT_EQ(data_.d_value, received->d_value);

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
  void SendProcessMessage(const HoneycombRefPtr<HoneycombBrowser>& browser,
                          const HoneycombRefPtr<HoneycombFrame>& frame) {
    EXPECT_TRUE(HoneycombCurrentlyOn(send_thread_));

    auto message = CreateTestMessage(data_);
    frame->SendProcessMessage(PID_RENDERER, message);

    // The message is invalidated immediately
    EXPECT_FALSE(message->IsValid());
  }

  honey_thread_id_t send_thread_;
  TrackCallback got_message_;
  const TestData data_;

  IMPLEMENT_REFCOUNTING(SharedMessageTestHandler);
};

}  // namespace

TEST(SendSharedProcessMessageTest, CanSendAndReceiveFromUiThread) {
  HoneycombRefPtr<SharedMessageTestHandler> handler =
      new SharedMessageTestHandler(TID_UI);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(SendSharedProcessMessageTest, CanSendAndReceiveFromIoThread) {
  HoneycombRefPtr<SharedMessageTestHandler> handler =
      new SharedMessageTestHandler(TID_IO);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Entry point for creating shared process message renderer test objects.
// Called from client_app_delegates.cc.
void CreateSharedProcessMessageTests(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new SharedMessageRendererTest());
}
