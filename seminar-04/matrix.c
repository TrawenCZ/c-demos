#include <stdlib.h>
#include <stdio.h>

void fill_matrix(int min_value, int max_value, size_t mrows, size_t mcols, int matrix[mrows][mcols]);
void print_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols]);

int find_max_min_in_array(size_t length, int *array, int *max, int *min)
{
    if (array == NULL || (max == NULL && min == NULL)) {
        return 0;
    }

    for (size_t i = 1; i < length; i++) {
        if (max && *max < array[i]) {
            *max = array[i];
        }

        if (min && *min > array[i]) {
            *min = array[i];
        }
    }
    return (max && min) ? 2 : 1;
}

void multiply_matrix(int mrows, int mcols, int matrix[mrows][mcols], int number) 
{
    for (int i = 0; i < mrows; i++) {
        for (int j = 0; j < mcols; j++) {
            matrix[i][j] *= number;
        }
    }
}

int main(void) {
    int mrows = 2;
    int mcols = 2;
    int test_matrix[mrows][mcols];
    int multiplier = 2;

    int max = test_matrix[0][0];
    int min = test_matrix[0][0];
    int* pMax = &max;
    int* pMin = &min;

    printf("matrix is pointing to %p with size of %zu\n", test_matrix, sizeof(test_matrix));
    printf("matrix[0] is pointing to %p with size of %zu\n", test_matrix[0], sizeof(test_matrix[0]));
    printf("matrix[1] is pointing to %p with size of %zu\n", test_matrix[1], sizeof(test_matrix[1]));
    printf("matrix[1][1] si pointing to %p with size of %zu\n", test_matrix[1][1], sizeof(test_matrix[1][1]));

    printf("Base matrix:\n");
    fill_matrix(1, 5, mrows, mcols, test_matrix);
    print_matrix(mrows, mcols, test_matrix);
    printf("Matrix multiplied by %d:\n", multiplier);
    multiply_matrix(mrows, mcols, test_matrix, multiplier);
    print_matrix(mrows, mcols, test_matrix);


    for (int i = 0; i < mrows; i++) {
        int* pointer = test_matrix[i];
        find_max_min_in_array(mcols, pointer, pMax, pMin);
    }
    printf("Max: %d, Min: %d\n", max, min);
    for (int i = 0; i < mrows; i++) {
        int* pointer = test_matrix[i];
        max = test_matrix[i][0];
        min = test_matrix[i][0];
        find_max_min_in_array(mcols, pointer, pMax, pMin);
        printf("Row no. %d, max: %d, min: %d\n", i+1, max, min);
    }
    return 0;
}


void fill_matrix(int min_value, int max_value, size_t mrows, size_t mcols, int matrix[mrows][mcols])
{
    if (max_value <= min_value) {
        return;
    }
    for (size_t i = 0; i < mrows; i++) {
        for (size_t j = 0; j < mcols; j++) {
            matrix[i][j] = rand() % (max_value - min_value) + min_value;
        }
    }
}

void print_matrix(size_t mrows, size_t mcols, int matrix[mrows][mcols])
{
    for (size_t i = 0; i < mrows; i++) {
        for (size_t j = 0; j < mcols; j++) {
            printf("%d\t", matrix[i][j]);
            
        }
        printf("\n");
    }
}
