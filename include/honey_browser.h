// Copyright (c) 2012 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// The contents of this file must follow a specific format in order to
// support the Honeycomb translator tool. See the translator.README.txt file in the
// tools directory for more information.
//

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_H_
#pragma once

#include <vector>
#include "include/honey_base.h"
#include "include/honey_devtools_message_observer.h"
#include "include/honey_drag_data.h"
#include "include/honey_frame.h"
#include "include/honey_image.h"
#include "include/honey_navigation_entry.h"
#include "include/honey_registration.h"
#include "include/honey_request_context.h"

class HoneycombBrowserHost;
class HoneycombClient;

///
/// Class used to represent a browser. When used in the browser process the
/// methods of this class may be called on any thread unless otherwise indicated
/// in the comments. When used in the render process the methods of this class
/// may only be called on the main thread.
///
/*--honey(source=library)--*/
class HoneycombBrowser : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// True if this object is currently valid. This will return false after
  /// HoneycombLifeSpanHandler::OnBeforeClose is called.
  ///
  /*--honey()--*/
  virtual bool IsValid() = 0;

  ///
  /// Returns the browser host object. This method can only be called in the
  /// browser process.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowserHost> GetHost() = 0;

  ///
  /// Returns true if the browser can navigate backwards.
  ///
  /*--honey()--*/
  virtual bool CanGoBack() = 0;

  ///
  /// Navigate backwards.
  ///
  /*--honey()--*/
  virtual void GoBack() = 0;

  ///
  /// Returns true if the browser can navigate forwards.
  ///
  /*--honey()--*/
  virtual bool CanGoForward() = 0;

  ///
  /// Navigate forwards.
  ///
  /*--honey()--*/
  virtual void GoForward() = 0;

  ///
  /// Returns true if the browser is currently loading.
  ///
  /*--honey()--*/
  virtual bool IsLoading() = 0;

  ///
  /// Reload the current page.
  ///
  /*--honey()--*/
  virtual void Reload() = 0;

  ///
  /// Reload the current page ignoring any cached data.
  ///
  /*--honey()--*/
  virtual void ReloadIgnoreCache() = 0;

  ///
  /// Stop loading the page.
  ///
  /*--honey()--*/
  virtual void StopLoad() = 0;

  ///
  /// Returns the globally unique identifier for this browser. This value is
  /// also used as the tabId for extension APIs.
  ///
  /*--honey()--*/
  virtual int GetIdentifier() = 0;

  ///
  /// Returns true if this object is pointing to the same handle as |that|
  /// object.
  ///
  /*--honey()--*/
  virtual bool IsSame(HoneycombRefPtr<HoneycombBrowser> that) = 0;

  ///
  /// Returns true if the browser is a popup.
  ///
  /*--honey()--*/
  virtual bool IsPopup() = 0;

  ///
  /// Returns true if a document has been loaded in the browser.
  ///
  /*--honey()--*/
  virtual bool HasDocument() = 0;

  ///
  /// Returns the main (top-level) frame for the browser. In the browser process
  /// this will return a valid object until after
  /// HoneycombLifeSpanHandler::OnBeforeClose is called. In the renderer process this
  /// will return NULL if the main frame is hosted in a different renderer
  /// process (e.g. for cross-origin sub-frames). The main frame object will
  /// change during cross-origin navigation or re-navigation after renderer
  /// process termination (due to crashes, etc).
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetMainFrame() = 0;

  ///
  /// Returns the focused frame for the browser.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetFocusedFrame() = 0;

  ///
  /// Returns the frame with the specified identifier, or NULL if not found.
  ///
  /*--honey(capi_name=get_frame_byident)--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetFrame(int64_t identifier) = 0;

  ///
  /// Returns the frame with the specified name, or NULL if not found.
  ///
  /*--honey(optional_param=name)--*/
  virtual HoneycombRefPtr<HoneycombFrame> GetFrame(const HoneycombString& name) = 0;

  ///
  /// Returns the number of frames that currently exist.
  ///
  /*--honey()--*/
  virtual size_t GetFrameCount() = 0;

  ///
  /// Returns the identifiers of all existing frames.
  ///
  /*--honey(count_func=identifiers:GetFrameCount)--*/
  virtual void GetFrameIdentifiers(std::vector<int64_t>& identifiers) = 0;

  ///
  /// Returns the names of all existing frames.
  ///
  /*--honey()--*/
  virtual void GetFrameNames(std::vector<HoneycombString>& names) = 0;
};

