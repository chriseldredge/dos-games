;	Static Name Aliases
;
	TITLE   svga.c
	.MODEL  LARGE
	.386p
	.387
INCLUDELIB      LLIBCE
INCLUDELIB	OLDNAMES.LIB
SVGA_TEXT	SEGMENT  WORD USE16 PUBLIC 'CODE'
SVGA_TEXT	ENDS
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
EXTRN	__fltused:ABS
EXTRN	_MOUSE_Hide:FAR
EXTRN	_MOUSE_Show:FAR
EXTRN	_abs:FAR
EXTRN	_fclose:FAR
EXTRN	_fgetc:FAR
EXTRN	__aFuldiv:FAR
EXTRN	__fmemcpy:FAR
EXTRN	__aFlmul:FAR
EXTRN	__fmemset:FAR
EXTRN	__aFulmul:FAR
EXTRN	_fopen:FAR
EXTRN	_fprintf:FAR
EXTRN	__inp:FAR
EXTRN	_free:FAR
EXTRN	__outp:FAR
EXTRN	_fread:FAR
EXTRN	__disable:FAR
EXTRN	_fscanf:FAR
EXTRN	__aFldiv:FAR
EXTRN	__ffree:FAR
EXTRN	_fseek:FAR
EXTRN	_malloc:FAR
EXTRN	_getch:FAR
EXTRN	_VESA_SetMode:FAR
EXTRN	_printf:FAR
EXTRN	_VESA_ReturnInfo:FAR
EXTRN	_outp:FAR
EXTRN	_VESA_ReturnModeInfo:FAR
EXTRN	_VESA_SetPage:FAR
EXTRN	_memset:FAR
EXTRN	_sprintf:FAR
EXTRN	_XMS_Init:FAR
EXTRN	__enable:FAR
EXTRN	_XMS_Alloc:FAR
EXTRN	_XMS_Free:FAR
EXTRN	_strlen:FAR
EXTRN	_XMS_Copy:FAR
EXTRN	_XMS_CopyFromXMS:FAR
EXTRN	_XMS_CopyToXMS:FAR
EXTRN	__filbuf:FAR
_BSS      SEGMENT
COMM NEAR	_FileList:	 1:	 1300
COMM NEAR	_FilePath:	 1:	 128
COMM NEAR	_dummy:	BYTE:	 2
COMM NEAR	_MOUSE_BUTTON_LEFT:	BYTE:	 2
COMM NEAR	_CurrentBank:	BYTE:	 2
COMM NEAR	_MOUSE_BUTTON_RIGHT:	BYTE:	 2
COMM NEAR	_BytesPerBank:	BYTE:	 4
COMM NEAR	_BytesPerLine:	BYTE:	 2
COMM NEAR	_MOUSE_X:	BYTE:	 2
COMM NEAR	_MOUSE_Y:	BYTE:	 2
COMM NEAR	_WindowControl:	BYTE:	 4
COMM NEAR	_MOUSE_OLD_X:	BYTE:	 2
COMM NEAR	_MaxX:	BYTE:	 2
COMM NEAR	_MOUSE_OLD_Y:	BYTE:	 2
COMM NEAR	_MaxY:	BYTE:	 2
COMM NEAR	_MOUSE_LIMIT_X1:	BYTE:	 2
COMM NEAR	_CHAR_WIDTH:	BYTE:	 1
COMM NEAR	_MOUSE_LIMIT_X2:	BYTE:	 2
COMM NEAR	_CHAR_HEIGHT:	BYTE:	 1
COMM NEAR	_MOUSE_LIMIT_Y1:	BYTE:	 2
COMM NEAR	_CHAR_BITS_PER_PIXEL:	BYTE:	 1
COMM NEAR	_MOUSE_LIMIT_Y2:	BYTE:	 2
COMM NEAR	_CHAR_MAX:	BYTE:	 1
COMM NEAR	_MOUSE_SENSITIVITY_X:	BYTE:	 2
COMM NEAR	_video_buffer:	BYTE:	 4
COMM NEAR	_MOUSE_SENSITIVITY_Y:	BYTE:	 2
COMM NEAR	_video_buffer_w:	BYTE:	 4
COMM NEAR	_MOUSE_Port:	BYTE:	 2
COMM NEAR	_CharSet:	BYTE:	 4
COMM NEAR	_MOUSE_SHOWING:	BYTE:	 2
COMM NEAR	_stx:	 4:	 1024
COMM NEAR	_combytes:	 1:	 3
COMM NEAR	_sts:	 2:	 1024
COMM NEAR	_inbyte:	BYTE:	 1
COMM NEAR	_stb:	 1:	 1024
COMM NEAR	_bytenum:	BYTE:	 1
COMM NEAR	_stp:	 1:	 1024
COMM NEAR	_MOUSE_CURSOR:	 1:	 257
COMM NEAR	_ROM_SET:	BYTE:	 8
COMM NEAR	_BackupSprite:	 1:	 257
COMM NEAR	_XMS_Address:	BYTE:	 4
_BSS      ENDS
_DATA      SEGMENT
$SG945	DB	'V',  00H
$SG947	DB	'E',  00H
$SG949	DB	'S',  00H
$SG950	DB	'A',  00H
$SG1139	DB	'rb',  00H
$SG1167	DB	'rb',  00H
$SG1294	DB	'GetImage Error',  0aH,  00H
$SG1307	DB	00H
$SG1326	DB	'%s%c',  00H
$SG1333	DB	0aH, '(1) 320x200',  0aH, '(2) 640x400',  0aH, '(3) 640x480 '
	DB	'(default)',  0aH, '(4) 800x600',  0aH, '(5) 1024x768',  0aH, '('
	DB	'6) 1280x1024',  0aH, 'Enter the index of the 256 Color SVGA '
	DB	'mode you would like to use: ',  00H
$SG1338	DB	'%c',  0aH,  00H
$SG1525	DB	'r',  00H
$SG1529	DB	'%3d %3d %3d',  00H
$SG1539	DB	'w',  00H
$SG1543	DB	0aH, '%3d %3d %3d',  00H
_DATA      ENDS
SVGA_TEXT      SEGMENT
	ASSUME	CS: SVGA_TEXT
	PUBLIC	_SVGA_Init
_SVGA_Init	PROC FAR
; Line 35
	enter	514,0
;	vesablock = -258
;	vesamodeblock = -514
; Line 39
	call	FAR PTR _XMS_Init
	or	ax,ax
	jne	SHORT $I941
; Line 40
$L1579:
	xor	ax,ax
	leave	
	ret	
	nop	
; Line 42
$I941:
	lea	ax,WORD PTR [bp-258]	;vesablock
	push	ss
	push	ax
	call	FAR PTR _VESA_ReturnInfo
	add	sp,4
; Line 46
	cmp	BYTE PTR [bp-258],0	;vesablock
	jne	SHORT $I944
	mov	ax,OFFSET DGROUP:$SG945
	mov	cx,ds
	or	cx,ax
	jne	SHORT $L1579
$I944:
	cmp	BYTE PTR [bp-257],0
	jne	SHORT $I946
	mov	ax,OFFSET DGROUP:$SG947
	mov	cx,ds
	or	cx,ax
	jne	SHORT $L1579
$I946:
	cmp	BYTE PTR [bp-256],0
	jne	SHORT $I948
	mov	ax,OFFSET DGROUP:$SG949
	mov	cx,ds
	or	cx,ax
	jne	SHORT $L1579
$I948:
	cmp	BYTE PTR [bp-255],0
	jne	SHORT $I942
	mov	ax,OFFSET DGROUP:$SG950
	mov	cx,ds
	or	cx,ax
	jne	SHORT $L1579
; Line 54
$I942:
	mov	WORD PTR _CurrentBank,-1	;ffffH
; Line 57
	mov	BYTE PTR _ROM_SET,8
; Line 58
	mov	BYTE PTR _ROM_SET+1,8
; Line 59
	mov	BYTE PTR _ROM_SET+2,1
; Line 60
	mov	BYTE PTR _ROM_SET+3,255	;00ffH
; Line 61
	mov	DWORD PTR _ROM_SET+4,-268371346	;f000fa6eH
; Line 62
	push	ds
	push	OFFSET DGROUP:_ROM_SET
	push	cs
	call	NEAR PTR _SVGA_SetFont
; Line 63
	mov	DWORD PTR _video_buffer,-1610612736	;a0000000H
; Line 64
	mov	DWORD PTR _video_buffer_w,-1610612736	;a0000000H
; Line 66
	mov	ax,1
; Line 67
	leave	
	ret	

_SVGA_Init	ENDP
	PUBLIC	_SVGA_SetupMode
_SVGA_SetupMode	PROC FAR
; Line 70
	enter	264,0
;	vesamodeblock = -256
;	j = -258
;	page = -260
;	runner = -264
;	VMode = 6
; Line 73
	mov	BYTE PTR [bp-260],0	;page
; Line 74
	mov	DWORD PTR [bp-264],0	;runner
; Line 76
	lea	ax,WORD PTR [bp-256]	;vesamodeblock
	push	ss
	push	ax
	push	WORD PTR [bp+6]	;VMode
	call	FAR PTR _VESA_ReturnModeInfo
	add	sp,6
; Line 78
	push	cs
	call	NEAR PTR _SVGA_Init
	or	ax,ax
	jne	SHORT $I958
; Line 79
$L1580:
	xor	ax,ax
	leave	
	ret	
	nop	
; Line 82
$I958:
	test	BYTE PTR [bp-256],1	;vesamodeblock
	je	SHORT $L1580
; Line 86
	cmp	BYTE PTR [bp-231],8
	jne	SHORT $L1580
; Line 89
	mov	eax,DWORD PTR [bp-244]
	mov	DWORD PTR _WindowControl,eax
; Line 90
	mov	ax,WORD PTR [bp-238]
	mov	WORD PTR _BytesPerLine,ax
; Line 91
	mov	ax,WORD PTR [bp-252]
	sub	dx,dx
	mov	dl,ah
	mov	ah,al
	sub	al,al
	add	ax,ax
	adc	dx,dx
	add	ax,ax
	adc	dx,dx
	mov	WORD PTR _BytesPerBank,ax
	mov	WORD PTR _BytesPerBank+2,dx
; Line 92
	mov	ax,WORD PTR [bp-238]
	mov	WORD PTR _MaxX,ax
	mov	ax,WORD PTR [bp-236]
	mov	WORD PTR _MaxY,ax
; Line 94
	mov	WORD PTR [bp-258],0	;j
	jmp	SHORT $F961
	nop	
$I964:
; Line 104
	mov	bx,WORD PTR [bp-258]	;j
	mov	BYTE PTR _stb[bx],0
$I965:
; Line 106
	cmp	WORD PTR [bp-258],0	;j
	je	SHORT $I966
	mov	bx,WORD PTR [bp-258]	;j
	cmp	BYTE PTR _stb[bx+65535],1
	jne	SHORT $I966
; Line 107
	inc	BYTE PTR [bp-260]	;page
; Line 109
$I966:
	mov	bx,WORD PTR [bp-258]	;j
	mov	al,BYTE PTR [bp-260]	;page
	mov	BYTE PTR _stp[bx],al
; Line 110
	mov	ax,WORD PTR _BytesPerLine
	sub	dx,dx
	add	WORD PTR [bp-264],ax	;runner
	adc	WORD PTR [bp-262],dx
; Line 111
	inc	WORD PTR [bp-258]	;j
$F961:
	mov	ax,WORD PTR _MaxY
	cmp	WORD PTR [bp-258],ax	;j
	jae	SHORT $FB963
; Line 96
	mov	ax,WORD PTR [bp-258]	;j
	mul	WORD PTR _BytesPerLine
	mov	bx,WORD PTR [bp-258]	;j
	shl	bx,2
	mov	WORD PTR _stx[bx],ax
	mov	WORD PTR _stx[bx+2],0
; Line 97
	mov	ax,WORD PTR [bp-264]	;runner
	mov	bx,WORD PTR [bp-258]	;j
	add	bx,bx
	mov	WORD PTR _sts[bx],ax
; Line 98
	mov	ax,WORD PTR _MaxX
	sub	dx,dx
	add	ax,WORD PTR [bp-264]	;runner
	adc	dx,WORD PTR [bp-262]
	add	ax,1
	adc	dx,0
	cmp	dx,WORD PTR _BytesPerBank+2
	jb	$I964
	ja	SHORT $L1544
	cmp	ax,WORD PTR _BytesPerBank
	jb	$I964
$L1544:
; Line 100
	mov	bx,WORD PTR [bp-258]	;j
	mov	BYTE PTR _stb[bx],1
; Line 101
	mov	eax,DWORD PTR _BytesPerBank
	sub	DWORD PTR [bp-264],eax	;runner
; Line 103
	jmp	$I965
$FB963:
; Line 113
	push	WORD PTR [bp+6]	;VMode
	call	FAR PTR _VESA_SetMode
; Line 114
	mov	ax,1
; Line 115
	leave	
	ret	
	nop	

_SVGA_SetupMode	ENDP
	PUBLIC	_SVGA_PutPixel
_SVGA_PutPixel	PROC FAR
; Line 120
	enter	4,0
	push	di
	push	si
;	bank = -2
;	pixel_pos = -4
;	color = 10
;	Yc = 8
;	Xc = 6
; Line 124
	mov	bx,WORD PTR [bp+8]	;Yc
	add	bx,bx
	mov	ax,WORD PTR _sts[bx]
	add	ax,WORD PTR [bp+6]	;Xc
	mov	WORD PTR [bp-4],ax	;pixel_pos
; Line 125
	mov	bx,WORD PTR [bp+8]	;Yc
	mov	al,BYTE PTR _stp[bx]
	cbw	
	mov	WORD PTR [bp-2],ax	;bank
; Line 126
	cmp	BYTE PTR _stb[bx],0
	je	SHORT $I977
; Line 127
	add	bx,bx
	sub	dx,dx
	mov	ax,WORD PTR _sts[bx]
	add	ax,WORD PTR [bp+6]	;Xc
	adc	dx,dx
	cmp	dx,WORD PTR _BytesPerBank+2
	jb	SHORT $I977
	ja	SHORT $L1545
	cmp	ax,WORD PTR _BytesPerBank
	jb	SHORT $I977
$L1545:
; Line 128
	inc	WORD PTR [bp-2]	;bank
; Line 130
$I977:
	call	FAR PTR __disable
; Line 132
	cmp	WORD PTR _MOUSE_SHOWING,0
	je	SHORT $I979
; Line 133
	mov	ax,WORD PTR _MOUSE_X
	cmp	WORD PTR [bp+6],ax	;Xc
	jb	SHORT $I979
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+6]	;Xc
	jbe	SHORT $I979
	mov	ax,WORD PTR _MOUSE_Y
	cmp	WORD PTR [bp+8],ax	;Yc
	jb	SHORT $I979
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+8]	;Yc
	jbe	SHORT $I979
