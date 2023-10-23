// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_PANEL_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_PANEL_IMPL_H_
#pragma once

#include "include/views/honey_fill_layout.h"
#include "include/views/honey_layout.h"
#include "include/views/honey_panel.h"
#include "include/views/honey_window.h"

#include "libhoneycomb/browser/views/box_layout_impl.h"
#include "libhoneycomb/browser/views/fill_layout_impl.h"
#include "libhoneycomb/browser/views/layout_util.h"
#include "libhoneycomb/browser/views/view_impl.h"

#include "base/logging.h"

// Helpers for template boiler-plate.
#define HONEYCOMB_PANEL_IMPL_T HONEYCOMB_VIEW_IMPL_T
#define HONEYCOMB_PANEL_IMPL_A HONEYCOMB_VIEW_IMPL_A
#define HONEYCOMB_PANEL_IMPL_D HoneycombPanelImpl<HONEYCOMB_PANEL_IMPL_A>

// Template for implementing HoneycombPanel-derived classes. See comments in
// view_impl.h for a usage overview.
HONEYCOMB_PANEL_IMPL_T class HoneycombPanelImpl : public HONEYCOMB_VIEW_IMPL_D {
 public:
  using ParentClass = HONEYCOMB_VIEW_IMPL_D;

  // HoneycombPanel methods. When adding new As*() methods make sure to update
  // HoneycombViewAdapter::GetFor() in view_adapter.cc.
  HoneycombRefPtr<HoneycombWindow> AsWindow() override { return nullptr; }
  HoneycombRefPtr<HoneycombFillLayout> SetToFillLayout() override;
  HoneycombRefPtr<HoneycombBoxLayout> SetToBoxLayout(
      const HoneycombBoxLayoutSettings& settings) override;
  HoneycombRefPtr<HoneycombLayout> GetLayout() override;
  void Layout() override;
  void AddChildView(HoneycombRefPtr<HoneycombView> view) override;
  void AddChildViewAt(HoneycombRefPtr<HoneycombView> view, int index) override;
  void ReorderChildView(HoneycombRefPtr<HoneycombView> view, int index) override;
  void RemoveChildView(HoneycombRefPtr<HoneycombView> view) override;
  void RemoveAllChildViews() override;
  size_t GetChildViewCount() override;
  HoneycombRefPtr<HoneycombView> GetChildViewAt(int index) override;

  // HoneycombView methods:
  HoneycombRefPtr<HoneycombPanel> AsPanel() override { return this; }

  // HoneycombViewAdapter methods:
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override {
    ParentClass::GetDebugInfo(info, include_children);
    if (include_children) {
      const size_t count = ParentClass::content_view()->children().size();
      if (count > 0U) {
        base::Value::List children;

        for (size_t i = 0U; i < count; ++i) {
          views::View* view = ParentClass::content_view()->children()[i];
          HoneycombViewAdapter* adapter = HoneycombViewAdapter::GetFor(view);
          if (adapter) {
            base::Value::Dict child_info;
            adapter->GetDebugInfo(&child_info, include_children);
            children.Append(std::move(child_info));
          }
        }

        info->Set("children", std::move(children));
      }
    }
  }

 protected:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  explicit HoneycombPanelImpl(HoneycombRefPtr<HoneycombViewDelegateClass> delegate)
      : ParentClass(delegate) {}

  void Initialize() override {
    ParentClass::Initialize();

    // Create the default layout object.
    SetToFillLayout();
  }
};

HONEYCOMB_PANEL_IMPL_T HoneycombRefPtr<HoneycombFillLayout> HONEYCOMB_PANEL_IMPL_D::SetToFillLayout() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  return HoneycombFillLayoutImpl::Create(ParentClass::content_view());
}

HONEYCOMB_PANEL_IMPL_T HoneycombRefPtr<HoneycombBoxLayout> HONEYCOMB_PANEL_IMPL_D::SetToBoxLayout(
    const HoneycombBoxLayoutSettings& settings) {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  return HoneycombBoxLayoutImpl::Create(settings, ParentClass::content_view());
}

