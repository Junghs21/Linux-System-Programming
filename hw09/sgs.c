#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "sg.h"


int	Sockfd;	//Global socket file descriptor

//Signal handler to close the server
void
CloseServer()
{
	close(Sockfd);
	printf("\nScatter/Gather TCP Server exit.....\n");

	exit(0);
}


main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, n;
	struct sockaddr_in	cliAddr, servAddr;
	MsgType				msg;
	HeaderType			hdr;
	struct iovec		iov[2];	//Scatter, Gather I/O vector

	signal(SIGINT, CloseServer);	//Set signal handler for graceful termination

	//Create a TCP socket
	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Bind to all available interfaces
	servAddr.sin_port = htons(SERV_TCP_PORT);	//Use predefined port

	//Bind the socket to the server address
	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5);	//Start listening for incoming connections

	printf("Scatter/Gather TCP Server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		//Accept a connection from a client
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}											
		
		//Set up scatter, gatter I/O for reading
		iov[0].iov_base = (char *)&hdr;
		iov[0].iov_len = sizeof(hdr);
		iov[1].iov_base = (char *)&msg;
		iov[1].iov_len = sizeof(msg);
		//Read data using scatter, gather I/O
		if ((n = readv(newSockfd, iov, 2)) < 0)  {
			perror("read");
			exit(1);
		}
		printf("Received request: %s(%s).....", msg.data, hdr.info);

		//Prepare and send a reply message using scatter, gather I/O
		strcpy(hdr.info, "REPLY");
		msg.type = MSG_REPLY;
		sprintf(msg.data, "This is a reply from %d.", getpid());
		if (writev(newSockfd, iov, 2) < 0)  {
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		//Close the connection
		close(newSockfd);
	}
}