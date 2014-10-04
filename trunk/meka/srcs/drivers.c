//-----------------------------------------------------------------------------
// MEKA - drivers.c
// Machine Drivers - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_palview.h"
#include "app_tileview.h"

//-----------------------------------------------------------------------------

ts_driver * g_driver = NULL;

static ts_driver drivers [DRV_MAX] =
{
  // Note: the "colors" field is the number of colors to be shown in the Palette applet                                      (work)
  // Driver ----- Name ------- Full Name ------------ CPU ----- VDP Chip --- SND Chip ------ X -- Y -- XS - YS - XE - YSS/SE/INT - C - RAM -----
  {  DRV_SMS,    "SMS",      "Sega Master System",    CPU_Z80,  VDP_SMSGG,   SND_SN76489,    256, 192,  0,  0,   255, 0,  0,  0,  32,  0x02000 },
  {  DRV_GG,     "GG",       "Sega Game Gear",        CPU_Z80,  VDP_SMSGG,   SND_SN76489,    160, 144, 48, 24,   207, 0,  0,  0,  32,  0x02000 },
  {  DRV_SG1000, "SG-1000",  "Sega Game 1000",        CPU_Z80,  VDP_TMS9918, SND_SN76489AN,  256, 192,  0,  0,   255, 0,  0,  0,  16,  0x01000 },
  {  DRV_SC3000, "SC-3000",  "Sega Computer 3000",    CPU_Z80,  VDP_TMS9918, SND_SN76489AN,  256, 192,  0,  0,   255, 0,  0,  0,  16,  0x08000 },
  {  DRV_COLECO, "COLECO",   "Coleco Vision",         CPU_Z80,  VDP_TMS9918, SND_SN76489AN,  256, 192,  0,  0,   255, 0,  0,  0,  16,  0x00400 },
  {  DRV_MSX___, "MSX",      "MSX-1",                 CPU_Z80,  VDP_TMS9918, SND_SN76489AN,  256, 192,  0,  0,   255, 0,  0,  0,  16,  0 /*?*/ },
  {  DRV_NES___, "NES",      "Nintendo",              0,		0,			 0,				 256, 240,  0,  0,   255, 0,  0,  0,  32,  0x00800 },
  {  DRV_SF7000, "SF-7000",  "Super Control Station", CPU_Z80,  VDP_TMS9918, SND_SN76489AN,  256, 192,  0,  0,   255, 0,  0,  0,  16,  0x10000 }
  // Driver ----- Name ------- Full Name ------------ CPU ----- VDP Chip --- SND Chip ------ X -- Y -- XS - YS - XE - YSS/SE/INT - C - RAM -----
};

//-----------------------------------------------------------------------------

static const ts_driver_filename_extension   drivers_ext [] =
{
    { "SMS",      DRV_SMS         },
    { "MK3",      DRV_SMS         },
    { "GG",       DRV_GG          },
    { "SG",       DRV_SG1000      },
    { "SC",       DRV_SC3000      },
    { "SF7",      DRV_SF7000      },
    { "OMV",      DRV_SG1000      }, // Othello Multivision
    { "COL",      DRV_COLECO      },
    { "ROM",      DRV_COLECO      },
    { "BIN",      DRV_COLECO      },
    { 0,          DRV_SMS         }
};

//-----------------------------------------------------------------------------

void    drv_init (void)
{
    drv_set (DRV_SMS);
}

void    drv_set (int num)
{
    if (num < 0 || num >= DRV_MAX)
    {
        Quit_Msg("%s", Msg_Get(MSG_Driver_Unknown));
    }
    else
    {
        g_driver = &drivers[num];
        if (opt.GUI_Inited == TRUE)
        {
            int palette_max = 2;
            switch (g_driver->vdp)
            {
                case VDP_SMSGG:     palette_max = 2;  break;
                case VDP_TMS9918:   palette_max = 15; break;
            }
            TileViewer_Configure_PaletteMax(palette_max);
            PaletteViewer_SetPaletteSize(&PaletteViewer, g_driver->colors);
        }
    }
}

int         drv_get_from_filename_extension(const char *filename_extension)
{
	int i = 0;
    while (drivers_ext [i].filename_extension != NULL)
    {
        if (stricmp(filename_extension, drivers_ext [i].filename_extension) == 0)
            return (drivers_ext [i].driver);
        i++;
    }
    return (drivers_ext [i].driver);
}

//-----------------------------------------------------------------------------
// drv_id_to_mode(int id)
// Convert driver ID to the old kind of ID
// (used by the savestate loader, when MSV version is < 0x05
//-----------------------------------------------------------------------------
int     drv_id_to_mode(int id)
{
	switch (id)
	{
	case DRV_GG:        return (1);
	case DRV_SG1000:    return (2);
	case DRV_SC3000:    return (2 | 8);
	case DRV_COLECO:    return (2 | 4);
	case DRV_SF7000:    return (-1); // Was not existing, then
	}
	return (0);
}

//-----------------------------------------------------------------------------
