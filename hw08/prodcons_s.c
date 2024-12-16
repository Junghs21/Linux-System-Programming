#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "semlib2.h"
#include "prodcons.h"

//Shared buffer and semaphores
BoundedBufferType	Buf;	//Shared buffer
sem_t				EmptySem, FullSem, MutexSem;	//Semaphores for empty, full and mutual exclusion

//Function to simulate a thread sleeping for a specified time
void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond;
	pthread_mutex_t		mutex;
	struct timespec		ts;
	struct timeval		tv;

	//Initialize condition variable and mutex
	if (pthread_cond_init(&cond, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	//Calculate the absolute time to sleep
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	//Wait using condition variable for the specified time
	if (pthread_mutex_lock(&mutex) < 0)  {
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  {
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	//Destroy condition variable and mutex
	if (pthread_cond_destroy(&cond) < 0)  {
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  {
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

//Producer thread function
void
Producer(void *dummy)
{
	int		i, data;

	printf("Producer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  {
		if (sem_wait(&EmptySem) < 0)  {	//Wait for an empty slot
			perror("sem_wait");
			pthread_exit(NULL);
		}
		//Lock the buffer
		if (sem_wait(&MutexSem) < 0)  {
			perror("sem_wait");
			pthread_exit(NULL);
		}

		//Produce an item
		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000;
		Buf.buf[Buf.in].data = data;	//Add data to the buffer
		Buf.in = (Buf.in + 1) % MAX_BUF;	//Update circular buffer index
		Buf.counter++;	//Increment the item count

		//Unlock the buffer
		if (sem_post(&MutexSem) < 0)  {
			perror("sem_post");
			pthread_exit(NULL);
		}
		//Signal that there is a full slot
		if (sem_post(&FullSem) < 0)  {
			perror("sem_post");
			pthread_exit(NULL);
		}

		ThreadUsleep(data);	//Simulate production time
	}

	printf("Producer: Produced %d items.....\n", i);
	printf("Producer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL);
}

//Consumer thread function
void
Consumer(void *dummy)
{
	int		i, data;

	printf("Consumer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  {
		if (sem_wait(&FullSem) < 0)  {	//Wait for a full slot
			perror("sem_wait");
			pthread_exit(NULL);
		}
		if (sem_wait(&MutexSem) < 0)  {	//Lock the buffer
			perror("sem_wait");
			pthread_exit(NULL);
		}

		//Consume an item
		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data;	//Retrieve data from the buffer
		Buf.out = (Buf.out + 1) % MAX_BUF;	//Update circular buffer index
		Buf.counter--;	//Decrement the item count

		//Unlock the buffer
		if (sem_post(&MutexSem) < 0)  {
			perror("sem_post");
			pthread_exit(NULL);
		}
		//Signal that there is an empty slot
		if (sem_post(&EmptySem) < 0)  {
			perror("sem_post");
			pthread_exit(NULL);
		}

		ThreadUsleep((rand()%100)*10000);	//Simulate consumption time
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL);
}

main()
{
	pthread_t	tid1, tid2;

	srand(0x8888);	//Seed the random number generator

	//Initialize semaphores
	if (sem_init(&EmptySem, 0, MAX_BUF) < 0)  {	//Track empty slots
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&FullSem, 0, 0) < 0)  {	//Tracks full slots
		perror("sem_init");
		exit(1);
	}
	if (sem_init(&MutexSem, 0, 1) < 0)  {	//Mutex for buffer access
		perror("sem_init");
		exit(1);
	}

	//Create producer thread
	if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	//Create consumer thread
	if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	//Wait for threads to complete
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
											
	printf("Main    : %d items in buffer.....\n", Buf.counter);	//Print the final state of the buffer

	//Destroy semaphores
	if (sem_destroy(&EmptySem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&FullSem) < 0)  {
		perror("sem_destroy");
	}
	if (sem_destroy(&MutexSem) < 0)  {
		perror("sem_destroy");
	}
}