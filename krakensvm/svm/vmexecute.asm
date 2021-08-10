; This file is part of krakensvm-mg by medievalghoul, licensed under the MIT license:
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

SELF_OFFSET equ 3008h

include ../inc/asm_inc.asm

extern vmexit_handler : proc

.code
svmlaunch proc
    mov rsp, rcx 

svm_loop:
    mov rax, [rsp]

    ; Load a subset of the VMCB to the processor 
    vmload rax

    ; Execute the Guest machine
    vmrun rax

    ; Store a subset of the processor in the VMCB
    vmsave rax

    ; Make sure this is saved because host code will
    ; destroy it

    ; The stack decreased by 8 * 15
    pushaq_m

    ; Set up the arguments that being taken by vmexit_handler
    mov rdx, rsp

    mov rcx, [rsp + 8 * 15 + SELF_OFFSET]
    ; Allocate space for the 16 register XMM register, along
    ; with homing space
    sub rsp, 0x120

    movaps [rsp], xmm0
    movaps [rsp + 10h], xmm1
    movaps [rsp + 20h], xmm2
    movaps [rsp + 30h], xmm3
    movaps [rsp + 40h], xmm4
    movaps [rsp + 50h], xmm5
    movaps [rsp + 60h], xmm6
    movaps [rsp + 70h], xmm7
    movaps [rsp + 80h], xmm8
    movaps [rsp + 90h], xmm9
    movaps [rsp + 0Ah], xmm10
    movaps [rsp + 0Bh], xmm11
    movaps [rsp + 0Ch], xmm12
    movaps [rsp + 0Dh], xmm13
    movaps [rsp + 0Eh], xmm14
    movaps [rsp + 0Fh], xmm15

    ; Call the C-level #VMEXIT Handler
    call vmexit_handler

    ; Restore the XMM registers
    movaps xmm0, [rsp]
    movaps xmm1, [rsp + 10h]
    movaps xmm2, [rsp + 20h]
    movaps xmm3, [rsp + 30h]
    movaps xmm4, [rsp + 40h]
    movaps xmm5, [rsp + 50h]
    movaps xmm6, [rsp + 60h]
    movaps xmm7, [rsp + 70h]
    movaps xmm8, [rsp + 80h]
    movaps xmm9, [rsp + 90h]
    movaps xmm10, [rsp + 0Ah]
    movaps xmm11, [rsp + 0Bh]
    movaps xmm12, [rsp + 0Ch]
    movaps xmm13, [rsp + 0Dh]
    movaps xmm14, [rsp + 0Eh]
    movaps xmm15, [rsp + 0Fh]

    ; Deallocate space for 16 register XMM register
    add rsp, 0x120

    test al, al
    popaq_m

    jmp svm_loop

svm_terminate:
    mov rsp, rcx
svmlaunch endp

END
