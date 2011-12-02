//---------------------------------------------------------------------------------
// QDCDataAcquisitionv1.0
// Developer:   Pedro F. Toledo 
//              <pedrotoledocorrea@gmail.com>
// Description: Acquisition system for dual QDC CAEN v792 through a VMEUSB CAEN v17
//              18 and data storage on a bidimentional CERN Root Histogram
//Libraries------------------------------------------------------------------------
#include "CAENVMEenvironment.h"         //For this setup communication
//ROOT Libraries-------------------------------------------------------------------
#include <TFile.h>			//For root file saving	
#include <TH2.h>			//Fot root bidimentional Histograms
//System defines-------------------------------------------------------------------
//Local Variables------------------------------------------------------------------
int             	Data1[4096];           	//For QDC1 Blt Reads
int			Data2[4096];		//For QDC2 Blt Reads
int			Hits1[4096][2];		//Fot QDC1 valid hits
int			Hits2[4096][2];		//For QDC2 valid hits
int			i1;			//For QDC1 valid hits count
int			i2;			//For QDC2 valid hits count
int   	          	Hits[64];		//Counted Hits for Each Channel
int			MinHits;		//Minimum counted hits
char			HDest;			//Root File Histogram Destination
//User set variables---------------------------------------------------------------
int			TMask;			//For Scanf Mask
unsigned long int	Mask;               	//Channel acquisition mask
int             	Samples;            	//Total points to be acquired per channel
//main-----------------------------------------------------------------------------
int main(int argc, char *argv[]){
	//ROOT Variable for histogram
	TH2F *Channels = new TH2F("Channels","Dual QDC Acquisition",64,0,64,4096,0,4096) ;
	//Checking args, on error display help
	printf("------------------------------------------------------\n                QDCDataAcquisitionv1.0\n\nContact: Pedro F. Toledo <pedrotoledocorrea@gmail.com>\n------------------------------------------------------\n\n") ;
	if(argc<5){
		printf("Format is: QDCDataAcquisitionv1.0 <Mask Ch:63-32> <Mas\nk Ch:31-0> <Samples> <TH2F Filename>\n") ;
		printf("Example: QDCDataAcquisitionv1.0 FFFF0000 F0000000 1000\nexample.root\n") ;
		printf("This will acquire 1000 samples from channels 28-31 and\n48-63, store them on a TH2F with x the channel  number\nand save them on example.root file.\n") ;
		exit(5) ;
	}
	//Clearing arrays
	for(int i=0;i<64;i++){
		Hits[i]=0;
	}
	//Connecting the USB Link
	CAENVMEUSBLink Link;
	//Coneccting to QDCs
	QDCConnection QDC1(Link, 0x00110000) ;
	QDCConnection QDC2(Link, 0x00220000) ;
	//reading acquisition mask
	sscanf(argv[1],"%X",&TMask) ;
	Mask=((unsigned long int)TMask)<<32 ;
	sscanf(argv[2],"%X",&TMask) ;
	Mask=Mask+TMask;
	//reading samples
	sscanf(argv[3],"%d",&Samples) ;
	if(Samples<1){
		printf("\n\n Invalid amount of samples\n") ;
		exit(5) ;
	}
	TFile f(argv[4],"recreate") ;
	//Setting thresholds to 0
	for(int i=0;i<32;i++){
		if(Mask&(((unsigned long int)0x1)<<(i+ 0))){
			QDC1.QDCWrite(0x1080+i*2,0x0) ;
		}
		else{
			QDC1.QDCWrite(0x1080+i*2,0x1F0) ;
		}
		if(Mask&(((unsigned long int)0x1)<<(i+32))){
			QDC2.QDCWrite(0x1080+i*2,0x0) ;
		}
		else{
			QDC2.QDCWrite(0x1080+i*2,0x1F0) ;
		}
        }
	//Qdc reset
	QDC1.QDCWrite(0x1016, 1) ;
	QDC2.QDCWrite(0x1016, 1) ;
	//Starting the acquisition iterations
	printf("Acquiting Mask: %08X%08X\n",(unsigned int)((Mask&0xFFFFFFFF00000000)>>32),(unsigned int)Mask&0xFFFFFFFF) ;
	printf("Total Samples:  %d\n",Samples) ;
	printf("Progress:         0%%") ;
	while(Samples > MinHits){
		//reading the events stored on the QDC
		QDC1.QDCReadBLT(4096, Data1) ;
		QDC2.QDCReadBLT(4096, Data2) ;
		//Iterpreting the data obtained from the QDC
		QDCDataInterpreter(4096, Data1, &i1, Hits1) ;
		QDCDataInterpreter(4096, Data2, &i2, Hits2) ;
		//Filling Histogram
		for(int i=0;i<i1;i++){
			if(Mask&(((unsigned long int)0x1)<<(Hits1[i][0]+ 0))){
				if(Hits[Hits1[i][0]   ]<Samples){
					Channels->Fill(Hits1[i][0],Hits1[i][1]) ;
					Hits[Hits1[i][0]   ]++;
				}
			}
		}
		for(int i=0;i<i2;i++){
			if(Mask&(((unsigned long int)0x1)<<(Hits2[i][0]+32))){
				if(Hits[Hits2[i][0]+32]<Samples){
		                        Channels->Fill(Hits2[i][0]+32,Hits2[i][1]) ;
					Hits[Hits2[i][0]+32]++;
				}
			}
                }
		//Measuring the most unfilled histogram
		MinHits=-1;
		for(int i=1;i<64;i++){
			if(Mask&(((unsigned long int)0x1)<<i)){
				if(MinHits<0){
					MinHits=Hits[i];
				}
				if(MinHits>Hits[i]){
					MinHits=Hits[i];
				}
			}
		}
		//Printing progress
		printf("\b\b\b\b%3d%%",100*MinHits/Samples) ;
	}
	//Clossing seccuence
	printf("\n") ;
	fflush(stdout) ;
	Channels->Write();
	Link.Close() ;             //terminates the USB Link */
}

