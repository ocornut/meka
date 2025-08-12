//
// Meka - sound_logging.h
// Sound Logging
//

void    Sound_Log_Init();
void    Sound_Log_Close();

#define SOUND_LOG_ID_MAX (9999) /* Security measure for not going in infinite loop with short file name */
void    Sound_Log_Init_Game();

void    Sound_LogWAV_Start();
void    Sound_LogWAV_Stop();

void    Sound_LogVGM_Start();
void    Sound_LogVGM_Stop();
void    Sound_LogVGM_Accuracy_Switch();
