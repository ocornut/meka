//-----------------------------------------------------------------------------
// MEKA - inputs_c.c
// Inputs Configuration Applet - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_tools.h"
#include "g_widget.h"
#include "keyinfo.h"
#include "inputs_c.h"
#include "inputs_t.h"
#include "glasses.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_app_inputs_config     Inputs_CFG;

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

static void Inputs_CFG_Layout(t_app_inputs_config *app, bool setup);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define INPUTS_CFG_CHECK_X      (9)
#define INPUTS_CFG_CHECK_Y      (9)

// FIXME: pure crap layouting
// FIXME: Actually, values below are not size of the whole inputs configuration applet
#define INPUTS_CFG_FRAME_X      (150)
#define INPUTS_CFG_FRAME_Y      ((GUI_LOOK_FRAME_PAD1_Y + GUI_LOOK_FRAME_PAD2_Y * 2)                     \
                                 + ((INPUT_MAP_MAX + 3) * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y)) \
                                 - GUI_LOOK_LINES_SPACING_Y)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_CFG_Switch (void)
{
    Inputs_CFG.active ^= 1;
    gui_box_show (Inputs_CFG.box, Inputs_CFG.active, TRUE);
    gui_menu_inverse_check (menus_ID.inputs, 7);
}

void    Inputs_CFG_Init_Applet (void)
{
    t_app_inputs_config *app = &Inputs_CFG; // Global instance
    t_frame frame;

    // Setup members
    app->active = FALSE;
    app->Current_Map = -1;
    app->Current_Source = 0;

    frame.pos.x     = 420;
    frame.pos.y     = 408;
    frame.size.x    = 165 + (INPUTS_CFG_FRAME_X + GUI_LOOK_FRAME_SPACING_X);
    frame.size.y    = 150;
    app->box = gui_box_new(&frame, Msg_Get(MSG_Inputs_Config_BoxTitle));
    Desktop_Register_Box ("INPUTS", app->box, 1, &app->active);

    // Layout
    Inputs_CFG_Layout(app, TRUE);
}

