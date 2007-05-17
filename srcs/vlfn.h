//-----------------------------------------------------------------------------
// MEKA - vlfn.h
// User filenames DB - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

typedef struct
{
    char *      filename;
    t_db_entry *db_entry;
} t_vlfn_entry;

typedef struct
{
    char        filename [FILENAME_LEN];
    t_list *    entries;
} t_vlfn_db;

t_vlfn_db       VLFN_DataBase;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void            VLFN_Init           (void);
void            VLFN_Close          (void);

t_vlfn_entry *  VLFN_FindByFileName (const char *file_name);
void            VLFN_AddEntry       (const char *file_name, t_db_entry *db_entry);
void            VLFN_RemoveEntry    (const char *file_name);

//-----------------------------------------------------------------------------

