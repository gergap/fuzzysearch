#ifndef TAGFILE_H_YADXKKLJ
#define TAGFILE_H_YADXKKLJ

#include <stdint.h>
#include <stddef.h>

struct list;

struct tag {
    char *tagname;       /**< the tagname of a tag. */
    uint32_t lettermask; /**< bitmask of used letters. */
};

struct tagfile {
    size_t num_tags;  /**< number of used tags */
    size_t size_tags; /**< size of tag array (elements, not bytes) */
    struct tag *tags;
};

void tag_init(struct tag *tag, const char *name);
void tag_clear(struct tag *tag);
void tagfile_init(struct tagfile *tf);
void tagfile_clear(struct tagfile *tf);
struct tag *tagfile_get(struct tagfile *tf, int index);
int tagfile_add_tag(struct tagfile *tf, const char *name);
int tagfile_load(struct tagfile *tf, const char *filename);
int tagfile_search(struct tagfile *tf, const char *search, struct list *l);

#endif /* end of include guard: TAGFILE_H_YADXKKLJ */



