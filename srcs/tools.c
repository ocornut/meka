//-----------------------------------------------------------------------------
// MEKA - tools.c
// Various tools - Code
//-----------------------------------------------------------------------------
// Note: obsolete ?
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

// SEPARATE FIRST WORD AND THE REST OF A STRING -------------------------------
// FIXME: Obsolete. Use parse_getword()
void    Get_First_Word (char *str, char *wrd, char separator)
{
 char   *p;

 p = strchr (str, separator);
 if (p == NULL)
    {
    strcpy (wrd, str);
    str [0] = EOSTR;
    }
 else
    {
    *p = EOSTR;
    strcpy (wrd, str);
    strcpy (str, p + 1);
    }
}

/*
{
 int    i, j;
 char   s1 [256];

 for (i = 0, j = 0; str [i] && str [i] != separator; i ++)
     s1 [j ++] = str [i];
 s1 [j] = EOSTR;
 strcpy (wrd, s1);
 if (str [i] == EOSTR)
    {
    str [0] = EOSTR;
    }
 else
    {
    i++;
    for (i, j = 0; str [i]; i ++)
       s1 [j ++] = str [i];
    s1 [j] = EOSTR;
    strcpy (str, s1);
    }
}
*/

// KILL EXTENSION FROM A FILENAME ---------------------------------------------
// FIXME: move to libmy
void    killext (char *org)
{
 char  *p = strrchr (org, '.');
 if (p != NULL) *p = EOSTR;
}

// KEEP EXTENSION FROM A FILENAME ---------------------------------------------
// FIXME: move to libmy
void    keepext (char *org)
{
 char   s [FILENAME_LEN];
 char  *p = strrchr (org, '.');

 if (p == NULL) return;
 strcpy (s, p + 1);
 strcpy (org, s);
}

// KILL PATH FROM A FILENAME --------------------------------------------------
// FIXME: move to libmy
void    killpath (char *org)
{
 char   *p;
 char   str1 [FILENAME_LEN];

 if ((p = strrchr (org, '/')))
    {
    p ++;
    strcpy (str1, p);
    strcpy (org, str1);
    }
#ifndef UNIX
 else
 if ((p = strrchr (org, '\\')))
    {
    p ++;
    strcpy (str1, p);
    strcpy (org, str1);
    }
 #endif
}

// Extract filename from 'src' and copy it to location 'dst'
void    StrCpyPathRemoved(char *dst, char *src)
{
  char *p = strrchr(src, '/');

  #ifndef UNIX
  char *p2 = strrchr(src, '\\');
  if (p2 != NULL && p2 > p)
     p = p2;
  #endif

  p = (p ? p + 1 : src);
  strcpy (dst, p);
}

//-----------------------------------------------------------------------------

