// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/context_menu_params_impl.h"

#include "base/logging.h"
#include "third_party/blink/public/mojom/context_menu/context_menu.mojom.h"

HoneycombContextMenuParamsImpl::HoneycombContextMenuParamsImpl(
    content::ContextMenuParams* value)
    : HoneycombValueBase<HoneycombContextMenuParams, content::ContextMenuParams>(
          value,
          nullptr,
          kOwnerNoDelete,
          true,
          new HoneycombValueControllerNonThreadSafe()) {
  // Indicate that this object owns the controller.
  SetOwnsController();
}

int HoneycombContextMenuParamsImpl::GetXCoord() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().x;
}

int HoneycombContextMenuParamsImpl::GetYCoord() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().y;
}

HoneycombContextMenuParamsImpl::TypeFlags HoneycombContextMenuParamsImpl::GetTypeFlags() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, CM_TYPEFLAG_NONE);
  const content::ContextMenuParams& params = const_value();
  int type_flags = CM_TYPEFLAG_NONE;
  if (!params.page_url.is_empty()) {
    type_flags |= CM_TYPEFLAG_PAGE;
  }
  if (!params.frame_url.is_empty()) {
    type_flags |= CM_TYPEFLAG_FRAME;
  }
  if (!params.link_url.is_empty()) {
    type_flags |= CM_TYPEFLAG_LINK;
  }
  if (params.media_type != blink::mojom::ContextMenuDataMediaType::kNone) {
    type_flags |= CM_TYPEFLAG_MEDIA;
  }
  if (!params.selection_text.empty()) {
    type_flags |= CM_TYPEFLAG_SELECTION;
  }
  if (params.is_editable) {
    type_flags |= CM_TYPEFLAG_EDITABLE;
  }
  return static_cast<TypeFlags>(type_flags);
}

HoneycombString HoneycombContextMenuParamsImpl::GetLinkUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().link_url.spec();
}

HoneycombString HoneycombContextMenuParamsImpl::GetUnfilteredLinkUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().unfiltered_link_url.spec();
}

HoneycombString HoneycombContextMenuParamsImpl::GetSourceUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().src_url.spec();
}

bool HoneycombContextMenuParamsImpl::HasImageContents() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, true);
  return const_value().has_image_contents;
}

HoneycombString HoneycombContextMenuParamsImpl::GetTitleText() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().title_text;
}

HoneycombString HoneycombContextMenuParamsImpl::GetPageUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().page_url.spec();
}

HoneycombString HoneycombContextMenuParamsImpl::GetFrameUrl() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().frame_url.spec();
}

HoneycombString HoneycombContextMenuParamsImpl::GetFrameCharset() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().frame_charset;
}

HoneycombContextMenuParamsImpl::MediaType HoneycombContextMenuParamsImpl::GetMediaType() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, CM_MEDIATYPE_NONE);
  return static_cast<MediaType>(const_value().media_type);
}

HoneycombContextMenuParamsImpl::MediaStateFlags
HoneycombContextMenuParamsImpl::GetMediaStateFlags() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, CM_MEDIAFLAG_NONE);
  return static_cast<MediaStateFlags>(const_value().media_flags);
}

HoneycombString HoneycombContextMenuParamsImpl::GetSelectionText() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().selection_text;
}

HoneycombString HoneycombContextMenuParamsImpl::GetMisspelledWord() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().misspelled_word;
}

bool HoneycombContextMenuParamsImpl::GetDictionarySuggestions(
    std::vector<HoneycombString>& suggestions) {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);

  if (!suggestions.empty()) {
    suggestions.clear();
  }

  if (const_value().dictionary_suggestions.empty()) {
    return false;
  }

  std::vector<std::u16string>::const_iterator it =
      const_value().dictionary_suggestions.begin();
  for (; it != const_value().dictionary_suggestions.end(); ++it) {
    suggestions.push_back(*it);
  }

  return true;
}

bool HoneycombContextMenuParamsImpl::IsEditable() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().is_editable;
}

bool HoneycombContextMenuParamsImpl::IsSpellCheckEnabled() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().spellcheck_enabled;
}

HoneycombContextMenuParamsImpl::EditStateFlags
HoneycombContextMenuParamsImpl::GetEditStateFlags() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, CM_EDITFLAG_NONE);
  return static_cast<EditStateFlags>(const_value().edit_flags);
}

bool HoneycombContextMenuParamsImpl::IsCustomMenu() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return !const_value().custom_items.empty();
}
