// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/renderer/dom_document_impl.h"
#include "libhoney/renderer/dom_node_impl.h"
#include "libhoney/renderer/thread_util.h"

#include "base/logging.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_node.h"
#include "third_party/blink/public/web/web_range.h"

using blink::WebDocument;
using blink::WebElement;
using blink::WebLocalFrame;
using blink::WebNode;
using blink::WebRange;
using blink::WebString;
using blink::WebURL;

HoneycombDOMDocumentImpl::HoneycombDOMDocumentImpl(HoneycombBrowserImpl* browser,
                                       WebLocalFrame* frame)
    : browser_(browser), frame_(frame) {
  const WebDocument& document = frame_->GetDocument();
  DCHECK(!document.IsNull());
}

HoneycombDOMDocumentImpl::~HoneycombDOMDocumentImpl() {
  HONEYCOMB_REQUIRE_RT();

  // Verify that the Detach() method has been called.
  DCHECK(frame_ == nullptr);
}

HoneycombDOMDocumentImpl::Type HoneycombDOMDocumentImpl::GetType() {
  if (!VerifyContext()) {
    return DOM_DOCUMENT_TYPE_UNKNOWN;
  }

  const WebDocument& document = frame_->GetDocument();
  if (document.IsHTMLDocument()) {
    return DOM_DOCUMENT_TYPE_HTML;
  }
  if (document.IsXHTMLDocument()) {
    return DOM_DOCUMENT_TYPE_XHTML;
  }
  if (document.IsPluginDocument()) {
    return DOM_DOCUMENT_TYPE_PLUGIN;
  }
  return DOM_DOCUMENT_TYPE_UNKNOWN;
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetDocument() {
  const WebDocument& document = frame_->GetDocument();
  return GetOrCreateNode(document.GetDocument());
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetBody() {
  const WebDocument& document = frame_->GetDocument();
  return GetOrCreateNode(document.Body());
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetHead() {
  WebDocument document = frame_->GetDocument();
  return GetOrCreateNode(document.Head());
}

HoneycombString HoneycombDOMDocumentImpl::GetTitle() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  const WebDocument& document = frame_->GetDocument();
  const WebString& title = document.Title();
  if (!title.IsNull()) {
    str = title.Utf16();
  }

  return str;
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetElementById(const HoneycombString& id) {
  const WebDocument& document = frame_->GetDocument();
  return GetOrCreateNode(
      document.GetElementById(WebString::FromUTF16(id.ToString16())));
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetFocusedNode() {
  const WebDocument& document = frame_->GetDocument();
  return GetOrCreateNode(document.FocusedElement());
}

bool HoneycombDOMDocumentImpl::HasSelection() {
  if (!VerifyContext()) {
    return false;
  }

  return frame_->HasSelection();
}

int HoneycombDOMDocumentImpl::GetSelectionStartOffset() {
  if (!VerifyContext()) {
    return 0;
  }

  if (!frame_->HasSelection()) {
    return 0;
  }

  const WebRange& range = frame_->SelectionRange();
  if (range.IsNull()) {
    return 0;
  }

  return range.StartOffset();
}

int HoneycombDOMDocumentImpl::GetSelectionEndOffset() {
  if (!VerifyContext()) {
    return 0;
  }

  if (!frame_->HasSelection()) {
    return 0;
  }

  const WebRange& range = frame_->SelectionRange();
  if (range.IsNull()) {
    return 0;
  }

  return range.EndOffset();
}

HoneycombString HoneycombDOMDocumentImpl::GetSelectionAsMarkup() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (!frame_->HasSelection()) {
    return str;
  }

  const WebString& markup = frame_->SelectionAsMarkup();
  if (!markup.IsNull()) {
    str = markup.Utf16();
  }

  return str;
}

HoneycombString HoneycombDOMDocumentImpl::GetSelectionAsText() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  if (!frame_->HasSelection()) {
    return str;
  }

  const WebString& text = frame_->SelectionAsText();
  if (!text.IsNull()) {
    str = text.Utf16();
  }

  return str;
}

HoneycombString HoneycombDOMDocumentImpl::GetBaseURL() {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  const WebDocument& document = frame_->GetDocument();
  const WebURL& url = document.BaseURL();
  if (!url.IsNull()) {
    GURL gurl = url;
    str = gurl.spec();
  }

  return str;
}

HoneycombString HoneycombDOMDocumentImpl::GetCompleteURL(const HoneycombString& partialURL) {
  HoneycombString str;
  if (!VerifyContext()) {
    return str;
  }

  const WebDocument& document = frame_->GetDocument();
  const WebURL& url =
      document.CompleteURL(WebString::FromUTF16(partialURL.ToString16()));
  if (!url.IsNull()) {
    GURL gurl = url;
    str = gurl.spec();
  }

  return str;
}

HoneycombRefPtr<HoneycombDOMNode> HoneycombDOMDocumentImpl::GetOrCreateNode(
    const blink::WebNode& node) {
  if (!VerifyContext()) {
    return nullptr;
  }

  // Nodes may potentially be null.
  if (node.IsNull()) {
    return nullptr;
  }

  if (!node_map_.empty()) {
    // Locate the existing node, if any.
    NodeMap::const_iterator it = node_map_.find(node);
    if (it != node_map_.end()) {
      return it->second;
    }
  }

  // Create the new node object.
  HoneycombRefPtr<HoneycombDOMNode> nodeImpl(new HoneycombDOMNodeImpl(this, node));
  node_map_.insert(std::make_pair(node, nodeImpl.get()));
  return nodeImpl;
}

void HoneycombDOMDocumentImpl::RemoveNode(const blink::WebNode& node) {
  if (!VerifyContext()) {
    return;
  }

  if (!node_map_.empty()) {
    NodeMap::iterator it = node_map_.find(node);
    if (it != node_map_.end()) {
      node_map_.erase(it);
    }
  }
}

void HoneycombDOMDocumentImpl::Detach() {
  if (!VerifyContext()) {
    return;
  }

  // If you hit this assert it means that you are keeping references to node
  // objects beyond the valid scope.
  DCHECK(node_map_.empty());

  // If you hit this assert it means that you are keeping references to this
  // document object beyond the valid scope.
  DCHECK(HasOneRef());

  if (!node_map_.empty()) {
    NodeMap::const_iterator it = node_map_.begin();
    for (; it != node_map_.end(); ++it) {
      static_cast<HoneycombDOMNodeImpl*>(it->second)->Detach();
    }
    node_map_.clear();
  }

  frame_ = nullptr;
}

bool HoneycombDOMDocumentImpl::VerifyContext() {
  if (!HONEYCOMB_CURRENTLY_ON_RT() || frame_ == nullptr) {
    DCHECK(false);
    return false;
  }
  return true;
}
