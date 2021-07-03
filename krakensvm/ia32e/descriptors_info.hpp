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

namespace ia32e
{
  //
  // A struct format on the Segment Descriptors
  //

  typedef
    struct _desc_table_fmt_t
  {
    //
    // The size of the segment
    //

    uint16_t segment_limit_low;

    //
    // The low part of the base of the descriptor
    //

    uint16_t base_address_low;

    union
    {
      struct
      {
        uint32_t base_address_mid   : 8;
        uint32_t type               : 4;
        uint32_t zero               : 1;
        uint32_t desc_priv_level    : 2;
        uint32_t present            : 1;
        uint32_t segment_limit_high : 4;
        uint32_t avalible           : 1;    
        uint32_t reversed_          : 2;
        uint32_t granularity        : 1;
        uint32_t base_address_high  : 8;
      };
    };

    uint32_t base_address_upper;

    //
    // Bits [7:0] and [13:31] are reserved, but all the other bits are equal to 0.
    // So it'd make sense to make the all zeros since they won't be used or modified by us
    //

    uint32_t all_zeros;

  } segment_desc_64_t;

  //
  // A struct format for the Descriptor-Table Registers (GDTR, IDTR)
  //

  typedef
    struct _desc_reg_fmt_t
  {
    uint16_t limit;
    uint64_t base;

  } descriptor_reg_64_t;



}; // namespace ia32e
