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
	if (remove(UNIX_DG_PATH) < 0)  {	//Remove the socket file
		perror("remove");
	}

	printf("\nUNIX-domain Connection-Less Server exit.....\n");

	exit(0);
}


main(int argc, char *argv[])
{
	int					servAddrLen, cliAddrLen, n;
	struct sockaddr_un	cliAddr, servAddr;
	MsgType				msg;

	//Set signal handler for graceful termination
	signal(SIGINT, CloseServer);

	//Create a UNIX domain socket
	if ((Sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_DG_PATH);
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	//Bind the socket to the server address
	if (bind(Sockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	printf("UNIX-domain Connection-Less Server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		//Receive a request message from the client
		if ((n = recvfrom(Sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  {	
			perror("recvfrom");		
			exit(1);
		}
		printf("Received request: %s.....", msg.data);

		//Prepare and send a reply message
		msg.type = MSG_REPLY;
		sprintf(msg.data, "This is a reply from %d.", getpid());
		if (sendto(Sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
			perror("sendto");
			exit(1);
		}
		printf("Replied.\n");
	}
}