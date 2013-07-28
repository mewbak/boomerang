#
# Copyright (C) 2004 by Intel Research
# created by Jens Troeger
#
# FILE:     arm.spec
# OVERVIEW: This is the New Jersey Machine Code Toolkit core specification file
#           for the ARM processor; cf. the ARM Architecture Reference Manual
#           Addison-Wesley 2001
#
# $Author: savage $
# $Date: 2004/05/24 21:00:03 $
# $Id: arm.spec,v 1.22 2004/05/24 21:00:03 savage Exp $
# $Revision: 1.22 $
#
# 07 Apr 2004 - Jens: created
# 19 Apr 2004 - Jens: except for the SBO/SBZ fields the spec is complete for ARM
# 22 Apr 2004 - Jens: added SBO/SBZ fields
# 23 Apr 2004 - Jens: started to modify the specification such that Walkabout's emugen
#                     accepts the file (it does not implement the complete syntax):
#                     opcodes of constructors (for Walkabout) must not have fieldnames,
#                     only pattern names
# 18 May 2004 - Jens: i finished the spec and MLTK actually accepts it; however, it seems
#                     to be too complex to process...
# 19 May 2004 - Jens: started to reduce the number of fields which hopefully will reduce
#                     time/memory required to process the spec
# 20 May 2004 - Jens: the MLTK fails because of the size of the matcher file; so to reduce
#                     number of instruction-semantics i make "cond" into an operand rather
#                     than part of the opcode

# what the...
#   ldm(3)  what's the ^ bit?

# instruction fields (cf. section 3.1) the table basically contains all types of instructions.

fields of instruction (32)

#  # condition 
#  condition 28:31 
  cond 28:31

  # register fields for all instructions (note that the SBZ and SBO fields are
  # either r0 or r15 for the different registers!)
  Rn 16:19 CRn 16:19 RdHi 16:19 RdLo 12:15 Rd 12:15 CRd 12:15 Rs 8:11 Rm 0:3 CRm 0:3

  # some instructions use register fields as value fields; from a syntactical point
  # of view this distinction doesn't matter, however, SSL doesn't allow to interpret
  # a register-field as a value -> overlay value (V) fields
  VRn 16:19 VCRn 16:19 VRdHi 16:19 VRdLo 12:15 VRd 12:15 VCRd 12:15 VRs 8:11 VRm 0:3 VCRm 0:3

  # individual bits (i wish i could have aliases for fields)
  bit25 25:25 bit24 24:24 bit23 23:23 bit22 22:22 bit21 21:21 bit20 20:20
  bit7 7:7 bit6 6:6 bit5 5:5 bit4 4:4

  # shifter operand for arith/log, moves and compares
  shiftamount 7:11 shift 5:6

  # i just call fields "blob" because they have different names for different instructions
  blob_26_27 26:27 blob_25_27 25:27 blob_24_25 24:25 blob_24_27 24:27 blob_23_27 23:27 blob_23_24 23:24 
  blob_21_27 21:27 blob_21_24 21:24 blob_21_23 21:23 blob_21_22 21:22 blob_20_27 20:27 blob_20_23 20:23 blob_20_21 20:21
  blob_8_19 8:19 blob_5_7 5:7 blob_5_6 5:6 blob_4_7 4:7

  # fields stretching from bit 0
  blob_0_11 0:11 blob_0_7 0:7 blob_0_15 0:15 blob_0_23 0:23

# further field information (for Walkabout fieldinfos must be replaced by patterns!)

fieldinfo

  # names+values for register fields
  [ Rn CRn Rd CRd Rs Rm CRm] is [
    names [ "%r0" "%r1" "%r2" "%r3" "%r4" "%r5" "%r6" "%r7" 
            "%r8" "%r9" "%r10" "%r11" "%r12" "%r13" "%r14" "%r15" ] ]

# here are the patterns that represent the fieldinfo

#patterns
#  [ EQ NE CS CC MI OK VS VC 
#    HI KS GE LT GT KE AL NV ] is condition = { 0 to 15 }
#
#patterns
#  cond is EQ | NE | CS | CC | MI | OK | VS | VC | 
#          HI | KS | GE | LT | GT | KE | AL | NV

# it is ugly, i know but at least Walkabout doesn't barf anymore
# (its parser does not accept fieldnames in constructors); in a 
# decent spec this should can simply be removed and the fieldnames
# ought to be used directly in a constructor

patterns 
  [ zS oS ] is bit20 = [ 0 1 ]
  S         is zS | oS

  [ zW oW ] is bit21 = [ 0 1 ]
  W         is zW | oW

patterns # shifts for the shifter operands and addressing modes
  [ LSL LSR ASR ROR ] is shift = [ 0 1 2 3 ]

patterns 
  shifty is LSL | LSR | ASR | ROR

