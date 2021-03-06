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
#define MSSM             true
#define VERBOSE          true
#define DEBUG            false
#define LOOP             true
#define USESSBKG         false
#define scaleByBinWidth  false
#define DOSPLIT          false
#define useZDataMC       false
#define useWMC           false
#define StudyQCD         false
#define StudyQCDOlivier  false
#define includeWG        true
#define FastMode         false
#define DisplayWeights   true

typedef map<TString, TChain* >  mapchain;

///////////////////////////////////////////////////////////////////////////////////////////////
void EvaluateZTTEmb(float Emb_inclusive, float ZTT_MC_inclusive, float TTbar_MC_Emb_inclusive, TH1F* h_ZTT_cat, TH1F* h_TTbar_MC_Emb_cat)
{
  float Weight_From_ZTT_MC = (ZTT_MC_inclusive)/(Emb_inclusive-TTbar_MC_Emb_inclusive) ;
  h_ZTT_cat->Add(h_TTbar_MC_Emb_cat,-1.);
  h_ZTT_cat->Scale(Weight_From_ZTT_MC);
  
  return ;
}

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
void chooseSelection(TString variable_,
		     TString version_,
		     string selection_, 
		     TCut& tiso, 
		     TCut& tdecaymode, 
		     TCut& ltiso,
		     TCut& mtiso, 
		     TCut& antimu, 
		     TCut& antiele, 
		     TCut& liso,
		     TCut& laiso,
		     TCut& lliso,
		     TCut& selection,
		     TCut& tpt,
		     TCut& lpt
		     )
{
  cout<<"VERSION in chooseSelection :"<< version_<<endl;

  if(version_.Contains("SVfitMassCut")) selection = selection && "diTauNSVfitMass>50.";

  if(version_.Contains("tauptbin")) 
    tpt="ptL2>45 && ptL2<50";
  else if(version_.Contains("taupt20")){
    if(version_.Contains("taupt2030"))
      tpt="ptL2>20 && ptL2<30";
    else
      tpt="ptL2>20";
  }
  else if(version_.Contains("taupt30")){
    if(version_.Contains("taupt3045"))
      tpt="ptL2>30 && ptL2<45";
    else
      tpt="ptL2>30";
  }
  else if(version_.Contains("taupt45")){
    if(version_.Contains("taupt4560"))
      tpt="ptL2>45 && ptL2<60";
    else
      tpt="ptL2>45";
  }
  else if(version_.Contains("taupt60")){
    if(version_.Contains("taupt6090"))
      tpt="ptL2>60 && ptL2<90";
    else
      tpt="ptL2>60";
  }
  else if(version_.Contains("taupt90"))
    tpt="ptL2>90";
  else if(selection_.find("novbfLow")!=string::npos)
    tpt="ptL2>20";
  else                            
    tpt="ptL2>30";      
  
  if(version_.Contains("mupt30")) lpt="ptL1>30 ";
  //     lpt="ptL1>30 && abs(etaL1)<2.1 && abs(etaL2)<2.1";      
          
  if(version_.Contains("2bTagged")) 
    selection =" nJets20BTagged>1";

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
  if(version_.Contains("AntiMu1Loose"))         antimu = "tightestAntiMuWP>0";
  else if(version_.Contains("AntiMu1Medium"))   antimu = "tightestAntiMuWP>1";
  else if(version_.Contains("AntiMu1Tight"))    antimu = "tightestAntiMuWP>2";
  else if(version_.Contains("AntiMu2Loose"))    antimu = "tightestAntiMu2WP>0";
  else if(version_.Contains("AntiMu2Medium"))   antimu = "tightestAntiMu2WP>1";
  else if(version_.Contains("AntiMu2Tight"))    antimu = "tightestAntiMu2WP>2";
  else if(version_.Contains("AntiMu3Loose"))    antimu = "tightestAntiMu3WP>0";
  else if(version_.Contains("AntiMu3Tight"))    antimu = "tightestAntiMu3WP>1";
  else if(version_.Contains("AntiMuMVALoose"))  antimu = "tightestAntiMuMVAWP>0";
  else if(version_.Contains("AntiMuMVAMedium")) antimu = "tightestAntiMuMVAWP>1";
  else if(version_.Contains("AntiMuMVATight"))  antimu = "tightestAntiMuMVAWP>2";

  // Anti-Ele discriminator //
  if(version_.Contains("AntiEleLoose"))        antiele = "tightestAntiECutWP > 0";
  else if(version_.Contains("AntiEleMedium"))  antiele = "tightestAntiECutWP > 1";
  else if(version_.Contains("AntiEleTight"))   antiele = "tightestAntiECutWP > 2";
  else if(version_.Contains("AntiEle5VLoose")) antiele = "tightestAntiEMVA5WP > 0";
  else if(version_.Contains("AntiEle5Loose"))  antiele = "tightestAntiEMVA5WP > 1";
  else if(version_.Contains("AntiEle5Medium")) antiele = "tightestAntiEMVA5WP > 2";
  else if(version_.Contains("AntiEle5Tight"))  antiele = "tightestAntiEMVA5WP > 3";
  else if(version_.Contains("AntiEle5VTight")) antiele = "tightestAntiEMVA5WP > 4";

  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos) antiele = "tightestAntiEMVA5WP>1";

  //TauID
  if(version_.Contains("TauOldDM")) tdecaymode = "decayModeFindingOldDM>0.5";
  //if(version_.Contains("TauOldDM")) tdecaymode = "decayModeFindingOldDM>0";
  else if(version_.Contains("TauNewDM")) tdecaymode = "decayModeFindingNewDM>0";

  // TauIso
  // TauIso DB3Hits cut-based //
  if(version_.Contains("HPSDB3H")) {
    tiso   = "hpsDB3H<1.5" ;
    ltiso  = "hpsDB3H<10.0" ;
    mtiso  = "hpsDB3H<5.0" ;
  }
  if(version_.Contains("HPSDB3H") && version_.Contains("RelaxIso")) {
    tiso   = "hpsDB3H<10.0" ;
    ltiso  = "hpsDB3H<10.0" ;
    mtiso  = "hpsDB3H<10.0" ;
  }
  //   else if(version_.Contains("HPSMVA3newDMwLT")) {
  //     tiso   = "tightestHPSMVA3newDMwLTWP>2" ;//Tight 3
  //     ltiso   = "tightestHPSMVA3newDMwLTWP>0" ;//Loose 1
  //     mtiso   = "tightestHPSMVA3newDMwLTWP>1" ;//Medium 2
  //   }
  //   else if(version_.Contains("HPSMVA3newDMwoLT")) {
  //     tiso   = "tightestHPSMVA3newDMwoLTWP>2" ;//Tight 3
  //     ltiso   = "tightestHPSMVA3newDMwoLTWP>0" ;//Loose 1
  //     mtiso   = "tightestHPSMVA3newDMwoLTWP>1" ;//Medium 2
  //   }
  else if(version_.Contains("HPSMVA3oldDMwLTLoose")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>-1" ;//Loose 1
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  else if(version_.Contains("HPSMVA3oldDMwLTMedium")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Loose 1
//     ltiso   = "tightestHPSMVA3oldDMwLTWP>-1" ;//Loose 1
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  else if(version_.Contains("HPSMVA3oldDMwLTTight")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>2" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Loose 1
//     ltiso   = "tightestHPSMVA3oldDMwLTWP>-1" ;//Loose 1
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  else if(version_.Contains("HPSMVA3oldDMwLTVTight")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>3" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Loose 1
//     ltiso   = "tightestHPSMVA3oldDMwLTWP>-1" ;//Loose 1
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  //   else if(version_.Contains("HPSMVA3oldDMwoLT")) {
  //     tiso   = "tightestHPSMVA3oldDMwoLTWP>2" ;//Tight 3
  //     ltiso   = "tightestHPSMVA3oldDMwoLTWP>0" ;//Loose 1
  //     mtiso   = "tightestHPSMVA3oldDMwoLTWP>1" ;//Medium 2
  //   }

  if(variable_.Contains("hpsMVA3oldDMwLT")) {
    tiso   = "etaL1<999" ;//Tight 3
    ltiso   = "etaL1<999" ;//Loose 1
    mtiso   = "etaL1<999" ;//Medium 2
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

  /*
    cout<<"sbinPair = "<<sbinPair<<endl;
    cout<<"sbinCat = "<<sbinCat<<endl;
    cout<<"type = "<<type<<endl;
    cout<<"version_ = "<<version_<<endl;
    cout<<"analysis_ = "<<analysis_<<endl;
    cout<<"RUN = "<<RUN<<endl;
    cout << "cut = ";
    cut.Print();
  */

  //  if(DEBUG) 
  //     cout << "Start drawHistogram : " << type << " " << version_ << " " << RUN << endl;

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
    
    TCut TrueGenTauMatching("1");

    // Reweighting
    TCut weight         = "run>0";
    TCut sampleWeight   = "run>0";
    TCut weightDY = "run>0";
    TCut weightW  = "run>0";
    TCut weightEmb= "run>0";

    if(type.Contains("MC")) {

      sampleWeight = "sampleWeight";
      if(version_.Contains("NoDYExcl"))    weightDY = "sampleWeightDY*TauSpinnerWeight";
      else                                 weightDY = "weightHepNupDY*TauSpinnerWeight";
      if(version_.Contains("NoWHighStat")) weightW  = "weightHepNup";
      else                                 weightW  = "weightHepNupHighStatW";
      
      if(MSSM && !(version_.Contains("NoWCorr"))){
	if(type.Contains("WJetUp")) weightW  *= "weightTauFakeWJetUp";
        else if(type.Contains("WJetDown")) weightW  *= "weightTauFakeWJetDown";
        else if(type.Contains("WJet")) weightW  *= "weightTauFakeWJet";
      }
      //       cout<<"weightW : "<<weightW<<endl;
      

      if(     version_.Contains("SoftABC"))  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABCShift*SFTau*SFMu_ABC*ZmmWeight*weightDecayMode)";
      else if(version_.Contains("SoftD"))    weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*ZmmWeight*weightDecayMode)";
      else if(version_.Contains("SoftLTau")) weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*ZmmWeight*weightDecayMode)";
      else if(!version_.Contains("Soft")) {
	if(     RUN=="ABC")                  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABC*SFTau*SFMu_ABC*ZmmWeight*weightDecayMode)";
	else if(RUN=="D")                    weight = "(puWeightD*HLTweightTauD*HLTweightMuD*SFTau*SFMu_D*ZmmWeight*weightDecayMode)";
	else                                 weight = "(puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*ZmmWeight*weightDecayMode)";
      }      

      // DecayModeNoCorr
      if(version_.Contains("DecayModeNoCorr"))
	{
	  cout<<"DECAY MODE NO CORR !!!"<<endl;
	  if(     version_.Contains("SoftABC"))  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABCShift*SFTau*SFMu_ABC*ZmmWeight)";
	  else if(version_.Contains("SoftD"))    weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*ZmmWeight)";
	  else if(version_.Contains("SoftLTau")) weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTMuSoft*SFTau*SFMu_D*ZmmWeight)";
	  else if(!version_.Contains("Soft")) {
	    if(     RUN=="ABC")                  weight = "(puWeightHCP*HLTweightTauABC*HLTweightMuABC*SFTau*SFMu_ABC*ZmmWeight)";
	    else if(RUN=="D")                    weight = "(puWeightD*HLTweightTauD*HLTweightMuD*SFTau*SFMu_D*ZmmWeight)";
	    else                                 weight = "(puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*ZmmWeight)";
	  }      
	}

      if(version_.Contains("NoPU")) {
	weight = "(HLTweightTau*HLTweightMu*SFTau*SFMu*weightDecayMode*ZmmWeight)";
      }
      //to be used when weight is available
      if(!type.Contains("SUSY"))
	{
	  if(type.Contains("GGFHUp")) 
	    weight *= "HqTWeightUp"; 
	  else if(type.Contains("GGFHDown"))  
	    weight *= "HqTWeightDown";
	  else if(type.Contains("GGFH"))
	    weight *= "HqTWeight";

	  if(type.Contains("MCSMEffTUp"))
	    {
	      weight *= "weightForHighPtTauEfficiencySMSignalsUp";
	    }
	  else if(type.Contains("MCSMEffTDown"))
	    {
	      weight *= "weightForHighPtTauEfficiencySMSignalsDown";
	    }
	  
	}
      else
	{
	  /*
	  //Old HqT*HIGLU
	  if(type.Contains("GGHUp")) 
	  weight *= "mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp"; 
	  else if(type.Contains("GGHDown"))  
	  weight *= "mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown";
	  else
	  weight *= "mssmHiggsPtReweightGluGlu_mhmax";
	  */
	  if(type.Contains("GGHUp"))             weight *= "mssmHiggsPtReweightGluGluPOWHEG_mhmodplus_tanBetaUp"; 
	  else if(type.Contains("GGHDown"))      weight *= "mssmHiggsPtReweightGluGluPOWHEG_mhmodplus_tanBetaDown";
	  else if(type.Contains("GGHScaleUp"))   weight *= "mssmHiggsPtReweightGluGluPOWHEG_mhmodplus_ScaleUp";
	  else if(type.Contains("GGHScaleDown")) weight *= "mssmHiggsPtReweightGluGluPOWHEG_mhmodplus_ScaleDown";
	  else                                   weight *= "mssmHiggsPtReweightGluGluPOWHEG_mhmodplus";

	  if(type.Contains("MCSUSYEffTUp"))        weight *= "weightForHighPtTauEfficiencyMSSMSignalsUp";
	  else if(type.Contains("MCSUSYEffTDown")) weight *= "weightForHighPtTauEfficiencyMSSMSignalsDown";

	}
      
    }
    else if(type.Contains("Embed")) {
      genMass     = "genDiTauMass>50 && HLTxMu17Mu8>0.5"; // HLTxMu17Mu8
      genMass = genMass && TrueGenTauMatching;
      weightEmb   = "embeddingWeight";

      if(version_.Contains("Soft")) {
	weightEmb = "embeddingFilterEffWeight*TauSpinnerWeight*ZmumuEffWeight";
	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTMuABCShift*SFTau*SFMuID_ABC*ZmmWeight*weightDecayMode)";
	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*ZmmWeight*weightDecayMode)";
	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*ZmmWeight*weightDecayMode)";
      }
      else if(!version_.Contains("Soft")) {
	if(RUN=="ABC")                       weight = "(HLTTauABC*HLTMuABC*SFTau*SFMuID_ABC*ZmmWeight*weightDecayMode)";
	else if(RUN=="D")                    weight = "(HLTTauD*HLTMuD*SFTau*SFMuID_D*ZmmWeight*weightDecayMode)";
	else                                 weight = "(HLTTau*HLTMu*SFTau*SFMuID*ZmmWeight*weightDecayMode)";
      }

      if(version_.Contains("DecayModeNoCorr"))
	{
	  if(version_.Contains("Soft")) {
	    weightEmb = "embeddingFilterEffWeight*TauSpinnerWeight*ZmumuEffWeight*ZmmWeight";
	    if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTMuABCShift*SFTau*SFMuID_ABC*ZmmWeight)";
	    else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*ZmmWeight)";
	    else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTMuSoft*SFTau*SFMuID_D*ZmmWeight)";
	  }
	  else if(!version_.Contains("Soft")) {
	    if(RUN=="ABC")                       weight = "(HLTTauABC*HLTMuABC*SFTau*SFMuID_ABC*ZmmWeight)";
	    else if(RUN=="D")                    weight = "(HLTTauD*HLTMuD*SFTau*SFMuID_D*ZmmWeight)";
	    else                                 weight = "(HLTTau*HLTMu*SFTau*SFMuID*ZmmWeight)";
	  }
	}
    }
    
    if(type.Contains("TTJetsUp")) 
      weight *= "topPtWeightUp"; 
    else if(type.Contains("TTJetsDown")) 
      weight *= "topPtWeightDown"; 
    else if(type.Contains("TTJets")) 
      weight *= "topPtWeightNom"; 


    if(type.Contains("TTJetsEmb"))
      {
	weight *= "embeddingWeight";
	genMass = genMass && TrueGenTauMatching;
      }

    if(type.Contains("TTJetsTauFakeUp") && !type.Contains("Emb")) weight *= "weightTauFakeTTbarMCUp";
    else if(type.Contains("TTJetsTauFakeDown") && !type.Contains("Emb")) weight *= "weightTauFakeTTbarMCDown";
    else if(type.Contains("TTJets") && !type.Contains("Emb")) weight *= "weightTauFakeTTbarMC";
    
    
    if(!version_.Contains("NoQCDCorr") && type.Contains("QCDCorr"))
      {
	if(type.Contains("QCDCorrUp")) weight *= "weightJetFakeQCD*weightJetFakeQCD" ;	
	else if(type.Contains("QCDCorrDown")) weight *= "1." ;
	else weight *= "weightJetFakeQCD" ;
      }

    if(!version_.Contains("NoLooseTightWCorr") && type.Contains("LooseTightWCorr")) weight *= "weightJetFakeW" ;

    //     if(type.Contains("SUSY")) cout<<"weight : "<<weight<<endl;
    //if(DisplayWeights) cout<<"weight : "<<weight<<endl;
    
    // Loop over entries to choose the event's pair instead of using pairIndex
    if(LOOP) {

      if(DEBUG) cout << "-- produce skim" << endl;
      //tree->Draw(">>+skim", sbinPair && genMass,"entrylist");
      //tree->Draw(">>+skim", sbinPair,"entrylist");

      tree->Draw(">>+skim", sbinPair,"entrylist");
      //      cout<<"hello = "<<sbinPair<<endl;
      //       tree->Draw(">>+skim", sbinPair,"entrylist",100);


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
      //       tree->SetBranchStatus("etaL2",  1); tree->SetBranchAddress("etaL2",  &etaL2 );
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
      TCut test = cut*weight*sampleWeight*weightDY*weightW*genMass*sbinCat ;

      //tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*sbinCat*genMass*passL1ETMCut*hltMatch);
      tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch);
      if(DisplayWeights)
	{
	  TCut test2 = cut*weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch;
	  cout<<"FULL CUT = "<<TString(test2)<<endl;
	}
      //cout<<"variable = "<<variable<<endl;
      //cout<<"cut = "<<cut<<endl;
      //cout<<"weight = "<<weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch<<endl;

      // Reset entry list
      tree->SetEntryList(0);
      skim->Reset();
      if(DEBUG) cout << "-- reset skim : " << skim->GetN() << "entries" << endl;
    }// if LOOP    
    else {
      TCut pairIndex="pairIndex<1";
      //tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*sbinCat*genMass*passL1ETMCut*hltMatch*pairIndex);
      tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*weightEmb*sbinCat*genMass*passL1ETMCut*hltMatch*pairIndex);
    }

    // Scale the histogram, compute norm and err
    h->Scale(scaleFactor);

    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries()) * (normalization/h->GetEntries());

    //cout << h->GetEntries() << " entries => integral=" << normalization << endl;
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
			    TString WJetType,  TString RUN,
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
    cout<<"-> doing OSWinSignalRegionMC"<<endl;//Opposite sign W in signal region Monte Carlo W+jets
    TCut cut_local0 = sbinPZetaRelInclusive&&pZ ;
    drawHistogram(sbinPairIso,sbinCat,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, cut_local0);//evaluate W(SR)|MC -- new implementation of background estimation
    //     drawHistogram(sbinPairIso,sbinCatForWextrapolation,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRelInclusive&&pZ);//evaluate W(SR)|MC
    
    //Opposite sign W in sideband region Monte Carlo W+jets 
    cout<<"-> doing OSWinSidebandRegionMC"<<endl;
    TCut cut_local = sbinPZetaRelInclusive&&apZ ;
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, cut_local);//evaluate W(SB)|MC -- new implementation of background estimation
  }
  //scaleFactorOS      = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;
  scaleFactorOS      = OSWinSidebandRegionMC>0 ? OSWinSignalRegionMC/OSWinSidebandRegionMC : 1.0 ;//extrapolation factor
  float scaleFactorOSError = scaleFactorOS*(ErrorW1/OSWinSignalRegionMC + ErrorW2/OSWinSidebandRegionMC);//error on the extrapolation factor
  if(useMt)
    cout << "Extrap. factor for W " << sign << " : P(Mt>"     << antiWsdb << ")/P(Mt<"   << antiWsgn  << ") ==> " <<OSWinSignalRegionMC<<"/"<<OSWinSidebandRegionMC <<" = "<< scaleFactorOS << " +/- " << scaleFactorOSError << endl;
  else
    cout << "Extrap. factor for W " << sign << " : P(pZeta<- "<< antiWsdb << ")/P(pZeta>"<< antiWsgn << ") ==> " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;    

  //   cout << OSWinSignalRegionMC << "/" << OSWinSidebandRegionMC << " = " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;

  //Do the extrapolation factor without relaxing the cuts on the W
  drawHistogram(sbinPairIso,sbinCat,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRel&&pZ);
  drawHistogram(sbinPairIso,sbinCat,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRel&&apZ);
 
  float OSTTbarinSidebandRegionMC = 0.;
  float ErrorTT = 0.;
  //TTbar in the HighMt W sidebdand
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MCTTJets", version_,analysis_, RUN,mapAllTrees["TTbar"],  variable,  OSTTbarinSidebandRegionMC,     ErrorTT, scaleFactor*TTxsectionRatio , hWMt, sbinPZetaRel&&apZ);

  //Diboson in the sideband
  float OSOthersinSidebandRegionMC   = 0.;
  float ErrorVV = 0 ;
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["Others"],    variable, OSOthersinSidebandRegionMC  ,ErrorVV,  scaleFactor , hWMt, sbinPZetaRel&&apZ);

  //DY in the sideband
  float OSDYtoTauinSidebandRegionMC  = 0.;
  float ErrorDYTauTau = 0;
  if(useZDataMC)
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,ErrorDYTauTau,  scaleFactor*lumiCorrFactor*ExtrapolationFactorSidebandZDataMC*ExtrapolationFactorZDataMC , hWMt, sbinPZetaRel&&apZ);
  else 
    drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,ErrorDYTauTau,  scaleFactor*lumiCorrFactor, hWMt, sbinPZetaRel&&apZ);

  //DY->tau tau, jet to tau
  float OSDYJtoTauinSidebandRegionMC = 0.;
  float ErrorDYJet = 0;
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"],  variable, OSDYJtoTauinSidebandRegionMC ,ErrorDYJet, scaleFactor*lumiCorrFactor*JtoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);

  //DY->tau tau, mu to tau
  float ErrorDYMu = 0;
  float OSDYMutoTauinSidebandRegionMC = 0.;
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, OSDYMutoTauinSidebandRegionMC ,ErrorDYMu,scaleFactor*lumiCorrFactor*MutoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);

  //DY->tautau, tau to lepton, jet to tau
  float OSDYtoTauLLinSidebandRegionMC  = 0.;
  float ErrorZTTLL=0;
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauLL"],  variable, OSDYtoTauLLinSidebandRegionMC ,ErrorZTTLL,  scaleFactor*lumiCorrFactor, hWMt, sbinPZetaRel&&apZ);

  //QCD in W sideband region: data sideband anti iso
  float OSQCDinSidebandRegionData = 0.; float OSAIsoEventsinSidebandRegionData = 0.;
  drawHistogram(sbinPairAiso,sbinCatForWextrapolation,"Data_FR", version_,analysis_, RUN, mapAllTrees["Data"],  variable,        OSQCDinSidebandRegionData,   Error, 1.0         , hWMt, sbinPZetaRelaIso&&apZ);

