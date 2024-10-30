#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum1, sum2;	//Global variables to hold partial sums from each tread

/**
 * [Function Name]  : sum_range1(void *arg)
 * [Description]    : 
 * 	- Calculates the sum of numbers from 1 to 50
 * 	- Stores it in a global variable "sum1"
 * [Input]	    :
 * 	Nothing
 * [Output]	    :
 * 	Updates the global variable "sum1" with the calculated sum
 * [Call By]	    :
 * 	Main()
 * [Calls]	    :
 * 	pthread_exit
 * [Given]	    :
 * 	Nothing
 * [Returns]	    :
 * 	Nothing
 */
void *sum_range1(void *arg) {	
	
	for(int i = 1; i <= 50; i++) {
		sum1 += i;
	}
	
	pthread_exit(NULL);	//Exit the thread
}

/**
 * [Function Name]  : sum_range2(void *arg)
 * [Description]    : 
 * 	- Calculates the sum of numbers from 51 to 100
 * 	- Stores it in a global variable "sum2"
 * [Input]	    :
 * 	Nothing
 * [Output]	    :
 * 	Updates the global variable "sum2" with the calculated sum
 * [Call By]	    :
 * 	Main()
 * [Calls]	    :
 * 	pthread_exit
 * [Given]	    :
 * 	Nothing
 * [Returns]	    :
 * 	Nothing
 */
void *sum_range2(void *arg) {	
	
	for(int i = 51; i <= 100; i++) {
		sum2 += i;
	}

	pthread_exit(NULL);	//Exit the thread
}

/**
 * [Program Name] : Assignment 5
 * [Description]  : 
 * 	- Creates two threads to compute the sum of numbers from 1 to 100, divided into two ranges
 * 	- Each thread calculates a partial sum 1 to 50 and 51 to 100
 * 	- main thread waits for both threads to complete, then combines the partial sums
 * [Input]	  :
 * 	Nothing
 * [Output]	  :
 * 	pthread_t tid1, tid2	- Each pthread ID
 * 	int totalSum		- Result of sum from both threads
 * [Calls]	  :
 * 	pthread_create		- Create each thread to calculate the sum
 * 	pthread_join		- Wait for the each tread to finish
 * [Special Note] : Thread synchronization is ensured using pthread_join to wait for each thread's completion
 * 		    Compile with -pthread flag(e.g, gcc -o tadd tadd.c -pthread)
 */
int main() {

	pthread_t tid1, tid2;	//Thread IDs for the two threads

	//Create the first thread to calculate the sum from 1 to 50
	if(pthread_create(&tid1, NULL, sum_range1, NULL) < 0) {
		perror("pthread_create");
		exit(1);
	}
	
	//Create the second thread to calculate the sum from 51 to 100
	if(pthread_create(&tid2, NULL, sum_range2, NULL) < 0) {
		perror("pthread_create");
		exit(1);
	}

	//Wait for the first thread to finish
	if(pthread_join(tid1, NULL) < 0) {
		perror("pthread_join");
		exit(1);
	}

	//Wait for the second thread to finish
	if(pthread_join(tid2, NULL) < 0) {
		perror("pthread_join");
		exit(1);
	}

	int totalSum = sum1 + sum2;	//Calculate the total sum from both threads

	//Print the thread IDs and the result
	printf("Threads terminated: tid1 = %d, tid2 = %d\n totalSum = %d\n", tid1, tid2, totalSum);

}