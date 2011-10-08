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
#include <algorithm>

#define SAVE   true
#define MINEt 30.0
#define MINPt1 25.0 //20
#define MINPt2 20.0 //15

using namespace ROOT::Math;
using namespace std;


void resetBranchesLeptTau(TTree* tree,  Int_t isMu = 1){

  tree->SetBranchStatus("jetsIDP4",1);
  tree->SetBranchStatus("diTauVisP4",0);
  tree->SetBranchStatus("diTauCAP4",0);
  tree->SetBranchStatus("diTauICAP4",0);
  tree->SetBranchStatus("diTauSVfit1P4",0);
  tree->SetBranchStatus("diTauSVfit2P4",0);
  tree->SetBranchStatus("diTauSVfit3P4",0);
  tree->SetBranchStatus("diTauLegsP4",1);
  tree->SetBranchStatus("genDiTauLegsP4",0);
  tree->SetBranchStatus("METP4",0);
  tree->SetBranchStatus("genMETP4",0);
  tree->SetBranchStatus("jetsP4",0);
  tree->SetBranchStatus("genJetsIDP4",0);
  tree->SetBranchStatus("jetsBtagHE",0);
  tree->SetBranchStatus("jetsBtagHP",0);
  tree->SetBranchStatus("sumEt",0);
  tree->SetBranchStatus("MtLeg1",  0);
  tree->SetBranchStatus("pZeta",   0);
  tree->SetBranchStatus("pZetaVis",0);
  tree->SetBranchStatus("chIsoLeg1",1);
  tree->SetBranchStatus("nhIsoLeg1",1);
  tree->SetBranchStatus("phIsoLeg1",1);
  tree->SetBranchStatus("chIsoPULeg1",0);
  tree->SetBranchStatus("nhIsoPULeg1",0);
  tree->SetBranchStatus("phIsoPULeg1",0);
  tree->SetBranchStatus("rhoFastJet",0);
  tree->SetBranchStatus("chIsoLeg2",0);
  tree->SetBranchStatus("nhIsoLeg2",0);
  tree->SetBranchStatus("phIsoLeg2",0);
  tree->SetBranchStatus("dxy1",0);
  tree->SetBranchStatus("dxy2",0);
  tree->SetBranchStatus("dz1",0);
  tree->SetBranchStatus("dz2",0);
  tree->SetBranchStatus("run",0);
  tree->SetBranchStatus("event",0);
  tree->SetBranchStatus("lumi",0);
  tree->SetBranchStatus("mcPUweight",1);
  tree->SetBranchStatus("numPV",0);
  tree->SetBranchStatus("numOfDiTaus",0);
  tree->SetBranchStatus("decayMode",0);
  tree->SetBranchStatus("tightestHPSWP",1);
  tree->SetBranchStatus("visibleTauMass",0);
  tree->SetBranchStatus("isTauLegMatched",1);
  if(!isMu){
    tree->SetBranchStatus("isElecLegMatched",0);
    tree->SetBranchStatus("elecFlag",1);
  }
  else{
    tree->SetBranchStatus("isMuLegMatched",0);
    tree->SetBranchStatus("muFlag",1);
  }
  tree->SetBranchStatus("diTauCharge",1);
  tree->SetBranchStatus("tauXTriggers",0);
  tree->SetBranchStatus("triggerBits",0);
  tree->SetBranchStatus("leadPFChargedHadrCandTrackPt",0);
}

