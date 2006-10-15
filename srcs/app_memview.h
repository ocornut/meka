//-----------------------------------------------------------------------------
// MEKA - memview.h
// Memory Viewer - Headers
//-----------------------------------------------------------------------------

#ifndef _MEKA_MEMVIEW_H_
#define _MEKA_MEMVIEW_H_

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEMTYPE_Z80   (0)
#define MEMTYPE_ROM   (1)
#define MEMTYPE_RAM   (2)
#define MEMTYPE_VRAM  (3)
#define MEMTYPE_PRAM  (4)
#define MEMTYPE_SRAM  (5)
#define MEMTYPE_MAX   (6)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    int                 memtype;
    int                 memblock_first;
    int                 size;
    int                 addr_start;
    int                 addr_length;
    const char *        name;
    t_widget *          button;
} t_memory_section;

typedef struct
{
    // Logic
    int                 memblocks_max;
    int                 memblock_first;
    int                 memblock_lines_nbr;
    t_memory_section    sections[MEMTYPE_MAX];
    t_memory_section *  section_current;

    // Interface
    byte                active;
    t_gui_box *         box;
    BITMAP *            box_gfx;

    // Interface - Top (values)
    t_frame             frame_view;
    t_widget *          values_hex_box;
    t_widget *          values_ascii_box;
    bool                values_edit_active;
    int                 values_edit_position;
    t_widget *          values_edit_inputbox;

    // Interface - Bottom (control)
    t_widget *          bottom_box;
    t_widget *          address_edit_inputbox;

} t_memory_viewer;

t_memory_viewer MemoryViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void      MemoryViewer_Init              (void);
void      MemoryViewer_Switch            (void);
void      MemoryViewer_Update            (void);
void      MemoryViewer_Update_Inputs     (void);
void      MemoryViewer_LoadROM           (void);

//-----------------------------------------------------------------------------

#endif // _MEKA_MEMVIEW_H_
