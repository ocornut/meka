//-----------------------------------------------------------------------------
// MEKA - debugger.c
// MEKA Z80 Debugger - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "bios.h"
#include "debugger.h"
#include "desktop.h"
#include "g_widget.h"
#include "mappers.h"
#include "libparse.h"
#include "vmachine.h"
#include "z80marat/Z80DebugHelpers.h"
#include <ctype.h>
#include <string.h>

#ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define DEBUGGER_APP_TITLE              "Debugger"
#define DEBUGGER_APP_CPUSTATE_LINES     (2)

#define DEBUGGER_WATCH_FLOOD_LIMIT      (50)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_debugger      Debugger;

//-----------------------------------------------------------------------------
// External declaration
//-----------------------------------------------------------------------------

int Z80_Disassemble(char *dst, word addr, bool display_symbols, bool display_symbols_for_current_index_registers, bool resolve_indirect_offsets);
int Z80_Assemble(const char *src, byte dst[8]);

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void     Debugger_Applet_Init();
static void     Debugger_Applet_Layout(bool setup);
static void     Debugger_Applet_RedrawState();
static void     Debugger_Applet_UpdateShortcuts();

// Misc
static void     Debugger_Help(const char *cmd);
static void     Debugger_SetTrap(int trap); 
static void     Debugger_InputBoxCallback(t_widget *w);
static bool     Debugger_CompletionCallback(t_widget *w);

// Evaluator
static int      Debugger_Eval_GetValue(char **src, t_debugger_value *result);
bool            Debugger_Eval_ParseConstant(const char *value, t_debugger_value *result, t_debugger_eval_value_format default_format);
static int      Debugger_Eval_ParseExpression(char **expr, t_debugger_value *result);
static bool     Debugger_Eval_ParseVariable(int variable_replacement_flags, const char *var, t_debugger_value *result);

static void     Debugger_GetAccessString(int access, char buf[5])
{
    char *p = buf;
    if (access & BREAKPOINT_ACCESS_R)
        *p++ = 'R';
    if (access & BREAKPOINT_ACCESS_W)
        *p++ = 'W';
    if (access & BREAKPOINT_ACCESS_X)
        *p++ = 'X';
    if (access & BREAKPOINT_ACCESS_E)
        *p++ = 'E';
    *p = 0;
}

// Hooks
static void                     Debugger_Hooks_Install();
static void                     Debugger_Hooks_Uninstall();
static void                     Debugger_WrZ80_Hook(register u16 addr, register u8 value);
static u8                       Debugger_RdZ80_Hook(register u16 addr);
static void                     Debugger_OutZ80_Hook(register u16 addr, register u8 value);
static u8                       Debugger_InZ80_Hook(register u16 addr);

// Breakpoints
static void                     Debugger_BreakPoints_List();
static void                     Debugger_BreakPoints_Clear(bool disabled_only);
//static void                   Debugger_BreakPoints_RefreshCpuExecTraps();
static int                      Debugger_BreakPoints_AllocateId();
static t_debugger_breakpoint *  Debugger_BreakPoints_SearchById(int id);

// Breakpoint
static t_debugger_breakpoint *  Debugger_BreakPoint_Add(int type, int location, int access_flags, int address_start, int address_end, int auto_delete, const char *desc);
static void                     Debugger_BreakPoint_Remove(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_Enable(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_Disable(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_SetDataCompare(t_debugger_breakpoint *breakpoint, int data_compare_length, u8 data_compare_bytes[8]);
static void                     Debugger_BreakPoint_GetSummaryLine(t_debugger_breakpoint *breakpoint, char *buf);
static const char *             Debugger_BreakPoint_GetTypeName(t_debugger_breakpoint *breakpoint);
static bool                     Debugger_BreakPoint_ActivatedVerbose(t_debugger_breakpoint *breakpoint, int access, int addr, int value);

// Symbols
bool                            Debugger_Symbols_Load();
static void                     Debugger_Symbols_Clear();
static void                     Debugger_Symbols_ListByName(char *search_name);
static void                     Debugger_Symbols_ListByAddr(u32 cpu_addr);
const t_debugger_symbol *       Debugger_Symbols_GetFirstByAddr(u32 cpu_addr);
const t_debugger_symbol *       Debugger_Symbols_GetLastByAddr(u32 cpu_addr);
const t_debugger_symbol *       Debugger_Symbols_GetClosestPreviousByAddr(u32 cpu_addr, int range);
static void                     Debugger_Symbols_Vars_ListByName(char *search_name);
static void                     Debugger_Symbols_Vars_ListByAddr(u32 addr);

// Symbol
static t_debugger_symbol *      Debugger_Symbol_Add(u16 addr, int bank, const char *name);
static void                     Debugger_Symbol_Remove(t_debugger_symbol *symbol);
static int                      Debugger_Symbol_CompareByRomOrCpuAddress(const t_debugger_symbol *symbol1, const t_debugger_symbol *symbol2);

// History
static bool                     Debugger_History_Callback(t_widget *w, int level);
static void                     Debugger_History_AddLine(const char *line_to_add);
static void                     Debugger_History_List(const char *search_term_arg);

// Shortcuts
static void                     Debugger_ShortcutButton_Callback(t_widget* w);

// Values
static void                     Debugger_Value_SetCpuRegister(t_debugger_value *value, const char *name, void *data, int data_size);
static void                     Debugger_Value_SetSymbol(t_debugger_value *value, t_debugger_symbol *symbol, bool rom_addr);
static void                     Debugger_Value_SetComputed(t_debugger_value *value, u32 data, int data_size);
static void                     Debugger_Value_SetDirect(t_debugger_value *value, u32 data, int data_size);
static void                     Debugger_Value_Read(t_debugger_value *value);
static void                     Debugger_Value_Write(t_debugger_value *value, u32 data);

// Reverse Map
static void                     Debugger_ReverseMap(u16 addr);
static int                      Debugger_ReverseMapFindRomAddress(u16 addr, bool* is_bios);

//-----------------------------------------------------------------------------
// Data - Command Info/Help
//-----------------------------------------------------------------------------

struct t_debugger_command_info
{
    const char *        command_short;
    const char *        command_long;
    const char *        abstract;
    const char *        description;
};

static t_debugger_command_info              DebuggerCommandInfos[] =
{
    {
        "S", "STEP",
        "Step over",
        // Description
        "S: Step over\n"
        "Usage:\n"
        " S             ; Step over current instruction\n"
        "Examples:\n"
        " 0000: CALL $0100\n"
        " 0003: LD HL, $1FFF\n"
        " S             ; Resume execution after the call"
    },
    {
        "C", "CONT",
        "Continue",
        // Description
        "C/CONT: Continue execution\n"
        "Usage:\n"
        " C             ; Continue\n"
        " C address     ; Continue up to reaching <address>"
    },
    {
        "SO", "STEPOUT",
        "Continue to next RET instruction",
        // Description
        "SO/STEPOUT: Continue to next RET* instruction",
    },
    {
        NULL, "CLOCK",
        "Display Z80 cycle accumulator",
        // Description
        "CLOCK: Display Z80 cycle accumulator\n"
        "Usage:\n"
        " CLOCK         ; Display cycle accumulator\n"
        " CLOCK R[ESET] ; Reset cycle accumulator"
    },
    {
        "J", "JUMP",
        "Jump",
        // Description
        "J/JUMP: Jump\n"
        "Usage:\n"
        " J address     ; Jump to <address>\n"
        "Examples:\n"
        " J 0           ; Jump back to $0000 (reset)\n"
        " J 1000        ; Jump to $1000\n"
        "Note:\n"
        " Equivalent to SET PC=address"
    },
    {
        "B", "BREAK",
        "Manage breakpoints",
        // Description
        "B/BREAK: Manage breakpoints\n"
        "Usage:\n"
        " B address [..address2]         ; Add CPU breakpoint\n"
        " B [access] [bus] address [=xx] ; Add breakpoint\n"
        " B LIST                         ; List breakpoints\n"
        " B REMOVE id                    ; Remove breakpoint <id>\n"
        " B ENABLE id                    ; Enable breakpoint <id>\n"
        " B DISABLE id                   ; Disable breakpoint <id>\n"
        " B CLEAR                        ; Clear breakpoints\n"
        "Parameters:\n"
        " address : breakpoint address, can be a range or label\n"
        " access  : access to trap, any from r/w/x (rwx)\n"
        " bus     : bus/event, one from cpu/io/vram/pal/rom/line (cpu)\n"
        //" id      : breakpoint identifier\n"
        "Examples:\n"
        " B 0038          ; break when CPU access $0038\n"
        " B w io 7f       ; break on IO write to $7F\n"
        " B rx e000..ffff ; break on CPU read/exec from $E000+\n"
        " B x =0,0,C9     ; break on CPU execution of NOP NOP RET\n"
        " B w vram 3f00.. ; break on VRAM write to SAT\n"
        " B w pram 0 =03  ; break on PRAM write of color 0 as $03\n"
        " B line #13      ; break on display line 13"
    },
    {
        "W", "WATCH",
        "Watchpoints",
        // Description
        "W/WATCH: Manage watchpoints\n"
        "Usage:\n"
        " Same as B/BREAK.\n"
        " Watchpoints will display value but won't break.\n"
        "Examples:\n"
        " W r io dd      ; watch all IO read from DDh\n"
        " W w pal 0..    ; watch all palette write\n"
        " W w pal 0.. =3 ; watch all palette write of red color"
        //"Warning:\n"
        //" This feature can massively slow down display and fill up logs."
        //" As a safety measure, disk-logging is disabled for watchpoints"
        //" reports."
    },
    {
        "P", "PRINT",
        "Print expression",
        // Description
        "P/PRINT: Print expression\n"
        "Usage:\n"
        " P expr\n"
        " P expr[,expr,...]\n"
        "Examples:\n"
        " P IX,IY       ; print IX and IY registers\n"
        " P 1200+34     ; print $1234\n"
        " P %00101010   ; print 42\n"
        " P HL+(BC*4)   ; print HL+BC*4\n"
        " P label       ; print label"
    },
    {
        "R", "REGS",
        "Dump Z80 registers",
        // Description
        "R/REGS: Dump Z80 registers\n"
        "Usage:\n"
        " R"
    },
    {
        "SYM", "SYMBOLS",
        "Find symbols",
        // Description
        "SYM/SYMBOLS: Find symbols\n"
        "Usage:\n"
        " SYM [name]\n"
        " SYM @addr\n"
        "Parameters:\n"
        " name : symbol name to search for\n"
        " addr : symbol address to search for\n"
        "Examples:\n"
        " SYM vdp         ; search for symbol matching 'vdp'\n"
        " SYM @HL         ; search for symbol at address HL"
    },
    {
        NULL, "VARS",
        "Display variables",
        // Description
        "VARS: display variables\n"
        "Variables are symbols declared within a RAM location.\n"
        "Usage:\n"
        " VARS [name]\n"
        " VARS @addr\n"
        "Parameters:\n"
        " name : variable/symbol name to search for\n"
        " addr : variable/symbol address to search for\n"
        "Examples:\n"
        " VARS             ; display all variables\n"
        " VARS player      ; display variables matching 'player'\n"
        " VARS @HL         ; display variables at address HL"
    },
    {
        NULL, "SET",
        "Set Z80 register",
        // Description
        "SET: Set Z80 register\n"
        "Usage:\n"
        " SET register=value [,...]\n"
        "Parameters:\n"
        " register : Z80 register name\n"
        " value    : value to assign to register\n"
        "Examples:\n"
        " SET BC=$1234    ; set BC register to $1234\n"
        " SET DE=HL,HL=0  ; set DE=HL, then zero HL"
    },
    {
        NULL, "RMAP",
        "Reverse map Z80 address",
        // Description
        "RMAP: Reverse map Z80 address\n"
        "Usage:\n"
        " RMAP address\n"
        "Parameters:\n"
        " address : address in Z80 space\n"
        "Examples:\n"
        " RMAP $8001      ; eg: print 'ROM $14001 (Page 5 +0001)'\n"
        " RMAP $E001      ; eg: print 'RAM $C001'"
    },
    {
        "M", "MEM",
        "Dump memory",
        // Description
        "M/MEM: Dump memory\n"
        "Usage:\n"
        " M [address] [len]\n"
        "Parameters:\n"
        " address : address to dump memory from (PC)\n"
        " len     : bytes to dump (128)"
        "Examples:\n"
        " M              ; dump 128 bytes at PC\n"
        " M HL BC        ; dump BC bytes at HL"
    },
    {
        "ST", "STACK",
        "Dump stack",
        // Description
        "ST/STACK: Dump stack\n"
        "Usage:\n"
        " ST [len]\n"
        "Parameters:\n"
        " len     : bytes to dump (8)"
        "Examples:\n"
        " ST             ; dump 8 bytes at SP\n"
        " ST 100         ; dump 100 bytes at SP"
    },
    {
        "TR", "TRACE",
        "Trace past execution",
        // Description
        "TR/TRACE: Trace past execution\n"
        "Usage:\n"
        " TR [cnt]\n"
        " TR all         ; trace all (since last clear)\n"
        " TR clear       ; clear trace log\n"
        " TR regs        ; toggle dumping extra registers\n"
        "Parameters:\n"
        " cnt     : number of previous instruction (16)"
    },
    {
        "D", "DASM",
        "Disassemble",
        // Description
        "D/DASM: Disassemble instructions\n"
        "Usage:\n"
        " D [address] [cnt]\n"
        "Parameters:\n"
        " address : address to disassemble from (PC)\n"
        " cnt     : number of instruction to disassemble (10)"
    },
    {
        NULL, "MEMEDIT",
        "Memory Editor",
        // Description
        "MEMEDIT: Spawn memory editor\n"
        "Usage:\n"
        " MEMEDIT [lines] [cols]\n"
        "Parameters:\n"
        " lines : number of lines\n"
        " cols  : number of columns"
    },
    {
        "HI", "HISTORY",
        "History",
        // Description
        "HI/HISTORY: Print/search history\n"
        "Usage:\n"
        " HISTORY       ; Print history\n"
        " HISTORY word  ; Search history\n"
    },
    {
        "H", "HELP",
        "Help",
        // Description
        "H/HELP: Online help\n"
        "Usage:\n"
        " H             ; Get general help\n"
        " H command     ; Get detailed help on a command"
    },
    { 0 }
};

//-----------------------------------------------------------------------------
// Data - Bus infos
//-----------------------------------------------------------------------------

static t_debugger_bus_info  DebuggerBusInfos[BREAKPOINT_LOCATION_MAX_] =
{
    { BREAKPOINT_LOCATION_CPU,      "CPU",  2,  0x0000,     0xFFFF,     BREAKPOINT_ACCESS_RWX,  DEBUGGER_DATA_COMPARE_LENGTH_MAX    },
    { BREAKPOINT_LOCATION_ROM,      "ROM",  3,  0x000000,   0x1FFFFF,   BREAKPOINT_ACCESS_RWX,  DEBUGGER_DATA_COMPARE_LENGTH_MAX    },  // 'addr_max' is updated on media change. NB- 'w' is unusual but it doesn't hurt supporting it?
    { BREAKPOINT_LOCATION_IO,       "IO",   1,  0x00,       0xFF,       BREAKPOINT_ACCESS_RW,   1                                   },
    { BREAKPOINT_LOCATION_VRAM,     "VRAM", 2,  0x0000,     0x3FFF,     BREAKPOINT_ACCESS_RW,   DEBUGGER_DATA_COMPARE_LENGTH_MAX    },
    { BREAKPOINT_LOCATION_PRAM,     "PAL",  1,  0x00,       0x3F,       BREAKPOINT_ACCESS_W,    DEBUGGER_DATA_COMPARE_LENGTH_MAX    },  // 'addr_max' is updated on driver change
    { BREAKPOINT_LOCATION_LINE,     "LINE", 2,  0,          312,        BREAKPOINT_ACCESS_E,    0                                   },  // FIXME: A bit hacky
};

//-----------------------------------------------------------------------------
// Data - Applet
//-----------------------------------------------------------------------------

struct t_debugger_shortcut
{
    const char*         name;
    const char*         command;
    t_widget*           button;
};

struct t_debugger_app
{
    t_gui_box *         box;
    ALLEGRO_BITMAP *    box_gfx;
    t_widget *          console;
    t_widget *          input_box;
    t_font_id           font_id;
    int                 font_height;
    t_frame             frame_disassembly;
    t_frame             frame_shortcuts;
    t_frame             frame_cpustate;
    int                 disassembly_lines;

    std::vector<t_debugger_shortcut>    shortcuts;
    int                                 shortcuts_freeze;
};

t_debugger_app          DebuggerApp;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void        Debugger_Init_Values()
{
    Debugger.enabled = FALSE;
    Debugger.active = FALSE;
    Debugger.trap_set = FALSE;
    Debugger.trap_address = (u16)-1;
    Debugger.stepping = 0;
    Debugger.stepping_trace_after = 0;
    Debugger.stepping_out_enable = false;
    Debugger.stepping_out_stack_ref = 0x0000;
    Debugger.breakpoints = NULL;
    memset(Debugger.breakpoints_cpu_space,  0, sizeof(Debugger.breakpoints_cpu_space));
    memset(Debugger.breakpoints_io_space,   0, sizeof(Debugger.breakpoints_io_space));
    memset(Debugger.breakpoints_vram_space, 0, sizeof(Debugger.breakpoints_vram_space));
    memset(Debugger.breakpoints_pram_space, 0, sizeof(Debugger.breakpoints_pram_space));
    memset(Debugger.breakpoints_line_space, 0, sizeof(Debugger.breakpoints_line_space));
    Debugger.symbols = NULL;
    Debugger.symbols_count = 0;
    memset(Debugger.symbols_cpu_space,  0, sizeof(Debugger.symbols_cpu_space));
    Debugger.history_max = 99;  // Note: if more than 2 digits, update code in Debugger_History_List()
    Debugger.history_count = 1;
    Debugger.history = (t_debugger_history_item*)malloc(sizeof(t_debugger_history_item) * Debugger.history_max);
    memset(Debugger.history, 0, sizeof(t_debugger_history_item) * Debugger.history_max);
    Debugger.history_current_index = 0;
    Debugger.log_file = NULL;
    Debugger.log_filename = "debuglog.txt";
    Debugger.watch_counter = 0;
    Debugger.cycle_counter = 0;

    memset(Debugger.cpu_exec_traps, 0, sizeof(Debugger.cpu_exec_traps));

    Debugger.pc_detail_log_show_extra_registers = false;

    Debugger.trackback_scroll_offset = 0;

    // Add Z80 CPU registers variables
    Debugger.variables_cpu_registers = NULL;
    {
        Z80 *cpu = &sms.R;
        t_debugger_value *value;

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "PC",   &cpu->PC.W, 16);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "SP",   &cpu->SP.W, 16);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "AF",   &cpu->AF.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "A",    &cpu->AF.B.h, 8);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "BC",   &cpu->BC.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "B",    &cpu->BC.B.h, 8);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "C",    &cpu->BC.B.l, 8);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "DE",   &cpu->DE.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "D",    &cpu->DE.B.h, 8);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "E",    &cpu->DE.B.l, 8);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "HL",   &cpu->HL.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "H",    &cpu->HL.B.h, 8);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "L",    &cpu->HL.B.l, 8);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "IX",   &cpu->IX.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "IY",   &cpu->IY.W, 16);

        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "AF'",  &cpu->AF1.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "BC'",  &cpu->BC1.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "DE'",  &cpu->DE1.W, 16);
        list_add(&Debugger.variables_cpu_registers, (value = (t_debugger_value*)malloc(sizeof(t_debugger_value))));
        Debugger_Value_SetCpuRegister(value, "HL'",  &cpu->HL1.W, 16);
    }

    // Note: Some more clearing will be done by Debugger_MachineReset()
}

