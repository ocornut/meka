//-----------------------------------------------------------------------------
// MEKA - file.c
// ROM File Loading & File Tools - Code
//-----------------------------------------------------------------------------

#include "shared.h"
#include "bios.h"
#include "blitintf.h"
#include "file.h"
#include "db.h"
#include "debugger.h"
#include "desktop.h"
#include "patch.h"
#include "saves.h"
#include "sdsc.h"
#include "vlfn.h"
#ifdef MEKA_ZIP
    #include "libaddon/zip/unzip.h"
#endif

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

static int      Loading_UserVerbose = YES;

//-----------------------------------------------------------------------------
// Forward declaration
//-----------------------------------------------------------------------------

static int      Load_ROM_Init_Memory    (void);
int             Load_ROM_File           (void);
int             Load_ROM_Zipped         (void);
int             Load_ROM_Main           (void);
void            Load_ROM_Misc           (int reset);

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Check_OverDump()
// Check if the loaded media is an overdump, and reduce size accordingly
//-----------------------------------------------------------------------------
// Note: this function used to have a tolerance of 2 bytes to compare 
// overdumped data. So if two half of a file were only 2 bytes different,
// the file was considered as an overdump. This feature was removed, since
// it makes bad dump/over dump tracking more difficult.
//-----------------------------------------------------------------------------
// FIXME: Function should work on a t_media_image eventually.
//-----------------------------------------------------------------------------
static void     Check_OverDump (void)
{
    int         overdump_ratio = 1;

    // Msg (MSGT_DEBUG, "tsms.Size_ROM=%d", tsms.Size_ROM);

    // No check on ROM that are multiple of 8192 ...
    if ((tsms.Size_ROM % 8192) != 0)
        return; 

    // Added a limit to 8192 (the only smaller ROM I know is ColecoVision homebrew Tic Tac Toe)
    while (tsms.Size_ROM > 8192)
    {
        int size_half = tsms.Size_ROM / 2;
        u8 *d1 = &ROM[0];
        u8 *d2 = &ROM[size_half];
        int i;
        for (i = size_half; i > 0; i--)
            if (*d1++ != *d2++)
                break;
        if (i > 0)
            break;
        overdump_ratio *= 2;
        tsms.Size_ROM /= 2;
    }

    // Verbose
    if (Loading_UserVerbose && overdump_ratio > 1)
        Msg (MSGT_USER, Msg_Get (MSG_OverDump), overdump_ratio);
}

