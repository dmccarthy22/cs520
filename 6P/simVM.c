#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "simVM.h"



void updateTLB(void *, int);
void updatePageTable(void *, int);
void diskWrite(void *);

typedef struct
{
    unsigned int sizeVM;    //SIze of virtual memory in pages
    unsigned int sizePM;   // size of the physical memory in pages
    unsigned int pageSize; // size of a page in words
    unsigned int sizeTLB;  // number of translation lookaside buffer entries
    char pageReplAlg;      // page replacement alg.: 0 is Round Robin, 1 is LRU
    char tlbReplAlg;       //page replacement alg.: 0 is Round Robin, 1 is LRU
    unsigned int *TLB; //Translation Lookaside buffer
    unsigned int *pt;       //page table
    unsigned int *VMdata;       
    unsigned int *clockTLB;     //clock for TLB
    unsigned int *clockPT;      //Clock for pt
    unsigned int *dirty; //0 if clean, 1 if dirty
    unsigned int ptLRU; //LRU Index
    int currentClock;       //incremented each read and right
    int diskWrites;     //number of disk writes
    int ptRR;           //Only index for page table: depends on pageReplAlg
    int tlbRR;          //Only index for TLB: depends on tlbReplAlg
    int pageFaults;     //Number of pagefaults
    int TLBMiss;        //Number of TLB misses

} VM;

void *createVM(
    unsigned int sizeVM,   // size of the virtual memory in pages
    unsigned int sizePM,   // size of the physical memory in pages
    unsigned int pageSize, // size of a page in words
    unsigned int sizeTLB,  // number of translation lookaside buffer entries
    char pageReplAlg,      // page replacement alg.: 0 is Round Robin, 1 is LRU
    char tlbReplAlg        // TLB replacement alg.: 0 is Round Robin, 1 is LRU
)
{
    if (sizeVM < sizePM)
    {
        fprintf(stderr, "error physical memory larger then virtual memory\n");
        exit(-1);
    }
    if (pageSize % 2 != 0)
    {
        fprintf(stderr, "page size must be power of 2\n");
        exit(-1);
    }
    if (sizeVM * pageSize > 4294967296)
    {
        fprintf(stderr, "error too large\n");
        exit(-1);
    }
    if (sizeTLB > sizePM)
    {
        fprintf(stderr, "TLB size must be less than or equal to physical memory size\n");
        exit(-1);
    }

    VM *vm = malloc(sizeof(VM));
    if (vm == NULL)
    {
        exit(-1);
    }
    vm->currentClock = 0;
    vm->sizeVM = sizeVM;
    vm->sizePM = sizePM;
    vm->pageSize = pageSize;
    vm->TLBMiss = 0;
    vm->pageFaults = 0;
    vm->sizeTLB = sizeTLB;
    vm->pageReplAlg = pageReplAlg;
    vm->tlbReplAlg = tlbReplAlg;
    vm->ptLRU = 0;
    vm->ptRR = 0;
    vm->tlbRR = 0;
    vm->diskWrites = 0;
    vm->TLB = malloc(sizeTLB * sizeof(unsigned int));
    if (vm->TLB == NULL)
    {
        exit(-1);
    }
    vm->pt = malloc(sizePM * sizeof(unsigned int));
    if (vm->pt == NULL)
    {
        exit(-1);
    }
    vm->VMdata = malloc(sizeVM * pageSize * sizeof(unsigned int));
    if (vm->VMdata == NULL)
    {
        exit(-1);
    }
    for (int i = 0; i < sizePM; i++)
    {
        vm->pt[i] = i;
    }
    for (int i = 0; i < sizeTLB; i++)
    {
        vm->TLB[i] = i;
    }

    vm->clockTLB = malloc(sizeTLB * sizeof(unsigned int));
    for (int i = 0; i < sizeTLB; i++)
    {
        vm->clockTLB[i] = 0;
    }
    vm->clockPT = malloc(sizePM * sizeof(unsigned int));
    for (int i = 0; i < sizePM; i++)
    {
        vm->clockPT[i] = 0;
    }
    vm->dirty = malloc(sizePM * sizeof( int));
    for (int i = 0; i < sizePM; i++)
    {
        vm->dirty[i] = 0;
    }

    return vm;
}

