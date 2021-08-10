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

#include <vmexit_handler.hpp>

// Injecting General Protection 
auto inject_gp(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injection Invalid Opcode
auto inject_ud(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injecting Page Fault
auto inject_pf(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

//
// #VMEXIT Handler
//

// vminstructions_handler Substitutes having a vmload_handler, vmsave_handler and vmrun_handler
auto vminstructions_handler(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void
{
  inject_gp(vcpu_data);
}

auto vmcall_handler(vmcb::pvcpu_ctx_t vcpu_data,
                   guest_status_t guest_status) noexcept -> void
{
  if (guest_status.guest_registers->r14 != 'KREN')
  { inject_ud(vcpu_data); }
}

// Setting up the MSR Permission bitmap, to filter out the MSRs I want
// to intercept
auto setup_msrpermissions_bitmap(void* msrpermission_map) noexcept -> void
{
  RTL_BITMAP msr_bitmaps;
  constexpr uint32_t bits_per_msr    = 2;
  constexpr uint32_t msr_range_base  = 0xC0000000;
  constexpr uint16_t size_of_vectors = 0x4000;

  // Initialize the bitmap then clear the bits...
  // AMD MSR bitmap consists of four separate bit vectors, each vectors are
  // 16 kbits (2 kbytes) or 0x4000 bytes. 
  RtlInitializeBitMap(&msr_bitmaps,
                      reinterpret_cast<unsigned long*>(msrpermission_map),
                      (size_of_vectors) * 4);

  RtlClearAllBits(&msr_bitmaps);

  // this wasn't fully understood by me, even after looking into the functions and
  // doing the calculations
  constexpr uint64_t offset_2nd_base = (ia32_efer - msr_range_base) * bits_per_msr;
  constexpr uint64_t offset          = offset_2nd_base + size_of_vectors;
  RtlSetBits(&msr_bitmaps, offset + 1, 1);

}

auto msr_handler(vmcb::pvcpu_ctx_t vcpu_data,
                 guest_status_t guest_status) noexcept -> void
{

}

auto vmexit_handler(vmcb::pvcpu_ctx_t vcpu_data,
                    pguest_reg_ctx_t guest_registers) noexcept -> bool
{
  guest_status_t current_guest_status;

  current_guest_status.guest_registers = guest_registers;

  switch (vcpu_data->guest_vmcb.control_area.exitcode)
  {
    case VMEXIT::_VMRUN:
      vminstructions_handler(vcpu_data);
      break;

    case VMEXIT::_VMLOAD:
      vminstructions_handler(vcpu_data);
      break;

    case VMEXIT::_VMSAVE:
      vminstructions_handler(vcpu_data);
      break;

    case VMEXIT::_MSR:
      msr_handler(vcpu_data, current_guest_status);
      break;

    case VMEXIT::_VMMCALL:
      vminstructions_handler(vcpu_data);
      break;

    return current_guest_status.vmexit_status;

  }


  _disable();

  // Disabling EFER.SVME
  svm::svm_disabling();
}

//
// Event Injection
//

// Injecting General Protection
auto inject_gp(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void
{
  event_injection event;
  event.vector = 13;
  event.type   = 3;
  event.err_val  = 1;
  event.valid    = 1;
  event.err_code = 1;
  
  vcpu_data->guest_vmcb.control_area.eventinj = event.value;
}

// Injection Invalid Opcode
auto inject_ud(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void
{
  event_injection event;
  event.vector = 6;
  event.type   = 3;
  event.err_val  = 1;
  event.valid    = 1;
  event.err_code = 1;
  
  vcpu_data->guest_vmcb.control_area.eventinj = event.value;
}

// Injecting Page Fault
auto inject_pf(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void
{
  event_injection event;
  event.vector = 14;
  event.type   = 3;
  event.err_val  = 1;
  event.valid    = 1;
  event.err_code = 1;

  vcpu_data->guest_vmcb.control_area.eventinj = event.value;
}
