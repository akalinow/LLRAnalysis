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

#include "HiggsAnalysis/CombinedLimit/interface/TH1Keys.h"

#define VERBOSE          true
#define SAVE             true
#define addVH            true
#define EMBEDDEDSAMPLES  true
#define W3JETS           true
#define LOOSEISO         true
#define kFactorSM         1.0

#define USESSBKG         false
#define scaleByBinWidth  false
#define DOSPLIT          false
#define studyQCDshape    false
#define useZDataMC       false
///////////////////////////////////////////////////////////////////////////////////////////////

void makeHistoFromDensity(TH1* hDensity, TH1* hHistogram){

  if(hDensity->GetNbinsX() != hHistogram->GetNbinsX()){
    cout << "makeHistoFromDensity: different binning" << endl;
    return;
  }

  for(int k = 1 ; k <= hDensity->GetNbinsX(); k++){
    float bink   = hDensity->GetBinContent(k);
    float widthk = hHistogram->GetBinWidth(k);
    hDensity->SetBinContent(k, bink*widthk );
  }
  hDensity->Scale(hHistogram->Integral()/hDensity->Integral());
}

///////////////////////////////////////////////////////////////////////////////////////////////


void createStringsIsoFakeRate(TString fileName = "FakeRate.root",
			      string& scaleFactMu     = *( new string()), 
			      string& scaleFactMuUp   = *( new string()),
			      string& scaleFactMuDown = *( new string()),
			      string variableX = "ptL1",
			      string variableY = "FakeRate",
			      string interspace = "_",
			      string selection = "Mu_ptL1_incl",
			      bool verbose = false){

 TFile FakeRate(fileName,"READ");
 if(FakeRate.IsZombie()){
   cout << "Missing FR histos... create dummy ones" << endl;
   scaleFactMu     = "(etaL1<999)";
   scaleFactMuUp   = "(etaL1<999)";
   scaleFactMuDown = "(etaL1<999)";
   return;
 }
 
 TF1*  frMu     = (TF1*)FakeRate.Get(("fit"+interspace+selection).c_str());
 TH1F* frMuUp   = (TH1F*)FakeRate.Get(("h"+variableY+"ErrUp"+selection).c_str());
 TH1F* frMuDown = (TH1F*)FakeRate.Get(("h"+variableY+"ErrDown"+selection).c_str());

 if(!frMu || !frMuUp || !frMuDown){
   cout << "Missing FR histos... exit" << endl;
   return;
 }
 
 vector<int> binsFR;
 binsFR.push_back(17);  binsFR.push_back(20);
 binsFR.push_back(22);  binsFR.push_back(24);
 binsFR.push_back(26);  binsFR.push_back(28);
 binsFR.push_back(30);  binsFR.push_back(32);
 binsFR.push_back(34);  binsFR.push_back(36);
 binsFR.push_back(40);  binsFR.push_back(45);
 binsFR.push_back(50);  binsFR.push_back(60); 
 binsFR.push_back(80);  binsFR.push_back(100);
 binsFR.push_back(9999);

 scaleFactMu     = "( ";
 scaleFactMuUp   = "( ";
 scaleFactMuDown = "( ";

 for(unsigned int i = 0; i < binsFR.size()-1; i++){
   
    float min = binsFR[i];
    float max = binsFR[i+1];
    
    float bin = frMuUp->FindBin((max+min)/2.);
    if( bin == frMuUp->GetNbinsX() + 1) bin--;
    
    float weightBinMu_i     = frMu->Eval( (max+min)/2.)>0 ?       1./frMu->Eval( (max+min)/2.)      : 0.0;
    float weightBinMu_iUp   = frMuUp->GetBinContent( bin )>0 ?    1./frMuUp->GetBinContent( bin )   : 0.0;
    float weightBinMu_iDown = frMuDown->GetBinContent( bin )>0 ?  1./frMuDown->GetBinContent( bin ) : 0.0;
    
    scaleFactMu     += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinMu_i ) );
    scaleFactMuUp   += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinMu_iUp   ) );
    scaleFactMuDown += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinMu_iDown ) );

    if(i < binsFR.size() - 2 ){
      scaleFactMu     += " + ";
      scaleFactMuUp   += " + ";
      scaleFactMuDown += " + ";
    }
 }
 
 scaleFactMu     += " )";
 scaleFactMuUp   += " )";
 scaleFactMuDown += " )";
 
 if(verbose){
   cout << scaleFactMu << endl;
   cout << scaleFactMuUp << endl;
   cout << scaleFactMuDown << endl;
 }

}

void drawHistogramMC(TTree* tree = 0, 
		     TString variable = "", 
		     float& normalization      = *(new float()), 
		     float& normalizationError = *(new float()), 
		     float scaleFactor = 0., 
		     TH1F* h = 0, 
		     TCut cut = TCut(""),
		     int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    tree->Draw(variable+">>"+TString(h->GetName()),"(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*weightHepNup)"*cut);
    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
    if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramMC has raised an error" << endl;
    return;
  }
}



void drawHistogramEmbed(TTree* tree = 0, 
			TString variable = "", 
			float& normalization      = *(new float()), 
			float& normalizationError = *(new float()), 
			float scaleFactor = 0., 
			TH1F* h = 0, 
			TCut cut = TCut(""),
			int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    tree->Draw(variable+">>"+TString(h->GetName()),"(HLTTau*HLTMu*embeddingWeight)"*cut);
    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
    if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramEmbed has raised an error" << endl;
    return;
  }
}

void drawHistogramData(TTree* tree = 0, 
		       TString variable = "", 
		       float& normalization      = *(new float()), 
		       float& normalizationError = *(new float()), 
		       float scaleFactor = 0., 
		       TH1F* h = 0, 
		       TCut cut = TCut(""),
		       int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    tree->Draw(variable+">>"+TString(h->GetName()),cut);
    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
    if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramData has raised an error" << endl;
    return;
  }
}

void drawHistogramDataFakeRate(TTree* tree = 0, 
			       TString variable = "", 
			       float& normalization      = *(new float()), 
			       float& normalizationError = *(new float()), 
			       float scaleFactor = 0., 
			       TH1F* h = 0, 
			       TCut cut = TCut(""),
			       string scaleFact = "",
			       int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    tree->Draw(variable+">>"+TString(h->GetName()),TCut(scaleFact.c_str())*cut);
    normalization      = h->Integral()*scaleFactor;
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
     if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramDataFakeRate has raised an error" << endl;
    return;
  }
}

void drawHistogramMCFakeRate(TTree* tree = 0, 
			     TString variable = "", 
			     float& normalization      = *(new float()), 
			     float& normalizationError = *(new float()), 
			     float scaleFactor = 0., 
			     TH1F* h = 0, 
			     TCut cut = TCut(""),
			     string scaleFact = "",
			     int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    TString cutWeight = "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*weightHepNup)*"+scaleFact;
    tree->Draw(variable+">>"+TString(h->GetName()),TCut(cutWeight.Data())*cut);
    normalization      = h->Integral()*scaleFactor;
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
     if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramMCFakeRate has raised an error" << endl;
    return;
  }
}

