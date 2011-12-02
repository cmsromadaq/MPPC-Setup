#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
#include <HVPowerSupply.h>
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
	TestAssembly *assembly = new TestAssembly("/dev/ttyUSB0");
	assembly->Debug(setup->Debug(1));
	//Connecting the USB Link
	CAENVMEUSBLink Link;
	//Coneccting to QDCs
	QDCConnection QDC1(Link, 0x00110000) ;
	QDCConnection QDC2(Link, 0x00220000) ;
	HVPowerSupply *bvps = new HVPowerSupply();
	bvps->LoadBiasVoltage(1,"/home/utfsm/alan/Documents/test/HVPowerSupply/BVPSmap.dat");

	int i,i1,i2;
	int Data1[4096],Data2[4096];
	int MinHits,Hits[64];
	int GateCounter1 = 0;
	int GateCounter2 = 0;
	int GetGateCounter;
	std::string fname;
	std::ofstream file;
	
	/*float bvps_testVolt 70.0;
	unsigned short bvps_channel[12];
	unsigned short bvps_status[12];
	bool bvps_on = true;
	bool bvps_off = false;

	for (i=0;i<12;i++) {
		bvps_channel[i]=i;
		bvps_status[i]=1;
	}
	float par = 20;
	bvps->setCurrent(1,12,bvps_channel,&par);
	par = 10;
	bvps->setRampUp(1,12,bvps_channel,&par);
	bvps->setBias(1,length(bvps_channel),bvps_channel,&bvps_testVolt);
	bvps->pwOnChannel(1,length(bvps_channel),bvps_channel,&bvps_on);
	msleep(3000);*/

/**********************************************************************/
	
	i1 = 0;
	i2 = 0;
	assembly->Reset();
	for(int addr=0;addr<8;addr++){
		GateCounter1=0;
		GateCounter2=0;
		GetGateCounter=0;
		MinHits=0;
		for(i=0;i<64;i++)
			Hits[i]=0;
		fname.assign(setup->GetFName()+"-addr"+int2str(addr)+".dat");
		if(setup->Debug(1))
			std::cout << "Opening file: " << fname << std::endl;
		file.open(fname.c_str(),std::ios_base::ate);
		assembly->SetAddress(addr);
		assembly->EnableVoltageMeasurements();
		//Setting thresholds to 0
		for(i=0;i<32;i++){
			if(setup->Mask(i)){
				QDC1.EnableChannel(i);
			}else{
				QDC1.DisableChannel(i) ;
			}
			if(setup->Mask(i+32)){
				QDC2.EnableChannel(i) ;
			}else{
				QDC2.DisableChannel(i) ;
			}
		}
		//Qdc reset
		QDC1.Reset();
		QDC2.Reset();
		//Starting the acquisition iterations
		std::cout << "Total Samples:  " << setup->GetSamples() << std::endl;
		printf("Progress:         0%%") ;
		while(setup->GetSamples() > MinHits){
			//reading the events stored on the QDC
			QDC1.QDCReadBLT(4096, Data1) ;
			QDC2.QDCReadBLT(4096, Data2) ;
			setup->SaveData(Data1,Hits,&GateCounter1,0,file);
			setup->SaveData(Data2,Hits,&GateCounter2,1,file);
			MinHits=-1;
			for(i=1;i<64;i++){
				if(setup->Mask(i)){
					if(MinHits<0)
						MinHits=Hits[i];
					if(MinHits>Hits[i])
						MinHits=Hits[i];
				}
			}
			//Printing progress
			printf("\b\b\b\b%3d%%",100*MinHits/setup->GetSamples()) ;
		}
		//Clossing seccuence
		printf("\n") ;
		fflush(stdout) ;
		assembly->DisableVoltageMeasurements();
		file.close();
	}
	//Channels->Write();
	/*bvps->pwOnChannel(1,length(bvps_channel),bvps_channel,&bvps_off);
	msleep(100);*/
	Link.Close() ;             //terminates the USB Link */
	assembly->Close();
	delete assembly;
	//delete bvps;
	//delete assembly;
	return 0;
}

