//-----------------------------------------------------------------------------
// MEKA - drivers.h
// Machine Drivers - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Note: this field is saved into savestate so we have to maintain its binary compatibility.
enum t_machine_driver
{
	DRV_SMS		= 0,
	DRV_GG      = 1,
	DRV_SG1000	= 2,
	DRV_SC3000	= 3,
	DRV_COLECO	= 4,
	DRV_MSX___	= 5,
	DRV_NES___	= 6,
	DRV_SF7000	= 7,
	DRV_MAX		= 8,
};

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
  u8			id;
  const char *	short_name;
  const char *	full_name;
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

extern ts_driver * g_driver;

struct ts_driver_filename_extension
{
    const char *	filename_extension;
    int				driver;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void                    drv_init (void);
void                    drv_set (int);
int                     drv_get_from_filename_extension (const char *filename_extension);
int                     drv_is_known_filename_extension(const char *filename_extension);
int                     drv_id_to_mode (int);

//-----------------------------------------------------------------------------

