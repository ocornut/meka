//-----------------------------------------------------------------------------
// MEKA - textview.h
// Text Viewer Applet - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define TEXTVIEW_LINES          (44)
#define TEXTVIEW_COLUMNS        (60)
#define TEXTVIEW_PAD_X          (4)
#define TEXTVIEW_SCROLLBAR_LX   (7)
#define TEXTVIEW_VEL_STD        (6) // 8
#define TEXTVIEW_VEL_MAX        (5678)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
  int           ID_Box, ID_Font;
  BITMAP *      ID_BoxGfx;
  int           Size_X, Size_Y; /* Columns, Lines */
  char **       Lines;
  int           Num_Lines;
  byte          Need_Update;
  int           Pad_X, Pad_Y;
  int           Len_X;
  int           Vel_Y, Pos_Y, Max_Pos_Y, Max_Start_Pos_Y, Pos_Per_Page;
  int           Font_Height;
} t_textviewer;

struct
{
  byte          Active;
  int           CurrentFile;
  t_textviewer *TV;
} TextViewer;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            TextViewer_Init(void);
int             TextViewer_Open(t_textviewer *TV, char *name, char *filename);
void            TextViewer_Update(t_textviewer *TV);
t_textviewer   *TextViewer_New(char *, int, int, int, int, int, int);
void            TextViewer_Update_Inputs(t_textviewer *TV);
void            TextViewer_Refresh(void);

void            TextViewer_Switch(char *box_title, char *filename, int current_file);
void            TextViewer_Switch_Doc_Main(void);
#ifdef WIN32
void            TextViewer_Switch_Doc_MainW(void);
#endif
#ifdef UNIX
void            TextViewer_Switch_Doc_MainU(void);
#endif
void            TextViewer_Switch_Doc_Compat(void);
void            TextViewer_Switch_Doc_Multiplayer_Games(void);
void            TextViewer_Switch_Doc_Changes(void);
void            TextViewer_Switch_Close(void);

//-----------------------------------------------------------------------------

