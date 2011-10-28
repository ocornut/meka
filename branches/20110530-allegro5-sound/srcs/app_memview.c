//-----------------------------------------------------------------------------
// MEKA - app_memview.c
// Memory Viewer - Code
//-----------------------------------------------------------------------------
// Note: currently referred as "Memory Viewer" in the code, but always as "Memory Editor" to the user.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "bmemory.h"
#include "coleco.h"
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
static void     MemoryViewer_UpdateInputs(t_memory_viewer *mv);
static void     MemoryViewer_MediaReload(t_memory_viewer *mv);
static void		MemoryViewer_UpdateAllMemoryRanges(t_memory_viewer *mv);

static void     MemoryViewer_Switch(t_widget *w);

static void		MemoryViewer_ViewPane(t_memory_viewer *mv, t_memory_type memtype);
static void     MemoryViewer_ViewPaneCallback(t_widget *w);

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

void	MemoryRange_GetDetails(t_memory_type memtype, t_memory_range* mr)
{
	mr->memtype = memtype;
	switch (memtype)
	{
	case MEMTYPE_Z80:
		{
			mr->name			= "Z80";
			mr->size			= 0x10000;
			mr->addr_start		= 0x0000;
			mr->addr_hex_length	= 4;
			mr->data			= NULL;
			break;
		}
	case MEMTYPE_ROM:
		{
			mr->name			= "ROM";
			mr->size			= tsms.Size_ROM;
			mr->addr_start		= 0x00000;
			mr->addr_hex_length	= 5;
			mr->data			= ROM;
			break;
		}
	case MEMTYPE_RAM:
		{
			int ram_len, ram_start_addr;
			Mapper_Get_RAM_Infos(&ram_len, &ram_start_addr);
			mr->name			= "RAM";
			mr->size			= ram_len;
			mr->addr_start		= ram_start_addr;
			mr->addr_hex_length	= 4;
			mr->data			= RAM;
			break;
		}
	case MEMTYPE_VRAM:
		{
			mr->name			= "VRAM";
			mr->size			= 0x4000;
			mr->addr_start		= 0x0000;
			mr->addr_hex_length	= 4;
			mr->data			= VRAM;
			break;
		}
	case MEMTYPE_PRAM:
		{
			int pram_size = 0;
			switch (g_machine.driver_id)
			{
			case DRV_SMS:   pram_size = 32;  break;
			case DRV_GG:    pram_size = 64;  break;
			}
			mr->name			= "PAL";	// FIXME: "PRAM" ?
			mr->size			= pram_size;
			mr->addr_start		= 0x00;
			mr->addr_hex_length	= 2;
			mr->data			= PRAM;
			break;
		}
	case MEMTYPE_SRAM:
		{
			void* sram_data;
			int sram_size;
			BMemory_Get_Infos((void **)&sram_data, &sram_size);
			mr->name			= "Save";	// FIXME: "SRAM"?
			mr->size			= sram_size;
			mr->addr_start		= 0x0000;
			mr->addr_hex_length	= 4;
			mr->data			= (u8*)sram_data;
			break;
		}
	case MEMTYPE_VREG:
		{
			mr->name			= "VREG";
			mr->size			= 11;
			mr->addr_start		= 0x0000;
			mr->addr_hex_length	= 1;
			mr->data			= &sms.VDP[0];
			break;
		}
	default:
		assert(0);
	}
}

u8		t_memory_range::ReadByte(int addr) const
{
	assert(addr >= 0 && addr < this->size);
	switch (this->memtype)
	{
		case MEMTYPE_Z80: 
			if (!(g_machine_flags & MACHINE_POWER_ON))
				return 0x00;
			return RdZ80_NoHook(addr);
		case MEMTYPE_ROM: 
		case MEMTYPE_RAM: 
		case MEMTYPE_VRAM: 
		case MEMTYPE_PRAM:
		case MEMTYPE_SRAM:
		case MEMTYPE_VREG:
			return this->data[addr];
	}
	assert(0);
	return 0x00;
}

