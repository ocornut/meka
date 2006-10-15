//-----------------------------------------------------------------------------
// MEKA - memview.c
// Memory Viewer - Code
//-----------------------------------------------------------------------------
// Based on code by Valerie Tching.
//-----------------------------------------------------------------------------
// Note: currently referred as "MemoryViewer" in the code,
// but always as "Memory Editor" to the user.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "bmemory.h"
#include "desktop.h"
#include "g_widget.h"
#include "mappers.h"
#include "inputs_t.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEMVIEW_LINES   (16)
#define MEMVIEW_COLUMNS (16)

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

void    MemoryViewer_ViewZ80(void);
void    MemoryViewer_ViewROM(void);
void    MemoryViewer_ViewRAM(void);
void    MemoryViewer_ViewVRAM(void);
void    MemoryViewer_ViewPRAM(void);
void    MemoryViewer_ViewSRAM(void);
void    MemoryViewer_SwitchButton(int new_memtype);

void    MemoryViewer_InputBoxAddr_EnterCallback(t_widget *w);
void    MemoryViewer_InputBoxValue_EditCallback(t_widget *w);
void    MemoryViewer_InputBoxValue_EnterCallback(t_widget *w);
void    MemoryViewer_ClickBottom(t_widget *w);
void    MemoryViewer_ClickMemoryHex(t_widget *w);
void    MemoryViewer_ClickMemoryAscii(t_widget *w);
void    MemoryViewer_SetupEditValueBox(void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MemoryViewer_Init (void)
// Create and initialize Memory Editor applet
//-----------------------------------------------------------------------------
void      MemoryViewer_Init (void)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     box_id;
    int     font_id;
    t_frame frame;

    // Off by default
    mv->active = NO;

    // Create box
    font_id = F_MIDDLE;
    mv->frame_view.pos.x = 226;
    mv->frame_view.pos.y  = 53;
    // A column is made with 2 figures plus a space (except the last one)
    mv->frame_view.size.x = 4;
    // Start of the line: "XXXXX:" (6 characters)
    mv->frame_view.size.x += (Font_Height(font_id) * 6) - 7;
    mv->frame_view.size.x += (MEMVIEW_COLUMNS * (Font_Height(font_id) * (2) - 1) + Font_Height(font_id)) - 3;
    mv->frame_view.size.x += (Font_Height(font_id)) - 3; // Space between numbers and ASCII
    mv->frame_view.size.x += ((Font_Height(font_id) - 2) * 16);
    mv->frame_view.size.x += 4;
    mv->frame_view.size.y = 4;
    mv->frame_view.size.y += MEMVIEW_LINES * Font_Height(font_id);
    mv->frame_view.size.y += 3;
    frame = mv->frame_view;
    // Scrollbar
    frame.size.x += 7;
    // Bottom bar
    frame.size.y += 1 + 1 + Font_Height(F_SMALL) + 2;
    box_id = gui_box_create (frame.pos.x, frame.pos.y, frame.size.x, frame.size.y, Msg_Get(MSG_MemoryEditor_BoxTitle));
    mv->box = gui.box[box_id];
    mv->box->update = MemoryViewer_Update;
    mv->box_gfx = create_bitmap (frame.size.x + 1, frame.size.y + 1);
    gui_set_image_box (box_id, mv->box_gfx);

    // Set exclusive inputs flag to avoid messing with emulation
    // mv->box->focus_inputs_exclusive = YES;

    // Register to desktop (applet is disabled by default)
    Desktop_Register_Box ("MEMORY", box_id, NO, &mv->active);

    // Add close Button
    widget_closebox_add (box_id, MemoryViewer_Switch);

    // Horizontal line to separate buttons from memory
    line (mv->box_gfx, 0, mv->frame_view.size.y, frame.size.x, mv->frame_view.size.y, GUI_COL_BORDERS);

    // Setup Memory sections
    {
        t_memory_section *section;
        frame.pos.x = 187;
        frame.pos.y = mv->frame_view.size.y + 1;
        frame.size.x = 34;
        frame.size.y = Font_Height (F_SMALL) + 3;

        // Z80
        section = &mv->sections[MEMTYPE_Z80];
        section->memtype        = MEMTYPE_Z80;
        section->memblock_first = 0;
        section->size           = 0x10000;
        section->addr_start     = 0x0000;
        section->addr_length    = 4;
        section->name           = "Z80";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewZ80, (char *)section->name);

        // ROM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_ROM];
        section->memtype        = MEMTYPE_ROM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0x00000;
        section->addr_length    = 5;
        section->name           = "ROM";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewROM, (char *)section->name);

        // RAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_RAM];
        section->memtype        = MEMTYPE_RAM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0;    // Unknown as of yet
        section->addr_length    = 4;
        section->name           = "RAM";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewRAM, (char *)section->name);

        // VRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_VRAM];
        section->memtype        = MEMTYPE_VRAM;
        section->memblock_first = 0;
        section->size           = 0x4000;
        section->addr_start     = 0x0000;
        section->addr_length    = 4;
        section->name           = "VRAM";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewVRAM, (char *)section->name);

        // PRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_PRAM];
        section->memtype        = MEMTYPE_PRAM;
        section->memblock_first = 0;
        section->size           = 0x20; // Unknown as of yet
        section->addr_start     = 0x00;
        section->addr_length    = 2;
        section->name           = "PAL";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewPRAM, (char *)section->name);

        // SRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_SRAM];
        section->memtype        = MEMTYPE_SRAM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0;
        section->addr_length    = 4;
        section->name           = "Save";
        section->button         = widget_button_add_draw (box_id, &frame, LOOK_THIN, F_SMALL, 1, MemoryViewer_ViewSRAM, (char *)section->name);
    }

    // Invisible Button to catch click on bottom (to cancel editing)
    frame.pos.x = 0;
    frame.pos.y = mv->frame_view.size.y;
    frame.size.x = mv->box->frame.size.x;
    frame.size.y = mv->box->frame.size.y - mv->frame_view.size.y;
    mv->bottom_box = widget_button_add(box_id, &frame, 1, MemoryViewer_ClickBottom);

    // Address text
    frame.pos.y = mv->frame_view.size.y + 1;
    frame.size.y = Font_Height (F_SMALL) + 3;
    Font_Print (font_id, mv->box_gfx, "Address:", 5, frame.pos.y + 4, GUI_COL_TEXT_IN_BOX);
    line (mv->box_gfx, 92, frame.pos.y, 92, frame.pos.y + frame.size.y, GUI_COL_BORDERS);

    // Goto Address InputBox
    Font_Print (font_id, mv->box_gfx, "Goto", 100, frame.pos.y + 4, GUI_COL_TEXT_IN_BOX);
    frame.pos.x = 128;
    frame.size.x = 40;
    mv->address_edit_inputbox = widget_inputbox_add(box_id, &frame, 5, F_MIDDLE, MemoryViewer_InputBoxAddr_EnterCallback);
    widget_inputbox_set_content_type(mv->address_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);

    // Invisible buttons to catch click on memory for value editing
    frame.pos.x = 4 + (Font_Height(font_id) * 6) - 7;
    frame.pos.y = 4;
    frame.size.x = (MEMVIEW_COLUMNS * (Font_Height(font_id) * (2) - 1) + Font_Height(font_id)) - 3;
    frame.size.y = MEMVIEW_LINES * Font_Height(font_id); 
    mv->values_hex_box = widget_button_add(box_id, &frame, 1, MemoryViewer_ClickMemoryHex);
    
    frame.pos.x += frame.size.x + Font_Height(font_id) - 3;
    frame.size.x = MEMVIEW_COLUMNS * (Font_Height(font_id) - 2);
    mv->values_ascii_box = widget_button_add(box_id, &frame, 1, MemoryViewer_ClickMemoryAscii);

    // Scrollbar
    frame.pos.x = mv->frame_view.size.x;
    frame.pos.y = 0;
    frame.size.x = 7;
    frame.size.y = mv->frame_view.size.y - 1;
    mv->memblock_first = 0;
    mv->memblocks_max = 0;
    mv->memblock_lines_nbr = 16;
    widget_scrollbar_add (box_id, &frame, &mv->memblocks_max, &mv->memblock_first, &mv->memblock_lines_nbr, MemoryViewer_Update);
    line (mv->box_gfx, frame.pos.x - 1, 0, frame.pos.x - 1, frame.size.y, GUI_COL_BORDERS);

    // Input box for memory values
    mv->values_edit_active = FALSE;
    mv->values_edit_position = 0;
    frame.pos.x = 0;
    frame.pos.y = 0;
    frame.size.x = Font_Height(F_MIDDLE) * 2 + 3;
    frame.size.y = Font_Height(F_MIDDLE);
    mv->values_edit_inputbox = widget_inputbox_add(box_id, &frame, 2, F_MIDDLE, MemoryViewer_InputBoxValue_EnterCallback);
    widget_inputbox_set_callback_edit(mv->values_edit_inputbox, MemoryViewer_InputBoxValue_EditCallback);
    widget_inputbox_set_flags(mv->values_edit_inputbox, WIDGET_INPUTBOX_FLAG_NO_CURSOR | WIDGET_INPUTBOX_FLAG_NO_MOVE_CURSOR | WIDGET_INPUTBOX_FLAG_NO_DELETE | WIDGET_INPUTBOX_FLAG_HIGHLIGHT_CURRENT_CHAR, TRUE);
    widget_inputbox_set_content_type(mv->values_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);
    widget_inputbox_set_insert_mode(mv->values_edit_inputbox, TRUE);
    mv->values_edit_inputbox->update = NULL;
    mv->box->n_widgets--; // Hide

    // Initialize memory types, and start by viewing RAM
    mv->section_current = &mv->sections[MEMTYPE_ROM];   // anything but RAM
    MemoryViewer_ViewRAM();

    // Simulate a ROM load, so that default mapping setting are applied
    // (this is just for those checking the Memory Editor on startup before loading a ROM)
    MemoryViewer_LoadROM();
}

