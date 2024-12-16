#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//Semaphores for syncrhonization
sem_t request_sem, response_sem;
char message[1024];	//Shared message buffer

/**
 * [Function Name]  : void* response_thread(void* arg)
 * [Description]    :
 * 	- Handles the response process in a separate thread using semaphores
 * [Input]	    :
 * 	void* arg - Generic argument for the thread function(unused)
 * [Output]	    :
 * 	Nothing
 * [Calls By]	    :
 * 	pthread_create()
 * [Calls]	    :
 * 	sem_wait(), snprintf(), sem_post()
 * [Given]	    :
 * 	Assumes that the shared messsage buffer is managed by the main thread
 * [Returns]	    :
 * 	NULL - Standard return value for thread functions
 */
void* response_thread(void* arg) {

	//Wait for the request semaphore
	sem_wait(&request_sem);

	//Process the received request and prepare the reply
	printf("Received request: %s.....", message);
	snprintf(message, sizeof(message), "This is a reply from %d.", getpid());
	printf("Replied.\n");

	//Signal that the reply is ready
	sem_post(&response_sem);

	return NULL;
}


/**
 * [Program Name]  : Assignment 8
 * [Description]   :
 * 	- Implements multi-thread communication using semaphores
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Prints the sent request and received reply messages
 * [Calls]	   :
 * 	sem_init() - Initializes the semaphore with an initial value
 * 	sem_wait() - Ddcreases the semaphore value or waits if it is 0
 * 	sem_post() - Increases the semaphore value and wakes up waiting threads
 * 	sem_destroy() - Cleans up the semaphore resources
 * [Special Notes] :
 * 	Assumes proper synchornization using semaphores
 */
int main() {

	pthread_t thread;

	//Initialize semaphores
	sem_init(&request_sem, 0, 0);
	sem_init(&response_sem, 0, 0);

	//Create the response thread
	pthread_create(&thread, NULL, response_thread, NULL);

	//Prepare the request message
	snprintf(message, sizeof(message), "This is a request from %d.", getpid());
	printf("Sent a request.....\n");

	//Signal the response thread with the request semaphore
	sem_post(&request_sem);

	//Wait for the response semaphore
	sem_wait(&response_sem);

	//Print the received reply
	printf("Received reply: %s\n", message);

	//Wait for the response thread to finish
	pthread_join(thread, NULL);

	//Clean up
	sem_destroy(&request_sem);
	sem_destroy(&response_sem);
}