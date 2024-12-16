#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

main(int argc, char *argv[])
{
	int			fdin, fdout;
	char		*src, *dst;
	struct stat	statbuf;

	//Validate the number of command-line arguments
	if (argc != 3)  {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(1);
	}

	//Open the source file in read-only mode
	if ((fdin = open(argv[1], O_RDONLY)) < 0)  {
		perror("open");
		exit(1);
	}
	//Open the destination file in read/write mode, creating or truncating it
	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)  {
		perror("open");
		exit(1);
	}

	//Get the size of the source file
	if (fstat(fdin, &statbuf) < 0)  {
		perror("fstat");
		exit(1);
	}
	//Adjust the size of the destination file to match the source file size
	if (lseek(fdout, statbuf.st_size-1, SEEK_SET) < 0)  {
		perror("lseek");
		exit(1);
	}
	write(fdout, "", 1);	//Write a dummy byte to allocate space

	//Map the source file to memory in read-only mode
	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) 
			== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}
	//Map the destination file to memory in write mode
	if ((dst = mmap(0, statbuf.st_size, PROT_WRITE, MAP_SHARED, fdout, 0)) 
			== (caddr_t) -1)  {
		perror("mmap");
		exit(1);
	}

	memcpy(dst, src, statbuf.st_size);	//Copy the contents of the source file to the destination file

	close(fdin);
	close(fdout);
}