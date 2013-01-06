//-----------------------------------------------------------------------------
// MEKA - patch.h
// Patching System - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#define PATCH_ACTION_SEEK               (1)
#define PATCH_ACTION_WRITE              (2)

#define PATCH_ACTION_SEEK_STR           "seek"
#define PATCH_ACTION_WRITE_STR          "write"

#define PATCH_CRC_TYPE_NONE             (0) // Apply to all ROM!
#define PATCH_CRC_TYPE_MEKACRC          (1)
#define PATCH_CRC_TYPE_CRC32            (2)

#define PATCH_CRC_MATCH_ALL             "*"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_patch_action
{
    int         address;
    int         data_length;
    u8 *        data;
};

struct t_patch
{
    int         crc_type;
    t_meka_crc  crc_mekacrc;
    u32         crc_crc32;
    t_list *    rom_patches;
    t_list *    mem_patches;
};

struct t_patches
{
    char        filename[FILENAME_LEN];
    t_list *    patches;
    t_patch *   patch_current;
};

extern t_patches Patches;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// Load MEKA.PAT
void	Patches_List_Init ();

// Apply patches
void    Patches_ROM_Initialize();
void    Patches_ROM_Apply();
void    Patches_MEM_Apply();

//-----------------------------------------------------------------------------