///
/// Callback interface for HoneycombBrowserHost::RunFileDialog. The methods of this
/// class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombRunFileDialogCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Called asynchronously after the file dialog is dismissed.
  /// |file_paths| will be a single value or a list of values depending on the
  /// dialog mode. If the selection was cancelled |file_paths| will be empty.
  ///
  /*--honey(optional_param=file_paths)--*/
  virtual void OnFileDialogDismissed(
      const std::vector<HoneycombString>& file_paths) = 0;
};

///
/// Callback interface for HoneycombBrowserHost::GetNavigationEntries. The methods of
/// this class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombNavigationEntryVisitor : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method that will be executed. Do not keep a reference to |entry| outside
  /// of this callback. Return true to continue visiting entries or false to
  /// stop. |current| is true if this entry is the currently loaded navigation
  /// entry. |index| is the 0-based index of this entry and |total| is the total
  /// number of entries.
  ///
  /*--honey()--*/
  virtual bool Visit(HoneycombRefPtr<HoneycombNavigationEntry> entry,
                     bool current,
                     int index,
                     int total) = 0;
};

///
/// Callback interface for HoneycombBrowserHost::PrintToPDF. The methods of this class
/// will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombPdfPrintCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method that will be executed when the PDF printing has completed. |path|
  /// is the output path. |ok| will be true if the printing completed
  /// successfully or false otherwise.
  ///
  /*--honey()--*/
  virtual void OnPdfPrintFinished(const HoneycombString& path, bool ok) = 0;
};

///
/// Callback interface for HoneycombBrowserHost::DownloadImage. The methods of this
/// class will be called on the browser process UI thread.
///
/*--honey(source=client)--*/
class HoneycombDownloadImageCallback : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Method that will be executed when the image download has completed.
  /// |image_url| is the URL that was downloaded and |http_status_code| is the
  /// resulting HTTP status code. |image| is the resulting image, possibly at
  /// multiple scale factors, or empty if the download failed.
  ///
  /*--honey(optional_param=image)--*/
  virtual void OnDownloadImageFinished(const HoneycombString& image_url,
                                       int http_status_code,
                                       HoneycombRefPtr<HoneycombImage> image) = 0;
};

///
/// Class used to represent the browser process aspects of a browser. The
/// methods of this class can only be called in the browser process. They may be
/// called on any thread in that process unless otherwise indicated in the
/// comments.
///
/*--honey(source=library)--*/
class HoneycombBrowserHost : public virtual HoneycombBaseRefCounted {
 public:
  typedef honey_drag_operations_mask_t DragOperationsMask;
  typedef honey_file_dialog_mode_t FileDialogMode;
  typedef honey_mouse_button_type_t MouseButtonType;
  typedef honey_paint_element_type_t PaintElementType;

  ///
  /// Create a new browser using the window parameters specified by
  /// |windowInfo|. All values will be copied internally and the actual window
  /// (if any) will be created on the UI thread. If |request_context| is empty
  /// the global request context will be used. This method can be called on any
  /// browser process thread and will not block. The optional |extra_info|
  /// parameter provides an opportunity to specify extra information specific to
  /// the created browser that will be passed to
  /// HoneycombRenderProcessHandler::OnBrowserCreated() in the render process.
  ///
  /*--honey(optional_param=client,optional_param=url,
          optional_param=request_context,optional_param=extra_info)--*/
  static bool CreateBrowser(const HoneycombWindowInfo& windowInfo,
                            HoneycombRefPtr<HoneycombClient> client,
                            const HoneycombString& url,
                            const HoneycombBrowserSettings& settings,
                            HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
                            HoneycombRefPtr<HoneycombRequestContext> request_context);

