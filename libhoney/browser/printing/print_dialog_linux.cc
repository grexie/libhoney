// Copyright (c) 2014 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoney/browser/printing/print_dialog_linux.h"

#include <string>
#include <vector>

#include "libhoney/browser/extensions/browser_extensions_util.h"
#include "libhoney/browser/print_settings_impl.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/common/app_manager.h"
#include "libhoney/common/frame_util.h"

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "content/public/browser/global_routing_id.h"
#include "printing/metafile.h"
#include "printing/mojom/print.mojom-shared.h"
#include "printing/print_job_constants.h"
#include "printing/print_settings.h"

using content::BrowserThread;
using printing::PageRanges;
using printing::PrintSettings;

namespace {

HoneycombRefPtr<HoneycombBrowserHostBase> GetBrowserForContext(
    printing::PrintingContextLinux* context) {
  // The print preview dialog won't have a valid child ID.
  if (!frame_util::IsValidChildId(context->render_process_id())) {
    return nullptr;
  }

  return extensions::GetOwnerBrowserForGlobalId(
      frame_util::MakeGlobalId(context->render_process_id(),
                               context->render_frame_id()),
      nullptr);
}

HoneycombRefPtr<HoneycombPrintHandler> GetPrintHandlerForBrowser(
    HoneycombRefPtr<HoneycombBrowserHostBase> browser) {
  if (browser) {
    if (auto client = browser->GetClient()) {
      return client->GetPrintHandler();
    }
  }
  return nullptr;
}

}  // namespace

class HoneycombPrintDialogCallbackImpl : public HoneycombPrintDialogCallback {
 public:
  explicit HoneycombPrintDialogCallbackImpl(HoneycombRefPtr<HoneycombPrintDialogLinux> dialog)
      : dialog_(dialog) {}

  HoneycombPrintDialogCallbackImpl(const HoneycombPrintDialogCallbackImpl&) = delete;
  HoneycombPrintDialogCallbackImpl& operator=(const HoneycombPrintDialogCallbackImpl&) =
      delete;

  void Continue(HoneycombRefPtr<HoneycombPrintSettings> settings) override {
    if (HONEYCOMB_CURRENTLY_ON_UIT()) {
      if (dialog_.get()) {
        dialog_->OnPrintContinue(settings);
        dialog_ = nullptr;
      }
    } else {
      HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                    base::BindOnce(&HoneycombPrintDialogCallbackImpl::Continue, this,
                                   settings));
    }
  }

  void Cancel() override {
    if (HONEYCOMB_CURRENTLY_ON_UIT()) {
      if (dialog_.get()) {
        dialog_->OnPrintCancel();
        dialog_ = nullptr;
      }
    } else {
      HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                    base::BindOnce(&HoneycombPrintDialogCallbackImpl::Cancel, this));
    }
  }

  void Disconnect() { dialog_ = nullptr; }

 private:
  HoneycombRefPtr<HoneycombPrintDialogLinux> dialog_;

  IMPLEMENT_REFCOUNTING(HoneycombPrintDialogCallbackImpl);
};

class HoneycombPrintJobCallbackImpl : public HoneycombPrintJobCallback {
 public:
  explicit HoneycombPrintJobCallbackImpl(HoneycombRefPtr<HoneycombPrintDialogLinux> dialog)
      : dialog_(dialog) {}

  HoneycombPrintJobCallbackImpl(const HoneycombPrintJobCallbackImpl&) = delete;
  HoneycombPrintJobCallbackImpl& operator=(const HoneycombPrintJobCallbackImpl&) = delete;

  void Continue() override {
    if (HONEYCOMB_CURRENTLY_ON_UIT()) {
      if (dialog_.get()) {
        dialog_->OnJobCompleted();
        dialog_ = nullptr;
      }
    } else {
      HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                    base::BindOnce(&HoneycombPrintJobCallbackImpl::Continue, this));
    }
  }

  void Disconnect() { dialog_ = nullptr; }

 private:
  HoneycombRefPtr<HoneycombPrintDialogLinux> dialog_;

  IMPLEMENT_REFCOUNTING(HoneycombPrintJobCallbackImpl);
};

