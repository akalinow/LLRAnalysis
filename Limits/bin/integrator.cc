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

#include <Math/IFunction.h>
#include <Math/IFunctionfwd.h>
#include "Math/IntegratorMultiDim.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"

using namespace std;


class Integrand : public ROOT::Math::IMultiGenFunction {
  
public:
  //enum VMpol  { kVMpolUndefined, kVMlongitudinalPol, kVMtransversePol };
  Integrand(double);
  void SetParameterZ(double);
  virtual ~Integrand();
  virtual ROOT::Math::IMultiGenFunction* Clone() const { return new Integrand(0); }

private:
  double z_;
  double DoEval(const double*) const;
  unsigned int NDim() const { return 2;}
  mutable long counter_;
};


Integrand::Integrand(double z): z_(z), counter_(0){
}
void Integrand::SetParameterZ(double z){
  z_ = z;
}
Integrand::~Integrand(){ cout << "counter " << counter_ << endl;}

double Integrand::DoEval(const double* input) const{

  //cout <<  input[0] << " --- " << input[1] << endl;

  counter_++;
  
  double mRho = 0.770;
  double mA1  = 1.260;
  double mPi  = 0.1396;
  double A    = ( mRho*mRho + mA1*mA1)/(mA1*mRho);
  double B    = (-mRho*mRho + mA1*mA1)/(mA1*mRho);
  double C    = 2*mA1/mRho;
  double q    = 1./2*TMath::Sqrt(mRho*mRho-4*mPi*mPi);

  //double x =  1./mA1*(mRho/4*A + mRho/4*B*input[0] + q*B/2*input[1] + q*A/2*input[0]*input[1]  - 
  //	      q*TMath::Sqrt(1-input[1]*input[1])*TMath::Sqrt(1-input[0]*input[0])*TMath::Cos(input[2]));
  
  double cosPhiRho =
    -(mA1*z_ - mRho/4*A - mRho/4*B*input[0] - q*B/2*input[1] - q*A/2*input[0]*input[1])/
    (q*TMath::Sqrt(1-input[0]*input[0])*TMath::Sqrt(1-input[1]*input[1]));
  if( cosPhiRho<-1 || cosPhiRho>1) return 0.;

  double integrand =   
    (A*input[0]*input[1]-2*TMath::Sqrt(1-input[0]*input[0])*TMath::Sqrt(1-input[1]*input[1])* cosPhiRho)*
    (A*input[0]*input[1]-2*TMath::Sqrt(1-input[0]*input[0])*TMath::Sqrt(1-input[1]*input[1])* cosPhiRho)/
    (8*TMath::Pi()/9*(A*A+8))/
    (q/mA1 * TMath::Sqrt(1-cosPhiRho*cosPhiRho) * TMath::Sqrt(1-input[0]*input[0]) * TMath::Sqrt(1-input[1]*input[1]));
  if(integrand<0) cout << "Negative value at cosPhiRho" << cosPhiRho << ", cosThetaA1 "
 		       << input[0] << ", cosThetaRho " << input[1] << " !!!!" << endl;
  
  

  return integrand;
}

Double_t pdfTF3(Double_t *x, Double_t *params){

  double mRho = 0.770;
  double mA1  = 1.260;
  double mPi  = 0.1396;
  double A    = ( mRho*mRho + mA1*mA1)/(mA1*mRho);
  double B    = (-mRho*mRho + mA1*mA1)/(mA1*mRho);
  double C    = 2*mA1/mRho;
  double q    = 1./2*TMath::Sqrt(mRho*mRho-4*mPi*mPi);
 
  double xx =  1./mA1*(mRho/4*A + mRho/4*B*x[0] + q*B/2*x[1] + q*A/2*x[0]*x[1]  -
                      q*TMath::Sqrt(1-x[1]*x[1])*TMath::Sqrt(1-x[0]*x[0])*TMath::Cos(x[2]));

  double integrand = TMath::Abs(xx-params[0])<0.05 ?
    (A*x[0]*x[1]-2*TMath::Sqrt(1-x[0]*x[0])*TMath::Sqrt(1-x[1]*x[1])*TMath::Cos(x[2]))*
    (A*x[0]*x[1]-2*TMath::Sqrt(1-x[0]*x[0])*TMath::Sqrt(1-x[1]*x[1])*TMath::Cos(x[2]))/
    (8*TMath::Pi()/9*(A*A+8)) : 0;

  return integrand;
}

double pdf(string polarization, double cosThetaA, double cosThetaRho, double phiRho){

  double mRho = 0.770;
  double mA1  = 1.260;
  double A    = ( mRho*mRho + mA1*mA1)/(mA1*mRho);

  double pdfValue = polarization.find("L")!=string::npos ?
    (A*cosThetaA*cosThetaRho-2*TMath::Sqrt(1-cosThetaA*cosThetaA)*TMath::Sqrt(1-cosThetaRho*cosThetaRho)*TMath::Cos(phiRho))*
    (A*cosThetaA*cosThetaRho-2*TMath::Sqrt(1-cosThetaA*cosThetaA)*TMath::Sqrt(1-cosThetaRho*cosThetaRho)*TMath::Cos(phiRho))/
    (8*TMath::Pi()/9*(A*A+8)) :

    ((A*TMath::Sqrt(1-cosThetaA*cosThetaA)*cosThetaRho + 2*cosThetaA*TMath::Sqrt(1-cosThetaRho*cosThetaRho)*TMath::Cos(phiRho))*
     (A*TMath::Sqrt(1-cosThetaA*cosThetaA)*cosThetaRho + 2*cosThetaA*TMath::Sqrt(1-cosThetaRho*cosThetaRho)*TMath::Cos(phiRho)) +
     4*(1-cosThetaRho*cosThetaRho)*TMath::Sin(phiRho)*TMath::Sin(phiRho))/
    (16*TMath::Pi()/9*(A*A+8));
    ;

  return pdfValue;
}


