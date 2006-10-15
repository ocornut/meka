//-----------------------------------------------------------------------------
// MEKA - debugger.c
// MEKA Z80 Debugger - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "debugger.h"
#include "desktop.h"
#include "g_widget.h"
#include "tools/libparse.h"
#include <ctype.h>
#include <string.h>

#ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define DEBUGGER_APP_TITLE              "Debugger"
#define DEBUGGER_APP_CPUSTATE_LINES     (2)

//-----------------------------------------------------------------------------
// External declaration
//-----------------------------------------------------------------------------

int     Z80_Disassemble(char *dst, word addr, bool display_symbols, bool resolve_indirect_offsets);
int     Z80_Assemble(const char *src, byte dst[8]);

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void     Debugger_Applet_Init(void);
static void     Debugger_Redraw_State(void);

// Misc
static void     Debugger_Help(const char *cmd);
static void     Debugger_SetTrap(int trap); 
static void     Debugger_InputBoxCallback(t_widget *w);
static bool     Debugger_CompletionCallback(t_widget *w);

// Evaluator
static bool     Debugger_Eval_GetValue(char **src, t_debugger_value *result);
static bool     Debugger_Eval_GetExpression(char **expr, t_debugger_value *result);
static bool     Debugger_Eval_GetVariable(int variable_replacement_flags, const char *var, t_debugger_value *result);

static void     Debugger_GetAccessString(int access, char buf[4])
{
    char *p = buf;
    if (access & BREAKPOINT_ACCESS_R)
        *p++ = 'R';
    if (access & BREAKPOINT_ACCESS_W)
        *p++ = 'W';
    if (access & BREAKPOINT_ACCESS_X)
        *p++ = 'X';
    *p = 0;
}

// Hooks
static void                     Debugger_Hooks_Install(void);
static void                     Debugger_Hooks_Uninstall(void);
static void                     Debugger_WrZ80_Hook(register int addr, register u8 value);
static u8                       Debugger_RdZ80_Hook(register int addr);
static void                     Debugger_OutZ80_Hook(register u16 addr, register u8 value);
static u8                       Debugger_InZ80_Hook(register u16 addr);

// Breakpoints
static void                     Debugger_BreakPoints_List(void);
static void                     Debugger_BreakPoints_Clear(bool disabled_only);
//static void                   Debugger_BreakPoints_RefreshCpuExecTraps(void);
static int                      Debugger_BreakPoints_AllocateId(void);
static t_debugger_breakpoint *  Debugger_BreakPoints_SearchById(int id);

// Breakpoint
static t_debugger_breakpoint *  Debugger_BreakPoint_Add(int type, int location, int access_flags, int address_start, int address_end, int auto_delete, const char *desc);
static void                     Debugger_BreakPoint_Remove(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_Enable(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_Disable(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_GetSummaryLine(t_debugger_breakpoint *breakpoint, char *buf);
static const char *             Debugger_BreakPoint_GetTypeName(t_debugger_breakpoint *breakpoint);
static void                     Debugger_BreakPoint_ActivatedVerbose(t_debugger_breakpoint *breakpoint, int access, int addr, int value);

// Symbols
static void                     Debugger_Symbols_Load(void);
static void                     Debugger_Symbols_Clear(void);
static void                     Debugger_Symbols_List(char *search_name);
t_debugger_symbol *             Debugger_Symbols_GetFirstByAddr(int addr);
t_debugger_symbol *             Debugger_Symbols_GetLastByAddr(int addr);

// Symbol
static t_debugger_symbol *      Debugger_Symbol_Add(u16 addr, int bank, const char *name);
static void                     Debugger_Symbol_Remove(t_debugger_symbol *symbol);
static int                      Debugger_Symbol_CompareByAddress(t_debugger_symbol *symbol1, t_debugger_symbol *symbol2);

// History
static bool                     Debugger_History_Callback(t_widget *w, int level);
static void                     Debugger_History_AddLine(const char *line);
static void                     Debugger_History_List(void);

//-----------------------------------------------------------------------------
// Data - Command Info/Help
//-----------------------------------------------------------------------------

typedef struct
{
    const char *        command_short;
    const char *        command_long;
    const char *        abstract;
    const char *        description;
} t_debugger_command_info;

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
        " 0000: CALL 0100h\n"
        " 0003: LD HL, 1FFFh\n"
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
        "J", "JUMP",
        "Jump",
        // Description
        "J/JUMP: Jump\n"
        "Usage:\n"
        " J address     ; Jump to <address>\n"
        "Examples:\n"
        " J 0           ; Jump back to 0000h (reset)\n"
        " J 1000        ; Jump to 1000h"
    },
    {
        "B", "BREAK",
        "Manage breakpoints",
        // Description
        "B/BREAK: Manage breakpoints\n"
        "Usage:\n"
        " B address [..address2]    ; Add CPU R/W/X breakpoint\n"
        " B [access] [bus] address  ; Add breakpoint (advanced)\n"
        " B LIST                    ; List breakpoints\n"
        " B REMOVE id               ; Remove breakpoint <id>\n"
        " B ENABLE id               ; Enable breakpoint <id>\n"
        " B DISABLE id              ; Disable breakpoint <id>\n"
        " B CLEAR                   ; Clear breakpoints\n"
        "Parameters:\n"
        " access  : access to trap, any from r/w/x (rwx)\n"
        " bus     : bus to watch, one from cpu/io/vram/pal (cpu)\n"
        " address : breakpoint address, can be a range\n"
        " id      : breakpoint identifier\n"
        "Examples:\n"
        " B 0038          ; break when CPU access 0038h\n"
        " B w io 7f       ; break on IO write to 7Fh\n"
        " B rx e000..ffff ; break on CPU read/exec from E000h+\n"
        " B w vram 3f00.. ; break on VRAM write to SAT"
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
        " W r io dd      ; watch and log all IO read from DDh"
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
        " R\n"
    },
    {
        "SYM", "SYMBOLS",
        "List symbols",
        // Description
        "SYM/SYMBOLS: List symbols\n"
        "Usage:\n"
        " SYM [name]\n"
        "Parameters:\n"
        " name : symbol name to search for (*)"
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
        " len     : length to dump, in byte (128)"
        "Examples:\n"
        " M              ; dump 128 bytes at PC"
        " M HL BC        ; dump BC bytes at HL"
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

typedef struct
{
    int                 location;
    char *              name;
    int                 bus_addr_size;  // in bytes
    int                 addr_min;
    int                 addr_max;
    int                 access;
} t_debugger_bus_info;

static t_debugger_bus_info  DebuggerBusInfos[BREAKPOINT_LOCATION_MAX_] =
{
    { BREAKPOINT_LOCATION_CPU,  "CPU",  2,  0x0000, 0xFFFF, BREAKPOINT_ACCESS_RWX   },
    { BREAKPOINT_LOCATION_IO,   "IO",   1,  0x00,   0xFF,   BREAKPOINT_ACCESS_RW    },
    { BREAKPOINT_LOCATION_VRAM, "VRAM", 2,  0x0000, 0x3FFF, BREAKPOINT_ACCESS_RW    },
    { BREAKPOINT_LOCATION_PRAM, "PAL",  1,  0x00,   0x3F,   BREAKPOINT_ACCESS_W     },
};

//-----------------------------------------------------------------------------
// Data - Applet
//-----------------------------------------------------------------------------

typedef struct
{
    t_gui_box *         box;
    BITMAP *            box_gfx;
    t_widget *          console;
    t_widget *          input_box;
    int                 font_id;
    int                 font_height;
    t_frame             frame_disassembly;
    t_frame             frame_cpustate;
} t_debugger_app;

t_debugger_app          DebuggerApp;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void        Debugger_Init_Values (void)
{
    Debugger.Enabled = NO;
    Debugger.Active = NO;
    Debugger.trap_set = FALSE;
    Debugger.trap_address = -1;
    Debugger.stepping = 0;
    Debugger.stepping_trace_after = 0;
    Debugger.breakpoints = NULL;
    memset(Debugger.breakpoints_cpu_space,  0, sizeof(Debugger.breakpoints_cpu_space));
    memset(Debugger.breakpoints_io_space,   0, sizeof(Debugger.breakpoints_io_space));
    memset(Debugger.breakpoints_vram_space, 0, sizeof(Debugger.breakpoints_vram_space));
    memset(Debugger.breakpoints_pram_space, 0, sizeof(Debugger.breakpoints_pram_space));
    Debugger.symbols = NULL;
    Debugger.symbols_count = 0;
    memset(Debugger.symbols_cpu_space,  0, sizeof(Debugger.symbols_cpu_space));
    Debugger.history_count = 1;
    Debugger.history_max = 16;
    Debugger.history = malloc(sizeof(char *) * Debugger.history_max);
    memset(Debugger.history, 0, sizeof(char *) * Debugger.history_max);
    Debugger.history_current_level = 0;
    Debugger.log_file = NULL;
    Debugger.log_filename = "debuglog.txt";
    Debugger.watch_counter = 0;
    memset(Debugger_CPU_Exec_Traps, 0, sizeof(Debugger_CPU_Exec_Traps));

    // Note: Some more clearing will be done by Debugger_MachineReset()
}

static void Debugger_Init_LogFile(void)
{
    // Open log file if not already open
    if (Configuration.debugger_log_enabled && Debugger.log_file == NULL)
    {
        char filename[FILENAME_LEN];
        if (!file_exists (file.dir_debug, 0xFF, NULL))
            meka_mkdir (file.dir_debug);
        sprintf (filename, "%s/%s", file.dir_debug, Debugger.log_filename);
        Debugger.log_file = fopen (filename, "a+t");
        if (Debugger.log_file != NULL)
            fprintf (Debugger.log_file, Msg_Get (MSG_Log_Session_Start), meka_date_getf ());
    }
}

void        Debugger_Init (void)
{
    ConsolePrintf ("%s\n", Msg_Get (MSG_Debug_Init));
    Debugger_Applet_Init ();

    // Open log file
    if (Debugger.Active)
        Debugger_Init_LogFile();

    // Print welcome line
    Debugger_Printf (Msg_Get (MSG_Debug_Welcome));
    Debugger_Printf ("Enter H for help.");
    Debugger_Printf ("Press TAB for completion.");
}

void        Debugger_Close (void)
{
    if (Debugger.log_file != NULL)
    {
        fclose(Debugger.log_file);
        Debugger.log_file = NULL;
    }
    free(Debugger.history);
    Debugger.history = NULL;
}

void        Debugger_Enable (void)
{
    Debugger.Enabled = YES;
    Debugger.Active  = NO;
    Debugger.trap_set = FALSE;
    Debugger.trap_address = 0x0000;
}

//-----------------------------------------------------------------------------
// Debugger_MachineReset ()
// Called when the machine gets reseted
//-----------------------------------------------------------------------------
void        Debugger_MachineReset(void)
{
    // Reset breakpoint on CPU
    if (Debugger.Active)
    {
        Debugger_Printf (Msg_Get (MSG_Machine_Reset));
        Debugger_SetTrap(0x0000);
        sms.R.Trace = 1;

        // Reset trap table
        // Debugger_BreakPointRefreshCpuExecTraps();

        // Clear Z80 PC log queue
        memset(Debugger_Z80_PC_Log_Queue, 0, sizeof(Debugger_Z80_PC_Log_Queue));
        Debugger_Z80_PC_Log_Queue_Write = 0;
        Debugger_Z80_PC_Log_Queue_First = 0;

        // Hook Z80 read/write and I/O
        Debugger_Hooks_Install();
    }
}

//-----------------------------------------------------------------------------
// Debugger_MediaReload(void)
// Called when the media (ROM) gets changed/reloaded
//-----------------------------------------------------------------------------
void        Debugger_MediaReload(void)
{
    Debugger_Symbols_Load();
}

//-----------------------------------------------------------------------------
// Debugger_Update ()
// Update MEKA debugger
//-----------------------------------------------------------------------------
void        Debugger_Update(void)
{
    if (Debugger.Active)
        Debugger_Redraw_State();
    // Reset watch counter
    Debugger.watch_counter = 0;
}

int         Debugger_Hook(Z80 *R)
{
    const u16 pc = R->PC.W;
    // Debugger_Printf("hook, pc=%04X", pc);

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
        Debugger_Printf("Break at %04Xh", pc);

    // If we arrived from a breakpoint CPU exec trap...
    if (Debugger_CPU_Exec_Traps[pc])
    {
        int cnt = Debugger_CPU_Exec_Traps[pc];
        t_list *breakpoints;
        for (breakpoints = Debugger.breakpoints_cpu_space[pc]; breakpoints != NULL; breakpoints = breakpoints->next)
        {
            t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
            if (breakpoint->enabled)
                if (breakpoint->location == BREAKPOINT_LOCATION_CPU && (breakpoint->access_flags & BREAKPOINT_ACCESS_X))
                {
                    Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_X, pc, -1);
                    cnt--;
                }
        }
        assert(cnt == 0);
    }

    // Update state
    Debugger_Redraw_State();

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
        Debugger.trap_address = -1;
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

void                        Debugger_BreakPoints_Clear(bool disabled_only)
{
    t_list *breakpoints;

    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; )
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        breakpoints = breakpoints->next;
        if (disabled_only && breakpoint->enabled)
            continue;
        Debugger_BreakPoint_Remove(breakpoint);
    }
    if (disabled_only)
        Debugger_Printf("Disabled breakpoints cleared.");
    else
        Debugger_Printf("Breakpoints cleared.");
}

void                        Debugger_BreakPoints_List(void)
{
    t_list *breakpoints;
    
    Debugger_Printf("Breakpoints/Watchpoints:");
    if (Debugger.breakpoints == NULL)
    {
        Debugger_Printf(" <None>");
        return;
    }
    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        char buf[256];
        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
        Debugger_Printf(" %s", buf);
    }
}

