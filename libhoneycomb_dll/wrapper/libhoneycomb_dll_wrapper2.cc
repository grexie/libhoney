// Copyright (c) 2022 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/base/honey_build.h"

#if defined(OS_WIN)

#include "include/base/honey_compiler_specific.h"
#include "include/base/honey_logging.h"
#include "include/honey_api_hash.h"
#include "include/internal/honey_win.h"

#if defined(ARCH_CPU_32_BITS)
NO_SANITIZE("cfi-icall")
int HoneycombRunWinMainWithPreferredStackSize(wWinMainPtr wWinMain,
                                        HINSTANCE hInstance,
                                        LPWSTR lpCmdLine,
                                        int nCmdShow) {
  CHECK(wWinMain && hInstance);

  const char* api_hash = honey_api_hash(0);
  if (strcmp(api_hash, HONEYCOMB_API_HASH_PLATFORM)) {
    // The libhoneycomb API hash does not match the current header API hash.
    DCHECK(false);
    return 0;
  }

  return honey_run_winmain_with_preferred_stack_size(wWinMain, hInstance,
                                                   lpCmdLine, nCmdShow);
}

int HoneycombRunMainWithPreferredStackSize(mainPtr main, int argc, char* argv[]) {
  CHECK(main);

  const char* api_hash = honey_api_hash(0);
  if (strcmp(api_hash, HONEYCOMB_API_HASH_PLATFORM)) {
    // The libhoneycomb API hash does not match the current header API hash.
    DCHECK(false);
    return 0;
  }

  return honey_run_main_with_preferred_stack_size(main, argc, argv);
}
#endif  // defined(ARCH_CPU_32_BITS)

NO_SANITIZE("cfi-icall") void HoneycombSetOSModalLoop(bool osModalLoop) {
  honey_set_osmodal_loop(osModalLoop);
}

#endif  // defined(OS_WIN)