//   cout<<"********//////*****////"<<endl;
//   cout<<"********//////*****////"<<endl;
//   cout<<"********//////*****////"<<endl;
//   cout<<"sbinPairAiso = "<<sbinPairAiso<<endl;
//   cout<<"sbinCatForWextrapolation = "<<sbinCatForWextrapolation<<endl;
//   cout<<"sbinPZetaRelaIso = "<<sbinPZetaRelaIso<<endl;
//   cout<<"apZ = "<<apZ<<endl;
//   cout<<"Error for QCD ====> "<<Error<<endl;
//   cout<<"OSQCDinSidebandRegionData = "<<OSQCDinSidebandRegionData<<endl;
//   cout<<"********//////*****////"<<endl;
//   cout<<"********//////*****////"<<endl;
//   cout<<"********//////*****////"<<endl;

  OSAIsoEventsinSidebandRegionData =  (OSQCDinSidebandRegionData/Error)*(OSQCDinSidebandRegionData/Error);
  //W in sideband anti iso (to subtract from data to get QCD)
  float OSWinSidebandRegionAIsoMC  = 0.;float OSAIsoEventsWinSidebandRegionAIsoMC  = 0.;
  drawHistogram(sbinPairAiso,sbinCatForWextrapolation,"MC_FR", version_,analysis_, RUN,mapAllTrees["WJets"], variable,OSWinSidebandRegionAIsoMC,   Error, scaleFactor , hWMt, sbinPZetaRelaIso&&apZ);
  OSAIsoEventsWinSidebandRegionAIsoMC = (OSWinSidebandRegionAIsoMC/Error)*(OSWinSidebandRegionAIsoMC/Error)*scaleFactor; 

  //Data sideband
  float ErrorData=0;
  drawHistogram(sbinPairIso,sbinCatForWextrapolation,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable, OSWinSidebandRegionDATA ,ErrorData, 1.0 , hWMt, sbinPZetaRel&&apZ, 1);

  cout << "Selected events in "          << sign 
       << " data from high Mt sideband " << OSWinSidebandRegionDATA 
       << " +/- "                        << ErrorData << endl;

  if(selection_.find("vbfTight")!=string::npos)OSWinSidebandRegionDATA = hWMt->GetEntries();
  cout << "Selected events in " << sign << " data from high Mt sideband " << OSWinSidebandRegionDATA << endl;

  //Subtract all the components except W from data...
  OSWinSidebandRegionDATA -= OSTTbarinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSOthersinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYtoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYJtoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYMutoTauinSidebandRegionMC;
  OSWinSidebandRegionDATA -= OSDYtoTauLLinSidebandRegionMC;
  if(useFakeRate) OSWinSidebandRegionDATA -= (OSQCDinSidebandRegionData-OSWinSidebandRegionAIsoMC);

  float SquaredErrorWsdb = ErrorData*ErrorData + ErrorDYMu*ErrorDYMu + ErrorDYJet*ErrorDYJet + ErrorDYTauTau*ErrorDYTauTau + ErrorVV*ErrorVV + ErrorTT*ErrorTT + ErrorZTTLL*ErrorZTTLL;
  float ErrorWsdb = SquaredErrorWsdb>=0 ? TMath::Sqrt(SquaredErrorWsdb) : -1.0;

  //W+jets from data = (Sideband from data)* SF
  OSWinSignalRegionDATA = OSWinSidebandRegionDATA * scaleFactorOS ;

  float SquaredErrorWsgn = scaleFactorOS*scaleFactorOS*ErrorWsdb*ErrorWsdb + scaleFactorOSError*scaleFactorOSError*OSWinSidebandRegionDATA*OSWinSidebandRegionDATA;
  float ErroWsgn         = SquaredErrorWsgn>=0 ? TMath::Sqrt(SquaredErrorWsgn) : -1.0;

  cout << "- expected from TTbar          " << OSTTbarinSidebandRegionMC     << " +/- " << ErrorTT       << endl;
  cout << "- expected from Others         " << OSOthersinSidebandRegionMC    << " +/- " << ErrorVV       << endl;
  cout << "- expected from DY->tautau     " << OSDYtoTauinSidebandRegionMC   << " +/- " << ErrorDYTauTau << endl;
  cout << "- expected from DY->ll, l->tau " << OSDYMutoTauinSidebandRegionMC << " +/- " << ErrorDYMu     << endl;
  cout << "- expected from DY->ll, j->tau " << OSDYJtoTauinSidebandRegionMC  << " +/- " << ErrorDYJet    << endl;
  cout << "- expected from DY->tautau, ll " << OSDYtoTauLLinSidebandRegionMC << " +/- " << ErrorZTTLL    << endl;
  cout << "- expected from QCD " << OSQCDinSidebandRegionData << ", obtained from " << OSAIsoEventsinSidebandRegionData << " anti-isolated events " << endl;
  cout << "  (MC predicts " << OSWinSidebandRegionAIsoMC << " W events in the aIso region, from a total of " << OSAIsoEventsWinSidebandRegionAIsoMC << " events)" << endl;
  if(!useFakeRate) cout << " !!! QCD with fake-rate not subtracted !!!" << endl;

  cout << "W NORMALIZATION PARAMETERS"  << endl
       << " ==> OSWinSignalRegionMC     = " << OSWinSignalRegionMC     << " +/- " << ErrorW1   << endl
       << " ==> OSWinSidebandRegionMC   = " << OSWinSidebandRegionMC   << " +/- " << ErrorW2   << endl
       << " ==> OSWinSidebandRegionDATA = " << OSWinSidebandRegionDATA << " +/- " << ErrorWsdb << endl
       << " ==> OSWinSignalRegionDATA   = " << OSWinSignalRegionDATA   << " +/- " << ErroWsgn  << endl
       << " ==> scaleFactorOS           = " << scaleFactorOS           << " +/- " << scaleFactorOSError << endl;

  if(useWMC){
    OSWinSignalRegionDATA = OSWinSignalRegionMC;
    cout << "useWMC : OSWinSignalRegionDATA"  <<OSWinSignalRegionDATA<< endl;

  }
}

