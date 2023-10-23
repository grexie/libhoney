// Copyright (c) 2011 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//

#include <cstddef>

#include "include/base/honey_build.h"
#include "include/honey_api_hash.h"
#include "include/honey_version.h"

#if defined(OS_WIN)
#include "include/internal/honey_win.h"
#endif

HONEYCOMB_EXPORT int honey_version_info(int entry) {
  switch (entry) {
    case 0:
      return HONEYCOMB_VERSION_MAJOR;
    case 1:
      return HONEYCOMB_VERSION_MINOR;
    case 2:
      return HONEYCOMB_VERSION_PATCH;
    case 3:
      return HONEYCOMB_COMMIT_NUMBER;
    case 4:
      return CHROME_VERSION_MAJOR;
    case 5:
      return CHROME_VERSION_MINOR;
    case 6:
      return CHROME_VERSION_BUILD;
    case 7:
      return CHROME_VERSION_PATCH;
    default:
      return 0;
  }
}

HONEYCOMB_EXPORT const char* honey_api_hash(int entry) {
  switch (entry) {
    case 0:
      return HONEYCOMB_API_HASH_PLATFORM;
    case 1:
      return HONEYCOMB_API_HASH_UNIVERSAL;
    case 2:
      return HONEYCOMB_COMMIT_HASH;
    default:
      return NULL;
  }
}

#if defined(OS_WIN)

#if defined(ARCH_CPU_32_BITS)
HONEYCOMB_EXPORT int honey_run_winmain_with_preferred_stack_size(wWinMainPtr wWinMain,
                                                         HINSTANCE hInstance,
                                                         LPWSTR lpCmdLine,
                                                         int nCmdShow) {
  return HoneycombRunWinMainWithPreferredStackSize(wWinMain, hInstance, lpCmdLine,
                                             nCmdShow);
}

HONEYCOMB_EXPORT int honey_run_main_with_preferred_stack_size(mainPtr main,
                                                      int argc,
                                                      char* argv[]) {
  return HoneycombRunMainWithPreferredStackSize(main, argc, argv);
}
#endif  // defined(ARCH_CPU_32_BITS)

HONEYCOMB_EXPORT void honey_set_osmodal_loop(int osModalLoop) {
  HoneycombSetOSModalLoop(osModalLoop ? true : false);
}

#endif  // defined(OS_WIN)
