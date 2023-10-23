// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_VIEW_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_VIEW_IMPL_H_
#pragma once

// Honeycomb exposes views framework functionality via a hierarchy of HoneycombView and
// related objects. While the goal is to accurately represent views framework
// capabilities there is not always a direct 1:1 mapping between the Honeycomb
// implementation and the underlying views implementation. Certain liberties
// have been taken with the Honeycomb API design to clarify the user experience.
//
// Honeycomb implementation overview:
//
// HoneycombView-derived classes (HoneycombPanel, HoneycombLabelButton, etc.) are implemented
// using a specialization of the HoneycombViewImpl template. On Initialize() the
// HoneycombViewImpl object creates an underlying views::View object via the
// CreateRootView() method. The views::View objects are implemented using a
// specialization of the HoneycombViewView template. HoneycombViewView extends the
// views::View-derived class and executes HoneycombViewDelegate-derived callbacks by
// overriding views::View methods.
//
// Example 1: The HoneycombBasicPanelImpl object created via HoneycombPanel::CreatePanel()
// has the following object hierarchy:
//
//   HoneycombView => HoneycombPanel =>
//   HoneycombViewImpl<views::View, HoneycombPanel, HoneycombPanelDelegate> =>
//   HoneycombPanelImpl<views::View, HoneycombPanel, HoneycombPanelDelegate> =>
//   HoneycombBasicPanelImpl.
//
// And the HoneycombBasicPanelView object created via
// HoneycombBasicPanelImpl::CreateRootView() has the following object hierarchy:
//
//   views::View =>
//   HoneycombViewView<views::View, HoneycombPanelDelegate> =>
//   HoneycombPanelView<views::View, HoneycombPanelDelegate> =>
//   HoneycombBasicPanelView.
//
// Example 2: In some cases an intermediary type is required to meet Honeycomb
// template requirements (e.g. HoneycombViewView requires a no-argument constructor).
// The HoneycombBasicLabelButtonImpl object created via
// HoneycombLabelButton::CreateLabelButton() has the following object hierarchy:
//
//   HoneycombView => HoneycombButton => HoneycombLabelButton =>
//   HoneycombViewImpl<views::LabelButton, HoneycombLabelButton, HoneycombButtonDelegate> =>
//   HoneycombButtonImpl<views::LabelButton, HoneycombLabelButton, HoneycombButtonDelegate> =>
//   HoneycombLabelButtonImpl<views::LabelButton, HoneycombLabelButton,
//                      HoneycombButtonDelegate> =>
//   HoneycombBasicLabelButtonImpl
//
// And the HoneycombBasicLabelButtonView object created via
// HoneycombBasicLabelButtonImpl::CreateRootView() has the following object hierarchy:
//
//   views::View => views::Button => views::LabelButton =>
//   LabelButtonEx (used to implement the required no-argument constructor) =>
//   HoneycombViewView<LabelButtonEx, HoneycombButtonDelegate> =>
//   HoneycombButtonView<LabelButtonEx, HoneycombButtonDelegate> =>
//   HoneycombLabelButtonView<LabelButtonEx, HoneycombButtonDelegate> =>
//   HoneycombBasicLabelButtonView.
//
//
// General design considerations:
//
// HoneycombView classes are ref-counted whereas views::View classes are not. There
// is generally a 1:1 relationship between HoneycombView and views::View objects.
// However, there may be intermediary views::View objects that are not exposed
// by the Honeycomb layer. For example:
// - views::Widget creates views::RootView and views::ContentView child objects;
// - views::ScrollView creates views::ScrollView::Viewport child objects.
//
// The views::View class exposes methods that are not applicable for a subset of
// views implementations. For example:
// - Calling AddChildView() on a views::LabelButton is unexpected;
// - Adding a child to a views::ScrollView requires calling the SetContents()
//   method instead of AddChildView().
// To avoid user confusion Honeycomb introduces a HoneycombPanel type that extends HoneycombView
// and exposes common child management functionality. Types that allow
// arbitrary children extend HoneycombPanel instead of HoneycombView.
//
//
// Object ownership considerations:
//
// On initial creation the HoneycombViewImpl object owns an underlying views::View
// object (created by overriding the CreateRootView() method) and the
// views::View object holds a non-ref-counted reference to the HoneycombViewImpl
// object. If a HoneycombViewImpl is destroyed (all refs released) then the underlying
// views::View object is deleted.
//
// When a views::View object is parented to another views::View (via
// HoneycombPanel::AddChildView or similar) the ownership semantics change. The
// HoneycombViewImpl swaps its owned reference for an unowned reference and the
// views::View gains a ref-counted reference to the HoneycombViewImpl
// (HoneycombView::IsAttached() now returns true).
//
// When a parent views::View is deleted all child views::Views in the view
// hierarchy are also deleted (see [1] for exceptions). When this happens the
// ref-counted HoneycombViewImpl reference held by the views::View is released. The
// HoneycombViewImpl is deleted if the client kept no references, otherwise the
// HoneycombViewImpl is marked as invalid (HoneycombView::IsValid() now returns false).
//
// When a views::View is removed from the view hierarchy (via
// HoneycombPanel::RemoveChildView or similar) the initial ownership state is
// restored. The HoneycombViewImpl regains ownership of the views::View and the
// ref-counted HoneycombViewImpl reference held by the views::View is released.
//
// The relationship between HoneycombViewImpl and views::View objects is managed using
// the view_util:: functions. Type conversion is facilitated using the As*()
// methods exposed by HoneycombView-derived classes and the HoneycombViewAdapter interface
// implemented by HoneycombViewImpl. See view_util.[cc|h] for implementation details.
//
// Some other object types are also tied to views::View lifetime. For example,
// HoneycombLayout and the underling views::LayoutManager objects are owned by the
// views::View that they're assigned to. This relationship is managed using the
// layout_util:: functions in layout_util.[cc|h].
//
// [1] By default views::View objects are deleted when the parent views::View
//     object is deleted. However, this behavior can be changed either
//     explicitly by calling set_owned_by_client() or implicitly by using
//     interfaces like WidgetDelegateView (where WidgetDelegate is-a View, and
//     the View is deleted when the native Widget is destroyed). Honeycomb
//     implementations that utilize this behavior must take special care with
//     object ownership management.
//
//
// To implement a new HoneycombView-derived class:
//
// 1. Choose a views class to expose.
//    * We'll create a new HoneycombFooBar class which exposes a hypothetical
//      views::FooBar class. The views::FooBar class might look like this:
//
//      File ui/views/foo_bar.h:
//
//      namespace views {
//
//      // FooBar view does a task on child views.
//      class FooBar : public View {
//       public:
//        FooBar();
//
//        // Do a task.
//        void DoTask();
//        // Called when the task is done.
//        virtual void OnTaskDone();
//
//        // View methods:
//        void Layout() override;  // Implements custom layout of child views.
//      };
//
//      }  // namespace views
//
// 2. Determine the existing HoneycombView-derived class that the new view class
//    should extend.
//    * Since in this example HoneycombFooBar can have arbitrary child views we'll
//      have it extend HoneycombPanel.
//
// 3. Determine whether the new view class can use an existing delegate class
//    (like HoneycombPanelDelegate) or whether it needs its own delegate class.
//    * Since HoneycombFooBar has an OnTaskDone() callback we'll add a new
//      HoneycombFooBarDelegate class to expose it.
//
// 4. Create new header files in the honey/include/views/ directory.
//    * Using existing files as a model, the resulting header contents might
//      look like this:
//
//      File honey/include/views/honey_foo_bar.h:
//
//      ///
//      // A FooBar view does a task on child views.
//      ///
//      /*--honey(source=library)--*/
//      class HoneycombFooBar : public HoneycombPanel {
//       public:
//        ///
//        // Create a new FooBar.
//        ///
//        /*--honey(optional_param=delegate)--*/
//        static HoneycombRefPtr<HoneycombFooBar> CreateFooBar(
//            HoneycombRefPtr<HoneycombFooBarDelegate> delegate);
//
//        ///
//        // Do a task.
//        ///
//        /*--honey()--*/
//        virtual void DoTask() =0;
//      };
//
//      File honey/include/views/honey_foo_bar_delegate.h:
//
//      ///
//      // Implement this interface to handle FooBar events.
//      ///
//      /*--honey(source=client)--*/
//      class HoneycombFooBarDelegate : public HoneycombPanelDelegate {
//       public:
//        ///
//        // Called when the task is done.
//        ///
//        /*--honey()--*/
//        virtual void OnTaskDone(HoneycombRefPtr<HoneycombFooBar> foobar) {}
//      };
//
// 5. Add an As*() method to the HoneycombView-derived class.
//    * Using existing file contents as a model, make the following changes in
//      honey/include/views/honey_panel.h:
//      * Forward declare the HoneycombFooBar class.
//      * Add a new HoneycombPanel::AsFooBar() method:
//
//        ///
//        // Returns this Panel as a FooBar or NULL if this is not a FooBar.
//        ///
//        /*--honey()--*/
//        virtual HoneycombRefPtr<HoneycombFooBar> AsFooBar() =0;
//
// 6. Add a default implementation for the As*() method to the HoneycombViewImpl-
//    derived class.
//    * Using existing file contents as a model, make the following changes in
//      honey/libhoneycomb/browser/views/panel_impl.h:
//      * Include "include/views/honey_foo_bar.h".
//      * Add a default HoneycombPanelImpl::AsFooBar() implementation:
//
//        HoneycombRefPtr<HoneycombFooBar> AsFooBar() override { return nullptr; }
//
// 7. Update the HoneycombViewAdapter::GetFor() method implementation to call the
//    As*() method.
//    * Using existing file contents as a model, make the following changes in
//      honey/libhoneycomb/browser/views/view_adapter.cc:
//      * Include "include/views/honey_foo_bar.h".
//      * Call the AsFooBar() method to identify the adapter object:
//
//        ... if (view->AsPanel()) {
//          HoneycombRefPtr<HoneycombPanel> panel = view->AsPanel();
//          if (panel->AsFooBar()) {
//            adapter = static_cast<HoneycombFooBarImpl*>(panel->AsFooBar().get());
//          } else ...
//        } else ...
//
// 8. Implement the HoneycombViewView-derived class.
//    * Using existing files as a model (for example, HoneycombBasicPanelView), create
//      a HoneycombFooBarView class at honey/libhoneycomb/browser/views/foo_bar_view.[cc|h].
//      This class:
//      * Extends HoneycombPanelView<views::FooBar, HoneycombFooBarDelegate>.
//      * Overrides the views::FooBar::OnTaskDone method to execute the
//        HoneycombFooBarDelegate::OnTaskDone callback:
//
//        void HoneycombFooBarView::OnTaskDone() {
//          if (honey_delegate())
//            honey_delegate()->OnTaskDone(GetHoneycombFooBar());
//        }
//
// 9. Implement the HoneycombViewImpl-derived class.
//    * Use existing files as a model (for example, HoneycombBasicPanelImpl), create a
//      HoneycombFooBarImpl class at honey/libhoneycomb/browser/views/foo_bar_impl.[cc|h].
//      This class:
//      * Extends HoneycombPanelImpl<views::FooBar, HoneycombFooBar, HoneycombFooBarDelegate>.
//      * Implements AsFooBar() to return |this|.
//      * Implements CreateRootView() to return a new HoneycombFooBarView instance.
//      * Implements the HoneycombFooBar::DoTask() method to call
//        views::FooBar::DoTask():
//
//        void HoneycombFooBarImpl::DoTask() {
//          HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
//          root_view()->DoTask();
//        }
//
// 10. Implement the static method that creates the HoneycombViewImpl-derived object
//     instance.
//     * Use existing files as a model (for example, HoneycombBasicPanelImpl),
//       implement the HoneycombFooBar::CreateFooBar static method in
//       honey/libhoneycomb/browser/views/foo_bar_impl.cc. This method:
//       * Creates a new HoneycombFooBarImpl object.
//       * Calls Initialize() on the HoneycombFooBarImpl object.
//       * Returns the HoneycombFooBarImpl object.
//
// 11. Add the new source files from #7 and #8 to the 'libhoneycomb_static' target in
//     honey.gyp.
//
// 12. Update the Honeycomb project files and build.
//     * Run honey/tools/translator.[bat|sh] to update the translation layer for
//       the new/modified classes. This tool needs to be run whenever header
//       files in the honey/include/ directory are changed.
//     * Run honey/honey_create_projects.[bat|sh] to update the Ninja build files.
//     * Build Honeycomb using Ninja.
//

