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

#ifndef HONEYCOMB_INCLUDE_HONEYCOMB_STREAM_H_
#define HONEYCOMB_INCLUDE_HONEYCOMB_STREAM_H_

#include "include/honey_base.h"

///
/// Interface the client can implement to provide a custom stream reader. The
/// methods of this class may be called on any thread.
///
/*--honey(source=client)--*/
class HoneycombReadHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Read raw binary data.
  ///
  /*--honey()--*/
  virtual size_t Read(void* ptr, size_t size, size_t n) = 0;

  ///
  /// Seek to the specified offset position. |whence| may be any one of
  /// SEEK_CUR, SEEK_END or SEEK_SET. Return zero on success and non-zero on
  /// failure.
  ///
  /*--honey()--*/
  virtual int Seek(int64_t offset, int whence) = 0;

  ///
  /// Return the current offset position.
  ///
  /*--honey()--*/
  virtual int64_t Tell() = 0;

  ///
  /// Return non-zero if at end of file.
  ///
  /*--honey()--*/
  virtual int Eof() = 0;

  ///
  /// Return true if this handler performs work like accessing the file system
  /// which may block. Used as a hint for determining the thread to access the
  /// handler from.
  ///
  /*--honey()--*/
  virtual bool MayBlock() = 0;
};

///
/// Class used to read data from a stream. The methods of this class may be
/// called on any thread.
///
/*--honey(source=library)--*/
class HoneycombStreamReader : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Create a new HoneycombStreamReader object from a file.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombStreamReader> CreateForFile(const HoneycombString& fileName);

  ///
  /// Create a new HoneycombStreamReader object from data.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombStreamReader> CreateForData(void* data, size_t size);

  ///
  /// Create a new HoneycombStreamReader object from a custom handler.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombStreamReader> CreateForHandler(
      HoneycombRefPtr<HoneycombReadHandler> handler);

  ///
  /// Read raw binary data.
  ///
  /*--honey()--*/
  virtual size_t Read(void* ptr, size_t size, size_t n) = 0;

  ///
  /// Seek to the specified offset position. |whence| may be any one of
  /// SEEK_CUR, SEEK_END or SEEK_SET. Returns zero on success and non-zero on
  /// failure.
  ///
  /*--honey()--*/
  virtual int Seek(int64_t offset, int whence) = 0;

  ///
  /// Return the current offset position.
  ///
  /*--honey()--*/
  virtual int64_t Tell() = 0;

  ///
  /// Return non-zero if at end of file.
  ///
  /*--honey()--*/
  virtual int Eof() = 0;

  ///
  /// Returns true if this reader performs work like accessing the file system
  /// which may block. Used as a hint for determining the thread to access the
  /// reader from.
  ///
  /*--honey()--*/
  virtual bool MayBlock() = 0;
};

///
/// Interface the client can implement to provide a custom stream writer. The
/// methods of this class may be called on any thread.
///
/*--honey(source=client)--*/
class HoneycombWriteHandler : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Write raw binary data.
  ///
  /*--honey()--*/
  virtual size_t Write(const void* ptr, size_t size, size_t n) = 0;

  ///
  /// Seek to the specified offset position. |whence| may be any one of
  /// SEEK_CUR, SEEK_END or SEEK_SET. Return zero on success and non-zero on
  /// failure.
  ///
  /*--honey()--*/
  virtual int Seek(int64_t offset, int whence) = 0;

  ///
  /// Return the current offset position.
  ///
  /*--honey()--*/
  virtual int64_t Tell() = 0;

  ///
  /// Flush the stream.
  ///
  /*--honey()--*/
  virtual int Flush() = 0;

  ///
  /// Return true if this handler performs work like accessing the file system
  /// which may block. Used as a hint for determining the thread to access the
  /// handler from.
  ///
  /*--honey()--*/
  virtual bool MayBlock() = 0;
};

///
/// Class used to write data to a stream. The methods of this class may be
/// called on any thread.
///
/*--honey(source=library)--*/
class HoneycombStreamWriter : public virtual HoneycombBaseRefCounted {
 public:
  ///
  /// Create a new HoneycombStreamWriter object for a file.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombStreamWriter> CreateForFile(const HoneycombString& fileName);
  ///
  /// Create a new HoneycombStreamWriter object for a custom handler.
  ///
  /*--honey()--*/
  static HoneycombRefPtr<HoneycombStreamWriter> CreateForHandler(
      HoneycombRefPtr<HoneycombWriteHandler> handler);

  ///
  /// Write raw binary data.
  ///
  /*--honey()--*/
  virtual size_t Write(const void* ptr, size_t size, size_t n) = 0;

  ///
  /// Seek to the specified offset position. |whence| may be any one of
  /// SEEK_CUR, SEEK_END or SEEK_SET. Returns zero on success and non-zero on
  /// failure.
  ///
  /*--honey()--*/
  virtual int Seek(int64_t offset, int whence) = 0;

  ///
  /// Return the current offset position.
  ///
  /*--honey()--*/
  virtual int64_t Tell() = 0;

  ///
  /// Flush the stream.
  ///
  /*--honey()--*/
  virtual int Flush() = 0;

  ///
  /// Returns true if this writer performs work like accessing the file system
  /// which may block. Used as a hint for determining the thread to access the
  /// writer from.
  ///
  /*--honey()--*/
  virtual bool MayBlock() = 0;
};

#endif  // HONEYCOMB_INCLUDE_HONEYCOMB_STREAM_H_
