//-----------------------------------------------------------------------------
// MEKA - games.c
// Hidden Games - Code
//-----------------------------------------------------------------------------
// Featuring:
//   - Johannes Holmberg (BreakOut & Pong)
//   - Nicolas Lannier / Archeide (Tetris)
//   - Julien Frelat / Gollum (Brainwash)
//-----------------------------------------------------------------------------

#include "shared.h"
#include "games.h"
#include "inputs_t.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Games_Blit (void)
{
	blit (games_bmp, screenbuffer, 0, 0, 0, 0, 256, 192);
}

void    Init_Games (void)
{
	game_running = GAME_RUNNING_NONE;
	slow_down_game = 0;
	games_bmp = create_bitmap_ex(16, 256, 192);
	BreakOut_Init ();
	Tetris_Init ();
	BrainWash_Init ();
	Pong_Init ();
}

//-----------------------------------------------------------------------------
// Break Out
//-----------------------------------------------------------------------------

void    BreakOut_Start (void)
{
    if (!(machine & MACHINE_POWER_ON))
    {
        game_running = GAME_RUNNING_BREAKOUT;
        Msg (MSGT_USER, "Welcome to -BReAk oUt- by Johannes Holmberg");
    }
    BreakOut_Init ();
}

void    BreakOut_Init (void)
{
    bo.scr = games_bmp;
    clear_to_color (bo.scr, COLOR_BLACK);
    bo.paddle = 116;
    bo.ballx = 125;
    bo.bally = 180;
    switch (Random(4))
    { case 0: bo.bdx = -2.0; break; case 1: bo.bdx = -1.0; break;
    case 2: bo.bdx =  1.0; break; case 3: bo.bdx =  2.0; break; }
    bo.bdy = -2.0;
    bo.score = 0;
    BreakOut_Reset_Bricks ();
    BreakOut_Move_Paddle (0);
}

void    BreakOut_Reset_Bricks (void)
{
    int    x, y;

    bo.bricks = 56;
    for (y = 0; y < 64; y += 8)
        for (x = 0; x < 224; x += 32)
            rectfill (bo.scr, 16 + x, 48 + y, x + 46, y + 54, COLOR_WHITE);
}

void    BreakOut_Move_Paddle (int where)
{
    rectfill (bo.scr, bo.paddle, 184, bo.paddle + 39, 191, COLOR_BLACK);
    bo.paddle += where;
    rectfill (bo.scr, bo.paddle, 184, bo.paddle + 39, 191, COLOR_WHITE);
}

void    BreakOut_Draw_Score (void)
{
 int    i;
 char   s [20];
 static int last_score = -1;

 if (bo.score == last_score)
    return;

 Font_SetCurrent (F_MIDDLE);
 rectfill (bo.scr, 1, 1, 80, Font_Height(-1), COLOR_BLACK);
 sprintf (s, "SCORE: %d", bo.score);
 for (i = 0; i < 5; i ++) 
     if (Random(2)) 
         s [i] += ('a' - 'A');
 Font_Print (-1, bo.scr, s, 1, 1, COLOR_WHITE);
 last_score = bo.score;
 if ((bo.score % 56) == 0)
    switch (bo.score / 56)
       {
       case 1: Msg (MSGT_USER, "Are you crazy or what ?"); break;
       case 2: Msg (MSGT_USER, "Don't you have better things to do ?"); break;
       case 3: Msg (MSGT_USER, "There are better games to play, really.."); break;
       case 4: Msg (MSGT_USER, "Let me tell you a secret.."); break;
       case 5: Msg (MSGT_USER, "HAHAHAHA - I have no secrets to tell you (yet)"); break;
       case 6: Msg (MSGT_USER, "Still playing ?"); break;
       case 7: Msg (MSGT_USER, "Ok, I'll shut up."); break;
       default:
          if ((bo.score / 56) >= 8) // False secret message :)
             Msg (MSGT_USER, "Xfg6_ZhDjsA_9dyAyz_dSyQA_UdEDjDy2");
          break;
       }
}

