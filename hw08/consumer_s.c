#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include "prodcons.h"

main()
{
	BoundedBufferType	*pBuf;
	int					shmid, i, data;
	int					emptySemid, fullSemid, mutexSemid;

	//Create or access shared memory
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
		perror("shmget");
		exit(1);
	}
	//Attach the shared memory to the process's address space
	if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  {
		perror("shmat");
		exit(1);
	}

	//Initialize semaphores
	if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0)  {
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((fullSemid = semInit(FULL_SEM_KEY)) < 0)  {
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}
	if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0)  {
		fprintf(stderr, "semInit failure\n");
		exit(1);
	}

	//Set initial values for semaphores
	if (semInitValue(emptySemid, MAX_BUF) < 0)  {
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}
	if (semInitValue(fullSemid, 0) < 0)  {
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}
	if (semInitValue(mutexSemid, 1) < 0)  {
		fprintf(stderr, "semInitValue failure\n");
		exit(1);
	}

	srand(0x9999);	//Seed the random number generator
	for (i = 0 ; i < NLOOPS ; i++)  {	//Loop to consume items
		if (semWait(fullSemid) < 0)  {	//Wait for a filled slot
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		if (semWait(mutexSemid) < 0)  {	//Lock the buffer
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		//Consume an item
		printf("Consumer: Consuming an item.....\n");
		data = pBuf->buf[pBuf->out].data;	//Retrieve data from the buffer
		pBuf->out = (pBuf->out + 1) % MAX_BUF;	//Increment circular buffer index
		pBuf->counter--;	//Update item count in the buffer

		//Unlock the buffer
		if (semPost(mutexSemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		//Signal that a slot is now available
		if (semPost(emptySemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}

		usleep((rand()%100)*10000);	//Simulate consumption time
	}

	printf("Consumer: Consumed %d items.....\n", i);	//Print the total number of consumed items
	printf("Consumer: %d items in buffer.....\n", pBuf->counter);	//Print the final state of the buffer
}