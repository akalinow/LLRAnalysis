#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TCut.h"
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
#define MINEt 25.0
#define MINPt1 25.0 //20
#define MINPt2 25.0 //15

using namespace ROOT::Math;
using namespace std;



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
  tree->SetBranchStatus("Zmass",  1);
  tree->SetBranchStatus("isLegFromTau",  0);
  tree->SetBranchStatus("chIsoLeg1",1);
  tree->SetBranchStatus("nhIsoLeg1",1);
  tree->SetBranchStatus("phIsoLeg1",1);
  tree->SetBranchStatus("chIsoPULeg1",0);
  tree->SetBranchStatus("nhIsoPULeg1",0);
  tree->SetBranchStatus("phIsoPULeg1",0);
  tree->SetBranchStatus("rhoFastJet",0);
  tree->SetBranchStatus("chIsoLeg2",1);
  tree->SetBranchStatus("nhIsoLeg2",1);
  tree->SetBranchStatus("phIsoLeg2",1);
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
  tree->SetBranchStatus("triggerBits",1);
}


void plotEleEle(string variable_ = "Zmass", string labelX_ = "mass(#mu#mu)", Int_t nBins_ = 60, Float_t xMin_ = 60, Float_t xMax_ = 120 , Int_t applyPre_ = 0, Int_t applyVBF_ = 0, Int_t rescaleZpt_ = 1, Int_t scleToData_ = 1, Int_t Logy_ = 0){

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->SetLogy(Logy_);

  TLegend* leg = new TLegend(0.55,0.68,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  TFile* fData   = new TFile("/data_CMS//cms/lbianchini/ZelelPlusJetsSpring11/treeZelelPlusJets_Run2011-Ele-Zjets.root");
  TFile* fZJets  = new TFile("/data_CMS//cms/lbianchini/ZelelPlusJetsSpring11/treeZelelPlusJets_DYJets-50-madgraph-PUS1-Zjets.root");
  TFile* fWJets  = new TFile("/data_CMS//cms/lbianchini/ZelelPlusJetsSpring11/treeZelelPlusJets_WJets-madgraph-PUS1-Zjets.root");
  TFile* fTTbar  = new TFile("/data_CMS//cms/lbianchini/ZelelPlusJetsSpring11/treeZelelPlusJets_TTJets-madgraph-PUS1-Zjets.root");

  if( fData->IsZombie() || fZJets->IsZombie() || fWJets->IsZombie() || fTTbar->IsZombie()){
    cout << "No file" << endl;
    exit(1);
  }

  vector<string> samples;

  samples.push_back("Data");
  samples.push_back("WJets");
  samples.push_back("TTbar");
  samples.push_back("ZJets");

  std::map<std::string,float> crossSec;
  crossSec["TTbar"]=( 157.5 );
  crossSec["WJets"]=( 31314.0);
  crossSec["ZJets"]=( 3048  );
  crossSec["Data"]=( 0 );

  std::map<string, TFile*> aMap;
  aMap["TTbar"] = fTTbar;
  aMap["WJets"] = fWJets;
  aMap["ZJets"] = fZJets;
  aMap["Data"]  = fData;


  TH1F* hPtZMC    = new TH1F("hPtZMC"," ; leading E_{T} ; A.U.",100,0,400);
  TH1F* hPtZRD    = new TH1F("hPtZRD"," ; leading E_{T} ; A.U.",100,0,400);
  ((TTree*)fData->Get("zPlusJetsAnalyzer/tree"))->Draw("sqrt((diElectronLegsP4[0].Px()+diElectronLegsP4[1].Px())*(diElectronLegsP4[0].Px()+diElectronLegsP4[1].Px()) + (diElectronLegsP4[0].Py()+diElectronLegsP4[1].Py())*(diElectronLegsP4[0].Py()+diElectronLegsP4[1].Py()))>>hPtZRD","( ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diElectronLegsP4[0].Pt()<0.08 && abs(diElectronLegsP4[0].Eta())<1.5) || ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diElectronLegsP4[0].Pt()<0.04 && abs(diElectronLegsP4[0].Eta())<1.5)) && (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/diElectronLegsP4[1].Pt() < 0.3  && Zmass<100 && Zmass>80");
  ((TTree*)fZJets->Get("zPlusJetsAnalyzer/tree"))->Draw("sqrt((diElectronLegsP4[0].Px()+diElectronLegsP4[1].Px())*(diElectronLegsP4[0].Px()+diElectronLegsP4[1].Px()) + (diElectronLegsP4[0].Py()+diElectronLegsP4[1].Py())*(diElectronLegsP4[0].Py()+diElectronLegsP4[1].Py()))>>hPtZMC","mcPUweight*(( ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diElectronLegsP4[0].Pt()<0.08 && abs(diElectronLegsP4[0].Eta())<1.5) || ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diElectronLegsP4[0].Pt()<0.04 && abs(diElectronLegsP4[0].Eta())<1.5)) && (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/diElectronLegsP4[1].Pt() < 0.3  && Zmass<100 && Zmass>80)");

  hPtZMC->Scale(1./hPtZMC->Integral());
  hPtZRD->Scale(1./hPtZRD->Integral());

  TH1F* hRatio = (TH1F*)hPtZRD->Clone("hRatio"); 
  hRatio->Reset();
  hRatio->Divide(hPtZRD,hPtZMC,1.0,1.0);

  //hRatio->Draw();

  //TH1F* hPt2    = new TH1F("hPt2"," ; second leading E_{T} ; A.U.",40,0,200);
  //TH1F* hDeta   = new TH1F("hDeta"," ; |#Delta#eta| ; A.U.",20,0,8);
  //TH1F* hMjj    = new TH1F("hMjj"," ; M_{jj} ; A.U.",30,0,1200);

  float Lumi = 160.;

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi));
  TH1F* hData = new TH1F();


  for(unsigned int i = 0 ; i < samples.size() ; i++){

    TH1F* allEvents = (TH1F*)(aMap[samples[i]])->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    float scaleFactor = crossSec[samples[i]]>0 ? Lumi/(totalEvents/crossSec[samples[i]]) : 1 ;

    TTree* currentTree = (TTree*)(aMap[samples[i]])->Get("zPlusJetsAnalyzer/tree");
    resetBranchesLeptLept(currentTree,0);

    cout << samples[i] <<  "  " << currentTree->GetEntries() << endl;


    TH1F* h1   = new TH1F("h1",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1} ; %s ; Events/%.1f", Lumi, labelX_.c_str(), (xMax_-xMin_)/nBins_ ), nBins_, xMin_, xMax_);
    h1->SetName(samples[i].c_str());

    std::vector< LV >* jets       = new std::vector< LV >();
    std::vector< LV >* diObjects  = new std::vector< LV >();
    std::vector< int >* triggerBits  = new std::vector< int >();
    float mcPUweight; float Zmass;
    float chIsoLeg1, nhIsoLeg1, phIsoLeg1;
    float chIsoLeg2, nhIsoLeg2, phIsoLeg2;

    currentTree->SetBranchAddress("jetsIDP4", &jets);
    currentTree->SetBranchAddress("diElectronLegsP4", &diObjects);
    currentTree->SetBranchAddress("triggerBits", &triggerBits);
    currentTree->SetBranchAddress("mcPUweight", &mcPUweight);
    currentTree->SetBranchAddress("chIsoLeg1", &chIsoLeg1);
    currentTree->SetBranchAddress("nhIsoLeg1", &nhIsoLeg1);
    currentTree->SetBranchAddress("phIsoLeg1", &phIsoLeg1);
    currentTree->SetBranchAddress("chIsoLeg2", &chIsoLeg2);
    currentTree->SetBranchAddress("nhIsoLeg2", &nhIsoLeg2);
    currentTree->SetBranchAddress("phIsoLeg2", &phIsoLeg2);
    currentTree->SetBranchAddress("Zmass", &Zmass);


    TBranch *brjetsIDP4     = currentTree->GetBranch("jetsIDP4");
    TBranch *brdiElectronLegsP4 = currentTree->GetBranch("diElectronLegsP4");
    TBranch *brtriggerBits = currentTree->GetBranch("triggerBits");
    TBranch *brmcPUweight   = currentTree->GetBranch("mcPUweight");
    TBranch *brchIsoLeg1    = currentTree->GetBranch("chIsoLeg1");
    TBranch *brnhIsoLeg1    = currentTree->GetBranch("nhIsoLeg1");
    TBranch *brphIsoLeg1    = currentTree->GetBranch("phIsoLeg1");
    TBranch *brchIsoLeg2    = currentTree->GetBranch("chIsoLeg2");
    TBranch *brnhIsoLeg2    = currentTree->GetBranch("nhIsoLeg2");
    TBranch *brphIsoLeg2    = currentTree->GetBranch("phIsoLeg2");
    TBranch *brZmass        = currentTree->GetBranch("Zmass");

    int nEntries = currentTree->GetEntries();

    for(unsigned n = 0 ; n< nEntries ; n++){

      brjetsIDP4->GetEntry(n);
      brdiElectronLegsP4->GetEntry(n);
      //brtriggerBits->GetEntry(n);
      brmcPUweight->GetEntry(n);
      brchIsoLeg1->GetEntry(n);
      brnhIsoLeg1->GetEntry(n);
      brphIsoLeg1->GetEntry(n);
      brchIsoLeg2->GetEntry(n);
      brnhIsoLeg2->GetEntry(n);
      brphIsoLeg2->GetEntry(n);
      brZmass->GetEntry(n);


      if(n%10000==0) cout << n << endl;

      if( !( ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diObjects)[0].Pt()<0.08 && abs((*diObjects)[0].Eta())<1.5) || ((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diObjects)[0].Pt()<0.04 && abs((*diObjects)[0].Eta())>1.5)   ) || (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/(*diObjects)[1].Pt() > 0.3  || Zmass>100 || Zmass<80 )  continue;
      if(applyPre_ && !(jets->size()>1 && (*jets)[0].Pt()>MINPt1 && (*jets)[1].Pt()> MINPt2 && (*jets)[1].Eta()*(*jets)[0].Eta()<0 )) continue;
      if(applyVBF_ && !( (*jets)[0].Pt()> 38 && (*jets)[1].Pt()> 25 && abs((*jets)[1].Eta() - (*jets)[0].Eta())>2.7  && ((*jets)[1] + (*jets)[0]).M()>520 ) ) continue;

      int counter = 0;
      for(unsigned int it = 0; it<jets->size(); it++){
        if( (*jets)[it].Pt()>MINEt && abs((*jets)[it].Eta())<5.0 ) counter++;
      }

      map<string, float> vMap;
      vMap["mult"] = float(counter);
      vMap["mass"] = Zmass;
      vMap["pt"] =  ((*diObjects)[0]+(*diObjects)[1]).Pt();
      vMap["pt1"] = jets->size()>0 ? (*jets)[0].Pt() : -99;   
      vMap["pt2"] = jets->size()>1 ? (*jets)[1].Pt() : -99;   
      vMap["eta1"] = jets->size()>0 ? (*jets)[0].Eta() : -99;   
      vMap["eta2"] = jets->size()>1 ? (*jets)[1].Eta() : -99;   
      vMap["Deta"] = jets->size()>1 ? abs((*jets)[1].Eta() - (*jets)[0].Eta()) : -99;
      vMap["Mjj"]  = jets->size()>1 ? ((*jets)[1] + (*jets)[0]).M() : -99;

      float ZptScale = rescaleZpt_ ? hRatio->GetBinContent((int)(vMap["pt"]/4.+2)) : 1; 
      float weight = (samples[i]).find("Data")!=string::npos ? 1 : mcPUweight*scaleFactor*ZptScale;     
      h1->Fill( vMap[variable_] , weight );
    }

    delete jets; delete diObjects; delete triggerBits;
    
    if( (samples[i]).find("ZJets")!=string::npos ) {
      h1->SetFillColor(kRed);
      if(scleToData_)  h1->Scale(hData->Integral()/h1->Integral());
      leg->AddEntry(h1,"Z+jets","F");
    }
    if( (samples[i]).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (samples[i]).find("WJets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
  
    if( (samples[i]).find("Data")!=string::npos ){
      h1->Sumw2();
      h1->SetMarkerStyle(20);
      h1->SetMarkerSize(1.2);
      h1->SetMarkerColor(kBlack);
      h1->SetLineColor(kBlack);
      hData = (TH1F*)h1->Clone("hData");
      leg->AddEntry(hData,"DATA","P");
      cout <<  samples[i] << " ==> " << h1->Integral() << "+/-" << h1->Integral()/h1->GetEntries()*sqrt(h1->GetEntries()) << endl;
      continue;
    }
    
    aStack->Add(h1);

    cout <<  samples[i] << " ==> " << h1->Integral() << "+/-" << h1->Integral()/h1->GetEntries()*sqrt(h1->GetEntries()) << endl;
 
 
  }// end loop smaples


  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");

  leg->SetHeader("e^{-}+e^{+}");
  leg->Draw();

  //gPad->SaveAs("tmp2.png");

  return;
  
}



