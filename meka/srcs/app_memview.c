//-----------------------------------------------------------------------------
// MEKA - app_memview.c
// Memory Viewer - Code
//-----------------------------------------------------------------------------
// Based on code by Valerie Tching.
//-----------------------------------------------------------------------------
// Note: currently referred as "Memory Viewer" in the code,
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

#define MEMVIEW_COLUMNS_8_PADDING   (5)     // Horizontal padding between each 8 columns

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_memory_viewer *   MemoryViewer_MainInstance;
t_list *            MemoryViewers;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static void     MemoryViewer_Layout(t_memory_viewer *mv, bool setup);
static void     MemoryViewer_Update(t_memory_viewer *mv);
static void     MemoryViewer_Update_Inputs(t_memory_viewer *mv);
static void     MemoryViewer_MediaReload(t_memory_viewer *mv);

static void     MemoryViewer_Switch(t_widget *w);

static void     MemoryViewer_ViewZ80(t_widget *w);
static void     MemoryViewer_ViewROM(t_widget *w);
static void     MemoryViewer_ViewRAM(t_widget *w);
static void     MemoryViewer_ViewVRAM(t_widget *w);
static void     MemoryViewer_ViewPRAM(t_widget *w);
static void     MemoryViewer_ViewSRAM(t_widget *w);
static void     MemoryViewer_ViewVREG(t_widget *w);

static void		MemoryViewer_InputBoxAddress_EnterCallback(t_widget *w);
static void		MemoryViewer_InputBoxValue_EditCallback(t_widget *w);
static void		MemoryViewer_InputBoxValue_EnterCallback(t_widget *w);
static void		MemoryViewer_ClickBottom(t_widget *w);
static void		MemoryViewer_ClickMemoryHex(t_widget *w);
static void		MemoryViewer_ClickMemoryAscii(t_widget *w);
static void		MemoryViewer_SetupEditValueBox(t_memory_viewer *mv);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MemoryViewer_New(bool register_desktop, int size_columns, int size_lines)
// Create and initialize Memory Editor applet
//-----------------------------------------------------------------------------
t_memory_viewer *   MemoryViewer_New(bool register_desktop, int size_columns, int size_lines)
{
    t_memory_viewer *mv = malloc(sizeof(t_memory_viewer));
    t_frame frame;

    // Check parameters
    assert(size_columns >= -1);
    assert(size_lines >= -1);

    // Add to global list
    list_add(&MemoryViewers, mv);

    // Setup members
    mv->active = TRUE;
    mv->size_columns    = (size_columns != -1) ? size_columns : Configuration.memory_editor_columns;
    mv->size_lines      = (size_lines != -1)   ? size_lines   : Configuration.memory_editor_lines;

    mv->frame_hex.pos.x  = 4 + (Font_Height(F_MIDDLE) * 6) - 7;
    mv->frame_hex.pos.y  = 4;
    mv->frame_hex.size.x = (mv->size_columns * (Font_Height(F_MIDDLE) * (2) - 1)) + ((mv->size_columns - 1) / 8) * MEMVIEW_COLUMNS_8_PADDING;
    mv->frame_hex.size.y = mv->size_lines * Font_Height(F_MIDDLE);

    mv->frame_ascii.pos.x  = mv->frame_hex.pos.x + mv->frame_hex.size.x + MEMVIEW_COLUMNS_8_PADDING;
    mv->frame_ascii.pos.y  = 4;
    mv->frame_ascii.size.x = (mv->size_columns * (Font_Height(F_MIDDLE) - 2));
    mv->frame_ascii.size.y = mv->size_lines * Font_Height(F_MIDDLE);

    // Create box
    mv->frame_view.pos.x = 226;
    mv->frame_view.pos.y  = 53;
    // A column is made with 2 figures plus a space (except the last one)
    mv->frame_view.size.x = 4;
    // Start of the line: "XXXXX:" (6 characters)
    mv->frame_view.size.x += (Font_Height(F_MIDDLE) * 6) - 7;   // Address
    mv->frame_view.size.x += mv->frame_hex.size.x;              // Hexadecimal
    mv->frame_view.size.x += MEMVIEW_COLUMNS_8_PADDING;         // Padding
    mv->frame_view.size.x += mv->frame_ascii.size.x;            // ASCII
    mv->frame_view.size.x += 4;                                 // Padding
    mv->frame_view.size.y = 4 + mv->frame_hex.size.y + 3;
    frame = mv->frame_view;
    // Scrollbar
    frame.size.x += 7;
    // Bottom bar
    frame.size.y += 1 + 1 + Font_Height(F_SMALL) + 2;

    mv->box = gui_box_new(&frame, Msg_Get(MSG_MemoryEditor_BoxTitle));
    mv->box->user_data = mv;
    mv->box_gfx = mv->box->gfx_buffer;
    mv->box->destroy = MemoryViewer_Delete;
    mv->box->flags |= GUI_BOX_FLAGS_TAB_STOP;

    // Set exclusive inputs flag to avoid messing with emulation
    // mv->box->focus_inputs_exclusive = TRUE;

    // Register to desktop (applet is disabled by default)
    if (register_desktop)
        Desktop_Register_Box("MEMORY", mv->box, FALSE, &mv->active);

    // Layout
    MemoryViewer_Layout(mv, TRUE);

    // Setup other members
    mv->memblock_first = 0;
    mv->memblocks_max = 0;
    mv->values_edit_active = FALSE;
    mv->values_edit_position = 0;
    mv->section_current = &mv->sections[MEMTYPE_ROM];   // anything but RAM (see below)

    // Start by viewing RAM
    MemoryViewer_ViewRAM(mv->sections[MEMTYPE_RAM].button);

    // Simulate a ROM load, so that default mapping setting are applied
    // (this is for when checking the Memory Editor on startup before loading a ROM)
    MemoryViewer_MediaReload(mv);

    // Return new instance
    return (mv);
}

