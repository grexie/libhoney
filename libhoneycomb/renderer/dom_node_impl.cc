// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/renderer/dom_node_impl.h"

#include "libhoneycomb/common/tracker.h"
#include "libhoneycomb/renderer/blink_glue.h"
#include "libhoneycomb/renderer/browser_impl.h"
#include "libhoneycomb/renderer/dom_document_impl.h"
#include "libhoneycomb/renderer/thread_util.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_dom_event.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_form_control_element.h"
#include "third_party/blink/public/web/web_input_element.h"
#include "third_party/blink/public/web/web_node.h"
#include "third_party/blink/public/web/web_select_element.h"

using blink::WebDocument;
using blink::WebDOMEvent;
using blink::WebElement;
using blink::WebFormControlElement;
using blink::WebInputElement;
using blink::WebNode;
using blink::WebSelectElement;
using blink::WebString;

HoneycombDOMNodeImpl::HoneycombDOMNodeImpl(HoneycombRefPtr<HoneycombDOMDocumentImpl> document,
                               const blink::WebNode& node)
    : document_(document), node_(node) {}

HoneycombDOMNodeImpl::~HoneycombDOMNodeImpl() {
  HONEYCOMB_REQUIRE_RT();

  if (document_.get() && !node_.IsNull()) {
    // Remove the node from the document.
    document_->RemoveNode(node_);
  }
}

HoneycombDOMNodeImpl::Type HoneycombDOMNodeImpl::GetType() {
  if (!VerifyContext()) {
    return DOM_NODE_TYPE_UNSUPPORTED;
  }

  return blink_glue::GetNodeType(node_);
}

bool HoneycombDOMNodeImpl::IsText() {
  if (!VerifyContext()) {
    return false;
  }

  return node_.IsTextNode();
}

bool HoneycombDOMNodeImpl::IsElement() {
  if (!VerifyContext()) {
    return false;
  }

  return node_.IsElementNode();
}

// Logic copied from RenderViewImpl::IsEditableNode.
bool HoneycombDOMNodeImpl::IsEditable() {
  if (!VerifyContext()) {
    return false;
  }

  if (node_.IsContentEditable()) {
    return true;
  }

  if (node_.IsElementNode()) {
    const WebElement& element = node_.To<WebElement>();
    if (blink_glue::IsTextControlElement(element)) {
      return true;
    }

    // Also return true if it has an ARIA role of 'textbox'.
    for (unsigned i = 0; i < element.AttributeCount(); ++i) {
      if (base::EqualsCaseInsensitiveASCII(element.AttributeLocalName(i).Utf8(),
                                           "role")) {
        if (base::EqualsCaseInsensitiveASCII(element.AttributeValue(i).Utf8(),
                                             "textbox")) {
          return true;
        }
        break;
      }
    }
  }

  return false;
}

bool HoneycombDOMNodeImpl::IsFormControlElement() {
  if (!VerifyContext()) {
    return false;
  }

  if (node_.IsElementNode()) {
    const WebElement& element = node_.To<WebElement>();
    return element.IsFormControlElement();
  }

  return false;
}

HoneycombString HoneycombDOMNodeImpl::GetFormControlElementType() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (node_.IsElementNode()) {
    const WebElement& element = node_.To<WebElement>();
    if (element.IsFormControlElement()) {
      // Retrieve the type from the form control element.
      const WebFormControlElement& formElement =
          node_.To<WebFormControlElement>();

      const std::u16string& form_control_type =
          formElement.FormControlType().Utf16();
      str = form_control_type;
    }
  }

  return str;
}

bool HoneycombDOMNodeImpl::IsSame(HoneycombRefPtr<HoneycombDOMNode> that) {
  if (!VerifyContext()) {
    return false;
  }

  HoneycombDOMNodeImpl* impl = static_cast<HoneycombDOMNodeImpl*>(that.get());
  if (!impl || !impl->VerifyContext()) {
    return false;
  }

  return node_.Equals(impl->node_);
}

HoneycombString HoneycombDOMNodeImpl::GetName() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  const WebString& name = blink_glue::GetNodeName(node_);
  if (!name.IsNull()) {
    str = name.Utf16();
  }

  return str;
}

HoneycombString HoneycombDOMNodeImpl::GetValue() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (node_.IsElementNode()) {
    const WebElement& element = node_.To<WebElement>();
    if (element.IsFormControlElement()) {
      // Retrieve the value from the form control element.
      const WebFormControlElement& formElement =
          node_.To<WebFormControlElement>();

      std::u16string value;
      const std::u16string& form_control_type =
          formElement.FormControlType().Utf16();
      if (form_control_type == u"text") {
        const WebInputElement& input_element =
            formElement.To<WebInputElement>();
        value = input_element.Value().Utf16();
      } else if (form_control_type == u"select-one") {
        const WebSelectElement& select_element =
            formElement.To<WebSelectElement>();
        value = select_element.Value().Utf16();
      }

      base::TrimWhitespace(value, base::TRIM_LEADING, &value);
      str = value;
    }
  }

  if (str.empty()) {
    const WebString& value = node_.NodeValue();
    if (!value.IsNull()) {
      str = value.Utf16();
    }
  }

  return str;
}

