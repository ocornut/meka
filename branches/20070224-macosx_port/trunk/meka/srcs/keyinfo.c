//-----------------------------------------------------------------------------
// MEKA - keysname.c
// Keyboard Keys Names - Code
//-----------------------------------------------------------------------------
// FIXME: may want to use hash tables to retrieve keys by scancode/name
//-----------------------------------------------------------------------------

#include "shared.h"
#include "keyinfo.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static const t_key_info  KeyInfo_Table [] =
{
    { KEY_A,            "A",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_B,            "B",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_C,            "C",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_D,            "D",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_E,            "E",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_F,            "F",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_G,            "G",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_H,            "H",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_I,            "I",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_J,            "J",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_K,            "K",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_L,            "L",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_M,            "M",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_N,            "N",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_O,            "O",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_P,            "P",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_Q,            "Q",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_R,            "R",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_S,            "S",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_T,            "T",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_U,            "U",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_V,            "V",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_W,            "W",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_X,            "X",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_Y,            "Y",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_Z,            "Z",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { KEY_0,            "0",             KEY_INFO_PRINTABLE                          },
    { KEY_1,            "1",             KEY_INFO_PRINTABLE                          },
    { KEY_2,            "2",             KEY_INFO_PRINTABLE                          },
    { KEY_3,            "3",             KEY_INFO_PRINTABLE                          },
    { KEY_4,            "4",             KEY_INFO_PRINTABLE                          },
    { KEY_5,            "5",             KEY_INFO_PRINTABLE                          },
    { KEY_6,            "6",             KEY_INFO_PRINTABLE                          },
    { KEY_7,            "7",             KEY_INFO_PRINTABLE                          },
    { KEY_8,            "8",             KEY_INFO_PRINTABLE                          },
    { KEY_9,            "9",             KEY_INFO_PRINTABLE                          },

    { KEY_0_PAD,        "0 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_1_PAD,        "1 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_2_PAD,        "2 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_3_PAD,        "3 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_4_PAD,        "4 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_5_PAD,        "5 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_6_PAD,        "6 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_7_PAD,        "7 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_8_PAD,        "8 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_9_PAD,        "9 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { KEY_F1,           "F1",            0                                           },
    { KEY_F2,           "F2",            0                                           },
    { KEY_F3,           "F3",            0                                           },
    { KEY_F4,           "F4",            0                                           },
    { KEY_F5,           "F5",            0                                           },
    { KEY_F6,           "F6",            0                                           },
    { KEY_F7,           "F7",            0                                           },
    { KEY_F8,           "F8",            0                                           },
    { KEY_F9,           "F9",            0                                           },
    { KEY_F10,          "F10",           0                                           },
    { KEY_F11,          "F11",           0                                           },
    { KEY_F12,          "F12",           0                                           },

    { KEY_ESC,          "Escape",        0                                           },
    { KEY_TILDE,        "Tilde",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_MINUS,        "Minus",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_EQUALS,       "Equals",        KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_BACKSPACE,    "Backspace",                          KEY_INFO_ALLOW_USE     },
    { KEY_TAB,          "Tab",                                KEY_INFO_ALLOW_USE     },
    { KEY_OPENBRACE,    "[",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_CLOSEBRACE,   "]",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_ENTER,        "Enter",                              KEY_INFO_ALLOW_USE     },
    { KEY_COLON,        ";",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // //??Semi Colon (;), not Colon (:)
    { KEY_QUOTE,        "'",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Quote ('), not Double Quote (")
    { KEY_BACKSLASH,    "\\",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_BACKSLASH2,   "\\",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_COMMA,        ",",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_STOP,         ".",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Stop is .
    { KEY_SLASH,        "/",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_SPACE,        "Space",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_INSERT,       "Insert",                             KEY_INFO_ALLOW_USE     },
    { KEY_DEL,          "Delete",                             KEY_INFO_ALLOW_USE     },
    { KEY_HOME,         "Home",                               KEY_INFO_ALLOW_USE     },
    { KEY_END,          "End",                                KEY_INFO_ALLOW_USE     },
    { KEY_PGUP,         "Page Up",                            KEY_INFO_ALLOW_USE     },
    { KEY_PGDN,         "Page Down",                          KEY_INFO_ALLOW_USE     },
    { KEY_LEFT,         "Left",                               KEY_INFO_ALLOW_USE     },
    { KEY_RIGHT,        "Right",                              KEY_INFO_ALLOW_USE     },
    { KEY_UP,           "Up",                                 KEY_INFO_ALLOW_USE     },
    { KEY_DOWN,         "Down",                               KEY_INFO_ALLOW_USE     },
    { KEY_SLASH_PAD,    "/ (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_ASTERISK,     "*",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { KEY_MINUS_PAD,    "Minus (Pad)",   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_PLUS_PAD,     "Plus (Pad)",    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_DEL_PAD,      "Delete (Pad)",                       KEY_INFO_ALLOW_USE     },
    { KEY_ENTER_PAD,    "Enter (Pad)",                        KEY_INFO_ALLOW_USE     },
    { KEY_PRTSCR,       "Print Screen",                       KEY_INFO_ALLOW_USE     },
    { KEY_PAUSE,        "Pause",         0                                           },
    { KEY_ABNT_C1,      "ABNT_C1",                            KEY_INFO_ALLOW_USE     }, // ?
    { KEY_YEN,          "Yen",                                KEY_INFO_ALLOW_USE     }, // Should be printable, if the font include the yen character
    { KEY_KANA,         "Kana",                               KEY_INFO_ALLOW_USE     },
    { KEY_CONVERT,      "Convert",                            KEY_INFO_ALLOW_USE     },
    { KEY_NOCONVERT,    "No Convert",                         KEY_INFO_ALLOW_USE     },
    { KEY_AT,           "At",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_CIRCUMFLEX,   "Circumflex",    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { KEY_COLON2,       "Colon (2)",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { KEY_KANJI,        "Kanji",                              KEY_INFO_ALLOW_USE     },

    // Those are commented as 'MacOs' in Allegro's keyboard.h, yet I got KEY_SEMICOLON by pressing M on french keyboard...
    { KEY_EQUALS_PAD,   "Equal (Pad)",   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { KEY_BACKQUOTE,    "Backquote",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { KEY_SEMICOLON,    "Semicolon",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { KEY_COMMAND,      "Command",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?

    { KEY_LSHIFT,       "Left Shift",                         KEY_INFO_ALLOW_USE     },
    { KEY_RSHIFT,       "Right Shift",                        KEY_INFO_ALLOW_USE     },
    { KEY_RCONTROL,     "Right Control",                      KEY_INFO_ALLOW_USE     },
    { KEY_LCONTROL,     "Left Control",                       KEY_INFO_ALLOW_USE     },
    { KEY_ALT,          "Alt",                                KEY_INFO_ALLOW_USE     },
    { KEY_ALTGR,        "Alt GR",                             KEY_INFO_ALLOW_USE     },
    { KEY_LWIN,         "Left Win",                           KEY_INFO_ALLOW_USE     },
    { KEY_RWIN,         "Right Win",                          KEY_INFO_ALLOW_USE     },
    { KEY_MENU,         "Menu Win",                           KEY_INFO_ALLOW_USE     },
    { KEY_SCRLOCK,      "Scroll Lock",   0                                           },
    { KEY_NUMLOCK,      "Num Lock",      0                                           },
    { KEY_CAPSLOCK,     "Caps Lock",                          KEY_INFO_ALLOW_USE     },

    { -1, NULL, 0 }
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

const t_key_info *    KeyInfo_FindByScancode(int scancode)
{
    const t_key_info *ki = &KeyInfo_Table[0];
    while (ki->scancode != -1)
    {
        if (ki->scancode == scancode)
            return (ki);
        ki++;
    }
    return (NULL);
}

const t_key_info *    KeyInfo_FindByName(const char *name)
{
    const t_key_info *ki = &KeyInfo_Table[0];
    while (ki->scancode != -1)
    {
        if (stricmp(ki->name, name) == 0)
            return (ki);
        ki++;
    }
    return (NULL);
}

//-----------------------------------------------------------------------------