static void Inputs_CFG_Layout(t_app_inputs_config *app, bool setup)
{
    t_frame frame;

	al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    if (setup)
    {
        // Add closebox widget
        widget_closebox_add(app->box, (t_widget_callback)Inputs_CFG_Switch);

        // Peripheral change button
        frame.pos.x = 10;
        frame.pos.y = 18;
        frame.size.x = al_get_bitmap_width(Graphics.Inputs.InputsBase);
        frame.size.y = 80-2;
        widget_button_add(app->box, &frame, 1, (t_widget_callback)Inputs_CFG_Peripheral_Change_Handler, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // Input source change button
        frame.pos.x = 170;
        frame.pos.y = 10-Font_Height(F_MIDDLE)/2;
        frame.size.x = INPUTS_CFG_FRAME_X - 5;
        frame.size.y = Font_Height(F_MIDDLE);
        widget_button_add(app->box, &frame, 1|2, (t_widget_callback)Inputs_CFG_Current_Source_Change, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // Input map change button
        frame.pos.x = 170 /* + (INPUTS_CFG_FRAME_X / 2)*/;
        frame.pos.y = 48;
        frame.size.x = (INPUTS_CFG_FRAME_X /* / 2 */) - 10;
        frame.size.y = INPUT_MAP_MAX * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y);
        widget_button_add(app->box, &frame, 1, (t_widget_callback)Inputs_CFG_Map_Change_Handler, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // 'Enabled' checkbox
        frame.pos.x = 170;
        frame.pos.y = 19;
        frame.size.x = INPUTS_CFG_CHECK_X;
        frame.size.y = INPUTS_CFG_CHECK_Y;
        app->CheckBox_Enabled = widget_checkbox_add(app->box, &frame, &Inputs.Sources [app->Current_Source]->enabled, NULL);

        // Emulate Digital check need to be added before drawing source
        // because currently it is drawn on the box before having the chance
        // to be disabled... so anyway drawing source will clear it.
        frame.pos.x = 170;
        frame.pos.y = 19 + (7 * 2) + (2 + 6) * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y);
        frame.size.x = INPUTS_CFG_CHECK_X;
        frame.size.y = INPUTS_CFG_CHECK_Y;
        app->CheckBox_Emulate_Digital = widget_checkbox_add(app->box, &frame, &app->CheckBox_Emulate_Digital_Value, (t_widget_callback)Inputs_CFG_Emulate_Digital_Handler);
        widget_disable(app->CheckBox_Emulate_Digital);
    }

    // Draw input base
	al_draw_bitmap(Graphics.Inputs.InputsBase, 10, 34, 0x0000);

    // Draw current peripheral
    Inputs_CFG_Peripherals_Draw();

    // Draw current input source
    Inputs_CFG_Current_Source_Draw ();
}

byte        Inputs_CFG_Current_Source_Draw_Map (int i, ALLEGRO_COLOR Color)
{
    t_app_inputs_config *app = &Inputs_CFG; // Global instance

    int         x, y;
    char *      MapName;
    char        MapValue[128];
    t_input_src *input_src = Inputs.Sources [Inputs_CFG.Current_Source];
    t_input_map *Map = &input_src->Map[i];

    MapName = Inputs_Get_MapName (input_src->type, i);
    if (MapName == NULL)
        return FALSE;

    // Set default font
    Font_SetCurrent(F_SMALL);

    x = 165;
    // Shift Y position by 2 steps for analog devices
    if (input_src->flags & INPUT_SRC_FLAGS_ANALOG && i > INPUT_MAP_ANALOG_AXIS_Y_REL)
        i -= 2;
    y = 10 + GUI_LOOK_FRAME_PAD1_Y + (2 + i) * (Font_Height(F_CURRENT) + GUI_LOOK_LINES_SPACING_Y) + 7;

    if (Map->Idx == -1)
	{
        sprintf (MapValue, "<Null>");
	}
    else
    {
        switch (input_src->type)
        {
        case INPUT_SRC_TYPE_KEYBOARD:
            {
                const t_key_info *key_info = KeyInfo_FindByScancode (Map->Idx);
                strcpy (MapValue, key_info ? key_info->name : "error");
                break;
            }
        case INPUT_SRC_TYPE_JOYPAD:
            switch (Map->Type)
            {
            case INPUT_MAP_TYPE_JOY_AXIS:
                sprintf (MapValue, "Stick %d, Axis %d, %c",
                    INPUT_MAP_GET_STICK(Map->Idx),
                    INPUT_MAP_GET_AXIS(Map->Idx),
                    INPUT_MAP_GET_DIR_LR(Map->Idx) ? '+' : '-');
                break;
            case INPUT_MAP_TYPE_JOY_BUTTON:
                sprintf (MapValue, "Button %d", Map->Idx);
                break;
            }
            break;
        case INPUT_SRC_TYPE_MOUSE:
            switch (Map->Type)
            {
            case INPUT_MAP_TYPE_MOUSE_AXIS:
                sprintf (MapValue, "Axis %d (%c)",
                    INPUT_MAP_GET_AXIS(Map->Idx),
                    'X' + INPUT_MAP_GET_AXIS(Map->Idx));
                break;
            case INPUT_MAP_TYPE_MOUSE_BUTTON:
                sprintf (MapValue, "Button %d", Map->Idx+1);
                break;
            }
            break;
        }
    }

	al_set_target_bitmap(app->box->gfx_buffer);
    Font_Print(F_CURRENT, MapName, x + GUI_LOOK_FRAME_PAD_X, y, Color);
    Font_Print(F_CURRENT, MapValue, x + (INPUTS_CFG_FRAME_X / 2), y, Color);
    // y += Font_Height() + GUI_LOOK_LINES_SPACING_Y;

    // Set both checkbox widgets as dirty (because we drawn over them during the clear)
    widget_set_dirty(app->CheckBox_Enabled);
    widget_set_dirty(app->CheckBox_Emulate_Digital);

    return TRUE;
}

void    Inputs_CFG_Current_Source_Draw (void)
{
    t_app_inputs_config *app = &Inputs_CFG; // Global instance
    ALLEGRO_BITMAP *bmp = app->box->gfx_buffer;

    int             x = 165;
    int             y = 10;
    int             font_height;
    int             frame_x = INPUTS_CFG_FRAME_X;
    int             frame_y = INPUTS_CFG_FRAME_Y;
    t_input_src *   input_src = Inputs.Sources [Inputs_CFG.Current_Source];

    // x = 165 + (i / 2) * (frame_sx + GUI_LOOK_FRAME_SPACING_X);
    // y = 10 + (i % 2) * (frame_sy + GUI_LOOK_FRAME_SPACING_Y);

    // Set update flag
    app->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Set font to use
    Font_SetCurrent(F_MIDDLE);
    font_height = Font_Height();

    // Clear area to display on
	al_set_target_bitmap(bmp);
    al_draw_filled_rectangle(x, y - font_height / 2,
        x + frame_x + 1, y - font_height / 2 + frame_y + 1,
        COLOR_SKIN_WINDOW_BACKGROUND);

    // Do the actual display
    {
        char buf[128];
        sprintf(buf, "%d/%d: %s >>", app->Current_Source+1, Inputs.Sources_Max, input_src->name);
        gui_rect_titled (bmp, buf, F_MIDDLE, LOOK_THIN,
            x, y, x + frame_x, y + frame_y,
            COLOR_SKIN_WIDGET_GENERIC_BORDER, COLOR_SKIN_WINDOW_BACKGROUND, /*COLOR_SKIN_WINDOW_TEXT*/COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT);
    }

    // Set font to use
    Font_SetCurrent (F_SMALL);
    font_height = Font_Height();
    y += GUI_LOOK_FRAME_PAD1_Y;

    // Enable Check
    Font_Print(F_CURRENT, Msg_Get(MSG_Inputs_Config_Source_Enabled), x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, COLOR_SKIN_WINDOW_TEXT);
    y += font_height + GUI_LOOK_LINES_SPACING_Y;

    // Player
    char buf[64];
	snprintf(buf, sizeof(buf), Msg_Get(MSG_Inputs_Config_Source_Player), input_src->player + 1);
	Font_Print(F_CURRENT, buf, x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, COLOR_SKIN_WINDOW_TEXT);
	y += font_height + GUI_LOOK_LINES_SPACING_Y;

    // Horizontal Separator
    al_draw_line(x + 4, y + 3 + 0.5f, x + frame_x - 4 + 1, y + 3 + 0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);
    y += 7;

    // Mapping
    for (int i = 0; i < INPUT_MAP_MAX; i++)
        if (Inputs_CFG_Current_Source_Draw_Map (i, COLOR_SKIN_WINDOW_TEXT))
            y += font_height + GUI_LOOK_LINES_SPACING_Y;

    // Quit now if it is not an analog device..
    if (!(input_src->flags & INPUT_SRC_FLAGS_ANALOG))
        return;

    // Horizontal Separator
    al_draw_line(x + 4, y + 3 + 0.5f, x + frame_x - 4, y + 3 + 0.5f, COLOR_SKIN_WINDOW_SEPARATORS, 0);
    y += 7;

    // Emulate Digital
    widget_checkbox_redraw (app->CheckBox_Emulate_Digital);
    Font_Print(F_CURRENT, Msg_Get(MSG_Inputs_Config_Source_Emulate_Joypad), x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, COLOR_SKIN_WINDOW_TEXT);
    y += font_height + GUI_LOOK_LINES_SPACING_Y;
}

void    Inputs_CFG_Current_Source_Change (t_widget *w)
{
	t_app_inputs_config *app = &Inputs_CFG;   // Global instance

	if (w->mouse_buttons_activation & 1)
	{
		app->Current_Source++;
		if (app->Current_Source == Inputs.Sources_Max)
			app->Current_Source = 0;
	}
	else if (w->mouse_buttons_activation & 2)
	{
		if (app->Current_Source == 0)
			app->Current_Source = Inputs.Sources_Max;
		app->Current_Source--;
	}

    Inputs_CFG_Current_Source_Draw ();
    widget_checkbox_set_pvalue (app->CheckBox_Enabled, &Inputs.Sources [app->Current_Source]->enabled);
    widget_checkbox_redraw (app->CheckBox_Enabled);
    if (app->Current_Map != -1)
        Inputs_CFG_Map_Change_End (); // a bit crap...

    {
        t_input_src *input_src = Inputs.Sources [app->Current_Source];
        if (input_src->flags & INPUT_SRC_FLAGS_ANALOG)
        {
            app->CheckBox_Emulate_Digital_Value = (input_src->flags & INPUT_SRC_FLAGS_EMULATE_DIGITAL) ? TRUE : FALSE;
            widget_enable (app->CheckBox_Emulate_Digital);
            widget_checkbox_redraw (app->CheckBox_Emulate_Digital);
        }
        else
        {
            widget_disable (app->CheckBox_Emulate_Digital);
        }
    }
}

void        Inputs_CFG_Peripherals_Draw (void)
{
    t_app_inputs_config *app = &Inputs_CFG; // Global instance
    ALLEGRO_BITMAP *bmp = app->box->gfx_buffer;

    int     i;
    ALLEGRO_BITMAP *sprite = NULL;

    // Set update flag
    app->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Set font to use
    Font_SetCurrent (F_SMALL);

    // Clear area to display on
	al_set_target_bitmap(bmp);
    al_draw_filled_rectangle(10, 20, 10 + al_get_bitmap_width(Graphics.Inputs.InputsBase) + 1, 20 + Font_Height() + 1, COLOR_SKIN_WINDOW_BACKGROUND);
    al_draw_filled_rectangle(10, 58, 10 + al_get_bitmap_width(Graphics.Inputs.InputsBase) + 1, 121 + 1, COLOR_SKIN_WINDOW_BACKGROUND);

    // Print 'click to select peripheral' message
    Font_PrintCentered(F_CURRENT, Msg_Get(MSG_Inputs_Config_Peripheral_Click), 
        10 + 11 + (64 / 2) + (58 / 2),
        4, COLOR_SKIN_WINDOW_TEXT);

    // Do the actual display
    for (i = 0; i < PLAYER_MAX; i++)
    {
        // Print name
        const char *name = Inputs_Peripheral_Infos [Inputs.Peripheral [i]].name;
        Font_PrintCentered(F_CURRENT, name, 
            10 + 11 + (i ? 64 : 0) + (58 / 2), // X
            20, // Y
            COLOR_SKIN_WINDOW_TEXT);

        // Draw peripheral sprite
        switch (Inputs.Peripheral [i])
        {
            case INPUT_JOYPAD:        sprite = Graphics.Inputs.Joypad;          break;
            case INPUT_LIGHTPHASER:   sprite = Graphics.Inputs.LightPhaser;     break;
            case INPUT_PADDLECONTROL: sprite = Graphics.Inputs.PaddleControl;   break;
            case INPUT_SPORTSPAD:     sprite = Graphics.Inputs.SportsPad;       break;
            case INPUT_TVOEKAKI:      sprite = Graphics.Inputs.TvOekaki;        break;
        }
        if (sprite != NULL)
        {
            al_draw_bitmap(sprite,
				10 + 11 + (i ? 64 : 0) + (58 - al_get_bitmap_width(sprite)) / 2, // X
				58, 0x0000);
        }
    }

    // 3-D Glasses
    // Draw below player 2 peripheral
    if (Glasses.Enabled)
    {
        ALLEGRO_BITMAP *b = Graphics.Inputs.Glasses;
        const int x = 10 + 11 + 64 + (58 - al_get_bitmap_width(b)) / 2;
        const int y = 58 + al_get_bitmap_height(sprite) + 5;
        // rectfill (bmp, x, y, x + b->w, y + b->h, COLOR_SKIN_WINDOW_BACKGROUND);
        al_draw_bitmap(b, x, y, 0x0000);
    }
}

void    Inputs_CFG_Peripheral_Change_Handler (t_widget *w)
{
    const int player = (w->mouse_x <= w->frame.size.x / 2) ? 0 : 1; // 0 or 1 depending on the side the widget was clicked on
    Inputs_Peripheral_Next (player);
}

void    Inputs_CFG_Peripheral_Change (int Player, int Periph)
{
    Inputs.Peripheral [Player] = Periph;
    Inputs_CFG_Peripherals_Draw ();
    Inputs_Peripheral_Change_Update ();
}

void    Inputs_CFG_Map_Change_Handler (t_widget *w)
{
	t_app_inputs_config *app = &Inputs_CFG; // Global instance

	int            MapIdx = (w->mouse_y * 8) / w->frame.size.y;
    t_input_src *  input_src = Inputs.Sources [app->Current_Source];

    if (app->Current_Map != -1)
        return;

    // Note: eating mouse press FIXME
    gui.mouse.buttons_prev = gui.mouse.buttons;

    if (input_src->flags & INPUT_SRC_FLAGS_ANALOG)
    {
        if (MapIdx >= 6)
            return;
        if (MapIdx >= 2)
            MapIdx += 2; // Add two because X_REL/Y_REL are not shown
    }

    switch (input_src->type)
    {
    case INPUT_SRC_TYPE_KEYBOARD:
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Keyboard));
        break;
    case INPUT_SRC_TYPE_JOYPAD:
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad));
        break;
    case INPUT_SRC_TYPE_MOUSE:
        if (MapIdx < 4)
        {
            Msg (MSGT_USER, Msg_Get (MSG_Inputs_Src_Map_Mouse_No_A));
            return;
        }
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Mouse));
        break;
    default:
        Msg (MSGT_USER, "Error #24813R");
        return;
    }

    // Change cursor to the '...' one
    Inputs_SetMouseCursor(MEKA_MOUSE_CURSOR_WAIT);

    // Be sure nothing is kept highlighted
    if (app->Current_Map != -1)
        Inputs_CFG_Current_Source_Draw_Map (app->Current_Map, COLOR_SKIN_WINDOW_TEXT);

    // Set current map, for the updater
    app->Current_Map = MapIdx;
    Inputs_CFG_Current_Source_Draw_Map (MapIdx, COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT);
}

