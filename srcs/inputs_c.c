//-----------------------------------------------------------------------------
// MEKA - inputs_c.c
// Inputs Configuration Applet - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "desktop.h"
#include "g_widget.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define INPUTS_CFG_CHECK_X      (9)
#define INPUTS_CFG_CHECK_Y      (9)

// FIXME: pure crap layouting
#define INPUTS_CFG_FRAME_X      (150)
#define INPUTS_CFG_FRAME_Y      ((GUI_LOOK_FRAME_PAD1_Y + GUI_LOOK_FRAME_PAD2_Y * 2)                     \
                                 + ((INPUT_MAP_MAX + 3) * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y)) \
                                 - GUI_LOOK_LINES_SPACING_Y)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Inputs_CFG_Switch (void)
{
    Inputs_CFG.Active ^= 1;
    gui_box_show (gui.box[Inputs_CFG.ID], Inputs_CFG.Active, TRUE);
    gui_menu_inverse_check (menus_ID.inputs, 7);
}

void    Inputs_CFG_Init_Applet (void)
{
    t_frame frame;

    Inputs_CFG.Active = 0;
    Inputs_CFG.Res_X = 165 + (INPUTS_CFG_FRAME_X + GUI_LOOK_FRAME_SPACING_X);
    Inputs_CFG.Res_Y = 150;
    Inputs_CFG.ID = gui_box_create (307, 282, Inputs_CFG.Res_X, Inputs_CFG.Res_Y, Msg_Get (MSG_Inputs_Configuration_BoxTitle));
    Inputs_CFG.Bmp = create_bitmap (Inputs_CFG.Res_X + 1, Inputs_CFG.Res_Y + 1);
    gui.box[Inputs_CFG.ID]->update = Inputs_CFG_Map_Change_Update;
    gui_set_image_box (Inputs_CFG.ID, Inputs_CFG.Bmp);
    Desktop_Register_Box ("INPUTS", Inputs_CFG.ID, 1, &Inputs_CFG.Active);

    widget_closebox_add (Inputs_CFG.ID, Inputs_CFG_Switch);

    draw_sprite (Inputs_CFG.Bmp, Graphics.Inputs.InputsBase, 10, 18);
    Inputs_CFG_Peripherals_Draw ();
    frame.pos.x = 10;
    frame.pos.y = 2;
    frame.size.x = Graphics.Inputs.InputsBase->w;
    frame.size.y = 80-2;
    widget_button_add (Inputs_CFG.ID, &frame, 1, Inputs_CFG_Peripheral_Change_Handler);

    Inputs_CFG.Current_Map = -1;

    // Emulate Digital check need to be added before drawing source
    // because currently it is drawn on the box before having the chance
    // to be disabled... so anyway drawing source will clear it.
    frame.pos.x = 170;
    frame.pos.y = 19 + (7 * 2) + (2 + 6) * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y);
    frame.size.x = INPUTS_CFG_CHECK_X;
    frame.size.y = INPUTS_CFG_CHECK_Y;
    Inputs_CFG.CheckBox_Emulate_Digital = widget_checkbox_add (Inputs_CFG.ID, &frame, &Inputs_CFG.CheckBox_Emulate_Digital_Value, Inputs_CFG_Emulate_Digital_Handler);
    widget_disable (Inputs_CFG.CheckBox_Emulate_Digital);

    Inputs_CFG.Current_Source = 0;
    Inputs_CFG_Current_Source_Draw ();

    frame.pos.x = 170;
    frame.pos.y = 10-Font_Height(F_MIDDLE)/2;
    frame.size.x = INPUTS_CFG_FRAME_X - 5;
    frame.size.y = Font_Height(F_MIDDLE);
    widget_button_add (Inputs_CFG.ID, &frame, 1, Inputs_CFG_Current_Source_Change);

    frame.pos.x = 170 /* + (INPUTS_CFG_FRAME_X / 2)*/;
    frame.pos.y = 48;
    frame.size.x = (INPUTS_CFG_FRAME_X /* / 2 */) - 10;
    frame.size.y = INPUT_MAP_MAX * (Font_Height(F_SMALL) + GUI_LOOK_LINES_SPACING_Y);
    widget_button_add (Inputs_CFG.ID, &frame, 1, Inputs_CFG_Map_Change_Handler);

    // 'Enabled' checkbox
    frame.pos.x = 170;
    frame.pos.y = 19;
    frame.size.x = INPUTS_CFG_CHECK_X;
    frame.size.y = INPUTS_CFG_CHECK_Y;
    Inputs_CFG.CheckBox_Enabled = widget_checkbox_add (Inputs_CFG.ID, &frame, &Inputs.Sources [Inputs_CFG.Current_Source]->Enabled, NULL);
}