HONEYCOMB_PANEL_IMPL_T HoneycombRefPtr<HoneycombLayout> HONEYCOMB_PANEL_IMPL_D::GetLayout() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  return layout_util::GetFor(ParentClass::content_view());
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::Layout() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  return ParentClass::root_view()->Layout();
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::AddChildView(HoneycombRefPtr<HoneycombView> view) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view.get());
  DCHECK(view->IsValid());
  if (!view.get() || !view->IsValid()) {
    return;
  }

  auto* view_ptr = view->IsAttached()
                       ? view_util::GetFor(view)
                       : view_util::PassOwnership(view).release();
  DCHECK(view_ptr);
  ParentClass::content_view()->AddChildView(view_ptr);
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::AddChildViewAt(HoneycombRefPtr<HoneycombView> view,
                                                       int index) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view.get());
  DCHECK(view->IsValid());
  DCHECK_GE(index, 0);
  DCHECK_LE(static_cast<unsigned int>(index),
            ParentClass::content_view()->children().size());
  if (!view.get() || !view->IsValid() || index < 0 ||
      (static_cast<unsigned int>(index) >
       ParentClass::content_view()->children().size())) {
    return;
  }

  auto* view_ptr = view->IsAttached()
                       ? view_util::GetFor(view)
                       : view_util::PassOwnership(view).release();
  DCHECK(view_ptr);
  ParentClass::content_view()->AddChildViewAt(view_ptr, index);
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::ReorderChildView(
    HoneycombRefPtr<HoneycombView> view,
    int index) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view.get());
  DCHECK(view->IsValid());
  DCHECK(view->IsAttached());
  if (!view.get() || !view->IsValid() || !view->IsAttached()) {
    return;
  }

  views::View* view_ptr = view_util::GetFor(view);
  DCHECK(view_ptr);
  DCHECK_EQ(view_ptr->parent(), ParentClass::content_view());
  if (!view_ptr || view_ptr->parent() != ParentClass::content_view()) {
    return;
  }

  ParentClass::content_view()->ReorderChildView(view_ptr, index);
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::RemoveChildView(
    HoneycombRefPtr<HoneycombView> view) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view.get());
  DCHECK(view->IsValid());
  DCHECK(view->IsAttached());
  if (!view.get() || !view->IsValid() || !view->IsAttached()) {
    return;
  }

  views::View* view_ptr = view_util::GetFor(view);
  DCHECK(view_ptr);
  DCHECK_EQ(view_ptr->parent(), ParentClass::content_view());
  if (!view_ptr || view_ptr->parent() != ParentClass::content_view()) {
    return;
  }

  ParentClass::content_view()->RemoveChildView(view_ptr);
  view_util::ResumeOwnership(view);
}

HONEYCOMB_PANEL_IMPL_T void HONEYCOMB_PANEL_IMPL_D::RemoveAllChildViews() {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  while (!ParentClass::content_view()->children().empty()) {
    HoneycombRefPtr<HoneycombView> view = view_util::GetFor(
        ParentClass::content_view()->children().front(), false);
    RemoveChildView(view);
  }
}

HONEYCOMB_PANEL_IMPL_T size_t HONEYCOMB_PANEL_IMPL_D::GetChildViewCount() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0U);
  return ParentClass::content_view()->children().size();
}

HONEYCOMB_PANEL_IMPL_T HoneycombRefPtr<HoneycombView> HONEYCOMB_PANEL_IMPL_D::GetChildViewAt(
    int index) {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  DCHECK_GE(index, 0);
  DCHECK_LT(static_cast<unsigned int>(index),
            ParentClass::content_view()->children().size());
  if (index < 0 || (static_cast<unsigned int>(index) >=
                    ParentClass::content_view()->children().size())) {
    return nullptr;
  }

  HoneycombRefPtr<HoneycombView> view =
      view_util::GetFor(ParentClass::content_view()->children()[index], false);
  DCHECK(view);
  return view;
}

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_PANEL_IMPL_H_