  ///
  /// Create a new browser using the window parameters specified by
  /// |windowInfo|. If |request_context| is empty the global request context
  /// will be used. This method can only be called on the browser process UI
  /// thread. The optional |extra_info| parameter provides an opportunity to
  /// specify extra information specific to the created browser that will be
  /// passed to HoneycombRenderProcessHandler::OnBrowserCreated() in the render
  /// process.
  ///
  /*--honey(optional_param=client,optional_param=url,
          optional_param=request_context,optional_param=extra_info)--*/
  static HoneycombRefPtr<HoneycombBrowser> CreateBrowserSync(
      const HoneycombWindowInfo& windowInfo,
      HoneycombRefPtr<HoneycombClient> client,
      const HoneycombString& url,
      const HoneycombBrowserSettings& settings,
      HoneycombRefPtr<HoneycombDictionaryValue> extra_info,
      HoneycombRefPtr<HoneycombRequestContext> request_context);

  ///
  /// Returns the hosted browser object.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombBrowser> GetBrowser() = 0;

  ///
  /// Request that the browser close. The JavaScript 'onbeforeunload' event will
  /// be fired. If |force_close| is false the event handler, if any, will be
  /// allowed to prompt the user and the user can optionally cancel the close.
  /// If |force_close| is true the prompt will not be displayed and the close
  /// will proceed. Results in a call to HoneycombLifeSpanHandler::DoClose() if the
  /// event handler allows the close or if |force_close| is true. See
  /// HoneycombLifeSpanHandler::DoClose() documentation for additional usage
  /// information.
  ///
  /*--honey()--*/
  virtual void CloseBrowser(bool force_close) = 0;

  ///
  /// Helper for closing a browser. Call this method from the top-level window
  /// close handler (if any). Internally this calls CloseBrowser(false) if the
  /// close has not yet been initiated. This method returns false while the
  /// close is pending and true after the close has completed. See
  /// CloseBrowser() and HoneycombLifeSpanHandler::DoClose() documentation for
  /// additional usage information. This method must be called on the browser
  /// process UI thread.
  ///
  /*--honey()--*/
  virtual bool TryCloseBrowser() = 0;

  ///
  /// Set whether the browser is focused.
  ///
  /*--honey()--*/
  virtual void SetFocus(bool focus) = 0;

  ///
  /// Retrieve the window handle (if any) for this browser. If this browser is
  /// wrapped in a HoneycombBrowserView this method should be called on the browser
  /// process UI thread and it will return the handle for the top-level native
  /// window.
  ///
  /*--honey()--*/
  virtual HoneycombWindowHandle GetWindowHandle() = 0;

  ///
  /// Retrieve the window handle (if any) of the browser that opened this
  /// browser. Will return NULL for non-popup browsers or if this browser is
  /// wrapped in a HoneycombBrowserView. This method can be used in combination with
  /// custom handling of modal windows.
  ///
  /*--honey()--*/
  virtual HoneycombWindowHandle GetOpenerWindowHandle() = 0;

  ///
  /// Returns true if this browser is wrapped in a HoneycombBrowserView.
  ///
  /*--honey()--*/
  virtual bool HasView() = 0;

  ///
  /// Returns the client for this browser.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombClient> GetClient() = 0;

  ///
  /// Returns the request context for this browser.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRequestContext> GetRequestContext() = 0;

  ///
  /// Returns true if this browser can execute the specified zoom command. This
  /// method can only be called on the UI thread.
  ///
  /*--honey()--*/
  virtual bool CanZoom(honey_zoom_command_t command) = 0;

  ///
  /// Execute a zoom command in this browser. If called on the UI thread the
  /// change will be applied immediately. Otherwise, the change will be applied
  /// asynchronously on the UI thread.
  ///
  /*--honey()--*/
  virtual void Zoom(honey_zoom_command_t command) = 0;

  ///
  /// Get the default zoom level. This value will be 0.0 by default but can be
  /// configured with the Chrome runtime. This method can only be called on the
  /// UI thread.
  ///
  /*--honey()--*/
  virtual double GetDefaultZoomLevel() = 0;

  ///
  /// Get the current zoom level. This method can only be called on the UI
  /// thread.
  ///
  /*--honey()--*/
  virtual double GetZoomLevel() = 0;

  ///
  /// Change the zoom level to the specified value. Specify 0.0 to reset the
  /// zoom level to the default. If called on the UI thread the change will be
  /// applied immediately. Otherwise, the change will be applied asynchronously
  /// on the UI thread.
  ///
  /*--honey()--*/
  virtual void SetZoomLevel(double zoomLevel) = 0;