TArrayF createBins(int nBins_ = 80 ,
		   float xMin_ = 0.,
		   float xMax_ = 400.,
		   int& nBins = *(new int()),
		   string selection_   = "inclusive",
		   TString variable_   = "diTauVisMass",
		   //TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_Sep12/src/Bianchi/Limits/bin/results/bins/"
		   TString location    = "/data_CMS/cms/htautau/HCP12/binning/"
		   ){

  // input txt file with bins
  ifstream is;

  TArrayF dummy(2);
  dummy[0] = xMin_; dummy[1] = xMax_;
  
  char* c = new char[10];
  is.open(Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  if(nBins_<0 &&  !is.good()){
    cout << "Bins file not found" << endl;
    return dummy;
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
  nBins =  nBins_>0 ? nBins_ : nBinsFromFile-1 ;
  TArrayF bins(nBins+1);
  cout << "Making histograms with " << nBins << " bins:" << endl;

  is.close();
  is.open(Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  
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

  return bins;

}

void evaluateWextrapolation(string sign = "OS", bool useFakeRate = false, string selection_ = "",
			    float& scaleFactorOS= *(new float()), 
			    float& OSWinSignalRegionDATA= *(new float()),   float& OSWinSignalRegionMC = *(new float()), 
			    float& OSWinSidebandRegionDATA= *(new float()), float& OSWinSidebandRegionMC = *(new float()), 
			    float& scaleFactorTTOS = *(new float()),
			    TH1F* hWMt=0, TString variable="",
			    TTree* backgroundWJets=0, TTree* backgroundTTbar=0, TTree* backgroundOthers=0, 
			    TTree* backgroundDYTauTau=0, TTree* backgroundDYJtoTau=0, TTree* backgroundDYMutoTau=0, TTree* data=0,
			    float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor=0.,
			    float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
			    float MutoTauCorrectionFactor = 0., float JtoTauCorrectionFactor = 0.,
			    float antiWsdb = 0., float antiWsgn = 0., bool useMt = true,
			    string scaleFactMu = "",
			    TCut sbinPZetaRelForWextrapolation = "",
			    TCut sbinPZetaRel = "",  TCut sbinRelPZetaRel = "",
			    TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="",
			    TCut sbinPZetaRelaIsoInclusive = "", TCut sbinPZetaRelaIso = "", 
			    TCut vbf="", TCut boost="", TCut zeroJet = ""){
  
  float Error = 0.; float ErrorW1 = 0.;   float ErrorW2 = 0.;
  drawHistogramMC(backgroundWJets,variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRelForWextrapolation&&pZ);
  drawHistogramMC(backgroundWJets,variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRelForWextrapolation&&apZ);
  scaleFactorOS      = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;
  float scaleFactorOSError = scaleFactorOS*(ErrorW1/OSWinSignalRegionMC + ErrorW2/OSWinSidebandRegionMC);
  if(useMt)
    cout << "Extrap. factor for W " << sign << " : P(Mt>"     << antiWsdb << ")/P(Mt<"   << antiWsgn << ") ==> " <<OSWinSidebandRegionMC<<"/"<<OSWinSignalRegionMC<<" = "<< scaleFactorOS << " +/- " << scaleFactorOSError << endl;
  else
    cout << "Extrap. factor for W " << sign << " : P(pZeta<- "<< antiWsdb << ")/P(pZeta>"<< antiWsgn << ") ==> " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;    

  // restore with full cut
  drawHistogramMC(backgroundWJets,variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRel&&pZ);
  // restore with full cut
  drawHistogramMC(backgroundWJets,variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRel&&apZ);
 

  float OSTTbarinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundTTbar,  variable,  OSTTbarinSidebandRegionMC,     Error, scaleFactor*TTxsectionRatio , hWMt, sbinPZetaRel&&apZ);


  TCut bTagCut; TCut bTagCutaIso;
  if(selection_.find("novbf")!=string::npos){
    bTagCut     = sbinPZetaRelInclusive    &&apZ&&TCut("nJets20BTagged>1")&&zeroJet;
    bTagCutaIso = sbinPZetaRelaIsoInclusive&&apZ&&TCut("nJets20BTagged>1")&&zeroJet;
  }
  else if(selection_.find("boost")!=string::npos){
    bTagCut     = sbinPZetaRelInclusive    &&apZ&&TCut("nJets20BTagged>1")&&boost;
    bTagCutaIso = sbinPZetaRelaIsoInclusive&&apZ&&TCut("nJets20BTagged>1")&&boost;
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    bTagCut     = sbinPZetaRelInclusive    &&apZ&&TCut("nJets20BTagged>0")&&vbf;
    bTagCutaIso = sbinPZetaRelaIsoInclusive&&apZ&&TCut("nJets20BTagged>0")&&vbf;
  }
  else{
    bTagCut     = sbinPZetaRel             &&apZ&&TCut("nJets20BTagged>1");
    bTagCutaIso = sbinPZetaRelaIso         &&apZ&&TCut("nJets20BTagged>1");
  }

  float OSTTbarinSidebandRegionBtagMC = 0.;
  drawHistogramMC(backgroundTTbar, variable, OSTTbarinSidebandRegionBtagMC,  Error, scaleFactor*TTxsectionRatio, hWMt, bTagCut);
  float OSWinSidebandRegionBtagMC = 0.;
  drawHistogramMC(backgroundWJets, variable, OSWinSidebandRegionBtagMC,      Error, scaleFactor                , hWMt, bTagCut);
  float OSOthersinSidebandRegionBtagMC = 0.;
  drawHistogramMC(backgroundOthers, variable, OSOthersinSidebandRegionBtagMC,Error, scaleFactor                , hWMt, bTagCut);
  float OSQCDinSidebandRegionBtag = 0.;
  drawHistogramDataFakeRate(data, variable, OSQCDinSidebandRegionBtag,       Error, 1.0                        , hWMt, bTagCutaIso, scaleFactMu);
  float OSWinSidebandRegionBtagAIsoMC = 0.;
  drawHistogramMCFakeRate(backgroundWJets, variable, OSWinSidebandRegionBtagAIsoMC,  Error, scaleFactor        , hWMt, bTagCutaIso, scaleFactMu);
  float OSDatainSidebandRegionBtag = 0.;
  drawHistogramData(data, variable, OSDatainSidebandRegionBtag,  Error, 1.0 , hWMt, bTagCut);

  scaleFactorTTOS = OSTTbarinSidebandRegionBtagMC>0 ? 
    (OSDatainSidebandRegionBtag-
     OSOthersinSidebandRegionBtagMC-
     OSWinSidebandRegionBtagMC-
     (OSQCDinSidebandRegionBtag-OSWinSidebandRegionBtagAIsoMC))/OSTTbarinSidebandRegionBtagMC : 1.0;
  cout << "Normalizing TTbar from sideband: " << OSTTbarinSidebandRegionBtagMC << " events expected from TTbar." << endl 
       << "From WJets " << OSWinSidebandRegionBtagMC <<  ", from QCD " << OSQCDinSidebandRegionBtag << " (of which " 
       << OSWinSidebandRegionBtagAIsoMC << " expected from anti-isolated W)"
       << ", from others " << OSOthersinSidebandRegionBtagMC << endl;
  cout << "Observed " << OSDatainSidebandRegionBtag << endl;
  cout << "====> scale factor for " << sign << " TTbar is " << scaleFactorTTOS << endl;
  if(scaleFactorTTOS<0){
    cout << "!!! scale factor is negative... set it to 1 !!!" << endl;
    scaleFactorTTOS = 1.0;
  }
  OSTTbarinSidebandRegionMC *= scaleFactorTTOS; // to comment
  cout << "Contribution from TTbar in " << sign << " is " << OSTTbarinSidebandRegionMC << endl;

  float OSOthersinSidebandRegionMC   = 0.;
  drawHistogramMC(backgroundOthers,    variable, OSOthersinSidebandRegionMC  ,Error,  scaleFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYtoTauinSidebandRegionMC  = 0.;
  if(useZDataMC)
    drawHistogramMC(backgroundDYTauTau,  variable, OSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor*ExtrapolationFactorSidebandZDataMC*ExtrapolationFactorZDataMC , hWMt, sbinPZetaRel&&apZ);
  else 
    drawHistogramMC(backgroundDYTauTau,  variable, OSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor, hWMt, sbinPZetaRel&&apZ);
  float OSDYJtoTauinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundDYJtoTau,  variable, OSDYJtoTauinSidebandRegionMC ,Error, scaleFactor*lumiCorrFactor*JtoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYMutoTauinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundDYMutoTau, variable, OSDYMutoTauinSidebandRegionMC ,Error,scaleFactor*lumiCorrFactor*MutoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);

  float OSQCDinSidebandRegionData = 0.; float OSAIsoEventsinSidebandRegionData = 0.;
  drawHistogramDataFakeRate(data,  variable,        OSQCDinSidebandRegionData,   Error, 1.0         , hWMt, sbinPZetaRelaIso&&apZ, scaleFactMu);
  OSAIsoEventsinSidebandRegionData =  (OSQCDinSidebandRegionData/Error)*(OSQCDinSidebandRegionData/Error);
  float OSWinSidebandRegionAIsoMC  = 0.;float OSAIsoEventsWinSidebandRegionAIsoMC  = 0.;
  drawHistogramMCFakeRate(backgroundWJets, variable,OSWinSidebandRegionAIsoMC,   Error, scaleFactor , hWMt, sbinPZetaRelaIso&&apZ, scaleFactMu);
  OSAIsoEventsWinSidebandRegionAIsoMC = (OSWinSidebandRegionAIsoMC/Error)*(OSWinSidebandRegionAIsoMC/Error)*scaleFactor; 

  drawHistogramData(data, variable, OSWinSignalRegionDATA ,Error, 1.0 , hWMt, sbinPZetaRel&&apZ);
  cout << "Selected events in " << sign << " data from high Mt sideband " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= OSTTbarinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSOthersinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYtoTauinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYJtoTauinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYMutoTauinSidebandRegionMC;
  if(useFakeRate) OSWinSignalRegionDATA -= (OSQCDinSidebandRegionData-OSWinSidebandRegionAIsoMC);
  OSWinSignalRegionDATA /= scaleFactorOS;
  cout << "- expected from TTbar          " << OSTTbarinSidebandRegionMC << endl;
  cout << "- expected from Others         " << OSOthersinSidebandRegionMC << endl;
  cout << "- expected from DY->tautau     " << OSDYtoTauinSidebandRegionMC << endl;
  cout << "- expected from DY->ll, l->tau " << OSDYMutoTauinSidebandRegionMC << endl;
  cout << "- expected from DY->ll, j->tau " << OSDYJtoTauinSidebandRegionMC  << endl;
  cout << "- expected from QCD " << OSQCDinSidebandRegionData << ", obtained from " << OSAIsoEventsinSidebandRegionData << " anti-isolated events " << endl;
  cout << "  (MC predicts " << OSWinSidebandRegionAIsoMC << " W events in the aIso region, from a total of " << OSAIsoEventsWinSidebandRegionAIsoMC << " events)" << endl;
  if(!useFakeRate) cout << " !!! QCD with fake-rate not subtracted !!!" << endl;
  cout << "W+jets in " << sign << " region is estimated to be " <<  OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS
       << " = " <<  OSWinSignalRegionDATA << endl;
  cout << " ==> the MC prediction is " << OSWinSignalRegionMC << " +/- " << ErrorW1 << endl;

  OSWinSidebandRegionDATA = OSWinSignalRegionDATA*scaleFactorOS;
}

void evaluateQCD(TH1F* qcdHisto = 0, TH1F* ssHisto = 0, bool evaluateWSS = true, string sign = "SS", bool useFakeRate = false, bool removeMtCut = false, string selection_ = "", 
		 float& SSQCDinSignalRegionDATAIncl_ = *(new float()), float& SSIsoToSSAIsoRatioQCD = *(new float()), float& scaleFactorTTSSIncl = *(new float()),
		 float& extrapFactorWSSIncl = *(new float()), 
		 float& SSWinSignalRegionDATAIncl = *(new float()), float& SSWinSignalRegionMCIncl = *(new float()),
		 float& SSWinSidebandRegionDATAIncl = *(new float()), float& SSWinSidebandRegionMCIncl = *(new float()),
		 TH1F* hExtrap=0, TString variable = "",
		 TTree* backgroundWJets=0, TTree* backgroundTTbar=0, TTree* backgroundOthers=0, 
		 TTree* backgroundDYTauTau=0, TTree* backgroundDYJtoTau=0, TTree* backgroundDYMutoTau=0, TTree* data=0,
		 float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor = 0.,
		 float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
		 float  MutoTauCorrectionFactor=0. , float JtoTauCorrectionFactor=0.,
		 float OStoSSRatioQCD = 0.,
		 float antiWsdb=0., float antiWsgn=0., bool useMt=true,
		 string scaleFactMu="",
		 TCut sbin = "",
		 TCut sbinPZetaRelForWextrapolation = "",
		 TCut sbinPZetaRel ="", TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="", 
		 TCut sbinPZetaRelaIsoInclusive="", TCut sbinPZetaRelaIso="", TCut sbinPZetaRelaIsoSideband = "", 
		 TCut vbf="", TCut boost="", TCut zeroJet="",
		 bool subtractTT=true, bool subtractVV=true){

  if(evaluateWSS)
    evaluateWextrapolation(sign, useFakeRate, selection_ , 
			   extrapFactorWSSIncl, 
			   SSWinSignalRegionDATAIncl,   SSWinSignalRegionMCIncl,
			   SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
			   scaleFactorTTSSIncl,
			   hExtrap, variable,
			   backgroundWJets, backgroundTTbar, backgroundOthers, 
			   backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
			   scaleFactor, TTxsectionRatio,lumiCorrFactor,
			   ExtrapolationFactorSidebandZDataMC,ExtrapolationFactorZDataMC, 
			   MutoTauCorrectionFactor, JtoTauCorrectionFactor,
			   antiWsdb, antiWsgn, useMt,
			   scaleFactMu,
			   sbinPZetaRelForWextrapolation,
			   sbinPZetaRel, sbinPZetaRel,
			   pZ, apZ, sbinPZetaRelInclusive, 
			   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbf, boost, zeroJet);
  
  float Error = 0.;
  float SSQCDinSignalRegionDATAIncl = 0.;
  drawHistogramData(data, variable,              SSQCDinSignalRegionDATAIncl,        Error, 1.0,         hExtrap, sbin, 1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap,  1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap,  1.0);

  float SSWJetsinSidebandRegionMCIncl    = 0.;
  drawHistogramMC(backgroundWJets,     variable, SSWJetsinSidebandRegionMCIncl,      Error, scaleFactor*(SSWinSidebandRegionDATAIncl/SSWinSidebandRegionMCIncl), hExtrap, sbin,1);
  if(!removeMtCut){
    hExtrap->Scale(SSWinSignalRegionDATAIncl/hExtrap->Integral());
    SSWJetsinSidebandRegionMCIncl = SSWinSignalRegionDATAIncl;
  }
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);

  float SSTTbarinSidebandRegionMCIncl    = 0.;
  if(subtractTT) {
    drawHistogramMC(backgroundTTbar,     variable, SSTTbarinSidebandRegionMCIncl,      Error, scaleFactor*TTxsectionRatio*scaleFactorTTSSIncl,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSOthersinSidebandRegionMCIncl    = 0.;
  if(subtractVV) {
    drawHistogramMC(backgroundOthers,     variable, SSOthersinSidebandRegionMCIncl,     Error, scaleFactor,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSDYMutoTauinSidebandRegionMCIncl = 0.;
  drawHistogramMC(backgroundDYMutoTau, variable, SSDYMutoTauinSidebandRegionMCIncl,  Error, lumiCorrFactor*scaleFactor*MutoTauCorrectionFactor,    hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauinSidebandRegionMCIncl = 0.;
  if(useZDataMC)
    drawHistogramMC(backgroundDYTauTau,  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor*ExtrapolationFactorZDataMC, hExtrap, sbin,1);
  else
    drawHistogramMC(backgroundDYTauTau,  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYJtoTauinSidebandRegionMCIncl = 0.;
  drawHistogramMC(backgroundDYJtoTau,  variable, SSDYJtoTauinSidebandRegionMCIncl,   Error, lumiCorrFactor*scaleFactor*JtoTauCorrectionFactor,     hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  cout << "Selected events in inclusive " << sign << " data " << SSQCDinSignalRegionDATAIncl << endl;
  SSQCDinSignalRegionDATAIncl  -= SSWJetsinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSTTbarinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSOthersinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYMutoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYJtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl *= OStoSSRatioQCD;
  if(qcdHisto!=0) qcdHisto->Scale(OStoSSRatioQCD);

  cout << "- expected from WJets          " << SSWJetsinSidebandRegionMCIncl << endl;
  cout << "- expected from TTbar          " << SSTTbarinSidebandRegionMCIncl << endl;
  cout << "- expected from Others         " << SSOthersinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->tautau     " << SSDYtoTauinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->ll, l->tau " << SSDYMutoTauinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->ll, j->tau " << SSDYJtoTauinSidebandRegionMCIncl  << endl;
  cout << "QCD in inclusive SS region is estimated to be " << SSQCDinSignalRegionDATAIncl/OStoSSRatioQCD  << "*" << OStoSSRatioQCD
       << " = " <<  SSQCDinSignalRegionDATAIncl << endl;
  SSQCDinSignalRegionDATAIncl_ = SSQCDinSignalRegionDATAIncl;

  float SSQCDinSignalRegionDATAInclaIso = 0.;
  drawHistogramData(data, variable, SSQCDinSignalRegionDATAInclaIso,    Error, 1.0, hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSWinSignalRegionMCInclaIso   = 0.;
  drawHistogramMC(backgroundWJets, variable, SSWinSignalRegionMCInclaIso,       Error,   scaleFactor*(SSWinSignalRegionDATAIncl/SSWinSignalRegionMCIncl) , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSTTbarinSignalRegionMCInclaIso   = 0.;
  drawHistogramMC(backgroundTTbar, variable, SSTTbarinSignalRegionMCInclaIso,   Error,   scaleFactor*scaleFactorTTSSIncl , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  cout << "Anti-isolated " << sign << " events inclusive = " << SSQCDinSignalRegionDATAInclaIso << ", of which we expect "
       << SSWinSignalRegionMCInclaIso << " from W+jets " << " and " << SSTTbarinSignalRegionMCInclaIso << " from TTbar" << endl;

  SSIsoToSSAIsoRatioQCD = (SSQCDinSignalRegionDATAIncl)/(SSQCDinSignalRegionDATAInclaIso-SSWinSignalRegionMCInclaIso-SSTTbarinSignalRegionMCInclaIso) ;
  cout << "The extrapolation factor Iso<0.1 / Iso>0.2 is " << SSIsoToSSAIsoRatioQCD << endl;

}


void cleanQCDHisto(bool removeW = true, TH1F* hCleaner = 0, TH1F* hLooseIso = 0, TString variable = "",
		   TTree* backgroundWJets = 0, TTree* backgroundTTbar = 0, TTree* backgroundOthers = 0, 
		   TTree* backgroundDYMutoTau=0, TTree* backgroundDYJtoTau=0, TTree* backgroundDYTauTau=0,
		   float scaleFactor = 0., float WJetsCorrectionFactor=0., float TTbarCorrectionFactor=0.,
		   float MutoTauCorrectionFactor=0., float JtoTauCorrectionFactor=0., float DYtoTauTauCorrectionFactor=0.,
		   TCut sbinSSlIso1 = ""){

  if(hLooseIso==0) return;

  cout << "Cleaning QCD histo with relaxed isolation from backgrounds..." << endl;

  float Error = 0.;
  float totalEvents = hLooseIso->Integral();

  float NormalizationWinLooseRegion = 0.;
  if(removeW){
    drawHistogramMC(backgroundWJets,variable,      NormalizationWinLooseRegion,     Error, scaleFactor*WJetsCorrectionFactor, hCleaner, sbinSSlIso1 ,1);
    hLooseIso->Add(hCleaner,-1.0);
  }
  float NormalizationTTbarinLooseRegion = 0.;
  drawHistogramMC(backgroundTTbar, variable,     NormalizationTTbarinLooseRegion, Error, scaleFactor*TTbarCorrectionFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationOthersinLooseRegion = 0.;
  drawHistogramMC(backgroundOthers, variable,    NormalizationOthersinLooseRegion,Error, scaleFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYMutotauinLooseRegion= 0.;
  drawHistogramMC(backgroundDYMutoTau, variable, NormalizationDYMutotauinLooseRegion,  Error,  scaleFactor*MutoTauCorrectionFactor,    hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYJtotauinLooseRegion= 0.;
  drawHistogramMC(backgroundDYJtoTau, variable,  NormalizationDYJtotauinLooseRegion,   Error,  scaleFactor*JtoTauCorrectionFactor,     hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationDYTauTauinLooseRegion = 0.;
  drawHistogramMC(backgroundDYTauTau,  variable, NormalizationDYTauTauinLooseRegion,    Error, scaleFactor*DYtoTauTauCorrectionFactor, hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);

  float totalRemoved = NormalizationWinLooseRegion+NormalizationTTbarinLooseRegion+NormalizationOthersinLooseRegion+
    NormalizationDYMutotauinLooseRegion+NormalizationDYJtotauinLooseRegion+NormalizationDYTauTauinLooseRegion;
  cout << " ==> removed " << totalRemoved << " events from a total of " << totalEvents << " (" << totalRemoved/totalEvents*100 << " %)" << endl;
}

void shapeQCD(TH1F* hQCD=0, TH1F* hData=0, TH1F* hW=0, TH1F* hTT=0, 
	      TH1F* hDYMuTau=0, TH1F* hDYJetTau=0, TH1F* hDYTauTau=0, TH1F* hOthers=0,
	      TString variable = "", TCut selIncl = "", TCut categ = "",
	      TTree* backgroundWJets = 0, TTree* backgroundTTbar = 0, TTree* backgroundOthers = 0, 
	      TTree* backgroundDYMutoTau=0, TTree* backgroundDYJtoTau=0, TTree* backgroundDYTauTau=0, TTree* data=0,
	      float scaleFactor = 0., float WJetsCorrectionFactor=0., float TTbarCorrectionFactor=0.,
	      float MutoTauCorrectionFactor=0., float JtoTauCorrectionFactor=0., float DYtoTauTauCorrectionFactor=0.
	      ){

  cout << endl << "-- Study QCD shape" << endl
       << "Selection : " << selIncl << endl
       << "Category : "  << categ << endl;

  float Error = 0.;
  //float totalEvents = hLooseIso->Integral();

  float nData = 0.;
  drawHistogramData(data, variable, nData, Error, 1.0, hData, selIncl ,1);
  hQCD->Add(hData,1.0);
  //
  float nW = 0.;
  drawHistogramMC(backgroundWJets, variable, nW, Error, scaleFactor*WJetsCorrectionFactor, hW, selIncl ,1);
  hQCD->Add(hW,-1.0);
  //
  float nTT = 0.;
  drawHistogramMC(backgroundTTbar, variable, nTT, Error, scaleFactor*TTbarCorrectionFactor, hTT, selIncl,1);
  hQCD->Add(hTT,-1.0);
  //
  float nOthers = 0.;
  drawHistogramMC(backgroundOthers, variable, nOthers, Error, scaleFactor, hOthers, selIncl,1);
  hQCD->Add(hOthers,-1.0);
  //
  float  nDYMuTau=0.;
  drawHistogramMC(backgroundDYMutoTau, variable, nDYMuTau,  Error,  scaleFactor*MutoTauCorrectionFactor,    hDYMuTau  , selIncl,1);
  hQCD->Add(hDYMuTau,-1.0);
  //
  float  nDYJetTau= 0.;
  drawHistogramMC(backgroundDYJtoTau, variable,  nDYJetTau,   Error,  scaleFactor*JtoTauCorrectionFactor,     hDYJetTau  , selIncl,1);
  hQCD->Add(hDYJetTau,-1.0);
  //
  float nDYTauTau = 0.;
  drawHistogramMC(backgroundDYTauTau,  variable, nDYTauTau,    Error, scaleFactor*DYtoTauTauCorrectionFactor, hDYTauTau  , selIncl,1);
  hQCD->Add(hDYTauTau,-1.0);
  
  cout << "number of events : Data=" << nData << " ; W=" << nW << " ; TT=" << nTT << " ; Others=" << nOthers 
       << " ; DYMuTau=" << nDYMuTau  << " ; DYJetTau="   << nDYJetTau << " ; DYTauTau=" << nDYTauTau << endl << endl;

}



void evaluateWusingSSEvents(TH1F* hCleaner = 0, int bin_low = 0, int bin_high = 0,
			    string selection_ = "",
			    float& pseudoExtrapolationFactor = *(new float()),
			    float& ErrorPseudoExtrapolationFactor = *(new float()),
			    TH1F* hWMt=0, TString variable="",
			    TTree* backgroundWJets=0, TTree* backgroundTTbar=0, TTree* backgroundOthers=0, 
			    TTree* backgroundDYTauTau=0, TTree* backgroundDYJtoTau=0, TTree* backgroundDYMutoTau=0, TTree* data=0,
			    float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor=0.,
			    float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
			    float MutoTauCorrectionFactor = 0., float JtoTauCorrectionFactor = 0.,
			    float scaleFactorTTSS = 0.,
			    string scaleFactMu    = "",
			    TCut sbinPZetaRelSS   = ""){

  float Error = 0.;
  hCleaner->Reset();
  if ( !hCleaner->GetSumw2N() )hCleaner->Sumw2();

  cout << "Extrapolation from bin [1," <<  bin_low << "] and [" <<  bin_high << ",inf]" << endl;

  float SSData = 0.;
  drawHistogramData(data, variable, SSData ,Error, 1.0 , hWMt, sbinPZetaRelSS, 1);
  hCleaner->Add(hWMt, +1.0);
  float SSTTbarinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundTTbar,  variable,  SSTTbarinSidebandRegionMC,     Error, scaleFactor*TTxsectionRatio , hWMt, sbinPZetaRelSS,1);  
  SSTTbarinSidebandRegionMC *= scaleFactorTTSS;
  hCleaner->Add(hWMt, -scaleFactorTTSS);
  cout << "Contribution from TTbar in SS is " << SSTTbarinSidebandRegionMC << endl;
  float SSOthersinSidebandRegionMC   = 0.;
  drawHistogramMC(backgroundOthers,    variable, SSOthersinSidebandRegionMC  ,Error,  scaleFactor , hWMt, sbinPZetaRelSS, 1);
  hCleaner->Add(hWMt, -1.0);
  float SSDYtoTauinSidebandRegionMC  = 0.;
  if(useZDataMC)
    drawHistogramMC(backgroundDYTauTau,  variable, SSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor*ExtrapolationFactorZDataMC , hWMt, sbinPZetaRelSS,1);
  else 
    drawHistogramMC(backgroundDYTauTau,  variable, SSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor , hWMt, sbinPZetaRelSS,1);
  hCleaner->Add(hWMt, -1.0);
  float SSDYJtoTauinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundDYJtoTau,  variable, SSDYJtoTauinSidebandRegionMC ,Error, scaleFactor*lumiCorrFactor*JtoTauCorrectionFactor , hWMt, sbinPZetaRelSS,1);
  hCleaner->Add(hWMt, -1.0);
  float SSDYMutoTauinSidebandRegionMC = 0.;
  drawHistogramMC(backgroundDYMutoTau, variable, SSDYMutoTauinSidebandRegionMC ,Error,scaleFactor*lumiCorrFactor*MutoTauCorrectionFactor , hWMt, sbinPZetaRelSS,1);
  hCleaner->Add(hWMt, -1.0);
  
  cout << "Selected events in SS data " << SSData << endl;
  SSData -= SSTTbarinSidebandRegionMC;
  SSData -= SSOthersinSidebandRegionMC;
  SSData -= SSDYtoTauinSidebandRegionMC;
  SSData -= SSDYJtoTauinSidebandRegionMC;
  SSData -= SSDYMutoTauinSidebandRegionMC;
  cout << "- expected from TTbar "          << SSTTbarinSidebandRegionMC << endl;
  cout << "- expected from Others "         << SSOthersinSidebandRegionMC << endl;
  cout << "- expected from DY->tautau "     << SSDYtoTauinSidebandRegionMC << endl;
  cout << "- expected from DY->ll, l->tau " << SSDYMutoTauinSidebandRegionMC << endl;
  cout << "- expected from DY->ll, j->tau " << SSDYJtoTauinSidebandRegionMC  << endl;

  double errorNum = 0.; double errorDen = 0.;
  float num = hCleaner->IntegralAndError(bin_high+1, hCleaner->GetNbinsX(),errorNum);
  float den = hCleaner->IntegralAndError(1, bin_low, errorDen);
  pseudoExtrapolationFactor = num/den;
  ErrorPseudoExtrapolationFactor = pseudoExtrapolationFactor*TMath::Sqrt( errorNum*errorNum/num/num  +  errorDen*errorDen/den/den);
  cout << " ==> the high -> low extrapolation factor using SS events is "
       << num << "/" << den << " = " << pseudoExtrapolationFactor << " +/- " << ErrorPseudoExtrapolationFactor << endl;
  
}







/////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void plotMuTau( Int_t mH_           = 120,
		Int_t useEmbedding_ = 0,
		string selection_   = "inclusive",
		string analysis_    = "",		  
		TString variable_   = "diTauVisMass",
		TString XTitle_     = "full mass",
		TString Unities_    = "GeV",
		TString outputDir   = "test",
		Int_t nBins_ = 80, Float_t xMin_=0, Float_t xMax_=400,
		Float_t magnifySgn_ = 1.0,
		Float_t hltEff_     = 1.0,
		Int_t logy_         = 0,
		Float_t maxY_       = 1.2,
		//TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_p2_topup/src/Bianchi/Limits/bin/results/"
		TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_p2_topup/src/Bianchi/Limits/bin/results/"
		) 
{   

  cout << endl;
  cout << "@@@@@@@@@@@@@@@@@@ Category  = " << selection_     <<  endl;
  cout << "@@@@@@@@@@@@@@@@@@ Variable  = " << string(variable_.Data()) <<  endl;
  cout << endl;

  //string postfix_ = "Raw";
  string postfix_ = "";
  ofstream out(Form(location+"/histograms/%s/yieldsMuTau_mH%d_%s_%s.txt",
		    outputDir.Data(),mH_,selection_.c_str(), analysis_.c_str() ),ios_base::out); 
  out.precision(5);
  int nBins = nBins_;
  TArrayF bins = createBins(nBins_, xMin_, xMax_, nBins, selection_, variable_);

  float Lumi                               = 808.472 + 4398.0 + 495.003 + 5719.0 + 652.755 + 31.099 + 7274.0; //last 3 are residual json from RunB and RunC prompt and Run2012D
  float lumiCorrFactor                     = 1 ;    //= (1-0.056);
  float TTxsectionRatio                    = 0.98; //lumiCorrFactor*(165.8/157.5) ;
  float OStoSSRatioQCD                     = 1.06;
  float SSIsoToSSAIsoRatioQCD              = 1.0;
  float MutoTauCorrectionFactor            = 0.976;
  float JtoTauCorrectionFactor             = 0.976;
  float ExtrapolationFactorZ               = 1.0;
  float ErrorExtrapolationFactorZ          = 1.0;
  float ExtrapolationFactorZDataMC         = 1.0;
  float ExtrapolationFactorSidebandZDataMC = 1.0;
  float ExtrapolationFactorZFromSideband   = 1.0;
  float scaleFactorTTOS                    = 1.0;
  float scaleFactorTTSS                    = 1.0;
  float scaleFactorTTSSIncl                = 1.0;

  cout << endl;
  cout << "Input: " << endl;
  cout << " > Lumi           = " << Lumi/1000. << " fb-1" << endl;
  cout << " > DY xsection SF = " << lumiCorrFactor << endl;
  cout << " > TTbar SF       = " << TTxsectionRatio << endl;
  cout << " > QCD OS/SS SF   = " << OStoSSRatioQCD << endl;
  cout << " > J->tau SF      = " << JtoTauCorrectionFactor << endl;
  cout << " > Mu->tau SF     = " << MutoTauCorrectionFactor << endl;
  cout << endl;

  /////////////////  change SVfit mass here ///////////////////

  string variableStr = "";
  TString variable(variableStr.c_str());
  variable = variable_;

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
  float antiWsgn  = useMt ? 20. :  20. ;
  float antiWsdb  = useMt ? 70. :  40. ; 

  bool use2Dcut   = false;
  if( use2Dcut ){
    antiWcut = "!(MtLeg1MVA<40 && (pZetaMVA-1.5*pZetaVisMVA)>-20)";
    antiWsgn = 0.5;
    antiWsdb = 0.5;
  }

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TPad* pad1 = new TPad("pad1DEta","",0.05,0.22,0.96,0.97);
  TPad* pad2 = new TPad("pad2DEta","",0.05,0.02,0.96,0.20);
 
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(logy_);
  gStyle->SetOptStat(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleH(0.07);
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleOffset(1.3,"y");

  TLegend* leg = new TLegend(0.63,0.48,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);
  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=8 TeV}{%.1f fb^{-1} #tau_{#mu}#tau_{had}}", Lumi/1000. ));

  THStack* aStack = new THStack("aStack","");

  TH1F* hSiml     = new TH1F( "hSiml"   ,"all"               , nBins , bins.GetArray());
  TH1F* hSgn      = new TH1F( "hSgn "   ,"vbf+ggf"           , nBins , bins.GetArray());         hSgn->SetFillColor(0); hSgn->SetLineColor(kBlue);hSgn->SetLineWidth(2);hSgn->SetLineStyle(kDashed);
  TH1F* hSgn1     = new TH1F( "hSgn1"   ,"vbf"               , nBins , bins.GetArray());         hSgn1->SetLineWidth(2);
  TH1F* hSgn2     = new TH1F( "hSgn2"   ,"ggf"               , nBins , bins.GetArray());         hSgn2->SetLineWidth(2);
  TH1F* hSgn3     = new TH1F( "hSgn3"   ,"vh"                , nBins , bins.GetArray());         hSgn3->SetLineWidth(2);
  TH1F* hData     = new TH1F( "hData"   ,"        "          , nBins , bins.GetArray());         hData->SetMarkerStyle(20);hData->SetMarkerSize(1.2);hData->SetMarkerColor(kBlack);hData->SetLineColor(kBlack);hData->SetXTitle(XTitle_+" ("+Unities_+")");hData->SetYTitle(Form(" Events/(%.1f %s)", hData->GetBinWidth(1), Unities_.Data() ) );hData->SetTitleSize(0.04,"X");hData->SetTitleSize(0.05,"Y");hData->SetTitleOffset(0.95,"Y");
  TH1F* hDataEmb  = new TH1F( "hDataEmb","Embedded"          , nBins , bins.GetArray());         hDataEmb->SetFillColor(kOrange-4);
  TH1F* hW        = new TH1F( "hW"      ,"W+jets"            , nBins , bins.GetArray());         hW->SetFillColor(kRed+2);
  TH1F* hWSS      = new TH1F( "hWSS"    ,"W+jets SS"         , nBins , bins.GetArray());
  TH1F* hWMinusSS = new TH1F( "hWMinusSS","W+jets - SS"      , nBins , bins.GetArray());         hWMinusSS->SetFillColor(kRed+2);
  TH1F* hW3Jets   = new TH1F( "hW3Jets" ,"W+3jets"           , nBins , bins.GetArray());         hW3Jets->SetFillColor(kRed+2);
  TH1F* hEWK      = new TH1F( "hEWK"    ,"EWK"               , nBins , bins.GetArray());         hEWK->SetFillColor(kRed+2);
  TH1F* hZtt      = new TH1F( "hZtt"    ,"Ztautau"           , nBins , bins.GetArray());         hZtt->SetFillColor(kOrange-4);
  TH1F* hZmm      = new TH1F( "hZmm"    ,"Z+jets, mu->tau"   , nBins , bins.GetArray());         hZmm->SetFillColor(kBlue-2);
  TH1F* hZmmLoose = new TH1F( "hZmmLoose","Z+jets, mu->tau"  , nBins , bins.GetArray());         hZmmLoose->SetFillColor(kBlue-2);
  TH1F* hZmj      = new TH1F( "hZmj"    ,"Z+jets, jet to tau", nBins , bins.GetArray());         hZmj->SetFillColor(kBlue-2);
  TH1F* hZmjLoose = new TH1F( "hZmjLoose","Z+jets, jet->tau" , nBins , bins.GetArray());         hZmjLoose->SetFillColor(kBlue-2);
  TH1F* hZfakes   = new TH1F( "hZfakes" ,"Z+jets, jet to tau", nBins , bins.GetArray());         hZfakes->SetFillColor(kBlue-2);
  TH1F* hTTb      = new TH1F( "hTTb"    ,"ttbar"             , nBins , bins.GetArray());         hTTb->SetFillColor(kBlue-8); 
  TH1F* hQCD      = new TH1F( "hQCD"    ,"QCD"               , nBins , bins.GetArray());         hQCD->SetFillColor(kMagenta-10);
  TH1F* hSS       = new TH1F( "hSS"     ,"same-sign"         , nBins , bins.GetArray());         hSS->SetFillColor(kMagenta-10);
  TH1F* hSSLooseVBF= new TH1F("hSSLooseVBF" ,"same-sign"     , nBins , bins.GetArray());         hSSLooseVBF->SetFillColor(kMagenta-10);
  TH1F* hLooseIso1= new TH1F( "hLooseIso1","Loose Iso"       , nBins , bins.GetArray());
  TH1F* hLooseIso2= new TH1F( "hLooseIso2","Loose Iso"       , nBins , bins.GetArray());
  TH1F* hLooseIso3= new TH1F( "hLooseIso3","Loose Iso"       , nBins , bins.GetArray());
  TH1F* hAntiIso  = new TH1F( "hAntiIso","Anti Iso"          , nBins , bins.GetArray());
  TH1F* hAntiIsoFR= new TH1F( "hAntiIsoFR","Anti Iso * FR"   , nBins , bins.GetArray());
  TH1F* hVV       = new TH1F( "hVV"     ,"Diboson"           , nBins , bins.GetArray());         hVV->SetFillColor(kRed+2);
  TH1F* hW3JetsMediumTauIso               = new TH1F( "hW3JetsMediumTauIso" ,  "W+3jets (medium tau-iso)"                  , nBins , bins.GetArray());
  TH1F* hW3JetsLooseTauIso                = new TH1F( "hW3JetsLooseTauIso" ,  "W+3jets (loose tau-iso)"                    , nBins , bins.GetArray());
  TH1F* hW3JetsMediumTauIsoRelVBF         = new TH1F( "hW3JetsMediumTauIsoRelVBF" ,  "W+3jets (medium tau-iso)"            , nBins , bins.GetArray());
  TH1F* hW3JetsMediumTauIsoRelVBFMinusSS  = new TH1F( "hW3JetsMediumTauIsoRelVBFMinusSS" ,  "W+3jets (medium tau-iso)-SS"  , nBins , bins.GetArray());
  TH1F* hWLooseBTag                       = new TH1F( "hWLooseBTag" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());
  TH1F* hDataAntiIsoLooseTauIso           = new TH1F( "hDataAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso"          , nBins , bins.GetArray()); hDataAntiIsoLooseTauIso->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoQCD        = new TH1F( "hDataAntiIsoLooseTauIsoQCD"   ,"data anti-iso, norm QCD"            , nBins , bins.GetArray()); hDataAntiIsoLooseTauIsoQCD->SetFillColor(kMagenta-10);
  TH1F* hggH110    = new TH1F( "hggH110"   ,"ggH110"               , nBins , bins.GetArray()); hggH110->SetLineWidth(2);
  TH1F* hggH115    = new TH1F( "hggH115"   ,"ggH115"               , nBins , bins.GetArray()); hggH115->SetLineWidth(2);
  TH1F* hggH120    = new TH1F( "hggH120"   ,"ggH120"               , nBins , bins.GetArray()); hggH120->SetLineWidth(2);
  TH1F* hggH125    = new TH1F( "hggH125"   ,"ggH125"               , nBins , bins.GetArray()); hggH125->SetLineWidth(2);
  TH1F* hggH130    = new TH1F( "hggH130"   ,"ggH130"               , nBins , bins.GetArray()); hggH130->SetLineWidth(2);
  TH1F* hggH135    = new TH1F( "hggH135"   ,"ggH135"               , nBins , bins.GetArray()); hggH135->SetLineWidth(2);
  TH1F* hggH140    = new TH1F( "hggH140"   ,"ggH140"               , nBins , bins.GetArray()); hggH140->SetLineWidth(2);
  TH1F* hggH145    = new TH1F( "hggH145"   ,"ggH145"               , nBins , bins.GetArray()); hggH145->SetLineWidth(2);
  TH1F* hqqH110    = new TH1F( "hqqH110"   ,"qqH110"               , nBins , bins.GetArray()); hqqH110->SetLineWidth(2);
  TH1F* hqqH115    = new TH1F( "hqqH115"   ,"qqH115"               , nBins , bins.GetArray()); hqqH115->SetLineWidth(2);
  TH1F* hqqH120    = new TH1F( "hqqH120"   ,"qqH120"               , nBins , bins.GetArray()); hqqH120->SetLineWidth(2);
  TH1F* hqqH125    = new TH1F( "hqqH125"   ,"qqH125"               , nBins , bins.GetArray()); hqqH125->SetLineWidth(2);
  TH1F* hqqH130    = new TH1F( "hqqH130"   ,"qqH130"               , nBins , bins.GetArray()); hqqH130->SetLineWidth(2);
  TH1F* hqqH135    = new TH1F( "hqqH135"   ,"qqH135"               , nBins , bins.GetArray()); hqqH135->SetLineWidth(2); 
  TH1F* hqqH140    = new TH1F( "hqqH140"   ,"qqH140"               , nBins , bins.GetArray()); hqqH140->SetLineWidth(2);
  TH1F* hqqH145    = new TH1F( "hqqH145"   ,"qqH145"               , nBins , bins.GetArray()); hqqH145->SetLineWidth(2);
  TH1F* hVH110     = new TH1F( "hVH110"   ,"VH110"                 , nBins , bins.GetArray()); hVH110->SetLineWidth(2);
  TH1F* hVH115     = new TH1F( "hVH115"   ,"VH115"                 , nBins , bins.GetArray()); hVH115->SetLineWidth(2);
  TH1F* hVH120     = new TH1F( "hVH120"   ,"VH120"                 , nBins , bins.GetArray()); hVH120->SetLineWidth(2);
  TH1F* hVH125     = new TH1F( "hVH125"   ,"VH125"                 , nBins , bins.GetArray()); hVH125->SetLineWidth(2);
  TH1F* hVH130     = new TH1F( "hVH130"   ,"VH130"                 , nBins , bins.GetArray()); hVH130->SetLineWidth(2);
  TH1F* hVH135     = new TH1F( "hVH135"   ,"VH135"                 , nBins , bins.GetArray()); hVH135->SetLineWidth(2);
  TH1F* hVH140     = new TH1F( "hVH140"   ,"VH140"                 , nBins , bins.GetArray()); hVH140->SetLineWidth(2);
  TH1F* hVH145     = new TH1F( "hVH145"   ,"VH145"                 , nBins , bins.GetArray()); hVH145->SetLineWidth(2);

  // QCD shapes //
  const int nSign=2;
  const int nSamples=8;
  TH1F* h_QCDshape[nSamples][nSign];
  TString name_QCDsign[nSign]={"OS","SS"};
  TString name_QCDshape[nSamples]={"hQCD","hData","hW","hTT","hDYMuTau","hDYJetTau","hDYTauTau","hOthers"};
  int color_QCDshape[nSamples]={kRed+2,kMagenta-10,kBlue-8,kGreen+2,kViolet,kOrange-4,kBlue};
  TString nom="";

  for(int iS=0 ; iS<nSign ; iS++) {
    for(int iSam=0 ; iSam<nSamples ; iSam++) {
      nom = name_QCDshape[iSam]+"_"+name_QCDsign[iS]+"_qcdSh";
      h_QCDshape[iSam][iS] = new TH1F(nom,nom, nBins , bins.GetArray());         
      h_QCDshape[iSam][iS]->SetFillColor(color_QCDshape[iSam]);
    }
  }
  vector<string> SUSYhistos;
  //SUSYhistos.push_back("SUSYGG90"); SUSYhistos.push_back("SUSYGG100"); SUSYhistos.push_back("SUSYGG120"); SUSYhistos.push_back("SUSYGG130");
  //SUSYhistos.push_back("SUSYGG140");SUSYhistos.push_back("SUSYGG160"); SUSYhistos.push_back("SUSYGG180"); SUSYhistos.push_back("SUSYGG200");
  //SUSYhistos.push_back("SUSYGG250");SUSYhistos.push_back("SUSYGG300"); SUSYhistos.push_back("SUSYGG350"); SUSYhistos.push_back("SUSYGG400");
  //SUSYhistos.push_back("SUSYGG450");SUSYhistos.push_back("SUSYGG500"); SUSYhistos.push_back("SUSYGG600"); SUSYhistos.push_back("SUSYGG700");
  //SUSYhistos.push_back("SUSYGG800");SUSYhistos.push_back("SUSYGG900"); SUSYhistos.push_back("SUSYBB90");  SUSYhistos.push_back("SUSYBB100");
  //SUSYhistos.push_back("SUSYBB120");SUSYhistos.push_back("SUSYBB130"); SUSYhistos.push_back("SUSYBB140"); SUSYhistos.push_back("SUSYBB160");
  //SUSYhistos.push_back("SUSYBB180");SUSYhistos.push_back("SUSYBB200"); SUSYhistos.push_back("SUSYBB250"); SUSYhistos.push_back("SUSYBB300");
  //SUSYhistos.push_back("SUSYBB350");SUSYhistos.push_back("SUSYBB400"); SUSYhistos.push_back("SUSYBB450"); SUSYhistos.push_back("SUSYBB500");
  //SUSYhistos.push_back("SUSYBB600");SUSYhistos.push_back("SUSYBB700"); SUSYhistos.push_back("SUSYBB800"); SUSYhistos.push_back("SUSYBB900");
  std::map<string,TH1F*> mapSUSYhistos;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    mapSUSYhistos.insert( make_pair(SUSYhistos[i], 
				    new TH1F(Form("h%s",SUSYhistos[i].c_str()) ,
					     Form("%s", SUSYhistos[i].c_str()), 
					     nBins , bins.GetArray()) ) 
			  );
  }

  TH1F* hParameters   = new TH1F( "hParameters", "" ,30, 0, 30);
 ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

  // get the FR-file
  string scaleFactMu         = "";
  string scaleFactMuUp       = "";
  string scaleFactMuDown     = "";
  string scaleFactTauQCD     = "";
  string scaleFactTauQCDUp   = "";
  string scaleFactTauQCDDown = "";
  string scaleFactTauW       = "";
  string scaleFactTauWUp     = "";
  string scaleFactTauWDown   = "";

  createStringsIsoFakeRate("FakeRate.root", scaleFactMu,     scaleFactMuUp,     scaleFactMuDown,     "ptL1", "FakeRate",    "_",   "MuTau_Mu_ptL1_incl");
  createStringsIsoFakeRate("FakeRate.root", scaleFactTauQCD, scaleFactTauQCDUp, scaleFactTauQCDDown, "ptL2", "FakeRateQCD", "QCD_","MuTau_Tau_ptL2_QCDSS02_WSS60_incl");
  createStringsIsoFakeRate("FakeRate.root", scaleFactTauW,   scaleFactTauWUp,   scaleFactTauWDown,   "ptL2", "FakeRateW",   "W_",  "MuTau_Tau_ptL2_QCDOS02_WOS60_incl");


  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

  //TString pathToFile = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesMoriondV3/";
  //TString pathToFile = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesV2";  
  //TString pathToFile = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesHCPV2TestPU/";  
  TString pathToFile = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesMoriondNewRecoilV2/";  
  TString pathToFileData = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesMoriondNewRecoilV2_Run2012D/";
  TString pathToFileNewtauES = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesNewRecoilNewTauES/";
  TString pathToFileNewtauESBugFix = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesNewRecoilNewTauES/RecoilBugFix/";
  TString pathToFileWJ = "/data_CMS/cms/anayak/H2TauTauHCP/Moriond/MuTauStream/NtuplesMoriondNewRecoilV2/WJetNewSF/";

  // Open the files
  TFile *fData            = new TFile(pathToFileData+"/nTuple_Data_2012ABCD_MuTau.root", "READ");
  //TFile *fData            = new TFile(pathToFile+"/nTuple_Data_2012ABC_MuTau.root", "READ");
  TFile *fDataEmbedded    = new TFile(pathToFileNewtauES+"/nTuple_Embedded_2012ABCD_MuTau.root", "READ");
  //TFile *fDataEmbedded    = new TFile(pathToFileData+"/nTuple_Embedded_2012ABCD_MuTau.root", "READ");
  //TFile *fDataEmbedded    = new TFile(pathToFile+"/nTuple_Embedded_2012ABC_MuTau.root", "READ");
  //TFile *fBackgroundDY    = new TFile(pathToFile+"/nTupleDYJets-MuTau-50-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  TFile *fBackgroundDY    = new TFile(pathToFileNewtauESBugFix+"/nTupleDYJets-MuTau-50-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  //TFile *fBackgroundWJets = new TFile(pathToFileWJ+"/nTupleWJets-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  TFile *fBackgroundWJets = new TFile(pathToFileWJ+"/nTupleWJetsAllBins-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  //TFile *fBackgroundW3Jets= new TFile(pathToFileWJ+"/nTupleW3Jets-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  TFile *fBackgroundW3Jets= new TFile(pathToFileWJ+"/nTupleWJetsAllBins-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundTTbar = new TFile(pathToFile+"/nTupleTTJets-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  TFile *fBackgroundOthers= new TFile(pathToFile+"/nTupleOthers-MuTau-madgraph-PUS10_run_Open_MuTauStream.root","READ");
  
  vector<int> hMasses;
  hMasses.push_back(110);hMasses.push_back(115);hMasses.push_back(120);hMasses.push_back(125);
  hMasses.push_back(130);hMasses.push_back(135);hMasses.push_back(140);hMasses.push_back(145);

  const int nProd=3;
  const int nMasses=11;
  TString nameProd[nProd]={"GluGluToHToTauTau","VBF_HToTauTau","WH_ZH_TTH_HToTauTau"};
  TString nameMasses[nMasses]={"110","115","120","125","130","135","140","145","150","155","160"};

  TFile *fSignal[nProd][nMasses];

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      fSignal[iP][iM] = new TFile(pathToFileNewtauES+"/nTuple"+nameProd[iP]+"_M-"+nameMasses[iM]+"_Open_MuTauStream.root","READ");
       
  std::map<string,TFile*> mapSUSYfiles;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    mapSUSYfiles.insert( make_pair(SUSYhistos[i], new TFile(Form("%s/nTuple%s-MuTau-powheg-PUS6_run_Open_MuTauStream.root",pathToFile.Data(),SUSYhistos[i].c_str()) ,"READ")  )  );
  }
  

  TString tree         = "outTreePtOrd"+postfix_+analysis_;
  TString treeEmbedded = "outTreePtOrd"+postfix_;
  if(analysis_.find("TauUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("TauDown")!=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("MuUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("MuDown")!=string::npos) 
    treeEmbedded = tree;

  TTree *data                = (TTree*)fData->Get(("outTreePtOrd"+postfix_).c_str());
  TTree *dataEmbedded        = EMBEDDEDSAMPLES ? (TTree*)fDataEmbedded->Get(treeEmbedded) : 0;

  // Split DY into 3 sub-samples (TauTau, MuToTau, JetToTau)
  TFile *dummy1, *fBackgroundDYTauTau, *fBackgroundDYMuToTau, *fBackgroundDYJetToTau;
  TTree *backgroundDYTauTau, *backgroundDYMutoTau, *backgroundDYJtoTau, *backgroundDY;

  if(DOSPLIT) {
    cout << "SPLIT DY SAMPLE ON THE FLY" << endl;
    //
    dummy1 = new TFile("dummy2.root","RECREATE");
    backgroundDY = (TTree*)fBackgroundDY->Get(tree);
    //
    cout << "Now copying g/Z -> tau+ tau- " << endl;
    backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
    //
    cout << "Now copying g/Z -> mu+mu- mu->tau" << endl;
    backgroundDYMutoTau = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
    //
    cout << "Now copying g/Z -> mu+mu- jet->tau" << endl;
    backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau
  }
  else {
    cout << "USE DY SEPARATE SUB-SAMPLES" << endl;
    //
    fBackgroundDYTauTau   = new TFile(pathToFileNewtauESBugFix+"/nTuple_DYJ_TauTau_Open_MuTauStream.root"  ,"READ");
    fBackgroundDYMuToTau  = new TFile(pathToFileNewtauESBugFix+"/nTuple_DYJ_MuToTau_Open_MuTauStream.root" ,"READ");
    fBackgroundDYJetToTau = new TFile(pathToFileNewtauESBugFix+"/nTuple_DYJ_JetToTau_Open_MuTauStream.root","READ");
    //
    backgroundDYTauTau    = (TTree*)(fBackgroundDYTauTau  ->Get(tree));
    backgroundDYMutoTau   = (TTree*)(fBackgroundDYMuToTau ->Get(tree));
    backgroundDYJtoTau    = (TTree*)(fBackgroundDYJetToTau->Get(tree));
  }

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
  cout << backgroundDYMutoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau" << endl;

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundW3Jets    = W3JETS ? (TTree*)fBackgroundW3Jets->Get(tree) : 0;
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 
  TTree *signal[nProd][nMasses];

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      signal[iP][iM] = (TTree*) fSignal[iP][iM]->Get(tree);

  std::map<string,TTree*> mapSUSYtrees;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    TTree* treeSusy = (mapSUSYfiles.find(SUSYhistos[i]))->second ? (TTree*)((mapSUSYfiles.find(SUSYhistos[i]))->second)->Get(tree) : 0;
    mapSUSYtrees.insert( make_pair( SUSYhistos[i], treeSusy )) ;
  }




  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  ///// LEPT PT ///////
  TCut lpt("ptL1>20 && isPFMuon && isTightMuon");
  TCut tpt("ptL2>20");
  /*
  if(selection_.find("High")!=string::npos)
    tpt = tpt&&TCut("ptL2>40");
  else if(selection_.find("Low")!=string::npos)
    tpt = tpt&&TCut("ptL2<40");
  */
  ////// TAU ISO //////
  TCut tiso("tightestHPSMVAWP>=0"); 
  TCut ltiso("tightestHPSMVAWP>-99");
  TCut mtiso("hpsMVA>0.7");

  ////// MU ISO ///////
  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

 
  ////// EVENT WISE //////
  TCut lveto("muFlag!=1 && vetoEvent==0"); //muFlag==0
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  //TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));
  TCut apZ2(Form("((%s)>60 && (%s)<120)",antiWcut.c_str(),antiWcut.c_str()));
  TCut hltevent("pairIndex<1 && HLTx==1 && ( run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");


  ////// CATEGORIES ///
  TCut zeroJet("nJets30<1");
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut vbf("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
  TCut vbfLoose("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfLooseQCD("nJets20>=2 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");
  TCut boost("nJets30>0 && pt1>30 && nJets20BTagged<1");
  boost = boost && !vbf /*&& !vh*/;
  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut bTagLoose("nJets30<2 && nJets20BTaggedLoose>0"); //for W shape in b-Category
  //TCut bTagLoose("nJets30<2 && nJets20>=1");
  TCut nobTag("nJets30<2 && nJets20BTagged==0");
  TCut novbf("nJets30<1 && nJets20BTagged==0");

  if(selection_.find("High")!=string::npos) {
    boost = boost&&TCut("ptL2>40"); 
    novbf = novbf&&TCut("ptL2>40");
  }
  else if(selection_.find("Low")!=string::npos){
    boost = boost&&TCut("ptL2<40"); 
    novbf = novbf&&TCut("ptL2<40"); 
  }

  bool removeMtCut     = bool(selection_.find("NoMt")!=string::npos);
  bool invertDiTauSign = bool(selection_.find("SS")!=string::npos);

  TCut MtCut       = removeMtCut     ? "(etaL1<999)" : pZ;
  TCut diTauCharge = invertDiTauSign ? SS : OS; 

  TCut sbin; TCut sbinEmbedding; TCut sbinEmbeddingPZetaRel; TCut sbinPZetaRel; TCut sbinSS; 
  TCut sbinPZetaRelSS; TCut sbinSSaIso; TCut sbinAiso;
  TCut sbinSSlIso1; TCut sbinSSlIso2; TCut sbinSSlIso3;
  TCut sbinPZetaRelaIso; TCut sbinPZetaRelSSaIso;  TCut sbinPZetaRelSSaIsoMtiso; 
  TCut sbinSSaIsoLtiso; TCut sbinSSaIsoMtiso;
  TCut sbinSSltiso; TCut sbinSSmtiso; TCut sbinLtiso; TCut sbinMtiso; TCut sbinPZetaRelMtiso;

  TCut sbinInclusive;
  sbinInclusive                     = lpt && tpt && tiso && liso && lveto && diTauCharge && MtCut  && hltevent && hltmatch;
  TCut sbinEmbeddingInclusive;
  sbinEmbeddingInclusive            = lpt && tpt && tiso && liso && lveto && diTauCharge && MtCut                         ;
  TCut sbinPZetaRelEmbeddingInclusive;
  sbinPZetaRelEmbeddingInclusive    = lpt && tpt && tiso && liso && lveto && diTauCharge                                  ;
  TCut sbinPZetaRelSSInclusive;
  sbinPZetaRelSSInclusive           = lpt && tpt && tiso && liso && lveto && SS                    && hltevent && hltmatch;
  TCut sbinPZetaRelInclusive;
  sbinPZetaRelInclusive             = lpt && tpt && tiso && liso && lveto && diTauCharge           && hltevent && hltmatch;
  TCut sbinSSInclusive;
  sbinSSInclusive                   = lpt && tpt && tiso && liso && lveto && SS          && MtCut  && hltevent && hltmatch;
  TCut sbinSSaIsoInclusive;
  sbinSSaIsoInclusive               = lpt && tpt && tiso && laiso&& lveto && SS          && MtCut  && hltevent && hltmatch;
  TCut sbinAisoInclusive;
  sbinAisoInclusive                 = lpt && tpt && tiso && laiso&& lveto && diTauCharge && MtCut  && hltevent && hltmatch;
  TCut sbinPZetaRelSSaIsoInclusive;
  sbinPZetaRelSSaIsoInclusive       = lpt && tpt && tiso && laiso&& lveto && SS                    && hltevent && hltmatch;
  TCut sbinPZetaRelSSaIsoMtisoInclusive;
  sbinPZetaRelSSaIsoMtisoInclusive  = lpt && tpt && mtiso&& laiso&& lveto && SS                    && hltevent && hltmatch;

  TCut sbinSSaIsoLtisoInclusive;
  sbinSSaIsoLtisoInclusive          = lpt && tpt && mtiso&& laiso&& lveto && SS && MtCut           && hltevent && hltmatch;
  TCut sbinSSaIsoMtisoInclusive;
  sbinSSaIsoMtisoInclusive          = lpt && tpt && mtiso&& laiso&& lveto && SS && MtCut           && hltevent && hltmatch;
  TCut sbinPZetaRelaIsoInclusive;
  sbinPZetaRelaIsoInclusive         = lpt && tpt && tiso && laiso&& lveto && diTauCharge           && hltevent && hltmatch;

  TCut sbinSSltisoInclusive;
  sbinSSltisoInclusive              = lpt && tpt && ltiso&& liso && lveto && SS && MtCut           && hltevent && hltmatch;
  TCut sbinLtisoInclusive;
  sbinLtisoInclusive                = lpt && tpt && ltiso&& liso && lveto && diTauCharge && MtCut  && hltevent && hltmatch;
  TCut sbinMtisoInclusive;
  sbinMtisoInclusive                = lpt && tpt && mtiso&& liso && lveto && diTauCharge && MtCut  && hltevent && hltmatch;
  TCut sbinPZetaRelLtisoInclusive;
  sbinPZetaRelLtisoInclusive        = lpt && tpt && ltiso&& liso && lveto && diTauCharge           && hltevent && hltmatch;


  TCut sbinTmp("");
  if(selection_.find("inclusive")!=string::npos) 
    sbinTmp = "etaL1<999";
  else if(selection_.find("oneJet")!=string::npos)
    sbinTmp = oneJet;
  else if(selection_.find("twoJets")!=string::npos)
    sbinTmp = twoJets;
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
    sbinTmp = vbf;
  else if(selection_.find("vh")!=string::npos)
    sbinTmp = vh;
  else if(selection_.find("novbf")!=string::npos)
    sbinTmp = novbf;
  else if(selection_.find("boost")!=string::npos)
    sbinTmp = boost;
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)
    sbinTmp = bTag;
  else if(selection_.find("nobTag")!=string::npos)
    sbinTmp = nobTag;


  sbin                   =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge  && MtCut  && hltevent && hltmatch ;
  sbinEmbedding          =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge  && MtCut                          ;
  sbinEmbeddingPZetaRel  =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge                                    ;
  sbinPZetaRel           =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge            && hltevent && hltmatch ;
  sbinPZetaRelaIso       =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && diTauCharge            && hltevent && hltmatch ;
  sbinPZetaRelSSaIso     =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && SS                     && hltevent && hltmatch ;
  sbinSS                 =  sbinTmp && lpt && tpt && tiso && liso && lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinPZetaRelSS         =  sbinTmp && lpt && tpt && tiso && liso && lveto && SS                     && hltevent && hltmatch ;
  sbinAiso               =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && diTauCharge  && MtCut  && hltevent && hltmatch ;
  sbinSSaIso             =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSlIso1            =  sbinTmp && lpt && tpt && tiso && lliso&& lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSlIso2            =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSlIso3            =  sbinTmp && lpt && tpt && mtiso&& lliso&& lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSaIsoLtiso        =  sbinTmp && lpt && tpt && ltiso&& laiso&& lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSaIsoMtiso        =  sbinTmp && lpt && tpt && mtiso&& laiso&& lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSltiso            =  sbinTmp && lpt && tpt && ltiso&& liso && lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinSSmtiso            =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && SS           && MtCut  && hltevent && hltmatch ;
  sbinLtiso              =  sbinTmp && lpt && tpt && ltiso&& liso && lveto && diTauCharge  && MtCut  && hltevent && hltmatch ;
  sbinMtiso              =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && diTauCharge  && MtCut  && hltevent && hltmatch ;
  sbinPZetaRelMtiso      =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && diTauCharge            && hltevent && hltmatch ;
  sbinPZetaRelSSaIsoMtiso=  sbinTmp && lpt && tpt && mtiso&& laiso&& lveto && SS                     && hltevent && hltmatch ;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << endl;
  cout << "#############################################################" << endl;
  cout << ">>>>>>>>>>> BEGIN Compute inclusive informations <<<<<<<<<<<<" << endl;
  cout << "#############################################################" << endl;
  cout << endl;

  cout << "******** Extrapolation factors for Z->tautau normalization ********" << endl;
  // inclusive DY->tautau:
  TH1F* hExtrap = new TH1F("hExtrap","",nBins , bins.GetArray());
  float Error = 0.;

  float ExtrapDYInclusive = 0.;
  drawHistogramMC(backgroundDYTauTau, variable, ExtrapDYInclusive,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->tautau             = " << ExtrapDYInclusive << " +/- " <<  Error << endl; 

  float ExtrapDYInclusivePZetaRel = 0.;
  drawHistogramMC(backgroundDYTauTau, variable, ExtrapDYInclusivePZetaRel,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRelInclusive);
  cout << "All Z->tautau (pZeta Rel) = " << ExtrapDYInclusivePZetaRel << " +/- " <<  Error << endl; 

  float ExtrapLFakeInclusive = 0.;
  drawHistogramMC(backgroundDYMutoTau, variable,ExtrapLFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->mumu, mu->tau      = " << ExtrapLFakeInclusive << " +/- " <<  Error << endl;

  float ExtrapJFakeInclusive = 0.;
  drawHistogramMC(backgroundDYJtoTau, variable, ExtrapJFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->mumu, j->tau       = " << ExtrapJFakeInclusive << " +/- " <<  Error << endl;
  cout << endl;

  float ExtrapDYNum = 0.;
  drawHistogramMC(backgroundDYTauTau, variable, ExtrapDYNum,                  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbin);
  float ExtrapDYNuminSidebandRegion = 0.;
  drawHistogramMC(backgroundDYTauTau, variable, ExtrapDYNuminSidebandRegion,  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel&&apZ);
  float ExtrapDYNumPZetaRel = 0.;
  drawHistogramMC(backgroundDYTauTau, variable, ExtrapDYNumPZetaRel,          Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel);


  float ExtrapolationFactorMadGraph      = ExtrapDYNum/ExtrapDYInclusive;
  float ErrorExtrapolationFactorMadGraph = TMath::Sqrt(ExtrapolationFactorMadGraph*(1-ExtrapolationFactorMadGraph)/ExtrapDYInclusive);
  cout << "Extrap. factor using MadGraph            = " << ExtrapolationFactorMadGraph << " +/- " << ErrorExtrapolationFactorMadGraph << endl;

  float ExtrapEmbedNum = 0.;
  drawHistogramEmbed(dataEmbedded, variable, ExtrapEmbedNum,                 Error, 1.0, hExtrap, sbinEmbedding);
  float ExtrapEmbedNuminSidebandRegion = 0.;
  drawHistogramEmbed(dataEmbedded, variable, ExtrapEmbedNuminSidebandRegion, Error, 1.0, hExtrap, sbinEmbeddingPZetaRel&&apZ);
  float ExtrapEmbedNumPZetaRel = 0.;
  drawHistogramEmbed(dataEmbedded, variable, ExtrapEmbedNumPZetaRel,         Error, 1.0, hExtrap, sbinEmbeddingPZetaRel);
  float ExtrapEmbedDen = 0.;
  drawHistogramEmbed(dataEmbedded, variable, ExtrapEmbedDen,                 Error, 1.0, hExtrap, sbinEmbeddingInclusive);
  float ExtrapEmbedDenPZetaRel = 0.;
  drawHistogramEmbed(dataEmbedded, variable, ExtrapEmbedDenPZetaRel,         Error, 1.0, hExtrap, sbinPZetaRelEmbeddingInclusive);

  ExtrapolationFactorZ             = ExtrapEmbedNum/ExtrapEmbedDen; 
  ErrorExtrapolationFactorZ        = TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen);
  ExtrapolationFactorZDataMC       = ExtrapolationFactorZ/ExtrapolationFactorMadGraph;
  ExtrapolationFactorZFromSideband = ExtrapEmbedDen/ExtrapEmbedDenPZetaRel;

  float sidebandRatioMadgraph = ExtrapDYNuminSidebandRegion/ExtrapDYNumPZetaRel;
  float sidebandRatioEmbedded = ExtrapEmbedNuminSidebandRegion/ExtrapEmbedNumPZetaRel;
  ExtrapolationFactorSidebandZDataMC = (sidebandRatioMadgraph > 0) ? sidebandRatioEmbedded/sidebandRatioMadgraph : 1.0;

  cout << "Extrap. factor using embedded sample     = " << ExtrapolationFactorZ << " +/- " << ErrorExtrapolationFactorZ << endl;
  cout << " ==> data/MC (signal region)             = " << ExtrapolationFactorZDataMC << " +/- " 
       << ExtrapolationFactorZDataMC*(ErrorExtrapolationFactorMadGraph/ExtrapolationFactorMadGraph + 
				      ErrorExtrapolationFactorZ/ExtrapolationFactorZ) 
       << endl;
  cout << " ==> data/MC (sideband-to-signal)        = " << ExtrapolationFactorSidebandZDataMC << endl;
  cout << " ==> data (sideband-to-signal inclusive) = " << ExtrapolationFactorZFromSideband << endl;
  
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << endl;
  cout << "******** Extrapolation factors for QCD normalization ********" << endl;

  float SSQCDinSignalRegionDATAIncl = 0.; 
  float extrapFactorWSSIncl = 0.;
  float SSWinSignalRegionDATAIncl = 0.;
  float SSWinSignalRegionMCIncl = 0.;
  float SSWinSidebandRegionDATAIncl = 0.;
  float SSWinSidebandRegionMCIncl = 0.;      
  if(invertDiTauSign) OStoSSRatioQCD = 1.0;

  evaluateQCD(0, 0, true, "SS", false, removeMtCut, "inclusive", 
	      SSQCDinSignalRegionDATAIncl , SSIsoToSSAIsoRatioQCD, scaleFactorTTSSIncl,
	      extrapFactorWSSIncl, 
	      SSWinSignalRegionDATAIncl, SSWinSignalRegionMCIncl,
	      SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
 	      hExtrap, variable,
 	      backgroundWJets, backgroundTTbar, backgroundOthers, 
 	      backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
 	      Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
	      ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
 	      MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
	      OStoSSRatioQCD,
 	      antiWsdb, antiWsgn, useMt,
 	      scaleFactMu,
	      sbinSSInclusive,
	      sbinPZetaRelSSInclusive,
 	      sbinPZetaRelSSInclusive, pZ, apZ, sbinPZetaRelSSInclusive, 
 	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoMtisoInclusive, 
	      vbf, oneJet, zeroJet);

  delete hExtrap;

  cout << endl;
  cout << "#############################################################" << endl;
  cout << ">>>>>>>>>>> END Compute inclusive informations <<<<<<<<<<<<<<" << endl;
  cout << "#############################################################" << endl;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
 
  std::vector<string> samples;
  samples.push_back("SS");
  samples.push_back("WJets");
  samples.push_back("Data");
  if(backgroundW3Jets)
    samples.push_back("W3Jets");
  samples.push_back("TTbar");
  samples.push_back("Others");
  samples.push_back("DYMutoTau");
  samples.push_back("DYJtoTau");
  samples.push_back("DYToTauTau");
  if(dataEmbedded)
    samples.push_back("Embedded");
  for(unsigned int i = 0 ; i < hMasses.size() ; i++) {
    samples.push_back(string(Form("ggH%d",hMasses[i])));
    samples.push_back(string(Form("qqH%d",hMasses[i])));
    samples.push_back(string(Form("VH%d",hMasses[i])));
  }
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    TTree* susyTree = (mapSUSYtrees.find( SUSYhistos[i] ))->second ;
    if( susyTree ) samples.push_back(string(Form("%s",  SUSYhistos[i].c_str() )));
  }

  std::map<std::string,TTree*> tMap;
  tMap["Data"]         = data;
  tMap["Embedded"]     = dataEmbedded;
  tMap["DYToTauTau"]   = backgroundDYTauTau;
  tMap["DYMutoTau"]    = backgroundDYMutoTau;
  tMap["DYJtoTau"]     = backgroundDYJtoTau;
  tMap["WJets"]        = backgroundWJets;
  tMap["W3Jets"]       = backgroundW3Jets;
  tMap["Others"]       = backgroundOthers;
  tMap["TTbar"]        = backgroundTTbar;
  tMap["SS"]           = data;

  string shortProd[nProd]={"ggH","qqH","VH"};
  string sMasses[nMasses] = {"110","115","120","125","130","135","140","145","150","155","160"};

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      tMap[ shortProd[iP] + sMasses[iM] ] = signal[iP][iM] ;

  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    TTree* susyTree = (mapSUSYtrees.find( SUSYhistos[i] ))->second ;
    tMap[SUSYhistos[i]] = susyTree ;
  }



  std::map<TString,Float_t> vMap;

  float SSQCDinSignalRegionDATA = 0.; 
  float extrapFactorWSS = 0.;             
  float SSWinSignalRegionDATA = 0.;       float SSWinSignalRegionMC = 0.; float SSWinSidebandRegionDATA = 0.; float SSWinSidebandRegionMC = 0.;
  float extrapFactorWOSW3Jets = 0.;   
  float OSWinSignalRegionDATAW3Jets = 0.; float OSWinSignalRegionMCW3Jets = 0.; float OSWinSidebandRegionDATAW3Jets = 0.; float OSWinSidebandRegionMCW3Jets = 0.;
  float extrapFactorWOSWJets  = 0.; 
  float OSWinSignalRegionDATAWJets  = 0.; float OSWinSignalRegionMCWJets  = 0.; float OSWinSidebandRegionDATAWJets  = 0.; float OSWinSidebandRegionMCWJets  = 0.; 
  float scaleFactorTTOSW3Jets = 0.;
  float scaleFactorTTOSWJets  = 0.;
 
  TTree* treeForWestimation;
  if(W3JETS && ( ( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ) || selection_.find("twoJets")!=string::npos ) )
    treeForWestimation = backgroundW3Jets ;
  else treeForWestimation = backgroundWJets ;
  
  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << endl;
    cout << ">>>> Dealing with sample ## " << samples[iter] << " ##" << endl;

    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1       = new TH1F( h1Name ,"" , nBins , bins.GetArray());
    TH1F* hCleaner = new TH1F("hCleaner","",nBins , bins.GetArray());
    if ( !h1->GetSumw2N() )h1->Sumw2(); if ( !hCleaner->GetSumw2N() )hCleaner->Sumw2();
    TTree* currentTree = 0;
    
    if((it->first).find("SS")!=string::npos){
      
      currentTree = (it->second);

      cout << "************** BEGIN QCD evaluation using SS events *******************" << endl;

      TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
      float dummy1 = 0.;      


      TCut sbinPZetaRelSSForWextrapolation = sbinPZetaRelSS;
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	sbinPZetaRelSSForWextrapolation = (sbinPZetaRelSSInclusive&&vbfLoose);     
      

      evaluateQCD(h1, hCleaner, true, "SS", false, removeMtCut, selection_, 
		  SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
		  extrapFactorWSS, 
		  SSWinSignalRegionDATA, SSWinSignalRegionMC,
		  SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
		  hExtrapSS, variable,
		  treeForWestimation, backgroundTTbar, backgroundOthers, 
		  backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
		  Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
		  ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
		  MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
		  OStoSSRatioQCD,
		  antiWsdb, antiWsgn, useMt,
		  scaleFactMu,
		  sbinSS,
		  sbinPZetaRelSSForWextrapolation,
		  sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
		  sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
		  vbfLoose, oneJet, zeroJet);

      cout << "************** END QCD evaluation using SS events *******************" << endl;

      delete hExtrapSS;

      hQCD->Add(h1, 1.0); //hQCD->Sumw2();
      hSS->Add(hCleaner, OStoSSRatioQCD); //hSS->Sumw2();
      hCleaner->Reset();

    }
    else{

      currentTree = (it->second);

      if((it->first).find("Embed")==string::npos){

	float Error = 0.;

	if((it->first).find("DYToTauTau")!=string::npos){
	  float NormDYToTauTau = 0.;
	  drawHistogramMC(currentTree, variable, NormDYToTauTau, Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbin, 1);
	  hZtt->Add(h1, ExtrapolationFactorZFromSideband);
	}
	if((it->first).find("TTbar")!=string::npos){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){ 
	    float NormTTjets = 0.; 
	    TCut sbinVBFLoose = sbinInclusive && vbfLoose; 
	    drawHistogramMC(currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinVBFLoose, 1);
	    hTTb->Add(hCleaner, 1.0);
	    NormTTjets = 0.;
	    drawHistogramMC(currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., h1, sbin, 1);
	    hTTb->Scale(h1->Integral()/hTTb->Integral()); 
	  } 
	  else{
	    float NormTTjets = 0.;
	    drawHistogramMC(currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., h1, sbin, 1);
	    hTTb->Add(h1, 1.0);
	  }
	}
	else if((it->first).find("W3Jets")!=string::npos){

	  TH1F* hExtrapW3Jets = new TH1F("hExtrapW3Jets","",nBins , bins.GetArray());
	 
	  cout << "************** BEGIN W+3jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinPZetaRelForWextrapolation = sbinPZetaRel;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	    sbinPZetaRelForWextrapolation = (sbinPZetaRelInclusive&&vbfLoose);     

	  evaluateWextrapolation("OS", false, selection_, 
				 extrapFactorWOSW3Jets, 
				 OSWinSignalRegionDATAW3Jets,   OSWinSignalRegionMCW3Jets,
				 OSWinSidebandRegionDATAW3Jets, OSWinSidebandRegionMCW3Jets,
				 scaleFactorTTOSW3Jets,
				 hExtrapW3Jets, variable,
				 currentTree, backgroundTTbar, backgroundOthers, 
				 backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 scaleFactMu,
				 sbinPZetaRelForWextrapolation,
				 sbinPZetaRel, sbinPZetaRel,
				 pZ, apZ, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, oneJet, zeroJet);

	  cout << "************** END W+3jets normalization using high-Mt sideband *******************" << endl;
	  delete hExtrapW3Jets;

	  float NormW3Jets = 0.;
	  drawHistogramMC(currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAW3Jets/OSWinSidebandRegionMCW3Jets);
	  else h1->Scale(OSWinSignalRegionDATAW3Jets/h1->Integral());
	  hW3Jets->Add(h1, 1.0);

	  drawHistogramMC(currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinMtiso, 1);
	  hW3JetsMediumTauIso->Add(hCleaner, hW3Jets->Integral()/hCleaner->Integral());

	  drawHistogramMC(currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtiso, 1);
	  hW3JetsLooseTauIso->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());

	  drawHistogramMC(currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive&&vbfLoose, 1);
	  hW3JetsMediumTauIsoRelVBF->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());
	  
	  hW3JetsMediumTauIsoRelVBFMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAW3Jets));
	  //hW3JetsMediumTauIsoRelVBF->Sumw2(); hW3JetsMediumTauIsoRelVBFMinusSS->Sumw2();
	  if(((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
	      selection_.find("twoJets")!=string::npos)){
	    if(!USESSBKG) hEWK->Add(hW3JetsMediumTauIsoRelVBF,1.0);
	    else  hEWK->Add(hW3JetsMediumTauIsoRelVBFMinusSS,1.0);
	  }
	}
	else if((it->first).find("WJets")!=string::npos){

	  TH1F* hExtrapW = new TH1F("hExtrap","",nBins , bins.GetArray());
	  
	  cout << "************** BEGIN W+jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinPZetaRelForWextrapolation = sbinPZetaRel;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	    sbinPZetaRelForWextrapolation = (sbinPZetaRelInclusive&&vbfLoose); 
	  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos) 
            sbinPZetaRelForWextrapolation = (sbinPZetaRelInclusive&&bTagLoose);

	  evaluateWextrapolation("OS", false, selection_, 
				 extrapFactorWOSWJets, 
				 OSWinSignalRegionDATAWJets,   OSWinSignalRegionMCWJets,
				 OSWinSidebandRegionDATAWJets, OSWinSidebandRegionMCWJets,
				 scaleFactorTTOSWJets,
				 hExtrapW, variable,
				 currentTree, backgroundTTbar, backgroundOthers, 
				 backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 scaleFactMu,
				 sbinPZetaRelForWextrapolation,
				 sbinPZetaRel, sbinPZetaRel,
				 pZ, apZ, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet);
	  delete hExtrapW;

	  cout << "************** END W+jets normalization using high-Mt sideband *******************" << endl;

	  float NormSSWJets = 0.;
	  drawHistogramMC(currentTree, variable, NormSSWJets, Error,   Lumi*hltEff_/1000., h1, sbinSS, 1);
	  if(removeMtCut) h1->Scale(SSWinSidebandRegionDATA/SSWinSidebandRegionMC);
	  else h1->Scale(SSWinSignalRegionDATA/h1->Integral());
	  hWSS->Add(h1, 1.0);

	  float NormWJets = 0.;
	  drawHistogramMC(currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	  else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());
	  hW->Add(h1, 1.0); //hW->Sumw2();
	  
	  hWMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));

	  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    float NormWJetsBTag = 0.;
	    drawHistogramMC(currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive&&bTagLoose, 1); 
	    hWLooseBTag->Add(hCleaner,  h1->Integral()/hCleaner->Integral());
	    hEWK->Add(hWLooseBTag,1.0);
	  }
	  else if(!((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
	       selection_.find("twoJets")!=string::npos)) {
	    if(!USESSBKG) hEWK->Add(h1,1.0);
	    else hEWK->Add(hWMinusSS,1.0);
	  }
	}
	else if((it->first).find("DYMutoTau")!=string::npos){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){  
            float NormDYMutoTau = 0.;
            TCut sbinVBFLoose = sbinInclusive && twoJets;  
	    if(useZDataMC)
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbinVBFLoose, 1);
	    else
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbinVBFLoose, 1);
            float NormDYMutoTauEmbdLoose = 0.; 
	    drawHistogramEmbed(dataEmbedded, variable, NormDYMutoTauEmbdLoose,  Error, 1.0 , hCleaner,  sbinVBFLoose  ,1);
	    hCleaner->Reset();
	    float NormDYMutoTauEmbd = 0.;
	    drawHistogramEmbed(dataEmbedded, variable, NormDYMutoTauEmbd,  Error, 1.0 , hCleaner,  sbin  ,1);
            h1->Scale(NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose);
	    hZmm->Add(h1, 1.0); //hZmm->Sumw2(); 
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    hEWK->Add(h1,1.0); 
          }  
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    float NormDYMutoTau = 0.; 
            TCut sbinbTagLoose = sbinInclusive && oneJet;   
	    if(useZDataMC)
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinbTagLoose, 1); 
	    else
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinbTagLoose, 1);
	    NormDYMutoTau = 0.;  
            drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZ*hltEff_/1000., h1, sbinInclusive, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
	    hZmm->Add(hCleaner, 1.0); //hZmm->Sumw2();  
            hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2(); 
            hEWK->Add(hCleaner,1.0);
	  }
	  else{
	    float NormDYMutoTau = 0.;
	    if(useZDataMC)
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else 
	      drawHistogramMC(currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmm->Add(h1, 1.0); //hZmm->Sumw2();
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    hEWK->Add(h1,1.0);
	  }
	}
	else if((it->first).find("DYJtoTau")!=string::npos){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){   
            float NormDYJtoTau = 0.; 
            TCut sbinVBFLoose = sbinInclusive && vbfLoose;   
	    drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinVBFLoose, 1);
	    NormDYJtoTau = 0.; 
	    if(useZDataMC)
	      drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1); 
	    else
	      drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
	    hZmj->Add(hCleaner, 1.0); //hZmj->Sumw2();
	    hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2();
	    hEWK->Add(hCleaner,1.0); 
	  }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    float NormDYJtoTau = 0.;  
            TCut sbinbTagLoose = sbinInclusive && oneJet;    
            drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinbTagLoose, 1); 
            NormDYJtoTau = 0.;  
            drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZ*hltEff_/1000., h1, sbinInclusive, 1);  
            hCleaner->Scale(h1->Integral()/hCleaner->Integral()); 
            hZmj->Add(hCleaner, 1.0); //hZmj->Sumw2(); 
            hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2(); 
            hEWK->Add(hCleaner,1.0);  
          } 
	  else{
	    float NormDYJtoTau = 0.;
	    if(useZDataMC)
	      drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogramMC(currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmj->Add(h1, 1.0); //hZmj->Sumw2();
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    hEWK->Add(h1,1.0);
	  }
	}
	else if((it->first).find("Others")!=string::npos){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){    
            float NormOthers = 0.;  
            TCut sbinVBFLoose = sbinInclusive && vbfLoose;    
	    drawHistogramMC(currentTree, variable, NormOthers, Error,     Lumi*hltEff_/1000., hCleaner, sbinVBFLoose, 1);
	    NormOthers = 0.; 
            drawHistogramMC(currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
            hCleaner->Scale(h1->Integral()/hCleaner->Integral()); 
            hVV->Add(hCleaner, 1.0);  
            hEWK->Add(hCleaner,1.0);  
          } 
	  else{
	    float NormOthers = 0.;
	    drawHistogramMC(currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
	    hVV->Add(h1, 1.0);
	    hEWK->Add(h1,1.0);
	  }
	}
	else if((it->first).find("Data")!=string::npos){

	  float NormData = 0.;
	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , h1, sbin, 1);
	  hData->Add(h1, 1.0);
	  if ( !hData->GetSumw2N() )hData->Sumw2();
	  
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtiso ,1);
	    float tmpNorm = hCleaner->Integral();
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtisoInclusive&&vbfLooseQCD ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral()));

	    //get efficiency of events passing QCD selection to pass the category selection 
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    float tmpNormQCDSel = hCleaner->Integral();
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1); 
            float tmpNormCatSel = hCleaner->Integral();
	    float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
	    //Normalize to Inclusive measured QCD times the above efficiency
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("novbfHigh")!=string::npos || selection_.find("boost")!=string::npos){
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    float NormDYMutoTau = 0;
	    drawHistogramMC(backgroundDYMutoTau, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    float NormDYJtoTau = 0.; 
            drawHistogramMC(backgroundDYJtoTau, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    float NormTTjets = 0.; 
            drawHistogramMC(backgroundTTbar, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTSS*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }

	  else if(selection_.find("novbfLow")!=string::npos) {
	    TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
	    TCut sbinPZetaRelSSForWextrapolation = sbinPZetaRelSS;
	    float dummy1 = 0.;      
	    evaluateQCD(hDataAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapSS, variable,
			treeForWestimation, backgroundTTbar, backgroundOthers, 
			backgroundDYTauTau, backgroundDYJtoTau, backgroundDYMutoTau, data,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			scaleFactMu,
			sbinSS,
			sbinPZetaRelSSForWextrapolation,
			sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
			vbfLoose, oneJet, zeroJet, true, true);
	    
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    delete hExtrapSS;
	    hCleaner->Reset();
	  }

	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1); 
            hDataAntiIsoLooseTauIso->Add(hCleaner); 
            float NormDYMutoTau = 0; 
	    if(useZDataMC)
	      drawHistogramMC(backgroundDYMutoTau, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    else
	      drawHistogramMC(backgroundDYMutoTau, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
            float NormDYJtoTau = 0.;  
	    if(useZDataMC)
	      drawHistogramMC(backgroundDYJtoTau, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    else
	      drawHistogramMC(backgroundDYJtoTau, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
            float NormTTjets = 0.;  
            drawHistogramMC(backgroundTTbar, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    if(selection_.find("High")!=string::npos){
	      //get efficiency of events passing QCD selection to pass the category selection  
	      drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive,1); 
	      float tmpNormQCDSel = hCleaner->Integral(); 
	      drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1);  
	      float tmpNormCatSel = hCleaner->Integral(); 
	      float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel; 
	      //Normalize to Inclusive measured QCD times the above efficiency 
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral()); 
	    }
	    else{
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    }
	  }
	  else{
	    drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtiso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD);
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }
	  //hDataAntiIsoLooseTauIsoQCD->Sumw2();
	  //hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());

	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso1 ,1);
	  hLooseIso1->Add(hCleaner, 1.0);
	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso2 ,1);
	  hLooseIso2->Add(hCleaner, 1.0);
	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso3 ,1);
	  hLooseIso3->Add(hCleaner, 1.0);
	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso  ,1);
	  hAntiIso->Add(hCleaner, 1.0);
	  drawHistogramDataFakeRate(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso , scaleFactMu ,1);
	  hAntiIsoFR->Add(hCleaner, 1.0);
	  
	  cleanQCDHisto(true,hCleaner, hLooseIso1, variable, 
			backgroundWJets, backgroundTTbar, backgroundOthers, 
			backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, 
			Lumi*hltEff_/1000., (SSWinSidebandRegionDATA/SSWinSidebandRegionMC), 
			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso1);
 	  cleanQCDHisto(true,hCleaner, hLooseIso2, variable, 
 			backgroundWJets, backgroundTTbar, backgroundOthers, 
 			backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso2);
 	  cleanQCDHisto(true,hCleaner, hLooseIso3, variable, 
 			backgroundWJets, backgroundTTbar, backgroundOthers, 
 			backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso3);


	  drawHistogramData(currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSInclusive&&vbfLoose , 1);
	  hSSLooseVBF->Add(hCleaner, 1.0);
	  cleanQCDHisto(false,hCleaner, hSSLooseVBF, variable, 
 			backgroundWJets, backgroundTTbar, backgroundOthers, 
 			backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSInclusive&&vbfLoose);
	  hSSLooseVBF->Scale(hSS->Integral()/hSSLooseVBF->Integral());

	  // Study QCD shape (Nadir) //
	  if(studyQCDshape) {
	    TCut catForShape=sbinTmp;
	    if(selection_=="vbf") catForShape=vbfLoose;
	    //{"hQCD","hData","hW","hTT","hDYMuTau","hDYJetTau","hDYTauTau","hOthers"};

	    shapeQCD( h_QCDshape[0][0],h_QCDshape[1][0],h_QCDshape[2][0],h_QCDshape[3][0],
		      h_QCDshape[4][0],h_QCDshape[5][0],h_QCDshape[6][0],h_QCDshape[7][0],
		      //variable, sbinAisoInclusive, catForShape,
		      variable, "", "",
		      treeForWestimation, backgroundTTbar, backgroundOthers, 
		      backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, data,
		      Lumi*hltEff_/1000., 1.0, 1.0,
		      MutoTauCorrectionFactor, JtoTauCorrectionFactor, 1.0);
	      
	    shapeQCD( h_QCDshape[0][1],h_QCDshape[1][1],h_QCDshape[2][1],h_QCDshape[3][1],
		      h_QCDshape[4][1],h_QCDshape[5][1],h_QCDshape[6][1],h_QCDshape[7][1],
		      //variable, sbinSSaIsoInclusive, catForShape,
		      variable, "", "",
		      treeForWestimation, backgroundTTbar, backgroundOthers, 
		      backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, data,
		      Lumi*hltEff_/1000., 1.0, 1.0,
		      MutoTauCorrectionFactor, JtoTauCorrectionFactor, 1.0);

	    float nData_qcdSh=0, Error=0;
	    drawHistogramData(data, variable, nData_qcdSh, Error, 1.0, h_QCDshape[1][0], "" ,1);
	    cout << "TRY TO DEBUG : nData=" << nData_qcdSh << endl;

	    for(int iS=0 ; iS<nSign ; iS++)
	      for(int iSam=0 ; iSam<nSamples ; iSam++)
		h_QCDshape[iSam][iS]->Sumw2();
	    
	  }

