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

#include "../inc/krakensvm.hpp"

/*
 * Before enabling SVM, software should detect whether SVM can be enabled using
 * the following algorithm
 */

namespace svm
{
  static auto svm_support_checking() noexcept -> bool
  {
    int32_t regs[4]      = {};

    //
    // checks to see if the processor is a AMD processor
    //

    bool processor_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::cpu_vendor_string);
            return regs[1] == 'htuA' &&
                   regs[2] == 'DMAc' &&
                   regs[3] == 'itne';
          }();

    //
    // checks to see if the processor supports SVM
    //

    bool svm_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            return (regs[2] & (int)cpuid_e::svm_fn) == 1;
          }();

    //
    // checks to see if the processor supports Nested Paging
    //

    bool nest_paging_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            return (regs[3] & (int)cpuid_e::nest_page_fn) == 1;
          }();

    //
    // checks to see if the EFER.SVM can be enabled, if the VM_CR.SVMDIS
    // is set then we can't enable SVM.
    //

    bool vmcr_check =
          [&regs]()
          {
            return (__readmsr(vm_cr) & vm_cr_svmdis) == 0;
          }();

    //
    // checks to see if SVM is disable at boot
    //

    bool svm_lock =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            return regs[3] == 0;
          }();

    return processor_check &&
           svm_check       &&
           vmcr_check      &&
           svm_lock;
  }

  auto svm_enabling() noexcept -> void
  {
    if (svm_support_checking())
    {
      __writemsr(ia32_efer,
                 __readmsr(ia32_efer) | ia32_efer_svme);
    }
    else
    {
      KdPrint(("[!] SVM is not fully usable for you pc"));
    }
  }
};
