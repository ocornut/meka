//
// Linked List Library
// 1999-2001
//

#ifndef __LIST_H__
#define __LIST_H__

struct t_list
{
	t_list *	next;
	void *		elem;
};

/* Add an element to the beginning of the given list */
void   	list_add(t_list **list, void *elem);

/* Add an element to the end of the given list */
void   	list_add_to_end(t_list **list, void *elem);

/* Free a list, call free() on all elements */
void   	list_free(t_list **list);

/* Free a list, do not free elements (should be done by the program) */
void   	list_free_no_elem(t_list **list);

typedef void (*t_list_free_handler)(void*);
typedef int (*t_list_cmp_handler)(void *, void *);

/* Free a list, call given function on all elements */
void    list_free_custom(t_list **list, void (*custom_free)(void*));

/* Remove a given element from a list. */
void    list_remove(t_list **list, void *elem);

/* Reverse a list */
void   	list_reverse(t_list **list);

/* Return number of elements of the given list */
int		list_size(const t_list *list);

/* Sort a list using the given comparing strcmp-sslike function */
void   	list_sort(t_list **list, int (*fct)(void *elem1, void *elem2));

/* Convert given list to a table */
void *  list_to_table(const t_list *list);

#endif

