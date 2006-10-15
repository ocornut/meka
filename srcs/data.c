//-----------------------------------------------------------------------------
// MEKA - data.c
// Data Loading - Code
//-----------------------------------------------------------------------------
// FIXME: The proper way of loading the data file would be to check for the
// type of all data (eg: if known bitmap index is actually a bitmap, etc...).
//-----------------------------------------------------------------------------

#include "shared.h"
#include "bios.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data_Bitmap_IncrementColor  (BITMAP *bmp, int increment, int exclude)
// Increment colors in given bitmap by given increment.
// A "key" color can be excluded.
//-----------------------------------------------------------------------------
void        Data_Bitmap_IncrementColor  (BITMAP *bmp, int increment, int exclude)
{
    int     i, j;
    u8 *    p = bmp->dat;

    for (j = 0; j < bmp->h; j ++)
        for (i = 0; i < bmp->w; i ++)
        {
            const u8 color = *p;
            if (color != exclude)
                *p = color + increment;
            p ++;
        }
}

//-----------------------------------------------------------------------------
// Data_Bitmap_ReplaceColor (BITMAP *bmp, int color_old, int color_new)
// Replace given color in a bitmap.
//-----------------------------------------------------------------------------
static void Data_Bitmap_ReplaceColor (BITMAP *bmp, int color_old, int color_new)
{
    int     i, j;
    u8 *    p = bmp->dat;

    for (j = 0; j < bmp->h; j ++)
        for (i = 0; i < bmp->w; i ++)
        {
            if (*p == color_old)
                *p = color_new;
            p ++;
        }
}

void *      Data_CopyInBiggerArea (void *src, int src_size, int dst_size)
{
    void *  p = malloc (dst_size);
    memcpy (p, src, src_size);
    memset ((u8 *)p + src_size, 0, dst_size - src_size);
    return (p);
}

