// Copyright 2017 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoneycomb/browser/osr/osr_accessibility_util.h"

#include <algorithm>
#include <string>
#include <utility>

#include "base/json/string_escape.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "content/public/browser/ax_event_notification_details.h"
#include "ui/accessibility/ax_enum_util.h"
#include "ui/accessibility/ax_enums.mojom.h"
#include "ui/accessibility/ax_text_utils.h"
#include "ui/accessibility/ax_tree_update.h"
#include "ui/gfx/geometry/transform.h"

namespace {
using ui::ToString;

template <typename T>
HoneycombRefPtr<HoneycombListValue> ToHoneycombValue(const std::vector<T>& vecData);

template <>
HoneycombRefPtr<HoneycombListValue> ToHoneycombValue<int>(const std::vector<int>& vecData) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();
  for (size_t i = 0; i < vecData.size(); i++) {
    value->SetInt(i, vecData[i]);
  }

  return value;
}

// Helper function for AXNodeData::ToHoneycombValue - Converts AXState attributes to
// HoneycombListValue.
HoneycombRefPtr<HoneycombListValue> ToHoneycombValue(uint32_t state) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();

  int index = 0;
  // Iterate and find which states are set.
  for (unsigned i = static_cast<unsigned>(ax::mojom::Role::kMinValue) + 1;
       i <= static_cast<unsigned>(ax::mojom::Role::kMaxValue); i++) {
    if (state & (1 << i)) {
      value->SetString(index++, ToString(static_cast<ax::mojom::State>(i)));
    }
  }
  return value;
}

// Helper function for AXNodeData::ToHoneycombValue - converts GfxRect to
// HoneycombDictionaryValue.
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const gfx::RectF& bounds) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();
  value->SetDouble("x", bounds.x());
  value->SetDouble("y", bounds.y());
  value->SetDouble("width", bounds.width());
  value->SetDouble("height", bounds.height());
  return value;
}

// Helper Functor for adding AxNodeData::attributes to AXNodeData::ToHoneycombValue.
struct PopulateAxNodeAttributes {
  HoneycombRefPtr<HoneycombDictionaryValue> attributes;

  explicit PopulateAxNodeAttributes(HoneycombRefPtr<HoneycombDictionaryValue> attrs)
      : attributes(attrs) {}

