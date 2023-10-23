// Copyright (c) 2015 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/honeyclient/browser/test_runner.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>

#include "include/base/honey_callback.h"
#include "include/honey_parser.h"
#include "include/honey_task.h"
#include "include/honey_trace.h"
#include "include/views/honey_browser_view.h"
#include "include/wrapper/honey_closure_task.h"
#include "include/wrapper/honey_stream_resource_handler.h"
#include "tests/honeyclient/browser/binding_test.h"
#include "tests/honeyclient/browser/client_handler.h"
#include "tests/honeyclient/browser/dialog_test.h"
#include "tests/honeyclient/browser/main_context.h"
#include "tests/honeyclient/browser/media_router_test.h"
#include "tests/honeyclient/browser/preferences_test.h"
#include "tests/honeyclient/browser/resource.h"
#include "tests/honeyclient/browser/response_filter_test.h"
#include "tests/honeyclient/browser/root_window_manager.h"
#include "tests/honeyclient/browser/scheme_test.h"
#include "tests/honeyclient/browser/server_test.h"
#include "tests/honeyclient/browser/urlrequest_test.h"
#include "tests/honeyclient/browser/window_test.h"
#include "tests/shared/browser/resource_util.h"
#include "tests/shared/common/string_util.h"

namespace client {
namespace test_runner {

namespace {

const char kTestHost[] = "tests";
const char kLocalHost[] = "localhost";
const char kTestOrigin[] = "https://tests/";

// Pages handled via StringResourceProvider.
const char kTestGetSourcePage[] = "get_source.html";
const char kTestGetTextPage[] = "get_text.html";

// Set page data and navigate the browser. Used in combination with
// StringResourceProvider.
void LoadStringResourcePage(HoneycombRefPtr<HoneycombBrowser> browser,
                            const std::string& page,
                            const std::string& data) {
  HoneycombRefPtr<HoneycombClient> client = browser->GetHost()->GetClient();
  ClientHandler* client_handler = static_cast<ClientHandler*>(client.get());
  client_handler->SetStringResource(page, data);
  browser->GetMainFrame()->LoadURL(kTestOrigin + page);
}

void RunGetSourceTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  class Visitor : public HoneycombStringVisitor {
   public:
    explicit Visitor(HoneycombRefPtr<HoneycombBrowser> browser) : browser_(browser) {}
    virtual void Visit(const HoneycombString& string) override {
      std::string source = AsciiStrReplace(string, "<", "&lt;");
      source = AsciiStrReplace(source, ">", "&gt;");
      std::stringstream ss;
      ss << "<html><body bgcolor=\"white\">Source:<pre>" << source
         << "</pre></body></html>";
      LoadStringResourcePage(browser_, kTestGetSourcePage, ss.str());
    }

   private:
    HoneycombRefPtr<HoneycombBrowser> browser_;
    IMPLEMENT_REFCOUNTING(Visitor);
  };

  browser->GetMainFrame()->GetSource(new Visitor(browser));
}

void RunGetTextTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  class Visitor : public HoneycombStringVisitor {
   public:
    explicit Visitor(HoneycombRefPtr<HoneycombBrowser> browser) : browser_(browser) {}
    virtual void Visit(const HoneycombString& string) override {
      std::string text = AsciiStrReplace(string, "<", "&lt;");
      text = AsciiStrReplace(text, ">", "&gt;");
      std::stringstream ss;
      ss << "<html><body bgcolor=\"white\">Text:<pre>" << text
         << "</pre></body></html>";
      LoadStringResourcePage(browser_, kTestGetTextPage, ss.str());
    }

   private:
    HoneycombRefPtr<HoneycombBrowser> browser_;
    IMPLEMENT_REFCOUNTING(Visitor);
  };

  browser->GetMainFrame()->GetText(new Visitor(browser));
}

void RunRequestTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  // Create a new request
  HoneycombRefPtr<HoneycombRequest> request(HoneycombRequest::Create());

  if (browser->GetMainFrame()->GetURL().ToString().find(kTestOrigin) != 0) {
    // The LoadRequest method will fail with "bad IPC message" reason
    // INVALID_INITIATOR_ORIGIN (213) unless you first navigate to the
    // request origin using some other mechanism (LoadURL, link click, etc).
    Alert(browser, "Please first navigate to a " + std::string(kTestOrigin) +
                       " URL. "
                       "For example, first load Tests > Other Tests.");
    return;
  }

