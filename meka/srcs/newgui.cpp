#include "shared.h"
#include "newgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_memory_editor.h"
#include "backends/imgui_impl_allegro5.h"
#include "IconsFontAwesome5.h"  // See https://fontawesome.com/icons?d=gallery

#include "app_cheatfinder.h"
#include "app_filebrowser.h"
#include "app_game.h"
#include "app_mapview.h"
#include "app_memview.h"
#include "app_textview.h"
#include "app_tileview.h"
#include "blitintf.h"
#include "capture.h"
#include "datadump.h"
#include "debugger.h"
#include "file.h"
#include "fskipper.h"
#include "glasses.h"
#include "inputs_c.h"
#include "palette.h"
#include "rapidfir.h"
#include "saves.h"
#include "sk1100.h"
#include "tvtype.h"
#include "vmachine.h"
#include "vdp.h"
#include "sound/fmunit.h"
#include "sound/psg.h"
#include "sound/s_misc.h"
#include "sound/sound_logging.h"

// FIXME-IMGUI: Skinning: font, colors, etc.

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_newgui    g_newgui;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

static void NewGui_InitImGui()
{
    // Create Dear ImGui context
    ConsolePrintf("Initializing dear imgui %s...\n", IMGUI_VERSION);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

    // Setup Dear ImGui fonts
    {
        ImFontConfig icon_font_cfg;
        icon_font_cfg.MergeMode = true;
        icon_font_cfg.GlyphMinAdvanceX = 20.0f;
        //icon_font_cfg.OversampleH = icon_font_cfg.OversampleV = 1.0f;
        //static ImWchar icon_font_ranges[] = { ICON_MIN_FA , ICON_MAX_FA, 0 };

        Str128 filename;
        //io.Fonts->AddFontDefault();
        filename.setf("%s/Data/fonts/DroidSansMono.ttf", g_env.Paths.EmulatorDirectory);
        io.Fonts->AddFontFromFileTTF(filename.c_str(), 20.0f);

        filename.setf("%s/Data/fonts/Font Awesome 5 Free-Solid-900.otf", g_env.Paths.EmulatorDirectory);
        io.Fonts->AddFontFromFileTTF(filename.c_str(), 13.0f, &icon_font_cfg);// , icon_font_ranges);
    }

    NewGui_InitStyle();

    // Setup Dear ImGui bindings
    ImGui_ImplAllegro5_Init(NULL);
}

static void NewGui_InitApplets()
{
    t_newgui* ng = &g_newgui;

    // Log
    ng->log_data = new ImGuiTextBuffer();
    if (!ng->log_filename.empty())
    {
        ng->log_file = fopen(ng->log_filename.c_str(), "a+t");
        if (ng->log_file)
            fprintf(ng->log_file, Msg_Get(MSG_Log_Session_Start), meka_date_getf());
    }
}

void    NewGui_Init()
{
    NewGui_InitImGui();
    NewGui_InitApplets();
}

static void NewGui_CloseImGui()
{
    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
}

static void NewGui_CloseApplets()
{
    t_newgui* ng = &g_newgui;

    if (ng->log_file)
    {
        fclose(ng->log_file);
        ng->log_file = NULL;
        ng->log_data->clear();
    }
    delete ng->log_data;
    ng->log_data = NULL;
}

void    NewGui_Close()
{
    NewGui_CloseApplets();
    NewGui_CloseImGui();
}

//-----------------------------------------------------------------------------
// APPLET: Game
//-----------------------------------------------------------------------------

