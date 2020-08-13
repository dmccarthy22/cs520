#include <stdio.h>
#include <stdint.h>
//#include "getFP.s"
#include <stdbool.h>

long getFP();

int frameCount(){
    long h = getFP();

    int count = 0;
    long newfp;
    long old;
    old =h;
    while ((newfp = *((long *)old)) > old){
    
        old = newfp;
        count++;
    }
   // printf("%x\n", count);

    



    return count;

}