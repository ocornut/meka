/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                           Z80.c                         **/
/**                                                         **/
/** This file contains implementation for Z80 CPU. Don't    **/
/** forget to provide RdZ80(), WrZ80(), InZ80(), OutZ80(),  **/
/** LoopZ80(), and PatchZ80() functions to accomodate the   **/
/** emulated machine's architecture.                        **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2002                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
/* Heavily modified by Omar Cornut, for usage in MEKA.       */
/*************************************************************/


//*** MEKA-START ***

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Define to enable debugging features
// #define MEKA_Z80_DEBUGGER    // already defined in project

// Definitions below are related to various attempts to fix/tweak 
// the Z80 emulator.

// Define to reload IPeriod everytime. Else, IPeriod is added to current ICount (which is accurate)
// #define MEKA_Z80_IPERIOD_TRUNC

// Define to enable timing on NMI/Int acceptance
#define MEKA_Z80_INTERRUPTS_TAKES_TIME

// Define to log interruptions
// #define MEKA_Z80_DEBUG_LOG_INTERRUPTS

// Define to enable change in interrupt emulation (WIP)
#define MEKA_Z80_INT_NEW

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "shared.h"
#include "Z80.h"
#include "Tables.h"
#include "debugger.h"

#include "Z80Call.c"

//-----------------------------------------------------------------------------
// External declaration
//-----------------------------------------------------------------------------

extern void    Msg(int attr, const char *format, ...);

extern int              Debugger_Hook(Z80 *R);
extern int              Debugger_CPU_Exec_Traps[0x10000];
extern unsigned short   Debugger_Z80_PC_Last;
extern unsigned short   Debugger_Z80_PC_Log_Queue[256];
extern int              Debugger_Z80_PC_Log_Queue_Write;
extern int              Debugger_Z80_PC_Log_Queue_First;

//*** MEKA-END ***

/** System-Dependent Stuff ***********************************/
/** This is system-dependent code put here to speed things  **/
/** up. It has to stay inlined to be fast.                  **/
/*************************************************************/
//*** MEKA-START ***
// extern byte *Mem_Pages[];
// INLINE byte RdZ80(word A) { return(Mem_Pages[(A)>>13][(A)&0x1FFF]); }
//*** MEKA-END ***

#define S(Fl)        R->AF.B.l|=Fl
#define R(Fl)        R->AF.B.l&=~(Fl)
#define FLAGS(Rg,Fl) R->AF.B.l=Fl|ZSTable[Rg]

#define M_RLC(Rg)      \
  R->AF.B.l=Rg>>7;Rg=(Rg<<1)|R->AF.B.l;R->AF.B.l|=PZSTable[Rg]
#define M_RRC(Rg)      \
  R->AF.B.l=Rg&0x01;Rg=(Rg>>1)|(R->AF.B.l<<7);R->AF.B.l|=PZSTable[Rg]
#define M_RL(Rg)       \
  if(Rg&0x80)          \
  {                    \
    Rg=(Rg<<1)|(R->AF.B.l&C_FLAG); \
    R->AF.B.l=PZSTable[Rg]|C_FLAG; \
  }                    \
  else                 \
  {                    \
    Rg=(Rg<<1)|(R->AF.B.l&C_FLAG); \
    R->AF.B.l=PZSTable[Rg];        \
  }
#define M_RR(Rg)       \
  if(Rg&0x01)          \
  {                    \
    Rg=(Rg>>1)|(R->AF.B.l<<7);     \
    R->AF.B.l=PZSTable[Rg]|C_FLAG; \
  }                    \
  else                 \
  {                    \
    Rg=(Rg>>1)|(R->AF.B.l<<7);     \
    R->AF.B.l=PZSTable[Rg];        \
  }

#define M_SLA(Rg)      \
  R->AF.B.l=Rg>>7;Rg<<=1;R->AF.B.l|=PZSTable[Rg]
#define M_SRA(Rg)      \
  R->AF.B.l=Rg&C_FLAG;Rg=(Rg>>1)|(Rg&0x80);R->AF.B.l|=PZSTable[Rg]

#define M_SLL(Rg)      \
  R->AF.B.l=Rg>>7;Rg=(Rg<<1)|0x01;R->AF.B.l|=PZSTable[Rg]
#define M_SRL(Rg)      \
  R->AF.B.l=Rg&0x01;Rg>>=1;R->AF.B.l|=PZSTable[Rg]

#define M_BIT(Bit,Rg)  \
  R->AF.B.l=(R->AF.B.l&C_FLAG)|H_FLAG|PZSTable[Rg&(1<<Bit)]

#define M_SET(Bit,Rg) Rg|=1<<Bit
#define M_RES(Bit,Rg) Rg&=~(1<<Bit)

#define M_POP(Rg)      \
  R->Rg.B.l=RdZ80(R->SP.W++);R->Rg.B.h=RdZ80(R->SP.W++)
#define M_PUSH(Rg)     \
  WrZ80(--R->SP.W,R->Rg.B.h);WrZ80(--R->SP.W,R->Rg.B.l)

