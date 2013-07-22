//-----------------------------------------------------------------------------
// MEKA - debugger.h
// Z80 Debugger - Headers
//-----------------------------------------------------------------------------

#ifdef MEKA_Z80_DEBUGGER

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#define DEBUGGER_VARIABLE_REPLACEMENT_CPU_REGS  (0x0001)
#define DEBUGGER_VARIABLE_REPLACEMENT_SYMBOLS   (0x0002)
#define DEBUGGER_VARIABLE_REPLACEMENT_ALL       (DEBUGGER_VARIABLE_REPLACEMENT_CPU_REGS | DEBUGGER_VARIABLE_REPLACEMENT_SYMBOLS)

#define DEBUGGER_DATA_COMPARE_LENGTH_MAX        (8)

#define BREAKPOINT_ACCESS_R                     (0x01)
#define BREAKPOINT_ACCESS_W                     (0x02)
#define BREAKPOINT_ACCESS_X                     (0x04)
#define BREAKPOINT_ACCESS_RW                    (BREAKPOINT_ACCESS_R | BREAKPOINT_ACCESS_W)
#define BREAKPOINT_ACCESS_RWX                   (BREAKPOINT_ACCESS_R | BREAKPOINT_ACCESS_W | BREAKPOINT_ACCESS_X)
#define BREAKPOINT_ACCESS_E                     (0x08)

#define BREAKPOINT_LOCATION_CPU                 (0)
#define BREAKPOINT_LOCATION_ROM                 (1)
#define BREAKPOINT_LOCATION_IO                  (2)
#define BREAKPOINT_LOCATION_VRAM                (3)
#define BREAKPOINT_LOCATION_PRAM                (4)
#define BREAKPOINT_LOCATION_LINE                (5)
#define BREAKPOINT_LOCATION_MAX_                (6)

#define BREAKPOINT_TYPE_BREAK                   (0)
#define BREAKPOINT_TYPE_WATCH                   (1)

struct t_debugger_breakpoint
{
    int         enabled;
    int         id;
    int         type;
    int         location;
    int         access_flags;
    int         address_range[2];               // If single address, both values are equal
    int         auto_delete;                    // If -1, decrement on each break, delete when 0
    int         data_compare_length;
    u8          data_compare_bytes[DEBUGGER_DATA_COMPARE_LENGTH_MAX];
    char *      desc;
};

struct t_debugger_bus_info
{
    int                 location;
    const char *        name;
    int                 bus_addr_size;  // in bytes
    int                 addr_min;
    int                 addr_max;
    int                 access;
    int                 data_compare_length_max;
};

struct t_debugger_symbol
{
    u16         addr;
    int         bank;                           // Currently unsupported, set to -1
    char *      name;
    char *      name_uppercase;					// For grep
};

struct t_debugger_history_item
{
	char *		line;
	char *		line_uppercase;					// For grep
	int			cursor_pos;						// -1 = end
};

enum t_debugger_value_flags
{
	DEBUGGER_VALUE_FLAGS_ACCESS_READ	= 0x01,
	DEBUGGER_VALUE_FLAGS_ACCESS_WRITE	= 0x02,
};

enum t_debugger_value_source
{
    DEBUGGER_VALUE_SOURCE_COMPUTED,             // Computed
    DEBUGGER_VALUE_SOURCE_DIRECT,               // Direct input value
    DEBUGGER_VALUE_SOURCE_CPU_REG,              // From CPU
    DEBUGGER_VALUE_SOURCE_SYMBOL_CPU_ADDR,      // From symbol
	DEBUGGER_VALUE_SOURCE_SYMBOL_ROM_ADDR,      //
};

// 'value' also refered as 'variables'
struct t_debugger_value
{
    u32						data;               // Value data
    u16						data_size;          // Value size in bits
	int						flags;				// Value flags (enum t_debugger_value_flags) // FIXME-ENUM
    t_debugger_value_source source;             // Value source type
    void *					source_data;        // Value source (if applicable)
    const char *            name;               // Value name
};

enum t_debugger_eval_value_format
{
    DEBUGGER_EVAL_VALUE_FORMAT_UNKNOWN,
    DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX,
    DEBUGGER_EVAL_VALUE_FORMAT_INT_BIN,
    DEBUGGER_EVAL_VALUE_FORMAT_INT_DEC,
    DEBUGGER_EVAL_VALUE_FORMAT_STRING,
};

