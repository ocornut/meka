//-----------------------------------------------------------------------------
// MEKA - tfile.c
// Text file reading functions
//-----------------------------------------------------------------------------

#include "shared.h"
#include "libmy.h"
#include "liblist.h"
#include "tfile.h"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

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
        Chomp(p_cur);
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
