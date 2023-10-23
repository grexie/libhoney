// Copyright 2017 The Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

// Base class implementation for Honeycomb Acccessibility node. This is subclassed and
// used by both IAccessible/NSAccessibility protocol implementation.

#include "tests/honeyclient/browser/osr_accessibility_node.h"

#include "tests/honeyclient/browser/osr_accessibility_helper.h"

namespace client {

OsrAXNode::OsrAXNode(const HoneycombString& treeId,
                     int nodeId,
                     HoneycombRefPtr<HoneycombDictionaryValue> value,
                     OsrAccessibilityHelper* helper)
    : tree_id_(treeId),
      node_id_(nodeId),
      platform_accessibility_(nullptr),
      parent_(nullptr),
      offset_container_id_(-1),
      accessibility_helper_(helper) {
  UpdateValue(value);
}

void OsrAXNode::UpdateLocation(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  // Update Bounds
  if (value->HasKey("bounds")) {
    HoneycombRefPtr<HoneycombDictionaryValue> loc = value->GetDictionary("bounds");
    if (loc) {
      location_ = HoneycombRect(loc->GetDouble("x"), loc->GetDouble("y"),
                          loc->GetDouble("width"), loc->GetDouble("height"));
    }
  }
  // Update offsets
  if (value->HasKey("offset_container_id")) {
    offset_container_id_ = OsrAccessibilityHelper::CastToInt(
        value->GetValue("offset_container_id"));
  }
}

void OsrAXNode::UpdateValue(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  if (value->HasKey("role")) {
    role_ = value->GetString("role");
  }

  if (value->HasKey("child_ids")) {
    HoneycombRefPtr<HoneycombListValue> childs = value->GetList("child_ids");
    // Reset child Ids
    child_ids_.clear();
    for (size_t idx = 0; idx < childs->GetSize(); idx++) {
      child_ids_.push_back(
          OsrAccessibilityHelper::CastToInt(childs->GetValue(idx)));
    }
  }
  // Update Location
  if (value->HasKey("location")) {
    HoneycombRefPtr<HoneycombDictionaryValue> loc = value->GetDictionary("location");
    if (loc) {
      location_ = HoneycombRect(loc->GetDouble("x"), loc->GetDouble("y"),
                          loc->GetDouble("width"), loc->GetDouble("height"));
    }
  }
  // Update offsets
  if (value->HasKey("offset_container_id")) {
    offset_container_id_ = OsrAccessibilityHelper::CastToInt(
        value->GetValue("offset_container_id"));
  }
  // Update attributes
  if (value->HasKey("attributes")) {
    child_tree_id_ = "";

    attributes_ = value->GetDictionary("attributes");

    if (attributes_) {
      scroll_.x = attributes_->HasKey("scrollX")
                      ? OsrAccessibilityHelper::CastToInt(
                            attributes_->GetValue("scrollX"))
                      : 0;
      scroll_.y = attributes_->HasKey("scrollY")
                      ? OsrAccessibilityHelper::CastToInt(
                            attributes_->GetValue("scrollY"))
                      : 0;
    }

    if (attributes_ && attributes_->HasKey("childTreeId")) {
      child_tree_id_ = attributes_->GetString("childTreeId");
    }
    if (attributes_ && attributes_->HasKey("name")) {
      name_ = attributes_->GetString("name");
    }
    if (attributes_ && attributes_->HasKey("value")) {
      value_ = attributes_->GetString("value");
    }
    if (attributes_ && attributes_->HasKey("description")) {
      description_ = attributes_->GetString("description");
    }
  }
}

HoneycombWindowHandle OsrAXNode::GetWindowHandle() const {
  if (accessibility_helper_) {
    return accessibility_helper_->GetWindowHandle();
  }
  return nullptr;
}

HoneycombRefPtr<HoneycombBrowser> OsrAXNode::GetBrowser() const {
  if (accessibility_helper_) {
    return accessibility_helper_->GetBrowser();
  }
  return nullptr;
}

void OsrAXNode::SetParent(OsrAXNode* parent) {
  parent_ = parent;
}

HoneycombRect OsrAXNode::AxLocation() const {
  HoneycombRect loc = location_;
  loc.x -= scroll_.x;
  loc.y -= scroll_.y;
  OsrAXNode* offsetNode =
      accessibility_helper_->GetNode(OsrAXTreeId(), offset_container_id_);
  if (!offsetNode) {
    OsrAXNode* p = parent_;
    while (p) {
      if (p->OsrAXTreeId() != OsrAXTreeId()) {
        offsetNode = p;
        break;
      }
      p = p->parent_;
    }
  }
  // Add offset from parent Location
  if (offsetNode) {
    HoneycombRect offset = offsetNode->AxLocation();
    loc.x += offset.x;
    loc.y += offset.y;
  }
  return loc;
}

int OsrAXNode::GetChildCount() const {
  int count = static_cast<int>(child_ids_.size());
  if (!child_tree_id_.empty()) {
    OsrAXNode* childTreeRootNode =
        accessibility_helper_->GetTreeRootNode(child_tree_id_);
    if (childTreeRootNode) {
      count++;
    }
  }
  return count;
}

OsrAXNode* OsrAXNode::ChildAtIndex(int index) const {
  int count = static_cast<int>(child_ids_.size());
  if (index < count) {
    return accessibility_helper_->GetNode(OsrAXTreeId(), child_ids_[index]);
  }
  if ((index == count) && (!child_tree_id_.empty())) {
    OsrAXNode* childTreeRootNode =
        accessibility_helper_->GetTreeRootNode(child_tree_id_);
    if (childTreeRootNode) {
      return childTreeRootNode;
    }
  }

  return nullptr;
}

// Create and return the platform specific OsrAXNode Object
OsrAXNode* OsrAXNode::CreateNode(const HoneycombString& treeId,
                                 int nodeId,
                                 HoneycombRefPtr<HoneycombDictionaryValue> value,
                                 OsrAccessibilityHelper* helper) {
  return new OsrAXNode(treeId, nodeId, value, helper);
}

}  // namespace client
