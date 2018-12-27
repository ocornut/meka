//-----------------------------------------------------------------------------
// MEKA - app_memview.c
// Memory Viewer - Code
//-----------------------------------------------------------------------------
// Note: currently referred as "Memory Viewer" in the code, but always as "Memory Editor" to the user.
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_memview.h"
#include "app_cheatfinder.h"
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
			if (addr >= 0 && addr < this->size)
				return this->data[addr];
			return 0xFF;
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


// Create and initialize Memory Editor applet
t_memory_viewer *   MemoryViewer_New(bool register_desktop, int size_columns, int size_lines)
{
    t_memory_viewer* app = (t_memory_viewer*)malloc(sizeof(t_memory_viewer));

    // Check parameters
    assert(size_columns >= -1);
    assert(size_lines >= -1);

    // Add to global list
    list_add(&MemoryViewers, app);

    // Setup members
    app->active = TRUE;
    app->size_columns    = (size_columns != -1) ? size_columns : g_configuration.memory_editor_columns;
    app->size_lines      = (size_lines != -1)   ? size_lines   : g_configuration.memory_editor_lines;

	app->frame_hex.size.x = (app->size_columns * (Font_Height(FONTID_MEDIUM) * (2) - 1)) + ((app->size_columns - 1) / 8) * MEMVIEW_COLUMNS_8_PADDING;
	app->frame_hex.size.y = app->size_lines * Font_Height(FONTID_MEDIUM);
	app->frame_ascii.size.x = (app->size_columns * (Font_Height(FONTID_MEDIUM) - 2));
	app->frame_ascii.size.y = app->size_lines * Font_Height(FONTID_MEDIUM);

	// Default size
	t_frame frame;
	frame.pos.x = 10;
	frame.pos.y = 395;

	frame.size.x = 4;
	frame.size.x += (Font_Height(FONTID_MEDIUM) * 6) - 7;	// Address
	frame.size.x += app->frame_hex.size.x;					// Hexadecimal
	frame.size.x += MEMVIEW_COLUMNS_8_PADDING;				// Padding
	frame.size.x += app->frame_ascii.size.x;				// ASCII
	frame.size.x += 4 + 7;									// Padding, Scrollbar
	
	frame.size.y = 4 + app->frame_hex.size.y + 3;
    frame.size.y += 1 + 1 + Font_Height(FONTID_SMALL) + 2;	// Bottom bar

	// Create box
    app->box = gui_box_new(&frame, Msg_Get(MSG_MemoryEditor_BoxTitle));
    app->box->user_data = app;
    app->box->destroy = (t_gui_box_destroy_handler)MemoryViewer_Delete;
	// app->box->focus_inputs_exclusive = TRUE; // Set exclusive inputs flag to avoid messing with emulation
    app->box->flags |= GUI_BOX_FLAGS_TAB_STOP;
	app->box->flags |= GUI_BOX_FLAGS_ALLOW_RESIZE;
	app->box->size_min.x = app->box->frame.size.x;
	app->box->size_min.y = 32;
	app->box->size_max.x = app->box->frame.size.x;
	app->box->size_max.y = 9999;

    // Register to desktop (applet is disabled by default)
    if (register_desktop)
	{
        Desktop_Register_Box("MEMORY", app->box, FALSE, &app->active);
	}

    // Layout
    MemoryViewer_Layout(app, true);

    // Setup other members
    app->memblock_first = 0;
    app->memblocks_max = 0;
    app->values_edit_active = false;
    app->values_edit_position = 0;
    app->pane_current = &app->panes[MEMTYPE_ROM];   // anything but RAM (see below)

    // Start by viewing RAM
	MemoryViewer_ViewPane(app, MEMTYPE_RAM);

    // Simulate a ROM load, so that default mapping setting are applied
    // (this is for when checking the Memory Editor on startup before loading a ROM)
    MemoryViewer_MediaReload(app);

    // Return new instance
    return (app);
}

void	MemoryViewer_Delete(t_memory_viewer* app)
{
    list_remove(&MemoryViewers, app);
    free(app);
}

