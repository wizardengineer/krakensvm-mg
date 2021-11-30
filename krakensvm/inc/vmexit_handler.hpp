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

#include <hv_util.hpp>
#include <vmcb.hpp>
#include <krakensvm.hpp>
#include <paging.hpp>

//
// #VMEXIT Handler
//

// auto vmmcall_handler       (vmcb::pvcpu_ctx_t vcpu_data, guest_status_t guest_status) noexcept -> void;

// vminstructions_handler Substitutes having a vmload_handler, vmsave_handler and vmrun_handler
auto vminstructions_handler(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;
auto cpuid_handler         (vmcb::pvcpu_ctx_t vcpu_data, guest_status_t& guest_status) noexcept -> void;
auto msr_handler           (vmcb::pvcpu_ctx_t vcpu_data, guest_status_t& guest_status) noexcept -> void;

extern "C"
{
auto vmexit_handler        (vmcb::pvcpu_ctx_t vcpu_data, pguest_reg_ctx_t guest_regs)  noexcept -> bool;
}

auto setup_msrpermissions_bitmap (void* msrpermission_map) noexcept -> void;

//
// Event Injection
//

// Injecting #GP General Protection
auto inject_gp             (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injection #UD Invalid Opcode
auto inject_ud             (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injecting #PF Page Fault, Purely made as an example of doing a page fault injection
auto inject_pf             (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

typedef union event_injection
{
  uint64_t value;

  event_injection() : value(0) {}

  struct
  {
    // VECTOR—Bits 7:0. The 8-bit IDT vector of the interrupt or exception. If TYPE is 2 (NMI), the
    // VECTOR field is ignored.
    uint64_t vector  : 8;

    // TYPE—Bits 10:8. Qualifies the guest exception or interrupt to generate. Table 15-11 shows
    // possible valuesand their corresponding interrupt or exception types.Values not indicated are
    // unused and reserved.
    uint64_t type    : 3;

    // EV (Error Code Valid)—Bit 11. Set to 1 if the exception should push an error code onto the stack;
    // clear to 0 otherwise.
    uint64_t err_val : 1;

    // Reserved, SBZ 
    uint64_t sbz     : 19;

    // V (Valid)—Bit 31. Set to 1 if an event is to be injected into the guest; clear to 0 otherwise.
    uint64_t valid   : 1;

    // ERRORCODE—Bits 63:32. If EV is set to 1, the error code to be pushed onto the stack, ignored
    // otherwise.
    uint64_t err_code: 32;
  };
} event_injection, *pevent_injection;


