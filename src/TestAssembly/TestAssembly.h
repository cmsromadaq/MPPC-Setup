
#ifndef __TEST_ASSEMBLY_CONTROLLER__
#define __TEST_ASSEMBLY_CONTROLLER__

#include <string>
#include <termios.h>


class TestAssembly {
	public:
		TestAssembly();
		TestAssembly(const char* device);
		~TestAssembly();
		
		int Connect();
		int Connect(const char* device);
		int Close();
		void SetAddress(int addr);
		void EnableVoltageMeasurements();
		void DisableVoltageMeasurements();
		void Reset();
		void GetError(std::string *data);
		bool IsOpen();
		void Debug(bool deb);
		std::string Identify();
		int GetPort();
	private:
		int arduino;
		bool connected;
		bool debug;
		int baud;
		char endChar;
		struct termios toptions;
		int Write(std::string data);
		int Read(std::string *data);
};

#endif
