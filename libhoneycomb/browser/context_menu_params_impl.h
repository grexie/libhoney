// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
#define HONEYCOMB_LIBHONEYCOMB_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
#pragma once

#include "include/honey_context_menu_handler.h"
#include "libhoneycomb/common/value_base.h"

#include "content/public/browser/context_menu_params.h"

// HoneycombContextMenuParams implementation. This class is not thread safe.
class HoneycombContextMenuParamsImpl
    : public HoneycombValueBase<HoneycombContextMenuParams, content::ContextMenuParams> {
 public:
  explicit HoneycombContextMenuParamsImpl(content::ContextMenuParams* value);

  HoneycombContextMenuParamsImpl(const HoneycombContextMenuParamsImpl&) = delete;
  HoneycombContextMenuParamsImpl& operator=(const HoneycombContextMenuParamsImpl&) = delete;

  // HoneycombContextMenuParams methods.
  int GetXCoord() override;
  int GetYCoord() override;
  TypeFlags GetTypeFlags() override;
  HoneycombString GetLinkUrl() override;
  HoneycombString GetUnfilteredLinkUrl() override;
  HoneycombString GetSourceUrl() override;
  bool HasImageContents() override;
  HoneycombString GetTitleText() override;
  HoneycombString GetPageUrl() override;
  HoneycombString GetFrameUrl() override;
  HoneycombString GetFrameCharset() override;
  MediaType GetMediaType() override;
  MediaStateFlags GetMediaStateFlags() override;
  HoneycombString GetSelectionText() override;
  HoneycombString GetMisspelledWord() override;
  bool GetDictionarySuggestions(std::vector<HoneycombString>& suggestions) override;
  bool IsEditable() override;
  bool IsSpellCheckEnabled() override;
  EditStateFlags GetEditStateFlags() override;
  bool IsCustomMenu() override;
};

#endif  // HONEYCOMB_LIBHONEYCOMB_BROWSER_CONTEXT_MENU_PARAMS_IMPL_H_
