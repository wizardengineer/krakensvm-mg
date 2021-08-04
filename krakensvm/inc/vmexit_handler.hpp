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

auto vminstructions_handler(vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;
// auto vmload_handler        (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;
// auto vmsave_handler        (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;
auto cpuid_handler         (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;
auto msr_handler           (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;

auto vmexit_handler        (vmcb::pvcpu_ctx_t vcpu_data) noexcept -> bool;

//
// Event Injection
//


