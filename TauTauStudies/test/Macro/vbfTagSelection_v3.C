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




void makeZJetsTrees(){


  TFile *outFile = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets_2015.root","RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
  TTree* outTreePtEtaOrd  = new TTree("outTreePtEtaOrd","tree jets pT-eta ord");
  TTree* outTreePtDEtaOrd = new TTree("outTreePtDEtaOrd","tree jets pT-Deta ord");
  TTree* outTreePtMjjOrd  = new TTree("outTreePtMjjOrd","tree jets pT-Mjj ord");

  double pt1,pt2,eta1,eta2,Deta,Mjj, Dphi ;
  outTreePtOrd->Branch("pt1",  &pt1,"pt1/D");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/D");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtEtaOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtEtaOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtDEtaOrd->Branch("pt1",  &pt1,"pt1/D");
  outTreePtDEtaOrd->Branch("pt2",  &pt2,"pt2/D");
  outTreePtDEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtDEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtDEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtDEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtDEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtMjjOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtMjjOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtMjjOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtMjjOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtMjjOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtMjjOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtMjjOrd->Branch("Mjj",  &Mjj,"Mjj/D");
 

  TFile* file   = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/zjetsTree.root","READ");
  TTree* currentTree = (TTree*)file->Get("zPlusJetsAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  currentTree->SetBranchAddress("jetsIDP4",   &jets);

  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    if(jets->size()>1 && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    if(jets->size()>1 && abs((*jets)[0].Eta()-(*jets)[1].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtEtaOrd->Fill();
      continue;
    } else if(jets->size()>2 && abs((*jets)[0].Eta()-(*jets)[2].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[2].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[2].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[2].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[2].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[2].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[2].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[2]).M();
      outTreePtEtaOrd->Fill();
      continue;
    }
    outTreePtEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

   double Deta_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && abs((*jets)[j].Eta()-(*jets)[i].Eta()) > Deta_tmp){
	 Deta_tmp = abs((*jets)[j].Eta()-(*jets)[i].Eta());
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtDEtaOrd->Fill();
     continue;
   }

    outTreePtDEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
   
   double Mjj_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && ((*jets)[j]+(*jets)[i]).M() > Mjj_tmp){
	 Mjj_tmp = ((*jets)[j]+(*jets)[i]).M();
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtMjjOrd->Fill();
     continue;
   }

   outTreePtMjjOrd->Fill();
  }




 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();
  
}




void makeVbfTrees(){

  // selection for tha tag jet:
  // 1- order the reconstructed jets by pT: at least 3 jets with pT>15
  // 2- t1 = j1 if pT(j1) > X
  // 3- t2 = if(j2 if pT(j2) > Y && |dEta(j1,j2)|> Z)      ==> j2
  //         else if(j3 if pT(j3) > Y && |dEta(j1,j3)|> Z) ==> j3


  TFile *outFile = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleVbf.root","RECREATE");  
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
  TTree* outTreePtEtaOrd  = new TTree("outTreePtEtaOrd","tree jets pT-eta ord");
  TTree* outTreePtDEtaOrd = new TTree("outTreePtDEtaOrd","tree jets pT-Deta ord");
  TTree* outTreePtMjjOrd  = new TTree("outTreePtMjjOrd","tree jets pT-Mjj ord");

  double pt1,pt2,eta1,eta2,Deta,Mjj, Dphi ;
  outTreePtOrd->Branch("pt1",  &pt1,"pt1/D");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/D");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtEtaOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtEtaOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtDEtaOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtDEtaOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtDEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtDEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtDEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtDEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtDEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtMjjOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtMjjOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtMjjOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtMjjOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtMjjOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtMjjOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtMjjOrd->Branch("Mjj",  &Mjj,"Mjj/D");
 

  TFile* file   = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/vbfTree.root","READ");
  TTree* currentTree = (TTree*)file->Get("vbfJetAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;


  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  currentTree->SetBranchAddress("jetsP4",   &jets); //jetBranchName.c_str()
  //std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjets;
  //currentTree->SetBranchAddress("tagjetsP4",&tagjets);


  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    if(jets->size()>1 && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    if(jets->size()>1 && abs((*jets)[0].Eta()-(*jets)[1].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtEtaOrd->Fill();
      continue;
    } else if(jets->size()>2 && abs((*jets)[0].Eta()-(*jets)[2].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[2].Et()>MINPt2){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[2].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[2].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[2].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[2].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[2].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[2]).M();
      outTreePtEtaOrd->Fill();
      continue;
    }
    outTreePtEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

   double Deta_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && abs((*jets)[j].Eta()-(*jets)[i].Eta()) > Deta_tmp){
	 Deta_tmp = abs((*jets)[j].Eta()-(*jets)[i].Eta());
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtDEtaOrd->Fill();
     continue;
   }

    outTreePtDEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
   
   double Mjj_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && ((*jets)[j]+(*jets)[i]).M() > Mjj_tmp){
	 Mjj_tmp = ((*jets)[j]+(*jets)[i]).M();
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtMjjOrd->Fill();
     continue;
   }

   outTreePtMjjOrd->Fill();
  }




 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();
  
}



