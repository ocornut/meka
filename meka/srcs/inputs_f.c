//-----------------------------------------------------------------------------
// MEKA - inputs_f.c
// Inputs File Load/Save - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "keyinfo.h"
#include "libparse.h"

//-----------------------------------------------------------------------------

FILE   *INP_File;
INLINE  void INP_Write_Line     (const char *s)                   { fprintf(INP_File, "%s\n", s); }
INLINE  void INP_Write_Str      (const char *name, const char *s) { fprintf(INP_File, "%s = %s\n", name, s); }
INLINE  void INP_Write_Int      (const char *name, int value)     { fprintf(INP_File, "%s = %d\n", name, value); }
INLINE  void INP_Write_Float    (const char *name, float value)   { fprintf(INP_File, "%s = %.2f\n", name, value); }

//-----------------------------------------------------------------------------

static const char *Inputs_Src_List_KeyWords [] =
{
    "type", "enabled", "player",
    "driver", "connection",
    "emulate_digital", "emulate_analog", "digital_falloff",
    "player_up", "player_down", "player_left", "player_right",
    "player_button1", "player_button2", "player_start_pause", "player_reset",
    "player_x_axis", "player_y_axis",
    "mouse_speed_x", // OBSOLETE
    "mouse_speed_y", // OBSOLETE
    "cabinet_mode",  // particular case
    NULL
};

//-----------------------------------------------------------------------------

