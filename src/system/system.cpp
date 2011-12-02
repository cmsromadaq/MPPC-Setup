#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
//#include <HVPowerSupply.h>
#include <PolyScienceChiller.h>
#define msleep(x) usleep((x)*1000)


#ifndef __SUPER_TRANSLATOR_INT2STRING__
#define __SUPER_TRANSLATOR_INT2STRING__
std::string int2str(int i){
        std::ostringstream conv;
        conv << i;
        return conv.str();
}
#endif

int main(int argc, char *argv[]){
	ProgramParameters *setup = new ProgramParameters(argc,argv);
	TestAssembly *assembly = new TestAssembly(setup->GetControllerDevice());
	QDCConnection QDC1(0x00110000);
	QDCConnection QDC2(0x00220000);

	assembly->Debug(setup->Debug(1));

	int i,i1,i2;
	int Data1[4096],Data2[4096];
	int MinHits,Hits[64];
	unsigned long int GateCounter1 = 0;
	unsigned long int GateCounter2 = 0;
	unsigned long int GetGateCounter;
	std::string fname;
	std::ofstream file;
	
	i1 = 0;
	i2 = 0;
	assembly->Reset();
	QDC1.SetIpedRegister(180);
	QDC2.SetIpedRegister(180);
	for(i=0;i<32;i++){
		//if(setup->Mask(i)){
			QDC1.EnableChannel(i);
		/*}else{
			QDC1.DisableChannel(i) ;
		}
		if(setup->Mask(i+32)){*/
			QDC2.EnableChannel(i) ;
		/*}else{
			QDC2.DisableChannel(i) ;
		}*/
		//std::cout << "Threshold for qdc 1 channel " << i << " is: " << QDC1.ReadThresholdValue(i) << std::endl;
		//std::cout << "Threshold for qdc 2 channel " << i << " is: " << QDC2.ReadThresholdValue(i) << std::endl;
	}
	for(int addr=0;addr<8;addr++){
		for(i=0;i<32;i++){
			std::cout << addr << "\t" << i << "\t0x" << std::hex << QDC1.ReadThresholdValue(i) << "\t0x" << QDC2.ReadThresholdValue(i) << std::dec << std::endl;
		}
		GateCounter1=0;
		GateCounter2=0;
		GetGateCounter=0;
		MinHits=0;
		for(i=0;i<64;i++)
			Hits[i]=0;
		fname.assign(setup->GetFName());
		fname.append("-addr"+int2str(addr)+".dat");
		if(setup->Debug(1))
			std::cout << "Opening file: " << fname << std::endl;
		file.open(fname.c_str(),std::ios_base::ate);
		assembly->SetAddress(addr);
		assembly->EnableVoltageMeasurements();
		QDC1.Reset();
		QDC2.Reset();
		if(setup->Verbose())
			std::cout << "Total Samples:  " << setup->GetSamples() << std::endl;
		if(setup->Verbose())
			printf("Progress:         0%%") ;
		while(setup->GetSamples() > MinHits){
			QDC1.QDCReadBLT(4096, Data1) ;
			QDC2.QDCReadBLT(4096, Data2) ;
			setup->SaveData(Data1,&Hits[0],&GateCounter1,0,file);
			setup->SaveData(Data2,&Hits[0],&GateCounter2,1,file);
			MinHits=-1;
			for(i=1;i<64;i++){
				if(setup->Mask(i)){
					if(MinHits<0)
						MinHits=Hits[i];
					if(MinHits>Hits[i])
						MinHits=Hits[i];
				}
			}
			if(setup->Verbose())
				printf("\b\b\b\b%3d%%",100*MinHits/setup->GetSamples()) ;
		}
		if(setup->Verbose()){
			printf("\n") ;
			fflush(stdout) ;
		}
		assembly->DisableVoltageMeasurements();
		file.close();
	}
	assembly->Close();
/*std::cout << "Deleting controller" << std::endl;
	delete assembly;
std::cout << "Deletting chiller " << std::endl;
	delete chiller;
std::cout << "Deleting setup " << std::endl;
	//delete setup;
std::cout << "Deleting bvps " << std::endl;
	delete bvps;
std::cout << "Leaving ..." << std::endl;*/
	return 0;
}

