#ifndef __HVPOWERSUPPLY__
#define __HVPOWERSUPPLY__
#define length(x)	(sizeof(x)/sizeof(x[0]))
//#include <fstream>
//#include <string.h>

class	HVPowerSupply
{
private:
	char *ip;
	char *sys;
	char *usr;
	char *pass;
	bool newInitialized;
public:
	HVPowerSupply(); //Default constructor.
	
	 
	HVPowerSupply(	char *ip,
					char *sys,
					char *usr,
					char *pass);
	int connect();

	int setBias(	short slot,
					unsigned short len, 
					unsigned short *channel,
					float *value);

	int getBias(	short slot, 
					unsigned short len,
					unsigned short *channel,
					float *value);

	int setCurrent(	short slot,
					unsigned short len, 
					unsigned short *channel,
					float *value);
	
		
	int getCurrent(	short slot, 
					unsigned short len,
					unsigned short *channel,
					float *value);
	
	int setRampUp(	short slot,
					unsigned short len, 
					unsigned short *channel,
					float *value);
					
	int setRampDown(	short slot,
					unsigned short len, 
					unsigned short *channel,
					float *value);

	int pwOnBehavior(	short slot, 
						unsigned short len,
						unsigned short *channel,
						bool *value);
	
	int pwOffBehavior(	short slot, 
						unsigned short len,
						unsigned short *channel,
						bool *value);
		
	int pwOnChannel(	short slot, 
						unsigned short len,
						unsigned short *channel,
						bool *value);
					
	void LoadBiasVoltage(short slot, char *path);
	std::string GetStatus(short slot, unsigned short len, unsigned short *channel);
	char* GetError();

	~HVPowerSupply();
};
#endif
