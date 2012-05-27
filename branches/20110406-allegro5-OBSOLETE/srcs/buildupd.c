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
// #ifndef ARCH_UNIX
// #undef ARCH_DOS
// #undef ARCH_WIN32
// #define ARCH_UNIX
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
    fprintf (f, "char MEKA_BUILD_DATE[] = \"%04d/%02d/%02d\";\n", year, month, day);
    fprintf (f, "char MEKA_BUILD_TIME[] = \"%02i:%02i:%02i\";\n", hour, minute, second);

    // Close file
    fclose (f);

    return (0);
}

//-----------------------------------------------------------------------------
