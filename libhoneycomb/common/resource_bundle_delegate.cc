#include "libhoneycomb/common/resource_bundle_delegate.h"

#include "libhoneycomb/common/app_manager.h"

base::FilePath HoneycombResourceBundleDelegate::GetPathForResourcePack(
    const base::FilePath& pack_path,
    ui::ResourceScaleFactor scale_factor) {
  // Only allow the honey pack file to load.
  if (!pack_loading_disabled_ && allow_pack_file_load_) {
    return pack_path;
  }
  return base::FilePath();
}

base::FilePath HoneycombResourceBundleDelegate::GetPathForLocalePack(
    const base::FilePath& pack_path,
    const std::string& locale) {
  if (!pack_loading_disabled_) {
    return pack_path;
  }
  return base::FilePath();
}

gfx::Image HoneycombResourceBundleDelegate::GetImageNamed(int resource_id) {
  return gfx::Image();
}

gfx::Image HoneycombResourceBundleDelegate::GetNativeImageNamed(int resource_id) {
  return gfx::Image();
}

base::RefCountedMemory* HoneycombResourceBundleDelegate::LoadDataResourceBytes(
    int resource_id,
    ui::ResourceScaleFactor scale_factor) {
  return nullptr;
}

absl::optional<std::string> HoneycombResourceBundleDelegate::LoadDataResourceString(
    int resource_id) {
  return absl::nullopt;
}

bool HoneycombResourceBundleDelegate::GetRawDataResource(
    int resource_id,
    ui::ResourceScaleFactor scale_factor,
    base::StringPiece* value) const {
  auto application = HoneycombAppManager::Get()->GetApplication();
  if (application) {
    HoneycombRefPtr<HoneycombResourceBundleHandler> handler =
        application->GetResourceBundleHandler();
    if (handler.get()) {
      void* data = nullptr;
      size_t data_size = 0;
      if (scale_factor != ui::kScaleFactorNone) {
        if (handler->GetDataResourceForScale(
                resource_id, static_cast<honey_scale_factor_t>(scale_factor),
                data, data_size)) {
          *value = base::StringPiece(static_cast<char*>(data), data_size);
        }
      } else if (handler->GetDataResource(resource_id, data, data_size)) {
        *value = base::StringPiece(static_cast<char*>(data), data_size);
      }
    }
  }

  return (pack_loading_disabled_ || !value->empty());
}

bool HoneycombResourceBundleDelegate::GetLocalizedString(
    int message_id,
    std::u16string* value) const {
  auto application = HoneycombAppManager::Get()->GetApplication();
  if (application) {
    HoneycombRefPtr<HoneycombResourceBundleHandler> handler =
        application->GetResourceBundleHandler();
    if (handler.get()) {
      HoneycombString honey_str;
      if (handler->GetLocalizedString(message_id, honey_str)) {
        *value = honey_str;
      }
    }
  }

  return (pack_loading_disabled_ || !value->empty());
}