#include "include/views/honey_browser_view.h"
#include "include/views/honey_button.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_scroll_view.h"
#include "include/views/honey_textfield.h"
#include "include/views/honey_view.h"

#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/views/view_adapter.h"
#include "libhoneycomb/browser/views/view_util.h"

#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/values.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/view.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_VIEW_IMPL_T                               \
  template <class ViewsViewClass, class HoneycombViewClass, \
            class HoneycombViewDelegateClass>
#define HONEYCOMB_VIEW_IMPL_A ViewsViewClass, HoneycombViewClass, HoneycombViewDelegateClass
#define HONEYCOMB_VIEW_IMPL_D HoneycombViewImpl<HONEYCOMB_VIEW_IMPL_A>

// Base template for implementing HoneycombView-derived classes. See above comments
// for a usage overview.
HONEYCOMB_VIEW_IMPL_T class HoneycombViewImpl : public HoneycombViewAdapter, public HoneycombViewClass {
 public:
  // Necessary for the HONEYCOMB_REQUIRE_VALID_*() macros to compile.
  using ParentClass = HONEYCOMB_VIEW_IMPL_D;

  // Returns the content views::View object that should be the target of most
  // customization actions. May be the root view or a child of the root view.
  virtual views::View* content_view() const { return root_view(); }

  // Returns the Honeycomb delegate as the derived type which may be nullptr.
  HoneycombViewDelegateClass* delegate() const { return delegate_.get(); }

  // Returns the root views::View object owned by this HoneycombView.
  ViewsViewClass* root_view() const { return root_view_ref_; }

  // HoneycombViewAdapter methods:
  views::View* Get() const override { return root_view(); }
  std::unique_ptr<views::View> PassOwnership() override {
    DCHECK(root_view_);
    return std::move(root_view_);
  }
  void ResumeOwnership() override {
    DCHECK(root_view_ref_);
    DCHECK(!root_view_);
    root_view_.reset(root_view_ref_);
  }
  void Detach() override {
    if (root_view_) {
      root_view_.reset();
    }
    root_view_ref_ = nullptr;
  }
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override {
    info->Set("type", GetDebugType());
    info->Set("id", root_view()->GetID());

    // Use GetBounds() because some subclasses (like HoneycombWindowImpl) override it.
    const HoneycombRect& bounds = GetBounds();
    base::Value::Dict bounds_value;
    bounds_value.Set("x", bounds.x);
    bounds_value.Set("y", bounds.y);
    bounds_value.Set("width", bounds.width);
    bounds_value.Set("height", bounds.height);
    info->Set("bounds", std::move(bounds_value));
  }

  // HoneycombView methods. When adding new As*() methods make sure to update
  // HoneycombViewAdapter::GetFor() in view_adapter.cc.
  HoneycombRefPtr<HoneycombBrowserView> AsBrowserView() override { return nullptr; }
  HoneycombRefPtr<HoneycombButton> AsButton() override { return nullptr; }
  HoneycombRefPtr<HoneycombPanel> AsPanel() override { return nullptr; }
  HoneycombRefPtr<HoneycombScrollView> AsScrollView() override { return nullptr; }
  HoneycombRefPtr<HoneycombTextfield> AsTextfield() override { return nullptr; }
  HoneycombString GetTypeString() override;
  HoneycombString ToString(bool include_children) override;
  bool IsValid() override;
  bool IsAttached() override;
  bool IsSame(HoneycombRefPtr<HoneycombView> that) override;
  HoneycombRefPtr<HoneycombViewDelegate> GetDelegate() override;
  HoneycombRefPtr<HoneycombWindow> GetWindow() override;
  int GetID() override;
  void SetID(int id) override;
  int GetGroupID() override;
  void SetGroupID(int group_id) override;
  HoneycombRefPtr<HoneycombView> GetParentView() override;
  HoneycombRefPtr<HoneycombView> GetViewForID(int id) override;
  void SetBounds(const HoneycombRect& bounds) override;
  HoneycombRect GetBounds() override;
  HoneycombRect GetBoundsInScreen() override;
  void SetSize(const HoneycombSize& size) override;
  HoneycombSize GetSize() override;
  void SetPosition(const HoneycombPoint& position) override;
  HoneycombPoint GetPosition() override;
  void SetInsets(const HoneycombInsets& insets) override;
  HoneycombInsets GetInsets() override;
  HoneycombSize GetPreferredSize() override;
  void SizeToPreferredSize() override;
  HoneycombSize GetMinimumSize() override;
  HoneycombSize GetMaximumSize() override;
  int GetHeightForWidth(int width) override;
  void InvalidateLayout() override;
  void SetVisible(bool visible) override;
  bool IsVisible() override;
  bool IsDrawn() override;
  void SetEnabled(bool enabled) override;
  bool IsEnabled() override;
  void SetFocusable(bool focusable) override;
  bool IsFocusable() override;
  bool IsAccessibilityFocusable() override;
  void RequestFocus() override;
  void SetBackgroundColor(honey_color_t color) override;
  honey_color_t GetBackgroundColor() override;
  bool ConvertPointToScreen(HoneycombPoint& point) override;
  bool ConvertPointFromScreen(HoneycombPoint& point) override;
  bool ConvertPointToWindow(HoneycombPoint& point) override;
  bool ConvertPointFromWindow(HoneycombPoint& point) override;
  bool ConvertPointToView(HoneycombRefPtr<HoneycombView> view, HoneycombPoint& point) override;
  bool ConvertPointFromView(HoneycombRefPtr<HoneycombView> view, HoneycombPoint& point) override;

 protected:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombViewImpl(HoneycombRefPtr<HoneycombViewDelegateClass> delegate)
      : delegate_(delegate), root_view_ref_(nullptr) {}

  // Initialize this object.
  virtual void Initialize() {
    root_view_.reset(CreateRootView());
    DCHECK(root_view_.get());
    root_view_ref_ = root_view_.get();
    view_util::Register(this);
    InitializeRootView();
  }

  // Create the root views::View object.
  virtual ViewsViewClass* CreateRootView() = 0;

  // Perform required initialization of the root_view() object created by
  // CreateRootView(). Called after this object has been registered.
  virtual void InitializeRootView() = 0;

 private:
  HoneycombRefPtr<HoneycombViewDelegateClass> delegate_;

  // Owned reference to the views::View wrapped by this object. Will be nullptr
  // before the View is created and after the View's ownership is transferred.
  std::unique_ptr<ViewsViewClass> root_view_;

  // Unowned reference to the views::View wrapped by this object. Will be
  // nullptr before the View is created and after the View is destroyed.
  ViewsViewClass* root_view_ref_;
};

