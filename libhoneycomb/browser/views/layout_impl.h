// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LAYOUT_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LAYOUT_IMPL_H_
#pragma once

#include "include/views/honey_box_layout.h"
#include "include/views/honey_fill_layout.h"
#include "include/views/honey_layout.h"

#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/views/layout_adapter.h"
#include "libhoneycomb/browser/views/layout_util.h"

#include "base/logging.h"
#include "ui/views/layout/layout_manager.h"
#include "ui/views/view.h"

// Base template for implementing HoneycombLayout-derived classes. See comments in
// view_impl.h for a usage overview.
template <class ViewsLayoutClass, class HoneycombLayoutClass>
class HoneycombLayoutImpl : public HoneycombLayoutAdapter, public HoneycombLayoutClass {
 public:
  // Returns the underlying views::LayoutManager object as the derived type.
  // Does not transfer ownership.
  ViewsLayoutClass* layout() const { return layout_ref_; }

  // Returns the views::View that owns this object.
  views::View* owner_view() const { return owner_view_; }

  // HoneycombLayoutAdapter methods:
  views::LayoutManager* Get() const override { return layout(); }
  void Detach() override {
    owner_view_ = nullptr;
    layout_ref_ = nullptr;
  }

  // HoneycombLayout methods. When adding new As*() methods make sure to update
  // HoneycombLayoutAdapter::GetFor() in layout_adapter.cc.
  HoneycombRefPtr<HoneycombBoxLayout> AsBoxLayout() override { return nullptr; }
  HoneycombRefPtr<HoneycombFillLayout> AsFillLayout() override { return nullptr; }
  bool IsValid() override {
    HONEYCOMB_REQUIRE_UIT_RETURN(false);
    return !!layout_ref_;
  }

 protected:
  // Create a new implementation object.
  // Always call Initialize() after creation.
  HoneycombLayoutImpl() : layout_ref_(nullptr), owner_view_(nullptr) {}

  // Initialize this object and assign ownership to |owner_view|.
  void Initialize(views::View* owner_view) {
    DCHECK(owner_view);
    owner_view_ = owner_view;
    layout_ref_ = CreateLayout();
    DCHECK(layout_ref_);
    owner_view->SetLayoutManager(base::WrapUnique(layout_ref_));
    layout_util::Assign(this, owner_view);
  }

  // Create the views::LayoutManager object.
  virtual ViewsLayoutClass* CreateLayout() = 0;

 private:
  // Unowned reference to the views::LayoutManager wrapped by this object. Will
  // be nullptr after the views::LayoutManager is destroyed.
  ViewsLayoutClass* layout_ref_;

  // Unowned reference to the views::View that owns this object. Will be nullptr
  // after the views::LayoutManager is destroyed.
  views::View* owner_view_;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_VIEWS_LAYOUT_IMPL_H_
