// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_SCROLL_VIEW_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_SCROLL_VIEW_IMPL_H_
#pragma once

#include "include/views/honey_scroll_view.h"
#include "include/views/honey_view_delegate.h"

#include "libhoneycomb/browser/views/scroll_view_view.h"
#include "libhoneycomb/browser/views/view_impl.h"

class HoneycombScrollViewImpl
    : public HoneycombViewImpl<HoneycombScrollViewView, HoneycombScrollView, HoneycombViewDelegate> {
 public:
  using ParentClass =
      HoneycombViewImpl<HoneycombScrollViewView, HoneycombScrollView, HoneycombViewDelegate>;

  HoneycombScrollViewImpl(const HoneycombScrollViewImpl&) = delete;
  HoneycombScrollViewImpl& operator=(const HoneycombScrollViewImpl&) = delete;

  // Create a new HoneycombScrollView instance. |delegate| may be nullptr.
  static HoneycombRefPtr<HoneycombScrollViewImpl> Create(
      HoneycombRefPtr<HoneycombViewDelegate> delegate);

  // HoneycombScrollView methods:
  HoneycombRefPtr<HoneycombScrollView> AsScrollView() override { return this; }
  void SetContentView(HoneycombRefPtr<HoneycombView> view) override;
  HoneycombRefPtr<HoneycombView> GetContentView() override;
  HoneycombRect GetVisibleContentRect() override;
  bool HasHorizontalScrollbar() override;
  int GetHorizontalScrollbarHeight() override;
  bool HasVerticalScrollbar() override;
  int GetVerticalScrollbarWidth() override;

  // HoneycombViewAdapter methods:
  std::string GetDebugType() override { return "ScrollView"; }
  void GetDebugInfo(base::Value::Dict* info, bool include_children) override;

 private:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  // |delegate| may be nullptr.
  HoneycombScrollViewImpl(HoneycombRefPtr<HoneycombViewDelegate> delegate);

  // HoneycombViewImpl methods:
  HoneycombScrollViewView* CreateRootView() override;
  void InitializeRootView() override;

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(HoneycombScrollViewImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_SCROLL_VIEW_IMPL_H_
