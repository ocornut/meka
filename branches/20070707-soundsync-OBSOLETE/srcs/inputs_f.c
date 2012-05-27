//-----------------------------------------------------------------------------
// MEKA - inputs_f.c
// Inputs File Load/Save - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "config_j.h"
#include "keyinfo.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------

FILE   *INP_File;
INLINE  void INP_Write_Line     (char *s)                   { fprintf (INP_File, "%s\n", s); }
INLINE  void INP_Write_Str      (char *name, char *s)       { fprintf (INP_File, "%s = %s\n", name, s); }
INLINE  void INP_Write_Int      (char *name, int value)     { fprintf (INP_File, "%s = %d\n", name, value); }
INLINE  void INP_Write_Float    (char *name, float value)   { fprintf (INP_File, "%s = %.2f\n", name, value); }

//-----------------------------------------------------------------------------

static const char *Inputs_Src_List_KeyWords [] =
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

static int  Load_Inputs_Src_Parse_Var (int VarIdx, char *s, t_input_src *input_src)
{
    char w[256];

    if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
        return MEKA_ERR_EMPTY;

    switch (VarIdx)
    {
    case  0: // type -----------------------------------------------------------
        if (!strcmp(w, "keyboard"))
        {
            input_src->type = INPUT_SRC_TYPE_KEYBOARD;
            input_src->flags = INPUT_SRC_FLAGS_DIGITAL;
            input_src->Connected_and_Ready = TRUE;
            return MEKA_ERR_OK;
        }
        if (!strcmp(w, "joypad"))
        {
            input_src->type = INPUT_SRC_TYPE_JOYPAD;
            input_src->flags = INPUT_SRC_FLAGS_DIGITAL;
            input_src->Connected_and_Ready = FALSE;
            return MEKA_ERR_OK;
        }
        if (!strcmp(w, "mouse"))
        {
            input_src->type = INPUT_SRC_TYPE_MOUSE;
            input_src->flags = INPUT_SRC_FLAGS_ANALOG;
            input_src->Connected_and_Ready = (g_Env.mouse_installed ? TRUE : FALSE);
            return MEKA_ERR_OK;
        }
        return MEKA_ERR_SYNTAX;

    case  1: // enabled --------------------------------------------------------
        if (!strcmp(w, "yes"))      { input_src->enabled = TRUE; return MEKA_ERR_OK; }
        if (!strcmp(w, "no"))       { input_src->enabled = FALSE;  return MEKA_ERR_OK; }
        return MEKA_ERR_SYNTAX;

    case  2: // player ---------------------------------------------------------
        input_src->player = atoi(w) - 1;
        if (input_src->player != 0 && input_src->player != 1)
            return MEKA_ERR_SYNTAX;
        return MEKA_ERR_OK;

    case  3: // driver ---------------------------------------------------------
        if (input_src->type == INPUT_SRC_TYPE_JOYPAD)
            input_src->Driver = Config_Driver_Joy_Str_to_Int(w);
        return MEKA_ERR_OK;

    case  4: // connection -----------------------------------------------------
        input_src->Connection_Port = atoi(w);
        if (input_src->Connection_Port > 0) input_src->Connection_Port -= 1;
        return MEKA_ERR_OK;

    case  5: // emulate_digital ------------------------------------------------
        if (!strcmp (w, "yes"))      { input_src->flags |= INPUT_SRC_FLAGS_EMULATE_DIGITAL; return MEKA_ERR_OK; }
        if (!strcmp (w, "no"))       { input_src->flags &= ~INPUT_SRC_FLAGS_EMULATE_DIGITAL; return MEKA_ERR_OK; }
        return MEKA_ERR_SYNTAX;

    case  6: // emulate_analog -------------------------------------------------
        if (!strcmp (w, "yes"))      { input_src->flags |= INPUT_SRC_FLAGS_EMULATE_ANALOG; return MEKA_ERR_OK; }
        if (!strcmp (w, "no"))       { input_src->flags &= ~INPUT_SRC_FLAGS_EMULATE_ANALOG; return MEKA_ERR_OK; }
        return MEKA_ERR_SYNTAX;

    case  7: // digital_falloff ------------------------------------------------
        input_src->Analog_to_Digital_FallOff = atof (w);
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
            //if (MapIdx >= INPUT_MAP_DIGITAL_UP && MapIdx <= INPUT_MAP_DOWN && !(input_src->Result_Type & DIGITAL))
            //   return MEKA_ERR_INCOHERENT;

            if (!strcmp (w, "key"))
            {
                const t_key_info *key_info;
                if (input_src->type != INPUT_SRC_TYPE_KEYBOARD)
                    return MEKA_ERR_INCOHERENT;
                //if (!Parse_LineGet (&w, &s))
                //  return MEKA_ERR_SYNTAX;
                parse_skip_spaces(&s);
                if (StrNull (s))
                    return MEKA_ERR_SYNTAX;
                key_info = KeyInfo_FindByName(s);
                if (key_info != NULL)
                {
                    input_src->Map [MapIdx].Type = INPUT_MAP_TYPE_KEY;
                    input_src->Map [MapIdx].Idx = key_info->scancode;
                }
                return MEKA_ERR_OK;
            }

            if (!strcmp (w, "joy"))
            {
                int stick, axis, dir;

                if (input_src->type != INPUT_SRC_TYPE_JOYPAD)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE) || strcmp(w, "stick") || !parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                stick = atoi(w);
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE) || strcmp(w, "axis") || !parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                axis = atoi(w);
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE) || strcmp(w, "dir") || !parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                dir = atoi(w);
                input_src->Map [MapIdx].Type = INPUT_MAP_TYPE_JOY_AXIS;
                input_src->Map [MapIdx].Idx = MAKE_STICK_AXIS_DIR (stick, axis, dir);
                return MEKA_ERR_OK;
            }

            if (!strcmp (w, "joy_button"))
            {
                if (input_src->type != INPUT_SRC_TYPE_JOYPAD)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                input_src->Map [MapIdx].Type = INPUT_MAP_TYPE_JOY_BUTTON;
                input_src->Map [MapIdx].Idx = atoi(w);
                return MEKA_ERR_OK;
            }

            if (!strcmp (w, "mouse_button"))
            {
                if (input_src->type != INPUT_SRC_TYPE_MOUSE)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                input_src->Map [MapIdx].Type = INPUT_MAP_TYPE_MOUSE_BUTTON;
                input_src->Map [MapIdx].Idx = atoi(w);
                return MEKA_ERR_OK;
            }

            return MEKA_ERR_SYNTAX;
        }

    case 16: // player_x_axis --------------------------------------------------
    case 17: // player_y_axis --------------------------------------------------
        {
            int MapIdx = INPUT_MAP_ANALOG_AXIS_X + VarIdx - 16;

            if (!(input_src->flags & INPUT_SRC_FLAGS_ANALOG))
                return MEKA_ERR_INCOHERENT;

            if (!strcmp (w, "mouse_axis"))
            {
                if (input_src->type != INPUT_SRC_TYPE_MOUSE)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                input_src->Map [MapIdx].Type = input_src->Map [MapIdx + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
                input_src->Map [MapIdx].Idx = input_src->Map [MapIdx + 2].Idx = MAKE_AXIS(atoi(w));
                return MEKA_ERR_OK;
            }

            return MEKA_ERR_SYNTAX;
        }

    case 18: // joy_driver -----------------------------------------------------
        {
            Inputs.Sources_Joy_Driver = Config_Driver_Joy_Str_to_Int (w);
            return MEKA_ERR_OK;
        }

    case 19: // mouse_speed_x --------------------------------------------------
        {
            Inputs.MouseSpeed_X = atoi (w);
            return MEKA_ERR_OK;
        }

    case 20: // mouse_speed_y --------------------------------------------------
        {
            Inputs.MouseSpeed_Y = atoi (w);
            return MEKA_ERR_OK;
        }

    case 21: // cabinet_mode ---------------------------------------------------
        {
            Inputs.Cabinet_Mode = atoi (w);
            return MEKA_ERR_OK;
        }
    }

    return MEKA_ERR_SYNTAX;
}