; Line 134
	mov	si,WORD PTR [bp+8]	;Yc
	sub	si,WORD PTR _MOUSE_Y
	shl	si,4
	sub	si,WORD PTR _MOUSE_X
	mov	bx,WORD PTR [bp+6]	;Xc
	cmp	BYTE PTR _MOUSE_CURSOR[bx][si],0
	je	SHORT $I980
; Line 136
	mov	al,BYTE PTR [bp+10]	;color
	mov	si,WORD PTR [bp+8]	;Yc
	sub	si,WORD PTR _MOUSE_Y
	shl	si,4
	sub	si,WORD PTR _MOUSE_X
	mov	BYTE PTR _BackupSprite[bx][si],al
	jmp	SHORT $L1581
$I980:
; Line 141
	mov	al,BYTE PTR [bp+10]	;color
	mov	si,WORD PTR [bp+8]	;Yc
	sub	si,WORD PTR _MOUSE_Y
	shl	si,4
	sub	si,WORD PTR _MOUSE_X
	mov	BYTE PTR _BackupSprite[bx][si],al
; Line 143
$I979:
	mov	ax,WORD PTR _CurrentBank
	cmp	WORD PTR [bp-2],ax	;bank
	je	SHORT $I982
; Line 144
	push	1
	push	WORD PTR [bp-2]	;bank
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 146
$I982:
; Line 148
	mov	ax,-24576	;a000H
; Line 149
	mov	es,ax
; Line 150
	mov	di,WORD PTR [bp-4]	;pixel_pos
; Line 151
	mov	al,BYTE PTR [bp+10]	;color
; Line 152
	stosb
; Line 153
; Line 154
$L1581:
	call	FAR PTR __enable
; Line 155
	pop	si
	pop	di
	leave	
	ret	
	nop	

_SVGA_PutPixel	ENDP
	PUBLIC	_SVGA_GetPixel
_SVGA_GetPixel	PROC FAR
; Line 158
	enter	8,0
	push	di
	push	si
;	bank = -4
;	pixel_pos = -8
;	Yc = 8
;	Xc = 6
; Line 161
	mov	ax,WORD PTR [bp+8]	;Yc
	mul	WORD PTR _BytesPerLine
	add	ax,WORD PTR [bp+6]	;Xc
	adc	dx,0
	mov	WORD PTR [bp-8],ax	;pixel_pos
	mov	WORD PTR [bp-6],dx
; Line 162
	push	DWORD PTR _BytesPerBank
	push	dx
	push	ax
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	DWORD PTR [bp-4],eax	;bank
; Line 163
	imul	DWORD PTR _BytesPerBank
	sub	DWORD PTR [bp-8],eax	;pixel_pos
; Line 165
	mov	ax,WORD PTR _CurrentBank
	sub	dx,dx
	cmp	ax,WORD PTR [bp-4]	;bank
	jne	SHORT $L1548
	cmp	dx,WORD PTR [bp-2]
	je	SHORT $I990
$L1548:
; Line 166
	push	1
	push	WORD PTR [bp-4]	;bank
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 168
$I990:
	cmp	WORD PTR _MOUSE_SHOWING,0
	je	SHORT $I991
; Line 170
	call	FAR PTR __disable
; Line 171
	mov	ax,WORD PTR _MOUSE_X
	cmp	WORD PTR [bp+6],ax	;Xc
	jb	SHORT $I992
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+6]	;Xc
	jbe	SHORT $I992
	mov	ax,WORD PTR _MOUSE_Y
	cmp	WORD PTR [bp+8],ax	;Yc
	jb	SHORT $I992
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+8]	;Yc
	jbe	SHORT $I992
; Line 172
	mov	si,WORD PTR [bp+8]	;Yc
	sub	si,WORD PTR _MOUSE_Y
	shl	si,4
	mov	bx,WORD PTR [bp+6]	;Xc
	sub	si,WORD PTR _MOUSE_X
	mov	al,BYTE PTR _BackupSprite[bx][si]
	pop	si
	pop	di
	leave	
	ret	
; Line 173
$I992:
	call	FAR PTR __enable
; Line 176
$I991:
; Line 178
	mov	ax,-24576	;a000H
; Line 179
	mov	es,ax
; Line 180
	mov	ax,WORD PTR [bp-8]	;pixel_pos
; Line 181
	mov	di,ax
; Line 182
	mov	al,BYTE PTR es:[di]
; Line 183
; Line 184
	pop	si
	pop	di
	leave	
	ret	

_SVGA_GetPixel	ENDP
	PUBLIC	_SVGA_PrintChar
_SVGA_PrintChar	PROC FAR
; Line 188
	enter	10,0
	push	si
;	x = -2
;	y = -4
;	work_char = -8
;	bit_mask = -10
;	trans_flag = 14
;	color = 12
;	c = 10
;	yc = 8
;	xc = 6
; Line 191
	mov	BYTE PTR [bp-10],128	;0080H	;bit_mask
; Line 193
	cmp	BYTE PTR _CHAR_BITS_PER_PIXEL,1
	jne	$I1008
; Line 195
	mov	al,BYTE PTR [bp+10]	;c
	cbw	
	mov	cl,BYTE PTR _CHAR_HEIGHT
	sub	ch,ch
	imul	cx
	add	ax,WORD PTR _CharSet
	mov	dx,WORD PTR _CharSet+2
	mov	WORD PTR [bp-8],ax	;work_char
	mov	WORD PTR [bp-6],dx
; Line 197
	mov	WORD PTR [bp-4],0	;y
	jmp	SHORT $F1009
	nop	
$I1015:
	cmp	WORD PTR [bp+14],1	;trans_flag
	jne	SHORT $I1017
; Line 207
	push	0
$L1582:
	mov	ax,WORD PTR [bp-4]	;y
	add	ax,WORD PTR [bp+8]	;yc
	push	ax
	mov	ax,WORD PTR [bp-2]	;x
	add	ax,WORD PTR [bp+6]	;xc
	push	ax
	push	cs
	call	NEAR PTR _SVGA_PutPixel
	add	sp,6
; Line 209
$I1017:
	shr	BYTE PTR [bp-10],1	;bit_mask
; Line 211
	inc	WORD PTR [bp-2]	;x
$F1012:
	mov	al,BYTE PTR _CHAR_WIDTH
	sub	ah,ah
	cmp	ax,WORD PTR [bp-2]	;x
	jle	SHORT $FB1014
; Line 204
	les	bx,DWORD PTR [bp-8]	;work_char
	mov	al,BYTE PTR es:[bx]
	cbw	
	mov	cl,BYTE PTR [bp-10]	;bit_mask
	sub	ch,ch
	test	ax,cx
	je	SHORT $I1015
; Line 205
	mov	al,BYTE PTR [bp+12]	;color
	push	ax
	jmp	SHORT $L1582
	nop	
$FB1014:
; Line 213
	inc	WORD PTR [bp-8]	;work_char
; Line 215
	inc	WORD PTR [bp-4]	;y
$F1009:
	mov	al,BYTE PTR _CHAR_HEIGHT
	sub	ah,ah
	cmp	ax,WORD PTR [bp-4]	;y
	jle	$FB1021
; Line 199
	mov	BYTE PTR [bp-10],128	;0080H	;bit_mask
; Line 201
	mov	WORD PTR [bp-2],0	;x
	jmp	SHORT $F1012
	nop	
$I1008:
; Line 219
	mov	cl,BYTE PTR _CHAR_HEIGHT
	sub	ch,ch
	mov	al,BYTE PTR [bp+10]	;c
	cbw	
	imul	cx
	mov	cl,BYTE PTR _CHAR_WIDTH
	imul	cx
	add	ax,WORD PTR _CharSet
	mov	dx,WORD PTR _CharSet+2
	mov	WORD PTR [bp-8],ax	;work_char
	mov	WORD PTR [bp-6],dx
; Line 221
	mov	WORD PTR [bp-4],0	;y
	jmp	SHORT $F1019
	nop	
$I1025:
	cmp	WORD PTR [bp+14],1	;trans_flag
	jne	SHORT $I1027
; Line 226
	push	0
$L1583:
	mov	ax,WORD PTR [bp-4]	;y
	add	ax,WORD PTR [bp+8]	;yc
	push	ax
	mov	ax,si
	add	ax,WORD PTR [bp+6]	;xc
	push	ax
	push	cs
	call	NEAR PTR _SVGA_PutPixel
	add	sp,6
; Line 227
$I1027:
	inc	WORD PTR [bp-2]	;x
$F1022:
	mov	al,BYTE PTR _CHAR_WIDTH
	sub	ah,ah
	cmp	ax,WORD PTR [bp-2]	;x
	jle	SHORT $FC1020
; Line 223
	imul	WORD PTR [bp-4]	;y
	mov	bx,ax
	mov	es,WORD PTR [bp-6]
	add	bx,WORD PTR [bp-8]	;work_char
	mov	si,WORD PTR [bp-2]	;x
	cmp	BYTE PTR es:[bx][si],0
	je	SHORT $I1025
; Line 224
	mov	al,BYTE PTR _CHAR_WIDTH
	sub	ah,ah
	imul	WORD PTR [bp-4]	;y
	mov	bx,ax
	add	bx,WORD PTR [bp-8]	;work_char
	mov	al,BYTE PTR es:[bx][si]
	push	ax
	jmp	SHORT $L1583
$FC1020:
	inc	WORD PTR [bp-4]	;y
$F1019:
	mov	al,BYTE PTR _CHAR_HEIGHT
	sub	ah,ah
	cmp	ax,WORD PTR [bp-4]	;y
	jle	SHORT $FB1021
; Line 222
	mov	WORD PTR [bp-2],0	;x
	jmp	SHORT $F1022
$FB1021:
; Line 228
	pop	si
	leave	
	ret	
	nop	

_SVGA_PrintChar	ENDP
	PUBLIC	_SVGA_PrintString
_SVGA_PrintString	PROC FAR
; Line 233
	enter	2,0
	push	si
;	index = -2
;	trans_flag = 16
;	string = 12
;	color = 10
;	y = 8
;	x = 6
; Line 236
	mov	WORD PTR [bp-2],0	;index
	jmp	SHORT $F1040
$FC1041:
; Line 239
	push	WORD PTR [bp+16]	;trans_flag
	push	WORD PTR [bp+10]	;color
	mov	al,BYTE PTR es:[bx][si]
	push	ax
	push	WORD PTR [bp+8]	;y
	mov	al,BYTE PTR _CHAR_WIDTH
	sub	ah,ah
	imul	si
	add	ax,WORD PTR [bp+6]	;x
	push	ax
	push	cs
	call	NEAR PTR _SVGA_PrintChar
	add	sp,10	;000aH
; Line 241
	inc	WORD PTR [bp-2]	;index
$F1040:
	les	bx,DWORD PTR [bp+12]	;string
	mov	si,WORD PTR [bp-2]	;index
	cmp	BYTE PTR es:[bx][si],0
	jne	SHORT $FC1041
; Line 242
	pop	si
	leave	
	ret	
	nop	

_SVGA_PrintString	ENDP
SVGA_TEXT      ENDS
CONST      SEGMENT
$T1549	DD	000000000r   ;	.00000000
CONST      ENDS
SVGA_TEXT      SEGMENT
	ASSUME	CS: SVGA_TEXT
	PUBLIC	_Sgn
_Sgn	PROC FAR
; Line 246
	push	bp
	mov	bp,sp
;	A = 6
; Line 247
	fldz	
	fcom	DWORD PTR [bp+6]	;A
	fstp	ST(0)
	fstsw	ax
	sahf	
	jae	SHORT $I1047
	mov	ax,1
	leave	
	ret	
; Line 248
$I1047:
	fldz	
	fcom	DWORD PTR [bp+6]	;A
	fstp	ST(0)
	fstsw	ax
	sahf	
	jbe	SHORT $I1048
	mov	ax,-1	;ffffH
	leave	
	ret	
	nop	
; Line 249
$I1048:
	fldz	
	fst	DWORD PTR [bp+6]	;A
	fcom	DWORD PTR $T1549
	fstp	ST(0)
	fstsw	ax
	sahf	
	je	SHORT $I1049
	xor	ax,ax
; Line 250
$I1049:
	leave	
	ret	
	nop	

_Sgn	ENDP
	PUBLIC	_SVGA_Line
_SVGA_Line	PROC FAR
; Line 253
	enter	20,0
;	I = -2
;	S = -4
;	D1x = -6
;	D1y = -8
;	D2x = -10
;	D2y = -12
;	U = -14
;	V = -16
;	M = -18
;	N = -20
;	color = 14
;	y2 = 12
;	x2 = 10
;	y1 = 8
;	x1 = 6
; Line 256
	mov	ax,WORD PTR [bp+10]	;x2
	sub	ax,WORD PTR [bp+6]	;x1
	mov	WORD PTR [bp-14],ax	;U
; Line 257
	mov	ax,WORD PTR [bp+12]	;y2
	sub	ax,WORD PTR [bp+8]	;y1
	mov	WORD PTR [bp-16],ax	;V
; Line 258
	fild	WORD PTR [bp-14]	;U
	sub	sp,4
	mov	bx,sp
	fstp	DWORD PTR [bx]
	fwait	
	push	cs
	call	NEAR PTR _Sgn
	add	sp,4
	mov	WORD PTR [bp-6],ax	;D1x
; Line 259
	fild	WORD PTR [bp-16]	;V
	sub	sp,4
	mov	bx,sp
	fstp	DWORD PTR [bx]
	fwait	
	push	cs
	call	NEAR PTR _Sgn
	add	sp,4
	mov	WORD PTR [bp-8],ax	;D1y
; Line 260
	fild	WORD PTR [bp-14]	;U
	sub	sp,4
	mov	bx,sp
	fstp	DWORD PTR [bx]
	fwait	
	push	cs
	call	NEAR PTR _Sgn
	add	sp,4
	mov	WORD PTR [bp-10],ax	;D2x
; Line 261
	mov	WORD PTR [bp-12],0	;D2y
; Line 262
	push	WORD PTR [bp-14]	;U
	call	FAR PTR _abs
	add	sp,2
	mov	WORD PTR [bp-18],ax	;M
; Line 263
	push	WORD PTR [bp-16]	;V
	call	FAR PTR _abs
	add	sp,2
	mov	WORD PTR [bp-20],ax	;N
; Line 264
	mov	ax,WORD PTR [bp-18]	;M
	cmp	WORD PTR [bp-20],ax	;N
	jl	SHORT $I1071
; Line 266
	mov	WORD PTR [bp-10],0	;D2x
; Line 267
	fild	WORD PTR [bp-16]	;V
	sub	sp,4
	mov	bx,sp
	fstp	DWORD PTR [bx]
	fwait	
	push	cs
	call	NEAR PTR _Sgn
	add	sp,4
	mov	WORD PTR [bp-12],ax	;D2y
; Line 268
	push	WORD PTR [bp-16]	;V
	call	FAR PTR _abs
	add	sp,2
	mov	WORD PTR [bp-18],ax	;M
; Line 269
	push	WORD PTR [bp-14]	;U
	call	FAR PTR _abs
	add	sp,2
	mov	WORD PTR [bp-20],ax	;N
