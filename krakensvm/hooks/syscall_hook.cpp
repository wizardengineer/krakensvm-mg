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

#include <syscall_hook.hpp>
#include <hook_utils.hpp>

#include <capstone/capstone.h>
#include <windef.h>
#include <winnt.h>

void* arena_alloc(size_t n) { appended_alloc += n; return hypervisor_arena + appended_alloc - n; }

namespace hk
{

  auto contruct_lstar_hook(uint64_t kernal_base, uint64_t original_lstar) noexcept -> std::pair<bool, int>
  {
    bool status = false;

    // Instruction data structs that'll hold and map information for our
    // KiSystemCall64(Shadow) rebuild
    cs_insn *current_instruction = nullptr,
            *next_instruction    = nullptr,
            *end_of_instruction  = nullptr,

            *temp_instruction    = nullptr;

    csh handle;

    // Let us know if the if we're dealing with Shadow syscall handler
    // or not
    bool existence_of_syscall        = false,
         existence_of_syscall_shadow = false;

    PRUNTIME_FUNCTION handler_function_entry = nullptr;

    // This will be stored info about ServiceDescriptorTable(Shadow)
    _hook_lstar_info hook_table_instance;

    handler_function_entry = RtlLookupFunctionEntry(kernal_base, (uint64_t*)original_lstar, 0);

    // Pointers to our PKSERVICE_TABLE_DESCRIPTOR
    auto [service_descriptor_table, service_descriptor_table_shadow] = utils::get_service_descriptor_table();
    if (!service_descriptor_table || !service_descriptor_table_shadow) return { status, 0 };

    //if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) return { status, 0 };

    return { status, 0 };
  }

  auto syscallhook_init(int context) noexcept -> bool
  {
    contruct_lstar_hook(utils::get_kernelbase_addr(), 0);
    return true;
  }

};

