#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chat.h"

#define MAX_BUF 256

int Sockfd;

/**
 * [Function Name] : void *ReceiveMessages(void *arg)
 * [Description]   :
 * 	- Continuously receives messages from the server and prints them to the console
 * 	- Terminates the client if the server disconnects
 * [Input]	   :
 * 	void *arg - Not used in this implementation
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	main() - Executed as a thread to handle incoming messages
 * [Calls]	   :
 * 	rec() - To receive messages from the server
 * [Given]	   :
 * 	- Assumes the server connection is established and the socket descriptor Sockfd is valid
 * [Returns]	   :
 * 	Nothing
 */
void *ReceiveMessages(void *arg) {	//Thread to receive messages

	char buf[MAX_BUF];
	int n;

	while(1) {
		n = recv(Sockfd, buf, MAX_BUF, 0); //Receive message from server

		if (n <= 0) {
			if (n == 0) {
				fprintf(stderr, "Server terminated.....\n");
			} else {
				perror("recv");
			}
			close(Sockfd);
			exit(1);
		}
		printf("%s", buf); //Print received message
	}
	return NULL;
}

/**
 * [Function Name] : void *SendMessages(void *arg)
 * [Description]   :
 * 	- Continuously reads input from the user and sends messages to the server
 * 	- Terminates the client on critical send errors
 * [Input]	   :
 * 	void *arg - Not used in this implementation
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	main() - Executed as a thread to handle outgoing messages
 * [Calls]	   :
 * 	fgets() - To get user input from stdin
 * 	send() - To send emssages to the server
 * [Given]	   :
 * 	- Assumes the server connection is established and the socket descriptor Sockfd is valid
 * [Returns]	   :
 * 	Nothing
 */
void *SendMessages(void *arg) {	//Thread to send messages

	char buf[MAX_BUF];
	int n;

	while(1) {
		fgets(buf, MAX_BUF, stdin); //Get input from user
		n = send(Sockfd, buf, strlen(buf) + 1, 0); //Send message to server

		if (n < 0) {
			perror("send");
			exit(1);
		}
	}
	return NULL;
}

/**
 * [Function Name] : void CloseClient(int signo)
 * [Description]   :
 * 	- Gracefully terminates the client by closing the socket and printing a termination message
 * [Input]	   :
 * 	int signo - Signal number triggering the termination
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	- Signal handler for SIGINT in main()
 * [Calls]	   :
 * 	close() - To close the socket connection
 * [Given]	   :
 * 	- Assumes the socket descriptor Sockfd is valid and connected
 * [Returns]	   :
 * 	Nothing
 */
void CloseClient(int signo) {	//Handles client termination

	close(Sockfd); // Close socket
	printf("\nChat client terminated.....\n");
	exit(0);
}

/**
 * [Program Name] : TermProject
 * [Description]  :
 * 	- A client program that connects to a chat server
 * 	- Sends and receives messages to/from the server using multi-threading
 * [Input]	  :
 * 	char *argv[] - Command-line arguments, including the server's IP address
 * [Output]	  :
 * 	Nothing
 * [Calls]	  :
 * 	ReceiveMessages() - Handle incoming messages from the server
 * 	SendMessages() - Handle sending messages to the server
 * [Special Notes]:
 * 	- Terminates gracefully when interrupted with SIGINT
 * 	- Requires the server's IP address as a command-line argument
 */
int main(int argc, char *argv[]) {

	struct sockaddr_in servAddr;
	struct hostent *hp;
	pthread_t recvThread, sendThread;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
		exit(1);
	}

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port = htons(SERV_TCP_PORT);

	if (isdigit(argv[1][0])) {
		servAddr.sin_addr.s_addr = inet_addr(argv[1]); //Use IP address directly
	} else {
		if ((hp = gethostbyname(argv[1])) == NULL) {
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length); //Resolve hostname
	}

	if (connect(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("connect");
		exit(1);
	}

	signal(SIGINT, CloseClient); //Graceful termination
	
	printf("Enter ID: ");
	fflush(stdout);
	char buf[MAX_BUF];
	fgets(buf, MAX_BUF, stdin);
	*strchr(buf, '\n') = '\0';
	if (send(Sockfd, buf, strlen(buf) + 1, 0) < 0) {
		perror("send");
		exit(1);
	}
	printf("Press ^C to exit\n");
	
	//Create threads for sending and receiving messages
	pthread_create(&recvThread, NULL, ReceiveMessages, NULL);
	pthread_create(&sendThread, NULL, SendMessages, NULL);

	pthread_join(recvThread, NULL); //Wait for receiving thread
	pthread_join(sendThread, NULL); //Wait for sending thread

	return 0;
}