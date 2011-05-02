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

static void* Data_CopyInBiggerArea(void *src, int src_size, int dst_size)
{
    void *  p = malloc (dst_size);
    memcpy (p, src, src_size);
    memset ((u8 *)p + src_size, 0, dst_size - src_size);
    return (p);
}

static void* Data_LoadBinary(const char* name)
{
	char filename_buf[256];
	FILE* f;
	int size;
	void* buf;

	sprintf(filename_buf, "datafiles/%s", name);
	f = fopen(filename_buf, "rb");
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = Memory_Alloc(size);
	fread(buf, 1, size, f);
	return buf;
}

static ALLEGRO_BITMAP*	Data_LoadBitmap(const char* name)
{
	char filename_buf[256];
	ALLEGRO_BITMAP* bitmap;

	sprintf(filename_buf, "datafiles/%s", name);
	bitmap = al_load_bitmap(filename_buf);
	if (!bitmap)
	{
		//Quit_Msg("Error loading bitmap data: \"%s\"", filename_buf);
		Quit_Msg (Msg_Get (MSG_Failed));
		return NULL;
	}
	return bitmap;
}

// Load Allegro 4 type fonts
static ALLEGRO_FONT* Data_LoadFontA4(const char* name)
{
	char filename_buf[256];
	ALLEGRO_FONT* font;

	sprintf(filename_buf, "datafiles/%s", name);
	font = al_load_bitmap_font(filename_buf);
	return font;
}

//-----------------------------------------------------------------------------
// Data_Init (void)
// Load graphics, font & BIOS data from datafile
// Some data processing is done here
//-----------------------------------------------------------------------------
void            Data_Init(void)
{
    // Print loading message to console
    ConsolePrint (Msg_Get (MSG_Datafile_Loading));

	// FIXME-ALLEGRO5: Load from an archive file.
	// FIXME-ALLEGRO5: No error handling.

	// Cursors
	Graphics.Cursors.Bitmaps.Main		= Data_LoadBitmap("cursor_main.tga");
	Graphics.Cursors.Bitmaps.Wait		= Data_LoadBitmap("cursor_wait.tga");
	Graphics.Cursors.Bitmaps.LightPhaser= Data_LoadBitmap("cursor_lightphaser.tga");
	Graphics.Cursors.Bitmaps.SportsPad	= Data_LoadBitmap("cursor_sportspad.tga");
	Graphics.Cursors.Bitmaps.TvOekaki   = Data_LoadBitmap("cursor_tvoekaki.tga");

	Graphics.Cursors.Main			= al_create_mouse_cursor(Graphics.Cursors.Bitmaps.Main, 0, 0);
	Graphics.Cursors.Wait			= al_create_mouse_cursor(Graphics.Cursors.Bitmaps.Wait, 6, 2);
	Graphics.Cursors.LightPhaser	= al_create_mouse_cursor(Graphics.Cursors.Bitmaps.LightPhaser, 7, 7);
	Graphics.Cursors.SportsPad		= al_create_mouse_cursor(Graphics.Cursors.Bitmaps.SportsPad, 7, 7);
	Graphics.Cursors.TvOekaki		= al_create_mouse_cursor(Graphics.Cursors.Bitmaps.TvOekaki, 3, 12);

	// Miscellaenous
	Graphics.Misc.Dragon            = Data_LoadBitmap("gfx_dragon.tga");
	Graphics.Misc.Heart1            = Data_LoadBitmap("heart1.tga");
	Graphics.Misc.Heart2            = Data_LoadBitmap("heart2.tga");

	// Inputs
	Graphics.Inputs.InputsBase      = Data_LoadBitmap("gfx_inputs.tga");
	Graphics.Inputs.Glasses         = Data_LoadBitmap("gfx_glasses.tga");
	Graphics.Inputs.Joypad          = Data_LoadBitmap("gfx_joypad.tga");
	Graphics.Inputs.LightPhaser     = Data_LoadBitmap("gfx_lightphaser.tga");
	Graphics.Inputs.PaddleControl   = Data_LoadBitmap("gfx_paddlecontrol.tga");
	Graphics.Inputs.SportsPad       = Data_LoadBitmap("gfx_sportspad.tga");
	Graphics.Inputs.SuperHeroPad    = Data_LoadBitmap("gfx_superheropad.tga");
	Graphics.Inputs.TvOekaki        = Data_LoadBitmap("gfx_tvoekaki.tga");
	Graphics.Inputs.SK1100_Keyboard = Data_LoadBitmap("gfx_keyboard.tga");

	// Machines
	Graphics.Machines.MasterSystem          = Data_LoadBitmap("machine_sms.tga");
	Graphics.Machines.MasterSystem_Cart     = Data_LoadBitmap("machine_sms_cartridge.tga");
	Graphics.Machines.MasterSystem_Light    = Data_LoadBitmap("machine_sms_light.tga");
	Graphics.Machines.ColecoVision          = Data_LoadBitmap("machine_coleco.tga");

	// Flags
	Graphics.Flags [FLAG_UNKNOWN]   = Data_LoadBitmap("flag_unknown.tga");
	Graphics.Flags [FLAG_AU]        = Data_LoadBitmap("flag_au.tga");
	Graphics.Flags [FLAG_BR]        = Data_LoadBitmap("flag_br.tga");
	Graphics.Flags [FLAG_JP]        = Data_LoadBitmap("flag_jp.tga");
	Graphics.Flags [FLAG_KR]        = Data_LoadBitmap("flag_kr.tga");
	Graphics.Flags [FLAG_FR]        = Data_LoadBitmap("flag_fr.tga");
	Graphics.Flags [FLAG_US]        = Data_LoadBitmap("flag_us.tga");
	Graphics.Flags [FLAG_CH]        = Data_LoadBitmap("flag_ch.tga");
	Graphics.Flags [FLAG_HK]        = Data_LoadBitmap("flag_hk.tga");
	Graphics.Flags [FLAG_EU]        = Data_LoadBitmap("flag_eu.tga");
	Graphics.Flags [FLAG_PT]        = Data_LoadBitmap("flag_pt.tga");
	Graphics.Flags [FLAG_DE]        = Data_LoadBitmap("flag_de.tga");
	Graphics.Flags [FLAG_IT]        = Data_LoadBitmap("flag_it.tga");
	Graphics.Flags [FLAG_SP]        = Data_LoadBitmap("flag_sp.tga");
	Graphics.Flags [FLAG_SW]        = Data_LoadBitmap("flag_sw.tga");
	Graphics.Flags [FLAG_NZ]        = Data_LoadBitmap("flag_nz.tga");
	Graphics.Flags [FLAG_UK]        = Data_LoadBitmap("flag_uk.tga");
	Graphics.Flags [FLAG_CA]        = Data_LoadBitmap("flag_ca.tga");
	Graphics.Flags [FLAG_TW]        = Data_LoadBitmap("flag_tw.tga");

	// Icons
	Graphics.Icons.BAD              = Data_LoadBitmap("icon_bad.tga");
	Graphics.Icons.BIOS             = Data_LoadBitmap("icon_bios.tga");
	Graphics.Icons.Hack             = Data_LoadBitmap("icon_hack.tga");
	Graphics.Icons.HomeBrew         = Data_LoadBitmap("icon_homebrew.tga");
	Graphics.Icons.Prototype        = Data_LoadBitmap("icon_proto.tga");
	Graphics.Icons.Translation_JP   = Data_LoadBitmap("icon_trans_jp.tga");
	Graphics.Icons.Translation_JP_US= Data_LoadBitmap("icon_trans_jp_us.tga");

    // BIOS
    // Note: BIOSes are unpacked in a bigger memory area, so they can be mapped
    // without having to handle memory accesses outside their allocated range.
    // The same trick is done on loading < 48 KB Sega 8-bits ROM images.
    BIOS_ROM            = Data_CopyInBiggerArea(Data_LoadBinary("rom_sms.rom"),     0x2000, 0xC000);
    BIOS_ROM_Jap        = Data_CopyInBiggerArea(Data_LoadBinary("rom_smsj.rom"),	0x2000, 0xC000);
    BIOS_ROM_Coleco     = Data_CopyInBiggerArea(Data_LoadBinary("rom_coleco.rom"),  0x2000, 0x2000);
    BIOS_ROM_SF7000     = Data_CopyInBiggerArea(Data_LoadBinary("rom_sf7000.rom"),  0x2000, 0x4000);

    // Patch SF-7000 BIOS (FIXME: move to sf7000.c)
    // Bios_ROM_SF7000[0x112] = Bios_ROM_SF7000[0x113] = Bios_ROM_SF7000[0x114] = 0x00;
    // Bios_ROM_SF7000[0x183] = Bios_ROM_SF7000[0x184] = Bios_ROM_SF7000[0x185] = 0x00;
    BIOS_ROM_SF7000[0x1D8] = BIOS_ROM_SF7000[0x1D9] = BIOS_ROM_SF7000[0x1DA] = 0x00;

    // Fonts
    Fonts_AddFont(F_LARGE,	Data_LoadFontA4("font_0.tga"));  // Font Large Sized
    Fonts_AddFont(F_MIDDLE, Data_LoadFontA4("font_1.tga"));  // Font Middle Sized
    Fonts_AddFont(F_SMALL,  Data_LoadFontA4("font_2.tga"));  // Font Small Sized
}

