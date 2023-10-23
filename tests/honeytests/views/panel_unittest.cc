// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/views/honey_box_layout.h"
#include "include/views/honey_fill_layout.h"
#include "include/views/honey_layout.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_panel_delegate.h"
#include "include/views/honey_window.h"
#include "tests/honeytests/thread_helper.h"
#include "tests/gtest/include/gtest/gtest.h"

#define PANEL_TEST(name) UI_THREAD_TEST(ViewsPanelTest, name)

namespace {

class EmptyPanelDelegate : public HoneycombPanelDelegate {
 public:
  EmptyPanelDelegate() {}

 private:
  IMPLEMENT_REFCOUNTING(EmptyPanelDelegate);
  DISALLOW_COPY_AND_ASSIGN(EmptyPanelDelegate);
};

void CreatePanel(HoneycombRefPtr<HoneycombPanelDelegate> delegate) {
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(delegate);
  EXPECT_TRUE(panel.get());

  // Verify the derived View relationship.
  EXPECT_TRUE(panel->AsPanel().get());
  EXPECT_FALSE(panel->AsWindow().get());
  EXPECT_TRUE(panel->IsSame(panel));

  // Verify default View state.
  EXPECT_STREQ("Panel", panel->GetTypeString().ToString().c_str());
  EXPECT_TRUE(panel->IsValid());
  EXPECT_FALSE(panel->IsAttached());
  if (delegate) {
    EXPECT_EQ(delegate.get(), panel->GetDelegate().get());
  } else {
    EXPECT_FALSE(panel->GetDelegate().get());
  }
  EXPECT_EQ(0, panel->GetID());
  EXPECT_FALSE(panel->GetParentView().get());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel->GetBounds());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel->GetBoundsInScreen());
  EXPECT_EQ(HoneycombSize(0, 0), panel->GetPreferredSize());
  EXPECT_EQ(HoneycombSize(0, 0), panel->GetMinimumSize());
  EXPECT_EQ(HoneycombSize(0, 0), panel->GetMaximumSize());
  EXPECT_EQ(HoneycombSize(0, 0), panel->GetMaximumSize());
  EXPECT_EQ(0, panel->GetHeightForWidth(100));
  EXPECT_TRUE(panel->IsVisible());
  EXPECT_FALSE(panel->IsDrawn());
  EXPECT_TRUE(panel->IsEnabled());
  EXPECT_FALSE(panel->IsFocusable());
  EXPECT_FALSE(panel->IsAccessibilityFocusable());
  EXPECT_EQ(HoneycombColorSetARGB(255, 255, 255, 255), panel->GetBackgroundColor());

  // Verify default Panel state.
  EXPECT_TRUE(panel->GetLayout().get());
  EXPECT_EQ(0U, panel->GetChildViewCount());

  // Destroy the Panel.
  panel = nullptr;

  if (delegate) {
    // Verify that nothing is keeping a reference to the delegate.
    EXPECT_TRUE(delegate->HasOneRef());
  }
}

void CreatePanelNoDelegateImpl() {
  CreatePanel(nullptr);
}

void CreatePanelWithDelegateImpl() {
  CreatePanel(new EmptyPanelDelegate);
}

}  // namespace

// Test creation.
PANEL_TEST(CreatePanelNoDelegate)
PANEL_TEST(CreatePanelWithDelegate)

namespace {

class ParentPanelDelegate : public HoneycombPanelDelegate {
 public:
  ParentPanelDelegate() {}

  void OnParentViewChanged(HoneycombRefPtr<HoneycombView> view,
                           bool added,
                           HoneycombRefPtr<HoneycombView> parent) override {
    EXPECT_FALSE(true);  // Not reached.
  }

  void OnChildViewChanged(HoneycombRefPtr<HoneycombView> view,
                          bool added,
                          HoneycombRefPtr<HoneycombView> child) override {
    Changed changed;
    changed.view_ = view;
    changed.added_ = added;
    changed.child_ = child;
    changed_.push_back(changed);
  }

  void Verify(int callback_index,
              HoneycombRefPtr<HoneycombView> view,
              bool added,
              HoneycombRefPtr<HoneycombView> child) {
    EXPECT_LT(callback_index, static_cast<int>(changed_.size()));
    EXPECT_TRUE(view->IsSame(changed_[callback_index].view_))
        << "callback_index " << callback_index;
    EXPECT_EQ(added, changed_[callback_index].added_)
        << "callback_index " << callback_index;
    EXPECT_TRUE(child->IsSame(changed_[callback_index].child_))
        << "callback_index " << callback_index;
  }

  void Reset() { changed_.clear(); }

  bool IsReset() const { return changed_.empty(); }

  struct Changed {
    HoneycombRefPtr<HoneycombView> view_;
    bool added_ = false;
    HoneycombRefPtr<HoneycombView> child_;
  };
  std::vector<Changed> changed_;

 private:
  IMPLEMENT_REFCOUNTING(ParentPanelDelegate);
  DISALLOW_COPY_AND_ASSIGN(ParentPanelDelegate);
};

class ChildPanelDelegate : public HoneycombPanelDelegate {
 public:
  ChildPanelDelegate() {}

  void OnParentViewChanged(HoneycombRefPtr<HoneycombView> view,
                           bool added,
                           HoneycombRefPtr<HoneycombView> parent) override {
    EXPECT_FALSE(on_parent_view_changed_);
    on_parent_view_changed_ = true;
    view_ = view;
    added_ = added;
    parent_ = parent;
  }

  void OnChildViewChanged(HoneycombRefPtr<HoneycombView> view,
                          bool added,
                          HoneycombRefPtr<HoneycombView> child) override {
    EXPECT_FALSE(true);  // Not reached.
  }

  void Verify(HoneycombRefPtr<HoneycombView> view, bool added, HoneycombRefPtr<HoneycombView> parent) {
    EXPECT_TRUE(on_parent_view_changed_);
    EXPECT_TRUE(view->IsSame(view_));
    EXPECT_EQ(added, added_);
    EXPECT_TRUE(parent->IsSame(parent_));
  }

  void Reset() {
    on_parent_view_changed_ = false;
    view_ = nullptr;
    added_ = false;
    parent_ = nullptr;
  }

  bool IsReset() const { return !on_parent_view_changed_; }