void evaluateQCD(mapchain mapAllTrees, TString version_, TString analysis_,
		 TString WJetType, TString RUN,
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
    evaluateWextrapolation(mapAllTrees, version_,analysis_, WJetType, RUN, sign, useFakeRate, selection_ , 
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

  //Data SS in signal region
  float Error = 0.;
  float SSQCDinSignalRegionDATAIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable,              SSQCDinSignalRegionDATAIncl,        Error, 1.0,         hExtrap, sbin, 1);
//   cout<<"********* HERE IS STANDARD MEASUREMENT OF QCD IN SS *********"<<endl;
//   cout<<"      sbinPairIso = "<<sbinPairIso<<endl;
//   cout<<"      sbinCatForSbin = "<<sbinCatForSbin<<endl;
//   cout<<"      sbin = "<<sbin<<endl;
//   cout<<"********* END *********"<<endl;
  if(qcdHisto!=0) qcdHisto->Add(hExtrap,  1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap,  1.0);

  //W+jets SS -- data driven
  float SSWJetsinSidebandRegionMCIncl    = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, WJetType,version_,analysis_, RUN,mapAllTrees["WJets"],     variable, SSWJetsinSidebandRegionMCIncl,      Error, scaleFactor*(SSWinSidebandRegionDATAIncl/SSWinSidebandRegionMCIncl), hExtrap, sbin,1);
  if(!removeMtCut){
    hExtrap->Scale(SSWinSignalRegionDATAIncl/hExtrap->Integral());
    SSWJetsinSidebandRegionMCIncl = SSWinSignalRegionDATAIncl;
  }
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);

  //TTbar SS MC
  float SSTTbarinSidebandRegionMCIncl    = 0.;
  if(subtractTT) {
    drawHistogram(sbinPairIso,sbinCatForSbin,"MCTTJets", version_,analysis_, RUN,mapAllTrees["TTbar"],     variable, SSTTbarinSidebandRegionMCIncl,      Error, scaleFactor*TTxsectionRatio*scaleFactorTTSSIncl,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  //Diboson SS MC
  float SSOthersinSidebandRegionMCIncl    = 0.;
  if(subtractVV) {
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["Others"],     variable, SSOthersinSidebandRegionMCIncl,     Error, scaleFactor,       hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  //DY SS MC Mu To Tau
  float SSDYMutoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYMutoTau"], variable, SSDYMutoTauinSidebandRegionMCIncl,  Error, lumiCorrFactor*scaleFactor*MutoTauCorrectionFactor,    hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  //DY SS MC Tau Tau
  float SSDYtoTauinSidebandRegionMCIncl = 0.;
  if(useZDataMC)
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor*ExtrapolationFactorZDataMC, hExtrap, sbin,1);
  else
    drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  //DY SS MC Jet To Tau
  float SSDYJtoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYJtoTau"],  variable, SSDYJtoTauinSidebandRegionMCIncl,   Error, lumiCorrFactor*scaleFactor*JtoTauCorrectionFactor,     hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  //DY SS MC Jet To Tau, Tau To Lepton
  float SSDYtoTauLLinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauLL"],  variable, SSDYtoTauLLinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  //DY SS MC, Jet To Tau, Tau To Jet
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

  //All non-QCD contributions subtracted from data, taken in the SR (OS, iso) --> dump
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

  //Data SS anti-iso
  float SSQCDinSignalRegionDATAInclaIso = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable, SSQCDinSignalRegionDATAInclaIso,    Error, 1.0, hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  //W+jets MC SS anti-iso
  float SSWinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat,WJetType, version_,analysis_, RUN,mapAllTrees["WJets"], variable, SSWinSignalRegionMCInclaIso,       Error,   scaleFactor*(SSWinSignalRegionDATAIncl/SSWinSignalRegionMCIncl) , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  //TTbar MC SS anti-iso
  float SSTTbarinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat,"MCTTJets", version_,analysis_, RUN,mapAllTrees["TTbar"], variable, SSTTbarinSignalRegionMCInclaIso,   Error,   scaleFactor*scaleFactorTTSSIncl , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  cout << "Anti-isolated " << sign << " events inclusive = " << SSQCDinSignalRegionDATAInclaIso << ", of which we expect "
       << SSWinSignalRegionMCInclaIso << " from W+jets " << " and " << SSTTbarinSignalRegionMCInclaIso << " from TTbar" << endl;

  //extrapolation factor from SS iso to SS anti-iso
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
  drawHistogram(sbinPairIso,sbinCat,"MCTTJets", version_,analysis_, RUN,mapAllTrees["TTbar"], variable,     NormalizationTTbarinLooseRegion, Error, scaleFactor*TTbarCorrectionFactor, hCleaner, sbinSSlIso1,1);
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
		// 		Float_t antiWsgn    = 20,
		// 		Float_t antiWsdb    = 70,
		TString location    = ""
		// 		TString location    = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_11_p6_NewTauID/src/LLRAnalysis/Limits/bin/results/"
		) 
{   

  location = gSystem->pwd();
  location += "/results/";

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
  ofstream out(Form(location+"/%s/yields/yieldsMuTau_mH%d_%s_%s_%s.txt",
		    outputDir.Data(),mH_,selection_.c_str(), analysis_.Data(),variable_.Data() ),ios_base::out); 
  out.precision(5);
  int nBins = nBins_;

  if(DEBUG) cout << "createBins" << endl;
  TArrayF bins = createBins(nBins_, xMin_, xMax_, nBins, selection_, variable_);
  if(selection_.find("ZmmSel")!=string::npos)
    bins = createBins(nBins_, xMin_, xMax_, nBins, "inclusive", variable_);
  cout<<"Bins : "<<endl;
  for(int i=0 ; i<bins.GetSize() ; i++)cout<<"bin "<<i<<"   "<<bins[i]<<endl;
  
  // LUMINOSITY //
  float Lumi, LumiABC, LumiD, LumiSoftABC;
  //
  if(RERECO) {
    LumiABC     = 889.362 + 4429 + 7152;
    LumiD       = 7368; // 7318;
  }
  else {
    LumiABC     = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 ; // 2012ABC 
    LumiD       = 7274;                                         // 2012D 
  }
  //
  if(RUN=="ABC")    Lumi = LumiABC;
  else if(RUN=="D") Lumi = LumiD;
  else              Lumi = 19712.0;
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

  //   string antiWcut = "MtLeg1MVA";
  bool useMt      = true;

  string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
  //   float antiWsgn  = useMt ? 20. :  20. ;
  float antiWsgn  = useMt ? 30. :  20. ; // mTcut at 30
  if(selection_.find("ZmmSel")!=string::npos) antiWsgn  = useMt ? 40. :  20. ; //mt cut at 40 for ZeeSF

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
  //   if(variable=="dxyErrTau")c1->SetLogx(1);

  TPad* pad1 = new TPad("pad1DEta","",0.05,0.22,0.96,0.97);
  TPad* pad2 = new TPad("pad2DEta","",0.05,0.02,0.96,0.20);
 
  pad1->SetFillColor(0);
  pad2->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  pad1->SetLogy(logy_);
  //   if(variable=="dxyErrTau")pad1->SetLogx(1);
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

  TLegend* leg = new TLegend(0.53,0.48,0.85,0.88,NULL,"brNDC");
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
  TH1F* hDataEmb_1Prong0Pi0  = new TH1F( "hDataEmb_1Prong0Pi0","Embedded"          , nBins , bins.GetArray());         hDataEmb_1Prong0Pi0->SetFillColor(kOrange-4);
  TH1F* hDataEmb_1Prong1Pi0  = new TH1F( "hDataEmb_1Prong1Pi0","Embedded"          , nBins , bins.GetArray());         hDataEmb_1Prong1Pi0->SetFillColor(kOrange-4);
  TH1F* hDataEmb_3Prong  = new TH1F( "hDataEmb_3Prong","Embedded"          , nBins , bins.GetArray());         hDataEmb_3Prong->SetFillColor(kOrange-4);
  TH1F* hDataEmb_NotSubtracted = new TH1F( "hDataEmb_NotSubtracted","Embedded no TTbarEmb subtracted", nBins ,bins.GetArray());hDataEmb_NotSubtracted->SetFillColor(kOrange-4);
  TH1F* hDataEmb_NotSubtracted_1Prong0Pi0 = new TH1F( "hDataEmb_NotSubtracted_1Prong0Pi0","Embedded no TTbarEmb subtracted_1Prong0Pi0", nBins ,bins.GetArray());hDataEmb_NotSubtracted_1Prong0Pi0->SetFillColor(kOrange-4);
  TH1F* hDataEmb_NotSubtracted_1Prong1Pi0 = new TH1F( "hDataEmb_NotSubtracted_1Prong1Pi0","Embedded no TTbarEmb subtracted_1Prong1Pi0", nBins ,bins.GetArray());hDataEmb_NotSubtracted_1Prong1Pi0->SetFillColor(kOrange-4);
  TH1F* hDataEmb_NotSubtracted_3Prong = new TH1F( "hDataEmb_NotSubtracted_3Prong","Embedded no TTbarEmb subtracted_3Prong", nBins ,bins.GetArray());hDataEmb_NotSubtracted_3Prong->SetFillColor(kOrange-4);
  //   TH1F* hDataEmb2  = new TH1F( "hDataEmb2","Embedded2"       , nBins , bins.GetArray());         hDataEmb2->SetFillColor(kOrange-4);
  TH1F* hW        = new TH1F( "hW"      ,"W+jets"            , nBins , bins.GetArray());         hW->SetFillColor(kRed+2);
  TH1F* hWTight        = new TH1F( "hWTight"      ,"W+jets"            , nBins , bins.GetArray());         hWTight->SetFillColor(kRed+2);
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
  TH1F* hTTbUp    = new TH1F( "hTTbUp"    ,"ttbarUp"         , nBins , bins.GetArray());         hTTbUp->SetFillColor(kBlue-8); 
  TH1F* hTTbDown  = new TH1F( "hTTbDown"    ,"ttbarDown"     , nBins , bins.GetArray());         hTTbDown->SetFillColor(kBlue-8); 
  TH1F* hTTbTauFakeUp    = new TH1F( "hTTbTauFakeUp"    ,"ttbarTauFakeUp"         , nBins , bins.GetArray());         hTTbTauFakeUp->SetFillColor(kBlue-8); 
  TH1F* hTTbTauFakeDown  = new TH1F( "hTTbTauFakeDown"    ,"ttbarTauFakeDown"     , nBins , bins.GetArray());         hTTbTauFakeDown->SetFillColor(kBlue-8); 
  TH1F* hTTbEmb   = new TH1F( "hTTbEmb" ,"ttbarEmbedded"     , nBins , bins.GetArray());         hTTbEmb->SetLineWidth(2); hTTbEmb->SetLineColor(kMagenta);hTTbEmb->SetFillStyle(3004);
  TH1F* hTTbEmb_1Prong0Pi0   = new TH1F( "hTTbEmb_1Prong0Pi0" ,"ttbarEmbedded_1Prong0Pi0"     , nBins , bins.GetArray());         hTTbEmb_1Prong0Pi0->SetLineWidth(2); hTTbEmb_1Prong0Pi0->SetLineColor(kMagenta);hTTbEmb_1Prong0Pi0->SetFillStyle(3004);
  TH1F* hTTbEmb_1Prong1Pi0   = new TH1F( "hTTbEmb_1Prong1Pi0" ,"ttbarEmbedded_1Prong1Pi0"     , nBins , bins.GetArray());         hTTbEmb_1Prong1Pi0->SetLineWidth(2); hTTbEmb_1Prong1Pi0->SetLineColor(kMagenta);hTTbEmb_1Prong1Pi0->SetFillStyle(3004);
  TH1F* hTTbEmb_3Prong   = new TH1F( "hTTbEmb_3Prong" ,"ttbarEmbedded_3Prong"     , nBins , bins.GetArray());         hTTbEmb_3Prong->SetLineWidth(2); hTTbEmb_3Prong->SetLineColor(kMagenta);hTTbEmb_3Prong->SetFillStyle(3004);
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
  TH1F* hWLooseBTagTight                       = new TH1F( "hWLooseBTagTight" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());

  // QCD estimations
  TH1F* hQCD                           = new TH1F( "hQCD"         ,"QCD full vbf"      , nBins , bins.GetArray());                                    hQCD        ->SetFillColor(kMagenta-10);
  TH1F* hQCDrelaxVBF                   = new TH1F( "hQCDRelaxVBF" ,"QCD relax vbf"     , nBins , bins.GetArray());                                    hQCDrelaxVBF->SetFillColor(kMagenta-10); //ND
  //
  TH1F* hDataAntiIsoLooseTauIsoFullVBF = new TH1F( "hDataAntiIsoLooseTauIsoFullVBF","data anti-iso,loose tau-iso,full vbf",  nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoFullVBF ->SetFillColor(kMagenta-10);//ND
  TH1F* hDataAntiIsoLooseTauIsoRelaxVBF= new TH1F( "hDataAntiIsoLooseTauIsoRelaxVBF","data anti-iso,loose tau-iso,relax vbf",nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoRelaxVBF->SetFillColor(kMagenta-10);//ND
  //
  TH1F* hDataAntiIsoLooseTauIso        = new TH1F( "hDataAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataAntiIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoCorrUp        = new TH1F( "hDataAntiIsoLooseTauIsoCorrUp"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoCorrUp   ->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoCorrDown        = new TH1F( "hDataAntiIsoLooseTauIsoCorrDown"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoCorrDown   ->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoQCD     = new TH1F( "hDataAntiIsoLooseTauIsoQCD"   ,"data anti-iso, norm QCD"            , nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoQCD->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoQCDCorrUp     = new TH1F( "hDataAntiIsoLooseTauIsoQCDCorrUp"   ,"data anti-iso, norm QCD"            , nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoQCDCorrUp->SetFillColor(kMagenta-10);
  TH1F* hDataAntiIsoLooseTauIsoQCDCorrDown     = new TH1F( "hDataAntiIsoLooseTauIsoQCDCorrDown"   ,"data anti-iso, norm QCD"            , nBins, bins.GetArray()); hDataAntiIsoLooseTauIsoQCDCorrDown->SetFillColor(kMagenta-10);

  // QCD shapes
  //Olivier
  // SS tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_TightTauIsoMVA = new TH1F("hQCD_SS_ReverseMuIso_TightTauIsoMVA","hQCD_SS_ReverseMuIso_TightTauIsoMVA",nBins,bins.GetArray());
  // SS loose non-tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA = new TH1F("hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA","hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA",nBins,bins.GetArray());
  // SS tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_central = new TH1F("hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_central","hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_central",nBins,bins.GetArray());
  // SS loose non-tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_central = new TH1F("hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_central","hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_central",nBins,bins.GetArray());
  // SS tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_medium = new TH1F("hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_medium","hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_medium",nBins,bins.GetArray());
  // SS loose non-tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_medium = new TH1F("hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_medium","hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_medium",nBins,bins.GetArray());
  // SS tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_forward = new TH1F("hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_forward","hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_forward",nBins,bins.GetArray());
  // SS loose non-tight tau iso
  TH1F* hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_forward = new TH1F("hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_forward","hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_forward",nBins,bins.GetArray());

  // SS anti-iso
  TH1F* hDataSSAntiIsoLooseTauIso= new TH1F( "hDataSSAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray());hDataSSAntiIsoLooseTauIso->SetFillColor(kMagenta-10);
  // SS Iso
  TH1F* hDataSSIsoLooseTauIso = new TH1F( "hDataSSIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataSSIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  // OS anti-iso
  TH1F* hDataOSAntiIsoLooseTauIso = new TH1F( "hDataOSAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataOSAntiIsoLooseTauIso   ->SetFillColor(kMagenta-10);
  // OS iso
  TH1F* hDataOSIsoLooseTauIso = new TH1F( "hDataOSIsoLooseTauIso"   ,"data anti-iso, loose tau-iso,relax vbf", nBins, bins.GetArray()); hDataOSIsoLooseTauIso   ->SetFillColor(kMagenta-10);

  //W Up/Down
  TH1F* hW_TFUp        = new TH1F( "hW_TFUp"      ,"W+jets"            , nBins , bins.GetArray());         hW_TFUp->SetFillColor(kRed+2);
  TH1F* hW_TFUpTight        = new TH1F( "hW_TFUpTight"      ,"W+jets"            , nBins , bins.GetArray());         hW_TFUpTight->SetFillColor(kRed+2);

  TH1F* hW_TFDown        = new TH1F( "hW_TFDown"      ,"W+jets"            , nBins , bins.GetArray());    hW_TFDown->SetFillColor(kRed+2);
  TH1F* hW_TFDownTight        = new TH1F( "hW_TFDownTight"      ,"W+jets"            , nBins , bins.GetArray());    hW_TFDownTight->SetFillColor(kRed+2);

  TH1F* hWLooseBTag_TFUp      = new TH1F( "hWLooseBTag_TFUp" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());
  TH1F* hWLooseBTag_TFUpTight      = new TH1F( "hWLooseBTag_TFUpTight" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());

  TH1F* hWLooseBTag_TFDown      = new TH1F( "hWLooseBTag_TFDown" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());
  TH1F* hWLooseBTag_TFDownTight      = new TH1F( "hWLooseBTag_TFDownTight" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());

  //histograms with fine binning for MSSM
  TH1F* hDataEmb_fb  = new TH1F( "hDataEmb_fb","Embedded"          , 400, 0., 2000.); hDataEmb_fb->SetFillColor(kOrange-4); 
  TH1F* hDataEmb_NotSubtracted_fb  = new TH1F( "hDataEmb_NotSubtracted_fb","Embedded not TTbarEmb subtracted", 400, 0., 2000.); hDataEmb_NotSubtracted_fb->SetFillColor(kOrange-4); 
  //TH1F* hDataEmb2_fb = new TH1F( "hDataEmb2_fb","Embedded2"        , 400, 0., 2000.); hDataEmb2_fb->SetFillColor(kOrange-4); 
  TH1F* hTTbEmb_fb  = new TH1F( "hTTbEmb_fb","TTbar Embedded"          , 400, 0., 2000.); hTTbEmb_fb->SetFillColor(kOrange-4);

  TH1F* hW_fb        = new TH1F( "hW_fb"      ,"W+jets"            , 400, 0., 2000.); hW_fb->SetFillColor(kRed+2);
  TH1F* hWTight_fb        = new TH1F( "hWTight_fb"      ,"W+jets"            , 400, 0., 2000.); hWTight_fb->SetFillColor(kRed+2);

  TH1F* hW_TFUp_fb     = new TH1F( "hW_TFUp_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFUp_fb->SetFillColor(kRed+2 );
  TH1F* hW_TFUpTight_fb     = new TH1F( "hW_TFUpTight_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFUpTight_fb->SetFillColor(kRed+2 );

  TH1F* hW_TFDown_fb     = new TH1F( "hW_TFDown_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFDown_fb->SetFillColor(kRed+2 );
  TH1F* hW_TFDownTight_fb     = new TH1F( "hW_TFDownTight_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFDownTight_fb->SetFillColor(kRed+2 );
  
  TH1F* hZtt_fb      = new TH1F( "hZtt_fb"    ,"Ztautau"           , 400, 0., 2000.); hZtt_fb->SetFillColor(kOrange-4); 
  TH1F* hZmm_fb      = new TH1F( "hZmm_fb"    ,"Z+jets, mu->tau"   , 400, 0., 2000.); hZmm_fb->SetFillColor(kBlue-2);
  TH1F* hZmj_fb      = new TH1F( "hZmj_fb"    ,"Z+jets, jet to tau", 400, 0., 2000.); hZmj_fb->SetFillColor(kBlue-2);
  TH1F* hTTb_fb      = new TH1F( "hTTb_fb"    ,"ttbar"             , 400, 0., 2000.); hTTb_fb->SetFillColor(kBlue-8);
  TH1F* hQCD_fb      = new TH1F( "hQCD_fb"    ,"QCD full vbf"      , 400, 0., 2000.); hQCD_fb->SetFillColor(kMagenta-10);
  TH1F* hQCD_CorrUp_fb      = new TH1F( "hQCD_CorrUp_fb"    ,"QCD full vbf"      , 400, 0., 2000.); hQCD_CorrUp_fb->SetFillColor(kMagenta-10);
  TH1F* hQCD_CorrDown_fb      = new TH1F( "hQCD_CorrDown_fb"    ,"QCD full vbf"      , 400, 0., 2000.); hQCD_CorrDown_fb->SetFillColor(kMagenta-10);
  TH1F* hVV_fb       = new TH1F( "hVV_fb"     ,"Diboson"           , 400, 0., 2000.); hVV_fb->SetFillColor(kRed+2);

  TH1F* hZmm_Up      = new TH1F( "hZmm_Up"    ,"Z+jets, mu->tau"   , nBins , bins.GetArray()); hZmm_Up->SetFillColor(kBlue-2);
  TH1F* hZmm_Down    = new TH1F( "hZmm_Down"    ,"Z+jets, mu->tau"   , nBins , bins.GetArray()); hZmm_Down->SetFillColor(kBlue-2);

  TH1F* hSignal[nProd][nMasses];
  TH1F* hSMEffTUp[nProd][nMasses];
  TH1F* hSMEffTDown[nProd][nMasses];
  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      hSignal[iP][iM] = new TH1F("h"+nameProd[iP]+nameMasses[iM], nameProd[iP]+nameMasses[iM], nBins , bins.GetArray());
      hSignal[iP][iM]->SetLineWidth(2);

      hSMEffTUp[iP][iM] = new TH1F("hSMEffTUp"+nameProd[iP]+nameMasses[iM], nameProd[iP]+nameMasses[iM], nBins , bins.GetArray());
      hSMEffTUp[iP][iM]->SetLineWidth(2);
      
      hSMEffTDown[iP][iM] = new TH1F("hSMEffTDown"+nameProd[iP]+nameMasses[iM], nameProd[iP]+nameMasses[iM], nBins , bins.GetArray());
      hSMEffTDown[iP][iM]->SetLineWidth(2);
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

  //GGH Higgs pT weights up/down
  TH1F* hSUSYGGHUp[nMassesS]; TH1F* hSUSYGGHDown[nMassesS];
  TH1F* hSUSYGGHScaleUp[nMassesS]; TH1F* hSUSYGGHScaleDown[nMassesS];
  for(int iM=0 ; iM<nMassesS ; iM++) {
    //tanB
    hSUSYGGHUp[iM] = new TH1F("hSUSYGGH"+nameMassesS[iM]+"Up", "SUSYGGH"+nameMassesS[iM]+"Up", nBins , bins.GetArray()); 
    hSUSYGGHUp[iM]->SetLineWidth(2);
    hSUSYGGHDown[iM] = new TH1F("hSUSYGGH"+nameMassesS[iM]+"Down", "SUSYGGH"+nameMassesS[iM]+"Down", nBins , bins.GetArray());  
    hSUSYGGHDown[iM]->SetLineWidth(2); 
    //QCD scale
    hSUSYGGHScaleUp[iM] = new TH1F("hSUSYGGH"+nameMassesS[iM]+"ScaleUp", "SUSYGGH"+nameMassesS[iM]+"ScaleUp", nBins , bins.GetArray()); 
    hSUSYGGHScaleUp[iM]->SetLineWidth(2);
    hSUSYGGHScaleDown[iM] = new TH1F("hSUSYGGH"+nameMassesS[iM]+"ScaleDown", "SUSYGGH"+nameMassesS[iM]+"ScaleDown", nBins , bins.GetArray());  
    hSUSYGGHScaleDown[iM]->SetLineWidth(2); 
  }

  TH1F* hSignalWW[nProdWW][nMassesWW];
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      hSignalWW[iP][iM] = new TH1F("h"+nameProdWW[iP]+nameMassesWW[iM], nameProdWW[iP]+nameMassesWW[iM], nBins , bins.GetArray());
      hSignalWW[iP][iM]->SetLineWidth(2);
    }
  }  

  TH1F* hSusy[nProdS][nMassesS];
  TH1F* hSusyEffTUp[nProdS][nMassesS];
  TH1F* hSusyEffTDown[nProdS][nMassesS];

  TH1F* hSusyNoWeight[nProdS][nMassesS];

  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++) {
      for(int iM=0 ; iM<nMassesS ; iM++) {
	hSusy[iP][iM] = new TH1F("hSUSY"+nameProdS[iP]+nameMassesS[iM], nameProdS[iP]+nameMassesS[iM], nBins , bins.GetArray());
	hSusy[iP][iM]->SetLineWidth(2);

	hSusyNoWeight[iP][iM] = new TH1F("hSUSYNoWeight"+nameProdS[iP]+nameMassesS[iM], nameProdS[iP]+nameMassesS[iM], nBins , bins.GetArray());
	hSusyNoWeight[iP][iM]->SetLineWidth(2);

	hSusyEffTUp[iP][iM] = new TH1F("hSUSYEffTUp"+nameProdS[iP]+nameMassesS[iM], nameProdS[iP]+nameMassesS[iM], nBins , bins.GetArray());
	hSusyEffTUp[iP][iM]->SetLineWidth(2);

	hSusyEffTDown[iP][iM] = new TH1F("hSUSYEffTDown"+nameProdS[iP]+nameMassesS[iM], nameProdS[iP]+nameMassesS[iM], nBins , bins.GetArray());
	hSusyEffTDown[iP][iM]->SetLineWidth(2);
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

  TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_OlivierTES_TauSpinner/MuTau/";
  //   TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_OlivierTES/MuTau/";
  //   TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewTauIDVariables/MuTau/";
  //   TString pathToFile    = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewTauIDVariables/MuTau/";
  TString pathToFileDY    = pathToFile;
  TString pathToFileHWW   = pathToFile; 

  cout<<"**********************************"<<endl;
  cout<<"**********************************"<<endl;
  cout<<"           FAST MODE? "<<FastMode<<endl;
  cout<<"**********************************"<<endl;
  cout<<"**********************************"<<endl;


  // DATA //
  TChain *data = new TChain("outTreePtOrd");
  if(RUN.Contains("ABC")) {
    if(!FastMode) data->Add(pathToFile+"/nTupleRun2012A*Data_MuTau.root");
    if(!FastMode) data->Add(pathToFile+"/nTupleRun2012B*Data_MuTau.root");
    if(!FastMode) data->Add(pathToFile+"/nTupleRun2012C*Data_MuTau.root");
  }
  if(RUN.Contains("D")) {
    if(!FastMode) data->Add(pathToFile+"/nTupleRun2012D*Data_MuTau.root");
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
      if(!FastMode) dataEmbedded->Add(pathToFile+"/nTupleRun2012A*Embedded_MuTau_"+fileAnalysisEmbedded+".root");//fm
      if(!FastMode) dataEmbedded->Add(pathToFile+"/nTupleRun2012B*Embedded_MuTau_"+fileAnalysisEmbedded+".root");//fm
      if(!FastMode) dataEmbedded->Add(pathToFile+"/nTupleRun2012C*Embedded_MuTau_"+fileAnalysisEmbedded+".root");//fm
    }
    if(RUN.Contains("D")){
      if(!FastMode) dataEmbedded->Add(pathToFile+"/nTupleRun2012D*Embedded_MuTau_"+fileAnalysisEmbedded+".root");//fm
    }
  }
  else {
    if(!FastMode) dataEmbedded->Add(pathToFile+"/nTupleRun2012D*EmbeddedLowPt_MuTau_"+fileAnalysisEmbedded+".root");
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
  TChain *backgroundTTbarEmb   = new TChain(treeMC);
  TChain *backgroundOthers     = new TChain(treeMC);
  TChain *backgroundWJets      = new TChain(treeMC);
  TChain *backgroundW3Jets     = new TChain(treeMC);
  //
  
  //   backgroundDY      ->Add(pathToFileDY+"nTupleDYJets_MuTau_"+fileAnalysis+".root");

  
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsTauTau_tauPolarOff_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsZTTL_tauPolarOff_MuTau_"+fileAnalysis+".root");//fm
  if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsZTTJ_tauPolarOff_MuTau_"+fileAnalysis+".root");//fm
  if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsMuToTau_tauPolarOff_MuTau_"+fileAnalysis+".root");//fm
  if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsJetToTau_tauPolarOff_MuTau_"+fileAnalysis+".root");//fm
  
  if(!version_.Contains("NoDYExcl")) {
    
    if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets1Jets*_MuTau_"+fileAnalysis+".root");//fm
    if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets2Jets*_MuTau_"+fileAnalysis+".root");//fm
    if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets3Jets*_MuTau_"+fileAnalysis+".root");//fm
    if(!FastMode) backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets4Jets*_MuTau_"+fileAnalysis+".root");//fm
    
  }
  //
  
  if(!FastMode) backgroundTTbar   ->Add(pathToFile+"nTupleTTJets_*_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundTTbarEmb->Add(pathToFile+"nTupleTTJets-Embedded_MuTau_"+fileAnalysis+".root");//fm
  //
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleT-tW_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleTbar-tW_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWWJetsTo2L2Nu_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo2L2Q_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo3LNu_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Nu_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Q_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo4L_MuTau_"+fileAnalysis+".root");
  
  if(includeWG){
    if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWGToLNuG_MuTau_"+fileAnalysis+".root");
    if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2E_MuTau_"+fileAnalysis+".root");
    if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2Mu_MuTau_"+fileAnalysis+".root");
    if(!FastMode) backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2Tau_MuTau_"+fileAnalysis+".root");
  }
  
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets-p1_MuTau_"     +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets-p2_MuTau_"     +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets1Jets_MuTau_"   +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets2Jets_MuTau_"   +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets3Jets_MuTau_"   +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets4Jets_MuTau_"   +fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets1JetsV19_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets2JetsV19_MuTau_"+fileAnalysis+".root");
  if(!FastMode) backgroundWJets   ->Add(pathToFile+"nTupleWJets3JetsV19_MuTau_"+fileAnalysis+".root");
  
  //   backgroundWJets   ->Add(pathToFile+"nTupleWJets4JetsV19_MuTau_"+fileAnalysis+".root");

  //backgroundW3Jets  ->Add(pathToFile+"nTupleWJets3Jets_MuTau_"+fileAnalysis+".root");
  backgroundW3Jets = backgroundWJets;

  if(!backgroundDY)    cout << "###  NTUPLE DY NOT FOUND ###" << endl;  
  if(!backgroundTTbar) cout << "###  NTUPLE TT NOT FOUND ###" << endl;  
  if(!backgroundTTbarEmb) cout << "###  NTUPLE TTEMB NOT FOUND ###" << endl;  
  if(!backgroundOthers)cout << "###  NTUPLE VVt NOT FOUND ###" << endl;  
  if(!backgroundWJets) cout << "###  NTUPLE W NOT FOUND ###" << endl;  
  if(!backgroundW3Jets)cout << "###  NTUPLE W3 NOT FOUND ###" << endl;  

  

  TChain *signal[nProd][nMasses];
  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      signal[iP][iM] = new TChain(treeMC);
      if(!FastMode) signal[iP][iM]->Add(pathToFile+"/nTuple"+nameProd[iP]+nameMasses[iM]+"_MuTau_"+fileAnalysis+".root");
      if(!signal[iP][iM])cout << "###  NTUPLE Signal " << nameProd[iP]+nameMasses[iM] << " NOT FOUND ###" << endl;  
    }
  }
  TChain *signalWW[nProdWW][nMassesWW];
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      signalWW[iP][iM] = new TChain(treeMC);
      if(!FastMode) signalWW[iP][iM]->Add(pathToFileHWW+"/nTuple"+nameProdWW[iP]+nameMassesWW[iM]+"_MuTau_"+fileAnalysis+".root");
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
      backgroundDYTauTau  ->Add(pathToFileDY+"/nTupleDYJets*JetsTauTau_MuTau_"  +fileAnalysis+".root");
      backgroundDYTauTau  ->Add(pathToFileDY+"/nTupleDYJetsTauTau_tauPolarOff_MuTau_"  +fileAnalysis+".root");
      
      if(!FastMode) backgroundDYTauTauLL->Add(pathToFileDY+"/nTupleDYJets*JetsZTTL_MuTau_"    +fileAnalysis+".root");//was uncommented
      if(!FastMode) backgroundDYTauTauLL->Add(pathToFileDY+"/nTupleDYJetsZTTL_tauPolarOff_MuTau_"    +fileAnalysis+".root");//was uncommented

      if(!FastMode) backgroundDYTauTauJJ->Add(pathToFileDY+"/nTupleDYJets*JetsZTTJ_MuTau_"    +fileAnalysis+".root");//was uncommented
      if(!FastMode) backgroundDYTauTauJJ->Add(pathToFileDY+"/nTupleDYJetsZTTJ_tauPolarOff_MuTau_"    +fileAnalysis+".root");//was uncommented

      if(!FastMode) backgroundDYMutoTau ->Add(pathToFileDY+"/nTupleDYJets*JetsMuToTau_MuTau_" +fileAnalysis+".root");//was uncommented
      if(!FastMode) backgroundDYMutoTau ->Add(pathToFileDY+"/nTupleDYJetsMuToTau_tauPolarOff_MuTau_" +fileAnalysis+".root");//was uncommented

      if(!FastMode) backgroundDYJtoTau  ->Add(pathToFileDY+"/nTupleDYJets*JetsJetToTau_MuTau_"+fileAnalysis+".root");//was uncommented
      if(!FastMode) backgroundDYJtoTau  ->Add(pathToFileDY+"/nTupleDYJetsJetToTau_tauPolarOff_MuTau_"+fileAnalysis+".root");//was uncommented

      cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
      cout << backgroundDYMutoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
      cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau" << endl;
      cout << backgroundDYTauTauLL->GetEntries()  << " come from DY->tautau LL"         << endl;
      cout << backgroundDYTauTauJJ->GetEntries()  << " come from DY->tautau JJ"         << endl;

      //cout<<"HERE IS TEST FOR tauPolarOff = "<<backgroundDYTauTau->GetEntries()+backgroundDYTauTauLL->GetEntries()+backgroundDYTauTauJJ->GetEntries()+backgroundDYMutoTau->GetEntries()+backgroundDYJtoTau->GetEntries()<<endl;

    }
    else 
      {
	backgroundDYTauTau  ->Add(pathToFileDY+"/nTupleDYJetsTauTau_tauPolarOff_MuTau_"+fileAnalysis+".root");
	if(!FastMode) backgroundDYTauTauLL->Add(pathToFileDY+"/nTupleDYJetsZTTL_tauPolarOff_MuTau_"    +fileAnalysis+".root");//was uncommented
	if(!FastMode) backgroundDYTauTauJJ->Add(pathToFileDY+"/nTupleDYJetsZTTJ_tauPolarOff_MuTau_"    +fileAnalysis+".root");//was uncommented
	if(!FastMode) backgroundDYMutoTau ->Add(pathToFileDY+"/nTupleDYJetsMuToTau_tauPolarOff_MuTau_"+fileAnalysis+".root");//was uncommented
	if(!FastMode) backgroundDYJtoTau  ->Add(pathToFileDY+"/nTupleDYJetsJetToTau_tauPolarOff_MuTau_"+fileAnalysis+".root");//was uncommented
      }
  }

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
  cout << backgroundDYMutoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau" << endl;
  cout << backgroundDYTauTauLL->GetEntries()  << " come from DY->tautau LL"         << endl;
  cout << backgroundDYTauTauJJ->GetEntries()  << " come from DY->tautau JJ"         << endl;

  // MAPPING //
  if(VERBOSE) cout << "-- gather the trees" << endl;

  const int nVarious = 13;
  const int nChainsSM1 = nVarious  + nProd*nMasses;
  const int nChainsSM = nChainsSM1 + nProdWW*nMassesWW;
  const int nChains  = nChainsSM + nProdS*nMassesS;
  TString treeNamesVarious[nVarious]={"SS","WJets","Data","W3Jets","TTbar","Others","DYToTauTau","DYToTauTauLL","DYToTauTauJJ","DYMutoTau","DYJtoTau","Embedded","TTbarEmbedded"};
  TChain* chainsVarious[nVarious]   ={data,backgroundWJets,data,backgroundW3Jets,backgroundTTbar,backgroundOthers,
				      backgroundDYTauTau,backgroundDYTauTauLL,backgroundDYTauTauJJ,backgroundDYMutoTau,backgroundDYJtoTau,dataEmbedded,backgroundTTbarEmb};
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

  //   if(DEBUG) {
  cout << "######################" << endl
       << "### LIST OF CHAINS ###" << endl;
  for(int iCh=0 ; iCh<nChains ; iCh++) cout << treeNames[iCh] << endl;
  cout << "######################" << endl;
  //   }

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
  //TCut tpt("ptL2>20");
  TCut tpt("ptL2>30");
  TCut antimu("tightestAntiMuWP>0");


  TCut antiele("tightestAntiEMVA5WP > 0");

//   TCut antiele("tightestAntiEMVA5WP > 0");
  TCut tiso("hpsDB3H<1.5");
  TCut tdecaymode("decayModeFindingOldDM>0.5");
  //TCut tdecaymode("decayModeFindingOldDM>0");
  TCut ltiso("hpsDB3H<10.0");
  TCut mtiso("hpsDB3H<5.0");
  TCut selection("diTauNSVfitMass>-999"); 

  // Choose selection wrt version_
  chooseSelection(variable_,version_, selection_, tiso, tdecaymode, ltiso, mtiso, antimu, antiele,liso,laiso,lliso,selection,tpt,lpt);
  cout<<"ltiso : "<<ltiso<<endl;
  if(selection_.find("vbfRelaxMt")!=string::npos) antiWsgn+=10;

  if(variable_.Contains("hpsMVA3oldDMwLT")) {
    tiso   = "etaL1<999" ;//Tight 3
    ltiso   = "etaL1<999" ;//Loose 1
    mtiso   = "etaL1<999" ;//Medium 2
  }
  if(variable_.Contains("svX")||
     variable_.Contains("svY")||
     variable_.Contains("svZ")||
     variable_.Contains("svCov")||
     variable_.Contains("flightLength")
     ) {
    tpt = tpt&&TCut("hasSecVtx>0.5");
  }

  if(selection_.find("1Prong0Pi0")!=string::npos || version_.Contains("1Prong0Pi0"))
    tpt = tpt&&TCut("decayMode==0");
  if(selection_.find("1Prong1Pi0")!=string::npos || version_.Contains("1Prong1Pi0"))
    tpt = tpt&&TCut("decayMode==1");
  if(selection_.find("3Prongs")!=string::npos || version_.Contains("3Prongs"))
    tpt = tpt&&TCut("decayMode==2");
  if(selection_.find("BL")!=string::npos || version_.Contains("BL"))
    tpt = tpt&&TCut("TMath::Abs(etaL2)<1.479");
  if(selection_.find("EC")!=string::npos || version_.Contains("EC"))
    tpt = tpt&&TCut("TMath::Abs(etaL2)>1.479");

  //Barrel/EndCap
  if(version_.Contains("Barrel"))
    {
      //       tpt = tpt&&TCut("TMath::Abs(etaL2)<=5");
      tpt = tpt&&TCut("TMath::Abs(etaL2)<=1.479");
    }
  else if(version_.Contains("EndCap"))
    {
      //       tpt = tpt&&TCut("TMath::Abs(etaL2)>0");
      tpt = tpt&&TCut("TMath::Abs(etaL2)>1.479");
    }

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
  if(version_.Contains("aIso"))  liso=laiso;
  if(version_.Contains("ltiso")) {
    tiso=ltiso; 
    mtiso=ltiso;
  }

  bool removeMtCut     = bool(selection_.find("NoMt")!=string::npos);
  bool invertDiTauSign = bool(selection_.find("SS")!=string::npos);
  TCut MtCut       = removeMtCut     ? "(etaL1<999)" : pZ;
  if(selection_.find("HighMt")!=string::npos) {
    //     MtCut="MtLeg1MVA>70";
    //     pZ= "MtLeg1MVA>70";
    //     apZ= "MtLeg1MVA>70";
    MtCut="MtLeg1MVA>50";
    pZ= "MtLeg1MVA>50";
    apZ= "MtLeg1MVA>50";
    if(version_.Contains("WJetsNorm")){
      MtCut="MtLeg1MVA>70";
      pZ= "MtLeg1MVA>70";
      apZ= "MtLeg1MVA>60 && MtLeg1MVA<120 ";
    }
  }
  cout<<"MtCut "<<MtCut<<endl;
  cout<<"pZ "<<pZ<<endl;
  cout<<"apZ "<<apZ<<endl;

  TCut diTauCharge = invertDiTauSign ? SS : OS; 
  string sign = invertDiTauSign ? "SS" : "OS";
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
    vbf = TCut("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1 && nJets20BTagged<1");
  else{
    vbf = TCut("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1 && nJets20BTagged<1");
    vbf = vbf && !vbfTight;
  }
  TCut vbfLoose("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedTightZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");

  TCut vbfLooseQCD("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  /*
    if(version_.Contains("Soft")) {
    vbfLooseQCD = vbf;
    vbfLoose = vbf;
    }
  */
  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");
  TCut boost("nJets30>0 && pt1>30 && nJets20BTagged<1");
  boost = boost && !vbf /*&& !vh*/;
  if(!version_.Contains("OldCat")) boost = boost && !vbfTight ;

  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut bTagLoose("nJets30<2 && nJets20BTaggedLoose>0"); //for W shape in b-Category
  //TCut bTagLoose("nJets30<2 && nJets20>=1");
  TCut nobTag("nJets20BTagged==0");
  TCut inclusive("");

  TCut novbf("nJets30<1 && nJets20BTagged==0");
  TCut ZmmSel("MEtMVA<30 && nJets30<2 && ptL2>30");

  if(!MSSM)
    {
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
  else
    {
      if(selection_.find("nobTag")!=string::npos)
	{
	  if(selection_.find("nobTagHigh")!=string::npos && !selection_.find("HighMt")!=string::npos)
	    {
	      nobTag = nobTag&&TCut("ptL2>60.");
	    }
	  else if(selection_.find("nobTagMedium")!=string::npos)
	    {
	      nobTag = nobTag&&TCut("ptL2>45.&&ptL2<=60.");
	    }
	  else if(selection_.find("nobTagLow")!=string::npos)
	    {
	      nobTag = nobTag&&TCut("ptL2>30.&&ptL2<=45.");
	    }
	}
      else if(selection_.find("bTag")!=string::npos && !selection_.find("nobTag")!=string::npos)
	{
	  if(selection_.find("bTagHigh")!=string::npos && !selection_.find("HighMt")!=string::npos)
	    {
	      bTagLoose = bTagLoose&&TCut("ptL2>45.") ;
	      bTag = bTag&&TCut("ptL2>45.");
	    }
	  else if(selection_.find("bTagLow")!=string::npos)
	    {
	      bTagLoose = bTagLoose&&TCut("ptL2>30.&&ptL2<=45.") ;
	      bTag = bTag&&TCut("ptL2>30.&&ptL2<=45.");
	    }	  
	}
      else if(selection_.find("inclusive")!=string::npos)
	{
	  if(selection_.find("High")!=string::npos && !selection_.find("HighMt")!=string::npos)
	    {
	      inclusive = inclusive&&TCut("ptL2>60.");
	    }
	  else if(selection_.find("Medium")!=string::npos)
	    {
	      inclusive = inclusive&&TCut("ptL2>45.&&ptL2<=60.");
	    }
	  else if(selection_.find("Low")!=string::npos)
	    {
	      inclusive = inclusive&&TCut("ptL2>30.&&ptL2<=45.");
	    }	  
	}
    }

  TCut sbinCatIncl("etaL1<999");
  TCut sbinCat("");
  if(     selection_.find("inclusive")!=string::npos) sbinCat = inclusive&&TCut("etaL1<999");
  else if(selection_.find("oneJet")!=string::npos)    sbinCat = oneJet;
  else if(selection_.find("twoJets")!=string::npos)   sbinCat = twoJets;
  else if(selection_.find("vh")!=string::npos)        sbinCat = vh;
  else if(selection_.find("novbf")!=string::npos)     sbinCat = novbf;
  else if(selection_.find("ZmmSel")!=string::npos)     sbinCat = ZmmSel;
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

  TCut sbinPresel           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && hltevent   ;
  TCut sbinEmbeddingPresel  = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection               ;
  TCut sbinaIsoPresel       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && hltevent   ;
  TCut sbinlIsoPresel       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && lliso && lveto && selection && hltevent   ;
  TCut sbinaIsoLtisoPresel  = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && hltevent  ;
  TCut sbinLtisoPresel      = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && hltevent   ;

  TCut sbinInclusive                     = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinChargeRelInclusive            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection                 && MtCut  && hltevent   ;
  TCut sbinChargeRelLtisoInclusive       = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection                && MtCut  && hltevent   ;
  TCut sbinChargeRelPZetaRelInclusive    = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection                          && hltevent   ;
  TCut sbinEmbeddingInclusive            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut                ;
  TCut sbinPZetaRelEmbeddingInclusive    = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge                          ;
  TCut sbinPZetaRelSSInclusive           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS                     && hltevent   ;
  TCut sbinPZetaRelInclusive             = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge            && hltevent   ;
  TCut sbinSSInclusive                   = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoInclusive               = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinAisoInclusive                 = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelSSaIsoInclusive       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                     && hltevent   ;
  TCut sbinPZetaRelSSaIsoMtisoInclusive  = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                     && hltevent   ;

  TCut sbinSSaIsoLtisoInclusive          = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoMtisoInclusive          = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinPZetaRelaIsoInclusive         = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge            && hltevent   ;

  TCut sbinSSltisoInclusive              = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinLtisoInclusive                = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinMtisoInclusive                = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelLtisoInclusive        = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge            && hltevent   ;

  TCut sbin                   = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinEmbedding          = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut                ;
  TCut sbinEmbeddingPZetaRel  = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge                          ;
  TCut sbinPZetaRel           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelaIso       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelSSaIso     = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                     && hltevent   ;
  TCut sbinSS                 = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinPZetaRelSS         = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS                     && hltevent   ;
  TCut sbinAiso               = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinSSaIso             = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso1            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && lliso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso2            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSlIso3            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && lliso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoLtiso        = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSaIsoMtiso        = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSltiso            = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinSSmtiso            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS           && MtCut  && hltevent   ;
  TCut sbinLtiso              = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinMtiso              = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge  && MtCut  && hltevent   ;
  TCut sbinPZetaRelMtiso      = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge            && hltevent   ;
  TCut sbinPZetaRelSSaIsoMtiso= lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                     && hltevent   ;

  cout << sbin << endl;
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

  //Monte Carlo ZTT Inclusive
  float ExtrapDYInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusive,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->tautau             = " << ExtrapDYInclusive << " +/- " <<  Error << endl; 
  
  //Monte Carlo ZTT Inclusive No Mt cut
  float ExtrapDYInclusivePZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusivePZetaRel,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRelInclusive);
  cout << "All Z->tautau (pZeta Rel) = " << ExtrapDYInclusivePZetaRel << " +/- " <<  Error << endl; 

  //not used
  float ExtrapLFakeInclusive = 0.;
  float ExtrapJFakeInclusive = 0.;

  //Monte Carlo ZTT in the category
  float ExtrapDYNum = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNum,                  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbin);

  //Monte Carlo ZTT in the category High Mt
  float ExtrapDYNuminSidebandRegion = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNuminSidebandRegion,  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel&&apZ);
  
  //Monte Carlo ZTT in the category No Mt
  float ExtrapDYNumPZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], variable, ExtrapDYNumPZetaRel,          Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel);
  //Monte Carlo ZTT: compute the probability for an event to be in a category
  float ExtrapolationFactorMadGraph      = ExtrapDYNum/ExtrapDYInclusive;//probability to enter a category taken from MC
  float ErrorExtrapolationFactorMadGraph = TMath::Sqrt(ExtrapolationFactorMadGraph*(1-ExtrapolationFactorMadGraph)/ExtrapDYInclusive);
  cout << "Extrap. factor using MadGraph            = " << ExtrapolationFactorMadGraph << " +/- " << ErrorExtrapolationFactorMadGraph << endl;

  //Embedded ZTT in the category
  float ExtrapEmbedNum = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNum,                 Error, 1.0, hExtrap, sbinEmbedding);
  //Embedded ZTT in the category High Mt
  float ExtrapEmbedNuminSidebandRegion = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNuminSidebandRegion, Error, 1.0, hExtrap, sbinEmbeddingPZetaRel&&apZ);

  if(DEBUG) cout << "drawHistogram Embed sbinEmbeddingPZetaRel" << endl;  
  //Embedded ZTT in the category No Mt
  float ExtrapEmbedNumPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedNumPZetaRel,         Error, 1.0, hExtrap, sbinEmbeddingPZetaRel);

  if(DEBUG) cout << "drawHistogram Embed sbinEmbeddingInclusive" << endl;
  //Embedded ZTT Inclusive
  float ExtrapEmbedDen = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedDen,                 Error, 1.0, hExtrap, sbinEmbeddingInclusive);

  if(DEBUG) cout << "drawHistogram Embed sbinPZetaRelEmbeddingInclusive" << endl;
  //Embedded ZTT Inclusive No Mt
  float ExtrapEmbedDenPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"Embed", version_,analysis_, RUN,mapAllTrees["Embedded"], variable, ExtrapEmbedDenPZetaRel,         Error, 1.0, hExtrap, sbinPZetaRelEmbeddingInclusive);

  //Probability to enter category, from Embedded
  ExtrapolationFactorZ             = ExtrapEmbedDen!=0 ? ExtrapEmbedNum/ExtrapEmbedDen : 0; 
  ErrorExtrapolationFactorZ        = ExtrapEmbedDen!=0 ? TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen) : 0;
  //Ratio of the probabilities from Embedded to MC
  ExtrapolationFactorZDataMC       = ExtrapolationFactorMadGraph!=0 ? ExtrapolationFactorZ/ExtrapolationFactorMadGraph : 0;
  //Extrapolation factor from No Mt to Mt < 30 GeV from Embedded
  ExtrapolationFactorZFromSideband = ExtrapEmbedDenPZetaRel!=0 ? ExtrapEmbedDen/ExtrapEmbedDenPZetaRel : 0;
  //Extrapolation factor from High Mt to No Mt from MC
  float sidebandRatioMadgraph = ExtrapDYNumPZetaRel!=0 ? ExtrapDYNuminSidebandRegion/ExtrapDYNumPZetaRel : 0;
  //Extrapolation factor from High Mt to No Mt from Embedded
  float sidebandRatioEmbedded = ExtrapEmbedNumPZetaRel!=0 ? ExtrapEmbedNuminSidebandRegion/ExtrapEmbedNumPZetaRel : 0;
  //Ratio between Embedded and MC for the previous extrapolation factors
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

  // (mapchain mapAllTrees, TString version_, TString analysis_,
  // 		 TString WJetType, TString RUN,
  // 		 TH1F* qcdHisto, TH1F* ssHisto, bool evaluateWSS, string sign, bool useFakeRate, bool removeMtCut, string selection_, 
  // 		 float& SSQCDinSignalRegionDATAIncl_, float& SSIsoToSSAIsoRatioQCD, float& scaleFactorTTSSIncl,
  // 		 float& extrapFactorWSSIncl, 
  // 		 float& SSWinSignalRegionDATAIncl, float& SSWinSignalRegionMCIncl,
  // 		 float& SSWinSidebandRegionDATAIncl, float& SSWinSidebandRegionMCIncl,
  // 		 TH1F* hExtrap, TString variable = "",
  // 		 float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor = 0.,
  // 		 float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
  // 		 float  MutoTauCorrectionFactor=0. , float JtoTauCorrectionFactor=0.,
  // 		 float OStoSSRatioQCD = 0.,
  // 		 float antiWsdb=0., float antiWsgn=0., bool useMt=true,
  // 		 TCut sbin = "",
  // 		 TCut sbinCatForWextrapolation = "",
  // 		 TCut sbinPZetaRel ="", TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="", 
  // 		 TCut sbinPZetaRelaIsoInclusive="", TCut sbinPZetaRelaIso="", TCut sbinPZetaRelaIsoSideband = "", 
  // 		 TCut vbf="", TCut boost="", TCut zeroJet="", TCut sbinCat="", TCut sbinCatForSbin="",
  // 		 TCut sbinPairIso="", TCut sbinPairAiso="",
  // 		 bool subtractTT=true, bool subtractVV=true)

  //   evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet", RUN, 0, 0, true, "SS", false, removeMtCut, "inclusive", 
  // 	      SSQCDinSignalRegionDATAIncl , SSIsoToSSAIsoRatioQCD, scaleFactorTTSSIncl,
  // 	      extrapFactorWSSIncl, 
  // 	      SSWinSignalRegionDATAIncl, SSWinSignalRegionMCIncl,
  // 	      SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
  //  	      hExtrap, variable,
  //  	      Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
  // 	      ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
  //  	      MutoTauCorrectionFactor, JtoTauCorrectionFactor, 
  // 	      OStoSSRatioQCD,
  //  	      antiWsdb, antiWsgn, useMt,
  // 	      sbinSSInclusive,
  // 	      sbinCatIncl,
  //  	      sbinPZetaRelSSInclusive, pZ, apZ, sbinPZetaRelSSInclusive, 
  //  	      //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoMtisoInclusive, 
  //  	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, 
  // 	      vbf, oneJet, zeroJet, sbinCatIncl, sbinCatIncl, sbinPresel, sbinaIsoPresel, true, true);

  evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet", RUN, 0, 0, true, "SS", false, removeMtCut, "inclusive", 
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
	      sbinChargeRelPZetaRelInclusive,//sbinPZetaRelInclusive,//changed here OD+IN
 	      sbinPZetaRelSSInclusive, pZ, apZ, sbinPZetaRelSSInclusive, 
 	      //sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoMtisoInclusive, 
 	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, 
	      vbf, oneJet, zeroJet, sbinCat, sbinCatIncl, sbinPresel, sbinaIsoPresel, true, true);//new implementation Mar14
  // 	      vbf, oneJet, zeroJet, sbinCatIncl, sbinCatIncl, sbinPresel, sbinaIsoPresel, true, true);//new implementation Mar14

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
  float scaleFactorTTOSW3Jets = 1.;
  float scaleFactorTTOSWJets  = 1.;

  mapchain::iterator it;
  TString currentName, h1Name, h2Name ;
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
    h2Name         = "h2_"+currentName;
    TH1F* h1       = new TH1F( h1Name ,"" , nBins , bins.GetArray());
    TH1F* h2       = new TH1F( h2Name ,"" , nBins , bins.GetArray());

    TH1F* hCleaner = new TH1F("hCleaner","",nBins , bins.GetArray());
    TH1F* hCleanerfb = new TH1F("hCleanerfb","",400, 0., 2000.); //fine binning hostogram for MSSM
    if ( !h1->GetSumw2N() )       h1->Sumw2(); 
    if ( !hCleaner->GetSumw2N() ) hCleaner->Sumw2();
    if ( !hCleanerfb->GetSumw2N() ) hCleanerfb->Sumw2();

    if(currentName.Contains("SS")){
      
      cout << "************** BEGIN QCD evaluation using SS events *******************" << endl;

      TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
      float dummy1 = 0.;      

      TCut sbinCatForWextrapolation = sbinChargeRelPZetaRelInclusive;//changed here OD+IN
      //       TCut sbinCatForWextrapolation = sbinPZetaRelInclusive;//changed here OD+IN
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	sbinCatForWextrapolation = vbfLoose;
      
      // hQCD for relaxed vbf selection
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) {

	evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCDrelaxVBF, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, h1, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

      // evaluateQCD(mapchain mapAllTrees, TString version_, TString analysis_,
      // 		 TString WJetType, TString RUN,
      // 		 TH1F* qcdHisto, TH1F* ssHisto, bool evaluateWSS, string sign, bool useFakeRate, bool removeMtCut, string selection_, 
      // 		 float& SSQCDinSignalRegionDATAIncl_, float& SSIsoToSSAIsoRatioQCD, float& scaleFactorTTSSIncl,
      // 		 float& extrapFactorWSSIncl, 
      // 		 float& SSWinSignalRegionDATAIncl, float& SSWinSignalRegionMCIncl,
      // 		 float& SSWinSidebandRegionDATAIncl, float& SSWinSidebandRegionMCIncl,
      // 		 TH1F* hExtrap, TString variable = "",
      // 		 float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor = 0.,
      // 		 float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
      // 		 float  MutoTauCorrectionFactor=0. , float JtoTauCorrectionFactor=0.,
      // 		 float OStoSSRatioQCD = 0.,
      // 		 float antiWsdb=0., float antiWsgn=0., bool useMt=true,
      // 		 TCut sbin = "",
      // 		 TCut sbinCatForWextrapolation = "",
      // 		 TCut sbinPZetaRel ="", TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="", 
      // 		 TCut sbinPZetaRelaIsoInclusive="", TCut sbinPZetaRelaIso="", TCut sbinPZetaRelaIsoSideband = "", 
      // 		 TCut vbf="", TCut boost="", TCut zeroJet="", TCut sbinCat="", TCut sbinCatForSbin="",
      // 		 TCut sbinPairIso="", TCut sbinPairAiso="",
      // 		 bool subtractTT=true, bool subtractVV=true){
      //   drawHistogram(sbinPairIso,sbinCatForSbin,"Data", version_,analysis_, RUN, mapAllTrees["Data"], variable,              SSQCDinSignalRegionDATAIncl,        Error, 1.0,         hExtrap, sbin, 1);//Olivier


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
	
	evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, h1fb, hCleanerfb, true, "SS", false, removeMtCut, selection_,  
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
    }
    else{

      if(!currentName.Contains("Embed")){

	float Error = 0.;

	if(currentName.Contains("DYToTauTau")){
	  float NormDYToTauTau = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYToTauTau, Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbin, 1);
	  hZtt->Add(h1, ExtrapolationFactorZFromSideband);
	  float NormDYToTauTauLL = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, mapAllTrees["DYToTauTauLL"], variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
          hZtt->Add(hCleaner, 1.0);

	  //fine binning for MSSM
	  if(selection_.find("bTag")!=string::npos){
	    hCleanerfb->Reset(); float NormDYToTauTau_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYToTauTau_fb, Error,  Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZtt_fb->Add(hCleanerfb, ExtrapolationFactorZFromSideband);
	    hCleanerfb->Reset();
	    float NormDYToTauTauLL_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, mapAllTrees["DYToTauTauLL"], variable, NormDYToTauTauLL_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZtt_fb->Add(hCleanerfb, 1.0);
	    hZtt_fb->Scale(hZtt->Integral()/hZtt_fb->Integral());
	    hCleanerfb->Reset();
	  }
	}
	if(currentName.Contains("TTbar")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){ 
	    float NormTTjets = 0.; 

	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight,"MCTTJets", version_,analysis_, RUN,currentTree, variable, NormTTjets, Error,   Lumi*TTxsectionRatio*scaleFactorTTOSW3Jets*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose,"MCTTJets", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSW3Jets*hltEff_/1000., hCleaner, sbinInclusive, 1);

	    hTTb->Add(hCleaner, 1.0);
	    cout << "--- TTbar shape histogram has Integral=" << hTTb->Integral() << endl;
	    NormTTjets = 0.;

	    drawHistogram(sbinPresel,sbinCat,"MCTTJets", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSW3Jets*hltEff_/1000., h1, sbin, 1);
	    cout << "--- TTbar Norm histogram has Integral=" << h1->Integral() << endl;
	    hTTb->Scale( hTTb->Integral()!=0 ? h1->Integral()/hTTb->Integral() : 1.0 ); 
	  } 
	  else{
	    float NormTTjets = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MCTTJets", version_,analysis_, RUN,currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., h1, sbin, 1);
	    hTTb->Add(h1, 1.0);

	    //Top pt reweighting
	    hCleaner->Reset();
	    float NormTTjetsUp = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MCTTJetsUp",version_,analysis_, RUN, currentTree, variable, NormTTjetsUp,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    hTTbUp->Add(hCleaner, 1.0);

	    hCleaner->Reset();
	    float NormTTjetsDown = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MCTTJetsDown",version_,analysis_, RUN, currentTree, variable, NormTTjetsDown,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    hTTbDown->Add(hCleaner, 1.0);

	    //Jet to tau FR correction (from W+Jets)
	    hCleaner->Reset();
	    float NormTTjetsTauFakeUp = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MCTTJetsTauFakeUp",version_,analysis_, RUN, currentTree, variable, NormTTjetsTauFakeUp,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    hTTbTauFakeUp->Add(hCleaner, 1.0);

	    hCleaner->Reset();
	    float NormTTjetsTauFakeDown = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MCTTJetsTauFakeDown",version_,analysis_, RUN, currentTree, variable, NormTTjetsTauFakeDown,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    hTTbTauFakeDown->Add(hCleaner, 1.0);

	    //fine binning for MSSM 
	    if(selection_.find("bTag")!=string::npos){
	      hCleanerfb->Reset(); float NormTTjets_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat,"MCTTJets", version_,analysis_, RUN,currentTree, variable, NormTTjets_fb,     Error,  Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleanerfb, sbin, 1);
	      hTTb_fb->Add(hCleanerfb, hTTb->Integral()/hCleanerfb->Integral());
	      hCleanerfb->Reset();
	    }
	  }
	}
	else if(currentName.Contains("W3Jets") && selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){

	  TH1F* hExtrapW3Jets = new TH1F("hExtrapW3Jets","",nBins , bins.GetArray());
	 
	  cout << "************** BEGIN W+3jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinCatForWextrapolation = sbinChargeRelPZetaRelInclusive;//sbinPZetaRelInclusive;//changed here OD+IN
	  TCut apZCut = apZ;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    sbinCatForWextrapolation = vbfLoose;
	    if(selection_.find("vbfTight")!=string::npos)
	      sbinCatForWextrapolation = vbfRelaxedTight;
	    apZCut = apZ2;
	  }
	  evaluateWextrapolation(mapAllTrees, version_,analysis_, "MC", RUN, "OS", false, selection_, 
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
// 	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAW3Jets/OSWinSidebandRegionMCW3Jets);//comented 021214
	  if(removeMtCut) h1->Scale(OSWinSignalRegionDATAW3Jets/OSWinSignalRegionMCW3Jets);//added 021214
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
	else if(currentName.Contains("WJets") && !(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)){

	  TH1F* hExtrapW = new TH1F("hExtrap","",nBins , bins.GetArray());
	  TCut sbinCatForWextrapolation = sbinChargeRelPZetaRelInclusive;//changed here OD+IN;

	  //Get W Up/Down histograms as well
// 	  if(selection_.find("bTag")!=string::npos && MSSM){
	    
	    TH1F* hExtrapWUp = new TH1F("hExtrapUp","",nBins , bins.GetArray());

	    cout << "************** BEGIN W+jets (Up) normalization using high-Mt sideband *******************" << endl;

	    //normalization computed in signal region (Tight)
	    float OSWinSignalRegionDATAWJetsUp = 0.; float OSWinSignalRegionMCWJetsUp = 0.;
	    float OSWinSidebandRegionDATAWJetsUp = 0.; float OSWinSidebandRegionMCWJetsUp = 0.;
	    float scaleFactorTTOSWJetsUp = 0.; float extrapFactorWOSWJetsUp = 0.;
	    evaluateWextrapolation(mapAllTrees, version_,analysis_, "MC_WJetUp", RUN, "OS", false, selection_,
				   extrapFactorWOSWJetsUp,
				   OSWinSignalRegionDATAWJetsUp,   OSWinSignalRegionMCWJetsUp,
				   OSWinSidebandRegionDATAWJetsUp, OSWinSidebandRegionMCWJetsUp,
				   scaleFactorTTOSWJetsUp,
				   hExtrapWUp, variable,
				   Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				   ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				   MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				   antiWsdb, antiWsgn, useMt,
				   sbinCatForWextrapolation,
				   sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				   pZ, apZ, sbinPZetaRelInclusive,
				   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet,
				   sbinCat, sbinPresel, sbinaIsoPresel);
	    delete hExtrapWUp;


	    h1->Reset();
	    
	    //Shape in Tight tau iso -- for the increased bbb errors
	    float NormWJetsUp = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC_WJetUp", version_,analysis_, RUN,currentTree, variable, NormWJetsUp, Error,   Lumi*hltEff_/1000., h1, sbin, 1);

	    hW_TFUpTight->Reset(); 
            hW_TFUpTight->Add(h1, 1.0);

	    // 	    cout<<"integral in tight = "<<hW_TFUpTight->Integral()<<endl;

	    //Nominal shape in Loose tau iso
	    h1->Reset();
	    drawHistogram(sbinLtisoPresel,sbinCat,"MC_WJetUp_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsUp, Error,   Lumi*hltEff_/1000., h1, sbinLtiso, 1);

	    // 	    cout<<"integral in loose = "<<h1->Integral()<<endl;

	    ////compute loose to tight ratio
	    Double_t LooseToTightRatioUp = h1->Integral()/hW_TFUpTight->Integral();

	    // 	    cout<<"ratio of loose to tight integrals = "<<LooseToTightRatioUp<<endl;

	    ////normalize to expectation from data = OSWinSignalRegionDATAWJetsUp 
// 	    if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJetsUp/OSWinSidebandRegionMCWJetsUp);//comented 021214
	    if(removeMtCut) h1->Scale((hW_TFUpTight->Integral()*OSWinSignalRegionDATAWJetsUp/OSWinSignalRegionMCWJetsUp)/h1->Integral());//added 021214
	    else h1->Scale(OSWinSignalRegionDATAWJetsUp/h1->Integral());

	    ////increase the error using loose to tight ratio
	    for(Int_t ibin = 1 ; ibin <= h1->GetNbinsX() ; ++ibin)
	      {
		// 		cout<<"bin #"<<ibin<<endl;
		// 		cout<<"bin content = "<<h1->GetBinContent(ibin)<<endl;
		// 		cout<<"bin error before = "<<h1->GetBinError(ibin)<<endl;
		h1->SetBinError(ibin, h1->GetBinError(ibin)*pow(LooseToTightRatioUp,0.5));
		// 		cout<<"bin error after = "<<h1->GetBinError(ibin)<<endl;
		// 		cout<<"--"<<endl;
	      }

	    ////fill shape
	    hW_TFUp->Reset(); 
            hW_TFUp->Add(h1, 1.0);

	    //fine binning for MSSM
	    if(selection_.find("nobTag")!=string::npos){
	      float NormWJets_fbUp = 0.;
	      hW_TFUp_fb->Reset();
	      hW_TFUpTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat,"MC_WJetUp", version_,analysis_, RUN,currentTree, variable, NormWJets_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hW_TFUpTight_fb->Add(hCleanerfb,1.) ;

	      // 	      cout<<"fb: integral in tight = "<<hW_TFUpTight_fb->Integral()<<endl;

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,sbinCat,"MC_WJetUp_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJets_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtiso, 1);
	      // 	      cout<<"fb: integral in loose = "<<hCleanerfb->Integral()<<endl;
	      ////compute loose to tight ratio
	      Double_t LooseToTightRatioUp_fb = hCleanerfb->Integral()/hW_TFUpTight_fb->Integral();
	      
	      // 	      cout<<"fb: ratio of loose to tight integrals = "<<LooseToTightRatioUp_fb<<endl;

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsUp 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJetsUp/OSWinSidebandRegionMCWJetsUp);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hW_TFUpTight->Integral()*OSWinSignalRegionDATAWJetsUp/OSWinSignalRegionMCWJetsUp)/ hCleanerfb->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJetsUp/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin)
		{
		  // 		  cout<<"bin #"<<ibin<<endl;
		  // 		  cout<<"bin content = "<<hCleanerfb->GetBinContent(ibin)<<endl;
		  // 		  cout<<"bin error before = "<<hCleanerfb->GetBinError(ibin)<<endl;
		  hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatioUp_fb,0.5));
		  // 		  cout<<"bin error after = "<<hCleanerfb->GetBinError(ibin)<<endl;
		  // 		  cout<<"--"<<endl;
		}
	      
	      ////fill shape
	      hW_TFUp_fb->Add(hCleanerfb,1.);

	    }
	    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	      float NormWJetsBTag = 0.;
	      hWLooseBTag_TFUp->Reset(); hWLooseBTag_TFUpTight->Reset();
	      hCleaner->Reset();

	      //tight
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJetUp", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      hWLooseBTag_TFUpTight->Add(hCleaner,1.);
	      
	      //loose
	      hCleaner->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJetUp_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatioUp = hCleaner->Integral()/hWLooseBTag_TFUpTight->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsUp 
