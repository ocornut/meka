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

extern bool						g_keyboard_state[ALLEGRO_KEY_MAX];
extern int						g_keyboard_modifiers;
extern ALLEGRO_MOUSE_STATE		g_mouse_state;

//-----------------------------------------------------------------------------