  ///
  /// Call to run a file chooser dialog. Only a single file chooser dialog may
  /// be pending at any given time. |mode| represents the type of dialog to
  /// display. |title| to the title to be used for the dialog and may be empty
  /// to show the default title ("Open" or "Save" depending on the mode).
  /// |default_file_path| is the path with optional directory and/or file name
  /// component that will be initially selected in the dialog. |accept_filters|
  /// are used to restrict the selectable file types and may any combination of
  /// (a) valid lower-cased MIME types (e.g. "text/*" or "image/*"), (b)
  /// individual file extensions (e.g.
  /// ".txt" or ".png"), or (c) combined description and file extension
  /// delimited using "|" and ";" (e.g. "Image Types|.png;.gif;.jpg").
  /// |callback| will be executed after the dialog is dismissed or immediately
  /// if another dialog is already pending. The dialog will be initiated
  /// asynchronously on the UI thread.
  ///
  /*--honey(optional_param=title,optional_param=default_file_path,
          optional_param=accept_filters)--*/
  virtual void RunFileDialog(FileDialogMode mode,
                             const HoneycombString& title,
                             const HoneycombString& default_file_path,
                             const std::vector<HoneycombString>& accept_filters,
                             HoneycombRefPtr<HoneycombRunFileDialogCallback> callback) = 0;

  ///
  /// Download the file at |url| using HoneycombDownloadHandler.
  ///
  /*--honey()--*/
  virtual void StartDownload(const HoneycombString& url) = 0;

  ///
  /// Download |image_url| and execute |callback| on completion with the images
  /// received from the renderer. If |is_favicon| is true then cookies are not
  /// sent and not accepted during download. Images with density independent
  /// pixel (DIP) sizes larger than |max_image_size| are filtered out from the
  /// image results. Versions of the image at different scale factors may be
  /// downloaded up to the maximum scale factor supported by the system. If
  /// there are no image results <= |max_image_size| then the smallest image is
  /// resized to |max_image_size| and is the only result. A |max_image_size| of
  /// 0 means unlimited. If |bypass_cache| is true then |image_url| is requested
  /// from the server even if it is present in the browser cache.
  ///
  /*--honey()--*/
  virtual void DownloadImage(const HoneycombString& image_url,
                             bool is_favicon,
                             uint32_t max_image_size,
                             bool bypass_cache,
                             HoneycombRefPtr<HoneycombDownloadImageCallback> callback) = 0;

  ///
  /// Print the current browser contents.
  ///
  /*--honey()--*/
  virtual void Print() = 0;

  ///
  /// Print the current browser contents to the PDF file specified by |path| and
  /// execute |callback| on completion. The caller is responsible for deleting
  /// |path| when done. For PDF printing to work on Linux you must implement the
  /// HoneycombPrintHandler::GetPdfPaperSize method.
  ///
  /*--honey(optional_param=callback)--*/
  virtual void PrintToPDF(const HoneycombString& path,
                          const HoneycombPdfPrintSettings& settings,
                          HoneycombRefPtr<HoneycombPdfPrintCallback> callback) = 0;

  ///
  /// Search for |searchText|. |forward| indicates whether to search forward or
  /// backward within the page. |matchCase| indicates whether the search should
  /// be case-sensitive. |findNext| indicates whether this is the first request
  /// or a follow-up. The search will be restarted if |searchText| or
  /// |matchCase| change. The search will be stopped if |searchText| is empty.
  /// The HoneycombFindHandler instance, if any, returned via
  /// HoneycombClient::GetFindHandler will be called to report find results.
  ///
  /*--honey()--*/
  virtual void Find(const HoneycombString& searchText,
                    bool forward,
                    bool matchCase,
                    bool findNext) = 0;

  ///
  /// Cancel all searches that are currently going on.
  ///
  /*--honey()--*/
  virtual void StopFinding(bool clearSelection) = 0;

