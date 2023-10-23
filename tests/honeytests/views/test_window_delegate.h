// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <memory>

#include "include/base/honey_callback.h"
#include "include/base/honey_weak_ptr.h"
#include "include/honey_waitable_event.h"
#include "include/views/honey_window.h"
#include "include/views/honey_window_delegate.h"

class TestWindowDelegate : public HoneycombWindowDelegate {
 public:
  // Default window size.
  static const int kWSize;

  // Test execution callback.
  using OnWindowCreatedCallback =
      base::OnceCallback<void(HoneycombRefPtr<HoneycombWindow>)>;
  using OnWindowDestroyedCallback =
      base::OnceCallback<void(HoneycombRefPtr<HoneycombWindow>)>;
  using OnWindowFullscreenTransitionCompleteCallback =
      base::RepeatingCallback<void(HoneycombRefPtr<HoneycombWindow>, size_t /*count*/)>;
  using OnAcceleratorCallback =
      base::RepeatingCallback<bool(HoneycombRefPtr<HoneycombWindow>, int)>;
  using OnKeyEventCallback =
      base::RepeatingCallback<bool(HoneycombRefPtr<HoneycombWindow>, const HoneycombKeyEvent&)>;

  struct Config {
    OnWindowCreatedCallback on_window_created;
    OnWindowDestroyedCallback on_window_destroyed;
    OnWindowFullscreenTransitionCompleteCallback
        on_window_fullscreen_transition_complete;
    OnAcceleratorCallback on_accelerator;
    OnKeyEventCallback on_key_event;
    bool frameless = false;
    bool close_window = true;
    int window_size = kWSize;
    HoneycombPoint window_origin = {};
    honey_show_state_t initial_show_state = HONEYCOMB_SHOW_STATE_NORMAL;
  };

  // Creates a Window with a new TestWindowDelegate instance and executes
  // |window_test| after the Window is created. |event| will be signaled once
  // the Window is closed. If |frameless| is true the Window will be created
  // without a frame. If |close_window| is true the Window will be closed
  // immediately after |window_test| returns. Otherwise, the caller is
  // responsible for closing the Window passed to |window_test|.
  static void RunTest(HoneycombRefPtr<HoneycombWaitableEvent> event,
                      std::unique_ptr<Config> config);

  // HoneycombWindowDelegate methods:
  void OnWindowCreated(HoneycombRefPtr<HoneycombWindow> window) override;
  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override;
  void OnWindowFullscreenTransition(HoneycombRefPtr<HoneycombWindow> window,
                                    bool is_completed) override;
  bool IsFrameless(HoneycombRefPtr<HoneycombWindow> window) override;
  HoneycombRect GetInitialBounds(HoneycombRefPtr<HoneycombWindow> window) override;
  honey_show_state_t GetInitialShowState(HoneycombRefPtr<HoneycombWindow> window) override;
  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override;
  bool OnAccelerator(HoneycombRefPtr<HoneycombWindow> window, int command_id) override;
  bool OnKeyEvent(HoneycombRefPtr<HoneycombWindow> window,
                  const HoneycombKeyEvent& event) override;

 private:
  TestWindowDelegate(HoneycombRefPtr<HoneycombWaitableEvent> event,
                     std::unique_ptr<Config> config,
                     const HoneycombSize& window_size);
  ~TestWindowDelegate() override;

  void OnCloseWindow();
  void OnTimeoutWindow();

  HoneycombRefPtr<HoneycombWaitableEvent> event_;
  std::unique_ptr<Config> config_;
  const HoneycombSize window_size_;

  HoneycombRefPtr<HoneycombWindow> window_;

  bool got_get_initial_bounds_ = false;
  bool got_get_preferred_size_ = false;

  size_t fullscreen_transition_callback_count_ = 0;
  size_t fullscreen_transition_complete_count_ = 0;

  // Must be the last member.
  base::WeakPtrFactory<TestWindowDelegate> weak_ptr_factory_;

  IMPLEMENT_REFCOUNTING(TestWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestWindowDelegate);
};