; Line 271
$I1071:
	mov	ax,WORD PTR [bp-18]	;M
	sar	ax,1
	mov	WORD PTR [bp-4],ax	;S
; Line 273
	mov	WORD PTR [bp-2],0	;I
	jmp	SHORT $F1072
	nop	
$I1075:
; Line 285
	mov	ax,WORD PTR [bp-10]	;D2x
	add	WORD PTR [bp+6],ax	;x1
; Line 286
	mov	ax,WORD PTR [bp-12]	;D2y
$L1585:
	add	WORD PTR [bp+8],ax	;y1
; Line 288
	inc	WORD PTR [bp-2]	;I
$F1072:
	mov	ax,WORD PTR [bp-2]	;I
	cmp	WORD PTR [bp-18],ax	;M
	jl	SHORT $FB1074
; Line 275
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	DWORD PTR [bp+6]	;x1
	push	cs
	call	NEAR PTR _SVGA_PutPixel
	add	sp,6
; Line 276
	mov	ax,WORD PTR [bp-20]	;N
	add	WORD PTR [bp-4],ax	;S
; Line 277
	mov	ax,WORD PTR [bp-4]	;S
	cmp	WORD PTR [bp-18],ax	;M
	jg	SHORT $I1075
; Line 279
	mov	ax,WORD PTR [bp-18]	;M
	sub	WORD PTR [bp-4],ax	;S
; Line 280
	mov	ax,WORD PTR [bp-6]	;D1x
	add	WORD PTR [bp+6],ax	;x1
; Line 281
	mov	ax,WORD PTR [bp-8]	;D1y
	jmp	SHORT $L1585
	nop	
$FB1074:
; Line 289
	leave	
	ret	

_SVGA_Line	ENDP
	PUBLIC	_SVGA_HLine
_SVGA_HLine	PROC FAR
; Line 292
	enter	6,0
;	ofs = -4
;	bank = -6
;	color = 12
;	y1 = 10
;	x2 = 8
;	x1 = 6
; Line 296
	push	0
	push	WORD PTR _BytesPerLine
	movsx	eax,WORD PTR [bp+10]	;y1
	pop	ecx
	mul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	cx,ax
	mov	ax,WORD PTR [bp+6]	;x1
	mov	bx,dx
	cwd	
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR [bp-4],cx	;ofs
	mov	WORD PTR [bp-2],bx
; Line 297
	push	DWORD PTR _BytesPerBank
	push	bx
	push	cx
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	WORD PTR [bp-6],ax	;bank
; Line 298
	push	DWORD PTR _BytesPerBank
	cwd	
	push	dx
	push	ax
	pop	eax
	pop	ecx
	mul	ecx
	sub	DWORD PTR [bp-4],eax	;ofs
; Line 300
	mov	ax,WORD PTR _CurrentBank
	cmp	WORD PTR [bp-6],ax	;bank
	je	SHORT $I1088
; Line 301
	push	1
	mov	al,BYTE PTR [bp-6]	;bank
	sub	ah,ah
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 303
$I1088:
	cmp	WORD PTR _MOUSE_SHOWING,0
	jne	$I1089
; Line 305
	mov	ax,WORD PTR [bp+8]	;x2
	sub	ax,WORD PTR [bp+6]	;x1
	inc	ax
	cwd	
	add	ax,WORD PTR [bp-4]	;ofs
	adc	dx,WORD PTR [bp-2]
	cmp	dx,WORD PTR _BytesPerBank+2
	jb	SHORT $I1090
	ja	SHORT $L1551
	cmp	ax,WORD PTR _BytesPerBank
	jbe	SHORT $I1090
$L1551:
; Line 307
	mov	ax,WORD PTR _BytesPerBank
	sub	ax,WORD PTR [bp-4]	;ofs
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	mov	ax,WORD PTR [bp-4]	;ofs
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
	call	FAR PTR __fmemset
	add	sp,8
; Line 308
	push	1
	mov	ax,WORD PTR _CurrentBank
	inc	ax
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 309
	mov	ax,WORD PTR [bp-4]	;ofs
	sub	ax,WORD PTR _BytesPerBank
	sub	ax,WORD PTR [bp+6]	;x1
	add	ax,WORD PTR [bp+8]	;x2
	inc	ax
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	push	DWORD PTR _video_buffer
$L1587:
	call	FAR PTR __fmemset
	add	sp,8
; Line 311
	leave	
	ret	
	nop	
$I1090:
; Line 312
	mov	ax,WORD PTR [bp+8]	;x2
	sub	ax,WORD PTR [bp+6]	;x1
	inc	ax
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	mov	ax,WORD PTR [bp-4]	;ofs
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
	jmp	SHORT $L1587
	nop	
$I1089:
; Line 316
	mov	ax,WORD PTR _MOUSE_X
	cmp	WORD PTR [bp+6],ax	;x1
	jg	SHORT $I1095
	cmp	WORD PTR [bp+8],ax	;x2
	jge	SHORT $I1094
$I1095:
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+6]	;x1
	jl	SHORT $I1093
	mov	ax,WORD PTR _MOUSE_X
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+8]	;x2
	jg	SHORT $I1093
$I1094:
	mov	ax,WORD PTR _MOUSE_Y
	cmp	WORD PTR [bp+10],ax	;y1
	jl	SHORT $I1093
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp+10]	;y1
	jl	SHORT $I1093
; Line 317
	movsx	eax,WORD PTR [bp+6]	;x1
	mov	DWORD PTR [bp-4],eax	;ofs
	jmp	SHORT $F1096
	nop	
$FC1097:
	inc	DWORD PTR [bp-4]	;ofs
$F1096:
	movsx	eax,WORD PTR [bp+8]	;x2
	cmp	eax,DWORD PTR [bp-4]	;ofs
	jb	$I1099
$L1552:
; Line 318
	mov	al,BYTE PTR [bp+12]	;color
	push	ax
	push	WORD PTR [bp+10]	;y1
	push	WORD PTR [bp-4]	;ofs
	push	cs
	call	NEAR PTR _SVGA_PutPixel
	add	sp,6
	jmp	SHORT $FC1097
$I1093:
; Line 321
	mov	ax,WORD PTR [bp+8]	;x2
	sub	ax,WORD PTR [bp+6]	;x1
	inc	ax
	cwd	
	add	ax,WORD PTR [bp-4]	;ofs
	adc	dx,WORD PTR [bp-2]
	cmp	dx,WORD PTR _BytesPerBank+2
	jb	SHORT $I1100
	ja	SHORT $L1553
	cmp	ax,WORD PTR _BytesPerBank
	jbe	SHORT $I1100
$L1553:
; Line 323
	mov	ax,WORD PTR _BytesPerBank
	sub	ax,WORD PTR [bp-4]	;ofs
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	mov	ax,WORD PTR [bp-4]	;ofs
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
	call	FAR PTR __fmemset
	add	sp,8
; Line 324
	push	1
	mov	ax,WORD PTR _CurrentBank
	inc	ax
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 325
	mov	ax,WORD PTR [bp-4]	;ofs
	sub	ax,WORD PTR _BytesPerBank
	sub	ax,WORD PTR [bp+6]	;x1
	add	ax,WORD PTR [bp+8]	;x2
	inc	ax
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	push	DWORD PTR _video_buffer
	jmp	SHORT $L1586
	nop	
$I1100:
; Line 328
	mov	ax,WORD PTR [bp+8]	;x2
	sub	ax,WORD PTR [bp+6]	;x1
	inc	ax
	push	ax
	mov	al,BYTE PTR [bp+12]	;color
	sub	ah,ah
	push	ax
	mov	ax,WORD PTR [bp-4]	;ofs
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
$L1586:
	call	FAR PTR __fmemset
; Line 329
$I1099:
; Line 331
	leave	
	ret	

_SVGA_HLine	ENDP
	PUBLIC	_SVGA_Rectangle
_SVGA_Rectangle	PROC FAR
; Line 334
	enter	4,0
;	index = -2
;	m = -4
;	trans = 16
;	color = 14
;	y2 = 12
;	x2 = 10
;	y1 = 8
;	x1 = 6
; Line 338
	cmp	WORD PTR [bp+16],0	;trans
	je	SHORT $I1117
; Line 341
	mov	ax,WORD PTR [bp+8]	;y1
	mov	WORD PTR [bp-2],ax	;index
	jmp	SHORT $F1118
$FC1119:
	inc	WORD PTR [bp-2]	;index
$F1118:
	mov	ax,WORD PTR [bp+12]	;y2
	cmp	WORD PTR [bp-2],ax	;index
	jge	SHORT $I1121
; Line 342
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	WORD PTR [bp-2]	;index
	push	WORD PTR [bp+10]	;x2
	push	WORD PTR [bp+6]	;x1
	push	cs
	call	NEAR PTR _SVGA_HLine
	add	sp,8
	jmp	SHORT $FC1119
	nop	
$I1117:
; Line 346
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	WORD PTR [bp+8]	;y1
	push	WORD PTR [bp+10]	;x2
	push	WORD PTR [bp+6]	;x1
	push	cs
	call	NEAR PTR _SVGA_HLine
	add	sp,8
; Line 347
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	DWORD PTR [bp+10]	;x2
	push	WORD PTR [bp+6]	;x1
	push	cs
	call	NEAR PTR _SVGA_HLine
	add	sp,8
; Line 348
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	WORD PTR [bp+12]	;y2
	push	WORD PTR [bp+6]	;x1
	push	DWORD PTR [bp+6]	;x1
	push	cs
	call	NEAR PTR _SVGA_Line
	add	sp,10	;000aH
; Line 349
	mov	al,BYTE PTR [bp+14]	;color
	push	ax
	push	DWORD PTR [bp+10]	;x2
	push	WORD PTR [bp+8]	;y1
	push	WORD PTR [bp+10]	;x2
	push	cs
	call	NEAR PTR _SVGA_Line
; Line 350
$I1121:
; Line 351
	leave	
	ret	
	nop	

_SVGA_Rectangle	ENDP
	PUBLIC	_SVGA_Cls
_SVGA_Cls	PROC FAR
; Line 355
	enter	6,0
	push	di
	push	si
;	totalbanks = -4
;	i = -6
;	bc = 6
; Line 359
	push	DWORD PTR _BytesPerBank
	mov	ax,WORD PTR _MaxY
	mul	WORD PTR _MaxX
	push	dx
	push	ax
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	inc	eax
	mov	DWORD PTR [bp-4],eax	;totalbanks
	mov	dx,WORD PTR [bp-2]
; Line 361
	mov	BYTE PTR [bp-6],0	;i
	jmp	SHORT $F1127
	nop	
$FC1128:
	inc	BYTE PTR [bp-6]	;i
$F1127:
	mov	al,BYTE PTR [bp-6]	;i
	cbw	
	cwd	
	cmp	dx,WORD PTR [bp-2]
	jg	SHORT $FB1129
	jl	SHORT $L1554
	cmp	ax,WORD PTR [bp-4]	;totalbanks
	jae	SHORT $FB1129
$L1554:
; Line 363
	push	1
	mov	al,BYTE PTR [bp-6]	;i
	cbw	
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 366
	mov	ax,-24576	;a000H
; Line 367
	mov	es,ax
; Line 368
	mov	cx,-32768	;8000H
; Line 369
	mov	al,BYTE PTR [bp+6]	;bc
; Line 370
	mov	ah,al
; Line 372
	rep	stosw
; Line 373
; Line 374
	jmp	SHORT $FC1128
$FB1129:
; Line 375
	push	256	;0100H
	mov	al,BYTE PTR [bp+6]	;bc
	sub	ah,ah
	push	ax
	push	ds
	push	OFFSET DGROUP:_BackupSprite
	call	FAR PTR __fmemset
	add	sp,8
; Line 376
	pop	si
	pop	di
	leave	
	ret	
	nop	

_SVGA_Cls	ENDP
	PUBLIC	_SVGA_LoadFont
_SVGA_LoadFont	PROC FAR
; Line 379
	enter	56,0
	push	si
;	fp = -4
;	Dummy = -54
;	index = -56
;	f = 10
;	fn = 6
; Line 384
	push	ds
	push	OFFSET DGROUP:$SG1139
	push	DWORD PTR [bp+6]	;fn
	call	FAR PTR _fopen
	add	sp,8
	mov	WORD PTR [bp-4],ax	;fp
	mov	WORD PTR [bp-2],dx
	or	dx,ax
	jne	SHORT $I1138
; Line 385
$L1588:
	xor	ax,ax
	pop	si
	leave	
	ret	
; Line 387
$I1138:
	mov	WORD PTR [bp-56],0	;index
	jmp	SHORT $F1140
	nop	
$L1555:
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR __filbuf
	add	sp,4
$L1556:
	mov	si,WORD PTR [bp-56]	;index
	mov	BYTE PTR [bp-54][si],al
	inc	WORD PTR [bp-56]	;index
$F1140:
	cmp	WORD PTR [bp-56],9	;index
	jge	SHORT $FB1142
; Line 388
	les	bx,DWORD PTR [bp-4]	;fp
	dec	WORD PTR es:[bx+4]
	js	SHORT $L1555
	les	bx,DWORD PTR [bp-4]	;fp
	mov	si,WORD PTR es:[bx]
	inc	WORD PTR es:[bx]
	mov	es,WORD PTR es:[bx+2]
	mov	al,BYTE PTR es:[si]
	jmp	SHORT $L1556
$FB1142:
; Line 390
	cmp	BYTE PTR [bp-54],77	;004dH	;Dummy
	jne	SHORT $L1588
; Line 393
	mov	WORD PTR [bp-56],0	;index
	jmp	SHORT $F1144
	nop	
$L1557:
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR __filbuf
	add	sp,4
$L1558:
	mov	si,WORD PTR [bp-56]	;index
	mov	BYTE PTR [bp-54][si],al
	inc	WORD PTR [bp-56]	;index
$F1144:
	cmp	WORD PTR [bp-56],4	;index
	jge	SHORT $FB1146
; Line 394
	les	bx,DWORD PTR [bp-4]	;fp
	dec	WORD PTR es:[bx+4]
	js	SHORT $L1557
	les	bx,DWORD PTR [bp-4]	;fp
	mov	si,WORD PTR es:[bx]
	inc	WORD PTR es:[bx]
	mov	es,WORD PTR es:[bx+2]
	mov	al,BYTE PTR es:[si]
	jmp	SHORT $L1558
$FB1146:
; Line 396
	mov	al,BYTE PTR [bp-54]	;Dummy
	les	bx,DWORD PTR [bp+10]	;f
	mov	BYTE PTR es:[bx+2],al
; Line 397
	mov	al,BYTE PTR [bp-53]
	les	bx,DWORD PTR [bp+10]	;f
	mov	BYTE PTR es:[bx],al
; Line 398
	mov	al,BYTE PTR [bp-52]
	les	bx,DWORD PTR [bp+10]	;f
	mov	BYTE PTR es:[bx+1],al
; Line 399
	mov	al,BYTE PTR [bp-51]
	les	bx,DWORD PTR [bp+10]	;f
	mov	BYTE PTR es:[bx+3],al