byte    Inputs_CFG_Current_Source_Draw_Map (int i, int Color)
{
    int        x, y;
    char      *MapName;
    char       MapValue[128];
    t_input_src *ISrc = Inputs.Sources [Inputs_CFG.Current_Source];
    t_input_map *Map = &ISrc->Map[i];

    MapName = Inputs_Get_MapName (ISrc->Type, i);
    if (MapName == NULL)
        return NO;

    // Set default font
    Font_SetCurrent (F_SMALL);

    x = 165;
    // Shift Y position by 2 steps for analog devices
    if (ISrc->Result_Type & ANALOG && i > INPUT_MAP_ANALOG_AXIS_Y_REL)
        i -= 2;
    y = 10 + GUI_LOOK_FRAME_PAD1_Y + (2 + i) * (Font_Height(-1) + GUI_LOOK_LINES_SPACING_Y) + 7;

    if (Map->Idx == -1)
        sprintf (MapValue, "<Null>");
    else
        switch (ISrc->Type)
    {
        case INPUT_SRC_KEYBOARD:
            {
                t_key_info *key_info = KeyInfo_FindByScancode (Map->Idx);
                strcpy (MapValue, key_info ? key_info->name : "error");
                break;
            }
        case INPUT_SRC_JOYPAD:
            switch (Map->Type)
            {
            case INPUT_MAP_TYPE_JOY_AXIS:
                sprintf (MapValue, "Stick %d, Axis %d, %c",
                    INPUT_MAP_GET_STICK(Map->Idx),
                    INPUT_MAP_GET_AXIS(Map->Idx),
                    INPUT_MAP_GET_DIR_LR(Map->Idx) ? '+' : '-');
                break;
                // case INPUT_MAP_TYPE_JOY_AXIS_ANAL:
            case INPUT_MAP_TYPE_JOY_BUTTON:
                sprintf (MapValue, "Button %d", Map->Idx);
                break;
            }
            break;
        case INPUT_SRC_MOUSE:
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
    Font_Print (-1, Inputs_CFG.Bmp, MapName, x + GUI_LOOK_FRAME_PAD_X, y, Color);
    Font_Print (-1, Inputs_CFG.Bmp, MapValue, x + (INPUTS_CFG_FRAME_X / 2), y, Color);
    // y += Font_Height() + GUI_LOOK_LINES_SPACING_Y;
    return YES;
}

void    Inputs_CFG_Current_Source_Draw (void)
{
    int             i;
    int             x = 165;
    int             y = 10;
    int             font_height;
    int             frame_x = INPUTS_CFG_FRAME_X;
    int             frame_y = INPUTS_CFG_FRAME_Y;
    t_input_src *   ISrc = Inputs.Sources [Inputs_CFG.Current_Source];

    x = 165;
    y = 10;
    // x = 165 + (i / 2) * (frame_sx + GUI_LOOK_FRAME_SPACING_X);
    // y = 10 + (i % 2) * (frame_sy + GUI_LOOK_FRAME_SPACING_Y);

    // Set update flag
    gui.box [Inputs_CFG.ID]->must_redraw = YES;

    // Set font to use
    Font_SetCurrent (F_MIDDLE);
    font_height = Font_Height (-1);

    // Clear area to display on
    rectfill (Inputs_CFG.Bmp, x, y - font_height / 2,
        x + frame_x, y - font_height / 2 + frame_y,
        GUI_COL_FILL);

    // Do the actual display
    {
        char buf[128];
        sprintf(buf, "%d/%d: %s", Inputs_CFG.Current_Source+1, Inputs.Sources_Max, ISrc->Name);
        gui_rect_titled (Inputs_CFG.Bmp, buf, F_MIDDLE, LOOK_THIN,
            x, y, x + frame_x, y + frame_y,
            GUI_COL_BORDERS, GUI_COL_FILL, /*GUI_COL_TEXT_IN_BOX*/GUI_COL_TEXT_ACTIVE);
    }

    // Set font to use
    Font_SetCurrent (F_SMALL);
    font_height = Font_Height (-1);
    y += GUI_LOOK_FRAME_PAD1_Y;

    // Enable Check
    Font_Print (-1, Inputs_CFG.Bmp, "Enabled", x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, GUI_COL_TEXT_IN_BOX);
    y += font_height + GUI_LOOK_LINES_SPACING_Y;

    // Player
    {
        char PlayerStr [128];
        sprintf (PlayerStr, "Controlling Player %d", ISrc->Player + 1);
        Font_Print (-1, Inputs_CFG.Bmp, PlayerStr, x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, GUI_COL_TEXT_IN_BOX);
        y += font_height + GUI_LOOK_LINES_SPACING_Y;
    }

    // Horizontal Separator
    line (Inputs_CFG.Bmp, x + 4, y + 3, x + frame_x - 4, y + 3, GUI_COL_BORDERS);
    y += 7;

    // Mapping
    for (i = 0; i < INPUT_MAP_MAX; i++)
        if (Inputs_CFG_Current_Source_Draw_Map (i, GUI_COL_TEXT_IN_BOX))
            y += font_height + GUI_LOOK_LINES_SPACING_Y;

    // Quit now if it is not an analog device..
    if (!(ISrc->Result_Type & ANALOG))
        return;

    // Horizontal Separator
    line (Inputs_CFG.Bmp, x + 4, y + 3, x + frame_x - 4, y + 3, GUI_COL_BORDERS);
    y += 7;

    // Emulate Digital
    widget_checkbox_redraw (Inputs_CFG.CheckBox_Emulate_Digital);
    Font_Print (-1, Inputs_CFG.Bmp, "Emulate Joypad", x + GUI_LOOK_FRAME_PAD_X + INPUTS_CFG_CHECK_X + 3, y, GUI_COL_TEXT_IN_BOX);
    y += font_height + GUI_LOOK_LINES_SPACING_Y;
}

void    Inputs_CFG_Current_Source_Change (t_widget *w)
{
    Inputs_CFG.Current_Source = (Inputs_CFG.Current_Source + 1) % Inputs.Sources_Max;
    Inputs_CFG_Current_Source_Draw ();
    widget_checkbox_set_pvalue (Inputs_CFG.CheckBox_Enabled, &Inputs.Sources [Inputs_CFG.Current_Source]->Enabled);
    widget_checkbox_redraw (Inputs_CFG.CheckBox_Enabled);
    if (Inputs_CFG.Current_Map != -1)
        Inputs_CFG_Map_Change_End (); // a bit crap...

    {
        t_input_src *ISrc = Inputs.Sources [Inputs_CFG.Current_Source];
        if (ISrc->Result_Type & ANALOG)
        {
            Inputs_CFG.CheckBox_Emulate_Digital_Value = (ISrc->Result_Type & EMULATE_DIGITAL) ? YES : NO;
            widget_enable (Inputs_CFG.CheckBox_Emulate_Digital);
            widget_checkbox_redraw (Inputs_CFG.CheckBox_Emulate_Digital);
        }
        else
        {
            widget_disable (Inputs_CFG.CheckBox_Emulate_Digital);
        }
    }
}

void        Inputs_CFG_Peripherals_Draw (void)
{
    int     i;
    BITMAP *sprite = NULL;

    // Set update flag
    gui.box [Inputs_CFG.ID]->must_redraw = YES;

    // Set font to use
    Font_SetCurrent (F_SMALL);

    // Clear area to display on
    rectfill (Inputs_CFG.Bmp, 10,  4, 10 + Graphics.Inputs.InputsBase->w,  4 + Font_Height(-1), GUI_COL_FILL);
    rectfill (Inputs_CFG.Bmp, 10, 42, 10 + Graphics.Inputs.InputsBase->w, 105, GUI_COL_FILL);

    // Do the actual display
    for (i = 0; i < PLAYER_MAX; i++)
    {
        // Print name
        char *s = Inputs_Peripheral_Infos [Inputs.Peripheral [i]].Name;
        char buf[128];
        sprintf (buf, "%s", s);
        Font_PrintCentered(-1, Inputs_CFG.Bmp, buf, 
            10 + 11 + (i ? 64 : 0) + (58 / 2), // X
            4, // Y
            GUI_COL_TEXT_IN_BOX);

        // Draw peripheral sprite
        switch (Inputs.Peripheral [i])
        {
            case INPUT_JOYPAD:        sprite = Graphics.Inputs.Joypad;          break;
            case INPUT_LIGHTPHASER:   sprite = Graphics.Inputs.LightPhaser;     break;
            case INPUT_PADDLECONTROL: sprite = Graphics.Inputs.PaddleControl;   break;
            case INPUT_SPORTSPAD:     sprite = Graphics.Inputs.SportsPad;       break;
            case INPUT_TVOEKAKI:      sprite = Graphics.Inputs.TvOekaki;        break;
        }
        if (sprite)
            draw_sprite (Inputs_CFG.Bmp, sprite,
            10 + 11 + (i ? 64 : 0) + (58 - sprite->w) / 2, // X
            42); // Y
    }

    // 3-D Glasses
    if (Glasses.Enabled)
    {
        int x, y;
        BITMAP *b = Graphics.Inputs.Glasses;
        x = 10 + 11 + 64 + (58 - b->w) / 2;
        y = 42 + sprite->h + 5;
        // rectfill (Inputs_CFG.Bmp, x, y, x + b->w, y + b->h, GUI_COL_FILL);
        draw_sprite (Inputs_CFG.Bmp, b, x, y);
    }
}

void    Inputs_CFG_Peripheral_Change_Handler (t_widget *w)
{
    int    Player = (w->mx <= w->frame.size.x / 2) ? 0 : 1; // 0 or 1 depending on the side the widget was clicked on
    Inputs_Peripheral_Next (Player);
}

void    Inputs_CFG_Peripheral_Change (int Player, int Periph)
{
    Inputs.Peripheral [Player] = Periph;
    Inputs_CFG_Peripherals_Draw ();
    Inputs_Peripheral_Change_Update ();
}

void    Inputs_CFG_Map_Change_Handler (t_widget *w)
{
    int            MapIdx = (w->my * 8) / w->frame.size.y;
    t_input_src *  ISrc = Inputs.Sources [Inputs_CFG.Current_Source];

    if (Inputs_CFG.Current_Map != -1)
        return;

    // Note: eating mouse press FIXME
    gui_mouse.pbutton = gui_mouse.button;

    if (ISrc->Result_Type & ANALOG)
    {
        if (MapIdx >= 6)
            return;
        if (MapIdx >= 2)
            MapIdx += 2; // Add two because X_REL/Y_REL are not shown
    }

    switch (ISrc->Type)
    {
    case INPUT_SRC_KEYBOARD:
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Keyboard));
        break;
    case INPUT_SRC_JOYPAD:
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad));
        break;
    case INPUT_SRC_MOUSE:
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
    Set_Mouse_Cursor (5);

    // Be sure nothing is kept highlighted
    if (Inputs_CFG.Current_Map != -1)
        Inputs_CFG_Current_Source_Draw_Map (Inputs_CFG.Current_Map, GUI_COL_TEXT_IN_BOX);

    // Set current map, for the updater
    Inputs_CFG.Current_Map = MapIdx;
    Inputs_CFG_Current_Source_Draw_Map (MapIdx, GUI_COL_TEXT_ACTIVE);
}

