//-----------------------------------------------------------------------------
// MEKA - g_apps.c
// GUI Applets (some) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_applets (void);                     // create defaults applets
void    gui_applet_voice_rec (void);            // update the voice rec. applet

void    gui_applet_palette_configure (int size);
void    gui_applet_palette_update (void);              // update palette applet

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct  voice_type
{
  byte  Dir;
  byte  Value;
  byte  Old_Value;
  int   Delay;
};

struct  type_apps_opt
{
  struct voice_type Voice;
  int    Blood_Current_Drop;
  int    Palette_Size;
};

struct  type_apps_n
{
  byte  Palette, Tiles, Voice_Rec, Tech, About;
  byte  FM_Editor;
};

struct type_apps_bitmap
{
  BITMAP *Palette, *Tiles, *Voice_Rec, *Tech, *About;
  BITMAP *FM_Editor;
};

struct type_apps
{
  struct type_apps_n       active;
  struct type_apps_bitmap  gfx;
  struct type_apps_n       id;
  struct type_apps_opt     opt;
  int                      id_game;
} apps;

//-----------------------------------------------------------------------------

