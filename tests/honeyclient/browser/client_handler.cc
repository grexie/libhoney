// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/client_handler.h"

#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

#include "include/base/honey_callback.h"
#include "include/honey_browser.h"
#include "include/honey_command_ids.h"
#include "include/honey_frame.h"
#include "include/honey_parser.h"
#include "include/honey_shared_process_message_builder.h"
#include "include/honey_ssl_status.h"
#include "include/honey_x509_certificate.h"
#include "include/wrapper/honey_closure_task.h"
#include "tests/honeyclient/browser/main_context.h"
#include "tests/honeyclient/browser/root_window_manager.h"
#include "tests/honeyclient/browser/test_runner.h"
#include "tests/shared/browser/extension_util.h"
#include "tests/shared/browser/resource_util.h"
#include "tests/shared/common/binary_value_utils.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/common/string_util.h"

namespace client {

#if defined(OS_WIN)
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

namespace {

// Custom menu command Ids.
enum client_menu_ids {
  CLIENT_ID_SHOW_DEVTOOLS = MENU_ID_USER_FIRST,
  CLIENT_ID_CLOSE_DEVTOOLS,
  CLIENT_ID_INSPECT_ELEMENT,
  CLIENT_ID_SHOW_SSL_INFO,
  CLIENT_ID_CURSOR_CHANGE_DISABLED,
  CLIENT_ID_MEDIA_HANDLING_DISABLED,
  CLIENT_ID_OFFLINE,
  CLIENT_ID_TESTMENU_SUBMENU,
  CLIENT_ID_TESTMENU_CHECKITEM,
  CLIENT_ID_TESTMENU_RADIOITEM1,
  CLIENT_ID_TESTMENU_RADIOITEM2,
  CLIENT_ID_TESTMENU_RADIOITEM3,
};

// Must match the value in client_renderer.cc.
const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

std::string GetTimeString(const HoneycombTime& value) {
  if (value.GetTimeT() == 0) {
    return "Unspecified";
  }

  static const char* kMonths[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  std::string month;
  if (value.month >= 1 && value.month <= 12) {
    month = kMonths[value.month - 1];
  } else {
    month = "Invalid";
  }

  std::stringstream ss;
  ss << month << " " << value.day_of_month << ", " << value.year << " "
     << std::setfill('0') << std::setw(2) << value.hour << ":"
     << std::setfill('0') << std::setw(2) << value.minute << ":"
     << std::setfill('0') << std::setw(2) << value.second;
  return ss.str();
}

std::string GetTimeString(const HoneycombBaseTime& value) {
  HoneycombTime time;
  if (honey_time_from_basetime(value, &time)) {
    return GetTimeString(time);
  } else {
    return "Invalid";
  }
}

std::string GetBinaryString(HoneycombRefPtr<HoneycombBinaryValue> value) {
  if (!value.get()) {
    return "&nbsp;";
  }

  // Retrieve the value.
  const size_t size = value->GetSize();
  std::string src;
  src.resize(size);
  value->GetData(const_cast<char*>(src.data()), size, 0);

  // Encode the value.
  return HoneycombBase64Encode(src.data(), src.size());
}

#define FLAG(flag)                          \
  if (status & flag) {                      \
    result += std::string(#flag) + "<br/>"; \
  }

#define VALUE(val, def)       \
  if (val == def) {           \
    return std::string(#def); \
  }

std::string GetCertStatusString(honey_cert_status_t status) {
  std::string result;

  FLAG(CERT_STATUS_COMMON_NAME_INVALID);
  FLAG(CERT_STATUS_DATE_INVALID);
  FLAG(CERT_STATUS_AUTHORITY_INVALID);
  FLAG(CERT_STATUS_NO_REVOCATION_MECHANISM);
  FLAG(CERT_STATUS_UNABLE_TO_CHECK_REVOCATION);
  FLAG(CERT_STATUS_REVOKED);
  FLAG(CERT_STATUS_INVALID);
  FLAG(CERT_STATUS_WEAK_SIGNATURE_ALGORITHM);
  FLAG(CERT_STATUS_NON_UNIQUE_NAME);
  FLAG(CERT_STATUS_WEAK_KEY);
  FLAG(CERT_STATUS_PINNED_KEY_MISSING);
  FLAG(CERT_STATUS_NAME_CONSTRAINT_VIOLATION);
  FLAG(CERT_STATUS_VALIDITY_TOO_LONG);
  FLAG(CERT_STATUS_IS_EV);
  FLAG(CERT_STATUS_REV_CHECKING_ENABLED);
  FLAG(CERT_STATUS_SHA1_SIGNATURE_PRESENT);
  FLAG(CERT_STATUS_CT_COMPLIANCE_FAILED);

  if (result.empty()) {
    return "&nbsp;";
  }
  return result;
}

std::string GetSSLVersionString(honey_ssl_version_t version) {
  VALUE(version, SSL_CONNECTION_VERSION_UNKNOWN);
  VALUE(version, SSL_CONNECTION_VERSION_SSL2);
  VALUE(version, SSL_CONNECTION_VERSION_SSL3);
  VALUE(version, SSL_CONNECTION_VERSION_TLS1);
  VALUE(version, SSL_CONNECTION_VERSION_TLS1_1);
  VALUE(version, SSL_CONNECTION_VERSION_TLS1_2);
  VALUE(version, SSL_CONNECTION_VERSION_TLS1_3);
  VALUE(version, SSL_CONNECTION_VERSION_QUIC);
  return std::string();
}

std::string GetContentStatusString(honey_ssl_content_status_t status) {
  std::string result;

  VALUE(status, SSL_CONTENT_NORMAL_CONTENT);
  FLAG(SSL_CONTENT_DISPLAYED_INSECURE_CONTENT);
  FLAG(SSL_CONTENT_RAN_INSECURE_CONTENT);

  if (result.empty()) {
    return "&nbsp;";
  }
  return result;
}

// Load a data: URI containing the error message.
void LoadErrorPage(HoneycombRefPtr<HoneycombFrame> frame,
                   const std::string& failed_url,
                   honey_errorcode_t error_code,
                   const std::string& other_info) {
  std::stringstream ss;
  ss << "<html><head><title>Page failed to load</title></head>"
        "<body bgcolor=\"white\">"
        "<h3>Page failed to load.</h3>"
        "URL: <a href=\""
     << failed_url << "\">" << failed_url
     << "</a><br/>Error: " << test_runner::GetErrorString(error_code) << " ("
     << error_code << ")";

  if (!other_info.empty()) {
    ss << "<br/>" << other_info;
  }

  ss << "</body></html>";
  frame->LoadURL(test_runner::GetDataURI(ss.str(), "text/html"));
}

// Return HTML string with information about a certificate.
std::string GetCertificateInformation(HoneycombRefPtr<HoneycombX509Certificate> cert,
                                      honey_cert_status_t certstatus) {
  HoneycombRefPtr<HoneycombX509CertPrincipal> subject = cert->GetSubject();
  HoneycombRefPtr<HoneycombX509CertPrincipal> issuer = cert->GetIssuer();

  // Build a table showing certificate information. Various types of invalid
  // certificates can be tested using https://badssl.com/.
  std::stringstream ss;
  ss << "<h3>X.509 Certificate Information:</h3>"
        "<table border=1><tr><th>Field</th><th>Value</th></tr>";

  if (certstatus != CERT_STATUS_NONE) {
    ss << "<tr><td>Status</td><td>" << GetCertStatusString(certstatus)
       << "</td></tr>";
  }

  ss << "<tr><td>Subject</td><td>"
     << (subject.get() ? subject->GetDisplayName().ToString() : "&nbsp;")
     << "</td></tr>"
        "<tr><td>Issuer</td><td>"
     << (issuer.get() ? issuer->GetDisplayName().ToString() : "&nbsp;")
     << "</td></tr>"
        "<tr><td>Serial #*</td><td>"
     << GetBinaryString(cert->GetSerialNumber()) << "</td></tr>"
     << "<tr><td>Valid Start</td><td>" << GetTimeString(cert->GetValidStart())
     << "</td></tr>"
        "<tr><td>Valid Expiry</td><td>"
     << GetTimeString(cert->GetValidExpiry()) << "</td></tr>";

  HoneycombX509Certificate::IssuerChainBinaryList der_chain_list;
  HoneycombX509Certificate::IssuerChainBinaryList pem_chain_list;
  cert->GetDEREncodedIssuerChain(der_chain_list);
  cert->GetPEMEncodedIssuerChain(pem_chain_list);
  DCHECK_EQ(der_chain_list.size(), pem_chain_list.size());

  der_chain_list.insert(der_chain_list.begin(), cert->GetDEREncoded());
  pem_chain_list.insert(pem_chain_list.begin(), cert->GetPEMEncoded());

  for (size_t i = 0U; i < der_chain_list.size(); ++i) {
    ss << "<tr><td>DER Encoded*</td>"
          "<td style=\"max-width:800px;overflow:scroll;\">"
       << GetBinaryString(der_chain_list[i])
       << "</td></tr>"
          "<tr><td>PEM Encoded*</td>"
          "<td style=\"max-width:800px;overflow:scroll;\">"
       << GetBinaryString(pem_chain_list[i]) << "</td></tr>";
  }

  ss << "</table> * Displayed value is base64 encoded.";
  return ss.str();
}

void OnTestProcessMessageReceived(
    const HoneycombRefPtr<HoneycombFrame>& frame,
    const HoneycombRefPtr<HoneycombProcessMessage>& process_message,
    const bv_utils::TimePoint& finish_time) {
  DCHECK(process_message->IsValid());

  HoneycombRefPtr<HoneycombListValue> input_args = process_message->GetArgumentList();
  DCHECK_EQ(input_args->GetSize(), 1U);

  const auto renderer_msg =
      bv_utils::GetRendererMsgFromBinary(input_args->GetBinary(0));

  HoneycombRefPtr<HoneycombProcessMessage> response =
      HoneycombProcessMessage::Create(bv_utils::kTestSendProcessMessage);
  HoneycombRefPtr<HoneycombListValue> args = response->GetArgumentList();

  const auto message_size = std::max(input_args->GetBinary(0)->GetSize(),
                                     sizeof(bv_utils::BrowserMessage));
  std::vector<uint8_t> data(message_size);

  const auto browser_msg =
      reinterpret_cast<bv_utils::BrowserMessage*>(data.data());
  browser_msg->test_id = renderer_msg.test_id;
  browser_msg->duration = finish_time - renderer_msg.start_time;
  browser_msg->start_time = bv_utils::Now();

  args->SetBinary(0, bv_utils::CreateHoneycombBinaryValue(data));
  frame->SendProcessMessage(PID_RENDERER, response);
}

void OnTestSMRProcessMessageReceived(
    const HoneycombRefPtr<HoneycombFrame>& frame,
    const HoneycombRefPtr<HoneycombProcessMessage>& process_message,
    const bv_utils::TimePoint& finish_time) {
  DCHECK(process_message->IsValid());

  HoneycombRefPtr<HoneycombSharedMemoryRegion> region =
      process_message->GetSharedMemoryRegion();
  DCHECK_GE(region->Size(), sizeof(bv_utils::RendererMessage));

  const auto renderer_msg =
      static_cast<const bv_utils::RendererMessage*>(region->Memory());
  const auto message_size =
      std::max(region->Size(), sizeof(bv_utils::BrowserMessage));

  std::vector<uint8_t> data(message_size);
  const auto browser_msg =
      reinterpret_cast<bv_utils::BrowserMessage*>(data.data());
  browser_msg->test_id = renderer_msg->test_id;
  browser_msg->duration = finish_time - renderer_msg->start_time;
  browser_msg->start_time = bv_utils::Now();

  auto builder = HoneycombSharedProcessMessageBuilder::Create(
      bv_utils::kTestSendSMRProcessMessage, message_size);

  bv_utils::CopyDataIntoMemory(data, builder->Memory());

  frame->SendProcessMessage(PID_RENDERER, builder->Build());
}

bool IsAllowedPageActionIcon(honey_chrome_page_action_icon_type_t icon_type) {
  // Only the specified icons will be allowed.
  switch (icon_type) {
    case HONEYCOMB_CPAIT_FIND:
    case HONEYCOMB_CPAIT_ZOOM:
      return true;
    default:
      break;
  }
  return false;
}

bool IsAllowedToolbarButton(honey_chrome_toolbar_button_type_t button_type) {
  // All configurable buttons will be disabled.
  return false;
}

bool IsAllowedAppMenuCommandId(int command_id) {
  // Only the commands in this array will be allowed.
  static const int kAllowedCommandIds[] = {
      IDC_NEW_WINDOW,
      IDC_NEW_INCOGNITO_WINDOW,

      // Zoom buttons.
      IDC_ZOOM_MENU,
      IDC_ZOOM_PLUS,
      IDC_ZOOM_NORMAL,
      IDC_ZOOM_MINUS,
      IDC_FULLSCREEN,

      IDC_PRINT,
      IDC_FIND,
      IDC_FIND_NEXT,
      IDC_FIND_PREVIOUS,

      // "More tools" sub-menu and contents.
      IDC_MORE_TOOLS_MENU,
      IDC_CLEAR_BROWSING_DATA,
      IDC_MANAGE_EXTENSIONS,
      IDC_PERFORMANCE,
      IDC_TASK_MANAGER,
      IDC_DEV_TOOLS,

      // Edit buttons.
      IDC_EDIT_MENU,
      IDC_CUT,
      IDC_COPY,
      IDC_PASTE,

      IDC_OPTIONS,
      IDC_EXIT,
  };
  for (size_t i = 0; i < std::size(kAllowedCommandIds); ++i) {
    if (command_id == kAllowedCommandIds[i]) {
      return true;
    }
  }
  return false;
}

bool IsAllowedContextMenuCommandId(int command_id) {
  // Allow commands added by web content.
  if (command_id >= IDC_CONTENT_CONTEXT_CUSTOM_FIRST &&
      command_id <= IDC_CONTENT_CONTEXT_CUSTOM_LAST) {
    return true;
  }

  // Allow commands added by extensions.
  if (command_id >= IDC_EXTENSIONS_CONTEXT_CUSTOM_FIRST &&
      command_id <= IDC_EXTENSIONS_CONTEXT_CUSTOM_LAST) {
    return true;
  }

  // Only the commands in this array will be allowed.
  static const int kAllowedCommandIds[] = {
      // Page navigation.
      IDC_BACK,
      IDC_FORWARD,
      IDC_RELOAD,
      IDC_RELOAD_BYPASSING_CACHE,
      IDC_RELOAD_CLEARING_CACHE,
      IDC_STOP,

      // Printing.
      IDC_PRINT,

      // Edit controls.
      IDC_CONTENT_CONTEXT_CUT,
      IDC_CONTENT_CONTEXT_COPY,
      IDC_CONTENT_CONTEXT_PASTE,
      IDC_CONTENT_CONTEXT_PASTE_AND_MATCH_STYLE,
      IDC_CONTENT_CONTEXT_DELETE,
      IDC_CONTENT_CONTEXT_SELECTALL,
      IDC_CONTENT_CONTEXT_UNDO,
      IDC_CONTENT_CONTEXT_REDO,
  };
  for (size_t i = 0; i < std::size(kAllowedCommandIds); ++i) {
    if (command_id == kAllowedCommandIds[i]) {
      return true;
    }
  }
  return false;
}

void FilterContextMenuModel(HoneycombRefPtr<HoneycombMenuModel> model) {
  // Evaluate from the bottom to the top because we'll be removing menu items.
  for (size_t x = model->GetCount(); x > 0; --x) {
    const auto i = x - 1;
    const auto type = model->GetTypeAt(i);
    if (type == MENUITEMTYPE_SUBMENU) {
      // Filter sub-menu and remove if empty.
      auto sub_model = model->GetSubMenuAt(i);
      FilterContextMenuModel(sub_model);
      if (sub_model->GetCount() == 0) {
        model->RemoveAt(i);
      }
    } else if (type == MENUITEMTYPE_SEPARATOR) {
      // A separator shouldn't be the first or last element in the menu, and
      // there shouldn't be multiple in a row.
      if (i == 0 || i == model->GetCount() - 1 ||
          model->GetTypeAt(i + 1) == MENUITEMTYPE_SEPARATOR) {
        model->RemoveAt(i);
      }
    } else if (!IsAllowedContextMenuCommandId(model->GetCommandIdAt(i))) {
      model->RemoveAt(i);
    }
  }
}

}  // namespace

class ClientDownloadImageCallback : public HoneycombDownloadImageCallback {
 public:
  explicit ClientDownloadImageCallback(HoneycombRefPtr<ClientHandler> client_handler)
      : client_handler_(client_handler) {}

  void OnDownloadImageFinished(const HoneycombString& image_url,
                               int http_status_code,
                               HoneycombRefPtr<HoneycombImage> image) override {
    if (image) {
      client_handler_->NotifyFavicon(image);
    }
  }

 private:
  HoneycombRefPtr<ClientHandler> client_handler_;

  IMPLEMENT_REFCOUNTING(ClientDownloadImageCallback);
  DISALLOW_COPY_AND_ASSIGN(ClientDownloadImageCallback);
};

ClientHandler::ClientHandler(Delegate* delegate,
                             bool is_osr,
                             bool with_controls,
                             const std::string& startup_url)
    : is_osr_(is_osr),
      with_controls_(with_controls),
      startup_url_(startup_url),
      delegate_(delegate),
      console_log_file_(MainContext::Get()->GetConsoleLogPath()) {
  DCHECK(!console_log_file_.empty());

  resource_manager_ = new HoneycombResourceManager();
  test_runner::SetupResourceManager(resource_manager_, &string_resource_map_);

  // Read command line settings.
  HoneycombRefPtr<HoneycombCommandLine> command_line =
      HoneycombCommandLine::GetGlobalCommandLine();
  mouse_cursor_change_disabled_ =
      command_line->HasSwitch(switches::kMouseCursorChangeDisabled);
  offline_ = command_line->HasSwitch(switches::kOffline);
  filter_chrome_commands_ =
      command_line->HasSwitch(switches::kFilterChromeCommands);

#if defined(OS_LINUX)
  // Optionally use the client-provided GTK dialogs.
  const bool use_client_dialogs =
      command_line->HasSwitch(switches::kUseClientDialogs);

  // Determine if the client-provided GTK dialogs can/should be used.
  bool require_client_dialogs = false;
  bool support_client_dialogs = true;

  if (command_line->HasSwitch(switches::kMultiThreadedMessageLoop)) {
    // Default/internal GTK dialogs are not supported in combination with
    // multi-threaded-message-loop because Chromium doesn't support GDK threads.
    // This does not apply to the JS dialogs which use Views instead of GTK.
    if (!use_client_dialogs) {
      LOG(WARNING) << "Client dialogs must be used in combination with "
                      "multi-threaded-message-loop.";
    }
    require_client_dialogs = true;
  }

  if (MainContext::Get()->UseViews()) {
    // Client-provided GTK dialogs cannot be used in combination with Views
    // because the implementation of ClientDialogHandlerGtk requires a top-level
    // GtkWindow.
    if (use_client_dialogs) {
      LOG(ERROR) << "Client dialogs cannot be used in combination with Views.";
    }
    support_client_dialogs = false;
  }

  if (support_client_dialogs) {
    if (use_client_dialogs) {
      js_dialog_handler_ = new ClientDialogHandlerGtk();
    }
    if (use_client_dialogs || require_client_dialogs) {
      file_dialog_handler_ = js_dialog_handler_ ? js_dialog_handler_
                                                : new ClientDialogHandlerGtk();
      print_handler_ = new ClientPrintHandlerGtk();
    }
  }
#endif  // defined(OS_LINUX)
}

void ClientHandler::DetachDelegate() {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&ClientHandler::DetachDelegate, this));
    return;
  }