#define M_CALL         \
  J.B.l=RdZ80(R->PC.W++);J.B.h=RdZ80(R->PC.W++);         \
  WrZ80(--R->SP.W,R->PC.B.h);WrZ80(--R->SP.W,R->PC.B.l); \
  R->PC.W=J.W

#define M_JP  J.B.l=RdZ80(R->PC.W++);J.B.h=RdZ80(R->PC.W);R->PC.W=J.W
#define M_JR  R->PC.W+=(offset)RdZ80(R->PC.W)+1
#define M_RET R->PC.B.l=RdZ80(R->SP.W++);R->PC.B.h=RdZ80(R->SP.W++)

#define M_RST(Ad)      \
  WrZ80(--R->SP.W,R->PC.B.h);WrZ80(--R->SP.W,R->PC.B.l);R->PC.W=Ad

#define M_LDWORD(Rg)   \
  R->Rg.B.l=RdZ80(R->PC.W++);R->Rg.B.h=RdZ80(R->PC.W++)

#define M_ADD(Rg)      \
  J.W=R->AF.B.h+Rg;     \
  R->AF.B.l=            \
    (~(R->AF.B.h^Rg)&(Rg^J.B.l)&0x80? V_FLAG:0)| \
    J.B.h|ZSTable[J.B.l]|                        \
    ((R->AF.B.h^Rg^J.B.l)&H_FLAG);               \
  R->AF.B.h=J.B.l

#define M_SUB(Rg)      \
  J.W=R->AF.B.h-Rg;    \
  R->AF.B.l=           \
    ((R->AF.B.h^Rg)&(R->AF.B.h^J.B.l)&0x80? V_FLAG:0)| \
    N_FLAG|-J.B.h|ZSTable[J.B.l]|                      \
    ((R->AF.B.h^Rg^J.B.l)&H_FLAG);                     \
  R->AF.B.h=J.B.l

#define M_ADC(Rg)      \
  J.W=R->AF.B.h+Rg+(R->AF.B.l&C_FLAG); \
  R->AF.B.l=                           \
    (~(R->AF.B.h^Rg)&(Rg^J.B.l)&0x80? V_FLAG:0)| \
    J.B.h|ZSTable[J.B.l]|              \
    ((R->AF.B.h^Rg^J.B.l)&H_FLAG);     \
  R->AF.B.h=J.B.l

#define M_SBC(Rg)      \
  J.W=R->AF.B.h-Rg-(R->AF.B.l&C_FLAG); \
  R->AF.B.l=                           \
    ((R->AF.B.h^Rg)&(R->AF.B.h^J.B.l)&0x80? V_FLAG:0)| \
    N_FLAG|-J.B.h|ZSTable[J.B.l]|      \
    ((R->AF.B.h^Rg^J.B.l)&H_FLAG);     \
  R->AF.B.h=J.B.l

#define M_CP(Rg)       \
  J.W=R->AF.B.h-Rg;    \
  R->AF.B.l=           \
    ((R->AF.B.h^Rg)&(R->AF.B.h^J.B.l)&0x80? V_FLAG:0)| \
    N_FLAG|-J.B.h|ZSTable[J.B.l]|                      \
    ((R->AF.B.h^Rg^J.B.l)&H_FLAG)

#define M_AND(Rg) R->AF.B.h&=Rg;R->AF.B.l=H_FLAG|PZSTable[R->AF.B.h]
#define M_OR(Rg)  R->AF.B.h|=Rg;R->AF.B.l=PZSTable[R->AF.B.h]
#define M_XOR(Rg) R->AF.B.h^=Rg;R->AF.B.l=PZSTable[R->AF.B.h]

#define M_IN(Rg)        \
  Rg=InZ80(R->BC.B.l);  \
  R->AF.B.l=PZSTable[Rg]|(R->AF.B.l&C_FLAG)

#define M_INC(Rg)       \
  Rg++;                 \
  R->AF.B.l=            \
    (R->AF.B.l&C_FLAG)|ZSTable[Rg]|           \
    (Rg==0x80? V_FLAG:0)|(Rg&0x0F? 0:H_FLAG)

#define M_DEC(Rg)       \
  Rg--;                 \
  R->AF.B.l=            \
    N_FLAG|(R->AF.B.l&C_FLAG)|ZSTable[Rg]| \
    (Rg==0x7F? V_FLAG:0)|((Rg&0x0F)==0x0F? H_FLAG:0)

#define M_ADDW(Rg1,Rg2) \
  J.W=(R->Rg1.W+R->Rg2.W)&0xFFFF;                        \
  R->AF.B.l=                                             \
    (R->AF.B.l&~(H_FLAG|N_FLAG|C_FLAG))|                 \
    ((R->Rg1.W^R->Rg2.W^J.W)&0x1000? H_FLAG:0)|          \
    (((long)R->Rg1.W+(long)R->Rg2.W)&0x10000? C_FLAG:0); \
  R->Rg1.W=J.W

