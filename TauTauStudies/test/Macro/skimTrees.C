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

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>

#define SAVE   true
#define MINPt1 20.0 //20
#define MINPt2 15.0 //15

using namespace ROOT::Math;
using namespace std;





void makeTrees_ElecTauStream(int index = 4){

  std::vector<std::string> samples;
  samples.push_back("DYJets-50-madgraph-PUS1");
  samples.push_back("DYToEE-20-PUS1");
  samples.push_back("DYToTauTau-20-PUS1");
  samples.push_back("Zjets-alpgen-PUS1");
  samples.push_back("TTJets-madgraph-PUS1");
  samples.push_back("WJets-madgraph-PUS1");
  //samples.push_back("WW-pythia-PUS1");
  //samples.push_back("WZ-pythia-PUS1");
  //samples.push_back("ZZ-pythia-PUS1");
  samples.push_back("DiBoson");

  samples.push_back("QCD");
  samples.push_back("G1Jet");
  samples.push_back("VBFH115-powheg-PUS1");
  samples.push_back("VBFH135-powheg-PUS1");
  samples.push_back("GGFH115-powheg-PUS1");
  samples.push_back("GGFH130-powheg-PUS1");

  std::vector<float> crossSec;
  crossSec.push_back( 3048  );
  crossSec.push_back( 1666  );
  crossSec.push_back( 1666  );
  crossSec.push_back( -1  );
  crossSec.push_back( 157.5 );
  crossSec.push_back( 31314.0);
  crossSec.push_back( -1 );
  //crossSec.push_back( 2.9 );
  //crossSec.push_back( 10.4 );
  //crossSec.push_back( 4.297 );
  crossSec.push_back( -1 );
  crossSec.push_back( -1 );
  crossSec.push_back( 0.1012);
  crossSec.push_back( 0.05049);
  crossSec.push_back( 7.65e-02 * 18.13 );
  crossSec.push_back( 4.52e-02 * 13.8 );


  Float_t Lumi=1000;

  TString sample(samples[index]);


  //TString outName = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/nTuple"+sample+"_Open_ElecTauStream.root";
  TString outName = "nTuple"+sample+"_Open_ElecTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj;
  float Dphi,diTauSVFitMass,diTauVisPt,diTauVisEta,diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass;
  float ptL1,ptL2,etaL1,etaL2;
  float diTauCharge_,MtLeg1_;
  float numPV_; 
  float sampleWeight,combRelIsoLeg1;
  int tightestHPSWP_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto;
  float HLT;
  int isTauLegMatched_;

  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");
  outTreePtOrd->Branch("ptVeto", &ptVeto,"ptVeto/F");
  outTreePtOrd->Branch("diTauVisMass",    &diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,"diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitMass",  &diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("combRelIsoLeg1",  &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("HLT", &HLT,"HLT/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");

  //outTreePtOrd->Branch("diTauVisPt",    &diTauVisPt,"diTauVisPt/F");
  //outTreePtOrd->Branch("diTauVisEta",   &diTauVisEta,"diTauVisEta/F");
  //outTreePtOrd->Branch("diTauCAPt",     &diTauCAPt,"diTauCAPt/F");
  //outTreePtOrd->Branch("diTauCAEta",    &diTauCAEta,"diTauCAEta/F");
  //outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  //outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta,"diTauSVFitEta/F");
  //outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  //outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/ElecTauStream2011//treeElecTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TTree* currentTree = (TTree*)file->Get("elecTauStreamAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* allEvents = (TH1F*)file->Get("allEventsFilter/totalEvents");
  float totalEvents = allEvents->GetBinContent(1);
  float scaleFactor = ((crossSec[index])*Lumi) / totalEvents ;


  //currentTree->SetBranchStatus("diTauVisP4",0);
  currentTree->SetBranchStatus("diTauCAP4",0);
  //currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfit1P4",0);
  currentTree->SetBranchStatus("diTauSVfit2P4",0);
  //currentTree->SetBranchStatus("diTauSVfit3P4",0);
  //currentTree->SetBranchStatus("diTauLegsP4",0);
  currentTree->SetBranchStatus("genDiTauLegsP4",0);
  currentTree->SetBranchStatus("METP4",0);
  currentTree->SetBranchStatus("genMETP4",0);
  currentTree->SetBranchStatus("jetsP4",0);
  currentTree->SetBranchStatus("genJetsIDP4",0);
  currentTree->SetBranchStatus("jetsBtagHE",0);
  currentTree->SetBranchStatus("jetsBtagHP",0);
  currentTree->SetBranchStatus("sumEt",0);
  //currentTree->SetBranchStatus("MtLeg1",0);
  //currentTree->SetBranchStatus("chIsoLeg1",0);
  //currentTree->SetBranchStatus("nhIsoLeg1",0);
  //currentTree->SetBranchStatus("phIsoLeg1",0);
  currentTree->SetBranchStatus("chIsoLeg2",0);
  currentTree->SetBranchStatus("nhIsoLeg2",0);
  currentTree->SetBranchStatus("phIsoLeg2",0);
  currentTree->SetBranchStatus("dxy1",0);
  currentTree->SetBranchStatus("dxy2",0);
  currentTree->SetBranchStatus("run",0);
  currentTree->SetBranchStatus("event",0);
  //currentTree->SetBranchStatus("numPV",0);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  //currentTree->SetBranchStatus("tightestHPSWP",0);
  currentTree->SetBranchStatus("visibleTauMass",0);
  //currentTree->SetBranchStatus("isTauLegMatched",0);
  currentTree->SetBranchStatus("isElecLegMatched",0);
  //currentTree->SetBranchStatus("diTauCharge",0);
  //currentTree->SetBranchStatus("tauXTriggers",0);

  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsP4;
  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauVisP4;
  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauICAP4;
  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3P4;
  //std::vector<double>* jetsBtagHE;
 
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  int tightestHPSWP; 
  float  diTauCharge, MtLeg1; 
  float numPV;
  float weight;
  int isTauLegMatched;

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets        = new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsP4 = new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauVisP4  =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauICAP4 =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  //currentTree->SetBranchAddress("diTauICAP4",&diTauICAP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3P4 =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3P4);
  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);


  //currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  currentTree->SetBranchAddress("chIsoLeg1",&chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1",&nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1",&phIsoLeg1);
  currentTree->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);
  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("isTauLegMatched",&isTauLegMatched);


  if(crossSec[index]<0) currentTree->SetBranchAddress("weight",&weight);


  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);
    if(n%1000==0) cout << n << endl;
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
    diTauVisPt=-99;diTauVisEta=-99;diTauCAPt=-99;diTauCAEta=-99;
    diTauSVFitMass = -99;diTauSVFitPt=-99;diTauSVFitEta=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;diTauCharge_=-99;MtLeg1_=-99;
    tightestHPSWP_=-99;numPV_=-99;combRelIsoLeg1=-99;sampleWeight=-99;
    ptVeto=-99;HLT=-99;isTauLegMatched_=-99;

    if(jets->size()>1 && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2 && (*jets)[0].Eta()*(*jets)[1].Eta()<0 ){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) ;
      Mjj  = ((*jets)[0]+(*jets)[1]).M();

      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) && (*jets)[k].Et()>ptVeto) ptVeto=(*jets)[k].Et();  
      }

      diTauVisMass  = (*diTauVisP4)[0].M();
      diTauVisPt  = (*diTauVisP4)[0].Pt();
      diTauVisEta = (*diTauVisP4)[0].Eta();
      //diTauCAPt  = (*diTauICAP4)[0].Pt();
      //diTauCAEta = (*diTauICAP4)[0].Eta();
      diTauSVFitPt  = (*diTauSVfit3P4)[0].Pt();
      diTauSVFitEta = (*diTauSVfit3P4)[0].Eta();
      diTauSVFitMass = (*diTauSVfit3P4)[0].M();
      //jetsBtagHE1 = (*jetsBtagHE)[0];
      //jetsBtagHE2 = (*jetsBtagHE)[1];
      ptL1  = (*diTauLegsP4)[0].Pt();
      ptL2  = (*diTauLegsP4)[1].Pt();
      etaL1 = (*diTauLegsP4)[0].Eta();
      etaL2 = (*diTauLegsP4)[1].Eta();
      diTauCharge_ = diTauCharge;
      MtLeg1_ = MtLeg1;
      tightestHPSWP_ = tightestHPSWP;
      numPV_ = numPV;
      combRelIsoLeg1 = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
      sampleWeight = (scaleFactor>0) ? scaleFactor : weight;
      HLT=float((*tauXTriggers)[0]);   
      isTauLegMatched_ = isTauLegMatched;

      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete diTauLegsP4; delete diTauVisP4; delete diTauICAP4; delete diTauSVfit3P4; delete tauXTriggers;
  
}


