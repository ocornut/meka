//-----------------------------------------------------------------------------
// Parsing helper functions
// by Omar Cornut in 2000-2004
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libmy.h"
#include "libparse.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// trim_trailing_spaces (char *s)
// Trim trailing spaces by writing an end-of-string marker
//-----------------------------------------------------------------------------
void        trim_trailing_spaces (char *s)
{
    int     i;
    i = strlen(s) - 1;
    while (i >= 0 && isspace((unsigned char)s[i])) // u8 needed because of accentued characters in MEKA.MSG and isspace asserting
        s[i--] = EOSTR;
}

//-----------------------------------------------------------------------------
// skip_spaces (char **src)
// Skip all spaces in pointed string
//-----------------------------------------------------------------------------
void        skip_spaces (char **src)
{
    char    c;
    while ((c = **src) == ' ' || c == '\t')
        (*src) ++;
}

//-----------------------------------------------------------------------------
// parse_getword(char *dst, int dst_len, char **src, char *separators)
//-----------------------------------------------------------------------------
// Retrieve next word, as delimited by the given set of separators,
// into given location, with maximum length, starting at given pointed string.
// Increase pointed string source.
// If no location is given, automatically allocate result.
//-----------------------------------------------------------------------------
// Note: In case of repeated separators (eg: ",,,,", it doesn't return NULL 
// but empty strings. NULL is returned at end of source pointed string.
//-----------------------------------------------------------------------------
char *      parse_getword(char *dst, int dst_len, char **src, char *separators, char comment_char, t_parse_flags flags)
{
    char *  p;
    int     inhibit;
    char    c;
    char *  dst_write;

    // If dst is NULL, we have to allocate ourself
    if (dst == NULL)
    {
        dst_len = 0;
        inhibit = NO;
        for (p = *src; (c = *p) != EOSTR; p++)
        {
            if (!inhibit)
            {
                if (strchr (separators, c))
                    break;
                if (c == comment_char)
                    break;
                if (c == '\\')
                {
                    inhibit = YES;
                    continue;
                }
            }
            dst_len++;
            inhibit = NO;
        }
        if (dst_len == 0 && (*p == EOSTR || *p == comment_char))
            return (NULL);
        dst_len++; // for \0 storage
        dst = malloc(sizeof (char) * dst_len);
    }

    // Copy word until separator or end-of-string is found. Handle \ inhibitor.
    dst_write = dst;
    inhibit = NO;
    for (p = *src; (c = *p) != EOSTR; p++)
    {
        if (!inhibit)
        {
            if (strchr (separators, c))
                break;
            if (c == comment_char)
                break;
            if (c == '\\')
            {
                inhibit = YES;
                continue;
            }
        }
        if (dst_write - dst == dst_len)
            break;
        *dst_write++ = c;
        inhibit = NO;
    }
    *dst_write = EOSTR;

    // Set new source pointer
    if (flags & PARSE_FLAGS_DONT_EAT_SEPARATORS)
        *src = p;
    else
        *src = (*p == EOSTR || *p == comment_char) ? p : p + 1;

    // Return NULL at end of string
    if (dst_write - dst == 0 && (*p == EOSTR || *p == comment_char))
        return (NULL);

    // Trim trailing spaces
    trim_trailing_spaces(dst);

    return (dst);
}

//-----------------------------------------------------------------------------
// OLD/OBSOLETE
//-----------------------------------------------------------------------------

static char *   Parse_Separators = " \t"; // Default value

char*   Parse_GetSep (void)
{
 return Parse_Separators;
}

void    Parse_SetSep (char *Sep)
{
 Parse_Separators = Sep;
}

void    Parse_SkipSpaces (char **Src)
{
 while (*(*Src) == ' ' || *(*Src) == '\t')
   (*Src) += 1;
}

int     Parse_WordGet (t_word *Word, char **Src)
{
 int    i;
 while (*(*Src) && strchr (Parse_Separators, *(*Src)))
   (*Src) += 1;
 for (i = 0; (*Src)[i] && !strchr (Parse_Separators, (*Src)[i]); i++);
 Word->len = i;
 //if (i == 0)
 //   return 0;
 strncpy (Word->s, *Src, i);
 Word->s [i] = EOSTR;
 (*Src) += i;
 //Word->s = *Src; // StrNDup (*Src, i);
 //(*Src) += i;
 //(*Src)[0] = 0;
 //(*Src) += 1;
 return i;
}