void    BreakOut_Update (void)
{
    int    tx, ty;

    circlefill (bo.scr, bo.ballx, bo.bally, 3, COLOR_BLACK);
    bo.ballx += bo.bdx;
    bo.bally += bo.bdy;
    circlefill (bo.scr, bo.ballx, bo.bally, 3, COLOR_WHITE);
    tx = bo.ballx - 16;
    ty = bo.bally - 48;

    if (bo.bally < 160)
    {
        if (bo.bdy < 0)
            if (getpixel (bo.scr, bo.ballx, bo.bally - 4) != 0)
            {
                rectfill (bo.scr, (tx & 224) + 16, 48 + ((ty - 4) & 248), (tx & 224) + 46, 54 + ((ty - 4) & 248), COLOR_BLACK);
                bo.bdy =- bo.bdy; bo.bricks --; bo.score ++;
            }
            if (bo.bdx > 0)
                if (getpixel (bo.scr, bo.ballx + 4, bo.bally) != 0)
                {
                    rectfill (bo.scr, 16 + ((tx + 4) & 224), 48 + (ty & 248), ((tx + 4) & 224) + 46, (ty & 248) + 54, COLOR_BLACK);
                    bo.bdx =- bo.bdx; bo.bricks --; bo.score ++;
                }
                if (bo.bdy > 0)
                    if (getpixel (bo.scr, bo.ballx, bo.bally + 4) != 0)
                    {
                        rectfill (bo.scr, (tx & 224) + 16, 48 + ((ty + 4) & 248), (tx & 224) + 46, 54 + ((ty + 4) & 248), COLOR_BLACK);
                        bo.bdy =- bo.bdy; bo.bricks --; bo.score ++;
                    }
                    if (bo.bdx < 0)
                        if (getpixel (bo.scr, bo.ballx - 4, bo.bally) != 0)
                        {
                            rectfill (bo.scr, 16 + ((tx - 4) & 224), 48 + (ty & 248), ((tx - 4) & 224) + 46, (ty & 248) + 54, COLOR_BLACK);
                            bo.bdx =- bo.bdx; bo.bricks --; bo.score ++;
                        }
    }

    if (bo.bricks == 0)
        BreakOut_Reset_Bricks ();

    if (((int)bo.bally >= 180) && ((int)bo.bally < 184))
    {
        if (bo.ballx > bo.paddle)
            if (bo.ballx < bo.paddle + 40)
            {
                bo.bdy =- bo.bdy;
                bo.bdx = (bo.ballx - bo.paddle - 20) / 5;
            }
    }

    if ((int)bo.bally == 188) 
    { 
        BreakOut_Init (); 
        return; 
    }
    if ((int)bo.bally <= 14) 
        bo.bdy = -bo.bdy;
    if ((int)bo.ballx <= 4)
    {
        bo.bdx = -bo.bdx;
    }
    else if ((int)bo.ballx >= 251)
    {
        bo.bdx = -bo.bdx;
    }

    if (key [KEY_RIGHT])
        if (bo.paddle < 214)
            BreakOut_Move_Paddle (+5);
    if (key [KEY_LEFT])
        if (bo.paddle > 1)
            BreakOut_Move_Paddle (-5);

    BreakOut_Draw_Score ();
    Games_Blit ();
}

//-----------------------------------------------------------------------------
// Tetris
//-----------------------------------------------------------------------------

static int     f_sizex[7] = { 3, 2, 2, 1, 2, 2, 2 };
static int     f_sizey[7] = { 2, 3, 3, 4, 2, 3, 3 };

static int     figures[7][4][4] =
{
  { { 1, 1, 1, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 } },
  { { 1, 1, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 0, 0, 0, 0 } },
  { { 1, 1, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 0, 0 } },
  { { 1, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 0, 0, 0 } },
  { { 1, 1, 0, 0 },
    { 1, 1, 0, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 } },
  { { 1, 0, 0, 0 },
    { 1, 1, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 0, 0 } },
  { { 0, 1, 0, 0 },
    { 1, 1, 0, 0 },
    { 1, 0, 0, 0 },
    { 0, 0, 0, 0 } }
};

void    Tetris_Start (void)
{
    if (!(machine & MACHINE_POWER_ON))
    {
        game_running = GAME_RUNNING_TETRIS;
        Msg (MSGT_USER,"Welcome to -bUgGy tEtRiS- by Archeide");
    }
    Tetris_Init ();
}

