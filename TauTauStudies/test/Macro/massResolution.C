

{

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);


  vector<int> mH;
  mH.push_back(110);
  mH.push_back(115);
  mH.push_back(120);
  mH.push_back(125);
  mH.push_back(130);
  mH.push_back(135);
  mH.push_back(140);
  mH.push_back(145);

  Float_t mHs[]  = {110,115,120,125,130,135,140,145};
  Float_t mHsE[]  = {0,0,0,0,0,0,0,0};
  Float_t p0s[]  = {0,0,0,0,0,0,0,0};
  Float_t p0sE[] = {0,0,0,0,0,0,0,0};
  Float_t p1s[]  = {0,0,0,0,0,0,0,0};
  Float_t p1sE[] = {0,0,0,0,0,0,0,0};

  for(unsigned int m = 0; m< mH.size() ; m++){

    cout << "Mass mH = " << mH[m] << endl;

    TFile f(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleGGFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));
    TTree* tree = (TTree*)f.Get("outTreePtOrd");
    TH2F* h2 = new TH2F("h2","",10,0,100,80,0,400);
    TH1F* h1 = new TH1F("h1","CMS Preliminary 2012 #sqrt{s}=7 TeV Simulation; #sigma_{SVFit} (GeV); mean SVFit Mass (GeV)",10,0,100);
    tree->Draw("(diTauSVFitMass):diTauNSVfitMassErrUp>>h2",
	       "puWeight*HqTWeight*(MtLeg1<40 && muFlag==0 && diTauCharge==0 && HLTx && ptL1>17)",
	       "BOX");
    TH1F* h2_py = 0;
    for(int k = 0 ; k < 10 ; k++){
      h2_py = (TH1F*)h2->ProjectionY("_py",k+1,k+1,"");
      TF1 *gaus = new TF1("g","gaus",0,400);
      h2_py->Fit("g","","",40,160);
      TF1 *gausFit = h2_py->GetFunction("g");
      Double_t mean      = 0;
      Double_t meanError = 0;
      Double_t sigma      = 0;
      Double_t sigmaError = 0;
      if(gausFit){
	mean      = gausFit->GetParameter(1);
	meanError = gausFit->GetParError(1);
	sigma      = gausFit->GetParameter(2);
	sigmaError = gausFit->GetParError(2);
      }
      Double_t M = gausFit==0 ? h2_py->GetMean() : sigma ; 
      M = h2_py->GetMean();
      Double_t E = gausFit==0 ? h2_py->GetMeanError() : sigmaError; 
      E = h2_py->GetMeanError();
      h1->SetBinContent(k+1, M);
      h1->SetBinError(k+1, E );
    }

    //TF1 *func = new TF1("func","[0]*x + [1]",0,100);
    TF1 *func = new TF1("func","[0]+[1]*TMath::Erf([2]*x + [3])",0,100);
    //func->SetRange(20,80);
    //h1->Fit("func","","",20,80);
    func->SetParLimits(0,70,200);
    func->SetParLimits(1,0,100);
    func->SetParLimits(2,0,200);
    func->SetParLimits(2,0,10000);

    h1->Fit("func","","",0,100);


    TF1 *fit = h1->GetFunction("func");
    Double_t p0 = fit->GetParameter(0);
    Double_t p1 = fit->GetParameter(1);
    Double_t p0E = fit->GetParError(0);
    Double_t p1E = fit->GetParError(1);

    cout << "p0 = " << p0 << " +/- " << p0E << endl; 
    p0s[m] = p0;
    p0sE[m] = p0E;
    cout << "p1 = " << p1 << " +/- " << p1E << endl; 
    p1s[m] = p1;
    p1sE[m] = p1E;
    
    f.Close();
    //h1->Draw("E");
    
  }

  //return;


  c1->cd();

  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("");
 
  TGraphAsymmErrors* graph_p0 = new TGraphAsymmErrors(8, mHs, p0s, mHsE , mHsE, p0sE, p0sE);
  graph_p0->SetMarkerColor(kBlack);
  graph_p0->SetMarkerStyle(kOpenCircle);
  graph_p0->SetMarkerSize(2.0);
  graph_p0->SetLineColor(kRed);
  graph_p0->SetLineStyle(kDashed);
  graph_p0->SetLineWidth(2);

  TGraphAsymmErrors* graph_p1 = new TGraphAsymmErrors(8, mHs, p1s, mHsE , mHsE, p1sE, p1sE);
  graph_p1->SetMarkerColor(kBlack);
  graph_p1->SetMarkerStyle(kOpenCircle);
  graph_p1->SetMarkerSize(2.0);
  graph_p1->SetLineColor(kRed);
  graph_p1->SetLineStyle(kDashed);
  graph_p1->SetLineWidth(2);

  mg->Add(graph_p0);
  mg->Add(graph_p1);

  mg->Draw("ALP");
  
  c1->cd();
  gPad->Modified();
  mg->GetXaxis()->SetLimits(90,145);
  mg->GetYaxis()->SetTitleOffset(0.97);
  mg->GetYaxis()->SetTitle("Coefficients (1/GeV or units)");
  mg->GetXaxis()->SetTitle("m_{H} (GeV/c^{2})");
  mg->SetMinimum(0);
  mg->SetMaximum(100);


  TF1 *p0Func = new TF1("p0Func","[0]*x + [1]",90,145);
  TF1 *p1Func = new TF1("p1Func","[0]*x + [1]",90,145);

  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p0->Fit("p0Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p1->Fit("p1Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;





}
