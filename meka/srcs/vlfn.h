//-----------------------------------------------------------------------------
// MEKA - vlfn.h
// User filenames DB - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_vlfn_entry
{
    char *      filename;
    t_db_entry *db_entry;
};

struct t_vlfn_db
{
    char        filename [FILENAME_LEN];
    t_list *    entries;
};

extern t_vlfn_db VLFN_DataBase;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            VLFN_Init           (void);
void            VLFN_Close          (void);

t_vlfn_entry *  VLFN_FindByFileName (const char *file_name);
void            VLFN_AddEntry       (const char *file_name, t_db_entry *db_entry);
void            VLFN_RemoveEntry    (const char *file_name);

//-----------------------------------------------------------------------------