// 	      if(removeMtCut) hCleaner->Scale(OSWinSidebandRegionDATAWJetsUp/OSWinSidebandRegionMCWJetsUp);//comented 021214
	      if(removeMtCut) hCleaner->Scale((hW_TFUpTight->Integral()*OSWinSignalRegionDATAWJetsUp/OSWinSignalRegionMCWJetsUp)/hCleaner->Integral());//added 021214
	      else hCleaner->Scale(OSWinSignalRegionDATAWJetsUp/hCleaner->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleaner->GetNbinsX() ; ++ibin) hCleaner->SetBinError(ibin, hCleaner->GetBinError(ibin)*pow(LooseToTightRatioUp,0.5));

	      ////fill shape
	      hWLooseBTag_TFUp->Reset();
	      hWLooseBTag_TFUp->Add(hCleaner,1.);

	      //same for fine binning in MSSM
	      //fine binning for MSSM
	      hW_TFUp_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fbUp = 0.;
	      hW_TFUpTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJetUp", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	      hW_TFUpTight_fb->Add(hCleanerfb,1.);

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJetUp_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatioUp_fb = hCleanerfb->Integral()/hW_TFUpTight_fb->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsUp 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJetsUp/OSWinSidebandRegionMCWJetsUp);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hW_TFUpTight->Integral()*OSWinSignalRegionDATAWJetsUp/OSWinSignalRegionMCWJetsUp)/ hCleanerfb->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJetsUp/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin) hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatioUp_fb,0.5));

	      //fill histograms
	      hW_TFUp_fb->Reset();
	      hW_TFUp_fb->Add(hCleanerfb,1.);
	      
	    }
	    else//inclusive -- take histo directly from Tight
	      {
		hW_TFUp->Reset();
		hW_TFUp->Add(hW_TFUpTight,1.);
		//fine binning not used for inclusive
	      }


	    TH1F* hExtrapWDown = new TH1F("hExtrapDown","",nBins , bins.GetArray());

	    cout << "************** BEGIN W+jets (Down) normalization using high-Mt sideband *******************" << endl;

	    //normalization computed in signal region (Tight)
	    float OSWinSignalRegionDATAWJetsDown = 0.; float OSWinSignalRegionMCWJetsDown = 0.;
	    float OSWinSidebandRegionDATAWJetsDown = 0.; float OSWinSidebandRegionMCWJetsDown = 0.;
	    float scaleFactorTTOSWJetsDown = 0.; float extrapFactorWOSWJetsDown = 0.;
	    evaluateWextrapolation(mapAllTrees, version_,analysis_, "MC_WJetDown", RUN, "OS", false, selection_,
				   extrapFactorWOSWJetsDown,
				   OSWinSignalRegionDATAWJetsDown,   OSWinSignalRegionMCWJetsDown,
				   OSWinSidebandRegionDATAWJetsDown, OSWinSidebandRegionMCWJetsDown,
				   scaleFactorTTOSWJetsDown,
				   hExtrapWDown, variable,
				   Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				   ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				   MutoTauCorrectionFactor, JtoTauCorrectionFactor,
				   antiWsdb, antiWsgn, useMt,
				   sbinCatForWextrapolation,
				   sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				   pZ, apZ, sbinPZetaRelInclusive,
				   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet,
				   sbinCat, sbinPresel, sbinaIsoPresel);
	    delete hExtrapWDown;


	    h1->Reset();
	    
	    //Shape in Tight tau iso -- for the increased bbb errors
	    float NormWJetsDown = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC_WJetDown", version_,analysis_, RUN,currentTree, variable, NormWJetsDown, Error,   Lumi*hltEff_/1000., h1, sbin, 1);

	    hW_TFDownTight->Reset(); 
            hW_TFDownTight->Add(h1, 1.0);

	    // 	    cout<<"integral in tight = "<<hW_TFDownTight->Integral()<<endl;

	    //Nominal shape in Loose tau iso
	    h1->Reset();
	    drawHistogram(sbinLtisoPresel,sbinCat,"MC_WJetDown_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsDown, Error,   Lumi*hltEff_/1000., h1, sbinLtiso, 1);

	    // 	    cout<<"integral in loose = "<<h1->Integral()<<endl;

	    ////compute loose to tight ratio
	    Double_t LooseToTightRatioDown = h1->Integral()/hW_TFDownTight->Integral();

	    // 	    cout<<"ratio of loose to tight integrals = "<<LooseToTightRatioDown<<endl;

	    ////normalize to expectation from data = OSWinSignalRegionDATAWJetsDown 
// 	    if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJetsDown/OSWinSidebandRegionMCWJetsDown);//comented 021214
	    if(removeMtCut) h1->Scale((hW_TFDownTight->Integral()*OSWinSignalRegionDATAWJetsDown/OSWinSignalRegionMCWJetsDown)/h1->Integral());//added 021214
	    else h1->Scale(OSWinSignalRegionDATAWJetsDown/h1->Integral());

	    ////increase the error using loose to tight ratio
	    for(Int_t ibin = 1 ; ibin <= h1->GetNbinsX() ; ++ibin)
	      {
		// 		cout<<"bin #"<<ibin<<endl;
		// 		cout<<"bin content = "<<h1->GetBinContent(ibin)<<endl;
		// 		cout<<"bin error before = "<<h1->GetBinError(ibin)<<endl;
		h1->SetBinError(ibin, h1->GetBinError(ibin)*pow(LooseToTightRatioDown,0.5));
		// 		cout<<"bin error after = "<<h1->GetBinError(ibin)<<endl;
		// 		cout<<"--"<<endl;
	      }

	    ////fill shape
	    hW_TFDown->Reset(); 
            hW_TFDown->Add(h1, 1.0);

	    //fine binning for MSSM
	    if(selection_.find("nobTag")!=string::npos){
	      float NormWJets_fbDown = 0.;
	      hW_TFDown_fb->Reset();
	      hW_TFDownTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat,"MC_WJetDown", version_,analysis_, RUN,currentTree, variable, NormWJets_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hW_TFDownTight_fb->Add(hCleanerfb,1.) ;

	      // 	      cout<<"fb: integral in tight = "<<hW_TFDownTight_fb->Integral()<<endl;

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,sbinCat,"MC_WJetDown_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJets_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtiso, 1);
	      // 	      cout<<"fb: integral in loose = "<<hCleanerfb->Integral()<<endl;
	      ////compute loose to tight ratio
	      Double_t LooseToTightRatioDown_fb = hCleanerfb->Integral()/hW_TFDownTight_fb->Integral();
	      
	      // 	      cout<<"fb: ratio of loose to tight integrals = "<<LooseToTightRatioDown_fb<<endl;

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsDown 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJetsDown/OSWinSidebandRegionMCWJetsDown);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hW_TFDownTight->Integral()*OSWinSignalRegionDATAWJetsDown/OSWinSignalRegionMCWJetsDown)/ hCleanerfb->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJetsDown/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin)
		{
		  // 		  cout<<"bin #"<<ibin<<endl;
		  // 		  cout<<"bin content = "<<hCleanerfb->GetBinContent(ibin)<<endl;
		  // 		  cout<<"bin error before = "<<hCleanerfb->GetBinError(ibin)<<endl;
		  hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatioDown_fb,0.5));
		  // 		  cout<<"bin error after = "<<hCleanerfb->GetBinError(ibin)<<endl;
		  // 		  cout<<"--"<<endl;
		}
	      
	      ////fill shape
	      hW_TFDown_fb->Reset();
	      hW_TFDown_fb->Add(hCleanerfb,1.);

	    }
	    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	      float NormWJetsBTag = 0.;
	      hWLooseBTag_TFDown->Reset(); hWLooseBTag_TFDownTight->Reset();
	      hCleaner->Reset();

	      //tight
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJetDown", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      hWLooseBTag_TFDownTight->Add(hCleaner,1.);
	      
	      //loose
	      hCleaner->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJetDown_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatioDown = hCleaner->Integral()/hWLooseBTag_TFDownTight->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsDown 
