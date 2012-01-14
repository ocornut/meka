//-----------------------------------------------------------------------------
// MEKA - db.c
// MEKA Database - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "db.h"
#include "vdp.h"
#include "tools/libparse.h"
#include "tools/tfile.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_db DB;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static bool		DB_Load			(const char *filename, bool verbose);

t_db_entry *    DB_Entry_New    (int system, u32 crc32, t_meka_crc *mekacrc);
void            DB_Entry_Delete (t_db_entry *entry);

t_db_name *     DB_Name_New     (t_db_entry *entry, char *name, int country, int non_latin);
void            DB_Name_Delete  (t_db_name *dbname);

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// Note: this is not exactly the same as the extension in the drivers.c table
static struct
{
    char name[4];
    int  driver_id;
} DB_SystemTable[] =
{
    { "SMS",    DRV_SMS    },
    { "GG\0",   DRV_GG     },
    { "SG1",    DRV_SG1000 },
    { "SC3",    DRV_SC3000 },
    { "SF7",    DRV_SF7000 },
    { "OMV",    DRV_SG1000 },
    { "COL",    DRV_COLECO },
    { "",       -1         },
};

int			DB_FindDriverIdByName(const char* name)
{
    for (int i = 0; DB_SystemTable[i].driver_id != -1; i++)
        if (!strcmp(name, DB_SystemTable[i].name))
            return (DB_SystemTable[i].driver_id);
    return (-1);
}

const char*  DB_FindDriverNameById(int id)
{
    for (int i = 0; DB_SystemTable[i].driver_id != -1; i++)
        if (DB_SystemTable[i].driver_id == id)
            return (DB_SystemTable[i].name);
    return NULL;
}

static struct
{
    char	name[3];
    int		country_flag;
} DB_CountryTable[] = 
{
    { "EU", DB_COUNTRY_EU   },
    { "US", DB_COUNTRY_US   },
    { "JP", DB_COUNTRY_JP   },
    { "BR", DB_COUNTRY_BR   },
    { "KR", DB_COUNTRY_KR   },
    { "HK", DB_COUNTRY_HK   },
    { "AU", DB_COUNTRY_AU   },
    { "NZ", DB_COUNTRY_NZ   },
    { "FR", DB_COUNTRY_FR   },
    { "PT", DB_COUNTRY_PT   },
    { "DE", DB_COUNTRY_DE   },
    { "IT", DB_COUNTRY_IT   },
    { "SP", DB_COUNTRY_SP   },
    { "SW", DB_COUNTRY_SW   },
    { "CH", DB_COUNTRY_CH   },
    { "UK", DB_COUNTRY_UK   },
    { "CA", DB_COUNTRY_CA   },
	{ "TW", DB_COUNTRY_TW   },
    { "",   -1              },
};

int  DB_FindCountryFlagByName(const char* name)
{
    for (int i = 0; DB_CountryTable[i].country_flag != -1; i++)
        if (!strcmp(name, DB_CountryTable[i].name))
            return (DB_CountryTable[i].country_flag);
    return (-1);
}

const char* DB_FindCountryNameByFlag(int country_flag)
{
    for (int i = 0; DB_CountryTable[i].country_flag != -1; i++)
        if (country_flag == DB_CountryTable[i].country_flag)
            return (DB_CountryTable[i].name);
    return NULL;
}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DB_Entry_New (int system, u32 crc32, t_meka_crc *mekacrc)
// Create and initialize a new DB entry
//-----------------------------------------------------------------------------
t_db_entry *    DB_Entry_New (int system, u32 crc32, t_meka_crc *mekacrc)
{
    t_db_entry * entry = (t_db_entry *)Memory_Alloc(sizeof (t_db_entry));

    // Initialize
    entry->system       = system;
    entry->crc_crc32    = crc32;
    entry->crc_mekacrc  = *mekacrc;
    entry->names        = NULL;
    entry->country		= 0;
    entry->flags        = 0;
    entry->product_no   = NULL;
    entry->version      = NULL;
    entry->comments     = NULL;
    entry->authors      = NULL;
    entry->trans_country= -1;
    entry->emu_country  = -1;
    entry->emu_inputs   = -1;
    entry->emu_iperiod  = -1;
    entry->emu_mapper   = -1;
    entry->emu_tvtype   = -1;
    entry->emu_vdp_model= -1;

    return (entry);
}

