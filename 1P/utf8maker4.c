// generate some one-byte UTF-8 cases

#include <stdio.h>
#include <stdlib.h>

#define FILE_NAME "utf8-2"

int main()
{
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "cannot open %s\n", FILE_NAME);
        exit(-1);
    }

    // put out BOM
    putc(0xEF, fp);
    putc(0xBB, fp);
    putc(0xBF, fp);

    int i;

    for (int i = 0; i < 0xFF; i++){
        for (int j = 0; j < 0xFD; j++){
            putc(i, fp);
            putc(j, fp);
        }
    }
}