void        MemoryViewer_Delete(t_memory_viewer *mv)
{
    // Remove from global list
    list_remove(&MemoryViewers, mv);

    // Delete
    free(mv);
}

static void MemoryViewer_Layout(t_memory_viewer *mv, bool setup)
{
    t_frame frame;

    // Clear
    clear_to_color(mv->box->gfx_buffer, COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(mv->box, MemoryViewer_Switch);
        
    // Horizontal line to separate buttons from memory
    line (mv->box_gfx, 0, mv->frame_view.size.y, mv->box->frame.size.x, mv->frame_view.size.y, COLOR_SKIN_WINDOW_SEPARATORS);

    // Setup Memory sections
    if (setup)
    {
        t_memory_section *section;
        frame.pos.x = 179;
        frame.pos.y = mv->frame_view.size.y + 1;
        frame.size.x = 30;
        frame.size.y = Font_Height (F_SMALL) + 3;

        // Z80
        section = &mv->sections[MEMTYPE_Z80];
        section->memtype        = MEMTYPE_Z80;
        section->memblock_first = 0;
        section->size           = 0x10000;
        section->addr_start     = 0x0000;
        section->addr_length    = 4;
        section->name           = "Z80";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewZ80, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // ROM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_ROM];
        section->memtype        = MEMTYPE_ROM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0x00000;
        section->addr_length    = 5;
        section->name           = "ROM";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewROM, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // RAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_RAM];
        section->memtype        = MEMTYPE_RAM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0;    // Unknown as of yet
        section->addr_length    = 4;
        section->name           = "RAM";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewRAM, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // VRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_VRAM];
        section->memtype        = MEMTYPE_VRAM;
        section->memblock_first = 0;
        section->size           = 0x4000;
        section->addr_start     = 0x0000;
        section->addr_length    = 4;
        section->name           = "VRAM";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewVRAM, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // VREG
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_VREG];
        section->memtype        = MEMTYPE_VREG;
        section->memblock_first = 0;
        section->size           = 11;
        section->addr_start     = 0x0000;
        section->addr_length    = 1;
        section->name           = "VREG";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewVREG, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // PRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_PRAM];
        section->memtype        = MEMTYPE_PRAM;
        section->memblock_first = 0;
        section->size           = 0x20; // Unknown as of yet
        section->addr_start     = 0x00;
        section->addr_length    = 2;
        section->name           = "PAL";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewPRAM, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);

        // SRAM
        frame.pos.x += frame.size.x + 2;
        section = &mv->sections[MEMTYPE_SRAM];
        section->memtype        = MEMTYPE_SRAM;
        section->memblock_first = 0;
        section->size           = 0;    // Unknown as of yet
        section->addr_start     = 0;
        section->addr_length    = 4;
        section->name           = "Save";
        section->button         = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewSRAM, WIDGET_BUTTON_STYLE_SMALL, (char *)section->name);
    }

    // Invisible Button to catch click on bottom (to cancel editing)
    frame.pos.x = 0;
    frame.pos.y = mv->frame_view.size.y;
    frame.size.x = mv->box->frame.size.x;
    frame.size.y = mv->box->frame.size.y - mv->frame_view.size.y;
    if (setup)
        mv->bottom_box = widget_button_add(mv->box, &frame, 1, MemoryViewer_ClickBottom, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

    // Address text
    frame.pos.y = mv->frame_view.size.y + 1;
    frame.size.y = Font_Height (F_SMALL) + 3;
    Font_Print (F_MIDDLE, mv->box_gfx, "Address:", 5, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
    line(mv->box_gfx, 92, frame.pos.y, 92, frame.pos.y + frame.size.y, COLOR_SKIN_WINDOW_SEPARATORS);

    // Goto Address input box
    Font_Print (F_MIDDLE, mv->box_gfx, "Goto", 100, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
    if (setup)
    {
        frame.pos.x = 128;
        frame.size.x = 40;
        mv->address_edit_inputbox = widget_inputbox_add(mv->box, &frame, 5, F_MIDDLE, MemoryViewer_InputBoxAddress_EnterCallback);
        widget_inputbox_set_content_type(mv->address_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);
    }

    // Invisible buttons to catch click on memory for value editing
    if (setup)
    {
        // Hexadecimal part
        mv->values_hex_box = widget_button_add(mv->box, &mv->frame_hex, 1, MemoryViewer_ClickMemoryHex, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

        // ASCII part
        mv->values_ascii_box = widget_button_add(mv->box, &mv->frame_ascii, 1, MemoryViewer_ClickMemoryAscii, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);
    }

    // Scrollbar
    frame.pos.x = mv->frame_view.size.x;
    frame.pos.y = 0;
    frame.size.x = 7;
    frame.size.y = mv->frame_view.size.y - 1;
    if (setup)
        mv->widget_scrollbar = widget_scrollbar_add(mv->box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &mv->memblocks_max, &mv->memblock_first, &mv->size_lines, NULL);
    line(mv->box_gfx, frame.pos.x - 1, 0, frame.pos.x - 1, frame.size.y, COLOR_SKIN_WINDOW_SEPARATORS);

    // Input box for memory values
    if (setup)
    {
        frame.pos.x = 0;
        frame.pos.y = 0;
        frame.size.x = Font_Height(F_MIDDLE) * 2 + 3;
        frame.size.y = Font_Height(F_MIDDLE);
        mv->values_edit_inputbox = widget_inputbox_add(mv->box, &frame, 2, F_MIDDLE, MemoryViewer_InputBoxValue_EnterCallback);
        widget_inputbox_set_callback_edit(mv->values_edit_inputbox, MemoryViewer_InputBoxValue_EditCallback);
        widget_inputbox_set_flags(mv->values_edit_inputbox, WIDGET_INPUTBOX_FLAGS_NO_CURSOR | WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR | WIDGET_INPUTBOX_FLAGS_NO_DELETE | WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR, TRUE);
        widget_inputbox_set_content_type(mv->values_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);
        widget_inputbox_set_insert_mode(mv->values_edit_inputbox, TRUE);
        mv->values_edit_inputbox->update = NULL;
        widget_disable(mv->values_edit_inputbox);
    }
}

//-----------------------------------------------------------------------------
// MemoryViewer_Update(t_memory_viewer *mv)
// Refresh Memory Editor
//-----------------------------------------------------------------------------
static void        MemoryViewer_Update(t_memory_viewer *mv)
{
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

    // Skip update if not active
    if (!mv->active)
        return;

    // If skin has changed, redraw everything
    if (mv->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        MemoryViewer_Layout(mv, FALSE);
        mv->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
    else
    {
        // Clear anyway
        rectfill (mv->box_gfx, 0, 0, mv->frame_view.size.x - 2, mv->frame_view.size.y - 1, COLOR_SKIN_WINDOW_BACKGROUND);
    }

    // Always dirty (FIXME?)
    mv->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;
    widget_set_dirty(mv->widget_scrollbar);

    // Update inputs
    MemoryViewer_Update_Inputs(mv);

    y = 4;
    addr = mv->memblock_first * mv->size_columns;
    // FIXME: See with debugger or other tools if this can be abstracted and made generic
    switch (mv->section_current->memtype)
    {
    case MEMTYPE_Z80:   memory = NULL;      break;
    case MEMTYPE_ROM:   memory = ROM;       break;
    case MEMTYPE_RAM:   memory = RAM;       break;
    case MEMTYPE_VRAM:  memory = VRAM;      break;
    case MEMTYPE_VREG:  memory = sms.VDP;   break;
    case MEMTYPE_PRAM:  memory = PRAM;      break;
    case MEMTYPE_SRAM:
        {
            int dummy_len;
            BMemory_Get_Infos((void *)&memory, &dummy_len);
            break;
        }
    default:
        assert(0); 
        memory = NULL;
    }

    // Lines to separate address/hex/ascii
    // FIXME-SIZE
    {
        int x;
        x = 4 + font_height * 6 - 7 - 7;
        line (mv->box_gfx, x, 0, x, mv->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
        //x = 4 + font_height * 6 - 7 + (mv->size_columns * (font_height * 2 - 1) + font_height - 3) + font_height - 3 - 4;
        x = mv->frame_ascii.pos.x - 4;//MEMVIEW_COLUMNS_8_PADDING/2;
        line (mv->box_gfx, x, 0, x, mv->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
    }

    // Print current address
    // FIXME: Could create a label widget for this purpose.
    sprintf(buf, "%0*X", addr_length, addr_start + (mv->memblock_first * 16) + mv->values_edit_position);
    rectfill (mv->box_gfx, 56, mv->frame_view.size.y + 1 + 4, 91, mv->frame_view.size.y + 1 + 4 + Font_Height(font_id), COLOR_SKIN_WINDOW_BACKGROUND);
    Font_Print (font_id, mv->box_gfx, buf, 56, mv->frame_view.size.y + 1 + 4, COLOR_SKIN_WINDOW_TEXT);

    x = 4 + font_height * 6 - 7;
    y = 4;
    if (mv->section_current->size == 0)
    {
        char *text = "None";
        if (mv->section_current->memtype == MEMTYPE_Z80 && cur_drv->cpu != CPU_Z80)
            text = "You wish!";
        Font_Print (font_id, mv->box_gfx, text, x, y, COLOR_SKIN_WINDOW_TEXT);
    }
    else 
    {
        // Display all memory content lines
        for (row = 0; row != mv->size_lines; row++, y += font_height)
        {
            if (mv->memblock_first + row >= mv->memblocks_max)
                continue;

            // Print address
            sprintf(buf, "%0*X", addr_length, addr + addr_start);
            x = 4;
            Font_Print (font_id, mv->box_gfx, buf, x + (5 - addr_length) * (font_height - 2), y, COLOR_SKIN_WINDOW_TEXT);

            // Print 16-bytes in both hexadecimal and ASCII
            x = mv->frame_hex.pos.x;
            asciix = mv->frame_ascii.pos.x;

            for (col = 0; col != mv->size_columns; col++, x += font_height * (2) - 1, asciix += font_height - 2)
            {
                int color;

                // Space each 8 columns (for readability)
                if (col != 0 && ((col & 7) == 0))
                    x += MEMVIEW_COLUMNS_8_PADDING;

                // Get value
                if (section->memtype == MEMTYPE_Z80)
                    value = (machine & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00;
                else
                    value = memory ? memory[addr] : 0x00;

                // Print hexadecimal
                color = COLOR_SKIN_WINDOW_TEXT;
                sprintf(buf, "%02X", value);
                Font_Print (font_id, mv->box_gfx, buf, x, y, color);

                // Print ASCII
                if (mv->values_edit_active && (mv->values_edit_position == col + (row * mv->size_columns)))
                    color = COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT;
                buf[0] = isprint(value) ? value : '.';
                buf[1] = 0;
                Font_Print (font_id, mv->box_gfx, buf, asciix, y, color);

                addr++;
                if (addr >= section->size)
                    break;
            }
        }
    }

    // Refresh current value if edition cursor is at the beginning (no editing done yet)
    if (mv->values_edit_active)
    {
        if (widget_inputbox_get_cursor_pos(mv->values_edit_inputbox) == 0)
        {
            const int addr = (mv->memblock_first * mv->size_columns) + mv->values_edit_position;
            if (section->memtype == MEMTYPE_Z80)
                value = (machine & MACHINE_POWER_ON) ? RdZ80_NoHook(addr) : 0x00;
            else
                value = memory ? memory[addr] : 0x00;
            sprintf(buf, "%02X", value);
            widget_inputbox_set_value(mv->values_edit_inputbox, buf);
            widget_inputbox_set_cursor_pos(mv->values_edit_inputbox, 0);
        }
        
        // Always set dirty
        // This is a workaround to make sure that the widget is always rendered,
        // because we always clear the box manually
        widget_set_dirty(mv->values_edit_inputbox);
    }
}

// ACTION: ENABLE OR DISABLE MEMORY VIEWER -------------------------------------
static void MemoryViewer_Switch(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance
    if (mv == MemoryViewer_MainInstance)
    {
        MemoryViewer_SwitchMainInstance();
    }
    else
    {
        mv->active ^= 1;
        gui_box_show(mv->box, mv->active, TRUE);
        if (!mv->active)
        {
            // Flag GUI box for deletion
            mv->box->flags |= GUI_BOX_FLAGS_DELETE;
            return;
        }
    }
}

void    MemoryViewer_SwitchMainInstance()
{
    t_memory_viewer *mv = MemoryViewer_MainInstance;
    if (mv->active ^= 1)
        Msg(MSGT_USER, Msg_Get (MSG_MemoryEditor_Enabled));
    else
        Msg(MSGT_USER, Msg_Get (MSG_MemoryEditor_Disabled));
    gui_box_show(mv->box, mv->active, TRUE);
    gui_menu_inverse_check(menus_ID.tools, 4);
}

// ACTION: SWITCH TO THE DIFFERENT MEMORIES VIEWS -------------------------------

static void MemoryViewer_ViewSection(t_memory_viewer *mv, t_memory_section *section)
{
    if (mv->section_current == section)
        return;

    // Save bookmark
    mv->section_current->memblock_first = mv->memblock_first;

    // Update interface button
    widget_button_set_selected(mv->section_current->button, FALSE);
    widget_button_set_selected(section->button, TRUE);

    // Switch section
    mv->section_current    = section;
    mv->memblock_first     = section->memblock_first;
    mv->memblocks_max      = (section->size + mv->size_columns - 1) / mv->size_columns;
    assert(section->size >= 0);
    //if (mv->memblocks_max < mv->size_lines)
    //    mv->memblocks_max = mv->size_lines;
    MemoryViewer_SetupEditValueBox(mv);
}

static void      MemoryViewer_ViewZ80(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_Z80]);
}

static void      MemoryViewer_ViewROM(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_ROM]);
}

static void      MemoryViewer_ViewRAM(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_RAM]);
}

static void      MemoryViewer_ViewVRAM(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_VRAM]);
}

