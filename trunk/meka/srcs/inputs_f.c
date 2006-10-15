//-----------------------------------------------------------------------------
// MEKA - inputs_f.c
// Inputs File Load/Save - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "config_j.h"
#include "libparse.h"

//-----------------------------------------------------------------------------

FILE   *INP_File;
INLINE  void INP_Write_Line     (char *s)                   { fprintf (INP_File, "%s\n", s); }
INLINE  void INP_Write_Str      (char *name, char *s)       { fprintf (INP_File, "%s = %s\n", name, s); }
INLINE  void INP_Write_Int      (char *name, int value)     { fprintf (INP_File, "%s = %d\n", name, value); }
INLINE  void INP_Write_Float    (char *name, float value)   { fprintf (INP_File, "%s = %.2f\n", name, value); }

//-----------------------------------------------------------------------------

static char *Inputs_Src_List_KeyWords [] =
{
 "type", "enabled", "player",
 "driver", "connection",
 "emulate_digital", "emulate_analog", "digital_falloff",
 "player_up", "player_down", "player_left", "player_right",
 "player_button1", "player_button2", "player_start_pause", "player_reset",
 "player_x_axis", "player_y_axis",
 "joy_driver",    // particular case
 "mouse_speed_x", // particular case
 "mouse_speed_y", // particular case
 "cabinet_mode",  // particular case
  NULL
};

//-----------------------------------------------------------------------------

