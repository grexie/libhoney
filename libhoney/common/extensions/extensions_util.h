// Copyright 2015 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_EXTENSIONS_UTIL_H_
#define HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_EXTENSIONS_UTIL_H_

namespace extensions {

// Returns true if extensions have not been disabled via the command-line.
bool ExtensionsEnabled();

// Returns true if the PDF extension has not been disabled via the command-line.
bool PdfExtensionEnabled();

// Returns true if Print Preview has been enabled via the command-line.
bool PrintPreviewEnabled();

}  // namespace extensions

#endif  // HONEYCOMB_LIBHONEY_COMMON_EXTENSIONS_EXTENSIONS_UTIL_H_