//-----------------------------------------------------------------------------
// MemoryViewer_Update (void)
// Refresh Memory Editor box.
//-----------------------------------------------------------------------------
void        MemoryViewer_Update (void)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate
    t_memory_section *section = mv->section_current;

    int             row, col;
    int             x, y, asciix;
    int             addr;
    char            buf[9];
    const int       font_id = F_MIDDLE;
    const int       font_height = Font_Height(font_id);
    u8 *            memory;
    u8              value;
    const int       addr_length = section->addr_length;
    const int       addr_start  = section->addr_start;

    //mv->active = YES;
    mv->box->must_redraw = YES;

    // Clear
    rectfill (mv->box_gfx, 0, 0, mv->frame_view.size.x - 2, mv->frame_view.size.y - 1, GUI_COL_FILL);

    y = 4;
    addr = mv->memblock_first * 16;
    switch (mv->section_current->memtype)
    {
    case MEMTYPE_Z80:   memory = NULL;  break;
    case MEMTYPE_ROM:   memory = ROM;   break;
    case MEMTYPE_RAM:   memory = RAM;   break;
    case MEMTYPE_VRAM:  memory = VRAM;  break;
    case MEMTYPE_PRAM:  memory = PRAM;  break;
    case MEMTYPE_SRAM:
        {
            int dummy_len;
            BMemory_Get_Infos((void *)&memory, &dummy_len);
            break;
        }
    default:            assert(0);
    }

    // Draw vertical lines to separate address/hex/ascii
    x = 4 + font_height * 6 - 7 - 7;
    line (mv->box_gfx, x, 0, x, mv->frame_view.size.y - 1, GUI_COL_BORDERS);
    x = 4 + font_height * 6 - 7 + (MEMVIEW_COLUMNS * (font_height * 2 - 1) + font_height - 3) + font_height - 3 - 4;
    line (mv->box_gfx, x, 0, x, mv->frame_view.size.y - 1, GUI_COL_BORDERS);

    // Print current address
    // FIXME: Should create a label widget for this purpose.
    sprintf(buf, "%0*X", addr_length, addr_start + (mv->memblock_first * 16) + mv->values_edit_position);
    rectfill (mv->box_gfx, 56, mv->frame_view.size.y + 1 + 4, 91, mv->frame_view.size.y + 1 + 4 + Font_Height(font_id), GUI_COL_FILL);
    Font_Print (font_id, mv->box_gfx, buf, 56, mv->frame_view.size.y + 1 + 4, GUI_COL_TEXT_IN_BOX);

    x = 4 + font_height * 6 - 7;
    y = 4;
    if (mv->section_current->size == 0)
    {
        char *text = "None";
        if (mv->section_current->memtype == MEMTYPE_Z80 && cur_drv->cpu != CPU_Z80)
            text = "You wish!";
        Font_Print (font_id, mv->box_gfx, text, x, y, GUI_COL_TEXT_IN_BOX);
    }
    else 
    {
        // Display all memory content lines
        for (row = 0; row < MEMVIEW_LINES; row++, y += font_height)
        {
            if (mv->memblock_first + row >= mv->memblocks_max)
                continue;

            // Print address
            sprintf(buf, "%0*X", addr_length, addr + addr_start);
            x = 4;
            Font_Print (font_id, mv->box_gfx, buf, x + (5 - addr_length) * (font_height - 2), y, GUI_COL_TEXT_IN_BOX);

            // Print 16-bytes in both hexadecimal and ASCII
            x += font_height * 6 - 7;
            asciix = x + (MEMVIEW_COLUMNS * (font_height * 2 - 1) + font_height - 3) + font_height - 3;

            for (col = 0; col < MEMVIEW_COLUMNS; col++, x += font_height * (2) - 1, asciix += font_height - 2)
            {
                int color;

                // Space between columns 7 and 8 (for readability)
                if (col == 8)
                    x += Font_Height(font_id) - 3;

                // Get value
                if (section->memtype == MEMTYPE_Z80)
                    value = (machine & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00;
                else
                    value = memory ? memory[addr] : 0x00;

                // Print hexadecimal
                color = GUI_COL_TEXT_IN_BOX;
                sprintf(buf, "%02X", value);
                Font_Print (font_id, mv->box_gfx, buf, x, y, color);

                // Print ASCII
                if (mv->values_edit_active && (mv->values_edit_position == col + (row * MEMVIEW_COLUMNS)))
                    color = GUI_COL_TEXT_ACTIVE;
                buf[0] = isprint(value) ? value : '.';
                buf[1] = 0;
                Font_Print (font_id, mv->box_gfx, buf, asciix, y, color);

                addr++;
            }
        }
    }

    // Refresh current value if edition cursor is at the beginning (no editing done yet)
    if (mv->values_edit_active)
        if (widget_inputbox_get_cursor_pos(mv->values_edit_inputbox) == 0)
        {
            const int addr = (mv->memblock_first * 16) + mv->values_edit_position;
            if (section->memtype == MEMTYPE_Z80)
                value = (machine & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00;
            else
                value = memory ? memory[addr] : 0x00;
            sprintf(buf, "%02X", value);
            widget_inputbox_set_value(mv->values_edit_inputbox, buf);
            widget_inputbox_set_cursor_pos(mv->values_edit_inputbox, 0);
        }
}

// ACTION: ENABLE OR DISABLE MEMORY VIEWER -------------------------------------
void      MemoryViewer_Switch (void)
{
    if (MemoryViewer.active ^= 1)
        Msg (MSGT_USER, Msg_Get (MSG_MemoryEditor_Enabled));
    else
        Msg (MSGT_USER, Msg_Get (MSG_MemoryEditor_Disabled));
    gui_box_show (MemoryViewer.box, MemoryViewer.active, TRUE);
    gui_menu_inverse_check (menus_ID.tools, 3);
}

// ACTION: SWITCH TO THE DIFFERENT MEMORIES VIEWS -------------------------------

static void MemoryViewer_ViewSection(t_memory_viewer *mv, t_memory_section *section)
{
    if (mv->section_current == section)
        return;

    // Save bookmark
    mv->section_current->memblock_first = mv->memblock_first;

    // Update interface button
    MemoryViewer_SwitchButton(section->memtype);

    // Switch section
    MemoryViewer.section_current    = section;
    MemoryViewer.memblock_first     = section->memblock_first;
    MemoryViewer.memblocks_max      = section->size / 16;
    assert(section->size >= 0);
    //if (MemoryViewer.memblocks_max < MemoryViewer.memblock_lines_nbr)
    //    MemoryViewer.memblocks_max = MemoryViewer.memblock_lines_nbr;
    MemoryViewer_SetupEditValueBox();
}

void      MemoryViewer_ViewZ80(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_Z80]);
}