  bool on_parent_view_changed_ = false;
  HoneycombRefPtr<HoneycombView> view_;
  bool added_ = false;
  HoneycombRefPtr<HoneycombView> parent_;

 private:
  IMPLEMENT_REFCOUNTING(ChildPanelDelegate);
  DISALLOW_COPY_AND_ASSIGN(ChildPanelDelegate);
};

void ChildVerifyRemovedState(HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
                             HoneycombRefPtr<HoneycombPanel> parent_panel,
                             HoneycombRefPtr<ChildPanelDelegate> child_delegate,
                             HoneycombRefPtr<HoneycombPanel> child_panel) {
  EXPECT_FALSE(parent_panel->IsSame(child_panel));
  EXPECT_FALSE(child_panel->IsSame(parent_panel));
  EXPECT_FALSE(parent_panel->IsAttached());
  EXPECT_FALSE(child_panel->IsAttached());
  EXPECT_FALSE(parent_panel->GetParentView().get());
  EXPECT_FALSE(child_panel->GetParentView().get());
}

void ChildVerifyAddedState(HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
                           HoneycombRefPtr<HoneycombPanel> parent_panel,
                           HoneycombRefPtr<ChildPanelDelegate> child_delegate,
                           HoneycombRefPtr<HoneycombPanel> child_panel,
                           int expected_child_index) {
  EXPECT_FALSE(parent_panel->IsSame(child_panel));
  EXPECT_FALSE(child_panel->IsSame(parent_panel));
  EXPECT_FALSE(parent_panel->IsAttached());
  EXPECT_TRUE(child_panel->IsAttached());
  EXPECT_TRUE(
      child_panel->IsSame(parent_panel->GetChildViewAt(expected_child_index)));
  EXPECT_TRUE(child_panel->GetParentView()->IsSame(parent_panel));
}

void ChildVerifyFinalCallbackState(
    HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
    HoneycombRefPtr<HoneycombPanel> parent_panel,
    HoneycombRefPtr<ChildPanelDelegate> child_delegate,
    HoneycombRefPtr<HoneycombPanel> child_panel,
    int expected_parent_callback_index,
    bool added) {
  parent_delegate->Verify(expected_parent_callback_index, parent_panel, added,
                          child_panel);
  child_delegate->Verify(child_panel, added, parent_panel);
}

void ChildAdd(HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
              HoneycombRefPtr<HoneycombPanel> parent_panel,
              HoneycombRefPtr<ChildPanelDelegate> child_delegate,
              HoneycombRefPtr<HoneycombPanel> child_panel,
              int expected_child_index = 0,
              int expected_parent_callback_index = 0) {
  // Verify initial parent/child state.
  ChildVerifyRemovedState(parent_delegate, parent_panel, child_delegate,
                          child_panel);

  // Verify initial child callback state.
  EXPECT_TRUE(child_delegate->IsReset());

  // Add the child view.
  parent_panel->AddChildView(child_panel);

  // Verify final callback state.
  ChildVerifyFinalCallbackState(parent_delegate, parent_panel, child_delegate,
                                child_panel, expected_parent_callback_index,
                                true);

  // Reset child callback state.
  child_delegate->Reset();

  // Verify final parent/child state.
  ChildVerifyAddedState(parent_delegate, parent_panel, child_delegate,
                        child_panel, expected_child_index);
}

void ChildAddAt(HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
                HoneycombRefPtr<HoneycombPanel> parent_panel,
                HoneycombRefPtr<ChildPanelDelegate> child_delegate,
                HoneycombRefPtr<HoneycombPanel> child_panel,
                int child_index,
                int expected_parent_callback_index) {
  // Verify initial parent/child state.
  ChildVerifyRemovedState(parent_delegate, parent_panel, child_delegate,
                          child_panel);

  // Verify initial child callback state.
  EXPECT_TRUE(child_delegate->IsReset());

  // Add the child view.
  parent_panel->AddChildViewAt(child_panel, child_index);

  // Verify final callback state.
  ChildVerifyFinalCallbackState(parent_delegate, parent_panel, child_delegate,
                                child_panel, expected_parent_callback_index,
                                true);

  // Reset child callback state.
  child_delegate->Reset();

  // Verify final parent/child state.
  ChildVerifyAddedState(parent_delegate, parent_panel, child_delegate,
                        child_panel, child_index);
}

void ChildRemove(HoneycombRefPtr<ParentPanelDelegate> parent_delegate,
                 HoneycombRefPtr<HoneycombPanel> parent_panel,
                 HoneycombRefPtr<ChildPanelDelegate> child_delegate,
                 HoneycombRefPtr<HoneycombPanel> child_panel,
                 bool remove_all,
                 int expected_child_index = 0,
                 int expected_parent_callback_index = 0) {
  // Verify initial parent/child state.
  ChildVerifyAddedState(parent_delegate, parent_panel, child_delegate,
                        child_panel, expected_child_index);

  // Verify initial child callback state.
  EXPECT_TRUE(child_delegate->IsReset());

  // Remove the child view.
  if (remove_all) {
    parent_panel->RemoveAllChildViews();
  } else {
    parent_panel->RemoveChildView(child_panel);
  }

  // Verify final callback state.
  ChildVerifyFinalCallbackState(parent_delegate, parent_panel, child_delegate,
                                child_panel, expected_parent_callback_index,
                                false);

  // Reset child callback state.
  child_delegate->Reset();

  // Verify final parent/child state.
  ChildVerifyRemovedState(parent_delegate, parent_panel, child_delegate,
                          child_panel);
}

void ChildAddRemoveSingleImpl() {
  HoneycombRefPtr<ParentPanelDelegate> parent_delegate = new ParentPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> parent_panel = HoneycombPanel::CreatePanel(parent_delegate);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel = HoneycombPanel::CreatePanel(child_delegate);

  // Add and explicitly remove the child view.
  EXPECT_TRUE(parent_delegate->IsReset());
  ChildAdd(parent_delegate, parent_panel, child_delegate, child_panel);
  parent_delegate->Reset();

  ChildRemove(parent_delegate, parent_panel, child_delegate, child_panel,
              false);
  parent_delegate->Reset();

  // Add and implicitly remove the child view.
  ChildAdd(parent_delegate, parent_panel, child_delegate, child_panel);
  parent_delegate->Reset();

  ChildRemove(parent_delegate, parent_panel, child_delegate, child_panel,
              false);
  parent_delegate->Reset();
}

void ChildAddRemoveMultipleImpl() {
  HoneycombRefPtr<ParentPanelDelegate> parent_delegate = new ParentPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> parent_panel = HoneycombPanel::CreatePanel(parent_delegate);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate1 = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel1 = HoneycombPanel::CreatePanel(child_delegate1);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate2 = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel2 = HoneycombPanel::CreatePanel(child_delegate2);

  // Add multiple child views.
  EXPECT_TRUE(parent_delegate->IsReset());
  ChildAdd(parent_delegate, parent_panel, child_delegate1, child_panel1, 0, 0);
  EXPECT_TRUE(child_delegate2->IsReset());  // child2 not called.
  ChildAdd(parent_delegate, parent_panel, child_delegate2, child_panel2, 1, 1);
  EXPECT_TRUE(child_delegate1->IsReset());  // child1 not called.
  parent_delegate->Reset();

  EXPECT_EQ(2U, parent_panel->GetChildViewCount());

  // Explicitly remove specific child views.
  ChildRemove(parent_delegate, parent_panel, child_delegate1, child_panel1,
              false, 0, 0);
  EXPECT_TRUE(child_delegate2->IsReset());  // child2 not called.
  ChildRemove(parent_delegate, parent_panel, child_delegate2, child_panel2,
              false, 0, 1);
  EXPECT_TRUE(child_delegate1->IsReset());  // child1 not called.
  parent_delegate->Reset();

  EXPECT_EQ(0U, parent_panel->GetChildViewCount());

  // Add multiple child views.
  ChildAdd(parent_delegate, parent_panel, child_delegate1, child_panel1, 0, 0);
  EXPECT_TRUE(child_delegate2->IsReset());  // child2 not called.
  ChildAdd(parent_delegate, parent_panel, child_delegate2, child_panel2, 1, 1);
  EXPECT_TRUE(child_delegate1->IsReset());  // child1 not called.
  parent_delegate->Reset();

  EXPECT_EQ(2U, parent_panel->GetChildViewCount());

  EXPECT_TRUE(child_delegate1->IsReset());
  EXPECT_TRUE(child_delegate2->IsReset());

  // Implicitly remove all child views.
  parent_panel->RemoveAllChildViews();

  // Verify final callback state.
  ChildVerifyFinalCallbackState(parent_delegate, parent_panel, child_delegate1,
                                child_panel1, 0, false);
  ChildVerifyFinalCallbackState(parent_delegate, parent_panel, child_delegate2,
                                child_panel2, 1, false);

  EXPECT_EQ(0U, parent_panel->GetChildViewCount());

  // Reset callback state.
  parent_delegate->Reset();
  child_delegate1->Reset();
  child_delegate2->Reset();

  // Verify final parent/child state.
  ChildVerifyRemovedState(parent_delegate, parent_panel, child_delegate1,
                          child_panel1);
  ChildVerifyRemovedState(parent_delegate, parent_panel, child_delegate2,
                          child_panel2);
}

void ChildOrderImpl() {
  HoneycombRefPtr<ParentPanelDelegate> parent_delegate = new ParentPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> parent_panel = HoneycombPanel::CreatePanel(parent_delegate);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate1 = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel1 = HoneycombPanel::CreatePanel(child_delegate1);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate2 = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel2 = HoneycombPanel::CreatePanel(child_delegate2);

  HoneycombRefPtr<ChildPanelDelegate> child_delegate3 = new ChildPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> child_panel3 = HoneycombPanel::CreatePanel(child_delegate3);

  // Add child views at specific indexes.
  ChildAddAt(parent_delegate, parent_panel, child_delegate2, child_panel2, 0,
             0);
  ChildAddAt(parent_delegate, parent_panel, child_delegate3, child_panel3, 0,
             1);
  ChildAddAt(parent_delegate, parent_panel, child_delegate1, child_panel1, 1,
             2);
  parent_delegate->Reset();

  EXPECT_EQ(3U, parent_panel->GetChildViewCount());

  // ChildAddAt() will verify these results but let's check again just to make
  // sure.
  EXPECT_TRUE(child_panel3->IsSame(parent_panel->GetChildViewAt(0)));
  EXPECT_TRUE(child_panel1->IsSame(parent_panel->GetChildViewAt(1)));
  EXPECT_TRUE(child_panel2->IsSame(parent_panel->GetChildViewAt(2)));

  // Move panel2 to the front.
  parent_panel->ReorderChildView(child_panel2, 0);

  EXPECT_TRUE(child_panel2->IsSame(parent_panel->GetChildViewAt(0)));
  EXPECT_TRUE(child_panel3->IsSame(parent_panel->GetChildViewAt(1)));
  EXPECT_TRUE(child_panel1->IsSame(parent_panel->GetChildViewAt(2)));

  // Move panel3 to the end.
  parent_panel->ReorderChildView(child_panel3, -1);

  EXPECT_TRUE(child_panel2->IsSame(parent_panel->GetChildViewAt(0)));
  EXPECT_TRUE(child_panel1->IsSame(parent_panel->GetChildViewAt(1)));
  EXPECT_TRUE(child_panel3->IsSame(parent_panel->GetChildViewAt(2)));
}

void ChildVisibleImpl() {
  HoneycombRefPtr<HoneycombPanel> parent_panel = HoneycombPanel::CreatePanel(nullptr);
  HoneycombRefPtr<HoneycombPanel> child_panel1 = HoneycombPanel::CreatePanel(nullptr);
  HoneycombRefPtr<HoneycombPanel> child_panel2 = HoneycombPanel::CreatePanel(nullptr);

  // Nothing drawn by default.
  EXPECT_FALSE(parent_panel->IsDrawn());
  EXPECT_FALSE(child_panel1->IsDrawn());
  EXPECT_FALSE(child_panel2->IsDrawn());

  // Everything visible by default.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  parent_panel->AddChildView(child_panel1);
  parent_panel->AddChildView(child_panel2);

  // Still the same.
  EXPECT_FALSE(parent_panel->IsDrawn());
  EXPECT_FALSE(child_panel1->IsDrawn());
  EXPECT_FALSE(child_panel2->IsDrawn());
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  child_panel1->SetVisible(false);

  // Child1 not visible.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_FALSE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  child_panel1->SetVisible(true);

  // Everything visible.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  parent_panel->SetVisible(false);

  // Children visible.
  EXPECT_FALSE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  parent_panel->SetVisible(true);

  // Everything visible.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());
}

void ChildDrawnImpl() {
  HoneycombRefPtr<HoneycombPanel> parent_panel = HoneycombPanel::CreatePanel(nullptr);
  HoneycombRefPtr<HoneycombPanel> child_panel1 = HoneycombPanel::CreatePanel(nullptr);
  HoneycombRefPtr<HoneycombPanel> child_panel2 = HoneycombPanel::CreatePanel(nullptr);

  // Nothing drawn by default.
  EXPECT_FALSE(parent_panel->IsDrawn());
  EXPECT_FALSE(child_panel1->IsDrawn());
  EXPECT_FALSE(child_panel2->IsDrawn());

  // Everything visible by default.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel2->IsVisible());

  parent_panel->AddChildView(child_panel1);
  parent_panel->AddChildView(child_panel2);

  // Create and show a Window.
  HoneycombRefPtr<HoneycombWindow> window = HoneycombWindow::CreateTopLevelWindow(nullptr);
  window->AddChildView(parent_panel);
  window->CenterWindow(HoneycombSize(400, 400));
  window->Show();

  // Everything visible and drawn now.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(parent_panel->IsDrawn());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel1->IsDrawn());
  EXPECT_TRUE(child_panel2->IsVisible());
  EXPECT_TRUE(child_panel2->IsDrawn());

  child_panel1->SetVisible(false);

  // Child1 not visible or drawn.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(parent_panel->IsDrawn());
  EXPECT_FALSE(child_panel1->IsVisible());
  EXPECT_FALSE(child_panel1->IsDrawn());
  EXPECT_TRUE(child_panel2->IsVisible());
  EXPECT_TRUE(child_panel2->IsDrawn());

  child_panel1->SetVisible(true);

  // Everything visible and drawn.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(parent_panel->IsDrawn());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel1->IsDrawn());
  EXPECT_TRUE(child_panel2->IsVisible());
  EXPECT_TRUE(child_panel2->IsDrawn());

  parent_panel->SetVisible(false);

  // Children visible, but nothing drawn.
  EXPECT_FALSE(parent_panel->IsVisible());
  EXPECT_FALSE(parent_panel->IsDrawn());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_FALSE(child_panel1->IsDrawn());
  EXPECT_TRUE(child_panel2->IsVisible());
  EXPECT_FALSE(child_panel2->IsDrawn());

  parent_panel->SetVisible(true);

  // Everything visible and drawn.
  EXPECT_TRUE(parent_panel->IsVisible());
  EXPECT_TRUE(parent_panel->IsDrawn());
  EXPECT_TRUE(child_panel1->IsVisible());
  EXPECT_TRUE(child_panel1->IsDrawn());
  EXPECT_TRUE(child_panel2->IsVisible());
  EXPECT_TRUE(child_panel2->IsDrawn());

  // Close the window.
  window->Close();
}

}  // namespace

