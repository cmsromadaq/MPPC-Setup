void fito(){
	Double_t min = 0.0;
	Double_t max = 4096.0;
	using namespace RooFit;
	TH1D* hist = new TH1D("hist","Channel 37",4096,min,max);
	int i,j,k;
	std::ifstream file;
	file.open("Data/20110902/run002-woled-addr5.dat");
	if(file.is_open()){
		while(file.good()){
			file >> i >> j >> k;
			if(j == 32){
				hist->Fill(k);
			}
		}
		file.close();
		RooRealVar x("x","x",min,max);
		RooGaussian gauss[3];

		RooRealVar mean0("mean0","Mean of Gaussian 1",103.0,min,max);
		RooRealVar mean1("mean1","Mean of Gaussian 2",130.0,min,max);
		RooRealVar mean2("mean2","Mean of Gaussian 3",150.0,min,max);
	
		RooRealVar sigma0("sigma0","Width of Gaussian",9.87555,min,max);
		RooRealVar sigma1("sigma1","Width of Gaussian",9.87555,min,max);
		RooRealVar sigma2("sigma2","Width of Gaussian",9.87555,min,max);

		RooGaussian gauss0("gauss0","gauss(x,mean,sigma)",x,mean0,sigma0);
		RooGaussian gauss1("gauss1","gauss(x,mean,sigma)",x,mean1,sigma1);
		RooGaussian gauss2("gauss2","gauss(x,mean,sigma)",x,mean2,sigma2);

		RooRealVar const1("const1","first part amplitud",0.1,0.,1.);
		RooRealVar const2("const2","second part amplitud",0.1,0.,1.);
	
		std::cout << "Building the model" << std::endl;
		RooAddPdf submodel("submodel","gauss1+gauss2",RooArgList(gauss1,gauss2),const1);
		RooAddPdf model("model","gauss0+(gauss1+gauss2)",RooArgList(gauss0,submodel),const2);
		
		TH1* hh = (TH1*) gDirectory->Get("hist");
		RooDataHist data("data","dataset with x",x,hh);
		
		std::cout << "Fitting" << std::endl;
		//gauss.fitTo(data,Range(98,110));
		model.fitTo(data);
		
		std::cout << "Drawing" << std::endl;
		RooPlot* xframe = x.frame();
		//gauss.plotOn(xframe,LineColor(kRed));
		data.plotOn(xframe,LineColor(kBlue));
		//model.plotOn(xframe,LineColor(kRed));
		gauss0.plotOn(xframe,LineColor(kGreen));
		gauss1.plotOn(xframe,LineColor(kGreen));
		gauss2.plotOn(xframe,LineColor(kGreen));
		xframe->Draw();
		//mean.Print();
		//sigma.Print();
	}else{
		perror("ERROR: File is closed");
		return;
	}
}

