//-----------------------------------------------------------------------------
// MEKA - datadump.c
// Data Dumping - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "mappers.h"
#include "datadump.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_data_dump DataDump;

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef int (*t_data_dump_handler_ascii)(FILE *f_dump, int pos, u8 const *data, int len, int start_addr);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DataDump_Init ()
// Initialize data dumping functionnality
//-----------------------------------------------------------------------------
void    DataDump_Init (void)
{
    DataDump.Mode = DATADUMP_MODE_ASCII;
}

//-----------------------------------------------------------------------------
// DataDump_Init_Menus ()
// Create data dumping menu items in given GUI menu
//-----------------------------------------------------------------------------
void    DataDump_Init_Menus (int menu_id)
{
    menu_add_item (menu_id, "RAM",           NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_RAM,			NULL);
    menu_add_item (menu_id, "VRAM",          NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_VRAM,			NULL);
    menu_add_item (menu_id, "Palette",       NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_Palette,		NULL);
    menu_add_item (menu_id, "Sprites",       NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_Sprites,		NULL);
    //menu_add_item (menu_id, "BG/FG Map",   0,         (t_menu_callback)DataDump_BgFgMap,		NULL);
    menu_add_item (menu_id, "CPU Regs",      NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_CPURegs,		NULL);
    menu_add_item (menu_id, "VDP Regs",      NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_VRegs,			NULL);
    menu_add_item (menu_id, "OnBoardMemory", NULL,	MENU_ITEM_FLAG_ACTIVE, (t_menu_callback)DataDump_OnBoardMemory,	NULL);

    menus_ID.dump_cfg = menu_add_menu (menu_id, "Configuration",   MENU_ITEM_FLAG_ACTIVE);
    menu_add_item (menus_ID.dump_cfg, "ASCII",  NULL,	MENU_ITEM_FLAG_ACTIVE | ((DataDump.Mode == DATADUMP_MODE_ASCII) ? MENU_ITEM_FLAG_CHECKED : 0), (t_menu_callback)DataDump_Mode_Ascii,		NULL);
    menu_add_item (menus_ID.dump_cfg, "Raw",    NULL,	MENU_ITEM_FLAG_ACTIVE | ((DataDump.Mode == DATADUMP_MODE_RAW)   ? MENU_ITEM_FLAG_CHECKED : 0), (t_menu_callback)DataDump_Mode_Raw,		NULL);
}

//-----------------------------------------------------------------------------
// DataDump_Mode_Ascii ()
// Activate ASCII dumping mode
//-----------------------------------------------------------------------------
void    DataDump_Mode_Ascii (void)
{
    // Set ASCII mode
    DataDump.Mode = DATADUMP_MODE_ASCII;

    // Update GUI checks & print message to user
    gui_menu_uncheck_all (menus_ID.dump_cfg);
    gui_menu_check (menus_ID.dump_cfg, 0);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_DataDump_Mode_Ascii));
}

//-----------------------------------------------------------------------------
// DataDump_Mode_Raw ()
// Activate RAW dumping mode
//-----------------------------------------------------------------------------
void    DataDump_Mode_Raw (void)
{
    // Set RAW mode
    DataDump.Mode = DATADUMP_MODE_RAW;

    // Update GUI checks & print message to user
    gui_menu_uncheck_all (menus_ID.dump_cfg);
    gui_menu_check (menus_ID.dump_cfg, 1);
    Msg(MSGT_USER, "%s", Msg_Get(MSG_DataDump_Mode_Raw));
}

static void     DataDump_Write_Filename (char *s, const char *name)
{
    if (!al_filename_exists (g_env.Paths.DebugDirectory))
        al_make_directory (g_env.Paths.DebugDirectory);
    sprintf(s, "%s/%s", g_env.Paths.DebugDirectory, name);
}

static void     DataDump_Main_Raw (const char *name, const u8 *data, int len)
{
    FILE *      f_dump;
    char        filename[FILENAME_LEN];

    DataDump_Write_Filename (filename, name);
    if ((f_dump = fopen(filename, "wb")) == NULL)
    {
        Msg(MSGT_USER, Msg_Get(MSG_DataDump_Error), name);
        return;
    }

    // Dump
    fwrite (data, 1, len, f_dump);

    // Close file & print a message
    fclose (f_dump);
    Msg(MSGT_USER, Msg_Get(MSG_DataDump_Main),
        name,
        (len % 1024 == 0) ? (len / 1024) : len,
        (len % 1024 == 0) ? "KBytes" : "bytes",
        "Raw");
}

