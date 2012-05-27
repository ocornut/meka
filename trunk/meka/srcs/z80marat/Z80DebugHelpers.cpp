#include "shared.h"
#include "Z80.h"
#include "Z80DebugHelpers.h"
#include "Z80OpcodeEnums.h"

bool	Z80DebugHelper_IsRetExecuting(const Z80 *R)
{
	u16 pc = R->PC.W;

	u8 I = RdZ80_NoHook(pc++);

	if (I == PFX_ED)
	{
		while ((I = RdZ80_NoHook(pc++)) == PFX_ED) {}
		switch (I)
		{
		case RETI: return true;
		case RETN: return true;
		}
		return false;
	}

	if (I == PFX_DD || I == PFX_FD)
	{
		while ((I = RdZ80_NoHook(pc++)) == PFX_DD || I == PFX_FD) {}
		// RET opcodes after DD/FD prefixes are the same as with no prefix
	}

	switch (I)
	{
	case RET_NZ:  return (!(R->AF.B.l&Z_FLAG));
	case RET_NC:  return (!(R->AF.B.l&C_FLAG));
	case RET_PO:  return (!(R->AF.B.l&P_FLAG));
	case RET_P:   return (!(R->AF.B.l&S_FLAG));
	case RET_Z:   return ((R->AF.B.l&Z_FLAG)!=0);
	case RET_C:   return ((R->AF.B.l&C_FLAG)!=0);
	case RET_PE:  return ((R->AF.B.l&P_FLAG)!=0);
	case RET_M:   return ((R->AF.B.l&S_FLAG)!=0);
	case RET:	  return true;
	}

	return false;
}
