

{


  TCanvas *c1 = new TCanvas("c1Mass","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);

  TLegend* leg = new TLegend(0.65,0.65,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  vector<string> samples;
  samples.push_back("Z");
  samples.push_back("W");
  samples.push_back("QCD");
  samples.push_back("TTbar");

  TFile* fZ      = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleDYJets-madgraph-50-PU-L.root");
  TFile* fZMuMu  = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets.root");
  TFile* fW      = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleWJets-madgraph-PU-L.root");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleQCD-pythia-PU-L.root");
  TFile* fTTbar  = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleTT-madgraph-PU-L.root");
  TFile* fqqH115 = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleVBFH115-PU-L.root");
  // TFile* fqqH115 = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets.root");

  TString ordering = "outTreePtOrd";

  TTree* treeZ     = (TTree*)fZ->Get(ordering);
  TTree* treeZMuMu     = (TTree*)fZMuMu->Get(ordering);
  TTree* treeW     = (TTree*)fW->Get(ordering);
  TTree* treeQCD   = (TTree*)fQCD->Get(ordering);
  TTree* treeTTbar = (TTree*)fTTbar->Get(ordering);
  TTree* treeqqH115 = (TTree*)fqqH115->Get(ordering);

  ////////////////////////////////////////////////////////////////////////
  int nBins_pt1   = 30;
  float xLow_pt1  = 10;
  float xHigh_pt1 = 210;

  TH1F* hZ_pt1      = new TH1F("hZ_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  TH1F* hZMuMu_pt1  = new TH1F("hZMuMu_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  TH1F* hW_pt1      = new TH1F("hW_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  TH1F* hQCD_pt1    = new TH1F("hQCD_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  TH1F* hTTbar_pt1  = new TH1F("hTTbar_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  TH1F* hqqH115_pt1 = new TH1F("hqqH115_pt1"," ; p_{T}^{lead} (GeV) ; a.u.",nBins_pt1,xLow_pt1,xHigh_pt1);
  ////////////////////////////////////////////////////////////////////////
  int nBins_pt2   = 20;
  float xLow_pt2  = 10;
  float xHigh_pt2 = 110;

  TH1F* hZ_pt2      = new TH1F("hZ_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  TH1F* hZMuMu_pt2  = new TH1F("hZMuMu_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  TH1F* hW_pt2      = new TH1F("hW_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  TH1F* hQCD_pt2    = new TH1F("hQCD_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  TH1F* hTTbar_pt2  = new TH1F("hTTbar_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  TH1F* hqqH115_pt2 = new TH1F("hqqH115_pt2"," ; p_{T}^{sub-lead} (GeV) ; a.u.",nBins_pt2,xLow_pt2,xHigh_pt2);
  ////////////////////////////////////////////////////////////////////////
  int nBins_Mjj   = 30;
  float xLow_Mjj  = 0;
  float xHigh_Mjj = 1500;

  TH1F* hZ_Mjj      = new TH1F("hZ_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  TH1F* hZMuMu_Mjj  = new TH1F("hZMuMu_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  TH1F* hW_Mjj      = new TH1F("hW_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  TH1F* hQCD_Mjj    = new TH1F("hQCD_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  TH1F* hTTbar_Mjj  = new TH1F("hTTbar_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  TH1F* hqqH115_Mjj = new TH1F("hqqH115_Mjj","; M_{jj} (GeV) ; a.u.",nBins_Mjj,xLow_Mjj,xHigh_Mjj);
  ////////////////////////////////////////////////////////////////////////
  int nBins_Deta   = 20;
  float xLow_Deta  = 0;
  float xHigh_Deta = 8;

  TH1F* hZ_Deta      = new TH1F("hZ_Deta","; #Delta#eta ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  TH1F* hZMuMu_Deta  = new TH1F("hZMuMu_Deta","; #Delta#eta ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  TH1F* hW_Deta      = new TH1F("hW_Deta","; #Delta#eta ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  TH1F* hQCD_Deta    = new TH1F("hQCD_Deta","; #Delta#eta  ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  TH1F* hTTbar_Deta  = new TH1F("hTTbar_Deta","; #Delta#eta ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  TH1F* hqqH115_Deta = new TH1F("hqqH115_Deta","; #Delta#eta ; a.u.",nBins_Deta,xLow_Deta,xHigh_Deta);
  ////////////////////////////////////////////////////////////////////////
  int nBins_eta1   = 20;
  float xLow_eta1  = -5;
  float xHigh_eta1 = 5;

  TH1F* hZ_eta1      = new TH1F("hZ_eta1","; #eta^{lead} ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  TH1F* hZMuMu_eta1  = new TH1F("hZMuMu_eta1","; #eta^{lead} ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  TH1F* hW_eta1      = new TH1F("hW_eta1","; #eta^{lead} ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  TH1F* hQCD_eta1    = new TH1F("hQCD_eta1","; #eta^{lead}  ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  TH1F* hTTbar_eta1  = new TH1F("hTTbar_eta1","; #eta^{lead} ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  TH1F* hqqH115_eta1 = new TH1F("hqqH115_eta1","; #eta^{lead} ; a.u.",nBins_eta1,xLow_eta1,xHigh_eta1);
  ////////////////////////////////////////////////////////////////////////
  int nBins_eta2   = 20;
  float xLow_eta2  = -5;
  float xHigh_eta2 = 5;

  TH1F* hZ_eta2      = new TH1F("hZ_eta2","; #eta^{sub-lead} ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  TH1F* hZMuMu_eta2  = new TH1F("hZMuMu_eta2","; #eta^{sub-lead} ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  TH1F* hW_eta2      = new TH1F("hW_eta2","; #eta^{sub-lead} ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  TH1F* hQCD_eta2    = new TH1F("hQCD_eta2","; #eta^{sub-lead}  ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  TH1F* hTTbar_eta2  = new TH1F("hTTbar_eta2","; #eta^{sub-lead} ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  TH1F* hqqH115_eta2 = new TH1F("hqqH115_eta2","; #eta^{sub-lead} ; a.u.",nBins_eta2,xLow_eta2,xHigh_eta2);
  ////////////////////////////////////////////////////////////////////////
 
 
  treeZ->Draw("pt1>>hZ_pt1","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("pt1>>hZMuMu_pt1","pt1>0 && eta1*eta2<0");
  treeW->Draw("pt1>>hW_pt1","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("pt1>>hQCD_pt1","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("pt1>>hTTbar_pt1","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("pt1>>hqqH115_pt1","pt1>0 && eta1*eta2<0");

  treeZ->Draw("pt2>>hZ_pt2","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("pt2>>hZMuMu_pt2","pt1>0 && eta1*eta2<0");
  treeW->Draw("pt2>>hW_pt2","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("pt2>>hQCD_pt2","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("pt2>>hTTbar_pt2","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("pt2>>hqqH115_pt2","pt1>0 && eta1*eta2<0");

  treeZ->Draw("Mjj>>hZ_Mjj","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("Mjj>>hZMuMu_Mjj","pt1>0 && eta1*eta2<0");
  treeW->Draw("Mjj>>hW_Mjj","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("Mjj>>hQCD_Mjj","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("Mjj>>hTTbar_Mjj","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("Mjj>>hqqH115_Mjj","pt1>0 && eta1*eta2<0");

  treeZ->Draw("Deta>>hZ_Deta","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("Deta>>hZMuMu_Deta","pt1>0 && eta1*eta2<0");
  treeW->Draw("Deta>>hW_Deta","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("Deta>>hQCD_Deta","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("Deta>>hTTbar_Deta","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("Deta>>hqqH115_Deta","pt1>0 && eta1*eta2<0");
  
  treeZ->Draw("eta1>>hZ_eta1","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("eta1>>hZMuMu_eta1","pt1>0 && eta1*eta2<0");
  treeW->Draw("eta1>>hW_eta1","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("eta1>>hQCD_eta1","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("eta1>>hTTbar_eta1","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("eta1>>hqqH115_eta1","pt1>0 && eta1*eta2<0");

  treeZ->Draw("eta2>>hZ_eta2","pt1>0 && eta1*eta2<0");
  treeZMuMu->Draw("eta2>>hZMuMu_eta2","pt1>0 && eta1*eta2<0");
  treeW->Draw("eta2>>hW_eta2","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("eta2>>hQCD_eta2","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("eta2>>hTTbar_eta2","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("eta2>>hqqH115_eta2","pt1>0 && eta1*eta2<0");


  leg->AddEntry(hZ_pt1,"Z+jets","l");
  leg->AddEntry(hZMuMu_pt1,"Z#rightarrow#mu#mu+jets","l");
  leg->AddEntry(hW_pt1,"W+jets","l");
  leg->AddEntry(hQCD_pt1,"QCD#mu","l");
  leg->AddEntry(hTTbar_pt1,"t#bar{t}","l");
  leg->AddEntry(hqqH115_pt1,"qqH(115)","l");

  hZ_pt1->SetLineColor(kRed);
  hZMuMu_pt1->SetLineColor(kRed);
  hZMuMu_pt1->SetLineStyle(kDashed);
  hW_pt1->SetLineColor(kGreen);
  hQCD_pt1->SetLineColor(kBlue);
  hTTbar_pt1->SetLineColor(kMagenta);
  hqqH115_pt1->SetLineWidth(2);

  hZ_pt2->SetLineColor(kRed);
  hZMuMu_pt2->SetLineColor(kRed);
  hZMuMu_pt2->SetLineStyle(kDashed);
  hW_pt2->SetLineColor(kGreen);
  hQCD_pt2->SetLineColor(kBlue);
  hTTbar_pt2->SetLineColor(kMagenta);
  hqqH115_pt2->SetLineWidth(2);

  hZ_Mjj->SetLineColor(kRed);
  hZMuMu_Mjj->SetLineColor(kRed);
  hZMuMu_Mjj->SetLineStyle(kDashed);
  hW_Mjj->SetLineColor(kGreen);
  hQCD_Mjj->SetLineColor(kBlue);
  hTTbar_Mjj->SetLineColor(kMagenta);
  hqqH115_Mjj->SetLineWidth(2);

  hZ_Deta->SetLineColor(kRed);
  hZMuMu_Deta->SetLineColor(kRed);
  hZMuMu_Deta->SetLineStyle(kDashed);
  hW_Deta->SetLineColor(kGreen);
  hQCD_Deta->SetLineColor(kBlue);
  hTTbar_Deta->SetLineColor(kMagenta);
  hqqH115_Deta->SetLineWidth(2);
  
  hZ_eta1->SetLineColor(kRed);
  hZMuMu_eta1->SetLineColor(kRed);
  hZMuMu_eta1->SetLineStyle(kDashed);
  hW_eta1->SetLineColor(kGreen);
  hQCD_eta1->SetLineColor(kBlue);
  hTTbar_eta1->SetLineColor(kMagenta);
  hqqH115_eta1->SetLineWidth(2);

  hZ_eta2->SetLineColor(kRed);
  hZMuMu_eta2->SetLineColor(kRed);
  hZMuMu_eta2->SetLineStyle(kDashed);
  hW_eta2->SetLineColor(kGreen);
  hQCD_eta2->SetLineColor(kBlue);
  hTTbar_eta2->SetLineColor(kMagenta);
  hqqH115_eta2->SetLineWidth(2);



  c1->Divide(3,2);

  c1->cd(1);
  hZ_pt1->DrawNormalized("HIST");
  hZMuMu_pt1->DrawNormalized("HISTSAME");
  hW_pt1->DrawNormalized("HISTSAME");
  hQCD_pt1->DrawNormalized("HISTSAME");
  hTTbar_pt1->DrawNormalized("HISTSAME");
  hqqH115_pt1->DrawNormalized("HISTSAME");
  leg->Draw();
  
  c1->cd(2);
  hZ_pt2->DrawNormalized("HIST");
  hZMuMu_pt2->DrawNormalized("HISTSAME");
  hW_pt2->DrawNormalized("HISTSAME");
  hQCD_pt2->DrawNormalized("HISTSAME");
  hTTbar_pt2->DrawNormalized("HISTSAME");
  hqqH115_pt2->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(3);
  hZ_Mjj->DrawNormalized("HIST");
  hZMuMu_Mjj->DrawNormalized("HISTSAME");
  hW_Mjj->DrawNormalized("HISTSAME");
  hQCD_Mjj->DrawNormalized("HISTSAME");
  hTTbar_Mjj->DrawNormalized("HISTSAME");
  hqqH115_Mjj->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(4);
  hqqH115_Deta->DrawNormalized("HIST");
  hZ_Deta->DrawNormalized("HISTSAME");
  hZMuMu_Deta->DrawNormalized("HISTSAME");
  hW_Deta->DrawNormalized("HISTSAME");
  hQCD_Deta->DrawNormalized("HISTSAME");
  hTTbar_Deta->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(5);
  hZ_eta1->DrawNormalized("HIST");
  hTTbar_eta1->DrawNormalized("HISTSAME");
  hZMuMu_eta1->DrawNormalized("HISTSAME");
  hW_eta1->DrawNormalized("HISTSAME");
  hQCD_eta1->DrawNormalized("HISTSAME");
  hqqH115_eta1->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(6);
  hZ_eta2->DrawNormalized("HIST");
  hTTbar_eta2->DrawNormalized("HISTSAME");
  hZMuMu_eta2->DrawNormalized("HISTSAME");
  hW_eta2->DrawNormalized("HISTSAME");
  hQCD_eta2->DrawNormalized("HISTSAME");
  hqqH115_eta2->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->Update();

  c1->Draw();
 
}
