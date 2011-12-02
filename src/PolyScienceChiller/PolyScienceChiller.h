
#ifndef __POLYSCIENCE_CHILLER_9102A__
#define __POLYSCIENCE_CHILLER_9102A__

#include <string>
#include <termios.h>


class PolyScienceChiller {
	private:
		int handler;
		bool connected;
		bool debug;
		int baud;
		char endChar;
		struct termios toptions;
		int Write(std::string data);
		int Read(std::string *data);

	public:
		PolyScienceChiller();
		PolyScienceChiller(const char* device);
		~PolyScienceChiller();
		
		int Connect();
		int Connect(const char* device);
		int Close();
		void Reset();
		void GetError(std::string *data);
		bool IsOpen();
		void Debug(bool deb);
		std::string Identify();
		int GetPort();
	
		void SetSetpoint(float temperature);
		void SetOnOff(bool onoff);
		void SetPumpSpeed(int speed);
		void GetSetpoint(float *temperature);
		void GetInternalTemperature(float *temperature);
		void GetPumpSpeed(int *speed);
		
};

#endif
