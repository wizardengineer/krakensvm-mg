/* This file is part of krakensvm-mg by medievalghoul, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) medievalghoul 2021
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include <type_traits>
#include <utility>


namespace svm
{
  enum class cpuid_e : uint32_t // "e" for enum
  {

    // features
    svm_features            = 0x8000000a,
    svm_features_ex         = 0x80000001,
    processor_feature_id    = 0x00000001,
    processor_feature_id_ex = 0x80000001,
    cpu_vendor_string       = 0x00000000,
    hypervisor_present_ex   = 0x80000000,
    hypervisor_interface    = 0x40000001,
    hypervisor_vendor_id    = 0x40000000,

    // fn8000_0001_ecx_svm bit
    svm_fn                  = 0x00000004,

    // fn8000_0001_ebx_np bit
    nest_page_fn            = 0x00000001,

    // hypervisor specific features
    unload_feature          = 0x41414141
  };

  enum hypercall_num : uint64_t
  {
    syscallhook = 4,
    un_syscallhook
  };

  auto svm_support_checking  () noexcept -> bool;
  auto svm_enabling          () noexcept -> void;

  // Virtualize each processor
  auto virt_each_processors  () noexcept -> bool;

  // De-Virtualize each processor 'KRKN'
  auto devirt_each_processors() noexcept -> void;
  auto devirt_processor(void* shared_context) noexcept -> bool;

}; // namespace svm