# general constructors for shifter operands (cf. section 5.1)
# the shifter operand is encoded in bits 0:11 and 25:25 (I)

patterns
  SHI         is bit25 = 1
  SHR         is bit25 = 0 & bit4 = 0 & shift = 0 & shiftamount = 0
  SHRRLSL     is bit25 = 0 & bit4 = 1 & shift = 0
  SHRRLSR     is bit25 = 0 & bit4 = 1 & shift = 1
  SHRRASR     is bit25 = 0 & bit4 = 1 & shift = 2
  SHRRROR     is bit25 = 0 & bit4 = 1 & shift = 3
  SHRILSL     is bit25 = 0 & bit4 = 0 & shift = 0
  SHRILSR     is bit25 = 0 & bit4 = 0 & shift = 1
  SHRIASR     is bit25 = 0 & bit4 = 0 & shift = 2
  SHRIROR     is bit25 = 0 & bit4 = 0 & shift = 3
  SHRXR       is bit25 = 0 & bit4 = 0 & shift = 3 & shiftamount = 0

patterns 
  shreg       is SHRRLSL | SHRRLSR | SHRRASR | SHRRROR
  shimm       is SHRILSL | SHRILSR | SHRIASR | SHRIROR

# data processing instructions (cf. section 3.4)

patterns 
  [ AND EOR SUB RSB
    ADD ADC SBC RSC
    _   _   _   _
    ORR _   BIC _   ] is blob_21_24 = { 0 to 15 }   & blob_26_27 = 0

  [ MOV MVN ]         is blob_21_24 = [ 13 15 ]     & blob_26_27 = 0 & VRn = 0

  [ TST TEQ CMP CMN ] is blob_21_24 = [ 8 9 10 11 ] & blob_26_27 = 0 & VRd = 0 & bit20 = 1

patterns 
  moves    is MOV | MVN
  cmps     is CMP | CMN | TST | TEQ
  arith    is ADD | SUB | RSB | ADC | SBC | RSC
  log      is AND | BIC | EOR | ORR
  arithlog is arith | log
 
constructors # move instructions
  moves^S^SHI    cond, Rd, VRs, blob_0_7 # Rs = rotate
  moves^S^SHR    cond, Rd, Rm
  moves^S^shreg  cond, Rd, Rm, Rs
  moves^S^shimm  cond, Rd, Rm, shiftamount
  moves^S^SHRXR  cond, Rd, Rm

constructors # compare instructions
  cmps^SHI    cond, Rn, VRs, blob_0_7 # Rs = rotate
  cmps^SHR    cond, Rn, Rm
  cmps^shreg  cond, Rn, Rm, Rs
  cmps^shimm  cond, Rn, Rm, shiftamount
  cmps^SHRXR  cond, Rn, Rm

constructors # arith and logical instructions
  arithlog^S^SHI    cond, Rd, Rn, VRs, blob_0_7 # Rs = rotate
  arithlog^S^SHR    cond, Rd, Rn, Rm
  arithlog^S^shreg  cond, Rd, Rn, Rm, Rs
  arithlog^S^shimm  cond, Rd, Rn, Rm, shiftamount
  arithlog^S^SHRXR  cond, Rd, Rn, Rm

# multiply instructions (cf. section 3.5)

patterns # define extended opcodes for the different multiply instructions
  muls      is blob_4_7 = 9
  MLA       is muls & blob_21_27 = 1 # bit23 = 0 & bit22 = 0 & bit21 = 1
  MUL       is muls & blob_21_27 = 0 & VRd = 0 # bit23 = 0 & bit22 = 0 & bit21 = 0
  SMLAL     is muls & blob_21_27 = 7 # bit23 = 1 & bit22 = 1 & bit21 = 1
  SMULL     is muls & blob_21_27 = 6 # bit23 = 1 & bit22 = 1 & bit21 = 0
  UMLAL     is muls & blob_21_27 = 5 # bit23 = 1 & bit22 = 0 & bit21 = 1
  UMULL     is muls & blob_21_27 = 4 # bit23 = 1 & bit22 = 0 & bit21 = 0

patterns
  longmul   is SMLAL | SMULL | UMLAL | UMULL

constructors
  MLA^S       cond, Rd,   Rm,   Rs, Rn
  MUL^S       cond, Rn,   Rm,   Rs # Rn is called Rd in the book (pg A4-66) but actually refers to bits 16:19
  longmul^S   cond, RdLo, RdHi, Rm, Rs

# load and store instructions

# load and store word or unsigned byte (cf. section 5.2)

patterns 
  lwub   is blob_26_27 = 1 & bit20 = 1
  swub   is blob_26_27 = 1 & bit20 = 0

