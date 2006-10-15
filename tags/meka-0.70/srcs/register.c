//-----------------------------------------------------------------------------
// MEKA - register.c
// Registration - Code
//-----------------------------------------------------------------------------
// This is obsolete and should be removed. As of 2005/04/03, I removed all
// key-checking code and made registration an obsolete concept.
// I left this so that user name is still parsed and used in some places,
// but it will be totally removed someday.
//-----------------------------------------------------------------------------

#if 0 // Obsolete. Registration is no more.

#include "shared.h"
#include "register.h"

//-----------------------------------------------------------------------------

void            Register_Init (void)
{
    int           i, j;
    long          t_key;

    t_tfile *     tf;
    t_list *      lines;
    char *        line;
    int           line_cnt;
    int           len;

    registered.is = NO;
    registered.user_name = NULL;
    registered.user_name_only = NULL;
    registered.user_sentence = NULL;
    registered.user_key = 0;

    // Open and read MEKA.KEY/.REG file if present
    if ((tf = tfile_read (registered.filename[0])) == NULL)
        if ((tf = tfile_read (registered.filename[1])) == NULL)
            return;

    // Parse each line
    line_cnt = 0;
    for (lines = tf->data_lines; lines; lines = lines->next)
    {
        if (line_cnt >= 3)
            break;
        line_cnt += 1;
        line = lines->elem;

        if (StrNull (line))
        {
            continue;
        }
        switch (line_cnt)
        {
        case 1: registered.user_name = strdup (line);
            break;
        case 2: registered.user_sentence = strdup (line);
            break;
        case 3: registered.user_key = atoi (line);
            break;
        }
    }

    tfile_free (tf);

    if ((strlen (registered.user_name) < 10) || (strlen (registered.user_sentence) < 10))
        return;

    // Compute user_name_only
    {
        char *p;
        registered.user_name_only = strdup (registered.user_name);
        p = strchr (registered.user_name_only, '/');
        if (p && p > registered.user_name_only)
            *(p - 1) = EOSTR;
    }

    // Calculate unlock key
    // ***
    // <censored>
    // ***

    // Compare keys and set registration flag
    if (registered.user_key == t_key)
    {
        registered.is = YES;
        ConsolePrintf("This version is registered to: %s\n", registered.user_name_only);
    }
}

//-----------------------------------------------------------------------------

#endif
