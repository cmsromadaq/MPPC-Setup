#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ProgramParameters.h"
#include <string.h>
#include <fstream>

#ifndef __SUPER_TRANSLATOR_INT2STRING__
#define __SUPER_TRANSLATOR_INT2STRING__
#include <sstream>
std::string int2str(int i){
	std::ostringstream conv;
	conv << i;
	return conv.str();
}
#endif

ProgramParameters::ProgramParameters(int argc,char** argv){
	int i,separator;
	std::string param,temp,option,paramName,paramValue;
	this->debug   = 0;
	this->mask    = 0;
	this->samples = 0;
	this->verbose = true;
	this->qdcMap = NULL;
	this->bvpsMapFile = new char[63];
	strcpy(this->bvpsMapFile, "/home/utfsm/alan/Documents/test/etc/BVPSmap.dat");
	this->file_name_format = NULL;
	this->controllerDevice = new char[13];
	strcpy(this->controllerDevice, "/dev/ttyUSB0");
	this->chillerDevice    = new char[13];
	strcpy(this->chillerDevice, "/dev/ttyUSB1");

	for(i=1;i<argc;i++){
		if(argv[i][0]=='-' && argv[i][1]=='-' && argv[i][2]!='\0'){
			param.assign(argv[i]);
			separator  = param.find_last_of("="); 
			paramName  = param.substr(2,separator-2);
			paramValue = param.substr(1+separator);
			if(paramName=="debug"){
				this->debug = atoi(paramValue.c_str());
			}else if(paramName=="mask"){
#ifdef RUNNINGINM32MODE
				if(paramValue.length()<=8){
					this->mask = 0;
					this->mask2 = strtoul(paramValue.c_str(),NULL,16);
				}else{
					this->mask = strtoul(paramValue.substr(0,8).c_str(),NULL,16);
					this->mask2= strtoul(paramValue.substr(8,paramValue.length()-8).c_str(),NULL,16);
				}
#else
				this->mask = strtoul(paramValue.c_str(),NULL,16);
#endif
			}else if(paramName=="samples"){
				this->samples = atoi(paramValue.c_str());
			}else if(paramName=="file-format"){
				this->file_name_format = (char*) memchr(argv[i],'=',strlen(argv[i]));
				this->file_name_format++;
			}else if(paramName=="controller-device"){
				delete[] this->controllerDevice;
				this->controllerDevice = new char[paramValue.length()+1];
				strcpy(this->controllerDevice,paramValue.c_str());
			}else if(paramName=="chiller-device"){
				delete[] this->chillerDevice;
				this->chillerDevice = new char[paramValue.length()+1];
				strcpy(this->chillerDevice,paramValue.c_str());
			}else if(paramName=="bvps-map"){
				delete[] this->bvpsMapFile;
				this->bvpsMapFile = new char[paramValue.length()+1];
				strcpy(this->bvpsMapFile,paramValue.c_str());
			}else if(paramName=="help"){
				this->Help(argv[0]);
			}else{
				std::cout << "Unrecognized option\t\t: " << paramName << std::endl;
				exit(EXIT_FAILURE);
			}
		}else if(argv[i][0]=='-' && argv[i][1]!='-'){
			if(argv[i][1]=='\0')
				continue;
			if(argv[i][1]=='q'){
				this->verbose = false;
			}else if(argv[i][1]=='v'){
				this->verbose = true;
			}
		}
	}
	if(this->verbose){
		this->Presentation();
		std::cout << std::endl;
	}
	if(this->checkImportantParameters()){
		this->usage(argv[0]);
		exit(5);
	}
}

void ProgramParameters::Presentation(){
	std::cout << "------------------------------------------------------------------------------" << std::endl;
	std::cout << "\t\tDataAcquisitionSystem v2.0" << std::endl;
	std::cout << "Contact     :" << std::endl;
	std::cout << "QDC System  : Pedro Toledo <pedrotoledocorrea@gmail.com>" << std::endl;
	std::cout << "Power Supply: Orlando Soto <orlando.soto@usm.cl>" << std::endl;
	std::cout << "Manager     : Alam Toro S. <alam.toro@usm.cl>" << std::endl;
	std::cout << "------------------------------------------------------------------------------" << std::endl;
	
}

