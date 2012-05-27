//-----------------------------------------------------------------------------
// MEKA - vdp.h
// TMS9918/28 Accesses and Registers Handling - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// VDP Models (for accuracy, MEKA will head towards emulation of the
// specificities between the different VDP models)
// Values below are flags for the purpose of testing VDP for a certain
// feature more easily (like if (VDP_Model & (VDP_MODEL.. | VDP_MODEL..)))
#define VDP_MODEL_315_5124      (0x01)      // Mark III, Master System
#define VDP_MODEL_315_5226      (0x02)      // Later Master System, Master System II
#define VDP_MODEL_315_5378      (0x04)      // Game Gear
#define VDP_MODEL_315_5313      (0x08)      // Mega Drive

// VDP Video Change Flags
#define VDP_VIDEO_CHANGE_MODE   (0x01)
#define VDP_VIDEO_CHANGE_SIZE   (0x02)
#define VDP_VIDEO_CHANGE_ALL    (VDP_VIDEO_CHANGE_MODE | VDP_VIDEO_CHANGE_SIZE)

// FIXME: Latch. Remove/rename those definitions.
#define VDP_Access_Mode_1       (0)
#define VDP_Access_Mode_2       (1)

//-----------------------------------------------------------------------------
// VDP Registers Definitions
//-----------------------------------------------------------------------------
// FIXME: Rename below name/macro, use per-bit definitions/mask matching 
// naming in known/official documentations
//-----------------------------------------------------------------------------
// Unknown - 0          (sms.VDP[0] & 0x01) // "External Video Input"
// Unknown - 1          (sms.VDP[0] & 0x02) // Mode bit 0
// Unknown - 1          (sms.VDP[0] & 0x04) // Mode bit 1
#define Sprites_Left_8  (sms.VDP[0] & 0x08)
#define HBlank_ON       (sms.VDP[0] & 0x10)
#define Mask_Left_8     (sms.VDP[0] & 0x20)
#define Top_No_Scroll   (sms.VDP[0] & 0x40)
#define Right_No_Scroll (sms.VDP[0] & 0x80)
#define Sprites_Double  (sms.VDP[1] & 0x01)
#define Sprites_8x16    (sms.VDP[1] & 0x02)
#define Sprites_16x16   (sms.VDP[1] & 0x02)
#define Sprites_32x32   (sms.VDP[1] & 0x02)
// Unknown - 0          (sms.VDP[1] & 0x04)
// Unknown - 0          (sms.VDP[1] & 0x08) // Mode bit 2
#define Wide_Screen_28  (sms.VDP[1] & 0x10) // Also Mode bit 3
#define VBlank_ON       (sms.VDP[1] & 0x20)
#define Display_ON      (sms.VDP[1] & 0x40)
// Unknown - 1          (sms.VDP[1] & 0x80) // "VRAM 16 kbyte"
//-----------------------------------------------------------------------------
#define VDP_STATUS_SpriteCollision  (0x20)
#define VDP_STATUS_9thSprite        (0x40)
#define VDP_STATUS_VBlank           (0x80)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

int     VDP_Model_FindByName    (const char *name);

void    VDP_VideoMode_Change    (void);
void    VDP_VideoMode_Update    (void);
void	VDP_UpdateLineLimits    (void);

void    Tms_VDP_Out             (int vdp_register, int value);
void    Tms_VDP_Out_Data        (int value);
void    Tms_VDP_Out_Address     (int value);
u8      Tms_VDP_In_Status       (void);
u8      Tms_VDP_In_Data         (void);
void    Tms_VDP_Palette_Write   (int addr, int value);

//-----------------------------------------------------------------------------

