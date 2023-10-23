// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_HANDLER_H_
#define HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_HANDLER_H_

#include "include/honey_client.h"

#include <list>

class SimpleHandler : public HoneycombClient,
                      public HoneycombDisplayHandler,
                      public HoneycombLifeSpanHandler,
                      public HoneycombLoadHandler {
 public:
  explicit SimpleHandler(bool use_views);
  ~SimpleHandler();

  // Provide access to the single global instance of this object.
  static SimpleHandler* GetInstance();

  // HoneycombClient methods:
  virtual HoneycombRefPtr<HoneycombDisplayHandler> GetDisplayHandler() override {
    return this;
  }
  virtual HoneycombRefPtr<HoneycombLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }
  virtual HoneycombRefPtr<HoneycombLoadHandler> GetLoadHandler() override { return this; }

  // HoneycombDisplayHandler methods:
  virtual void OnTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                             const HoneycombString& title) override;

  // HoneycombLifeSpanHandler methods:
  virtual void OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  virtual bool DoClose(HoneycombRefPtr<HoneycombBrowser> browser) override;
  virtual void OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) override;

  // HoneycombLoadHandler methods:
  virtual void OnLoadError(HoneycombRefPtr<HoneycombBrowser> browser,
                           HoneycombRefPtr<HoneycombFrame> frame,
                           ErrorCode errorCode,
                           const HoneycombString& errorText,
                           const HoneycombString& failedUrl) override;

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

  // Returns true if the Chrome runtime is enabled.
  static bool IsChromeRuntimeEnabled();

 private:
  // Platform-specific implementation.
  void PlatformTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                           const HoneycombString& title);

  // True if the application is using the Views framework.
  const bool use_views_;

  // List of existing browser windows. Only accessed on the Honeycomb UI thread.
  typedef std::list<HoneycombRefPtr<HoneycombBrowser>> BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleHandler);
};

#endif  // HONEYCOMB_TESTS_HONEYCOMBSIMPLE_SIMPLE_HANDLER_H_