void    Tetris_Init (void)
{
    to.scr = games_bmp;
    clear_to_color (to.scr, COLOR_BLACK);
    Tetris_Reset ();
    Tetris_Move_Figure (0, 0);
}

void    Tetris_Redraw (void)
{
    int   x, y;

    for (x = 0; x < 20; x++)
        for (y = 0; y < 24; y++)
            rectfill (to.scr, x*8, y*8, x*8+7, y*8+7, to.tab[x][y] ? COLOR_WHITE : COLOR_BLACK);
}

void    Tetris_Remove_Filled_Lines (void)
{
 int    x, y;
 int    filled_line;
 int    y2;
 int    nb_line = 0;

 for (y = 0; y < 24; y++)
     {
     filled_line = 1;
     for (x = 0;x < 20 && filled_line;x++)
         if (!to.tab[x][y])
            filled_line = 0;
     if (filled_line)
        {
        for (y2 = y-1;y2 >= 0;y2--)
            for (x = 0;x < 20;x++)
                to.tab[x][y2+1] = to.tab[x][y2];
        for (x = 0;x < 20;x++)
            to.tab[x][0] = 0;
        nb_line++;
        }
     }
 if (nb_line == 1) to.score += 10+to.level;
 if (nb_line == 2) to.score += 30+to.level*2;
 if (nb_line == 3) to.score += 60+to.level*3;
 if (nb_line == 4) to.score += 100+to.level*4;
 Tetris_Redraw ();
}

void    Tetris_Reset (void)
{
 int    i, j;

 to.type = Random(7);
 to.orient = Random(4);
 to.posx = 5;
 to.posy = 0;
 to.score = 0;
 to.level = 1;
 for (i = 0; i < 20; i++)
    for (j = 0; j < 24; j++)
       to.tab[i][j] = 0;
 rectfill (to.scr, 0, 0, 20*8, 24*8, COLOR_BLACK);
 line (to.scr, 165, 0, 165, 192, COLOR_WHITE);
}

int     Tetris_Test_Collisions (int wherex, int wherey)
{
 int    x, y;
 int    rx, ry;
 int    block;

 if (to.posx+wherex < 0)
    return (2);
 if (to.posx+((to.orient&1) ? f_sizey[to.type] : f_sizex[to.type])+wherex > 20)
    return (2);
 for (x = 0;x < 4;x++)
    for (y = 0;y < 4;y++)
       {
       if (to.orient&2)
          {
          rx = (to.orient&1)?(f_sizex[to.type]-1-y):(f_sizex[to.type]-1-x);
          ry = (to.orient&1)?(x):(f_sizey[to.type]-1-y);
          }
       else
          {
          rx = (to.orient&1)?(y):(x);
          ry = (to.orient&1)?(f_sizey[to.type]-1-x):(y);
          }
       block = 0;
       if (rx >= 0 && ry >= 0)
          block = figures[to.type][ry][rx];
       if (block)
          {
          if (to.posy+y+wherey > 23)
              return (1);
          if (to.tab[to.posx+x+wherex][to.posy+y+wherey])
              return (1);
          }
       }
 return (0);
}

void    Tetris_Draw_Figure (int clear)
{
 int    x, y;
 int    rx, ry;
 int    block;

 for (x = 0;x < 4;x++)
     for (y = 0;y < 4;y++)
         {
         if (to.orient&2)
            {
            rx = (to.orient&1)?(f_sizex[to.type]-1-y):(f_sizex[to.type]-1-x);
            ry = (to.orient&1)?(x):(f_sizey[to.type]-1-y);
            }
         else
            {
            rx = (to.orient&1)?(y):(x);
            ry = (to.orient&1)?(f_sizey[to.type]-1-x):(y);
            }
         if (rx < 0 || ry < 0)
            block = 0;
         else
            block = figures[to.type][ry][rx];
         if (block)
            {
            to.tab[to.posx+x][to.posy+y] = clear?0:1;
            rectfill(to.scr, (to.posx+x)*8, (to.posy+y)*8, (to.posx+x)*8+7, (to.posy+y)*8+7,
                     clear ? COLOR_BLACK : COLOR_WHITE);
            }
         }
}

void    Tetris_Move_Figure (int wherex, int wherey)
{
 Tetris_Draw_Figure (1);
 to.posx += wherex;
 to.posy += wherey;
 Tetris_Draw_Figure (0);
}

