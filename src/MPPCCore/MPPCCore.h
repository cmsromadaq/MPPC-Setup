#ifndef __MAIN_MPPC_CHARACTERIZATION_CORE__
#define __MAIN_MPPC_CHARACTERIZATION_CORE__

#include <unistd.h>
#include <TNtuple.h>
#include <CAENVMEenvironment.h>
#include <TestAssembly.h>
#include <ProgramParameters.h>
#ifdef RUNNINGINM32MODE
#include <HVPowerSupply.h>
#endif
#include <PolyScienceChiller.h>
class CORE : public ProgramParameters {
	private:
		//static ProgramParameters *setup;
		QDCConnection *QDC1;
		QDCConnection *QDC2;
		TestAssembly *assembly;
		PolyScienceChiller *chiller;
#ifdef RUNNINGINM32MODE
		//HVPowerSupply *bvps;
#endif
		int addrIndex;
		int mppcIndex;
		int *temp;
		int tempIndex;
		static int instancesCount;

	public:
		CORE(int argc, char *argv[]);
		~CORE();

		void SetDebugLevels(int level);
		int Initialize();
		int SaveDataFromQDC();
		void SetAddrIndex(int index);
		int GetAddrIndex();
		void SetMppcIndex(int index);
		int GetMppcIndex();
		void GetDataFileName(char* filename);
		void EnableVoltageMeasurements();
		void DisableVoltageMeasurements();
		int GetSamples();
		void LoadTemp();
		void SetTemp(int temp);
		int GetTempIndex();
		float GetTemp();
#ifdef RUNNINGINM32MODE
		void PowerOnHV(bool state);
#endif
		void InterpreteAndSaveData(int *Data, int *Hits, unsigned long int *GateCounter, int board , std::ofstream & file,TNtuple *tupla);
};

#endif
