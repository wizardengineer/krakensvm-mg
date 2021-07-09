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

#include <vmcb.hpp>

using namespace ia32e;

namespace vmcb
{
  //
  // Preparation for the Virtualization of the processor
  //

  auto create_virt_prep(vcpu_ctx_t& vcpu_data) noexcept -> void
  {
    uint64_t host_vmcb_pa = {}, guest_vmcb_pa = {};
    seg::descriptor_reg_64_t gdtr_ptr, idtr_ptr;

    _sgdt(&gdtr_ptr);
    __sidt(&idtr_ptr);

    //
    // Interception data members of Guest VMCB Control State:
    // & Physical Address being assigned & ASID
    //

    vcpu_data.guest_vmcb
      .control_area.intercept_misc_vector_3 |= // INTERCEPT_MSR_PROT |
                                               INTERCEPT_CPUID    |
                                               INTERCEPT_FERR_FRE;

    vcpu_data.guest_vmcb
      .control_area.intercept_misc_vector_4 |= INTERCEPT_VMRUN |
                                               INTERCEPT_EFER;

    vcpu_data.guest_vmcb.control_area.guest_asid = 1;

    host_vmcb_pa = MmGetPhysicalAddress(&vcpu_data.host_vmcb).QuadPart;

    guest_vmcb_pa = MmGetPhysicalAddress(&vcpu_data.guest_vmcb).QuadPart;

    //
    // Descriptor Table Registers / Segment Registers & Control Registers & GP Registers
    //

    //Descriptor Table Registers / Segment Registers
    vcpu_data.guest_vmcb.save_state.gdtr.base_addr = gdtr_ptr.base;
    vcpu_data.guest_vmcb.save_state.gdtr.limit = gdtr_ptr.limit;

    vcpu_data.guest_vmcb.save_state.idtr.base_addr = idtr_ptr.base;
    vcpu_data.guest_vmcb.save_state.idtr.limit = idtr_ptr.limit;

    vcpu_data.guest_vmcb.save_state.es.limit = __segmentlimit(__reades());
    vcpu_data.guest_vmcb.save_state.cs.limit = __segmentlimit(__readcs());
    vcpu_data.guest_vmcb.save_state.ss.limit = __segmentlimit(__readss());
    vcpu_data.guest_vmcb.save_state.ds.limit = __segmentlimit(__readds());
    vcpu_data.guest_vmcb.save_state.fs.limit = __segmentlimit(__readfs());
    vcpu_data.guest_vmcb.save_state.gs.limit = __segmentlimit(__readgs());

    vcpu_data.guest_vmcb.save_state.es.selector = __reades();
    vcpu_data.guest_vmcb.save_state.cs.selector = __readcs();
    vcpu_data.guest_vmcb.save_state.ss.selector = __readss();
    vcpu_data.guest_vmcb.save_state.ds.selector = __readds();
    vcpu_data.guest_vmcb.save_state.fs.selector = __readfs();
    vcpu_data.guest_vmcb.save_state.gs.selector = __readgs();

    const auto [es_base, es_attr] = seg::segment_info(gdtr_ptr, __reades());
    vcpu_data.guest_vmcb.save_state.es.base_addr = es_base;
    vcpu_data.guest_vmcb.save_state.es.attribute = es_attr;

    const auto [cs_base, cs_attr] = seg::segment_info(gdtr_ptr, __readcs());
    vcpu_data.guest_vmcb.save_state.cs.base_addr = cs_base;
    vcpu_data.guest_vmcb.save_state.cs.attribute = cs_attr;

    const auto [ss_base, ss_attr] = seg::segment_info(gdtr_ptr, __readss());
    vcpu_data.guest_vmcb.save_state.ss.base_addr = ss_base;
    vcpu_data.guest_vmcb.save_state.ss.attribute = ss_attr;

    const auto [ds_base, ds_attr] = seg::segment_info(gdtr_ptr, __readds());
    vcpu_data.guest_vmcb.save_state.ds.base_addr = ds_base;
    vcpu_data.guest_vmcb.save_state.ds.attribute = ds_attr;

    const auto [fs_base, fs_attr] = seg::segment_info(gdtr_ptr, __readfs());
    vcpu_data.guest_vmcb.save_state.fs.base_addr = fs_base;
    vcpu_data.guest_vmcb.save_state.fs.attribute = fs_attr;

    const auto [gs_base, gs_attr] = seg::segment_info(gdtr_ptr, __readgs());
    vcpu_data.guest_vmcb.save_state.gs.base_addr = gs_base;
    vcpu_data.guest_vmcb.save_state.gs.attribute = gs_attr;

    // Control Registers
    vcpu_data.guest_vmcb.save_state.cr0 = uint64_t(__readcr0);
    vcpu_data.guest_vmcb.save_state.cr2 = uint64_t(__readcr2);
    vcpu_data.guest_vmcb.save_state.cr3 = uint64_t(__readcr3);
    vcpu_data.guest_vmcb.save_state.cr4 = uint64_t(__readcr4);
    vcpu_data.guest_vmcb.save_state.efer = uint64_t(__readmsr(ia32_efer));

    // GP Register
    vcpu_data.guest_vmcb.save_state.rsp = __readrsp();
    vcpu_data.guest_vmcb.save_state.rip = __readrip();
    // https://en.wikipedia.org/wiki/Page_attribute_table
    vcpu_data.guest_vmcb.save_state.g_pat = __readmsr(ia32_pat);

    __svm_vmsave(guest_vmcb_pa);

    __writemsr(vm_hsave_pa, MmGetPhysicalAddress(&vcpu_data.host_state_area).QuadPart);

    __svm_vmsave(host_vmcb_pa);

  }
};
