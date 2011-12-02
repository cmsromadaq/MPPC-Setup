#include <stdio.h>
#include <stdlib.h>
#include "HVPowerSupply.h"
#define Nch 8


unsigned short channel[Nch];
bool  on=true, off=false;

int main(int argc, char* argv[])
{
	if (argc!=2) {
		printf("Error, solo se aceptan dos paràmetros\n\
			VI_onoff <0/1>");		
		return 1;
	}
    HVPowerSupply *dev1 = new HVPowerSupply((char *)"192.168.1.103",(char *)"SY1525",(char *)"admin",(char *)"admin");
	for (int i=0;i<Nch;i++) {
		channel[i]=i+4;
	}
	switch (atoi(argv[1])){
		case 1:	dev1->pwOnChannel(1,length(channel),channel,&on);	
					break;
		case 0:	dev1->pwOnChannel(1,length(channel),channel,&off);	
					break;
		default :	printf("no se hizo nada, comando mal utilizado\n");
					break;
	}	
	delete dev1;
	
return 0;
}

