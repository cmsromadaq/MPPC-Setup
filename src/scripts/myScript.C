#include <iostream>
#include <fstream>
#include <sstream>
#include <TH1I.h>
#include <TCanvas.h>

std::string int2str(int i){
	std::ostringstream conv;
	conv << i;
	return conv.str();
}

void myScript(std::string fname){
	std::ifstream file;
	file.open(fname.c_str());
	
	if(file.is_open()){
		Double_t X[16],Y[16],Xerr[16],Yerr[16];
		//TCanvas *c1 = new TCanvas("c1","Data",200,10,800,600);
		TCanvas **can = new TCanvas*[17];
		TH1I **hist = new TH1I*[16];
		std::string histoname,histotitle,fname,canname;
		for(int i=0;i<16;i++){
			histoname.assign("hist"+int2str(i));
			histotitle.assign("Channel "+int2str(i));
			hist[i] = new TH1I(histoname.c_str(),histotitle.c_str(),4096,0,4096);
		}
		
		int event,channel,data;
		char evnt[11],chn[11], dt[11];
		std::cout << "Reading file " << fname << "and generating histograms" << std::endl;
		while(file.good()){
			//file >> event >> channel >> data;
			file >> evnt >> chn >> dt;
			event = atoi(evnt);
			channel = atoi(chn);
			data = atoi(dt);
			//std::cout << "***** Event:" << event << " Channel:" << channel << " Data:" << data << std::endl;
			if(data == -1)
				continue;
			if(channel>31 && channel<48){
				//std::cout << "Event:" << event << " Channel:" << channel << " Data:" << data << std::endl;
				hist[channel-32]->Fill(data);
			}
		}
		file.close();
		for(int i=0;i<16;i++){
			fname.assign("data_"+int2str(i)+".png");
			canname.assign("Canvas"+int2str(i));
			X[i] = i;
			Y[i] = hist[i]->GetMean();
			Xerr[i] = 0.01;
			Yerr[i] = hist[i]->GetRMS();
			if(hist[i]->GetEntries() != 0){
				can[i] = new TCanvas(canname.c_str(),"Data",200,10,800,600);
				hist[i]->Draw();
			}
			//can[i]->Print(fname.c_str(),"png");
		}
		can[16] = new TCanvas("graphCan","Data",200,10,800,600);
		TGraphErrors *graph = new TGraphErrors(16,X,Y,Xerr,Yerr);
		graph->Draw("ALP");
		can[16]->Print("graph.png","png");
	}else{
		perror("Cannot open the file");
	}
}

/*int main(){
	myScript();
	return 0;
}*/
