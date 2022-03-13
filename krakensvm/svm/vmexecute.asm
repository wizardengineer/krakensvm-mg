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

.const

SELF_OFFSET equ 3008h

.code

extern vmexit_handler : proc

; Vmcall function
__svm_vmmcall proc
    vmmcall
    ret
__svm_vmmcall endp

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

    ; Note that before the pushes of those 15 registers (rax...r15), the value of the 
    ; RSP pointed at the address of "guest_vmcb_pa".
    ;
    ; Now this is our current stack state is looking like so far, after the pushes of  
    ; those 15 registers (rax...r15).
    ; -----------------------------------------------------------------------------------------
    ; RSP Points at     -> 0xff...df0 -> R15          : the start of what "pguest_reg_ctx_t" struct 
    ;                                                   will be point to.
    ;                   -> 0xff...df8 -> R14          :
    ;			  	          -> 0xff...e00 -> R13          :
    ;				   	                 ...                  :              
    ;			              -> 0xff...e60 -> RAX          :
    ; RSP + 8*15        -> 0xff...e68 -> guest_vmcb_pa: 4th data member in the "vcpu_ctx_t" struct
    ; 				          -> 0xff...e70 -> host_vmcb_pa : 5th data member in the "vcpu_ctx_t" struct
    ; RSP + 8*16+8      -> 0xff...e78 -> self         : 6th data member in the "vcpu_ctx_t" struct
    ;
    ; -----------------------------------------------------------------------------------------
    ;
    ; We'll need to understand the state of our stack so we can set our parameters of the 
    ; function "vmexit_handler(vmcb::pvcpu_ctx_t vcpu_data, pguest_reg_ctx_t guest_regs)". 
    ; 
    ; -----------------------------------------------------------------------------------------

    ; Set up the arguments that being taken by vmexit_handler, rdx will be the second argument "guest_regs"
    mov rdx, rsp

    ; 88h == 8 * 16 + 8
    ; It is took me so long to find the offset to the *self,
    ; but thanks to the greatness of Windbg i was able to
    ; find the offset.

    mov rcx, [rsp + 88h]

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
    ;sub rsp, 20h

    ; Call the C-level #VMEXIT Handler
    call vmexit_handler
    ;add rsp, 20h

    ; Restore the XMM registers
    movaps xmm0, [rsp + 010h]
    movaps xmm1, [rsp + 020h]
    movaps xmm2, [rsp + 030h]
    movaps xmm3, [rsp + 040h]
    movaps xmm4, [rsp + 050h]
    movaps xmm5, [rsp + 060h]

    ; Deallocate space for 6 register XMM register
    add rsp, 88h

    test al, al

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

    jnz svm_terminate
    jmp svm_loop

svm_terminate:
    mov rsp, rcx

    mov ecx, 'KRKN'

    jmp rbx
svmlaunch endp

END