#define M_ADCW(Rg)      \
  I=R->AF.B.l&C_FLAG;J.W=(R->HL.W+R->Rg.W+I)&0xFFFF;           \
  R->AF.B.l=                                                   \
    (((long)R->HL.W+(long)R->Rg.W+(long)I)&0x10000? C_FLAG:0)| \
    (~(R->HL.W^R->Rg.W)&(R->Rg.W^J.W)&0x8000? V_FLAG:0)|       \
    ((R->HL.W^R->Rg.W^J.W)&0x1000? H_FLAG:0)|                  \
    (J.W? 0:Z_FLAG)|(J.B.h&S_FLAG);                            \
  R->HL.W=J.W

#define M_SBCW(Rg)      \
  I=R->AF.B.l&C_FLAG;J.W=(R->HL.W-R->Rg.W-I)&0xFFFF;           \
  R->AF.B.l=                                                   \
    N_FLAG|                                                    \
    (((long)R->HL.W-(long)R->Rg.W-(long)I)&0x10000? C_FLAG:0)| \
    ((R->HL.W^R->Rg.W)&(R->HL.W^J.W)&0x8000? V_FLAG:0)|        \
    ((R->HL.W^R->Rg.W^J.W)&0x1000? H_FLAG:0)|                  \
    (J.W? 0:Z_FLAG)|(J.B.h&S_FLAG);                            \
  R->HL.W=J.W

enum Codes
{
  NOP, LD_BC_WORD, LD_xBC_A, INC_BC, INC_B, DEC_B, LD_B_BYTE, RLCA,     // 00-07
  EX_AF_AF, ADD_HL_BC, LD_A_xBC, DEC_BC, INC_C, DEC_C, LD_C_BYTE, RRCA, // 08-0F
  DJNZ, LD_DE_WORD, LD_xDE_A, INC_DE, INC_D, DEC_D, LD_D_BYTE, RLA,     // 10-17
  JR, ADD_HL_DE, LD_A_xDE, DEC_DE, INC_E, DEC_E, LD_E_BYTE, RRA,        // 18-1F
  JR_NZ, LD_HL_WORD, LD_xWORD_HL, INC_HL, INC_H, DEC_H, LD_H_BYTE, DAA, // 20-27
  JR_Z, ADD_HL_HL, LD_HL_xWORD, DEC_HL, INC_L, DEC_L, LD_L_BYTE, CPL,   // 28-2F
  JR_NC, LD_SP_WORD, LD_xWORD_A, INC_SP, INC_xHL, DEC_xHL, LD_xHL_BYTE, SCF, // 30-37
  JR_C, ADD_HL_SP, LD_A_xWORD, DEC_SP, INC_A, DEC_A, LD_A_BYTE, CCF,    // 38-3F
  LD_B_B, LD_B_C, LD_B_D, LD_B_E, LD_B_H, LD_B_L, LD_B_xHL, LD_B_A,     // 40-47
  LD_C_B, LD_C_C, LD_C_D, LD_C_E, LD_C_H, LD_C_L, LD_C_xHL, LD_C_A,     // 48-4F
  LD_D_B, LD_D_C, LD_D_D, LD_D_E, LD_D_H, LD_D_L, LD_D_xHL, LD_D_A,     // 50-57
  LD_E_B, LD_E_C, LD_E_D, LD_E_E, LD_E_H, LD_E_L, LD_E_xHL, LD_E_A,     // 58-5F
  LD_H_B, LD_H_C, LD_H_D, LD_H_E, LD_H_H, LD_H_L, LD_H_xHL, LD_H_A,     // 60-67
  LD_L_B, LD_L_C, LD_L_D, LD_L_E, LD_L_H, LD_L_L, LD_L_xHL, LD_L_A,     // 68-6F
  LD_xHL_B, LD_xHL_C, LD_xHL_D, LD_xHL_E, LD_xHL_H, LD_xHL_L, HALT, LD_xHL_A, // 70-77
  LD_A_B, LD_A_C, LD_A_D, LD_A_E, LD_A_H, LD_A_L, LD_A_xHL, LD_A_A,     // 78-7F
  ADD_B, ADD_C, ADD_D, ADD_E, ADD_H, ADD_L, ADD_xHL, ADD_A,             // 80-87
  ADC_B, ADC_C, ADC_D, ADC_E, ADC_H, ADC_L, ADC_xHL, ADC_A,             // 88-8F
  SUB_B, SUB_C, SUB_D, SUB_E, SUB_H, SUB_L, SUB_xHL, SUB_A,             // 90-97
  SBC_B, SBC_C, SBC_D, SBC_E, SBC_H, SBC_L, SBC_xHL, SBC_A,             // 98-9F
  AND_B, AND_C, AND_D, AND_E, AND_H, AND_L, AND_xHL, AND_A,             // A0-A7
  XOR_B, XOR_C, XOR_D, XOR_E, XOR_H, XOR_L, XOR_xHL, XOR_A,             // A8-AF
  OR_B, OR_C, OR_D, OR_E, OR_H, OR_L, OR_xHL, OR_A,                     // B0-B7
  CP_B, CP_C, CP_D, CP_E, CP_H, CP_L, CP_xHL, CP_A,                     // B8-BF
  RET_NZ, POP_BC, JP_NZ, JP, CALL_NZ, PUSH_BC, ADD_BYTE, RST00,         // C0-C7
  RET_Z, RET, JP_Z, PFX_CB, CALL_Z, CALL, ADC_BYTE, RST08,              // C8-CF
  RET_NC, POP_DE, JP_NC, OUTA, CALL_NC, PUSH_DE, SUB_BYTE, RST10,       // D0-D7
  RET_C, EXX, JP_C, INA, CALL_C, PFX_DD, SBC_BYTE, RST18,               // D8-DF
  RET_PO, POP_HL, JP_PO, EX_HL_xSP, CALL_PO, PUSH_HL, AND_BYTE, RST20,  // E0-E7
  RET_PE, LD_PC_HL, JP_PE, EX_DE_HL, CALL_PE, PFX_ED, XOR_BYTE, RST28,  // E8-EF
  RET_P, POP_AF, JP_P, DI, CALL_P, PUSH_AF, OR_BYTE, RST30,             // F0-F7
  RET_M, LD_SP_HL, JP_M, EI, CALL_M, PFX_FD, CP_BYTE, RST38             // F8-FF
};

