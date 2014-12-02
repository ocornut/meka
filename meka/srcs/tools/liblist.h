//
// Linked List Library
// 1999-2001
// FIXME: shit
//

#ifndef __LIST_H__
#define __LIST_H__

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