void    Tetris_Draw_Score (void)
{
 char          s[20];
 static int    last_score = -1;
 static int    last_level = -1;

 if (to.score == last_score)
    return;
 Font_SetCurrent (F_MIDDLE);
 rectfill (to.scr, 171, 1, 250, Font_Height(-1), COLOR_BLACK);
 sprintf (s, "Score: %d", to.score);
 Font_Print (-1, to.scr, s, 171, 1, COLOR_WHITE);
 last_score = to.score;
 if (to.level != last_level)
    {
    rectfill (to.scr, 171, 41, 250, Font_Height(-1), COLOR_BLACK);
    sprintf (s, "Level: %d", to.level);
    Font_Print (-1, to.scr, s, 171, 41, COLOR_WHITE);
    last_level = to.level;
    if (to.level > 1)
       switch (to.level)
          {
          case 2: Msg (MSGT_USER, "Do you enjoy wasting your time ?"); break;
          case 3: Msg (MSGT_USER, "I think you should get a life !"); break;
          case 4: Msg (MSGT_USER, "Ok... you must be very bored..."); break;
          case 5: Msg (MSGT_USER, "...So, i'll propose you a new game."); break;
          case 6: Msg (MSGT_USER, "Find the bugs of this game :-)"); break;
          case 7: Msg (MSGT_USER, "You don't have find them yet ?"); break;
          case 8: Msg (MSGT_USER, "Ok, I'll tell you one..."); break;
          case 9: Msg (MSGT_USER, "... play another 2147483638 levels.."); break;
          case 10: Msg (MSGT_USER, "... and you'll see that the game is very slow !"); break;
          default:
            switch (Random(3))
              {
              case 0:
                Msg (MSGT_USER, "only %i levels to wait..", 2147483638-to.level);
                break;
              case 1:
                Msg (MSGT_USER, "Windows will crash before, anyway");
                break;
              case 2:
                Msg (MSGT_USER, "In fact, Windows have the same bug :");
                Msg (MSGT_USER, "when the timer int overflows, Windows crash :-)");
                break;
             }
          }
    }
}

void    Tetris_Update (void)
{
 static int    timer = 0;
 static int    keep_space = 0;
 static int    keep_right = 0;
 static int    keep_left = 0;

 if (!slow_down_game)
    {
    slow_down_game = 1;
    Games_Blit ();
    return;
    }
 slow_down_game = 0;

  if (!key[KEY_RIGHT])
    keep_right = 0;
  if (key[KEY_RIGHT] && (!keep_right || keep_right++ >= 2))
    {
      Tetris_Draw_Figure(1);
      if (!Tetris_Test_Collisions(+1,0))
        {
          to.posx++;
          Tetris_Draw_Figure(0);
        }
      else
        Tetris_Draw_Figure(0);
      keep_right = 1;
    }
  if (!key[KEY_LEFT])
    keep_left = 0;
  if (key[KEY_LEFT] && (!keep_left || keep_left++ >= 2))
    {
      Tetris_Draw_Figure(1);
      if (!Tetris_Test_Collisions(-1,0))
        {
          to.posx--;
          Tetris_Draw_Figure(0);
        }
      else
        Tetris_Draw_Figure(0);
      keep_left = 1;
    }
  if (++timer > 10 - to.level || key [KEY_DOWN])
    {
      timer %= 10;
      Tetris_Draw_Figure(1);
      if (!Tetris_Test_Collisions(0,+1))
        {
          to.posy++;
          Tetris_Draw_Figure(0);
        }
      else
        {
          Tetris_Draw_Figure(0);
          Tetris_Remove_Filled_Lines();
          to.type = Random(7);
          to.orient = Random(4);
          to.posx = 10;
          to.posy = 0;
          Tetris_Move_Figure(0,0);
        }
    }
  if (!key[KEY_SPACE])
    keep_space = 0;
  if (key[KEY_SPACE] && !keep_space)
    {
      Tetris_Draw_Figure(1);
      to.orient = (to.orient+1)%4;
      Tetris_Draw_Figure(0);
      keep_space = 1;
    }
  if (key[KEY_ENTER])
    Tetris_Reset();
  to.level = (to.score / 100)+1;
  Tetris_Draw_Score();
 Games_Blit ();
}

