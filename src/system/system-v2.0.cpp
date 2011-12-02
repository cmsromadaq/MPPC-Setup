//---------------------------------------------------------------------------------
// QDCDataAcquisitionv1.0
// Developer:   Pedro F. Toledo 
//              <pedrotoledocorrea@gmail.com>
// Description: Acquisition system for dual QDC CAEN v792 through a VMEUSB CAEN v17
//              18 and data storage on a bidimentional CERN Root Histogram
//Libraries------------------------------------------------------------------------
#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
#include <iostream>
#include <fstream>
#include <sstream>
//ROOT Libraries-------------------------------------------------------------------
#include <TFile.h>			//For root file saving	
#include <TH2.h>			//Fot root bidimentional Histograms
//System defines-------------------------------------------------------------------
//Local Variables------------------------------------------------------------------
/*int             	Data1[4096];           	//For QDC1 Blt Reads
int			Data2[4096];		//For QDC2 Blt Reads
int			Hits1[4096][3];		//Fot QDC1 valid hits
int			Hits2[4096][3];		//For QDC2 valid hits
int			i1;			//For QDC1 valid hits count
int			i2;			//For QDC2 valid hits count*/
/*int   	          	Hits[64];		//Counted Hits for Each Channel
int			MinHits;		//Minimum counted hits*/
//char			HDest;			//Root File Histogram Destination
/*int                     GateCounter1=0;         //For gate counter events shift
int                     GateCounter2=0;         //For gate counter events shift
int                     GetGateCounter;         //For internal reading*/
//main-----------------------------------------------------------------------------
std::string int2str(int i){
        std::ostringstream conv;
        conv << i;
        return conv.str();
}

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
	int i,i1,i2,j,map[64];
	int Data1[4096],Data2[4096];
	int Hits1[4096][3],Hits2[4096][3];
	int MinHits,Hits[64];
	int GateCounter1=0;
	int GateCounter2=0;
	int GetGateCounter;
	std::string fname;
	std::ofstream file[32];
	
	// Loading Map from MPPC Place to QDC Channel
	std::ifstream mapfile("/home/utfsm/alan/Documents/test/system/map.dat");
	if(mapfile.is_open()){
		while(mapfile.good()){
			mapfile >> i >> j;
			if(i>=0 && i<64)
				map[i] = j;
		}
		std::cout << "Channels already mapped" << std::endl;
		mapfile.close();
	}else{
		fprintf(stderr,"ERROR: There is not possible to map channel. Be careful");
		for(i=0;i<64;i++)
			map[i] = i;
	}

	assembly->Reset();
	for(i=0;i<32;i++){
		if(setup->Mask(i)){
			fname.assign(setup->GetFName());
			fname.append("-MPPC"+int2str(i)+".dat");
			file[i].open(fname.c_str(),std::ios_base::ate);
		}
	}
	for(int addr=0;addr<8;addr++){
		GateCounter1=0;
		GateCounter2=0;
		GetGateCounter=0;
		MinHits=0;
		for(i=0;i<64;i++){
			Hits[i]=0;
		}
		assembly->SetAddress(addr);
		assembly->EnableVoltageMeasurements();
		for(i=0;i<32;i++){
			if(setup->Mask(map[i])){
				QDC1.EnableChannel(i) ;
			}else{
				QDC1.DisableChannel(i) ;
			}
			if(setup->Mask(map[i+32])){
				QDC2.EnableChannel(i) ;
			}else{
				QDC2.DisableChannel(i) ;
			}
		}
		//Qdc reset
		QDC1.Reset();
		QDC2.Reset();
		//Starting the acquisition iterations
		printf("Total Samples:  %d\n",setup->GetSamples());
		printf("Progress:         0%%") ;
		while(setup->GetSamples() > MinHits){
			//reading the events stored on the QDC
			QDC1.QDCReadBLT(4096, Data1) ;
			QDC2.QDCReadBLT(4096, Data2) ;
			//Iterpreting the data obtained from the QDC
			QDCDataInterpreter(4096, Data1, &i1, Hits1) ;
			QDCDataInterpreter(4096, Data2, &i2, Hits2) ;
			if(setup->Debug(1)){
				std::cout << "Valid data from 1:" << i1 <<std::endl;
				std::cout << "Valid data from 2:" << i2 <<std::endl;
			}
			//Filling Histogram
			GetGateCounter=0;
			for(i=0;i<i1;i++){
				if(setup->Mask(map[Hits1[i][0]])){
					if(Hits[Hits1[i][0]   ]<setup->GetSamples()){
						file[map[Hits1[i][0]]] << Hits1[i][2] + GateCounter1;
						file[map[Hits1[i][0]]] << "\t";
						if(Hits1[i][0]<8)
							file[map[Hits1[i][0]]] << addr;
						else
							file[map[Hits1[i][0]]] << addr + 8;
						file[map[Hits1[i][0]]] << "\t";
						file[map[Hits1[i][0]]] << Hits1[i][1];
						file[map[Hits1[i][0]]] << std::endl;
						GetGateCounter=(GetGateCounter<Hits1[i][2])?Hits1[i][2]:GetGateCounter;
						Hits[Hits1[i][0]   ]++;
					}
				}
			}
			GateCounter1+=GetGateCounter;
			GetGateCounter=0;
			for(i=0;i<i2;i++){
				if(setup->Mask(Hits2[i][0]+32)){
					if(Hits[Hits2[i][0]+32]<setup->GetSamples()){
						file[map[Hits2[i][0]+32]] << Hits2[i][2] + GateCounter2;
						file[map[Hits2[i][0]+32]] << "\t";
						if((Hits2[i][0] + 32) < 8)
							file[map[Hits2[i][0]+32]] << addr;
						else
							file[map[Hits2[i][0]+32]] << addr + 8;
						file[map[Hits2[i][0]+32]] << "\t";
						file[map[Hits2[i][0]+32]] << Hits2[i][1];
						file[map[Hits2[i][0]+32]] << std::endl;
						GetGateCounter=(GetGateCounter<Hits2[i][2])?Hits2[i][2]:GetGateCounter;
						Hits[Hits2[i][0]+32]++;
					}
				}
			}
			GateCounter2+=GetGateCounter;
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
	//	file.close();
	}
	//Channels->Write();
	for(i=0;i<32;i++){
		file[i].close();
	}
	Link.Close() ;             //terminates the USB Link */
	assembly->Close();
	delete assembly;
	return 0;
}