static int  Load_Inputs_Src_Parse_Var (int var_idx, char *s, t_input_src *input_src)
{
    char w[256];
    if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
        return MEKA_ERR_EMPTY;

    switch (var_idx)
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
            input_src->Connected_and_Ready = (g_env.mouse_installed ? TRUE : FALSE);
            return MEKA_ERR_OK;
        }
        return MEKA_ERR_SYNTAX;

    case  1: // enabled --------------------------------------------------------
        if (!strcmp(w, "yes"))      { input_src->enabled = TRUE; return MEKA_ERR_OK; }
        if (!strcmp(w, "no"))       { input_src->enabled = FALSE; return MEKA_ERR_OK; }
        return MEKA_ERR_SYNTAX;

    case  2: // player ---------------------------------------------------------
        input_src->player = atoi(w) - 1;
        if (input_src->player != 0 && input_src->player != 1)
            return MEKA_ERR_SYNTAX;
        return MEKA_ERR_OK;

    case  3: // driver ---------------------------------------------------------
		// FIXME-OBSOLETE
        return MEKA_ERR_OK;

    case  4: // connection -----------------------------------------------------
        input_src->Connection_Port = atoi(w);
        if (input_src->Connection_Port > 0) input_src->Connection_Port -= 1;
        return MEKA_ERR_OK;

    case  5: // emulate_digital ------------------------------------------------
        if (!strcmp(w, "yes"))      { input_src->flags |= INPUT_SRC_FLAGS_EMULATE_DIGITAL; return MEKA_ERR_OK; }
        if (!strcmp(w, "no"))       { input_src->flags &= ~INPUT_SRC_FLAGS_EMULATE_DIGITAL; return MEKA_ERR_OK; }
        return MEKA_ERR_SYNTAX;

    case  6: // emulate_analog -------------------------------------------------
        if (!strcmp(w, "yes"))      { input_src->flags |= INPUT_SRC_FLAGS_EMULATE_ANALOG; return MEKA_ERR_OK; }
        if (!strcmp(w, "no"))       { input_src->flags &= ~INPUT_SRC_FLAGS_EMULATE_ANALOG; return MEKA_ERR_OK; }
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
            const int map_idx = INPUT_MAP_DIGITAL_UP + var_idx - 8;
			t_input_map_entry* map = &input_src->Map[map_idx];

            // FIXME: ???
            //if (MapIdx >= INPUT_MAP_DIGITAL_UP && MapIdx <= INPUT_MAP_DOWN && !(input_src->Result_Type & DIGITAL))
            //   return MEKA_ERR_INCOHERENT;

            if (!strcmp(w, "key"))
            {
                const t_key_info *key_info;
                if (input_src->type != INPUT_SRC_TYPE_KEYBOARD)
                    return MEKA_ERR_INCOHERENT;
                //if (!Parse_LineGet (&w, &s))
                //  return MEKA_ERR_SYNTAX;
                parse_skip_spaces(&s);
                if (StrIsNull(s))
                    return MEKA_ERR_SYNTAX;
                key_info = KeyInfo_FindByName(s);
                if (key_info != NULL)
                {
                    map->type = INPUT_MAP_TYPE_KEY;
                    map->hw_index = key_info->scancode;
                }
                return MEKA_ERR_OK;
            }

            if (!strcmp(w, "joy"))
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
                map->type = INPUT_MAP_TYPE_JOY_AXIS;
                map->hw_index = stick;
				map->hw_axis = axis;
				map->hw_direction = dir;
                return MEKA_ERR_OK;
            }

            if (!strcmp(w, "joy_button"))
            {
                if (input_src->type != INPUT_SRC_TYPE_JOYPAD)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                map->type = INPUT_MAP_TYPE_JOY_BUTTON;
                map->hw_index = atoi(w);
                return MEKA_ERR_OK;
            }

            if (!strcmp(w, "mouse_button"))
            {
                if (input_src->type != INPUT_SRC_TYPE_MOUSE)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                map->type = INPUT_MAP_TYPE_MOUSE_BUTTON;
                map->hw_index = atoi(w);
                return MEKA_ERR_OK;
            }

            return MEKA_ERR_SYNTAX;
        }

    case 16: // player_x_axis --------------------------------------------------
    case 17: // player_y_axis --------------------------------------------------
        {
            const int map_idx = INPUT_MAP_ANALOG_AXIS_X + var_idx - 16;
			t_input_map_entry* map = &input_src->Map[map_idx];

            if (!(input_src->flags & INPUT_SRC_FLAGS_ANALOG))
                return MEKA_ERR_INCOHERENT;

            if (!strcmp(w, "mouse_axis"))
            {
                if (input_src->type != INPUT_SRC_TYPE_MOUSE)
                    return MEKA_ERR_INCOHERENT;
                if (!parse_getword(w, sizeof(w), &s, " \t", ';', PARSE_FLAGS_NONE))
                    return MEKA_ERR_SYNTAX;
                map->type = input_src->Map[map_idx + 2].type = INPUT_MAP_TYPE_MOUSE_AXIS;
                map->hw_index = input_src->Map[map_idx + 2].hw_index = atoi(w);
                return MEKA_ERR_OK;
            }

            return MEKA_ERR_SYNTAX;
        }

    case 18: // mouse_speed_x --------------------------------------------------
    case 19: // mouse_speed_y --------------------------------------------------
        return MEKA_ERR_OK;

    case 20: // cabinet_mode ---------------------------------------------------
        {
            Inputs.Cabinet_Mode = atoi(w);
            return MEKA_ERR_OK;
        }
    }

    return MEKA_ERR_SYNTAX;
}

