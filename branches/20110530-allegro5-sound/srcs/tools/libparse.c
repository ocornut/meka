//-----------------------------------------------------------------------------
// Parsing helper functions
// by Omar Cornut in 2000-2004
//-----------------------------------------------------------------------------

#include "shared.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libparse.h"

#ifndef FALSE
#define FALSE   0
#endif
#ifndef  TRUE
#define TRUE    1
#endif

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// parse_trim_trailing_spaces(char *s)
// Trim trailing spaces by writing an end-of-string marker
//-----------------------------------------------------------------------------
void        parse_trim_trailing_spaces (char *s)
{
    int     i;
    i = strlen(s) - 1;
    while (i >= 0 && isspace((unsigned char)s[i])) // u8 needed because of accentued characters in MEKA.MSG and isspace asserting
        s[i--] = EOSTR;
}

//-----------------------------------------------------------------------------
// parse_skip_spaces(char **src)
// Skip all spaces in pointed string
//-----------------------------------------------------------------------------
void        parse_skip_spaces(char **src)
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
        inhibit = FALSE;
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
                    inhibit = TRUE;
                    continue;
                }
            }
            dst_len++;
            inhibit = FALSE;
        }
        if (dst_len == 0 && (*p == EOSTR || *p == comment_char))
            return (NULL);
        dst_len++; // for \0 storage
        dst = (char*)malloc(sizeof (char) * dst_len);
    }

    // Copy word until separator or end-of-string is found. Handle \ inhibitor.
    dst_write = dst;
    inhibit = FALSE;
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
                inhibit = TRUE;
                continue;
            }
        }
        if (dst_write - dst == dst_len)
            break;
        if (inhibit)
        {
            if (c == 'n')
                c = '\n';
            else if (c == 't')
                c = '\t';
        }
        *dst_write++ = c;
        inhibit = FALSE;
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
    parse_trim_trailing_spaces(dst);

    return (dst);
}

char *      parse_escape_string(const char *src, const char *escape_chars)
{
    char *  p;
    char *  dst;
    char    c;
    int     count;
    int     src_length;

    // Default escape characters
    if (escape_chars == NULL)
        escape_chars = PARSE_ESCAPE_CHARACTERS_DEFAULT;

    // Count characters to escape
    count = 0;
    for (src_length = 0; (c = src[src_length]) != '\0'; src_length++)
    {
        if (strchr(escape_chars, c) != NULL)
            count++;
    }

    // If none, return NULL
    if (count == 0)
        return NULL;

    dst = (char*)malloc(sizeof(char) * (src_length + count + 1));
    p = dst;
    while ((c = *src++) != '\0')
    {
        if (strchr(escape_chars, c) != NULL)
        {
            *p++ = '\\';
            //count--;
        }
        *p++ = c;
    }
    *p = '\0';

    return (dst);
}

char *      parse_unescape_string(const char *src, const char *escape_chars)
{
    char *  dst;
    char *  p;
    char    c;
    int     src_length;

    // Default escape characters
    if (escape_chars == NULL)
        escape_chars = PARSE_ESCAPE_CHARACTERS_DEFAULT;

    // Make it easier, allocate same as source length
    src_length = strlen(src);
    dst = (char*)malloc(sizeof(char) * (src_length + 1));
    p = dst;

    // Copy
    while ((c = *src++) != '\0')
    {
        if (strchr(escape_chars, c) != NULL)
        {
            c = *src++;
            if (c == '\0')  // FIXME: not exact behavior, but at least we're safe here
                break;
        }
        *p++ = c;
    }
    *p = '\0';

    return (dst);
}

//-----------------------------------------------------------------------------

