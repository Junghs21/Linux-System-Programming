#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

BoundedBufferType	Buf;	//Global shared buffer declartion

//Function to sleep for a specified time
void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond;
	pthread_mutex_t		mutex;
	struct timespec		ts;
	struct timeval		tv;

	if (pthread_cond_init(&cond, NULL) < 0)  {	//Initialize condition vairable and mutex
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	//Calculate absolute time for the sleep
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	if (pthread_mutex_lock(&mutex) < 0)  {	//Use condition variable to wait for teh specified time
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  {
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	//Destroy the condition variable and mutex
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
		if (Buf.counter == MAX_BUF)  {	//Wait if the buffer is full
			printf("Producer: Buffer full. Waiting.....\n");
			while (Buf.counter == MAX_BUF)
				;	//Busy-wait
		}

		//Produce an item
		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000;	//Generate random data
		Buf.buf[Buf.in].data = data;	//Store the data in the buffer
		Buf.in = (Buf.in + 1) % MAX_BUF;	//Manage circular buffer index
		Buf.counter++;

		ThreadUsleep(data);	//Simulate production time
	}

	printf("Producer: Produced %d items.....\n", i);
	printf("Producer: %d items in buffer.....\n", Buf.counter);
							
	pthread_exit(NULL);	//Exit thread
}

//Consumer thread function
void
Consumer(void *dummy)
{
	int		i, data;

	printf("Consumer: Start.....\n");

	for (i = 0 ; i < NLOOPS ; i++)  {
		if (Buf.counter == 0)  {	//Wait if the buffer is empty
			printf("Consumer: Buffer empty. Waiting.....\n");
			while (Buf.counter == 0)
				;	//Busy-wait
		}

		//Consume an item
		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data;	//Retrieve data from the buffer
		Buf.out = (Buf.out + 1) % MAX_BUF;	//Manage circular buffer index
		Buf.counter--;

		ThreadUsleep((rand()%100)*10000);	//Simulate consumption time
	}

	printf("Consumer: Consumed %d items.....\n", i);
	printf("Consumer: %d items in buffer.....\n", Buf.counter);

	pthread_exit(NULL);
}

main()
{
	pthread_t	tid1, tid2;

	srand(0x8888);	//Send the random number generator

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

	//Wait for threads to finish
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

	printf("Main    : %d items in buffer.....\n", Buf.counter);
}