void            Load_Inputs_Src_List()
{
    t_input_src *   input_src = NULL;

    // Open and read file
    ConsolePrint(Msg_Get(MSG_Inputs_Src_Loading));
    t_tfile* tf = tfile_read (Inputs.FileName);
    if (tf == NULL)
        Quit_Msg("%s", meka_strerror());
    ConsolePrint("\n");

    // Parse each line
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        char* line = (char*) lines->elem;
        line_cnt += 1;

        if (StrIsNull(line))
            continue;

        if (line[0] == '[' && line[strlen(line) - 1] == ']')
        {
            input_src = Inputs_Sources_Add(StrNDup(line + 1, strlen(line) - 2));
            // ConsolePrintf ("new source --> %s <--\n", CurSrc->Name);
            continue;
        }

		char w[256];
        StrLower(line);
        if (!parse_getword(w, sizeof(w), &line, "=", ';', PARSE_FLAGS_NONE))
            continue;

        for (int i = 0; Inputs_Src_List_KeyWords[i]; i++)
		{
            if (strcmp(w, Inputs_Src_List_KeyWords[i]) == 0)
            {
                // FIXME: this is ugly
                if (input_src == NULL
                    && strcmp(w, "mouse_speed_x") != 0
                    && strcmp(w, "mouse_speed_y") != 0
                    && strcmp(w, "cabinet_mode")  != 0)
                {
                    tfile_free(tf);
                    Quit_Msg(Msg_Get(MSG_Inputs_Src_Missing), line_cnt);
                }

                parse_skip_spaces(&line);
                if (!parse_getword(w, sizeof(w), &line, "", ';', PARSE_FLAGS_NONE))
                {
                    tfile_free(tf);
                    Quit_Msg(Msg_Get(MSG_Inputs_Src_Equal), line_cnt);
                }

                switch (Load_Inputs_Src_Parse_Var(i, w, input_src))
                {
                case MEKA_ERR_SYNTAX:
                    tfile_free(tf);
                    Quit_Msg(Msg_Get(MSG_Inputs_Src_Syntax_Param), line_cnt);
                case MEKA_ERR_INCOHERENT :
                    tfile_free(tf);
                    Quit_Msg(Msg_Get(MSG_Inputs_Src_Inconsistency), line_cnt);
                    break;
                    // FIXME: EMPTY is not handled there
                }
                break;
            }
            if (!Inputs_Src_List_KeyWords [i])
            {
                tfile_free(tf);
                Quit_Msg(Msg_Get(MSG_Inputs_Src_Unrecognized), line_cnt, w);
            }
		}
    }

    // Free file data
    tfile_free(tf);

    // Verify that we have enough inputs sources
    if (Inputs.Sources_Max == 0)
        Quit_Msg("%s", Msg_Get(MSG_Inputs_Src_Not_Enough));
}

