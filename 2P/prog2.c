static int counter(int);
//
// f2i: convert IEEE single-precision floating-point to
// 32-bit two's complement integer.
//
// i2f: convert 32-bit two's complement integer to IEEE
// single-precision floating point.
//
// s2d: convert IEEE single-precision floating-point to
// IEEE double-precision floating point.
//
// Remember when you implement these functions you may not use the
// float or double types. Everything must be done using only integer
// types.

#include <stdint.h>
#include <inttypes.h>
#include<stdio.h>


// f2i
//   Parameter: 32-bit value that should be interpreted as an IEEE
//     single-precision floating-point value.
//   Return value: 32-bit two's complement integer value produced by
//     converting the parameter to integer.
//   Note: This routine should duplicate what the Intel 64 hardware
//     does when converting floats to ints. Be sure to consider NaNs,
//     denormalized numbers, infinities, overflow, etc.
int32_t f2i(uint32_t in)
{

    unsigned int sign = in >> 31;
    //printf("%x\n", sign);
    unsigned int exponent = in << 1;
    exponent = exponent >> 24;
    unsigned int significand;
    significand = in << 9;
    
    significand = significand >> 9;
    //printf("%x\n", exponent);
    //printf("%x\n", exponent);
    if (exponent == 0){
        return 0;
    }
   
     else if (exponent == 0xFF){
        return 0x80000000;
    }
    else if (exponent < 0x7f){
      //  printf("LESS THAN 0x7f\n");
        return 0;
        
    }
    
    else if (exponent == 0xFFFFFFFF){
        return  0x80000000;
    }

    exponent = exponent - 127;
    
    if (exponent > 0x1e){
        return 0x80000000;
    }
   // printf("%d\n", exponent);

   
   // printf("%x\n", significand);
    int discard = 23 - exponent;
    unsigned int temp = significand;
    if (discard < 0){
        temp = temp | 0x800000;
        temp = temp << discard*-1;
    } else {
    temp = temp | 0x800000;
    temp = temp >> discard;
    }
    int32_t  fin = temp;
  // printf("%x\n", temp);
    if (sign == 1){

        return -fin;
    }
    else if (sign == 0){

        return fin;
    }
    return 0; // just a stub for now; always return zero
}
static int counter(int count)
{
    if ((count & 0x80000000) == 0x80000000)
    {
        return 1;
    }
    else
    {
        return 1 + counter(count << 1);
    }
}

// i2f
//   Parameter: 32-bit two's complement integer value.
//   Return value: 32-bit value that should be interpreted as an IEEE
//     single-precision floating-point value produced converting the
//     parameter to floating point.
//   Note: This routine should duplicate what the Intel 64 hardware
//     does when converting ints to floats.
uint32_t i2f(int32_t in)
{
    unsigned int original = in;
    if (original ==0){
        return 0;
    }

     int sign = (in & 0x80000000);      //mask to find sign
     if (sign == 0x80000000){
         original = ~original;
         original++;
     }
     int shift = counter(original);     //find first 0
     unsigned int a = original << shift;
     unsigned int significand  = (a & 0xFFFFFE00);
     significand = significand >>9;     //23 bits
     unsigned int guardstick = (a & 0x180);
     unsigned int sticky = (a & 0x7F);
     unsigned int exponent = 32-shift;
     exponent = exponent +127;
     if(guardstick == 384){
         significand++;
     }
     if (guardstick == 256){
         if (sticky == 0){
             if (significand%2 == 1){
                 significand++;
             }
         }
         if (sticky >0){
             significand++;
         }
     }
     //printf("%x\n", exponent);
     //printf("%x\n", significandshift);
     uint32_t fin = 0;
     fin += exponent <<23;
     fin += significand;
     if (sign == 0x80000000){
         fin += 0x80000000;
     }
     //printf("%x\n", fin);
     return fin;

         //     int exp = 0;
         //     int shift = 0;
         //     unsigned int temp = in <<1;

         //    // printf("%x\n", temp);
         //     int discard = 31-exp;

         //     unsigned int guardstick = in << shift;
         //     guardstick = guardstick>>shift;
         //     printf("%x\n", guardstick);
         //     exp = exp + 127;
         //     unsigned int guard = guardstick >> 4;
         //     printf("%x\n", guard);
         //     unsigned int sticky = guardstick << 2;
         //     sticky = sticky >> 2;
         //    // printf("%x\n", sticky);
         //     temp = temp << 2;
         //     temp = temp >> 2;

         //   //  printf("%x\n", temp);

         //     uint32_t exponent = exp;
         //     exponent = exponent << 23;
         //     printf("%x\n", exponent);
         //     uint32_t fin;
         //     fin= exponent | temp;
         //     if (sign == 1){
         //         fin = fin | 0x80000000;
         //     }
         //     printf("%x\n", fin);
         //     return fin;

         //return 0; // just a stub for now; always return all zero bits
}