patterns
  LDR    is lwub & bit22 = 0
  LDRB   is lwub & bit22 = 1 
  STR    is swub & bit22 = 0
  STRB   is swub & bit22 = 1

patterns
  lswub  is LDR | STR | LDRB | STRB

patterns # WUB addressing modes
  WUB_I     is blob_24_25 = 1 & bit21 = 0 # imm # bit25 = 0 & bit24 = 1
  WUB_IPX   is blob_24_25 = 1 & bit21 = 1 # imm pre index # bit25 = 0 & bit24 = 1
  WUB_IOX   is blob_24_25 = 0 & bit21 = 0 # imm post index # bit25 = 0 & bit24 = 0
  WUB_SR    is blob_24_25 = 3 & bit21 = 0 & bit4 = 0 # scaled register # bit25 = 1 & bit24 = 1
  WUB_SRPX  is blob_24_25 = 3 & bit21 = 1 & bit4 = 0 # scaled register pre index # bit25 = 1 & bit24 = 1
  WUB_SROX  is blob_24_25 = 3 & bit21 = 0 & bit4 = 0 # scaled register post index # bit25 = 1 & bit24 = 0
  WUB_R     is blob_24_25 = 3 & bit21 = 0 & bit4 = 0 & shift = 0 & shiftamount = 0 # register # bit25 = 1 & bit24 = 1
  WUB_RPX   is blob_24_25 = 3 & bit21 = 1 & bit4 = 0 & shift = 0 & shiftamount = 0 # register pre index # bit25 = 1 & bit24 = 1
  WUB_POX   is blob_24_25 = 2 & bit21 = 0 & bit4 = 0 & shift = 0 & shiftamount = 0 # register post index # bit25 = 1 & bit24 = 0

  wubimm    is WUB_I  | WUB_IPX  | WUB_IOX
  wubsreg   is WUB_SR | WUB_SRPX | WUB_SROX
  wubreg    is WUB_R  | WUB_RPX  | WUB_POX

constructors 
  lswub^wubimm          cond, Rd, Rn, bit23, blob_0_11
  lswub^wubsreg^shifty  cond, Rd, Rn, bit23, Rm, shiftamount
  lswub^wubreg          cond, Rd, Rn, bit23, Rm
 # LDRBT,LDRT not supported yet

# load/store halfword and load signed byte

patterns # HSB addressing modes
  lhsb   is blob_25_27 = 0 & bit20 = 1 # blob_26_27 = 0 & bit25 = 0
  shsb   is blob_25_27 = 0 & bit20 = 0 # blob_26_27 = 0 & bit25 = 0

patterns
  LDRH   is lhsb & blob_5_6 = 1 # bit6 = 0 & bit5 = 1
  LDRSH  is lhsb & blob_5_6 = 3 # bit6 = 1 & bit5 = 1 
  LDRSB  is lhsb & blob_5_6 = 2 # bit6 = 1 & bit5 = 0
  STRH   is shsb & blob_5_6 = 1 # bit6 = 0 & bit5 = 1

patterns 
  lshsb  is LDRH | STRH | LDRSH | LDRSB

patterns 
  HSB_I     is bit24 = 1 & blob_21_22 = 2 & bit7 = 1 & bit4 = 1 # bit22 = 1 & bit21 = 0
  HSB_IPX   is bit24 = 1 & blob_21_22 = 3 & bit7 = 1 & bit4 = 1 # bit22 = 1 & bit21 = 1
  HSB_IOX   is bit24 = 0 & blob_21_22 = 2 & bit7 = 1 & bit4 = 1 # bit22 = 1 & bit21 = 0
  HSB_R     is bit24 = 1 & blob_21_22 = 0 & bit7 = 1 & bit4 = 1 # bit22 = 0 & bit21 = 0
  HSB_RPX   is bit24 = 1 & blob_21_22 = 1 & bit7 = 1 & bit4 = 1 # bit22 = 0 & bit21 = 1
  HSB_ROX   is bit24 = 0 & blob_21_22 = 0 & bit7 = 1 & bit4 = 1 # bit22 = 0 & bit21 = 0

  hsbimm    is HSB_I | HSB_IPX | HSB_IOX
  hsbreg    is HSB_R | HSB_RPX | HSB_ROX

constructors
  lshsb^hsbimm    cond, Rd, Rn, bit23, VRs, VRm # Rs = ImmHi, Rm = ImmLo
  lshsb^hsbreg    cond, Rd, Rn, bit23, Rm

# load/store multiple 

patterns
  [ IA IB DA DB ] is blob_23_24 = [ 1 3 0 2 ]

patterns
  lsmmode  is IA | IB | DA | DB

