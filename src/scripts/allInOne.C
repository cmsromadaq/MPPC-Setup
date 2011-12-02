#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TMath.h>
#include <string.h>
#include <TStyle.h>

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

bool checkFolder(std::string fname){
	DIR *dp;
	dirent *d;
	char *dirimghome = getenv("HOME");
	char *imgHome = new char[strlen(dirimghome)+24];
	strcpy(imgHome,dirimghome);
	strcat(imgHome,"/Documents/test/Images");
	std::string folder = fname.substr(5,8);
	if(dp = opendir(imgHome)){
		while(d = readdir(dp)){
			if(!strcmp(d->d_name,".") || !strcmp(d->d_name,".."))
				continue;
			if(folder.compare(d->d_name)==0){
				closedir(dp);
				return false;
			}
		}
		folder.assign(imgHome);
		folder.append("/"+fname.substr(5,8));
		std::cout << "Creating folder: " << folder << std::endl;
		if(mkdir(folder.c_str(),S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)){
			perror("ERROR: there is not possible to continue due to problems in folder structure");
			return true;
		}
		closedir(dp);
	}else{
		perror("ERROR: there is not possible to continue due to problems in folder structure");
		return true;
	}
	return false;
}

void allInOne(char* fnamex,char* message,int gaussNum){
	std::string fname;
	int i,j,map[64];
	std::ifstream mapFile;
	std::string fileName;
	char *home;

	fname.assign(fnamex);
	//home = getenv("HOME");
	strcpy(home,getenv("HOME"));
	strcat(home,"/Documents/test/src/core/map.dat");
	std::cout << "Opening mapping file in: " << home << std::endl;
	mapFile.open(home);
	if(mapFile.is_open()){
		while(mapFile.good()){
			mapFile >> i >> j;
			if(i>=0 && i<64)
				map[i] = j;
		}
		//std::cout << "Channels already mapped" << std::endl;
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
		can[i] = new TCanvas(canname.c_str(),cantitle.c_str(),200,10,1200,800);
		for(j=0;j<16;j++){
			index = i*16 + j;
			histoname.assign("MPPC"+int2str(i)+"_Channel"+int2str(j));
			histotitle.assign("MPPC "+int2str(i)+" Channel "+int2str(j));
			hist[index] = new TH1I(histoname.c_str(), histotitle.c_str(),200,0,200);
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
				//if(channel>=16 && channel<32){
				if(channel>31 && channel<48){
					if(channel < 40){
					//if(channel < 24){
						index = 16*(map[channel] - 8) + i;
						//index = 16*(map[channel] - 16) + i;
					}else{
						//index = 16*(map[channel] - 16) + 8 + i;
						index = 16*(map[channel] - 8) + 8 + i;
					}
//std::cout << "Data: " << data << "    \tin channel: " << channel << "    \tmapped to: " << map[channel] << "    \tindex:" << index << std::endl;
					hist[index]->Fill(data);
				}
			}
			file.close();
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m opening the file:" << fileName << std::endl;
			exit(1);
		}
	}
	
	std::ofstream ofile;
	std::string imageName;

	imageName.assign("Images");
	imageName.append(fname.substr(fname.find_first_of('/')));
	imageName.assign(imageName.substr(0,imageName.length()-4));
	imageName.append("MPPC");

	ofile.open(std::string(fname+"-resume.dat").c_str());
	ofile << "MPPC\tChannel\tMean\tRMS\tconstant_0\tmean_0\tsigma_0\tconstant_1\tmean_1\tsigma_1\tconstant_2\tmean_2\tsigma_2" << std::endl;

	int binmax;
	Double_t max;
	TF1 **func = new TF1*[128];
	style->cd();
	style->SetOptFit(111);
	if(checkFolder(fname)){
		perror("ERROR: Something strange.");
		exit(1);
	}
	for(i=0;i<8;i++){
		//std::cout << "Fitting channels for MPPC: " << i << std::endl;
		for(j=0;j<16;j++){
			can[i]->cd();
			index = i*16+j;
			//hist[index]->SetLineColor(j);
			if(j==0)
				hist[index]->Draw();
			else
				hist[index]->Draw("same");
			if(gaussNum == 0)
				continue;
			max = hist[index]->GetMaximum();
			binmax = hist[index]->GetMaximumBin();
			switch(gaussNum){
				case 1:
					func[index] = new TF1("fiter",GausFunc,0,400,3); 
					break;
				case 2:
					func[index] = new TF1("fiter",myFunc2,0,400,6); 
					break;
				case 3:
					func[index] = new TF1("fiter",myFunc3,0,400,9); 
					break;
				case 4:
					func[index] = new TF1("fiter",myFunc4,0,400,12); 
					break;
				default:
					std::cout << "Not supported option" << std::endl;
					return;
			}
			switch(gaussNum){
			case 4:
				func[index]->SetParName(9,"constant_3");
				func[index]->SetParName(10,"mean_3");
				func[index]->SetParName(11,"sigma_3");
				func[index]->SetParLimits(9,0,30000);
				func[index]->SetParLimits(10,hist[index]->GetMean()-50,hist[index]->GetMean()+50);
				func[index]->SetParLimits(11,15,hist[index]->GetMean());
				func[index]->SetParameter(9,max);
				func[index]->SetParameter(10,hist[index]->GetMean());
				func[index]->SetParameter(11,hist[index]->GetRMS());
			case 3:
				func[index]->SetParName(6,"constant_2");
				func[index]->SetParName(7,"mean_2");
				func[index]->SetParName(8,"sigma_2");
				func[index]->SetParLimits(6,0,30000);
				func[index]->SetParLimits(7,binmax+25,binmax+55);
				func[index]->SetParLimits(8,0,15);
				func[index]->SetParameter(6,hist[index]->GetBinContent(binmax+40));
				func[index]->SetParameter(7, binmax+40.0);
				func[index]->SetParameter(8,   5.0);
			case 2:
				func[index]->SetParName(3,"constant_1");
				func[index]->SetParName(4,"mean_1");
				func[index]->SetParName(5,"sigma_1");
				func[index]->SetParLimits(3,0,30000);
				func[index]->SetParLimits(4,binmax+5,binmax+35);
				func[index]->SetParLimits(5,0,15);
				func[index]->SetParameter(3,hist[index]->GetBinContent(binmax+20));
				func[index]->SetParameter(4, binmax+20.0);
				func[index]->SetParameter(5,   5.0);
			case 1:
				func[index]->SetParName(0,"constant_0");
				func[index]->SetParName(1,"mean_0");
				func[index]->SetParName(2,"sigma_0");
				func[index]->SetParLimits(0,0,30000);
				func[index]->SetParLimits(1,binmax-15,binmax+15);
				func[index]->SetParLimits(2,0,15);
				func[index]->SetParameter(0,max);
				func[index]->SetParameter(1,binmax);
				func[index]->SetParameter(2,   5.0);
				break;
			case 0:
				break;
			default:
				std::cout << "Not supported option" << std::endl;
				return ;
			}
			hist[index]->Fit("fiter","Q+","",0,400);
			func[index]->SetLineColor(4);
			style->SetOptFit(111);
			//gStyle->SetOptFit(111);
			func[index]->Draw("same");
			ofile << i << "\t" << j << "\t" << hist[index]->GetMean();
			ofile << "\t" << hist[index]->GetRMS()<< "\t";
			ofile << func[index]->GetParameter(0) << "\t";
			ofile << func[index]->GetParameter(1) << "\t";
			ofile << func[index]->GetParameter(2) << "\t";
			if(gaussNum>1){
				ofile << func[index]->GetParameter(3) << "\t";
				ofile << func[index]->GetParameter(4) << "\t";
				ofile << func[index]->GetParameter(5) << "\t";
			}
			if(gaussNum>2){
				ofile << func[index]->GetParameter(6) << "\t";
				ofile << func[index]->GetParameter(7) << "\t";
				ofile << func[index]->GetParameter(8) << std::endl;
			}
			if(gaussNum>3){
				ofile << func[index]->GetParameter(9) << "\t";
				ofile << func[index]->GetParameter(10) << "\t";
				ofile << func[index]->GetParameter(11) << "\t";
			}
			ofile << std::endl;
		}
		ofile.close();
		can[i]->Print(std::string(imageName+int2str(i)+"-all-"+int2str(gaussNum)+"g.ps").c_str(),"ps");
		can[i]->Print(std::string(imageName+int2str(i)+"-all-"+int2str(gaussNum)+"g.png").c_str(),"png");
	}
}

int main(int argc, char **argv){
	if(argc == 4){
		int gn = atoi(argv[3]);
		allInOne(argv[1],argv[2],gn);
		return 0;
	}else{
		std::cout << "Usage:" << std::endl;
		std::cout << argv[0] << " FileMaskFormat Message NumberofGaussCurves" << std::endl;
		return 1;
	}
}
