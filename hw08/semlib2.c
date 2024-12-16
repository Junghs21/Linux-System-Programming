#include <stdio.h>
#include <pthread.h>
#include "semlib2.h"


//Initialize a semaphore
int
sem_init(sem_t *sem, int pshared, int value)
{
	//Check if pshared is set
	if (pshared)  {
		fprintf(stderr, "Function not supported\n");
		return -1;
	}

	//Initialize the mutex for the semaphore
	if (pthread_mutex_init(&sem->mutex, NULL) < 0)
		return -1;

	//Initialize the condition variable for teh semaphore
	if (pthread_cond_init(&sem->cond, NULL) < 0)
		return -1;

	sem->sval = value;	//Set the initial value of the semaphore

	return 0;
}

//Wait(decrement) on a semaphore
int
sem_wait(sem_t *sem)
{
	//Lock the semaphore's mutex
	if (pthread_mutex_lock(&sem->mutex) < 0)
		return -1;

	//If the semaphore value is 0, wait until it's incremented
	if (sem->sval == 0)  {
		while (sem->sval == 0)  {	//Loop to handle spurious wakeups
			if (pthread_cond_wait(&sem->cond, &sem->mutex) < 0)  {
				if (pthread_mutex_unlock(&sem->mutex) < 0)	//Unlock the mutex and return error if the wait fails
					return -1;
				return -1;
			}
		}
		sem->sval--;	//Decrement the semaphore value after waking up
	}
	else  {
		sem->sval--;	//Decrement the semaphore value if it's greater than 0
	}

	//Unlock the semaphore's mutex
	if (pthread_mutex_unlock(&sem->mutex) < 0)
		return -1;

	return 0;
}

//Try to wait on a semaphore without blocking
int
sem_trywait(sem_t *sem)
{
	//Lock the semaphore's mutex
	if (pthread_mutex_lock(&sem->mutex) < 0)
		return -1;

	//Check if teh semaphore value is 0
	if (sem->sval == 0)  {
		if (pthread_mutex_unlock(&sem->mutex) < 0)	//Unlock the mutex if the value is 0
			return -1;
		return -1;
	}
	else  {
		sem->sval--;	//Decrement the semaphore value if it's greater than 0
	}

	//Unlock the semaphore's mutex
	if (pthread_mutex_unlock(&sem->mutex) < 0)
		return -1;

	return 0;
}

//Post(increment) a semaphore
int
sem_post(sem_t *sem)
{
	//Lock the semaphore's mutex
	if (pthread_mutex_lock(&sem->mutex) < 0)
		return -1;

	//If the semaphore value is 0, signal one waiting thread
	if (sem->sval == 0)  {
		if (pthread_cond_signal(&sem->cond) < 0)  {
			if (pthread_mutex_unlock(&sem->mutex) < 0)	//Unlock the mutex if signaling fails
				return -1;
			return -1;
		}
	}

	sem->sval++;	//Increment the semaphore value

	//Unlock the semaphore's mutex
	if (pthread_mutex_unlock(&sem->mutex) < 0)
		return -1;

	return 0;
}

//Get the current value of a semaphore
int
sem_getvalue(sem_t *sem, int *sval)
{
	*sval = sem->sval;	//Assign the semaphore value to the provided pointer

	return 0;
}

//Destroy a semaphore
int
sem_destroy(sem_t *sem)
{
	//Destroy the mutex associated with the semaphore
	if (pthread_mutex_destroy(&sem->mutex) < 0)
		return -1;

	//Destroy the condition vairable associated with the semaphore
	if (pthread_cond_destroy(&sem->cond) < 0)
		return -1;

	return 0;
}