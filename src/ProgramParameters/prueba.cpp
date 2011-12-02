#include <fstream>
#include <iostream>
#include "ProgramParameters.h"

int main(int argc,char** argv){
	//char* file;
	int j;
	ProgramParameters *setup = new ProgramParameters(argc,argv);
	std::cout << "Data " << setup->GetFName() << std::endl;
	/*for(j=0;j<64;j++){
		if(setup->Mask(j)){
			std::cout << j << " true" << std::endl;
		}else{
			std::cout << j << " false *********************"<< std::endl;
		}
	}
	std::cout << "MAPFILE: " << setup->GetBVPSmapPath() << std::endl;*/
	/*std::cout << setup->getFreqIni() << std::endl;
        std::cout << setup->getFrequencyFin() << std::endl;*/
	/*for(j=1;j<10;j++){
		setup->setIReference((double) j/1000);
		//std::cout << setup->getIReference() << std::endl;
		saveGraphics("prueba",setup);
		std::cout << SetOutputFileName("prueba","root",setup) << std::endl;
	}*/
        /*std::cout << setup->getPMFHistogramFlag() << std::endl;
        std::cout << setup->getSusceptCurveFlag() << std::endl;
        std::cout << setup->getPointsNumber() << std::endl;
        std::cout << setup->getPMFNumber() << std::endl;
        std::cout << setup->getMeasuredChannel() << std::endl;
        std::cout << setup->getInyectedChannel() << std::endl;
        std::cout << setup->getTriggerChannel() << std::endl;
	setup->getTxtFilePath(file);
	if(file!=0){
		std::cout << file << std::endl;
		std::cout << "Name: " << file << std::endl;
	}*/
	return 0;
}
