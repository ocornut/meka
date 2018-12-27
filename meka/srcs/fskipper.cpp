//-----------------------------------------------------------------------------
// MEKA - fskipper.c
// Frame Skipper - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "fskipper.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_fskipper				fskipper;
ALLEGRO_TIMER*			g_timer_throttle = NULL;
ALLEGRO_EVENT_QUEUE*	g_timer_throttle_event_queue = NULL;
ALLEGRO_TIMER*			g_timer_seconds = NULL;
ALLEGRO_EVENT_QUEUE*	g_timer_seconds_event_queue = NULL;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Frame_Skipper_Init_Values (void)
{
    fskipper.Mode                       = FRAMESKIP_MODE_THROTTLED;
    fskipper.Throttled_Speed            = 60;
    fskipper.Throttled_Frame_Elapsed    = 0;
    fskipper.Unthrottled_Frameskip      = 1;
    fskipper.Unthrottled_Counter        = 0;
    fskipper.Show_Current_Frame         = TRUE;
    fskipper.FPS                        = 0.0f;
    fskipper.FPS_Display                = FALSE;
	fskipper.FPS_SecondsElapsed			= 0;
	fskipper.FPS_FrameCountAccumulator	= 0;
}

void    Frame_Skipper_Init()
{
    // Throttle
	g_timer_throttle = al_create_timer(1.0f/(float)fskipper.Throttled_Speed);
	g_timer_throttle_event_queue = al_create_event_queue();
	al_register_event_source(g_timer_throttle_event_queue, al_get_timer_event_source(g_timer_throttle));
	al_start_timer(g_timer_throttle);

    // FPS Counter
	g_timer_seconds = al_create_timer(1.0f);
	g_timer_seconds_event_queue = al_create_event_queue();
	al_register_event_source(g_timer_seconds_event_queue, al_get_timer_event_source(g_timer_seconds));
	al_start_timer(g_timer_seconds);
}

bool Frame_Skipper()
{
    if (fskipper.Mode == FRAMESKIP_MODE_THROTTLED)
    {
		// Auto frame-skipping
		ALLEGRO_EVENT timer_event;
		while (al_get_next_event(g_timer_throttle_event_queue, &timer_event))
			if (timer_event.type == ALLEGRO_EVENT_TIMER)
				fskipper.Throttled_Frame_Elapsed++;

		while (fskipper.Throttled_Frame_Elapsed == 0)
		{
			// Blocking
			al_wait_for_event(g_timer_throttle_event_queue, &timer_event);
			if (timer_event.type == ALLEGRO_EVENT_TIMER)
				fskipper.Throttled_Frame_Elapsed++;
		}

        // If retard is too high, force drawing a frame so it doesn't freeze
        // It is also good since huge delay (+1/4th second) will not be catched
        if (fskipper.Throttled_Frame_Elapsed > 15)
        { 
            fskipper.Throttled_Frame_Elapsed = 1;
        }

        // Skip next frame if we have more than one to go (we're late)
        // Else don't skip
        if (fskipper.Throttled_Frame_Elapsed -- > 1)
            return FALSE;
    }
    else
    {
        // Skip Standard_Counter-1 frames every Standard_Counter frames
        if (fskipper.Unthrottled_Counter < fskipper.Unthrottled_Frameskip)
        {
            fskipper.Unthrottled_Counter ++;
            return FALSE;
        }
        fskipper.Unthrottled_Counter = 1;
    }

	// Poll FPS timer 
	ALLEGRO_EVENT timer_event;
	while (al_get_next_event(g_timer_seconds_event_queue, &timer_event))
	{
		switch (timer_event.type)
		{
		case ALLEGRO_EVENT_TIMER:
			if (timer_event.timer.source == g_timer_seconds)
			{
				fskipper.FPS_SecondsElapsed ++;
				break;
			}
			break;
		}
	}

    // Compute FPS if a new second has elapsed
    fskipper.FPS_FrameCountAccumulator++;
	if (fskipper.FPS_SecondsElapsed > 0)
    {
        //int elapsed = (int)(cycle_current - fskipper.FPS_LastComputedTime);
        //int fps = (fskipper.Frame_Rendered * cycle_per_second + (cycle_per_second / 2)) / elapsed;
        //fskipper.FPS = fps;
        //Msg(MSGT_DEBUG, "Frame_Rendered = %d, FPS = %d", fskipper.Frame_Rendered, fskipper.FPS);
        fskipper.FPS = (float)fskipper.FPS_FrameCountAccumulator / (float)fskipper.FPS_SecondsElapsed;
        fskipper.FPS_SecondsElapsed = 0;
        fskipper.FPS_FrameCountAccumulator = 0;
    }

	// Will show next frame
    return TRUE;
}

void Frame_Skipper_Switch()
{
    if (fskipper.Mode == FRAMESKIP_MODE_THROTTLED)
    {
        fskipper.Mode = FRAMESKIP_MODE_UNTHROTTLED;
        fskipper.Unthrottled_Counter = 1;
    }
    else
    {
        fskipper.Mode = FRAMESKIP_MODE_THROTTLED;
        fskipper.Throttled_Frame_Elapsed = 0;
    }
    Frame_Skipper_Show();
}

void Frame_Skipper_Configure (int v)
{
    switch (fskipper.Mode)
    {
    case FRAMESKIP_MODE_THROTTLED:
        {
            fskipper.Throttled_Speed += (v * 10);
			fskipper.Throttled_Speed = Clamp(fskipper.Throttled_Speed, 10, 400);			// Range 10->400 Hz
			al_set_timer_speed(g_timer_throttle, 1.0f/(float)fskipper.Throttled_Speed);
			Sound_UpdateClockSpeed();
            break;
        }
    case FRAMESKIP_MODE_UNTHROTTLED:
        {
            fskipper.Unthrottled_Frameskip += v;
			fskipper.Unthrottled_Frameskip = Clamp(fskipper.Unthrottled_Frameskip, 1, 9);	// Range 1/1 to 1/9
            break;
        }
    }
    Frame_Skipper_Show();
}

void    Frame_Skipper_Show()
{
    if (fskipper.Mode == FRAMESKIP_MODE_THROTTLED)
        Msg(MSGT_USER, Msg_Get(MSG_Frameskip_Auto), fskipper.Throttled_Speed);
    else
        Msg(MSGT_USER, Msg_Get(MSG_Frameskip_Standard), fskipper.Unthrottled_Frameskip);
}

void    Frame_Skipper_Switch_FPS_Counter()
{
    fskipper.FPS_Display = !fskipper.FPS_Display;
    if (fskipper.FPS_Display)
        Msg(MSGT_USER, "%s", Msg_Get(MSG_FPS_Counter_Enabled));
    else
        Msg(MSGT_USER, "%s", Msg_Get(MSG_FPS_Counter_Disabled));
}

//-----------------------------------------------------------------------------

