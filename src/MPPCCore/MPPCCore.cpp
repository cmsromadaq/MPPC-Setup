#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CAENVMEenvironment.h>         //For this setup communication
#include <TestAssembly.h>
#include <ProgramParameters.h>
#include <PolyScienceChiller.h>
#include <TNtuple.h>
#include "MPPCCore.h"
#define msleep(x) usleep((x)*1000)


#ifndef __SUPER_TRANSLATOR_INT2STRING__
#define __SUPER_TRANSLATOR_INT2STRING__
std::string int2str(int i){
	std::ostringstream conv;
	conv << i;
	return conv.str();
}
#endif

int CORE::instancesCount=0;

CORE::CORE(int argc, char *argv[]) : ProgramParameters(argc,argv){
	if(this->instancesCount!=0){
		std::cerr << "\e[0;31mERROR:\e[0m More than 1 instance of CORE is not allowed." << std::endl;
		exit(1);
	}
	std::fstream file;
	file.open("var/workingflag",std::fstream::in | std::fstream::out);
	int flag;
	if(!file.is_open()){
		std::cerr << "\e[0;31mERROR:\e[0m Imposible to lock the proccess." << std::endl;
		exit(1);
	}
	file >> flag;
	if(flag){
		std::cerr << "\e[0;31mERROR:\e[0m Proccess locked." << std::endl;
		exit(1);
	}
	file << "1";
	file.close();
	//this->chiller = new PolyScienceChiller(this->GetChillerDevice());
	this->assembly = new TestAssembly(this->GetControllerDevice());
	this->QDC1 = new QDCConnection(0x00110000);
	this->QDC2 = new QDCConnection(0x00220000);
#ifdef RUNNINGINM32MODE
	//this->bvps = new HVPowerSupply();
#endif
	this->instancesCount++;
	this->temp = NULL;
}

CORE::~CORE(){
	this->instancesCount--;
	std::fstream file("/opt/public/var/workingflag",std::fstream::out);
	if(!file.is_open()){
		std::cerr << "\e[0;31mERROR:\e[0m Imposible to unlock the proccess." << std::endl;
	}else{
		file << "0";
		file.close();
	}
	if(this->temp != NULL)
		delete this->temp;
}

void CORE::SetDebugLevels(int level){
	this->assembly->Debug(level);
	/*this->chiller->Debug(level);
	this->QDC1->Debug(level);
        this->QDC2->Debug(level);*/
}

int CORE::Initialize(){
	int i;
	this->assembly->Reset();
	//Enable used channels of QDC, and reset the equipment
	for(i=0;i<32;i++){
		if(this->Mask(i)){
			this->QDC1->EnableChannel(i);
		}else{
			this->QDC1->DisableChannel(i);
		}
		if(this->Mask(i+32)){
			this->QDC2->EnableChannel(i);
		}else{
			this->QDC2->DisableChannel(i);
		}
	}
	this->QDC1->Reset();
	this->QDC2->Reset();
	return 0;
}

int CORE::SaveDataFromQDC(){
	int i,MinHits=0,Hits[64];
	int Data1[4096],Data2[4096];
	unsigned long int GateCounter1 = 0;
	unsigned long int GateCounter2 = 0;
	std::string fname;
	std::ofstream file;
	//TNtuple *tupla = new TNtupla(fname.c_str(),"Tupla with data files","Place:Channel:GateCount:Data");
	fname.assign(this->GetFName());
	fname.append("-addr"+int2str(this->addrIndex)+".dat");
	//if(ProgramParameters::Debug(1))
	if(this->Debug(1))
		std::cout << "Opening file: " << fname << std::endl;
	file.open(fname.c_str(),std::ios_base::ate);
	for(i=0;i<64;i++)
		Hits[i]=0;
	this->EnableVoltageMeasurements();
	while(this->GetSamples() > MinHits){
		this->QDC1->QDCReadBLT(4096, Data1);
		this->QDC2->QDCReadBLT(4096, Data2);
		this->InterpreteAndSaveData(Data1,Hits,&GateCounter1,0,file,NULL);
		this->InterpreteAndSaveData(Data2,Hits,&GateCounter2,1,file,NULL);
		MinHits=-1;
		for(i=1;i<64;i++){
			if(ProgramParameters::Mask(i)){
				if(MinHits<0)
					MinHits=Hits[i];
				if(MinHits>Hits[i])
					MinHits=Hits[i];
			}
		}
	}
	this->DisableVoltageMeasurements();
	file.close();
	return 0;
}

void CORE::SetAddrIndex(int index){
	this->addrIndex = index;
	this->assembly->SetAddress(this->addrIndex);
}

int CORE::GetAddrIndex(){
	return this->addrIndex;
}

void CORE::SetMppcIndex(int index){
	this->mppcIndex = index;
}

int CORE::GetMppcIndex(){
	return this->mppcIndex;
}

void CORE::GetDataFileName(char *filename){
	if(filename!=NULL){
		delete filename;
	}
	std::string fileFormat;
	int namelen;
	//fileFormat.assign(ProgramParameters::GetFName());
	fileFormat.assign(this->GetFName());
	fileFormat.append("-addr"+int2str(this->addrIndex)+".dat");
	namelen = fileFormat.size();
	namelen++;
	//filename = new char[fileFormat.length()+1];
	filename = new char[namelen];
	strcpy(filename,fileFormat.c_str());
}