void      MemoryViewer_ViewROM(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_ROM]);
}

void      MemoryViewer_ViewRAM(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_RAM]);
}

void      MemoryViewer_ViewVRAM(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_VRAM]);
}

void      MemoryViewer_ViewPRAM(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_PRAM]);
}

void      MemoryViewer_ViewSRAM(void)
{
    MemoryViewer_ViewSection(&MemoryViewer, &MemoryViewer.sections[MEMTYPE_SRAM]);
}

void          MemoryViewer_SwitchButton(int new_memtype)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    t_frame *   frame;
    char *      name;
    const int   FontIdx = F_SMALL;

    // FIXME: Cannot the buttons have their label & pressed state changed and automatically redrawn ?
    frame = &mv->section_current->button->frame;
    name = (char *)mv->section_current->name;
    rectfill (mv->box_gfx, frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 2, frame->pos.y + frame->size.y - 2, GUI_COL_BUTTONS);
    gui_rect (mv->box_gfx, LOOK_THIN, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, GUI_COL_BORDERS);
    Font_Print (FontIdx, mv->box_gfx, name, frame->pos.x + ((frame->size.x - Font_TextLength (FontIdx, name)) / 2), frame->pos.y + ((frame->size.y - Font_Height(FontIdx)) / 2) + 1, GUI_COL_TEXT_ACTIVE);

    frame = &mv->sections[new_memtype].button->frame;
    name = (char *)mv->sections[new_memtype].name;
    rectfill (mv->box_gfx, frame->pos.x + 2, frame->pos.y + 2, frame->pos.x + frame->size.x - 2, frame->pos.y + frame->size.y - 2, GUI_COL_HIGHLIGHT);
    gui_rect (mv->box_gfx, LOOK_THIN, frame->pos.x, frame->pos.y, frame->pos.x + frame->size.x, frame->pos.y + frame->size.y, GUI_COL_BORDERS);
    Font_Print (FontIdx, mv->box_gfx, name, frame->pos.x + ((frame->size.x - Font_TextLength (FontIdx, name)) / 2), frame->pos.y + ((frame->size.y - Font_Height(FontIdx)) / 2) + 1, GUI_COL_TEXT_ACTIVE);
}

