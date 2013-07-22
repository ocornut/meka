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

void    Random_Init             (void);

char   *StrNDup                 (const char *src, int n);

unsigned short *StrCpyU16		(unsigned short *s1, unsigned short *s2);
unsigned short *StrDupToU16		(const char *src);
unsigned short *StrNDupToU16	(const char *src, int n);
int             StrLenU16		(const unsigned short *s);

bool	StrIsNull               (const char *s);
void    StrReplace              (char *s, char c1, char c2);
void	StrUpper				(char *s);
void	StrLower				(char *s);

int	    Match                   (const char *src, const char *wildcards);

void    StrChomp                (char *s);
void    StrTrim                 (char *s);
void    StrTrimEnd              (char *s);
void    StrRemoveBlanks         (char *s);

void    Write_Bits_Field        (int v, int n_bits, char* out_field);
int     BCD_to_Dec              (int bcd);

