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

void histo1D(std::string fname){
	std::ifstream file;
	file.open(fname.c_str());
	
	if(file.is_open()){
		Double_t X[16],Y[16],Xerr[16],Yerr[16];
		TCanvas **can = new TCanvas*[5];
		TH1I **hist = new TH1I*[16];
		std::string histoname,histotitle,fname,canname,cantitle;
		for(int i=0;i<16;i++){
			histoname.assign("hist"+int2str(i));
			histotitle.assign("Channel "+int2str(i+32));
			hist[i] = new TH1I(histoname.c_str(),histotitle.c_str(),500,0,500);
		}
		
		int event,channel,data;
		char evnt[11],chn[11], dt[11];
		std::cout << "Reading file " << fname << "and generating histograms" << std::endl;
		while(file.good()){
			file >> evnt >> chn >> dt;
			event = atoi(evnt);
			channel = atoi(chn);
			data = atoi(dt);
			if(data == -1)
				continue;
			if(channel>31 && channel<48){
				hist[channel-32]->Fill(data);
			}
		}
		file.close();
		std::cout << "Generating Histograms" << std::endl;
		for(int i=0;i<16;i++){
			X[i] = i;
			Y[i] = hist[i]->GetMean();
			Xerr[i] = 0.01;
			Yerr[i] = hist[i]->GetRMS();
			if(hist[i]->GetEntries() != 0){
				if(i%4==0){
					canname.assign("Canvas"+int2str(i/4));
					cantitle.assign("Data"+int2str(i/4));
					std::cout << "Working on canvas: " << i/4 << std::endl;
					can[i/4] = new TCanvas(canname.c_str(),cantitle.c_str(),200,10,800,600);
					can[i/4]->Divide(2,2);
					fname.assign("Images/data_"+int2str(i)+".png");
				}
				can[i/4]->cd(1+(i%4));
				//hist[i]->GetYaxis()->SetTitle("ADC Count");
				hist[i]->GetXaxis()->SetTitle("ADC Count");
				hist[i]->Draw();
				if(i%4==3)
					can[i/4]->Print(fname.c_str(),"png");
			}
		}
		std::cout << "Generating Resume" << std::endl;
		can[4] = new TCanvas("graphCan","Data",200,10,800,600);
		TGraphErrors *graph = new TGraphErrors(16,X,Y,Xerr,Yerr);
		graph->GetXaxis()->SetTitle("Channel number");
		graph->GetYaxis()->SetTitle("Mean value of each channel");
		graph->Draw("ALP");
		can[4]->Print("Images/graph.png","png");
	}else{
		perror("Cannot open the file");
	}
}

/*int main(){
	myScript();
	return 0;
}*/