//-----------------------------------------------------------------------------
// Brain Wash
//-----------------------------------------------------------------------------

static const int BrainWashfigures[13][5][5] =
{
/* FIGURE DE BASE */
  { { 0,0,0,0,0 },
    { 0,0,0,0,0 },
    { 0,0,1,0,0 },
    { 0,1,1,1,0 },
    { 0,0,0,0,0 } },
/* FIGURE DE BASE INVERSE...ARGH */
  { { 0,0,0,0,0 },
    { 0,1,1,1,0 },
    { 0,0,1,0,0 },
    { 0,0,0,0,0 },
    { 0,0,0,0,0 } },
/* UN CINQ QUI FAIT CHIER */
  { { 0,0,0,0,0 },
    { 0,1,0,1,0 },
    { 0,0,1,0,0 },
    { 0,1,0,1,0 },
    { 0,0,0,0,0 } },
/* UN ... CHIEN */
  { { 0,1,0,0,0 },
    { 1,1,0,0,1 },
    { 0,1,1,1,0 },
    { 0,1,0,1,0 },
    { 0,1,0,1,0 } },
/* UN ... OISEAU */
  { { 1,0,0,0,1 },
    { 1,0,0,0,1 },
    { 0,1,0,1,0 },
    { 0,1,0,1,0 },
    { 0,0,1,0,0 } },
/* UN ... BONHOMME */
  { { 1,1,0,1,1 },
    { 1,1,0,1,1 },
    { 0,0,1,0,0 },
    { 1,0,0,0,1 },
    { 0,1,1,1,0 } },
/* UNE ... A VOTRE AVIS ? */
  { { 0,0,1,0,0 },
    { 0,0,1,0,0 },
    { 0,0,1,0,0 },
    { 1,1,1,1,1 },
    { 1,1,0,1,1 } },
/* UN ... DAMIER ! */
  { { 1,0,1,0,1 },
    { 0,1,0,1,0 },
    { 1,0,1,0,1 },
    { 0,1,0,1,0 },
    { 1,0,1,0,1 } },
/* LE M DE MEKA */
  { { 1,0,0,0,1 },
    { 1,1,0,1,1 },
    { 1,0,1,0,1 },
    { 1,0,1,0,1 },
    { 1,0,0,0,1 } },
/* LE E DE MEKA */
  { { 1,1,1,1,1 },
    { 1,0,0,0,1 },
    { 1,1,1,0,0 },
    { 1,0,0,0,1 },
    { 1,1,1,1,1 } },
/* LE K DE MEKA */
  { { 1,0,0,1,1 },
    { 1,0,1,0,0 },
    { 1,1,0,0,0 },
    { 1,0,1,0,0 },
    { 1,0,0,1,1 } },
/* LE A DE MEKA */
  { { 0,1,1,1,0 },
    { 1,0,0,0,1 },
    { 1,1,1,1,1 },
    { 1,0,0,0,1 },
    { 1,0,0,0,1 } },
/* UN TRUC TROP DUR */
  { { 1,1,1,1,1 },
    { 0,1,0,0,1 },
    { 1,1,1,1,1 },
    { 1,0,1,1,0 },
    { 0,1,1,1,1 } }
};

void    BrainWash_Start (void)
{
 if (!(machine & MACHINE_POWER_ON))
    {
    game_running = GAME_RUNNING_BRAINWASH;
    Msg (MSGT_USER, "Welcome to -BrAin waSH- by Julien Frelat / Gollum");
    Msg (MSGT_USER, "Rules: Try to clean the black board with your");
    Msg (MSGT_USER, "     | magical eraser ! As you wish: a BRAIN WASH");
    Msg (MSGT_USER, "     | for you or a little RELAXATION.. :)");
    }
 BrainWash_Init ();
}

void    BrainWash_Draw_Figure (int x, int y)
{
 int i, j, px, py, CE, CF;

 for (j=0;j<5;j++)
    for (i=0;i<5;i++)
    {
        px = (x+i)<<2;
        py = ((y+j)<<2)+16;
        CE = BrainWashfigures[bwo.figure][j][i] == 1 ? COLOR_WHITE : COLOR_BLACK;
        CF = getpixel (bwo.scr, px, py);
        rectfill (bwo.scr, px,py,px+3,py+3,
                  CF == COLOR_BLACK
                  ? CE
                  : ((CE == COLOR_WHITE) ? COLOR_BLACK : CF));
    }
}