  // Set the request URL
  request->SetURL(GetTestURL("request"));

  // Add post data to the request.  The correct method and content-
  // type headers will be set by Honeycomb.
  HoneycombRefPtr<HoneycombPostDataElement> postDataElement(HoneycombPostDataElement::Create());
  std::string data = "arg1=val1&arg2=val2";
  postDataElement->SetToBytes(data.length(), data.c_str());
  HoneycombRefPtr<HoneycombPostData> postData(HoneycombPostData::Create());
  postData->AddElement(postDataElement);
  request->SetPostData(postData);

  // Add a custom header
  HoneycombRequest::HeaderMap headerMap;
  headerMap.insert(std::make_pair("X-My-Header", "My Header Value"));
  request->SetHeaderMap(headerMap);

  // Load the request
  browser->GetMainFrame()->LoadRequest(request);
}

void RunNewWindowTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  auto config = std::make_unique<RootWindowConfig>();
  config->with_controls = true;
  config->with_osr = browser->GetHost()->IsWindowRenderingDisabled();
  MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
      std::move(config));
}

void RunPopupWindowTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  browser->GetMainFrame()->ExecuteJavaScript(
      "window.open('https://www.google.com');", "about:blank", 0);
}

void RunDialogWindowTest(HoneycombRefPtr<HoneycombBrowser> browser) {
  auto browser_view = HoneycombBrowserView::GetForBrowser(browser);
  if (!browser_view) {
    LOG(ERROR) << "Dialog windows require Views";
    return;
  }

  auto config = std::make_unique<RootWindowConfig>();
  config->window_type = WindowType::DIALOG;
  config->parent_window = browser_view->GetWindow();
  MainContext::Get()->GetRootWindowManager()->CreateRootWindow(
      std::move(config));
}

const char kPrompt[] = "Prompt.";
const char kPromptFPS[] = "FPS";
const char kPromptDSF[] = "DSF";

// Handles execution of prompt results.
class PromptHandler : public HoneycombMessageRouterBrowserSide::Handler {
 public:
  PromptHandler() {}

  // Called due to honeyQuery execution.
  virtual bool OnQuery(HoneycombRefPtr<HoneycombBrowser> browser,
                       HoneycombRefPtr<HoneycombFrame> frame,
                       int64_t query_id,
                       const HoneycombString& request,
                       bool persistent,
                       HoneycombRefPtr<Callback> callback) override {
    // Parse |request| which takes the form "Prompt.[type]:[value]".
    const std::string& request_str = request;
    if (request_str.find(kPrompt) != 0) {
      return false;
    }

    std::string type = request_str.substr(sizeof(kPrompt) - 1);
    size_t delim = type.find(':');
    if (delim == std::string::npos) {
      return false;
    }

    const std::string& value = type.substr(delim + 1);
    type = type.substr(0, delim);

    // Canceling the prompt dialog returns a value of "null".
    if (value != "null") {
      if (type == kPromptFPS) {
        SetFPS(browser, atoi(value.c_str()));
      } else if (type == kPromptDSF) {
        SetDSF(browser, static_cast<float>(atof(value.c_str())));
      }
    }

    // Nothing is done with the response.
    callback->Success(HoneycombString());
    return true;
  }

 private:
  void SetFPS(HoneycombRefPtr<HoneycombBrowser> browser, int fps) {
    if (fps <= 0) {
      // Reset to the default value.
      HoneycombBrowserSettings settings;
      MainContext::Get()->PopulateBrowserSettings(&settings);
      fps = settings.windowless_frame_rate;
    }

    browser->GetHost()->SetWindowlessFrameRate(fps);
  }

  void SetDSF(HoneycombRefPtr<HoneycombBrowser> browser, float dsf) {
    MainMessageLoop::Get()->PostClosure(
        base::BindOnce(&PromptHandler::SetDSFOnMainThread, browser, dsf));
  }

  static void SetDSFOnMainThread(HoneycombRefPtr<HoneycombBrowser> browser, float dsf) {
    RootWindow::GetForBrowser(browser->GetIdentifier())
        ->SetDeviceScaleFactor(dsf);
  }
};

