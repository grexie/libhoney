// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_SCHEME_REGISTRAR_IMPL_H_
#define HONEYCOMB_LIBHONEY_COMMON_SCHEME_REGISTRAR_IMPL_H_
#pragma once

#include <string>
#include <vector>

#include "include/honey_scheme.h"

#include "content/public/common/content_client.h"

class HoneycombSchemeRegistrarImpl : public HoneycombSchemeRegistrar {
 public:
  HoneycombSchemeRegistrarImpl();

  HoneycombSchemeRegistrarImpl(const HoneycombSchemeRegistrarImpl&) = delete;
  HoneycombSchemeRegistrarImpl& operator=(const HoneycombSchemeRegistrarImpl&) = delete;

  // HoneycombSchemeRegistrar methods.
  bool AddCustomScheme(const HoneycombString& scheme_name, int options) override;

  void GetSchemes(content::ContentClient::Schemes* schemes);

 private:
  content::ContentClient::Schemes schemes_;
  std::set<std::string> registered_schemes_;
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_SCHEME_REGISTRAR_IMPL_H_