enum CodesCB
{
  RLC_B, RLC_C, RLC_D, RLC_E, RLC_H, RLC_L, RLC_xHL, RLC_A,             // 00-07
  RRC_B, RRC_C, RRC_D, RRC_E, RRC_H, RRC_L, RRC_xHL, RRC_A,             // 08-0F
  RL_B, RL_C, RL_D, RL_E, RL_H, RL_L, RL_xHL, RL_A,                     // 10-17
  RR_B, RR_C, RR_D, RR_E, RR_H, RR_L, RR_xHL, RR_A,                     // 18-1F
  SLA_B, SLA_C, SLA_D, SLA_E, SLA_H, SLA_L, SLA_xHL, SLA_A,             // 20-27
  SRA_B, SRA_C, SRA_D, SRA_E, SRA_H, SRA_L, SRA_xHL, SRA_A,             // 28-2F
  SLL_B, SLL_C, SLL_D, SLL_E, SLL_H, SLL_L, SLL_xHL, SLL_A,             // 30-37
  SRL_B, SRL_C, SRL_D, SRL_E, SRL_H, SRL_L, SRL_xHL, SRL_A,             // 38-3F
  BIT0_B, BIT0_C, BIT0_D, BIT0_E, BIT0_H, BIT0_L, BIT0_xHL, BIT0_A,     // 40-47
  BIT1_B, BIT1_C, BIT1_D, BIT1_E, BIT1_H, BIT1_L, BIT1_xHL, BIT1_A,     // 48-4F
  BIT2_B, BIT2_C, BIT2_D, BIT2_E, BIT2_H, BIT2_L, BIT2_xHL, BIT2_A,     // 50-57
  BIT3_B, BIT3_C, BIT3_D, BIT3_E, BIT3_H, BIT3_L, BIT3_xHL, BIT3_A,     // 58-5F
  BIT4_B, BIT4_C, BIT4_D, BIT4_E, BIT4_H, BIT4_L, BIT4_xHL, BIT4_A,     // 60-67
  BIT5_B, BIT5_C, BIT5_D, BIT5_E, BIT5_H, BIT5_L, BIT5_xHL, BIT5_A,     // 68-6F
  BIT6_B, BIT6_C, BIT6_D, BIT6_E, BIT6_H, BIT6_L, BIT6_xHL, BIT6_A,     // 70-77
  BIT7_B, BIT7_C, BIT7_D, BIT7_E, BIT7_H, BIT7_L, BIT7_xHL, BIT7_A,     // 78-7F
  RES0_B, RES0_C, RES0_D, RES0_E, RES0_H, RES0_L, RES0_xHL, RES0_A,     // 80-87
  RES1_B, RES1_C, RES1_D, RES1_E, RES1_H, RES1_L, RES1_xHL, RES1_A,     // 88-8F
  RES2_B, RES2_C, RES2_D, RES2_E, RES2_H, RES2_L, RES2_xHL, RES2_A,     // 90-97
  RES3_B, RES3_C, RES3_D, RES3_E, RES3_H, RES3_L, RES3_xHL, RES3_A,     // 98-9F
  RES4_B, RES4_C, RES4_D, RES4_E, RES4_H, RES4_L, RES4_xHL, RES4_A,     // A0-A7
  RES5_B, RES5_C, RES5_D, RES5_E, RES5_H, RES5_L, RES5_xHL, RES5_A,     // A8-AF
  RES6_B, RES6_C, RES6_D, RES6_E, RES6_H, RES6_L, RES6_xHL, RES6_A,     // B0-B7
  RES7_B, RES7_C, RES7_D, RES7_E, RES7_H, RES7_L, RES7_xHL, RES7_A,     // B8-BF
  SET0_B, SET0_C, SET0_D, SET0_E, SET0_H, SET0_L, SET0_xHL, SET0_A,     // C0-C7
  SET1_B, SET1_C, SET1_D, SET1_E, SET1_H, SET1_L, SET1_xHL, SET1_A,     // C8-CF
  SET2_B, SET2_C, SET2_D, SET2_E, SET2_H, SET2_L, SET2_xHL, SET2_A,     // D0-D7
  SET3_B, SET3_C, SET3_D, SET3_E, SET3_H, SET3_L, SET3_xHL, SET3_A,     // D8-DF
  SET4_B, SET4_C, SET4_D, SET4_E, SET4_H, SET4_L, SET4_xHL, SET4_A,     // E0-E7
  SET5_B, SET5_C, SET5_D, SET5_E, SET5_H, SET5_L, SET5_xHL, SET5_A,     // E8-EF
  SET6_B, SET6_C, SET6_D, SET6_E, SET6_H, SET6_L, SET6_xHL, SET6_A,     // F0-F7
  SET7_B, SET7_C, SET7_D, SET7_E, SET7_H, SET7_L, SET7_xHL, SET7_A      // F8-FF
};