static void Debugger_Init_LogFile()
{
    // Open log file if not already open
    if (g_config.debugger_log_enabled && Debugger.log_file == NULL)
    {
        char filename[FILENAME_LEN];
        if (!al_filename_exists(g_env.Paths.DebugDirectory))
            al_make_directory(g_env.Paths.DebugDirectory);
        sprintf(filename, "%s/%s", g_env.Paths.DebugDirectory, Debugger.log_filename);
        Debugger.log_file = fopen(filename, "a+t");
        if (Debugger.log_file != NULL)
            fprintf(Debugger.log_file, Msg_Get(MSG_Log_Session_Start), meka_date_getf ());
    }
}

void        Debugger_Init()
{
    ConsolePrintf("%s\n", Msg_Get(MSG_Debug_Init));
    Debugger_Applet_Init();

    // Open log file
    if (Debugger.active)
        Debugger_Init_LogFile();

    // Print welcome line
    Debugger_Printf("%s\n", Msg_Get(MSG_Debug_Welcome));
    Debugger_Printf("Enter H for help. Open HELP menu for details.\n");
    Debugger_Printf("Press TAB for completion.\n");
}

void        Debugger_Close()
{
    if (Debugger.log_file != NULL)
    {
        fclose(Debugger.log_file);
        Debugger.log_file = NULL;
    }
    // FIXME-HISTORY: free history lines
    free(Debugger.history);
    Debugger.history = NULL;
}

void        Debugger_Enable()
{
    Debugger.enabled = TRUE;
    Debugger.active  = FALSE;
    Debugger.trap_set = FALSE;
    Debugger.trap_address = 0x0000;
}

// Called when the machine gets reseted
void        Debugger_MachineReset()
{
    if (!Debugger.active)
        return;

    // Reset breakpoint on CPU
    Debugger_Printf("%s\n", Msg_Get(MSG_Machine_Reset));
    Debugger_SetTrap(0x0000);
    sms.R.Trace = 1;

    // Reset trap table
    // Debugger_BreakPointRefreshCpuExecTraps();

    Debugger.pc_last = 0;
    memset(Debugger.pc_exec_points, 0, sizeof(Debugger.pc_exec_points));
    Debugger.pc_detail_log_data.resize(64*1024); // 256 KB buffer
    Debugger.pc_detail_log_data[Debugger.pc_detail_log_data.size()-1].pc = 0xffff;
    Debugger.pc_detail_log_head = 0;
    Debugger.pc_detail_log_count = 0;

    // Hook Z80 read/write and I/O
    Debugger_Hooks_Install();

    // Update PRAM size
    if (g_driver->id == DRV_GG)
        DebuggerBusInfos[BREAKPOINT_LOCATION_PRAM].addr_max = 0x3F;
    else
        DebuggerBusInfos[BREAKPOINT_LOCATION_PRAM].addr_max = 0x1F;

    // Reset cycle counter
    Debugger.cycle_counter = 0;
}

// Called when the media (ROM) gets changed/reloaded
void        Debugger_MediaReload()
{
    if (!Debugger.enabled)
        return;

    // Update ROM size
    DebuggerBusInfos[BREAKPOINT_LOCATION_ROM].addr_max = (tsms.Size_ROM - 1);

    // Reload symbols
    Debugger_Symbols_Load();
}

void    Debugger_Update()
{
    if (Debugger.active)
    {
        // If skin has changed, redraw everything
        t_debugger_app* app = &DebuggerApp;
        if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
        {
            Debugger_Applet_Layout(FALSE);
            app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        }
        Debugger_Applet_UpdateShortcuts();
        Debugger_Applet_RedrawState();
    }

    // Reset watch counter
    Debugger.watch_counter = 0;
}

void    Debugger_Applet_UpdateShortcuts()
{
    t_debugger_app* app = &DebuggerApp;

    if (app->shortcuts_freeze > 0)
    {
        // We intentionally skip 1 update on simple STEP operation to avoid flickering buttons
        app->shortcuts_freeze--;
        return;
    }

    app->shortcuts[0].command = (bool)(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)) ? "C" : "STEPINTO";
    widget_button_set_label(app->shortcuts[0].button, (bool)(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)) ? "Cont" : "Stop");
    widget_button_set_grayed_out(app->shortcuts[0].button, !(bool)(g_machine_flags & (MACHINE_POWER_ON)));
    widget_button_set_grayed_out(app->shortcuts[1].button, !(bool)(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)));
    widget_button_set_grayed_out(app->shortcuts[2].button, !(bool)(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)));
    widget_button_set_grayed_out(app->shortcuts[3].button, !(bool)(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)));
    widget_button_set_grayed_out(app->shortcuts[4].button, !(bool)(g_machine_flags & (MACHINE_POWER_ON)));
}

int     Debugger_Hook(Z80 *R)
{
    const u16 pc = R->PC.W;
    // Debugger_Printf("hook, pc=%04X\n", pc);

    // If in stepping, disable current hook/breakpoint
    // Note that the stepping flag is reseted after each opcode execution, so it
    // only serves to avoid getting repeated breakpoint on the same location and
    // unable to "leave" the instruction.
    if (Debugger.stepping)
    {
        R->Trace = Debugger.stepping_trace_after;
        Debugger.stepping = 0;
        return (1);
    }

    // Always remove stepping flag, so we can break at another point
    // eg: if we stepped on a CALL instruction, or if an interrupt was raised
    // Debugger.stepping = -1;

    // If we arrived from a trap, print a line about it
    if (pc == Debugger.trap_address)
        Debugger_Printf("Break at $%04X\n", pc);

    // If we arrived from a breakpoint CPU exec trap...
    if (Debugger.cpu_exec_traps[pc])
    {
        int cnt = Debugger.cpu_exec_traps[pc];
        bool break_activated = FALSE;
        
        for (t_list* breakpoints = Debugger.breakpoints_cpu_space[pc]; breakpoints != NULL; breakpoints = breakpoints->next)
        {
            t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
            if (breakpoint->enabled)
            {
                if ((breakpoint->access_flags & BREAKPOINT_ACCESS_X) && (breakpoint->location == BREAKPOINT_LOCATION_CPU || breakpoint->location == BREAKPOINT_LOCATION_ROM) )
                {
                    if (Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_X, pc, RdZ80_NoHook(pc)))
                        break_activated = TRUE;
                    cnt--;
                }
            }
        }
        assert(cnt == 0);
        if (!break_activated && !R->Trace)
        {
            //R->Trace = 0;
            return (1);
        }
    }

    if (Debugger.stepping_out_enable)
    {
        if (R->SP.W < Debugger.stepping_out_stack_ref)
            return (1);
        if (!Z80DebugHelper_IsRetExecuting(R))
            return (1);
        Debugger.stepping_out_enable = false;
    }

    // Update state
    //Debugger_Applet_UpdateShortcuts();
    //Debugger_Applet_RedrawState();

    // Set machine in debugging mode (halted)
    Machine_Debug_Start();    

    // Ask Z80 emulator to stop now
    return (0);
}

void    Debugger_SetTrap(int trap)
{
    if (trap == -1)
    {
        Debugger.trap_set = FALSE;
        Debugger.trap_address = (u16)-1;
        sms.R.Trap = 0xFFFF;
    }
    else
    {
        Debugger.trap_set = TRUE;
        Debugger.trap_address = trap;
        sms.R.Trap = Debugger.trap_address;
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - Breakpoints Manager
//-----------------------------------------------------------------------------

void    Debugger_BreakPoints_Clear(bool disabled_only)
{
    for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; )
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        breakpoints = breakpoints->next;
        if (disabled_only && breakpoint->enabled)
            continue;
        Debugger_BreakPoint_Remove(breakpoint);
    }
    if (disabled_only)
        Debugger_Printf("Disabled breakpoints cleared.\n");
    else
        Debugger_Printf("Breakpoints cleared.\n");
}

void    Debugger_BreakPoints_List()
{
    Debugger_Printf("Breakpoints/Watchpoints:\n");
    if (Debugger.breakpoints == NULL)
    {
        Debugger_Printf(" <None>\n");
        return;
    }
    for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        char buf[256];
        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
        Debugger_Printf(" %s\n", buf);
    }
}

// FIXME: May want to find the first empty slot (instead of max+1)
int     Debugger_BreakPoints_AllocateId()
{
    int max = -1;
    for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        if (breakpoint->id > max)
            max = breakpoint->id;
    }
    return (max + 1);
}

t_debugger_breakpoint *     Debugger_BreakPoints_SearchById(int id)
{
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        if (breakpoint->id == id)
            return (breakpoint);
    }
    return (NULL);
}

/*
void    Debugger_BreakPoints_RefreshCpuExecTraps()
{
    // First clear table
    memset(Debugger_Z80_PC_Trap, 0, sizeof(Debugger_Z80_PC_Trap));

    // Then go thru all breakpoints to add their trap
    for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; )
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        if (breakpoint->location == BREAKPOINT_LOCATION_CPU)
            if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            {
                int addr;
                for (addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)// = (addr + 1) & 0xffff)
                    Debugger_CPU_Exec_Traps[addr]++;
            }
    }
}
*/

//-----------------------------------------------------------------------------
// FUNCTIONS - Breakpoint
//-----------------------------------------------------------------------------

t_debugger_breakpoint *     Debugger_BreakPoint_Add(int type, int location, int access_flags, int address_start, int address_end, int auto_delete, const char *desc)
{
    t_debugger_breakpoint * breakpoint;

    // Check parameters
    assert(address_start <= address_end);
    assert(address_start >= 0);
    assert(address_end <= DebuggerBusInfos[location].addr_max);
    assert(type == BREAKPOINT_TYPE_BREAK || type == BREAKPOINT_TYPE_WATCH);

    // Create and setup breakpoint
    breakpoint = (t_debugger_breakpoint*)malloc(sizeof (t_debugger_breakpoint));
    breakpoint->enabled = FALSE;
    breakpoint->id = Debugger_BreakPoints_AllocateId();
    breakpoint->type = type;
    breakpoint->location = location;
    breakpoint->access_flags = access_flags;
    breakpoint->address_range[0] = address_start;
    breakpoint->address_range[1] = address_end;
    breakpoint->auto_delete = auto_delete;
    breakpoint->data_compare_length = 0;
    breakpoint->desc = desc ? strdup(desc) : NULL;

    // Add to global breakpoint list
    list_add_to_end(&Debugger.breakpoints, breakpoint);

    // Enable
    Debugger_BreakPoint_Enable(breakpoint);

    return (breakpoint);
}

void                        Debugger_BreakPoint_Remove(t_debugger_breakpoint *breakpoint)
{
    // Check parameters
    assert(breakpoint != NULL);

    // Disable
    Debugger_BreakPoint_Disable(breakpoint);

    // Remove from global breakpoint list
    list_remove(&Debugger.breakpoints, breakpoint);

    // Delete members
    if (breakpoint->desc != NULL)
        free (breakpoint->desc);

    // Delete
    free(breakpoint);
}

void                     Debugger_BreakPoint_Enable(t_debugger_breakpoint *breakpoint)
{
    if (breakpoint->enabled)
        return;

    // Set flag
    breakpoint->enabled = true;

    // Add to corresponding bus space list
    t_list ** bus_lists;
    switch (breakpoint->location)
    {
    case BREAKPOINT_LOCATION_CPU:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_ROM:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_IO:    bus_lists = Debugger.breakpoints_io_space;    break;
    case BREAKPOINT_LOCATION_VRAM:  bus_lists = Debugger.breakpoints_vram_space;  break;
    case BREAKPOINT_LOCATION_PRAM:  bus_lists = Debugger.breakpoints_pram_space;  break;
    case BREAKPOINT_LOCATION_LINE:  bus_lists = Debugger.breakpoints_line_space;  break;
    default: assert(0); return;
    }

    // Add to CPU exec trap?
    bool cpu_exec_trap = false;
    if (breakpoint->location == BREAKPOINT_LOCATION_CPU || breakpoint->location == BREAKPOINT_LOCATION_ROM)
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            cpu_exec_trap = true;

    if (breakpoint->location == BREAKPOINT_LOCATION_ROM)
    {
        // FIXME: Redundant breakpoints will be added if range span over the size of a page
        // FIXME: Most mapper use 0x4000 sized pages, it would be an optimisation to know this information
        const int mapper_page_size = 0x2000;
        const int mapper_bank_count = 0xC000/mapper_page_size;
        const int addr_min = (int)(breakpoint->address_range[0] & (mapper_page_size-1));
        const int addr_max = (int)(breakpoint->address_range[1] & (mapper_page_size-1));    // Prewrap both ends of the range to avoid duplicate additions.
        for (int addr = addr_min; ; addr++)
        {
            const u32 addr0 = (u32)(addr & (mapper_page_size-1));
            for (int i = 0; i != mapper_bank_count; i++)
            {
                const u32 addr_candidate = addr0 | (i * mapper_page_size);
                list_add(&bus_lists[addr_candidate], breakpoint);
                if (cpu_exec_trap)
                    Debugger.cpu_exec_traps[addr_candidate]++;
            }
            if (addr0 == (u32)addr_max)
                break;
        }
    }
    else
    {
        for (int addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)
        {
            list_add(&bus_lists[addr], breakpoint);
            if (cpu_exec_trap)
                Debugger.cpu_exec_traps[addr]++;
        }
    }
}

void                     Debugger_BreakPoint_Disable(t_debugger_breakpoint *breakpoint)
{
    if (!breakpoint->enabled)
        return;

    // Set flag
    breakpoint->enabled = false;

    // Remove from bus space list
    t_list ** bus_lists;
    switch (breakpoint->location)
    {
    case BREAKPOINT_LOCATION_CPU:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_ROM:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_IO:    bus_lists = Debugger.breakpoints_io_space;    break;
    case BREAKPOINT_LOCATION_VRAM:  bus_lists = Debugger.breakpoints_vram_space;  break;
    case BREAKPOINT_LOCATION_PRAM:  bus_lists = Debugger.breakpoints_pram_space;  break;
    case BREAKPOINT_LOCATION_LINE:  bus_lists = Debugger.breakpoints_line_space;  break;
    default: assert(0); return;
    }

    // Add to CPU exec trap?
    bool cpu_exec_trap = false;
    if (breakpoint->location == BREAKPOINT_LOCATION_CPU || breakpoint->location == BREAKPOINT_LOCATION_ROM)
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            cpu_exec_trap = true;

    if (breakpoint->location == BREAKPOINT_LOCATION_ROM)
    {
        const int mapper_page_size = 0x2000;
        const int mapper_bank_count = 0xC000/mapper_page_size;
        const int addr_min = (int)(breakpoint->address_range[0] & (mapper_page_size-1));
        const int addr_max = (int)(breakpoint->address_range[1] & (mapper_page_size-1));    // Prewrap both ends of the range to avoid duplicate additions.
        for (int addr = addr_min; ; addr++)
        {
            const u32 addr0 = (u32)(addr & (mapper_page_size-1));
            for (int i = 0; i != mapper_bank_count; i++)
            {
                const u32 addr_candidate = addr0 | (i * mapper_page_size);
                list_remove(&bus_lists[addr_candidate], breakpoint);
                if (cpu_exec_trap)
                    Debugger.cpu_exec_traps[addr_candidate]--;
            }
            if (addr0 == (u32)addr_max)
                break;
        }
    }
    else
    {
        for (int addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)
        {
            list_remove(&bus_lists[addr], breakpoint);
            if (cpu_exec_trap)
                Debugger.cpu_exec_traps[addr]--;
        }
    }
}

void                        Debugger_BreakPoint_SetDataCompare(t_debugger_breakpoint *breakpoint, int data_compare_length, u8 data_compare_bytes[8])
{
    assert(data_compare_length >= 0 && data_compare_length <= DEBUGGER_DATA_COMPARE_LENGTH_MAX);
    breakpoint->data_compare_length = data_compare_length;
    if (data_compare_length != 0)
        memcpy(breakpoint->data_compare_bytes, data_compare_bytes, data_compare_length);
}

const char *                Debugger_BreakPoint_GetTypeName(t_debugger_breakpoint *breakpoint)
{
    switch (breakpoint->type)
    {
    case BREAKPOINT_TYPE_BREAK: 
        return "Breakpoint";
    case BREAKPOINT_TYPE_WATCH:
        return "Watchpoint";
    }
    assert(0);
    return ("XXX");
}

