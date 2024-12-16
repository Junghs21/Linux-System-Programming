#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "synclib.h"
#include "shm.h"

#define NLOOPS 10
#define SIZE sizeof(long)

//Function to update the shared counter
int update(long *ptr) {
	return ((*ptr)++);	//Increment the value pointed by ptr and return the new value
}

/**
 * [Program Name]  : Assignment 7
 * [Description]   :
 * 	- A program demonstrating inter-process communication using shared memory(shmget, shmat)
 * 	- Synchronization primitives for alternating updates between parent and child process
 * [Input]	   :
 * 	None directly from the user
 * 	The program initializes a shared memory segment and synchronizes process actions
 * [Output]	   :
 * 	Parent and child processs print the updated counter values to the console
 * [Calls]	   :
 * 	shmget() - Creates a shared memory segment
 * 	shmat()  - Attaches the shared memory segment to the process address
 * 	shmctl() - Controls and removes the shared memory segment
 * 	fork()   - Creates a child process
 * 	TELL_WAIT(), TELL_PARENT(), TELL_CHILD(), WAIT_PARENT(), WAIT_CHILD() - Synchronization primitives for process communication
 * [Special Notes] :
 * 	The program uses shared memory to maintain a shared counter, updated alternately by parent and child process
 * 	The shared memory key(SHM_KEY) and size(SHM_SIZE) are predefined in "shm.h"
 * 	Synchronization is achieved using functions defined in "synclib.h"
 * 	The shared memory segment is cleaned up by the parent process after completion
 */
int main() {
	int i, counter;
	int shmid;
	char *ptr;
	pid_t pid;

	TELL_WAIT();	//Initialize synchronization primitives for parent and child communication
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid > 0) {	//Parent process
		//Create and attach the shared memory segment
		if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
			perror("shmget");
			exit(1);
		}
		if ((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
			perror("shmat");
			exit(1);
		}

		//Update the shared counter and alternate with the child process
		for (i = 0; i < NLOOPS; i += 2) {
			if ((counter = update((long *)ptr)) != i) {
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Parent: counter=%d\n", counter);
			TELL_CHILD();	//Notify the child process
			WAIT_CHILD();	//Wait for the child process to complete
		}
	} else {	//Child process
		//Attach the shared memory segment
		if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
			perror("shmget");
			exit(1);
		}
		if ((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
			perror("shmat");
			exit(1);
		}
		
		//Update the shared counter and alternate with the parent process
		for (i = 1; i < NLOOPS; i += 2) {
			WAIT_PARENT();	//Wait for the parent process to complete
			if ((counter = update((long *)ptr)) != i) {
				fprintf(stderr, "Counter mismatch\n");
				exit(1);
			}
			printf("Child : counter=%d\n", counter);
			TELL_PARENT();	//Notify the parent process
		}
	}
	
	//Remove the shared memory segment
	if (shmctl(shmid, IPC_RMID, 0) < 0) {
		perror("shmctl");
		exit(1);
	}
}