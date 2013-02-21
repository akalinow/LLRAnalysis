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

#include "TF3.h"
#include "Math/WrappedMultiTF1.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "Math/AllIntegrationTypes.h"

#include <Math/IFunction.h>
#include <Math/IFunctionfwd.h>
#include "Math/IntegratorMultiDim.h"
#include <Math/Integrator.h>

#include "PhysicsTools/FWLite/interface/TFileService.h"

using namespace std;


class Integrand : public ROOT::Math::IGenFunction {
  
public:
  enum VMtype{ kRho, kA1};
  Integrand(Integrand::VMtype);
  void SetParameterY(double);
  void SetParameterPolSign(int);
  void SetParameterVM(VMtype);
  virtual ~Integrand();
  virtual ROOT::Math::IGenFunction* Clone() const { return new Integrand(kRho); }

private:
  double y_;
  double DoEval(double) const;
  unsigned int NDim() const { return 1;}
  int polSign_;
  VMtype vm_;
  TGraph* rhoLPlus_;
  TGraph*rhoLMinus_;
  TGraph* rhoTPlus_;
  TGraph* rhoTMinus_;
  TGraph* a1LzFraction_;
  TGraph* a1TzFraction_;
  TFile* fxFrac_;
  TFile* fzFrac_;

  mutable long counter_;
};


Integrand::Integrand(VMtype vm): 
  y_(0), counter_(0), polSign_(-1), vm_(vm){

  fxFrac_ = new TFile("./shapes.root","READ");
  fzFrac_ = new TFile("./polarimeterA1.root","READ");

  if(vm_ == kRho){
    rhoLPlus_      = (TGraph*)fxFrac_->Get("gRhoLPlus");
    rhoLMinus_     = (TGraph*)fxFrac_->Get("gRhoLMinus");
    rhoTPlus_      = (TGraph*)fxFrac_->Get("gRhoTPlus");
    rhoTMinus_     = (TGraph*)fxFrac_->Get("gRhoTMinus");
    a1LzFraction_  = 0;
    a1TzFraction_  = 0;
  }
  else if(vm_ == kA1){
    rhoLPlus_      = (TGraph*)fxFrac_->Get("gA1LPlus");
    rhoLMinus_     = (TGraph*)fxFrac_->Get("gA1LMinus");
    rhoTPlus_      = (TGraph*)fxFrac_->Get("gA1TPlus");
    rhoTMinus_     = (TGraph*)fxFrac_->Get("gA1TMinus");
    a1LzFraction_  = (TGraph*)fzFrac_->Get("a1LzFraction");
    a1TzFraction_  = (TGraph*)fzFrac_->Get("a1TzFraction");
  }
  else{
    rhoLPlus_ = 0;
    rhoLMinus_ = 0;
    rhoTPlus_ = 0;
    rhoTMinus_ = 0;
    a1LzFraction_  = 0;
    a1TzFraction_  = 0;
  }


}
void Integrand::SetParameterY(double y){
  y_ = y;
}
void Integrand::SetParameterVM(VMtype vm){
  vm_ = vm;
}
void Integrand::SetParameterPolSign(int polSign){
  polSign_ = polSign;
}
Integrand::~Integrand(){ 
  cout << "counter " << counter_ << endl;

  fxFrac_->Close();
  delete fxFrac_;
  fzFrac_->Close();
  delete fzFrac_;
}


double Integrand::DoEval(double input) const{

  counter_++;
  
  double probLx = (polSign_ == +1) ? rhoLPlus_->Eval(input) : rhoLMinus_->Eval(input);
  double probTx = (polSign_ == +1) ? rhoTPlus_->Eval(input) : rhoTMinus_->Eval(input);

  double z = y_/input;

  double probLz = 0.;
  double probTz = 0.;

  if(vm_ == kRho){
    probLz = 3.*(2.*z - 1.)*(2.*z - 1.);
    probTz = 6.*z*(1. - z);
  }
  else if(vm_ == kA1){
    probLz = a1LzFraction_->Eval(z);
    probTz = a1TzFraction_->Eval(z);
  }
  else{
    probLz = 0.;
    probTz = 0.;
  }

  double prob = (probLz*probLx + probTz*probTx);

  double integrand = input>=y_ ? 1./input*prob : 0.0;

  return integrand;
}




