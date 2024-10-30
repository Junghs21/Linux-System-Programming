#include <stdio.h>

main(int argc, char *argv[], char *envp[])
{
	int			i;
	char		**p;
	extern char	**environ;

	printf("List command-line arguments\n");
	for (i = 0 ; i < argc ; i++)  {	//Loop through command-line arguments
		printf("%s\n", argv[i]);	//Print each command-line argument
	}

	printf("\n");
	printf("List environment variables from environ variable\n");
#if 1	//Condtional compilation: using indexing or pointer for environ
	for (i = 0 ; environ[i] != NULL ; i++)  {	//Loop through environment variables using indexing
		printf("%s\n", environ[i]);	//Print each environment variable using indexing
	}
#else
	for (p = environ ; *p != NULL ; p++)  {	//Loop through environment variables using a pointer
		printf("%s\n", *p);	//Print each environment variable using a pointer
	}
#endif

	printf("\n");
	printf("List environment variables from envp variable\n");
#if 1	//Loop through environment variables using indexing
	for (i = 0 ; envp[i] != NULL ; i++)  {	//Loop through environment variables using indexing
		printf("%s\n", envp[i]);
	}
#else
	for (p = envp ; *p != NULL ; p++)  {	//Loop through environment variables using a pointer
		printf("%s\n", *p);
	}
#endif
}