  ///
  /// Open developer tools (DevTools) in its own browser. The DevTools browser
  /// will remain associated with this browser. If the DevTools browser is
  /// already open then it will be focused, in which case the |windowInfo|,
  /// |client| and |settings| parameters will be ignored. If
  /// |inspect_element_at| is non-empty then the element at the specified (x,y)
  /// location will be inspected. The |windowInfo| parameter will be ignored if
  /// this browser is wrapped in a HoneycombBrowserView.
  ///
  /*--honey(optional_param=windowInfo,optional_param=client,
          optional_param=settings,optional_param=inspect_element_at)--*/
  virtual void ShowDevTools(const HoneycombWindowInfo& windowInfo,
                            HoneycombRefPtr<HoneycombClient> client,
                            const HoneycombBrowserSettings& settings,
                            const HoneycombPoint& inspect_element_at) = 0;

  ///
  /// Explicitly close the associated DevTools browser, if any.
  ///
  /*--honey()--*/
  virtual void CloseDevTools() = 0;

  ///
  /// Returns true if this browser currently has an associated DevTools browser.
  /// Must be called on the browser process UI thread.
  ///
  /*--honey()--*/
  virtual bool HasDevTools() = 0;

  ///
  /// Send a method call message over the DevTools protocol. |message| must be a
  /// UTF8-encoded JSON dictionary that contains "id" (int), "method" (string)
  /// and "params" (dictionary, optional) values. See the DevTools protocol
  /// documentation at https://chromedevtools.github.io/devtools-protocol/ for
  /// details of supported methods and the expected "params" dictionary
  /// contents. |message| will be copied if necessary. This method will return
  /// true if called on the UI thread and the message was successfully submitted
  /// for validation, otherwise false. Validation will be applied asynchronously
  /// and any messages that fail due to formatting errors or missing parameters
  /// may be discarded without notification. Prefer ExecuteDevToolsMethod if a
  /// more structured approach to message formatting is desired.
  ///
  /// Every valid method call will result in an asynchronous method result or
  /// error message that references the sent message "id". Event messages are
  /// received while notifications are enabled (for example, between method
  /// calls for "Page.enable" and "Page.disable"). All received messages will be
  /// delivered to the observer(s) registered with AddDevToolsMessageObserver.
  /// See HoneycombDevToolsMessageObserver::OnDevToolsMessage documentation for
  /// details of received message contents.
  ///
  /// Usage of the SendDevToolsMessage, ExecuteDevToolsMethod and
  /// AddDevToolsMessageObserver methods does not require an active DevTools
  /// front-end or remote-debugging session. Other active DevTools sessions will
  /// continue to function independently. However, any modification of global
  /// browser state by one session may not be reflected in the UI of other
  /// sessions.
  ///
  /// Communication with the DevTools front-end (when displayed) can be logged
  /// for development purposes by passing the
  /// `--devtools-protocol-log-file=<path>` command-line flag.
  ///
  /*--honey()--*/
  virtual bool SendDevToolsMessage(const void* message,
                                   size_t message_size) = 0;

  ///
  /// Execute a method call over the DevTools protocol. This is a more
  /// structured version of SendDevToolsMessage. |message_id| is an incremental
  /// number that uniquely identifies the message (pass 0 to have the next
  /// number assigned automatically based on previous values). |method| is the
  /// method name. |params| are the method parameters, which may be empty. See
  /// the DevTools protocol documentation (linked above) for details of
  /// supported methods and the expected |params| dictionary contents. This
  /// method will return the assigned message ID if called on the UI thread and
  /// the message was successfully submitted for validation, otherwise 0. See
  /// the SendDevToolsMessage documentation for additional usage information.
  ///
  /*--honey(optional_param=params)--*/
  virtual int ExecuteDevToolsMethod(int message_id,
                                    const HoneycombString& method,
                                    HoneycombRefPtr<HoneycombDictionaryValue> params) = 0;