static void     DataDump_Main_Ascii (const char *name, const u8 *data, int len, int start_addr, t_data_dump_handler_ascii func)
{
    int         i;
    FILE *      f_dump;
    char        filename[FILENAME_LEN];

    DataDump_Write_Filename (filename, name);
    if ((f_dump = fopen(filename, "wt")) == NULL)
    {
        Msg(MSGT_USER, Msg_Get(MSG_DataDump_Error), name);
        return;
    }

    // Print a header
    fprintf(f_dump, MEKA_NAME_VERSION "\n");
    fprintf(f_dump, "** %s dump\n", name);
    fprintf(f_dump, "** File: %s\n", g_env.Paths.MediaImageFile);
    // Note: meka_date_getf() return a string with an ending \n
    fprintf(f_dump, "** Date: %s\n", meka_date_getf());

    // Dumping
    i = 0;
    while (i < len)
        i = func(f_dump, i, data, len, start_addr);

    // Close file & print a message
    fclose (f_dump);
    Msg(MSGT_USER, Msg_Get(MSG_DataDump_Main),
        name,
        (len % 1024 == 0) ? (len / 1024) : len,
        (len % 1024 == 0) ? "Kbytes" : "bytes",
        "ASCII");
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_Standard ()
// Data dump ASCII output handler:
// - Standard (hexadecimal & ascii equivalents, 16 per line)
//-----------------------------------------------------------------------------
static int  DataDump_Handler_Ascii_Standard (FILE *f_dump, int pos, u8 const *data, int len, int start_addr)
{
    int     i;

    fprintf(f_dump, "%04X-%04X | ", pos + start_addr, pos + start_addr + 15);
    for (i = 0; (i < 16) && (pos + i < len); i++)
        fprintf(f_dump, "%02X ", data[pos + i]);
    fprintf(f_dump, "| ");
    for (i = 0; (i < 16) && (pos + i < len); i++)
        fprintf(f_dump, "%c", (data[pos + i] >= 32) ? data[pos + i] : '.');
    fprintf(f_dump, "\n");
    return (pos + 16);
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_CPURegs_Reg ()
// Helper function to dump a Z80 CPU register
//-----------------------------------------------------------------------------
static void DataDump_Handler_Ascii_CPURegs_Reg (FILE *f_dump, const char *name, word value, const char *comment)
{
    char    bitfields[2][9];

    StrWriteBitfield ((value >> 8) & 0xFF, 8, bitfields[0]);
    StrWriteBitfield (value & 0xFF, 8, bitfields[1]);
    if (comment)
        fprintf(f_dump, "%-3s = $%04X | %%%s.%s | %s\n", name, value, bitfields[0], bitfields[1], comment);
    else
        fprintf(f_dump, "%-3s = $%04X | %%%s.%s |\n", name, value, bitfields[0], bitfields[1]);
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_CPURegs ()
// Data dump ASCII output handler:
// - CPU Registers (all registers at once, given by 'data' pointer)
//-----------------------------------------------------------------------------
static int  DataDump_Handler_Ascii_CPURegs (FILE *f_dump, int pos, const u8 *data, int len, int start_addr)
{
    Z80 *   R;

    R = (Z80 *)data;
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "AF", R->AF.W, "Accumulator and flags (SZ.H.PNC)");
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "BC", R->BC.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "DE", R->DE.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "HL", R->HL.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "IX", R->IX.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "IY", R->IY.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "PC", R->PC.W, "Program Counter");
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "SP", R->SP.W, "Stack Pointer");
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "AF'", R->AF1.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "BC'", R->BC1.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "DE'", R->DE1.W, NULL);
    DataDump_Handler_Ascii_CPURegs_Reg (f_dump, "HL'", R->HL1.W, NULL);
    fprintf(f_dump, "%-3s = $%02X\n", "I", (byte)(R->I));
    fprintf(f_dump, "%-3s = $%02X\n", "R", (byte)((R->R & 0x7F)|(R->R7)));
    fprintf(f_dump, "IPeriod = %d\n", R->IPeriod);
    fprintf(f_dump, "ICount  = %d\n", R->ICount);
    fprintf(f_dump, "IFF1 = %d\nIFF2 = %d\nIM   = %d\nEI   = %d\nHALT = %d\n", 
        (R->IFF & IFF_1) ? 1 : 0, (R->IFF & IFF_2) ? 1 : 0, (R->IFF & (IFF_IM1 | IFF_IM2)) >> 1, (R->IFF & IFF_EI) ? 1 : 0, (R->IFF & IFF_HALT) ? 1 : 0);
    fprintf(f_dump, "\n");
    fprintf(f_dump, "(some implementation information were not dumped here.\n");
    fprintf(f_dump, " Please contact me if they may be of your interest)\n");
    return (sizeof (sms.R));
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_VReg ()
// Data dump ASCII output handler:
// - VDP Registers (hexadecimal & binary, 1 per line)
//-----------------------------------------------------------------------------
static int  DataDump_Handler_Ascii_VReg (FILE *f_dump, int pos, const u8 *data, int len, int start_addr)
{
    char    bitfield[9];

    StrWriteBitfield (data[pos], 8, bitfield);
    fprintf(f_dump, "VReg[%02d] = $%02X | %%%s\n", pos, data[pos], bitfield);
    return (pos + 1);
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_Palette ()
// Data dump ASCII output handler:
// - Palette (colors, 1 or 2 bytes per line depending on driver)
//-----------------------------------------------------------------------------
static int  DataDump_Handler_Ascii_Palette (FILE *f_dump, int pos, const u8 *data, int len, int start_addr)
{
    char    bitfields[2][9];

    switch (g_machine.driver_id)
    {
    case DRV_SMS:
        {
            const u8 palette_data = data[pos];
            StrWriteBitfield (palette_data, 8, bitfields[0]);
            fprintf(f_dump, "Color %02d : %%%s | $%02X | R=%d, G=%d, B=%d\n", pos, bitfields[0], palette_data, palette_data & 3, (palette_data >> 2) & 3, (palette_data >> 4) & 3);
            return (pos + 1);
        }
    case DRV_GG:
        {
            const u16 palette_data = *(u16 *)(data + pos);
            StrWriteBitfield (data[pos+0], 8, bitfields[0]);
            StrWriteBitfield (data[pos+1], 8, bitfields[1]);
            fprintf(f_dump, "Color %02d : %%%s.%s | $%04X | R=%d,%s G=%d,%s B=%d\n", pos / 2, bitfields[1], bitfields[0], palette_data, (palette_data & 15), (palette_data & 15) < 10 ? " " : "", (palette_data >> 4) & 15, ((palette_data >> 4) & 15) < 10 ? " " : "", (palette_data >> 8) & 15);
            return (pos + 2);
        }
    }
    Msg(MSGT_USER, "Error #9011: datadump.c::DataDump_Handler_Ascii_Palette(). Please report.");
    return (len); // End dump
}

//-----------------------------------------------------------------------------
// DataDump_Handler_Ascii_Sprite ()
// Data dump ASCII output handler:
// - Sprites (x, y, attr, unknown)
//-----------------------------------------------------------------------------
static int  DataDump_Handler_Ascii_Sprite (FILE *f_dump, int pos, const u8 *data, int len, int start_addr)
{
	if (tsms.VDP_VideoMode > 4)
	{
		int  n;
		n = pos / 4;
		if (pos == 0)
		{
			fprintf(f_dump,
				"Sprite pattern base: $%04X\n"
				"Sprite shift X (early clock): %d pixels\n\n",
				(int)(g_machine.VDP.sprite_pattern_gen_address - VRAM),
				g_machine.VDP.sprite_shift_x);
			fprintf(f_dump,
				"            Raw Data      |    X     Y     T     ?\n"
				"---------- --------------- ------------------------\n");
		}
		fprintf(f_dump,
			"Sprite %02d : %02X %02X . %02X %02X |  % 4d  % 4d  % 4d  % 4d\n",
			n, data[n], data[0x40 + n], data[0x80 + n*2], data[0x80 + n*2+1],
			data[0x80 + n*2], data[n], data[0x80 + n*2+1], data[0x40 + n]);
	}
    else
    {
        if (pos == 0)
		{
			fprintf(f_dump,
				"Sprite pattern base: $%04X\n\n",
				(int)(g_machine.VDP.sprite_pattern_gen_address - VRAM));
			fprintf(f_dump,
				"            Raw Data    |    X     Y     T    C/A\n"
				"----------- ------------ ------------------------\n");
		}
        fprintf(f_dump,
            "Sprite %02d : %02X %02X %02X %02X |  % 4d  % 4d  % 4d  % 4d%s\n",
            pos / 4,
            data[pos+0], data[pos+1], data[pos+2], data[pos+3],
            data[pos+1], data[pos+0], data[pos+2], data[pos+3],
            (data[pos+3] & 128) ? " (Shifted)" : "");

    }

    // Increase data pointer by 4 (size of a sprite entry)
    return (pos + 4);
}

//-----------------------------------------------------------------------------
// Dump RAM
//-----------------------------------------------------------------------------
void        DataDump_RAM (void)
{
    int     len;
    int     start_addr;

    Mapper_Get_RAM_Infos (&len, &start_addr);
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("RAM", RAM, len);
    else
        DataDump_Main_Ascii ("RAM", RAM, len, start_addr, DataDump_Handler_Ascii_Standard);
}

//-----------------------------------------------------------------------------
// Dump VRAM
//-----------------------------------------------------------------------------
void        DataDump_VRAM (void)
{
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("VRAM", VRAM, 0x4000);
    else
        DataDump_Main_Ascii ("VRAM", VRAM, 0x4000, 0x0000, DataDump_Handler_Ascii_Standard);
}

//-----------------------------------------------------------------------------
// Dump Palette
//-----------------------------------------------------------------------------
void        DataDump_Palette (void)
{
    int     len;

    switch (g_machine.driver_id)
    {
    case DRV_SMS:      len = 32;       break;
    case DRV_GG:       len = 64;       break;
    default:           len = 0;        break;
    }
    if (len == 0)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_DataDump_Error_Palette));
        return;
    }
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("Palette", PRAM, len);
    else
        DataDump_Main_Ascii ("Palette", PRAM, len, 0, DataDump_Handler_Ascii_Palette);
}

