;------------------------------------------------------------------------------
; MEKA - videoasm.asm
; Video - Assembly Routines
;------------------------------------------------------------------------------

BITS    32

; DATA SECTION ----------------------------------------------------------------
SECTION .data
; USE32

%IFDEF ASM_SYMBOLS_REQUIRE_UNDERSCORE

%DEFINE  _Decode_Tile_ASM            Decode_Tile_ASM
%DEFINE  _Decode_Tile_ASM_Init       Decode_Tile_ASM_Init
%DEFINE  _Sprite_Collide_Line_ASM    Sprite_Collide_Line_ASM

%DEFINE  _VRAM                       VRAM
%DEFINE  _Sprites_Collision_Table    Sprites_Collision_Table
%DEFINE  _Do_Collision               Do_Collision
%DEFINE  _sms                        sms

%ENDIF ; ASM_SYMBOLS_REQUIRE_UNDERSCORE

EXTERN  _VRAM
EXTERN  _Sprites_Collision_Table
EXTERN  _Do_Collision
EXTERN  _sms

GLOBAL  _Decode_Tile_ASM
GLOBAL  _Decode_Tile_ASM_Init
GLOBAL  _Sprite_Collide_Line_ASM

		
_bitmask:
	dd	0x00000008,0x00000800,0x00080000,0x08000000
	dd	0x00000080,0x00008000,0x00800000,0x80000000

; BBS SECTION -----------------------------------------------------------------
SECTION .bss
; USE32

_planar_to_chunk:
        resb    1024

; CODE SECTION ----------------------------------------------------------------
SECTION .text
; USE32

;------------------------------------------------------------------------------
; Function: Decode_Tile_ASM_Init
; Parameters: none
;------------------------------------------------------------------------------
_Decode_Tile_ASM_Init:

        pushad
        mov     edi, _planar_to_chunk           ; edi = planar_to_chunk
        mov     ebx, _bitmask                   ; ebx = bitmap
        xor     ecx, ecx                        ; ecx = 0

.L1
        mov     eax, ecx                        ; eax = ecx
        xor     edx, edx                        ; edx = 0
        mov     ebp, 7                          ; ebp = 7
.L2
        shr     eax, 1                          ; eax >>= 1
        jnc     .L3                             ; if (NC) goto .L3
        or      edx, [ebx + ebp * 4]            ; edx |= ebx[ebp]
.L3
        dec     ebp                             ; ebp--
        jns     .L2                             ; if (NS) goto .L2

        mov     [edi], edx                      ; *edi = edx
        add     edi, 4                          ; edi += 4
        inc     ecx                             ; ecx++

        cmp     ecx, 256                        ; if (ecx != 256)
        jb      .L1                             ;    goto .L1

        popad
        ret

;------------------------------------------------------------------------------
; GCC version
; (does not compile under Windows)
;------------------------------------------------------------------------------
;	.text
;	.align	2
;_Decode_Tile_ASM_Init:
;	pushal
;        leal    _planar_to_chunk, %edi
;	leal	_bitmask, %ebx
;	xorl	%ecx, %ecx
;
;1:	movl	%ecx, %eax
;	xorl	%edx, %edx
;	movl	$7, %ebp
;2:	shrl	$1, %eax
;	jnc	3f
;	orl	(%ebx,%ebp,4), %edx
;3:	decl	%ebp
;	jns	2b
;	movl	%edx, (%edi)
;	addl	$4, %edi
;	incl	%ecx
;	cmpl	$256, %ecx
;	jb	1b
;
;	popal
;	ret
;
;	.data
;	.align	2
;_bitmask:
;	.long	0x00000008,0x00000800,0x00080000,0x08000000
;	.long	0x00000080,0x00008000,0x00800000,0x80000000
;
;        .comm   _planar_to_chunk, 1024
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Function: Decode_Tile_ASM
; Parameters: tile_n, gfx.Tile_Decoded[tile_n]
;------------------------------------------------------------------------------
_Decode_Tile_ASM:

        push    ebx
        push    ecx
        push    esi
        push    edi

;       push    edx
;       mov     edx, 0x42420002

        mov     ebx, _planar_to_chunk
        mov     esi, _VRAM
        ; xor   edx, edx
        ; mov   dx, word [esp + 20]     ; edx = tile_n
        mov     edx, dword [esp + 20]   ; edx = tile_n
        mov     edi, dword [esp + 24]   ; edi = gfx.Tile_Decoded[tile_n]

        shl     edx, 5                  ; edx >>= 5
        mov     ecx, 8                  ; ecx = 8

        xor     eax, eax                ; eax = 0
; !?
        add     esi, edx

        ; Planar to chunky-4
.L1
        mov     al, byte [esi + 0]
        mov     edx, dword [ebx + eax * 4]
        mov     al, byte [esi + 1]
        shr     edx, 1
        or      edx, [ebx + eax * 4]
        mov     al, byte [esi + 2]
        shr     edx, 1
        or      edx, [ebx + eax * 4]
        mov     al, byte [esi + 3]
        shr     edx, 1
        or      edx, [ebx + eax * 4]
        add     esi, 4                  ; esi += 4

        ; Expand chunky-4 to chunky-8
        mov     eax, edx
        and     edx, 0xF0F0F0F0
        and     eax, 0x0F0F0F0F
        shr     edx, 4
        mov     [edi], eax
        xor     eax, eax
        mov     [edi + 4], edx
        add     edi, 8

        dec     ecx
        jnz     .L1

