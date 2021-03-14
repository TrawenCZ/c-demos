#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char ascii_to_base58(int ascii) {
    if (ascii <= 8) return (ascii + '1');
    if (ascii <= 16) return (ascii + 56);
    if (ascii <= 21) return (ascii + 57);
    if (ascii <= 32) return (ascii + 58);
    if (ascii <= 43) return (ascii + 'A' - 1);
    if (ascii > 43) return (ascii + 'A');
}

bool encode(void)
{   
    int counter = 4;
    int32_t baseconv = 0;
    int8_t entry;
    char prnt1;
    char prnt2;
    char prnt3;
    char prnt4;
    char prnt5;
    char prnt6;
    while ((entry=getchar()) != EOF){
        counter--;
        baseconv <<= 8;
        baseconv += entry;
        if (counter == 0) {
            counter = 4;

            prnt1 = ascii_to_base58(baseconv % 58);
            baseconv /= 58;
            prnt2 = ascii_to_base58(baseconv % 58);
            baseconv /= 58;
            prnt3 = ascii_to_base58(baseconv % 58);
            baseconv /= 58;
            prnt4 = ascii_to_base58(baseconv % 58);
            baseconv /= 58;
            prnt5 = ascii_to_base58(baseconv % 58);
            baseconv /= 58;
            prnt6 = ascii_to_base58(baseconv % 58);

            baseconv = 0;
            printf("%c%c%c%c%c%c", prnt6, prnt5, prnt4, prnt3, prnt2, prnt1);
        }
    }
    if (counter < 4) {
        baseconv <<= counter*8;
        prnt1 = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
        prnt2 = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
        prnt3 = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
        prnt4 = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
        prnt5 = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
        prnt6 = ascii_to_base58(baseconv % 58);

        printf("%c%c%c%c%c%c", prnt6, prnt5, prnt4, prnt3, prnt2, prnt1);
    }
    putchar('\n');
    return true;
}

bool decode(void)
{
    
    return false;
}

// ================================
// DO NOT MODIFY THE FOLLOWING CODE
// ================================
int main(int argc, char **argv)
{
    if ((argc == 1) || (argc == 2 && !strcmp(argv[1], "-e"))) {
        encode();
    } else if (argc == 2 && !strcmp(argv[1], "-d")) {
        if (!decode()) {
            fprintf(stderr, "Input isn't encoded via Base58!\n");
            return EXIT_FAILURE;
        }
    } else {
        fprintf(stderr, "Invalid switch, use -e or -d\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
