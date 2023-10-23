// Copyright 2022 The Honeycomb Authors. Portions Copyright
// 2017 The Chromium Authors and 2016 the CEF Authors. Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "libhoneycomb/common/cdm_host_file_path.h"

#include "base/check.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/notreached.h"
#include "base/path_service.h"
#include "build/build_config.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_version.h"

#if BUILDFLAG(IS_MAC)
#include "libhoneycomb/common/util_mac.h"
#endif

namespace honey {

namespace {

// TODO(xhwang): Move this to a common place if needed.
const base::FilePath::CharType kSignatureFileExtension[] =
    FILE_PATH_LITERAL(".sig");

// Returns the signature file path given the |file_path|. This function should
// only be used when the signature file and the file are located in the same
// directory.
base::FilePath GetSigFilePath(const base::FilePath& file_path) {
  return file_path.AddExtension(kSignatureFileExtension);
}

bool FileExists(const base::FilePath& path) {
  return base::PathExists(path) && !base::DirectoryExists(path);
}

}  // namespace

void AddCdmHostFilePaths(
    std::vector<media::CdmHostFilePath>* cdm_host_file_paths) {
  DVLOG(1) << __func__;
  DCHECK(cdm_host_file_paths);
  DCHECK(cdm_host_file_paths->empty());

#if BUILDFLAG(IS_WIN)

  // Find the full path to the current executable.
  base::FilePath honey_exe;
  CHECK(base::PathService::Get(base::FILE_EXE, &honey_exe));
  const auto honey_exe_sig = GetSigFilePath(honey_exe);
  DVLOG(2) << __func__ << ": exe_path=" << honey_exe.value()
           << ", signature_path=" << honey_exe_sig.value();

  if (FileExists(honey_exe_sig)) {
    cdm_host_file_paths->emplace_back(honey_exe, honey_exe_sig);
  }

  // Find the full path to the module. This may be the same as the executable if
  // libhoneycomb is statically linked.
  base::FilePath honey_module;
  CHECK(base::PathService::Get(base::FILE_MODULE, &honey_module));
  if (honey_module != honey_exe) {
    const auto honey_module_sig = GetSigFilePath(honey_module);
    DVLOG(2) << __func__ << ": module_path=" << honey_module.value()
             << ", signature_path=" << honey_module_sig.value();

    if (FileExists(honey_module_sig)) {
      cdm_host_file_paths->emplace_back(honey_module, honey_module_sig);
    }
  }

#elif BUILDFLAG(IS_MAC)

  // Find the full path to the current executable.
  base::FilePath honey_exe;
  CHECK(base::PathService::Get(base::FILE_EXE, &honey_exe));

  // Find the sig file for the executable in the main Resources directory. This
  // directory may be empty if we're not bundled.
  const auto main_resources_path = util_mac::GetMainResourcesDirectory();
  if (!main_resources_path.empty()) {
    const auto exe_name = honey_exe.BaseName();
    const auto exe_sig_path =
        GetSigFilePath(main_resources_path.Append(exe_name));

    DVLOG(2) << __func__ << ": exe_path=" << honey_exe.value()
             << ", signature_path=" << exe_sig_path.value();

    if (FileExists(exe_sig_path)) {
      cdm_host_file_paths->emplace_back(honey_exe, exe_sig_path);
    }
  }

  // Find the sig file for the framework in the framework Resources directory.
  // This directory may be empty if we're not bundled.
  const auto framework_resources_path =
      util_mac::GetFrameworkResourcesDirectory();
  if (!framework_resources_path.empty()) {
    const auto framework_name = util_mac::GetFrameworkName();
    const auto framework_path =
        util_mac::GetFrameworkDirectory().Append(framework_name);
    const auto framework_sig_path =
        GetSigFilePath(framework_resources_path.Append(framework_name));

    DVLOG(2) << __func__ << ": framework_path=" << framework_path.value()
             << ", signature_path=" << framework_sig_path.value();

    if (FileExists(framework_sig_path)) {
      cdm_host_file_paths->emplace_back(framework_path, framework_sig_path);
    }
  }

#endif  // !BUILDFLAG(IS_MAC)
}

}  // namespace honey
