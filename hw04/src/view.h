#ifndef VIEW_H
#define VIEW_H

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct view
{
    const char *begin;
    const char *end;
};

/**
 * @brief Create a view
 *
 * @param value C string
 * @param length length of the string; in case it is zero or less the value is computed by `strlen`
 * @return struct view
 */
static inline struct view view_create(const char *value, int length)
{
    struct view result;
    result.begin = value;
    if (length <= 0) {
        if (result.begin)
            length = strlen(result.begin);
        else
            length = 0;
    }
    result.end = value + length;
    return result;
}

/**
 * @brief Get the length of a view
 *
 * @param view
 * @return int length of the view
 */
static inline int view_length(struct view view)
{
    return view.end - view.begin;
}

/**
 * @brief Check if a view is empty
 *
 * @param view
 * @return bool True is the view is empty
 */
static inline bool view_empty(struct view view)
{
    return view.end == view.begin;
}

/**
 * @brief Compare two views
 *
 * @param lhs
 * @param rhs
 * @return int (lhs < rhs): <0
 *             (lhs == rhs): ==
 *             (lhs > rhs): 0<
 */
static inline int view_cmp(struct view lhs, struct view rhs)
{
    int lhs_length = view_length(lhs);
    int rhs_length = view_length(rhs);
    int length;
    int second_round;

    if (lhs_length == rhs_length) {
        length = lhs_length;
        second_round = 0;
    } else if (lhs_length < rhs_length) {
        length = lhs_length;
        second_round = -1;
    } else {
        length = rhs_length;
        second_round = 1;
    }

    int result = strncmp(lhs.begin, rhs.begin, length);
    if (!result)
        result = second_round;

    return result;
}

/**
 * @brief Apply a predicate on all characters
 *
 * @param view
 * @param predicate (int) -> int; not NULL
 * @return int True if all characters satisfy the predicate
 */
static inline bool view_all(struct view view, int (*predicate)(int))
{
    bool value = true;
    for (const char *i = view.begin; value && i != view.end; ++i)
        value = predicate(*i);
    return value;
}

/**
 * @brief Apply a predicate on all characters
 *
 * @param view
 * @param predicate (int) -> int; not NULL
 * @return int True if any character satisfies the predicate
 */
static inline bool view_any(struct view view, int (*predicate)(int))
{
    bool value = false;
    for (const char *i = view.begin; !value && i != view.end; ++i)
        value = predicate(*i);
    return value;
}

/**
 * @brief Apply a predicate on all characters
 *
 * @param view
 * @param predicate (int) -> int; not NULL
 * @return int Count all characters satisfying predicate
 */
static inline int view_count(struct view view, int (*predicate)(int))
{
    int count = 0;
    for (const char *i = view.begin; i != view.end; ++i) {
        if (predicate(*i))
            ++count;
    }
    return count;
}

/**
 * @brief Remove prefix of a view
 *
 * @param view
 * @param prefix Remove `min(prefix, view_length(view))` first characters of the view
 * @return struct view Shortened view
 */
static inline struct view view_remove_prefix(struct view view, int prefix)
{
    if (view.end - view.begin < prefix)
        view.begin = view.end;
    else
        view.begin += prefix;
    return view;
}

/**
 * @brief Remove sufix of a view
 *
 * @param view
 * @param suffix Remove `min(suffix, view_length(view))` last characters of the view
 * @return struct view Shortened view
 */
static inline struct view view_remove_suffix(struct view view, int suffix)
{
    if (view.end - view.begin < suffix)
        view.end = view.begin;
    else
        view.end -= suffix;
    return view;
}

/**
 * @brief Trim characters from the front while predicate returns nonzero.
 *
 * @param view
 * @param predicate (int) -> int; if NULL, defaults to isspace
 * @return struct view Trimmed view
 */
static inline struct view view_trim_front(struct view view, int (*predicate)(int))
{
    if (!predicate)
        predicate = isspace;
    for (; view.begin && view.begin != view.end; ++view.begin) {
        if (!predicate(*view.begin))
            break;
    }
    return view;
}

/**
 * @brief Trim characters from the back while predicate returns nonzero.
 *
 * @param view
 * @param predicate (int) -> int; if NULL, defaults to isspace
 * @return struct view Trimmed view
 */