  DCHECK(delegate_);
  delegate_ = nullptr;
}

bool ClientHandler::OnProcessMessageReceived(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombProcessId source_process,
    HoneycombRefPtr<HoneycombProcessMessage> message) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  const auto finish_time = bv_utils::Now();

  if (message_router_->OnProcessMessageReceived(browser, frame, source_process,
                                                message)) {
    return true;
  }

  // Check for messages from the client renderer.
  const std::string& message_name = message->GetName();
  if (message_name == kFocusedNodeChangedMessage) {
    // A message is sent from ClientRenderDelegate to tell us whether the
    // currently focused DOM node is editable. Use of |focus_on_editable_field_|
    // is redundant with HoneycombKeyEvent.focus_on_editable_field in OnPreKeyEvent
    // but is useful for demonstration purposes.
    focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
    return true;
  }

  if (message_name == bv_utils::kTestSendProcessMessage) {
    OnTestProcessMessageReceived(frame, message, finish_time);
    return true;
  }

  if (message_name == bv_utils::kTestSendSMRProcessMessage) {
    OnTestSMRProcessMessageReceived(frame, message, finish_time);
    return true;
  }

  return false;
}

bool ClientHandler::OnChromeCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                                    int command_id,
                                    honey_window_open_disposition_t disposition) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(MainContext::Get()->UseChromeRuntime());

  const bool allowed = IsAllowedAppMenuCommandId(command_id) ||
                       IsAllowedContextMenuCommandId(command_id);

  bool block = false;
  if (filter_chrome_commands_) {
    // Block all commands that aren't specifically allowed.
    block = !allowed;
  } else if (!with_controls_) {
    // If controls are hidden, block all commands that don't target the current
    // tab or aren't specifically allowed.
    block = disposition != WOD_CURRENT_TAB || !allowed;
  }

  if (block) {
    LOG(INFO) << "Blocking command " << command_id << " with disposition "
              << disposition;
    return true;
  }

  // Default handling.
  return false;
}