// 	      if(removeMtCut) hCleaner->Scale(OSWinSidebandRegionDATAWJetsDown/OSWinSidebandRegionMCWJetsDown);//comented 021214
	      if(removeMtCut) hCleaner->Scale((hW_TFDownTight->Integral()*OSWinSignalRegionDATAWJetsDown/OSWinSignalRegionMCWJetsDown)/hCleaner->Integral());//added 021214
	      else hCleaner->Scale(OSWinSignalRegionDATAWJetsDown/hCleaner->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleaner->GetNbinsX() ; ++ibin) hCleaner->SetBinError(ibin, hCleaner->GetBinError(ibin)*pow(LooseToTightRatioDown,0.5));

	      ////fill shape
	      hWLooseBTag_TFDown->Reset();
	      hWLooseBTag_TFDown->Add(hCleaner,1.);

	      //same for fine binning in MSSM
	      //fine binning for MSSM
	      hW_TFDown_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fbDown = 0.;
	      hW_TFDownTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJetDown", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	      hW_TFDownTight_fb->Add(hCleanerfb,1.);

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJetDown_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatioDown_fb = hCleanerfb->Integral()/hW_TFDownTight_fb->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJetsDown 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJetsDown/OSWinSidebandRegionMCWJetsDown);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hW_TFDownTight->Integral()*OSWinSignalRegionDATAWJetsDown/OSWinSignalRegionMCWJetsDown)/hCleanerfb->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJetsDown/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin) hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatioDown_fb,0.5));

	      //fill histograms
	      hW_TFDown_fb->Reset();
	      hW_TFDown_fb->Add(hCleanerfb,1.);
	      
	    }
	    else//inclusive -- take histo directly from Tight
	      {
		hW_TFDown->Reset();
		hW_TFDown->Add(hW_TFDownTight,1.);
		//fine binning not used for inclusive
	      }


	    //}
	  
	  cout << "************** BEGIN W+jets (nominal) normalization using high-Mt sideband *******************" << endl;
	  
	  hExtrapW->Reset();
	  evaluateWextrapolation(mapAllTrees, version_,analysis_, "MC_WJet",RUN, "OS", false, selection_, 
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

	  
	  cout << "************** END W+jets (nominal) normalization using high-Mt sideband *******************" << endl;

	  //For the shape of W in same sign region --> taken from MC
	  float NormSSWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormSSWJets, Error,   Lumi*hltEff_/1000., h1, sbinSS, 1);
	  //normalized to the estimation of W from data

// 	  if(removeMtCut) h1->Scale(SSWinSidebandRegionDATA/SSWinSidebandRegionMC);//comented 021214
	  if(removeMtCut) h1->Scale(SSWinSignalRegionDATA/SSWinSignalRegionMC);//added 021214
	  else h1->Scale( h1->Integral()!=0 ? SSWinSignalRegionDATA/h1->Integral() : 1.0 );
	  hWSS->Add(h1, 1.0);
	  
	  //Tight histogram -- for inclusive and increased bbb errors
	  h1->Reset();
	  float NormWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC_WJet",version_,analysis_, RUN,currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);

	  hWTight->Add(h1,1.);

	  cout<<"hWTight integral : "<<hWTight->Integral()<<endl;

	  //Loose shape -- for nobtag and btag categories
	  h1->Reset();
	  drawHistogram(sbinLtisoPresel,sbinCat, "MC_WJet_LooseTightWCorr",version_,analysis_, RUN, currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbinLtiso, 1);
	  
	  cout<<"h1 integral : "<<h1->Integral()<<endl;

	  ////loose to tight ratio
	  Double_t LooseToTightRatio = h1->Integral()/hWTight->Integral();

	  ////normalize to expectation from data = OSWinSignalRegionDATAWJetsDown 
