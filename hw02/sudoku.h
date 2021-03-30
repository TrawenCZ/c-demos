/**
 * @file sudoku.h
 * @author PB071
 * @date 10 Feb 2021
 * @brief Sudoku - second homework of PB071 in spring semester of 2021.
 *
 * Your task is to implement following functions in the sudoku.c file.
 *
 * @see https://www.fi.muni.cz/pb071/hw/homework-02
 */

// The two lines below enable bonus code in the attached main. 
// Uncomment when implemented.
//#define BONUS_GENERIC_SOLVE
//#define BONUS_GENERATE

#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>
#include <stdbool.h>

/* ************************************************************** *
 *          Remove set digits from squares with unknown           *
 * ************************************************************** */

/**
 * @brief Eliminate the possible digits in unknown squares in the row.
 *
 * E.g. for |...|456|789| the unknown squares will have
 * bitset 7 (000000111).
 *
 * @param sudoku 2D array of digit bitsets
 * @param row_index starting from 0 (leftmost) to 8 (rightmost)
 */
bool eliminate_row(unsigned int sudoku[9][9], int row_index);

/**
 * @brief Eliminate the possible digits in unknown squares in the column.
 *
 * @param sudoku 2D array of digit bitsets
 * @param col_index starting from 0 (top) to 8 (bottom)
 */
bool eliminate_col(unsigned int sudoku[9][9], int col_index);

/**
 * @brief Eliminate the possible digits in unknown squares in the box.
 *
 * @param sudoku 2D array of digit bitsets
 * @param row_index of the top most row in box, one of 0, 3, 6
 * @param col_index of the left most column in box, one of 0, 3, 6
 */
bool eliminate_box(unsigned int sudoku[9][9], int row_index, int col_index);

/* ************************************************************** *
 *                       Check/Solve sudoku                       *
 * ************************************************************** */

/**
 * @brief Check if the sudoku is to be solved yet, or is allready
 * complete.
 *
 * @note Does not care about validity, just checks whether cell
 * without unique value exists!
 * @param sudoku 2D array of digit bitsets
 */
bool needs_solving(unsigned int sudoku[9][9]);

/**
 * @brief Check if the set digits do not conflict with one another.
 *
 * @note The squares that do not have set digits are not checked!
 * @param sudoku 2D array of digit bitsets
 */
bool is_valid(unsigned int sudoku[9][9]);

/**
 * @brief Solve the sudoku using elimination as much as possible
 * without guessing or backtracking.
 *
 * If you fully solve the sudoku return true, otherwise false.
 *
 * @note In case the sudoku is invalid, reports to user with one
 * line message on STDERR and returns false. The state of sudoku
 * is then not defined.
 *
 * @param sudoku 2D array of digit bitsets
 */
bool solve(unsigned int sudoku[9][9]);

/* ************************************************************** *
 *                          Input/Output                          *
 * ************************************************************** */

/**
 * @brief Load the sudoku from STDIN.
 *
 * The digits are read from left to right and from top to bottom,
 * just as common english text. Zero is interpreted as an unknown
 * digit, i.e. a full digit bitset and NOT an empty set (000000000).
 *
 * @example
 * 000456789000123456000789123312000967697000845845000312231574000968231000574968000
 *
 * @note In case of invalid input, prints one line message
 * on STDERR and returns false. In such case, the sudoku is in undefined state,
 * and might contain partialy loaded sudoku.
 *
 * @note The exact format is described in assignment.
 *
 * @param sudoku 2D array to store digit bitsets, passed in undefined state.
 *
 * @return true if sudoku was successfuly loaded, false otherwise.
 */
bool load(unsigned int sudoku[9][9]);

/**
 * @brief Prints sudoku to STDOUT in grid with highlighted boxes.
 *
 * The X in the grid are substituted by respective digit or '.' if unknown.
 * If no digit can be placed to the cell, '!' is printed instead.
 *
 * @verbatim
 * +-------+-------+-------+
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * +-------+-------+-------+
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * +-------+-------+-------+
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * | X X X | X X X | X X X |
 * +-------+-------+-------+
 * @endverbatim
 *
 * @param sudoku 2D array of digit bitsets
 */
void print(unsigned int sudoku[9][9]);

/* ************************************************************** *
 *                              Bonus                             *
 * ************************************************************** */

#ifdef BONUS_GENERATE
void generate(unsigned int sudoku[9][9]);
#endif

#ifdef BONUS_GENERIC_SOLVE
bool generic_solve(unsigned int sudoku[9][9]);
#endif

#endif //SUDOKU_H