void Prompt(HoneycombRefPtr<HoneycombBrowser> browser,
            const std::string& type,
            const std::string& label,
            const std::string& default_value) {
  // Prompt the user for a new value. Works as follows:
  // 1. Show a prompt() dialog via JavaScript.
  // 2. Pass the result to window.honeyQuery().
  // 3. Handle the result in PromptHandler::OnQuery.
  const std::string& code = "window.honeyQuery({'request': '" +
                            std::string(kPrompt) + type + ":' + prompt('" +
                            label + "', '" + default_value + "')});";
  browser->GetMainFrame()->ExecuteJavaScript(
      code, browser->GetMainFrame()->GetURL(), 0);
}

void PromptFPS(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&PromptFPS, browser));
    return;
  }

  // Format the default value string.
  std::stringstream ss;
  ss << browser->GetHost()->GetWindowlessFrameRate();

  Prompt(browser, kPromptFPS, "Enter FPS", ss.str());
}

void PromptDSF(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!MainMessageLoop::Get()->RunsTasksOnCurrentThread()) {
    // Execute on the main thread.
    MainMessageLoop::Get()->PostClosure(base::BindOnce(&PromptDSF, browser));
    return;
  }

  // Format the default value string.
  std::stringstream ss;
  ss << RootWindow::GetForBrowser(browser->GetIdentifier())
            ->GetDeviceScaleFactor();

  Prompt(browser, kPromptDSF, "Enter Device Scale Factor", ss.str());
}

void BeginTracing() {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&BeginTracing));
    return;
  }

  HoneycombBeginTracing(HoneycombString(), nullptr);
}

void EndTracing(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&EndTracing, browser));
    return;
  }

  class Client : public HoneycombEndTracingCallback, public HoneycombRunFileDialogCallback {
   public:
    explicit Client(HoneycombRefPtr<HoneycombBrowser> browser) : browser_(browser) {
      RunDialog();
    }

    void RunDialog() {
      static const char kDefaultFileName[] = "trace.txt";
      std::string path = MainContext::Get()->GetDownloadPath(kDefaultFileName);
      if (path.empty()) {
        path = kDefaultFileName;
      }

      // Results in a call to OnFileDialogDismissed.
      browser_->GetHost()->RunFileDialog(
          FILE_DIALOG_SAVE,
          /*title=*/HoneycombString(), path,
          /*accept_filters=*/std::vector<HoneycombString>(), this);
    }

    void OnFileDialogDismissed(
        const std::vector<HoneycombString>& file_paths) override {
      if (!file_paths.empty()) {
        // File selected. Results in a call to OnEndTracingComplete.
        HoneycombEndTracing(file_paths.front(), this);
      } else {
        // No file selected. Discard the trace data.
        HoneycombEndTracing(HoneycombString(), nullptr);
      }
    }

    void OnEndTracingComplete(const HoneycombString& tracing_file) override {
      Alert(browser_,
            "File \"" + tracing_file.ToString() + "\" saved successfully.");
    }

   private:
    HoneycombRefPtr<HoneycombBrowser> browser_;

    IMPLEMENT_REFCOUNTING(Client);
  };

  new Client(browser);
}

void PrintToPDF(HoneycombRefPtr<HoneycombBrowser> browser) {
  if (!HoneycombCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    HoneycombPostTask(TID_UI, base::BindOnce(&PrintToPDF, browser));
    return;
  }

  class Client : public HoneycombPdfPrintCallback, public HoneycombRunFileDialogCallback {
   public:
    explicit Client(HoneycombRefPtr<HoneycombBrowser> browser) : browser_(browser) {
      RunDialog();
    }

    void RunDialog() {
      static const char kDefaultFileName[] = "output.pdf";
      std::string path = MainContext::Get()->GetDownloadPath(kDefaultFileName);
      if (path.empty()) {
        path = kDefaultFileName;
      }

      std::vector<HoneycombString> accept_filters;
      accept_filters.push_back(".pdf");

      // Results in a call to OnFileDialogDismissed.
      browser_->GetHost()->RunFileDialog(FILE_DIALOG_SAVE,
                                         /*title=*/HoneycombString(), path,
                                         accept_filters, this);
    }

    void OnFileDialogDismissed(
        const std::vector<HoneycombString>& file_paths) override {
      if (!file_paths.empty()) {
        HoneycombPdfPrintSettings settings;

        // Display a header and footer.
        settings.display_header_footer = true;
        HoneycombString(&settings.header_template) =
            "<div style=\"width: 100%; font-size: 9px; position: relative;\">"
            "<div style=\"position: absolute; left: 5px;\">"
            "<span class=\"title\"></span></div>"
            "</div>";
        HoneycombString(&settings.footer_template) =
            "<div style=\"width: 100%; font-size: 9px; position: relative;\">"
            "<div style=\"position: absolute; left: 5px;\">"
            "<span class=\"date\"></span></div>"
            "<div style=\"position: absolute; right: 5px;\">"
            "<span class=\"pageNumber\"></span>/"
            "<span class=\"totalPages\"></span></div>"
            "</div>";

        // Print to the selected PDF file.
        browser_->GetHost()->PrintToPDF(file_paths[0], settings, this);
      }
    }

    void OnPdfPrintFinished(const HoneycombString& path, bool ok) override {
      Alert(browser_, "File \"" + path.ToString() + "\" " +
                          (ok ? "saved successfully." : "failed to save."));
    }

   private:
    HoneycombRefPtr<HoneycombBrowser> browser_;

    IMPLEMENT_REFCOUNTING(Client);
  };

  new Client(browser);
}

