//
// Linked List Library
// Omar Cornut, 1999-2001
//

#include "liblist.h"

void *  malloc(int);
void    free(void *);

// LIST_ADD.C ---------------------------------------------------------------

void		list_add(t_list **list, void *elem)
{
    t_list *    item;

    item = malloc(sizeof (t_list));
    item->elem = elem;
    item->next = *list;
    *list = item;
}

void		list_add_to_end(t_list **list, void *elem)
{
    t_list *      item;

    item = malloc(sizeof (t_list));
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

// LIST_FREE.C --------------------------------------------------------------

void		list_free(t_list **list)
{
  t_list	*next;

  while (*list)
    {
      next = (*list)->next;
      free((*list)->elem);
      free(*list);
      *list = next;
    }
}

void		list_free_no_elem(t_list **list)
{
  t_list	*next;
  while (*list)
    {
      next = (*list)->next;
      free(*list);
      *list = next;
    }
}

void            list_free_custom(t_list **list, void (*custom_free)())
{
  t_list	*next;

  while (*list)
    {
      next = (*list)->next;
      if (custom_free != 0)
        custom_free((*list)->elem);
      free(*list);
      *list = next;
    }
}

// LIST_REMOVE.C ------------------------------------------------------------

void            list_remove(t_list **list, void *elem, void (*freer)())
{
  t_list        *elem_prev;
  t_list        *save;
  t_list        *tmp;

  elem_prev = 0;
  save = *list;
  while (*list)
    {
      if ((*list)->elem == elem)
        {
          tmp = *list;
          *list = (*list)->next;
	  if (freer)
	    freer(tmp->elem);
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

// LIST_REVERSE.C -----------------------------------------------------------

void		list_reverse(t_list **list)
{
  t_list	*src;
  t_list	*dest;

  if (*list == 0)
    return;
  src = 0;
  while ((*list)->next)
    {
      dest = (*list)->next;
      (*list)->next = src;
      src = *list;
      *list = dest;
    }
  (*list)->next = src;
}

// LIST_SIZE.C --------------------------------------------------------------

int	list_size(t_list *list)
{
  int	cnt;

  cnt = 0;
  while (list)
    {
      cnt += 1;
      list = list->next;
    }
  return (cnt);
}

// LIST_SORT.C --------------------------------------------------------------

void		list_sort(t_list **list, int (*fct)(void *elem1, void *elem2))
{
  t_list	*i;
  t_list	*j;
  t_list	*temp;

  i = *list;
  while (i)
    {
      j = i->next;
      while (j)
	{
	  if (fct(i->elem, j->elem) >= 0)
	    {
	      temp = i->elem;
	      i->elem = j->elem;
	      j->elem = temp;
	    }
	  j = j->next;
	}
      i = i->next;
    }
}

// LIST_TO_TAB.C ------------------------------------------------------------

void	*list_to_tab(t_list *list)
{
  int	i;
  void	**table;
  int	size;

  size = list_size(list);
  table = malloc(sizeof (void *) * (size + 1));
  for (i = 0; i < size; i++)
    {
      table[i] = list->elem;
      list = list->next;
    }
  table[i] = 0;
  return (table);
}