static void NewGui_GameDraw()
{
    // FIXME-IMGUI: Resize in fixed steps
    // FIXME-IMGUI: Calculate client size
    ImGuiStyle& style = ImGui::GetStyle();
    char game_id[256];
    snprintf(game_id, ARRAYSIZE(game_id), "%s###Game", gamebox_get_name());
    float scale = g_config.game_window_scale;
    int x_start = g_driver->x_start;
    int y_start = g_driver->y_show_start;
    int x_res = g_driver->x_res;
    int y_res = g_driver->y_res;
    ImGui::SetNextWindowSize(ImVec2(x_res * scale + style.WindowBorderSize * 2.0f, y_res * scale + ImGui::GetFrameHeight() + style.WindowBorderSize * 2.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowBorderSize, style.WindowBorderSize));
    if (ImGui::Begin(game_id, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
    {
        /*
        if ((g_driver->id == DRV_SMS) && (Mask_Left_8))
        {
            // Center screen when 8 left columns are masked - this not logical but looks good
            al_draw_filled_rectangle(x_dst, y_dst, x_dst + 4 * scale, y_dst + y_len*scale, COLOR_BLACK);
            al_draw_filled_rectangle(x_dst + (x_len - 4)*scale, y_dst, x_dst + x_len*scale, y_dst + y_len*scale, COLOR_BLACK);
            x_len -= 8;
            x_start += 8;
            x_dst += 4 * scale;
        }
        */
        int tex_w = al_get_bitmap_width(screenbuffer);
        int tex_h = al_get_bitmap_height(screenbuffer);
        ImVec2 uv0((float)(x_start) / tex_w, (float)(y_start) / tex_h);
        ImVec2 uv1((float)(x_start + x_res) / tex_w, (float)(y_start + y_res) / tex_h);
        ImGui::Image((ImTextureID)screenbuffer, ImVec2(x_res * scale, y_res * scale), uv0, uv1);
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

//-----------------------------------------------------------------------------
// APPLET: Log/Messages
//-----------------------------------------------------------------------------

static void NewGui_LogDraw()
{
    t_newgui* ng = &g_newgui;
    if (!g_config.log_active)
        return;

    Str128f title("%s###Log", Msg_Get(MSG_Message_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &g_config.log_active))
    {
        ImGui::End();
        return;
    }

    if (ImGui::SmallButton("Clear"))
        ng->log_data->clear();
    ImGui::SameLine();
    if (ImGui::SmallButton("Copy"))
        ImGui::SetClipboardText(ng->log_data->c_str());
    ImGui::Separator();

    //static ImGuiTextFilter filter;
    //filter.Draw("##filter");

    ImGui::BeginChild("scrolling");
    const char* log_data = ng->log_data->c_str();
    ImGui::TextUnformatted(log_data);
    if (ng->log_scroll_to_bottom)
    {
        ImGui::SetScrollHereY(1.0f);
        ng->log_scroll_to_bottom = false;
    }

    ImGui::EndChild();
    ImGui::End();
}

void    NewGui_LogAddTextLine(const char* line)
{
    t_newgui* ng = &g_newgui;

    ng->log_data->appendf("%s\n", line);
    ng->log_scroll_to_bottom = true;
    if (ng->log_file)
        fprintf(ng->log_file, "%s\n", line);
}

//-----------------------------------------------------------------------------
// APPLET: Memory Editor
//-----------------------------------------------------------------------------

static void NewGui_MemEditorDraw()
{
    t_memory_viewer* app = MemoryViewer_MainInstance;
    if (!app->active)
        return;

    Str128f title("%s###Memory Editor", Msg_Get(MSG_MemoryEditor_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &app->active))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("##MemType"))
    {
        for (int i = 0; i != MEMTYPE_MAX_; i++)
        {
            t_memory_pane* pane = &app->panes[i];
            t_memory_type memtype = (t_memory_type)i;
            MemoryRange_GetDetails(memtype, &pane->memrange);
            if (ImGui::BeginTabItem(pane->memrange.name))
            {
                MemoryViewer_ViewPane(app, (t_memory_type)i);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    t_memory_range* memrange = &app->pane_current->memrange;
    static MemoryEditor me; // FIXME-IMGUI: Move to instance.
    me.OptAddrDigitsCount = 5; // memrange->addr_hex_length;
    me.ReadFn = [](const ImU8*, size_t off, void* user_data)
    {
        t_memory_range* memrange = (t_memory_range*)user_data;
        return memrange->ReadByte(off);
    };
    me.WriteFn = [](ImU8*, size_t off, ImU8 d, void* user_data)
    {
        t_memory_range* memrange = (t_memory_range*)user_data;
        memrange->WriteByte(off, d);
    };
    me.UserData = (void*)memrange;
    me.DrawContents(NULL, memrange->size, memrange->addr_start);

    if (memrange->size == 0)
    {
        ImGui::BeginChild("##scrolling"); // FIXME-IMGUI: Knows internals of memory editor.
        ImGui::Text("None");
        ImGui::EndChild();
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------
// APPLET: Palette Viewer
//-----------------------------------------------------------------------------

// FIXME-IMGUI: Resize mechanism. Window resize steps? Zoom button?
static void NewGui_PaletteDraw()
{
    if (!g_config.palette_active)
        return;

    Str128f title("%s###Palette", Msg_Get(MSG_Palette_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &g_config.palette_active, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    // State
    // FIXME: Move to saved structure.
    static bool opt_show_memory = false;
    static int opt_icon_size = 20;
    opt_icon_size = ImMax(8, opt_icon_size);

    // Runtime
    ImGuiStyle& style = ImGui::GetStyle();
    float ui_scale = g_config.ui_scale;

    ImVec2 icon_size(opt_icon_size * ui_scale, opt_icon_size * ui_scale);
    ImVec2 spacing(2 * ui_scale, 2 * ui_scale);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    int hovered_n = -1;

    // Draw palette colors
    // We use a single item to avoid hovering gaps between the color blocks.
    const int colors_count = g_driver->colors;
    const int columns = 16;
    const int lines = (colors_count + columns - 1) / columns;
    for (int n = 0; n < colors_count; n++)
    {
        int nx = n % columns;
        int ny = n / columns;
        ImVec2 block_pos = ImGui::GetCursorScreenPos() + (icon_size + spacing) * ImVec2(nx, ny);
        draw_list->AddRectFilled(block_pos, block_pos + icon_size, ImColor(*(ImVec4*)&Palette_Emulation[n]));
        draw_list->AddRect(block_pos, block_pos + icon_size, IM_COL32(255, 255, 255, 60));
    }
    ImGui::InvisibleButton("##colors", ImVec2((icon_size.x + spacing.x) * columns, (icon_size.y + spacing.y) * lines - spacing.y));
    if (ImGui::IsItemHovered())
    {
        ImVec2 mouse_pos_rel = ImGui::GetMousePos() - ImGui::GetItemRectMin();         // FIXME-IMGUI: Retrieve hovered relative mouse position?
        hovered_n = ImClamp((int)(mouse_pos_rel.x / (icon_size.x + spacing.x)), 0, columns - 1) + columns * (int)(mouse_pos_rel.y / (icon_size.y + spacing.y));
    }

    t_memory_range range;
    MemoryRange_GetDetails(MEMTYPE_PRAM, &range);
    size_t pram_entry_size = 0;
    if (g_driver->id == DRV_SMS)
        pram_entry_size = 1;
    else if (g_driver->id == DRV_GG)
        pram_entry_size = 2;

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::MenuItem("Copy"); // FIXME-IMGUI: Unfinished
        ImGui::EndPopup();
    }

    // Draw preview tooltip
    if (hovered_n != -1)
    {
        ImGui::BeginTooltip();

        ImVec4 col_f = *(ImVec4*)&Palette_Emulation[hovered_n];
        ImGui::ColorButton("##peek", col_f, ImGuiColorEditFlags_NoTooltip, icon_size * 2.0f);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::BeginGroup();

        ImGui::Text("Color %02d ($%02X)", hovered_n, hovered_n);

        char color_bits[20];
        if (pram_entry_size == 1)
        {
            StrWriteBitfield(PRAM[hovered_n], 8, color_bits);
            ImGui::Text("VDP Data: %%%s ($%02X)", color_bits, PRAM[hovered_n]);
        }
        else if (pram_entry_size == 2)
        {
            StrWriteBitfield(PRAM[hovered_n * 2 + 1], 8, color_bits);
            StrWriteBitfield(PRAM[hovered_n * 2 + 0], 8, color_bits + 8+1);
            color_bits[8] = '.';
            ImGui::Text("VDP Data: %%%s ($%02X%02X", color_bits, PRAM[hovered_n * 2 + 1], PRAM[hovered_n * 2 + 0]); // FIXME-IMGUI: Is endianness correct?
        }

        // FIXME-IMGUI: More details, e.g R/G/B value in system bit depth
        ImU32 col_32 = ImGui::ColorConvertFloat4ToU32(col_f);
        ImGui::Text("R: %-3d\nG: %-3d\nB: %-3d", (col_32 >> IM_COL32_R_SHIFT) & 0xFF, (col_32 >> IM_COL32_G_SHIFT) & 0xFF, (col_32 >> IM_COL32_B_SHIFT) & 0xFF);

        ImGui::EndGroup();
        ImGui::EndTooltip();
    }

    float button_w = 70.0f * ui_scale;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2) * ui_scale);  // FIXME-IMGUI: Helper for "tight mode"
    ImGui::SetNextItemWidth(button_w);
    ImGui::DragInt("##zoom", &opt_icon_size, 0.10f, 8.0f, 48.0f, ICON_FA_EXPAND " %d");
#if 0
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_COPY " Copy", ImVec2(button_w, 0.0f)))
    {
        // FIXME-IMGUI : Unfinished
    }
#endif
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PEN_SQUARE " Edit", ImVec2(button_w, 0.0f)))
        opt_show_memory ^= 1;
    ImGui::PopStyleVar();

    if (opt_show_memory)
    {
        static MemoryEditor me; // FIXME-IMGUI: move to instance.
        me.Cols = 16;
        me.Lines = range.size / me.Cols;
        me.OptShowAscii = false;
        me.OptShowOptions = false;
        me.OptAddrDigitsCount = 3;
        if (hovered_n >= 0)
        {
            me.HighlightMin = hovered_n * pram_entry_size;
            me.HighlightMax = me.HighlightMin + pram_entry_size;
        }
        else
        {
            me.HighlightMin = me.HighlightMax = (size_t)-1;
        }
        me.WriteFn = [](u8* data, size_t off, u8 v, void*)
        {
            t_memory_range range;
            MemoryRange_GetDetails(MEMTYPE_PRAM, &range);
            range.WriteByte((int)off, v);
        };
        me.DrawContents(range.data, range.size, range.addr_start);
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------
// APPLET: Options
//-----------------------------------------------------------------------------

static void NewGui_OptionsDraw()
{
    if (!g_config.options_active)
        return;

    Str128f title("%s###Options", Msg_Get(MSG_Options_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &g_config.options_active, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::SeparatorText("Emulation");
    ImGui::Checkbox(Msg_Get(MSG_Options_BIOS_Enable), &g_config.enable_BIOS);
    //if (ImGui::Checkbox(Msg_Get(MSG_Options_Bright_Palette), &g_config.palette_type)) Palette_Emu_Reload();
    ImGui::Checkbox(Msg_Get(MSG_Options_Allow_Opposite_Directions), &g_config.allow_opposite_directions);

    ImGui::Spacing();
    ImGui::SeparatorText("User Interface");
    ImGui::Separator();
    if (ImGui::Checkbox(Msg_Get(MSG_Options_DB_Display), &g_config.fb_uses_DB))
        FB_Load_Directory();
    ImGui::Checkbox(Msg_Get(MSG_Options_Product_Number), &g_config.show_product_number);
    ImGui::Checkbox(Msg_Get(MSG_Options_Load_Close), &g_config.fb_close_after_load);
    ImGui::Checkbox(Msg_Get(MSG_Options_Load_FullScreen), &g_config.fullscreen_after_load);
    ImGui::Checkbox(Msg_Get(MSG_Options_FullScreen_Messages), &g_config.show_fullscreen_messages);
    ImGui::Checkbox(Msg_Get(MSG_Options_GUI_VSync), &g_config.video_mode_gui_vsync);
    ImGui::Checkbox(Msg_Get(MSG_Options_Capture_Crop_Align), &g_config.capture_crop_align_8x8);
    if (ImGui::Checkbox(Msg_Get(MSG_Options_NES_Enable), &g_config.enable_NES))
    {
        g_config.enable_NES = false;
        Msg(MSGT_STATUS_BAR, "%s", Msg_Get(MSG_NES_Deny_Facts));
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------
// APPLET: Tech Info
//-----------------------------------------------------------------------------

static void NewGui_TechInfoDraw()
{
    if (!g_config.techinfo_active)
        return;

    Str128f title("%s###TechInfo", Msg_Get(MSG_TechInfo_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &g_config.techinfo_active))
    {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 }); // Compact

    ImGui::Text("Driver: %s (%s), %s", g_driver->full_name, g_driver->short_name, (sms.Country == COUNTRY_EXPORT) ? "Export" : "Japan");

    if (ImGui::TreeNodeEx("CPU", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (Debugger.enabled && Debugger.active)
            ImGui::Text("IPeriod:%d/%d - Lines:%d/%d", CPU_GetICount(), CPU_GetIPeriod(), tsms.VDP_Line, g_machine.TV_lines);
        else
            ImGui::Text("IPeriod:%d - Lines:%d", CPU_GetIPeriod(), g_machine.TV_lines);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("IO", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Port $DE:$%02X - Port $3F:$%02X - Joy:$%04X - GG:$%02X - Paddle:$%02X,$%02X", (sms.Input_Mode), (tsms.Port3F), tsms.Control[7], (tsms.Control_GG), (Inputs.Paddle[0].x), (Inputs.Paddle[1].x));
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("VDP", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const char* vdp_model = "";
        switch (g_machine.VDP.model)
        {
        case VDP_MODEL_315_5124: vdp_model = "315-5124"; break;
        case VDP_MODEL_315_5246: vdp_model = "315-5246"; break;
        case VDP_MODEL_315_5378: vdp_model = "315-5378"; break;
        case VDP_MODEL_315_5313: vdp_model = "315-5313"; break;
        }
        ImGui::Text("Model:%s - Display Mode:%d", vdp_model, tsms.VDP_VideoMode);
        ImGui::Text("Status:$%02X - Address:$%04X - Latch:$%02X - IE0:%d - IE1:%d - DIS:%d", sms.VDP_Status, sms.VDP_Address, sms.VDP_Access_First, (VBlank_ON ? 1 : 0), (HBlank_ON ? 1 : 0), (Display_ON ? 1 : 0));
        ImGui::Text("Scroll: X:$%02X - Y:$%02X - LeftColumnBlank:%d - HSI:%d - VSI:%d", sms.VDP[8], sms.VDP[9], (Mask_Left_8 ? 1 : 0), (Top_No_Scroll ? 1 : 0), (Right_No_Scroll ? 1 : 0));
        ImGui::Text("Sprites: Size:%s - Double:%d - EarlyClock:%d - SAT:$%04X - SPG:$%04X", (Sprites_8x16 ? "8x16" : "8x8"), (Sprites_Double ? 1 : 0), (Sprites_Left_8 ? 1 : 0), (int)(g_machine.VDP.sprite_attribute_table - VRAM), (int)(g_machine.VDP.sprite_pattern_gen_address - VRAM));
        ImGui::Text("Border Color: %d", (sms.VDP[7] & 15));
        ImGui::SameLine();
        ImGui::ColorButton("##Border", *(ImVec4*)&Palette_Emulation[sms.VDP[7] & 15]);
        ImGui::Text("TMS9918: Name:$%04X - Color:$%04X - Pattern:$%04X - SPG:$%04X", (int)(g_machine.VDP.name_table_address - VRAM), (int)(g_machine.VDP.sg_color_table_address - VRAM), (int)(g_machine.VDP.sg_pattern_gen_address - VRAM), (int)(g_machine.VDP.sprite_pattern_gen_address - VRAM));
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Mapper", ImGuiTreeNodeFlags_DefaultOpen))
    {
        char buf[256] = "";
        char* p = buf;
        for (int i = 0; i != g_machine.mapper_regs_count; i++)
        {
            if (i > 0)
                p += sprintf(p, ",");
            p += sprintf(p, "$%02X", g_machine.mapper_regs[i]);
        }
        ImGui::Text("Type:%d - Ctrl:$%02X - Regs:%s - Pages:[%d/%d][%d/%d]",
            g_machine.mapper, sms.SRAM_Mapping_Register, buf, tsms.Pages_Count_8k, tsms.Pages_Mask_8k, tsms.Pages_Count_16k, tsms.Pages_Mask_16k);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("PSG", ImGuiTreeNodeFlags_DefaultOpen))
    {
        t_psg* psg = &PSG;
        ImGui::Text("Tone 0: %03X,%01X  Tone 1: %03X,%01X  Tone 2: %03X,%01X  Noise:%02X,%01X (%s)  Stereo:%02X",
            psg->Registers[0], psg->Registers[1], psg->Registers[2], psg->Registers[3],
            psg->Registers[4], psg->Registers[5], psg->Registers[6], psg->Registers[7],
            ((psg->Registers[6] & 0x04) ? "White" : "Periodic"), psg->Stereo);
        ImGui::TreePop();
    }

    if (FM_Regs != NULL && ImGui::TreeNode("YM2413"))
    {
        ImGui::Text("Custom inst: %02X/%02X/%02X/%02X/%02X/%02X/%02X/%02X  Rhythm: %02X",
            FM_Regs[0], FM_Regs[1], FM_Regs[2], FM_Regs[3], FM_Regs[4], FM_Regs[5], FM_Regs[6], FM_Regs[7], FM_Regs[0xe]);
        for (int i = 0; i < 9; ++i)
            ImGui::Text(" Tone %d: %01X,%03X,%c,%c,%01X,%01X", i,
                (FM_Regs[i + 0x20] & 0x6) >> 1, // Block
                FM_Regs[i + 0x10] | ((FM_Regs[i + 0x20] & 1) << 8), // F-num
                (FM_Regs[i + 0x20] & 0x20) != 0 ? 'S' : '-', // Sustain
                (FM_Regs[i + 0x20] & 0x10) != 0 ? 'K' : '-', // Key
                FM_Regs[i + 0x30] & 0x0f, // Volume
                FM_Regs[i + 0x30] >> 4 // Instrument
            );
        ImGui::TreePop();
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

//-----------------------------------------------------------------------------
// APPLET: About Box
//-----------------------------------------------------------------------------

static void NewGui_AboutDraw()
{
    if (!g_config.about_active)
        return;

    Str128f title("%s###About", Msg_Get(MSG_About_BoxTitle));
    const float ui_scale = g_config.ui_scale;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12) * ui_scale);
    bool open = ImGui::Begin(title.c_str(), &g_config.about_active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::PopStyleVar();
    if (!open)
    {
        ImGui::End();
        return;
    }

    ALLEGRO_BITMAP* bmp = Graphics.Misc.Dragon;

    ImVec2 p = ImGui::GetCursorScreenPos();

    float text_height = ImGui::GetTextLineHeightWithSpacing() * 4.0f;
    ImVec2 image_size = ImVec2(al_get_bitmap_width(bmp), al_get_bitmap_height(bmp)) * ui_scale;

    ImGui::SetCursorScreenPos({ p.x + 8.0f * ui_scale, p.y + ImMax(0.0f, (text_height - image_size.y) * 0.5f)});
    ImGui::Image((ImTextureID)bmp, image_size);

    ImGui::SetCursorScreenPos({ p.x + image_size.x + 8.0f * ui_scale * 3.0f, p.y });
    ImGui::BeginGroup();
    ImGui::Text(Msg_Get(MSG_About_Line_Meka_Date), MEKA_NAME_VERSION, MEKA_DATE);
    ImGui::Text(Msg_Get(MSG_About_Line_Authors), MEKA_AUTHORS_SHORT);
    ImGui::TextLinkOpenURL(MEKA_HOMEPAGE);
    ImGui::Text("Built %s, %s", MEKA_BUILD_DATE, MEKA_BUILD_TIME);
    ImGui::EndGroup();

    ImGui::End();
}

//-----------------------------------------------------------------------------
// Main Menu Bar
//-----------------------------------------------------------------------------

static void    NewGui_DrawMenu()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    // MAIN/FILE
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Main)))
    {
        // FIXME-IMGUI: FB_Switch() toggle
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_LoadROM), "ALT+L", FB.active))        FB_Switch();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_FreeROM), NULL))                      Free_ROM();
        ImGui::Separator();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_Save), "F5"))               SaveState_Save();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_Load), "F7"))               SaveState_Load();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_PrevSlot), "F6"))           SaveState_SetPrevSlot();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_NextSlot), "F8"))           SaveState_SetNextSlot();
        ImGui::Separator();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_Options), "Alt+O", &g_config.options_active)) {}
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Main_Language)))
        {
            ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false); // FIXME-IMGUI: doesn't work because Parent menu name will change, missing ### in loc data.
            for (t_list* langs = Messages.Langs; langs; langs = langs->next)
            {
                t_lang* lang = (t_lang*)langs->elem;
                if (ImGui::MenuItem(lang->Name, NULL, lang == Messages.Lang_Cur))
                {
                	Messages.Lang_Cur = lang;
                    Msg(MSGT_USER, Msg_Get(MSG_Language_Set), Messages.Lang_Cur->Name);
                }
            }
            ImGui::PopItemFlag();
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_Quit), "F10")) Action_Quit();

        ImGui::EndMenu();
    }

    // MACHINE
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Machine)))
    {
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Machine_Power)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_Power_On), NULL, (g_machine_flags & MACHINE_POWER_ON) != 0))
                Machine_ON();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_Power_Off), NULL, (g_machine_flags & MACHINE_POWER_ON) == 0))
                Machine_OFF();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Machine_Region)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_Region_Export), NULL, g_config.country == COUNTRY_EXPORT))
                Set_Country(COUNTRY_EXPORT);
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_Region_Japan), NULL, g_config.country == COUNTRY_JAPAN))
                Set_Country(COUNTRY_JAPAN);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Machine_TVType)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_TVType_NTSC), NULL, TV_Type_User->id == TVTYPE_NTSC))
                TVType_Set(TVTYPE_NTSC, TRUE);
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_TVType_PALSECAM), NULL, TV_Type_User->id == TVTYPE_PAL_SECAM))
                TVType_Set(TVTYPE_PAL_SECAM, TRUE);
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Machine_PauseEmulation), "F12", (g_machine_flags & MACHINE_PAUSED) != 0))
            Machine_Pause();
        if (ImGui::MenuItem(Msg_Get(Msg_Menu_Machine_ResetEmulation), "Alt+Backspace"))
            Machine_Reset();

        ImGui::EndMenu();
    }

    // VIDEO
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video)))
    {
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_FullScreen), "Escape"))
            Action_Switch_Mode();
        if (ImGui::BeginMenu(Msg_Get(Msg_Menu_Video_ScreenCapture)))
        {
            if (ImGui::MenuItem(Msg_Get(Msg_Menu_Video_ScreenCapture_Capture), "PrintScreen"))
                Capture_MenuHandler_Capture();
            if (ImGui::MenuItem(Msg_Get(Msg_Menu_Video_ScreenCapture_CaptureRepeat)))
                Capture_MenuHandler_AllFrames();
            ImGui::MenuItem(Msg_Get(Msg_Menu_Video_ScreenCapture_IncludeGui), NULL, &g_config.capture_include_gui);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video_Themes)))
        {
            Skins_DrawMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video_Blitters)))
        {
            Blitters_DrawMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video_Layers)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_Layers_Sprites), "F11", (opt.Layer_Mask & LAYER_SPRITES) != 0))
                Action_Switch_Layer_Sprites();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_Layers_Sprites), "Ctrl+F11", (opt.Layer_Mask & LAYER_BACKGROUND) != 0))
                Action_Switch_Layer_Background();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video_Flickering)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_Flickering_Auto), NULL, (g_config.sprite_flickering & SPRITE_FLICKERING_AUTO) != 0))
                Action_Switch_Flickering_Auto();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_Flickering_Yes), NULL, !(g_config.sprite_flickering & SPRITE_FLICKERING_AUTO) && (g_config.sprite_flickering & SPRITE_FLICKERING_ENABLED)))
                Action_Switch_Flickering_Yes();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_Flickering_No), NULL, !(g_config.sprite_flickering & SPRITE_FLICKERING_AUTO) && !(g_config.sprite_flickering & SPRITE_FLICKERING_ENABLED)))
                Action_Switch_Flickering_No();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Video_3DGlasses)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_3DGlasses_Enabled), NULL, Glasses.Enabled))
                Glasses_Switch_Enable();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_3DGlasses_ShowBothSides), NULL, Glasses.Mode == GLASSES_MODE_SHOW_BOTH, Glasses.Enabled))
                Glasses_Switch_Mode_Show_Both();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_3DGlasses_ShowLeftSide), NULL, Glasses.Mode == GLASSES_MODE_SHOW_ONLY_LEFT, Glasses.Enabled))
                Glasses_Switch_Mode_Show_Left();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_3DGlasses_ShowRightSide), NULL, Glasses.Mode == GLASSES_MODE_SHOW_ONLY_RIGHT, Glasses.Enabled))
                Glasses_Switch_Mode_Show_Right();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Video_3DGlasses_UsesCOMPort), NULL, Glasses.Mode == GLASSES_MODE_COM_PORT, Glasses.Enabled))
                Glasses_Switch_Mode_Com_Port();
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    // SOUND/AUDIO
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Sound)))
    {
        ImGui::Text(Msg_Get(MSG_Menu_Sound_Volume));
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::BeginDisabled(); // FIXME-IMGUI: value doesn't actually work
        ImGui::SliderInt("##Volume", &Sound.MasterVolume, 0, 128);
        ImGui::EndDisabled();
        ImGui::Separator();

        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Sound_FM)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_FM_Enabled), NULL, Sound.FM_Enabled == true))
                FM_Enable();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_FM_Disabled), NULL, Sound.FM_Enabled == false))
                FM_Disable();
            //if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_FM_Editor), NULL, apps.active.FM_Editor))
            //    FM_Editor_Switch();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Sound_Channels)))
        {
            for (int n = 0; n < 3; n++)
                ImGui::Checkbox(Str30f(Msg_Get(MSG_Menu_Sound_Channels_Tone), n + 1).c_str(), &PSG.Channels[n].Active);
            ImGui::Checkbox(Msg_Get(MSG_Menu_Sound_Channels_Noises), &PSG.Channels[3].Active);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Sound_Capture)))
        {
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_Capture_VGM_Start), (Sound.LogVGM.Logging == VGM_LOGGING_NO) ? "Alt+V" : "", false, (Sound.LogVGM.Logging == VGM_LOGGING_NO)))
                Sound_LogVGM_Start();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_Capture_VGM_Stop), (Sound.LogVGM.Logging != VGM_LOGGING_NO) ? "Alt+V" : "", false, (Sound.LogVGM.Logging != VGM_LOGGING_NO)))
                Sound_LogVGM_Stop();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_Capture_VGM_SampleAccurate), NULL, Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_SAMPLE))
                Sound_LogVGM_Accuracy_Switch();
            ImGui::Separator();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_Capture_WAV_Start), NULL, false, Sound.LogWav == NULL))
                Sound_LogWAV_Start();
            if (ImGui::MenuItem(Msg_Get(MSG_Menu_Sound_Capture_WAV_Stop), NULL, false, Sound.LogWav != NULL))
                Sound_LogWAV_Stop();
            ImGui::EndMenu();
        }
        //SoundDebugApp_DrawMenu();
        ImGui::EndMenu();
    }

    // INPUTS
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Inputs)))
    {
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_Joypad), "F9", (Inputs.Peripheral[0] == INPUT_JOYPAD)))
            Inputs_Switch_Joypad();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_LightPhaser), "", (Inputs.Peripheral[0] == INPUT_LIGHTPHASER)))
            Inputs_Switch_LightPhaser();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_PaddleControl), "", (Inputs.Peripheral[0] == INPUT_PADDLECONTROL)))
            Inputs_Switch_PaddleControl();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_SportsPad), "", (Inputs.Peripheral[0] == INPUT_SPORTSPAD)))
            Inputs_Switch_SportsPad();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_GraphicBoard), "", (Inputs.Peripheral[0] == INPUT_GRAPHICBOARD)))
            Inputs_Switch_GraphicBoard();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_GraphicBoardV2), "", (Inputs.Peripheral[0] == INPUT_GRAPHICBOARD_V2)))
            Inputs_Switch_GraphicBoardV2();
        ImGui::Separator();

        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_SK1100), "CTRL+F9", Inputs.SK1100_Enabled))
            SK1100_Switch();
        ImGui::Separator();

        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Inputs_RapidFire)))
        {
            if (ImGui::MenuItem(Str30f(Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 1, 1).c_str(), "", (RapidFire & RAPIDFIRE_J1B1) != 0))
                RapidFire_Switch_J1B1();
            if (ImGui::MenuItem(Str30f(Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 1, 2).c_str(), "", (RapidFire & RAPIDFIRE_J1B2) != 0))
                RapidFire_Switch_J1B2();
            if (ImGui::MenuItem(Str30f(Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 2, 1).c_str(), "", (RapidFire & RAPIDFIRE_J2B1) != 0))
                RapidFire_Switch_J2B1();
            if (ImGui::MenuItem(Str30f(Msg_Get(MSG_Menu_Inputs_RapidFire_PxBx), 2, 2).c_str(), "", (RapidFire & RAPIDFIRE_J2B2) != 0))
                RapidFire_Switch_J2B2();
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Inputs_Configuration), NULL, Inputs_CFG.active))
            Inputs_CFG_Switch();

        ImGui::EndMenu();
    }

    // TOOLS
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Tools)))
    {
        ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_Messages), "Alt+M", &g_config.log_active);    // FIXME: Rename "Messages" to "Log" in the end user mesages
        ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_Palette), "Alt+P", &g_config.palette_active);
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_TilesViewer), "Alt+T", TileViewer.active)) TileViewer_Switch();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_TilemapViewer), "", TilemapViewer_MainInstance->active)) TilemapViewer_SwitchMainInstance();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_MemoryEditor), "", MemoryViewer_MainInstance->active)) MemoryViewer_SwitchMainInstance();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_CheatFinder), "", g_CheatFinder_MainInstance->active)) CheatFinder_SwitchMainInstance();
        ImGui::MenuItem(Msg_Get(MSG_Menu_Tools_TechInfo), "", &g_config.techinfo_active);
        ImGui::EndMenu();
    }

    // DEBUG