void    Debugger_BreakPoint_GetSummaryLine(t_debugger_breakpoint *breakpoint, char *buf)
{
    char    addr_string[16];
    int     bus_size;
    t_debugger_bus_info *bus_info = &DebuggerBusInfos[breakpoint->location];
    
    bus_size = bus_info->bus_addr_size; 

    if (breakpoint->address_range[0] != breakpoint->address_range[1])
        sprintf(addr_string, "%0*X..%0*X", bus_size * 2, breakpoint->address_range[0], bus_size * 2, breakpoint->address_range[1]);
    else
        sprintf(addr_string, "%0*X", bus_size * 2, breakpoint->address_range[0]);

    sprintf(buf, "%c%d%c %s %-4s  %c%c%c  %-10s  %s", 
        breakpoint->enabled ? '[' : '(',
        breakpoint->id, 
        breakpoint->enabled ? ']' : ')',
        (breakpoint->type == BREAKPOINT_TYPE_BREAK) ? "Break" : "Watch",
        bus_info->name, 
        (bus_info->access & (BREAKPOINT_ACCESS_R | BREAKPOINT_ACCESS_E) ? 
            ((breakpoint->access_flags & BREAKPOINT_ACCESS_E) ? ('E') : 
    ((breakpoint->access_flags & BREAKPOINT_ACCESS_R) ? 'R' : '.')) : ' '),
        (bus_info->access & BREAKPOINT_ACCESS_W ? ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) ? 'W' : '.') : ' '),
        (bus_info->access & BREAKPOINT_ACCESS_X ? ((breakpoint->access_flags & BREAKPOINT_ACCESS_X) ? 'X' : '.') : ' '),
        addr_string,
        (breakpoint->enabled == FALSE) ? "(disabled) " : ""
        );

    // Add data compare bytes (if any)
    if (breakpoint->data_compare_length != 0)
    {
        char data_compare_string[1 + (DEBUGGER_DATA_COMPARE_LENGTH_MAX*3) + 2];
        int pos = 1;
        int i;
        strcpy(data_compare_string, "=");
        for (i = 0; i != breakpoint->data_compare_length; i++)
        {
            sprintf(&data_compare_string[pos], "%02X", breakpoint->data_compare_bytes[i]);
            pos += 2;
            if (i + 1 != breakpoint->data_compare_length)
                data_compare_string[pos++] = ',';
        }
        data_compare_string[pos++] = ' ';
        data_compare_string[pos] = '\0';
        strcat(buf, data_compare_string);
    }

    // Add description (if any)
    if (breakpoint->desc != NULL)
        strcat(buf, breakpoint->desc);

    // Trim trailing spaces
    StrTrimEnd(buf);
}

bool    Debugger_BreakPoint_ActivatedVerbose(t_debugger_breakpoint *breakpoint, int access, int addr, int value)
{
    t_debugger_bus_info *bus_info = &DebuggerBusInfos[breakpoint->location];
    const char *action;
    char buf[256];

    // Debugger_Printf("Debugger_BreakPoint_ActivatedVerbose() %04x\n", addr);

    int rmapped_addr = addr;

    if (breakpoint->location == BREAKPOINT_LOCATION_ROM)
    {
        bool is_bios;
        const int rom_addr = Debugger_ReverseMapFindRomAddress(addr, &is_bios);
        if (rom_addr == -1)
            return FALSE;
        if (rom_addr < breakpoint->address_range[0] || rom_addr > breakpoint->address_range[1])
            return FALSE;   // False candidate (due to mapping)
        rmapped_addr = rom_addr;
    }

    // Data comparer
    // FIXME
    if (breakpoint->data_compare_length != 0)
    {
        if (breakpoint->data_compare_bytes[0] != value)
            return false;
        if (breakpoint->data_compare_length > 1)
        {
            int i;
            for (i = 1; i != breakpoint->data_compare_length; i++)
            {
                int value2 = Debugger_Bus_Read(breakpoint->location, rmapped_addr + i);
                if (value2 != breakpoint->data_compare_bytes[i])
                    return false;
            }
        }
    }

    // Action
    if (breakpoint->type == BREAKPOINT_TYPE_BREAK)
    {
        // Break
        sms.R.Trace = 1;
        Debugger.stepping_out_enable = false;
        action = "break";
    }
    else
    {
        // Watch
        if (++Debugger.watch_counter >= DEBUGGER_WATCH_FLOOD_LIMIT)
        {
            // Flood?
            if (Debugger.watch_counter == DEBUGGER_WATCH_FLOOD_LIMIT)
                Debugger_Printf("Maximum number of watch triggered this frame (%d)\nWill stop displaying more, to prevent flood.\nConsider removing/tuning your watchpoints.\n", DEBUGGER_WATCH_FLOOD_LIMIT);
            return true;
        }
        action = "watch";
    }

    // Verbose to user
    if (access & BREAKPOINT_ACCESS_R)
    {
        sprintf(buf, "%04X: [%d] %s %s read from %0*X, read value=%02x", 
            Debugger.pc_last,
            breakpoint->id,
            action,
            bus_info->name,
            bus_info->bus_addr_size * 2,
            rmapped_addr,
            value);
    }
    else if (access & BREAKPOINT_ACCESS_W)
    {
        sprintf(buf, "%04X: [%d] %s %s write to %0*X, writing value=%02x", 
            Debugger.pc_last,
            breakpoint->id,
            action,
            bus_info->name,
            bus_info->bus_addr_size * 2,
            rmapped_addr,
            value);
    }
    else if (access & BREAKPOINT_ACCESS_X)
    {
        sprintf(buf, "%04X: [%d] %s %s execution", 
            Debugger.pc_last,
            breakpoint->id,
            action,
            bus_info->name);
    }
    else if (access & BREAKPOINT_ACCESS_E)
    {
        sprintf(buf, "%04X: [%d] %s %s %d event", 
            Debugger.pc_last,
            breakpoint->id,
            action,
            bus_info->name,
            rmapped_addr);
    }
    else
    {
        assert(0);
    }

    // Print data comparer info
    if (breakpoint->data_compare_length != 0)
    {
        // Only if more than 1 byte or when executed
        // (because for read/write the byte is already shown on the line)
        if (breakpoint->data_compare_length >= 2 || (access & (BREAKPOINT_ACCESS_X | BREAKPOINT_ACCESS_E)))
        {
            char data_compare_string[8 + (DEBUGGER_DATA_COMPARE_LENGTH_MAX*3) + 2];
            int pos;
            int i;
            strcpy(data_compare_string, ", match ");
            pos = strlen(data_compare_string);
            for (i = 0; i != breakpoint->data_compare_length; i++)
            {
                sprintf(&data_compare_string[pos], "%02X", breakpoint->data_compare_bytes[i]);
                pos += 2;
                if (i + 1 != breakpoint->data_compare_length)
                    data_compare_string[pos++] = ',';
            }
            data_compare_string[pos] = '\0';
            strcat(buf, data_compare_string);
        }
    }

    Debugger_Printf("%s\n", buf);

    return true;
}

//-----------------------------------------------------------------------------
// FUNCTIONS - BUS
//-----------------------------------------------------------------------------

int      Debugger_Bus_Read(int bus, int addr)
{
    switch (bus)
    {
    case BREAKPOINT_LOCATION_CPU:
        {
            addr &= 0xFFFF;
            return (g_machine_flags & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00;
        }
    case BREAKPOINT_LOCATION_VRAM:
        {
            addr &= 0x3FFF;
            return VRAM[addr];
        }
    case BREAKPOINT_LOCATION_PRAM:
        {
            switch (g_machine.driver_id)
            {
            case DRV_SMS:   addr &= 31;  break;
            case DRV_GG:    addr &= 63;  break;
            default:
                // FIXME
                return -1;
            }
            return PRAM[addr];
        }
    default:
        {
            assert(0);
            return -1;
        }
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - SYMBOLS
//-----------------------------------------------------------------------------

enum t_debugger_symbol_file_type
{
    DEBUGGER_SYMBOL_FILE_TYPE_WLA,
    DEBUGGER_SYMBOL_FILE_TYPE_SJASM,
    DEBUGGER_SYMBOL_FILE_TYPE_TASM,
    DEBUGGER_SYMBOL_FILE_TYPE_MAX_
};

bool    Debugger_Symbols_TryParseLine(const char* line_original, t_debugger_symbol_file_type symbol_file_type)
{
    char line_buf[512];
    strcpy(line_buf, line_original);
    char* line = line_buf;

    int bank;
    u32 addr32;
    char name[512];

    switch (symbol_file_type)
    {
    case DEBUGGER_SYMBOL_FILE_TYPE_WLA:
        {
            // NO$GMB/WLA format
            //  "0000:c007 VarScanlineMetrics"
            if (sscanf(line, "%X:%X %s", &bank, &addr32, name) == 3)
            {
                Debugger_Symbol_Add(addr32 & 0xFFFF, bank, name);
                return true;
            }
            if (sscanf(line, "%X %s", &addr32, name) == 2)
            {
                Debugger_Symbol_Add(addr32 & 0xFFFF, -1, name);
                return true;
            }
            break;
        }
    case DEBUGGER_SYMBOL_FILE_TYPE_SJASM:
        {
            // SJASM format
            //  "pause_music: equ 0000074Fh"
            if (parse_getword(name, countof(name), &line, ":\t\r\n", ';', PARSE_FLAGS_NONE))
            {
                parse_skip_spaces(&line);
                if (sscanf(line, "equ %Xh", &addr32) == 1)
                {
                    Debugger_Symbol_Add(addr32 & 0xFFFF, -1, name);
                    return true;
                }
            }
            break;
        }
    case DEBUGGER_SYMBOL_FILE_TYPE_TASM:
        {
            if (parse_getword(name, countof(name), &line, " \t", ';', PARSE_FLAGS_NONE))
            {
                parse_skip_spaces(&line);
                if (sscanf(line, "%Xh", &addr32) == 1)
                {
                    Debugger_Symbol_Add(addr32 & 0xFFFF, -1, name);
                    return true;
                }
            }
            break;
        }
    default:
        break;
    }
    return false;
}

bool    Debugger_Symbols_Load()
{
    if (!Debugger.enabled)
        return false;

    // First clear any existing symbol
    Debugger_Symbols_Clear();

    // Load symbol file
    // 1. Try "image.sym"
    char symbol_filename[FILENAME_LEN];
    strcpy(symbol_filename, g_env.Paths.MediaImageFile);
    StrPath_RemoveExtension(symbol_filename);
    strcat(symbol_filename, ".sym");
    t_tfile* symbol_file = tfile_read(symbol_filename);
    if (symbol_file == NULL)
    {
        // Note: we silently fail on MEKA_ERR_FILE_OPEN (file not found / cannot open)
        if (meka_errno != MEKA_ERR_FILE_OPEN)
            Msg(MSGT_USER, "%s", meka_strerror());

        // 2. Try "image.ext.sym"
        snprintf(symbol_filename, FILENAME_LEN, "%s.sym", g_env.Paths.MediaImageFile);
        symbol_file = tfile_read(symbol_filename);
        if (symbol_file == NULL)
        {
            if (meka_errno != MEKA_ERR_FILE_OPEN)
                Msg(MSGT_USER, "%s", meka_strerror());
            return false;
        }
    }
    StrPath_RemoveDirectory(symbol_filename);

    int line_cnt = 0;
    bool error = false;
    for (t_list* lines = symbol_file->data_lines; lines; lines = lines->next)
    {
        char* line = (char*) lines->elem;
        line_cnt += 1;

        // Msg(MSGT_DEBUG, "%s", line);

        // Strip comments, skip empty lines
        char* p = strchr(line, ';');
        if (p != NULL)
            *p = EOSTR;
        StrTrim(line);
        if (StrIsNull(line))
            continue;

        // Parse
        bool parse_success = false;
        for (int symbol_file_type = 0; symbol_file_type != DEBUGGER_SYMBOL_FILE_TYPE_MAX_; symbol_file_type++)
        {
            if (Debugger_Symbols_TryParseLine(line, (t_debugger_symbol_file_type)symbol_file_type))
            {
                parse_success = true;
                break;
            }
        }
        if (!parse_success)
        {
            if (!error)
            {
                error = true;
                Msg(MSGT_USER, Msg_Get(MSG_Debug_Symbols_Error), symbol_filename);
            }
            Msg(MSGT_USER_LOG, Msg_Get(MSG_Debug_Symbols_Error_Line), line_cnt);
        }
    }

    // Free symbol file data
    tfile_free(symbol_file);

    // Sort by address
    list_sort(&Debugger.symbols, (int (*)(void *, void *))Debugger_Symbol_CompareByRomOrCpuAddress);

    // Verbose
    char buf[256];
    sprintf(buf, Msg_Get(MSG_Debug_Symbols_Loaded), Debugger.symbols_count, symbol_filename);
    Debugger_Printf("%s\n", buf);

    return (Debugger.symbols_count > 0);
}

void    Debugger_Symbols_Clear()
{
    for (t_list* symbols = Debugger.symbols; symbols != NULL; )
    {
        t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
        symbols = symbols->next;
        Debugger_Symbol_Remove(symbol);
    }
    assert(Debugger.symbols_count == 0);
}

void    Debugger_Symbols_ListByName(char *search_name)
{
    if (search_name)
    {
        Debugger_Printf("Symbols matching \"%s\":\n", search_name);
        search_name = strdup(search_name);
        StrUpper(search_name);
    }
    else
    {
        Debugger_Printf("Symbols:\n");
    }

    int count = 0;
    for (t_list* symbols = Debugger.symbols; symbols != NULL; symbols = symbols->next)
    {
        t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;

        // If search_name was specified, skip symbol not matching it
        if (search_name != NULL)
            if (strstr(symbol->name_uppercase, search_name) == NULL)
                continue;
        count++;
        Debugger_Printf(" %04X  %s\n", symbol->cpu_addr, symbol->name);
    }
    if (count == 0)
    {
        Debugger_Printf(" <None>\n");
    }
    else
    {
        Debugger_Printf("%d symbols\n", count);
    }
    if (search_name != NULL)
    {
        // Free the uppercase duplicate we made
        free(search_name);
    }
}

void    Debugger_Symbols_ListByAddr(u32 addr_request)
{
    addr_request &= 0xFFFF;
    u32 addr_sym = addr_request;
    while (addr_sym != (u32)-1)
    {
        if (Debugger.symbols_cpu_space[addr_sym] != NULL)
            break;
        addr_sym--;
    }

    if (addr_sym == addr_request || addr_sym == (u32)-1)
        Debugger_Printf("Symbols at address \"%04x\":\n", addr_request);
    else
        Debugger_Printf("Symbols near address \"%04x\":\n", addr_request);

    if (addr_sym == (u32)-1)
    {
        Debugger_Printf(" <None>\n");
    }
    else
    {
        t_list *symbols;
        for (symbols = Debugger.symbols_cpu_space[addr_sym]; symbols != NULL; symbols = symbols->next)
        {
            t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
            if (addr_sym != addr_request)
                Debugger_Printf(" %04X  %s + %X = %04X\n", symbol->cpu_addr, symbol->name, addr_request - addr_sym, addr_request);
            else
                Debugger_Printf(" %04X  %s\n", symbol->cpu_addr, symbol->name);
        }
    }
}

static void Debugger_Symbols_Vars_Print(const t_debugger_symbol* symbol, const t_list* next)
{
    // Default to 2
    int var_size = 2;
    const t_debugger_symbol* symbol_next = next ? (t_debugger_symbol *)next->elem : NULL;
    if (symbol_next != NULL)
    {
        const int offset_to_next_symbol = (int)(symbol_next->cpu_addr - symbol->cpu_addr);
        if (offset_to_next_symbol < 2)
            var_size = 1;
    }

    int var_value = 0;
    for (int i = 0; i < var_size; i++)
        var_value |= Debugger_Bus_Read(BREAKPOINT_LOCATION_CPU, (symbol->cpu_addr+i)&0xffff) << (i * 8);

    //char binary_s[2][9];
    //Write_Bits_Field((var_value >> 0) & 0xFF, 8, binary_s[0]);
    //Write_Bits_Field((var_value >> 8) & 0xFF, 8, binary_s[1]);

    //if (var_size == 1)
    Debugger_Printf(" %04X: %-28s = %-*s$%0*hX  dec: %d\n", symbol->cpu_addr, symbol->name, (2-var_size)*2, "", var_size*2, var_value, var_value);
    //else
    //  Debugger_Printf(" %04X  %s = $%0*hX  dec: %d, bin: %%%s.%s\n", symbol->addr, symbol->name, var_size, var_value, var_value, binary_s[0], binary_s[1]);
}

void    Debugger_Symbols_Vars_ListByName(char *search_name)
{
    if (search_name)
    {
        Debugger_Printf("Variables matching \"%s\":\n", search_name);
        search_name = strdup(search_name);
        StrUpper(search_name);
    }
    else
    {
        Debugger_Printf("Variables:\n");
    }

    int ram_len;
    int ram_start_addr;
    Mapper_Get_RAM_Infos(&ram_len, &ram_start_addr);

    int count = 0;
    for (t_list* symbols = Debugger.symbols; symbols != NULL; symbols = symbols->next)
    {
        const t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;

        const bool is_in_ram = symbol->cpu_addr >= ram_start_addr && symbol->cpu_addr < ram_start_addr+ram_len;
        if (!is_in_ram)
            continue;

        // If search_name was specified, skip symbol not matching it
        if (search_name != NULL)
            if (strstr(symbol->name_uppercase, search_name) == NULL)
                continue;
        count++;

        Debugger_Symbols_Vars_Print(symbol, symbols->next);
    }

    if (count == 0)
    {
        Debugger_Printf(" <None>\n");
    }
    if (search_name != NULL)
    {
        // Free the uppercase duplicate we made
        free(search_name);
    }
}

void    Debugger_Symbols_Vars_ListByAddr(u32 addr)
{
    addr &= 0xFFFF;

    Debugger_Printf("Variables at address \"%04x\":\n", addr);

    if (Debugger.symbols_cpu_space[addr] == NULL)
    {
        Debugger_Printf(" <None>\n");
    }
    else
    {
        for (const t_list* symbols = Debugger.symbols_cpu_space[addr]; symbols != NULL; symbols = symbols->next)
        {
            t_debugger_symbol *symbol = (t_debugger_symbol*)symbols->elem;
            Debugger_Symbols_Vars_Print(symbol, symbols->next);
        }
    }
}

const t_debugger_symbol *     Debugger_Symbols_GetFirstByAddr(u32 cpu_addr)
{
    const int rom_addr = Debugger_ReverseMapFindRomAddress((u16)cpu_addr, NULL);
    for (const t_list* symbols = Debugger.symbols_cpu_space[(u16)cpu_addr]; symbols; symbols = symbols->next)
    {
        const t_debugger_symbol* symbol = (const t_debugger_symbol *)symbols->elem;
        if (rom_addr == -1 || symbol->rom_addr == -1 || rom_addr == symbol->rom_addr)
            return symbol;
    }
    return NULL;
}

// This function is useful because there's often cases where the programmer sets 
// one 'end' symbol and a following 'start' symbol and they are at the same address.
// In most of those cases, we want the last one.
const t_debugger_symbol *     Debugger_Symbols_GetLastByAddr(u32 cpu_addr)
{
    const int rom_addr = Debugger_ReverseMapFindRomAddress((u16)cpu_addr, NULL);
    const t_debugger_symbol* last_valid_symbol = NULL;
    for (const t_list* symbols = Debugger.symbols_cpu_space[(u16)cpu_addr]; symbols; symbols = symbols->next)
    {
        const t_debugger_symbol* symbol = (const t_debugger_symbol *)symbols->elem;
        if (rom_addr == -1 || symbol->rom_addr == -1 || rom_addr == symbol->rom_addr)
            last_valid_symbol = symbol;
    }
    return last_valid_symbol;
}

const t_debugger_symbol *       Debugger_Symbols_GetClosestPreviousByAddr(u32 addr, int range)
{
    while (range >= 0)
    {
        const t_debugger_symbol * symbol = Debugger_Symbols_GetLastByAddr(addr);
        if (symbol != NULL)
            return symbol;
        addr--;
        range--;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// FUNCTIONS - SYMBOL
//-----------------------------------------------------------------------------

t_debugger_symbol *     Debugger_Symbol_Add(u16 cpu_addr, int bank, const char *name)
{
    // Check parameters
    assert(name != NULL);

    // Create and setup symbol
    t_debugger_symbol* symbol = new t_debugger_symbol();
    symbol->cpu_addr = cpu_addr;
    symbol->rom_addr = (u32)-1;
    if (bank == 0)
        symbol->rom_addr = symbol->cpu_addr;
    if (bank > 0)
        symbol->rom_addr = ((bank * 0x4000) + (symbol->cpu_addr & 0x3fff));
    symbol->bank = bank;
    symbol->name = strdup(name);
    symbol->name_uppercase = strdup(name);
    StrUpper(symbol->name_uppercase);

    // Add to global symbol list and CPU space list
    list_add(&Debugger.symbols, symbol);
    list_add_to_end(&Debugger.symbols_cpu_space[symbol->cpu_addr], symbol);

    // Increase global counter
    Debugger.symbols_count++;

    return (symbol);
}

void    Debugger_Symbol_Remove(t_debugger_symbol *symbol)
{
    // Check parameters
    assert(symbol != NULL);
    assert(symbol->name);
    assert(symbol->name_uppercase);

    // Remove from global symbol list and CPU space list
    list_remove(&Debugger.symbols, symbol);
    list_remove(&Debugger.symbols_cpu_space[symbol->cpu_addr], symbol);

    // Delete
    free(symbol->name);
    free(symbol->name_uppercase);
    delete symbol;

    // Decrease global counter
    Debugger.symbols_count--;
    assert(Debugger.symbols_count >= 0);
}

int     Debugger_Symbol_CompareByRomOrCpuAddress(const t_debugger_symbol* symbol1, const t_debugger_symbol* symbol2)
{
    const u32 addr1 = symbol1->rom_addr == -1 ? symbol1->cpu_addr : symbol1->rom_addr;
    const u32 addr2 = symbol2->rom_addr == -1 ? symbol2->cpu_addr : symbol2->rom_addr;
    return (addr1 - addr2);
}

//-----------------------------------------------------------------------------
// FUNCTIONS - HOOKS
//-----------------------------------------------------------------------------

// Hook Z80 read/write and I/O
void     Debugger_Hooks_Install()
{
    RdZ80 = Debugger_RdZ80_Hook;
    WrZ80 = Debugger_WrZ80_Hook;
    InZ80 = Debugger_InZ80_Hook;
    OutZ80 = Debugger_OutZ80_Hook;
}

// Unhook Z80 read/write and I/O
void     Debugger_Hooks_Uninstall()
{
    RdZ80 = RdZ80_NoHook;
    WrZ80 = WrZ80_NoHook;
    InZ80 = InZ80_NoHook;
    OutZ80 = OutZ80_NoHook;
}

void        Debugger_WrZ80_Hook(register u16 addr, register u8 value)
{
    for (t_list* breakpoints = Debugger.breakpoints_cpu_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
    WrZ80_NoHook(addr, value);
}

u8          Debugger_RdZ80_Hook(register u16 addr)
{
    const u8 value = RdZ80_NoHook(addr);
    for (t_list* breakpoints = Debugger.breakpoints_cpu_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Special case, if a X handler is installed and we're now executing, ignore R breakpoint
            // It is not logical but much better for end user, who is likely to use RWX in most cases
            if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            {
                if (addr >= Debugger.pc_last && addr <= Debugger.pc_last + 6)   // quick check to 6
                    if (addr <= Debugger.pc_last + Z80_Disassemble(NULL, Debugger.pc_last, false, false, false))
                        continue;
            }

            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_R, addr, value);
        }        
    }
    return (value);
}

static void     Debugger_OutZ80_Hook(register u16 addr, register u8 value)
{
    for (t_list* breakpoints = Debugger.breakpoints_io_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
    OutZ80_NoHook(addr, value);
}

static u8       Debugger_InZ80_Hook(register u16 addr)
{
    const u8 value = InZ80_NoHook(addr);
    for (t_list* breakpoints = Debugger.breakpoints_io_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_R, addr, value);
        }        
    }
    return (value);
}

void            Debugger_RasterLine_Hook(register int line)
{
    for (t_list* breakpoints = Debugger.breakpoints_line_space[line]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;

        // Verbose break/watch point result
        Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_E, line, 0);
    }
}

void            Debugger_RdVRAM_Hook(register int addr, register u8 value)
{
    for (t_list* breakpoints = Debugger.breakpoints_vram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_R, addr, value);
        }        
    }
}

