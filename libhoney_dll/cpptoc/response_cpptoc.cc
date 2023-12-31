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
// $hash=f0c69112a4eba6862aa54a4df7fa10f7e361edf7$
//

#include "libhoney_dll/cpptoc/response_cpptoc.h"
#include "libhoney_dll/transfer_util.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

HONEYCOMB_EXPORT honey_response_t* honey_response_create() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  HoneycombRefPtr<HoneycombResponse> _retval = HoneycombResponse::Create();

  // Return type: refptr_same
  return HoneycombResponseCppToC::Wrap(_retval);
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int HONEYCOMB_CALLBACK response_is_read_only(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombResponseCppToC::Get(self)->IsReadOnly();

  // Return type: bool
  return _retval;
}

honey_errorcode_t HONEYCOMB_CALLBACK response_get_error(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return ERR_NONE;
  }

  // Execute
  honey_errorcode_t _retval = HoneycombResponseCppToC::Get(self)->GetError();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK response_set_error(struct _honey_response_t* self,
                                     honey_errorcode_t error) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombResponseCppToC::Get(self)->SetError(error);
}

int HONEYCOMB_CALLBACK response_get_status(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return 0;
  }

  // Execute
  int _retval = HoneycombResponseCppToC::Get(self)->GetStatus();

  // Return type: simple
  return _retval;
}

void HONEYCOMB_CALLBACK response_set_status(struct _honey_response_t* self,
                                      int status) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }

  // Execute
  HoneycombResponseCppToC::Get(self)->SetStatus(status);
}

honey_string_userfree_t HONEYCOMB_CALLBACK
response_get_status_text(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombResponseCppToC::Get(self)->GetStatusText();

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK response_set_status_text(struct _honey_response_t* self,
                                           const honey_string_t* statusText) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Unverified params: statusText

  // Execute
  HoneycombResponseCppToC::Get(self)->SetStatusText(HoneycombString(statusText));
}

honey_string_userfree_t HONEYCOMB_CALLBACK
response_get_mime_type(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombResponseCppToC::Get(self)->GetMimeType();

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK response_set_mime_type(struct _honey_response_t* self,
                                         const honey_string_t* mimeType) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Unverified params: mimeType

  // Execute
  HoneycombResponseCppToC::Get(self)->SetMimeType(HoneycombString(mimeType));
}

honey_string_userfree_t HONEYCOMB_CALLBACK
response_get_charset(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombResponseCppToC::Get(self)->GetCharset();

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK response_set_charset(struct _honey_response_t* self,
                                       const honey_string_t* charset) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Unverified params: charset

  // Execute
  HoneycombResponseCppToC::Get(self)->SetCharset(HoneycombString(charset));
}

honey_string_userfree_t HONEYCOMB_CALLBACK
response_get_header_by_name(struct _honey_response_t* self,
                            const honey_string_t* name) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }
  // Verify param: name; type: string_byref_const
  DCHECK(name);
  if (!name) {
    return NULL;
  }

  // Execute
  HoneycombString _retval =
      HoneycombResponseCppToC::Get(self)->GetHeaderByName(HoneycombString(name));

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK response_set_header_by_name(struct _honey_response_t* self,
                                              const honey_string_t* name,
                                              const honey_string_t* value,
                                              int overwrite) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: name; type: string_byref_const
  DCHECK(name);
  if (!name) {
    return;
  }
  // Unverified params: value

  // Execute
  HoneycombResponseCppToC::Get(self)->SetHeaderByName(
      HoneycombString(name), HoneycombString(value), overwrite ? true : false);
}

void HONEYCOMB_CALLBACK response_get_header_map(struct _honey_response_t* self,
                                          honey_string_multimap_t headerMap) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: headerMap; type: string_map_multi_byref
  DCHECK(headerMap);
  if (!headerMap) {
    return;
  }

  // Translate param: headerMap; type: string_map_multi_byref
  std::multimap<HoneycombString, HoneycombString> headerMapMultimap;
  transfer_string_multimap_contents(headerMap, headerMapMultimap);

  // Execute
  HoneycombResponseCppToC::Get(self)->GetHeaderMap(headerMapMultimap);

  // Restore param: headerMap; type: string_map_multi_byref
  honey_string_multimap_clear(headerMap);
  transfer_string_multimap_contents(headerMapMultimap, headerMap);
}

void HONEYCOMB_CALLBACK response_set_header_map(struct _honey_response_t* self,
                                          honey_string_multimap_t headerMap) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Verify param: headerMap; type: string_map_multi_byref_const
  DCHECK(headerMap);
  if (!headerMap) {
    return;
  }

  // Translate param: headerMap; type: string_map_multi_byref_const
  std::multimap<HoneycombString, HoneycombString> headerMapMultimap;
  transfer_string_multimap_contents(headerMap, headerMapMultimap);

  // Execute
  HoneycombResponseCppToC::Get(self)->SetHeaderMap(headerMapMultimap);
}

honey_string_userfree_t HONEYCOMB_CALLBACK
response_get_url(struct _honey_response_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombResponseCppToC::Get(self)->GetURL();

  // Return type: string
  return _retval.DetachToUserFree();
}

void HONEYCOMB_CALLBACK response_set_url(struct _honey_response_t* self,
                                   const honey_string_t* url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self) {
    return;
  }
  // Unverified params: url

  // Execute
  HoneycombResponseCppToC::Get(self)->SetURL(HoneycombString(url));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

HoneycombResponseCppToC::HoneycombResponseCppToC() {
  GetStruct()->is_read_only = response_is_read_only;
  GetStruct()->get_error = response_get_error;
  GetStruct()->set_error = response_set_error;
  GetStruct()->get_status = response_get_status;
  GetStruct()->set_status = response_set_status;
  GetStruct()->get_status_text = response_get_status_text;
  GetStruct()->set_status_text = response_set_status_text;
  GetStruct()->get_mime_type = response_get_mime_type;
  GetStruct()->set_mime_type = response_set_mime_type;
  GetStruct()->get_charset = response_get_charset;
  GetStruct()->set_charset = response_set_charset;
  GetStruct()->get_header_by_name = response_get_header_by_name;
  GetStruct()->set_header_by_name = response_set_header_by_name;
  GetStruct()->get_header_map = response_get_header_map;
  GetStruct()->set_header_map = response_set_header_map;
  GetStruct()->get_url = response_get_url;
  GetStruct()->set_url = response_set_url;
}

// DESTRUCTOR - Do not edit by hand.

HoneycombResponseCppToC::~HoneycombResponseCppToC() {}

template <>
HoneycombRefPtr<HoneycombResponse>
HoneycombCppToCRefCounted<HoneycombResponseCppToC, HoneycombResponse, honey_response_t>::
    UnwrapDerived(HoneycombWrapperType type, honey_response_t* s) {
  DCHECK(false) << "Unexpected class type: " << type;
  return nullptr;
}

template <>
HoneycombWrapperType HoneycombCppToCRefCounted<HoneycombResponseCppToC,
                                   HoneycombResponse,
                                   honey_response_t>::kWrapperType = WT_RESPONSE;
