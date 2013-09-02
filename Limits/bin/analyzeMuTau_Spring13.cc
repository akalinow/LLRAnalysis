#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include "TChain.h"
#include "TEntryList.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"
#include "TH1F.h"
#include "TH2F.h"
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

#define RERECO           true
#define MSSM             false
#define VERBOSE          true
#define DEBUG            false
#define LOOP             true
#define USESSBKG         false
#define scaleByBinWidth  false
#define DOSPLIT          false
#define useZDataMC       false
#define StudyQCD         false
typedef map<TString, TChain* >  mapchain;

///////////////////////////////////////////////////////////////////////////////////////////////
TH1F* blindHistogram(TH1F* h, float xmin, float xmax, TString name) {

  TH1F* hOut = (TH1F*)h->Clone(name);
  int nBins  = hOut->GetNbinsX();
  float x    = 0;

  for(int i=1; i<nBins+1 ; i++) {
    x = hOut->GetBinCenter(i);
    if( x>xmin && x<xmax ) hOut->SetBinContent(i,-10);
  }

  return hOut;

}
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
void chooseSelection(TString version_, TCut& tiso, TCut& ltiso, TCut& mtiso, TCut& antimu, TCut& liso, TCut& laiso, TCut& lliso, TCut& tpt)
{

  if(version_.Contains("tauptbin")) tpt="ptL2>45 && ptL2<50";
  else                              tpt="ptL2>20";

  // Lepton isolation //
  liso = "combRelIsoLeg1DBetav2<0.10";
  laiso = "combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50";
  lliso = "combRelIsoLeg1DBetav2<0.30";

  if(version_.Contains("VeryTightLiso")) {
    liso = "combRelIsoLeg1DBetav2<0.04";
    laiso = "combRelIsoLeg1DBetav2>0.10 && combRelIsoLeg1DBetav2<0.30";
    lliso = "combRelIsoLeg1DBetav2<0.06";
  }

  else if(version_.Contains("TightLiso")) {
    liso = "combRelIsoLeg1DBetav2<0.06";
    laiso = "combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50";
    lliso = "combRelIsoLeg1DBetav2<0.15";
  }

  // scan anti-iso trend
  if(     version_.Contains("LAiso1")) laiso = "combRelIsoLeg1DBetav2>0.10 && combRelIsoLeg1DBetav2<=0.20";
  else if(version_.Contains("LAiso2")) laiso = "combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<=0.30";
  else if(version_.Contains("LAiso3")) laiso = "combRelIsoLeg1DBetav2>0.30 && combRelIsoLeg1DBetav2<=0.40";
  else if(version_.Contains("LAiso4")) laiso = "combRelIsoLeg1DBetav2>0.40 && combRelIsoLeg1DBetav2<=0.50";

  // Anti-Mu discriminator //
  if(version_.Contains("AntiMu1"))      antimu = "tightestAntiMuWP>2";
  else if(version_.Contains("AntiMu2")) antimu = "tightestAntiMu2WP>2";
  
  // TauIso1 //
  if(version_.Contains("TauIso1")) {
    tiso   = "tightestHPSMVAWP>=0" ;
    ltiso  = "tightestHPSMVAWP>-99" ;
    mtiso  = "hpsMVA>0.7" ;
  }
  // TauIso2 //
  else if(version_.Contains("TauIso2")) {
    tiso   = "tightestHPSMVA2WP>=0" ;
    ltiso  = "tightestHPSMVA2WP>-99" ;
    mtiso  = "hpsMVA2>0.7" ;
  }
  // TauIso DB3Hits cut-based //
  else if(version_.Contains("HPSDB3H")) {

    if(version_.Contains("VeryTightTiso")) {
      tiso   = "hpsDB3H<0.5" ;
      ltiso  = "hpsDB3H<1.5" ;
      mtiso  = "hpsDB3H<1" ;
    }
    else if(version_.Contains("TightTiso")) {
      tiso   = "hpsDB3H<0.8" ;
      ltiso  = "hpsDB3H<2" ;
      mtiso  = "hpsDB3H<1.5" ;
    }
    else {
      tiso   = "hpsDB3H<1.5" ;
      ltiso  = "hpsDB3H<10.0" ;
      //mtiso  = "hpsDB3H<1" ;
      mtiso  = "hpsDB3H<5.0";
    }
  }

}
///////////////////////////////////////////////////////////////////////////////////////////////

void createStringsIsoFakeRate(TString fileName,
			      string& scaleFactMu, 
			      string& scaleFactMuUp,
			      string& scaleFactMuDown,
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
///////////////////////////////////////////////////////////////////////////////////////////////
void drawHistogram(TCut sbinPair,
		   TCut sbinCat,
		   TString type,
		   TString version_,
		   TString analysis_,
		   TString RUN,
		   TChain* tree, 
		   TString variable, 
		   float& normalization, 
		   float& normalizationError, 
		   float scaleFactor,
		   TH1F* h,
		   TCut cut,
		   int verbose = 0){

  if(DEBUG) cout << "Start drawHistogram : " << type << " " << version_ << " " << RUN << endl;

  // Start processing
  if(tree!=0 && h!=0){

    // Special cases for data/embed
    if(type.Contains("Data")) {
      if(variable=="caloMEtNoHF")    variable="caloMEtNoHFUncorr";
      if(variable=="caloMEtNoHFPhi") variable="caloMEtNoHFUncorrPhi";
    }
    if(type.Contains("Embed") && variable=="L1etmCorr")    variable="L1etm";
    
    //ZL Up/Down shape
    if(type.Contains("MC_ZLUp") && (variable=="diTauNSVfitMass" || variable=="diTauVisMass"))
      variable = variable+"ZLUp";
    else if(type.Contains("MC_ZLDown") && (variable=="diTauNSVfitMass" || variable=="diTauVisMass"))
      variable= variable+"ZLDown";

    // HLT matching //
    TCut hltMatch("run>0");
    if(version_.Contains("SoftD")) {
      if(type.Contains("Data"))    hltMatch = "HLTmatchSoft";
      //else if(type.Contains("MC")) hltMatch = "HLTmatchIsoMu8Tau20";
    }

    // L1ETM CUT //
    TCut passL1ETMCut("run>0");
    if(     version_.Contains("SoftABC")) {
      if(     analysis_.Contains("L1EtmUp"))   passL1ETMCut="passL1etmCutABCUp";
      else if(analysis_.Contains("L1EtmDown")) passL1ETMCut="passL1etmCutABCDown";
      else                                     passL1ETMCut="passL1etmCutABC";
    }
    else if(version_.Contains("SoftD"))    {
      if(     analysis_.Contains("L1EtmUp"))   passL1ETMCut="passL1etmCutUp";
      else if(analysis_.Contains("L1EtmDown")) passL1ETMCut="passL1etmCutDown";
      else                                     passL1ETMCut="passL1etmCut";
    }

    // GenMass cut //
    TCut genMass("run>0");

    // Reweighting
    TCut weight         = "run>0";
    TCut sampleWeight   = "run>0";
    TCut weightDY = "run>0";
    TCut weightW  = "run>0";
    TCut weightEmb= "run>0";

    if(type.Contains("MC")) {

      sampleWeight = "sampleWeight";
      if(version_.Contains("NoDYExcl"))    weightDY = "sampleWeightDY";
      else                                 weightDY = "weightHepNupDY";
      if(version_.Contains("NoWHighStat")) weightW  = "weightHepNup";
      else                                 weightW  = "weightHepNupHighStatW";
      
      if(     version_.Contains("SoftABC"))  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABCShift*SFTau*SFMu_ABC*weightDecayMode)";
      else if(version_.Contains("SoftD"))    weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*weightDecayMode)";
      else if(version_.Contains("SoftLTau")) weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*weightDecayMode)";
      else if(!version_.Contains("Soft")) {
	if(     RUN=="ABC")                  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABC*SFTau*SFMu_ABC*weightDecayMode)";
	else if(RUN=="D")                    weight = "(puWeightD*HLTweightTauD*HLTweightMuD*SFTau*SFMu_D*weightDecayMode)";
	else                                 weight = "(puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*weightDecayMode)";
      }      
      //to be used when weight is available
      if(type.Contains("GGFHUp")) 
	weight *= "HqTWeightUp"; 
      else if(type.Contains("GGFHDown"))  
	weight *= "HqTWeightDown";
      else
	weight *= "HqTWeight";
      
    }
    else if(type.Contains("Embed")) {
      genMass     = "genDiTauMass>50 && HLTxMu17Mu8>0.5"; // HLTxMu17Mu8
      weightEmb   = "embeddingWeight";
      if(version_.Contains("Soft")) {
	weightEmb = "embeddingFilterEffWeight*TauSpinnerWeight*ZmumuEffWeight";
	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTMuABCShift*SFTau*SFMuID_ABC*weightDecayMode)";
	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*weightDecayMode)";
	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*weightDecayMode)";
      }
      else if(!version_.Contains("Soft")) {
	if(RUN=="ABC")                       weight = "(HLTTauABC*HLTMuABC*SFTau*SFMuID_ABC*weightDecayMode)";
	else if(RUN=="D")                    weight = "(HLTTauD*HLTMuD*SFTau*SFMuID_D*weightDecayMode)";
	else                                 weight = "(HLTTau*HLTMu*SFTau*SFMuID*weightDecayMode)";
      }
    }
    
    // Loop over entries to choose the event's pair instead of using pairIndex
    if(LOOP) {

      if(DEBUG) cout << "-- produce skim" << endl;
      //tree->Draw(">>+skim", sbinPair && genMass,"entrylist");
      //tree->Draw(">>+skim", sbinPair,"entrylist");

      tree->Draw(">>+skim", sbinPair,"entrylist");

      TEntryList *skim = (TEntryList*)gDirectory->Get("skim");
      int nEntries = skim->GetN();
      tree->SetEntryList(skim);
      if(DEBUG) cout << "-- produced skim : " << skim->GetN() << "entries" << endl;

      if(DEBUG) cout << "Reset h" << endl;
      h->Reset();

      // Declare tree variables
      int   run  = 0;
      int   event= 0; 
      float etaL2= 0;
      float embeddingWeight= 0;
      int nJets30=0;
      float pt1=0;
      float pt2=0;
      float Mjj=0;
      float Deta=0;
      int isVetoInJets=0;

      // Declare tree branches
      if(DEBUG) cout << "-- declare tree branches" << endl;
      tree->SetBranchStatus("*",  0);   
      tree->SetBranchStatus("run",    1); tree->SetBranchAddress("run",    &run   );
      tree->SetBranchStatus("event",  1); tree->SetBranchAddress("event",  &event );
      tree->SetBranchStatus("etaL2",  1); tree->SetBranchAddress("etaL2",  &etaL2 );
      tree->SetBranchStatus("embeddingWeight",  1); tree->SetBranchAddress("embeddingWeight",  &embeddingWeight );
      tree->SetBranchStatus("nJets30",  1); tree->SetBranchAddress("nJets30",  &nJets30 );
      tree->SetBranchStatus("pt1",  1); tree->SetBranchAddress("pt1",  &pt1 );
      tree->SetBranchStatus("pt2",  1); tree->SetBranchAddress("pt2",  &pt2 );
      tree->SetBranchStatus("Mjj",  1); tree->SetBranchAddress("Mjj",  &Mjj );
      tree->SetBranchStatus("Deta",  1); tree->SetBranchAddress("Deta",  &Deta );
      tree->SetBranchStatus("isVetoInJets",  1); tree->SetBranchAddress("isVetoInJets",  &isVetoInJets );
    
      // Variables for the loop
      int nReject=0, nAccept=0;
      int treenum, iEntry, chainEntry, lastRun, lastEvent;
      treenum = iEntry = chainEntry = lastRun = lastEvent = -1;

      if(DEBUG) cout << "-- loop" << endl;
      for(Long64_t i=0 ; i<nEntries ; i++) {
	
	iEntry     = skim->GetEntryAndTree(i, treenum);
	chainEntry = iEntry + (tree->GetTreeOffset())[treenum];
	tree->GetEntry(chainEntry);

	// reject all pairs of the evt except first passing selection (skim)
	if(run==lastRun && event==lastEvent) {
	  
	  skim->Remove(i, tree);

	  nReject++ ;
	  if(DEBUG && i%500==0) {
	    cout << "Removed " << nReject << " entries" << endl;
	    cout << "Run " << run << "   event " << event << endl;
	  }
	}
	else {
	  nAccept++ ;
	  if(DEBUG && i%10000==0) cout << "Accepted " << nAccept << " entries" << endl;
	  lastRun  = run;
	  lastEvent= event;
	}

	// TESTING STUFF : RecHit Embed investigation
	//TCut vbf("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
	if(DEBUG) {
	  if(etaL2>0.8 && etaL2<0.9) {
	    cout << "&&&& Spike : " << run << "  " << event << "  " << embeddingWeight ;
	    if(nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1) cout << " => VBF CAT" << endl;
	    else cout << endl;
	  }
	}

      }
      if(DEBUG) cout << "-- end loop" << endl;

      // Let all branches be readable again
      tree->SetBranchStatus("*",  1); 

      // Usual Draw
      if(DEBUG) cout << "-- setEntryList again" << endl;
      tree->SetEntryList(skim); // modified skim (choice of the best pair done in the loop)

      //tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*sbinCat*genMass*passL1ETMCut*hltMatch);
      tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch);

      // Reset entry list
      tree->SetEntryList(0);
      skim->Reset();
      if(DEBUG) cout << "-- reset skim : " << skim->GetN() << "entries" << endl;
    }    
    else {
      TCut pairIndex="pairIndex<1";
      //tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*sbinCat*genMass*passL1ETMCut*hltMatch*pairIndex);
      tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch*pairIndex);
    }

    // Scale the histogram, compute norm and err
    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries()) * (normalization/h->GetEntries());

    if(DEBUG) cout << h->GetEntries() << " entries => integral=" << normalization << endl;
    if(verbose==0) h->Reset();

  }
  else{
    cout << "ERROR : drawHistogram => null pointers to tree or histogram. Exit." << endl;
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
		   //TString location    = "/data_CMS/cms/htautau/HCP12/binning/mssm_tailfit/"
		   ){

  // input txt file with bins
  ifstream is;

  TArrayF dummy(2);
  dummy[0] = xMin_; dummy[1] = xMax_;
  
  char* c = new char[10];
  string filename = Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str());
  if(MSSM)
    filename = Form(location+"/bins_muTau_%s_%s_mssm.txt",variable_.Data(), selection_.c_str());
  
  is.open(filename);
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
  //is.open(Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  is.open(filename);
  
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

