// Copyright 2017 The Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_NODE_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_NODE_H_
#pragma once

#include <vector>

#include "include/honey_browser.h"

#if defined(OS_MAC)
typedef void HoneycombNativeAccessible;
#if __OBJC__
#if __has_feature(objc_arc)
#define CAST_HONEYCOMB_NATIVE_ACCESSIBLE_TO_NSOBJECT(accessible) \
  (__bridge NSObject*)accessible
#define CAST_NSOBJECT_TO_HONEYCOMB_NATIVE_ACCESSIBLE(object) \
  (__bridge HoneycombNativeAccessible*)object
#else  // __has_feature(objc_arc)
#define CAST_HONEYCOMB_NATIVE_ACCESSIBLE_TO_NSOBJECT(accessible) (NSObject*)accessible
#define CAST_NSOBJECT_TO_HONEYCOMB_NATIVE_ACCESSIBLE(object) \
  (__bridge HoneycombNativeAccessible*)object
#endif  // __has_feature(objc_arc)
#endif  // __OBJC__
#elif defined(OS_WIN)
struct IAccessible;
typedef IAccessible HoneycombNativeAccessible;
#else
#error "Unsupported platform"
#endif

namespace client {

class OsrAccessibilityHelper;

// OsrAXNode is the base class for implementation for the NSAccessibility
// protocol for interacting with VoiceOver and other accessibility clients.
class OsrAXNode {
 public:
  // Create and return the platform specific OsrAXNode Object.
  static OsrAXNode* CreateNode(const HoneycombString& treeId,
                               int nodeId,
                               HoneycombRefPtr<HoneycombDictionaryValue> value,
                               OsrAccessibilityHelper* helper);

  // Update Value.
  void UpdateValue(HoneycombRefPtr<HoneycombDictionaryValue> value);

  // UpdateLocation
  void UpdateLocation(HoneycombRefPtr<HoneycombDictionaryValue> value);

  // Fire a platform-specific notification that an event has occurred on
  // this object.
  void NotifyAccessibilityEvent(std::string event_type) const;

  // Call Destroy rather than deleting this, because the subclass may
  // use reference counting.
  void Destroy();

  // Return NSAccessibility Object for Mac/ IAccessible for Windows
  HoneycombNativeAccessible* GetNativeAccessibleObject(OsrAXNode* parent);

  HoneycombNativeAccessible* GetParentAccessibleObject() const {
    return parent_ ? parent_->platform_accessibility_ : nullptr;
  }

  OsrAccessibilityHelper* GetAccessibilityHelper() const {
    return accessibility_helper_;
  }

  int GetChildCount() const;

  // Return the Child at the specified index
  OsrAXNode* ChildAtIndex(int index) const;

  const HoneycombString& AxRole() const { return role_; }

  const HoneycombString& OsrAXTreeId() const { return tree_id_; }

  int OsrAXNodeId() const { return node_id_; }

  const HoneycombString& AxValue() const { return value_; }

  const HoneycombString& AxName() const { return name_; }

  const HoneycombString& AxDescription() const { return description_; }

  HoneycombRect AxLocation() const;

  HoneycombWindowHandle GetWindowHandle() const;

  HoneycombRefPtr<HoneycombBrowser> GetBrowser() const;

  void SetParent(OsrAXNode* parent);

 protected:
  OsrAXNode(const HoneycombString& treeId,
            int nodeId,
            HoneycombRefPtr<HoneycombDictionaryValue> value,
            OsrAccessibilityHelper* helper);

  HoneycombString tree_id_;
  int node_id_;
  HoneycombString child_tree_id_;
  HoneycombString role_;
  HoneycombString value_;
  HoneycombString name_;
  HoneycombString description_;
  HoneycombRect location_;
  HoneycombPoint scroll_;
  std::vector<int> child_ids_;
  HoneycombNativeAccessible* platform_accessibility_;
  OsrAXNode* parent_;
  int offset_container_id_;
  OsrAccessibilityHelper* accessibility_helper_;
  HoneycombRefPtr<HoneycombDictionaryValue> attributes_;
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_NODE_H_
