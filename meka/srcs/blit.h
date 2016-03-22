//-----------------------------------------------------------------------------
// MEKA - blit.h
// Blitters - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BLITTER_NORMAL          (0)
#define BLITTER_TVMODE          (1)
#define BLITTER_TVMODE_DOUBLE   (2)
#define BLITTER_HQ2X            (3)
#define BLITTER_MAX             (4)

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Blit_Init();
void	Blit_CreateVideoBuffers();
void    Blit_DestroyVideoBuffers();

void    Blit_Fullscreen();
void	Blit_Fullscreen_UpdateBounds();

void    Blit_GUI();

void    Blit_Fullscreen_Normal          (void);
void    Blit_Fullscreen_Double          (void);
void    Blit_Fullscreen_TV_Mode         (void);
void    Blit_Fullscreen_TV_Mode_Double  (void);
void    Blit_Fullscreen_HQ2X            (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Buffers
extern ALLEGRO_BITMAP *         Blit_Buffer_LineScratch;    // Line buffer scratch pad
extern ALLEGRO_BITMAP *         Blit_Buffer_Double;         // Double-sized buffer

//-----------------------------------------------------------------------------
