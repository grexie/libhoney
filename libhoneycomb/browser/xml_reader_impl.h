// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_XML_READER_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_XML_READER_IMPL_H_
#pragma once

#include <libxml/xmlreader.h>
#include <sstream>

#include "base/threading/platform_thread.h"
#include "include/honey_xml_reader.h"

// Implementation of HoneycombXmlReader
class HoneycombXmlReaderImpl : public HoneycombXmlReader {
 public:
  HoneycombXmlReaderImpl();
  ~HoneycombXmlReaderImpl() override;

  // Initialize the reader context.
  bool Initialize(HoneycombRefPtr<HoneycombStreamReader> stream,
                  EncodingType encodingType,
                  const HoneycombString& URI);

  bool MoveToNextNode() override;
  bool Close() override;
  bool HasError() override;
  HoneycombString GetError() override;
  NodeType GetType() override;
  int GetDepth() override;
  HoneycombString GetLocalName() override;
  HoneycombString GetPrefix() override;
  HoneycombString GetQualifiedName() override;
  HoneycombString GetNamespaceURI() override;
  HoneycombString GetBaseURI() override;
  HoneycombString GetXmlLang() override;
  bool IsEmptyElement() override;
  bool HasValue() override;
  HoneycombString GetValue() override;
  bool HasAttributes() override;
  size_t GetAttributeCount() override;
  HoneycombString GetAttribute(int index) override;
  HoneycombString GetAttribute(const HoneycombString& qualifiedName) override;
  HoneycombString GetAttribute(const HoneycombString& localName,
                         const HoneycombString& namespaceURI) override;
  HoneycombString GetInnerXml() override;
  HoneycombString GetOuterXml() override;
  int GetLineNumber() override;
  bool MoveToAttribute(int index) override;
  bool MoveToAttribute(const HoneycombString& qualifiedName) override;
  bool MoveToAttribute(const HoneycombString& localName,
                       const HoneycombString& namespaceURI) override;
  bool MoveToFirstAttribute() override;
  bool MoveToNextAttribute() override;
  bool MoveToCarryingElement() override;

  // Add another line to the error string.
  void AppendError(const HoneycombString& error_str);

  // Verify that the reader exists and is being accessed from the correct
  // thread.
  bool VerifyContext();

 protected:
  base::PlatformThreadId supported_thread_id_;
  HoneycombRefPtr<HoneycombStreamReader> stream_;
  xmlTextReaderPtr reader_;
  std::stringstream error_buf_;

  IMPLEMENT_REFCOUNTING(HoneycombXmlReaderImpl);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_XML_READER_IMPL_H_