; Line 401
	les	bx,DWORD PTR [bp+10]	;f
	mov	al,BYTE PTR es:[bx+1]
	mul	BYTE PTR es:[bx+3]
	sub	ch,ch
	mov	cl,BYTE PTR es:[bx]
	mul	cx
	push	ax
	call	FAR PTR _malloc
	add	sp,2
	les	bx,DWORD PTR [bp+10]	;f
	mov	WORD PTR es:[bx+4],ax
	mov	WORD PTR es:[bx+6],dx
	les	bx,DWORD PTR [bp+10]	;f
	mov	ax,WORD PTR es:[bx+6]
	or	ax,WORD PTR es:[bx+4]
	je	$L1588
; Line 404
	push	DWORD PTR [bp-4]	;fp
	push	1
	mov	al,BYTE PTR es:[bx+1]
	mul	BYTE PTR es:[bx+3]
	sub	ch,ch
	mov	cl,BYTE PTR es:[bx]
	mul	cx
	push	ax
	push	DWORD PTR es:[bx+4]
	call	FAR PTR _fread
	add	sp,12	;000cH
; Line 405
	mov	ax,1
; Line 406
	pop	si
	leave	
	ret	
	nop	

_SVGA_LoadFont	ENDP
	PUBLIC	_SVGA_LoadPCX
_SVGA_LoadPCX	PROC FAR
; Line 409
	enter	164,0
	push	si
;	fp = -4
;	temp = -8
;	i = -10
;	j = -12
;	k = -14
;	count = -18
;	c = -20
;	numbytes = -22
;	l = -24
;	wbuf = -28
;	wptr = -32
;	Offset = -36
;	h = -164
;	img = 10
;	fn = 6
; Line 419
	push	DWORD PTR [bp+10]	;img
	push	cs
	call	NEAR PTR _SVGA_KillImage
	add	sp,4
; Line 420
	les	bx,DWORD PTR [bp+10]	;img
	mov	WORD PTR es:[bx+774],1
; Line 422
	push	ds
	push	OFFSET DGROUP:$SG1167
	push	DWORD PTR [bp+6]	;fn
	call	FAR PTR _fopen
	add	sp,8
	mov	WORD PTR [bp-4],ax	;fp
	mov	WORD PTR [bp-2],dx
	or	dx,ax
	jne	SHORT $I1166
; Line 423
$L1589:
	xor	ax,ax
	pop	si
	leave	
	ret	
	nop	
; Line 425
$I1166:
	lea	ax,WORD PTR [bp-164]	;h
	mov	WORD PTR [bp-8],ax	;temp
	mov	WORD PTR [bp-6],ss
; Line 426
	mov	WORD PTR [bp-10],0	;i
$FC1169:
; Line 427
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR _fgetc
	add	sp,4
	les	bx,DWORD PTR [bp-8]	;temp
	mov	si,WORD PTR [bp-10]	;i
	mov	BYTE PTR es:[bx][si],al
	inc	WORD PTR [bp-10]	;i
	cmp	WORD PTR [bp-10],128	;0080H	;i
	jl	SHORT $FC1169
; Line 429
	cmp	BYTE PTR [bp-161],8
	jne	SHORT $L1589
; Line 431
	push	DWORD PTR 196607	;0002ffffH
	push	-768	;fd00H
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR _fseek
	add	sp,10	;000aH
; Line 432
	push	DWORD PTR [bp-4]	;fp
	push	DWORD PTR 66304	;00010300H
	mov	eax,DWORD PTR [bp+10]	;img
	add	ax,6
	push	eax
	call	FAR PTR _fread
	add	sp,12	;000cH
; Line 434
	mov	WORD PTR [bp-10],0	;i
$FC1173:
; Line 436
	mov	si,WORD PTR [bp-10]	;i
	mov	ax,si
	add	si,si
	add	si,ax
	les	bx,DWORD PTR [bp+10]	;img
	shr	BYTE PTR es:[bx+6][si],2
; Line 437
	mov	si,WORD PTR [bp-10]	;i
	mov	ax,si
	add	si,si
	add	si,ax
	les	bx,DWORD PTR [bp+10]	;img
	shr	BYTE PTR es:[bx+7][si],2
; Line 438
	mov	si,WORD PTR [bp-10]	;i
	mov	ax,si
	add	si,si
	add	si,ax
	les	bx,DWORD PTR [bp+10]	;img
	shr	BYTE PTR es:[bx+8][si],2
; Line 439
	inc	WORD PTR [bp-10]	;i
	cmp	WORD PTR [bp-10],256	;0100H	;i
	jl	SHORT $FC1173
; Line 441
	push	DWORD PTR 0
	push	128	;0080H
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR _fseek
	add	sp,10	;000aH
; Line 443
	mov	ax,WORD PTR [bp-156]
	sub	ax,WORD PTR [bp-160]
	inc	ax
	les	bx,DWORD PTR [bp+10]	;img
	mov	WORD PTR es:[bx+2],ax
; Line 444
	mov	ax,WORD PTR [bp-154]
	sub	ax,WORD PTR [bp-158]
	inc	ax
	les	bx,DWORD PTR [bp+10]	;img
	mov	WORD PTR es:[bx+4],ax
; Line 446
	push	DWORD PTR 1024	;00000400H
	les	bx,DWORD PTR [bp+10]	;img
	mov	ax,WORD PTR es:[bx+2]
	imul	WORD PTR es:[bx+4]
	push	dx
	push	ax
	pop	eax
	pop	ecx
	cdq	
	idiv	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	inc	ax
	push	ax
	call	FAR PTR _XMS_Alloc
	add	sp,2
	les	bx,DWORD PTR [bp+10]	;img
	mov	WORD PTR es:[bx],ax
; Line 447
	les	bx,DWORD PTR [bp+10]	;img
	mov	ax,WORD PTR es:[bx+2]
	inc	ax
	push	ax
	call	FAR PTR _malloc
	add	sp,2
	mov	WORD PTR [bp-28],ax	;wbuf
	mov	WORD PTR [bp-26],dx
; Line 449
	sub	eax,eax
	mov	DWORD PTR [bp-36],eax	;Offset
; Line 451
	mov	WORD PTR [bp-12],ax	;j
	jmp	$F1175
$FC1179:
; Line 456
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR _fgetc
	add	sp,4
	mov	BYTE PTR [bp-20],al	;c
; Line 457
	cmp	al,191	;00bfH
	jbe	SHORT $I1181
; Line 459
	add	al,64	;0040H
	mov	BYTE PTR [bp-24],al	;l
; Line 460
	push	DWORD PTR [bp-4]	;fp
	call	FAR PTR _fgetc
	add	sp,4
	mov	BYTE PTR [bp-20],al	;c
; Line 461
	mov	al,BYTE PTR [bp-24]	;l
	sub	ah,ah
	push	ax
	mov	al,BYTE PTR [bp-20]	;c
	push	ax
	push	DWORD PTR [bp-32]	;wptr
	call	FAR PTR _memset
	add	sp,8
; Line 462
	mov	al,BYTE PTR [bp-24]	;l
	sub	ah,ah
	add	WORD PTR [bp-32],ax	;wptr
; Line 463
	add	WORD PTR [bp-14],ax	;k
; Line 465
	jmp	SHORT $L1592
$I1181:
; Line 467
	mov	al,BYTE PTR [bp-20]	;c
	les	bx,DWORD PTR [bp-32]	;wptr
	mov	BYTE PTR es:[bx],al
; Line 468
	inc	WORD PTR [bp-32]	;wptr
	inc	WORD PTR [bp-14]	;k
; Line 470
$L1592:
	mov	ax,WORD PTR [bp-14]	;k
	les	bx,DWORD PTR [bp+10]	;img
	cmp	WORD PTR es:[bx+2],ax
	jg	SHORT $FC1179
; Line 471
	push	DWORD PTR [bp-36]	;Offset
	movsx	eax,WORD PTR es:[bx+2]
	push	eax
	push	WORD PTR es:[bx]
	push	DWORD PTR [bp-28]	;wbuf
	call	FAR PTR _XMS_CopyToXMS
	add	sp,14	;000eH
; Line 472
	les	bx,DWORD PTR [bp+10]	;img
	movsx	eax,WORD PTR es:[bx+2]
	add	DWORD PTR [bp-36],eax	;Offset
; Line 473
	inc	WORD PTR [bp-12]	;j
$F1175:
	mov	ax,WORD PTR [bp-12]	;j
	les	bx,DWORD PTR [bp+10]	;img
	cmp	WORD PTR es:[bx+4],ax
	jle	SHORT $FB1177
; Line 453
	mov	eax,DWORD PTR [bp-28]	;wbuf
	mov	DWORD PTR [bp-32],eax	;wptr
; Line 454
	mov	WORD PTR [bp-14],0	;k
	jmp	SHORT $L1592
	nop	
$FB1177:
; Line 475
	push	DWORD PTR [bp-28]	;wbuf
	call	FAR PTR __ffree
	add	sp,4
; Line 476
	mov	ax,1
; Line 477
	pop	si
	leave	
	ret	

_SVGA_LoadPCX	ENDP
	PUBLIC	_SVGA_SetFont
_SVGA_SetFont	PROC FAR
; Line 480
	push	bp
	mov	bp,sp
;	f = 6
; Line 481
	les	bx,DWORD PTR [bp+6]	;f
	mov	al,BYTE PTR es:[bx+3]
	mov	BYTE PTR _CHAR_MAX,al
; Line 482
	mov	al,BYTE PTR es:[bx+1]
	mov	BYTE PTR _CHAR_HEIGHT,al
; Line 483
	mov	al,BYTE PTR es:[bx]
	mov	BYTE PTR _CHAR_WIDTH,al
; Line 484
	mov	al,BYTE PTR es:[bx+2]
	mov	BYTE PTR _CHAR_BITS_PER_PIXEL,al
; Line 485
	mov	eax,DWORD PTR es:[bx+4]
	mov	DWORD PTR _CharSet,eax
; Line 486
	leave	
	ret	

_SVGA_SetFont	ENDP
	PUBLIC	_SVGA_ShowImage
_SVGA_ShowImage	PROC FAR
; Line 489
	enter	70,0
	push	di
	push	si
;	p = -64
;	nw = -66
;	nbo = -68
;	nbs = -70
;	buf = -4
;	nb = -8
;	sx = -10
;	sy = -12
;	yc = -14
;	w = -16
;	h = -18
;	i = -20
;	d = -22
;	bufseg = -24
;	bufoff = -26
;	offset = -30
;	p_off = -34
;	bank = -38
;	x_off = -42
;	y_off = -46
;	work1 = -50
;	work2 = -54
;	LastPage = -58
;	NoOfPages = -60
;	m = -62
;	rop = 786
;	y = 784
;	x = 782
;	img = 6
; Line 502
	mov	ax,WORD PTR [bp+8]
	mov	WORD PTR [bp-16],ax	;w
	mov	ax,WORD PTR [bp+10]
	mov	WORD PTR [bp-18],ax	;h
; Line 504
	mov	ax,WORD PTR _BytesPerLine
	cmp	WORD PTR [bp-16],ax	;w
	jne	$I1220
	cmp	WORD PTR [bp+786],0	;rop
	jne	$I1220
	cmp	WORD PTR [bp+782],0	;x
	jne	$I1220
	cmp	WORD PTR [bp+784],0	;y
	jne	$I1220
; Line 506
	mov	ax,WORD PTR [bp-18]	;h
	imul	WORD PTR [bp-16]	;w
	mov	WORD PTR [bp-50],ax	;work1
	mov	WORD PTR [bp-48],dx
; Line 507
	push	DWORD PTR _BytesPerBank
	push	dx
	push	ax
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	WORD PTR [bp-60],ax	;NoOfPages
; Line 509
	push	DWORD PTR _BytesPerBank
	cwd	
	push	dx
	push	ax
	pop	eax
	pop	ecx
	imul	ecx
	mov	DWORD PTR [bp-54],eax	;work2
; Line 510
	mov	ax,WORD PTR [bp-50]	;work1
	mov	dx,WORD PTR [bp-48]
	sub	ax,WORD PTR [bp-54]	;work2
	sbb	dx,WORD PTR [bp-52]
	mov	WORD PTR [bp-58],ax	;LastPage
	mov	WORD PTR [bp-56],dx
; Line 512
	sub	eax,eax
	mov	DWORD PTR [bp-30],eax	;offset
; Line 514
	mov	WORD PTR [bp-20],ax	;i
	jmp	SHORT $F1221
	nop	
$FC1222:
; Line 516
	push	1
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 517
	push	DWORD PTR [bp-30]	;offset
	push	DWORD PTR _BytesPerBank
	push	WORD PTR [bp+6]	;img
	push	DWORD PTR -1610612736	;a0000000H
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 518
	mov	ax,WORD PTR _BytesPerBank
	mov	dx,WORD PTR _BytesPerBank+2
	add	WORD PTR [bp-30],ax	;offset
	adc	WORD PTR [bp-28],dx
; Line 519
	inc	WORD PTR [bp-20]	;i
$F1221:
	mov	ax,WORD PTR [bp-20]	;i
	cmp	WORD PTR [bp-60],ax	;NoOfPages
	jg	SHORT $FC1222
; Line 521
	mov	ax,WORD PTR [bp-56]
	or	ax,WORD PTR [bp-58]	;LastPage
	je	$FB1264
; Line 523
	push	1
	push	WORD PTR [bp-20]	;i
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 524
	push	DWORD PTR [bp-30]	;offset
	push	DWORD PTR [bp-58]	;LastPage
	push	WORD PTR [bp+6]	;img
	push	DWORD PTR -1610612736	;a0000000H
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 527
	jmp	$FB1264
$I1220:
; Line 529
	cmp	WORD PTR [bp+782],0	;x
	jge	SHORT $I1226
; Line 531
	mov	ax,WORD PTR [bp-16]	;w
	add	ax,WORD PTR [bp+782]	;x
	or	ax,ax
	jle	$I1268
; Line 534
	push	WORD PTR [bp+782]	;x
	call	FAR PTR _abs
	add	sp,2
	cwd	
	mov	WORD PTR [bp-42],ax	;x_off
	mov	WORD PTR [bp-40],dx
; Line 535
	mov	WORD PTR [bp-10],0	;sx
; Line 536
	sub	WORD PTR [bp-16],ax	;w
; Line 538
	jmp	SHORT $I1228
	nop	
$I1226:
	mov	ax,WORD PTR _MaxX
	cmp	WORD PTR [bp+782],ax	;x
	jae	$I1268
; Line 542
	mov	ax,WORD PTR [bp+782]	;x
	mov	WORD PTR [bp-10],ax	;sx
; Line 543
	sub	eax,eax
	mov	DWORD PTR [bp-42],eax	;x_off
; Line 544
$I1228:
; Line 546
	cmp	WORD PTR [bp+784],0	;y
	jge	SHORT $I1231
; Line 548
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp+784]	;y
	js	$I1268
