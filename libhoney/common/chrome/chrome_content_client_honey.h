// Copyright 2015 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_CONTENT_CLIENT_HONEYCOMB_H_
#define HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_CONTENT_CLIENT_HONEYCOMB_H_

#include "chrome/common/chrome_content_client.h"

class ChromeContentClientHoneycomb : public ChromeContentClient {
 public:
  ChromeContentClientHoneycomb();
  ~ChromeContentClientHoneycomb() override;

  // content::ContentClient overrides.
  void AddContentDecryptionModules(
      std::vector<content::CdmInfo>* cdms,
      std::vector<media::CdmHostFilePath>* cdm_host_file_paths) override;
  void AddAdditionalSchemes(Schemes* schemes) override;
};

#endif  // HONEYCOMB_LIBHONEY_COMMON_CHROME_CHROME_CONTENT_CLIENT_HONEYCOMB_H_
