//-----------------------------------------------------------------------------
// MEKA - config_v.h
// Configuration File: Video Drivers - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

typedef struct
{
    char *      desc;
    int         drv_id;
    int         drv_id_switch_fs_win;       // Note: this is a helper for usage by ALT-ENTER. Eventually we'll have a better way to switch mode.
    char *      comment;
} t_video_driver;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

t_video_driver *    VideoDriver_FindByDesc(char *s);
t_video_driver *    VideoDriver_FindByDriverId(int drv_id);
void                VideoDriver_DumpAllDesc(FILE *f);

//-----------------------------------------------------------------------------