static void MemoryViewer_Layout(t_memory_viewer* app, bool setup)
{
	{
		const float fh = Font_Height(FONTID_MEDIUM);

		int contents_y = app->box->frame.size.y;
		contents_y -= (4 + 3);
		contents_y -= (1 + 1) + Font_Height(FONTID_SMALL) + 2;	// Bottom box
		app->size_lines = contents_y / fh;

		app->frame_hex.pos.x  = 4 + (fh * 6) - 7;
		app->frame_hex.pos.y  = 4;
		app->frame_hex.size.x = (app->size_columns * (fh * 2 - 1)) + ((app->size_columns - 1) / 8) * MEMVIEW_COLUMNS_8_PADDING;
		app->frame_hex.size.y = app->size_lines * fh;

		app->frame_ascii.pos.x  = app->frame_hex.pos.x + app->frame_hex.size.x + MEMVIEW_COLUMNS_8_PADDING;
		app->frame_ascii.pos.y  = 4;
		app->frame_ascii.size.x = (app->size_columns * (fh - 2));
		app->frame_ascii.size.y = app->size_lines * fh;

		// A column is made with 2 figures plus a space (except the last one)
		app->frame_view.pos.x = 0;
		app->frame_view.pos.y = 0;
		app->frame_view.size.x = 4;
		// Start of the line: "XXXXX:" (6 characters)
		app->frame_view.size.x += (fh * 6) - 7;					// Address
		app->frame_view.size.x += app->frame_hex.size.x;		// Hexadecimal
		app->frame_view.size.x += MEMVIEW_COLUMNS_8_PADDING;	// Padding
		app->frame_view.size.x += app->frame_ascii.size.x;		// ASCII
		app->frame_view.size.x += 4;							// Padding
		app->frame_view.size.y = contents_y + 4 + 3;//4 + app->frame_hex.size.y + 3;
	}

	al_set_target_bitmap(app->box->gfx_buffer);
    al_clear_to_color(COLOR_SKIN_WINDOW_BACKGROUND);

    // Add closebox widget
    if (setup)
        widget_closebox_add(app->box, MemoryViewer_Switch);
        
    // Horizontal line to separate buttons from memory
    al_draw_hline(0, app->frame_view.size.y, app->box->frame.size.x, COLOR_SKIN_WINDOW_SEPARATORS);

    // Setup Memory sections
    {
		t_frame frame;
        frame.pos.x = 174;
        frame.pos.y = app->frame_view.size.y + 1;
        frame.size.x = 30;
        frame.size.y = Font_Height(FONTID_SMALL) + 3;

		for (int i = 0; i != MEMTYPE_MAX_; i++)
		{
			t_memory_pane* pane = &app->panes[i];
			t_memory_type memtype = (t_memory_type)i;
			MemoryRange_GetDetails(memtype, &pane->memrange);
			pane->memblock_first = 0;

			if (setup)
				pane->button = widget_button_add(app->box, &frame, 1, MemoryViewer_ViewPaneCallback, FONTID_SMALL, (char *)pane->memrange.name, (void*)memtype);
			else
				pane->button->frame = frame;

			frame.pos.x += frame.size.x + 2;
		}
    }

    // Invisible Button to catch click on bottom (to cancel editing)
	t_frame frame;
    frame.SetPos(0, app->frame_view.size.y);
    frame.size.x = app->box->frame.size.x - 16;
    frame.size.y = app->box->frame.size.y - app->frame_view.size.y;
    if (setup)
        app->bottom_box = widget_button_add(app->box, &frame, 1, MemoryViewer_ClickBottom, FONTID_NONE, NULL);
	else
		app->bottom_box->frame = frame;

    // Address text
    frame.pos.y = app->frame_view.size.y + 1;
    frame.size.y = Font_Height(FONTID_SMALL) + 3;
    Font_Print(FONTID_MEDIUM, "Address:", 5, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
    al_draw_vline(92, frame.pos.y, frame.pos.y + frame.size.y, COLOR_SKIN_WINDOW_SEPARATORS);

    // Goto Address input box
    Font_Print(FONTID_MEDIUM, "Goto", 100, frame.pos.y + 4, COLOR_SKIN_WINDOW_TEXT);
    frame.pos.x = 128;
    frame.size.x = 40;
	if (setup)
	{
        app->address_edit_inputbox = widget_inputbox_add(app->box, &frame, 5, FONTID_MEDIUM, MemoryViewer_InputBoxAddress_EnterCallback);
        widget_inputbox_set_content_type(app->address_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);
    }
	else
	{
		app->address_edit_inputbox->frame = frame;
	}

    // Invisible buttons to catch click on memory for value editing
    if (setup)
    {
        app->values_hex_box = widget_button_add(app->box, &app->frame_hex, 1, MemoryViewer_ClickMemoryHex, FONTID_NONE, NULL);
        app->values_ascii_box = widget_button_add(app->box, &app->frame_ascii, 1, MemoryViewer_ClickMemoryAscii, FONTID_NONE, NULL);
    }
	else
	{
		app->values_hex_box->frame = app->frame_hex;
		app->values_ascii_box->frame = app->frame_ascii;
	}

    // Scrollbar
    frame.pos.x = app->frame_view.size.x;
    frame.pos.y = 0;
    frame.size.x = 7;
    frame.size.y = app->frame_view.size.y - 1;
    if (setup)
	{
        app->widget_scrollbar = widget_scrollbar_add(app->box, WIDGET_SCROLLBAR_TYPE_VERTICAL, &frame, &app->memblocks_max, &app->memblock_first, app->size_lines, NULL);
	}
	else
	{
		app->widget_scrollbar->frame = frame;
		widget_scrollbar_set_page_step(app->widget_scrollbar, app->size_lines);
	}

    // Input box for memory values
    frame.pos.x = 0;
    frame.pos.y = 0;
    frame.size.x = Font_Height(FONTID_MEDIUM) * 2 + 3;
    frame.size.y = Font_Height(FONTID_MEDIUM);
	if (setup)
	{
        app->values_edit_inputbox = widget_inputbox_add(app->box, &frame, 2, FONTID_MEDIUM, MemoryViewer_InputBoxValue_EnterCallback);
        widget_inputbox_set_callback_edit(app->values_edit_inputbox, MemoryViewer_InputBoxValue_EditCallback);
        widget_inputbox_set_flags(app->values_edit_inputbox, WIDGET_INPUTBOX_FLAGS_NO_CURSOR | WIDGET_INPUTBOX_FLAGS_NO_MOVE_CURSOR | WIDGET_INPUTBOX_FLAGS_NO_DELETE | WIDGET_INPUTBOX_FLAGS_NO_SELECTION | WIDGET_INPUTBOX_FLAGS_HIGHLIGHT_CURRENT_CHAR, TRUE);
        widget_inputbox_set_content_type(app->values_edit_inputbox, WIDGET_CONTENT_TYPE_HEXADECIMAL);
        widget_inputbox_set_overwrite_mode(app->values_edit_inputbox, TRUE);
        app->values_edit_inputbox->update_func = NULL;
        widget_set_enabled(app->values_edit_inputbox, false);
    }
	else
	{
		//app->values_edit_inputbox->frame = frame;
		MemoryViewer_SetupEditValueBox(app);
	}
}

static const std::vector<u32>* MemoryViewer_GetHighlightList(t_memory_viewer* app)
{
	const t_cheat_finder* cheat_finder = g_CheatFinder_MainInstance; 
	if (cheat_finder && cheat_finder->active && !cheat_finder->addresses_to_highlight_in_memory_editor.empty() && cheat_finder->memtype == app->pane_current->memrange.memtype)
		return &cheat_finder->addresses_to_highlight_in_memory_editor;
	return NULL;
}

static bool	MemoryViewer_IsAddressHighlighted(const std::vector<u32>* highlight_list, u32 addr)
{
	if (!highlight_list)
		return false;
	return std::find(highlight_list->begin(), highlight_list->end(), addr) != highlight_list->end();
}

static void        MemoryViewer_Update(t_memory_viewer* app)
{
    t_memory_pane *pane = app->pane_current;

    char            buf[9];
    const t_font_id	font_id = FONTID_MEDIUM;
    const int       font_height = Font_Height(font_id);
    const int       addr_length = pane->memrange.addr_hex_length;
    const int       addr_start  = pane->memrange.addr_start;

    // Skip update if not active
    if (!app->active)
        return;

	MemoryViewer_UpdateAllMemoryRanges(app);

    // If skin has changed, redraw everything
	ALLEGRO_BITMAP* box_gfx = app->box->gfx_buffer;
	al_set_target_bitmap(box_gfx);
    if (app->box->flags & GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT)
    {
        MemoryViewer_Layout(app, FALSE);
        app->box->flags &= ~GUI_BOX_FLAGS_DIRTY_REDRAW_ALL_LAYOUT;
    }
    else
    {
        // Clear anyway
        al_draw_filled_rectangle(0, 0, app->frame_view.size.x - 1, app->frame_view.size.y, COLOR_SKIN_WINDOW_BACKGROUND);
    }

    // Update inputs
    MemoryViewer_UpdateInputs(app);

    int y = 4;
    int addr = app->memblock_first * app->size_columns;
    
	//const u8* memory = mv->pane_current->memrange.data;

    // Lines to separate address/hex/ascii
    // FIXME-SIZE
    {
        int x;
        x = 4 + font_height * 6 - 7 - 7;
        al_draw_vline(x, 0, app->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
        //x = 4 + font_height * 6 - 7 + (mv->size_columns * (font_height * 2 - 1) + font_height - 3) + font_height - 3 - 4;
        x = app->frame_ascii.pos.x - 4;//MEMVIEW_COLUMNS_8_PADDING/2;
        al_draw_vline(x, 0, app->frame_view.size.y - 1, COLOR_SKIN_WINDOW_SEPARATORS);
    }

    // Print current address
    // FIXME: Could create a label widget for this purpose.
	const int addr_offset = (app->memblock_first * app->size_columns) + app->values_edit_position;
	const int addr_abs = addr_start + addr_offset;
    sprintf(buf, "%0*X", addr_length, addr_abs);
    al_draw_filled_rectangle(56, app->frame_view.size.y + 1 + 4, 91+1, app->frame_view.size.y + 1 + 4 + Font_Height(font_id) + 1, COLOR_SKIN_WINDOW_BACKGROUND);
    Font_Print(font_id, buf, 56, app->frame_view.size.y + 1 + 4, COLOR_SKIN_WINDOW_TEXT);

    y = 4;
    if (app->pane_current->memrange.size == 0)
    {
        const char *text = "None";
        if (app->pane_current->memrange.memtype == MEMTYPE_Z80 && g_driver->cpu != CPU_Z80)
            text = "You wish!";
		int x = 4 + font_height * 6 - 7;
        Font_Print(font_id, text, x, y, COLOR_SKIN_WINDOW_TEXT);

		app->values_edit_active = false;
		MemoryViewer_SetupEditValueBox(app);
    }
    else 
    {
		// Highlight request
		const std::vector<u32>* highlight_list = MemoryViewer_GetHighlightList(app);
		widget_set_highlight(app->values_edit_inputbox, MemoryViewer_IsAddressHighlighted(highlight_list, addr_offset));

		const int y_size = font_height;
		const int x_hex_size = font_height * (2) - 1;
		const int x_asc_size = font_height - 2;

        // Display all memory content lines
        for (int row = 0; row != app->size_lines; row++, y += y_size)
        {
            if (app->memblock_first + row >= app->memblocks_max)
                continue;

            // Print address on every new line
            sprintf(buf, "%0*X", addr_length, addr + addr_start);
            Font_Print(font_id, buf, 4 + (5 - addr_length) * (font_height - 2), y, COLOR_SKIN_WINDOW_TEXT);

            // Print 16-bytes in both hexadecimal and ASCII
            int x_hex = app->frame_hex.pos.x;
            int x_asc = app->frame_ascii.pos.x;

			for (int col = 0; col != app->size_columns; col++, x_hex += x_hex_size, x_asc += x_asc_size)
            {
                // Space each 8 columns (for readability)
                if (col != 0 && ((col & 7) == 0))
                    x_hex += MEMVIEW_COLUMNS_8_PADDING;

				// Highlight?
				if (MemoryViewer_IsAddressHighlighted(highlight_list, addr))
					al_draw_filled_rectangle(x_hex-2,y-1,x_hex+x_hex_size-2,y+y_size, COLOR_SKIN_WIDGET_GENERIC_SELECTION);

				// Get value
				const u8 v = pane->memrange.ReadByte(addr);

                // Print hexadecimal
                ALLEGRO_COLOR color = COLOR_SKIN_WINDOW_TEXT;
                sprintf(buf, "%02X", v);
                Font_Print(font_id, buf, x_hex, y, color);

                // Print ASCII
                if (app->values_edit_active && (app->values_edit_position == col + (row * app->size_columns)))
                    color = COLOR_SKIN_WINDOW_TEXT_HIGHLIGHT;
                buf[0] = isprint(v) ? v : '.';
                buf[1] = 0;
                Font_Print(font_id, buf, x_asc, y, color);

                addr++;
                if (addr >= (int)pane->memrange.size)
                    break;
            }
        }
    }

    // Refresh current value if edition cursor is at the beginning (no editing done yet)
    if (app->values_edit_active)
    {
        if (widget_inputbox_get_cursor_pos(app->values_edit_inputbox) == 0)
        {
            const int addr = (app->memblock_first * app->size_columns) + app->values_edit_position;
			const u8 v = pane->memrange.ReadByte(addr);
            sprintf(buf, "%02X", v);
            widget_inputbox_set_value(app->values_edit_inputbox, buf);
            widget_inputbox_set_cursor_pos(app->values_edit_inputbox, 0);
        }
    }
}

static void MemoryViewer_Switch(t_memory_viewer* mv)
{
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

static void MemoryViewer_Switch(t_widget* w)
{
    t_memory_viewer* app = (t_memory_viewer *)w->box->user_data; // Get instance
	MemoryViewer_Switch(app);
}

void    MemoryViewer_SwitchMainInstance()
{
    t_memory_viewer* app = MemoryViewer_MainInstance;
    if (app->active ^= 1)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_MemoryEditor_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_MemoryEditor_Disabled));
    gui_box_show(app->box, app->active, TRUE);
    gui_menu_toggle_check(menus_ID.tools, 4);
}

static void MemoryViewer_ViewPane(t_memory_viewer* app, t_memory_type memtype)
{
	t_memory_pane* pane = &app->panes[memtype];
    if (app->pane_current == pane)
        return;

    // Save bookmark
    app->pane_current->memblock_first = app->memblock_first;

    // Update interface button
    widget_set_highlight(app->pane_current->button, FALSE);
    widget_set_highlight(pane->button, TRUE);

    // Switch section
    app->pane_current    = pane;
    app->memblock_first	= pane->memblock_first;
    app->memblocks_max   = (pane->memrange.size + app->size_columns - 1) / app->size_columns;
    assert(pane->memrange.size >= 0);
    //if (mv->memblocks_max < mv->size_lines)
    //    mv->memblocks_max = mv->size_lines;
    MemoryViewer_SetupEditValueBox(app);
}

static void MemoryViewer_ViewPaneCallback(t_widget* w)
{
    t_memory_viewer* app = (t_memory_viewer *)w->box->user_data;
	t_memory_type memtype = (t_memory_type)(intptr_t)w->user_data;
    MemoryViewer_ViewPane(app, memtype);
}

static void MemoryViewer_UpdateAllMemoryRanges(t_memory_viewer* app)
{
	for (int i = 0; i != MEMTYPE_MAX_; i++)
	{
		t_memory_pane* pane = &app->panes[i];
		MemoryRange_GetDetails((t_memory_type)i, &pane->memrange);

		const int memblocks_max = (pane->memrange.size + app->size_columns - 1) / app->size_columns;
		if (pane->memblock_first + app->size_lines > memblocks_max)
			pane->memblock_first = MAX(memblocks_max - app->size_lines, 0);

		if (app->pane_current == pane)
			app->memblocks_max = memblocks_max;
	}
}

static void MemoryViewer_MediaReload(t_memory_viewer* app)
{
	MemoryViewer_UpdateAllMemoryRanges(app);
    MemoryViewer_SetupEditValueBox(app);
}

void	MemoryViewer_GotoAddress(t_memory_viewer* app, t_memory_type memtype, u32 offset)
{
	if (!app->active)
		MemoryViewer_Switch(app);

	// Switch pane if requested
	if (memtype != MEMTYPE_UNKNOWN)
		MemoryViewer_ViewPane(app, memtype);

	// Check boundaries
	t_memory_pane* pane = app->pane_current;
	const t_memory_range* memrange = &pane->memrange;
	if (offset < 0 || offset >= (u32)memrange->size)
	{
		char buf[16];
		sprintf(buf, "%0*X", memrange->addr_hex_length, memrange->addr_start+offset);
		Msg(MSGT_USER, Msg_Get(MSG_MemoryEditor_Address_Out_of_Bound), buf, memrange->name);
		return;
	}

	// Jump to given address
	app->memblock_first = (offset / app->size_columns) - (app->size_lines/2);
	if (app->memblock_first < 0)
		app->memblock_first = 0;
	if (app->memblock_first + app->size_lines > app->memblocks_max)
		app->memblock_first = MAX(app->memblocks_max - app->size_lines, 0);
	app->values_edit_active = TRUE;
	app->values_edit_position = offset - (app->memblock_first * app->size_columns);
	MemoryViewer_SetupEditValueBox(app);

	// Clear address box
	widget_inputbox_set_value(app->address_edit_inputbox, "");
}

//-----------------------------------------------------------------------------
// Enter callback handler on 'address' input box.
// Set current cursor position to given address (if valid)
//-----------------------------------------------------------------------------
void      MemoryViewer_InputBoxAddress_EnterCallback(t_widget* w)
{
    t_memory_viewer* app = (t_memory_viewer *)w->box->user_data; // Get instance

    // Read address
    const char* text = widget_inputbox_get_value(app->address_edit_inputbox);
	int addr = 0;
    sscanf(text, "%X", &addr);

	const int offset = addr - app->pane_current->memrange.addr_start;
	MemoryViewer_GotoAddress(app, app->pane_current->memrange.memtype, offset);
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
    // Msg(MSGT_DEBUG, "Edit, cursor at %d", cursor);
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

// Enter callback handler of moving 'value' input box.
// Write input value at current memory location.
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
		Msg(MSGT_USER, "%s", Msg_Get(MSG_MemoryEditor_WriteZ80_Unable));
	}
    mv->values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void		MemoryViewer_ClickBottom(t_widget* w)
{
    t_memory_viewer* app = (t_memory_viewer *)w->box->user_data; // Get instance

    app->values_edit_active = FALSE;
    MemoryViewer_SetupEditValueBox(app);
}

static void		MemoryViewer_ClickMemoryHex(t_widget* w)
{
    t_memory_viewer* app = (t_memory_viewer *)w->box->user_data; // Get instance

    // Msg(MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);

    // Clicking in empty columns disable edition
    for (int i = 0; i < (app->size_columns - 1) / 8; i++)
    {
        const int max_x = (i + 1) * (8 * (Font_Height(FONTID_MEDIUM) * (2) - 1) + MEMVIEW_COLUMNS_8_PADDING);
        const int min_x = max_x - MEMVIEW_COLUMNS_8_PADDING;
        if (w->mouse_x >= min_x && w->mouse_x < max_x)
        {
            // Hide edit value input box
            app->values_edit_active = FALSE;
            MemoryViewer_SetupEditValueBox(app);
            return;
        }
    }

    // Inside
    // FIXME-SIZE
	int x, y;
    x = w->mouse_x / (Font_Height(FONTID_MEDIUM) * (2) - 1);
    x = (w->mouse_x - (x / 8) * MEMVIEW_COLUMNS_8_PADDING) / (Font_Height(FONTID_MEDIUM) * (2) - 1);
    y = (w->mouse_y / Font_Height(FONTID_MEDIUM));

    app->values_edit_position = x + y * app->size_columns;
    app->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox(app);
}

static void        MemoryViewer_ClickMemoryAscii(t_widget *w)
{
    t_memory_viewer *mv = (t_memory_viewer *)w->box->user_data; // Get instance

    int x = w->mouse_x / (Font_Height(FONTID_MEDIUM) - 2);
    int y = w->mouse_y / Font_Height(FONTID_MEDIUM);
    // Msg(MSGT_DEBUG, "click w->mx = %d, w->my = %d (frame %d x %d)\n", w->mx, w->my, w->frame.size.x, w->frame.size.y);
    // Msg(MSGT_DEBUG, "x = %d, y = %d\n", x, y);

    mv->values_edit_position = x + y * mv->size_columns;
    mv->values_edit_active = TRUE;
    MemoryViewer_SetupEditValueBox(mv);
}

static void    MemoryViewer_SetupEditValueBox(t_memory_viewer* app)
{
    // Disable if out of boundaries
    const int addr = app->memblock_first * app->size_columns + app->values_edit_position;
    if (addr < 0 || addr >= app->pane_current->memrange.size)
    {
        app->values_edit_active = FALSE;
        app->values_edit_position = 0;
    }

    if (app->values_edit_active)
    {
        // Position input box
        {
            t_frame *frame = &app->values_edit_inputbox->frame;
            const int pos = app->values_edit_position;
            frame->pos.x = (pos % app->size_columns) * (Font_Height(FONTID_MEDIUM) * 2 - 1);
            // Spacing every 8 bytes
            frame->pos.x += MEMVIEW_COLUMNS_8_PADDING * ((pos % app->size_columns) / 8);
            frame->pos.y = (pos / app->size_columns) * Font_Height(FONTID_MEDIUM);
            frame->pos.x += app->values_hex_box->frame.pos.x - 5; // Coordinates are parent relative
            frame->pos.y += app->values_hex_box->frame.pos.y - 1;
        }

        // Show input box if not already active
        if (app->values_edit_inputbox->update_func == NULL)
        {
            app->values_edit_inputbox->update_func = widget_inputbox_update;
            widget_set_enabled(app->values_edit_inputbox, true);
            app->address_edit_inputbox->update_func = NULL;
        }

        // Setup input box default content
        u8 value = app->pane_current->memrange.ReadByte(addr);
        char buf[3];
        sprintf(buf, "%02X", value);
        widget_inputbox_set_value(app->values_edit_inputbox, buf);
        widget_inputbox_set_cursor_pos(app->values_edit_inputbox, 0);
    }
    else
    {
        // Hide input box if active
        if (app->values_edit_inputbox->update_func != NULL)
        {
            app->address_edit_inputbox->update_func = widget_inputbox_update;
            app->values_edit_inputbox->update_func = NULL;
            widget_set_enabled(app->values_edit_inputbox, false);
        }
    }
}

// Poll and update user inputs.
// FIXME: This function is probably unnecessary big.
static void     MemoryViewer_UpdateInputs(t_memory_viewer *mv)
{
    // Check for focus
    if (!gui_box_has_focus(mv->box))
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
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGUP, FALSE, 30, 3) || gui.mouse.wheel_rel > 0)
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
    else if (Inputs_KeyPressed_Repeat(ALLEGRO_KEY_PGDN, FALSE, 30, 3) || gui.mouse.wheel_rel < 0)
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

void      MemoryViewers_Update()
{
    for (t_list* mvs = MemoryViewers; mvs != NULL; mvs = mvs->next)
    {
        t_memory_viewer *mv = (t_memory_viewer *)mvs->elem;
        MemoryViewer_Update(mv);
    }
}

void      MemoryViewers_MediaReload()
{
    for (t_list* mvs = MemoryViewers; mvs != NULL; mvs = mvs->next)
    {
        t_memory_viewer *mv = (t_memory_viewer *)mvs->elem;
        MemoryViewer_MediaReload(mv);
    }
}

//-----------------------------------------------------------------------------
