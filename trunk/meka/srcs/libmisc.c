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

void    StrWriteBitfield(int v, int n_bits, char* out_field)
{
	char* p = out_field;

	for (int bit_idx = n_bits-1; bit_idx >= 0; bit_idx--)
	{
		*p++ = (v & (1 << bit_idx)) ? '1' : '0';
		if (bit_idx != 0 && (bit_idx & 7) == 0)
			*p++ = '.';
	}
	*p = EOSTR;
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

int	StrMatch (const char *src, const char *wildcards)
{
	int nbr = 0;
	for (int val = 0; ; val++)
	{
		if (wildcards[val] == '*')
		{
			int i = 0;
			do
			{
				nbr = nbr + StrMatch(src + val + i, wildcards + val + 1);
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

//
// Linked List Library
// 1999-2001
// FIXME: shit
//

void		list_add(t_list **list, void *elem)
{
	t_list* item = (t_list*)malloc(sizeof (t_list));
	item->elem = elem;
	item->next = *list;
	*list = item;
}

void		list_add_to_end(t_list **list, void *elem)
{
	t_list* item = (t_list*)malloc(sizeof (t_list));
	item->elem = elem;
	item->next = 0;
	if (*list == 0)
	{
		// List is empty, stick on the beginning
		*list = item;
	}
	else
	{
		// Get to end of list (slow) to add element
		t_list *list2 = *list;
		while (list2->next)
			list2 = list2->next;
		list2->next = item;
	}
}

void		list_free(t_list **list)
{
	while (*list)
	{
		t_list* next = (*list)->next;
		free((*list)->elem);
		free(*list);
		*list = next;
	}
}

void		list_free_no_elem(t_list **list)
{
	while (*list)
	{
		t_list* next = (*list)->next;
		free(*list);
		*list = next;
	}
}

void            list_free_custom(t_list **list, void (*custom_free)(void *))
{
	while (*list)
	{
		t_list* next = (*list)->next;
		if (custom_free != 0)
		{
			void *elem = (*list)->elem;
			custom_free(elem);
		}
		free(*list);
		*list = next;
	}
}

void            list_remove(t_list **list, void *elem)
{
	t_list* elem_prev = 0;
	t_list* save = *list;
	while (*list)
	{
		if ((*list)->elem == elem)
		{
			t_list* tmp = *list;
			*list = (*list)->next;
			free(tmp);
			if (!elem_prev)
				save = *list;
			else
				elem_prev->next = *list;
		}
		if ((elem_prev = *list))
			*list = (*list)->next;
	}
	*list = save;
}

void	list_reverse(t_list **list)
{
	if (*list == 0)
		return;
	t_list* src = 0;
	while ((*list)->next)
	{
		t_list* dest = (*list)->next;
		(*list)->next = src;
		src = *list;
		*list = dest;
	}
	(*list)->next = src;
}

int		list_size(const t_list *list)
{
	int cnt = 0;
	while (list)
	{
		cnt++;
		list = list->next;
	}
	return (cnt);
}

void	list_sort(t_list **list, int (*fct)(void *elem1, void *elem2))
{
	t_list* i = *list;
	while (i)
	{
		t_list* j = i->next;
		while (j)
		{
			if (fct(i->elem, j->elem) >= 0)
			{
				void* temp = i->elem;
				i->elem = j->elem;
				j->elem = temp;
			}
			j = j->next;
		}
		i = i->next;
	}
}
