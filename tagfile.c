#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "tagfile.h"
#include "list.h"
#include "metric.h"

#define countof(x) (sizeof((x)) / sizeof((x)[0]))
/* step size for reallocating memory */
#define MEM_STEP_SIZE 1000
/* define search result threshold, we don't report result below this */
#define THRESHOLD 100 /* 10% matching */

/* computes a bitmask reflecting the used letters in string \c text. */
static uint32_t compute_lettermask(const char *text)
{
    /* notes, we could also make this case sensitive here,
     * be using a uint64_t and adding the upper case letters.
     * In 64bit we could store A-Za-z0-9_-.
     */
    static char letters[] = "abcdefghijklmnopqrstuvwxyz_-";
    unsigned int i;
    char c;
    uint32_t bit = 1;
    uint32_t mask = 0;

    static_assert(sizeof(letters) <= 31, "assert1");
    for (i = 0; i < countof(letters); i++) {
        c = letters[i];
        if (strchr(text, c)) {
            mask |= bit;
        }
        bit <<= 1;
    }

    return mask;
}

void tag_init(struct tag *tag, const char *name)
{
    tag->tagname = strdup(name);
    tag->lettermask = compute_lettermask(name);
}

void tag_clear(struct tag *tag)
{
    if (tag->tagname) {
        free(tag->tagname);
        tag->tagname = NULL;
    }
    tag->lettermask = 0;
}

struct tag *tagfile_get(struct tagfile *tf, int index)
{
    return &tf->tags[index];
}

int tagfile_add_tag(struct tagfile *tf, const char *name)
{
    if (tf->num_tags == tf->size_tags) {
        tf->size_tags += MEM_STEP_SIZE;
        /* reallocate memory */
        void *tmp = realloc(tf->tags, tf->size_tags * sizeof(struct tag));
        if (tmp) {
            /* realloc succeeded, assign new pointer */
            tf->tags = tmp;
        } else {
            /* handle error */
            return -1;
        }
    }

    tag_init(&tf->tags[tf->num_tags++], name);

    return 0;
}

void tagfile_init(struct tagfile *tf)
{
    tf->size_tags = tf->num_tags = 0;
    tf->tags = NULL;
}

void tagfile_clear(struct tagfile *tf)
{
    size_t i;

    if (tf->tags) {
        for (i = 0; i < tf->num_tags; ++i) {
            tag_clear(&tf->tags[i]);
        }
        free(tf->tags);
    }
    tf->size_tags = tf->num_tags = 0;
    tf->tags = NULL;
}

int tagfile_load(struct tagfile *tf, const char *filename)
{
    FILE *f = fopen(filename, "r");
    char line[2048];
    int ret = 0;
    char *tab;

    if (f == NULL) {
        fprintf(stderr, "Could not open file '%s': %s\n", filename, strerror(errno));
        return -1;
    }

    while(!feof(f)) {
        tab = fgets(line, sizeof(line), f);
        if (tab == NULL) break; /* handle error */
        if (line[0] == '!') continue; /* skip comments */
        tab = strchr(line, '\t');
        if (tab == NULL) continue; /* skip invalid line */
        *tab = 0; /* terminate string */

        /* add tag to tagfile data */
        ret = tagfile_add_tag(tf, line);
        if (ret != 0) break;
    }

#ifdef _DEBUG
    printf("%zu tags have been loaded. Used memory = %zu KB\n", tf->num_tags, tf->size_tags * sizeof(struct tag) / 1024);
#endif

    return ret;
}

int tagfile_search(struct tagfile *tf, const char *search, struct list *l)
{
    uint32_t lettermask = compute_lettermask(search);
    size_t i;
    size_t matches = 0;
    int m;

    l->len = 0;
    for (i = 0; i < tf->num_tags; ++i) {
        if ((tf->tags[i].lettermask & lettermask) == lettermask) {
            m = string_metric(tf->tags[i].tagname, search);
#ifdef _DEBUG
            printf("%i, %s\n", m, tf->tags[i].tagname);
#endif
            if (m >= THRESHOLD) list_add(l, m, i);
            matches++;
        }
    }

#if 0
    printf("search: %s, %zu matches (letters)\n", search, matches);
    for (i = 0; i < l.len; ++i) {
        printf("score: %i, index: %i, text: %s\n", l.el[i].m, l.el[i].i, tf->tags[l.el[i].i].tagname);
    }
#endif

    return l->len;
}

