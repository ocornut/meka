//-----------------------------------------------------------------------------
// MEKA - app_memview.h
// Memory Viewer - Headers
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

enum t_memory_type
{
	MEMTYPE_UNKNOWN	= -1,
	MEMTYPE_Z80		= 0,
	MEMTYPE_ROM		= 1,
	MEMTYPE_RAM		= 2,
	MEMTYPE_VRAM	= 3,
	MEMTYPE_PRAM	= 4,
	MEMTYPE_SRAM	= 5,
	MEMTYPE_VREG	= 6,
	MEMTYPE_MAX_	= 7,
};

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_memory_range
{
	t_memory_type		memtype;
	const char *		name;
	int					size;
	int					addr_start;
	int					addr_hex_length;
	u8 *				data;

	u8					ReadByte(int addr) const;
	bool				WriteByte(int addr, u8 v);
};

struct t_memory_pane
{
	t_memory_range		memrange;
    int                 memblock_first;
    t_widget *          button;
};

struct t_memory_viewer
{
    // Logic
    int                 size_columns;
    int                 size_lines;
    int                 memblocks_max;
    int                 memblock_first;
    t_memory_pane		panes[MEMTYPE_MAX_];
    t_memory_pane *		pane_current;

    // Interface
    bool                active;
    t_gui_box *         box;
    t_widget *          widget_scrollbar;

    // Interface - Top (values)
    t_frame             frame_view;
    t_frame             frame_hex;
    t_frame             frame_ascii;
    t_widget *          values_hex_box;
    t_widget *			values_ascii_box;
    bool                values_edit_active;
    int                 values_edit_position;
    t_widget *          values_edit_inputbox;

    // Interface - Bottom (control)
	t_widget *          bottom_box;
    t_widget *          address_edit_inputbox;
};

extern t_memory_viewer *MemoryViewer_MainInstance;
extern t_list *         MemoryViewers;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_memory_viewer *       MemoryViewer_New(bool register_desktop, int size_columns, int size_lines);
void                    MemoryViewer_Delete(t_memory_viewer* app);
void					MemoryViewer_GotoAddress(t_memory_viewer* app, t_memory_type memtype, u32 offset);
void                    MemoryViewer_SwitchMainInstance(void);

void                    MemoryViewers_Update(void);
void                    MemoryViewers_MediaReload(void);

void					MemoryRange_GetDetails(t_memory_type memtype, t_memory_range* out);

//-----------------------------------------------------------------------------