HoneycombPrintingContextLinuxDelegate::HoneycombPrintingContextLinuxDelegate() = default;

printing::PrintDialogLinuxInterface*
HoneycombPrintingContextLinuxDelegate::CreatePrintDialog(
    printing::PrintingContextLinux* context) {
  HONEYCOMB_REQUIRE_UIT();

  printing::PrintDialogLinuxInterface* interface = nullptr;

  auto browser = GetBrowserForContext(context);
  if (!browser) {
    LOG(ERROR) << "No associated browser in CreatePrintDialog; using default "
                  "printing implementation.";
  }

  auto handler = GetPrintHandlerForBrowser(browser);
  if (!handler) {
    if (default_delegate_) {
      interface = default_delegate_->CreatePrintDialog(context);
      DCHECK(interface);
    }
  } else {
    interface = new HoneycombPrintDialogLinux(context, browser, handler);
  }

  if (!interface) {
    LOG(ERROR) << "Null interface in CreatePrintDialog; printing will fail.";
  }

  return interface;
}

gfx::Size HoneycombPrintingContextLinuxDelegate::GetPdfPaperSize(
    printing::PrintingContextLinux* context) {
  HONEYCOMB_REQUIRE_UIT();

  gfx::Size size;

  auto browser = GetBrowserForContext(context);
  if (!browser) {
    LOG(ERROR) << "No associated browser in GetPdfPaperSize; using default "
                  "printing implementation.";
  }

  auto handler = GetPrintHandlerForBrowser(browser);
  if (!handler) {
    if (default_delegate_) {
      size = default_delegate_->GetPdfPaperSize(context);
      DCHECK(!size.IsEmpty());
    }
  } else {
    const printing::PrintSettings& settings = context->settings();
    HoneycombSize honey_size = handler->GetPdfPaperSize(
        browser.get(), settings.device_units_per_inch());
    size.SetSize(honey_size.width, honey_size.height);
  }

  if (size.IsEmpty()) {
    LOG(ERROR) << "Empty size value returned in GetPdfPaperSize; PDF printing "
                  "will fail.";
  }
  return size;
}

void HoneycombPrintingContextLinuxDelegate::SetDefaultDelegate(
    ui::PrintingContextLinuxDelegate* delegate) {
  DCHECK(!default_delegate_);
  default_delegate_ = delegate;
}

HoneycombPrintDialogLinux::HoneycombPrintDialogLinux(PrintingContextLinux* context,
                                         HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                                         HoneycombRefPtr<HoneycombPrintHandler> handler)
    : context_(context), browser_(browser), handler_(handler) {
  HONEYCOMB_REQUIRE_UIT();
  DCHECK(context_);
  DCHECK(browser_);
  DCHECK(handler_);

  // Paired with the ReleaseDialog() call.
  AddRef();

  handler->OnPrintStart(browser_.get());
}

HoneycombPrintDialogLinux::~HoneycombPrintDialogLinux() {
  // It's not safe to dereference |context_| during the destruction of this
  // object because the PrintJobWorker which owns |context_| may already have
  // been deleted.
  HONEYCOMB_REQUIRE_UIT();
  handler_->OnPrintReset(browser_.get());
}

void HoneycombPrintDialogLinux::UseDefaultSettings() {
  UpdateSettings(std::make_unique<PrintSettings>(), true);
}

void HoneycombPrintDialogLinux::UpdateSettings(
    std::unique_ptr<PrintSettings> settings) {
  UpdateSettings(std::move(settings), false);
}

#if BUILDFLAG(ENABLE_OOP_PRINTING_NO_OOP_BASIC_PRINT_DIALOG)
void HoneycombPrintDialogLinux::LoadPrintSettings(
    const printing::PrintSettings& settings) {
  // TODO(linux): Need to read data from |settings.system_print_dialog_data()|?
  UseDefaultSettings();
}
#endif

