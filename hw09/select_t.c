#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "select.h"

int TcpSockfd;
int UdpSockfd;
int UcoSockfd;
int UclSockfd;

/**
 * [Function Name] : void CloseServer()
 * [Description]   :
 * 	-Closes all sockets and removes UNIX domain socket files before terminating the server
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Main program or signal handler during server shutdown
 * [Calls]	   :
 * 	close() - Closes the specified socket
 * 	remove() - Removes the specified UNIX domain socket file
 * [Given]	   :
 * 	Global socket descriptors
 * [Returns]	   :
 * 	Nothing
 */
void CloseServer() {	//Close all sockets and remove UNIX domain socket files

	close(TcpSockfd);	//Close TCP socket
	close(UdpSockfd);	//Close UDP socket
	close(UcoSockfd);	//Close UNIX stream socket
	close(UclSockfd);	//Close UNIX datagram socket

	//Remove UNIX domain socket files
	if (remove(UNIX_STR_PATH) < 0) {
		perror("remove");
	}
	if (remove(UNIX_DG_PATH) < 0) {
		perror("remove");
	}

	printf("\nServer daemon exit.....\n");

	exit(0);
}

/**
 * [Function Name] : void MakeTcpSocket()
 * [Description]   :
 * 	- Create and configures a TCP socket for the server
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Initialized TCP socket
 * [Call By]	   :
 * 	Main program during server initialization
 * [Calls]	   :
 * 	socket() - Creates a TCP socket
 * 	bzero() - Zeros out memory for server address structure
 * 	bind() - Binds the socket to an IP address and port
 * [Given]	   :
 * 	Global variable TcpSockfd for storing the socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void MakeTcpSocket() {	//Create and configure a TCP socket

	struct sockaddr_in servAddr;

	//Create a TCP socket
	if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	//Set up server address
	bzero((char *)&servAddr, sizeof(servAddr));	//Zero out the address structure
	servAddr.sin_family = PF_INET;	//Use IPv4
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Accept connections on any interface
	servAddr.sin_port = htons(SERV_TCP_PORT);	//Set port

	//Bind the socket to the address
	if (bind(TcpSockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("bind");
		exit(1);
	}

	listen(TcpSockfd, 5);	//Start listening for connections
}

/**
 * [Function Name] : void MakeUdpSocket()
 * [Description]   :
 * 	- Create and configures a UDP socket for the server
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Initialized UDP socket
 * [Call By]	   :
 * 	Main program during server initialization
 * [Calls]	   :
 * 	socket() - Creates a UDP socket
 * 	bzero() - Zeros out memory for server address structure
 * 	bind() - Binds the socket to an IP address and port
 * [Given]	   :
 * 	Global variable UdpSockfd for storing the socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void MakeUdpSocket() {	//Create and configure a UDP socket

	struct sockaddr_in servAddr;

	//Create a UDP socket
	if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	//Set up server address
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Accept connections on any interface
	servAddr.sin_port = htons(SERV_UDP_PORT);

	//Bind the socket to the address
	if (bind(UdpSockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("bind");
		exit(1);
	}
}

/**
 * [Function Name] : void MakeUcoSocket()
 * [Description]   :
 * 	- Create and configures a UNIX domain stream socket
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Initialized UNIX domain stream socket
 * [Call By]	   :
 * 	Main program during server initialization
 * [Calls]	   :
 * 	socket() - Creates a UNIX domain stream socket
 * 	bzero() - Zeros out memory for server address structure
 * 	bind() - Binds the socket to a file path
 * 	listen() - Starts listening for incoming connections
 * [Given]	   :
 * 	Global variable UcoSockfd for storing the socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void MakeUcoSocket() {

	struct sockaddr_un servAddr;	//UNIX domain socket address structure
	int servAddrLen;

	//Create a UNIX domain socket for stream-based communication
	if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;	//Use UNIX domain protocol
	strcpy(servAddr.sun_path, UNIX_STR_PATH);	//Set the file path for the socket
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);	//Calculate the length of the address

	//Bind the socket to the file path
	if (bind(UcoSockfd, (struct sockaddr *)&servAddr, servAddrLen) < 0) {
		perror("bind");
		exit(1);
	}

	listen(UcoSockfd, 5);	//Start listening for connections
}

/**
 * [Function Name] : void MakeUclSocket()
 * [Description]   :
 * 	- Create and configures a UNIX domain datagram socket
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	Initialized UNIX domain datagram socket
 * [Call By]	   :
 * 	Main program during server initialization
 * [Calls]	   :
 * 	socket() - Creates a UNIX domain datagram socket
 * 	bzero() - Zeros out memory for server address structure
 * 	bind() - Binds the socket to a file path
 * [Given]	   :
 * 	Global variable UclSockfd for storing the socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void MakeUclSocket() {

	struct sockaddr_un servAddr;	//UNIX domain socket address structure
	int servAddrLen;

	//Create a UNIX domain socket for datagram-based communication
	if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;	//Use UNIX domain protocol
	strcpy(servAddr.sun_path, UNIX_DG_PATH);	//Set the file path for the socket
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	//Bind the socket to the file path
	if (bind(UclSockfd, (struct sockaddr *)&servAddr, servAddrLen) < 0) {
		perror("bind");
		exit(1);
	}
}

/**
 * [Function Name] : void *ProcessTcpRequest(void *arg)
 * [Description]   : 
 * 	- Handles a TCP client request in a separate thread, processes the message and sends a reply
 * [Input]	   :
 * 	void *arg - Pointer to an integer representing the socket file descriptor for the client connection
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Nothing
 * [Calls]	   :
 * 	read() - Reads a message from the client socket
 * 	sprintf() - Formats the reply message
 * 	write() - Sends the reply message to the client
 * 	pthread_exit() - Terminates the thread when processing is complete
 * [Given]	   :
 * 	arg points to a valid socket descriptor for a connected TCP client
 * [Returns]	   :
 * 	Nothing
 */
