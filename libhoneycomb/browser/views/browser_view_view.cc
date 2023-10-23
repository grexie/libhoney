// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/browser_view_view.h"

#include "libhoneycomb/browser/views/browser_view_impl.h"

HoneycombBrowserViewView::HoneycombBrowserViewView(HoneycombBrowserViewDelegate* honey_delegate,
                                       Delegate* browser_view_delegate)
    : ParentClass(honey_delegate), browser_view_delegate_(browser_view_delegate) {
  DCHECK(browser_view_delegate_);
}

void HoneycombBrowserViewView::ViewHierarchyChanged(
    const views::ViewHierarchyChangedDetails& details) {
  ParentClass::ViewHierarchyChanged(details);
  if (details.is_add && details.child == this) {
    gfx::Size size = GetPreferredSize();
    if (size.IsEmpty()) {
      // No size was provided for this View. Size it to the parent by default
      // or, depending on the Layout, the browser may be initially 0x0 size and
      // will not display until the parent is next resized (resulting in a call
      // to WebView::OnBoundsChanged). For example, this can happen when adding
      // this View to a HoneycombWindow with FillLayout and then calling
      // HoneycombWindow::Show() without first resizing the HoneycombWindow.
      size = details.parent->GetPreferredSize();
      if (!size.IsEmpty()) {
        SetSize(size);
      }
    }

    browser_view_delegate_->OnBrowserViewAdded();
  }
}

void HoneycombBrowserViewView::OnBoundsChanged(const gfx::Rect& previous_bounds) {
  ParentClass::OnBoundsChanged(previous_bounds);
  browser_view_delegate_->OnBoundsChanged();
}

void HoneycombBrowserViewView::OnGestureEvent(ui::GestureEvent* event) {
  if (browser_view_delegate_->OnGestureEvent(event)) {
    return;
  }
  ParentClass::OnGestureEvent(event);
}