// INITIALIZE FILENAME STRINGS WITH APPROPRIATE VALUES ------------------------
void    Filenames_Init (void)
{
    char *p;

    // Get and save current directory
    getcwd (Env.Paths.StartingDirectory, FILENAME_LEN);

    // Find emulator directory --------------------------------------------------
    strcpy (Env.Paths.EmulatorDirectory, params_v [0]);
    #ifndef UNIX
        StrReplace (Env.Paths.EmulatorDirectory, '\\', '/');
    #endif
    p = strrchr (Env.Paths.EmulatorDirectory, '/');
    if (p)
        *p = EOSTR;
    else
        strcpy (Env.Paths.EmulatorDirectory, Env.Paths.StartingDirectory);

    // ConsolePrintf ("Env.Paths.StartingDirectory = %s\n", Env.Paths.StartingDirectory);
    // ConsolePrintf ("Env.Paths.EmulatorDirectory = %s\n", Env.Paths.EmulatorDirectory);
    // ConsolePrintf ("params_v[0] = %s\n", params_v [0]);

#ifdef UNIX
    {   // ????
        int len;
        strcpy (file.temp, Env.Paths.EmulatorDirectory);
        realpath (file.temp, Env.Paths.EmulatorDirectory);
        len = strlen (Env.Paths.EmulatorDirectory);
        Env.Paths.EmulatorDirectory [len] = '/';
        Env.Paths.EmulatorDirectory [len + 1] = EOSTR;
    }
#else
    strlwr (Env.Paths.EmulatorDirectory);
#endif

    // Datafiles ----------------------------------------------------------------
    sprintf (Env.Paths.DataFile,        "%s/meka.dat",      Env.Paths.EmulatorDirectory);
    sprintf (Env.Paths.DataBaseFile,    "%s/meka.nam",      Env.Paths.EmulatorDirectory);
    sprintf (Patches.filename,        "%s/meka.pat",        Env.Paths.EmulatorDirectory);
    sprintf (VLFN_DataBase.filename,    "%s/meka.fdb",      Env.Paths.EmulatorDirectory);
    sprintf (Themes.filename,         "%s/meka.thm",    Env.Paths.EmulatorDirectory);
    sprintf (blitters.filename,       "%s/meka.blt",    Env.Paths.EmulatorDirectory);
    //sprintf (registered.filename [0], "%s/meka.reg",    Env.Paths.EmulatorDirectory);
    //sprintf (registered.filename [1], "%s/meka.key",    Env.Paths.EmulatorDirectory);
    sprintf (Desktop.filename,        "%s/meka.dsk",    Env.Paths.EmulatorDirectory);
    sprintf (Inputs.FileName,         "%s/meka.inp",    Env.Paths.EmulatorDirectory);
    sprintf (Messages.FileName,       "%s/meka.msg",    Env.Paths.EmulatorDirectory);

    // Documentations -----------------------------------------------------------
    sprintf (Env.Paths.DocumentationMain,   "%s/meka.txt",      Env.Paths.EmulatorDirectory);
#ifdef WIN32
    sprintf (Env.Paths.DocumentationMainW,  "%s/mekaw.txt",     Env.Paths.EmulatorDirectory);
#elif UNIX
    sprintf (Env.Paths.DocumentationMainU,  "%s/mekanix.txt",   Env.Paths.EmulatorDirectory);
#endif
    sprintf (Env.Paths.DocumentationCompat, "%s/compat.txt",    Env.Paths.EmulatorDirectory);
    sprintf (Env.Paths.DocumentationMulti,  "%s/multi.txt",     Env.Paths.EmulatorDirectory);
    sprintf (Env.Paths.DocumentationChanges,"%s/changes.txt",   Env.Paths.EmulatorDirectory);

    // Configuration file -------------------------------------------------------
#ifdef WIN32
    sprintf (Env.Paths.ConfigurationFile,   "%s/mekaw.cfg",     Env.Paths.EmulatorDirectory);
#else
    sprintf (Env.Paths.ConfigurationFile,   "%s/meka.cfg",      Env.Paths.EmulatorDirectory);
#endif

    // Directories
#ifdef DOS
    sprintf (file.dir_captures,       "%s/Shots",       Env.Paths.EmulatorDirectory);
#else
    sprintf (file.dir_captures,       "%s/Screenshots", Env.Paths.EmulatorDirectory);
#endif
    sprintf (file.dir_saves,          "%s/Saves",       Env.Paths.EmulatorDirectory);
    sprintf (file.dir_musics,         "%s/Music"  ,     Env.Paths.EmulatorDirectory);
    sprintf (file.dir_debug,          "%s/Debug",       Env.Paths.EmulatorDirectory);

    // ROM ----------------------------------------------------------------------
    strcpy (file.rom,  "");
    strcpy (file.save, "");
}

void    Filenames_Init_ROM (void)
{
    // ROM (when parsed from command line)
    if (StrNull (file.rom))
    {
        strcpy (file.save, "");
        return;
    }

    // Save state
    strcpy   (file.temp, file.rom);
    killext  (file.temp);
    killpath (file.temp);
    sprintf  (file.save, "%s/%s.sav", file.dir_saves, file.temp);
}

bool    Load_ROM_Command_Line (void)
{
    if (StrNull(file.rom))
        return (FALSE);
    return Load_ROM (LOAD_COMMANDLINE, YES);
}

