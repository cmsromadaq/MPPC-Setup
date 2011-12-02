#include <iostream>
#include <TestAssembly.h>

int main(){
	std::cout << "Starting..." << std::endl;
	TestAssembly *assembly = new TestAssembly("/dev/ttyUSB0");

	int channel;
	assembly->Reset();
	std::cout << "Connected to: " << assembly->Identify() << std::endl;
	for(channel=0;channel<2;channel++){
		std::cout << "Working in Channel " << channel << std::endl;
		assembly->SetAddress(channel);
		assembly->EnableVoltageMeasurements();
		assembly->DisableVoltageMeasurements();
	}
	assembly->Close();
	return 0;
}
