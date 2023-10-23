// Copyright (c) 2012 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/browser/download_manager_delegate.h"

#include <tuple>

#include "include/honey_download_handler.h"
#include "libhoneycomb/browser/alloy/alloy_browser_host_impl.h"
#include "libhoneycomb/browser/context.h"
#include "libhoneycomb/browser/download_item_impl.h"
#include "libhoneycomb/browser/thread_util.h"

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/common/chrome_constants.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/download_item_utils.h"
#include "content/public/browser/web_contents.h"
#include "net/base/filename_util.h"
#include "third_party/blink/public/mojom/choosers/file_chooser.mojom.h"

using content::DownloadManager;
using content::WebContents;
using download::DownloadItem;

namespace {

// Helper function to retrieve the HoneycombDownloadHandler.
HoneycombRefPtr<HoneycombDownloadHandler> GetDownloadHandler(
    HoneycombRefPtr<AlloyBrowserHostImpl> browser) {
  HoneycombRefPtr<HoneycombClient> client = browser->GetClient();
  if (client.get()) {
    return client->GetDownloadHandler();
  }
  return nullptr;
}

// HoneycombBeforeDownloadCallback implementation.
class HoneycombBeforeDownloadCallbackImpl : public HoneycombBeforeDownloadCallback {
 public:
  HoneycombBeforeDownloadCallbackImpl(const base::WeakPtr<DownloadManager>& manager,
                                uint32_t download_id,
                                const base::FilePath& suggested_name,
                                content::DownloadTargetCallback callback)
      : manager_(manager),
        download_id_(download_id),
        suggested_name_(suggested_name),
        callback_(std::move(callback)) {}

  HoneycombBeforeDownloadCallbackImpl(const HoneycombBeforeDownloadCallbackImpl&) = delete;
  HoneycombBeforeDownloadCallbackImpl& operator=(
      const HoneycombBeforeDownloadCallbackImpl&) = delete;

  void Continue(const HoneycombString& download_path, bool show_dialog) override {
    if (HONEYCOMB_CURRENTLY_ON_UIT()) {
      if (download_id_ <= 0) {
        return;
      }

      if (manager_) {
        base::FilePath path = base::FilePath(download_path);
        HONEYCOMB_POST_USER_VISIBLE_TASK(
            base::BindOnce(&HoneycombBeforeDownloadCallbackImpl::GenerateFilename,
                           manager_, download_id_, suggested_name_, path,
                           show_dialog, std::move(callback_)));
      }

      download_id_ = 0;
    } else {
      HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                    base::BindOnce(&HoneycombBeforeDownloadCallbackImpl::Continue,
                                   this, download_path, show_dialog));
    }
  }

 private:
  static void GenerateFilename(base::WeakPtr<DownloadManager> manager,
                               uint32_t download_id,
                               const base::FilePath& suggested_name,
                               const base::FilePath& download_path,
                               bool show_dialog,
                               content::DownloadTargetCallback callback) {
    HONEYCOMB_REQUIRE_BLOCKING();

    base::FilePath suggested_path = download_path;
    if (!suggested_path.empty()) {
      // Create the directory if necessary.
      base::FilePath dir_path = suggested_path.DirName();
      if (!base::DirectoryExists(dir_path) &&
          !base::CreateDirectory(dir_path)) {
        DCHECK(false) << "failed to create the download directory";
        suggested_path.clear();
      }
    }

    if (suggested_path.empty()) {
      if (base::PathService::Get(base::DIR_TEMP, &suggested_path)) {
        // Use the temp directory.
        suggested_path = suggested_path.Append(suggested_name);
      } else {
        // Use the current working directory.
        suggested_path = suggested_name;
      }
    }

    HONEYCOMB_POST_TASK(
        HONEYCOMB_UIT,
        base::BindOnce(&HoneycombBeforeDownloadCallbackImpl::ChooseDownloadPath,
                       manager, download_id, suggested_path, show_dialog,
                       std::move(callback)));
  }