void            Load_Inputs_Src_List (void)
{
    int             i;
    char            w[256];
    t_input_src *   input_src = NULL;

    t_tfile *       tf;
    t_list *        lines;
    int             line_cnt;

    // Open and read file
    ConsolePrint (Msg_Get (MSG_Inputs_Src_Loading));
    tf = tfile_read (Inputs.FileName);
    if (tf == NULL)
        Quit_Msg (meka_strerror());
    ConsolePrint ("\n");

    // Parse each line
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        char *line;

        line_cnt += 1;
        line = lines->elem;

        if (StrNull(line))
            continue;

        if (line[0] == '[' && line[strlen(line) - 1] == ']')
        {
            input_src = Inputs_Sources_Add (StrNDup (line + 1, strlen(line) - 2));
            // ConsolePrintf ("new source --> %s <--\n", CurSrc->Name);
            continue;
        }

        strlwr(line);
        if (!parse_getword(w, sizeof(w), &line, "=", ';', PARSE_FLAGS_NONE))
            continue;

        for (i = 0; Inputs_Src_List_KeyWords[i]; i++)
            if (strcmp(w, Inputs_Src_List_KeyWords[i]) == 0)
            {
                // FIXME: this is ugly
                if (input_src == NULL && strcmp (w, "joy_driver") != 0
                    && strcmp (w, "mouse_speed_x") != 0
                    && strcmp (w, "mouse_speed_y") != 0
                    && strcmp (w, "cabinet_mode")  != 0)
                {
                    tfile_free(tf);
                    Quit_Msg (Msg_Get (MSG_Inputs_Src_Missing), line_cnt);
                }

                parse_skip_spaces(&line);
                if (!parse_getword(w, sizeof(w), &line, "", ';', PARSE_FLAGS_NONE))
                {
                    tfile_free(tf);
                    Quit_Msg (Msg_Get (MSG_Inputs_Src_Equal), line_cnt);
                }

                switch (Load_Inputs_Src_Parse_Var(i, w, input_src))
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
                Quit_Msg (Msg_Get (MSG_Inputs_Src_Unrecognized), line_cnt, w);
            }
    }

    // Free file data
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
 INP_Write_Line ("; " MEKA_NAME " " MEKA_VERSION " - Inputs Configuration");
 INP_Write_Line ("; This file is automatically updated and rewritten when quitting");
 INP_Write_Line ("; Feel free to edit this file manually if you feel the need to.");
 INP_Write_Line ("; However, any comment you may manually add will be deleted!");
 INP_Write_Line (";-----------------------------------------------------------------------------");
 INP_Write_Line ("");