void *ProcessTcpRequest(void *arg) {	//Thread to handle TCP requests

	int newSockfd = *(int *)arg;	//Extract socket file descriptor
	free(arg);	//Free allocated memory for socket descriptor
	MsgType msg;

	//Read message from client
	if (read(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
		perror("read");
		close(newSockfd);	//Close socket
		pthread_exit(NULL);
	}

	printf("Received TCP request: %s.....", msg.data);

	//Prepare reply message
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	//Send reply to client
	if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
		perror("write");
	}

	printf("Replied.\n");
	close(newSockfd);
	pthread_exit(NULL);
}

/**
 * [Function Name] : void *ProcessUdpRequest(void *arg)
 * [Description]   : 
 * 	- Handles a UDP client request, processes the message and sends a reply to the client
 * [Input]	   :
 * 	void *arg - Not used in this function, placeholder for thread argument compatibility
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Nothing
 * [Calls]	   :
 * 	recvfrom() - Receives a message from the client via UDP socket
 * 	sprintf() - Formats the reply message
 * 	sendto() - Sends the reply message to the client
 * 	pthread_exit() - Terminates the thread when processing is complete
 * [Given]	   :
 * 	UdpSockfd - A valid UDP socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void *ProcessUdpRequest(void *arg) {

	MsgType msg;
	struct sockaddr_in cliAddr;	//Client address structure
	socklen_t cliAddrLen = sizeof(cliAddr);

	//Receive a UDP message from the client
	if (recvfrom(UdpSockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0) {
		perror("recvfrom");
		pthread_exit(NULL);
	}

	printf("Received UDP request: %s.....", msg.data);

	//Prepare reply message
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	//Send reply message to the client
	if (sendto(UdpSockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0) {
		perror("sendto");
	}

	printf("Replied.\n");
	pthread_exit(NULL);
}

/**
 * [Function Name] : void *ProcessUcoRequest(void *arg)
 * [Description]   : 
 * 	- Handles a UNIX domain stream socket client request in a separate thread, processes the message and sends a reply
 * [Input]	   :
 * 	void *arg - Pointer to an integer representing the socket file descriptor for the client connection
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Nothing
 * [Calls]	   :
 * 	read() - Reads a message from the client socket
 * 	sprintf() - Formats the reply message
 * 	write() - Sends the reply message to the client
 * 	pthread_exit() - Terminates the thread when processing is complete
 * [Given]	   :
 * 	arg points to a valid socket descriptor for a connected UNIX domain stream socket client
 * [Returns]	   :
 * 	Nothing
 */
void *ProcessUcoRequest(void *arg) {

	int newSockfd = *(int *)arg;	//Extract socket descriptor
	free(arg);	//Free allocated memory for socket descriptor
	MsgType msg;

	//Read message from the client
	if (read(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
		perror("read");
		close(newSockfd);
		pthread_exit(NULL);
	}

	printf("Received UNIX-domain CO request: %s.....", msg.data);

	//Prepare reply message
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	//Send reply message to the client
	if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
		perror("write");
	}

	printf("Replied.\n");
	close(newSockfd);
	pthread_exit(NULL);
}

