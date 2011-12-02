#ifndef __PROGRAM_PARAMETERS__
#define __PROGRAM_PARAMETERS__

//#include <iostream>
//#include <fstream>
//#include <sstream>
#include <string.h>


class ProgramParameters {
private:
	int debug;
	unsigned long int mask;
#ifdef RUNNINGINM32MODE
	unsigned long int mask2;
#endif
	int	samples;
	int	*qdcMap;
	char*	bvpsMapFile;
	char*	file_name_format;
	char*	controllerDevice;
	char*	chillerDevice;
	bool	verbose;

public:
		ProgramParameters(int argc,char** argv);
		~ProgramParameters();
	void	Presentation();
	int	checkImportantParameters();
	void	usage(char* argv);
	void	Help(char* argv);
	bool	Debug(int level);
	bool	Mask(int i);
	int	GetSamples();
	void	SaveData(int *Data, int *Hits, unsigned long int *GateCounter, int board, std::ofstream & file);
	char*	GetFName();
	char*	GetBVPSmapPath();
	char*	GetControllerDevice();
	char*	GetChillerDevice();
	void	LoadQDCMap();
	void	UnloadQDCMap();
	int	QDCMap(int channel);
	bool	Verbose();
};

#endif

