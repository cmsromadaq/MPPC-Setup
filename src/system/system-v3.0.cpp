#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <TFile.h>			//For root file saving	
#include <TH2.h>			//Fot root bidimentional Histograms


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
	//reading acquisition mask
	int i,i1,i2,map[64];
	int Data1[4096],Data2[4096];
	int Hits1[4096][3],Hits2[4096][3];
	int MinHits,Hits[64];
	int GateCounter1 = 0;
	int GateCounter2 = 0;
	int GetGateCounter;
	std::string fname;
	std::ofstream file;

	/*sscanf(argv[1],"%X",&TMask) ;
	Mask=((unsigned long int)TMask)<<32 ;
	sscanf(argv[2],"%X",&TMask) ;
	Mask=Mask+TMask;*/
	
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
			//Iterpreting the data obtained from the QDC
			QDCDataInterpreter(4096, Data1, &i1, Hits1);
			QDCDataInterpreter(4096, Data2, &i2, Hits2);
			if(setup->Debug(1)){
				std::cout << "Valid data from 1:" << i1 <<std::endl;
				std::cout << "Valid data from 2:" << i2 <<std::endl;
			}
			//Filling Histogram
			GetGateCounter=0;
			for(int i=0;i<i1;i++){
				if(setup->Mask(Hits1[i][0])){
					if(Hits[Hits1[i][0]] < setup->GetSamples()){
						if(setup->Debug(3))
							std::cout << "Event:" << Hits1[i][2] + GateCounter1 << " Channel:" << Hits1[i][0] << " Data:" << Hits1[i][1] << std::endl;
						file << Hits1[i][2] + GateCounter1;
						file << "\t";
						file << Hits1[i][0];
						file << "\t";
						file << Hits1[i][1];
						file << std::endl;
						GetGateCounter = (GetGateCounter<Hits1[i][2])?Hits1[i][2]:GetGateCounter;
						Hits[Hits1[i][0]   ]++;
					}
				}
			}
			GateCounter1+=GetGateCounter;
			GetGateCounter=0;
			for(int i=0;i<i2;i++){
				if(setup->Mask(Hits2[i][0]+32)){
					if(Hits[Hits2[i][0]+32] < setup->GetSamples()){
						if(setup->Debug(3))
							std::cout << "Event:" << Hits2[i][2] + GateCounter2 << " Channel:" << Hits2[i][0] << " Data:" << Hits2[i][1] << std::endl;
						file << Hits2[i][2] + GateCounter2;
						file << "\t";
						file << Hits2[i][0]+32;
						file << "\t";
						file << Hits2[i][1];
						file << std::endl;
						GetGateCounter=(GetGateCounter<Hits2[i][2])?Hits2[i][2]:GetGateCounter;
						Hits[Hits2[i][0]+32]++;
					}
				}
			}
			GateCounter2+=GetGateCounter;
			GetGateCounter=0;
			//Measuring the most unfilled histogram
			MinHits=-1;
			for(i=1;i<64;i++){
				if(setup->Mask(i)){
					if(MinHits<0){
						MinHits=Hits[i];
					}
					if(MinHits>Hits[i]){
						MinHits=Hits[i];
					}
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
	Link.Close() ;             //terminates the USB Link */
	assembly->Close();
	delete assembly;
	return 0;
}

