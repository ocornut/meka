#include "shared.h"
#include "newgui.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_memory_editor.h"
#include "imgui_impl_allegro5.h"
#include "IconsFontAwesome5.h"  // See https://fontawesome.com/icons?d=gallery

#include "app_filebrowser.h"
#include "app_game.h"
#include "app_palview.h"
#include "app_memview.h"
#include "palette.h"
#include "saves.h"
#include "vmachine.h"

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui fonts
    {
        ImFontConfig icon_font_cfg;
        icon_font_cfg.MergeMode = true;
        icon_font_cfg.GlyphMinAdvanceX = 13.0f;
        icon_font_cfg.OversampleH = icon_font_cfg.OversampleV = 1.0f;
        static ImWchar icon_font_ranges[] = { ICON_MIN_FA , ICON_MAX_FA, 0 };
        Str128f filename("%s/Data/fonts/Font Awesome 5 Free-Solid-900.otf", g_env.Paths.EmulatorDirectory);
        io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromFileTTF(filename.c_str(), 13.0f, &icon_font_cfg, icon_font_ranges);
    }

    // Setup Dear ImGui style
    {
        ImGui::StyleColorsDark();
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.00f, 0.94f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.16f, 0.26f, 1.00f);
    }

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

void    NewGui_GameDraw()
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

void    NewGui_LogDraw()
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

void    NewGui_MemEditorDraw()
{
    if (!MemoryViewer_MainInstance->active)
        return;

    Str128f title("%s###Memory Editor", Msg_Get(MSG_MemoryEditor_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &MemoryViewer_MainInstance->active))
    {
        ImGui::End();
        return;
    }

    t_memory_range range;
    MemoryRange_GetDetails(MEMTYPE_RAM, &range);

    static MemoryEditor me;
    me.OptAddrDigitsCount = range.addr_hex_length;
    me.DrawContents(range.data, range.size, range.addr_start);

    ImGui::End();
}

// Applet: Palette Viewer
// FIXME-IMGUI: Resize mechanism. Window resize steps? Zoom button?
void    NewGui_PaletteDraw()
{
    if (!PaletteViewer.active)
        return;

    Str128f title("%s###Palette", Msg_Get(MSG_Palette_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &PaletteViewer.active, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    // State
    // FIXME: Move to saved structure.
    static bool opt_show_memory = false;
    static int opt_icon_size = 20;

    // Runtime
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 icon_size(opt_icon_size, opt_icon_size);
    ImVec2 spacing(2, 2);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    int hovered_n = -1;

    // Draw palette colors
    // We use a single item to avoid hovering gaps between the color blocks.
    const int columns = 16;
    const int lines = (PaletteViewer.palette_size + columns - 1) / columns;
    for (int n = 0; n < PaletteViewer.palette_size; n++)
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
            ImGui::Text("VDP Data: %%%s", color_bits);
        }
        else if (pram_entry_size == 2)
        {
            StrWriteBitfield(PRAM[hovered_n * 2 + 1], 8, color_bits);
            StrWriteBitfield(PRAM[hovered_n * 2 + 0], 8, color_bits + 8+1);
            color_bits[8] = '.';
            ImGui::Text("VDP Data: %%%s", color_bits);
        }

        // FIXME-IMGUI: More details, e.g R/G/B value in system bit depth
        ImU32 col_32 = ImGui::ColorConvertFloat4ToU32(col_f);
        ImGui::Text("R: %-3d\nG: %-3d\nB: %-3d", (col_32 >> IM_COL32_R_SHIFT) & 0xFF, (col_32 >> IM_COL32_G_SHIFT) & 0xFF, (col_32 >> IM_COL32_B_SHIFT) & 0xFF);

        ImGui::EndGroup();
        ImGui::EndTooltip();
    }

    float button_w = 60.0f;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));  // FIXME-IMGUI: Helper for "tight mode"
    ImGui::PushItemWidth(button_w);
    ImGui::DragInt("##zoom", &opt_icon_size, 0.10f, 8.0f, 48.0f, ICON_FA_EXPAND " %d");
    ImGui::PopItemWidth();
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
        static MemoryEditor me;
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
        me.WriteFn = [](u8* data, size_t off, u8 v) // FIXME-IMGUI: memory editor need user data
        {
            t_memory_range range;
            MemoryRange_GetDetails(MEMTYPE_PRAM, &range);
            range.WriteByte((int)off, v);
        };
        me.DrawContents(range.data, range.size, range.addr_start);
    }

    ImGui::End();
}

void    NewGui_OptionsDraw()
{
    if (!g_config.options_active)
        return;

    Str128f title("%s###Options", Msg_Get(MSG_Options_BoxTitle));
    if (!ImGui::Begin(title.c_str(), &g_config.options_active, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Emulation");
    ImGui::Separator();
    ImGui::Checkbox(Msg_Get(MSG_Options_BIOS_Enable), &g_config.enable_BIOS);
    //if (ImGui::Checkbox(Msg_Get(MSG_Options_Bright_Palette), &g_config.palette_type)) Palette_Emu_Reload();
    ImGui::Checkbox(Msg_Get(MSG_Options_Allow_Opposite_Directions), &g_config.allow_opposite_directions);

    ImGui::Spacing();
    ImGui::Text("User Interface");
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

void    NewGui_MainMenu()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File"))//Msg_Get(MSG_Menu_Main)))
    {
        // FIXME-IMGUI: FB_Switch() toggle
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_LoadROM), "ALT+L", FB.active))        FB_Switch();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_FreeROM), NULL))                      Free_ROM();
        ImGui::Separator();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_Save), "F5"))               SaveState_Save();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_Load), "F7"))               SaveState_Load();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_PrevSlot), "F6"))           SaveState_SetPrevSlot();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_SaveState_PrevSlot), "F8"))           SaveState_SetNextSlot();
        ImGui::Separator();
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_Options), "Alt+O", &g_config.options_active)) {}
        if (ImGui::BeginMenu(Msg_Get(MSG_Menu_Main_Language)))
        {
            for (t_list* langs = Messages.Langs; langs; langs = langs->next)
            {
                t_lang* lang = (t_lang*)langs->elem;
                if (ImGui::MenuItem(lang->Name, NULL, lang == Messages.Lang_Cur))
                {
                	Messages.Lang_Cur = lang;
                    Msg(MSGT_USER, Msg_Get(MSG_Language_Set), Messages.Lang_Cur->Name);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem(Msg_Get(MSG_Menu_Main_Quit), "F10")) Action_Quit();

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void    NewGui_Draw()
{
    NewGui_MainMenu();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6,6));

    NewGui_GameDraw();
    NewGui_LogDraw();
    NewGui_MemEditorDraw();
    NewGui_PaletteDraw();
    NewGui_OptionsDraw();

    ImGui::ShowDemoWindow();

    ImGui::PopStyleVar(3);
}