//-----------------------------------------------------------------------------
// Reload_ROM (void)
// Reload current ROM.
//-----------------------------------------------------------------------------
bool    Reload_ROM (void)
{
    if (StrNull(file.rom))
    {
        Msg (MSGT_USER, "%s", Msg_Get(MSG_LoadROM_Reload_No_ROM));
        return (FALSE);
    }
    if (Load_ROM (LOAD_INTERFACE, FALSE))
    {
        Msg (MSGT_USER, "%s", Msg_Get(MSG_LoadROM_Reload_Reloaded));
        return (TRUE);
    }
    return (FALSE);
}

//-----------------------------------------------------------------------------
// Load_ROM (int mode, int user_verbose)
// Load media/ROM from given filename.
// If user_verbose if false, avoid printing stuff to the message box
//-----------------------------------------------------------------------------
// Note: path to ROM filename must be set in 'file.rom' before calling this
//-----------------------------------------------------------------------------
bool    Load_ROM (int mode, int user_verbose)
{
    int   reset;

    // Set file global flag
    Loading_UserVerbose = user_verbose;

    switch (mode)
    {
    case LOAD_COMMANDLINE:
        if (user_verbose)
            ConsolePrint (Msg_Get (MSG_LoadROM_Loading));
        break;
    case LOAD_INTERFACE:
        // FIXME: do not save Backed Memory in non-verbose mode
        // This mode is only used by the file browser. This way we avoid loading
        // and saving all battery backed memory when doing a "Load All".
        // Of course, this is a little hack but it's better this way.
        if (user_verbose)
            BMemory_Save ();
        break;
    }

    if (Load_ROM_Main () != MEKA_ERR_OK)
    {
        switch (mode)
        {
        case LOAD_COMMANDLINE:
            Quit_Msg("%s\n\"%s\"\n", meka_strerror(), file.rom);
            // Quit_Msg (meka_strerror());
            return (FALSE);
        case LOAD_INTERFACE:
            Msg (MSGT_USER, Msg_Get (MSG_Error_Base), meka_strerror());
            return (FALSE);
        }
    }

    // If we are already in SF-7000 mode, do not reset ---------------------------
    if (cur_drv->id == DRV_SF7000)
        reset = NO;
    else
        reset = YES;

    // Miscellaenous stuff (including reset)
    Load_ROM_Misc (reset);

    if (mode == LOAD_COMMANDLINE)
    {
        if (user_verbose)
            ConsolePrint ("\n");
        if (opt.State_Load != -1)
        {
            opt.State_Current = opt.State_Load; // Note: we're not calling the function to avoid displaying the 'slot change' message
            opt.State_Load = -1;
            Load_Game ();
        }
    }

    // Verbose
    if (user_verbose)
    {
        // Display success message
        StrCpyPathRemoved (GenericBuffer, file.rom);
        if (cur_drv->id != DRV_SF7000)
            Msg (MSGT_USER, Msg_Get (MSG_LoadROM_Success), GenericBuffer);
        else
            Msg (MSGT_USER, Msg_Get (MSG_LoadDisk_Success), GenericBuffer);

        // Display data from DB
        if (DB_CurrentEntry)
        {
            // Name
            Msg (MSGT_USER, "\"%s\"", DB_Entry_GetCurrentName (DB_CurrentEntry));

            // Comment
            if (DB_CurrentEntry->comments)
                Msg (MSGT_USER_BOX, Msg_Get (MSG_LoadROM_Comment), DB_CurrentEntry->comments);

            // Show SMS-GG mode info
            if (DB_CurrentEntry->flags & DB_FLAG_SMSGG_MODE)
            {
                if (DB_CurrentEntry->comments)
                    // Append to comment message
                    Msg (MSGT_USER_BOX, "%s", Msg_Get (MSG_LoadROM_SMSGG_Mode_Comment));
                else // Print the comment marker before
                    Msg (MSGT_USER_BOX, Msg_Get (MSG_LoadROM_Comment), Msg_Get (MSG_LoadROM_SMSGG_Mode_Comment));
            }

            // Show BAD ROM warning
            if (DB_CurrentEntry->flags & DB_FLAG_BAD)
            {
                Msg (MSGT_USER_BOX, Msg_Get (MSG_LoadROM_Warning));
                Msg (MSGT_USER_BOX, Msg_Get (MSG_LoadROM_Bad_Dump_Long));
                Msg (MSGT_USER_INFOLINE, Msg_Get (MSG_LoadROM_Bad_Dump_Short));
            }

            // Show Product Number
            if (DB_CurrentEntry->product_no && Configuration.show_product_number)
                Msg (MSGT_USER_BOX, Msg_Get (MSG_LoadROM_Product_Num), DB_CurrentEntry->product_no);
        }

        // Show SDSC Header
        if (user_verbose)
            SDSC_Read_and_Display ();
    }

    // Automatically change input peripheral depending on MEKA.NAM entry
    // FIXME: we don't do it at all if !user_verbose, to save us from passing
    // the verbose flag all down to inputs peripherals changing functions.
    // Since the verbose flag is only cleared by the file browser "Load All"
    // functionnality, it is ok to avoid changing inputs.
    if (user_verbose)
        Input_ROM_Change ();

    return (TRUE);
}

