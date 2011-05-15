//-----------------------------------------------------------------------------
// MEKA - config_v.h
// Configuration File: Video Drivers - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

#if 0	// FIXME-ALLEGRO5: no video driver
struct t_video_driver
{
    char *      desc;
    int         drv_id;
    int         drv_id_switch_fs_win;       // Note: this is a helper for usage by ALT-ENTER. Eventually we'll have a better way to switch mode.
    char *      comment;
};
#endif

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

#if 0 	// FIXME-ALLEGRO5: no video driver
t_video_driver *    VideoDriver_FindByDesc(char *s);
t_video_driver *    VideoDriver_FindByDriverId(int drv_id);
void                VideoDriver_DumpAllDesc(FILE *f);
#endif

//-----------------------------------------------------------------------------
