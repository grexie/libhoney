// Copyright (C) 2023 Grexie. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "libhoney/browser/views/layout_util.h"

#include <utility>

#include "libhoney/browser/views/layout_adapter.h"

#include "ui/views/view.h"

namespace layout_util {

namespace {

// Manages the association between views::View and HoneycombLayout instances.
class UserData : public base::SupportsUserData::Data {
 public:
  static HoneycombRefPtr<HoneycombLayout> GetFor(const views::View* view) {
    UserData* data = static_cast<UserData*>(view->GetUserData(UserDataKey()));
    if (data) {
      return data->layout_;
    }
    return nullptr;
  }

  // Assign ownership of the underlying views::LayoutManager to |owner_view|.
  // The views::View that owns the views::LayoutManager will gain a ref-counted
  // reference to the HoneycombLayout and the HoneycombLayout will keep an unowned reference
  // to the views::LayoutManager. Destruction of the views::View will release
  // the reference to the HoneycombLayout.
  static void Assign(HoneycombRefPtr<HoneycombLayout> honey_layout, views::View* owner_view) {
    DCHECK(owner_view);
    DCHECK(honey_layout->IsValid());

    views::LayoutManager* layout = HoneycombLayoutAdapter::GetFor(honey_layout)->Get();
    DCHECK(layout);

    // The HoneycombLayout previously associated with |owner_view|, if any, will be
    // destroyed by this call.
    owner_view->SetUserData(UserDataKey(),
                            base::WrapUnique(new UserData(honey_layout)));
  }

 private:
  friend std::default_delete<UserData>;

  explicit UserData(HoneycombRefPtr<HoneycombLayout> honey_layout) : layout_(honey_layout) {
    DCHECK(layout_);
  }

  ~UserData() override { HoneycombLayoutAdapter::GetFor(layout_)->Detach(); }

  static void* UserDataKey() {
    // We just need a unique constant. Use the address of a static that
    // COMDAT folding won't touch in an optimizing linker.
    static int data_key = 0;
    return reinterpret_cast<void*>(&data_key);
  }

  HoneycombRefPtr<HoneycombLayout> layout_;
};

}  // namespace

HoneycombRefPtr<HoneycombLayout> GetFor(const views::View* view) {
  return UserData::GetFor(view);
}

void Assign(HoneycombRefPtr<HoneycombLayout> layout, views::View* owner_view) {
  return UserData::Assign(layout, owner_view);
}

}  // namespace layout_util
