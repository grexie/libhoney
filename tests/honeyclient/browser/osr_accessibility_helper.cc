// Copyright 2017 The Honeycomb Authors. Portions copyright
// 2013 The Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "tests/honeyclient/browser/osr_accessibility_helper.h"
#include "tests/honeyclient/browser/osr_accessibility_node.h"

namespace client {

OsrAXTree::OsrAXTree() : root_node_id_(-1) {}

OsrAXNode* OsrAXTree::GetNode(int nodeId) const {
  auto result = node_map_.find(nodeId);
  if (result != node_map_.end()) {
    return result->second;
  }
  return nullptr;
}

void OsrAXTree::EraseNode(int nodeId) {
  node_map_.erase(nodeId);
}

void OsrAXTree::AddNode(OsrAXNode* node) {
  node_map_[node->OsrAXNodeId()] = node;
}

void OsrAXTree::UpdateTreeData(HoneycombRefPtr<HoneycombDictionaryValue> value) {
  if (value->HasKey("parent_tree_id")) {
    parent_tree_id_ = value->GetString("parent_tree_id");
  } else {
    parent_tree_id_ = "";
  }

  // may also update following:
  // doctype, title, url, mimetype
}

OsrAccessibilityHelper::OsrAccessibilityHelper(HoneycombRefPtr<HoneycombValue> value,
                                               HoneycombRefPtr<HoneycombBrowser> browser)
    : focused_node_id_(-1), browser_(browser) {
  UpdateAccessibilityTree(value);
}

int OsrAccessibilityHelper::CastToInt(HoneycombRefPtr<HoneycombValue> value) {
  if (value->GetType() == VTYPE_STRING) {
    const std::string& str = value->GetString();
    return atoi(str.c_str());
  } else {
    return value->GetInt();
  }
}

void OsrAccessibilityHelper::UpdateAccessibilityLocation(
    HoneycombRefPtr<HoneycombValue> value) {
  if (!value || value->GetType() != VTYPE_LIST) {
    return;
  }

  HoneycombRefPtr<HoneycombListValue> locationChangeList = value->GetList();
  size_t locationChangeCount = locationChangeList->GetSize();
  if (locationChangeCount == 0) {
    return;
  }

  for (size_t i = 0; i < locationChangeCount; i++) {
    HoneycombRefPtr<HoneycombDictionaryValue> locationChangeDict =
        locationChangeList->GetDictionary(i);
    if (!locationChangeDict->HasKey("ax_tree_id") ||
        !locationChangeDict->HasKey("new_location") ||
        !locationChangeDict->HasKey("id")) {
      continue;
    }
    HoneycombString treeId = locationChangeDict->GetString("ax_tree_id");
    int nodeId = CastToInt(locationChangeDict->GetValue("id"));

    HoneycombRefPtr<HoneycombDictionaryValue> newLocationDict =
        locationChangeDict->GetDictionary("new_location");
    if (!newLocationDict) {
      continue;
    }

    OsrAXNode* node = GetNode(treeId, nodeId);
    if (!node) {
      continue;
    }
    node->UpdateLocation(newLocationDict);
  }
}

void OsrAccessibilityHelper::UpdateAccessibilityTree(
    HoneycombRefPtr<HoneycombValue> value) {
  if (!value || value->GetType() != VTYPE_DICTIONARY) {
    return;
  }

  HoneycombRefPtr<HoneycombDictionaryValue> mainDict = value->GetDictionary();
  if (!mainDict->HasKey("ax_tree_id") || !mainDict->HasKey("updates")) {
    return;
  }

  HoneycombString treeId = mainDict->GetString("ax_tree_id");
  HoneycombRefPtr<HoneycombListValue> updatesList = mainDict->GetList("updates");

  size_t updatesCount = updatesList->GetSize();
  if (updatesCount == 0) {
    return;
  }

  for (size_t i = 0; i < updatesCount; i++) {
    HoneycombRefPtr<HoneycombDictionaryValue> updateDict = updatesList->GetDictionary(i);
    UpdateLayout(treeId, updateDict);
  }
}

OsrAXNode* OsrAccessibilityHelper::GetRootNode() const {
  return GetTreeRootNode(root_tree_id_);
}

OsrAXNode* OsrAccessibilityHelper::GetFocusedNode() const {
  auto tree = accessibility_node_map_.find(focused_tree_id_);
  if (tree != accessibility_node_map_.end()) {
    return tree->second.GetNode(focused_node_id_);
  }

  return nullptr;
}

OsrAXNode* OsrAccessibilityHelper::GetTreeRootNode(
    const HoneycombString& treeId) const {
  auto tree = accessibility_node_map_.find(treeId);
  if (tree != accessibility_node_map_.end()) {
    return tree->second.GetNode(tree->second.GetRootNodeId());
  }

  return nullptr;
}

void OsrAccessibilityHelper::UpdateLayout(
    const HoneycombString& treeId,
    HoneycombRefPtr<HoneycombDictionaryValue> update) {
  if (!update) {
    return;
  }

  // If a node is to be cleared
  if (update->HasKey("node_id_to_clear")) {
    int nodeId = CastToInt(update->GetValue("node_id_to_clear"));

    // reset root node if that is to be cleared
    auto tree = accessibility_node_map_.find(treeId);
    if (tree != accessibility_node_map_.end()) {
      if (tree->second.GetRootNodeId() == nodeId) {
        root_tree_id_ = "";
        tree->second.SetRootNodeId(-1);
      }
    }
    if ((focused_tree_id_ == treeId) && (focused_node_id_ == nodeId)) {
      UpdateFocusedNode("", -1);
    }
    OsrAXNode* node = GetNode(treeId, nodeId);
    DestroyNode(node);
  }

  // get tree data
  if (update->HasKey("tree_data") && update->HasKey("has_tree_data") &&
      update->GetBool("has_tree_data")) {
    HoneycombRefPtr<HoneycombDictionaryValue> tree_data =
        update->GetDictionary("tree_data");
    auto& tree = accessibility_node_map_[treeId];
    tree.UpdateTreeData(tree_data);
    if (tree.GetParentTreeId().empty()) {
      root_tree_id_ = treeId;
    }
    if (tree_data->HasKey("focus_id") && tree_data->HasKey("focused_tree_id")) {
      UpdateFocusedNode(tree_data->GetString("focused_tree_id"),
                        CastToInt(tree_data->GetValue("focus_id")));
    }
  }

  // Now initialize/update the node data.
  if (update->HasKey("nodes")) {
    HoneycombRefPtr<HoneycombListValue> nodes = update->GetList("nodes");

    for (size_t index = 0; index < nodes->GetSize(); index++) {
      HoneycombRefPtr<HoneycombDictionaryValue> node = nodes->GetDictionary(index);
      if (node) {
        auto& tree = accessibility_node_map_[treeId];
        int nodeId = CastToInt(node->GetValue("id"));
        OsrAXNode* axNode = tree.GetNode(nodeId);
        // Create if it is a new one
        if (axNode) {
          axNode->UpdateValue(node);
        } else {
          axNode = OsrAXNode::CreateNode(treeId, nodeId, node, this);
          tree.AddNode(axNode);
        }
      }
    }
  }

  if (update->HasKey("root_id")) {
    int nodeId = CastToInt(update->GetValue("root_id"));
    OsrAXNode* node = GetNode(treeId, nodeId);
    if (node != nullptr) {
      auto& tree = accessibility_node_map_[treeId];
      tree.SetRootNodeId(nodeId);
    }
  }
}

void OsrAccessibilityHelper::UpdateFocusedNode(const HoneycombString& treeId,
                                               int nodeId) {
  if ((focused_tree_id_ == treeId) && (focused_node_id_ == nodeId)) {
    return;
  }
  focused_tree_id_ = treeId;
  focused_node_id_ = nodeId;

  // Now Notify Screen Reader
  OsrAXNode* axNode = GetFocusedNode();
  if (axNode) {
    axNode->NotifyAccessibilityEvent("focus");
  }
}

void OsrAccessibilityHelper::Reset() {
  accessibility_node_map_.clear();
  root_tree_id_ = "";
  focused_tree_id_ = "";
  focused_node_id_ = -1;
}

void OsrAccessibilityHelper::DestroyNode(OsrAXNode* node) {
  if (node) {
    HoneycombString treeId = node->OsrAXTreeId();
    int numChilds = node->GetChildCount();
    if (numChilds > 0) {
      for (int i = 0; i < numChilds; i++) {
        OsrAXNode* childNode = node->ChildAtIndex(i);
        if (!childNode) {
          continue;
        }
        childNode->SetParent(nullptr);
        if (childNode->OsrAXTreeId() == treeId) {
          DestroyNode(childNode);
        }
      }
    }
    auto tree = accessibility_node_map_.find(treeId);
    if (tree != accessibility_node_map_.end()) {
      tree->second.EraseNode(node->OsrAXNodeId());
    }

    node->Destroy();
  }
}

OsrAXNode* OsrAccessibilityHelper::GetNode(const HoneycombString& treeId,
                                           int nodeId) const {
  auto tree = accessibility_node_map_.find(treeId);
  if (tree != accessibility_node_map_.end()) {
    return tree->second.GetNode(nodeId);
  }

  return nullptr;
}

}  // namespace client
