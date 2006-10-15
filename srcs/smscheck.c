// FIXME: This has nothing to do ni MEKA. 
// Move elsewhere (some rewrite/cleaning would be good also, but
// it's less useful now with Maxim's header reader)

//
// SMS Checker (c) Omar Cornut (Bock) 2000, 2001
// Sega 8-bit ROM tool, showing checksum and various informations
//

#define PROG_NAME       "SMS Checker"
#define AUTHOR          "Omar Cornut (Bock)"
#define VERSION         "0.2b"
#define WEB_PAGE        "http://www.smspower.org"

//-- EXTERNAL STUFF ---------------------------------------------------------
// Notice: This program uses some low level functions that arent mine (but
//   from the usual C Library), which are:
//
//   opendir(), readdir(), closedir()
//   malloc(), free()
//   open(), read(), close()
//
#ifdef DOS
 #include <io.h>
#endif
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//---------------------------------------------------------------------------

#include "libmy.h"
#include "liblist.h"

typedef unsigned char   byte;
typedef unsigned short  word;

typedef struct
{
  char  Sega_ID[8];
  word  Unknown_W1; // Protocol ?
  word  Checksum;
  word  PartNo;     // Product Number
  byte  Version;
  byte  System_and_Size;
} t_smsheader;

// Systems:
//  0x30  Master System Japan
//  0x40  Master System Export
//  0x50  Game Gear Japan
//  0x60  Game Gear International
//  0x70  Game Gear Export

// Sizes: (used for checksum)
//  0x0A  8k
//  0x0B  16k
//  0x0C  32k
//  0x0D  48k
//  0x0E  64k
//  0x0F  128k
//  0x00  256k

void    smschecker_process_file (char *FileName)
{
  int           i;
  int           fd;
  int           filesize;
  t_smsheader   smsheader;
  int           checksum_len;
  byte         *filebuf;

  printf ("\n[%s]\n", FileName);
#ifdef DOS
  if ((fd = open (FileName, O_RDONLY | O_BINARY)) == -1)
#else
  if ((fd = open (FileName, O_RDONLY)) == -1)
#endif
     {
     printf ("Error opening file!\n");
     return;
     }

  lseek (fd, 0, SEEK_END);
#ifdef DOS
  filesize = tell (fd);
#else
  filesize = lseek (fd, 0, SEEK_CUR);
#endif

  printf ("Size ...... %i\n", filesize);
  if (filesize < 0x8000)
     {
     printf ("File is too short to contains an header!\n");
     close (fd);
     return;
     }

  lseek (fd, 0x8000 - sizeof (smsheader), SEEK_SET);
  if (read (fd, &smsheader, sizeof (smsheader)) != sizeof (smsheader))
     {
     printf ("Unable to read header!\n");
     close (fd);
     return;
     }

  // HEADER -------------------------------------------------------------------
  {
  printf ("Header ....");
  for (i = 0; i < sizeof (smsheader); i++)
      printf (" %02X", ((byte *)(&smsheader))[i]);
  printf ("\n");
  }

  // SYSTEM -------------------------------------------------------------------
  {
  byte System = (smsheader.System_and_Size & 0xF0) >> 4;
  printf ("System .... ");
  switch (System)
    {
    case 3:  printf ("SMS Japan");        break;
    case 4:  printf ("SMS Export");       break;
    case 5:  printf ("GG Japan");         break;
    case 6:  printf ("GG International"); break;
    case 7:  printf ("GG Export");        break;
    default: printf ("Unknown");          break;
    }
  printf (" (%i)\n", System);
  }

  // SIZE ---------------------------------------------------------------------
  {
  byte Size = (smsheader.System_and_Size & 0x0F);
  printf ("Size ...... ");
  switch (Size)
    {
    case 0x0A: checksum_len = 0x02000;  printf ("8k");      break;
    case 0x0B: checksum_len = 0x04000;  printf ("16k");     break;
    case 0x0C: checksum_len = 0x08000;  printf ("32k");     break;
    case 0x0D: checksum_len = 0x0C000;  printf ("48k");     break;
    case 0x0E: checksum_len = 0x10000;  printf ("64k");     break;
    case 0x0F: checksum_len = 0x20000;  printf ("128k");    break;
    case 0x00: checksum_len = 0x40000;  printf ("256k+");   break;
    default:   checksum_len = filesize; printf ("Unknown"); break;
    }
  printf (" \n");
  }

  // CHECKSUM -----------------------------------------------------------------
  {
  word Checksum;

  printf ("CheckSum .. ");
  filebuf = malloc(checksum_len);
  lseek (fd, 0, SEEK_SET);
  if (read (fd, filebuf, checksum_len) != checksum_len)
    {
    printf ("Unable to read enough file data!\n");
    close (fd);
    return;
    }
  // Compute checksum
  Checksum = 0;
  for (i = 0; i < checksum_len; i++)
    Checksum += filebuf [i];
  // Remove header from checksum
  if (checksum_len >= 0x8000)
     for (i = 0x8000 - sizeof (smsheader); i < 0x8000; i++)
        Checksum -= filebuf [i];
  // Mask highter bits
  Checksum &= 0xFFFF;
  // Print out result
  printf ("%04X, ", Checksum);
  if (smsheader.Checksum == Checksum)
     printf ("Ok\n");
  else
     printf ("Bad. Should be %04X\n", smsheader.Checksum);
  }

  // MAPPER -------------------------------------------------------------------
  {
  int c8000 = 0;
  int cFFFF = 0;

  // Search for mapper accessing code
  for (i = 0; i < 0x8000; i++)
     if (filebuf [i] == 0x32) // Z80 Opcode: LD (xxxx), A
        {
        if (filebuf[i + 1] == 0x00 && filebuf[i + 2] == 0x80)
           { i += 2; c8000++; }
        else
        if (filebuf[i + 1] == 0xFF && filebuf[i + 2] == 0xFF)
           { i += 2; cFFFF++; }
        }

  // Print out result
  printf ("Mapper .... %02d/%02d -> %s", c8000, cFFFF, (c8000 > cFFFF) ? "CodeMasters" : "Normal");
  }

  free (filebuf);
  close (fd);
}

