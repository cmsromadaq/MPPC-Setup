#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <fstream>
#include <iostream>
#include "HVPowerSupply.h"
#include <CAENHVWrapper.h>
#define	msleep(x) usleep((x)*1000)
#define Nch 8



float volt=0.0,curr=0.0,step=10.0,testVolt=0.0;
unsigned short channel[Nch],status[Nch];
bool  on=true, off=false;
float chLimit[Nch]={17,17,17,17,17,17,17,17};
float voltList[Nch]={0},currList[Nch]={0},par=0.0,tm;
double t0;
int i,j,k,statSum=0,Nmed=10;
struct timeb t;


std::ofstream ofile;

int main(int argc, char* argv[]){
	if (argc>3) {
		printf("Error, solo se aceptan dos paràmetros\n\
			VI_Test [<nombre archivo>] [<numero de mediciones>]");		
		return 1;
	}else if (argc > 2){
		ofile.open(argv[1]);
		Nmed=atoi(argv[2]);
	}else if (argc > 1)
		ofile.open(argv[1]);
	else
		ofile.open("datos.dat");
	HVPowerSupply *dev1 = new HVPowerSupply("192.168.1.103",SY2527,"admin","admin");
	printf("Performing bias test\n");
	for (i=0;i<Nch;i++) {
		channel[i]=i+4;
		status[i]=1;
	}	
	float par=20;
	dev1->setCurrent(1,Nch,channel,&par);
	par=10;
	dev1->setRampUp(1,Nch,channel,&par);
	printf("Performing power on channels...\n");
	ofile<<"Channel\tCount\tstatus\tVoltage\tCurrent\ttime"<<std::endl;
	dev1->setBias(1,length(channel),channel,&testVolt);
	std::cout << dev1->pwOnChannel(1,length(channel),channel,&on) << std::endl;
	ftime(&t);
	t0=(double)t.time+(double)t.millitm/1000;
	for (i=0;i<Nmed;i++){
		testVolt+=step;
		dev1->setBias(1,length(channel),channel,&testVolt);
		msleep((step==10)?3000:600);
		for (k=0;k<50;k++){
			statSum=0;
			dev1->getBias(1,Nch,channel,voltList);
			dev1->getCurrent(1,Nch,channel,currList);
			ftime(&t);
			tm=(double)t.time+(double)t.millitm/1000-t0;		
			for (int j=0;j<length(channel);j++){		
				ofile<<j<<"\t"<<i<<"\t"<<status[j]<<"\t"<<voltList[j]<<"\t"<<currList[j]<<"\t"<<tm<<std::endl;
				statSum+=status[j];
				//printf("channel %d --> Vmon: %f\tImon: %f\ttime: %f\n",j,voltList[j],currList[j],tm);
				if(currList[j]>chLimit[j]){
					status[j]=0;
					dev1->pwOnChannel(1,1,channel+j,&off);			
				}			
			}
		}
		step=(testVolt<60)?10:(testVolt<70)?1:0.05;
		msleep(100);
		if(statSum<1)
			break;
	}
	dev1->pwOnChannel(1,length(channel),channel,&off);
	ofile.close();
	delete dev1;
	return 0;
}