void makeQCDTrees(){


  TFile *outFile = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleVBF115.root","RECREATE");
  TTree* outTreePtOrd     = new TTree("outTreePtOrd","tree jets pT-ord");
  TTree* outTreePtEtaOrd  = new TTree("outTreePtEtaOrd","tree jets pT-eta ord");
  TTree* outTreePtDEtaOrd = new TTree("outTreePtDEtaOrd","tree jets pT-Deta ord");
  TTree* outTreePtMjjOrd  = new TTree("outTreePtMjjOrd","tree jets pT-Mjj ord");

  double pt1,pt2,eta1,eta2,Deta,Mjj, Dphi ;
  outTreePtOrd->Branch("pt1",  &pt1,"pt1/D");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/D");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtEtaOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtEtaOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtDEtaOrd->Branch("pt1",  &pt1,"pt1/D");
  outTreePtDEtaOrd->Branch("pt2",  &pt2,"pt2/D");
  outTreePtDEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtDEtaOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtDEtaOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtDEtaOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtDEtaOrd->Branch("Mjj",  &Mjj,"Mjj/D");

  outTreePtMjjOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtMjjOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtMjjOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtMjjOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtMjjOrd->Branch("Deta", &Deta,"Deta/D");
  outTreePtMjjOrd->Branch("Dphi", &Dphi,"Dphi/D");
  outTreePtMjjOrd->Branch("Mjj",  &Mjj,"Mjj/D");
 

  TFile* file   = new TFile("/data_CMS/cms/lbianchini/MuTauStream/NoMuIsoNoTauIsoNo2Tcuts/treeMuTauStream_VBFH115-PU-L.root","READ");
  TTree* currentTree = (TTree*)file->Get("muTauStreamAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  currentTree->SetBranchStatus("diTauVisP4",0);
  currentTree->SetBranchStatus("diTauCAP4",0);
  currentTree->SetBranchStatus("diTauICAP4",0);
  currentTree->SetBranchStatus("diTauSVfit1P4",0);
  currentTree->SetBranchStatus("diTauSVfit2P4",0);
  currentTree->SetBranchStatus("diTauSVfit3P4",0);
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
  currentTree->SetBranchStatus("numPV",0);
  currentTree->SetBranchStatus("numOfDiTaus",0);
  currentTree->SetBranchStatus("decayMode",0);
  //currentTree->SetBranchStatus("tightestHPSWP",0);
  currentTree->SetBranchStatus("visibleTauMass",0);
  currentTree->SetBranchStatus("isTauLegMatched",0);
  currentTree->SetBranchStatus("isMuLegMatched",0);
  currentTree->SetBranchStatus("isTauLegMatched",0);
  currentTree->SetBranchStatus("isMuLegMatched",0);
  //currentTree->SetBranchStatus("diTauCharge",0);

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsP4;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; int tightestHPSWP; float  diTauCharge,MtLeg1;

  currentTree->SetBranchAddress("jetsIDP4",   &jets);
  currentTree->SetBranchAddress("diTauLegsP4",&diTauLegsP4);
  currentTree->SetBranchAddress("chIsoLeg1",&chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1",&nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1",&phIsoLeg1);
  currentTree->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  currentTree->SetBranchAddress("diTauCharge",&diTauCharge);
  currentTree->SetBranchAddress("MtLeg1",&MtLeg1);


  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    bool eventSel = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt()<0.1 && MtLeg1<40 && diTauCharge==0;

    if(jets->size()>1 && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2 && eventSel){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;

    bool eventSel = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt()<0.1 && MtLeg1<40 && diTauCharge==0;
 
    if(jets->size()>1 && abs((*jets)[0].Eta()-(*jets)[1].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[1].Et()>MINPt2 && eventSel){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[1].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[1].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[1].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[1]).M();
      outTreePtEtaOrd->Fill();
      continue;
    } else if(jets->size()>2 && abs((*jets)[0].Eta()-(*jets)[2].Eta())>1.4  && (*jets)[0].Et()>MINPt1 && (*jets)[2].Et()>MINPt2 && eventSel){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[2].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[2].Eta();
      Deta = abs(eta1-eta2);
      Dphi = abs((*jets)[0].Phi()-(*jets)[2].Phi()) > TMath::Pi() ? 
	-abs( (*jets)[0].Phi()-(*jets)[2].Phi() ) + 2*TMath::Pi() :
	abs( (*jets)[0].Phi()-(*jets)[2].Phi() );
      Mjj  = ((*jets)[0]+(*jets)[2]).M();
      outTreePtEtaOrd->Fill();
      continue;
    }
    outTreePtEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
   bool eventSel = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt()<0.1 && MtLeg1<40 && diTauCharge==0;


   double Deta_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && abs((*jets)[j].Eta()-(*jets)[i].Eta()) > Deta_tmp){
	 Deta_tmp = abs((*jets)[j].Eta()-(*jets)[i].Eta());
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999  && eventSel ){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtDEtaOrd->Fill();
     continue;
   }

    outTreePtDEtaOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
   currentTree->GetEntry(n);
   
   pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;
   bool eventSel = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt()<0.1 && MtLeg1<40 && diTauCharge==0;

   double Mjj_tmp = -99; unsigned int lead=999 ; unsigned int trail=999;
   for(unsigned int i=0; i<jets->size(); i++){
     for(unsigned int j=0; j<jets->size(); j++){
       if(j>i && (*jets)[i].Et()>MINPt1 && (*jets)[j].Et()>MINPt2 && ((*jets)[j]+(*jets)[i]).M() > Mjj_tmp){
	 Mjj_tmp = ((*jets)[j]+(*jets)[i]).M();
	 lead = i;
	 trail = j;
       }
     }// j
   }// i
   
   if(lead!=999 && trail!=999   && eventSel){
     pt1  = (*jets)[lead].Pt();
     pt2  = (*jets)[trail].Pt();
     eta1 = (*jets)[lead].Eta();
     eta2 = (*jets)[trail].Eta();
     Deta = abs(eta1-eta2);
     Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
       -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi() :
       abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() );
     Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
     outTreePtMjjOrd->Fill();
     continue;
   }

   outTreePtMjjOrd->Fill();
  }




 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();
  
}