void evaluateWextrapolation(mapchain mapAllTrees, TString version_, TString analysis_,
			    TString RUN,
			    string sign, bool useFakeRate, string selection_,
			    float& scaleFactorOS, 
			    float& OSWinSignalRegionDATA,   float& OSWinSignalRegionMC, 
			    float& OSWinSidebandRegionDATA, float& OSWinSidebandRegionMC, 
			    float& scaleFactorTTOS,
			    TH1F* hWMt, TString variable="",
			    float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor=0.,
			    float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
			    float MutoTauCorrectionFactor = 0., float JtoTauCorrectionFactor = 0.,
			    float antiWsdb = 0., float antiWsgn = 0., bool useMt = true,
			    TCut sbinCatForWextrapolation = "",
			    TCut sbinPZetaRel = "",  TCut sbinRelChargePZetaRel = "",
			    TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="",
			    TCut sbinPZetaRelaIsoInclusive = "", TCut sbinPZetaRelaIso = "", 
			    TCut vbf="", TCut boost="", TCut zeroJet = "", TCut sbinCat="", 
			    TCut sbinPairIso="", TCut sbinPairAiso=""){
  
  float Error = 0.; float ErrorW1 = 0.;   float ErrorW2 = 0.;
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinRelChargePZetaRel&&pZ);
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinRelChargePZetaRel&&apZ);
  }
  else{
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRelInclusive&&pZ);
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRelInclusive&&apZ);
  }
  //scaleFactorOS      = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;
  scaleFactorOS      = OSWinSidebandRegionMC>0 ? OSWinSignalRegionMC/OSWinSidebandRegionMC : 1.0 ;
  float scaleFactorOSError = scaleFactorOS*(ErrorW1/OSWinSignalRegionMC + ErrorW2/OSWinSidebandRegionMC);
  if(useMt)
    cout << "Extrap. factor for W " << sign << " : P(Mt>"     << antiWsdb << ")/P(Mt<"   << antiWsgn << ") ==> " <<OSWinSidebandRegionMC<<"/"<<OSWinSignalRegionMC<<" = "<< scaleFactorOS << " +/- " << scaleFactorOSError << endl;
  else
    cout << "Extrap. factor for W " << sign << " : P(pZeta<- "<< antiWsdb << ")/P(pZeta>"<< antiWsgn << ") ==> " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;    

  cout << OSWinSignalRegionMC << "/" << OSWinSidebandRegionMC << " = " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;

  // restore with full cut
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRel&&pZ);
  // restore with full cut
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRel&&apZ);
 
  float OSTTbarinSidebandRegionMC = 0.;
  float ErrorTT = 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"],  variable,  OSTTbarinSidebandRegionMC,     ErrorTT, scaleFactor*TTxsectionRatio , hWMt, sbinPZetaRel&&apZ);

  /* //Remove this unnecessary sideband TTbar extrapolation
  TCut bTagCut; TCut bTagCutaIso; TCut sbinCatBtag;
  if(selection_.find("novbf")!=string::npos){
    sbinCatBtag = zeroJet && TCut("nJets20BTagged>1");
    bTagCut     = sbinPZetaRelInclusive     && apZ;
    bTagCutaIso = sbinPZetaRelaIsoInclusive && apZ;
  }
  else if(selection_.find("boost")!=string::npos){
    sbinCatBtag = boost && TCut("nJets20BTagged>1");
    bTagCut     = sbinPZetaRelInclusive     && apZ;
    bTagCutaIso = sbinPZetaRelaIsoInclusive && apZ;
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    sbinCatBtag = vbf && TCut("nJets20BTagged>0");
    bTagCut     = sbinPZetaRelInclusive     && apZ;
    bTagCutaIso = sbinPZetaRelaIsoInclusive && apZ;
  }
  else{
    sbinCatBtag = sbinCat && TCut("nJets20BTagged>1");
    bTagCut     = sbinPZetaRel             &&apZ;
    bTagCutaIso = sbinPZetaRelaIso         &&apZ;
  }

  float OSTTbarinSidebandRegionBtagMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, OSTTbarinSidebandRegionBtagMC,  Error, scaleFactor*TTxsectionRatio, hWMt, bTagCut);
  float OSWinSidebandRegionBtagMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag,"MC", version_,analysis_, RUN,mapAllTrees["WJets"], variable, OSWinSidebandRegionBtagMC,      Error, scaleFactor                , hWMt, bTagCut);
  float OSOthersinSidebandRegionBtagMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag,"MC", version_,analysis_, RUN,mapAllTrees["Others"], variable, OSOthersinSidebandRegionBtagMC,Error, scaleFactor                , hWMt, bTagCut);
  float OSQCDinSidebandRegionBtag = 0.;
  drawHistogram(sbinPairAiso,sbinCatBtag,"Data_FR", version_,analysis_, RUN, mapAllTrees["Data"], variable, OSQCDinSidebandRegionBtag,       Error, 1.0                        , hWMt, bTagCutaIso);
  float OSWinSidebandRegionBtagAIsoMC = 0.;
  drawHistogram(sbinPairAiso,sbinCatBtag,"MC_FR", version_,analysis_, RUN,mapAllTrees["WJets"], variable, OSWinSidebandRegionBtagAIsoMC,  Error, scaleFactor        , hWMt, bTagCutaIso);
  float OSDatainSidebandRegionBtag = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable, OSDatainSidebandRegionBtag,  Error, 1.0 , hWMt, bTagCut);

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
  */
  if(scaleFactorTTOS<=0){
    cout << "!!! scale factor is negative... set it to 1 !!!" << endl;
    scaleFactorTTOS = 1.0;
  }
  //OSTTbarinSidebandRegionMC *= scaleFactorTTOS; // to comment
  cout << "Contribution from TTbar in " << sign << " is " << OSTTbarinSidebandRegionMC << endl;

  float OSOthersinSidebandRegionMC   = 0.;
  float ErrorVV = 0 ;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["Others"],    variable, OSOthersinSidebandRegionMC  ,ErrorVV,  scaleFactor , hWMt, sbinPZetaRel&&apZ);

  float OSDYtoTauinSidebandRegionMC  = 0.;
  float ErrorDYTauTau = 0;
  if(useZDataMC)
    drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,ErrorDYTauTau,  scaleFactor*lumiCorrFactor*ExtrapolationFactorSidebandZDataMC*ExtrapolationFactorZDataMC , hWMt, sbinPZetaRel&&apZ);
  else 
    drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,ErrorDYTauTau,  scaleFactor*lumiCorrFactor, hWMt, sbinPZetaRel&&apZ);

  float OSDYJtoTauinSidebandRegionMC = 0.;
  float ErrorDYJet = 0;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"],  variable, OSDYJtoTauinSidebandRegionMC ,ErrorDYJet, scaleFactor*lumiCorrFactor*JtoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);

  float ErrorDYMu = 0;
  float OSDYMutoTauinSidebandRegionMC = 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, OSDYMutoTauinSidebandRegionMC ,ErrorDYMu,scaleFactor*lumiCorrFactor*MutoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);

  //check this one
  float OSDYtoTauLLinSidebandRegionMC  = 0.;
  float ErrorZTTLL=0;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauLL"],  variable, OSDYtoTauLLinSidebandRegionMC ,ErrorZTTLL,  scaleFactor*lumiCorrFactor, hWMt, sbinPZetaRel&&apZ);

  float OSQCDinSidebandRegionData = 0.; float OSAIsoEventsinSidebandRegionData = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"Data_FR", version_,analysis_, RUN, mapAllTrees["Data"],  variable,        OSQCDinSidebandRegionData,   Error, 1.0         , hWMt, sbinPZetaRelaIso&&apZ);
  OSAIsoEventsinSidebandRegionData =  (OSQCDinSidebandRegionData/Error)*(OSQCDinSidebandRegionData/Error);
  float OSWinSidebandRegionAIsoMC  = 0.;float OSAIsoEventsWinSidebandRegionAIsoMC  = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"MC_FR", version_,analysis_, RUN,mapAllTrees["WJets"], variable,OSWinSidebandRegionAIsoMC,   Error, scaleFactor , hWMt, sbinPZetaRelaIso&&apZ);
  OSAIsoEventsWinSidebandRegionAIsoMC = (OSWinSidebandRegionAIsoMC/Error)*(OSWinSidebandRegionAIsoMC/Error)*scaleFactor; 

  float ErrorData=0;
  drawHistogram(sbinPairIso,sbinCat,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable, OSWinSidebandRegionDATA ,ErrorData, 1.0 , hWMt, sbinPZetaRel&&apZ);

  cout << "Selected events in "          << sign 
       << " data from high Mt sideband " << OSWinSidebandRegionDATA 
       << " +/- "                        << ErrorData << endl;

  if(selection_.find("vbfTight")!=string::npos)OSWinSidebandRegionDATA = hWMt->GetEntries();
  cout << "Selected events in " << sign << " data from high Mt sideband " << OSWinSidebandRegionDATA << endl;

  OSWinSidebandRegionDATA -= OSTTbarinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSOthersinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYtoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYJtoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYMutoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYtoTauLLinSidebandRegionMC;
  if(useFakeRate) OSWinSidebandRegionDATA -= (OSQCDinSidebandRegionData-OSWinSidebandRegionAIsoMC);

  float SquaredErrorWsdb = ErrorData*ErrorData + ErrorDYMu*ErrorDYMu + ErrorDYJet*ErrorDYJet + ErrorDYTauTau*ErrorDYTauTau + ErrorVV*ErrorVV + ErrorTT*ErrorTT + ErrorZTTLL*ErrorZTTLL;
  float ErrorWsdb = SquaredErrorWsdb>=0 ? TMath::Sqrt(SquaredErrorWsdb) : -1.0;

  OSWinSignalRegionDATA = OSWinSidebandRegionDATA * scaleFactorOS ;

  float SquaredErrorWsgn = scaleFactorOS*scaleFactorOS*ErrorWsdb*ErrorWsdb + scaleFactorOSError*scaleFactorOSError*OSWinSidebandRegionDATA*OSWinSidebandRegionDATA;
  float ErroWsgn         = SquaredErrorWsgn>=0 ? TMath::Sqrt(SquaredErrorWsgn) : -1.0;

  cout << "- expected from TTbar          " << OSTTbarinSidebandRegionMC     << " +/- " << ErrorTT       << endl;
  cout << "- expected from Others         " << OSOthersinSidebandRegionMC    << " +/- " << ErrorVV       << endl;
  cout << "- expected from DY->tautau     " << OSDYtoTauinSidebandRegionMC   << " +/- " << ErrorDYTauTau << endl;
  cout << "- expected from DY->ll, l->tau " << OSDYMutoTauinSidebandRegionMC << " +/- " << ErrorDYMu     << endl;
  cout << "- expected from DY->ll, j->tau " << OSDYJtoTauinSidebandRegionMC  << " +/- " << ErrorDYJet    << endl;
  cout << "- expected from DY->tautau, ll " << OSDYtoTauLLinSidebandRegionMC << endl;
  cout << "- expected from QCD " << OSQCDinSidebandRegionData << ", obtained from " << OSAIsoEventsinSidebandRegionData << " anti-isolated events " << endl;
  cout << "  (MC predicts " << OSWinSidebandRegionAIsoMC << " W events in the aIso region, from a total of " << OSAIsoEventsWinSidebandRegionAIsoMC << " events)" << endl;
  if(!useFakeRate) cout << " !!! QCD with fake-rate not subtracted !!!" << endl;

  cout << "W NORMALIZATION PARAMETERS"  << endl
       << " ==> OSWinSignalRegionMC     = " << OSWinSignalRegionMC     << " +/- " << ErrorW1   << endl
       << " ==> OSWinSidebandRegionMC   = " << OSWinSidebandRegionMC   << " +/- " << ErrorW2   << endl
       << " ==> OSWinSidebandRegionDATA = " << OSWinSidebandRegionDATA << " +/- " << ErrorWsdb << endl
       << " ==> OSWinSignalRegionDATA   = " << OSWinSignalRegionDATA   << " +/- " << ErroWsgn  << endl
       << " ==> scaleFactorOS           = " << scaleFactorOS           << " +/- " << scaleFactorOSError << endl;

}

void evaluateQCD(mapchain mapAllTrees, TString version_, TString analysis_,
		 TString RUN,
		 TH1F* qcdHisto, TH1F* ssHisto, bool evaluateWSS, string sign, bool useFakeRate, bool removeMtCut, string selection_, 
		 float& SSQCDinSignalRegionDATAIncl_, float& SSIsoToSSAIsoRatioQCD, float& scaleFactorTTSSIncl,
		 float& extrapFactorWSSIncl, 
		 float& SSWinSignalRegionDATAIncl, float& SSWinSignalRegionMCIncl,
		 float& SSWinSidebandRegionDATAIncl, float& SSWinSidebandRegionMCIncl,
		 TH1F* hExtrap, TString variable = "",
		 float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor = 0.,
		 float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
		 float  MutoTauCorrectionFactor=0. , float JtoTauCorrectionFactor=0.,
		 float OStoSSRatioQCD = 0.,
		 float antiWsdb=0., float antiWsgn=0., bool useMt=true,
		 TCut sbin = "",
		 TCut sbinCatForWextrapolation = "",
		 TCut sbinPZetaRel ="", TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="", 
		 TCut sbinPZetaRelaIsoInclusive="", TCut sbinPZetaRelaIso="", TCut sbinPZetaRelaIsoSideband = "", 
		 TCut vbf="", TCut boost="", TCut zeroJet="", TCut sbinCat="", TCut sbinCatForSbin="",
		 TCut sbinPairIso="", TCut sbinPairAiso="",
		 bool subtractTT=true, bool subtractVV=true){

  if(evaluateWSS)
    evaluateWextrapolation(mapAllTrees, version_,analysis_, RUN, sign, useFakeRate, selection_ , 
			   extrapFactorWSSIncl, 
			   SSWinSignalRegionDATAIncl,   SSWinSignalRegionMCIncl,
			   SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
			   scaleFactorTTSSIncl,
			   hExtrap, variable,
			   scaleFactor, TTxsectionRatio,lumiCorrFactor,
			   ExtrapolationFactorSidebandZDataMC,ExtrapolationFactorZDataMC, 
			   MutoTauCorrectionFactor, JtoTauCorrectionFactor,
			   antiWsdb, antiWsgn, useMt,
			   sbinCatForWextrapolation,
			   sbinPZetaRel, sbinPZetaRel,
			   pZ, apZ, sbinPZetaRelInclusive, 
			   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbf, boost, zeroJet, sbinCat,
			   sbinPairIso, sbinPairAiso
			   );

  float Error = 0.;
  float SSQCDinSignalRegionDATAIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable,              SSQCDinSignalRegionDATAIncl,        Error, 1.0,         hExtrap, sbin, 1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap,  1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap,  1.0);

  float SSWJetsinSidebandRegionMCIncl    = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],     variable, SSWJetsinSidebandRegionMCIncl,      Error, scaleFactor*(SSWinSidebandRegionDATAIncl/SSWinSidebandRegionMCIncl), hExtrap, sbin,1);
  if(!removeMtCut){
    hExtrap->Scale(SSWinSignalRegionDATAIncl/hExtrap->Integral());
    SSWJetsinSidebandRegionMCIncl = SSWinSignalRegionDATAIncl;
  }
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);

  float SSTTbarinSidebandRegionMCIncl    = 0.;
  if(subtractTT) {
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"],     variable, SSTTbarinSidebandRegionMCIncl,      Error, scaleFactor*TTxsectionRatio*scaleFactorTTSSIncl,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSOthersinSidebandRegionMCIncl    = 0.;
  if(subtractVV) {
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["Others"],     variable, SSOthersinSidebandRegionMCIncl,     Error, scaleFactor,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSDYMutoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, SSDYMutoTauinSidebandRegionMCIncl,  Error, lumiCorrFactor*scaleFactor*MutoTauCorrectionFactor,    hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauinSidebandRegionMCIncl = 0.;
  if(useZDataMC)
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor*ExtrapolationFactorZDataMC, hExtrap, sbin,1);
  else
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYJtoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"],  variable, SSDYJtoTauinSidebandRegionMCIncl,   Error, lumiCorrFactor*scaleFactor*JtoTauCorrectionFactor,     hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauLLinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauLL"],  variable, SSDYtoTauLLinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauJJinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"],  variable, SSDYtoTauJJinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  cout << "Selected events in inclusive " << sign << " data " << SSQCDinSignalRegionDATAIncl << endl;
  SSQCDinSignalRegionDATAIncl  -= SSWJetsinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSTTbarinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSOthersinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYMutoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYJtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauLLinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauJJinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl *= OStoSSRatioQCD;
  if(qcdHisto!=0) qcdHisto->Scale(OStoSSRatioQCD);

  cout << "- expected from WJets          " << SSWJetsinSidebandRegionMCIncl << endl;
  cout << "- expected from TTbar          " << SSTTbarinSidebandRegionMCIncl << endl;
  cout << "- expected from Others         " << SSOthersinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->tautau     " << SSDYtoTauinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->ll, l->tau " << SSDYMutoTauinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->ll, j->tau " << SSDYJtoTauinSidebandRegionMCIncl  << endl;
  cout << "- expected from DY->tautau, ll " << SSDYtoTauLLinSidebandRegionMCIncl << endl;
  cout << "- expected from DY->tautau, jj " << SSDYtoTauJJinSidebandRegionMCIncl << endl;
  cout << "QCD in inclusive SS region is estimated to be " << SSQCDinSignalRegionDATAIncl/OStoSSRatioQCD  << "*" << OStoSSRatioQCD
       << " = " <<  SSQCDinSignalRegionDATAIncl << endl;
  SSQCDinSignalRegionDATAIncl_ = SSQCDinSignalRegionDATAIncl;

  float SSQCDinSignalRegionDATAInclaIso = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable, SSQCDinSignalRegionDATAInclaIso,    Error, 1.0, hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSWinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["WJets"], variable, SSWinSignalRegionMCInclaIso,       Error,   scaleFactor*(SSWinSignalRegionDATAIncl/SSWinSignalRegionMCIncl) , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSTTbarinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, SSTTbarinSignalRegionMCInclaIso,   Error,   scaleFactor*scaleFactorTTSSIncl , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  cout << "Anti-isolated " << sign << " events inclusive = " << SSQCDinSignalRegionDATAInclaIso << ", of which we expect "
       << SSWinSignalRegionMCInclaIso << " from W+jets " << " and " << SSTTbarinSignalRegionMCInclaIso << " from TTbar" << endl;

  SSIsoToSSAIsoRatioQCD = (SSQCDinSignalRegionDATAIncl)/(SSQCDinSignalRegionDATAInclaIso-SSWinSignalRegionMCInclaIso-SSTTbarinSignalRegionMCInclaIso) ;
  cout << "The extrapolation factor Iso<0.1 / Iso>0.2 is " << SSIsoToSSAIsoRatioQCD << endl;

}


