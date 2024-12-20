#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "unix.h"


main(int argc, char *argv[])
{
	int					sockfd, n, servAddrLen, myAddrLen, peerAddrLen;
	struct sockaddr_un	servAddr, myAddr, peerAddr;
	MsgType				msg;

	//Create a UNIX domain datagram socket
	if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize client (self) address structure
	bzero((char *)&myAddr, sizeof(myAddr));
	myAddr.sun_family = PF_UNIX;
	sprintf(myAddr.sun_path, ".unix-%d", getpid());	//Unique socket file for this client
	myAddrLen = strlen(myAddr.sun_path) + sizeof(myAddr.sun_family);

	//Bind the socket to the client address
	if (bind(sockfd, (struct sockaddr *)&myAddr, myAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_DG_PATH);	//Server socket file path
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	//Prepare and send a request message to the server
	msg.type = MSG_REQUEST;
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (sendto(sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&servAddr, servAddrLen) < 0)  {
		perror("sendto");
		exit(1);
	}
	printf("Sent a request.....");

	//Receive the reply message from the server
	peerAddrLen = sizeof(peerAddr);
	if ((n = recvfrom(sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&peerAddr, &peerAddrLen)) < 0)  {
		perror("recvfrom");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	//Close the socket
	close(sockfd);
	//Remove the cient socket file
	if (remove(myAddr.sun_path) < 0)  {
		perror("remove");
		exit(1);
	}
}