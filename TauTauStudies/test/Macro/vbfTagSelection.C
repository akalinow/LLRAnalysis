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

#define SAVE true

using namespace ROOT::Math;
using namespace std;


void count(){

  // selection for tha tag jet:
  // 1- order the reconstructed jets by pT: at least 3 jets with pT>15
  // 2- t1 = j1 if pT(j1) > X
  // 3- t2 = if(j2 if pT(j2) > Y && |dEta(j1,j2)|> Z)      ==> j2
  //         else if(j3 if pT(j3) > Y && |dEta(j1,j3)|> Z) ==> j3


  TFile *outFile = new TFile("nTupleVbf.root","RECREATE");
  TTree* outTreePtOrd    = new TTree("outTreePtOrd","tree with pT and eta of jets pT-ord");
  TTree* outTreePtEtaOrd = new TTree("outTreePtEtaOrd","tree with pT and eta of jets pT-eta ord");

  double pt1,pt2,eta1,eta2;
  outTreePtOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/D");
  outTreePtEtaOrd->Branch("pt1", &pt1,"pt1/D");
  outTreePtEtaOrd->Branch("pt2", &pt2,"pt2/D");
  outTreePtEtaOrd->Branch("eta1", &eta1,"eta1/D");
  outTreePtEtaOrd->Branch("eta2", &eta2,"eta2/D");
 

  TFile* file   = new TFile("../vbfTree.root","READ");
  TTree* currentTree = (TTree*)file->Get("vbfJetAnalyzer/tree");
  int nEntries = currentTree->GetEntries() ;

  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jets;
  std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjets;
  currentTree->SetBranchAddress("jetsP4",   &jets);
  currentTree->SetBranchAddress("tagjetsP4",&tagjets);

  for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;

    if(jets->size()>1){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      outTreePtOrd->Fill();
      continue;
    }
    
    outTreePtOrd->Fill();
  }


 for (int n = 0; n < nEntries ; n++) {
    currentTree->GetEntry(n);

    pt1=-99;pt2=-99;eta1=-99,eta2=-99;

    if(jets->size()>1 && abs((*jets)[0].Eta()-(*jets)[1].Eta())>1.4 ){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[1].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[1].Eta();
      outTreePtEtaOrd->Fill();
      continue;
    } else if(jets->size()>2 && abs((*jets)[0].Eta()-(*jets)[2].Eta())>1.4 ){
      pt1  = (*jets)[0].Pt();
      pt2  = (*jets)[2].Pt();
      eta1 = (*jets)[0].Eta();
      eta2 = (*jets)[2].Eta();
      outTreePtEtaOrd->Fill();
      continue;
    }
    
    outTreePtEtaOrd->Fill();
  }
 file->Close();

 //outFile->cd();
 //outTreePtOrd->Write();
 //outTreePtEtaOrd->Write();
 outFile->Write();
 outFile->Close();
  
}
