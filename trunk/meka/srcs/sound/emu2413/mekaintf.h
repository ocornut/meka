//
// Meka - MEKAINTF.H
// Interface to EMU2413
//

//-----------------------------------------------------------------------------

int     FM_Digital_Init        ();
void    FM_Digital_Close        (void);
void    FM_Digital_Active       (void);
void    FM_Digital_WriteSamples	(void *buffer, int length);

//-----------------------------------------------------------------------------

// Interface (see FMUNIT.C/.H)
void    FM_Digital_Reset        (void);
void    FM_Digital_Write        (int Register, int Value);
void    FM_Digital_Mute         (void);
void    FM_Digital_Resume       (void);
void    FM_Digital_Regenerate   (void);

//-----------------------------------------------------------------------------


