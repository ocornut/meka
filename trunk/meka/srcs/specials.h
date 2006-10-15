//-----------------------------------------------------------------------------
// MEKA - specials.h
// Special effects - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MAX_BLOOD_DROP          (300)
#define MAX_SNOW_FLAKES         (400)
#define MAX_HEARTS              (16)  // must be <= to MAX_BLOOD_DROP

#define SPECIAL_NOTHING         (0)
#define SPECIAL_BLOOD           (1)
#define SPECIAL_HEARTS          (2)
#define SPECIAL_SNOW            (3)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OLD ------------------------------------------------------------------------
void gui_applet_blood_create  (int v, int x, int y);

// NEW ------------------------------------------------------------------------
void special_effects_init               (void);
void special_effects_update_after       (void);
void special_effects_update_before      (void);
void special_effects_snow_init          (int i);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct  BLOOD_DROP
{
  int   v;    // 0: not active, 1->4: active
  int   x, y;
  int   save;
};

BITMAP *hearts_save [MAX_HEARTS];
struct BLOOD_DROP blood [MAX_BLOOD_DROP];
struct BLOOD_DROP snow [MAX_SNOW_FLAKES];

//-----------------------------------------------------------------------------

