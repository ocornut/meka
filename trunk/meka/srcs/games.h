//-----------------------------------------------------------------------------
// MEKA - games.h
// Hidden Games - Headers
//-----------------------------------------------------------------------------
// Featuring:
//   - Johannes Holmberg (BreakOut & Pong)
//   - Nicolas Lannier / Archeide (Tetris)
//   - Julien Frelat / Gollum (Brainwash)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define GAME_RUNNING_NONE       (0)
#define GAME_RUNNING_BREAKOUT   (1)
#define GAME_RUNNING_TETRIS     (2)
#define GAME_RUNNING_BRAINWASH  (3)
#define GAME_RUNNING_PONG       (4)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

BITMAP  *games_bmp;
int     game_running;
int     slow_down_game;
void    Init_Games (void);

//-----------------------------------------------------------------------------
// Break Out
//-----------------------------------------------------------------------------

void    BreakOut_Start (void);
void    BreakOut_Init (void);
void    BreakOut_Update (void);
void    BreakOut_Move_Paddle (int where);
void    BreakOut_Reset_Bricks (void);
void    BreakOut_Draw_Score (void);

//-----------------------------------------------------------------------------
// Tetris
//-----------------------------------------------------------------------------

void    Tetris_Start (void);
void    Tetris_Init (void);
void    Tetris_Update (void);
void    Tetris_Move_Figure (int wherex, int wherey);
void    Tetris_Reset (void);
void    Tetris_Draw_Score (void);

//-----------------------------------------------------------------------------
// Brain Wash
//-----------------------------------------------------------------------------

void    BrainWash_Init (void);
void    BrainWash_Start (void);
void    BrainWash_Update (void);
void    BrainWash_Move_Erasor (int wherex, int wherey);
void    BrainWash_Reset_Board (void);
void    BrainWash_Draw_Score (void);

//-----------------------------------------------------------------------------
// Pong
//-----------------------------------------------------------------------------

void    Pong_Init (void);
void    Pong_Init_Ball (void);
void    Pong_Start (void);
void    Pong_Update (void);

//-----------------------------------------------------------------------------

typedef struct
{
 int paddle, score, bricks;
 float ballx, bally, bdx, bdy;
 BITMAP *scr;
} BreakOut_Type;

BreakOut_Type bo;

typedef struct
{
  int   type; /* type de figure */
//                    o
//        o  o   oo   o   o   o
//   o    o  o   oo   o  oo   oo
//  ooo  oo  oo       o  o     o
//   0    1   2   3   4   5   6
  int   orient; /* 0 = normal; 1 = horiz.; 2 = vert.; 3 = both */
  int   posx;
  int   posy;
  int   level;
  int   tab [20] [24];
  int   score;
  BITMAP *scr;
} Tetris_Type;

Tetris_Type to;

typedef struct
{
 int posx;
 int posy;
 int level;
 int figure;
 BITMAP *scr;
} BrainWash_Type;

BrainWash_Type bwo;

typedef struct
{
   float ballx,bally,balldx,balldy;
   int p1y,p2y;
   int p1score,p2score;
   BITMAP *scr;
} Pong_Type;

Pong_Type pong;

//-----------------------------------------------------------------------------

