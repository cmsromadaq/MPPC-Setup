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

void histo1DMPPC(std::string fname,std::string mess){
	int i,j,map[64];
	std::ifstream mapFile;
	std::string fileName;
	
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
	
	Double_t X[8],Y[8],Xerr[8],Yerr[8];
	TCanvas **can = new TCanvas*[8];
	TH1I **hist = new TH1I*[128];
	int event, channel, data, index;
	char evnt[11], chn[11], dt[11];
	std::string histoname,histotitle,canname,cantitle;

	for(i=0;i<8;i++){
		canname.assign("Canvas"+int2str(i)+mess);
		cantitle.assign("Data MPPC "+int2str(i+8)+mess);
		can[i] = new TCanvas(canname.c_str(),cantitle.c_str(),200,10,2500,800);
		can[i]->Divide(8,2);
		for(j=0;j<16;j++){
			index = i*16 + j;
			histoname.assign("hist"+int2str(index));
			histotitle.assign("MPPC "+int2str(i)+" Channel "+int2str(j));
			hist[index] = new TH1I(histoname.c_str(), histotitle.c_str(),400,0,400);
		}
	}

	std::ofstream ofile;
	ofile.open(std::string(fname+"-resume.dat").c_str());

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
	ofile.open(std::string(fname+"-resume.dat").c_str());
	ofile << "MPPC\tChannel\tMean\tRMS" << std::endl;

	for(i=0;i<8;i++){
		for(j=0;j<16;j++){
			can[i]->cd(j+1);
			index = i*16+j;
			hist[index]->Draw();
			ofile << i << "\t" << j << "\t" << hist[index]->GetMean();
			ofile << "\t" << hist[index]->GetRMS() << std::endl;
		}
	}
	ofile.close();
	
}