bool ClientHandler::IsChromeAppMenuItemVisible(HoneycombRefPtr<HoneycombBrowser> browser,
                                               int command_id) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(MainContext::Get()->UseChromeRuntime());
  if (!filter_chrome_commands_) {
    return true;
  }
  return IsAllowedAppMenuCommandId(command_id);
}

bool ClientHandler::IsChromePageActionIconVisible(
    honey_chrome_page_action_icon_type_t icon_type) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(MainContext::Get()->UseChromeRuntime());
  if (!filter_chrome_commands_) {
    return true;
  }
  return IsAllowedPageActionIcon(icon_type);
}

bool ClientHandler::IsChromeToolbarButtonVisible(
    honey_chrome_toolbar_button_type_t button_type) {
  HONEYCOMB_REQUIRE_UI_THREAD();
  DCHECK(MainContext::Get()->UseChromeRuntime());
  if (!filter_chrome_commands_) {
    return true;
  }
  return IsAllowedToolbarButton(button_type);
}

void ClientHandler::OnBeforeContextMenu(HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombRefPtr<HoneycombContextMenuParams> params,
                                        HoneycombRefPtr<HoneycombMenuModel> model) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  const bool use_chrome_runtime = MainContext::Get()->UseChromeRuntime();
  if (use_chrome_runtime && (!with_controls_ || filter_chrome_commands_)) {
    // Remove all disallowed menu items.
    FilterContextMenuModel(model);
  }

  if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0) {
    // Add a separator if the menu already has items.
    if (model->GetCount() > 0) {
      model->AddSeparator();
    }

    if (!use_chrome_runtime) {
      // TODO(chrome-runtime): Add support for this.
      // Add DevTools items to all context menus.
      model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, "&Show DevTools");
      model->AddItem(CLIENT_ID_CLOSE_DEVTOOLS, "Close DevTools");
      model->AddSeparator();
      model->AddItem(CLIENT_ID_INSPECT_ELEMENT, "Inspect Element");
    }

    if (HasSSLInformation(browser)) {
      model->AddSeparator();
      model->AddItem(CLIENT_ID_SHOW_SSL_INFO, "Show SSL information");
    }

    if (!use_chrome_runtime) {
      // TODO(chrome-runtime): Add support for this.
      model->AddSeparator();
      model->AddCheckItem(CLIENT_ID_CURSOR_CHANGE_DISABLED,
                          "Cursor change disabled");
      if (mouse_cursor_change_disabled_) {
        model->SetChecked(CLIENT_ID_CURSOR_CHANGE_DISABLED, true);
      }

      model->AddSeparator();
      model->AddCheckItem(CLIENT_ID_MEDIA_HANDLING_DISABLED,
                          "Media handling disabled");
      if (media_handling_disabled_) {
        model->SetChecked(CLIENT_ID_MEDIA_HANDLING_DISABLED, true);
      }
    }

    model->AddSeparator();
    model->AddCheckItem(CLIENT_ID_OFFLINE, "Offline mode");
    if (offline_) {
      model->SetChecked(CLIENT_ID_OFFLINE, true);
    }

    // Test context menu features.
    BuildTestMenu(model);
  }

  if (delegate_) {
    delegate_->OnBeforeContextMenu(model);
  }
}

