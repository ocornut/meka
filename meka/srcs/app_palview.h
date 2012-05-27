//-----------------------------------------------------------------------------
// MEKA - app_palview.h
// Palette Viewer - Interface
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    // Interface
    bool                active;
    bool                dirty;
    t_gui_box *         box;
    BITMAP *            box_gfx;
    t_frame             frame_palette;
    t_frame             frame_info;
    t_widget *          frame_palette_zone;

    // Logic
    int                 palette_size;
    int                 color_displayed; // -1 if none, else 0 to palette_size-1
    int                 color_hovered;   // -1 if none, else 0 to palette_size-1
    int                 color_selected;  // -1 if none, else 0 to palette_size-1

} t_app_palette_viewer;

t_app_palette_viewer    PaletteViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    PaletteViewer_Init              (t_app_palette_viewer *pv);
void    PaletteViewer_Switch            (void);
void    PaletteViewer_Update            (void);
void    PaletteViewer_SetPaletteSize    (t_app_palette_viewer *pv, int palette_size);

//-----------------------------------------------------------------------------
