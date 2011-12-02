#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <string.h>
#include <TStyle.h>
#include <TROOT.h>

std::string int2str(int i){
	std::ostringstream conv;
	conv << i;
	return conv.str();
}

int plotCurrents(char *ifilename){
	int i,slot, channel, count;
	Double_t vol, curr, time;
	std::ifstream ifile(ifilename);
	TCanvas *can = new TCanvas("can","Canvas",200,10,800,600);
	TH1F *hist[36];

	if(!ifile.is_open()){
		std::cerr << "\e[0;31mERROR:\e[0m Cannot open file " << ifilename << std::endl;
		return EXIT_FAILURE;
	}
	for(i=0;i<36;i++){
		hist[i] = new TH1F(
				std::string("hist"+int2str(i)).c_str(),
				std::string("Slot"+int2str(2*(i/12))+" Channel"+int2str(i%12)).c_str(),
				20,0,0.2);
	}
	while(ifile.good()){
		ifile >> slot >> channel >> count >> vol >> curr >> time;
		if(vol>5.0){
			hist[slot*6+channel]->Fill(curr);
		}
	}
	ifile.close();
	for(i=0;i<36;i++){
		if(hist[i]->GetEntries()!=0){
			hist[i]->Draw();
			can->Print(std::string("icurr_Slot"+int2str(2*(i/12))+"_Channel"+int2str(i%12)+".png").c_str());
			std::cout << std::string("icurr_Slot"+int2str(2*(i/12))+"_Channel"+int2str(i%12)+".png").c_str() << std::endl;
		}
	}
	return EXIT_SUCCESS;
}

int main(int argc, char **argv){
	if(argc<2){
		std::cerr << "\e[0;31mERROR:\e[0m input file name is needed" << std::endl;
		return EXIT_FAILURE;
	}
	return plotCurrents(argv[1]);
}
