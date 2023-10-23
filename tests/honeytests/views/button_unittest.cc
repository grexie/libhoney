// Copyright (c) 2016 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_callback.h"
#include "include/views/honey_button.h"
#include "include/views/honey_button_delegate.h"
#include "include/views/honey_label_button.h"
#include "include/views/honey_menu_button.h"
#include "include/views/honey_menu_button_delegate.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeytests/image_util.h"
#include "tests/honeytests/test_handler.h"
#include "tests/honeytests/thread_helper.h"
#include "tests/honeytests/views/test_window_delegate.h"
#include "tests/gtest/include/gtest/gtest.h"

#define BUTTON_TEST(name) UI_THREAD_TEST(ViewsButtonTest, name)
#define BUTTON_TEST_ASYNC(name) UI_THREAD_TEST_ASYNC(ViewsButtonTest, name)

namespace {

HoneycombRefPtr<HoneycombImage> CreateIconImage() {
  HoneycombRefPtr<HoneycombImage> image = HoneycombImage::CreateImage();
  image_util::LoadIconImage(image, 1.0);
  image_util::LoadIconImage(image, 2.0);
  return image;
}

const char kButtonText[] = "My Button";

void VerifyButtonStyle(HoneycombRefPtr<HoneycombButton> button) {
  // Test state.
  EXPECT_EQ(HONEYCOMB_BUTTON_STATE_NORMAL, button->GetState());
  button->SetState(HONEYCOMB_BUTTON_STATE_HOVERED);
  EXPECT_EQ(HONEYCOMB_BUTTON_STATE_HOVERED, button->GetState());
  button->SetState(HONEYCOMB_BUTTON_STATE_PRESSED);
  EXPECT_EQ(HONEYCOMB_BUTTON_STATE_PRESSED, button->GetState());
  button->SetState(HONEYCOMB_BUTTON_STATE_DISABLED);
  EXPECT_EQ(HONEYCOMB_BUTTON_STATE_DISABLED, button->GetState());
  button->SetState(HONEYCOMB_BUTTON_STATE_NORMAL);

  button->SetTooltipText("Some tooltip text");
  button->SetAccessibleName("MyButton");
}

void VerifyLabelButtonImage(HoneycombRefPtr<HoneycombLabelButton> button,
                            honey_button_state_t state,
                            HoneycombRefPtr<HoneycombImage> image) {
  EXPECT_FALSE(button->GetImage(state).get()) << "state = " << state;
  button->SetImage(state, image);
  EXPECT_TRUE(image->IsSame(button->GetImage(state))) << "state = " << state;
  button->SetImage(state, nullptr);
  EXPECT_FALSE(button->GetImage(state).get()) << "state = " << state;
}

void VerifyLabelButtonStyle(HoneycombRefPtr<HoneycombLabelButton> button) {
  VerifyButtonStyle(button);

  // Test set/get text.
  EXPECT_STREQ(kButtonText, button->GetText().ToString().c_str());
  const char kText[] = "My text";
  button->SetText(kText);
  EXPECT_STREQ(kText, button->GetText().ToString().c_str());

  // Test images.
  HoneycombRefPtr<HoneycombImage> image = CreateIconImage();
  VerifyLabelButtonImage(button, HONEYCOMB_BUTTON_STATE_NORMAL, image);
  VerifyLabelButtonImage(button, HONEYCOMB_BUTTON_STATE_HOVERED, image);
  VerifyLabelButtonImage(button, HONEYCOMB_BUTTON_STATE_PRESSED, image);
  VerifyLabelButtonImage(button, HONEYCOMB_BUTTON_STATE_DISABLED, image);

  // Test colors.
  const honey_color_t color = HoneycombColorSetARGB(255, 255, 0, 255);
  button->SetTextColor(HONEYCOMB_BUTTON_STATE_NORMAL, color);
  button->SetTextColor(HONEYCOMB_BUTTON_STATE_HOVERED, color);
  button->SetTextColor(HONEYCOMB_BUTTON_STATE_PRESSED, color);
  button->SetTextColor(HONEYCOMB_BUTTON_STATE_DISABLED, color);
  button->SetEnabledTextColors(color);

  // Test alignment.
  button->SetHorizontalAlignment(HONEYCOMB_HORIZONTAL_ALIGNMENT_LEFT);
  button->SetHorizontalAlignment(HONEYCOMB_HORIZONTAL_ALIGNMENT_CENTER);
  button->SetHorizontalAlignment(HONEYCOMB_HORIZONTAL_ALIGNMENT_RIGHT);

  // Test fonts.
  button->SetFontList("Arial, 14px");

  // Test sizes.
  button->SetMinimumSize(HoneycombSize(100, 100));
  button->SetMaximumSize(HoneycombSize(100, 100));
}

void VerifyMenuButtonStyle(HoneycombRefPtr<HoneycombMenuButton> button) {
  VerifyLabelButtonStyle(button);
}

class EmptyMenuButtonDelegate : public HoneycombMenuButtonDelegate {
 public:
  EmptyMenuButtonDelegate() {}

