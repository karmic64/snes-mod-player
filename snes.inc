;processor flags
P_N  = $80
P_V  = $40
P_M  = $20
P_X  = $10
P_D  = $08
P_I  = $04
P_Z  = $02
P_C  = $01

;buttons
BTN_B = $8000
BTN_Y = $4000
BTN_SEL = $2000
BTN_ST = $1000
BTN_U = $0800
BTN_D = $0400
BTN_L = $0200
BTN_R = $0100
BTN_A = $0080
BTN_X = $0040
BTN_LB = $0020
BTN_RB = $0010


;ppu writable regs
INIDISP    = $2100
OBSEL      = $2101
OAMADDL    = $2102
OAMADDH    = $2103
OAMDATA    = $2104
BGMODE     = $2105
MOSAIC     = $2106
BG1SC      = $2107
BG2SC      = $2108
BG3SC      = $2109
BG4SC      = $210a
BG12NBA    = $210b
BG34NBA    = $210c
BG1HOFS    = $210d
BG1VOFS    = $210e
M7HOFS     = $210d
M7VOFS     = $210e
BG2HOFS    = $210f
BG2VOFS    = $2110
BG3HOFS    = $2111
BG3VOFS    = $2112
BG4HOFS    = $2113
BG4VOFS    = $2114
VMAIN      = $2115
VMADDL     = $2116
VMADDH     = $2117
VMDATAL    = $2118
VMDATAH    = $2119
M7SEL      = $211a
M7A        = $211b
M7B        = $211c
M7C        = $211d
M7D        = $211e
M7X        = $211f
M7Y        = $2120
CGADD      = $2121
CGDATA     = $2122
W12SEL     = $2123
W34SEL     = $2124
WOBJSEL    = $2125
WH0        = $2126
WH1        = $2127
WH2        = $2128
WH3        = $2129
WBGLOG     = $212a
WOBJLOG    = $212b
TM         = $212c
TS         = $212d
TMW        = $212e
TSW        = $212f
CGWSEL     = $2130
CGADSUB    = $2131
COLDATA    = $2132
SETINI     = $2133

;ppu readable regs
MPYL       = $2134
MPYM       = $2135
MPYH       = $2136
SLHV       = $2137
RDOAM      = $2138
RDVRAML    = $2139
RDVRAMH    = $213a
RDCGRAM    = $213b
OPHCT      = $213c
OPVCT      = $213d
STAT77     = $213e
STAT78     = $213f

;apu comms
APUIO0     = $2140
APUIO1     = $2141
APUIO2     = $2142
APUIO3     = $2143

;wram access
WMDATA     = $2180
WMADDL     = $2181
WMADDM     = $2182
WMADDH     = $2183

;cpu r/w ports
JOYWR      = $4016
JOYA       = $4016
JOYB       = $4017

;cpu writable ports
NMITIMEN   = $4200
WRIO       = $4201
WRMPYA     = $4202
WRMPYB     = $4203
WRDIVL     = $4204
WRDIVH     = $4205
WRDIVB     = $4206
HTIMEL     = $4207
HTIMEH     = $4208
VTIMEL     = $4209
VTIMEH     = $420a
MDMAEN     = $420b
HDMAEN     = $420c
MEMSEL     = $420d

;cpu readable ports
RDNMI      = $4210
TIMEUP     = $4211
HVBJOY     = $4212
RDIO       = $4213
RDDIVL     = $4214
RDDIVH     = $4215
RDMPYL     = $4216
RDMPYH     = $4217
JOY1L      = $4218
JOY1H      = $4219
JOY2L      = $421a
JOY2H      = $421b
JOY3L      = $421c
JOY3H      = $421d
JOY4L      = $421e
JOY4H      = $421f

;dma registers
DMAP0      = $4300
BBAD0      = $4301
A1T0L      = $4302
A1T0H      = $4303
A1B0       = $4304
DAS0L      = $4305
DAS0H      = $4306
DASB0      = $4307
A2A0L      = $4308
A2A0H      = $4309
NTRL0      = $430a
;channel 1
DMAP1      = $4310
BBAD1      = $4311
A1T1L      = $4312
A1T1H      = $4313
A1B1       = $4314
DAS1L      = $4315
DAS1H      = $4316
DASB1      = $4317
A2A1L      = $4318
A2A1H      = $4319
NTRL1      = $431a
;channel 2
DMAP2      = $4320
BBAD2      = $4321
A1T2L      = $4322
A1T2H      = $4323
A1B2       = $4324
DAS2L      = $4325
DAS2H      = $4326
DASB2      = $4327
A2A2L      = $4328
A2A2H      = $4329
NTRL2      = $432a
;channel 3
DMAP3      = $4330
BBAD3      = $4331
A1T3L      = $4332
A1T3H      = $4333
A1B3       = $4334
DAS3L      = $4335
DAS3H      = $4336
DASB3      = $4337
A2A3L      = $4338
A2A3H      = $4339
NTRL3      = $433a
;channel 4
DMAP4      = $4340
BBAD4      = $4341
A1T4L      = $4342
A1T4H      = $4343
A1B4       = $4344
DAS4L      = $4345
DAS4H      = $4346
DASB4      = $4347
A2A4L      = $4348
A2A4H      = $4349
NTRL4      = $434a
;channel 5
DMAP5      = $4350
BBAD5      = $4351
A1T5L      = $4352
A1T5H      = $4353
A1B5       = $4354
DAS5L      = $4355
DAS5H      = $4356
DASB5      = $4357
A2A5L      = $4358
A2A5H      = $4359
NTRL5      = $435a
;channel 6
DMAP6      = $4360
BBAD6      = $4361
A1T6L      = $4362
A1T6H      = $4363
A1B6       = $4364
DAS6L      = $4365
DAS6H      = $4366
DASB6      = $4367
A2A6L      = $4368
A2A6H      = $4369
NTRL6      = $436a
;channel 7
DMAP7      = $4370
BBAD7      = $4371
A1T7L      = $4372
A1T7H      = $4373
A1B7       = $4374
DAS7L      = $4375
DAS7H      = $4376
DASB7      = $4377
A2A7L      = $4378
A2A7H      = $4379
NTRL7      = $437a



