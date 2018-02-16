.586P
.MODEL FLAT,C

.DATA
 extrn "C", PC : WORD
.CODE

 cpypal_ PROC C
  PUBLIC cpypal_
    mov edi, eax
    mov esi, edx
    mov ecx, ebx
    cld
   lblCpyPal:
    lodsw
    mov bx, ax
    and ax, 1fh
    cmp ax, PC
    jg zb
    xor ax, ax
    jmp gg
   zb:
    sub ax, PC
   gg:
    shr bx, 5
    mov dx, bx
    shr bx, 6
    and bx, 1fh
    cmp bx, PC
    jl zg
    sub bx, PC
    shl bx, 11
    or  ax, bx
   zg:
    and dx, 3fh
    cmp dx, PC
    jl zr
    sub dx, PC
    cmp dx, PC
    jl zr
    sub dx, PC
    shl dx, 5
    or  ax, dx
   zr:
    stosw                      
    loop lblCpyPal
    ret
 cpypal_ ENDP

 FadeSplice_ PROC C
  PUBLIC FadeSplice_
   mov edi, eax                 ; arg1: destination image pointer
   mov esi, edx                 ; arg2: source image pointer
   mov ecx, ebx                 ; arg3: size
lpFS:
   mov ax, [esi]                ; load samples
   mov bx, [edi]                ;  "      "

   and ax, 1Fh                  ; filter blue sample
   and bx, 1Fh                  ;   "     "      "

   cmp ax, bx                   ; compare and take action
   je gogreen                   ; samples are equal; skip
   jg decblue
   inc ax
   jmp gogreen
decblue:
   dec ax
gogreen:
   mov dx, ax                   ; store blue in buffer

   mov ax, [esi]                ; load samples
   mov bx, [edi]                ;  "      "

   shr ax, 5                    ; filter green sample
   shr bx, 5
   and ax, 3Fh
   and bx, 3Fh

   cmp ax, bx                   ; compare and take action
   je gored                     ; samples are equal; skip
   jg decgreen
   inc ax
   cmp ax, bx
   je gored
   inc ax
   jmp gored
decgreen:
   dec ax
   cmp ax, bx
   je gored
   dec ax
gored:                          
   shl ax, 5
   or dx, ax                    ; store green in buffer

   mov ax, [esi]                ; load samples
   mov bx, [edi]                ;  "      "

   shr ax, 11                    ; filter red sample
   shr bx, 11
   and ax, 1Fh
   and bx, 1Fh

   cmp ax, bx                   ; compare and take action
   je goend                     ; samples are equal; skip
   jg decred
   inc ax
   jmp goend
decred:
   dec ax
goend:
   shl ax, 11
   or dx, ax                    ; store red in buffer

   mov [esi], dx                ; export new sample

   add edi, 2                   ; point to next sample
   add esi, 2                   ;   "   "   "     "

   dec ecx                      ; loop maintenance
   cmp ecx, 0
   je fsdone
   jmp lpFS
fsdone:
   ret
 FadeSplice_ ENDP

END
   