// Test child behaviors.
PANEL_TEST(ChildAddRemoveSingle)
PANEL_TEST(ChildAddRemoveMultiple)
PANEL_TEST(ChildOrder)
PANEL_TEST(ChildVisible)
PANEL_TEST(ChildDrawn)

namespace {

class SizingPanelDelegate : public HoneycombPanelDelegate {
 public:
  SizingPanelDelegate() {}

  HoneycombSize GetPreferredSize(HoneycombRefPtr<HoneycombView> view) override {
    got_get_preferred_size_ = true;
    view_ = view;
    return preferred_size_;
  }

  HoneycombSize GetMinimumSize(HoneycombRefPtr<HoneycombView> view) override {
    got_get_minimum_size_ = true;
    view_ = view;
    return minimum_size_;
  }

  HoneycombSize GetMaximumSize(HoneycombRefPtr<HoneycombView> view) override {
    got_get_maximum_size_ = true;
    view_ = view;
    return maximum_size_;
  }

  int GetHeightForWidth(HoneycombRefPtr<HoneycombView> view, int width) override {
    got_get_height_for_width_ = true;
    view_ = view;
    width_ = width;
    return height_for_width_;
  }

  void Reset() {
    preferred_size_ = HoneycombSize(0, 0);
    minimum_size_ = HoneycombSize(0, 0);
    maximum_size_ = HoneycombSize(0, 0);
    height_for_width_ = 0;
    got_get_preferred_size_ = false;
    got_get_minimum_size_ = false;
    got_get_maximum_size_ = false;
    got_get_height_for_width_ = false;
    view_ = nullptr;
    width_ = 0;
  }