void    Load_Header_and_Footer_Remove (int *pstart, long *psize)
{
    int start = 0;
    int size = *psize;
    
    switch (cur_machine.driver_id)
    {
    case DRV_COLECO: //--- Coleco Vision
        // Skip 128 bytes header + 512 bytes footer if necessary
        // (both on the same time)
        if (size > 0x1000)
        {
            if ((size % 0x1000) == 128 + 512)
            { start += 128; size -= 128 + 512; }
            else
            {
                if ((size % 0x1000) == 128)
                { start += 128; size -= 128; }
                if ((size % 0x1000) == 512)
                { size -= 512; }
            }
        }
        break;
    case DRV_SF7000: //--- SF-7000
        break;
    case DRV_NES:    //--- Nintendo
        break;
    default:         //--- Master System, Game Gear, SG-1000, SC-3000...
        // Skip 512 bytes header if necessary
        if (size > 0x4000 && (size % 0x4000) == 512)
        { start += 512; size -= 512; }
        // Skip 64 bytes footer if necessary
        if (size > 0x1000 && (size % 0x1000) == 64)
        { size -= 64; }
        break;
    }

    *pstart = start;
    *psize = size;
}

//-----------------------------------------------------------------------------
// Load_ROM_Zipped (void)
// Load ROM from a ZIP file
//-----------------------------------------------------------------------------
#ifdef MEKA_ZIP
int             Load_ROM_Zipped (void)
{
    int           err = UNZ_OK;
    unzFile       zf = NULL;
    unz_file_info zf_infos;
    int           start_at;

    zf = unzOpen(file.rom);
    if (zf == NULL)
        return (MEKA_ERR_ZIP_LOADING); // Error loading ZIP file

    // Locating..
    err = unzGoToFirstFile (zf);
    if (err != UNZ_OK) { unzClose (zf); return (MEKA_ERR_ZIP_INTERNAL); }

    // Getting informations..
    unzGetCurrentFileInfo (zf, &zf_infos, file.temp, FILENAME_LEN, NULL, 0, NULL, 0);
    tsms.Size_ROM = zf_infos.uncompressed_size;

    // Setting driver ------------------------------------------------------------
    // Must be done there because we don't have the filename before..
    keepext (file.temp);
    strupr (file.temp);
    cur_machine.driver_id = drv_get_from_ext (file.temp);

    // Remove Header & Footer
    Load_Header_and_Footer_Remove (&start_at, &tsms.Size_ROM);

    // Check out if the ROM isn't actually empty
    if (tsms.Size_ROM <= 0)
    { unzClose (zf); return (MEKA_ERR_FILE_EMPTY); } /* File empty */

    // Allocate necessary memory to load ROM -------------------------------------
    if (Load_ROM_Init_Memory () == -1)
    { unzClose (zf); return (MEKA_ERR_MEMORY); } /* Not enough memory */

    // Opening..
    err = unzOpenCurrentFile (zf);
    if (err != UNZ_OK) { unzClose (zf); return (MEKA_ERR_ZIP_INTERNAL); }

    // Skipping header if necessary..
    if (start_at != 0)
        unzReadCurrentFile (zf, ROM, start_at);

    // Reading..
    err = unzReadCurrentFile (zf, ROM, tsms.Size_ROM);
    if (err < 0 || err != tsms.Size_ROM)
    {
        unzCloseCurrentFile (zf);
        unzClose (zf);
        return (MEKA_ERR_ZIP_INTERNAL);
    }

    // Closing..
    unzCloseCurrentFile (zf);
    unzClose (zf);

    return (MEKA_ERR_OK);
}
#endif // ifdef MEKA_ZIP

