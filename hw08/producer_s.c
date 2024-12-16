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

	srand(0x8888);	//Seed the random number generator
	for (i = 0 ; i < NLOOPS ; i++)  {	//Loop to produce items
		if (semWait(emptySemid) < 0)  {	//Wait for an empty slot
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}
		if (semWait(mutexSemid) < 0)  {	//Look the buffer(mutex)
			fprintf(stderr, "semWait failure\n");
			exit(1);
		}

		//Produce an item
		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000;	//Generate random data
		pBuf->buf[pBuf->in].data = data;	//Store the data in the buffer
		pBuf->in = (pBuf->in + 1) % MAX_BUF;	//Increment circular buffer index
		pBuf->counter++;	//Update item count in the buffer

		//Unlock the buffer(mutex)
		if (semPost(mutexSemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		//Signal that a new item is available
		if (semPost(fullSemid) < 0)  {
			fprintf(stderr, "semPost failure\n");
			exit(1);
		}
		usleep(data);	//Simulate production time
	}

	printf("Producer: Produced %d items.....\n", i);

	sleep(2);	//Wait for 2 seconds to let consumers process 
	printf("Producer: %d items in buffer.....\n", pBuf->counter);

	//Destroy semaphores
	if (semDestroy(emptySemid) < 0)  {
		fprintf(stderr, "semDestroy failure\n");
	}
	if (semDestroy(fullSemid) < 0)  {
		fprintf(stderr, "semDestroy failure\n");
	}
	if (semDestroy(mutexSemid) < 0)  {
		fprintf(stderr, "semDestroy failure\n");
	}
	//Detach and remove shared memory
	if (shmctl(shmid, IPC_RMID, 0) < 0)  {
		perror("shmctl");
		exit(1);
	}
}