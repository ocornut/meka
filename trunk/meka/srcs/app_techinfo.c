//-----------------------------------------------------------------------------
// MEKA - techinfo.c
// Technical Information Applet - Code
//-----------------------------------------------------------------------------
// TO DO:
//  - re do.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_techinfo.h"
#include "desktop.h"
#include "debugger.h"
#include "g_widget.h"
#include "vdp.h"

//-----------------------------------------------------------------------------

static  char    TechInfo_Lines[TECHINFO_LINES][512 /* TECHINFO_COLUMNS */];

void        TechInfo_Init (void)
{
    int    i;
    
    apps.id.Tech = gui_box_create (150, 382,
        TECHINFO_COLUMNS * Font_TextLength (F_MIDDLE, " "),
        TECHINFO_LINES * Font_Height (F_MIDDLE),
        Msg_Get (MSG_TechInfo_BoxTitle));
    apps.gfx.Tech = create_bitmap (gui.box[apps.id.Tech]->frame.size.x + 1, gui.box[apps.id.Tech]->frame.size.y + 1);
    gui_set_image_box (apps.id.Tech, apps.gfx.Tech);
    gui.box [apps.id.Tech]->update = TechInfo_Update;
    widget_closebox_add (apps.id.Tech, Action_Switch_Tech);
    Desktop_Register_Box ("TECHINFO", apps.id.Tech, 0, &apps.active.Tech);
    // Clear lines
    for (i = 0; i < TECHINFO_LINES; i++)
        strcpy (TechInfo_Lines[i], "");

    // Easter Egg: BreakOut
    {
        t_frame frame;
        frame.pos.x = gui.box[apps.id.Tech]->frame.size.x - 50;
        frame.pos.y = 30;
        frame.size.x = gui.box[apps.id.Tech]->frame.size.x - 38;
        frame.size.y = 40;
        widget_button_add (apps.id.Tech, &frame, 1, BreakOut_Start);
    }
}

// UPDATE ONE LINE ------------------------------------------------------------
void        TechInfo_Update_Line (char *line, byte line_n)
{
    int     h;
    int     y;

    // If line hasn't changed, ignore the update
    if (strcmp (TechInfo_Lines[line_n], line) == 0)
        return;

    // Copy new line
    strcpy (TechInfo_Lines[line_n], line);

    Font_SetCurrent (F_MIDDLE);
    h = Font_Height (-1);
    y = (h * line_n);

    rectfill (apps.gfx.Tech, 0, y, gui.box[apps.id.Tech]->frame.size.x, y + h - 1, GUI_COL_FILL);
    Font_Print (-1, apps.gfx.Tech, line, 4, y, GUI_COL_TEXT_IN_BOX);

    // Set box dirty flag
    gui.box[apps.id.Tech]->must_redraw = YES;
}