#ifdef MEKA_Z80_DEBUGGER
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Debug)))
    {
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Debug_Enabled), "Scroll Lock", Debugger.active))
            Debugger_Switch();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Debug_ReloadROM)))
            Reload_ROM();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Debug_ReloadSymbols)))
            Debugger_Symbols_Load();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Debug_StepFrame), "Ctrl+F12"))
            Debugger_StepFrame();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Debug_LoadStateAndContinue), "Ctrl+F7"))
        {
            SaveState_Load();
            char command[128] = "CONT";
            Debugger_InputParseCommand(command); // non-const input
        }
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Debug_Dump)))
        {
            DataDump_DrawMenu();
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
#endif

    // HELP
    if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Help)))
    {
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Help_Documentation), NULL, TextViewer.active))
            TextViewer_Switch_Doc_Main();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Help_Compat)))
            TextViewer_Switch_Doc_Compat();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Help_Multiplayer_Games)))
            TextViewer_Switch_Doc_Multiplayer_Games();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Help_Changes)))
            TextViewer_Switch_Doc_Changes();
#ifdef MEKA_Z80_DEBUGGER
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Help_Debugger)))
            TextViewer_Switch_Doc_Debugger();
#endif // MEKA_Z80_DEBUGGR
        ImGui::MenuItem(Msg_Get(MSG_Menu_Help_About), NULL, &g_config.about_active);
        ImGui::EndMenu();
    }

    ImGui::SameLine(0.0f, 20.0f);;
    ImGui::SetNextItemWidth(ImGui::CalcTextSize("999999").x);
    ImGui::DragFloat("Scale", &g_config.ui_scale_next, 0.01f, 0.5f, 6.0f, "%0.2f");

    ImGui::EndMainMenuBar();
}

