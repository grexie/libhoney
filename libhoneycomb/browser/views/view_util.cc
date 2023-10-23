// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/views/view_util.h"

#include <utility>

#include "libhoneycomb/browser/views/view_adapter.h"

#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/window/non_client_view.h"

#if BUILDFLAG(IS_WIN)
#include "ui/display/win/screen_win.h"
#endif

namespace view_util {

namespace {

// Manages the association between views::View and HoneycombView instances.
class UserData : public base::SupportsUserData::Data {
 public:
  // Create the initial association between the views::View and the HoneycombView. The
  // HoneycombView owns the views::View at this stage.
  static void Register(HoneycombRefPtr<HoneycombView> honey_view) {
    DCHECK(honey_view->IsValid());
    DCHECK(!honey_view->IsAttached());

    views::View* view = HoneycombViewAdapter::GetFor(honey_view)->Get();
    DCHECK(view);

    // The HoneycombView should not already be registered.
    DCHECK(!view->GetUserData(UserDataKey()));

    view->SetUserData(UserDataKey(), base::WrapUnique(new UserData(honey_view)));
  }

  static HoneycombRefPtr<HoneycombView> GetFor(const views::View* view) {
    DCHECK(view);
    UserData* data = static_cast<UserData*>(view->GetUserData(UserDataKey()));
    if (data) {
      return data->view_ref_;
    }
    return nullptr;
  }

  // Transfer ownership of the views::View to the caller. The views::View will
  // gain a ref-counted reference to the HoneycombView and the HoneycombView will keep an
  // unowned reference to the views::View. Destruction of the views::View will
  // release the ref-counted reference to the HoneycombView.
  [[nodiscard]] static std::unique_ptr<views::View> PassOwnership(
      HoneycombRefPtr<HoneycombView> honey_view) {
    DCHECK(honey_view->IsValid());
    DCHECK(!honey_view->IsAttached());

    std::unique_ptr<views::View> view =
        HoneycombViewAdapter::GetFor(honey_view)->PassOwnership();
    DCHECK(view);

    UserData* data = static_cast<UserData*>(view->GetUserData(UserDataKey()));
    DCHECK(data);
    data->TakeReference();

    return view;
  }

  // The HoneycombView resumes ownership of the views::View. The views::View no longer
  // keeps a ref-counted reference to the HoneycombView.
  static void ResumeOwnership(HoneycombRefPtr<HoneycombView> honey_view) {
    DCHECK(honey_view->IsValid());
    DCHECK(honey_view->IsAttached());

    HoneycombViewAdapter* adapter = HoneycombViewAdapter::GetFor(honey_view);
    adapter->ResumeOwnership();

    views::View* view = adapter->Get();
    DCHECK(view);

    UserData* data = static_cast<UserData*>(view->GetUserData(UserDataKey()));
    DCHECK(data);
    data->ReleaseReference();
  }

 private:
  friend std::default_delete<UserData>;

  explicit UserData(HoneycombRefPtr<HoneycombView> honey_view) : view_ref_(honey_view.get()) {
    DCHECK(view_ref_);
  }

  ~UserData() override {
    if (view_) {
      // The HoneycombView does not own the views::View. Remove the HoneycombView's
      // reference to the views::View.
      HoneycombViewAdapter::GetFor(view_)->Detach();
    }
  }

  void TakeReference() { view_ = view_ref_; }

  void ReleaseReference() { view_ = nullptr; }

  static void* UserDataKey() {
    // We just need a unique constant. Use the address of a static that
    // COMDAT folding won't touch in an optimizing linker.
    static int data_key = 0;
    return reinterpret_cast<void*>(&data_key);
  }

