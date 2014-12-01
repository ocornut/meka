//-----------------------------------------------------------------------------
// MEKA - data.h
// Data Loading - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
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
struct t_data_graphics
{
    // Cursors
    struct
    {
		struct
		{
			ALLEGRO_BITMAP *    Main;
			ALLEGRO_BITMAP *    Wait;
			ALLEGRO_BITMAP *    LightPhaser;
			ALLEGRO_BITMAP *    SportsPad;
			ALLEGRO_BITMAP *    TvOekaki;
		} Bitmaps;
		ALLEGRO_MOUSE_CURSOR *	Main;
		ALLEGRO_MOUSE_CURSOR *	Wait;
		ALLEGRO_MOUSE_CURSOR *	LightPhaser;
		ALLEGRO_MOUSE_CURSOR *	SportsPad;
		ALLEGRO_MOUSE_CURSOR *	TvOekaki;
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
		ALLEGRO_BITMAP *    GraphicBoardV2;
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

    // Miscellaneous
    struct
    {
        ALLEGRO_BITMAP *    Dragon;
        ALLEGRO_BITMAP *    Heart1;
        ALLEGRO_BITMAP *    Heart2;
    } Misc;

};

extern t_data_graphics     Graphics;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Data_Init                   (void);
void    Data_Close                  (void);

void    Data_CreateVideoBuffers     (void);
bool	Data_LoadFont(ALLEGRO_FONT** pfont, const char* name, int size);

//-----------------------------------------------------------------------------
