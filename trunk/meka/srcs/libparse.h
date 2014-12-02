//-----------------------------------------------------------------------------
// Parsing helper functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

#define PARSE_ESCAPE_CHARACTERS_DEFAULT     "\"\\;" // " \ ;

enum t_parse_flags
{
    PARSE_FLAGS_NONE                    = 0,
    PARSE_FLAGS_DONT_EAT_SEPARATORS     = 0x0001,
    //PARSE_FLAGS_STRING_INHIBIT        = 0x0002    // FIXME: TODO. "" enclosed string is inhibited
};

// Text file
struct t_tfile
{
	int           size;
	char *        data_raw;
	t_list *      data_lines;
	int           data_lines_count;
};

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

t_tfile *   tfile_read(const char *filename);
void        tfile_free(t_tfile *tf);

void        parse_trim_trailing_spaces  (char *s);
void        parse_skip_spaces           (char **src);
char *      parse_getword               (char *dst, int dst_len, char **src, const char *separators, char comment_char, t_parse_flags flags = PARSE_FLAGS_NONE);

char *      parse_escape_string         (const char *src, const char *escape_chars);
char *      parse_unescape_string       (const char *src, const char *escape_chars);

//-----------------------------------------------------------------------------
