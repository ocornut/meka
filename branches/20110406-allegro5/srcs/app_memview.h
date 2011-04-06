//-----------------------------------------------------------------------------
// MEKA - app_memview.h
// Memory Viewer - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define MEMTYPE_Z80   (0)
#define MEMTYPE_ROM   (1)
#define MEMTYPE_RAM   (2)
#define MEMTYPE_VRAM  (3)
#define MEMTYPE_PRAM  (4)
#define MEMTYPE_SRAM  (5)
#define MEMTYPE_VREG  (6)
#define MEMTYPE_MAX   (7)

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
    int                 size_columns;
    int                 size_lines;
    int                 memblocks_max;
    int                 memblock_first;
    t_memory_section    sections[MEMTYPE_MAX];
    t_memory_section *  section_current;

    // Interface
    bool                active;
    t_gui_box *         box;
    ALLEGRO_BITMAP *            box_gfx;
    t_widget *          widget_scrollbar;

    // Interface - Top (values)
    t_frame             frame_view;
    t_frame             frame_hex;
    t_frame             frame_ascii;
    t_widget *          values_hex_box;
    t_widget *          values_ascii_box;
    bool                values_edit_active;
    int                 values_edit_position;
    t_widget *          values_edit_inputbox;

    // Interface - Bottom (control)
    t_widget *          bottom_box;
    t_widget *          address_edit_inputbox;

} t_memory_viewer;

extern t_memory_viewer *MemoryViewer_MainInstance;
extern t_list *         MemoryViewers;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_memory_viewer *       MemoryViewer_New(bool register_desktop, int size_columns, int size_lines);
void                    MemoryViewer_Delete(t_memory_viewer *mv);
void                    MemoryViewer_SwitchMainInstance(void);

void                    MemoryViewers_Update(void);
void                    MemoryViewers_MediaReload(void);

//-----------------------------------------------------------------------------
