// Copyright (c) 2014 The Honeycomb Authors.
// Portions Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_PRINT_HANDLER_GTK_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_PRINT_HANDLER_GTK_H_
#pragma once

#include <memory>

#include "include/honey_print_handler.h"

namespace client {

class ClientPrintHandlerGtk : public HoneycombPrintHandler {
 public:
  ClientPrintHandlerGtk();
  virtual ~ClientPrintHandlerGtk();

  // HoneycombPrintHandler methods.
  void OnPrintStart(HoneycombRefPtr<HoneycombBrowser> browser) override;
  void OnPrintSettings(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRefPtr<HoneycombPrintSettings> settings,
                       bool get_defaults) override;
  bool OnPrintDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                     bool has_selection,
                     HoneycombRefPtr<HoneycombPrintDialogCallback> callback) override;
  bool OnPrintJob(HoneycombRefPtr<HoneycombBrowser> browser,
                  const HoneycombString& document_name,
                  const HoneycombString& pdf_file_path,
                  HoneycombRefPtr<HoneycombPrintJobCallback> callback) override;
  void OnPrintReset(HoneycombRefPtr<HoneycombBrowser> browser) override;
  HoneycombSize GetPdfPaperSize(HoneycombRefPtr<HoneycombBrowser> browser,
                          int device_units_per_inch) override;

 private:
  // Print handler.
  struct PrintHandler;
  std::unique_ptr<PrintHandler> print_handler_;

  IMPLEMENT_REFCOUNTING(ClientPrintHandlerGtk);
  DISALLOW_COPY_AND_ASSIGN(ClientPrintHandlerGtk);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_PRINT_HANDLER_GTK_H_