// LOAD A ROM FROM A FILE -----------------------------------------------------
int             Load_ROM_File (void)
{
    FILE *      f;
    int         start_at;

    // Setting driver -----------------------------------------------------------
    // Must be done there because Load_ROM_Zip
    cur_machine.driver_id = drv_get_from_ext (file.temp);

    // Open file ----------------------------------------------------------------
    if (!(f = fopen (file.rom, "rb")))
        return (MEKA_ERR_FILE_OPEN);

    // Get file size
    fseek (f, 0, SEEK_END);
    tsms.Size_ROM = ftell (f);

    // Remove Header & Footer
    Load_Header_and_Footer_Remove (&start_at, &tsms.Size_ROM);

    // Check out if the ROM isn't actually empty
    if (tsms.Size_ROM <= 0)
        return (MEKA_ERR_FILE_EMPTY);       // File empty .. FIXME: to short ? because of header..
    fseek (f, start_at, SEEK_SET);

    // Allocate necessary memory to load ROM ------------------------------------
    if (Load_ROM_Init_Memory () == -1)
        return (MEKA_ERR_MEMORY);           // Not enough memory

    // Read data then close file ------------------------------------------------
    if (fread (ROM, 1, tsms.Size_ROM, f) != (unsigned int)tsms.Size_ROM)
        return (MEKA_ERR_FILE_READ);        // Error reading file
    fclose (f);

    // Copy data for Colecovision mirroring -------------------------------------
    if (cur_machine.driver_id == DRV_COLECO)
    {
        // FIXME
    }

    return (MEKA_ERR_OK);
}

// ALLOCATE SUFFICIENT MEMORY TO LOAD ROM -------------------------------------
static int      Load_ROM_Init_Memory (void)
{
    u8 *        p;
    int         alloc_size;

    // FIXME: The computation below are so old that I should be checking them someday. I
    // I'm sure that something wrong lies here.
    tsms.Pages_Mask_8k = 1;
    tsms.Pages_Count_8k = (tsms.Size_ROM / 0x2000);
    if (tsms.Size_ROM % 0x2000) tsms.Pages_Count_8k += 1;
    while (tsms.Pages_Mask_8k < tsms.Pages_Count_8k)
        tsms.Pages_Mask_8k *= 2;
    tsms.Pages_Mask_8k --;
    tsms.Pages_Count_8k --;
    tsms.Pages_Mask_16k = 1;
    tsms.Pages_Count_16k = (tsms.Size_ROM / 0x4000);
    if (tsms.Size_ROM % 0x4000) tsms.Pages_Count_16k += 1;
    while (tsms.Pages_Mask_16k < tsms.Pages_Count_16k)
        tsms.Pages_Mask_16k *= 2;
    tsms.Pages_Mask_16k --;
    tsms.Pages_Count_16k --;

    // Calculate allocation size to upper bound of Pages_Count_16k
    // If ROM is smaller than 48 kb, malloc 48 kb to avoid problem reading
    // data under Z80 emulation (default Sega mapper).
    alloc_size = (tsms.Pages_Mask_16k + 1) * 0x4000;
    if (alloc_size < 0xC000)
        alloc_size = 0xC000;

    // Allocate
    if (!(p = malloc (alloc_size)))
        return (-1);
    if (Game_ROM)
        free (Game_ROM);
    Game_ROM = p;

    // Fill ROM with 0xFF
    // - SG-1000 Safari Hunting : ROM is 16kb, access 48 kb memory map
    //   (actually there's a correct way to emulate that, see Cgfm2's SC-3000 
    //    documentation, in the meanwhile, filling with 0xFF allow the game to work)
    // - ROM image that are not power of 2 trying to access inexistant pages.
    //   eg: Shinobi (UE) [b2].sms
    memset (Game_ROM, 0xFF, alloc_size);

    ROM = Game_ROM;

    return (0);
}

