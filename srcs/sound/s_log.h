//
// Meka - S_LOG.H
// Sound Logging
//

#ifndef __S_LOG_H__
#define __S_LOG_H__

void    Sound_Log_Init (void);
void    Sound_Log_Close (void);

#define SOUND_LOG_ID_MAX (9999) /* Security measure for not going in infinite loop with short file name */
void    Sound_Log_Init_Game (void);

void    Sound_LogWAV_Start (void);
void    Sound_LogWAV_Stop (void);

void    Sound_LogVGM_Start (void);
void    Sound_LogVGM_Stop (void);
void    Sound_LogVGM_Accuracy_Switch (void);

#endif /* __S_LOG_H__ */

