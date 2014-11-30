//-----------------------------------------------------------------------------
// MEKA - inputs_u.h
// Inputs Update - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            Inputs_Emulation_Update(bool running);

void            Inputs_Sources_Init();
void            Inputs_Sources_Close();
t_input_src *   Inputs_Sources_Add(char *name);
void            Inputs_Sources_Update();
void            Inputs_Sources_ClearOutOfFocus();

extern float					g_keyboard_state[ALLEGRO_KEY_MAX];		// -1: not pressed, 0:just pressed, >0: held time
extern int						g_keyboard_modifiers;
extern ALLEGRO_MOUSE_STATE		g_mouse_state;

//-----------------------------------------------------------------------------