// LOAD A ROM INTO MEMORY, RESET SYSTEM AND VARIOUS STUFF.. -------------------
int             Load_ROM_Main ()
{
    int           err;
#ifdef MEKA_ZIP
    int         zipped = NO;
#endif

    Filenames_Init_ROM ();

    // Check extension ----------------------------------------------------------
    strcpy (file.temp, file.rom);
    keepext (file.temp);
    strupr (file.temp);
    if (strcmp (file.temp, "ZIP") == 0)
    {
#ifdef MEKA_ZIP
        zipped = YES;
#else
        meka_errno = MEKA_ERR_ZIP_NOT_SUPPORTED;
        return (MEKA_ERR_ZIP_NOT_SUPPORTED); // ZIP files not supported
#endif
    }

#ifdef MEKA_ZIP
    if (zipped)
        err = Load_ROM_Zipped ();
    else
#endif
        err = Load_ROM_File ();

    // Now done in Load_ROM_xxx()
    // cur_machine.driver_id = drv_get_from_ext (file.temp);

    return (meka_errno = err);
}

void    Load_ROM_Misc (int reset)
{
    // Check for overdump
    Check_OverDump ();

    // Perform checksum and DB lookup
    Checksum_Perform (ROM, tsms.Size_ROM);

    // Automatic SMS-GG mode
    // This is because GoodTools kept putting SMS-mode GG games with a .GG extension
    // So the emulator is forced to look them up with a database ...
    if (DB_CurrentEntry && (DB_CurrentEntry->flags & DB_FLAG_SMSGG_MODE))
    {
        // Why the test? Because we want the game to disfunction "properly" with a .SG/.COL extension
        // Of course, in the future, MEKA may could force ALL driver based on DB entry.
        // But this will cause a problem for Pit Pot secret screen in SG-1000 mode (and Hang On, etc...)
        // Then this will require advanced-user selectable machine.
        if (cur_machine.driver_id == DRV_GG)
            cur_machine.driver_id = DRV_SMS;
    }

    // Initialize patching system for this ROM and apply
    Patches_ROM_Initialize ();
    Patches_ROM_Apply ();

    // Set driver
    drv_set (cur_machine.driver_id);

    // Do not system if old AND new driver is SF7000 (for disk change, this is slighty hacky)
    if (reset || cur_machine.driver_id != DRV_SF7000)
    {
        Machine_Init ();
        machine |= MACHINE_ROM_LOADED;
        Machine_Insert_Cartridge ();
        Machine_ON ();
    }

    if (cur_machine.driver_id == DRV_SF7000)
        FDC765_Disk_Insert (0, ROM, tsms.Size_ROM);

    // FIXME: do not save Backed Memory in non-verbose mode
    // Read the full comment next to BMemory_Save() in Load_ROM()
    if (Loading_UserVerbose)
        BMemory_Load ();

    // Update game boxes
    gamebox_rename_all ();

    // Miscellaenous things to apply when machine type change
    Change_System_Misc ();

    // BIOS load/unload
    // FIXME: this is a mess
    if ((Configuration.enable_BIOS) && (cur_machine.driver_id == DRV_SMS) && (sms.Country == COUNTRY_EUR_US))
        BIOS_Load ();
    else
        BIOS_Unload ();
}

//-----------------------------------------------------------------------------
