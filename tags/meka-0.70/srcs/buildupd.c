//-----------------------------------------------------------------------------
// MEKA - buildupd.c
// Update build date and time and store in BUILD.C - Code
//-----------------------------------------------------------------------------
// Output may looks like:
//      char MEKA_BUILD_DATE[] = "14 April 2000";
//      char MEKA_BUILD_TIME[] = "21:53:31";
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
// FIXME
#ifndef UNIX
#undef DOS
#undef WIN32
#define UNIX
#endif

#include "tools_t.c"

//-----------------------------------------------------------------------------

int     main (int ac, char **av)
{
  FILE *f;
  int    hour, minute, second, day, month, year, day_of_week;

  f = fopen("build.c", "wt");
  if (f == NULL)
     {
     printf ("Error creating/updating build.c\n");
     exit (1);
     }

  meka_get_time_date(&hour, &minute, &second, &day, &month, &year, &day_of_week);
  fprintf (f, "char MEKA_BUILD_DATE[] = \"%d %s %d\";\n", day, month_name_table[month], year);
  fprintf (f, "char MEKA_BUILD_TIME[] = \"%02i:%02i:%02i\";\n", hour, minute, second);
  fclose (f);
  return (0);
}

//-----------------------------------------------------------------------------

