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

	//Create or connect to a shared memory segment
	if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
		perror("shmget");
		exit(1);
	}
	//Attach the sahred memory segment to the process's address space
	if ((ptr = shmat(shmid, 0, 0)) == (void *) - 1)  {
		perror("shmat");
		exit(1);
	}

	pInt = (int *)ptr;
	while ((*pInt) == 0)
		;

	pData = ptr + sizeof(int);	//Point to the data section
	printf("Received request: %s.....", pData);
	sprintf(pData, "This is a reply from %d.", getpid());
	*pInt = 0;
	printf("Replied.\n");

	sleep(1);

	/* Detach shared memory */
	if (shmdt(ptr) < 0)  {	//Detach the shared memory segment from the process
		perror("shmdt");
		exit(1);
	}

	/* Remove shared memory */
	if (shmctl(shmid, IPC_RMID, 0) < 0)  {	//Remove the shared memory segment
		perror("shmctl");
		exit(1);
	}
}