double myIntegrator(double z){

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

  Double_t x[100];
  Double_t yL[100];
  Double_t yT[100];

  fwlite::TFileService fs = fwlite::TFileService("polarimeterA1.root");
  TH1F* hXL = new TH1F("hXL","a^{-}_{1 L} #rightarrow #pi^{-}#pi^{0}#pi^{0}; x_{#pi^{-}} ; d#Gamma/dx_{#pi^{-}}",100,0,1);
  TH1F* hXT = new TH1F("hXT","a^{-}_{1 T} #rightarrow #pi^{-}#pi^{0}#pi^{0}; x_{#pi^{-}} ; d#Gamma/dx_{#pi^{-}}",100,0,1);

  //Integrand* IntegrandA1 = new Integrand(0);
  //IntegrandA1->SetParameterZ(z);
  //ROOT::Math::IntegratorMultiDim* integrator_ = new ROOT::Math::IntegratorMultiDim(*IntegrandA1,
  //									   ROOT::Math::IntegrationMultiDim::kADAPTIVE,
  //									   1.E-9, 1E-9,1000000);
  //integrator_->SetRelTolerance(1E-16);
  //integrator_->SetFunction(*IntegrandA1);

  //TF3 f("pdf", pdfTF3, -1,1,-1,1, 0, 2*TMath::Pi(),1);
  //f.SetParameter(0,z);
  //ROOT::Math::WrappedMultiTF1 wf1(f);
  // Create the Integrator
  //ROOT::Math::AdaptiveIntegratorMultiDim* ig = new  ROOT::Math::AdaptiveIntegratorMultiDim(1.E-12,1.E-12,200000);
  // Set parameters of the integration
  //ig.SetMinPts(10000);
  //ig->SetFunction(wf1);
  //ig->SetRelTolerance(0.00001);
  
  //const double xMin[] = {-1,-1};
  //const double xMax[] = { 1, 1};
  //double integral  = integrator_->Integral(xMin,xMax);
  //double integral  = ig->Integral(xMin,xMax);
  //cout << integral << endl;
  //delete IntegrandA1;
  //return integral;

  
  double mRho = 0.770;
  double mA1  = 1.260;
  double mPi  = 0.1396;
  double A    = ( mRho*mRho + mA1*mA1)/(mA1*mRho);
  double B    = (-mRho*mRho + mA1*mA1)/(mA1*mRho);
  double C    = mRho/mA1/2.;


  for(int i = 0; i < 401 ; i++){
    for(int j = 0; j < 401 ; j++){
      for(int k = 0; k < 201 ; k++){

	double cosThetaA_i   = -1.+2*0.0025*i;
	double cosThetaRho_j = -1.+2*0.0025*j;
	double phiRho_k      = 2*TMath::Pi()/200*k;
	double q             = 1./2*TMath::Sqrt(mRho*mRho-4*mPi*mPi);

	double pdf_ijkL = pdf("L",cosThetaA_i, cosThetaRho_j, phiRho_k);
	double pdf_ijkT = pdf("T",cosThetaA_i, cosThetaRho_j, phiRho_k);
	double x       =  1./mA1*(mRho/4*A + mRho/4*B*cosThetaA_i + q*B/2*cosThetaRho_j + q*A/2*cosThetaA_i*cosThetaRho_j  - 
				  q*TMath::Sqrt(1-cosThetaRho_j*cosThetaRho_j)*TMath::Sqrt(1-cosThetaA_i*cosThetaA_i)*TMath::Cos(phiRho_k));

	hXL->Fill(x, pdf_ijkL);
	hXT->Fill(x, pdf_ijkT);
      }
    }
  }

  hXL->Scale(1./hXL->Integral()/hXL->GetBinWidth(1));
  hXT->Scale(1./hXT->Integral()/hXL->GetBinWidth(1));

  for(int i = 1; i <=hXL->GetNbinsX(); i++){
    x[i]      = i*hXL->GetBinWidth(1);
    yL[i]     = hXL->GetBinContent(i);
    yT[i]     = hXT->GetBinContent(i);
  } 
  TGraph* a1L =  new TGraph(100, x, yL);
  a1L->SetName("a1LzFraction");
  gDirectory->Append(a1L);
  TGraph* a1T =  new TGraph(100, x, yT);
  a1T->SetName("a1TzFraction");
  gDirectory->Append(a1T);

  c1->cd();
  hXL->Draw();
  c1->SaveAs("hzA1L.pdf");
  c1->cd();
  hXT->Draw();
  c1->SaveAs("hzA1T.pdf");
  //delete hXL; delete hXT;

  return double(1);
  
}

void makePlot(){

  fwlite::TFileService fs = fwlite::TFileService("pdf.root");
  TH1F* hXL = new TH1F("hXL","",100,0,1);

  //hXL->Fill( 0.5 , myIntegrator( 0.5 ));

  for(int i = 0 ; i < 1001; i++){
      double x = i/1000.;
      hXL->Fill( x , myIntegrator( x ));
  }

}



int main(int argc, const char* argv[])
{

  std::cout << "integrator of A1 distinguishable pion xFraction" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  //makePlot();

  myIntegrator(  1.0 );

}