// UPDATE TECHNICAL INFORMATIONS APPLET ---------------------------------------
void        TechInfo_Update (void)
{
    char    line [512];
    int     line_n = 0;

    sprintf (line, "   [MODE] %s (%s)", cur_drv->full_name, cur_drv->short_name);
    TechInfo_Update_Line  (line, line_n ++);

    if (cur_drv->id != DRV_NES)
    {
        /* char m[9];
        switch (tsms.VDP_Model)
        {
        case VDP_MODEL_315_5124: sprintf(m, "315-5124"); break;
        case VDP_MODEL_315_5226: sprintf(m, "315-5226"); break;
        case VDP_MODEL_315_5378: sprintf(m, "315-5378"); break;
        case VDP_MODEL_315_5313: sprintf(m, "315-5313"); break;
        } */
        sprintf (line, "    [VDP] " /* "Type:%s - " */ "Mode:%d - VInt:%d - HInt:%d - Display:%s - Status:%02X - Address:%04X",
            /*m,*/ (tsms.VDP_VideoMode), (VBlank_ON?1:0), (HBlank_ON?1:0), (Display_ON?"On":"Off"), sms.VDP_Status, sms.VDP_Address);
            TechInfo_Update_Line  (line, line_n ++);
    }
    else
    {
        char m[3];
        switch (nes->Mirroring)
        {
        case NES_MIRRORING_H:  sprintf(m, "H");  break;
        case NES_MIRRORING_V:  sprintf(m, "V");  break;
        case NES_MIRRORING_4S: sprintf(m, "4S"); break;
        default:               sprintf(m, "No"); break;
        }
        sprintf (line, "    [VDP] VBL:%d - Mirroring:%s - BG/SPR:%s,%s - Status:%02X - Address:%04X",
            (NES_VBlank_ON?1:0), m, (NES_Display_BG?"On":"Off"), (NES_Display_SPR?"On":"Off"), sms.VDP_Status, sms.VDP_Address);
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        sprintf (line, " [SCROLL] Mask_Left:%d - Lock_Top:%s - Lock_Right:%s - X:%02X - Y:%02X",
            (Mask_Left_8?1:0), (Top_No_Scroll?"Yes":"No"), (Right_No_Scroll?"Yes":"No"), sms.VDP[8], sms.VDP[9]);
        TechInfo_Update_Line  (line, line_n ++);
    }
    else
    {
        sprintf (line, " [SCROLL] NT:%d - Mask_Left:%d,%d - X:%02X - Y: %02X",
            (nes->CR0 & NES_CR0_NT_ADDR_MASK), (NES_Mask_Left_BG?1:0), (NES_Mask_Left_SPR?1:0), nes->Scroll[NES_SCROLL_HORIZONTAL], nes->Scroll[NES_SCROLL_VERTICAL]);
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        sprintf (line, "[SPRITES] Doubled:%s - Zoomed: %s - Left_8: %s - Address:%04X - Set:%d",
            (Sprites_8x16?"Yes":"No"), (Sprites_Double?"Yes":"No"), (Sprites_Left_8?"Yes":"No"), (int)(SPR_AREA - VRAM), tgfx.Base_Sprite);
        TechInfo_Update_Line  (line, line_n ++);
    }
    else
    {
        sprintf (line, "[SPRITES] Doubled:%s - Set:%d",
            (NES_Sprites_8x16?"Yes":"No"), ((nes->CR0 & NES_CR0_SPR_PAT)?1:0));
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        sprintf (line, " [INPUTS] PortDE:%02X - Port3F:%02X - Joy:%04X - GG:%02X - Paddles:%02X,%02X",
            (sms.Input_Mode), (tsms.Periph_Nat), tsms.Control[7], (tsms.Control_GG), (Inputs.Paddle_X [PLAYER_1]), (Inputs.Paddle_X [PLAYER_2]));
        TechInfo_Update_Line  (line, line_n ++);
    }

#ifdef MEKA_Z80_DEBUGGER
    if (Debugger.Enabled && Debugger.Active)
        sprintf (line, "[VARIOUS] Country:%d - Border:%d - IPeriod:%d/%d - Lines:%d/%d",
        sms.Country, (sms.VDP[7] & 15), CPU_GetICount(), CPU_GetIPeriod(), tsms.VDP_Line, cur_machine.TV_lines);
    else
#endif
        sprintf (line, "[VARIOUS] Country:%d - Border:%d - IPeriod:%d - Lines:%d",
        sms.Country, (sms.VDP[7] & 15), CPU_GetIPeriod(), cur_machine.TV_lines);
    TechInfo_Update_Line  (line, line_n ++);

    if (cur_drv->id != DRV_NES)
    {
        sprintf(line, "[TMS9918] Back_area:%04X - Back_col:%04X - Back_tile:%04X - Spr_tile:%04X", (int)(BACK_AREA - VRAM), (int)(SG_BACK_COLOR - VRAM), (int)(SG_BACK_TILE - VRAM), (int)(SPR_TILE - VRAM));
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        t_psg *psg = &PSG;
        sprintf (line, "  [SOUND] Tone 0: %03X,%01X  Tone 1: %03X,%01X  Tone 2: %03X,%01X  Noise:%02X,%01X (%s)",
            psg->Registers[0], psg->Registers[1], psg->Registers[2], psg->Registers[3],
            psg->Registers[4], psg->Registers[5], psg->Registers[6], psg->Registers[7],
            ((psg->Registers[6] & 0x04) ? "White" : "Periodic"));
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        sprintf (line, " [MEMORY] Mapper:%d - F0:%02X - F1:%02X - F2:%02X - MapReg:%02X - Pages:[%d/%d][%d/%d]",
            (cur_machine.mapper), (sms.Pages_Reg[0]), (sms.Pages_Reg[1]), (sms.Pages_Reg[2]), (sms.Mapping_Register), (tsms.Pages_Count_8k), (tsms.Pages_Mask_8k), (tsms.Pages_Count_16k), (tsms.Pages_Mask_16k));
        TechInfo_Update_Line  (line, line_n ++);
    }
    else
    {
        sprintf (line, " [MEMORY] Mapper:%d - PRG:%d - CHR:%d - Trainer:%s - SaveRAM:%s",
            NES_Mapper->id, NES_Prg_Cnt, NES_Chr_Cnt, NESHEAD_TRAINER(NES_Header) ? "Yes" : "No", NESHEAD_SAVERAM(NES_Header) ? "Yes" : "No");
        TechInfo_Update_Line  (line, line_n ++);
    }

    if (cur_drv->id != DRV_NES)
    {
        char VDP_f [2] [9];
        Write_Bits_Field (sms.VDP[0], 8, VDP_f[0]);
        Write_Bits_Field (sms.VDP[1], 8, VDP_f[1]);
        sprintf (line, "[UNKNOWN] 00:%d 01:%d 02:%d 12:%d 13:%d 14:%d 17:%d VDP[0]=%s VDP[1]=%s",
            ((sms.VDP[0] & 0x01) ? 1: 0),
            ((sms.VDP[0] & 0x02) ? 1: 0),
            ((sms.VDP[0] & 0x04) ? 1: 0),
            ((sms.VDP[1] & 0x04) ? 1: 0),
            ((sms.VDP[1] & 0x08) ? 1: 0),
            ((sms.VDP[1] & 0x10) ? 1: 0),
            ((sms.VDP[1] & 0x80) ? 1: 0),
            VDP_f [0], VDP_f [1]);
        TechInfo_Update_Line (line, line_n ++);
    }

    // Blank left lines
    while (line_n < TECHINFO_LINES)
        TechInfo_Update_Line ("", line_n ++);
}

//-----------------------------------------------------------------------------

