#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void cancelCatchException(void);
void throwException(int);
int catchException(void);

#define INSUFFICIENT_MEMORY "malloc returned NULL in catchException\n"
#define INVALID_EXCEPTION_NUMBER "throwException: Invalid exception number %d\n"
#define UNHANDLED_EXCEPTION "unhandled exception %d\n"

long saveRBP();
//long saveRIP();
long saveRBX();
long saveR12();
long saveR13();
long saveR14();
long saveR15();
//void setRBP(long);
//void setRIP(long);
void setRBX(long);
void setR12(long);
void setR13(long);
void setR14(long);
void setR15(long);
void pareStack(long, int);

static struct Snap *stack = 0;

typedef struct Snap
{
    long newRbp;
    long oldRbp;
    long rip;
    long rbx;
    long r12;
    long r13;
    long r14;
    long r15;
    struct Snap *next;
} Snap;
static struct Snap *stack;

int catchException()
{
    Snap *snap = malloc(sizeof(Snap));
    if (snap == NULL){
        fprintf(stderr, INSUFFICIENT_MEMORY);
        exit(-1);
    }
    snap->newRbp = saveRBP();
    snap->oldRbp = *((long *)snap->newRbp);
    snap->rip = *(((long *)snap->newRbp) + 1);
    snap->rbx = saveRBX();
    snap->r12 = saveR12();
    snap->r13 = saveR13();
    snap->r14 = saveR14();
    snap->r15 = saveR15();
    snap->next = stack;
    stack = snap;
    return 0;
    
}

void throwException(int exception)
{
    if (stack == 0){
        fprintf(stderr, UNHANDLED_EXCEPTION, exception);
        exit(-1);
    }
    if(exception<1){
        fprintf(stderr, INVALID_EXCEPTION_NUMBER, exception);
        exit(-1);
    }
    Snap *snap = stack;
    stack = stack->next;
    *(long *)snap->newRbp = snap->oldRbp;
    *(((long *)snap->newRbp) + 1) = snap->rip;
    setRBX(snap->rbx);
    setR12(snap->r12);
    setR13(snap->r13);
    setR14(snap->r14);
    setR15(snap->r15);
    pareStack(snap->newRbp, exception);
}

    void cancelCatchException()
    {
        if(stack !=0){
            stack = stack->next;
        }

    }

    //