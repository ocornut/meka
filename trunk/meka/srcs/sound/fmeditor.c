// Outdated
// - Based on OPL wrapper
// - Use widget 'id' (made obsolete)
// Could be worked out to be up to date, if someone has the motivation...

#include "shared.h"

#if 0

/*
struct type_apps_bitmap
{
  ALLEGRO_BITMAP *FM_Editor;
};

struct type_apps
{
    struct
    {
        bool FM_Editor;
    } active;
    struct type_apps_bitmap  gfx;
    struct
    {
        byte FM_Editor;
    } id;
} apps;
*/


/****************************************************************/
/*  FM voice editor version 0.01 for MEKA                       */
/*                               Programmed by Hiromitsu Shioya */
/****************************************************************/

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define  FM_EDITOR_SIZE_X    (320)
#define  FM_EDITOR_SIZE_Y    (224)
#define  FM_EDITOR_FONT      (F_MEDIUM)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static int  fontx, fonty;

struct t_fmeditor_app
{
  t_gui_box *   box;
  int           current_voice_number;
};

extern t_fmeditor_app  FM_Editor;

enum {
  SEL_FMNUM = 0,
  SEL_KSL,
  SEL_MUL,
  SEL_AR,
  SEL_SL,
  SEL_EG,
  SEL_DR,
  SEL_RR,
  SEL_TL,
  SEL_KSR,
  SEL_WV,
  SEL_FBC,

  SEL_ERROR
};

char *fmsheet[] = {
  "  Instrument #%2d (%s)",
  /*123456789012345678901234567890*/
  "                | OP1 | OP2 |",
  "  KSL           |  %2d |  %2d | Max 3",
  "  MULTIPLE      |  %2d |  %2d | Max 15",
  "  ATTACK        |  %2d |  %2d | Max 15",
  "  SUSTAIN LEVEL |  %2d |  %2d | Max 15",
  "  EG (sustain)  |  %2d |  %2d | 0/1",
  "  DECAY         |  %2d |  %2d | Max 15",
  "  RELEASE       |  %2d |  %2d | Max 15",
  "  TOTAL LEVEL   | %3d | %3d | Max 63",
  "  KSR           |  %2d |  %2d | 0/1",
  "  WAVE FORM     |  %2d |  %2d | 0/1/2/3",
  "  FEEDBACK/CON  |  %2d |  %2d | FB:Max7, CON:0/1",
  " Click L:+1 R:-1",
};

int param_limit[][2] = {
  {  0,  3 },  {  0,  3 },	/* KSL */
  {  0, 15 },  {  0, 15 },	/* MUL */
  {  0, 15 },  {  0, 15 },	/* AR */
  {  0, 15 },  {  0, 15 },	/* SL */
  {  0,  1 },  {  0,  1 },	/* EG */
  {  0, 15 },  {  0, 15 },	/* DR */
  {  0, 15 },  {  0, 15 },	/* RR */
  {  0, 63 },  {  0, 63 },	/* TL */
  {  0,  1 },  {  0,  1 },	/* KSR */
  {  0,  3 },  {  0,  3 },	/* WV */
  {  0,  7 },  {  0,  1 },	/* FB/CON */
};

extern FM_OPL_Patch     FM_OPL_Patchs[YM2413_INSTRUMENTS];
extern int              fmVol[YM2413_VOLUME_STEPS];
extern int              vcref[9];

