//-----------------------------------------------------------------------------
// MEKA - desktop.h
// Desktop Manager - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

typedef struct
{
    t_list *    items;
    char        filename[FILENAME_LEN];
} t_desktop;

t_desktop       Desktop;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void    Desktop_Init(void);
void    Desktop_Close(void);
void    Desktop_SetStateToBoxes(void);

void    Desktop_Register_Box(const char *name, t_gui_box *box, int default_active, bool *active_org);

void    Desktop_Load(void);
void    Desktop_Save(void);

//-----------------------------------------------------------------------------