void BrainWash_Init (void)
{
  bwo.scr = games_bmp;
  bwo.level = 1;
  bwo.posx = 6 * 5;
  bwo.posy = 4 * 5;
  BrainWash_Reset_Board ();
}

void    BrainWash_Reset_Board (void)
{
  int   i;

  bwo.figure = (bwo.level-1)%13;
  clear_to_color (bwo.scr, COLOR_BLACK);
  for (i = 0; i < bwo.level; i += 1)
    BrainWash_Draw_Figure (Random(52), Random(32));
  BrainWash_Draw_Figure (bwo.posx,bwo.posy);
}

void    BrainWash_Draw_Erasor (int wherex, int wherey)
{
    line(bwo.scr, wherex, 16, wherex, 192, COLOR_WHITE);
    line(bwo.scr, 0,wherey, 256, wherey, COLOR_WHITE);
}

void    BrainWash_Move_Erasor (int wherex, int wherey)
{
  BrainWash_Draw_Figure (bwo.posx,bwo.posy);
  bwo.posx += wherex;
  bwo.posy += wherey;
  if (bwo.posx<0) bwo.posx = 0;
  if (bwo.posx>52) bwo.posx = 52;
  if (bwo.posy<0) bwo.posy = 0;
  if (bwo.posy>32) bwo.posy = 32;
  BrainWash_Draw_Figure (bwo.posx,bwo.posy);
}

void    BrainWash_Update (void)
{
 int dx = 0, dy = 0;
 int trouve = 0;
 int i, j;

 if (!slow_down_game)
    {
    slow_down_game = 1;
    Games_Blit ();
    return;
    }
 slow_down_game = 0;

 if (key[KEY_RIGHT]) dx=1;
 if (key[KEY_LEFT]) dx=-1;
 if (key[KEY_UP]) dy=-1;
 if (key[KEY_DOWN]) dy=1;
 if (key[KEY_T])
    {
    bwo.level+=1;
    BrainWash_Reset_Board();
    }
    if (Inputs_KeyPressed (KEY_SPACE, FALSE))
    {
    BrainWash_Draw_Figure(bwo.posx,bwo.posy);
    for (j=0;j<32;j++)
       for (i=0;i<52;i++)
          if (getpixel(bwo.scr,i<<2,(j<<2)+16) != 0)
             {
             trouve = 1;
             break;
             };
    if (!trouve)
       {
       bwo.level += 1;
       BrainWash_Reset_Board ();
       }
    }
 if ((dx!=0) || (dy!=0)) BrainWash_Move_Erasor (dx,dy);
 BrainWash_Draw_Score();
 Games_Blit ();
}

void    BrainWash_Draw_Score (void)
{
  char  s [20];
  static int last_level = -1;

  if (bwo.level == last_level)
     return;
  Font_SetCurrent (F_MIDDLE);
  rectfill (bwo.scr, 1, 1, 80, 8, COLOR_BLACK);
  sprintf (s, "LEVEL: %d", bwo.level);
  Font_Print (-1, bwo.scr, s, 1, 1, COLOR_WHITE);
  last_level = bwo.level;
  if (bwo.level==1) return;
  switch (bwo.level)
  {
   case 2:  Msg (MSGT_USER, "Too easy game..."); break;
   case 4:  Msg (MSGT_USER, "Oh did you notice that figures seem to change ?"); break;
   case 8:  Msg (MSGT_USER, "I suppose you notice that texts change too !"); break;
   case 10: Msg (MSGT_USER, "Er...Still there ?"); break;
   case 15: Msg (MSGT_USER, "You like this game ?"); break;
   case 25: Msg (MSGT_USER, "Woaw, you are a real warrior in order to come here"); break;
   case 35: Msg (MSGT_USER, "Ok, I'll tell you one..."); break;
   case 40: Msg (MSGT_USER, "... cheat code... try..."); break;
   case 60: Msg (MSGT_USER, "... the..."); break;
   case 65: Msg (MSGT_USER, "... T key !"); break;
   default:
     if (bwo.level>=100)  Msg (MSGT_USER, "You are a cheater !");
     if (bwo.level>=1000) Msg (MSGT_USER, "Doh ! A nice TV effect like Zoop's one !");
  }
}

