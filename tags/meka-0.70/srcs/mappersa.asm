;------------------------------------------------------------------------------
; MEKA - mappersa.asm
; Mappers - Assembly Routines (for memory handlers)
;------------------------------------------------------------------------------

BITS    32

; DATA SECTION ----------------------------------------------------------------
SECTION .data
; USE32

; Uncomment for UNIX build
;%DEFINE  _Read_Default_ASM         Read_Default_ASM
;%DEFINE  _Write_Mapper_32kRAM_ASM  Write_Mapper_32kRAM_ASM
;%DEFINE  _Mem_Pages                Mem_Pages
	
GLOBAL  _Read_Default_ASM
GLOBAL  _Write_Mapper_32kRAM_ASM
EXTERN  _Mem_Pages

; CODE SECTION ----------------------------------------------------------------
SECTION .text
; USE32

;------------------------------------------------------------------------------
; Function: Read_Default_ASM
; Parameters: word Addr
; Return: byte data
;------------------------------------------------------------------------------
; byte Read_Default(word Addr)
; return (Mem_Pages [Addr >> 13] [Addr /* & 0x1FFF */]);
;------------------------------------------------------------------------------
_Read_Default_ASM:

        push    edi
        push    esi
        push    ebp

        ;xor   edi, edi
        ;mov   di, word [esp + 16]      ; edi = Addr & 0xFFFF
        mov     edi, dword [esp + 16]   ; edi = Addr

        mov     ebp, _Mem_Pages         ; ebp = Mem_Pages
        mov     esi, edi                ; esi = Addr

        ; and   esi, 0x1FFF             ; esi: Addr & 0x1FFF
        shr     edi, 13                 ; edi: Addr >> 13

        mov     edi, dword [ebp + edi * 4]    ; edi = Mem_Pages [Addr >> 13]
        mov     eax, dword [edi + esi]         ; al = edi [Addr]

        pop     ebp
        pop     esi
        pop     edi

        ret
;------------------------------------------------------------------------------
; GCC version
; (does not compile under Windows)
;------------------------------------------------------------------------------
; 	.text
; 	.align	2
;
;_Read_Default_ASM:
;        pushl   %edi
;        pushl   %esi
;        pushl   %ebp
;
;        movl    16(%esp), %edi          #; edi = Addr
;        leal    _Mem_Pages, %ebp        #; ebp = Mem_Pages
;        movl    %edi, %esi              #; esi = Addr
;
;        ;#andl    $0x1FFF, %esi           #; esi: Addr & 0x1FFF
;        shrl    $13, %edi               #; edi: Addr >> 13
;
;        movl    (%ebp, %edi, 4), %edi   #; edi = Mem_Pages [Addr >> 13]
;        movb    (%edi, %esi, 1), %al    #; al = edi [Addr & 0x1FFF]
;
;        popl    %ebp
;        popl    %esi
;        popl    %edi
;        ret
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Function: Write_Mapper_32kRAM_ASM
; Parameters: word Addr, byte value
; Return: nothing
;------------------------------------------------------------------------------
; switch (Addr >> 13)
;   {
;   case 4: Mem_Pages [4] [Addr & 0x1FFF] = Value; return;
;   case 5: Mem_Pages [5] [Addr & 0x1FFF] = Value; return;
;   case 6: Mem_Pages [6] [Addr & 0x1FFF] = Value; return;
;   case 7: Mem_Pages [7] [Addr & 0x1FFF] = Value; return;
;   }
;------------------------------------------------------------------------------
_Write_Mapper_32kRAM_ASM:

        push    edi
        push    esi
        push    ebp
        push    ecx

        ; xor   edi, edi
        ; mov   di, word [esp + 20]     ; edi = Addr
        mov     edi, dword [esp + 20]   ; edi = Addr

        mov     esi, edi                ; esi = Addr

        ; and   esi, 0x1FFF             ; esi: Addr & 0x1FFF
        shr     edi, 13                 ; edi: Addr >> 13

        test    edi, 0x04               ; test bit 2, if Addr is < 0x8000
        jz      .write_unable           ; then do not perform the write

        mov     ebp, _Mem_Pages         ; ebp = Mem_Pages
        mov     ecx, dword [esp + 24]   ; ecx = value

        mov     edi, [ebp + edi * 4]    ; edi = Mem_Pages [Addr >> 13]
        mov     [edi + esi], cl         ; edi [Addr] = cl (low byte of value)

.write_unable
        pop     ecx
        pop     ebp
        pop     esi
        pop     edi

        ret
;------------------------------------------------------------------------------
; GCC version
; (does not compile under Windows)
;------------------------------------------------------------------------------
;	.text
;	.align	2
;
;_Write_Mapper_32kRAM_ASM:
;        pushl   %edi
;        pushl   %esi
;        pushl   %ebp
;        pushl   %ecx
;
;        movl    20(%esp), %edi          #; edi = Addr
;        movl    %edi, %esi              #; esi = Addr
;
;        ;#andl    $0x1FFF, %esi           #; esi: Addr & 0x1FFF
;        shrl    $13, %edi               #; edi = Addr >> 13
;
;        test    $4, %edi
;        jz      1f
;
;        leal    _Mem_Pages, %ebp        #; ebp = Mem_Pages
;
;        movl    24(%esp), %ecx          ;# ecx = value
;
;        movl    (%ebp, %edi, 4), %edi
;        movb    %cl, (%edi, %esi, 1)
;
;1:
;        popl    %ecx
;        popl    %ebp
;        popl    %esi
;        popl    %edi
;        ret
;------------------------------------------------------------------------------

END

