// Copyright (c) 2020 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/devtools/devtools_manager.h"

#include "libhoney/browser/devtools/devtools_controller.h"
#include "libhoney/browser/devtools/devtools_frontend.h"
#include "libhoney/features/runtime.h"

#include "content/public/browser/web_contents.h"

namespace {

// May be created on any thread but will be destroyed on the UI thread.
class HoneycombDevToolsRegistrationImpl : public HoneycombRegistration,
                                    public HoneycombDevToolsController::Observer {
 public:
  explicit HoneycombDevToolsRegistrationImpl(
      HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer)
      : observer_(observer) {
    DCHECK(observer_);
  }

  HoneycombDevToolsRegistrationImpl(const HoneycombDevToolsRegistrationImpl&) = delete;
  HoneycombDevToolsRegistrationImpl& operator=(const HoneycombDevToolsRegistrationImpl&) =
      delete;

  ~HoneycombDevToolsRegistrationImpl() override {
    HONEYCOMB_REQUIRE_UIT();

    // May be null if OnDevToolsControllerDestroyed was called.
    if (!controller_) {
      return;
    }

    controller_->RemoveObserver(this);
  }

  void Initialize(HoneycombBrowserHostBase* browser,
                  base::WeakPtr<HoneycombDevToolsController> controller) {
    HONEYCOMB_REQUIRE_UIT();
    DCHECK(browser && controller);
    DCHECK(!browser_ && !controller_);
    browser_ = browser;
    controller_ = controller;

    controller_->AddObserver(this);
  }

 private:
  // HoneycombDevToolsController::Observer methods:
  bool OnDevToolsMessage(const base::StringPiece& message) override {
    HONEYCOMB_REQUIRE_UIT();
    return observer_->OnDevToolsMessage(browser_, message.data(),
                                        message.size());
  }

  void OnDevToolsMethodResult(int message_id,
                              bool success,
                              const base::StringPiece& result) override {
    HONEYCOMB_REQUIRE_UIT();
    observer_->OnDevToolsMethodResult(browser_, message_id, success,
                                      result.data(), result.size());
  }

  void OnDevToolsEvent(const base::StringPiece& method,
                       const base::StringPiece& params) override {
    HONEYCOMB_REQUIRE_UIT();
    observer_->OnDevToolsEvent(browser_, std::string(method), params.data(),
                               params.size());
  }

  void OnDevToolsAgentAttached() override {
    HONEYCOMB_REQUIRE_UIT();
    observer_->OnDevToolsAgentAttached(browser_);
  }

  void OnDevToolsAgentDetached() override {
    HONEYCOMB_REQUIRE_UIT();
    observer_->OnDevToolsAgentDetached(browser_);
  }

  void OnDevToolsControllerDestroyed() override {
    HONEYCOMB_REQUIRE_UIT();
    browser_ = nullptr;
    controller_.reset();
  }

  HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer_;

  HoneycombBrowserHostBase* browser_ = nullptr;
  base::WeakPtr<HoneycombDevToolsController> controller_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombDevToolsRegistrationImpl);
};

}  // namespace

HoneycombDevToolsManager::HoneycombDevToolsManager(HoneycombBrowserHostBase* inspected_browser)
    : inspected_browser_(inspected_browser), weak_ptr_factory_(this) {
  HONEYCOMB_REQUIRE_UIT();
}

HoneycombDevToolsManager::~HoneycombDevToolsManager() {
  HONEYCOMB_REQUIRE_UIT();
}

void HoneycombDevToolsManager::ShowDevTools(const HoneycombWindowInfo& windowInfo,
                                      HoneycombRefPtr<HoneycombClient> client,
                                      const HoneycombBrowserSettings& settings,
                                      const HoneycombPoint& inspect_element_at) {
  HONEYCOMB_REQUIRE_UIT();
  if (devtools_frontend_) {
    if (!inspect_element_at.IsEmpty()) {
      devtools_frontend_->InspectElementAt(inspect_element_at.x,
                                           inspect_element_at.y);
    }
    devtools_frontend_->Focus();
    return;
  }

  if (honey::IsChromeRuntimeEnabled()) {
    NOTIMPLEMENTED();
  } else {
    auto alloy_browser = static_cast<AlloyBrowserHostImpl*>(inspected_browser_);
    devtools_frontend_ = HoneycombDevToolsFrontend::Show(
        alloy_browser, windowInfo, client, settings, inspect_element_at,
        base::BindOnce(&HoneycombDevToolsManager::OnFrontEndDestroyed,
                       weak_ptr_factory_.GetWeakPtr()));
  }
}

void HoneycombDevToolsManager::CloseDevTools() {
  HONEYCOMB_REQUIRE_UIT();
  if (!devtools_frontend_) {
    return;
  }
  devtools_frontend_->Close();
}

bool HoneycombDevToolsManager::HasDevTools() {
  HONEYCOMB_REQUIRE_UIT();
  return !!devtools_frontend_;
}

bool HoneycombDevToolsManager::SendDevToolsMessage(const void* message,
                                             size_t message_size) {
  HONEYCOMB_REQUIRE_UIT();
  if (!message || message_size == 0) {
    return false;
  }

  if (!EnsureController()) {
    return false;
  }

  return devtools_controller_->SendDevToolsMessage(
      base::StringPiece(static_cast<const char*>(message), message_size));
}

int HoneycombDevToolsManager::ExecuteDevToolsMethod(
    int message_id,
    const HoneycombString& method,
    HoneycombRefPtr<HoneycombDictionaryValue> params) {
  HONEYCOMB_REQUIRE_UIT();
  if (method.empty()) {
    return 0;
  }

  if (!EnsureController()) {
    return 0;
  }

  if (params && params->IsValid()) {
    HoneycombDictionaryValueImpl* impl =
        static_cast<HoneycombDictionaryValueImpl*>(params.get());
    HoneycombValueController::AutoLock lock_scope(impl->controller());
    return devtools_controller_->ExecuteDevToolsMethod(
        message_id, method, impl->GetValueUnsafe()->GetIfDict());
  } else {
    return devtools_controller_->ExecuteDevToolsMethod(message_id, method,
                                                       nullptr);
  }
}

// static
HoneycombRefPtr<HoneycombRegistration> HoneycombDevToolsManager::CreateRegistration(
    HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer) {
  DCHECK(observer);
  return new HoneycombDevToolsRegistrationImpl(observer);
}

void HoneycombDevToolsManager::InitializeRegistrationOnUIThread(
    HoneycombRefPtr<HoneycombRegistration> registration) {
  HONEYCOMB_REQUIRE_UIT();

  if (!EnsureController()) {
    return;
  }

  static_cast<HoneycombDevToolsRegistrationImpl*>(registration.get())
      ->Initialize(inspected_browser_, devtools_controller_->GetWeakPtr());
}

void HoneycombDevToolsManager::OnFrontEndDestroyed() {
  devtools_frontend_ = nullptr;
}

bool HoneycombDevToolsManager::EnsureController() {
  if (!devtools_controller_) {
    devtools_controller_.reset(new HoneycombDevToolsController(
        inspected_browser_->contents_delegate()->web_contents()));
  }
  return true;
}
