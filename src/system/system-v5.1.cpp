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

#define Nch 8

int main(int argc, char *argv[]){
	ProgramParameters *setup = new ProgramParameters(argc,argv);
	PolyScienceChiller *chiller = new PolyScienceChiller(setup->GetChillerDevice());
	HVPowerSupply *bvps = new HVPowerSupply();
	TestAssembly *assembly = new TestAssembly(setup->GetControllerDevice());
	CAENVMEUSBLink Link;
	QDCConnection QDC1(Link, 0x00110000) ;
	QDCConnection QDC2(Link, 0x00220000) ;

	assembly->Debug(setup->Debug(1));

	int i,i1,i2;
	int Data1[4096],Data2[4096];
	int MinHits,Hits[64];
	int GateCounter1 = 0;
	int GateCounter2 = 0;
	int GetGateCounter;
	std::string fname;
	std::ofstream file;
	
	/*float *testVolt;
	unsigned short channel[Nch],status[Nch];
	bool  *on, *off;
	int i;
	int result;
	float *par;

	testVolt = new float[Nch];
	on = new bool[Nch];
	off = new bool[Nch];
	par = new float[Nch];

	for (i=0;i<Nch;i++) {
                channel[i]=i+4;
                status[i]=1;
		par[i] = 20.0;
		testVolt[i] = 0.0;
		on[i] = true;
		off[i] = false;
        }
	char* ip="192.168.1.103";
	char* sys="SY1527";
	char* user="admin";
	char* pass="admin";
	std::cout << "warning 1 "  <<  std::endl;
	HVPowerSupply *dev1 = new HVPowerSupply(ip,sys,user,pass);
	std::cout << "warning 2 " <<  std::endl;
        dev1->setCurrent(1,Nch,channel,par);
	std::cout << "warning 3 " <<  std::endl;
	for (i=0;i<Nch;i++)
	        par[i] = 10.0;
        result = dev1->setRampUp(1,Nch,channel,par);
	if(result){
		std::cerr << "ERROR setting ramp up " << result << std::endl;
	}
        printf("Performing power on channels...\n");
        result = dev1->setBias(1,Nch,channel,testVolt);
	std::cout << "warning 4 " << result <<  std::endl;
	result = dev1->pwOnChannel(1,Nch,channel,on);
	if(result){
		std::cout << dev1->GetError() <<  std::endl;
		std::cout << "ERROR setting Power On " << result <<  std::endl;
	}
	msleep(5000);
	dev1->LoadBiasVoltage(1,"/home/utfsm/alan/Documents/test/src/HVPowerSupply/BVPSmap.dat");
	result = dev1->pwOnChannel(1,Nch,channel,off);
	if(result){
		std::cout << dev1->GetError() <<  std::endl;
		std::cout << "ERROR setting Power On " << result <<  std::endl;
	}

	delete testVolt;
	delete on;
	delete off;
	delete par;*/
	int result;
	result = dev1->setCurrent(1,Nch,channel,&par);
	if(result){
		std::cout << "ERROR setting current limit in channel " << result << std::endl;
	}
	par=10;
	result = dev1->setRampUp(1,Nch,channel,&par);
	if(result){
		std::cout << "ERROR setting ramp up in channel " << result << std::endl;
	}
	printf("Performing power on channels...\n");
	result = dev1->setBias(1,length(channel),channel,&testVolt);
	if(result)
		std::cout << "ERROR setting bias voltage " << result << std::endl;
	result = dev1->pwOnChannel(1,length(channel),channel,&on);
	if(result)
		std::cout << "ERROR setting Power On " << result <<  std::endl;

	result = dev1->pwOnChannel(1,length(channel),channel,&off);
	if(result)
		std::cout << "ERROR closing " << std::endl;

	delete dev1;*/







	/*
	bool on = true;
	bool off = false;

	float currentLimit = 20.0;
	float rampUp = 10.0;
	short unsigned int channel,ChannelS[8];
	for (channel=4;channel<12;channel++){
		ChannelS[channel-4] = channel;
	}
	for (channel=0;channel<8;channel++){
		if(bvps->setCurrent(1,1,&channel,&currentLimit)){
			std::cout << "ERROR setting current limit in channel " << channel << std::endl;
		}
		if(bvps->setRampUp( 1,1,&channel,&rampUp)){
			std::cout << "ERROR setting ramp up in channel " << channel << std::endl;
		}
		if(bvps->pwOnBehavior(1,1,&channel,&on)){
			std::cout << "ERROR setting Power On behaviour in channel " << channel << " error: " << bvps->GetError() << std::endl;
		}
		if(bvps->pwOnChannel(1,1,&channel,&bvps_on)){
			std::cout << "ERROR setting Power On in channel " << channel<< " error: " << bvps->GetError() << std::endl;
		}
		msleep(3000);*/
//	}
	//bvps->LoadBiasVoltage(1,setup->GetBVPSmapPath());
	//std::cout << "Trying to turn on channels " << bvps->pwOnChannel(1,length(ChannelS),ChannelS,&bvps_on) << " " << bvps->GetError() << std::endl;
	/*if(bvps->pwOnChannel(1,8,ChannelS,&on)){
		std::cout << "ERROR setting Power On error: " << bvps->GetError() << std::endl;
	}
	msleep(3000);
	std::cout << "Waiting 3s for stabilization of voltage" << std::endl;
	msleep(3000);

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
		QDC1.Reset();
		QDC2.Reset();
		std::cout << "Total Samples:  " << setup->GetSamples() << std::endl;
		printf("Progress:         0%%") ;
		while(setup->GetSamples() > MinHits){
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
			printf("\b\b\b\b%3d%%",100*MinHits/setup->GetSamples()) ;
		}
		printf("\n") ;
		fflush(stdout) ;
		assembly->DisableVoltageMeasurements();
		file.close();
	}*/
	//std::cout << "Trying to turn on channels " << bvps->pwOnChannel(1,length(ChannelS),ChannelS,&off) << std::endl;
//std::cout << "Closing Link" << std::endl;
//	Link.Close();
//std::cout << "Closing controller" << std::endl;
	//assembly->Close();
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

