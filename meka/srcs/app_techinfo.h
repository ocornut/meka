//-----------------------------------------------------------------------------
// MEKA - techinfo.h
// Technical Information Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TECHINFO_LINES          (14)
#define TECHINFO_COLUMNS        (90)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_app_tech_info
{
    bool        active;
    t_gui_box * box;
    char        lines[TECHINFO_LINES][512 /* TECHINFO_COLUMNS */];
    bool        lines_dirty[TECHINFO_LINES];

};

extern t_app_tech_info TechInfo;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TechInfo_Init(void);
void    TechInfo_Update(void);
void    TechInfo_Switch(void);

//-----------------------------------------------------------------------------

