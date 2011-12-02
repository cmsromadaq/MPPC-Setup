#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>
#include "HVPowerSupply.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#define	msleep(x) usleep((x)*1000)
#define Nch 12

//float valuelist[]={10,20,30,40,50};
/*typedef struct nn {
               time_t         time;
               unsigned short millitm;
               short          timezone;
               short          dstflag;
           }timeb,*ptimeb;*/
void error(const char *msg)
{
    perror(msg);
}

float volt=0.0,curr=0.0,step=10.0,testVolt=0.0;
unsigned short channel[Nch];
bool  on[8]={true}, off[8]={false};
char byte, buffer[256];
float chLimit[Nch]={10,10,10,10,10,10,10,10};
float voltList[Nch]={0},currList[Nch]={0},par=0.0;
double t0,tm;
int i,j,k,statSum=0,Nmed=5,nb=0,nSlot;
long unsigned int tcount=0,tout=300;
struct timeval t;
char *pstr=NULL;
char ofilename[100]={'d','a','t','o','s','.','d','a','t','\0'};
char slotList[6]={'0','\0'};
int delay=0;
std::ofstream ofile;

int main(int argc, char* argv[])
{
	for (i=1;i<argc;i++){
		if(argv[i][0]=='-'){
			switch (argv[i][1]){
				case 'f':
					strcpy(ofilename,argv[++i]);
					break;
				case 'n':
					Nmed=atoi(argv[++i]);
					break;
				case 's':
					nSlot=strlen(argv[++i]);
					strcpy(slotList,argv[i]);
					break;
				case 'd':
					delay = atoi(argv[++i]);
					break;
				default	:
					printf("wrong option\n");
					return 1;		
			}
		}
			
	}
	ofile.open(ofilename);
	unsigned slots[nSlot];
	for (j=0;j<nSlot;j++){
		slots[j]=(unsigned)(slotList[j]-'0');
		//printf("slots[%d]: %d, slotList[%d]: %c\n",j,slots[j],j,slotList[j]);
	}	
	//printf("%s\n",slotList);
	
	unsigned int status[nSlot][Nch];	
	
    	HVPowerSupply *dev1 = new HVPowerSupply((char *)"192.168.1.103",(char *)"SY1525",(char *)"admin",(char *)"admin");
	printf("Performing Current test\nCaen power Supply : %s\n",dev1->GetError());
	printf("slots to be measured ");
	for (j=0;j<nSlot;j++)
		printf(":%d:",slots[j]);
	putchar('\n');
	printf("Number of measurements: %d\n",Nmed);
	
	for (i=0;i<Nch;i++) {
		channel[i]=i;
		for (j=0;j<nSlot;j++)
			status[j][i]=1;
	}	
	/*float par=20;
	dev1->setCurrent(1,Nch,channel,&par);
	par=10;
	dev1->setRampUp(1,Nch,channel,&par);
	printf("Performing power on channels...\n");
	*/
	
	//float curr[length(valuelist)],volt[length(valuelist)];
	ofile<<"#Slot\tChannel\tCount\tVoltage[V]\tCurrent[uA]\ttime[s]"<<std::endl;
	/*dev1->setBias(1,length(channel),channel,&testVolt);
	dev1->pwOnChannel(1,length(channel),channel,off);
	printf("info -1: %s\n",dev1->getError());
	//msleep(1000);
	dev1->pwOnChannel(1,length(channel),channel,on);
	printf("info 0: %s\n",dev1->getError());
	*/
	gettimeofday(&t,NULL);
 	t0=(double)t.tv_sec+(double)t.tv_usec/1000000;
	
	for (i=0;i<Nmed;i++){
	
		
		/*testVolt+=step;
		dev1->setBias(1,length(channel),channel,&testVolt);
		msleep((step==10)?3000:600);
		*/
		
		for (k=0;k<nSlot;k++){
			
			for (j=0;j<(int)length(channel);j++){
				dev1->getBias(slots[k],1,channel+j,&voltList[j]);
				dev1->getCurrent(slots[k],1,channel+j,&currList[j]);
				gettimeofday(&t,NULL);
				tm=((double)t.tv_sec+(double)t.tv_usec/1000000) - t0;
				ofile<<slots[k]<<"\t"<<j<<"\t"<<i<<"\t"<<voltList[j]<<"\t\t"<<currList[j]<<"\t\t"<<tm<<std::endl;
								
			}
			
			
		}
		msleep(delay);
		gettimeofday(&t,NULL);
		tm=((double)t.tv_sec+(double)t.tv_usec/1000000) - t0;
		printf("Complete: %4f%%\t time left: %4f [s]\r",(float)(i+1)/(float)Nmed*100.0,tm*(double)Nmed/(double)i-tm);
	}
	putchar('\n');	
	ofile.close();
	delete dev1;
	return 0;
}

