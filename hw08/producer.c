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

	srand(0x8888);	//Send the random number generator
	for (i = 0 ; i < NLOOPS ; i++)  {	//Loop to produce items
		if (pBuf->counter == MAX_BUF)  {	//Wait if the buffer is full
			printf("Producer: Buffer full. Waiting.....\n");
			while (pBuf->counter == MAX_BUF)
				;	//Busy-wait until space is available in the buffer
		}

		//Produce an item
		printf("Producer: Producing an item.....\n");
		data = (rand()%100)*10000;	//Generate random data
		pBuf->buf[pBuf->in].data = data;	//Store the data in the buffer
		pBuf->in = (pBuf->in + 1) % MAX_BUF;	//Increment circular buffer
		pBuf->counter++;	//Update item count in the buffer

		usleep(data);	//Simulate production time
	}

	printf("Producer: Produced %d items.....\n", i);

	sleep(2);	//Allow some time for other process to run
	printf("Producer: %d items in buffer.....\n", pBuf->counter);	//Print the final state of the buffer
											
	//Remove the shared memory
	if (shmctl(shmid, IPC_RMID, 0) < 0)  {
		perror("shmctl");
		exit(1);
	}
}