// ACTION: THINGS TO DO WHEN A ROM IS LOADED -------------------------------

void      MemoryViewer_LoadROM(void)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     z80_memblocks_max, rom_memblocks_max, ram_memblocks_max, pram_memblocks_max, sram_memblocks_max;
    int     ram_len, ram_start_addr;
    int     sram_len;
    int     pram_len;
    u8 *    sram_buf;

    // Z80
    if (cur_drv->cpu == CPU_Z80)
        mv->sections[MEMTYPE_Z80].size = 0x10000;
    else
        mv->sections[MEMTYPE_Z80].size = 0;
    z80_memblocks_max = mv->sections[MEMTYPE_Z80].size / 16;
    if (mv->sections[MEMTYPE_Z80].memblock_first + mv->memblock_lines_nbr > z80_memblocks_max)
        mv->sections[MEMTYPE_Z80].memblock_first = MAX(z80_memblocks_max - mv->memblock_lines_nbr, 0);

    // ROM
    rom_memblocks_max = tsms.Size_ROM / 16;
    mv->sections[MEMTYPE_ROM].size = tsms.Size_ROM;
    if (mv->sections[MEMTYPE_ROM].memblock_first + mv->memblock_lines_nbr > rom_memblocks_max)
        mv->sections[MEMTYPE_ROM].memblock_first = MAX(rom_memblocks_max - mv->memblock_lines_nbr, 0);

    // RAM
    Mapper_Get_RAM_Infos(&ram_len, &ram_start_addr);
    mv->sections[MEMTYPE_RAM].size = ram_len;
    mv->sections[MEMTYPE_RAM].addr_start = ram_start_addr;
    ram_memblocks_max = ram_len / 16;
    if (mv->sections[MEMTYPE_RAM].memblock_first + mv->memblock_lines_nbr > ram_memblocks_max)
        mv->sections[MEMTYPE_RAM].memblock_first = MAX(ram_memblocks_max - mv->memblock_lines_nbr, 0);

    // PRAM
    switch (cur_machine.driver_id)
    {
    case DRV_SMS:   pram_len = 32;  break;
    case DRV_GG:    pram_len = 64;  break;
    default:        pram_len = 0;   break;
    }
    mv->sections[MEMTYPE_PRAM].size = pram_len;
    pram_memblocks_max = pram_len / 16;
    if (mv->sections[MEMTYPE_PRAM].memblock_first + mv->memblock_lines_nbr > pram_memblocks_max)
        mv->sections[MEMTYPE_PRAM].memblock_first = MAX(pram_memblocks_max - mv->memblock_lines_nbr, 0);

    // SRAM
    BMemory_Get_Infos((void *)&sram_buf, &sram_len);
    mv->sections[MEMTYPE_SRAM].size = sram_len;
    sram_memblocks_max = sram_len / 16;
    if (mv->sections[MEMTYPE_SRAM].memblock_first + mv->memblock_lines_nbr > sram_memblocks_max)
        mv->sections[MEMTYPE_SRAM].memblock_first = MAX(sram_memblocks_max - mv->memblock_lines_nbr, 0);

    // Adjust current positions
    mv->memblock_first = mv->section_current->memblock_first;
    switch (mv->section_current->memtype)
    {
    case MEMTYPE_Z80:   mv->memblocks_max = z80_memblocks_max;  break;
    case MEMTYPE_ROM:   mv->memblocks_max = rom_memblocks_max;  break;
    case MEMTYPE_RAM:   mv->memblocks_max = ram_memblocks_max;  break;
    case MEMTYPE_PRAM:  mv->memblocks_max = pram_memblocks_max; break;
    case MEMTYPE_SRAM:  mv->memblocks_max = sram_memblocks_max; break;
    }

    MemoryViewer_SetupEditValueBox();
    // MemoryViewer_Update();
}

