#include <stdio.h>
#include <stdint.h>
//#include "framecount.c"
#include <stdbool.h>
//extern long getFP();
//extern int frameCount();
int frameCount();
int a();
int b();
int c();


int main()
{
    int fc = a(420);
    return 0;
}
int a(int g){
    g+=69;
    int x = b(g);
    return x;

}
int b(int g){
    
    g--;
   int x= c(g);
    return x;

}
int c(int g){

    return frameCount();
    printf("SAY HUH\n");


}