void    Inputs_CFG_Update(t_app_inputs_config *app)
{
    // Skip update if not active
    if (!Inputs_CFG.active)
        return;

    // If skin has changed, redraw everything
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        Inputs_CFG_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
        app->dirty = TRUE;
    }

    // Update input map change (if any occuring)
    Inputs_CFG_Map_Change_Update();

    // Skip redraw if not dirty
    if (!app->dirty)
        return;

    // Redraw
    app->dirty = FALSE;
    app->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // ...
}

void    Inputs_CFG_Map_Change_Update (void)
{
    t_app_inputs_config *app = &Inputs_CFG; // Global instance

    int           i;
    bool          found;
    t_input_src * input_src;

    if (Inputs_CFG.Current_Map == -1)
        return;

    found = FALSE;
    input_src = Inputs.Sources[Inputs_CFG.Current_Source];

    // Pressing ESC cancel map change
	// Eat the keypress to avoid it having side effect of switching to fullscreen mode
    if (Inputs_KeyPressed(ALLEGRO_KEY_ESCAPE, true))
    {
        found = TRUE;
        input_src->Map [Inputs_CFG.Current_Map].Idx = -1;
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Cancelled));
        Inputs_CFG_Map_Change_End ();
        return;
    }

    // Check if a key/button/axis can be used as the mapping
    switch (input_src->type)
    {
        // Keyboard ----------------------------------------------------------------
    case INPUT_SRC_TYPE_KEYBOARD:
        {
            for (i = 0; i < ALLEGRO_KEY_MAX; i++)
			{
				// Eat keypresses to avoid them having a side effect with the GUI or game
                if (Inputs_KeyPressed(i, true))
                {
                    const t_key_info *key_info = KeyInfo_FindByScancode(i);
                    if (key_info)
                    {
                        input_src->Map [Inputs_CFG.Current_Map].Idx = i;
                        input_src->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_KEY;
                        found = TRUE;
                        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Keyboard_Ok), key_info->name);
                    }
                    else
                    {
                        Msg (MSGT_DEBUG, "KeyInfo_FindByScancode(%x) failed", i);
                    }
                    break;
                }
			}
            break;
        }
