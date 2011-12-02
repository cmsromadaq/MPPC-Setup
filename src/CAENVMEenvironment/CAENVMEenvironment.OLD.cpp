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
// Methodes CAENVMEUSBLink---------------------------------------------------------
CAENVMEUSBLink::CAENVMEUSBLink(void){
        short Device ;
        Device = atoi("USB") ;
        if(CAENVME_Init(cvV1718, Link, Device, &BHandle) != cvSuccess ){
                printf("\n\n Error opening the USB Link to the device\n") ;
                exit(1) ;
        }
	printf("Connection with CAEN-V1718 Established through USB\n") ;
}
void CAENVMEUSBLink::ReadInternalRegister(int Address, int *Value){
	unsigned short RegRead;
        CAENVME_ReadRegister(BHandle,(CVRegisters) Address, &RegRead) ;
        *Value = (int) RegRead;
}
void CAENVMEUSBLink::Close(void){
	CAENVME_End(BHandle) ;
}
// Methodes QDCConnection----------------------------------------------------------
QDCConnection::QDCConnection(CAENVMEUSBLink USBLink, int QDCAddressToConnect){
	int Data;
	BHandle = USBLink.BHandle ;
	QDCAddress = QDCAddressToConnect ;
	if(QDCAddress%0x10000 != 0){
		printf("\n\n Invalid QDC address\n") ;
                exit(7) ;
	}
	else{
		if(CAENVME_ReadCycle(BHandle, QDCAddress + 0x1000, &Data, cvA24_U_DATA, cvD16) != cvSuccess){
			printf("\n\n Error opening the QDC link on address %08X\n",QDCAddress);
                	exit(7) ;	
		}
	}
	printf("QDC Connected at VME Address %08X: Firmware v%d.%d%d\n",QDCAddress,((Data&0xF000)>>12)*10+((Data&0xF00)>>8),(Data&0xF0)>>4,Data&0xF) ;
}
void QDCConnection::QDCWrite(int Address, int Value){
	int chk;
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
		chk=CAENVME_WriteCycle(BHandle, QDCAddress + Address, &Value, cvA24_U_DATA, cvD16) ;
	}
	else{
		printf("\n\n Invalid Writing QDC Address\n") ;
                exit(2) ;
	}
	if(chk != cvSuccess){
		printf("\n\n Error writing QDC at address 0x%08X\n", QDCAddress) ;
		exit(2) ;
	}
}
void QDCConnection::QDCRead(int Address, int *Value){
	int chk;
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
			chk=CAENVME_ReadCycle(BHandle, QDCAddress + Address, &Data, cvA24_U_DATA, cvD32) ;
		}
		else{
			chk=CAENVME_ReadCycle(BHandle, QDCAddress + Address, &Data, cvA24_U_DATA, cvD16) ;
                }
		*Value = Data;	
	}
	else{
		printf("\n\n Invalid Reading QDC Address\n") ;
                exit(3) ;
	}
	if(chk != cvSuccess){
                printf("\n\n Error reading QDC at address 0x%08X\n", QDCAddress) ;
                exit(3);
        }
}
int QDCConnection::QDCReadBLT(int Size, int *Data){
	int Total;
        unsigned char *Datai;
	if(Size <= 4096 and Size > 0){
		Datai = (unsigned char*)malloc(Size*sizeof(unsigned char)*4) ;
	        CAENVME_BLTReadCycle(BHandle, QDCAddress, Datai, 4*4096, cvA24_U_DATA, cvD32, &Total) ;
		for(int i=0;i<Size;i++){
			if(i<Total/4){
	               		Data[i] =(Datai[i*4+3]<<24)+(Datai[i*4+2]<<16)+(Datai[i*4+1]<<8)+Datai[i*4+0] ;
        		}
			else{
				Data[i] = -1;
			}
		}
        	return Total/4;
	}
	else{
                printf("\n\n Invalid BLT Reading Length\n") ;
                exit(4) ;
	}
}
// Methodes QDCTools---------------------------------------------------------------
void QDCDataInterpreter(int Size, int *Data, int *Valid, int Hits[][3]){
        int InHeader=0;
        int i, j;
        int Channel;
	int GateCounter=-1;
        //Cleaning Memory
        for(i=0;i<Size;i++){
              	Hits[i][0] = -1;
		Hits[i][1] = -1;
        }
        //Completing Result Array
        i=0;
	j=0; 
        while(i<Size){
                //Cheking for the information Header
		if(Data[i]==-1){
			*Valid=j;
			return;
		}
                if(InHeader==0 && (Data[i]&0x07000000)==0x02000000){
                        //Header read and expected
                        InHeader=1;
                        i++;
			GateCounter++;
                }
                else if(InHeader==0 && (Data[i]&0x07000000)!=0x02000000){
                        i=4096;
                        continue;
                }
                else if(InHeader==1 && (Data[i]&0x07000000)==0x02000000){
                        //Unexpected Header
                        exit(6) ;
                }
                else if(InHeader==1 && (Data[i]&0x07000000)==0x04000000){
                        //Section End
                        InHeader=0;
                        i++;
                }
                //When Im inside a valid information section
                if(InHeader==1){
                        if((Data[i]&0x07000000)!=0x0){
                                //Other than event detected
                                exit(6) ;
                        }
                        else{
                                //Obtaining the channel
                                Channel=(Data[i]&0x001f0000)>>16;
                                //Checking for overflow or underthreshold condition
                                if(Data[i]&0x3000){
                                        //For invalid data
                                        i++;
                                }
                                else{
                                        //without overflow or underthreshold
                                        Hits[j][0] = Channel;
					Hits[j][2] = GateCounter;
					Hits[j++][1] = Data[i]&0xFFF;
                                        i++;
                                }
                        }
                }
        }
	*Valid = j; 
}
