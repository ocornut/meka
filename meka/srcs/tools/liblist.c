//
// Linked List Library
// 1999-2001
//

#include "shared.h"
#include "liblist.h"

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

// LIST_REMOVE.C ------------------------------------------------------------

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

// LIST_REVERSE.C -----------------------------------------------------------

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

void	*list_to_table(const t_list *list)
{
	int size = list_size(list);
	void** table = (void**)malloc(sizeof (void *) * (size + 1));
	int i;
	for (i = 0; i < size; i++)
	{
		table[i] = list->elem;
		list = list->next;
	}
	table[i] = 0;
	return (table);
}