/************************************************/
// FM Editor Redraw function
/************************************************/
void    FM_Editor_Redraw (void)
{
  int   i, line;
  int   vmax;
  char  mesg[512], **mbase;
  unsigned char *voice;

  Font_SetCurrent (FM_EDITOR_FONT);

  mbase = &fmsheet[0];
  line = 0;

  /**** clear frame buffer ****/
  rectfill (apps.gfx.FM_Editor, 0, 0, FM_EDITOR_SIZE_X - 1, FM_EDITOR_SIZE_Y - 1, COLOR_SKIN_WINDOW_BACKGROUND);

  /**** set number/parameters ****/
  sprintf(mesg, *mbase, FM_Editor.current_voice_number, (char *)FM_Instruments_Name[FM_Editor.current_voice_number]);
  Font_Print (-1, apps.gfx.FM_Editor, mesg, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
  mbase++;
  line++;
  Font_Print (-1, apps.gfx.FM_Editor, *mbase, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
  mbase++;
  line++;

  voice = (unsigned char *)(&FM_OPL_Patchs[FM_Editor.current_voice_number]);
  for (i = 0; i < sizeof (FM_OPL_Patch) / 2; i++ )
    {
    sprintf(mesg, *mbase, *voice, *(voice + 1));
    Font_Print (-1, apps.gfx.FM_Editor, mesg, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
    voice += 2;
    mbase++;
    line++;
  }
  Font_Print (-1, apps.gfx.FM_Editor, *mbase, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
  line += 2;

  /**** now select FM voice ****/
  if (!(FM_Regs[0x0e] & 0x20))
     {
     sprintf(mesg, " FM-MODE: 9 voices mode");
     Font_Print (-1, apps.gfx.FM_Editor, mesg, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
     line++;
     vmax = 9;
     }
  else
     {
     sprintf(mesg, " FM-MODE: 6 voices & rhythm mode");
     Font_Print (-1, apps.gfx.FM_Editor, mesg, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
     line++;
     vmax = 6;
     }
  for (i = 0; i < 9; i++)
     {
     if (i < vmax)  sprintf(mesg, "  Channel #%d = %2d[%02x] ", i, (FM_Regs[0x30 + i]>>4)&0x0f, FM_Regs[0x30 + i]&0x0f);
     else           sprintf(mesg, "  Channel #%d = %2d[%02x]*", i, (FM_Regs[0x30 + i]>>4)&0x0f, FM_Regs[0x30 + i]&0x0f);
     Font_Print (-1, apps.gfx.FM_Editor, mesg, 0, line * fonty, COLOR_SKIN_WINDOW_TEXT);
     line++;
     }
}

/************************************************/
// Callback function for widgets clicks
/************************************************/
void    FM_Editor_CallBack (t_widget *w)
{
  int   i, vr, vl;
  signed char *voice = (signed char *)(&FM_OPL_Patchs[FM_Editor.current_voice_number]);

  vr = (w->id >> 1) - 1;
  switch (vr)
    {
    case -1:
       /**** fm num ****/
       FM_Editor.current_voice_number += 1 - (2 * (w->id & 1));
       if (FM_Editor.current_voice_number < 1)            FM_Editor.current_voice_number = 1;
       else if (FM_Editor.current_voice_number >= 0x10)   FM_Editor.current_voice_number = 0x0f;
       break;
    default:
       *(voice + vr) += 1 - (2 * (w->id & 1));
       if (*(voice + vr) < param_limit[vr][0])       *(voice + vr) = param_limit[vr][0];
       else if (*(voice + vr) > param_limit[vr][1])  *(voice + vr) = param_limit[vr][1];
       // Now update voices setting
       if (!(FM_Regs[0x0e] & 0x20)) vl = 9;
       else                         vl = 6;
       for (i = 0; i < vl; i++)
          {
          if (((FM_Regs[0x30 + i] >> 4) & 0x0f) == FM_Editor.current_voice_number)
             {
             vcref[i] = -1;
             }
          }
       break;
    }
}

/************************************************/
// Initialize FM Editor Applet
/************************************************/
void        FM_Editor_Init (void)
{
    int     i;
    t_frame frame;

    FM_Editor.current_voice_number = 1;       /* first voice is "1" */
    fonty = Font_Height (FM_EDITOR_FONT);
    fontx = 6; // Arbitrary.. is actual FM_EDITOR_FONT character width

    apps.id.FM_Editor = gui_box_create (300, 80, FM_EDITOR_SIZE_X - 1, FM_EDITOR_SIZE_Y - 1, Msg_Get(MSG_FM_Editor_BoxTitle));
    FM_Editor.box = gui.box[apps.id.FM_Editor];
    apps.gfx.FM_Editor = al_create_bitmap (FM_EDITOR_SIZE_X, FM_EDITOR_SIZE_Y);
    gui_set_image_box (apps.id.FM_Editor, apps.gfx.FM_Editor);
    FM_Editor.box->update = FM_Editor_Redraw;
    Desktop_Register_Box ("FMEDITOR", apps.id.FM_Editor, 0, &apps.active.FM_Editor);

    frame.pos.x = fontx*8;
    frame.pos.y = fonty*0;
    frame.size.x = fontx*2;
    frame.size.y = fonty*1;
    widget_button_add (apps.id.FM_Editor, &frame, 1, FM_Editor_CallBack);
    widget_button_add (apps.id.FM_Editor, &frame, 2, FM_Editor_CallBack);

    for (i = 0; i < 11; i++)
    {
        /**** OP1 ****/
        frame.pos.x = fontx*17;
        frame.pos.y = fonty*(i+2);
        frame.size.x = (fontx*5)-1;
        frame.size.y = (fonty*1)-1;
        widget_button_add (apps.id.FM_Editor, &frame, 1, FM_Editor_CallBack);
        widget_button_add (apps.id.FM_Editor, &frame, 2, FM_Editor_CallBack);
        /**** OP2 ****/
        frame.pos.x = fontx*23;
        widget_button_add (apps.id.FM_Editor, &frame, 1, FM_Editor_CallBack);
        widget_button_add (apps.id.FM_Editor, &frame, 2, FM_Editor_CallBack);
    }

    widget_closebox_add(FM_Editor.box, FM_Editor_Switch);
}

/************************************************/
// Switch FM Editor Applet
/************************************************/
void    FM_Editor_Switch (void)
{
    apps.active.FM_Editor ^= 1;
    gui_box_show (gui.box[apps.id.FM_Editor], apps.active.FM_Editor, TRUE);
    if (apps.active.FM_Editor)
        Msg(MSGT_USER, Msg_Get(MSG_FM_Editor_Enabled));
    else
        Msg(MSGT_USER, Msg_Get(MSG_FM_Editor_Disabled));
    gui_menu_inverse_check (menus_ID.fm, 3);
}

/* EOF */

#endif
