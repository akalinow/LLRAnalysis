

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


  TFile* fZ      = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleDYJets-madgraph-50-PU-L_ext.root");
  TFile* fW      = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleWJets-madgraph-PU-L_ext.root");
  TFile* fQCD    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleQCD-pythia-PU-L_ext.root");
  TFile* fTTbar  = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleTT-madgraph-PU-L_ext.root");
  TFile* fqqH115 = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleVBFH115-PU-L_ext.root");

  TString ordering = "outTreePtOrd";

  TTree* treeZ     = (TTree*)fZ->Get(ordering);
  TTree* treeW     = (TTree*)fW->Get(ordering);
  TTree* treeQCD   = (TTree*)fQCD->Get(ordering);
  TTree* treeTTbar = (TTree*)fTTbar->Get(ordering);
  TTree* treeqqH115= (TTree*)fqqH115->Get(ordering);

  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauVisPt   = 20;
  float xLow_diTauVisPt  = 10;
  float xHigh_diTauVisPt = 250;

  TH1F* hZ_diTauVisPt      = new TH1F("hZ_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);
  TH1F* hZMuMu_diTauVisPt  = new TH1F("hZMuMu_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);
  TH1F* hW_diTauVisPt      = new TH1F("hW_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);
  TH1F* hQCD_diTauVisPt    = new TH1F("hQCD_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);
  TH1F* hTTbar_diTauVisPt  = new TH1F("hTTbar_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);
  TH1F* hqqH115_diTauVisPt = new TH1F("hqqH115_diTauVisPt"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisPt,xLow_diTauVisPt,xHigh_diTauVisPt);

  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauSVFitPt   = 20;
  float xLow_diTauSVFitPt  = 10;
  float xHigh_diTauSVFitPt = 250;

  TH1F* hZ_diTauSVFitPt      = new TH1F("hZ_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hZMuMu_diTauSVFitPt  = new TH1F("hZMuMu_diTauSVFitPt"," ; SVFitp_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hW_diTauSVFitPt      = new TH1F("hW_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hQCD_diTauSVFitPt    = new TH1F("hQCD_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hTTbar_diTauSVFitPt  = new TH1F("hTTbar_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hqqH115_diTauSVFitPt = new TH1F("hqqH115_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauCAPt   = 20;
  float xLow_diTauCAPt  = 10;
  float xHigh_diTauCAPt = 250;

  TH1F* hZ_diTauCAPt      = new TH1F("hZ_diTauCAPt"," ;Coll. Approx  p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hZMuMu_diTauCAPt  = new TH1F("hZMuMu_diTauCAPt"," ; Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hW_diTauCAPt      = new TH1F("hW_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hQCD_diTauCAPt    = new TH1F("hQCD_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hTTbar_diTauCAPt  = new TH1F("hTTbar_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hqqH115_diTauCAPt = new TH1F("hqqH115_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauVisEta   = 20;
  float xLow_diTauVisEta  = -5;
  float xHigh_diTauVisEta = 5;

  TH1F* hZ_diTauVisEta      = new TH1F("hZ_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hZMuMu_diTauVisEta  = new TH1F("hZMuMu_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hW_diTauVisEta      = new TH1F("hW_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hQCD_diTauVisEta    = new TH1F("hQCD_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hTTbar_diTauVisEta  = new TH1F("hTTbar_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hqqH115_diTauVisEta = new TH1F("hqqH115_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);

  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauSVFitEta   = 20;
  float xLow_diTauSVFitEta  = -5;
  float xHigh_diTauSVFitEta = 5;

  TH1F* hZ_diTauSVFitEta      = new TH1F("hZ_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hZMuMu_diTauSVFitEta  = new TH1F("hZMuMu_diTauSVFitEta"," ; SVFitp_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hW_diTauSVFitEta      = new TH1F("hW_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hQCD_diTauSVFitEta    = new TH1F("hQCD_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hTTbar_diTauSVFitEta  = new TH1F("hTTbar_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hqqH115_diTauSVFitEta = new TH1F("hqqH115_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauCAEta   = 20;
  float xLow_diTauCAEta  = -5;
  float xHigh_diTauCAEta = 5;

  TH1F* hZ_diTauCAEta      = new TH1F("hZ_diTauCAEta"," ;Coll. Approx  p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hZMuMu_diTauCAEta  = new TH1F("hZMuMu_diTauCAEta"," ; Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hW_diTauCAEta      = new TH1F("hW_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hQCD_diTauCAEta    = new TH1F("hQCD_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hTTbar_diTauCAEta  = new TH1F("hTTbar_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hqqH115_diTauCAEta = new TH1F("hqqH115_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
 



 

  treeZ->Draw("diTauVisPt>>hZ_diTauVisPt","pt1>0 && eta1*eta2<0");
  treeW->Draw("diTauVisPt>>hW_diTauVisPt","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("diTauVisPt>>hQCD_diTauVisPt","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("diTauVisPt>>hTTbar_diTauVisPt","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("diTauVisPt>>hqqH115_diTauVisPt","pt1>0 && eta1*eta2<0");

  treeZ->Draw("diTauSVFitPt>>hZ_diTauSVFitPt","pt1>0 && eta1*eta2<0");
  treeW->Draw("diTauSVFitPt>>hW_diTauSVFitPt","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("diTauSVFitPt>>hQCD_diTauSVFitPt","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("diTauSVFitPt>>hTTbar_diTauSVFitPt","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("diTauSVFitPt>>hqqH115_diTauSVFitPt","pt1>0 && eta1*eta2<0");

  treeZ->Draw("diTauCAPt>>hZ_diTauCAPt","pt1>0 && eta1*eta2<0");
  treeW->Draw("diTauCAPt>>hW_diTauCAPt","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("diTauCAPt>>hQCD_diTauCAPt","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("diTauCAPt>>hTTbar_diTauCAPt","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("diTauCAPt>>hqqH115_diTauCAPt","pt1>0 && eta1*eta2<0");

  treeZ->Draw("(diTauVisEta-(eta1+eta2)/2)>>hZ_diTauVisEta","pt1>0 && eta1*eta2<0");
  treeW->Draw("(diTauVisEta-(eta1+eta2)/2)>>hW_diTauVisEta","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("(diTauVisEta-(eta1+eta2)/2)>>hQCD_diTauVisEta","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("(diTauVisEta-(eta1+eta2)/2)>>hTTbar_diTauVisEta","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("(diTauVisEta-(eta1+eta2)/2)>>hqqH115_diTauVisEta","pt1>0 && eta1*eta2<0");

  treeZ->Draw("(diTauSVFitEta-(eta1+eta2)/2)>>hZ_diTauSVFitEta","pt1>0 && eta1*eta2<0");
  treeW->Draw("(diTauSVFitEta-(eta1+eta2)/2)>>hW_diTauSVFitEta","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("(diTauSVFitEta-(eta1+eta2)/2)>>hQCD_diTauSVFitEta","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("(diTauSVFitEta-(eta1+eta2)/2)>>hTTbar_diTauSVFitEta","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("(diTauSVFitEta-(eta1+eta2)/2)>>hqqH115_diTauSVFitEta","pt1>0 && eta1*eta2<0");

  treeZ->Draw("(diTauCAEta-(eta1+eta2)/2)>>hZ_diTauCAEta","pt1>0 && eta1*eta2<0");
  treeW->Draw("(diTauCAEta-(eta1+eta2)/2)>>hW_diTauCAEta","pt1>0 && eta1*eta2<0");
  treeQCD->Draw("(diTauCAEta-(eta1+eta2)/2)>>hQCD_diTauCAEta","pt1>0 && eta1*eta2<0");
  treeTTbar->Draw("(diTauCAEta-(eta1+eta2)/2)>>hTTbar_diTauCAEta","pt1>0 && eta1*eta2<0");
  treeqqH115->Draw("(diTauCAEta-(eta1+eta2)/2)>>hqqH115_diTauCAEta","pt1>0 && eta1*eta2<0");

 


  leg->AddEntry(hZ_diTauVisPt,"Z+jets","l");
  leg->AddEntry(hW_diTauVisPt,"W+jets","l");
  leg->AddEntry(hQCD_diTauVisPt,"QCD#mu","l");
  leg->AddEntry(hTTbar_diTauVisPt,"t#bar{t}","l");
  leg->AddEntry(hqqH115_diTauVisPt,"qqH(115)","l");

 
  hZ_diTauVisPt->SetLineColor(kRed);
  hW_diTauVisPt->SetLineColor(kGreen);
  hQCD_diTauVisPt->SetLineColor(kBlue);
  hTTbar_diTauVisPt->SetLineColor(kMagenta);
  hqqH115_diTauVisPt->SetLineWidth(2);

  hZ_diTauSVFitPt->SetLineColor(kRed);
  hW_diTauSVFitPt->SetLineColor(kGreen);
  hQCD_diTauSVFitPt->SetLineColor(kBlue);
  hTTbar_diTauSVFitPt->SetLineColor(kMagenta);
  hqqH115_diTauSVFitPt->SetLineWidth(2);

  hZ_diTauCAPt->SetLineColor(kRed);
  hW_diTauCAPt->SetLineColor(kGreen);
  hQCD_diTauCAPt->SetLineColor(kBlue);
  hTTbar_diTauCAPt->SetLineColor(kMagenta);
  hqqH115_diTauCAPt->SetLineWidth(2);

  hZ_diTauVisEta->SetLineColor(kRed);
  hW_diTauVisEta->SetLineColor(kGreen);
  hQCD_diTauVisEta->SetLineColor(kBlue);
  hTTbar_diTauVisEta->SetLineColor(kMagenta);
  hqqH115_diTauVisEta->SetLineWidth(2);

  hZ_diTauSVFitEta->SetLineColor(kRed);
  hW_diTauSVFitEta->SetLineColor(kGreen);
  hQCD_diTauSVFitEta->SetLineColor(kBlue);
  hTTbar_diTauSVFitEta->SetLineColor(kMagenta);
  hqqH115_diTauSVFitEta->SetLineWidth(2);

  hZ_diTauCAEta->SetLineColor(kRed);
  hW_diTauCAEta->SetLineColor(kGreen);
  hQCD_diTauCAEta->SetLineColor(kBlue);
  hTTbar_diTauCAEta->SetLineColor(kMagenta);
  hqqH115_diTauCAEta->SetLineWidth(2);



  c1->Divide(3,2);

  c1->cd(1);
  hQCD_diTauVisPt->DrawNormalized("HIST");
  hZ_diTauVisPt->DrawNormalized("HISTSAME");
  hW_diTauVisPt->DrawNormalized("HISTSAME");
  hTTbar_diTauVisPt->DrawNormalized("HISTSAME");
  hqqH115_diTauVisPt->DrawNormalized("HISTSAME");
  leg->Draw();
  
  c1->cd(2);
  hQCD_diTauSVFitPt->DrawNormalized("HIST");
  hZ_diTauSVFitPt->DrawNormalized("HISTSAME");
  hW_diTauSVFitPt->DrawNormalized("HISTSAME");
  hTTbar_diTauSVFitPt->DrawNormalized("HISTSAME");
  hqqH115_diTauSVFitPt->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(3);
  hQCD_diTauCAPt->DrawNormalized("HIST");
  hZ_diTauCAPt->DrawNormalized("HISTSAME");
  hW_diTauCAPt->DrawNormalized("HISTSAME");
  hTTbar_diTauCAPt->DrawNormalized("HISTSAME");
  hqqH115_diTauCAPt->DrawNormalized("HISTSAME");
  leg->Draw();
 
  c1->cd(4);
  hqqH115_diTauVisEta->DrawNormalized("HIST");
  hZ_diTauVisEta->DrawNormalized("HISTSAME");
  hQCD_diTauVisEta->DrawNormalized("HISTSAME");
  hW_diTauVisEta->DrawNormalized("HISTSAME");
  hTTbar_diTauVisEta->DrawNormalized("HISTSAME");
  leg->Draw();
  
  c1->cd(5);
  hqqH115_diTauSVFitEta->DrawNormalized("HIST");
  hZ_diTauSVFitEta->DrawNormalized("HISTSAME");
  hQCD_diTauSVFitEta->DrawNormalized("HISTSAME");
  hW_diTauSVFitEta->DrawNormalized("HISTSAME");
  hTTbar_diTauSVFitEta->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(6);
  hTTbar_diTauCAEta->DrawNormalized("HIST");
  hqqH115_diTauCAEta->DrawNormalized("HISTSAME");
  hZ_diTauCAEta->DrawNormalized("HISTSAME");
  hQCD_diTauCAEta->DrawNormalized("HISTSAME");
  hW_diTauCAEta->DrawNormalized("HISTSAME");
  leg->Draw();
 

  c1->Update();

  c1->Draw();
 
}
