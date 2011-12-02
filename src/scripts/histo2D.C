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

void histo2D(std::string fname){
	std::ifstream file;
	file.open(fname.c_str());
	
	if(file.is_open()){
		Double_t X[16],Y[16],Xerr[16],Yerr[16];
		//TCanvas *c1 = new TCanvas("c1","Data",200,10,800,600);
		TCanvas **can = new TCanvas*[17];
		TH2I **hist = new TH2I*[16];
		std::string histoname,histotitle,fname,canname;
		for(int i=0;i<16;i++){
			histoname.assign("hist"+int2str(i));
			histotitle.assign("Channel "+int2str(i));
			hist[i] = new TH2I(histoname.c_str(),histotitle.c_str(),600,0,600,600,0,600);
		}
		
		int event,channel,data;
		int tempEvnt, tempCh,tempDat;
		char evnt[11],chn[11], dt[11];
		std::cout << "All declared" << std::endl;
		tempEvnt = -1;
		tempCh = -1;
		tempDat = -1;
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
				//hist[channel-32]->Fill(data);
				if(channel==32){
					tempEvnt = event;
					tempCh   = channel;
					tempDat  = data;
				}else if(channel!=32){
					if(tempEvnt == event){
						hist[channel-32]->Fill(data,tempDat);
					}
				}
				
			}
		}
		file.close();
		for(int i=1;i<16;i++){
			fname.assign("data_"+int2str(i)+".png");
			canname.assign("Canvas"+int2str(i));
			X[i] = i;
			Y[i] = hist[i]->GetMean();
			Xerr[i] = 0.01;
			Yerr[i] = hist[i]->GetRMS();
			if(hist[i]->GetEntries() != 0){
				hist[i]->SetTitle(std::string("Channel 0 and "+int2str(i)).c_str());
				hist[i]->GetXaxis()->SetTitle(std::string("ADC Count for Channel "+int2str(i)).c_str());
				hist[i]->GetYaxis()->SetTitle("ADC Count for Channel 0");
				can[i] = new TCanvas(canname.c_str(),"Data",200,10,1024,768);
				hist[i]->Draw();
			}
			//can[i]->Print(fname.c_str(),"png");
		}
/*		can[16] = new TCanvas("graphCan","Data",200,10,800,600);
		TGraphErrors *graph = new TGraphErrors(16,X,Y,Xerr,Yerr);
		graph->Draw("ALP");
		can[16]->Print("graph.png","png");*/
	}else{
		perror("Cannot open the file");
	}
}

/*int main(){
	histo2D("data.dat");
	return 0;
}*/
