//-----------------------------------------------------------------------------
// MEKA - config_j.c
// Configuration File: Joypad/stick Drivers - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "config_j.h"

//-----------------------------------------------------------------------------

static S2I_TYPE S2J_Table [] =
 {
   { "none",            JOY_TYPE_NONE                   },
#ifdef JOY_TYPE_STANDARD
   { "2b",		JOY_TYPE_STANDARD		},
#endif
#ifdef JOY_TYPE_2PADS
   { "dual",		JOY_TYPE_2PADS			},
   { "2pads",		JOY_TYPE_2PADS			},
#endif
#ifdef JOY_TYPE_4BUTTON
   { "4b",		JOY_TYPE_4BUTTON		},
#endif
#ifdef JOY_TYPE_6BUTTON
   { "6b",		JOY_TYPE_6BUTTON		},
#endif
#ifdef JOY_TYPE_8BUTTON
   { "8b",		JOY_TYPE_8BUTTON		},
#endif
#ifdef JOY_TYPE_FSPRO
   { "fspro",		JOY_TYPE_FSPRO			},
   { "flightstick",    	JOY_TYPE_FSPRO			},
#endif
#ifdef JOY_TYPE_WINGEX
   { "wingex",		JOY_TYPE_WINGEX			},
   { "wingmanextreme", 	JOY_TYPE_WINGEX			},
#endif
#ifdef JOY_TYPE_SIDEWINDER_AG
   { "sw",		JOY_TYPE_SIDEWINDER_AG          },
   { "sidewinder",     	JOY_TYPE_SIDEWINDER_AG		},
#endif
#ifdef JOY_TYPE_GAMEPADPRO
   { "gravis",		JOY_TYPE_GAMEPADPRO		},
   { "gamepadpro",     	JOY_TYPE_GAMEPADPRO		},
#endif
#ifdef JOY_TYPE_GRIP
   { "grip",		JOY_TYPE_GRIP			},
#endif
#ifdef JOY_TYPE_GRIP4
   { "grip4",		JOY_TYPE_GRIP4			},
#endif
#ifdef JOY_TYPE_SNESPAD_LPT1
   { "snes1",       	JOY_TYPE_SNESPAD_LPT1		},
   { "sneslpt1",       	JOY_TYPE_SNESPAD_LPT1		},
   { "snes2",       	JOY_TYPE_SNESPAD_LPT2		},
   { "sneslpt2",       	JOY_TYPE_SNESPAD_LPT2		},
   { "snes3",       	JOY_TYPE_SNESPAD_LPT3		},
   { "sneslpt3",       	JOY_TYPE_SNESPAD_LPT3		},
#endif
#ifdef JOY_TYPE_PSXPAD_LPT1
   { "psx1",		JOY_TYPE_PSXPAD_LPT1		},
   { "psxlpt1",		JOY_TYPE_PSXPAD_LPT1		},
   { "psx2",		JOY_TYPE_PSXPAD_LPT2		},
   { "psxlpt2",		JOY_TYPE_PSXPAD_LPT2		},
   { "psx3",		JOY_TYPE_PSXPAD_LPT3		},
   { "psxlpt3",		JOY_TYPE_PSXPAD_LPT3		},
#endif
#ifdef JOY_TYPE_N64PAD_LPT1
   { "n641",		JOY_TYPE_N64PAD_LPT1		},
   { "n64ltp1",		JOY_TYPE_N64PAD_LPT1		},
   { "n642",		JOY_TYPE_N64PAD_LPT2		},
   { "n64ltp2",		JOY_TYPE_N64PAD_LPT2	       	},
   { "n643",		JOY_TYPE_N64PAD_LPT3		},
   { "n64ltp3",		JOY_TYPE_N64PAD_LPT3		},
#endif
#ifdef JOY_TYPE_DB9_LPT1
   { "db9lpt1",		JOY_TYPE_DB9_LPT1		},
   { "smslpt1",		JOY_TYPE_DB9_LPT1	       	},
   { "db9lpt2",		JOY_TYPE_DB9_LPT2		},
   { "smslpt2",		JOY_TYPE_DB9_LPT2		},
   { "db9lpt3",		JOY_TYPE_DB9_LPT3		},
   { "smslpt3",		JOY_TYPE_DB9_LPT3		},
#endif
#ifdef JOY_TYPE_TURBOGRAFX_LPT1
   { "nec1",		JOY_TYPE_TURBOGRAFX_LPT1	},
   { "neclpt1",		JOY_TYPE_TURBOGRAFX_LPT1	},
   { "nec2",		JOY_TYPE_TURBOGRAFX_LPT2	},
   { "neclpt2",		JOY_TYPE_TURBOGRAFX_LPT2	},
   { "nec3",		JOY_TYPE_TURBOGRAFX_LPT3	},
   { "neclpt3",		JOY_TYPE_TURBOGRAFX_LPT3	},
#endif
#ifdef JOY_TYPE_WINGWARRIOR
   { "wingwar",		JOY_TYPE_WINGWARRIOR		},
   { "wingmanwarrior", 	JOY_TYPE_WINGWARRIOR		},
#endif
#ifdef JOY_TYPE_IFSEGA_ISA
   { "ifsegaisa",      	JOY_TYPE_IFSEGA_ISA		},
#endif
#ifdef JOY_TYPE_IFSEGA_PCI
   { "ifsegapci",      	JOY_TYPE_IFSEGA_PCI		},
#endif
#ifdef JOY_TYPE_IFSEGA_PCI_FAST
   { "ifsegapci2",      JOY_TYPE_IFSEGA_PCI_FAST	},
   { "ifsegapcifast",      JOY_TYPE_IFSEGA_PCI_FAST	},
#endif
#ifdef JOY_TYPE_LINUX_ANALOGUE
   { "linux",		JOY_TYPE_LINUX_ANALOGUE		},
#endif
#ifdef JOY_TYPE_WIN32
   { "win",		JOY_TYPE_WIN32			},
#endif
  { NULL, 0 }
 };

//-----------------------------------------------------------------------------

int         Config_Driver_Joy_Str_to_Int (char *DriverName)
{
    int     i;

    for (i = 0; S2J_Table[i].name; i ++)
        if (stricmp (DriverName, S2J_Table [i].name) == 0)
            return (S2J_Table [i].value);

    return (JOY_TYPE_AUTODETECT);
}

char *      Config_Driver_Joy_Int_to_Str (int a)
{
    int     i;

    for (i = 0; S2J_Table [i].name; i ++)
        if (a == S2J_Table [i].value)
            return (S2J_Table [i].name);
    return "auto";
}

//-----------------------------------------------------------------------------

