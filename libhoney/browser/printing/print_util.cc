// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/printing/print_util.h"

#include "libhoney/browser/thread_util.h"

#include "base/files/file_util.h"
#include "chrome/browser/printing/print_view_manager.h"
#include "chrome/browser/printing/print_view_manager_common.h"
#include "components/printing/browser/print_to_pdf/pdf_print_utils.h"

namespace print_util {

namespace {

// Write the PDF file to disk.
void SavePdfFile(const HoneycombString& path,
                 HoneycombRefPtr<HoneycombPdfPrintCallback> callback,
                 scoped_refptr<base::RefCountedMemory> data) {
  HONEYCOMB_REQUIRE_BLOCKING();
  DCHECK_GT(data->size(), 0U);

  const bool ok =
      base::WriteFile(path, reinterpret_cast<const char*>(data->data()),
                      data->size()) == static_cast<int>(data->size());

  if (callback) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombPdfPrintCallback::OnPdfPrintFinished,
                                 callback, path, ok));
  }
}

void OnPDFCreated(const HoneycombString& path,
                  HoneycombRefPtr<HoneycombPdfPrintCallback> callback,
                  print_to_pdf::PdfPrintResult print_result,
                  scoped_refptr<base::RefCountedMemory> data) {
  if (print_result != print_to_pdf::PdfPrintResult::kPrintSuccess) {
    LOG(ERROR) << "PrintToPDF failed with error: "
               << print_to_pdf::PdfPrintResultToString(print_result);
    callback->OnPdfPrintFinished(HoneycombString(), false);
    return;
  }

  // Save the PDF file to disk and then execute the callback.
  HONEYCOMB_POST_USER_VISIBLE_TASK(
      base::BindOnce(&SavePdfFile, path, callback, std::move(data)));
}

}  // namespace

void Print(content::WebContents* web_contents, bool print_preview_disabled) {
  // Like chrome::Print() but specifying the WebContents.
  printing::StartPrint(web_contents, print_preview_disabled,
                       /*has_selection=*/false);
}

// Implementation based on PageHandler::PrintToPDF.
void PrintToPDF(content::WebContents* web_contents,
                const HoneycombString& path,
                const HoneycombPdfPrintSettings& settings,
                HoneycombRefPtr<HoneycombPdfPrintCallback> callback) {
  const bool display_header_footer = !!settings.display_header_footer;

  // Defaults to no header/footer.
  absl::optional<std::string> header_template, footer_template;
  if (display_header_footer) {
    if (settings.header_template.length > 0) {
      header_template = HoneycombString(&settings.header_template);
    }
    if (settings.footer_template.length > 0) {
      footer_template = HoneycombString(&settings.footer_template);
    }
  }

  // Defaults to 1.0.
  absl::optional<double> scale;
  if (settings.scale > 0) {
    scale = settings.scale;
  }

  // Defaults to letter size.
  absl::optional<double> paper_width, paper_height;
  if (settings.paper_width > 0 && settings.paper_height > 0) {
    paper_width = settings.paper_width;
    paper_height = settings.paper_height;
  }

  // Defaults to kDefaultMarginInInches.
  absl::optional<double> margin_top, margin_bottom, margin_left, margin_right;
  if (settings.margin_type == PDF_PRINT_MARGIN_NONE) {
    margin_top = 0;
    margin_bottom = 0;
    margin_left = 0;
    margin_right = 0;
  } else if (settings.margin_type == PDF_PRINT_MARGIN_CUSTOM) {
    if (settings.margin_top >= 0) {
      margin_top = settings.margin_top;
    }
    if (settings.margin_bottom >= 0) {
      margin_bottom = settings.margin_bottom;
    }
    if (settings.margin_left >= 0) {
      margin_left = settings.margin_left;
    }
    if (settings.margin_right >= 0) {
      margin_right = settings.margin_right;
    }
  }

  absl::variant<printing::mojom::PrintPagesParamsPtr, std::string>
      print_pages_params = print_to_pdf::GetPrintPagesParams(
          web_contents->GetPrimaryMainFrame()->GetLastCommittedURL(),
          !!settings.landscape, display_header_footer,
          !!settings.print_background, scale, paper_width, paper_height,
          margin_top, margin_bottom, margin_left, margin_right,
          HoneycombString(&settings.header_template),
          HoneycombString(&settings.footer_template), !!settings.prefer_css_page_size,
          !!settings.generate_tagged_pdf);

  if (absl::holds_alternative<std::string>(print_pages_params)) {
    LOG(ERROR) << "PrintToPDF failed with error: "
               << absl::get<std::string>(print_pages_params);
    callback->OnPdfPrintFinished(HoneycombString(), false);
    return;
  }

  DCHECK(absl::holds_alternative<printing::mojom::PrintPagesParamsPtr>(
      print_pages_params));

  if (auto* print_manager =
          printing::PrintViewManager::FromWebContents(web_contents)) {
    print_manager->PrintToPdf(
        web_contents->GetPrimaryMainFrame(), HoneycombString(&settings.page_ranges),
        std::move(absl::get<printing::mojom::PrintPagesParamsPtr>(
            print_pages_params)),
        base::BindOnce(&OnPDFCreated, path, callback));
    return;
  }

  LOG(ERROR) << "PrintToPDF was not handled.";
  callback->OnPdfPrintFinished(HoneycombString(), false);
}

}  // namespace print_util
