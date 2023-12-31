// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=f53f587d16aeb5a66ed0cd8f5ef90a6b00833443$
//

#include "libhoney_dll/ctocpp/xml_reader_ctocpp.h"
#include "libhoney_dll/ctocpp/stream_reader_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"

// STATIC METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
HoneycombRefPtr<HoneycombXmlReader> HoneycombXmlReader::Create(HoneycombRefPtr<HoneycombStreamReader> stream,
                                             EncodingType encodingType,
                                             const HoneycombString& URI) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: stream; type: refptr_same
  DCHECK(stream.get());
  if (!stream.get()) {
    return nullptr;
  }
  // Verify param: URI; type: string_byref_const
  DCHECK(!URI.empty());
  if (URI.empty()) {
    return nullptr;
  }

  // Execute
  honey_xml_reader_t* _retval = honey_xml_reader_create(
      HoneycombStreamReaderCToCpp::Unwrap(stream), encodingType, URI.GetStruct());

  // Return type: refptr_same
  return HoneycombXmlReaderCToCpp::Wrap(_retval);
}

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::MoveToNextNode() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_next_node)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->move_to_next_node(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::Close() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, close)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->close(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::HasError() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, has_error)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->has_error(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetError() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_error)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_error(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombXmlReader::NodeType HoneycombXmlReaderCToCpp::GetType() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_type)) {
    return XML_NODE_UNSUPPORTED;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_xml_node_type_t _retval = _struct->get_type(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") int HoneycombXmlReaderCToCpp::GetDepth() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_depth)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_depth(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetLocalName() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_local_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_local_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetPrefix() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_prefix)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_prefix(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetQualifiedName() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_qualified_name)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_qualified_name(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetNamespaceURI() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_namespace_uri)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_namespace_uri(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetBaseURI() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_base_uri)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_base_uri(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetXmlLang() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_xml_lang)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_xml_lang(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::IsEmptyElement() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, is_empty_element)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_empty_element(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::HasValue() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, has_value)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->has_value(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetValue() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_value)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_value(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::HasAttributes() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, has_attributes)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->has_attributes(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") size_t HoneycombXmlReaderCToCpp::GetAttributeCount() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_attribute_count)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = _struct->get_attribute_count(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetAttribute(int index) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_attribute_byindex)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: index; type: simple_byval
  DCHECK_GE(index, 0);
  if (index < 0) {
    return HoneycombString();
  }

  // Execute
  honey_string_userfree_t _retval =
      _struct->get_attribute_byindex(_struct, index);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombXmlReaderCToCpp::GetAttribute(const HoneycombString& qualifiedName) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_attribute_byqname)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: qualifiedName; type: string_byref_const
  DCHECK(!qualifiedName.empty());
  if (qualifiedName.empty()) {
    return HoneycombString();
  }

  // Execute
  honey_string_userfree_t _retval =
      _struct->get_attribute_byqname(_struct, qualifiedName.GetStruct());

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall")
HoneycombString HoneycombXmlReaderCToCpp::GetAttribute(const HoneycombString& localName,
                                           const HoneycombString& namespaceURI) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_attribute_bylname)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: localName; type: string_byref_const
  DCHECK(!localName.empty());
  if (localName.empty()) {
    return HoneycombString();
  }
  // Verify param: namespaceURI; type: string_byref_const
  DCHECK(!namespaceURI.empty());
  if (namespaceURI.empty()) {
    return HoneycombString();
  }

  // Execute
  honey_string_userfree_t _retval = _struct->get_attribute_bylname(
      _struct, localName.GetStruct(), namespaceURI.GetStruct());

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetInnerXml() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_inner_xml)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_inner_xml(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") HoneycombString HoneycombXmlReaderCToCpp::GetOuterXml() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_outer_xml)) {
    return HoneycombString();
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  honey_string_userfree_t _retval = _struct->get_outer_xml(_struct);

  // Return type: string
  HoneycombString _retvalStr;
  _retvalStr.AttachToUserFree(_retval);
  return _retvalStr;
}

NO_SANITIZE("cfi-icall") int HoneycombXmlReaderCToCpp::GetLineNumber() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, get_line_number)) {
    return 0;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->get_line_number(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::MoveToAttribute(int index) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_attribute_byindex)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: index; type: simple_byval
  DCHECK_GE(index, 0);
  if (index < 0) {
    return false;
  }

  // Execute
  int _retval = _struct->move_to_attribute_byindex(_struct, index);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombXmlReaderCToCpp::MoveToAttribute(const HoneycombString& qualifiedName) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_attribute_byqname)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: qualifiedName; type: string_byref_const
  DCHECK(!qualifiedName.empty());
  if (qualifiedName.empty()) {
    return false;
  }

  // Execute
  int _retval =
      _struct->move_to_attribute_byqname(_struct, qualifiedName.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool HoneycombXmlReaderCToCpp::MoveToAttribute(const HoneycombString& localName,
                                         const HoneycombString& namespaceURI) {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_attribute_bylname)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: localName; type: string_byref_const
  DCHECK(!localName.empty());
  if (localName.empty()) {
    return false;
  }
  // Verify param: namespaceURI; type: string_byref_const
  DCHECK(!namespaceURI.empty());
  if (namespaceURI.empty()) {
    return false;
  }

  // Execute
  int _retval = _struct->move_to_attribute_bylname(
      _struct, localName.GetStruct(), namespaceURI.GetStruct());

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::MoveToFirstAttribute() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_first_attribute)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->move_to_first_attribute(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::MoveToNextAttribute() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_next_attribute)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->move_to_next_attribute(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool HoneycombXmlReaderCToCpp::MoveToCarryingElement() {
  shutdown_checker::AssertNotShutdown();

  honey_xml_reader_t* _struct = GetStruct();
  if (HONEYCOMB_MEMBER_MISSING(_struct, move_to_carrying_element)) {
    return false;
  }

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->move_to_carrying_element(_struct);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

HoneycombXmlReaderCToCpp::HoneycombXmlReaderCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

HoneycombXmlReaderCToCpp::~HoneycombXmlReaderCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
honey_xml_reader_t*
HoneycombCToCppRefCounted<HoneycombXmlReaderCToCpp, HoneycombXmlReader, honey_xml_reader_t>::
    UnwrapDerived(HoneycombWrapperType type, HoneycombXmlReader* c) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCToCppRefCounted<HoneycombXmlReaderCToCpp,
                                   HoneycombXmlReader,
                                   honey_xml_reader_t>::kWrapperType =
    WT_XML_READER;