void            Debugger_WrVRAM_Hook(register int addr, register u8 value)
{
    for (t_list* breakpoints = Debugger.breakpoints_vram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
}

void            Debugger_WrPRAM_Hook(register int addr, register u8 value)
{
    for (t_list* breakpoints = Debugger.breakpoints_pram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
}

// Enable or disable the debugger window and associated processing.
void    Debugger_Switch()
{
    // Msg(MSGT_DEBUG, "Debugger_Switch()");
    if (!Debugger.enabled)
        return;

    Debugger.active ^= 1;
    gui_box_show(DebuggerApp.box, Debugger.active, true);
    if (Debugger.active)
    {
        gui_menu_check(menus_ID.debug, 0);
        Machine_Debug_Start();
        // ??
        // Meka_Z80_Debugger_SetBreakPoint (Debugger.break_point_address);
    }
    else
    {
        gui_menu_uncheck_range(menus_ID.debug, 0, 0);
        Machine_Debug_Stop();
        sms.R.Trap = 0xFFFF;
    }

    // Setup/disable hooks
    if (Debugger.active)
    {
        Debugger_Hooks_Install();
    }
    else
    {
        Debugger_Hooks_Uninstall();
    }

    // Open log file (if not already open)
    if (Debugger.active)
        Debugger_Init_LogFile();
}

void        Debugger_PrintEx(bool debugger, bool log, bool ui, char* buf)
{
    // Output to debug console
    if (debugger)
    {
#ifdef ARCH_WIN32
        OutputDebugString(buf);
#endif
    }

    // Log to file
    if (log)
    if (Debugger.log_file != NULL)
    {
        fprintf(Debugger.log_file, "%s", buf);
        fflush(Debugger.log_file);
    }

    if (ui)
    {
        widget_textbox_print_scroll(DebuggerApp.console, TRUE, buf);
    }
}

// Print a formatted line to the debugger console
void        Debugger_Printf(const char *format, ...)
{
    char    buf[1024];
    va_list param_list;

    va_start(param_list, format);
    vsprintf(buf, format, param_list);
    va_end(param_list);

    Debugger_PrintEx(true, true, true, buf);
}

// Initialize the debugger applet
static void Debugger_Applet_Init()
{
    t_debugger_app* app = &DebuggerApp;

    t_frame frame;

    // Create box
    app->font_id = (t_font_id)g_config.font_debugger;
    app->font_height = Font_Height(app->font_id);
    frame.pos.x     = 428;
    frame.pos.y     = 50;
    frame.size.x    = 380;
    frame.size.y    = 600;

    app->box = gui_box_new(&frame, DEBUGGER_APP_TITLE);
    app->box_gfx = app->box->gfx_buffer;
    app->box->flags |= GUI_BOX_FLAGS_FOCUS_INPUTS_EXCLUSIVE;        // Set exclusive inputs flag to avoid messing with emulation
    app->box->flags |= GUI_BOX_FLAGS_TAB_STOP;                      // CTRL+TAB stops here
    app->box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;                  // Can be resized
    app->box->size_min.x = 220;
    app->box->size_min.y = 150;

    // Register to desktop (applet is disabled by default)
    Desktop_Register_Box("DEBUGGER", app->box, FALSE, &Debugger.active);

    // Layout
    Debugger_Applet_Layout(TRUE);
}

static void     Debugger_Applet_Layout(bool setup)
{
    t_debugger_app* app = &DebuggerApp;

    t_frame     frame;

    // Resize
    int contents_y = app->box->frame.size.y;
    contents_y -= (1 + 1 + DEBUGGER_APP_CPUSTATE_LINES) * app->font_height;
    contents_y -= app->font_height + 14; // Shortcut
    contents_y -= app->font_height + 10; // Input box
    contents_y -= 2*3;                   // Padding

    // Clear
    app->box_gfx = app->box->gfx_buffer;
    al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(app->box, (t_widget_callback)Debugger_Switch);

    // Add console (textbox widget)
    frame.pos.x = 6;
    frame.pos.y = 2;
    frame.size.x = app->box->frame.size.x - (6*1);
    frame.size.y = (contents_y * 0.5f);
    if (setup)
        app->console = widget_textbox_add(app->box, &frame, app->font_id);
    else
        app->console->frame = frame;
    frame.pos.y += frame.size.y;

    // Add line
    al_set_target_bitmap(app->box_gfx);
    al_draw_hline(frame.pos.x, frame.pos.y + app->font_height / 2, frame.pos.x + frame.size.x, COLOR_SKIN_WINDOW_SEPARATORS);
    frame.pos.y += app->font_height;

    // Setup disassembly frame
    app->frame_disassembly.pos.x   = frame.pos.x;
    app->frame_disassembly.pos.y   = frame.pos.y;
    app->frame_disassembly.size.x  = frame.size.x;
    app->frame_disassembly.size.y  = contents_y * 0.5f;
    app->disassembly_lines = MAX(0, (int)floorf(app->frame_disassembly.size.y / (app->font_height+1)));
    frame.pos.y += app->frame_disassembly.size.y;

    // Add line
    al_set_target_bitmap(app->box_gfx);
    al_draw_hline(frame.pos.x, frame.pos.y + app->font_height / 2, frame.pos.x + frame.size.x, COLOR_SKIN_WINDOW_SEPARATORS);
    frame.pos.y += app->font_height;

    // Shortcuts
    app->frame_shortcuts.pos.x   = frame.pos.x;
    app->frame_shortcuts.pos.y   = frame.pos.y;
    app->frame_shortcuts.size.x  = frame.size.x;
    app->frame_shortcuts.size.y  = app->font_height + 14;
    {
        // FIXME: the code in Debugger_Applet_UpdateShortcuts() is pretty hard-coded for the 5 entries below for now (to update their active state), maybe strcmp the button
        app->shortcuts_freeze = 0;
        static const t_debugger_shortcut shortcuts_def[] =
        {
            { "Cont", "C", },
            { "Step", "STEPINTO" },
            { "Step Over", "S" },
            { "Step Out",  "SO" },
            { "Stack", "STACK" },
            //{ "Breakpoints", "B L" },
            //{ "Regs", "REGS" },
        };
        int button_w = 0;
        for (int i = 0; i < countof(shortcuts_def); i++)
        {
            const t_debugger_shortcut* sh_def = &shortcuts_def[i];
            button_w = MAX(button_w, Font_TextWidth(app->font_id, sh_def->name));
        }
        button_w += 8;

        if (setup)
        {
            assert(app->shortcuts.empty());
        }
        DrawCursor dc(app->frame_shortcuts.pos, app->font_id);
        for (int i = 0; i < countof(shortcuts_def); i++)
        {
            const t_debugger_shortcut* sh_def = &shortcuts_def[i];

            t_debugger_shortcut sh;
            sh.name = sh_def->name;
            sh.command = sh_def->command;

            //t_frame frame(dc.pos, v2i(Font_TextLength(F_LARGE, sh.name) + 3, Font_Height(F_LARGE) + 3));
            t_frame frame(dc.pos, v2i(button_w, app->font_height + 10));
            if (setup)
            {
                sh.button = widget_button_add(app->box, &frame, 1, Debugger_ShortcutButton_Callback, app->font_id, (const char *)sh.name, (void*)i);
                app->shortcuts.push_back(sh);
            }
            else
            {
                app->shortcuts[i].button->frame = frame;
            }
            dc.pos.x += frame.size.x + 2;
        }
    }
    frame.pos.y += app->frame_shortcuts.size.y;

    // Setup CPU state frame
    app->frame_cpustate.pos.x   = frame.pos.x;
    app->frame_cpustate.pos.y   = frame.pos.y;
    app->frame_cpustate.size.x  = frame.size.x;
    app->frame_cpustate.size.y  = DEBUGGER_APP_CPUSTATE_LINES * app->font_height;
    frame.pos.y += app->frame_cpustate.size.y;

    // Add input box
    frame.size.x = app->box->frame.size.x - 16; // Leave room for resize grip
    frame.size.y = app->font_height + 8;
    frame.pos.x = 4;
    frame.pos.y = app->box->frame.size.y - frame.size.y - 2;
    if (setup)
    {
        app->input_box = widget_inputbox_add(app->box, &frame, 56, app->font_id, Debugger_InputBoxCallback);
        widget_inputbox_set_flags(app->input_box, WIDGET_INPUTBOX_FLAGS_COMPLETION, TRUE);
        widget_inputbox_set_callback_completion(app->input_box, Debugger_CompletionCallback);
        widget_inputbox_set_flags(app->input_box, WIDGET_INPUTBOX_FLAGS_HISTORY, TRUE);
        widget_inputbox_set_callback_history(app->input_box, Debugger_History_Callback);
    }
    else
    {
        app->input_box->frame = frame;
    }
}

//-----------------------------------------------------------------------------
// Debugger_Disassemble_Format(char *dst, u16 addr, bool cursor)
//-----------------------------------------------------------------------------
// Disassemble one instruction at given address and produce a formatted 
// string with address, opcode and instruction description.
// Return instruction length.
//-----------------------------------------------------------------------------
int         Debugger_Disassemble_Format(char *dst, u16 addr, bool cursor)
{
    char  instr[256];
    const int len = Z80_Disassemble(instr, addr, true, true, true);
    if (dst != NULL)
    {
        char instr_opcodes[128];
        for (int i = 0; i < len; i++)
            sprintf(instr_opcodes + (i*3), "%02X ", RdZ80_NoHook ((addr + i) & 0xFFFF));
        sprintf(dst, "%04X: %-12s%c%s", addr, instr_opcodes, cursor ? '>' : ' ', instr);
    }

    return (len);
}

//-----------------------------------------------------------------------------
// Debugger_GetZ80SummaryLines(const char ***lines_out, bool simpl)
// Return array of string pointer containing a Z80 summary
//-----------------------------------------------------------------------------
// Note: output array contains reference to static buffers. 
// Be sure to make a copy if you want to reuse later.
//-----------------------------------------------------------------------------
static int  Debugger_GetZ80SummaryLines(char *** const lines_out, bool simple)
{
    static char     line1[256];
    static char     line2[256];
    static char     line3[256];
    static char *   lines[4] = { line1, line2, line3, NULL };
    char            flags[9];
    Z80 *           cpu = &sms.R;

    // Compute flags string
    int i;
    for (i = 0; i < 8; i++)
        flags[i] = (cpu->AF.B.l & (1 << (7 - i))) ? "SZyHxPNC"[i] : '.';
    flags[i] = EOSTR;

    *lines_out = &lines[0];

    if (simple)
    {
        // Line 1
        sprintf(line1, "AF:%04X  BC:%04X  DE:%04X  HL:%04X  IX:%04X  IY:%04X",
            cpu->AF.W, cpu->BC.W, cpu->DE.W, cpu->HL.W, cpu->IX.W, cpu->IY.W);

        // Line 2
        sprintf(line2, "PC:%04X  SP:%04X  Flags:[%s]  R:%02X  %s%s", 
            cpu->PC.W, cpu->SP.W, flags, (cpu->R & 0x7F) | (cpu->R7),
            (cpu->IFF & IFF_1) ? "EI" : "DI", (cpu->IFF & IFF_HALT) ? " HALT" : "");

        return (2);
    }
    else
    {
        // Line 1
        sprintf(line1, "AF:%04X  BC:%04X  DE:%04X  HL:%04X  IX:%04X  IY:%04X",
            cpu->AF.W, cpu->BC.W, cpu->DE.W, cpu->HL.W, cpu->IX.W, cpu->IY.W);

        // Line 2
        sprintf(line2, "AF'%04X  BC'%04X  DE'%04X  HL'%04X",
            cpu->AF1.W, cpu->BC1.W, cpu->DE1.W, cpu->HL1.W);

        // Line 3
        sprintf(line3, "PC:%04X  SP:%04X  Flags:[%s]  R:%02X  %s%s", 
            cpu->PC.W, cpu->SP.W, flags, (cpu->R & 0x7F) | (cpu->R7),
            (cpu->IFF & IFF_1) ? "EI" : "DI", (cpu->IFF & IFF_HALT) ? " HALT" : "");

        return (3);
    }
}

// Redraw disassembly and CPU state
void    Debugger_Applet_RedrawState()
{
    t_debugger_app* app = &DebuggerApp;

    // Mouse wheel scroll disassembly
    if (!(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)))
        if (g_machine_flags & MACHINE_POWER_ON)
            Debugger.trackback_scroll_offset = 0;
    if (gui_box_has_focus(app->box))
    {
        if (gui.mouse.wheel_rel)
        {
            const int wheel_speed = (g_keyboard_modifiers & ALLEGRO_KEYMOD_CTRL) ? 5 : 1;
            Debugger.trackback_scroll_offset += gui.mouse.wheel_rel * wheel_speed;
        }
    }

    // Redraw Disassembly
    if ((g_machine_flags & MACHINE_POWER_ON) && (g_driver->cpu == CPU_Z80))
    {
        t_frame frame = app->frame_disassembly;

        u16     pc;
        int     skip_labels = 0;    // Number of labels to skip on first instruction to be aligned properly
        int     trackback_lines_base = (app->disassembly_lines / 3) + 1; 
        trackback_lines_base = MIN(trackback_lines_base, 16); // Max 10 because of buffer below
        //  1 -> 1
        //  5 -> 2
        //  9 -> 3, etc.
        // 13 -> 4
        // Max = 10 ?

        // Clear disassembly buffer
        al_set_target_bitmap(app->box_gfx);
        al_draw_filled_rectangle(frame.pos.x, frame.pos.y, frame.pos.x+frame.size.x+1, frame.pos.y+frame.size.y+1, COLOR_SKIN_WINDOW_BACKGROUND);

        // Figure out where to start disassembly
        // This is tricky code due to the trackback feature.
        // Successive PC are logged by the debugging CPU emulator and it helps with the trackback.
        pc = sms.R.PC.W;

        // Scroll
        int trackback_lines = trackback_lines_base + Debugger.trackback_scroll_offset;
        
        if (trackback_lines < 0)
        {
            // Forward
            while (trackback_lines < 0)
            {
                int inst_len = Z80_Disassemble(NULL, (u16)pc, false, false, false);
                if ((int)pc + inst_len <= 0xffff)
                    pc += inst_len;
                trackback_lines++;
            }
        }

        if (trackback_lines > 0)
        {
            while (trackback_lines > 0)
            {
                int pc_trackback = pc;
                for (int b = 1; b < 7; b++)     // ~Maximum 7 bytes per op
                {
                    int inst_start = (int)pc - b;
                    if (inst_start < 0)
                        break;
                    
                    int inst_len = Debugger.pc_exec_points[inst_start];
                    if (inst_len == 0)
                        continue;

                    // lazily convert 0xff stored by CPU convert to opcode len
                    // zero out following bytes of the same instruction, so that in case of instructions sharing byte, the last executed one will override.
                    if (inst_len == 0xff)
                    {
                        inst_len = Z80_Disassemble(NULL, (u16)(inst_start), false, false, false);
                        Debugger.pc_exec_points[inst_start] = inst_len;
                        for (int z = 1; z < inst_len; z++)
                            Debugger.pc_exec_points[inst_start + z] = 0;
                    }

                    if (inst_start + inst_len == pc)
                    {
                        pc_trackback = inst_start;
                        trackback_lines--;

                        // account for labels showing in disassembler
                        if (g_config.debugger_disassembly_display_labels)
                            if (Debugger.symbols_cpu_space[pc_trackback])
                                trackback_lines -= list_size(Debugger.symbols_cpu_space[pc_trackback]);
                    }
                    break;
                }

                if (pc == pc_trackback)
                {
                    // clamp user scroll offset
                    if (Debugger.trackback_scroll_offset != 0)
                        Debugger.trackback_scroll_offset -= trackback_lines;
                        //MIN(Debugger.trackback_scroll_offset, trackback_lines - (trackback_lines_base + Debugger.trackback_scroll_offset));
                    break;
                }
                pc = pc_trackback;
            }
            if (trackback_lines < 0)
                skip_labels = -trackback_lines;
        }

        // label_a:
        //  XOR
        //  LD
        //  LD
        //  LDIR
        // label_b:
        //  JP
        // label_c:
        // label_d:
        //  LD
        //  DEC
        //  JR NZ

        // Disassemble instructions starting at 'PC'
        for (int i = 0; i < app->disassembly_lines; i++)
        {
            char buf[256];
            const ALLEGRO_COLOR text_color = (pc == sms.R.PC.W) ? COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT : COLOR_SKIN_WINDOW_TEXT;
            int opcode_size;

            if (g_config.debugger_disassembly_display_labels)
            {
                bool pc_in_bios;
                const int pc_rom_addr = Debugger_ReverseMapFindRomAddress(pc, &pc_in_bios);

                // Display symbols/labels
                if (Debugger.symbols_cpu_space[pc] != NULL)
                {
                    for (const t_list* symbols = Debugger.symbols_cpu_space[pc]; symbols != NULL; symbols = symbols->next)
                    {
                        const t_debugger_symbol* symbol = (t_debugger_symbol*)symbols->elem;
                        if (skip_labels > 0)
                        {
                            skip_labels--;
                            continue;
                        }

                        if (pc_rom_addr != -1 && symbol->rom_addr != -1 && symbol->rom_addr != pc_rom_addr)
                            continue;

                        sprintf(buf, "%s:", symbol->name);
                        Font_Print(app->font_id, buf, frame.pos.x, frame.pos.y + (i * (app->font_height+1)), COLOR_SKIN_WINDOW_TEXT);
                        i++;
                        if (i >= app->disassembly_lines)
                            break;
                    }
                    if (i >= app->disassembly_lines)
                        break;
                }
            }

            //// The trick here is to re-add all disassembled instruction into the PC log queue
            //Debugger_Z80_PC_Log_Queue_Add(pc);

            // Disassemble
            //if (g_config.debugger_disassembly_display_labels && Debugger.symbols_count != 0)
            buf[0] = ' ';
            opcode_size = Debugger_Disassemble_Format(buf + 1, pc, pc == sms.R.PC.W);

            // Breakpoints
            {
                bool on_breakpoint = false;
                for (t_list* breakpoints = Debugger.breakpoints_cpu_space[pc]; breakpoints != NULL; breakpoints = breakpoints->next)
                {
                    t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
                    if (breakpoint->location == BREAKPOINT_LOCATION_ROM)    // FIXME: Support the '!' indicator for ROM breakpoints
                        continue;
                    if (breakpoint->type == BREAKPOINT_TYPE_BREAK)
                        on_breakpoint = true;
                }
                if (on_breakpoint)
                {
                    buf[0] = '!';
                }
            }

            Font_Print(app->font_id, buf, frame.pos.x, frame.pos.y + (i * (app->font_height+1)), text_color);

            pc += opcode_size;
        }
    }

    // Redraw CPU State
    if (g_driver->cpu == CPU_Z80)
    {
        // Clear CPU state buffer
        t_frame frame = app->frame_cpustate;
        al_set_target_bitmap(app->box_gfx);
        al_draw_filled_rectangle(frame.pos.x, frame.pos.y, frame.pos.x + frame.size.x+1, frame.pos.y + frame.size.y+1, COLOR_SKIN_WINDOW_BACKGROUND);
        int y = frame.pos.y;

        // Print Z80 summary lines
        char ** lines;
        const int lines_count = Debugger_GetZ80SummaryLines(&lines, TRUE); 
        assert(lines_count >= DEBUGGER_APP_CPUSTATE_LINES); // Display first 2 lines
        Font_Print(app->font_id, lines[0], frame.pos.x, y, COLOR_SKIN_WINDOW_TEXT);
        y += app->font_height;
        Font_Print(app->font_id, lines[1], frame.pos.x, y, COLOR_SKIN_WINDOW_TEXT);
        
        // Print Z80 running state with nifty ASCII rotating animation
        if (!(g_machine_flags & (MACHINE_PAUSED | MACHINE_DEBUGGING)))
        {
            const char *label = "OFF";
            if (g_machine_flags & MACHINE_POWER_ON)
            {
                static int running_counter = 0;
                switch (running_counter >> 1)
                {
                    case 0: label = "RUNNING |";  break;
                    case 1: label = "RUNNING /";  break;
                    case 2: label = "RUNNING -";  break;
                    case 3: label = "RUNNING \\"; break;
                }
                running_counter = (running_counter + 1) % 8;
            }
            const int w = Font_TextWidth(app->font_id, lines[1]) + Font_TextWidth(app->font_id, "  ");
            Font_Print(app->font_id, label, frame.pos.x + w, y, COLOR_SKIN_WINDOW_TEXT);
        }

    }
}

//-----------------------------------------------------------------------------
// Debugger_Help(const char *cmd)
//-----------------------------------------------------------------------------
// Print help for debugger or for a given command.
//-----------------------------------------------------------------------------
static void     Debugger_Help(const char *cmd)
{
    if (cmd == NULL)
    {
        // Generic help
        Debugger_Printf("Debugger Help:\n");
        Debugger_Printf("-- Flow:\n");
        Debugger_Printf(" <Enter>                : Step into"                  "\n");
        Debugger_Printf(" S                      : Step over"                  "\n");
        Debugger_Printf(" SO                     : Step out (up to next RET)"   "\n");
        Debugger_Printf(" C [addr]               : Continue (up to <addr>)"    "\n");
        Debugger_Printf(" J addr                 : Jump to <addr>"             "\n");
        Debugger_Printf("-- Breakpoints:\n");
        Debugger_Printf(" B [access] [bus] addr  : Add breakpoint"             "\n");
        Debugger_Printf(" W [access] [bus] addr  : Add watchpoint"             "\n");
        //Debugger_Printf(" B                      : Detailed breakpoint help"   "\n");
        //Debugger_Printf(" B LIST                 : List breakpoints"          "\n");
        //Debugger_Printf(" B REMOVE n             : Remove breakpoint"         "\n");
        //Debugger_Printf(" B ENABLE/DISABLE n     : Enable/disable breakpoint" "\n");
        //Debugger_Printf(" B CLEAR                : Clear breakpoints"         "\n");
        Debugger_Printf("-- Inspect/Modify:\n");
        Debugger_Printf(" R                      : Dump Z80 registers"         "\n");
        Debugger_Printf(" P expr                 : Print evaluated expression" "\n");
        Debugger_Printf(" M [addr] [cnt]         : Memory dump at <addr>"      "\n");
        Debugger_Printf(" D [addr] [cnt]         : Disassembly at <addr>"      "\n");
        Debugger_Printf(" STACK [cnt]            : Stack dump"                 "\n");
        Debugger_Printf(" TRACE [cnt|clear|regs] : Trace past execution"       "\n");
        Debugger_Printf(" RMAP addr              : Reverse map Z80 address"    "\n");
        Debugger_Printf(" VARS [name|@addr]      : Display variables"          "\n");
        Debugger_Printf(" SYM [name|@addr]       : Find symbols"               "\n");
        Debugger_Printf(" SET register=value     : Set Z80 register"           "\n");
        Debugger_Printf(" CLOCK [RESET]          : Display Z80 cycle counter"  "\n");
        Debugger_Printf("-- Miscellaneous:\n");
        Debugger_Printf(" MEMEDIT [lines] [cols] : Spawn memory editor"        "\n");
        Debugger_Printf(" HISTORY [word]         : Print/search history"       "\n");
        Debugger_Printf(" H,? [command]          : Help"                       "\n");
        Debugger_Printf("Use H for detailed help on individual command."       "\n");
    }
    else
    {
        // Search for specific command
        t_debugger_command_info *command_info = &DebuggerCommandInfos[0];
        while (command_info->command_long != NULL)
        {
            if ((command_info->command_short && !stricmp(cmd, command_info->command_short)) || (command_info->command_long && !stricmp(cmd, command_info->command_long)))
            {
                Debugger_Printf("%s\n", command_info->description);
                return;
            }
            command_info++;
        }
        Debugger_Printf("Unknown command \"%s\" !\n", cmd);
    }
}

void        Debugger_InputParseCommand_BreakWatch(char *line, int type)
{
    char    arg[256];

    int                 access = 0;
    int                 location = -1;
    t_debugger_value    address_start;
    t_debugger_value    address_end;
    int                 data_compare_length = 0;
    u8                  data_compare_bytes[DEBUGGER_DATA_COMPARE_LENGTH_MAX] = { 0 };
    char *              desc = NULL;

    if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
    {
        if (type == BREAKPOINT_TYPE_BREAK)
            Debugger_Help("B");
        else if (type == BREAKPOINT_TYPE_WATCH)
            Debugger_Help("W");
        return;
    }

    // B NOPNOP -> B x =0,0 "(NOP NOP)"
    // FIXME-WIP: Of course, a generic macro system would be welcome as well.
    if (!stricmp(arg, "nopnop"))
    {
        static char break_nopnop_string[] = "x =0,0 \"(NOP NOP)\"";
        line = break_nopnop_string;
        parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE);
    }
        
    // B LIST
    if (!stricmp(arg, "l") || !stricmp(arg, "list"))
    {
        Debugger_BreakPoints_List();
        return;
    }

    // B CLEAR
    if (!stricmp(arg, "clear"))
    {
        if (parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            if (!stricmp(arg, "disabled"))
            {
                Debugger_BreakPoints_Clear(TRUE);
                return;
            }
            Debugger_Printf("Syntax error!\n");
            Debugger_Printf("Type HELP B for usage instruction.\n");
            return;
        }
        Debugger_BreakPoints_Clear(FALSE);
        return;
    }

    // B ENABLE
    if (!stricmp(arg, "enable"))
    {
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            Debugger_Printf("Missing parameter!\n");
        }
        else
        {
            if (!stricmp(arg, "ALL"))
            {
                for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
                {
                    t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
                    Debugger_BreakPoint_Enable(breakpoint);
                }
                Debugger_Printf("Enabled all breakpoints/watchpoints.\n");
            }
            else
            {
                int id = atoi(arg);
                t_debugger_breakpoint* breakpoint = Debugger_BreakPoints_SearchById(id);
                if (breakpoint)
                {
                    char buf[256];
                    if (breakpoint->enabled)
                    {
                        Debugger_Printf("%s [%d] already enabled.\n", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    else
                    {
                        Debugger_BreakPoint_Enable(breakpoint);
                        Debugger_Printf("%s [%d] enabled.\n", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
                    Debugger_Printf(" %s\n", buf);
                }
                else
                {
                    Debugger_Printf("Breakpoint [%s] not found!\n", arg);
                }
            }
        }
        return;
    }

    // B DISABLE
    if (!stricmp(arg, "disable"))
    {
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            Debugger_Printf("Missing parameter!\n");
        }
        else
        {
            if (!stricmp(arg, "ALL"))
            {
                for (t_list* breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
                {
                    t_debugger_breakpoint* breakpoint = (t_debugger_breakpoint*)breakpoints->elem;
                    Debugger_BreakPoint_Disable(breakpoint);
                }
                Debugger_Printf("Disabled all breakpoints/watchpoints.\n");
            }
            else
            {
                int id = atoi(arg); // FIXME: no error check
                t_debugger_breakpoint* breakpoint = Debugger_BreakPoints_SearchById(id);
                if (breakpoint)
                {
                    char buf[256];
                    if (!breakpoint->enabled)
                    {
                        Debugger_Printf("%s [%d] already disabled.\n", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    else
                    {
                        Debugger_BreakPoint_Disable(breakpoint);
                        Debugger_Printf("%s [%d] disabled.\n", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
                    Debugger_Printf(" %s\n", buf);
                }
                else
                {
                    Debugger_Printf("Breakpoint [%s] not found!\n", arg);
                }
            }
        }
        return;
    }

    // B REMOVE
    if (!stricmp(arg, "remove"))
    {
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            Debugger_Printf("Missing parameter!\n");
        }
        else
        {
            int id = atoi(arg); // FIXME: no error check
            t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
            if (breakpoint)
            {
                Debugger_Printf("%s [%d] removed.\n", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                Debugger_BreakPoint_Remove(breakpoint);
            }
            else
            {
                Debugger_Printf("Breakpoint [%s] not found!\n", arg);
            }
        }
        return;
    }

    // If no known argument, revert to adding breakpoint

    // Parse Access
    {
        char *p = arg;
        char  c;
        while ((c = *p++) != EOSTR)
        {
            if (c == 'r' || c == 'R')
                access |= BREAKPOINT_ACCESS_R;
            else if (c == 'w' || c == 'W')
                access |= BREAKPOINT_ACCESS_W;
            else if (c == 'x' || c == 'X')
                access |= BREAKPOINT_ACCESS_X;
            else if (c == 'e' || c == 'E')
                access |= BREAKPOINT_ACCESS_E;
            else
            {
                access = 0;
                break;
            }
        }
        if (type == BREAKPOINT_TYPE_WATCH && (access & BREAKPOINT_ACCESS_X)) // Watch
        {
            Debugger_Printf("Cannot watch execution. Use breakpoints.\n");
            return;
        }
    }
    if (access != 0)
    {
        // Get next argument
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            Debugger_Printf("Missing parameter!\n");
            Debugger_Printf("Type HELP B/W for usage instruction.\n");
            return;
        }
    }

    // Parse Bus
    if (!stricmp(arg, "CPU"))
        location = BREAKPOINT_LOCATION_CPU;
    else if (!stricmp(arg, "ROM"))
        location = BREAKPOINT_LOCATION_ROM;
    else if (!stricmp(arg, "IO"))
        location = BREAKPOINT_LOCATION_IO;
    else if (!stricmp(arg, "VRAM"))
        location = BREAKPOINT_LOCATION_VRAM;
    else if (!stricmp(arg, "PAL") || !stricmp(arg, "PRAM"))
        location = BREAKPOINT_LOCATION_PRAM;
    else if (!stricmp(arg, "LINE"))
        location = BREAKPOINT_LOCATION_LINE;
    if (location == -1)
    {
        // Default
        location = BREAKPOINT_LOCATION_CPU;
    }
    else
    {
        // Get next argument
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            Debugger_Printf("Syntax error!\n");
            Debugger_Printf("Type HELP B/W for usage instruction.\n");
            return;
        }
    }

    // Now that we know the bus, check access validity
    if (access == 0)
    {
        // Default
        access = DebuggerBusInfos[location].access;
        if (type == BREAKPOINT_TYPE_WATCH) // Watch, automatically remove X
            access &= ~BREAKPOINT_ACCESS_X;
    }
    else
    {
        // Check user-given access rights
        int access_unpermitted = access & ~DebuggerBusInfos[location].access;
        if (access_unpermitted != 0)
        {
            char buf[5];
            Debugger_GetAccessString(access_unpermitted, buf);
            Debugger_Printf("Access %s not permitted on this bus.\n", buf);
            return;
        }
    }

    // Parse Adress(es)
    if (strcmp(arg, "..") == 0 || arg[0] == '=')
    {
        // If given address is '..' or no address but provided a data comparer, 
        // use the bus full range.
        t_debugger_bus_info *bus_info = &DebuggerBusInfos[location];
        Debugger_Value_SetDirect(&address_start, (u32)bus_info->addr_min, 16);
        Debugger_Value_SetDirect(&address_end,   (u32)bus_info->addr_max, 16);
    }
    else
    {
        t_debugger_bus_info *bus_info = &DebuggerBusInfos[location];

        // Clear out
        Debugger_Value_SetDirect(&address_start, (u32)-1, 16);
        Debugger_Value_SetDirect(&address_end,   (u32)-1, 16);

        // Parse different kind of ranges (A, A.., A..B, ..B)
        char* p = arg;
        if (Debugger_Eval_ParseExpression(&p, &address_start) > 0)
        {
            // Default is no range, so end==start
            address_start.data = address_start.data;
            address_end = address_start;
        }
        if (strncmp(p, "..", 2) == 0)
        {
            // Skip range points
            while (*p == '.')
                p++;

            // Get second part of the range
            if (address_start.data == (u32)-1)
                address_start.data = bus_info->addr_min;
            if (Debugger_Eval_ParseExpression(&p, &address_end) > 0)
                address_end.data = address_end.data;
            else
                address_end.data = bus_info->addr_max;
        }

        if (p[0] != '\0')
        {
            Debugger_Printf("Syntax error!\n");
            Debugger_Printf("Type HELP B for usage instruction.\n");
            return;
        }

        if (address_start.data == (u32)-1 || address_end.data == (u32)-1)
        {
            Debugger_Printf("Syntax error!\n");
            Debugger_Printf("Type HELP B for usage instruction.\n");
            return;
        }

        // Check out address range
        if (address_end.data < address_start.data)
        {
            Debugger_Printf("Second address in range must be higher.\n");
            return;
        }
        if (address_start.data < (u32)bus_info->addr_min || address_start.data > (u32)bus_info->addr_max)
        {
            if (bus_info->location == BREAKPOINT_LOCATION_LINE)
            {
                Debugger_Printf("Address %X is out of %s range (%d..%d).\n", 
                    address_start.data, 
                    bus_info->name, bus_info->addr_min, bus_info->addr_max);
            }
            else
            {
                Debugger_Printf("Address %X is out of %s range (%0*X..%0*X).\n", 
                    address_start.data, 
                    bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
            }
            return;
        }
        if (address_end.data < (u32)bus_info->addr_min || address_end.data > (u32)bus_info->addr_max)
        {
            if (bus_info->location == BREAKPOINT_LOCATION_LINE)
            {
                Debugger_Printf("Address %X is out of %s range (%d..%d).\n", 
                    address_end.data, 
                    bus_info->name, bus_info->addr_min, bus_info->addr_max);
            }
            else
            {
                Debugger_Printf("Address %X is out of %s range (%0*X..%0*X).\n", 
                    address_end.data, 
                    bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
            }
            return;
        }

        // Get next argument
        if (line[0] == '=')
            parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE);
    }

    // Parse Data Comparer
    if (arg[0] == '=')
    {
        char *p;
        char value_buf[128];
        const int data_compare_length_max = DebuggerBusInfos[location].data_compare_length_max;

        //parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE);
        //assert(arg[0] == '=');
        p = arg + 1;

        while (parse_getword(value_buf, sizeof(value_buf), &p, ",", 0, PARSE_FLAGS_NONE))
        {
            t_debugger_value value;
            if (data_compare_length >= data_compare_length_max)
            {
                if (data_compare_length_max == 0)
                    Debugger_Printf("Error: data comparing on this bus is not allowed!\n");
                else
                    Debugger_Printf("Error: data comparing on this bus is limited to %d bytes!\n", data_compare_length_max);
                return;
            }
            if ((access & BREAKPOINT_ACCESS_W) && (data_compare_length >= 1))
            {
                Debugger_Printf("Error: data comparing for write accesses is limited to 1 byte! Only read/execute accesses can uses more.\n");
                return;
            }
            if (!Debugger_Eval_ParseConstant(value_buf, &value))
            {
                Debugger_Printf("Syntax error!\n");
                return;
            }
            if (value.data & ~0xFF)
            {
                Debugger_Printf("Error: comparing values must be given in bytes.\n\"%s\" doesn't fit in byte.\n",
                    value_buf);
                return;
            }
            data_compare_bytes[data_compare_length++] = value.data;
        }
        
        //sscanf(p, "%02X", 

    }

    // Parse or create description
    {
        assert(desc == NULL);
        StrTrim(line);
        if (line[0] != '\0')
        {
            if (line[0] == '\"')
            {
                char *desc_end;
                line++;
                desc_end = strchr(line, '\"');
                if (desc_end != NULL)
                    *desc_end = '\0';
                desc = line;
            }
            else
            {
                Debugger_Printf("Syntax error!\n");
                Debugger_Printf("Type HELP B for usage instruction.\n");
                return;
            }
        }
        else 
        {
            // Create automatic description containing symbol
            if (address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR || address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR
             || address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR || address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR)
            {
                static char buf[512];
                if (address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR || address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR)
                    strcpy(buf, ((t_debugger_symbol *)address_start.source_data)->name);
                else
                    sprintf(buf, "%04hX", address_start.data);
                if (address_start.data != address_end.data)
                {
                    strcat(buf, "..");
                    if (address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR || address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR)
                        strcat(buf, ((t_debugger_symbol *)address_end.source_data)->name);
                    else
                        sprintf(buf+strlen(buf), "%04hX", address_end.data);
                }
                desc = buf; // Ok since Debugger_BreakPoint_Add() - called below - does a strcpy
            }
        }
    }
        
    // Add breakpoint
    {
        t_debugger_breakpoint *breakpoint;
        char buf[256];

        breakpoint = Debugger_BreakPoint_Add(type, location, access, address_start.data, address_end.data, -1, desc);
        if (data_compare_length != 0)
        {
            Debugger_BreakPoint_SetDataCompare(breakpoint, data_compare_length, data_compare_bytes);
        }
 
        // Verbose
        Debugger_Printf("%s [%d] added.\n", Debugger_BreakPoint_GetTypeName(breakpoint), breakpoint->id);
        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
        Debugger_Printf(" %s\n", buf);
    }
}

void        Debugger_StepInto()
{
    if (g_machine_flags & MACHINE_POWER_ON)
    {
        // If machine is in PAUSE state, consider is the same as in DEBUGGING state
        // (Allows step during pause)
        if ((g_machine_flags & MACHINE_DEBUGGING) || (g_machine_flags & MACHINE_PAUSED))
        {
            // Step into
            Debugger.stepping = 1;
            Debugger.stepping_trace_after = sms.R.Trace = 1;
            Debugger.stepping_out_enable = false;
            Machine_Debug_Stop();
        }
        else
        {
            // Activate debugging
            Debugger.stepping = 0;
            Debugger.stepping_out_enable = false;
            Debugger_Printf("Breaking at $%04X\n", sms.R.PC.W);
            //Debugger_Applet_RedrawState();
            Machine_Debug_Start();
            //Debugger_Hook (&sms.R);
            sms.R.Trace = 1;
        }
    }
    else
    {
        Debugger_Printf("Command unavailable while machine is not running!\n");
    }
}

void        Debugger_InputParseCommand(char* line)
{
    char    cmd[64];
    char    arg[256];

    // Process command
    parse_getword(cmd, sizeof(cmd), &line, " ", 0, PARSE_FLAGS_NONE);
    StrUpper(cmd);

    // H - HELP
    if (!strcmp(cmd, "H") || !strcmp(cmd, "?") || !strcmp(cmd, "HELP"))
    {
        if (parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
            Debugger_Help(arg);
        else
            Debugger_Help(NULL);
        return;
    }

    // HI - HISTORY
    if (!strcmp(cmd, "HI") || !strcmp(cmd, "HISTORY"))
    {
        if (!StrIsNull(line))
            Debugger_History_List(line);
        else
            Debugger_History_List(NULL);
        return;
    }

    // B - BREAKPOINT
    if (!strcmp(cmd, "B") || !strcmp(cmd, "BR") || !strcmp(cmd, "BRK") || !strcmp(cmd, "BREAK"))
    {
        Debugger_InputParseCommand_BreakWatch(line, BREAKPOINT_TYPE_BREAK);
        return;
    }
    if (!strcmp(cmd, "W") || !strcmp(cmd, "WATCH"))
    {
        Debugger_InputParseCommand_BreakWatch(line, BREAKPOINT_TYPE_WATCH);
        return;
    }

    // STEPINTO (undocumented / for shortcuts)
    if (!strcmp(cmd, "STEPINTO"))
    {
        Debugger_StepInto();
        return;
    }

    // C - CONTINUE
    if (!strcmp(cmd, "C") || !strcmp(cmd, "CONT") || !strcmp(cmd, "CONTINUE"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
            return;
        }

        if (!(g_machine_flags & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start();
        }

        t_debugger_value value;
        if (Debugger_Eval_ParseExpression(&line, &value) > 0)
        {
            // Continue up to...
            u16 addr = value.data;
            Debugger_Printf("Continuing up to $%04X\n", addr);
            Debugger_SetTrap(addr);
        }
        else
        {
            // Continue
            // Disable one-time trap
            Debugger_SetTrap(-1);
        }

        // Stop tracing
        sms.R.Trace = 0;
        Machine_Debug_Stop();

        // Setup a single stepping so that the CPU emulator won't break
        // on the same address right now.
        Debugger.stepping = 1;
        Debugger.stepping_trace_after = 0;

        return;
    }

    // SO - STEPOUT
    if (!strcmp(cmd, "SO") || !strcmp(cmd, "STEPOUT"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
            return;
        }

        if (!(g_machine_flags & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start();
        }

        // Start tracing
        sms.R.Trace = 1;
        Debugger.stepping_out_enable = true;
        Debugger.stepping_out_stack_ref = sms.R.SP.W;
        Debugger.stepping = 1;  // to avoid breaking again on current RET instruction
        Debugger.stepping_trace_after = 1;
        Machine_Debug_Stop();
        Debugger_SetTrap(-1);

        return;
    }

    // J - JUMP
    if (!strcmp(cmd, "J") || !strcmp(cmd, "JP") || !strcmp(cmd, "JUMP"))
    {
        t_debugger_value value;
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
            return;
        }
        if (!(g_machine_flags & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start();
        }
        if (Debugger_Eval_ParseExpression(&line, &value) > 0)
        {
            sms.R.PC.W = value.data;
            Debugger_Printf("Jump to $%04X\n", sms.R.PC.W);
            //Debugger_Applet_RedrawState();
        }
        else
        {
            Debugger_Printf("Missing parameter!\n");
        }
        return;
    }

    // S - STEP OVER
    if (!strcmp(cmd, "S") || !strcmp(cmd, "STEP"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            // Get address of following instruction
            // Do not verbose since this is just a step-over
            u16 addr = sms.R.PC.W + Z80_Disassemble(NULL, sms.R.PC.W, false, false, false);
            Debugger_SetTrap (addr);
            sms.R.Trace = 0;
            Machine_Debug_Stop();
        }
        return;
    }

    // P - PRINT
    if (!strcmp(cmd, "P") || !strcmp(cmd, "PRINT"))
    {
        t_debugger_value value;
        StrTrim(line);
        if (line[0])
        {
            char *p = line;
            while (*p && Debugger_Eval_ParseExpression(&p, &value) > 0)
            {
                const s32 data = value.data;
                const int data_size_bytes = Clamp<int>(value.data_size/8, 2, 4); //data & 0xFFFF0000) ? ((data & 0xFF000000) ? 4 : 3) : (2);

                // Write binary buffer
                char binary_s[32+4+1];
                StrWriteBitfield(data, data_size_bytes*8, binary_s);

                // Write ascii buffer
                char ascii_s[16];
                if (data_size_bytes == 1)
                {
                    if (isprint(data & 0xFF))
                        sprintf(ascii_s, "  asc: '%c'", data & 0xFF);
                    else
                        sprintf(ascii_s, "  asc: N/A ");
                }
                else
                    ascii_s[0] = '\0';
                Debugger_Printf(" $%0*hX  bin: %%%s%s  dec: %d\n", data_size_bytes * 2, data, binary_s, ascii_s, data);

                // Skip comma to get to next expression, if any
                if (*p == ',')
                    p++;
            }
        }
        else
        {
            Debugger_Help("P");
        }
        return;
    }

    // CLOCK
    if (!strcmp(cmd, "CLOCK"))
    {
        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            // Display clock
            Debugger_Printf("Clock: %lld cycles\n", Debugger.cycle_counter);
            return;
        }

        if (!stricmp(arg, "r") || !stricmp(arg, "reset"))
        {
            // Reset clock
            Debugger.cycle_counter = 0;
            Debugger_Printf("Clock reset\n");
            Debugger_Printf("Clock: %lld cycles\n", Debugger.cycle_counter);
            return;
        }

        Debugger_Help("CLOCK");
        return;
    }

    // SET
    if (!strcmp(cmd, "SET"))
    {
        if (!(g_machine_flags & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start();
        }

        StrTrim(line);
        if (line[0])
        {
            char *p = line;
            while (*p)
            {
                t_list *vars;
                t_debugger_value *lvalue;

                // Get variable name to assign too
                if (!parse_getword(arg, sizeof(arg), &p, "=", 0, PARSE_FLAGS_NONE))
                {
                    Debugger_Printf("Missing parameter!\n");
                    Debugger_Help("SET");
                    return;
                }

                // Search variable
                // (currently only support CPU registers)
                lvalue = NULL;
                for (vars = Debugger.variables_cpu_registers; vars != NULL; vars = vars->next)
                {
                    t_debugger_value *var = (t_debugger_value *)vars->elem;
                    if (!stricmp(var->name, arg))
                    {
                        if (!(var->flags & DEBUGGER_VALUE_FLAGS_ACCESS_WRITE))
                        {
                            Debugger_Printf("Variable \"%s\" is read-only!\n", var->name);
                            lvalue = NULL;
                        }
                        else
                        {
                            lvalue = var;
                        }
                        break;
                    }
                }
                if (lvalue != NULL)
                {
                    // Get right value
                    t_debugger_value rvalue;
                    if (Debugger_Eval_ParseExpression(&p, &rvalue) > 0)
                    {
                        // Assign
                        if (rvalue.data_size > lvalue->data_size)
                            if (rvalue.data & ~((1 << lvalue->data_size) - 1))
                                Debugger_Printf("Warning: value truncated from %d to %d bits.\n", rvalue.data_size, lvalue->data_size);
                        Debugger_Value_Write(lvalue, rvalue.data);
                    }
                    else
                    {
                        // Abort
                        return;
                    }
                }
                else
                {
                    Debugger_Printf("Unknown variable: %s\n", arg);
                }

                // Skip comma to get to next expression, if any
                if (*p == ',')
                    p++;
            }
       }
        else
        {
            Debugger_Help("SET");
        }
        return;
    }

    // R - REGS
    if (!strcmp(cmd, "R") || !strcmp(cmd, "REGS"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            char **lines;
            const int lines_count = Debugger_GetZ80SummaryLines(&lines, FALSE);
            int i;
            for (i = 0; i != lines_count; i++)
                Debugger_Printf("%s\n", lines[i]);
        }
        return;
    }

    // D - DISASSEMBLE
    if (!strcmp(cmd, "D") || !strcmp(cmd, "DASM"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            u16 addr = sms.R.PC.W;
            int len  = 10;
            t_debugger_value value;
            int expr_error;
            if ((expr_error = Debugger_Eval_ParseExpression(&line, &value)) < 0)
            {
                Debugger_Printf("Syntax error!\n");
                return;
            }
            if (expr_error > 0)
            {
                addr = value.data;
                parse_skip_spaces(&line);
                if ((expr_error = Debugger_Eval_ParseExpression(&line, &value)) < 0)
                {
                    Debugger_Printf("Syntax error!\n");
                    return;
                }
                if (expr_error > 0)
                {
                    len = value.data;
                }
            }
 
            {
                int i;
                for (i = 0; i < len; i++)
                {
                    char buf[256];
                    const int opcode_size = Debugger_Disassemble_Format(buf, addr, addr == sms.R.PC.W);

                    // Display symbols/labels (if any)
                    const int rom_addr = Debugger_ReverseMapFindRomAddress(addr, NULL);
                    for (t_list* symbols = Debugger.symbols_cpu_space[addr]; symbols != NULL; symbols = symbols->next)
                    {
                        const t_debugger_symbol* symbol = (const t_debugger_symbol*)symbols->elem;
                        if (rom_addr != -1 && symbol->rom_addr != -1 && symbol->rom_addr != rom_addr)
                            continue;
                        Debugger_Printf("%s:\n", symbol->name);
                    }

                    // Display instruction
                    Debugger_Printf(" %s\n", buf);

                    addr += opcode_size;
                }
            }
        }
        return;
    }

    // RMAP
    if (!strcmp(cmd, "RMAP"))
    {
        StrTrim(line);
        if (line[0])
        {
            if (!(g_machine_flags & MACHINE_POWER_ON))
            {
                Debugger_Printf("Command unavailable while machine is not running!\n");
            }
            else
            {
                t_debugger_value value;
                char *p = line;
                while (*p && Debugger_Eval_ParseExpression(&p, &value) > 0)
                {
                    const s16 addr = value.data;
                    Debugger_ReverseMap(addr);

                    // Skip comma to get to next expression, if any
                    if (*p == ',')
                        p++;
                }
            }
        }
        else
        {
            Debugger_Help("RMAP");
        }
        return;
    }

    // SYMBOLS - SYMBOLS
    if (!strcmp(cmd, "SYM") || !strcmp(cmd, "SYMBOL") || !strcmp(cmd, "SYMBOLS"))
    {
        StrTrim(line);
        if (line[0] == '@')
        {
            t_debugger_value value;
            line++;
            if (Debugger_Eval_ParseExpression(&line, &value) < 0)
            {
                Debugger_Printf("Syntax error!\n");
                return;
            }
            else
            {
                Debugger_Symbols_ListByAddr(value.data);
            }
        }
        else
        {
            Debugger_Symbols_ListByName(!StrIsNull(line) ? line : NULL);
        }
        return;
    }

    // VARS
    if (!strcmp(cmd, "VAR") || !strcmp(cmd, "VARS"))
    {
        StrTrim(line);
        if (line[0] == '@')
        {
            t_debugger_value value;
            line++;
            if (Debugger_Eval_ParseExpression(&line, &value) < 0)
            {
                Debugger_Printf("Syntax error!\n");
                return;
            }
            else
            {
                Debugger_Symbols_Vars_ListByAddr(value.data);
            }
        }
        else
        {
            Debugger_Symbols_Vars_ListByName(!StrIsNull(line) ? line : NULL);
        }
        return;
    }

    // M - MEMORY DUMP
    if (!strcmp(cmd, "M") || !strcmp(cmd, "MEM"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            u16 addr = sms.R.PC.W;
            int len  = 16*8;
            t_debugger_value value;
            if (Debugger_Eval_ParseExpression(&line, &value) > 0)
            {
                addr = value.data;
                parse_skip_spaces(&line);
                if (Debugger_Eval_ParseExpression(&line, &value) > 0)
                {
                    len = value.data;
                }
            }
            while (len > 0)
            {
                char buf[256];
                u8   data[8];
                char *p;
                int  line_len = (len >= 8) ? 8 : len;
                sprintf(buf, "%04X-%04X | ", addr, (addr + line_len - 1) & 0xFFFF);
                p = buf + strlen(buf);
                int i;
                for (i = 0; i < line_len; i++)
                {
                    data[i] = RdZ80_NoHook((addr + i) & 0xFFFF);
                    sprintf(p, "%02X ", data[i]);
                    p += 3;
                }
                if (i < 8)
                {
                    p += sprintf(p, "%-*s", (8 - line_len) * 3, "");
                }
                sprintf(p, "| ");
                p += 2;
                for (i = 0; i < line_len; i++)
                    *p++ = (isprint(data[i]) ? data[i] : '.');
                *p++ = '\n';
                *p = EOSTR;
                Debugger_Printf("%s", buf);
                addr += 8;
                len -= line_len;
            }
        }
        return;
    }

    // ST - STACK DUMP
    if (!strcmp(cmd, "ST") || !strcmp(cmd, "STACK"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            u16 addr = sms.R.SP.W;
            int len  = 8;
            t_debugger_value value;
            if (Debugger_Eval_ParseExpression(&line, &value) > 0)
                len = value.data;

            const t_debugger_symbol* symbol = Debugger_Symbols_GetClosestPreviousByAddr(sms.R.PC.W, 256); // be generous
            if (symbol != NULL)
                Debugger_Printf(" Current PC:     %04X      %s+%X\n", sms.R.PC.W, symbol->name, sms.R.PC.W-symbol->cpu_addr);
            else
                Debugger_Printf(" Current PC:     %04X\n", sms.R.PC.W);
            Debugger_Printf("------------------------\n");
            Debugger_Printf(" Stack   8-bit   16-bit\n");
            Debugger_Printf("------------------------\n");
            while (len > 0)
            {
                const u8 v8 = RdZ80_NoHook(addr & 0xFFFF);
                const u16 v16 = (RdZ80_NoHook((addr+1) & 0xFFFF) << 8) | v8;
                
                symbol = Debugger_Symbols_GetClosestPreviousByAddr(v16, 256);
                if (symbol != NULL)
                    Debugger_Printf(" %04X:   %02X      %04X      %s+%X\n", addr, v8, v16, symbol->name, v16-symbol->cpu_addr);
                else
                    Debugger_Printf(" %04X:   %02X      %04X\n", addr, v8, v16);
                addr++;
                len--;
            }
        }
        return;
    }

    // TR - TRACE
    if (!strcmp(cmd, "TR") || !strcmp(cmd, "TRACE"))
    {
        if (!(g_machine_flags & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running!\n");
        }
        else
        {
            int cnt = MIN(16, Debugger.pc_detail_log_count);
        
            parse_skip_spaces(&line);

            char* line_start = line;
            if (parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
            {
                StrUpper(arg);
                if (strcmp(arg, "REGS") == 0)
                {
                    Debugger.pc_detail_log_show_extra_registers = !Debugger.pc_detail_log_show_extra_registers;
                    if (Debugger.pc_detail_log_show_extra_registers)
                        Debugger_Printf("Trace will show: PC, AF, BC, DE, HL, IX, IY, SP\n");
                    else
                        Debugger_Printf("Trace will show: PC, AF, BC, DE, HL\n");
                    return;
                }
                else if (strcmp(arg, "CLEAR") == 0)
                {
                    Debugger.pc_detail_log_head = 0;
                    Debugger.pc_detail_log_count = 0;
                    Debugger.pc_detail_log_data[Debugger.pc_detail_log_data.size()-1].pc = 0xffff;
                    Debugger_Printf("Trace log cleared.\n");
                    return;
                }
                else if (strcmp(arg, "ALL") == 0)
                {
                    cnt = Debugger.pc_detail_log_count;
                }
                else
                {
                    t_debugger_value value;
                    line = line_start;
                    if (Debugger_Eval_ParseExpression(&line, &value) > 0)
                    {
                        cnt = value.data;
                    }
                    else
                    {
                        Debugger_Printf("Syntax error!\n");
                        Debugger_Help("TRACE");
                        return;
                    }
                }
            }

            cnt = MIN(cnt, (int)Debugger.pc_detail_log_count);

            Debugger_Printf("Tracing %d instruction%s (of total %d recorded)\n", cnt, cnt>1?"s":"", Debugger.pc_detail_log_count);
            for (int i = cnt; i > 0; i--)
            {
                int n = (Debugger.pc_detail_log_head - i + Debugger.pc_detail_log_data.size()) % Debugger.pc_detail_log_data.size();
                const t_debugger_exec_log_entry* e = &Debugger.pc_detail_log_data[n];

                char instr[128];
                if (int len = Z80_Disassemble(instr, e->pc, false, false, true))
                {
                    char buf[256];

                    //char instr_opcodes[128];
                    //for (int i = 0; i < len; i++)
                    //  sprintf(instr_opcodes + (i*3), "%02X ", RdZ80_NoHook((e->pc + i) & 0xFFFF));
                    if (Debugger.pc_detail_log_show_extra_registers)
                        sprintf(buf, "%04X: %-18s ; AF:%04X BC:%04X DE:%04X HL:%04X IX:%04X IY:%04X SP:%04X\n", 
                            e->pc, instr, e->af, e->bc, e->de, e->hl, e->ix, e->iy, e->sp);
                    else
                        sprintf(buf, "%04X: %-18s ; AF:%04X BC:%04X DE:%04X HL:%04X\n", 
                            e->pc, instr, e->af, e->bc, e->de, e->hl);

                    if (cnt > 256)
                        Debugger_PrintEx(false, true, false, buf);
                    else
                        Debugger_PrintEx(true, true, true, buf);
                }       
            }
            if (cnt > 256)
            {
                char buf[256];
                strcpy(buf, "(output in file Debug/debuglog.txt)");
                Debugger_PrintEx(true, false, true, buf);
            }
        }
        return;
    }

    // MEMEDIT - MEMORY EDITOR SPAWN
    if (!strcmp(cmd, "MEMEDIT") || !strcmp(cmd, "MEMEDITOR"))
    {
        int size_x = -1;
        int size_y = -1;
        if (parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            if (sscanf(arg, "%d", &size_y) < 1)
            {
                Debugger_Printf("Syntax error!\n");
                Debugger_Help("MEMEDIT");
                return;
            }
        }
        if (parse_getword(arg, sizeof(arg), &line, " ", 0, PARSE_FLAGS_NONE))
        {
            if (sscanf(arg, "%d", &size_x) < 1)
            {
                Debugger_Printf("Syntax error!\n");
                Debugger_Help("MEMEDIT");
                return;
            }
        }
        if (size_x < 1 && size_x != -1)
            size_x = -1;
        if (size_y < 1 && size_y != -1)
            size_y = -1;
        MemoryViewer_New(FALSE, size_x, size_y);
        return;
    }

    // Unknown command
    Debugger_Printf("Syntax error!\n");
}

void        Debugger_ShortcutButton_Callback(t_widget* w)
{
    t_debugger_app* app = &DebuggerApp;

    t_debugger_shortcut* sh = &app->shortcuts[(int)(intptr_t)w->user_data];

    char* command = strdup(sh->command);
    Debugger_InputParseCommand(command);        // non-const input
    free(command);

    app->shortcuts_freeze = 1;
}

//-----------------------------------------------------------------------------
// Debugger_InputBoxCallback(t_widget *w)
//-----------------------------------------------------------------------------
// Input box widget callback. Called when the user validate a line with ENTER.
// Perform command-line processing.
//-----------------------------------------------------------------------------
void        Debugger_InputBoxCallback(t_widget *w)
{
    char    line_buf[512];

    strcpy(line_buf, widget_inputbox_get_value(DebuggerApp.input_box));
    StrTrim(line_buf);

    // Clear input box
    widget_inputbox_set_value(DebuggerApp.input_box, "");

    // An empty line means step into or activate debugging
    if (line_buf[0] == EOSTR)
    {
        if (g_machine_flags & MACHINE_POWER_ON)
            Debugger_StepInto();
        return;
    }

    // Add input to history
    //// Note: add after executing command, so that HISTORY doesn't show itself
    Debugger_History_AddLine(line_buf);

    // Print line to the console, as a user command log
    // Note: passing address of the color because we need a theme switch to be reflected on this
    widget_textbox_set_current_color(DebuggerApp.console, &COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT);
    Debugger_Printf("# %s\n", line_buf);
    widget_textbox_set_current_color(DebuggerApp.console, &COLOR_SKIN_WINDOW_TEXT);

    // Parse command
    Debugger_InputParseCommand(line_buf);
}

//-----------------------------------------------------------------------------
// FUNCTIONS - Values/Variables
//-----------------------------------------------------------------------------

void     Debugger_Value_SetCpuRegister(t_debugger_value *value, const char *name, void *data, int data_size)
{
    value->data         = 0;
    value->data_size    = data_size;
    value->flags        = DEBUGGER_VALUE_FLAGS_ACCESS_READ | DEBUGGER_VALUE_FLAGS_ACCESS_WRITE;
    value->source       = DEBUGGER_VALUE_SOURCE_CPU_REG;
    value->source_data  = data;
    value->name         = name;
    Debugger_Value_Read(value);
}

void     Debugger_Value_SetSymbol(t_debugger_value *value, t_debugger_symbol *symbol, bool rom_addr)
{
    value->data         = 0;
    value->data_size    = 16;
    value->flags        = DEBUGGER_VALUE_FLAGS_ACCESS_READ;
    value->source       = rom_addr ? DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR : DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR;
    value->source_data  = symbol;
    value->name         = symbol->name;
    Debugger_Value_Read(value);
}

void    Debugger_Value_Read(t_debugger_value *value)
{
    assert(value->flags & DEBUGGER_VALUE_FLAGS_ACCESS_READ);
    switch (value->source)
    {
    case DEBUGGER_VALUE_SOURCE_CPU_REG:
        {
            if (value->data_size == 8)
                value->data = *(u8 *)value->source_data;
            else if (value->data_size == 16)
                value->data = *(u16 *)value->source_data;
            else
                assert(0);
        }
        break;
    case DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR:
        {
            t_debugger_symbol* symbol = (t_debugger_symbol *)value->source_data;
            value->data = symbol->cpu_addr;
        }
        break;
    case DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR:
        {
            t_debugger_symbol* symbol = (t_debugger_symbol *)value->source_data;
            value->data = symbol->rom_addr;
            value->data_size = 24;
        }
        break;
    default:
        assert(0);
    }
}

void    Debugger_Value_Write(t_debugger_value *value, u32 data)
{
    assert(value->flags & DEBUGGER_VALUE_FLAGS_ACCESS_WRITE);

    value->data = data;
    switch (value->source)
    {
    case DEBUGGER_VALUE_SOURCE_CPU_REG:
        if (value->data_size == 8)
            *(u8 *)value->source_data = value->data;
        else if (value->data_size == 16)
            *(u16 *)value->source_data = value->data;
        else
            assert(0);
        break;
    default:
        assert(0);
    }
}

//..update below

void     Debugger_Value_SetComputed(t_debugger_value *value, u32 data, int data_size)
{
    value->data         = data;
    value->data_size    = data_size;
    value->flags        = DEBUGGER_VALUE_FLAGS_ACCESS_READ;
    value->source       = DEBUGGER_VALUE_SOURCE_COMPUTED;
    value->source_data  = NULL;
    value->name         = NULL;
}

void     Debugger_Value_SetDirect(t_debugger_value *value, u32 data, int data_size)
{
    value->data         = data;
    value->data_size    = data_size;
    value->flags        = DEBUGGER_VALUE_FLAGS_ACCESS_READ;
    value->source       = DEBUGGER_VALUE_SOURCE_DIRECT;
    value->source_data  = NULL;
    value->name         = NULL;
}

//-----------------------------------------------------------------------------
// FUNCTIONS - Expression Evaluator
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Debugger_Eval_GetVariable(int variable_replacement_flags, const char *var, t_debugger_value *result)
//-----------------------------------------------------------------------------
// Replace given variable by looking for corresponding CPU register/symbol name.
// Return TRUE on success and fill result, else return FALSE.
//-----------------------------------------------------------------------------
// FIXME:
// - Should do something better in the future. 
//   Generalized abstraction to access system components?
//   Anyway - this is quick and dirty but it suffise now.
//-----------------------------------------------------------------------------
bool    Debugger_Eval_ParseVariable(int variable_replacement_flags, const char *var, t_debugger_value *result)
{
    // CPU registers
    if (variable_replacement_flags & DEBUGGER_VARIABLE_REPLACEMENT_CPU_REGS)
    {
        t_list *vars;
        for (vars = Debugger.variables_cpu_registers; vars != NULL; vars = vars->next)
        {
            t_debugger_value *value = (t_debugger_value *)vars->elem;
            if (!stricmp(value->name, var))
            {
                *result = *value;
                Debugger_Value_Read(result);
                return true;
            }
        }
    }

    // Symbols
    if (variable_replacement_flags & DEBUGGER_VARIABLE_REPLACEMENT_SYMBOLS)
    {
        // Go thru all symbols
        const bool is_rom_addr = (*var == ':');
        for (t_list *symbols = Debugger.symbols; symbols != NULL; symbols = symbols->next)
        {
            t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
            if (!stricmp(is_rom_addr ? var+1 : var, symbol->name))
            {
                Debugger_Value_SetSymbol(result, symbol, is_rom_addr);
                return true;
            }
        }
    }

    return false;
}

int  Debugger_Eval_ParseIntegerHex(const char* s, const char** out_end)
{
    int         result = 0;
    char        c;

    while ((c = *s) != '\0')
    {
        int digit;
        if (c >= '0' && c <= '9')
            digit = c - '0';
        else if (c >= 'a' && c <= 'f')
            digit = c + 10 - 'a';
        else if (c >= 'A' && c <= 'F')
            digit = c + 10 - 'A';
        else
            break;
        assert(digit >= 0x00 && digit <= 0x0F);
        result = (result << 4) | digit;
        s++;
    }
    if (out_end)
        *out_end = s;
    return (result);
}

static int  Debugger_Eval_ParseInteger(const char *s, const char *base, const char **end)
{
    int         result = 0;
    const int   len_base = strlen(base);
    char        c;

    while ((c = *s) != '\0')
    {
        const char *digit = strchr(base, *s); // Note: this is not exactly the fastest thing to do...
        if (digit == NULL)
            break;
        result = (result * len_base) + (digit - base);
        s++;
    }
    *end = s;
    return (result);
}

bool    Debugger_Eval_ParseConstant(const char *value, t_debugger_value *result, t_debugger_eval_value_format default_format)
{
    // Debugger_Printf(" - token = %s\n", token);

    // Assume default hexadecimal
    t_debugger_eval_value_format value_format = default_format;
    if (*value == '$')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX;
        value++;
    }
    else
    if (*value == '%')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_BIN;
        value++;
    }
    else
    if (*value == '#')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC;
        value++;
    }
    else
    if (*value == '0' && (value[1] == 'x' || value[1] == 'X'))
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX;
        value += 2;
    }

    {
        const char *  parse_end;
        int data;
        switch (value_format)
        {
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX:
            data = Debugger_Eval_ParseIntegerHex(value, &parse_end);
            break;
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_BIN:
            data = Debugger_Eval_ParseInteger(value, "01", &parse_end);
            break;
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC:
            data = Debugger_Eval_ParseInteger(value, "0123456789", &parse_end);
            break;
        default:
            assert(0);
            return false;
        }

        //if (data > (1<<15)-1 || data < -(1<<5))
        if (data > 0xffff || data < -0x7fff)
            Debugger_Value_SetDirect(result, data, 24);
        else
            Debugger_Value_SetDirect(result, data, 16);

        if (*parse_end != '\0')
        {
            // Syntax error
            // Note: 'src' pointer not advanced, this is what we want here
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Debugger_Eval_GetValue(char **src_result, t_debugger_value *result)
//-----------------------------------------------------------------------------
// Parse a single value out of given string.
// Advance string pointer.
// Return:
//  > 0 : success
//    0 : no value found
//  < 0 : parsing error
//-----------------------------------------------------------------------------
int    Debugger_Eval_GetValue(char **src_result, t_debugger_value *result)
{
    //t_debugger_eval_value_format value_format;
    char    token_buf[256];
    char *  token = token_buf;
    char *  src = *src_result;
    int     expr_error;

    // Debugger_Printf("Debugger_Eval_GetValue(\"%s\")\n", src);
    parse_skip_spaces(&src);

    // Parenthesis open a sub expression
    if (*src == '(')
    {
        src++;
        expr_error = Debugger_Eval_ParseExpression(&src, result);
        if (expr_error <= 0)
            return (expr_error);
        if (*src != ')')
        {
            // Unterminated parenthesis
            Debugger_Printf("Syntax Error - Missing closing parenthesis!\n");
            *src_result = src;
            return (-1);
        }
        src++;
        *src_result = src;
        return (expr_error);
    }

    // Get token
    if (!parse_getword(token_buf, sizeof(token_buf), &src, " \t\n+-*/&|^(),.", 0, PARSE_FLAGS_DONT_EAT_SEPARATORS))
        return (0);
    if (token[0] == '\0')
        return (0);

    // Attempt to see if it's a variable
    if (Debugger_Eval_ParseVariable(DEBUGGER_VARIABLE_REPLACEMENT_ALL, token, result))
    {
        *src_result = src;
        return (1);
    }

    // Else a direct value
    if (Debugger_Eval_ParseConstant(token, result))
    {
        *src_result = src;
        return (1);
    }

    return (-1);
}

static int  Debugger_Eval_GetExpression_Block(char **expr, t_debugger_value *result)
{
    t_debugger_value value1;
    t_debugger_value value2;

    char* p = (char *)*expr; 
    // Debugger_Printf("Debugger_Eval_GetExpression_Block(\"%s\")\n", p);

    parse_skip_spaces(&p);
    if (p[0] == '\0')
    {
        // Empty expression
        return (0);
    }

    // Get first value
    int expr_error = Debugger_Eval_GetValue(&p, &value1);
    if (expr_error <= 0)
    {
        Debugger_Printf("Syntax error at \"%s\"!\n", p);
        Debugger_Printf("                 ^ invalid value or label\n");
        return (expr_error);
    }
    for (;;)
    {
        //parse_skip_spaces(&p);

        // Get operator
        const char op = *p;

        if (op == ',' || op == '.' || op == ' ')
            break;

        // Chain of addition/subtraction are handled by Debugger_Eval_GetExpression()
        if (op == '+' || op == '-') 
            break;

        // Stop parsing here on end-of-string or parenthesis closure
        if (op == '\0' || op == ')')
            break;

        // Verify that we have a valid operator
        if (!strchr("*/&|^", op))
        {
            Debugger_Printf("Syntax error at \"%s\"!\n", p);
            Debugger_Printf("                 ^ unexpected operator\n");
            return (-1);
        }
        p++;

        // Get a second value (since all our operator are binary operator now)
        expr_error = Debugger_Eval_GetValue(&p, &value2);
        if (expr_error <= 0)
        {
            Debugger_Printf("Syntax error at \"%s\"!\n", p);
            Debugger_Printf("                 ^ invalid value or label\n");
            return (expr_error);
        }

        {
            // Process operator
            int data_size = MAX(value1.data_size, value1.data_size);
            switch (op)
            {
            case '&':
                Debugger_Value_SetComputed(&value1, value1.data & value2.data, data_size);
                break;
            case '|':
                Debugger_Value_SetComputed(&value1, value1.data | value2.data, data_size);
                break;
            case '^':
                Debugger_Value_SetComputed(&value1, value1.data ^ value2.data, data_size);
                break;
            case '*':
                Debugger_Value_SetComputed(&value1, value1.data * value2.data, data_size);
                break;
            case '/':
                Debugger_Value_SetComputed(&value1, value1.data / value2.data, data_size);
                break;
            default:
                assert(0);
                break;
            }
        }
    }

    // Ok
    *result = value1;
    *expr = p;
    return (1);
}

//-----------------------------------------------------------------------------
// Debugger_Eval_GetExpression(char **expr, t_debugger_value *result)
//-----------------------------------------------------------------------------
// Parse and evaluate expression from given string.
// Advance string pointer.
// Return:
//  > 0 : success
//    0 : no value found
//  < 0 : parsing error
//-----------------------------------------------------------------------------
// Expression exemples:
//  A
//  (A)
//  A+B
//  A+B*C
//  A+(B*C)
//  ((0xFF^0x10)&%11110000)
//-----------------------------------------------------------------------------
int     Debugger_Eval_ParseExpression(char **expr, t_debugger_value *result)
{
    char *  p;
    char    op;
    int     expr_error;
    t_debugger_value value1;
    t_debugger_value value2;

    p = (char *)*expr; 
    // Debugger_Printf("Debugger_Eval_GetExpression(\"%s\")\n", p);

    parse_skip_spaces(&p);
    if (p[0] == '\0')
    {
        // Empty expression
        return (0);
    }

    // Get first expression block
    expr_error = Debugger_Eval_GetExpression_Block(&p, &value1);
    if (expr_error <= 0)
        return (expr_error);

    for (;;)
    {
        //parse_skip_spaces(&p);

        // Get operator
        op = *p;

        if (op == ',' || op == '.' || op == ' ')
            break;

        // Stop parsing here on end-of-string or parenthesis closure
        if (op == '\0' || op == ')')
            break;

        // Verify that we have a valid operator
        if (!strchr("+-", op))
        {
            Debugger_Printf("Syntax error at \"%s\"!\n", p);
            Debugger_Printf("                 ^ unexpected operator\n");
            return (-1);
        }
        p++;

        // Get a second expression block (since all our operator are binary operator now)
        expr_error = Debugger_Eval_GetExpression_Block(&p, &value2);
        if (expr_error < 0)
            return (expr_error);
        if (expr_error == 0)
        {
            Debugger_Printf("Syntax error at \"%s\"!\n", p);
            Debugger_Printf("                 ^ invalid value or label\n");
            return (-1);
        }

        {
            // Process operator
            int data_size = MAX(value1.data_size, value1.data_size);
            switch (op)
            {
            case '+':
                Debugger_Value_SetComputed(&value1, value1.data + value2.data, data_size);
                break;
            case '-':
                Debugger_Value_SetComputed(&value1, value1.data - value2.data, data_size);
                break;
            default:
                assert(0);
                break;
            }
        }
    }

    // Ok
    *result = value1;
    *expr = p;
    return (1);
}

//-----------------------------------------------------------------------------
// FUNCTIONS - COMPLETION
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Debugger_CompletionCallback(t_widget *w)
// Called by the input widget. Add completion string, if any.
//-----------------------------------------------------------------------------
bool        Debugger_CompletionCallback(t_widget *w)
{
    char *  current_word;
    int     current_word_len;
    bool    first_word;
    t_list *matching_words;
    int     matching_words_count = 0;
    char *  result;
    
    const char* word_delimiters = " \t\r\n,:;";

    // Get current word
    int pos = widget_inputbox_get_cursor_pos(w);
    const char *s = widget_inputbox_get_value(w) + pos;
    current_word_len = 0;
    while (pos-- > 0 && !strchr(word_delimiters, s[-1]))
    {
        s--;
        current_word_len++;
    }
    // if (current_word_len == 0)
    //    return false;
    current_word = StrNDup(s, current_word_len);

    // Attempt to find if there's a word before this
    // We need this to contextually complete with commands (1st word) or symbols (subsequent words)
    // This is kinda hacky (should split the string by token)
    first_word = TRUE;
    while (pos-- > 0)
    {
        if (!isspace(s[0]))
        {
            first_word = FALSE;
            break;
        }
        s--;
    }

    // Build a list of matching words
    matching_words = NULL;
    matching_words_count = 0;

    if (first_word)
    {
        // Complete with command
        t_debugger_command_info *command_info = &DebuggerCommandInfos[0];
        while (command_info->command_long != NULL)
        {
            if (!strnicmp(current_word, command_info->command_long, current_word_len))
            {
                list_add(&matching_words, (char *)command_info->command_long);
                matching_words_count++;
            }
            command_info++;
        }
    }
    else
    {
        // Complete with symbols
        t_list *symbols;
        for (symbols = Debugger.symbols; symbols != NULL; symbols = symbols->next)
        {
            t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
            if (!strnicmp(current_word, symbol->name, current_word_len))
            {
                list_add(&matching_words, symbol->name);
                matching_words_count++;
            }
        }
    }

    if (matching_words_count == 0)
    {
        // No match
        Debugger_Printf("No match for \"%s\"\n", current_word);
        result = NULL;
    }
    else if (matching_words_count == 1)
    {
        // Single match, no ambiguity :)
        const char *complete_word = (const char *)matching_words->elem;
        result = strdup(complete_word);
    }
    else
    {
        // Multiple matches
        int common_prefix_size;
        t_list *matches;
        
        // Sort matches by name
        list_sort(&matching_words, (t_list_cmp_handler)stricmp);
        
        // Print them
        if (current_word_len > 0)
            Debugger_Printf("%d matches for \"%s\":\n", matching_words_count, current_word);
        else
            Debugger_Printf("%d matches:\n", matching_words_count);
        for (matches = matching_words; matches != NULL; matches = matches->next)
        {
            const char *complete_word = (const char *)matches->elem;
            Debugger_Printf(" - %s\n", complete_word);
        }

        // Find common prefix, if any
        // FIXME: There's probably better algorithm to perform this.
        common_prefix_size = current_word_len;
        for (;;)
        {
            // Get current character of first match
            char c;
            matches = matching_words;
            c = ((char *)matches->elem)[common_prefix_size];
            if (c == '\0')
                break;
            
            // Compare it with following matches
            for (matches = matches->next; matches != NULL; matches = matches->next)
            {
                char c2 = ((char *)matches->elem)[common_prefix_size];
                if (c != c2)
                    break;
            }
            // Haven't got thru all the list means there was a difference, break
            // Note: might want to use goto in this kind of case
            if (matches != NULL)
                break;
            common_prefix_size++;
        }

        if (common_prefix_size > 0)
        {
            result = StrNDup((char *)matching_words->elem, common_prefix_size);
        }
        else
        {
            result = NULL;
        }
    }

    // Free temporary work data
    free(current_word);
    list_free_custom(&matching_words, NULL);

    // Complete
    if (result != NULL)
    {
        // We want the full word to be replaced (this makes things prettier :)
        // So we delete the beginning of the word
        while (current_word_len-- > 0)
            widget_inputbox_delete_current_char(w);

        // Then, re-add the full word
        widget_inputbox_insert_chars(w, result);
        free(result);

        // If we had only one match, add a space
        if (matching_words_count == 1)
            widget_inputbox_insert_chars(w, " ");

        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - HISTORY
//-----------------------------------------------------------------------------

// FIXME-OPT: Absolutely lame implementation, because we don't have decent data structure libraries.
void    Debugger_History_AddLine(const char *line_to_add)
{
    t_debugger_history_item *item;
    char *line;
    char *line_uppercase;
    bool item_added;
    int n;

    // Shift all history entries by one up to matching one which is moved back to front (entry 1).
    // Entry 0 is current input line and is fixed.
    // 3 bye        3 hello
    // 2 hello  --> 2 sega
    // 1 sega       1 <line>
    // 0            0 

    // Duplicate line and convert to uppercase
    // Even when we find a matching entry, we will replace it by was what typed to keep last character casing.
    // It's a rather useless detail, meaning it is indispensable.
    line = strdup(line_to_add);
    line_uppercase = strdup(line_to_add);
    StrUpper(line_uppercase);

    // Search for duplicate entry in history
    for (n = 1; n < Debugger.history_count; n++)
    {
        if (strcmp(Debugger.history[n].line_uppercase, line_uppercase) == 0)
            break;
    }
    
    //Msg(MSGT_USER, "n = %d, h_count = %d, h_max = %d", n, Debugger.history_count, Debugger.history_max);

    if (n < Debugger.history_count || n == Debugger.history_max - 1)
    {
        // Delete last or matching entry
        free(Debugger.history[n].line);
        free(Debugger.history[n].line_uppercase);
        item_added = FALSE;
    }
    else
    {
        item_added = TRUE;
    }

    // Shift
    while (n > 1)
    {
        Debugger.history[n] = Debugger.history[n - 1];
        n--;
    }

    // Duplicate and add new entry
    item = &Debugger.history[1];
    item->line = line;
    item->line_uppercase = line_uppercase;
    item->cursor_pos = -1;

    // Increase counter
    if (item_added)
        if (Debugger.history_count < Debugger.history_max)
            Debugger.history_count++;

    // Reset current index every time a new line is typed
    Debugger.history_current_index = 0;
}

//-----------------------------------------------------------------------------
// Debugger_History_Callback(t_widget *w)
// Called by the input widget. Handle history.
//-----------------------------------------------------------------------------
bool        Debugger_History_Callback(t_widget *w, int level)
{
    if (level != -1 && level != 1)
        return false;

    // Bound check
    const int new_index = Debugger.history_current_index + level;
    if (new_index < 0 || new_index >= Debugger.history_count)
        return false;

    // If leaving index 0 (current line), save current line to item 0
    if (Debugger.history_current_index == 0)
    {
        free(Debugger.history[0].line);
        free(Debugger.history[0].line_uppercase);
        Debugger.history[0].line = strdup(widget_inputbox_get_value(w));
        Debugger.history[0].line_uppercase = strdup(Debugger.history[0].line);
        Debugger.history[0].cursor_pos = widget_inputbox_get_cursor_pos(w);
        StrUpper(Debugger.history[0].line_uppercase);
    }

    // Restore new item
    widget_inputbox_set_value(w, Debugger.history[new_index].line);
    if (Debugger.history[new_index].cursor_pos != -1)
        widget_inputbox_set_cursor_pos(w, Debugger.history[new_index].cursor_pos);
    Debugger.history_current_index = new_index;

    return true;
}

void        Debugger_History_List(const char *search_term_arg)
{
    int     index;
    char   *search_term;

    if (search_term_arg)
    {
        Debugger_Printf("History lines matching \"%s\":\n", search_term_arg);
        search_term = strdup(search_term_arg);
        StrUpper(search_term);
    }
    else
    {
        Debugger_Printf("History:\n");
        search_term = NULL;
    }
    //if (n <= 1)  // It's always 1 as current command was already pushed into history
    //{
    //    Debugger_Printf(" <None>\n");
    //    return;
    //}

    for (index = Debugger.history_count - 1; index >= 1; index--)
    {
        t_debugger_history_item *item = &Debugger.history[index];

        // If search term was specified, skip history line not matching it
        if (search_term != NULL)
            if (strstr(item->line_uppercase, search_term) == NULL)
                continue;

        // Print
        Debugger_Printf(" %*s[%d] %s\n",
            (Debugger.history_count >= 10 && index < 10) ? 1 : 0, "", 
            index, Debugger.history[index]);
    }

    if (search_term != NULL)
    {
        // Free the uppercase duplicate we made
        free(search_term);
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - REVERSE MAP
//-----------------------------------------------------------------------------

int         Debugger_ReverseMapFindRomAddress(u16 addr, bool* is_bios)
{
    const int mem_pages_index = (addr >> 13);
    const u8 *mem_pages_base = Mem_Pages[mem_pages_index] + (mem_pages_index << 13);

    int offset;

    // - ROM
    offset = (mem_pages_base - ROM) + (addr & 0x1FFF);
    if (offset >= 0 && offset < tsms.Size_ROM)
    {
        if (is_bios) *is_bios = false;
        return offset;
    }
            
    // - ROM (special hack for first 1 KB)
    // FIXME: Report ROM instead of SMS BIOS in those cases. Anyway SMS BIOS is poorly emulated.
    offset = (mem_pages_base - Game_ROM_Computed_Page_0) + (addr & 0x1FFF);
    if (offset >= 0 && offset < 0x4000)
    {
        if (is_bios) *is_bios = false;
        return offset;
    }

    // - BIOSes
    offset = (mem_pages_base - BIOS_ROM) + (addr & 0x1fff);
    if (offset >= 0 && offset < 0x2000)
    {
        if (is_bios) *is_bios = true;
        return offset;
    }
    offset = (mem_pages_base - BIOS_ROM_Jap) + (addr & 0x1fff);
    if (offset >= 0 && offset < 0x2000)
    {
        if (is_bios) *is_bios = true;
        return offset;
    }
    offset = (mem_pages_base - BIOS_ROM_Coleco) + (addr & 0x1fff);
    if (offset >= 0 && offset < 0x2000)
    {
        if (is_bios) *is_bios = true;
        return offset;
    }
    offset = (mem_pages_base - BIOS_ROM_SF7000) + (addr & 0x1fff);
    if (offset >= 0 && offset < 0x4000)
    {
        if (is_bios) *is_bios = true;
        return offset;
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Note: this is completely hard-coded to handle the most common cases.
// The reason is that this feature was planned since a long time using a more
// generic approach, but since I could not get myself to code that version, I'd
// rather code the simple one so it is immediately useful.
//-----------------------------------------------------------------------------
// FIXME: Could support mappers registers, although it's not super useful.
// FIXME: Not great at supporting mirrored ranges.
//-----------------------------------------------------------------------------
void        Debugger_ReverseMap(u16 addr)
{
    int     ram_len;
    int     ram_start_addr;
    Mapper_Get_RAM_Infos(&ram_len, &ram_start_addr);

    int     sram_len;
    u8 *    sram_buf;
    BMemory_Get_Infos((void**)&sram_buf, &sram_len);

    //if (addr < 0x400)
    //  Debugger_Printf(" Z80 $%04X = ROM $%05X (Page %d, Offset %d)", addr, addr, 0, addr & 0x3FFF);

    const int mem_pages_index = (addr >> 13);
    const u8 *mem_pages_base = Mem_Pages[mem_pages_index] + (mem_pages_index << 13);

    // Pages can be pointing to:
    // - ROM
    // - Game_ROM_Computed_Page_0
    // - RAM
    // - SRAM
    // - BIOS_ROM
    // - BIOS_ROM_Jap
    // - BIOS_ROM_Coleco
    // - BIOS_ROM_SF7000
    // Using direct pointer arithmetic comparisons.
    int offset;

    // - ROM, Game_ROM_Computed_Page_0, BIOS_ROM*
    bool is_bios;
    offset = Debugger_ReverseMapFindRomAddress(addr, &is_bios);
    if (offset != -1)
    {
        if (is_bios)
            Debugger_Printf(" Z80 $%04X = BIOS $%04X", addr, offset);
        else            
            Debugger_Printf(" Z80 $%04X = ROM $%05X (Page %X +%04X)", addr, offset, offset >> 14, addr & 0x3FFF);
    }

    // - RAM
    offset = (mem_pages_base - RAM) + (addr & MIN(0x1fff, ram_len - 1));
    if (offset >= 0 && offset < ram_len)
        Debugger_Printf(" Z80 $%04X = RAM $%04X", addr, ram_start_addr + offset);
        //Debugger_Printf(" Z80 $%04X = RAM $%04X = RAM $%04X", addr, ram_start_addr + offset, ram_start_addr + ram_len + offset);

    // - SRAM
    offset = (mem_pages_base - SRAM) + (addr & MIN(0x1fff, sram_len - 1));
    if (offset >= 0 && offset < sram_len)
        Debugger_Printf(" Z80 $%04X = SRAM $%04X", addr, offset);

    //break;
}

//-----------------------------------------------------------------------------

#endif // ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