enum CodesED
{
  DB_00, DB_01, DB_02, DB_03, DB_04, DB_05, DB_06, DB_07,               // 00-07
  DB_08, DB_09, DB_0A, DB_0B, DB_0C, DB_0D, DB_0E, DB_0F,               // 08-0F
  DB_10, DB_11, DB_12, DB_13, DB_14, DB_15, DB_16, DB_17,               // 10-17
  DB_18, DB_19, DB_1A, DB_1B, DB_1C, DB_1D, DB_1E, DB_1F,               // 18-1F
  DB_20, DB_21, DB_22, DB_23, DB_24, DB_25, DB_26, DB_27,               // 20-27
  DB_28, DB_29, DB_2A, DB_2B, DB_2C, DB_2D, DB_2E, DB_2F,               // 28-2F
  DB_30, DB_31, DB_32, DB_33, DB_34, DB_35, DB_36, DB_37,               // 30-37
  DB_38, DB_39, DB_3A, DB_3B, DB_3C, DB_3D, DB_3E, DB_3F,               // 38-3F
  IN_B_xC, OUT_xC_B, SBC_HL_BC, LD_xWORDe_BC, NEG, RETN, IM_0, LD_I_A,    // 40-47
  IN_C_xC, OUT_xC_C, ADC_HL_BC, LD_BC_xWORDe, DB_4C, RETI, DB_4E, LD_R_A, // 48-4F
  IN_D_xC, OUT_xC_D, SBC_HL_DE, LD_xWORDe_DE, DB_54, DB_55, IM_1, LD_A_I, // 50-57
  IN_E_xC, OUT_xC_E, ADC_HL_DE, LD_DE_xWORDe, DB_5C, DB_5D, IM_2, LD_A_R, // 58-5F
  IN_H_xC, OUT_xC_H, SBC_HL_HL, LD_xWORDe_HL, DB_64, DB_65, DB_66, RRD,   // 60-67
  IN_L_xC, OUT_xC_L, ADC_HL_HL, LD_HL_xWORDe, DB_6C, DB_6D, DB_6E, RLD,   // 68-6F
  IN_F_xC, OUT_xC_0, SBC_HL_SP, LD_xWORDe_SP, DB_74, DB_75, DB_76, DB_77, // 70-77
  IN_A_xC, OUT_xC_A, ADC_HL_SP, LD_SP_xWORDe, DB_7C, DB_7D, DB_7E, DB_7F, // 78-7F
  DB_80, DB_81, DB_82, DB_83, DB_84, DB_85, DB_86, DB_87,               // 80-87
  DB_88, DB_89, DB_8A, DB_8B, DB_8C, DB_8D, DB_8E, DB_8F,               // 88-8F
  DB_90, DB_91, DB_92, DB_93, DB_94, DB_95, DB_96, DB_97,               // 90-97
  DB_98, DB_99, DB_9A, DB_9B, DB_9C, DB_9D, DB_9E, DB_9F,               // 98-9F
  LDI, CPI, INI, OUTI, DB_A4, DB_A5, DB_A6, DB_A7,                      // A0-A7
  LDD, CPD, IND, OUTD, DB_AC, DB_AD, DB_AE, DB_AF,                      // A8-AF
  LDIR, CPIR, INIR, OTIR, DB_B4, DB_B5, DB_B6, DB_B7,                   // B0-B7
  LDDR, CPDR, INDR, OTDR, DB_BC, DB_BD, DB_BE, DB_BF,                   // B8-BF
  DB_C0, DB_C1, DB_C2, DB_C3, DB_C4, DB_C5, DB_C6, DB_C7,               // C0-C7
  DB_C8, DB_C9, DB_CA, DB_CB, DB_CC, DB_CD, DB_CE, DB_CF,               // C8-CF
  DB_D0, DB_D1, DB_D2, DB_D3, DB_D4, DB_D5, DB_D6, DB_D7,               // D0-D7
  DB_D8, DB_D9, DB_DA, DB_DB, DB_DC, DB_DD, DB_DE, DB_DF,               // D8-DF
  DB_E0, DB_E1, DB_E2, DB_E3, DB_E4, DB_E5, DB_E6, DB_E7,               // E0-E7
  DB_E8, DB_E9, DB_EA, DB_EB, DB_EC, DB_ED, DB_EE, DB_EF,               // E8-EF
  DB_F0, DB_F1, DB_F2, DB_F3, DB_F4, DB_F5, DB_F6, DB_F7,               // F0-F7
  DB_F8, DB_F9, DB_FA, DB_FB, DB_FC, DB_FD, DB_FE, DB_FF                // F8-FF
};

