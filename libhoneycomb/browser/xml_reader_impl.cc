// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/browser/xml_reader_impl.h"

#include "include/honey_stream.h"

#include "base/logging.h"
#include "base/notreached.h"

// Static functions

// static
HoneycombRefPtr<HoneycombXmlReader> HoneycombXmlReader::Create(HoneycombRefPtr<HoneycombStreamReader> stream,
                                             EncodingType encodingType,
                                             const HoneycombString& URI) {
  HoneycombRefPtr<HoneycombXmlReaderImpl> impl(new HoneycombXmlReaderImpl());
  if (!impl->Initialize(stream, encodingType, URI)) {
    return nullptr;
  }
  return impl.get();
}

// HoneycombXmlReaderImpl

namespace {

/**
 * xmlInputReadCallback:
 * @context:  an Input context
 * @buffer:  the buffer to store data read
 * @len:  the length of the buffer in bytes
 *
 * Callback used in the I/O Input API to read the resource
 *
 * Returns the number of bytes read or -1 in case of error
 */
int XMLCALL xml_read_callback(void* context, char* buffer, int len) {
  HoneycombRefPtr<HoneycombStreamReader> reader(static_cast<HoneycombStreamReader*>(context));
  return reader->Read(buffer, 1, len);
}

/**
 * xmlTextReaderErrorFunc:
 * @arg: the user argument
 * @msg: the message
 * @severity: the severity of the error
 * @locator: a locator indicating where the error occured
 *
 * Signature of an error callback from a reader parser
 */
void XMLCALL xml_error_callback(void* arg,
                                const char* msg,
                                xmlParserSeverities severity,
                                xmlTextReaderLocatorPtr locator) {
  if (!msg) {
    return;
  }

  std::string error_str(msg);
  if (!error_str.empty() && error_str[error_str.length() - 1] == '\n') {
    error_str.resize(error_str.length() - 1);
  }

  std::stringstream ss;
  ss << error_str << ", line " << xmlTextReaderLocatorLineNumber(locator);

  LOG(INFO) << ss.str();

  HoneycombRefPtr<HoneycombXmlReaderImpl> impl(static_cast<HoneycombXmlReaderImpl*>(arg));
  impl->AppendError(ss.str());
}

/**
 * xmlStructuredErrorFunc:
 * @userData:  user provided data for the error callback
 * @error:  the error being raised.
 *
 * Signature of the function to use when there is an error and
 * the module handles the new error reporting mechanism.
 */
void XMLCALL xml_structured_error_callback(void* userData, xmlErrorPtr error) {
  if (!error->message) {
    return;
  }

  std::string error_str(error->message);
  if (!error_str.empty() && error_str[error_str.length() - 1] == '\n') {
    error_str.resize(error_str.length() - 1);
  }

  std::stringstream ss;
  ss << error_str << ", line " << error->line;

  LOG(INFO) << ss.str();

  HoneycombRefPtr<HoneycombXmlReaderImpl> impl(static_cast<HoneycombXmlReaderImpl*>(userData));
  impl->AppendError(ss.str());
}

HoneycombString xmlCharToString(const xmlChar* xmlStr, bool free) {
  if (!xmlStr) {
    return HoneycombString();
  }

  const char* str = reinterpret_cast<const char*>(xmlStr);
  HoneycombString wstr = std::string(str);

  if (free) {
    xmlFree(const_cast<xmlChar*>(xmlStr));
  }

  return wstr;
}

}  // namespace

HoneycombXmlReaderImpl::HoneycombXmlReaderImpl()
    : supported_thread_id_(base::PlatformThread::CurrentId()),
      reader_(nullptr) {}

HoneycombXmlReaderImpl::~HoneycombXmlReaderImpl() {
  if (reader_ != nullptr) {
    if (!VerifyContext()) {
      // Close() is supposed to be called directly. We'll try to free the reader
      // now on the wrong thread but there's no guarantee this call won't crash.
      xmlFreeTextReader(reader_);
    } else {
      Close();
    }
  }
}

