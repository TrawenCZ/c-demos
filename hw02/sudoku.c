#include "sudoku.h"
#include <stdlib.h>

int translate_to_printable(unsigned int source) {
    if (source == 0) return -1;
    unsigned int source_checker = 1;
    for (int i = 0; i < 9; i++) {
        if (source_checker == source) return i+1;
        source_checker <<= 1;
    }
    return -2;
}

/**
 * @brief  Check whether given number is present in the set.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param query     number which should be checked for presence
 * 
 * @return          true if set, false otherwise
 */
static bool bitset_is_set(unsigned int original, unsigned int query) {
	return ((original & query) != 0);
}

/**
 * @brief  Check whether given cell has a unique value assigned.
 *
 * @param original  bitset to check for single vs. multiple values.
 * 
 * @return          true if set, false otherwise
 */
static bool bitset_is_unique(unsigned int original) {
    unsigned int checker = 1;
    for (int i = 0; i < 9; i++) {
        if (original == checker) return true;
        checker <<= 1;
    }
	return false;
}

/**
 * @brief Compute the bitset of all done numbers in the box.
 *
 * You might like a similar function for row and for column.
 *
 * @param sudoku 2D array of digit bitsets
 * @param row_index of the top most row in box, one of 0, 3, 6
 * @param col_index of the left most column in box, one of 0, 3, 6
 */
static int box_bitset(unsigned int sudoku[9][9], int row_index, int col_index) {
    unsigned int bitset = 0;
    for (int row = row_index; row < row_index + 3; row++) {
        for (int col = col_index; col < col_index + 3; col++) {
            if (!bitset_is_unique(sudoku[row][col])) continue;
            bitset |= sudoku[row][col];
        }
    }
    return bitset;
}

/**
 * @brief  Drop number from bit set.
 *
 * For detailed description, see bitset_add.
 *
 * @param original  contents of the 2D sudoku cell.
 * @param number    to be dropped from the set
 * 
 * @return          new value of the cell without the number included
 */
static unsigned int bitset_drop(unsigned int original, unsigned int number) {
    return original & ~number;
}


/* ************************************************************** *
 *               Functions required by assignment                 *
 * ************************************************************** */

bool eliminate_row(unsigned int sudoku[9][9], int row_index)
{
    unsigned int row_bitset = 0, holder;
    bool sth_changed = false;

    for (int col = 0; col < 9; col++) {
        if (!bitset_is_unique(sudoku[row_index][col])) continue;
        row_bitset |= sudoku[row_index][col];
    }

    for (int col = 0; col < 9; col++) {
        if (bitset_is_unique(sudoku[row_index][col])) continue;
        holder = sudoku[row_index][col];
        sudoku[row_index][col] = bitset_drop(sudoku[row_index][col], row_bitset);
        if (sudoku[row_index][col] != holder) { sth_changed = true; }
    }
    return sth_changed;
}

bool eliminate_col(unsigned int sudoku[9][9], int col_index)
{
    unsigned int col_bitset = 0, holder;
    bool sth_changed = false;
    
    for (int row = 0; row < 9; row++) {
        if (!bitset_is_unique(sudoku[row][col_index])) continue;
        col_bitset |= sudoku[row][col_index];
    }

    for (int row = 0; row < 9; row++) {
        if (bitset_is_unique(sudoku[row][col_index])) continue;
        holder = sudoku[row][col_index];
        sudoku[row][col_index] = bitset_drop(sudoku[row][col_index], col_bitset);
        if (sudoku[row][col_index] != holder) { sth_changed = true; }
    }

    return sth_changed;
}

bool eliminate_box(unsigned int sudoku[9][9], int row_index, int col_index)
{
    unsigned int b_bitset = box_bitset(sudoku, row_index, col_index), holder;
    bool sth_changed = false;

    for (int row = row_index; row < row_index + 3; row++) {
        for (int col = col_index; col < col_index + 3; col++) {
            if (bitset_is_unique(sudoku[row][col])) continue;
            holder = sudoku[row][col];
            sudoku[row][col] = bitset_drop(sudoku[row][col], b_bitset);
            if (sudoku[row][col] != holder) { sth_changed = true; }
        }
    }

    return sth_changed;
}

bool needs_solving(unsigned int sudoku[9][9])
{
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            if (!bitset_is_unique(sudoku[row][col])) return true;
        }
    }
    return false;
}