static void CodesCB(register Z80 *R)
{
  register byte I;

  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesCB[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_CB][I]++;
  #endif
  switch (I)
  {
#include "CodesCB.h"
    default:
      if (R->TrapBadOps)
        printf
        (
          "[Z80 %lX] Unrecognized instruction: CB %02X at PC=%04X\n",
          (long)(R->User),RdZ80(R->PC.W-1),R->PC.W-2
        );
  }
}

static void CodesDDCB(register Z80 *R)
{
  register pair J;
  register byte I;

#define XX IX
  J.W = R->XX.W + (offset)RdZ80(R->PC.W++);
  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesXXCB[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_DDCB][I]++;
  #endif
  switch (I)
  {
#include "CodesXCB.h"
    default:
      if (R->TrapBadOps)
        printf
        (
          "[Z80 %lX] Unrecognized instruction: DD CB %02X %02X at PC=%04X\n",
          (long)(R->User),RdZ80(R->PC.W-2),RdZ80(R->PC.W-1),R->PC.W-4
        );
  }
#undef XX
}

static void CodesFDCB(register Z80 *R)
{
  register pair J;
  register byte I;

#define XX IY
  J.W = R->XX.W + (offset)RdZ80(R->PC.W++);
  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesXXCB[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_FDCB][I]++;
  #endif
  switch (I)
  {
#include "CodesXCB.h"
    default:
      if (R->TrapBadOps)
        printf
        (
          "[Z80 %lX] Unrecognized instruction: FD CB %02X %02X at PC=%04X\n",
          (long)R->User,RdZ80(R->PC.W-2),RdZ80(R->PC.W-1),R->PC.W-4
        );
  }
#undef XX
}

static void CodesED(register Z80 *R)
{
  register byte I;
  register pair J;

  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesED[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_ED][I]++;
  #endif
  switch (I)
  {
#include "CodesED.h"
    /*
    ** Note: ED cannot be repeated
    case PFX_ED:
      R->PC.W--;
      Msg(0, "%04x: Repeated ED", R->PC.W);
      break;
    */
    default:
      if (R->TrapBadOps)
        printf
        (
          "[Z80 %lX] Unrecognized instruction: ED %02X at PC=%04X\n",
          (long)R->User,RdZ80(R->PC.W-1),R->PC.W-2
        );
  }
}

static void CodesDD(register Z80 *R)
{
  register byte I;
  register pair J;

#define XX IX
  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesXX[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_DD][I]++;
  #endif
  switch (I)
  {
#include "CodesXX.h"
    case PFX_FD:
    case PFX_DD:
      R->PC.W--;
      // Msg(0, "%04x: CodesDD() Repeated %02x", R->PC.W, I);
      break;
    case PFX_CB:
      CodesDDCB(R); break;
    default:
      if (R->TrapBadOps)
        printf
        (
          "[Z80 %lX] Unrecognized instruction: DD %02X at PC=%04X\n",
          (long)R->User,RdZ80(R->PC.W-1),R->PC.W-2
        );
  }
#undef XX
}

static void CodesFD(register Z80 *R)
{
  register byte I;
  register pair J;

#define XX IY
  I = RdZ80(R->PC.W++);
  R->ICount -= CyclesXX[I];
  #ifdef MEKA_Z80_OPCODES_USAGE
    Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_FD][I]++;
  #endif
  switch (I)
  {
#include "CodesXX.h"
    case PFX_FD:
    case PFX_DD:
      R->PC.W--;
      // Msg(0, "%04x: CodesFD() Repeated %02x", R->PC.W, I);
      break;
    case PFX_CB:
      CodesFDCB(R);break;
    default:
        printf
        (
          "Unrecognized instruction: FD %02X at PC=%04X\n",
          RdZ80(R->PC.W-1),R->PC.W-2
        );
  }
#undef XX
}

