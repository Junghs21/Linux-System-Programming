#include <stdio.h>
#include "semlib.h"
 
 
//Initialize a semaphore with the given key
int
semInit(key_t key)
{
	int     semid;

	//Create a new semaphore or access an existing one
	if ((semid = semget(key, 1, 0600 | IPC_CREAT)) < 0)  {
		perror("semget");
		return -1;
	}

	return semid;	//Return the semaphore ID
}
 
//Set an initial value for the semaphore
int
semInitValue(int semid, int value)
{
	union semun {
		int     val;
	} semun;

	semun.val = value;	//Set the desired initial value
	if (semctl(semid, 0, SETVAL, semun) < 0)  {
		perror("semctl");
		return -1;
	}

	return semid;
}
 
//Perform a wait on the semaphore
int
semWait(int semid)
{
	struct sembuf   semcmd;
	     
	semcmd.sem_num = 0;	//Semaphore index
	semcmd.sem_op = -1;	//Decrement semaphore value
	semcmd.sem_flg = SEM_UNDO;	//Automatically undo operation on precess exit
	if (semop(semid, &semcmd, 1) < 0)  {
		perror("semop");
		return -1;
	}

	return 0;
}
 
//Perform a try-wait on the semaphore
int
semTryWait(int semid)
{
	struct sembuf   semcmd;
	     
	semcmd.sem_num = 0;	//Semaphore index
	semcmd.sem_op = -1;	//Decrement semaphore value
	semcmd.sem_flg = IPC_NOWAIT | SEM_UNDO;	//Non-block with undo flag
	if (semop(semid, &semcmd, 1) < 0)  {
		perror("semop");
		return -1;
	}

	return 0;
}

//Perform a signal on the semaphore
int
semPost(int semid)
{
	struct sembuf   semcmd;
	     
	semcmd.sem_num = 0;	//Semaphore index
	semcmd.sem_op = 1;	//Increment semaphore value
	semcmd.sem_flg = SEM_UNDO;	//Automatically undo operation on processexit
	if (semop(semid, &semcmd, 1) < 0)  {
		perror("semop");
		return -1;
	}
			     
	return 0;
}

//Retrieve the current value of teh semaphore
int
semGetValue(int semid)
{
	union semun {
		int     val;
	} dummy;
	     
	//Get the semaphore value
	return semctl(semid, 0, GETVAL, dummy);
}
 
//Destroy the semaphore and release resources
int
semDestroy(int semid)
{
	union semun {
		int     val;
	} dummy;

	//Remove the semaphore from the system
	if (semctl(semid, 0, IPC_RMID, dummy) < 0)  {
		perror("semctl");
		return -1;
	}
	close(semid);	//Close the semaphore ID

	return 0;
}