#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include "unix.h"


int	Sockfd;	//Server socket file descriptor

void
CloseServer()	//Signal handler to close the server and remove the socket file
{
	close(Sockfd);
	if (remove(UNIX_STR_PATH) < 0)  {	//Remove the socket file
		perror("remove");
	}

	printf("\nUNIX-domain Connection-Oriented Server exit.....\n");

	exit(0);
}


main(int argc, char *argv[])
{
	int					newSockfd, servAddrLen, cliAddrLen, n;
	struct sockaddr_un	cliAddr, servAddr;
	MsgType				msg;

	//Set signal handler for graceful termination
	signal(SIGINT, CloseServer);

	//Create a UNIX domain socket
	if ((Sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_STR_PATH);	//Set the socket file path
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	//Bind the socket to the server address
	if (bind(Sockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5);	//Start listening for incoming connections

	printf("UNIX-domain Connection-Oriented Server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		//Accept a connection from a client
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}
		
		//Read the client's request message
		if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
			perror("read");
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		//Prepare and send a reply message
		msg.type = MSG_REPLY;
		sprintf(msg.data, "This is a reply from %d.", getpid());
		if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		//Close the connection
		close(newSockfd);
	}
}