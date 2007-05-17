//-----------------------------------------------------------------------------
// MEKA - bmemory.h
// Backed Memory Devices Emulation - Headers
//-----------------------------------------------------------------------------

void    BMemory_Verify_Usage      (void);

void    BMemory_Load              (void);
void    BMemory_Save              (void);
void    BMemory_Load_State        (FILE *f);
void    BMemory_Save_State        (FILE *f);
void    BMemory_Get_Infos         (void **data, int *len);

//-----------------------------------------------------------------------------
// SRAM
//-----------------------------------------------------------------------------

void    BMemory_SRAM_Load         (FILE *f);
void    BMemory_SRAM_Save         (FILE *f);
void    BMemory_SRAM_Load_State   (FILE *f);
void    BMemory_SRAM_Save_State   (FILE *f);
void    BMemory_SRAM_Get_Infos    (void **data, int *len);

//-----------------------------------------------------------------------------

