

{


  TCanvas *c1 = new TCanvas("c1","Canvas",10,30,650,600);
  c1->SetGrid(1,1);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogx(1);

  TLegend* leg = new TLegend(0.15,0.65,0.3,0.8,NULL,"brNDC");
  leg->SetFillStyle(4000);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);

  TFile* f_Zeleele = new TFile("/data_CMS/cms/lbianchini/tagAndProbSpring11/treeEtoTauEff_DYToEE-20-PUS1.root");
  TFile* f_Ztautau = new TFile("/data_CMS/cms/lbianchini/tagAndProbSpring11/treeEtoTauEff_DYToTauTau-20-PUS1.root");

  f_Zeleele->cd();
  TTree* tree_Zeleele = (TTree*) gDirectory->Get("tauFakeRateAnalyzer/tree"); 
  f_Ztautau->cd();
  TTree* tree_Ztautau = (TTree*) gDirectory->Get("tauFakeRateAnalyzer/tree"); 

  TH2F* h2 = new TH2F("h2"," CMS Preliminary 2011  #sqrt{s}=7 TeV   Simulation; prompt-electron fake-rate ; hadronic tau efficiency",220,0,1.1,100,0.6,1.1);
  TH1F* h1 = new TH1F("h1","",1,-10,10);

  float vx_L[3];
  float vy_L[3];

  float vx_M[3];
  float vy_M[3];

  float vx_T[3];
  float vy_T[3];

  tree_Zeleele->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>0)");
  float ZeleeleL = h1->Integral();
  h1->Reset();
  tree_Ztautau->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>0)");
  float ZtautauL = h1->Integral();
  h1->Reset();

  tree_Zeleele->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>1)");
  float ZeleeleM = h1->Integral();
  h1->Reset();
  tree_Ztautau->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>1)");
  float ZtautauM = h1->Integral();
  h1->Reset();

  tree_Zeleele->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>2)");
  float ZeleeleT = h1->Integral();
  h1->Reset();
  tree_Ztautau->Draw("eta>>h1","mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>2)");
  float ZtautauT = h1->Integral();
  h1->Reset();
 

  for(int i = 0; i<3; i++){
    tree_Zeleele->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>0 && tightestAntiEWP>%d)",i));
    float Zeleele = h1->Integral();
    h1->Reset();
    tree_Ztautau->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>0 && tightestAntiEWP>%d)",i));
    float Ztautau = h1->Integral();
    h1->Reset();
    
    vx_L[i]=Zeleele/ZeleeleL;
    vy_L[i]=Ztautau/ZtautauL;
    cout << Zeleele << " --- " << Ztautau << endl;
  }

  for(int i = 0; i<3; i++){
    tree_Zeleele->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>1 && tightestAntiEWP>%d)",i));
    float Zeleele = h1->Integral();
    h1->Reset();
    tree_Ztautau->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>1 && tightestAntiEWP>%d)",i));
    float Ztautau = h1->Integral();
    h1->Reset();
    
    vx_M[i]=Zeleele/ZeleeleM;
    vy_M[i]=Ztautau/ZtautauM;
    cout << Zeleele << " --- " << Ztautau << endl;
  }

  for(int i = 0; i<3; i++){
    tree_Zeleele->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>2 && tightestAntiEWP>%d)",i));
    float Zeleele = h1->Integral();
    h1->Reset();
    tree_Ztautau->Draw("eta>>h1",Form("mcPUweight*(pt>20 && abs(eta)>1.5 && tightestHPSWP>2 && tightestAntiEWP>%d)",i));
    float Ztautau = h1->Integral();
    h1->Reset();
    
    vx_T[i]=Zeleele/ZeleeleT;
    vy_T[i]=Ztautau/ZtautauT;
    cout << Zeleele << " --- " << Ztautau << endl;
  }

  TGraph* graph_L = new TGraph(3,vx_L,vy_L);
  TGraph* graph_M = new TGraph(3,vx_M,vy_M);
  TGraph* graph_T = new TGraph(3,vx_T,vy_T);

  graph_L->SetMarkerColor(kBlue);
  graph_L->SetMarkerSize(1.8);
  graph_L->SetMarkerStyle(kOpenSquare);

  graph_M->SetMarkerColor(kRed);
  graph_M->SetMarkerSize(1.8);
  graph_M->SetMarkerStyle(kOpenCircle);

  graph_T->SetMarkerColor(kMagenta);
  graph_T->SetMarkerSize(1.8);
  graph_T->SetMarkerStyle(kOpenStar);

  h2->Draw();
  graph_L->Draw("PSAME");
  graph_M->Draw("PSAME");
  graph_T->Draw("PSAME");

  leg->SetHeader("|#eta|>1.5");
  leg->AddEntry(graph_L,"Loose Iso" ,"P");
  leg->AddEntry(graph_M,"Medium Iso","P");
  leg->AddEntry(graph_T,"Tight Iso" ,"P");

  leg->Draw();

}
