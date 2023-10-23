// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_WINDOW_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_WINDOW_H_
#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "include/base/honey_callback_forward.h"
#include "include/honey_menu_model_delegate.h"
#include "include/views/honey_browser_view.h"
#include "include/views/honey_browser_view_delegate.h"
#include "include/views/honey_button_delegate.h"
#include "include/views/honey_label_button.h"
#include "include/views/honey_menu_button.h"
#include "include/views/honey_menu_button_delegate.h"
#include "include/views/honey_overlay_controller.h"
#include "include/views/honey_textfield.h"
#include "include/views/honey_textfield_delegate.h"
#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"
#include "tests/honeyclient/browser/image_cache.h"
#include "tests/honeyclient/browser/root_window.h"
#include "tests/honeyclient/browser/views_menu_bar.h"
#include "tests/honeyclient/browser/views_overlay_controls.h"

namespace client {

typedef std::set<HoneycombRefPtr<HoneycombExtension>> ExtensionSet;

// Implements a HoneycombWindow that hosts a single HoneycombBrowserView and optional
// Views-based controls. All methods must be called on the browser process UI
// thread.
class ViewsWindow : public HoneycombBrowserViewDelegate,
                    public HoneycombMenuButtonDelegate,
                    public HoneycombMenuModelDelegate,
                    public HoneycombTextfieldDelegate,
                    public HoneycombWindowDelegate,
                    public ViewsMenuBar::Delegate {
 public:
  // Delegate methods will be called on the browser process UI thread.
  class Delegate {
   public:
    // Return true if the window should show controls.
    virtual bool WithControls() = 0;

    // Return true if the window should be created initially hidden.
    virtual bool InitiallyHidden() = 0;

    // Returns the parent for this window.
    virtual HoneycombRefPtr<HoneycombWindow> GetParentWindow() = 0;

    // Return the initial window bounds.
    virtual HoneycombRect GetInitialBounds() = 0;

    // Return the initial window show state.
    virtual honey_show_state_t GetInitialShowState() = 0;

    // Returns the ImageCache.
    virtual scoped_refptr<ImageCache> GetImageCache() = 0;

    // Called when the ViewsWindow is created.
    virtual void OnViewsWindowCreated(HoneycombRefPtr<ViewsWindow> window) = 0;

    // Called when the ViewsWindow is closing.
    virtual void OnViewsWindowClosing(HoneycombRefPtr<ViewsWindow> window) = 0;

    // Called when the ViewsWindow is destroyed. All references to |window|
    // should be released in this callback.
    virtual void OnViewsWindowDestroyed(HoneycombRefPtr<ViewsWindow> window) = 0;

    // Called when the ViewsWindow is activated (becomes the foreground window).
    virtual void OnViewsWindowActivated(HoneycombRefPtr<ViewsWindow> window) = 0;

    // Return the Delegate for the popup window controlled by |client|.
    virtual Delegate* GetDelegateForPopup(HoneycombRefPtr<HoneycombClient> client) = 0;

    // Create a window for |extension|. |source_bounds| are the bounds of the
    // UI element, like a button, that triggered the extension.
    virtual void CreateExtensionWindow(HoneycombRefPtr<HoneycombExtension> extension,
                                       const HoneycombRect& source_bounds,
                                       HoneycombRefPtr<HoneycombWindow> parent_window,
                                       base::OnceClosure close_callback) = 0;

    // Called to execute a test. See resource.h for |test_id| values.
    virtual void OnTest(int test_id) = 0;

    // Called to exit the application.
    virtual void OnExit() = 0;

   protected:
    virtual ~Delegate() {}
  };

  // Create a new top-level ViewsWindow hosting a browser with the specified
  // configuration.
  static HoneycombRefPtr<ViewsWindow> Create(
      WindowType type,
      Delegate* delegate,
      HoneycombRefPtr<HoneycombClient> client,
      const HoneycombString& url,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombRequestContext> request_context);

  void Show();
  void Hide();
  void Minimize();
  void Maximize();
  void SetBounds(const HoneycombRect& bounds);
  void SetBrowserSize(const HoneycombSize& size,
                      bool has_position,
                      const HoneycombPoint& position);
  void Close(bool force);
  void SetAddress(const std::string& url);
  void SetTitle(const std::string& title);
  void SetFavicon(HoneycombRefPtr<HoneycombImage> image);
  void SetFullscreen(bool fullscreen);
  void SetAlwaysOnTop(bool on_top);
  void SetLoadingState(bool isLoading, bool canGoBack, bool canGoForward);
  void SetDraggableRegions(const std::vector<HoneycombDraggableRegion>& regions);
  void TakeFocus(bool next);
  void OnBeforeContextMenu(HoneycombRefPtr<HoneycombMenuModel> model);
  void OnExtensionsChanged(const ExtensionSet& extensions);

  bool GetWindowRestorePreferences(honey_show_state_t& show_state,
                                   std::optional<HoneycombRect>& dip_bounds);
  void SetTitlebarHeight(const std::optional<float>& height);

  // HoneycombBrowserViewDelegate methods:
  HoneycombRefPtr<HoneycombBrowserViewDelegate> GetDelegateForPopupBrowserView(
      HoneycombRefPtr<HoneycombBrowserView> browser_view,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombClient> client,
      bool is_devtools) override;
  bool OnPopupBrowserViewCreated(HoneycombRefPtr<HoneycombBrowserView> browser_view,
                                 HoneycombRefPtr<HoneycombBrowserView> popup_browser_view,
                                 bool is_devtools) override;
  ChromeToolbarType GetChromeToolbarType(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) override;
  bool UseFramelessWindowForPictureInPicture(
      HoneycombRefPtr<HoneycombBrowserView> browser_view) override;

  // HoneycombButtonDelegate methods:
  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override;

  // HoneycombMenuButtonDelegate methods:
  void OnMenuButtonPressed(
      HoneycombRefPtr<HoneycombMenuButton> menu_button,
      const HoneycombPoint& screen_point,
      HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) override;

  // HoneycombMenuModelDelegate methods:
  void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                      int command_id,
                      honey_event_flags_t event_flags) override;

