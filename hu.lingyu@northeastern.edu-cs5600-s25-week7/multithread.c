#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#define ARRAY_SIZE 10

// Global array and mutex
int global_array[ARRAY_SIZE];

void *worker(void *data)
{
    char *name = (char*)data;
 
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        usleep(50000);
        global_array[i] = global_array[i] + i;
        printf("Hi from thread name = %s\n", name);
        // printf("Hi from thread ID = %p\n", pthread_self());
    }
 
    printf("Thread %s done!\n", name);
    return NULL;
}

int main(void)
{
    pthread_t th1, th2;
    pthread_create(&th1, NULL, worker, "X");
    pthread_create(&th2, NULL, worker, "Y");
    sleep(2);
    printf("====> Cancelling Thread ID: %p\n", (void*)th2);
    pthread_cancel(th2);
    usleep(100000);
    printf("====> Cancelling Thread ID: %p\n", (void*)th1);
    pthread_cancel(th1);
    printf("exiting from main program\n");
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        printf("global_array[%d] = %d\n", i, global_array[i]);
    }
    return 0;
}