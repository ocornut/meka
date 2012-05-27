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
    int         tiles_per_page;
    int         tiles_width;
    int         tiles_height;
    t_frame     tiles_display_frame;
	t_frame		tile_selected_frame;
    t_widget *  tiles_display_zone;

	t_widget *	vram_addr_tms9918_scrollbar;
	int			vram_addr_tms9918_current;
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

