// Copyright 2013 The Honeycomb Authors. Portions Copyright
// 2011 the Chromium Authors and 2016 the CEF Authors. All rights reserved. Use of this source code is
// governed by a BSD-style license that can be found in the LICENSE file.

#include "base/notreached.h"
#include "sandbox/win/src/sandbox.h"
#include "sandbox/win/src/sandbox_factory.h"

#include "honey/libhoneycomb/features/features.h"
#include "include/honey_sandbox_win.h"

namespace {

// From content/app/sandbox_helper_win.cc:
void InitializeSandboxInfo(sandbox::SandboxInterfaceInfo* info) {
  info->broker_services = sandbox::SandboxFactory::GetBrokerServices();
  if (info->broker_services) {
    // Ensure the proper mitigations are enforced for the browser process.
    info->broker_services->RatchetDownSecurityMitigations(
        sandbox::MITIGATION_DEP | sandbox::MITIGATION_DEP_NO_ATL_THUNK |
        sandbox::MITIGATION_HARDEN_TOKEN_IL_POLICY);
    // Note: these mitigations are "post-startup".  Some mitigations that need
    // to be enabled sooner (e.g. MITIGATION_EXTENSION_POINT_DISABLE) are done
    // so in Chrome_ELF.
  } else {
    info->target_services = sandbox::SandboxFactory::GetTargetServices();
  }
}

}  // namespace

void* honey_sandbox_info_create() {
  sandbox::SandboxInterfaceInfo* info = new sandbox::SandboxInterfaceInfo();
  memset(info, 0, sizeof(sandbox::SandboxInterfaceInfo));
  InitializeSandboxInfo(info);
  return info;
}

void honey_sandbox_info_destroy(void* sandbox_info) {
  delete static_cast<sandbox::SandboxInterfaceInfo*>(sandbox_info);
}

#if BUILDFLAG(IS_HONEYCOMB_SANDBOX_BUILD)

// Avoid bringing in partition_alloc dependencies.
namespace partition_alloc {
bool ReleaseReservation() {
  DCHECK(false);
  return false;
}

void TerminateBecauseOutOfMemory(size_t size) {
  DCHECK(false);
}
}  // namespace partition_alloc

#endif  // BUILDFLAG(IS_HONEYCOMB_SANDBOX_BUILD)