// FIXME: May want to find the first empty slot (instead of max+1)
int                         Debugger_BreakPoints_AllocateId(void)
{
    int max = -1;
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
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
void                        Debugger_BreakPoints_RefreshCpuExecTraps(void)
{
    t_list *                breakpoints;

    // First clear table
    memset(Debugger_Z80_PC_Trap, 0, sizeof(Debugger_Z80_PC_Trap));

    // Then go thru all breakpoints to add their trap
    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; )
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
    assert(address_end < 0x10000);
    assert(type == BREAKPOINT_TYPE_BREAK || type == BREAKPOINT_TYPE_WATCH);

    // Create and setup breakpoint
    breakpoint = malloc(sizeof (t_debugger_breakpoint));
    breakpoint->enabled = TRUE;
    breakpoint->id = Debugger_BreakPoints_AllocateId();
    breakpoint->type = type;
    breakpoint->location = location;
    breakpoint->access_flags = access_flags;
    breakpoint->address_range[0] = address_start;
    breakpoint->address_range[1] = address_end;
    breakpoint->auto_delete = auto_delete;
    breakpoint->desc = desc ? strdup(desc) : NULL;

    // Add to global breakpoint list
    list_add_to_end(&Debugger.breakpoints, breakpoint);

    // Enable
    Debugger_BreakPoint_Enable(breakpoint);

    // Verbose
    {
        char buf[256];
        Debugger_Printf("%s [%d] added.", Debugger_BreakPoint_GetTypeName(breakpoint), breakpoint->id);
        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
        Debugger_Printf(" %s", buf);
    }

    return (breakpoint);
}

void                        Debugger_BreakPoint_Remove(t_debugger_breakpoint *breakpoint)
{
    // Check parameters
    assert(breakpoint != NULL);

    // Disable
    Debugger_BreakPoint_Disable(breakpoint);

    // Remove from global breakpoint list
    list_remove(&Debugger.breakpoints, breakpoint, NULL);

    // Delete members
    if (breakpoint->desc != NULL)
        free (breakpoint->desc);

    // Delete
    free(breakpoint);
}

void                     Debugger_BreakPoint_Enable(t_debugger_breakpoint *breakpoint)
{
    int         addr;
    t_list **   bus_lists;

    // Set flag
    breakpoint->enabled = TRUE;

    // Add to corresponding bus space list
    switch (breakpoint->location)
    {
    case BREAKPOINT_LOCATION_CPU:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_IO:    bus_lists = Debugger.breakpoints_io_space;    break;
    case BREAKPOINT_LOCATION_VRAM:  bus_lists = Debugger.breakpoints_vram_space;  break;
    case BREAKPOINT_LOCATION_PRAM:  bus_lists = Debugger.breakpoints_pram_space;  break;
    default:			    assert(0); return;
    }
    for (addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)
        list_add(&bus_lists[addr], breakpoint);

    // Add to CPU exec trap
    if (breakpoint->location == BREAKPOINT_LOCATION_CPU)
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            for (addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)// = (addr + 1) & 0xffff)
                Debugger_CPU_Exec_Traps[addr]++;
}

void                     Debugger_BreakPoint_Disable(t_debugger_breakpoint *breakpoint)
{
    int         addr;
    t_list **   bus_lists;

    // Set flag
    breakpoint->enabled = FALSE;

    // Remove from bus space list
    switch (breakpoint->location)
    {
    case BREAKPOINT_LOCATION_CPU:   bus_lists = Debugger.breakpoints_cpu_space;   break;
    case BREAKPOINT_LOCATION_IO:    bus_lists = Debugger.breakpoints_io_space;    break;
    case BREAKPOINT_LOCATION_VRAM:  bus_lists = Debugger.breakpoints_vram_space;  break;
    case BREAKPOINT_LOCATION_PRAM:  bus_lists = Debugger.breakpoints_pram_space;  break;
    default:			    assert(0); return;
    }
    for (addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)
        list_remove(&bus_lists[addr], breakpoint, NULL);

    // Remove CPU exec trap
    if (breakpoint->location == BREAKPOINT_LOCATION_CPU)
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            for (addr = breakpoint->address_range[0]; addr <= breakpoint->address_range[1]; addr++)// = (addr + 1) & 0xffff)
                Debugger_CPU_Exec_Traps[addr]--;
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

void                        Debugger_BreakPoint_GetSummaryLine(t_debugger_breakpoint *breakpoint, char *buf)
{
    char    addr_string[16];
    int     bus_size;
    t_debugger_bus_info *bus_info = &DebuggerBusInfos[breakpoint->location];
    
    bus_size = bus_info->bus_addr_size; 

    if (breakpoint->address_range[0] != breakpoint->address_range[1])
        sprintf(addr_string, "%0*X..%0*X", bus_size * 2, breakpoint->address_range[0], bus_size * 2, breakpoint->address_range[1]);
    else
        sprintf(addr_string, "%0*X", bus_size * 2, breakpoint->address_range[0]);

    sprintf (buf, "%c%d%c %s %-4s  %c%c%c  %-9s  %s%s", 
        breakpoint->enabled ? '[' : '(',
        breakpoint->id, 
        breakpoint->enabled ? ']' : ')',
        (breakpoint->type == BREAKPOINT_TYPE_BREAK) ? "Break" : "Watch",
        bus_info->name, 
        (bus_info->access & BREAKPOINT_ACCESS_R ? ((breakpoint->access_flags & BREAKPOINT_ACCESS_R) ? 'R' : '.') : ' '),
        (bus_info->access & BREAKPOINT_ACCESS_W ? ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) ? 'W' : '.') : ' '),
        (bus_info->access & BREAKPOINT_ACCESS_X ? ((breakpoint->access_flags & BREAKPOINT_ACCESS_X) ? 'X' : '.') : ' '),
        addr_string,
        (breakpoint->enabled == FALSE) ? "(disabled) " : "",
        breakpoint->desc ? breakpoint->desc : ""
        );
    Trim_End(buf);
}

