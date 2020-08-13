#include <stdio.h>
#include <stdlib.h>
int main (int argc, char *argv[]){

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
    
    if (argc != 3){
        fprintf(stderr, "%s %d\n", ERR_TWO_ARGUMENTS);
        exit(-1);
    }
    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL){
        printf("cannot open %s\n", argv[1]);
        exit(-1);
    }
    unsigned int bom1 = getc(fp);
    unsigned int bom2 = getc(fp);
    unsigned int bom3 = getc(fp);
    if(bom1!=0xEF){
        fprintf(stderr, "%d\n", ERR_MISSING_BOM);
        return -1;
    }
    if(bom2!=0xBB){
        fprintf(stderr, "%d\n", ERR_MISSING_BOM);
        return -1;
    }
    if(bom3!=0xBF){
        fprintf(stderr, "%d\n", ERR_MISSING_BOM);
        return -1;
    }
    unsigned int byte;
    putc(0xFE, fn);     //big endian bom
    putc(0xFF, fn);
    byte = getc(fp);
    while ((byte  != EOF))
    {

        if (byte >> 7 == 0){
            unsigned int a = 0x7F;    //01111111
            byte = byte & a;
            putc(0x00, fn);
            putc(byte, fn);
            
        }
       else if(byte >> 5 == 0x06){ //2 bytes
            unsigned int byte2 = getc(fp);
            unsigned int a = 0x1F;  //11111
            unsigned int b = 0x3F;  //111111
            byte = byte & a;
            byte2 = byte2 & b;
            byte = byte<< 6;
            unsigned int combo = byte | byte2;
            putc(combo>>8, fn);
            putc(combo & 0x00FF, fn);
        }
        else if (byte >> 4 == 0xE){ //3 byte
            unsigned int byte2 = getc(fp);
            unsigned int byte3 = getc(fp);
            unsigned int a = 0x0F;  //1111
            unsigned int b = 0x3F; //0x111111
            byte = byte & a;
            byte2 = byte2 & b;
            byte3 = byte3 & b;
            unsigned int combo = byte << 12;
            unsigned int h = byte2 << 6;

            combo = combo | h;
            combo = combo | byte3;
            putc(combo >> 8, fn);
            putc(combo, fn);
        }
        else if (byte>>3 == 0x1E){  // 4 byte
            unsigned int byte2= getc(fp);
            unsigned int byte3 = getc(fp);
            unsigned int byte4 = getc(fp);
            unsigned int a = 0x07;  //111
            unsigned int b = 0x3F; //111111
            byte = (byte & a) <<18;
            byte2 = (byte2 & b)<<12;
            byte3 = (byte3 & b) <<6;
             byte4= byte4 & b;
            // byte = byte<<18;
            // byte2 = byte2<<12;
            // byte3 = byte3<<6;
            unsigned int combo = byte | byte2 | byte3 | byte4;
            combo = combo - 0x10000;    //subtract
            unsigned int top = (combo >> 10) & 0x3FF;
            top = top + 0xD800;     //add to leading
            unsigned int bottom = combo & 0x0003FF;
            bottom = bottom + 0xDC00;   //add to trailing
            putc(top >> 8, fn);
            putc(top & 0x00FF, fn);
            putc(bottom >> 8, fn);
            putc(bottom & 0x00FF, fn);
        }
        byte = getc(fp);
       }
    fclose(fp);
    fclose(fn);
    return 0;
}