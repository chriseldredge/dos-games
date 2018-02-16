;	Static Name Aliases
;
	TITLE   pcx.c
	.MODEL  LARGE
	.386p
	.387
INCLUDELIB      LLIBCE
INCLUDELIB	OLDNAMES.LIB
PCX_TEXT	SEGMENT  WORD USE16 PUBLIC 'CODE'
PCX_TEXT	ENDS
_DATA	SEGMENT  WORD USE16 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD USE16 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD USE16 PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT  BYTE USE16 PUBLIC 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT  BYTE USE16 PUBLIC 'DEBTYP'
$$TYPES	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP, SS: DGROUP
EXTRN	__acrtused:ABS
EXTRN	_SVGA_GetUserMode:FAR
EXTRN	_VESA_SetMode:FAR
EXTRN	_SVGA_Init:FAR
EXTRN	_SVGA_SetupMode:FAR
EXTRN	_SVGA_Cls:FAR
EXTRN	_strcat:FAR
EXTRN	_SVGA_SetAllPalette:FAR
EXTRN	_strcpy:FAR
EXTRN	_printf:FAR
EXTRN	_SVGA_LoadPCX:FAR
EXTRN	_SetKb:FAR
EXTRN	_SVGA_PrintString:FAR
EXTRN	_ResetKb:FAR
EXTRN	_SVGA_ShowImage:FAR
EXTRN	_SearchFile:FAR
EXTRN	_SVGA_KillImage:FAR
_BSS      SEGMENT
COMM NEAR	_CurrentBank:	BYTE:	 2
COMM NEAR	_BytesPerBank:	BYTE:	 4
COMM NEAR	_BytesPerLine:	BYTE:	 2
COMM NEAR	_WindowControl:	BYTE:	 4
COMM NEAR	_MaxX:	BYTE:	 2
COMM NEAR	_MaxY:	BYTE:	 2
COMM NEAR	_CHAR_WIDTH:	BYTE:	 1
COMM NEAR	_CHAR_HEIGHT:	BYTE:	 1
COMM NEAR	_CHAR_BITS_PER_PIXEL:	BYTE:	 1
COMM NEAR	_CHAR_MAX:	BYTE:	 1
COMM NEAR	_video_buffer:	BYTE:	 4
COMM NEAR	_video_buffer_w:	BYTE:	 4
COMM NEAR	_CharSet:	BYTE:	 4
COMM NEAR	_ROM_SET:	BYTE:	 8
COMM NEAR	_Key:	BYTE:	 1
COMM NEAR	_KeyFlags:	 1:	 128
COMM NEAR	_FileList:	 1:	 1300
COMM NEAR	_FilePath:	 1:	 128
_BSS      ENDS
_DATA      SEGMENT
$SG770	DB	'Specify a PCX to show!!',  00H
$SG771	DB	'.PCX',  00H
$SG773	DB	'File(s) not found.',  0aH,  00H
$SG775	DB	'Files found: %i',  00H
$SG779	DB	0aH, '''%s''',  00H
$SG781	DB	0aH, 'That mode is not supported by your video card!!',  0aH
	DB	00H
_DATA      ENDS
PCX_TEXT      SEGMENT
	ASSUME	CS: PCX_TEXT
	PUBLIC	_main
_main	PROC FAR
; Line 6
	enter	926,0
	push	di
	push	si
;	Filename = -926
;	pcx = -776
;	i = -778
;	tf = -780
;	x = -782
;	y = -784
;	argv = 8
;	argc = 6
; Line 12
	cmp	WORD PTR [bp+6],2	;argc
	jge	SHORT $I769
; Line 14
	push	ds
	push	OFFSET DGROUP:$SG770
$L798:
	call	FAR PTR _printf
	add	sp,4
; Line 15
	pop	si
	pop	di
	leave	
	ret	
; Line 18
$I769:
	call	FAR PTR _SVGA_Init
; Line 20
	push	ds
	push	OFFSET DGROUP:$SG771
	les	bx,DWORD PTR [bp+8]	;argv
	push	DWORD PTR es:[bx+4]
	call	FAR PTR _SearchFile
	add	sp,8
	mov	WORD PTR [bp-780],ax	;tf
; Line 21
	or	ax,ax
	jne	SHORT $I772
; Line 23
	push	ds
	push	OFFSET DGROUP:$SG773
	jmp	SHORT $L798
	nop	
$I772:
; Line 28
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG775
	call	FAR PTR _printf
	add	sp,6
; Line 29
	mov	WORD PTR [bp-782],0	;x
	jmp	SHORT $F776
	nop	
$FC777:
; Line 31
	push	ds
	push	OFFSET DGROUP:_FilePath
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 32
	mov	ax,WORD PTR [bp-782]	;x
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	shl	ax,2
	add	ax,cx
	add	ax,OFFSET DGROUP:_FileList
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	call	FAR PTR _strcat
	add	sp,8
; Line 34
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG779
	call	FAR PTR _printf
	add	sp,8
; Line 35
	inc	WORD PTR [bp-782]	;x
$F776:
	mov	ax,WORD PTR [bp-780]	;tf
	cmp	WORD PTR [bp-782],ax	;x
	jl	SHORT $FC777