void                        Debugger_BreakPoint_ActivatedVerbose(t_debugger_breakpoint *breakpoint, int access, int addr, int value)
{
    t_debugger_bus_info *bus_info = &DebuggerBusInfos[breakpoint->location];
    const char *action;

    // Debugger_Printf("Debugger_BreakPoint_ActivatedVerbose() %04x", addr);

    // Action
    if (breakpoint->type == BREAKPOINT_TYPE_BREAK)
    {
        // Break
        sms.R.Trace = 1;
        action = "break";
    }
    else
    {
        // Watch
        if (++Debugger.watch_counter >= 100)
        {
            if (Debugger.watch_counter == 100)
                Debugger_Printf("Maximum number of watch triggered this frame (100)\nWill stop displaying more, to prevent flood.\nConsider removing/tuning your watchpoints.");
            return;
        }
        action = "watch";
    }

    // Verbose to user
    if (access & BREAKPOINT_ACCESS_R)
    {
        Debugger_Printf("%04X: [%d] %s %s read from %0*X, read value=%02x", 
            Debugger_Z80_PC_Last,
            breakpoint->id,
            action,
            bus_info->name,
            bus_info->bus_addr_size * 2,
            addr,
            value);
    }
    else if (access & BREAKPOINT_ACCESS_W)
    {
        Debugger_Printf("%04X: [%d] %s %s write to %0*X, value=%02x", 
            Debugger_Z80_PC_Last,
            breakpoint->id,
            action,
            bus_info->name,
            bus_info->bus_addr_size * 2,
            addr,
            value);
    }
    else if (access & BREAKPOINT_ACCESS_X)
    {
        Debugger_Printf("%04X: [%d] %s %s execution", 
            Debugger_Z80_PC_Last,
            breakpoint->id,
            action,
            bus_info->name);
    }
    else
    {
        assert(0);
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - SYMBOLS
//-----------------------------------------------------------------------------

void     Debugger_Symbols_Load(void)
{
    char        symbol_filename[FILENAME_MAX];
    t_tfile *   symbol_file;
    t_list *    lines;
    int         line_cnt;

    // First, clear any existing symbol
    Debugger_Symbols_Clear();

    // Load symbol file
    // 1. Try "image.sym"
    strcpy(symbol_filename, file.rom);
    killext(symbol_filename);
    strcat(symbol_filename, ".sym");
    symbol_file = tfile_read(symbol_filename);
    if (symbol_file == NULL)
    {
        // Note: we silently fail on MEKA_ERR_FILE_OPEN (file not found / cannot open)
        if (meka_errno != MEKA_ERR_FILE_OPEN)
            Msg(MSGT_USER, meka_strerror());

        // 2. Try "image.ext.sym"
        snprintf(symbol_filename, FILENAME_MAX, "%s.sym", file.rom);
        symbol_file = tfile_read(symbol_filename);
        if (symbol_file == NULL)
        {
            if (meka_errno != MEKA_ERR_FILE_OPEN)
                Msg(MSGT_USER, meka_strerror());
            return;
        }
    }
    killpath(symbol_filename);

    line_cnt = 0;
    for (lines = symbol_file->data_lines; lines; lines = lines->next)
    {
        char *p;
        char *line = lines->elem;
        line_cnt += 1;

        // Msg (MSGT_DEBUG, "%s", line);

        // Strip comments, skip empty lines
        // FIXME: Can't we have some proper functions/tools to do this kind of thing?
        p = strchr(line, ';');
        if (p != NULL)
            *p = EOSTR;
        Trim(line);
        if (StrNull(line))
            continue;

        // Parse
        {
            u16 bank;
            u16 addr;
            char *name;
            int n;
            if (sscanf(line, "%hX:%hX %n", &bank, &addr, &n) < 2)
            {
                Msg(MSGT_USER, Msg_Get(MSG_Debug_Symbols_Error));
                Msg(MSGT_USER_BOX, Msg_Get(MSG_Debug_Symbols_Error_Line), line_cnt);
                tfile_free(symbol_file);
                return;
            }
            line += n;
            skip_spaces(&line);
            name = parse_getword(NULL, -1, &line, " \t\r\n", ';', 0);
            if (name == NULL)
            {
                Msg(MSGT_USER, Msg_Get(MSG_Debug_Symbols_Error), symbol_filename);
                Msg(MSGT_USER_BOX, Msg_Get(MSG_Debug_Symbols_Error_Line), line_cnt);
                tfile_free(symbol_file);
                return;
            }
            Debugger_Symbol_Add(addr, bank, name);
        }
    
    }

    // Free symbol file data
    tfile_free(symbol_file);

    // Sort symbols
    list_sort(&Debugger.symbols, (int (*)(void *, void *))Debugger_Symbol_CompareByAddress);

    // Verbose
    Msg(MSGT_USER, Msg_Get(MSG_Debug_Symbols_Loaded), Debugger.symbols_count, symbol_filename);
    Debugger_Printf(Msg_Get(MSG_Debug_Symbols_Loaded), Debugger.symbols_count, symbol_filename);
}

void    Debugger_Symbols_Clear(void)
{
    t_list *symbols;
    for (symbols = Debugger.symbols; symbols!= NULL; )
    {
        t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
        symbols = symbols->next;
        Debugger_Symbol_Remove(symbol);
    }
    assert(Debugger.symbols_count == 0);
}

void    Debugger_Symbols_List(char *search_name)
{
    t_list *symbols;
    int count;
    
    if (search_name)
    {
        Debugger_Printf("Symbols matching \"%s\":", search_name);
        search_name = strdup(search_name);
        strupr(search_name);
    }
    else
    {
        Debugger_Printf("Symbols:");
    }

    count = 0;
    for (symbols = Debugger.symbols; symbols != NULL; symbols = symbols->next)
    {
        t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;

        // If search_name was specified, skip symbol not matching it
        if (search_name != NULL)
            if (strstr(symbol->name_uppercase, search_name) == NULL)
                continue;
        count++;
        Debugger_Printf(" %04X  %s", symbol->addr, symbol->name);
    }
    if (count == 0)
    {
        Debugger_Printf(" <None>");
    }
    else
    {
        Debugger_Printf("%d symbols", count);
    }
    if (search_name != NULL)
    {
        free(search_name);
    }
}

t_debugger_symbol *     Debugger_Symbols_GetFirstByAddr(int addr)
{
    t_list *symbols = Debugger.symbols_cpu_space[(u16)addr];
    if (symbols == NULL)
        return (NULL);
    return ((t_debugger_symbol *)symbols->elem);
}

// Note: this function is useful, as there's often cases where the programmer sets 
// one 'end' symbol and a following 'start' symbol and they are at the same address.
// In most of those cases, we want the last one.
t_debugger_symbol *     Debugger_Symbols_GetLastByAddr(int addr)
{
    t_list *symbols = Debugger.symbols_cpu_space[(u16)addr];
    if (symbols == NULL)
        return (NULL);
    while (symbols->next != NULL)
        symbols = symbols->next;
    return ((t_debugger_symbol *)symbols->elem);
}

//-----------------------------------------------------------------------------
// FUNCTIONS - SYMBOL
//-----------------------------------------------------------------------------

t_debugger_symbol *     Debugger_Symbol_Add(u16 addr, int bank, const char *name)
{
    t_debugger_symbol * symbol;

    // Check parameters
    assert(name != NULL);

    // Create and setup symbol
    symbol = malloc(sizeof (t_debugger_symbol));
    symbol->addr = addr;
    symbol->bank = bank;
    symbol->name = (char *)name;
    symbol->name_uppercase = strdup(name);
    strupr(symbol->name_uppercase);

    // Add to global symbol list and CPU space list
    list_add(&Debugger.symbols, symbol);
    list_add_to_end(&Debugger.symbols_cpu_space[symbol->addr], symbol);

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
    list_remove(&Debugger.symbols, symbol, NULL);
    list_remove(&Debugger.symbols_cpu_space[symbol->addr], symbol, NULL);

    // Delete
    free(symbol->name);
    free(symbol->name_uppercase);
    free(symbol);

    // Decrease global counter
    Debugger.symbols_count--;
    assert(Debugger.symbols_count >= 0);
}

int     Debugger_Symbol_CompareByAddress(t_debugger_symbol *symbol1, t_debugger_symbol *symbol2)
{
    return (symbol1->addr - symbol2->addr);
}

//-----------------------------------------------------------------------------
// FUNCTIONS - HOOKS
//-----------------------------------------------------------------------------

// Hook Z80 read/write and I/O
void     Debugger_Hooks_Install(void)
{
    RdZ80 = Debugger_RdZ80_Hook;
    WrZ80 = Debugger_WrZ80_Hook;
    InZ80 = Debugger_InZ80_Hook;
    OutZ80 = Debugger_OutZ80_Hook;
}

// Unhook Z80 read/write and I/O
void     Debugger_Hooks_Uninstall(void)
{
    RdZ80 = RdZ80_NoHook;
    WrZ80 = WrZ80_NoHook;
    InZ80 = InZ80_NoHook;
    OutZ80 = OutZ80_NoHook;
}

void        Debugger_WrZ80_Hook(register int addr, register u8 value)
{
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_cpu_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
    WrZ80_NoHook(addr, value);
}

u8          Debugger_RdZ80_Hook(register int addr)
{
    u8 value = RdZ80_NoHook(addr);
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_cpu_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Special case, if a X handler is installed and we're now executing, ignore R breakpoint
            // It is not logical but much better for end user, who is likely to use RWX in most cases
            if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            {
                if (addr >= Debugger_Z80_PC_Last && addr <= Debugger_Z80_PC_Last + 6)   // quick check to 6
                    if (addr <= Debugger_Z80_PC_Last + Z80_Disassemble(NULL, Debugger_Z80_PC_Last, FALSE, FALSE))
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
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_io_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
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
    u8 value = InZ80_NoHook(addr);
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_io_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_R, addr, value);
        }        
    }
    return (value);
}

