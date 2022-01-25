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

#include <krakensvm.hpp>
#include <vmexit_handler.hpp>
#include <hv_util.hpp>
#include <paging.hpp>
#include <vmcb.hpp>

extern "C" NTSYSAPI NTSTATUS NTAPI ZwYieldExecution();

/*
 * Before enabling SVM, software should detect whether SVM can be enabled using
 * the following algorithm
 */

using namespace ia32e;

//
// Simple Logging Print
//

auto kprint_info(const char* format, ...) noexcept -> void
{
  va_list args;
  va_start(args, format);

  vDbgPrintExWithPrefix("[KRAKEN]: ", DPFLTR_IHVDRIVER_ID,
    DPFLTR_ERROR_LEVEL, format, args);
  va_end(args);
}


namespace svm
{
  //
  // Prototypes
  //

  template<class R, class param>
  auto exec_each_processors(R (*function)(param), param arguments) noexcept -> std::pair<bool, int>;

  // De-Virtualize each processor 'KRKN'
  auto devirt_processor(void* shared_context) noexcept -> bool;
  auto devirt_each_processors() noexcept -> void;

  auto svm_support_checking() noexcept -> bool
  {
    int32_t regs[4]      = {};

    // checks to see if the processor is a AMD processor
    bool processor_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::cpu_vendor_string);
            return regs[1] == 'htuA' &&
                   regs[2] == 'DMAc' &&
                   regs[3] == 'itne';
          }();

    // checks to see if the processor supports SVM
    bool svm_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features_ex);
            return (regs[2] & (int)cpuid_e::svm_fn) != 0;
          }();

    // checks to see if the processor supports Nested Paging
    bool nest_paging_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            return (regs[3] & (int)cpuid_e::nest_page_fn) == 1;
          }();

    // checks to see if the EFER.SVM can be enabled, if the VM_CR.SVMDIS
    // is set then we can't enable SVM.
    bool vmcr_check =
          [&regs]()
          {
            return (__readmsr(vm_cr) & vm_cr_svmdis) == 0;
          }();

    // checks to see if SVM is disable at boot
    bool svm_lock =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            return regs[3] != 0;
          }();

    return processor_check     &&
           svm_check           &&
           vmcr_check          &&
           nest_paging_check   &&
           svm_lock;
  }

  //
  // Enables the EFER.SVME if svm_support_checking returns a true
  //

  auto svm_enabling() noexcept -> void
  {
      __writemsr(ia32_efer,
                 __readmsr(ia32_efer) | ia32_efer_svme);
  }

  //
  // Virualize all processors
  //

  auto virt_each_processors() noexcept -> bool
  {
    vmcb::ppaging_data shared_page_info = nullptr;

    shared_page_info = static_cast<vmcb::ppaging_data>(mm::page_aligned_alloc(sizeof vmcb::paging_data));

    shared_page_info->msrpm_addr = mm::contiguous_alloc(PAGE_SIZE * 2);
    
    // I wasn't able to make this into a label to be used with goto's
    // due to all the errors I was encountering, mainly being the
    // "bypasses declarations with initialization"
    auto _deallocation = [&](bool status = false, int completed_processor = 0)
    {
      if (status == false)
      {
        if (completed_processor >= 1)
        {
          devirt_each_processors();
        }
        else {
          free_page_aligned_alloc(shared_page_info);
          free_contiguous_alloc(shared_page_info->msrpm_addr);
        }
      }

      return status;
    };

    if (svm::svm_support_checking() == false)
    {
      kprint_info("Processor Don't Supports SVM...\n");

      return _deallocation();
    }

    if (shared_page_info->msrpm_addr == nullptr)
    {
      return _deallocation();
    }
 
    setup_msrpermissions_bitmap(shared_page_info->msrpm_addr);
    
    auto [status, completed_processor] = svm::exec_each_processors<bool, vmcb::ppaging_data>(vmcb::virt_cpu_init, shared_page_info);
    //vmcb::virt_cpu_init(shared_page_info);

    // Hook
    svm::exec_each_processors<void, int>( []() -> void { __svm_vmmcall(hypercall_num::syscallhook, nullptr); return; }, NULL);

    return _deallocation(status, completed_processor);

  }



  //
  // Devirtualization of all processors
  //

  auto devirt_processor(void* shared_context) noexcept -> bool
  {
    using namespace vmcb;

    kprint_info("The start of the devirtualization.\n");
    int32_t registers[4] = {};
    uint64_t high = {}, low = {};

    // this double pointer("shared_page_ptr") will point to our "shared_page_info" data struct that's
    // manage in the `function virt_each_processors()`. The shared_context argument of this
    // function, will point to an empty ppaging_data struct. That we'll make hold
    // the address of the orignal "shared_page_info" that's been allocated in `function
    // virt_each_processors()`. This is all done so then we will deallocate the values
    // of "shared_page_info" by the caller (`function devirt_each_processors()`)
    ppaging_data* shared_page_ptr = nullptr;
    pvcpu_ctx_t vcpu_data = nullptr;

    __cpuidex(registers, (int)cpuid_e::unload_feature, (int)cpuid_e::unload_feature);
    if (registers[2] != 'KRKN')
    {
      // kprint_info("Unable to Devirtualize Processor...\n");
      return false;
    }

    low = *(registers + 0) & 0xffffffff;
    high = *(registers + 3);

    vcpu_data = reinterpret_cast<pvcpu_ctx_t>(high << 32 | low);

    shared_page_ptr = static_cast<ppaging_data*>(shared_context);
    *shared_page_ptr = vcpu_data->self_shared_page_info;
    free_page_aligned_alloc(vcpu_data);

    return true;
  }

  auto devirt_each_processors() noexcept -> void
  {
    vmcb::ppaging_data shared_page_info = nullptr;
    svm::exec_each_processors<bool, void*>(devirt_processor, shared_page_info);

    if (shared_page_info != nullptr)
    {
      free_contiguous_alloc(shared_page_info->msrpm_addr);
      free_page_aligned_alloc(shared_page_info);
    }
  }

  // Execute on each processors (cores). For executing our code on
  // each processor, we'll be using the affinity mask. Rather
  // than using DPCs or IPI.
  //
  // Thanks to daax wonderful post - https://revers.engineering/day-3-multiprocessor-initialization-error-handling-the-vmcs/
  // -   Generic type R represent the return value
  template<class R = void, class param = void>
  auto exec_each_processors(R (*function)(param), param arguments) noexcept -> std::pair<bool, int>
  {
    GROUP_AFFINITY old_affinity, affinity;
    PROCESSOR_NUMBER processor_number;

    bool status{};
    int completed_processors{}, index{};
    int processors_amount{};

    processors_amount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
   

    for (; index < processors_amount; index++)
    {
      completed_processors += index;
      
      // Covert from Index to the processor number
      if (KeGetProcessorNumberFromIndex(index, &processor_number) != 0)
      {
        return { false, completed_processors };
      }

      memset(&affinity, 0, sizeof GROUP_AFFINITY);
      affinity.Group = processor_number.Group;
      affinity.Mask = (KAFFINITY)1ull << processor_number.Number;

      KeSetSystemGroupAffinityThread(&affinity, &old_affinity);
      ZwYieldExecution();

      status = function(arguments);
      KeRevertToUserGroupAffinityThread(&old_affinity);
    }

    return { status, completed_processors };
  }

}; // namespace svm
