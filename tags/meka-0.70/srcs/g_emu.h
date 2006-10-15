//-----------------------------------------------------------------------------
// MEKA - g_emu.h
// GUI things related with emulated machines - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

int     gamebox_create (int x, int y);
void    gamebox_create_on_mouse_pos (void);
void    gamebox_kill_all (void);
void    gamebox_kill_last (void);

int     gamebox_x (void);
int     gamebox_y (void);

void    gamebox_draw (int which, int sx, int sy, BITMAP *game_buffer);
void    gamebox_resize_all (void);
void    gamebox_rename_all (void);

//-----------------------------------------------------------------------------

