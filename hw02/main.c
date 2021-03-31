/*
 * Placing interface header here checks for header selfsufficiency
 */
#include "sudoku.h"

/*
 * General standard headers
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Pseudorandom support (bonus)
 */
#include <time.h>

/*
 * Color support depends on unix-like terminals, therefore it is disabled
 * by default.
 *
 * Yet, if you ever wondered how the terminal colors work... this is how:
 * - enable this code with #define ENABLE_COLOR between this comment
 * and the #if below
 * - compile on Linux/Mac/... WSL should work as well
 *
 * This changes the exact output of the program - so do not enable unless
 * you have successfully passed hw02.
 */
#if (\
    defined(ENABLE_COLOR) \
    && (defined(__linux__) || defined(__unix__) || defined(__APPLE__)) \
)
    #define RED     "\x1b[31m"
    #define GREEN   "\x1b[32m"
    #define YELLOW  "\x1b[33m"
    #define BLUE    "\x1b[34m"
    #define MAGENTA "\x1b[35m"
    #define CYAN    "\x1b[36m"
    #define RESET   "\x1b[0m"
#else
    #define RED
    #define GREEN
    #define YELLOW
    #define BLUE
    #define MAGENTA
    #define CYAN
    #define RESET
#endif

static const int SILENCE_NO_REPORT = 1;
static const int SILENCE_NO_RESULT = 2;


/**
 * This function should provide number stored in the corresponding cell of the
 * sudoku.
 *
 * @param sudoku - 2D array[row][col] of cell bitsets (int used as set of bits)
 *
 * @return - number stored in the cell bitset, if unique,
 *         - 0 for cases without unique number set
 *
 * NOTE: @c static means the function will not be visible outside this
 * file.c (+-). Use static for your helpers and auxiliaries.
 */
static int get_number(unsigned int sudoku[9][9], int row, int col)
{
    // TODO
    fprintf(stderr, "get_number() is not implemented by default\n");
    abort();
}

static void raw_print(unsigned int sudoku[9][9])
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            int n = get_number(sudoku, row, col);
            putchar('0' + n);
        }
    }
    putchar('\n');
}

