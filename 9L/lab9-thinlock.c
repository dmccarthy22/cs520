#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

#define MAX_T 24
int partialSums[MAX_T];
int argcount;
FILE *arguments[MAX_T];
pthread_mutex_t mu;
typedef struct
{
    char *name;
    int count;
} filecount_t;
filecount_t f[MAX_T];
pthread_t pt[MAX_T];
void *count(void *);
int sum = 0;
atomic_flag t;
int pthread_yield();
int z;

int main(int argc, char *argv[])
{
    int q = argc - 1;
    z = q;
    if (pthread_mutex_init(&mu, NULL) != 0)
    {
        error("can't init mutex");
    }
    for (int v = 0; v < q; v++)
    {
        f[v].name = argv[v + 1];
        f[v].count = 0;
    }

    int i;

    atomic_flag_clear(&t);

    for (i = 1; i < argc; i++)
    {
        if (pthread_create(&pt[i - 1], NULL, count, (void *)&f[i - 1]) != 0)
        {
            printf("ERROR\n");
        }
    }

    
    for (int l = 0; l < argc - 1; l++)
    {
        if (pthread_join(pt[l], NULL))
        {
            error("error in thread join");
        }
    }
    sort(argc - 1);

    for (int k = 0; k < argc - 1; k++)
    {
        printf("%s=%d\n", f[k].name, f[k].count);
    }
    printf("Total Count=%d\n", sum);
    return 0;
}

void sort(int n)
{
    filecount_t temp;
    for (int k = 0; k < n - 1; k++)
    {
        for (int j = k + 1; j < n; j++)
        {
            if (strcmp(f[k].name, f[j].name) > 0)
            {
                temp = f[k];
                f[k] = f[j];
                f[j] = temp;
            }
        }
    }
}

void *count(void *in)
{

    filecount_t *b = (filecount_t *)in;

    char *c = b->name;
    FILE *fp;

    if (fopen(c, "r")==NULL){
        return 0;

    }

    fp = fopen(c, "r");
    char v;

    while (!feof(fp))
    {
        v = getc(fp);
        if (v == '\n')
        {
            ((filecount_t *)in)->count = ((filecount_t *)in)->count + 1;
        }
    }
    if (pthread_mutex_lock(&mu) != 0)
    {
        error("error in mutex_lock in child");
    }
    sum += ((filecount_t *)(in))->count;
    if (pthread_mutex_unlock(&mu) != 0)
    {
        error("error in mutex_unlock in child");
    }
    for (int l = 0; l < z; l++)
    {
        if (!atomic_flag_test_and_set(&t))
        {
            break;
        }
        if (pthread_yield())
        {
            fprintf(stderr, "pthread_yield failed!\n");
            exit(-1);
        }
    }
    atomic_flag_clear(&t);
    fclose(fp);
    return 0;
}