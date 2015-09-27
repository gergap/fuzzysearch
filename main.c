#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>
#include <errno.h>
#include <assert.h>

#define countof(x) (sizeof((x)) / sizeof((x)[0]))
/* step size for reallocating memory */
#define MEM_STEP_SIZE 1000

struct tag {
    char *tagname;
    uint32_t lettermask;
};

struct tagfile {
    size_t num_tags;  /**< number of used tags */
    size_t size_tags; /**< size of tag array (elements, not bytes) */
    struct tag *tags;
};

uint32_t compute_lettermask(const char *text)
{
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
        fprintf(stderr, "Could not open file '%s': %s", filename, strerror(errno));
        return -1;
    }

    while(!feof(f)) {
        fgets(line, sizeof(line), f);
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

int string_metric(const char *a, const char *b);

struct listel {
    int m;
    int i;
};

#define LIST_LEN 10
struct list {
    int len;
    struct listel el[LIST_LEN];
};

void list_add(struct list *l, int max, int index)
{
    int i = 0;
    int j;
    int insert = 0;
    int end;

    /* find insert pos */
    for (i = 0; i < l->len; ++i) {
        if (max > l->el[i].m) {
            insert = 1;
            break;
        }
    }

    if (insert) {
        /* insert element */
        end = l->len;
        if (end == LIST_LEN) end--;
        for (j = end; j > i; j--) {
            l->el[j] = l->el[j-1];
        }
        l->el[i].m = max;
        l->el[i].i = index;
        if (l->len < LIST_LEN) l->len++;
    } else {
        /* append element */
        if (l->len < LIST_LEN) {
            l->el[l->len].m = max;
            l->el[l->len].i = index;
            l->len++;
        }
    }
}

void tagfile_search(struct tagfile *tf, const char *search)
{
    uint32_t lettermask = compute_lettermask(search);
    size_t i;
    size_t matches = 0;
    int m;
    int max = 0;
    int imax;
    struct list l = { 0 };

    for (i = 0; i < tf->num_tags; ++i) {
        if ((tf->tags[i].lettermask & lettermask) == lettermask) {
            m = string_metric(search, tf->tags[i].tagname);
            if (m > max) {
                max = m;
                imax = i;
            }
            list_add(&l, m, i);
            matches++;
        }
    }

    printf("search: %s, %zu matches\n", search, matches);
    printf("highest ranking: %i: %s\n", max, tf->tags[imax].tagname);
    for (i = 0; i < l.len; ++i) {
        printf("rank: %i, index: %i, text: %s\n", l.el[i].m, l.el[i].i, tf->tags[l.el[i].i].tagname);
    }
}

/**
 * a: first string
 * b: second string
 * A = f(a)
 * B = f(b)
 * where f() returns the Menge aller Buchstabenpaare...
 * Metric = 2 * |f(a) \cap f(b)| / ( |f(a)| + |f(b)| )
 * string: st tr ri in ng
 * strong: st tr ro on ng
 * result: 3 of 5 indentical
 *
 * This implementation avoids creating lists of letter pairs to compute the metric.
 * Instead it iterates just of the strings and compares the letter pairs in place.
 *
 * @param a
 * @param b
 *
 * @return
 */
int string_metric(const char *a, const char *b)
{
    int lena = strlen(a);
    int lenb = strlen(b);
    int i = 0;
    int j = 0;
    char a1, a2;
    char b1, b2;
    int n = lena + lenb - 2; /* |f(a)| + |f(b)| */
    int m = 0;

    /* sanity check: both strings must be at least two letters long */
    if (lena < 2 || lenb < 2) return -1;

    a2 = a[i++];
    while (a[i] != 0) {
        a1 = a2;
        a2 = a[i++];
        j = 0;
        b2 = b[j++];
        while (b[j] != 0) {
            b1 = b2;
            b2 = b[j++];
            if (a1 == b1 && a2 == b2) {
                m++;
            }
        }
    }

    m = 2000 * m / n;
    return m;
}

int main(int argc, char *argv[])
{
    struct tagfile tf;
    char filename[PATH_MAX] = "/home/gergap/work/embeddedstack/.git/tags";
    char search[256] = "";
    char *c;
    struct timespec start, end;

    tagfile_init(&tf);
    tagfile_load(&tf, filename);

    do {
        fgets(search, sizeof(search), stdin);
        c = strchr(search, '\n');
        if (c) *c = 0;

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        tagfile_search(&tf, search);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        end.tv_sec -= start.tv_sec;
        end.tv_nsec -= start.tv_nsec;
        printf("time: %ld us\n", end.tv_nsec / 1000);
    } while (strcmp(search, "exit") != 0);

    tagfile_clear(&tf);

    return 0;
}
