//-----------------------------------------------------------------------------
// MEKA - sk1100.c
// SK-1100 (Sega Keyboard) / SC-3000 Keyboard Emulation - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "inputs_t.h"
#include "sk1100.h"
#include "skin_bg.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_sk1100_key
{
    int     row;
    int     bit;
    char    desc[16];
};

#define SK1100_KEYS_NUM (62)

//-----------------------------------------------------------------------------

static t_sk1100_key SK1100_Keys [SK1100_KEYS_NUM] =
{
  // Row: 0 -------------------------------------------------------------------
  {  0, 0x0001, "1"              }, //  0
  {  0, 0x0002, "Q"              }, //  1
  {  0, 0x0004, "A"              }, //  2
  {  0, 0x0008, "Z"              }, //  3
  {  0, 0x0010, "Eng Dier's"     }, //  4
  {  0, 0x0020, ","              }, //  5
  {  0, 0x0040, "K"              }, //  6
  {  0, 0x0080, "I"              }, //  7
  {  0, 0x0100, "8"              }, //  8
  // Row: 1 -------------------------------------------------------------------
  {  1, 0x0001, "2"              }, //  9
  {  1, 0x0002, "W"              }, // 10
  {  1, 0x0004, "S"              }, // 11
  {  1, 0x0008, "X"              }, // 12
  {  1, 0x0010, "Space"          }, // 13
  {  1, 0x0020, "."              }, // 14
  {  1, 0x0040, "L"              }, // 15
  {  1, 0x0080, "O"              }, // 16
  {  1, 0x0100, "9"              }, // 17
  // Row: 2 -------------------------------------------------------------------
  {  2, 0x0001, "3"              }, // 18
  {  2, 0x0002, "E"              }, // 19
  {  2, 0x0004, "D"              }, // 20
  {  2, 0x0008, "C"              }, // 21
  {  2, 0x0010, "Home/Clear"     }, // 22
  {  2, 0x0020, "/"              }, // 23
  {  2, 0x0040, ";"              }, // 24
  {  2, 0x0080, "P"              }, // 25
  {  2, 0x0100, "0"              }, // 26
  // Row: 3 -------------------------------------------------------------------
  {  3, 0x0001, "4"              }, // 27
  {  3, 0x0002, "R"              }, // 28
  {  3, 0x0004, "F"              }, // 29
  {  3, 0x0008, "V"              }, // 30
  {  3, 0x0010, "Insert/Delete"  }, // 31
  {  3, 0x0020, "Pi"             }, // 32
  {  3, 0x0040, ":"              }, // 33
  {  3, 0x0080, "@"              }, // 34
  {  3, 0x0100, "-"              }, // 35
  // Row: 4 -------------------------------------------------------------------
  {  4, 0x0001, "5"              }, // 36
  {  4, 0x0002, "T"              }, // 37
  {  4, 0x0004, "G"              }, // 38
  {  4, 0x0008, "B"              }, // 39
  // 4, 0x0010, <UNUSED>
  {  4, 0x0020, "Down Arrow"     }, // 40
  {  4, 0x0040, "]"              }, // 41
  {  4, 0x0080, "["              }, // 42
  {  4, 0x0100, "^"              }, // 43
  // Row: 5 -------------------------------------------------------------------
  {  5, 0x0001, "6"              }, // 44
  {  5, 0x0002, "Y"              }, // 45
  {  5, 0x0004, "H"              }, // 46
  {  5, 0x0008, "N"              }, // 47
  // 5, 0x0010, <UNUSED>
  {  5, 0x0020, "Left Arrow"     }, // 48
  {  5, 0x0040, "Return"         }, // 49
  // 5, 0x0080, <UNUSED>
  {  5, 0x0100, "\x9d"              }, // 50
  // 5, 0x0200, <UNUSED>
  // 5, 0x0400, <UNUSED>
  {  5, 0x0800, "Func"           }, // 51
  // Row: 6 -------------------------------------------------------------------
  {  6, 0x0001, "7"              }, // 52
  {  6, 0x0002, "U"              }, // 53
  {  6, 0x0004, "J"              }, // 54
  {  6, 0x0008, "M"              }, // 55
  // 6, 0x0010, <UNUSED>
  {  6, 0x0020, "Right Arrow"    }, // 56
  {  6, 0x0040, "Up Arrow"       }, // 57
  // 6, 0x0080, <UNUSED>
  {  6, 0x0100, "Break"          }, // 58
  {  6, 0x0200, "Graph"          }, // 59
  {  6, 0x0400, "Ctrl"           }, // 60
  {  6, 0x0800, "Shift"          }  // 61
};

//-----------------------------------------------------------------------------

struct t_sk1100_map
{
    int   key_pc;
    int   key_sk1100;
};

#define SK1100_MAPPING_NUM (62+7)

