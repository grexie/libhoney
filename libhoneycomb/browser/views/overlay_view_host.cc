// Copyright 2021 The Honeycomb Authors. Portions copyright
// 2011 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "libhoneycomb/browser/views/overlay_view_host.h"

#include "libhoneycomb/browser/views/view_util.h"
#include "libhoneycomb/browser/views/window_view.h"

#include "base/i18n/rtl.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/theme_copying_widget.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/compositor/compositor.h"
#include "ui/compositor/layer.h"

namespace {

class HoneycombOverlayControllerImpl : public HoneycombOverlayController {
 public:
  HoneycombOverlayControllerImpl(HoneycombOverlayViewHost* host, HoneycombRefPtr<HoneycombView> view)
      : host_(host), view_(view) {}

  HoneycombOverlayControllerImpl(const HoneycombOverlayControllerImpl&) = delete;
  HoneycombOverlayControllerImpl& operator=(const HoneycombOverlayControllerImpl&) = delete;

  bool IsValid() override {
    // View validity implies that HoneycombOverlayViewHost is still valid, because the
    // Widget that it owns (and that owns the View) is still valid.
    return view_ && view_->IsValid();
  }

  bool IsSame(HoneycombRefPtr<HoneycombOverlayController> that) override {
    return that && that->GetContentsView()->IsSame(view_);
  }

  HoneycombRefPtr<HoneycombView> GetContentsView() override { return view_; }

  HoneycombRefPtr<HoneycombWindow> GetWindow() override {
    if (IsValid()) {
      return view_util::GetWindowFor(host_->window_view()->GetWidget());
    }
    return nullptr;
  }

  honey_docking_mode_t GetDockingMode() override {
    if (IsValid()) {
      return host_->docking_mode();
    }
    return HONEYCOMB_DOCKING_MODE_TOP_LEFT;
  }

  void Destroy() override {
    if (IsValid()) {
      host_->Destroy();
      view_ = nullptr;
    }
  }

  void SetBounds(const HoneycombRect& bounds) override {
    if (IsValid() && host_->docking_mode() == HONEYCOMB_DOCKING_MODE_CUSTOM) {
      host_->SetOverlayBounds(
          gfx::Rect(bounds.x, bounds.y, bounds.width, bounds.height));
    }
  }

  HoneycombRect GetBounds() override {
    if (IsValid()) {
      const auto& bounds = host_->bounds();
      return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
    }
    return HoneycombRect();
  }

  HoneycombRect GetBoundsInScreen() override {
    if (IsValid()) {
      const auto& bounds = host_->widget()->GetWindowBoundsInScreen();
      return HoneycombRect(bounds.x(), bounds.y(), bounds.width(), bounds.height());
    }
    return HoneycombRect();
  }

  void SetSize(const HoneycombSize& size) override {
    if (IsValid() && host_->docking_mode() == HONEYCOMB_DOCKING_MODE_CUSTOM) {
      // Update the size without changing the origin.
      const auto& origin = host_->bounds().origin();
      host_->SetOverlayBounds(
          gfx::Rect(origin, gfx::Size(size.width, size.height)));
    }
  }

  HoneycombSize GetSize() override {
    const auto& bounds = GetBounds();
    return HoneycombSize(bounds.width, bounds.height);
  }

  void SetPosition(const HoneycombPoint& position) override {
    if (IsValid() && host_->docking_mode() == HONEYCOMB_DOCKING_MODE_CUSTOM) {
      // Update the origin without changing the size.
      const auto& size = host_->bounds().size();
      host_->SetOverlayBounds(
          gfx::Rect(gfx::Point(position.x, position.y), size));
    }
  }

  HoneycombPoint GetPosition() override {
    const auto& bounds = GetBounds();
    return HoneycombPoint(bounds.x, bounds.y);
  }

  void SetInsets(const HoneycombInsets& insets) override {
    if (IsValid() && host_->docking_mode() != HONEYCOMB_DOCKING_MODE_CUSTOM) {
      host_->SetOverlayInsets(insets);
    }
  }

  HoneycombInsets GetInsets() override {
    if (IsValid()) {
      return host_->insets();
    }
    return HoneycombInsets();
  }

  void SizeToPreferredSize() override {
    if (IsValid()) {
      if (host_->docking_mode() == HONEYCOMB_DOCKING_MODE_CUSTOM) {
        // Update the size without changing the origin.
        const auto& origin = host_->bounds().origin();
        const auto& preferred_size = host_->view()->GetPreferredSize();
        host_->SetOverlayBounds(gfx::Rect(origin, preferred_size));
      } else {
        host_->MoveIfNecessary();
      }
    }
  }

  void SetVisible(bool visible) override {
    if (IsValid()) {
      if (visible) {
        host_->MoveIfNecessary();
        host_->widget()->Show();
      } else {
        host_->widget()->Hide();
      }
    }
  }

  bool IsVisible() override {
    if (IsValid()) {
      return host_->widget()->IsVisible();
    }
    return false;
  }

  bool IsDrawn() override { return IsVisible(); }

 private:
  HoneycombOverlayViewHost* const host_;
  HoneycombRefPtr<HoneycombView> view_;

  IMPLEMENT_REFCOUNTING(HoneycombOverlayControllerImpl);
};

}  // namespace

HoneycombOverlayViewHost::HoneycombOverlayViewHost(HoneycombWindowView* window_view,
                                       honey_docking_mode_t docking_mode)
    : window_view_(window_view), docking_mode_(docking_mode) {}