HONEYCOMB_VIEW_IMPL_T HoneycombString HONEYCOMB_VIEW_IMPL_D::GetTypeString() {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombString());
  return GetDebugType();
}

HONEYCOMB_VIEW_IMPL_T HoneycombString HONEYCOMB_VIEW_IMPL_D::ToString(bool include_children) {
  HONEYCOMB_REQUIRE_UIT_RETURN(HoneycombString());
  base::Value::Dict info;
  if (IsValid()) {
    GetDebugInfo(&info, include_children);
  } else {
    info.Set("type", GetDebugType());
  }

  std::string json_string;
  base::JSONWriter::WriteWithOptions(info, 0, &json_string);
  return json_string;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsValid() {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return !!root_view_ref_;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsAttached() {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  return !root_view_.get();
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsSame(HoneycombRefPtr<HoneycombView> that) {
  HONEYCOMB_REQUIRE_UIT_RETURN(false);
  HoneycombViewImpl* that_impl = static_cast<HoneycombViewImpl*>(that.get());
  if (!that_impl) {
    return false;
  }
  return this == that_impl;
}

HONEYCOMB_VIEW_IMPL_T HoneycombRefPtr<HoneycombViewDelegate> HONEYCOMB_VIEW_IMPL_D::GetDelegate() {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  return delegate();
}

HONEYCOMB_VIEW_IMPL_T HoneycombRefPtr<HoneycombWindow> HONEYCOMB_VIEW_IMPL_D::GetWindow() {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  if (root_view()) {
    return view_util::GetWindowFor(root_view()->GetWidget());
  }
  return nullptr;
}

HONEYCOMB_VIEW_IMPL_T int HONEYCOMB_VIEW_IMPL_D::GetID() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0);
  return root_view()->GetID();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetID(int id) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetID(id);
}

HONEYCOMB_VIEW_IMPL_T int HONEYCOMB_VIEW_IMPL_D::GetGroupID() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0);
  return root_view()->GetGroup();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetGroupID(int group_id) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  if (root_view()->GetGroup() != -1) {
    return;
  }
  root_view()->SetGroup(group_id);
}

