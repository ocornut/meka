//-----------------------------------------------------------------------------
// MEKA - data.h
// Data Loading - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Index in MEKA.DAT
#define DATA_CURSOR_LIGHTPHASER          0        /* BMP  */
#define DATA_CURSOR_MAIN                 1        /* BMP  */
#define DATA_CURSOR_SPORTSPAD            2        /* BMP  */
#define DATA_CURSOR_TVOEKAKI             3        /* BMP  */
#define DATA_CURSOR_WAIT                 4        /* BMP  */
#define DATA_FLAG_AU                     5        /* BMP  */
#define DATA_FLAG_BR                     6        /* BMP  */
#define DATA_FLAG_CH                     7        /* BMP  */
#define DATA_FLAG_DE                     8        /* BMP  */
#define DATA_FLAG_EU                     9        /* BMP  */
#define DATA_FLAG_FR                     10       /* BMP  */
#define DATA_FLAG_HK                     11       /* BMP  */
#define DATA_FLAG_IT                     12       /* BMP  */
#define DATA_FLAG_JP                     13       /* BMP  */
#define DATA_FLAG_KR                     14       /* BMP  */
#define DATA_FLAG_NZ                     15       /* BMP  */
#define DATA_FLAG_PT                     16       /* BMP  */
#define DATA_FLAG_SP                     17       /* BMP  */
#define DATA_FLAG_SW                     18       /* BMP  */
#define DATA_FLAG_UK                     19       /* BMP  */
#define DATA_FLAG_UNKNOWN                20       /* BMP  */
#define DATA_FLAG_US                     21       /* BMP  */
#define DATA_FONT_0                      22       /* FONT */
#define DATA_FONT_1                      23       /* FONT */
#define DATA_FONT_2                      24       /* FONT */
#define DATA_GFX_DRAGON                  25       /* BMP  */
#define DATA_GFX_GLASSES                 26       /* BMP  */
#define DATA_GFX_HEART1                  27       /* BMP  */
#define DATA_GFX_HEART2                  28       /* BMP  */
#define DATA_GFX_INPUTS                  29       /* BMP  */
#define DATA_GFX_JOYPAD                  30       /* BMP  */
#define DATA_GFX_KEYBOARD                31       /* BMP  */
#define DATA_GFX_LIGHTPHASER             32       /* BMP  */
#define DATA_GFX_PADDLECONTROL           33       /* BMP  */
#define DATA_GFX_SPORTSPAD               34       /* BMP  */
#define DATA_GFX_SUPERHEROPAD            35       /* BMP  */
#define DATA_GFX_TVOEKAKI                36       /* BMP  */
#define DATA_ICON_BAD                    37       /* BMP  */
#define DATA_ICON_BIOS                   38       /* BMP  */
#define DATA_ICON_HACK                   39       /* BMP  */
#define DATA_ICON_HOMEBREW               40       /* BMP  */
#define DATA_ICON_PROTO                  41       /* BMP  */
#define DATA_ICON_TRANS_JP               42       /* BMP  */
#define DATA_ICON_TRANS_JP_US            43       /* BMP  */
#define DATA_MACHINE_COLECO              44       /* BMP  */
#define DATA_MACHINE_SMS                 45       /* BMP  */
#define DATA_MACHINE_SMS_CART            46       /* BMP  */
#define DATA_MACHINE_SMS_LIGHT           47       /* BMP  */
#define DATA_ROM_COLECO                  48       /* DATA */
#define DATA_ROM_SF7000                  49       /* DATA */
#define DATA_ROM_SMS                     50       /* DATA */
#define DATA_ROM_SMS_J                   51       /* DATA */
#define DATA_COUNT                       52

//-----------------------------------------------------------------------------

// Flags id
// Do NOT reorganize the flags as their value is saved in MEKA.VFN
// FIXME: the above statement sucks. VLFN should be reoverhauled just to 
// associate filename with DB entries
// FIXME: VLFN was now dropped, I think the statement above is not valid
// anymore.
#define FLAG_UNKNOWN    (0)
#define FLAG_AU         (1)     // Australia
#define FLAG_BR         (2)     // Brazil
#define FLAG_JP         (3)     // Japan
#define FLAG_KR         (4)     // Korea
#define FLAG_FR         (5)     // France
#define FLAG_US         (6)     // USA
#define FLAG_CH         (7)     // China
#define FLAG_HK         (8)     // Hong-Kong
#define FLAG_EU         (9)     // Europe
#define FLAG_PT         (10)    // Portugal
#define FLAG_DE         (11)    // Deutschland/Germany
#define FLAG_IT         (12)    // Italia
#define FLAG_SP         (13)    // Spain
#define FLAG_SW         (14)    // Sweden
#define FLAG_NZ         (15)    // New-Zealand
#define FLAG_UK         (16)    // United-Kingdom
#define FLAG_COUNT      (17)

//-----------------------------------------------------------------------------
// t_data_graphics Graphics
// Hold handler to all graphics data
//-----------------------------------------------------------------------------
typedef struct
{
    // Cursors
    struct
    {
        BITMAP *    Main;
        BITMAP *    Wait;
        BITMAP *    LightPhaser;
        BITMAP *    SportsPad;
        BITMAP *    TvOekaki;
    } Cursors;

    // Flags
    BITMAP *        Flags [FLAG_COUNT];

    // Icons
    struct
    {
        BITMAP *    BAD;
        BITMAP *    BIOS;
        BITMAP *    Hack;
        BITMAP *    HomeBrew;
        BITMAP *    Prototype;
        BITMAP *    Translation_JP;
        BITMAP *    Translation_JP_US;
    } Icons;

    // Inputs
    struct
    {
        BITMAP *    InputsBase;
        BITMAP *    Glasses;
        BITMAP *    Joypad;
        BITMAP *    LightPhaser;
        BITMAP *    PaddleControl;
        BITMAP *    SportsPad;
        BITMAP *    SuperHeroPad;
        BITMAP *    TvOekaki;
        BITMAP *    SK1100_Keyboard;
    } Inputs;

    // Machines
    struct
    {
        BITMAP *    MasterSystem;
        BITMAP *    MasterSystem_Cart;
        BITMAP *    MasterSystem_Light;
        BITMAP *    ColecoVision;
    } Machines;

    // Miscellaenous
    struct
    {
        BITMAP *    Dragon;
        BITMAP *    Heart1;
        BITMAP *    Heart2;
    } Misc;

} t_data_graphics;

t_data_graphics     Graphics;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Data_Init                   (void);
void    Data_Close                  (void);

void    Data_UpdateVideoMode        (void);

//-----------------------------------------------------------------------------
