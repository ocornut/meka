//-----------------------------------------------------------------------------
// MEKA - drivers.h
// Machine Drivers - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Machines
#define DRV_SMS         (0)
#define DRV_GG          (1)
#define DRV_SG1000      (2)
#define DRV_SC3000      (3)
#define DRV_COLECO      (4)
#define DRV_MSX         (5)
#define DRV_NES___		(6)
#define DRV_SF7000      (7)
#define DRV_MAX         (8)

// CPU Type
#define CPU_Z80         (0)

// VDP Type
#define VDP_SMSGG       (0)
#define VDP_TMS9918     (1)

// Sound Type
#define SND_SN76489AN   (0)
#define SND_SN76489     (1)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct ts_driver
{
  byte  id;
  char *short_name;
  char *full_name;
  int   cpu;
  int   vdp;
  int   snd;
  int   x_res;
  int   y_res;
  int   x_start;
  int   y_start;
  int   x_end;
  int   y_show_start; // Working variable
  int   y_show_end;   // Working variable
  int   y_int;        // Working variable
  int   colors;
  int   ram;
};

extern ts_driver * cur_drv;

struct ts_driver_filename_extension
{
    char *  filename_extension;
    int     driver;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void                    drv_init (void);
void                    drv_set (int);
int                     drv_get_from_filename_extension (const char *filename_extension);
int                     drv_id_to_mode (int);

//-----------------------------------------------------------------------------

