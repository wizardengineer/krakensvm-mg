/* This file is part of mg-hypervisor by medievalghoul, licensed under the MIT license:
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

#include <krakensvm.hpp>

typedef union
{
  int64_t value;
  struct
  {
    uint64_t low;
    int64_t high;
  } parts;
} physical_addr;

namespace ia32e::mm
{
  typedef
    union _pml4e_fmt
  {
    uint64_t value;

    struct
    {
      uint64_t present  : 1;  // Present; must be 1 to reference a page-directory-pointer table
      uint64_t rd_wr    : 1;  // Read/write; if 0, writes may not be allowed to the 512-GByte region controlled by this entry
      uint64_t user_sup : 1;  // User/supervisor; if 0, user-mode accesses are not allowed to the 512-GByte region controlled by this entry
      uint64_t pwt      : 1;  // Page-level write-through;  determines the memory type used to access
      uint64_t pcd      : 1;  // Page-level write-through;  determines the memory type used to access
      uint64_t accessed : 1;  // Accessed; indicates whether this entry has been used for linear-address translation
      uint64_t ignored  : 1;  // Ignored
      uint64_t reversed : 1;  // Reserved (must be 0)
      uint64_t ignored1 : 1;  // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t reversed1: 1;  // Reserved (must be 0)
      uint64_t phys_addr: 40; // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t reversed2: 2;  // Reserved (must be 0)
      uint64_t ignored2 : 11; // Ignored
      uint64_t efer_nxe : 1;  // If IA32_EFER.NXE = 1, execute-disable 
    };
  } pml4e_t, *ppml4e_t;



}; // namespace ia32e::mm