int ProgramParameters::checkImportantParameters(){
	if(this->debug < 0){
		return 1;
	}
#ifdef RUNNINGINM32MODE
	if(this->mask2 > 0 || this->mask > 0){ //Barrido en corriente
		if(this->mask > 0xFFFFFFFF || this->mask2 > 0xFFFFFFFF || (this->mask == 0 && this->mask2 == 0)){
			std::cerr << "\e[0;31mERROR:\e[0m Each part of the Mask must be lower than 0xFFFFFFFF" << std::endl;
			return EXIT_FAILURE;
		}
		if(this->verbose)
			std::cout << "Mask set to\t\t\t: " << std::hex << this->mask << " " << this->mask2 << std::dec << std::endl;
#else
	if(this->mask > 0){ 
		if(this->mask > 0xFFFFFFFFFFFFFFFF){
			std::cerr << "\e[0;31mERROR:\e[0m Mask must be lower than 0xFFFFFFFFFFFFFFFF" << std::endl;
			return EXIT_FAILURE;
		}
		if(this->verbose)
			std::cout << "Mask set to\t\t\t: " << std::hex << this->mask << std::dec << std::endl;
#endif
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m Mask must be a value over 0" << std::endl;
		return EXIT_FAILURE;
	}
	if(this->samples<1){
		std::cerr << "\e[0;31mERROR:\e[0m Samples must be setted" << std::endl;
		return EXIT_FAILURE;
	}else{
		if(this->verbose)
			std::cout << "Samples set to\t\t\t: " << this->samples << std::endl;
	}
	//if(this->file_name_format.compare("")==0){
	if(this->file_name_format == NULL){
		std::cerr << "\e[0;31mERROR:\e[0m A name format is needed" << std::endl;
		return EXIT_FAILURE;
	}else{
		if(this->verbose)
			std::cout << "File name format set to\t\t: " << this->file_name_format << std::endl;
	}
	if(strcmp(this->controllerDevice,"") == 0){
		std::cerr << "\e[0;31mERROR:\e[0m A device for the controller must be specifyed" << std::endl;
		return EXIT_FAILURE;
	}else{
		if(this->verbose)
			std::cout << "Controller device set to\t\t: " << this->controllerDevice << std::endl;
	}
	if(strcmp(this->chillerDevice,"") == 0){
		std::cerr << "\e[0;31mERROR:\e[0m A device for chiller must be specifyed" << std::endl;
		return EXIT_FAILURE;
	}else{
		if(this->verbose)
			std::cout << "Chiller device set to\t\t: " << this->chillerDevice << std::endl;
	}
	return 0;
}

void ProgramParameters::usage(char* argv){
	std::cout << std::endl << "Format is: " << std::endl;
	std::cout << "\t" << argv << "\t--mask=<Mask MPPC:32-1> --samples=<Samples> " << std::endl;
	std::cout << "\t\t--file-format=<Filename> [--debug=<Debug Level>]" << std::endl << std::endl;
	std::cout << "Example:" << std::endl;
	std::cout << "\t" << argv << "\t--mask=FFFF0000 --samples=1000 --file-name=example" << std::endl << std::endl;
	std::cout << "This will acquire 1000 samples from MPPC position 32-17";
	std::cout << ", store them on a set of data files which names begin with ";
	std::cout << "the word example." << std::endl;
}

void ProgramParameters::Help(char* argv){
	this->usage(argv);
	std::cout << std::endl << "Also available options are:" << std::endl;
	std::cout << "--controller-device:\tSpecify the device that represent the Controller" << std::endl;
	std::cout << "--chiller-device:\tSpecify the device that represent the Chiller" << std::endl;
	std::cout << "--bvps-map:\t\tSpecify the path for the channels mapping" << std::endl;
	std::cout << "\t\t\tin the Bias Voltage Power Supply" << std::endl;
	exit(0);
}

ProgramParameters::~ProgramParameters(){
	/*std::cout << "Deleting Bias Voltage Map File Path" << std::endl;
	delete this->bvpsMapFile;
	std::cout << "Deleting Controller Device" << std::endl;
	delete this->controllerDevice;
	std::cout << "Deleting Chiller Device" << std::endl;
	delete this->chillerDevice;*/
	if(this->qdcMap!=NULL)
		this->UnloadQDCMap();
}

bool ProgramParameters::Debug(int level){
	return this->debug&(((unsigned long int)0x1)<<level);
}

bool ProgramParameters::Mask(int i){
	if(i<0 || i>63){
		std::cerr << "\e[0;31mERROR:\e[0m ProgramParamedters::Mask: Mask parameter must be between 0 and 31" << std::endl;
		exit(1);
	}
	unsigned long int temp;
#ifdef RUNNINGINM32MODE
	if(i<32){
		temp = (((unsigned long int)0x1)<<i);
		return this->mask2&temp;
	}else{
		temp = (((unsigned long int)0x1)<<(i-32));
		return this->mask&temp;
	}
#else
	temp = (((unsigned long int)0x1)<<i);
	return this->mask&temp;
#endif
}

int ProgramParameters::GetSamples(){
	return this->samples;
}

char* ProgramParameters::GetFName(){
	return this->file_name_format;
}

void ProgramParameters::SaveData(int *Data, int *Hits, unsigned long int *GateCounter, int board , std::ofstream & file){
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
                        //i=4096;
                        i++;
                        continue;
                }else if(InHeader==1 && dataKind==0x02000000){
                        //Unexpected Header
                        i++;
			continue;
			//exit(6) ;
                }else if(InHeader==1 && dataKind==0x04000000){
                        //Section End
                        InHeader=0;
                        i++;
                }
                //When Im inside a valid information section
                if(InHeader==1){
//printf("%u\t",i);
                        if((Data[i]&0x07000000)!=0x0){
                                //Other than event detected
                                //exit(6) ;
				//std::cout << "Other than event detected " << i << std::endl;
				i++;
				continue;
                        }
			//Obtaining the channel
			if(board==0)
				Channel = (Data[i]&0x001f0000)>>16;
			else if(board==1)
				Channel = 32 + ((Data[i]&0x001f0000)>>16);
//std::cout << "*****************" << Channel << std::endl;
			if(this->Mask(Channel) == false){
				//std::cout << "Channel " << Channel << " disabled " << std::endl;
				i++;
				continue;
			}
//std::cout << "*****************" << Channel << std::endl;
			if(Hits[Channel] >= this->samples){
				//std::cout << "Channel " << Channel << " filled " << i << std::endl;
				i++;
				continue;
			}
			//Checking for overflow or underthreshold condition
			if((Data[i]&0x3000) != 0){
				std::cout << "Channel " << Channel << "in overflow " << i << std::endl;
				i++;
				continue;
			}
//std::cout << Channel << std::endl;
//if(Channel >= 0 && Channel < 64){
			if(file!=NULL){
				//without overflow or underthreshold
				buffer.assign(int2str(*GateCounter)+"\t"+int2str(Channel)+"\t");
				//buffer.assign(int2str(*GateCounter + GetGateCounter)+"\t"+int2str(Channel)+"\t");
				buffer.append(int2str(Data[i]&0xFFF)+"\n");
				file << buffer.c_str();
			}/*else if(tupla!==NULL){
				if(((int)(Channel%16)/8) == 0)
					mppcch = this->addrIndex;
				else
					mppcch = this->addrIndex + 8;
				mppcplace = map[Channel];
				tupla->Fill(mppcplace,mppcch,*GateCounter,Data[i]&0xFFF);
			}*/
			//Integration of GetGateCounter be carefull with this line
			//GetGateCounter = (GetGateCounter < Hits[Channel])?Hits1[Channel]:GetGateCounter;
			Hits[Channel]++;
//std::cout << Channel << "In " << Hits[Channel] << std::endl;
			//std::cout << "Hits in 4: " << Hits[34] << "..." << std::endl;
			i++;
//}
                }
        }
	//GateCounter += GetGateCounter;
	//std::cout << "Hits in 4: " << Hits[4] << std::endl;
	return;
}

char* ProgramParameters::GetControllerDevice(){
	return this->controllerDevice;
}

char* ProgramParameters::GetChillerDevice(){
	return this->chillerDevice;
}

char* ProgramParameters::GetBVPSmapPath(){
	return this->bvpsMapFile;
}

void ProgramParameters::LoadQDCMap(){
	int i,j;
	std::ifstream mapFile;
	mapFile.open("/data/test/etc/qdcconfig.map");
	if(mapFile.is_open()){
		this->qdcMap = new int[64];
		while(mapFile.good()){
			mapFile >> i >> j;
			if(i>=0 && i<64)
				this->qdcMap[i] = j;
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m Problems trying to map QDC Channels" << std::endl;
	}
	mapFile.close();
}

void ProgramParameters::UnloadQDCMap(){
	delete this->qdcMap;
}

int ProgramParameters::QDCMap(int channel){
	if(this->qdcMap == NULL)
		this->LoadQDCMap();
	return this->qdcMap[channel];
}

bool ProgramParameters::Verbose(){
	return this->verbose;
}
