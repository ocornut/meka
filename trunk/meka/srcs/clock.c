//-----------------------------------------------------------------------------
// MEKA - clock.c
// Profiling Clocks - Code
// by David Michel (DOS/x86 only)
//-----------------------------------------------------------------------------
// NOTE: outdated, currently unused, properly not functionnal
//-----------------------------------------------------------------------------

#include "shared.h"

#ifdef CLOCK

//-----------------------------------------------------------------------------
// x86 Macros
//-----------------------------------------------------------------------------

#define INT_DISABLE()   asm volatile ("cli")
#define INT_ENABLE()    asm volatile ("sti")
#define IO_DELAY()      asm volatile ("jmp 0f; 0:")

t_clock clocks[CLOCK_MAX] =
{
  { CLOCK_CPU,            "            CPU + I/O" },
  { CLOCK_GFX_PALETTE,    "          GFX Palette" },
  { CLOCK_GFX_BACK,       "    GFX Refresh BG/FG" },
  { CLOCK_GFX_SPRITES,    "  GFX Refresh Sprites" },
  { CLOCK_GFX_BLIT,       "      Fullscreen Blit" },
  { CLOCK_GUI_BLIT,       "             GUI Blit" },
  { CLOCK_GUI_REDRAW,     "           GUI Redraw" },
  { CLOCK_GUI_UPDATE  ,   "           GUI Update" },
  { CLOCK_FRAME_SKIPPER,  "        Frame Skipper" },
  { CLOCK_VSYNC,          "                VSync" },
};

// DRAW CLOCK VALUES ----------------------------------------------------------
void    Clock_Draw(void)
{
  int   i;
  char  s[256];

  Font_Change (F_SMALL);
  for (i = 0; i < CLOCK_MAX; i++)
      {
      sprintf (s, "%s%s", clocks[i].name, Clock_GetString(Clock[i].time));
      Font_Write (screen, str, 2, 5 + (Font_Height() * i), COLOR_BLACK);
      Font_Write (screen, str, 3, 6 + (Font_Height() * i), COLOR_WHITE);
      // rectfill (screen, 110, 30 + (20 * i), 110 + (Clock[i].time / 100), 45 + (20 * i), 80);
      Clock[i].time = 0;
      }
}

// START A CLOCK --------------------------------------------------------------
void    Clock_Start(int n)
{
  Clock[n].start  = Clock_GetTime();
  Clock[n].active = 1;
}

// STOP A CLOCK ---------------------------------------------------------------
void    Clock_Stop(int n)
{
  int   i;

  if (!Clock[n].active)
     return;

  Clock[n].frame++;
  Clock[n].stop    = Clock_GetTime();
  Clock[n].active  = 0;
  Clock[n].time    = Clock_Diff(Clock[n].start, Clock[n].stop);
  Clock[n].time   -= Clock[n].skip;
  Clock[n].skip    = 0;
  Clock[n].min     = MIN(Clock[n].min, Clock[n].time);
  Clock[n].max     = MAX(Clock[n].max, Clock[n].time);
  Clock[n].total  += Clock[n].time;
  Clock[n].average = Clock[n].total / Clock[n].frame;

  for (i = 0; i < CLOCK_MAX; i++)
      {
      if (Clock[i].active)
         {
         Clock[i].skip += Clock[n].time;
         }
      }
}

// CALCULATE DIFFERENCE BETWEEN A CLOCK START AND STOP ------------------------
int     Clock_Diff(int start, int stop)
{
  unsigned int t;
  if (start < stop)
     t = (start + 0x10000) - stop;
  else
     t = (start) - stop;
  return (t);
}

// RETURN CURRENT TIME --------------------------------------------------------
int     Clock_GetTime(void)
{
  unsigned int t;

  INT_DISABLE();
  outportb(0x43, 0x80);
  IO_DELAY();
  t  = inportb(0x42);
  IO_DELAY();
  t |= inportb(0x42) << 8;
  IO_DELAY();
  INT_ENABLE();

  return (t);
}

// RETURN A STRING CONTAINING A CLOCK VALUE IN AN HUMAN READABLE FORMAT -------
char *  Clock_GetString(int time)
{
  static char string[32];
  int value, ms, us;

  value = (time * 838) / 1000;
  ms = (value / 1000);
  us = (value % 1000);

  sprintf(string, "%3i.%03i", ms, us);
  return (string);
}

// INITIALIZE CLOCKS ----------------------------------------------------------
void    Clock_Init (void)
{
  INT_DISABLE ();
  outportb (0x43, 0xB4);
  IO_DELAY ();
  outportb (0x42, 0xFF);
  IO_DELAY ();
  outportb (0x42, 0xFF);
  IO_DELAY ();
  outportb (0x61, (inportb(0x61) & 0xFC) | 0x01);
  INT_ENABLE ();
  Clock_Reset ();
}

// RESET ALL CLOCKS -----------------------------------------------------------
void    Clock_Reset (void)
{
  int   i;

  for (i = 0; i < CLOCK_MAX; i++)
      {
      Clock[i].frame = 0;
      Clock[i].min = 999999999;
      Clock[i].max = 0;
      Clock[i].average = 0;
      Clock[i].skip = 0;
      Clock[i].total = 0;
      Clock[i].active = 0;
      }
}

// CLOSE CLOCK SYSTEM ---------------------------------------------------------
void    Clock_Close (void)
{
  outportb(0x61, inportb(0x61) & 0xFC);
}

#endif

//-----------------------------------------------------------------------------
