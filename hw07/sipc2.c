#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"


main()
{
	int		shmid;
	char	*ptr, *pData;
	int		*pInt;


	/* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {	//Create or connect to a shared memory segment
		perror("shmget");
		exit(1);
	}
	if ((ptr = shmat(shmid, 0, 0)) == (void *) - 1)  {	//Attach the shared memory segment to the process's address space
		perror("shmat");
		exit(1);
	}

	pInt = (int *)ptr;
	pData = ptr + sizeof(int);
	sprintf(pData, "This is a request from %d.", getpid());
	*pInt = 1;
	printf("Sent a request.....");

	while ((*pInt) == 1)
		;

	printf("Received reply: %s\n", pData);

	/* Detach shared memory */
	if (shmdt(ptr) < 0)  {	//Detach the shared memory segment from the process
		perror("shmdt");
		exit(1);
	}
}