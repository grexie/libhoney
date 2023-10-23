// Copyright 2021 The Honeycomb Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_VIEWS_H_
#define HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_VIEWS_H_
#pragma once

namespace views {
class View;
}

namespace honey {

// Returns true if |view| is a HoneycombView.
bool IsHoneycombView(views::View* view);

}  // namespace honey

#endif  // HONEYCOMB_LIBHONEY_BROWSER_CHROME_VIEWS_CHROME_VIEWS_H_
