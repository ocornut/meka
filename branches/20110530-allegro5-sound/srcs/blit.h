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

void    Blit_Init();
void	Blit_CreateVideoBuffers();

void    Blit_Fullscreen();
void    Blit_Fullscreen_Misc();

void    Blit_GUI();

void    Blit_Fullscreen_Normal          (void);
void    Blit_Fullscreen_Double          (void);
void    Blit_Fullscreen_TV_Mode         (void);
void    Blit_Fullscreen_TV_Mode_Double  (void);
void    Blit_Fullscreen_Eagle           (void);
void    Blit_Fullscreen_HQ2X            (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_blitters_table_entry
{
	void	(*func)();
	int		x_fact;
	int		y_fact;
};

void        Blitters_Get_Factors(int *, int *);

struct t_blit_cfg
{
	int		src_sx;
	int		dst_sx;
	int		src_sy;
	int		dst_sy;
	float	tv_mode_factor;
};

extern t_blit_cfg blit_cfg;

// Buffers
extern ALLEGRO_BITMAP *         Blit_Buffer_LineScratch;    // Line buffer stratch pad
extern ALLEGRO_BITMAP *         Blit_Buffer_Double;         // Double-sized buffer

//-----------------------------------------------------------------------------