//  	  cleanQCDHisto(hCleaner, hAntiIso, variable, 
//  			backgroundWJets, backgroundTTbar, backgroundOthers, 
//  			backgroundDYMutoTau, backgroundDYJtoTau, backgroundDYTauTau, 
//  			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
//  			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
//  			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSaIso);


// 	  float pseudoExtrapolationFactor = 0.; float ErrorPseudoExtrapolationFactor = 0.;
// 	  TH1F* hCleanerMt = new TH1F("hCleanerMt","",400,0,400);
// 	  TH1F* hWMtMt     = new TH1F("hWMtMt",    "",400,0,400);
// 	  evaluateWusingSSEvents(hCleanerMt,int(antiWsgn),int(antiWsdb), 
// 				 selection_,
// 				 pseudoExtrapolationFactor,
// 				 ErrorPseudoExtrapolationFactor,
// 				 hWMtMt, TString(antiWcut.c_str()),
// 				 backgroundWJets,backgroundTTbar,backgroundOthers,
// 				 backgroundDYTauTau,backgroundDYJtoTau,backgroundDYMutoTau,currentTree,
// 				 Lumi*hltEff_/1000.,TTxsectionRatio,lumiCorrFactor,
// 				 ExtrapolationFactorSidebandZDataMC,ExtrapolationFactorZDataMC,
// 				 MutoTauCorrectionFactor,JtoTauCorrectionFactor,
// 				 scaleFactorTTSS,
// 				 scaleFactMu,
// 				 sbinPZetaRelSS);
// 	  float a      = OSWinSidebandRegionDATAWJets/pseudoExtrapolationFactor;
// 	  float errorA = a*TMath::Sqrt( 1./OSWinSidebandRegionDATAWJets + 
// 					(ErrorPseudoExtrapolationFactor/pseudoExtrapolationFactor)*
// 					(ErrorPseudoExtrapolationFactor/pseudoExtrapolationFactor));
// 	  float b      = OSWinSidebandRegionDATAWJets/SSWinSidebandRegionDATA;
// 	  float errorB = b*TMath::Sqrt( 1./OSWinSidebandRegionDATAWJets + 1./SSWinSidebandRegionDATA );
// 	  float c      = hDataAntiIsoLooseTauIso->Integral()/OStoSSRatioQCD;
// 	  float errorC = 0.10*c;
// 	  float WfromSSEvents      = a - b*c;
// 	  float ErrorWfromSSEvents = TMath::Sqrt(errorA*errorA + c*c*errorB*errorB + b*b*errorC*errorC);
// 	  cout << "WfromSSEvents = " << OSWinSidebandRegionDATAWJets << "/" << pseudoExtrapolationFactor 
// 	       << " - " << OSWinSidebandRegionDATAWJets/SSWinSidebandRegionDATA << "*" << hDataAntiIsoLooseTauIso->Integral()/OStoSSRatioQCD 
// 	       << " = " << WfromSSEvents << " +/- " << ErrorWfromSSEvents <<  endl;
// 	  delete hCleanerMt; delete hWMtMt;
	}



	else if((it->first).find("qqH") !=string::npos || 
		(it->first).find("ggH") !=string::npos ||
		(it->first).find("VH")  !=string::npos  ||
		(it->first).find("SUSY")!=string::npos){

	  float NormSign = 0.;
	  drawHistogramMC(currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, sbin, 1);
	   
	  if((it->first).find(string(Form("qqH%d",mH_)))!=string::npos){
	    hSgn1->Add(h1,1.0);
	    hSgn1->Scale(magnifySgn_);
	    hSgn->Add(hSgn1,1.0);
	  }
	  else if((it->first).find(string(Form("ggH%d",mH_)))!=string::npos){
	    hSgn2->Add(h1,1.0);
	    hSgn2->Scale(magnifySgn_);
	    hSgn->Add(hSgn2,1.0);
	  }
	  else  if((it->first).find(string(Form("VH%d",mH_)))!=string::npos){
	    hSgn3->Add(h1,1.0);
	    hSgn3->Scale(magnifySgn_);
	    hSgn->Add(hSgn3,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",110)))!=string::npos){
	    hggH110->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",115)))!=string::npos){
	    hggH115->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",120)))!=string::npos){
	    hggH120->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",125)))!=string::npos){
	    hggH125->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",130)))!=string::npos){
	    hggH130->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",135)))!=string::npos){
	    hggH135->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",140)))!=string::npos){
	    hggH140->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("ggH%d",145)))!=string::npos){
	    hggH145->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",110)))!=string::npos){
	    hqqH110->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",115)))!=string::npos){
	    hqqH115->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",120)))!=string::npos){
	    hqqH120->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",125)))!=string::npos){
	    hqqH125->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",130)))!=string::npos){
	    hqqH130->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",135)))!=string::npos){
	    hqqH135->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",140)))!=string::npos){
	    hqqH140->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("qqH%d",145)))!=string::npos){
	    hqqH145->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",110)))!=string::npos){
	    hVH110->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",115)))!=string::npos){
	    hVH115->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",120)))!=string::npos){
	    hVH120->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",125)))!=string::npos){
	    hVH125->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",130)))!=string::npos){
	    hVH130->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",135)))!=string::npos){
	    hVH135->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",140)))!=string::npos){
	    hVH140->Add(h1,1.0);
	  }
	  if((it->first).find(string(Form("VH%d",145)))!=string::npos){
	    hVH145->Add(h1,1.0);
	  }

	  if((it->first).find("SUSY")!=string::npos){
	    TH1F* histoSusy =  (mapSUSYhistos.find( (it->first) ))->second;
	    histoSusy->Add(h1,1.0);
	    histoSusy->SetLineWidth(2);
	  }

	}

      }

      else{
	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  /* //HCP method 
	    float NormEmbed = 0.;
	  TCut sbinEmbeddingLoose = sbinEmbeddingInclusive && vbfLoose;
	  drawHistogramEmbed(currentTree, variable, NormEmbed,  Error, 1.0 , hCleaner,  sbinEmbeddingLoose  ,1);
	  hDataEmb->Add(hCleaner, 1.0);
	  NormEmbed = 0.; 
	  drawHistogramEmbed(currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1); 
	  h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral()); 
	  hDataEmb->Scale(h1->Integral()/hDataEmb->Integral());
	  */
	  //Moriond method
	  float NormEmbed = 0.; 
          drawHistogramEmbed(currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1); 
          h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral()); 
          hDataEmb->Add(h1, 1.0); 
	}
	else{
	  float NormEmbed = 0.;
	  drawHistogramEmbed(currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1);
	  h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral());
	  hDataEmb->Add(h1, 1.0);
	}
      }
    }
  
    /////////////////////////////////////////////////////////////////////////////////////

    if(VERBOSE) cout<<(it->first) << " ==> " 
		    << h1->Integral() << " +/- " 
		    << TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries())
		    << endl;

    //out << (it->first) << "  " << h1->Integral() << " $\\pm$ " <<  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()) << endl;
    char* c = new char[50];
    if(h1->Integral()>=10) 
      sprintf(c,"$%.0f\\pm%.0f$",h1->Integral(),  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()));
    else if(h1->Integral()>=1)
      sprintf(c,"$%.1f\\pm%.1f$",h1->Integral(),  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()));
    else if(h1->Integral()>=0.1)
      sprintf(c,"$%.2f\\pm%.2f$",h1->Integral(),  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()));
    else if(h1->Integral()>=0.01)
      sprintf(c,"$%.3f\\pm%.3f$",h1->Integral(),  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()));
    else
      sprintf(c,"$%.5f\\pm%.5f$",h1->Integral(),  TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries()));
    out << string(c) << "  //" << (it->first) << endl;
    delete c;

    delete hCleaner;
  }
  out.close();

  cout << endl;
  cout << "All samples done. Filling hParameters..." << endl;
  hParameters->SetBinContent(1, ExtrapolationFactorZ);               hParameters->GetXaxis()->SetBinLabel(1,"ExtrapolationFactorZ");
  hParameters->SetBinContent(2, ErrorExtrapolationFactorZ);          hParameters->GetXaxis()->SetBinLabel(2,"ErrorExtrapolationFactorZ");
  hParameters->SetBinContent(3, ExtrapolationFactorZDataMC);         hParameters->GetXaxis()->SetBinLabel(3,"ExtrapolationFactorZDataMC");
  hParameters->SetBinContent(4, ExtrapolationFactorZFromSideband);   hParameters->GetXaxis()->SetBinLabel(4,"ExtrapolationFactorZFromSideband");
  hParameters->SetBinContent(5, ExtrapolationFactorSidebandZDataMC); hParameters->GetXaxis()->SetBinLabel(5,"ExtrapolationFactorSidebandZDataMC");
  hParameters->SetBinContent(6, extrapFactorWSSIncl);                hParameters->GetXaxis()->SetBinLabel(6,"extrapFactorWSSIncl");
  hParameters->SetBinContent(7, SSWinSignalRegionDATAIncl);          hParameters->GetXaxis()->SetBinLabel(7,"SSWinSignalRegionDATAIncl");
  hParameters->SetBinContent(8, SSWinSignalRegionMCIncl);            hParameters->GetXaxis()->SetBinLabel(8,"SSWinSignalRegionMCIncl");
  hParameters->SetBinContent(9, SSQCDinSignalRegionDATAIncl);        hParameters->GetXaxis()->SetBinLabel(9,"SSQCDinSignalRegionDATAIncl");
  hParameters->SetBinContent(10,extrapFactorWSS);                    hParameters->GetXaxis()->SetBinLabel(10,"extrapFactorWSS");
  hParameters->SetBinContent(11,SSWinSignalRegionDATA);              hParameters->GetXaxis()->SetBinLabel(11,"SSWinSignalRegionDATA");
  hParameters->SetBinContent(12,SSWinSignalRegionMC);                hParameters->GetXaxis()->SetBinLabel(12,"SSWinSignalRegionMC");
  hParameters->SetBinContent(13,SSQCDinSignalRegionDATA);            hParameters->GetXaxis()->SetBinLabel(13,"SSQCDinSignalRegionDATA");
  hParameters->SetBinContent(14,extrapFactorWOSWJets);               hParameters->GetXaxis()->SetBinLabel(14,"extrapFactorWOSWJets");
  hParameters->SetBinContent(15,OSWinSignalRegionDATAWJets);         hParameters->GetXaxis()->SetBinLabel(15,"OSWinSignalRegionDATAWJets");
  hParameters->SetBinContent(16,OSWinSignalRegionMCWJets );          hParameters->GetXaxis()->SetBinLabel(16,"OSWinSignalRegionMCWJets");   
  hParameters->SetBinContent(17,extrapFactorWOSW3Jets);              hParameters->GetXaxis()->SetBinLabel(17,"extrapFactorWOSW3Jets");
  hParameters->SetBinContent(18,OSWinSignalRegionDATAW3Jets);        hParameters->GetXaxis()->SetBinLabel(18,"OSWinSignalRegionDATAW3Jets");
  hParameters->SetBinContent(19,OSWinSignalRegionMCW3Jets );         hParameters->GetXaxis()->SetBinLabel(19,"OSWinSignalRegionMCW3Jets");
  hParameters->SetBinContent(20,scaleFactorTTOS);                    hParameters->GetXaxis()->SetBinLabel(20,"scaleFactorTTOS");
  hParameters->SetBinContent(21,scaleFactorTTSS);                    hParameters->GetXaxis()->SetBinLabel(21,"scaleFactorTTSS");
  hParameters->SetBinContent(22,scaleFactorTTSSIncl);                hParameters->GetXaxis()->SetBinLabel(22,"scaleFactorTTSSIncl");
  hParameters->SetBinContent(23,SSIsoToSSAIsoRatioQCD);              hParameters->GetXaxis()->SetBinLabel(23,"SSIsoToSSAIsoRatioQCD");
  hParameters->SetBinContent(24,ExtrapDYInclusive);                  hParameters->GetXaxis()->SetBinLabel(24,"ExtrapDYInclusive");
  hParameters->SetBinContent(25,ExtrapDYInclusivePZetaRel);          hParameters->GetXaxis()->SetBinLabel(25,"ExtrapDYInclusivePZetaRel");
  hParameters->SetBinContent(26,ExtrapLFakeInclusive);               hParameters->GetXaxis()->SetBinLabel(26,"ExtrapLFakeInclusive");
  hParameters->SetBinContent(27,ExtrapJFakeInclusive);               hParameters->GetXaxis()->SetBinLabel(27,"ExtrapJFakeInclusive");
  hParameters->SetBinContent(28,ExtrapolationFactorMadGraph);        hParameters->GetXaxis()->SetBinLabel(28,"ExtrapolationFactorMadGraph");
  hParameters->SetBinContent(29,ErrorExtrapolationFactorMadGraph);   hParameters->GetXaxis()->SetBinLabel(29,"ErrorExtrapolationFactorMadGraph");

  hParameters->GetXaxis()->LabelsOption("v");

  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass")){ 
    hData->Scale(1.0, "width");
    hTTb->Scale(1.0, "width");
    hDataEmb->Scale(1.0, "width");
    hZtt->Scale(1.0, "width");
    hDataAntiIsoLooseTauIsoQCD->Scale(1.0, "width");
    hQCD->Scale(1.0, "width");
    hSSLooseVBF->Scale(1.0, "width");
    hSS->Scale(1.0, "width");
    hEWK->Scale(1.0, "width");
    hSgn->Scale(1.0, "width");
  }

  hSiml->Add(hTTb,1.0);
  if(useEmbedding_)
    hSiml->Add(hDataEmb,1.0);
  else 
    hSiml->Add(hZtt);

  if(!USESSBKG){
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
      hSiml->Add(hDataAntiIsoLooseTauIsoQCD,1.0);
    else if(selection_.find("novbf")!=string::npos || selection_.find("boost")!=string::npos)
      hSiml->Add(hDataAntiIsoLooseTauIsoQCD,1.0);
    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)
      hSiml->Add(hDataAntiIsoLooseTauIsoQCD,1.0);
    else
      hSiml->Add(hQCD,1.0);
  }
  else{
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
      hSiml->Add(hSSLooseVBF,1.0);
    }
    else
      hSiml->Add(hSS,1.0);
  }

  hSiml->Add(hEWK,1.0);

  if(!USESSBKG){
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
      aStack->Add(hDataAntiIsoLooseTauIsoQCD);
    }
    else if(selection_.find("novbf")!=string::npos || selection_.find("boost")!=string::npos) 
      aStack->Add(hDataAntiIsoLooseTauIsoQCD);
    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)
      aStack->Add(hDataAntiIsoLooseTauIsoQCD);
    else
      aStack->Add(hQCD);
  }
  else{
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
      aStack->Add(hSSLooseVBF);
    }
    else
      aStack->Add(hSS);
  }
  
  aStack->Add(hEWK);
  aStack->Add(hTTb);
  if(useEmbedding_)
    aStack->Add(hDataEmb);
  else
    aStack->Add(hZtt);
  if(!logy_)
    aStack->Add(hSgn);
  
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hSgn,Form("(%.0fx) H#rightarrow#tau#tau m_{H}=%d",magnifySgn_,mH_),"F");
  if(useEmbedding_)
    leg->AddEntry(hDataEmb,"Z#rightarrow#tau#tau (embedded)","F");
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
  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass")){
    hStack->SetYTitle(Form(" Events/(%.0f %s)", 1.0, Unities_.Data() ) );
    hData->SetYTitle(Form(" Events/(%.0f %s)", 1.0, Unities_.Data() ) );
  }
  else 
    hStack->SetYTitle(Form(" Events/(%.0f %s)", hStack->GetBinWidth(1), Unities_.Data() ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.95,"Y");
  if(!logy_)
    hData->SetAxisRange(0.0, TMath::Max( hData->GetMaximum(), hSiml->GetMaximum() )*maxY_ ,"Y");
  else
    hData->SetAxisRange(0.1, TMath::Max( hData->GetMaximum(), hSiml->GetMaximum() )*maxY_ ,"Y");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");
  if(logy_)
    hSgn->Draw("HISTSAME");
 
  leg->Draw();

  pad2->cd();
  gStyle->SetOptStat(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleH(0.07);
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleOffset(1.3,"y");

  TH1F* hRatio = new TH1F( "hRatio" ," ; ; #frac{(DATA-MC)}{#sqrt{DATA}}" , nBins , bins.GetArray());
  hRatio->Reset();
  hRatio->SetXTitle("");
  hRatio->SetYTitle("#frac{(DATA-MC)}{MC}");

  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");

  float maxPull = 0.;
  for(int k = 0 ; k < hRatio->GetNbinsX(); k++){
    float pull = hData->GetBinContent(k) - hSiml->GetBinContent(k);
    if(hSiml->GetBinContent(k)>0)
      pull /= hSiml->GetBinContent(k);
    hRatio->SetBinContent(k, pull);
    if(TMath::Abs(pull) > maxPull)
      maxPull = TMath::Abs(pull);
  }
  hRatio->SetAxisRange(-1.2*maxPull,1.2*maxPull,"Y");
  hRatio->Draw("P");

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");
  
  //return;

  c1->SaveAs(Form(location+"/plots/%s/plot_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
  c1->SaveAs(Form(location+"/plots/%s/plot_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));

  // templates for fitting
  TFile* fout = new TFile(Form(location+"/histograms/%s/muTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");
  fout->cd();

  hSiml->Write();
  hQCD->Write();
  hSS->Write();
  hSSLooseVBF->Write();
  hZmm->Write();
  hZmmLoose->Write();
  hZmj->Write();
  hZmjLoose->Write();
  hZfakes->Write();
  hTTb->Write();
  hZtt->Write();
  hDataEmb->Write();
  hLooseIso1->Write();
  hLooseIso2->Write();
  hLooseIso3->Write();
  hAntiIso->Write();
  hAntiIsoFR->Write();
  hW->Write();
  hWSS->Write();
  hWMinusSS->Write();
  hW3Jets->Write();
  hVV->Write();
  hEWK->Write();
  hSgn1->Write();
  hSgn2->Write();
  hSgn3->Write();
  hW3JetsLooseTauIso->Write();
  hW3JetsMediumTauIso->Write();
  hW3JetsMediumTauIsoRelVBF->Write();
  hW3JetsMediumTauIsoRelVBFMinusSS->Write();
  hWLooseBTag->Write();
  hDataAntiIsoLooseTauIso->Write();
  hDataAntiIsoLooseTauIsoQCD->Write();
  hData->Write();
  hParameters->Write();
  hggH110->Write(); hggH115->Write(); hggH120->Write(); hggH125->Write();  
  hggH130->Write(); hggH135->Write(); hggH140->Write(); hggH145->Write(); 
  hqqH110->Write(); hqqH115->Write(); hqqH120->Write(); hqqH125->Write();  
  hqqH130->Write(); hqqH135->Write(); hqqH140->Write(); hqqH145->Write();  
  hVH110->Write();  hVH115->Write();  hVH120->Write();  hVH125->Write();  
  hVH130->Write();  hVH135->Write();  hVH140->Write();  hVH145->Write(); 
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    ((mapSUSYhistos.find( SUSYhistos[i] ))->second)->Write();
  }
 
  for(int iS=0 ; iS<nSign ; iS++)
    for(int iSam=0 ; iSam<nSamples ; iSam++)
      h_QCDshape[iSam][iS]->Write();

  fout->Write();
  fout->Close();

  delete hQCD; delete hSS; delete hSSLooseVBF; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hZtt; 
  delete hW; delete hWSS; delete hWMinusSS; delete hW3Jets; delete hAntiIso; delete hAntiIsoFR;
  delete hZmmLoose; delete hZmjLoose; delete hLooseIso1; delete hLooseIso2; delete hLooseIso3;
  delete hVV; delete hSgn; delete hSgn1; delete hSgn2; delete hSgn3; delete hData; delete hParameters;
  delete hW3JetsLooseTauIso; delete hW3JetsMediumTauIso; delete hW3JetsMediumTauIsoRelVBF; delete hW3JetsMediumTauIsoRelVBFMinusSS; delete hWLooseBTag;
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoQCD;
  delete hggH110; delete hggH115 ; delete hggH120; delete hggH125; delete hggH130; delete hggH135; delete hggH140; delete hggH145;
  delete hqqH110; delete hqqH115 ; delete hqqH120; delete hqqH125; delete hqqH130; delete hqqH135; delete hqqH140; delete hqqH145;
  delete hVH110;  delete hVH115 ;  delete hVH120;  delete hVH125;  delete hVH130;  delete hVH135;  delete hVH140;  delete hVH145;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++) delete mapSUSYhistos.find( SUSYhistos[i] )->second ;

  for(int iS=0 ; iS<nSign ; iS++)
    for(int iSam=0 ; iSam<nSamples ; iSam++)
      delete h_QCDshape[iSam][iS];
      
  delete aStack;  delete hEWK; delete hSiml; delete hDataEmb;  delete hRatio; delete line;
  delete fout;

  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      fSignal[iP][iM]->Close();
      delete fSignal[iP][iM];
    }
  }

  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    (mapSUSYfiles.find( SUSYhistos[i] )->second)->Close();
    delete mapSUSYfiles.find( SUSYhistos[i] )->second ;
  }

  fBackgroundOthers->Close();delete fBackgroundOthers;
  fBackgroundTTbar->Close(); delete fBackgroundTTbar;
  fBackgroundWJets->Close(); delete fBackgroundWJets;
  fData->Close();            delete fData; 
  dummy1->Close();           delete dummy1;
  fBackgroundDY->Close();    delete fBackgroundDY;

}


///////////////////////////////////////////////////////////////////////////////////////////////



void plotMuTauAll( Int_t useEmbedded = 1, TString outputDir = "TempTest"){
      
  vector<string> variables;
  vector<int> mH;

  //variables.push_back("diTauVisMass");
  variables.push_back("diTauNSVfitMass");
 
  //mH.push_back(105);
  //mH.push_back(110);
  //mH.push_back(115);
  //mH.push_back(120);
  mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);
  //mH.push_back(140);
  //mH.push_back(145);
  //mH.push_back(160);
  
  //plotMuTau(125,1,"inclusive",""   ,"decayMode",     "#tau_{h} decay mode","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
  //plotMuTau(125,1,"inclusive",""   ,"visibleTauMass","visible #tau_{h} mass","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  
  //plotMuTau(125,1,"inclusive",""   ,"MEtMVA","MET","GeV"                        ,outputDir,40,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"inclusive",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,32,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"inclusiveNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,40,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"inclusive",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,50,0,200,5.0,1.0,0,1.2); 
  //plotMuTau(125,1,"inclusive",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,60,0,360,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"inclusive",""   ,"etaL1","#mu #eta", "units"              ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"inclusive",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"inclusive",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"inclusive",""   ,"etaL2","#tau #eta","units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"inclusive",""   ,"numPV","reconstructed vertexes","units"             ,outputDir,30,0,30,5.0,1.0,0,1.5);
  //plotMuTau(125,1,"inclusive",""   ,"nJets30","jet multiplicity","units"                 ,outputDir,10,0, 10,5.0,1.0,1,10);
  //plotMuTau(125,1,"bTag",""        ,"ptB1", "leading b-tagged jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
  //plotMuTau(125,1,"bTag",""        ,"etaB1","leading b-tagged jet #eta","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //
  //plotMuTau(125,1,"novbfLow",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0,   1.2);
  //plotMuTau(125,1,"novbfLow",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"novbfLowNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfLow",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,20,0,200,5.0,1.0,0,1.2);  
  ////plotMuTau(125,1,"novbfLow",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfLow",""   ,"etaL1","#mu #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"novbfLow",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfLow",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfLow",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //
  //plotMuTau(125,1,"novbfHigh",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
  //plotMuTau(125,1,"novbfHigh",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"novbfHighNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfHigh",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2); 
  //plotMuTau(125,1,"novbfHigh",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfHigh",""   ,"etaL1","#mu #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"novbfHigh",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfHigh",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"novbfHigh",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //
  //
  //plotMuTau(125,1,"boostLow",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
  //plotMuTau(125,1,"boostLow",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"boostLowNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostLow",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,20,0,200,5.0,1.0,0,1.2);  
  ////plotMuTau(125,1,"boostLow",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostLow",""   ,"etaL1","#mu #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"boostLow",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostLow",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostLow",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //
  //
  //plotMuTau(125,1,"boostHigh",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
  //plotMuTau(125,1,"boostHigh",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"boostHighNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostHigh",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2); 
  plotMuTau(125,1,"boostHigh",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostHigh",""   ,"etaL1","#mu #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"boostHigh",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostHigh",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"boostHigh",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  //
  //plotMuTau(125,1,"vbf",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
  //plotMuTau(125,1,"vbf",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"vbfNoMt",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"vbf",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2);  
  plotMuTau(125,1,"vbf",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"vbf",""   ,"etaL1","#mu #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0, 2.);
  //plotMuTau(125,1,"vbf",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"vbf",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
  //plotMuTau(125,1,"vbf",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
  return;

  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){

        plotMuTau(mH[j],useEmbedded,"novbfLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

        plotMuTau(mH[j],useEmbedded,"novbfHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"novbfHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

        plotMuTau(mH[j],useEmbedded,"boostLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      
        plotMuTau(mH[j],useEmbedded,"boostHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotMuTau(mH[j],useEmbedded,"boostHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

	plotMuTau(mH[j],useEmbedded,"vbf",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
        plotMuTau(mH[j],useEmbedded,"vbf","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
        plotMuTau(mH[j],useEmbedded,"vbf","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
        plotMuTau(mH[j],useEmbedded,"vbf","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
        plotMuTau(mH[j],useEmbedded,"vbf","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 

    }
  }
  
  return;

}

int main(int argc, const char* argv[])
{

  std::cout << "plotMuTau()" << std::endl;
  gROOT->SetBatch(true);

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  int mH, nBins, logy; 
  float magnify, hltEff, xMin, xMax, maxY;
  string category, analysis, variable, xtitle, unity, outputDir;

  //cout << argc << " arguments" << endl < endl;

  if(argc==1) plotMuTauAll();
  //else if(argc>14) { 
  else {
    mH=(int)atof(argv[1]); category=argv[2]; variable=argv[3]; 

    int i=4;
    while(string(argv[i])!="XX") {
      xtitle += string(argv[i]);
      xtitle += " "; 
      i++ ;
    }
    i++ ;
    unity=argv[i]; 

    nBins=(int)atof(argv[i+1]); xMin=atof(argv[i+2]); xMax=atof(argv[i+3]); 

    magnify=atof(argv[i+4]); hltEff=atof(argv[i+5]); logy=(int)atof(argv[i+6]); maxY=atof(argv[i+7]) ;

    outputDir=argv[i+8]; analysis = argc>i+9 ? argv[i+9] : ""; 
    
    cout << endl
	 << "plotMuTau(" << mH << ",1," << category << "," << analysis << "," << variable << "," << xtitle << "," 
	 << unity << "," << outputDir << "," << nBins << "," << xMin << "," << xMax << "," << magnify << "," 
	 << hltEff << "," << logy << "," << maxY << ")" << endl << endl;

    plotMuTau(mH,1,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY);

  }
  //else { cout << "Please put at least 14 arguments" << endl; return 1;}

  cout << "DONE" << endl;
  return 0;
}
