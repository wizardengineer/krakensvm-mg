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
#include <syscall_hook.hpp>

using namespace ia32e;

uint64_t OriginalKiSystemCallAddress = 0;

// This will determine which syscall we'd using base on the index and
// 
uint8_t enabled_syscall_hooks[0x1000];

// This is generally not needed, but i did this to circumvent bug that dealt with
// linker issues, that i couldn't seem to find a solution
extern "C" uint64_t get_enabled_syscall_addr()       { return (uint64_t)&enabled_syscall_hooks; }
extern "C" uint64_t get_original_kisystemcall_addr() { return OriginalKiSystemCallAddress;      }

// Injecting General Protection 
auto inject_gp(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injection Invalid Opcode
auto inject_ud(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;

// Injecting Page Fault
auto inject_pf(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void;


//
// #VMEXIT Handler
//

auto vmmcall_handler(vmcb::pvcpu_ctx_t vcpu_data, guest_status_t guest_status) noexcept -> void
{
  // x64	  x86	      Information Provided
  // RCX	 EDX:EAX	  Hypercall Input Value
  uint64_t hypercall_number = guest_status.guest_registers->rcx;

  // x64	  x86	      Information Provided
  // RDX	 EBX:EDX	  Hypercall Input Value
  uint64_t context = (uint64_t)guest_status.guest_registers->rdx;

  auto simply_hook = [&]() -> void
  {
    int lstar_value = __readmsr(ia32_lstar);
    if (lstar_value == vcpu_data->original_lstar)
    {
      lstar_value = (uint64_t)&MyKiSystemCall64Hook;
    }
    __writemsr(ia32_lstar, lstar_value);
  };

  auto unsimply_hook = [&]() -> void
  {
    __writemsr(ia32_lstar, vcpu_data->original_lstar);
  };

  switch (hypercall_number)
  {
    case svm::hypercall_num::syscallhook:
      simply_hook();
      break;

    case svm::hypercall_num::un_syscallhook:
      unsimply_hook();
      break;

    default:
      vminstructions_handler(vcpu_data);
  }
}

// vminstructions_handler Substitutes having a vmload_handler, vmsave_handler and vmrun_handler
auto vminstructions_handler(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> void
{
  inject_gp(vcpu_data);
}

/* This was gonna to be used as a way to unload the driver, but i decided to use
 * to use CPUID instead. After I had a better understand of it's interception

auto vmmcall_handler(vmcb::pvcpu_ctx_t vcpu_data,
                   guest_status_t guest_status) noexcept -> void
{
  if (guest_status.guest_registers->r14 != 'KREN')
  { inject_ud(vcpu_data); }
}
*/

auto cpuid_handler(vmcb::pvcpu_ctx_t vcpu_data,
                   guest_status_t& guest_status) noexcept -> void
{
  int registers[4] = {};
  int leaf {}, subleaf {};

  seg::segment_attribute_64_t attribute;
  attribute.value = vcpu_data->guest_vmcb.save_state.ss.attribute.value;

  leaf    = static_cast<int>(guest_status.guest_registers->rax);
  subleaf = static_cast<int>(guest_status.guest_registers->rcx);

  __cpuidex(registers, leaf,  subleaf);

  switch(leaf)
  {

    // checking a magic value from CPUID to see if a request if coming
    // from the ring 0. This will be used for unloading the driver. 
    case static_cast<int>(svm::cpuid_e::unload_feature):
      if (subleaf == static_cast<int>(svm::cpuid_e::unload_feature) && attribute.dpl == 0) // 0 being the System DPL
      { guest_status.vmexit_status = true; }
      break;

    // checks if the leaf is 1, an indicator to determine if the machine
    // is running under a Hypervisor
    case static_cast<int>(svm::cpuid_e::processor_feature_id):
      registers[2] |= static_cast<int>(svm::cpuid_e::hypervisor_present_ex);
      break;

    // add our HV interface identifier
    case static_cast<int>(svm::cpuid_e::hypervisor_interface):
      registers[0] = '0#vH';
      break;

    // this case will be executed after an instance of it was initiated
    // by the `hypervisor_vendor_id_installed` function in "svm/vmcb.cpp"
    case static_cast<int>(svm::cpuid_e::hypervisor_vendor_id):
      // The maximum input value for hypervisor CPUID information
      registers[0] = 0x40000001; 
      registers[1] = 'ddeM';
      registers[2] = 'saWy';
      registers[3] = 'ereH';
      break;
  }



  guest_status.guest_registers->rax = registers[0];
  guest_status.guest_registers->rbx = registers[1];
  guest_status.guest_registers->rcx = registers[2];
  guest_status.guest_registers->rdx = registers[3];

  //kprint_info("EAX: %lx, EBX: %lx, ECX: %lx, EDX: %lx\n", registers[0], registers[1], registers[2], registers[3]);

  vcpu_data->guest_vmcb.save_state.rip = vcpu_data->guest_vmcb.control_area.n_rip;
    
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

  // Set the bit to indicating write access should be intercepted for EFER MSR
  constexpr uint64_t offset_2nd_base = (ia32_efer - msr_range_base) * bits_per_msr;
  constexpr uint64_t offset          = offset_2nd_base + size_of_vectors;
  RtlSetBits(&msr_bitmaps, offset + 1, 1);

  // Set the bit to indicating write/read access should be intercepted for LSTAR MSR
  constexpr uint64_t offset_3nd_base = (ia32_lstar - msr_range_base) * bits_per_msr;
  constexpr uint64_t offset1         = offset_3nd_base + size_of_vectors;

  RtlSetBits(&msr_bitmaps, offset, 1);    // setting the read access
  RtlSetBits(&msr_bitmaps, offset + 1, 1);// setting the write access

}

//template<>
//static auto msr_read_manager(uint32_t msr_value) -> void;

auto msr_handler(vmcb::pvcpu_ctx_t vcpu_data,
                 guest_status_t& guest_status) noexcept -> void
{
  // The MSR value will be held in ecx_value
  uint32_t ecx_value = {};
  uint64_t msr_value = {};
  bool write_access  = (vcpu_data->guest_vmcb.control_area.exitinfo1 == 1ul);

  // Get the MSR that's been intercepted, which is stored in RCX
  ecx_value = guest_status.guest_registers->rcx & 0xffffffff;

  bool msr_ranges    = ( (ecx_value <= 0x00001fff) || (ecx_value <= 0xC0000000) ||
                         (ecx_value <= 0xC0001fff));

  msr_value = guest_status.guest_registers->rax & 0xffffffff |
              guest_status.guest_registers->rdx & 0xffffffff;

  switch(ecx_value)
  {
    case ia32_efer:
      if (write_access) [[likely]]
      {
        if ((msr_value & ia32_efer_svme) == 0)
        {
          inject_gp(vcpu_data);
        }

        vcpu_data->guest_vmcb.save_state.efer = msr_value;
      }
      break;

    case ia32_lstar:
      if (write_access)
      {
        //kprint_info("THERE WAS A IA32_LSTAR WRITE!!!");
        if (msr_value == vcpu_data->original_lstar)
        {
          msr_value = (uint64_t)&MyKiSystemCall64Hook;
        }
        __writemsr(ia32_lstar, msr_value);
      }
      else
      {
        //kprint_info("THERE WAS A IA32_LSTAR READ!!!");
        if (vcpu_data->original_lstar)
        {
          msr_value = vcpu_data->original_lstar;
        }
        else { msr_value = __readmsr(ia32_lstar); }
      }
      break;

    default:
      if (msr_ranges) [[unlikely]]
      {
        // If the MSR aka ecx_value is something other than EFER for any reason
        // then proceed to manage the RDMSR/WRMSR for that unspecified MSR.
        // I specified for only EFER to be intercepted, so this "else if" is most
        // likely not going to be executed.

        if (write_access)
        {
          __writemsr(ecx_value, msr_value);
        }
        else
        {
          uint64_t readmsr_value = __readmsr(ecx_value);
          guest_status.guest_registers->rax = readmsr_value & 0xffffffff;
          guest_status.guest_registers->rdx = readmsr_value >> 32 & 0xffffffff;
        }
      }
  }
  
  vcpu_data->guest_vmcb.save_state.rip = vcpu_data->guest_vmcb.control_area.n_rip;
}

extern "C" auto vmexit_handler(vmcb::pvcpu_ctx_t vcpu_data,
                               pguest_reg_ctx_t guest_regs) noexcept -> bool
{
  guest_status_t current_guest_status;

  current_guest_status.guest_registers = guest_regs;
  
  __svm_vmload(vcpu_data->host_vmcb_pa);

  // I've been stuck on a bug (VMEXIT_INVALID) for not adding this one line.
  // So this happened because the Guest Rax is overrwritten by host value on
  // the execution of #VMEXIT. Guest Rax value is stored on the Guest VMCB instead
  //
  current_guest_status.guest_registers->rax = vcpu_data->guest_vmcb.save_state.rax;

  OriginalKiSystemCallAddress = vcpu_data->original_lstar;

  //kprint_info("SYSCALLHOOK_INIT\n");
  int lstar_value = __readmsr(ia32_lstar);
  if (lstar_value == OriginalKiSystemCallAddress)
  {
    lstar_value = (uint64_t)&MyKiSystemCall64Hook;
  }
  __writemsr(ia32_lstar, lstar_value);

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

    case VMEXIT::_CPUID:
      cpuid_handler(vcpu_data, current_guest_status);
      break;

    case VMEXIT::_VMMCALL:
      // This isn't need, i only added vmmcall intercept for learning purposes.
      vmmcall_handler(vcpu_data, current_guest_status);
      break;

    case VMEXIT::_INVALID:
      vmexit_invalid_dump(__func__, vcpu_data);
      break;

    default:
      __debugbreak();
  }

  if (current_guest_status.vmexit_status == true)
  {
    current_guest_status.guest_registers->rax = reinterpret_cast<uint64_t>(vcpu_data) & 0xffffffff;
    current_guest_status.guest_registers->rbx = vcpu_data->guest_vmcb.control_area.n_rip;
    current_guest_status.guest_registers->rcx = vcpu_data->guest_vmcb.save_state.rsp;
    current_guest_status.guest_registers->rdx = reinterpret_cast<uint64_t>(vcpu_data) & 0xffffffff;

    // Load back the guest state back in the processor
    __svm_vmload(vcpu_data->guest_vmcb_pa);

    // Disable interrupts than set the GIF (global interrupt flag)
    _disable();
    // https://docs.microsoft.com/en-us/cpp/intrinsics/svm-stgi?view=msvc-160
    __svm_stgi();

    // Disabling EFER.SVME and restore guest RFLAGS
    //svm::svm_disabling();
    __writemsr(ia32_efer, __readmsr(ia32_efer) & ~(1 << 12));
    __writeeflags(vcpu_data->guest_vmcb.save_state.rflags);

    return current_guest_status.vmexit_status;
  }

  // Save guest rax since it'll be overwritten by host memory
  guest_regs->rax = vcpu_data->guest_vmcb.save_state.rax;

  return current_guest_status.vmexit_status;
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