void diskWrite(void *handle)
{

    VM *vm = (VM *)handle;
    if (vm->dirty[vm->ptRR] == 1)
    {
        vm->diskWrites++;
        vm->dirty[vm->ptRR] = 0;
        }
    
}

void updateTLB(void *handle, int mapping)
{

    VM *vm = (VM *)handle;
    int alg = (int)vm->tlbReplAlg;
    if (alg == 0)
    {
        vm->TLB[vm->tlbRR] = mapping;
        vm->tlbRR++;

        if (vm->tlbRR == vm->sizeTLB)
        {
            vm->tlbRR = 0;
        }
    }
    else
    {
        int smallest = vm->clockTLB[0];
        int pos = 0;
        for (int i = 0; i < vm->sizeTLB; i++)
        {

            if (smallest > vm->clockTLB[i])
            {
                pos = i;
                smallest = vm->clockTLB[i];
            }
        }
        vm->tlbRR = pos;
        vm->TLB[pos] = mapping;
        vm->clockTLB[pos] = vm->currentClock;
    }
}

void updatePageTable(void *handle, int mapping)
{
    VM *vm = (VM *)handle;
    int alg = (int)vm->pageReplAlg;
    if (alg == 0)
    {
        vm->pt[vm->ptRR] = mapping;
        vm->ptRR++;
        if (vm->ptRR == vm->sizePM)
        {
            vm->ptRR = 0;
        }
    }
    else
    {
        int smallest = vm->clockPT[0];
        int pos = 0;
        for (int i = 0; i < vm->sizePM; i++)
        {

            if (smallest > vm->clockPT[i])
            {
                pos = i;
                smallest = vm->clockPT[i];
            }
        }
        vm->ptLRU = pos;
        vm->ptRR = pos;
        vm->pt[pos] = mapping;
        vm->clockPT[pos] = vm->currentClock;
    }
}

int findSmallest(void * handle){
    VM *vm = (VM *)handle;
    int smallest = vm->clockPT[0];
    int pos = 0;
    for (int i = 0; i < vm->sizePM; i++)
    {

        if (smallest > vm->clockPT[i])
        {
            pos = i;
            smallest = vm->clockPT[i];
        }
    }
    return pos;
}

int readInt(void *handle, unsigned int address)
{
    VM *vm = (VM *)handle;
    int mapping = address / vm->pageSize;
    vm->currentClock++;     //increment clock
    for (int i = 0; i < vm->sizeTLB; i++)   //search through tlb
    {
        if (vm->TLB[i] == mapping)
        {
            vm->clockTLB[i] = vm->currentClock;
            for (int j = 0; j < vm->sizePM; j++)       //physical memory
            {
                if (vm->pt[j] == mapping)
                {
                    vm->clockPT[j] = vm->currentClock;
                }
            }
            return vm->VMdata[address];     //successful find in tlb
        }
    }
    vm->TLBMiss++;
    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            updateTLB(vm, mapping);
            vm->clockPT[i] = vm->currentClock;  //set clock
            return vm->VMdata[address];         //successful find in Physical memory
        }
    }
    vm->pageFaults++;
    vm->ptRR = findSmallest(vm);        //Set index to LRU index
    diskWrite(vm);      //need to check if dirty, no dirty bits made in read
    updatePageTable(vm, mapping); //update pagetable
    updateTLB(vm, mapping);     //update TLB
    
    

    return vm->VMdata[address];
}

