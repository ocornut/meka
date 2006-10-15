//
// Meka - TFILE.H
// Text File handling library (pseudo class)
//

// Structure

typedef struct
{
  int           size;
  char          *data_raw;
  t_list        *data_lines;
}               t_tfile;

// Methods

t_tfile         *tfile_read(char *filename);
void            tfile_free(t_tfile *tf);

