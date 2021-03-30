#include <stdlib.h>
#include <stddef.h>

#ifndef MY_STRING_H
#define MY_STRING_H

/*
 * Get the string length
 * @param str - Input string
 * @return Length of string; if str is NULL returns 0
 */
size_t string_length(const char *str);

/**
 * Creates string copy
 * @param result - Where copy will be stored
 * @param origin - From what string you will create copy
 * @return pointer to copied string
 */
char *string_copy(char *result, const char *origin);

/**
 * Counts chars in string
 * @param string - Input string where you will count chars
 * @param ch - Char to be count
 * @return Number of occurrences of character ch
 */
size_t string_count_char(const char *string, char ch);

/* COMPLEX */


/**
 * Counts substrings in string
 * @param original - Input string where you counting substrings
 * @param substring - Substring to be count
 * @return Number of substrings
 */
size_t string_count_substr(const char *original, const char *substring);

/**
 * Counts number of occurences of the string in the strings array
 *
 * @param array_size - Size of the a strings array
 * @param array - array of strings
 * @param string - string we are trying to find
 */

size_t string_count_string_in_array(const char *array[], const char *string);

/**
 * Splits given string and result puts into result matrix
 * @param original - Input string
 * @param result - Result matrix which has fixed 50 rows and 256 cols
 * @param size - Number of rows of result matrix which will be used
 * @param delim - Character by which will be split original string. Result substring will not contain this character.
 */
void string_split(const char *original, char result[50][256], int *size, char delim);

#endif