#ifdef MEKA_JOY
        // Digital Joypad/Joystick -------------------------------------------------
    case INPUT_SRC_TYPE_JOYPAD:
        {
			ALLEGRO_JOYSTICK *joystick = al_get_joystick(input_src->Connection_Port);
			if (!joystick)
				break;

			ALLEGRO_JOYSTICK_STATE state;
			al_get_joystick_state(joystick, &state);

            // Check buttons
			const int num_buttons = al_get_joystick_num_buttons(joystick);
            for (i = 0; i < num_buttons; i++)
			{
				if (state.button[i])
                {
                    input_src->Map [Inputs_CFG.Current_Map].Idx = i;
                    input_src->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_JOY_BUTTON;
					found = TRUE;
                    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad_Ok_B), i);
                    break;
                }
			}
			if (found)
				break;

            // Check axis
			const int num_sticks = al_get_joystick_num_sticks(joystick);
            for (i = 0; i < num_sticks; i++)
            {
				const int num_axes = al_get_joystick_num_axes(joystick, i);
                for (int j = 0; j < num_axes; j++)
                {
                    const float axis_pos = state.stick[i].axis[j];
					//Msg (MSGT_DEBUG, "- axis %d - pos %f", j, axis_pos);
					if (axis_pos > INPUT_JOY_DEADZONE || axis_pos < -INPUT_JOY_DEADZONE)
                    {
                        input_src->Map [Inputs_CFG.Current_Map].Idx = MAKE_STICK_AXIS_DIR (i, j, (axis_pos > 0.0f ? 1 : 0));
                        input_src->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_JOY_AXIS;
                        found = TRUE;
                        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad_Ok_A), i, j, (axis_pos > 0.0f ? '+' : '-'));
                        break;
                    }
                }
                if (found)
                    break;
            }
            break;
        }