void HoneycombPrintDialogLinux::ShowDialog(
    gfx::NativeView parent_view,
    bool has_selection,
    PrintingContextLinux::PrintSettingsCallback callback) {
  HONEYCOMB_REQUIRE_UIT();

  callback_ = std::move(callback);

  HoneycombRefPtr<HoneycombPrintDialogCallbackImpl> callback_impl(
      new HoneycombPrintDialogCallbackImpl(this));

  if (!handler_->OnPrintDialog(browser_.get(), has_selection,
                               callback_impl.get())) {
    callback_impl->Disconnect();
    OnPrintCancel();
  }
}

void HoneycombPrintDialogLinux::PrintDocument(
    const printing::MetafilePlayer& metafile,
    const std::u16string& document_name) {
  // This runs on the print worker thread, does not block the UI thread.
  DCHECK(!HONEYCOMB_CURRENTLY_ON_UIT());

  // The document printing tasks can outlive the PrintingContext that created
  // this dialog.
  AddRef();

  bool success = base::CreateTemporaryFile(&path_to_pdf_);

  if (success) {
    base::File file;
    file.Initialize(path_to_pdf_,
                    base::File::FLAG_CREATE_ALWAYS | base::File::FLAG_WRITE);
    success = metafile.SaveTo(&file);
    file.Close();
    if (!success) {
      base::DeleteFile(path_to_pdf_);
    }
  }

  if (!success) {
    LOG(ERROR) << "Saving metafile failed";
    // Matches AddRef() above.
    Release();
    return;
  }

  // No errors, continue printing.
  HONEYCOMB_POST_TASK(
      HONEYCOMB_UIT, base::BindOnce(&HoneycombPrintDialogLinux::SendDocumentToPrinter, this,
                              document_name));
}

void HoneycombPrintDialogLinux::ReleaseDialog() {
  context_ = nullptr;
  Release();
}

bool HoneycombPrintDialogLinux::UpdateSettings(
    std::unique_ptr<PrintSettings> settings,
    bool get_defaults) {
  HONEYCOMB_REQUIRE_UIT();

  HoneycombRefPtr<HoneycombPrintSettingsImpl> settings_impl(
      new HoneycombPrintSettingsImpl(std::move(settings), false));
  handler_->OnPrintSettings(browser_.get(), settings_impl.get(), get_defaults);

  context_->InitWithSettings(settings_impl->TakeOwnership());
  return true;
}

void HoneycombPrintDialogLinux::SendDocumentToPrinter(
    const std::u16string& document_name) {
  HONEYCOMB_REQUIRE_UIT();

  if (!handler_.get()) {
    OnJobCompleted();
    return;
  }

  HoneycombRefPtr<HoneycombPrintJobCallbackImpl> callback_impl(
      new HoneycombPrintJobCallbackImpl(this));

  if (!handler_->OnPrintJob(browser_.get(), document_name, path_to_pdf_.value(),
                            callback_impl.get())) {
    callback_impl->Disconnect();
    OnJobCompleted();
  }
}

void HoneycombPrintDialogLinux::OnPrintContinue(
    HoneycombRefPtr<HoneycombPrintSettings> settings) {
  HoneycombPrintSettingsImpl* impl =
      static_cast<HoneycombPrintSettingsImpl*>(settings.get());
  context_->InitWithSettings(impl->TakeOwnership());
  std::move(callback_).Run(printing::mojom::ResultCode::kSuccess);
}

void HoneycombPrintDialogLinux::OnPrintCancel() {
  std::move(callback_).Run(printing::mojom::ResultCode::kCanceled);
}

void HoneycombPrintDialogLinux::OnJobCompleted() {
  HONEYCOMB_POST_BACKGROUND_TASK(base::GetDeleteFileCallback(path_to_pdf_));

  // Printing finished. Matches AddRef() in PrintDocument();
  Release();
}
