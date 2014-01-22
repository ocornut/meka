/** Z80-Call *************************************************/
/** This file contains debugging tools created for MEKA     **/
/** in order to log what opcodes are being used in a game.  **/
/*************************************************************/

// Define to enable Z80-Call functionnality
// (Slow down emulation)
// #define MEKA_Z80_OPCODES_USAGE

#ifdef MEKA_Z80_OPCODES_USAGE

#define MEKA_Z80_OPCODE_PREFIX_NONE     (0)
#define MEKA_Z80_OPCODE_PREFIX_CB       (1)
#define MEKA_Z80_OPCODE_PREFIX_DD       (2)
#define MEKA_Z80_OPCODE_PREFIX_DDCB     (3)
#define MEKA_Z80_OPCODE_PREFIX_ED       (4)
#define MEKA_Z80_OPCODE_PREFIX_FD       (5)
#define MEKA_Z80_OPCODE_PREFIX_FDCB     (6)
#define MEKA_Z80_OPCODE_PREFIX_MAX      (7)

int     Z80_Opcodes_Usage [MEKA_Z80_OPCODE_PREFIX_MAX] [256];

void    Z80_Opcodes_Usage_Reset (void)
{
 memset (Z80_Opcodes_Usage, 0, sizeof (Z80_Opcodes_Usage));
}

extern  byte  *ROM;
extern  byte  *Mem_Pages[8];
extern  int    Z80_Disassemble(char *S, word A, bool display_symbols, bool display_symbols_for_current_index_registers, bool resolve_indirect_offsets);

void    Z80_Opcodes_Usage_Print (void)
{
 int    i, j;
 byte  *op_code = Mem_Pages[0] = ROM; // Note: this will screws up data in ROM
 int    op_code_pos;
 byte   op_name[32];
 byte  *cycles;

 printf ("Z80_Opcodes_Usage_Print();\n");
 for (i = 0; i < MEKA_Z80_OPCODE_PREFIX_MAX; i++)
     {
     op_code_pos = 0;
     memset (op_code, 0, 16);
     switch (i)
        {
        case MEKA_Z80_OPCODE_PREFIX_CB:   cycles = CyclesCB;   printf ("---- Prefix: CB\n");   op_code[op_code_pos++] = 0xCB; break;
        case MEKA_Z80_OPCODE_PREFIX_DD:   cycles = CyclesXX;   printf ("---- Prefix: DD\n");   op_code[op_code_pos++] = 0xDD; break;
        case MEKA_Z80_OPCODE_PREFIX_DDCB: cycles = CyclesXXCB; printf ("---- Prefix: DDCB\n"); op_code[op_code_pos++] = 0xDD; op_code[op_code_pos++] = 0xCB; break;
        case MEKA_Z80_OPCODE_PREFIX_ED:   cycles = CyclesED;   printf ("---- Prefix: ED\n");   op_code[op_code_pos++] = 0xED; break;
        case MEKA_Z80_OPCODE_PREFIX_FD:   cycles = CyclesXX;   printf ("---- Prefix: FD\n");   op_code[op_code_pos++] = 0xFD; break;
        case MEKA_Z80_OPCODE_PREFIX_FDCB: cycles = CyclesXXCB; printf ("---- Prefix: FDCB\n"); op_code[op_code_pos++] = 0xFD; op_code[op_code_pos++] = 0xCB; break;
        case MEKA_Z80_OPCODE_PREFIX_NONE:
        default:                          cycles = Cycles;     printf ("---- Prefix: None\n"); break;
        }
     for (j = 0; j < 256; j++)
        if (Z80_Opcodes_Usage[i][j] > 0)
           {
           op_code[op_code_pos] = j;
           Z80_Disassemble(op_name, 0x0000, false, false, false);
           printf ("Opcode %02X : %- 12i%-20s%d\n", j, Z80_Opcodes_Usage[i][j], op_name, cycles[j]);
           }
     }
}

#endif /* MEKA_Z80_OPCODES_USAGE */