HONEYCOMB_VIEW_IMPL_T HoneycombRefPtr<HoneycombView> HONEYCOMB_VIEW_IMPL_D::GetParentView() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  views::View* view = root_view()->parent();
  if (!view) {
    return nullptr;
  }
  return view_util::GetFor(view, true);
}

HONEYCOMB_VIEW_IMPL_T HoneycombRefPtr<HoneycombView> HONEYCOMB_VIEW_IMPL_D::GetViewForID(int id) {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  views::View* view = root_view()->GetViewByID(id);
  if (!view) {
    return nullptr;
  }
  return view_util::GetFor(view, true);
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetBounds(const HoneycombRect& bounds) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetBoundsRect(
      gfx::Rect(bounds.x, bounds.y, bounds.width, bounds.height));
}

HONEYCOMB_VIEW_IMPL_T HoneycombRect HONEYCOMB_VIEW_IMPL_D::GetBounds() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRect());
  const gfx::Rect& bounds = root_view()->bounds();
  return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
}

HONEYCOMB_VIEW_IMPL_T HoneycombRect HONEYCOMB_VIEW_IMPL_D::GetBoundsInScreen() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRect());
  const gfx::Rect& bounds = root_view()->GetBoundsInScreen();
  return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetSize(const HoneycombSize& size) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetSize(gfx::Size(size.width, size.height));
}

