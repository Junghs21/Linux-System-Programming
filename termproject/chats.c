#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "chat.h"

#define MAX_CLIENT 5
#define MAX_ID 32
#define MAX_BUF 256

typedef struct {
	int sockfd;	//Socket file descriptor for the client
	int inUse;	//Indicates if the slot is in use
	char uid[MAX_ID];	//User ID
} ClientType;

int Sockfd;
ClientType Client[MAX_CLIENT];

/**
 * [Function Name] : void CloseServer(int signo)
 * [Description]   :
 * 	- Closes all active client connections and the server socket
 * 	- Handles the termination of the server gracefully upon receiving a termination signal
 * [Input]	   :
 * 	int signo - The signal number received by the handler
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	- signal(SIGINT, CloseServer)
 * [Calls]	   :
 * 	close() - To close active sockets
 * [Given]	   :
 * 	- Assumes all client connections are tracked in the Client array
 * [Returns]	   :
 * 	Nothing
 */
void CloseServer(int signo) {	//Handles server termination

	close(Sockfd);	//Close the server socket

	for (int i = 0; i < MAX_CLIENT; i++) {
		if (Client[i].inUse) {
			close(Client[i].sockfd);	//Close client sockets
		}
	}

	printf("\nChat server terminated.....\n");
	exit(0);
}

/**
 * [Function Name] : int GetID()
 * [Desciption]	   :
 * 	- Finds and returns the index of the first available slot in the Client array
 * 	- Marks the slot as "in use" after allocation
 * [Input]	   :
 * 	Nothing
 * [Output]	   :
 * 	int id - index of an available client slot
 * [Call By]	   :
 * 	- main() when a new client connection is established
 * [Calls]	   :
 * 	Nothing
 * [Given]	   :
 * 	- Assumes the Client array is initialized with inUse flags set to 0
 * [Returns]	   :
 * 	- The index of the first available client slot
 * 	- Returns -1 if no slots are available
 */
int GetID() {	//Finds an available client slot

	for (int i = 0; i < MAX_CLIENT; i++) {
		if (!Client[i].inUse) {
			Client[i].inUse = 1;	//Mark slot as in-use
			return i;	//Return the available slot index
		}
	}
	return -1;
}

/**
 * [Function Name] : void SendToOtherClients(int id, char *buf)
 * [Description]   :
 * 	- Sends a message from one client to all other connected clients
 * 	- Prepends the sender's user ID to the message
 * [Input]	   :
 * 	int id - The ID of the client sending the message
 * 	char *buf - The message to be sent
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	- ProcessClientInput() after receiving a message
 * [Calls]	   :
 * 	send() - Sends the message to connected clients
 * [Given]	   :
 * 	- Assumes all active connections are tracked in the Client array
 * [Returns]	   :
 * 	Nothing
 */
void SendToOtherClients(int id, char *buf) {	//Sends messages to all other clients

	char msg[MAX_BUF + MAX_ID];
	sprintf(msg, "%s> %s", Client[id].uid, buf);	//Format the message

	printf("%s", msg);
	fflush(stdout);

	for (int i = 0; i < MAX_CLIENT; i++) {
		if (Client[i].inUse && i != id) {	//Send to all except the sender 
			if (send(Client[i].sockfd, msg, strlen(msg) + 1, 0) < 0) {	//Send message
				perror("send");
			}
		}
	}
}

/**
 * [Function Name] : void ProcessClientInput(int id)
 * [Description]   :
 * 	- Processes input received from a specific client
 * 	- Handles client disconnection and broadcasts messages to other clients
 * [Input]	   :
 * 	int id - The ID of the client whose input is being processed
 * [Output]	   :
 * 	Nothing
 * [Call By]	   :
 * 	- main() when data is ready to be read from a client socket
 * [Calls]	   :
 * 	recv() - Receives data from the client socket
 * 	SendToOtherClients() - Broadcasts messages to other clients
 * [Given]	   :
 * 	- Assumes the Client array tracks active connections
 * [Returns]	   :
 * 	Nothing
 */
void ProcessClientInput(int id) {	//Processes client input

	char buf[MAX_BUF];
	int n = recv(Client[id].sockfd, buf, MAX_BUF, 0);	//Receive client input

	if (n < 0) {	//If client disconnects
		perror("recv");
		Client[id].inUse = 0;	//Mark slot as free
		close(Client[id].sockfd);	//Close the socket
	} else if (n == 0) {
		printf("Client %d log-out(ID: %s).....\n", id, Client[id].uid);
		Client[id].inUse = 0;

		close(Client[id].sockfd);

		strcpy(buf, "log-out.....\n");
		SendToOtherClients(id, buf);	//Forward message to others
	} else {
		SendToOtherClients(id, buf);
	}
}

/**
 * [Program Name] : TermProject
 * [Description]  :
 * 	- Implements a chat server that allows multiple clients to connect and communicate with each other
 * 	- Manages client connections using the select() system call and forwards messages between clients
 * [Input]  	  :
 * 	cliAddrLen - Length of the client address structure
 * 	sockaddr_in - Storing client and server socket addresses
 * 	fd_set readfds - File descriptor set for monitoring sockets
 * 	fd_set allfds - File descriptor set to track all active sockets
 * [Output]	  :
 * 	Nothing
 * [Calls]	  :
 * 	GetID() - Finds an available client slot for a new connection
 * 	ProcessClientInput() - Processes incoming messages from clients
 * 	SendToOtherClients() - Broadcasts a client's message to others
 * [Special Notes]
 * 	- Server supports up to MAX_CLIENT simultaneous connections
 * 	- The TCP port is predefined by SERV_TCP_PORT
 * 	- Server runs indefinitely until interrupted
 */
int main(int argc, char *argv[]) {

	int cliAddrLen, newSockfd;
	struct sockaddr_in cliAddr, servAddr;
	fd_set readfds, allfds;

	signal(SIGINT, CloseServer);	//Set termination handler

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {	//Create a TCP socket
		perror("socket");
		exit(1);
	}

	int one = 1;
	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_TCP_PORT);	//Bind to any interface

	if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {	//Bind socket
		perror("bind");
		exit(1);
	}
	
	listen(Sockfd, 5);	//Start listening for connections
	printf("Chat server started.....\n");

	FD_ZERO(&allfds);
	FD_SET(Sockfd, &allfds);	//Add the server socket to the FD set

	while (1) {
		readfds = allfds;
		
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {	//Monitor sockets
			perror("select");
			exit(1);
		}

		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &readfds)) {
				if (i == Sockfd) {	//New client connection
					cliAddrLen = sizeof(cliAddr);
					newSockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);

					if (newSockfd < 0) {
						perror("accept");
						continue;
					}
					
					int id = GetID();	//Find available client slot
					if (id < 0) {
						printf("Max clients reached. Connection refused.\n");
						close(newSockfd);
					} else {
						Client[id].sockfd = newSockfd;	//Assign socket to client
						FD_SET(newSockfd, &allfds);	//Add to FD set
						
						if (recv(newSockfd, Client[id].uid, MAX_ID, 0) > 0) {
							printf("Client %d log-in(ID: %s).....\n", id, Client[id].uid);
						} else {
							perror("recv");
							close(newSockfd);
							Client[id].inUse = 0;
						}
					}
				} else {
					for (int id = 0; id < MAX_CLIENT; id++) {
						if (Client[id].inUse && Client[id].sockfd == i) {
							ProcessClientInput(id);	//Process client input
							
							if (!Client[id].inUse) {
								FD_CLR(i, &allfds);
							}
							break;
						}
					}
				}
			}
		}
	}
}