; Line 550
	push	WORD PTR [bp+784]	;y
	call	FAR PTR _abs
	add	sp,2
	imul	WORD PTR [bp+8]
	mov	WORD PTR [bp-46],ax	;y_off
	mov	WORD PTR [bp-44],dx
; Line 551
	mov	WORD PTR [bp-12],0	;sy
; Line 552
	push	WORD PTR [bp+784]	;y
	call	FAR PTR _abs
	add	sp,2
	sub	WORD PTR [bp-18],ax	;h
; Line 554
	jmp	SHORT $I1233
$I1231:
	mov	ax,WORD PTR _MaxY
	cmp	WORD PTR [bp+784],ax	;y
	jae	$I1268
; Line 558
	mov	ax,WORD PTR [bp+784]	;y
	mov	WORD PTR [bp-12],ax	;sy
; Line 559
	mov	DWORD PTR [bp-46],0	;y_off
; Line 560
$I1233:
; Line 562
	mov	ax,WORD PTR [bp-16]	;w
	add	ax,WORD PTR [bp-10]	;sx
	cmp	ax,WORD PTR _MaxX
	jbe	SHORT $I1236
; Line 563
	mov	ax,WORD PTR _MaxX
	sub	ax,WORD PTR [bp-10]	;sx
	mov	WORD PTR [bp-16],ax	;w
; Line 565
$I1236:
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp-12]	;sy
	cmp	ax,WORD PTR _MaxY
	jbe	SHORT $I1237
; Line 566
	mov	ax,WORD PTR _MaxY
	sub	ax,WORD PTR [bp-12]	;sy
	inc	ax
	mov	WORD PTR [bp-18],ax	;h
; Line 568
$I1237:
	mov	ax,WORD PTR [bp-46]	;y_off
	mov	dx,WORD PTR [bp-44]
	add	ax,WORD PTR [bp-42]	;x_off
	adc	dx,WORD PTR [bp-40]
	mov	WORD PTR [bp-30],ax	;offset
	mov	WORD PTR [bp-28],dx
; Line 569
	mov	ax,WORD PTR [bp-12]	;sy
	mov	WORD PTR [bp-14],ax	;yc
; Line 571
	cmp	WORD PTR [bp-16],1	;w
	jl	$I1268
; Line 574
	cmp	WORD PTR _MOUSE_SHOWING,0
	je	SHORT $I1239
; Line 585
	mov	ax,WORD PTR _MOUSE_X
	cmp	WORD PTR [bp-10],ax	;sx
	jg	SHORT $I1242
	mov	ax,WORD PTR [bp-16]	;w
	add	ax,WORD PTR [bp-10]	;sx
	cmp	ax,WORD PTR _MOUSE_X
	jg	SHORT $I1241
$I1242:
	mov	ax,WORD PTR _MOUSE_X
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp-10]	;sx
	jl	SHORT $I1239
	mov	ax,WORD PTR [bp-16]	;w
	add	ax,WORD PTR [bp-10]	;sx
	mov	cx,WORD PTR _MOUSE_X
	add	cx,16	;0010H
	cmp	ax,cx
	jle	SHORT $I1239
$I1241:
	mov	ax,WORD PTR _MOUSE_Y
	cmp	WORD PTR [bp-12],ax	;sy
	jg	SHORT $I1244
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp-12]	;sy
	cmp	ax,WORD PTR _MOUSE_Y
	jg	SHORT $I1243
$I1244:
	mov	ax,WORD PTR _MOUSE_Y
	add	ax,16	;0010H
	cmp	ax,WORD PTR [bp-12]	;sy
	jl	SHORT $I1239
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp-12]	;sy
	mov	cx,WORD PTR _MOUSE_Y
	add	cx,16	;0010H
	cmp	ax,cx
	jle	SHORT $I1239
$I1243:
; Line 587
	call	FAR PTR __disable
; Line 588
	mov	WORD PTR [bp-62],1	;m
; Line 589
	call	FAR PTR _MOUSE_Hide
; Line 591
	jmp	SHORT $I1246
	nop	
$I1239:
; Line 594
	mov	WORD PTR [bp-62],0	;m
$I1246:
; Line 596
	cmp	WORD PTR [bp+786],0	;rop
	je	$I1247
; Line 598
	push	WORD PTR [bp-16]	;w
	call	FAR PTR _malloc
	add	sp,2
	mov	WORD PTR [bp-4],ax	;buf
	mov	WORD PTR [bp-2],dx
; Line 599
	mov	WORD PTR [bp-24],dx	;bufseg
; Line 600
	mov	WORD PTR [bp-26],ax	;bufoff
; Line 602
	mov	ax,WORD PTR [bp-12]	;sy
	mov	WORD PTR [bp-20],ax	;i
	jmp	SHORT $F1248
	nop	
$FC1249:
	inc	WORD PTR [bp-20]	;i
$F1248:
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp-12]	;sy
	cmp	ax,WORD PTR [bp-20]	;i
	jle	$FB1250
; Line 604
	push	DWORD PTR [bp-30]	;offset
	movsx	eax,WORD PTR [bp-16]	;w
	push	eax
	push	WORD PTR [bp+6]	;img
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 605
	push	0
	push	WORD PTR _BytesPerLine
	movsx	eax,WORD PTR [bp-14]	;yc
	pop	ecx
	imul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	cx,ax
	mov	ax,WORD PTR [bp-10]	;sx
	mov	bx,dx
	cwd	
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR [bp-34],cx	;p_off
	mov	WORD PTR [bp-32],bx
; Line 606
	push	DWORD PTR _BytesPerBank
	push	bx
	push	cx
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	DWORD PTR [bp-38],eax	;bank
; Line 607
	imul	DWORD PTR _BytesPerBank
	sub	DWORD PTR [bp-34],eax	;p_off
; Line 609
	mov	ax,WORD PTR _CurrentBank
	sub	dx,dx
	cmp	ax,WORD PTR [bp-38]	;bank
	jne	SHORT $L1560
	cmp	dx,WORD PTR [bp-36]
	je	SHORT $I1251
$L1560:
; Line 610
	push	1
	push	WORD PTR [bp-38]	;bank
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 613
$I1251:
	movsx	eax,WORD PTR [bp-16]	;w
	add	eax,DWORD PTR [bp-34]	;p_off
	mov	edx,eax
	shr	edx,16	;00000010H
	cmp	eax,DWORD PTR _BytesPerBank
	jae	SHORT $I1252
$L1561:
; Line 617
	cld	
; Line 618
	push	ds
; Line 619
	push	si
; Line 621
	mov	ax,-24576	;a000H
; Line 622
	mov	es,ax
; Line 623
	mov	di,WORD PTR [bp-34]	;p_off
; Line 624
	mov	ax,WORD PTR [bp-24]	;bufseg
; Line 625
	mov	ds,ax
; Line 626
	mov	si,WORD PTR [bp-26]	;bufoff
; Line 627
	mov	cx,WORD PTR [bp-16]	;w
; Line 629
$LoopStart1253:
; Line 630
	lodsb
; Line 631
	cmp	al,0
; Line 632
	jne	SHORT $DoPixel1254
; Line 634
	inc	di
; Line 635
	jmp	$Finish1255
	nop	
; Line 636
$DoPixel1254:
; Line 637
	stosb
; Line 638
$Finish1255:
; Line 639
	loop	$LoopStart1253
; Line 641
	pop	si
; Line 642
	pop	ds
; Line 643
; Line 645
	jmp	SHORT $I1256
	nop	
$I1252:
; Line 647
	mov	WORD PTR [bp-22],0	;d
	jmp	SHORT $F1257
	nop	
$FC1258:
; Line 648
	les	bx,DWORD PTR [bp-4]	;buf
	mov	si,WORD PTR [bp-22]	;d
	cmp	BYTE PTR es:[bx][si],0
	je	SHORT $I1260
	mov	al,BYTE PTR es:[bx][si]
	push	ax
	push	WORD PTR [bp-20]	;i
	mov	ax,si
	add	ax,WORD PTR [bp-10]	;sx
	push	ax
	push	cs
	call	NEAR PTR _SVGA_PutPixel
	add	sp,6
; Line 649
$I1260:
	inc	WORD PTR [bp-22]	;d
$F1257:
	mov	ax,WORD PTR [bp-16]	;w
	cmp	WORD PTR [bp-22],ax	;d
	jl	SHORT $FC1258
$I1256:
; Line 650
	mov	ax,WORD PTR [bp+8]
	cwd	
	add	WORD PTR [bp-30],ax	;offset
	adc	WORD PTR [bp-28],dx
; Line 651
	inc	WORD PTR [bp-14]	;yc
; Line 652
	jmp	$FC1249
	nop	
$FB1250:
; Line 653
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _free
	add	sp,4
; Line 655
	jmp	$FB1264
	nop	
$I1247:
; Line 657
	mov	ax,WORD PTR [bp-12]	;sy
	mov	WORD PTR [bp-20],ax	;i
	jmp	SHORT $F1262
$I1266:
; Line 675
	push	DWORD PTR [bp-30]	;offset
	mov	eax,DWORD PTR _BytesPerBank
	sub	eax,DWORD PTR [bp-34]	;p_off
	push	eax
	push	WORD PTR [bp+6]	;img
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 676
	push	1
	mov	ax,WORD PTR _CurrentBank
	inc	ax
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 677
	mov	DWORD PTR [bp-4],-1610612736	;a0000000H	;buf
; Line 678
	mov	eax,DWORD PTR [bp-30]	;offset
	sub	eax,DWORD PTR [bp-34]	;p_off
	add	eax,DWORD PTR _BytesPerBank
	push	eax
	movsx	eax,WORD PTR [bp-16]	;w
	sub	eax,DWORD PTR _BytesPerBank
	add	eax,DWORD PTR [bp-34]	;p_off
	mov	edx,eax
	shr	edx,16	;00000010H
$L1595:
	push	dx
	push	ax
	push	WORD PTR [bp+6]	;img
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 679
	mov	ax,WORD PTR [bp+8]
	cwd	
	add	WORD PTR [bp-30],ax	;offset
	adc	WORD PTR [bp-28],dx
; Line 681
	inc	WORD PTR [bp-20]	;i
$F1262:
	mov	ax,WORD PTR [bp-18]	;h
	add	ax,WORD PTR [bp-12]	;sy
	cmp	ax,WORD PTR [bp-20]	;i
	jle	$FB1264
; Line 659
	push	0
	push	WORD PTR _BytesPerLine
	movsx	eax,WORD PTR [bp-20]	;i
	pop	ecx
	imul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	cx,ax
	mov	ax,WORD PTR [bp-10]	;sx
	mov	bx,dx
	cwd	
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR [bp-34],cx	;p_off
	mov	WORD PTR [bp-32],bx
; Line 660
	push	DWORD PTR _BytesPerBank
	push	bx
	push	cx
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	DWORD PTR [bp-38],eax	;bank
; Line 661
	imul	DWORD PTR _BytesPerBank
	sub	DWORD PTR [bp-34],eax	;p_off
; Line 663
	mov	ax,WORD PTR _CurrentBank
	sub	dx,dx
	cmp	ax,WORD PTR [bp-38]	;bank
	jne	SHORT $L1562
	cmp	dx,WORD PTR [bp-36]
	je	SHORT $I1265
$L1562:
; Line 664
	push	1
	push	WORD PTR [bp-38]	;bank
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 666
$I1265:
	mov	ax,WORD PTR [bp-34]	;p_off
	sub	dx,dx
	sub	ax,dx
	sbb	dx,24576	;6000H
	mov	WORD PTR [bp-4],ax	;buf
	mov	WORD PTR [bp-2],dx
; Line 668
	movsx	eax,WORD PTR [bp-16]	;w
	add	eax,DWORD PTR [bp-34]	;p_off
	mov	edx,eax
	shr	edx,16	;00000010H
	cmp	eax,DWORD PTR _BytesPerBank
	jae	$I1266
$L1563:
; Line 670
	push	DWORD PTR [bp-30]	;offset
	mov	ax,WORD PTR [bp-16]	;w
	cwd	
	jmp	$L1595
	nop	
$FB1264:
; Line 684
	cmp	WORD PTR [bp-62],0	;m
	je	SHORT $I1268
; Line 686
	call	FAR PTR __enable
; Line 687
	call	FAR PTR _MOUSE_Show
; Line 691
$I1268:
	pop	si
	pop	di
	leave	
	ret	

_SVGA_ShowImage	ENDP
	PUBLIC	_SVGA_GetImage
_SVGA_GetImage	PROC FAR
; Line 694
	enter	32,0
;	buf = -4
;	bufseg = -6
;	bufoff = -8
;	sx = -10
;	sy = -12
;	w = -14
;	h = -16
;	yc = -18
;	i = -20
;	p_off = -24
;	bank = -28
;	o = -32
;	height = 16
;	width = 14
;	y = 12
;	x = 10
;	img = 6
; Line 695
	mov	DWORD PTR [bp-4],0	;buf
; Line 701
	push	DWORD PTR [bp+6]	;img
	push	cs
	call	NEAR PTR _SVGA_KillImage
	add	sp,4
; Line 702
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+774],1
; Line 704
	mov	ax,WORD PTR [bp+14]	;width
	mov	WORD PTR [bp-14],ax	;w
; Line 705
	mov	ax,WORD PTR [bp+16]	;height
	mov	WORD PTR [bp-16],ax	;h
; Line 706
	mov	ax,WORD PTR [bp+10]	;x
	mov	WORD PTR [bp-10],ax	;sx
; Line 707
	mov	ax,WORD PTR [bp+12]	;y
	mov	WORD PTR [bp-12],ax	;sy
; Line 709
	mov	ax,WORD PTR [bp+14]	;width
	add	ax,WORD PTR [bp+10]	;x
	cmp	ax,WORD PTR _MaxX
	jbe	SHORT $I1292
; Line 710
	mov	ax,WORD PTR _MaxX
	sub	ax,WORD PTR [bp+10]	;x
	mov	WORD PTR [bp-14],ax	;w
; Line 712
$I1292:
	mov	ax,WORD PTR [bp-14]	;w
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+2],ax
; Line 713
	mov	ax,WORD PTR [bp-16]	;h
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+4],ax
; Line 714
	push	DWORD PTR 1024	;00000400H
	mov	ax,WORD PTR [bp-16]	;h
	imul	WORD PTR [bp-14]	;w
	push	dx
	push	ax
	pop	eax
	pop	ecx
	cdq	
	idiv	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	inc	ax
	push	ax
	call	FAR PTR _XMS_Alloc
	add	sp,2
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx],ax
; Line 716
	push	WORD PTR [bp-14]	;w
	call	FAR PTR _malloc
	add	sp,2
	mov	WORD PTR [bp-4],ax	;buf
	mov	WORD PTR [bp-2],dx
	mov	ax,dx
	or	ax,WORD PTR [bp-4]	;buf
	jne	SHORT $I1293
; Line 718
	push	ds
	push	OFFSET DGROUP:$SG1294
	call	FAR PTR _printf
	add	sp,4
; Line 721
$I1293:
; Line 723
	mov	ax,WORD PTR [bp-12]	;sy
	mov	WORD PTR [bp-18],ax	;yc
