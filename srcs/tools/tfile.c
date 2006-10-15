//
// Meka - TFILE.C
// Text File handling library (pseudo class)
//

#include "shared.h"
#include "libmy.h"
#include "liblist.h"

t_tfile         *tfile_read(char *filename)
{
 t_tfile        *tf;
 FILE           *f;
 int            size;
 char           *p_cur, *p_new;

 // Open and file and try getting its size
 if ((f = fopen(filename, "rb")) == NULL)
    { meka_errno = MEKA_ERR_FILE_OPEN; return NULL; }
 if (fseek(f, 0, SEEK_END) != 0 || (size = ftell(f)) == -1 || fseek(f, 0, SEEK_SET) != 0)
    { meka_errno = MEKA_ERR_FILE_READ; return NULL; }

 // Allocate the t_tfile and read file data into to
 tf = malloc(sizeof (t_tfile));
 tf->size = size;
 tf->data_raw = malloc(sizeof (char) * size + 1);

 if (fread(tf->data_raw, sizeof (char), size, f) < (unsigned int)size)
    { meka_errno = MEKA_ERR_FILE_READ; return NULL; }
 tf->data_raw[size] = EOSTR;
 fclose(f);

 // Parse raw data to create the lines list
 tf->data_lines = NULL;
 p_cur = tf->data_raw;
 while ((p_new = strchr(p_cur, '\n')) != NULL)
    {
    *p_new = EOSTR;
    Chomp(p_cur);
    list_add(&tf->data_lines, p_cur);
    p_cur = p_new + 1;
    }

 // Handle last line case
 if (p_cur < tf->data_raw + tf->size)
    {
    list_add(&tf->data_lines, p_cur);
    }

 // Reverse list (as elements were added to the beginning each time)
 list_reverse(&tf->data_lines);

 // Ok
 meka_errno = MEKA_ERR_OK;
 return (tf);
}

void            tfile_free(t_tfile *tf)
{
 list_free_no_elem(&tf->data_lines);
 free(tf->data_raw);
 free(tf);
}

