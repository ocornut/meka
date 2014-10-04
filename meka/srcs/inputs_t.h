//-----------------------------------------------------------------------------
// MEKA - inputs_t.h
// Inputs Tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void	Inputs_KeyClearAllState			(void);
void    Inputs_KeyEat                   (int keycode);

void    Inputs_KeyPressQueue_Remove     (t_key_press *keypress);
void    Inputs_KeyPressQueue_Clear      (void);

bool	Inputs_KeyDown					(int keycode);
bool    Inputs_KeyPressed               (int keycode, bool eat);
bool    Inputs_KeyPressed_Repeat        (int keycode, bool eat, int delay, int rate);

//-----------------------------------------------------------------------------