void cleanQCDHisto(mapchain mapAllTrees, TString version_, TString analysis_,
		   TString RUN = "ABCD",
		   bool removeW = true, TH1F* hCleaner = 0, TH1F* hLooseIso = 0, TString variable = "",
		   float scaleFactor = 0., float WJetsCorrectionFactor=0., float TTbarCorrectionFactor=0.,
		   float MutoTauCorrectionFactor=0., float JtoTauCorrectionFactor=0., float DYtoTauTauCorrectionFactor=0.,
		   TCut sbinSSlIso1 = "", TCut sbinCat="", TCut sbinPairIso="", TCut sbinPairAiso=""){

  if(hLooseIso==0) return;

  cout << "Cleaning QCD histo with relaxed isolation from backgrounds..." << endl;

  float Error = 0.;
  float totalEvents = hLooseIso->Integral();

  float NormalizationWinLooseRegion = 0.;
  if(removeW){
    drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["WJets"],variable,      NormalizationWinLooseRegion,     Error, scaleFactor*WJetsCorrectionFactor, hCleaner, sbinSSlIso1 ,1);
    hLooseIso->Add(hCleaner,-1.0);
  }
  float NormalizationTTbarinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable,     NormalizationTTbarinLooseRegion, Error, scaleFactor*TTbarCorrectionFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationOthersinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["Others"], variable,    NormalizationOthersinLooseRegion,Error, scaleFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYMutotauinLooseRegion= 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, NormalizationDYMutotauinLooseRegion,  Error,  scaleFactor*MutoTauCorrectionFactor,    hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYJtotauinLooseRegion= 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable,  NormalizationDYJtotauinLooseRegion,   Error,  scaleFactor*JtoTauCorrectionFactor,     hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationDYTauTauinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, NormalizationDYTauTauinLooseRegion,    Error, scaleFactor*DYtoTauTauCorrectionFactor, hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);

  float totalRemoved = NormalizationWinLooseRegion+NormalizationTTbarinLooseRegion+NormalizationOthersinLooseRegion+
    NormalizationDYMutotauinLooseRegion+NormalizationDYJtotauinLooseRegion+NormalizationDYTauTauinLooseRegion;
  cout << " ==> removed " << totalRemoved << " events from a total of " << totalEvents << " (" << totalRemoved/totalEvents*100 << " %)" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void plotMuTau( Int_t mH_           = 120,
		Int_t useEmbedding_ = 0,
		string selection_   = "inclusive",
		TString analysis_   = "",		  
		TString variable_   = "diTauVisMass",
		TString XTitle_     = "full mass",
		TString Unities_    = "GeV",
		TString outputDir   = "test",
		Int_t nBins_ = 80, Float_t xMin_=0, Float_t xMax_=400,
		Float_t magnifySgn_ = 1.0,
		Float_t hltEff_     = 1.0,
		Int_t logy_         = 0,
		Float_t maxY_       = 1.2,
		TString RUN         = "ABCD",
		TString version_    = "AntiMu1_TauIso1",
		Float_t antiWsgn    = 20,
		Float_t antiWsdb    = 70,
		//TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_p2_topup/src/Bianchi/Limits/bin/results/"
		//TString location    = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_p2_Trees/src/LLRAnalysis/Limits/bin/results/"
		//TString location    = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_5_3_10_SyncRecoEleIdJEC/src/LLRAnalysis/Limits/bin/results/"
		TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_10_git/src/LLRAnalysis/Limits/bin/results/"
		) 
{   

  cout << endl;
  cout << "@@@@@@@@@@@@@@@@@@ Category  = " << selection_     <<  endl;
  cout << "@@@@@@@@@@@@@@@@@@ Variable  = " << string(variable_.Data()) <<  endl;
  cout << endl;

  ostringstream ossiTmH("");
  ossiTmH << mH_ ;
  TString TmH_ = ossiTmH.str();

  const int nProd=3;
  const int nMasses=15;
  TString nameProd[nProd]={"GGFH","VBFH","VH"};
  int hMasses[nMasses]={90,95,100,105,110,115,120,125,130,135,140,145,150,155,160};
  TString nameMasses[nMasses];

  if(DEBUG) cout << "build masses string" << endl;
  for(int iM=0 ; iM<nMasses ; iM++) nameMasses[iM]=TString(Form("%d",hMasses[iM]));

  const int nProdWW=2;
  const int nMassesWW=11;
  TString nameProdWW[nProdWW]={"GGFHWW","VBFHWW"};
  int hMassesWW[nMassesWW]={110,115,120,125,130,135,140,145,150,155,160};
  TString nameMassesWW[nMassesWW];

  if(DEBUG) cout << "build masses string" << endl;
  for(int iM=0 ; iM<nMassesWW ; iM++) nameMassesWW[iM]=TString(Form("%d",hMassesWW[iM]));

  const int nProdS=2;
  const int nMassesS=21;
  TString nameProdS[nProdS]={"GGH","BBH"};
  int hMassesS[nMassesS]={80,90,100,110,120,130,140,160,180,200,250,300,350,400,450,500,600,700,800,900,1000};
  TString nameMassesS[nMassesS];
  if(DEBUG) cout << "build masses string" << endl;
  for(int iM=0 ; iM<nMassesS ; iM++) nameMassesS[iM]=TString(Form("%d",hMassesS[iM]));

  if(DEBUG) cout << "prepare yields file" << endl;
  ofstream out(Form(location+"/%s/yields/yieldsMuTau_mH%d_%s_%s.txt",
		    outputDir.Data(),mH_,selection_.c_str(), analysis_.Data() ),ios_base::out); 
  out.precision(5);
  int nBins = nBins_;

  if(DEBUG) cout << "createBins" << endl;
  TArrayF bins = createBins(nBins_, xMin_, xMax_, nBins, selection_, variable_);

  // LUMINOSITY //
  float Lumi, LumiABC, LumiD, LumiSoftABC;
  //
  if(RERECO) {
    LumiABC     = 889.362 + 4429 + 7152;
    LumiD       = 7318;
  }
  else {
    LumiABC     = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 ; // 2012ABC 
    LumiD       = 7274;                                         // 2012D 
  }
  //
  if(RUN=="ABC")    Lumi = LumiABC;
  else if(RUN=="D") Lumi = LumiD;
  else              Lumi = LumiABC + LumiD;
  //
  LumiSoftABC = LumiABC;            // needs modification ?
  if(version_.Contains("SoftABC"))  Lumi = LumiSoftABC;

  /////////////////

  float lumiCorrFactor                     = 1 ;    //= (1-0.056);
  //float TTxsectionRatio                    = 0.98; //lumiCorrFactor*(165.8/157.5) ;
  float TTxsectionRatio                    = 0.96; // Summer13 value from twiki
  float OStoSSRatioQCD                     = 1.06;
  float SSIsoToSSAIsoRatioQCD              = 1.0;
  float MutoTauCorrectionFactor            = 1.0;
  float JtoTauCorrectionFactor             = 1.0;
//   float MutoTauCorrectionFactor            = 0.976;
//   float JtoTauCorrectionFactor             = 0.976;
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

  string antiWcut = "MtLeg1MVA";
  bool useMt      = true;

  //string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
  //float antiWsgn  = useMt ? 20. :  20. ;
  //float antiWsdb  = useMt ? 70. :  40. ; 

  //bool use2Dcut   = false;
  //if( use2Dcut ){
  //antiWcut = "!(MtLeg1MVA<40 && (pZetaMVA-1.5*pZetaVisMVA)>-20)";
  //antiWsgn = 0.5;
  //antiWsdb = 0.5;
  //}

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

  // QCD estimations
  TH1F* hQCD                           = new TH1F( "hQCD"         ,"QCD full vbf"      , nBins , bins.GetArray());                                    hQCD        ->SetFillColor(kMagenta-10);
  TH1F* hQCDrelaxVBF                   = new TH1F( "hQCDRelaxVBF" ,"QCD relax vbf"     , nBins , bins.GetArray());                                    hQCDrelaxVBF->SetFillColor(kMagenta-10); //ND
  //
  TH1F* hDataAntiIsoLooseTauIsoFullVBF = new TH1F( "hDataAntiIsoLooseTauIsoFullVBF","data anti-iso,loose tau-iso,full vbf",  nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoFullVBF ->SetFillColor(kMagenta-10);//ND
  TH1F* hDataAntiIsoLooseTauIsoRelaxVBF= new TH1F( "hDataAntiIsoLooseTauIsoRelaxVBF","data anti-iso,loose tau-iso,relax vbf",nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoRelaxVBF->SetFillColor(kMagenta-10);//ND
  //
  TH1F* hDataAntiIsoLooseTauIso        = new TH1F( "hDataAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataAntiIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoQCD     = new TH1F( "hDataAntiIsoLooseTauIsoQCD"   ,"data anti-iso, norm QCD"            , nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoQCD->SetFillColor(kMagenta-10);

  // QCD shapes
  // SS anti-iso
  TH1F* hDataSSAntiIsoLooseTauIso= new TH1F( "hDataSSAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray());hDataSSAntiIsoLooseTauIso->SetFillColor(kMagenta-10);
  // SS Iso
  TH1F* hDataSSIsoLooseTauIso = new TH1F( "hDataSSIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataSSIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  // OS anti-iso
  TH1F* hDataOSAntiIsoLooseTauIso = new TH1F( "hDataOSAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataOSAntiIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  // OS iso
  TH1F* hDataOSIsoLooseTauIso = new TH1F( "hDataOSIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataOSIsoLooseTauIso   ->SetFillColor(kMagenta-10);

  //histograms with fine binning for MSSM
  TH1F* hDataEmb_fb  = new TH1F( "hDataEmb_fb","Embedded"          , 400, 0., 2000.); hDataEmb_fb->SetFillColor(kOrange-4); 
  TH1F* hW_fb        = new TH1F( "hW_fb"      ,"W+jets"            , 400, 0., 2000.); hW_fb->SetFillColor(kRed+2);
  TH1F* hZtt_fb      = new TH1F( "hZtt_fb"    ,"Ztautau"           , 400, 0., 2000.); hZtt_fb->SetFillColor(kOrange-4); 
  TH1F* hZmm_fb      = new TH1F( "hZmm_fb"    ,"Z+jets, mu->tau"   , 400, 0., 2000.); hZmm_fb->SetFillColor(kBlue-2);
  TH1F* hZmj_fb      = new TH1F( "hZmj_fb"    ,"Z+jets, jet to tau", 400, 0., 2000.); hZmj_fb->SetFillColor(kBlue-2);
  TH1F* hTTb_fb      = new TH1F( "hTTb_fb"    ,"ttbar"             , 400, 0., 2000.); hTTb_fb->SetFillColor(kBlue-8);
  TH1F* hQCD_fb      = new TH1F( "hQCD_fb"    ,"QCD full vbf"      , 400, 0., 2000.); hQCD_fb->SetFillColor(kMagenta-10);
  TH1F* hVV_fb       = new TH1F( "hVV_fb"     ,"Diboson"           , 400, 0., 2000.); hVV_fb->SetFillColor(kRed+2);

  TH1F* hZmm_Up      = new TH1F( "hZmm_Up"    ,"Z+jets, mu->tau"   , nBins , bins.GetArray()); hZmm_Up->SetFillColor(kBlue-2);
  TH1F* hZmm_Down    = new TH1F( "hZmm_Down"    ,"Z+jets, mu->tau"   , nBins , bins.GetArray()); hZmm_Down->SetFillColor(kBlue-2);

  TH1F* hSignal[nProd][nMasses];
  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      hSignal[iP][iM] = new TH1F("h"+nameProd[iP]+nameMasses[iM], nameProd[iP]+nameMasses[iM], nBins , bins.GetArray());
      hSignal[iP][iM]->SetLineWidth(2);
    }
  }
  //GGH Higgs pT weights up/down
  TH1F* hGGFHUp[nMasses]; TH1F* hGGFHDown[nMasses];
  for(int iM=0 ; iM<nMasses ; iM++) {
    hGGFHUp[iM] = new TH1F("hGGFH"+nameMasses[iM]+"Up", "GGFH"+nameMasses[iM]+"Up", nBins , bins.GetArray()); 
    hGGFHUp[iM]->SetLineWidth(2);
    hGGFHDown[iM] = new TH1F("hGGFH"+nameMasses[iM]+"Down", "GGFH"+nameMasses[iM]+"Down", nBins , bins.GetArray());  
    hGGFHDown[iM]->SetLineWidth(2); 
  }
  
  TH1F* hSignalWW[nProdWW][nMassesWW];
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      hSignalWW[iP][iM] = new TH1F("h"+nameProdWW[iP]+nameMassesWW[iM], nameProdWW[iP]+nameMassesWW[iM], nBins , bins.GetArray());
      hSignalWW[iP][iM]->SetLineWidth(2);
    }
  }

  TH1F* hSusy[nProdS][nMassesS];

  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++) {
      for(int iM=0 ; iM<nMassesS ; iM++) {
	hSusy[iP][iM] = new TH1F("h"+nameProdS[iP]+nameMassesS[iM], nameProdS[iP]+nameMassesS[iM], nBins , bins.GetArray());
	hSusy[iP][iM]->SetLineWidth(2);
      }
    }
  }
  
  TH1F* hParameters   = new TH1F( "hParameters", "" ,30, 0, 30);
 ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  /*
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
  */

  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

  //TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_Summer13_TES/MuTau/update/";
  TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_Summer13_TES/MuTau/update3/";
  TString pathToFileDY    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_Summer13_TES/MuTau/update4/";
  TString pathToFileHWW   = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_Summer13_TES/MuTau/updateHWW/";
  // DATA //
  TChain *data = new TChain("outTreePtOrd");
  if(RUN.Contains("ABC")) {
    data->Add(pathToFile+"/nTupleRun2012A*Data_MuTau.root");
    data->Add(pathToFile+"/nTupleRun2012B*Data_MuTau.root");
    data->Add(pathToFile+"/nTupleRun2012C*Data_MuTau.root");
  }
  if(RUN.Contains("D")) {
    data->Add(pathToFile+"/nTupleRun2012D*Data_MuTau.root");
  }

  if(!data) cout << "### DATA NTUPLE NOT FOUND ###" << endl;

  // EMBEDDED //
  TString treeEmbedded,fileAnalysisEmbedded;
  if(analysis_.Contains("TauUp") || analysis_.Contains("TauDown") ){
    treeEmbedded = "outTreePtOrd"+analysis_;
    fileAnalysisEmbedded = analysis_; 
  }
  else {
    treeEmbedded = "outTreePtOrd";
    fileAnalysisEmbedded = "nominal"; 
  }
  TChain *dataEmbedded = new TChain(treeEmbedded);
  //
  if(!version_.Contains("Soft")) {
    if(RUN.Contains("ABC")) {
      dataEmbedded->Add(pathToFile+"/nTupleRun2012A*EmbeddedPF_MuTau_"+fileAnalysisEmbedded+".root");
      dataEmbedded->Add(pathToFile+"/nTupleRun2012B*EmbeddedPF_MuTau_"+fileAnalysisEmbedded+".root");
      dataEmbedded->Add(pathToFile+"/nTupleRun2012C*EmbeddedPF_MuTau_"+fileAnalysisEmbedded+".root");
    }
    if(RUN.Contains("D")) dataEmbedded->Add(pathToFile+"/nTupleRun2012D*EmbeddedPF_MuTau_"+fileAnalysisEmbedded+".root");
  }
  else {
    dataEmbedded->Add(pathToFile+"/nTupleRun2012D*EmbeddedLowPt_MuTau_"+fileAnalysisEmbedded+".root");
  }

  //cout << "FILE EMBEDDED #1 : " << dataEmbedded->GetFile()->GetName() << endl;

  //if(!dataEmbedded) cout << "### EMBEDDED NTUPLE NOT FOUND ###" << endl;

  // BACKGROUNDS //
  TString treeMC,fileAnalysis;
  if(analysis_.Contains("TauUp") || analysis_.Contains("TauDown") || analysis_.Contains("JetUp") || analysis_.Contains("JetDown") ) {
    treeMC = "outTreePtOrd"+analysis_;
    fileAnalysis=analysis_;
  }
  else {
    treeMC = "outTreePtOrd";
    fileAnalysis="nominal";
  }
  //
  TChain *backgroundDY         = new TChain(treeMC);
  TChain *backgroundDYTauTau   = new TChain(treeMC);
  TChain *backgroundDYTauTauLL = new TChain(treeMC);
  TChain *backgroundDYTauTauJJ = new TChain(treeMC);
  TChain *backgroundDYMutoTau  = new TChain(treeMC);
  TChain *backgroundDYJtoTau   = new TChain(treeMC);
  TChain *backgroundTTbar      = new TChain(treeMC);
  TChain *backgroundOthers     = new TChain(treeMC);
  TChain *backgroundWJets      = new TChain(treeMC);
  TChain *backgroundW3Jets     = new TChain(treeMC);
  //
  backgroundDY      ->Add(pathToFileDY+"nTupleDYJets_MuTau_"+fileAnalysis+".root");
  
  if(!version_.Contains("NoDYExcl")) {
    backgroundDY      ->Add(pathToFileDY+"nTupleDYJets1Jets_MuTau_"+fileAnalysis+".root");
    backgroundDY      ->Add(pathToFileDY+"nTupleDYJets2Jets_MuTau_"+fileAnalysis+".root");
    backgroundDY      ->Add(pathToFileDY+"nTupleDYJets3Jets_MuTau_"+fileAnalysis+".root");
    backgroundDY      ->Add(pathToFileDY+"nTupleDYJets4Jets_MuTau_"+fileAnalysis+".root");
  }
  //
  backgroundTTbar   ->Add(pathToFile+"nTupleTTJets_*_MuTau_"+fileAnalysis+".root");
  //
  backgroundOthers  ->Add(pathToFile+"nTupleT-tW_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleTbar-tW_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWWJetsTo2L2Nu_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo2L2Q_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo3LNu_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Nu_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Q_MuTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo4L_MuTau_"+fileAnalysis+".root");
  //
  backgroundWJets   ->Add(pathToFile+"nTupleWJets-p1_MuTau_"     +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets-p2_MuTau_"     +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets1Jets_MuTau_"   +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets2Jets_MuTau_"   +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets3Jets_MuTau_"   +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets4Jets_MuTau_"   +fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets1JetsV19_MuTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets2JetsV19_MuTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets3JetsV19_MuTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets4JetsV19_MuTau_"+fileAnalysis+".root");

  //backgroundW3Jets  ->Add(pathToFile+"nTupleWJets3Jets_MuTau_"+fileAnalysis+".root");
  backgroundW3Jets = backgroundWJets;

  if(!backgroundDY)    cout << "###  NTUPLE DY NOT FOUND ###" << endl;  
  if(!backgroundTTbar) cout << "###  NTUPLE TT NOT FOUND ###" << endl;  
  if(!backgroundOthers)cout << "###  NTUPLE VVt NOT FOUND ###" << endl;  
  if(!backgroundWJets) cout << "###  NTUPLE W NOT FOUND ###" << endl;  
  if(!backgroundW3Jets)cout << "###  NTUPLE W3 NOT FOUND ###" << endl;  

  TChain *signal[nProd][nMasses];
  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      signal[iP][iM] = new TChain(treeMC);
      signal[iP][iM]->Add(pathToFile+"/nTuple"+nameProd[iP]+nameMasses[iM]+"_MuTau_"+fileAnalysis+".root");
      if(!signal[iP][iM])cout << "###  NTUPLE Signal " << nameProd[iP]+nameMasses[iM] << " NOT FOUND ###" << endl;  
    }
  }
  TChain *signalWW[nProdWW][nMassesWW];
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      signalWW[iP][iM] = new TChain(treeMC);
      signalWW[iP][iM]->Add(pathToFileHWW+"/nTuple"+nameProdWW[iP]+nameMassesWW[iM]+"_MuTau_"+fileAnalysis+".root");
      if(!signalWW[iP][iM])cout << "###  NTUPLE Signal " << nameProdWW[iP]+nameMassesWW[iM] << " NOT FOUND ###" << endl;
    }
  }
  TChain *signalSusy[nProdS][nMassesS];
  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++) {
      for(int iM=0 ; iM<nMassesS ; iM++) {
	signalSusy[iP][iM] = new TChain(treeMC);
	signalSusy[iP][iM]->Add(pathToFile+"/nTupleSUSY"+nameProdS[iP]+nameMassesS[iM]+"_MuTau_"+fileAnalysis+".root");
      }
    }
  }

  // Split DY into 3 sub-samples (TauTau, MuToTau, JetToTau)
  TFile *dummy1;
  if(DOSPLIT) {
    cout << "SPLIT DY SAMPLE ON THE FLY" << endl;
    //
    dummy1 = new TFile("dummy2.root","RECREATE");
    //
    cout << "Now copying g/Z -> tau+ tau- " << endl;
    backgroundDYTauTau = (TChain*)backgroundDY->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
    //
    cout << "Now copying g/Z -> mu+mu- mu->tau" << endl;
    backgroundDYMutoTau = (TChain*)backgroundDY->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
    //
    cout << "Now copying g/Z -> mu+mu- jet->tau" << endl;
    backgroundDYJtoTau  = (TChain*)backgroundDY->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau
  }
  else {
    cout << "USE DY SEPARATE SUB-SAMPLES" << endl;
    //
    if(!version_.Contains("NoDYExcl")) {
      backgroundDYTauTau  ->Add(pathToFileDY+"/nTupleDYJets*TauTau_MuTau_"  +fileAnalysis+".root");
      backgroundDYTauTauLL->Add(pathToFileDY+"/nTupleDYJets*ZTTL_MuTau_"    +fileAnalysis+".root");
      backgroundDYTauTauJJ->Add(pathToFileDY+"/nTupleDYJets*ZTTJ_MuTau_"    +fileAnalysis+".root");
      backgroundDYMutoTau ->Add(pathToFileDY+"/nTupleDYJets*MuToTau_MuTau_" +fileAnalysis+".root");
      backgroundDYJtoTau  ->Add(pathToFileDY+"/nTupleDYJets*JetToTau_MuTau_"+fileAnalysis+".root");
    }
    else {
      backgroundDYTauTau  ->Add(pathToFileDY+"/nTupleDYJets_TauTau_MuTau_"+fileAnalysis+".root");
      backgroundDYTauTauLL->Add(pathToFileDY+"/nTupleDYJets_ZTTL_MuTau_"    +fileAnalysis+".root");
      backgroundDYTauTauJJ->Add(pathToFileDY+"/nTupleDYJets_ZTTJ_MuTau_"    +fileAnalysis+".root");
      backgroundDYMutoTau ->Add(pathToFileDY+"/nTupleDYJets_MuToTau_MuTau_"+fileAnalysis+".root");
      backgroundDYJtoTau  ->Add(pathToFileDY+"/nTupleDYJets_JetToTau_MuTau_"+fileAnalysis+".root");
    }
  }

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
  cout << backgroundDYMutoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau" << endl;
  cout << backgroundDYTauTauLL->GetEntries()  << " come from DY->tautau LL"         << endl;
  cout << backgroundDYTauTauJJ->GetEntries()  << " come from DY->tautau JJ"         << endl;

  // MAPPING //
  if(VERBOSE) cout << "-- gather the trees" << endl;

  const int nVarious = 12;
  const int nChainsSM1 = nVarious  + nProd*nMasses;
  const int nChainsSM = nChainsSM1 + nProdWW*nMassesWW;
  const int nChains  = nChainsSM + nProdS*nMassesS;
  TString treeNamesVarious[nVarious]={"SS","WJets","Data","W3Jets","TTbar","Others","DYToTauTau","DYToTauTauLL","DYToTauTauJJ","DYMutoTau","DYJtoTau","Embedded"};
  TChain* chainsVarious[nVarious]   ={data,backgroundWJets,data,backgroundW3Jets,backgroundTTbar,backgroundOthers,
				      backgroundDYTauTau,backgroundDYTauTauLL,backgroundDYTauTauJJ,backgroundDYMutoTau,backgroundDYJtoTau,dataEmbedded};
  TString  treeNames[nChains];
  TChain*  chains[nChains];
  mapchain mapAllTrees;

  for(int iCh=0 ; iCh<nChains ; iCh++) {
    
    if(iCh<nVarious) { // fill various tree names and trees
      treeNames[iCh] = treeNamesVarious[iCh];
      chains[iCh]    = chainsVarious[iCh];
    }    
    else if(iCh<nChainsSM1){ // fill signal names and trees
      treeNames[iCh] = nameProd[ int((iCh-nVarious)/nMasses) ] + nameMasses[ int((iCh-nVarious)%nMasses) ];
      chains[iCh]    = signal[ int((iCh-nVarious)/nMasses) ][ int((iCh-nVarious)%nMasses) ];
    }
    else if(iCh<nChainsSM){ // fill signal names and trees
      treeNames[iCh] = nameProdWW[ int((iCh-nChainsSM1)/nMassesWW) ] + nameMassesWW[ int((iCh-nChainsSM1)%nMassesWW) ];
      chains[iCh]    = signalWW[ int((iCh-nChainsSM1)/nMassesWW) ][ int((iCh-nChainsSM1)%nMassesWW) ];
    }
    else { // fill signal names and trees
      treeNames[iCh] = "SUSY"+nameProdS[ int((iCh-nChainsSM)/nMassesS) ] + nameMassesS[ int((iCh-nChainsSM)%nMassesS) ];
      chains[iCh]    = signalSusy[ int((iCh-nChainsSM)/nMassesS) ][ int((iCh-nChainsSM)%nMassesS) ];
    }
    mapAllTrees[ treeNames[iCh] ] = chains[iCh]; // create an entry in the map
  }

  if(DEBUG) {
    cout << "######################" << endl
	 << "### LIST OF CHAINS ###" << endl;
    for(int iCh=0 ; iCh<nChains ; iCh++) cout << treeNames[iCh] << endl;
    cout << "######################" << endl;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  ///// MU PT+ID+ISO ///////
  TCut lpt("ptL1>20");
  TCut lptMax("ptL1>-999");
  TCut lID("isPFMuon && isTightMuon");

  if(     version_.Contains("SoftABC"))  {lpt="ptL1>16"; lptMax="ptL1<=20";}
  else if(version_.Contains("SoftD"))    {lpt="ptL1>9";  lptMax="ptL1<=20";}
  else if(version_.Contains("SoftLTau")) {lpt="ptL1>9";  lptMax="ptL1<=20";}
  else if(!version_.Contains("Soft"))    {lpt="ptL1>20"; lptMax="ptL1>-999";}
  if(     version_.Contains("NoMaxPt"))  {lptMax="ptL1>-999";}
  lpt = lpt && lptMax ;

  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

  ////// TAU PT+ID+ISO //////
  TCut tpt("ptL2>20");
  TCut antimu("tightestAntiMuWP>2");
  TCut tiso("tightestHPSMVAWP>=0");
  TCut ltiso("tightestHPSMVAWP>-99");
  TCut mtiso("hpsMVA>0.7");

  // Choose selection wrt version_
  chooseSelection(version_, tiso, ltiso, mtiso, antimu,liso,laiso,lliso,tpt);
  if(selection_.find("vbfRelaxMt")!=string::npos) antiWsgn+=10;

  ////// EVENT WISE //////
  TCut lveto="muFlag!=1 && vetoEventOld==0";
  /*
  if(RERECO) lveto = "muFlag!=1 && vetoEventNew==0"; // New = NewEleID
  else       lveto = "muFlag!=1 && vetoEvent==0";    // muFlag==0
  */
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  //TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));
  TCut apZ2(Form("((%s)>60 && (%s)<120)",antiWcut.c_str(),antiWcut.c_str()));

  // Check OS vs SS distributions (e.g. QCD)
  if(version_.Contains("OS")) { 
    SS=OS;
    OStoSSRatioQCD=1.0;
  }
  else if(version_.Contains("SS")) { 
    OS=SS;
    OStoSSRatioQCD=1.0;
  }
  if(version_.Contains("aIso")) liso=laiso;

  bool removeMtCut     = bool(selection_.find("NoMt")!=string::npos);
  bool invertDiTauSign = bool(selection_.find("SS")!=string::npos);
  TCut MtCut       = removeMtCut     ? "(etaL1<999)" : pZ;
  TCut diTauCharge = invertDiTauSign ? SS : OS; 

  // HLT matching //
  TCut hltevent("HLTx==1 && HLTmatch==1");
  if(     version_.Contains("SoftABC"))  hltevent = "HLTxIsoMu15ETM20==1 && HLTmatchIsoMu15ETM20==1" ;
  //else if(version_.Contains("SoftD"))    hltevent = "HLTxSoft==1         && HLTmatchSoft==1" ;
  else if(version_.Contains("SoftD"))    hltevent = "HLTxSoft==1" ;
  else if(version_.Contains("SoftLTau")) hltevent = "HLTxIsoMu8Tau20==1  && HLTmatchIsoMu8Tau20==1" ;
  else if(!version_.Contains("Soft"))    hltevent = "HLTx==1             && HLTmatch==1" ;
  else {
    cout << "ERROR : Soft analyses need RUN set to ABC or D, nothing else is allowed." << endl;
    return;
  }

  ////// CATEGORIES ///
  TCut zeroJet("nJets30<1");
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut vbfTight("nJets30>=2 && pt1>30 && pt2>30 && Mjj>700 && Deta>4.0 && isVetoInJets!=1 && diTauRecoPt>100 && nJets20BTagged<1");
  TCut vbfRelaxedTight("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");
  TCut vbfRelaxedTightQCD("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");
  TCut vbf;
  if (version_.Contains("OldCat"))  
    vbf = TCut("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
  else{
    vbf = TCut("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1 && nJets20BTagged<1");
    vbf = vbf && !vbfTight;
  }
  TCut vbfLoose("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedTightZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");

  TCut vbfLooseQCD("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2");

  if(version_.Contains("Soft")) {
    vbfLooseQCD = vbf;
    vbfLoose = vbf;
  }

  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");
  TCut boost("nJets30>0 && pt1>30 && nJets20BTagged<1");
  boost = boost && !vbf /*&& !vh*/;
  if(!version_.Contains("OldCat")) boost = boost && !vbfTight ;

  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut bTagLoose("nJets30<2 && nJets20BTaggedLoose>0"); //for W shape in b-Category
  //TCut bTagLoose("nJets30<2 && nJets20>=1");
  TCut nobTag("nJets20BTagged==0");

  TCut novbf("nJets30<1 && nJets20BTagged==0");

  // SWITCH TO OLD CATEGORIES //
  if(version_.Contains("OldCat") && !version_.Contains("OldCatVBF")) {
    if(selection_.find("High")!=string::npos) {
      boost = boost&&TCut("ptL2>40"); 
      novbf = novbf&&TCut("ptL2>40");
    }
    else if(selection_.find("Low")!=string::npos){
      boost = boost&&TCut("ptL2<40"); 
      novbf = novbf&&TCut("ptL2<40"); 
    }
  }
  else {
    if(selection_.find("High")!=string::npos) {
      boost = boost&&TCut("ptL2>45");
      novbf = novbf&&TCut("ptL2>45");
    }
    else if(selection_.find("Medium")!=string::npos){
      boost = boost&&TCut("ptL2>30 && ptL2<45");
      novbf = novbf&&TCut("ptL2>30 && ptL2<45");
    }
    else if(selection_.find("Low")!=string::npos){
      boost = boost&&TCut("ptL2<30");
      novbf = novbf&&TCut("ptL2<30");
    }
    if(selection_.find("highhiggs")!=string::npos) {
      boost = boost&&TCut("diTauRecoPt>100");
    }
    else if(selection_.find("lowhiggs")!=string::npos) {
      boost = boost&&TCut("diTauRecoPt<100");
    }
  }
  TCut sbinCatIncl("etaL1<999");
  TCut sbinCat("");
  if(     selection_.find("inclusive")!=string::npos) sbinCat = "etaL1<999";
  else if(selection_.find("oneJet")!=string::npos)    sbinCat = oneJet;
  else if(selection_.find("twoJets")!=string::npos)   sbinCat = twoJets;
  else if(selection_.find("vh")!=string::npos)        sbinCat = vh;
  else if(selection_.find("novbf")!=string::npos)     sbinCat = novbf;
  else if(selection_.find("boost")!=string::npos)     sbinCat = boost;
  else if(selection_.find("nobTag")!=string::npos)    sbinCat = nobTag;
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos && selection_.find("vbfTight")==string::npos)   sbinCat = vbf;
  else if(selection_.find("vbfTight")!=string::npos)   sbinCat = vbfTight;
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos) sbinCat = bTag;

  // Global TCuts = category && object selection && event selection //

  // TEST PURPOSES : MIMIC pairIndex cuts ONLY to compare loop vs pairIndex
  //lID=diTauCharge=MtCut=SS=lveto="run>0";
  //hltevent="HLTmatch==1";

  // difference between pairIndex and inclusive selections :
  // lID, lveto, diTauCharge, MtCut

  TCut sbinPresel           = lpt && lID && tpt && tiso  && antimu && liso  && lveto && hltevent   ;
  TCut sbinEmbeddingPresel  = lpt && lID && tpt && tiso  && antimu && liso  && lveto               ;
  TCut sbinaIsoPresel       = lpt && lID && tpt && tiso  && antimu && laiso && lveto && hltevent   ;
  TCut sbinlIsoPresel       = lpt && lID && tpt && tiso  && antimu && lliso && lveto && hltevent   ;
  TCut sbinaIsoLtisoPresel  = lpt && lID && tpt && ltiso  && antimu && laiso && lveto && hltevent  ;
  TCut sbinLtisoPresel      = lpt && lID && tpt && ltiso  && antimu && liso  && lveto && hltevent   ;

  TCut sbinInclusive                     = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinChargeRelInclusive        = lpt && lID && tpt && tiso  && antimu && liso  && lveto                 && MtCut  && hltevent   ;
  TCut sbinChargeRelLtisoInclusive   = lpt && lID && tpt && ltiso  && antimu && liso  && lveto                && MtCut  && hltevent   ;
  TCut sbinChargeRelPZetaRelInclusive = lpt && lID && tpt && tiso  && antimu && liso  && lveto                          && hltevent   ;
  TCut sbinEmbeddingInclusive            = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge  && MtCut                ;
  TCut sbinPZetaRelEmbeddingInclusive    = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge                          ;
  TCut sbinPZetaRelSSInclusive           = lpt && lID && tpt && tiso  && antimu && liso  && lveto && SS                     && hltevent   ;
  TCut sbinPZetaRelInclusive             = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge            && hltevent   ;
  TCut sbinSSInclusive                   = lpt && lID && tpt && tiso  && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoInclusive               = lpt && lID && tpt && tiso  && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinAisoInclusive                 = lpt && lID && tpt && tiso  && antimu && laiso && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelSSaIsoInclusive       = lpt && lID && tpt && tiso  && antimu && laiso && lveto && SS                     && hltevent   ;
  TCut sbinPZetaRelSSaIsoMtisoInclusive  = lpt && lID && tpt && mtiso && antimu && laiso && lveto && SS                     && hltevent   ;

  TCut sbinSSaIsoLtisoInclusive          = lpt && lID && tpt && ltiso && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoMtisoInclusive          = lpt && lID && tpt && mtiso && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinPZetaRelaIsoInclusive         = lpt && lID && tpt && tiso  && antimu && laiso && lveto && diTauCharge            && hltevent   ;

  TCut sbinSSltisoInclusive              = lpt && lID && tpt && ltiso && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinLtisoInclusive                = lpt && lID && tpt && ltiso && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinMtisoInclusive                = lpt && lID && tpt && mtiso && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelLtisoInclusive        = lpt && lID && tpt && ltiso && antimu && liso  && lveto && diTauCharge            && hltevent   ;

  TCut sbin                   = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinEmbedding          = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge  && MtCut                ;
  TCut sbinEmbeddingPZetaRel  = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge                          ;
  TCut sbinPZetaRel           = lpt && lID && tpt && tiso  && antimu && liso  && lveto && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelaIso       = lpt && lID && tpt && tiso  && antimu && laiso && lveto && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelSSaIso     = lpt && lID && tpt && tiso  && antimu && laiso && lveto && SS                     && hltevent   ;
  TCut sbinSS                 = lpt && lID && tpt && tiso  && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinPZetaRelSS         = lpt && lID && tpt && tiso  && antimu && liso  && lveto && SS                     && hltevent   ;
  TCut sbinAiso               = lpt && lID && tpt && tiso  && antimu && laiso && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinSSaIso             = lpt && lID && tpt && tiso  && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso1            = lpt && lID && tpt && tiso  && antimu && lliso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso2            = lpt && lID && tpt && mtiso && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso3            = lpt && lID && tpt && mtiso && antimu && lliso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoLtiso        = lpt && lID && tpt && ltiso && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoMtiso        = lpt && lID && tpt && mtiso && antimu && laiso && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSltiso            = lpt && lID && tpt && ltiso && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinSSmtiso            = lpt && lID && tpt && mtiso && antimu && liso  && lveto && SS           && MtCut  && hltevent   ;
  TCut sbinLtiso              = lpt && lID && tpt && ltiso && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinMtiso              = lpt && lID && tpt && mtiso && antimu && liso  && lveto && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelMtiso      = lpt && lID && tpt && mtiso && antimu && liso  && lveto && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelSSaIsoMtiso= lpt && lID && tpt && mtiso && antimu && laiso && lveto && SS                     && hltevent   ;

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

  // TEST : TO BE REMOVED
  //cout << "=================> TEST : DATA sbinEmbeddingPZetaRel" << endl;
  //float NormTest=0;
  //drawHistogram(sbinCatIncl,"Data", version_,analysis_, RUN,mapAllTrees["Data"], variable, NormTest,         Error, 1.0, hExtrap, sbinEmbeddingPZetaRel);
  //

  float ExtrapDYInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusive,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->tautau             = " << ExtrapDYInclusive << " +/- " <<  Error << endl; 

  float ExtrapDYInclusivePZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusivePZetaRel,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRelInclusive);
  cout << "All Z->tautau (pZeta Rel) = " << ExtrapDYInclusivePZetaRel << " +/- " <<  Error << endl; 

  float ExtrapLFakeInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable,ExtrapLFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->mumu, mu->tau      = " << ExtrapLFakeInclusive << " +/- " <<  Error << endl;

  float ExtrapJFakeInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable, ExtrapJFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->mumu, j->tau       = " << ExtrapJFakeInclusive << " +/- " <<  Error << endl;
  cout << endl;

  float ExtrapDYNum = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNum,                  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbin);
  float ExtrapDYNuminSidebandRegion = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNuminSidebandRegion,  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel&&apZ);
  float ExtrapDYNumPZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNumPZetaRel,          Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel);
  float ExtrapolationFactorMadGraph      = ExtrapDYNum/ExtrapDYInclusive;
  float ErrorExtrapolationFactorMadGraph = TMath::Sqrt(ExtrapolationFactorMadGraph*(1-ExtrapolationFactorMadGraph)/ExtrapDYInclusive);
  cout << "Extrap. factor using MadGraph            = " << ExtrapolationFactorMadGraph << " +/- " << ErrorExtrapolationFactorMadGraph << endl;

  float ExtrapEmbedNum = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNum,                 Error, 1.0, hExtrap, sbinEmbedding);
  float ExtrapEmbedNuminSidebandRegion = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNuminSidebandRegion, Error, 1.0, hExtrap, sbinEmbeddingPZetaRel&&apZ);

  if(DEBUG) cout << "drawHistogram Embed sbinEmbeddingPZetaRel" << endl;  
  float ExtrapEmbedNumPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNumPZetaRel,         Error, 1.0, hExtrap, sbinEmbeddingPZetaRel);

  if(DEBUG) cout << "drawHistogram Embed sbinEmbeddingInclusive" << endl;
  float ExtrapEmbedDen = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedDen,                 Error, 1.0, hExtrap, sbinEmbeddingInclusive);

  if(DEBUG) cout << "drawHistogram Embed sbinPZetaRelEmbeddingInclusive" << endl;
  float ExtrapEmbedDenPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedDenPZetaRel,         Error, 1.0, hExtrap, sbinPZetaRelEmbeddingInclusive);

  ExtrapolationFactorZ             = ExtrapEmbedDen!=0 ? ExtrapEmbedNum/ExtrapEmbedDen : 0; 
  ErrorExtrapolationFactorZ        = ExtrapEmbedDen!=0 ? TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen) : 0;
  ExtrapolationFactorZDataMC       = ExtrapolationFactorMadGraph!=0 ? ExtrapolationFactorZ/ExtrapolationFactorMadGraph : 0;
  ExtrapolationFactorZFromSideband = ExtrapEmbedDenPZetaRel!=0 ? ExtrapEmbedDen/ExtrapEmbedDenPZetaRel : 0;

  float sidebandRatioMadgraph = ExtrapDYNumPZetaRel!=0 ? ExtrapDYNuminSidebandRegion/ExtrapDYNumPZetaRel : 0;
  float sidebandRatioEmbedded = ExtrapEmbedNumPZetaRel!=0 ? ExtrapEmbedNuminSidebandRegion/ExtrapEmbedNumPZetaRel : 0;
  ExtrapolationFactorSidebandZDataMC = (sidebandRatioMadgraph > 0) ? sidebandRatioEmbedded/sidebandRatioMadgraph : 1.0;
  if(!useEmbedding_) {
    ExtrapolationFactorZ = ExtrapolationFactorZDataMC = ExtrapolationFactorZFromSideband = sidebandRatioEmbedded = ExtrapolationFactorSidebandZDataMC = 1;
  }

  cout << "Extrap. factor using embedded sample     = " << ExtrapolationFactorZ << " +/- " << ErrorExtrapolationFactorZ << endl;
  cout << " ==> data/MC (signal region)             = " << ExtrapolationFactorZDataMC << " +/- " 
       << ExtrapolationFactorZDataMC*(ErrorExtrapolationFactorMadGraph/ExtrapolationFactorMadGraph + 
				      ErrorExtrapolationFactorZ/ExtrapolationFactorZ) 
       << endl;
  cout << " ==> data/MC (sideband-to-signal)        = " << ExtrapolationFactorSidebandZDataMC << endl;
  cout << " ==> data (sideband-to-signal inclusive) = " << ExtrapolationFactorZFromSideband << endl;
  
  float extrap_ztt     = ExtrapEmbedDenPZetaRel!=0 ? ExtrapEmbedNum / ExtrapEmbedDenPZetaRel : -1.0;
  float err_extrap_ztt = ( extrap_ztt<=1 && ExtrapEmbedDenPZetaRel!=0 ) ? TMath::Sqrt(extrap_ztt*(1-extrap_ztt)/ExtrapEmbedDenPZetaRel) : -1.0;
  
  cout << " => total extrap ztt = " << extrap_ztt << " +/- " << err_extrap_ztt << endl;

  //if(DEBUG) return; // TESTING : TO REMOVE !!

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

  evaluateQCD(mapAllTrees, version_,analysis_, RUN, 0, 0, true, "SS", false, removeMtCut, "inclusive", 
	      SSQCDinSignalRegionDATAIncl , SSIsoToSSAIsoRatioQCD, scaleFactorTTSSIncl,
	      extrapFactorWSSIncl, 
	      SSWinSignalRegionDATAIncl, SSWinSignalRegionMCIncl,
	      SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
 	      hExtrap, variable,
 	      Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
	      ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
 	      MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
	      OStoSSRatioQCD,
 	      antiWsdb, antiWsgn, useMt,
	      sbinSSInclusive,
	      sbinCatIncl,
 	      sbinPZetaRelSSInclusive, pZ, apZ, sbinPZetaRelSSInclusive, 
 	      //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoMtisoInclusive, 
 	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, 
	      vbf, oneJet, zeroJet, sbinCatIncl, sbinCatIncl, sbinPresel, sbinaIsoPresel, true, true);

  delete hExtrap;

  cout << endl;
  cout << "#############################################################" << endl;
  cout << ">>>>>>>>>>> END Compute inclusive informations <<<<<<<<<<<<<<" << endl;
  cout << "#############################################################" << endl;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
 
  float SSQCDinSignalRegionDATA = 0.; 
  float extrapFactorWSS = 0.;             
  float SSWinSignalRegionDATA = 0.;       float SSWinSignalRegionMC = 0.; float SSWinSidebandRegionDATA = 0.; float SSWinSidebandRegionMC = 0.;
  float extrapFactorWOSW3Jets = 0.;   
  float OSWinSignalRegionDATAW3Jets = 0.; float OSWinSignalRegionMCW3Jets = 0.; float OSWinSidebandRegionDATAW3Jets = 0.; float OSWinSidebandRegionMCW3Jets = 0.;
  float extrapFactorWOSWJets  = 0.; 
  float OSWinSignalRegionDATAWJets  = 0.; float OSWinSignalRegionMCWJets  = 0.; float OSWinSidebandRegionDATAWJets  = 0.; float OSWinSidebandRegionMCWJets  = 0.; 
  float scaleFactorTTOSW3Jets = 0.;
  float scaleFactorTTOSWJets  = 0.;

  mapchain::iterator it;
  TString currentName, h1Name;
  //TH1F *h1, *hCleaner;
  TChain* currentTree;

  for(int iCh=0 ; iCh<nChains ; iCh++) {

    cout        << endl        << ">>>> Dealing with sample ## " ;
    currentName = treeNames[iCh];
    cout        << currentName << " ##" << endl;

    //if(!currentName.Contains("Data")) continue; // TEST : TO REMOVE !
    //if(!currentName.Contains("Embed")) continue; // TEST : TO REMOVE !

    it = mapAllTrees.find(currentName);
    if(it!=mapAllTrees.end()) currentTree = it->second;
    else {
      cout << "ERROR : sample not found in the map ! continue;" << endl;
      continue;
    }

    if(!currentTree) {
      cout << "ERROR : no such tree" << endl;
      continue;
    }

    if(!MSSM && currentName.Contains("SUSY")) continue;

    h1Name         = "h1_"+currentName;
    TH1F* h1       = new TH1F( h1Name ,"" , nBins , bins.GetArray());
    TH1F* hCleaner = new TH1F("hCleaner","",nBins , bins.GetArray());
    TH1F* hCleanerfb = new TH1F("hCleanerfb","",400, 0., 2000.); //fine binning hostogram for MSSM
    if ( !h1->GetSumw2N() )       h1->Sumw2(); 
    if ( !hCleaner->GetSumw2N() ) hCleaner->Sumw2();
    if ( !hCleanerfb->GetSumw2N() ) hCleanerfb->Sumw2();

    if(currentName.Contains("SS")){
      
      cout << "************** BEGIN QCD evaluation using SS events *******************" << endl;

      TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
      float dummy1 = 0.;      

      TCut sbinCatForWextrapolation = sbinCat;
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	sbinCatForWextrapolation = vbfLoose;
      
      // hQCD for relaxed vbf selection
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) {

	evaluateQCD(mapAllTrees, version_,analysis_, RUN, hQCDrelaxVBF, hCleaner, true, "SS", false, removeMtCut, selection_, 
		    SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
		    extrapFactorWSS, 
		    SSWinSignalRegionDATA, SSWinSignalRegionMC,
		    SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
		    hExtrapSS, variable,
		    Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
		    ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
		    MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
		    OStoSSRatioQCD,
		    antiWsdb, antiWsgn, useMt,
		    sbinSS,
		    sbinCatForWextrapolation,
		    sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
		    //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
		    sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, 
		    vbfLoose, oneJet, zeroJet, sbinCat, vbfLooseQCD, sbinPresel, sbinaIsoPresel, true, true);

	hCleaner->Reset();
      }

      // Normal hQCD
      evaluateQCD(mapAllTrees, version_,analysis_, RUN, h1, hCleaner, true, "SS", false, removeMtCut, selection_, 
		  SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
		  extrapFactorWSS, 
		  SSWinSignalRegionDATA, SSWinSignalRegionMC,
		  SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
		  hExtrapSS, variable,
		  Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
		  ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
		  MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
		  OStoSSRatioQCD,
		  antiWsdb, antiWsgn, useMt,
		  sbinSS,
		  sbinCatForWextrapolation,
		  sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
		  //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
		  sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, 
		  vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel, true, true);

      cout << "************** END QCD evaluation using SS events *******************" << endl;

      //delete hExtrapSS;
      hExtrapSS->Reset();

      hQCD->Add(h1, 1.0); //hQCD->Sumw2();
      hSS->Add(hCleaner, OStoSSRatioQCD); //hSS->Sumw2();
      hCleaner->Reset();

      //fine binning for MSSM
      if(selection_.find("nobTag")!=string::npos){
	TH1F* h1fb = new TH1F("h1fb","",400, 0., 2000.); //fine binning hostogram for MSSM 
	if ( !h1fb->GetSumw2N() ) h1fb->Sumw2();
	TH1F* hExtrapSSfb = new TH1F("hExtrapSSfb","",400, 0., 2000.);
	if ( !hExtrapSSfb->GetSumw2N() ) hExtrapSSfb->Sumw2();
	
	evaluateQCD(mapAllTrees, version_,analysis_, RUN, h1fb, hCleanerfb, true, "SS", false, removeMtCut, selection_,  
		    SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS, 
		    extrapFactorWSS,  
		    SSWinSignalRegionDATA, SSWinSignalRegionMC, 
		    SSWinSidebandRegionDATA, SSWinSidebandRegionMC, 
		    hExtrapSSfb, variable, 
		    Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor, 
		    ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC, 
		    MutoTauCorrectionFactor, JtoTauCorrectionFactor,  
		    OStoSSRatioQCD, 
		    antiWsdb, antiWsgn, useMt, 
		    sbinSS, 
		    sbinCatForWextrapolation, 
		    sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive,  
		    //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso,  
		    sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso,  
		    vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel, true, true); 

	cout << "************** END QCD evaluation using SS events *******************" << endl;
	hQCD_fb->Add(h1fb, h1->Integral()/h1fb->Integral()); 
	hCleanerfb->Reset();
	delete hExtrapSSfb;
      }
      delete hExtrapSS;
    }
    else{

      if(!currentName.Contains("Embed")){

	float Error = 0.;

	if(currentName.Contains("DYToTauTau")){
	  float NormDYToTauTau = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYToTauTau, Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbin, 1);
	  hZtt->Add(h1, ExtrapolationFactorZFromSideband);
	  float NormDYToTauTauLL = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
          hZtt->Add(hCleaner, 1.0);

	  //fine binning for MSSM
	  if(selection_.find("bTag")!=string::npos){
	    hCleanerfb->Reset(); float NormDYToTauTau_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYToTauTau_fb, Error,  Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZtt_fb->Add(hCleanerfb, ExtrapolationFactorZFromSideband);
	    hCleanerfb->Reset();
	    float NormDYToTauTauLL_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZtt_fb->Add(hCleanerfb, 1.0);
	    hZtt_fb->Scale(hZtt->Integral()/hZtt_fb->Integral());
	    hCleanerfb->Reset();
	  }
	}
	if(currentName.Contains("TTbar")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){ 
	    float NormTTjets = 0.; 
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight,"MC", version_,analysis_, RUN,currentTree, variable, NormTTjets, Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hTTb->Add(hCleaner, 1.0);
	    NormTTjets = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., h1, sbin, 1);
	    hTTb->Scale( hTTb->Integral()!=0 ? h1->Integral()/hTTb->Integral() : 1.0 ); 
	  } 
	  else{
	    float NormTTjets = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., h1, sbin, 1);
	    hTTb->Add(h1, 1.0);

	    //fine binning for MSSM 
	    if(selection_.find("bTag")!=string::npos){
	      hCleanerfb->Reset(); float NormTTjets_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormTTjets_fb,     Error,  Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleanerfb, sbin, 1);
	      hTTb_fb->Add(hCleanerfb, hTTb->Integral()/hCleanerfb->Integral());
	      hCleanerfb->Reset();
	    }
	  }
	}
	else if(currentName.Contains("W3Jets")){

	  TH1F* hExtrapW3Jets = new TH1F("hExtrapW3Jets","",nBins , bins.GetArray());
	 
	  cout << "************** BEGIN W+3jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinCatForWextrapolation = sbinCat;
	  TCut apZCut = apZ;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    sbinCatForWextrapolation = vbfLoose;
	    if(selection_.find("vbfTight")!=string::npos)
	      sbinCatForWextrapolation = vbfRelaxedTight;
	    apZCut = apZ2;
	  }
	  evaluateWextrapolation(mapAllTrees, version_,analysis_, RUN, "OS", false, selection_, 
				 extrapFactorWOSW3Jets, 
				 OSWinSignalRegionDATAW3Jets,   OSWinSignalRegionMCW3Jets,
				 OSWinSidebandRegionDATAW3Jets, OSWinSidebandRegionMCW3Jets,
				 scaleFactorTTOSW3Jets,
				 hExtrapW3Jets, variable,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 sbinCatForWextrapolation,
				 sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				 pZ, apZCut, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, oneJet, zeroJet, 
				 sbinCat, sbinPresel, sbinaIsoPresel);

	  cout << "************** END W+3jets normalization using high-Mt sideband *******************" << endl;
	  delete hExtrapW3Jets;

	  float NormW3Jets = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAW3Jets/OSWinSidebandRegionMCW3Jets);
	  else h1->Scale( h1->Integral()!=0 ? OSWinSignalRegionDATAW3Jets/h1->Integral() : 1.0 );
	  hW3Jets->Add(h1, 1.0);

	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);
	  //drawHistogram(sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinMtiso, 1);
	  hW3JetsMediumTauIso->Add(hCleaner, hW3Jets->Integral()/hCleaner->Integral());

	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtiso, 1);
	  hW3JetsLooseTauIso->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());
	  
	  if(selection_.find("vbfTight")!=string::npos)
	    drawHistogram(sbinLtisoPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelLtisoInclusive, 1);
	  else
	    drawHistogram(sbinPresel,vbfLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
	  hW3JetsMediumTauIsoRelVBF->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());
	  
	  hW3JetsMediumTauIsoRelVBFMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAW3Jets));
	  //hW3JetsMediumTauIsoRelVBF->Sumw2(); hW3JetsMediumTauIsoRelVBFMinusSS->Sumw2();
	  if(((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
	      selection_.find("twoJets")!=string::npos)){
	    if(!USESSBKG) hEWK->Add(hW3JetsMediumTauIsoRelVBF,1.0);
	    else  hEWK->Add(hW3JetsMediumTauIsoRelVBFMinusSS,1.0);
	  }
	}
	else if(currentName.Contains("WJets")){

	  TH1F* hExtrapW = new TH1F("hExtrap","",nBins , bins.GetArray());
	  
	  cout << "************** BEGIN W+jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinCatForWextrapolation = sbinCat;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	    sbinCatForWextrapolation = vbfLoose; 
	  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos) 
            sbinCatForWextrapolation = bTagLoose;

	  evaluateWextrapolation(mapAllTrees, version_,analysis_, RUN, "OS", false, selection_, 
				 extrapFactorWOSWJets, 
				 OSWinSignalRegionDATAWJets,   OSWinSignalRegionMCWJets,
				 OSWinSidebandRegionDATAWJets, OSWinSidebandRegionMCWJets,
				 scaleFactorTTOSWJets,
				 hExtrapW, variable,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 sbinCatForWextrapolation,
				 sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				 pZ, apZ, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet, 
				 sbinCat, sbinPresel, sbinaIsoPresel);
	  delete hExtrapW;

	  cout << "************** END W+jets normalization using high-Mt sideband *******************" << endl;

	  float NormSSWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormSSWJets, Error,   Lumi*hltEff_/1000., h1, sbinSS, 1);
	  if(removeMtCut) h1->Scale(SSWinSidebandRegionDATA/SSWinSidebandRegionMC);
	  else h1->Scale( h1->Integral()!=0 ? SSWinSignalRegionDATA/h1->Integral() : 1.0 );
	  hWSS->Add(h1, 1.0);

	  float NormWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	  else h1->Scale( h1->Integral()!=0 ? OSWinSignalRegionDATAWJets/h1->Integral() : 1.0 );
	  //hW->Add(h1, 1.0); //hW->Sumw2();
	  //hWMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));
	  
	  //fine binning for MSSM
	  if(selection_.find("nobTag")!=string::npos){
	    hCleanerfb->Reset(); float NormWJets_fb = 0.;
	    hW_fb->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	    hW_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());
	  }
	  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    float NormWJetsBTag = 0.;
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1); 
	    hWLooseBTag->Add(hCleaner,  h1->Integral()/hCleaner->Integral());
	    hEWK->Add(hWLooseBTag,1.0);

	    //fine binning for MSSM
	    hW_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fb = 0.;
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	    hW_fb->Add(hCleanerfb,  h1->Integral()/hCleanerfb->Integral());
	  }
	  else if(selection_.find("boostMedium")!=string::npos){
	    float NormWJets_NoOS = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
	    hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
	    hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
	  }
	  else if(selection_.find("boostHighlowhiggs")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
            drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
            hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(selection_.find("boostHighhighhiggs")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
	    
            drawHistogram(sbinLtisoPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelLtisoInclusive, 1);
            hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(selection_.find("boostHigh")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
	    
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
            hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(!((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
	       selection_.find("twoJets")!=string::npos)) {
	    hW->Reset(); hWMinusSS->Reset();
	    hW->Add(h1, 1.0);
	    hWMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));
	    if(!USESSBKG) hEWK->Add(h1,1.0);
	    else hEWK->Add(hWMinusSS,1.0);
	  }
	}
	else if(currentName.Contains("DYMutoTau")){
	  if(selection_.find("vbfTight")!=string::npos){
	    float NormDYMutoTau = 0.;
	    if(useZDataMC){
              drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    }
	    else{
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    }
	    //Take the shape from relaxed VBF cuts with jet Pt 20 GeV (update for summer)
            float NormDYMutoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale( h1->Integral()/hCleaner->Integral() );
            if(hCleaner) {
              hZmm->Add(hCleaner, 1.0); //hZmm->Sumw2();
              hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2();
              hEWK->Add(hCleaner,1.0);
            }
	    //2% Up/Down
	    NormDYMutoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC_ZLUp", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYMutoTauShape = 0.; hCleaner->Reset();
            drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC_ZLDown", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
            hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	  }
	  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){  
            float NormDYMutoTau = 0.;
	    /* //This method using embed not used anymore
	    //get Norm from Inclusive 
            float NormDYMutoTauIncl = 0.;
	    if(useZDataMC){
	      drawHistogram(sbinPresel,twoJets,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbinInclusive, 1);
	      //drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauIncl, Error,  Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    }
	    else{
	      drawHistogram(sbinPresel,twoJets,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbinInclusive, 1);
	      //drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauIncl, Error,  Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    }
	    cout<<"DYMutoTau in vbf : Ref. yield "<<NormDYMutoTau<<endl;
	    //get Eff. of Embed from 2jet sel. to Category
            float NormDYMutoTauEmbdLoose = 0.; 
	    drawHistogram(sbinEmbeddingPresel,twoJets,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, NormDYMutoTauEmbdLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1);
	    hCleaner->Reset();
	    float NormDYMutoTauEmbd = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, NormDYMutoTauEmbd,  Error, 1.0 , hCleaner,  sbinEmbedding  ,1);
	    //NormDYMutoTau = NormDYMutoTauEmbdLoose!=0 ? (NormDYMutoTauIncl * NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose) : 0.;
	    NormDYMutoTau = NormDYMutoTauEmbdLoose!=0 ? (NormDYMutoTau * NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose) : 0.;
            h1->Scale( NormDYMutoTau/h1->Integral() );
	    cout<<"DYMutoTau in vbf : Embed eff. "<<NormDYMutoTauEmbd<<"/"<<NormDYMutoTauEmbdLoose<<"="<<NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose<<" , Yield "<<NormDYMutoTau<<endl;
	    */
	    //Take yields directly from MC
	    if(useZDataMC)
              drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
            else
              drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);

            //Take the shape from relaxed VBF cuts with jet Pt 20 GeV (update for summer)
            float NormDYMutoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedZL,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale( NormDYMutoTau/hCleaner->Integral() );
	    if(hCleaner) {
	      hZmm->Add(hCleaner, 1.0); //hZmm->Sumw2(); 
	      hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2();
	      hEWK->Add(hCleaner,1.0); 
	    }
	    NormDYMutoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedZL,"MC_ZLUp", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral() );
	    NormDYMutoTauShape = 0.; hCleaner->Reset();
            drawHistogram(sbinPresel,vbfRelaxedZL,"MC_ZLDown", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
            hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral() );
          }  
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    /* //this method not used anymore, keep it for backup
	    float NormDYMutoTau = 0.; 
	    //get Norm from Inclusive  
            float NormDYMutoTauIncl = 0.;
	    if(useZDataMC){
	      drawHistogram(sbinPresel,oneJet,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbinInclusive, 1); 
	      //drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauIncl, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    }
	    else{
	      drawHistogram(sbinPresel,oneJet,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbinInclusive, 1);
	      //drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauIncl, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
		}
	    //get Eff. of Embed from onejet sel to Category
	    float NormDYMutoTauEmbdLoose = 0.;
	    drawHistogram(sbinEmbeddingPresel,oneJet,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, NormDYMutoTauEmbdLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1); 
            hCleaner->Reset(); 
            float NormDYMutoTauEmbd = 0.; 
            drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, NormDYMutoTauEmbd,Error, 1.0 , hCleaner,  sbinEmbedding  ,1); 
            //NormDYMutoTau = NormDYMutoTauEmbdLoose!=0 ? (NormDYMutoTauIncl * NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose) : 0.; 
	    NormDYMutoTau = NormDYMutoTauEmbdLoose!=0 ? (NormDYMutoTau * NormDYMutoTauEmbd/NormDYMutoTauEmbdLoose) : 0.;
	    h1->Scale( NormDYMutoTau/h1->Integral() ); 
	    */
	    //Norm with full selection
	    float NormDYMutoTau = 0.; 
            if(useZDataMC) 
              drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin,  1); 
            else  
              drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
            if(hCleaner) { 
              hZmm->Add(hCleaner, 1.0); //hZmm->Sumw2();  
              hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2(); 
              hEWK->Add(hCleaner,1.0);  
            } 
	    NormDYMutoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,bTagLoose,"MC_ZLUp", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYMutoTau = 0; hCleaner->Reset();
            drawHistogram(sbinPresel,bTagLoose,"MC_ZLDown", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
            hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());

	    //fine binning for MSSM
	    hCleanerfb->Reset();hZmm_fb->Reset(); float NormDYMutoTau_fb = 0;
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau_fb, Error, Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinInclusive, 1); 
	    hZmm_fb->Add(hCleanerfb, hZmm->Integral()/hCleanerfb->Integral());
	  }
	  else{
	    float NormDYMutoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else 
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmm->Add(h1, 1.0); //hZmm->Sumw2();
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    hEWK->Add(h1,1.0);

	    NormDYMutoTau = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC_ZLUp", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYMutoTau = 0.; hCleaner->Reset();
            drawHistogram(sbinPresel,sbinCat,"MC_ZLDown", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
            hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());

	    //fine binning for MSSM 
	    if(selection_.find("nobTag")!=string::npos){
	      hCleanerfb->Reset();hZmm_fb->Reset(); float NormDYMutoTau_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau_fb, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hZmm_fb->Add(hCleanerfb, hZmm->Integral()/hCleanerfb->Integral());
	    }
	  }
	}
	else if(currentName.Contains("DYJtoTau")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){   
            float NormDYJtoTau = 0.; 
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    NormDYJtoTau = 0.; 
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1); 
	    else
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hCleaner->Scale( hCleaner->Integral()!=0 ? h1->Integral()/hCleaner->Integral() : 1.0 );
	    if(hCleaner) {
	      hZmj->Add(hCleaner, 1.0); //hZmj->Sumw2();
	      hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2();
	      hEWK->Add(hCleaner,1.0); 
	    }
	    //Add ZTTJ component to ZJ
	    float NormDYTTJtoTau = 0; hCleaner->Reset(); 
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,backgroundDYTauTauJJ, variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    if(hCleaner) {
              hZmj->Add(hCleaner, 1.0);
              hZfakes->Add(hCleaner,1.0);
              hEWK->Add(hCleaner,1.0);
            }
	  }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    float NormDYJtoTau = 0.;  
            drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinInclusive, 1); 
            NormDYJtoTau = 0.;  
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);  
	    else
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
            hCleaner->Scale(h1->Integral()/hCleaner->Integral()); 
            hZmj->Add(hCleaner, 1.0); //hZmj->Sumw2(); 
            hZfakes->Add(hCleaner,1.0); //hZfakes->Sumw2(); 
            hEWK->Add(hCleaner,1.0);  

	    //Add ZTTJ component to ZJ
            float NormDYTTJtoTau = 0; hCleaner->Reset();
            drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,backgroundDYTauTauJJ, variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
            if(hCleaner) {
              hZmj->Add(hCleaner, 1.0);
              hZfakes->Add(hCleaner,1.0);
              hEWK->Add(hCleaner,1.0);
            }
	    
	    //fine binning for MSSM   
	    hCleanerfb->Reset();hZmj_fb->Reset(); float NormDYJtoTau_fb = 0.;  
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau_fb, Error,  Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	    hZmj_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());

	    //Add ZTTJ component to ZJ
            NormDYTTJtoTau = 0; hCleanerfb->Reset();
            drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,backgroundDYTauTauJJ, variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
            if(hCleanerfb) {
              hZmj_fb->Add(hCleanerfb, 1.0);
            }
	  } 
	  else{
	    float NormDYJtoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmj->Add(h1, 1.0); //hZmj->Sumw2();
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    hEWK->Add(h1,1.0);

	    //Add ZTTJ component to ZJ
            float NormDYTTJtoTau = 0; hCleaner->Reset();
            drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,backgroundDYTauTauJJ, variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
            if(hCleaner) {
              hZmj->Add(hCleaner, 1.0);
              hZfakes->Add(hCleaner,1.0);
              hEWK->Add(hCleaner,1.0);
            }
	    //fine binning for MSSM  
            if(selection_.find("nobTag")!=string::npos){ 
              hCleanerfb->Reset();hZmj_fb->Reset(); float NormDYJtoTau_fb = 0.; 
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau_fb, Error, Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hZmj_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());

	      //Add ZTTJ component to ZJ
	      float NormDYTTJtoTau_fb = 0; hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,backgroundDYTauTauJJ, variable, NormDYTTJtoTau_fb, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      if(hCleanerfb) {
		hZmj_fb->Add(hCleanerfb, 1.0);
	      }
	    }
	  }
	}
	else if(currentName.Contains("Others")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){    
            float NormOthers = 0.;  
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight,"MC", version_,analysis_, RUN,currentTree, variable, NormOthers, Error,     Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormOthers, Error,     Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    NormOthers = 0.; 
            drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
            hCleaner->Scale( hCleaner->Integral()!=0 ? h1->Integral()/hCleaner->Integral() : 1.0 ); 
	    if(hCleaner) {
	      hVV->Add(hCleaner, 1.0);  
	      hEWK->Add(hCleaner,1.0);  
	    }
          } 
	  else{
	    float NormOthers = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
	    hVV->Add(h1, 1.0);
	    hEWK->Add(h1,1.0);

	    //fine binning for MSSM    
            if(selection_.find("bTag")!=string::npos){ 
              hCleanerfb->Reset();hVV_fb->Reset(); float NormOthers_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormOthers_fb, Error,  Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
              hVV_fb->Add(hCleanerfb, hVV->Integral()/hCleanerfb->Integral());
	    }
	  }
	}
	else if(currentName.Contains("Data")){

	  //TFile* ftest = new TFile(Form(location+"/%s/histograms/test_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()),"RECREATE");

	  float NormData = 0.;
	  drawHistogram(sbinPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , h1, sbin, 1);
	  if(VERBOSE) cout << "DATA YIELD : " << NormData << "  +/- " << Error << endl;
	  hData->Add(h1, 1.0);
	  
	  //ftest->cd();
	  //hData->Write();
	  //h1->Write();
	  //ftest->Write();
 	  //ftest->Close();
	  //return;
	  //continue;

	  if ( !hData->GetSumw2N() )hData->Sumw2();

	  // Other QCD shapes
	  TH1F* hExtrapNadir = new TH1F("hExtrapNadir","",nBins , bins.GetArray());
	  float dummy1 = 0.;
	  TCut sbinCatNadir = sbinCat;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) sbinCatNadir = vbfLooseQCD;
	  
	  if(StudyQCD) {
	    evaluateQCD(mapAllTrees, version_,analysis_, RUN, hDataSSAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapNadir, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbinSSaIso,
			sbinCatNadir,
			sbinPZetaRelSSaIso, pZ, apZ, sbinPZetaRelSSaIsoInclusive, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, // useless arguments
			vbfLoose, oneJet, zeroJet, sbinCatNadir, sbinCatNadir, sbinaIsoPresel, sbinaIsoPresel, true, true);
	    hCleaner->Reset();
	    hExtrapNadir->Reset();

	    evaluateQCD(mapAllTrees, version_,analysis_, RUN, hDataSSIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapNadir, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbinSS,
			sbinCatNadir,
			sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, // useless
			vbfLoose, oneJet, zeroJet, sbinCatNadir, sbinCatNadir, sbinPresel, sbinaIsoPresel, true, true);
	    hCleaner->Reset();
	    hExtrapNadir->Reset();

	    evaluateQCD(mapAllTrees, version_,analysis_, RUN, hDataOSAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapNadir, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbinAiso,
			sbinCatNadir,
			sbinPZetaRelaIso, pZ, apZ, sbinPZetaRelaIsoInclusive, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, // useless
			vbfLoose, oneJet, zeroJet, sbinCatNadir, sbinCatNadir, sbinaIsoPresel, sbinaIsoPresel, true, true);
	    hCleaner->Reset();
	    hExtrapNadir->Reset();

	    evaluateQCD(mapAllTrees, version_,analysis_, RUN, hDataOSIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapNadir, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbin,
			sbinCatNadir,
			sbinPZetaRel, pZ, apZ, sbinPZetaRelInclusive, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, // useless
			vbfLoose, oneJet, zeroJet, sbinCatNadir, sbinCatNadir, sbinPresel, sbinaIsoPresel, true, true);
	    hCleaner->Reset();
	    hExtrapNadir->Reset();
	  
	    delete hExtrapNadir;
	    hCleaner->Reset();
	  }
	  
	  if(selection_.find("vbfTight")!=string::npos && selection_.find("novbf")==string::npos){ 
	    
            drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1); 
            float tmpNorm = hCleaner->Integral(); 
            hDataAntiIsoLooseTauIsoFullVBF->Add(hCleaner); //ND 
	    
            drawHistogram(sbinaIsoPresel,vbfRelaxedTightQCD,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1); 
            hDataAntiIsoLooseTauIsoRelaxVBF->Add(hCleaner); //ND 
 
            hDataAntiIsoLooseTauIso->Add(hCleaner); //, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral())); 
 
            //get efficiency of events passing QCD selection to pass the category selection  
            drawHistogram(sbinaIsoLtisoPresel,sbinCatIncl,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1); 
	    //drawHistogram(sbinaIsoPresel,sbinCatIncl,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1); //for test
            float tmpNormQCDSel = hCleaner->Integral(); 
            drawHistogram(sbinaIsoLtisoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso, 1);  
	    //drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso, 1); //for test
            float tmpNormCatSel = hCleaner->Integral(); 
            float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel; 
            //Normalize to Inclusive measured QCD times the above efficiency 
            hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral()); 
	    cout<<"QCD in vbfTight : Ref. yield "<<SSQCDinSignalRegionDATAIncl<<" eff. "<<tmpNormCatSel<<"/"<<tmpNormQCDSel<<"="<<effQCDToCatSel<<" , Total QCD "<<hDataAntiIsoLooseTauIsoQCD->Integral()<<endl;
          }
	  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){

	    drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    float tmpNorm = hCleaner->Integral();
	    hDataAntiIsoLooseTauIsoFullVBF->Add(hCleaner); //ND

	    drawHistogram(sbinaIsoPresel,vbfLooseQCD,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    hDataAntiIsoLooseTauIsoRelaxVBF->Add(hCleaner); //ND

	    hDataAntiIsoLooseTauIso->Add(hCleaner); //, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral()));

	    //get efficiency of events passing QCD selection to pass the category selection 
	    drawHistogram(sbinaIsoPresel,sbinCatIncl,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    float tmpNormQCDSel = hCleaner->Integral();
	    drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1); 
            float tmpNormCatSel = hCleaner->Integral();
	    float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
	    //Normalize to Inclusive measured QCD times the above efficiency
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("boostHighhighhiggs")!=string::npos || selection_.find("boostHighmediumhiggs")!=string::npos){

            drawHistogram(sbinaIsoLtisoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0, hCleaner, sbinSSaIsoLtisoInclusive ,1);
	    
            hDataAntiIsoLooseTauIso->Add(hCleaner); //, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral()));

            //get efficiency of events passing QCD selection to pass the category selection
            drawHistogram(sbinaIsoLtisoPresel,sbinCatIncl,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1);
            float tmpNormQCDSel = hCleaner->Integral();
            drawHistogram(sbinaIsoLtisoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0, hCleaner, sbinSSaIsoLtiso, 1);
            float tmpNormCatSel = hCleaner->Integral();
            float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
            //Normalize to Inclusive measured QCD times the above efficiency
            hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	    cout<<"QCD in 1jet_high_highhiggs : Ref. yield "<<SSQCDinSignalRegionDATAIncl<<" eff. "<<tmpNormCatSel<<"/"<<tmpNormQCDSel<<"="<<effQCDToCatSel<<" , Total QCD "<<hDataAntiIsoLooseTauIsoQCD->Integral()<<endl;
          }
	  else if(selection_.find("novbfHigh")!=string::npos || selection_.find("novbfMedium")!=string::npos || selection_.find("boost")!=string::npos){
	    drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    /* //No need to subtract anyother background contributions, summer recommendation 
	    float NormDYMutoTau = 0;
	    drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    float NormDYJtoTau = 0.; 
            drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    float NormTTjets = 0.; 
            drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTSS*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    */
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("novbfLow")!=string::npos) {
	    TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
	    float dummy1 = 0.;      
	    evaluateQCD(mapAllTrees, version_,analysis_, RUN, hDataAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapSS, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbinSS,
			sbinCat,
			sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
			//sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, 
			vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel, true, true);
	    
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    delete hExtrapSS;
	    hCleaner->Reset();
	  }

	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    drawHistogram(sbinaIsoPresel,bTagLoose,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1); 
	    hDataAntiIsoLooseTauIso->Add(hCleaner); 
	    //fine binning histogram for MSSM
	    hCleanerfb->Reset(); hQCD_fb->Reset(); float NormData_fb = 0.; 
	    drawHistogram(sbinaIsoPresel,bTagLoose,"Data", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, sbinSSaIso ,1);
	    hQCD_fb->Add(hCleanerfb);
	    /* //subtraction not required anymore
            float NormDYMutoTau = 0; 
	    if(useZDataMC)
	      drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    else
	      drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    hCleanerfb->Reset(); float NormDYMutoTau_fb = 0.;
	    drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, NormDYMutoTau_fb, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    hQCD_fb->Add(hCleanerfb, -1.0);
            float NormDYJtoTau = 0.;  
	    if(useZDataMC)
	      drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    else
	      drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    hCleanerfb->Reset(); float NormDYJtoTau_fb = 0.;
	    drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"], variable, NormDYJtoTau_fb, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    hQCD_fb->Add(hCleanerfb, -1.0);
            float NormTTjets = 0.;  
            drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
            hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    hCleanerfb->Reset(); float NormTTjets_fb = 0.;
	    drawHistogram(sbinaIsoPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, NormTTjets_fb,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    hQCD_fb->Add(hCleanerfb, -1.0); 
	    */
	    if(selection_.find("High")!=string::npos){
	      //get efficiency of events passing QCD selection to pass the category selection  
	      drawHistogram(sbinaIsoPresel,sbinCatIncl,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive,1); 
	      float tmpNormQCDSel = hCleaner->Integral(); 
	      drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1);  
	      float tmpNormCatSel = hCleaner->Integral(); 
	      float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel; 
	      //Normalize to Inclusive measured QCD times the above efficiency 
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral()); 
	    }
	    else{
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	      hQCD_fb->Scale(hQCD->Integral()/hQCD_fb->Integral());
	    }
	  }
	  else{
	    //drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtiso ,1);
	    drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD);
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }
	  //hDataAntiIsoLooseTauIsoQCD->Sumw2();
	  //hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  /* //Not necessary for the analysis at the moment
	  drawHistogram(sbinlIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso1 ,1);
	  hLooseIso1->Add(hCleaner, 1.0);
	  drawHistogram(sbinlIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso2 ,1);
	  hLooseIso2->Add(hCleaner, 1.0);
	  drawHistogram(sbinlIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso3 ,1);
	  hLooseIso3->Add(hCleaner, 1.0);
	  drawHistogram(sbinaIsoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso  ,1);
	  hAntiIso->Add(hCleaner, 1.0);
	  drawHistogram(sbinaIsoPresel,sbinCat,"Data_FR", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso ,1);
	  hAntiIsoFR->Add(hCleaner, 1.0);
	  
	  cleanQCDHisto(mapAllTrees, version_,analysis_, RUN, true,hCleaner, hLooseIso1, variable, 
			Lumi*hltEff_/1000., (SSWinSidebandRegionDATA/SSWinSidebandRegionMC), 
			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso1,sbinCat,sbinlIsoPresel);
 	  cleanQCDHisto(mapAllTrees, version_,analysis_, RUN, true,hCleaner, hLooseIso2, variable, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso2,sbinCat,sbinlIsoPresel);
 	  cleanQCDHisto(mapAllTrees, version_,analysis_, RUN, true,hCleaner, hLooseIso3, variable, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso3,sbinCat,sbinlIsoPresel);
			

	  drawHistogram(sbinPresel,vbfLoose,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSInclusive , 1);
	  hSSLooseVBF->Add(hCleaner, 1.0);
	  cleanQCDHisto(mapAllTrees, version_,analysis_, RUN, false,hCleaner, hSSLooseVBF, variable, 
 			Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
 			TTxsectionRatio*scaleFactorTTSS, MutoTauCorrectionFactor*lumiCorrFactor, 
 			JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSInclusive,vbfLoose,sbinPresel);
	  hSSLooseVBF->Scale(hSS->Integral()/hSSLooseVBF->Integral());
	  */
	} 
	else if(currentName.Contains("VBFH")  || 
		currentName.Contains("GGFH")  ||
		currentName.Contains("VH")    ||
		currentName.Contains("SUSY")){

	  float NormSign = 0.;
	  if(VERBOSE) cout << "SIGNAL " << currentName << " ENTRIES " << currentTree->GetEntries() << endl;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, sbin, 1);

	  if(currentName.Contains("VBFH"+TmH_)){
	    hSgn1->Add(h1,1.0);
	    hSgn1->Scale(magnifySgn_);
	    hSgn->Add(hSgn1,1.0);
	  }
	  else if(currentName.Contains("GGFH"+TmH_)){
	    hSgn2->Add(h1,1.0);
	    hSgn2->Scale(magnifySgn_);
	    hSgn->Add(hSgn2,1.0);
	  }
	  else  if(currentName.Contains("VH"+TmH_)){
	    hSgn3->Add(h1,1.0);
	    hSgn3->Scale(magnifySgn_);
	    hSgn->Add(hSgn3,1.0);
	  }	   
	  
	  for(int iP=0 ; iP<nProd ; iP++)
	    for(int iM=0 ; iM<nMasses ; iM++)
	      if(currentName.Contains(nameProd[iP]+nameMasses[iM]))
		hSignal[iP][iM]->Add(h1,1.0);
	  //to be used when weight is available
	  for(int iM=0 ; iM<nMasses ; iM++){
	    if(currentName.Contains("GGFH"+nameMasses[iM])){
	      hCleaner->Reset(); float NormSignUp = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MCGGFHUp", version_, analysis_,RUN,currentTree, variable, NormSignUp, Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);
	      hGGFHUp[iM]->Add(hCleaner,1.0);
	      hCleaner->Reset(); float NormSignDown = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MCGGFHDown", version_, analysis_,RUN,currentTree, variable, NormSignDown, Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);
	      hGGFHDown[iM]->Add(hCleaner,1.0);
	    }
	  }
	  
	  for(int iP=0 ; iP<nProdWW ; iP++)
            for(int iM=0 ; iM<nMassesWW ; iM++)
              if(currentName.Contains(nameProdWW[iP]+nameMassesWW[iM]))
                hSignalWW[iP][iM]->Add(h1,1.0);

	  if(MSSM) {
	    if(currentName.Contains("SUSY")){
	      //select events within 30% of Higgs mass
	      TString sampleName = currentName;
	      if(sampleName.Contains("SUSYGGH"))sampleName.ReplaceAll("SUSYGGH", "");
	      else if(sampleName.Contains("SUSYBBH"))sampleName.ReplaceAll("SUSYBBH", "");
	      float mA = atof(sampleName.Data());
	      //cout<<" SUSY mass "<<currentName<<" "<<mA<<endl; 
	      TCut HWidth(Form("genVMass > 0.7*%f && genVMass < 1.3*%f", mA, mA));  
	      //cout<<" width cut "<<HWidth<<endl; 

	      float NormSign = 0.; 
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);

	      for(int iP=0 ; iP<nProdS ; iP++)
		for(int iM=0 ; iM<nMassesS ; iM++){
		  //if(currentName.Contains(nameProdS[iP]+nameMassesS[iM]))
		  TString ProcessName("SUSY"+nameProdS[iP]+nameMassesS[iM]);
		  if(currentName==ProcessName)
		    hSusy[iP][iM]->Add(h1,1.0);
		}
	      //TH1F* histoSusy =  (mapSUSYhistos.find( currentName ))->second;
	      //histoSusy->Add(h1,1.0);
	      //histoSusy->SetLineWidth(2);
	    }
	  }
	}
      }

      else{
	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  /* //HCP method 
	    float NormEmbed = 0.;
	  TCut sbinEmbeddingLoose = sbinEmbeddingInclusive && vbfLoose;
	  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbed,  Error, 1.0 , hCleaner,  sbinEmbeddingLoose  ,1);
	  hDataEmb->Add(hCleaner, 1.0);
	  NormEmbed = 0.; 
	  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1); 
	  h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral()); 
	  hDataEmb->Scale(h1->Integral()/hDataEmb->Integral());
	  */
	  //Moriond method
	  float NormEmbed = 0.; 
          drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1); 
          h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral()); 
          hDataEmb->Add(h1, 1.0); 

	  //Add ZTTLL
	  float NormDYToTauTauLL = 0.;
          drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
          hDataEmb->Add(hCleaner, 1.0);
	}
	else{
	  float NormEmbed = 0.;
	  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1);
	  h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral());
	  hDataEmb->Add(h1, 1.0);

	  //Add ZTTLL
          float NormDYToTauTauLL = 0.;
          drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
          hDataEmb->Add(hCleaner, 1.0);
	  
	  //fine binning for MSSM
	  if(selection_.find("bTag")!=string::npos){ 
	    hCleanerfb->Reset(); float NormEmbed_fb = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbed_fb, Error, 1.0 , hCleanerfb,  sbinEmbedding  ,1);
	    //hDataEmb_fb->Add(hCleanerfb, hDataEmb->Integral()/hCleanerfb->Integral());
	    hDataEmb_fb->Add(hCleanerfb, (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband));

	    //Add ZTTLL
	    float NormDYToTauTauLL_fb = 0.; 
	    hCleanerfb->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hDataEmb_fb->Add(hCleanerfb, 1.0);
	    hDataEmb_fb->Scale(hDataEmb->Integral()/hDataEmb_fb->Integral());
	  }
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

    delete hCleaner; delete hCleanerfb;
  }
  out.close();

  //TFile* ftest2 = new TFile(Form(location+"/%s/histograms/test2_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()),"RECREATE");
  //ftest2->cd();
  //hData->Write();
  //ftest2->Write();
  //ftest2->Close();
  //return;

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

