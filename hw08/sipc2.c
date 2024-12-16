#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "semlib.h"

/**
 * [Program Name] : Assignment 8
 * [Description]  :
 * 	- Receives a request from sipc1 through shared memory and sends a reply
 * [Input]	  :
 * 	Reads data from the shared memory buffer
 * 	Uses semaphores to synchronize with sipc1
 * [Output]	  :
 * 	Displays the message received from sipc1
 * 	Displays the reply sent back to sipc1
 * [Calls]	  :
 * 	semInit - Initializes a semaphore with a given key
 * 	semWait - Decrements the semaphore value
 * 	semPost - Increments the semaphore value
 * 	semDestroy - Removes a semaphore from the system
 * [Special Notes]:
 * 	Shared memory is utilized for inter-process communication
 */
main() {

	int shmid;
	char *ptr, *pData;
	int *pInt;
	int emptySem, fullSem, mutexSem;

	if((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
		perror("shmget");
		exit(1);
	}
	if((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
		perror("shmat");
		exit(1);
	}

	if((emptySem = semInit(EMPTY_SEM_KEY)) < 0) {
		perror("semInit");
		exit(1);
	}
	if((fullSem = semInit(FULL_SEM_KEY)) < 0) {
		perror("semInit");
		exit(1);
	}
	if((mutexSem = semInit(MUTEX_SEM_KEY)) < 0) {
		perror("semInit");
		exit(1);
	}

	pInt = (int *)ptr;
	pData = ptr + sizeof(int);

	semWait(emptySem);
	printf("Received request: %s\n", pData);

	sprintf(pData, "This is a reply from %d.", getpid());
	semPost(fullSem);

	if(shmctl(shmid, IPC_RMID, 0) < 0) {
		perror("shmctl");
		exit(1);
	}
}