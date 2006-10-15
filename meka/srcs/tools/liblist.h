//
// Linked List Library
// Omar Cornut, 1999-2001
//

#ifndef __LIST_H__
#define __LIST_H__

typedef struct	s_list
{
  struct s_list	*next;
  void		*elem;
}		t_list;

/* Add an element to the beginning of the given list */
void   	list_add(t_list **list, void *elem);

/* Add an element to the end of the given list */
void   	list_add_to_end(t_list **list, void *elem);

/* Concatenate a list to another one */
void   	list_concat(t_list **list1, t_list *list2);

/* Free a list, call free() on all elements */
void   	list_free(t_list **list);

/* Free a list, do not free elements (should be done by the program) */
void   	list_free_no_elem(t_list **list);

/* Free a list, call given function on all elements */
void    list_free_custom(t_list **list, void (*custom_free)());

/* Merge a list in another. Given function is to compare if an element is not already in the other */
void   	list_merge(t_list **list1, t_list *list2, int (*cmp)(void *elem1, void *elem2));

/* Print a list, treating elements as strings, and adding \n between each */
void	list_print_str(t_list *list);

/* Remove a given element from a list. if freer != NULL, call freer on the ->elem */
void    list_remove(t_list **list, void *elem, void (*freer)());

/* Reverse a list */
void   	list_reverse(t_list **list);

/* Return number of elements of the given list */
int	list_size(t_list *list);

/* Sort a list using the given comparing strcmp-sslike function */
void   	list_sort(t_list **list, int (*fct)(void *elem1, void *elem2));

/* Convert given list to a table */
void	*list_to_tab(t_list *list);

#endif