  bool IsReset() const {
    return !got_get_preferred_size_ && !got_get_minimum_size_ &&
           !got_get_maximum_size_ && !got_get_height_for_width_;
  }

  HoneycombSize preferred_size_;
  HoneycombSize minimum_size_;
  HoneycombSize maximum_size_;
  int height_for_width_ = 0;

  bool got_get_preferred_size_ = false;
  bool got_get_minimum_size_ = false;
  bool got_get_maximum_size_ = false;
  bool got_get_height_for_width_ = false;

  HoneycombRefPtr<HoneycombView> view_;
  int width_ = 0;

 private:
  IMPLEMENT_REFCOUNTING(SizingPanelDelegate);
  DISALLOW_COPY_AND_ASSIGN(SizingPanelDelegate);
};

void SizeNoDelegateImpl() {
  HoneycombRefPtr<SizingPanelDelegate> delegate = new SizingPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(delegate);

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel->GetBounds());

  // Set and get the bounds.
  panel->SetBounds(HoneycombRect(100, 100, 200, 200));
  EXPECT_EQ(HoneycombRect(100, 100, 200, 200), panel->GetBounds());
  EXPECT_EQ(HoneycombSize(200, 200), panel->GetSize());
  EXPECT_EQ(HoneycombPoint(100, 100), panel->GetPosition());

  // GetBoundsInScreen() drops the position because there is no Window.
  EXPECT_EQ(HoneycombRect(0, 0, 200, 200), panel->GetBoundsInScreen());

  // Adjust the position but keep the size the same.
  panel->SetPosition(HoneycombPoint(50, 50));
  EXPECT_EQ(HoneycombRect(50, 50, 200, 200), panel->GetBounds());
  EXPECT_EQ(HoneycombSize(200, 200), panel->GetSize());
  EXPECT_EQ(HoneycombPoint(50, 50), panel->GetPosition());