//YIELDS

  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    out<<"Yields for VBF :"<<endl;
    out<<"VBF data : hData -> "<<hData->Integral()<<endl;
    out<<"VBF Ztt : hDataEmb -> "<<hDataEmb->Integral()<<endl;
    out<<"VBF QCD : hDataAntiIsoLooseTauIsoQCD -> "<<hDataAntiIsoLooseTauIsoQCD->Integral()<<endl;
    out<<"VBF W : hW3JetsMediumTauIsoRelVBF -> "<<hW3JetsMediumTauIsoRelVBF->Integral()<<endl;
    out<<"VBF Zee j->t : hZmj -> "<<hZmj->Integral()<<endl;
    out<<"VBF Zee e->t : hZmm -> "<<hZmm->Integral()<<endl;
    out<<"VBF TTb : hTTb -> "<<hTTb->Integral()<<endl;
    out<<"VBF VV : hVV -> "<<hVV->Integral()<<endl;
  }
  else if(selection_.find("boost")!=string::npos){
    out<<"Yields for boost :"<<endl;
    out<<"boost data : hData -> "<<hData->Integral()<<endl;
    out<<"boost Ztt : hDataEmb -> "<<hDataEmb->Integral()<<endl;
    out<<"boost high QCD : hDataAntiIsoLooseTauIsoQCD -> "<<hDataAntiIsoLooseTauIsoQCD->Integral()<<endl;
    out<<"boost low QCD : hQCD -> "<<hQCD->Integral()<<endl;
    out<<"boost W : hW -> "<<hW->Integral()<<endl;
    out<<"boost Zee j->t : hZmj -> "<<hZmj->Integral()<<endl;
    out<<"boost Zee e->t : hZmm -> "<<hZmm->Integral()<<endl;
    out<<"boost TTb : hTTb -> "<<hTTb->Integral()<<endl;
    out<<"boost VV : hVV -> "<<hVV->Integral()<<endl;
  }
  else if(selection_.find("bTag")!=string::npos){
    out<<"Yields for bTag :"<<endl;
    out<<"bTag data : hData -> "<<hData->Integral()<<endl;
    out<<"bTag Ztt : hDataEmb -> "<<hDataEmb->Integral()<<endl;
    out<<"bTag high QCD : hDataAntiIsoLooseTauIso -> "<<hDataAntiIsoLooseTauIso->Integral()<<endl;
    out<<"bTag W : hW -> "<<hW->Integral()<<endl;
    out<<"bTag Zee j->t : hZmj -> "<<hZmj->Integral()<<endl;
    out<<"bTag Zee e->t : hZmm -> "<<hZmm->Integral()<<endl;
    out<<"bTag TTb : hTTb -> "<<hTTb->Integral()<<endl;
    out<<"bTag VV : hVV -> "<<hVV->Integral()<<endl;
  }
  else{
    out<<"Yields for "<<selection_<<" : "<<endl;
    out<<selection_<<" data : hData -> "<<hData->Integral()<<endl;
    out<<selection_<<" Ztt : hDataEmb -> "<<hDataEmb->Integral()<<endl;
    out<<selection_<<" high QCD : hQCD -> "<<hQCD->Integral()<<endl;
    out<<selection_<<" W : hW -> "<<hW->Integral()<<endl;
    out<<selection_<<" Zee j->t : hZmj -> "<<hZmj->Integral()<<endl;
    out<<selection_<<" Zee e->t : hZmm -> "<<hZmm->Integral()<<endl;
    out<<selection_<<" TTb : hTTb -> "<<hTTb->Integral()<<endl;
    out<<selection_<<" VV : hVV -> "<<hVV->Integral()<<endl;
  }
  out.close();

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

  c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));

  TH1F *hDataBlind, *hRatioBlind;

  // Plot blinded histogram
  if(variable_.Contains("Mass")) {

    hDataBlind  = blindHistogram(hData,  100, 160, "hDataBlind");
    hRatioBlind = blindHistogram(hRatio, 100, 160, "hRatioBlind");

    c1 = new TCanvas("c2","",5,30,650,600);
    c1->SetGrid(0,0);
    c1->SetFillStyle(4000);
    c1->SetFillColor(10);
    c1->SetTicky();
    c1->SetObjectStat(0);
    c1->SetLogy(logy_);

    pad1 = new TPad("pad2_1DEta","",0.05,0.22,0.96,0.97);
    pad2 = new TPad("pad2_2DEta","",0.05,0.02,0.96,0.20);
    
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

    hDataBlind->Draw("P");
    aStack->Draw("HISTSAME");
    hDataBlind->Draw("PSAME");
    if(logy_) hSgn->Draw("HISTSAME");
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

    hRatioBlind->Draw("P");  

    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));

  }

  // templates for fitting
  TFile* fout = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()),"RECREATE");
  fout->cd();

  hSiml->Write();
  hQCD->Write();
  hQCDrelaxVBF->Write();
  hSS->Write();
  hSSLooseVBF->Write();
  hZmm->Write();
  hZmmLoose->Write();
  hZmm_Up->Write();
  hZmm_Down->Write();
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
  hDataAntiIsoLooseTauIsoFullVBF->Write();
  hDataAntiIsoLooseTauIsoRelaxVBF->Write();
  hDataAntiIsoLooseTauIsoQCD->Write();

  if(StudyQCD) {
    hDataSSAntiIsoLooseTauIso->Write();
    hDataSSIsoLooseTauIso->Write();
    hDataOSAntiIsoLooseTauIso->Write();
    hDataOSIsoLooseTauIso->Write();
  }

  hData->Write();
  hParameters->Write();

  //fine binning histograms for MSSM
  hDataEmb_fb->Write(); hW_fb->Write(); hZtt_fb->Write(); 
  hZmm_fb->Write(); hZmj_fb->Write(); hTTb_fb->Write(); 
  hQCD_fb->Write(); hVV_fb->Write(); 

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      if(hSignal[iP][iM]) hSignal[iP][iM]->Write();
  for(int iM=0 ; iM<nMasses ; iM++){
    hGGFHUp[iM]->Write();
    hGGFHDown[iM]->Write();
  }
  for(int iP=0 ; iP<nProdWW ; iP++)
    for(int iM=0 ; iM<nMassesWW ; iM++)
      if(hSignalWW[iP][iM]) hSignalWW[iP][iM]->Write();
  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++)
      for(int iM=0 ; iM<nMassesS ; iM++)
	if(hSusy[iP][iM]) hSusy[iP][iM]->Write(); 
  }
  fout->Write();
  fout->Close();

  delete hQCD; delete hSS; delete hSSLooseVBF; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hZtt; delete hZmm_Up; delete hZmm_Down; 
  delete hW; delete hWSS; delete hWMinusSS; delete hW3Jets; delete hAntiIso; delete hAntiIsoFR;
  delete hZmmLoose; delete hZmjLoose; delete hLooseIso1; delete hLooseIso2; delete hLooseIso3;
  delete hVV; delete hSgn; delete hSgn1; delete hSgn2; delete hSgn3; delete hData; delete hParameters;
  delete hW3JetsLooseTauIso; delete hW3JetsMediumTauIso; delete hW3JetsMediumTauIsoRelVBF; delete hW3JetsMediumTauIsoRelVBFMinusSS; delete hWLooseBTag;
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoQCD; delete hDataAntiIsoLooseTauIsoFullVBF;

  if(StudyQCD) {
    delete hDataSSAntiIsoLooseTauIso;
    delete hDataSSIsoLooseTauIso;
    delete hDataOSAntiIsoLooseTauIso;
    delete hDataOSIsoLooseTauIso;
  }

  delete hDataEmb_fb; delete hZtt_fb; delete hW_fb; delete hZmm_fb; delete hZmj_fb; delete hTTb_fb;
  delete hQCD_fb; delete hVV_fb;

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      if(hSignal[iP][iM]) delete hSignal[iP][iM];
  for(int iM=0 ; iM<nMasses ; iM++){
    delete hGGFHUp[iM]; delete hGGFHDown[iM];
  }
  for(int iP=0 ; iP<nProdWW ; iP++)
    for(int iM=0 ; iM<nMassesWW ; iM++)
      if(hSignalWW[iP][iM]) delete hSignalWW[iP][iM];
  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++)
      for(int iM=0 ; iM<nMassesS ; iM++)
	if(hSusy[iP][iM]) delete hSusy[iP][iM];
  }
  delete aStack;  delete hEWK; delete hSiml; delete hDataEmb;  delete hRatio; delete line;
  delete fout;

  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      //signal[iP][iM]->Close();
      delete signal[iP][iM];
    }
  }
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      delete signalWW[iP][iM];
    }
  }
  for(int iP=0 ; iP<nProdS ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      delete signalSusy[iP][iM];
    }
  }
  
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

  int mH, nBins, logy, useEmb; 
  float magnify, hltEff, xMin, xMax, maxY;
  float antiWsgn, antiWsdb;
  string category, analysis, variable, xtitle, unity, outputDir, RUN, version;

  if(argc==1) plotMuTauAll();
  else if(argc==20) { 

    mH         =  (int)atof(argv[1]); 
    category   =  argv[2]; 
    variable   =  argv[3]; 
    xtitle     =  argv[4]; 
    unity      =  argv[5]; 
    nBins      =  (int)atof(argv[6]); 
    xMin       =  atof(argv[7]); 
    xMax       =  atof(argv[8]); 
    magnify    =  atof(argv[9]); 
    hltEff     =  atof(argv[10]); 
    logy       =  (int)atof(argv[11]); 
    maxY       =  atof(argv[12]) ;
    outputDir  =  argv[13]; 
    RUN        =  argv[14] ;
    version    =  argv[15];
    useEmb     =  (int)atof(argv[16]);
    antiWsgn   =  atof(argv[17]);
    antiWsdb   =  atof(argv[18]);
    analysis   =  argv[19]; 

    cout << endl << "RUN : " << RUN << " | VERSION : " << version << " | ANALYSIS : " << analysis << endl << endl;

    plotMuTau(mH,useEmb,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY, RUN, version,antiWsgn,antiWsdb);
  }
  else { cout << "Please put 20 arguments" << endl; return 1;}

  cout << "DONE" << endl;
  return 0;
}

