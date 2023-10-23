// Copyright (c) 2014 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef libhoneycombCOMB_BROWSER_PRINTING_PRINT_DIALOG_LINUX_H_
#define libhoneycombCOMB_BROWSER_PRINTING_PRINT_DIALOG_LINUX_H_

#include "include/honey_print_handler.h"
#include "libhoneycomb/browser/browser_host_base.h"

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/task/sequenced_task_runner_helpers.h"
#include "content/public/browser/browser_thread.h"
#include "printing/buildflags/buildflags.h"
#include "printing/print_dialog_linux_interface.h"
#include "ui/linux/linux_ui.h"

namespace printing {
class MetafilePlayer;
class PrintSettings;
}  // namespace printing

using printing::PrintingContextLinux;

class HoneycombPrintingContextLinuxDelegate
    : public ui::PrintingContextLinuxDelegate {
 public:
  HoneycombPrintingContextLinuxDelegate();

  HoneycombPrintingContextLinuxDelegate(const HoneycombPrintingContextLinuxDelegate&) =
      delete;
  HoneycombPrintingContextLinuxDelegate& operator=(
      const HoneycombPrintingContextLinuxDelegate&) = delete;

  printing::PrintDialogLinuxInterface* CreatePrintDialog(
      printing::PrintingContextLinux* context) override;
  gfx::Size GetPdfPaperSize(printing::PrintingContextLinux* context) override;

  void SetDefaultDelegate(ui::PrintingContextLinuxDelegate* delegate);

 private:
  ui::PrintingContextLinuxDelegate* default_delegate_ = nullptr;
};

// Needs to be freed on the UI thread to clean up its member variables.
class HoneycombPrintDialogLinux : public printing::PrintDialogLinuxInterface,
                            public base::RefCountedThreadSafe<
                                HoneycombPrintDialogLinux,
                                content::BrowserThread::DeleteOnUIThread> {
 public:
  HoneycombPrintDialogLinux(const HoneycombPrintDialogLinux&) = delete;
  HoneycombPrintDialogLinux& operator=(const HoneycombPrintDialogLinux&) = delete;

  // PrintDialogLinuxInterface implementation.
  void UseDefaultSettings() override;
  void UpdateSettings(
      std::unique_ptr<printing::PrintSettings> settings) override;
#if BUILDFLAG(ENABLE_OOP_PRINTING_NO_OOP_BASIC_PRINT_DIALOG)
  void LoadPrintSettings(const printing::PrintSettings& settings) override;
#endif
  void ShowDialog(
      gfx::NativeView parent_view,
      bool has_selection,
      PrintingContextLinux::PrintSettingsCallback callback) override;
  void PrintDocument(const printing::MetafilePlayer& metafile,
                     const std::u16string& document_name) override;
  void ReleaseDialog() override;

 private:
  friend class base::DeleteHelper<HoneycombPrintDialogLinux>;
  friend class base::RefCountedThreadSafe<
      HoneycombPrintDialogLinux,
      content::BrowserThread::DeleteOnUIThread>;
  friend struct content::BrowserThread::DeleteOnThread<
      content::BrowserThread::UI>;
  friend class HoneycombPrintDialogCallbackImpl;
  friend class HoneycombPrintJobCallbackImpl;
  friend class HoneycombPrintingContextLinuxDelegate;

  HoneycombPrintDialogLinux(PrintingContextLinux* context,
                      HoneycombRefPtr<HoneycombBrowserHostBase> browser,
                      HoneycombRefPtr<HoneycombPrintHandler> handler);
  ~HoneycombPrintDialogLinux() override;

  bool UpdateSettings(std::unique_ptr<printing::PrintSettings> settings,
                      bool get_defaults);

  // Prints document named |document_name|.
  void SendDocumentToPrinter(const std::u16string& document_name);

  // Handles print dialog response.
  void OnPrintContinue(HoneycombRefPtr<HoneycombPrintSettings> settings);
  void OnPrintCancel();

  // Handles print job response.
  void OnJobCompleted();

  // Printing dialog callback.
  PrintingContextLinux::PrintSettingsCallback callback_;

  PrintingContextLinux* context_;
  HoneycombRefPtr<HoneycombBrowserHostBase> browser_;
  HoneycombRefPtr<HoneycombPrintHandler> handler_;

  base::FilePath path_to_pdf_;
};

#endif  // libhoneycombCOMB_BROWSER_PRINTING_PRINT_DIALOG_LINUX_H_