  // Adjust the size but keep the position the same.
  panel->SetSize(HoneycombSize(400, 400));
  EXPECT_EQ(HoneycombRect(50, 50, 400, 400), panel->GetBounds());
  EXPECT_EQ(HoneycombSize(400, 400), panel->GetSize());
  EXPECT_EQ(HoneycombPoint(50, 50), panel->GetPosition());

  // No delegate methods were called during this test.
  EXPECT_TRUE(delegate->IsReset());
}

void SizeWithDelegateImpl() {
  HoneycombRefPtr<SizingPanelDelegate> delegate = new SizingPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(delegate);

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel->GetBounds());

  HoneycombSize expected_size(100, 100);

  // Test GetPreferredSize().
  delegate->preferred_size_ = expected_size;
  EXPECT_EQ(expected_size, panel->GetPreferredSize());
  EXPECT_TRUE(delegate->got_get_preferred_size_);
  EXPECT_FALSE(delegate->got_get_minimum_size_);
  EXPECT_FALSE(delegate->got_get_maximum_size_);
  EXPECT_FALSE(delegate->got_get_height_for_width_);
  EXPECT_TRUE(panel->IsSame(delegate->view_));
  delegate->Reset();

  // Test GetMinimumSize().
  delegate->minimum_size_ = expected_size;
  EXPECT_EQ(expected_size, panel->GetMinimumSize());
  EXPECT_FALSE(delegate->got_get_preferred_size_);
  EXPECT_TRUE(delegate->got_get_minimum_size_);
  EXPECT_FALSE(delegate->got_get_maximum_size_);
  EXPECT_FALSE(delegate->got_get_height_for_width_);
  EXPECT_TRUE(panel->IsSame(delegate->view_));
  delegate->Reset();

  // Test GetMaximumSize().
  delegate->maximum_size_ = expected_size;
  EXPECT_EQ(expected_size, panel->GetMaximumSize());
  EXPECT_FALSE(delegate->got_get_preferred_size_);
  EXPECT_FALSE(delegate->got_get_minimum_size_);
  EXPECT_TRUE(delegate->got_get_maximum_size_);
  EXPECT_FALSE(delegate->got_get_height_for_width_);
  EXPECT_TRUE(panel->IsSame(delegate->view_));
  delegate->Reset();

  int expected_width = 200;
  int expected_height = 100;

  // Test GetHeightForWidth().
  delegate->height_for_width_ = expected_height;
  EXPECT_EQ(expected_height, panel->GetHeightForWidth(expected_width));
  EXPECT_FALSE(delegate->got_get_preferred_size_);
  EXPECT_FALSE(delegate->got_get_minimum_size_);
  EXPECT_FALSE(delegate->got_get_maximum_size_);
  EXPECT_TRUE(delegate->got_get_height_for_width_);
  EXPECT_EQ(expected_width, delegate->width_);
  EXPECT_TRUE(panel->IsSame(delegate->view_));
  delegate->Reset();
}

}  // namespace

// Test sizing.
PANEL_TEST(SizeNoDelegate)
PANEL_TEST(SizeWithDelegate)

namespace {

void FillLayoutCreateImpl() {
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(nullptr);

  // Explicitly set to FillLayout.
  panel->SetToFillLayout();

  HoneycombRefPtr<HoneycombLayout> layout = panel->GetLayout();
  EXPECT_TRUE(layout.get());
  EXPECT_TRUE(layout->AsFillLayout().get());
}

void FillLayoutSizeToPreferredSizeImpl() {
  HoneycombRefPtr<SizingPanelDelegate> delegate = new SizingPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(delegate);

  // Default Layout is FillLayout.
  HoneycombRefPtr<HoneycombLayout> layout = panel->GetLayout();
  EXPECT_TRUE(layout.get());
  EXPECT_TRUE(layout->AsFillLayout().get());

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel->GetBounds());

  HoneycombSize expected_size(100, 100);

  delegate->preferred_size_ = expected_size;

  // Trigger use of the preferred size.
  panel->Layout();

  EXPECT_TRUE(delegate->got_get_preferred_size_);
  EXPECT_FALSE(delegate->got_get_minimum_size_);
  EXPECT_FALSE(delegate->got_get_maximum_size_);
  EXPECT_FALSE(delegate->got_get_height_for_width_);
  EXPECT_TRUE(panel->IsSame(delegate->view_));
  delegate->Reset();

  // Size is now the preferred size.
  EXPECT_EQ(expected_size, panel->GetSize());

  // No additional delegate methods were called.
  EXPECT_TRUE(delegate->IsReset());
}

void FillLayoutSizeHierarchyImpl() {
  HoneycombRefPtr<HoneycombPanel> panel_parent = HoneycombPanel::CreatePanel(nullptr);
  HoneycombRefPtr<HoneycombPanel> panel_child = HoneycombPanel::CreatePanel(nullptr);

  HoneycombSize expected_size(100, 100);

  // Default Layout is FillLayout.
  HoneycombRefPtr<HoneycombLayout> layout1 = panel_parent->GetLayout();
  EXPECT_TRUE(layout1.get());
  EXPECT_TRUE(layout1->AsFillLayout().get());

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_parent->GetBounds());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_child->GetBounds());

  // Without delegates the size must be set on the parent.
  panel_parent->SetSize(expected_size);

  // FillLayout is the default Layout. Both panels should end up with the same
  // size.
  panel_parent->AddChildView(panel_child);

  // Force layout.
  panel_parent->Layout();

  // Panels are now the same size.
  EXPECT_EQ(expected_size, panel_parent->GetSize());
  EXPECT_EQ(expected_size, panel_child->GetSize());

  // Resize the parent panel to a larger size.
  HoneycombSize expected_size2(200, 200);
  panel_parent->SetSize(expected_size2);

  // Force layout.
  panel_parent->Layout();

  // Panels are now the larger size.
  EXPECT_EQ(expected_size2, panel_parent->GetSize());
  EXPECT_EQ(expected_size2, panel_child->GetSize());
}