void resetBranchesLeptLept(TTree* tree, Int_t isMu = 1){

  //tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("jetsIDP4",1);
  tree->SetBranchStatus("jetsP4",0);
  tree->SetBranchStatus("genJetsIDP4",0);
  if(!isMu){
    tree->SetBranchStatus("diElectronLegsP4",1);
    tree->SetBranchStatus("extraElectrons",0);
  }
  else{
    tree->SetBranchStatus("diMuonLegsP4",1);
    tree->SetBranchStatus("extraMuons",0);
  }
  tree->SetBranchStatus("METP4",0);
  tree->SetBranchStatus("jetsBtagHE",0);
  tree->SetBranchStatus("jetsBtagHP",0);
  tree->SetBranchStatus("sumEt",0);
  tree->SetBranchStatus("MtLeg1",  0);
  tree->SetBranchStatus("MtLeg2",  0);
  tree->SetBranchStatus("Zmass",  0);
  tree->SetBranchStatus("isLegFromTau",  0);
  tree->SetBranchStatus("chIsoLeg1",1);
  tree->SetBranchStatus("nhIsoLeg1",1);
  tree->SetBranchStatus("phIsoLeg1",1);
  tree->SetBranchStatus("chIsoPULeg1",0);
  tree->SetBranchStatus("nhIsoPULeg1",0);
  tree->SetBranchStatus("phIsoPULeg1",0);
  tree->SetBranchStatus("rhoFastJet",0);
  tree->SetBranchStatus("chIsoLeg2",0);
  tree->SetBranchStatus("nhIsoLeg2",0);
  tree->SetBranchStatus("phIsoLeg2",0);
  tree->SetBranchStatus("chIsoPULeg2",0);
  tree->SetBranchStatus("nhIsoPULeg2",0);
  tree->SetBranchStatus("phIsoPULeg2",0);
  tree->SetBranchStatus("dxy1",0);
  tree->SetBranchStatus("dxy2",0);
  tree->SetBranchStatus("dz1",0);
  tree->SetBranchStatus("dz2",0);
  tree->SetBranchStatus("run",0);
  tree->SetBranchStatus("event",0);
  tree->SetBranchStatus("lumi",0);
  tree->SetBranchStatus("mcPUweight",1);
  tree->SetBranchStatus("numPV",0);
  tree->SetBranchStatus("triggerBits",0);
}


