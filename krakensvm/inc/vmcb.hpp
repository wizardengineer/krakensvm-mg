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
#include <paging.hpp>
#include <segment_intrins.h>
#include <descriptors_info.hpp>
#include <hv_util.hpp>

extern "C" void svmlaunch(uint64_t* guestvmcb_pa);

namespace vmcb
{
  template<class type>
  concept integral = std::is_integral_v<type>;

  union clean_field
  {
    uint64_t value;

    struct
    {
      uint32_t intercepts  : 1;
      uint32_t iopm        : 1;
      uint32_t asid        : 1;
      uint32_t tpr         : 1;
      uint32_t nested_page : 1;
      uint32_t crx         : 1;
      uint32_t drx         : 1;
      uint32_t dt          : 1;
      uint32_t segs        : 1;
      uint32_t cr2         : 1;
      uint32_t ldr         : 1;
      uint32_t avic        : 1;
      uint32_t cet         : 1;
      uint32_t reversed    : 19;

      uint32_t reserved_2;

    } fields;
  };

  static_assert(sizeof(clean_field) == 0x8,
                  "Size does not match up with the VMCB Control Area clean_field");

  //
  // Table B-1. VMCB Layout, Control Area.
  //

  typedef
    struct _control_area_fmt_t
  {
    _control_area_fmt_t() = default;

    // got the offset formating idea from tandasat =)

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

    clean_field vmcb_clean_bits;              // +0x0c0
    

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

  static_assert(sizeof(control_area_64_t) == 0x400,
                  "Size does not match up with the VMCB Control Area");

  //
  // I will later expand them more for debugging reasons.
  // This may be a bit trivial, however the reason for adding the functions
  // bits_shift() and bits_and(), was to be able to get a single out a
  // bit or a chunk of the data members that's been compressed rather than
  // verbosely being explicit as the certain vectors in the VMCB Control Area
  // and State Save Area. I.E, notice how `intercept_misc_vector_3` is not split
  // into different bitfields.
  //

  template<size_t num, class T>
    requires integral<T>
  constexpr inline auto bits_shift(T& data_member) noexcept -> uint8_t
  {
    return (data_member >> num) & 1;
  }


  template<size_t num, class T>
    requires integral<T>
  constexpr inline auto bits_and(T& data_member) noexcept -> uint8_t
  {
    return data_member & num;
  }

  //
  // segment registers in VMCB Save State Struct data members
  //

  struct seg_register
  {
    uint16_t selector;
    uint16_t attribute;
    uint32_t limit;
    uint64_t base_addr;
  };

  static_assert(sizeof(seg_register) == 0x10,
                  "Size of the Segment Register Format for VMCB is invalid");

  //
  // Table B-2. VMCB Layout, State Save Area
  //

  typedef
    struct _save_state_fmt_t
  {
    _save_state_fmt_t() = default;

    seg_register es;                          // +0x000
    seg_register cs;                          // +0x010
    seg_register ss;                          // +0x020
    seg_register ds;                          // +0x030
    seg_register fs;                          // +0x040
    seg_register gs;                          // +0x050
    seg_register gdtr;                        // +0x060
    seg_register ldtr;                        // +0x070
    seg_register idtr;                        // +0x080
    seg_register tr;                          // +0x090

    uint8_t reserved1[0xca - 0xa0];           // +0x0a0
    uint8_t cpl;                              // +0x0cb
    uint32_t reserved2;                       // +0x0cc
    uint64_t efer;                            // +0x0d0
    uint8_t reserved3[0x147 - 0xd8];          // +0x0d9
    uint64_t cr4;                             // +0x148
    uint64_t cr3;                             // +0x150
    uint64_t cr0;                             // +0x158
    uint64_t dr7;                             // +0x160
    uint64_t dr6;                             // +0x168
    uint64_t rflags;                          // +0x170
    uint64_t rip;                             // +0x178
    uint8_t reserved4[0x1d7 - 0x180];         // +0x180
    uint64_t rsp;                             // +0x1d8
    uint64_t s_cet;                           // +0x1e0
    uint64_t ssp;                             // +0x1e8
    uint64_t isst_addr;                       // +0x1f0
    uint64_t rax;                             // +0x1f8
    uint64_t star;                            // +0x200
    uint64_t lstar;                           // +0x208
    uint64_t cstar;                           // +0x210
    uint64_t sfmask;                          // +0x218
    uint64_t kernel_gs_base;                  // +0x220
    uint64_t sysenter_cs;                     // +0x228
    uint64_t sysenter_esp;                    // +0x230
    uint64_t sysenter_eip;                    // +0x238
    uint64_t cr2;                             // +0x240
    uint8_t reserved5[0x267 - 0x248];         // +0x248
    uint64_t g_pat;                           // +0x268
    uint64_t dbg_ctrl;                        // +0x270
    uint64_t br_from;                         // +0x278
    uint64_t br_to;                           // +0x280
    uint64_t last_excp_from;                  // +0x288
    uint8_t reserved6[0x2df - 0x298];         // +0x298
    uint64_t spec_ctrl;                       // +0x2e0
    uint32_t reserved7;                       // +0x2e4

  } save_state_64_t, *psave_state_64_t;

  static_assert(sizeof(save_state_64_t) == 0x2e8,
                  "Size does not match up with the VMCB Control Area");

  typedef
    struct vmcb_fmt_t
  {
    control_area_64_t control_area;
    save_state_64_t   save_state;

  } vmcb_64_t, *pvmcb_64_t;

  //
  // VCPU Specific data 
  //

  typedef struct _vcpu_ctx_fmt_t
  {
    __declspec(align(PAGE_SIZE)) vmcb_64_t guest_vmcb;
    __declspec(align(PAGE_SIZE)) vmcb_64_t host_vmcb;

    __declspec(align(PAGE_SIZE)) uint8_t host_state_area[PAGE_SIZE];

    uint64_t guest_vmcb_pa;

    uint64_t msrpm_pa; // Physical Address of "MSR Permission Maps"
  } vcpu_ctx_t, * pvcpu_ctx_t;


  auto create_virt_prep(pvcpu_ctx_t vcpu_data, register_ctx_t& host_info) noexcept -> void;
  auto virt_cpu_init   () noexcept -> void;
};