bool ClientHandler::OnContextMenuCommand(HoneycombRefPtr<HoneycombBrowser> browser,
                                         HoneycombRefPtr<HoneycombFrame> frame,
                                         HoneycombRefPtr<HoneycombContextMenuParams> params,
                                         int command_id,
                                         EventFlags event_flags) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  switch (command_id) {
    case CLIENT_ID_SHOW_DEVTOOLS:
      ShowDevTools(browser, HoneycombPoint());
      return true;
    case CLIENT_ID_CLOSE_DEVTOOLS:
      CloseDevTools(browser);
      return true;
    case CLIENT_ID_INSPECT_ELEMENT:
      ShowDevTools(browser, HoneycombPoint(params->GetXCoord(), params->GetYCoord()));
      return true;
    case CLIENT_ID_SHOW_SSL_INFO:
      ShowSSLInformation(browser);
      return true;
    case CLIENT_ID_CURSOR_CHANGE_DISABLED:
      mouse_cursor_change_disabled_ = !mouse_cursor_change_disabled_;
      return true;
    case CLIENT_ID_MEDIA_HANDLING_DISABLED:
      media_handling_disabled_ = !media_handling_disabled_;
      return true;
    case CLIENT_ID_OFFLINE:
      offline_ = !offline_;
      SetOfflineState(browser, offline_);
      return true;
    default:  // Allow default handling, if any.
      return ExecuteTestMenu(command_id);
  }
}

