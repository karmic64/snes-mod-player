    .cpu "65816"
    .include "snes.inc"
    
    .include "mod-list.asm"
    
    
    .enc "ascii"
    .cdef " ~",$20
    
    .virtual 0
rand
    .fill 16
    
    
spc_flag
    .byte ?
last_song
    .byte ?
spc_busy
    .byte ?
    
    
rawjoy
    .word ?
joy
    .word ?
    
    
    
selected
    .byte ?
    .endv
    
    
    
    
    .logical $808000
    .autsiz
    
    
    
    .include "spc-snes.asm"
    
    
    
    .dpage 0
    .databank 0
    .as
    .xs
reset
    phk
    plb
    .databank *>>16
    
    pea INIDISP
    pld
    .dpage INIDISP
    
    lda #0
    xba
    
    lda #$8f
    sta INIDISP
    
    lda #1
    sta TM
    stz TS
    
    stz SETINI
    sta BGMODE
    stz MOSAIC
    lda #$fc
    sta BG1SC
    stz BG12NBA
    
    lda #$ff
    stz BG1VOFS
    stz BG1VOFS
    sta BG1HOFS
    sta BG1HOFS
    
    lda #1
    sta OBSEL
    
    stz W12SEL
    stz WOBJSEL
    stz TMW
    lda #$30
    sta CGWSEL
    stz CGADSUB
    
    
    rep #P_X
    lda #$80
    sta VMAIN
    
    ldx #0
    stx VMADDL
    ldx #size(font)-1
-   lda font,x
    tay
    sty VMDATAL
    txa
    and #7
    bne +
    tay
    .rept 8
      sty VMDATAL
    .next
+   dex
    bpl -
    
    stz CGADD
    stz CGDATA
    stz CGDATA
    txa
    sta CGDATA
    sta CGDATA
    
    
    ldx #$7c00
    stx VMADDL
    ldy #0
    ldx #(32*32)-1
-   sty VMDATAL
    dex
    bpl -
    
    
    
    
    pea 0
    pld
    .dpage 0
    
    
    
    jsl spc_init
    
    
    lda #$ff
    sta spc_flag
    sta last_song
    inc a
    sta spc_busy
    
    
    
    
    ldy #$ffff
    sty rawjoy
    
    lda rand
    bne +
    lda #$55
    sta rand
+   
    
    bra +
-   sbc #len(mod_list)
+   cmp #len(mod_list)
    bcs -
    sta selected
    sta spc_flag
    
    
    
    
    lda RDNMI,b
    lda #$81
    sta NMITIMEN,b
    
    
mainloop
    lda rand
    lsr
    .for i = 1, i < 16, i=i+1
      ror rand+i
    .next
    bcc +
    eor #$e1
+   sta rand
    
    lda spc_flag
    bmi mainloop
    pha
    lda #1
    sta spc_busy
    lda #$ff
    sta spc_flag
    pla
    cmp #$7f
    beq _stop
    cmp last_song
    beq +
    sta last_song
    asl
    adc last_song
    tax
    
    lda module_tbl,x
    ldy module_tbl+1,x
    jsl spc_upload_module
    
+   jsl spc_play_module
    
_end
    lda #0
    sta spc_busy
    bra mainloop
    
    
_stop
    jsl spc_stop_module
    bra _end
    
    
    
    
    
    
module_tbl
ptr := module_base
    .for i = 0, i < len(mod_list), i=i+1
      .byte (ptr / $8000) | $80
      .word (ptr & $7fff) | $8000
      
      ptr += len(mod_list[i][0])
      
      .cerror $0700+size(spcdata)+len(mod_list[i][0]) > $ffff, mod_list[i][1] .. " is too big"
    .next
    
    
    
    
module_names_tbl
index := 0
    .for i = 0, i < len(mod_list), i=i+1
      .byte len(mod_list[i][1])
      .word index
      
      index += len(mod_list[i][1])
    .next
    
    
module_names
    .for i = 0, i < len(mod_list), i=i+1
      .text mod_list[i][1]
    .next
    
    
    
    
font  .text binary("EK-ANGLE.itf",0,$80*8)[::-1]





nmi rep #P_M|P_X
    pha
    phx
    phy
    phd
    phb
    
    lda #0
    tcd
    .dpage 0
    phk
    plb
    .databank *>>16
    
    sep #P_M
    
    
    lda #$80
    sta VMAIN,b
    
    ldx #$7e00
    stx VMADDL,b
    ldy #' '
    ldx #32
-   sty VMDATAL,b
    dex
    bne -
    
    lda selected
    asl
    adc selected
    tay
    lda module_names_tbl,y
    lsr
    eor #$ff
    inc a
    clc
    adc #16
    rep #P_M
    clc
    adc #$7e00
    sta VMADDL,b
    lda #0
    sep #P_M
    ldx module_names_tbl+1,y
    lda module_names_tbl,y
    tay
    
-   lda module_names,x
    sta VMDATAL,b
    stz VMDATAH,b
    inx
    dey
    bne -
    
    
    
    
    
    lda #$0f
    sta INIDISP,b
    
    
    
    
    
-   lda HVBJOY,b
    lsr
    bcs -
    
    rep #P_M
    ldy rawjoy
    lda JOY1L,b
    sta rawjoy
    tya
    eor #$ffff
    and rawjoy
    sta joy
    
    
    
    
    sep #P_X
    ldy selected
    lda joy
    bit #BTN_L | BTN_U
    beq +
    dey
    bpl +
    ldy #len(mod_list)-1
+   bit #BTN_R | BTN_D
    beq +
    iny
    cpy #len(mod_list)
    bcc +
    ldy #0
+   sty selected
    
    bit #BTN_X | BTN_Y
    beq +
    ldy #$7f
    bra ++
+   
    bit #BTN_A | BTN_B
    beq ++
+   ldx spc_busy
    bne +
    sty spc_flag
+   
    
    
    
    
    
    
    
    
    plb
    pld
    rep #P_M|P_X
    ply
    plx
    pla
    rti


    
    
    
    
    * = $80ffb0
    .as
    .xs
    .dpage 0
    .databank 0
reset_stub
    clc
    xce
    rol MEMSEL
    jml reset
    
    
nmi_stub
    jml nmi
    
    
    * = $80ffc0
    .text "SPC SOUND            "
    .byte $30,0
    .byte 11,0
    
    * = $80ffea
    .word nmi_stub&$ffff
    
    * = $80fffc
    .word reset_stub&$ffff
    .word 0
    .here
    
    
module_base
    .for i = 0, i < len(mod_list), i=i+1
      .text mod_list[i][0]
    .next
    
    * = $1fffff
    .byte 0
    