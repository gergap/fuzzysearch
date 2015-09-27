#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>
#include <ncurses.h>

#include "tagfile.h"
#include "list.h"

/** Ncurses demo program showing the tagfile functionality. */
int main(int argc, char *argv[])
{
    struct tagfile tf;
    char filename[PATH_MAX] = "/home/gergap/work/embeddedstack/.git/tags";
    char search[256] = "";
    int len = 0;
    struct list l;
    struct timespec start, end;
    bool exit = false;
    int ret;
#ifdef HAVE_CURSES
    struct tag *t;
    int ch;
    int i;
#endif

    if (argc > 1) {
        strncpy(filename, argv[1], sizeof(filename));
        filename[sizeof(filename)-1] = 0;
    }

    tagfile_init(&tf);
    ret = tagfile_load(&tf, filename);
    if (ret != 0) {
        return EXIT_FAILURE;
    }

#ifdef HAVE_CURSES
    initscr();
    raw();
    printw("Input: ");
#endif

    do {
#ifdef HAVE_CURSES
        ch = getch();
        clear();
        /* process input */
        switch (ch) {
        case KEY_BACKSPACE:
        case KEY_DC:
        case 127:
            /* delete character */
            if (len > 0) {
                search[--len] = 0;
            }
            break;
        case 27:
            exit = true;
            break;
        default:
            search[len++] = ch;
            break;
        }
        /* output input */
        mvprintw(0, 0, "Input: %s (len=%i)", search, len);
#else
        printf("Input: ");
        fgets(search, sizeof(search), stdin);
        len = strlen(search) - 1; /* eat \n */
        search[len] = 0;
        if (strcmp(search, "exit") == 0) exit = true;
        /* output input */
        printf("Input: %s (len=%i)\n", search, len);
#endif

        if (len < 2) continue;

        /* query */
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        tagfile_search(&tf, search, &l);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        /* output result */
#ifdef HAVE_CURSES
        for (i = 0; i < l.len; ++i) {
            t = tagfile_get(&tf, l.el[i].index);
            mvprintw(i+1, 0, "%s (score=%i, index=%i)", t->tagname, l.el[i].metric, l.el[i].index);
        }
        end.tv_sec -= start.tv_sec;
        end.tv_nsec -= start.tv_nsec;
        i++;
        mvprintw(i++, 0, "time: %ld us\n", end.tv_nsec / 1000);
        mvprintw(i++, 0, "Press <Esc> to exit.\n");
#endif
    } while(!exit);

#ifdef HAVE_CURSES
    endwin();
#endif

    tagfile_clear(&tf);

    return 0;
}
