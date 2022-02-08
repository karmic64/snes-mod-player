    .cpu "65816"
    .autsiz
    
    
    .virtual $f0
dpage_base
spc_comms_cnt
    .byte ?
spc_temp
    .fill 8
    .endv
    
    
spc_init
    php
    rep #P_M | P_X
    pha
    phx
    phy
    phb
    phd
    sep #P_M
    ;rep #P_X
    
    pea #APUIO0 & $ff00
    pld
    .dpage APUIO0 & $ff00
    phk
    plb
    .databank *>>16
    
    ldx #$bbaa
-   cpx APUIO0
    bne -
    
    ldx #$0700
    stx APUIO2
    lda #$01
    sta APUIO1
    lda #$cc
    sta APUIO0
-   cmp APUIO0
    bne -
    
    ldy #0
-   lda spcdata,y
    sta APUIO1
    tya
    sta APUIO0
-   cmp APUIO0
    bne -
    iny
    cpy #size(spcdata)
    bcc --
    
    ldx #$0700
    stx APUIO2
    stz APUIO1
    lda #((size(spcdata)+2) & $ff) | 1
    sta APUIO0
-   cmp APUIO0
    bne -
    
    
    stz spc_comms_cnt,b
    stz APUIO0
    stz APUIO1
    stz APUIO2
    stz APUIO3
    lda #$9a
-   cmp APUIO3
    bne -
    
    pld
    plb
    rep #P_M | P_X
    ply
    plx
    pla
    plp
    rtl
    
    
    
    ;bank in A, index in 16-bit Y
spc_upload_module
    php
    phb
    phd
    
    sep #P_M
    rep #P_X
    pea dpage_base & $ff00
    pld
    .dpage dpage_base & $ff00
    phk
    plb
    .databank (*>>16)
    
    stz spc_temp
    stz spc_temp+1
    sta spc_temp+2
    
    lda #0
    xba
    jsr spc_send_byte
    
    
    ;;;;send patterns
    jsr _getbyte ;amount of patterns
    sta spc_temp+3
    jsr spc_send_byte
    
-   jsr _getbyte ;pattern size
    tax ;0 length = $100 length
    bne +
    ldx #$0100
+   jsr spc_send_byte
    
-   jsr _getbyte ;actual data
    jsr spc_send_byte
    dex
    bne -
    dec spc_temp+3
    bne --
    
    
    
    
    ;;;;send sequences
    
    jsr _getbyte ;song length
    tax
    bne + ;0 length = $100 length
    ldx #$0100
+   stx spc_temp+3
    jsr spc_send_byte
    
    
    lda #4 ;actual data
    sta spc_temp+5
-   ldx spc_temp+3
-   jsr _getbyte
    jsr spc_send_byte
    dex
    bne -
    dec spc_temp+5
    bne --
    
    
    ;;;;send samples
    lda #31 ;always 31 count
    sta spc_temp+3
    
-   jsr _getbyte ;size
    sta spc_temp+4
    jsr spc_send_byte
    jsr _getbyte
    sta spc_temp+5
    jsr spc_send_byte
    
    ldx spc_temp+4 ;0 means no sample
    beq _skipsamp
    inx
    inx
    inx
    inx
    
-   jsr _getbyte ;loop point, volume, finetune, actual data
    jsr spc_send_byte
    dex
    bne -
    
_skipsamp
    dec spc_temp+3
    bne --
    
    
    
    pld
    plb
    plp
    rtl
    
_getbyte
    lda [spc_temp],y
    iny
    beq +
    rts
+   ldy #$8000
    inc spc_temp+2
    rts
    
    
    
    
spc_play_module
    php
    sep #P_M
    pha
    phb
    phd
    
    
    pea dpage_base & $ff00
    pld
    .dpage dpage_base & $ff00
    phk
    plb
    
    lda #2
    jsr spc_send_byte
    
    pld
    plb
    pla
    plp
    rtl
    
    
    
spc_stop_module
    php
    sep #P_M
    pha
    phb
    phd
    
    
    pea dpage_base & $ff00
    pld
    .dpage dpage_base & $ff00
    phk
    plb
    
    lda #4
    jsr spc_send_byte
    
    pld
    plb
    pla
    plp
    rtl
    
    
    
    
    
    
    
spc_send_byte
    .as
    .databank 0
    .dpage dpage_base & $ff00
    pha
-   lda APUIO0
    cmp spc_comms_cnt
    bne -
    pla
    sta APUIO1
    lda spc_comms_cnt
    inc a
    sta APUIO0
    sta spc_comms_cnt
    rts
    
    
    
    
spcdata .binary "spc.spcbin"