  HoneycombRefPtr<HoneycombView> view_;
  HoneycombView* view_ref_;
};

}  // namespace

const SkColor kDefaultBackgroundColor = SkColorSetARGB(255, 255, 255, 255);
const char kDefaultFontList[] = "Arial, Helvetica, 14px";

void Register(HoneycombRefPtr<HoneycombView> view) {
  UserData::Register(view);
}

HoneycombRefPtr<HoneycombView> GetFor(const views::View* view, bool find_known_parent) {
  if (!view) {
    return nullptr;
  }

  if (!find_known_parent) {
    return UserData::GetFor(view);
  }

  HoneycombRefPtr<HoneycombView> honey_view;
  const views::View* current_view = view;
  do {
    honey_view = UserData::GetFor(current_view);
    if (honey_view) {
      break;
    }
    current_view = current_view->parent();
  } while (current_view);

  return honey_view;
}

views::View* GetFor(HoneycombRefPtr<HoneycombView> view) {
  return HoneycombViewAdapter::GetFor(view)->Get();
}

std::unique_ptr<views::View> PassOwnership(HoneycombRefPtr<HoneycombView> view) {
  return UserData::PassOwnership(view);
}

void ResumeOwnership(HoneycombRefPtr<HoneycombView> view) {
  UserData::ResumeOwnership(view);
}

HoneycombRefPtr<HoneycombWindow> GetWindowFor(views::Widget* widget) {
  HoneycombRefPtr<HoneycombWindow> window;

  // If |widget| is an overlay, retrieve the host Widget.
  if (widget) {
    if (auto widget_view = GetHostView(widget)) {
      widget = widget_view->GetWidget();
    }
  }

  if (widget) {
    // The views::WidgetDelegate should be a HoneycombWindowView and |content_view|
    // should be the same HoneycombWindowView. However, just in case the views::Widget
    // was created by something else let's go about this the safer way.
    views::View* content_view = widget->widget_delegate()->GetContentsView();
    HoneycombRefPtr<HoneycombView> honey_view = GetFor(content_view, false);
    if (honey_view && honey_view->AsPanel()) {
      window = honey_view->AsPanel()->AsWindow();
    }

    // The Window should always exist if we created the views::Widget.
    DCHECK(window);
  }

  return window;
}

display::Display GetDisplayNearestPoint(const gfx::Point& point,
                                        bool input_pixel_coords) {
  gfx::Point find_point = point;
#if BUILDFLAG(IS_WIN)
  if (input_pixel_coords) {
    find_point = gfx::ToFlooredPoint(
        display::win::ScreenWin::ScreenToDIPPoint(gfx::PointF(point)));
  }
#endif
  return display::Screen::GetScreen()->GetDisplayNearestPoint(find_point);
}

display::Display GetDisplayMatchingBounds(const gfx::Rect& bounds,
                                          bool input_pixel_coords) {
  gfx::Rect find_bounds = bounds;
#if BUILDFLAG(IS_WIN)
  if (input_pixel_coords) {
    find_bounds =
        display::win::ScreenWin::ScreenToDIPRect(nullptr, find_bounds);
  }
#endif
  return display::Screen::GetScreen()->GetDisplayMatching(find_bounds);
}

void ConvertPointFromPixels(gfx::Point* point, float device_scale_factor) {
  *point = gfx::ToFlooredPoint(
      gfx::ScalePoint(gfx::PointF(*point), 1.0f / device_scale_factor));
}

void ConvertPointToPixels(gfx::Point* point, float device_scale_factor) {
  *point = gfx::ToFlooredPoint(
      gfx::ScalePoint(gfx::PointF(*point), device_scale_factor));
}

#if BUILDFLAG(IS_WIN)
gfx::Point ConvertPointFromPixels(const gfx::Point& point) {
  return gfx::ToFlooredPoint(
      display::win::ScreenWin::ScreenToDIPPoint(gfx::PointF(point)));
}

gfx::Point ConvertPointToPixels(const gfx::Point& point) {
  return display::win::ScreenWin::DIPToScreenPoint(point);
}

gfx::Rect ConvertRectFromPixels(const gfx::Rect& rect) {
  return display::win::ScreenWin::ScreenToDIPRect(nullptr, rect);
}

gfx::Rect ConvertRectToPixels(const gfx::Rect& rect) {
  return display::win::ScreenWin::DIPToScreenRect(nullptr, rect);
}
#endif  // BUILDFLAG(IS_WIN)

bool ConvertPointToScreen(views::View* view,
                          gfx::Point* point,
                          bool output_pixel_coords) {
  if (!view->GetWidget()) {
    return false;
  }

  views::View::ConvertPointToScreen(view, point);

  if (output_pixel_coords) {
    const display::Display& display = GetDisplayNearestPoint(*point, false);
    ConvertPointToPixels(point, display.device_scale_factor());
  }

  return true;
}

bool ConvertPointFromScreen(views::View* view,
                            gfx::Point* point,
                            bool input_pixel_coords) {
  if (!view->GetWidget()) {
    return false;
  }

  if (input_pixel_coords) {
    const display::Display& display = GetDisplayNearestPoint(*point, true);
    ConvertPointFromPixels(point, display.device_scale_factor());
  }

  views::View::ConvertPointFromScreen(view, point);

  return true;
}

bool ConvertPointToWindow(views::View* view, gfx::Point* point) {
  views::Widget* widget = view->GetWidget();
  if (!widget) {
    return false;
  }

  views::View::ConvertPointToWidget(view, point);

  if (widget->non_client_view()) {
    views::NonClientFrameView* non_client_frame_view =
        widget->non_client_view()->frame_view();
    if (non_client_frame_view) {
      // When using a custom drawn NonClientFrameView the native Window will not
      // know the actual client bounds. Adjust the native Window bounds for the
      // reported client bounds.
      const gfx::Rect& client_bounds =
          non_client_frame_view->GetBoundsForClientView();
      *point -= client_bounds.OffsetFromOrigin();
    }
  }

  return true;
}

bool ConvertPointFromWindow(views::View* view, gfx::Point* point) {
  views::Widget* widget = view->GetWidget();
  if (!widget) {
    return false;
  }

  if (widget->non_client_view()) {
    views::NonClientFrameView* non_client_frame_view =
        widget->non_client_view()->frame_view();
    if (non_client_frame_view) {
      // When using a custom drawn NonClientFrameView the native Window will not
      // know the actual client bounds. Adjust the native Window bounds for the
      // reported client bounds.
      const gfx::Rect& client_bounds =
          non_client_frame_view->GetBoundsForClientView();
      *point += client_bounds.OffsetFromOrigin();
    }
  }

  views::View::ConvertPointFromWidget(view, point);

  return true;
}

}  // namespace view_util