  // HoneycombTextfieldDelegate methods:
  bool OnKeyEvent(HoneycombRefPtr<HoneycombTextfield> textfield,
                  const HoneycombKeyEvent& event) override;

  // HoneycombWindowDelegate methods:
  void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) override;
  void OnWindowClosing(HoneycombRefPtr<HoneycombWindow> window) override;
  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override;
  void OnWindowActivationChanged(HoneycombRefPtr<HoneycombWindow> window,
                                 bool active) override;
  void OnWindowBoundsChanged(HoneycombRefPtr<HoneycombWindow> window,
                             const HoneycombRect& new_bounds) override;
  HoneycombRefPtr<HoneycombWindow> GetParentWindow(HoneycombRefPtr<HoneycombWindow> window,
                                       bool* is_menu,
                                       bool* can_activate_menu) override;
  bool IsWindowModalDialog(HoneycombRefPtr<HoneycombWindow> window) override;
  HoneycombRect GetInitialBounds(HoneycombRefPtr<HoneycombWindow> window) override;
  honey_show_state_t GetInitialShowState(HoneycombRefPtr<HoneycombWindow> window) override;
  bool IsFrameless(HoneycombRefPtr<HoneycombWindow> window) override;
  bool WithStandardWindowButtons(HoneycombRefPtr<HoneycombWindow> window) override;
  bool GetTitlebarHeight(HoneycombRefPtr<HoneycombWindow> window,
                         float* titlebar_height) override;
  bool CanResize(HoneycombRefPtr<HoneycombWindow> window) override;
  bool CanMaximize(HoneycombRefPtr<HoneycombWindow> window) override;
  bool CanMinimize(HoneycombRefPtr<HoneycombWindow> window) override;
  bool CanClose(HoneycombRefPtr<HoneycombWindow> window) override;
  bool OnAccelerator(HoneycombRefPtr<HoneycombWindow> window, int command_id) override;
  bool OnKeyEvent(HoneycombRefPtr<HoneycombWindow> window,
                  const HoneycombKeyEvent& event) override;
  void OnWindowFullscreenTransition(HoneycombRefPtr<HoneycombWindow> window,
                                    bool is_completed) override;

  // HoneycombViewDelegate methods:
  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override;
  HoneycombSize GetMinimumSize(HoneycombRefPtr<HoneycombView> view) override;
  void OnFocus(HoneycombRefPtr<HoneycombView> view) override;
  void OnBlur(HoneycombRefPtr<HoneycombView> view) override;
  void OnWindowChanged(HoneycombRefPtr<HoneycombView> view, bool added) override;
  void OnLayoutChanged(HoneycombRefPtr<HoneycombView> view,
                       const HoneycombRect& new_bounds) override;

  // ViewsMenuBar::Delegate methods:
  void MenuBarExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                             int command_id,
                             honey_event_flags_t event_flags) override;

 private:
  // |delegate| is guaranteed to outlive this object.
  // |browser_view| may be nullptr, in which case SetBrowserView() will be
  // called.
  ViewsWindow(WindowType type,
              Delegate* delegate,
              HoneycombRefPtr<HoneycombBrowserView> browser_view);

  void SetBrowserView(HoneycombRefPtr<HoneycombBrowserView> browser_view);

  // Create controls.
  void CreateMenuModel();
  HoneycombRefPtr<HoneycombLabelButton> CreateBrowseButton(const std::string& label,
                                               int id);
  HoneycombRefPtr<HoneycombMenuButton> CreateMenuButton();
  HoneycombRefPtr<HoneycombView> CreateLocationBar();

  // Add the BrowserView to the Window.
  void AddBrowserView();

  // Add other controls to the Window.
  void AddControls();

  // Add keyboard accelerators to the Window.
  void AddAccelerators();

  // Control whether the top menu butons are focusable.
  void SetMenuFocusable(bool focusable);

  // Enable or disable a view by |id|.
  void EnableView(int id, bool enable);

  // Show/hide top controls on the Window.
  void ShowTopControls(bool show);

  // Update extension controls on the Window.
  void UpdateExtensionControls();

  void OnExtensionIconsLoaded(const ExtensionSet& extensions,
                              const ImageCache::ImageSet& images);
  void OnExtensionWindowClosed();

  void NudgeWindow();

  const WindowType type_;
  Delegate* delegate_;  // Not owned by this object.
  HoneycombRefPtr<HoneycombBrowserView> browser_view_;
  bool frameless_;
  bool with_controls_;
  bool with_overlay_controls_;
  bool with_standard_buttons_;
  ChromeToolbarType chrome_toolbar_type_;
  bool use_window_modal_dialog_;
  bool use_bottom_controls_;
  bool hide_pip_frame_;
  HoneycombRefPtr<HoneycombWindow> window_;

  HoneycombRefPtr<HoneycombMenuModel> button_menu_model_;
  HoneycombRefPtr<ViewsMenuBar> menu_bar_;
  HoneycombRefPtr<HoneycombView> toolbar_;
  HoneycombRefPtr<HoneycombMenuButton> menu_button_;
  HoneycombRefPtr<HoneycombView> location_bar_;
  bool menu_has_focus_;
  int last_focused_view_;
  std::optional<HoneycombRect> last_visible_bounds_;

  HoneycombSize minimum_window_size_;

  HoneycombRefPtr<ViewsOverlayControls> overlay_controls_;

  std::optional<float> default_titlebar_height_;
  std::optional<float> override_titlebar_height_;

  // Structure representing an extension.
  struct ExtensionInfo {
    ExtensionInfo(HoneycombRefPtr<HoneycombExtension> extension, HoneycombRefPtr<HoneycombImage> image)
        : extension_(extension), image_(image) {}

    HoneycombRefPtr<HoneycombExtension> extension_;
    HoneycombRefPtr<HoneycombImage> image_;
  };
  typedef std::vector<ExtensionInfo> ExtensionInfoSet;

  ExtensionInfoSet extensions_;
  HoneycombRefPtr<HoneycombPanel> extensions_panel_;
  HoneycombRefPtr<HoneycombMenuButtonPressedLock> extension_button_pressed_lock_;

  IMPLEMENT_REFCOUNTING(ViewsWindow);
  DISALLOW_COPY_AND_ASSIGN(ViewsWindow);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_VIEWS_WINDOW_H_
