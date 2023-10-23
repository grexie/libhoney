// Copyright 2022 The Honeycomb Authors.
// Portions copyright 2014 The Chromium Authors and 2016 the CEF Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_ALLOY_DIALOGS_ALLOY_CONSTRAINED_WINDOW_VIEWS_CLIENT_H_
#define HONEYCOMB_LIBHONEY_BROWSER_ALLOY_DIALOGS_ALLOY_CONSTRAINED_WINDOW_VIEWS_CLIENT_H_

#include <memory>

#include "components/constrained_window/constrained_window_views_client.h"

// Creates a ConstrainedWindowViewsClient for the Chrome environment.
std::unique_ptr<constrained_window::ConstrainedWindowViewsClient>
CreateAlloyConstrainedWindowViewsClient();

#endif  // HONEYCOMB_LIBHONEY_BROWSER_ALLOY_DIALOGS_ALLOY_CONSTRAINED_WINDOW_VIEWS_CLIENT_H_