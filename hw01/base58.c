#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define BASE58_CHARS "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"

int ascii_to_base58(int ascii) {
    if (ascii <= 8) return (ascii + 49);
    if (ascii <= 16) return (ascii + 56);
    if (ascii <= 21) return (ascii + 57);
    if (ascii <= 32) return (ascii + 58);
    if (ascii <= 43) return (ascii + 64);
    return (ascii + 65);
}

int base58_to_ascii(int base58) {
    if (base58 <= 57) return (base58 - 49);
    if (base58 <= 72) return (base58 - 56);
    if (base58 <= 78) return (base58 - 57);
    if (base58 <= 90) return (base58 - 58);
    if (base58 <= 107) return (base58 - 64);
    return (base58 - 65);
} 

void printer(uint32_t baseconv) 
{
    uint8_t array[6];
    for (uint8_t i = 0; i < 6; i++) 
    {
        array[i] = ascii_to_base58(baseconv % 58);
        baseconv /= 58;
    }
    baseconv = 0;
    printf("%c%c%c%c%c%c", array[5], array[4], array[3], array[2], array[1], array[0]);
}

bool encode(void)
{   
    uint8_t counter = 4;
    uint32_t baseconv = 0;
    int16_t entry;
    while ((entry=getchar()) != EOF){
        counter--;
        baseconv <<= 8;
        baseconv += entry;
        if (counter == 0) 
        {
            counter = 4;

            printer(baseconv);
        }
    }
    if (counter < 4) 
    {
        baseconv <<= counter*8;
        printer(baseconv);
    }
    putchar('\n');
    return true;
}

bool decode(void)
{
    int8_t entry;
    uint8_t counter = 6;
    uint32_t baseconv = 0;
    uint16_t array[4];
    while ((entry=getchar()) != EOF) 
    {
        if (isspace(entry)) continue;

        if (strchr(BASE58_CHARS, entry) == NULL || entry == '\0') return false;
        counter--;
        baseconv *= 58;
        baseconv += base58_to_ascii(entry);
        if (counter == 0) {
            counter = 6;
            for (uint8_t i = 0; i < 4; i++) 
            {
                array[i] = baseconv & 0xFF;
                baseconv >>= 8;
            }
            baseconv = 0;
            printf("%c%c%c%c", array[3], array[2], array[1], array[0]);
        }
    }
    if (counter != 6) return false;
    return true;
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