void FillLayoutSizeHierarchyWithDelegate(bool size_from_parent) {
  HoneycombRefPtr<SizingPanelDelegate> delegate_parent = new SizingPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> panel_parent = HoneycombPanel::CreatePanel(delegate_parent);
  HoneycombRefPtr<SizingPanelDelegate> delegate_child = new SizingPanelDelegate();
  HoneycombRefPtr<HoneycombPanel> panel_child = HoneycombPanel::CreatePanel(delegate_child);

  HoneycombSize expected_size(100, 100);

  // The default layout is FillLayout, but explicitly set it anyways just for
  // some testing variety.
  panel_parent->SetToFillLayout();
  panel_child->SetToFillLayout();

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_parent->GetBounds());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_child->GetBounds());

  // With delegates the size can come from either the parent or child.
  if (size_from_parent) {
    delegate_parent->preferred_size_ = expected_size;
  } else {
    delegate_child->preferred_size_ = expected_size;
  }

  // FillLayout is the default Layout. Both panels should end up with the same
  // size.
  panel_parent->AddChildView(panel_child);

  // No delegate methods were called yet.
  EXPECT_TRUE(delegate_parent->IsReset());
  EXPECT_TRUE(delegate_child->IsReset());

  // Force layout.
  panel_parent->Layout();

  // delegate_parent will be called to get the preferred size for panel_parent.
  EXPECT_TRUE(delegate_parent->got_get_preferred_size_);
  EXPECT_FALSE(delegate_parent->got_get_minimum_size_);
  EXPECT_FALSE(delegate_parent->got_get_maximum_size_);
  EXPECT_FALSE(delegate_parent->got_get_height_for_width_);
  EXPECT_TRUE(panel_parent->IsSame(delegate_parent->view_));
  delegate_parent->Reset();

  // delegate_child will be called to get the preferred size for panel_child.
  EXPECT_TRUE(delegate_child->got_get_preferred_size_);
  EXPECT_FALSE(delegate_child->got_get_minimum_size_);
  EXPECT_FALSE(delegate_child->got_get_maximum_size_);
  EXPECT_FALSE(delegate_child->got_get_height_for_width_);
  EXPECT_TRUE(panel_child->IsSame(delegate_child->view_));
  delegate_child->Reset();

  // Panels are now the same size.
  EXPECT_EQ(expected_size, panel_parent->GetSize());
  EXPECT_EQ(expected_size, panel_child->GetSize());

  // Resize the parent panel to a larger size.
  HoneycombSize expected_size2(200, 200);
  panel_parent->SetSize(expected_size2);

  // Force layout.
  panel_parent->Layout();

  // Panels are now the larger size.
  EXPECT_EQ(expected_size2, panel_parent->GetSize());
  EXPECT_EQ(expected_size2, panel_child->GetSize());

  // No additional delegate methods were called.
  EXPECT_TRUE(delegate_parent->IsReset());
  EXPECT_TRUE(delegate_child->IsReset());
}

void FillLayoutSizeHierarchyFromParentWithDelegateImpl() {
  FillLayoutSizeHierarchyWithDelegate(true);
}

void FillLayoutSizeHierarchyFromChildWithDelegateImpl() {
  FillLayoutSizeHierarchyWithDelegate(false);
}

}  // namespace

// Test FillLayout.
PANEL_TEST(FillLayoutCreate)
PANEL_TEST(FillLayoutSizeToPreferredSize)
PANEL_TEST(FillLayoutSizeHierarchy)
PANEL_TEST(FillLayoutSizeHierarchyFromParentWithDelegate)
PANEL_TEST(FillLayoutSizeHierarchyFromChildWithDelegate)

namespace {

void BoxLayoutCreateImpl() {
  HoneycombRefPtr<HoneycombPanel> panel = HoneycombPanel::CreatePanel(nullptr);

  HoneycombBoxLayoutSettings settings;

  // Explicitly set to BoxLayout.
  panel->SetToBoxLayout(settings);

  HoneycombRefPtr<HoneycombLayout> layout = panel->GetLayout();
  EXPECT_TRUE(layout.get());
  EXPECT_TRUE(layout->AsBoxLayout().get());
}

const int kBLParentSize = 100;
const int kBLChildSize = 10;

void BoxLayoutSizeHierarchy(bool with_delegate,
                            const HoneycombBoxLayoutSettings& settings,
                            const HoneycombRect& expected_child1_bounds,
                            const HoneycombRect& expected_child2_bounds,
                            int child1_flex = 0,
                            int child2_flex = 0) {
  HoneycombRefPtr<SizingPanelDelegate> delegate_parent;
  if (with_delegate) {
    delegate_parent = new SizingPanelDelegate();
  }
  HoneycombRefPtr<HoneycombPanel> panel_parent = HoneycombPanel::CreatePanel(delegate_parent);

  HoneycombRefPtr<SizingPanelDelegate> delegate_child1, delegate_child2;
  if (with_delegate) {
    delegate_child1 = new SizingPanelDelegate();
    delegate_child2 = new SizingPanelDelegate();
  }
  HoneycombRefPtr<HoneycombPanel> panel_child1 = HoneycombPanel::CreatePanel(delegate_child1);
  HoneycombRefPtr<HoneycombPanel> panel_child2 = HoneycombPanel::CreatePanel(delegate_child2);

  // Default bounds are empty.
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_parent->GetBounds());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_child1->GetBounds());
  EXPECT_EQ(HoneycombRect(0, 0, 0, 0), panel_child2->GetBounds());

  // Give the parent a size.
  HoneycombSize initial_parent_size(kBLParentSize, kBLParentSize);
  if (with_delegate) {
    delegate_parent->preferred_size_ = initial_parent_size;
  } else {
    panel_parent->SetSize(initial_parent_size);
  }

  // Give the children a size smaller than the parent.
  HoneycombSize initial_child_size(kBLChildSize, kBLChildSize);
  if (with_delegate) {
    delegate_child1->preferred_size_ = initial_child_size;
    delegate_child2->preferred_size_ = initial_child_size;
  } else {
    panel_child1->SetSize(initial_child_size);
    panel_child2->SetSize(initial_child_size);
  }

  // Set to BoxLayout with |settings|.
  panel_parent->SetToBoxLayout(settings);

  panel_parent->AddChildView(panel_child1);
  panel_parent->AddChildView(panel_child2);

  if (child1_flex > 0 || child2_flex > 0) {
    // Flex will apply relative stretch in the main axis direction.
    HoneycombRefPtr<HoneycombBoxLayout> layout = panel_parent->GetLayout()->AsBoxLayout();
    if (child1_flex > 0) {
      layout->SetFlexForView(panel_child1, child1_flex);
    }
    if (child2_flex > 0) {
      layout->SetFlexForView(panel_child2, child2_flex);
    }
  }

  if (with_delegate) {
    // No delegate methods were called yet.
    EXPECT_TRUE(delegate_parent->IsReset());
    EXPECT_TRUE(delegate_child1->IsReset());
    EXPECT_TRUE(delegate_child2->IsReset());
  }

  // Force layout.
  panel_parent->Layout();

  if (with_delegate) {
    // delegate_parent will be called to get the preferred size for
    // panel_parent.
    EXPECT_TRUE(delegate_parent->got_get_preferred_size_);
    EXPECT_FALSE(delegate_parent->got_get_minimum_size_);
    EXPECT_FALSE(delegate_parent->got_get_maximum_size_);
    EXPECT_FALSE(delegate_parent->got_get_height_for_width_);
    EXPECT_TRUE(panel_parent->IsSame(delegate_parent->view_));
    delegate_parent->Reset();

    // delegate_child1 will be called to get the preferred size for
    // panel_child1.
    // GetHeightForWidth may also be called depending on the settings.
    EXPECT_TRUE(delegate_child1->got_get_preferred_size_);
    EXPECT_FALSE(delegate_child1->got_get_minimum_size_);
    EXPECT_FALSE(delegate_child1->got_get_maximum_size_);
    EXPECT_TRUE(panel_child1->IsSame(delegate_child1->view_));
    delegate_child1->Reset();

    // delegate_child2 will be called to get the preferred size for
    // panel_child2.
    // GetHeightForWidth may also be called depending on the settings.
    EXPECT_TRUE(delegate_child2->got_get_preferred_size_);
    EXPECT_FALSE(delegate_child2->got_get_minimum_size_);
    EXPECT_FALSE(delegate_child2->got_get_maximum_size_);
    EXPECT_TRUE(panel_child2->IsSame(delegate_child2->view_));
    delegate_child2->Reset();
  }

  // The parent should be the same size.
  EXPECT_EQ(initial_parent_size, panel_parent->GetSize());

  // Children should have the expected bounds.
  EXPECT_EQ(expected_child1_bounds, panel_child1->GetBounds());
  EXPECT_EQ(expected_child2_bounds, panel_child2->GetBounds());

  if (with_delegate) {
    // No additional delegate methods were called.
    EXPECT_TRUE(delegate_parent->IsReset());
    EXPECT_TRUE(delegate_child1->IsReset());
    EXPECT_TRUE(delegate_child2->IsReset());
  }
}

