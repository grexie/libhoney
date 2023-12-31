# Copyright (c) 2019 The Honeycomb Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

from __future__ import absolute_import
from honey_api_hash import honey_api_hash
from honey_parser import get_copyright
from file_util import *
import os
import sys


def make_api_hash_header(cpp_header_dir):
  # calculate api hashes
  api_hash_calculator = honey_api_hash(cpp_header_dir, verbose=False)
  api_hash = api_hash_calculator.calculate()

  result = get_copyright(full=True, translator=False) + \
"""//
// ---------------------------------------------------------------------------
//
// This file was generated by the make_api_hash_header.py tool.
//

#ifndef HONEYCOMB_INCLUDE_API_HASH_H_
#define HONEYCOMB_INCLUDE_API_HASH_H_

#include "include/internal/honey_export.h"

// The API hash is created by analyzing Honeycomb header files for C API type
// definitions. The hash value will change when header files are modified in a
// way that may cause binary incompatibility with other builds. The universal
// hash value will change if any platform is affected whereas the platform hash
// values will change only if that particular platform is affected.
#define HONEYCOMB_API_HASH_UNIVERSAL "$UNIVERSAL$"
#if defined(OS_WIN)
#define HONEYCOMB_API_HASH_PLATFORM "$WINDOWS$"
#elif defined(OS_MAC)
#define HONEYCOMB_API_HASH_PLATFORM "$MAC$"
#elif defined(OS_LINUX)
#define HONEYCOMB_API_HASH_PLATFORM "$LINUX$"
#endif

#ifdef __cplusplus
extern "C" {
#endif

///
// Returns Honeycomb API hashes for the libhoney library. The returned string is owned
// by the library and should not be freed. The |entry| parameter describes which
// hash value will be returned:
// 0 - HONEYCOMB_API_HASH_PLATFORM
// 1 - HONEYCOMB_API_HASH_UNIVERSAL
// 2 - HONEYCOMB_COMMIT_HASH (from honey_version.h)
///
HONEYCOMB_EXPORT const char* honey_api_hash(int entry);

#ifdef __cplusplus
}
#endif
#endif  // HONEYCOMB_INCLUDE_API_HASH_H_
"""

  # Substitute hash values for placeholders.
  for platform, value in api_hash.items():
    result = result.replace('$%s$' % platform.upper(), value)

  return result


def write_api_hash_header(output, cpp_header_dir):
  output = os.path.abspath(output)
  result = make_api_hash_header(cpp_header_dir)
  ret = write_file_if_changed(output, result)

  # Also write to |cpp_header_dir| if a different path from |output|, since we
  # need to commit the hash header for honey_version.py to correctly calculate the
  # version number based on git history.
  header_path = os.path.abspath(
      os.path.join(cpp_header_dir, os.path.basename(output)))
  if (output != header_path):
    write_file_if_changed(header_path, result)

  return ret


def main(argv):
  if len(argv) < 3:
    print(("Usage:\n  %s <output_filename> <cpp_header_dir>" % argv[0]))
    sys.exit(-1)
  write_api_hash_header(argv[1], argv[2])


if '__main__' == __name__:
  main(sys.argv)
