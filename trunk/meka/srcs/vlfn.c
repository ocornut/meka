//-----------------------------------------------------------------------------
// MEKA - vlfn.c
// User filenames DB - Code
//-----------------------------------------------------------------------------
// FIXME: rename from vlfn.* to something else?
//-----------------------------------------------------------------------------

#include "shared.h"
#include "app_filebrowser.h"
#include "db.h"
#include "vlfn.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

t_vlfn_db VLFN_DataBase;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static t_vlfn_entry *   VLFN_Entry_New      (const char *file_name, t_db_entry *db_entry);
static void             VLFN_Entry_Delete   (t_vlfn_entry *entry);

static void             VLFN_DataBase_Load  (void);
static void             VLFN_DataBase_Save  (void);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            VLFN_Init (void)
{
    VLFN_DataBase.entries = NULL;
    VLFN_DataBase_Load();
}

void            VLFN_Close (void)
{
    VLFN_DataBase_Save();
}

static t_vlfn_entry *   VLFN_Entry_New (const char *file_name, t_db_entry *db_entry)
{
    t_vlfn_entry *entry;
    entry = (t_vlfn_entry *)malloc(sizeof (t_vlfn_entry));
    entry->filename = (char *)file_name;
    entry->db_entry = db_entry;
    return (entry);
}

static void      VLFN_Entry_Delete (t_vlfn_entry *entry)
{
    free (entry->filename);
    free (entry);
}

static int       VLFN_Entries_Compare (t_vlfn_entry *entry1, t_vlfn_entry *entry2)
{
    return (strcmp(entry1->filename, entry2->filename));
}

//-----------------------------------------------------------------------------
// VLFN_DataBase_Load (void)
// Load VLFN database from MEKA.FDB file.
//-----------------------------------------------------------------------------
void            VLFN_DataBase_Load (void)
{
    t_tfile *   tf;
    t_list *    lines;
    char *      line;
    int         line_cnt;

    ConsolePrint(Msg_Get(MSG_FDB_Loading));

    // Open and read file
    tf = tfile_read (VLFN_DataBase.filename);
    if (tf == NULL)
    {
        ConsolePrintf ("%s\n", meka_strerror());
        return;
    }

    // Ok
    ConsolePrint("\n");

    // Parse each line
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        line_cnt += 1;
        line = (char*)lines->elem;

        char* w = parse_getword(NULL, 0, &line, "/", ';', PARSE_FLAGS_NONE);
        if (w == NULL)
            continue;
        else
        {
            char            buf[1024];
            char *          file_name;
            u32             crc_crc32;
            t_meka_crc      crc_mekacrc;

            // Save allocated filename
            file_name = w;

            // Get CRCs
            crc_crc32 = 0;
            crc_mekacrc.v[0] = crc_mekacrc.v[1] = 0;
            while ((w = parse_getword(buf, 1024, &line, "/", ';', PARSE_FLAGS_NONE)) != NULL)
            {
                if (!strncmp(w, "MEKACRC:", 8))
                {
                    if (sscanf(w + 8, "%08X%08X", &crc_mekacrc.v[0], &crc_mekacrc.v[1]) != 2)
                        continue; // Syntax error
                }
                else if (!strncmp(w, "CRC32:", 6))
                {
                    if (sscanf(w + 6, "%08x", &crc_crc32) != 1)
                        continue; // Syntax error
                }
            }

            // Requires at least MekaCRC
            // (to be changed by CRC32 when MekaCRC is dropped)
            if (crc_mekacrc.v[0] == 0 && crc_mekacrc.v[1] == 0)
                continue;

            {
                // Find DB entry
                t_db_entry *db_entry = DB_Entry_Find (crc_crc32, &crc_mekacrc);
                if (db_entry)
                {
                    // Create VLFN entry
                    t_vlfn_entry *  entry;
                    entry = VLFN_Entry_New (file_name, db_entry);
                    list_add (&VLFN_DataBase.entries, entry);
                }
            }
        }
    }

    // Free file data
    tfile_free(tf);
}

//-----------------------------------------------------------------------------
// VLFN_DataBase_Save (void)
// Save VLFN database back to MEKA.FDB file.
//-----------------------------------------------------------------------------
void        VLFN_DataBase_Save (void)
{
    FILE *  f;
    t_list *list;

    if ((f = fopen(VLFN_DataBase.filename, "wt")) == 0)
        return; // FIXME: report that somewhere ?

    // Sort entries by file name before writing, so the output file is more sexy
    list_sort (&VLFN_DataBase.entries, (int (*)(void *, void *))VLFN_Entries_Compare);

    // Write header
    fprintf(f, ";-----------------------------------------------------------------------------\n");
    fprintf(f, "; " MEKA_NAME " " MEKA_VERSION " - User Filenames DataBase\n");
    fprintf(f, "; Associate user filenames with MEKA DataBase entries.\n");
    fprintf(f, "; This information is used by the file loader.\n");
    fprintf(f, "; This file is automatically updated and rewritten by the emulator.\n");
    fprintf(f, ";-----------------------------------------------------------------------------\n\n");

    // Write all entries
    for (list = VLFN_DataBase.entries; list != NULL; list = list->next)
    {
        t_vlfn_entry* entry     = (t_vlfn_entry*)list->elem;
        t_db_entry* db_entry    = entry->db_entry;
        fprintf(f, "%s", entry->filename);
        if (db_entry->crc_crc32 != 0)
            fprintf(f, "/CRC32:%08x", db_entry->crc_crc32);
        // Note: MekaCRC is always written now!
        fprintf(f, "/MEKACRC:%08X%08X\n", db_entry->crc_mekacrc.v[0], db_entry->crc_mekacrc.v[1]);
    }

    fprintf(f, "\n;-----------------------------------------------------------------------------\n\n");

    // Close write
    fclose (f);

    // Free all entries
    list_free_custom (&VLFN_DataBase.entries, (t_list_free_handler)VLFN_Entry_Delete);
}

//-----------------------------------------------------------------------------
// VLFN_FindByFileName (const char *file_name)
// Retrieve VLFN entry from filename.
//-----------------------------------------------------------------------------
t_vlfn_entry *  VLFN_FindByFileName (const char *file_name)
{
    // Linear find
    for (t_list* list = VLFN_DataBase.entries; list != NULL; list = list->next)
    {
        t_vlfn_entry* entry = (t_vlfn_entry*)list->elem;
        if (!stricmp (file_name, entry->filename))
            return (entry);
    }
    return (NULL);
}

void        VLFN_AddEntry (const char *file_name, t_db_entry *db_entry)
{
    t_vlfn_entry *entry;

    entry = VLFN_FindByFileName (file_name);
    if (entry)
    {
        // Update existing entry
        entry->db_entry = db_entry;
    }
    else
    {
        // Add new entry
        entry = VLFN_Entry_New (strdup (file_name), db_entry);
        list_add (&VLFN_DataBase.entries, entry);
    }
}

void        VLFN_RemoveEntry (const char *file_name)
{
    t_vlfn_entry *entry = VLFN_FindByFileName (file_name);
    if (entry != NULL)
    {
        // Delete and remove from list
        list_remove(&VLFN_DataBase.entries, entry);
        VLFN_Entry_Delete(entry);

        // Ask file browser to reload names
        FB_Reload_Names();
    }
}

//-----------------------------------------------------------------------------

