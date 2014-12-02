//
// Meka - LIBMY.H
// Various functions - Headers and Constants
//

#define EOSTR                   (0)

unsigned short* StrCpyU16		(unsigned short *s1, unsigned short *s2);
unsigned short* StrDupToU16		(const char *src);
unsigned short* StrNDupToU16	(const char *src, int n);
int             StrLenU16		(const unsigned short *s);

char*	StrNDup                 (const char *src, int n);
bool	StrIsNull               (const char *s);
void    StrReplace              (char *s, char c1, char c2);
void	StrUpper				(char *s);
void	StrLower				(char *s);

int	    StrMatch                (const char *src, const char *wildcards);
void    StrChomp                (char *s);
void    StrTrim                 (char *s);
void    StrTrimEnd              (char *s);
void    StrRemoveBlanks         (char *s);

void    Write_Bits_Field        (int v, int n_bits, char* out_field);

