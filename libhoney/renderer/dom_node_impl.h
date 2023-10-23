// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_DOM_NODE_IMPL_H_
#define HONEYCOMB_LIBHONEY_DOM_NODE_IMPL_H_
#pragma once

#include "include/honey_dom.h"
#include "third_party/blink/public/web/web_node.h"

class HoneycombDOMDocumentImpl;

class HoneycombDOMNodeImpl : public HoneycombDOMNode {
 public:
  HoneycombDOMNodeImpl(HoneycombRefPtr<HoneycombDOMDocumentImpl> document,
                 const blink::WebNode& node);
  ~HoneycombDOMNodeImpl() override;

  // HoneycombDOMNode methods.
  Type GetType() override;
  bool IsText() override;
  bool IsElement() override;
  bool IsEditable() override;
  bool IsFormControlElement() override;
  HoneycombString GetFormControlElementType() override;
  bool IsSame(HoneycombRefPtr<HoneycombDOMNode> that) override;
  HoneycombString GetName() override;
  HoneycombString GetValue() override;
  bool SetValue(const HoneycombString& value) override;
  HoneycombString GetAsMarkup() override;
  HoneycombRefPtr<HoneycombDOMDocument> GetDocument() override;
  HoneycombRefPtr<HoneycombDOMNode> GetParent() override;
  HoneycombRefPtr<HoneycombDOMNode> GetPreviousSibling() override;
  HoneycombRefPtr<HoneycombDOMNode> GetNextSibling() override;
  bool HasChildren() override;
  HoneycombRefPtr<HoneycombDOMNode> GetFirstChild() override;
  HoneycombRefPtr<HoneycombDOMNode> GetLastChild() override;
  HoneycombString GetElementTagName() override;
  bool HasElementAttributes() override;
  bool HasElementAttribute(const HoneycombString& attrName) override;
  HoneycombString GetElementAttribute(const HoneycombString& attrName) override;
  void GetElementAttributes(AttributeMap& attrMap) override;
  bool SetElementAttribute(const HoneycombString& attrName,
                           const HoneycombString& value) override;
  HoneycombString GetElementInnerText() override;
  HoneycombRect GetElementBounds() override;

  // Will be called from HoneycombDOMDocumentImpl::Detach().
  void Detach();

  // Verify that the object exists and is being accessed on the UI thread.
  bool VerifyContext();

 protected:
  HoneycombRefPtr<HoneycombDOMDocumentImpl> document_;
  blink::WebNode node_;

  IMPLEMENT_REFCOUNTING(HoneycombDOMNodeImpl);
};

#endif  // HONEYCOMB_LIBHONEY_DOM_NODE_IMPL_H_
