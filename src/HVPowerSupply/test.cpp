#include <fstream>
#include <iostream>
#include <CAENHVWrapper.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "HVPowerSupply.h"
#define	msleep(x) usleep((x)*1000)

int main(int argc, char* argv[]){
	char* sys="SY1527";
	char* ip ="192.168.1.103";
	short unsigned int channel[8];
	bool on = true;
	bool off = false;
	std::cout << "starting ..." << std::endl;
	//HVPowerSupply *bvps = new HVPowerSupply();
	//HVPowerSupply *bvps = new HVPowerSupply("192.168.1.103","SY1525","admin","admin");
	//HVPowerSupply bvps("192.168.1.103","SY1525","admin","admin");

	if(CAENHV_InitSystem(sys,LINKTYPE_TCPIP,ip,"admin","admin")){
		std::cout << "Not conectted" << CAENHV_GetError(sys) << std::endl;
		exit(1);
	}
	if(CAENHV_SetChParam(sys,1,"Pw",8,channel,&on)){
		std::cout << "Not on: " << CAENHV_GetError(sys) << std::endl;
		exit(1);
	}
	msleep(5000);
	if(CAENHV_SetChParam(sys,1,"Pw",8,channel,&off)){
		std::cout << "Not off: " << CAENHV_GetError(sys) << std::endl;
	}
	if(CAENHV_DeinitSystem(sys)){
		std::cout << "Not delete: " << CAENHV_GetError(sys) << std::endl;
		exit(1);
	}
	//short unsigned int channel = 4;
	//bvps.LoadBiasVoltage(1,"/home/utfsm/alan/Documents/test/src/HVPowerSupply/BVPSmap.dat");
/*		float value = 20.0;
		bvps->setCurrent(1,1,&channel,&value);
		value = 10.0;
		bvps->setRampUp(1,1,&channel,&value);
		bvps->pwOnChannel(1,1,&channel,&on);*/
	//std::cout << "Clearing memory..." << std::endl;
	//delete bvps;	
	return 0;
}

