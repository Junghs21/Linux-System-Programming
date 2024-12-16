#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

//Shared buffer and synchronization primitives
BoundedBufferType	Buf;	//Shared buffer
pthread_cond_t		NotFull;	//Condition variable for "not full"
pthread_cond_t		NotEmpty;	//Condition variable for "not empty"
pthread_mutex_t		Mutex;		//Mutex for critical section

//Function to simulate thread sleeping for a specified time
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

	//Calculate absolute sleep time
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	//Wait for the specified time using condition variable
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
		if (pthread_mutex_lock(&Mutex) < 0)  {	//Lock the mutex
			perror("pthread_mutex_lock");
			pthread_exit(NULL);
		}

		//Wait until the buffer is not full
		while (Buf.counter == MAX_BUF)  {
			if (pthread_cond_wait(&NotFull, &Mutex) < 0)  {
				perror("pthread_cond_wait");
				pthread_exit(NULL);
			}
		}

		//Produce an item
		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000;
		Buf.buf[Buf.in].data = data;	//Store data in the buffer
		Buf.in = (Buf.in + 1) % MAX_BUF;	//Update circular buffer index
		Buf.counter++;	//Increment buffer counter

		//Signal that the buffer is not empty
		if (pthread_cond_signal(&NotEmpty) < 0)  {
			perror("pthread_cond_signal");
			pthread_exit(NULL);
		}
		//Unlock the mutex
		if (pthread_mutex_unlock(&Mutex) < 0)  {
			perror("pthread_mutex_unlock");
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
		if (pthread_mutex_lock(&Mutex) < 0)  {	//Lock the mutex
			perror("pthread_mutex_lock");
			pthread_exit(NULL);
		}

		//Wait untile the buffer is not empty
		while (Buf.counter == 0)  {
			if (pthread_cond_wait(&NotEmpty, &Mutex) < 0)  {
				perror("pthread_cond_wait");
				pthread_exit(NULL);
			}
		}

		//Consume an item
		printf("Consumer: Consuming an item.....\n");
		data = Buf.buf[Buf.out].data;		//Retrieve data from the buffer
		Buf.out = (Buf.out + 1) % MAX_BUF;	//Update circular buffer index
		Buf.counter--;				//Decrement buffer counter

		//Signal that the buffer is not full
		if (pthread_cond_signal(&NotFull) < 0)  {
			perror("pthread_cond_signal");
			pthread_exit(NULL);
		}
		//Unlock the mutex
		if (pthread_mutex_unlock(&Mutex) < 0)  {
			perror("pthread_mutex_unlock");
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

	srand(0x8888);	//Seed random number generator

	//Initialize condition variables and mutex
	if (pthread_cond_init(&NotFull, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_cond_init(&NotEmpty, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&Mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
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

	//Wait for threads to finish
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

	printf("Main    : %d items in buffer.....\n", Buf.counter);	//Print final state of the buffer

	//Destroy condition variables and mutex
	if (pthread_cond_destroy(&NotFull) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_cond_destroy(&NotEmpty) < 0)  {
		perror("pthread_cond_destroy");
	}
	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
	}
}