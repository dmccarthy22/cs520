#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
int main(int argc, char *argv[])
{
#define ERR_TWO_ARGUMENTS "Requires 2 arguments, input file and output file"
#define ERR_INVALID_FILE "Cannot open file:"
#define ERR_OVERLONG_ENCODING "Overlong Encoding at offset:"
#define ERR_NONCHARACTER "The sequence decodes to a noncharacter: start byte at offset:"
#define ERR_RESERVED_SURROGATES "The sequence decodes to a value reserved for surrogates: start byte at offset:"
#define ERR_OUT_OF_RANGE "The sequence decodes to a value greater than 0x10FFFF: start byte at offset:"
#define ERR_INCOMPLETE_CODE_UNIT "Incomplete UTF-16 code unit at offset:"
#define ERR_UNPAIRED_SURROGATE "Unpaired surrogate at offset:"
#define ERR_MISSING_BOM "Missing BOM"
#define ERR_INCORRECT_START_BYTE "Incorrect start byte at offset:"
    FILE *fn;
    fn = fopen(argv[2], "w");
    if (argc != 3)
    {
        fprintf(stderr, "%d\n", ERR_TWO_ARGUMENTS);
        exit(-1);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("cannot open %s\n", argv[1]);
        exit(-1);
    }
    unsigned int bom = getc(fp);
    unsigned int bom2 = getc(fp);
    unsigned int byte = getc(fp);
    putc(0xEF, fn); //bom
    putc(0xBB, fn); //bom
    putc(0xBF, fn); //bom
    unsigned int combo;
    bool be = false; //big/little endian
    while (byte != EOF)
    {
        if (bom == 0xFE && bom2 == 0xFF)
        { //big endian
            unsigned int lead = byte;
            unsigned int tail = getc(fp);
            combo = lead << 8;
            combo = combo | tail;
            be = true;
        }
        else
        { //little endian
            unsigned int lead = getc(fp);
            unsigned int tail = byte;
            combo = lead << 8;
            combo = combo | tail;
        }
        if (combo >= 0xD800 && combo <= 0xDBFF) //2 bytes
        {
            unsigned int combo2;
            byte = getc(fp);
            if (be) //big endian
            {
                unsigned int lead = byte;
                unsigned int tail = getc(fp);
                combo2 = lead << 8;
                combo2 = combo2 | tail;
            }
            else
            {
                unsigned int lead = getc(fp);
                unsigned int tail = byte;
                combo2 = lead << 8;
                combo2 = combo2 | tail;
            }

            combo = combo - 0xD800;
            combo2 = combo2 - 0xDC00;
            unsigned int fin = (combo << 10) | combo2;
           // fin = fin | combo2;
            fin = fin + 0x10000;
            unsigned int a = 0xF0;      //11110
            unsigned int b = 0x02 << 6; //10
            unsigned int c = 0x02 << 6; //10
            unsigned int d = 0x02 << 6; //10
            unsigned int e = 0x3F;
            a = a | ((fin >> 18) & 0x07);
            b = b | ((fin >> 12) & e);
            c = c | ((fin >> 6) & e);
            d = d | (fin & e);
            putc(a, fn);
            putc(b, fn);
            putc(c, fn);
            putc(d, fn);
        }
        else
        {
            if (combo >> 7 == 0x00)
            {
                unsigned int a = 0x7F;
                combo = combo & a;
                putc(combo, fn);
            }
            else if (combo >> 11 == 0x00)
            {
                unsigned int a = 0x06 << 5;
                unsigned int b = 0x02 << 6;
                unsigned int c = 0x3f;

                unsigned int tot = combo >> 6 | a;
                combo = (combo & c) | b;
                putc(tot, fn);
                putc(combo, fn);
            }
            else if (combo >> 16 == 0x00)
            {
                unsigned int a = 0x0E << 4;
                unsigned int b = 0x02 << 6;
                unsigned int c = 0x02 << 6;
                a = a | combo >> 12;
                b = b | (combo >> 6 & 0x3F);
                c = c | (combo & 0x3F);
                putc(a, fn);
                putc(b, fn);
                putc(c, fn);
            }
        }
        byte = getc(fp);
    }
    fclose(fp);
    fclose(fn);
}