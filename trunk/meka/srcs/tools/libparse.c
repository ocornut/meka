//-----------------------------------------------------------------------------
// Parsing helper functions
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
// Retrieve next word, as delimited by the given set of separators,
// into given location, with maximum length, starting at given pointed string.
// Increase pointed string source.
// If no location is given, automatically allocate result.
//-----------------------------------------------------------------------------
// Note: In case of repeated separators (eg: ",,,,", it doesn't return NULL 
// but empty strings. NULL is returned at end of source pointed string.
//-----------------------------------------------------------------------------
char *      parse_getword(char *dst, int dst_len, char **src, const char *separators, char comment_char, t_parse_flags flags)
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

t_tfile *       tfile_read(const char *filename)
{
	// Open and file
	FILE* f;
	if ((f = fopen(filename, "rb")) == NULL)
	{ 
		meka_errno = MEKA_ERR_FILE_OPEN; 
		return NULL; 
	}

	// Gets its size
	int size;
	if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) == -1 || fseek(f, 0, SEEK_SET) != 0)
	{ 
		meka_errno = MEKA_ERR_FILE_READ; 
		return NULL; 
	}

	// Allocate the t_tfile and read file data into to
	t_tfile* tf = (t_tfile*)malloc(sizeof (t_tfile));
	tf->size = size;
	tf->data_raw = (char*)malloc(sizeof (char) * size + 1);
	tf->data_lines = NULL;

	if (fread(tf->data_raw, sizeof (char), size, f) < (unsigned int)size)
	{ 
		meka_errno = MEKA_ERR_FILE_READ; 
		tfile_free(tf);
		return NULL; 
	}
	tf->data_raw[size] = EOSTR;
	fclose(f);

	// Silently ignore UTF-8 header (for meka.nam)
	char* p_cur = tf->data_raw;
	if ((u8)p_cur[0] == 0xEF && (u8)p_cur[1] == 0xBB && (u8)p_cur[2] == 0xBF)
		p_cur += 3;

	// Parse raw data to create the lines list
	int lines_count = 0;
	char* p_new;
	while ((p_new = strchr(p_cur, '\n')) != NULL)
	{
		*p_new = EOSTR;
		StrChomp(p_cur);
		list_add_to_end(&tf->data_lines, p_cur);
		lines_count++;
		p_cur = p_new + 1;
	}

	// Handle last line case
	if (p_cur < tf->data_raw + tf->size)
	{
		list_add_to_end(&tf->data_lines, p_cur);
		lines_count++;
	}
	tf->data_lines_count = lines_count;

	// OK
	meka_errno = MEKA_ERR_OK;
	return (tf);
}

void            tfile_free(t_tfile *tf)
{
	list_free_no_elem(&tf->data_lines);
	free(tf->data_raw);
	free(tf);
}

//-----------------------------------------------------------------------------

