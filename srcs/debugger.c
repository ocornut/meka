//-----------------------------------------------------------------------------
// MEKA - debugger.c
// Z80 Debugger - Code
//-----------------------------------------------------------------------------
//
// 1999/12/07
//  - Implementation based on Marat's Z80 debugger.
// 1999/12/18
//  - Same binary now support running with and without debugger.
//  - Added preliminary GUI applet with code disassembly and Z80 registers.
// 1999/12/28
//  - Restarted from scratch, using the new TextBox applet
// 2004/02/15
//  - Minor tweaking. This code sucks. :(
// 2004/03/08-10
//  - Ported debugger to the GUI, removed text mode.
// 2005/01
//  - Tweaks.
//  - Added disassembly trackback feature (in conjunction with modified Z80 emulator).
// 2005/02
//  - Many fixes/improvements.
//  - Added new breakpoint system.
//  - Detailed per-command help.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "debugger.h"
#include "desktop.h"
#include "g_widget.h"
#include "tools/libparse.h"

#ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define DEBUGGER_VERSION                "Alpha 3"
#define DEBUGGER_APP_TITLE              "Debugger"
#define DEBUGGER_APP_CPUSTATE_LINES     (2)

//-----------------------------------------------------------------------------
// External declaration
//-----------------------------------------------------------------------------

int     Z80_Disassemble(char *dst, word addr);
int     Z80_Assemble(const char *src, byte dst[8]);

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void     Meka_Z80_Debugger_Applet_Init (void);
static void     Meka_Z80_Debugger_Applet_InputBox_Callback (void);
static void     Meka_Z80_Debugger_Redraw_State (void);

// Misc
static void     Debugger_Help(const char *cmd);
static void     Debugger_SetTrap(int trap); 

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
static void     Debugger_Hooks_Install(void);
static void     Debugger_Hooks_Uninstall(void);
static void     Debugger_WrZ80_Hook(register int addr, register u8 value);
static u8       Debugger_RdZ80_Hook(register int addr);
static void     Debugger_OutZ80_Hook(register u16 addr, register u8 value);
static u8       Debugger_InZ80_Hook(register u16 addr);

// Breakpoints
static void                     Debugger_BreakPoints_List(void);
static void                     Debugger_BreakPoints_Clear(void);
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
        " # S           ; Resume execution after the call"
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
        " B address                 ; Add CPU R/W breakpoint\n"
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
        " B 0038         ; break when CPU access 0038h\n"
        " B w io 7f      ; break on IO write to 7Fh\n"
        " B rx e000-ffff ; break on CPU read/exec from E000h+\n"
        " B w vram 3f00- ; break on VRAM write to SAT"
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
        " P expression\n"
        "Examples:\n"
        " P 1200+34     ; print $1234\n"
        " P %00101010   ; print 42\n"
        " P HL+BC*4     ; print HL+BC*4"
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
// Functions
//-----------------------------------------------------------------------------

void        Debugger_Init_Values (void)
{
    Debugger.Enabled = NO;
    Debugger.Active = NO;
    Debugger.trap_set = FALSE;
    Debugger.trap_address = -1;
    Debugger.stepping = -1;
    Debugger.stepping_trace_after = 0;
    Debugger.breakpoints = NULL;
    memset(Debugger.breakpoints_cpu_space,  0, sizeof(Debugger.breakpoints_cpu_space));
    memset(Debugger.breakpoints_io_space,   0, sizeof(Debugger.breakpoints_io_space));
    memset(Debugger.breakpoints_vram_space, 0, sizeof(Debugger.breakpoints_vram_space));
    memset(Debugger.breakpoints_pram_space, 0, sizeof(Debugger.breakpoints_pram_space));
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
    Meka_Z80_Debugger_Applet_Init ();

    // Open log file
    if (Debugger.Active)
        Debugger_Init_LogFile();

    // Print welcome line
    Debugger_Printf (Msg_Get (MSG_Debug_Welcome), DEBUGGER_VERSION);
    Debugger_Printf ("Enter H for help.");
}

