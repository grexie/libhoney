// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libhoneycomb/common/tracker.h"

// HoneycombTrackNode implementation.

HoneycombTrackNode::HoneycombTrackNode() : track_next_(nullptr), track_prev_(nullptr) {}

HoneycombTrackNode::~HoneycombTrackNode() {}

void HoneycombTrackNode::InsertTrackPrev(HoneycombTrackNode* object) {
  if (track_prev_) {
    track_prev_->SetTrackNext(object);
  }
  object->SetTrackNext(this);
  object->SetTrackPrev(track_prev_);
  track_prev_ = object;
}

void HoneycombTrackNode::InsertTrackNext(HoneycombTrackNode* object) {
  if (track_next_) {
    track_next_->SetTrackPrev(object);
  }
  object->SetTrackPrev(this);
  object->SetTrackNext(track_next_);
  track_next_ = object;
}

void HoneycombTrackNode::RemoveTracking() {
  if (track_next_) {
    track_next_->SetTrackPrev(track_prev_);
  }
  if (track_prev_) {
    track_prev_->SetTrackNext(track_next_);
  }
  track_next_ = nullptr;
  track_prev_ = nullptr;
}

// HoneycombTrackManager implementation.

HoneycombTrackManager::HoneycombTrackManager() : object_count_(0) {}

HoneycombTrackManager::~HoneycombTrackManager() {
  DeleteAll();
}

void HoneycombTrackManager::Add(HoneycombTrackNode* object) {
  base::AutoLock lock_scope(lock_);
  if (!object->IsTracked()) {
    tracker_.InsertTrackNext(object);
    ++object_count_;
  }
}

bool HoneycombTrackManager::Delete(HoneycombTrackNode* object) {
  base::AutoLock lock_scope(lock_);
  if (object->IsTracked()) {
    object->RemoveTracking();
    delete object;
    --object_count_;
    return true;
  }
  return false;
}

void HoneycombTrackManager::DeleteAll() {
  base::AutoLock lock_scope(lock_);
  HoneycombTrackNode* next;
  do {
    next = tracker_.GetTrackNext();
    if (next) {
      next->RemoveTracking();
      delete next;
    }
  } while (next != nullptr);
  object_count_ = 0;
}