void ClientHandler::OnAddressChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                    HoneycombRefPtr<HoneycombFrame> frame,
                                    const HoneycombString& url) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Only update the address for the main (top-level) frame.
  if (frame->IsMain()) {
    NotifyAddress(url);
  }
}

void ClientHandler::OnTitleChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                  const HoneycombString& title) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyTitle(title);
}

void ClientHandler::OnFaviconURLChange(
    HoneycombRefPtr<HoneycombBrowser> browser,
    const std::vector<HoneycombString>& icon_urls) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (!icon_urls.empty() && download_favicon_images_) {
    browser->GetHost()->DownloadImage(icon_urls[0], true, 16, false,
                                      new ClientDownloadImageCallback(this));
  }
}

void ClientHandler::OnFullscreenModeChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                           bool fullscreen) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyFullscreen(fullscreen);
}

bool ClientHandler::OnConsoleMessage(HoneycombRefPtr<HoneycombBrowser> browser,
                                     honey_log_severity_t level,
                                     const HoneycombString& message,
                                     const HoneycombString& source,
                                     int line) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  FILE* file = fopen(console_log_file_.c_str(), "a");
  if (file) {
    std::stringstream ss;
    ss << "Level: ";
    switch (level) {
      case LOGSEVERITY_DEBUG:
        ss << "Debug" << NEWLINE;
        break;
      case LOGSEVERITY_INFO:
        ss << "Info" << NEWLINE;
        break;
      case LOGSEVERITY_WARNING:
        ss << "Warn" << NEWLINE;
        break;
      case LOGSEVERITY_ERROR:
        ss << "Error" << NEWLINE;
        break;
      default:
        NOTREACHED();
        break;
    }
    ss << "Message: " << message.ToString() << NEWLINE
       << "Source: " << source.ToString() << NEWLINE << "Line: " << line
       << NEWLINE << "-----------------------" << NEWLINE;
    fputs(ss.str().c_str(), file);
    fclose(file);
  }

  return false;
}

bool ClientHandler::OnAutoResize(HoneycombRefPtr<HoneycombBrowser> browser,
                                 const HoneycombSize& new_size) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyAutoResize(new_size);
  return true;
}

bool ClientHandler::OnCursorChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombCursorHandle cursor,
                                   honey_cursor_type_t type,
                                   const HoneycombCursorInfo& custom_cursor_info) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Return true to disable default handling of cursor changes.
  return mouse_cursor_change_disabled_;
}

bool ClientHandler::CanDownload(HoneycombRefPtr<HoneycombBrowser> browser,
                                const HoneycombString& url,
                                const HoneycombString& request_method) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (!with_controls_) {
    // Block the download.
    LOG(INFO) << "Blocking download";
    return false;
  }

  // Allow the download.
  return true;
}

void ClientHandler::OnBeforeDownload(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDownloadItem> download_item,
    const HoneycombString& suggested_name,
    HoneycombRefPtr<HoneycombBeforeDownloadCallback> callback) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Continue the download and show the "Save As" dialog.
  callback->Continue(MainContext::Get()->GetDownloadPath(suggested_name), true);
}

void ClientHandler::OnDownloadUpdated(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombDownloadItem> download_item,
    HoneycombRefPtr<HoneycombDownloadItemCallback> callback) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (download_item->IsComplete()) {
    test_runner::Alert(browser, "File \"" +
                                    download_item->GetFullPath().ToString() +
                                    "\" downloaded successfully.");
  }
}

bool ClientHandler::OnDragEnter(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombDragData> dragData,
                                HoneycombDragHandler::DragOperationsMask mask) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Forbid dragging of URLs and files.
  if ((mask & DRAG_OPERATION_LINK) && !dragData->IsFragment()) {
    test_runner::Alert(browser, "honeyclient blocks dragging of URLs and files");
    return true;
  }

  return false;
}

void ClientHandler::OnDraggableRegionsChanged(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    const std::vector<HoneycombDraggableRegion>& regions) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyDraggableRegions(regions);
}

void ClientHandler::OnTakeFocus(HoneycombRefPtr<HoneycombBrowser> browser, bool next) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyTakeFocus(next);
}

bool ClientHandler::OnSetFocus(HoneycombRefPtr<HoneycombBrowser> browser,
                               FocusSource source) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (initial_navigation_) {
    HoneycombRefPtr<HoneycombCommandLine> command_line =
        HoneycombCommandLine::GetGlobalCommandLine();
    if (command_line->HasSwitch(switches::kNoActivate)) {
      // Don't give focus to the browser on creation.
      return true;
    }
  }

  return false;
}

bool ClientHandler::OnPreKeyEvent(HoneycombRefPtr<HoneycombBrowser> browser,
                                  const HoneycombKeyEvent& event,
                                  HoneycombEventHandle os_event,
                                  bool* is_keyboard_shortcut) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  /*
  if (!event.focus_on_editable_field && event.windows_key_code == 0x20) {
    // Special handling for the space character when an input element does not
    // have focus. Handling the event in OnPreKeyEvent() keeps the event from
    // being processed in the renderer. If we instead handled the event in the
    // OnKeyEvent() method the space key would cause the window to scroll in
    // addition to showing the alert box.
    if (event.type == KEYEVENT_RAWKEYDOWN)
      test_runner::Alert(browser, "You pressed the space bar!");
    return true;
  }
  */

  return false;
}

bool ClientHandler::OnBeforePopup(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    const HoneycombString& target_url,
    const HoneycombString& target_frame_name,
    HoneycombLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const HoneycombPopupFeatures& popupFeatures,
    HoneycombWindowInfo& windowInfo,
    HoneycombRefPtr<HoneycombClient>& client,
    HoneycombBrowserSettings& settings,
    HoneycombRefPtr<HoneycombDictionaryValue>& extra_info,
    bool* no_javascript_access) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (target_disposition == WOD_NEW_PICTURE_IN_PICTURE) {
    // Use default handling for document picture-in-picture popups.
    client = nullptr;
    return false;
  }

  // Return true to cancel the popup window.
  return !CreatePopupWindow(browser, false, popupFeatures, windowInfo, client,
                            settings);
}