  void OnMenuButtonPressed(
      HoneycombRefPtr<HoneycombMenuButton> menu_button,
      const HoneycombPoint& screen_point,
      HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) override {
    EXPECT_TRUE(false);  // Not reached.
  }

  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override {
    EXPECT_TRUE(false);  // Not reached.
  }

 private:
  IMPLEMENT_REFCOUNTING(EmptyMenuButtonDelegate);
  DISALLOW_COPY_AND_ASSIGN(EmptyMenuButtonDelegate);
};

void RunLabelButtonStyle(HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<HoneycombLabelButton> button = HoneycombLabelButton::CreateLabelButton(
      new EmptyMenuButtonDelegate(), kButtonText);

  // Must be added to a parent window before retrieving the style to avoid
  // a CHECK() in View::GetNativeTheme(). See https://crbug.com/1056756.
  window->AddChildView(button);
  window->Layout();

  VerifyLabelButtonStyle(button);
}

void LabelButtonStyleImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created = base::BindOnce(RunLabelButtonStyle);
  TestWindowDelegate::RunTest(event, std::move(config));
}

void RunMenuButtonStyle(HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<HoneycombMenuButton> button = HoneycombMenuButton::CreateMenuButton(
      new EmptyMenuButtonDelegate(), kButtonText);

  // Must be added to a parent window before retrieving the style to avoid
  // a CHECK() in View::GetNativeTheme(). See https://crbug.com/1056756.
  window->AddChildView(button);
  window->Layout();

  VerifyMenuButtonStyle(button);
}

void MenuButtonStyleImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created = base::BindOnce(RunMenuButtonStyle);
  TestWindowDelegate::RunTest(event, std::move(config));
}

}  // namespace

// Test Button getters/setters.
BUTTON_TEST_ASYNC(LabelButtonStyle)
BUTTON_TEST_ASYNC(MenuButtonStyle)

namespace {

// Mouse click delay in MS.
const int kClickDelayMS = 100;

const int kButtonID = 1;

class TestButtonDelegate : public HoneycombButtonDelegate {
 public:
  TestButtonDelegate() {}

  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override {
    EXPECT_TRUE(button.get());
    EXPECT_EQ(button->GetID(), kButtonID);

    // Complete the test by closing the window.
    button->GetWindow()->Close();
  }

