// Copyright (c) 2023 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the Honeycomb translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=9d44fae857002c47910b25efb8e471ed453d2b55$
//

#include "include/capi/honey_app_capi.h"
#include "include/capi/honey_crash_util_capi.h"
#include "include/capi/honey_file_util_capi.h"
#include "include/capi/honey_i18n_util_capi.h"
#include "include/capi/honey_origin_whitelist_capi.h"
#include "include/capi/honey_parser_capi.h"
#include "include/capi/honey_path_util_capi.h"
#include "include/capi/honey_process_util_capi.h"
#include "include/capi/honey_scheme_capi.h"
#include "include/capi/honey_ssl_info_capi.h"
#include "include/capi/honey_task_capi.h"
#include "include/capi/honey_trace_capi.h"
#include "include/capi/honey_v8_capi.h"
#include "include/capi/test/honey_test_helpers_capi.h"
#include "include/honey_app.h"
#include "include/honey_crash_util.h"
#include "include/honey_file_util.h"
#include "include/honey_i18n_util.h"
#include "include/honey_origin_whitelist.h"
#include "include/honey_parser.h"
#include "include/honey_path_util.h"
#include "include/honey_process_util.h"
#include "include/honey_scheme.h"
#include "include/honey_ssl_info.h"
#include "include/honey_task.h"
#include "include/honey_trace.h"
#include "include/honey_v8.h"
#include "include/test/honey_test_helpers.h"
#include "libhoney_dll/cpptoc/binary_value_cpptoc.h"
#include "libhoney_dll/cpptoc/command_line_cpptoc.h"
#include "libhoney_dll/cpptoc/frame_cpptoc.h"
#include "libhoney_dll/cpptoc/value_cpptoc.h"
#include "libhoney_dll/ctocpp/app_ctocpp.h"
#include "libhoney_dll/ctocpp/completion_callback_ctocpp.h"
#include "libhoney_dll/ctocpp/end_tracing_callback_ctocpp.h"
#include "libhoney_dll/ctocpp/scheme_handler_factory_ctocpp.h"
#include "libhoney_dll/ctocpp/task_ctocpp.h"
#include "libhoney_dll/ctocpp/v8handler_ctocpp.h"
#include "libhoney_dll/shutdown_checker.h"
#include "libhoney_dll/template_util.h"
#include "libhoney_dll/transfer_util.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

HONEYCOMB_EXPORT int honey_execute_process(const honey_main_args_t* args,
                                   struct _honey_app_t* application,
                                   void* windows_sandbox_info) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: args; type: simple_byref_const
  DCHECK(args);
  if (!args) {
    return 0;
  }
  // Unverified params: application, windows_sandbox_info

  // Translate param: args; type: simple_byref_const
  HoneycombMainArgs argsVal = args ? *args : HoneycombMainArgs();

  // Execute
  int _retval = HoneycombExecuteProcess(argsVal, HoneycombAppCToCpp::Wrap(application),
                                  windows_sandbox_info);

  // Return type: simple
  return _retval;
}

HONEYCOMB_EXPORT int honey_initialize(const honey_main_args_t* args,
                              const struct _honey_settings_t* settings,
                              struct _honey_app_t* application,
                              void* windows_sandbox_info) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: args; type: simple_byref_const
  DCHECK(args);
  if (!args) {
    return 0;
  }
  // Verify param: settings; type: struct_byref_const
  DCHECK(settings);
  if (!settings) {
    return 0;
  }
  if (!template_util::has_valid_size(settings)) {
    DCHECK(false) << "invalid settings->[base.]size";
    return 0;
  }
  // Unverified params: application, windows_sandbox_info

  // Translate param: args; type: simple_byref_const
  HoneycombMainArgs argsVal = args ? *args : HoneycombMainArgs();
  // Translate param: settings; type: struct_byref_const
  HoneycombSettings settingsObj;
  if (settings) {
    settingsObj.Set(*settings, false);
  }

  // Execute
  bool _retval =
      HoneycombInitialize(argsVal, settingsObj, HoneycombAppCToCpp::Wrap(application),
                    windows_sandbox_info);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT void honey_shutdown() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

