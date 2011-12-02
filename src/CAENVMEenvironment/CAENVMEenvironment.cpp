// QDCDataAqcisitionv1.0
//
// Developer:   Pedro F. Toledo
//              <pedrotoledocorrea@gmail.com>
//
// CAENVMEenvironment.cpp
//
// Description: This file contains the class methodes required for the QDCDataAcqui
//              sition program, been designed for the version 1.0
//
// Libraries-----------------------------------------------------------------------
//nclude <stdio.h>
//nclude <stdarg.h>
//#include <stdint.h> 
//#include "CAENVMElib.h"
//#include <stdlib.h>
//#include <string.h>
#include "CAENVMEenvironment.h"         //Custom Environment
#include <fstream>
#include <iostream>

unsigned int QDCConnection::num = 0;
int32_t QDCConnection::BHandle = 0;

QDCConnection::QDCConnection(int QDCAddressToConnect){
	int Data,result;
	this->debug = false;
	if(this->num == 0){
		this->BHandle = 0;
		short Device = atoi("USB");
		result = CAENVME_Init(cvV1718, 0, Device, &BHandle);
		if(result != cvSuccess ){
			std::cerr << "\e[0;31mERROR:\e[0m ";
			switch(result){
				case -1:
					std::cerr << "VME bus error during the cycle";
					break;
				case -2:
					std::cerr << "Communication error";
					break;
				case -3:
					std::cerr << "Unspecified error";
					break;
				case -4:
					std::cerr << "Invalid parameter";
					break;
				case -5:
					std::cerr << "Timeout error";
					break;
			}
			std::cerr << ", opening the USB Link to V1718 device" << std::endl;
		//if(CAENVME_Init(cvV1718, 0, Device, &BHandle) != cvSuccess ){
			//std::cout << "initializing device in: 0x" << std::hex << QDCAddressToConnect << std::dec << " with error " << result << std::endl;
			exit(1);
		}
		if(this->Debug())
			std::cout << "Connection with CAEN-V1718 Established through USB" << std::endl;
	}
	this->QDCAddress = QDCAddressToConnect;
	if(QDCAddress%0x10000 != 0){
		std::cerr << "\e[0;31mERROR:\e[0m Invalid QDC address" << std::endl;
		exit(7);
	}else{
		if(CAENVME_ReadCycle(QDCConnection::BHandle, this->QDCAddress + 0x1000, &Data, cvA24_U_DATA, cvD16) != cvSuccess){
			std::cerr << "\e[0;31mERROR:\e[0m opening the QDC link on address " << std::hex << this->QDCAddress << std::dec << std::endl;
                	exit(7) ;	
		}
	}
	if(this->Debug()){
		std::cout << "QDC Connected at VME Address " << std::hex << this->QDCAddress;
		std::cout << ": Firmware v" << std::dec << ((Data&0xF000)>>12)*10+((Data&0xF00)>>8);
		std::cout << "." << ((Data&0xF0)>>4);
		std::cout << (Data&0xF) << std::endl;
		//printf("QDC Connected at VME Address %08X: Firmware v%d.%d%d\n",this->QDCAddress,((Data&0xF000)>>12)*10+((Data&0xF00)>>8),(Data&0xF0)>>4,Data&0xF) ;
		}
	this->num++;
}

QDCConnection::~QDCConnection(){
	this->num--;
	if(this->num==0)
		CAENVME_End(this->BHandle);
}

void QDCConnection::QDCWrite(int Address, int Value){
	if(
		Address%2 == 0    and 
		Address >= 0x1002 and 
		Address <= 0x10BF and 
		Address != 0x100E and
		Address != 0x1022 and
		Address != 0x1024 and
		Address != 0x1026 and
		Address != 0x1070 and
		Address != 0x1072 and
		!(Address < 0x1000 and Address > 0x07FC)){
		if(CAENVME_WriteCycle(this->BHandle, this->QDCAddress + Address, &Value, cvA24_U_DATA, cvD16)!=cvSuccess){
			std::cerr << "\e[0;31mERROR:\e[0m writing QDC at address 0x" << std::hex << QDCAddress << std::dec << std::endl;
			exit(2);
		}
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m Invalid Writing QDC Address" << std::endl;
                exit(2) ;
	}
}

void QDCConnection::QDCRead(int Address, int *Value){
	unsigned int Data = 0 ;
	if(
	Address%2 == 0    and
	Address >= 0      and
	Address <= 0x10BF and
	Address != 0x1016 and
	Address != 0x1028 and
	Address != 0x102A and
	Address != 0x1034 and
	Address != 0x1036 and
	Address != 0x1038 and
	Address != 0x103A and
	Address != 0x103E and
	Address != 0x1040 and
	Address != 0x1064 and
	Address != 0x1068 and
	!(Address < 0x1000 and Address > 0x07FC)){
		if(Address <= 0x07FC){
			if(CAENVME_ReadCycle(this->BHandle, this->QDCAddress + Address, &Data, cvA24_U_DATA, cvD32) != cvSuccess){
				std::cerr << "\e[0;31mERROR:\e[0m reading QDC at address 0x" << std::hex << QDCAddress << std::dec << std::endl;
				exit(3);
			}
		}else{
			if(CAENVME_ReadCycle(this->BHandle, this->QDCAddress + Address, &Data, cvA24_U_DATA, cvD16) != cvSuccess){
				std::cerr << "\e[0;31mERROR:\e[0m reading QDC at address 0x" << std::hex << this->QDCAddress << std::endl;
				exit(3);
			}
                }
		*Value = Data;	
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m Invalid Reading QDC Address" << std::endl;
                exit(3) ;
	}
}

