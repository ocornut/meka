//
// Meka - LIBMY.H
// Various functions - Headers and Constants
//

#define EOSTR                   (0)

#define OK                      (0)
#define ERR                     (1)

#define Maj(c)                  ((c >= 'a' && c <= 'z') ? (c + 'A' - 'a') : (c))
#define Min(c)                  ((c >= 'A' && c <= 'Z') ? (c + 'a' - 'A') : (c))
#define Limit(a,b)              (((b) > (a)) ? (a) : (b))

#ifndef WIN32
  #define Random(a)             (random() % (a))
#else
  #define Random(a)             (rand() % (a))
#endif

char   *StrNDup                 (const char *src, int n);

unsigned short *StrCpyUnicode   (unsigned short *s1, unsigned short *s2);
unsigned short *StrDupToUnicode (const char *src);
unsigned short *StrNDupToUnicode(const char *src, int n);
int             StrLenUnicode   (const unsigned short *s);

int     StrNull                 (char *s);
void    StrReplace              (char *s, char c1, char c2);
int     GetNbrHex               (const char *s);

int     Power                   (int base, int power);

int	    Match                   (const char *src, const char *wildcards);

void    Chomp                   (char *s);
void    Trim                    (char *s);
void    Trim_End                (char *s);
void    Remove_Spaces           (char *s);
void    Replace_Backslash_N     (char *s);
void    Write_Bits_Field        (int v, int n_bits, char *field);
void    Random_Init             (void);
int     BCD_to_Dec              (int bcd);