void MuteAudio(HoneycombRefPtr<HoneycombBrowser> browser, bool mute) {
  HoneycombRefPtr<HoneycombBrowserHost> host = browser->GetHost();
  host->SetAudioMuted(mute);
}

void RunOtherTests(HoneycombRefPtr<HoneycombBrowser> browser) {
  browser->GetMainFrame()->LoadURL(GetTestURL("other_tests"));
}

// Provider that dumps the request contents.
class RequestDumpResourceProvider : public HoneycombResourceManager::Provider {
 public:
  explicit RequestDumpResourceProvider(const std::string& url) : url_(url) {
    DCHECK(!url.empty());
  }

  bool OnRequest(scoped_refptr<HoneycombResourceManager::Request> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url != url_) {
      // Not handled by this provider.
      return false;
    }

    HoneycombResponse::HeaderMap response_headers;
    HoneycombRefPtr<HoneycombStreamReader> response =
        GetDumpResponse(request->request(), response_headers);

    request->Continue(new HoneycombStreamResourceHandler(200, "OK", "text/html",
                                                   response_headers, response));
    return true;
  }

 private:
  std::string url_;

  DISALLOW_COPY_AND_ASSIGN(RequestDumpResourceProvider);
};

// Provider that returns string data for specific pages. Used in combination
// with LoadStringResourcePage().
class StringResourceProvider : public HoneycombResourceManager::Provider {
 public:
  StringResourceProvider(const std::set<std::string>& pages,
                         StringResourceMap* string_resource_map)
      : pages_(pages), string_resource_map_(string_resource_map) {
    DCHECK(!pages.empty());
  }

  bool OnRequest(scoped_refptr<HoneycombResourceManager::Request> request) override {
    HONEYCOMB_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url.find(kTestOrigin) != 0U) {
      // Not handled by this provider.
      return false;
    }

    const std::string& page = url.substr(strlen(kTestOrigin));
    if (pages_.find(page) == pages_.end()) {
      // Not handled by this provider.
      return false;
    }

    std::string value;
    StringResourceMap::const_iterator it = string_resource_map_->find(page);
    if (it != string_resource_map_->end()) {
      value = it->second;
    } else {
      value = "<html><body>No data available</body></html>";
    }

    HoneycombRefPtr<HoneycombStreamReader> response = HoneycombStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(value.c_str())), value.size());

    request->Continue(new HoneycombStreamResourceHandler(
        200, "OK", "text/html", HoneycombResponse::HeaderMap(), response));
    return true;
  }

 private:
  const std::set<std::string> pages_;

  // Only accessed on the IO thread.
  StringResourceMap* string_resource_map_;

  DISALLOW_COPY_AND_ASSIGN(StringResourceProvider);
};