// 	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);//comented 021214
	  if(removeMtCut) h1->Scale((hWTight->Integral()*OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets)/h1->Integral());//added 021214
	  else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());
	  
	  ////increase the error using loose to tight ratio
	  for(Int_t ibin = 1 ; ibin <= h1->GetNbinsX() ; ++ibin) h1->SetBinError(ibin, h1->GetBinError(ibin)*pow(LooseToTightRatio,0.5));

	  ////fill the histogram
	  hW->Add(h1,1.);

	  cout<<"hW integral : "<<hW->Integral()<<endl;

	  //no-btag
	  if(selection_.find("nobTag")!=string::npos)
	    {
	      float NormWJets_fb = 0.;
	      hW_fb->Reset();
	      hWTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat,"MC_WJet", version_,analysis_, RUN,currentTree, variable, NormWJets_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hWTight_fb->Add(hCleanerfb,1.) ;

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,sbinCat,"MC_WJet_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJets_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtiso, 1);

	      Double_t LooseToTightRatio_fb = hCleanerfb->Integral()/hWTight_fb->Integral();
	      
	      ////normalize to expectation from data = OSWinSignalRegionDATAWJets 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hWTight->Integral()*OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets)/ hCleanerfb->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJets/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin) hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatio_fb,0.5));
	      
	      ////fill shape
	      hW_fb->Add(hCleanerfb,1.);
	    }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)
	    {
	      float NormWJetsBTag = 0.;
	      hWLooseBTag->Reset(); hWLooseBTagTight->Reset();
	      hCleaner->Reset();

	      //tight
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJet", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      hWLooseBTagTight->Add(hCleaner,1.);
	      
	      //loose
	      hCleaner->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJet_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatio = hCleaner->Integral()/hWLooseBTagTight->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJets 
// 	      if(removeMtCut) hCleaner->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);//comented 021214
	      if(removeMtCut) hCleaner->Scale((hWTight->Integral()*OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets)/hCleaner->Integral());//added 021214
	      else hCleaner->Scale(OSWinSignalRegionDATAWJets/hCleaner->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleaner->GetNbinsX() ; ++ibin) hCleaner->SetBinError(ibin, hCleaner->GetBinError(ibin)*pow(LooseToTightRatio,0.5));

	      ////fill shape
	      hWLooseBTag->Reset();
	      hWLooseBTag->Add(hCleaner,1.);

	      cout<<"hWLooseBTag integral : "<<hWLooseBTag->Integral()<<endl;

	      //fine binning for MSSM
	      hW_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fb = 0.;
	      hWTight_fb->Reset();

	      //tight
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,bTagLoose,"MC_WJet", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	      hWTight_fb->Add(hCleanerfb,1.);

	      //loose
	      hCleanerfb->Reset();
	      drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJet_LooseTightWCorr", version_,analysis_, RUN,currentTree, variable, NormWJetsBTag_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtisoInclusive, 1);

	      ////loose to tight ratio
	      Double_t LooseToTightRatio_fb = hCleanerfb->Integral()/hWTight_fb->Integral();

	      ////normalize to expectation from data = OSWinSignalRegionDATAWJets 
// 	      if(removeMtCut) hCleanerfb->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);//comented 021214
	      if(removeMtCut) hCleanerfb->Scale((hWTight->Integral()*OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets)/hCleaner->Integral());//added 021214
	      else hCleanerfb->Scale(OSWinSignalRegionDATAWJets/hCleanerfb->Integral());

	      ////increase the error using loose to tight ratio
	      for(Int_t ibin = 1 ; ibin <= hCleanerfb->GetNbinsX() ; ++ibin) hCleanerfb->SetBinError(ibin, hCleanerfb->GetBinError(ibin)*pow(LooseToTightRatio_fb,0.5));

	      //fill histograms
	      hW_fb->Add(hCleanerfb,1.);
	    }
	  else if(!version_.Contains("JetTauFR"))//nominal = inclusive - shape from tight, normalization from data
	    {
	      hW->Reset(); hWMinusSS->Reset();
	      hW->Add(hWTight, 1.0);
	      if(removeMtCut) hW->Scale(OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets);//added 021214
	      else hW->Scale(OSWinSignalRegionDATAWJets/hWTight->Integral());
	      hWMinusSS->Add(hWTight, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));
	      if(removeMtCut) hWMinusSS->Scale(OSWinSignalRegionDATAWJets/OSWinSignalRegionMCWJets);//added 021214

	      cout<<"Nominal/nobTag integrals : "<<endl;
	      cout<<"hWTight integral : "<<hWTight->Integral()<<endl;
	      cout<<"OSWinSignalRegionDATAWJets: "<<OSWinSignalRegionDATAWJets<<endl;
	      cout<<"OSWinSignalRegionMCWJets: "<<OSWinSignalRegionMCWJets<<endl;
	      cout<<"hW integral : "<<hW->Integral()<<endl;
	      //hWMinusSS->Add(hWTight, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));//commented 261014
	      //if(!USESSBKG) hEWK->Add(hWTight,1.0);//commented 261014
	      //else hEWK->Add(hWMinusSS,1.0);//commented 261014
	    }
	  else if(version_.Contains("JetTauFR"))//JetTauFR measurement
	    {
	      hW->Reset();//commented 261014
	      //cout<<"WJets from MC"<<endl;
	      float NormWJets_NoOS = 0; hCleaner->Reset();//commented 261014
	      drawHistogram(sbinPresel,sbinCat,"MC_WJet", version_,analysis_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);//commented 261014

	      hW->Add(hCleaner,hWTight->Integral()/hCleaner->Integral());//commented 261014
	      hEWK->Add(hCleaner,hWTight->Integral()/hCleaner->Integral());//commented 261014
	      //cout<<"Scale WJets "<<hWTight->Integral()/hCleaner->Integral()<<endl;
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
	    /*
	      if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTauShape, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      else
	    */
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
	      {
		cout<<"Zmm checks = "<<endl;
		cout<<"sbinPresel = "<<sbinPresel<<endl;
		cout<<"sbinCat = "<<sbinCat<<endl;
		cout<<"sbin = "<<sbin<<endl;
		cout<<"Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000. = "<<Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000.<<endl;
		drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormDYMutoTau, Error,   Lumi*lumiCorrFactor*MutoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
		cout<<"Integral = "<<h1->Integral()<<endl;
	      }
	    hZmm->Add(h1, 1.0); //hZmm->Sumw2();
	    hZfakes->Add(h1,1.0); //hZfakes->Sumw2();
	    //hEWK->Add(h1,1.0);

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

	  //Adding DYToTauTauLL
	  //Z->tautau->mu+mu->mu+th (fake from mu)
	  cout<<"FILLING DYToTauTauLL"<<endl;
	  cout<<"Integral hZmm before = "<<hZmm->Integral()<<endl;
	  
	  hCleaner->Reset(); float NormDYToTauTauLL = 0;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, mapAllTrees["DYToTauTauLL"], variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	  //Mapping it to ZL
	  if(hCleaner) { 
	    hZmm->Add(hCleaner, 1.0);
	    hZfakes->Add(hCleaner,1.0);
	    hEWK->Add(hCleaner,1.0);  
	  } 
	  
	  //fine binning for MSSM
	  hCleanerfb->Reset();hZmm_fb->Reset(); float NormDYToTauTauLL_fb = 0;
	  drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauLL"], variable, NormDYToTauTauLL_fb, Error, Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	  //Mapping it to ZL
	  if(hCleanerfb)
	    {
	      hZmm_fb->Add(hCleanerfb, 1.0);
	    }
	  
	  cout<<"Integral hZmm after = "<<hZmm->Integral()<<endl;
	}
	// 	else if(currentName.Contains("DYToTauTauLL"))
	// 	  {
		    
	// 	  }
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
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    if(hCleaner) {
	      hZmj->Add(hCleaner, 1.0);
	      hZfakes->Add(hCleaner,1.0);
	      hEWK->Add(hCleaner,1.0);
	    }
	  }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){

	    cout<<"//////////////////////"<<endl;
	    cout<<"//////CUT FOR ZJ///////"<<endl;

	    float NormDYJtoTau = 0.;  
	    hCleaner->Reset();
	    if(useZDataMC) drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinInclusive, 1); 
	    else drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1); 
	    cout<<"hCleaner integral = "<<hCleaner->Integral()<<endl;

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
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    if(hCleaner) {
	      hZmj->Add(hCleaner, 1.0);
	      hZfakes->Add(hCleaner,1.0);
	      hEWK->Add(hCleaner,1.0);
	    }
	    
	    cout<<"//////////////////////"<<endl;

	    //fine binning for MSSM   
	    hCleanerfb->Reset();hZmj_fb->Reset(); float NormDYJtoTau_fb = 0.;  
	    drawHistogram(sbinPresel,bTagLoose,"MC", version_,analysis_, RUN,currentTree, variable, NormDYJtoTau_fb, Error,  Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	    hZmj_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());

	    //Add ZTTJ component to ZJ
	    NormDYTTJtoTau = 0; hCleanerfb->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
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
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
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
	      drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau_fb, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
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

	  if(StudyQCDOlivier)
	    {
	      //Olivier QCD Study

	      //SS, lepton anti-iso, tau_h tight iso
	      //tightestHPSMVA3oldDMwLTWP>2
	      TCut sbinSSaIso_TightIso_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_TightIso_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_TightIso_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_TightIso_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_TightIso_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_TightIso_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_TightIso_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_TightTauIsoMVA, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_TightIso_Olivier,
			  sbinCat_TightIso_Olivier,
			  sbinPZetaRelSSaIso_TightIso_Olivier, pZ_TightIso_Olivier, apZ_TightIso_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_TightIso_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_Olivier, sbinPZetaRelSSaIso_TightIso_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_TightIso_Olivier, sbinCat_TightIso_Olivier, sbinaIsoPresel_TightIso_Olivier, sbinaIsoPresel_TightIso_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //SS, lepton anti-iso, tau_h loose non tight iso
	      // tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3
	      TCut sbinSSaIso_LooseNonTightIso_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_LooseNonTightIso_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_LooseNonTightIso_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_LooseNonTightIso_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_LooseNonTightIso_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_LooseNonTightIso_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_LooseNonTightIso_Olivier,
			  sbinCat_LooseNonTightIso_Olivier,
			  sbinPZetaRelSSaIso_LooseNonTightIso_Olivier, pZ_LooseNonTightIso_Olivier, apZ_LooseNonTightIso_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_Olivier, sbinPZetaRelSSaIso_LooseNonTightIso_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_LooseNonTightIso_Olivier, sbinCat_LooseNonTightIso_Olivier, sbinaIsoPresel_LooseNonTightIso_Olivier, sbinaIsoPresel_LooseNonTightIso_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //tau eta < 1.2
	      //SS, lepton anti-iso, tau_h tight iso
	      //tightestHPSMVA3oldDMwLTWP>2
	      TCut sbinSSaIso_TightIso_eta_central_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_TightIso_eta_central_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_TightIso_eta_central_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_TightIso_eta_central_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_TightIso_eta_central_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_TightIso_eta_central_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_TightIso_eta_central_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_central, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_TightIso_eta_central_Olivier,
			  sbinCat_TightIso_eta_central_Olivier,
			  sbinPZetaRelSSaIso_TightIso_eta_central_Olivier, pZ_TightIso_eta_central_Olivier, apZ_TightIso_eta_central_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_central_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_TightIso_eta_central_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_central_Olivier, sbinPZetaRelSSaIso_TightIso_eta_central_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_TightIso_eta_central_Olivier, sbinCat_TightIso_eta_central_Olivier, sbinaIsoPresel_TightIso_eta_central_Olivier, sbinaIsoPresel_TightIso_eta_central_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //SS, lepton anti-iso, tau_h loose non tight iso
	      // tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3
	      TCut sbinSSaIso_LooseNonTightIso_eta_central_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_LooseNonTightIso_eta_central_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_LooseNonTightIso_eta_central_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_LooseNonTightIso_eta_central_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_LooseNonTightIso_eta_central_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_central_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_LooseNonTightIso_eta_central_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)<1.2))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_central, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_LooseNonTightIso_eta_central_Olivier,
			  sbinCat_LooseNonTightIso_eta_central_Olivier,
			  sbinPZetaRelSSaIso_LooseNonTightIso_eta_central_Olivier, pZ_LooseNonTightIso_eta_central_Olivier, apZ_LooseNonTightIso_eta_central_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_central_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_central_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_central_Olivier, sbinPZetaRelSSaIso_LooseNonTightIso_eta_central_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_LooseNonTightIso_eta_central_Olivier, sbinCat_LooseNonTightIso_eta_central_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_central_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_central_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //tau eta in 1.2, 1.7
	      //SS, lepton anti-iso, tau_h tight iso
	      //tightestHPSMVA3oldDMwLTWP>2
	      TCut sbinSSaIso_TightIso_eta_medium_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_TightIso_eta_medium_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_TightIso_eta_medium_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_TightIso_eta_medium_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_TightIso_eta_medium_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_TightIso_eta_medium_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_TightIso_eta_medium_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_medium, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_TightIso_eta_medium_Olivier,
			  sbinCat_TightIso_eta_medium_Olivier,
			  sbinPZetaRelSSaIso_TightIso_eta_medium_Olivier, pZ_TightIso_eta_medium_Olivier, apZ_TightIso_eta_medium_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_medium_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_TightIso_eta_medium_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_medium_Olivier, sbinPZetaRelSSaIso_TightIso_eta_medium_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_TightIso_eta_medium_Olivier, sbinCat_TightIso_eta_medium_Olivier, sbinaIsoPresel_TightIso_eta_medium_Olivier, sbinaIsoPresel_TightIso_eta_medium_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //SS, lepton anti-iso, tau_h loose non tight iso
	      // tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3
	      TCut sbinSSaIso_LooseNonTightIso_eta_medium_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_LooseNonTightIso_eta_medium_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_LooseNonTightIso_eta_medium_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_LooseNonTightIso_eta_medium_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_LooseNonTightIso_eta_medium_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_medium_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_LooseNonTightIso_eta_medium_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.2&&abs(etaL2)<1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_medium, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_LooseNonTightIso_eta_medium_Olivier,
			  sbinCat_LooseNonTightIso_eta_medium_Olivier,
			  sbinPZetaRelSSaIso_LooseNonTightIso_eta_medium_Olivier, pZ_LooseNonTightIso_eta_medium_Olivier, apZ_LooseNonTightIso_eta_medium_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_medium_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_medium_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_medium_Olivier, sbinPZetaRelSSaIso_LooseNonTightIso_eta_medium_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_LooseNonTightIso_eta_medium_Olivier, sbinCat_LooseNonTightIso_eta_medium_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_medium_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_medium_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //tau eta > 1.7
	      //SS, lepton anti-iso, tau_h tight iso
	      //tightestHPSMVA3oldDMwLTWP>2
	      TCut sbinSSaIso_TightIso_eta_forward_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_TightIso_eta_forward_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_TightIso_eta_forward_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_TightIso_eta_forward_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_TightIso_eta_forward_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_TightIso_eta_forward_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_TightIso_eta_forward_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>2))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_forward, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_TightIso_eta_forward_Olivier,
			  sbinCat_TightIso_eta_forward_Olivier,
			  sbinPZetaRelSSaIso_TightIso_eta_forward_Olivier, pZ_TightIso_eta_forward_Olivier, apZ_TightIso_eta_forward_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_forward_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_TightIso_eta_forward_Olivier, sbinPZetaRelSSaIsoInclusive_TightIso_eta_forward_Olivier, sbinPZetaRelSSaIso_TightIso_eta_forward_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_TightIso_eta_forward_Olivier, sbinCat_TightIso_eta_forward_Olivier, sbinaIsoPresel_TightIso_eta_forward_Olivier, sbinaIsoPresel_TightIso_eta_forward_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      //SS, lepton anti-iso, tau_h loose non tight iso
	      // tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3
	      TCut sbinSSaIso_LooseNonTightIso_eta_forward_Olivier = "(((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(((MtLeg1MVA)<30.000000)))&&(HLTx==1&& HLTmatch==1)";
	      TCut sbinCat_LooseNonTightIso_eta_forward_Olivier = "etaL1<999";
	      TCut sbinPZetaRelSSaIso_LooseNonTightIso_eta_forward_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&& HLTmatch==1)";
	      TCut pZ_LooseNonTightIso_eta_forward_Olivier = "((MtLeg1MVA)<30.000000)";
	      TCut apZ_LooseNonTightIso_eta_forward_Olivier = "((MtLeg1MVA)>70.000000)";
	      TCut sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_forward_Olivier = "((((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(diTauCharge!=0))&&(HLTx==1&&HLTmatch==1)";
	      TCut sbinaIsoPresel_LooseNonTightIso_eta_forward_Olivier = "(((((((((((ptL1>20)&&(ptL1>-999))&&(isPFMuon && isTightMuon))&&(ptL2>30&&abs(etaL2)>1.7))&&(tightestHPSMVA3oldDMwLTWP>0 && tightestHPSMVA3oldDMwLTWP< 3))&&(decayModeFindingOldDM>0.5))&&(tightestAntiMuMVAWP>1))&&(tightestAntiECutWP > 0))&&(combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50))&&(muFlag!=1 && vetoEventOld==0))&&(diTauNSVfitMass>-999))&&(HLTx==1&&HLTmatch==1)";

	      evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_forward, hCleaner, true, "SS", false, removeMtCut, selection_, 
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
			  sbinSSaIso_LooseNonTightIso_eta_forward_Olivier,
			  sbinCat_LooseNonTightIso_eta_forward_Olivier,
			  sbinPZetaRelSSaIso_LooseNonTightIso_eta_forward_Olivier, pZ_LooseNonTightIso_eta_forward_Olivier, apZ_LooseNonTightIso_eta_forward_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_forward_Olivier, 
			  sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_forward_Olivier, sbinPZetaRelSSaIsoInclusive_LooseNonTightIso_eta_forward_Olivier, sbinPZetaRelSSaIso_LooseNonTightIso_eta_forward_Olivier, // useless arguments
			  vbfLoose, oneJet, zeroJet, sbinCat_LooseNonTightIso_eta_forward_Olivier, sbinCat_LooseNonTightIso_eta_forward_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_forward_Olivier, sbinaIsoPresel_LooseNonTightIso_eta_forward_Olivier, true, true);
	      hCleaner->Reset();
	      hExtrapNadir->Reset();

	      // 	  cout<<"*****************"<<endl;
	      // 	  cout<<"Selection from Nadir (adapted) = "<<endl;
	      // 	  cout<<"       sbinSSaIso = "<<sbinSSaIso<<endl;
	      // 	  cout<<"       sbinCatNadir = "<<sbinCatNadir<<endl;
	      // 	  cout<<"       sbinPZetaRelSSaIso = "<<sbinPZetaRelSSaIso<<endl;
	      // 	  cout<<"       pZ = "<<pZ<<endl;
	      // 	  cout<<"       apZ = "<<apZ<<endl;
	      // 	  cout<<"       sbinPZetaRelSSaIsoInclusive = "<<sbinPZetaRelSSaIsoInclusive<<endl;
	      // 	  cout<<"       sbinCatNadir = "<<sbinCatNadir<<endl;
	      // 	  cout<<"       sbinaIsoPresel = "<<sbinaIsoPresel<<endl;
	      // 	  cout<<"*****************"<<endl;

	    }
	    
	  if(StudyQCD) {
	    evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hDataSSAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

	    evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hDataSSIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

	    evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hDataOSAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

	    evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hDataOSIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

	    if(version_.Contains("Soft"))
	      drawHistogram(sbinaIsoLtisoPresel,vbfLooseQCD,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1);
	    else
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
	  //else if(selection_.find("boostHighlowhiggs")!=string::npos || selection_.find("boostMedium")!=string::npos || selection_.find("boostHigh")!=string::npos){
	  else if(selection_.find("boostHighlowhiggs")!=string::npos || selection_.find("boostHigh")!=string::npos){
	    drawHistogram(sbinaIsoLtisoPresel,sbinCat,"Data", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }
	  //else if(selection_.find("novbfHigh")!=string::npos || selection_.find("novbfMedium")!=string::npos || selection_.find("boost")!=string::npos){
	  else if(selection_.find("novbfHigh")!=string::npos || selection_.find("boost")!=string::npos){
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
	  //else if(selection_.find("novbfLow")!=string::npos) {
	  else if(selection_.find("novbfLow")!=string::npos || selection_.find("novbfMedium")!=string::npos ) {
	    TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
	    float dummy1 = 0.;      
	    evaluateQCD(mapAllTrees, version_,analysis_, "MC_WJet",RUN, hDataAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
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

	    TCut temp_sbinaIsoLtisoPresel = sbinaIsoLtisoPresel && "MtLeg1MVA<30.";
	    TCut temp_bTagLoose =  bTagLoose && "MtLeg1MVA<30.";
	    TCut temp_sbinSSaIsoLtiso = sbinSSaIsoLtiso && "MtLeg1MVA<30.";

	    //Data anti-loose lepton iso, tau-iso, bTag loose
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorr", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1); 
	    cout<<"Integral before = "<<hDataAntiIsoLooseTauIso->Integral()<<endl;
	    hDataAntiIsoLooseTauIso->Add(hCleaner); 

	    //QCDCorrUp
	    hCleaner->Reset();
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorrUp", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1); 
	    hDataAntiIsoLooseTauIsoCorrUp->Add(hCleaner); 

	    //QCDCorrDown
	    hCleaner->Reset();
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorrDown", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1); 
	    hDataAntiIsoLooseTauIsoCorrDown->Add(hCleaner); 


	    //Same --> fine binning
	    hCleanerfb->Reset(); hQCD_fb->Reset(); float NormData_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorr", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    cout<<"Integral before fb = "<<hQCD_fb->Integral()<<endl;
	    hQCD_fb->Add(hCleanerfb);

	    hCleanerfb->Reset(); hQCD_CorrUp_fb->Reset(); float NormDataCorrUp_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorrUp", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    hQCD_CorrUp_fb->Add(hCleanerfb);

	    hCleanerfb->Reset(); hQCD_CorrDown_fb->Reset(); float NormDataCorrDown_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_bTagLoose,"DataQCDCorrDown", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    hQCD_CorrDown_fb->Add(hCleanerfb);

	    //Normalize to evaluate QCD output: the shape is taken from anti-loose, bTag loose
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    hQCD_fb->Scale(hQCD->Integral()/hQCD_fb->Integral());

	    hDataAntiIsoLooseTauIsoQCDCorrUp->Add(hDataAntiIsoLooseTauIsoCorrUp, hQCD->Integral()/hDataAntiIsoLooseTauIsoCorrUp->Integral());
	    hQCD_CorrUp_fb->Scale(hQCD->Integral()/hQCD_CorrUp_fb->Integral());

	    hDataAntiIsoLooseTauIsoQCDCorrDown->Add(hDataAntiIsoLooseTauIsoCorrDown, hQCD->Integral()/hDataAntiIsoLooseTauIsoCorrDown->Integral());
	    hQCD_CorrDown_fb->Scale(hQCD->Integral()/hQCD_CorrDown_fb->Integral());
	    
	  }
	  else{//no btag and inclusive

	    TCut temp_sbinaIsoLtisoPresel = sbinaIsoLtisoPresel && "MtLeg1MVA<30.";
	    TCut temp_sbinCat =  sbinCat && "MtLeg1MVA<30.";
	    TCut temp_sbinSSaIsoLtiso = sbinSSaIsoLtiso && "MtLeg1MVA<30.";

	    //Data anti-loose lepton iso, loose tau-iso
	    hCleaner->Reset();
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorr", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1);
	    cout<<"Integral before = "<<hDataAntiIsoLooseTauIso->Integral()<<endl;
	    hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD);

	    //QCDCorrUp
	    hCleaner->Reset();
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorrUp", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1);
	    hDataAntiIsoLooseTauIsoCorrUp->Add(hCleaner, SSIsoToSSAIsoRatioQCD);	 

	    //QCDCorrDown
	    hCleaner->Reset();
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorrDown", version_,analysis_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, temp_sbinSSaIsoLtiso ,1);
	    hDataAntiIsoLooseTauIsoCorrDown->Add(hCleaner, SSIsoToSSAIsoRatioQCD);	 	    

	    //Normalize to evaluate QCD output: the shape is taken from anti-loose, bTag loose
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    hDataAntiIsoLooseTauIsoQCDCorrUp->Add(hDataAntiIsoLooseTauIsoCorrUp, hQCD->Integral()/hDataAntiIsoLooseTauIsoCorrUp->Integral());
	    hDataAntiIsoLooseTauIsoQCDCorrDown->Add(hDataAntiIsoLooseTauIsoCorrDown, hQCD->Integral()/hDataAntiIsoLooseTauIsoCorrDown->Integral());

	    //Finebins QCD
	    hCleanerfb->Reset(); hQCD_fb->Reset(); float NormData_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorr", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    cout<<"Integral before fb = "<<hQCD_fb->Integral()<<endl;
	    hQCD_fb->Add(hCleanerfb);
	    hQCD_fb->Scale(hQCD->Integral()/hQCD_fb->Integral());

	    //Finebins QCD CorrUp
	    hCleanerfb->Reset(); hQCD_CorrUp_fb->Reset(); float NormDataCorrUp_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorrUp", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    hQCD_CorrUp_fb->Add(hCleanerfb);
	    hQCD_CorrUp_fb->Scale(hQCD->Integral()/hQCD_CorrUp_fb->Integral());

	    //Finebins QCD CorrDown
	    hCleanerfb->Reset(); hQCD_CorrDown_fb->Reset(); float NormDataCorrDown_fb = 0.; 
	    drawHistogram(temp_sbinaIsoLtisoPresel,temp_sbinCat,"DataQCDCorrDown", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, temp_sbinSSaIsoLtiso ,1);
	    hQCD_CorrDown_fb->Add(hCleanerfb);
	    hQCD_CorrDown_fb->Scale(hQCD->Integral()/hQCD_CorrDown_fb->Integral());
	  }
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
		{
		  //cout<<"prod = "<<nameProd[iP]<<endl;
		  //cout<<"mass = "<<nameMasses[iM]<<endl;
		  hSignal[iP][iM]->Add(h1,1.0);
		  //cout<<"analysis_ = "<<analysis_<<endl;
		  //cout<<"histo mean = "<<h1->GetMean()<<endl;
		}
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

	    for(int iP=0 ; iP<nProd ; iP++)
	      {
		//HERE ADD: CMS_eff_t_mssmHigh_mutau_8TeV for SM
		TString ProcessName2(nameProd[iP]+nameMasses[iM]);

		if(currentName==ProcessName2)
		  {
		    h1->Reset(); float NormSMEffTUp = 0.;
		    drawHistogram(sbinPresel,sbinCat,"MCSMEffTUp", version_, analysis_,RUN,currentTree, variable, NormSMEffTUp, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
		    hSMEffTUp[iP][iM]->Add(h1,1.0);
			    
		    h1->Reset(); float NormSMEffTDown = 0.;
		    drawHistogram(sbinPresel,sbinCat,"MCSMEffTDown", version_, analysis_,RUN,currentTree, variable, NormSMEffTDown, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
		    hSMEffTDown[iP][iM]->Add(h1,1.0);
		  }
	      }
	  }
	  
	  for(int iP=0 ; iP<nProdWW ; iP++)
	    for(int iM=0 ; iM<nMassesWW ; iM++)
	      if(currentName.Contains(nameProdWW[iP]+nameMassesWW[iM]))
		hSignalWW[iP][iM]->Add(h1,1.0);

	  if(MSSM) {
	    if(currentName.Contains("SUSY"))
	      {
		//select events within 30% of Higgs mass
		TString sampleName = currentName;
		if(sampleName.Contains("SUSYGGH"))sampleName.ReplaceAll("SUSYGGH", "");
		else if(sampleName.Contains("SUSYBBH"))sampleName.ReplaceAll("SUSYBBH", "");
		float mA = atof(sampleName.Data());
		TCut HWidth(Form("genVMass > 0.7*%f && genVMass < 1.3*%f", mA, mA));  

		float NormSign = 0.; 
		drawHistogram(sbinPresel,sbinCat,"MCSUSY", version_,analysis_, RUN,currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);

		h2->Reset();
		NormSign = 0.; 
		drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN,currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h2, (sbin&&HWidth), 1);

		for(int iP=0 ; iP<nProdS ; iP++)
		  {
		    for(int iM=0 ; iM<nMassesS ; iM++)
		      {			
			TString ProcessName("SUSY"+nameProdS[iP]+nameMassesS[iM]);
			if(currentName==ProcessName)
			  {
			    //cout<<"Standard histogram = "<<h1->Integral()<<endl;
			    hSusy[iP][iM]->Add(h1,1.0);
			    hSusyNoWeight[iP][iM]->Add(h2,1.0);
			  }
		      }
		  }

		for(int iM=0 ; iM<nMassesS ; iM++)
		  {
		    TString ProcessName("SUSYGGH"+nameMassesS[iM]);
		    if(currentName==ProcessName)
		      {
			h1->Reset(); float NormSignUp = 0.;
			drawHistogram(sbinPresel,sbinCat,"MCSUSYGGHUp", version_, analysis_,RUN,currentTree, variable, NormSignUp, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			hSUSYGGHUp[iM]->Add(h1,1.0);

			h1->Reset(); float NormSignDown = 0.;
			drawHistogram(sbinPresel,sbinCat,"MCSUSYGGHDown", version_, analysis_,RUN,currentTree, variable, NormSignDown, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			hSUSYGGHDown[iM]->Add(h1,1.0);

			//QCD scale
			h1->Reset(); float NormSignScaleUp = 0.;
			drawHistogram(sbinPresel,sbinCat,"MCSUSYGGHScaleUp", version_, analysis_, RUN,currentTree, variable, NormSignUp, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			hSUSYGGHScaleUp[iM]->Add(h1,1.0);
			
			h1->Reset(); float NormSignScaleDown = 0.;
			drawHistogram(sbinPresel,sbinCat,"MCSUSYGGHScaleDown", version_, analysis_, RUN,currentTree, variable, NormSignDown, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			hSUSYGGHScaleDown[iM]->Add(h1,1.0);
			
		      }//End SUSY GGH

		    for(int iP=0 ; iP<nProdS ; iP++)
		      {
			//HERE ADD: CMS_eff_t_mssmHigh_mutau_8TeV
			TString ProcessName2("SUSY"+nameProdS[iP]+nameMassesS[iM]);

			if(currentName==ProcessName2)
			  {
			    h1->Reset(); float NormSusyEffTUp = 0.;
			    drawHistogram(sbinPresel,sbinCat,"MCSUSYEffTUp", version_, analysis_,RUN,currentTree, variable, NormSusyEffTUp, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			    hSusyEffTUp[iP][iM]->Add(h1,1.0);
			    
			    h1->Reset(); float NormSusyEffTDown = 0.;
			    drawHistogram(sbinPresel,sbinCat,"MCSUSYEffTDown", version_, analysis_,RUN,currentTree, variable, NormSusyEffTDown, Error,   Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);
			    hSusyEffTDown[iP][iM]->Add(h1,1.0);
			  }
		      }
		  }//End Masses

		//TH1F* histoSusy =  (mapSUSYhistos.find( currentName ))->second;
		//histoSusy->Add(h1,1.0);
		//histoSusy->SetLineWidth(2);

	      }//End SUSY
	  }//End MSSM
	}//End MC Signal
      }
      else{
	if(!currentName.Contains("TTbarEmbedded")){
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
	    //remove TTbar embedded contamination
	    h1->Reset();
	    float NormTTjetsEmb = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJets",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
	    hTTbEmb->Add(h1, 1.0);
	    cout<<"TTbarEmbedded (passing here): "<<hTTbEmb->Integral()<<endl;
	    hDataEmb->Add(hTTbEmb, -1.0);

	    //Add ZTTLL
	    float NormDYToTauTauLL = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_,analysis_, RUN, mapAllTrees["DYToTauTauLL"], variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hDataEmb->Add(hCleaner, 1.0);
	  }
	  else{
	    //new method Oct14	    //ZTT Embedded_cat_i = Embedded data_cat_i*weight  - TTbar MC Embedded_cat_i
	    //weight = (NormZTTInclusive+NormTTjetsEmbInclusive)/NormEmbedInclusive

// 	    cout<<"---------------"<<endl;
// 	    cout<<"TEST TTbar Emb Olivier = "<<endl;


	    // 	    cout<<"OLD = sbin Cat for Emb is = "<<sbinCat<<endl;
	    // 	    cout<<"OLD = sbinEmbeddingPresel = "<<sbinEmbeddingPresel<<endl;
	    // 	    cout<<""<<endl;
	    // 	    cout<<"NEW = sbinCatIncl"<<sbinCatIncl<<endl;
// 	    cout<<"NEW sbin inclusive for Emb is = "<<sbinPZetaRelEmbeddingInclusive<<endl;


	    
	    TH1F* temp_histo_counter = new TH1F("temp_histo_counter","temp_histo_counter",1,-0.5,+0.5);
	    temp_histo_counter->Reset();

	    cout<<"     1) Compute inclusive embedded data (no mT cut)"<<endl;
	    float NormEmbedInclusive = 0.;
	    drawHistogram(sbinPZetaRelEmbeddingInclusive,sbinCatIncl,"Embed", version_,analysis_, RUN,currentTree, "0", NormEmbedInclusive,  Error, 1.0 , temp_histo_counter, sbinPZetaRelEmbeddingInclusive, 1);
// 	    drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"Embed", version_,analysis_, RUN,currentTree, "0", NormEmbedInclusive,  Error, 1.0 , temp_histo_counter,  sbinEmbeddingPresel, 1);
// 	    
	    cout<<"Embedded integral = "<<temp_histo_counter->Integral()<<endl;
	    temp_histo_counter->Reset();
	    h1->Reset();

	    cout<<"     2) Compute inclusive ZTT MC (no mT cut)"<<endl;
	    float NormZTTInclusive = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCatIncl,"MC", version_,analysis_, RUN,mapAllTrees["DYToTauTau"], "0", NormZTTInclusive,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., temp_histo_counter, sbinPZetaRelInclusive, 1);
	    cout <<"ZTT MC = "<<temp_histo_counter->Integral()<<endl;
	    temp_histo_counter->Reset();
	    h1->Reset();	    

	    cout<<"     3) Compute inclusive TTbar MC Embedded (no mT cut)"<<endl;
	    float NormTTjetsEmbInclusive = 0.;
	    drawHistogram(sbinPZetaRelEmbeddingInclusive,sbinCatIncl, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, "0", NormTTjetsEmbInclusive,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., temp_histo_counter, sbinPZetaRelEmbeddingInclusive, 1);
// 	    drawHistogram(sbinEmbeddingPresel,sbinCatIncl, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, "0", NormTTjetsEmbInclusive,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., temp_histo_counter, sbinEmbeddingPresel, 1);
	    cout <<"TTbar MC Embed = "<<temp_histo_counter->Integral()<<endl;
	    temp_histo_counter->Reset();
	    h1->Reset();

	    float Weight_From_ZTT_MC = (NormZTTInclusive)/(NormEmbedInclusive-NormTTjetsEmbInclusive) ;
	    cout<<"Weight_From_ZTT_MC=(NormZTTInclusive)/(NormEmbedInclusive-NormTTjetsEmbInclusive) = "<<"("<<NormZTTInclusive<<")"<<"/("<<NormEmbedInclusive<<"-"<<NormTTjetsEmbInclusive<<")"<<endl;
	    cout<<"Weight_From_ZTT_MC = "<<Weight_From_ZTT_MC<<endl; 

	    cout<<"     4) Compute category embedded data"<<endl;
	    float NormEmbedNew = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbedNew,  Error, 1.0 , h1,  sbinEmbedding  ,1);
	    cout<<"Embedded category integral = "<<h1->Integral()<<endl;
	    cout<<"Scale embedded: "<<endl;
	    TH1F* h_Emb_category = (TH1F*)h1->Clone("h_Emb_category");
	    h1->Reset();

	    cout<<"     5) Compute category TTbar MC Embedded"<<endl;
	    float NormTTjetsEmbNew = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
	    cout <<"TTbar MC Embed category integral = "<<h1->Integral()<<endl;
	    cout <<"Number of bins = "<<h1->GetNbinsX()<<endl;
	    TH1F* h_TTbarMC_Emb_category = (TH1F*)h1->Clone("h_TTbarMC_Emb_category");
	    h1->Reset(); 


	    cout<<"     6) Compute final number: ZTT Embedded_cat_i = (Embedded data_cat_i  - TTbar MC Embedded_cat_i)*weight"<<endl;
	    TH1F* h_ZTT_Emb_category = (TH1F*)h_Emb_category->Clone("h_ZTT_Emb_category");
	    EvaluateZTTEmb(NormEmbedInclusive, NormZTTInclusive, NormTTjetsEmbInclusive, h_ZTT_Emb_category, h_TTbarMC_Emb_category);

	    cout<<" ZTT Embedded_cat_i = "<<(h_Emb_category->Integral()-h_TTbarMC_Emb_category->Integral())*Weight_From_ZTT_MC<<endl;
	    cout<<"                    = "<<h_ZTT_Emb_category->Integral()<<endl;

	    hDataEmb_NotSubtracted = (TH1F*)h_Emb_category->Clone("hDataEmb_NotSubtracted");
	    hDataEmb_NotSubtracted->Scale(Weight_From_ZTT_MC);
	    h_TTbarMC_Emb_category->Scale(Weight_From_ZTT_MC);
	    hTTbEmb = (TH1F*)h_TTbarMC_Emb_category->Clone("hTTbEmb");
	    hDataEmb = (TH1F*)h_ZTT_Emb_category->Clone("hDataEmb");

	    Double_t Norm = hDataEmb->Integral();
	    cout<<"hDataEmb->Integral() = "<<hDataEmb->Integral()<<endl;
	    

	    //decay modes
	    TCut decayModeRequirement ;

	    //1-prong + 0Pi0
	    float NormEmbedNew_1Prong0Pi0 = 0.;
	    decayModeRequirement = "decayMode==0";
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbedNew_1Prong0Pi0,  Error, 1.0 , h1,  sbinEmbedding&&decayModeRequirement  ,1);
	    TH1F* h_Emb_category_1Prong0Pi0 = (TH1F*)h1->Clone("h_Emb_category_1Prong0Pi0");
	    h1->Reset();

	    float NormTTjetsEmbNew_1Prong0Pi0 = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew_1Prong0Pi0,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding&&decayModeRequirement, 1);
	    TH1F* h_TTbarMC_Emb_category_1Prong0Pi0 = (TH1F*)h1->Clone("h_TTbarMC_Emb_category_1Prong0Pi0");
	    h1->Reset(); 

	    TH1F* h_ZTT_Emb_category_1Prong0Pi0 = (TH1F*)h_Emb_category_1Prong0Pi0->Clone("h_ZTT_Emb_category_1Prong0Pi0");
	    EvaluateZTTEmb(NormEmbedInclusive, NormZTTInclusive, NormTTjetsEmbInclusive, h_ZTT_Emb_category_1Prong0Pi0, h_TTbarMC_Emb_category_1Prong0Pi0);
	    hDataEmb_NotSubtracted_1Prong0Pi0 = (TH1F*)h_Emb_category_1Prong0Pi0->Clone("hDataEmb_NotSubtracted_1Prong0Pi0");
	    hDataEmb_NotSubtracted_1Prong0Pi0->Scale(Weight_From_ZTT_MC);
	    h_TTbarMC_Emb_category_1Prong0Pi0->Scale(Weight_From_ZTT_MC);
	    hTTbEmb_1Prong0Pi0 = (TH1F*)h_TTbarMC_Emb_category_1Prong0Pi0->Clone("hTTbEmb_1Prong0Pi0");
	    hDataEmb_1Prong0Pi0 = (TH1F*)h_ZTT_Emb_category_1Prong0Pi0->Clone("hDataEmb_1Prong0Pi0");

	    //1-prong + 1Pi0
	    float NormEmbedNew_1Prong1Pi0 = 0.;
	    decayModeRequirement = "decayMode==1";
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbedNew_1Prong1Pi0,  Error, 1.0 , h1,  sbinEmbedding&&decayModeRequirement  ,1);
	    TH1F* h_Emb_category_1Prong1Pi0 = (TH1F*)h1->Clone("h_Emb_category_1Prong1Pi0");
	    h1->Reset();

	    float NormTTjetsEmbNew_1Prong1Pi0 = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew_1Prong1Pi0,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding&&decayModeRequirement, 1);
	    TH1F* h_TTbarMC_Emb_category_1Prong1Pi0 = (TH1F*)h1->Clone("h_TTbarMC_Emb_category_1Prong1Pi0");
	    h1->Reset(); 

	    TH1F* h_ZTT_Emb_category_1Prong1Pi0 = (TH1F*)h_Emb_category_1Prong1Pi0->Clone("h_ZTT_Emb_category_1Prong1Pi0");
	    EvaluateZTTEmb(NormEmbedInclusive, NormZTTInclusive, NormTTjetsEmbInclusive, h_ZTT_Emb_category_1Prong1Pi0, h_TTbarMC_Emb_category_1Prong1Pi0);
	    hDataEmb_NotSubtracted_1Prong1Pi0 = (TH1F*)h_Emb_category_1Prong1Pi0->Clone("hDataEmb_NotSubtracted_1Prong1Pi0");
	    hDataEmb_NotSubtracted_1Prong1Pi0->Scale(Weight_From_ZTT_MC);
	    h_TTbarMC_Emb_category_1Prong1Pi0->Scale(Weight_From_ZTT_MC);
	    hTTbEmb_1Prong1Pi0 = (TH1F*)h_TTbarMC_Emb_category_1Prong1Pi0->Clone("hTTbEmb_1Prong1Pi0");
	    hDataEmb_1Prong1Pi0 = (TH1F*)h_ZTT_Emb_category_1Prong1Pi0->Clone("hDataEmb_1Prong1Pi0");

	    //3-prong
	    float NormEmbedNew_3Prong = 0.;
	    decayModeRequirement = "decayMode==4";
	    drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbedNew_3Prong,  Error, 1.0 , h1,  sbinEmbedding&&decayModeRequirement  ,1);
	    TH1F* h_Emb_category_3Prong = (TH1F*)h1->Clone("h_Emb_category_3Prong");
	    h1->Reset();

	    float NormTTjetsEmbNew_3Prong = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew_3Prong,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding&&decayModeRequirement, 1);
	    TH1F* h_TTbarMC_Emb_category_3Prong = (TH1F*)h1->Clone("h_TTbarMC_Emb_category_3Prong");
	    h1->Reset(); 

	    TH1F* h_ZTT_Emb_category_3Prong = (TH1F*)h_Emb_category_3Prong->Clone("h_ZTT_Emb_category_3Prong");
	    EvaluateZTTEmb(NormEmbedInclusive, NormZTTInclusive, NormTTjetsEmbInclusive, h_ZTT_Emb_category_3Prong, h_TTbarMC_Emb_category_3Prong);
	    hDataEmb_NotSubtracted_3Prong = (TH1F*)h_Emb_category_3Prong->Clone("hDataEmb_NotSubtracted_3Prong");
	    hDataEmb_NotSubtracted_3Prong->Scale(Weight_From_ZTT_MC);
	    h_TTbarMC_Emb_category_3Prong->Scale(Weight_From_ZTT_MC);
	    hTTbEmb_3Prong = (TH1F*)h_TTbarMC_Emb_category_3Prong->Clone("hTTbEmb_3Prong");
	    hDataEmb_3Prong = (TH1F*)h_ZTT_Emb_category_3Prong->Clone("hDataEmb_3Prong");

	    //fine binning for MSSM
	    if(selection_.find("bTag")!=string::npos){ 

	      float NormEmbedNew_fb = 0.;
	      drawHistogram(sbinEmbeddingPresel,sbinCat,"Embed", version_,analysis_, RUN,currentTree, variable, NormEmbedNew_fb,  Error, 1.0 , hCleanerfb,  sbinEmbedding  ,1);
	      TH1F* h_Emb_category_fb = (TH1F*)hCleanerfb->Clone("h_Emb_category_fb");
	      hCleanerfb->Reset();

	      float NormTTjetsEmbNew_fb = 0.;
	      drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew_fb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbinEmbedding, 1);
	      TH1F* h_TTbarMC_Emb_category_fb = (TH1F*)hCleanerfb->Clone("h_TTbarMC_Emb_category_fb");
	      hCleanerfb->Reset(); 

	      TH1F* h_ZTT_Emb_category_fb = (TH1F*)h_Emb_category_fb->Clone("h_ZTT_Emb_category_fb");
	      EvaluateZTTEmb(NormEmbedInclusive, NormZTTInclusive, NormTTjetsEmbInclusive, h_ZTT_Emb_category_fb, h_TTbarMC_Emb_category_fb);
	      hDataEmb_fb = (TH1F*)h_ZTT_Emb_category_fb->Clone("hDataEmb_fb");
	      hDataEmb_NotSubtracted_fb = (TH1F*)h_Emb_category_fb->Clone("hDataEmb_NotSubtracted_fb");
	      hDataEmb_NotSubtracted_fb->Scale(Weight_From_ZTT_MC);
	      h_TTbarMC_Emb_category_fb->Scale(Weight_From_ZTT_MC);
	      hTTbEmb_fb = (TH1F*)h_TTbarMC_Emb_category_fb->Clone("hTTbEmb_fb");
	    }
	  }
	}
