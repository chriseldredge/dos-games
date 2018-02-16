  .MODEL LARGE, C
  .386
  .DATA
    EXTRN C BytesPerLine  : WORD
    EXTRN C CurrentBank   : WORD
    EXTRN C WindowControl : DWORD
  
  .CODE
  ASSUME CS:@CODE, DS:@DATA

  PUBLIC C VESA_ReturnInfo                ; VESA function 00
  PUBLIC C VESA_ReturnModeInfo            ; VESA function 01
  PUBLIC C VESA_SetMode                   ; VESA function 02
  PUBLIC C VESA_SetPage                   ; We call WindowControl on this one.
                                          ; We could use VESA function 05!

  VESA_ReturnInfo PROC FAR
    USES AX, BX, ES, DI
    ARG  InfoBuffer:DWORD
     
      mov  ax, 4F00h                     ; Function 00: Get VESA info
      les  di, [InfoBuffer]
      int  10h

      ret
  VESA_ReturnInfo ENDP
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  VESA_ReturnModeInfo PROC FAR
    USES AX, BX, ES, DI
    ARG ModeNum:WORD, ModeBuffer:DWORD

      mov  ax, 4F01h                     ; Function 01: Get VESA info
      mov  cx, [ModeNum]
      les  di, [ModeBuffer]
      int  10h

      ret

  VESA_ReturnModeInfo ENDP
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  VESA_SetMode PROC FAR
    USES AX, BX
    ARG  Mode:WORD

      mov   ax, 4F02h                   ; Function 02: Set VESA mode
      mov   bx, [Mode]                  ; Bx = Mode
      int   10h                         ; Interrupt BIOS graphics handler
      ret
  VESA_SetMode ENDP
;=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  VESA_SetPage PROC FAR
    USES AX, BX, DX
    ARG  NewPage:WORD, D:WORD

      cmp  [d], 0
      jz   NoSet
      cli
NoSet:
      xor  bx, bx
      mov  dx, [NewPage]
      mov  [CurrentBank], dx
      call [WindowControl]

      cmp  [d], 0
      jz   OSet
      sti
OSet:
      ret
  VESA_SetPage ENDP
END

