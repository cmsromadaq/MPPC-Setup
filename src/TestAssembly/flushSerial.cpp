#include <iostream>
#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <TestAssembly.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char** argv){
	if(argc < 2){
		std::cerr << "\e[0;31mERROR:\e[0m more argments are need it" << std::endl;
		std::cout << "Example: " << argv[0] << " /dev/ttyUSB0" << std::endl;
		return 1;
	}
	int arduino = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
	if(arduino==-1){
		std::cerr << "\e[0;31mERROR:\e[0m Could not establish connection with Test Assembly in: " << argv[1] << std::endl;
		return 1;
	}
	if(tcflush(arduino,TCIOFLUSH)){
		std::cerr << "\e[0;31mERROR:\e[0m Impossible to flush the port" << std::endl;
		return 1;
	}
	if(close(arduino)){
		std::cerr << "\e[0;31mERROR:\e[0m Impossible to flush the port" << std::endl;
		return 1;
	}
	std::cout << "\e[0;32mSerial port " << argv[1] << " flushed" << std::endl;
	return 0;
}

