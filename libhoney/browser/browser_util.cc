// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/browser_util.h"

#include "content/public/common/input/native_web_keyboard_event.h"

namespace browser_util {

bool GetHoneycombKeyEvent(const content::NativeWebKeyboardEvent& event,
                    HoneycombKeyEvent& honey_event) {
  switch (event.GetType()) {
    case blink::WebKeyboardEvent::Type::kRawKeyDown:
      honey_event.type = KEYEVENT_RAWKEYDOWN;
      break;
    case blink::WebKeyboardEvent::Type::kKeyDown:
      honey_event.type = KEYEVENT_KEYDOWN;
      break;
    case blink::WebKeyboardEvent::Type::kKeyUp:
      honey_event.type = KEYEVENT_KEYUP;
      break;
    case blink::WebKeyboardEvent::Type::kChar:
      honey_event.type = KEYEVENT_CHAR;
      break;
    default:
      return false;
  }

  honey_event.modifiers = 0;
  if (event.GetModifiers() & blink::WebKeyboardEvent::kShiftKey) {
    honey_event.modifiers |= EVENTFLAG_SHIFT_DOWN;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kControlKey) {
    honey_event.modifiers |= EVENTFLAG_CONTROL_DOWN;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kAltKey) {
    honey_event.modifiers |= EVENTFLAG_ALT_DOWN;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kMetaKey) {
    honey_event.modifiers |= EVENTFLAG_COMMAND_DOWN;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kIsKeyPad) {
    honey_event.modifiers |= EVENTFLAG_IS_KEY_PAD;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kIsLeft) {
    honey_event.modifiers |= EVENTFLAG_IS_LEFT;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kIsRight) {
    honey_event.modifiers |= EVENTFLAG_IS_RIGHT;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kAltGrKey) {
    honey_event.modifiers |= EVENTFLAG_ALTGR_DOWN;
  }
  if (event.GetModifiers() & blink::WebKeyboardEvent::kIsAutoRepeat) {
    honey_event.modifiers |= EVENTFLAG_IS_REPEAT;
  }

  honey_event.windows_key_code = event.windows_key_code;
  honey_event.native_key_code = event.native_key_code;
  honey_event.is_system_key = event.is_system_key;
  honey_event.character = event.text[0];
  honey_event.unmodified_character = event.unmodified_text[0];

  return true;
}

bool GetHoneycombKeyEvent(const ui::KeyEvent& event, HoneycombKeyEvent& honey_event) {
  content::NativeWebKeyboardEvent native_event(event);
  return GetHoneycombKeyEvent(native_event, honey_event);
}

}  // namespace browser_util