void        Debugger_Close (void)
{
    if (Debugger.log_file != NULL)
    {
        fclose(Debugger.log_file);
        Debugger.log_file = NULL;
    }
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
void        Debugger_MachineReset (void)
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
// Debugger_Update ()
// Update MEKA debugger
//-----------------------------------------------------------------------------
void        Debugger_Update (void)
{
    if (Debugger.Active)
        Meka_Z80_Debugger_Redraw_State ();
    // Reset watch counter
    Debugger.watch_counter = 0;
}

int         Debugger_Hook (Z80 *R)
{
    u16 pc = R->PC.W;
    // Debugger_Printf("hook, pc=%04X", pc);

    // If stepping, ask Z80 emulator to continue (a single time)
    if (Debugger.stepping == pc)
    {
        R->Trace = Debugger.stepping_trace_after;
        // If disabling tracing, disable stepping immediately or else we'll never break!
        if (R->Trace == 0)
            Debugger.stepping = -1;
        return (1);
    }

    // Always remove stepping flag, so we can break at another point
    // eg: if we stepped on a CALL instruction, or if an interrupt was raised
    Debugger.stepping = -1;

    // If we arrived from a trap, print a line about it
    if (pc == Debugger.trap_address)
        Debugger_Printf("Break at %04Xh", pc);

    // If we arrived from a breakpoint CPU exec trap...
    if (Debugger_CPU_Exec_Traps[pc])
    {
        int cnt = Debugger_CPU_Exec_Traps[pc];
        t_list *breakpoints;
        for (breakpoints = Debugger.breakpoints; breakpoints != NULL; breakpoints = breakpoints->next)
        {
            t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
            if (breakpoint->enabled)
                if (breakpoint->location == BREAKPOINT_LOCATION_CPU && (breakpoint->access_flags & BREAKPOINT_ACCESS_X))
                    if (pc >= breakpoint->address_range[0] && pc <= breakpoint->address_range[1])
                    {
                        Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_X, pc, -1);
                        cnt--;
                    }
        }
        assert(cnt == 0);
    }

    // Update state
    Meka_Z80_Debugger_Redraw_State();

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
// Functions - Breakpoints Manager
//-----------------------------------------------------------------------------

void                        Debugger_BreakPoints_Clear(void)
{
    t_list *breakpoints;

    for (breakpoints = Debugger.breakpoints; breakpoints != NULL; )
    {
        t_debugger_breakpoint *breakpoint = (t_debugger_breakpoint *)breakpoints->elem;
        breakpoints = breakpoints->next;
        Debugger_BreakPoint_Remove(breakpoint);
    }
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
// Functions - Breakpoints
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
        sprintf(addr_string, "%0*X-%0*X", bus_size * 2, breakpoint->address_range[0], bus_size * 2, breakpoint->address_range[1]);
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
// HOOKS
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_R) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
        {
            // Special case, if a X handler is installed and we're now executing, ignore R breakpoint
            // It is not logical but much better for end user, who is likely to use RWX in most cases
            if (breakpoint->access_flags & BREAKPOINT_ACCESS_X)
            {
                if (addr >= Debugger_Z80_PC_Last && addr <= Debugger_Z80_PC_Last + 6)   // quick check to 6
                    if (addr <= Debugger_Z80_PC_Last + Z80_Disassemble(NULL, Debugger_Z80_PC_Last))
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_R) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_R) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
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
        if ((breakpoint->access_flags & BREAKPOINT_ACCESS_W) && addr >= breakpoint->address_range[0] && addr <= breakpoint->address_range[1])
        {
            // Verbose break/watch point result
            Debugger_BreakPoint_ActivatedVerbose(breakpoint, BREAKPOINT_ACCESS_W, addr, value);
        }
    }
}

