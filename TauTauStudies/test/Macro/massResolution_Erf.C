

{

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);


  vector<int> mH;
  //mH.push_back(90);
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
  Float_t p2s[]  = {0,0,0,0,0,0,0,0};
  Float_t p2sE[] = {0,0,0,0,0,0,0,0};
  Float_t p3s[]  = {0,0,0,0,0,0,0,0};
  Float_t p3sE[] = {0,0,0,0,0,0,0,0};

  for(unsigned int m = 0; m< mH.size() ; m++){

    cout << "Mass mH = " << mH[m] << endl;

    TCanvas *c2 = new TCanvas("c2","",5,30,650,600);
    c2->SetGrid(0,0);
    c2->SetFillStyle(4000);
    c2->SetFillColor(10);
    c2->SetTicky();
    c2->SetObjectStat(0);

    TLegend* leg = new TLegend(0.50,0.15,0.75,0.55,NULL,"brNDC");
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetFillColor(10);
    leg->SetTextSize(0.04);
    leg->SetHeader(Form("H(%d)#rightarrow#tau#tau",mH[m]));
    //leg->SetHeader("Z#rightarrow#tau#tau");



    TFile f(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleGGFH%d-MuTau-powheg-PUS6_run_Open_MuTauStream.root",mH[m]));
    //TFile f("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStreamFall11_09Feb2012/nTupleDYJets-MuTau-50-madgraph-PUS6_run_Open_MuTauStream.root");
    TTree* tree = (TTree*)f.Get("outTreePtOrd");
    TH2F* h2 = new TH2F("h2","",10,0,100,80,0,400);
    TH1F* h1 = new TH1F("h1","CMS Preliminary 2012 #sqrt{s}=7 TeV Simulation; #sigma_{SVFit} (GeV/c^{2}); #mu_{SVFit} (GeV/c^{2})",10,0,100);
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


    TF1 *func = new TF1("func","[0]+[1]*TMath::Erf([2]*x+[3])",0,100);
    func->SetParLimits(0,70,180);
    func->SetParLimits(1,0,100);
    func->SetParLimits(2,0,1);
    func->SetParLimits(3,-10,10);

    h1->Fit("func","","",0,100);

    TF1 *fit = h1->GetFunction("func");
    Double_t p0 = fit->GetParameter(0);
    Double_t p0E = fit->GetParError(0);
    Double_t p1 = fit->GetParameter(1);
    Double_t p1E = fit->GetParError(1);
    Double_t p2 = fit->GetParameter(2);
    Double_t p2E = fit->GetParError(2);
    Double_t p3 = fit->GetParameter(3);
    Double_t p3E = fit->GetParError(3);

    cout << "p0 = " << p0 << " +/- " << p0E << endl; 
    p0s[m] = p0;
    p0sE[m] = p0E;
    cout << "p1 = " << p1 << " +/- " << p1E << endl; 
    p1s[m] = p1;
    p1sE[m] = p1E;
    cout << "p2 = " << p2 << " +/- " << p2E << endl; 
    p2s[m] = p2;
    p2sE[m] = p2E;
    cout << "p3 = " << p3 << " +/- " << p3E << endl; 
    p3s[m] = p3;
    p3sE[m] = p3E;
    
    c2->cd();

    h1->SetStats(0);
    h1->Draw("E");
    leg->AddEntry(func,Form( "#splitline{#splitline{p_{0} = %.0f #pm %.0f}{p_{1} = %.0f #pm %.0f}}{#splitline{p_{2} = %.3f #pm %.3f}{p_{3} = %.2f #pm %.2f} }" ,p0,p0E,p1,p1E,p2,p2E,p3,p3E));
    leg->Draw(); 

    c2->SaveAs(Form("ResVsMean_mH%d.pdf",mH[m]));   
    f.Close();
  }
  //
  //return;

  c1->cd();


  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("");
 
  TGraphAsymmErrors* graph_p0 = new TGraphAsymmErrors(8, mHs, p0s, mHsE , mHsE, p0sE, p0sE);
  graph_p0->SetMarkerColor(kBlack);
  graph_p0->SetMarkerStyle(kOpenCircle);
  graph_p0->SetMarkerSize(2.0);
  graph_p0->SetLineColor(kBlack);
  graph_p0->SetLineStyle(kSolid);
  graph_p0->SetLineWidth(2);

  TGraphAsymmErrors* graph_p1 = new TGraphAsymmErrors(8, mHs, p1s, mHsE , mHsE, p1sE, p1sE);
  graph_p1->SetMarkerColor(kRed);
  graph_p1->SetMarkerStyle(kFullCircle);
  graph_p1->SetMarkerSize(2.0);
  graph_p1->SetLineColor(kRed);
  graph_p1->SetLineStyle(kDashed);
  graph_p1->SetLineWidth(2);

  TGraphAsymmErrors* graph_p2 = new TGraphAsymmErrors(8, mHs, p2s, mHsE , mHsE, p2sE, p2sE);
  graph_p2->SetMarkerColor(kBlue);
  graph_p2->SetMarkerStyle(kOpenSquare);
  graph_p2->SetMarkerSize(2.0);
  graph_p2->SetLineColor(kBlue);
  graph_p2->SetLineStyle(kDotted);
  graph_p2->SetLineWidth(2);

  TGraphAsymmErrors* graph_p3 = new TGraphAsymmErrors(8, mHs, p3s, mHsE , mHsE, p3sE, p3sE);
  graph_p3->SetMarkerColor(kMagenta);
  graph_p3->SetMarkerStyle(kFullCircle);
  graph_p3->SetMarkerSize(2.0);
  graph_p3->SetLineColor(kMagenta);
  graph_p3->SetLineStyle(5);
  graph_p3->SetLineWidth(2);

  //mg->Add(graph_p0);
  //mg->Add(graph_p1);
  //mg->Add(graph_p2);
  mg->Add(graph_p3);

  mg->Draw("AP");
  
  gPad->Modified();
  mg->GetXaxis()->SetLimits(105,150);
  mg->GetYaxis()->SetTitleOffset(1.05);
  mg->GetYaxis()->SetTitle("Coefficient");
  mg->GetXaxis()->SetTitle("m_{H} (GeV/c^{2})");
  mg->SetMinimum(-2.8);
  mg->SetMaximum(-1.1);

  gStyle->SetOptFit(0);

  TF1 *p0Func = new TF1("p0Func","[0]*x + [1]",90,145);
  p0Func->SetLineColor(kBlack);
  TF1 *p1Func = new TF1("p1Func","[0]*x + [1]",90,145);
  p1Func->SetLineColor(kRed);
  TF1 *p2Func = new TF1("p2Func","[0]*x + [1]",90,145);
  p2Func->SetLineColor(kBlue);
  TF1 *p3Func = new TF1("p3Func","[0]*x + [1]",90,145);
  p3Func->SetLineColor(kMagenta);

  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p0->Fit("p0Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p1->Fit("p1Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p2->Fit("p2Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;
  graph_p3->Fit("p3Func","","",110,145);
  cout << "@@@@@@@@@@@@@@@@@@@@@@" << endl;

  TLegend* leg = new TLegend(0.30,0.12,0.55,0.45,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  //leg->AddEntry(p0Func,Form("p_{0} #splitline{Slope = %.2f #pm %.2f}{Intercept = %.0f #pm %.0f}",p0Func->GetParameter(0),p0Func->GetParError(0),p0Func->GetParameter(1),p0Func->GetParError(1) ));
  //leg->AddEntry(p1Func,Form("p_{1} #splitline{Slope = %.2f #pm %.2f}{Intercept = %.0f #pm %.0f}",p1Func->GetParameter(0),p1Func->GetParError(0),p1Func->GetParameter(1),p1Func->GetParError(1) ));
  //leg->AddEntry(p2Func,Form("p_{2} #splitline{Slope = %.5f #pm %.5f}{Intercept = %.4f #pm %.4f}",p2Func->GetParameter(0),p2Func->GetParError(0),p2Func->GetParameter(1),p2Func->GetParError(1) ));
  leg->AddEntry(p3Func,Form("p_{3} #splitline{Slope = %.3f #pm %.3f}{Intercept = %.2f #pm %.2f}",p3Func->GetParameter(0),p3Func->GetParError(0),p3Func->GetParameter(1),p3Func->GetParError(1) ));


  leg->Draw(); 

  //c1->SaveAs("ParametersVsMH_p3.pdf");

}
