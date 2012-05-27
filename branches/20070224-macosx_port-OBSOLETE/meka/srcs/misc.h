//-----------------------------------------------------------------------------
// MEKA - misc.h
// Miscellaneous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEKA_MOUSE_CURSOR_NONE           (0)
#define MEKA_MOUSE_CURSOR_STANDARD       (1)
#define MEKA_MOUSE_CURSOR_LIGHT_PHASER   (2)
#define MEKA_MOUSE_CURSOR_SPORTS_PAD     (3)
#define MEKA_MOUSE_CURSOR_TV_OEKAKI      (4)
#define MEKA_MOUSE_CURSOR_WAIT           (5)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Close_Button_Callback   (void);
void    Switch_In_Callback      (void);
void    Switch_Out_Callback     (void);

void    Change_System_Misc      (void);
void    Change_Mode_Misc        (void); // Do various things when changing mode

void    Set_Mouse_Cursor        (int mouse_cursor);

void    Show_End_Message        (void);

void    Quit (void);
void    Quit_Msg (const char *format, ...)      FORMAT_PRINTF (1);

//-----------------------------------------------------------------------------

