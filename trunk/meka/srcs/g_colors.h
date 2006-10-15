//-----------------------------------------------------------------------------
// MEKA - g_colors.h
// GUI Color Management & Setting - Headers
//-----------------------------------------------------------------------------

// Enable so that the palette viewer will show GUI palette
// #define SHOW_GUI_PALETTE

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// COLORS STARTING INDEXES ----------------------------------------------------
#define MAX_COLORS                    (256)
#define GUI_COL_START                 (64)
// ----------------------------------------------------------------------------

// GENERAL PURPOSE COLORS -----------------------------------------------------
#define GUI_COL_MISC_NUM              (20+19)              // Last 19 are for the keyboard
#define GUI_COL_BLACK                 (GUI_COL_START + 0)  // Black
#define GUI_COL_WHITE                 (GUI_COL_START + 10) // White
// ----------------------------------------------------------------------------

// THEME COLORS ---------------------------------------------------------------
#define GUI_COL_THEME_NUM             (9)
#define GUI_COL_THEME_START           (GUI_COL_START + GUI_COL_MISC_NUM)
#define GUI_COL_THEME_GRADIENTS_NUM   (16)
#define GUI_COL_THEME_GRADIENTS_START (GUI_COL_THEME_START + GUI_COL_THEME_NUM)
#define GUI_COL_THEME_GRADIENTS_POWER (1)
// ----------------------------------------------------------------------------
#define GUI_COL_BACKGROUND            (GUI_COL_THEME_START + 0) // Background 1
#define GUI_COL_BACKGROUND_2          (GUI_COL_THEME_START + 1) // Background 2
#define GUI_COL_TEXT_ACTIVE           (GUI_COL_THEME_START + 2)
#define GUI_COL_TEXT_N_ACTIVE         (GUI_COL_THEME_START + 3)
#define GUI_COL_TEXT_IN_BOX           (GUI_COL_THEME_START + 4)
#define GUI_COL_FILL                  (GUI_COL_THEME_START + 5)
#define GUI_COL_BORDERS               (GUI_COL_THEME_START + 6)
#define GUI_COL_HIGHLIGHT             (GUI_COL_THEME_START + 7)
#define GUI_COL_BARS                  (GUI_COL_THEME_START + 8)
#define GUI_COL_BUTTONS               (GUI_COL_BARS)
#define GUI_COL_TEXT_STATUS           (GUI_COL_TEXT_ACTIVE)
// ----------------------------------------------------------------------------

// MACHINE / AVAILABLE COLORS -------------------------------------------------
#define GUI_COL_AVAIL_START           (GUI_COL_THEME_GRADIENTS_START + GUI_COL_THEME_GRADIENTS_NUM)
#define GUI_COL_AVAIL_NUM             (MAX_COLORS - GUI_COL_AVAIL_START)
#define GUI_COL_MACHINE_START         (GUI_COL_AVAIL_START)
#define GUI_COL_MACHINE_NUM           (128)
#if GUI_COL_MACHINE_NUM > GUI_COL_AVAIL_NUM
 #error "Not enough colors availables. See G_COLORS.H"
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    gui_init_colors                 (void);
void    gui_init_colors_icons           (void);
void    gui_palette_update              (void);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

byte    gui_palette_need_update;

//-----------------------------------------------------------------------------