  static void ChooseDownloadPath(base::WeakPtr<DownloadManager> manager,
                                 uint32_t download_id,
                                 const base::FilePath& suggested_path,
                                 bool show_dialog,
                                 content::DownloadTargetCallback callback) {
    if (!manager) {
      return;
    }

    DownloadItem* item = manager->GetDownload(download_id);
    if (!item || item->GetState() != DownloadItem::IN_PROGRESS) {
      return;
    }

    bool handled = false;

    if (show_dialog) {
      WebContents* web_contents =
          content::DownloadItemUtils::GetWebContents(item);
      HoneycombRefPtr<AlloyBrowserHostImpl> browser =
          AlloyBrowserHostImpl::GetBrowserForContents(web_contents);
      if (browser.get()) {
        handled = true;

        blink::mojom::FileChooserParams params;
        params.mode = blink::mojom::FileChooserParams::Mode::kSave;
        if (!suggested_path.empty()) {
          params.default_file_name = suggested_path;
          if (!suggested_path.Extension().empty()) {
            params.accept_types.push_back(
                HoneycombString(suggested_path.Extension()));
          }
        }

        browser->RunFileChooserForBrowser(
            params,
            base::BindOnce(
                &HoneycombBeforeDownloadCallbackImpl::ChooseDownloadPathCallback,
                std::move(callback)));
      }
    }

    if (!handled) {
      std::move(callback).Run(
          suggested_path, DownloadItem::TARGET_DISPOSITION_OVERWRITE,
          download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
          download::DownloadItem::InsecureDownloadStatus::UNKNOWN,
          suggested_path, base::FilePath(), std::string() /*mime_type*/,
          download::DOWNLOAD_INTERRUPT_REASON_NONE);
    }
  }

  static void ChooseDownloadPathCallback(
      content::DownloadTargetCallback callback,
      const std::vector<base::FilePath>& file_paths) {
    DCHECK_LE(file_paths.size(), (size_t)1);

    base::FilePath path;
    if (file_paths.size() > 0) {
      path = file_paths.front();
    }

    // The download will be cancelled if |path| is empty.
    std::move(callback).Run(
        path, DownloadItem::TARGET_DISPOSITION_OVERWRITE,
        download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
        download::DownloadItem::InsecureDownloadStatus::UNKNOWN, path,
        base::FilePath(), std::string() /*mime_type*/,
        download::DOWNLOAD_INTERRUPT_REASON_NONE);
  }

  base::WeakPtr<DownloadManager> manager_;
  uint32_t download_id_;
  base::FilePath suggested_name_;
  content::DownloadTargetCallback callback_;

  IMPLEMENT_REFCOUNTING(HoneycombBeforeDownloadCallbackImpl);
};

// HoneycombDownloadItemCallback implementation.
class HoneycombDownloadItemCallbackImpl : public HoneycombDownloadItemCallback {
 public:
  explicit HoneycombDownloadItemCallbackImpl(
      const base::WeakPtr<DownloadManager>& manager,
      uint32_t download_id)
      : manager_(manager), download_id_(download_id) {}

  HoneycombDownloadItemCallbackImpl(const HoneycombDownloadItemCallbackImpl&) = delete;
  HoneycombDownloadItemCallbackImpl& operator=(const HoneycombDownloadItemCallbackImpl&) =
      delete;

  void Cancel() override {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombDownloadItemCallbackImpl::DoCancel, this));
  }

  void Pause() override {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombDownloadItemCallbackImpl::DoPause, this));
  }

  void Resume() override {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT,
                  base::BindOnce(&HoneycombDownloadItemCallbackImpl::DoResume, this));
  }

 private:
  void DoCancel() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      if (item && item->GetState() == DownloadItem::IN_PROGRESS) {
        item->Cancel(true);
      }
    }

    download_id_ = 0;
  }

  void DoPause() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      if (item && item->GetState() == DownloadItem::IN_PROGRESS) {
        item->Pause();
      }
    }
  }

  void DoResume() {
    if (download_id_ <= 0) {
      return;
    }

    if (manager_) {
      DownloadItem* item = manager_->GetDownload(download_id_);
      if (item && item->CanResume()) {
        item->Resume(true);
      }
    }
  }

  base::WeakPtr<DownloadManager> manager_;
  uint32_t download_id_;

  IMPLEMENT_REFCOUNTING(HoneycombDownloadItemCallbackImpl);
};

}  // namespace