void ClientHandler::OnAfterCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  browser_count_++;

  if (!message_router_) {
    // Create the browser-side router for query handling.
    HoneycombMessageRouterConfig config;
    message_router_ = HoneycombMessageRouterBrowserSide::Create(config);

    // Register handlers with the router.
    test_runner::CreateMessageHandlers(message_handler_set_);
    MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    for (; it != message_handler_set_.end(); ++it) {
      message_router_->AddHandler(*(it), false);
    }
  }

  // Set offline mode if requested via the command-line flag.
  if (offline_) {
    SetOfflineState(browser, true);
  }

  if (browser->GetHost()->GetExtension()) {
    // Browsers hosting extension apps should auto-resize.
    browser->GetHost()->SetAutoResizeEnabled(true, HoneycombSize(20, 20),
                                             HoneycombSize(1000, 1000));

    HoneycombRefPtr<HoneycombExtension> extension = browser->GetHost()->GetExtension();
    if (extension_util::IsInternalExtension(extension->GetPath())) {
      // Register the internal handler for extension resources.
      extension_util::AddInternalExtensionToResourceManager(extension,
                                                            resource_manager_);
    }
  }

  NotifyBrowserCreated(browser);
}

bool ClientHandler::DoClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  NotifyBrowserClosing(browser);

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void ClientHandler::OnBeforeClose(HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (--browser_count_ == 0) {
    // Remove and delete message router handlers.
    MessageHandlerSet::const_iterator it = message_handler_set_.begin();
    for (; it != message_handler_set_.end(); ++it) {
      message_router_->RemoveHandler(*(it));
      delete *(it);
    }
    message_handler_set_.clear();
    message_router_ = nullptr;
  }

  NotifyBrowserClosed(browser);
}

void ClientHandler::OnLoadingStateChange(HoneycombRefPtr<HoneycombBrowser> browser,
                                         bool isLoading,
                                         bool canGoBack,
                                         bool canGoForward) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (!isLoading && initial_navigation_) {
    initial_navigation_ = false;
  }

  NotifyLoadingState(isLoading, canGoBack, canGoForward);
}

void ClientHandler::OnLoadError(HoneycombRefPtr<HoneycombBrowser> browser,
                                HoneycombRefPtr<HoneycombFrame> frame,
                                ErrorCode errorCode,
                                const HoneycombString& errorText,
                                const HoneycombString& failedUrl) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED) {
    return;
  }

  // Don't display an error for external protocols that we allow the OS to
  // handle. See OnProtocolExecution().
  if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
    std::string urlStr = frame->GetURL();
    if (urlStr.find("spotify:") == 0) {
      return;
    }
  }

  // Load the error page.
  LoadErrorPage(frame, failedUrl, errorCode, errorText);
}

bool ClientHandler::OnRequestMediaAccessPermission(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    const HoneycombString& requesting_origin,
    uint32_t requested_permissions,
    HoneycombRefPtr<HoneycombMediaAccessCallback> callback) {
  callback->Continue(media_handling_disabled_ ? HONEYCOMB_MEDIA_PERMISSION_NONE
                                              : requested_permissions);
  return true;
}

bool ClientHandler::OnBeforeBrowse(HoneycombRefPtr<HoneycombBrowser> browser,
                                   HoneycombRefPtr<HoneycombFrame> frame,
                                   HoneycombRefPtr<HoneycombRequest> request,
                                   bool user_gesture,
                                   bool is_redirect) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

bool ClientHandler::OnOpenURLFromTab(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    const HoneycombString& target_url,
    HoneycombRequestHandler::WindowOpenDisposition target_disposition,
    bool user_gesture) {
  if (target_disposition == WOD_NEW_BACKGROUND_TAB ||
      target_disposition == WOD_NEW_FOREGROUND_TAB) {
    // Handle middle-click and ctrl + left-click by opening the URL in a new
    // browser window.
    auto config = std::make_unique<RootWindowConfig>();
    config->with_controls = with_controls_;
    config->with_osr = is_osr_;
    config->url = target_url;
    MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
        std::move(config));
    return true;
  }

  // Open the URL in the current browser window.
  return false;
}

HoneycombRefPtr<HoneycombResourceRequestHandler> ClientHandler::GetResourceRequestHandler(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    bool is_navigation,
    bool is_download,
    const HoneycombString& request_initiator,
    bool& disable_default_handling) {
  HONEYCOMB_REQUIRE_IO_THREAD();
  return this;
}

bool ClientHandler::GetAuthCredentials(HoneycombRefPtr<HoneycombBrowser> browser,
                                       const HoneycombString& origin_url,
                                       bool isProxy,
                                       const HoneycombString& host,
                                       int port,
                                       const HoneycombString& realm,
                                       const HoneycombString& scheme,
                                       HoneycombRefPtr<HoneycombAuthCallback> callback) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  // Used for testing authentication with a proxy server.
  // For example, CCProxy on Windows.
  if (isProxy) {
    callback->Continue("guest", "guest");
    return true;
  }

  // Used for testing authentication with https://jigsaw.w3.org/HTTP/.
  if (host == "jigsaw.w3.org") {
    callback->Continue("guest", "guest");
    return true;
  }

  return false;
}

bool ClientHandler::OnCertificateError(HoneycombRefPtr<HoneycombBrowser> browser,
                                       ErrorCode cert_error,
                                       const HoneycombString& request_url,
                                       HoneycombRefPtr<HoneycombSSLInfo> ssl_info,
                                       HoneycombRefPtr<HoneycombCallback> callback) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  if (cert_error == ERR_CERT_COMMON_NAME_INVALID &&
      request_url.ToString().find("https://www.magpcss.com/") == 0U) {
    // Allow magpcss.com to load despite having a certificate common name of
    // magpcss.org.
    callback->Continue();
    return true;
  }

  HoneycombRefPtr<HoneycombX509Certificate> cert = ssl_info->GetX509Certificate();
  if (cert.get()) {
    // Load the error page.
    LoadErrorPage(browser->GetMainFrame(), request_url, cert_error,
                  GetCertificateInformation(cert, ssl_info->GetCertStatus()));
  }

  return false;  // Cancel the request.
}

