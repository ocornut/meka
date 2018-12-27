//
// Meka - sound_logging.c
// Sound Logging
//

#include "shared.h"
#include "sound_logging.h"
#include "wav.h"

void    Sound_Log_Init(void)
{
	Sound.LogWav                   = NULL;
	Sound.LogWav_SizeData          = 0;
	Sound.LogWav_FileName_Template = strdup("%s-%02d.wav");
	Sound.LogWav_ID                = 0;
	Sound.LogVGM.Logging           = VGM_LOGGING_NO;
	Sound.LogVGM_Logging_Accuracy  = VGM_LOGGING_ACCURACY_FRAME;
	Sound.LogVGM_FileName_Template = strdup("%s-%02d.vgm");
	Sound.LogVGM_ID                = 0;
}

void    Sound_Log_Close(void)
{
    if (Sound.LogWav != NULL)
        Sound_LogWAV_Stop();
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
        Sound_LogVGM_Stop();
}

void    Sound_Log_Init_Game(void)
{
    Sound.LogWav_ID = Sound.LogVGM_ID = 1; // Reset counter for a new game
}

void    Sound_Log_FileName_Get(char *result, const char *filename_template, int *id)
{
    char   s1 [FILENAME_LEN];
    char   s2 [FILENAME_LEN];

    // Create Directory if necessary --------------------------------------------
    if (!al_filename_exists(g_env.Paths.MusicDirectory))
        al_make_directory(g_env.Paths.MusicDirectory);

    // Create second template ----------------------------------------------------
    const char * game_name;
    if ((g_machine_flags & MACHINE_RUN) == MACHINE_RUN) // If a game is loaded & runnnig
    {
        strcpy(s1, g_env.Paths.MediaImageFile);
        StrPath_RemoveDirectory (s1);
        StrPath_RemoveExtension (s1);
        game_name = s1;
    }
    else
    {
        game_name = "meka";
    }
    sprintf(s2, "%%s/%s", filename_template);

    do
    {
        sprintf(result, s2, g_env.Paths.MusicDirectory, game_name, *id);
        (*id) ++;
    }
    while (al_filename_exists(result) != 0 && *id < SOUND_LOG_ID_MAX);
}

void    Sound_LogWAV_Start(void)
{
	if (Sound.LogWav != NULL)
		Sound_LogWAV_Stop();
	if (Sound.LogWav == NULL)
	{ // Start Logging
		char FileName[FILENAME_LEN];
		Sound_Log_FileName_Get (FileName, Sound.LogWav_FileName_Template, &Sound.LogWav_ID);
		if (Sound.LogWav_ID >= SOUND_LOG_ID_MAX)
		{
			Msg(MSGT_USER, "%s", Msg_Get(MSG_Sound_Dumping_Error_File_1));
			return;
		}
		Sound.LogWav = WAV_Start(FileName);
		StrPath_RemoveDirectory (FileName);
		if (Sound.LogWav == NULL)
		{
			Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Error_File_2), FileName);
			return;
		}
		else
		{
			Sound.LogWav_SizeData = 0;
			Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Start), FileName);
			gui_menu_active_range (TRUE, menus_ID.sound_log, 4, 4);
		}
	}
}

void    Sound_LogWAV_Stop(void)
{
	if (Sound.LogWav != NULL)
	{
		WAV_Close(Sound.LogWav, Sound.LogWav_SizeData);
		Sound.LogWav = NULL;
		Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Stop),
			(double)Sound.LogWav_SizeData / ((16 / 8) * 1 * Sound.SampleRate));
		gui_menu_active_range (FALSE, menus_ID.sound_log, 4, 4);
	}
}

void    Sound_LogVGM_Start(void)
{
	char   FileName[FILENAME_LEN];

	if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
		Sound_LogVGM_Stop();

	// Start Logging
	Sound_Log_FileName_Get(FileName, Sound.LogVGM_FileName_Template, &Sound.LogVGM_ID);
	if (Sound.LogVGM_ID >= SOUND_LOG_ID_MAX)
	{
		Msg(MSGT_USER, "%s", Msg_Get(MSG_Sound_Dumping_Error_File_1));
		return;
	}
	if (VGM_Start(&Sound.LogVGM, FileName, Sound.LogVGM_Logging_Accuracy) != MEKA_ERR_OK)
	{
		StrPath_RemoveDirectory (FileName);
		Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Error_File_2), FileName);
		return;
	}
	else
	{
		StrPath_RemoveDirectory (FileName);
		Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Start), FileName);
		gui_menu_active_range (TRUE, menus_ID.sound_log, 1, 1);
	}
}

void    Sound_LogVGM_Stop(void)
{
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
    {
        VGM_Close(&Sound.LogVGM);
        Msg(MSGT_USER, Msg_Get(MSG_Sound_Dumping_Stop),
            (double)Sound.LogVGM.vgm_header.total_samples / 44100);
        gui_menu_active_range (FALSE, menus_ID.sound_log, 1, 1);
    }
}

void    Sound_LogVGM_Accuracy_Switch(void)
{
    if (Sound.LogVGM_Logging_Accuracy == VGM_LOGGING_ACCURACY_SAMPLE)
    {
        Sound.LogVGM_Logging_Accuracy = VGM_LOGGING_ACCURACY_FRAME;
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Sound_Dumping_VGM_Acc_Frame));
    }
    else
    {
        Sound.LogVGM_Logging_Accuracy = VGM_LOGGING_ACCURACY_SAMPLE;
        Msg(MSGT_USER, "%s", Msg_Get(MSG_Sound_Dumping_VGM_Acc_Sample));
    }
    gui_menu_toggle_check (menus_ID.sound_log, 2);
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)
    {
        Msg(MSGT_USER_BOX, "%s", Msg_Get(MSG_Sound_Dumping_VGM_Acc_Change));
    }
}