static bool valid_box_bitset(unsigned int sudoku[9][9], int row_index, int col_index) {
    unsigned int bitset = 0;
    for (int row = row_index; row < row_index + 3; row++) {
        for (int col = col_index; col < col_index + 3; col++) {
            if (!bitset_is_unique(sudoku[row][col])) continue;
            if (bitset_is_set(bitset, sudoku[row][col])) { return false; }
            bitset += sudoku[row][col];
        }
    }
    return true;
}

bool is_valid(unsigned int sudoku[9][9])
{
    unsigned int cols_bitsets[9] = {0};
    unsigned int row_bitset;

    for (int row = 0; row < 7; row += 3) {
        for (int col = 0; col < 7; col += 3) {
            if (!valid_box_bitset(sudoku, row, col)) { return false; }
        }
    }

    for (int row = 0; row < 9; row++) {
        row_bitset = 0;
        for (int col = 0; col < 9; col++) {
            if (!bitset_is_unique(sudoku[row][col])) continue;
            if (bitset_is_set(row_bitset, sudoku[row][col]) || 
                bitset_is_set(cols_bitsets[col], sudoku[row][col]) || sudoku[row][col] == 0) { return false; }
            row_bitset += sudoku[row][col];
            cols_bitsets[col] += sudoku[row][col];
        }
    }

    return true;
}

bool error() 
{
    fprintf(stderr, "Sudoku is not valid!\n");
    return false;
}

bool solve(unsigned int sudoku[9][9])
{
    if (!needs_solving(sudoku)) return true;
    if (!is_valid(sudoku)) return error();
    
    bool row_changes = false, col_changes = false, box_changes = false, entering_while = true;

    while (row_changes || col_changes || box_changes || entering_while) {
        row_changes = false, col_changes = false, box_changes = false, entering_while = false;
        for (int row = 0; row < 9; row++) { row_changes = eliminate_row(sudoku, row) || row_changes; if (!is_valid(sudoku)) return false; }
        for (int col = 0; col < 9; col++) { col_changes = eliminate_col(sudoku, col) || col_changes; if (!is_valid(sudoku)) return false; }
        for (int row = 0; row < 7; row += 3) { 
            for (int col = 0; col < 7; col += 3) {
                box_changes = eliminate_box(sudoku, row, col) || box_changes; if (!is_valid(sudoku)) return false; 
            }
        }
    }

    if (needs_solving(sudoku)) return false; 
    if (!is_valid(sudoku)) return error();

    return true;
}

bool load_error(int input) 
{
    fprintf(stderr, "Failed to load input. ");
    switch (input) {
        case 1:
            fprintf(stderr, "Mistake in '+-------+-------+-------+' separator.\n");
            break;
        case 2:
            fprintf(stderr, "Position of '|' taken by another symbol.\n");
            break;
        case 3:
            fprintf(stderr, "Positon of 'New Line' separator taken by another symbol.\n");
            break;
        case 4:
            fprintf(stderr, "Positon of ' ' (space) taken by another symbol.\n");
            break;
        case 5:
            fprintf(stderr, "Positon of '.', '!' or '1-9' taken by another symbol.\n");
            break;
        case 6:
            fprintf(stderr, "One of symbols not in range '0-9'.\n");
            break;
        case 7:
            fprintf(stderr, "Input not long enough.\n");
            break;
        case 8:
            fprintf(stderr, "Input too long.\n");
            break;
        case 9:
            fprintf(stderr, "No valid input.\n");
            break;
    }
    return false;
}

