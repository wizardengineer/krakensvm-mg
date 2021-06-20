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

#include <krakensvm.hpp>

namespace vmcb
{
  //
  // Table B-1, this table describes the layout for control area of VMCB
  //

  typedef
    struct _control_area_fmt_t
  {
    _control_area_fmt_t() = default;

    
    union
    {
      uint64_t intercept_read_cr0  : 16;      // +0x000
      uint64_t intercept_write_cr0 : 16;      // +0x002
      uint64_t intercept_read_dr0  : 16;      // +0x004
      uint64_t intercept_write_dr0 : 16;      // +0x006
    };
  
    uint32_t intercept_exceptions_vector;     // +0x008
    uint32_t intercept_misc_vector_3;         // +0x00c
    uint32_t intercept_misc_vector_4;         // +0x010
   
    uint32_t intercept_misc_vector_5 : 5;     // +0x014
    uint32_t reserved_sbz_1          : 27;
   

    uint8_t reserved[0x3b - 0x18];            // +0x018–0x03b

    uint16_t pause_filter_threshold;          // +0x03c
    uint16_t pause_filter_count;              // +0x03e

    //
    // Physical base address of IOPM (bits 11:0 are ignored.)
    // 
    
    uint64_t iopm_base_pa;                    // +0x040

    //
    // Physical base address of MSRPM (bits 11:0 are ignored.)
    //

    uint64_t msrpm_base_pa;                   // +0x048

    //
    // TSC_OFFSET To be added in RDTSC and RDTSCP
    //

    uint64_t tsc_offset;                      // +0x050

    uint64_t guest_asid     : 32;             // +0x058
    uint64_t tlb_control    : 8;
    uint64_t reserved_sbz_2 : 24;

    uint64_t virtual_misc_vector;             // +0x060
    uint64_t interrupt_misc_vector;           // +0x068
    uint64_t exitcode;                        // +0x070
    uint64_t exitinto1;                       // +0x078
    uint64_t exitinfo2;                       // +0x080
    uint64_t exitintinfo;                     // +0x088

    uint64_t enable_misc_vector;              // +0x090
    uint64_t avic_apic_bar;                   // +0x098
    uint64_t guest_pa_ghcb;                   // +0x0a0
    uint64_t eventinj;                        // +0x0a8
    uint64_t nested_page_cr3;                 // +0x0b0
    uint64_t lbr_virtualization_enable;       // +0x0b8
    uint64_t vmcb_clean_bits;                 // +0x0c0
    uint64_t n_rip;                           // +0x0c8

    uint8_t numbers_bytes_fetched;            // +0x0d0
    uint8_t guest_intruction_bytes[15];

    uint64_t avic_apic_backing_page_pointer;  // +0x0e0
    uint64_t reserved_sbz_3;                  // +0x0e8
    uint64_t avic_logical_table_pointer;      // +0x0f0
    uint64_t avic_physical_table_pointer;     // +0x0f8
    uint64_t reserved_sbz_4;                  // +0x100
    uint64_t vmsa_pointer;                    // +0x108
    uint8_t reserved_sbz_5[0x400 - 0x110];

  } control_area_64_t, *pcontrol_area_64_t;

  static_assert(sizeof(control_area_64_t) == 1024,
                  "Size does not match up with the VMCB Control Area");

  //
  // This may seem a bit trivial, however the reason for adding the functions
  // bits_split() and bits_extract(), was to be able to get a single out a
  // bit or a chunk of the data members that's been compressed rather than
  // verbosely being explicit as the certain vectors in the VMCB Control Area
  // and State Save Area. I.E, notice how `intercept_misc_vector_3` is not split
  // into different bitfields.
  //

  template<class T>
    requires integral<T>
  constexpr auto bits_split(uint8_t num, T& data_member) noexcept -> uint8_t
  {
    return (data_member >> num) & 1;
  }


  template<class T>
    requires integral<T>
  constexpr auto bits_extract(uint8_t num, T& data_member) noexcept -> uint8_t
  {
    return data_member & num;
  }

  auto create_init() noexcept -> void;
};

