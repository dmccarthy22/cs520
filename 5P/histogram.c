#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "concurrentBuffer.h"

static void *work1(void *);
static void *work2(void *);
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 5
#endif
int count[51];
void getCount(char *);

typedef struct
{
    FILE *fp;
    void *buffer;
    char *name;

} INFO;
pthread_mutex_t mu;

int main(int argc, char *argv[])
{
    //

    if (pthread_mutex_init(&mu, NULL) != 0)
    {
        fprintf(stderr, "can't init mutex");
    }
    // FILE *fp = fopen(argv[1], "r");
    // if (fp == NULL)
    // {
    //     fprintf(stderr, "can't open %s\n", argv[1]);
    //     exit(-1);
    // }
    void *cBuffer = createConcurrentBuffer(BUFFER_SIZE);
    if (cBuffer == NULL)
    {
        fprintf(stderr, "can't create the concurrent buffer\n");
        exit(-1);
    }
    INFO *arr[argc-1];

    pthread_t thread1[argc-1];
    for (int i = 0; i < argc-1; i++)
    {
        INFO *ip = malloc(sizeof(INFO));
        ip->buffer = cBuffer;
        ip->name = argv[i+1];
        if (pthread_create(&thread1[i], NULL, work1, ip) != 0)
        {
            fprintf(stderr, "can't create thread 1\n");
            exit(-1);
        }
    }

    pthread_t thread2[argc-1];
    for (int i = 0; i < argc-1; i++)
    {
        INFO *ip = malloc(sizeof(INFO));
        ip->buffer = cBuffer;
       ip->name = argv[i+1];
        if (pthread_create(&thread2[i], NULL, work2, cBuffer) != 0)
        {
            fprintf(stderr, "can't create thread 2\n");
            exit(-1);
        }
    }
    for (int i = 0; i < argc-1; i++)
    {

        if (pthread_join(thread1[i], NULL))
        {
            fprintf(stderr, "join for thread 1 fails\n");
            exit(-1);
        }

        if (pthread_join(thread2[i], NULL))
        {
            fprintf(stderr, "join for thread 2 fails\n");
            exit(-1);
        }
    }
    for (int i = 6; i < 51; i++)
    {
        printf("%d %d\n", i, count[i]);
    }
    
    deleteConcurrentBuffer(cBuffer);

    return 0;
}
#define MAX_LINE_LENGTH 1000
static char *getLine(FILE *fp)
{
    char buf[MAX_LINE_LENGTH];
    int i = 0;
    int c = getc(fp);
    if (c == EOF)
        return NULL;
    while (c != EOF && c != '\n')
    {
        buf[i] = c;
        i += 1;
        if (i == MAX_LINE_LENGTH)
        {
            fprintf(stderr, "maximum line length (%d) exceeded\n", MAX_LINE_LENGTH);
            exit(-1);
        }
        c = getc(fp);
    }
    if (c == '\n')
    {
        buf[i] = c;
        i += 1;
    }
    buf[i] = 0;
    char *s = malloc(i + 1);
    if (s == NULL)
    {
        fprintf(stderr, "malloc failed in getLine\n");
        exit(-1);
    }
    strcpy(s, buf);
    return s;
}
static void *work1(void *info)
{
    INFO *ip = info;
    
    if (fopen(ip->name, "r") == 0)
    {
        return 0;
    }
    ip->fp = fopen(ip->name, "r");
    
    // read the lines in the file
    char *line;
    while ((line = getLine(ip->fp)) != NULL)
    {

        putConcurrentBuffer(ip->buffer, line);
    }
    putConcurrentBuffer(ip->buffer, NULL);

    // put a NULL to indicate EOF
   
    fclose(ip->fp);
    return NULL;
}

static void *work2(void *buffer)
{

    char *line;

    while ((line = getConcurrentBuffer(buffer)) != NULL)
    {

        if (pthread_mutex_lock(&mu) != 0)
        {
            fprintf(stderr, "error in mutex_lock");
        }

        getCount(line);
        if (pthread_mutex_unlock(&mu) != 0)
        {
            fprintf(stderr, "error in mutex_unlock");
        }
    }


}

void getCount(char *line)
{

    int counter = 0;
    for (int i = 0; i < strlen(line); i++)
    {

        if ((line[i] > 64 && line[i] < 91) || (line[i] > 96 && line[i] < 123))
        {
            counter = counter + 1;
        }
        else
        {
            count[counter] = count[counter] + 1;
            counter = 0;
        }
    }
}