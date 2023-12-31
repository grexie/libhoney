// Copyright (c) 2012 The Honeycomb Authors.
// Portions copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_FILE_DIALOG_MANAGER_H_
#define HONEYCOMB_LIBHONEY_BROWSER_FILE_DIALOG_MANAGER_H_
#pragma once

#include <memory>
#include <set>

#include "include/honey_browser.h"

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/public/mojom/choosers/file_chooser.mojom.h"
#include "ui/shell_dialogs/select_file_dialog.h"

namespace content {
class FileSelectListener;
}  // namespace content

class HoneycombBrowserHostBase;
class HoneycombSelectFileDialogListener;

class HoneycombFileDialogManager {
 public:
  explicit HoneycombFileDialogManager(HoneycombBrowserHostBase* browser);

  HoneycombFileDialogManager(const HoneycombFileDialogManager&) = delete;
  HoneycombFileDialogManager& operator=(const HoneycombFileDialogManager&) = delete;

  ~HoneycombFileDialogManager();

  // Delete the runner to free any platform constructs.
  void Destroy();

  // Run a file dialog with the specified parameters. See
  // HoneycombBrowserHost::RunFileDialog for usage documentation. This method should
  // be called via HoneycombBrowserHostBase::RunFileDialog.
  void RunFileDialog(honey_file_dialog_mode_t mode,
                     const HoneycombString& title,
                     const HoneycombString& default_file_path,
                     const std::vector<HoneycombString>& accept_filters,
                     HoneycombRefPtr<HoneycombRunFileDialogCallback> callback);

  // The argument vector will be empty if the dialog was canceled.
  using RunFileChooserCallback =
      base::OnceCallback<void(const std::vector<base::FilePath>&)>;

  // Run the file dialog specified by |params|. |callback| will be executed
  // synchronously or asynchronously after the dialog is dismissed. This method
  // should be called via HoneycombBrowserHostBase::RunFileChooser.
  void RunFileChooser(const blink::mojom::FileChooserParams& params,
                      RunFileChooserCallback callback);

  // Run a ui::SelectFileDialog with the specified parameters. See
  // ui::SelectFileDialog for usage documentation. This method should be called
  // via HoneycombBrowserHostBase::RunSelectFile. It will be called for all file
  // dialogs after interception via HoneycombSelectFileDialog::SelectFileImpl.
  void RunSelectFile(ui::SelectFileDialog::Listener* listener,
                     std::unique_ptr<ui::SelectFilePolicy> policy,
                     ui::SelectFileDialog::Type type,
                     const std::u16string& title,
                     const base::FilePath& default_path,
                     const ui::SelectFileDialog::FileTypeInfo* file_types,
                     int file_type_index,
                     const base::FilePath::StringType& default_extension,
                     gfx::NativeWindow owning_window,
                     void* params);

  // Must be called when the |listener| passed to RunSelectFile is destroyed.
  void SelectFileListenerDestroyed(ui::SelectFileDialog::Listener* listener);

 private:
  [[nodiscard]] RunFileChooserCallback MaybeRunDelegate(
      const blink::mojom::FileChooserParams& params,
      RunFileChooserCallback callback);

  void SelectFileDoneByDelegateCallback(
      ui::SelectFileDialog::Listener* listener,
      void* params,
      const std::vector<base::FilePath>& paths);
  void SelectFileDoneByListenerCallback(bool listener_destroyed);

  // HoneycombBrowserHostBase pointer is guaranteed to outlive this object.
  HoneycombBrowserHostBase* const browser_;

  // Used when running a platform dialog via RunSelectFile.
  scoped_refptr<ui::SelectFileDialog> dialog_;
  HoneycombSelectFileDialogListener* dialog_listener_ = nullptr;

  // List of all currently active listeners.
  std::set<ui::SelectFileDialog::Listener*> active_listeners_;

  base::WeakPtrFactory<HoneycombFileDialogManager> weak_ptr_factory_{this};
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_JAVASCRIPT_DIALOG_MANAGER_H_
