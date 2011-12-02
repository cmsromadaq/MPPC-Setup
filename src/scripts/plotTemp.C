
{
#include "TNtuple.h"
#include <fstream>
#include "TFile.h"
#include <math.h>


	TFile *rFile = new TFile("resultado2.5.root","RECREATE");
	TNtuple *sensors=new TNtuple("Sens","Sens","sensor:mean:var:time");
	ifstream iFile;
	Double_t mean,var,time;
	Int_t sensor;
	int line=0;
	char aux[100];
	iFile.open("./run002_temp.dat");
	if(!iFile.good()){
		std::cerr << "File not loaded" << std::endl;
	}else{	
		iFile.getline(aux,100);
		while(iFile.good()){
			iFile>>sensor>>mean>>var>>time;
			
			if (mean<0)
				continue;
			line++;
//			std::cout<<line<<std::endl;
			//istd::cout<<sensor<<"\t"<<mean<<"\t"<<var<<"\t"<<time<<std::endl;
			mean= 1.0/( (1.0/298.0) + (1.0/3970.0)*log(mean/30.0) ) - 273.0;
			var= 1.0/( (1.0/298.0) + (1.0/3970.0)*log(var/30.0)) - 273.0;
			sensors->Fill(sensor,mean,var,time);
			//std::cout<<sensor<<"\t"<<mean<<"\t"<<var<<"\t"<<time<<std::endl;
		}	
		iFile.close();
		rFile->Write();
	}	
	rFile->Close();
	std::cout<<"done!"<<std::endl;
}
