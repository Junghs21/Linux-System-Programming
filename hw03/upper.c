#include <stdio.h>
#include <ctype.h>

/**
 * [Function Name] : void convert_to_uppercase(const char *filename)
 * [Description]   :
 * 	- reads the content of a file character by character.
 * 	- converts each character to uppercase and prints it.
 * [Input]   	   :
 * 	file to be opened and read
 * [Output]	   :
 * 	file content converted to uppercase
 * [Call By]	   :
 * 	Main()
 * [Calls]	   :
 * 	Nothing
 * [Given]	   :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
void convert_to_uppercase(const char *filename) {
	
	FILE *src;
	int ch;	//Variable to hold each character

	//Open the source file in read mode
	if((src = fopen(filename, "r")) == NULL) {
		perror("fopen");

		return;
	}

	//Read each character from the file and convert it to uppercase
	while((ch = fgetc(src)) != EOF) {
		putchar(toupper(ch));
	}

	//Close the file
	fclose(src);
}

/**
 * [Program Name]  : Assignment 3(homework 2)
 * [Description]   :
 * 	- Takes a test file as input and converts its contents to uppercase.
 * 	- printing the result to the screen.
 * [Input]	   :
 * 	A text file name is provided as a command-line argument.
 * 	In this case, text file is "test.in.txt"
 * [Output]	   :
 * 	Capitalized conversion of all the contents of the input file.
 * [Calls]	   :
 * 	calls the "convert_to_uppercase" function to handle the file processing.
 * [Special Notes] :
 * 	- program expects a valid text file as input.
 * 	- If the file does not exist or cannot be opened, an error is displayed.
 */
int main(int argc, char *argv[]) {

	//Check if the correct number of arguments ir provided
	if(argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);

		return 1;
	}

	//Call the function to convert the file content to uppercase
	convert_to_uppercase(argv[1]);

	return 0;
}