int QDCConnection::QDCReadBLT(int Size, int *Data){
	int Total;
        unsigned char *Datai;
	if(Size <= 4096 and Size > 0){
		Datai = (unsigned char*)malloc(Size*sizeof(unsigned char)*4) ;
	        CAENVME_BLTReadCycle(this->BHandle, this->QDCAddress, Datai, 4*4096, cvA24_U_DATA, cvD32, &Total) ;
		for(int i=0;i<Size;i++){
			if(i<Total/4){
	               		Data[i] =(Datai[i*4+3]<<24)+(Datai[i*4+2]<<16)+(Datai[i*4+1]<<8)+Datai[i*4+0] ;
        		}else{
				Data[i] = -1;
			}
		}
        	return Total/4;
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m Invalid BLT Reading Length" << std::endl;
		exit(4) ;
	}
	return 0;
}

void QDCConnection::EnableChannel(int i){
	this->QDCWrite(0x1080+i*2,1);
}

void QDCConnection::DisableChannel(int i){
	this->QDCWrite(0x1080+i*2,0x1F0);
}

int QDCConnection::ReadThresholdValue(int i){
	int Value;
	this->QDCRead(0x1080+i*2,&Value);
	return Value;
}

void QDCConnection::Reset(){
	this->QDCWrite(0x1016,1);
}

/* This method change how the Event Counter count. There are two modes 
   setted by mode. The modes are:
	Mode A (ALL TRG = 1): it counts all events (default);
	Mode B (ALL TRG = 0): it counts only the accepted events
   In the mode B, the Event Counter count only if VETO, FCLR and BUSY
   are not active*/
int QDCConnection::GetBitSet2Register(){
	int Value;
	this->QDCRead(0x1032, &Value);
	return Value;
}

void QDCConnection::SetEventCounterMode(int mode){
	if(mode!=1 && mode!=0){
		std::cerr << "\e[0;31mERROR:\e[0m QDCConnection::SetEventCounterMode: writing in the port." << std::endl;
		exit(1);
	}
	int mask;
	int Value = this->GetBitSet2Register();
	std::cout << "Data:  0x" << std::hex << Value << std::endl;
	mask = (~(1<<14)) & 0x7FFF;
	std::cout << "Mask:  0x" << std::hex << mask << std::endl;
	Value &= mask;
	Value |= (mode << 14);
	std::cout << "Value: 0x" << std::hex << Value << std::endl;
	this->QDCWrite(0x1032,Value);
	/*std::cout << "Value of Bit Set 2 Register is: 0x" << std::hex << Value << std::endl;
	return Value;*/
}

bool QDCConnection::Debug(){
	return this->debug;
}

void QDCConnection::Debug(bool debug){
	this->debug = debug;
}

int QDCConnection::IpedRegister(){
	int Value;
	this->QDCRead(0x1060, &Value);
	return Value;
}

void QDCConnection::SetIpedRegister(int Value){
	this->QDCWrite(0x1060,Value);
}

int QDCConnection::StepThreshold(){
	int Value;
	this->QDCRead(0x1032, &Value);
	return Value&0x100;
}

// Methodes QDCTools---------------------------------------------------------------
/*void QDCDataInterpreter(int Size, int *Data, int *Valid, int Hits[][3]){
        int InHeader=0;
        int i, j;
        int Channel;
	int GateCounter=-1;
	int dataKind;
        //Cleaning Memory
        for(i=0;i<Size;i++){
              	Hits[i][0] = -1;
		Hits[i][1] = -1;
        }
        //Completing Result Array
        i=0;
	j=0; 
        while(i<Size){
//std::cout << "********* in i:" << i << std::endl;
                //Cheking for the information Header
		if(Data[i]==-1){
			*Valid=j;
			return;
		}
		dataKind = Data[i]&0x07000000;
                if(InHeader==0 && dataKind == 0x02000000){
			//std::cout << "Header detected in i:" << i << std::endl;
                        //Header read and expected
                        InHeader = 1;
                        i++;
			GateCounter++;
                }else if(InHeader==0 && dataKind !=0x02000000){
                        i=4096;
                        continue;
                }else if(InHeader==1 && dataKind == 0x02000000){
                        //Unexpected Header
                        //fprintf(stderr,"\nERROR: Unexpected header %d %d\n",i,InHeader);
			i++;
			continue;
			//exit(6) ;
                }else if(InHeader==1 && dataKind == 0x04000000){
                        //Section End
                        InHeader=0;
                        i++;
                }
		//When Im inside a valid information section
                if(InHeader==1){
			//std::cout << "Data detected in i:" << i << std::endl;
                        if((Data[i]&0x07000000) != 0x0){
                                //Other than event detected
                                //fprintf(stderr,"\nERROR: Other than event detected\n");
				i++;
				continue;
                                //exit(6) ;
                        }else{
                                //Obtaining the channel
                                Channel = (Data[i]&0x001f0000)>>16;
                                //Checking for overflow or underthreshold condition
                                if(Data[i]&0x3000){
                                        //For invalid data
                                        i++;
                                }else{
                                        //without overflow or underthreshold
                                        Hits[j][0] = Channel;
					Hits[j][2] = GateCounter;
					Hits[j][1] = Data[i]&0xFFF;
					j++;
                                        i++;
                                }
                        }
                }
        }
	*Valid = j; 
}*/


