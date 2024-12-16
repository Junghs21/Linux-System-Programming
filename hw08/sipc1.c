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
 * 	- Sends a request to the shared memory
 * 	- Waits for a reply from sipc2
 * [Input] 	  : 
 * 	Uses shared memory to write data to the buffer
 * 	Uses semaphores to coordinate synchronization between sipc1 and sipc2
 * [Output]	  :
 * 	Displays the message sent to sipc2
 * 	Displays the reply received from sipc2
 * [Calls]	  :
 * 	semInit - Initializes a semaphore with a given key
 * 	semWait - Decrements the semaphore value
 * 	semPost - Increments the semaphore value
 * 	semDestroy - Removes a semaphore from the system
 * [Special Notes]:
 * 	Shared memory is utilized for inter-process communication
 */
int main() {
	
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

	if(semInitValue(emptySem, 1) < 0 || semInitValue(fullSem, 0) < 0 || semInitValue(mutexSem, 1) < 0) {
		perror("semInitValue");
		exit(1);
	}

	pInt = (int *)ptr;
	pData = ptr + sizeof(int);
	sprintf(pData, "This is a request from %d.", getpid());
	*pInt = 1;

	printf("Sent a request...\n");

	semWait(fullSem);
	printf("Received reply: %s\n", pData);

	semDestroy(emptySem);
	semDestroy(fullSem);
	semDestroy(mutexSem);

	if(shmctl(shmid, IPC_RMID, 0) < 0) {
		perror("shmctl");
		exit(1);
	}
}