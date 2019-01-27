//-----------------------------------------------------------------------------
// MEKA - tvoekaki.c
// TV Oekaki / Graphic Table Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "mappers.h"
#include "tvoekaki.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_tvoekaki  TVOekaki;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TVOekaki_Init (void)
{
    TVOekaki.X = TVOekaki.Y = 0;
    TVOekaki.Infos = TVOEKAKI_BIT_AXIS | TVOEKAKI_BIT_ON_BOARD;
}

// Terebi Oekaki update function
// This is supposed to work with an analog mouse
// Fullscreen mode should be enabled to work properly
// Need to rename variable to something mouse independant :-)
void    TVOekaki_Update(int device_x, int device_y, int device_b_field)
{
    int    nmouse_b = device_b_field;
    static int lmouse_b = 0;

    TVOekaki.X = device_x;
    TVOekaki.Y = device_y;

    // Those values were experimentally found
    // Apparently the real board doesn't allow going to the far sides

    TVOekaki.X -= 5;
    if (TVOekaki.X < 0) TVOekaki.X = 0;
    if (TVOekaki.X > (g_driver->x_res - 1) - 4) TVOekaki.X = (g_driver->x_res - 1) - 4;

    TVOekaki.Y -= 4;
    if (TVOekaki.Y < 0) TVOekaki.Y = 0;
    if (TVOekaki.Y > (g_driver->y_res - 1)) TVOekaki.Y = (g_driver->y_res - 1);
    TVOekaki.Y += 8 * 4;

    if (nmouse_b & 1)
        TVOekaki.Infos |= (TVOEKAKI_BIT_PRESSED);
    else
        TVOekaki.Infos &= (~TVOEKAKI_BIT_PRESSED);

    if ((nmouse_b & 2) != 0 && (lmouse_b & 2) == 0)
    {
        TVOekaki.Infos ^= (TVOEKAKI_BIT_ON_BOARD);
    }

    lmouse_b = nmouse_b;
}

// [MAPPER: TV OEKAKI] READ BYTE ----------------------------------------------
// FIXME: since the TV Oekaki ROM is 0x8000 bytes, removing the Read handler
// should be possible. The update() and write() functions will then directly
// poke in the memory space.
READ_FUNC (Read_Mapper_TVOekaki)
{
    switch (Addr)
    {
    case 0x8000: // Pen Pressure, Pen Position Ready
        // Bit 0: Pen Pressure (0: pressed, 1: unpressed)
        // Bit 7: Pen Position Ready (0: ready, 1: still reading)
        return (TVOEKAKI_PRESSED ? 0x00 : 0x01);
    case 0xA000:
        if (TVOEKAKI_NOT_ON_BOARD)
            return (0x00);
        if (TVOEKAKI_AXIS_X)
            return (TVOekaki.X);
        return (TVOekaki.Y);
    }
    return (Mem_Pages [Addr >> 13] [Addr]);
}

// [MAPPER: TV OEKAKI] WRITE BYTE --------------------------------------------
WRITE_FUNC (Write_Mapper_TVOekaki)
{
 switch (Addr)
   {
   case 0x6000: // Pen Axis Choice
        // Bit 0: Pen Axis Choice (0: X, 1: Y)
        if (Value & 1)
           TVOekaki.Infos &= (~TVOEKAKI_BIT_AXIS);
        else
           TVOekaki.Infos |= (TVOEKAKI_BIT_AXIS);
        break;
   case 0xA000:
        break;
   }
 switch (Addr >> 13)
   {
   // RAM [0xC000] = [0xE000] -----------------------------------------------
   case 6: Mem_Pages [6] [Addr & 0xEFFF] = Mem_Pages [6] [Addr | 0x1000] = Value; return;
   case 7: Mem_Pages [7] [Addr & 0xEFFF] = Mem_Pages [7] [Addr | 0x1000] = Value; return;
   }

 Write_Error (Addr, Value);
}

//-----------------------------------------------------------------------------

