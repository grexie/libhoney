// Copyright 2017 The Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_HELPER_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_HELPER_H_

#include <map>

#include "include/honey_browser.h"

namespace client {

class OsrAXNode;
class OsrAccessibilityHelper;

class OsrAXTree {
 public:
  OsrAXTree();
  OsrAXNode* GetNode(int nodeId) const;
  void EraseNode(int nodeId);
  void UpdateTreeData(HoneycombRefPtr<HoneycombDictionaryValue> value);
  void AddNode(OsrAXNode* node);
  const HoneycombString& GetParentTreeId() const { return parent_tree_id_; }
  int GetRootNodeId() const { return root_node_id_; }
  void SetRootNodeId(int nodeId) { root_node_id_ = nodeId; }

 private:
  HoneycombString parent_tree_id_;
  int root_node_id_;
  std::map<int, OsrAXNode*> node_map_;
};

// Helper class that abstracts Renderer Accessibility tree and provides a
// uniform interface to be consumed by IAccessible interface on Windows and
// NSAccessibility implementation on Mac in HoneycombClient.
class OsrAccessibilityHelper {
 public:
  OsrAccessibilityHelper(HoneycombRefPtr<HoneycombValue> value,
                         HoneycombRefPtr<HoneycombBrowser> browser);

  void UpdateAccessibilityTree(HoneycombRefPtr<HoneycombValue> value);

  void UpdateAccessibilityLocation(HoneycombRefPtr<HoneycombValue> value);

  OsrAXNode* GetRootNode() const;

  OsrAXNode* GetFocusedNode() const;

  HoneycombWindowHandle GetWindowHandle() const {
    return browser_->GetHost()->GetWindowHandle();
  }

  HoneycombRefPtr<HoneycombBrowser> GetBrowser() const { return browser_; }

  OsrAXNode* GetNode(const HoneycombString& treeId, int nodeId) const;

  OsrAXNode* GetTreeRootNode(const HoneycombString& treeId) const;

  static int CastToInt(HoneycombRefPtr<HoneycombValue> value);

 private:
  void Reset();

  void UpdateLayout(const HoneycombString& treeId,
                    HoneycombRefPtr<HoneycombDictionaryValue> update);

  void UpdateFocusedNode(const HoneycombString& treeId, int nodeId);

  // Destroy the node and remove from Map
  void DestroyNode(OsrAXNode* node);
  HoneycombString root_tree_id_;
  HoneycombString focused_tree_id_;
  int focused_node_id_;
  HoneycombRefPtr<HoneycombBrowser> browser_;
  std::map<HoneycombString, OsrAXTree> accessibility_node_map_;
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_OSR_ACCESSIBILITY_HELPER_H_
