#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h"

main()
{
	BoundedBufferType	*pBuf;
	int					shmid, i, data;

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

	srand(0x9999);	//Seed the random number generator
	for (i = 0 ; i < NLOOPS ; i++)  {	//Loop to consume items
		if (pBuf->counter == 0)  {	//Wait if the buffer is empty
			printf("Consumer: Buffer empty. Waiting.....\n");
			while (pBuf->counter == 0)
				;	//Busy-wait until an item is available in the buffer
		}

		//Consume an item
		printf("Consumer: Consuming an item.....\n");
		data = pBuf->buf[pBuf->out].data;	//Retrieve data from the buffer
		pBuf->out = (pBuf->out + 1) % MAX_BUF;	//Increment circular bufferindex
		pBuf->counter--;	//Update item count in the buffer

		usleep((rand()%100)*10000);	//Simulate consumption time
	}

	printf("Consumer: Consumed %d items.....\n", i);	//Print the total number of consumed items
	printf("Consumer: %d items in buffer.....\n", pBuf->counter);	//Print the final state of the buffer
}