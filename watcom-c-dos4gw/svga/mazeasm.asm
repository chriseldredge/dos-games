.586P
.MODEL FLAT,C

.DATA

 BIT_SHIFT EQU 14

 extrn "C",viewimage  : DWORD
 extrn "C",pixelcount : DWORD
 extrn "C",pixelscale : DWORD
 extrn "C",wallptr    : DWORD 
 extrn "C",top        : DWORD 
 extrn "C",ray        : DWORD 
 extrn "C",bottom     : DWORD 
 extrn "C",sprite_x   : DWORD 
.CODE

 rendersliver_ PROC C
  PUBLIC rendersliver_
        mov edi, DWORD PTR [viewimage+0]
        mov eax, top                                   
        mov ebx, eax
        shl ax, 8
        shl bx, 6
        add ax, bx
        add eax, ray
        add edi, eax
        mov esi, DWORD PTR [wallptr]
        mov eax, pixelcount
        mov edx, eax
        shr eax, BIT_SHIFT
        shl ax, 8
        mov bx, ax
        shr bx, 2
        add ax, bx
        add eax, sprite_x
        add esi, eax
        mov ecx, bottom
        xor edx, edx
        mov edx, pixelcount
        mov ebx, pixelscale
lpop:   movsb
        add edi, 319
        add edx, ebx
        mov eax, edx
        shr eax, BIT_SHIFT
        shl ax, 6
        add eax, sprite_x
        add eax, [wallptr]
        mov esi, eax
        loop lpop
        ret                 ; blaze! I'm a dork!
  rendersliver_ ENDP
END
