#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
#include <HVPowerSupply.h>
#include <PolyScienceChiller.h>*/
#include <MPPCCore.h>


int main(int argc, char *argv[]){
	CORE *core = new CORE(argc,argv);
	int addr;
	//int temp;

	core->Initialize();
	/*for(temp=0;temp<3;temp++){
		core->SetTemp(temp);*/
#ifdef RUNNINGINM32MODE
		core->PowerOnHV(true);
#endif
		for(addr=0;addr<8;addr++){
			core->SetAddrIndex(addr);
			core->SaveDataFromQDC();
		}
#ifdef RUNNINGINM32MODE
		core->PowerOnHV(false);
#endif
		delete core;
	//}
	return 0;
}