//-----------------------------------------------------------------------------
// Debugger_Switch ()
// Enable or disable the debugger
// Called from various places
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
// Print a formatted line to the debugger console
//-----------------------------------------------------------------------------
void        Debugger_Printf (const char *format, ...)
{
    char    buf[1024];
    va_list param_list;
    char *  p;

    va_start (param_list, format);
    vsprintf (buf, format, param_list);
    va_end (param_list);

    // Log to file
    if (Debugger.log_file != NULL)
        fprintf(Debugger.log_file, "%s\n", buf);

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
// Meka_Z80_Debugger_Applet_Init ()
// Initialize the debugger applet
//-----------------------------------------------------------------------------
static void Meka_Z80_Debugger_Applet_Init (void)
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
    DebuggerApp.input_box = widget_inputbox_add(box_id, &frame, 46, F_MIDDLE, Meka_Z80_Debugger_Applet_InputBox_Callback);
}

//-----------------------------------------------------------------------------
// Meka_Z80_Debugger_Disassemble_Format (char *dst, u16 addr)
// Disassemble one instruction at given address and produce a formatted 
// string with address, opcode and instruction description.
// Return instruction length.
//-----------------------------------------------------------------------------
int         Meka_Z80_Debugger_Disassemble_Format (char *dst, u16 addr)
{
    int     len;
    char    instr[128];

    len = Z80_Disassemble(instr, addr);
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
// Meka_Z80_Debugger_Redraw_State ()
// Redraw disassembly and CPU state
//-----------------------------------------------------------------------------
void        Meka_Z80_Debugger_Redraw_State (void)
{
    int     i;

    if (!(machine & MACHINE_POWER_ON))
        return;

    // Disassembly
    {
        t_frame frame = DebuggerApp.frame_disassembly;

        int     pc;
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
                    pc_history[delta] = Z80_Disassemble(NULL, Debugger_Z80_PC_Log_Queue[i]);
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
                        pc_history[pc_temp - inst_after] = inst_len = Z80_Disassemble(NULL, inst_after);
                        i = pc_temp - inst_after;
                        inst_after += inst_len;
                    }

                    // Went after PC, something was wrong. Might happen on data bytes between close code, etc.
                    // FIXME: This actually happens, see TODO.TXT. Requires fix, workaround, or silent fail.
                    if (inst_after > pc_temp)
                    {
                        Msg (0, "[Warning] inst_after = %04x > pc_temp = %04x", inst_after, pc_temp);
                        break;
                    }

                    // This is the instruction right before pc_temp!
                    // We successfully trackbacked one instruction.
                    if (inst_after == pc_temp)
                    {
                        pc_temp -= inst_len;
                        trackback_lines--;
                    }

                    // No more trackback to do
                    if (trackback_lines <= 0)
                        break;
                }
            }
            pc = pc_temp;
        }

        // Disassemble instructions
        for (i = 0; i < Configuration.debugger_disassembly_lines; i++)
        {
            int y = frame.pos.y + (i * DebuggerApp.font_height);
            char buf[256];
            int text_color = (pc == sms.R.PC.W) ? GUI_COL_TEXT_ACTIVE : GUI_COL_TEXT_IN_BOX;

            // The trick here is to re-add all disassembled instruction into the PC log queue
            //Debugger_Z80_PC_Log_Queue_Add(pc);

            // Disassemble
            pc += Meka_Z80_Debugger_Disassemble_Format (buf, pc);
            Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, buf, frame.pos.x, y, text_color);
        }
    }

    // CPU State
    {
        char    flags[9];
        u8      reg;
        char    line[256];
        int     y;
        Z80 *   cpu = &sms.R;

        // Clear CPU state buffer
        t_frame frame = DebuggerApp.frame_cpustate;
        rectfill (DebuggerApp.box_gfx, frame.pos.x, frame.pos.y, frame.pos.x + frame.size.x, frame.pos.y + frame.size.y, GUI_COL_FILL);
        y = frame.pos.y;

        // Compute flags
        reg = cpu->AF.B.l;
        for (i = 0; i < 8; i++)
            flags[i] = (reg & (1 << (7 - 1))) ? "SZ.H.PNC"[i] : '.';
        flags[i] = EOSTR;

        sprintf(line, "AF:%04X  BC:%04X  HL:%04X  DE:%04X  IX:%04X  IY:%04X",
            cpu->AF.W, cpu->BC.W, cpu->HL.W, cpu->DE.W, cpu->IX.W, cpu->IY.W);
        Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, line, frame.pos.x, y, GUI_COL_TEXT_IN_BOX);
        y += DebuggerApp.font_height;

        sprintf(line, "PC:%04X  SP:%04X  Flags:[%s]  %s%s", 
            cpu->PC.W, cpu->SP.W, flags,
            (cpu->IFF & IFF_1) ? "EI" : "DI", (cpu->IFF & IFF_HALT) ? " HALT" : ""
            );
        Font_Print (DebuggerApp.font_id, DebuggerApp.box_gfx, line, frame.pos.x, y, GUI_COL_TEXT_IN_BOX);
        y += DebuggerApp.font_height;

    }

    DebuggerApp.box->must_redraw = YES;
}

