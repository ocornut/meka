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
// Data
//-----------------------------------------------------------------------------

static DATAFILE *   g_Datafile = NULL;
static t_list *     g_DatafileBitmapCopy32 = NULL;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void *      Data_CopyInBiggerArea(void *src, int src_size, int dst_size)
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
void            Data_Init(void)
{
    DATAFILE *  df;

    // Print loading message to console
    ConsolePrint (Msg_Get (MSG_Datafile_Loading));

    // Open and load datafile
    assert(g_Datafile == NULL);
    df = g_Datafile = load_datafile (g_Env.Paths.DataFile);
    if (g_Datafile == NULL)
        Quit_Msg (Msg_Get (MSG_Failed));
    ConsolePrint ("\n");

    // Make a copy of ALL bitmaps
    // This is required to handle successives calls to fixup_datafile()
    g_DatafileBitmapCopy32 = NULL;

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
        Graphics.Flags [FLAG_CA]        = df [DATA_FLAG_CA].dat;

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
    // without having to handle memory accesses outside their allocated range.
    // The same trick is done on loading < 48 KB Sega 8-bits ROM images.
    BIOS_ROM            = Data_CopyInBiggerArea (df [DATA_ROM_SMS].dat,     0x2000, 0xC000);
    BIOS_ROM_Jap        = Data_CopyInBiggerArea (df [DATA_ROM_SMS_J].dat,   0x2000, 0xC000);
    BIOS_ROM_Coleco     = Data_CopyInBiggerArea (df [DATA_ROM_COLECO].dat,  0x2000, 0x2000);
    BIOS_ROM_SF7000     = Data_CopyInBiggerArea (df [DATA_ROM_SF7000].dat,  0x2000, 0x4000);

    // Patch SF-7000 BIOS (FIXME: move to sf7000.c)
    // Bios_ROM_SF7000[0x112] = Bios_ROM_SF7000[0x113] = Bios_ROM_SF7000[0x114] = 0x00;
    // Bios_ROM_SF7000[0x183] = Bios_ROM_SF7000[0x184] = Bios_ROM_SF7000[0x185] = 0x00;
    BIOS_ROM_SF7000[0x1D8] = BIOS_ROM_SF7000[0x1D9] = BIOS_ROM_SF7000[0x1DA] = 0x00;

    // Fonts
    Fonts_AddFont(F_LARGE,     df[DATA_FONT_0].dat);  // Font Large Sized
    Fonts_AddFont(F_MIDDLE,    df[DATA_FONT_1].dat);  // Font Middle Sized
    Fonts_AddFont(F_SMALL,     df[DATA_FONT_2].dat);  // Font Small Sized
}

void    Data_Close(void)
{
    assert(g_Datafile != NULL);
    list_free_custom(&g_DatafileBitmapCopy32, destroy_bitmap);
    unload_datafile(g_Datafile);
    g_Datafile = NULL;
}

// Convert truecolor graphics stored in datafile in current native format
void    Data_UpdateVideoMode()
{
    int i;
    t_list *bmp_copies;

    assert(g_Datafile != NULL);
    bmp_copies = g_DatafileBitmapCopy32;

    if (bmp_copies == NULL)
    {
        fixup_datafile(g_Datafile);
        for (i = 0; g_Datafile[i].type != DAT_END; i++)
        {
            if (g_Datafile[i].type == DAT_BITMAP)
            {
                BITMAP * dat_bmp = g_Datafile[i].dat;
                BITMAP * copy_bmp = create_bitmap_ex(32, dat_bmp->w, dat_bmp->h);
                blit(dat_bmp, copy_bmp, 0, 0, 0, 0, dat_bmp->w, dat_bmp->h);
                list_add_to_end(&g_DatafileBitmapCopy32, copy_bmp);
            }
        }
        bmp_copies = g_DatafileBitmapCopy32;
    }

    for (i = 0; g_Datafile[i].type != DAT_END; i++)
    {
        if (g_Datafile[i].type == DAT_BITMAP)
        {
            BITMAP * dat_bmp = g_Datafile[i].dat;
            BITMAP * copy_bmp = bmp_copies->elem;
            blit(copy_bmp, dat_bmp, 0, 0, 0, 0, dat_bmp->w, dat_bmp->h);
            bmp_copies = bmp_copies->next;
        }
    }
    //fixup_datafile(g_Datafile);
}

//-----------------------------------------------------------------------------

