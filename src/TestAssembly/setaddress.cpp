#include <iostream>
#include <TestAssembly.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv){
	int channel;
	channel = atoi(argv[1]);
	if(channel<0 && channel>7){
		perror("ERROR: Channel must be between 0 and 7");
		return 1;
	}
	TestAssembly *assembly = new TestAssembly("/dev/ttyUSB0");
	assembly->Debug(true);
	assembly->SetAddress(channel);
	assembly->Close();
	std::cout << "TestAssembly Controller: Address setted to " << channel << std::endl;
	return 0;
}