  // Int Attributes
  void operator()(const std::pair<ax::mojom::IntAttribute, int32_t> attr) {
    if (attr.first == ax::mojom::IntAttribute::kNone) {
      return;
    }

    switch (attr.first) {
      case ax::mojom::IntAttribute::kNone:
        break;
      case ax::mojom::IntAttribute::kScrollX:
      case ax::mojom::IntAttribute::kScrollXMin:
      case ax::mojom::IntAttribute::kScrollXMax:
      case ax::mojom::IntAttribute::kScrollY:
      case ax::mojom::IntAttribute::kScrollYMin:
      case ax::mojom::IntAttribute::kScrollYMax:
      case ax::mojom::IntAttribute::kHasPopup:
      case ax::mojom::IntAttribute::kIsPopup:
      case ax::mojom::IntAttribute::kHierarchicalLevel:
      case ax::mojom::IntAttribute::kTextSelStart:
      case ax::mojom::IntAttribute::kTextSelEnd:
      case ax::mojom::IntAttribute::kAriaColumnCount:
      case ax::mojom::IntAttribute::kAriaCellColumnIndex:
      case ax::mojom::IntAttribute::kAriaRowCount:
      case ax::mojom::IntAttribute::kAriaCellRowIndex:
      case ax::mojom::IntAttribute::kTableRowCount:
      case ax::mojom::IntAttribute::kTableColumnCount:
      case ax::mojom::IntAttribute::kTableCellColumnIndex:
      case ax::mojom::IntAttribute::kTableCellRowIndex:
      case ax::mojom::IntAttribute::kTableCellColumnSpan:
      case ax::mojom::IntAttribute::kTableCellRowSpan:
      case ax::mojom::IntAttribute::kTableColumnHeaderId:
      case ax::mojom::IntAttribute::kTableColumnIndex:
      case ax::mojom::IntAttribute::kTableHeaderId:
      case ax::mojom::IntAttribute::kTableRowHeaderId:
      case ax::mojom::IntAttribute::kTableRowIndex:
      case ax::mojom::IntAttribute::kActivedescendantId:
      case ax::mojom::IntAttribute::kInPageLinkTargetId:
      case ax::mojom::IntAttribute::kErrormessageIdDeprecated:
      case ax::mojom::IntAttribute::kDOMNodeId:
      case ax::mojom::IntAttribute::kDropeffect:
      case ax::mojom::IntAttribute::kMemberOfId:
      case ax::mojom::IntAttribute::kNextFocusId:
      case ax::mojom::IntAttribute::kNextWindowFocusId:
      case ax::mojom::IntAttribute::kNextOnLineId:
      case ax::mojom::IntAttribute::kPreviousFocusId:
      case ax::mojom::IntAttribute::kPreviousWindowFocusId:
      case ax::mojom::IntAttribute::kPreviousOnLineId:
      case ax::mojom::IntAttribute::kSetSize:
      case ax::mojom::IntAttribute::kPosInSet:
      case ax::mojom::IntAttribute::kPopupForId:
        attributes->SetInt(ToString(attr.first), attr.second);
        break;
      case ax::mojom::IntAttribute::kDefaultActionVerb:
        attributes->SetString(
            ToString(attr.first),
            ui::ToString(
                static_cast<ax::mojom::DefaultActionVerb>(attr.second)));
        break;
      case ax::mojom::IntAttribute::kInvalidState: {
        auto state = static_cast<ax::mojom::InvalidState>(attr.second);
        if (ax::mojom::InvalidState::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kCheckedState: {
        auto state = static_cast<ax::mojom::CheckedState>(attr.second);
        if (ax::mojom::CheckedState::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kRestriction:
        attributes->SetString(
            ToString(attr.first),
            ToString(static_cast<ax::mojom::Restriction>(attr.second)));
        break;
      case ax::mojom::IntAttribute::kListStyle: {
        auto state = static_cast<ax::mojom::ListStyle>(attr.second);
        if (ax::mojom::ListStyle::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kSortDirection: {
        auto state = static_cast<ax::mojom::SortDirection>(attr.second);
        if (ax::mojom::SortDirection::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kTextAlign: {
        auto state = static_cast<ax::mojom::TextAlign>(attr.second);
        if (ax::mojom::TextAlign::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kNameFrom:
        attributes->SetString(
            ToString(attr.first),
            ToString(static_cast<ax::mojom::NameFrom>(attr.second)));
        break;
      case ax::mojom::IntAttribute::kColorValue:
      case ax::mojom::IntAttribute::kBackgroundColor:
      case ax::mojom::IntAttribute::kColor:
        attributes->SetString(ToString(attr.first),
                              base::StringPrintf("0x%X", attr.second));
        break;
      case ax::mojom::IntAttribute::kDescriptionFrom:
        attributes->SetString(
            ToString(attr.first),
            ToString(static_cast<ax::mojom::DescriptionFrom>(attr.second)));
        break;
      case ax::mojom::IntAttribute::kAriaCurrentState: {
        auto state = static_cast<ax::mojom::AriaCurrentState>(attr.second);
        if (ax::mojom::AriaCurrentState::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kTextDirection: {
        auto state = static_cast<ax::mojom::WritingDirection>(attr.second);
        if (ax::mojom::WritingDirection::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kTextPosition: {
        auto state = static_cast<ax::mojom::TextPosition>(attr.second);
        if (ax::mojom::TextPosition::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kTextStyle: {
        static ax::mojom::TextStyle textStyleArr[] = {
            ax::mojom::TextStyle::kBold, ax::mojom::TextStyle::kItalic,
            ax::mojom::TextStyle::kUnderline,
            ax::mojom::TextStyle::kLineThrough,
            ax::mojom::TextStyle::kOverline};

        HoneycombRefPtr<HoneycombListValue> list = HoneycombListValue::Create();
        int index = 0;
        // Iterate and find which states are set.
        for (unsigned i = 0; i < std::size(textStyleArr); i++) {
          if (attr.second & static_cast<int>(textStyleArr[i])) {
            list->SetString(index++, ToString(textStyleArr[i]));
          }
        }
        attributes->SetList(ToString(attr.first), list);
      } break;
      case ax::mojom::IntAttribute::kTextOverlineStyle:
      case ax::mojom::IntAttribute::kTextStrikethroughStyle:
      case ax::mojom::IntAttribute::kTextUnderlineStyle: {
        auto state = static_cast<ax::mojom::TextDecorationStyle>(attr.second);
        if (ax::mojom::TextDecorationStyle::kNone != state) {
          attributes->SetString(ToString(attr.first), ToString(state));
        }
      } break;
      case ax::mojom::IntAttribute::kAriaCellColumnSpan:
      case ax::mojom::IntAttribute::kAriaCellRowSpan:
      case ax::mojom::IntAttribute::kImageAnnotationStatus: {
        // TODO(honey): Implement support for Image Annotation Status,
        // kAriaCellColumnSpan and kAriaCellRowSpan
      } break;
    }
  }

  // Set Bool Attributes.
  void operator()(const std::pair<ax::mojom::BoolAttribute, bool> attr) {
    if (attr.first != ax::mojom::BoolAttribute::kNone) {
      attributes->SetBool(ToString(attr.first), attr.second);
    }
  }
  // Set String Attributes.
  void operator()(
      const std::pair<ax::mojom::StringAttribute, std::string>& attr) {
    if (attr.first != ax::mojom::StringAttribute::kNone) {
      attributes->SetString(ToString(attr.first), attr.second);
    }
  }
  // Set Float attributes.
  void operator()(const std::pair<ax::mojom::FloatAttribute, float>& attr) {
    if (attr.first != ax::mojom::FloatAttribute::kNone) {
      attributes->SetDouble(ToString(attr.first), attr.second);
    }
  }

  // Set Int list attributes.
  void operator()(const std::pair<ax::mojom::IntListAttribute,
                                  std::vector<int32_t>>& attr) {
    if (attr.first != ax::mojom::IntListAttribute::kNone) {
      HoneycombRefPtr<HoneycombListValue> list;

      if (ax::mojom::IntListAttribute::kMarkerTypes == attr.first) {
        list = HoneycombListValue::Create();
        int index = 0;
        for (size_t i = 0; i < attr.second.size(); ++i) {
          auto type = static_cast<ax::mojom::MarkerType>(attr.second[i]);

          if (type == ax::mojom::MarkerType::kNone) {
            continue;
          }

          static ax::mojom::MarkerType marktypeArr[] = {
              ax::mojom::MarkerType::kSpelling, ax::mojom::MarkerType::kGrammar,
              ax::mojom::MarkerType::kTextMatch};

          // Iterate and find which markers are set.
          for (unsigned j = 0; j < std::size(marktypeArr); j++) {
            if (attr.second[i] & static_cast<int>(marktypeArr[j])) {
              list->SetString(index++, ToString(marktypeArr[j]));
            }
          }
        }
      } else {
        list = ToHoneycombValue(attr.second);
      }
      attributes->SetList(ToString(attr.first), list);
    }
  }
};

// Converts AXNodeData to HoneycombDictionaryValue(like AXNodeData::ToString).
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const ui::AXNodeData& node) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (node.id != -1) {
    value->SetInt("id", node.id);
  }

  value->SetString("role", ToString(node.role));
  value->SetList("state", ToHoneycombValue(node.state));

  if (node.relative_bounds.offset_container_id != -1) {
    value->SetInt("offset_container_id",
                  node.relative_bounds.offset_container_id);
  }

  value->SetDictionary("location", ToHoneycombValue(node.relative_bounds.bounds));

  // Transform matrix is private, so we set the string that Clients can parse
  // and use if needed.
  if (node.relative_bounds.transform &&
      !node.relative_bounds.transform->IsIdentity()) {
    value->SetString("transform", node.relative_bounds.transform->ToString());
  }

  if (!node.child_ids.empty()) {
    value->SetList("child_ids", ToHoneycombValue(node.child_ids));
  }

  HoneycombRefPtr<HoneycombListValue> actions_strings;
  size_t actions_idx = 0;
  for (int action_index = static_cast<int>(ax::mojom::Action::kMinValue) + 1;
       action_index <= static_cast<int>(ax::mojom::Action::kMaxValue);
       ++action_index) {
    auto action = static_cast<ax::mojom::Action>(action_index);
    if (node.HasAction(action)) {
      if (!actions_strings) {
        actions_strings = HoneycombListValue::Create();
      }
      actions_strings->SetString(actions_idx++, ToString(action));
    }
  }
  if (actions_strings) {
    value->SetList("actions", actions_strings);
  }

  HoneycombRefPtr<HoneycombDictionaryValue> attributes = HoneycombDictionaryValue::Create();
  PopulateAxNodeAttributes func(attributes);

  // Poupulate Int Attributes.
  std::for_each(node.int_attributes.begin(), node.int_attributes.end(), func);

  // Poupulate String Attributes.
  std::for_each(node.string_attributes.begin(), node.string_attributes.end(),
                func);

  // Poupulate Float Attributes.
  std::for_each(node.float_attributes.begin(), node.float_attributes.end(),
                func);

  // Poupulate Bool Attributes.
  std::for_each(node.bool_attributes.begin(), node.bool_attributes.end(), func);

  // Populate int list attributes.
  std::for_each(node.intlist_attributes.begin(), node.intlist_attributes.end(),
                func);

  value->SetDictionary("attributes", attributes);

  return value;
}

// Converts AXTreeData to HoneycombDictionaryValue(like AXTreeData::ToString).
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const ui::AXTreeData& treeData) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (!treeData.tree_id.ToString().empty()) {
    value->SetString("tree_id", treeData.tree_id.ToString());
  }

  if (!treeData.parent_tree_id.ToString().empty()) {
    value->SetString("parent_tree_id", treeData.parent_tree_id.ToString());
  }

  if (!treeData.focused_tree_id.ToString().empty()) {
    value->SetString("focused_tree_id", treeData.focused_tree_id.ToString());
  }

  if (!treeData.doctype.empty()) {
    value->SetString("doctype", treeData.doctype);
  }

  value->SetBool("loaded", treeData.loaded);

  if (treeData.loading_progress != 0.0) {
    value->SetDouble("loading_progress", treeData.loading_progress);
  }

  if (!treeData.mimetype.empty()) {
    value->SetString("mimetype", treeData.mimetype);
  }
  if (!treeData.url.empty()) {
    value->SetString("url", treeData.url);
  }
  if (!treeData.title.empty()) {
    value->SetString("title", treeData.title);
  }

  if (treeData.sel_anchor_object_id != -1) {
    value->SetInt("sel_anchor_object_id", treeData.sel_anchor_object_id);
    value->SetInt("sel_anchor_offset", treeData.sel_anchor_offset);
    value->SetString("sel_anchor_affinity",
                     ToString(treeData.sel_anchor_affinity));
  }
  if (treeData.sel_focus_object_id != -1) {
    value->SetInt("sel_focus_object_id", treeData.sel_anchor_object_id);
    value->SetInt("sel_focus_offset", treeData.sel_anchor_offset);
    value->SetString("sel_focus_affinity",
                     ToString(treeData.sel_anchor_affinity));
  }

  if (treeData.focus_id != -1) {
    value->SetInt("focus_id", treeData.focus_id);
  }

  return value;
}

// Converts AXTreeUpdate to HoneycombDictionaryValue(like AXTreeUpdate::ToString).
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const ui::AXTreeUpdate& update) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (update.has_tree_data) {
    value->SetBool("has_tree_data", true);
    value->SetDictionary("tree_data", ToHoneycombValue(update.tree_data));
  }

  if (update.node_id_to_clear != 0) {
    value->SetInt("node_id_to_clear", update.node_id_to_clear);
  }

  if (update.root_id != 0) {
    value->SetInt("root_id", update.root_id);
  }

  value->SetList("nodes", ToHoneycombValue(update.nodes));

  return value;
}

// Converts AXEvent to HoneycombDictionaryValue.
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const ui::AXEvent& event) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (event.event_type != ax::mojom::Event::kNone) {
    value->SetString("event_type", ToString(event.event_type));
  }

  if (event.id != -1) {
    value->SetInt("id", event.id);
  }

  if (event.event_from != ax::mojom::EventFrom::kNone) {
    value->SetString("event_from", ToString(event.event_from));
  }

  if (event.action_request_id != -1) {
    value->SetInt("action_request_id", event.action_request_id);
  }

  return value;
}

// Convert AXEventNotificationDetails to HoneycombDictionaryValue.
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(
    const content::AXEventNotificationDetails& eventData) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (!eventData.ax_tree_id.ToString().empty()) {
    value->SetString("ax_tree_id", eventData.ax_tree_id.ToString());
  }

  if (eventData.updates.size() > 0) {
    HoneycombRefPtr<HoneycombListValue> updates = HoneycombListValue::Create();
    updates->SetSize(eventData.updates.size());
    size_t i = 0;
    for (const auto& update : eventData.updates) {
      updates->SetDictionary(i++, ToHoneycombValue(update));
    }
    value->SetList("updates", updates);
  }

  if (eventData.events.size() > 0) {
    HoneycombRefPtr<HoneycombListValue> events = HoneycombListValue::Create();
    events->SetSize(eventData.events.size());
    size_t i = 0;
    for (const auto& event : eventData.events) {
      events->SetDictionary(i++, ToHoneycombValue(event));
    }
    value->SetList("events", events);
  }

  return value;
}

// Convert AXRelativeBounds to HoneycombDictionaryValue. Similar to
// AXRelativeBounds::ToString. See that for more details
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(const ui::AXRelativeBounds& location) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (location.offset_container_id != -1) {
    value->SetInt("offset_container_id", location.offset_container_id);
  }

  value->SetDictionary("bounds", ToHoneycombValue(location.bounds));

  // Transform matrix is private, so we set the string that Clients can parse
  // and use if needed.
  if (location.transform && !location.transform->IsIdentity()) {
    value->SetString("transform", location.transform->ToString());
  }

  return value;
}

// Convert AXLocationChangeNotificationDetails to HoneycombDictionaryValue.
HoneycombRefPtr<HoneycombDictionaryValue> ToHoneycombValue(
    const content::AXLocationChangeNotificationDetails& locData) {
  HoneycombRefPtr<HoneycombDictionaryValue> value = HoneycombDictionaryValue::Create();

  if (locData.id != -1) {
    value->SetInt("id", locData.id);
  }

  if (!locData.ax_tree_id.ToString().empty()) {
    value->SetString("ax_tree_id", locData.ax_tree_id.ToString());
  }

  value->SetDictionary("new_location", ToHoneycombValue(locData.new_location));

  return value;
}

template <typename T>
HoneycombRefPtr<HoneycombListValue> ToHoneycombValue(const std::vector<T>& vecData) {
  HoneycombRefPtr<HoneycombListValue> value = HoneycombListValue::Create();

  for (size_t i = 0; i < vecData.size(); i++) {
    value->SetDictionary(i, ToHoneycombValue(vecData[i]));
  }

  return value;
}

}  // namespace

namespace osr_accessibility_util {

HoneycombRefPtr<HoneycombValue> ParseAccessibilityEventData(
    const content::AXEventNotificationDetails& data) {
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  value->SetDictionary(ToHoneycombValue(data));
  return value;
}

HoneycombRefPtr<HoneycombValue> ParseAccessibilityLocationData(
    const std::vector<content::AXLocationChangeNotificationDetails>& data) {
  HoneycombRefPtr<HoneycombValue> value = HoneycombValue::Create();
  value->SetList(ToHoneycombValue(data));
  return value;
}

}  // namespace osr_accessibility_util
