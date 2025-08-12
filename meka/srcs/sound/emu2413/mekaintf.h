//
// Meka - MEKAINTF.H
// Interface to EMU2413
//

//-----------------------------------------------------------------------------

int     FM_Digital_Init();
void    FM_Digital_Close();
void    FM_Digital_Active();
void    FM_Digital_WriteSamples	(s16 *buffer, int length);

//-----------------------------------------------------------------------------

// Interface (see FMUNIT.C/.H)
void    FM_Digital_Reset();
void    FM_Digital_Write(int Register, int Value);
void    FM_Digital_Mute();
void    FM_Digital_Resume();
void    FM_Digital_Regenerate();

//-----------------------------------------------------------------------------


