//-----------------------------------------------------------------------------
// Parsing helper functions
// by Omar Cornut in 2000-2004
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        trim_trailing_spaces    (char *s);
void        skip_spaces             (char **src);

char *      parse_getword           (char *dst, int dst_len, char **src, char *separators, char comment_char);

//-----------------------------------------------------------------------------
// OLD/OBSOLETE
//-----------------------------------------------------------------------------

// NOTE: Separators are undefined on start and must be set
// before any use of the Parse_WordGet function.

typedef struct  s_word
{
  char  s[128]; /* FIXME: that sucks */
  int   len;
}               t_word;

char*   Parse_GetSep (void);
void    Parse_SetSep (char *s);
int     Parse_WordGet (t_word *Word, char **Src);
void    Parse_SkipSpaces (char **Src);

//-----------------------------------------------------------------------------
