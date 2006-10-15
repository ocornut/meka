//-----------------------------------------------------------------------------
// MEKA - keysname.c
// Keyboard Keys Names - Code
//-----------------------------------------------------------------------------
// FIXME: may want to use hash tables to retrieve keys by scancode/name
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_key_info *    KeyInfo_FindByScancode(int scancode)
{
    t_key_info *ki = &KeyInfo_Table[0];
    while (ki->scancode != -1)
    {
        if (ki->scancode == scancode)
            return (ki);
        ki++;
    }
    return (NULL);
}

t_key_info *    KeyInfo_FindByName(char *name)
{
    t_key_info *ki = &KeyInfo_Table[0];
    while (ki->scancode != -1)
    {
        if (stricmp(ki->name, name) == 0)
            return (ki);
        ki++;
    }
    return (NULL);
}

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_key_info  KeyInfo_Table [] =
 {
   { KEY_A,            "A",             'a',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_B,            "B",             'b',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_C,            "C",             'c',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_D,            "D",             'd',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_E,            "E",             'e',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_F,            "F",             'f',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_G,            "G",             'g',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_H,            "H",             'h',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_I,            "I",             'i',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_J,            "J",             'j',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_K,            "K",             'k',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_L,            "L",             'l',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_M,            "M",             'm',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_N,            "N",             'n',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_O,            "O",             'o',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_P,            "P",             'p',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_Q,            "Q",             'q',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_R,            "R",             'r',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_S,            "S",             's',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_T,            "T",             't',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_U,            "U",             'u',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_V,            "V",             'v',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_W,            "W",             'w',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_X,            "X",             'x',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_Y,            "Y",             'y',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_Z,            "Z",             'z',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

   { KEY_0,            "0",             '0',    KEY_INFO_PRINTABLE                          },
   { KEY_1,            "1",             '1',    KEY_INFO_PRINTABLE                          },
   { KEY_2,            "2",             '2',    KEY_INFO_PRINTABLE                          },
   { KEY_3,            "3",             '3',    KEY_INFO_PRINTABLE                          },
   { KEY_4,            "4",             '4',    KEY_INFO_PRINTABLE                          },
   { KEY_5,            "5",             '5',    KEY_INFO_PRINTABLE                          },
   { KEY_6,            "6",             '6',    KEY_INFO_PRINTABLE                          },
   { KEY_7,            "7",             '7',    KEY_INFO_PRINTABLE                          },
   { KEY_8,            "8",             '8',    KEY_INFO_PRINTABLE                          },
   { KEY_9,            "9",             '9',    KEY_INFO_PRINTABLE                          },

   { KEY_0_PAD,        "0 (Pad)",       '0',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_1_PAD,        "1 (Pad)",       '1',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_2_PAD,        "2 (Pad)",       '2',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_3_PAD,        "3 (Pad)",       '3',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_4_PAD,        "4 (Pad)",       '4',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_5_PAD,        "5 (Pad)",       '5',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_6_PAD,        "6 (Pad)",       '6',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_7_PAD,        "7 (Pad)",       '7',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_8_PAD,        "8 (Pad)",       '8',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_9_PAD,        "9 (Pad)",       '9',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

   { KEY_F1,           "F1",            0,      0                                           },
   { KEY_F2,           "F2",            0,      0                                           },
   { KEY_F3,           "F3",            0,      0                                           },
   { KEY_F4,           "F4",            0,      0                                           },
   { KEY_F5,           "F5",            0,      0                                           },
   { KEY_F6,           "F6",            0,      0                                           },
   { KEY_F7,           "F7",            0,      0                                           },
   { KEY_F8,           "F8",            0,      0                                           },
   { KEY_F9,           "F9",            0,      0                                           },
   { KEY_F10,          "F10",           0,      0                                           },
   { KEY_F11,          "F11",           0,      0                                           },
   { KEY_F12,          "F12",           0,      0                                           },

   { KEY_ESC,          "Escape",        0,      0                                           },
   { KEY_TILDE,        "Tilde",         '~',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_MINUS,        "Minus",         '-',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_EQUALS,       "Equals",        '=',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_BACKSPACE,    "Backspace",     0,                           KEY_INFO_ALLOW_USE     },
   { KEY_TAB,          "Tab",           0,                           KEY_INFO_ALLOW_USE     },
   { KEY_OPENBRACE,    "[",             '[',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_CLOSEBRACE,   "]",             ']',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_ENTER,        "Enter",         0,                           KEY_INFO_ALLOW_USE     },
   { KEY_COLON,        ":",             ':',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Semi Colon (;), not Colon (:)
   //{ KEY_COLON,      "\\;",           ':',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Backslashed version, too. There's a hack to handle it in INPUTS_F.C
   { KEY_QUOTE,        "'",             '\'',   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Quote ('), not Double Quote (")
   { KEY_BACKSLASH,    "\\",            '\\',   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_BACKSLASH2,   "\\",            '\\',   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_COMMA,        ",",             ',',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_STOP,         ".",             '.',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Stop is .
   { KEY_SLASH,        "/",             '/',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_SPACE,        "Space",         ' ',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_INSERT,       "Insert",        0,                           KEY_INFO_ALLOW_USE     },
   { KEY_DEL,          "Delete",        0,                           KEY_INFO_ALLOW_USE     },
   { KEY_HOME,         "Home",          0,                           KEY_INFO_ALLOW_USE     },
   { KEY_END,          "End",           0,                           KEY_INFO_ALLOW_USE     },
   { KEY_PGUP,         "Page Up",       0,                           KEY_INFO_ALLOW_USE     },
   { KEY_PGDN,         "Page Down",     0,                           KEY_INFO_ALLOW_USE     },
   { KEY_LEFT,         "Left",          0,                           KEY_INFO_ALLOW_USE     },
   { KEY_RIGHT,        "Right",         0,                           KEY_INFO_ALLOW_USE     },
   { KEY_UP,           "Up",            0,                           KEY_INFO_ALLOW_USE     },
   { KEY_DOWN,         "Down",          0,                           KEY_INFO_ALLOW_USE     },
   { KEY_SLASH_PAD,    "/ (Pad)",       '/',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_ASTERISK,     "*",             '*',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

   { KEY_MINUS_PAD,    "Minus (Pad)",   '-',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_PLUS_PAD,     "Plus (Pad)",    '+',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_DEL_PAD,      "Delete (Pad)",  0,                           KEY_INFO_ALLOW_USE     },
   { KEY_ENTER_PAD,    "Enter (Pad)",   0,                           KEY_INFO_ALLOW_USE     },
   { KEY_PRTSCR,       "Print Screen",  0,                           KEY_INFO_ALLOW_USE     },
   { KEY_PAUSE,        "Pause",         0,      0                                           },
   { KEY_ABNT_C1,      "ABNT_C1",       0,                           KEY_INFO_ALLOW_USE     }, // ?
   { KEY_YEN,          "Yen",           0,                           KEY_INFO_ALLOW_USE     }, // Should be printable, if the font include the yen character
   { KEY_KANA,         "Kana",          0,                           KEY_INFO_ALLOW_USE     },
   { KEY_CONVERT,      "Convert",       0,                           KEY_INFO_ALLOW_USE     },
   { KEY_NOCONVERT,    "No Convert",    0,                           KEY_INFO_ALLOW_USE     },
   { KEY_AT,           "At",            '@',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_CIRCUMFLEX,   "Circumflex",    '^',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
   { KEY_COLON2,       "Colon (2)",     ':',    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
   { KEY_KANJI,        "Kanji",         0,                           KEY_INFO_ALLOW_USE     },

   { KEY_LSHIFT,       "Left Shift",    0,                           KEY_INFO_ALLOW_USE     },
   { KEY_RSHIFT,       "Right Shift",   0,                           KEY_INFO_ALLOW_USE     },
   { KEY_RCONTROL,     "Right Control", 0,                           KEY_INFO_ALLOW_USE     },
   { KEY_LCONTROL,     "Left Control",  0,                           KEY_INFO_ALLOW_USE     },
   { KEY_ALT,          "Alt",           0,                           KEY_INFO_ALLOW_USE     },
   { KEY_ALTGR,        "Alt GR",        0,                           KEY_INFO_ALLOW_USE     },
   { KEY_LWIN,         "Left Win",      0,                           KEY_INFO_ALLOW_USE     },
   { KEY_RWIN,         "Right Win",     0,                           KEY_INFO_ALLOW_USE     },
   { KEY_MENU,         "Menu Win",      0,                           KEY_INFO_ALLOW_USE     },
   { KEY_SCRLOCK,      "Scroll Lock",   0,      0                                           },
   { KEY_NUMLOCK,      "Num Lock",      0,      0                                           },
   { KEY_CAPSLOCK,     "Caps Lock",     0,                           KEY_INFO_ALLOW_USE     },

   { -1, NULL, 0, 0 }
 };

//-----------------------------------------------------------------------------