float readFloat(void *handle, unsigned int address)
{
    //same as readInt, but must typecast
    VM *vm = (VM *)handle;
    int mapping = address / vm->pageSize;
    vm->currentClock++;
    for (int i = 0; i < vm->sizeTLB; i++)
    {
        if (vm->TLB[i] == mapping)
        {
            vm->clockTLB[i] = vm->currentClock;
            for (int j = 0; j < vm->sizePM; j++)
            {
                if (vm->pt[j] == mapping)
                {
                    vm->clockPT[j] = vm->currentClock;
                }
            }
            return *(float*)&vm->VMdata[address];
        }
    }
    vm->TLBMiss++;
    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            updateTLB(vm, mapping);
            vm->clockPT[i] = vm->currentClock;
            return *(float*)&vm->VMdata[address];
        }
    }
    vm->pageFaults++;
    vm->ptRR = findSmallest(vm);
    diskWrite(vm);
    updateTLB(vm, mapping);
    updatePageTable(vm, mapping);
   

    return *(float*)&vm->VMdata[address];
}

void writeInt(void *handle, unsigned int address, int value)
{
    VM *vm = (VM *)handle;
    int mapping = address / vm->pageSize;
    vm->currentClock++;     //increments each read+write
    for (int i = 0; i < vm->sizeTLB; i++)
    {
        if (vm->TLB[i] == mapping)
        {
            vm->clockTLB[i] = vm->currentClock;
            for (int j = 0; j < vm->sizePM; j++)
            {
                if (vm->pt[j] == mapping)
                {
                    vm->dirty[j] = 1;       //page is changed, set bit to dirty
                    vm->clockPT[j] = vm->currentClock;  //set clock
                }
            }
            vm->VMdata[address] = value;        //successful find 
            return;
        }
    }
    vm->TLBMiss++;
    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            updateTLB(vm, mapping);
            vm->clockPT[i] = vm->currentClock;
            vm->dirty[i] = 1;           //page is changed set bit to dirty

            vm->VMdata[address] = value;
            return;
        }
    }
    vm->pageFaults++;
    vm->ptRR = findSmallest(vm);        //update the pt index to LRU
    diskWrite(vm);                      //check if bit is dirty, increment diskwrite
    updateTLB(vm, mapping);             //update the TLB
    updatePageTable(vm, mapping);       //update page table
   

    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            vm->dirty[i] = 1;
        }
    }

    vm->VMdata[address] = value;
    return;
}

void writeFloat(void *handle, unsigned int address, float value)
{ //same writeInt but must typecast
    VM *vm = (VM *)handle;
    int mapping = address / vm->pageSize;
    vm->currentClock++;
    for (int i = 0; i < vm->sizeTLB; i++)
    {
        if (vm->TLB[i] == mapping)
        {
            vm->clockTLB[i] = vm->currentClock;
            for (int j = 0; j < vm->sizePM; j++)
            {
                if (vm->pt[j] == mapping)
                {
                    vm->dirty[j] = 1;

                    vm->clockPT[j] = vm->currentClock;
                }
            }
            vm->VMdata[address] = *(int*)&value;
            return;
        }
    }
    vm->TLBMiss++;
    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            updateTLB(vm, mapping);
            vm->clockPT[i] = vm->currentClock;
            vm->dirty[i] = 1;

            vm->VMdata[address] = *(int*)&value;
            return;
        }
    }
    vm->pageFaults++;
    vm->ptRR = findSmallest(vm);
    diskWrite(vm);
    updateTLB(vm, mapping);
    updatePageTable(vm, mapping);
    

    for (int i = 0; i < vm->sizePM; i++)
    {
        if (vm->pt[i] == mapping)
        {
            vm->dirty[i] = 1;
        }
    }

    vm->VMdata[address] = *(int*)&value;
    return;
}

void printStatistics(void *handle)
{
    VM *vm = (VM *)handle;
    printf("Number of page faults: ");
    printf("%i\n", vm->pageFaults);

    printf("Number of TLB misses: ");
    printf("%i\n", vm->TLBMiss);

    printf("Number of disk writes: ");
    printf("%i\n", vm->diskWrites);


  
}

void cleanupVM(void *handle)
{
    VM *vm = handle;
    free(vm->TLB);
    free(vm->pt);
    free(vm->VMdata);
    free(vm->clockTLB);
    free(vm->clockPT);
    free(vm->dirty);
    free(vm);
}