/** ResetZ80() ***********************************************/
/** This function can be used to reset the register struct  **/
/** before starting execution with Z80(). It sets the       **/
/** registers to their supposed initial values.             **/
/*************************************************************/
void    ResetZ80(Z80 *R)
{
  R->PC.W     = 0x0000;
  R->SP.W     = 0xDFF0;
  R->AF.W     = 0x0000;
  R->BC.W     = 0x0000;
  R->DE.W     = 0x0000;
  R->HL.W     = 0x0000;
  R->AF1.W    = 0x0000;
  R->BC1.W    = 0x0000;
  R->DE1.W    = 0x0000;
  R->HL1.W    = 0x0000;
  R->IX.W     = 0x0000;
  R->IY.W     = 0x0000;
  R->I        = 0x00;
  R->IFF      = 0x00;
  R->ICount   = R->IPeriod;
  R->IRequest = INT_NONE;
}

/** IntZ80() *************************************************/
/** This function will generate interrupt of given vector.  **/
/*************************************************************/
void IntZ80(Z80 *R,word Vector)
{
  if((R->IFF&IFF_1)||(Vector==INT_NMI))
  {
    /* If HALTed, take CPU off HALT instruction */
    if(R->IFF&IFF_HALT) { R->PC.W++;R->IFF&=~IFF_HALT; }

    /* Save PC on stack */
    M_PUSH(PC);

    /* Automatically reset IRequest if needed */
    //*** MEKA-START : Commented out, Reading VDP Status does that
    // if(R->IAutoReset&&(Vector==R->IRequest)) R->IRequest=INT_NONE;
    //*** MEKA_STOP

    /* If it is NMI... */
    if(Vector==INT_NMI)
    {
      /* Copy IFF1 to IFF2 */
      if(R->IFF&IFF_1) R->IFF|=IFF_2; else R->IFF&=~IFF_2;
      /* Clear IFF1 */
      R->IFF&=~(IFF_1|IFF_EI);
      /* Jump to hardwired NMI vector */
      R->PC.W=0x0066;
      /* Takes time ? */
      #ifdef MEKA_Z80_INTERRUPTS_TAKES_TIME
         R->ICount -= 11;
      #endif
      /* Done */
      return;
    }

    /* Further interrupts off */
    R->IFF&=~(IFF_1|IFF_2|IFF_EI);

    /* If in IM2 mode... */
    if(R->IFF&IFF_IM2)
    {
      /* Make up the vector address */
      Vector=(Vector&0xFF)|((word)(R->I)<<8);
      /* Read the vector */
      R->PC.B.l=RdZ80(Vector++);
      R->PC.B.h=RdZ80(Vector);
      /* Takes time ? */
      #ifdef MEKA_Z80_INTERRUPTS_TAKES_TIME
         R->ICount -= 19;
      #endif
      /* Done */
      return;
    }

    /* If in IM1 mode, just jump to hardwired IRQ vector */
    if(R->IFF&IFF_IM1)
    {
      R->PC.W=0x0038;
      /* Takes time ? */
      #ifdef MEKA_Z80_INTERRUPTS_TAKES_TIME
         R->ICount -= 13;
      #endif
      return;
    }

    /* If in IM0 mode... */

    /* Takes time ? */
    #ifdef MEKA_Z80_INTERRUPTS_TAKES_TIME
       R->ICount -= 13;
    #endif

    /* Jump to a vector */
    switch(Vector)
    {
      case INT_RST00: R->PC.W=0x0000;break;
      case INT_RST08: R->PC.W=0x0008;break;
      case INT_RST10: R->PC.W=0x0010;break;
      case INT_RST18: R->PC.W=0x0018;break;
      case INT_RST20: R->PC.W=0x0020;break;
      case INT_RST28: R->PC.W=0x0028;break;
      case INT_RST30: R->PC.W=0x0030;break;
      case INT_RST38: R->PC.W=0x0038;break;
    }
  }
}