HoneycombDownloadManagerDelegate::HoneycombDownloadManagerDelegate(DownloadManager* manager)
    : manager_(manager), manager_ptr_factory_(manager) {
  DCHECK(manager);
  manager->AddObserver(this);

  DownloadManager::DownloadVector items;
  manager->GetAllDownloads(&items);
  DownloadManager::DownloadVector::const_iterator it = items.begin();
  for (; it != items.end(); ++it) {
    OnDownloadCreated(manager, *it);
  }
}

HoneycombDownloadManagerDelegate::~HoneycombDownloadManagerDelegate() {
  if (manager_) {
    manager_->SetDelegate(nullptr);
    manager_->RemoveObserver(this);
  }

  while (!item_browser_map_.empty()) {
    OnDownloadDestroyed(item_browser_map_.begin()->first);
  }
}

void HoneycombDownloadManagerDelegate::OnDownloadUpdated(DownloadItem* download) {
  HoneycombRefPtr<AlloyBrowserHostImpl> browser = GetBrowser(download);
  HoneycombRefPtr<HoneycombDownloadHandler> handler;
  if (browser.get()) {
    handler = GetDownloadHandler(browser);
  }

  if (handler.get()) {
    HoneycombRefPtr<HoneycombDownloadItemImpl> download_item(
        new HoneycombDownloadItemImpl(download));
    HoneycombRefPtr<HoneycombDownloadItemCallback> callback(new HoneycombDownloadItemCallbackImpl(
        manager_ptr_factory_.GetWeakPtr(), download->GetId()));

    handler->OnDownloadUpdated(browser.get(), download_item.get(), callback);

    std::ignore = download_item->Detach(nullptr);
  }
}

void HoneycombDownloadManagerDelegate::OnDownloadDestroyed(DownloadItem* item) {
  item->RemoveObserver(this);

  AlloyBrowserHostImpl* browser = nullptr;

  ItemBrowserMap::iterator it = item_browser_map_.find(item);
  DCHECK(it != item_browser_map_.end());
  if (it != item_browser_map_.end()) {
    browser = it->second;
    item_browser_map_.erase(it);
  }

  if (browser) {
    // Determine if any remaining DownloadItems are associated with the same
    // browser. If not, then unregister as an observer.
    bool has_remaining = false;
    ItemBrowserMap::const_iterator it2 = item_browser_map_.begin();
    for (; it2 != item_browser_map_.end(); ++it2) {
      if (it2->second == browser) {
        has_remaining = true;
        break;
      }
    }

    if (!has_remaining) {
      browser->RemoveObserver(this);
    }
  }
}

void HoneycombDownloadManagerDelegate::OnDownloadCreated(DownloadManager* manager,
                                                   DownloadItem* item) {
  // This callback may arrive after DetermineDownloadTarget, so we allow
  // association from either method.
  HoneycombRefPtr<AlloyBrowserHostImpl> browser = GetOrAssociateBrowser(item);
  if (!browser) {
    // If the download is rejected (e.g. ALT+click on an invalid protocol link)
    // then an "interrupted" download will be started via DownloadManagerImpl::
    // StartDownloadWithId (originating from CreateInterruptedDownload) with no
    // associated WebContents and consequently no associated Honeycomb browser. In
    // that case DetermineDownloadTarget will be called before this method.
    // TODO(honey): Figure out how to expose this via a client callback.
    const std::vector<GURL>& url_chain = item->GetUrlChain();
    if (!url_chain.empty()) {
      LOG(INFO) << "Rejected download of " << url_chain.back().spec();
    }
    item->Cancel(true);
  }
}

void HoneycombDownloadManagerDelegate::ManagerGoingDown(DownloadManager* manager) {
  DCHECK_EQ(manager, manager_);
  manager->SetDelegate(nullptr);
  manager->RemoveObserver(this);
  manager_ptr_factory_.InvalidateWeakPtrs();
  manager_ = nullptr;
}

