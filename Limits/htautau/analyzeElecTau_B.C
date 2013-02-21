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
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"

#define VERBOSE true
#define SAVE    true
#define addVH   true
#define kFactorSM 1.0
#define EMBEDDEDSAMPLES   true



///////////////////////////////////////////////////////////////////////////////////////////////

void plotElecTau( Int_t mH_           = 120,
		  Int_t useEmbedding_ = 1,
		  string selection_   = "inclusive",
		  string analysis_    = "",		  
		  TString variable_   = "diTauVisMass",
		  TString XTitle_     = "full mass",
		  TString Unities_    = "GeV",
		  TString outputDir   = "Nov2011/",
		  Int_t nBins_ = 40, Float_t xMin_=0, Float_t xMax_=200,
		  Float_t magnifySgn_ = 1.0,
		  Float_t hltEff_     = 1.0,
		  Int_t logy_ = 0 ) 
{   

  // input txt file with bins
  ifstream is;

  char* c = new char[10];
  is.open(Form("bins/bins_eTau_%s.txt",variable_.Data())); 
  if(nBins_<0 &&  !is.good()){
    cout << "Bins file not found" << endl;
    return;
  }

  int nBinsFromFile = 0;
  while (is.good())     
    {
      is.getline(c,999,',');     
      if (is.good()){
	nBinsFromFile++;
	//cout << c << endl;
      }
    }

  // choose the number of bins
  int nBins =  nBins_>0 ? nBins_ : nBinsFromFile-1 ;
  Float_t bins[nBins+1];
  cout << "Making histograms with " << nBins << " bins:" << endl;

  is.close();
  is.open(Form("bins/bins_eTau_%s.txt",variable_.Data())); 
  
  nBinsFromFile = 0;

  if(nBins_>0){
    for( ; nBinsFromFile <= nBins ; nBinsFromFile++){
      bins[nBinsFromFile] =  xMin_ + nBinsFromFile*(xMax_-xMin_)/nBins_;
    }
  }
  else{
    while (is.good())  
      {
	is.getline(c,999,',');     
	if (is.good() && nBinsFromFile<=nBins) {
	  bins[nBinsFromFile] = atof(c);
	  cout << bins[nBinsFromFile] << ", " ;
	}
	nBinsFromFile++;
      }
    cout << endl;
  }

  // Luminosity analyzed  
  float Lumi = (-47.4 + 215.3 + 913.5 + 410.5 + (450.6+203.7) + 2457. )* 1.00 ;

  float OStoSSRatioQCD            = 1.07;
  float WcorrectionFactorOS       = 0.96;  
  float WcorrectionFactorSS       = 1.20;  
  float EtoTauCorrectionFactor    = 1.00;
  float JtoTauCorrectionFactor    = 1.00;

  float embeddedMEtCutEff         = 1.00;
  float madgraphMEtCutEff         = 1.00;

  float NoVbfExtrapolationFactorZ = 0.993;
  float VbfExtrapolationFactorZ   = 1.004;
  float BoostExtrapolationFactorZ = 0.94;
  float VbfExtrapolationFactorW   = 1.004;
  float BoostExtrapolationFactorW = 0.94;

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1Corr" : "-(pZetaCorr-1.5*pZetaVisCorr)" ;
  float antiWsgn  = useMt ? 40. :  20. ; 
  float antiWsdb  = useMt ? 60. :  40. ; 

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TLegend* leg = new TLegend(0.55,0.42,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);
  leg->SetHeader(Form("#splitline{CMS Preliminary}{#splitline{%.1f fb^{-1} #sqrt{s}=7 TeV}{#tau_{e}#tau_{had}}}", Lumi/1000. ));

  THStack* aStack = new THStack("aStack","");

  TH1F* hSiml    = new TH1F( "hSiml"   ,"all"               , nBins , bins);
  TH1F* hSgn     = new TH1F( "hSgn "   ,"vbf+ggf"           , nBins , bins);
  TH1F* hSgn1    = new TH1F( "hSgn1"   ,"vbf"               , nBins , bins);
  TH1F* hSgn2    = new TH1F( "hSgn2"   ,"ggf"               , nBins , bins);
  TH1F* hSgn3    = new TH1F( "hSgn3"   ,"vh"                , nBins , bins);
  TH1F* hData    = new TH1F( "hData"   ,"Observed"          , nBins , bins);
  TH1F* hDataEmb = new TH1F( "hDataEmb","Embedded"          , nBins , bins);
  TH1F* hW       = new TH1F( "hW"      ,"W+jets"            , nBins , bins);
  TH1F* hEWK     = new TH1F( "hEWK"    ,"EWK"               , nBins , bins);
  TH1F* hZtt     = new TH1F( "hZtt"    ,"Ztautau"           , nBins , bins);
  TH1F* hZmm     = new TH1F( "hZmm"    ,"Z+jets, e  to tau" , nBins , bins);
  TH1F* hZmj     = new TH1F( "hZmj"    ,"Z+jets, jet to tau", nBins , bins);
  TH1F* hZfakes  = new TH1F( "hZfakes" ,"Z+jets, jet to tau", nBins , bins); 
  TH1F* hTTb     = new TH1F( "hTTb"    ,"ttbar"             , nBins , bins);
  TH1F* hQCD     = new TH1F( "hQCD"    ,"QCD"               , nBins , bins);
  TH1F* hVV      = new TH1F( "hVV"     ,"Diboson"           , nBins , bins);

  // pZeta OS, N pZ sideband OS, pZeta SS, N sideband SS, N QCD SS, OS/SS
  TH1F* hParameters   = new TH1F( "hParameters", "" , 7, 0,7);

  // Open the files
  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fDataEmbedded              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleRun2011-ElecTau-Embedded-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fSignalVBF         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",mH_) ,"READ");  
  TFile *fSignalGGH         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",mH_),"READ");
  TFile *fSignalVH         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",mH_),"READ");  
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleDYJets-ElecTau-50-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_06Dec2011//nTupleOthers-ElecTau-PUS6_run_Open_ElecTauStream.root","READ"); 

  // choose the analysis: Nominal "", jet up/Down "JetUp/Down" , elec up/down "MuUp/Down" , tau up/down "TauUp/Down"
  TString tree         = "outTreePtOrd"+analysis_;
  TString treeEmbedded = "outTreePtOrd";
  if(analysis_.find("TauUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("TauDown")!=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("ElecUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("ElecDown")!=string::npos) 
    treeEmbedded = tree;

  TTree *data                = (TTree*)fData->Get("outTreePtOrd");
  TTree *dataEmbedded        = EMBEDDEDSAMPLES ? (TTree*)fDataEmbedded->Get(treeEmbedded) : 0;
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *signalVH            = addVH ? (TTree*)fSignalVH->Get(tree) : 0;

  // split the DY->ll into l=e/mu and l=tau (MC level)
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> e+e- e->tau" << endl;
  TTree *backgroundDYEtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> e+e- e->tau
  cout << "Now copying g/Z -> e+e- j->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> e+e- jet->tau

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"      << endl;
  cout << backgroundDYEtoTau->GetEntries()  << " come from DY->ee, e->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->ee, jet->tau"<< endl;

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 
  TCut lpt("ptL1>20 && TMath::Abs(etaL1)<2.1 && tightestMVAWP>=1");
  TCut tpt("ptL2>20 && TMath::Abs(etaL2)<2.3 && tightestAntiEMVAWP>0.5");
  TCut tiso("tightestHPSDBWP>0");
  //TCut liso("combRelIsoLeg1DBeta<0.10");
  TCut liso("ptL1>0");
  TCut lveto("elecFlag==0");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut vbf("pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1");
  TCut novbf("pt1<150 && pt2<30");
  TCut boost("pt1>150 && !(pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1)");
  TCut bTag("nJets30<=1 && nJets20BTagged>=1");
  TCut nobTag("nJets30<=1 && nJets20BTagged==0");
  TCut hltevent("HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");
  //TCut hltevent("run==1 || (HLTx==1 && run>=163269)");
  //TCut hltmatch("run==1 || HLTmatch==1");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));

  TCut sbin; TCut sbinEmbedding; TCut sbinEmbeddingPZetaRel; TCut sbinPZetaRel; TCut sbinSS; TCut sbinPZetaRelSS; TCut sbinPZetaRev; TCut sbinPZetaRevSS;

  if(selection_.find("inclusive")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                         ;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                               ;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch;
  }
  else if(selection_.find("oneJet")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && oneJet;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && oneJet;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && oneJet;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && oneJet;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && oneJet;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && oneJet;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && oneJet;
  }
  else if(selection_.find("twoJets")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && twoJets;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && twoJets;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && twoJets;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && twoJets;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && twoJets;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && twoJets;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && twoJets;
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && vbf;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && vbf;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && vbf;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && vbf;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && vbf;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && vbf;
  }
  else if(selection_.find("novbf")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && novbf;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && novbf;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && novbf;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && novbf;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && novbf;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && novbf;
  }
  else if(selection_.find("boost")!=string::npos ){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && boost;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && boost;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && boost;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && boost;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && boost;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && boost;
  }
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && bTag;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && bTag;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && bTag;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && bTag;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && bTag;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && bTag;    
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && bTag;
  }
  else if(selection_.find("nobTag")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && nobTag;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && nobTag;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && nobTag;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && nobTag;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && nobTag;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && nobTag;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && nobTag;
  }

  // estimate the W+jets in the selection bin using pZeta extrapolation

  TH1F* hWMt = new TH1F("hWMt","",1,-10,10);

  ///////////////////////////////////////// Doing OS first...
  hWMt->Reset();
  backgroundWJets->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the W+jets OS sample" << endl;
  float OSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float OSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorOS = OSWinSidebandRegionMC/OSWinSignalRegionMC;

  if(useMt)
    cout << "Extrapolation factor for W OS : P(MtCorr>" << antiWsdb << ")/P(MtCorr<" << antiWsgn << ") ==> " << scaleFactorOS << endl;
  else
    cout << "Extrapolation factor for W OS : P(pZetaCorr<- "<< antiWsdb << ")/P(pZetaCorr>"<< antiWsgn << ") ==> " << scaleFactorOS << endl;    
 
  cout << "Estimating cobtribution from Ztt, ttbar and others in OS low pZeta tail..." << endl;
  hWMt->Reset();
  backgroundTTbar->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float ttbarExtrOS  = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from ttbar in OS is " << ttbarExtrOS << endl;
  hWMt->Reset();
  backgroundOthers->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float othersExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in OS is " << othersExtrOS << endl;
  hWMt->Reset();
  backgroundDYTauTau->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float dytautauExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from DY->tautau in OS is " << dytautauExtrOS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float dyjtotauExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from DY->ee, jet->tau in OS is " << dyjtotauExtrOS << endl;
  hWMt->Reset();
  backgroundDYEtoTau->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRel&&apZ));
  float dyetotauExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from DY->ee, e->tau in OS is " << dyjtotauExtrOS << endl;

  float OSWinSignalRegionDATA = data->GetEntries(sbinPZetaRev);
  cout << "Selected events in data in low pZeta/low Mt tail " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= ttbarExtrOS;
  OSWinSignalRegionDATA -= othersExtrOS;
  OSWinSignalRegionDATA -= dytautauExtrOS;
  OSWinSignalRegionDATA -= dyjtotauExtrOS;
  OSWinSignalRegionDATA -= dyetotauExtrOS;
  OSWinSignalRegionDATA /= scaleFactorOS;
  cout << "W+jets in signal region is estimated to be "  
       << OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS << " = " 
       << OSWinSignalRegionDATA <<  " +/- " << sqrt(OSWinSignalRegionDATA/scaleFactorOS)/scaleFactorOS << endl;
  cout << "  ===> the MC prediction was " << OSWinSignalRegionMC << endl;

  hParameters->SetBinContent(1, 1./scaleFactorOS );
  hParameters->SetBinContent(2, OSWinSignalRegionDATA*scaleFactorOS );

  ///////////////////////////////////////// Doing SS last...
  hWMt->Reset();
  backgroundWJets->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRelSS&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the SS W+jets sample" << endl;
  float SSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRelSS&&apZ));
  float SSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorSS = SSWinSidebandRegionMC/SSWinSignalRegionMC;
 
  if(useMt)
    cout << "Extrapolation factor for W SS : P(MtCorr>" << antiWsdb << ")/P(MtCorr<" << antiWsgn << ") ==> " << scaleFactorSS << endl;
  else
    cout << "Extrapolation factor for W SS : P(pZetaCorr<- "<< antiWsdb << ")/P(pZetaCorr>"<< antiWsgn << ") ==> " << scaleFactorSS << endl;    

  hWMt->Reset();
  cout << "Estimating contribution from Ztt, ttbar and others in SS low pZeta tail..." << endl;
  backgroundTTbar->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRelSS&&apZ));
  float ttbarExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from ttbar in SS is " << ttbarExtrSS << endl;
  hWMt->Reset();
  backgroundOthers->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRelSS&&apZ));
  float othersExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in SS is " << othersExtrSS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw("etaL1>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*(sbinPZetaRelSS&&apZ));
  float dyjtotauExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from DY->ee, jet->tau in SS is " << dytautauExtrOS << endl;
 
  float SSWinSignalRegionDATA = data->GetEntries(sbinPZetaRevSS);
  cout << "Selected events in data in low pZeta/low Mt tail " << SSWinSignalRegionDATA << endl;
  SSWinSignalRegionDATA -= ttbarExtrSS;
  SSWinSignalRegionDATA -= othersExtrSS;
  SSWinSignalRegionDATA -= dyjtotauExtrSS;
  SSWinSignalRegionDATA /= scaleFactorSS;
  cout << "W+jets in SS signal region is estimated to be "  
       << SSWinSignalRegionDATA*scaleFactorSS << "/" << scaleFactorSS << " = " 
       << SSWinSignalRegionDATA <<  " +/- " << sqrt(SSWinSignalRegionDATA/scaleFactorSS)/scaleFactorSS << endl;
  cout << "  ===> the MC prediction was " << SSWinSignalRegionMC << endl;

  hParameters->SetBinContent(3, 1./scaleFactorSS );
  hParameters->SetBinContent(4, SSWinSignalRegionDATA*scaleFactorSS );


  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("Data");
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  if(signalVH)
    samples.push_back("VH115");
  samples.push_back("Others");
  samples.push_back("TTbar");
  samples.push_back("SS");
  samples.push_back("WJets");
  samples.push_back("DYEtoTau");
  samples.push_back("DYJtoTau");
  samples.push_back("DYToTauTau");
  if(dataEmbedded)
    samples.push_back("Embedded");

  

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]      = data;
  tMap["Embedded"]  = dataEmbedded;
  tMap["ggH115"]    = signalGGH;
  tMap["qqH115"]    = signalVBF;
  tMap["VH115"]     = signalVH;
  tMap["DYToTauTau"]= backgroundDYTauTau;
  tMap["DYEtoTau"]  = backgroundDYEtoTau;
  tMap["DYJtoTau"]  = backgroundDYJtoTau;
  tMap["WJets"]     = backgroundWJets;
  tMap["Others"]    = backgroundOthers;
  tMap["TTbar"]     = backgroundTTbar;
  tMap["SS"]        = data;


  
  std::map<TString,Float_t> vMap;


  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << "Dealing with sample " << samples[iter] << endl;
    
    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,"" , nBins , bins);

    TTree* currentTree = 0;
    
    if((it->first).find("SS")!=string::npos){
      
      cout << "Remove W contamination from SS data sample ... " << endl;
      currentTree = (it->second);
      
      float error2OnQCD = 0.0;

      TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
      hHelp->Reset();
      currentTree->Draw(variable_+">>hHelp", sbinSS);
      int SSevents = hHelp->GetEntries();
      cout << "Selected SS events in data " << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);
      
      hHelp->Reset();
      backgroundWJets->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from W+jets (from " << hHelp->GetEntries() << " entries)" << endl;
      float sFWSS = 
	( selection_.find("novbf")!=string::npos || selection_.find("nobTag")!=string::npos || selection_.find("inclusive")!=string::npos) ? SSWinSignalRegionDATA/SSWinSignalRegionMC : WcorrectionFactorSS; // from the extrapolation factor DATA/MC
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) sFWSS *= VbfExtrapolationFactorW;
      else if(selection_.find("boost")!=string::npos)  sFWSS *= BoostExtrapolationFactorW;
      hHelp->Scale(sFWSS*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from W+jets by extrapolating" << endl;
      cout << " ==> removing W+jets from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow( hHelp->Integral()/hHelp->GetEntries(), 2)*hHelp->GetEntries(); // error on MC W+jets SS events
      error2OnQCD +=  pow(WcorrectionFactorSS*0.06,2)*pow(hHelp->GetEntries(),2);                                   // error on W+jets extrapolation factor ==> 6% according to Artur
      cout << sqrt(error2OnQCD) << " <==  W" << endl;      

      hHelp->Reset();
      backgroundTTbar->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from TTbar (from " << hHelp->GetEntries() << " entries)" << endl;
      hHelp->Scale(1.0*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from TTbar" << endl;
      cout << " ==> removing TTbar from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC TTbar SS events
      cout << sqrt(error2OnQCD) << " <== W + TTb" << endl;          

      hHelp->Reset();
      backgroundDYJtoTau->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from DY->ee, jet->tau" << endl;
      hHelp->Scale(JtoTauCorrectionFactor*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->ee, jet->tau" << endl;
      cout << " ==> removing DY->ee, jet->tau from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->ee, e->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1)" << endl;

      hHelp->Reset();
      backgroundDYEtoTau->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from DY->ee, e->tau" << endl;
      hHelp->Scale(EtoTauCorrectionFactor*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->ee, e->tau" << endl;
      cout << " ==> removing DY->ee, e->tau from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->ee, e->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1) + DY(2)" << endl;       

      //  OS/SS ratio
      h1->Scale(OStoSSRatioQCD);
      cout << "After removing the expected contribution from W+jets and rescaling by " << OStoSSRatioQCD << " we expect " 
	   << h1->Integral() << " events from QCD processes" << endl;

      hParameters->SetBinContent(5, SSevents);
      hParameters->SetBinContent(6, h1->Integral()/SSevents);

      cout << "Total unceratinty from bkg subtraction in SS region is " << sqrt(error2OnQCD) << endl;
      float totalRelErrorOnQCD = 0.02 + sqrt(error2OnQCD)/h1->Integral(); //0.02 ==> uncertainty on OS/SS ratio
      hParameters->SetBinContent(7,totalRelErrorOnQCD);

    }
    else{

      currentTree = (it->second);

      if((it->first).find("Embed")==string::npos){
	if((it->first).find("DYToTauTau")!=string::npos){
	  currentTree->Draw(variable_+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*HqTWeight)"*sbinPZetaRel);  
	  float madgraphNoMEtCut = h1->Integral();
	  h1->Reset();
	  currentTree->Draw(variable_+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*HqTWeight)"*sbin);
	  madgraphMEtCutEff = h1->Integral()/madgraphNoMEtCut;
	  cout << "Efficiency of antiW cut on madgraph " << madgraphMEtCutEff << endl;
	}
	else
	  currentTree->Draw(variable_+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightElec**SFElec*SFTau*SFEtoTau*HqTWeight)"*sbin);
      }
      else{
	currentTree->Draw(variable_+">>"+h1Name, "(HLTTau*HLTElec*embeddingWeight)"*sbinEmbeddingPZetaRel);
	float embeddedNoMEtCut = h1->Integral();
	h1->Reset();
	currentTree->Draw(variable_+">>"+h1Name, "(HLTTau*HLTElec*embeddingWeight)"*sbinEmbedding);
	
	embeddedMEtCutEff =  h1->Integral()/embeddedNoMEtCut;
	cout << "Efficiency of antiW cut on embedded " << embeddedMEtCutEff << endl;
      }

      // scale by correction factors
      if((it->first).find("Data")==string::npos)
	h1->Scale(Lumi/1000*hltEff_);

      // if W+jets, scale by extrapolation
      float sFWOS = 
	( selection_.find("novbf")!=string::npos || selection_.find("nobTag")!=string::npos || selection_.find("inclusive")!=string::npos) ? 
	OSWinSignalRegionDATA/OSWinSignalRegionMC : WcorrectionFactorOS;
      if((it->first).find("WJets")!=string::npos){
	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) sFWOS *= VbfExtrapolationFactorW;
	else if(selection_.find("boost")!=string::npos){
	  sFWOS *= BoostExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << BoostExtrapolationFactorW << " according to the Z->mumu+j+vbf/Z->mumu+j ratio" << endl;
	}
	h1->Scale( sFWOS );
	hW->Add(h1,1.0);
      }

      // if DY->tautau, and vbf scale by ratio data/MC
      if((it->first).find("DYToTauTau")!=string::npos){
	if(selection_.find("novbf")!=string::npos ){
	  cout << "DY->tautau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	  h1->Scale(NoVbfExtrapolationFactorZ);
	}
	else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  cout << "DY->tautau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	  h1->Scale(VbfExtrapolationFactorZ);
	}
	else if(selection_.find("boost")!=string::npos){
	  cout << "DY->tautau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	  h1->Scale( BoostExtrapolationFactorZ );
	}
      }

      // if DY->ee, e->tau, scale by fake-rate
      if((it->first).find("DYEtoTau")!=string::npos){
	float sF = EtoTauCorrectionFactor;
	if(selection_.find("novbf")!=string::npos ){
	  sF *= NoVbfExtrapolationFactorZ;
	  cout << "DY->ee, e->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	}
	else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  sF *= VbfExtrapolationFactorZ;
	  cout << "DY->ee, e->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	}
	else if(selection_.find("boost")!=string::npos){
	  cout << "DY->ee, e->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	  sF *= BoostExtrapolationFactorZ;
	}
	h1->Scale( sF );
	hZmm->Add(h1,1.0);
	hZfakes->Add(h1,1.0);
      }

      // if DY->ee, jet->tau, scale by fake-rate
      if((it->first).find("DYJtoTau")!=string::npos){
	float sF = JtoTauCorrectionFactor;
	if(selection_.find("novbf")!=string::npos ){
	  sF *= NoVbfExtrapolationFactorZ;
	  cout << "DY->ee, jet->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	}
	else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  sF *= VbfExtrapolationFactorZ;
	  cout << "DY->ee, jet->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	}
	else if(selection_.find("boost")!=string::npos){
	  sF *= BoostExtrapolationFactorZ;
	  cout << "DY->ee, jet->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	}
	h1->Scale( sF );
	hZmj->Add(h1,1.0);
	hZfakes->Add(h1,1.0);
      }

    }
  
    /////////////////////////////////////////////////////////////////////////////////////

    // Legend
    if( (it->first).find("DYEtoTau")!=string::npos ||  
	(it->first).find("DYJtoTau")!=string::npos || 
	(it->first).find("WJets")!=string::npos || 
	(it->first).find("Others")!=string::npos ) {
      hEWK->SetFillColor(kRed-3);
      hEWK->Add(h1,1.0);

      if( (it->first).find("Others")!=string::npos )
	hVV->Add(h1,1.0);
    }

    if( (it->first).find("DYToTauTau")!=string::npos ) {
      hZtt->Add(h1,1.0);
      hZtt->SetFillColor(kYellow-9);
    }
    if( (it->first).find("Embedded")!=string::npos ) {
      hDataEmb->Add(h1,1.0);
      if(hZtt->Integral()>0) hDataEmb->Scale(hZtt->Integral()/hDataEmb->Integral());
      hDataEmb->Scale(embeddedMEtCutEff/madgraphMEtCutEff);
      hDataEmb->SetFillColor(kYellow-9);
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      hTTb->Add(h1,1.0);
      hTTb->SetFillColor(kBlue-2);     
    }
    if( (it->first).find("SS")!=string::npos ) {
      hQCD->Add(h1,1.0);
      hQCD->SetFillColor(kMagenta-9);
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1->Add(h1,1.0);
      hSgn1->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2->Add(h1,1.0);
      hSgn2->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn2->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find("VH115")!=string::npos){
      hSgn3->Add(h1,1.0);
      hSgn3->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn3->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find("Data")!=string::npos){
      hData->Add(h1,1.0);
      hData->Sumw2();
      hData->SetMarkerStyle(20);
      hData->SetMarkerSize(1.2);
      hData->SetMarkerColor(kBlack);
      hData->SetLineColor(kBlack);
      hData->SetXTitle(XTitle_+" ("+Unities_+")");
      hData->SetYTitle(Form(" Events/(%.1f %s)", hData->GetBinWidth(1), Unities_.Data() ) );
      hData->SetTitleSize(0.04,"X");
      hData->SetTitleSize(0.05,"Y");
      hData->SetTitleOffset(0.95,"Y");
    }

    // adding alltogether
    hSiml->Add(h1,1.0);

    if(VERBOSE) cout<<(it->first) << " ==> " 
		    << h1->Integral() << " +/- " 
		    << TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries())
		    << endl;
  }

  // all signal summed together:
  hSgn->Add(hSgn1,hSgn2,1,1);
  hSgn->Add(hSgn3);
  hSgn->SetFillColor(0);
  hSgn->SetLineColor(kBlue);
  hSgn->SetLineWidth(2);
  hSgn->SetLineStyle(kDashed);

  //Adding to the stack
  aStack->Add(hQCD);
  aStack->Add(hEWK);
  aStack->Add(hTTb);
  if(useEmbedding_)
    aStack->Add(hDataEmb);
  else
    aStack->Add(hZtt);
  aStack->Add(hSgn);

  // legend
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hSgn,Form("(%.0fx) H#rightarrow#tau#tau m_{H}=%d",magnifySgn_,mH_),"F");
  if(useEmbedding_)
    leg->AddEntry(hZtt,"Z#rightarrow#tau#tau (embedded)","F");
  else
    leg->AddEntry(hZtt,"Z#rightarrow#tau#tau","F");
  leg->AddEntry(hTTb,"t#bar{t}","F");
  leg->AddEntry(hEWK,"Electroweak","F");
  leg->AddEntry(hQCD,"QCD","F");
  
  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");
  
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle(XTitle_+" ("+Unities_+")");
  hStack->SetYTitle(Form(" Events/(%.0f %s)", hStack->GetBinWidth(1), Unities_.Data() ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.95,"Y");

  leg->Draw();

  return;

  //c1->SaveAs(Form("plots/%s/plot_eTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));

  // templates for fitting
  TFile* fout = new TFile(Form("histograms/%s/eTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");
  fout->cd();
  hQCD->Write();
  hZmm->Write();
  hZmj->Write();
  hZfakes->Write();
  hTTb->Write();
  hZtt->Write();
  hDataEmb->Write();
  hW->Write();
  hVV->Write();
  hSgn1->Write();
  hSgn2->Write();
  hData->Write();
  hParameters->Write();
  fout->Write();
  fout->Close();

  delete hQCD; delete hZmm; delete hZmj; delete hTTb; delete hZtt; delete hW; delete hZfakes;
  delete hVV; delete hSgn1; delete hSgn2; delete hData; delete hParameters;
  delete hWMt; delete aStack; delete hEWK; delete hSgn3; delete hDataEmb; delete hSgn; delete hSiml;
  delete fout;
  fSignalGGH->Close();       delete fSignalGGH; 
  fSignalVBF->Close();       delete fSignalVBF;
  fBackgroundOthers->Close();delete fBackgroundOthers;
  fBackgroundTTbar->Close(); delete fBackgroundTTbar;
  fBackgroundWJets->Close(); delete fBackgroundWJets;
  fData->Close();            delete fData; 
  dummy1->Close();           delete dummy1;
  fBackgroundDY->Close();    delete fBackgroundDY;

}


///////////////////////////////////////////////////////////////////////////////////////////////



void plotElecTauAll(  Int_t useEmbedded = 1, TString outputDir = "Oct2011/CutBased" ){

  vector<string> variables;
  vector<int> mH;
  
  //variables.push_back("diTauVisMass");
  //variables.push_back("diTauNSVfitMass");
  variables.push_back("diTauSVFitMass");
  
  //mH.push_back(100);

  mH.push_back(105);

  //mH.push_back(110);
  //mH.push_back(115);

  mH.push_back(120);

  //mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);

  mH.push_back(140);

  //mH.push_back(145);
  
  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){

      plotElecTau(mH[j], useEmbedded, "vbf",""         ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","TauUp"    ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","TauDown"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","JetUp"    ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","JetDown"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","ElecUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "vbf","ElecDown" ,variables[i],"mass","GeV", outputDir,-1);

      plotElecTau(mH[j], useEmbedded, "novbf",""        ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","TauUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","TauDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","JetUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","JetDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","ElecUp"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "novbf","ElecDown",variables[i],"mass","GeV", outputDir,-1);

      plotElecTau(mH[j], useEmbedded, "boost",""        ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","TauUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","TauDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","JetUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","JetDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","ElecUp"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "boost","ElecDown",variables[i],"mass","GeV", outputDir,-1);

      plotElecTau(mH[j], useEmbedded, "twoJets",""        ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","TauUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","TauDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","JetUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","JetDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","ElecUp"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "twoJets","ElecDown",variables[i],"mass","GeV", outputDir,-1);
      
      plotElecTau(mH[j], useEmbedded, "oneJet",""        ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","TauUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","TauDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","JetUp"   ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","JetDown" ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","ElecUp"  ,variables[i],"mass","GeV", outputDir,-1);
      plotElecTau(mH[j], useEmbedded, "oneJet","ElecDown",variables[i],"mass","GeV", outputDir,-1);

    }
  }
  
  

}
