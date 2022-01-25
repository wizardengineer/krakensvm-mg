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

#include <descriptors_info.hpp>

namespace ia32e::seg
{
  auto segment_access_rights(segment_desc_64_t* segment_descriptor) noexcept -> uint16_t
  {
    segment_attribute_64_t attribute;

    attribute.dpl         = segment_descriptor->desc_priv_level;
    attribute.type        = segment_descriptor->type;
    attribute.granularity = segment_descriptor->granularity;
    attribute.present     = segment_descriptor->present;
    attribute.longmode    = segment_descriptor->long_mode;
    attribute.def_bit     = segment_descriptor->default_bit;
    attribute.system      = segment_descriptor->system;
    attribute.avl         = segment_descriptor->avalible;
    attribute.reserved = 0;

    return attribute.value;
  }

  auto segment_info(descriptor_reg_64_t& gdt_ptr, uint16_t&& selector) noexcept -> std::pair<uint64_t, uint16_t>
  {
    segment_desc_64_t* seg_descriptor =
      reinterpret_cast<segment_desc_64_t*>(gdt_ptr.base + (selector & ~3));

    uint16_t attributes = segment_access_rights(seg_descriptor);

    uint64_t base_address =
      uint64_t(seg_descriptor->base_address_high << 24 |
        seg_descriptor->base_address_mid << 16 |
        seg_descriptor->base_address_low);

    return { base_address, attributes };
  }
}; // namespace ia32e::seg
