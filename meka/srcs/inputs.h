//-----------------------------------------------------------------------------
// MEKA - inputs.h
// User Inputs & Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_mouse_cursor
{
	MEKA_MOUSE_CURSOR_NONE,
	MEKA_MOUSE_CURSOR_STANDARD,
	MEKA_MOUSE_CURSOR_LIGHT_PHASER,
	MEKA_MOUSE_CURSOR_SPORTS_PAD,
	MEKA_MOUSE_CURSOR_TV_OEKAKI,
	MEKA_MOUSE_CURSOR_WAIT,
};

// Note: casted to s8 in DB storage so make sure it fits.
// Note: input configuration does a +1%max so we need consecutive integers.
enum t_input_peripheral
{
	INPUT_JOYPAD			= 0,
	INPUT_LIGHTPHASER		= 1,
	INPUT_PADDLECONTROL		= 2,
	INPUT_SPORTSPAD			= 3,
	INPUT_GRAPHICBOARD		= 4,
	INPUT_GRAPHICBOARD_V2	= 5,
	INPUT_PERIPHERAL_MAX	= 6,
};

// Input Connection Possibilities ---------------------------------------------
//
//   Keyboard --> Joypad
//            --> Paddle Control (?)    TODO
//   Joypad   --> Joypad
//            --> Paddle Control (?)    TODO
//   Mouse    --> Light Phaser
//            --> Paddle Control
//            --> Sports Pad
//            --> TV Oekaki
//            --> Joypad (?)            TODO
//
//-----------------------------------------------------------------------------

// Input Sources Types
enum t_input_src_type
{
    INPUT_SRC_TYPE_KEYBOARD = 0,
    INPUT_SRC_TYPE_JOYPAD   = 1,    // Digital only (yet)
    INPUT_SRC_TYPE_MOUSE    = 2,
};

// Input Sources Flags
enum t_input_src_flags
{
    INPUT_SRC_FLAGS_DIGITAL         = 0x0001,
    INPUT_SRC_FLAGS_EMULATE_DIGITAL = 0x0002,
    INPUT_SRC_FLAGS_ANALOG          = 0x0004,
    INPUT_SRC_FLAGS_EMULATE_ANALOG  = 0x0008,
};

// Players Definitions
#define  PLAYER_NO      (-1)
#define  PLAYER_1       (0)
#define  PLAYER_2       (1)
#define  PLAYER_MAX     (2)

// Input Mapping Types
enum t_input_map_type
{
	INPUT_MAP_TYPE_KEY = 0,
	INPUT_MAP_TYPE_JOY_BUTTON = 1,
	INPUT_MAP_TYPE_JOY_AXIS = 2,
	INPUT_MAP_TYPE_MOUSE_BUTTON = 3,
	INPUT_MAP_TYPE_MOUSE_AXIS = 4,
};

#define INPUT_JOY_DEADZONE		(0.2f)			// -0.2f to 0.2f is neutral

struct t_input_peripheral_info
{
    const char*	name;
};
extern  const t_input_peripheral_info Inputs_Peripheral_Infos [INPUT_PERIPHERAL_MAX];

//-----------------------------------------------------------------------------

// Input Mapping Indexes ------------------------------------------------------
// Digital Devices (Keyboard/Joystick)
#define  INPUT_MAP_DIGITAL_UP           (0)
#define  INPUT_MAP_DIGITAL_DOWN         (1)
#define  INPUT_MAP_DIGITAL_LEFT         (2)
#define  INPUT_MAP_DIGITAL_RIGHT        (3)
// Analog Devices (Mouse)
#define  INPUT_MAP_ANALOG_AXIS_X        (0)
#define  INPUT_MAP_ANALOG_AXIS_Y        (1)
#define  INPUT_MAP_ANALOG_AXIS_X_REL    (2)
#define  INPUT_MAP_ANALOG_AXIS_Y_REL    (3)
// Various
#define  INPUT_MAP_BUTTON1              (4)
#define  INPUT_MAP_BUTTON2              (5)
#define  INPUT_MAP_PAUSE_START          (6)
#define  INPUT_MAP_RESET                (7)
// Max
#define  INPUT_MAP_MAX                  (8)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_key_press
{
    int     scancode;
    int     ascii;
};

struct t_input_map_entry
{
    t_input_map_type	type;						// Axis, Button, Wheel, etc..
    int					hw_index;					// Index of button/stick
	int					hw_axis;
	int					hw_direction;
    int					current_value;              // Result, For buttons: 1 if pressed, for axis: contains value
	int					pressed_counter;
};

struct t_input_src
{
    char *				name;                  
    int					flags;						// enum t_input_src_flags // FIXME-ENUM               
    t_input_src_type    type;
    bool                enabled;
    int                 player;						// PLAYER_1 or PLAYER_2

    int					Connection_Port;            // Joypad Number, COM Port, etc.. (device & machine dependant)
    float				Analog_to_Digital_FallOff;  // Default: 0.8f
    bool				Connected_and_Ready;        // No/Yes
    t_input_map_entry	Map[INPUT_MAP_MAX];
};

struct t_peripheral_graphic_board_v2
{
	u8 unknown;		  // 0xFD-0xFF
	u8 buttons;       // 3-bit
	u8 x, y;
	u8 read_index;
};

struct t_peripheral_paddle
{
	// FIXME
	u8 x;
};

struct t_peripheral_sportspad
{
	// FIXME
	u8 x, y;
	u8 latch;
};

struct t_inputs
{
    char            FileName[FILENAME_LEN];        // Path to the MEKA.INP file
	t_mouse_cursor	mouse_cursor;
    // Emulation
    t_input_peripheral	Peripheral[PLAYER_MAX];        // 2 inputs ports on emulated machines
    t_input_src **  Sources;
    int             Sources_Max;
    int             SK1100_Enabled;					// Boolean. Set when SK-1100 enabled.

	// Machine-side peripheral data
	t_peripheral_paddle				Paddle[PLAYER_MAX];
	t_peripheral_sportspad			SportsPad[PLAYER_MAX];
	t_peripheral_graphic_board_v2	GraphicBoardV2[PLAYER_MAX];

	// Keyboard
    t_list *        KeyPressedQueue;                // Queued keypresses
    
	// GUI
    bool            Cabinet_Mode;                   // Boolean. Invert ESC and F10 (this is until inputs keys are fully configurable)
};

extern t_inputs Inputs;

const char *    Inputs_Get_MapName(int Type, int MapIdx);
void            Inputs_Peripheral_Next(int Player);
int             Inputs_Peripheral_Result_Type(int Periph);
void            Inputs_Peripheral_Change_Update(void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Input_ROM_Change            (void);

void    Inputs_SetMouseCursor		(t_mouse_cursor mouse_cursor);

void    Inputs_Check_GUI            (bool sk1100_pressed);

void    Inputs_Switch_Current       (void);
void    Inputs_Switch_Joypad        (void);
void    Inputs_Switch_LightPhaser   (void);
void    Inputs_Switch_PaddleControl (void);
void    Inputs_Switch_SportsPad     (void);
void    Inputs_Switch_GraphicBoard  (void);
void    Inputs_Switch_GraphicBoardV2(void);

u8		Input_Port_DC               (void);
u8		Input_Port_DD               (void);

//-----------------------------------------------------------------------------

