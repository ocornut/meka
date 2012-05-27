//-----------------------------------------------------------------------------
// MEKA - tvtype.h
// TV Types emulation (NTSC/PAL/SECAM) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TVTYPE_NTSC       (0)
#define TVTYPE_PAL_SECAM  (1)
#define TVTYPE_AUTO       (2) // AUTO must not be 0 or 1 because the values are used for menu ticks positionning

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TVType_Init_Values      (void);
void    TVType_Update_Values    (void);

void    TVType_Set              (int tv_type, bool verbose);
void    TVType_Set_NTSC         (void);
void    TVType_Set_PAL_SECAM    (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_tv_type
{
    int     id;
    const char *  name;
    int     screen_lines;      
    int     screen_frequency;  // in Hz
    int     CPU_clock;
};

extern t_tv_type        TV_Type_Table[];
extern t_tv_type *      TV_Type_User;
// Note: TV Type for emulation is pointed by 'g_machine.TV_Type'

//-----------------------------------------------------------------------------

