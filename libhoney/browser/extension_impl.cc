// Copyright (c) 2017 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/extension_impl.h"

#include "libhoney/browser/request_context_impl.h"
#include "libhoney/browser/thread_util.h"
#include "libhoney/common/values_impl.h"

#include "extensions/common/extension.h"

HoneycombExtensionImpl::HoneycombExtensionImpl(const extensions::Extension* extension,
                                   HoneycombRequestContext* loader_context,
                                   HoneycombRefPtr<HoneycombExtensionHandler> handler)
    : id_(extension->id()),
      path_(extension->path().value()),
      manifest_(
          new HoneycombDictionaryValueImpl(extension->manifest()->value()->Clone(),
                                     /*read_only=*/true)),
      loader_context_(loader_context),
      handler_(handler) {}

HoneycombString HoneycombExtensionImpl::GetIdentifier() {
  return id_;
}

HoneycombString HoneycombExtensionImpl::GetPath() {
  return path_;
}

HoneycombRefPtr<HoneycombDictionaryValue> HoneycombExtensionImpl::GetManifest() {
  return manifest_;
}

bool HoneycombExtensionImpl::IsSame(HoneycombRefPtr<HoneycombExtension> that) {
  HoneycombExtensionImpl* that_impl = static_cast<HoneycombExtensionImpl*>(that.get());
  if (!that_impl) {
    return false;
  }

  // Maybe the same object.
  if (this == that_impl) {
    return true;
  }

  return id_ == that_impl->id_ && path_ == that_impl->path_ &&
         loader_context_ == that_impl->loader_context_;
}

HoneycombRefPtr<HoneycombExtensionHandler> HoneycombExtensionImpl::GetHandler() {
  return handler_;
}

HoneycombRefPtr<HoneycombRequestContext> HoneycombExtensionImpl::GetLoaderContext() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return nullptr;
  }

  return loader_context_;
}

bool HoneycombExtensionImpl::IsLoaded() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    DCHECK(false) << "called on invalid thread";
    return false;
  }

  return !unloaded_;
}

void HoneycombExtensionImpl::Unload() {
  if (!HONEYCOMB_CURRENTLY_ON_UIT()) {
    HONEYCOMB_POST_TASK(HONEYCOMB_UIT, base::BindOnce(&HoneycombExtensionImpl::Unload, this));
    return;
  }

  // Will be NULL for internal extensions. They can't be unloaded.
  if (!loader_context_) {
    return;
  }

  if (unloaded_) {
    return;
  }

  // HoneycombExtensionHandler callbacks triggered by UnloadExtension may check this
  // flag, so set it here.
  unloaded_ = true;

  [[maybe_unused]] const bool result =
      static_cast<HoneycombRequestContextImpl*>(loader_context_)
          ->GetBrowserContext()
          ->UnloadExtension(id_);
  DCHECK(result);
}

void HoneycombExtensionImpl::OnExtensionLoaded() {
  HONEYCOMB_REQUIRE_UIT();
  if (handler_) {
    handler_->OnExtensionLoaded(this);
  }
}

void HoneycombExtensionImpl::OnExtensionUnloaded() {
  HONEYCOMB_REQUIRE_UIT();
  // Should not be called for internal extensions.
  DCHECK(loader_context_);

  unloaded_ = true;
  loader_context_ = nullptr;

  if (handler_) {
    handler_->OnExtensionUnloaded(this);
  }
}