//-----------------------------------------------------------------------------
// Data_Init (void)
// Load graphics, font & BIOS data from datafile
// Some data processing is done here
//-----------------------------------------------------------------------------
void            Data_Init (void)
{
    int         i;
    DATAFILE *  df;
    char *      passwd = strdup ("westone0000000");

    // Print loading message to console
    ConsolePrint (Msg_Get (MSG_Datafile_Loading));

    // Stupid and lame password protection
    // So at least the password is not 'raw' in the executable
    passwd [4] = '0'; 

    // Open datafile
    packfile_password (passwd);
    df = load_datafile (Env.Paths.DataFile);
    packfile_password (NULL);
    free (passwd);
    if (df == NULL)
        Quit_Msg (Msg_Get (MSG_Failed));

    ConsolePrint ("\n");

    // Assign pointers to data:
    {
        // Cursors
        Graphics.Cursors.Main           = df [DATA_CURSOR_MAIN].dat;
        Graphics.Cursors.Wait           = df [DATA_CURSOR_WAIT].dat;
        Graphics.Cursors.LightPhaser    = df [DATA_CURSOR_LIGHTPHASER].dat;
        Graphics.Cursors.SportsPad      = df [DATA_CURSOR_SPORTSPAD].dat;
        Graphics.Cursors.TvOekaki       = df [DATA_CURSOR_TVOEKAKI].dat;

        // Miscellaenous
        Graphics.Misc.Dragon            = df [DATA_GFX_DRAGON].dat;
        Graphics.Misc.Heart1            = df [DATA_GFX_HEART1].dat;
        Graphics.Misc.Heart2            = df [DATA_GFX_HEART2].dat;

        // Inputs
        Graphics.Inputs.InputsBase      = df [DATA_GFX_INPUTS].dat;
        Graphics.Inputs.Glasses         = df [DATA_GFX_GLASSES].dat;
        Graphics.Inputs.Joypad          = df [DATA_GFX_JOYPAD].dat;
        Graphics.Inputs.LightPhaser     = df [DATA_GFX_LIGHTPHASER].dat;
        Graphics.Inputs.PaddleControl   = df [DATA_GFX_PADDLECONTROL].dat;
        Graphics.Inputs.SportsPad       = df [DATA_GFX_SPORTSPAD].dat;
        Graphics.Inputs.SuperHeroPad    = df [DATA_GFX_SUPERHEROPAD].dat;
        Graphics.Inputs.TvOekaki        = df [DATA_GFX_TVOEKAKI].dat;
        Graphics.Inputs.SK1100_Keyboard = df [DATA_GFX_KEYBOARD].dat;

        // Machines
        Graphics.Machines.MasterSystem          = df [DATA_MACHINE_SMS].dat;
        Graphics.Machines.MasterSystem_Cart     = df [DATA_MACHINE_SMS_CART].dat;
        Graphics.Machines.MasterSystem_Light    = df [DATA_MACHINE_SMS_LIGHT].dat;
        Graphics.Machines.ColecoVision          = df [DATA_MACHINE_COLECO].dat;

        // Flags
        Graphics.Flags [FLAG_UNKNOWN]   = df [DATA_FLAG_UNKNOWN].dat;
        Graphics.Flags [FLAG_AU]        = df [DATA_FLAG_AU].dat;
        Graphics.Flags [FLAG_BR]        = df [DATA_FLAG_BR].dat;
        Graphics.Flags [FLAG_JP]        = df [DATA_FLAG_JP].dat;
        Graphics.Flags [FLAG_KR]        = df [DATA_FLAG_KR].dat;
        Graphics.Flags [FLAG_FR]        = df [DATA_FLAG_FR].dat;
        Graphics.Flags [FLAG_US]        = df [DATA_FLAG_US].dat;
        Graphics.Flags [FLAG_CH]        = df [DATA_FLAG_CH].dat;
        Graphics.Flags [FLAG_HK]        = df [DATA_FLAG_HK].dat;
        Graphics.Flags [FLAG_EU]        = df [DATA_FLAG_EU].dat;
        Graphics.Flags [FLAG_PT]        = df [DATA_FLAG_PT].dat;
        Graphics.Flags [FLAG_DE]        = df [DATA_FLAG_DE].dat;
        Graphics.Flags [FLAG_IT]        = df [DATA_FLAG_IT].dat;
        Graphics.Flags [FLAG_SP]        = df [DATA_FLAG_SP].dat;
        Graphics.Flags [FLAG_SW]        = df [DATA_FLAG_SW].dat;
        Graphics.Flags [FLAG_NZ]        = df [DATA_FLAG_NZ].dat;
        Graphics.Flags [FLAG_UK]        = df [DATA_FLAG_UK].dat;

        // Icons
        Graphics.Icons.BAD              = df [DATA_ICON_BAD].dat;
        Graphics.Icons.BIOS             = df [DATA_ICON_BIOS].dat;
        Graphics.Icons.Hack             = df [DATA_ICON_HACK].dat;
        Graphics.Icons.HomeBrew         = df [DATA_ICON_HOMEBREW].dat;
        Graphics.Icons.Prototype        = df [DATA_ICON_PROTO].dat;
        Graphics.Icons.Translation_JP   = df [DATA_ICON_TRANS_JP].dat;
        Graphics.Icons.Translation_JP_US= df [DATA_ICON_TRANS_JP_US].dat;
    }

    // BIOS
    // Note: BIOSes are unpacked in a bigger memory area, so they can be mapped
    // without having to handle read outside of their real data.
    // The same trick is done on loading < 48 kb Sega 8-bits ROM images.
    BIOS_ROM            = Data_CopyInBiggerArea (df [DATA_ROM_SMS].dat,     0x2000, 0xC000);
    BIOS_ROM_Jap        = Data_CopyInBiggerArea (df [DATA_ROM_SMS_J].dat,   0x2000, 0xC000);
    BIOS_ROM_Coleco     = Data_CopyInBiggerArea (df [DATA_ROM_COLECO].dat,  0x2000, 0x2000);
    BIOS_ROM_SF7000     = Data_CopyInBiggerArea (df [DATA_ROM_SF7000].dat,  0x2000, 0x4000);

    // Patch SF-7000 BIOS (FIXME: move to sf7000.c)
    // Bios_ROM_SF7000[0x112] = Bios_ROM_SF7000[0x113] = Bios_ROM_SF7000[0x114] = 0x00;
    // Bios_ROM_SF7000[0x183] = Bios_ROM_SF7000[0x184] = Bios_ROM_SF7000[0x185] = 0x00;
    BIOS_ROM_SF7000[0x1D8] = BIOS_ROM_SF7000[0x1D9] = BIOS_ROM_SF7000[0x1DA] = 0x00;

    // Fonts
    Fonts_AddFont (F_LARGE,     df [DATA_FONT_0].dat);  // Font Large Sized
    Fonts_AddFont (F_MIDDLE,    df [DATA_FONT_1].dat);  // Font Middle Sized
    Fonts_AddFont (F_SMALL,     df [DATA_FONT_2].dat);  // Font Small Sized

    // Post process data
    for (i = 0; i < DATA_COUNT; i ++)
    {
        if (df[i].type != DAT_BITMAP)
            continue;
        switch (i)
        {
        case DATA_CURSOR_LIGHTPHASER:
        case DATA_CURSOR_MAIN:
        case DATA_CURSOR_SPORTSPAD:
        case DATA_CURSOR_TVOEKAKI:
        case DATA_CURSOR_WAIT:
        case DATA_GFX_DRAGON:
        case DATA_GFX_GLASSES:
        case DATA_GFX_INPUTS:
        case DATA_GFX_JOYPAD:
        case DATA_GFX_KEYBOARD:
        case DATA_GFX_LIGHTPHASER:
        case DATA_GFX_PADDLECONTROL:
        case DATA_GFX_SPORTSPAD:
        case DATA_GFX_SUPERHEROPAD:
        case DATA_GFX_TVOEKAKI:
        case DATA_ICON_BAD:
        case DATA_ICON_BIOS:
        case DATA_ICON_HACK:
        case DATA_ICON_HOMEBREW:
        case DATA_ICON_PROTO:
        case DATA_ICON_TRANS_JP:
        case DATA_ICON_TRANS_JP_US:
        case DATA_FLAG_AU:
        case DATA_FLAG_BR:
        case DATA_FLAG_CH:
        case DATA_FLAG_DE:
        case DATA_FLAG_EU:
        case DATA_FLAG_FR:
        case DATA_FLAG_HK:
        case DATA_FLAG_IT:
        case DATA_FLAG_JP:
        case DATA_FLAG_KR:
        case DATA_FLAG_NZ:
        case DATA_FLAG_PT:
        case DATA_FLAG_SP:
        case DATA_FLAG_SW:
        case DATA_FLAG_UK:
        case DATA_FLAG_UNKNOWN:
        case DATA_FLAG_US:
            // Switch pixel values to use palette starting from GUI_COL_START-1
            Data_Bitmap_IncrementColor (df[i].dat, GUI_COL_START - 1, 0);
            break;
        case DATA_GFX_HEART1:
        case DATA_GFX_HEART2:
            // Hearts gets their color from the theme (with indeed sucks...)
            Data_Bitmap_IncrementColor (df[i].dat, GUI_COL_BARS - 1, 0);
            break;
        case DATA_MACHINE_COLECO:
        case DATA_MACHINE_SMS:
        case DATA_MACHINE_SMS_CART:
        case DATA_MACHINE_SMS_LIGHT:
            // Machines colors starts at 128
            Data_Bitmap_IncrementColor (df[i].dat, GUI_COL_MACHINE_START, 0);
            break;
        }
    }
}

//-----------------------------------------------------------------------------

