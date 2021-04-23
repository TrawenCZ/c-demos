#define CUT
#define CUT_MAIN
#include "cut.h"

#include <stdlib.h>
#include <string.h>
#include "dynamicLib.h"

TEST(dyn_strcpy)
{
    const char *orig = "Ahoj svet !";

    SUBTEST(standard_string)
    {
        char *copy = dyn_strcpy(orig);
        CHECK(copy != NULL);
        CHECK(strcmp(orig, copy) == 0);
        free(copy);
    }

    SUBTEST(provided_null)
    {
        CHECK(dyn_strcpy(NULL) == NULL);
    }
}

TEST(dyn_strjoin)
{
    const char *ahoj = "Ahoj ";
    const char *svet = "svet!";

    SUBTEST(two_words)
    {
        char *result = dyn_strjoin(ahoj, svet);
        CHECK(result != NULL);
        CHECK(strcmp(result, "Ahoj svet!") == 0);
        free(result);
    }

    SUBTEST(both_empty)
    {
        char *result = dyn_strjoin("", "");
        CHECK(result != NULL);
        CHECK(strcmp(result, "") == 0);
        free(result);
    }

    SUBTEST(both_null)
    {
        CHECK(dyn_strjoin(NULL, NULL) == NULL);
    }

    SUBTEST(first_null)
    {
        CHECK(dyn_strjoin(NULL, ahoj) == NULL);
    }

    SUBTEST(second_null)
    {
        CHECK(dyn_strjoin(ahoj, NULL) == NULL);
    }
}

TEST(dyn_alloc2d)
{
    SUBTEST(three_rows_cols_from_one_to_three)
    {
        size_t cols[] = { 1, 2, 3 };
        char **matrix = (char **)dyn_alloc2d(3, cols);
        CHECK(matrix != NULL);

        for (int i = 0; i < 3; i++) {
            CHECK(matrix[i] != NULL);
            free(matrix[i]);
        }
        free(matrix);
    }

    SUBTEST(two_rows_with_zero)
    {
        size_t cols[] = { 0, 1 };
        char **matrix = (char **)dyn_alloc2d(2, cols);
        CHECK(matrix != NULL);

        CHECK(matrix[0] == NULL);

        CHECK(matrix[1] != NULL);
        free(matrix[1]);

        free(matrix);
    }

    SUBTEST(zero_rows)
    {
        size_t cols[] = { 0, 1 };
        CHECK(dyn_alloc2d(0, cols) == NULL);
    }

    SUBTEST(columns_are_null)
    {
        CHECK(dyn_alloc2d(5, NULL) == NULL);
    }
}

TEST(dyn_free2d)
{
    SUBTEST(free_three_rows)
    {
        int **matrix = malloc(sizeof(int *) * 3);

        for (int i = 0; i < 3; i++) {
            matrix[i] = malloc(sizeof(int) * (i + 1));
        }

        CHECK(dyn_free2d((void **)matrix, 3) == 0);
    }

    SUBTEST(null_memory)
    {
        CHECK(dyn_free2d(NULL, 2) == 1);
    }
}

TEST(dyn_str_split_sub)
{
    SUBTEST(func_test)
    {
        const char *orig = "carrot, bannana, orange, kebab, mouse";
        const char expected[10][20] = { "carrot", "bannana", "orange", "kebab", "mouse" };
        size_t size;
        char **split = dyn_str_split_sub(orig, ", ", &size);
        CHECK(size == 5);

        for (int i = 0; i < 5; i++) {
            CHECK(strcmp(expected[i], split[i]) == 0);
        }

        for (size_t i = 0; i < size; i++) {
            free(split[i]);
        }

        free(split);
    }

    SUBTEST(null_as_size)
    {
        CHECK(dyn_str_split_sub("ahoj, svet", ", ", NULL) == NULL);
    }

    SUBTEST(null_as_input)
    {
        size_t size = 0;
        CHECK(dyn_str_split_sub(NULL, ", ", &size) == NULL);
    }

    SUBTEST(null_as_substring)
    {
        size_t size = 0;
        CHECK(dyn_str_split_sub("ahoj, svet", NULL, &size) == NULL);
    }
}
