//-----------------------------------------------------------------------------
// MEKA - blitintf.h
// Blitter configuration file and GUI interface - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define BLITTER_OS_SEP              "::"
#define BLITTER_OS_DOS              "DOS"
#define BLITTER_OS_WIN              "WIN"
#define BLITTER_OS_UNIX             "UNIX"

typedef enum
{
    BLITTER_STRETCH_NONE        = 0,
    BLITETR_STRETCH_MAX_INT     = 1,     // Default
    BLITTER_STRETCH_MAX_RATIO   = 2,
    BLITTER_STRETCH_MAX         = 3,
} t_blitter_stretch;

//-----------------------------------------------------------------------------
// Blitter Data
//-----------------------------------------------------------------------------

typedef struct
{
    char *              name;
    int                 index;
    int                 res_x;
    int                 res_y;
    int                 blitter;
    //int                 driver;
    bool                tv_colors;
    int                 refresh_rate;
    t_blitter_stretch   stretch;
} t_blitter;

//-----------------------------------------------------------------------------
// Blitter Functions
//-----------------------------------------------------------------------------

t_blitter *     Blitter_New(char *name);
void            Blitter_Delete(t_blitter *b);

//-----------------------------------------------------------------------------
// Blitters Data
//-----------------------------------------------------------------------------

typedef struct
{
    int         count;
    t_blitter * current;
    t_list *    list;
    char        filename[FILENAME_LEN];
    char *      blitter_configuration_name;
}               t_blitters;

t_blitters      Blitters;

//-----------------------------------------------------------------------------
// Blitters Functions
//-----------------------------------------------------------------------------

void        Blitters_Init_Values(void);
void        Blitters_Init(void);
void        Blitters_Close(void);

t_blitter * Blitters_FindBlitterByName(const char *name);

void        Blitters_SwitchNext(void);
void        Blitters_Menu_Init(int menu_id);

//-----------------------------------------------------------------------------

