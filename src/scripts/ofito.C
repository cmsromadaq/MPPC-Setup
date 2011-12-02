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

Double_t GausFunc(Double_t *x,Double_t *par){
	Double_t cuad = 0;
	if(par[2] != 0)
		cuad = (x[0]-par[1])/par[2];
	return par[0]*TMath::Exp(-0.5*cuad*cuad);
}
Double_t myFunc(Double_t *x,Double_t *par){
	return GausFunc(x,par) + GausFunc(x,&par[3]) + GausFunc(x,&par[6]);
}
Double_t myFunc2(Double_t *x,Double_t *par){
	return GausFunc(x,par) + GausFunc(x,&par[3]) + GausFunc(x,&par[6]) + GausFunc(x,&par[9]);
}

void ofito(char* filepath,int chan,bool quad){
	std::ifstream file;
	std::string histotitle("channel ");
	file.open(filepath);
	//file.open("Data/20110914/run000-woled-addr5.dat");
	//file.open("Data/20110819/run001-woled-addr5.dat");
	//file.open("Data/20110902/run002-woled-addr5.dat");
	
	if(file.is_open()){
		TCanvas *can = new TCanvas(filepath,filepath,200,10,800,800);
		histotitle.append(int2str(chan));
		TH1I *hist = new TH1I("hist0",histotitle.c_str(),250,0,250);
		std::string fname,canname,cantitle;
		
		int event,channel,data;
		
		while(file.good()){
			file >> event >> channel >> data;
			if(data == -1)
				continue;
			if(channel==chan)
				hist->Fill(data);
		}
		file.close();
		hist->Draw();
		int min = 0;
		int max = 200;
		float grab = 0;
		float igrab = 0;

		grab = hist->GetMaximum();
		igrab = hist->GetMaximumBin();
		TF1 *func;
		if(quad)
			func = new TF1("fiter",myFunc2,min,max,12);
		else
			func = new TF1("fiter",myFunc,min,max,9);
		

		func->SetParLimits(0,0,30000);
		func->SetParLimits(2,0,15);
		func->SetParLimits(3,0,30000);
		func->SetParLimits(5,0,15);
		func->SetParLimits(6,0,30000);
		func->SetParLimits(8,0,10);
		if(quad)
			func->SetParLimits(10,0,30000);
		func->SetParameter(0,grab);
		func->SetParameter(1,igrab);
		func->SetParameter(2,   5.0);
		func->SetParameter(3,hist->GetBinContent(igrab+25));
		func->SetParameter(4, igrab+25.0);
		func->SetParameter(5,   5.0);
		func->SetParameter(6,hist->GetBinContent(igrab+50));
		func->SetParameter(7, igrab+50.0);
		func->SetParameter(8,   5.0);
		if(quad){
			func->SetParameter(9,hist->GetBinContent(igrab+50));
			func->SetParameter(10,hist->GetMean());
			func->SetParameter(11,hist->GetRMS());
		}
		func->SetParName(0,"constant_0");
		func->SetParName(1,"mean_0");
		func->SetParName(2,"sigma_0");
		func->SetParName(3,"constant_1");
		func->SetParName(4,"mean_1");
		func->SetParName(5,"sigma_1");
		func->SetParName(6,"constant_2");
		func->SetParName(7,"mean_2");
		func->SetParName(8,"sigma_2");
		if(quad){
			func->SetParName(9,"constant_3");
			func->SetParName(10,"mean_3");
			func->SetParName(11,"sigma_3");
		}
		hist->Fit("fiter","V+","",min,max);
		func->SetLineColor(4);
		gStyle->SetOptFit(111);
		func->Draw("same");
		TF1 *gaussf1 = new TF1("gaus1",GausFunc,0,200,3);
		TF1 *gaussf2 = new TF1("gaus2",GausFunc,0,200,3);
		TF1 *gaussf3 = new TF1("gaus3",GausFunc,0,200,3);
		if(quad)
			TF1 *gaussf4 = new TF1("gaus4",GausFunc,0,200,3);
		gaussf1->SetParameter(0,func->GetParameter(0));
		gaussf1->SetParameter(1,func->GetParameter(1));
		gaussf1->SetParameter(2,func->GetParameter(2));
		gaussf2->SetParameter(0,func->GetParameter(3));
		gaussf2->SetParameter(1,func->GetParameter(4));
		gaussf2->SetParameter(2,func->GetParameter(5));
		gaussf3->SetParameter(0,func->GetParameter(6));
		gaussf3->SetParameter(1,func->GetParameter(7));
		gaussf3->SetParameter(2,func->GetParameter(8));
		gaussf1->SetLineColor(2);
		gaussf2->SetLineColor(2);
		gaussf3->SetLineColor(2);
		gaussf1->Draw("same");
		gaussf2->Draw("same");
		gaussf3->Draw("same");
		if(quad){
			gaussf4->SetParameter(0,func->GetParameter(9));
			gaussf4->SetParameter(1,func->GetParameter(10));
			gaussf4->SetParameter(2,func->GetParameter(11));
			gaussf4->SetLineColor(3);
			gaussf4->Draw("same");
		}
		//Double_t limit1 = (func->GetParameter(1)+func->GetParameter(4))/2;
		//Double_t limit2 = (func->GetParameter(4)+func->GetParameter(7))/2;
		std::cout << "Integral of pedestal: " << gaussf1->Integral(0,250) << std::endl;
		std::cout << "Integral of 1 pe    :  " << gaussf2->Integral(0,250) << std::endl;
		std::cout << "Integral of 2 pe    :   " << gaussf3->Integral(0,250.0) << std::endl;
		if(quad)
			std::cout << "Integral of noise   :   " << gaussf4->Integral(0,250.0) << std::endl;
	}else{
		perror("Cannot open the file");
	}
}

