//-----------------------------------------------------------------------------
// MEKA - buildupd.c
// Update build date and time and store in "build.c" file.
//-----------------------------------------------------------------------------
// Output may looks like:
//      char MEKA_BUILD_DATE[] = "14 April 2000";
//      char MEKA_BUILD_TIME[] = "21:53:31";
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

// FIXME: Must be a reason for this related to the include below. Check it out.
// #ifndef UNIX
// #undef DOS
// #undef WIN32
// #define UNIX
// #endif

#include "tools_t.c"

//-----------------------------------------------------------------------------

int     main (int argc, char **argv)
{
    FILE *  f;
    int	    hour, minute, second, day, month, year, day_of_week;
    const char *filename = (argc > 1) ? argv[1] : "build.c";

    // Get time
    meka_get_time_date(&hour, &minute, &second, &day, &month, &year, &day_of_week);

    // Open "build.c" for output
    f = fopen(filename, "wt");
    if (f == NULL)
    {
        printf ("Error creating/updating build.c\n");
        return (1);
    }

    // Output strings
#ifdef DOS
    fprintf (f, "char MEKA_BUILD_SYSTEM[] = \"DOS\";\n");
#endif
#ifdef WIN32
    fprintf (f, "char MEKA_BUILD_SYSTEM[] = \"Win32\";\n");
#endif
#ifdef UNIX
    fprintf (f, "char MEKA_BUILD_SYSTEM[] = \"Un*x\";\n");
#endif
    fprintf (f, "char MEKA_BUILD_DATE[] = \"%04d/%02d/%02d\";\n", year, month, day);
    fprintf (f, "char MEKA_BUILD_TIME[] = \"%02i:%02i:%02i\";\n", hour, minute, second);

    // Close file
    fclose (f);

    return (0);
}

//-----------------------------------------------------------------------------
