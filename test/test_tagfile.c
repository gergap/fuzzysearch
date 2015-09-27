#include <testlib.h>
#include <tagfile.h>
#include <list.h>

static struct tagfile g_tf;

void test_tagfile_search()
{
    struct list l = { 0 };
    int ret;
    int index;
    struct tag *t;

    ret = tagfile_search(&g_tf, "ta_se", &l);
    UVERIFY(ret > 0);
    index = l.el[0].index;
    t = tagfile_get(&g_tf, index);
    UVERIFY(t != NULL);
    UCOMPARESTR(t->tagname, "tagfile_search");

    UBENCHMARK {
        tagfile_search(&g_tf, "ta_se", &l);
    }
}

void test_init()
{
    tagfile_init(&g_tf);
    UVERIFY(tagfile_load(&g_tf, "tags") == 0);
}

void test_cleanup()
{
    tagfile_clear(&g_tf);
}

void register_tests()
{
    UREGISTER_INIT(test_init);
    UREGISTER_CLEANUP(test_cleanup);
    UREGISTER_TEST(test_tagfile_search);
}

UTEST_MAIN()