; Line 724
	mov	DWORD PTR [bp-32],0	;o
; Line 726
	mov	ax,WORD PTR [bp-12]	;sy
	mov	WORD PTR [bp-20],ax	;i
	jmp	$F1295
	nop	
$I1299:
; Line 738
	mov	ax,WORD PTR _BytesPerBank
	sub	ax,WORD PTR [bp-24]	;p_off
	push	ax
	mov	ax,WORD PTR [bp-24]	;p_off
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR __fmemcpy
	add	sp,10	;000aH
; Line 739
	push	1
	mov	ax,WORD PTR _CurrentBank
	inc	ax
	push	ax
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 740
	mov	ax,WORD PTR [bp-24]	;p_off
	sub	ax,WORD PTR _BytesPerBank
	add	ax,WORD PTR [bp-14]	;w
	push	ax
	push	DWORD PTR _video_buffer
	mov	eax,DWORD PTR [bp-4]	;buf
	sub	ax,WORD PTR [bp-14]	;w
	add	ax,WORD PTR _BytesPerBank
	push	eax
$L1596:
	call	FAR PTR __fmemcpy
	add	sp,10	;000aH
; Line 742
	push	DWORD PTR [bp-32]	;o
	movsx	eax,WORD PTR [bp-14]	;w
	push	eax
	les	bx,DWORD PTR [bp+6]	;img
	push	WORD PTR es:[bx]
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _XMS_CopyToXMS
	add	sp,14	;000eH
; Line 743
	movsx	eax,WORD PTR [bp-14]	;w
	add	DWORD PTR [bp-32],eax	;o
; Line 744
	inc	WORD PTR [bp-18]	;yc
; Line 745
	inc	WORD PTR [bp-20]	;i
$F1295:
	mov	ax,WORD PTR [bp-16]	;h
	add	ax,WORD PTR [bp-12]	;sy
	cmp	ax,WORD PTR [bp-20]	;i
	jle	$FB1297
; Line 728
	push	0
	push	WORD PTR _BytesPerLine
	movsx	eax,WORD PTR [bp-18]	;yc
	pop	ecx
	mul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	mov	cx,ax
	mov	ax,WORD PTR [bp-10]	;sx
	mov	bx,dx
	cwd	
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR [bp-24],cx	;p_off
	mov	WORD PTR [bp-22],bx
; Line 729
	push	DWORD PTR _BytesPerBank
	push	bx
	push	cx
	pop	eax
	pop	ecx
	xor	edx,edx
	div	ecx
	mov	DWORD PTR [bp-28],eax	;bank
; Line 730
	mul	DWORD PTR _BytesPerBank
	sub	DWORD PTR [bp-24],eax	;p_off
; Line 732
	mov	ax,WORD PTR _CurrentBank
	sub	dx,dx
	cmp	ax,WORD PTR [bp-28]	;bank
	jne	SHORT $L1564
	cmp	dx,WORD PTR [bp-26]
	je	SHORT $I1298
$L1564:
	push	1
	push	WORD PTR [bp-28]	;bank
	call	FAR PTR _VESA_SetPage
	add	sp,4
; Line 734
$I1298:
	movsx	eax,WORD PTR [bp-14]	;w
	add	eax,DWORD PTR [bp-24]	;p_off
	cmp	eax,DWORD PTR _BytesPerBank
	jae	$I1299
$L1565:
; Line 735
	push	WORD PTR [bp-14]	;w
	mov	ax,WORD PTR [bp-24]	;p_off
	add	ax,WORD PTR _video_buffer
	mov	dx,WORD PTR _video_buffer+2
	push	dx
	push	ax
	push	DWORD PTR [bp-4]	;buf
	jmp	$L1596
	nop	
$FB1297:
; Line 746
	push	DWORD PTR [bp-4]	;buf
	call	FAR PTR _free
; Line 747
	leave	
	ret	
	nop	

_SVGA_GetImage	ENDP
	PUBLIC	_SVGA_ConvertString
_SVGA_ConvertString	PROC FAR
; Line 750
	enter	1004,0
	push	si
;	i = -2
;	string = -1002
;	c = -1004
;	str = 6
; Line 755
	push	ds
	push	OFFSET DGROUP:$SG1307
	lea	ax,WORD PTR [bp-1002]	;string
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,8
; Line 757
	mov	WORD PTR [bp-2],0	;i
	jmp	$F1308
	nop	
$I1311:
	cmp	BYTE PTR [bp-1004],97	;0061H	;c
	jl	SHORT $I1313
	cmp	BYTE PTR [bp-1004],122	;007aH	;c
	jg	SHORT $I1313
; Line 764
	sub	BYTE PTR [bp-1004],70	;0046H	;c
; Line 765
	jmp	SHORT $I1325
	nop	
$I1313:
	cmp	BYTE PTR [bp-1004],32	;0020H	;c
	jne	SHORT $I1315
; Line 766
	mov	BYTE PTR [bp-1004],53	;0035H	;c
; Line 767
	jmp	SHORT $I1325
$I1315:
	cmp	BYTE PTR [bp-1004],44	;002cH	;c
	jne	SHORT $I1317
; Line 768
	mov	BYTE PTR [bp-1004],54	;0036H	;c
; Line 769
	jmp	SHORT $I1325
$I1317:
	cmp	BYTE PTR [bp-1004],46	;002eH	;c
	jne	SHORT $I1319
; Line 770
	mov	BYTE PTR [bp-1004],55	;0037H	;c
; Line 771
	jmp	SHORT $I1325
$I1319:
	cmp	BYTE PTR [bp-1004],63	;003fH	;c
	jne	SHORT $I1321
; Line 772
	mov	BYTE PTR [bp-1004],56	;0038H	;c
; Line 773
	jmp	SHORT $I1325
$I1321:
	cmp	BYTE PTR [bp-1004],33	;0021H	;c
	jne	SHORT $I1323
; Line 774
	mov	BYTE PTR [bp-1004],57	;0039H	;c
; Line 775
	jmp	SHORT $I1325
$I1323:
	cmp	BYTE PTR [bp-1004],48	;0030H	;c
	jl	SHORT $I1325
	cmp	BYTE PTR [bp-1004],57	;0039H	;c
	jg	SHORT $I1325
; Line 776
	add	BYTE PTR [bp-1004],10	;000aH	;c
; Line 778
$I1325:
	mov	al,BYTE PTR [bp-1004]	;c
	cbw	
	push	ax
	lea	ax,WORD PTR [bp-1002]	;string
	push	ss
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG1326
	push	ss
	push	ax
	call	FAR PTR _sprintf
	add	sp,14	;000eH
; Line 779
	inc	WORD PTR [bp-2]	;i
$F1308:
	push	DWORD PTR [bp+6]	;str
	call	FAR PTR _strlen
	add	sp,4
	cmp	ax,WORD PTR [bp-2]	;i
	jbe	SHORT $FB1310
; Line 759
	les	bx,DWORD PTR [bp+6]	;str
	mov	si,WORD PTR [bp-2]	;i
	mov	al,BYTE PTR es:[bx][si]
	mov	BYTE PTR [bp-1004],al	;c
; Line 761
	cmp	al,65	;0041H
	jl	$I1311
	cmp	al,90	;005aH
	jg	$I1311
; Line 762
	sub	BYTE PTR [bp-1004],64	;0040H	;c
; Line 763
	jmp	SHORT $I1325
	nop	
$FB1310:
; Line 780
	lea	ax,WORD PTR [bp-1002]	;string
	mov	dx,ss
; Line 781
	pop	si
	leave	
	ret	
	nop	

_SVGA_ConvertString	ENDP
	PUBLIC	_SVGA_KillImage
_SVGA_KillImage	PROC FAR
; Line 784
	push	bp
	mov	bp,sp
;	img = 6
; Line 785
	les	bx,DWORD PTR [bp+6]	;img
	cmp	WORD PTR es:[bx+774],0
	je	SHORT $I1330
	push	WORD PTR es:[bx]
	call	FAR PTR _XMS_Free
; Line 786
$I1330:
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+774],0
; Line 787
	leave	
	ret	

_SVGA_KillImage	ENDP
	PUBLIC	_SVGA_GetUserMode
_SVGA_GetUserMode	PROC FAR
; Line 790
	enter	2,0
;	i = -2
; Line 799
	push	ds
	push	OFFSET DGROUP:$SG1333
	call	FAR PTR _printf
	add	sp,4
; Line 800
	mov	WORD PTR [bp-2],0	;i
; Line 801
$FC1335:
	cmp	WORD PTR [bp-2],49	;0031H	;i
	jl	SHORT $FB1337
	cmp	WORD PTR [bp-2],55	;0037H	;i
	jle	SHORT $FB1336
$FB1337:
	cmp	WORD PTR [bp-2],13	;000dH	;i
	je	SHORT $FB1336
; Line 802
	call	FAR PTR _getch
	mov	WORD PTR [bp-2],ax	;i
	jmp	SHORT $FC1335
	nop	
$FB1336:
; Line 804
	push	WORD PTR [bp-2]	;i
	push	ds
	push	OFFSET DGROUP:$SG1338
	call	FAR PTR _printf
	add	sp,6
; Line 806
	mov	ax,WORD PTR [bp-2]	;i
	cmp	ax,51	;0033H
	je	SHORT $SC1350
	jg	SHORT $L1566
	sub	ax,13	;000dH
	je	SHORT $SC1350
	sub	ax,36	;0024H
	je	SHORT $SC1343
	dec	ax
	je	SHORT $SC1344
	jmp	SHORT $SB1340
; Line 808
$SC1343:
	mov	ax,19	;0013H
	leave	
	ret	
	nop	
; Line 809
$SC1344:
	mov	ax,256	;0100H
	leave	
	ret	
	nop	
; Line 811
$SC1346:
	mov	ax,259	;0103H
	leave	
	ret	
	nop	
; Line 812
$SC1347:
	mov	ax,261	;0105H
	leave	
	ret	
	nop	
; Line 813
$SC1348:
	mov	ax,263	;0107H
	jmp	SHORT $SB1340
	nop	
; Line 814
$SC1349:
	xor	ax,ax
	jmp	SHORT $SB1340
; Line 815
$SC1350:
	mov	ax,257	;0101H
	leave	
	ret	
	nop	
$L1566:
	sub	ax,52	;0034H
	je	SHORT $SC1346
	dec	ax
	je	SHORT $SC1347
	dec	ax
	je	SHORT $SC1348
	dec	ax
	je	SHORT $SC1349
$SB1340:
; Line 817
	leave	
	ret	

_SVGA_GetUserMode	ENDP
	PUBLIC	_SVGA_MergeImage
_SVGA_MergeImage	PROC FAR
; Line 820
	enter	66,0
	push	di
	push	si
;	db = -4
;	sb = -8
;	dbs = -10
;	dbo = -12
;	sbs = -14
;	sbo = -16
;	sx = -18
;	sy = -20
;	w = -22
;	h = -24
;	i = -26
;	x_off = -30
;	y_off = -34
;	d_off = -38
;	s_off = -42
;	d_x_off = -46
;	d_y_off = -50
;	xb = -66
;	rop = 1570
;	y = 1568
;	x = 1566
;	s = 790
;	th = 788
;	tw = 786
;	dy = 784
;	dx = 782
;	d = 6
; Line 825
	xor	eax,eax
	mov	DWORD PTR [bp-20],eax	;sy
	mov	DWORD PTR [bp-24],eax	;h
	mov	WORD PTR [bp-26],ax	;i
; Line 826
	sub	ecx,ecx
	mov	DWORD PTR [bp-30],ecx	;x_off
	mov	DWORD PTR [bp-34],ecx	;y_off
	mov	DWORD PTR [bp-38],ecx	;d_off
	mov	DWORD PTR [bp-42],ecx	;s_off
; Line 827
	mov	DWORD PTR [bp-46],ecx	;d_x_off
	mov	DWORD PTR [bp-50],ecx	;d_y_off
; Line 830
	cmp	WORD PTR [bp+1566],ax	;x
	jge	SHORT $I1388
; Line 833
	push	WORD PTR [bp+1566]	;x
	call	FAR PTR _abs
	add	sp,2
	cwd	
	mov	WORD PTR [bp-46],ax	;d_x_off
	mov	WORD PTR [bp-44],dx
; Line 835
	jmp	SHORT $I1389
$I1388:
	mov	ax,WORD PTR [bp+792]
	cmp	WORD PTR [bp+1566],ax	;x
	jge	$FB1422
; Line 838
	movsx	eax,WORD PTR [bp+1566]	;x
	mov	DWORD PTR [bp-30],eax	;x_off
$I1389:
; Line 840
	mov	ax,WORD PTR [bp+786]	;tw
	cwd	
	add	ax,WORD PTR [bp-30]	;x_off
	adc	dx,WORD PTR [bp-28]
	mov	cx,ax
	mov	ax,WORD PTR [bp+792]
	mov	bx,dx
	cwd	
	cmp	bx,dx
	jl	SHORT $I1392
	jg	SHORT $L1569
	cmp	cx,ax
	jbe	SHORT $I1392
$L1569:
; Line 841
	sub	ax,WORD PTR [bp-30]	;x_off
	jmp	SHORT $L1597
	nop	
$I1392:
; Line 843
	mov	ax,WORD PTR [bp+786]	;tw
$L1597:
	mov	WORD PTR [bp-22],ax	;w
; Line 845
	cmp	WORD PTR [bp+1568],0	;y
	jge	SHORT $I1394
; Line 847
	mov	DWORD PTR [bp-34],0	;y_off
; Line 848
	push	WORD PTR [bp+1568]	;y
	call	FAR PTR _abs
	add	sp,2
	cwd	
	mov	WORD PTR [bp-50],ax	;d_y_off
	mov	WORD PTR [bp-48],dx
; Line 850
	jmp	SHORT $I1395
	nop	
$I1394:
	mov	ax,WORD PTR [bp+794]
	cmp	WORD PTR [bp+1568],ax	;y
	jge	$FB1422
; Line 853
	movsx	eax,WORD PTR [bp+1568]	;y
	mov	DWORD PTR [bp-34],eax	;y_off
$I1395:
; Line 855
	mov	ax,WORD PTR [bp+788]	;th
	cwd	
	add	ax,WORD PTR [bp-34]	;y_off
	adc	dx,WORD PTR [bp-32]
	mov	cx,ax
	mov	ax,WORD PTR [bp+794]
	mov	bx,dx
	cwd	
	cmp	bx,dx
	jl	SHORT $I1398
	jg	SHORT $L1572
	cmp	cx,ax
	jbe	SHORT $I1398
$L1572:
; Line 856
	sub	ax,WORD PTR [bp-34]	;y_off
	jmp	SHORT $L1598
	nop	
$I1398:
; Line 858
	mov	ax,WORD PTR [bp+788]	;th
$L1598:
	mov	WORD PTR [bp-24],ax	;h
