// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libhoneycomb/common/chrome/chrome_content_client_honey.h"

#include "libhoneycomb/common/app_manager.h"

#include "chrome/common/media/cdm_registration.h"

#if BUILDFLAG(ENABLE_CDM_HOST_VERIFICATION)
#include "libhoneycomb/common/cdm_host_file_path.h"
#endif

ChromeContentClientHoneycomb::ChromeContentClientHoneycomb() = default;
ChromeContentClientHoneycomb::~ChromeContentClientHoneycomb() = default;

void ChromeContentClientHoneycomb::AddContentDecryptionModules(
    std::vector<content::CdmInfo>* cdms,
    std::vector<media::CdmHostFilePath>* cdm_host_file_paths) {
  if (cdms) {
    RegisterCdmInfo(cdms);
  }

#if BUILDFLAG(ENABLE_CDM_HOST_VERIFICATION)
  if (cdm_host_file_paths) {
    honey::AddCdmHostFilePaths(cdm_host_file_paths);
  }
#endif
}

void ChromeContentClientHoneycomb::AddAdditionalSchemes(Schemes* schemes) {
  ChromeContentClient::AddAdditionalSchemes(schemes);
  HoneycombAppManager::Get()->AddAdditionalSchemes(schemes);
}