//-----------------------------------------------------------------------------
// DB_Entry_Delete (t_db_entry *entry)
// Delete given DB Entry
//-----------------------------------------------------------------------------
void            DB_Entry_Delete (t_db_entry *entry)
{
    if (entry->names != NULL)
    {
        t_db_name *dbname = entry->names;
        while (dbname != NULL)
        {
            t_db_name *next = dbname->next;
            DB_Name_Delete (dbname);
            dbname = next;
        }
    }
    if (entry->product_no != NULL)
        free (entry->product_no);
    if (entry->version != NULL)
        free (entry->version);
    if (entry->comments != NULL)
        free (entry->comments);
    free (entry);
}

//-----------------------------------------------------------------------------
// DB_Name_New (t_db_entry *, char *name, int country, int non_latin)
// Create and initialize a new DB Name, then add into a DB Entry
//-----------------------------------------------------------------------------
t_db_name *     DB_Name_New (t_db_entry *entry, char *name, int country, int non_latin)
{
    t_db_name* dbname = (t_db_name*)Memory_Alloc(sizeof (t_db_name));

    // Initialize
    dbname->name        = name;
    dbname->country     = country;
    dbname->non_latin   = non_latin;
    dbname->next        = NULL;

    // Add at end of entry chained list (Note: this is a relatively slow operation)
    if (entry->names == NULL)
    {
        entry->names = dbname;
    }
    else
    {
        t_db_name *previous = entry->names;
        while (previous->next != NULL)
            previous = previous->next;
        previous->next = dbname;
    }

    return (dbname);
}

//-----------------------------------------------------------------------------
// DB_Name_Delete (t_db_name *dbname)
// Delete given DB Name
//-----------------------------------------------------------------------------
void            DB_Name_Delete (t_db_name *dbname)
{
    free (dbname->name);
    free (dbname);
}

// Initialize and load Meka database
bool			DB_Init(const char* filename, bool verbose)
{
    DB.entries                      = NULL;
    DB.entries_counter_format_old   = 0;
    DB.entries_counter_format_new   = 0;

    DB.current_entry = NULL;
    return DB_Load(filename, verbose);
}

