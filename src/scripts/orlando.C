#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

std::string int2str(int i){
	std::ostringstream conv;
	conv << i;
	return conv.str();
}

int OrlandosFunc(char *iname){
	int i,slot,channel;
	Double_t run, vol, curr, time;
	TCanvas *can = new TCanvas("can","Canvas",200,10,600,800);
	std::ifstream file(Form("%s.dat",iname));
	char temp[256];

	TH1F *hist[36];
	for(i=0;i<36;i++){
		hist[i] = new TH1F(
			std::string("hist"+int2str(i)).c_str(),
			std::string("Slot"+int2str(2*(i/12))+" Channel"+int2str(i%12)).c_str(),
			20,0,0.2);
	}
	file.getline(temp,256);
	while(file.good()){
		file >> slot >> channel >> run >> vol >> curr >> time;
		hist[(slot*6+channel)]->Fill(curr);
	}
	file.close();
	std::string ofilename(iname);
	can->Print(std::string(ofilename+".ps[").c_str());
	//can->Print(Form("%s.ps[",iname));
	for(i=0;i<36;i++){
		hist[i]->Draw();
		//can->Print(Form("%s.ps",iname));
		can->Print(std::string(ofilename+".ps").c_str());
		std::cout << Form("%s.ps",iname) << std::endl;
	}
	//can->Print(Form("%s.ps]",iname));
	can->Print(std::string(ofilename+".ps]").c_str());
	return 0;
}
