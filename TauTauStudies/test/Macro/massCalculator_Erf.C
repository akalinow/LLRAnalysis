

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
    c->SetLogy(0);

    TLegend* leg = new TLegend(0.50,0.65,0.75,0.85,NULL,"brNDC");
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetFillColor(10);
    leg->SetTextSize(0.04);
    if(mH[m]>90) 
      leg->SetHeader(Form("H(%d)#rightarrow#tau#tau",mH[m]));
    else
      //leg->SetHeader(Form("Z#rightarrow#tau#tau",mH[m]));
      leg->SetHeader(Form("t#bar{t}",mH[m]));
  
    TH1F* hNew = new TH1F("hNew","CMS Preliminary 2012 #sqrt{s}=7 TeV Simulation; SVFit Mass (GeV); events",20,0,400);
    TH1F* hOld = new TH1F("hOld","CMS Preliminary 2012 #sqrt{s}=7 TeV Simulation ; SVFit Mass (GeV); events",20,0,400);
  
    string file = (mH[m]==90) ? 
      "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleRun2011-MuTau-Embedded-All_run_Open_MuTauStream.root" :
      //"/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleTTJets-MuTau-madgraph-PUS6_run_Open_MuTauStream.root" :
      //"/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleRun2011-MuTau-All_run_Open_MuTauStream.root" :
      string(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleVBFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));

    //TFile f("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleRun2011-MuTau-Embedded-All_run_Open_MuTauStream.root");
    //TFile f(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleGGFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));
    TFile f(file.c_str());
    
    TTree* tree = (TTree*)f.Get("outTreePtOrd");
    
    float diTauSVFitMass, diTauNSVfitMassErrUp, MtLeg1, HLTx, diTauCharge, pt1,pt2,Deta,Mjj, combRelIsoLeg1DBeta;
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
    tree->SetBranchAddress("combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta);
    
    int nMax=200;
    
    //float c0 = 9.06333e-01;
    //float c1 = 4.04623e+01;
    //float c2 = 6.03565e-02;
    //float c3 = 3.98075e+01;
    //float c4 = 2.22399e-04;
    //float c5 = 9.70104e-03;
    //float c6 = -1.46589e-02;
    //float c7 = 1.29710e-01;

    float c0 = 9.08725e-01;
    float c1 = 4.01539e+01;
    float c2 = 7.48472e-02;
    float c3 = 3.79035e+01;
    float c4 = 1.93179e-04;
    float c5 = 1.35327e-02;
    float c6 = -1.36772e-02;
    float c7 = 6.04364e-05;
  
    for(int n = 0; n < tree->GetEntries() && n < 20000 ; n++){
      if(n%50000==0) cout << "Event " << n << endl;
      tree->GetEntry(n);
      
      if(!( MtLeg1<40 && diTauCharge==0 && muFlag==0 && combRelIsoLeg1DBeta>0.3)) continue;

      float mass      = diTauSVFitMass;
      float s         = diTauNSVfitMassErrUp;
      float massH     = 120;
      float bias      = (c0*massH+c1) + (c2*massH+c3)*TMath::Erf((c4*massH+c5)*s + (c6*massH+c7)) - massH;

      int applyCorr = int( diTauNSVfitMassErrUp > 0) ;

      hOld->Fill(mass);
      hNew->Fill(mass - applyCorr*( bias ));
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
    hNew->SetLineWidth(2);
    hOld->SetLineColor(kBlack);
    hOld->SetLineWidth(2);

    hNew->DrawNormalized("");
    hOld->DrawNormalized("SAME");
    
    cout << "Old Mass " << mH[m] << ": underflow " <<  hOld->GetBinContent(0) << endl;
    cout << "New Mass " << mH[m] << ": underflow " <<  hNew->GetBinContent(0) << endl;

    leg->AddEntry(hOld,Form("Raw: #mu=%.0f, RMS=%.0f",mean_old,sigma_old));
    leg->AddEntry(hNew,Form("Cal: #mu=%.0f, RMS=%.0f",mean_new,sigma_new));
    leg->Draw();
    
    c->SaveAs(Form("CalibratedMass_mH%d.pdf",mH[m]));

    f.Close();

    //delete h1; delete h2;
    
  }
  
}
