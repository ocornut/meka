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

enum t_tilemap_viewer_layout
{
	TILEMAP_VIEWER_LAYOUT_SMSGG,
	TILEMAP_VIEWER_LAYOUT_SMSGG_EXTRAHEIGHT,
	TILEMAP_VIEWER_LAYOUT_SGSC,
};

struct t_tilemap_viewer
{
    bool                    active;
    t_gui_box *             box;
	t_tilemap_viewer_layout	layout;
    t_frame                 frame_box;
    t_frame                 frame_tilemap;
    t_frame                 frame_infos;
    t_frame                 frame_config;
    t_frame                 frame_tilemap_addr;
    t_widget *              frame_tilemap_zone;

    t_widget *              widget_tilemap_addr_scrollbar;
    int                     widget_tilemap_addr_scrollbar_slot_cur;
    t_widget *              widget_tilemap_addr_auto_checkbox;

    bool                    config_bg;
    bool                    config_fg;
    bool                    config_hflip;
    bool                    config_vflip;
    bool                    config_scroll;
    bool                    config_scroll_raster;
    int                     config_tilemap_addr;
    bool                    config_tilemap_addr_auto;
	int						config_tilemap_addr_manual_base_addr;		// SMS: $0000  Wide: $0700  SG/SC: $0000
	int						config_tilemap_addr_manual_step_size;		// SMS: $0800  Wide: $1000  SG/SC: $0400  
	int						config_tilemap_addr_manual_step_count;		// SMS: 8      Wide: 4      SG/SC: 16
    int                     tile_hovered;
    int                     tile_selected;
};

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
