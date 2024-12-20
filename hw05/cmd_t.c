#include <stdio.h>
#include <pthread.h>

#define	MAX_CMD		256

void
DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");

	pthread_exit(NULL);
}

main()
{
	char		cmd[MAX_CMD];
	pthread_t	tid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	
		if (cmd[0] == 'q')
			break;

		if (pthread_create(&tid, NULL, (void *)DoCmd, (void *)cmd) < 0)  {	//The thread that performs the DoCmd function is named tid1, and the factor that will deliver the cmd to the DoCmd function is delivered and check error
			perror("pthread_create");
			exit(1);
		}

#if 0
		pthread_join(tid, NULL);
#endif
	}
}