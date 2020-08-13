#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: lab1 filename\n");
        exit(-1);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL){
        printf("cannot open %s\n", argv[1]);
        exit(-1);
    }
    int counter = 0;
    int ch = getc(fp);
    printf("%08d", counter);
    printf(" %02x", ch);
    counter++;
    ch = getc(fp);
    while (ch != EOF){

        if (counter%16 == 0){
            printf("\n%08d", counter);
        }
        
        printf(" %02x", ch);
        counter++;
        ch = getc(fp);
        //printf("%d", ch);
    }
    printf("\n%08d\n", counter);
    return 0;
}