// Add a file extension to |url| if none is currently specified.
std::string RequestUrlFilter(const std::string& url) {
  if (url.find(kTestOrigin) != 0U) {
    // Don't filter anything outside of the test origin.
    return url;
  }

  // Identify where the query or fragment component, if any, begins.
  size_t suffix_pos = url.find('?');
  if (suffix_pos == std::string::npos) {
    suffix_pos = url.find('#');
  }

  std::string url_base, url_suffix;
  if (suffix_pos == std::string::npos) {
    url_base = url;
  } else {
    url_base = url.substr(0, suffix_pos);
    url_suffix = url.substr(suffix_pos);
  }

  // Identify the last path component.
  size_t path_pos = url_base.rfind('/');
  if (path_pos == std::string::npos) {
    return url;
  }

  const std::string& path_component = url_base.substr(path_pos);

  // Identify if a file extension is currently specified.
  size_t ext_pos = path_component.rfind(".");
  if (ext_pos != std::string::npos) {
    return url;
  }

  // Rebuild the URL with a file extension.
  return url_base + ".html" + url_suffix;
}

}  // namespace

void RunTest(HoneycombRefPtr<HoneycombBrowser> browser, int id) {
  if (!browser) {
    return;
  }

  switch (id) {
    case ID_TESTS_GETSOURCE:
      RunGetSourceTest(browser);
      break;
    case ID_TESTS_GETTEXT:
      RunGetTextTest(browser);
      break;
    case ID_TESTS_WINDOW_NEW:
      RunNewWindowTest(browser);
      break;
    case ID_TESTS_WINDOW_POPUP:
      RunPopupWindowTest(browser);
      break;
    case ID_TESTS_WINDOW_DIALOG:
      RunDialogWindowTest(browser);
      break;
    case ID_TESTS_REQUEST:
      RunRequestTest(browser);
      break;
    case ID_TESTS_ZOOM_IN:
      browser->GetHost()->Zoom(HONEYCOMB_ZOOM_COMMAND_IN);
      break;
    case ID_TESTS_ZOOM_OUT:
      browser->GetHost()->Zoom(HONEYCOMB_ZOOM_COMMAND_OUT);
      break;
    case ID_TESTS_ZOOM_RESET:
      browser->GetHost()->Zoom(HONEYCOMB_ZOOM_COMMAND_RESET);
      break;
    case ID_TESTS_OSR_FPS:
      PromptFPS(browser);
      break;
    case ID_TESTS_OSR_DSF:
      PromptDSF(browser);
      break;
    case ID_TESTS_TRACING_BEGIN:
      BeginTracing();
      break;
    case ID_TESTS_TRACING_END:
      EndTracing(browser);
      break;
    case ID_TESTS_PRINT:
      browser->GetHost()->Print();
      break;
    case ID_TESTS_PRINT_TO_PDF:
      PrintToPDF(browser);
      break;
    case ID_TESTS_MUTE_AUDIO:
      MuteAudio(browser, true);
      break;
    case ID_TESTS_UNMUTE_AUDIO:
      MuteAudio(browser, false);
      break;
    case ID_TESTS_OTHER_TESTS:
      RunOtherTests(browser);
      break;
  }
}

std::string DumpRequestContents(HoneycombRefPtr<HoneycombRequest> request) {
  std::stringstream ss;

  ss << "URL: " << std::string(request->GetURL());
  ss << "\nMethod: " << std::string(request->GetMethod());

  HoneycombRequest::HeaderMap headerMap;
  request->GetHeaderMap(headerMap);
  if (headerMap.size() > 0) {
    ss << "\nHeaders:";
    HoneycombRequest::HeaderMap::const_iterator it = headerMap.begin();
    for (; it != headerMap.end(); ++it) {
      ss << "\n\t" << std::string((*it).first) << ": "
         << std::string((*it).second);
    }
  }

  HoneycombRefPtr<HoneycombPostData> postData = request->GetPostData();
  if (postData.get()) {
    HoneycombPostData::ElementVector elements;
    postData->GetElements(elements);
    if (elements.size() > 0) {
      ss << "\nPost Data:";
      HoneycombRefPtr<HoneycombPostDataElement> element;
      HoneycombPostData::ElementVector::const_iterator it = elements.begin();
      for (; it != elements.end(); ++it) {
        element = (*it);
        if (element->GetType() == PDE_TYPE_BYTES) {
          // the element is composed of bytes
          ss << "\n\tBytes: ";
          if (element->GetBytesCount() == 0) {
            ss << "(empty)";
          } else {
            // retrieve the data.
            size_t size = element->GetBytesCount();
            char* bytes = new char[size];
            element->GetBytes(size, bytes);
            ss << std::string(bytes, size);
            delete[] bytes;
          }
        } else if (element->GetType() == PDE_TYPE_FILE) {
          ss << "\n\tFile: " << std::string(element->GetFile());
        }
      }
    }
  }

  return ss.str();
}

