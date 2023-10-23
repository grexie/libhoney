// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_WIN_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_WIN_H_
#pragma once

#include <windows.h>

#include <commdlg.h>

#include <memory>
#include <string>

#include "tests/honeyclient/browser/browser_window.h"
#include "tests/honeyclient/browser/root_window.h"

namespace client {

// Windows implementation of a top-level native window in the browser process.
// The methods of this class must be called on the main thread unless otherwise
// indicated.
class RootWindowWin : public RootWindow, public BrowserWindow::Delegate {
 public:
  // Constructor may be called on any thread.
  RootWindowWin();
  ~RootWindowWin();

  // RootWindow methods.
  void Init(RootWindow::Delegate* delegate,
            std::unique_ptr<RootWindowConfig> config,
            const HoneycombBrowserSettings& settings) override;
  void InitAsPopup(RootWindow::Delegate* delegate,
                   bool with_controls,
                   bool with_osr,
                   const HoneycombPopupFeatures& popupFeatures,
                   HoneycombWindowInfo& windowInfo,
                   HoneycombRefPtr<HoneycombClient>& client,
                   HoneycombBrowserSettings& settings) override;
  void Show(ShowMode mode) override;
  void Hide() override;
  void SetBounds(int x, int y, size_t width, size_t height) override;
  void Close(bool force) override;
  void SetDeviceScaleFactor(float device_scale_factor) override;
  float GetDeviceScaleFactor() const override;
  HoneycombRefPtr<HoneycombBrowser> GetBrowser() const override;
  ClientWindowHandle GetWindowHandle() const override;
  bool WithWindowlessRendering() const override;
  bool WithExtension() const override;

 private:
  void ContinueInitOnUIThread(std::unique_ptr<RootWindowConfig> config,
                              const HoneycombBrowserSettings& settings);
  void ContinueInitOnMainThread(std::unique_ptr<RootWindowConfig> config,
                                const HoneycombBrowserSettings& settings);

  void CreateBrowserWindow(const std::string& startup_url);
  void CreateRootWindow(const HoneycombBrowserSettings& settings,
                        bool initially_hidden);

  // Register the root window class.
  static void RegisterRootClass(HINSTANCE hInstance,
                                const std::wstring& window_class,
                                HBRUSH background_brush);

  // Window procedure for the edit field.
  static LRESULT CALLBACK EditWndProc(HWND hWnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam);

  // Window procedure for the find dialog.
  static LRESULT CALLBACK FindWndProc(HWND hWnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam);

  // Window procedure for the root window.
  static LRESULT CALLBACK RootWndProc(HWND hWnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam);

  // Event handlers.
  void OnPaint();
  void OnFocus();
  void OnActivate(bool active);
  void OnSize(bool minimized);
  void OnMove();
  void OnDpiChanged(WPARAM wParam, LPARAM lParam);
  bool OnEraseBkgnd();
  bool OnCommand(UINT id);
  void OnFind();
  void OnFindEvent();
  void OnAbout();
  void OnNCCreate(LPCREATESTRUCT lpCreateStruct);
  void OnCreate(LPCREATESTRUCT lpCreateStruct);
  bool OnClose();
  void OnDestroyed();

  // BrowserWindow::Delegate methods.
  void OnBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnBrowserWindowDestroyed() override;
  void OnSetAddress(const std::string& url) override;
  void OnSetTitle(const std::string& title) override;
  void OnSetFullscreen(bool fullscreen) override;
  void OnAutoResize(const HoneycombSize& new_size) override;
  void OnSetLoadingState(bool isLoading,
                         bool canGoBack,
                         bool canGoForward) override;
  void OnSetDraggableRegions(
      const std::vector<HoneycombDraggableRegion>& regions) override;

  void NotifyDestroyedIfDone();

  static void SaveWindowRestoreOnUIThread(const WINDOWPLACEMENT& placement);

  // After initialization all members are only accessed on the main thread.
  // Members set during initialization.
  bool with_controls_ = false;
  bool always_on_top_ = false;
  bool with_osr_ = false;
  bool with_extension_ = false;
  bool is_popup_ = false;
  HoneycombRect initial_bounds_;
  honey_show_state_t initial_show_state_ = HONEYCOMB_SHOW_STATE_NORMAL;
  std::unique_ptr<BrowserWindow> browser_window_;
  HoneycombBrowserSettings browser_settings_;
  bool initialized_ = false;

  // Main window.
  HWND hwnd_ = nullptr;

  // Draggable region.
  HRGN draggable_region_ = nullptr;

  // Font for buttons and text fields.
  HFONT font_ = nullptr;
  int font_height_ = 0;

  // Buttons.
  HWND back_hwnd_ = nullptr;
  HWND forward_hwnd_ = nullptr;
  HWND reload_hwnd_ = nullptr;
  HWND stop_hwnd_ = nullptr;

  // URL text field.
  HWND edit_hwnd_ = nullptr;
  WNDPROC edit_wndproc_old_ = nullptr;

  // Find dialog.
  HWND find_hwnd_ = nullptr;
  UINT find_message_id_ = 0;
  WNDPROC find_wndproc_old_ = nullptr;

  // Find dialog state.
  FINDREPLACE find_state_ = {0};
  WCHAR find_buff_[80] = {0};
  std::wstring find_what_last_;
  bool find_next_ = false;
  bool find_match_case_last_ = false;

  bool window_destroyed_ = false;
  bool browser_destroyed_ = false;

  bool called_enable_non_client_dpi_scaling_ = false;

  DISALLOW_COPY_AND_ASSIGN(RootWindowWin);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_ROOT_WINDOW_WIN_H_
