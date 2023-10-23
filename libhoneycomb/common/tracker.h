// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef HONEYCOMB_LIBHONEYCOMB_COMMON_TRACKER_H_
#define HONEYCOMB_LIBHONEYCOMB_COMMON_TRACKER_H_
#pragma once

#include "include/honey_base.h"

#include "base/synchronization/lock.h"

// Class extended by objects that must be tracked.  After creating a tracked
// object you should add it to the appropriate track manager.
class HoneycombTrackNode {
 public:
  HoneycombTrackNode();
  virtual ~HoneycombTrackNode();

  // Returns true if the object is currently being tracked.
  inline bool IsTracked() { return (track_prev_ || track_next_); }

 private:
  inline HoneycombTrackNode* GetTrackPrev() { return track_prev_; }
  inline void SetTrackPrev(HoneycombTrackNode* base) { track_prev_ = base; }
  inline HoneycombTrackNode* GetTrackNext() { return track_next_; }
  inline void SetTrackNext(HoneycombTrackNode* base) { track_next_ = base; }

  // Insert a new object into the tracking list before this object.
  void InsertTrackPrev(HoneycombTrackNode* object);

  // Insert a new object into the tracking list after this object.
  void InsertTrackNext(HoneycombTrackNode* object);

  // Remove this object from the tracking list.
  void RemoveTracking();

 private:
  HoneycombTrackNode* track_next_;
  HoneycombTrackNode* track_prev_;

  friend class HoneycombTrackManager;
};

// Class used to manage tracked objects.  A single instance of this class
// should be created for each intended usage.  Any objects that have not been
// removed by explicit calls to the Destroy() method will be removed when the
// manager object is destroyed.  A manager object can be created as either a
// member variable of another class or by using lazy initialization:
// base::LazyInstance<HoneycombTrackManager> g_singleton = LAZY_INSTANCE_INITIALIZER;
class HoneycombTrackManager : public HoneycombBaseRefCounted {
 public:
  HoneycombTrackManager();
  ~HoneycombTrackManager() override;

  // Add an object to be tracked by this manager.
  void Add(HoneycombTrackNode* object);

  // Delete an object tracked by this manager.
  bool Delete(HoneycombTrackNode* object);

  // Delete all objects tracked by this manager.
  void DeleteAll();

  // Returns the number of objects currently being tracked.
  inline int GetCount() { return object_count_; }

 private:
  HoneycombTrackNode tracker_;
  int object_count_;

  base::Lock lock_;

  IMPLEMENT_REFCOUNTING(HoneycombTrackManager);
};

#endif  // HONEYCOMB_LIBHONEYCOMB_COMMON_TRACKER_H_
