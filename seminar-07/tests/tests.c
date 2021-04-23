#define CUT
#define CUT_MAIN

//#ifndef DEBUG
//#  define CUT_FORK_MODE
//#endif

#include "cut.h"

#include <stdio.h>
#include <string.h>

#include "list.h"

#include "test_utils.h"

//-----------------------------------------------------------------------------
//  Callbacks
//-----------------------------------------------------------------------------

#define UNUSED(VARIABLE)                                                    \
    ((void) (VARIABLE))

#define NODE_SET_INT(NODE, VALUE)                                           \
    do {                                                                    \
        int _value = (VALUE);                                               \
        memcpy((NODE)->data, &_value, sizeof(int));                         \
    } while (0)

static
struct node **list_fake_nodes(struct list *list, size_t size)
{
    struct node **result = malloc((size + 1) * sizeof(struct node *));

    if (result == NULL)
        cut_FatalExit("dummyNodes: Cannot allocate memory for nodes");

    for (unsigned i = 0; i < size; ++i) {
        result[i] = malloc(sizeof(struct node)
#if defined(LLIST_ENABLE_FAM)
                        + list->elem_size
#endif
                    );

        if (result[i] == NULL)
            cut_FatalExit("dummyNodes: Cannot allocate memory for node");

#if !defined(LLIST_ENABLE_FAM)
        result[i]->data = malloc(list->elem_size);
        if (result[i]->data == NULL)
            cut_FatalExit("dummyNodes: Cannot allocate memory for data");
#endif
    }

    for (unsigned i = 0; i < size; ++i) {
        result[i]->next = (i + 1 == size) ? NULL : result[i + 1];
#if defined(LLIST_ENABLE_TWO_LINKS)
        result[i]->prev = (i == 0) ? NULL : result[i - 1];
#endif
    }

    list->head = result[0];
    list->tail = result[size - 1];

    result[size] = NULL;
    return result;
}

static
void list_unfake_nodes(struct node **nodes)
{
    if (nodes == NULL)
        return;

    for (unsigned i = 0; nodes[i] != NULL; ++i) {
#if !defined(LLIST_ENABLE_FAM)
        free(nodes[i]->data);
#endif

        free(nodes[i]);
    }

    free(nodes);
}

//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------

//--  list_initialize  --------------------------------------------------------

TEST(list_init)
{
    // This test makes sure that ⟨list_initialize()⟩ initializes
    // head and tail to ⟨NULL⟩.
    struct list list;
    LIST_INITIALIZE(list, (struct node*)(42), (struct node*)(666), 90210);

    list_init(&list, sizeof(void*));

    CHECK(list.head == NULL);
    CHECK(list.tail == NULL);
    CHECK(list.elem_size == sizeof(void*));
}

//--  list_destroy  -----------------------------------------------------------

TEST(list_destroy)
{
    struct list list;
    struct node **nodes = NULL;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    SUBTEST(empty) {
        (void) nodes;

        // This should just end without SIGSEGV nor other bad stuff.
        list_destroy(&list);
    }

    SUBTEST(not_empty) {
        // Create 3 nodes.
        nodes = list_fake_nodes(&list, 3);

        for (int i = 0; i < 1; ++i)
            NODE_SET_INT(nodes[i], i);

        list_destroy(&list);

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 0, NULL);
    }

    free(nodes);
}

//--  list_size  --------------------------------------------------------------

TEST(list_size)
{
    struct list list;
    struct node **nodes = NULL;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    SUBTEST(empty) {
        (void) nodes;
        CHECK(list_size(&list) == 0U);
    }

    SUBTEST(single_value) {
        nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], 67);

        CHECK(list_size(&list) == 1U);
    }

    SUBTEST(two_values) {
        nodes = list_fake_nodes(&list, 2);
        NODE_SET_INT(nodes[0], 67);
        NODE_SET_INT(nodes[1], 76);

        CHECK(list_size(&list) == 2U);
    }

    SUBTEST(five_values) {
        nodes = list_fake_nodes(&list, 5);
        for (unsigned i = 0; i < 5; ++i)
            NODE_SET_INT(nodes[i], i);

        CHECK(list_size(&list) == 5U);
    }

    list_unfake_nodes(nodes);
}

