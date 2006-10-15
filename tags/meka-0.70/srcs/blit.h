//-----------------------------------------------------------------------------
// MEKA - blit.h
// Blitters - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BLITTER_NORMAL          (0)
#define BLITTER_DOUBLE          (1)
#define BLITTER_SCANLINES       (2)
#define BLITTER_TVMODE          (3)
#define BLITTER_PARALLEL        (4)
#define BLITTER_TVMODE_DOUBLE   (5)
#define BLITTER_EAGLE           (6)
#define BLITTER_MAX             (7)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init (void);
void    Blit_Fullscreen (void);
void    Blit_Fullscreen_Misc (void);
void    Blit_Fullscreen_Message (void);
void    Blit_GUI (void);

void    Blit_Fullscreen_Normal             (void);
void    Blit_Fullscreen_Double             (void);
void    Blit_Fullscreen_Parallel           (void);
void    Blit_Fullscreen_Scanlines          (void);
void    Blit_Fullscreen_TV_Mode            (void);
void    Blit_Fullscreen_TV_Mode_Double     (void);
#ifdef MEKA_EAGLE
void    Blit_Fullscreen_Eagle              (void);
#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct          s_blitters_table_entry
{
  void (*func)();
  int x_fact, y_fact;
}                       t_blitters_table_entry;

t_blitters_table_entry Blitters_Table[BLITTER_MAX];

BITMAP *Work_Line;
void    Blitters_Get_Factors (int *, int *);

struct
{
  int   src_sx, dst_sx;
  int   src_sy, dst_sy;
  float tv_mode_factor;
} blit_cfg;

//-----------------------------------------------------------------------------

