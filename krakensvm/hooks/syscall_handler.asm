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

GS_BASE equ 01h

.code

extern test_simple : proc

MyKiSystemCall64 proc

    ; Swap GS base with Kernel PCR
    swapgs



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

    ; Jump back to the original KiSystemCall64(Shadow), 
    ; RCX hold the original value of LSTAR. Which holds
    ; KiSystemCall64(Shadow).
    jmp rcx

MyKiSystemCall64 endp

END
