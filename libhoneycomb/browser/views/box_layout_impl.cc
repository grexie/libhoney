// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/box_layout_impl.h"

#include "libhoneycomb/browser/thread_util.h"
#include "libhoneycomb/browser/views/view_util.h"

// static
HoneycombRefPtr<HoneycombBoxLayoutImpl> HoneycombBoxLayoutImpl::Create(
    const HoneycombBoxLayoutSettings& settings,
    views::View* owner_view) {
  HONEYCOMB_REQUIRE_UIT_RETURN(nullptr);
  HoneycombRefPtr<HoneycombBoxLayoutImpl> impl = new HoneycombBoxLayoutImpl(settings);
  impl->Initialize(owner_view);
  return impl;
}

void HoneycombBoxLayoutImpl::SetFlexForView(HoneycombRefPtr<HoneycombView> view, int flex) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK_GE(flex, 0);
  if (flex < 0) {
    return;
  }

  DCHECK(view && view->IsValid() && view->IsAttached());
  if (!view || !view->IsValid() || !view->IsAttached()) {
    return;
  }

  views::View* view_ptr = view_util::GetFor(view);
  DCHECK_EQ(view_ptr->parent(), owner_view());
  if (view_ptr->parent() != owner_view()) {
    return;
  }

  layout()->SetFlexForView(view_ptr, flex);
}

void HoneycombBoxLayoutImpl::ClearFlexForView(HoneycombRefPtr<HoneycombView> view) {
  HONEYCOMB_REQUIRE_VALID_RETURN_VOID();
  DCHECK(view && view->IsValid() && view->IsAttached());
  if (!view || !view->IsValid() || !view->IsAttached()) {
    return;
  }

  views::View* view_ptr = view_util::GetFor(view);
  DCHECK_EQ(view_ptr->parent(), owner_view());
  if (view_ptr->parent() != owner_view()) {
    return;
  }

  layout()->ClearFlexForView(view_ptr);
}

HoneycombBoxLayoutImpl::HoneycombBoxLayoutImpl(const HoneycombBoxLayoutSettings& settings)
    : settings_(settings) {}

views::BoxLayout* HoneycombBoxLayoutImpl::CreateLayout() {
  views::BoxLayout* layout = new views::BoxLayout(
      settings_.horizontal ? views::BoxLayout::Orientation::kHorizontal
                           : views::BoxLayout::Orientation::kVertical,
      gfx::Insets::VH(settings_.inside_border_vertical_spacing,
                      settings_.inside_border_horizontal_spacing),
      settings_.between_child_spacing);
  layout->set_main_axis_alignment(
      static_cast<views::BoxLayout::MainAxisAlignment>(
          settings_.main_axis_alignment));
  layout->set_cross_axis_alignment(
      static_cast<views::BoxLayout::CrossAxisAlignment>(
          settings_.cross_axis_alignment));
  layout->set_inside_border_insets(gfx::Insets::TLBR(
      settings_.inside_border_insets.top, settings_.inside_border_insets.left,
      settings_.inside_border_insets.bottom,
      settings_.inside_border_insets.right));
  layout->set_minimum_cross_axis_size(settings_.minimum_cross_axis_size);
  if (settings_.default_flex > 0) {
    layout->SetDefaultFlex(settings_.default_flex);
  }
  return layout;
}
