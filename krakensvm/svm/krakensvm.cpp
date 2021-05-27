#include <krakensvm.hpp>

namespace svm
{

  auto svm_support_checking() noexcept -> bool
  {
    int32_t regs[4]      = {};

    //
    // checks to see if the processor is a AMD processor
    //

    bool processor_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::cpu_vendor_string);
            if (regs[1] == 'htuA' &&
                regs[2] == 'DMAc' &&
                regs[3] == 'itne') { return true; }
            return false;
          }();

    //
    // checks to see if the processor supports SVM
    //

    bool svm_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            if ((regs[2] &(int)cpuid_e::svm_fn) == 1)
            { return true; }

            return false;
          }();

    //
    // checks to see if the processor supports Nested Paging
    //

    bool nest_paging_check =
          [&regs]()
          {
            __cpuid(regs, (int)cpuid_e::svm_features);
            if (regs[3] & (int)cpuid_e::nest_page_fn == 1)
            { return true; }

            return false;
          }();

    //
    // checks to see if the EFER.SVM can be enabled, if the VM_CR.SVMDIS
    // is set then we can't enable SVM.
    //
    bool vmcr_check =
          [&regs]()
          {
            if (__readmsr() == )
          }();

    return processor_check && svm_check;
  }

  auto svm_enabling()         noexcept -> void
  {

  }
};