//--  list_is_empty  ----------------------------------------------------------

TEST(list_is_empty)
{
    struct list list;
    struct node **nodes = NULL;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    SUBTEST(when_it_is_indeed_empty) {
        UNUSED(nodes);
        CHECK(list_is_empty(&list));
    }

    SUBTEST(when_it_is_in_fact_not_empty) {
        nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], 42);

        CHECK(!list_is_empty(&list));
    }

    SUBTEST(when_it_is_in_fact_not_empty_at_all) {
        nodes = list_fake_nodes(&list, 3);
        for (unsigned i = 0; i < 3; ++i)
            NODE_SET_INT(nodes[i], 42);

        CHECK(!list_is_empty(&list));
    }

    SUBTEST(make_sure_that_list_size_is_not_used) {
        // This test makes sure ⟨list_is_empty()⟩ does not use
        // ⟨list_size()⟩, as it would be extremely inefficient.
        struct node h, s, t;

        // Make head and tail point to a single node ⟨s⟩
        // where ⟨s⟩ behaves like a 'sink' - it points to itself.
#if defined(LLIST_ENABLE_TWO_LINKS)
        h.prev =
#endif
            t.next = NULL;

#if defined(LLIST_ENABLE_TWO_LINKS)
        t.prev = s.prev =
#endif
            h.next = s.next = &s;

        // The test will fail if list_size is called on this abomination…
        list.head = &h;
        list.tail = &t;
        // … well, OK, it will not fail, it will not end at all.

        // BUT you can solve halting problem in your free time and
        // uncomment the following line:
        //CHECK(will_halt(&list_is_empty, &list));

        CHECK(!list_is_empty(&list));
    }

    list_unfake_nodes(nodes);
}

//--  list_push_back  ---------------------------------------------------------

TEST(list_push_back)
{
    struct list list;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    int numbers[5];
    for (unsigned i = 0; i < 5; ++i)
        numbers[i] = i;

    SUBTEST(single_value_to_empty_list) {
        CHECK(list_push_back(&list, &numbers[0]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 1, numbers);
    }

    SUBTEST(single_value_to_list_with_one_element) {
        struct node **nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], numbers[0]);
        CHECK(list_push_back(&list, &numbers[1]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 2, numbers);

        // Actual nodes will be ⟨free()⟩d in ⟨free_nodes()⟩.
        free(nodes);
    }

    SUBTEST(single_value_to_list_with_two_elements) {
        struct node **nodes = list_fake_nodes(&list, 2);
        NODE_SET_INT(nodes[0], numbers[0]);
        NODE_SET_INT(nodes[1], numbers[1]);
        CHECK(list_push_back(&list, &numbers[2]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 3, numbers);

        // Actual nodes will be ⟨free()⟩d in ⟨free_nodes()⟩.
        free(nodes);
    }

    SUBTEST(more_elements) {
        for (unsigned i = 0; i < 5; ++i)
            CHECK(list_push_back(&list, &numbers[i]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 5, numbers);
    }

    free_nodes(list.head);
}

//--  list_pop_front  ---------------------------------------------------------

TEST(list_pop_front)
{
    struct list list;
    struct node **nodes = NULL;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    SUBTEST(empty_list) {
        UNUSED(nodes);

        int dummy;
        CHECK(!list_pop_front(&list, &dummy));
        LIST_CHECK_INTEGRITY(&list);
    }

    SUBTEST(single_value) {
        nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], 42);

        int popped;
        CHECK(list_pop_front(&list, &popped));
        CHECK(popped == 42);

        nodes[0] = NULL;
        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 0, NULL);
    }

    SUBTEST(two_values) {
        nodes = list_fake_nodes(&list, 2);

        int numbers[2];
        for (unsigned i = 0; i < 2; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 1; expected >= 0; --expected) {
            int popped;

            CHECK(list_pop_front(&list, &popped));
            CHECK(popped == (1 - expected));

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers + (2 - expected));
        }
    }

    SUBTEST(five_values) {
        nodes = list_fake_nodes(&list, 5);

        int numbers[5];
        for (unsigned i = 0; i < 5; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 4; expected >= 0; --expected) {
            int popped;

            CHECK(list_pop_front(&list, &popped));
            CHECK(popped == (4 - expected));

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers + (5 - expected));
        }
    }

    SUBTEST(five_values_with_NULL_target) {
        nodes = list_fake_nodes(&list, 5);

        int numbers[5];
        for (unsigned i = 0; i < 5; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 4; expected >= 0; --expected) {
            CHECK(list_pop_front(&list, NULL));

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers + (5 - expected));
        }
    }

    list_unfake_nodes(nodes);
}
#if defined(LLIST_ENABLE_ALL_ENDS)