void    NewGui_DrawStatusBar()
{
    float height = ImGui::GetFrameHeight();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.FramePadding);
    bool ret = ImGui::BeginViewportSideBar("##StatusBar", ImGui::GetMainViewport(), ImGuiDir_Down, height, ImGuiWindowFlags_NoSavedSettings);
    ImGui::PopStyleVar();
    if (!ret)
        return;

    ImVec2 p1 = ImGui::GetCursorScreenPos();
    ImVec2 p2 = p1 + ImGui::GetContentRegionAvail();

    // Time
    const float time_w = ImGui::CalcTextSize("99:99::99 ").x;
    char buf[16];
    p2.x -= time_w;
    ImGui::SetCursorScreenPos({ p2.x, p1.y });
    ImGui::TextAligned(0.0f, time_w, "%s", meka_time_getf(buf));

    // FPS
    const float fps_w = ImGui::CalcTextSize("9999 FPS").x;
    p2.x -= time_w;
    ImGui::SetCursorScreenPos({ p2.x, p1.y });
    ImGui::TextAligned(0.0f, fps_w, "%.1f FPS", fskipper.FPS);

    // Message
    if (g_gui_status.time_remaining > 0.0f)
    {
        ImGui::SetCursorScreenPos(p1);
        ImGui::TextAligned(0.0f, p2.x - p1.x, "%s", g_gui_status.message);
        g_gui_status.time_remaining -= ImGui::GetIO().DeltaTime;
    }

    ImGui::End();
}

