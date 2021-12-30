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

#include <cstdint>

//
// I've decide not to use this because of strange bit field placements I wasn't
// comfortable with using, with this project. 
//
namespace ia32e::control_registers
{

  // Figure 3-1. Control Register 0 (CR0)
  union _cr0_t
  {
    uint64_t value;

    struct
    {
      uint64_t protection_enabled  : 1;
      uint64_t monitor_coprocessor : 1;
      uint64_t emulation           : 1;
      uint64_t task_switched       : 1;
      uint64_t extension_type      : 1;
      uint64_t numeric_error       : 1;
      uint64_t reserved            : 9;
      uint64_t write_protect       : 1;
      uint64_t reserved_1          : 1;
      uint64_t alignment_mask      : 1;
      uint64_t reserved_2          : 9;
      uint64_t not_write_through   : 1;
      uint64_t cache_disable       : 1;
      uint64_t paging              : 1;
      uint64_t reserved_mbz        : 32;
    };
  } cr0_t;

  // Figure 3-3. Control Register 2 (CR2)—Long Mode
  union _cr2_t
  {
    uint64_t value;

    struct
    {
      uint64_t page_fault_addr1 : 32;
      uint64_t page_fault_addr2 : 32;
    };
  } cr2_t;

  union _cr3_t
  {
    uint64_t value;

    struct
    {
      uint64_t processor_context_id : 12;
      uint64_t page_base_address    : 39;
      uint64_t reserved_mbz         : 11;
    };
  };


};