bool	t_memory_range::WriteByte(int addr, u8 v)
{
	assert(addr >= 0 && addr < this->size);
	switch (this->memtype)
	{
	case MEMTYPE_Z80:
		{
			if (!(g_machine_flags & MACHINE_POWER_ON))
				return false;
			WrZ80_NoHook(addr, v);
			return true;
		}
	case MEMTYPE_ROM:     
		{
			this->data[addr] = v;
			// We have a special handling there, because SMS/GG mapper emulation is using a 
			// different memory area for addresses between 0x0000 and 0x0400
			if (addr < 0x4000)
				if (Mem_Pages[0] == Game_ROM_Computed_Page_0)
					Game_ROM_Computed_Page_0[addr] = v;
			return true;
		}
	case MEMTYPE_RAM:
		{
			if (g_driver->id == DRV_COLECO)
				Write_Mapper_Coleco(0x6000 + addr, v);  // special case for ColecoVision crazy mirroring
			else
				this->data[addr] = v;
			return true;
		}
	case MEMTYPE_VRAM:    
		{
			// Mark corresponding tile as dirty
			this->data[addr] = v;     
			tgfx.Tile_Dirty[addr >> 5] |= TILE_DIRTY_DECODE;
			return true;
		}
	case MEMTYPE_PRAM:
		{
			Tms_VDP_Palette_Write(addr, v);
			return true;
		}
	case MEMTYPE_SRAM:
		{
			this->data[addr] = v;
			return true;
		}
	case MEMTYPE_VREG:
		{
			Tms_VDP_Out(addr, v);
			return true;
		}
	}
	assert(0);
	return false;
}


