//-----------------------------------------------------------------------------
// MEKA - g_box.h
// GUI Boxes - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_gui_box *		gui_box_new(const t_frame* frame, const char* title);
void			gui_box_destroy_widgets(t_gui_box* box);
void            gui_box_delete(t_gui_box* box);
void			gui_box_create_video_buffer(t_gui_box* box);
void			gui_box_destroy_video_buffer(t_gui_box* box);
void			gui_box_resize(t_gui_box* box, int size_x, int size_y, bool interactive);

int				gui_box_find_z(const t_gui_box* box);

//-----------------------------------------------------------------------------
// Functions / Make obsolete
//-----------------------------------------------------------------------------

void    gui_update_boxes        ();

void    gui_box_show            (t_gui_box *box, bool enable, bool focus);
void    gui_box_set_focus       (t_gui_box *box);
int     gui_box_has_focus       (t_gui_box *box);
void    gui_box_set_title       (t_gui_box *box, const char *title);

void    gui_box_clip_position   (t_gui_box *box);

//-----------------------------------------------------------------------------