//-----------------------------------------------------------------------------
// DB_Load_Entry (char *line)
// Load DB entry from given line
//-----------------------------------------------------------------------------
static int      DB_Load_Entry (char *line)
{
    t_db_entry *entry;
    int         value;
    u32         crc_crc32;
    t_meka_crc  crc_mekacrc;
    char *      w;
    char        buf[1024+1];

    // Get system
    // FIXME: linear research with strcmp. Could be int compared, or even hashed.
    parse_getword(buf, 4, &line, " \t", ';');
    value = DB_FindDriverIdByName(buf);
    if (value == -1)
        return (1); // Silent return if there's anything else than a system

    parse_skip_spaces (&line);
    // printf("line: %s\n", line);

    // Get CRC32 & MekaCRC
    if (sscanf (line, "%08x %08X%08X", &crc_crc32, &crc_mekacrc.v[0], &crc_mekacrc.v[1]) != 3)
        return (0);
    line += 8 + 1 + 16;
    parse_skip_spaces (&line);

    // Create and add entry to global list
    entry = DB_Entry_New (value, crc_crc32, &crc_mekacrc); // value hold system
    list_add (&DB.entries, entry);
    DB.entries_counter_format_new++;

    // Get and add default name
    if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
        return (0);
    DB_Name_New (entry, w, 0, FALSE);

    // Parse optional parameters
    while ((w = parse_getword(buf, 1024, &line, "=/", ';')) != NULL)
    {
        StrUpper(w);
        // printf ("field '%s'\n", w);

        if (!strncmp (w, "NAME_", 5))
        {
            w += 5;
            if ((value = DB_FindCountryFlagByName(w)) == -1)
                return (0);
            if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
                break;
            DB_Name_New (entry, w, value, FALSE);
            // printf("adding country %d name %s", value, w);
        }
        else if (!strcmp (w, "COUNTRY"))
        {
            while (line[-1] == ',' || line[-1] == '=')
            {
                if (!(w = parse_getword(buf, 3, &line, ",/", ';')))
                    return (0);
                if ((value = DB_FindCountryFlagByName(w)) == -1)
                    return (0);
                entry->country |= value;
            }
        }
        else if (!strcmp (w, "PRODUCT_NO"))
        {
            if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
                return (0);
            entry->product_no = w;
        }
        else if (!strcmp (w, "EMU_COUNTRY"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",/", ';')))
                return (0);
            entry->emu_country = atoi (w);
        }
        else if (!strcmp (w, "EMU_INPUTS"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            StrLower(w);
            if (!strcmp(w, "joypad"))
                entry->emu_inputs = INPUT_JOYPAD;
            else if (!strcmp(w, "lightphaser"))
                entry->emu_inputs = INPUT_LIGHTPHASER;
            else if (!strcmp(w, "paddle"))
                entry->emu_inputs = INPUT_PADDLECONTROL;
            else if (!strcmp(w, "sportspad"))
                entry->emu_inputs = INPUT_SPORTSPAD;
            else if (!strcmp(w, "tvoekaki"))
                entry->emu_inputs = INPUT_TVOEKAKI;
            else
                return (0);
        }
        else if (!strcmp (w, "EMU_IPERIOD"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            entry->emu_iperiod = atoi (w);
        }
        else if (!strcmp (w, "EMU_MAPPER"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            entry->emu_mapper = atoi (w);
        }
        else if (!strcmp (w, "EMU_VDP"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            if ((entry->emu_vdp_model = VDP_Model_FindByName(w)) == -1)
                return (0);
        }
        else if (!strcmp (w, "EMU_TVTYPE"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            StrLower(w);
            if (!strcmp(w, "pal"))
                entry->emu_tvtype = TVTYPE_PAL_SECAM;
            else if (!strcmp(w, "ntsc"))
                entry->emu_tvtype = TVTYPE_NTSC;
            else
                return (0);
        }
        else if (!strcmp (w, "EMU_3D"))
            entry->flags |= DB_FLAG_EMU_3D;
        else if (!strcmp (w, "EMU_SPRITE_FLICKER"))
            entry->flags |= DB_FLAG_EMU_SPRITE_FLICKER;
        else if (!strcmp (w, "COMMENT"))
        {
            if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
                return (0);
            entry->comments = w;
        }
        else if (!strcmp (w, "FLAGS"))
        {
            while (line[-1] == ',' || line[-1] == '=')
            {
                if (!(w = parse_getword(buf, 16, &line, ",/", ';')))
                    return (0);
                if      (!strcmp (w, "BAD"))        entry->flags |= DB_FLAG_BAD;
                else if (!strcmp (w, "BIOS"))       entry->flags |= DB_FLAG_BIOS;
                else if (!strcmp (w, "SMSGG_MODE")) entry->flags |= DB_FLAG_SMSGG_MODE;
                else if (!strcmp (w, "HACK"))       entry->flags |= DB_FLAG_HACK;
                else if (!strcmp (w, "TRANS"))      entry->flags |= DB_FLAG_TRANS;
                else if (!strcmp (w, "PROTO"))      entry->flags |= DB_FLAG_PROTO;
                else if (!strcmp (w, "HOMEBREW"))   entry->flags |= DB_FLAG_HOMEBREW;
                else return (0);
            }
        }
        else if (!strcmp (w, "TRANS"))
        {
            if (!(w = parse_getword(buf, 1024, &line, "/", ';')))
                return (0);
            // In TRANS field only, 'EN' can be specified. It currently gets the UK flag.
            if (!strcmp (w, "EN"))
                value = DB_COUNTRY_UK;
            else if ((value = DB_FindCountryFlagByName(w)) == -1)
				return (0);
            entry->trans_country = value;
        }
        else if (!strcmp (w, "AUTHORS"))
        {
            if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
                return (0);
            entry->authors = w;
        }
        else if (!strcmp (w, "VERSION"))
        {
            if (!(w = parse_getword(NULL, 0, &line, "/", ';')))
                return (0);
            entry->version = w;
        }
        else
        {
            // No error for empty fields (",,")
            // This is mainly to handle end of line comments ("FIELD=heh    ; comment")
            if (strlen(w) > 0)
                return (0);
        }
    }

    // Ok
    return (1);
}

//-----------------------------------------------------------------------------
// DB_Load_EntryOldFormat (char *line)
// Load DB entry in old format from given line
//-----------------------------------------------------------------------------
static int      DB_Load_EntryOldFormat (char *line)
{
    t_db_entry *entry;
    t_meka_crc  mekacrc;
    char *      w;
    char        buf[1024+1];

    // Get MekaCRC
    sscanf (line, "%08X%08X", &mekacrc.v[0], &mekacrc.v[1]);
    line += 16 + 1;

    // Get name
    if (!(w = parse_getword(NULL, 0, &line, ",", ';')))
        return (0);

    // Create and add entry to global list
    // Note that the old format doesn't store system nor crc32
    entry = DB_Entry_New (-1, 0, &mekacrc);
    list_add (&DB.entries, entry);
    DB.entries_counter_format_old++;

    // Add default name
    DB_Name_New (entry, w, 0, FALSE);

    // Parse optional parameters
    while ((w = parse_getword(buf, 1024, &line, "=,", ';')) != NULL)
    {
        StrUpper(w);
        // printf ("field '%s'\n", w);
        if (!strcmp (w, "JAPNAME"))
        {
            if (!(w = parse_getword(NULL, 0, &line, ",", ';')))
                return (0);
            DB_Name_New (entry, w, DB_COUNTRY_JP, FALSE);
        }
        else if (!strcmp (w, "VER"))
        {
            // In the old format, the 'VER' field holds both COUNTRY and VERSION
            char *wp;
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);

            wp = buf;
            // printf("VER = %s\n", wp);

            // Get COUNTRY part
            if      (!strnicmp (wp, "Brazilian", 9))
            {   wp += 9; entry->country |= DB_COUNTRY_BR; }
            else if (!strnicmp (wp, "English",   7))
            {   wp += 7; entry->country |= DB_COUNTRY_EU | DB_COUNTRY_US; }
            else if (!strnicmp (wp, "European",  8))
            {   wp += 8; entry->country |= DB_COUNTRY_EU; }
            else if (!strnicmp (wp, "Export",    6))
            {   wp += 6; entry->country |= DB_COUNTRY_EU | DB_COUNTRY_US; }
            else if (!strnicmp (wp, "French",    6))
            {   wp += 6; entry->country |= DB_COUNTRY_FR; }
            else if (!strnicmp (wp, "Hong-Kong", 9))
            {   wp += 9; entry->country |= DB_COUNTRY_HK; }
            else if (!strnicmp (wp, "International", 13))
            {   wp += 13;   }
            else if (!strnicmp (wp, "Japanese",  8))
            {   wp += 8; entry->country |= DB_COUNTRY_JP; }
            else if (!strnicmp (wp, "Korean",    6))
            {   wp += 6; entry->country |= DB_COUNTRY_KR; }
            else if (!strnicmp (wp, "USA",       3))
            {   wp += 3; entry->country |= DB_COUNTRY_US; }
            // else
            //    printf("Unknown country in version!");
            
            // If there's anything left after known countries, it goes to VERSION
            if (*wp != EOSTR)
            {
                parse_skip_spaces (&wp);
                entry->version = strdup (wp);
                // printf("--> NEW VERSION = '%s'\n", entry->version);
            }
        }
        else if (!strcmp (w, "BAD"))
            entry->flags |= DB_FLAG_BAD;
        else if (!strcmp (w, "HACK"))
            entry->flags |= DB_FLAG_HACK;
        else if (!strcmp (w, "BIOS"))
            entry->flags |= DB_FLAG_BIOS;
        else if (!strcmp (w, "PROTO"))
            entry->flags |= DB_FLAG_PROTO;
        else if (!strcmp (w, "SMSGG_MODE")) 
            entry->flags |= DB_FLAG_SMSGG_MODE;
        else if (!strcmp (w, "3D"))
            entry->flags |= DB_FLAG_EMU_3D;
        else if (!strcmp (w, "FLICKER"))
            entry->flags |= DB_FLAG_EMU_SPRITE_FLICKER;
        else if (!strcmp (w, "COMMENT"))
        {
            if (!(w = parse_getword(NULL, 0, &line, ",", ';')))
                return (0);
            entry->comments = w;
        }
        else if (!strcmp (w, "ID"))
        {
            if (!(w = parse_getword(NULL, 0, &line, ",", ';')))
                return (0);
            entry->product_no = w;
        }
        else if (!strcmp (w, "MAPPER"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            entry->emu_mapper = atoi (w);
        }
        else if (!strcmp (w, "COUNTRY"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            entry->emu_country = atoi (w);
        }
        else if (!strcmp (w, "TRANS"))
        {
            int value;
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            // In TRANS field only, 'EN' can be specified. It currently gets the UK flag.
            if (!strcmp (w, "EN"))
                value = DB_COUNTRY_UK;
            else
                if ((value = DB_FindCountryFlagByName(w)) == -1)
                    return (0);
            entry->trans_country = value;
            entry->flags |= DB_FLAG_TRANS;
        }
        else if (!strcmp (w, "IPERIOD"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            entry->emu_iperiod = atoi (w);
        }
        else if (!strcmp (w, "TVTYPE"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            StrLower(w);
            if (!strcmp(w, "pal") || !strcmp(w, "secam") || !strcmp(w, "pal/secam"))
                entry->emu_tvtype = TVTYPE_PAL_SECAM;
            else if (!strcmp(w, "ntsc"))
                entry->emu_tvtype = TVTYPE_NTSC;
            else
                return (0);
        }
        else if (!strcmp (w, "VDP"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
			if ((entry->emu_vdp_model = VDP_Model_FindByName(w)) == -1)
				return (0);
        }
        else if (!strcmp (w, "INPUT"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            StrLower(w);
            if (!strcmp(w, "joypad"))
                entry->emu_inputs = INPUT_JOYPAD;
            else if (!strcmp(w, "lightphaser"))
                entry->emu_inputs = INPUT_LIGHTPHASER;
            else if (!strcmp(w, "paddle"))
                entry->emu_inputs = INPUT_PADDLECONTROL;
            else if (!strcmp(w, "sportspad"))
                entry->emu_inputs = INPUT_SPORTSPAD;
            else if (!strcmp(w, "tvoekaki"))
                entry->emu_inputs = INPUT_TVOEKAKI;
            else
                return (0);
        }
        else if (!strcmp (w, "AUTHORS"))
        {
            if (!(w = parse_getword(NULL, 0, &line, ",", ';')))
                return (0);
            entry->authors = w;
        }
        else if (!strcmp (w, "DATE"))
        {
            if (!(w = parse_getword(buf, 1024, &line, ",", ';')))
                return (0);
            // FIXME: yet ignored
        }
        else
            return (0); // Unknown field
    }

    // Ok
    return (1);
}

//-----------------------------------------------------------------------------
// DB_Load_Line (char *line)
// Process one line loading
//-----------------------------------------------------------------------------
static int      DB_Load_Line (char *line)
{
    int         i;
    int         ret;

    // printf("line: %s\n", line);

    // Detect DB entry format
    for (i = 0; i < 16; i++)
        if (!isxdigit(line[i]))
            break;
    if (i == 16 && line[16] == ',')
        ret = DB_Load_EntryOldFormat (line);
    else
        ret = DB_Load_Entry (line);

    // Ok
    return (ret);
}

//-----------------------------------------------------------------------------
// Initialize and load given Meka DB file
//-----------------------------------------------------------------------------
static bool		DB_Load (const char *filename, bool verbose)
{
	if (verbose)
		ConsolePrint (Msg_Get (MSG_DB_Loading));

    // Open and read file
    t_tfile* tf = tfile_read (filename);
    if (tf == NULL)
    {
        ConsolePrintf ("%s\n", meka_strerror());
        return false;
    }

    // Ok
    if (verbose)
		ConsolePrint ("\n");

    // Parse each line
    int line_cnt = 0;
    for (t_list* lines = tf->data_lines; lines; lines = lines->next)
    {
        line_cnt += 1;
        char* line = (char*)lines->elem;

        // Strip comments
        // FIXME
        // Trim spaces?
        // FIXME
        // Skip empty lines
        //if (line[0] == EOSTR)
        //    continue;

        if (DB_Load_Line (line) == 0)
        {
            tfile_free (tf); 
            Quit_Msg (Msg_Get (MSG_DB_SyntaxError), line_cnt);
        }
    }

    // Free file data
    tfile_free(tf);

    // FIXME - Uncomment for counting progress of DB transition
    //ConsolePrintf ("ENTRIES NEW = %d, OLD = %d\n", DB.entries_counter_format_new, DB.entries_counter_format_old);
    //Quit();

	return true;
}

//-----------------------------------------------------------------------------
// Release all Meka DB data
//-----------------------------------------------------------------------------
void            DB_Close (void)
{
    list_free_custom (&DB.entries, (t_list_free_handler)DB_Entry_Delete);
    DB.entries = NULL;
    DB.current_entry = NULL;
}

//-----------------------------------------------------------------------------
// Find DB entry matching given crc32 and/or mekacrc
// Set any parameter to zero for no comparaison
//-----------------------------------------------------------------------------
// FIXME: Could use a map/hash to find data back
// FIXME: Once all DB entries will be converted to new format, MekaCRC should be dropped.
//-----------------------------------------------------------------------------
t_db_entry *    DB_Entry_Find(u32 crc32, const t_meka_crc *mekacrc)
{
    // All given CRC should not be null
    if (!crc32 && (!mekacrc || (!mekacrc->v[0] && !mekacrc->v[1])))
        return (NULL);

    // Linear find
    for (t_list* list = DB.entries; list != NULL; list = list->next)
    {
        t_db_entry* entry = (t_db_entry*)list->elem;
        if ((!entry->crc_crc32 && mekacrc) || entry->crc_crc32 == crc32)
		{
			if (mekacrc)
			{
				if (entry->crc_mekacrc.v[0] != mekacrc->v[0] || entry->crc_mekacrc.v[1] != mekacrc->v[1])
					continue;
			}
			return (entry);
		}
    }

    return (NULL);
}

// Get current name for current DB entry
// Handle MEKA setting of the current country.
const char *	DB_Entry_GetCurrentName (const t_db_entry *entry)
{
    // In Japanese country mode, search for a Japanese name
    if (g_configuration.country == COUNTRY_JAPAN)
    {
        const t_db_name *name = DB_Entry_GetNameByCountry (entry, DB_COUNTRY_JP);
        if (name)
            return (name->name);
    }

    // Return first name
    return (entry->names->name);
}

// Find an entry name for given country. May return NULL if not found.
const t_db_name *	DB_Entry_GetNameByCountry (const t_db_entry *entry, int country)
{
    t_db_name * name = entry->names;
    while (name)
    {
        if (name->country == country)
            return (name);
        name = name->next;
    }
    return (NULL);
}

// Select flag to display for given DB entry
// Note: this is named 'Select' as it may return nothing, and cannot return
// multiple flags. This is used by the file browser / vlfn system.
int             DB_Entry_SelectDisplayFlag(const t_db_entry *entry)
{
    int         country = entry->country;

    // Return a flag only for single country games
    switch (country)
    {
    case DB_COUNTRY_AU : return (FLAG_AU);
    case DB_COUNTRY_BR : return (FLAG_BR);
    case DB_COUNTRY_JP : return (FLAG_JP);
    case DB_COUNTRY_KR : return (FLAG_KR);
    case DB_COUNTRY_FR : return (FLAG_FR);
    case DB_COUNTRY_US : return (FLAG_US);
    case DB_COUNTRY_HK : return (FLAG_HK);
    case DB_COUNTRY_NZ : return (FLAG_NZ);
    case DB_COUNTRY_PT : return (FLAG_PT);  // Unused in MEKA DataBase
    case DB_COUNTRY_DE : return (FLAG_DE);  // Unused in MEKA DataBase
    case DB_COUNTRY_IT : return (FLAG_IT);  // Unused in MEKA DataBase
    case DB_COUNTRY_SP : return (FLAG_SP);  // Unused in MEKA DataBase
    case DB_COUNTRY_SW : return (FLAG_SW);  // Unused in MEKA DataBase
    case DB_COUNTRY_CH : return (FLAG_CH);  // Unused in MEKA DataBase
    case DB_COUNTRY_UK : return (FLAG_UK);  // Unused in MEKA DataBase
    case DB_COUNTRY_CA : return (FLAG_CA);
	case DB_COUNTRY_TW : return (FLAG_TW);
    case DB_COUNTRY_EU : break;             // No country for Europe only
    }

    // Special case to account for asian releases
	if (country == (DB_COUNTRY_JP | DB_COUNTRY_KR))
		return FLAG_JP;
	if (country == (DB_COUNTRY_JP | DB_COUNTRY_TW))
		return FLAG_JP;

	// Do not display a flag
    return (-1);
}

// Get translation flag for given DB entry
int             DB_Entry_GetTranslationFlag  (const t_db_entry *entry)
{
    switch (entry->trans_country)
    {
    case DB_COUNTRY_AU : return (FLAG_AU);  // Unused in MEKA DataBase
    case DB_COUNTRY_BR : return (FLAG_BR);  // Unused in MEKA DataBase -> portuguese is used
    case DB_COUNTRY_JP : return (FLAG_JP);  // Unused in MEKA DataBase
    case DB_COUNTRY_KR : return (FLAG_KR);  // Unused in MEKA DataBase
    case DB_COUNTRY_FR : return (FLAG_FR);
    case DB_COUNTRY_US : return (FLAG_US);  // Unused in MEKA DataBase
    case DB_COUNTRY_HK : return (FLAG_HK);  // Unused in MEKA DataBase
    case DB_COUNTRY_NZ : return (FLAG_NZ);  // Unused in MEKA DataBase
    case DB_COUNTRY_PT : return (FLAG_PT);
    case DB_COUNTRY_DE : return (FLAG_DE);
    case DB_COUNTRY_IT : return (FLAG_IT);
    case DB_COUNTRY_SP : return (FLAG_SP);
    case DB_COUNTRY_SW : return (FLAG_SW);
    case DB_COUNTRY_CH : return (FLAG_CH);  // Unused in MEKA DataBase
    case DB_COUNTRY_EU : return (FLAG_EU);  // Unused in MEKA DataBase
    case DB_COUNTRY_UK : return (FLAG_UK);
    case DB_COUNTRY_CA : return (FLAG_CA);  // Unused in MEKA DataBase
    }

    return (-1);
}

//-----------------------------------------------------------------------------