; Line 38
	call	FAR PTR _SVGA_GetUserMode
	mov	WORD PTR [bp-782],ax	;x
; Line 39
	push	ax
	call	FAR PTR _SVGA_SetupMode
	add	sp,2
	or	ax,ax
	jne	SHORT $I780
; Line 41
	push	ds
	push	OFFSET DGROUP:$SG781
	jmp	$L798
	nop	
; Line 45
$I780:
	call	FAR PTR _SetKb
; Line 47
	mov	WORD PTR [bp-778],0	;i
	jmp	$F782
$FC787:
; Line 63
	cmp	BYTE PTR _KeyFlags+77,0
	je	SHORT $I789
	mov	ax,WORD PTR [bp-782]	;x
	add	ax,WORD PTR [bp-774]
	cmp	ax,WORD PTR _MaxX
	jb	SHORT $I789
; Line 65
	dec	WORD PTR [bp-782]	;x
	jmp	SHORT $L802
	nop	
$I789:
	cmp	BYTE PTR _KeyFlags+75,0
	je	SHORT $I791
	cmp	WORD PTR [bp-782],0	;x
	jge	SHORT $I791
; Line 70
	inc	WORD PTR [bp-782]	;x
; Line 71
$L802:
	push	0
	push	WORD PTR [bp-784]	;y
	push	WORD PTR [bp-782]	;x
	sub	sp,776	;0308H
	lea	si,WORD PTR [bp-776]	;pcx
	mov	di,sp
	push	ss
	pop	es
	mov	ecx,194	;000000c2H
	rep	movsd
	call	FAR PTR _SVGA_ShowImage
	add	sp,782	;030eH
; Line 73
$I791:
	cmp	BYTE PTR _KeyFlags+80,0
	je	SHORT $I792
	mov	ax,WORD PTR [bp-784]	;y
	add	ax,WORD PTR [bp-772]
	cmp	ax,WORD PTR _MaxY
	jb	SHORT $I792
; Line 75
	dec	WORD PTR [bp-784]	;y
	jmp	SHORT $L804
	nop	
$I792:
	cmp	BYTE PTR _KeyFlags+72,0
	je	SHORT $I794
	cmp	WORD PTR [bp-784],0	;y
	jge	SHORT $I794
; Line 80
	inc	WORD PTR [bp-784]	;y
; Line 81
$L804:
	push	0
$L803:
	push	WORD PTR [bp-784]	;y
	push	WORD PTR [bp-782]	;x
	sub	sp,776	;0308H
	lea	si,WORD PTR [bp-776]	;pcx
	mov	di,sp
	push	ss
	pop	es
	mov	ecx,194	;000000c2H
	rep	movsd
	call	FAR PTR _SVGA_ShowImage
	add	sp,782	;030eH
; Line 83
$I794:
	cmp	BYTE PTR _KeyFlags+28,0
	je	$FC787
; Line 84
$FC796:
	cmp	BYTE PTR _KeyFlags+28,0
	jne	SHORT $FC796
; Line 85
	lea	ax,WORD PTR [bp-776]	;pcx
	push	ss
	push	ax
	call	FAR PTR _SVGA_KillImage
	add	sp,4
; Line 86
$FC783:
	inc	WORD PTR [bp-778]	;i
$F782:
	mov	ax,WORD PTR [bp-778]	;i
	cmp	WORD PTR [bp-780],ax	;tf
	jle	$FB784
; Line 49
	push	ds
	push	OFFSET DGROUP:_FilePath
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	call	FAR PTR _strcpy
	add	sp,8
; Line 50
	push	0
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	push	DWORD PTR 917514	;000e000aH
	push	0
	call	FAR PTR _SVGA_PrintString
	add	sp,12	;000cH
; Line 51
	mov	ax,WORD PTR [bp-778]	;i
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	shl	ax,2
	add	ax,cx
	add	ax,OFFSET DGROUP:_FileList
	push	ds
	push	ax
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	call	FAR PTR _strcat
	add	sp,8
; Line 53
	push	0
	call	FAR PTR _SVGA_Cls
	add	sp,2
; Line 54
	push	0
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	push	DWORD PTR 917504	;000e0000H
	push	0
	call	FAR PTR _SVGA_PrintString
	add	sp,12	;000cH
; Line 55
	lea	ax,WORD PTR [bp-776]	;pcx
	push	ss
	push	ax
	lea	ax,WORD PTR [bp-926]	;Filename
	push	ss
	push	ax
	call	FAR PTR _SVGA_LoadPCX
	add	sp,8
	or	ax,ax
	je	$FC783
; Line 56
	lea	ax,WORD PTR [bp-770]
	push	ss
	push	ax
	call	FAR PTR _SVGA_SetAllPalette
	add	sp,4
; Line 58
	mov	ax,10	;000aH
	mov	WORD PTR [bp-784],ax	;y
	mov	WORD PTR [bp-782],ax	;x
; Line 59
	push	1
	jmp	$L803
$FB784:
; Line 88
	call	FAR PTR _ResetKb
; Line 89
	push	3
	call	FAR PTR _VESA_SetMode
	add	sp,2
; Line 90
	pop	si
	pop	di
	leave	
	ret	
	nop	

_main	ENDP
PCX_TEXT	ENDS
END