static void     Debugger_Help(const char *cmd)
{
    if (cmd == NULL)
    {
        // Generic help
        Debugger_Printf ("Debugger Help:" );
        Debugger_Printf ("-- Flow:");
        Debugger_Printf (" <CR>                  : Step into"                 );
        Debugger_Printf (" S                     : Step over"                 ) ;
        Debugger_Printf (" C                     : Continue"                  );
        Debugger_Printf (" C addr                : Continue up to <addr>"     );
        Debugger_Printf (" J addr                : Jump to <addr>"            );
        Debugger_Printf ("-- Breakpoints:");
        Debugger_Printf (" B [access] [bus] addr : Add breakpoint"            );
        Debugger_Printf (" B LIST                : List breakpoints"          );
        Debugger_Printf (" B REMOVE n            : Remove breakpoint"         );
        Debugger_Printf (" B ENABLE/DISABLE n    : Enable/disable breakpoint" );
        Debugger_Printf (" B CLEAR               : Clear breakpoints"         );
        Debugger_Printf (" W [access] [bus] addr : Add watchpoint"            );
        Debugger_Printf ("-- Inspect:");
        //Debugger_Printf (" P expr                : Print evaluated expression");
        Debugger_Printf (" M [addr] [len]        : Memory dump at <addr>"     );
        Debugger_Printf (" D [addr] [cnt]        : Disassembly at <addr>"     );
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
                /*
                if (command_info->command_long)
                    Debugger_Printf("Help for \"%s\" (%s/%s)", command_info->abstract, command_info->command_short, command_info->command_long);
                else
                    Debugger_Printf("Help for \"%s\" (%s)", command_info->abstract, command_info->command_short);
                */
                Debugger_Printf("%s", command_info->description);
                return;
            }
            command_info++;
        }
        Debugger_Printf("Unknown command \"%s\" !", cmd);
    }
}

