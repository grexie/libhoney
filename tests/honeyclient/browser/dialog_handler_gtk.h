// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
#define HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
#pragma once

#include <gtk/gtk.h>

#include "include/base/honey_callback_forward.h"
#include "include/honey_dialog_handler.h"
#include "include/honey_jsdialog_handler.h"

namespace client {

class ClientDialogHandlerGtk : public HoneycombDialogHandler,
                               public HoneycombJSDialogHandler {
 public:
  ClientDialogHandlerGtk();

  // HoneycombDialogHandler methods.
  bool OnFileDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                    FileDialogMode mode,
                    const HoneycombString& title,
                    const HoneycombString& default_file_path,
                    const std::vector<HoneycombString>& accept_filters,
                    HoneycombRefPtr<HoneycombFileDialogCallback> callback) override;

  // HoneycombJSDialogHandler methods.
  bool OnJSDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                  const HoneycombString& origin_url,
                  JSDialogType dialog_type,
                  const HoneycombString& message_text,
                  const HoneycombString& default_prompt_text,
                  HoneycombRefPtr<HoneycombJSDialogCallback> callback,
                  bool& suppress_message) override;
  bool OnBeforeUnloadDialog(HoneycombRefPtr<HoneycombBrowser> browser,
                            const HoneycombString& message_text,
                            bool is_reload,
                            HoneycombRefPtr<HoneycombJSDialogCallback> callback) override;
  void OnResetDialogState(HoneycombRefPtr<HoneycombBrowser> browser) override;

 private:
  struct OnFileDialogParams {
    HoneycombRefPtr<HoneycombBrowser> browser;
    FileDialogMode mode;
    HoneycombString title;
    HoneycombString default_file_path;
    std::vector<HoneycombString> accept_filters;
    HoneycombRefPtr<HoneycombFileDialogCallback> callback;
  };
  void OnFileDialogContinue(const OnFileDialogParams& params,
                            GtkWindow* window);

  struct OnJSDialogParams {
    HoneycombRefPtr<HoneycombBrowser> browser;
    HoneycombString origin_url;
    JSDialogType dialog_type;
    HoneycombString message_text;
    HoneycombString default_prompt_text;
    HoneycombRefPtr<HoneycombJSDialogCallback> callback;
  };
  void OnJSDialogContinue(const OnJSDialogParams& params, GtkWindow* window);

  void GetWindowAndContinue(HoneycombRefPtr<HoneycombBrowser> browser,
                            base::OnceCallback<void(GtkWindow*)> callback);

  static void OnDialogResponse(GtkDialog* dialog,
                               gint response_id,
                               ClientDialogHandlerGtk* handler);

  GtkWidget* gtk_dialog_;
  HoneycombRefPtr<HoneycombJSDialogCallback> js_dialog_callback_;

  IMPLEMENT_REFCOUNTING(ClientDialogHandlerGtk);
  DISALLOW_COPY_AND_ASSIGN(ClientDialogHandlerGtk);
};

}  // namespace client

#endif  // HONEYCOMB_TESTS_HONEYCOMBCLIENT_BROWSER_DIALOG_HANDLER_GTK_H_
