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

namespace ia32e::seg
{
  //
  // A struct format on the Memory-Segment Descriptor
  //

#pragma pack(push, 1)
  typedef union _mem_desc_table_fmt_t
  {
    uint64_t value;

    struct 
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
        uint32_t middle_value;

        struct
        {
          uint32_t base_address_mid   : 8;
          //uint32_t accessed           : 1;
          //uint32_t write_read         : 1;
          //uint32_t con_exp            : 1;
          uint32_t type               : 4;
          uint32_t system             : 1;
          uint32_t desc_priv_level    : 2;
          uint32_t present            : 1;
          uint32_t segment_limit_high : 4;
          uint32_t avalible           : 1;    
          uint32_t long_mode          : 1;
          uint32_t default_bit        : 1;
          uint32_t granularity        : 1;
          uint32_t base_address_high  : 8;
        };
      };
    };
  } segment_desc_64_t;

  static_assert(sizeof(segment_desc_64_t) == 8);

  //
  // A struct format on the System-Segment Descriptors Tables
  //

  typedef
    struct _system_desc_table_fmt_t
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
  } segment_desc_table_64_t;

  static_assert(sizeof(segment_desc_table_64_t) == 16);

  // 
  // Segment Attribute
  //

  typedef union _segment_attribute_fmt_t
  {
    uint16_t value;

    struct
    {
      uint16_t type    : 4;
      uint16_t system  : 1;
      uint16_t dpl     : 2;
      uint16_t present : 1;
      uint16_t avl     : 1;
      uint16_t longmode: 1;
      uint16_t def_bit : 1;  // default bit
      uint16_t granularity : 1;
      uint16_t reserved: 4;
    };
  } segment_attribute_64_t, *psegment_attribute_64_t;

  static_assert(sizeof(segment_attribute_64_t) == 2);

  //
  // A struct format for the Descriptor-Table Registers (GDTR, IDTR)
  //

  typedef
    struct _desc_reg_fmt_t
  {
    uint16_t limit;
    uint64_t base;
  } descriptor_reg_64_t;

  static_assert(sizeof(descriptor_reg_64_t) == 10);
#pragma pack(pop)

  //auto segment_access_rights(descriptor_reg_64_t& segment_descriptor) noexcept -> uint64_t;

  auto segment_access_rights(segment_desc_64_t* segment_descriptor) noexcept -> uint64_t
  {
    segment_attribute_64_t attribute;

    attribute.dpl  = segment_descriptor->desc_priv_level;
    attribute.type = segment_descriptor->type;
    attribute.granularity = segment_descriptor->granularity;
    attribute.present     = segment_descriptor->present;
    attribute.longmode = segment_descriptor->long_mode;
    attribute.def_bit  = segment_descriptor->default_bit;
    attribute.system   = segment_descriptor->system;
    attribute.avl      = segment_descriptor->avalible;
    attribute.reserved = 0;

    return attribute.value;
  }

  //auto segment_info(descriptor_reg_64_t& gdt_ptr) noexcept -> std::pair<uint64_t, uint16_t>;
  auto segment_info(descriptor_reg_64_t& gdt_ptr, uint16_t&& selector) noexcept -> std::pair<uint64_t, uint16_t>
  {
    segment_desc_64_t* seg_descriptor =
      reinterpret_cast<segment_desc_64_t*>(gdt_ptr.base + (selector & ~3));

    uint16_t attributes = segment_access_rights(seg_descriptor);

    uint64_t base_address =
      uint64_t(seg_descriptor->base_address_high << 24 |
               seg_descriptor->base_address_mid  << 16 |
               seg_descriptor->base_address_low);

    return { base_address, attributes };
  }
}; // namespace ia32e::seg
