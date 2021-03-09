#include <stdio.h>


void draw_line(int length) {
    while(length != 0) {
        putchar('#');
        length--;
    }
    putchar('\n');
}

void draw_square(int size) {
    int length = size;
    while(length != 0) {
        for (int i = size; i != 0; i--) {
            putchar('#');
            putchar('#');
        }
        putchar('\n');
        length--;
    }
    putchar('\n');
}

void draw_rectangle(int a, int b, char fill) {
    while(a != 0) {
        for (int i = b; i != 0; i--) {
            putchar(fill);
        }
        putchar('\n');
        length--;
    }
    putchar('\n');
}

int main(void)
{
    printf("Zde se bude kreslit!\nVložte číslo odpovídající typu obrazce (čára = 1, čtverec = 2, obdelník = 3, kruh = 4, elipsa = 5):\n");

    int x = 0;
    while ((scanf("%d", &x) != 1) || x < 1 || x > 5) {
        printf("Read failed. Try again.\nVložte číslo odpovídající typu obrazce (čára = 1, čtverec = 2, obdelník = 3, kruh = 4, elipsa = 5):\n");
        scanf("%*s");
    }
    switch (x) {
        
    }

    return 0;
}