// 	else {//TTbarEmbedded
// 	  cout<<"currentName = "<<currentName<<endl;
// 	  float NormTTjetsEmb = 0.;
// 	  drawHistogram(sbinEmbeddingPresel,sbinCat,"MCTTJetsEmb", version_,analysis_, RUN,currentTree, variable, NormTTjetsEmb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
// 	  hTTbEmb->Add(h1, 1.0);
// 	  cout<<"TTbarEmbedded : "<<hTTbEmb->Integral()<<endl;

// 	  //fine binning for MSSM
// 	  if(selection_.find("bTag")!=string::npos)
// 	    { 
// 	      hCleanerfb->Reset(); float NormTTjetsEmbNew_fb = 0.;
// 	      drawHistogram(sbinEmbeddingPresel,sbinCat, "MCTTJetsEmb",version_,analysis_,RUN, backgroundTTbarEmb, variable, NormTTjetsEmbNew_fb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbinEmbedding, 1);
// 	      hTTbEmb_fb->Add(hCleanerfb,1.);
// 	      hCleanerfb->Reset(); 
// 	    }
// 	}
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
    out<<selection_<<" TTbEmb : hTTbEmb -> "<<hTTbEmb->Integral()<<endl;
    out<<selection_<<" VV : hVV -> "<<hVV->Integral()<<endl;
  }
  out.close();

  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass")){ 
    hData->Scale(1.0, "width");
    hTTb->Scale(1.0, "width");
    hDataEmb->Scale(1.0, "width");
    hDataEmb_NotSubtracted->Scale(1.0, "width");
    //hDataEmb2->Scale(1.0, "width");
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

  //VV + W + ZJ
  hSiml->Add(hEWK,1.0);

  //Zmm
  hSiml->Add(hZmm,1.0);

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

  // TT
  aStack->Add(hTTb);

  // VV + W + ZJ
  aStack->Add(hEWK);

  // ZL
  aStack->Add(hZmm);

  // ZTT
  if(useEmbedding_)
    aStack->Add(hDataEmb);
  else
    aStack->Add(hZtt);

  TH1F* hSgnSUSY ;
  TH1F* hSgnSUSY1 ;
  TH1F* hSgnSUSY2 ;
  if(MSSM) {
    float crossSection = 1.0;
    cout<<"Modifying xsec : m=130 tanBeta=5"<<endl;
    crossSection = 1.76395701575004349;
    hSgnSUSY = (TH1F*)hSusy[0][5]->Clone("hSgnSUSY");
    hSgnSUSY->Add(hSusy[1][5]);
    hSgnSUSY->Scale(magnifySgn_*crossSection);
    hSgnSUSY->SetLineColor(kBlue);
    cout<<"Modifying xsec : m=300 tanBeta=12"<<endl;
    crossSection = 0.188078490754561067;
    hSgnSUSY1 = (TH1F*)hSusy[0][11]->Clone("hSgnSUSY1");
    hSgnSUSY1->Add(hSusy[1][11]);
    hSgnSUSY1->Scale(magnifySgn_*crossSection);
    hSgnSUSY1->SetLineColor(kRed);
    cout<<"Modifying xsec : m=600 tanBeta=35"<<endl;
    crossSection = 0.0519465144122727707;
    hSgnSUSY2 = (TH1F*)hSusy[0][16]->Clone("hSgnSUSY2");
    hSgnSUSY2->Add(hSusy[1][16]);
    hSgnSUSY2->Scale(magnifySgn_*crossSection);
    hSgnSUSY2->SetLineColor(kGreen+2);
  }
  if(!logy_ && !MSSM)
    aStack->Add(hSgn);
  //   if(!logy_ && MSSM)
  //     aStack->Add(hSgnSUSY);
  
  leg->AddEntry(hData,"Observed","P");
  if(MSSM){
    leg->AddEntry(hSgnSUSY,Form("(%.0fx) #phi#rightarrow#tau#tau m_{A}=130 tan#beta=5",magnifySgn_),"F");
    leg->AddEntry(hSgnSUSY1,Form("(%.0fx) #phi#rightarrow#tau#tau m_{A}=300 tan#beta=12",magnifySgn_),"F");
    leg->AddEntry(hSgnSUSY2,Form("(%.0fx) #phi#rightarrow#tau#tau m_{A}=600 tan#beta=35",magnifySgn_),"F");
  }
  else 
    leg->AddEntry(hSgn,Form("(%.0fx) H#rightarrow#tau#tau m_{H}=%d",magnifySgn_,mH_),"F");
  
  if(useEmbedding_)
    leg->AddEntry(hDataEmb,"Z#rightarrow#tau#tau (embedded)","F");
  else
    leg->AddEntry(hZtt,"Z#rightarrow#tau#tau","F"); 
  leg->AddEntry(hZmm,"Z#rightarrow#mu#mu","F");
  leg->AddEntry(hEWK,"Electroweak","F");
  leg->AddEntry(hQCD,"QCD","F");
  leg->AddEntry(hTTb,"t#bar{t}","F");
  
  if(MSSM && version_.Contains("TTbarEmb")){
    leg->AddEntry(hTTbEmb,"t#bar{t} embedded","F");
  }

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
  //   if(selection_.find("nobTag")!=string::npos && variable_.Contains("diTauNSVfitMass")) hData->SetAxisRange(0.1, 12000 ,"Y");
  //   if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos && variable_.Contains("diTauNSVfitMass"))hData->SetAxisRange(0.1, 140 ,"Y");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");
  if(logy_ && !MSSM)
    hSgn->Draw("HISTSAME");
  if(MSSM){
    hSgnSUSY->Draw("HISTSAME");
    hSgnSUSY1->Draw("HISTSAME");
    hSgnSUSY2->Draw("HISTSAME");
  }

  if(MSSM && version_.Contains("TTbarEmb")){
    hTTbEmb->Draw("HISTSAME");
  }

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

  //   TH1F* hRatio = new TH1F( "hRatio" ," ; ; #frac{(DATA-MC)}{#sqrt{DATA}}" , nBins , bins.GetArray());
  //   hRatio->Reset();
  //   hRatio->SetXTitle("");
  //   hRatio->SetYTitle("#frac{(DATA-MC)}{MC}");
  //   if(selection_.find("HighMt")!=string::npos) hRatio->SetYTitle("#frac{DATA}{MC}");

  //   hRatio->SetMarkerStyle(kFullCircle);
  //   hRatio->SetMarkerSize(0.8);
  //   hRatio->SetLabelSize(0.12,"X");
  //   hRatio->SetLabelSize(0.10,"Y");
  //   hRatio->SetTitleSize(0.12,"Y");
  //   hRatio->SetTitleOffset(0.36,"Y");

  //   float maxPull = 0.;
  //   for(int k = 0 ; k < hRatio->GetNbinsX(); k++){
  //     float pull = hData->GetBinContent(k) - hSiml->GetBinContent(k);
  //     if(selection_.find("HighMt")!=string::npos) pull = hData->GetBinContent(k);
  //     if(hSiml->GetBinContent(k)>0)
  //       pull /= hSiml->GetBinContent(k);
  //     hRatio->SetBinContent(k, pull);
  //     if(TMath::Abs(pull) > maxPull)
  //       maxPull = TMath::Abs(pull);
  //   }
  // //   hRatio->SetAxisRange(-1.2*maxPull,1.2*maxPull,"Y");
  //   hRatio->SetAxisRange(-0.5,0.5,"Y");
  //   if(selection_.find("HighMt")!=string::npos) hRatio->SetAxisRange(0.5,1.5,"Y");
  //   hRatio->Draw("P");

  TH1F * hRatio = (TH1F*)hData->Clone("hRatio");
  hRatio->Sumw2();
  if(selection_.find("HighMt")==string::npos) hRatio->Add(hSiml,-1);

  if(!version_.Contains("DecayMode")) hRatio->Divide(hSiml);//Decay mode scaling
  else hRatio->Divide(hDataEmb);

  hRatio->SetXTitle("");
  if(!version_.Contains("DecayMode")) hRatio->SetYTitle("#frac{DATA-MC}{MC}");
  else hRatio->SetYTitle("#frac{DATA-MC}{Z#rightarrow#tau#tau (Emb)}");

  if(selection_.find("HighMt")!=string::npos) hRatio->SetYTitle("#frac{DATA}{MC}");
  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  if(selection_.find("HighMt")!=string::npos) hRatio->SetAxisRange(0.5,1.5,"Y");
  else hRatio->SetAxisRange(-0.5,0.5,"Y");
  hRatio->Draw("EP");

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  
  TF1* line1 = new TF1("line","1",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line1->SetLineStyle(3);
  line1->SetLineWidth(1.5);
  line1->SetLineColor(kBlack);
  
  if(selection_.find("HighMt")!=string::npos) line1->Draw("SAME");
  else  line->Draw("SAME");

  //return;

  if(logy_){
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_log.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  }
  else{
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  }
  //   pad1->SaveAs(Form(location+"/%s/plots/pad_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  //   pad1->SaveAs(Form(location+"/%s/plots/pad_muTau_mH%d_%s_%s_%s.eps",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  //   pad1->SaveAs(Form(location+"/%s/plots/pad_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));

  /*
  //Plot of MC compared to Embedded for MVA input variables
  c1 = new TCanvas("c3","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TPad* pad3 = new TPad("padControl","",0.05,0.22,0.96,0.97);
  pad3->SetFillColor(0);
  pad3->Draw();

  pad3->cd();
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

  TLegend* leg3 = new TLegend(0.63,0.48,0.85,0.88,NULL,"brNDC");
  leg3->SetFillStyle(0);
  leg3->SetBorderSize(0);
  leg3->SetFillColor(10);
  leg3->SetTextSize(0.03);
  leg3->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=8 TeV}{%.1f fb^{-1} #tau_{#mu}#tau_{had}}", Lumi/1000. ));

  TH1F* hSiml_Scaled;
  TH1F* hZtt_Scaled;
  TH1F* hDataEmb_Scaled ;
  TH1F* hSgn_Scaled ;
  TH1F* hSgnSUSY_Scaled ;
  TH1F* hSgnSUSY1_Scaled ;
  TH1F* hSgnSUSY2_Scaled ;
  hSiml_Scaled = (TH1F*)hSiml->Clone("hSiml_Scaled");
  hSiml_Scaled->SetTitle("Scaled histograms");
  hDataEmb_Scaled = (TH1F*)hDataEmb->Clone("hDataEmb_Scaled");
  hZtt_Scaled = (TH1F*)hZtt->Clone("hZtt_Scaled");
  hSgn_Scaled = (TH1F*)hSgn->Clone("hSgn_Scaled");
  hSgnSUSY_Scaled = (TH1F*)hSgnSUSY->Clone("hSgnSUSY_Scaled");
  hSgnSUSY1_Scaled = (TH1F*)hSgnSUSY1->Clone("hSgnSUSY1_Scaled");
  hSgnSUSY2_Scaled = (TH1F*)hSgnSUSY2->Clone("hSgnSUSY2_Scaled");

  if(useEmbedding_)
  hSiml_Scaled->Add(hDataEmb,-1.0);
  else 
  hSiml_Scaled->Add(hZtt,-1.0);
  
  hSiml_Scaled->Scale(1.0/hSiml_Scaled->Integral());
  hZtt_Scaled->Scale(1.0/hZtt_Scaled->Integral());
  hDataEmb_Scaled->Scale(1.0/hDataEmb_Scaled->Integral());
  hSgn_Scaled->Scale(1.0/hSgn_Scaled->Integral());
  hSgnSUSY_Scaled->Scale(1.0/hSgnSUSY_Scaled->Integral());
  hSgnSUSY1_Scaled->Scale(1.0/hSgnSUSY1_Scaled->Integral());
  hSgnSUSY2_Scaled->Scale(1.0/hSgnSUSY2_Scaled->Integral());

  hSiml_Scaled->ResetAttFill();
  hZtt_Scaled->ResetAttFill();
  hDataEmb_Scaled->ResetAttFill();
  hSgn_Scaled->ResetAttFill();
  hSgnSUSY_Scaled->ResetAttFill();
  hSgnSUSY1_Scaled->ResetAttFill();
  hSgnSUSY2_Scaled->ResetAttFill();

  hSiml_Scaled->SetLineWidth(2);
  hZtt_Scaled->SetLineWidth(2);
  hDataEmb_Scaled->SetLineWidth(2);
  hSgn_Scaled->SetLineWidth(2);
  hSgnSUSY_Scaled->SetLineWidth(2);
  hSgnSUSY1_Scaled->SetLineWidth(2);
  hSgnSUSY2_Scaled->SetLineWidth(2);

  hSiml_Scaled->SetLineColor(kRed);
  hZtt_Scaled->SetLineColor(kBlue);
  hDataEmb_Scaled->SetLineColor(kBlue);
  hSgn_Scaled->SetLineColor(kGreen+2);
  hSgnSUSY_Scaled->SetLineColor(kGreen+2);
  hSgnSUSY1_Scaled->SetLineColor(kMagenta);
  hSgnSUSY2_Scaled->SetLineColor(kBlue-1);

  hSiml_Scaled->SetXTitle(XTitle_+" ("+Unities_+")");
  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass")){
  hSiml_Scaled->SetYTitle(Form(" dN/dm_{#tau#tau}(1/%s)", Unities_.Data() ) );
  }
  else 
  hSiml_Scaled->SetYTitle(Form("a.u. ", hSiml->GetBinWidth(1), Unities_.Data() ) );
  hSiml_Scaled->SetTitleSize(0.04,"X");
  hSiml_Scaled->SetTitleSize(0.05,"Y");
  hSiml_Scaled->SetTitleOffset(0.95,"Y");

  if(!logy_)
  hSiml_Scaled->SetAxisRange(0.0, TMath::Max( hSiml_Scaled->GetMaximum(), hDataEmb_Scaled->GetMaximum() )*1.2 ,"Y");
  else
  hSiml_Scaled->SetAxisRange(0.1, TMath::Max( hSiml_Scaled->GetMaximum(), hDataEmb_Scaled->GetMaximum() )*1.2 ,"Y");

  hSiml_Scaled->Draw("HIST");
  if(useEmbedding_)
  hDataEmb_Scaled->Draw("HISTSAME");
  else
  hZtt_Scaled->Draw("HISTSAME");

  //   if(logy_ && !MSSM)
  //     hSgn_Scaled->Draw("HISTSAME");
  //   if(MSSM){
  //     hSgnSUSY_Scaled->Draw("HISTSAME");
  //     hSgnSUSY1_Scaled->Draw("HISTSAME");
  //     hSgnSUSY2_Scaled->Draw("HISTSAME");
  //   }

  //   if(MSSM){
  //     leg3->AddEntry(hSgnSUSY_Scaled," #phi#rightarrow#tau#tau m_{A}=130 tan#beta=5","F");
  //     leg3->AddEntry(hSgnSUSY1_Scaled," #phi#rightarrow#tau#tau m_{A}=300 tan#beta=12","F");
  //     leg3->AddEntry(hSgnSUSY2_Scaled," #phi#rightarrow#tau#tau m_{A}=600 tan#beta=35","F");
  //   }
  //   else 
  //     leg3->AddEntry(hSgn_Scaled,Form("(%.0fx) H#rightarrow#tau#tau m_{H}=%d",magnifySgn_,mH_),"F");
  
  if(useEmbedding_)
  leg3->AddEntry(hDataEmb_Scaled,"Z#rightarrow#tau#tau (embedded)","F");
  else
  leg3->AddEntry(hZtt_Scaled,"Z#rightarrow#tau#tau","F"); 
  leg3->AddEntry(hSiml_Scaled,"Other MC backgrounds","F");

  leg3->Draw();

  if(logy_){
  c1->SaveAs(Form(location+"/%s/plots/plotControl_muTau_mH%d_%s_%s_%s_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  c1->SaveAs(Form(location+"/%s/plots/plotControl_muTau_mH%d_%s_%s_%s_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  }
  else{
  c1->SaveAs(Form(location+"/%s/plots/plotControl_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  c1->SaveAs(Form(location+"/%s/plots/plotControl_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
  }

  */

  TH1F *hDataBlind, *hRatioBlind;

  // Plot blinded histogram
  if(variable_.Contains("Mass")) {

    if(MSSM) {
      hDataBlind  = blindHistogram(hData,  100, 2000, "hDataBlind");
      hRatioBlind = blindHistogram(hRatio, 100, 2000, "hRatioBlind");
    }
    else{
      hDataBlind  = blindHistogram(hData,  100, 160, "hDataBlind");
      hRatioBlind = blindHistogram(hRatio, 100, 160, "hRatioBlind");
    }

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
    if(logy_ && !MSSM) hSgn->Draw("HISTSAME");
    if(logy_ && MSSM) hSgnSUSY->Draw("HISTSAME");
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

    if(logy_){
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind_log.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    }
    else{
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_muTau_mH%d_%s_%s_%s_blind.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.Data(),variable_.Data()));
    }
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
  hTTbUp->Write();
  hTTbDown->Write();
  hTTbTauFakeUp->Write();
  hTTbTauFakeDown->Write();
  hTTbEmb->Write();
  hTTbEmb_1Prong0Pi0->Write();
  hTTbEmb_1Prong1Pi0->Write();
  hTTbEmb_3Prong->Write();
  hTTbEmb_fb->Write();
  hZtt->Write();
  //cout<<"Number of bins before write = "<<hDataEmb2->GetNbinsX()<<endl;
  hDataEmb->Write();
  hDataEmb_1Prong0Pi0->Write();
  hDataEmb_1Prong1Pi0->Write();
  hDataEmb_3Prong->Write();
  hDataEmb_NotSubtracted->Write();
  hDataEmb_NotSubtracted_1Prong0Pi0->Write();
  hDataEmb_NotSubtracted_1Prong1Pi0->Write();
  hDataEmb_NotSubtracted_3Prong->Write();
  //hDataEmb2->Write();
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
  hDataAntiIsoLooseTauIsoCorrUp->Write();
  hDataAntiIsoLooseTauIsoCorrDown->Write();
  hDataAntiIsoLooseTauIsoFullVBF->Write();
  hDataAntiIsoLooseTauIsoRelaxVBF->Write();
  hDataAntiIsoLooseTauIsoQCD->Write();
  hDataAntiIsoLooseTauIsoQCDCorrUp->Write();
  hDataAntiIsoLooseTauIsoQCDCorrDown->Write();

  if(StudyQCD) {
    hDataSSAntiIsoLooseTauIso->Write();
    hDataSSIsoLooseTauIso->Write();
    hDataOSAntiIsoLooseTauIso->Write();
    hDataOSIsoLooseTauIso->Write();

  }
  hQCD_SS_ReverseMuIso_TightTauIsoMVA->Write();
  hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA->Write();

  hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_central->Write();
  hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_central->Write();

  hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_medium->Write();
  hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_medium->Write();

  hQCD_SS_ReverseMuIso_TightTauIsoMVA_eta_forward->Write();
  hQCD_SS_ReverseMuIso_LooseNonTightTauIsoMVA_eta_forward->Write();

  hData->Write();
  hParameters->Write();

  //fine binning histograms for MSSM
  hDataEmb_fb->Write(); hW_fb->Write(); hZtt_fb->Write(); 
  hDataEmb_NotSubtracted_fb->Write();
  //hDataEmb2_fb->Write();
  hZmm_fb->Write(); hZmj_fb->Write(); hTTb_fb->Write(); 
  hQCD_fb->Write(); hVV_fb->Write(); 
  hQCD_CorrUp_fb->Write(); hQCD_CorrDown_fb->Write();

  //hW up/Down
  cout<<"hW_TFUp->Integral() "<<hW_TFUp->Integral()<<endl;
  cout<<"hW_TFUp->GetBinContent(28) "<<hW_TFUp->GetBinContent(28)<<endl;
  cout<<"hW_TFUp->Integral(28,28) "<<hW_TFUp->Integral(28,28)<<endl;
  hW_TFUp->Write(); hW_TFDown->Write();
  hWLooseBTag_TFUp->Write(); hWLooseBTag_TFDown->Write();
  hW_TFUp_fb->Write(); hW_TFDown_fb->Write();

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      {
	if(hSignal[iP][iM]) hSignal[iP][iM]->Write();
	if(hSMEffTUp[iP][iM]) hSMEffTUp[iP][iM]->Write(); 
	if(hSMEffTDown[iP][iM]) hSMEffTDown[iP][iM]->Write(); 
      }
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
	{
	  if(hSusy[iP][iM]) hSusy[iP][iM]->Write(); 
	  if(hSusyNoWeight[iP][iM]) hSusyNoWeight[iP][iM]->Write(); 
	  if(hSusyEffTUp[iP][iM]) hSusyEffTUp[iP][iM]->Write(); 
	  if(hSusyEffTDown[iP][iM]) hSusyEffTDown[iP][iM]->Write(); 
	}

    for(int iM=0 ; iM<nMassesS ; iM++){
      hSUSYGGHUp[iM]->Write();
      hSUSYGGHDown[iM]->Write();
      hSUSYGGHScaleUp[iM]->Write();
      hSUSYGGHScaleDown[iM]->Write();
    }
  }

  if(variable_.Contains("Mass")) hDataBlind->Write();

  fout->Write();
  fout->Close();

  delete hQCD; delete hSS; delete hSSLooseVBF; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hTTbUp; delete hTTbDown; delete hTTbEmb; delete hZtt; delete hZmm_Up; delete hZmm_Down; delete hTTbTauFakeUp; delete hTTbTauFakeDown;
  delete hTTbEmb_1Prong0Pi0; delete hTTbEmb_1Prong1Pi0; delete hTTbEmb_3Prong;
  delete hW; delete hWSS; delete hWMinusSS; delete hW3Jets; delete hAntiIso; delete hAntiIsoFR;
  delete hZmmLoose; delete hZmjLoose; delete hLooseIso1; delete hLooseIso2; delete hLooseIso3;
  delete hVV; delete hSgn; delete hSgn1; delete hSgn2; delete hSgn3; delete hData; delete hParameters;
  delete hW3JetsLooseTauIso; delete hW3JetsMediumTauIso; delete hW3JetsMediumTauIsoRelVBF; delete hW3JetsMediumTauIsoRelVBFMinusSS; delete hWLooseBTag;
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoQCD; delete hDataAntiIsoLooseTauIsoFullVBF;
  delete hDataAntiIsoLooseTauIsoCorrUp; delete hDataAntiIsoLooseTauIsoQCDCorrUp;
  delete hDataAntiIsoLooseTauIsoCorrDown; delete hDataAntiIsoLooseTauIsoQCDCorrDown;

  delete hW_TFUp; delete hW_TFDown; delete hWLooseBTag_TFUp; delete hWLooseBTag_TFDown;
  delete hW_TFUp_fb; delete hW_TFDown_fb;

  if(StudyQCD) {
    delete hDataSSAntiIsoLooseTauIso;
    delete hDataSSIsoLooseTauIso;
    delete hDataOSAntiIsoLooseTauIso;
    delete hDataOSIsoLooseTauIso;
  }

  delete hDataEmb_fb; delete hZtt_fb; delete hW_fb; delete hZmm_fb; delete hZmj_fb; delete hTTb_fb;
  delete hTTbEmb_fb;
  delete hDataEmb_NotSubtracted_fb;
  //delete hDataEmb2_fb;
  delete hQCD_fb; delete hVV_fb;
  delete hQCD_CorrUp_fb;
  delete hQCD_CorrDown_fb;

  for(int iP=0 ; iP<nProd ; iP++)
    for(int iM=0 ; iM<nMasses ; iM++)
      if(hSignal[iP][iM]) delete hSignal[iP][iM];
  for(int iM=0 ; iM<nMasses ; iM++){
    delete hGGFHUp[iM]; delete hGGFHDown[iM];
  }
  for(int iM=0 ; iM<nMassesS ; iM++){
    delete hSUSYGGHUp[iM]; delete hSUSYGGHDown[iM];
    delete hSUSYGGHScaleUp[iM]; delete hSUSYGGHScaleDown[iM];
  }
  for(int iP=0 ; iP<nProdWW ; iP++)
    for(int iM=0 ; iM<nMassesWW ; iM++)
      if(hSignalWW[iP][iM]) delete hSignalWW[iP][iM];
  if(MSSM) {
    for(int iP=0 ; iP<nProdS ; iP++)
      for(int iM=0 ; iM<nMassesS ; iM++)
	{
	  if(hSusy[iP][iM]) delete hSusy[iP][iM];
	  if(hSusyNoWeight[iP][iM]) delete hSusyNoWeight[iP][iM];
	  if(hSusyEffTUp[iP][iM]) delete hSusyEffTUp[iP][iM];
	  if(hSusyEffTDown[iP][iM]) delete hSusyEffTDown[iP][iM];
	}
  }
  delete aStack;  delete hEWK; delete hSiml; delete hDataEmb;  delete hRatio; delete line;
  delete hDataEmb_NotSubtracted;
  delete hDataEmb_NotSubtracted_1Prong0Pi0;
  delete hDataEmb_NotSubtracted_1Prong1Pi0;
  delete hDataEmb_NotSubtracted_3Prong;
  //delete hDataEmb2;
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

  if(StudyQCDOlivier)
    {
      std::cout<<"!!! WARNING !!!"<<endl;
      std::cout<<"StudyQCDOlivier is ON, so the QCD estimation may be different from the standard one !"<<endl;
      std::cout<<"!!! END OF WARNING !!!"<<endl;
    }

  std::cout << "plotMuTau()" << std::endl;
  gROOT->SetBatch(true);

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  int mH, nBins, logy, useEmb; 
  float magnify, hltEff, xMin, xMax, maxY;
  float antiWsgn, antiWsdb;
  string category, analysis, variable, xtitle, unity, outputDir, RUN, version;

  if(argc==1) plotMuTauAll();
  //   else if(argc==20) { 
  else if(argc>16) { 

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
    analysis   =  argc>17 ? argv[17] : "";
    //     antiWsgn   =  atof(argv[17]);
    //     antiWsdb   =  atof(argv[18]);
    cout << endl << "RUN : " << RUN << " | VERSION : " << version << " | ANALYSIS : " << analysis << endl << endl;

    //     plotMuTau(mH,useEmb,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY, RUN, version,antiWsgn,antiWsdb);
    plotMuTau(mH,useEmb,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY, RUN, version);
  }
  else { cout << "Please put at least 16 arguments" << endl; return 1;}

  cout << "DONE" << endl;
  return 0;
}

