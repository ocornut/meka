//-----------------------------------------------------------------------------
// MEKA - blitintf.h
// Blitter configuration file and GUI interface - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BLITTER_NONE    ((t_blitter *)(0))
#define BLITTER_IGNORE  ((t_blitter *)(-1))

#define BLITTER_OS_SEP  "::"
#define BLITTER_OS_WIN  "WIN"
#define BLITTER_OS_UNIX "UNIX"

//-----------------------------------------------------------------------------
// Blitter Data
//-----------------------------------------------------------------------------

typedef struct  s_blitter
{
  char *        name;
  int           res_x;
  int           res_y;
  int           blitter;
  int           driver;
  int           flip;
  int           tv_colors;
  int           vsync;
  int           refresh_rate;
}               t_blitter;

//-----------------------------------------------------------------------------
// Blitter Functions
//-----------------------------------------------------------------------------

t_blitter *     Blitter_New (char *name);
void            Blitter_Delete (t_blitter *b);

//-----------------------------------------------------------------------------
// Blitters Data
//-----------------------------------------------------------------------------

typedef struct  s_blitters
{
 int            num;
 int            current_num;
 t_blitter *    current;
 t_list *       list;
 char           filename[FILENAME_LEN];
}               t_blitters;

t_blitters      blitters;

//-----------------------------------------------------------------------------
// Blitters Functions
//-----------------------------------------------------------------------------

void    Blitters_Load (void);
void    Blitters_Current_Update (void);
void    Blitters_Free (void);
void    Blitters_Init_Values (void);
int     Blitters_Parse_Line (char *, char *);
int     Blitters_Str2Num (char *s);
void    Blitters_Switch (void);
void    Blitters_Menu_Init (int menu_id);

//-----------------------------------------------------------------------------