HoneycombRefPtr<HoneycombStreamReader> GetDumpResponse(
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombResponse::HeaderMap& response_headers) {
  std::string origin;

  // Extract the origin request header, if any. It will be specified for
  // cross-origin requests.
  {
    HoneycombRequest::HeaderMap requestMap;
    request->GetHeaderMap(requestMap);

    HoneycombRequest::HeaderMap::const_iterator it = requestMap.begin();
    for (; it != requestMap.end(); ++it) {
      const std::string& key = AsciiStrToLower(it->first);
      if (key == "origin") {
        origin = it->second;
        break;
      }
    }
  }

  if (!origin.empty() &&
      (origin.find("https://" + std::string(kTestHost)) == 0 ||
       origin.find("http://" + std::string(kLocalHost)) == 0)) {
    // Allow cross-origin XMLHttpRequests from test origins.
    response_headers.insert(
        std::make_pair("Access-Control-Allow-Origin", origin));

    // Allow the custom header from the xmlhttprequest.html example.
    response_headers.insert(
        std::make_pair("Access-Control-Allow-Headers", "My-Custom-Header"));
  }

  const std::string& dump = DumpRequestContents(request);
  std::string str =
      "<html><body bgcolor=\"white\"><pre>" + dump + "</pre></body></html>";
  HoneycombRefPtr<HoneycombStreamReader> stream = HoneycombStreamReader::CreateForData(
      static_cast<void*>(const_cast<char*>(str.c_str())), str.size());
  DCHECK(stream);
  return stream;
}

std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         HoneycombURIEncode(HoneycombBase64Encode(data.data(), data.size()), false)
             .ToString();
}

std::string GetErrorString(honey_errorcode_t code) {
// Case condition that returns |code| as a string.
#define CASE(code) \
  case code:       \
    return #code

  switch (code) {
    CASE(ERR_NONE);
    CASE(ERR_FAILED);
    CASE(ERR_ABORTED);
    CASE(ERR_INVALID_ARGUMENT);
    CASE(ERR_INVALID_HANDLE);
    CASE(ERR_FILE_NOT_FOUND);
    CASE(ERR_TIMED_OUT);
    CASE(ERR_FILE_TOO_BIG);
    CASE(ERR_UNEXPECTED);
    CASE(ERR_ACCESS_DENIED);
    CASE(ERR_NOT_IMPLEMENTED);
    CASE(ERR_CONNECTION_CLOSED);
    CASE(ERR_CONNECTION_RESET);
    CASE(ERR_CONNECTION_REFUSED);
    CASE(ERR_CONNECTION_ABORTED);
    CASE(ERR_CONNECTION_FAILED);
    CASE(ERR_NAME_NOT_RESOLVED);
    CASE(ERR_INTERNET_DISCONNECTED);
    CASE(ERR_SSL_PROTOCOL_ERROR);
    CASE(ERR_ADDRESS_INVALID);
    CASE(ERR_ADDRESS_UNREACHABLE);
    CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
    CASE(ERR_TUNNEL_CONNECTION_FAILED);
    CASE(ERR_NO_SSL_VERSIONS_ENABLED);
    CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
    CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
    CASE(ERR_CERT_COMMON_NAME_INVALID);
    CASE(ERR_CERT_DATE_INVALID);
    CASE(ERR_CERT_AUTHORITY_INVALID);
    CASE(ERR_CERT_CONTAINS_ERRORS);
    CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
    CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
    CASE(ERR_CERT_REVOKED);
    CASE(ERR_CERT_INVALID);
    CASE(ERR_CERT_END);
    CASE(ERR_INVALID_URL);
    CASE(ERR_DISALLOWED_URL_SCHEME);
    CASE(ERR_UNKNOWN_URL_SCHEME);
    CASE(ERR_TOO_MANY_REDIRECTS);
    CASE(ERR_UNSAFE_REDIRECT);
    CASE(ERR_UNSAFE_PORT);
    CASE(ERR_INVALID_RESPONSE);
    CASE(ERR_INVALID_CHUNKED_ENCODING);
    CASE(ERR_METHOD_NOT_SUPPORTED);
    CASE(ERR_UNEXPECTED_PROXY_AUTH);
    CASE(ERR_EMPTY_RESPONSE);
    CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
    CASE(ERR_CACHE_MISS);
    CASE(ERR_INSECURE_RESPONSE);
    default:
      return "UNKNOWN";
  }
}

