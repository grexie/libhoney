// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/honey_pack_strings.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_panel_delegate.h"
#include "include/views/honey_scroll_view.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/thread_helper.h"
#include "tests/honeytests/views/test_window_delegate.h"
#include "tests/gtest/include/gtest/gtest.h"

#define SCROLL_VIEW_TEST_ASYNC(name) \
  UI_THREAD_TEST_ASYNC(ViewsScrollViewTest, name)

namespace {

const int kScrollViewID = 1;
const int kContentPanelID = 2;

// Make the Panel larger then the Window so scroll bars appear.
const int kContentPanelSize = TestWindowDelegate::kWSize + 200;

class TestScrollViewDelegate : public HoneycombViewDelegate {
 public:
  TestScrollViewDelegate() {}

  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override {
    EXPECT_EQ(kScrollViewID, view->GetID());
    got_get_preferred_size_ = true;
    return HoneycombSize(kContentPanelSize, kContentPanelSize);
  }

  bool got_get_preferred_size_ = false;

 private:
  IMPLEMENT_REFCOUNTING(TestScrollViewDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestScrollViewDelegate);
};

class TestPanelDelegate : public HoneycombPanelDelegate {
 public:
  TestPanelDelegate() {}

  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override {
    EXPECT_EQ(kContentPanelID, view->GetID());
    got_get_preferred_size_ = true;
    return HoneycombSize(kContentPanelSize, kContentPanelSize);
  }

  bool got_get_preferred_size_ = false;

 private:
  IMPLEMENT_REFCOUNTING(TestPanelDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestPanelDelegate);
};

void RunScrollViewLayout(bool with_delegate, HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<TestScrollViewDelegate> scroll_view_delegate;
  HoneycombRefPtr<TestPanelDelegate> panel_delegate;
  if (with_delegate) {
    scroll_view_delegate = new TestScrollViewDelegate();
    panel_delegate = new TestPanelDelegate();
  }

  HoneycombRefPtr<HoneycombScrollView> scroll_view =
      HoneycombScrollView::CreateScrollView(scroll_view_delegate);
  EXPECT_TRUE(scroll_view.get());
  EXPECT_TRUE(scroll_view->AsScrollView().get());

  // Verify default state.
  EXPECT_FALSE(scroll_view->GetContentView().get());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), scroll_view->GetVisibleContentRect());
  EXPECT_FALSE(scroll_view->HasHorizontalScrollbar());
  EXPECT_FALSE(scroll_view->HasVerticalScrollbar());

  scroll_view->SetID(kScrollViewID);
  scroll_view->SetBackgroundColor(HoneycombColorSetARGB(255, 0, 255, 0));

  HoneycombRefPtr<HoneycombPanel> content_panel = HoneycombPanel::CreatePanel(panel_delegate);
  content_panel->SetID(kContentPanelID);
  content_panel->SetBackgroundColor(HoneycombColorSetARGB(255, 255, 0, 0));

  if (!with_delegate) {
    // Explicitly set the content panel size. Otherwise, it will come from the
    // delegate.
    content_panel->SetSize(HoneycombSize(kContentPanelSize, kContentPanelSize));
  }

  scroll_view->SetContentView(content_panel);
  EXPECT_TRUE(content_panel->IsSame(scroll_view->GetContentView()));

  window->AddChildView(scroll_view);

  // Force layout.
  window->Layout();

  EXPECT_TRUE(scroll_view->HasHorizontalScrollbar());
  EXPECT_TRUE(scroll_view->HasVerticalScrollbar());

  if (with_delegate) {
    // Layout() of the ScrollView no longer needs to call us for the size,
    // see https://crrev.com/2701734b44.
    EXPECT_FALSE(scroll_view_delegate->got_get_preferred_size_);
    EXPECT_TRUE(panel_delegate->got_get_preferred_size_);
  }

  window->Show();

  // With default FillLayout the ScrollView should be the size of the Window's
  // client area.
  const HoneycombRect& client_bounds = window->GetClientAreaBoundsInScreen();
  const HoneycombRect& scroll_view_bounds = scroll_view->GetBoundsInScreen();
  EXPECT_EQ(client_bounds, scroll_view_bounds);

  // Content panel size should be unchanged.
  const HoneycombSize& content_panel_size = content_panel->GetSize();
  EXPECT_EQ(HoneycombSize(kContentPanelSize, kContentPanelSize), content_panel_size);

  const int sb_height = scroll_view->GetHorizontalScrollbarHeight();
  EXPECT_GT(sb_height, 0);
  const int sb_width = scroll_view->GetVerticalScrollbarWidth();
  EXPECT_GT(sb_width, 0);

  // Verify visible content panel region.
  const HoneycombRect& visible_rect = scroll_view->GetVisibleContentRect();
  EXPECT_EQ(HoneycombRect(0, 0, scroll_view_bounds.width - sb_width,
                    scroll_view_bounds.height - sb_height),
            visible_rect);
}

void ScrollViewLayout(HoneycombRefPtr<HoneycombWaitableEvent> event, bool with_delegate) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created =
      base::BindOnce(RunScrollViewLayout, with_delegate);
  TestWindowDelegate::RunTest(event, std::move(config));
}

void ScrollViewLayoutWithDelegateImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  ScrollViewLayout(event, true);
}

void ScrollViewLayoutNoDelegateImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  ScrollViewLayout(event, false);
}

}  // namespace

// Test ScrollView layout. This is primarily to exercise exposed Honeycomb APIs and is
// not intended to comprehensively test ScrollView-related behavior (which we
// presume that Chromium is testing).
SCROLL_VIEW_TEST_ASYNC(ScrollViewLayoutWithDelegate)
SCROLL_VIEW_TEST_ASYNC(ScrollViewLayoutNoDelegate)