void    Inputs_CFG_Map_Change_Update (void)
{
    int           i, j;
    byte          Found;
    t_input_src * ISrc;

    if (Inputs_CFG.Current_Map == -1)
        return;
    Found = NO;
    ISrc = Inputs.Sources [Inputs_CFG.Current_Source];

    if (key[KEY_ESC])
    {
        Found = YES;
        ISrc->Map [Inputs_CFG.Current_Map].Idx = -1;
        key[KEY_ESC] = 0; // Disable the key to avoid it to have an effect now
        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Cancelled));
    }
    else
        // Check if a key/button/axis can be used as the mapping
        switch (ISrc->Type)
    {
        // Keyboard ----------------------------------------------------------------
        case INPUT_SRC_KEYBOARD:
            {
                for (i = 0; i < KEY_MAX; i++)
                    if (key [i])
                    {
                        t_key_info *key_info = KeyInfo_FindByScancode(i);
                        ISrc->Map [Inputs_CFG.Current_Map].Idx = i;
                        ISrc->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_KEY;
                        key[i] = 0; // Disable the key to avoid it to have an effect now
                        Found = YES;
                        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Keyboard_Ok), key_info->name);
                        break;
                    }
                    break;
            }
#ifdef MEKA_JOY
            // Digital Joypad/Joystick -------------------------------------------------
        case INPUT_SRC_JOYPAD:
            {
                JOYSTICK_INFO *joystick;
                poll_joystick(); // It is necessary ?
                joystick = &joy[ISrc->Connection_Port];

                // Check buttons
                for (i = 0; i < joystick->num_buttons; i++)
                    if (joystick->button [i].b)
                    {
                        ISrc->Map [Inputs_CFG.Current_Map].Idx = i;
                        ISrc->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_JOY_BUTTON;
                        joystick->button [i].b = 0; // Disable the button to avoid..
                        Found = YES;
                        Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad_Ok_B), i);
                        break;
                    }
                    if (Found)
                        break;

                    // Check axis
                    for (i = 0; i < joystick->num_sticks; i++)
                    {
                        JOYSTICK_STICK_INFO *stick = &joystick->stick[i];
                        // Msg (MSGT_DEBUG, "stick %d, flags=%04X", i, stick->flags);
                        for (j = 0; j < stick->num_axis; j++)
                        {
                            JOYSTICK_AXIS_INFO *axis = &stick->axis[j];
                            // Msg (MSGT_DEBUG, "- axis %d - pos %d - d1 %d - d2 %d\n", j, axis->pos, axis->d1, axis->d2);
                            if (axis->d1 || axis->d2)
                            {
                                ISrc->Map [Inputs_CFG.Current_Map].Idx = MAKE_STICK_AXIS_DIR (i, j, (axis->d1 ? 0 : 1));
                                ISrc->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_JOY_AXIS;
                                Found = YES;
                                // Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_Inputs_Src_Map_Joypad_Ok_A), i, j, (axis->d1 ? '-' : '+'));
                                axis->d1 = axis->d2 = 0; // Need to be done on last line
                                break;
                            }
                        }
                        if (Found)
                            break;
                    }
                    break;
            }
