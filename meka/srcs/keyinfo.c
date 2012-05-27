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
    { ALLEGRO_KEY_A,            "A",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_B,            "B",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_C,            "C",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_D,            "D",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_E,            "E",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_F,            "F",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_G,            "G",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_H,            "H",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_I,            "I",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_J,            "J",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_K,            "K",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_L,            "L",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_M,            "M",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_N,            "N",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_O,            "O",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_P,            "P",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_Q,            "Q",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_R,            "R",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_S,            "S",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_T,            "T",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_U,            "U",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_V,            "V",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_W,            "W",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_X,            "X",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_Y,            "Y",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_Z,            "Z",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { ALLEGRO_KEY_0,            "0",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_1,            "1",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_2,            "2",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_3,            "3",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_4,            "4",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_5,            "5",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_6,            "6",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_7,            "7",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_8,            "8",             KEY_INFO_PRINTABLE                          },
    { ALLEGRO_KEY_9,            "9",             KEY_INFO_PRINTABLE                          },

    { ALLEGRO_KEY_PAD_0,        "0 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_1,        "1 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_2,        "2 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_3,        "3 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_4,        "4 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_5,        "5 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_6,        "6 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_7,        "7 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_8,        "8 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_9,        "9 (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { ALLEGRO_KEY_F1,           "F1",            0                                           },
    { ALLEGRO_KEY_F2,           "F2",            0                                           },
    { ALLEGRO_KEY_F3,           "F3",            0                                           },
    { ALLEGRO_KEY_F4,           "F4",            0                                           },
    { ALLEGRO_KEY_F5,           "F5",            0                                           },
    { ALLEGRO_KEY_F6,           "F6",            0                                           },
    { ALLEGRO_KEY_F7,           "F7",            0                                           },
    { ALLEGRO_KEY_F8,           "F8",            0                                           },
    { ALLEGRO_KEY_F9,           "F9",            0                                           },
    { ALLEGRO_KEY_F10,          "F10",           0                                           },
    { ALLEGRO_KEY_F11,          "F11",           0                                           },
    { ALLEGRO_KEY_F12,          "F12",           0                                           },

    { ALLEGRO_KEY_ESCAPE,       "Escape",        0                                           },
    { ALLEGRO_KEY_TILDE,        "Tilde",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_MINUS,        "Minus",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_EQUALS,       "Equals",        KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_BACKSPACE,    "Backspace",                          KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_TAB,          "Tab",                                KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_OPENBRACE,    "[",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_CLOSEBRACE,   "]",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_ENTER,        "Enter",                              KEY_INFO_ALLOW_USE     },
    //{ ALLEGRO_KEY_COLON,        ";",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // //??Semi Colon (;), not Colon (:)
    { ALLEGRO_KEY_SEMICOLON,	"Semicolon",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_QUOTE,        "'",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Quote ('), not Double Quote (")
    { ALLEGRO_KEY_BACKSLASH,    "\\",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_BACKSLASH2,   "\\",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_COMMA,        ",",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_FULLSTOP,     ".",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // Stop is .
    { ALLEGRO_KEY_SLASH,        "/",             KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_SPACE,        "Space",         KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_INSERT,       "Insert",                             KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_DELETE,       "Delete",                             KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_HOME,         "Home",                               KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_END,          "End",                                KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PGUP,         "Page Up",                            KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PGDN,         "Page Down",                          KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_LEFT,         "Left",                               KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_RIGHT,        "Right",                              KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_UP,           "Up",                                 KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_DOWN,         "Down",                               KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_SLASH,    "/ (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_ASTERISK, "* (Pad)",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },

    { ALLEGRO_KEY_PAD_MINUS,    "Minus (Pad)",   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_PLUS,     "Plus (Pad)",    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_DELETE,   ". (Pad)",							  KEY_INFO_ALLOW_USE     },
	{ 123,						". (Pad)",                            KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAD_ENTER,    "Enter (Pad)",                        KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PRINTSCREEN,  "Print Screen",                       KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_PAUSE,        "Pause",         0                                           },
    { ALLEGRO_KEY_ABNT_C1,      "ABNT_C1",                            KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_YEN,          "Yen",                                KEY_INFO_ALLOW_USE     }, // Should be printable, if the font include the yen character
    { ALLEGRO_KEY_KANA,         "Kana",                               KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_CONVERT,      "Convert",                            KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_NOCONVERT,    "No Convert",                         KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_AT,           "At",            KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_CIRCUMFLEX,   "Circumflex",    KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_COLON2,       "Colon (2)",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_KANJI,        "Kanji",                              KEY_INFO_ALLOW_USE     },

    // Those are commented as 'MacOs' in Allegro's keyboard.h, yet I got KEY_SEMICOLON by pressing M on french keyboard...
    { ALLEGRO_KEY_PAD_EQUALS,   "Equal (Pad)",   KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_BACKQUOTE,    "Backquote",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_SEMICOLON2,   "Semicolon",     KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?
    { ALLEGRO_KEY_COMMAND,      "Command",       KEY_INFO_PRINTABLE | KEY_INFO_ALLOW_USE     }, // ?

    { ALLEGRO_KEY_LSHIFT,       "Left Shift",                         KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_RSHIFT,       "Right Shift",                        KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_RCTRL,		"Right Control",                      KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_LCTRL,		"Left Control",                       KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_ALT,          "Alt",                                KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_ALTGR,        "Alt GR",                             KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_LWIN,         "Left Win",                           KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_RWIN,         "Right Win",                          KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_MENU,         "Menu Win",                           KEY_INFO_ALLOW_USE     },
    { ALLEGRO_KEY_SCROLLLOCK,   "Scroll Lock",   0                                           },
    { ALLEGRO_KEY_NUMLOCK,      "Num Lock",      0                                           },
    { ALLEGRO_KEY_CAPSLOCK,     "Caps Lock",                          KEY_INFO_ALLOW_USE     },

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

