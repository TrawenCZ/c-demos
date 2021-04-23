#define _POSIX_C_SOURCE 1

#define CUT
#define CUT_MAIN
#include "cut.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "list_utils.h"

#include "test_utils.h"

//=============================================================================
//  Callbacks
//=============================================================================

static
bool is_even(const void *number)
{
    return *((const int*) number) % 2 == 0;
}

static
bool is_odd(const void *number)
{
    return *((const int*) number) % 2 != 0;
}

static
bool is_zero(const void *number)
{
    return *((const int*) number) == 0;
}

/* If called multiple times, saves all arguments in a single value
 * as a polynomial.
 *
 * example:
 *
 *     a = 1; r = 0;
 *     poly(1); poly(2); poly(3)
 *
 *     r == 123
 */
struct polynomial_context {
    int a;
    int r;
};

static
enum action_command poly(void *n, void *context)
{
    struct polynomial_context *pctxt = context;

    pctxt->r += (pctxt->a * *((int*) n));
    pctxt->a *= 10;

    return ACTION_CONTINUE;
}

// Integral comparator for ⟨list_sort⟩.
static
int cmpnum(const void *a, const void *b)
{
    int *na = (int*) a;
    int *nb = (int*) b;

    return (*na) - (*nb);
}

//=============================================================================
//  Tests
//=============================================================================

#define list_init_int(LIST)                                               \
    list_init((LIST), sizeof(int))

//--  list_any  ---------------------------------------------------------------

TEST(list_any)
{
    struct list list;
    list_init_int(&list);

    SUBTEST(empty) {
        CHECK(!list_any(&list, &is_zero));
    }

    SUBTEST(odd_value_true) {
        int numbers[] = { 0, 0, 0, 0, 1, 0 };
        for (int i = 0; i < 6; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_any(&list, &is_odd));
    }

    SUBTEST(even_value_false) {
        int numbers[] = { 1, 3, 5, 7, 9 };
        for (int i = 0; i < 5; ++i)
            list_push_back(&list, numbers + i);

        CHECK(!list_any(&list, &is_even));
    }

    list_destroy(&list);
}

//--  list_all  ---------------------------------------------------------------

TEST(list_all)
{
    struct list list;
    list_init_int(&list);

    SUBTEST(empty) {
        CHECK(list_all(&list, &is_zero));
    }

    SUBTEST(zero_values_true) {
        int numbers[] = { 0, 0, 0, 0, };
        for (int i = 0; i < 4; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_all(&list, &is_zero));
    }

    SUBTEST(odd_values_false) {
        int numbers[] = { 1, 3, 5, 7, 9, 11 };
        for (int i = 0; i < 6; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_all(&list, &is_odd));
    }

    list_destroy(&list);

}

//--  list_insert  ------------------------------------------------------------

TEST(list_insert)
{
    struct list list;
    list_init_int(&list);

    SUBTEST(begin) {
        int numbers[] = { 0, 1, 2, 3 };
        for (int i = 1; i < 4; ++i)
            list_push_back(&list, &numbers[i]);

        CHECK(list_insert(&list, 0, numbers));
        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 4, numbers);
    }

    SUBTEST(end) {
        int numbers[] = { 0, 1, 2, 3 };
        for (int i = 0; i < 3; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_insert(&list, 3, numbers + 3));
        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 4, numbers);
    }

    SUBTEST(middle) {
        int numbers[] = { -2, -1, 0, 1, 2 };
        for (int i = 0; i < 5; ++i) {
            if (i != 2)
                list_push_back(&list, numbers + i);
        }

        CHECK(list_insert(&list, 2, numbers + 2));
        LIST_CHECK_INTEGRITY(&list);
        LIST_CHECK_CONTENTS(&list, 5, numbers);
    }

    list_destroy(&list);
}

//--  list_get  ---------------------------------------------------------------

TEST(list_get)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 0, 1, 2, 3, 4 };
    for (int i = 0; i < 5; ++i)
        list_push_back(&list, numbers + i);

    SUBTEST(non_existent) {
        const void *ptr = list_get(&list, 8);

        CHECK(ptr == NULL);
    }

    SUBTEST(members) {
        for (int i = 0; i < 5; ++i) {
            void *expected = &numbers[i];
            const void *got = list_get(&list, i);

            CHECK(memcmp(got, expected, sizeof(int)) == 0);
        }
    }

    list_destroy(&list);
}

//--  list_get_index  ---------------------------------------------------------

