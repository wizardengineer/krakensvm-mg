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

//
// Debugger Information, function will be located in:
//            "svm/debugger_info.cpp"
// Canonicalization and Consistency Checks
//

auto vmexit_invalid_dump(const char* from, vmcb::pvcpu_ctx_t vmcb_data) noexcept -> void
{
  kprint_info("From %s Canonicalization and Consistency Checks: \n", from);

  // Checks to see if EFER.SVME is zero
  kprint_info("*\tEFER.SVME => VALUE: %i; %s",(__readmsr(ia32_efer) >> 12U) & 1,
                                              (__readmsr(ia32_efer) >> 12U) & 1 ? "Is Not Zero\n" :
                                                                                  "Is Zero - INVALID\n",
                                              (__readmsr(ia32_efer) >> 12U) & 1 );
  // Checks CR0.CD is zero and CR0.NW is set
  kprint_info("*\tCR0.CD => VALUE: %i; %s and CR0.NW %s",
    vmcb_data->guest_vmcb.save_state.cr0 & (1U << 30),
    vmcb_data->guest_vmcb.save_state.cr0 & (1U << 30) == 0 ? "Is Zero" :
                                                             "Is Not Zero - INVALID",

    vmcb_data->guest_vmcb.save_state.cr0 & (1U << 29) != 1 ? "Is Not Set\n" :
                                                             "Is Set - INVALID\n"
    );
  // Checks CR0[63:32] are not zero
  kprint_info("*\tCR0 => VALUE: 0x%x; Reserved MBZ %s",
    vmcb_data->guest_vmcb.save_state.cr0 >> 32U,
    vmcb_data->guest_vmcb.save_state.cr0 >> 32U == 0 ? "Is Zero\n" :
                                                       "Is Not Zero - INVALID\n");
  // Checks Any MBZ bit of CR3 is set
  kprint_info("*\tCR3 => VALUE: 0x%x; Reserved MBZ %s",
    vmcb_data->guest_vmcb.save_state.cr3,
    vmcb_data->guest_vmcb.save_state.cr3 >> 52U == 0 ? "Is Zero\n" :
                                                       "Is Not Zero - INVALID\n");
  // Checks Any MBZ bit of CR4 is set
  kprint_info("*\tCR4 => VALUE: 0x%x; Reserved MBZ %s",
    vmcb_data->guest_vmcb.save_state.cr4,
    vmcb_data->guest_vmcb.save_state.cr4 & (1U << 19) == 0 &&
    vmcb_data->guest_vmcb.save_state.cr4 >> 24U == 0 ? "Is Zero\n" :
                                                       "Is Not Zero - INVALID\n");
  // Checks DR6[63:32] are not zero
  kprint_info("*\tDR6 => VALUE: 0x%x; Reserved MBZ %s",
    vmcb_data->guest_vmcb.save_state.dr6,
    vmcb_data->guest_vmcb.save_state.dr6 >> 32U != 0 ? "Is Not Zero\n" :
                                                       "Is Zero - INVALID\n");
   // Checks DR7[63:32] are not zero
  kprint_info("*\tDR7 => VALUE: 0x%x; Reserved MBZ %s",
    vmcb_data->guest_vmcb.save_state.dr7,
    vmcb_data->guest_vmcb.save_state.dr7 >> 32U != 0 ? "Is Not Zero\n" :
                                                       "Is Zero - INVALID\n");
   // Checks to see if Any MBZ bit of EFER is set.
  kprint_info("*\tEFER MBZ %s", (__readmsr(ia32_efer) >> 17U) == 0 ? "Is Zero\n" :
                                                                    "Is Not Zero - INVALID\n");
  //
  // Don't a have to do a check for  if
  // "EFER.LMA or EFER.LME is non-zero and this processor does not support long mode."
  // because we already know this was meant for processor with long mode supported
  //

  if ( (vmcb_data->guest_vmcb.save_state.efer & (1U << 8))  != 0
    && (vmcb_data->guest_vmcb.save_state.cr0 & (1U << 31))  != 0
    && (vmcb_data->guest_vmcb.save_state.cr4 & (1U << 5))   != 0
    && (vmcb_data->guest_vmcb.save_state.cs.attribute.longmode != 0)
    && (vmcb_data->guest_vmcb.save_state.cs.attribute.def_bit  != 0))
  {
    kprint_info("*\tEFER.LME, CR0.PG, CR4.PAE, CS.L and CS.D are all non-zero.\n");
  }

  // INTERCEPT_VMRUN
  kprint_info("*\tVMRUN intercept bit %s", vmcb_data->guest_vmcb.control_area.
    intercept_misc_vector_4 & INTERCEPT_VMRUN != 0 ? "Is Not Cleared\n" : "Is Cleared - INVALID\n");

  kprint_info("The END of Canonicalization and Consistency Checks. \n\n");
}