// ACTION: INPUT BOXES --------------------------------------------------------

void      MemoryViewer_InputBoxAddr_EnterCallback(t_widget *w)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     addr;
    char *  text;

    // Read address
    text = widget_inputbox_get_value(mv->address_edit_inputbox);
    sscanf(text, "%X", &addr);

    // Check boundaries
    if (addr < mv->section_current->addr_start || addr >= mv->section_current->addr_start + mv->section_current->size)
    {
        char buf[12];
        sprintf(buf, "%0*X", mv->section_current->addr_length, addr);
        Msg (MSGT_USER, Msg_Get(MSG_MemoryEditor_Address_Out_of_Bound), buf, mv->section_current->name);
        return;
    }

    // Jump to given address
    addr -= mv->section_current->addr_start;
    mv->memblock_first = addr / 16;
    if (mv->memblock_first + mv->memblock_lines_nbr > mv->memblocks_max)
        mv->memblock_first = MAX(mv->memblocks_max - mv->memblock_lines_nbr, 0);
    mv->values_edit_active = TRUE;
    mv->values_edit_position = addr & 15;
    MemoryViewer_SetupEditValueBox();
    // MemoryViewer_Update();
    // FIXME: clear ?
    widget_inputbox_set_value(mv->address_edit_inputbox, "");
}

