#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"

#define	MSG_MODE	(0600 | IPC_CREAT)


main()
{
	int		msqid, n;
	MsgType	msg;


	//Create or retrieve a message queue with the specified key
	if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0)  {
		perror("msgget");
		exit(1);
	}

	//Receive a message from the message queue
	if ((n = msgrcv(msqid, (char *)&msg, sizeof(msg), 0, 0)) < 0)  {
		perror("msgrcv");
		exit(1);
	}
	printf("Received request: %s.....", msg.data);

	//Prepare and send a reply message
	msg.type = MSG_REPLY;	//Set the message type to reply
	sprintf(msg.data, "This is a reply from %d.", getpid());	//Format the reply message with the process ID
	if (msgsnd(msqid, (char *)&msg, sizeof(msg), 0) < 0)  {
		perror("msgsnd");
		exit(1);
	}
	printf("Replied.\n");

	sleep(1);

	//Remove the message queue
	if (msgctl(msqid, IPC_RMID, 0) < 0)  {
		perror("msgctl");
		exit(1);
	}
}