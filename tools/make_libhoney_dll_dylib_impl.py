# Copyright (c) 2018 The Honeycomb Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

from __future__ import absolute_import
from honey_parser import *
from file_util import *
import os

# Other headers that export C API functions.
OTHER_HEADERS = [
    'honey_api_hash.h',
    'honey_version.h',
    'internal/honey_logging_internal.h',
    'internal/honey_string_list.h',
    'internal/honey_string_map.h',
    'internal/honey_string_multimap.h',
    'internal/honey_string_types.h',
    'internal/honey_thread_internal.h',
    'internal/honey_time.h',
    'internal/honey_trace_event_internal.h',
]


def make_libhoney_dll_dylib_impl_parts(name, retval, args):
  # Split arguments into types and names.
  arg_types = ''
  arg_names = ''
  for arg in args:
    if len(arg_types) > 0:
      arg_types += ', '
      arg_names += ', '
    pos = arg.rfind(' ')
    arg_types += arg[0:pos]
    arg_names += arg[pos + 1:]

  declare = 'decltype(&%s) %s;\n' % (name, name)

  init = '  INIT_ENTRY(%s);' % name

  impl = """NO_SANITIZE("cfi-icall") %s %s(%s) {
  %sg_libhoney_pointers.%s(%s);
}

""" % (retval, name, ', '.join(args), 'return '
       if retval != 'void' else '', name, arg_names)

  return (declare, init, impl)


def make_libhoney_dll_dylib_impl_func(func):
  name = func.get_capi_name()
  parts = func.get_capi_parts([], True)
  retval = parts['retval']
  args = parts['args']
  return make_libhoney_dll_dylib_impl_parts(name, retval, args)


def make_libhoney_dll_dylib_impl(header):
  filenames = []
  includes = [
      '#include "include/base/honey_compiler_specific.h"',
      '#include "include/wrapper/honey_library_loader.h"',
  ]
  ptr_declare = ''
  ptr_init = ''
  ptr_impl = ''

  # Include required headers for global functions.
  for func in header.get_funcs():
    declare, init, impl = make_libhoney_dll_dylib_impl_func(func)
    ptr_declare += declare
    ptr_init += init
    ptr_impl += impl

    filename = func.get_file_name()
    if not filename in filenames:
      includes.append('#include "include/capi/%s"' % func.get_capi_file_name())
      filenames.append(filename)

  # Include required headers for static class functions.
  allclasses = header.get_classes()
  for cls in allclasses:
    funcs = cls.get_static_funcs()
    for func in funcs:
      declare, init, impl = make_libhoney_dll_dylib_impl_func(func)
      ptr_declare += declare
      ptr_init += init
      ptr_impl += impl

    if len(funcs) > 0:
      filename = cls.get_file_name()
      if not filename in filenames:
        includes.append('#include "include/capi/%s"' % cls.get_capi_file_name())
        filenames.append(filename)

  # Parse other headers.
  root_directory = header.get_root_directory()
  for other in OTHER_HEADERS:
    path = os.path.join(root_directory, other)
    content = read_file(path)
    funcs = get_function_impls(content, 'HONEYCOMB_EXPORT', False)
    for func in funcs:
      declare, init, impl = make_libhoney_dll_dylib_impl_parts(
          func['name'], func['retval'], func['args'])
      ptr_declare += declare
      ptr_init += init
      ptr_impl += impl

    includes.append('#include "include/%s"' % other)

  # Build the final output.
  result = get_copyright() + """

#include <dlfcn.h>
#include <stdio.h>

""" + "\n".join(sorted(includes)) + """

// GLOBAL WRAPPER FUNCTIONS - Do not edit by hand.

namespace {

void* g_libhoney_handle = nullptr;

void* libhoney_get_ptr(const char* path, const char* name) {
  void* ptr = dlsym(g_libhoney_handle, name);
  if (!ptr) {
    fprintf(stderr, "dlsym %s: %s\\n", path, dlerror());
  }
  return ptr;
}

struct libhoney_pointers {
""" + ptr_declare + """
} g_libhoney_pointers = {0};

#define INIT_ENTRY(name) \
  g_libhoney_pointers.name = (decltype(&name))libhoney_get_ptr(path, #name); \
  if (!g_libhoney_pointers.name) { \
    return 0; \
  }

int libhoney_init_pointers(const char* path) {
""" + ptr_init + """
  return 1;
}

}  // namespace

int honey_load_library(const char* path) {
  if (g_libhoney_handle)
    return 0;

  g_libhoney_handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL | RTLD_FIRST);
  if (!g_libhoney_handle) {
    fprintf(stderr, "dlopen %s: %s\\n", path, dlerror());
    return 0;
  }

  if (!libhoney_init_pointers(path)) {
    honey_unload_library();
    return 0;
  }

  return 1;
}

int honey_unload_library() {
  int result = 0;
  if (g_libhoney_handle) {
    result = !dlclose(g_libhoney_handle);
    if (!result) {
      fprintf(stderr, "dlclose: %s\\n", dlerror());
    }
    g_libhoney_handle = nullptr;
  }
  return result;
}

""" + ptr_impl
  return result


def write_libhoney_dll_dylib_impl(header, file):
  newcontents = make_libhoney_dll_dylib_impl(header)
  return (file, newcontents)


# Test the module.
if __name__ == "__main__":
  import sys

  # Verify that the correct number of command-line arguments are provided.
  if len(sys.argv) < 2:
    sys.stderr.write('Usage: ' + sys.argv[0] + ' <cpp_header_dir>\n')
    sys.exit()

  cpp_header_dir = sys.argv[1]

  # Create the header object. Should match the logic in translator.py.
  header = obj_header()
  header.set_root_directory(cpp_header_dir)
  excluded_files = ['honey_api_hash.h', 'honey_application_mac.h', 'honey_version.h']
  header.add_directory(cpp_header_dir, excluded_files)
  header.add_directory(os.path.join(cpp_header_dir, 'test'))
  header.add_directory(os.path.join(cpp_header_dir, 'views'))

  # Dump the result to stdout.
  sys.stdout.write(make_libhoney_dll_dylib_impl(header))