bool ClientHandler::OnSelectClientCertificate(
    HoneycombRefPtr<HoneycombBrowser> browser,
    bool isProxy,
    const HoneycombString& host,
    int port,
    const X509CertificateList& certificates,
    HoneycombRefPtr<HoneycombSelectClientCertificateCallback> callback) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  HoneycombRefPtr<HoneycombCommandLine> command_line =
      HoneycombCommandLine::GetGlobalCommandLine();
  if (!command_line->HasSwitch(switches::kSslClientCertificate)) {
    return false;
  }

  const std::string& cert_name =
      command_line->GetSwitchValue(switches::kSslClientCertificate);

  if (cert_name.empty()) {
    callback->Select(nullptr);
    return true;
  }

  std::vector<HoneycombRefPtr<HoneycombX509Certificate>>::const_iterator it =
      certificates.begin();
  for (; it != certificates.end(); ++it) {
    HoneycombString subject((*it)->GetSubject()->GetDisplayName());
    if (subject == cert_name) {
      callback->Select(*it);
      return true;
    }
  }

  return true;
}

void ClientHandler::OnRenderProcessTerminated(HoneycombRefPtr<HoneycombBrowser> browser,
                                              TerminationStatus status) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  message_router_->OnRenderProcessTerminated(browser);

  // Don't reload if there's no start URL, or if the crash URL was specified.
  if (startup_url_.empty() || startup_url_ == "chrome://crash") {
    return;
  }

  HoneycombRefPtr<HoneycombFrame> frame = browser->GetMainFrame();
  std::string url = frame->GetURL();

  // Don't reload if the termination occurred before any URL had successfully
  // loaded.
  if (url.empty()) {
    return;
  }

  // Convert URLs to lowercase for easier comparison.
  url = AsciiStrToLower(url);
  const std::string& start_url = AsciiStrToLower(startup_url_);

  // Don't reload the URL that just resulted in termination.
  if (url.find(start_url) == 0) {
    return;
  }

  frame->LoadURL(startup_url_);
}

void ClientHandler::OnDocumentAvailableInMainFrame(
    HoneycombRefPtr<HoneycombBrowser> browser) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // Restore offline mode after main frame navigation. Otherwise, offline state
  // (e.g. `navigator.onLine`) might be wrong in the renderer process.
  if (offline_) {
    SetOfflineState(browser, true);
  }
}

honey_return_value_t ClientHandler::OnBeforeResourceLoad(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombCallback> callback) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return resource_manager_->OnBeforeResourceLoad(browser, frame, request,
                                                 callback);
}

HoneycombRefPtr<HoneycombResourceHandler> ClientHandler::GetResourceHandler(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return resource_manager_->GetResourceHandler(browser, frame, request);
}

HoneycombRefPtr<HoneycombResponseFilter> ClientHandler::GetResourceResponseFilter(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombResponse> response) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  return test_runner::GetResourceResponseFilter(browser, frame, request,
                                                response);
}

void ClientHandler::OnProtocolExecution(HoneycombRefPtr<HoneycombBrowser> browser,
                                        HoneycombRefPtr<HoneycombFrame> frame,
                                        HoneycombRefPtr<HoneycombRequest> request,
                                        bool& allow_os_execution) {
  HONEYCOMB_REQUIRE_IO_THREAD();

  std::string urlStr = request->GetURL();

  // Allow OS execution of Spotify URIs.
  if (urlStr.find("spotify:") == 0) {
    allow_os_execution = true;
  }
}

int ClientHandler::GetBrowserCount() const {
  HONEYCOMB_REQUIRE_UI_THREAD();
  return browser_count_;
}

void ClientHandler::ShowDevTools(HoneycombRefPtr<HoneycombBrowser> browser,
                                 const HoneycombPoint& inspect_element_at) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute this method on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&ClientHandler::ShowDevTools, this,
                                       browser, inspect_element_at));
    return;
  }

  HoneycombWindowInfo windowInfo;
  HoneycombRefPtr<HoneycombClient> client;
  HoneycombBrowserSettings settings;

  MainContext::Get()->PopulateBrowserSettings(&settings);

  HoneycombRefPtr<HoneycombBrowserHost> host = browser->GetHost();

  // Test if the DevTools browser already exists.
  bool has_devtools = host->HasDevTools();
  if (!has_devtools) {
    // Create a new RootWindow for the DevTools browser that will be created
    // by ShowDevTools().
    has_devtools = CreatePopupWindow(browser, true, HoneycombPopupFeatures(),
                                     windowInfo, client, settings);
  }

  if (has_devtools) {
    // Create the DevTools browser if it doesn't already exist.
    // Otherwise, focus the existing DevTools browser and inspect the element
    // at |inspect_element_at| if non-empty.
    host->ShowDevTools(windowInfo, client, settings, inspect_element_at);
  }
}

void ClientHandler::CloseDevTools(HoneycombRefPtr<HoneycombBrowser> browser) {
  browser->GetHost()->CloseDevTools();
}

bool ClientHandler::HasSSLInformation(HoneycombRefPtr<HoneycombBrowser> browser) {
  HoneycombRefPtr<HoneycombNavigationEntry> nav =
      browser->GetHost()->GetVisibleNavigationEntry();

  return (nav && nav->GetSSLStatus() &&
          nav->GetSSLStatus()->IsSecureConnection());
}

void ClientHandler::ShowSSLInformation(HoneycombRefPtr<HoneycombBrowser> browser) {
  std::stringstream ss;
  HoneycombRefPtr<HoneycombNavigationEntry> nav =
      browser->GetHost()->GetVisibleNavigationEntry();
  if (!nav) {
    return;
  }

  HoneycombRefPtr<HoneycombSSLStatus> ssl = nav->GetSSLStatus();
  if (!ssl) {
    return;
  }

  ss << "<html><head><title>SSL Information</title></head>"
        "<body bgcolor=\"white\">"
        "<h3>SSL Connection</h3>"
     << "<table border=1><tr><th>Field</th><th>Value</th></tr>";

  HoneycombURLParts urlparts;
  if (HoneycombParseURL(nav->GetURL(), urlparts)) {
    HoneycombString port(&urlparts.port);
    ss << "<tr><td>Server</td><td>" << HoneycombString(&urlparts.host).ToString();
    if (!port.empty()) {
      ss << ":" << port.ToString();
    }
    ss << "</td></tr>";
  }

  ss << "<tr><td>SSL Version</td><td>"
     << GetSSLVersionString(ssl->GetSSLVersion()) << "</td></tr>";
  ss << "<tr><td>Content Status</td><td>"
     << GetContentStatusString(ssl->GetContentStatus()) << "</td></tr>";

  ss << "</table>";

  HoneycombRefPtr<HoneycombX509Certificate> cert = ssl->GetX509Certificate();
  if (cert.get()) {
    ss << GetCertificateInformation(cert, ssl->GetCertStatus());
  }

  ss << "</body></html>";

  auto config = std::make_unique<RootWindowConfig>();
  config->with_controls = false;
  config->with_osr = is_osr_;
  config->url = test_runner::GetDataURI(ss.str(), "text/html");
  MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
      std::move(config));
}