void BoxLayoutSizeHierarchyVerticalStretch(bool with_delegate) {
  // Vertical layout with children stretched along the horizontal axis.
  //
  // -----------
  // |111111111|
  // |222222222|
  // |         |
  // |         |
  // |         |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;

  HoneycombRect expected_child1_bounds(0, 0, kBLParentSize, kBLChildSize);
  HoneycombRect expected_child2_bounds(0, kBLChildSize, kBLParentSize, kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyVerticalStretchImpl() {
  BoxLayoutSizeHierarchyVerticalStretch(false);
}

void BoxLayoutSizeHierarchyVerticalStretchWithDelegateImpl() {
  BoxLayoutSizeHierarchyVerticalStretch(true);
}

void BoxLayoutSizeHierarchyHorizontalStretch(bool with_delegate) {
  // Horizontal layout with children stretched along the vertical axis.
  //
  // -----------
  // |12       |
  // |12       |
  // |12       |
  // |12       |
  // |12       |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.horizontal = true;

  HoneycombRect expected_child1_bounds(0, 0, kBLChildSize, kBLParentSize);
  HoneycombRect expected_child2_bounds(kBLChildSize, 0, kBLChildSize, kBLParentSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyHorizontalStretchImpl() {
  BoxLayoutSizeHierarchyHorizontalStretch(false);
}

void BoxLayoutSizeHierarchyHorizontalStretchWithDelegateImpl() {
  BoxLayoutSizeHierarchyHorizontalStretch(true);
}

void BoxLayoutSizeHierarchyVerticalCenter(bool with_delegate) {
  // Vertical layout with children centered along the horizontal axis.
  //
  // -----------
  // |    1    |
  // |    2    |
  // |         |
  // |         |
  // |         |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.cross_axis_alignment = HONEYCOMB_CROSS_AXIS_ALIGNMENT_CENTER;

  int xoffset = (kBLParentSize - kBLChildSize) / 2;
  HoneycombRect expected_child1_bounds(xoffset, 0, kBLChildSize, kBLChildSize);
  HoneycombRect expected_child2_bounds(xoffset, kBLChildSize, kBLChildSize,
                                 kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyVerticalCenterImpl() {
  BoxLayoutSizeHierarchyVerticalCenter(false);
}

void BoxLayoutSizeHierarchyVerticalCenterWithDelegateImpl() {
  BoxLayoutSizeHierarchyVerticalCenter(true);
}

void BoxLayoutSizeHierarchyHorizontalCenter(bool with_delegate) {
  // Horizontal layout with children centered along the vertical axis.
  //
  // -----------
  // |         |
  // |         |
  // |12       |
  // |         |
  // |         |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.horizontal = true;
  settings.cross_axis_alignment = HONEYCOMB_CROSS_AXIS_ALIGNMENT_CENTER;

  int yoffset = (kBLParentSize - kBLChildSize) / 2;
  HoneycombRect expected_child1_bounds(0, yoffset, kBLChildSize, kBLChildSize);
  HoneycombRect expected_child2_bounds(kBLChildSize, yoffset, kBLChildSize,
                                 kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyHorizontalCenterImpl() {
  BoxLayoutSizeHierarchyHorizontalCenter(false);
}

void BoxLayoutSizeHierarchyHorizontalCenterWithDelegateImpl() {
  BoxLayoutSizeHierarchyHorizontalCenter(true);
}

void BoxLayoutSizeHierarchyVerticalCenterCenter(bool with_delegate) {
  // Vertical layout with children centered along the horizontal and vertical
  // axis.
  //
  // -----------
  // |         |
  // |    1    |
  // |    2    |
  // |         |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.main_axis_alignment = HONEYCOMB_MAIN_AXIS_ALIGNMENT_CENTER;
  settings.cross_axis_alignment = HONEYCOMB_CROSS_AXIS_ALIGNMENT_CENTER;

  int xoffset = (kBLParentSize - kBLChildSize) / 2;
  int yoffset = (kBLParentSize - (kBLChildSize * 2)) / 2;
  HoneycombRect expected_child1_bounds(xoffset, yoffset, kBLChildSize, kBLChildSize);
  HoneycombRect expected_child2_bounds(xoffset, yoffset + kBLChildSize, kBLChildSize,
                                 kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyVerticalCenterCenterImpl() {
  BoxLayoutSizeHierarchyVerticalCenterCenter(false);
}

void BoxLayoutSizeHierarchyVerticalCenterCenterWithDelegateImpl() {
  BoxLayoutSizeHierarchyVerticalCenterCenter(true);
}

void BoxLayoutSizeHierarchyHorizontalCenterCenter(bool with_delegate) {
  // Horizontal layout with children centered along the vertical and horizontal
  // axis.
  //
  // -----------
  // |         |
  // |         |
  // |   12    |
  // |         |
  // |         |
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.horizontal = true;
  settings.main_axis_alignment = HONEYCOMB_MAIN_AXIS_ALIGNMENT_CENTER;
  settings.cross_axis_alignment = HONEYCOMB_CROSS_AXIS_ALIGNMENT_CENTER;

  int xoffset = (kBLParentSize - (kBLChildSize * 2)) / 2;
  int yoffset = (kBLParentSize - kBLChildSize) / 2;
  HoneycombRect expected_child1_bounds(xoffset, yoffset, kBLChildSize, kBLChildSize);
  HoneycombRect expected_child2_bounds(xoffset + kBLChildSize, yoffset, kBLChildSize,
                                 kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds);
}

void BoxLayoutSizeHierarchyHorizontalCenterCenterImpl() {
  BoxLayoutSizeHierarchyHorizontalCenterCenter(false);
}

void BoxLayoutSizeHierarchyHorizontalCenterCenterWithDelegateImpl() {
  BoxLayoutSizeHierarchyHorizontalCenterCenter(true);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexOne(bool with_delegate) {
  // Vertical layout with child1 stretched along the horizontal and vertical
  // axis and child2 stretched along the horizontal axis only (unequal flex).
  //
  // -----------
  // |111111111|
  // |111111111|
  // |111111111|
  // |111111111|
  // |222222222|
  // -----------
  //
  HoneycombBoxLayoutSettings settings;

  HoneycombRect expected_child1_bounds(0, 0, kBLParentSize,
                                 kBLParentSize - kBLChildSize);
  HoneycombRect expected_child2_bounds(0, kBLParentSize - kBLChildSize, kBLParentSize,
                                 kBLChildSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds, 1, 0);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexOneImpl() {
  BoxLayoutSizeHierarchyVerticalStretchFlexOne(false);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexOneWithDelegateImpl() {
  BoxLayoutSizeHierarchyVerticalStretchFlexOne(true);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexOne(bool with_delegate) {
  // Horizontal layout with child1 stretched along the vertical and horizontal
  // axis and child2 stretched along the vertical axis only (unequal flex).
  //
  // -----------
  // |111111112|
  // |111111112|
  // |111111112|
  // |111111112|
  // |111111112|
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.horizontal = true;

  HoneycombRect expected_child1_bounds(0, 0, kBLParentSize - kBLChildSize,
                                 kBLParentSize);
  HoneycombRect expected_child2_bounds(kBLParentSize - kBLChildSize, 0, kBLChildSize,
                                 kBLParentSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds, 1, 0);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexOneImpl() {
  BoxLayoutSizeHierarchyHorizontalStretchFlexOne(false);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexOneWithDelegateImpl() {
  BoxLayoutSizeHierarchyHorizontalStretchFlexOne(true);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexBoth(bool with_delegate) {
  // Vertical layout with children stretched along the horizontal and vertical
  // axis (equal flex).
  //
  // -----------
  // |111111111|
  // |111111111|
  // |111111111|
  // |222222222|
  // |222222222|
  // |222222222|
  // -----------
  //
  HoneycombBoxLayoutSettings settings;

  HoneycombRect expected_child1_bounds(0, 0, kBLParentSize, kBLParentSize / 2);
  HoneycombRect expected_child2_bounds(0, kBLParentSize / 2, kBLParentSize,
                                 kBLParentSize / 2);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds, 1, 1);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexBothImpl() {
  BoxLayoutSizeHierarchyVerticalStretchFlexBoth(false);
}

void BoxLayoutSizeHierarchyVerticalStretchFlexBothWithDelegateImpl() {
  BoxLayoutSizeHierarchyVerticalStretchFlexBoth(true);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexBoth(bool with_delegate) {
  // Horizontal layout with children stretched along the vertical and horizontal
  // axis (equal flex).
  //
  // -----------
  // |111122222|
  // |111122222|
  // |111122222|
  // |111122222|
  // |111122222|
  // -----------
  //
  HoneycombBoxLayoutSettings settings;
  settings.horizontal = true;

  HoneycombRect expected_child1_bounds(0, 0, kBLParentSize / 2, kBLParentSize);
  HoneycombRect expected_child2_bounds(kBLParentSize / 2, 0, kBLParentSize / 2,
                                 kBLParentSize);

  BoxLayoutSizeHierarchy(with_delegate, settings, expected_child1_bounds,
                         expected_child2_bounds, 1, 1);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexBothImpl() {
  BoxLayoutSizeHierarchyHorizontalStretchFlexBoth(false);
}

void BoxLayoutSizeHierarchyHorizontalStretchFlexBothWithDelegateImpl() {
  BoxLayoutSizeHierarchyHorizontalStretchFlexBoth(true);
}

}  // namespace

// Test BoxLayout. The BoxLayoutSizeHierarchy* tests are representative but not
// comprehensive (e.g. not all possible configurations are tested).
PANEL_TEST(BoxLayoutCreate)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretch)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretchWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretch)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretchWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalCenter)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalCenterWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalCenter)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalCenterWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalCenterCenter)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalCenterCenterWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalCenterCenter)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalCenterCenterWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretchFlexOne)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretchFlexOneWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretchFlexOne)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretchFlexOneWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretchFlexBoth)
PANEL_TEST(BoxLayoutSizeHierarchyVerticalStretchFlexBothWithDelegate)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretchFlexBoth)
PANEL_TEST(BoxLayoutSizeHierarchyHorizontalStretchFlexBothWithDelegate)
