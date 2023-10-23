// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_CONTEXT_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_CONTEXT_H_
#pragma once

#include <string>

#include "include/base/honey_macros.h"
#include "include/base/honey_ref_counted.h"
#include "include/internal/honey_types_wrappers.h"
#include "tests/honeyclient/browser/osr_renderer_settings.h"

namespace client {

class RootWindowManager;

// Used to store global context in the browser process. The methods of this
// class are thread-safe unless otherwise indicated.
class MainContext {
 public:
  // Returns the singleton instance of this object.
  static MainContext* Get();

  // Returns the full path to the console log file.
  virtual std::string GetConsoleLogPath() = 0;

  // Returns the full path to |file_name|.
  virtual std::string GetDownloadPath(const std::string& file_name) = 0;

  // Returns the app working directory including trailing path separator.
  virtual std::string GetAppWorkingDirectory() = 0;

  // Returns the main application URL.
  virtual std::string GetMainURL() = 0;

  // Returns the background color.
  virtual honey_color_t GetBackgroundColor() = 0;

  // Returns true if the Chrome runtime will be used.
  virtual bool UseChromeRuntime() = 0;

  // Returns true if the Views framework will be used.
  virtual bool UseViews() = 0;

  // Returns true if windowless (off-screen) rendering will be used.
  virtual bool UseWindowlessRendering() = 0;

  // Returns true if touch events are enabled.
  virtual bool TouchEventsEnabled() = 0;

  // Returns true if the default popup implementation should be used.
  virtual bool UseDefaultPopup() = 0;

  // Populate |settings| based on command-line arguments.
  virtual void PopulateSettings(HoneycombSettings* settings) = 0;
  virtual void PopulateBrowserSettings(HoneycombBrowserSettings* settings) = 0;
  virtual void PopulateOsrSettings(OsrRendererSettings* settings) = 0;

  // Returns the object used to create/manage RootWindow instances.
  virtual RootWindowManager* GetRootWindowManager() = 0;

 protected:
  MainContext();
  virtual ~MainContext();

 private:
  DISALLOW_COPY_AND_ASSIGN(MainContext);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_MAIN_CONTEXT_H_