#if DCHECK_IS_ON()
  shutdown_checker::SetIsShutdown();
#endif

  // Execute
  HoneycombShutdown();
}

HONEYCOMB_EXPORT void honey_do_message_loop_work() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  HoneycombDoMessageLoopWork();
}

HONEYCOMB_EXPORT void honey_run_message_loop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  HoneycombRunMessageLoop();
}

HONEYCOMB_EXPORT void honey_quit_message_loop() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  HoneycombQuitMessageLoop();
}

HONEYCOMB_EXPORT int honey_crash_reporting_enabled() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombCrashReportingEnabled();

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT void honey_set_crash_key_value(const honey_string_t* key,
                                        const honey_string_t* value) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: key; type: string_byref_const
  DCHECK(key);
  if (!key) {
    return;
  }
  // Unverified params: value

  // Execute
  HoneycombSetCrashKeyValue(HoneycombString(key), HoneycombString(value));
}

HONEYCOMB_EXPORT int honey_create_directory(const honey_string_t* full_path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: full_path; type: string_byref_const
  DCHECK(full_path);
  if (!full_path) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombCreateDirectory(HoneycombString(full_path));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_get_temp_directory(honey_string_t* temp_dir) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: temp_dir; type: string_byref
  DCHECK(temp_dir);
  if (!temp_dir) {
    return 0;
  }

  // Translate param: temp_dir; type: string_byref
  HoneycombString temp_dirStr(temp_dir);

  // Execute
  bool _retval = HoneycombGetTempDirectory(temp_dirStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_create_new_temp_directory(const honey_string_t* prefix,
                                             honey_string_t* new_temp_path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: new_temp_path; type: string_byref
  DCHECK(new_temp_path);
  if (!new_temp_path) {
    return 0;
  }
  // Unverified params: prefix

  // Translate param: new_temp_path; type: string_byref
  HoneycombString new_temp_pathStr(new_temp_path);

  // Execute
  bool _retval = HoneycombCreateNewTempDirectory(HoneycombString(prefix), new_temp_pathStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_create_temp_directory_in_directory(
    const honey_string_t* base_dir,
    const honey_string_t* prefix,
    honey_string_t* new_dir) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: base_dir; type: string_byref_const
  DCHECK(base_dir);
  if (!base_dir) {
    return 0;
  }
  // Verify param: new_dir; type: string_byref
  DCHECK(new_dir);
  if (!new_dir) {
    return 0;
  }
  // Unverified params: prefix

  // Translate param: new_dir; type: string_byref
  HoneycombString new_dirStr(new_dir);

  // Execute
  bool _retval = HoneycombCreateTempDirectoryInDirectory(
      HoneycombString(base_dir), HoneycombString(prefix), new_dirStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_directory_exists(const honey_string_t* path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(path);
  if (!path) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombDirectoryExists(HoneycombString(path));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_delete_file(const honey_string_t* path, int recursive) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(path);
  if (!path) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombDeleteFile(HoneycombString(path), recursive ? true : false);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_zip_directory(const honey_string_t* src_dir,
                                 const honey_string_t* dest_file,
                                 int include_hidden_files) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: src_dir; type: string_byref_const
  DCHECK(src_dir);
  if (!src_dir) {
    return 0;
  }
  // Verify param: dest_file; type: string_byref_const
  DCHECK(dest_file);
  if (!dest_file) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombZipDirectory(HoneycombString(src_dir), HoneycombString(dest_file),
                                 include_hidden_files ? true : false);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT void honey_load_crlsets_file(const honey_string_t* path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref_const
  DCHECK(path);
  if (!path) {
    return;
  }

  // Execute
  HoneycombLoadCRLSetsFile(HoneycombString(path));
}

HONEYCOMB_EXPORT int honey_is_rtl() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombIsRTL();

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_add_cross_origin_whitelist_entry(
    const honey_string_t* source_origin,
    const honey_string_t* target_protocol,
    const honey_string_t* target_domain,
    int allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(source_origin);
  if (!source_origin) {
    return 0;
  }
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(target_protocol);
  if (!target_protocol) {
    return 0;
  }
  // Unverified params: target_domain

  // Execute
  bool _retval = HoneycombAddCrossOriginWhitelistEntry(
      HoneycombString(source_origin), HoneycombString(target_protocol),
      HoneycombString(target_domain), allow_target_subdomains ? true : false);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_remove_cross_origin_whitelist_entry(
    const honey_string_t* source_origin,
    const honey_string_t* target_protocol,
    const honey_string_t* target_domain,
    int allow_target_subdomains) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: source_origin; type: string_byref_const
  DCHECK(source_origin);
  if (!source_origin) {
    return 0;
  }
  // Verify param: target_protocol; type: string_byref_const
  DCHECK(target_protocol);
  if (!target_protocol) {
    return 0;
  }
  // Unverified params: target_domain

  // Execute
  bool _retval = HoneycombRemoveCrossOriginWhitelistEntry(
      HoneycombString(source_origin), HoneycombString(target_protocol),
      HoneycombString(target_domain), allow_target_subdomains ? true : false);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_clear_cross_origin_whitelist() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombClearCrossOriginWhitelist();

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_resolve_url(const honey_string_t* base_url,
                               const honey_string_t* relative_url,
                               honey_string_t* resolved_url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: base_url; type: string_byref_const
  DCHECK(base_url);
  if (!base_url) {
    return 0;
  }
  // Verify param: relative_url; type: string_byref_const
  DCHECK(relative_url);
  if (!relative_url) {
    return 0;
  }
  // Verify param: resolved_url; type: string_byref
  DCHECK(resolved_url);
  if (!resolved_url) {
    return 0;
  }

  // Translate param: resolved_url; type: string_byref
  HoneycombString resolved_urlStr(resolved_url);

  // Execute
  bool _retval = HoneycombResolveURL(HoneycombString(base_url), HoneycombString(relative_url),
                               resolved_urlStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_parse_url(const honey_string_t* url,
                             struct _honey_urlparts_t* parts) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: url; type: string_byref_const
  DCHECK(url);
  if (!url) {
    return 0;
  }
  // Verify param: parts; type: struct_byref
  DCHECK(parts);
  if (!parts) {
    return 0;
  }
  if (!template_util::has_valid_size(parts)) {
    DCHECK(false) << "invalid parts->[base.]size";
    return 0;
  }

  // Translate param: parts; type: struct_byref
  HoneycombURLParts partsObj;
  if (parts) {
    partsObj.AttachTo(*parts);
  }

  // Execute
  bool _retval = HoneycombParseURL(HoneycombString(url), partsObj);

  // Restore param: parts; type: struct_byref
  if (parts) {
    partsObj.DetachTo(*parts);
  }

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_create_url(const struct _honey_urlparts_t* parts,
                              honey_string_t* url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: parts; type: struct_byref_const
  DCHECK(parts);
  if (!parts) {
    return 0;
  }
  if (!template_util::has_valid_size(parts)) {
    DCHECK(false) << "invalid parts->[base.]size";
    return 0;
  }
  // Verify param: url; type: string_byref
  DCHECK(url);
  if (!url) {
    return 0;
  }

  // Translate param: parts; type: struct_byref_const
  HoneycombURLParts partsObj;
  if (parts) {
    partsObj.Set(*parts, false);
  }
  // Translate param: url; type: string_byref
  HoneycombString urlStr(url);

  // Execute
  bool _retval = HoneycombCreateURL(partsObj, urlStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT honey_string_userfree_t
honey_format_url_for_security_display(const honey_string_t* origin_url) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: origin_url; type: string_byref_const
  DCHECK(origin_url);
  if (!origin_url) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombFormatUrlForSecurityDisplay(HoneycombString(origin_url));

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT honey_string_userfree_t
honey_get_mime_type(const honey_string_t* extension) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: extension; type: string_byref_const
  DCHECK(extension);
  if (!extension) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombGetMimeType(HoneycombString(extension));

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT void honey_get_extensions_for_mime_type(const honey_string_t* mime_type,
                                                 honey_string_list_t extensions) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: mime_type; type: string_byref_const
  DCHECK(mime_type);
  if (!mime_type) {
    return;
  }
  // Verify param: extensions; type: string_vec_byref
  DCHECK(extensions);
  if (!extensions) {
    return;
  }

  // Translate param: extensions; type: string_vec_byref
  std::vector<HoneycombString> extensionsList;
  transfer_string_list_contents(extensions, extensionsList);

  // Execute
  HoneycombGetExtensionsForMimeType(HoneycombString(mime_type), extensionsList);

  // Restore param: extensions; type: string_vec_byref
  honey_string_list_clear(extensions);
  transfer_string_list_contents(extensionsList, extensions);
}

HONEYCOMB_EXPORT honey_string_userfree_t honey_base64encode(const void* data,
                                                  size_t data_size) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombBase64Encode(data, data_size);

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT struct _honey_binary_value_t* honey_base64decode(
    const honey_string_t* data) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: string_byref_const
  DCHECK(data);
  if (!data) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombBinaryValue> _retval = HoneycombBase64Decode(HoneycombString(data));

  // Return type: refptr_same
  return HoneycombBinaryValueCppToC::Wrap(_retval);
}

HONEYCOMB_EXPORT honey_string_userfree_t honey_uriencode(const honey_string_t* text,
                                               int use_plus) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: text; type: string_byref_const
  DCHECK(text);
  if (!text) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombURIEncode(HoneycombString(text), use_plus ? true : false);

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT honey_string_userfree_t
honey_uridecode(const honey_string_t* text,
              int convert_to_utf8,
              honey_uri_unescape_rule_t unescape_rule) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: text; type: string_byref_const
  DCHECK(text);
  if (!text) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombURIDecode(
      HoneycombString(text), convert_to_utf8 ? true : false, unescape_rule);

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT struct _honey_value_t* honey_parse_json(
    const honey_string_t* json_string,
    honey_json_parser_options_t options) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: json_string; type: string_byref_const
  DCHECK(json_string);
  if (!json_string) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombValue> _retval = HoneycombParseJSON(HoneycombString(json_string), options);

  // Return type: refptr_same
  return HoneycombValueCppToC::Wrap(_retval);
}

HONEYCOMB_EXPORT struct _honey_value_t* honey_parse_json_buffer(
    const void* json,
    size_t json_size,
    honey_json_parser_options_t options) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: json; type: simple_byaddr
  DCHECK(json);
  if (!json) {
    return NULL;
  }

  // Execute
  HoneycombRefPtr<HoneycombValue> _retval = HoneycombParseJSON(json, json_size, options);

  // Return type: refptr_same
  return HoneycombValueCppToC::Wrap(_retval);
}

HONEYCOMB_EXPORT struct _honey_value_t* honey_parse_jsonand_return_error(
    const honey_string_t* json_string,
    honey_json_parser_options_t options,
    honey_string_t* error_msg_out) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: json_string; type: string_byref_const
  DCHECK(json_string);
  if (!json_string) {
    return NULL;
  }
  // Verify param: error_msg_out; type: string_byref
  DCHECK(error_msg_out);
  if (!error_msg_out) {
    return NULL;
  }

  // Translate param: error_msg_out; type: string_byref
  HoneycombString error_msg_outStr(error_msg_out);

  // Execute
  HoneycombRefPtr<HoneycombValue> _retval = HoneycombParseJSONAndReturnError(
      HoneycombString(json_string), options, error_msg_outStr);

  // Return type: refptr_same
  return HoneycombValueCppToC::Wrap(_retval);
}

HONEYCOMB_EXPORT honey_string_userfree_t
honey_write_json(struct _honey_value_t* node, honey_json_writer_options_t options) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: node; type: refptr_same
  DCHECK(node);
  if (!node) {
    return NULL;
  }

  // Execute
  HoneycombString _retval = HoneycombWriteJSON(HoneycombValueCppToC::Unwrap(node), options);

  // Return type: string
  return _retval.DetachToUserFree();
}

HONEYCOMB_EXPORT int honey_get_path(honey_path_key_t key, honey_string_t* path) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: path; type: string_byref
  DCHECK(path);
  if (!path) {
    return 0;
  }

  // Translate param: path; type: string_byref
  HoneycombString pathStr(path);

  // Execute
  bool _retval = HoneycombGetPath(key, pathStr);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_launch_process(struct _honey_command_line_t* command_line) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: command_line; type: refptr_same
  DCHECK(command_line);
  if (!command_line) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombLaunchProcess(HoneycombCommandLineCppToC::Unwrap(command_line));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_register_scheme_handler_factory(
    const honey_string_t* scheme_name,
    const honey_string_t* domain_name,
    struct _honey_scheme_handler_factory_t* factory) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(scheme_name);
  if (!scheme_name) {
    return 0;
  }
  // Unverified params: domain_name, factory

  // Execute
  bool _retval = HoneycombRegisterSchemeHandlerFactory(
      HoneycombString(scheme_name), HoneycombString(domain_name),
      HoneycombSchemeHandlerFactoryCToCpp::Wrap(factory));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_clear_scheme_handler_factories() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombClearSchemeHandlerFactories();

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_is_cert_status_error(honey_cert_status_t status) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombIsCertStatusError(status);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_currently_on(honey_thread_id_t threadId) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  bool _retval = HoneycombCurrentlyOn(threadId);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_post_task(honey_thread_id_t threadId,
                             struct _honey_task_t* task) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task);
  if (!task) {
    return 0;
  }

  // Execute
  bool _retval = HoneycombPostTask(threadId, HoneycombTaskCToCpp::Wrap(task));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_post_delayed_task(honey_thread_id_t threadId,
                                     struct _honey_task_t* task,
                                     int64_t delay_ms) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: task; type: refptr_diff
  DCHECK(task);
  if (!task) {
    return 0;
  }

  // Execute
  bool _retval =
      HoneycombPostDelayedTask(threadId, HoneycombTaskCToCpp::Wrap(task), delay_ms);

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_begin_tracing(const honey_string_t* categories,
                                 struct _honey_completion_callback_t* callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: categories, callback

  // Execute
  bool _retval = HoneycombBeginTracing(HoneycombString(categories),
                                 HoneycombCompletionCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int honey_end_tracing(const honey_string_t* tracing_file,
                               struct _honey_end_tracing_callback_t* callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: tracing_file, callback

  // Execute
  bool _retval = HoneycombEndTracing(HoneycombString(tracing_file),
                               HoneycombEndTracingCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT int64_t honey_now_from_system_trace_time() {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int64_t _retval = HoneycombNowFromSystemTraceTime();

  // Return type: simple
  return _retval;
}

HONEYCOMB_EXPORT int honey_register_extension(const honey_string_t* extension_name,
                                      const honey_string_t* javascript_code,
                                      struct _honey_v8handler_t* handler) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: extension_name; type: string_byref_const
  DCHECK(extension_name);
  if (!extension_name) {
    return 0;
  }
  // Verify param: javascript_code; type: string_byref_const
  DCHECK(javascript_code);
  if (!javascript_code) {
    return 0;
  }
  // Unverified params: handler

  // Execute
  bool _retval = HoneycombRegisterExtension(HoneycombString(extension_name),
                                      HoneycombString(javascript_code),
                                      HoneycombV8HandlerCToCpp::Wrap(handler));

  // Return type: bool
  return _retval;
}

HONEYCOMB_EXPORT void honey_execute_java_script_with_user_gesture_for_tests(
    struct _honey_frame_t* frame,
    const honey_string_t* javascript) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: frame; type: refptr_same
  DCHECK(frame);
  if (!frame) {
    return;
  }
  // Unverified params: javascript

  // Execute
  HoneycombExecuteJavaScriptWithUserGestureForTests(HoneycombFrameCppToC::Unwrap(frame),
                                              HoneycombString(javascript));
}

HONEYCOMB_EXPORT void honey_set_data_directory_for_tests(const honey_string_t* dir) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: dir; type: string_byref_const
  DCHECK(dir);
  if (!dir) {
    return;
  }

  // Execute
  HoneycombSetDataDirectoryForTests(HoneycombString(dir));
}