double myIntegrator(double y, int polSign, Integrand::VMtype vm){

  Integrand* IntegrandRho = new Integrand(vm);
  IntegrandRho->SetParameterY(y);
  IntegrandRho->SetParameterPolSign(polSign);
  IntegrandRho->SetParameterVM(vm);

  ROOT::Math::Integrator* integrator_ = 
    new ROOT::Math::Integrator(*IntegrandRho);
  integrator_->SetRelTolerance(1e-06);
  ////new ROOT::Math::Integrator(*IntegrandRho,
  //		       ROOT::Math::IntegrationOneDim::kADAPTIVE,
  //		       1.E-9, 1E-9,10000);


  integrator_->SetFunction(*IntegrandRho);

  
  double xMin = 0.01;
  double xMax =  1.0;
  double integral  = integrator_->Integral(xMin,xMax);

  //cout << integral << endl;

  delete IntegrandRho;
  return integral; 
  
}


void makePlot(){


  Double_t x[100];
  Double_t yRhoMinus[100];
  Double_t yRhoPlus[100];
  Double_t yA1Plus[100];
  Double_t yA1Minus[100];

  Double_t yIntegRhoMinus[100];
  Double_t yIntegRhoPlus[100];
  Double_t yIntegA1Plus[100];
  Double_t yIntegA1Minus[100];
  
  double normRhoMinus = 0.;
  double normRhoPlus  = 0.;
  double normA1Minus  = 0.;
  double normA1Plus   = 0.;

  for(int i = 0; i < 100; i++){
    cout << "Doing " << i << endl;
    double resultPlus  = myIntegrator(  i*0.01 , +1, Integrand::kRho);
    double resultMinus = myIntegrator(  i*0.01 , -1, Integrand::kRho);
    yRhoMinus[i] = resultMinus;
    yRhoPlus[i]  = resultPlus;
    x[i] = 0.01*i;
    normRhoMinus += (resultMinus*0.01);
    normRhoPlus  += (resultPlus*0.01);
  } 
  for(int i = 0; i < 100; i++){
    cout << "Doing " << i << endl;
    double resultPlus  = myIntegrator(  i*0.01 , +1, Integrand::kA1);
    double resultMinus = myIntegrator(  i*0.01 , -1, Integrand::kA1);
    yA1Minus[i] = resultMinus;
    yA1Plus[i]  = resultPlus;
    x[i] = 0.01*i;
    normA1Minus += (resultMinus*0.01);
    normA1Plus  += (resultPlus*0.01);
  }
 
  fwlite::TFileService fs = fwlite::TFileService("polarimetersY.root");

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

  /////////////////////////////////////////

  TH1F* hYA1Plus  = new TH1F("hYA1Plus", "#tau^{-}_{R} #rightarrow a_{1}^{-}; y_{#pi^{-}} ; d#Gamma/dy_{#pi^{-}}",100,0,1);
  TH1F* hYA1Minus = new TH1F("hYA1Minus","#tau^{-}_{L} #rightarrow a_{1}^{-}; y_{#pi^{-}} ; d#Gamma/dy_{#pi^{-}}",100,0,1);
  TH1F* hYRhoPlus  = new TH1F("hYRhoPlus", "#tau^{-}_{R} #rightarrow #rho^{-}; y_{#pi^{-}} ; d#Gamma/dy_{#pi^{-}}",100,0,1);
  TH1F* hYRhoMinus = new TH1F("hYRhoMinus","#tau^{-}_{L} #rightarrow #rho^{-}; y_{#pi^{-}} ; d#Gamma/dy_{#pi^{-}}",100,0,1);

  double counterA1Plus = 0;
  double counterA1Minus = 0;
  double counterRhoPlus = 0;
  double counterRhoMinus = 0;

  for(int i = 0 ; i < 100; i++){
    yA1Plus[i]  /= normA1Plus;
    yA1Minus[i] /= normA1Minus;
    yRhoPlus[i] /= normRhoPlus;
    yRhoMinus[i]/= normRhoMinus;

    counterA1Plus  += yA1Plus[i] ;
    counterA1Minus += yA1Minus[i] ;
    counterRhoPlus  += yRhoPlus[i] ;
    counterRhoMinus += yRhoMinus[i] ;

    yIntegA1Plus[i]   = counterA1Plus*0.01;
    yIntegA1Minus[i]  = counterA1Minus*0.01;
    yIntegRhoPlus[i]  = counterRhoPlus*0.01;
    yIntegRhoMinus[i] = counterRhoMinus*0.01;
  }

  for(int i = 0; i < hYA1Plus->GetNbinsX(); i++){
    hYA1Plus->SetBinContent(i+1, yA1Plus[i]);
  }
  for(int i = 0; i < hYA1Minus->GetNbinsX(); i++){
    hYA1Minus->SetBinContent(i+1, yA1Minus[i]);
  }
  for(int i = 0; i < hYRhoPlus->GetNbinsX(); i++){
    hYRhoPlus->SetBinContent(i+1, yRhoPlus[i]);
  }
  for(int i = 0; i < hYRhoMinus->GetNbinsX(); i++){
    hYRhoMinus->SetBinContent(i+1, yRhoMinus[i]);
  }

  c1->cd();
  TGraph* gRhoYMinus = new TGraph(100, x, yRhoMinus);
  gRhoYMinus->SetName("gRhoYMinus");
  gDirectory->Append(gRhoYMinus);
  TGraph* gRhoYPlus = new TGraph(100, x, yRhoPlus);
  gRhoYPlus->SetName("gRhoYPlus");
  gDirectory->Append(gRhoYPlus);
  TGraph* gNormRhoYMinus = new TGraph(100, x, yIntegRhoMinus);
  gNormRhoYMinus->SetName("gNormRhoYMinus");
  gDirectory->Append(gNormRhoYMinus);
  TGraph* gNormRhoYPlus = new TGraph(100, x, yIntegRhoPlus);
  gNormRhoYPlus->SetName("gNormRhoYPlus");
  gDirectory->Append(gNormRhoYPlus);

  hYRhoMinus->Draw();
  c1->SaveAs("hRhoYMinus.pdf");
  hYRhoPlus->Draw();
  c1->SaveAs("hRhoYPlus.pdf");

 
  c1->cd();
  TGraph* gA1YMinus = new TGraph(100, x, yA1Minus);
  gA1YMinus->SetName("gA1YMinus");
  gDirectory->Append(gA1YMinus);
  TGraph* gA1YPlus = new TGraph(100, x, yA1Plus);
  gA1YPlus->SetName("gA1YPlus");
  gDirectory->Append(gA1YPlus);
  TGraph* gNormA1YMinus = new TGraph(100, x, yIntegA1Minus);
  gNormA1YMinus->SetName("gNormA1YMinus");
  gDirectory->Append(gNormA1YMinus);
  TGraph* gNormA1YPlus = new TGraph(100, x, yIntegA1Plus);
  gNormA1YPlus->SetName("gNormA1YPlus");
  gDirectory->Append(gNormA1YPlus);

  hYA1Plus->Draw();
  c1->SaveAs("hA1YPlus.pdf");
  hYA1Minus->Draw();
  c1->SaveAs("hA1YMinus.pdf");

 

  return;
}


int main(int argc, const char* argv[])
{

  std::cout << "integrator of pho distinguishable pion xFraction" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  //myIntegrator(  0.5 , -1);

  makePlot();

}