; Line 860
	push	DWORD PTR [bp-34]	;y_off
	movsx	eax,WORD PTR [bp+792]
	pop	ecx
	imul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	add	ax,WORD PTR [bp-30]	;x_off
	adc	dx,WORD PTR [bp-28]
	mov	WORD PTR [bp-42],ax	;s_off
	mov	WORD PTR [bp-40],dx
; Line 862
	cmp	WORD PTR [bp+782],0	;dx
	jge	SHORT $I1400
; Line 864
	mov	DWORD PTR [bp-46],0	;d_x_off
; Line 865
	push	WORD PTR [bp+782]	;dx
	call	FAR PTR _abs
	add	sp,2
	cwd	
	add	WORD PTR [bp-30],ax	;x_off
	adc	WORD PTR [bp-28],dx
; Line 867
	jmp	SHORT $I1401
	nop	
$I1400:
	mov	ax,WORD PTR [bp+8]
	cmp	WORD PTR [bp+782],ax	;dx
	jge	$FB1422
; Line 870
	movsx	eax,WORD PTR [bp+782]	;dx
	add	DWORD PTR [bp-46],eax	;d_x_off
$I1401:
; Line 872
	mov	ax,WORD PTR [bp-22]	;w
	cwd	
	add	ax,WORD PTR [bp-46]	;d_x_off
	adc	dx,WORD PTR [bp-44]
	mov	cx,ax
	mov	ax,WORD PTR [bp+8]
	mov	bx,dx
	cwd	
	cmp	bx,dx
	jl	SHORT $I1404
	jg	SHORT $L1575
	cmp	cx,ax
	jbe	SHORT $I1404
$L1575:
; Line 873
	sub	ax,WORD PTR [bp-46]	;d_x_off
	mov	WORD PTR [bp-22],ax	;w
; Line 875
$I1404:
	cmp	WORD PTR [bp+784],0	;dy
	jge	SHORT $I1405
; Line 876
	push	WORD PTR [bp+1568]	;y
	call	FAR PTR _abs
	add	sp,2
	cwd	
	add	WORD PTR [bp-34],ax	;y_off
	adc	WORD PTR [bp-32],dx
; Line 877
	jmp	SHORT $I1406
	nop	
$I1405:
	mov	ax,WORD PTR [bp+10]
	cmp	WORD PTR [bp+784],ax	;dy
	jge	$FB1422
; Line 880
	movsx	eax,WORD PTR [bp+784]	;dy
	add	DWORD PTR [bp-50],eax	;d_y_off
$I1406:
; Line 882
	mov	ax,WORD PTR [bp-24]	;h
	cwd	
	add	ax,WORD PTR [bp-50]	;d_y_off
	adc	dx,WORD PTR [bp-48]
	mov	cx,ax
	mov	ax,WORD PTR [bp+10]
	mov	bx,dx
	cwd	
	cmp	bx,dx
	jl	SHORT $I1409
	jg	SHORT $L1578
	cmp	cx,ax
	jbe	SHORT $I1409
$L1578:
; Line 883
	sub	ax,WORD PTR [bp-50]	;d_y_off
	mov	WORD PTR [bp-24],ax	;h
; Line 885
$I1409:
	push	DWORD PTR [bp-50]	;d_y_off
	movsx	eax,WORD PTR [bp+8]
	pop	ecx
	imul	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	add	ax,WORD PTR [bp-46]	;d_x_off
	adc	dx,WORD PTR [bp-44]
	mov	WORD PTR [bp-38],ax	;d_off
	mov	WORD PTR [bp-36],dx
; Line 887
	cmp	WORD PTR [bp-22],1	;w
	jl	$FB1422
	cmp	WORD PTR [bp-24],1	;h
	jl	$FB1422
; Line 889
	cmp	WORD PTR [bp+1570],0	;rop
	je	$I1412
; Line 891
	push	WORD PTR [bp-22]	;w
	call	FAR PTR _malloc
	add	sp,2
	mov	WORD PTR [bp-4],ax	;db
	mov	WORD PTR [bp-2],dx
; Line 892
	mov	WORD PTR [bp-10],dx	;dbs
; Line 893
	mov	WORD PTR [bp-12],ax	;dbo
; Line 894
	push	WORD PTR [bp-22]	;w
	call	FAR PTR _malloc
	add	sp,2
	mov	WORD PTR [bp-8],ax	;sb
	mov	WORD PTR [bp-6],dx
; Line 895
	mov	WORD PTR [bp-14],dx	;sbs
; Line 896
	mov	WORD PTR [bp-16],ax	;sbo
; Line 898
	mov	ax,WORD PTR [bp-20]	;sy
	mov	WORD PTR [bp-26],ax	;i
	jmp	SHORT $F1413
	nop	
$FC1414:
	inc	WORD PTR [bp-26]	;i
$F1413:
	mov	ax,WORD PTR [bp-24]	;h
	add	ax,WORD PTR [bp-20]	;sy
	cmp	ax,WORD PTR [bp-26]	;i
	jle	$FB1415
; Line 900
	push	DWORD PTR [bp-42]	;s_off
	movsx	eax,WORD PTR [bp-22]	;w
	push	eax
	push	WORD PTR [bp+790]	;s
	push	DWORD PTR [bp-8]	;sb
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 901
	push	DWORD PTR [bp-38]	;d_off
	movsx	eax,WORD PTR [bp-22]	;w
	push	eax
	push	WORD PTR [bp+6]	;d
	push	DWORD PTR [bp-4]	;db
	call	FAR PTR _XMS_CopyFromXMS
	add	sp,14	;000eH
; Line 904
	push	ds
; Line 905
	push	si
; Line 907
	mov	ax,WORD PTR [bp-14]	;sbs
; Line 908
	mov	bx,WORD PTR [bp-10]	;dbs
; Line 909
	mov	ds,ax
; Line 910
	mov	es,bx
; Line 911
	mov	di,WORD PTR [bp-12]	;dbo
; Line 912
	mov	si,WORD PTR [bp-16]	;sbo
; Line 913
	mov	cx,WORD PTR [bp-22]	;w
; Line 915
$cLoop1416:
; Line 916
	lodsb
; Line 917
	cmp	al,0
; Line 918
	jne	SHORT $cDoPixel1417
; Line 920
	inc	di
; Line 921
	jmp	$cFinish1418
	nop	
; Line 922
$cDoPixel1417:
; Line 923
	stosb
; Line 924
$cFinish1418:
; Line 925
	loop	$cLoop1416
; Line 927
	pop	si
; Line 928
	pop	ds
; Line 929
; Line 930
	push	DWORD PTR [bp-38]	;d_off
	movsx	eax,WORD PTR [bp-22]	;w
	push	eax
	push	WORD PTR [bp+6]	;d
	push	DWORD PTR [bp-4]	;db
	call	FAR PTR _XMS_CopyToXMS
	add	sp,14	;000eH
; Line 931
	movsx	eax,WORD PTR [bp+8]
	add	DWORD PTR [bp-38],eax	;d_off
; Line 932
	mov	ax,WORD PTR [bp+792]
	cwd	
	add	WORD PTR [bp-42],ax	;s_off
	adc	WORD PTR [bp-40],dx
; Line 933
	jmp	$FC1414
$FB1415:
; Line 934
	push	DWORD PTR [bp-4]	;db
	call	FAR PTR _free
	add	sp,4
; Line 935
	push	DWORD PTR [bp-8]	;sb
	call	FAR PTR _free
	add	sp,4
; Line 937
	pop	si
	pop	di
	leave	
	ret	
$I1412:
; Line 939
	movsx	eax,WORD PTR [bp-22]	;w
	mov	DWORD PTR [bp-66],eax	;xb
; Line 940
	mov	ax,WORD PTR [bp+790]	;s
	mov	WORD PTR [bp-62],ax
; Line 941
	mov	eax,DWORD PTR [bp-42]	;s_off
	mov	DWORD PTR [bp-60],eax
; Line 942
	mov	ax,WORD PTR [bp+6]	;d
	mov	WORD PTR [bp-56],ax
; Line 943
	mov	eax,DWORD PTR [bp-38]	;d_off
	mov	dx,WORD PTR [bp-36]
	mov	DWORD PTR [bp-54],eax
; Line 944
	mov	WORD PTR [bp-26],0	;i
	jmp	SHORT $F1420
$I1423:
; Line 958
	lea	ax,WORD PTR [bp-66]	;xb
	push	ss
	push	ax
	call	FAR PTR _XMS_Copy
	add	sp,4
$I1424:
; Line 960
	movsx	eax,WORD PTR [bp+8]
	add	DWORD PTR [bp-54],eax
; Line 961
	mov	ax,WORD PTR [bp+792]
	cwd	
	add	WORD PTR [bp-60],ax
	adc	WORD PTR [bp-58],dx
; Line 962
	inc	WORD PTR [bp-26]	;i
$F1420:
	mov	ax,WORD PTR [bp-24]	;h
	cmp	WORD PTR [bp-26],ax	;i
	jge	SHORT $FB1422
; Line 946
	test	BYTE PTR [bp-22],1	;w
	je	SHORT $I1423
; Line 948
	mov	DWORD PTR [bp-66],2	;xb
; Line 949
	lea	ax,WORD PTR [bp-66]	;xb
	push	ss
	push	ax
	call	FAR PTR _XMS_Copy
	add	sp,4
; Line 950
	mov	ax,WORD PTR [bp-22]	;w
	dec	ax
	cwd	
	mov	WORD PTR [bp-66],ax	;xb
	mov	WORD PTR [bp-64],dx
; Line 951
	inc	DWORD PTR [bp-60]
; Line 952
	inc	DWORD PTR [bp-54]
; Line 953
	lea	ax,WORD PTR [bp-66]	;xb
	push	ss
	push	ax
	call	FAR PTR _XMS_Copy
	add	sp,4
; Line 954
	dec	DWORD PTR [bp-60]
; Line 955
	dec	DWORD PTR [bp-54]
; Line 957
	jmp	SHORT $I1424
	nop	
$FB1422:
; Line 964
	pop	si
	pop	di
	leave	
	ret	

_SVGA_MergeImage	ENDP
	PUBLIC	_SVGA_InitImage
_SVGA_InitImage	PROC FAR
; Line 967
	push	bp
	mov	bp,sp
;	h = 12
;	w = 10
;	img = 6
; Line 968
	les	bx,DWORD PTR [bp+6]	;img
	cmp	WORD PTR es:[bx+774],0
	je	SHORT $I1432
	push	es
	push	bx
	push	cs
	call	NEAR PTR _SVGA_KillImage
	mov	sp,bp
; Line 969
$I1432:
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+774],1
; Line 970
	mov	ax,WORD PTR [bp+10]	;w
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+2],ax
; Line 971
	mov	ax,WORD PTR [bp+12]	;h
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx+4],ax
; Line 972
	push	DWORD PTR 1024	;00000400H
	mov	ax,WORD PTR [bp+12]	;h
	imul	WORD PTR [bp+10]	;w
	push	dx
	push	ax
	pop	eax
	pop	ecx
	cdq	
	idiv	ecx
	mov	edx,eax
	shr	edx,16	;00000010H
	inc	ax
	push	ax
	call	FAR PTR _XMS_Alloc
	les	bx,DWORD PTR [bp+6]	;img
	mov	WORD PTR es:[bx],ax
; Line 973
	leave	
	ret	
	nop	

_SVGA_InitImage	ENDP
	PUBLIC	_SVGA_SetPalette
_SVGA_SetPalette	PROC FAR
; Line 977
	push	bp
	mov	bp,sp
;	color = 8
;	index = 6
; Line 978
	push	DWORD PTR 16712646	;00ff03c6H
	call	FAR PTR __outp
	mov	sp,bp
; Line 979
	push	WORD PTR [bp+6]	;index
	push	968	;03c8H
	call	FAR PTR __outp
	mov	sp,bp
; Line 980
	les	bx,DWORD PTR [bp+8]	;color
	sub	ah,ah
	mov	al,BYTE PTR es:[bx]
	push	ax
	push	969	;03c9H
	call	FAR PTR __outp
	mov	sp,bp
; Line 981
	les	bx,DWORD PTR [bp+8]	;color
	sub	ah,ah
	mov	al,BYTE PTR es:[bx+1]
	push	ax
	push	969	;03c9H
	call	FAR PTR __outp
	mov	sp,bp
; Line 982
	les	bx,DWORD PTR [bp+8]	;color
	sub	ah,ah
	mov	al,BYTE PTR es:[bx+2]
	push	ax
	push	969	;03c9H
	call	FAR PTR __outp
; Line 983
	leave	
	ret	

_SVGA_SetPalette	ENDP
	PUBLIC	_SVGA_SetAllPalette
_SVGA_SetAllPalette	PROC FAR
; Line 986
	enter	2,0
	push	si
;	i = -2
;	pal = 6
; Line 989
	push	DWORD PTR 16712646	;00ff03c6H
	call	FAR PTR _outp
	add	sp,4
; Line 990
	mov	WORD PTR [bp-2],0	;i
$FC1443:
; Line 992
	push	WORD PTR [bp-2]	;i
	push	968	;03c8H
	call	FAR PTR _outp
	add	sp,4
; Line 993
	mov	bx,WORD PTR [bp-2]	;i
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+6]	;pal
	mov	al,BYTE PTR es:[bx][si]
	sub	ah,ah
	push	ax
	push	969	;03c9H
	call	FAR PTR _outp
	add	sp,4
; Line 994
	mov	si,WORD PTR [bp-2]	;i
	mov	ax,si
	add	si,si
	add	si,ax
	les	bx,DWORD PTR [bp+6]	;pal
	sub	ah,ah
	mov	al,BYTE PTR es:[bx+1][si]
	push	ax
	push	969	;03c9H
	call	FAR PTR _outp
	add	sp,4
; Line 995
	mov	si,WORD PTR [bp-2]	;i
	mov	ax,si
	add	si,si
	add	si,ax
	les	bx,DWORD PTR [bp+6]	;pal
	sub	ah,ah
	mov	al,BYTE PTR es:[bx+2][si]
	push	ax
	push	969	;03c9H
	call	FAR PTR _outp
	add	sp,4
; Line 996
	inc	WORD PTR [bp-2]	;i
	cmp	WORD PTR [bp-2],256	;0100H	;i
	jl	SHORT $FC1443
; Line 997
	pop	si
	leave	
	ret	

_SVGA_SetAllPalette	ENDP
	PUBLIC	_SVGA_GetPalette
_SVGA_GetPalette	PROC FAR
; Line 1000
	push	bp
	mov	bp,sp
;	color = 8
;	index = 6
; Line 1001
	push	DWORD PTR 16712646	;00ff03c6H
	call	FAR PTR __outp
	mov	sp,bp
; Line 1002
	push	WORD PTR [bp+6]	;index
	push	967	;03c7H
	call	FAR PTR __outp
	mov	sp,bp
; Line 1003
	push	969	;03c9H
	call	FAR PTR __inp
	mov	sp,bp
	les	bx,DWORD PTR [bp+8]	;color
	mov	BYTE PTR es:[bx],al
; Line 1004
	push	969	;03c9H
	call	FAR PTR __inp
	mov	sp,bp
	les	bx,DWORD PTR [bp+8]	;color
	mov	BYTE PTR es:[bx+1],al
