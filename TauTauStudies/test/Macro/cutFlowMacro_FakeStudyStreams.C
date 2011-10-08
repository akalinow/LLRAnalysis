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
#include "TPluginManager.h"
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "THStack.h"

//#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#endif

#define VERBOSE true

void cutFlowStudyElec( TString weightFile = "TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",
		       Double_t effS_ = 0.3) 
{
  
  ofstream out("cutFlow-ElecTauStream.txt");
  out.precision(4);

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
  reader->BookMVA( "Cuts", TString("/home/llr/cms/lbianchini/CMSSW_3_9_9/src/LLRAnalysis/TauTauStudies/test/Macro/weights/")+weightFile ); 
 
  TFile *fFullSignalVBF           = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_VBFH115-powheg-PUS1.root","READ"); 
  TFile *fFullSignalGGH           = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_GGFH115-powheg-PUS1.root","READ");  
  TFile *fFullBackgroundDYTauTau  = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_DYToTauTau-20-PUS1.root","READ"); 
  TFile *fFullBackgroundDYEleEle  = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_DYToEE-20-PUS1.root","READ"); 
  TFile *fFullBackgroundWJets     = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_WJets-madgraph-PUS1.root","READ"); 
  TFile *fFullBackgroundQCD       = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_QCD.root","READ"); 
  TFile *fFullBackgroundG1J       = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_G1Jet.root","READ"); 
  TFile *fFullBackgroundTTbar     = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_TTJets-madgraph-PUS1.root","READ"); 
  TFile *fFullBackgroundDiBoson   = new TFile("/data_CMS/cms/lbianchini//ElecTauStream2011/treeElecTauStream_DiBoson.root","READ"); 

  // OpenNTuples
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleVBFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleGGFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDYToTauTau-20-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYEleEle  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDYToEE-20-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleQCD_Open_ElecTauStream.root";
  TString fBackgroundNameG1J       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleG1Jet_Open_ElecTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleTTJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDiBoson_Open_ElecTauStream.root";

  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYEleEle(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundG1J(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundDiBoson(0);
 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYEleEle = TFile::Open( fBackgroundNameDYEleEle ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundG1J      = TFile::Open( fBackgroundNameG1J ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar ||
     !fSignalGGH || !fBackgroundDYEleEle || !fBackgroundG1J || !fBackgroundDiBoson ){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *backgroundDYTauTau  = (TTree*)fBackgroundDYTauTau->Get(tree);
  TTree *backgroundDYEleEle  = (TTree*)fBackgroundDYEleEle->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundG1J       = (TTree*)fBackgroundG1J->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["Zeleele"]=backgroundDYEleEle;
  tMap["Wjets"]=backgroundWJets;
  tMap["G1J"]=backgroundG1J;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["DiBoson"]=backgroundDiBoson;

  std::map<std::string,TTree*>::iterator jt;

  Float_t pt1_, pt2_;
  Float_t Deta_, Mjj_;
  Float_t Dphi,diTauSVFitPt,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1,sampleWeight,ptVeto,HLT;
  Int_t tightestHPSWP;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("DiBoson");
  samples.push_back("TTbar");
  samples.push_back("Wjets");
  samples.push_back("Zeleele");
  samples.push_back("Ztautau");
  samples.push_back("G1J");
  samples.push_back("QCD");

  std::map<std::string,float> crossSec;
  crossSec["ggH115"]=( 7.65e-02 * 18.13 );
  crossSec["qqH115"]=( 0.1012);
  crossSec["DiBoson"]=( -1  );
  crossSec["TTbar"]=( 157.5 );
  crossSec["Wjets"]=( 31314.0);
  crossSec["Zeleele"]=( 1666  );
  crossSec["Ztautau"]=( 1666  );
  crossSec["G1J"]=( -1 );
  crossSec["QCD"]=( -1 );

  float Lumi = 1000;


  // here I choose the order in the stack
  std::vector<string> filters;
  filters.push_back("total");
  filters.push_back("vertex");
  filters.push_back("1-e");
  filters.push_back("0-mu");
  filters.push_back("e-ID");
  filters.push_back("tau-ID");
  filters.push_back("Delta R e-tau");
  filters.push_back("e-iso");
  filters.push_back("tau-iso");
  filters.push_back("Mt");
  filters.push_back("OS");
  filters.push_back("2-jets");
  filters.push_back("VBF cuts");
  filters.push_back("jet-veto");
  filters.push_back("HLT");

  // here I define the map between a sample name and its file ptr
  std::map<std::string,TFile*> fullMap;
  fullMap["ggH115"]   = fFullSignalGGH;
  fullMap["qqH115"]   = fFullSignalVBF;
  fullMap["Ztautau"]  = fFullBackgroundDYTauTau;
  fullMap["Zeleele"]  = fFullBackgroundDYEleEle;
  fullMap["Wjets"]    = fFullBackgroundWJets;
  fullMap["G1J"]      = fFullBackgroundG1J;
  fullMap["QCD"]      = fFullBackgroundQCD;
  fullMap["TTbar"]    = fFullBackgroundTTbar;
  fullMap["DiBoson"]  = fFullBackgroundDiBoson;

  std::map<std::string,TFile*>::iterator it;

  std::map<std::string,float> cutMap_allEventsFilter;
  std::map<std::string,float> cutMap_allEventsFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    float tot = totalEvents;
    if(crossSec[it->first]>0) tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    cutMap_allEventsFilter[it->first] = tot;
    cutMap_allEventsFilterE[it->first] = sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents);
  }
  std::map<std::string,float> cutMap_vertexScrapingFilter;
  std::map<std::string,float> cutMap_vertexScrapingFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("vertexScrapingFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_vertexScrapingFilter[it->first] = tot;
    cutMap_vertexScrapingFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_oneElectronFilter;
  std::map<std::string,float> cutMap_oneElectronFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("oneElectronFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_oneElectronFilter[it->first] = tot;
    cutMap_oneElectronFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }

  std::map<std::string,float> cutMap_noMuonFilter;
  std::map<std::string,float> cutMap_noMuonFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("noMuonFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_noMuonFilter[it->first] = tot;
    cutMap_noMuonFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }

  std::map<std::string,float> cutMap_electronLegFilter;
  std::map<std::string,float> cutMap_electronLegFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("electronLegFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_electronLegFilter[it->first] = tot;
    cutMap_electronLegFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_tauLegFilter;
  std::map<std::string,float> cutMap_tauLegFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("tauLegFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_tauLegFilter[it->first] = tot;
    cutMap_tauLegFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_atLeastOneDiTauFilter;
  std::map<std::string,float> cutMap_atLeastOneDiTauFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("atLeastOneDiTauFilter/totalEvents");
    float tot = allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_atLeastOneDiTauFilter[it->first] = tot;
    cutMap_atLeastOneDiTauFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_ElecIso;
  std::map<std::string,float> cutMap_ElecIsoE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04))"
      : "weight*((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04))";
    ((TTree*) (it->second->Get("elecTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_ElecIso[it->first] = tot;
    cutMap_ElecIsoE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_TauIso;
  std::map<std::string,float> cutMap_TauIsoE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0"
      : "weight*(((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0)";
    ((TTree*) (it->second->Get("elecTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_TauIso[it->first] = tot;
    cutMap_TauIsoE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_Mt;
  std::map<std::string,float> cutMap_MtE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0 && MtLeg1<40"
      : "weight*(((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0 && MtLeg1<40)";
    ((TTree*) (it->second->Get("elecTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_Mt[it->first] = tot;
    cutMap_MtE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_OS;
  std::map<std::string,float> cutMap_OSE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40"
      : "weight*(((abs(diTauLegsP4[0].Eta())<1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.08) || (abs(diTauLegsP4[0].Eta())>1.5 && (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.04)) && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)";
    ((TTree*) (it->second->Get("elecTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_OS[it->first] = tot;
    cutMap_OSE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_VBFPre;
  std::map<std::string,float> cutMap_VBFPreE;
  for(jt = tMap.begin(); jt != tMap.end(); jt++){
    cout<<jt->first<<endl;
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  "sampleWeight*(pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)"; 
    jt->second->Draw("etaL1>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    cutMap_VBFPre[jt->first] = tot;
    cutMap_VBFPreE[jt->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_VBF;
  std::map<std::string,float> cutMap_VBFE;
  std::map<std::string,float> cutMap_JetVeto;
  std::map<std::string,float> cutMap_JetVetoE;
  std::map<std::string,float> cutMap_HLT;
  std::map<std::string,float> cutMap_HLTE;
  for(jt = tMap.begin(); jt != tMap.end(); jt++){
    cout<<jt->first<<endl;
    TCut cut =  "(pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)"; 

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TTree* currentTree = (TTree*)(jt->second)->CopyTree(cut);
    float tot = 0;
    int counter = 0;
    float tot2 = 0;
    int counter2 = 0;
    float tot3 = 0;
    int counter3 = 0;

    currentTree->SetBranchAddress( "pt1", &pt1_ );
    currentTree->SetBranchAddress( "pt2", &pt2_ );
    currentTree->SetBranchAddress( "Deta",&Deta_ );
    currentTree->SetBranchAddress( "Mjj", &Mjj_ );
    currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
    //currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
    currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
    currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
    currentTree->SetBranchAddress( "ptL1", &ptL1 );
    currentTree->SetBranchAddress( "ptL2",  &ptL2 );
    currentTree->SetBranchAddress( "etaL1", &etaL1 );
    currentTree->SetBranchAddress( "etaL2", &etaL2 );
    currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
    currentTree->SetBranchAddress( "tightestHPSWP",&tightestHPSWP);
    currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
    currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
    currentTree->SetBranchAddress( "numPV",&numPV);
    currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
    currentTree->SetBranchAddress( "ptVeto",&ptVeto);
    currentTree->SetBranchAddress( "HLT",&HLT);

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {

      currentTree->GetEntry(ievt);

      if (ievt%10000 == 0){
	std::cout << (jt->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
      }

      pt1  = pt1_;
      pt2  = pt2_;
      Deta = Deta_;
      Mjj  = Mjj_;

      bool pass = effS_>0 ? reader->EvaluateMVA( "Cuts", effS_ ) : (pt1>0);

      if(pass){
	tot+=sampleWeight;
	counter++;
	if(ptVeto<20){
	  tot2+=sampleWeight;
	  counter2++;
	  if(HLT>0.5 && HLT<1.5){
	    tot3+=sampleWeight;
	    counter3++;
	  }
	}
      }
    
    }// end loop   

    cutMap_VBF[jt->first] = tot;
    cutMap_VBFE[jt->first] = counter>0 ? sqrt(counter)*tot/counter : 0;
    cutMap_JetVeto[jt->first] = tot2;
    cutMap_JetVetoE[jt->first] = counter2>0 ? sqrt(counter2)*tot2/counter2 : 0;
    cutMap_HLT[jt->first] = tot3;
    cutMap_HLTE[jt->first] = counter3>0 ? sqrt(counter3)*tot3/counter3 : 0;
  }


  
 

  std::vector< std::map<std::string,float> > allFilters;
  allFilters.push_back(cutMap_allEventsFilter);
  allFilters.push_back(cutMap_vertexScrapingFilter);
  allFilters.push_back(cutMap_oneElectronFilter);
  allFilters.push_back(cutMap_noMuonFilter);
  allFilters.push_back(cutMap_electronLegFilter);
  allFilters.push_back(cutMap_tauLegFilter);
  allFilters.push_back(cutMap_atLeastOneDiTauFilter);
  allFilters.push_back(cutMap_ElecIso);
  allFilters.push_back(cutMap_TauIso);
  allFilters.push_back(cutMap_Mt);
  allFilters.push_back(cutMap_OS);
  allFilters.push_back(cutMap_VBFPre);
  allFilters.push_back(cutMap_VBF);
  allFilters.push_back(cutMap_JetVeto);
  allFilters.push_back(cutMap_HLT);

  std::vector< std::map<std::string,float> > allFiltersE;
  allFiltersE.push_back(cutMap_allEventsFilterE);
  allFiltersE.push_back(cutMap_vertexScrapingFilterE);
  allFiltersE.push_back(cutMap_oneElectronFilterE);
  allFiltersE.push_back(cutMap_noMuonFilterE);
  allFiltersE.push_back(cutMap_electronLegFilterE);
  allFiltersE.push_back(cutMap_tauLegFilterE);
  allFiltersE.push_back(cutMap_atLeastOneDiTauFilterE);
  allFiltersE.push_back(cutMap_ElecIsoE);
  allFiltersE.push_back(cutMap_TauIsoE);
  allFiltersE.push_back(cutMap_MtE);
  allFiltersE.push_back(cutMap_OSE);
  allFiltersE.push_back(cutMap_VBFPreE);
  allFiltersE.push_back(cutMap_VBFE);
  allFiltersE.push_back(cutMap_JetVetoE);
  allFiltersE.push_back(cutMap_HLTE);

  //out<<"\\begin{center}"<<endl;
  out<<"\\begin{tabular}[!htbp]{|c";
  for(int k = 0 ; k < samples.size(); k++) out<<"|c";
  out<<"|} \\hline"<<endl;
  out<< "Cut & ";
  for(int k = 0 ; k < samples.size(); k++){
    out << (fullMap.find(samples[k]))->first;
    if(k!=samples.size()-1) out <<" & " ;
    else out << " \\\\ " << endl;
  }
  out <<  " \\hline" << endl;

  
  for(int i = 0; i < allFilters.size(); i++){
    out << filters[i] << " & ";
    for(int k = 0 ; k < samples.size(); k++){
      out << (allFilters[i].find(samples[k]))->second << " $\\pm$ " << (allFiltersE[i].find(samples[k]))->second;
      if(k!=samples.size()-1) out <<" & " ;
      else out << " \\\\ " << endl;
    }
    out <<  " \\hline" << endl;

  }
  
  out<<"\\end{tabular}"<<endl;
  //out<<"\\end{center}"<<endl;
 

  return;

}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


void cutFlowStudyMu( TString weightFile = "TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",
		     Double_t effS_ = 0.3) 
{
  
  ofstream out("cutFlow-MuTauStream.txt");
  out.precision(4);

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
  reader->BookMVA( "Cuts", TString("/home/llr/cms/lbianchini/CMSSW_3_9_9/src/LLRAnalysis/TauTauStudies/test/Macro/weights/")+weightFile ); 
 
  TFile *fFullSignalVBF           = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_VBFH115-Mu-powheg-PUS1.root","READ"); 
  TFile *fFullSignalGGH           = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_GGFH115-Mu-powheg-PUS1.root","READ");  
  TFile *fFullBackgroundDYTauTau  = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_DYToTauTau-Mu-20-PUS1.root","READ"); 
  TFile *fFullBackgroundDYMuMu    = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_DYToMuMu-20-PUS1.root","READ"); 
  TFile *fFullBackgroundWJets     = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_WJets-Mu-madgraph-PUS1.root","READ"); 
  TFile *fFullBackgroundQCD       = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_QCDmu.root","READ"); 
  TFile *fFullBackgroundTTbar     = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_TTJets-Mu-madgraph-PUS1.root","READ"); 
  TFile *fFullBackgroundDiBoson   = new TFile("/data_CMS/cms/lbianchini//MuTauStream2011/treeMuTauStream_DiBoson-Mu.root","READ"); 

  // OpenNTuples
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleGGFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleDYToTauTau-Mu-20-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYMuMu  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleDYToMuMu-20-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleQCDmu_Open_MuTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/v2/nTupleDiBoson-Mu_Open_MuTauStream.root";

  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYMuMu(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundDiBoson(0);
 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYMuMu   = TFile::Open( fBackgroundNameDYMuMu ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar ||
     !fSignalGGH || !fBackgroundDYMuMu || !fBackgroundDiBoson ){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *backgroundDYTauTau  = (TTree*)fBackgroundDYTauTau->Get(tree);
  TTree *backgroundDYMuMu    = (TTree*)fBackgroundDYMuMu->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["Zmumu"]=backgroundDYMuMu;
  tMap["Wjets"]=backgroundWJets;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["DiBoson"]=backgroundDiBoson;

  std::map<std::string,TTree*>::iterator jt;

  Float_t pt1_, pt2_;
  Float_t Deta_, Mjj_;
  Float_t Dphi,diTauSVFitPt,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1,sampleWeight,ptVeto,HLT;
  Int_t tightestHPSWP;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("DiBoson");
  samples.push_back("TTbar");
  samples.push_back("Wjets");
  samples.push_back("Zmumu");
  samples.push_back("Ztautau");
  samples.push_back("QCD");

  std::map<std::string,float> crossSec;
  crossSec["ggH115"]=( 7.65e-02 * 18.13 );
  crossSec["qqH115"]=( 0.1012);
  crossSec["DiBoson"]=( -1  );
  crossSec["TTbar"]=( 157.5 );
  crossSec["Wjets"]=( 31314.0);
  crossSec["Zmumu"]=( 1666  );
  crossSec["Ztautau"]=( 1666  );
  crossSec["QCD"]=( 296600000*0.0002855 );

  float Lumi = 1000;


  // here I choose the order in the stack
  std::vector<string> filters;
  filters.push_back("total");
  filters.push_back("vertex");
  filters.push_back("1-mu");
  filters.push_back("0-e");
  filters.push_back("mu-ID");
  filters.push_back("tau-ID");
  filters.push_back("Delta R mu-tau");
  filters.push_back("mu-iso");
  filters.push_back("tau-iso");
  filters.push_back("Mt");
  filters.push_back("OS");
  filters.push_back("2-jets");
  filters.push_back("VBF cuts");
  filters.push_back("jet-veto");
  filters.push_back("HLT");

  // here I define the map between a sample name and its file ptr
  std::map<std::string,TFile*> fullMap;
  fullMap["ggH115"]   = fFullSignalGGH;
  fullMap["qqH115"]   = fFullSignalVBF;
  fullMap["Ztautau"]  = fFullBackgroundDYTauTau;
  fullMap["Zmumu"]    = fFullBackgroundDYMuMu;
  fullMap["Wjets"]    = fFullBackgroundWJets;
  fullMap["QCD"]      = fFullBackgroundQCD;
  fullMap["TTbar"]    = fFullBackgroundTTbar;
  fullMap["DiBoson"]  = fFullBackgroundDiBoson;

  std::map<std::string,TFile*>::iterator it;

  std::map<std::string,float> cutMap_allEventsFilter;
  std::map<std::string,float> cutMap_allEventsFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    float tot = totalEvents;
    if(crossSec[it->first]>0) tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    cutMap_allEventsFilter[it->first] = tot;
    cutMap_allEventsFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_vertexScrapingFilter;
  std::map<std::string,float> cutMap_vertexScrapingFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("vertexScrapingFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_vertexScrapingFilter[it->first] = tot;
    cutMap_vertexScrapingFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_oneElectronFilter;
  std::map<std::string,float> cutMap_oneElectronFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("oneMuonFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_oneElectronFilter[it->first] = tot;
    cutMap_oneElectronFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_noMuonFilter;
  std::map<std::string,float> cutMap_noMuonFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("noElecFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_noMuonFilter[it->first] = tot;
    cutMap_noMuonFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_electronLegFilter;
  std::map<std::string,float> cutMap_electronLegFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("muonLegFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_electronLegFilter[it->first] = tot;
    cutMap_electronLegFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_tauLegFilter;
  std::map<std::string,float> cutMap_tauLegFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("tauLegFilter/totalEvents");
    float tot =  allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_tauLegFilter[it->first] = tot;
    cutMap_tauLegFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_atLeastOneDiTauFilter;
  std::map<std::string,float> cutMap_atLeastOneDiTauFilterE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    allEvents = (TH1F*)(it->second)->Get("atLeastOneDiTauFilter/totalEvents");
    float tot = allEvents->GetBinContent(1);
    float totalEquivalentEvents = allEvents->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_atLeastOneDiTauFilter[it->first] = tot;
    cutMap_atLeastOneDiTauFilterE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
  }
  std::map<std::string,float> cutMap_ElecIso;
  std::map<std::string,float> cutMap_ElecIsoE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1"
      : "weight*((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1)";
    ((TTree*) (it->second->Get("muTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_ElecIso[it->first] = tot;
    cutMap_ElecIsoE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_TauIso;
  std::map<std::string,float> cutMap_TauIsoE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0"
      : "weight*((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0)";
    ((TTree*) (it->second->Get("muTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_TauIso[it->first] = tot;
    cutMap_TauIsoE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_Mt;
  std::map<std::string,float> cutMap_MtE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0 && MtLeg1<40"
      : "weight*((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0 && MtLeg1<40)";
    ((TTree*) (it->second->Get("muTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_Mt[it->first] = tot;
    cutMap_MtE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_OS;
  std::map<std::string,float> cutMap_OSE;
  for(it = fullMap.begin(); it != fullMap.end(); it++){
    cout<<it->first<<endl;
    TH1F* allEvents = (TH1F*)(it->second)->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  (crossSec[it->first]>0) ?  "(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40"
      : "weight*((chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)";
    ((TTree*) (it->second->Get("muTauStreamAnalyzer/tree")) )->Draw("diTauLegsP4[0].Eta()>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    if(crossSec[it->first]>0){
      tot *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
      //totalEquivalentEvents *= (Lumi/ (totalEvents/crossSec[it->first])  ) ;
    }
    cutMap_OS[it->first] = tot;
    cutMap_OSE[it->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_VBFPre;
  std::map<std::string,float> cutMap_VBFPreE;
  for(jt = tMap.begin(); jt != tMap.end(); jt++){
    cout<<jt->first<<endl;
    TH1F* h1 = new TH1F("h1","",1,-10,10); 
    TCut cut =  "sampleWeight*(pt1>0 && combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)"; 
    jt->second->Draw("etaL1>>h1",cut);
    float tot = h1->Integral();
    float totalEquivalentEvents = h1->GetEffectiveEntries();
    cutMap_VBFPre[jt->first] = tot;
    cutMap_VBFPreE[jt->first] = totalEquivalentEvents>0 ? sqrt(totalEquivalentEvents)*(tot/totalEquivalentEvents) : 0;
    delete h1;
  }
  std::map<std::string,float> cutMap_VBF;
  std::map<std::string,float> cutMap_VBFE;
  std::map<std::string,float> cutMap_JetVeto;
  std::map<std::string,float> cutMap_JetVetoE;
  std::map<std::string,float> cutMap_HLT;
  std::map<std::string,float> cutMap_HLTE;
  for(jt = tMap.begin(); jt != tMap.end(); jt++){
    cout<<jt->first<<endl;
    TCut cut =  "(pt1>0 && combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40)"; 

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TTree* currentTree = (TTree*)(jt->second)->CopyTree(cut);
    float tot = 0;
    int counter = 0;
    float tot2 = 0;
    int counter2 = 0;
    float tot3 = 0;
    int counter3 = 0;

    currentTree->SetBranchAddress( "pt1", &pt1_ );
    currentTree->SetBranchAddress( "pt2", &pt2_ );
    currentTree->SetBranchAddress( "Deta",&Deta_ );
    currentTree->SetBranchAddress( "Mjj", &Mjj_ );
    currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
    //currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
    currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
    currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
    currentTree->SetBranchAddress( "ptL1", &ptL1 );
    currentTree->SetBranchAddress( "ptL2",  &ptL2 );
    currentTree->SetBranchAddress( "etaL1", &etaL1 );
    currentTree->SetBranchAddress( "etaL2", &etaL2 );
    currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
    currentTree->SetBranchAddress( "tightestHPSWP",&tightestHPSWP);
    currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
    currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
    currentTree->SetBranchAddress( "numPV",&numPV);
    currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
    currentTree->SetBranchAddress( "ptVeto",&ptVeto);
    currentTree->SetBranchAddress( "HLT",&HLT);

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {

      currentTree->GetEntry(ievt);

      if (ievt%10000 == 0){
	std::cout << (jt->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
      }

      pt1  = pt1_;
      pt2  = pt2_;
      Deta = Deta_;
      Mjj  = Mjj_;

      bool pass = effS_>0 ? reader->EvaluateMVA( "Cuts", effS_ ) : (pt1>0);

      if(pass){
	tot+=sampleWeight;
	counter++;
	if(ptVeto<20){
	  tot2+=sampleWeight;
	  counter2++;
	  if(HLT>0.5 && HLT<1.5){
	    tot3+=sampleWeight;
	    counter3++;
	  }
	}
      }
    
    }// end loop   

    cutMap_VBF[jt->first] = tot;
    cutMap_VBFE[jt->first] = counter>0 ? sqrt(counter)*tot/counter : 0;
    cutMap_JetVeto[jt->first] = tot2;
    cutMap_JetVetoE[jt->first] = counter2>0 ? sqrt(counter2)*tot2/counter2 : 0;
    cutMap_HLT[jt->first] = tot3;
    cutMap_HLTE[jt->first] = counter3>0 ? sqrt(counter3)*tot3/counter3 : 0;
  }


  
 

  std::vector< std::map<std::string,float> > allFilters;
  allFilters.push_back(cutMap_allEventsFilter);
  allFilters.push_back(cutMap_vertexScrapingFilter);
  allFilters.push_back(cutMap_oneElectronFilter);
  allFilters.push_back(cutMap_noMuonFilter);
  allFilters.push_back(cutMap_electronLegFilter);
  allFilters.push_back(cutMap_tauLegFilter);
  allFilters.push_back(cutMap_atLeastOneDiTauFilter);
  allFilters.push_back(cutMap_ElecIso);
  allFilters.push_back(cutMap_TauIso);
  allFilters.push_back(cutMap_Mt);
  allFilters.push_back(cutMap_OS);
  allFilters.push_back(cutMap_VBFPre);
  allFilters.push_back(cutMap_VBF);
  allFilters.push_back(cutMap_JetVeto);
  allFilters.push_back(cutMap_HLT);

  std::vector< std::map<std::string,float> > allFiltersE;
  allFiltersE.push_back(cutMap_allEventsFilterE);
  allFiltersE.push_back(cutMap_vertexScrapingFilterE);
  allFiltersE.push_back(cutMap_oneElectronFilterE);
  allFiltersE.push_back(cutMap_noMuonFilterE);
  allFiltersE.push_back(cutMap_electronLegFilterE);
  allFiltersE.push_back(cutMap_tauLegFilterE);
  allFiltersE.push_back(cutMap_atLeastOneDiTauFilterE);
  allFiltersE.push_back(cutMap_ElecIsoE);
  allFiltersE.push_back(cutMap_TauIsoE);
  allFiltersE.push_back(cutMap_MtE);
  allFiltersE.push_back(cutMap_OSE);
  allFiltersE.push_back(cutMap_VBFPreE);
  allFiltersE.push_back(cutMap_VBFE);
  allFiltersE.push_back(cutMap_JetVetoE);
  allFiltersE.push_back(cutMap_HLTE);

  //out<<"\\begin{center}"<<endl;
  out<<"\\begin{tabular}[!htbp]{|c";
  for(int k = 0 ; k < samples.size(); k++) out<<"|c";
  out<<"|} \\hline"<<endl;
  out<< "Cut & ";
  for(int k = 0 ; k < samples.size(); k++){
    out << (fullMap.find(samples[k]))->first;
    if(k!=samples.size()-1) out <<" & " ;
    else out << " \\\\ " << endl;
  }
  out <<  " \\hline" << endl;

  
  for(int i = 0; i < allFilters.size(); i++){
    out << filters[i] << " & ";
    for(int k = 0 ; k < samples.size(); k++){
      out << (allFilters[i].find(samples[k]))->second << " $\\pm$ " << (allFiltersE[i].find(samples[k]))->second;
      if(k!=samples.size()-1) out <<" & " ;
      else out << " \\\\ " << endl;
    }
    out <<  " \\hline" << endl;

  }
  
  out<<"\\end{tabular}"<<endl;
  //out<<"\\end{center}"<<endl;
 

  return;

}
