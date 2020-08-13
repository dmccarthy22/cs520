// test for exception handler

#include <stdio.h>

void throwException(int);
void cancelCatchException(void);
int catchException(void);

void f1()
{
    fprintf(stderr, "f1 entered\n");
    validException();
    fprintf(stderr, "f1 exiting\n");
}

int main(int argc, char *argv[])
{
    int caughtException;

    fprintf(stderr, "main entered\n");
    if ((caughtException = catchException()))
    {
        fprintf(stderr, "catch clause entered\n");
        fprintf(stderr, "caught exception %d\n", caughtException);
        fprintf(stderr, "catch clause exiting\n");
    }
    else
    {
        fprintf(stderr, "try block entered\n");
        f1();
        fprintf(stderr, "try block exiting\n");
    }
    cancelCatchException();
    fprintf(stderr, "exception cancelled\n");
    fprintf(stderr, "main exiting\n");
    return 0;
}
void f2(){
    fprintf(stderr, "f2 entered\n");
    f3();
    fprintf(stderr, "f2 exiting\n");
}
void f3(){
    fprintf(stderr, "f3 entered\n");
    #if BAD_EXCEPTION
    errorException();
    #endif
    #if VALID_EXCEPTION
    f1();
    #endif
    fprintf(stderr, "f3 exiting\n");
}
void errorException(){
    fprintf(stderr, "Error Exception Entered\n");
    throwException(-69);
    fprintf(stderr, "Error Exception exiting\n");
}
void validException(){
    throwException(420);
}
