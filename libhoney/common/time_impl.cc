// Copyright (c) 2012 The Honeycomb Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/internal/honey_time_wrappers.h"
#include "include/internal/honey_types_wrappers.h"
#include "libhoney/common/time_util.h"

#include <limits>
#include <tuple>

#if BUILDFLAG(IS_WIN)
namespace {

// From MSDN, FILETIME "Contains a 64-bit value representing the number of
// 100-nanosecond intervals since January 1, 1601 (UTC)." This value must
// be less than 0x8000000000000000. Otherwise, the function
// FileTimeToSystemTime fails.
// From base/time/time_win.cc:
bool CanConvertToFileTime(int64_t us) {
  return us >= 0 && us <= (std::numeric_limits<int64_t>::max() / 10);
}

}  // namespace
#endif  // BUILDFLAG(IS_WIN)

void honey_time_to_basetime(const honey_time_t& honey_time, base::Time& time) {
  base::Time::Exploded exploded;
  exploded.year = honey_time.year;
  exploded.month = honey_time.month;
  exploded.day_of_week = honey_time.day_of_week;
  exploded.day_of_month = honey_time.day_of_month;
  exploded.hour = honey_time.hour;
  exploded.minute = honey_time.minute;
  exploded.second = honey_time.second;
  exploded.millisecond = honey_time.millisecond;
  std::ignore = base::Time::FromUTCExploded(exploded, &time);
}

void honey_time_from_basetime(const base::Time& time, honey_time_t& honey_time) {
#if BUILDFLAG(IS_WIN)
  int64_t t = time.ToDeltaSinceWindowsEpoch().InMicroseconds();
  if (!CanConvertToFileTime(t)) {
    return;
  }
#endif

  base::Time::Exploded exploded;
  time.UTCExplode(&exploded);
  honey_time.year = exploded.year;
  honey_time.month = exploded.month;
  honey_time.day_of_week = exploded.day_of_week;
  honey_time.day_of_month = exploded.day_of_month;
  honey_time.hour = exploded.hour;
  honey_time.minute = exploded.minute;
  honey_time.second = exploded.second;
  honey_time.millisecond = exploded.millisecond;
}

HONEYCOMB_EXPORT int honey_time_to_timet(const honey_time_t* honey_time, time_t* time) {
  if (!honey_time || !time) {
    return 0;
  }

  base::Time base_time;
  honey_time_to_basetime(*honey_time, base_time);
  *time = base_time.ToTimeT();
  return 1;
}

HONEYCOMB_EXPORT int honey_time_from_timet(time_t time, honey_time_t* honey_time) {
  if (!honey_time) {
    return 0;
  }

  base::Time base_time = base::Time::FromTimeT(time);
  honey_time_from_basetime(base_time, *honey_time);
  return 1;
}

HONEYCOMB_EXPORT int honey_time_to_doublet(const honey_time_t* honey_time, double* time) {
  if (!honey_time || !time) {
    return 0;
  }

  base::Time base_time;
  honey_time_to_basetime(*honey_time, base_time);
  *time = base_time.ToDoubleT();
  return 1;
}

HONEYCOMB_EXPORT int honey_time_from_doublet(double time, honey_time_t* honey_time) {
  if (!honey_time) {
    return 0;
  }

  base::Time base_time = base::Time::FromDoubleT(time);
  honey_time_from_basetime(base_time, *honey_time);
  return 1;
}

HONEYCOMB_EXPORT int honey_time_now(honey_time_t* honey_time) {
  if (!honey_time) {
    return 0;
  }

  base::Time base_time = base::Time::Now();
  honey_time_from_basetime(base_time, *honey_time);
  return 1;
}

HONEYCOMB_EXPORT honey_basetime_t honey_basetime_now() {
  return HoneycombBaseTime(base::Time::Now());
}

HONEYCOMB_EXPORT int honey_time_delta(const honey_time_t* honey_time1,
                              const honey_time_t* honey_time2,
                              long long* delta) {
  if (!honey_time1 || !honey_time2 || !delta) {
    return 0;
  }

  base::Time base_time1, base_time2;
  honey_time_to_basetime(*honey_time1, base_time1);
  honey_time_to_basetime(*honey_time2, base_time2);

  base::TimeDelta time_delta = base_time2 - base_time1;
  *delta = time_delta.InMilliseconds();
  return 1;
}

HONEYCOMB_EXPORT int honey_time_to_basetime(const honey_time_t* from,
                                    honey_basetime_t* to) {
  if (!from || !to) {
    return 0;
  }

  base::Time::Exploded exploded;
  exploded.year = from->year;
  exploded.month = from->month;
  exploded.day_of_week = from->day_of_week;
  exploded.day_of_month = from->day_of_month;
  exploded.hour = from->hour;
  exploded.minute = from->minute;
  exploded.second = from->second;
  exploded.millisecond = from->millisecond;
  base::Time time;
  bool result = base::Time::FromUTCExploded(exploded, &time);
  *to = HoneycombBaseTime(time);
  return result ? 1 : 0;
}

HONEYCOMB_EXPORT int honey_time_from_basetime(const honey_basetime_t from,
                                      honey_time_t* to) {
  if (!to) {
    return 0;
  }

  base::Time time = HoneycombBaseTime(from);

  base::Time::Exploded exploded;
  time.UTCExplode(&exploded);
  to->year = exploded.year;
  to->month = exploded.month;
  to->day_of_week = exploded.day_of_week;
  to->day_of_month = exploded.day_of_month;
  to->hour = exploded.hour;
  to->minute = exploded.minute;
  to->second = exploded.second;
  to->millisecond = exploded.millisecond;
  return exploded.HasValidValues() ? 1 : 0;
}