static  t_sk1100_map SK1100_Mapping [SK1100_MAPPING_NUM] =
{
    // Row: 0 -------------------------------------------------------------------
    { ALLEGRO_KEY_1,               0      },
    { ALLEGRO_KEY_Q,               1      },
    { ALLEGRO_KEY_A,               2      },
    { ALLEGRO_KEY_Z,               3      },
    { ALLEGRO_KEY_ALT,             4      }, // Eng Dier's
    { ALLEGRO_KEY_COMMA,           5      },
    { ALLEGRO_KEY_K,               6      },
    { ALLEGRO_KEY_I,               7      },
    { ALLEGRO_KEY_8,               8      },
    // Row: 1 -------------------------------------------------------------------
    { ALLEGRO_KEY_2,               9      },
    { ALLEGRO_KEY_W,              10      },
    { ALLEGRO_KEY_S,              11      },
    { ALLEGRO_KEY_X,              12      },
    { ALLEGRO_KEY_SPACE,          13      },
    { ALLEGRO_KEY_FULLSTOP,       14      }, // .
    { ALLEGRO_KEY_L,              15      },
    { ALLEGRO_KEY_O,              16      },
    { ALLEGRO_KEY_9,              17      },
    // Row: 2 -------------------------------------------------------------------
    { ALLEGRO_KEY_3,              18      },
    { ALLEGRO_KEY_E,              19      },
    { ALLEGRO_KEY_D,              20      },
    { ALLEGRO_KEY_C,              21      },
    { ALLEGRO_KEY_ALTGR,          22      }, // Home Clear
    { ALLEGRO_KEY_SLASH,          23      },
    { ALLEGRO_KEY_SEMICOLON,      24      }, // ;
	{ ALLEGRO_KEY_SEMICOLON2,     24      }, // ; ++
	{ ALLEGRO_KEY_COLON2,         33      }, // :
    { ALLEGRO_KEY_P,              25      },
    { ALLEGRO_KEY_0,              26      },
    // Row: 3 -------------------------------------------------------------------
    { ALLEGRO_KEY_4,              27      },
    { ALLEGRO_KEY_R,              28      },
    { ALLEGRO_KEY_F,              29      },
    { ALLEGRO_KEY_V,              30      },
    { ALLEGRO_KEY_RCTRL,		  31      }, // Insert/Delete
    { ALLEGRO_KEY_BACKSPACE,      31      }, // Insert/Delete ++
    { ALLEGRO_KEY_ABNT_C1,        32      }, // Pi
    { ALLEGRO_KEY_PAD_SLASH,      32      }, // Pi ++
    { ALLEGRO_KEY_BACKSLASH2,     32      }, // Pi ++
    { ALLEGRO_KEY_QUOTE,          33      }, // :
    { ALLEGRO_KEY_OPENBRACE,      34,     }, // @
    { ALLEGRO_KEY_MINUS,          35      },
    // Row: 4 -------------------------------------------------------------------
    { ALLEGRO_KEY_5,              36      },
    { ALLEGRO_KEY_T,              37      },
    { ALLEGRO_KEY_G,              38      },
    { ALLEGRO_KEY_B,              39      },
    { ALLEGRO_KEY_DOWN,           40      }, // Down Arrow
    { ALLEGRO_KEY_BACKSLASH,      41      }, // ]
    { ALLEGRO_KEY_CLOSEBRACE,     42      }, // [
    { ALLEGRO_KEY_EQUALS,         43      }, // ^
    // Row: 5 -------------------------------------------------------------------
    { ALLEGRO_KEY_6,              44      },
    { ALLEGRO_KEY_Y,              45      },
    { ALLEGRO_KEY_H,              46      },
    { ALLEGRO_KEY_N,			  47      },
    { ALLEGRO_KEY_LEFT,           48      }, // Left Arrow
    { ALLEGRO_KEY_ENTER,          49      }, // Return
    { ALLEGRO_KEY_YEN,            50,     }, // Yen
    { ALLEGRO_KEY_TILDE,          50,     }, // Yen ++
    { ALLEGRO_KEY_TAB,            51      }, // Func
    // Row: 6 -------------------------------------------------------------------
    { ALLEGRO_KEY_7,              52      },
    { ALLEGRO_KEY_U,              53      },
    { ALLEGRO_KEY_J,              54      },
    { ALLEGRO_KEY_M,              55      },
    { ALLEGRO_KEY_RIGHT,          56      }, // Right Arrow
    { ALLEGRO_KEY_UP,             57      }, // Up Arrow
    { ALLEGRO_KEY_END,            58      }, // Break             (!!?)
    { ALLEGRO_KEY_LCTRL,          59      }, // Graph
    { ALLEGRO_KEY_CAPSLOCK,       60      }, // Ctrl
    { ALLEGRO_KEY_LSHIFT,         61      }, // Shift
    { ALLEGRO_KEY_RSHIFT,         61      }, // Shift ++
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Enable/disable SK-1100 emulation
void    SK1100_Switch()
{
    Inputs.SK1100_Enabled ^= 1;
    gui_menu_toggle_check (menus_ID.inputs, 7);
    Skins_Background_Redraw();
    gui.info.must_redraw = TRUE;
    if (Inputs.SK1100_Enabled)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_SK1100_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Inputs_SK1100_Disabled));
}

// Clear SK-1100 data for emulation
void    SK1100_Clear()
{
    int i;
    for (i = 0; i < 7; i ++)
        tsms.Control [i] = 0xFFFF;
}

// Update SK-1100 data for emulation
void    SK1100_Update()
{
    int i;
    SK1100_Clear();
    for (i = 0; i != SK1100_MAPPING_NUM; i ++)
    {
		const t_sk1100_map* k = &SK1100_Mapping[i];
        if (Inputs_KeyDown(k->key_pc))
        {
            const t_sk1100_key *sk1100_key = &SK1100_Keys[k->key_sk1100];
            tsms.Control [sk1100_key->row] &= (~sk1100_key->bit);
            tsms.Control_Check_GUI = FALSE;
			//Msg(MSGT_USER, "Pressed %d -> %d", k->key_pc, k->key_sk1100);
        }
    }
}

//-----------------------------------------------------------------------------
