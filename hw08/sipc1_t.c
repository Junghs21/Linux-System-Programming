#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//Mutex and condition vairable for synchronization
pthread_mutex_t mutex;
pthread_cond_t cond;
int flag = 0;	//Flag to indicate request/reply status
char message[1024];	//Shared message buffer

/**
 * [Function Name] : void* response_thread(void* arg)
 * [Description]   : 
 * 	- Handles the response process in a separate thread
 * [Input]	   :
 * 	void* arg - Generic argument for the thread function(unused)
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	pthread_create()
 * [Calls]	   :
 * 	pthread_mutex_lock(), pthread_cond_wait(), snprintf(), pthread_mutex_unlock()
 * [Given]	   :
 * 	Assumes that the shared message and flag are managed by the main thread
 * [Returns]	   :
 * 	Null - Standard return value for thread functions
 */
void* response_thread(void* arg) {

	pthread_mutex_lock(&mutex);

	//Wait until the flag is set by the main thread
	while (flag == 0) {
		pthread_cond_wait(&cond, &mutex);
	}
	//Process the received request and prepare the reply
	printf("Received request: %s.....", message);
	snprintf(message, sizeof(message), "This is a reply from %d.", getpid());
	flag = 0;
	printf("Replied.\n");
	
	//Signal the main thread that the reply is ready
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	return NULL;
}

/**
 * [Program Name]  : Assignment 8
 * [Description]   :
 * 	- Implements multi-thread communication using mutex and condition variables
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Prints the sent request and received reply messages
 * [Calls]	   :
 * 	pthread_mutex_init() - Initializes the mutex for thread synchronization
 * 	pthread_cond_init() - Initializes the condition variable for thread signaling
 * 	pthread_mutex_lock() - Locks the mutex to protect shared resources
 * 	pthread_mutex_unlock() - Unlocks the mutex after accessing shared resources
 * 	pthread_cond_wait() - Makes a thread wait for a condition variable to be signaled
 * 	pthread_cond_signal() - Signals a waiting thread to continue execution
 * [Special Notes] :
 * 	Assumes proper synchornization using mutex and condition variables
 */
int main() {

	pthread_t thread;

	//Initialize mutex and condition variable
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	//Create the response thread
	pthread_create(&thread, NULL, response_thread, NULL);

	pthread_mutex_lock(&mutex);

	//Prepare the request message
	snprintf(message, sizeof(message), "This is a request from %d.", getpid());
	flag = 1;	//Set flag to indicate a request is ready
	pthread_cond_signal(&cond);	//Signal the response thread
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&mutex);

	//Wait until the response thread sets the flag back to 0
	while (flag == 1) {
		pthread_cond_wait(&cond, &mutex);
	}

	//Print the received reply
	printf("Received reply: %s\n", message);
	pthread_mutex_unlock(&mutex);

	//Wait for the response thread to finish
	pthread_join(thread, NULL);

	//Clean up
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}