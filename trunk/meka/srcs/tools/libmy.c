//-----------------------------------------------------------------------------
// MEKA - LIBMY.C
// Various helper functions - Code
//-----------------------------------------------------------------------------
// FIXME: many of those functions are now useless, outdated or not efficient
//-----------------------------------------------------------------------------

#include "shared.h"

//-----------------------------------------------------------------------------

unsigned short *StrCpyU16(unsigned short *s1, unsigned short *s2)
{
	unsigned short  *r = s1;

	while (*s2)
		*s1++ = *s2++;
	*s1 = EOSTR;
	return (r);
}

int     StrLenU16(const unsigned short *s)
{
	int    i = 0;
	while (*s++)
		i++;
	return (i);
}

char   *StrNDup(const char *src, int n)
{
	int    n2;
	char  *ret, *dst;

	n2 = strlen (src);
	if (n2 < n)
		n = n2;
	ret = dst = (char*)malloc (sizeof (char) * (n + 1));
	while (*src && n --)
		*dst++ = *src++;
	*dst = EOSTR;
	return (ret);
}

unsigned short   *StrDupToU16(const char *src)
{
	u16* ret = (u16*)malloc (sizeof (unsigned short) * (strlen (src) + 1));
	u16* dst = ret;
	while (*src)
	{
		*dst++ = *src++;
	}
	*dst = EOSTR;
	return (ret);
}

unsigned short   *StrNDupToU16(const char *src, int n)
{
	int n2 = strlen (src);
	if (n2 < n)
		n = n2;
	u16* ret = (u16*)malloc (sizeof (unsigned short) * (n + 1));
	u16* dst = ret;
	while (*src && n --)
	{
		*dst++ = *src++;
	}
	*dst = EOSTR;
	return (ret);
}

bool	StrIsNull(const char *s)
{
	if (s == 0 || *s == EOSTR)
		return true;
	return false;
}

void	StrUpper(char *s)
{
	char c;
	while ((c = *s) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			c = c - 'a' + 'A';
			*s = c;
		}
		s++;
	}
}

void	StrLower(char *s)
{
	char c;
	while ((c = *s) != 0)
	{
		if (c >= 'A' && c <= 'Z')
		{
			c = c - 'A' + 'a';
			*s = c;
		}
		s++;
	}
}

void StrReplace (char *s, char c1, char c2)
{
	while (*s)
	{
		if (*s == c1)
			*s = c2;
		s++;
	}
}

int	Match (const char *src, const char *wildcards)
{
	int nbr = 0;
	for (int val = 0; ; val++)
	{
		if (wildcards[val] == '*')
		{
			int i = 0;
			do
			{
				nbr = nbr + Match(src + val + i, wildcards + val + 1);
				i++;
			}
			while (src[val + i - 1] != EOSTR);
			return (nbr);
		}
		else
		{
			if (wildcards[val] != src[val])
				return (0);
			if (wildcards[val] == EOSTR)
				return (1);
		}
	}
}

void    StrChomp (char *s)
{
	int   last;

	last = strlen(s) - 1;
	while (last >= 0 && (s[last] == '\n' || s[last] == '\r'))
	{
		s[last] = EOSTR;
		last -= 1;
	}
}

void    StrTrim (char *s)
{
	char * s1 = s;
	char * s2 = s;
	while (*s2 == ' ' || *s2 == '\t')
		s2++;
	if (s1 != s2)
	{
		while (*s2 != EOSTR)
			*s1++ = *s2++;
		*s1 = EOSTR;
	}
	StrTrimEnd(s);
}

void     StrTrimEnd(char *s)
{
	int i = strlen(s) - 1;
	while (i > 0 && (s[i] == ' ' || s[i] == '\t'))
	{
		s[i] = EOSTR;
		i--;
	}
}

void    StrRemoveBlanks(char *s)
{
	char *dst;

	dst = s;
	while (*s != EOSTR)
	{
		if (*s == ' ' || *s == '\t')
			s++;
		else
			*dst++ = *s++;
	}
	*dst = EOSTR;
}

void    Write_Bits_Field (int v, int n_bits, char *field)
{
	char  *p = field + n_bits;

	*p-- = EOSTR;
	while (n_bits-- > 0)
	{
		*p-- = (v & 0x01) ? '1' : '0';
		v >>= 1;
	}
}

void    Random_Init (void)
{
	srand ((unsigned int)time (NULL));
#ifndef ARCH_WIN32
	srandom (time (NULL));
#endif
}

int		Random(int max)
{
#ifndef ARCH_WIN32
	return random() % max;
#else
	return rand() % max;
#endif
}

float	RandomFloat(float max)
{
	return max * ((float)Random(65535) / 65535.0f);
}

float	RandomFloat(float min, float max)
{
	return min + (max - min) * ((float)Random(65535) / 65535.0f);
}

// Convert a BCD number to decimal
// Note: no error handling is done, if using A-F values
int     BCD_to_Dec(int bcd)
{
	int    ret;
	int    pow;

	ret = 0;
	pow = 1;
	while (bcd > 0)
	{
		ret += (bcd & 0xF) * pow;
		bcd >>= 4;
		pow *= 10;
	}

	return (ret);
}
