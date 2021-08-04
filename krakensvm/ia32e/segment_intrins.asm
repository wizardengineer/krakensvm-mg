; got this Semantics idea from Daax & XEROXZ
; XEROXZ - (https://githacks.org/_xeroxz/bluepill/-/blob/master/segment_intrin.asm)
; Daax - 7 Days virtualization 

.code
__reades proc
    mov rax, es
    ret
__reades endp

__readcs proc
    mov rax, cs
    ret
__readcs endp

__readss proc
    mov rax, ss
    ret
__readss endp

__readds proc
    mov rax, ds
    ret
__readds endp

__readfs proc
    mov rax, fs
    ret
__readfs endp

__readgs proc
    mov rax, gs
    ret
__readgs endp

__readtr proc
    str ax
    ret
__readtr endp

__readldt proc
    sldt ax
    ret
__readldt endp

__readrsp proc
    mov rax, rsp
    add rax, 8
    ret
__readrsp endp

__readrip proc
    mov rax, [rsp]
    ret
__readrip endp

__readrflags proc
    pushfq
    pop rax
    popfq
    ret
__readrflags endp

END
