#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

/**
 * [Function Name]  : void listDetails(const char *directory, const char *filename)
 * [Description]    : Prints detailed information about the specified file in the given directory.
 * [Input]          : 
 * 	directory - the path of the directory containing the file
 * 	filename  - the name of the file whose details are to be printed
 * [Output]         : 
 * 	Displays file type, permissions, hard links, owner, group, size, modification time, and name.
 * [Call By] 	    : ListDirectoryContents
 * [Calls]	    : lstat, getpwuid, getgraid, strftime, printf
 * [Given]	    :
 * 	Nothing
 * [Returns]	    :
 * 	Nothing
 */
void listDetails(const char *directory, const char *filename) {
	
	struct stat statbuf;
	struct passwd *pwd;
	struct group *grp;
	char timebuf[80];
	char fullpath[1024];	//Full path to the file used for accessing file details

	//Construct the full path to the file
	snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, filename);

	//Get the file status using lstat
	if(lstat(filename, &statbuf) < 0) {
		perror("lstat");

		return;
	}

	//Print file type and permissions
	printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
	printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
	printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
	printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
	printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
	printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
	printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
	printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
	printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");

	//Print the number of hard links
	printf(" %ld", statbuf.st_nlink);

	//Print the owner's name
	if((pwd = getpwuid(statbuf.st_uid)) != NULL) {
		printf(" %s", pwd->pw_name);
	}
	else {
		printf(" %d", statbuf.st_uid);
	}

	//Print the group's name
	if((grp = getgrgid(statbuf.st_gid)) != NULL) {
		printf(" %s", grp->gr_name);
	}
	else {
		printf(" %d", statbuf.st_gid);
	}

	//Print the file size
	printf(" %5ld", statbuf.st_size);

	//Print the modification time
	strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
	printf(" %s", timebuf);

	//Print the file name
	printf(" %s\n", filename);
}

/**
 * [Function Name]  : void listDirectoryContents(const char *path)
 * [Description]    : lists all files and directories in the specified path and prints their details.
 * [Input]	    :
 * 	path - the path of the directory to be listed
 * [Output]	    :
 * 	Displays the total block size and details of each file and directory within the given path.
 * [Call By]	    : main
 * [Calls]	    : opendir, readdir, lstat, snprintf, rewinddir, perror, printf
 * [Given]	    :
 * 	Nothing
 * [Returns]	    :
 * 	Nothing
 */
void listDirectoryContents(const char *path) {

	DIR *dp;
	struct dirent *dep;
	struct stat statbuf;
	long totalBlocks = 0;

	//Open the directory specified by path
	if((dp = opendir(path)) == NULL) {
		perror("opendir");

		return;
	}

	//Read each directory entry to calculate total blocks
	while((dep = readdir(dp)) != NULL) {
		if(strcmp(dep->d_name, ".") == 0 || strcmp(dep->d_name, "..") == 0) {
			continue;
		}

		char fullpath[1024];
		snprintf(fullpath, sizeof(fullpath), "%s/%s", path, dep->d_name);

		if(lstat(fullpath, &statbuf) == 0) {
			totalBlocks += statbuf.st_blocks;
		}
	}

	//Print total blocks
	printf("total %ld\n", totalBlocks / 2);	//Divide by 2 to match the 'ls' output foramat

	//Reset directory pointer to list details
	rewinddir(dp);

	//Read each directory entry
	while((dep = readdir(dp)) != NULL) {
		if(strcmp(dep->d_name, ".") == 0 || strcmp(dep->d_name, "..") == 0) {
			continue;
		}

		listDetails(path, dep->d_name);	//Call listDetails to print details of each file or directory
	}

	closedir(dp);
}

/**
 * [Program Name]  : Assignment 4
 * [Description]   : A program that mimics the behavior of the "ls -l" command, displaying detailed information about files in the specified directory.
 * [Input]	   :
 * 	None directly from the user, but uses the current direcotry as a fixed path.
 * 	It reads directory contents and file details using system calls.
 * [Output]	   :
 * 	Displays total block size and detailed information for each file.
 * [Calls]	   : listDirectoryContents, listDetails, opendir, closedir, perror
 * [Special Notes] :
 * 	- The program assumes the existence of the directory "home/lsp60/hw04" and accesses it directly.
 * 	- Use POSIX library functions and system calls to access directory and file information.
 * 	- No external data or input is required, the output is based on the files in the specified directory,
 */
int main() {

	const char *path = "/home/lsp60/hw04";	//Define the path to the current directory
	DIR *dp = opendir(path);		//Check if the specified directory can be opened

	if(dp == NULL) {
		perror("Failed to open the current directory");
		exit(1);
	}

	closedir(dp);

	listDirectoryContents(".");	//Call the function to list the directory contents

	return 0;
}