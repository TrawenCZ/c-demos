#include <stdio.h>
#include <stdlib.h>


int find_max_in_array(int array[10]) {
    int maximum = array[0];
    for (int i = 1; i != 10; i++) {
        if (array[i] > maximum) {
            maximum = array[i];
        }
    }
    return maximum;
}

int find_min_in_array(size_t length, int *array) {
    int minimum = array[0];
    for (int i = 1; i != length; i++) {
        if (array[i] < minimum) {
            minimum = array[i];
        }
    }
    return minimum;
}

int find_sum_in_array(size_t length, int *array) {
    int total = 0;
    for (int i = 0; i != length; i++) {
        total += array[i];
    }
    return total;
}

int gcd(int a, int b) {
    int divisor = 1;
    for (int i = 2; i != a && i != b; i++) {
        if (a % i == 0 && b % i == 0) {
            divisor = i;
        }
    }
    return divisor;
}

int find_gcd_in_array(size_t length, int *array) {
    int divisor = 1;
    if (find_min_in_array(length, array) == 1) return 1;
    for (int i = 2; i <= find_min_in_array(length, array); i++) {
        for (int a = 0; a != length; a++) {
            if (array[a] % i != 0) {break;}
            if (a == length - 1) {divisor = i;}
        }
    }
    return divisor;
}

int find_max_min_in_array(size_t length, int *array, int *max, int *min) {
    if ((min == NULL && max == NULL) || array == NULL) return 0;
    if (min == NULL && max != NULL) {
        *max = array[0];
        for (int i = 1; i != 10; i++) {
            if (array[i] > *max) {*max = array[i];}
        }
        return 1;
        }
    if (max == NULL && max != NULL) {
        *min = array[0];
        for (int i = 1; i != 10; i++) {
            if (array[i] < *min) {*min = array[i];}
        }
        return 1;
    }
    *min = array[0];
    *max = array[0];
    for (int i = 1; i != 10; i++) {
        if (array[i] < *min) {*min = array[i];}
        if (array[i] > *max) {*max = array[i];}
    }
    return 2;
}

int main(void)
{
    printf("Zadejte 10 cislic: ");
    int array[10], num;
    int total = 0;
    
    for (int i = 0; i != 10; i++) {
        scanf("%d", &num);
        array[i] = num;
        total += num;
    }
    printf("Array: [");
    for (int i = 0; i != 10; i++) {
        if (i != 9) {
        printf("%d, ", array[i]);
        }
        else {
            printf("%d]\n", array[i]);
        }
    }
    int max, min;
    int* pMax = &max;
    int* pMin = &min;
    find_max_min_in_array(10, array, pMax, pMin);
    printf("Sum: %d\n", total);
    printf("Max: %d; Min: %d\n", max, min);
    
    int gcd = find_gcd_in_array(10, array);
    printf("GCD: %d; ", gcd);
    printf("Factors: [");
    for (int i = 0; i != 10; i++) {
        if (i != 9) {
            printf("%d, ", array[i] / gcd);
        }
        else {
            printf("%d]\n", array[i] / gcd);
        }
    }
    return 0;
}
