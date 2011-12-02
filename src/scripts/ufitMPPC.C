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
	int maxHistoBin = 1000;
	int minHistoBin = 0;
	std::string fname;
	int i,j,map[64],pinout[16];
	std::ifstream mapFile;
	std::string fileName;
	char *home = new char[45];

	fname.assign(fnamex);
	//home = getenv("HOME");
	//strcpy(home,getenv("HOME"));
	strcpy(home,"/data/test");
	strcat(home,"/etc/qdcconfig.map");
	//std::cout << "Opening mapping file in: " << home << std::endl;
	mapFile.open(home);
	if(mapFile.is_open()){
		while(mapFile.good()){
			mapFile >> i >> j;
			if(i>=0 && i<64)
				map[i] = j;
		}
		//std::cout << "MPPC pinout already mapped" << std::endl;
		mapFile.close();
	}else{
		fprintf(stderr,"ERROR: There is not possible to map channel. Be careful");
		for(i=0;i<64;i++)
			map[i] = i;
	}
	
	strcpy(home,"/data/test");
	strcat(home,"/etc/MPPCPinOut.map");
	//std::cout << "Opening pinout file in: " << home << std::endl;
	mapFile.open(home);
	if(mapFile.is_open()){
		while(mapFile.good()){
			mapFile >> i >> j;
			if(i>=0 && i<16)
				pinout[i] = j;
		}
		std::cout << "MPPC channel numbering according to document: \"JEFFERSON 110624 S12045.pdf\"" << std::endl;	
		mapFile.close();
	}else{
		fprintf(stderr,"ERROR: There is not possible to map channel. Be careful");
		for(i=0;i<64;i++)
			map[i] = i;
	}
	
	TStyle *style = new TStyle("mrfs","MPPC Response Fitting Style");
	//TROOT groot;
	//groot.ProcessLine("gErrorIgnoreLevel = 1001;");
	gROOT->ProcessLine("gErrorIgnoreLevel = 1001;");
	TCanvas **can = new TCanvas*[32];
	TH1I **hist = new TH1I*[512];
	int event, channel, data, index;
	char evnt[11], chn[11], dt[11];
	std::string histoname,histotitle,canname,cantitle;

	// Initiallizing Canvas and Histograms classes
	for(i=0;i<32;i++){
		canname.assign("Canvas"+int2str(i)+message);
		cantitle.assign("Data MPPC "+int2str(i)+message);
		can[i] = new TCanvas(canname.c_str(),cantitle.c_str(),200,10,3600,869);
		can[i]->Divide(8,2);
		for(j=0;j<16;j++){
			index = i*16 + j;
			histoname.assign("MPPC"+int2str(i)+"_Channel"+int2str(j));
			histotitle.assign("MPPC "+int2str(i)+" Channel "+int2str(j));
			hist[index] = new TH1I(histoname.c_str(), histotitle.c_str(),maxHistoBin - minHistoBin,minHistoBin,maxHistoBin);
		}
	}

	// Filling Histograms
	for(i=0;i<8;i++){
		fileName.assign(fname+"-addr"+int2str(i)+".dat");
		//std::cout << "Openning the file: " << fileName << std::endl;
		std::ifstream file;
		file.open(fileName.c_str());
		if(file.is_open()){
			while(file.good()){
				file >> event >> channel >> data;
				if(((int)(channel%16)/8) == 1){
					index = 16*(map[channel]) + pinout[i];
				}else{
					index = 16*(map[channel]) + pinout[8 + i];
				}
				//index = channel*8 + i;
				hist[index]->Fill(data);
			}
			file.close();
		}else{
			std::cerr << "\e[0;31mERROR:\e[0m opening the file:" << fileName << std::endl;
			//exit(1);
		}
	}
	
	std::ofstream ofile;
	std::string imageName;

	imageName.assign("Images");
	imageName.append(fname.substr(fname.find_first_of('/'))+"-");
	imageName.append("MPPC");

	std::string outputfilename;
	outputfilename.assign(fname);
	outputfilename.append("-resume.dat");
	//std::cout << "Opening file: " << outputfilename << std::endl;
	ofile.open(outputfilename.c_str());
	//ofile.open(std::string(fname+"-resume.dat").c_str());
	if(!ofile.is_open())
		std::cerr << "ERROR: Output file not opened" << std::endl;
	ofile << "MPPC\tChannel\tMean\tRMS\tconstant_0\tmean_0\tsigma_0\tconstant_1\tmean_1\tsigma_1\tconstant_2\tmean_2\tsigma_2" << std::endl;

	int binmax,draw;
	Double_t max;
	Double_t separation = 50;
	TF1 **func = new TF1*[512];//("fiter",myFunc,0,400,9);
	//TStyle *style = new TStyle("mrfs","MPPC Response Fitting Style");
	style->cd();
	style->SetOptFit(111);
#ifdef CHECKFOLDEREXISTANCE
	if(checkFolder(fname)){
		perror("ERROR: Something strange.");
		exit(1);
	}
#endif
	for(i=0;i<32;i++){
		//std::cout << "Fitting channels for MPPC: " << i << std::endl;
		draw = 0;
		for(j=0;j<16;j++){
			can[i]->cd(j+1);
			index = i*16+j;
			if(hist[index]->GetEntries() == 0){
				delete hist[index];
				draw++;
				continue;
			}
			hist[index]->Draw();
			ofile << i << "\t" << j << "\t" << hist[index]->GetMean();
			ofile << "\t" << hist[index]->GetRMS()<< "\t";
			max = hist[index]->GetMaximum();
			binmax = hist[index]->GetMaximumBin();
			if(gaussNum == 0){
				ofile << std::endl;
				continue;
			}
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
				func[index]->SetParLimits(7,binmax-15+2*separation,binmax+15+2*separation);
				func[index]->SetParLimits(8,0,15);
				func[index]->SetParameter(6,hist[index]->GetBinContent(binmax+2*separation));
				func[index]->SetParameter(7, binmax+2*separation);
				func[index]->SetParameter(8,   5.0);
			case 2:
				func[index]->SetParName(3,"constant_1");
				func[index]->SetParName(4,"mean_1");
				func[index]->SetParName(5,"sigma_1");
				func[index]->SetParLimits(3,0,30000);
				func[index]->SetParLimits(4,binmax-15+separation,binmax+15+separation);
				func[index]->SetParLimits(5,0,15);
				func[index]->SetParameter(3,hist[index]->GetBinContent(binmax+separation));
				func[index]->SetParameter(4, binmax+separation);
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
				std::cerr << "ERROR: Not supported option" << std::endl;
				return ;
			}
			hist[index]->Fit("fiter","Q+","",0,400);
			func[index]->SetLineColor(4);
			style->SetOptFit(111);
			//gStyle->SetOptFit(111);
			func[index]->Draw("same");
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
#ifdef CHECKFOLDEREXISTANCE
		if(draw < 16){
			if(i<10){
				can[i]->Print(std::string(imageName+"0"+int2str(i)+"-"+int2str(gaussNum)+"g.ps").c_str(),"ps");
				can[i]->Print(std::string(imageName+"0"+int2str(i)+"-"+int2str(gaussNum)+"g.png").c_str(),"png");
			}else{
				can[i]->Print(std::string(imageName+int2str(i)+"-"+int2str(gaussNum)+"g.ps").c_str(),"ps");
				can[i]->Print(std::string(imageName+int2str(i)+"-"+int2str(gaussNum)+"g.png").c_str(),"png");
			}
		}
#endif
	}
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