HONEYCOMB_VIEW_IMPL_T HoneycombSize HONEYCOMB_VIEW_IMPL_D::GetSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombSize());
  // Call GetBounds() since child classes may override it.
  const HoneycombRect& bounds = GetBounds();
  return HoneycombSize(bounds.width, bounds.height);
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetPosition(const HoneycombPoint& position) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetPosition(gfx::Point(position.x, position.y));
}

HONEYCOMB_VIEW_IMPL_T HoneycombPoint HONEYCOMB_VIEW_IMPL_D::GetPosition() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombPoint());
  // Call GetBounds() since child classes may override it.
  const HoneycombRect& bounds = GetBounds();
  return HoneycombPoint(bounds.x, bounds.y);
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetInsets(const HoneycombInsets& insets) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  const gfx::Insets& gfx_insets =
      gfx::Insets::TLBR(insets.top, insets.left, insets.bottom, insets.right);
  root_view()->SetBorder(
      gfx_insets.IsEmpty() ? nullptr : views::CreateEmptyBorder(gfx_insets));
}

HONEYCOMB_VIEW_IMPL_T HoneycombInsets HONEYCOMB_VIEW_IMPL_D::GetInsets() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombInsets());
  const auto insets = root_view()->GetInsets();
  return HoneycombInsets(insets.top(), insets.left(), insets.bottom(),
                   insets.right());
}