struct t_debugger
{
    int         enabled;                        // Enabled and initialized
    bool        active;                         // Currently showing on GUI
    bool        trap_set;
    u16         trap_address;
    int         stepping;                       // Set when we are doing a single step
    int         stepping_trace_after;
	bool		continuing_to_next_ret;
    t_list *    breakpoints;
    t_list *    breakpoints_cpu_space[0x10000]; // 0000-FFFF : each Z80 address has its list of appliable breakpoints
    t_list *    breakpoints_io_space[0x100];
    t_list *    breakpoints_vram_space[0x4000];
    t_list *    breakpoints_pram_space[0x40];
    t_list *    breakpoints_line_space[313];
    t_list *    symbols;
    int         symbols_count;
    t_list *    symbols_cpu_space[0x10000];
	int			history_max;
    int         history_count;
	t_debugger_history_item *	history;
	int			history_current_index;			// 0: new/current edit line, 1+: history lines items
    t_list *	variables_cpu_registers;
    FILE *		log_file;
    const char *log_filename;
    int			watch_counter;                  // For current frame
    long long   cycle_counter;					// Cycle counting accumulator. Only increment in RunZ80_Debugging(), not RunZ80(). 
};

extern t_debugger   Debugger;

// This is like with breakpoints_cpu_space but with direct access to merged CPU read breakpoints. 
// The Z80 emulator use that to trap CPU read of first opcode byte *BEFORE* execution started.
// Otherwise, breakpoints works by stopping CPU after the event happened.
extern int          Debugger_CPU_Exec_Traps[0x10000];

// PC log queue (for trackback feature)
extern u16          Debugger_Z80_PC_Last;
extern u16          Debugger_Z80_PC_Log_Queue[128];
extern int          Debugger_Z80_PC_Log_Queue_Back;
extern int			Debugger_Z80_PC_Log_Queue_Front;

#define DEBUGGER_Z80_PC_LOG_QUEUE_MASK		127

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Init
void                        Debugger_Init_Values(void);
void                        Debugger_Init(void);
void                        Debugger_Close(void);

// Main
void                        Debugger_MachineReset(void);
void                        Debugger_MediaReload(void);
void                        Debugger_Enable(void);
void                        Debugger_Update(void);
void                        Debugger_Switch(void);
void                        Debugger_Printf(const char *format, ...);

// Symbols
bool						Debugger_Symbols_Load();
t_debugger_symbol *         Debugger_Symbols_GetFirstByAddr(u32 addr);
t_debugger_symbol *         Debugger_Symbols_GetLastByAddr(u32 addr);
t_debugger_symbol *         Debugger_Symbols_GetClosestPreviousByAddr(u32 addr, int range);

// Hooks
int                         Debugger_Hook(Z80 *R);
void                        Debugger_RasterLine_Hook(register int line);
void                        Debugger_RdVRAM_Hook(register int addr, register u8 value);
void                        Debugger_WrVRAM_Hook(register int addr, register u8 value);
void                        Debugger_WrPRAM_Hook(register int addr, register u8 value);

// Parser
bool						Debugger_Eval_ParseConstant(const char *value, t_debugger_value *result, t_debugger_eval_value_format default_format = DEBUGGER_EVAL_VALUE_FORMAT_INT_HEX);
int							Debugger_Eval_ParseIntegerHex(const char* s, const char** out_end = NULL);

// Bus Data Access Helpers
int                         Debugger_Bus_Read(int bus, int addr);

//-----------------------------------------------------------------------------
// Functions - Inline
//-----------------------------------------------------------------------------

static INLINE
void  Debugger_Z80_PC_Log_Queue_Add(unsigned short pc)
{
	// This break the constant-time property and make this way slower but gives us
	for (int i = Debugger_Z80_PC_Log_Queue_Back; i != Debugger_Z80_PC_Log_Queue_Front; i = (i - 1) & DEBUGGER_Z80_PC_LOG_QUEUE_MASK)
		if (Debugger_Z80_PC_Log_Queue[i] == pc)
			return;

	// NB- constant time version
    Debugger_Z80_PC_Log_Queue[Debugger_Z80_PC_Log_Queue_Back] = pc;
    Debugger_Z80_PC_Log_Queue_Back = (Debugger_Z80_PC_Log_Queue_Back + 1) & DEBUGGER_Z80_PC_LOG_QUEUE_MASK;
    if (Debugger_Z80_PC_Log_Queue_Back == Debugger_Z80_PC_Log_Queue_Front)
        Debugger_Z80_PC_Log_Queue_Front = (Debugger_Z80_PC_Log_Queue_Front + 1) & DEBUGGER_Z80_PC_LOG_QUEUE_MASK;
}

//-----------------------------------------------------------------------------

#endif