/**
 * [Function Name] : void *ProcessUclRequest(void *arg)
 * [Description]   : 
 * 	- Handles a UNIX domain datagram socket client request, proccesses the message and sends a reply
 * [Input]	   :
 * 	void *arg - Not used in this function, placeholder for thread argument compatibility
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	Nothing
 * [Calls]	   :
 * 	recvfrom() - Receives a message from the client via UNIX domain datagram socket
 * 	sprintf() - Formats the reply message
 * 	sendto() - Sends the reply message to the client
 * 	pthread_exit() - Terminates the thread when processing is complete
 * [Given]	   :
 * 	UclSockfd - A valid UNIX domain datagram socket descriptor
 * [Returns]	   :
 * 	Nothing
 */
void *ProcessUclRequest(void *arg) {

	MsgType msg;
	struct sockaddr_un cliAddr;	//Client address structure for UNIX domain
	socklen_t cliAddrLen = sizeof(cliAddr);

	//Receive a message from the UNIX domain datagram socket
	if (recvfrom(UclSockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0) {
		perror("recvfrom");
		pthread_exit(NULL);
	}

	printf("Received UNIX-domain CL request: %s.....", msg.data);

	//Prepare reply message
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	//Send reply message to the client
	if (sendto(UclSockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0) {
		perror("sendto");
	}

	printf("Replied.\n");
	pthread_exit(NULL);
}

/**
 * [Program Name] : Assignment 9
 * [Description]  :
 * 	- Handles multiple communication protocols
 * 	- Manages client requests in a multi-thread manner
 * [Input]	  :
 * 	TCP socket connections
 * 	UDP datagrams
 * 	UNIX domain stream and datagram messages
 * [Output]	  :
 * 	Responses sent to clients based on their requests
 * [Calls]	  :
 * 	pthread_create() - Creates threads for handling client requests
 * 	pthread_detach() - Detaches threads to manage resources automatically
 * 	accept() - Accepts new TCP and UNIX domain stream connections
 * 	recvfrom() - Handles incoming UDP and UNIX domain datagram messages
 * 	signal() - Handles SIGINT for proper cleanup
 * 	CloseServer() - Cleans up resources and terminates the server
 * [Special Notes]:
 * 	Uses multi-threading to handle simultaneous client requests
 * 	Ensures resource cleanup using signal handlers and thread detachment
 */
int main(int argc, char *argv[]) {

	pthread_t thread;	//Thread identifier
	struct sockaddr_in cliAddr;
	struct sockaddr_un cliAddrUn;
	socklen_t cliAddrLen;	//Length of client address

	signal(SIGINT, CloseServer);	//Handle SIGINT to clean up resources

	//Initialize server sockets
	MakeTcpSocket();	//Set up TCP socket
	MakeUdpSocket();	//Set up UDP socket
	MakeUcoSocket();	//Create UNIX domain stream socket
	MakeUclSocket();	//Create UNIX domain datagram socket

	printf("Server daemon started.....\n");

	while (1) {
		int *newSockfd = malloc(sizeof(int));	//Allocate memory for new socket descriptor
		//Accept a new TCP connection
		cliAddrLen = sizeof(cliAddr);
		*newSockfd = accept(TcpSockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);

		if (*newSockfd >= 0) {
			//Handle TCP request in a new thread
			pthread_create(&thread, NULL, ProcessTcpRequest, newSockfd);
			pthread_detach(thread);	//Detach thread to free resources automatically
		}

		//Check for incoming UDP datagrams
		cliAddrLen = sizeof(cliAddr);
		if (recvfrom(UdpSockfd, NULL, 0, MSG_PEEK, (struct sockaddr *)&cliAddr, &cliAddrLen) == 0) {
			//Handle UDP request in a new thread
			pthread_create(&thread, NULL, ProcessUdpRequest, NULL);
			pthread_detach(thread);
		}

		//Accept a new UNIX domain stream connection
		cliAddrLen = sizeof(cliAddrUn);
		*newSockfd = accept(UcoSockfd, (struct sockaddr *)&cliAddrUn, &cliAddrLen);

		if (*newSockfd >= 0) {
			//Handle UNIX domain stream request in a new thread
			pthread_create(&thread, NULL, ProcessUcoRequest, newSockfd);
			pthread_detach(thread);
		}

		//Check for incoming UNIX domain datagram messages
		cliAddrLen = sizeof(cliAddrUn);
		if (recvfrom(UclSockfd, NULL, 0, MSG_PEEK, (struct sockaddr *)&cliAddrUn, &cliAddrLen) == 0) {
			//Handle UNIX domain datagram request in a new thread
			pthread_create(&thread, NULL, ProcessUclRequest, NULL);
			pthread_detach(thread);
		}
	}
}