void makeTrees_MuTauStream(int index = 4){

  std::vector<std::string> samples;
  samples.push_back("DYJets-Mu-50-madgraph-PUS1");
  samples.push_back("DYToMuMu-20-PUS1");
  samples.push_back("DYToTauTau-Mu-20-PUS1");
  samples.push_back("Zjets-Mu-alpgen-PUS1");
  samples.push_back("TTJets-Mu-madgraph-PUS1");
  samples.push_back("WJets-Mu-madgraph-PUS1");
  samples.push_back("DiBoson-Mu");

  samples.push_back("QCDmu");

  samples.push_back("VBFH115-Mu-powheg-PUS1");
  samples.push_back("VBFH135-Mu-powheg-PUS1");
  samples.push_back("GGFH115-Mu-powheg-PUS1");
  samples.push_back("GGFH130-Mu-powheg-PUS1");

  std::vector<float> crossSec;
  crossSec.push_back( 3048  );
  crossSec.push_back( 1666  );
  crossSec.push_back( 1666  );
  crossSec.push_back( -1 );
  crossSec.push_back( 157.5 );
  crossSec.push_back( 31314.0);
  crossSec.push_back( -1 );
 
  crossSec.push_back( 84679 );
  crossSec.push_back( 0.1012);
  crossSec.push_back( 0.05049);
  crossSec.push_back( 7.65e-02 * 18.13 );
  crossSec.push_back( 4.52e-02 * 13.8 );


  Float_t Lumi=1000;

  TString sample(samples[index]);


  //TString outName = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/nTuple"+sample+"_Open_ElecTauStream.root";
  TString outName = "nTuple"+sample+"_Open_MuTauStream.root";
  TFile *outFile = new TFile(outName,"RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
 
  float pt1,pt2,eta1,eta2,Deta,Mjj;
  float Dphi,diTauSVFitMass,diTauVisPt,diTauVisEta,diTauCAPt,diTauCAEta,diTauSVFitPt,diTauSVFitEta;
  float diTauVisMass;
  float ptL1,ptL2,etaL1,etaL2;
  float diTauCharge_,MtLeg1_;
  float numPV_; 
  float sampleWeight,combRelIsoLeg1;
  int tightestHPSWP_;
  float jetsBtagHE1,jetsBtagHE2;
  float ptVeto;
  float HLT;
  int isTauLegMatched_;
 
  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");
  outTreePtOrd->Branch("ptVeto", &ptVeto,"ptVeto/F");
  outTreePtOrd->Branch("diTauVisMass",    &diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,"diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitMass",  &diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("etaL1", &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2", &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",  &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",  &ptL2,"ptL2/F");
  outTreePtOrd->Branch("diTauCharge",  &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",  &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("numPV",  &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",  &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("combRelIsoLeg1",  &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("tightestHPSWP",  &tightestHPSWP_,"tightestHPSWP/I");
  outTreePtOrd->Branch("HLT", &HLT,"HLT/F");
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");

  //outTreePtOrd->Branch("diTauVisPt",    &diTauVisPt,"diTauVisPt/F");
  //outTreePtOrd->Branch("diTauVisEta",   &diTauVisEta,"diTauVisEta/F");
  //outTreePtOrd->Branch("diTauCAPt",     &diTauCAPt,"diTauCAPt/F");
  //outTreePtOrd->Branch("diTauCAEta",    &diTauCAEta,"diTauCAEta/F");
  //outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  //outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta,"diTauSVFitEta/F");
  //outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  //outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");

 
  TString inName = "/data_CMS/cms/lbianchini/MuTauStream2011//treeMuTauStream_"+sample+".root";
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }
  TTree* currentTree = (TTree*)file->Get("muTauStreamAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  TH1F* allEvents = (TH1F*)file->Get("allEventsFilter/totalEvents");
  float totalEvents = allEvents->GetBinContent(1);
  float scaleFactor = ((crossSec[index])*Lumi) / totalEvents ;


  //currentTree->SetBranchStatus("diTauVisP4",0);
  currentTree->SetBranchStatus("diTauCAP4",0);
  //currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfit1P4",0);
  currentTree->SetBranchStatus("diTauSVfit2P4",0);
  //currentTree->SetBranchStatus("diTauSVfit3P4",0);
  //currentTree->SetBranchStatus("diTauLegsP4",0);
  currentTree->SetBranchStatus("genDiTauLegsP4",0);
  currentTree->SetBranchStatus("METP4",0);
  currentTree->SetBranchStatus("genMETP4",0);
  currentTree->SetBranchStatus("jetsP4",0);
  currentTree->SetBranchStatus("genJetsIDP4",0);
  currentTree->SetBranchStatus("jetsBtagHE",0);
  currentTree->SetBranchStatus("jetsBtagHP",0);
  currentTree->SetBranchStatus("sumEt",0);
  //currentTree->SetBranchStatus("MtLeg1",0);
  //currentTree->SetBranchStatus("chIsoLeg1",0);
  //currentTree->SetBranchStatus("nhIsoLeg1",0);
  //currentTree->SetBranchStatus("phIsoLeg1",0);
  currentTree->SetBranchStatus("chIsoLeg2",0);
  currentTree->SetBranchStatus("nhIsoLeg2",0);
  currentTree->SetBranchStatus("phIsoLeg2",0);
  currentTree->SetBranchStatus("dxy1",0);
  currentTree->SetBranchStatus("dxy2",0);
  currentTree->SetBranchStatus("run",0);
  currentTree->SetBranchStatus("event",0);
  //currentTree->SetBranchStatus("numPV",0);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  //currentTree->SetBranchStatus("tightestHPSWP",0);
  currentTree->SetBranchStatus("visibleTauMass",0);
  //currentTree->SetBranchStatus("isTauLegMatched",0);
  currentTree->SetBranchStatus("isMuLegMatched",0);
  //currentTree->SetBranchStatus("diTauCharge",0);
  //currentTree->SetBranchStatus("tauXTriggers",0);

  //std::vector<double>* jetsBtagHE;
 
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  int tightestHPSWP; 
  float  diTauCharge, MtLeg1; 
  float numPV;
  float weight;
  int isTauLegMatched;

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets        = new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsP4 = new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauVisP4  =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauVisP4",&diTauVisP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauICAP4 =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  //currentTree->SetBranchAddress("diTauICAP4",&diTauICAP4);
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfit3P4 =  new std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  currentTree->SetBranchAddress("diTauSVfit3P4",&diTauSVfit3P4);
  std::vector< int >* tauXTriggers =  new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",&tauXTriggers);

  //currentTree->SetBranchAddress("jetsBtagHE",&jetsBtagHE);

  currentTree->SetBranchAddress("chIsoLeg1",&chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1",&nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1",&phIsoLeg1);
  currentTree->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);
  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);
  currentTree->SetBranchAddress("numPV",&numPV);
  currentTree->SetBranchAddress("isTauLegMatched",&isTauLegMatched);

  if(crossSec[index]<0) currentTree->SetBranchAddress("weight",&weight);


  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);
    if(n%1000==0) cout << n << endl;
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
    diTauVisPt=-99;diTauVisEta=-99;diTauCAPt=-99;diTauCAEta=-99;
    diTauSVFitMass = -99;diTauSVFitPt=-99;diTauSVFitEta=-99;diTauVisMass=-99;
    ptL1=-99;ptL2=-99;etaL1=-99;etaL2=-99;diTauCharge_=-99;MtLeg1_=-99;
    tightestHPSWP_=-99;numPV_=-99;combRelIsoLeg1=-99;sampleWeight=-99;
    ptVeto=-99;HLT=-99;isTauLegMatched_=-99;

    if(jets->size()>1 && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2 && (*jets)[0].Eta()*(*jets)[1].Eta()<0 ){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi()  :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) ;
      Mjj  = ((*jets)[0]+(*jets)[1]).M();

      for(unsigned k=0; k < jets->size(); k++){
	if(k>1 && 
	   (  ((*jets)[k].Eta()>(*jets)[1].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[0].Eta()-0.5) || 
	      ((*jets)[k].Eta()>(*jets)[0].Eta()+0.5 &&  (*jets)[k].Eta()<(*jets)[1].Eta()-0.5) ) && (*jets)[k].Et()>ptVeto) ptVeto=(*jets)[k].Et();  
      }

      diTauVisMass  = (*diTauVisP4)[0].M();
      diTauVisPt  = (*diTauVisP4)[0].Pt();
      diTauVisEta = (*diTauVisP4)[0].Eta();
      //diTauCAPt  = (*diTauICAP4)[0].Pt();
      //diTauCAEta = (*diTauICAP4)[0].Eta();
      diTauSVFitPt  = (*diTauSVfit3P4)[0].Pt();
      diTauSVFitEta = (*diTauSVfit3P4)[0].Eta();
      diTauSVFitMass = (*diTauSVfit3P4)[0].M();
      //jetsBtagHE1 = (*jetsBtagHE)[0];
      //jetsBtagHE2 = (*jetsBtagHE)[1];
      ptL1  = (*diTauLegsP4)[0].Pt();
      ptL2  = (*diTauLegsP4)[1].Pt();
      etaL1 = (*diTauLegsP4)[0].Eta();
      etaL2 = (*diTauLegsP4)[1].Eta();
      diTauCharge_ = diTauCharge;
      MtLeg1_ = MtLeg1;
      tightestHPSWP_ = tightestHPSWP;
      numPV_ = numPV;
      combRelIsoLeg1 = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
      sampleWeight = (scaleFactor>0) ? scaleFactor : weight;
      HLT=float((*tauXTriggers)[0]);  
      isTauLegMatched_ = isTauLegMatched;

      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete diTauLegsP4; delete diTauVisP4; delete diTauICAP4; delete diTauSVfit3P4; delete tauXTriggers;
  
}



void doAllSamplesElec(){
 
  for( unsigned int k = 0; k < 12 ; k++)  makeTrees_ElecTauStream(k);

  return;

}

void doAllSamplesMu(){
 
  for( unsigned int k = 0; k < 11 ; k++)  makeTrees_MuTauStream(k);

  return;

}
