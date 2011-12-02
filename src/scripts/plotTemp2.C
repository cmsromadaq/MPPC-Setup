#include <TNtuple.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <TFile.h>
#include <TCanvas.h>
#include <math.h>

void plotTemp2(char *fname){
	TCanvas *can = new TCanvas("tempplot","Temperature Plot",200,10,800,600);
	TFile *rFile = new TFile("resultado2.5.root","RECREATE");
	TNtuple *sensors=new TNtuple("Sens","Sens","sensor:mean:var:time");
	ifstream iFile;
	Double_t mean,var,time;
	Int_t sensor;
	int line=0;
	char aux[100];
	iFile.open(fname);
	if(!iFile.good()){
		std::cerr << "File not loaded" << std::endl;
		rFile->Close();
		return;
	}
	iFile.getline(aux,100);
	while(iFile.good()){
		iFile >> sensor >> mean >> var >> time;
		if (mean < 0)
			continue;
		line++;
		//std::cout<<line<<std::endl;
		//istd::cout<<sensor<<"\t"<<mean<<"\t"<<var<<"\t"<<time<<std::endl;
		mean= 1.0/( (1.0/298.0) + (1.0/3970.0)*log(mean/30.0) ) - 273.0;
		var= 1.0/( (1.0/298.0) + (1.0/3970.0)*log(var/30.0)) - 273.0;
		sensors->Fill(sensor,mean,var,time);
		//std::cout<<sensor<<"\t"<<mean<<"\t"<<var<<"\t"<<time<<std::endl;
	}	
	iFile.close();
	rFile->Write();
	rFile->Close();
	std::cout<<"done!"<<std::endl;
}

int main(int argc, char **argv){
	if(argc < 2){
		std::cerr << "\e[0;31mERROR:\e[0m Data file name must be spacified"
		return EXIT_FAILURE;
	}
	plotTemp2(argv[1]);
	return EXIT_SUCCESS;
}
