//-----------------------------------------------------------------------------
// MEKA - capture.c
// Screen Capture - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "capture.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Capture_Init ()
// Initialize screen capture system
// Needs to be called before loading the configuration file
//-----------------------------------------------------------------------------
void        Capture_Init (void)
{
    Capture_Init_Game ();
    #ifdef DOS
        Capture.filename_template = "%.5s-%02d.png"; /* Short File Name */
    #else
        Capture.filename_template = "%s-%02d.png";   /* Long File Name (SMS Power) */
    #endif
}

//-----------------------------------------------------------------------------
// Capture_Init_Game ()
// Screen capture per-game initialization
// Called each time a new Game/ROM is loaded
//-----------------------------------------------------------------------------
void        Capture_Init_Game (void)
{
    Capture.request = NO;
    Capture.id_number = 1;
}

//-----------------------------------------------------------------------------
// Capture_Request ()
// Request a screen capture (will be actually done on next rendering)
//-----------------------------------------------------------------------------
void        Capture_Request (void)
{
    Capture.request = YES;
}

//-----------------------------------------------------------------------------
// Capture_FileName_Get (char *dst)
// Compute filename for next screen capture
//-----------------------------------------------------------------------------
void        Capture_FileName_Get (char *dst)
{
    char *  game_name;
    char    s1 [FILENAME_LEN];
    char    s2 [FILENAME_LEN];

    // Create directory if necessary
    if (!file_exists (file.dir_captures, 0xFF, NULL))
    {
        meka_mkdir (file.dir_captures);
        // chmod (file.dir_captures, 1, 0);
    }

    // Create second template
    if ((machine & MACHINE_RUN) == MACHINE_RUN) // If a game is loaded & running
    {
        strcpy (s1, file.rom);
        killpath (s1);
        killext (s1);
        game_name = s1;
    }
    else
    {
        game_name = CAPTURE_DEFAULT_PREFIX;
    }
    sprintf (s2, "%%s/%s", Capture.filename_template);

    // Create a filename and check if the file already exists. Loop if it is the case.
    do
    {
        sprintf (dst, s2, file.dir_captures, game_name, Capture.id_number);
        Capture.id_number ++;
    }
    while (file_exists (dst, 0xFF, NULL) != 0 && Capture.id_number < CAPTURE_ID_MAX);
}

//-----------------------------------------------------------------------------
// Capture_Screen (void)
// Capture current screen to a file
//-----------------------------------------------------------------------------
void            Capture_Screen (void)
{
    PALETTE     pal;
    BITMAP *    bmp;
    BITMAP *    source;
    char        s1 [FILENAME_LEN];
    int         x_start, x_len;
    int         y_start, y_len;

    Capture.request = NO;

    // Get a filename
    Capture_FileName_Get (s1);
    if (Capture.id_number >= CAPTURE_ID_MAX)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Capture_Error_File));
        return;
    }

    switch (Meka_State)
    {
    case MEKA_STATE_FULLSCREEN: // Fullscreen
        x_start = cur_drv->x_start;
        y_start = cur_drv->y_show_start;
        x_len = cur_drv->x_res;
        y_len = cur_drv->y_res;
        // Crop left column
        if ((cur_drv->id == DRV_SMS) && (tsms.VDP_VideoMode > 4) && (Mask_Left_8))
        {
            x_start += 8;
            x_len -= 8;
        }
        source = screenbuffer;
        break;

    case MEKA_STATE_GUI: // GUI mode
        x_start = 0;
        y_start = 0;
        x_len = cfg.GUI_Res_X;
        y_len = cfg.GUI_Res_Y;
        source = gui_buffer;
        break;

    default: // Unknown Mode
        return;
    }

    acquire_bitmap(source);
    bmp = create_sub_bitmap (source, x_start, y_start, x_len, y_len);
    if (bmp == NULL)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Capture_Error));
        return;
    }
    release_bitmap(source);

    get_palette (pal);
    if (save_bitmap (s1, bmp, pal) != 0)
    {
        Msg (MSGT_USER, Msg_Get (MSG_Capture_Error));
        destroy_bitmap (bmp);
        return;
    }

    destroy_bitmap (bmp);

    // Verbose
    killpath (s1);
    Msg (MSGT_USER, Msg_Get (MSG_Capture_Done), s1);
}

//-----------------------------------------------------------------------------

