#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>
#include <cstring>

#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TGraphPainter.h"
#include "TMultiGraph.h"
#include "TTree.h"

using namespace std;


void plot( TString stream  = "MuTau", 
	   TString channel = "SM0",
	   TString legend  = "#tau_{#mu}#tau_{h}",
	   Float_t xMax_   = 50){

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  
  TLegend* leg = new TLegend(0.14,0.60,0.41,0.85,NULL,"brNDC");
  leg->SetBorderSize(0);
  leg->SetTextSize(0.03);
  leg->SetFillColor(0);


  float X[]        = {110,115,120,125,130,135,140,145};

  float obsY[]     = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expY[]     = {0.,0.,0.,0.,0.,0.,0.,0.};

  float expX1sL[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expX1sH[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expY1sL[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expY1sH[]  = {0.,0.,0.,0.,0.,0.,0.,0.};

  float expX2sL[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expX2sH[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expY2sL[]  = {0.,0.,0.,0.,0.,0.,0.,0.};
  float expY2sH[]  = {0.,0.,0.,0.,0.,0.,0.,0.};

  int nMassPoints = 8;

  for(unsigned int i = 0 ; i < nMassPoints; i++){

    TFile f(Form("higgsCombine%s%s.Asymptotic.mH%.0f.root",stream.Data(),channel.Data(),X[i]),"READ");
    if(f.IsZombie()){
      cout << "Cannot open file for " << string(channel.Data()) << " and mass " << X[i] << endl;
      continue;
    }

    Double_t r;
    TTree* limit = (TTree*)f.Get("limit");
    limit->SetBranchAddress("limit",&r);

    for(int k = 0 ; k< limit->GetEntries() ; k++){
      limit->GetEntry(k);
      if(k==0) expY2sL[i] = r;
      if(k==1) expY1sL[i] = r;
      if(k==2) expY[i]    = r;
      if(k==3) expY1sH[i] = r;
      if(k==4) expY2sH[i] = r;
      if(k==5) obsY[i]    = r;
    }

  }
 


  for(int i = 0 ; i < nMassPoints ; i++){
    expY1sH[i] = TMath::Abs(expY1sH[i]-expY[i]);
    expY1sL[i] = TMath::Abs(expY1sL[i]-expY[i]);
    expY2sH[i] = TMath::Abs(expY2sH[i]-expY[i]);
    expY2sL[i] = TMath::Abs(expY2sL[i]-expY[i]);

    cout << "Mass " << X[i] << " => " << expY2sL[i] << ", " << expY1sL[i] << ", " << expY1sH[i] << ", "
	 << expY2sH[i] << ", " << endl;
  }

  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("");

  TGraphAsymmErrors* expected = new TGraphAsymmErrors(nMassPoints, X, expY, expX1sL ,expX1sL , expX1sL, expX1sL);
  TGraphAsymmErrors* oneSigma = new TGraphAsymmErrors(nMassPoints, X, expY, expX1sL, expX1sL,  expY1sL, expY1sH);
  TGraphAsymmErrors* twoSigma = new TGraphAsymmErrors(nMassPoints, X, expY, expX2sL, expX2sL,  expY2sL, expY2sH);
  TGraphAsymmErrors* observed = new TGraphAsymmErrors(nMassPoints, X, obsY, expX1sL ,expX1sL , expX1sL, expX1sL);

 
  oneSigma->SetMarkerColor(kBlack);
  oneSigma->SetMarkerStyle(kFullCircle);
  oneSigma->SetFillColor(kGreen);
  oneSigma->SetFillStyle(1001);

  twoSigma->SetMarkerColor(kBlack);
  twoSigma->SetMarkerStyle(kFullCircle);
  twoSigma->SetFillColor(kYellow);
  twoSigma->SetFillStyle(1001);

  expected->SetMarkerColor(kBlack);
  expected->SetMarkerStyle(kFullCircle);
  expected->SetMarkerSize(1.5);
  expected->SetLineColor(kBlack);
  expected->SetLineWidth(2);

  observed->SetMarkerColor(kBlue);
  observed->SetMarkerStyle(1);
  observed->SetLineColor(kBlue);
  observed->SetLineWidth(4);

  mg->Add(twoSigma);
  mg->Add(oneSigma);
  mg->Add(expected);
  mg->Add(observed);

  mg->Draw("ALP3");

  c1->cd();
  gPad->Modified();
  mg->GetXaxis()->SetLimits(110,145);
  mg->GetYaxis()->SetTitleOffset(0.97);
  mg->SetMinimum(0.);
  mg->SetMaximum(xMax_);
  mg->GetXaxis()->SetTitle("m_{H} (GeV)");
  mg->GetYaxis()->SetTitle("#sigma X BR(H#rightarrow#tau#tau)_{95% CLs}/#sigma_{SM}");

  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=7 TeV}{4.6 fb^{-1}, %s}",legend.Data()));

  leg->AddEntry(expected,"Expected","P");
  leg->AddEntry(observed,"Observed","L");

  leg->Draw();

  TF1 *line = new TF1("line","1",100,150);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);

  line->Draw("SAME");

  gPad->SaveAs(Form("limit_%s_%s.png",stream.Data(), channel.Data()));
  gPad->SaveAs(Form("limit_%s_%s.pdf",stream.Data(), channel.Data()));

}


void plotAll(){

  // MuTau : SM0
  plot( "MuTau","SM0","#tau_{#mu}#tau_{h} SM0",50);
  // MuTau : SM1
  plot( "MuTau","SM1","#tau_{#mu}#tau_{h} SM1",60);
  // MuTau : SM2
  plot( "MuTau","SM2","#tau_{#mu}#tau_{h} SM2",50);
  // MuTau : Combined
  plot( "MuTau","SM","#tau_{#mu}#tau_{h} SM",20);

  // ElecTau : SM0
  plot( "ElecTau","SM0","#tau_{e}#tau_{h} SM0",100);
  // ElecTau : SM1
  plot( "ElecTau","SM1","#tau_{e}#tau_{h} SM1",60);
  // ElecTau : SM2
  plot( "ElecTau","SM2","#tau_{e}#tau_{h} SM2",50);
  // ElecTau : SM
  plot( "ElecTau","SM","#tau_{e}#tau_{h} SM",30);

  // MuTau+ElecTau: SM
  plot( "Combined","SM","#tau_{e}#tau_{h}, #tau_{#mu}#tau_{h} SM",15);


}
