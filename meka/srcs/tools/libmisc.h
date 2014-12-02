//
// Meka - LIBMY.H
// Various functions - Headers and Constants
//

#ifndef _LIB_MISC_H_
#define _LIB_MISC_H_

#define EOSTR                   (0)

unsigned short* StrCpyU16		(unsigned short *s1, unsigned short *s2);
unsigned short* StrDupToU16		(const char *src);
unsigned short* StrNDupToU16	(const char *src, int n);
int             StrLenU16		(const unsigned short *s);

void    StrWriteBitfield		(int v, int bit_count, char* out_buf);

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

//
// Linked List Library
// 1999-2001
// FIXME: shit
//

struct t_list
{
	t_list *	next;
	void *		elem;
};

void   	list_add(t_list **list, void *elem);
void   	list_add_to_end(t_list **list, void *elem);

void   	list_free(t_list **list);			// Free a list, call free() on all elements
void   	list_free_no_elem(t_list **list);	// Free a list, do not free elements (should be done by the program)
typedef void (*t_list_free_handler)(void*);
typedef int (*t_list_cmp_handler)(void *, void *);
void    list_free_custom(t_list **list, void (*custom_free)(void*));

void    list_remove(t_list **list, void *elem);
void   	list_reverse(t_list **list);
int		list_size(const t_list *list);
void   	list_sort(t_list **list, int (*fct)(void *elem1, void *elem2));

#endif
