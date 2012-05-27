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

void    DataDump_Init                   (void);
void    DataDump_Init_Menus             (int menu_id);

void    DataDump_Mode_Ascii             (void);
void    DataDump_Mode_Raw               (void);

void    DataDump_RAM                    (void);
void    DataDump_VRAM                   (void);
void    DataDump_Tiles                  (void);
void    DataDump_Palette                (void);
void    DataDump_Sprites                (void);
void    DataDump_BgFgMap                (void);
void    DataDump_CPURegs                (void);
void    DataDump_VRegs                  (void);
void    DataDump_OnBoardMemory          (void);

//-----------------------------------------------------------------------------