bool HoneycombDOMNodeImpl::SetValue(const HoneycombString& value) {
  if (!VerifyContext()) {
    return false;
  }

  if (node_.IsElementNode()) {
    return false;
  }

  return blink_glue::SetNodeValue(node_,
                                  WebString::FromUTF16(value.ToString16()));
}

HoneycombString HoneycombDOMNodeImpl::GetAsMarkup() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  const WebString& markup = blink_glue::CreateNodeMarkup(node_);
  if (!markup.IsNull()) {
    str = markup.Utf16();
  }

  return str;
}

HoneycombRefPtr<HoneycombDOMDocument> HoneycombDOMNodeImpl::GetDocument() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_.get();
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMNodeImpl::GetParent() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_->GetOrCreateNode(node_.ParentNode());
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMNodeImpl::GetPreviousSibling() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_->GetOrCreateNode(node_.PreviousSibling());
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMNodeImpl::GetNextSibling() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_->GetOrCreateNode(node_.NextSibling());
}

bool HoneycombDOMNodeImpl::HasChildren() {
  if (!VerifyContext()) {
    return false;
  }

  return !node_.FirstChild().IsNull();
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMNodeImpl::GetFirstChild() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_->GetOrCreateNode(node_.FirstChild());
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMNodeImpl::GetLastChild() {
  if (!VerifyContext()) {
    return nullptr;
  }

  return document_->GetOrCreateNode(node_.LastChild());
}

HoneycombString HoneycombDOMNodeImpl::GetElementTagName() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return str;
  }

  const WebElement& element = node_.To<blink::WebElement>();
  const WebString& tagname = element.TagName();
  if (!tagname.IsNull()) {
    str = tagname.Utf16();
  }

  return str;
}

bool HoneycombDOMNodeImpl::HasElementAttributes() {
  if (!VerifyContext()) {
    return false;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return false;
  }

  const WebElement& element = node_.To<blink::WebElement>();
  return (element.AttributeCount() > 0);
}

bool HoneycombDOMNodeImpl::HasElementAttribute(const HoneycombString& attrName) {
  if (!VerifyContext()) {
    return false;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return false;
  }

  const WebElement& element = node_.To<blink::WebElement>();
  return element.HasAttribute(WebString::FromUTF16(attrName.ToString16()));
}

HoneycombString HoneycombDOMNodeImpl::GetElementAttribute(const HoneycombString& attrName) {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return str;
  }

  const WebElement& element = node_.To<blink::WebElement>();
  const WebString& attr =
      element.GetAttribute(WebString::FromUTF16(attrName.ToString16()));
  if (!attr.IsNull()) {
    str = attr.Utf16();
  }

  return str;
}

void HoneycombDOMNodeImpl::GetElementAttributes(AttributeMap& attrMap) {
  if (!VerifyContext()) {
    return;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return;
  }

  const WebElement& element = node_.To<blink::WebElement>();
  unsigned int len = element.AttributeCount();
  if (len == 0) {
    return;
  }

  for (unsigned int i = 0; i < len; ++i) {
    std::u16string name = element.AttributeLocalName(i).Utf16();
    std::u16string value = element.AttributeValue(i).Utf16();
    attrMap.insert(std::make_pair(name, value));
  }
}

bool HoneycombDOMNodeImpl::SetElementAttribute(const HoneycombString& attrName,
                                         const HoneycombString& value) {
  if (!VerifyContext()) {
    return false;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return false;
  }

  WebElement element = node_.To<blink::WebElement>();
  element.SetAttribute(WebString::FromUTF16(attrName.ToString16()),
                       WebString::FromUTF16(value.ToString16()));
  return true;
}

HoneycombString HoneycombDOMNodeImpl::GetElementInnerText() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return str;
  }

  WebElement element = node_.To<blink::WebElement>();
  const WebString& text = element.TextContent();
  if (!text.IsNull()) {
    str = text.Utf16();
  }

  return str;
}

HoneycombRect HoneycombDOMNodeImpl::GetElementBounds() {
  HoneycombRect rect;
  if (!VerifyContext()) {
    return rect;
  }

  if (!node_.IsElementNode()) {
    DCHECK(false);
    return rect;
  }

  WebElement element = node_.To<blink::WebElement>();
  const auto& rc = element.BoundsInWidget();
  rect.Set(rc.x(), rc.y(), rc.width(), rc.height());

  return rect;
}

void HoneycombDOMNodeImpl::Detach() {
  document_ = nullptr;
  node_.Assign(WebNode());
}

bool HoneycombDOMNodeImpl::VerifyContext() {
  if (!document_.get()) {
    DCHECK(false);
    return false;
  }
  if (!document_->VerifyContext()) {
    return false;
  }
  if (node_.IsNull()) {
    DCHECK(false);
    return false;
  }
  return true;
}