void    Write_Inputs_Src_List()
{
	if (!(INP_File = fopen(Inputs.FileName, "wt")))
		return;

	INP_Write_Line (";-----------------------------------------------------------------------------");
	INP_Write_Line ("; " MEKA_NAME " " MEKA_VERSION " - Inputs Configuration");
	INP_Write_Line ("; This file is automatically updated and rewritten when quitting");
	INP_Write_Line ("; Feel free to edit this file manually if you feel the need to.");
	INP_Write_Line ("; However, any comment you may manually add will be deleted!");
	INP_Write_Line (";-----------------------------------------------------------------------------");
	INP_Write_Line ("");

	INP_Write_Line (";-----------------------------------------------------------------------------");
	INP_Write_Line ("; Links:");
	INP_Write_Line ("; Using a SMS/DB-9 controller on your computer:");
	INP_Write_Line (";  - Read the TECH.TXT file at first!");
	INP_Write_Line (";  - SmsCardPad");
	INP_Write_Line (";    http://www.smspower.org/smscartpad/");
	INP_Write_Line (";  - PPJoy joystick driver for Windows 2000/XP");
	INP_Write_Line (";    Download mirror: http://www.zophar.net/joy/ppjoy.html");
	INP_Write_Line (";    Obsolete official website: http://web.archive.org/web/20021018045524/http://www.geocities.com/deonvdw/");
	INP_Write_Line (";  - ArcadeWereld.nl usb controller board");
	INP_Write_Line (";    http://www.smspower.org/forums/15321-SMSDB9ControllerOnPcUsingUSB");
	INP_Write_Line ("; Schematics and drivers for various console controllers adapters:");
	INP_Write_Line (";  - Direct Pad Pro");
	INP_Write_Line (";    http://www.arcadecontrols.com/Mirrors/www.ziplabel.com/dpadpro/");
	INP_Write_Line (";-----------------------------------------------------------------------------");
	INP_Write_Line ("");
	INP_Write_Line (";-----------------------------------------------------------------------------");
	INP_Write_Line ("; Miscellaneous features:");
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
	INP_Write_Line (";   Set to no in order to tell MEKA to ignore input from this device");
	INP_Write_Line ("; player = 1 | 2");
	INP_Write_Line (";   Player number");
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

	for (int i = 0; i < Inputs.Sources_Max; i++)
	{
		char s [256], s2 [256];
		t_input_src *input_src = Inputs.Sources[i];
		sprintf(s, "[%s]", input_src->name);
		INP_Write_Line(s);

		switch (input_src->type)
		{
		case INPUT_SRC_TYPE_KEYBOARD: sprintf(s, "keyboard");  break;
		case INPUT_SRC_TYPE_JOYPAD:   sprintf(s, "joypad");    break;
		case INPUT_SRC_TYPE_MOUSE:    sprintf(s, "mouse");     break;
		}
		INP_Write_Str ("type               ", s);
		if (input_src->type == INPUT_SRC_TYPE_JOYPAD)
			INP_Write_Int  ("connection         ", input_src->Connection_Port + 1);
		INP_Write_Str ("enabled            ", input_src->enabled ? "yes" : "no");
		INP_Write_Int ("player             ", input_src->player + 1);

		for (int j = 0; j < INPUT_MAP_MAX; j++)
		{
			const t_input_map_entry* map = &input_src->Map[j];
			if (map->hw_index == -1)
				continue;
			s[0] = EOSTR;
			if (input_src->flags & INPUT_SRC_FLAGS_DIGITAL)
			{
				switch (j)
				{
				case INPUT_MAP_DIGITAL_UP:    strcpy(s, "player_up          "); break;
				case INPUT_MAP_DIGITAL_DOWN:  strcpy(s, "player_down        "); break;
				case INPUT_MAP_DIGITAL_LEFT:  strcpy(s, "player_left        "); break;
				case INPUT_MAP_DIGITAL_RIGHT: strcpy(s, "player_right       "); break;
				}
			}
			if (input_src->flags & INPUT_SRC_FLAGS_ANALOG)
			{
				switch (j)
				{
				case INPUT_MAP_ANALOG_AXIS_X: strcpy(s, "player_x_axis      "); break;
				case INPUT_MAP_ANALOG_AXIS_Y: strcpy(s, "player_y_axis      "); break;
				}
			}
			if (StrIsNull(s))
			{
				switch (j)
				{
				case INPUT_MAP_BUTTON1:     strcpy(s, "player_button1     "); break;
				case INPUT_MAP_BUTTON2:     strcpy(s, "player_button2     "); break;
				case INPUT_MAP_PAUSE_START: strcpy(s, "player_start_pause "); break;
				case INPUT_MAP_RESET:       strcpy(s, "player_reset       "); break;
				}
			}
			if (StrIsNull(s))
				continue;
			switch (input_src->type)
			{
				// Keyboard -----------------------------------------------------------
			case INPUT_SRC_TYPE_KEYBOARD:
				{
					// Handle special case of the ; key that has to be backslashed
					// Removed because... colon is actually ':', not ';' :)
					// if (n == KEY_COLON)
					//    sprintf(s2, "key \\;");
					//else
					{
						const t_key_info *key_info = KeyInfo_FindByScancode(map->hw_index);
						const char *key_name = key_info ? key_info->name : "error";
						char *key_name_escaped = parse_escape_string(key_name, NULL);
						sprintf(s2, "key %s", key_name_escaped ? key_name_escaped : key_name);
					}
					break;
				}
				// Joypad -------------------------------------------------------------
			case INPUT_SRC_TYPE_JOYPAD:
				switch (map->type)
				{
				case INPUT_MAP_TYPE_JOY_BUTTON:
					sprintf(s2, "joy_button %i", map->hw_index);
					break;
				case INPUT_MAP_TYPE_JOY_AXIS:
					sprintf(s2, "joy stick %i axis %i dir %i", map->hw_index, map->hw_axis, map->hw_direction);
					break;
				}
				break;
				// Mouse --------------------------------------------------------------
			case INPUT_SRC_TYPE_MOUSE:
				switch (map->type)
				{
				case INPUT_MAP_TYPE_MOUSE_BUTTON:
					sprintf(s2, "mouse_button %i", map->hw_index);
					break;
				case INPUT_MAP_TYPE_MOUSE_AXIS:
					sprintf(s2, "mouse_axis %i", map->hw_index);
					break;
				}
				break;
			}
			if (!StrIsNull(s2))
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

