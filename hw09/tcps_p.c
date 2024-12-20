#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include "tcp.h"

int Sockfd;

/**
 * [Function Name]  : void CloseServer()
 * [Description]    :
 * 	- Closes thes server socket and terminates the server process
 * [Input]	    :
 * 	Nothing
 * [Output]	    :
 * 	Nothing
 * [Call By]	    :
 * 	Signal handlers or other server functions
 * [Calls]	    :
 * 	close() - Closes the specified socket
 * [Given]	    :
 * 	Sockfd - global variable for the server socket descriptor
 * [Returns]	    :
 * 	Nothing
 */
void CloseServer() {
	close(Sockfd);
	printf("\nTCP Server exit.....\n");
	exit(0);
}

/**
 * [Function Name] : void HandleClient(int sockfd)
 * [Description]   :
 * 	- Handles communication with a client, processing requests and sending responses
 * [Input]	   :
 * 	int sockfd - socket file descriptor for the client connection
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Parent process or server loop for client handling
 * [Calls]	   :
 * 	read() - Reads data from the client socket
 * 	write() - Sends data to the client socket
 * [Given]	   :
 * 	sockfd - valid socket descriptor for a connected client
 * [Returns]	   :
 * 	Nothing
 */
void HandleClient(int sockfd) {	//Function to handle client requests in child process

	MsgType msg;
	int n;

	//Read the client's request
	if((n = read(sockfd, (char *)&msg, sizeof(msg))) < 0) {
		perror("read");
		exit(1);
	}
	printf("Received request: %s....", msg.data);

	//Prepare and send a reply
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());
	if (write(sockfd, (char *)&msg, sizeof(msg)) < 0) {
		perror("write");
		exit(1);
	}
	printf("Replied.\n");

	//Close the client socket
	close(sockfd);
	exit(0);	//Exit the child process
}

/**
 * [Program Name] : Assignment 9
 * [Description]  :
 * 	- Implements a simple TCP server that handles client requests and sends replies
 * 	- The server uses sockets for communication and demonstrates basic socket programming
 * [Input]	  :
 * 	Socket descriptors for handling client connections
 * 	Messages received from clients
 * [Output]	  :
 * 	Replies sent to clients through the socket
 * 	Console messages for debugging and status updates
 * [Calls]	  :
 * 	socket() - Creates a socket for communication
 * 	bind() - Binds the socket to a specific port and IP address
 * 	listen() - Listens for incoming client connections
 * 	accept() - Accepts a new client connection
 * [Special Notes]:
 * 	Requires basic knowledge of socket programming and UNIX system calls
 * 	Signal handling is used to ensure proper cleanup during termination
 */
int main(int argc, char* argv[]) {

	int newSockfd, cliAddrLen;
	struct sockaddr_in cliAddr, servAddr;
	pid_t pid;

	//Set up signal handler for graceful termination
	signal(SIGINT, CloseServer);

	//Create a TCP socket
	if((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
    	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available interfaces
	servAddr.sin_port = htons(SERV_TCP_PORT);    // Use predefined port

	//Bind the socket to the server address
	if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("bind");
		exit(1);
	}

	//Start listening for incoming connections
	listen(Sockfd, 5);
	printf("TCP Server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	
	while(1) {
		//Accept a connection from a client 
		newSockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);

		if(newSockfd < 0) {
			perror("accept");
			exit(1);
		}

		//Fork a child process to handle the client request
		if((pid = fork()) < 0) {
			perror("fork");
			
			close(newSockfd);

			continue;
		} 
		else if(pid == 0) {
			//Child process - handle the client
			close(Sockfd);	//Child doesn't need the listening socket
			
			HandleClient(newSockfd);
		}
		else {
			//Parent process - close the connected socket
			close(newSockfd);
		}
	}
}