static inline struct view view_trim_back(struct view view, int (*predicate)(int))
{
    if (!predicate)
        predicate = isspace;
    for (; view.end && view.begin != view.end; --view.end) {
        if (!predicate(*(view.end - 1))) {
            break;
        }
    }
    return view;
}

/**
 * @brief Trim characters from the front and the back while predicate returns nonzero.
 *
 * @param view
 * @param predicate (int) -> int; if NULL, defaults to isspace
 * @return struct view Trimmed view
 */
static inline struct view view_trim(struct view view, int (*predicate)(int))
{
    view = view_trim_front(view, predicate);
    view = view_trim_back(view, predicate);
    return view;
}

/**
 * @brief Get the head of a view divided by a `delimiter` from the tail.
 *
 *      Example: "head/long/tail" will lead to head="head" and tail="long/tail" if delimiter='/'
 *
 * @param tail Tail without head, the first character is `delimiter` (or it is empty).
 * @param delimiter
 * @return struct view Head
 */
static inline struct view view_head(struct view *tail, char delimiter)
{
    struct view head;
    head.begin = head.end = tail->begin;
    for (; tail->begin != tail->end; ++tail->begin, ++head.end) {
        if (*tail->begin == delimiter) {
            ++tail->begin;
            break;
        }
    }
    return head;
}

/**
 * @brief Get the tail of a view divided by a `delimiter` from the head.
 *
 *      Example: "head/long/tail" will lead to tail="tail" and head="head/long" if delimiter='/'
 *
 * @param tail Tail without head, the first character is `delimiter` (or it is empty).
 * @param delimiter
 * @return struct view Head
 */
static inline struct view view_tail(struct view *head, char delimiter)
{
    struct view tail;
    tail.begin = tail.end = head->end;
    for (; head->begin != head->end; --tail.begin, --head->end) {
        if (*(head->end - 1) == delimiter) {
            --head->end;
            break;
        }
    }
    return tail;
}

/**
 * @brief Get a word from the front of a view divided by a `predicate`.
 *
 * @param tail Tail without a word, the first character satisfies the `predicate`.
 * @param predicate (int) -> int; if NULL, defaults to isspace
 * @return struct view Word
 */
static inline struct view view_word_front(struct view *tail, int (*predicate)(int))
{
    if (!predicate)
        predicate = isspace;

    struct view head;
    head.begin = head.end = tail->begin;
    for (; tail->begin != tail->end; ++tail->begin, ++head.end) {
        if (predicate(*tail->begin)) {
            break;
        }
    }
    return head;
}

/**
 * @brief Get a word from the back of a view divided by a `predicate`.
 *
 * @param head Head without a word, the last character satisfies the `predicate`.
 * @param predicate (int) -> int; if NULL, defaults to isspace
 * @return struct view
 */
static inline struct view view_word_back(struct view *head, int (*predicate)(int))
{
    if (!predicate)
        predicate = isspace;

    struct view tail;
    tail.begin = tail.end = head->end;
    for (; head->begin != head->end; --tail.begin, --head->end) {
        if (predicate(*(head->end - 1)))
            break;
    }
    return tail;
}

/**
 * @brief Materialize a view into a C string
 *
 * @param view
 * @return char* Malloc'ed C string from the view. NULL in case no memory is available.
 */
static inline char *view_materialize(struct view view)
{
    int length = view_length(view);

    char *result = (char *) malloc(length + 1);
    if (!result)
        return NULL;

    if (length)
        strncpy(result, view.begin, length);
    result[length] = '\0';
    return result;
}

static inline char *view_join(int view_count, struct view views[view_count])
{
    int length = 0;
    for (int i = 0; i < view_count; ++i)
        length += view_length(views[i]);

    char *result = (char *) malloc(length + 1);
    if (!result)
        return NULL;

    char *cursor = result;
    for (int i = 0; i < view_count; ++i) {
        int item_length = view_length(views[i]);
        if (!item_length)
            continue;
        strncpy(cursor, views[i].begin, item_length);
        cursor += item_length;
    }
    result[length] = '\0';
    return result;
}

#endif
