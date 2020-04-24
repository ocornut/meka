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

t_data_graphics     Graphics;
enum
{
    DATA_PROCESS_NULLIFY = 0x01,
    DATA_PROCESS_FREE    = 0x02,
    DATA_PROCESS_LOAD    = 0x04,
};
static int DataProcessFlags;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void* Data_CopyInBiggerArea(void *src, int src_size, int dst_size)
{
    void *p = malloc (dst_size);
    memcpy (p, src, src_size);
    memset ((u8 *)p + src_size, 0, dst_size - src_size);
    return (p);
}

static void* Data_LoadBinary(const char* name)
{
    char filename_buf[FILENAME_LEN];
    sprintf(filename_buf, "%s/Data/%s", g_env.Paths.EmulatorDirectory, name);

    ALLEGRO_FILE * f = al_fopen(filename_buf, "rb");
    al_fseek(f, 0, SEEK_END);
    const int size = al_ftell(f);
    al_fseek(f, 0, SEEK_SET);

    void* buf = Memory_Alloc(size);
    al_fread(f, buf, size);
    return buf;
}

static bool Data_LoadBitmap(ALLEGRO_BITMAP** pbitmap, const char* name)
{
    if (DataProcessFlags & DATA_PROCESS_NULLIFY)
        *pbitmap = NULL;

    if (DataProcessFlags & DATA_PROCESS_FREE)
    {
        if (*pbitmap != NULL)
        {
            al_destroy_bitmap(*pbitmap);
            *pbitmap = NULL;
        }
    }

    if (DataProcessFlags & DATA_PROCESS_LOAD)
    {
        char filename_buf[FILENAME_LEN];
        sprintf(filename_buf, "%s/Data/%s", g_env.Paths.EmulatorDirectory, name);

        // Even when loaded from a file the system use the format specified by manually (good!)
        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA);

        *pbitmap = al_load_bitmap(filename_buf);
        if (!*pbitmap)
        {
            Quit_Msg("%s\nError loading image: \"%s\"", Msg_Get(MSG_Failed), name);
            return false;
        }

        // We use ff00ff pink as alpha mask for practical purpose
        al_convert_mask_to_alpha(*pbitmap, al_map_rgb(0xFF,0x00,0xFF));
    }

    return true;
}

static bool Data_LoadMouseCursor(ALLEGRO_MOUSE_CURSOR** pcursor, ALLEGRO_BITMAP* bitmap, int pivot_x, int pivot_y)
{
    if (DataProcessFlags & DATA_PROCESS_NULLIFY)
        *pcursor = NULL;

    if (DataProcessFlags & DATA_PROCESS_FREE)
    {
        if (*pcursor != NULL)
        {
            al_destroy_mouse_cursor(*pcursor);
            *pcursor = NULL;
        }
    }

    if (DataProcessFlags & DATA_PROCESS_LOAD)
    {
        #ifndef ARCH_ANDROID
        *pcursor = al_create_mouse_cursor(bitmap, pivot_x, pivot_y);
        #endif
    }

    return true;
}

bool Data_LoadFont(ALLEGRO_FONT** pfont, const char* name, int size)
{
    if (DataProcessFlags & DATA_PROCESS_NULLIFY)
        *pfont = NULL;

    if (DataProcessFlags & DATA_PROCESS_FREE)
    {
        if (*pfont != NULL)
        {
            al_destroy_font(*pfont);
            *pfont = NULL;
        }
    }

    if (DataProcessFlags & DATA_PROCESS_LOAD)
    {
        char filename_buf[FILENAME_LEN];
        sprintf(filename_buf, "%s/Data/%s", g_env.Paths.EmulatorDirectory, name);

        ALLEGRO_FONT* font = al_load_font(filename_buf, size, 0);
        if (!font)
        {
            Quit_Msg("%s\nError loading font: \"%s\"", Msg_Get(MSG_Failed), name);
            return false;
        }
        *pfont = font;
    }

    return true;
}