void plot(Int_t doMuon_ = 1, Int_t doSameStream_ = 0){

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;


  TLegend* leg = new TLegend(0.55,0.68,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
 
  TFile* fEleTau = new TFile("/data_CMS//cms/lbianchini/ElecTauStream2011_iter2/treeElecTauStream_DYJets-50-madgraph-PUS1.root");
  TFile* fMuTau  = new TFile("/data_CMS//cms/lbianchini/MuTauStream2011_iter2/treeMuTauStream_DYJets-Mu-50-madgraph-PUS1.root");
  TFile* fEleEle = new TFile("/data_CMS//cms/lbianchini/ZelelPlusJetsSpring11/treeZelelPlusJets_DYJets-50-madgraph-PUS1-Zjets.root");
  TFile* fMuMu   = new TFile("/data_CMS//cms/lbianchini/ZmumuPlusJetsSpring11/treeZmumuPlusJets_DYJets-Mu-50-madgraph-PUS1-Zjets.root");

  TTree* tEleTau = 0;
  TTree* tEleEle = 0;

  if(doMuon_ && !doSameStream_){
   tEleTau  = (TTree*)fMuTau->Get("muTauStreamAnalyzer/tree") ;
   tEleEle  = (TTree*)fMuMu->Get("zPlusJetsAnalyzer/tree") ; 
  }
  if(!doMuon_ && !doSameStream_){
   tEleTau  = (TTree*)fEleTau->Get("elecTauStreamAnalyzer/tree");
   tEleEle  = (TTree*)fEleEle->Get("zPlusJetsAnalyzer/tree");
  }
  if(doMuon_ && doSameStream_){
   tEleTau  = (TTree*)fEleTau->Get("elecTauStreamAnalyzer/tree");
   tEleEle  = (TTree*)fMuTau->Get("muTauStreamAnalyzer/tree") ;
  }
  if(!doMuon_ && doSameStream_){
   tEleTau  = (TTree*)fEleEle->Get("zPlusJetsAnalyzer/tree");
   tEleEle  = (TTree*)fMuMu->Get("zPlusJetsAnalyzer/tree") ; 
  }


  int nEntriesEleTau = tEleTau->GetEntries() ;
  int nEntriesEleEle = tEleEle->GetEntries() ;

  TH1F* hMult   = new TH1F("hMult"," ; exclusive number of jets E_{T}>30 GeV ; A.U.",8,0,8);
  TH1F* hPt1    = new TH1F("hPt1"," ; leading E_{T} ; A.U.",40,0,200);
  TH1F* hPt2    = new TH1F("hPt2"," ; second leading E_{T} ; A.U.",40,0,200);
  TH1F* hDeta   = new TH1F("hDeta"," ; |#Delta#eta| ; A.U.",20,0,8);
  TH1F* hMjj    = new TH1F("hMjj"," ; M_{jj} ; A.U.",30,0,1200);

  TH1F* hMultEleTau  = (TH1F*)hMult->Clone("hMultEleTau");
  TH1F* hMultEleEle  = (TH1F*)hMult->Clone("hMultEleEle");

  TH1F* hPt1EleTau  = (TH1F*)hPt1->Clone("hPt1EleTau");
  TH1F* hPt1EleEle  = (TH1F*)hPt1->Clone("hPt1EleEle");

  TH1F* hPt2EleTau  = (TH1F*)hPt2->Clone("hPt2EleTau");
  TH1F* hPt2EleEle  = (TH1F*)hPt2->Clone("hPt2EleEle");

  TH1F* hDetaEleTau  = (TH1F*)hDeta->Clone("hDetaEleTau");
  TH1F* hDetaEleEle  = (TH1F*)hDeta->Clone("hDetaEleEle");

  TH1F* hMjjEleTau  = (TH1F*)hMjj->Clone("hMjjEleTau");
  TH1F* hMjjEleEle  = (TH1F*)hMjj->Clone("hMjjEleEle");

  if(!doMuon_ && !doSameStream_) {
    resetBranchesLeptTau(tEleTau,0);
    resetBranchesLeptLept(tEleEle,0);
  }
  if(doMuon_ && !doSameStream_) {
    resetBranchesLeptTau(tEleTau,1);
    resetBranchesLeptLept(tEleEle,1);
  }
  if(doMuon_ && doSameStream_) {
    resetBranchesLeptTau(tEleTau,0);
    resetBranchesLeptTau(tEleEle,1);
  }
  if(!doMuon_ && doSameStream_) {
    resetBranchesLeptLept(tEleTau,0);
    resetBranchesLeptLept(tEleEle,1);
  }

  std::vector< LV >* jetsEleTau = new std::vector< LV >();
  std::vector< LV >* jetsEleEle = new std::vector< LV >();
  std::vector< LV >* diObjectsEleTau  = new std::vector< LV >();
  std::vector< LV >* diObjectsEleEle  = new std::vector< LV >();

  tEleTau->SetBranchAddress("jetsIDP4",   &jetsEleTau);
  tEleEle->SetBranchAddress("jetsIDP4",   &jetsEleEle);

  TBranch *brDiObjectsEleTau    = 0;
  TBranch *brDiObjectsEleEle    = 0;

  if(!doMuon_ && !doSameStream_) {
    tEleTau->SetBranchAddress("diTauLegsP4",   &diObjectsEleTau);
    tEleEle->SetBranchAddress("diElectronLegsP4",   &diObjectsEleEle);
    brDiObjectsEleTau =  tEleTau->GetBranch("diTauLegsP4");
    brDiObjectsEleEle =  tEleEle->GetBranch("diElectronLegsP4");
  }
  if(doMuon_ && !doSameStream_) {
    tEleTau->SetBranchAddress("diTauLegsP4",   &diObjectsEleTau);
    tEleEle->SetBranchAddress("diMuonLegsP4",   &diObjectsEleEle);
    brDiObjectsEleTau =  tEleTau->GetBranch("diTauLegsP4");
    brDiObjectsEleEle =  tEleEle->GetBranch("diMuonLegsP4");
  }
  if(doMuon_ && doSameStream_) {
    tEleTau->SetBranchAddress("diTauLegsP4",   &diObjectsEleTau);
    tEleEle->SetBranchAddress("diTauLegsP4",   &diObjectsEleEle);
    brDiObjectsEleTau =  tEleTau->GetBranch("diTauLegsP4");
    brDiObjectsEleEle =  tEleEle->GetBranch("diTauLegsP4");
  }
  if(!doMuon_ && doSameStream_) {
    tEleTau->SetBranchAddress("diElectronLegsP4",   &diObjectsEleTau);
    tEleEle->SetBranchAddress("diMuonLegsP4",   &diObjectsEleEle);
    brDiObjectsEleTau =  tEleTau->GetBranch("diElectronLegsP4");
    brDiObjectsEleEle =  tEleEle->GetBranch("diMuonLegsP4");
  }

  int tightestHPSWP,isTauLegMatched;
  float diTauCharge;
  float mcPUweightEleTau,mcPUweightEleEle;
  float chIsoLeg1, nhIsoLeg1, phIsoLeg1;

  tEleTau->SetBranchAddress("mcPUweight",   &mcPUweightEleTau);
  tEleTau->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
  tEleTau->SetBranchAddress("diTauCharge",  &diTauCharge);
  tEleTau->SetBranchAddress("chIsoLeg1",  &chIsoLeg1);
  tEleTau->SetBranchAddress("nhIsoLeg1",  &nhIsoLeg1);
  tEleTau->SetBranchAddress("phIsoLeg1",  &phIsoLeg1);

  TBranch *brHPSEleTau    = 0;
  TBranch *brMCEleTau    = 0;

  if(doMuon_){
    tEleTau->SetBranchAddress("muFlag",&isTauLegMatched);
    brMCEleTau     = tEleTau->GetBranch("muFlag");
  }
  else{
    tEleTau->SetBranchAddress("elecFlag",&isTauLegMatched);
    brMCEleTau     = tEleTau->GetBranch("elecFlag");
  }

  tEleEle->SetBranchAddress("mcPUweight",&mcPUweightEleEle);

  TBranch *brJetsEleTau   = tEleTau->GetBranch("jetsIDP4");
  TBranch *brWeightEleTau = tEleTau->GetBranch("mcPUweight");
  TBranch *brChargeEleTau = 0;
  TBranch *brChIsoEleTau    =  tEleTau->GetBranch("chIsoLeg1");
  TBranch *brNhIsoEleTau    =  tEleTau->GetBranch("nhIsoLeg1");
  TBranch *brPhIsoEleTau    =  tEleTau->GetBranch("phIsoLeg1");


  if( !(!doMuon_ && doSameStream_) ){
    brChargeEleTau = tEleTau->GetBranch("diTauCharge");
    brHPSEleTau    = tEleTau->GetBranch("tightestHPSWP");
  }

  for (int n = 0; n < nEntriesEleTau ; n++) {
 
    //tEleTau->GetEntry(n);
    brJetsEleTau->GetEntry(n);
    brDiObjectsEleTau->GetEntry(n);
    brWeightEleTau->GetEntry(n);
    brChIsoEleTau->GetEntry(n);
    brNhIsoEleTau->GetEntry(n);
    brPhIsoEleTau->GetEntry(n);

    if( !(!doMuon_ && doSameStream_) ){
      brChargeEleTau->GetEntry(n);
      brHPSEleTau->GetEntry(n);
      brMCEleTau->GetEntry(n);
    }

    cout << chIsoLeg1 << "  " << (*diObjectsEleTau)[0].Pt() << "  " << isTauLegMatched << "  " << tightestHPSWP << endl;

    if(n%1000==0) cout << n << endl;

    if(!(!doMuon_ && doSameStream_) && (tightestHPSWP<1 || diTauCharge!=0 || isTauLegMatched!=0 || (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diObjectsEleTau)[0].Pt()>0.1 )) continue;

    int counter = 0;
    for(unsigned int it = 0; it<jetsEleTau->size(); it++){
      if( (*jetsEleTau)[it].Et()>MINEt ) counter++;
    }
    hMultEleTau->Fill(counter,mcPUweightEleTau);
    if(jetsEleTau->size()>0) hPt1EleTau->Fill((*jetsEleTau)[0].Et(),mcPUweightEleTau);
    if(jetsEleTau->size()>1) hPt2EleTau->Fill((*jetsEleTau)[1].Et(),mcPUweightEleTau);
    if(jetsEleTau->size()>1 && (*jetsEleTau)[0].Et()>MINPt1 && (*jetsEleTau)[1].Et()>MINPt2 && (*jetsEleTau)[0].Eta()*(*jetsEleTau)[1].Eta()<0 ){
      hDetaEleTau->Fill( abs((*jetsEleTau)[1].Eta()-(*jetsEleTau)[0].Eta()) ,mcPUweightEleTau);
      hMjjEleTau->Fill( ((*jetsEleTau)[1]+(*jetsEleTau)[0]).M() ,mcPUweightEleTau);
    }
  }

  TBranch *brJetsEleEle   = tEleEle->GetBranch("jetsIDP4");
  TBranch *brWeightEleEle = tEleEle->GetBranch("mcPUweight");
  TBranch *brChIsoEleEle    =  tEleEle->GetBranch("chIsoLeg1");
  TBranch *brNhIsoEleEle    =  tEleEle->GetBranch("nhIsoLeg1");
  TBranch *brPhIsoEleEle    =  tEleEle->GetBranch("phIsoLeg1");
  TBranch *brChargeEleEle = 0;
  TBranch *brHPSEleEle    = 0;
  TBranch *brMCEleEle   = 0;

  if( doMuon_ && doSameStream_ ){
    tEleEle->SetBranchAddress("tightestHPSWP",&tightestHPSWP);
    tEleEle->SetBranchAddress("diTauCharge",  &diTauCharge);
    tEleEle->SetBranchAddress("isTauLegMatched",  &isTauLegMatched);
    brChargeEleEle = tEleEle->GetBranch("diTauCharge");
    brHPSEleEle    = tEleEle->GetBranch("tightestHPSWP");
    //brMCEleEle     = tEleEle->GetBranch("isTauLegMatched");
    brMCEleEle     = tEleEle->GetBranch("muFlag");
  }

  for (int n = 0; n < nEntriesEleEle ; n++) {

    brJetsEleEle->GetEntry(n);
    brWeightEleEle->GetEntry(n);
    brDiObjectsEleEle->GetEntry(n);
    brChIsoEleEle->GetEntry(n);
    brNhIsoEleEle->GetEntry(n);
    brPhIsoEleEle->GetEntry(n);

    if( doMuon_ && doSameStream_ ){
      brChargeEleEle->GetEntry(n);
      brHPSEleEle->GetEntry(n);
      brMCEleEle->GetEntry(n);
    }

    //tEleEle->GetEntry(n);

    if(n%1000==0) cout << n << endl;

    if(doMuon_ && doSameStream_  && (tightestHPSWP<1 || diTauCharge!=0 || isTauLegMatched!=0)) continue;
    if(!(doMuon_ && doSameStream_ ) && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diObjectsEleTau)[0].Pt()>0.1 ) continue;

    int counter = 0;
    for(unsigned int it = 0; it<jetsEleEle->size(); it++){
      if( (*jetsEleEle)[it].Et()>MINEt ) counter++;
    }
    hMultEleEle->Fill(counter,mcPUweightEleEle);
    if(jetsEleEle->size()>0) hPt1EleEle->Fill((*jetsEleEle)[0].Et(),mcPUweightEleEle);
    if(jetsEleEle->size()>1) hPt2EleEle->Fill((*jetsEleEle)[1].Et(),mcPUweightEleEle);
    if(jetsEleEle->size()>1 && (*jetsEleEle)[0].Et()>MINPt1 && (*jetsEleEle)[1].Et()>MINPt2 && (*jetsEleEle)[0].Eta()*(*jetsEleEle)[1].Eta()<0 ){
      hDetaEleEle->Fill( abs((*jetsEleEle)[1].Eta()-(*jetsEleEle)[0].Eta()) ,mcPUweightEleEle);
      hMjjEleEle->Fill( ((*jetsEleEle)[1]+(*jetsEleEle)[0]).M() ,mcPUweightEleEle);
    }
  }
  

  hMultEleTau->SetLineWidth(2);
  hMultEleTau->SetLineColor(kRed);
  hMultEleTau->SetLineStyle(kDashed);
  hPt1EleTau->SetLineWidth(2);
  hPt1EleTau->SetLineColor(kRed);
  hPt1EleTau->SetLineStyle(kDashed);
  hPt2EleTau->SetLineWidth(2);
  hPt2EleTau->SetLineColor(kRed);
  hPt2EleTau->SetLineStyle(kDashed);
  hDetaEleTau->SetLineWidth(2);
  hDetaEleTau->SetLineColor(kRed);
  hDetaEleTau->SetLineStyle(kDashed);
  hMjjEleTau->SetLineWidth(2);
  hMjjEleTau->SetLineColor(kRed);
  hMjjEleTau->SetLineStyle(kDashed);

  hMultEleEle->SetLineWidth(2);
  hMultEleEle->SetLineColor(kBlue);
  hMultEleEle->SetLineStyle(kSolid);
  hPt1EleEle->SetLineWidth(2);
  hPt1EleEle->SetLineColor(kBlue);
  hPt1EleEle->SetLineStyle(kSolid);
  hPt2EleEle->SetLineWidth(2);
  hPt2EleEle->SetLineColor(kBlue);
  hPt2EleEle->SetLineStyle(kSolid);
  hDetaEleEle->SetLineWidth(2);
  hDetaEleEle->SetLineColor(kBlue);
  hDetaEleEle->SetLineStyle(kSolid);
  hMjjEleEle->SetLineWidth(2);
  hMjjEleEle->SetLineColor(kBlue);
  hMjjEleEle->SetLineStyle(kSolid);

  if(doMuon_ && !doSameStream_){
    leg->AddEntry(hMultEleTau,"Z#rightarrow#tau#tau#rightarrow#mu+#tau","L");
    leg->AddEntry(hMultEleEle,"Z#rightarrow#mu+#mu","L");
  } else if(!doMuon_ && !doSameStream_){
    leg->AddEntry(hMultEleTau,"Z#rightarrow#tau#tau#rightarrowe+#tau","L");
    leg->AddEntry(hMultEleEle,"Z#rightarrowe+e","L");
  } else if(doMuon_ && doSameStream_){
    leg->AddEntry(hMultEleTau,"Z#rightarrow#tau#tau#rightarrowe+#tau","L");
    leg->AddEntry(hMultEleEle,"Z#rightarrow#tau#tau#rightarrow#mu+#tau","L");
  }
  else if(!doMuon_ && doSameStream_){
    leg->AddEntry(hMultEleTau,"Z#rightarrowe+e","L");
    leg->AddEntry(hMultEleEle,"Z#rightarrow#mu+#mu","L");
  }

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->SetLogy(1);
  c0->cd();
  hMultEleTau->Sumw2();
  hMultEleTau->DrawNormalized("E");
  hMultEleEle->DrawNormalized("HISTSAME");
  leg->Draw();
  if(doMuon_) gPad->SaveAs("plots_MuTauStream/May19/jetMutlTauVsLep_Mu.pdf");
  else gPad->SaveAs("plots_ElecTauStream/May19/jetMutlTauVsLep_Elec.pdf");
  c0->Draw();

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(1);
  c1->cd();
  hPt1EleTau->Sumw2();
  hPt1EleTau->DrawNormalized("E");
  hPt1EleEle->DrawNormalized("HISTSAME");
  leg->Draw();
  if(doMuon_) gPad->SaveAs("plots_MuTauStream/May19/jetEt1TauVsLep_Mu.pdf");
  else gPad->SaveAs("plots_ElecTauStream/May19/jetEt1TauVsLep_Elec.pdf");
  c1->Draw();

  TCanvas *c2 = new TCanvas("c2","",5,30,650,600);
  c2->SetGrid(0,0);
  c2->SetFillStyle(4000);
  c2->SetFillColor(10);
  c2->SetTicky();
  c2->SetObjectStat(0);
  c2->SetLogy(1);
  c2->cd();
  hPt2EleTau->Sumw2();
  hPt2EleTau->DrawNormalized("E");
  hPt2EleEle->DrawNormalized("HISTSAME");
  leg->Draw();
  if(doMuon_) gPad->SaveAs("plots_MuTauStream/May19/jetEt2TauVsLep_Mu.pdf");
  else gPad->SaveAs("plots_ElecTauStream/May19/jetEt2TauVsLep_Elec.pdf");
  c2->Draw();

  TCanvas *c3 = new TCanvas("c3","",5,30,650,600);
  c3->SetGrid(0,0);
  c3->SetFillStyle(4000);
  c3->SetFillColor(10);
  c3->SetTicky();
  c3->SetObjectStat(0);
  c3->SetLogy(0);
  c3->cd();
  hDetaEleTau->Sumw2();
  hDetaEleTau->DrawNormalized("E");
  hDetaEleEle->DrawNormalized("HISTSAME");
  leg->Draw();
  if(doMuon_) gPad->SaveAs("plots_MuTauStream/May19/jetDetaTauVsLep_Mu.pdf");
  else gPad->SaveAs("plots_ElecTauStream/May19/jetDetaTauVsLep_Elec.pdf");
  c3->Draw();

  TCanvas *c4 = new TCanvas("c4","",5,30,650,600);
  c4->SetGrid(0,0);
  c4->SetFillStyle(4000);
  c4->SetFillColor(10);
  c4->SetTicky();
  c4->SetObjectStat(0);
  c4->SetLogy(1);
  c4->cd();
  hMjjEleTau->Sumw2();
  hMjjEleTau->DrawNormalized("E");
  hMjjEleEle->DrawNormalized("HISTSAME");
  leg->Draw();
  if(doMuon_) gPad->SaveAs("plots_MuTauStream/May19/jetMjjTauVsLep_Mu.pdf");
  else gPad->SaveAs("plots_ElecTauStream/May19/jetMjjTauVsLep_Elec.pdf");
  c4->Draw();

  delete jetsEleEle; delete jetsEleTau; delete diObjectsEleTau; delete diObjectsEleEle;

  return;

}