#endif // #ifdef MEKA_JOY
            // Mouse -------------------------------------------------------------------
        case INPUT_SRC_MOUSE:
            {
                // Buttons
                if (Inputs_CFG.Current_Map >= INPUT_MAP_BUTTON1)
                {
                    int n = -1;
                    if ((gui_mouse.button & 1) && !(gui_mouse.pbutton & 1))
                        n = 0;
                    else if ((gui_mouse.button & 2) && !(gui_mouse.pbutton & 2))
                        n = 1;
                    else if ((gui_mouse.button & 4) && !(gui_mouse.pbutton & 4))
                        n = 2;
                    if (n != -1)
                    {
                        ISrc->Map [Inputs_CFG.Current_Map].Idx = n;
                        ISrc->Map [Inputs_CFG.Current_Map].Type = INPUT_MAP_TYPE_MOUSE_BUTTON;
                        gui_mouse.pbutton = gui_mouse.button; // Note: eating mouse press FIXME
                        Found = YES;
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
                ISrc->Map [Inputs_CFG.Current_Map].Idx = ISrc->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (0);
                ISrc->Map [Inputs_CFG.Current_Map].Type = ISrc->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
                save_mouse[0] = -1;
                Found = YES;
                break;
                }
                if (mouse_y != save_mouse[1] && save_mouse[1] != -1)
                {
                ISrc->Map [Inputs_CFG.Current_Map].Idx = ISrc->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (1);
                ISrc->Map [Inputs_CFG.Current_Map].Type = ISrc->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
                save_mouse[1] = -1;
                Found = YES;
                break;
                }
                if (mouse_z != save_mouse[2] && save_mouse[2] != -1)
                {
                ISrc->Map [Inputs_CFG.Current_Map].Idx = ISrc->Map [Inputs_CFG.Current_Map + 2].Idx = MAKE_AXIS (2);
                ISrc->Map [Inputs_CFG.Current_Map].Type = ISrc->Map [Inputs_CFG.Current_Map + 2].Type = INPUT_MAP_TYPE_MOUSE_AXIS;
                save_mouse[2] = -1;
                Found = YES;
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

    if (Found == NO)
        return;
    Inputs_CFG_Map_Change_End ();
}

void    Inputs_CFG_Map_Change_End (void)
{
    // Need to restore cursor.
    // FIXME: the method sucks! need to sort those functions anyway.
    Inputs_Peripheral_Change_Update ();

    // Refresh current source after changing mapping
    Inputs_CFG_Current_Source_Draw ();
    widget_checkbox_set_pvalue (Inputs_CFG.CheckBox_Enabled, &Inputs.Sources [Inputs_CFG.Current_Source]->Enabled);
    widget_checkbox_redraw (Inputs_CFG.CheckBox_Enabled);

    // Set current map back to -1
    Inputs_CFG.Current_Map = -1;
}

void    Inputs_CFG_Emulate_Digital_Handler (t_widget *w)
{
    t_input_src *ISrc = Inputs.Sources [Inputs_CFG.Current_Source];
    if (!(ISrc->Result_Type & ANALOG))
        return;
    if (Inputs_CFG.CheckBox_Emulate_Digital_Value)
        ISrc->Result_Type |= EMULATE_DIGITAL;
    else
        ISrc->Result_Type &= ~EMULATE_DIGITAL;
}

//-----------------------------------------------------------------------------

