#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#ifdef CHECKFOLDEREXISTANCE
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <TH1I.h>
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

#ifdef CHECKFOLDEREXISTANCE
bool checkFolder(std::string fname){
	DIR *dp;
	dirent *d;
	char *dirimghome = getenv("HOME");
	char *imgHome = new char[40];
	strcpy(imgHome,"/data/test/Images");
	std::string folder = fname.substr(5,8);
	//std::cout << "Opening the folder " << imgHome << std::endl;
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
		//std::cout << "Creating folder: " << folder << std::endl;
		if(mkdir(folder.c_str(),S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)){
			std::cerr << "ERROR: there is not possible to continue due to problems in folder structure. Trying to create folder: " << folder << std::endl;
			//perror("ERROR: there is not possible to continue due to problems in folder structure");
			return true;
		}
		closedir(dp);
	}else{
		perror("ERROR: there is not possible to continue due to problems in folder structure");
		return true;
	}
	return false;
}
#endif
void ufitMPPC(char* fnamex,char* message,int gaussNum){
	int maxHistoBin = 4096;
	int minHistoBin = 0;
	std::string fname(fnamex);
	int i,j,map[64],pinout[16];
	std::ifstream mapFile;
	std::string fileName;
	char *home = new char[45];
	
	TStyle *style = new TStyle("mrfs","MPPC Response Fitting Style");
	//TROOT groot;
	//groot.ProcessLine("gErrorIgnoreLevel = 1001;");
	gROOT->ProcessLine("gErrorIgnoreLevel = 1001;");
	TCanvas *can = new TCanvas("can","Channels from QDC",200,10,3600,869);
	can->Divide(8,2);
	TH1I **hist = new TH1I*[64];
	int event, channel, data, index;
	char evnt[11], chn[11], dt[11];
	std::string histoname,histotitle,canname,cantitle;

	// Initiallizing Canvas and Histograms classes
	for(i=0;i<64;i++){
		histoname.assign("Channel"+int2str(i));
		histotitle.assign("Channel "+int2str(i));
		hist[i] = new TH1I(histoname.c_str(), histotitle.c_str(),maxHistoBin - minHistoBin,minHistoBin,maxHistoBin);
	}
	// Filling Histograms
	std::ifstream file;
	file.open(fnamex);
	if(file.is_open()){
		while(file.good()){
			file >> event >> channel >> data;
			hist[channel]->Fill(data);
		}
		file.close();
	}else{
		std::cerr << "\e[0;31mERROR:\e[0m opening the file:" << fnamex << std::endl;
		exit(1);
	}
	
	std::ofstream ofile;
	std::string imageName;

	imageName.assign("Images");
	int imgNameBegin = fname.find_first_of('/');
	int imgNameEnd   = fname.find_first_of('-');
	imageName.append(fname.substr(imgNameBegin,imgNameEnd - imgNameBegin)+"-");
	imageName.append("QDC");
	std::cout << imageName << std::endl;

#ifdef CHECKFOLDEREXISTANCE
	if(checkFolder(fname)){
		perror("ERROR: Something strange.");
		exit(1);
	}
#endif
	int draw = 0;
	for(i=0;i<64;i++){
		if(hist[i]->GetEntries() == 0){
			delete hist[i];
		}else{
			draw++;
			can->cd(draw);
			hist[i]->Draw();
			//draw++;
		}
	}
#ifdef CHECKFOLDEREXISTANCE
	can->Print(std::string(imageName+".ps").c_str(),"ps");
	can->Print(std::string(imageName+".png").c_str(),"png");
#endif
}

int main(int argc, char **argv){
	if(argc == 4){
		int gn = atoi(argv[3]);
		ufitMPPC(argv[1],argv[2],gn);
		return 0;
	}else{
		std::cout << "Usage:" << std::endl;
		std::cout << argv[0] << " FileMaskFormat Message NumberofGaussCurves" << std::endl;
		return 1;
	}
}
