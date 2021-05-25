#pragma once

#define IA32_MSR_EFER                0xC0000080
#define IA32_MSR_EFER_SVM_BIT        (1UL << 12)

#include <stdint.h>
#include "util.hpp"

namespace svm
{
  enum class cpuid_e : uint32_t // "e" for enum
  {

    // features
    svm_features            = 0x8000000a,
    processor_feature_id    = 0x80000001,
    processor_feature_id_ex = 0x00000001,
    cpu_vendor_string       = 0x00000000,

    // fn0000_0001_ecx_svm
    hypervisor_present_ex   = 0x80000000,

    // fn8000_0001_ecx_svm
    svm_fn                  = 0x00000004,

    // fn8000_0001_ebx_np
    nest_page_ebx           = 0x00000001
  };

  auto svm_support_checking() noexcept -> bool;
  auto svm_enabling()         noexcept -> void;
};
