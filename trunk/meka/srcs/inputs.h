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

// Input Sources Types --------------------------------------------------------
#define  INPUT_SRC_UNUSED               (0)
#define  INPUT_SRC_KEYBOARD             (1)
#define  INPUT_SRC_JOYPAD               (2) // Digital
#define  INPUT_SRC_MOUSE                (3)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#define  PLAYER_NO      (-1)
#define  PLAYER_1       (0)
#define  PLAYER_2       (1)
#define  PLAYER_MAX     (2)
//-----------------------------------------------------------------------------

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
#define  DIGITAL                        (1)
#define  EMULATE_DIGITAL                (DIGITAL << 1)
#define  ANALOG                         (4)
#define  EMULATE_ANALOG                 (ANALOG << 1)
//-----------------------------------------------------------------------------
typedef struct
{
        char   *Name;
        int     Result_Type;
}       t_input_peripheral_info;
extern  t_input_peripheral_info Inputs_Peripheral_Infos [INPUT_PERIPHERAL_MAX];
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

typedef struct
{
    int     scancode;
    int     ascii;
} t_key_press;

typedef struct
{
    byte    Type;               // Axis, Button, Wheel, etc..
    int     Idx;                // Index of Axis/Stick/Button/Wheel, etc..
    int     Res;                // Result, For buttons: 1 if pressed, for axis: contains value
} t_input_map;

typedef struct
{
    char *          Name;                       // Source name
    byte            Type;                       // 0: INPUT_SRC_xx / INPUT_SRC_UNUSED being theorically useless!
    byte            Enabled;                    // Enabled
    byte            Player;                     // PLAYER_1 or PLAYER_2
    byte            Connection_Port;            // Joypad Number, COM Port, etc.. (device & machine dependant)
    byte            Result_Type;                // Bit 0 set if Analog, else Digital, Bit 1 set if emulating other
    float           Analog_to_Digital_FallOff;  // Default: 0.8f
    byte            Connected_and_Ready;        // No/Yes
    int             Driver;                     // Driver (NOW UNUSED)
    t_input_map     Map[INPUT_MAP_MAX];
    int             Map_Counters[INPUT_MAP_MAX];
} t_input_src;

// FIXME: yet unused
typedef struct
{
    u8              x;
} t_peripheral_paddlecontrol;

typedef struct
{
    char            FileName [FILENAME_LEN];        // Path to the MEKA.INP file
    // Emulation
    byte            Peripheral [PLAYER_MAX];        // 2 inputs ports on emulated machines
    t_input_src **  Sources;
    int             Sources_Max;
    int             Sources_Joy_Driver;
    int             Keyboard_Enabled;               // Boolean. Set when SK-1100 enabled.
    u8              Paddle_X [PLAYER_MAX];
    char            SportPad_XY [PLAYER_MAX] [2];
    byte            SportPad_Latch [PLAYER_MAX];
    // Mouse
    int             MouseSpeed_X;                   // Mouse speed
    int             MouseSpeed_Y;                   //
    int             MouseMickeys_X;                 // Mouse movement in mickeys for this frame
    int             MouseMickeys_Y;                 //
    // Keyboard
    t_key_press     KeyPressed;                     // (Single!) current keypressed (this isn't very good, but...)
    // GUI
    int             Cabinet_Mode;                   // Boolean. Invert ESC and F10 (this is until inputs keys are fully configurable)
} t_inputs;

t_inputs        Inputs;

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
void    Inputs_Switch_SportPad      (void);
void    Inputs_Switch_TVOekaki      (void);

byte    Input_Port_DC               (void);
byte    Input_Port_DD               (void);

//-----------------------------------------------------------------------------