//-----------------------------------------------------------------------------
// Meka_Z80_Debugger_Applet_InputBox_Callback ()
// Input box widget callback. Called when the user validate a line with ENTER.
//-----------------------------------------------------------------------------
void        Meka_Z80_Debugger_Applet_InputBox_Callback (void)
{
    char    line_buf[512];
    char *  line;
    char    cmd[64];
    char    arg[256];
    int     cmd_id;

    strcpy (line_buf, widget_inputbox_get_value(DebuggerApp.input_box));
    Trim (line_buf);
    line = line_buf;

    // Clear input box
    widget_inputbox_set_value(DebuggerApp.input_box, "");

    // An empty line means step into or activate debugging
    if (line_buf[0] == EOSTR)
    {
        if (machine & MACHINE_POWER_ON)
        {
            if (machine & MACHINE_DEBUGGING)
            {
                // Step into
                Debugger.stepping = sms.R.PC.W;
                Debugger.stepping_trace_after = sms.R.Trace = 1;
                Machine_Debug_Stop ();
            }
            else
            {
                // Activate debugging
                Debugger.stepping = -1;
                Debugger_Printf("Breaking at %04Xh", sms.R.PC.W);
                Meka_Z80_Debugger_Redraw_State();
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
    parse_getword(cmd, sizeof(cmd), &line, " ", 0);
    strupr(cmd);

    // H - HELP
    if (!strcmp(cmd, "H") || !strcmp(cmd, "?") || !strcmp(cmd, "HELP"))
    {
        if (parse_getword(arg, sizeof(arg), &line, " ", 0))
            Debugger_Help(arg);
        else
            Debugger_Help(NULL);
        return;
    }
    
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

        if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
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
            Debugger_BreakPoints_Clear();
            return;
        }

        // B ENABLE
        if (!stricmp(arg, "enable"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                int id = atoi(arg);
                t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
                if (breakpoint)
                {
                    if (breakpoint->enabled)
                    {
                        Debugger_Printf("%s [%d] already enabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    else
                    {
                        Debugger_BreakPoint_Enable(breakpoint);
                        Debugger_Printf("%s [%d] enabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                }
                else
                {
                    Debugger_Printf("Breakpoint [%s] not found!", arg);
                }
            }
            return;
        }

        // B DISABLE
        if (!stricmp(arg, "disable"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                int id = atoi(arg);
                t_debugger_breakpoint *breakpoint = Debugger_BreakPoints_SearchById(id);
                if (breakpoint)
                {
                    if (!breakpoint->enabled)
                    {
                        Debugger_Printf("%s [%d] already disabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                    else
                    {
                        Debugger_BreakPoint_Disable(breakpoint);
                        Debugger_Printf("%s [%d] disabled.", Debugger_BreakPoint_GetTypeName(breakpoint), id);
                    }
                }
                else
                {
                    Debugger_Printf("Breakpoint [%s] not found!", arg);
                }
            }
            return;
        }

        // B REMOVE
        if (!stricmp(arg, "remove"))
        {
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
            {
                Debugger_Printf("Missing parameter!");
            }
            else
            {
                int id = atoi(arg);
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
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
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
            if (!parse_getword(arg, sizeof(arg), &line, " ", 0))
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
            int address_start = -1;
            int address_end = -1;
            int len;
            t_debugger_bus_info *bus_info = &DebuggerBusInfos[location];

            // Parse different kind of ranges (-A, A-A, -A, A)
            if (sscanf(arg, "-%X", &address_end) == 1)
            {
                address_start = bus_info->addr_min;
            }
            else if (sscanf(arg, "%X-%X", &address_start, &address_end) == 2)
            {
            }
            /*
            else if (sscanf(arg, "%X-", &address_start) == 1)
            {
                
            } */
            else if (sscanf(arg, "%X%n", &address_start, &len) == 1)
            {
                if (arg[len]=='-')
                    address_end = bus_info->addr_max;
                else
                    address_end = address_start;
            }

            if (address_start != -1 && address_end != -1)
            {
                // Check out address range
                if (address_end < address_start)
                {
                    Debugger_Printf("Second address in range must be higher.");
                    return;
                }
                if (address_start < bus_info->addr_min || address_start > bus_info->addr_max)
                {
                    Debugger_Printf("Address %X is out of %s range (%0*X-%0*X).", 
                        address_start, 
                        bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
                    return;
                }
                if (address_end < bus_info->addr_min || address_end > bus_info->addr_max)
                {
                    Debugger_Printf("Address %X is out of %s range (%0*X-%0*X).", 
                        address_end, 
                        bus_info->name, bus_info->bus_addr_size * 2, bus_info->addr_min, bus_info->bus_addr_size * 2, bus_info->addr_max);
                    return;
                }

                // Get description (if any) and add breakpoint
                {
                    int type = (cmd_id == 1) ? BREAKPOINT_TYPE_BREAK : BREAKPOINT_TYPE_WATCH;
                    Trim(line);
                    if (strlen(line) > 0)
                        Debugger_BreakPoint_Add(type, location, access, address_start, address_end, -1, line);  // Desc
                    else
                        Debugger_BreakPoint_Add(type, location, access, address_start, address_end, -1, NULL);
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

        if (parse_getword(arg, sizeof(arg), &line, " ", 0))
            Debugger_Printf("Additionnal argument ignored.");

        return;
    }
    
    // C - CONTINUE
    if (!strcmp(cmd, "C") || !strcmp(cmd, "CONT") || !strcmp(cmd, "CONTINUE"))
    {
        if (machine & MACHINE_DEBUGGING)
        {
            u16 addr;
            if (sscanf (line, "%hX", &addr) == 1)
            {
                // Continue up to
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
            Debugger.stepping = sms.R.PC.W;
            Debugger.stepping_trace_after = 0;
        }
        else
        {
            Debugger_Printf ("Command unavailable while machine is not being debugged");
        }
        return;
    }

    // J - JUMP
    if (!strcmp(cmd, "J") || !strcmp(cmd, "JP") || !strcmp(cmd, "JUMP"))
    {
        u16     addr;
        if (!(machine & MACHINE_POWER_ON))
        {
            Debugger_Printf ("Command unavailable while machine is not running");
            return;
        }
        if (!(machine & MACHINE_DEBUGGING))
        {
            // If running, stop and entering into debugging state
            Machine_Debug_Start ();
        }
        if (sscanf (line, "%hX", &addr) == 1)
        {
            //if (addr < 0x0000 || addr > 0xFFFF)
            //    Debugger_Printf ("Out of range error");
            //else
            {
                sms.R.PC.W = addr;
                Debugger_Printf("Jump to %04Xh", sms.R.PC.W);
                Meka_Z80_Debugger_Redraw_State ();
            }
        }
        else
        {
            Debugger_Printf ("Missing parameter!");
        }
        return;
    }

    // S - STEP OVER
    if (!strcmp(cmd, "S"))
    {
        if (machine & MACHINE_DEBUGGING)
        {
            // Get address of following instruction
            // Do not verbose since this is just a step-over
            u16 addr = sms.R.PC.W + Z80_Disassemble(NULL, sms.R.PC.W);
            Debugger_SetTrap (addr);
            sms.R.Trace = 0;
            Machine_Debug_Stop ();
        }
        else
        {
            Debugger_Printf ("Command unavailable while machine is not running");
        }
        return;
    }

    // P - PRINT
    if (!strcmp(cmd, "P") || !strcmp(cmd, "PRINT"))
    {
        int value = 0x1234;
        char binary[2][9];
        Write_Bits_Field ((value & 0xFF), 8, binary[0]);
        Write_Bits_Field ((value >> 8) & 0xFF, 8, binary[1]);
        Debugger_Printf("FIXME: Yet unimplemented");
        Debugger_Printf(" $%04x   %%%s.%s   '%c'   %d", value, binary[0], binary[1], (value & 0xFF), value);
        return;
    }

    // D - DISASSEMBLE
    if (!strcmp(cmd, "D") || !strcmp(cmd, "DASM"))
    {
        if (machine & MACHINE_POWER_ON)
        {
            u16 addr = sms.R.PC.W;
            int len  = 10;
            sscanf (line, "%hX %X", &addr, &len);
            // if (addr < 0x0000 || addr > 0xFFFF)
            //    Debugger_Printf ("Out of range error");
            //else
            {
                int i;
                for (i = 0; i < len; i++)
                {
                    char buf[256];
                    addr += Meka_Z80_Debugger_Disassemble_Format (buf, addr);
                    Debugger_Printf (buf);
                }
            }
        }
        else
        {
            Debugger_Printf ("Command unavailable while machine is not running");
        }
        return;
    }

    // M - MEMORY DUMP
    if (!strcmp(cmd, "M") || !strcmp(cmd, "MEM"))
    {
        if (machine & MACHINE_POWER_ON)
        {
            u16 addr = sms.R.PC.W;
            int len  = 16*8;
            sscanf (line, "%hX %X", &addr, &len);
            //if (addr < 0x0000 || addr > 0xFFFF)
            //    Debugger_Printf ("Out of range error");
            //else
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
        else
        {
            Debugger_Printf ("Command unavailable while machine is not running");
        }
        return;
    }

    // Unknown command
    Debugger_Printf ("Syntax error");
}

#endif // ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------

