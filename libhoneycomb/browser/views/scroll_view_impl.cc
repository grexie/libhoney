// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/scroll_view_impl.h"

// static
HoneycombRefPtr<HoneycombScrollView> HoneycombScrollView::CreateScrollView(
    HoneycombRefPtr<HoneycombViewDelegate> delegate) {
  return HoneycombScrollViewImpl::Create(delegate);
}

// static
HoneycombRefPtr<HoneycombScrollViewImpl> HoneycombScrollViewImpl::Create(
    HoneycombRefPtr<HoneycombViewDelegate> delegate) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombScrollViewImpl> view = new HoneycombScrollViewImpl(delegate);
  view->Initialize();
  return view;
}

void HoneycombScrollViewImpl::SetContentView(HoneycombRefPtr<HoneycombView> view) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view.get());
  DCHECK(view->IsValid());
  DCHECK(!view->IsAttached());
  if (!view.get() || !view->IsValid() || view->IsAttached()) {
    return;
  }

  root_view()->SetContents(view_util::PassOwnership(view));
}

HoneycombRefPtr<HoneycombView> HoneycombScrollViewImpl::GetContentView() {
  HONEYCOMB_REQUIRE_VALID_RETURN(nullptr);
  return view_util::GetFor(root_view()->contents(), false);
}

HoneycombRect HoneycombScrollViewImpl::GetVisibleContentRect() {
  HONEYCOMB_REQUIRE_VALID_RETURN(HoneycombRect());
  const gfx::Rect& rect = root_view()->GetVisibleRect();
  return HoneycombRect(rect.x(), rect.y(), rect.width(), rect.height());
}

bool HoneycombScrollViewImpl::HasHorizontalScrollbar() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  const views::ScrollBar* scrollbar = root_view()->horizontal_scroll_bar();
  return scrollbar && scrollbar->GetVisible();
}

int HoneycombScrollViewImpl::GetHorizontalScrollbarHeight() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0);
  return root_view()->GetScrollBarLayoutHeight();
}

bool HoneycombScrollViewImpl::HasVerticalScrollbar() {
  HONEYCOMB_REQUIRE_VALID_RETURN(false);
  const views::ScrollBar* scrollbar = root_view()->vertical_scroll_bar();
  return scrollbar && scrollbar->GetVisible();
}

int HoneycombScrollViewImpl::GetVerticalScrollbarWidth() {
  HONEYCOMB_REQUIRE_VALID_RETURN(0);
  return root_view()->GetScrollBarLayoutWidth();
}

void HoneycombScrollViewImpl::GetDebugInfo(base::Value::Dict* info,
                                     bool include_children) {
  ParentClass::GetDebugInfo(info, include_children);
  if (include_children) {
    views::View* view = root_view()->contents();
    HoneycombViewAdapter* adapter = HoneycombViewAdapter::GetFor(view);
    if (adapter) {
      base::Value::Dict child_info;
      adapter->GetDebugInfo(&child_info, include_children);
      info->Set("content_view", std::move(child_info));
    }
  }
}

HoneycombScrollViewImpl::HoneycombScrollViewImpl(HoneycombRefPtr<HoneycombViewDelegate> delegate)
    : ParentClass(delegate) {}

HoneycombScrollViewView* HoneycombScrollViewImpl::CreateRootView() {
  return new HoneycombScrollViewView(delegate());
}

void HoneycombScrollViewImpl::InitializeRootView() {
  static_cast<HoneycombScrollViewView*>(root_view())->Initialize();
}
