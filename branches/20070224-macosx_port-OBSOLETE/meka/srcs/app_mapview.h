//-----------------------------------------------------------------------------
// MEKA - app_mapview.h
// TileMap Viewer - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    bool                    active;
    t_gui_box *             box;
    t_frame                 frame_box;
    t_frame                 frame_tilemap;
    t_frame                 frame_infos;
    t_frame                 frame_config;
    t_frame                 frame_tilemap_addr;
    t_widget *              frame_tilemap_zone;

    t_widget *              widget_tilemap_addr_scrollbar;
    int                     widget_tilemap_addr_scrollbar_max;
    int                     widget_tilemap_addr_scrollbar_cur;
    int                     widget_tilemap_addr_scrollbar_per_page;
    t_widget *              widget_tilemap_addr_checkbox;

    bool                    config_bg;
    bool                    config_fg;
    bool                    config_hflip;
    bool                    config_vflip;
    bool                    config_scroll;
    bool                    config_scroll_raster;
    int                     config_tilemap_addr;
    bool                    config_tilemap_addr_auto;
    int                     tile_hovered;
    int                     tile_selected;

} t_tilemap_viewer;

extern t_tilemap_viewer *	TilemapViewer_MainInstance;
extern t_list *				TilemapViewers;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_tilemap_viewer *          TilemapViewer_New(bool register_desktop);
void                        TilemapViewer_Delete(t_tilemap_viewer *app);
void                        TilemapViewer_SwitchMainInstance(void);

void                        TilemapViewers_Update(void);

//-----------------------------------------------------------------------------