HONEYCOMB_VIEW_IMPL_T HoneycombSize HONEYCOMB_VIEW_IMPL_D::GetPreferredSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombSize());
  const gfx::Size& size = root_view()->GetPreferredSize();
  return HoneycombSize(size.width(), size.height());
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SizeToPreferredSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SizeToPreferredSize();
}

HONEYCOMB_VIEW_IMPL_T HoneycombSize HONEYCOMB_VIEW_IMPL_D::GetMinimumSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombSize());
  const gfx::Size& size = root_view()->GetMinimumSize();
  return HoneycombSize(size.width(), size.height());
}

HONEYCOMB_VIEW_IMPL_T HoneycombSize HONEYCOMB_VIEW_IMPL_D::GetMaximumSize() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombSize());
  const gfx::Size& size = root_view()->GetMaximumSize();
  return HoneycombSize(size.width(), size.height());
}

HONEYCOMB_VIEW_IMPL_T int HONEYCOMB_VIEW_IMPL_D::GetHeightForWidth(int width) {
  HONEYCOMB_REQUIRE_VALID_RETURN(0);
  return root_view()->GetHeightForWidth(width);
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::InvalidateLayout() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->InvalidateLayout();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetVisible(bool visible) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetVisible(visible);
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsVisible() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->GetVisible();
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsDrawn() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->IsDrawn();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetEnabled(bool enabled) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetEnabled(enabled);
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsEnabled() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->GetEnabled();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetFocusable(bool focusable) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->SetFocusBehavior(focusable ? views::View::FocusBehavior::ALWAYS
                                          : views::View::FocusBehavior::NEVER);
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsFocusable() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->IsFocusable();
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::IsAccessibilityFocusable() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  return root_view()->IsAccessibilityFocusable();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::RequestFocus() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  root_view()->RequestFocus();
}