//-----------------------------------------------------------------------------
// Pong
//-----------------------------------------------------------------------------

void    Pong_Start (void)
{
 if (!(machine & MACHINE_POWER_ON))
    {
    game_running = GAME_RUNNING_PONG;
    Msg (MSGT_USER, "Welcome to -PoNG- by Johannes Holmberg");
    }
 Pong_Init ();
}

void    Pong_Init (void)
{
 pong.scr = games_bmp;
 clear_to_color (pong.scr, COLOR_BLACK);
 Pong_Init_Ball ();
 pong.p1y = pong.p2y = 80;
 pong.p1score = pong.p2score = 0;
 rectfill (pong.scr, 9, pong.p1y, 13, pong.p1y + 32, COLOR_WHITE);
 rectfill (pong.scr, 250, pong.p2y, 254, pong.p2y + 32, COLOR_WHITE);
}

void    Pong_Init_Ball (void)
{
 pong.ballx = 128;
 pong.bally = 96;
 pong.balldx = 2; //(Random(2)) ? 0.5 : -0.5;
 pong.balldy = (Random(2)) ? 0.2 : -0.2;
}

void    Pong_Update (void)
{
 char   score [20];

 circlefill (pong.scr, pong.ballx, pong.bally, 3, COLOR_BLACK);
 pong.ballx += pong.balldx;
 pong.bally += pong.balldy;
 circlefill (pong.scr, pong.ballx, pong.bally, 3, COLOR_WHITE);
 if (pong.balldx > 0)
    {
    if (pong.ballx > 255)
       {
       pong.p1score ++;
       circlefill (pong.scr, pong.ballx, pong.bally, 3, COLOR_BLACK);
       Pong_Init_Ball ();
       }
    if (pong.ballx > 245)
       if ((pong.bally >= pong.p2y - 1) && (pong.bally <= pong.p2y + 33))
          {
          pong.balldx =- pong.balldx;
          pong.balldy = (pong.bally - pong.p2y - 16) / 6;
          }
    }
 else
    {
    if (pong.ballx < 9)
       {
       pong.p2score ++;
       circlefill (pong.scr, pong.ballx, pong.bally, 3, COLOR_BLACK);
       Pong_Init_Ball ();
       }
    if (pong.ballx < 19)
       if ((pong.bally >= pong.p1y - 1) && (pong.bally <= pong.p1y + 33))
          {
          pong.balldx =- pong.balldx;
          pong.balldy = (pong.bally - pong.p1y - 16) / 6;
          }
    }
 if (key[KEY_W] | key[KEY_S])
    {
    rectfill (pong.scr, 9, pong.p1y, 13, pong.p1y + 32, COLOR_BLACK);
    if ((key [KEY_W]) && (pong.p1y > 12)) pong.p1y -= 3;
    if ((key [KEY_S]) && (pong.p1y < 157)) pong.p1y += 3;
    rectfill (pong.scr, 9, pong.p1y, 13, pong.p1y + 32, COLOR_WHITE);
    }
 if (key [KEY_UP] | key [KEY_DOWN])
    {
    rectfill (pong.scr, 250, pong.p2y, 254, pong.p2y + 32, COLOR_BLACK);
    if ((key[KEY_UP]) && (pong.p2y > 12)) pong.p2y -= 3;
    if ((key[KEY_DOWN]) && (pong.p2y < 157)) pong.p2y += 3;
    rectfill (pong.scr, 250, pong.p2y, 254, pong.p2y + 32, COLOR_WHITE);
    }
 if ((pong.balldy > 0) && (pong.bally > 187)) pong.balldy =- pong.balldy;
 if ((pong.balldy < 0) && (pong.bally < 14))  pong.balldy =- pong.balldy;

 Font_SetCurrent (F_MIDDLE);
 sprintf (score, "%d - %d", pong.p1score, pong.p2score);
 rectfill (pong.scr, 0, 1, 255, 1 + Font_Height(-1), COLOR_BLACK);
 Font_Print (-1, pong.scr, score, 8 + ((248 - Font_TextLength(-1, score)) / 2), 1, COLOR_WHITE);

 Games_Blit ();
}

//-----------------------------------------------------------------------------

