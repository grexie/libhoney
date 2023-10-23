// Copyright 2015 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_
#define HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_

#include "components/printing/renderer/print_render_frame_helper.h"

namespace extensions {

class HoneycombPrintRenderFrameHelperDelegate
    : public printing::PrintRenderFrameHelper::Delegate {
 public:
  explicit HoneycombPrintRenderFrameHelperDelegate(bool is_windowless);
  ~HoneycombPrintRenderFrameHelperDelegate() override;

  blink::WebElement GetPdfElement(blink::WebLocalFrame* frame) override;
  bool IsPrintPreviewEnabled() override;
  bool ShouldGenerateTaggedPDF() override;
  bool OverridePrint(blink::WebLocalFrame* frame) override;

 private:
  bool is_windowless_;
};

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEYCOMB_RENDERER_EXTENSIONS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_