HONEYCOMB_VIEW_IMPL_T void HONEYCOMB_VIEW_IMPL_D::SetBackgroundColor(honey_color_t color) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  content_view()->SetBackground(views::CreateSolidBackground(color));
}

HONEYCOMB_VIEW_IMPL_T honey_color_t HONEYCOMB_VIEW_IMPL_D::GetBackgroundColor() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return content_view()->background()->get_color();
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointToScreen(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  if (!view_util::ConvertPointToScreen(root_view(), &gfx_point, false)) {
    return false;
  }
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointFromScreen(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  if (!view_util::ConvertPointFromScreen(root_view(), &gfx_point, false)) {
    return false;
  }
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointToWindow(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  if (!view_util::ConvertPointToWindow(root_view(), &gfx_point)) {
    return false;
  }
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointFromWindow(HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  if (!view_util::ConvertPointFromWindow(root_view(), &gfx_point)) {
    return false;
  }
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointToView(
    HoneycombRefPtr<HoneycombView> view,
    HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (!root_view()->GetWidget()) {
    return false;
  }
  views::View* target_view = view_util::GetFor(view);
  if (!target_view || target_view->GetWidget() != root_view()->GetWidget()) {
    return false;
  }
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  views::View::ConvertPointToTarget(root_view(), target_view, &gfx_point);
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

HONEYCOMB_VIEW_IMPL_T bool HONEYCOMB_VIEW_IMPL_D::ConvertPointFromView(
    HoneycombRefPtr<HoneycombView> view,
    HoneycombPoint& point) {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  if (!root_view()->GetWidget()) {
    return false;
  }
  views::View* target_view = view_util::GetFor(view);
  if (!target_view || target_view->GetWidget() != root_view()->GetWidget()) {
    return false;
  }
  gfx::Point gfx_point = gfx::Point(point.x, point.y);
  views::View::ConvertPointToTarget(target_view, root_view(), &gfx_point);
  point = HoneycombPoint(gfx_point.x(), gfx_point.y());
  return true;
}

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_VIEW_IMPL_H_