; Line 1005
	push	969	;03c9H
	call	FAR PTR __inp
	les	bx,DWORD PTR [bp+8]	;color
	mov	BYTE PTR es:[bx+2],al
; Line 1006
	leave	
	ret	
	nop	

_SVGA_GetPalette	ENDP
	PUBLIC	_SVGA_FadeIn
_SVGA_FadeIn	PROC FAR
; Line 1009
	enter	514,0
	push	si
;	RGB = -4
;	str = -504
;	pc = -506
;	index = -508
;	clock = -512
;	now = -514
;	p = 8
;	d = 6
; Line 1013
	mov	WORD PTR [bp-512],1132	;046cH	;clock
	xor	al,al
	mov	WORD PTR [bp-510],0
; Line 1016
	mov	BYTE PTR [bp-2],al
	mov	BYTE PTR [bp-3],al
	mov	BYTE PTR [bp-4],al	;RGB
; Line 1018
	mov	WORD PTR [bp-508],0	;index
$FC1462:
; Line 1019
	lea	ax,WORD PTR [bp-4]	;RGB
	push	ss
	push	ax
	push	WORD PTR [bp-508]	;index
	push	cs
	call	NEAR PTR _SVGA_SetPalette
	add	sp,6
	inc	WORD PTR [bp-508]	;index
	cmp	WORD PTR [bp-508],256	;0100H	;index
	jl	SHORT $FC1462
; Line 1021
	mov	WORD PTR [bp-508],0	;index
	jmp	SHORT $F1464
; Line 1034
$FC1465:
	inc	WORD PTR [bp-508]	;index
$F1464:
	cmp	WORD PTR [bp-508],64	;0040H	;index
	jge	$FB1466
; Line 1023
	les	bx,DWORD PTR [bp-512]	;clock
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR [bp-514],ax	;now
; Line 1024
	mov	WORD PTR [bp-506],0	;pc
$FC1468:
; Line 1026
	lea	ax,WORD PTR [bp-4]	;RGB
	push	ss
	push	ax
	push	WORD PTR [bp-506]	;pc
	push	cs
	call	NEAR PTR _SVGA_GetPalette
	add	sp,6
; Line 1027
	mov	bx,WORD PTR [bp-506]	;pc
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+8]	;p
	mov	al,BYTE PTR [bp-4]	;RGB
	cmp	BYTE PTR es:[bx][si],al
	jbe	SHORT $I1470
	inc	BYTE PTR [bp-4]	;RGB
; Line 1028
$I1470:
	mov	al,BYTE PTR [bp-3]
	mov	si,WORD PTR [bp-506]	;pc
	mov	cx,si
	add	si,si
	add	si,cx
	mov	bx,WORD PTR [bp+8]	;p
	cmp	BYTE PTR es:[bx+1][si],al
	jbe	SHORT $I1471
	inc	BYTE PTR [bp-3]
; Line 1029
$I1471:
	mov	al,BYTE PTR [bp-2]
	mov	si,cx
	add	si,si
	add	si,cx
	cmp	BYTE PTR es:[bx+2][si],al
	jbe	SHORT $I1472
	inc	BYTE PTR [bp-2]
; Line 1030
$I1472:
	lea	ax,WORD PTR [bp-4]	;RGB
	push	ss
	push	ax
	push	WORD PTR [bp-506]	;pc
	push	cs
	call	NEAR PTR _SVGA_SetPalette
	add	sp,6
; Line 1031
	inc	WORD PTR [bp-506]	;pc
	cmp	WORD PTR [bp-506],256	;0100H	;pc
	jl	SHORT $FC1468
; Line 1032
	les	bx,DWORD PTR [bp-512]	;clock
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR [bp-514],ax	;now
; Line 1033
$FC1474:
	les	bx,DWORD PTR [bp-512]	;clock
	mov	ax,WORD PTR es:[bx]
	sub	ax,WORD PTR [bp-514]	;now
	push	ax
	call	FAR PTR _abs
	add	sp,2
	cmp	ax,WORD PTR [bp+6]	;d
	jge	$FC1465
	jmp	SHORT $FC1474
	nop	
$FB1466:
; Line 1035
	pop	si
	leave	
	ret	
	nop	

_SVGA_FadeIn	ENDP
	PUBLIC	_SVGA_FadeOut
_SVGA_FadeOut	PROC FAR
; Line 1038
	enter	14,0
;	c = -4
;	i = -6
;	j = -8
;	clock = -12
;	now = -14
;	d = 6
; Line 1041
	mov	DWORD PTR [bp-12],1132	;0000046cH	;clock
; Line 1045
	mov	WORD PTR [bp-8],0	;j
	jmp	SHORT $F1484
	nop	
$FC1488:
	inc	WORD PTR [bp-6]	;i
$F1487:
	cmp	WORD PTR [bp-6],256	;0100H	;i
	jl	SHORT $FC1488
; Line 1050
	lea	ax,WORD PTR [bp-4]	;c
	push	ss
	push	ax
	push	WORD PTR [bp-6]	;i
	push	cs
	call	NEAR PTR _SVGA_GetPalette
	add	sp,6
; Line 1051
	cmp	BYTE PTR [bp-4],1	;c
	jb	SHORT $I1490
	dec	BYTE PTR [bp-4]	;c
; Line 1052
$I1490:
	cmp	BYTE PTR [bp-3],1
	jb	SHORT $I1491
	dec	BYTE PTR [bp-3]
; Line 1053
$I1491:
	cmp	BYTE PTR [bp-2],1
	jb	SHORT $I1492
	dec	BYTE PTR [bp-2]
; Line 1054
$I1492:
	lea	ax,WORD PTR [bp-4]	;c
	push	ss
	push	ax
	push	WORD PTR [bp-6]	;i
	push	cs
	call	NEAR PTR _SVGA_SetPalette
	add	sp,6
; Line 1056
$FC1494:
	les	bx,DWORD PTR [bp-12]	;clock
	mov	ax,WORD PTR es:[bx]
	sub	ax,WORD PTR [bp-14]	;now
	push	ax
	call	FAR PTR _abs
	add	sp,2
	cmp	ax,WORD PTR [bp+6]	;d
	jl	SHORT $FC1494
; Line 1057
	inc	WORD PTR [bp-8]	;j
$F1484:
	cmp	WORD PTR [bp-8],64	;0040H	;j
	jge	SHORT $FB1486
; Line 1047
	les	bx,DWORD PTR [bp-12]	;clock
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR [bp-14],ax	;now
; Line 1048
	mov	WORD PTR [bp-6],0	;i
	jmp	SHORT $F1487
	nop	
$FB1486:
; Line 1058
	leave	
	ret	

_SVGA_FadeOut	ENDP
	PUBLIC	_SVGA_CyclePalette
_SVGA_CyclePalette	PROC FAR
; Line 1061
	enter	506,0
	push	di
	push	si
;	c = -4
;	i = -6
;	str = -506
;	up = 14
;	f = 12
;	s = 10
;	p = 6
; Line 1066
	cmp	WORD PTR [bp+14],0	;up
	je	SHORT $I1508
; Line 1068
	mov	ax,WORD PTR [bp+10]	;s
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	add	ax,WORD PTR [bp+6]	;p
	mov	dx,WORD PTR [bp+8]
	push	ds
	lea	di,WORD PTR [bp-4]	;c
	mov	si,ax
	push	ss
	pop	es
	mov	ds,dx
	ASSUME DS: NOTHING
	movsw
	movsb
	pop	ds
	ASSUME DS: DGROUP
; Line 1070
	mov	BYTE PTR [bp-4],25	;0019H	;c
; Line 1072
	mov	WORD PTR [bp-6],cx	;i
	jmp	SHORT $F1509
	nop	
$FC1510:
; Line 1073
	mov	bx,WORD PTR [bp-6]	;i
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+6]	;p
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	add	ax,si
	mov	dx,es
	add	ax,3
	push	ds
	lea	di,WORD PTR [bx][si]
	mov	si,ax
	mov	ds,dx
	ASSUME DS: NOTHING
	movsw
	movsb
	pop	ds
	ASSUME DS: DGROUP
	inc	WORD PTR [bp-6]	;i
$F1509:
	mov	ax,WORD PTR [bp+12]	;f
	cmp	WORD PTR [bp-6],ax	;i
	jl	SHORT $FC1510
; Line 1075
	mov	bx,ax
	jmp	SHORT $L1603
$I1508:
; Line 1079
	mov	ax,WORD PTR [bp+12]	;f
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	add	ax,WORD PTR [bp+6]	;p
	mov	dx,WORD PTR [bp+8]
	push	ds
	lea	di,WORD PTR [bp-4]	;c
	mov	si,ax
	push	ss
	pop	es
	mov	ds,dx
	ASSUME DS: NOTHING
	movsw
	movsb
	pop	ds
	ASSUME DS: DGROUP
; Line 1081
	mov	WORD PTR [bp-6],cx	;i
	jmp	SHORT $F1513
	nop	
$FC1514:
; Line 1082
	mov	bx,WORD PTR [bp-6]	;i
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+6]	;p
	mov	cx,ax
	add	ax,ax
	add	ax,cx
	add	ax,si
	mov	dx,es
	sub	ax,3
	push	ds
	lea	di,WORD PTR [bx][si]
	mov	si,ax
	mov	ds,dx
	ASSUME DS: NOTHING
	movsw
	movsb
	pop	ds
	ASSUME DS: DGROUP
	dec	WORD PTR [bp-6]	;i
$F1513:
	mov	ax,WORD PTR [bp+10]	;s
	inc	ax
	cmp	ax,WORD PTR [bp-6]	;i
	jl	SHORT $FC1514
; Line 1084
	mov	bx,WORD PTR [bp+10]	;s
$L1603:
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+6]	;p
	lea	di,WORD PTR [bx][si]
	lea	si,WORD PTR [bp-4]	;c
	movsw
	movsb
; Line 1086
	push	es
	push	WORD PTR [bp+6]	;p
	push	cs
	call	NEAR PTR _SVGA_SetAllPalette
	add	sp,4
; Line 1087
	pop	si
	pop	di
	leave	
	ret	
	nop	

_SVGA_CyclePalette	ENDP
	PUBLIC	_SVGA_LoadPalette
_SVGA_LoadPalette	PROC FAR
; Line 1090
	enter	10,0
	push	si
;	index = -2
;	color = -6
;	fp = -10
;	the_palette = 10
;	filename = 6
; Line 1095
	push	ds
	push	OFFSET DGROUP:$SG1525
	push	DWORD PTR [bp+6]	;filename
	call	FAR PTR _fopen
	add	sp,8
	mov	WORD PTR [bp-10],ax	;fp
	mov	WORD PTR [bp-8],dx
	or	dx,ax
	jne	SHORT $I1524
; Line 1096
	xor	ax,ax
	pop	si
	leave	
	ret	
; Line 1098
$I1524:
	mov	WORD PTR [bp-2],0	;index
$FC1527:
; Line 1102
	lea	ax,WORD PTR [bp-4]
	push	ss
	push	ax
	lea	cx,WORD PTR [bp-5]
	push	ss
	push	cx
	lea	dx,WORD PTR [bp-6]	;color
	push	ss
	push	dx
	push	ds
	push	OFFSET DGROUP:$SG1529
	push	DWORD PTR [bp-10]	;fp
	call	FAR PTR _fscanf
	add	sp,20	;0014H
; Line 1104
	mov	bx,WORD PTR [bp-2]	;index
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+10]	;the_palette
	mov	al,BYTE PTR [bp-6]	;color
	mov	BYTE PTR es:[bx][si],al
; Line 1105
	mov	al,BYTE PTR [bp-5]
	mov	si,WORD PTR [bp-2]	;index
	mov	cx,si
	add	si,si
	add	si,cx
	les	bx,DWORD PTR [bp+10]	;the_palette
	mov	BYTE PTR es:[bx+1][si],al
; Line 1106
	mov	al,BYTE PTR [bp-4]
	mov	si,WORD PTR [bp-2]	;index
	mov	cx,si
	add	si,si
	add	si,cx
	les	bx,DWORD PTR [bp+10]	;the_palette
	mov	BYTE PTR es:[bx+2][si],al
; Line 1107
	inc	WORD PTR [bp-2]	;index
	cmp	WORD PTR [bp-2],255	;00ffH	;index
	jle	SHORT $FC1527
; Line 1109
	push	DWORD PTR [bp-10]	;fp
	call	FAR PTR _fclose
	add	sp,4
; Line 1110
	mov	ax,1
; Line 1111
	pop	si
	leave	
	ret	

_SVGA_LoadPalette	ENDP
	PUBLIC	_SVGA_SavePalette
_SVGA_SavePalette	PROC FAR
; Line 1114
	enter	10,0
	push	si
;	color = -6
;	fp = -10
;	index = -2
;	the_palette = 10
;	filename = 6
; Line 1119
	push	ds
	push	OFFSET DGROUP:$SG1539
	push	DWORD PTR [bp+6]	;filename
	call	FAR PTR _fopen
	add	sp,8
	mov	WORD PTR [bp-10],ax	;fp
	mov	WORD PTR [bp-8],dx
	or	dx,ax
	jne	SHORT $I1538
; Line 1120
	xor	ax,ax
	pop	si
	leave	
	ret	
; Line 1122
$I1538:
	mov	WORD PTR [bp-2],0	;index
$FC1541:
; Line 1124
	mov	bx,WORD PTR [bp-2]	;index
	mov	ax,bx
	add	bx,bx
	add	bx,ax
	les	si,DWORD PTR [bp+10]	;the_palette
	mov	al,BYTE PTR es:[bx][si]
	mov	BYTE PTR [bp-6],al	;color
; Line 1125
	mov	si,WORD PTR [bp-2]	;index
	mov	ax,si
	add	si,si
	mov	bx,WORD PTR [bp+10]	;the_palette
	add	si,ax
	mov	al,BYTE PTR es:[bx+1][si]
	mov	BYTE PTR [bp-5],al
; Line 1126
	mov	si,WORD PTR [bp-2]	;index
	mov	ax,si
	add	si,si
	add	si,ax
	mov	al,BYTE PTR es:[bx+2][si]
; Line 1128
	sub	ah,ah
	push	ax
	mov	al,BYTE PTR [bp-5]
	push	ax
	mov	al,BYTE PTR [bp-6]	;color
	push	ax
	push	ds
	push	OFFSET DGROUP:$SG1543
	push	DWORD PTR [bp-10]	;fp
	call	FAR PTR _fprintf
	add	sp,14	;000eH
; Line 1129
	inc	WORD PTR [bp-2]	;index
	cmp	WORD PTR [bp-2],255	;00ffH	;index
	jle	SHORT $FC1541
; Line 1131
	push	DWORD PTR [bp-10]	;fp
	call	FAR PTR _fclose
	add	sp,4
; Line 1132
	mov	ax,1
; Line 1133
	pop	si
	leave	
	ret	

_SVGA_SavePalette	ENDP
SVGA_TEXT	ENDS
END