  ///
  /// Add an observer for DevTools protocol messages (method results and
  /// events). The observer will remain registered until the returned
  /// Registration object is destroyed. See the SendDevToolsMessage
  /// documentation for additional usage information.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombRegistration> AddDevToolsMessageObserver(
      HoneycombRefPtr<HoneycombDevToolsMessageObserver> observer) = 0;

  ///
  /// Retrieve a snapshot of current navigation entries as values sent to the
  /// specified visitor. If |current_only| is true only the current navigation
  /// entry will be sent, otherwise all navigation entries will be sent.
  ///
  /*--honey()--*/
  virtual void GetNavigationEntries(
      HoneycombRefPtr<HoneycombNavigationEntryVisitor> visitor,
      bool current_only) = 0;

  ///
  /// If a misspelled word is currently selected in an editable node calling
  /// this method will replace it with the specified |word|.
  ///
  /*--honey()--*/
  virtual void ReplaceMisspelling(const HoneycombString& word) = 0;

  ///
  /// Add the specified |word| to the spelling dictionary.
  ///
  /*--honey()--*/
  virtual void AddWordToDictionary(const HoneycombString& word) = 0;

  ///
  /// Returns true if window rendering is disabled.
  ///
  /*--honey()--*/
  virtual bool IsWindowRenderingDisabled() = 0;

  ///
  /// Notify the browser that the widget has been resized. The browser will
  /// first call HoneycombRenderHandler::GetViewRect to get the new size and then call
  /// HoneycombRenderHandler::OnPaint asynchronously with the updated regions. This
  /// method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void WasResized() = 0;

  ///
  /// Notify the browser that it has been hidden or shown. Layouting and
  /// HoneycombRenderHandler::OnPaint notification will stop when the browser is
  /// hidden. This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void WasHidden(bool hidden) = 0;

  ///
  /// Send a notification to the browser that the screen info has changed. The
  /// browser will then call HoneycombRenderHandler::GetScreenInfo to update the
  /// screen information with the new values. This simulates moving the webview
  /// window from one display to another, or changing the properties of the
  /// current display. This method is only used when window rendering is
  /// disabled.
  ///
  /*--honey()--*/
  virtual void NotifyScreenInfoChanged() = 0;

  ///
  /// Invalidate the view. The browser will call HoneycombRenderHandler::OnPaint
  /// asynchronously. This method is only used when window rendering is
  /// disabled.
  ///
  /*--honey()--*/
  virtual void Invalidate(PaintElementType type) = 0;

  ///
  /// Issue a BeginFrame request to Chromium.  Only valid when
  /// HoneycombWindowInfo::external_begin_frame_enabled is set to true.
  ///
  /*--honey()--*/
  virtual void SendExternalBeginFrame() = 0;

  ///
  /// Send a key event to the browser.
  ///
  /*--honey()--*/
  virtual void SendKeyEvent(const HoneycombKeyEvent& event) = 0;

  ///
  /// Send a mouse click event to the browser. The |x| and |y| coordinates are
  /// relative to the upper-left corner of the view.
  ///
  /*--honey()--*/
  virtual void SendMouseClickEvent(const HoneycombMouseEvent& event,
                                   MouseButtonType type,
                                   bool mouseUp,
                                   int clickCount) = 0;

  ///
  /// Send a mouse move event to the browser. The |x| and |y| coordinates are
  /// relative to the upper-left corner of the view.
  ///
  /*--honey()--*/
  virtual void SendMouseMoveEvent(const HoneycombMouseEvent& event,
                                  bool mouseLeave) = 0;

  ///
  /// Send a mouse wheel event to the browser. The |x| and |y| coordinates are
  /// relative to the upper-left corner of the view. The |deltaX| and |deltaY|
  /// values represent the movement delta in the X and Y directions
  /// respectively. In order to scroll inside select popups with window
  /// rendering disabled HoneycombRenderHandler::GetScreenPoint should be implemented
  /// properly.
  ///
  /*--honey()--*/
  virtual void SendMouseWheelEvent(const HoneycombMouseEvent& event,
                                   int deltaX,
                                   int deltaY) = 0;

  ///
  /// Send a touch event to the browser for a windowless browser.
  ///
  /*--honey()--*/
  virtual void SendTouchEvent(const HoneycombTouchEvent& event) = 0;

  ///
  /// Send a capture lost event to the browser.
  ///
  /*--honey()--*/
  virtual void SendCaptureLostEvent() = 0;

  ///
  /// Notify the browser that the window hosting it is about to be moved or
  /// resized. This method is only used on Windows and Linux.
  ///
  /*--honey()--*/
  virtual void NotifyMoveOrResizeStarted() = 0;

  ///
  /// Returns the maximum rate in frames per second (fps) that
  /// HoneycombRenderHandler::OnPaint will be called for a windowless browser. The
  /// actual fps may be lower if the browser cannot generate frames at the
  /// requested rate. The minimum value is 1 and the maximum value is 60
  /// (default 30). This method can only be called on the UI thread.
  ///
  /*--honey()--*/
  virtual int GetWindowlessFrameRate() = 0;

  ///
  /// Set the maximum rate in frames per second (fps) that HoneycombRenderHandler::
  /// OnPaint will be called for a windowless browser. The actual fps may be
  /// lower if the browser cannot generate frames at the requested rate. The
  /// minimum value is 1 and the maximum value is 60 (default 30). Can also be
  /// set at browser creation via HoneycombBrowserSettings.windowless_frame_rate.
  ///
  /*--honey()--*/
  virtual void SetWindowlessFrameRate(int frame_rate) = 0;

  ///
  /// Begins a new composition or updates the existing composition. Blink has a
  /// special node (a composition node) that allows the input method to change
  /// text without affecting other DOM nodes. |text| is the optional text that
  /// will be inserted into the composition node. |underlines| is an optional
  /// set of ranges that will be underlined in the resulting text.
  /// |replacement_range| is an optional range of the existing text that will be
  /// replaced. |selection_range| is an optional range of the resulting text
  /// that will be selected after insertion or replacement. The
  /// |replacement_range| value is only used on OS X.
  ///
  /// This method may be called multiple times as the composition changes. When
  /// the client is done making changes the composition should either be
  /// canceled or completed. To cancel the composition call
  /// ImeCancelComposition. To complete the composition call either
  /// ImeCommitText or ImeFinishComposingText. Completion is usually signaled
  /// when:
  ///
  /// 1. The client receives a WM_IME_COMPOSITION message with a GCS_RESULTSTR
  ///    flag (on Windows), or;
  /// 2. The client receives a "commit" signal of GtkIMContext (on Linux), or;
  /// 3. insertText of NSTextInput is called (on Mac).
  ///
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey(optional_param=text, optional_param=underlines)--*/
  virtual void ImeSetComposition(
      const HoneycombString& text,
      const std::vector<HoneycombCompositionUnderline>& underlines,
      const HoneycombRange& replacement_range,
      const HoneycombRange& selection_range) = 0;

  ///
  /// Completes the existing composition by optionally inserting the specified
  /// |text| into the composition node. |replacement_range| is an optional range
  /// of the existing text that will be replaced. |relative_cursor_pos| is where
  /// the cursor will be positioned relative to the current cursor position. See
  /// comments on ImeSetComposition for usage. The |replacement_range| and
  /// |relative_cursor_pos| values are only used on OS X.
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey(optional_param=text)--*/
  virtual void ImeCommitText(const HoneycombString& text,
                             const HoneycombRange& replacement_range,
                             int relative_cursor_pos) = 0;

  ///
  /// Completes the existing composition by applying the current composition
  /// node contents. If |keep_selection| is false the current selection, if any,
  /// will be discarded. See comments on ImeSetComposition for usage. This
  /// method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void ImeFinishComposingText(bool keep_selection) = 0;

  ///
  /// Cancels the existing composition and discards the composition node
  /// contents without applying them. See comments on ImeSetComposition for
  /// usage.
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void ImeCancelComposition() = 0;

  ///
  /// Call this method when the user drags the mouse into the web view (before
  /// calling DragTargetDragOver/DragTargetLeave/DragTargetDrop).
  /// |drag_data| should not contain file contents as this type of data is not
  /// allowed to be dragged into the web view. File contents can be removed
  /// using HoneycombDragData::ResetFileContents (for example, if |drag_data| comes
  /// from HoneycombRenderHandler::StartDragging). This method is only used when
  /// window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragTargetDragEnter(HoneycombRefPtr<HoneycombDragData> drag_data,
                                   const HoneycombMouseEvent& event,
                                   DragOperationsMask allowed_ops) = 0;

  ///
  /// Call this method each time the mouse is moved across the web view during
  /// a drag operation (after calling DragTargetDragEnter and before calling
  /// DragTargetDragLeave/DragTargetDrop).
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragTargetDragOver(const HoneycombMouseEvent& event,
                                  DragOperationsMask allowed_ops) = 0;

  ///
  /// Call this method when the user drags the mouse out of the web view (after
  /// calling DragTargetDragEnter).
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragTargetDragLeave() = 0;

  ///
  /// Call this method when the user completes the drag operation by dropping
  /// the object onto the web view (after calling DragTargetDragEnter).
  /// The object being dropped is |drag_data|, given as an argument to
  /// the previous DragTargetDragEnter call.
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragTargetDrop(const HoneycombMouseEvent& event) = 0;

  ///
  /// Call this method when the drag operation started by a
  /// HoneycombRenderHandler::StartDragging call has ended either in a drop or
  /// by being cancelled. |x| and |y| are mouse coordinates relative to the
  /// upper-left corner of the view. If the web view is both the drag source
  /// and the drag target then all DragTarget* methods should be called before
  /// DragSource* mthods.
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragSourceEndedAt(int x, int y, DragOperationsMask op) = 0;

  ///
  /// Call this method when the drag operation started by a
  /// HoneycombRenderHandler::StartDragging call has completed. This method may be
  /// called immediately without first calling DragSourceEndedAt to cancel a
  /// drag operation. If the web view is both the drag source and the drag
  /// target then all DragTarget* methods should be called before DragSource*
  /// mthods.
  /// This method is only used when window rendering is disabled.
  ///
  /*--honey()--*/
  virtual void DragSourceSystemDragEnded() = 0;

  ///
  /// Returns the current visible navigation entry for this browser. This method
  /// can only be called on the UI thread.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombNavigationEntry> GetVisibleNavigationEntry() = 0;

  ///
  /// Set accessibility state for all frames. |accessibility_state| may be
  /// default, enabled or disabled. If |accessibility_state| is STATE_DEFAULT
  /// then accessibility will be disabled by default and the state may be
  /// further controlled with the "force-renderer-accessibility" and
  /// "disable-renderer-accessibility" command-line switches. If
  /// |accessibility_state| is STATE_ENABLED then accessibility will be enabled.
  /// If |accessibility_state| is STATE_DISABLED then accessibility will be
  /// completely disabled.
  ///
  /// For windowed browsers accessibility will be enabled in Complete mode
  /// (which corresponds to kAccessibilityModeComplete in Chromium). In this
  /// mode all platform accessibility objects will be created and managed by
  /// Chromium's internal implementation. The client needs only to detect the
  /// screen reader and call this method appropriately. For example, on macOS
  /// the client can handle the @"AXEnhancedUserInterface" accessibility
  /// attribute to detect VoiceOver state changes and on Windows the client can
  /// handle WM_GETOBJECT with OBJID_CLIENT to detect accessibility readers.
  ///
  /// For windowless browsers accessibility will be enabled in TreeOnly mode
  /// (which corresponds to kAccessibilityModeWebContentsOnly in Chromium). In
  /// this mode renderer accessibility is enabled, the full tree is computed,
  /// and events are passed to HoneycombAccessibiltyHandler, but platform
  /// accessibility objects are not created. The client may implement platform
  /// accessibility objects using HoneycombAccessibiltyHandler callbacks if desired.
  ///
  /*--honey()--*/
  virtual void SetAccessibilityState(honey_state_t accessibility_state) = 0;

  ///
  /// Enable notifications of auto resize via HoneycombDisplayHandler::OnAutoResize.
  /// Notifications are disabled by default. |min_size| and |max_size| define
  /// the range of allowed sizes.
  ///
  /*--honey()--*/
  virtual void SetAutoResizeEnabled(bool enabled,
                                    const HoneycombSize& min_size,
                                    const HoneycombSize& max_size) = 0;

  ///
  /// Returns the extension hosted in this browser or NULL if no extension is
  /// hosted. See HoneycombRequestContext::LoadExtension for details.
  ///
  /*--honey()--*/
  virtual HoneycombRefPtr<HoneycombExtension> GetExtension() = 0;

  ///
  /// Returns true if this browser is hosting an extension background script.
  /// Background hosts do not have a window and are not displayable. See
  /// HoneycombRequestContext::LoadExtension for details.
  ///
  /*--honey()--*/
  virtual bool IsBackgroundHost() = 0;

  ///
  /// Set whether the browser's audio is muted.
  ///
  /*--honey()--*/
  virtual void SetAudioMuted(bool mute) = 0;

  ///
  /// Returns true if the browser's audio is muted.  This method can only be
  /// called on the UI thread.
  ///
  /*--honey()--*/
  virtual bool IsAudioMuted() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_BROWSER_H_