void    NewGui_InitStyle()
{
    ImGui::StyleColorsDark();

    ImVec4* colors = ImGui::GetStyle().Colors;
    //colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.00f, 0.94f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.25f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.21f, 0.37f, 0.62f, 1.00f);

    NewGui_UpdateStyle();
}

// Called every frame
void    NewGui_UpdateStyle()
{
    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 backup_colors[ImGuiCol_COUNT];
    memcpy(backup_colors, &style.Colors, sizeof(style.Colors));
    style = ImGuiStyle(); // Reset sizes etc.
    memcpy(&style.Colors, backup_colors, sizeof(style.Colors));

    style.FontScaleMain = g_config.ui_scale;
    style.ScaleAllSizes(g_config.ui_scale);
}

void    NewGui_NewFrame()
{
    // FIXME-IMGUI: Hack before some of our code path abort current frame, e.g. break in Main_Loop_No_Emulation()
    ImGuiContext& g = *ImGui::GetCurrentContext();
    if (g.FrameCount > 0 && g.FrameCountEnded < g.FrameCount)
        ImGui::EndFrame();

    // Start the dear imgui frame
    ImGui_ImplAllegro5_NewFrame();
    ImGui::NewFrame();

    g_config.ui_scale = g_config.ui_scale_next;
    NewGui_UpdateStyle();
}

void    NewGui_Draw()
{
    NewGui_DrawMenu();
    NewGui_DrawStatusBar();

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6,6));

    NewGui_GameDraw();
    NewGui_LogDraw();
    NewGui_MemEditorDraw();
    NewGui_PaletteDraw();
    NewGui_OptionsDraw();
    NewGui_TechInfoDraw();
    NewGui_AboutDraw();

    ImGui::ShowDemoWindow();

    //ImGui::PopStyleVar(3);
}
