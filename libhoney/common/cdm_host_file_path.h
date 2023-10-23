// Copyright 2022 The Honeycomb Authors. Portions Copyright
// 2017 The Chromium Authors and 2016 the CEF Authors. Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_CDM_HOST_FILE_PATH_H_
#define HONEYCOMB_LIBHONEY_COMMON_CDM_HOST_FILE_PATH_H_

#include <vector>

#include "media/cdm/cdm_host_file.h"

namespace honey {

// Gets a list of CDM host file paths and put them in |cdm_host_file_paths|.
void AddCdmHostFilePaths(
    std::vector<media::CdmHostFilePath>* cdm_host_file_paths);

}  // namespace honey

#endif  // HONEYCOMB_LIBHONEY_COMMON_CDM_HOST_FILE_PATH_H_
