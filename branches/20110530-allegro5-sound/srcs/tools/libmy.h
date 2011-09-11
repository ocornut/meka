//
// Meka - LIBMY.H
// Various functions - Headers and Constants
//

#define EOSTR                   (0)

#define OK                      (0)
#define ERR                     (1)

int		Random(int max);
float	RandomFloat(float max = 1.0f);
float	RandomFloat(float min, float max);

char   *StrNDup                 (const char *src, int n);

unsigned short *StrCpyU16		(unsigned short *s1, unsigned short *s2);
unsigned short *StrDupToU16		(const char *src);
unsigned short *StrNDupToU16	(const char *src, int n);
int             StrLenU16		(const unsigned short *s);

int     StrNull                 (char *s);
void    StrReplace              (char *s, char c1, char c2);
void	StrUpper				(char *s);
void	StrLower				(char *s);
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

