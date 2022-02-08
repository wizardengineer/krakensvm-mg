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

#include <hook_utils.hpp>
#include <ntdef.h>
#include <wdm.h>

namespace utils
{
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

  auto get_service_descriptor_table()-> std::pair<PKSERVICE_TABLE_DESCRIPTOR, PKSERVICE_TABLE_DESCRIPTOR>
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
    const unsigned char KiSystemServiceStartPattern[] = {
        0x8B, 0xF8,                     // mov edi,eax
        0xC1, 0xEF, 0x07,               // shr edi,7
        0x83, 0xE7, 0x20,               // and edi,20h
        0x25, 0xFF, 0x0F, 0x00, 0x00    // and eax,0fffh  
    };

    uint32_t siganture_size = sizeof KiSystemServiceStartPattern;

    // "address_of_nonshadow_lea" will hold the address to his instruction:
    // 
    // 4c8d15c7202300  lea     r10, [nt!KeServiceDescriptorTable(fffff800`040c7840)]
    uint64_t *address_of_nonshadow_lea = nullptr,

    // "address_of_shadow_lea" will hold the address to his instruction:
    // 
    // 4c8d1d00212300  lea     r11, [nt!KeServiceDescriptorTableShadow
             *address_of_shadow_lea    = nullptr;

    uint64_t kernel_base {}, kiSSS_offset{};


    return { service_descriptor_table, service_descriptor_table_shdw };
  }
};
