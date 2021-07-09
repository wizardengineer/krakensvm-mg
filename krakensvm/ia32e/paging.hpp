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

#include <krakensvm.hpp>

namespace ia32e::mm
{
  //
  // Figure 5-18. 4-Kbyte PML4E—Long Mode
  //

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
      uint64_t ignored1 : 1;  // Ignored
      uint64_t avl      : 3;  // Reserved (must be 0)
      uint64_t phys_addr: 40; // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t avail    : 11; // Available
      uint64_t efer_nxe : 1;  // If IA32_EFER.NXE = 1, execute-disable 
    };
  } pml4e_t, *ppml4e_t;

  static_assert(sizeof(_pml4e_fmt) == 8,
                  "Size of PML4E is not Valid");

  //
  // Figure 5-19. 4-Kbyte PDPE—Long Mode
  //

  typedef
    union _pdpe_fmt_t
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
      uint64_t ignored1 : 1;  // Ignored
      uint64_t avl      : 3;  // Reserved (must be 0)
      uint64_t phys_addr: 40; // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t avail    : 11; // Available
      uint64_t efer_nxe : 1;  // If IA32_EFER.NXE = 1, execute-disable 4_t 
    };
  } pdpe_t, *ppdpe_t;

  static_assert(sizeof(_pdpe_fmt_t) == 8,
                  "Size of PDPE is not Valid");

  //
  // Figure 5-20. 4-Kbyte PDE—Long Mode
  //

  typedef
    union _pde_fmt_t
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
      uint64_t ignored1 : 1;  // Ignored
      uint64_t avl      : 3;  // Reserved (must be 0)
      uint64_t phys_addr: 40; // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t avail    : 11; // Available
      uint64_t efer_nxe : 1;  // If IA32_EFER.NXE = 1, execute-disable 4_t 
    };
  } pde_t, *ppde_t;

  static_assert(sizeof(_pde_fmt_t) == 8,
                  "Size of PDE is not Valid");

  //
  // Figure 5-21. 4-Kbyte PTE—Long Mode
  //

  typedef
    union _pte_fmt_t
  {
    uint64_t value;

    struct
    {
      uint64_t present  : 1;  // Present; must be 1 to reference a page-directory-pointer table
      uint64_t rd_wr    : 1;  // Read/write; if 0, writes may not be allowed to the 512-GByte region controlled by this entry
      uint64_t user_sup : 1;  // User/supervisor; if 0, user-mode accesses are not allowed to the 512-GByte region controlled by this entry
      uint64_t pwt      : 1;  // Page-level write-through;  determines the memory type used to access
      uint64_t pcd      : 1;  // Page-level write-through;  determines the memory type used to access
      uint64_t accessed : 1;  // Accessed;  indicates whether software has accessed the 4-KByte page referenced by this entry
      uint64_t dirty    : 1;  // Dirty; indicates whether software has written to the 4-KByte page referenced by this entry
      uint64_t pat      : 1;  // Indirectly determines the memory type used to access the 4-KByte page referenced by this entry
      uint64_t global   : 1;  // Global; if CR4.PGE = 1, determines whether the translation is global (see Section 4.10); ignored otherwise
      uint64_t avl      : 3;  // Reserved (must be 0)
      uint64_t phys_addr: 40; // Physical address of 4-KByte aligned page-directory-pointer table referenced by this entry
      uint64_t avail    : 7;  // Available
      uint64_t cr4_pke  : 4;
      uint64_t efer_nxe : 1;  // If IA32_EFER.NXE = 1, execute-disable 4_t 
    };
  } pte_t, *ppte_t;

  static_assert(sizeof(_pte_fmt_t) == 8,
                  "Size of PTE is not Valid");

}; // namespace ia32e::mm