bool HoneycombXmlReaderImpl::Initialize(HoneycombRefPtr<HoneycombStreamReader> stream,
                                  EncodingType encodingType,
                                  const HoneycombString& URI) {
  xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;
  switch (encodingType) {
    case XML_ENCODING_UTF8:
      enc = XML_CHAR_ENCODING_UTF8;
      break;
    case XML_ENCODING_UTF16LE:
      enc = XML_CHAR_ENCODING_UTF16LE;
      break;
    case XML_ENCODING_UTF16BE:
      enc = XML_CHAR_ENCODING_UTF16BE;
      break;
    case XML_ENCODING_ASCII:
      enc = XML_CHAR_ENCODING_ASCII;
      break;
    default:
      break;
  }

  // Create the input buffer.
  xmlParserInputBufferPtr input_buffer = xmlAllocParserInputBuffer(enc);
  if (!input_buffer) {
    return false;
  }

  input_buffer->context = stream.get();
  input_buffer->readcallback = xml_read_callback;

  // Create the text reader.
  std::string uriStr = URI;
  reader_ = xmlNewTextReader(input_buffer, uriStr.c_str());
  if (!reader_) {
    // Free the input buffer.
    xmlFreeParserInputBuffer(input_buffer);
    return false;
  }

  // Keep a reference to the stream.
  stream_ = stream;

  // Register the error callbacks.
  xmlTextReaderSetErrorHandler(reader_, xml_error_callback, this);
  xmlTextReaderSetStructuredErrorHandler(reader_, xml_structured_error_callback,
                                         this);

  return true;
}

bool HoneycombXmlReaderImpl::MoveToNextNode() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderRead(reader_) == 1 ? true : false;
}

bool HoneycombXmlReaderImpl::Close() {
  if (!VerifyContext()) {
    return false;
  }

  // The input buffer will be freed automatically.
  xmlFreeTextReader(reader_);
  reader_ = nullptr;
  return true;
}

bool HoneycombXmlReaderImpl::HasError() {
  if (!VerifyContext()) {
    return false;
  }

  return !error_buf_.str().empty();
}

HoneycombString HoneycombXmlReaderImpl::GetError() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return error_buf_.str();
}

HoneycombXmlReader::NodeType HoneycombXmlReaderImpl::GetType() {
  if (!VerifyContext()) {
    return XML_NODE_UNSUPPORTED;
  }

  switch (xmlTextReaderNodeType(reader_)) {
    case XML_READER_TYPE_ELEMENT:
      return XML_NODE_ELEMENT_START;
    case XML_READER_TYPE_END_ELEMENT:
      return XML_NODE_ELEMENT_END;
    case XML_READER_TYPE_ATTRIBUTE:
      return XML_NODE_ATTRIBUTE;
    case XML_READER_TYPE_TEXT:
      return XML_NODE_TEXT;
    case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
    case XML_READER_TYPE_WHITESPACE:
      return XML_NODE_WHITESPACE;
    case XML_READER_TYPE_CDATA:
      return XML_NODE_CDATA;
    case XML_READER_TYPE_ENTITY_REFERENCE:
      return XML_NODE_ENTITY_REFERENCE;
    case XML_READER_TYPE_PROCESSING_INSTRUCTION:
      return XML_NODE_PROCESSING_INSTRUCTION;
    case XML_READER_TYPE_COMMENT:
      return XML_NODE_COMMENT;
    case XML_READER_TYPE_DOCUMENT_TYPE:
      return XML_NODE_DOCUMENT_TYPE;
    default:
      break;
  }

  return XML_NODE_UNSUPPORTED;
}

int HoneycombXmlReaderImpl::GetDepth() {
  if (!VerifyContext()) {
    return -1;
  }

  return xmlTextReaderDepth(reader_);
}

HoneycombString HoneycombXmlReaderImpl::GetLocalName() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstLocalName(reader_), false);
}

HoneycombString HoneycombXmlReaderImpl::GetPrefix() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstPrefix(reader_), false);
}

HoneycombString HoneycombXmlReaderImpl::GetQualifiedName() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstName(reader_), false);
}

HoneycombString HoneycombXmlReaderImpl::GetNamespaceURI() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstNamespaceUri(reader_), false);
}

HoneycombString HoneycombXmlReaderImpl::GetBaseURI() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstBaseUri(reader_), false);
}

HoneycombString HoneycombXmlReaderImpl::GetXmlLang() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderConstXmlLang(reader_), false);
}

bool HoneycombXmlReaderImpl::IsEmptyElement() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderIsEmptyElement(reader_) == 1 ? true : false;
}

bool HoneycombXmlReaderImpl::HasValue() {
  if (!VerifyContext()) {
    return false;
  }

  if (xmlTextReaderNodeType(reader_) == XML_READER_TYPE_ENTITY_REFERENCE) {
    // Provide special handling to return entity reference values.
    return true;
  } else {
    return xmlTextReaderHasValue(reader_) == 1 ? true : false;
  }
}