//-----------------------------------------------------------------------------
// MemoryViewer_New(bool register_desktop, int size_columns, int size_lines)
// Create and initialize Memory Editor applet
//-----------------------------------------------------------------------------
t_memory_viewer *   MemoryViewer_New(bool register_desktop, int size_columns, int size_lines)
{
    t_memory_viewer* mv = (t_memory_viewer*)malloc(sizeof(t_memory_viewer));

    // Check parameters
    assert(size_columns >= -1);
    assert(size_lines >= -1);

    // Add to global list
    list_add(&MemoryViewers, mv);

    // Setup members
    mv->active = TRUE;
    mv->size_columns    = (size_columns != -1) ? size_columns : g_configuration.memory_editor_columns;
    mv->size_lines      = (size_lines != -1)   ? size_lines   : g_configuration.memory_editor_lines;

    mv->frame_hex.pos.x  = 4 + (Font_Height(F_MIDDLE) * 6) - 7;
    mv->frame_hex.pos.y  = 4;
    mv->frame_hex.size.x = (mv->size_columns * (Font_Height(F_MIDDLE) * (2) - 1)) + ((mv->size_columns - 1) / 8) * MEMVIEW_COLUMNS_8_PADDING;
    mv->frame_hex.size.y = mv->size_lines * Font_Height(F_MIDDLE);

    mv->frame_ascii.pos.x  = mv->frame_hex.pos.x + mv->frame_hex.size.x + MEMVIEW_COLUMNS_8_PADDING;
    mv->frame_ascii.pos.y  = 4;
    mv->frame_ascii.size.x = (mv->size_columns * (Font_Height(F_MIDDLE) - 2));
    mv->frame_ascii.size.y = mv->size_lines * Font_Height(F_MIDDLE);

    // Create box
    mv->frame_view.pos.x = 10;
    mv->frame_view.pos.y  = 395;
    // A column is made with 2 figures plus a space (except the last one)
    mv->frame_view.size.x = 4;
    // Start of the line: "XXXXX:" (6 characters)
    mv->frame_view.size.x += (Font_Height(F_MIDDLE) * 6) - 7;   // Address
    mv->frame_view.size.x += mv->frame_hex.size.x;              // Hexadecimal
    mv->frame_view.size.x += MEMVIEW_COLUMNS_8_PADDING;         // Padding
    mv->frame_view.size.x += mv->frame_ascii.size.x;            // ASCII
    mv->frame_view.size.x += 4;                                 // Padding
    mv->frame_view.size.y = 4 + mv->frame_hex.size.y + 3;

	t_frame frame;
    frame = mv->frame_view;
    // Scrollbar
    frame.size.x += 7;
    // Bottom bar
    frame.size.y += 1 + 1 + Font_Height(F_SMALL) + 2;

    mv->box = gui_box_new(&frame, Msg_Get(MSG_MemoryEditor_BoxTitle));
    mv->box->user_data = mv;
    mv->box->destroy = (t_gui_box_destroy_handler)MemoryViewer_Delete;
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
    mv->pane_current = &mv->panes[MEMTYPE_ROM];   // anything but RAM (see below)

    // Start by viewing RAM
	MemoryViewer_ViewPane(mv, MEMTYPE_RAM);

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

	ALLEGRO_BITMAP* box_gfx = mv->box->gfx_buffer;
	al_set_target_bitmap(mv->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(mv->box, MemoryViewer_Switch);
        
    // Horizontal line to separate buttons from memory
    al_draw_hline(0, mv->frame_view.size.y, mv->box->frame.size.x, COLOR_SKIN_WINDOW_SEPARATORS);

    // Setup Memory sections
    if (setup)
    {
        frame.pos.x = 179;
        frame.pos.y = mv->frame_view.size.y + 1;
        frame.size.x = 30;
        frame.size.y = Font_Height (F_SMALL) + 3;

		for (int i = 0; i != MEMTYPE_MAX_; i++)
		{
			t_memory_pane* pane = &mv->panes[i];
			t_memory_type memtype = (t_memory_type)i;
			MemoryRange_GetDetails(memtype, &pane->memrange);
			pane->memblock_first = 0;
			pane->button = widget_button_add(mv->box, &frame, 1, MemoryViewer_ViewPaneCallback, WIDGET_BUTTON_STYLE_SMALL, (char *)pane->memrange.name, (void*)memtype);

			frame.pos.x += frame.size.x + 2;
		}
    }

    // Invisible Button to catch click on bottom (to cancel editing)
    frame.SetPos(0, mv->frame_view.size.y);
    frame.size.x = mv->box->frame.size.x;
    frame.size.y = mv->box->frame.size.y - mv->frame_view.size.y;
    if (setup)
        mv->bottom_box = widget_button_add(mv->box, &frame, 1, MemoryViewer_ClickBottom, WIDGET_BUTTON_STYLE_INVISIBLE, NULL);

    // Address text
    frame.pos.y = mv->frame_view.size.y + 1;
    frame.size.y = Font_Height(F_SMALL) + 3;
    Font_Print(F_MIDDLE, "Address:", 5, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
    al_draw_vline(92, frame.pos.y, frame.pos.y + frame.size.y, COLOR_SKIN_WINDOW_SEPARATORS);

    // Goto Address input box
    Font_Print(F_MIDDLE, "Goto", 100, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
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
        mv->widget_scrollbar = widget_scrollbar_add(mv->box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &mv->memblocks_max, &mv->memblock_first, mv->size_lines, NULL);

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
        mv->values_edit_inputbox->update_func = NULL;
        widget_set_enabled(mv->values_edit_inputbox, false);
    }
}

//-----------------------------------------------------------------------------
// MemoryViewer_Update(t_memory_viewer *mv)
// Refresh Memory Editor
//-----------------------------------------------------------------------------
static void        MemoryViewer_Update(t_memory_viewer *mv)
{
    t_memory_pane *pane = mv->pane_current;

    char            buf[9];
    const t_font_id	font_id = F_MIDDLE;
    const int       font_height = Font_Height(font_id);
    const int       addr_length = pane->memrange.addr_hex_length;
    const int       addr_start  = pane->memrange.addr_start;

    // Skip update if not active
    if (!mv->active)
        return;

	MemoryViewer_UpdateAllMemoryRanges(mv);

    // If skin has changed, redraw everything
	ALLEGRO_BITMAP* box_gfx = mv->box->gfx_buffer;
	al_set_target_bitmap(box_gfx);
    if (mv->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        MemoryViewer_Layout(mv, FALSE);
        mv->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
    else
    {
        // Clear anyway
        al_draw_filled_rectangle(0, 0, mv->frame_view.size.x - 1, mv->frame_view.size.y, COLOR_SKIN_WINDOW_BACKGROUND);
    }

    // Always dirty (FIXME?)
    mv->box->flags |= GUI_BOX_FLAGS_DIRTY_REDRAW;

    // Update inputs
    MemoryViewer_UpdateInputs(mv);

    int y = 4;
    int addr = mv->memblock_first * mv->size_columns;
    
	const u8* memory = mv->pane_current->memrange.data;

    // Lines to separate address/hex/ascii
    // FIXME-SIZE
    {
        int x;
        x = 4 + font_height * 6 - 7 - 7;
        al_draw_vline(x, 0, mv->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
        //x = 4 + font_height * 6 - 7 + (mv->size_columns * (font_height * 2 - 1) + font_height - 3) + font_height - 3 - 4;
        x = mv->frame_ascii.pos.x - 4;//MEMVIEW_COLUMNS_8_PADDING/2;
        al_draw_vline(x, 0, mv->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
    }

    // Print current address
    // FIXME: Could create a label widget for this purpose.
    sprintf(buf, "%0*X", addr_length, addr_start + (mv->memblock_first * 16) + mv->values_edit_position);
    al_draw_filled_rectangle(56, mv->frame_view.size.y + 1 + 4, 91+1, mv->frame_view.size.y + 1 + 4 + Font_Height(font_id) + 1, COLOR_SKIN_WINDOW_BACKGROUND);
    Font_Print(font_id, buf, 56, mv->frame_view.size.y + 1 + 4, COLOR_SKIN_WINDOW_TEXT);

    int x = 4 + font_height * 6 - 7;
    y = 4;
    if (mv->pane_current->memrange.size == 0)
    {
        const char *text = "None";
        if (mv->pane_current->memrange.memtype == MEMTYPE_Z80 && g_driver->cpu != CPU_Z80)
            text = "You wish!";
        Font_Print(font_id, text, x, y, COLOR_SKIN_WINDOW_TEXT);
    }
    else 
    {
        // Display all memory content lines
        for (int row = 0; row != mv->size_lines; row++, y += font_height)
        {
            if (mv->memblock_first + row >= mv->memblocks_max)
                continue;

            // Print address
            sprintf(buf, "%0*X", addr_length, addr + addr_start);
            x = 4;
            Font_Print(font_id, buf, x + (5 - addr_length) * (font_height - 2), y, COLOR_SKIN_WINDOW_TEXT);

            // Print 16-bytes in both hexadecimal and ASCII
            x = mv->frame_hex.pos.x;
            int asciix = mv->frame_ascii.pos.x;

			for (int col = 0; col != mv->size_columns; col++, x += font_height * (2) - 1, asciix += font_height - 2)
            {
                // Space each 8 columns (for readability)
                if (col != 0 && ((col & 7) == 0))
                    x += MEMVIEW_COLUMNS_8_PADDING;

                // Get value
				const u8 v = pane->memrange.ReadByte(addr);

                // Print hexadecimal
                ALLEGRO_COLOR color = COLOR_SKIN_WINDOW_TEXT;
                sprintf(buf, "%02X", v);
                Font_Print(font_id, buf, x, y, color);

                // Print ASCII
                if (mv->values_edit_active && (mv->values_edit_position == col + (row * mv->size_columns)))
                    color = COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT;
                buf[0] = isprint(v) ? v : '.';
                buf[1] = 0;
                Font_Print(font_id, buf, asciix, y, color);

                addr++;
                if (addr >= (int)pane->memrange.size)
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
			const u8 v = pane->memrange.ReadByte(addr);
            sprintf(buf, "%02X", v);
            widget_inputbox_set_value(mv->values_edit_inputbox, buf);
            widget_inputbox_set_cursor_pos(mv->values_edit_inputbox, 0);
        }
    }
}

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

static void MemoryViewer_ViewPane(t_memory_viewer *mv, t_memory_type memtype)
{
	t_memory_pane* pane = &mv->panes[memtype];
    if (mv->pane_current == pane)
        return;

    // Save bookmark
    mv->pane_current->memblock_first = mv->memblock_first;

    // Update interface button
    widget_button_set_selected(mv->pane_current->button, FALSE);
    widget_button_set_selected(pane->button, TRUE);

    // Switch section
    mv->pane_current    = pane;
    mv->memblock_first	= pane->memblock_first;
    mv->memblocks_max   = (pane->memrange.size + mv->size_columns - 1) / mv->size_columns;
    assert(pane->memrange.size >= 0);
    //if (mv->memblocks_max < mv->size_lines)
    //    mv->memblocks_max = mv->size_lines;
    MemoryViewer_SetupEditValueBox(mv);
}

static void MemoryViewer_ViewPaneCallback(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data;
	t_memory_type memtype = (t_memory_type)(int)w->user_data;
    MemoryViewer_ViewPane(mv, memtype);
}

static void MemoryViewer_UpdateAllMemoryRanges(t_memory_viewer *mv)
{
	for (int i = 0; i != MEMTYPE_MAX_; i++)
	{
		t_memory_pane* pane = &mv->panes[i];
		MemoryRange_GetDetails((t_memory_type)i, &pane->memrange);

		const int memblocks_max = (pane->memrange.size + mv->size_columns - 1) / mv->size_columns;
		if (pane->memblock_first + mv->size_lines > memblocks_max)
			pane->memblock_first = MAX(memblocks_max - mv->size_lines, 0);

		if (mv->pane_current == pane)
			mv->memblocks_max = memblocks_max;
	}
}

static void MemoryViewer_MediaReload(t_memory_viewer *mv)
{
	MemoryViewer_UpdateAllMemoryRanges(mv);
    MemoryViewer_SetupEditValueBox(mv);
}

//-----------------------------------------------------------------------------
// MemoryViewer_InputBoxAddress_EnterCallback(t_widget *w)
// Enter callback handler on 'address' input box.
// Set current cursor position to given address (if valid)
//-----------------------------------------------------------------------------
void      MemoryViewer_InputBoxAddress_EnterCallback(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    // Read address
    const char* text = widget_inputbox_get_value(mv->address_edit_inputbox);
	int addr;
    sscanf(text, "%X", &addr);

    // Check boundaries
	t_memory_pane* pane = mv->pane_current;
	const t_memory_range* memrange = &pane->memrange;
    if (addr < memrange->addr_start || addr >= memrange->addr_start + (int)memrange->size)
    {
        char buf[12];
        sprintf(buf, "%0*X", memrange->addr_hex_length, addr);
        Msg (MSGT_USER, Msg_Get(MSG_MemoryEditor_Address_Out_of_Bound), buf, memrange->name);
        return;
    }

    // Jump to given address
    addr -= memrange->addr_start;
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

    // When cursor reach 2, switch to next byte
    const int cursor = widget_inputbox_get_cursor_pos(mv->values_edit_inputbox);
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
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->pane_current->memrange.size)
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
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->pane_current->memrange.size)
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

    const char* text = widget_inputbox_get_value(mv->values_edit_inputbox);
    const int addr = (mv->memblock_first * mv->size_columns) + mv->values_edit_position;
	int value;
    sscanf(text, "%X", &value);
	const bool ok = mv->pane_current->memrange.WriteByte(addr, (u8)value);
	if (!ok)
	{
		assert(mv->pane_current->memrange.memtype == MEMTYPE_Z80);
		Msg(MSGT_USER, Msg_Get(MSG_MemoryEditor_WriteZ80_Unable));
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

    // Msg (MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);

    // Clicking in empty columns disable edition
    for (int i = 0; i < (mv->size_columns - 1) / 8; i++)
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
	int x, y;
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

    int x = w->mouse_x / (Font_Height(F_MIDDLE) - 2);
    int y = w->mouse_y / Font_Height(F_MIDDLE);
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
    if (addr < 0 || addr >= mv->pane_current->memrange.size)
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
        if (mv->values_edit_inputbox->update_func == NULL)
        {
            mv->values_edit_inputbox->update_func = widget_inputbox_update;
            widget_set_enabled(mv->values_edit_inputbox, true);
            mv->address_edit_inputbox->update_func = NULL;
        }

        // Setup input box default content
        u8 value = mv->pane_current->memrange.ReadByte(addr);
        char buf[3];
        sprintf(buf, "%02X", value);
        widget_inputbox_set_value(mv->values_edit_inputbox, buf);
        widget_inputbox_set_cursor_pos(mv->values_edit_inputbox, 0);
    }
    else
    {
        // Hide input box if active
        if (mv->values_edit_inputbox->update_func != NULL)
        {
            mv->address_edit_inputbox->update_func = widget_inputbox_update;
            mv->values_edit_inputbox->update_func = NULL;
            widget_set_enabled(mv->values_edit_inputbox, false);
        }
    }
}

// Poll and update user inputs.
// FIXME: This function is probably unnecessary big.
static void     MemoryViewer_UpdateInputs(t_memory_viewer *mv)
{
    // Check for focus
    if (!gui_box_has_focus (mv->box))
        return;

    if (Inputs_KeyPressed(ALLEGRO_KEY_HOME, FALSE))
    {
        mv->memblock_first = 0;
        mv->values_edit_position = 0;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed(ALLEGRO_KEY_END, FALSE))
    {
        mv->memblock_first = MAX(mv->memblocks_max - mv->size_lines, 0);
        mv->values_edit_position = MIN(mv->size_lines * mv->size_columns, mv->pane_current->memrange.size - mv->memblock_first * mv->size_columns);
        if (mv->values_edit_position > 0)
            mv->values_edit_position--;
        mv->values_edit_active = TRUE;
        MemoryViewer_SetupEditValueBox(mv);
    }
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGUP, FALSE, 30, 3) || gui.mouse.z_rel > 0)
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
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGDN, FALSE, 30, 3) || gui.mouse.z_rel < 0)
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
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_UP, FALSE, 30, 3))
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
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_DOWN, FALSE, 30, 3))
    {
        if (mv->values_edit_position < (mv->size_lines/2-1) * mv->size_columns)
        {
            mv->values_edit_position += mv->size_columns;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->pane_current->memrange.size)
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
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat(ALLEGRO_KEY_LEFT, FALSE, 30, 3))
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
    else if (mv->values_edit_active && Inputs_KeyPressed_Repeat(ALLEGRO_KEY_RIGHT, FALSE, 30, 3))
    {
        if (mv->values_edit_position < (mv->size_lines-1) * mv->size_columns - 1)
        {
            mv->values_edit_position++;
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->pane_current->memrange.size)
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
            if (mv->memblock_first * mv->size_columns + mv->values_edit_position >= mv->pane_current->memrange.size)
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