void SetupResourceManager(HoneycombRefPtr<HoneycombResourceManager> resource_manager,
                          StringResourceMap* string_resource_map) {
  if (!HoneycombCurrentlyOn(TID_IO)) {
    // Execute on the browser IO thread.
    HoneycombPostTask(TID_IO, base::BindOnce(SetupResourceManager, resource_manager,
                                       string_resource_map));
    return;
  }

  const std::string& test_origin = kTestOrigin;

  // Add the URL filter.
  resource_manager->SetUrlFilter(base::BindRepeating(RequestUrlFilter));

  // Add provider for resource dumps.
  resource_manager->AddProvider(
      new RequestDumpResourceProvider(test_origin + "request.html"), 0,
      std::string());

  // Set of supported string pages.
  std::set<std::string> string_pages;
  string_pages.insert(kTestGetSourcePage);
  string_pages.insert(kTestGetTextPage);

  if (string_resource_map) {
    // Add provider for string resources.
    resource_manager->AddProvider(
        new StringResourceProvider(string_pages, string_resource_map), 0,
        std::string());
  }

// Add provider for bundled resource files.
#if defined(OS_WIN)
  // Read resources from the binary.
  resource_manager->AddProvider(
      CreateBinaryResourceProvider(test_origin, std::string()), 100,
      std::string());
#elif defined(OS_POSIX)
  // Read resources from a directory on disk.
  std::string resource_dir;
  if (GetResourceDir(resource_dir)) {
    resource_manager->AddDirectoryProvider(test_origin, resource_dir, 100,
                                           std::string());
  }
#endif
}

void Alert(HoneycombRefPtr<HoneycombBrowser> browser, const std::string& message) {
  if (browser->GetHost()->GetExtension()) {
    // Alerts originating from extension hosts should instead be displayed in
    // the active browser.
    browser = MainContext::Get()->GetRootWindowManager()->GetActiveBrowser();
    if (!browser) {
      return;
    }
  }

  // Escape special characters in the message.
  std::string msg = AsciiStrReplace(message, "\\", "\\\\");
  msg = AsciiStrReplace(msg, "'", "\\'");

  // Execute a JavaScript alert().
  HoneycombRefPtr<HoneycombFrame> frame = browser->GetMainFrame();
  frame->ExecuteJavaScript("alert('" + msg + "');", frame->GetURL(), 0);
}

std::string GetTestURL(const std::string& path) {
  return kTestOrigin + path;
}

bool IsTestURL(const std::string& url, const std::string& path) {
  HoneycombURLParts parts;
  HoneycombParseURL(url, parts);

  const std::string& url_host = HoneycombString(&parts.host);
  if (url_host != kTestHost && url_host != kLocalHost) {
    return false;
  }

  const std::string& url_path = HoneycombString(&parts.path);
  return url_path.find(path) == 0;
}

void CreateMessageHandlers(MessageHandlerSet& handlers) {
  handlers.insert(new PromptHandler);

  // Create the binding test handlers.
  binding_test::CreateMessageHandlers(handlers);

  // Create the dialog test handlers.
  dialog_test::CreateMessageHandlers(handlers);

  // Create the media router test handlers.
  media_router_test::CreateMessageHandlers(handlers);

  // Create the preferences test handlers.
  preferences_test::CreateMessageHandlers(handlers);

  // Create the server test handlers.
  server_test::CreateMessageHandlers(handlers);

  // Create the urlrequest test handlers.
  urlrequest_test::CreateMessageHandlers(handlers);

  // Create the window test handlers.
  window_test::CreateMessageHandlers(handlers);
}

void RegisterSchemeHandlers() {
  // Register the scheme handler.
  scheme_test::RegisterSchemeHandlers();
}

HoneycombRefPtr<HoneycombResponseFilter> GetResourceResponseFilter(
    HoneycombRefPtr<HoneycombBrowser> browser,
    HoneycombRefPtr<HoneycombFrame> frame,
    HoneycombRefPtr<HoneycombRequest> request,
    HoneycombRefPtr<HoneycombResponse> response) {
  // Create the response filter.
  return response_filter_test::GetResourceResponseFilter(browser, frame,
                                                         request, response);
}

}  // namespace test_runner
}  // namespace client