void HoneycombOverlayViewHost::Init(views::View* host_view, HoneycombRefPtr<HoneycombView> view) {
  DCHECK(view);

  // Match the logic in HONEYCOMB_PANEL_IMPL_D::AddChildView().
  auto controls_view = view->IsAttached()
                           ? base::WrapUnique(view_util::GetFor(view))
                           : view_util::PassOwnership(view);
  DCHECK(controls_view.get());

  honey_controller_ = new HoneycombOverlayControllerImpl(this, view);

  // Initialize the Widget.
  widget_ = std::make_unique<ThemeCopyingWidget>(window_view_->GetWidget());
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_CONTROL);
  params.delegate = this;
  params.name = "HoneycombOverlayViewHost";
  params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
  params.parent = window_view_->GetWidget()->GetNativeView();
  params.opacity = views::Widget::InitParams::WindowOpacity::kTranslucent;
  params.activatable = views::Widget::InitParams::Activatable::kNo;
  widget_->Init(std::move(params));

  view_ = widget_->GetContentsView()->AddChildView(std::move(controls_view));

  // Make the Widget background transparent. The View might still be opaque.
  if (widget_->GetCompositor()) {
    widget_->GetCompositor()->SetBackgroundColor(SK_ColorTRANSPARENT);
  }

  view_util::SetHostView(widget_.get(), host_view);

  if (honey::IsChromeRuntimeEnabled()) {
    // Some attributes associated with a Chrome toolbar are located via the
    // Widget. See matching logic in BrowserView::AddedToWidget.
    auto browser_view = BrowserView::GetBrowserViewForNativeWindow(
        view_util::GetNativeWindow(window_view_->GetWidget()));
    if (browser_view) {
      widget_->SetNativeWindowProperty(BrowserView::kBrowserViewKey,
                                       browser_view);
    }
  }

  // Set the initial bounds after the View has been added to the Widget.
  // Otherwise, preferred size won't calculate correctly.
  gfx::Rect bounds;
  if (docking_mode_ == HONEYCOMB_DOCKING_MODE_CUSTOM) {
    if (view_->size().IsEmpty()) {
      // Size to the preferred size to start.
      view_->SizeToPreferredSize();
    }

    // Top-left origin with existing size.
    bounds = gfx::Rect(gfx::Point(), view_->size());
  } else {
    bounds = ComputeBounds();
  }
  SetOverlayBounds(bounds);

  // Register for future bounds change notifications.
  view_->AddObserver(this);

  // Initially hidden.
  widget_->Hide();
}

void HoneycombOverlayViewHost::Destroy() {
  if (widget_ && !widget_->IsClosed()) {
    // Remove the child View immediately. It may be reused by the client.
    auto view = view_util::GetFor(view_, /*find_known_parent=*/false);
    widget_->GetContentsView()->RemoveChildView(view_);
    if (view) {
      view_util::ResumeOwnership(view);
    }

    widget_->Close();
  }
}

void HoneycombOverlayViewHost::MoveIfNecessary() {
  if (bounds_changing_ || docking_mode_ == HONEYCOMB_DOCKING_MODE_CUSTOM) {
    return;
  }
  SetOverlayBounds(ComputeBounds());
}

void HoneycombOverlayViewHost::SetOverlayBounds(const gfx::Rect& bounds) {
  // Avoid re-entrancy of this method.
  if (bounds_changing_) {
    return;
  }

  gfx::Rect new_bounds = bounds;

  // Keep the result inside the widget.
  new_bounds.Intersect(window_view_->bounds());

  if (new_bounds == bounds_) {
    return;
  }

  bounds_changing_ = true;

  bounds_ = new_bounds;
  if (view_->size() != bounds_.size()) {
    view_->SetSize(bounds_.size());
  }
  widget_->SetBounds(bounds_);

  bounds_changing_ = false;
}

void HoneycombOverlayViewHost::SetOverlayInsets(const HoneycombInsets& insets) {
  if (insets == insets_) {
    return;
  }
  insets_ = insets;
  MoveIfNecessary();
}

void HoneycombOverlayViewHost::OnViewBoundsChanged(views::View* observed_view) {
  MoveIfNecessary();
}

gfx::Rect HoneycombOverlayViewHost::ComputeBounds() const {
  // This method is only used with corner docking.
  DCHECK_NE(docking_mode_, HONEYCOMB_DOCKING_MODE_CUSTOM);

  // Find the area we have to work with.
  const auto& widget_bounds = window_view_->bounds();

  // Ask the view how large an area it needs to draw on.
  const auto& prefsize = view_->GetPreferredSize();

  // Swap left/right docking with RTL.
  const bool is_rtl = base::i18n::IsRTL();

  // Dock to the correct corner, considering insets in the docking corner only.
  int x = widget_bounds.x();
  int y = widget_bounds.y();
  if (((docking_mode_ == HONEYCOMB_DOCKING_MODE_TOP_RIGHT ||
        docking_mode_ == HONEYCOMB_DOCKING_MODE_BOTTOM_RIGHT) &&
       !is_rtl) ||
      ((docking_mode_ == HONEYCOMB_DOCKING_MODE_TOP_LEFT ||
        docking_mode_ == HONEYCOMB_DOCKING_MODE_BOTTOM_LEFT) &&
       is_rtl)) {
    x += widget_bounds.width() - prefsize.width() - insets_.right;
  } else {
    x += insets_.left;
  }
  if (docking_mode_ == HONEYCOMB_DOCKING_MODE_BOTTOM_LEFT ||
      docking_mode_ == HONEYCOMB_DOCKING_MODE_BOTTOM_RIGHT) {
    y += widget_bounds.height() - prefsize.height() - insets_.bottom;
  } else {
    y += insets_.top;
  }

  return gfx::Rect(x, y, prefsize.width(), prefsize.height());
}
