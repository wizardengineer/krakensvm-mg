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
#include <utility>
#include <hook_utils.hpp>

// extern "C" int MyKiSystemCall64Hook();

//
// Hypervisor Allocation
//

__declspec(selectany) uint64_t appended_alloc   {};
__declspec(selectany) char8_t* hypervisor_arena {};

namespace hk
{
  struct _hook_lstar_info
  {
    uint64_t* hook_lstar_table;
    uint64_t* hook_lstar_table_shadow;

    size_t hooked_table_size, hook_table_shdw_size;

    _hook_lstar_info() noexcept : hook_table_shdw_size (0),
                                  hooked_table_size    (0),
                                  hook_lstar_table     (nullptr),
                                  hook_lstar_table_shadow (nullptr) {}

    ~_hook_lstar_info() = default;

  };

  auto syscallhook_init    (int context)          noexcept -> bool;
  auto contruct_lstar_hook (uint64_t kernal_base) noexcept -> std::pair<bool, int>;
};
