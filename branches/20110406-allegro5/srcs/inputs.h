//-----------------------------------------------------------------------------
// MEKA - inputs.h
// User Inputs & Emulation - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Peripherals
#define  INPUT_JOYPAD           (0)
#define  INPUT_LIGHTPHASER      (1)
#define  INPUT_PADDLECONTROL    (2)
#define  INPUT_SPORTSPAD        (3)
#define  INPUT_TVOEKAKI         (4)
#define  INPUT_PERIPHERAL_MAX   (5)

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

// Input Mapping Types --------------------------------------------------------
#define  INPUT_MAP_TYPE_KEY             (0)
//-----------------------------------------------------------------------------
#define  INPUT_MAP_TYPE_JOY_BUTTON      (0)
#define  INPUT_MAP_TYPE_JOY_AXIS        (1)
#define  INPUT_MAP_TYPE_JOY_AXIS_ANAL   (2)
//-----------------------------------------------------------------------------
#define  INPUT_MAP_TYPE_MOUSE_BUTTON    (0)
#define  INPUT_MAP_TYPE_MOUSE_AXIS      (1)
//-----------------------------------------------------------------------------

struct t_input_peripheral_info
{
    char   *name;
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

// Axis Index Coding ----------------------------------------------------------
// Joypad: 00000000.0000000d.ssssssss.aaaaaaaa (Direction, Stick, Axis)
// Mouse:  00000000.00000000.00000000.aaaaaaaa (Axis)
#define      INPUT_MAP_GET_AXIS(m)         (m & 0x0000FF)
#define      INPUT_MAP_GET_STICK(m)        ((m & 0x00FF00) >> 8)
#define      INPUT_MAP_GET_DIR_LR(m)       (m & 0x010000)
#define      MAKE_AXIS(a)                  (a)
#define      MAKE_STICK_AXIS_DIR(s,a,d)    ((a & 0xFF) | ((s & 0xFF) << 8) | ((d & 0x01) << 16))
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_key_press
{
    int     scancode;
    int     ascii;
};

struct t_input_map
{
    byte    Type;               // Axis, Button, Wheel, etc..
    int     Idx;                // Index of Axis/Stick/Button/Wheel, etc..
    int     Res;                // Result, For buttons: 1 if pressed, for axis: contains value
};

struct t_input_src
{
    char *              name;                  
    int					flags;					// enum t_input_src_flags // FIXME-ENUM               
    t_input_src_type    type;
    bool                enabled;
    int                 player;                 // PLAYER_1 or PLAYER_2

    byte            Connection_Port;            // Joypad Number, COM Port, etc.. (device & machine dependant)
    float           Analog_to_Digital_FallOff;  // Default: 0.8f
    byte            Connected_and_Ready;        // No/Yes
    t_input_map     Map[INPUT_MAP_MAX];
    int             Map_Counters[INPUT_MAP_MAX];
};

// FIXME: yet unused
struct t_peripheral_paddlecontrol
{
    u8              x;
};

struct t_inputs
{
    char            FileName [FILENAME_LEN];        // Path to the MEKA.INP file
    // Emulation
    byte            Peripheral [PLAYER_MAX];        // 2 inputs ports on emulated machines
    t_input_src **  Sources;
    int             Sources_Max;
    int             Keyboard_Enabled;               // Boolean. Set when SK-1100 enabled.
    u8              Paddle_X [PLAYER_MAX];
    char            SportsPad_XY [PLAYER_MAX] [2];
    u8              SportsPad_Latch [PLAYER_MAX];
    // Mouse
    int             MouseSpeed_X;                   // Mouse speed
    int             MouseSpeed_Y;                   //
   // Keyboard
    t_list *        KeyPressedQueue;                // Queued keypresses
    // GUI
    int             Cabinet_Mode;                   // Boolean. Invert ESC and F10 (this is until inputs keys are fully configurable)
};

extern t_inputs Inputs;

char *          Inputs_Get_MapName (int Type, int MapIdx);
void            Inputs_Peripheral_Next (int Player);
int             Inputs_Peripheral_Result_Type (int Periph);
void            Inputs_Peripheral_Change_Update (void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Input_ROM_Change            (void);

void    Inputs_Check_GUI            (bool sk1100_pressed);

void    Inputs_Switch_Current       (void);
void    Inputs_Switch_Joypad        (void);
void    Inputs_Switch_LightPhaser   (void);
void    Inputs_Switch_PaddleControl (void);
void    Inputs_Switch_SportsPad     (void);
void    Inputs_Switch_TVOekaki      (void);

byte    Input_Port_DC               (void);
byte    Input_Port_DD               (void);

//-----------------------------------------------------------------------------