 private:
  IMPLEMENT_REFCOUNTING(TestButtonDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestButtonDelegate);
};

void ClickButton(HoneycombRefPtr<HoneycombWindow> window, int button_id) {
  HoneycombRefPtr<HoneycombView> button = window->GetViewForID(button_id);
  EXPECT_TRUE(button->AsButton());

  // Determine the middle of the button in screen coordinates.
  const HoneycombRect& bounds = button->GetBoundsInScreen();
  const HoneycombPoint& click_point =
      HoneycombPoint(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2);

  // Click the button.
  window->SendMouseMove(click_point.x, click_point.y);
  window->SendMouseEvents(MBT_LEFT, true, true);
}

void AddImage(HoneycombRefPtr<HoneycombLabelButton> button) {
  HoneycombRefPtr<HoneycombImage> image = CreateIconImage();
  button->SetImage(HONEYCOMB_BUTTON_STATE_NORMAL, image);
}

void RunLabelButtonClick(bool with_text,
                         bool with_image,
                         HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<HoneycombLabelButton> button = HoneycombLabelButton::CreateLabelButton(
      new TestButtonDelegate(), with_text ? kButtonText : "");
  button->SetID(kButtonID);

  EXPECT_TRUE(button->AsButton());
  EXPECT_TRUE(button->AsButton()->AsLabelButton());
  EXPECT_EQ(kButtonID, button->GetID());
  EXPECT_TRUE(button->IsVisible());
  EXPECT_FALSE(button->IsDrawn());

  if (with_text) {
    EXPECT_STREQ(kButtonText, button->GetText().ToString().c_str());
  } else {
    EXPECT_TRUE(button->GetText().empty());
  }

  if (with_image) {
    AddImage(button);
  }

  window->AddChildView(button);
  window->Layout();

  EXPECT_TRUE(window->IsSame(button->GetWindow()));
  EXPECT_TRUE(window->IsSame(button->GetParentView()));
  EXPECT_TRUE(button->IsSame(window->GetViewForID(kButtonID)));
  EXPECT_TRUE(button->IsVisible());
  EXPECT_TRUE(button->IsDrawn());

  window->Show();

  // Wait a bit before trying to click the button.
  HoneycombPostDelayedTask(TID_UI, base::BindOnce(ClickButton, window, kButtonID),
                     kClickDelayMS);
}

void LabelButtonClick(HoneycombRefPtr<HoneycombWaitableEvent> event,
                      bool with_button_frame,
                      bool with_button_text,
                      bool with_button_image) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created =
      base::BindOnce(RunLabelButtonClick, with_button_text, with_button_image);
  config->frameless = false;
  config->close_window = false;
  TestWindowDelegate::RunTest(event, std::move(config));
}

void LabelButtonClickFramedWithTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, true, true, true);
}

void LabelButtonClickFramedWithTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, true, true, false);
}

void LabelButtonClickFramedNoTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, true, false, true);
}

void LabelButtonClickFramedNoTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, true, false, false);
}

void LabelButtonClickFramelessWithTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, false, true, true);
}

void LabelButtonClickFramelessWithTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, false, true, false);
}

void LabelButtonClickFramelessNoTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, false, false, true);
}

void LabelButtonClickFramelessNoTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  LabelButtonClick(event, false, false, false);
}

}  // namespace

