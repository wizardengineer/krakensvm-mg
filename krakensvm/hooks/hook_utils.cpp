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

#include "hook_utils.hpp"
#include "pe.hpp"

namespace utils
{
  // Thanks to IPower:
  // https://github.com/iPower/KasperskyHook/blob/master/KasperskyHookDrv/utils.cpp
  //

  static bool memory_compare(const char* data_ptr, const unsigned char* bmask, const char* szmask)
  {
    for (; *szmask; ++szmask, ++data_ptr, ++bmask)
    {
      if (*szmask == 'x' && *data_ptr != *bmask) return false;
    }

    return true;
  }

  static uint64_t find_pattern(const uint64_t base, const size_t size, const unsigned char* bmask, const char* szmask)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (memory_compare(reinterpret_cast<const char*>(base + i), bmask, szmask)) return base + i;
    }

    return 0;
  }
  
  static auto find_pattern_section(const uint64_t base, const char* section, const unsigned char* bmask, const char* szmask) noexcept -> uint64_t
  {
    if (!base || !section || !bmask || !szmask) return 0;

    auto* psection = pe::section_header_getter(base, section);

    return psection ? find_pattern(base + psection->VirtualAddress, psection->SizeOfRawData, bmask, szmask) : 0;
  }

  uint64_t get_kernelbase_addr()
  {
    UNICODE_STRING routine{};
    uint64_t kernel_base{};

    RtlInitUnicodeString(&routine, L"RtlPcToFileHeader");
    using f_RtlPcToFileHeader = PVOID(*) (PVOID PcValue, PVOID* BaseOfImage);

    const f_RtlPcToFileHeader RtlPcToFileHeader =
      reinterpret_cast<f_RtlPcToFileHeader>(MmGetSystemRoutineAddress(&routine));

    if (!RtlPcToFileHeader) return 0xdead;

    RtlPcToFileHeader(RtlPcToFileHeader, reinterpret_cast<void**>(&kernel_base));

    return kernel_base;
  }

  auto get_service_descriptor_table() -> std::pair<PKSERVICE_TABLE_DESCRIPTOR, PKSERVICE_TABLE_DESCRIPTOR>
  {
    // The pointers that'll be holding the Address for our systems
    // nt!KeServiceDescriptorTable(Shadow) Data struct
    PKSERVICE_TABLE_DESCRIPTOR service_descriptor_table      = nullptr,
                               service_descriptor_table_shdw = nullptr;

    // The assembly we'll be interacting with:
    //
    // nt!KiSystemServiceStart:
    // 4889a3d8010000  mov     qword ptr[rbx + 1D8h], rsp
    // 8bf8            mov     edi, eax
    // c1ef07          shr     edi, 7
    // 83e720          and     edi, 20h
    // 25ff0f0000      and     eax, 0FFFh
    // nt!KiSystemServiceRepeat:
    // 4c8d15c7202300  lea     r10, [nt!KeServiceDescriptorTable(fffff800`040c7840)]
    // 4c8d1d00212300  lea     r11, [nt!KeServiceDescriptorTableShadow
    constexpr unsigned char KiSystemServiceStartPattern[] = {
        0x8B, 0xF8,                     // mov edi,eax
        0xC1, 0xEF, 0x07,               // shr edi,7
        0x83, 0xE7, 0x20,               // and edi,20h
        0x25, 0xFF, 0x0F, 0x00, 0x00    // and eax,0fffh  
    };

    uint32_t siganture_size = sizeof KiSystemServiceStartPattern;

    // "address_of_nonshadow_lea" will hold the address to this instruction:
    // 
    // 4c8d15c7202300  lea     r10, [nt!KeServiceDescriptorTable(fffff800`040c7840)]
    uint64_t address_of_nonshadow_lea = {},

    // "address_of_shadow_lea" will hold the address to this instruction:
    // 
    // 4c8d1d00212300  lea     r11, [nt!KeServiceDescriptorTableShadow
             address_of_shadow_lea    = {};

    uint64_t kernel_base {}, kiSSS_offset{};

    uint64_t relative_offest_shadow {}, relative_offest_nonshadow {};

    kernel_base = get_kernelbase_addr();
    if ( kernel_base == 0xdead) return { nullptr, nullptr };

    const auto pattern_result = find_pattern_section(kernel_base, ".text", KiSystemServiceStartPattern, "xxxxxxxxxxx??");
    if ( pattern_result == 0) return { nullptr, nullptr };

    // 4c8d15c7202300  lea     r10, [nt!KeServiceDescriptorTable(fffff800`040c7840)]
    address_of_nonshadow_lea = pattern_result + siganture_size;

    // 4c8d1d00212300  lea     r11, [nt!KeServiceDescriptorTableShadow
    address_of_shadow_lea = address_of_nonshadow_lea + 7; // Skip by 7 bytes

    // Check to see if we're managing the right opcodes;
    //
    // The "+ 0" was not needed, just added it to be more verbose

    // 4c 8d 15 c7 20 23 00
    if ( *(unsigned char*) (address_of_nonshadow_lea + 0) == 0x4c &&
         *(unsigned char*) (address_of_nonshadow_lea + 1) == 0x8d && 
         *(unsigned char*) (address_of_nonshadow_lea + 2) == 0x15)
    {
      relative_offest_nonshadow = *(uint64_t*) (address_of_nonshadow_lea + 3);
    }

    // 4c 8d 1d 00 21 23 00
    if ( *(unsigned char*) (address_of_shadow_lea + 0) == 0x4c &&
         *(unsigned char*) (address_of_shadow_lea + 1) == 0x8d && 
         *(unsigned char*) (address_of_shadow_lea + 2) == 0x1d)
    {
      relative_offest_shadow = *(uint64_t*) (address_of_shadow_lea + 3);
    }

    if (!relative_offest_shadow || !relative_offest_nonshadow) return { nullptr, nullptr };

    service_descriptor_table = (PKSERVICE_TABLE_DESCRIPTOR) (address_of_nonshadow_lea + relative_offest_nonshadow + 7);
    service_descriptor_table_shdw = (PKSERVICE_TABLE_DESCRIPTOR)(address_of_shadow_lea + relative_offest_shadow + 7);

    return { service_descriptor_table, service_descriptor_table_shdw };
  }
};
