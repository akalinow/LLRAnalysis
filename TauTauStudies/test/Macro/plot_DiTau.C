#include <cstdlib>
#include <iostream> 
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
#include "TCanvas.h"
#include "TLegend.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TFile.h"

//#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#endif


void plotAfterTagOpt( TString weightFile = "TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",
		      Double_t effS_ = 0.5, Double_t ptCut_ = 80){

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


  TMVA::Tools::Instance();

  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );   

  Float_t pt1, pt2;
  Float_t Deta, Mjj;
  Float_t eta1,eta2;

  reader->AddVariable( "pt1", &pt1);
  reader->AddVariable( "pt2", &pt2);
  reader->AddVariable( "Deta",&Deta);
  reader->AddVariable( "Mjj", &Mjj);

  reader->AddSpectator("eta1",&eta1);
  reader->AddSpectator("eta2",&eta2);

  reader->BookMVA( "Cuts", TString("weights/")+weightFile ); 

  std::map<std::string,TTree*> tMap;
  tMap["qqH115"]=treeqqH115;
  tMap["Zjets"]=treeZ;
  tMap["Wjets"]=treeW;
  tMap["QCD"]=treeQCD;
  tMap["TTbar"]=treeTTbar;


  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauVisPt   = 10;
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
  float xHigh_diTauSVFitPt = 500;

  TH1F* hZ_diTauSVFitPt      = new TH1F("hZ_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hZMuMu_diTauSVFitPt  = new TH1F("hZMuMu_diTauSVFitPt"," ; SVFitp_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hW_diTauSVFitPt      = new TH1F("hW_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hQCD_diTauSVFitPt    = new TH1F("hQCD_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hTTbar_diTauSVFitPt  = new TH1F("hTTbar_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  TH1F* hqqH115_diTauSVFitPt = new TH1F("hqqH115_diTauSVFitPt"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitPt,xLow_diTauSVFitPt,xHigh_diTauSVFitPt);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauCAPt   = 20;
  float xLow_diTauCAPt  = 10;
  float xHigh_diTauCAPt = 500;

  TH1F* hZ_diTauCAPt      = new TH1F("hZ_diTauCAPt"," ;Coll. Approx  p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hZMuMu_diTauCAPt  = new TH1F("hZMuMu_diTauCAPt"," ; Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hW_diTauCAPt      = new TH1F("hW_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hQCD_diTauCAPt    = new TH1F("hQCD_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hTTbar_diTauCAPt  = new TH1F("hTTbar_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  TH1F* hqqH115_diTauCAPt = new TH1F("hqqH115_diTauCAPt"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAPt,xLow_diTauCAPt,xHigh_diTauCAPt);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauVisEta   = 10;
  float xLow_diTauVisEta  = -5;
  float xHigh_diTauVisEta = 5;

  TH1F* hZ_diTauVisEta      = new TH1F("hZ_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hZMuMu_diTauVisEta  = new TH1F("hZMuMu_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hW_diTauVisEta      = new TH1F("hW_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hQCD_diTauVisEta    = new TH1F("hQCD_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hTTbar_diTauVisEta  = new TH1F("hTTbar_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);
  TH1F* hqqH115_diTauVisEta = new TH1F("hqqH115_diTauVisEta"," ;visible p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauVisEta,xLow_diTauVisEta,xHigh_diTauVisEta);

  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauSVFitEta   = 10;
  float xLow_diTauSVFitEta  = -5;
  float xHigh_diTauSVFitEta = 5;

  TH1F* hZ_diTauSVFitEta      = new TH1F("hZ_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hZMuMu_diTauSVFitEta  = new TH1F("hZMuMu_diTauSVFitEta"," ; SVFitp_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hW_diTauSVFitEta      = new TH1F("hW_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hQCD_diTauSVFitEta    = new TH1F("hQCD_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hTTbar_diTauSVFitEta  = new TH1F("hTTbar_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  TH1F* hqqH115_diTauSVFitEta = new TH1F("hqqH115_diTauSVFitEta"," ;SVFit p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauSVFitEta,xLow_diTauSVFitEta,xHigh_diTauSVFitEta);
  ////////////////////////////////////////////////////////////////////////
  int nBins_diTauCAEta   = 10;
  float xLow_diTauCAEta  = -5;
  float xHigh_diTauCAEta = 5;

  TH1F* hZ_diTauCAEta      = new TH1F("hZ_diTauCAEta"," ;Coll. Approx  p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hZMuMu_diTauCAEta  = new TH1F("hZMuMu_diTauCAEta"," ; Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hW_diTauCAEta      = new TH1F("hW_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hQCD_diTauCAEta    = new TH1F("hQCD_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hTTbar_diTauCAEta  = new TH1F("hTTbar_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  TH1F* hqqH115_diTauCAEta = new TH1F("hqqH115_diTauCAEta"," ;Coll. Approx. p_{T}^{#tau#tau} (GeV) ; a.u.",nBins_diTauCAEta,xLow_diTauCAEta,xHigh_diTauCAEta);
  ////////////////////////////////////////////////////////////////////////
  int nBins_ptL1   = 8;
  float xLow_ptL1  = 10;
  float xHigh_ptL1 = 150;

  TH1F* hZ_ptL1      = new TH1F("hZ_ptL1"," ;p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  TH1F* hZMuMu_ptL1  = new TH1F("hZMuMu_ptL1"," ; p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  TH1F* hW_ptL1      = new TH1F("hW_ptL1"," ;p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  TH1F* hQCD_ptL1    = new TH1F("hQCD_ptL1"," ;p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  TH1F* hTTbar_ptL1  = new TH1F("hTTbar_ptL1"," ;p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  TH1F* hqqH115_ptL1 = new TH1F("hqqH115_ptL1"," ;p_{T}^{#mu} (GeV) ; a.u.",nBins_ptL1,xLow_ptL1,xHigh_ptL1);
  ////////////////////////////////////////////////////////////////////////
  int nBins_ptL2   = 8;
  float xLow_ptL2  = 10;
  float xHigh_ptL2 = 150;

  TH1F* hZ_ptL2      = new TH1F("hZ_ptL2"," ;p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
  TH1F* hZMuMu_ptL2  = new TH1F("hZMuMu_ptL2"," ; p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
  TH1F* hW_ptL2      = new TH1F("hW_ptL2"," ;p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
  TH1F* hQCD_ptL2    = new TH1F("hQCD_ptL2"," ;p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
  TH1F* hTTbar_ptL2  = new TH1F("hTTbar_ptL2"," ;p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
  TH1F* hqqH115_ptL2 = new TH1F("hqqH115_ptL2"," ;p_{T}^{#tau} (GeV) ; a.u.",nBins_ptL2,xLow_ptL2,xHigh_ptL2);
 
  Double_t pt1_, pt2_;
  Double_t Deta_, Mjj_;
  Double_t eta1_, eta2_;
  double diTauVisPt_,diTauVisEta_,diTauCAPt_,diTauCAEta_,diTauSVFitPt_,diTauSVFitEta_;
  Double_t ptL1_, ptL2_;
  TCut mycuts = "pt1>0 && abs(eta1*eta2)/eta1/eta2<0"; 

  for(std::map<std::string,TTree*>::iterator it = tMap.begin(); it != tMap.end(); it++){

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TTree* currentTree = (TTree*)(it->second)->CopyTree(mycuts);

    Int_t counter    = 0;
    Int_t counterCut = 0;

    currentTree->SetBranchAddress( "pt1", &pt1_ );
    currentTree->SetBranchAddress( "pt2", &pt2_ );
    currentTree->SetBranchAddress( "Deta",&Deta_ );
    currentTree->SetBranchAddress( "Mjj", &Mjj_ );
    currentTree->SetBranchAddress( "eta1",&eta1_ );
    currentTree->SetBranchAddress( "eta2",&eta2_ );
    currentTree->SetBranchAddress("diTauVisPt",    &diTauVisPt_);
    currentTree->SetBranchAddress("diTauVisEta",   &diTauVisEta_);
    currentTree->SetBranchAddress("diTauCAPt",     &diTauCAPt_);
    currentTree->SetBranchAddress("diTauCAEta",    &diTauCAEta_);
    currentTree->SetBranchAddress("diTauSVFitPt",  &diTauSVFitPt_);
    currentTree->SetBranchAddress("diTauSVFitEta", &diTauSVFitEta_);
    currentTree->SetBranchAddress( "ptL1", &ptL1_ );
    currentTree->SetBranchAddress( "ptL2", &ptL2_ );

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {
      if (ievt%1000000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
      currentTree->GetEntry(ievt);
      pt1  = pt1_;
      pt2  = pt2_;
      Deta = Deta_;
      Mjj  = Mjj_;
      if(reader->EvaluateMVA( "Cuts", effS_ )){
	if((it->first).find("Z")!=string::npos){
	  hZ_diTauVisPt->Fill( diTauVisPt_ );
	  hZ_diTauSVFitPt->Fill( diTauSVFitPt_  );
	  hZ_diTauCAPt->Fill( diTauCAPt_  );
	  hZ_diTauVisEta->Fill( (diTauVisEta_ -(eta1_+eta2_)/2) );
	  hZ_diTauSVFitEta->Fill( (diTauSVFitEta_ -(eta1_+eta2_)/2) );
	  hZ_diTauCAEta->Fill( (diTauCAEta_ -(eta1_+eta2_)/2) );
	  hZ_ptL1->Fill( ptL1_ );
	  hZ_ptL2->Fill( ptL2_ );
	}
	if((it->first).find("W")!=string::npos){
	  hW_diTauVisPt->Fill( diTauVisPt_  );
	  hW_diTauSVFitPt->Fill( diTauSVFitPt_  );
	  hW_diTauCAPt->Fill( diTauCAPt_  );
	  hW_diTauVisEta->Fill( (diTauVisEta_ -(eta1_+eta2_)/2) );
	  hW_diTauSVFitEta->Fill( (diTauSVFitEta_ -(eta1_+eta2_)/2) );
	  hW_diTauCAEta->Fill( (diTauCAEta_ -(eta1_+eta2_)/2) );
	  hW_ptL1->Fill( ptL1_ );
	  hW_ptL2->Fill( ptL2_ );
	}
	if((it->first).find("QCD")!=string::npos){
	  hQCD_diTauVisPt->Fill( diTauVisPt_  );
	  hQCD_diTauSVFitPt->Fill( diTauSVFitPt_  );
	  hQCD_diTauCAPt->Fill( diTauCAPt_  );
	  hQCD_diTauVisEta->Fill( (diTauVisEta_ -(eta1_+eta2_)/2) );
	  hQCD_diTauSVFitEta->Fill( (diTauSVFitEta_ -(eta1_+eta2_)/2) );
	  hQCD_diTauCAEta->Fill( (diTauCAEta_ -(eta1_+eta2_)/2) );
	  hQCD_ptL1->Fill( ptL1_ );
	  hQCD_ptL2->Fill( ptL2_ );
	}
	if((it->first).find("TT")!=string::npos){
	  hTTbar_diTauVisPt->Fill( diTauVisPt_  );
	  hTTbar_diTauSVFitPt->Fill( diTauSVFitPt_  );
	  hTTbar_diTauCAPt->Fill( diTauCAPt_  );
	  hTTbar_diTauVisEta->Fill( (diTauVisEta_ -(eta1_+eta2_)/2) );
	  hTTbar_diTauSVFitEta->Fill( (diTauSVFitEta_ -(eta1_+eta2_)/2) );
	  hTTbar_diTauCAEta->Fill( (diTauCAEta_ -(eta1_+eta2_)/2) );
	  hTTbar_ptL1->Fill( ptL1_ );
	  hTTbar_ptL2->Fill( ptL2_ );
	}
	if((it->first).find("qqH")!=string::npos){
	  hqqH115_diTauVisPt->Fill( diTauVisPt_  );
	  hqqH115_diTauSVFitPt->Fill( diTauSVFitPt_  );
	  hqqH115_diTauCAPt->Fill( diTauCAPt_  );
	  hqqH115_diTauVisEta->Fill( (diTauVisEta_ -(eta1_+eta2_)/2) );
	  hqqH115_diTauSVFitEta->Fill( (diTauSVFitEta_ -(eta1_+eta2_)/2) );
	  hqqH115_diTauCAEta->Fill( (diTauCAEta_ -(eta1_+eta2_)/2) );
	  hqqH115_ptL1->Fill( ptL1_ );
	  hqqH115_ptL2->Fill( ptL2_ );
	}
	counter++;
	if(diTauSVFitPt_>ptCut_) counterCut++;
      }
    }
    cout<< "Sample " << it->first << " has a rejection factor " << (Float_t)counter/(Float_t)currentTree->GetEntries() << "+/-" <<
      TMath::Sqrt((Float_t)counter/currentTree->GetEntries()*(1-counter/currentTree->GetEntries())/currentTree->GetEntries()) 
	<< " for effS="<< effS_ << endl;
    cout << "Before tautau-pT cut " << counter << " --- after cut @ 80 GeV : " << counterCut << endl;  

  }

 
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
  
  hZ_ptL1->SetLineColor(kRed);
  hW_ptL1->SetLineColor(kGreen);
  hQCD_ptL1->SetLineColor(kBlue);
  hTTbar_ptL1->SetLineColor(kMagenta);
  hqqH115_ptL1->SetLineWidth(2);

  hZ_ptL2->SetLineColor(kRed);
  hW_ptL2->SetLineColor(kGreen);
  hQCD_ptL2->SetLineColor(kBlue);
  hTTbar_ptL2->SetLineColor(kMagenta);
  hqqH115_ptL2->SetLineWidth(2);


 

  c1->Divide(4,2);

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

  c1->cd(7);
  hQCD_ptL1->DrawNormalized("HIST");
  hTTbar_ptL1->DrawNormalized("HISTSAME");
  hqqH115_ptL1->DrawNormalized("HISTSAME");
  hZ_ptL1->DrawNormalized("HISTSAME");
  hW_ptL1->DrawNormalized("HISTSAME");
  leg->Draw();

  c1->cd(8);
  hQCD_ptL2->DrawNormalized("HIST");
  hTTbar_ptL2->DrawNormalized("HISTSAME");
  hqqH115_ptL2->DrawNormalized("HISTSAME");
  hZ_ptL2->DrawNormalized("HISTSAME");
  hW_ptL2->DrawNormalized("HISTSAME");
  leg->Draw();
 

  c1->Update();

  c1->Draw();
 
}
