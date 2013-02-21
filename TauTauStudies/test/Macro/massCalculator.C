

{

  vector<int> mH;
  mH.push_back(90);
  //mH.push_back(110);
  //mH.push_back(115);
  //mH.push_back(120);
  //mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);
  //mH.push_back(140);
  //mH.push_back(145);

  for(unsigned int m=0; m< mH.size() ; m++){

    TCanvas *c = new TCanvas(Form("c%d",mH[m]),"",5,30,650,600);
    c->SetGrid(0,0);
    c->SetFillStyle(4000);
    c->SetFillColor(10);
    c->SetTicky();
    c->SetObjectStat(0);

    TLegend* leg = new TLegend(0.55,0.65,0.80,0.85,NULL,"brNDC");
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetFillColor(10);
    leg->SetTextSize(0.04);
    leg->SetHeader(Form("m_{H}=%d",mH[m]));

    
    TH1F* hNew = new TH1F("hNew","CMS Preliminary 2012 #sqrt{s}=7 TeV Simulation; SVFit Mass (GeV); events",50,0,400);
    TH1F* hOld = new TH1F("hOld","CMS Preliminary 2012 #sqrt{s}=7 TeVSimulation ; SVFit Mass (GeV); events",50,0,400);
    
    string file = (mH[m]==90) ? 
      //"/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleRun2011-MuTau-Embedded-All_run_Open_MuTauStream.root" :
      "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleWJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root" :
      string(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleVBFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));

    //TFile f("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleRun2011-MuTau-Embedded-All_run_Open_MuTauStream.root");
    //TFile f(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleGGFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));
    TFile f(file.c_str());
    
    TTree* tree = (TTree*)f.Get("outTreePtOrd");
    
    float diTauSVFitMass, diTauNSVfitMassErrUp, MtLeg1, HLTx, diTauCharge, pt1,pt2,Deta,Mjj;
    int  muFlag;
    tree->SetBranchAddress("diTauSVFitMass",&diTauSVFitMass);
    tree->SetBranchAddress("diTauNSVfitMassErrUp",&diTauNSVfitMassErrUp);
    tree->SetBranchAddress("MtLeg1",&MtLeg1);
    tree->SetBranchAddress("HLTx",&HLTx);
    tree->SetBranchAddress("diTauCharge",&diTauCharge);
    tree->SetBranchAddress("muFlag",&muFlag);
    tree->SetBranchAddress("pt1",&pt1);
    tree->SetBranchAddress("pt2",&pt2);
    tree->SetBranchAddress("Deta",&Deta);
    tree->SetBranchAddress("Mjj",&Mjj);
    
    int nMax=200;
    
    float c0 = 0.0050;
    float c1 = 1.14;
    float c2 = 0.40;
    float c3 = 25;

    float c4 = -0.0059;
    float c5 = 0.44;
    float c6 = 0.36;
    float c7 = -17;
  
    for(int n = 0; n < tree->GetEntries() && n < 10000 ; n++){
      if(n%50000==0) cout << "Event " << n << endl;
      tree->GetEntry(n);
      
      if(!( MtLeg1<40 && diTauCharge==0 && muFlag==0)) continue;

      float mass = diTauSVFitMass;
      float mCalibr   = mass;
      
      float L = 0;
      for(int iter=0; iter<nMax && diTauNSVfitMassErrUp>30; iter++){
	float mHtest = 2.0*iter;
	//cout << mHtest << endl;
	double Lprime = TMath::Exp( -pow(mass-((c0*mHtest + c1)*diTauNSVfitMassErrUp + (c2*mHtest+c3)),2)/2*pow((c4*mHtest+c5)diTauNSVfitMassErrUp + (c6*mHtest+c7),2) ) ;
	//cout << Lprime << endl;
	if( Lprime > L){
	  L = Lprime;
	  mCalibr = mHtest;
	}
      }
      int applyCorr = int( diTauNSVfitMassErrUp > 0) ;
      //if(n%500==0) cout << "Old " << mass << " => new " << mH << endl; 
      //hNew->Fill(mCalibr);
      hOld->Fill(mass);
      hNew->Fill(mass - applyCorr*(((c0*120 + c1)*diTauNSVfitMassErrUp + (c2*120+c3)) - 120));
    }
    
    
    TF1 *gaus_old = new TF1("g_old","gaus",0,400);
    //hOld->Fit("g_old","","",mH[m]-40,mH[m]+40);
    TF1 *gausFit_old = hOld->GetFunction("g_old");
    Double_t mean_old      = hOld->GetMean();
    Double_t meanError_old = 0;
    Double_t sigma_old      = hOld->GetRMS();
    Double_t sigmaError_old = 0;
    if(gausFit_old && false){
      mean_old      = gausFit_old->GetParameter(1);
      meanError_old = gausFit_old->GetParError(1);
      sigma_old      = gausFit_old->GetParameter(2);
      sigmaError_old = gausFit_old->GetParError(2);
    }
    TF1 *gaus_new = new TF1("g_new","gaus",0,400);
    gaus_new->SetLineColor(kRed);
    //hNew->Fit("g_new","","",mH[m]-40,mH[m]+40);
    TF1 *gausFit_new = hNew->GetFunction("g_new");
    Double_t mean_new      = hNew->GetMean();
    Double_t meanError_new = 0;
    Double_t sigma_new      = hNew->GetRMS();
    Double_t sigmaError_new = 0;
    if(gausFit_new && false){
      mean_new      = gausFit_new->GetParameter(1);
      meanError_new = gausFit_new->GetParError(1);
      sigma_new      = gausFit_new->GetParameter(2);
      sigmaError_new = gausFit_new->GetParError(2);
    }
    
    hNew->SetStats(0);
    hOld->SetStats(0);
    
    c->cd();
    
    hNew->SetBinContent(1,hNew->GetBinContent(0)+hNew->GetBinContent(1));
    hOld->SetBinContent(1,hOld->GetBinContent(0)+hOld->GetBinContent(1));

    hNew->SetLineColor(kRed);
    hNew->Draw("");
    hOld->Draw("SAME");
    
    cout << "Old Mass " << mH[m] << ": underflow " <<  hOld->GetBinContent(0) << endl;
    cout << "New Mass " << mH[m] << ": underflow " <<  hNew->GetBinContent(0) << endl;

    leg->AddEntry(hOld,Form("Raw: #mu=%.0f, #sigma=%.0f",mean_old,sigma_old));
    leg->AddEntry(hNew,Form("Cal: #mu=%.0f, #sigma=%.0f",mean_new,sigma_new));
    leg->Draw();
    
    //c->SaveAs(Form("calibration_mH%d.png",mH[m]));
    f.Close();
    
  }
  
}
