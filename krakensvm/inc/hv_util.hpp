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

#pragma once

#define HV_POOL_TAG       'VHGM'
#define CPUID_MAX_REGS    4

#include <ntddk.h>
#include <intrin.h>

template<class type>
concept integral = std::is_integral_v<type>;

//
// CPUID
//

inline void __cpuid(int[CPUID_MAX_REGS], int);
#pragma intrinsic(__cpuid)

inline void __cpuidex(int[CPUID_MAX_REGS], int, int);
#pragma intrinsic(__cpuidex)

// 
// MSR
//

/*
 * The VM_CR MSR controls certain global aspects of SVM. The VM_CR.SVMDIS
 * is a bit, if this bit was set, it would disallow the act of  setting
 * EFER.SVM bit.
 */

#define vm_cr           0xC0010114
#define vm_cr_svmdis    0x10           // 0x10 will check the 5 bit of VM_CR

 /*
  * Extended Feature Enable Register (EFER)
  */

#define ia32_efer       0xC0000080
#define ia32_efer_svme  0x1000        // this will set the 13 bit of EFER 



