//-----------------------------------------------------------------------------
// MEKA - tileview.h
// Tile Viewer - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_app_tile_viewer
{
    bool        active;
    bool        dirty;
    t_gui_box * box;
    int         palette;        // 0 to palette_max-1
    int         palette_max;    // 1+
    int         tile_displayed; // -1 if none, else 0 to tiles_max-1
    int         tile_hovered;   // -1 if none, else 0 to tiles_max-1
    int         tile_selected;  // -1 if none, else 0 to tiles_max-1
    int         tiles_count;    // 448 or 512
    int         tiles_width;
    int         tiles_height;
    t_frame     tiles_display_frame;
    t_widget *  tiles_display_zone;
};

extern t_app_tile_viewer   TileViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    TileViewer_Init_Values  (void);
void    TileViewer_Init         (void);

void    TileViewer_Update       (t_app_tile_viewer *app);

void    TileViewer_Switch       (void);
void    TileViewer_Configure_PaletteMax (int palette_max);

//-----------------------------------------------------------------------------

