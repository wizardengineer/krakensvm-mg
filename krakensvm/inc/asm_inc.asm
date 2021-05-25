; This file is part of mg-hypervisor by medievalghoul, licensed under the MIT license:
;
; MIT License
;
; Copyright (c) medievalghoul 2021
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.
;

; simple include file that simple my ia32e assembly programming

.code

;
; @brief      it like pushad but for 64 bit registers...got this from tandasat :^)
;

pushaq_m macro
    push  rax
    push  rcx
    push  rdx
    push  -1
    push  rbp
    push  rsi
    push  rdi
    push  r8
    push  r9
    push  r10
    push  r11
    push  r12
    push  r13
    push  r14
    push  r15
endm


popaq_m macro
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx    ; Dummy for rsp (this value is destroyed by the next pop).
    pop  rbx
    pop  rdx
    pop  rcx
    pop  rax
endm

END
