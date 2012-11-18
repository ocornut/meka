//-----------------------------------------------------------------------------
// MEKA - cpu.h
// CPU related things (ports, memory accesses, interrupts) - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

extern int     CPU_Loop_Stop;  // Set to break from CPU emulation and return to mainloop()
extern int     CPU_ForceNMI;   // Set to force a NMI (currently only supported by the SG-1000/SC-3000 loop handlers)

//-----------------------------------------------------------------------------
// Macros and declarations, per Z80 emulator
//-----------------------------------------------------------------------------
// NOTE: MARAT_Z80 is the only maintained Z80 emulator in MEKA.
//       Some others may not compile as is, some may be way unusable.
//-----------------------------------------------------------------------------

#ifdef MARAT_Z80
  u16 Loop_SMS (void);
  #define Macro_Stop_CPU    { return (INT_QUIT); }
  #define CPU_GetPC()       (sms.R.PC.W)
  #define CPU_GetICount()   (sms.R.ICount)
  #define CPU_GetIPeriod()  (sms.R.IPeriod)
#elif RAZE_Z80
  #define INT_IRQ           (0x38)
  #define INT_NMI           (0x66)
  #define INT_NONE          (0xFFFF)
  #define INT_QUIT          (0xFFFE)
  #define Get_ICount        z80_get_cycles_elapsed ()
  #define Get_IPeriod       opt.Cur_IPeriod
#elif MAME_Z80
  #define INT_IRQ           (0x38)
  #define INT_NMI           (0x66)
  #define INT_NONE          (0xFFFF)
  #define INT_QUIT          (0xFFFE)
  void (*WrZ80)(word, byte);
  byte (*RdZ80)(word);
  word (*LoopZ80)();
  void CPU_Loop();
  int Get_Active_CPU (void);
  int Get_IRQ_Vector (int p);
  #define Macro_Stop_CPU    z80_exit()
  #define CPU_GetPC         z80_get_pc()
  #define Get_ICount        z80_ICount
  #define Get_IPeriod       opt.Cur_IPeriod
  word Loop_SMS (void);
#endif

#ifdef RAZE_Z80
  void (*WrZ80)(word, byte);
  byte (*RdZ80)(word);
  word (*LoopZ80)();
  void CPU_Loop();
  word Loop_SMS();
  #define Macro_Stop_CPU { return (INT_QUIT); }
  #define Raze_Update_Mapping(a, b, c) { z80_map_read (a, b, c); z80_map_fetch (a, b, c); }
#else
  #define Raze_Update_Mapping(a, b, c) { }
#endif

//-----------------------------------------------------------------------------

#define Interrupt_Loop_Misc_Line_Zero()                                     \
    {                                                                       \
    if (tsms.VDP_Video_Change)                                              \
       VDP_VideoMode_Change();                                             \
    if (Sound.LogVGM.Logging != VGM_LOGGING_NO)                             \
       VGM_NewFrame (&Sound.LogVGM);                                        \
    Patches_MEM_Apply();                                                   \
    }

//-----------------------------------------------------------------------------

#define Interrupt_Loop_Misc_Common                                          \
    {                                                                       \
    Sound_Update();                                                         \
    tsms.Control_Check_GUI = TRUE;                                          \
    Inputs_Sources_Update();   /* Poll input sources */                    \
    Inputs_Emulation_Update (TRUE); /* Might disable Control_Check_GUI */   \
    Inputs_Check_GUI (!tsms.Control_Check_GUI);                             \
    if ((opt.Force_Quit) || (CPU_Loop_Stop))                                \
       {                                                                    \
       /*tsms.VDP_Line --;*/ /* Not sure about its usefulness */            \
       Macro_Stop_CPU;                                                      \
       }                                                                    \
    }

//-----------------------------------------------------------------------------

#define Interrupt_Loop_Misc                                                 \
    {                                                                       \
    Interrupt_Loop_Misc_Common;                                             \
    if (Inputs.SK1100_Enabled)                                              \
       {                                                                    \
       if (Inputs_KeyDown(ALLEGRO_KEY_SCROLLLOCK))                          \
          Interrupt = INT_NMI;                                              \
       }                                                                    \
    else                                                                    \
    if ((tsms.Control_Start_Pause == 1) && (g_driver->id != DRV_GG))         \
       { tsms.Control_Start_Pause = 2; Interrupt = INT_NMI; }               \
    }

//-----------------------------------------------------------------------------

