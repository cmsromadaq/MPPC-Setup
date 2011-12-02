#include <iostream>
#include <TestAssembly.h>

int main(){
	TestAssembly *assembly = new TestAssembly("/dev/ttyUSB0");
	assembly->Debug(true);
	assembly->EnableVoltageMeasurements();
	assembly->Close();
	std::cout << "TestAssembly Controller: Voltage Enabled" << std::endl;
	return 0;
}