HoneycombString HoneycombXmlReaderImpl::GetValue() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  if (xmlTextReaderNodeType(reader_) == XML_READER_TYPE_ENTITY_REFERENCE) {
    // Provide special handling to return entity reference values.
    xmlNodePtr node = xmlTextReaderCurrentNode(reader_);
    if (node->content != nullptr) {
      return xmlCharToString(node->content, false);
    }
    return HoneycombString();
  } else {
    return xmlCharToString(xmlTextReaderConstValue(reader_), false);
  }
}

bool HoneycombXmlReaderImpl::HasAttributes() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderHasAttributes(reader_) == 1 ? true : false;
}

size_t HoneycombXmlReaderImpl::GetAttributeCount() {
  if (!VerifyContext()) {
    return 0;
  }

  return xmlTextReaderAttributeCount(reader_);
}

HoneycombString HoneycombXmlReaderImpl::GetAttribute(int index) {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderGetAttributeNo(reader_, index), true);
}

HoneycombString HoneycombXmlReaderImpl::GetAttribute(const HoneycombString& qualifiedName) {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  std::string qualifiedNameStr = qualifiedName;
  return xmlCharToString(
      xmlTextReaderGetAttribute(reader_, BAD_CAST qualifiedNameStr.c_str()),
      true);
}

HoneycombString HoneycombXmlReaderImpl::GetAttribute(const HoneycombString& localName,
                                         const HoneycombString& namespaceURI) {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  std::string localNameStr = localName;
  std::string namespaceURIStr = namespaceURI;
  return xmlCharToString(
      xmlTextReaderGetAttributeNs(reader_, BAD_CAST localNameStr.c_str(),
                                  BAD_CAST namespaceURIStr.c_str()),
      true);
}

HoneycombString HoneycombXmlReaderImpl::GetInnerXml() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderReadInnerXml(reader_), true);
}

HoneycombString HoneycombXmlReaderImpl::GetOuterXml() {
  if (!VerifyContext()) {
    return HoneycombString();
  }

  return xmlCharToString(xmlTextReaderReadOuterXml(reader_), true);
}

int HoneycombXmlReaderImpl::GetLineNumber() {
  if (!VerifyContext()) {
    return -1;
  }

  return xmlTextReaderGetParserLineNumber(reader_);
}

bool HoneycombXmlReaderImpl::MoveToAttribute(int index) {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderMoveToAttributeNo(reader_, index) == 1 ? true : false;
}

bool HoneycombXmlReaderImpl::MoveToAttribute(const HoneycombString& qualifiedName) {
  if (!VerifyContext()) {
    return false;
  }

  std::string qualifiedNameStr = qualifiedName;
  return xmlTextReaderMoveToAttribute(reader_,
                                      BAD_CAST qualifiedNameStr.c_str()) == 1
             ? true
             : false;
}

bool HoneycombXmlReaderImpl::MoveToAttribute(const HoneycombString& localName,
                                       const HoneycombString& namespaceURI) {
  if (!VerifyContext()) {
    return false;
  }

  std::string localNameStr = localName;
  std::string namespaceURIStr = namespaceURI;
  return xmlTextReaderMoveToAttributeNs(reader_, BAD_CAST localNameStr.c_str(),
                                        BAD_CAST namespaceURIStr.c_str()) == 1
             ? true
             : false;
}

bool HoneycombXmlReaderImpl::MoveToFirstAttribute() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderMoveToFirstAttribute(reader_) == 1 ? true : false;
}

bool HoneycombXmlReaderImpl::MoveToNextAttribute() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderMoveToNextAttribute(reader_) == 1 ? true : false;
}

bool HoneycombXmlReaderImpl::MoveToCarryingElement() {
  if (!VerifyContext()) {
    return false;
  }

  return xmlTextReaderMoveToElement(reader_) == 1 ? true : false;
}

void HoneycombXmlReaderImpl::AppendError(const HoneycombString& error_str) {
  if (!error_buf_.str().empty()) {
    error_buf_ << L"\n";
  }
  error_buf_ << error_str.ToString();
}

bool HoneycombXmlReaderImpl::VerifyContext() {
  if (base::PlatformThread::CurrentId() != supported_thread_id_) {
    // This object should only be accessed from the thread that created it.
    DCHECK(false);
    return false;
  }

  return (reader_ != nullptr);
}
