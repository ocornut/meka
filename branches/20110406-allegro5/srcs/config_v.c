//-----------------------------------------------------------------------------
// MEKA - config_v.c
// Configuration File: Video Drivers - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "config_v.h"

//-----------------------------------------------------------------------------

#if 0	// FIXME-ALLEGRO5: no video driver
t_video_driver  video_drivers_table[] =
{
    //-----------------------------------------------------------------------------
    // Global
    //-----------------------------------------------------------------------------
    { "auto",               GFX_AUTODETECT_FULLSCREEN,              GFX_AUTODETECT_WINDOWED,        "Default / Fullscreen"  },
#ifndef ARCH_DOS
    { "auto_win",           GFX_AUTODETECT_WINDOWED,                GFX_AUTODETECT_FULLSCREEN,      "Default / Windowed"    },
#endif
    { "safe",		    GFX_SAFE,                               0,                              "Find any working mode" },

    //-----------------------------------------------------------------------------
    // MS-DOS
    //-----------------------------------------------------------------------------
#ifdef GFX_VGA
    { "vga",		    GFX_VGA,                                0,                              NULL    },
#endif
#ifdef GFX_MODEX
    { "modex",		    GFX_MODEX,                              0,                              NULL    },
#endif
#ifdef GFX_VESA1
    { "vesa1",		    GFX_VESA1,                              0,                              NULL    },
#endif
#ifdef GFX_VESA2B
    { "vesa2b",		    GFX_VESA2B,                             0,                              NULL    },
#endif
#ifdef GFX_VESA2L
    { "vesa2l",		    GFX_VESA2L,                             0,                              NULL    },
#endif
#ifdef GFX_VESA3
    { "vesa3",		    GFX_VESA3,                              0,                              NULL    },
#endif
#ifdef GFX_VBEAF
    { "vbeaf",		    GFX_VBEAF,                              0,                              NULL    },
#endif
#ifdef GFX_EXTENDED
    { "extended",	    GFX_EXTENDED,                           0,                              NULL    },
#endif

    //-----------------------------------------------------------------------------
    // UNIX
    //-----------------------------------------------------------------------------
#ifdef GFX_XWINDOWS
    { "xwin",		    GFX_XWINDOWS,                           GFX_XWINDOWS_FULLSCREEN,        "X Window / Windowed"           },
#endif
#ifdef GFX_XWINDOWS_FULLSCREEN
    { "xwin_fs",	    GFX_XWINDOWS_FULLSCREEN,                GFX_XWINDOWS,                   "X Window / Fullscreen"         },
#endif
#ifdef GFX_XDGA
    { "xdga",		    GFX_XDGA,                               GFX_XDGA_FULLSCREEN,            "XFree86 DGA 1.0 / Windowed"    },
#endif
#ifdef GFX_XDGA_FULLSCREEN
    { "xdga_fs",	    GFX_XDGA_FULLSCREEN,                    GFX_XDGA,                       "XFree86 DGA 1.0 / Fullscreen"  },
#endif
#ifdef GFX_XDGA2
    { "xdga2",	            GFX_XDGA2,                              0,                              "XFree86 DGA 2.0 / Fullscreen"  },
#endif
#ifdef GFX_XDGA2_SOFT
    { "xdga2_soft",	    GFX_XDGA2_SOFT,                         0,                              "XFree86 DGA 2.0 Unaccelerated / Fullscreen" },
#endif
#ifdef GFX_FBCON
    { "fbcon",		    GFX_FBCON,                              0,                              "Framebuffer"                   },
#endif
#ifdef GFX_GGI
    { "ggi",		    GFX_GGI,                                0,                              "VBE/AF"                        },
#endif
#ifdef GFX_SVGALIB
    { "svgalib",	    GFX_SVGALIB,                            0,                              "SVGAlib"                       },
#endif
    //-----------------------------------------------------------------------------
    // Windows
    //-----------------------------------------------------------------------------
#ifdef GFX_DIRECTX
    { "directx",            GFX_DIRECTX,                            GFX_DIRECTX_WIN,                "DirectX / Fullscreen"          },
#endif
#ifdef GFX_DIRECTX_WIN
    { "directx_win",        GFX_DIRECTX_WIN,                        GFX_DIRECTX,                    "DirectX / Windowed"            },
#endif
#ifdef GFX_DIRECTX_SOFT
    { "directx_soft",       GFX_DIRECTX_SOFT,                       0,                              "DirectX Software / Fullscreen" },
#endif
#ifdef GFX_DIRECTX_SAFE
    { "directx_safe",	    GFX_DIRECTX_SAFE,                       0,                              "DirectX Safe"                  },
#endif
#ifdef GFX_DIRECTX_OVL
    { "directx_ovl",        GFX_DIRECTX_OVL,                        0,                              "DirectX Overlay / Windowed"    },
#endif
#ifdef GFX_GDI
    { "gdi",		    GFX_GDI,                                0,                              "GDI / Windowed"                },
#endif
    //-----------------------------------------------------------------------------
    // BeOS
    //-----------------------------------------------------------------------------
#ifdef GFX_BEOS
    { "beos",		        GFX_BEOS,                           0,                              NULL    },
#endif
#ifdef GFX_BEOS_FULLSCREEN
    { "beos_fullscreen",	GFX_BEOS_FULLSCREEN,                GFX_BEOS_WINDOWED,              NULL    },
#endif
#ifdef GFX_BEOS_FULLSCREEN_SAFE
    { "beos_fullscreen_safe",   GFX_BEOS_FULLSCREEN_SAFE,           GFX_BEOS_WINDOWED_SAFE,         NULL    },
#endif
#ifdef GFX_BEOS_WINDOWED
    { "beos_win",		GFX_BEOS_WINDOWED,                  GFX_BEOS_FULLSCREEN,            NULL    },
#endif
#ifdef GFX_BEOS_WINDOWED_SAFE
    { "beos_win_safe",   	GFX_BEOS_WINDOWED_SAFE,             GFX_BEOS_FULLSCREEN_SAFE,       NULL    },
#endif
    { NULL,       0           }
};
#endif

//-----------------------------------------------------------------------------

#if 0 	// FIXME-ALLEGRO5: no video driver
t_video_driver *    VideoDriver_FindByDesc(char *s)
{
    t_video_driver *drv = &video_drivers_table[0];
    while (drv->desc != NULL)
    {
        if (strcmp(s, drv->desc) == 0)
            return (drv);
        drv++;
    }

    // Default to auto
    return VideoDriver_FindByDesc("auto");
}

t_video_driver *    VideoDriver_FindByDriverId(int drv_id)
{
    t_video_driver *drv = &video_drivers_table[0];
    while (drv->desc != NULL)
    {
        if (drv->drv_id == drv_id)
            return (drv);
        drv++;
    }

    // Default to auto
    return VideoDriver_FindByDriverId(GFX_AUTODETECT_FULLSCREEN);
}

void                VideoDriver_DumpAllDesc(FILE *f)
{
    t_video_driver *drv = &video_drivers_table[0];
    while (drv->desc != NULL)
    {
        int len = fprintf(f, "    %s", drv->desc);
        if (drv->comment != NULL)
            fprintf(f, "%*s(%s)\n", 20 - len, "", drv->comment);
        else
            fprintf(f, "\n");
        drv++;
    }
}
#endif

//-----------------------------------------------------------------------------

