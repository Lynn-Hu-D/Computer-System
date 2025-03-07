#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 15    

int buffer[BUF_SIZE];   // The buffer to hold items
int num = 0;            // The current number of items in the buffer
int total_produced = 0; // The total number of items produced
int total_consumed = 0; // The total number of items consumed

pthread_mutex_t mut;	
pthread_cond_t cond; 

void *producer(void *param);
void *consumer(void *param);

int main (int argc, char *argv[])
{
  pthread_t t1, t2, t3, t4;	

  if(pthread_mutex_init(&mut, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }
  if(pthread_cond_init(&cond, NULL) != 0) {
    perror("pthread_cond_init");
    exit(1);
  }
  // Create producer threads
  if (pthread_create(&t1, NULL, producer, "producer_1") != 0) {
      perror("pthread_create");
      exit(1);
  }
  if (pthread_create(&t2, NULL, producer, "producer_2") != 0) {
      perror("pthread_create");
      exit(1);
  }

  // Create consumer threads
  if (pthread_create(&t3, NULL, consumer, "comsumer_1") != 0) {
      perror("pthread_create");
      exit(1);
  }
  if (pthread_create(&t4, NULL, consumer, "consumer_2") != 0) {
      perror("pthread_create");
      exit(1);
  }
  /* wait a while and then exit */
  while (total_consumed < 30) {
      sleep(1);
  }
  sleep(5);
  printf("WE ARE DONE\n");

  // Terminate all threads (join them if necessary)
  pthread_cancel(t1);
  pthread_cancel(t2);
  pthread_cancel(t3);
  pthread_cancel(t4);

  // Join the threads to clean up
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  return 0;
}

void *producer(void *param)
{
  char *name = (char*)param;
  int i=0;
  while(1) {
    pthread_mutex_lock (&mut);

    // Check if 30 items have been produced; if so, exit the thread
    if (total_produced >= 30) {
        pthread_mutex_unlock(&mut);  // Unlock mutex before exiting
        printf("Thread name: %s, Total producted: %d, exit consumer.\n", name, total_produced);
        pthread_exit(NULL);
    }
    total_produced++;  // Increment total produced items
    // record total_produced because total_produced can increase when conditionally waitting
    int record = total_produced;
    if (num > BUF_SIZE) exit(1);	
    while (num == BUF_SIZE)			
      pthread_cond_wait (&cond, &mut);
    buffer[num] = i;
    // printf("Produced num: %d, buffer[num] : %d\n", num, buffer[num]);

    num++;

    printf("Thread name: %s, Produced: %d, Total Produced: %d\n", name, i, record);
    i++;

    pthread_mutex_unlock (&mut);
    pthread_cond_signal (&cond);
    sleep(1);
  }
}

void *consumer(void *param)
{
  char *name = (char*)param;
  int i=0;
  while (1) {
    pthread_mutex_lock (&mut);

    // Check if 30 items have been consumed; if so, exit the thread
    if (total_consumed >= 30) {
        pthread_mutex_unlock(&mut);  // Unlock mutex before exiting
        printf("Thread name: %s, Total consumed: %d, exit consumer.\n", name, total_consumed);
        pthread_exit(NULL);
    }
    total_consumed++;  // Increment total consumed items
    int record = total_consumed;

    if (num < 0) exit(1);   
    while (num == 0)		
      pthread_cond_wait (&cond, &mut);
    i = buffer[num-1];
    // printf("Consumed num: %d, buffer[num] : %d, i= %d\n", num, buffer[num], i);

    num--;

    printf("Thread name: %s, Consumed: %d, Total Consumed: %d\n", name, i, record);

    pthread_mutex_unlock (&mut);
    pthread_cond_signal (&cond);
    sleep(3);
  }
}