static void print_binary(int n)
{
    for (int i = 8; i >= 0; --i) {
        putchar((n & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

static void usage(const char *program)
{
    const char *help =
            "Usage: %s OPTIONS\n"
            "\n"
            "This program loads a sudoku and takes following options:\n"
            "\n"
            "\t--help\t\tShow this helpful message (no sudoku loaded)\n"
            "\t--silent\tSuppress non-print output. Repeat for more silence.\n"
            "\n"
            "\t--raw\t\tPrint loaded sudoku without formatting\n"
            "\t--print\t\tPrint loaded sudoku in ASCII grid\n"
            "\t--cell\t\tPrint sudoku cell set value in binary\n"
            "\n"
            "\t--solve\t\t\"Solve\" sudoku using elimination only (no backtracking)\n"
#if defined(BONUS_GENERIC_SOLVE)
            "\t--generic-solve\tGeneric solver of any sudoku\n"
#endif
#if defined(BONUS_GENERATE)
            "\t--generate\tGenerate sudoku - remove digits as long as \"solvable\"\n"
            "\t--seed SEED\tInitialize pseudorandom number generator (deterministic rand)\n"
#endif
            "\n"
            "\t--check-valid\tCheck if the sudoku is (currently) valid\n"
            "\t--needs-solving\tCheck if the valid sudoku is solved\n"
            "\n"
            "\t--eliminate-row ROW\tEliminate the row\n"
            "\t--eliminate-col COL\tEliminate the column\n"
            "\t--eliminate-box RC\tEliminate the box (row 0 col 8 - 08)\n"
            "\n";

    printf(help, program, program);
}

static void eliminate_row_demo(unsigned int sudoku[9][9], const char *optarg, int silent)
{
    char *endptr = NULL;
    int row = strtol(optarg, &endptr, 10);
    if (*endptr != '\0' || endptr == optarg) {
        fprintf(stderr, "Invalid row index %s\n", optarg);
        return;
    }

    if (row < 0 || row > 8) {
        fprintf(stderr, "Row %d is out of bounds\n", row);
        return;
    }

    if (silent < SILENCE_NO_REPORT)
        printf(BLUE "\nELIMINATE ROW %i\n" RESET, row);

    eliminate_row(sudoku, row);
}

static void eliminate_col_demo(unsigned int sudoku[9][9], const char *optarg, int silent)
{
    char *endptr = NULL;
    int col = strtol(optarg, &endptr, 10);
    if (*endptr != '\0' || endptr == optarg) {
        fprintf(stderr, "Invalid column index %s\n", optarg);
        return;
    }

    if (col < 0 || col > 8) {
        fprintf(stderr, "Column %d is out of bounds\n", col);
        return;
    }

    if (silent < SILENCE_NO_REPORT)
        printf(BLUE "\nELIMINATE COL %i\n" RESET, col);

    eliminate_col(sudoku, col);
}

static void eliminate_box_demo(unsigned int sudoku[9][9], const char *optarg, int silent)
{
    char *endptr = NULL;
    int pos = strtol(optarg, &endptr, 10);
    if (*endptr != '\0' || endptr == optarg) {
        fprintf(stderr, "Invalid box index %s\n", optarg);
        return;
    }

    int row = pos / 10;
    int col = pos % 10;

    if ((row > 10) || (row % 3 != 0) || (col % 3 != 0)) {
        fprintf(stderr, "Coordinate %d is not a valid box coord\n", pos);
        return;
    }

    if (silent < SILENCE_NO_REPORT)
        printf(BLUE "\nELIMINATE BOX [%i,%i]\n" RESET, row, col);

    eliminate_box(sudoku, row, col);
}

static void print_cell_value(unsigned int sudoku[9][9], const char *optarg, int silent)
{
    char *endptr = NULL;
    int pos = strtol(optarg, &endptr, 10);
    if (*endptr != '\0' || endptr == optarg) {
        fprintf(stderr, "Invalid cell index %s\n", optarg);
        return;
    }

    int row = pos / 10;
    int col = pos % 10;

    if (row < 0 || row > 8) {
        fprintf(stderr, "Row %d is out of bounds\n", row);
        return;
    }

    if (col < 0 || col > 8) {
        fprintf(stderr, "Column %d is out of bounds\n", col);
        return;
    }

    if (silent < SILENCE_NO_REPORT)
        printf(BLUE "\nPRINT BINARY SET OF CELL [%i,%i]\n" RESET, row, col);
 
    print_binary(sudoku[row][col]);
}

static void init_rand(const char *optarg)
{
    char *endptr = NULL;
    int seed = strtol(optarg, &endptr, 10);

    if (*endptr != '\0' || endptr == optarg) {
        fprintf(stderr, "Invalid seed %s\n", optarg);
        return;
    }

    srand(seed);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    unsigned int sudoku[9][9];
    // intentionaly commented out so that valgrind can detect uninitialized access
    // memset(sudoku, 0, sizeof(sudoku));

    bool valid_load = false;
    int silent = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "--load") == 0) {
            valid_load = true;
        } else if (strcmp(argv[i], "--LOAD") == 0) {
            valid_load = true;
        } else if (strcmp(argv[i], "--silent") == 0) {
            ++silent;
        }
    }

    if (!valid_load) {
        if (silent < SILENCE_NO_REPORT)
            puts(MAGENTA "LOAD" RESET);
        if (!load(sudoku)) {
            if (silent < SILENCE_NO_RESULT)
                puts(RED "FAILED" RESET);
            return EXIT_FAILURE;
        }
    }

    for (int argi = 1; argi < argc; ++argi) {
        // skip empty options and non-options

        const char *option = argv[argi];
        const char *optarg = argv[argi + 1];

        if (strcmp(option, "--raw") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(YELLOW "\nRAW:" RESET);

            raw_print(sudoku);
        } else if (strcmp(option, "--print") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(YELLOW "\nPRINT:" RESET);

            print(sudoku);
        } else if (strcmp(option, "--cell") == 0) {
            if (argi + 1 >= argc) {
                fprintf(stderr, "Missing option argument for %s\n", option);
                return EXIT_FAILURE;
            }
            ++argi;

            print_cell_value(sudoku, optarg, silent);
        } else if (strcmp(option, "--solve") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(BLUE "\nSOLVE" RESET);

            bool done = solve(sudoku);
            if (silent < SILENCE_NO_RESULT)
                puts(done ? GREEN "SOLVED" RESET : RED "FAILED" RESET);
#if defined(BONUS_GENERATE)
        } else if (strcmp(option, "--generate") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(BLUE "\nGENERATE" RESET);

            generate(sudoku);
#endif
#if defined(BONUS_GENERIC_SOLVE)
        } else if (strcmp(option, "--generic-solve") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(BLUE "\nGENERIC_SOLVE" RESET);

            bool done = generic_solve(sudoku);
            if (silent < SILENCE_NO_RESULT)
                puts(done ? GREEN "SOLVED" RESET : RED "FAILED" RESET);
#endif
        } else if (strcmp(option, "--needs-solving") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts("\nCHECK NEEDS_SOLVING");

            if (silent < SILENCE_NO_RESULT)
                puts(needs_solving(sudoku) ? "NEEDS SOLVING" : "COMPLETE");
        } else if (strcmp(option, "--check-valid") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(CYAN "\nCHECK VALID" RESET);

            if (silent < SILENCE_NO_RESULT)
                puts(is_valid(sudoku) ? GREEN "OK" RESET : RED "FAIL" RESET);
        } else if (strcmp(option, "--eliminate-row") == 0) {
            if (argi + 1 >= argc) {
                fprintf(stderr, "Missing option argument for %s\n", option);
                return EXIT_FAILURE;
            }
            ++argi;

            eliminate_row_demo(sudoku, optarg, silent);
        } else if (strcmp(option, "--eliminate-col") == 0) {
            if (argi + 1 >= argc) {
                fprintf(stderr, "Missing option argument for %s\n", option);
                return EXIT_FAILURE;
            }
            ++argi;

            eliminate_col_demo(sudoku, optarg, silent);
        } else if (strcmp(option, "--eliminate-box") == 0) {
            if (argi + 1 >= argc) {
                fprintf(stderr, "Missing option argument for %s\n", option);
                return EXIT_FAILURE;
            }
            ++argi;

            eliminate_box_demo(sudoku, optarg, silent);
        } else if (strcmp(option, "--seed") == 0) {
            if (argi + 1 >= argc) {
                fprintf(stderr, "Missing option argument for %s\n", option);
                return EXIT_FAILURE;
            }
            ++argi;

            init_rand(optarg);
        } else if (strcmp(option, "--load") == 0) {
            if (silent < SILENCE_NO_REPORT)
                puts(MAGENTA "LOAD" RESET);

            if (!load(sudoku)) {
                if (silent < SILENCE_NO_RESULT)
                    puts(RED "FAILED" RESET);
            }
        } else if (strcmp(option, "--silent") == 0) {
            ; // nop
        } else {
            fprintf(stderr, "Unknown option `%s'.\n", option);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