// Test LabelButton functionality. This is primarily to exercise exposed Honeycomb
// APIs and is not intended to comprehensively test button-related behavior
// (which we presume that Chromium is testing).
BUTTON_TEST_ASYNC(LabelButtonClickFramedWithTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramedWithTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramedNoTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramedNoTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramelessWithTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramelessWithTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramelessNoTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(LabelButtonClickFramelessNoTextNoImageFramelessWindow)

namespace {

const int kMenuItemID = 2;
const char kMenuItemLabel[] = "My Menu Item";

void ClickMenuItem(HoneycombRefPtr<HoneycombMenuButton> menu_button) {
  // Determine the lower-right corner of the menu button, then offset a bit to
  // hit the first menu item.
  const HoneycombRect& bounds = menu_button->GetBoundsInScreen();
  const HoneycombPoint& click_point =
      HoneycombPoint(bounds.x + bounds.width + 10, bounds.y + bounds.height + 10);

  // Click the menu item.
  HoneycombRefPtr<HoneycombWindow> window = menu_button->GetWindow();
  window->SendMouseMove(click_point.x, click_point.y);
  window->SendMouseEvents(MBT_LEFT, true, true);
}

class TestMenuButtonDelegate : public HoneycombMenuButtonDelegate,
                               public HoneycombMenuModelDelegate {
 public:
  TestMenuButtonDelegate() {}

  void OnMenuButtonPressed(
      HoneycombRefPtr<HoneycombMenuButton> menu_button,
      const HoneycombPoint& screen_point,
      HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) override {
    window_ = menu_button->GetWindow();

    HoneycombRefPtr<HoneycombMenuModel> model = HoneycombMenuModel::CreateMenuModel(this);
    model->AddItem(kMenuItemID, kMenuItemLabel);

    // Verify color accessors.
    for (int i = 0; i < HONEYCOMB_MENU_COLOR_COUNT; ++i) {
      honey_menu_color_type_t color_type = static_cast<honey_menu_color_type_t>(i);
      honey_color_t color_out;
      honey_color_t color = HoneycombColorSetARGB(255, 255, 255, i);

      // No color set yet.
      color_out = 1;
      EXPECT_TRUE(model->GetColor(kMenuItemID, color_type, color_out));
      EXPECT_EQ(0U, color_out);
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(0, color_type, color_out));
      EXPECT_EQ(0U, color_out);
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(-1, color_type, color_out));
      EXPECT_EQ(0U, color_out);

      // Set the default color.
      EXPECT_TRUE(model->SetColorAt(-1, color_type, color));
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(-1, color_type, color_out));
      EXPECT_EQ(color, color_out);

      // Clear the default color.
      EXPECT_TRUE(model->SetColorAt(-1, color_type, 0));
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(-1, color_type, color_out));
      EXPECT_EQ(0U, color_out);

      // Set the index color.
      EXPECT_TRUE(model->SetColorAt(0, color_type, color));
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(0, color_type, color_out));
      EXPECT_EQ(color, color_out);

      // Clear the index color.
      EXPECT_TRUE(model->SetColorAt(0, color_type, 0));
      color_out = 1;
      EXPECT_TRUE(model->GetColorAt(0, color_type, color_out));
      EXPECT_EQ(0U, color_out);

      // Set the ID color.
      EXPECT_TRUE(model->SetColor(kMenuItemID, color_type, color));
      color_out = 1;
      EXPECT_TRUE(model->GetColor(kMenuItemID, color_type, color_out));
      EXPECT_EQ(color, color_out);

      // Clear the ID color.
      EXPECT_TRUE(model->SetColor(kMenuItemID, color_type, 0));
      color_out = 1;
      EXPECT_TRUE(model->GetColor(kMenuItemID, color_type, color_out));
      EXPECT_EQ(0U, color_out);

      // Index/ID doesn't exist.
      EXPECT_FALSE(model->SetColorAt(4, color_type, color));
      EXPECT_FALSE(model->SetColor(4, color_type, color));
      color_out = 1;
      EXPECT_FALSE(model->GetColorAt(4, color_type, color_out));
      EXPECT_FALSE(model->GetColor(4, color_type, color_out));
      EXPECT_EQ(1U, color_out);
    }

    // Verify font accessors.
    const std::string& font = "Tahoma, 12px";
    EXPECT_TRUE(model->SetFontListAt(0, font));
    EXPECT_TRUE(model->SetFontListAt(0, HoneycombString()));
    EXPECT_TRUE(model->SetFontList(kMenuItemID, font));
    EXPECT_TRUE(model->SetFontList(kMenuItemID, HoneycombString()));

    // Index/ID doesn't exist.
    EXPECT_FALSE(model->SetFontListAt(4, font));
    EXPECT_FALSE(model->SetFontList(4, font));

    // Wait a bit before trying to click the menu item.
    HoneycombPostDelayedTask(TID_UI, base::BindOnce(ClickMenuItem, menu_button),
                       kClickDelayMS);

    menu_button->ShowMenu(model, screen_point, HONEYCOMB_MENU_ANCHOR_TOPLEFT);
  }

  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override {}

  void ExecuteCommand(HoneycombRefPtr<HoneycombMenuModel> menu_model,
                      int command_id,
                      honey_event_flags_t event_flags) override {
    EXPECT_TRUE(menu_model.get());
    EXPECT_EQ(command_id, kMenuItemID);

    // Complete the test by closing the window.
    window_->GetWindow()->Close();
    window_ = nullptr;
  }

 private:
  HoneycombRefPtr<HoneycombWindow> window_;

  IMPLEMENT_REFCOUNTING(TestMenuButtonDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestMenuButtonDelegate);
};

