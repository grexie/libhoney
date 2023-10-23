// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_DOM_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_DOM_H_
#pragma once

#include <map>
#include "include/honey_base.h"

class HoneycombDOMDocument;
class HoneycombDOMNode;

///
/// Interface to implement for visiting the DOM. The methods of this class will
/// be called on the render process main thread.
///
/*--honey(source=client)--*/
class HoneycombDOMVisitor : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method executed for visiting the DOM. The document object passed to this
  /// method represents a snapshot of the DOM at the time this method is
  /// executed. DOM objects are only valid for the scope of this method. Do not
  /// keep references to or attempt to access any DOM objects outside the scope
  /// of this method.
  ///
  /*--honey()--*/
  virtual void Visit(HoneycombRefPtr<HoneycombDOMDocument> document) = 0;
};

///
/// Class used to represent a DOM document. The methods of this class should
/// only be called on the render process main thread thread.
///
/*--honey(source=library)--*/
class HoneycombDOMDocument : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_dom_document_type_t Type;

  ///
  /// Returns the document type.
  ///
  /*--honey(default_retval=DOM_DOCUMENT_TYPE_UNKNOWN)--*/
  virtual Type GetType() = 0;

  ///
  /// Returns the root document node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetDocument() = 0;

  ///
  /// Returns the BODY node of an HTML document.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetBody() = 0;

  ///
  /// Returns the HEAD node of an HTML document.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetHead() = 0;

  ///
  /// Returns the title of an HTML document.
  ///
  /*--honey()--*/
  virtual HoneycombString GetTitle() = 0;

  ///
  /// Returns the document element with the specified ID value.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetElementById(const HoneycombString& id) = 0;

  ///
  /// Returns the node that currently has keyboard focus.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetFocusedNode() = 0;

  ///
  /// Returns true if a portion of the document is selected.
  ///
  /*--honey()--*/
  virtual bool HasSelection() = 0;

  ///
  /// Returns the selection offset within the start node.
  ///
  /*--honey()--*/
  virtual int GetSelectionStartOffset() = 0;

  ///
  /// Returns the selection offset within the end node.
  ///
  /*--honey()--*/
  virtual int GetSelectionEndOffset() = 0;

  ///
  /// Returns the contents of this selection as markup.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSelectionAsMarkup() = 0;

  ///
  /// Returns the contents of this selection as text.
  ///
  /*--honey()--*/
  virtual HoneycombString GetSelectionAsText() = 0;

  ///
  /// Returns the base URL for the document.
  ///
  /*--honey()--*/
  virtual HoneycombString GetBaseURL() = 0;

  ///
  /// Returns a complete URL based on the document base URL and the specified
  /// partial URL.
  ///
  /*--honey()--*/
  virtual HoneycombString GetCompleteURL(const HoneycombString& partialURL) = 0;
};

///
/// Class used to represent a DOM node. The methods of this class should only be
/// called on the render process main thread.
///
/*--honey(source=library)--*/
class HoneycombDOMNode : public virtual HoneycombBaseRefCounted {
 public:
  typedef std::map<HoneycombString, HoneycombString> AttributeMap;
  typedef honey_dom_node_type_t Type;

  ///
  /// Returns the type for this node.
  ///
  /*--honey(default_retval=DOM_NODE_TYPE_UNSUPPORTED)--*/
  virtual Type GetType() = 0;

  ///
  /// Returns true if this is a text node.
  ///
  /*--honey()--*/
  virtual bool IsText() = 0;

  ///
  /// Returns true if this is an element node.
  ///
  /*--honey()--*/
  virtual bool IsElement() = 0;

  ///
  /// Returns true if this is an editable node.
  ///
  /*--honey()--*/
  virtual bool IsEditable() = 0;

  ///
  /// Returns true if this is a form control element node.
  ///
  /*--honey()--*/
  virtual bool IsFormControlElement() = 0;

  ///
  /// Returns the type of this form control element node.
  ///
  /*--honey()--*/
  virtual HoneycombString GetFormControlElementType() = 0;

  ///
  /// Returns true if this object is pointing to the same handle as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombDOMNode> that) = 0;

  ///
  /// Returns the name of this node.
  ///
  /*--honey()--*/
  virtual HoneycombString GetName() = 0;

  ///
  /// Returns the value of this node.
  ///
  /*--honey()--*/
  virtual HoneycombString GetValue() = 0;

  ///
  /// Set the value of this node. Returns true on success.
  ///
  /*--honey()--*/
  virtual bool SetValue(const HoneycombString& value) = 0;

  ///
  /// Returns the contents of this node as markup.
  ///
  /*--honey()--*/
  virtual HoneycombString GetAsMarkup() = 0;

  ///
  /// Returns the document associated with this node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMDocument> GetDocument() = 0;

  ///
  /// Returns the parent node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetParent() = 0;

  ///
  /// Returns the previous sibling node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetPreviousSibling() = 0;

  ///
  /// Returns the next sibling node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetNextSibling() = 0;

  ///
  /// Returns true if this node has child nodes.
  ///
  /*--honey()--*/
  virtual bool HasChildren() = 0;

  ///
  /// Return the first child node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetFirstChild() = 0;

  ///
  /// Returns the last child node.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombDOMNode> GetLastChild() = 0;

  // The following methods are valid only for element nodes.

  ///
  /// Returns the tag name of this element.
  ///
  /*--honey()--*/
  virtual HoneycombString GetElementTagName() = 0;

  ///
  /// Returns true if this element has attributes.
  ///
  /*--honey()--*/
  virtual bool HasElementAttributes() = 0;

  ///
  /// Returns true if this element has an attribute named |attrName|.
  ///
  /*--honey()--*/
  virtual bool HasElementAttribute(const HoneycombString& attrName) = 0;

  ///
  /// Returns the element attribute named |attrName|.
  ///
  /*--honey()--*/
  virtual HoneycombString GetElementAttribute(const HoneycombString& attrName) = 0;

  ///
  /// Returns a map of all element attributes.
  ///
  /*--honey()--*/
  virtual void GetElementAttributes(AttributeMap& attrMap) = 0;

  ///
  /// Set the value for the element attribute named |attrName|. Returns true on
  /// success.
  ///
  /*--honey()--*/
  virtual bool SetElementAttribute(const HoneycombString& attrName,
                                   const HoneycombString& value) = 0;

  ///
  /// Returns the inner text of the element.
  ///
  /*--honey()--*/
  virtual HoneycombString GetElementInnerText() = 0;

  ///
  /// Returns the bounds of the element in device pixels. Use
  /// "window.devicePixelRatio" to convert to/from CSS pixels.
  ///
  /*--honey()--*/
  virtual HoneycombRect GetElementBounds() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_DOM_H_
