//-----------------------------------------------------------------------------
// MEKA - db.c
// MEKA Database - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Flags
#define DB_FLAG_BAD                 (1 << 0)
#define DB_FLAG_BIOS                (1 << 1)
#define DB_FLAG_HACK                (1 << 2)
#define DB_FLAG_TRANS               (1 << 3)  // A translation is defined as a hack whose primary purpose is translation
#define DB_FLAG_PROTO               (1 << 4)  // Do not apply to HOMEBREW stuff, because they're likely to be often updated
#define DB_FLAG_HOMEBREW            (1 << 5)
#define DB_FLAG_SMSGG_MODE          (1 << 6)

// Emulation Flags
#define DB_FLAG_EMU_3D              (1 << 7)
#define DB_FLAG_EMU_SPRITE_FLICKER  (1 << 8)

// Country Flags
#define DB_COUNTRY_EU               (1 << 0)
#define DB_COUNTRY_US               (1 << 1)
#define DB_COUNTRY_JP               (1 << 2)
#define DB_COUNTRY_BR               (1 << 3)
#define DB_COUNTRY_KR               (1 << 4)
#define DB_COUNTRY_HK               (1 << 5)
#define DB_COUNTRY_AU               (1 << 6)
#define DB_COUNTRY_NZ               (1 << 7)
#define DB_COUNTRY_FR               (1 << 8)
#define DB_COUNTRY_PT               (1 << 9)    // Used by translation
#define DB_COUNTRY_DE               (1 << 10)   // Used by translation
#define DB_COUNTRY_IT               (1 << 11)   // Used by translation
#define DB_COUNTRY_SP               (1 << 12)   // Used by translation
#define DB_COUNTRY_SW               (1 << 13)   // Used by translation
#define DB_COUNTRY_CH               (1 << 14)   // Unused now ?
#define DB_COUNTRY_UK               (1 << 15)   // Used by translation
#define DB_COUNTRY_CA               (1 << 16)
#define DB_COUNTRY_TW               (1 << 17)
#define DB_COUNTRY_COUNT_			18

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_db_name
{
    // 16 bytes + name data
    char *          name;                   // UTF-8
    int             country     : 31;       // In this field, only specific LANGUAGE fields are specified (JP,BR,KR,HK) or if the name is different
    int             non_latin   : 1;        // Set if non-latin UTF-8 data. If not set, name is romanized.
    t_db_name *		next;
};

struct t_db_entry
{
    // Basic fields (x bytes + names data + version + comment)
    int             system;                 // Parsed to DRV_* definitions, -1 if unknown
    u32             crc_crc32;              // CRC32
    t_meka_crc      crc_mekacrc;            // MekaCRC
    t_db_name *     names;                  // Names (1st is default name)
    int             country;				// Country flags
    int             flags;                  // Flags (see definitions above)
    char *          product_no;             // Product Number
    char *          version;                // Version note
    char *          comments;               // Comments
    char *          authors;                // Author(s)
    int             trans_country;          // Translation country (if applicable)

    // Emulation purpose (7 bytes)
    // Note that the fields are s8 just to save a bit of memory
    s8              emu_country;            // -1 if auto
    s8              emu_inputs;             // -1 if auto
    s8              emu_mapper;             // -1 if auto
    s8              emu_tvtype;             // -1 if auto
    s8              emu_vdp_model;          // -1 if auto
    s16             emu_iperiod;            // -1 if auto
};

struct t_db
{
    t_list *        entries;
    int             entries_counter_format_old;
    int             entries_counter_format_new;
	t_db_entry *	current_entry;
};

extern t_db         DB;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

bool                DB_Init                     (const char* filename, bool verbose = true);
void                DB_Close                    (void);

t_db_entry *        DB_Entry_Find               (u32 crc32, const t_meka_crc *mekacrc = NULL);

//void				DB_Entry_Print				(const t_db_entry *entry);
int                 DB_Entry_SelectDisplayFlag  (const t_db_entry *entry);
const char *        DB_Entry_GetCurrentName     (const t_db_entry *entry);
const t_db_name *   DB_Entry_GetNameByCountry   (const t_db_entry *entry, int country);
int                 DB_Entry_GetTranslationFlag (const t_db_entry *entry);

int					DB_FindDriverIdByName		(const char* name);
const char*			DB_FindDriverNameById		(int driver_id);

int					DB_FindCountryFlagByName	(const char* name);
const char*			DB_FindCountryNameByFlag	(int country_flag);

//-----------------------------------------------------------------------------
