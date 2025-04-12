section .text

global fist_find_32

;;; ---------------------------------------------
;;; Descript:   find list elem index, if elem_size == 32 bytes
;;; Entry:      rdi = addr elem
;;;             rsi = data
;;;             rdx = coust bytes data
;;; Exit:       rax = index finded elem (0 if not find)
;;; Destroy:    rcx, ymm0-2
;;; ---------------------------------------------
fist_find_32:
    test rdx, rdx                                           ; check to zero size
je .ExitFailure

    mov rcx, 32                                             ; rcx - counter
    vmovdqa ymm0, [rdi]                                     ; ymm0 - elem
.Cicle:
    vmovdqa ymm1, [rsi + rcx]                               ; ymm1 - elem in data
    vpcmpeqb ymm2, ymm0, ymm1                               ; ymm2 - result cmp elem and elem in data
    vpmovmskb eax, ymm2                                     ; eax - cmp mask
    cmp eax, 0xFFFFFFFF                                     ; check if all bytes match
je .ExitSuccess

    add rcx, 32                                             ; rcx - next ind
    cmp rcx, rdx                                            ; check to end data
jl .Cicle

.ExitFailure:
    xor rax, rax
    vzeroupper                                              ; for optimize sse instrs
    ret

.ExitSuccess:
    mov rax, rcx                                            ; rax - index finded elem
    shr rax, 5
    vzeroupper                                              ; for optimize sse instrs
    ret