int     Load_Inputs_Src_Parse_Var (int VarIdx, char *s, t_input_src *ISrc)
{
 t_word w;

 if (!Parse_WordGet (&w, &s))
    return MEKA_ERR_EMPTY;

 switch (VarIdx)
 {
   case  0: // type -----------------------------------------------------------
      if (!strcmp (w.s, "keyboard"))
         {
         ISrc->Type = INPUT_SRC_KEYBOARD;
         ISrc->Result_Type = DIGITAL;
         ISrc->Connected_and_Ready = YES;
         return MEKA_ERR_OK;
         }
      if (!strcmp (w.s, "joypad"))
         {
         ISrc->Type = INPUT_SRC_JOYPAD;
         ISrc->Result_Type = DIGITAL;
         ISrc->Connected_and_Ready = NO;
         return MEKA_ERR_OK;
         }
      if (!strcmp (w.s, "mouse"))
         {
         ISrc->Type = INPUT_SRC_MOUSE;
         ISrc->Result_Type = ANALOG;
         ISrc->Connected_and_Ready = (cfg.Mouse_Installed ? YES : NO);
         return MEKA_ERR_OK;
         }
      return MEKA_ERR_SYNTAX;

   case  1: // enabled --------------------------------------------------------
      if (!strcmp (w.s, "yes"))      { ISrc->Enabled = YES; return MEKA_ERR_OK; }
      if (!strcmp (w.s, "no"))       { ISrc->Enabled = NO; return MEKA_ERR_OK; }
      return MEKA_ERR_SYNTAX;

   case  2: // player ---------------------------------------------------------
      ISrc->Player = GetNbr (w.s);
      if (ISrc->Player == 0)
         return MEKA_ERR_SYNTAX;
      ISrc->Player -= 1;
      return MEKA_ERR_OK;

   case  3: // driver ---------------------------------------------------------
      if (ISrc->Type == INPUT_SRC_JOYPAD)
         ISrc->Driver = Config_Driver_Joy_Str_to_Int (w.s);
      return MEKA_ERR_OK;

   case  4: // connection -----------------------------------------------------
      ISrc->Connection_Port = GetNbr (w.s);
      if (ISrc->Connection_Port > 0) ISrc->Connection_Port -= 1;
      return MEKA_ERR_OK;

   case  5: // emulate_digital ------------------------------------------------
      if (!strcmp (w.s, "yes"))      { ISrc->Result_Type |= EMULATE_DIGITAL; return MEKA_ERR_OK; }
      if (!strcmp (w.s, "no"))       { ISrc->Result_Type &= ~EMULATE_DIGITAL; return MEKA_ERR_OK; }
      return MEKA_ERR_SYNTAX;

   case  6: // emulate_analog -------------------------------------------------
      if (!strcmp (w.s, "yes"))      { ISrc->Result_Type |= EMULATE_ANALOG; return MEKA_ERR_OK; }
      if (!strcmp (w.s, "no"))       { ISrc->Result_Type &= ~EMULATE_ANALOG; return MEKA_ERR_OK; }
      return MEKA_ERR_SYNTAX;

   case  7: // digital_falloff ------------------------------------------------
      ISrc->Analog_to_Digital_FallOff = atof (w.s);
      return MEKA_ERR_OK;

   case  8: // player_up ------------------------------------------------------
   case  9: // player_down ----------------------------------------------------
   case 10: // player_left ----------------------------------------------------
   case 11: // player_right ---------------------------------------------------
   case 12: // player_button1 -------------------------------------------------
   case 13: // player_button2 -------------------------------------------------
   case 14: // player_start_pause ---------------------------------------------
   case 15: // player_reset ---------------------------------------------------
   {
      int MapIdx = INPUT_MAP_DIGITAL_UP + VarIdx - 8;

      // FIXME: ???
      //if (MapIdx >= INPUT_MAP_DIGITAL_UP && MapIdx <= INPUT_MAP_DOWN && !(ISrc->Result_Type & DIGITAL))
      //   return MEKA_ERR_INCOHERENT;

      if (!strcmp (w.s, "key"))
         {
         t_key_info *key_info;
         if (ISrc->Type != INPUT_SRC_KEYBOARD)
            return MEKA_ERR_INCOHERENT;
         //if (!Parse_LineGet (&w, &s))
         //  return MEKA_ERR_SYNTAX;
         Parse_SkipSpaces (&s);
         if (StrNull (s))
            return MEKA_ERR_SYNTAX;
         key_info = KeyInfo_FindByName(s);
         if (key_info != NULL)
         {
            ISrc->Map [MapIdx].Type = INPUT_MAP_TYPE_KEY;
            ISrc->Map [MapIdx].Idx = key_info->scancode;
         }
         return MEKA_ERR_OK;
         }

      if (!strcmp (w.s, "joy"))
         {
         int stick, axis, dir;

         if (ISrc->Type != INPUT_SRC_JOYPAD)
            return MEKA_ERR_INCOHERENT;
         if (!Parse_WordGet (&w, &s) || strcmp (w.s, "stick") || !Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         stick = GetNbr (w.s);
         if (!Parse_WordGet (&w, &s) || strcmp (w.s, "axis") || !Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         axis = GetNbr (w.s);
         if (!Parse_WordGet (&w, &s) || strcmp (w.s, "dir") || !Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         dir = GetNbr (w.s);
         ISrc->Map [MapIdx].Type = INPUT_MAP_TYPE_JOY_AXIS;
         ISrc->Map [MapIdx].Idx = MAKE_STICK_AXIS_DIR (stick, axis, dir);
         return MEKA_ERR_OK;
         }

      if (!strcmp (w.s, "joy_button"))
         {
         if (ISrc->Type != INPUT_SRC_JOYPAD)
            return MEKA_ERR_INCOHERENT;
         if (!Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         ISrc->Map [MapIdx].Type = INPUT_MAP_TYPE_JOY_BUTTON;
         ISrc->Map [MapIdx].Idx = GetNbr (w.s);
         return MEKA_ERR_OK;
         }

      if (!strcmp (w.s, "mouse_button"))
         {
         if (ISrc->Type != INPUT_SRC_MOUSE)
            return MEKA_ERR_INCOHERENT;
         if (!Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         ISrc->Map [MapIdx].Type = INPUT_MAP_TYPE_MOUSE_BUTTON;
         ISrc->Map [MapIdx].Idx = GetNbr (w.s);
         return MEKA_ERR_OK;
         }

      return MEKA_ERR_SYNTAX;
   }

   case 16: // player_x_axis --------------------------------------------------
   case 17: // player_y_axis --------------------------------------------------
   {
      int MapIdx = INPUT_MAP_ANALOG_AXIS_X + VarIdx - 16;

      if (!(ISrc->Result_Type & ANALOG))
         return MEKA_ERR_INCOHERENT;

      if (!strcmp (w.s, "mouse_axis"))
         {
         if (ISrc->Type != INPUT_SRC_MOUSE)
            return MEKA_ERR_INCOHERENT;
         if (!Parse_WordGet (&w, &s))
            return MEKA_ERR_SYNTAX;
         ISrc->Map [MapIdx].Type = ISrc->Map [MapIdx + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
         ISrc->Map [MapIdx].Idx = ISrc->Map [MapIdx + 2].Idx = MAKE_AXIS (GetNbr (w.s));
         return MEKA_ERR_OK;
         }

      return MEKA_ERR_SYNTAX;
   }

   case 18: // joy_driver -----------------------------------------------------
   {
      Inputs.Sources_Joy_Driver = Config_Driver_Joy_Str_to_Int (w.s);
      return MEKA_ERR_OK;
   }

   case 19: // mouse_speed_x --------------------------------------------------
   {
     Inputs.MouseSpeed_X = atoi (w.s);
     return MEKA_ERR_OK;
   }

   case 20: // mouse_speed_y --------------------------------------------------
   {
     Inputs.MouseSpeed_Y = atoi (w.s);
     return MEKA_ERR_OK;
   }

   case 21: // cabinet_mode ---------------------------------------------------
   {
     Inputs.Cabinet_Mode = atoi (w.s);
     return MEKA_ERR_OK;
   }
 }

 return MEKA_ERR_SYNTAX;
}

void            Load_Inputs_Src_List (void)
{
 int            i, j;
 char           s2 [256]; // FIXME
 char           *p;
 t_word         w;
 t_input_src    *CurSrc = NULL;

 t_tfile        *tf;
 t_list         *lines;
 char           *line;
 int            line_cnt;

 ConsolePrint (Msg_Get (MSG_Inputs_Src_Loading));

 // Open and read file --------------------------------------------------------
 tf = tfile_read (Inputs.FileName);
 if (tf == NULL)
    Quit_Msg (meka_strerror());

 // Ok
 ConsolePrint ("\n");

 // Configure LibParse
 Parse_SetSep (" \t=");

 // Parse each line -----------------------------------------------------------
 line_cnt = 0;
 for (lines = tf->data_lines; lines; lines = lines->next)
    {
    line_cnt += 1;
    line = lines->elem;

    // FIXME
    // Copy string and zap out comments
    // Handle inhibited (backslashed) ';' character.
    // We can use '\;' to specify the ';' key for the input system
    for (i = 0, j = 0; i < 256 && line[i]; i ++)
       {
       if (line [i] == ';' && (i == 0 || line [i - 1] != '\\'))
          break;
       s2 [j++] = line [i];
       }
    s2 [j] = EOSTR;
    if (StrNull (s2))
       continue;

    if (s2[0] == '[' && s2[strlen(s2) - 1] == ']')
       {
       CurSrc = Inputs_Sources_Add (StrNDup (s2 + 1, strlen(s2) - 2));
       // ConsolePrintf ("new source --> %s <--\n", CurSrc->Name);
       continue;
       }

    strlwr (s2);
    p = s2;
    Parse_WordGet (&w, &p);
    if (w.len == 0)
       continue;

    for (i = 0; Inputs_Src_List_KeyWords [i]; i++)
       if (strcmp (w.s, Inputs_Src_List_KeyWords [i]) == 0)
          {
          // FIXME: this is ugly
          if (CurSrc == NULL && strcmp (w.s, "joy_driver") != 0
                             && strcmp (w.s, "mouse_speed_x") != 0
                             && strcmp (w.s, "mouse_speed_y") != 0
                             && strcmp (w.s, "cabinet_mode")  != 0)
             {
             tfile_free(tf);
             Quit_Msg (Msg_Get (MSG_Inputs_Src_Missing), line_cnt);
             }
          p = strchr (p, '=');
          if (p == NULL)
             {
             tfile_free(tf);
             Quit_Msg (Msg_Get (MSG_Inputs_Src_Equal), line_cnt);
             }
          switch (Load_Inputs_Src_Parse_Var (i, p + 1, CurSrc))
            {
            case MEKA_ERR_SYNTAX:
                 tfile_free(tf);
                 Quit_Msg (Msg_Get (MSG_Inputs_Src_Syntax_Param), line_cnt);
            case MEKA_ERR_INCOHERENT :
                 tfile_free(tf);
                 Quit_Msg (Msg_Get (MSG_Inputs_Src_Inconsistency), line_cnt);
                 break;
            // FIXME: EMPTY is not handled there
            }
          break;
          }
    if (!Inputs_Src_List_KeyWords [i])
       {
       tfile_free(tf);
       Quit_Msg (Msg_Get (MSG_Inputs_Src_Unrecognized), line_cnt, w.s);
       }
    }

 // Free file data ------------------------------------------------------------
 tfile_free(tf);

 // Verify that we have enough inputs sources
 if (Inputs.Sources_Max == 0)
    Quit_Msg (Msg_Get (MSG_Inputs_Src_Not_Enough));
}

void    Write_Inputs_Src_List (void)
{
 int    i, j;

 if (!(INP_File = fopen (Inputs.FileName, "wt")))
    return;

 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; MEKA " VERSION " - Inputs Configuration");
 INP_Write_Line ("; This file is automatically updated and rewritten when quitting");
 INP_Write_Line ("; Feel free to edit this file manually if you feel the need to.");
 INP_Write_Line ("; However, any comment you may manually add will be deleted!");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
#ifdef DOS
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; (MS-DOS version only) select joypad/joystick driver here:");
 INP_Write_Line (";");
 INP_Write_Str  ("joy_driver         ", Config_Driver_Joy_Int_to_Str (Inputs.Sources_Joy_Driver));
 INP_Write_Line (";");
 INP_Write_Line ("; Available joypad/joystick drivers:");
 INP_Write_Line (";   none,                 (No controller)");
 INP_Write_Line (";   auto,                 (Autodetect)");
 INP_Write_Line (";   2b, 4b, 6b, 8b,       (Standard 2/4/6/8 buttons controllers)");
 INP_Write_Line (";   dual,                 (Two 2 buttons controllers)");
 INP_Write_Line (";   sidewinder,           (Microsoft Sidewinder)");
 INP_Write_Line (";   fspro,                (CH Flightstick Pro)");
 INP_Write_Line (";   wingex,               (Logitech Wingman Extreme / Trustmaster Mk. I compatible)");
 INP_Write_Line (";   wingwar,              (Logitech Wingman Warrior)");
 INP_Write_Line (";   gamepadpro,           (Gravis GamePad Pro)");
 INP_Write_Line (";   grip,                 (Gravis GrIP)");
 INP_Write_Line (";   grip4,                (Gravis GrIP / 4 axis only)");
 INP_Write_Line (";");
 INP_Write_Line ("; If you have an adapter for console controllers:");
 INP_Write_Line (";   db9lpt1  -> db9ltp3,  (DB-9 controllers: SMS/Megadrive/Atari/...)");
 INP_Write_Line (";   necltp1  -> neclpt3,  (PC Engine / Turbo Grafx controllers)");
 INP_Write_Line (";   psxlpt1  -> psxlpt3,  (Playstation controllers)");
 INP_Write_Line (";   sneslpt1 -> sneslpt3, (Super Famicom / Nintendo controllers)");
 INP_Write_Line (";   n64lpt1  -> n64lpt3,  (Nintendo 64 controllers)");
 INP_Write_Line (";   ifsegaisa,            (Sega Saturn ISA interface cards)");
 INP_Write_Line (";   ifsegapci, ifsegapci2 (Sega Saturn PCI interface cards)");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
#endif
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; Links:");
 INP_Write_Line ("; Using a SMS/DB-9 controller on your computer:");
 INP_Write_Line (";  - Read the TECH.TXT file at first!");
 INP_Write_Line (";  - SmsCardPad");
 INP_Write_Line (";    http://www.smspower.org/smscartpad/");
 INP_Write_Line (";  - PPJoy joystick driver for Windows 2000/XP");
 INP_Write_Line (";    http://www.geocities.com/deonvdw/");
 INP_Write_Line ("; Schematics and drivers for various console controllers adapters:");
 INP_Write_Line (";  - Direct Pad Pro");
 INP_Write_Line (";    http://www.arcadecontrols.com/Mirrors/www.ziplabel.com/dpadpro/");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; Configure mouse speed here:");
 INP_Write_Line (";");
 INP_Write_Int  ("mouse_speed_x       ", Inputs.MouseSpeed_X);
 INP_Write_Int  ("mouse_speed_y       ", Inputs.MouseSpeed_Y);
 INP_Write_Line (";");
 INP_Write_Line ("; The higher the value is, the slower the axis will be.");
 INP_Write_Line ("; 2 is the default for both axis. Use 1 for a fastest mouse movement.");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; Miscellaenous features:");
 INP_Write_Line (";");
 INP_Write_Line ("; Invert ESC (switch screens) and F10 (quit) keys. Arcade cabinet owners");
 INP_Write_Line ("; often have the ESC key mapped to a certain button they want to quit with.");
 INP_Write_Int  ("cabinet_mode        ", Inputs.Cabinet_Mode);
 INP_Write_Line (";");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("; Template for creating new input sources:");
 INP_Write_Line (";");
 INP_Write_Line ("; [Name]");
 INP_Write_Line ("; type = keyboard (digital) | joypad (digital) | mouse (analog)");
 INP_Write_Line (";   Select the type of input device");
 INP_Write_Line ("; enabled = yes | no");
 INP_Write_Line (";   Set to no in order to tell Meka to ignore input from this device");
 INP_Write_Line ("; player = 1 | 2");
 INP_Write_Line (";   Player on which inputs are applicated");
 INP_Write_Line ("; connection = <number>");
 INP_Write_Line (";   Joypad only. Select connection to use: 1 = first pad, 2 = second pad..");
 INP_Write_Line ("; emulate_digital");
 INP_Write_Line (";   Mouse only. Enable support for digital devices (such as SMS joypad),");
 INP_Write_Line (";   to play games with the mouse. Recommended with shooters mainly.");
 INP_Write_Line ("; digital_falloff (between 0 and 1, default being 0.8)");
 INP_Write_Line (";   Mouse only, when digital emulation is enabled. Reduce to increase");
 INP_Write_Line (";   precision, but most games will be harder to play.");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");

 for (i = 0; i < Inputs.Sources_Max; i++)
   {
   char s [256], s2 [256];
   t_input_src *ISrc = Inputs.Sources[i];
   sprintf (s, "[%s]", ISrc->Name);
   INP_Write_Line (s);
   switch (ISrc->Type)
     {
     case INPUT_SRC_KEYBOARD: sprintf (s, "keyboard");  break;
     case INPUT_SRC_JOYPAD:   sprintf (s, "joypad");    break;
     case INPUT_SRC_MOUSE:    sprintf (s, "mouse");     break;
     }
   INP_Write_Str ("type               ", s);
   if (ISrc->Type == INPUT_SRC_JOYPAD)
      INP_Write_Int  ("connection         ", ISrc->Connection_Port + 1);
   INP_Write_Str ("enabled            ", ISrc->Enabled ? "yes" : "no");
   INP_Write_Int ("player             ", ISrc->Player + 1);
   for (j = 0; j < INPUT_MAP_MAX; j++)
      {
      int n = ISrc->Map [j].Idx;
      if (n == -1)
         continue;
      s[0] = EOSTR;
      if (ISrc->Result_Type & DIGITAL)
        switch (j)
          {
          case INPUT_MAP_DIGITAL_UP:    strcpy (s, "player_up          "); break;
          case INPUT_MAP_DIGITAL_DOWN:  strcpy (s, "player_down        "); break;
          case INPUT_MAP_DIGITAL_LEFT:  strcpy (s, "player_left        "); break;
          case INPUT_MAP_DIGITAL_RIGHT: strcpy (s, "player_right       "); break;
          }
      else // ANALOG
      if (ISrc->Result_Type & ANALOG)
        switch (j)
          {
          case INPUT_MAP_ANALOG_AXIS_X: strcpy (s, "player_x_axis      "); break;
          case INPUT_MAP_ANALOG_AXIS_Y: strcpy (s, "player_y_axis      "); break;
          }
      if (StrNull(s))
        switch (j)
          {
          case INPUT_MAP_BUTTON1:     strcpy (s, "player_button1     "); break;
          case INPUT_MAP_BUTTON2:     strcpy (s, "player_button2     "); break;
          case INPUT_MAP_PAUSE_START: strcpy (s, "player_start_pause "); break;
          case INPUT_MAP_RESET:       strcpy (s, "player_reset       "); break;
          }
      if (StrNull(s))
         continue;
      switch (ISrc->Type)
        {
        // Keyboard -----------------------------------------------------------
        case INPUT_SRC_KEYBOARD:
            {
                // Handle special case of the ; key that has to be backslashed
                // Removed because... colon is actually ':', not ';' :)
                // if (n == KEY_COLON)
                //    sprintf (s2, "key \\;");
                //else
                {
                    t_key_info *key_info = KeyInfo_FindByScancode(n);
                    sprintf (s2, "key %s", key_info ? key_info->name : "error");
                }
                break;
            }
        // Joypad -------------------------------------------------------------
        case INPUT_SRC_JOYPAD:
             switch (ISrc->Map [j].Type)
               {
               case INPUT_MAP_TYPE_JOY_BUTTON:
                  sprintf (s2, "joy_button %i", n);
                  break;
               case INPUT_MAP_TYPE_JOY_AXIS:
                  sprintf (s2, "joy stick %i axis %i dir %i", INPUT_MAP_GET_STICK(n), INPUT_MAP_GET_AXIS(n), INPUT_MAP_GET_DIR_LR(n) ? 1 : 0);
                  break;
               // Not implemented
               // case INPUT_MAP_TYPE_JOY_AXIS_ANAL:
               //    break;
               }
             break;
        // Mouse --------------------------------------------------------------
        case INPUT_SRC_MOUSE:
             switch (ISrc->Map [j].Type)
               {
               case INPUT_MAP_TYPE_MOUSE_BUTTON:
                  sprintf (s2, "mouse_button %i", n);
                  break;
               case INPUT_MAP_TYPE_MOUSE_AXIS:
                  sprintf (s2, "mouse_axis %i", INPUT_MAP_GET_AXIS(n));
                  break;
               }
             break;
        }
      if (!StrNull(s2))
         INP_Write_Str (s, s2);
      }
   // Not yet implemented
   // if (ISrc->Result_Type == DIGITAL)
   //   INP_Write_Str ("emulate_analog     ", (ISrc->Result_Type & EMULATE_ANALOG) ? "yes" : "no");
   if (ISrc->Result_Type & ANALOG)
      {
      INP_Write_Str   ("emulate_digital    ", (ISrc->Result_Type & EMULATE_DIGITAL) ? "yes" : "no");
      INP_Write_Float ("digital_falloff    ", ISrc->Analog_to_Digital_FallOff);
      }
   INP_Write_Line ("");
   }

 INP_Write_Line (";-----------------------------------------------------------------------------");
 fclose (INP_File);
}

//-----------------------------------------------------------------------------

