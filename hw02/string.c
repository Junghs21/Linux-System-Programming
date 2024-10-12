#include <stdio.h>

/*
 * [Function Name] : int strlen_p(char *src)
 * [Description]   : 
 *  - Calculates the length of a string using a pointer.
 *	- Increase the pointer by 1 and the len by 1 until the null terminator('\0')
 * [Input] 	       : 
 *	char *str - pointer to the string to measure.
 * [Output]        :
 *	- The length of the string using variable len
 * [Call By]	   :
 *	Nothing
 * [Calls]	       :
 *	Nothing
 * [Given]	       :
 *	Nothing
 * [Returns]	   :
 *	Nothing
 */
int
strlen_p(char *str)
{
	int	len = 0;
			
	while (*str++)  {
		len++;
	}
	return len;
}

/*
 * [Function Name] : int strlen_a(char str[])
 * [Description]   :
 * 	- Caculates the length of a string using array idnexing.
 * 	- Iterates through the string array and increments the index until the null terminator is found.
 * [Input]	       :
 * 	char str[] - array of char representing the string to measure.
 * [Output]	       :
 * 	- The length of the string using variable len
 * [Call By[	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
int
strlen_a(char str[])
{
	int i;
			
	for (i = 0 ; str[i] != '\0' ; i++)
		;
	return i;
}

/*
 * [Function Name] : void strcpy_p(char *dst, char *src)
 * [Description]   :
 * 	- Copies a string from the source pointer to the destination pointer using pointers.
 * 	- Moves both src and dst pointers forward as char are copied.
 * [Input]	       :
 * 	char *dst - pointer to the destination where the string will be copied.
 * 	char *src - pointer to the source string to be copied.
 * [Output]	       :
 * 	- String copied to destination pointer.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
void
strcpy_p(char *dst, char *src)
{
	while (*src)  {
		*dst++ = *src++;
	}
	*dst = *src;
}

/*
 * [Function Name] : void strcpy_a(char dst[], char src[])
 * [Description]   :
 * 	- Copies a string from the source array to the destination array using array indexing.
 * 	- Loops through the source array, copying each character to the destination array.
 * [Input]	       :
 * 	char dst[] - destination array where the string will be copied.
 * 	char src[] - source array to copy the string from.
 * [Output]	       :
 * 	- String copied to destination array.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Give]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
void
strcpy_a(char dst[], char src[])
{
	int i;
			
	for (i = 0 ; src[i] != '\0'; i++)
		dst[i] = src[i];
	dst[i] = src[i];
}

/*
 * [Function Name] : void strcat_p(char *dst, char *src)
 * [Description]   :
 * 	- Concatenates two strings using pointers.
 * 	- Moves the destination pointer to the end of the string, then append the source string.
 * [Input]	       :
 * 	char *dst - pointer to the destination string where the source will be appended.
 * 	char *src - pointer to the source string to be concatenated.
 * [Output]	       :
 * 	- Source string added to destination pointer.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
void
strcat_p(char *dst, char *src)
{
	while (*dst++)
		;
	dst--;
	while (*src)  {
		*dst++ = *src++;
	}
	*dst = *src;
}

/*
 * [Function Name] : void strcat_a(char dst[], char src[])
 * [Description]   :
 * 	- Concatenates two strings using array indexing.
 * 	- Find the end of the destination array and appends the source array to it.
 * [Input]	       :
 * 	char dst[] - destination array where the source will be appended.
 * 	char src[] - source array to be concatenated.
 * [Output]	       :
 * 	- Source string added to destination array.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
void
strcat_a(char dst[], char src[])
{
	int i, j;
			
	for (i = 0 ; dst[i] != '\0' ; i++)
		;
	for (j = 0 ; src[j] != '\0' ; j++)
		dst[i+j] = src[j];
	dst[i+j] = src[j];
}

/*
 * [Function Name] : int strcmp_p(char *dst, char *src)
 * [Description]   :
 * 	- Compares two string using pointers.
 * 	- Iterates through both strings char by char until a mismatch is found or the '\0' is reached.
 * [Input]	       :
 * 	char *dst - pointer to the first string to compare.
 * 	char *src - pointer to the second string to compare.
 * [Output]	       :
 * 	- Return 0 if the strings are equal.
 * 	- Returns the difference of the first mismatched characters if the strings differ.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
int
strcmp_p(char *dst, char *src) {
	while(*dst || *src) {
		if(*dst != *src) {
			return *dst - *src;
		}
		dst++;
		src++;
	}
	return 0;
}

/*
 * [Function Name] : int strcmp_a(char dst[], char src[])
 * [Description    :
 * 	- Compares two strings using array indexing.
 * 	- Iterates through both arrays char by char until a mismatched is found or the '\0' is reached
 * [Input]	       :
 * 	char dst[] - first string array to compare.
 * 	char src[] - second string array to compare.
 * [Output]	       :
 * 	- Returns 0 if the strings are equal.
 * 	- Returns the difference of the first mismatched characters if the strings differ.
 * [Call By]	   :
 * 	Nothing
 * [Calls]	       :
 * 	Nothing
 * [Given]	       :
 * 	Nothing
 * [Returns]	   :
 * 	Nothing
 */
int
strcmp_a(char dst[], char src[]) {
	int i;

	for(i = 0; dst[i] || src[i]; i++) {
		if(dst[i] != src[i]) {
			return dst[i] - src[i];
		}
		i++;
	}
	return 0;
}

/*
 * [Program Name]  : Assignment 2
 * [Description]   :
 *  	- Demonstrates various string operations using both pointer and array indexing methods.
 * [Input] 	       :
 * 	- Predefined string literals and arrays are used for demonstration.
 * [Output]	       :
 * 	- The results of string operations(length, copy, concatenate, compare) are printed.
 * [Calls]	       :
 * 	strlen_p, strlen_a - to calculate the length of strings using pointers and arrays.
 * 	strcpy_p, strcpy_a - to copy strings using pointers and arrays.
 * 	strcat_p, strcat_a - to concatenates strings using pointers and arrays.
 * 	strcmp_p, strcmp_a - to compare strings using pointers and arrays.
 * [Special Notes] :
 * 	- strcmp_p, strcmp_a results are based on ASCII values difference of mismatched characters.
 */
main()
{
	int	len1, len2, cmp1, cmp2;
	char	str1[20], str2[20];

	//Start strlen methods	
	len1 = strlen_p("Hello");
	len2 = strlen_a("Hello");
	printf("strlen: p=%d, a=%d\n", len1, len2);
			
	//Start strcpy methods	
	strcpy_p(str1, "Hello");
	strcpy_a(str2, "Hello");
	printf("strcpy: p=%s, a=%s\n", str1, str2);
			
	//Start strcat methods	
	strcat_p(str1, ", World!");
	strcat_a(str2, ", World!");
	printf("strcat: p=%s, a=%s\n", str1, str2);

	//Start Compare two strings using strcmp methods
	cmp1 = strcmp_p("Hello", "Hello");
	cmp2 = strcmp_a("World", "Eorld");
	printf("strcmp: p=%d, a=%d\n", cmp1, cmp2);
}