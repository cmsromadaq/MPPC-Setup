#include <iostream>
#include <fstream>
#include <sstream>
#include <PolyScienceChiller.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
int main(int argc, char **argv){
	float temp;
	int speed;
	PolyScienceChiller *chiller = new PolyScienceChiller("/dev/ttyUSB1");

	chiller->Debug(false);
	//std::cout << "Connected to: " << chiller->Identify() << std::endl;
	if(argc<2){
		std::cerr << "\e[0;31mERROR:\e[0m Numeric option must be inserted. Options are:" << std::endl;
		std::cerr << "0: Get setpoint" << std::endl;
		std::cerr << "1: Set setpoint" << std::endl;
		std::cerr << "2: Get internal temperature" << std::endl;
		std::cerr << "3: Get pump speed" << std::endl;
		std::cerr << "4: Set pump speed" << std::endl;
		std::cerr << "5: ON / OFF" << std::endl;
		exit(EXIT_FAILURE);
	}
	int option = atoi(argv[1]);
	switch(option){
	case 0: // Get setpoint
		chiller->GetSetpoint(&temp);
		std::cout << "Setpoint: " << temp << " Celcium degree" << std::endl;
		break;
	case 1: //Set setpoint
		if(argc<3){
			std::cerr << "\e[0;31mERROR:\e[0m Pump Speed must be placed between 0 and 100" << std::endl;
			exit(EXIT_FAILURE);
		}
		temp = atof(argv[2]);
		chiller->SetSetpoint(temp);
		std::cout << "Setpoint: " << temp << " Celcium degree" << std::endl;
		break;
	case 2: // Get Internal Temperature
		chiller->GetInternalTemperature(&temp);
		std::cout << "Internal Temperature: " << temp << " Celcium degree" << std::endl;
		break;
	case 3:// Get Pump Speed
		chiller->GetPumpSpeed(&speed);
		std::cout << "Pump Speed: " << speed << std::endl;
		break;
	case 4: // Set Pump Speed
		if(argc<3){
			std::cerr << "\e[0;31mERROR:\e[0m Pump Speed must be placed between 0 and 100" << std::endl;
			exit(EXIT_FAILURE);
		}
		speed = atoi(argv[2]);
		if(speed > 100 || speed < 0){
			std::cerr << "\e[0;31mERROR:\e[0m Pump Speed must be placed between 0 and 100" << std::endl;
			exit(EXIT_FAILURE);
		}
		chiller->SetPumpSpeed(speed);
		std::cout << "Pump Speed seeted to: " << speed << std::endl;
		break;
	case 5: // ON OFF
		if(argc<3){
			std::cerr << "\e[0;31mERROR:\e[0m Pump Speed must be placed between 0 and 100" << std::endl;
			exit(EXIT_FAILURE);
		}
		if(strcmp(argv[2],"ON")==0){
			chiller->SetOnOff(1);
			std::cout << "Polyscience Chiller: ON" << std::endl;
		}else if(strcmp(argv[2],"OFF")==0){
			chiller->SetOnOff(0);
			std::cout << "Polyscience Chiller: OFF" << std::endl;
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m Option not recognized: " << argv[2] << std::endl;
		}
		break;
	default:
		std::cerr << "\e[0;31mERROR:\e[0m Option not recognized: " << argv[1] << std::endl;
	}
	return EXIT_SUCCESS;
}