void ClientHandler::SetStringResource(const std::string& page,
                                      const std::string& data) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    HoneycombPostTask(TID_IO, base::BindOnce(&ClientHandler::SetStringResource, this,
                                       page, data));
    return;
  }

  string_resource_map_[page] = data;
}

bool ClientHandler::CreatePopupWindow(HoneycombRefPtr<HoneycombBrowser> browser,
                                      bool is_devtools,
                                      const HoneycombPopupFeatures& popupFeatures,
                                      HoneycombWindowInfo& windowInfo,
                                      HoneycombRefPtr<HoneycombClient>& client,
                                      HoneycombBrowserSettings& settings) {
  HONEYCOMB_REQUIRE_UI_THREAD();

  // The popup browser will be parented to a new native window.
  // Don't show URL bar and navigation buttons on DevTools windows.
  MainContext::Get()->GetRootWindowManager()->CreateRootWindowAsPopup(
      with_controls_ && !is_devtools, is_osr_, popupFeatures, windowInfo,
      client, settings);

  return true;
}

void ClientHandler::NotifyBrowserCreated(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyBrowserCreated, this, browser));
    return;
  }

  if (delegate_) {
    delegate_->OnBrowserCreated(browser);
  }
}

void ClientHandler::NotifyBrowserClosing(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyBrowserClosing, this, browser));
    return;
  }

  if (delegate_) {
    delegate_->OnBrowserClosing(browser);
  }
}

void ClientHandler::NotifyBrowserClosed(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyBrowserClosed, this, browser));
    return;
  }

  if (delegate_) {
    delegate_->OnBrowserClosed(browser);
  }
}

void ClientHandler::NotifyAddress(const HoneycombString& url) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&ClientHandler::NotifyAddress, this, url));
    return;
  }

  if (delegate_) {
    delegate_->OnSetAddress(url);
  }
}

void ClientHandler::NotifyTitle(const HoneycombString& title) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&ClientHandler::NotifyTitle, this, title));
    return;
  }

  if (delegate_) {
    delegate_->OnSetTitle(title);
  }
}

void ClientHandler::NotifyFavicon(HoneycombRefPtr<HoneycombImage> image) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyFavicon, this, image));
    return;
  }

  if (delegate_) {
    delegate_->OnSetFavicon(image);
  }
}

void ClientHandler::NotifyFullscreen(bool fullscreen) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyFullscreen, this, fullscreen));
    return;
  }

  if (delegate_) {
    delegate_->OnSetFullscreen(fullscreen);
  }
}

void ClientHandler::NotifyAutoResize(const HoneycombSize& new_size) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyAutoResize, this, new_size));
    return;
  }

  if (delegate_) {
    delegate_->OnAutoResize(new_size);
  }
}

void ClientHandler::NotifyLoadingState(bool isLoading,
                                       bool canGoBack,
                                       bool canGoForward) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(base::BindOnce(&ClientHandler::NotifyLoadingState, this,
                                     isLoading, canGoBack, canGoForward));
    return;
  }

  if (delegate_) {
    delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
  }
}

void ClientHandler::NotifyDraggableRegions(
    const std::vector<HoneycombDraggableRegion>& regions) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyDraggableRegions, this, regions));
    return;
  }

  if (delegate_) {
    delegate_->OnSetDraggableRegions(regions);
  }
}

void ClientHandler::NotifyTakeFocus(bool next) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    // Execute this method on the main thread.
    MAIN_POST_CLOSURE(
        base::BindOnce(&ClientHandler::NotifyTakeFocus, this, next));
    return;
  }

  if (delegate_) {
    delegate_->OnTakeFocus(next);
  }
}

void ClientHandler::BuildTestMenu(HoneycombRefPtr<HoneycombMenuModel> model) {
  if (model->GetCount() > 0) {
    model->AddSeparator();
  }

  // Build the sub menu.
  HoneycombRefPtr<HoneycombMenuModel> submenu =
      model->AddSubMenu(CLIENT_ID_TESTMENU_SUBMENU, "Context Menu Test");
  submenu->AddCheckItem(CLIENT_ID_TESTMENU_CHECKITEM, "Check Item");
  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM1, "Radio Item 1", 0);
  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM2, "Radio Item 2", 0);
  submenu->AddRadioItem(CLIENT_ID_TESTMENU_RADIOITEM3, "Radio Item 3", 0);

  // Check the check item.
  if (test_menu_state_.check_item) {
    submenu->SetChecked(CLIENT_ID_TESTMENU_CHECKITEM, true);
  }

  // Check the selected radio item.
  submenu->SetChecked(
      CLIENT_ID_TESTMENU_RADIOITEM1 + test_menu_state_.radio_item, true);
}

bool ClientHandler::ExecuteTestMenu(int command_id) {
  if (command_id == CLIENT_ID_TESTMENU_CHECKITEM) {
    // Toggle the check item.
    test_menu_state_.check_item ^= 1;
    return true;
  } else if (command_id >= CLIENT_ID_TESTMENU_RADIOITEM1 &&
             command_id <= CLIENT_ID_TESTMENU_RADIOITEM3) {
    // Store the selected radio item.
    test_menu_state_.radio_item = (command_id - CLIENT_ID_TESTMENU_RADIOITEM1);
    return true;
  }

  // Allow default handling to proceed.
  return false;
}

void ClientHandler::SetOfflineState(HoneycombRefPtr<HoneycombBrowser> browser,
                                    bool offline) {
  // See DevTools protocol docs for message format specification.
  HoneycombRefPtr<HoneycombDictionaryValue> params = HoneycombDictionaryValue::Create();
  params->SetBool("offline", offline);
  params->SetDouble("latency", 0);
  params->SetDouble("downloadThroughput", 0);
  params->SetDouble("uploadThroughput", 0);
  browser->GetHost()->ExecuteDevToolsMethod(
      /*message_id=*/0, "Network.emulateNetworkConditions", params);
}

}  // namespace client