patterns 
  LDM      is blob_25_27 = 4 & bit22 = 0 & bit20 = 1 # LDM2 is similar to LDM with W=0 and LSM_reglist@[15:15]=0
  LDM3     is blob_25_27 = 4 & bit22 = 1 & bit20 = 1
  STM      is blob_25_27 = 4 & bit22 = 0 & bit20 = 0
  STM2     is blob_25_27 = 4 & bit22 = 1 & bit20 = 0

patterns
  lsm      is LDM | LDM3 | STM | STM2 

constructors
  lsm^lsmmode^W  cond, Rn, blob_0_15

# branches

patterns
  # bimm     is blob_25_27 = 5
  B        is blob_24_27 = 10 # bimm & bit24 = 0
  BL       is blob_24_27 = 11 # bimm & bit24 = 1
  BLX      is blob_25_27 = 5 & cond = 15 # alias BLX = BLNV

patterns
  breg     is blob_20_27 = 18 & blob_8_19 = 4095 # Rs = 15 & Rd = 15 & Rn = 15
  BLX2     is breg & blob_4_7 = 3
  BX       is breg & blob_4_7 = 1

patterns
  b        is B | BL
  bx       is BLX2 | BX

constructors
  b        cond, blob_0_23
  BLX      cond, blob_0_23, bit24
  bx       cond, Rm

# co-processor related instructions

patterns
  copro   is blob_24_27 = 14 # actual co-processor instructions
  coprols is copro & bit4 = 1 # co-processor loads and stores

patterns
  CDP  is copro & bit4 = 0 # co-processor data processing
  CDP2 is CDP & cond = 15 # alias CDP2 = CDPNE

  MCR  is coprols & bit20 = 0 # co-processor store
  MCR2 is MCR & cond = 15 # alias MCR2 = MCRNV

  MRC  is coprols & bit20 = 1 # co-processor load
  MRC2 is MRC & cond = 15 # alias MRC2 = MRCNV

constructors
  CDP       cond, VRs, blob_20_23, CRd, CRn, CRm, blob_5_7 # Rs = cp_num, blob_20_23 = opcode1, blob_5_7 = opcode2
  CDP2            VRs, blob_20_23, CRd, CRn, CRm, blob_5_7

  MCR       cond, VRs, blob_21_23, Rd, CRn, CRm, blob_5_7
  MCR2            VRs, blob_21_23, Rd, CRn, CRm, blob_5_7

  MRC       cond, VRs, blob_21_23, Rd, CRn, CRm, blob_5_7
  MRC2            VRs, blob_21_23, Rd, CRn, CRm, blob_5_7

#discard # the cond=NV forms are replaced by the *2 forms
#  CDP^"NV" 
#  MCR^"NV" 
#  MRC^"NV"

patterns
  STC    is blob_25_27 = 6 & bit20 = 0
  LDC    is blob_25_27 = 6 & bit20 = 1

  lscp   is STC | LDC

patterns 
  LSCPI  is bit24 = 1 & bit21 = 0 # immediate
  LSCPPI is bit24 = 1 & bit21 = 1 # pre index
  LSCPOI is bit24 = 0 & bit21 = 1 # post index
  LSCPNX is bit24 = 0 & bit21 = 0 # no index

  lscpaddrmode is LSCPI | LSCPPI | LSCPOI | LSCPNX

constructors
  lscp^lscpaddrmode   cond, VRs, CRd, Rn, blob_0_7, bit23 # Rs = cp_num

# swaps

patterns 
  swp   is blob_4_7 = 9 & Rs = 0

patterns
  SWP   is swp & blob_20_27 = 16
  SWPB  is swp & blob_20_27 = 20

patterns
  swaps is SWP | SWPB

constructors
  swaps cond, Rd, Rm, Rn

# move from gpr to status register and vice versa

patterns
  msr         is blob_20_21 = 2 & VRd = 15
  MSRI        is msr & blob_23_27 = 6
  MSRR        is msr & blob_23_27 = 2 & VRs = 0 & blob_4_7 = 0

patterns
  MRS         is blob_23_27 = 2 & blob_20_21 = 0 & VRn = 15 & blob_0_11 = 0

constructors
  MSRI        cond, VRn, bit22, blob_0_7, VRs # Rn = field_mask, Rs = rotate
  MSRR        cond, VRn, bit22, Rm # Rn = field_mask
  MRS         cond, Rd, bit22

# special instructions (each of the special instructions can't be really 
# grouped because, erm, well because it's a special instruction)

patterns
  BKPT        is cond = 14 & blob_20_27 = 18 & blob_4_7 = 7
  CLZ         is             blob_20_27 = 22 & blob_4_7 = 1 & VRn = 15 & VRs = 15
  SWI         is             blob_24_27 = 15

constructors
  BKPT        blob_8_19, VRm  # imm16 { imm16@[4:15] = blob_8_19, imm16@[0:3] = Rm }
  CLZ         cond, Rd, Rm
  SWI         cond, blob_0_23