void      MemoryViewer_InputBoxValue_EditCallback(t_widget *w)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     cursor;

    // When cursor reach 2, switch to next byte
    cursor = widget_inputbox_get_cursor_pos(mv->values_edit_inputbox);
    // Msg (MSGT_DEBUG, "Edit, cursor at %d", cursor);
    if (cursor == 2)
    {
        // Simulate validation, then re-enable edit box
        MemoryViewer_InputBoxValue_EnterCallback(w); 
        mv->values_edit_active = TRUE;

        // Go to next
        // (this is a copy of the handler code for KEY_RIGHT)
        if (mv->values_edit_position < (16-1) * 16 - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * 16 + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        else if (((mv->values_edit_position & 15) == 15) && mv->memblock_first + mv->memblock_lines_nbr < mv->memblocks_max)
        {
            mv->memblock_first++;
            mv->values_edit_position -= 16 - 1;
        }
        else if (mv->values_edit_position < 16 * 16 - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * 16 + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        MemoryViewer_SetupEditValueBox();
    }
    // FIXME: when cursor reach 2, write and move to next memory location
}

void      MemoryViewer_InputBoxValue_EnterCallback(t_widget *w)
{
    int     addr;
    int     value;
    char *  text;

    text = widget_inputbox_get_value(MemoryViewer.values_edit_inputbox);
    addr = MemoryViewer.memblock_first * 16 + MemoryViewer.values_edit_position;
    sscanf(text, "%X", &value);
    switch (MemoryViewer.section_current->memtype)
    {
    case MEMTYPE_Z80:
        {
            if (machine & MACHINE_POWER_ON)
                WrZ80_NoHook(addr, value);
            else
                Msg (MSGT_USER, Msg_Get(MSG_MemoryEditor_WriteZ80_Unable));
            break;
        }
    case MEMTYPE_ROM:     
        {
            ROM[addr] = value;
            // We have a special handling there, because SMS/GG mapper emulation is using a 
            // different memory area for addresses between 0x0000 and 0x0400
            if (addr < 0x4000)
                if (Mem_Pages [0] == Game_ROM_Computed_Page_0)
                    Game_ROM_Computed_Page_0[addr] = value;
            break;
        }
    case MEMTYPE_RAM:
        {
            if (cur_drv->id == DRV_COLECO)
                Write_Mapper_Coleco(0x6000 + addr, value);  // special hook for Colecovision
            else
                RAM[addr] = value;
            break;
        }
    case MEMTYPE_VRAM:    
        {
            VRAM[addr] = value;     
            // Mark corresponding tile as dirty
            tgfx.Tile_Dirty [addr >> 5] |= TILE_DIRTY_DECODE;
            break;
        }
    case MEMTYPE_PRAM:
        {
            Tms_VDP_Palette_Write(addr, value);
            break;
        }
    case MEMTYPE_SRAM:
        {
            SRAM[addr] = value;
            break;
        }
    }
    MemoryViewer.values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox();
}

void      MemoryViewer_ClickBottom(t_widget *w)
{
    MemoryViewer.values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox();
}

void        MemoryViewer_ClickMemoryHex(t_widget *w)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     x, y;

    // Msg (MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);

    // Out of the memory area
    if ((MEMVIEW_COLUMNS / 2) * (Font_Height(F_MIDDLE) * (2) - 1) <= w->mx && 
        w->mx < (MEMVIEW_COLUMNS / 2) * (Font_Height(F_MIDDLE) * (2) - 1) + Font_Height(F_MIDDLE) - 3)
    {
        // Hide edit value input box
        mv->values_edit_active = FALSE;
        MemoryViewer_SetupEditValueBox();
        return;
    }

    // Inside
    if (w->mx < (MEMVIEW_COLUMNS / 2) * (Font_Height(F_MIDDLE) * (2) - 1))
        x = w->mx / (Font_Height(F_MIDDLE) * (2) - 1); // Left half
    else
        x = (w->mx - Font_Height(F_MIDDLE) + 3) / (Font_Height(F_MIDDLE) * (2) - 1); // Right half
    y = (w->my / Font_Height(F_MIDDLE));

    mv->values_edit_position = x + y * MEMVIEW_COLUMNS;
    mv->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox();
}