#ifdef ARCH_DOS
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
   t_input_src *input_src = Inputs.Sources[i];
   sprintf (s, "[%s]", input_src->name);
   INP_Write_Line (s);
   switch (input_src->type)
     {
     case INPUT_SRC_TYPE_KEYBOARD: sprintf (s, "keyboard");  break;
     case INPUT_SRC_TYPE_JOYPAD:   sprintf (s, "joypad");    break;
     case INPUT_SRC_TYPE_MOUSE:    sprintf (s, "mouse");     break;
     }
   INP_Write_Str ("type               ", s);
   if (input_src->type == INPUT_SRC_TYPE_JOYPAD)
      INP_Write_Int  ("connection         ", input_src->Connection_Port + 1);
   INP_Write_Str ("enabled            ", input_src->enabled ? "yes" : "no");
   INP_Write_Int ("player             ", input_src->player + 1);
   for (j = 0; j < INPUT_MAP_MAX; j++)
      {
      int n = input_src->Map [j].Idx;
      if (n == -1)
         continue;
      s[0] = EOSTR;
      if (input_src->flags & INPUT_SRC_FLAGS_DIGITAL)
        switch (j)
          {
          case INPUT_MAP_DIGITAL_UP:    strcpy (s, "player_up          "); break;
          case INPUT_MAP_DIGITAL_DOWN:  strcpy (s, "player_down        "); break;
          case INPUT_MAP_DIGITAL_LEFT:  strcpy (s, "player_left        "); break;
          case INPUT_MAP_DIGITAL_RIGHT: strcpy (s, "player_right       "); break;
          }
      if (input_src->flags & INPUT_SRC_FLAGS_ANALOG)
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
      switch (input_src->type)
        {
        // Keyboard -----------------------------------------------------------
        case INPUT_SRC_TYPE_KEYBOARD:
            {
                // Handle special case of the ; key that has to be backslashed
                // Removed because... colon is actually ':', not ';' :)
                // if (n == KEY_COLON)
                //    sprintf (s2, "key \\;");
                //else
                {
                    const t_key_info *key_info = KeyInfo_FindByScancode(n);
                    char *key_name = key_info ? key_info->name : "error";
                    char *key_name_escaped = parse_escape_string(key_name, NULL);
                    sprintf(s2, "key %s", key_name_escaped ? key_name_escaped : key_name);
                }
                break;
            }
        // Joypad -------------------------------------------------------------
        case INPUT_SRC_TYPE_JOYPAD:
             switch (input_src->Map [j].Type)
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
        case INPUT_SRC_TYPE_MOUSE:
             switch (input_src->Map [j].Type)
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
         INP_Write_Str(s, s2);
      }
   // Not yet implemented
   // if (input_src->Result_Type == DIGITAL)
   //   INP_Write_Str ("emulate_analog     ", (input_src->flags & INPUT_SRC_FLAGS_EMULATE_ANALOG) ? "yes" : "no");
   if (input_src->flags & INPUT_SRC_FLAGS_ANALOG)
      {
      INP_Write_Str   ("emulate_digital    ", (input_src->flags & INPUT_SRC_FLAGS_EMULATE_DIGITAL) ? "yes" : "no");
      INP_Write_Float ("digital_falloff    ", input_src->Analog_to_Digital_FallOff);
      }
   INP_Write_Line ("");
   }

 INP_Write_Line (";-----------------------------------------------------------------------------");
 fclose (INP_File);
}

//-----------------------------------------------------------------------------