;       pop     edx

        pop     edi
        pop     esi
        pop     ecx
        pop     ebx

        ret

;------------------------------------------------------------------------------
; GCC version
; (does not compile under Windows)
;------------------------------------------------------------------------------
;	.text
;	.align	2
;_Decode_Tile_ASM:
;	pushl	%ebx
;	pushl	%ecx
;	pushl	%esi
;	pushl	%edi
;
;        leal    _planar_to_chunk, %ebx
;	movl	_VRAM, %esi
;	movl	20(%esp), %edx          ;# edx = tile_n
;	movl	24(%esp), %edi          ;# edi = gfx.Tile_Decoded[tile_n]
;	shll	$5, %edx
;	movl	$8, %ecx
;	xorl	%eax, %eax
;	addl	%edx, %esi
;
;      #; planar to chunky-4
;      #;
;1:	movb	(%esi), %al
;	movl	(%ebx,%eax,4), %edx
;	movb	1(%esi), %al
;	shrl	$1, %edx
;	orl	(%ebx,%eax,4), %edx
;	movb	2(%esi), %al
;	shrl	$1, %edx
;	orl	(%ebx,%eax,4), %edx
; 	movb	3(%esi), %al
;	shrl	$1, %edx
;	orl	(%ebx,%eax,4), %edx
;	addl	$4, %esi
;
;      #; expand chunky-4 to chunky-8
;      #;
;	movl	%edx,%eax
;	andl	$0xF0F0F0F0, %edx
;	andl	$0x0F0F0F0F, %eax
;	shrl	$4, %edx
;	movl	%eax, (%edi)
;	xorl	%eax, %eax
;	movl	%edx, 4(%edi)
;	addl	$8, %edi
;
;	decl	%ecx
;	jnz	1b
;
;	popl	%edi
;	popl	%esi
;	popl	%ecx
;	popl	%ebx
;	ret
;------------------------------------------------------------------------------
s
;------------------------------------------------------------------------------
; Function: Sprite_Collide_Line_ASM
; Parameters: byte *p_src, int x, int line
;------------------------------------------------------------------------------
;void     Sprite_Collide_Line (byte *p_src, int x)
;{
;  int           *p_collision_table;
;
;  p_collision_table = &Sprites_Collision_Table [x];
;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src++) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE; return; } } p_collision_table++;
;  if (*p_src  ) { if ((p_collision_table[0])++ > 0) { sms.VDP_Status |= VDP_STATUS_SpriteCollision; Do_Collision = FALSE;         } }
;}
;------------------------------------------------------------------------------
_Sprite_Collide_Line_ASM:

        push    eax
        push    ecx
        ; push  edx

        mov     ecx, [_Sprites_Collision_Table] ; ecx = Sprites_Collision_Table
        mov     eax, dword [esp + 16]           ; eax = x
        shl     eax, 2                          ;
        add     ecx, eax                        ; ecx = Sprites_Collision_Table + x

        mov     eax, dword [esp + 12]           ; eax = p_src
        ; mov   edx, 8                          ; edx = 8 (loop counter)

.L0
        test    byte [eax + 0], 0x0F            ; if (p_src[0] == 0)
        jz      .LB0                            ;    goto LB0
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB0    add     ecx, 4                          ; Sprites_Collision_Table++

.L1
        test    byte [eax + 1], 0x0F            ; if (p_src[1] == 0)
        jz      .LB1                            ;    goto LB1
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB1    add     ecx, 4                          ; Sprites_Collision_Table++

.L2
        test    byte [eax + 2], 0x0F            ; if (p_src[2] == 0)
        jz      .LB2                            ;    goto LB2
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB2    add     ecx, 4                          ; Sprites_Collision_Table++

.L3
        test    byte [eax + 3], 0x0F            ; if (p_src[3] == 0)
        jz      .LB3                            ;    goto LB3
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB3    add     ecx, 4                          ; Sprites_Collision_Table++

.L4
        test    byte [eax + 4], 0x0F            ; if (p_src[4] == 0)
        jz      .LB4                            ;    goto LB4
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB4    add     ecx, 4                          ; Sprites_Collision_Table++

.L5
        test    byte [eax + 5], 0x0F            ; if (p_src[5] == 0)
        jz      .LB5                            ;    goto LB5
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB5    add     ecx, 4                          ; Sprites_Collision_Table++

.L6
        test    byte [eax + 6], 0x0F            ; if (p_src[6] == 0)
        jz      .LB6                            ;    goto LB6
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB6    add     ecx, 4                          ; Sprites_Collision_Table++

.L7
        test    byte [eax + 7], 0x0F            ; if (p_src[7] == 0)
        jz      .LB7                            ;    goto LB7
        test    dword [ecx], 0xFF               ; if (Sprites_Collision_Table[0] == 0)
        jnz     near .Lcollide                  ;    goto Lcollide
        inc     dword [ecx]                     ; (Sprites_Collision_Table[0])++
.LB7
        ; dec   edx                             ; edx--
        ; jnz   .L0                             ; while (edx > 0) ..

.Lend
        ; pop   edx
        pop     ecx
        pop     eax
        ret

.Lcollide
        or      byte [_sms + 69], 0x20          ; sms.VDP_Status |= VDP_STATUS_SpriteCollision; FIXME
        mov     dword [_Do_Collision], 0        ; Do_Collision = FALSE
        ; pop   edx
        pop     ecx
        pop     eax
        ret

;------------------------------------------------------------------------------

%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
