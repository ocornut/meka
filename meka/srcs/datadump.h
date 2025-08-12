//-----------------------------------------------------------------------------
// MEKA - datadump.h
// Data Dumping - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define DATADUMP_MODE_ASCII             (0) /* Default */
#define DATADUMP_MODE_RAW               (1)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_data_dump
{
    int     Mode;
};

extern t_data_dump DataDump;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    DataDump_Init();
void    DataDump_DrawMenu();

void    DataDump_SetOutputMode(int output_mode);

void    DataDump_RAM();
void    DataDump_VRAM();
void    DataDump_Tiles();
void    DataDump_Palette();
void    DataDump_Sprites();
void    DataDump_BgFgMap();
void    DataDump_CPURegs();
void    DataDump_VRegs();
void    DataDump_OnBoardMemory();

//-----------------------------------------------------------------------------

