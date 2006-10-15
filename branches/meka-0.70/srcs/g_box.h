//-----------------------------------------------------------------------------
// MEKA - g_box.h
// GUI Boxes - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Box Attributes
#define A_Resizable     (0x01)
#define A_Killed        (0x02)
#define A_Show          (0x04)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
// FIXME: all function using integer id should be obsoleted
//-----------------------------------------------------------------------------

int     gui_box_create          (int pos_x, int pos_y, int size_x, int size_y, char label[]);
void    gui_box_delete          (int box_n);

void    gui_set_image_box       (int which, BITMAP *bitmap);
void    gui_init_default_box    (void);
void    gui_update_boxes        (void);

void    gui_box_show            (t_gui_box *box, bool enable, bool focus);
int     gui_box_find_plan       (t_gui_box *box);                           // replace gui_box_find()
void    gui_box_set_focus       (t_gui_box *box);
int     gui_box_has_focus       (t_gui_box *box);
void    gui_box_set_title       (t_gui_box *box, char *title);

void    gui_box_clip_position   (t_gui_box *box);

//-----------------------------------------------------------------------------

