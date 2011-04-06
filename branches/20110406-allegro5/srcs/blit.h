//-----------------------------------------------------------------------------
// MEKA - blit.h
// Blitters - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BLITTER_NORMAL          (0)
#define BLITTER_DOUBLE          (1)
#define BLITTER_TVMODE          (2)
#define BLITTER_TVMODE_DOUBLE   (3)
#define BLITTER_EAGLE           (4)
#define BLITTER_HQ2X            (5)
#define BLITTER_MAX             (6)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init (void);
void    Blit_Fullscreen (void);
void    Blit_Fullscreen_Misc (void);
void    Blit_Fullscreen_Message (void);
void    Blit_GUI (void);

void    Blit_Fullscreen_Normal          (void);
void    Blit_Fullscreen_Double          (void);
void    Blit_Fullscreen_TV_Mode         (void);
void    Blit_Fullscreen_TV_Mode_Double  (void);
void    Blit_Fullscreen_Eagle           (void);
void    Blit_Fullscreen_HQ2X            (void);

void	Blit_Fullscreen_CopyStretch		(void *src_buffer, int src_scale_x, int src_scale_y);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct          s_blitters_table_entry
{
	void	(*func)();
	int		x_fact;
	int		y_fact;
}                       t_blitters_table_entry;

void        Blitters_Get_Factors(int *, int *);

struct
{
	int		src_sx;
	int		dst_sx;
	int		src_sy;
	int		dst_sy;
	float	tv_mode_factor;
} blit_cfg;

// Buffers
extern ALLEGRO_BITMAP *         Blit_Buffer_LineScratch;    // Line buffer stratch pad
extern ALLEGRO_BITMAP *         Blit_Buffer_Double;         // Double-sized buffer
extern ALLEGRO_BITMAP *         Blit_Buffer_NativeTemp;

//-----------------------------------------------------------------------------
