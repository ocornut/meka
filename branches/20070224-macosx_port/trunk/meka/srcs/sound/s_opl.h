//-----------------------------------------------------------------------------
// MEKA - s_opl.h
// OPL Access - Headers
//-----------------------------------------------------------------------------

#ifndef MEKA_OPL
#error "OPL Access file included/compiled without MEKA_OPL defined!"
#endif

#ifndef __S_OPL_H__
#define __S_OPL_H__

//-----------------------------------------------------------------------------

void    Sound_OPL_Init_Config (void);

int     Sound_OPL_Read_Status (void);
void    Sound_OPL_Write (int R, int V);

int     Sound_OPL_Init (void);
void    Sound_OPL_Close (void);

//-----------------------------------------------------------------------------

#endif /* __S_OPL_H__ */