void plotMuMu(string variable_ = "mass", string labelX_ = "mass", Int_t nBins_ = 20, Float_t xMin_ = 80, Float_t xMax_ = 120 , Int_t applyPre_ = 1, Int_t applyVBF_ = 1,  Int_t rescaleZpt_ = 1, Int_t scleToData_ = 1, Float_t JES_ = 1. , Int_t Logy_ = 1){

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TCanvas *c0 = new TCanvas("c0","",5,30,650,600);
  c0->SetGrid(0,0);
  c0->SetFillStyle(4000);
  c0->SetFillColor(10);
  c0->SetTicky();
  c0->SetObjectStat(0);
  c0->SetLogy(Logy_);

  TLegend* leg = new TLegend(0.55,0.68,0.85,0.88,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);

  TFile* fData   = new TFile("/data_CMS//cms/lbianchini/ZmumuPlusJetsSpring11/treeZmumuPlusJets_Run2011-Mu-Zjets.root");
  TFile* fZJets  = new TFile("/data_CMS//cms/lbianchini/ZmumuPlusJetsSpring11/treeZmumuPlusJets_DYJets-Mu-50-madgraph-PUS1-Zjets.root");
  TFile* fWJets  = new TFile("/data_CMS//cms/lbianchini/ZmumuPlusJetsSpring11/treeZmumuPlusJets_WJets-Mu-madgraph-PUS1-Zjets.root");
  TFile* fTTbar  = new TFile("/data_CMS//cms/lbianchini/ZmumuPlusJetsSpring11/treeZmumuPlusJets_TTJets-Mu-madgraph-PUS1.root");

  if( fData->IsZombie() || fZJets->IsZombie() || fWJets->IsZombie() || fTTbar->IsZombie()){
    cout << "No file" << endl;
    exit(1);
  }

  vector<string> samples;

  samples.push_back("Data");
  samples.push_back("WJets");
  samples.push_back("TTbar");
  samples.push_back("ZJets");

  std::map<std::string,float> crossSec;
  crossSec["TTbar"]=( 157.5 );
  crossSec["WJets"]=( 31314.0);
  crossSec["ZJets"]=( 3048  );
  crossSec["Data"]=( 0 );

  std::map<string, TFile*> aMap;
  aMap["TTbar"] = fTTbar;
  aMap["WJets"] = fWJets;
  aMap["ZJets"] = fZJets;
  aMap["Data"]  = fData;

  TH1F* hPtZMC    = new TH1F("hPtZMC"," ; leading E_{T} ; A.U.",100,0,400);
  TH1F* hPtZRD    = new TH1F("hPtZRD"," ; leading E_{T} ; A.U.",100,0,400);
  ((TTree*)fData->Get("zPlusJetsAnalyzer/tree"))->Draw("sqrt((diMuonLegsP4[0].Px()+diMuonLegsP4[1].Px())*(diMuonLegsP4[0].Px()+diMuonLegsP4[1].Px()) + (diMuonLegsP4[0].Py()+diMuonLegsP4[1].Py())*(diMuonLegsP4[0].Py()+diMuonLegsP4[1].Py()))>>hPtZRD","(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diMuonLegsP4[0].Pt()<0.1 && (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/diMuonLegsP4[1].Pt() < 0.3  && Zmass<100 && Zmass>80");
  ((TTree*)fZJets->Get("zPlusJetsAnalyzer/tree"))->Draw("sqrt((diMuonLegsP4[0].Px()+diMuonLegsP4[1].Px())*(diMuonLegsP4[0].Px()+diMuonLegsP4[1].Px()) + (diMuonLegsP4[0].Py()+diMuonLegsP4[1].Py())*(diMuonLegsP4[0].Py()+diMuonLegsP4[1].Py()))>>hPtZMC","mcPUweight*( (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diMuonLegsP4[0].Pt()<0.1 && (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/diMuonLegsP4[1].Pt() < 0.3  && Zmass<100 && Zmass>80)");

  hPtZMC->Scale(1./hPtZMC->Integral());
  hPtZRD->Scale(1./hPtZRD->Integral());

  TH1F* hRatio = (TH1F*)hPtZRD->Clone("hRatio"); 
  hRatio->Reset();
  hRatio->Divide(hPtZRD,hPtZMC,1.0,1.0);



  //TH1F* hPt1    = new TH1F("hPt1"," ; leading E_{T} ; A.U.",40,0,200);
  //TH1F* hPt2    = new TH1F("hPt2"," ; second leading E_{T} ; A.U.",40,0,200);
  //TH1F* hDeta   = new TH1F("hDeta"," ; |#Delta#eta| ; A.U.",20,0,8);
  //TH1F* hMjj    = new TH1F("hMjj"," ; M_{jj} ; A.U.",30,0,1200);

  float Lumi = 136.;

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}",Lumi));
  TH1F* hData = new TH1F();


  for(unsigned int i = 0 ; i < samples.size() ; i++){

    TH1F* allEvents = (TH1F*)(aMap[samples[i]])->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);

    float scaleFactor = crossSec[samples[i]]>0 ? Lumi/(totalEvents/crossSec[samples[i]]) : 1 ;

    float JESuncert_ = ((samples[i]).find("Data")!=string::npos) ? 1.0 : JES_;

    TTree* currentTree = (TTree*)(aMap[samples[i]])->Get("zPlusJetsAnalyzer/tree");
    resetBranchesLeptLept(currentTree,1);

    cout << samples[i] <<  "  " << currentTree->GetEntries() << endl;


    TH1F* h1   = new TH1F("h1",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1} ; %s ; Events/%.1f", Lumi, labelX_.c_str(), (xMax_-xMin_)/nBins_ ), nBins_, xMin_, xMax_);
    h1->SetName(samples[i].c_str());

    std::vector< LV >* jets       = new std::vector< LV >();
    std::vector< LV >* diObjects  = new std::vector< LV >();
    std::vector< int >* triggerBits  = new std::vector< int >();
    float mcPUweight; float Zmass;
    float chIsoLeg1, nhIsoLeg1, phIsoLeg1;
    float chIsoLeg2, nhIsoLeg2, phIsoLeg2;

    currentTree->SetBranchAddress("jetsIDP4", &jets);
    currentTree->SetBranchAddress("diMuonLegsP4", &diObjects);
    currentTree->SetBranchAddress("triggerBits", &triggerBits);
    currentTree->SetBranchAddress("mcPUweight", &mcPUweight);
    currentTree->SetBranchAddress("chIsoLeg1", &chIsoLeg1);
    currentTree->SetBranchAddress("nhIsoLeg1", &nhIsoLeg1);
    currentTree->SetBranchAddress("phIsoLeg1", &phIsoLeg1);
    currentTree->SetBranchAddress("chIsoLeg2", &chIsoLeg2);
    currentTree->SetBranchAddress("nhIsoLeg2", &nhIsoLeg2);
    currentTree->SetBranchAddress("phIsoLeg2", &phIsoLeg2);
    currentTree->SetBranchAddress("Zmass", &Zmass);


    TBranch *brjetsIDP4     = currentTree->GetBranch("jetsIDP4");
    TBranch *brdiElectronLegsP4 = currentTree->GetBranch("diMuonLegsP4");
    TBranch *brtriggerBits = currentTree->GetBranch("triggerBits");
    TBranch *brmcPUweight   = currentTree->GetBranch("mcPUweight");
    TBranch *brchIsoLeg1    = currentTree->GetBranch("chIsoLeg1");
    TBranch *brnhIsoLeg1    = currentTree->GetBranch("nhIsoLeg1");
    TBranch *brphIsoLeg1    = currentTree->GetBranch("phIsoLeg1");
    TBranch *brchIsoLeg2    = currentTree->GetBranch("chIsoLeg2");
    TBranch *brnhIsoLeg2    = currentTree->GetBranch("nhIsoLeg2");
    TBranch *brphIsoLeg2    = currentTree->GetBranch("phIsoLeg2");
    TBranch *brZmass        = currentTree->GetBranch("Zmass");

    int nEntries = currentTree->GetEntries();

    for(unsigned n = 0 ; n< nEntries ; n++){

      brjetsIDP4->GetEntry(n);
      brdiElectronLegsP4->GetEntry(n);
      //brtriggerBits->GetEntry(n);
      brmcPUweight->GetEntry(n);
      brchIsoLeg1->GetEntry(n);
      brnhIsoLeg1->GetEntry(n);
      brphIsoLeg1->GetEntry(n);
      brchIsoLeg2->GetEntry(n);
      brnhIsoLeg2->GetEntry(n);
      brphIsoLeg2->GetEntry(n);
      brZmass->GetEntry(n);

      if(n%10000==0){
	cout << n << endl;
	cout <<JESuncert_ << endl;
      }

      if( (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diObjects)[0].Pt()>0.1 || (chIsoLeg2+nhIsoLeg2+phIsoLeg2)/(*diObjects)[1].Pt() > 0.3  || Zmass>100 || Zmass<80 )  continue;
      if(applyPre_ && !(jets->size()>1 && (*jets)[0].Pt()*JESuncert_>MINPt1 && (*jets)[1].Pt()*JESuncert_> MINPt2 && (*jets)[1].Eta()*(*jets)[0].Eta()<0 )) continue;
      if(applyVBF_ && !( (*jets)[0].Pt()*JESuncert_> 30 && (*jets)[1].Pt()*JESuncert_> 30 && abs((*jets)[1].Eta() - (*jets)[0].Eta())>3.5  && ((*jets)[1] + (*jets)[0]).M()*JESuncert_>450 && (jets->size()==2 || ((*jets)[2].Pt()<30)) ) ) continue;

      int counter = 0;
      for(unsigned int it = 0; it<jets->size(); it++){
        if( (*jets)[it].Pt()*JESuncert_>MINEt && abs((*jets)[it].Eta())<2.4 ) counter++;
      }

      map<string, float> vMap;
      vMap["mult"] = float(counter);
      vMap["mass"] = Zmass;
      vMap["pt"] =  ((*diObjects)[0]+(*diObjects)[1]).Pt();
      vMap["pt1"] = jets->size()>0 ? (*jets)[0].Pt()*JESuncert_ : -99;   
      vMap["pt2"] = jets->size()>1 ? (*jets)[1].Pt()*JESuncert_ : -99;   
      vMap["eta1"] = jets->size()>0 ? (*jets)[0].Eta()*JESuncert_ : -99;   
      vMap["eta2"] = jets->size()>1 ? (*jets)[1].Eta()*JESuncert_ : -99;   
      vMap["Deta"] = jets->size()>1 ? abs((*jets)[1].Eta() - (*jets)[0].Eta()) : -99;
      vMap["Mjj"]  = jets->size()>1 ? ((*jets)[1] + (*jets)[0]).M()*JESuncert_ : -99;

      float ZptScale = rescaleZpt_ ? hRatio->GetBinContent((int)(vMap["pt"]/4.+1)) : 1; 
      float weight = (samples[i]).find("Data")!=string::npos ? 1 : mcPUweight*scaleFactor*ZptScale;     
      h1->Fill( vMap[variable_] , weight );
    }

    delete jets; delete diObjects; delete triggerBits;
    
    if( (samples[i]).find("ZJets")!=string::npos ) {
      h1->SetFillColor(kRed);
      if(scleToData_)  h1->Scale(hData->Integral()/h1->Integral());
      leg->AddEntry(h1,"Z+jets","F");
    }
    if( (samples[i]).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (samples[i]).find("WJets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
  
    if( (samples[i]).find("Data")!=string::npos ){
      h1->Sumw2();
      h1->SetMarkerStyle(20);
      h1->SetMarkerSize(1.2);
      h1->SetMarkerColor(kBlack);
      h1->SetLineColor(kBlack);
      hData = (TH1F*)h1->Clone("hData");
      leg->AddEntry(hData,"DATA","P");
      cout <<  samples[i] << " ==> " << h1->Integral() << "+/-" << h1->Integral()/h1->GetEntries()*sqrt(h1->GetEntries()) << endl;
      continue;
    }
    
    aStack->Add(h1);

    cout <<  samples[i] << " ==> " << h1->Integral() << "+/-" << h1->Integral()/h1->GetEntries()*sqrt(h1->GetEntries()) << endl;
 
 
  }// end loop smaples


  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");

  leg->SetHeader("#mu+#mu");
  leg->Draw();

  //gPad->SaveAs("tmp2.png");

  return;
  
}
