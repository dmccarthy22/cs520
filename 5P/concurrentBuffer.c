//
// defines a concurrent buffer that supports the producer-consumer pattern
//
// the buffer elements are simply void* pointers
// note: the buffer does not make a copy of the data.
//       it simply stores a pointer to the data.
//
// the buffer is FIFO (first in, first out)
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "concurrentBuffer.h"

typedef struct {
    int size;
    int maxSize;
    void** line;
    pthread_mutex_t mu;
    pthread_cond_t get;
    pthread_cond_t put;
    int in;
    int out;
} buffer;


// create a concurrent buffer
//   size: the number of elements in the buffer
//   returns: a handle to be used to operate on the buffer
void *createConcurrentBuffer(unsigned int size)
{
   
    buffer *buff = malloc(sizeof(buffer));
    if (buff == NULL){
        free(buff);
        return NULL;
    }
    if (size<1){
        free(buff);
        return NULL;
    }
    buff->maxSize = size;
    buff->line =malloc(sizeof(void *) * size);
    if (buff->line == NULL){
        free(buff);
        return NULL;
    }
    buff->in = 0;
    buff->out = 0;
    buff->size = 0;
    if (pthread_mutex_init(&buff->mu, NULL) != 0)
    {
        free(buff);
        fprintf(stderr, "can't init mutex");
        return NULL;
    }
    if (pthread_cond_init(&buff->get, NULL) != 0)
    {
        free(buff);
        fprintf(stderr, "can't init condition variable");
        return NULL;
    }
    if (pthread_cond_init(&buff->put, NULL) != 0)
    {
        free(buff);
        fprintf(stderr, "can't init condition variable");
        return NULL;
    }
    for (int i = 0; i <size; i++){
        buff->line[i] = NULL;
    }

    return buff;
}

// put a value in a buffer
//   handle: handle for a concurrent buffer
//   p: pointer to be put in the buffer
//   note: calling thread will block until there is space in the buffer
void putConcurrentBuffer(void *handle, void *p)
{
   
    buffer *buff = handle;
    
    if (pthread_mutex_lock(&buff->mu) != 0)
    {
        fprintf(stderr, "error in mutex_lock in child");
        exit(-1);
   }

    while(buff->size == buff->maxSize){
        if (pthread_cond_wait(&buff->put, &buff->mu) != 0)
        {
           fprintf(stderr, "error in cond_wait by parent");
            exit(-1);
        }
    }
    if (buff->in == buff->maxSize)
    {
        buff->in = 0;
    }
    buff->line[buff->in] = p;
   (buff->size)++;
   (buff->in)++;
   if (pthread_cond_signal(&buff->get) != 0)
   {
       fprintf(stderr, "error in cond_signal");
       exit(-1);
   }
   if (pthread_mutex_unlock(&buff->mu) != 0)
   {
       fprintf(stderr, "error in mutex_unlock in child");
       exit(-1);
    }
}

// get a value from a buffer
//   handle: handle for a concurrent buffer
//   returns: pointer retrieved from buffer
//   note: calling thread will block until there is a value available
void *getConcurrentBuffer(void *handle)
{
    
    buffer *buff = handle;
    
    if (pthread_mutex_lock(&buff->mu)!=0){
        fprintf(stderr, "error in mutex_lock in child");
        exit(-1);
    }
    while(buff->size == 0){
        if(pthread_cond_wait(&buff->get, &buff->mu)!=0){
            fprintf(stderr, "error in cond_wait by parent");
            exit(-1);
        }
    }
    if (buff->out == buff->maxSize)
    {
        buff->out = 0;
    }
    void * j = buff->line[buff->out];
    (buff->size)--;
    (buff->out)++;
    if (pthread_cond_signal(&buff->put) != 0)
    {
        fprintf(stderr, "error in cond_signal");
        exit(-1);
    }

    if (pthread_mutex_unlock(&buff->mu) != 0)
    {
        fprintf(stderr, "error in mutex_unlock in child");
        exit(-1);
    }
  
   
    return j;
}
// delete a buffer
//   handle: handle for the concurrent buffer to be deleted
void deleteConcurrentBuffer(void *handle)
{
    
    buffer *buff = handle;
    free(buff->line);
    free(buff);
   
}