void        MemoryViewer_ClickMemoryAscii(t_widget *w)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    int     x, y;

    x = w->mx / (Font_Height(F_MIDDLE) - 2);
    y = w->my / Font_Height(F_MIDDLE);
    // Msg (MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);
    // Msg (MSGT_DEBUG, "x = %d, y = %d\n", x, y);

    mv->values_edit_position = x + y * MEMVIEW_COLUMNS;
    mv->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox();
}

void    MemoryViewer_SetupEditValueBox(void)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    // Disable if out of boundaries
    int addr = mv->memblock_first * MEMVIEW_COLUMNS + mv->values_edit_position;
    if (addr < 0 || addr >= mv->section_current->size)
    {
        mv->values_edit_active = FALSE;
        mv->values_edit_position = 0;
    }

    if (mv->values_edit_active)
    {
        // Position input box
        {
            t_frame *frame = &mv->values_edit_inputbox->frame;
            const int pos = mv->values_edit_position;
            frame->pos.x = (pos & 15) * (Font_Height(F_MIDDLE) * 2 - 1);
            if ((pos & 15) >= 8) // Second half
                frame->pos.x += Font_Height(F_MIDDLE) - 3;
            frame->pos.y = (pos / 16) * Font_Height(F_MIDDLE);
            frame->pos.x += mv->values_hex_box->frame.pos.x - 5; // Coordinates are parent relative
            frame->pos.y += mv->values_hex_box->frame.pos.y - 1;
        }

        // Show input box if not already active
        if (mv->values_edit_inputbox->update == NULL)
        {
            mv->values_edit_inputbox->update = widget_inputbox_update;
            mv->address_edit_inputbox->update = NULL;
            mv->box->n_widgets++;
        }

        // Setup input box default content
        {
            int value;
            char buf[3];
            switch (mv->section_current->memtype)
            {
                case MEMTYPE_Z80:   value = (machine & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00; break;
                case MEMTYPE_ROM:   value = ROM[addr];      break;
                case MEMTYPE_RAM:   value = RAM[addr];      break;
                case MEMTYPE_VRAM:  value = VRAM[addr];     break;
                case MEMTYPE_PRAM:  value = PRAM[addr];     break;
                case MEMTYPE_SRAM:
                    {
                        int dummy_len;
                        u8 *SRAM;
                        BMemory_Get_Infos((void *)&SRAM, &dummy_len);
                        value = SRAM ? SRAM[addr] : 0x00;
                        break;
                    }
                default:
                    assert(0);
                    return;
            }
            sprintf(buf, "%02X", value);
            widget_inputbox_set_value(mv->values_edit_inputbox, buf);
            widget_inputbox_set_cursor_pos(mv->values_edit_inputbox, 0);
        }
    }
    else
    {
        // Hide input box if active
        if (mv->values_edit_inputbox->update != NULL)
        {
            mv->box->n_widgets--;
            mv->address_edit_inputbox->update = widget_inputbox_update;
            mv->values_edit_inputbox->update = NULL;
        }
    }
}