static void      MemoryViewer_ViewPRAM(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_PRAM]);
}

static void      MemoryViewer_ViewSRAM(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_SRAM]);
}

static void      MemoryViewer_ViewVREG(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
    MemoryViewer_ViewSection(mv, &mv->sections[MEMTYPE_VREG]);
}

static void MemoryViewer_MediaReload(t_memory_viewer *mv)
{
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
    z80_memblocks_max = mv->sections[MEMTYPE_Z80].size / mv->size_columns;
    if (mv->sections[MEMTYPE_Z80].memblock_first + mv->size_lines > z80_memblocks_max)
        mv->sections[MEMTYPE_Z80].memblock_first = MAX(z80_memblocks_max - mv->size_lines, 0);

    // ROM
    rom_memblocks_max = tsms.Size_ROM / mv->size_columns;
    mv->sections[MEMTYPE_ROM].size = tsms.Size_ROM;
    if (mv->sections[MEMTYPE_ROM].memblock_first + mv->size_lines > rom_memblocks_max)
        mv->sections[MEMTYPE_ROM].memblock_first = MAX(rom_memblocks_max - mv->size_lines, 0);

    // RAM
    Mapper_Get_RAM_Infos(&ram_len, &ram_start_addr);
    mv->sections[MEMTYPE_RAM].size = ram_len;
    mv->sections[MEMTYPE_RAM].addr_start = ram_start_addr;
    ram_memblocks_max = ram_len / mv->size_columns;
    if (mv->sections[MEMTYPE_RAM].memblock_first + mv->size_lines > ram_memblocks_max)
        mv->sections[MEMTYPE_RAM].memblock_first = MAX(ram_memblocks_max - mv->size_lines, 0);

    // PRAM
    switch (cur_machine.driver_id)
    {
    case DRV_SMS:   pram_len = 32;  break;
    case DRV_GG:    pram_len = 64;  break;
    default:        pram_len = 0;   break;
    }
    mv->sections[MEMTYPE_PRAM].size = pram_len;
    pram_memblocks_max = pram_len / mv->size_columns;
    if (mv->sections[MEMTYPE_PRAM].memblock_first + mv->size_lines > pram_memblocks_max)
        mv->sections[MEMTYPE_PRAM].memblock_first = MAX(pram_memblocks_max - mv->size_lines, 0);

    // SRAM
    BMemory_Get_Infos((void *)&sram_buf, &sram_len);
    mv->sections[MEMTYPE_SRAM].size = sram_len;
    sram_memblocks_max = sram_len / mv->size_columns;
    if (mv->sections[MEMTYPE_SRAM].memblock_first + mv->size_lines > sram_memblocks_max)
        mv->sections[MEMTYPE_SRAM].memblock_first = MAX(sram_memblocks_max - mv->size_lines, 0);

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

    MemoryViewer_SetupEditValueBox(mv);
    // MemoryViewer_Update();
}

