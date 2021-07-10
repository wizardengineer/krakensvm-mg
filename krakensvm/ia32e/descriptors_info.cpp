#include <descriptors_info.hpp>

namespace ia32e::seg
{
  auto segment_access_rights(segment_desc_64_t* segment_descriptor) noexcept -> uint16_t
  {
    segment_attribute_64_t attribute;

    attribute.dpl = segment_descriptor->desc_priv_level;
    attribute.type = segment_descriptor->type;
    attribute.granularity = segment_descriptor->granularity;
    attribute.present = segment_descriptor->present;
    attribute.longmode = segment_descriptor->long_mode;
    attribute.def_bit = segment_descriptor->default_bit;
    attribute.system = segment_descriptor->system;
    attribute.avl = segment_descriptor->avalible;
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