void    smschecker_process_files (char *FileMask)
{
  char           *Path, *Files, *p;
  DIR            *Dir;
  struct dirent  *DirEnt;
  t_list         *FileList;

  FileMask = strdup (FileMask);

  #ifndef UNIX
    StrReplace (FileMask, '\\', '/');
  #endif

  if ((p = StrRChr (FileMask, '/')))
     {
     Path = FileMask;
     Files = p + 1;
     *p = EOSTR;
     }
  else
     {
     Path = ".";
     Files = FileMask;
     }

  if (!(Dir = opendir(Path)))
     {
     free (FileMask);
     return;
     }

  FileList = 0;
  while ((DirEnt = readdir(Dir)))
     {
     #ifdef UNIX
       if (Match (DirEnt->d_name, Files) > 0)
     #else
       if (MatchNCase (DirEnt->d_name, Files) > 0)
     #endif
          {
          int   i;
          char  *FileName;
          FileName = StrCreate (strlen(Path) + strlen(DirEnt->d_name) + 1);
          // I hate this kind of code. Where is my StrXCat function??? :(
          strcpy (FileName, Path);
          i = strlen(Path);
          FileName [i] = '/';
          strcpy (FileName + i + 1, DirEnt->d_name);
          list_add (&FileList, FileName);
          }
     }
  if (FileList)
     {
     list_sort (&FileList, (int (*)(void *, void *))strcmp);
     list_apply (FileList, (void (*)(void *))smschecker_process_file);
     list_free (&FileList);
     }
  else
     {
     printf ("Sorry, but no files matches with \"%s\".\n", FileMask);
     }

  closedir (Dir);
  free (FileMask);
}

int     main (int ArgC, char **ArgV)
{
  int   i;

  printf (PROG_NAME " " VERSION " - by " AUTHOR " in 2000\n" WEB_PAGE "\n--\n");
  if (ArgC < 2)
     {
     printf ("Syntax: %s [romfile] [romfile] ..\n", ArgV[0]);
     printf ("        (absolute/relative paths and wildcards allowed)\n");
     return 0;
     }
  for (i = 1; i < ArgC; i++)
      smschecker_process_files (ArgV[i]);
  return 0;
}

