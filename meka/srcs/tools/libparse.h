//-----------------------------------------------------------------------------
// Parsing helper functions
// by Omar Cornut in 2000-2004
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#define PARSE_ESCAPE_CHARACTERS_DEFAULT     "\"\\;" // " \ ;

typedef enum
{
    PARSE_FLAGS_NONE                    = 0,
    PARSE_FLAGS_DONT_EAT_SEPARATORS     = 0x0001,
    //PARSE_FLAGS_STRING_INHIBIT        = 0x0002    // FIXME: TODO. "" enclosed string is inhibited
} t_parse_flags;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void        parse_trim_trailing_spaces  (char *s);
void        parse_skip_spaces           (char **src);
char *      parse_getword               (char *dst, int dst_len, char **src, char *separators, char comment_char, t_parse_flags flags);

char *      parse_escape_string         (const char *src, const char *escape_chars);
char *      parse_unescape_string       (const char *src, const char *escape_chars);

//-----------------------------------------------------------------------------
