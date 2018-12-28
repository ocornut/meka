#pragma once

struct ImGuiTextBuffer;

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_newgui
{
    // Log
    ImGuiTextBuffer*    log_data = NULL;
    Str64               log_filename;
    FILE*               log_file = NULL;
    bool                log_scroll_to_bottom = false;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    NewGui_Init();
void    NewGui_Close();
void    NewGui_Draw();

void    NewGui_GameDraw();
void    NewGui_MemEditorDraw();
void    NewGui_PaletteDraw();

void    NewGui_LogDraw();
void    NewGui_LogAddTextLine(const char* line);

extern t_newgui     g_newgui;
