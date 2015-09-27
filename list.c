#include <string.h>
#include "list.h"

/* stupid simple priority list.
 * we only use this for very short lists,
 * so O(n) doesn't hurt much here.
 */
void list_add(struct list *l, int metric, int index)
{
    int i = 0;
    int j;
    int insert = 0;
    int end;

    /* find insert pos */
    for (i = 0; i < l->len; ++i) {
        if (metric > l->el[i].metric) {
            insert = 1;
            break;
        }
    }

    if (insert) {
        /* insert element */
        end = l->len;
        if (end == LIST_LEN) end--;
        memmove(&l->el[i+1], &l->el[i], sizeof(struct listel)*(end-i));
        l->el[i].metric = metric;
        l->el[i].index = index;
        if (l->len < LIST_LEN) l->len++;
    } else {
        /* append element */
        if (l->len < LIST_LEN) {
            l->el[l->len].metric = metric;
            l->el[l->len].index = index;
            l->len++;
        }
    }
}


