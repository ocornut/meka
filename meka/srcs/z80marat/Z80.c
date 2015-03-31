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
/* Modified for usage in MEKA. */
/*************************************************************/


//*** MEKA-START ***

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Define to enable debugging features
// #define MEKA_Z80_DEBUGGER    // already defined in project

// Define to enable timing on NMI/Int acceptance
#define MEKA_Z80_INTERRUPTS_TAKES_TIME

// Define to enable change in interrupt emulation (WIP)
//#define MEKA_Z80_INT_NEW

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "shared.h"
#include "Z80.h"
#include "Tables.h"
#include "debugger.h"

#include "Z80Call.c"

void    (*WrZ80)(register word Addr, register byte Value);
byte    (*RdZ80)(register word Addr);
void    (*WrZ80_NoHook)(register word Addr, register byte Value);
byte    (*RdZ80_NoHook)(register word Addr);
void    (*OutZ80)(register word Port, register byte Value);
byte    (*InZ80)(register word Port);
void    (*OutZ80_NoHook)(register word Port, register byte Value);
byte    (*InZ80_NoHook)(register word Port);
word    (*LoopZ80)(/*register Z80 *R*/ void);

//-----------------------------------------------------------------------------
// External declaration
//-----------------------------------------------------------------------------

extern void    Msg(int attr, const char *format, ...);

extern int     Debugger_Hook(Z80 *R);

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

#include "Z80OpcodeEnums.h"

static void CodesCB(register Z80 *R)
{
  register byte I;

  I = RdZ80(R->PC.W++);
  R->R++;
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
  R->R++;
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
  R->R++;
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
  R->R++;
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
  R->R        = 0x00;
  R->R7       = 0x00;
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
      //if(R->IFF&IFF_1) R->IFF|=IFF_2; else R->IFF&=~IFF_2;
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
          R->ICount+=R->IPeriod;                /* Add up to cycle counter  */
          if (J.W==INT_NONE) J.W=R->IRequest;   /* Pending IRQ */
        }
      }
      else
      {
        J.W = LoopZ80(/*R*/);                   /* Call periodic handler    */
        R->ICount+=R->IPeriod;                  /* Add up to cycle counter  */
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

#ifdef MEKA_Z80_DEBUGGER
		Debugger.pc_last = R->PC.W;

        // Log PC execution address
		// This allow us to have backtracking disassembly
		if (Debugger.pc_exec_points[R->PC.W] == 0)
			Debugger.pc_exec_points[R->PC.W] = 0xff;
#endif // MEKA_Z80_DEBUGGER

        // Turn tracing on when reached trap address
        if (R->PC.W == R->Trap)
            R->Trace = 1;

        // Call single-step debugger, exit if requested
#ifdef MEKA_Z80_DEBUGGER
        if (R->Trace || Debugger.cpu_exec_traps[R->PC.W])
            if (!Debugger_Hook(R))
                return (R->PC.W);

		// Trace
		if (size_t log_size = Debugger.pc_detail_log_data.size())
		{
			const size_t head = Debugger.pc_detail_log_head;
			const size_t prev = head == 0 ? log_size - 1 : head - 1;
			if (Debugger.pc_detail_log_data[prev].pc != R->PC.W)
			{
				t_debugger_exec_log_entry* e = &Debugger.pc_detail_log_data[head];
				e->af = R->AF.W;
				e->bc = R->BC.W;
				e->de = R->DE.W;
				e->hl = R->HL.W;
				e->ix = R->IX.W;
				e->iy = R->IY.W;
				e->pc = R->PC.W;
				e->sp = R->SP.W;
				if (++Debugger.pc_detail_log_head == log_size)
					Debugger.pc_detail_log_head = 0;
				if (Debugger.pc_detail_log_count < log_size)
					Debugger.pc_detail_log_count++;
			}
		}
#endif // MEKA_Z80_DEBUGGER

        // Save ICount before instruction
        icount_before_instruction = R->ICount;

        // Execute instruction
        I = RdZ80 (R->PC.W ++);
        R->R++;
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
#ifdef MEKA_Z80_DEBUGGER
        Debugger.cycle_counter += (icount_before_instruction - R->ICount);

        // Reset stepping flag
        Debugger.stepping = FALSE;
#endif // MEKA_Z80_DEBUGGER

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
                    R->ICount+=R->IPeriod;                /* Add up to cycle counter  */
                    if (J.W==INT_NONE) J.W=R->IRequest;   /* Pending IRQ */
                }
            }
            else
            {
                J.W = LoopZ80(/*R*/);                   /* Call periodic handler    */
                R->ICount+=R->IPeriod;                  /* Add up to cycle counter  */
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