bool HoneycombDownloadManagerDelegate::DetermineDownloadTarget(
    DownloadItem* item,
    content::DownloadTargetCallback* callback) {
  if (!item->GetForcedFilePath().empty()) {
    std::move(*callback).Run(
        item->GetForcedFilePath(), DownloadItem::TARGET_DISPOSITION_OVERWRITE,
        download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
        download::DownloadItem::InsecureDownloadStatus::UNKNOWN,
        item->GetForcedFilePath(), base::FilePath(),
        std::string() /*mime_type*/, download::DOWNLOAD_INTERRUPT_REASON_NONE);
    return true;
  }

  // This callback may arrive before OnDownloadCreated, so we allow association
  // from either method.
  HoneycombRefPtr<AlloyBrowserHostImpl> browser = GetOrAssociateBrowser(item);
  HoneycombRefPtr<HoneycombDownloadHandler> handler;
  if (browser.get()) {
    handler = GetDownloadHandler(browser);
  }

  if (handler.get()) {
    base::FilePath suggested_name = net::GenerateFileName(
        item->GetURL(), item->GetContentDisposition(), std::string(),
        item->GetSuggestedFilename(), item->GetMimeType(), "download");

    HoneycombRefPtr<HoneycombDownloadItemImpl> download_item(new HoneycombDownloadItemImpl(item));
    HoneycombRefPtr<HoneycombBeforeDownloadCallback> callbackObj(
        new HoneycombBeforeDownloadCallbackImpl(manager_ptr_factory_.GetWeakPtr(),
                                          item->GetId(), suggested_name,
                                          std::move(*callback)));

    handler->OnBeforeDownload(browser.get(), download_item.get(),
                              suggested_name.value(), callbackObj);

    std::ignore = download_item->Detach(nullptr);
  }

  return true;
}

void HoneycombDownloadManagerDelegate::GetNextId(
    content::DownloadIdCallback callback) {
  static uint32_t next_id = DownloadItem::kInvalidId + 1;
  std::move(callback).Run(next_id++);
}

std::string HoneycombDownloadManagerDelegate::ApplicationClientIdForFileScanning() {
  return std::string(chrome::kApplicationClientIDStringForAVScanning);
}

void HoneycombDownloadManagerDelegate::OnBrowserDestroyed(
    HoneycombBrowserHostBase* browser) {
  ItemBrowserMap::iterator it = item_browser_map_.begin();
  for (; it != item_browser_map_.end(); ++it) {
    if (it->second == browser) {
      // Don't call back into browsers that have been destroyed. We're not
      // canceling the download so it will continue silently until it completes
      // or until the associated browser context is destroyed.
      it->second = nullptr;
    }
  }
}

AlloyBrowserHostImpl* HoneycombDownloadManagerDelegate::GetOrAssociateBrowser(
    download::DownloadItem* item) {
  ItemBrowserMap::const_iterator it = item_browser_map_.find(item);
  if (it != item_browser_map_.end()) {
    return it->second;
  }

  AlloyBrowserHostImpl* browser = nullptr;
  content::WebContents* contents =
      content::DownloadItemUtils::GetWebContents(item);
  if (contents) {
    browser = AlloyBrowserHostImpl::GetBrowserForContents(contents).get();
    DCHECK(browser);
  }
  if (!browser) {
    return nullptr;
  }

  item->AddObserver(this);

  item_browser_map_.insert(std::make_pair(item, browser));

  // Register as an observer so that we can cancel associated DownloadItems when
  // the browser is destroyed.
  if (!browser->HasObserver(this)) {
    browser->AddObserver(this);
  }

  return browser;
}

AlloyBrowserHostImpl* HoneycombDownloadManagerDelegate::GetBrowser(
    DownloadItem* item) {
  ItemBrowserMap::const_iterator it = item_browser_map_.find(item);
  if (it != item_browser_map_.end()) {
    return it->second;
  }

  // If the download is rejected (e.g. ALT+click on an invalid protocol link)
  // then an "interrupted" download will be started via DownloadManagerImpl::
  // StartDownloadWithId (originating from CreateInterruptedDownload) with no
  // associated WebContents and consequently no associated Honeycomb browser. In that
  // case DetermineDownloadTarget will be called before OnDownloadCreated.
  DCHECK(!content::DownloadItemUtils::GetWebContents(item));
  return nullptr;
}
