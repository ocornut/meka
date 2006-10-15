//-----------------------------------------------------------------------------
// MEKA - clock.h
// Profiling Clocks - Headers
// by David Michel (DOS/x86 only)
//-----------------------------------------------------------------------------
// NOTE: outdated, currently unused, properly not functionnal
//-----------------------------------------------------------------------------

#ifdef CLOCK
#ifdef DOS
# include <pc.h>
#endif

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define CLOCK_CPU           (0)
#define CLOCK_GFX_PALETTE   (1)
#define CLOCK_GFX_BACK      (2)
#define CLOCK_GFX_SPRITES   (3)
#define CLOCK_GFX_BLIT      (4)
#define CLOCK_GUI_BLIT      (5)
#define CLOCK_GUI_REDRAW    (6)
#define CLOCK_GUI_UPDATE    (7)
#define CLOCK_FRAME_SKIPPER (8)
#define CLOCK_VSYNC         (9)
#define CLOCK_MAX           (10)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
  int           id;
  char *        name;
}               t_clock;

struct
  {
    unsigned int frame;
    unsigned int time;
    unsigned int skip;
    unsigned int min;
    unsigned int max;
    unsigned int average;
    unsigned int total;
    unsigned int start, stop;
    int active;
  } Clock [CLOCK_MAX];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Clock_Init      (void);
void    Clock_Close     (void);
void    Clock_Start     (int);
void    Clock_Stop      (int);
int     Clock_Diff      (int, int);
int     Clock_GetTime   (void);
char *  Clock_GetString (int);
void    Clock_Reset     (void);

void    Clock_Draw(void);

#else

#define Clock_Init()
#define Clock_Close()
#define Clock_Start(a)
#define Clock_Stop(a)
#define Clock_Diff(a,b)
#define Clock_GetTime()
#define Clock_GetString(a)
#define Clock_Reset()
#define Clock_Draw()

#endif

//-----------------------------------------------------------------------------