//-----------------------------------------------------------------------------
// Dump Sprites
//-----------------------------------------------------------------------------
void    DataDump_Sprites (void)
{
    int n_sprites = 0;

    switch (tsms.VDP_VideoMode)
    {
    case 0:  n_sprites = 0; break;
    case 1:
    case 2:
    case 3:  n_sprites = 32; break;
    default: n_sprites = 64; break;
    }

    if (n_sprites == 0)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_DataDump_Error_Sprites));
        return;
    }

    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("Sprites", g_machine.VDP.sprite_attribute_table, n_sprites * 4);
    else
        DataDump_Main_Ascii ("Sprites", g_machine.VDP.sprite_attribute_table, n_sprites * 4, /* Unused */ 0, DataDump_Handler_Ascii_Sprite);
}

//-----------------------------------------------------------------------------
// Dump BackGround/ForeGround screen map
// FIXME: To do
//-----------------------------------------------------------------------------
void    DataDump_BgFgMap (void)
{
}

//-----------------------------------------------------------------------------
// Dump CPU Registers
//-----------------------------------------------------------------------------
void    DataDump_CPURegs (void)
{
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("CPURegs", (byte *)&sms.R, sizeof (sms.R));
    else
        DataDump_Main_Ascii ("CPURegs", (byte *)&sms.R, sizeof (sms.R), 0, DataDump_Handler_Ascii_CPURegs);
}

//-----------------------------------------------------------------------------
// Dump VDP Registers
//-----------------------------------------------------------------------------
void    DataDump_VRegs (void)
{
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("VRegs", sms.VDP, 16);
    else
        DataDump_Main_Ascii ("VRegs", sms.VDP, 16, 0, DataDump_Handler_Ascii_VReg);
}

//-----------------------------------------------------------------------------
// Dump OnBoard Memory
//-----------------------------------------------------------------------------
void        DataDump_OnBoardMemory (void)
{
    void *  data;
    int     len;

    BMemory_Get_Infos (&data, &len);
    if (data == NULL)
    {
        Msg(MSGT_USER, "%s", Msg_Get(MSG_DataDump_Error_OB_Memory));
        return;
    }
    if (DataDump.Mode == DATADUMP_MODE_RAW)
        DataDump_Main_Raw   ("OBMem", (u8*)data, len);
    else
        DataDump_Main_Ascii ("OBMem", (u8*)data, len, 0, DataDump_Handler_Ascii_Standard);
}

//-----------------------------------------------------------------------------
