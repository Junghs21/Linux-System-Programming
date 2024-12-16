#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define	NUM_MEN		5	//Number of philosophers
#define	NLOOPS		5	//Number of thinking and eating iterations

#define	LEFT(i)		((i+NUM_MEN-1)%NUM_MEN)	//Left neighbor
#define	RIGHT(i)	((i+1)%NUM_MEN)		//Right neighbor

enum  { THINKING, EATING, HUNGRY };	//Philosopher states

sem_t	Philosopher[NUM_MEN];	//Semaphore for each philosopher
sem_t	Mutex;	//Mutex semaphore for critical section
int		State[NUM_MEN];	//Array to track philosopher states

//Function to simulate thread sleeping for a specified time
void
ThreadUsleep(int usecs)
{
	pthread_cond_t		cond;
	pthread_mutex_t		mutex;
	struct timespec		ts;
	struct timeval		tv;

	//Initialize condition variable and mutex
	if (pthread_cond_init(&cond, NULL) < 0)  {
		perror("pthread_cond_init");
		pthread_exit(NULL);
	}
	if (pthread_mutex_init(&mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		pthread_exit(NULL);
	}

	//Calculate absolute sleep time
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + usecs/1000000;
	ts.tv_nsec = (tv.tv_usec + (usecs%1000000)) * 1000;
	if (ts.tv_nsec >= 1000000000)  {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec++;
	}

	//Wait for the specified time using condition variable
	if (pthread_mutex_lock(&mutex) < 0)  {
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}
	if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0)  {
		perror("pthread_cond_timedwait");
		pthread_exit(NULL);
	}

	//Destroy condition variable and mutex
	if (pthread_cond_destroy(&cond) < 0)  {
		perror("pthread_cond_destroy");
		pthread_exit(NULL);
	}
	if (pthread_mutex_destroy(&mutex) < 0)  {
		perror("pthread_mutex_destroy");
		pthread_exit(NULL);
	}
}

//Simulate thinking
void
Thinking(int id)
{
	printf("Philosopher%d: Thinking.....\n", id);
	ThreadUsleep((rand()%200)*10000);
	printf("Philosopher%d: Want to eat.....\n", id);
}

//Simulate eating
void
Eating(int id)
{
	printf("Philosopher%d: Eating.....\n", id);
	ThreadUsleep((rand()%100)*10000);
}

//Test if a philosopher can eat
void
Test(int id)
{
	if (State[id] == HUNGRY && State[LEFT(id)] != EATING 
			&& State[RIGHT(id)] != EATING)  {
		State[id] = EATING;

		if (sem_post(&Philosopher[id]) < 0)  {	//Signal the philosopher to eat
			perror("sem_post");
			pthread_exit(NULL);
		}
	}
}

//Attempt to pick up chopsticks
void
PickUp(int id)
{
	if (sem_wait(&Mutex) < 0)  {	//Lock critical section
		perror("sem_wait");
		pthread_exit(NULL);
	}

	State[id] = HUNGRY;	//Set state to HUNGRY

	Test(id);	//Check if the philosopher can eat

	if (sem_post(&Mutex) < 0)  {	//Unlock critical section
		perror("sem_post");
		pthread_exit(NULL);
	}

	if (sem_wait(&Philosopher[id]) < 0)  {	//Wait until chopsticks are available
		perror("sem_wait");
		pthread_exit(NULL);
	}
}

//Put down chopsticks
void
PutDown(int id)
{
	if (sem_wait(&Mutex) < 0) {	//Lock critical section
		perror("sem_wait");
		pthread_exit(NULL);
	}

	State[id] = THINKING;	//Set state to THINKING

	Test(LEFT(id));	//Check if the left neighbor can eat
	Test(RIGHT(id));	//Check if the right neighbor can eat

	if (sem_post(&Mutex) < 0)  {	//Unlock critical section
		perror("sem_post");
		pthread_exit(NULL);
	}
}

//Function executed by each philosopher thread
void
DiningPhilosopher(int *pId)
{
	int		i;
	int		id = *pId;

	for (i = 0 ; i < NLOOPS ; i++)  {

		Thinking(id);	//Simulate thinking

		PickUp(id);	//Attempt to pick up chopsticks

		Eating(id);	//Simulate eating

		PutDown(id);	//Put down chopsticks
	}

	printf("Philosopher%d: thinking & eating %d times.....\n", id, i);

	pthread_exit(NULL);
}

main()
{
	pthread_t	tid[NUM_MEN];	//Threads for philosophers
	int			i, id[NUM_MEN];

	srand(0x8888);	//Seed random number generator

	//Initialize semaphores for each philosopher
	for (i = 0 ; i < NUM_MEN ; i++)  {
		if (sem_init(&Philosopher[i], 0, 0) < 0){	//Semaphore starts at 0
			perror("sem_init");
			exit(1);
		}
		id[i] = i;	//Assign ID to each philosopher
	}
	if (sem_init(&Mutex, 0, 1) < 0)  {	//Initialize mutex semaphore
		perror("sem_init");
		exit(1);
	}

	//Create threads for each philosopher
	for (i = 0 ; i < NUM_MEN ; i++)  {
		if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher, 
					(void *)&id[i]) < 0)  {
			perror("pthread_create");
			exit(1);
		}
	}

	//Wait for all threads to finish
	for (i = 0 ; i < NUM_MEN ; i++)  {
		if (pthread_join(tid[i], NULL) < 0)  {
			perror("pthread_join");
			exit(1);
		}
	}

	//Destroy semaphores for each philosopher
	for (i = 0 ; i < NUM_MEN ; i++)  {
		if (sem_destroy(&Philosopher[i]) < 0)  {
			perror("sem_destroy");
		}
	}

	//Destroy the mutex semaphore
	if (sem_destroy(&Mutex) < 0)  {
		perror("sem_destroy");
		exit(1);
	}
}