#include <stdio.h>

#define	MAX_BUF	256

main(int argc, char *argv[])
{
	FILE	*fp;
	char	buf[MAX_BUF];
	int		line;
	
	if (argc != 2)  {
		printf("Usage: %s filename\n", argv[0]);
		exit(1);
	}
	
	if ((fp = fopen(argv[1], "rt")) == NULL)  {
		perror("fopen");
		exit(1);
	}
	
	line = 1;
	//Read each line from the file until the end
	while (fgets(buf, MAX_BUF, fp))  {
		printf("%4d: %s", line++, buf);
	}
	
	fclose(fp);
}