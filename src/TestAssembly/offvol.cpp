#include <iostream>
#include <TestAssembly.h>
#include <stdio.h>

int main(){
	TestAssembly *assembly = new TestAssembly("/dev/ttyUSB0");
	assembly->Debug(true);
	assembly->DisableVoltageMeasurements();
	assembly->Close();
	std::cout << "TestAssembly Controller: Voltage Disabled" << std::endl;
	return 0;
}
