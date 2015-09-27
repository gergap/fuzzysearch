#ifndef LIST_H_I2OIYNOT
#define LIST_H_I2OIYNOT

/* TODO: this could be replaced by a max-heap,
 * which is embedded in pre-allocated array.
 */
#define LIST_LEN 10

struct listel {
    int metric;
    int index;
};

struct list {
    int len;
    struct listel el[LIST_LEN];
};

void list_add(struct list *l, int metric, int index);

#endif /* end of include guard: LIST_H_I2OIYNOT */

