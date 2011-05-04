//-----------------------------------------------------------------------------
// MEKA - LIBMY.C
// Various helper functions - Code
//-----------------------------------------------------------------------------
// FIXME: many of those functions are now useless, outdated or not efficient
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "libmy.h"

//-----------------------------------------------------------------------------

unsigned short *StrCpyUnicode (unsigned short *s1, unsigned short *s2)
{
 unsigned short  *r = s1;

 while (*s2)
   *s1++ = *s2++;
 *s1 = EOSTR;
 return (r);
}

int     StrLenUnicode (const unsigned short *s)
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
 ret = dst = malloc (sizeof (char) * (n + 1));
 while (*src && n --)
    *dst++ = *src++;
 *dst = EOSTR;
 return (ret);
}

unsigned short   *StrDupToUnicode(const char *src)
{
 unsigned short  *ret, *dst;

 ret = dst = malloc (sizeof (unsigned short) * (strlen (src) + 1));
 while (*src)
    {
    *dst++ = *src++;
    }
 *dst = EOSTR;
 return (ret);
}

unsigned short   *StrNDupToUnicode (const char *src, int n)
{
 int    n2;
 unsigned short  *ret, *dst;

 n2 = strlen (src);
 if (n2 < n)
    n = n2;
 ret = dst = malloc (sizeof (unsigned short) * (n + 1));
 while (*src && n --)
    {
    *dst++ = *src++;
    }
 *dst = EOSTR;
 return (ret);
}

int     StrNull (char *s)
{
 if (s == 0 || *s == EOSTR)
    return (1);
 return (0);
}

void    StrReplace (char *s, char c1, char c2)
{
 while (*s)
   {
   if (*s == c1)
      *s = c2;
   s++;
   }
}

/*
char    *StrSChr (char *s, char *s2)
{
 int    i;

 while (*s)
   {
   i = 0;
   while (s[i] == s2[i] && s[i] && s2[i])
     i++;
   if (s2[i] == EOSTR)
      return (s);
   s++;
   }
 return (0);
}
*/

int         GetNbrHex (const char *s)
{
    int     result = 0;

    while (*s != EOSTR)
    {
        int digit = *s;
        if (digit >= '0' && digit <= '9')
            digit = digit - '0';
        else if (digit >= 'a' && digit <= 'f')
            digit = digit - 'a' + 10;
        else if (digit >= 'A' && digit <= 'F')
            digit = digit - 'F' + 10;
        else
            break;
        result = result * 16 + digit;
        s++;
    }
    return (result);
}

int         Power (int base, int power)
{
    int ret = 1;
    while (power > 0)
    {
        ret = ret * base;
        power --;
    }
    return (ret);
}

int	Match (const char *src, const char *wildcards)
{
  int	nbr;
  int	val;
  int	i;

  nbr = 0;
  for (val = 0; ; val++)
    if (wildcards[val] == '*')
      {
	i = 0;
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

void    Chomp (char *s)
{
  int   last;

  last = strlen(s) - 1;
  while (last >= 0 && (s[last] == '\n' || s[last] == '\r'))
    {
    s[last] = EOSTR;
    last -= 1;
    }
}

void    Trim (char *s)
{
  char  *s1;
  char  *s2;

  s1 = s2 = s;
  while (*s2 == ' ' || *s2 == '\t')
    s2++;
  if (s1 != s2)
     {
     while (*s2 != EOSTR)
       *s1++ = *s2++;
     *s1 = EOSTR;
     }
  Trim_End(s);
}

void     Trim_End(char *s)
{
  int   i;

  i = strlen(s) - 1;
  while (i > 0 && (s[i] == ' ' || s[i] == '\t'))
    {
    s[i] = EOSTR;
    i--;
    }
}

void    Remove_Spaces(char *s)
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

void    Replace_Backslash_N(char *s)
{
  int   i;

  while (*s)
    {
    if (s[0] == '\\' && s[1] == 'n')
       {
       s[0] = '\n';
       for (i = 1; s[i]; i++)
          s[i] = s[i + 1];
       }
    s++;
    }
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

// INITIALIZE PIFOMETER -------------------------------------------------------
// (Random Number Generators)
void    Random_Init (void)
{
/*
 #ifdef ARCH_DOS
   struct time ttmp;
   gettime (&ttmp);
   srand (ttmp.ti_sec + (ttmp.ti_min * ttmp.ti_hour));
   srandom (ttmp.ti_sec + (ttmp.ti_min * ttmp.ti_hour));
 #else
*/
   srand ((unsigned int)time (NULL));
   #ifndef ARCH_WIN32
      srandom (time (NULL));
   #endif
/*
 #endif
*/
}

// Convert a BCD number to decimal
// Note: no error handling is done, if using A-F values
int     BCD_to_Dec (int bcd)
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

