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
        a--;
    }
    putchar('\n');
}

void draw_circle(int radius, char fill, char space) {
    for (int y = radius; y >= -radius; y--) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                putchar(fill);
                putchar(fill);
            }
            else {
                putchar(space);
                putchar(space);
            }
        }
        putchar('\n');
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
        case 1:
            printf("Vložte rozměr čáry:\n");
            int a = 0;
            while ((scanf("%d", &a) != 1) || a < 0) {
                printf("Read failed. Try again.\nVložte číslo znovu:\n");
                scanf("%*s");
            }
            draw_line(a);
            break;
        case 2:
            printf("Vložte rozměr čtverce:\n");
            int size = 0;
            while ((scanf("%d", &size) != 1) || size < 0) {
                printf("Read failed. Try again.\nVložte číslo znovu:\n");
                scanf("%*s");
            }
            draw_square(size);
            break;
        case 3:
            printf("Vložte výšku obdelníku:\n");
            int height = 0;
            while ((scanf("%d", &height) != 1) || height < 0) {
                printf("Read failed. Try again.\nVložte číslo výšky znovu:\n");
                scanf("%*s");
            }
            printf("Vložte šířku obdelníku:\n");
            int width = 0;
            while ((scanf("%d", &width) != 1) || width < 0) {
                printf("Read failed. Try again.\nVložte číslo šířky znovu:\n");
                scanf("%*s");
            }
            printf("Vložte výplň obdelníku:\n");
            char fill = ' ';
            while (scanf(" %c", &fill) != 1) {
                printf("Read failed. Try again.\nVložte výplň znovu:\n");
                scanf("%*s");
            }
            draw_rectangle(height, width, fill);
            break;
        case 4:
            printf("Vložte poloměr kružnice:\n");
            int radius = 0;
            while ((scanf("%d", &radius) != 1) || radius < 0) {
                printf("Read failed. Try again.\nVložte číslo výšky znovu:\n");
                scanf("%*s");
            }

            printf("Vložte výplň kruhu:\n");
            int fill1 = ' ';
            while (scanf(" %c", &fill1) != 1) {
                printf("Read failed. Try again.\nVložte výplň znovu:\n");
                scanf("%*s");
            }

            printf("Vložte výplň okrajů:\n");
            char space = ' ';
            while (scanf(" %c", &space) != 1) {
                printf("Read failed. Try again.\nVložte výplň znovu:\n");
                scanf("%*s");
            }

            draw_circle(radius, fill1, space);
            break;
        case 5:
            break;
    }

    return 0;
}