#endif // #ifdef MEKA_JOY
        // Mouse -------------------------------------------------------------------
    case INPUT_SRC_TYPE_MOUSE:
        {
            // Buttons
            if (Inputs_CFG.Current_Map >= INPUT_MAP_BUTTON1)
            {
                int n = -1;
                if ((gui.mouse.buttons & 1) && !(gui.mouse.buttons_prev & 1))
                    n = 0;
                else if ((gui.mouse.buttons & 2) && !(gui.mouse.buttons_prev & 2))
                    n = 1;
                else if ((gui.mouse.buttons & 4) && !(gui.mouse.buttons_prev & 4))
                    n = 2;
                if (n != -1)
                {
                    input_src->Map [Inputs_CFG.Current_Map].Idx = n;
                    input_src->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_MOUSE_BUTTON;
                    gui.mouse.buttons_prev = gui.mouse.buttons; // Note: eating mouse press FIXME
                    found = TRUE;
                    Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Mouse_Ok_B), n+1);
                    break;
                }
            }
            // Axis
            /*
            if (Inputs_CFG.Current_Map <= INPUT_MAP_ANALOG_AXIS_Y)
            {
            static int save_mouse[3] = { -1, -1, -1 };
            if (mouse_x != save_mouse[0] && save_mouse[0] != -1)
            {
            input_src->Map [Inputs_CFG.Current_Map].Idx = input_src->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (0);
            input_src->Map [Inputs_CFG.Current_Map].Type = input_src->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
            save_mouse[0] = -1;
            found = TRUE;
            break;
            }
            if (mouse_y != save_mouse[1] && save_mouse[1] != -1)
            {
            input_src->Map [Inputs_CFG.Current_Map].Idx = input_src->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (1);
            input_src->Map [Inputs_CFG.Current_Map].Type = input_src->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
            save_mouse[1] = -1;
            found = TRUE;
            break;
            }
            if (mouse_z != save_mouse[2] && save_mouse[2] != -1)
            {
            input_src->Map [Inputs_CFG.Current_Map].Idx = input_src->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (2);
            input_src->Map [Inputs_CFG.Current_Map].Type = input_src->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
            save_mouse[2] = -1;
            found = TRUE;
            break;
            }
            save_mouse[0] = mouse_x;
            save_mouse[1] = mouse_y;
            save_mouse[2] = mouse_z;
            }
            */
            break;
        }
    }

    if (!found)
        return;

    Inputs_CFG_Map_Change_End ();
    app->dirty = TRUE;
}

void    Inputs_CFG_Map_Change_End (void)
{
    // Need to restore cursor.
    // FIXME: the method sucks! need to sort those functions anyway.
    Inputs_Peripheral_Change_Update ();

    // Refresh current source after changing mapping
    Inputs_CFG_Current_Source_Draw ();
    widget_checkbox_set_pvalue (Inputs_CFG.CheckBox_Enabled, &Inputs.Sources [Inputs_CFG.Current_Source]->enabled);
    //widget_checkbox_redraw (Inputs_CFG.CheckBox_Enabled);

    // Set current map back to -1
    Inputs_CFG.Current_Map = -1;
}

void    Inputs_CFG_Emulate_Digital_Handler(t_widget *w)
{
    t_input_src *input_src = Inputs.Sources [Inputs_CFG.Current_Source];
    if (!(input_src->flags & INPUT_SRC_FLAGS_ANALOG))
        return;

    if (Inputs_CFG.CheckBox_Emulate_Digital_Value)
        input_src->flags |= INPUT_SRC_FLAGS_EMULATE_DIGITAL;
    else
        input_src->flags &= ~INPUT_SRC_FLAGS_EMULATE_DIGITAL;
}

//-----------------------------------------------------------------------------

