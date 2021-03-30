#define CUT
#define CUT_MAIN
#include "cut.h"
#include "stringLibrary.h"

#define UNUSED(a) ((void)(a))

#define ASSERT_IN(a, s, c) ASSERT(string_count_string_in_array(_c(a), s) == c)

#define _c(a) ((const char **)(a))

// String length
TEST(string_length)
{
    SUBTEST(single_word)
    {
        CHECK(string_length("Word") == 4);
    }

    SUBTEST(empty)
    {
        CHECK(string_length("") == 0);
    }

    SUBTEST(null)
    {
        CHECK(string_length(NULL) == 0);
    }

    SUBTEST(space)
    {
        CHECK(string_length("Ahoj svet!") == 10);
    }

    SUBTEST(multiple_words)
    {
        CHECK(string_length("Lorem\nIpsum\n Tabs Cols Rows\n") == 28);
    }
}

// String copy
TEST(string_copy)
{
    SUBTEST(single_word)
    {
        static char buffer[255];
        const char *pointer = string_copy(buffer, "Word");
        CHECK(pointer != NULL);
        CHECK(strcmp(buffer, "Word") == 0);
    }

    SUBTEST(empty)
    {
        static char buffer[255];
        const char *pointer = string_copy(buffer, "");
        CHECK(pointer != NULL);
        CHECK(strcmp(buffer, "") == 0);
    }

    SUBTEST(null)
    {
        static char buffer[255];
        const char *pointer = string_copy(buffer, NULL);
        CHECK(pointer == NULL);
    }
}

// String char count
TEST(string_count_char)
{
    SUBTEST(single_word)
    {
        CHECK(string_count_char("Ahoj", 'A') == 1);
    }

    SUBTEST(empty_word)
    {
        CHECK(string_count_char("", 'A') == 0);
    }

    SUBTEST(null)
    {
        CHECK(string_count_char(NULL, 'A') == 0);
    }

    SUBTEST(same_character)
    {
        CHECK(string_count_char("AAAA", 'A') == 4);
    }

    SUBTEST(case_sensitive_not_found)
    {
        CHECK(string_count_char("Ahoj", 'a') == 0);
    }

    SUBTEST(multiple_occurences)
    {
        CHECK(string_count_char("a aa aa bb ab bca", 'a') == 7);
    }
}

TEST(string_count_substr)
{
    SUBTEST(single_occurence)
    {
        CHECK(string_count_substr("Ahoj", "A") == 1);
    }

    SUBTEST(three_same_characters)
    {
        CHECK(string_count_substr("aaa", "a") == 3);
    }

    SUBTEST(empty_string)
    {
        CHECK(string_count_substr("", "a") == 0);
    }

    SUBTEST(null_string)
    {
        CHECK(string_count_substr(NULL, "a") == 0);
    }

    SUBTEST(empty_substring)
    {
        CHECK(string_count_substr("word", "") == 0);
    }

    SUBTEST(null_substring)
    {
        CHECK(string_count_substr("word", NULL) == 0);
    }

    SUBTEST(overlapping_substring)
    {
        CHECK(string_count_substr("aaab", "aa") == 2);
    }

    SUBTEST(substring_with_spaces)
    {
        CHECK(string_count_substr("abba baa ba b a", "a b") == 3);
    }
}

TEST(string_count_string_in_array)
{
    const char *const strings1[] = { "hello", "hi", 0 }; // all const

    SUBTEST(one_occurence_hello)
    {
        ASSERT_IN(strings1, "hello", 1);
    }

    SUBTEST(one_occurence_hi)
    {
        ASSERT_IN(strings1, "hi", 1);
    }

    SUBTEST(null_array)
    {
        ASSERT_IN(NULL, "hi", 0);
    }

    SUBTEST(non_null_array_and_word_null)
    {
        ASSERT_IN(strings1, NULL, 0);
    }

    SUBTEST(empty_array_should_not_contain_anyting)
    {
        const char *const empty[] = { NULL }; // all const
        ASSERT_IN(empty, NULL, 0);
        ASSERT_IN(empty, "Ahoj", 0);
        ASSERT_IN(empty, "word", 0);
        ASSERT_IN(empty, "", 0);
    }
    SUBTEST(array_with_null_as_first_element_should_behave_as_empty)
    {
        const char *const empty2[] = { NULL, "hello", NULL }; // all const
        ASSERT_IN(empty2, "hello", 0);
        ASSERT_IN(empty2, "", 0);
        ASSERT_IN(empty2, NULL, 0);
    }
    SUBTEST(multiple_occurences_of_word)
    {
        const char *const multi[] = { "hello", "hi", "hello", NULL };
        ASSERT_IN(multi, "hello", 2);
    }
}

TEST(string_split)
{
    SUBTEST(by_semicolon)
    {
        const char *original = "Ahoj;svet;000;Query";
        char result[50][256];
        int size;
        char delim = ';';
        string_split(original, result, &size, delim);
        CHECK(size == 4);
        CHECK(strcmp(result[0], "Ahoj") == 0);
        CHECK(strcmp(result[1], "svet") == 0);
        CHECK(strcmp(result[2], "000") == 0);
        CHECK(strcmp(result[3], "Query") == 0);
    }

    SUBTEST(by_space)
    {
        const char *original = "Ahoj svet 000 Query";
        char result[50][256];
        int size;
        char delim = ' ';
        string_split(original, result, &size, delim);
        CHECK(size == 4);
        CHECK(strcmp(result[0], "Ahoj") == 0);
        CHECK(strcmp(result[1], "svet") == 0);
        CHECK(strcmp(result[2], "000") == 0);
        CHECK(strcmp(result[3], "Query") == 0);
    }

    SUBTEST(empty_string)
    {
        const char *original = "";
        char result[50][256];
        int size;
        char delim = ';';
        string_split(original, result, &size, delim);
        CHECK(size == 0);
    }

    SUBTEST(string_contains_only_delimiters)
    {
        const char *original = ";;;";
        char result[50][256];
        int size;
        char delim = ';';
        string_split(original, result, &size, delim);
        CHECK(size == 4);
        CHECK(strcmp(result[0], "") == 0);
        CHECK(strcmp(result[1], "") == 0);
        CHECK(strcmp(result[2], "") == 0);
        CHECK(strcmp(result[3], "") == 0);
    }

     SUBTEST(string_too_long)
    {
            char original[257] = { '\0' };
            memset(original, '*', 256);
            char result[50][256];
            int size;
            char delim = '\n'; // delimiter doesn't matter
            string_split(original, result, &size, delim);
            CHECK(size == 1);
            CHECK(strlen(result[0]) == 255);
    }
}
