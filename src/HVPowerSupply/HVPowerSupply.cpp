#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include "HVPowerSupply.h"
#include <CAENHVWrapper.h>
#include <string.h>
#include <signal.h>

HVPowerSupply *ptrDev;

void interrupt_handler(int sig);

HVPowerSupply::HVPowerSupply(){
	this->newInitialized = true;
	this->ip = new char[14];
	strcpy(this->ip,"192.168.1.103");// Set ip of device.
	this->sys = new char[7];
	strcpy(this->sys,"SY1527");// Set name of device (could be any name).
	this->usr = new char[6];// Set user of device ( See device parameters for  ).
	strcpy(this->usr,"admin");
	this->pass = new char[6];
	strcpy(this->pass,"admin");// Set password of device.
	if ( this->connect() ){ //connect to CAEN 1525.
		std::cerr << "\e[0;31mERROR:\e[0m could not connect to caen " << sys << std::endl;
		exit(1);
	}
	ptrDev = this;
	signal(SIGINT,interrupt_handler);
	std::cout << "CAEN Bias Voltage Power Supply " << sys << " connected" << std::endl;
}
	 
HVPowerSupply::HVPowerSupply( char *ip, char *sys, char *usr, char *pass){
	this->newInitialized = false;
	this->ip=ip;
	this->sys=sys;
	this->usr=usr;
	this->pass=pass;
	if (this->connect()){
		std::cerr << "\e[0;31mERROR:\e[0m could not connect caen " << sys << std::endl;
		exit(1);
	}
	ptrDev = this;
	signal(SIGINT,interrupt_handler);
	std::cout << "The Device has been connected" << std::endl;
}

int HVPowerSupply::connect(){
	if(CAENHVInitSystem(this->sys,LINKTYPE_TCPIP,this->ip,this->usr,this->pass)){
		std::cout << "Not conectted" << std::endl;
		return 1;
	}
	return 0;
}
		
int HVPowerSupply::setBias(short slot, unsigned short len, unsigned short *channel, float *value){
	//short *chann=channel;
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"V0Set",len,channel,value) ){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}
	
int HVPowerSupply::getBias(short slot, unsigned short len, unsigned short *channel, float *value){
	short slt=slot;
	if ( CAENHVGetChParam(this->sys,slt,"VMon",len,channel,value) )	{
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}
	
int HVPowerSupply::setCurrent(short slot, unsigned short len, unsigned short *channel, float *value){
	//short *chann=channel;
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"I0Set",len,channel,value) ){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}

int HVPowerSupply::getCurrent(short slot, unsigned short len, unsigned short *channel, float *value){
	short slt=slot;
	if ( CAENHVGetChParam(this->sys,slt,"IMon",len,channel,value) )	{
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}

int HVPowerSupply::setRampUp(short slot, unsigned short len, unsigned short *channel, float *value){
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"RUp",len,channel,value )){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}

int HVPowerSupply::setRampDown(	short slot, unsigned short len, unsigned short *channel, float *value){
	//short *chann=channel;
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"RDwn",len,channel,value) ){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}


int HVPowerSupply::pwOnBehavior( short slot, unsigned short len, unsigned short *channel, bool *value){
	//short chann=channel;
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"Pon",len,channel,value) ){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}
	
int HVPowerSupply::pwOffBehavior( short slot, unsigned short len, unsigned short *channel, bool *value){
	short slt=slot;
	if ( CAENHVSetChParam(this->sys,slt,"PDwn",len,channel,value) ){
		std::cerr << "\e[0;31mERROR:\e[0m " << GetError() << std::endl;
		return 1;
	}
	return 0;
}

int HVPowerSupply::pwOnChannel(	short slot, unsigned short len, unsigned short *channel, bool *value){
	short slt=slot;
	/*if ( CAENHVSetChParam(this->sys,slt,"Pw",len,channel,value) ){
		return 1;
	}
	return 0;*/
	//return (int) CAENHVSetChParam(this->sys,slt,"Pw",len,channel,value);
	int result;
	result = CAENHVSetChParam(this->sys,slt,"Pw",len,channel,value);
	return result;
}
	
HVPowerSupply::~HVPowerSupply(){
	if ( CAENHVDeinitSystem(this->sys) ){
		printf("the device has not been disconnected\n");
	}else{
		if(newInitialized){
			delete this->ip;
			delete this->sys;
			delete this->usr;
			delete this->pass;
		}
		printf("the device has been disconnected\n");
	}
}

void HVPowerSupply::LoadBiasVoltage(short slot, char *path){
	short unsigned int channel;
	float biasVoltage;
	short slt = slot;
	std::ifstream mapFile;
	mapFile.open(path);
	if(mapFile.is_open()){
		while(mapFile.good()){
			mapFile >> channel >> biasVoltage;
			if(channel>11)
				break;
			if(this->setBias(slt,(unsigned short)1,&channel,&biasVoltage)){
				std::cerr << "\e[0;31mERROR:\e[0m HVPowerSupply::LoadBiasVoltage: trying to set the bias voltage readed in channel " << channel << std::endl;
				exit(1);
			}
		}
		mapFile.close();
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m HVPowerSupply::LoadMap: There is not possible to load the map: " << path << std::endl;
		exit(1);
	}
}

std::string HVPowerSupply::GetStatus(short slot, unsigned short len, unsigned short *channel){
	short slt = slot;
	char* buffer = new char[200];
	std::string a="";
	if(CAENHVGetChParam(this->sys,slt,"Status",len,channel,buffer)){
		return a;
	}
	a.assign(buffer);
	return a;
}

char* HVPowerSupply::GetError(){
	return CAENHVGetError(this->sys);
}

void  interrupt_handler(int sig){
	putchar('\n');
	printf("CTRL+C detected\n");
	printf("Shuting down bias voltage device\n");
	delete ptrDev;
	exit(1);
}
