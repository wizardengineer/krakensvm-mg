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

;MyHookKiSystemCall64Shadow proc
;    ret
;MyHookKiSystemCall64Shadow endp

.const

GS_USER_STACK     equ   010h
GS_KERNEL_STACK   equ  01A8h
MAX_SYSCALL_INDEX equ 01000h

.code

extern test_simple : proc
extern original_KiSystemCallAddress : dq

MyKiSystemCall64Hook proc

    ; Swap GS base with Kernel PCR
    swapgs

    ; Save the user stack pointer
    mov gs:[GS_USER_STACK], rsp

    mov rsp, gs:[GS_KERNEL_STACK]
    push 02Bh
    push qword ptr gs:[GS_USER_STACK]
    push r11
    push 33h


    ; Checks to see if array index is greater than
    ; Array size.
    cmp rax, MAX_SYSCALL_INDEX

    jg ExitPoint

    ; The stack decreased by 8 * 16 after all the pushes
    ; been executed
    push  rax
    push  rbx
    push  rcx
    push  rdx
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

    
    
    ; Allocate space for the 6 register XMM register
    sub rsp, 88h

    movaps [rsp + 010h], xmm0
    movaps [rsp + 020h], xmm1
    movaps [rsp + 030h], xmm2
    movaps [rsp + 040h], xmm3
    movaps [rsp + 050h], xmm4
    movaps [rsp + 060h], xmm5

    ; 0x20 bytes to for the homing space, storage of
    ; non-volatile registers

    ; Call the C-level #VMEXIT Handler
    call test_simple

    ; Restore the XMM registers
    movaps xmm0, [rsp + 010h]
    movaps xmm1, [rsp + 020h]
    movaps xmm2, [rsp + 030h]
    movaps xmm3, [rsp + 040h]
    movaps xmm4, [rsp + 050h]
    movaps xmm5, [rsp + 060h]

    ; Deallocate space for 6 register XMM register
    add rsp, 88h

    ; Pop all pushed registers
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
    pop  rdx
    pop  rcx
    pop  rbx
    pop  rax

ExitPoint:
    ; Restore UserMode stack pointer
    mov rsp, gs:[GS_USER_STACK]

    ; Jump back to the original KiSystemCall64(Shadow), 
    ; RCX hold the original value of LSTAR. Which holds
    ; KiSystemCall64(Shadow).
    swapgs
    jmp original_KiSystemCallAddress

MyKiSystemCall64Hook endp

END