bool load_long(unsigned int sudoku[9][9]) {
    bool valid = false;
    int row = 0, col = 0, abs_row = 0, abs_col;
    char break_line[] = "+-------+-------+-------+";
    char loaded;
    while (abs_row < 13) {
        switch (abs_row) {
            case 0:
                for (int i = 1; i < 25; i++) {
                    if (getchar() != break_line[i]) return load_error(1);
                }
                if (getchar() != '\n') return false;
                break;
            case 4:
            case 8:
            case 12:
                for (int i = 0; i < 25; i++) {
                    if (getchar() != break_line[i]) return load_error(1);
                }
                if (getchar() != '\n') return load_error(3);
                break;
            default:
                abs_col = 0;
                while (abs_col < 26) {
                    switch (abs_col) {
                        case 0:
                        case 8:
                        case 16:
                        case 24:
                            if (getchar() != '|') return load_error(2) ;
                            abs_col += 1;
                            break;
                        case 7:
                        case 15:
                        case 23:
                            if (getchar() != ' ') return load_error(4) ;
                            abs_col += 1;
                            break;
                        case 25:
                            if (getchar() != '\n') return load_error(3);
                            col = 0;
                            abs_col += 1;
                            row += 1;
                            break;
                        default:
                            if (getchar() != ' ') return load_error(4);
                            loaded = getchar();
                            if (loaded == '.' || loaded == '0') { sudoku[row][col] = 0x1FF; valid = true; }
                            if (loaded == '!') { sudoku[row][col] = 0; valid = true; }
                            if (loaded >= '1' && loaded <= '9') { sudoku[row][col] = (1 << (loaded - '1')); valid = true; }
                            if (!valid) return load_error(5);
                            col += 1;
                            abs_col += 2;
                            valid = false;
                            break;
                    }
                }
                break;
        }
        abs_row += 1;
    }
    return true;
}

unsigned int decrypter(char input) {
    if (input == '0') return 0x1FF;
    return 1 << (input - '1');
}

bool load_short(unsigned int sudoku[9][9], char firstchar) {
    int row = 0;
    int col = 1;
    sudoku[0][0] = decrypter(firstchar);
    char loaded;
    for (int i = 0; i < 80; i++) {
        loaded = getchar();
        if (loaded == '\n' || loaded == EOF) return load_error(7);
        if (loaded < '0' || loaded > '9') return load_error(6);
        sudoku[row][col] = decrypter(loaded);
        col += 1;
        if (col == 9) {col = 0; row += 1;}
    }
    loaded = getchar();
    if (loaded == EOF || loaded == '\n') return true;
    return load_error(8);
}

bool load(unsigned int sudoku[9][9])
{
    char loaded;
    while ((loaded = getchar()) != EOF) {
        if (loaded == '+') { return load_long(sudoku); }
        else if (loaded >= '0' && loaded <= '9') { return load_short(sudoku, loaded); }
        else if (loaded != '\n') return load_error(9);
    }
    return load_error(9);
}


void print(unsigned int sudoku[9][9])
{
    int translated;
    char break_line[] = "+-------+-------+-------+";

    for (int row_index = 0; row_index < 9; row_index++) {
        if (row_index == 0 || row_index == 3 || row_index == 6) { puts(break_line); }
        for (int col_index = 0; col_index < 9; col_index++) {

            if (col_index == 0) printf("|");
            if (col_index == 3 || col_index == 6) printf(" |");
            translated = translate_to_printable(sudoku[row_index][col_index]);
            switch (translated) {
                case -1:
                    printf(" !");
                    break;
                case -2:
                    printf(" .");
                    break;
                default:
                    printf(" %d", translated);
                    break;
            }
            
        }
        puts(" |");

    }
    puts(break_line);
}

/* ************************************************************** *
 *                              Bonus                             *
 * ************************************************************** */

void generate(unsigned int sudoku[9][9])
{
    if (!is_valid(sudoku)) return;
    unsigned int* raw = (unsigned int*) sudoku; unsigned int sudoku_copy[9][9];
    bool finished; unsigned int value_holder;
    int used_indexes[81] = {0};

    for (int i = 0; i < 81; i++) {
        if (!bitset_is_unique(raw[i])) { used_indexes[i] = 1; }
    }

    while (true) {
        finished = false;
        for (int i = 0; i < 82; i++) {
            if (i == 81) { finished = true; break; }
            if (used_indexes[i] == 0) break;
        }
        if (finished) break;

        int ran_index = rand() % 81;   // random index between 0 and 80 (81 is length of raw format for sudoku)
        if (used_indexes[ran_index] == 1) continue;
        used_indexes[ran_index] = 1;

        value_holder = raw[ran_index];
        raw[ran_index] = 0x1FF;
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                sudoku_copy[row][col] = sudoku[row][col];
            }
        }
        if (!solve(sudoku_copy)) raw[ran_index] = value_holder;
    }

}


#ifdef BONUS_GENERIC_SOLVE
bool generic_solve(unsigned int sudoku[9][9])
{
    return false; // todo
}
#endif