void CORE::EnableVoltageMeasurements(){
	this->assembly->EnableVoltageMeasurements();
}

void CORE::DisableVoltageMeasurements(){
	this->assembly->DisableVoltageMeasurements();
}

int CORE::GetSamples(){
	return ProgramParameters::GetSamples();
}

void CORE::LoadTemp(){
	int i,temper;
	std::ifstream file("etc/temp.map",std::fstream::in);
	this->temp = new int[3];
	if(!file.is_open()){
		std::cerr << "\e[0;31mERROR:\e[0m Imposible to load Temperatures date file." << std::endl;
		exit(1);
	}
	while(file.good()){
		file >> i >> temper;
		if(i>2){
			std::cerr << "\e[0;31mERROR:\e[0m So much temperatures in date file. Please check." << std::endl;
			exit(1);
		}
		this->temp[i] = temper;
	}
	
}

void CORE::SetTemp(int temp){
	if(this->temp == NULL){
		std::cerr << "\e[0;31mERROR:\e[0m No temperature has been loaded" << std::endl;
		exit(1);
	}
	this->tempIndex = temp;
	this->chiller->SetSetpoint((float)this->temp[this->tempIndex]);
}

int CORE::GetTempIndex(){
	return this->tempIndex;
}

float CORE::GetTemp(){
	float temperature=0;
	this->chiller->GetInternalTemperature(&temperature);
	return temperature;
}

#ifdef RUNNINGINM32MODE
void CORE::PowerOnHV(bool state){
	int i;
	short unsigned int channel;
	float par;
	this->bvps->LoadBiasVoltage(1,this->GetBVPSmapPath());
	for(i=0;i<8;i++){
		channel = i+4;
		if(state){
			par = 20.0;
			if(this->bvps->setCurrent(1,1,&channel,&par)){
				this->bvps->GetError();
				exit(1);
			}
			par = 10.0;
			if(this->bvps->setRampUp(1,1,&channel,&par)){
				this->bvps->GetError();
				exit(1);
			}
		}
		if(this->bvps->pwOnChannel(1,1,&channel,&state)){
			this->bvps->GetError();
			exit(1);
		}
	}
}
#endif

void CORE::InterpreteAndSaveData(int *Data, int *Hits, unsigned long int *GateCounter, int board , std::ofstream & file,TNtuple *tupla){
	int InHeader=0;
	int i = 0;
	int Channel=0;
	int dataKind;
	//unsigned long int GetGateCounter = 0;
	std::string buffer;

	//Completing Result Array
	//i=0;
	while(i<4096){
		//std::cout << i << "\t";
		//Cheking for the information Header
		if(Data[i]==-1){
			return;
		}
		dataKind = Data[i]&0x07000000;
		if(InHeader==0 && dataKind==0x02000000){
			//Header read and expected
			InHeader=1;
			i++;
			GateCounter++;
		}else if(InHeader==0 && dataKind!=0x02000000){
			i++;
			continue;
		}else if(InHeader==1 && dataKind==0x02000000){
			//Unexpected Header
			i++;
			continue;
		}else if(InHeader==1 && dataKind==0x04000000){
			//Section End
			InHeader=0;
			i++;
		}
		//When Im inside a valid information section
		if(InHeader==1){
			if((Data[i]&0x07000000)!=0x0){
				//Other than event detected
				//std::cout << "Other than event detected " << i << std::endl;
				i++;
				continue;
			}
			//Obtaining the channel
			if(board==0)
				Channel = (Data[i]&0x001f0000)>>16;
			else if(board==1)
				Channel = 32 + ((Data[i]&0x001f0000)>>16);
			if(this->Mask(Channel) == false){
			//	std::cout << "Channel " << Channel << " disabled " << "\t";
				i++;
				continue;
			}
			if(Hits[Channel] >= this->GetSamples()){
				//std::cout << "Channel " << Channel << " filled " << i << std::endl;
				i++;
				continue;
			}
			//Checking for overflow or underthreshold condition
			if((Data[i]&0x3000) != 0){
				//std::cout << "Channel " << Channel << "in overflow " << i << std::endl;
				i++;
				continue;
			}
			if(file!=NULL){
				//without overflow or underthreshold
				buffer.assign(int2str(*GateCounter)+"\t"+int2str(Channel)+"\t");
				//buffer.assign(int2str(*GateCounter + GetGateCounter)+"\t"+int2str(Channel)+"\t");
				buffer.append(int2str(Data[i]&0xFFF)+"\n");
				file << buffer.c_str();
			}/*else if(tupla!=NULL){
				if(((int)(Channel%16)/8) == 0)
					mppcch = this->addrIndex;
				else
					mppcch = this->addrIndex + 8;
				mppcplace = this->QDCMap(Channel);
				tupla->Fill(mppcplace,mppcch,*GateCounter,Data[i]&0xFFF);
			}*/
			//Integration of GetGateCounter be carefull with this line
			//GetGateCounter = (GetGateCounter < Hits[Channel])?Hits1[Channel]:GetGateCounter;
			Hits[Channel]++;
			//std::cout << "Hits in 4: " << Hits[34] << "..." << std::endl;
			i++;
		}
	}
	//GateCounter += GetGateCounter;
	//std::cout << "Hits in 4: " << Hits[4] << std::endl;
	return;
}