//
// s2d
//   Parameter: 32-bit value that should be interpreted as an IEEE
//     single-precision floating-point value.
//   Return value: 64-bit value that should be interpreted as an IEEE
//     double-precision floating-point value produced by converting the
//	   single-precision value supplied

uint64_t s2d(uint32_t in)

{
    //printf("INPUT IS %x\n", in);
    uint64_t fin =0;        //final value
    int shift = 0;      //temporary
    uint64_t sign = in >> 31;
    uint64_t exp = in << 1;
    uint64_t stored;
    exp = exp >> 24;        //8 bits
    uint32_t significand = in & 0x7FFFFF;
  //  printf("EXP: %x\n", exp);
  //  printf("SIGNIFICAND: %x\n", significand);
    
    if (exp == 0){
        if (significand != 0){
            
            shift = counter(in<< 9);        //denormalized
            shift--;
           
        } else if (significand == 0){       //total 0
            fin = (sign << 63); 
            return fin;
        }
    }
    else if (exp == 0xFF){
        if (significand == 0){
            fin = ((sign << 63) | 0x7ff0000000000000); //sign with 111111111
        } else {
            uint64_t a = 0x7ff;
            uint64_t b = (uint64_t) significand;
            fin = ((sign<< 63) | (a << 52) |((b | (1<<22)) << 29)); //| with 1 bit before significand
        }
       // printf("my_number: %#018" PRIx64 "\n", fin);
        return fin;
    }
   
    uint32_t act = exp - 127;   
    stored = 1023+act;              //bias number for 64 bit
    stored = stored - shift;
   // printf("%x\n", stored);
    uint64_t a = (uint64_t) stored;
    uint64_t b = (uint64_t) significand;
    fin = ((sign << 63) | (a << 52) | (b << 29));
   // printf("my_number: %#018" PRIx64 "\n", fin);
    return fin;
    return 0;
}





// printf("INPUT IS: %x\n", in);
//     int shift = 0;
//     uint32_t original = in;
//     uint32_t sign = in >> 31;
//     unsigned int stored = 1;
//     unsigned int significand = 1;
//     printf("%x\n", sign);
   


//     unsigned int exp = in <<1;
//     printf("EXP: %x\n", exp);
//     exp = exp >> 23;
//     printf("EXP: %x\n", exp);
//    // exp = (exp && 0x1FF);
//     if (exp == 0){  
//         if (in << 9 ==0){               //TRUE ZERO
//         stored = 0;
//         significand = 0;
//         }
//         else {
//     shift = counter(in<< 9);      //DENORMALIZED
//     shift--;
//         }
//     }
    
//     if (significand != 0 && stored !=0){
//     shift = counter(in);

//     unsigned int act = exp - 127;
//     stored = 1023+act;
//     stored = stored - shift;
//     printf("%x\n", shift);
//     int sigshift = 9;
//     sigshift+= shift+1;
//     significand = significand << sigshift;
//     significand = (in & 0xFFFF);

    
//     printf("%x\n", significand);
//     printf("%x\n", stored);

//     }
   
//     uint64_t sign64 = (uint64_t) sign;
//     printf("my_number: %#018" PRIx64 "\n", sign64);
//     uint64_t exponent64 = (uint64_t)stored;
//     printf("my_number: %#018" PRIx64 "\n", exponent64);
//     uint64_t significand64 = (uint64_t) significand;
//     printf("my_number: %#018" PRIx64 "\n", significand64);

//     uint64_t fin = 0;
//     fin = ((sign64<< 63) | (exponent64 << 52) |(significand64 << 32));
//     printf("my_number: %#018" PRIx64 "\n", fin);

//     return fin;
//     return 0; // just a stub for now; always return zero