#include "FWCore/FWLite/interface/AutoLibraryLoader.h"


#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <map>
#include <string>

#include "TChain.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TF1.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"
#include "TArrayF.h"
#include "TStyle.h"
#include "TGraph.h"

#include "TauAnalysis/CandidateTools/interface/SVfitVMlineShapeIntegral.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"

using namespace std;

void lineShape(){

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetGrid(0,0);
  c1->SetTicky();
  c1->SetObjectStat(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetOptStat(0000000);
  gStyle->SetOptFit(0111);
  //gStyle->SetOptTitle(0);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetPalette(1);

  //cout << (*lmLineShape)(atoi(polarization),atof(z)) << endl;

  fwlite::TFileService fs = fwlite::TFileService("shapes.root");


  Double_t x[100];
  Double_t yMinus[100];
  Double_t yPlus [100];
  Double_t yIntegMinus[100];
  Double_t yIntegPlus [100];

  cout << "Now doing for Rho L" << endl;
  TH1F* hRhoLMinus = new TH1F("hRhoLMinus","#tau_{L}^{-1} #rightarrow #rho_{L} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  TH1F* hRhoLPlus  = new TH1F("hRhoLPlus", "#tau_{R}^{-1} #rightarrow #rho_{L} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  SVfitVMlineShapeIntegral* lmLineShapeRhoL = 
    new SVfitVMlineShapeIntegral(SVfitVMlineShapeIntegrand::kVMrho, 
				 SVfitVMlineShapeIntegrand::kVMlongitudinalPol,
				 true);
  for(int i = 1; i <= 100; i++){
    double valueMinus = (*lmLineShapeRhoL)(-1,i*0.01); 
    double valuePlus  = (*lmLineShapeRhoL)(+1,i*0.01);
    hRhoLMinus->SetBinContent( i, valueMinus );
    hRhoLPlus->SetBinContent(  i, valuePlus);
    x[i-1] = i*0.01;
    yMinus[i-1] = valueMinus;
    yPlus[i-1]  = valuePlus;
  }
  for(int i = 1; i <= hRhoLMinus->GetNbinsX(); i++){
    yIntegMinus[i-1] = hRhoLMinus->Integral(1, i,"width");
    yIntegPlus[i-1] = hRhoLPlus->Integral(1, i,"width");
  }

  c1->cd();
  TGraph* gRhoLMinus = new TGraph(100, x, yMinus);
  gRhoLMinus->SetName("gRhoLMinus");
  gDirectory->Append(gRhoLMinus);
  TGraph* gRhoLPlus = new TGraph(100,  x, yPlus);
  gRhoLPlus->SetName("gRhoLPlus");
  gDirectory->Append(gRhoLPlus);
  TGraph* gNormRhoLMinus = new TGraph(100, x, yIntegMinus);
  gNormRhoLMinus->SetName("gNormRhoLMinus");
  gDirectory->Append(gNormRhoLMinus);
  TGraph* gNormRhoLPlus = new TGraph(100,  x, yIntegPlus);
  gNormRhoLPlus->SetName("gNormRhoLPlus");
  gDirectory->Append(gNormRhoLPlus);

  hRhoLMinus->Draw();
  c1->SaveAs("hRhoLMinus.pdf");
  hRhoLPlus->Draw();
  c1->SaveAs("hRhoLPlus.pdf");
  delete lmLineShapeRhoL;


  cout << "Now doing for Rho T" << endl;
  TH1F* hRhoTMinus = new TH1F("hRhoTMinus","#tau_{L}^{-1} #rightarrow #rho_{T} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  TH1F* hRhoTPlus  = new TH1F("hRhoTPlus", "#tau_{R}^{-1} #rightarrow #rho_{T} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  SVfitVMlineShapeIntegral* lmLineShapeRhoT = 
    new SVfitVMlineShapeIntegral(SVfitVMlineShapeIntegrand::kVMrho, 
				 SVfitVMlineShapeIntegrand::kVMtransversePol,
				 true);
  for(int i = 1; i <= 100; i++){
    double valueMinus = (*lmLineShapeRhoT)(-1,i*0.01); 
    double valuePlus  = (*lmLineShapeRhoT)(+1,i*0.01);
    hRhoTMinus->SetBinContent( i, valueMinus);
    hRhoTPlus->SetBinContent(  i, valuePlus);
    x[i-1] = i*0.01;
    yMinus[i-1] = valueMinus;
    yPlus[i-1]  = valuePlus;
  }
  for(int i = 1; i <= hRhoTMinus->GetNbinsX(); i++){
    yIntegMinus[i-1] = hRhoTMinus->Integral(1, i,"width");
    yIntegPlus[i-1]  = hRhoTPlus->Integral(1, i,"width");
  }

  c1->cd();
  TGraph* gRhoTMinus = new TGraph(100, x, yMinus);
  gRhoTMinus->SetName("gRhoTMinus");
  gDirectory->Append(gRhoTMinus);
  TGraph* gRhoTPlus = new TGraph(100,  x, yPlus);
  gRhoTPlus->SetName("gRhoTPlus");
  gDirectory->Append(gRhoTPlus);
  TGraph* gNormRhoTMinus = new TGraph(100, x, yIntegMinus);
  gNormRhoTMinus->SetName("gNormRhoTMinus");
  gDirectory->Append(gNormRhoTMinus);
  TGraph* gNormRhoTPlus = new TGraph(100,  x, yIntegPlus);
  gNormRhoTPlus->SetName("gNormRhoTPlus");
  gDirectory->Append(gNormRhoTPlus);

  hRhoTMinus->Draw();
  c1->SaveAs("hRhoTMinus.pdf");
  hRhoTPlus->Draw();
  c1->SaveAs("hRhoTPlus.pdf");
  delete lmLineShapeRhoT;

  cout << "Now doing for a1 L" << endl;
  TH1F* hA1LMinus = new TH1F("hA1LMinus","#tau_{L}^{-1} #rightarrow a_{1}_{L} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  TH1F* hA1LPlus  = new TH1F("hA1LPlus", "#tau_{R}^{-1} #rightarrow a_{1}_{L} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  SVfitVMlineShapeIntegral* lmLineShapeA1L = 
    new SVfitVMlineShapeIntegral(SVfitVMlineShapeIntegrand::kVMa1Charged, 
				 SVfitVMlineShapeIntegrand::kVMlongitudinalPol,
				 true);
  for(int i = 1; i <= 100; i++){
    double valueMinus = (*lmLineShapeA1L)(-1,i*0.01); 
    double valuePlus  = (*lmLineShapeA1L)(+1,i*0.01);
    hA1LMinus->SetBinContent( i, valueMinus);
    hA1LPlus->SetBinContent(  i, valuePlus);
    x[i-1] = i*0.01;
    yMinus[i-1] = valueMinus;
    yPlus[i-1]  = valuePlus;
  }
  for(int i = 1; i <= hA1LMinus->GetNbinsX(); i++){
    yIntegMinus[i-1] = hA1LMinus->Integral(1, i,"width");
    yIntegPlus[i-1]  = hA1LPlus->Integral(1, i,"width");
  }

  c1->cd();
  TGraph* gA1LMinus = new TGraph(100, x, yMinus);
  gA1LMinus->SetName("gA1LMinus");
  gDirectory->Append(gA1LMinus);
  TGraph* gA1LPlus = new TGraph(100,  x, yPlus);
  gA1LPlus->SetName("gA1LPlus");
  gDirectory->Append(gA1LPlus);
  TGraph* gNormA1LMinus = new TGraph(100, x, yIntegMinus);
  gNormA1LMinus->SetName("gNormA1LMinus");
  gDirectory->Append(gNormA1LMinus);
  TGraph* gNormA1LPlus = new TGraph(100,  x, yIntegPlus);
  gNormA1LPlus->SetName("gNormA1LPlus");
  gDirectory->Append(gNormA1LPlus);

  hA1LMinus->Draw();
  c1->SaveAs("hA1LMinus.pdf");
  hA1LPlus->Draw();
  c1->SaveAs("hA1LPlus.pdf");
  delete lmLineShapeA1L;

  cout << "Now doing for a1 T" << endl;
  TH1F* hA1TMinus = new TH1F("hA1TMinus","#tau_{L}^{-1} #rightarrow a_{1}_{T} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  TH1F* hA1TPlus  = new TH1F("hA1TPlus", "#tau_{R}^{-1} #rightarrow a_{1}_{T} ; x_{h} ; d#Gamma/dx_{h}",100,0,1);
  SVfitVMlineShapeIntegral* lmLineShapeA1T = 
    new SVfitVMlineShapeIntegral(SVfitVMlineShapeIntegrand::kVMa1Charged, 
				 SVfitVMlineShapeIntegrand::kVMtransversePol,
				 true);
  for(int i = 1; i <= 100; i++){
    double valueMinus = (*lmLineShapeA1T)(-1,i*0.01); 
    double valuePlus  = (*lmLineShapeA1T)(+1,i*0.01);
    hA1TMinus->SetBinContent( i, valueMinus);
    hA1TPlus->SetBinContent(  i, valuePlus);
    x[i-1] = i*0.01;
    yMinus[i-1] = valueMinus;
    yPlus[i-1]  = valuePlus;
  }
  for(int i = 1; i <= hA1TMinus->GetNbinsX(); i++){
    yIntegMinus[i-1] = hA1TMinus->Integral(1, i,"width");
    yIntegPlus[i-1]  = hA1TPlus->Integral(1, i,"width");
  }

  c1->cd();
  TGraph* gA1TMinus = new TGraph(100, x, yMinus);
  gA1TMinus->SetName("gA1TMinus");
  gDirectory->Append(gA1TMinus);
  TGraph* gA1TPlus = new TGraph(100,  x, yPlus);
  gA1TPlus->SetName("gA1TPlus");
  gDirectory->Append(gA1TPlus);
  TGraph* gNormA1TMinus = new TGraph(100, x, yIntegMinus);
  gNormA1TMinus->SetName("gNormA1TMinus");
  gDirectory->Append(gNormA1TMinus);
  TGraph* gNormA1TPlus = new TGraph(100,  x, yIntegPlus);
  gNormA1TPlus->SetName("gNormA1TPlus");
  gDirectory->Append(gNormA1TPlus);

  hA1TMinus->Draw();
  c1->SaveAs("hA1TMinus.pdf");
  hA1TPlus->Draw();
  c1->SaveAs("hA1TPlus.pdf");
  delete lmLineShapeA1T;

 


}



int main(int argc, const char* argv[])
{

  std::cout << "plotMuTau()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  lineShape();

}