static void Data_LoadBinaries();
static void Data_ProcessVideoBitmaps();

void Data_Init(void)
{
    // Print loading message to console
    ConsolePrint(Msg_Get(MSG_Datafile_Loading));

    // FIXME-ALLEGRO5: Load from an archive file + minimal error handling.
    DataProcessFlags = DATA_PROCESS_NULLIFY;// | DATA_PROCESS_FREE | DATA_PROCESS_LOAD;
    Data_ProcessVideoBitmaps();
    Data_LoadBinaries();

    ConsolePrint("\n");
}

void Data_ProcessVideoBitmaps()
{
    // Cursors
    Data_LoadBitmap(&Graphics.Cursors.Bitmaps.Main,         "cursor_main.png");
    Data_LoadBitmap(&Graphics.Cursors.Bitmaps.Wait,         "cursor_wait.png");
    Data_LoadBitmap(&Graphics.Cursors.Bitmaps.LightPhaser,  "cursor_lightphaser.png");
    Data_LoadBitmap(&Graphics.Cursors.Bitmaps.SportsPad,    "cursor_sportspad.png");
    Data_LoadBitmap(&Graphics.Cursors.Bitmaps.TvOekaki,     "cursor_tvoekaki.png");

    // Miscellaneous
    Data_LoadBitmap(&Graphics.Misc.Dragon,                  "gfx_dragon.png");
    Data_LoadBitmap(&Graphics.Misc.Heart1,                  "gfx_heart1.png");
    Data_LoadBitmap(&Graphics.Misc.Heart2,                  "gfx_heart2.png");

    // Inputs
    Data_LoadBitmap(&Graphics.Inputs.InputsBase,            "gfx_inputs.png");
    Data_LoadBitmap(&Graphics.Inputs.Glasses,               "gfx_glasses.png");
    Data_LoadBitmap(&Graphics.Inputs.Joypad,                "gfx_joypad.png");
    Data_LoadBitmap(&Graphics.Inputs.LightPhaser,           "gfx_lightphaser.png");
    Data_LoadBitmap(&Graphics.Inputs.PaddleControl,         "gfx_paddlecontrol.png");
    Data_LoadBitmap(&Graphics.Inputs.SportsPad,             "gfx_sportspad.png");
    Data_LoadBitmap(&Graphics.Inputs.SuperHeroPad,          "gfx_superheropad.png");
    Data_LoadBitmap(&Graphics.Inputs.TvOekaki,              "gfx_tvoekaki.png");
    Data_LoadBitmap(&Graphics.Inputs.GraphicBoardV2,        "gfx_graphicboardv2.png");
    Data_LoadBitmap(&Graphics.Inputs.SK1100_Keyboard,       "gfx_keyboard.png");

    // Machines
    Data_LoadBitmap(&Graphics.Machines.MasterSystem,        "machine_sms.png");
    Data_LoadBitmap(&Graphics.Machines.MasterSystem_Cart,   "machine_sms_cartridge.png");
    Data_LoadBitmap(&Graphics.Machines.MasterSystem_Light,  "machine_sms_light.png");
    Data_LoadBitmap(&Graphics.Machines.ColecoVision,        "machine_coleco.png");

    // Flags
    Data_LoadBitmap(&Graphics.Flags [FLAG_UNKNOWN],         "flag_unknown.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_AU],              "flag_au.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_BR],              "flag_br.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_JP],              "flag_jp.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_KR],              "flag_kr.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_FR],              "flag_fr.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_US],              "flag_us.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_CH],              "flag_ch.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_HK],              "flag_hk.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_EU],              "flag_eu.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_PT],              "flag_pt.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_DE],              "flag_de.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_IT],              "flag_it.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_SP],              "flag_sp.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_SW],              "flag_sw.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_NZ],              "flag_nz.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_UK],              "flag_uk.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_CA],              "flag_ca.png");
    Data_LoadBitmap(&Graphics.Flags [FLAG_TW],              "flag_tw.png");

    // Icons
    Data_LoadBitmap(&Graphics.Icons.BAD,                    "icon_bad.png");
    Data_LoadBitmap(&Graphics.Icons.BIOS,                   "icon_bios.png");
    Data_LoadBitmap(&Graphics.Icons.Hack,                   "icon_hack.png");
    Data_LoadBitmap(&Graphics.Icons.HomeBrew,               "icon_homebrew.png");
    Data_LoadBitmap(&Graphics.Icons.Prototype,              "icon_proto.png");
    Data_LoadBitmap(&Graphics.Icons.Translation_JP,         "icon_trans_jp.png");
    Data_LoadBitmap(&Graphics.Icons.Translation_JP_US,      "icon_trans_jp_us.png");

    // Fonts
    for (int i = 0; i < FONTID_COUNT_; i++)
    {
        Data_LoadFont(&Fonts[i].library_data, Fonts[i].load_filename,   Fonts[i].load_size);
        if (DataProcessFlags & DATA_PROCESS_LOAD)
            Fonts[i].height = al_get_font_line_height(Fonts[i].library_data);
    }

    // Mouse cursors
    Data_LoadMouseCursor(&Graphics.Cursors.Main,        Graphics.Cursors.Bitmaps.Main, 0, 0);
    Data_LoadMouseCursor(&Graphics.Cursors.Wait,        Graphics.Cursors.Bitmaps.Wait, 6, 2);
    Data_LoadMouseCursor(&Graphics.Cursors.LightPhaser, Graphics.Cursors.Bitmaps.LightPhaser, 7, 7);
    Data_LoadMouseCursor(&Graphics.Cursors.SportsPad,   Graphics.Cursors.Bitmaps.SportsPad, 7, 7);
    Data_LoadMouseCursor(&Graphics.Cursors.TvOekaki,    Graphics.Cursors.Bitmaps.TvOekaki, 3, 12);
}

