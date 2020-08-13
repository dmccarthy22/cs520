#include <stdint.h>
#include<inttypes.h>
#include <stdio.h>
//#include "prog2.c"

int main (int argc, char *argv[]){

    int32_t a = 0xc44444;

    uint32_t out = s2d(a);

    printf("%s08" PRIx32, 0xc44444);
    printf("\n");
    return 0;

}