//--  list_pop_back  ----------------------------------------------------------

TEST(list_pop_back)
{
    struct list list;
    struct node **nodes = NULL;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    SUBTEST(empty_list) {
        UNUSED(nodes);

        int dummy;
        CHECK(!list_pop_back(&list, &dummy));
        LIST_CHECK_INTEGRITY(&list);
    }

    SUBTEST(single_value) {
        nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], 42);

        int popped;
        CHECK(list_pop_back(&list, &popped));
        CHECK(popped == 42);

        nodes[0] = NULL;
        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 0, NULL);
    }

    SUBTEST(two_values) {
        nodes = list_fake_nodes(&list, 2);

        int numbers[2];
        for (unsigned i = 0; i < 2; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 1; expected >= 0; --expected) {
            int popped;

            CHECK(list_pop_back(&list, &popped));
            CHECK(popped == expected);

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers);
        }
    }

    SUBTEST(five_values) {
        nodes = list_fake_nodes(&list, 5);

        int numbers[5];
        for (unsigned i = 0; i < 5; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 4; expected >= 0; --expected) {
            int popped;

            CHECK(list_pop_back(&list, &popped));
            CHECK(popped == expected);

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers);
        }
    }

    SUBTEST(five_values_with_NULL_target) {
        nodes = list_fake_nodes(&list, 5);

        int numbers[5];
        for (unsigned i = 0; i < 5; ++i)
            NODE_SET_INT(nodes[i], numbers[i] = i);

        for (int expected = 4; expected >= 0; --expected) {
            CHECK(list_pop_back(&list, NULL));

            nodes[expected] = NULL;
            LIST_CHECK_INTEGRITY(&list);
            LIST_CHECK_CONTENTS(&list, expected, numbers);
        }
    }

    list_unfake_nodes(nodes);
}

//--  list_push_front  --------------------------------------------------------

TEST(list_push_front)
{
    struct list list;
    LIST_INITIALIZE(list, NULL, NULL, sizeof(int));

    int numbers[5];
    for (unsigned i = 0; i < 5; ++i)
        numbers[i] = i;

    SUBTEST(single_value_to_empty_list) {
        CHECK(list_push_front(&list, &numbers[0]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 1, numbers);
    }

    SUBTEST(single_value_to_list_with_one_element) {
        struct node **nodes = list_fake_nodes(&list, 1);
        NODE_SET_INT(nodes[0], numbers[1]);
        CHECK(list_push_front(&list, &numbers[0]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 2, numbers);

        // Actual nodes will be ⟨free()⟩d in ⟨free_nodes()⟩.
        free(nodes);
    }

    SUBTEST(single_value_to_list_with_two_elements) {
        struct node **nodes = list_fake_nodes(&list, 2);
        NODE_SET_INT(nodes[0], numbers[1]);
        NODE_SET_INT(nodes[1], numbers[2]);
        CHECK(list_push_front(&list, &numbers[0]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 3, numbers);

        // Actual nodes will be ⟨free()⟩d in ⟨free_nodes()⟩.
        free(nodes);
    }

    SUBTEST(more_elements) {
        for (unsigned i = 0; i < 5; ++i)
            CHECK(list_push_front(&list, &numbers[4 - i]));

        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 5, numbers);
    }

    free_nodes(list.head);
}
#endif // LLIST_ENABLE_ALL_ENDS
