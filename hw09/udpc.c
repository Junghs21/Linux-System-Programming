#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp.h"


main(int argc, char *argv[])
{
	int					sockfd, n, peerAddrLen;
	struct sockaddr_in	servAddr, peerAddr;
	MsgType				msg;

	//Create a UDP socket
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	//Initialize server address structure
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);	//Server address
	servAddr.sin_port = htons(SERV_UDP_PORT);		//Server port

	//Prepare and send a request message
	msg.type = MSG_REQUEST;
	sprintf(msg.data, "This is a request from %d.", getpid());
	if (sendto(sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)  {
		perror("sendto");
		exit(1);
	}
	printf("Sent a request.....");

	//Receive a reply message from the server
	peerAddrLen = sizeof(peerAddr);
	if ((n = recvfrom(sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&peerAddr, &peerAddrLen)) < 0)  {
		perror("recvfrom");
		exit(1);
	}
	printf("Received reply: %s\n", msg.data);

	//Close the socket
	close(sockfd);
}