//-----------------------------------------------------------------------------
// MemoryViewer_InputBoxAddress_EnterCallback(t_widget *w)
// Enter callback handler on 'address' input box.
// Set current cursor position to given address (if valid)
//-----------------------------------------------------------------------------
void      MemoryViewer_InputBoxAddress_EnterCallback(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    int     addr;
    const char *  text;

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
    mv->memblock_first = (addr / mv->size_columns) - (mv->size_lines/2);
	if (mv->memblock_first < 0)
		mv->memblock_first = 0;
    if (mv->memblock_first + mv->size_lines > mv->memblocks_max)
        mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
    mv->values_edit_active = TRUE;
	mv->values_edit_position = addr - (mv->memblock_first * mv->size_columns);
    MemoryViewer_SetupEditValueBox(mv);

	// Clear address box
	widget_inputbox_set_value(mv->address_edit_inputbox, "");
}

//-----------------------------------------------------------------------------
// MemoryViewer_InputBoxValue_EditCallback(t_widget *w)
// Edit callback handler of moving 'value' input box.
// When second digit is inputed, automatically call 'enter' callback,
// then move cursor to following location.
//-----------------------------------------------------------------------------
static void      MemoryViewer_InputBoxValue_EditCallback(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

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
        if (mv->values_edit_position < (mv->size_lines-1) * mv->size_columns - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        else if (((mv->values_edit_position % mv->size_columns) == (mv->size_columns - 1)) && mv->memblock_first + mv->size_lines < mv->memblocks_max)
        {
            mv->memblock_first++;
            mv->values_edit_position -= mv->size_columns - 1;
        }
        else if (mv->values_edit_position < mv->size_lines * mv->size_columns - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        MemoryViewer_SetupEditValueBox(mv);
    }
    // FIXME: when cursor reach 2, write and move to next memory location
}

//-----------------------------------------------------------------------------
// MemoryViewer_InputBoxValue_EnterCallback(t_widget *w)
// Enter callback handler of moving 'value' input box.
// Write inputed value at current memory location.
//-----------------------------------------------------------------------------
static void      MemoryViewer_InputBoxValue_EnterCallback(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    int     addr;
    int     value;
    const char *  text;

    text = widget_inputbox_get_value(mv->values_edit_inputbox);
    addr = (mv->memblock_first * mv->size_columns) + mv->values_edit_position;
    sscanf(text, "%X", &value);
    switch (mv->section_current->memtype)
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
                Write_Mapper_Coleco(0x6000 + addr, value);  // special case for Colecovision
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
    case MEMTYPE_VREG:
        {
            Tms_VDP_Out(addr, value);
            break;
        }
    }
    mv->values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void      MemoryViewer_ClickBottom(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    mv->values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void        MemoryViewer_ClickMemoryHex(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    int     x, y;
    int     i;

    // Msg (MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);

    // Clicking in empty columns disable edition
    for (i = 0; i < (mv->size_columns - 1) / 8; i++)
    {
        const int max_x = (i + 1) * (8 * (Font_Height(F_MIDDLE) * (2) - 1) + MEMVIEW_COLUMNS_8_PADDING);
        const int min_x = max_x - MEMVIEW_COLUMNS_8_PADDING;
        if (w->mouse_x >= min_x && w->mouse_x < max_x)
        {
            // Hide edit value input box
            mv->values_edit_active = FALSE;
            MemoryViewer_SetupEditValueBox(mv);
            return;
        }
    }

    // Inside
    // FIXME-SIZE
    x = w->mouse_x / (Font_Height(F_MIDDLE) * (2) - 1);
    x = (w->mouse_x - (x / 8) * MEMVIEW_COLUMNS_8_PADDING) / (Font_Height(F_MIDDLE) * (2) - 1);
    y = (w->mouse_y / Font_Height(F_MIDDLE));

    mv->values_edit_position = x + y * mv->size_columns;
    mv->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void        MemoryViewer_ClickMemoryAscii(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    int     x, y;

    x = w->mouse_x / (Font_Height(F_MIDDLE) - 2);
    y = w->mouse_y / Font_Height(F_MIDDLE);
    // Msg (MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);
    // Msg (MSGT_DEBUG, "x = %d, y = %d\n", x, y);

    mv->values_edit_position = x + y * mv->size_columns;
    mv->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void    MemoryViewer_SetupEditValueBox(t_memory_viewer *mv)
{
    // Disable if out of boundaries
    int addr = mv->memblock_first * mv->size_columns + mv->values_edit_position;
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
            frame->pos.x = (pos % mv->size_columns) * (Font_Height(F_MIDDLE) * 2 - 1);
            // Spacing every 8 bytes
            frame->pos.x += MEMVIEW_COLUMNS_8_PADDING * ((pos % mv->size_columns) / 8);
            frame->pos.y = (pos / mv->size_columns) * Font_Height(F_MIDDLE);
            frame->pos.x += mv->values_hex_box->frame.pos.x - 5; // Coordinates are parent relative
            frame->pos.y += mv->values_hex_box->frame.pos.y - 1;
        }

        // Show input box if not already active
        if (mv->values_edit_inputbox->update == NULL)
        {
            mv->values_edit_inputbox->update = widget_inputbox_update;
            widget_enable(mv->values_edit_inputbox);
            mv->address_edit_inputbox->update = NULL;
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
                case MEMTYPE_VREG:  value = sms.VDP[addr];  break;
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
            mv->address_edit_inputbox->update = widget_inputbox_update;
            mv->values_edit_inputbox->update = NULL;
            widget_disable(mv->values_edit_inputbox);
        }
    }
}

//-----------------------------------------------------------------------------
// MemoryViewer_Update_Inputs (t_memory_viewer *mv)
// Poll and update user inputs.
//-----------------------------------------------------------------------------
// FIXME: This function is probably unnecessary big.
//-----------------------------------------------------------------------------
static void     MemoryViewer_Update_Inputs(t_memory_viewer *mv)
{
    // Check for focus
    if (!gui_box_has_focus (mv->box))
        return;

    if (Inputs_KeyPressed (KEY_HOME, FALSE))
    {
        mv->memblock_first = 0;
        mv->values_edit_position = 0;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed (KEY_END, FALSE))
    {
        mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
        mv->values_edit_position = MIN(mv->size_lines * mv->size_columns, mv->section_current->size - mv->memblock_first * mv->size_columns);
        if (mv->values_edit_position > 0)
            mv->values_edit_position--;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed_Repeat (KEY_PGUP, FALSE, 30, 3) || gui.mouse.z_rel > 0)
    {
        mv->memblock_first -= mv->size_lines;
        if (mv->memblock_first < 0)
        {
            mv->memblock_first = 0;
            // if (!(gui_mouse.z_rel > 0))
            //    mv->values_edit_position = mv->values_edit_position & 15;
        }
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed_Repeat (KEY_PGDN, FALSE, 30, 3) || gui.mouse.z_rel < 0)
    {
        mv->memblock_first += mv->size_lines;
        if (mv->memblock_first + mv->size_lines > mv->memblocks_max)
        {
            mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
            // if (!(gui_mouse.z_rel < 0))
            //    mv->values_edit_position = (15 * 16) + (mv->values_edit_position & 15);
        }
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed_Repeat (KEY_UP, FALSE, 30, 3))
    {
        if (mv->size_lines > 1 && mv->values_edit_position >= (mv->size_lines/2) * mv->size_columns)
            mv->values_edit_position -= mv->size_columns;
        else
        {
            mv->memblock_first--;
            if (mv->memblock_first < 0)
            {
                mv->memblock_first = 0;
                if (mv->values_edit_position >= mv->size_columns)
                    mv->values_edit_position -= mv->size_columns;
            }
        }
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed_Repeat (KEY_DOWN, FALSE, 30, 3))
    {
        if (mv->values_edit_position < (mv->size_lines/2-1) * mv->size_columns)
        {
            mv->values_edit_position += mv->size_columns;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position -= mv->size_columns;
        }
        else
        {
            mv->memblock_first++;
            if (mv->memblock_first + mv->size_lines > mv->memblocks_max)
            {
                mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
                if (mv->values_edit_position < (mv->size_lines-1) * mv->size_columns)
                    mv->values_edit_position += mv->size_columns;
            }
        }
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat (KEY_LEFT, FALSE, 30, 3))
    {
        if (mv->values_edit_position > 1 * mv->size_columns)
            mv->values_edit_position--;
        else if (mv->memblock_first > 0 && (mv->size_lines > 1 || mv->values_edit_position == 0))
        {
            mv->memblock_first--;
            mv->values_edit_position += mv->size_columns - 1;
        }
        else if (mv->values_edit_position > 0)
            mv->values_edit_position--;
        // mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat (KEY_RIGHT, FALSE, 30, 3))
    {
        if (mv->values_edit_position < (mv->size_lines-1) * mv->size_columns - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        else if (((mv->values_edit_position % mv->size_columns) == (mv->size_columns - 1)) && mv->memblock_first + mv->size_lines < mv->memblocks_max)
        {
            mv->memblock_first++;
            mv->values_edit_position -= mv->size_columns - 1;
        }
        else if (mv->values_edit_position < (mv->size_lines * mv->size_columns) - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->section_current->size)
                mv->values_edit_position --;
        }
        // mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }

    // Limits
    if (mv->memblock_first + mv->size_lines > mv->memblocks_max)
        mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
    else if (mv->memblock_first < 0)
        mv->memblock_first = 0;
}

//-----------------------------------------------------------------------------

void      MemoryViewers_Update(void)
{
    t_list *mvs;
    for (mvs = MemoryViewers; mvs != NULL; mvs = mvs->next)
    {
        t_memory_viewer *mv = (t_memory_viewer *)mvs->elem;
        MemoryViewer_Update(mv);
    }
}

void      MemoryViewers_MediaReload(void)
{
    t_list *mvs;
    for (mvs = MemoryViewers; mvs != NULL; mvs = mvs->next)
    {
        t_memory_viewer *mv = (t_memory_viewer *)mvs->elem;
        MemoryViewer_MediaReload(mv);
    }
}

//-----------------------------------------------------------------------------
