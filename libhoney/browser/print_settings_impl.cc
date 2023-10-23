// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoney/browser/print_settings_impl.h"

#include "base/logging.h"

#include "printing/mojom/print.mojom.h"

HoneycombPrintSettingsImpl::HoneycombPrintSettingsImpl(
    std::unique_ptr<printing::PrintSettings> settings,
    bool read_only)
    : HoneycombValueBase<HoneycombPrintSettings, printing::PrintSettings>(
          settings.release(),
          nullptr,
          kOwnerWillDelete,
          read_only,
          nullptr) {}

bool HoneycombPrintSettingsImpl::IsValid() {
  return !detached();
}

bool HoneycombPrintSettingsImpl::IsReadOnly() {
  return read_only();
}

void HoneycombPrintSettingsImpl::SetOrientation(bool landscape) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->SetOrientation(landscape);
}

bool HoneycombPrintSettingsImpl::IsLandscape() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().landscape();
}

void HoneycombPrintSettingsImpl::SetPrinterPrintableArea(
    const HoneycombSize& physical_size_device_units,
    const HoneycombRect& printable_area_device_units,
    bool landscape_needs_flip) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  gfx::Size size(physical_size_device_units.width,
                 physical_size_device_units.height);
  gfx::Rect rect(printable_area_device_units.x, printable_area_device_units.y,
                 printable_area_device_units.width,
                 printable_area_device_units.height);
  mutable_value()->SetPrinterPrintableArea(size, rect, landscape_needs_flip);
}

void HoneycombPrintSettingsImpl::SetDeviceName(const HoneycombString& name) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_device_name(name.ToString16());
}

HoneycombString HoneycombPrintSettingsImpl::GetDeviceName() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, HoneycombString());
  return const_value().device_name();
}

void HoneycombPrintSettingsImpl::SetDPI(int dpi) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_dpi(dpi);
}

int HoneycombPrintSettingsImpl::GetDPI() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().dpi();
}

void HoneycombPrintSettingsImpl::SetPageRanges(const PageRangeList& ranges) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  printing::PageRanges page_ranges;
  PageRangeList::const_iterator it = ranges.begin();
  for (; it != ranges.end(); ++it) {
    const HoneycombRange& honey_range = *it;
    printing::PageRange range;
    range.from = honey_range.from;
    range.to = honey_range.to;
    page_ranges.push_back(range);
  }
  mutable_value()->set_ranges(page_ranges);
}

size_t HoneycombPrintSettingsImpl::GetPageRangesCount() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, 0);
  return const_value().ranges().size();
}

void HoneycombPrintSettingsImpl::GetPageRanges(PageRangeList& ranges) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(false);
  if (!ranges.empty()) {
    ranges.clear();
  }
  const printing::PageRanges& page_ranges = const_value().ranges();
  printing::PageRanges::const_iterator it = page_ranges.begin();
  for (; it != page_ranges.end(); ++it) {
    const printing::PageRange& range = *it;
    ranges.push_back(HoneycombRange(range.from, range.to));
  }
}

void HoneycombPrintSettingsImpl::SetSelectionOnly(bool selection_only) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_selection_only(selection_only);
}

bool HoneycombPrintSettingsImpl::IsSelectionOnly() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().selection_only();
}

void HoneycombPrintSettingsImpl::SetCollate(bool collate) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_collate(collate);
}

bool HoneycombPrintSettingsImpl::WillCollate() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().collate();
}

void HoneycombPrintSettingsImpl::SetColorModel(ColorModel model) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_color(static_cast<printing::mojom::ColorModel>(model));
}

HoneycombPrintSettings::ColorModel HoneycombPrintSettingsImpl::GetColorModel() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, COLOR_MODEL_UNKNOWN);
  return static_cast<ColorModel>(const_value().color());
}

void HoneycombPrintSettingsImpl::SetCopies(int copies) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_copies(copies);
}

int HoneycombPrintSettingsImpl::GetCopies() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, false);
  return const_value().copies();
}

void HoneycombPrintSettingsImpl::SetDuplexMode(DuplexMode mode) {
  HONEYCOMB_VALUE_VERIFY_RETURN_VOID(true);
  mutable_value()->set_duplex_mode(
      static_cast<printing::mojom::DuplexMode>(mode));
}

HoneycombPrintSettings::DuplexMode HoneycombPrintSettingsImpl::GetDuplexMode() {
  HONEYCOMB_VALUE_VERIFY_RETURN(false, DUPLEX_MODE_UNKNOWN);
  return static_cast<DuplexMode>(const_value().duplex_mode());
}

std::unique_ptr<printing::PrintSettings> HoneycombPrintSettingsImpl::TakeOwnership() {
  return base::WrapUnique(Detach(nullptr));
}

// HoneycombPrintSettings implementation.

// static
HoneycombRefPtr<HoneycombPrintSettings> HoneycombPrintSettings::Create() {
  return new HoneycombPrintSettingsImpl(std::make_unique<printing::PrintSettings>(),
                                  false);
}
