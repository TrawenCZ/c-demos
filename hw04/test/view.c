#include "cut.h"

#include <string.h>

#include "view.h"

TEST(view_basic)
{
    struct view view;
    char *materialized;

    view = view_create("", 0);
    CHECK(view_length(view) == 0);
    CHECK(view_empty(view));
    materialized = view_materialize(view);
    CHECK(strcmp("", materialized) == 0);
    free(materialized);

    view = view_create("hello", 0);
    CHECK(view_length(view) == 5);
    CHECK(!view_empty(view));
    materialized = view_materialize(view);
    CHECK(strcmp("hello", materialized) == 0);
    free(materialized);

    view = view_create("hello", 3);
    CHECK(view_length(view) == 3);
    materialized = view_materialize(view);
    CHECK(strcmp("hel", materialized) == 0);
    free(materialized);
}

TEST(view_cmp)
{
    struct view view1, view2;
    char *materialized1;
    char *materialized2;

    view1 = view_create("111", 2);
    materialized1 = view_materialize(view1);

    view2 = view_create("11", 0);
    CHECK(view_cmp(view1, view2) == 0);

    view2 = view_create("111", 3);
    materialized2 = view_materialize(view2);
    CHECK(view_cmp(view1, view2) < 0);
    CHECK(view_cmp(view2, view1) > 0);
    CHECK(strcmp(materialized1, materialized2) < 0);
    CHECK(strcmp(materialized2, materialized1) > 0);
    free(materialized2);

    view2 = view_create("22", 0);
    materialized2 = view_materialize(view2);
    CHECK(view_cmp(view1, view2) < 0);
    CHECK(view_cmp(view2, view1) > 0);
    CHECK(strcmp(materialized1, materialized2) < 0);
    CHECK(strcmp(materialized2, materialized1) > 0);
    free(materialized2);

    free(materialized1);
}

static int view_all_any_predicate(int letter)
{
    return isalnum(letter) || isspace(letter);
}

TEST(view_all_any_count)
{
    struct view view;

    view = view_create("1234567890 abcd", 0);
    CHECK(view_all(view, view_all_any_predicate));
    CHECK(!view_all(view, isalnum));
    CHECK(!view_all(view, isspace));

    CHECK(view_any(view, view_all_any_predicate));
    CHECK(view_any(view, isalnum));
    CHECK(view_any(view, isalpha));
    CHECK(view_any(view, isspace));
    CHECK(!view_any(view, iscntrl));

    CHECK(view_count(view, isspace) == 1);
    CHECK(view_count(view, isalpha) == 4);
    CHECK(view_count(view, isdigit) == 10);
}

TEST(view_remove)
{
    struct view view;
    int length;

    view = view_create("you shall not pass", 0);
    length = view_length(view);

    CHECK(view_length(view_remove_prefix(view, 1)) + 1 == length);
    CHECK(view_length(view_remove_suffix(view, 1)) + 1 == length);

    CHECK(view_cmp(view_remove_prefix(view, 2), view_create("u shall not pass", 0)) == 0);
    CHECK(view_cmp(view_remove_suffix(view, 2), view_create("you shall not pa", 0)) == 0);

    CHECK(view_empty(view_remove_prefix(view, length + 10)));
    CHECK(view_empty(view_remove_suffix(view, length + 10)));
}

TEST(view_trim)
{
    struct view view;

    view = view_create("abcd12aa34abcd", 0);
    struct view round1 = view_create("12aa34abcd", 0);
    struct view round2 = view_create("12aa34", 0);
    struct view round3 = view_create("aa", 0);

    view = view_trim_front(view, isalpha);
    CHECK(view_cmp(view, round1) == 0);

    view = view_trim_back(view, isalpha);
    CHECK(view_cmp(view, round2) == 0);

    view = view_trim(view, isdigit);
    CHECK(view_cmp(view, round3) == 0);
}

TEST(view_head_tail)
{
    struct view view;
    struct view head;
    struct view tail;

    view = view_create("hello/there/is/slash", 0);
    head = view_head(&view, '/');
    tail = view_tail(&view, '/');

    CHECK(view_cmp(head, view_create("hello", 0)) == 0);
    CHECK(view_cmp(tail, view_create("slash", 0)) == 0);
    CHECK(view_cmp(view, view_create("there/is", 0)) == 0);

    const char *blabla = "blablabla";
    view = view_create("blablabla", 0);
    head = view_head(&view, '.');

    CHECK(view_cmp(head, view_create(blabla, 0)) == 0);
    CHECK(view_cmp(view, view_create("", 0)) == 0);

    tail = view_tail(&head, ' ');
    CHECK(view_cmp(tail, view_create(blabla, 0)) == 0);
    CHECK(view_cmp(head, view_create("", 0)) == 0);
}

static int view_word_predicate(int letter)
{
    return letter == '\\' || letter == '/';
}

TEST(view_word)
{
    struct view view;
    struct view head;
    struct view tail;

    view = view_create("hello/there/is\\slash", 0);
    head = view_word_front(&view, view_word_predicate);
    tail = view_word_back(&view, view_word_predicate);

    CHECK(view_cmp(head, view_create("hello", 0)) == 0);
    CHECK(view_cmp(tail, view_create("slash", 0)) == 0);
    CHECK(view_cmp(view, view_create("/there/is\\", 0)) == 0);

    const char *blabla = "blablabla";
    view = view_create("blablabla", 0);
    head = view_word_front(&view, NULL);

    CHECK(view_cmp(head, view_create(blabla, 0)) == 0);
    CHECK(view_cmp(view, view_create("", 0)) == 0);

    tail = view_word_back(&head, NULL);
    CHECK(view_cmp(tail, view_create(blabla, 0)) == 0);
    CHECK(view_cmp(head, view_create("", 0)) == 0);
}
