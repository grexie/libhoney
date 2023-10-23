// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_VIEWS_VIEW_VIEW_H_
#define HONEYCOMB_LIBHONEY_BROWSER_VIEWS_VIEW_VIEW_H_
#pragma once

#include "include/views/honey_view.h"
#include "include/views/honey_view_delegate.h"

#include "libhoney/browser/thread_util.h"
#include "libhoney/browser/views/view_util.h"

#include "base/logging.h"
#include "ui/views/accessibility/accessibility_paint_checks.h"
#include "ui/views/background.h"
#include "ui/views/view.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_VIEW_VIEW_T \
  template <class ViewsViewClass, class HoneycombViewDelegateClass>
#define HONEYCOMB_VIEW_VIEW_A ViewsViewClass, HoneycombViewDelegateClass
#define HONEYCOMB_VIEW_VIEW_D HoneycombViewView<HONEYCOMB_VIEW_VIEW_A>

// Base template for implementing views::View-derived classes. The views::View-
// derived type passed to this template must provide a no-argument constructor
// (for example, see LabelButtonEx from basic_label_button_view.h). See comments
// in view_impl.h for a usage overview.
HONEYCOMB_VIEW_VIEW_T class HoneycombViewView : public ViewsViewClass {
 public:
  using ParentClass = ViewsViewClass;

  // Should be created from CreateRootView() in a HoneycombViewImpl-derived class.
  // Do not call complex views::View-derived methods from a HoneycombViewView-derived
  // constructor as they may attempt to call back into HoneycombViewImpl before
  // registration has been performed. |honey_delegate| may be nullptr.
  template <typename... Args>
  explicit HoneycombViewView(HoneycombViewDelegateClass* honey_delegate, Args... args)
      : ParentClass(args...), honey_delegate_(honey_delegate) {}

  // Should be called from InitializeRootView() in the HoneycombViewImpl-derived
  // class that created this object. This method will be called after
  // HoneycombViewImpl registration has completed so it is safe to call complex
  // views::View-derived methods here.
  virtual void Initialize() {
    // Use our defaults instead of the Views framework defaults.
    ParentClass::SetBackground(
        views::CreateSolidBackground(view_util::kDefaultBackgroundColor));

    // TODO(crbug.com/1218186): Remove this, if this view is focusable then it
    // needs to add a name so that the screen reader knows what to announce.
    ParentClass::SetProperty(views::kSkipAccessibilityPaintChecks, true);
  }

  // Returns the HoneycombViewDelegate-derived delegate associated with this view.
  // May return nullptr.
  HoneycombViewDelegateClass* honey_delegate() const { return honey_delegate_; }

  // Returns the HoneycombView associated with this view. May return nullptr during
  // HoneycombViewImpl initialization. If callbacks to the HoneycombViewImpl-derived class
  // are required define an interface that the HoneycombViewImpl-derived class can
  // implement and pass as an unowned instance to this object's constructor (see
  // for example HoneycombWindowView).
  HoneycombRefPtr<HoneycombView> GetHoneycombView() const {
    HoneycombRefPtr<HoneycombView> view = view_util::GetFor(this, false);
    DCHECK(view);
    return view;
  }

  // views::View methods:
  gfx::Size CalculatePreferredSize() const override;
  gfx::Size GetMinimumSize() const override;
  gfx::Size GetMaximumSize() const override;
  int GetHeightForWidth(int w) const override;
  void Layout() override;
  void ViewHierarchyChanged(
      const views::ViewHierarchyChangedDetails& details) override;
  void AddedToWidget() override;
  void RemovedFromWidget() override;
  void OnFocus() override;
  void OnBlur() override;

  // Return true if this View is expected to have a minimum size (for example,
  // a button where the minimum size is based on the label).
  virtual bool HasMinimumSize() const { return false; }

 private:
  void NotifyChildViewChanged(
      const views::ViewHierarchyChangedDetails& details);
  void NotifyParentViewChanged(
      const views::ViewHierarchyChangedDetails& details);

  // Not owned by this object.
  HoneycombViewDelegateClass* const honey_delegate_;
};

HONEYCOMB_VIEW_VIEW_T gfx::Size HONEYCOMB_VIEW_VIEW_D::CalculatePreferredSize() const {
  gfx::Size result;
  if (honey_delegate()) {
    HoneycombSize honey_size = honey_delegate()->GetPreferredSize(GetHoneycombView());
    if (!honey_size.IsEmpty()) {
      result = gfx::Size(honey_size.width, honey_size.height);
    }
  }
  if (result.IsEmpty()) {
    result = ParentClass::CalculatePreferredSize();
  }
  if (result.IsEmpty()) {
    // Some layouts like BoxLayout expect the preferred size to be non-empty.
    // The user may have set the size explicitly. Therefore return the current
    // size as the preferred size.
    result = ParentClass::size();
  }
  return result;
}

