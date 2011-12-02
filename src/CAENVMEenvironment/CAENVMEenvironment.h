// QDCDataAqcisitionv1.0
//
// Developer: 	Pedro F. Toledo
//		<pedrotoledocorrea@gmail.com>
//
// CAENVMEenvironment.h
//
// Description:	This file contains the class definitions required for the QDCDataAc
//		quisition program, been designed for the version 1.0
//
// Libraries-----------------------------------------------------------------------
//#include <stdarg.h>                     //From CAENVMEDemoi

#ifndef __CAEN_QDC_Connection__
#define __CAEN_QDC_Connection__

#include "CAENVMElib.h"                 //v1718 Lib

class QDCConnection{
	private:
		int 		QDCAddress ;
		short		Link;
		bool		debug;
		static int32_t	BHandle;
		static	unsigned int num;
	public:
				QDCConnection(int QDCAddressToConnect);
				~QDCConnection();
		void 		QDCWrite(int Address, int Value);
		void		QDCRead(int Address, int *Value);
		int		QDCReadBLT(int Size, int *Data);
		void		EnableChannel(int i);
		void		DisableChannel(int i);
		void		Reset();
		int		GetBitSet2Register();
		void		SetEventCounterMode(int mode);
		bool		Debug();
		void		Debug(bool debug);
		int		IpedRegister();
		int		StepThreshold();
		void		SetIpedRegister(int Value);
		int		ReadThresholdValue(int i);
};

#endif