void RunMenuButtonClick(bool with_text,
                        bool with_image,
                        HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<HoneycombMenuButton> button = HoneycombMenuButton::CreateMenuButton(
      new TestMenuButtonDelegate(), with_text ? kButtonText : "");
  button->SetID(kButtonID);

  EXPECT_TRUE(button->AsButton());
  EXPECT_TRUE(button->AsButton()->AsLabelButton());
  EXPECT_TRUE(button->AsButton()->AsLabelButton()->AsMenuButton());
  EXPECT_EQ(kButtonID, button->GetID());
  EXPECT_TRUE(button->IsVisible());
  EXPECT_FALSE(button->IsDrawn());

  if (with_text) {
    EXPECT_STREQ(kButtonText, button->GetText().ToString().c_str());
  } else {
    EXPECT_TRUE(button->GetText().empty());
  }

  if (with_image) {
    AddImage(button);
  }

  window->AddChildView(button);
  window->Layout();

  EXPECT_TRUE(window->IsSame(button->GetWindow()));
  EXPECT_TRUE(window->IsSame(button->GetParentView()));
  EXPECT_TRUE(button->IsSame(window->GetViewForID(kButtonID)));
  EXPECT_TRUE(button->IsVisible());
  EXPECT_TRUE(button->IsDrawn());

  window->Show();

  // Wait a bit before trying to click the button.
  HoneycombPostDelayedTask(TID_UI, base::BindOnce(ClickButton, window, kButtonID),
                     kClickDelayMS);
}

void MenuButtonClick(HoneycombRefPtr<HoneycombWaitableEvent> event,
                     bool with_button_frame,
                     bool with_button_text,
                     bool with_button_image) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created =
      base::BindOnce(RunMenuButtonClick, with_button_text, with_button_image);
  config->frameless = false;
  config->close_window = false;
  TestWindowDelegate::RunTest(event, std::move(config));
}

void MenuButtonClickFramedWithTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, true, true, true);
}

void MenuButtonClickFramedWithTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, true, true, false);
}

void MenuButtonClickFramedNoTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, true, false, true);
}

void MenuButtonClickFramedNoTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, true, false, false);
}

void MenuButtonClickFramelessWithTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, false, true, true);
}

void MenuButtonClickFramelessWithTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, false, true, false);
}

void MenuButtonClickFramelessNoTextWithImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, false, false, true);
}

void MenuButtonClickFramelessNoTextNoImageFramelessWindowImpl(
    HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonClick(event, false, false, false);
}

}  // namespace

// Test MenuButton functionality. This is primarily to exercise exposed Honeycomb
// APIs and is not intended to comprehensively test button-related behavior
// (which we presume that Chromium is testing).
BUTTON_TEST_ASYNC(MenuButtonClickFramedWithTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramedWithTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramedNoTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramedNoTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramelessWithTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramelessWithTextNoImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramelessNoTextWithImageFramelessWindow)
BUTTON_TEST_ASYNC(MenuButtonClickFramelessNoTextNoImageFramelessWindow)