HONEYCOMB_VIEW_VIEW_T gfx::Size HONEYCOMB_VIEW_VIEW_D::GetMinimumSize() const {
  gfx::Size result;
  if (honey_delegate()) {
    HoneycombSize honey_size = honey_delegate()->GetMinimumSize(GetHoneycombView());
    if (!honey_size.IsEmpty()) {
      result = gfx::Size(honey_size.width, honey_size.height);
    }
  }
  // We don't want to call ParentClass::GetMinimumSize() in all cases because
  // the default views::View implementation will call GetPreferredSize(). That
  // may result in size() being returned which keeps the View from shrinking.
  if (result.IsEmpty() && HasMinimumSize()) {
    result = ParentClass::GetMinimumSize();
  }
  return result;
}

HONEYCOMB_VIEW_VIEW_T gfx::Size HONEYCOMB_VIEW_VIEW_D::GetMaximumSize() const {
  gfx::Size result;
  if (honey_delegate()) {
    HoneycombSize honey_size = honey_delegate()->GetMaximumSize(GetHoneycombView());
    if (!honey_size.IsEmpty()) {
      result = gfx::Size(honey_size.width, honey_size.height);
    }
  }
  if (result.IsEmpty()) {
    result = ParentClass::GetMaximumSize();
  }
  return result;
}

HONEYCOMB_VIEW_VIEW_T int HONEYCOMB_VIEW_VIEW_D::GetHeightForWidth(int w) const {
  int result = 0;
  if (honey_delegate()) {
    result = honey_delegate()->GetHeightForWidth(GetHoneycombView(), w);
  }
  if (result == 0) {
    result = ParentClass::GetHeightForWidth(w);
  }
  if (result == 0) {
    // Some layouts like FillLayout will ignore the preferred size if this view
    // has no children. We want to use the preferred size if not otherwise
    // specified.
    result = ParentClass::GetPreferredSize().height();
  }
  return result;
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::Layout() {
  ParentClass::Layout();

  // If Layout() did not provide a size then use the preferred size.
  if (ParentClass::size().IsEmpty()) {
    ParentClass::SizeToPreferredSize();
  }

  if (honey_delegate()) {
    const auto new_bounds = ParentClass::bounds();
    HoneycombRect new_rect(new_bounds.x(), new_bounds.y(), new_bounds.width(),
                     new_bounds.height());
    honey_delegate()->OnLayoutChanged(GetHoneycombView(), new_rect);
  }
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::ViewHierarchyChanged(
    const views::ViewHierarchyChangedDetails& details) {
  NotifyChildViewChanged(details);
  NotifyParentViewChanged(details);
  ParentClass::ViewHierarchyChanged(details);
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::AddedToWidget() {
  ParentClass::AddedToWidget();
  if (honey_delegate()) {
    honey_delegate()->OnWindowChanged(GetHoneycombView(), /*added=*/true);
  }
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::RemovedFromWidget() {
  if (honey_delegate()) {
    honey_delegate()->OnWindowChanged(GetHoneycombView(), /*added=*/false);
  }
  ParentClass::RemovedFromWidget();
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::OnFocus() {
  if (honey_delegate()) {
    honey_delegate()->OnFocus(GetHoneycombView());
  }
  ParentClass::OnFocus();
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::OnBlur() {
  if (honey_delegate()) {
    honey_delegate()->OnBlur(GetHoneycombView());
  }
  ParentClass::OnBlur();
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::NotifyChildViewChanged(
    const views::ViewHierarchyChangedDetails& details) {
  if (!honey_delegate()) {
    return;
  }

  // Only interested with the parent is |this| object and the notification is
  // about an immediate child (notifications are also sent for grandchildren).
  if (details.parent != this || details.child->parent() != this) {
    return;
  }

  // Only notify for children that have a known Honeycomb root view. For example,
  // don't notify when ScrollView adds child scroll bars.
  HoneycombRefPtr<HoneycombView> child = view_util::GetFor(details.child, false);
  if (child) {
    honey_delegate()->OnChildViewChanged(GetHoneycombView(), details.is_add, child);
  }
}

HONEYCOMB_VIEW_VIEW_T void HONEYCOMB_VIEW_VIEW_D::NotifyParentViewChanged(
    const views::ViewHierarchyChangedDetails& details) {
  if (!honey_delegate()) {
    return;
  }

  // Only interested when the child is |this| object and notification is about
  // the immediate parent (notifications are sent for all parents).
  if (details.child != this || details.parent != ParentClass::parent()) {
    return;
  }

  // The immediate parent might be an intermediate view so find the closest
  // known Honeycomb root view. |parent| might be nullptr for overlays.
  HoneycombRefPtr<HoneycombView> parent = view_util::GetFor(details.parent, true);
  if (parent) {
    honey_delegate()->OnParentViewChanged(GetHoneycombView(), details.is_add, parent);
  }
}

#endif  // HONEYCOMB_LIBHONEY_BROWSER_VIEWS_VIEW_VIEW_H_
