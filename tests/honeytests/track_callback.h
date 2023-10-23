// Copyright (c) 2013 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_TESTS_HONEYCOMBTESTS_TRACK_CALLBACK_H_
#define HONEYCOMB_TESTS_HONEYCOMBTESTS_TRACK_CALLBACK_H_
#pragma once

class TrackCallback {
 public:
  TrackCallback() : gotit_(false) {}
  void yes() { gotit_ = true; }
  bool isSet() { return gotit_; }
  void reset() { gotit_ = false; }
  operator bool() const { return gotit_; }

 protected:
  bool gotit_;
};

#endif  // HONEYCOMB_TESTS_HONEYCOMBTESTS_TRACK_CALLBACK_H_