void            Debugger_RdVRAM_Hook(register int addr, register u8 value)
{
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_vram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_R)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_R, addr, value);
        }        
    }
}

void            Debugger_WrVRAM_Hook(register int addr, register u8 value)
{
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_vram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
}

void            Debugger_WrPRAM_Hook(register int addr, register u8 value)
{
    t_list *breakpoints;
    for (breakpoints = Debugger.breakpoints_pram_space[addr]; breakpoints != NULL; breakpoints = breakpoints->next)
    {
        t_debugger_breakpoint *breakpoint = breakpoints->elem;
        if (breakpoint->access_flags & BREAKPOINT_ACCESS_W)
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
}

//-----------------------------------------------------------------------------
// Debugger_Switch ()
//-----------------------------------------------------------------------------
// Enable or disable the debugger window and associated processing.
//-----------------------------------------------------------------------------
void        Debugger_Switch (void)
{
    // Msg (MSGT_DEBUG, "Debugger_Switch()");
    if (!Debugger.Enabled)
        return;
    Debugger.Active ^= 1;
    gui_box_show (DebuggerApp.box, Debugger.Active, TRUE);
    if (Debugger.Active)
    {
        gui_menu_check (menus_ID.debug, 0);
        Machine_Debug_Start ();
        // ??
        // Meka_Z80_Debugger_SetBreakPoint (Debugger.break_point_address);
    }
    else
    {
        gui_menu_un_check_one (menus_ID.debug, 0);
        Machine_Debug_Stop ();
        sms.R.Trap = 0xFFFF;
    }

    // Setup/disable hooks
    if (Debugger.Active)
    {
        Debugger_Hooks_Install();
    }
    else
    {
        Debugger_Hooks_Uninstall();
    }

    // Open log file (if not already open)
    if (Debugger.Active)
        Debugger_Init_LogFile();
}

//-----------------------------------------------------------------------------
// Debugger_Printf ()
//-----------------------------------------------------------------------------
// Print a formatted line to the debugger console
//-----------------------------------------------------------------------------
void        Debugger_Printf(const char *format, ...)
{
    char    buf[1024];
    va_list param_list;
    char *  p;

    va_start (param_list, format);
    vsprintf (buf, format, param_list);
    va_end (param_list);

    // Log to file
    if (Debugger.log_file != NULL)
    {
        fprintf(Debugger.log_file, "%s\n", buf);
        fflush(Debugger.log_file);
    }

    // Split message by line (\n) and send it to the various places
    p = buf;
    do
    {
        char *line = p;
        p = strchr (p, '\n');
        if (p)
        {
            *p++ = EOSTR;
        }
        widget_textbox_print_scroll (DebuggerApp.console, TRUE, line);
    }
    while (p != NULL);
}

//-----------------------------------------------------------------------------
// Debugger_Applet_Init ()
//-----------------------------------------------------------------------------
// Initialize the debugger applet
//-----------------------------------------------------------------------------
static void Debugger_Applet_Init (void)
{
    int     box_id;
    int     font_id;
    int     font_height;
    t_frame frame;

    // Create box
    DebuggerApp.font_id = font_id = F_MIDDLE;
    DebuggerApp.font_height = font_height = Font_Height (font_id);
    frame.pos.x     = 280;
    frame.pos.y     = 50;
    frame.size.x    = 350;
    frame.size.y    = ((Configuration.debugger_console_lines + 1 + Configuration.debugger_disassembly_lines + 1 + DEBUGGER_APP_CPUSTATE_LINES) * font_height) + 20 + (2*2); // 2*2=padding

    box_id = gui_box_create (frame.pos.x, frame.pos.y, frame.size.x, frame.size.y, DEBUGGER_APP_TITLE);
    DebuggerApp.box = gui.box[box_id];
    DebuggerApp.box_gfx = create_bitmap (frame.size.x + 1, frame.size.y + 1);
    gui_set_image_box (box_id, DebuggerApp.box_gfx);

    // Set exclusive inputs flag to avoid messing with emulation
    DebuggerApp.box->focus_inputs_exclusive = YES;

    // Register to desktop (applet is disabled by default)
    Desktop_Register_Box ("DEBUG", box_id, NO, &Debugger.Active);

    // Add closebox widget
    widget_closebox_add (box_id, Debugger_Switch);

    // Add console (textbox widget)
    frame.pos.x = 6;
    frame.pos.y = 2;
    frame.size.x = DebuggerApp.box->frame.size.x - (6*2);
    frame.size.y = Configuration.debugger_console_lines * font_height;
    DebuggerApp.console = widget_textbox_add(box_id, &frame, Configuration.debugger_console_lines, font_id);
    frame.pos.y += frame.size.y;

    // Add line
    hline (DebuggerApp.box_gfx, frame.pos.x, frame.pos.y + font_height / 2, frame.pos.x + frame.size.x, GUI_COL_BORDERS);
    frame.pos.y += font_height;

    // Setup disassembly frame
    DebuggerApp.frame_disassembly.pos.x   = frame.pos.x;
    DebuggerApp.frame_disassembly.pos.y   = frame.pos.y;
    DebuggerApp.frame_disassembly.size.x  = frame.size.x;
    DebuggerApp.frame_disassembly.size.y  = Configuration.debugger_disassembly_lines * font_height;
    frame.pos.y += DebuggerApp.frame_disassembly.size.y;

    // Add line
    hline (DebuggerApp.box_gfx, frame.pos.x, frame.pos.y + font_height / 2, frame.pos.x + frame.size.x, GUI_COL_BORDERS);
    frame.pos.y += font_height;

    // Setup CPU state frame
    DebuggerApp.frame_cpustate.pos.x   = frame.pos.x;
    DebuggerApp.frame_cpustate.pos.y   = frame.pos.y;
    DebuggerApp.frame_cpustate.size.x  = frame.size.x;
    DebuggerApp.frame_cpustate.size.y  = DEBUGGER_APP_CPUSTATE_LINES * font_height;
    frame.pos.y += DebuggerApp.frame_cpustate.size.y;

    // Add input box
    frame.pos.x = 4;
    frame.pos.y = DebuggerApp.box->frame.size.y - 16 - 2;
    frame.size.x = DebuggerApp.box->frame.size.x - (4*2);
    frame.size.y = 16;
    DebuggerApp.input_box = widget_inputbox_add(box_id, &frame, 56, F_MIDDLE, Debugger_InputBoxCallback);
    widget_inputbox_set_flags(DebuggerApp.input_box, WIDGET_INPUTBOX_FLAG_COMPLETION, TRUE);
    widget_inputbox_set_callback_completion(DebuggerApp.input_box, Debugger_CompletionCallback);
    //widget_inputbox_set_flags(DebuggerApp.input_box, WIDGET_INPUTBOX_FLAG_HISTORY, TRUE);
    //widget_inputbox_set_callback_history(DebuggerApp.input_box, Debugger_History_Callback);
}

//-----------------------------------------------------------------------------
// Debugger_Disassemble_Format (char *dst, u16 addr)
//-----------------------------------------------------------------------------
// Disassemble one instruction at given address and produce a formatted 
// string with address, opcode and instruction description.
// Return instruction length.
//-----------------------------------------------------------------------------
int         Debugger_Disassemble_Format (char *dst, u16 addr)
{
    int     len;
    char    instr[128];

    len = Z80_Disassemble(instr, addr, TRUE, TRUE);
    if (dst != NULL)
    {
        int     i;
        char    instr_opcodes[128];
        for (i = 0; i < len; i++)
            sprintf(instr_opcodes + (i*3), "%02X ", RdZ80_NoHook ((addr + i) & 0xFFFF));
        sprintf(dst, "%04X: %-12s %s", addr, instr_opcodes, instr);
    }

    return (len);
}

//-----------------------------------------------------------------------------
// Debugger_GetZ80SummaryLines(const char ***lines_out)
// Return array of string pointer containing a Z80 summary
//-----------------------------------------------------------------------------
// Note: output array contains reference to static buffers. 
// Be sure to make a copy if you want to reuse later.
//-----------------------------------------------------------------------------
static int  Debugger_GetZ80SummaryLines(const char ***lines_out)
{
    int             i;
    static char     line1[256];
    static char     line2[256];
    static char *   lines[3] = { line1, line2, NULL };
    char            flags[9];
    Z80 *           cpu = &sms.R;

   // Compute flags string
   for (i = 0; i < 8; i++)
       flags[i] = (cpu->AF.B.l & (1 << (7 - i))) ? "SZyHxPNC"[i] : '.';
   flags[i] = EOSTR;

   // Line 1
   sprintf(line1, "AF:%04X  BC:%04X  HL:%04X  DE:%04X  IX:%04X  IY:%04X",
       cpu->AF.W, cpu->BC.W, cpu->HL.W, cpu->DE.W, cpu->IX.W, cpu->IY.W);

   // Line 2
   sprintf(line2, "PC:%04X  SP:%04X  Flags:[%s]  %s%s", 
       cpu->PC.W, cpu->SP.W, flags,
       (cpu->IFF & IFF_1) ? "EI" : "DI", (cpu->IFF & IFF_HALT) ? " HALT" : "");

   *lines_out = lines;
   return (2);
}

//-----------------------------------------------------------------------------
// Debugger_Redraw_State()
// Redraw disassembly and CPU state
//-----------------------------------------------------------------------------
void        Debugger_Redraw_State(void)
{
    int     i;

    if (!(machine & MACHINE_POWER_ON))
        return;
    if (cur_drv->cpu != CPU_Z80)    // Unsupported
        return;

    // Redraw Disassembly
    {
        t_frame frame = DebuggerApp.frame_disassembly;

        u16     pc;
        int     skip_labels = 0;    // Number of labels to skip on first instruction to be aligned properly
        int     trackback_lines = ((Configuration.debugger_disassembly_lines - 1) / 4) + 1; 
        trackback_lines = MIN(trackback_lines, 10); // Max 10
        //  1 -> 1
        //  5 -> 2
        //  9 -> 3, etc.
        // 13 -> 4
        // Max = 10

        // Clear disassembly buffer
        rectfill (DebuggerApp.box_gfx, frame.pos.x, frame.pos.y, frame.pos.x + frame.size.x, frame.pos.y + frame.size.y, GUI_COL_FILL);

        // Figure out where to start disassembly
        // This is tricky code due to the trackback feature.
        // Successives PC are logged by the debugging CPU emulator and it helps with the trackback.
        pc = sms.R.PC.W;
        {
            int pc_temp = pc;
            int pc_history[10*4+1] = { 0 };
            int pc_history_size = trackback_lines*4;
            int i;

            // Find in PC log all values between PC-trackback_lines*4 and PC-1
            // The *4 is assuming instruction can't be more than 4 bytes averagely
            // Build pc_history[] table and fill it with instruction length when found in log.
            // If it happens that a previous instruction is more than trackback_lines*4 bytes before PC, 
            // then the trackback feature won't find the previous instruction. This is not a big problem
            // and it's extreme rare anyway (multi prefixes, etc).
            for (i = Debugger_Z80_PC_Log_Queue_First; i != Debugger_Z80_PC_Log_Queue_Write; i = (i + 1) & 255)
            {
                int delta = pc - Debugger_Z80_PC_Log_Queue[i];
                if (delta > 0 && delta <= pc_history_size)
                    pc_history[delta] = Z80_Disassemble(NULL, Debugger_Z80_PC_Log_Queue[i], FALSE, FALSE);
            }

            // Now look in pc_history
            for (i = 0; i < pc_history_size; i++)
            {
                if (pc_history[i] != 0)
                {
                    // Count instruction up to PC
                    int inst_len = pc_history[i];
                    int inst_after = pc-i + inst_len;

                    // Msg (0, "PC History -%02x : %04x (%d)", i, pc-i, inst_len);

                    while (inst_after < pc_temp)
                    {
                        // Retrieve next instruction until reaching PC
                        // eg:
                        //   0000 - known inst (3)
                        //   0003 - ? <- get this
                        //   0005 - PC
                        //..
                        pc_history[pc_temp - inst_after] = inst_len = Z80_Disassemble(NULL, inst_after, FALSE, FALSE);
                        i = pc_temp - inst_after;
                        inst_after += inst_len;
                    }

                    // Went after PC, something was wrong. Might happen on data bytes between close code, etc.
                    // FIXME: This actually happens, see TODO.TXT. Requires fix, workaround, or silent fail.
                    if (inst_after > pc_temp)
                    {
                        // Msg (0, "[Warning] inst_after = %04x > pc_temp = %04x", inst_after, pc_temp);
                        break;
                    }

                    // This is the instruction right before pc_temp!
                    // We successfully trackbacked one instruction.
                    if (inst_after == pc_temp)
                    {
                        pc_temp -= inst_len;
                        trackback_lines--;
                        if (Debugger.symbols_cpu_space[pc_temp])
                            trackback_lines -= list_size(Debugger.symbols_cpu_space[pc_temp]);
                    }

                    // No more trackback to do
                    if (trackback_lines <= 0)
                        break;
                }
            }
            pc = pc_temp;
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
        for (i = 0; i < Configuration.debugger_disassembly_lines; i++)
        {
            char buf[256];
            int text_color = (pc == sms.R.PC.W) ? GUI_COL_TEXT_ACTIVE : GUI_COL_TEXT_IN_BOX;

            if (Configuration.debugger_disassembly_display_labels)
            {
                // Display symbols/labels
                if (Debugger.symbols_cpu_space[pc] != NULL)
                {
                    t_list *symbols;
                    for (symbols = Debugger.symbols_cpu_space[pc]; symbols != NULL; symbols = symbols->next)
                    {
                        t_debugger_symbol *symbol = symbols->elem;
                        if (skip_labels > 0)
                        {
                            skip_labels--;
                            continue;
                        }
                        sprintf(buf, "%s:", symbol->name);
                        Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, buf, frame.pos.x, frame.pos.y + (i * DebuggerApp.font_height), GUI_COL_TEXT_IN_BOX);
                        i++;
                        if (i >= Configuration.debugger_disassembly_lines)
                            break;
                    }
                    if (i >= Configuration.debugger_disassembly_lines)
                        break;
                }
            }

            // The trick here is to re-add all disassembled instruction into the PC log queue
            //Debugger_Z80_PC_Log_Queue_Add(pc);

            // Disassemble
            if (Configuration.debugger_disassembly_display_labels && Debugger.symbols_count != 0)
            {
                buf[0] = ' ';
                pc += Debugger_Disassemble_Format(buf + 1, pc);
            }
            else
            {
                pc += Debugger_Disassemble_Format(buf, pc);
            }
            Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, buf, frame.pos.x, frame.pos.y + (i * DebuggerApp.font_height), text_color);
        }
    }

    // Redraw CPU State
    {
        char ** lines;
        int     lines_cnt;
        int     y;

        // Clear CPU state buffer
        t_frame frame = DebuggerApp.frame_cpustate;
        rectfill (DebuggerApp.box_gfx, frame.pos.x, frame.pos.y, frame.pos.x + frame.size.x, frame.pos.y + frame.size.y, GUI_COL_FILL);
        y = frame.pos.y;

        // Print Z80 summary lines
        lines_cnt = Debugger_GetZ80SummaryLines(&lines); 
        assert(lines_cnt == DEBUGGER_APP_CPUSTATE_LINES); // Should be 2 lines
        Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, lines[0], frame.pos.x, y, GUI_COL_TEXT_IN_BOX);
        y += DebuggerApp.font_height;
        Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, lines[1], frame.pos.x, y, GUI_COL_TEXT_IN_BOX);
        
        // Print Z80 running state
        if (!(machine & (MACHINE_PAUSED | MACHINE_DEBUGGING)))
        {
            static int running_counter = 0;
            char *running_string;
            switch (running_counter >> 1)
            {
                case 0: running_string = "RUNNING |"; break;
                case 1: running_string = "RUNNING /"; break;
                case 2: running_string = "RUNNING -"; break;
                case 3: running_string = "RUNNING \\"; break;
            }
            Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, running_string, frame.pos.x + frame.size.x - 68, y, GUI_COL_TEXT_IN_BOX);
            running_counter = (running_counter + 1) % 8;
        }

    }

    // Set redraw dirty flag
    DebuggerApp.box->must_redraw = YES;
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
        Debugger_Printf ("Debugger Help:" );
        Debugger_Printf ("-- Flow:");
        Debugger_Printf (" <CR>                  : Step into"                 );
        Debugger_Printf (" S                     : Step over"                 ) ;
        Debugger_Printf (" C [addr]              : Continue (up to <addr>)"   );
        Debugger_Printf (" J addr                : Jump to <addr>"            );
        Debugger_Printf ("-- Breakpoints:");
        Debugger_Printf (" B [access] [bus] addr : Add breakpoint"            );
        Debugger_Printf (" B LIST                : List breakpoints"          );
        Debugger_Printf (" B REMOVE n            : Remove breakpoint"         );
        Debugger_Printf (" B ENABLE/DISABLE n    : Enable/disable breakpoint" );
        Debugger_Printf (" B CLEAR               : Clear breakpoints"         );
        Debugger_Printf (" W [access] [bus] addr : Add watchpoint"            );
        Debugger_Printf ("-- Inspect:");
        Debugger_Printf (" P expr                : Print evaluated expression");
        Debugger_Printf (" SYM [name]            : List symbols"              );
        Debugger_Printf (" M [addr] [len]        : Memory dump at <addr>"     );
        Debugger_Printf (" D [addr] [cnt]        : Disassembly at <addr>"     );
        Debugger_Printf (" R                     : Dump Z80 registers"        );
        Debugger_Printf (" H,? [command]         : Help"                      );
        Debugger_Printf ("Use H for detailed help on individual command."  );
    }
    else
    {
        // Search for specific command
        t_debugger_command_info *command_info = &DebuggerCommandInfos[0];
        while (command_info->command_short != NULL)
        {
            if (!stricmp(cmd, command_info->command_short) || (command_info->command_long && !stricmp(cmd, command_info->command_long)))
            {
                Debugger_Printf("%s", command_info->description);
                return;
            }
            command_info++;
        }
        Debugger_Printf("Unknown command \"%s\" !", cmd);
    }
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
    char *  line;
    char    cmd[64];
    char    arg[256];
    int     cmd_id;

    strcpy (line_buf, widget_inputbox_get_value(DebuggerApp.input_box));
    Trim (line_buf);
    line = line_buf;

    // Add input to history
    //Debugger_History_AddLine(line);

    // Clear input box
    widget_inputbox_set_value(DebuggerApp.input_box, "");

    // An empty line means step into or activate debugging
    if (line_buf[0] == EOSTR)
    {
        if (machine & MACHINE_POWER_ON)
        {
            // If machine is in PAUSE state, consider is the same as in DEBUGGING state
            // (Allows step during pause)
            if ((machine & MACHINE_DEBUGGING) || (machine & MACHINE_PAUSED))
            {
                // Step into
                Debugger.stepping = 1;
                Debugger.stepping_trace_after = sms.R.Trace = 1;
                Machine_Debug_Stop ();
            }
            else
            {
                // Activate debugging
                Debugger.stepping = 0;
                Debugger_Printf("Breaking at %04Xh", sms.R.PC.W);
                Debugger_Redraw_State();
                Machine_Debug_Start();
                //Debugger_Hook (&sms.R);
                sms.R.Trace = 1;
            }
        }
        return;
    }

    // Print line to the console, as a user command log
    widget_textbox_set_current_color (DebuggerApp.console, GUI_COL_TEXT_ACTIVE);
    Debugger_Printf ("# %s", line);
    widget_textbox_set_current_color (DebuggerApp.console, GUI_COL_TEXT_IN_BOX);

    // Process command
    parse_getword(cmd, sizeof(cmd), &line, " ", 0, 0);
    strupr(cmd);

    // H - HELP
    if (!strcmp(cmd, "H") || !strcmp(cmd, "?") || !strcmp(cmd, "HELP"))
    {
        if (parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            Debugger_Help(arg);
        else
            Debugger_Help(NULL);
        return;
    }
    
    // HI - HISTORY
    /* if (!strcmp(cmd, "HI") || !strcmp(cmd, "HISTORY"))
    {
        Debugger_History_List();
        return;
    } */

    // B - BREAKPOINT
    cmd_id = 0;
    if (!strcmp(cmd, "B") || !strcmp(cmd, "BR") || !strcmp(cmd, "BRK") || !strcmp(cmd, "BREAK"))
        cmd_id = 1;
    if (!strcmp(cmd, "W") || !strcmp(cmd, "WATCH"))
        cmd_id = 2;
    if (cmd_id != 0)
    {
        int access = 0;
        int location = -1;

        if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
        {
            if (cmd_id == 1) // Break
                Debugger_Help("B");
            else if (cmd_id == 2) // Watch
                Debugger_Help("W");
            return;
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
            if (parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                if (!stricmp(arg, "disabled"))
                {
                    Debugger_BreakPoints_Clear(TRUE);
                    return;
                }
                Debugger_Printf("Syntax error!");
                Debugger_Printf("Type HELP B for usage instruction.");
                return;
            }
            Debugger_BreakPoints_Clear(FALSE);
            return;
        }

        // B ENABLE
        if (!stricmp(arg, "enable"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                if (!stricmp(arg, "ALL"))
                {
                    t_list *breakpoints;
                    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
                    {
                        t_debugger_breakpoint *breakpoint = breakpoints->elem;
                        Debugger_BreakPoint_Enable(breakpoint);
                    }
                    Debugger_Printf("Enabled all breakpoints/watchpoints.");
                }
                else
                {
                    int id = atoi(arg);
                    t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
                    if (breakpoint)
                    {
                        char buf[256];
                        if (breakpoint->enabled)
                        {
                            Debugger_Printf("%s [%d] already enabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                        }
                        else
                        {
                            Debugger_BreakPoint_Enable(breakpoint);
                            Debugger_Printf("%s [%d] enabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                        }
                        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
                        Debugger_Printf(" %s", buf);
                    }
                    else
                    {
                        Debugger_Printf("Breakpoint [%s] not found!", arg);
                    }
                }
            }
            return;
        }

        // B DISABLE
        if (!stricmp(arg, "disable"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                if (!stricmp(arg, "ALL"))
                {
                    t_list *breakpoints;
                    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
                    {
                        t_debugger_breakpoint *breakpoint = breakpoints->elem;
                        Debugger_BreakPoint_Disable(breakpoint);
                    }
                    Debugger_Printf("Disabled all breakpoints/watchpoints.");
                }
                else
                {
                    int id = atoi(arg); // FIXME: no error check
                    t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
                    if (breakpoint)
                    {
                        char buf[256];
                        if (!breakpoint->enabled)
                        {
                            Debugger_Printf("%s [%d] already disabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                        }
                        else
                        {
                            Debugger_BreakPoint_Disable(breakpoint);
                            Debugger_Printf("%s [%d] disabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                        }
                        Debugger_BreakPoint_GetSummaryLine(breakpoint, buf);
                        Debugger_Printf(" %s", buf);
                    }
                    else
                    {
                        Debugger_Printf("Breakpoint [%s] not found!", arg);
                    }
                }
            }
            return;
        }

        // B REMOVE
        if (!stricmp(arg, "remove"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                int id = atoi(arg); // FIXME: no error check
                t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
                if (breakpoint)
                {
                    Debugger_Printf("%s [%d] removed.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    Debugger_BreakPoint_Remove(breakpoint);
                }
                else
                {
                    Debugger_Printf("Breakpoint [%s] not found!", arg);
                }
            }
            return;
        }

        // If no known argument, revert to adding breakpoint

        // Access
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
                else
                {
                    access = 0;
                    break;
                }
            }
            if (cmd_id == 2 && (access & BREAKPOINT_ACCESS_X)) // Watch
            {
                Debugger_Printf("Cannot watch execution. Use breakpoints.");
                return;
            }
        }
        if (access != 0)
        {
            // Get next argument
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                Debugger_Printf("Missing parameter!");
                Debugger_Printf("Type HELP B/W for usage instruction.");
                return;
            }
        }

        // Bus
        if (!stricmp(arg, "CPU"))
            location = BREAKPOINT_LOCATION_CPU;
        else if (!stricmp(arg, "IO"))
            location = BREAKPOINT_LOCATION_IO;
        else if (!stricmp(arg, "VRAM"))
            location = BREAKPOINT_LOCATION_VRAM;
        else if (!stricmp(arg, "PAL") || !stricmp(arg, "PRAM"))
            location = BREAKPOINT_LOCATION_PRAM;
        if (location == -1)
        {
            // Default
            location = BREAKPOINT_LOCATION_CPU;
        }
        else
        {
            // Get next argument
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            {
                Debugger_Printf("Syntax error!");
                Debugger_Printf("Type HELP B/W for usage instruction.");
                return;
            }
        }

        // Now that we now the bus, check access
        if (access == 0)
        {
            // Default
            access = DebuggerBusInfos[location].access;
            if (cmd_id == 2) // Watch, automatically remove X
                access &= ~BREAKPOINT_ACCESS_X;
        }
        else
        {
            // Check user-given access rights
            int access_unpermitted = access & ~DebuggerBusInfos[location].access;
            if (access_unpermitted != 0)
            {
                char buf[4];
                Debugger_GetAccessString(access_unpermitted, buf);
                Debugger_Printf("Access %s not permitted on this bus.", buf);
                return;
            }
        }

        // Address
        {
            t_debugger_bus_info *bus_info = &DebuggerBusInfos[location];
            t_debugger_value     address_start;
            t_debugger_value     address_end;
            char *               p;

            // Clear out
            Debugger_Value_SetDirect(&address_start, -1, 16);
            Debugger_Value_SetDirect(&address_end,   -1, 16);

            // Parse different kind of ranges (A, A-, A-B, -B)
            p = arg;
            if (Debugger_Eval_GetExpression(&p, &address_start))
            {
                // Default is no range, so end==start
                address_start.data = (u16)address_start.data;
                address_end = address_start;
            }
            if (strncmp(p, "..", 2) == 0)
            {
                // Skip range points
                while (*p == '.')
                    p++;

                // Get second part of the range
                if (address_start.data == -1)
                    address_start.data = bus_info->addr_min;
                if (Debugger_Eval_GetExpression(&p, &address_end))
                    address_end.data = (u16)address_end.data;
                else
                    address_end.data = bus_info->addr_max;
            }

            if (address_start.data != -1 && address_end.data != -1)
            {
                // Check out address range
                if (address_end.data < address_start.data)
                {
                    Debugger_Printf("Second address in range must be higher.");
                    return;
                }
                if (address_start.data < (u32)bus_info->addr_min || address_start.data > (u32)bus_info->addr_max)
                {
                    Debugger_Printf("Address %X is out of %s range (%0*X-%0*X).", 
                        address_start.data, 
                        bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
                    return;
                }
                if (address_end.data < (u32)bus_info->addr_min || address_end.data > (u32)bus_info->addr_max)
                {
                    Debugger_Printf("Address %X is out of %s range (%0*X-%0*X).", 
                        address_end.data, 
                        bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
                    return;
                }

                // Get description (if any) and add breakpoint
                {
                    int type = (cmd_id == 1) ? BREAKPOINT_TYPE_BREAK : BREAKPOINT_TYPE_WATCH;

                    char *desc = NULL;
                    Trim(line);
                    if (strlen(line) > 0)
                        desc = line;
                    else 
                        if (address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL || address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL)
                        {
                            // Create automatic description containing symbol
                            static char buf[512];
                            if (address_start.source == DEBUGGER_VALUE_SOURCE_SYMBOL)
                                strcpy(buf, ((t_debugger_symbol *)address_start.source_data)->name);
                            else
                                sprintf(buf, "%04hX", address_start.data);
                            if (address_start.data != address_end.data)
                            {
                                strcat(buf, "..");
                                if (address_end.source == DEBUGGER_VALUE_SOURCE_SYMBOL)
                                    strcat(buf, ((t_debugger_symbol *)address_end.source_data)->name);
                                else
                                    sprintf(buf+strlen(buf), "%04hX", address_end.data);
                            }
                            desc = buf; // Ok since Debugger_BreakPoint_Add() does a strcpy
                        }

                    // Add breakpoint
                    Debugger_BreakPoint_Add(type, location, access, address_start.data, address_end.data, -1, desc);
                }
                return;
            }
            else
            {
                Debugger_Printf("Syntax error!");
                Debugger_Printf("Type HELP B for usage instruction.");
                return;
            }
        }

        if (parse_getword(arg, sizeof(arg), &line, " ", 0, 0))
            Debugger_Printf("Additionnal argument ignored.");

        return;
    }
    
    // C - CONTINUE
    if (!strcmp(cmd, "C") || !strcmp(cmd, "CONT") || !strcmp(cmd, "CONTINUE"))
    {
        t_debugger_value value;
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running");
            return;
        }

        if (!(machine & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start ();
        }

        if (Debugger_Eval_GetExpression(&line, &value))
        {
            // Continue up to...
            u16 addr = value.data;
            Debugger_Printf ("Continuing up to %04Xh", addr);
            Debugger_SetTrap (addr);
        }
        else
        {
            // Continue
            // Disable one-time trap
            Debugger_SetTrap (-1);
        }

        // Stop tracing
        sms.R.Trace = 0;
        Machine_Debug_Stop ();

        // Setup a single stepping so that the CPU emulator won't break
        // on the same address right now.
        Debugger.stepping = 1;
        Debugger.stepping_trace_after = 0;

        return;
    }

    // J - JUMP
    if (!strcmp(cmd, "J") || !strcmp(cmd, "JP") || !strcmp(cmd, "JUMP"))
    {
        t_debugger_value value;
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running");
            return;
        }
        if (!(machine & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start ();
        }
        if (Debugger_Eval_GetExpression(&line, &value))
        {
            sms.R.PC.W = value.data;
            Debugger_Printf("Jump to %04Xh", sms.R.PC.W);
            Debugger_Redraw_State ();
        }
        else
        {
            Debugger_Printf ("Missing parameter!");
        }
        return;
    }

    // S - STEP OVER
    if (!strcmp(cmd, "S") || !strcmp(cmd, "STEP"))
    {
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running");
        }
        else
        {
            // Get address of following instruction
            // Do not verbose since this is just a step-over
            u16 addr = sms.R.PC.W + Z80_Disassemble(NULL, sms.R.PC.W, FALSE, FALSE);
            Debugger_SetTrap (addr);
            sms.R.Trace = 0;
            Machine_Debug_Stop ();
        }
        return;
    }

    // P - PRINT
    if (!strcmp(cmd, "P") || !strcmp(cmd, "PRINT"))
    {
        t_debugger_value value;
        Trim(line);
        if (line[0])
        {
            char *p = line;
            while (*p && Debugger_Eval_GetExpression(&p, &value))
            {
                s16 data = value.data;
                int data_size_bytes = 2; //data & 0xFFFF0000) ? ((data & 0xFF000000) ? 4 : 3) : (2);
                char binary_s[2][9];
                char char_s[4];

                // Write binary buffer
                Write_Bits_Field((data >> 0) & 0xFF, 8, binary_s[0]);
                Write_Bits_Field((data >> 8) & 0xFF, 8, binary_s[1]);

                // Write ascii buffer
                if (isprint(data & 0xFF))
                    sprintf(char_s, "'%c'", data & 0xFF);
                else
                    sprintf(char_s, "N/A");
                Debugger_Printf(" $%0*hX  bin: %%%s.%s  asc: %s  dec: %d", data_size_bytes * 2, data, binary_s[1], binary_s[0], char_s, data);

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

    // R - REGS
    if (!strcmp(cmd, "R") || !strcmp(cmd, "REGS"))
    {
        char **lines;
        int lines_cnt = Debugger_GetZ80SummaryLines(&lines);
        int i;
        for (i = 0; i < lines_cnt; i++)
            Debugger_Printf("%s", lines[i]);
        return;
    }

    // D - DISASSEMBLE
    if (!strcmp(cmd, "D") || !strcmp(cmd, "DASM"))
    {
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running");
        }
        else
        {
            u16 addr = sms.R.PC.W;
            int len  = 10;
            t_debugger_value value;
            if (Debugger_Eval_GetValue(&line, &value))
            {
                addr = value.data;
                skip_spaces(&line);
                if (Debugger_Eval_GetValue(&line, &value))
                {
                    len = value.data;
                }
            }
            {
                int i;
                for (i = 0; i < len; i++)
                {
                    char buf[256];

                    // Display symbols/labels (if any)
                    t_list *symbols;
                    for (symbols = Debugger.symbols_cpu_space[addr]; symbols != NULL; symbols = symbols->next)
                    {
                        t_debugger_symbol *symbol = symbols->elem;
                        Debugger_Printf("%s:", symbol->name);
                    }

                    // Display instruction
                    addr += Debugger_Disassemble_Format(buf, addr);
                    Debugger_Printf(" %s", buf);
                }
            }
        }
        return;
    }

    // SYMBOLS - SYMBOLS
    if (!strcmp(cmd, "SYM") || !strcmp(cmd, "SYMBOL") || !strcmp(cmd, "SYMBOLS"))
    {
        if (!StrNull(line))
            Debugger_Symbols_List(line);
        else
            Debugger_Symbols_List(NULL);
        return;
    }

    // M - MEMORY DUMP
    if (!strcmp(cmd, "M") || !strcmp(cmd, "MEM"))
    {
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf("Command unavailable while machine is not running");
        }
        else
        {
            u16 addr = sms.R.PC.W;
            int len  = 16*8;
            t_debugger_value value;
            if (Debugger_Eval_GetValue(&line, &value))
            {
                addr = value.data;
                skip_spaces(&line);
                if (Debugger_Eval_GetValue(&line, &value))
                {
                    len = value.data;
                }
            }
            {
                int i;
                while (len > 0)
                {
                    char buf[256];
                    u8   data[8];
                    char *p;
                    int  line_len = (len >= 8) ? 8 : len;
                    sprintf(buf, "%04X-%04X | ", addr, (addr + line_len - 1) & 0xFFFF);
                    p = buf + strlen(buf);
                    for (i = 0; i < line_len; i++)
                    {
                        data[i] = RdZ80_NoHook((addr + i) & 0xFFFF);
                        sprintf(p, "%02X ", data[i]);
                        p += 3;
                    }
                    if (i < 8)
                    {
                        int n;
                        sprintf(p, "%-*s%n", (8 - line_len) * 3, "", &n);
                        p += n;
                    }
                    sprintf(p, "| ");
                    p += 2;
                    for (i = 0; i < line_len; i++)
                        *p++ = (isprint(data[i]) ? data[i] : '.');
                    *p = EOSTR;
                    Debugger_Printf (buf);
                    addr += 8;
                    len -= line_len;
                }
            }
        }
        return;
    }

    // Unknown command
    Debugger_Printf ("Syntax error");
}

//-----------------------------------------------------------------------------
// FUNCTIONS - Expression Evaluator
//-----------------------------------------------------------------------------

void     Debugger_Value_SetComputed(t_debugger_value *value, u32 data, int data_size)
{
    value->data         = data;
    value->data_size    = data_size;
    value->source       = DEBUGGER_VALUE_SOURCE_COMPUTED;
    value->source_data  = NULL;
}

void     Debugger_Value_SetCpuRegister(t_debugger_value *value, u32 data, int data_size)
{
    value->data         = data;
    value->data_size    = data_size;
    value->source       = DEBUGGER_VALUE_SOURCE_CPU_REG;
    value->source_data  = NULL;
}

void     Debugger_Value_SetDirect(t_debugger_value *value, u32 data, int data_size)
{
    value->data         = data;
    value->data_size    = data_size;
    value->source       = DEBUGGER_VALUE_SOURCE_DIRECT;
    value->source_data  = NULL;
}

void     Debugger_Value_SetSymbol(t_debugger_value *value, t_debugger_symbol *symbol)
{
    value->data         = symbol->addr;
    value->data_size    = 16;
    value->source       = DEBUGGER_VALUE_SOURCE_SYMBOL;
    value->source_data  = symbol;
}

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
bool    Debugger_Eval_GetVariable(int variable_replacement_flags, const char *var, t_debugger_value *result)
{
    // CPU registers
    if (variable_replacement_flags & DEBUGGER_VARIABLE_REPLACEMENT_CPU_REGS)
    {
        Z80 *   cpu = &sms.R;
        if (!stricmp(var, "PC"))
        {   Debugger_Value_SetCpuRegister(result, cpu->PC.W, 16);    return (TRUE);  }
        if (!stricmp(var, "SP"))
        {   Debugger_Value_SetCpuRegister(result, cpu->SP.W, 16);    return (TRUE);  }
        if (!stricmp(var, "AF"))
        {   Debugger_Value_SetCpuRegister(result, cpu->AF.W, 16);    return (TRUE);  }
        if (!stricmp(var, "A"))
        {   Debugger_Value_SetCpuRegister(result, cpu->AF.B.h, 8);   return (TRUE);  }
        if (!stricmp(var, "BC"))
        {   Debugger_Value_SetCpuRegister(result, cpu->BC.W, 16);    return (TRUE);  }
        if (!stricmp(var, "B"))
        {   Debugger_Value_SetCpuRegister(result, cpu->BC.B.h, 8);   return (TRUE);  }
        if (!stricmp(var, "C"))
        {   Debugger_Value_SetCpuRegister(result, cpu->BC.B.l, 8);   return (TRUE);  }
        if (!stricmp(var, "DE"))
        {   Debugger_Value_SetCpuRegister(result, cpu->DE.W, 16);    return (TRUE);  }
        if (!stricmp(var, "D"))
        {   Debugger_Value_SetCpuRegister(result, cpu->DE.B.h, 8);   return (TRUE);  }
        if (!stricmp(var, "E"))
        {   Debugger_Value_SetCpuRegister(result, cpu->DE.B.l, 8);   return (TRUE);  }
        if (!stricmp(var, "HL"))
        {   Debugger_Value_SetCpuRegister(result, cpu->HL.W, 16);    return (TRUE);  }
        if (!stricmp(var, "H"))
        {   Debugger_Value_SetCpuRegister(result, cpu->HL.B.h, 8);   return (TRUE);  }
        if (!stricmp(var, "L"))
        {   Debugger_Value_SetCpuRegister(result, cpu->HL.B.l, 8);   return (TRUE);  }
        if (!stricmp(var, "IX"))
        {   Debugger_Value_SetCpuRegister(result, cpu->IX.W, 16);    return (TRUE);  }
        if (!stricmp(var, "IY"))
        {   Debugger_Value_SetCpuRegister(result, cpu->IY.W, 16);    return (TRUE);  }
    }

    // Symbols
    if (variable_replacement_flags & DEBUGGER_VARIABLE_REPLACEMENT_SYMBOLS)
    {
        // Go thru all symbols
        t_list *symbols;
        for (symbols = Debugger.symbols; symbols!= NULL; symbols = symbols->next)
        {
            t_debugger_symbol *symbol = (t_debugger_symbol *)symbols->elem;
            if (!stricmp(var, symbol->name))
            {
                Debugger_Value_SetSymbol(result, symbol);
                return (TRUE);
            }
        }
    }

    return (FALSE);
}

static int  Debugger_Eval_ParseIntegerHex(const char *s, const char **end)
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
    *end = s;
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

//-----------------------------------------------------------------------------
// Debugger_Eval_GetValue(char **src_result, t_debugger_value *result)
//-----------------------------------------------------------------------------
// Parse a single value out of given string.
// Advance string pointer.
// Return TRUE on success and fill result, else return FALSE.
//-----------------------------------------------------------------------------
bool    Debugger_Eval_GetValue(char **src_result, t_debugger_value *result)
{
    t_debugger_eval_value_format value_format;
    char    token_buf[256];
    char *  token = token_buf;
    char *  src = *src_result;

    // Debugger_Printf("Debugger_Eval_GetValue(\"%s\")", src);
    skip_spaces(&src);

    // Parenthesis open a sub expression
    if (*src == '(')
    {
        src++;
        if (!Debugger_Eval_GetExpression(&src, result))
            return (FALSE);
        if (*src != ')')
        {
            // Unterminated parenthesis
            Debugger_Printf("Syntax Error - Missing closing parenthesis!");
            *src_result = src;
            return (FALSE);
        }
        src++;
        *src_result = src;
        return (TRUE);
    }

    // Get token
    if (!parse_getword(token_buf, sizeof(token_buf), &src, " \t\n+-*/&|^(),.", 0, PARSE_FLAGS_DONT_EAT_SEPARATORS))
        return (FALSE);
    if (token[0] == '\0')
        return (FALSE);

    // Debugger_Printf(" - token = %s", token);

    value_format = DEBUGGER_EVAL_VALUE_FORMAT_UNKNOWN;
    if (*token == '$')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX;
        token++;
    }
    else
    if (*token == '%')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_BIN;
        token++;
    }
    else
    if (*token == '#')
    {
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC;
        token++;
    }
    else
    if (*token == '0')
    {
        switch (token[1])
        {
        case 'x': case 'X': 
            value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX;
            token += 2;
            break;
        //case 'd': case 'D':
        //    value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC;
        //    token += 2;
        //    break;
        }
    }

    if (value_format == DEBUGGER_EVAL_VALUE_FORMAT_UNKNOWN)
    {
        // Attempt to see if it's a variable
        if (Debugger_Eval_GetVariable(DEBUGGER_VARIABLE_REPLACEMENT_ALL, token, result))
        {
            *src_result = src;
            return (TRUE);
        }

        // Else assume default hexadecimal
        value_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX;
    }

    {
        char *  parse_end;
        int     data;
        switch (value_format)
        {
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX:
            data = Debugger_Eval_ParseIntegerHex(token, &parse_end);
            break;
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_BIN:
            data = Debugger_Eval_ParseInteger(token, "01", &parse_end);
            break;
        case DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC:
            data = Debugger_Eval_ParseInteger(token, "0123456789", &parse_end);
            break;
        default:
            assert(0);
        }
        if (*parse_end != '\0')
        {
            // Syntax error
            return (FALSE);
        }

        Debugger_Value_SetDirect(result, data, 16);
    }

    *src_result = src;
    return (TRUE);
}

static bool Debugger_Eval_GetExpression_Block(char **expr, t_debugger_value *result)
{
    char *  p;
    char    op;
    t_debugger_value value1;
    t_debugger_value value2;

    p = (char *)*expr; 
    // Debugger_Printf("Debugger_Eval_GetExpression_Block(\"%s\")", p);

    skip_spaces(&p);
    if (p[0] == '\0')
    {
        // Empty expression
        return (FALSE);
    }

    // Get first value
    if (!Debugger_Eval_GetValue(&p, &value1))
    {
        Debugger_Printf("Syntax error at \"%s\"!", p);
        Debugger_Printf("                 ^ invalid value or label");
        return (FALSE);
    }
    for (;;)
    {
        skip_spaces(&p);

        // Get operator
        op = *p;

        if (op == ',' || op == '.')
            break;

        // Chain of addition/substraction are handled by Debugger_Eval_GetExpression()
        if (op == '+' || op == '-') 
            break;

        // Stop parsing here on end-of-string or parenthesis closure
        if (op == '\0' || op == ')')
            break;

        // Verify that we have a valid operator
        if (!strchr("*/&|^", op))
        {
            Debugger_Printf("Syntax error at \"%s\"!", p);
            Debugger_Printf("                 ^ unexpected operator");
            return (FALSE);
        }
        p++;

        // Get a second value (since all our operator are binary operator now)
        if (!Debugger_Eval_GetValue(&p, &value2))
        {
            Debugger_Printf("Syntax error at \"%s\"!", p);
            Debugger_Printf("                 ^ invalid value or label");
            return (FALSE);
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
    return (TRUE);
}

//-----------------------------------------------------------------------------
// Debugger_Eval_GetExpression(char **expr, t_debugger_value *result)
//-----------------------------------------------------------------------------
// Parse and evaluate expression from given string.
// Advance string pointer.
// Return TRUE on success and fill result, else return FALSE.
//-----------------------------------------------------------------------------
// Expression exemples:
//  A
//  (A)
//  A+B
//  A+B*C
//  A+(B*C)
//  ((0xFF^0x10)&%11110000)
//-----------------------------------------------------------------------------
bool    Debugger_Eval_GetExpression(char **expr, t_debugger_value *result)
{
    char *  p;
    char    op;
    t_debugger_value value1;
    t_debugger_value value2;

    p = (char *)*expr; 
    // Debugger_Printf("Debugger_Eval_GetExpression(\"%s\")", p);

    skip_spaces(&p);
    if (p[0] == '\0')
    {
        // Empty expression
        return (FALSE);
    }

    // Get first expression block
    if (!Debugger_Eval_GetExpression_Block(&p, &value1))
        return (FALSE);

    for (;;)
    {
        skip_spaces(&p);

        // Get operator
        op = *p;

        if (op == ',' || op == '.')
            break;

        // Stop parsing here on end-of-string or parenthesis closure
        if (op == '\0' || op == ')')
            break;

        // Verify that we have a valid operator
        if (!strchr("+-", op))
        {
            Debugger_Printf("Syntax error at \"%s\"!", p);
            Debugger_Printf("                 ^ unexpected operator");
            return (FALSE);
        }
        p++;

        // Get a second expression block (since all our operator are binary operator now)
        if (!Debugger_Eval_GetExpression_Block(&p, &value2))
        {
            Debugger_Printf("Syntax error at \"%s\"!", p);
            Debugger_Printf("                 ^ invalid value or label");
            return (FALSE);
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
    return (TRUE);
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
    
    // Get current word
    int pos = widget_inputbox_get_cursor_pos(w);
    char *s = widget_inputbox_get_value(w) + pos;
    current_word_len = 0;
    while (pos-- > 0 && !isspace(s[-1]))
    {
        s--;
        current_word_len++;
    }
    // if (current_word_len == 0)
    //    return (FALSE);
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
        while (command_info->command_short != NULL)
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
        Debugger_Printf("No match for \"%s\"", current_word);
        result = NULL;
    }
    else if (matching_words_count == 1)
    {
        // Single match, no ambiguity :)
        const char *complete_word = matching_words->elem;
        result = strdup(complete_word);
    }
    else
    {
        // Multiple matches
        int common_prefix_size;
        t_list *matches;
        
        // Sort matches by name
        list_sort(&matching_words, stricmp);
        
        // Print them
        if (current_word_len > 0)
            Debugger_Printf("%d matches for \"%s\":", matching_words_count, current_word);
        else
            Debugger_Printf("%d matches:", matching_words_count);
        for (matches = matching_words; matches != NULL; matches = matches->next)
        {
            const char *complete_word = matches->elem;
            Debugger_Printf(" - %s", complete_word);
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
        widget_inputbox_insert_string(w, result);
        free(result);

        // If we had only one match, add a space
        if (matching_words_count == 1)
            widget_inputbox_insert_char(w, ' ');

        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}

//-----------------------------------------------------------------------------
// FUNCTIONS - HISTORY
//-----------------------------------------------------------------------------

// FIXME: broken
void        Debugger_History_AddLine(const char *line)
{
    // Shift all history entries by one, except entry 0 which is fixed
    // 3 bye        3 hello
    // 2 hello  --> 2 sega
    // 1 sega       1 <line>
    // 0            0 

    // Note: this would be faster done with a linked list, but we'd require a
    // double linked list for other operators. We have no double-linked list yet.
    int n = Debugger.history_count - 1;
    if (n >= 1)
    {
        // Free last entry
        if (n == Debugger.history_max - 1)
            free(Debugger.history[n]);

        // Shift
        while (n >= 1)
        {
            Debugger.history[n + 1] = Debugger.history[n];
            n--;
        }
        Debugger.history[n] = NULL;
    }

    // Duplicate and add new entry
    assert(Debugger.history[1] == NULL);
    Debugger.history[1] = strdup(line);

    // Increase counter
    if (Debugger.history_count < Debugger.history_max)
        Debugger.history_count++;
}

//-----------------------------------------------------------------------------
// Debugger_History_Callback(t_widget *w)
// Called by the input widget. Handle history.
//-----------------------------------------------------------------------------
bool        Debugger_History_Callback(t_widget *w, int level)
{
    if (level != -1 && level != -1)
        return (FALSE);

    // ...

    return (TRUE);
}

void        Debugger_History_List(void)
{
    int n = Debugger.history_count;

    Debugger_Printf("History:");
    if (n <= 1)  // It's always 1 as current command was already pushed into history
    {
        Debugger_Printf(" <None>");
        return;
    }

    while (n > 1)
    {
        n--;
        Debugger_Printf(" %*s[%d] %s", 
            (Debugger.history_count >= 10 && n < 10) ? 1 : 0, "", 
            n, Debugger.history[n]);
    }
}

//-----------------------------------------------------------------------------

#endif // ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