void    Data_Close(void)
{
}

// Convert truecolor graphics stored in datafile in current native format
void    Data_UpdateVideoMode()
{
	// FIXME-ALLEGRO5: This should be obsolete now
#if 0
    int i;
    t_list *bmp_copies;

    assert(g_Datafile != NULL);
    bmp_copies = g_DatafileBitmapCopy32;

    if (bmp_copies == NULL)
    {
		// First time, copy everything into 32-bits buffers.
        fixup_datafile(g_Datafile);
        for (i = 0; g_Datafile[i].type != DAT_END; i++)
        {
            if (g_Datafile[i].type == DAT_BITMAP)
            {
                ALLEGRO_BITMAP * dat_bmp = g_Datafile[i].dat;
                ALLEGRO_BITMAP * copy_bmp = create_bitmap_ex(32, dat_bmp->w, dat_bmp->h);
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
			// Recreate it all
			ALLEGRO_BITMAP * src_bmp = bmp_copies->elem;
			ALLEGRO_BITMAP * dst_bmp = al_create_bitmap( src_bmp->w, src_bmp->h );
			al_destroy_bitmap( g_Datafile[i].dat );
			g_Datafile[i].dat = dst_bmp;
			blit(src_bmp, dst_bmp, 0, 0, 0, 0, src_bmp->w, src_bmp->h);
            bmp_copies = bmp_copies->next;
        }
    }
    //fixup_datafile(g_Datafile);
	Data_UpdateNamedPointers();
#endif
}

//-----------------------------------------------------------------------------

