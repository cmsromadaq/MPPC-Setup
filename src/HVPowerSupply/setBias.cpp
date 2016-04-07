#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include "HVPowerSupply.h"
#include <CAENHVWrapper.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define	msleep(x) usleep((x)*1000)
#define Nch 12


bool power = false, verbose=true;
unsigned slot = 0;
unsigned short channel = 0;
float current, bias;
int main(int argc, char* argv[])
{
    	HVPowerSupply *dev = new HVPowerSupply((char *)"192.168.1.103",SY2527,(char *)"admin",(char *)"admin");
	for (int i=1;i<argc;i++){
		if(argv[i][0]=='-'){
			switch (argv[i][1]){
				case 'p':
					power=( (atoi(argv[++i])>0)?true:false );
					break;
				case 's':
					slot=atoi(argv[++i]);
					break;
				case 'c':
					channel = atoi(argv[++i]);
					break;
				case 'q':
					verbose=false;
					break;
				default	:
					printf("wrong option\n");
					return 1;		
			}
		}
			
	}
	dev->pwOnChannel(slot,1,&channel,&power);
	dev->getBias(slot,1,&channel,&bias);
	dev->getCurrent(slot,1,&channel,&current);
	if (verbose) printf("Channel: %d\nVoltage: %2.2f [V].\nCurrent: %1.2f [uA].\n",channel,bias,current);
	delete dev;
}