TEST(list_get_index)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 0, 1, 2, 3, 4 };
    for (int i = 0; i < 5; ++i)
        list_push_back(&list, &numbers[i]);

    SUBTEST(non_existent) {
        int i = 8;
        int index = list_get_index(&list, &i);

        CHECK(index == -1);
    }

    SUBTEST(members) {
        for (int i = 0; i < 5; ++i) {
            int val = numbers[i];
            int index = list_get_index(&list, &val);

            CHECK(index == i);
        }
    }

    list_destroy(&list);
}

//--  list_remove  ------------------------------------------------------------

TEST(list_remove)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 0, 1, 2, 3 };

    for (int i = 0; i < 4; ++i) {
        list_push_back(&list, &numbers[i]);
    }

    SUBTEST(begin) {
        int removed;

        CHECK(list_remove(&list, 0, &removed));
        LIST_CHECK_INTEGRITY(&list);
        CHECK(removed == 0);

        LIST_CHECK_CONTENTS(&list, 3, &numbers[1]);
    }

    SUBTEST(end) {
        int removed;

        CHECK(list_remove(&list, 3, &removed));
        LIST_CHECK_INTEGRITY(&list);
        CHECK(removed == 3);

        LIST_CHECK_CONTENTS(&list, 3, &numbers[0]);
    }

    SUBTEST(middle) {
        int removed;

        CHECK(list_remove(&list, 1, &removed));
        LIST_CHECK_INTEGRITY(&list);
        CHECK(removed == 1);

        CHECK(list_remove(&list, 1, &removed));
        LIST_CHECK_INTEGRITY(&list);
        CHECK(removed == 2);

        numbers[1] = 3;
        LIST_CHECK_CONTENTS(&list, 2, numbers);
    }

    list_destroy(&list);
}

//--  list_find_first  --------------------------------------------------------

TEST(list_find_first)
{
    struct list list;
    list_init_int(&list);

    SUBTEST(no_match) {
        int numbers[] = { 1, 2, 3 };

        for (int i = 0; i < 3; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_find_first(&list, &is_zero) == NULL);
    }

    SUBTEST(match) {
        int numbers[] = { 1, 0, 2, 0, 3 };

        for (int i = 0; i < 5; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_find_first(&list, &is_zero) == list.head->next->data);
    }

    list_destroy(&list);
}

//--  list_find_last  ---------------------------------------------------------

TEST(list_find_last)
{
    struct list list;
    list_init_int(&list);

    SUBTEST(no_match) {
        int numbers[] = { 1, 2, 3 };

        for (int i = 0; i < 3; ++i)
            list_push_back(&list, numbers + i);

        CHECK(list_find_last(&list, &is_zero) == NULL);
    }

    SUBTEST(match) {
        int numbers[] = { 1, 0, 2, 0, 3 };

        for (int i = 0; i < 5; ++i)
            list_push_back(&list, numbers + i);

        struct node *pre_tail = list.head;
        while (pre_tail != NULL && pre_tail->next != list.tail)
            pre_tail = pre_tail->next;

        CHECK(list_find_last(&list, &is_zero) == pre_tail->data);
    }

    list_destroy(&list);
}

//--  list_for_each  ----------------------------------------------------------

TEST(list_for_each)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 3, 7, 2, 1 };

    for (int i = 0; i < 4; ++i)
        list_push_back(&list, numbers + i);

    struct polynomial_context pctxt = { .a = 1, .r = 0 };
    list_for_each(&list, &poly, &pctxt);
    CHECK(pctxt.r == 1273);

    list_destroy(&list);
}

//--  list_for_each_reverse  --------------------------------------------------

TEST(list_for_each_reverse)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 1, 3, 0, 4, 9 };

    for (int i = 0; i < 5; ++i)
        list_push_back(&list, numbers + i);

    struct polynomial_context pctxt = { .a = 1, .r = 0 };
    list_for_each_reverse(&list, &poly, &pctxt);
    CHECK(pctxt.r == 13049);

    list_destroy(&list);
}

//--  list_sort  --------------------------------------------------------------

TEST(list_sort)
{
    struct list list;
    list_init_int(&list);

    int numbers[] = { 6, 7, 2, 3, 8, 1, 9, 4, 0, 5 };
    int ordered[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    for (int i = 0; i < 10; ++i)
        list_push_back(&list, &numbers[i]);

    list_sort(&list, &cmpnum);

    LIST_CHECK_INTEGRITY(&list);
    LIST_CHECK_CONTENTS(&list, 10, ordered);

    list_destroy(&list);
}
