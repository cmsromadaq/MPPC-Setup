#include <iostream>
#include <fstream>
#include <sstream>
#include <TH1I.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <string.h>
#include <TStyle.h>
#include <TList.h>

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

Double_t myFunc2(Double_t *x,Double_t *par){
	return GausFunc(x,par) + GausFunc(x,&par[3]);
}

Double_t myFunc3(Double_t *x,Double_t *par){
	return GausFunc(x,par) + GausFunc(x,&par[3]) + GausFunc(x,&par[6]);
}

Double_t myFunc4(Double_t *x,Double_t *par){
	return GausFunc(x,par) + GausFunc(x,&par[3]) + GausFunc(x,&par[6]) + GausFunc(x,&par[9]);
}

void check(char* fnamex,char* message){
	std::string fname;
	int i,j,map[64];
	std::ifstream mapFile;
	std::string fileName;
	
	fname.assign(fnamex);
	mapFile.open("/home/utfsm/alan/Documents/test/src/core/map.dat");
	if(mapFile.is_open()){
		while(mapFile.good()){
			mapFile >> i >> j;
			if(i>=0 && i<64)
				map[i] = j;
		}
		std::cout << "Channels already mapped" << std::endl;
		mapFile.close();
	}else{
		fprintf(stderr,"ERROR: There is not possible to map channel. Be careful");
		for(i=0;i<64;i++)
			map[i] = i;
	}
	
	TStyle *style = new TStyle("mrfs","MPPC Response Fitting Style");
	TCanvas **can = new TCanvas*[8];
	TH1I **hist = new TH1I*[128];
	int event, channel, data, index;
	char evnt[11], chn[11], dt[11];
	std::string histoname,histotitle,canname,cantitle;

	for(i=0;i<8;i++){
		canname.assign("Canvas"+int2str(i)+message);
		cantitle.assign("Data MPPC "+int2str(i)+message);
		can[i] = new TCanvas(canname.c_str(),cantitle.c_str(),200,10,3600,869);
		can[i]->Divide(8,2);
		for(j=0;j<16;j++){
			index = i*16 + j;
			histoname.assign("MPPC"+int2str(i)+"_Channel"+int2str(j));
			histotitle.assign("MPPC "+int2str(i)+" Channel "+int2str(j));
			hist[index] = new TH1I(histoname.c_str(), histotitle.c_str(),400,0,400);
		}
	}

	for(i=0;i<8;i++){
		fileName.assign(fname+int2str(i)+".dat");
		std::cout << "Openning the file: " << fileName << std::endl;
		std::ifstream file;
		file.open(fileName.c_str());
		if(file.is_open()){
			while(file.good()){
				file >> event >> channel >> data;
				if(channel>31 && channel<48){
					if(channel < 40)
						index = 16*(map[channel] - 8) + i;
					else
						index = 16*(map[channel] - 8) + 8 + i;
					hist[index]->Fill(data);
				}
			}
			file.close();
		}else{
			fprintf(stderr,"ERROR: opening the file: %s",fileName.c_str());
			exit(1);
		}
	}
	
	std::ofstream ofile;
	std::string imageName,minFuncName;

	imageName.assign("Images");
	imageName.append(fname.substr(fname.find_first_of('/')));
	imageName.assign(imageName.substr(0,imageName.length()-4));
	imageName.append("MPPC");

	ofile.open(std::string(fname+"-resume.dat").c_str());
	ofile << "MPPC\tChannel\tMean\tRMS\tconstant_0\tmean_0\tsigma_0\tconstant_1\tmean_1\tsigma_1\tconstant_2\tmean_2\tsigma_2" << std::endl;

	int binmax,k,gaussChk,gaussNum,separation=20,temp;
	Double_t max,minChk,chisqndf;
	TF1 **func = new TF1*[4];
	TF1 *iter;
	TList *list;
	style->cd();
	style->SetOptFit(111);
	for(i=0;i<8;i++){
		std::cout << "Fitting channels for MPPC: " << i << std::endl;
		for(j=0;j<16;j++){
			minChk = 1000.0;
			can[i]->cd(j+1);
			index = i*16+j;
			max = hist[index]->GetMaximum();
			binmax = hist[index]->GetMaximumBin();
			func[0] = new TF1("fiter1",GausFunc,0,400,3);
			func[1] = new TF1("fiter2",myFunc2,0,400,6);
			func[2] = new TF1("fiter3",myFunc3,0,400,9);
			func[3] = new TF1("fiter4",myFunc4,0,400,12);
			for(gaussNum=0;gaussNum<4;gaussNum++){
				for(k=0;k<(gaussNum+1)*3;k++){
					temp = binmax + separation*(k/3);
					if((k%3)==0){
						func[gaussNum]->SetParName(k,std::string("constant_"+int2str(k/3)).c_str());
						func[gaussNum]->SetParLimits(k,-0.1,30000);
						func[gaussNum]->SetParameter(k,hist[index]->GetBinContent(temp));
					}else if((k%3)==1){
						func[gaussNum]->SetParName(k,std::string("mean_"+int2str(k/3)).c_str());
						func[gaussNum]->SetParLimits(k,temp-15,temp+15);
						func[gaussNum]->SetParameter(k,temp);
					}else{
						func[gaussNum]->SetParName(k,std::string("sigma_"+int2str(k/3)).c_str());
						func[gaussNum]->SetParLimits(k,0,125);
						func[gaussNum]->SetParameter(k,5.0);
					}
					//std::cout << "Setting parameter " << func[gaussNum]->GetParName(k) << " Value:" << func[gaussNum]->GetParameter(k) << "\tLimits in:" << parmin << " " << parmax << std::endl;
				}
				/*if(gaussNum==3){
					func[index]->SetParLimits(9,0,30000);
					func[index]->SetParLimits(10,hist[index]->GetMean()-50,hist[index]->GetMean()+50);
					func[index]->SetParLimits(11,15,hist[index]->GetMean());
					func[index]->SetParameter(9,max);
					func[index]->SetParameter(10,hist[index]->GetMean());
					func[index]->SetParameter(11,hist[index]->GetRMS());
				}*/
				minFuncName.assign("fiter"+int2str(gaussNum+1));
				hist[index]->Fit(minFuncName.c_str(),"Q+","",0,400);
				chisqndf = func[gaussNum]->GetChisquare() / func[gaussNum]->GetNDF();
				if(chisqndf < minChk){
					minChk = chisqndf;
					gaussChk = gaussNum;
				}
			}
			/*std::cout << "MPPC:" << i << "\tChannel:" << j << "\tChi2/Ndf:" << func[gaussChk]->GetChisquare() / func[gaussChk]->GetNDF();
			for(k=0;k<4;k++)
				std::cout << "    \tChi2/Ndf" << k+1 << ":" << func[k]->GetChisquare() / func[k]->GetNDF();
			std::cout << std::endl;*/
			minFuncName.assign("fiter"+int2str(gaussChk+1));
			list = hist[index]->GetListOfFunctions();
			for(k=0;k<4;k++){
				iter = (TF1*) list->At(k);
				if(iter == 0)
					break;
				if(minFuncName.compare(iter->GetName())!=0){
					list->Remove(iter);
					k--;
				}
			}
			func[gaussChk]->SetLineColor(4);
			style->SetOptFit(111);
			hist[index]->Draw();
			func[gaussChk]->Draw("same");
			ofile << i << "\t" << j << "\t" << hist[gaussChk]->GetMean();
			ofile << "\t" << hist[index]->GetRMS()<< "\t";
			ofile << gaussChk;
			for(k=0;k<(gaussChk+1)*3;k++){
				ofile << "\t" << func[gaussChk]->GetParameter(k);
			}
			ofile << std::endl;
			delete func[0];
			delete func[1];
			delete func[2];
			delete func[3];
		}
		ofile.close();
		can[i]->Print(std::string(imageName+int2str(i)+"-fitted.ps").c_str(),"ps");
		can[i]->Print(std::string(imageName+int2str(i)+"-fitted.png").c_str(),"png");
	}
}

int main(int argc, char **argv){
	if(argc == 3){
		check(argv[1],argv[2]);
		return 0;
	}else{
		std::cout << "Usage:" << std::endl;
		std::cout << argv[0] << " FileMaskFormat Message" << std::endl;
		return 1;
	}
}
