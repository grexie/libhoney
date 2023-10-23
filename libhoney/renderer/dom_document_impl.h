// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DOM_DOCUMENT_IMPL_H_
#define HONEYCOMB_LIBHONEY_DOM_DOCUMENT_IMPL_H_
#pragma once

#include <map>
#include "include/honey_dom.h"

namespace blink {
class WebLocalFrame;
class WebNode;
}  // namespace blink

class HoneycombBrowserImpl;

class HoneycombDOMDocumentImpl : public HoneycombDOMDocument {
 public:
  HoneycombDOMDocumentImpl(HoneycombBrowserImpl* browser, blink::WebLocalFrame* frame);
  ~HoneycombDOMDocumentImpl() override;

  // HoneycombDOMDocument methods.
  Type GetType() override;
  HoneycombRefPtr<HoneycombDOMNode> GetDocument() override;
  HoneycombRefPtr<HoneycombDOMNode> GetBody() override;
  HoneycombRefPtr<HoneycombDOMNode> GetHead() override;
  HoneycombString GetTitle() override;
  HoneycombRefPtr<HoneycombDOMNode> GetElementById(const HoneycombString& id) override;
  HoneycombRefPtr<HoneycombDOMNode> GetFocusedNode() override;
  bool HasSelection() override;
  int GetSelectionStartOffset() override;
  int GetSelectionEndOffset() override;
  HoneycombString GetSelectionAsMarkup() override;
  HoneycombString GetSelectionAsText() override;
  HoneycombString GetBaseURL() override;
  HoneycombString GetCompleteURL(const HoneycombString& partialURL) override;

  HoneycombBrowserImpl* GetBrowser() { return browser_; }
  blink::WebLocalFrame* GetFrame() { return frame_; }

  // The document maintains a map of all existing node objects.
  HoneycombRefPtr<HoneycombDOMNode> GetOrCreateNode(const blink::WebNode& node);
  void RemoveNode(const blink::WebNode& node);

  // Must be called before the object is destroyed.
  void Detach();

  // Verify that the object exists and is being accessed on the UI thread.
  bool VerifyContext();

 protected:
  HoneycombBrowserImpl* browser_;
  blink::WebLocalFrame* frame_;

  using NodeMap = std::map<blink::WebNode, HoneycombDOMNode*>;
  NodeMap node_map_;

  IMPLEMENT_REFCOUNTING(HoneycombDOMDocumentImpl);
};

#endif  // HONEYCOMB_LIBHONEY_DOM_DOCUMENT_IMPL_H_