namespace {

class TestMenuButtonCustomPopupDelegate : public HoneycombMenuButtonDelegate,
                                          public HoneycombWindowDelegate {
 public:
  explicit TestMenuButtonCustomPopupDelegate(bool can_activate)
      : can_activate_(can_activate) {}

  void OnMenuButtonPressed(
      HoneycombRefPtr<HoneycombMenuButton> menu_button,
      const HoneycombPoint& screen_point,
      HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock) override {
    parent_window_ = menu_button->GetWindow();
    button_pressed_lock_ = button_pressed_lock;

    popup_window_ = HoneycombWindow::CreateTopLevelWindow(this);
    popup_window_->SetBounds(HoneycombRect(screen_point.x, screen_point.y, 100, 100));

    HoneycombRefPtr<HoneycombLabelButton> button =
        HoneycombLabelButton::CreateLabelButton(this, "Button");
    button->SetFocusable(can_activate_);
    popup_window_->AddChildView(button);

    popup_window_->Show();

    // Wait a bit before trying to click the popup button.
    HoneycombPostDelayedTask(TID_UI, base::BindOnce(ClickMenuItem, menu_button),
                       kClickDelayMS);
  }

  void OnButtonPressed(HoneycombRefPtr<HoneycombButton> button) override {
    EXPECT_TRUE(button->GetWindow()->IsSame(popup_window_));
    popup_window_->Close();
    popup_window_ = nullptr;
    button_pressed_lock_ = nullptr;
  }

  HoneycombRefPtr<HoneycombWindow> GetParentWindow(HoneycombRefPtr<HoneycombWindow> window,
                                       bool* is_menu,
                                       bool* can_activate_menu) override {
    EXPECT_TRUE(parent_window_);
    *is_menu = true;
    *can_activate_menu = can_activate_;
    return parent_window_;
  }

  bool IsFrameless(HoneycombRefPtr<HoneycombWindow> window) override { return true; }

  void OnFocus(HoneycombRefPtr<HoneycombView> view) override {
    if (popup_window_ && view->GetWindow()->IsSame(popup_window_)) {
      EXPECT_TRUE(can_activate_);
      got_focus_.yes();
    }
  }

  void OnWindowDestroyed(HoneycombRefPtr<HoneycombWindow> window) override {
    if (can_activate_) {
      EXPECT_TRUE(got_focus_);
    } else {
      EXPECT_FALSE(got_focus_);
    }

    // Complete the test by closing the parent window.
    parent_window_->Close();
    parent_window_ = nullptr;
  }

 private:
  const bool can_activate_;

  HoneycombRefPtr<HoneycombWindow> parent_window_;
  HoneycombRefPtr<HoneycombWindow> popup_window_;
  HoneycombRefPtr<HoneycombMenuButtonPressedLock> button_pressed_lock_;

  TrackCallback got_focus_;

  IMPLEMENT_REFCOUNTING(TestMenuButtonCustomPopupDelegate);
  DISALLOW_COPY_AND_ASSIGN(TestMenuButtonCustomPopupDelegate);
};

void RunMenuButtonCustomPopupClick(bool can_activate,
                                   HoneycombRefPtr<HoneycombWindow> window) {
  HoneycombRefPtr<HoneycombMenuButton> button = HoneycombMenuButton::CreateMenuButton(
      new TestMenuButtonCustomPopupDelegate(can_activate), "Custom");
  button->SetID(kButtonID);

  window->AddChildView(button);
  window->Layout();

  window->Show();

  // Wait a bit before trying to click the button.
  HoneycombPostDelayedTask(TID_UI, base::BindOnce(ClickButton, window, kButtonID),
                     kClickDelayMS);
}

void MenuButtonCustomPopupClick(HoneycombRefPtr<HoneycombWaitableEvent> event,
                                bool can_activate) {
  auto config = std::make_unique<TestWindowDelegate::Config>();
  config->on_window_created =
      base::BindOnce(RunMenuButtonCustomPopupClick, can_activate);
  config->close_window = false;
  TestWindowDelegate::RunTest(event, std::move(config));
}

void MenuButtonCustomPopupActivateImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonCustomPopupClick(event, true);
}

void MenuButtonCustomPopupNoActivateImpl(HoneycombRefPtr<HoneycombWaitableEvent> event) {
  MenuButtonCustomPopupClick(event, false);
}

}  // namespace

BUTTON_TEST_ASYNC(MenuButtonCustomPopupActivate)
BUTTON_TEST_ASYNC(MenuButtonCustomPopupNoActivate)
