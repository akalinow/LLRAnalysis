#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "THStack.h"
#include "TBenchmark.h"
#include "TGraph.h"
#include "TVectorT.h"
#include "TMultiGraph.h"
#include "TBranch.h"

#include <vector>


using namespace std;



TGraph* rescalePlot(TH1F* h_=0, float scaleX_=1, float scaleY_=1){

  float vx[h_->GetNbinsX()];
  float vy[h_->GetNbinsX()];

  for(int bin=1; bin < h_->GetNbinsX(); bin++){
    vx[bin-1]= (bin*h_->GetBinWidth(1))*scaleX_;
    vy[bin-1]= 1-(-h_->GetBinContent(bin)+1)*scaleY_;
  }

  TGraph* graph = new TGraph(h_->GetNbinsX(),vx,vy);
  return graph;
}


void plot(){


  TString fSignalName        = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleVbf.root";
  TString fBackgroundName    = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets.root";

  TFile *fSignal(0); 
  TFile *fBackground(0); 

  fSignal = TFile::Open( fSignalName ); 
  fBackground = TFile::Open( fBackgroundName ); 

  if(!fSignal || !fBackground){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";
  TTree *signalPtOrd = (TTree*)fSignal->Get(tree);
  float signalPresectionPtOrd = (float)signalPtOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)signalPtOrd->GetEntries();
  TTree *backgroundPtOrd = (TTree*)fBackground->Get(tree);
  float backgroundPresectionPtOrd = (float)backgroundPtOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)backgroundPtOrd->GetEntries();
  tree = "outTreePtEtaOrd";
  TTree *signalPtEtaOrd = (TTree*)fSignal->Get(tree);
  float signalPresectionPtEtaOrd = (float)signalPtEtaOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)signalPtEtaOrd->GetEntries();
  TTree *backgroundPtEtaOrd = (TTree*)fBackground->Get(tree);
  float backgroundPresectionPtEtaOrd = (float)backgroundPtEtaOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)backgroundPtEtaOrd->GetEntries();
  tree = "outTreePtMjjOrd";
  TTree *signalPtMjjOrd = (TTree*)fSignal->Get(tree);
  float signalPresectionPtMjjOrd = (float)signalPtMjjOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)signalPtMjjOrd->GetEntries();
  TTree *backgroundPtMjjOrd = (TTree*)fBackground->Get(tree);
  float backgroundPresectionPtMjjOrd = (float)backgroundPtMjjOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)backgroundPtMjjOrd->GetEntries();
  tree = "outTreePtDEtaOrd";
  TTree *signalPtDEtaOrd = (TTree*)fSignal->Get(tree);
  float signalPresectionPtDEtaOrd = (float)signalPtDEtaOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)signalPtDEtaOrd->GetEntries();
  TTree *backgroundPtDEtaOrd = (TTree*)fBackground->Get(tree);
  float backgroundPresectionPtDEtaOrd = (float)backgroundPtDEtaOrd->GetEntries("pt1>0 && abs(eta1*eta2)/eta1/eta2<0")/(float)backgroundPtDEtaOrd->GetEntries();



  TFile* fPtEtaOrd = new TFile("tmvaRoot/TMVAPtEtaOrd.root");
  TFile* fPtDEtaOrd = new TFile("tmvaRoot/TMVAPtDEtaOrd.root");
  TFile* fPtMjjOrd = new TFile("tmvaRoot/TMVAPtMjjOrd.root");
  TFile* fPtOrd = new TFile("tmvaRoot/TMVAPtOrd.root");
  TFile* fPtOrdBDT = new TFile("tmvaRoot/TMVAPtOrdBDT.root");

  TH1F* PtEtaOrd = (TH1F*)fPtEtaOrd->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtDEtaOrd = (TH1F*)fPtDEtaOrd->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtMjjOrd = (TH1F*)fPtMjjOrd->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtOrd    = (TH1F*)fPtOrd->Get("Method_Cuts/Cuts/MVA_Cuts_rejBvsS");
  TH1F* PtOrdBDT = (TH1F*)fPtOrdBDT->Get("Method_BDT/BDT/MVA_BDT_rejBvsS");


  TGraph* graphPtOrd = rescalePlot(PtOrd,signalPresectionPtOrd,backgroundPresectionPtOrd);
  graphPtOrd->SetMarkerStyle(kFullCircle);
  graphPtOrd->SetMarkerSize(0.8);
  graphPtOrd->SetMarkerColor(kMagenta);
  TGraph* graphPtEtaOrd = rescalePlot(PtEtaOrd,signalPresectionPtEtaOrd,backgroundPresectionPtEtaOrd);
  graphPtEtaOrd->SetMarkerStyle(kFullStar);
  graphPtEtaOrd->SetMarkerSize(0.8);
  graphPtEtaOrd->SetMarkerColor(kRed);
  TGraph* graphPtMjjOrd = rescalePlot(PtMjjOrd,signalPresectionPtMjjOrd,backgroundPresectionPtMjjOrd);
  graphPtMjjOrd->SetMarkerStyle(kFullStar);
  graphPtMjjOrd->SetMarkerSize(0.8);
  graphPtMjjOrd->SetMarkerColor(kBlue);
  TGraph* graphPtDEtaOrd = rescalePlot(PtDEtaOrd,signalPresectionPtDEtaOrd,backgroundPresectionPtDEtaOrd);
  graphPtDEtaOrd->SetMarkerStyle(kFullSquare);
  graphPtDEtaOrd->SetMarkerSize(0.8);
  graphPtDEtaOrd->SetMarkerColor(kYellow);

  PtEtaOrd->SetLineColor(kRed);
  PtEtaOrd->SetLineStyle(kSolid);
  PtEtaOrd->SetLineWidth(2);

  PtDEtaOrd->SetLineColor(kYellow);
  PtDEtaOrd->SetLineStyle(kDashed);
  PtDEtaOrd->SetLineWidth(3);

  PtMjjOrd->SetLineColor(kBlue);
  PtMjjOrd->SetLineStyle(kDashed);
  PtMjjOrd->SetLineWidth(4);

  PtOrd->SetLineColor(kMagenta);
  PtOrd->SetLineStyle(kDotted);
  PtOrd->SetLineWidth(3);
  PtOrd->SetXTitle("Efficiency on qqH(115)");
  PtOrd->SetYTitle("Rejection of Z+jets");

  PtOrdBDT->SetLineColor(kGreen);
  PtOrdBDT->SetLineStyle(kDotted);
  PtOrdBDT->SetLineWidth(3);

  TH1F* hMaster = (TH1F*)PtOrd->Clone("hMaster");
  hMaster->Reset();
  //hMaster->Draw();
  
  PtOrd->Draw("HIST");
  PtEtaOrd->Draw("HISTSAME");
  PtDEtaOrd->Draw("HISTSAME");  
  PtMjjOrd->Draw("HISTSAME");
  PtOrdBDT->Draw("HISTSAME");
  
  /*
    graphPtOrd->Draw("P");
    graphPtEtaOrd->Draw("P");
    graphPtMjjOrd->Draw("P");
    graphPtDEtaOrd->Draw("P");
  */

  TLegend* leg = new TLegend(0.20,0.20,0.45,0.50,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("#splitline{Efficiency of passing the jet-tag}{after finding two tag jets p_{T}>15,20 in opposite hemispheres}");
  leg->AddEntry(PtEtaOrd,"p_{T}-#eta ordered","l");
  leg->AddEntry(PtDEtaOrd,"p_{T}-D#eta ordered","l");
  leg->AddEntry(PtMjjOrd,"p_{T}-Mjj ordered","l");
  leg->AddEntry(PtOrd,"p_{T} ordered","l");
  leg->AddEntry(PtOrdBDT,"p_{T} ordered BDT","l");
  leg->Draw();

}


