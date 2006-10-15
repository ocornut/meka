//-----------------------------------------------------------------------------
// MEKA - misc.h
// Miscellaneous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Close_Button_Callback   (void);
void    Switch_In_Callback      (void);
void    Switch_Out_Callback     (void);

void    Change_System_Misc      (void);
void    Change_Mode_Misc        (void); // Do various things when changing mode

void    Set_Mouse_Cursor        (int n);// Change mouse cursor

void    Show_End_Message        (void);

void    Quit (void);
void    Quit_Msg (const char *format, ...)      FORMAT_PRINTF (1);

//-----------------------------------------------------------------------------

