#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "CAENVMEenvironment.h"

int main(){
	QDCConnection *qdc1 = new QDCConnection(0x00110000);
	QDCConnection *qdc2 = new QDCConnection(0x00220000);
	//std::cout << "Value of Bit Set 2 Register is: 0x" << std::hex << qdc->GetBitSet2Register() << std::endl;
	//qdc->SetEventCounterMode(0);
	//std::cout << "Value of Bit Set 2 Register is: 0x" << std::hex << qdc->GetBitSet2Register() << std::endl;
	//std::cout << "Step Threshold 1: " << qdc1->StepThreshold() << std::endl;
	//std::cout << "Step Threshold 2: " << qdc2->StepThreshold() << std::endl;
	//qdc2->SetIpedRegister(180);
	//std::cout << "Pedestal Current 1: " << qdc1->SetIpedRegister(170) << std::endl;
	//std::cout << "Pedestal Current 2: " << qdc2->IpedRegister() << std::endl;
	for(int i=0;i<32;i++){
		qdc1->EnableChannel(i);
		qdc2->EnableChannel(i);
		std::cout << "Threshold for qdc 1 channel " << i << " is: " << qdc1->ReadThresholdValue(i) << std::endl;
		std::cout << "Threshold for qdc 2 channel " << i << " is: " << qdc2->ReadThresholdValue(i) << std::endl;
	}
	return 0;
}


