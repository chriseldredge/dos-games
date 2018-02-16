.386p
.code
PROC asmcpy
 void asmcpypal2(void);
 #pragma aux asmcpypal2 =        \
   "C:"                         \
   "lodsw"                      \
   "mov bx, ax"                 \
   "and ax, 1fh"                \
   "cmp ax, PC"                 \
   "jl zb"                      \
   "sub ax, PC"                 \
   "and ax, 1fh"                \
   "jmp gg"                     \
   "zb:"                        \
   "xor al, al"                 \
   "gg:"                        ;
 void asmcpypal3(void);
 #pragma aux asmcpypal3 =       \
   "shr bx, 5"                  \
   "mov dx, bx"                 \
   "shr bx, 6"                  \
   "and bx, 1fh"                \
   "sub bx, PC"                 \
   "and bx, 1fh"                \
   "shl bx, 11"                 \
   "or  bx, bx"                 \
   "and dx, 3fh"                \
   "sub dx, PC"                 \
   "sub dx, PC"                 \
   "and dx, 3fh"                \
   "shl dx, 5"                  \
   "or  dx, dx"                 \
   "stosw"                      \
   "loop C"                     ;

 void asmcpypal(void);
 #pragma aux asmcpypal =        \
   "les edi, t"                 \
   "lds esi, f"                 \
   "mov ecx, w"                 \
   "cld"                        \
   "xor eax, eax"               \
   "xor ebx, ebx"               \
   "xor edx, edx"               \