/** RunZ80() *************************************************/
/** This function will run Z80 code until an LoopZ80() call **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
word    RunZ80(Z80 *R)
{
  register byte I;
  register pair J;

  for (;;)
  {
    I = RdZ80 (R->PC.W ++);
    R->ICount -= Cycles[I];
    #ifdef MEKA_Z80_OPCODES_USAGE
       Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_NONE][I]++;
    #endif
    switch (I)
    {
#include "Codes.h"
      case PFX_CB: CodesCB(R); break;
      case PFX_ED: CodesED(R); break;
      case PFX_FD: CodesFD(R); break;
      case PFX_DD: CodesDD(R); break;
    }

    /* If cycle counter expired... */
    if (R->ICount <= 0)
    {
      #ifdef MEKA_Z80_DEBUG_LOG_INTERRUPTS
        {
        static int c = 0;
        printf("%04X: %-6d\n", R->PC.W, c++);
        }
      #endif

      /* If we have come after EI, get address from IRequest */
      /* Otherwise, get it from the loop handler             */
      if (R->IFF & IFF_EI)
      {
        R->IFF=(R->IFF&~IFF_EI)|IFF_1; /* Done with AfterEI state */
        R->ICount += R->IBackup - 1;   /* Restore the ICount       */

        /* Call periodic handler or set pending IRQ */
        if (R->ICount > 0) J.W = R->IRequest;
        else
        {
          J.W = LoopZ80(/*R*/);                 /* Call periodic handler    */
          #ifdef MEKA_Z80_IPERIOD_TRUNC
             R->ICount=R->IPeriod;              /* Reset the cycle counter  */
          #else
             R->ICount+=R->IPeriod;             /* Add up to cycle counter  */
          #endif
          if (J.W==INT_NONE) J.W=R->IRequest;   /* Pending IRQ */
        }
      }
      else
      {
        J.W = LoopZ80(/*R*/);                   /* Call periodic handler    */
        #ifdef MEKA_Z80_IPERIOD_TRUNC
           R->ICount=R->IPeriod;                /* Reset the cycle counter  */
        #else
           R->ICount+=R->IPeriod;               /* Add up to cycle counter  */
        #endif
        if (J.W==INT_NONE) J.W=R->IRequest;     /* Pending int-rupt */
      }

      if (J.W == INT_QUIT) return (R->PC.W);    /* Exit if INT_QUIT */
      if (J.W != INT_NONE) IntZ80 (R,J.W);      /* Int-pt if needed */
    }
  }

  /* Execution stopped */
  // Note: unreachable code
  return (R->PC.W);
}

word    RunZ80_Debugging(Z80 *R)
{
    register byte I;
    register pair J;
    int icount_before_instruction;

    for (;;)
    {
        // This is block of code that gets added in RunZ80_Debugging() compared to standard RunZ80()

        // Log PC
        Debugger_Z80_PC_Last = R->PC.W;
        Debugger_Z80_PC_Log_Queue[Debugger_Z80_PC_Log_Queue_Write] = R->PC.W;
        Debugger_Z80_PC_Log_Queue_Write = (Debugger_Z80_PC_Log_Queue_Write + 1) & 255;
        if (Debugger_Z80_PC_Log_Queue_Write == Debugger_Z80_PC_Log_Queue_First)
            Debugger_Z80_PC_Log_Queue_First = (Debugger_Z80_PC_Log_Queue_First + 1) & 255;
        // Debugger_Z80_PC_Log_Queue_Add(R->PC.W);

        // Turn tracing on when reached trap address
        if (R->PC.W == R->Trap)
            R->Trace = 1;

        // Call single-step debugger, exit if requested
        if (R->Trace || Debugger_CPU_Exec_Traps[R->PC.W])
            if (!Debugger_Hook (R))
                return (R->PC.W);

        // Save ICount before instruction
        icount_before_instruction = R->ICount;

        // Execute instruction
        I = RdZ80 (R->PC.W ++);
        R->ICount -= Cycles[I];
        #ifdef MEKA_Z80_OPCODES_USAGE
            Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_NONE][I]++;
        #endif
        switch (I)
        {
        #include "Codes.h"
      case PFX_CB: CodesCB(R); break;
      case PFX_ED: CodesED(R); break;
      case PFX_FD: CodesFD(R); break;
      case PFX_DD: CodesDD(R); break;
        }

        // Increment debugger cycle counter
        Debugger.cycle_counter += (icount_before_instruction - R->ICount);

        // Reset stepping flag
        Debugger.stepping = FALSE;

        /* If cycle counter expired... */
        if (R->ICount <= 0)
        {
            /* If we have come after EI, get address from IRequest */
            /* Otherwise, get it from the loop handler             */
            if (R->IFF & IFF_EI)
            {
                R->IFF=(R->IFF&~IFF_EI)|IFF_1; /* Done with AfterEI state */
                R->ICount += R->IBackup - 1;   /* Restore the ICount       */

                /* Call periodic handler or set pending IRQ */
                if (R->ICount>0) J.W=R->IRequest;
                else
                {
                    J.W = LoopZ80(/*R*/);                 /* Call periodic handler    */
                    #ifdef MEKA_Z80_IPERIOD_TRUNC
                        R->ICount=R->IPeriod;             /* Reset the cycle counter  */
                    #else
                        R->ICount+=R->IPeriod;            /* Add up to cycle counter  */
                    #endif
                    if (J.W==INT_NONE) J.W=R->IRequest;   /* Pending IRQ */
                }
            }
            else
            {
                J.W = LoopZ80(/*R*/);                   /* Call periodic handler    */
                #ifdef MEKA_Z80_IPERIOD_TRUNC
                    R->ICount=R->IPeriod;               /* Reset the cycle counter  */
                #else
                    R->ICount+=R->IPeriod;              /* Add up to cycle counter  */
                #endif
                if (J.W==INT_NONE) J.W=R->IRequest;     /* Pending int-rupt */
            }

            if (J.W == INT_QUIT) return (R->PC.W);    /* Exit if INT_QUIT */
            if (J.W != INT_NONE) IntZ80 (R,J.W);      /* Int-pt if needed */
        }
    }

    /* Execution stopped */
   // Note: unreachable code
    return (R->PC.W);
}

