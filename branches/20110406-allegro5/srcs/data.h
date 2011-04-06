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
#define DATA_FLAG_CA                     7        /* BMP  */
#define DATA_FLAG_CH                     8        /* BMP  */
#define DATA_FLAG_DE                     9        /* BMP  */
#define DATA_FLAG_EU                     10       /* BMP  */
#define DATA_FLAG_FR                     11       /* BMP  */
#define DATA_FLAG_HK                     12       /* BMP  */
#define DATA_FLAG_IT                     13       /* BMP  */
#define DATA_FLAG_JP                     14       /* BMP  */
#define DATA_FLAG_KR                     15       /* BMP  */
#define DATA_FLAG_NZ                     16       /* BMP  */
#define DATA_FLAG_PT                     17       /* BMP  */
#define DATA_FLAG_SP                     18       /* BMP  */
#define DATA_FLAG_SW                     19       /* BMP  */
#define DATA_FLAG_TW                     20       /* BMP  */
#define DATA_FLAG_UK                     21       /* BMP  */
#define DATA_FLAG_UNKNOWN                22       /* BMP  */
#define DATA_FLAG_US                     23       /* BMP  */
#define DATA_FONT_0                      24       /* ALLEGRO_FONT */
#define DATA_FONT_1                      25       /* ALLEGRO_FONT */
#define DATA_FONT_2                      26       /* ALLEGRO_FONT */
#define DATA_GFX_DRAGON                  27       /* BMP  */
#define DATA_GFX_GLASSES                 28       /* BMP  */
#define DATA_GFX_HEART1                  29       /* BMP  */
#define DATA_GFX_HEART2                  30       /* BMP  */
#define DATA_GFX_INPUTS                  31       /* BMP  */
#define DATA_GFX_JOYPAD                  32       /* BMP  */
#define DATA_GFX_KEYBOARD                33       /* BMP  */
#define DATA_GFX_LIGHTPHASER             34       /* BMP  */
#define DATA_GFX_PADDLECONTROL           35       /* BMP  */
#define DATA_GFX_SPORTSPAD               36       /* BMP  */
#define DATA_GFX_SUPERHEROPAD            37       /* BMP  */
#define DATA_GFX_TVOEKAKI                38       /* BMP  */
#define DATA_ICON_BAD                    39       /* BMP  */
#define DATA_ICON_BIOS                   40       /* BMP  */
#define DATA_ICON_HACK                   41       /* BMP  */
#define DATA_ICON_HOMEBREW               42       /* BMP  */
#define DATA_ICON_PROTO                  43       /* BMP  */
#define DATA_ICON_TRANS_JP               44       /* BMP  */
#define DATA_ICON_TRANS_JP_US            45       /* BMP  */
#define DATA_MACHINE_COLECO              46       /* BMP  */
#define DATA_MACHINE_SMS                 47       /* BMP  */
#define DATA_MACHINE_SMS_CART            48       /* BMP  */
#define DATA_MACHINE_SMS_LIGHT           49       /* BMP  */
#define DATA_ROM_COLECO                  50       /* DATA */
#define DATA_ROM_SF7000                  51       /* DATA */
#define DATA_ROM_SMS                     52       /* DATA */
#define DATA_ROM_SMS_J                   53       /* DATA */
#define DATA_COUNT                       54

//-----------------------------------------------------------------------------

// Flags id
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
#define FLAG_CA         (17)    // Canada
#define FLAG_TW         (18)	// Taiwan
#define FLAG_COUNT      (19)

//-----------------------------------------------------------------------------
// t_data_graphics Graphics
// Hold handler to all graphics data
//-----------------------------------------------------------------------------
typedef struct
{
    // Cursors
    struct
    {
        ALLEGRO_BITMAP *    Main;
        ALLEGRO_BITMAP *    Wait;
        ALLEGRO_BITMAP *    LightPhaser;
        ALLEGRO_BITMAP *    SportsPad;
        ALLEGRO_BITMAP *    TvOekaki;
    } Cursors;

    // Flags
    ALLEGRO_BITMAP *        Flags [FLAG_COUNT];

    // Icons
    struct
    {
        ALLEGRO_BITMAP *    BAD;
        ALLEGRO_BITMAP *    BIOS;
        ALLEGRO_BITMAP *    Hack;
        ALLEGRO_BITMAP *    HomeBrew;
        ALLEGRO_BITMAP *    Prototype;
        ALLEGRO_BITMAP *    Translation_JP;
        ALLEGRO_BITMAP *    Translation_JP_US;
    } Icons;

    // Inputs
    struct
    {
        ALLEGRO_BITMAP *    InputsBase;
        ALLEGRO_BITMAP *    Glasses;
        ALLEGRO_BITMAP *    Joypad;
        ALLEGRO_BITMAP *    LightPhaser;
        ALLEGRO_BITMAP *    PaddleControl;
        ALLEGRO_BITMAP *    SportsPad;
        ALLEGRO_BITMAP *    SuperHeroPad;
        ALLEGRO_BITMAP *    TvOekaki;
        ALLEGRO_BITMAP *    SK1100_Keyboard;
    } Inputs;

    // Machines
    struct
    {
        ALLEGRO_BITMAP *    MasterSystem;
        ALLEGRO_BITMAP *    MasterSystem_Cart;
        ALLEGRO_BITMAP *    MasterSystem_Light;
        ALLEGRO_BITMAP *    ColecoVision;
    } Machines;

    // Miscellaenous
    struct
    {
        ALLEGRO_BITMAP *    Dragon;
        ALLEGRO_BITMAP *    Heart1;
        ALLEGRO_BITMAP *    Heart2;
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
