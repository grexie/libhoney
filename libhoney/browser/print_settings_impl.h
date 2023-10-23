// Copyright (c) 2014 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_PRINT_SETTINGS_IMPL_H_
#define HONEYCOMB_LIBHONEY_BROWSER_PRINT_SETTINGS_IMPL_H_
#pragma once

#include "include/honey_print_settings.h"
#include "libhoney/common/value_base.h"

#include "printing/print_settings.h"

// HoneycombPrintSettings implementation
class HoneycombPrintSettingsImpl
    : public HoneycombValueBase<HoneycombPrintSettings, printing::PrintSettings> {
 public:
  HoneycombPrintSettingsImpl(std::unique_ptr<printing::PrintSettings> settings,
                       bool read_only);

  HoneycombPrintSettingsImpl(const HoneycombPrintSettingsImpl&) = delete;
  HoneycombPrintSettingsImpl& operator=(const HoneycombPrintSettingsImpl&) = delete;

  // HoneycombPrintSettings methods.
  bool IsValid() override;
  bool IsReadOnly() override;
  void SetOrientation(bool landscape) override;
  bool IsLandscape() override;
  void SetPrinterPrintableArea(const HoneycombSize& physical_size_device_units,
                               const HoneycombRect& printable_area_device_units,
                               bool landscape_needs_flip) override;
  void SetDeviceName(const HoneycombString& name) override;
  HoneycombString GetDeviceName() override;
  void SetDPI(int dpi) override;
  int GetDPI() override;
  void SetPageRanges(const PageRangeList& ranges) override;
  size_t GetPageRangesCount() override;
  void GetPageRanges(PageRangeList& ranges) override;
  void SetSelectionOnly(bool selection_only) override;
  bool IsSelectionOnly() override;
  void SetCollate(bool collate) override;
  bool WillCollate() override;
  void SetColorModel(ColorModel model) override;
  ColorModel GetColorModel() override;
  void SetCopies(int copies) override;
  int GetCopies() override;
  void SetDuplexMode(DuplexMode mode) override;
  DuplexMode GetDuplexMode() override;

  [[nodiscard]] std::unique_ptr<printing::PrintSettings> TakeOwnership();
};

#endif  // HONEYCOMB_LIBHONEY_BROWSER_PRINT_SETTINGS_IMPL_H_