void Data_LoadBinaries()
{
    // BIOS
    // Note: BIOSes are unpacked in a bigger memory area, so they can be mapped
    // without having to handle memory accesses outside their allocated range.
    // The same trick is done on loading < 48 KB Sega 8-bits ROM images.
    BIOS_ROM            = (u8*)Data_CopyInBiggerArea(Data_LoadBinary("rom_sms.rom"),     0x2000, 0xC000);
    BIOS_ROM_Jap        = (u8*)Data_CopyInBiggerArea(Data_LoadBinary("rom_smsj.rom"),   0x2000, 0xC000);
    BIOS_ROM_Coleco     = (u8*)Data_CopyInBiggerArea(Data_LoadBinary("rom_coleco.rom"),  0x2000, 0x2000);
    BIOS_ROM_SF7000     = (u8*)Data_CopyInBiggerArea(Data_LoadBinary("rom_sf7000.rom"),  0x2000, 0x4000);

    // Patch SF-7000 BIOS (FIXME: move to sf7000.c)
    // Bios_ROM_SF7000[0x112] = Bios_ROM_SF7000[0x113] = Bios_ROM_SF7000[0x114] = 0x00;
    // Bios_ROM_SF7000[0x183] = Bios_ROM_SF7000[0x184] = Bios_ROM_SF7000[0x185] = 0x00;
    BIOS_ROM_SF7000[0x1D8] = BIOS_ROM_SF7000[0x1D9] = BIOS_ROM_SF7000[0x1DA] = 0x00;
}

void    Data_Close(void)
{
    Data_DestroyVideoBuffers();
}

void    Data_CreateVideoBuffers()
{
    DataProcessFlags = DATA_PROCESS_FREE | DATA_PROCESS_LOAD;
    Data_ProcessVideoBitmaps();
}

void    Data_DestroyVideoBuffers()
{
    DataProcessFlags = DATA_PROCESS_FREE;
    Data_ProcessVideoBitmaps();
}

//-----------------------------------------------------------------------------