//-----------------------------------------------------------------------------
// MemoryViewer_Update_Inputs (void)
// Poll and update user inputs.
//-----------------------------------------------------------------------------
// FIXME: This function is probably unnecessary big.
//-----------------------------------------------------------------------------
void            MemoryViewer_Update_Inputs (void)
{
    t_memory_viewer *mv = &MemoryViewer; // for easier switch when we'll be able to instanciate

    // Check for focus
    if (!gui_box_has_focus (mv->box))
        return;

    if (Inputs_KeyPressed (KEY_HOME, NO))
    {
        mv->memblock_first = 0;
        mv->values_edit_position = 0;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }
    else if (Inputs_KeyPressed (KEY_END, NO))
    {
        mv->memblock_first = MAX(mv->memblocks_max - mv->memblock_lines_nbr, 0);
        mv->values_edit_position = MIN(16 * 16, mv->section_current->size - mv->memblock_first * 16);
        if (mv->values_edit_position > 0)
            mv->values_edit_position--;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }
    else if (Inputs_KeyPressed_Repeat (KEY_PGUP, NO, 30, 3) || gui_mouse.z_rel > 0)
    {
        mv->memblock_first -= mv->memblock_lines_nbr;
        if (mv->memblock_first < 0)
        {
            mv->memblock_first = 0;
            // if (!(gui_mouse.z_rel > 0))
            //    mv->values_edit_position = mv->values_edit_position & 15;
        }
        MemoryViewer_SetupEditValueBox();
    }
    else if (Inputs_KeyPressed_Repeat (KEY_PGDN, NO, 30, 3) || gui_mouse.z_rel < 0)
    {
        mv->memblock_first += mv->memblock_lines_nbr;
        if (mv->memblock_first + mv->memblock_lines_nbr > mv->memblocks_max)
        {
            mv->memblock_first = MAX(mv->memblocks_max - mv->memblock_lines_nbr, 0);
            // if (!(gui_mouse.z_rel < 0))
            //    mv->values_edit_position = (15 * 16) + (mv->values_edit_position & 15);
        }
        MemoryViewer_SetupEditValueBox();
    }
    else if (Inputs_KeyPressed_Repeat (KEY_UP, NO, 30, 3))
    {
        if (mv->values_edit_position >= 8 * 16)
            mv->values_edit_position -= 16;
        else
        {
            mv->memblock_first--;
            if (mv->memblock_first < 0)
            {
                mv->memblock_first = 0;
                if (mv->values_edit_position >= 16)
                    mv->values_edit_position -= 16;
            }
        }
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }
    else if (Inputs_KeyPressed_Repeat (KEY_DOWN, NO, 30, 3))
    {
        if (mv->values_edit_position < 7 * 16)
        {
            mv->values_edit_position += 16;
            if (mv->memblock_first * 16 + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position -= 16;
        }
        else
        {
            mv->memblock_first++;
            if (mv->memblock_first + mv->memblock_lines_nbr > mv->memblocks_max)
            {
                mv->memblock_first = MAX(mv->memblocks_max - mv->memblock_lines_nbr, 0);
                if (mv->values_edit_position < 15 * 16)
                    mv->values_edit_position += 16;
            }
        }
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat (KEY_LEFT, NO, 30, 3))
    {
        if (mv->values_edit_position > 1 * 16)
            mv->values_edit_position--;
        else if (mv->memblock_first > 0)
        {
            mv->memblock_first--;
            mv->values_edit_position += 16 - 1;
        }
        else if (mv->values_edit_position > 0)
            mv->values_edit_position--;
        // mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat (KEY_RIGHT, NO, 30, 3))
    {
        if (mv->values_edit_position < (16-1) * 16 - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * 16 + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        else if (((mv->values_edit_position & 15) == 15) && mv->memblock_first + mv->memblock_lines_nbr < mv->memblocks_max)
        {
            mv->memblock_first++;
            mv->values_edit_position -= 16 - 1;
        }
        else if (mv->values_edit_position < 16 * 16 - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * 16 + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        // mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox();
    }

    // Limits
    if (mv->memblock_first + mv->memblock_lines_nbr > mv->memblocks_max)
        mv->memblock_first = MAX(mv->memblocks_max - mv->memblock_lines_nbr, 0);
    else if (mv->memblock_first < 0)
        mv->memblock_first = 0;
}

//-----------------------------------------------------------------------------
