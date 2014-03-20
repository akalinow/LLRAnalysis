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

#define MSSM             true
#define VERBOSE          true
#define DEBUG            false
#define LOOP             true
#define W3JETS           false
#define USESSBKG         false
#define scaleByBinWidth  false
#define DOSPLIT          false
#define OldCat           false
#define useZDataMC       false
#define includeWG        true

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
void chooseSelection(TString variable_,
		     TString version_,
		     string selection_, 
		     TCut& tiso,
		     TCut& tdecaymode, 
		     TCut& ltiso, 
		     TCut& mtiso, 
		     TCut& antimu, 
		     TCut& antiele, 
		     TCut& lID, 
		     TCut& lveto, 
		     TCut& selection,
		     TCut& tpt
		     )
{
  cout<<"VERSION in chooseSelection :"<< version_<<endl;
  if(version_.Contains("tauptbin")) 
    tpt="ptL2>45 && ptL2<50";
  else if(version_.Contains("taupt20"))
    tpt="ptL2>20";
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
  
  if(version_.Contains("2bTagged")) 
    selection =" nJets20BTagged>1";

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

  //ElectronID
  if(version_.Contains("OldEleID"))  lID = "((TMath::Abs(scEtaL1)<0.80 && mvaPOGNonTrig>0.925) || (TMath::Abs(scEtaL1)<1.479 && TMath::Abs(scEtaL1)>0.80 && mvaPOGNonTrig>0.975) || (TMath::Abs(scEtaL1)>1.479 && mvaPOGNonTrig>0.985)) && nHits<0.5 && TMath::Abs(etaL1)<2.1";
  else if(version_.Contains("NewEleID")) 
    lID = "((TMath::Abs(scEtaL1)<0.80 && mvaPOGTrigNoIP>0.55) || (TMath::Abs(scEtaL1)<1.479 && TMath::Abs(scEtaL1)>0.80 && mvaPOGTrigNoIP>0.9) || (TMath::Abs(scEtaL1)>1.479 && mvaPOGTrigNoIP>0.925)) && nHits<0.5 && TMath::Abs(etaL1)<2.1";
  else if(version_.Contains("NewEleID_Tight")) 
    lID = "((TMath::Abs(scEtaL1)<0.80 && mvaPOGTrigNoIP>0.7875) || (TMath::Abs(scEtaL1)<1.479 && TMath::Abs(scEtaL1)>0.80 && mvaPOGTrigNoIP>0.95) || (TMath::Abs(scEtaL1)>1.479 && mvaPOGTrigNoIP>0.925)) && nHits<0.5 && TMath::Abs(etaL1)<2.1";
  
  //Third lepton veto
  if(version_.Contains("OldEleID")) lveto = "elecFlag==0 && vetoEventOld==0";
  if(version_.Contains("NewEleID")) lveto = "elecFlag==0 && vetoEventNew==0";

  //TauID
  if(version_.Contains("TauOldDM")) tdecaymode = "decayModeFindingOldDM>0";
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
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  else if(version_.Contains("HPSMVA3oldDMwLTTight")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>2" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Loose 1
    mtiso   = "tightestHPSMVA3oldDMwLTWP>1" ;//Medium 2
  }
  else if(version_.Contains("HPSMVA3oldDMwLTVTight")) {
    tiso   = "tightestHPSMVA3oldDMwLTWP>3" ;//Tight 3
    ltiso   = "tightestHPSMVA3oldDMwLTWP>0" ;//Loose 1
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

  // SVFit selection cut //
  if(version_.Contains("SVFit200"))      selection = "diTauNSVfitMass<200";
  else if(version_.Contains("200SVFit400")) selection = "diTauNSVfitMass>200 && diTauNSVfitMass<400";
  else if(version_.Contains("400SVFit600")) selection = "diTauNSVfitMass>400 && diTauNSVfitMass<600";
  else if(version_.Contains("SVFit600")) selection = "diTauNSVfitMass>600";

}
///////////////////////////////////////////////////////////////////////////////////////////////
void createStringsIsoFakeRate(TString fileName = "FakeRate.root",
			      string& scaleFactElec     = *( new string()), 
			      string& scaleFactElecUp   = *( new string()),
			      string& scaleFactElecDown = *( new string()),
			      string variableX = "ptL1",
			      string variableY = "FakeRate",
			      string interspace = "_",
			      string selection = "Elec_ptL1_incl",
			      bool verbose = false) {
  
  TFile FakeRate(fileName,"READ");
  if(FakeRate.IsZombie()){
    cout << "Missing FR histos... create dummy ones" << endl;
    scaleFactElec     = "(etaL1<999)";
    scaleFactElecUp   = "(etaL1<999)";
    scaleFactElecDown = "(etaL1<999)";
    return;
  }
 
  TF1*  frElec     = (TF1*)FakeRate.Get(("fit"+interspace+selection).c_str());
  TH1F* frElecUp   = (TH1F*)FakeRate.Get(("h"+variableY+"ErrUp"+selection).c_str());
  TH1F* frElecDown = (TH1F*)FakeRate.Get(("h"+variableY+"ErrDown"+selection).c_str());

  if(!frElec || !frElecUp || !frElecDown){
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

  scaleFactElec     = "( ";
  scaleFactElecUp   = "( ";
  scaleFactElecDown = "( ";

  for(unsigned int i = 0; i < binsFR.size()-1; i++){
   
    float min = binsFR[i];
    float max = binsFR[i+1];
    
    float bin = frElecUp->FindBin((max+min)/2.);
    if( bin == frElecUp->GetNbinsX() + 1) bin--;
    
    float weightBinElec_i     = frElec->Eval( (max+min)/2.)>0 ?       1./frElec->Eval( (max+min)/2.)      : 0.0;
    float weightBinElec_iUp   = frElecUp->GetBinContent( bin )>0 ?    1./frElecUp->GetBinContent( bin )   : 0.0;
    float weightBinElec_iDown = frElecDown->GetBinContent( bin )>0 ?  1./frElecDown->GetBinContent( bin ) : 0.0;
    
    scaleFactElec     += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinElec_i ) );
    scaleFactElecUp   += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinElec_iUp   ) );
    scaleFactElecDown += string( Form("(%s>=%f && %s<%f)*%f", variableX.c_str(), min , variableX.c_str(), max, weightBinElec_iDown ) );

    if(i < binsFR.size() - 2 ){
      scaleFactElec     += " + ";
      scaleFactElecUp   += " + ";
      scaleFactElecDown += " + ";
    }
  }
 
  scaleFactElec     += " )";
  scaleFactElecUp   += " )";
  scaleFactElecDown += " )";
 
  if(verbose){
    cout << scaleFactElec << endl;
    cout << scaleFactElecUp << endl;
    cout << scaleFactElecDown << endl;
  }

}
/////////EleIDSF for RecHitEmbedded
Float_t elecEffSF(Float_t pt, Float_t eta){

  //Define histogram with weights
  Double_t xAxis1[10] = {20, 25, 30, 35, 40, 50, 65, 80, 100, 200}; 
  Double_t yAxis1[4] = {0, 0.8, 1.479, 3}; 
   
  TH2F hPtEtaSF("hPtEtaSF","",9,xAxis1, 3,yAxis1);
  hPtEtaSF.SetBinContent(12,0.94448);
  hPtEtaSF.SetBinContent(13,0.963498);
  hPtEtaSF.SetBinContent(14,0.961211);
  hPtEtaSF.SetBinContent(15,0.97275);
  hPtEtaSF.SetBinContent(16,0.968306);
  hPtEtaSF.SetBinContent(17,0.99887);
  hPtEtaSF.SetBinContent(18,1.00868);
  hPtEtaSF.SetBinContent(19,1.01449);
  hPtEtaSF.SetBinContent(20,1.01274);
  hPtEtaSF.SetBinContent(21,1.01939);
  hPtEtaSF.SetBinContent(23,0.910509);
  hPtEtaSF.SetBinContent(24,0.933412);
  hPtEtaSF.SetBinContent(25,0.937532);
  hPtEtaSF.SetBinContent(26,0.949103);
  hPtEtaSF.SetBinContent(27,0.970999);
  hPtEtaSF.SetBinContent(28,0.978808);
  hPtEtaSF.SetBinContent(29,0.98384);
  hPtEtaSF.SetBinContent(30,1.01357);
  hPtEtaSF.SetBinContent(31,1.02283);
  hPtEtaSF.SetBinContent(32,1.02455);
  hPtEtaSF.SetBinContent(34,0.604604);
  hPtEtaSF.SetBinContent(35,0.696754);
  hPtEtaSF.SetBinContent(36,0.719897);
  hPtEtaSF.SetBinContent(37,0.764437);
  hPtEtaSF.SetBinContent(38,0.793726);
  hPtEtaSF.SetBinContent(39,0.846174);
  hPtEtaSF.SetBinContent(40,0.877331);
  hPtEtaSF.SetBinContent(41,0.943283);
  hPtEtaSF.SetBinContent(42,1.00928);
  hPtEtaSF.SetBinContent(43,1.26667);
  hPtEtaSF.SetEntries(24.8011);

  if(pt>199.99)
    pt=199.9;
  eta=fabs(eta);
  if(eta>2.99)
    eta=2.99;

  if(pt<20){
    //std::cout<<" pt="<<pt<<", eta="<<eta<<", eff="<<0<<std::endl;
    return 0;
  }
  Float_t eff=0;
  Int_t bin = hPtEtaSF.FindFixBin(pt,eta);
  eff = hPtEtaSF.GetBinContent(bin);
  //std::cout<<" pt="<<pt<<", eta="<<eta<<", eff="<<eff<<std::endl;

  return eff;
}
//////////


void drawHistogram(TCut sbinPair,
		   TCut sbinCat = TCut(""),
		   TString type = "MC",
		   TString version_ = "",
		   TString RUN = "ABCD",
		   TChain* tree = 0, 
		   TString variable = "", 
		   float& normalization      = *(new float()), 
		   float& normalizationError = *(new float()), 
		   float scaleFactor = 1.,
		   TH1F* h = 0,
		   TCut cut = TCut(""),
		   int verbose = 0){
  TCut genMass("run>0");
 //  if(DEBUG) 
//     cout << "Start drawHistogram : " << type << " " << version_ << " " << RUN << endl;

  // Special case for data
  if(type.Contains("Data")) {
    if(variable=="caloMEtNoHF")    variable="caloMEtNoHFUncorr";
    if(variable=="caloMEtNoHFPhi") variable="caloMEtNoHFUncorrPhi";
  }

  //ZL Up/Down shape
  if(type.Contains("MC_ZLUp") && (variable=="diTauNSVfitMass" || variable=="diTauVisMass"))
    variable = variable+"ZLUp";
  else if(type.Contains("MC_ZLDown") && (variable=="diTauNSVfitMass" || variable=="diTauVisMass"))
    variable= variable+"ZLDown";
  
  // Start processing
  if(tree!=0 && h!=0){

    TCut sampleWeight ="run>0";
    TCut weight   ="run>0";
    TCut weightDY ="run>0";
    TCut weightW  ="run>0";

    if(type.Contains("MC")) {

      sampleWeight = "sampleWeight";
      if(version_.Contains("NoDYExcl"))    weightDY = "sampleWeightDY";
      else                                 weightDY = "weightHepNupDY";
      if(version_.Contains("NoWHighStat")) weightW  = "weightHepNup";
      else                                 weightW  = "weightHepNupHighStatW";      

      if(     version_.Contains("SoftABC"))  weight = "(puWeightHCP*HLTweightTauABC*HLTweightEleABCShift*SFTau*SFEle_ABC*passL1etmCutABC*ZeeWeightHCP*weightDecayMode)";
      else if(version_.Contains("SoftD"))    weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTEleSoft*SFTau*SFEle_D*passL1etmCut*ZeeWeight*weightDecayMode)";
      else if(version_.Contains("SoftLTau")) weight = "(puWeightDLow*puWeightDHigh*HLTTauD*HLTEleSoft*SFTau*SFEle_D*ZeeWeight*weightDecayMode)";
      else if(!version_.Contains("Soft")) {
	if(     RUN=="ABC")                  weight = "(puWeightHCP*HLTweightTauABC*HLTweightEleABC*SFTau*SFEle_ABC*ZeeWeightHCP*weightDecayMode)";
	else if(RUN=="D")                    weight = "(puWeightD*HLTweightTauD*HLTweightEleD*SFTau*SFEle_D*ZeeWeight*weightDecayMode)";
	// 	else                                 weight = "(sampleWeight*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*weightHepNupDY*weightHepNup*ZeeWeight*weightDecayMode)";
	else if(version_.Contains("ZeeSel"))
	  weight = "(puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*weightDecayMode)";
	else 
	  weight = "(puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*ZeeWeight*weightDecayMode)";
      } //NoSoft     

      if(MSSM && !version_.Contains("NoWCorr")){
	if(type.Contains("WJetUp")) weightW  *= "weightTauFakeWJetUp";
        else if(type.Contains("WJetDown")) weightW  *= "weightTauFakeWJetDown";
        else if(type.Contains("WJet")) weightW  *= "weightTauFakeWJet";
      }
      cout<<"weightW : "<<weightW<<endl;

      //weights for GGFH pT re-weight
      if(type.Contains("GGFHUp"))
        weight *= "HqTWeightUp";
      else if(type.Contains("GGFHDown"))
        weight *= "HqTWeightDown";
      else
        weight *= "HqTWeight";
    }//MC
    else if(type.Contains("DY")){
      if(version_.Contains("ZeeSel"))
// 	weight = "(sampleWeightDY*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*weightHepNup*weightDecayMode)";
	weight = "(puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*weightDecayMode)";
      else 
// 	weight = "(sampleWeightDY*puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*weightHepNup*ZeeWeight*weightDecayMode)";
	weight = "(puWeight*HLTweightTau*HLTweightElec*SFTau*SFElec*ZeeWeight*weightDecayMode)";
    }
    else if(type.Contains("Embed")) {//PFEmbedding
      genMass = "HLTxMu17Mu8>0.5 && genDiTauMass>50"; // HLTxMu17Mu8
      if (version_.Contains("NoEleIDSF")){
	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTEleABCShift*passL1etmCutABC*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTEleSoft*passL1etmCut*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTEleSoft*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	else if(!version_.Contains("Soft")) {
	  if(RUN=="ABC")                       weight = "(HLTTauABC*HLTEleABC*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	  else if(RUN=="D")                    weight = "(HLTTauD*HLTEleD*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	  else                                 weight = "(HLTTau*HLTElec*SFElec*embeddingFilterEffWeight*weightDecayMode)";
	}
      }
      else{
	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTEleABCShift*passL1etmCutABC*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTEleSoft*passL1etmCut*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTEleSoft*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	else if(!version_.Contains("Soft")) {
	  if(RUN=="ABC")                       weight = "(HLTTauABC*HLTEleABC*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	  else if(RUN=="D")                    weight = "(HLTTauD*HLTEleD*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	  else                                 weight = "(HLTTau*HLTElec*SFElec*embeddingFilterEffWeight*elecEffSF*weightDecayMode)";
	}
      }
    }
//     else if(type.Contains("Embed")) {//RHEmbedding
//       genMass = "HLTxMu17Mu8>0.5 && genDiTauMass>50"; // HLTxMu17Mu8
//       if (version_.Contains("NoEleIDSF")){
// 	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTEleABCShift*passL1etmCutABC*SFElec*embeddingWeight*weightDecayMode)";
// 	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTEleSoft*passL1etmCut*SFElec*embeddingWeight*weightDecayMode)";
// 	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTEleSoft*SFElec*embeddingWeight*weightDecayMode)";
// 	else if(!version_.Contains("Soft")) {
// 	  if(RUN=="ABC")                       weight = "(HLTTauABC*HLTEleABC*SFElec*embeddingWeight*weightDecayMode)";
// 	  else if(RUN=="D")                    weight = "(HLTTauD*HLTEleD*SFElec*embeddingWeight*weightDecayMode)";
// 	  else                                 weight = "(HLTTau*HLTElec*SFElec*embeddingWeight*weightDecayMode)";
// 	}
//       }
//       else{
// 	if(     version_.Contains("SoftABC"))  weight = "(HLTTauABC*HLTEleABCShift*passL1etmCutABC*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	else if(version_.Contains("SoftD"))    weight = "(HLTTauD*HLTEleSoft*passL1etmCut*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	else if(version_.Contains("SoftLTau")) weight = "(HLTTauD*HLTEleSoft*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	else if(!version_.Contains("Soft")) {
// 	  if(RUN=="ABC")                       weight = "(HLTTauABC*HLTEleABC*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	  else if(RUN=="D")                    weight = "(HLTTauD*HLTEleD*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	  else                                 weight = "(HLTTau*HLTElec*SFElec*embeddingWeight*elecEffSF*weightDecayMode)";
// 	}
//       }
//     }

//     cout<<"weight : "<<weight<<endl;

    // Loop over entries to choose the event's pair instead of using pairIndex
    if(LOOP) {

      if(DEBUG) cout << "-- produce skim" << endl;
      tree->Draw(">>+skim", sbinPair,"entrylist");
//       tree->Draw(">>+skim", cut  && genMass,"entrylist",100);
      TEntryList *skim = (TEntryList*)gDirectory->Get("skim");
      int nEntries = skim->GetN();
      tree->SetEntryList(skim);
      if(DEBUG) cout << "-- produced skim : " << skim->GetN() << "entries" << endl;

      if(DEBUG) cout << "Reset h" << endl;
      h->Reset();

      // Declare tree variables
      int   run  = 0;
      int   event= 0; 

      // Declare tree branches
      if(DEBUG) cout << "-- declare tree branches" << endl;
      tree->SetBranchStatus("*",  0);   
      tree->SetBranchStatus("run",    1); tree->SetBranchAddress("run",    &run   );
      tree->SetBranchStatus("event",  1); tree->SetBranchAddress("event",  &event );
    
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
      }
      if(DEBUG) cout << "-- end loop" << endl;

      // Let all branches be readable again
      tree->SetBranchStatus("*",  1); 

      // Usual Draw
      if(DEBUG) cout << "-- setEntryList again" << endl;
      tree->SetEntryList(skim); // modified skim (choice of the best pair done in the loop)
      tree->Draw(variable+">>"+TString(h->GetName()),cut*weight*sampleWeight*weightDY*weightW*genMass*sbinCat);

      // Reset entry list
      tree->SetEntryList(0);
      skim->Reset();
      if(DEBUG) cout << "-- reset skim : " << skim->GetN() << "entries" << endl;
    }//if LOOP    
    else {
      TCut pairIndex="pairIndex[0]<1";
      tree->Draw(variable+">>"+TString(h->GetName()),cut*sampleWeight*weightDY*weightW*weight*genMass*pairIndex*sbinCat);
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
		   TString version_    = "AntiMu2_AntiEle2Tight_HPSDB3H_OldEleID",
		   //TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_Sep12/src/LLRAnalysis/Limits/bin/results/bins/"
		   //TString location    = "/data_CMS/cms/htautau/HCP12/binning/mssm_tailfitIvo/"
		   TString location    = "/data_CMS/cms/htautau/HCP12/binning/"
		   ){



  // input txt file with bins
  ifstream is;
  
  TArrayF dummy(2);
  dummy[0] = xMin_; dummy[1] = xMax_;
  
  char* c = new char[10];
  string filename = Form(location+"/bins_eTau_%s_%s.txt",variable_.Data(), selection_.c_str());
  if(MSSM)
    filename = Form(location+"/bins_eTau_%s_%s_MSSM.txt",variable_.Data(), selection_.c_str());

  cout<<"bins file : "<<filename<<endl;

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
// 	cout <<"c "<< c << endl;
      }
    }

  // choose the number of bins
  nBins =  nBins_>0 ? nBins_ : nBinsFromFile-1 ;
  TArrayF bins(nBins+1);
  cout << "Making histograms with " << nBins << " bins:" << endl;

  is.close();
//     is.open(Form(location+"/bins_eTau_%s_%s.txt",variable_.Data(), selection_.c_str()));
  is.open(filename);

  nBinsFromFile = 0;

  if(nBins_>0){
    for( ; nBinsFromFile <= nBins ; nBinsFromFile++){
      bins[nBinsFromFile] =  xMin_ + nBinsFromFile*(xMax_-xMin_)/nBins_;
      cout<<"bins : "<<bins[nBinsFromFile]<<endl;
    }
  }
  else{
    while (is.good())  
      {
	is.getline(c,999,',');     
	if (is.good() && nBinsFromFile<=nBins) {
	  bins[nBinsFromFile] = atof(c);
	  cout <<"bins from file : "<< bins[nBinsFromFile] << ", "<<endl; ;
	}
	nBinsFromFile++;
      }
    cout << endl;
  }

  return bins;

}

void evaluateWextrapolation(mapchain mapAllTrees, TString version_ = "",
			    TString WJetType = "MC", TString RUN = "ABCD",
			    string sign = "OS", bool useFakeRate = false, string selection_ = "",
			    float& scaleFactorOS= *(new float()), 
			    float& OSWinSignalRegionDATA= *(new float()),   float& OSWinSignalRegionMC = *(new float()), 
			    float& OSWinSidebandRegionDATA= *(new float()), float& OSWinSidebandRegionMC = *(new float()), 
			    float& scaleFactorTTOS = *(new float()),
			    TH1F* hWMt=0, TString variable="",
			    float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor=0.,
			    float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
			    float ElectoTauCorrectionFactor = 0., float JtoTauCorrectionFactor = 0.,
			    float antiWsdb = 0., float antiWsgn = 0., bool useMt = true,
			    TCut sbinCatForWextrapolation = "",
			    TCut sbinPZetaRel = "",  TCut sbinRelChargePZetaRel = "",
			    TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="",
			    TCut sbinPZetaRelaIsoInclusive = "", TCut sbinPZetaRelaIso = "", 
			    TCut vbf="", TCut boost="", TCut zeroJet = "", TCut sbinCat="",
			    TCut sbinPairIso="", TCut sbinPairAiso=""){

  
  float Error = 0.; float ErrorW1 = 0.;   float ErrorW2 = 0.;
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    drawHistogram(sbinPairIso,sbinCatForWextrapolation, "MC",version_, RUN, mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinRelChargePZetaRel&&pZ);
    drawHistogram(sbinPairIso,sbinCatForWextrapolation, "MC",version_, RUN, mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinRelChargePZetaRel&&apZ);
  }
  else{
    drawHistogram(sbinPairIso,sbinCatForWextrapolation, WJetType, version_, RUN, mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRel&&pZ);
    drawHistogram(sbinPairIso,sbinCatForWextrapolation, WJetType, version_, RUN, mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRel&&apZ);
  }
  scaleFactorOS      = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;
  float scaleFactorOSError = scaleFactorOS*(ErrorW1/OSWinSignalRegionMC + ErrorW2/OSWinSidebandRegionMC);
  if(VERBOSE){
    if(useMt)
      cout << "Extrap. factor for W " << sign << " : P(Mt>"     << antiWsdb << ")/P(Mt<"   << antiWsgn << ") ==> " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;
    else
      cout << "Extrap. factor for W " << sign << " : P(pZeta<- "<< antiWsdb << ")/P(pZeta>"<< antiWsgn << ") ==> " << scaleFactorOS << " +/- " << scaleFactorOSError << endl;    
  }
  // restore with full cut
  drawHistogram(sbinPairIso,sbinCat, WJetType, version_, RUN, mapAllTrees["WJets"],variable, OSWinSignalRegionMC,   ErrorW1, scaleFactor, hWMt, sbinPZetaRel&&pZ);
  // restore with full cut
  drawHistogram(sbinPairIso,sbinCat, WJetType, version_, RUN, mapAllTrees["WJets"],variable, OSWinSidebandRegionMC, ErrorW2, scaleFactor, hWMt, sbinPZetaRel&&apZ);
 

  float OSTTbarinSidebandRegionMC = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"],  variable,  OSTTbarinSidebandRegionMC,     Error, scaleFactor*TTxsectionRatio , hWMt, sbinPZetaRel&&apZ);

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
  drawHistogram(sbinPairIso,sbinCatBtag, "MC",version_, RUN, mapAllTrees["TTbar"], variable, OSTTbarinSidebandRegionBtagMC,  Error, scaleFactor*TTxsectionRatio, hWMt, bTagCut);
  float OSWinSidebandRegionBtagMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag, "MC",version_, RUN, mapAllTrees["WJets"], variable, OSWinSidebandRegionBtagMC,      Error, scaleFactor                , hWMt, bTagCut);
  float OSOthersinSidebandRegionBtagMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag, "MC",version_, RUN, mapAllTrees["Others"], variable, OSOthersinSidebandRegionBtagMC,Error, scaleFactor                , hWMt, bTagCut);
  float OSQCDinSidebandRegionBtag = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag, "Data_FR", version_, RUN, mapAllTrees["Data"], variable, OSQCDinSidebandRegionBtag,       Error, 1.0                        , hWMt, bTagCutaIso);
  float OSWinSidebandRegionBtagAIsoMC = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag, "MC_FR", version_, RUN, mapAllTrees["WJets"], variable, OSWinSidebandRegionBtagAIsoMC,  Error, scaleFactor        , hWMt, bTagCutaIso);
  float OSDatainSidebandRegionBtag = 0.;
  drawHistogram(sbinPairIso,sbinCatBtag, "Data", version_, RUN, mapAllTrees["Data"], variable, OSDatainSidebandRegionBtag,  Error, 1.0 , hWMt, bTagCut);

  scaleFactorTTOS = OSTTbarinSidebandRegionBtagMC>0 ? 
    (OSDatainSidebandRegionBtag-
     OSOthersinSidebandRegionBtagMC-
     OSWinSidebandRegionBtagMC-
     (OSQCDinSidebandRegionBtag-OSWinSidebandRegionBtagAIsoMC))/OSTTbarinSidebandRegionBtagMC : 1.0;
  if(VERBOSE){
    cout << "Normalizing TTbar from sideband: " << OSTTbarinSidebandRegionBtagMC << " events expected from TTbar." << endl 
	 << "From WJets " << OSWinSidebandRegionBtagMC <<  ", from QCD " << OSQCDinSidebandRegionBtag << " (of which " 
	 << OSWinSidebandRegionBtagAIsoMC << " expected from anti-isolated W)"
	 << ", from others " << OSOthersinSidebandRegionBtagMC << endl;
    cout << "Observed " << OSDatainSidebandRegionBtag << endl;
    cout << "====> scale factor for " << sign << " TTbar is " << scaleFactorTTOS << endl;
  }
  */
  if(scaleFactorTTOS<=0){
    if(VERBOSE) cout << "!!! scale factor is negative... set it to 1 !!!" << endl;
    scaleFactorTTOS = 1.0;
  }
  OSTTbarinSidebandRegionMC *= scaleFactorTTOS; // to comment
  if(VERBOSE) cout << "Contribution from TTbar in " << sign << " is " << OSTTbarinSidebandRegionMC << endl;

  float OSOthersinSidebandRegionMC   = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["Others"],    variable, OSOthersinSidebandRegionMC  ,Error,  scaleFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYtoTauinSidebandRegionMC  = 0.;
  if(useZDataMC)
    drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor*ExtrapolationFactorSidebandZDataMC*ExtrapolationFactorZDataMC , hWMt, sbinPZetaRel&&apZ);
  else
    drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"],  variable, OSDYtoTauinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYJtoTauinSidebandRegionMC = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"],  variable, OSDYJtoTauinSidebandRegionMC ,Error, scaleFactor*lumiCorrFactor*JtoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYElectoTauinSidebandRegionMC = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, OSDYElectoTauinSidebandRegionMC ,Error,scaleFactor*lumiCorrFactor*ElectoTauCorrectionFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYtoTauLLinSidebandRegionMC  = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauLL"],  variable, OSDYtoTauLLinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor , hWMt, sbinPZetaRel&&apZ);
  float OSDYtoTauJJinSidebandRegionMC  = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"],  variable, OSDYtoTauJJinSidebandRegionMC ,Error,  scaleFactor*lumiCorrFactor , hWMt, sbinPZetaRel&&apZ);

  float OSQCDinSidebandRegionData = 0.; float OSAIsoEventsinSidebandRegionData = 0.;
  drawHistogram(sbinPairAiso,sbinCat, "Data_FR", version_, RUN, mapAllTrees["Data"],  variable,        OSQCDinSidebandRegionData,   Error, 1.0         , hWMt, sbinPZetaRelaIso&&apZ);
  OSAIsoEventsinSidebandRegionData =  (OSQCDinSidebandRegionData/Error)*(OSQCDinSidebandRegionData/Error);
  float OSWinSidebandRegionAIsoMC  = 0.;float OSAIsoEventsWinSidebandRegionAIsoMC  = 0.;
  drawHistogram(sbinPairAiso,sbinCat, "MC_FR", version_, RUN, mapAllTrees["WJets"], variable,OSWinSidebandRegionAIsoMC,   Error, scaleFactor , hWMt, sbinPZetaRelaIso&&apZ);
  OSAIsoEventsWinSidebandRegionAIsoMC = (OSWinSidebandRegionAIsoMC/Error)*(OSWinSidebandRegionAIsoMC/Error)*scaleFactor; 

  drawHistogram(sbinPairIso,sbinCat, "Data", version_, RUN, mapAllTrees["Data"], variable, OSWinSignalRegionDATA ,Error, 1.0 , hWMt, sbinPZetaRel&&apZ);
  if(VERBOSE) cout << "Selected events in " << sign << " data from high Mt sideband " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= OSTTbarinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSOthersinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYtoTauinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYJtoTauinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYElectoTauinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYtoTauLLinSidebandRegionMC;
  OSWinSignalRegionDATA -= OSDYtoTauJJinSidebandRegionMC;
  if(useFakeRate) OSWinSignalRegionDATA -= (OSQCDinSidebandRegionData-OSWinSidebandRegionAIsoMC);
  OSWinSignalRegionDATA /= scaleFactorOS;
  if(VERBOSE){
    cout << "- expected from TTbar          " << OSTTbarinSidebandRegionMC << endl;
    cout << "- expected from Others         " << OSOthersinSidebandRegionMC << endl;
    cout << "- expected from DY->tautau     " << OSDYtoTauinSidebandRegionMC << endl;
    cout << "- expected from DY->ll, l->tau " << OSDYElectoTauinSidebandRegionMC << endl;
    cout << "- expected from DY->ll, j->tau " << OSDYJtoTauinSidebandRegionMC  << endl;
    cout << "- expected from DY->tautau, ll " << OSDYtoTauLLinSidebandRegionMC << endl;
    cout << "- expected from DY->tautau, jj " << OSDYtoTauJJinSidebandRegionMC << endl;
    cout << "- expected from QCD " << OSQCDinSidebandRegionData << ", obtained from " << OSAIsoEventsinSidebandRegionData << " anti-isolated events " << endl;
    cout << "  (MC predicts " << OSWinSidebandRegionAIsoMC << " W events in the aIso region, from a total of " << OSAIsoEventsWinSidebandRegionAIsoMC << " events)" << endl;
  }
  if(!useFakeRate) cout << " !!! QCD with fake-rate not subtracted !!!" << endl;
  if(VERBOSE){
    cout << "W+jets in " << sign << " region is estimated to be " <<  OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS
	 << " = " <<  OSWinSignalRegionDATA << endl;
    cout << " ==> the MC prediction is " << OSWinSignalRegionMC << " +/- " << ErrorW1 << endl;
  }
  OSWinSidebandRegionDATA = OSWinSignalRegionDATA*scaleFactorOS;
}

void evaluateQCD(mapchain mapAllTrees, TString version_ = "",
		 TString WJetType = "MC", TString RUN = "ABCD",
		 TH1F* qcdHisto = 0, TH1F* ssHisto = 0, bool evaluateWSS = true, string sign = "SS", bool useFakeRate = false, bool removeMtCut = false, string selection_ = "", 
		 float& SSQCDinSignalRegionDATAIncl_ = *(new float()), float& SSIsoToSSAIsoRatioQCD = *(new float()), float& scaleFactorTTSSIncl = *(new float()),
		 float& extrapFactorWSSIncl = *(new float()), 
		 float& SSWinSignalRegionDATAIncl = *(new float()), float& SSWinSignalRegionMCIncl = *(new float()),
		 float& SSWinSidebandRegionDATAIncl = *(new float()), float& SSWinSidebandRegionMCIncl = *(new float()),
		 TH1F* hExtrap=0, TString variable = "",
		 float scaleFactor=0., float TTxsectionRatio=0., float lumiCorrFactor = 0.,
		 float ExtrapolationFactorSidebandZDataMC = 0., float ExtrapolationFactorZDataMC = 0.,
		 float ElectoTauCorrectionFactor=0. , float JtoTauCorrectionFactor=0.,
		 float OStoSSRatioQCD = 0.,
		 float antiWsdb=0., float antiWsgn=0., bool useMt=true,
		 TCut sbin = "",
		 TCut sbinCatForWextrapolation = "",
		 TCut sbinPZetaRel ="", TCut pZ="", TCut apZ="", TCut sbinPZetaRelInclusive="", 
		 TCut sbinPZetaRelaIsoInclusive="", TCut sbinPZetaRelaIso="", TCut sbinPZetaRelaIsoSideband = "", 
		 TCut vbf="", TCut boost="", TCut zeroJet="", TCut sbinCat="", TCut sbinCatForSbin="",
		 TCut sbinPairIso="", TCut sbinPairAiso="",
		 bool substractTT=true, bool substractVV=true, bool substractDYTT=true){

  if(evaluateWSS)
    evaluateWextrapolation(mapAllTrees, version_, WJetType, RUN, sign, useFakeRate, selection_ , 
			   extrapFactorWSSIncl, 
			   SSWinSignalRegionDATAIncl,   SSWinSignalRegionMCIncl,
			   SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
			   scaleFactorTTSSIncl,
			   hExtrap, variable,
			   scaleFactor, TTxsectionRatio,lumiCorrFactor,
			   ExtrapolationFactorSidebandZDataMC,ExtrapolationFactorZDataMC, 
			   ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
			   antiWsdb, antiWsgn, useMt,
			   sbinCatForWextrapolation,
			   sbinPZetaRel, sbinPZetaRel,
			   pZ, apZ, sbinPZetaRelInclusive, 
			   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbf, boost, zeroJet, sbinCat,
			   sbinPairIso, sbinPairAiso
			   );
  
  float Error = 0.;
  float SSQCDinSignalRegionDATAIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, "Data", version_, RUN, mapAllTrees["Data"], variable,              SSQCDinSignalRegionDATAIncl,        Error, 1.0,         hExtrap, sbin, 1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap,  1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap,  1.0);

  float SSWJetsinSidebandRegionMCIncl    = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, WJetType,version_, RUN, mapAllTrees["WJets"],     variable, SSWJetsinSidebandRegionMCIncl,      Error, scaleFactor*(SSWinSidebandRegionDATAIncl/SSWinSidebandRegionMCIncl), hExtrap, sbin,1);
  if(!removeMtCut){
    hExtrap->Scale(SSWinSignalRegionDATAIncl/hExtrap->Integral());
    SSWJetsinSidebandRegionMCIncl = SSWinSignalRegionDATAIncl;
  }
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);

  float SSTTbarinSidebandRegionMCIncl    = 0.;
  if(substractTT) {
  drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["TTbar"],     variable, SSTTbarinSidebandRegionMCIncl,      Error, scaleFactor*TTxsectionRatio*scaleFactorTTSSIncl,       hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSOthersinSidebandRegionMCIncl    = 0.;
  if(substractVV) {
  drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["Others"],     variable, SSOthersinSidebandRegionMCIncl,     Error, scaleFactor,       hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSDYElectoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, SSDYElectoTauinSidebandRegionMCIncl,  Error, lumiCorrFactor*scaleFactor*ElectoTauCorrectionFactor,    hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauinSidebandRegionMCIncl = 0.;
  if(substractDYTT) {
    if(useZDataMC)
      drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor*ExtrapolationFactorZDataMC, hExtrap, sbin,1);
    else
      drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYToTauTau"],  variable, SSDYtoTauinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
    if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
    if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  }

  float SSDYJtoTauinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYJtoTau"],  variable, SSDYJtoTauinSidebandRegionMCIncl,   Error, lumiCorrFactor*scaleFactor*JtoTauCorrectionFactor,     hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);

  float SSDYtoTauLLinSidebandRegionMCIncl = 0.;
  drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYToTauTauLL"],  variable, SSDYtoTauLLinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  
  float SSDYtoTauJJinSidebandRegionMCIncl = 0.;
  /*drawHistogram(sbinPairIso,sbinCatForSbin, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"],  variable, SSDYtoTauJJinSidebandRegionMCIncl,    Error, lumiCorrFactor*scaleFactor, hExtrap, sbin,1);
  if(qcdHisto!=0) qcdHisto->Add(hExtrap, -1.0);
  if(ssHisto !=0) ssHisto->Add( hExtrap, -1.0);
  */
  if(VERBOSE)cout << "Selected events in inclusive " << sign << " data " << SSQCDinSignalRegionDATAIncl << endl;
//   if(version_.Contains("JetTauFR")){
//     cout<<"removing WJets from SS MC"<<endl;
//     SSQCDinSignalRegionDATAIncl  -= SSWinSidebandRegionMCIncl;
//   }
//   else
  SSQCDinSignalRegionDATAIncl  -= SSWJetsinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSTTbarinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSOthersinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYElectoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYJtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauLLinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl  -= SSDYtoTauJJinSidebandRegionMCIncl;
  SSQCDinSignalRegionDATAIncl *= OStoSSRatioQCD;
  if(qcdHisto!=0) qcdHisto->Scale(OStoSSRatioQCD);
  if(VERBOSE){
//     if(version_.Contains("JetTauFR")){
//       cout << "- expected from WJets          " << SSWinSidebandRegionMCIncl << endl;
//     }
//     else
    cout << "- expected from WJets          " << SSWJetsinSidebandRegionMCIncl << endl;
    cout << "- expected from TTbar          " << SSTTbarinSidebandRegionMCIncl << endl;
    cout << "- expected from Others         " << SSOthersinSidebandRegionMCIncl << endl;
    cout << "- expected from DY->tautau     " << SSDYtoTauinSidebandRegionMCIncl << endl;
    cout << "- expected from DY->ll, l->tau " << SSDYElectoTauinSidebandRegionMCIncl << endl;
    cout << "- expected from DY->ll, j->tau " << SSDYJtoTauinSidebandRegionMCIncl  << endl;
    cout << "- expected from DY->tautau, ll " << SSDYtoTauLLinSidebandRegionMCIncl << endl;
    cout << "- expected from DY->tautau, jj " << SSDYtoTauJJinSidebandRegionMCIncl << endl;
    cout << "QCD in inclusive SS region is estimated to be " << SSQCDinSignalRegionDATAIncl/OStoSSRatioQCD  << "*" << OStoSSRatioQCD
	 << " = " <<  SSQCDinSignalRegionDATAIncl << endl;
  }
  SSQCDinSignalRegionDATAIncl_ = SSQCDinSignalRegionDATAIncl;

  float SSQCDinSignalRegionDATAInclaIso = 0.;
  drawHistogram(sbinPairAiso,sbinCat, "Data", version_, RUN, mapAllTrees["Data"], variable, SSQCDinSignalRegionDATAInclaIso,    Error, 1.0, hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSWinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat, WJetType,version_, RUN, mapAllTrees["WJets"], variable, SSWinSignalRegionMCInclaIso,       Error,   scaleFactor*(SSWinSignalRegionDATAIncl/SSWinSignalRegionMCIncl) , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  float SSTTbarinSignalRegionMCInclaIso   = 0.;
  drawHistogram(sbinPairAiso,sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"], variable, SSTTbarinSignalRegionMCInclaIso,   Error,   scaleFactor*scaleFactorTTSSIncl , hExtrap, sbinPZetaRelaIsoSideband&&pZ);
  if(VERBOSE){
    cout << "Anti-isolated " << sign << " events inclusive = " << SSQCDinSignalRegionDATAInclaIso << ", of which we expect "
	 << SSWinSignalRegionMCInclaIso << " from W+jets " << " and " << SSTTbarinSignalRegionMCInclaIso << " from TTbar" << endl;
  }
  SSIsoToSSAIsoRatioQCD = (SSQCDinSignalRegionDATAIncl)/(SSQCDinSignalRegionDATAInclaIso-SSWinSignalRegionMCInclaIso-SSTTbarinSignalRegionMCInclaIso) ;
  if(VERBOSE)cout << "The extrapolation factor Iso<0.1 / Iso>0.2 is " << SSIsoToSSAIsoRatioQCD << endl;

}


void cleanQCDHisto(mapchain mapAllTrees, TString version_ = "",
		   TString RUN = "ABCD",
		   bool removeW = true, TH1F* hCleaner = 0, TH1F* hLooseIso = 0, TString variable = "",
		   float scaleFactor = 0., float WJetsCorrectionFactor=0., float TTbarCorrectionFactor=0.,
		   float ElectoTauCorrectionFactor=0., float JtoTauCorrectionFactor=0., float DYtoTauTauCorrectionFactor=0.,
		   TCut sbinSSlIso1 = "", TCut sbinCat="", TCut sbinPairIso="", TCut sbinPairAiso=""){

  if(hLooseIso==0) return;

  if(VERBOSE)cout << "Cleaning QCD histo with relaxed isolation from backgrounds..." << endl;

  float Error = 0.;
  float totalEvents = hLooseIso->Integral();

  float NormalizationWinLooseRegion = 0.;
  if(removeW){
    drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["WJets"],variable,      NormalizationWinLooseRegion,     Error, scaleFactor*WJetsCorrectionFactor, hCleaner, sbinSSlIso1 ,1);
    hLooseIso->Add(hCleaner,-1.0);
  }
  float NormalizationTTbarinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"], variable,     NormalizationTTbarinLooseRegion, Error, scaleFactor*TTbarCorrectionFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationOthersinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["Others"], variable,    NormalizationOthersinLooseRegion,Error, scaleFactor, hCleaner, sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYElectotauinLooseRegion= 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormalizationDYElectotauinLooseRegion,  Error,  scaleFactor*ElectoTauCorrectionFactor,    hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float  NormalizationDYJtotauinLooseRegion= 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable,  NormalizationDYJtotauinLooseRegion,   Error,  scaleFactor*JtoTauCorrectionFactor,     hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);
  float NormalizationDYTauTauinLooseRegion = 0.;
  drawHistogram(sbinPairIso,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"],  variable, NormalizationDYTauTauinLooseRegion,    Error, scaleFactor*DYtoTauTauCorrectionFactor, hCleaner  , sbinSSlIso1,1);
  hLooseIso->Add(hCleaner,-1.0);

  float totalRemoved = NormalizationWinLooseRegion+NormalizationTTbarinLooseRegion+NormalizationOthersinLooseRegion+
    NormalizationDYElectotauinLooseRegion+NormalizationDYJtotauinLooseRegion+NormalizationDYTauTauinLooseRegion;
  if(VERBOSE)cout << " ==> removed " << totalRemoved << " events from a total of " << totalEvents << " (" << totalRemoved/totalEvents*100 << " %)" << endl;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////7

void plotElecTau( Int_t mH_           = 120,
		  Int_t useEmbedding_ = 0,
		  string selection_   = "inclusive",
		  string analysis_    = "",		  
		  TString variable_   = "diTauVisMass",
		  TString XTitle_     = "full mass",
		  TString Unities_    = "GeV",
		  TString outputDir   = "",
		  Int_t nBins_ = 80, Float_t xMin_=0, Float_t xMax_=400,
		  Float_t magnifySgn_ = 1.0,
		  Float_t hltEff_     = 1.0,
		  Int_t logy_         = 0,
		  Float_t maxY_       = 1.2,
		  TString RUN         = "ABCD",
		  TString version_    = "Moriond",
		  TString location    = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_11_p6_NewTauID/src/LLRAnalysis/Limits/bin/results/"
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
  TString nameMasses[nMasses]={"90","95","100","105","110","115","120","125","130","135","140","145","150","155","160"};
  int hMasses[nMasses]={90,95,100,105,110,115,120,125,130,135,140,145,150,155,160};  

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
  
  ofstream out(Form(location+"/%s/yields/yieldsElecTau_mH%d_%s_%s_%s.txt",
		    outputDir.Data(),mH_,selection_.c_str(), analysis_.c_str(), variable_.Data() ),ios_base::out); 
  out.precision(5);
  int nBins = nBins_;

  TArrayF bins = createBins(nBins_, xMin_, xMax_, nBins, selection_, variable_,version_ );
  if(selection_.find("ZeeSel")!=string::npos)
    bins = createBins(nBins_, xMin_, xMax_, nBins, "inclusive", variable_);
  cout<<"Bins : "<<endl;
  for(int i=0 ; i<bins.GetSize() ; i++)cout<<"bin "<<i<<"   "<<bins[i]<<endl;
  
  // LUMINOSITY //
  float Lumi = 0.; 

  //PromptReco
  if(RUN=="ABC")    Lumi = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 ;       // 2012ABC 
  else if(RUN=="D") Lumi = 7274;                                               // 2012D 
//   else              Lumi = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 + 7274; // 2012ABCD
  else              Lumi = 19712.0; //19789.; // ReReco2012ABCD

  /////////////////

  float lumiCorrFactor                     = 1 ;    //= (1-0.056);
//   float TTxsectionRatio                    = 0.92; //lumiCorrFactor*(165.8/157.5) ;
  float TTxsectionRatio                    = 0.96; // Summer13 value from twiki
  float OStoSSRatioQCD                     = 1.06;
  float SSIsoToSSAIsoRatioQCD              = 1.0;
  float ElectoTauCorrectionFactor          = 1.0;
  float JtoTauCorrectionFactor             = 1.0;
//   float ElectoTauCorrectionFactor          = 0.976;
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
  cout << " > Elec->tau SF     = " << ElectoTauCorrectionFactor << endl;
  cout << endl;

  /////////////////  change SVfit mass here ///////////////////

  string variableStr = "";
  TString variable(variableStr.c_str());
  variable = variable_;

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
//   float antiWsgn  = useMt ? 20. :  20. ;
  float antiWsgn  = useMt ? 30. :  20. ; //mt cut at 30
  if(selection_.find("ZeeSel")!=string::npos) antiWsgn  = useMt ? 40. :  20. ; //mt cut at 40 for ZeeSF
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
  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=8 TeV}{%.1f fb^{-1} #tau_{e}#tau_{had}}", Lumi/1000. ));

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
  TH1F* hZmm      = new TH1F( "hZmm"    ,"Z+jets, e->tau"    , nBins , bins.GetArray());         hZmm->SetFillColor(kBlue-2);
  TH1F* hZmmLoose = new TH1F( "hZmmLoose","Z+jets, e->tau"   , nBins , bins.GetArray());         hZmmLoose->SetFillColor(kBlue-2);
  TH1F* hZmj      = new TH1F( "hZmj"    ,"Z+jets, jet to tau", nBins , bins.GetArray());         hZmj->SetFillColor(kBlue-2);
  TH1F* hZmjLoose = new TH1F( "hZmjLoose","Z+jets, jet->tau" , nBins , bins.GetArray());         hZmjLoose->SetFillColor(kBlue-2);
  TH1F* hZfakes   = new TH1F( "hZfakes" ,"Z+jets            ", nBins , bins.GetArray());         hZfakes->SetFillColor(kBlue-2);
  TH1F* hTTb      = new TH1F( "hTTb"    ,"ttbar"             , nBins , bins.GetArray());         hTTb->SetFillColor(kBlue-8); 
  TH1F* hTTbEmb   = new TH1F( "hTTbEmb" ,"ttbarEmbedded"     , nBins , bins.GetArray());         hTTbEmb->SetLineWidth(2); hTTbEmb->SetLineColor(kMagenta);hTTbEmb->SetFillStyle(3004);
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

  //W Up/Down
  TH1F* hW_TFUp        = new TH1F( "hW_TFUp"      ,"W+jets"            , nBins , bins.GetArray());         hW_TFUp->SetFillColor(kRed+2);
  TH1F* hW_TFDown        = new TH1F( "hW_TFDown"      ,"W+jets"            , nBins , bins.GetArray());    hW_TFDown->SetFillColor(kRed+2);
  TH1F* hWLooseBTag_TFUp      = new TH1F( "hWLooseBTag_TFUp" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());
  TH1F* hWLooseBTag_TFDown      = new TH1F( "hWLooseBTag_TFDown" ,  "W+jets (Loose b-Tag)", nBins , bins.GetArray());

  //histograms with fine binning for MSSM
  TH1F* hDataEmb_fb  = new TH1F( "hDataEmb_fb","Embedded"          , 400, 0., 2000.); hDataEmb_fb->SetFillColor(kOrange-4);
  TH1F* hTTbEmb_fb  = new TH1F( "hTTbEmb_fb","TTbar Embedded"          , 400, 0., 2000.); hTTbEmb_fb->SetFillColor(kOrange-4);
  TH1F* hW_fb        = new TH1F( "hW_fb"      ,"W+jets"            , 400, 0., 2000.); hW_fb->SetFillColor(kRed+2);
  TH1F* hW_TFUp_fb     = new TH1F( "hW_TFUp_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFUp_fb->SetFillColor(kRed+2 );
  TH1F* hW_TFDown_fb     = new TH1F( "hW_TFDown_fb"      ,"W+jets"            , 400, 0., 2000.); hW_TFDown_fb->SetFillColor(kRed+2 );
  
  TH1F* hZtt_fb      = new TH1F( "hZtt_fb"    ,"Ztautau"           , 400, 0., 2000.); hZtt_fb->SetFillColor(kOrange-4);
  TH1F* hZmm_fb      = new TH1F( "hZmm_fb"    ,"Z+jets, mu->tau"   , 400, 0., 2000.); hZmm_fb->SetFillColor(kBlue-2);
  TH1F* hZmj_fb      = new TH1F( "hZmj_fb"    ,"Z+jets, jet to tau", 400, 0., 2000.); hZmj_fb->SetFillColor(kBlue-2);
  TH1F* hTTb_fb      = new TH1F( "hTTb_fb"    ,"ttbar"             , 400, 0., 2000.); hTTb_fb->SetFillColor(kBlue-8);
  TH1F* hQCD_fb      = new TH1F( "hQCD_fb"    ,"QCD full vbf"      , 400, 0., 2000.); hQCD_fb->SetFillColor(kMagenta-10);
  TH1F* hVV_fb       = new TH1F( "hVV_fb"     ,"Diboson"           , 400, 0., 2000.); hVV_fb->SetFillColor(kRed+2);
  
  TH1F* hZmm_Up      = new TH1F( "hZmm_Up"    ,"Z+jets, e->tau"    , nBins , bins.GetArray());      hZmm_Up->SetFillColor(kBlue-2);
  TH1F* hZmm_Down    = new TH1F( "hZmm_Down"    ,"Z+jets, e->tau"    , nBins , bins.GetArray());      hZmm_Down->SetFillColor(kBlue-2);

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

  // get the FR-file
  string scaleFactElec       = "";
  string scaleFactElecUp     = "";
  string scaleFactElecDown   = "";
  string scaleFactTauQCD     = "";
  string scaleFactTauQCDUp   = "";
  string scaleFactTauQCDDown = "";
  string scaleFactTauW       = "";
  string scaleFactTauWUp     = "";
  string scaleFactTauWDown   = "";

  createStringsIsoFakeRate("FakeRate.root", scaleFactElec,     scaleFactElecUp,     scaleFactElecDown,     "ptL1", "FakeRate",    "_",   "ElecTau_Elec_ptL1_incl");
  createStringsIsoFakeRate("FakeRate.root", scaleFactTauQCD, scaleFactTauQCDUp, scaleFactTauQCDDown, "ptL2", "FakeRateQCD", "QCD_","ElecTau_Tau_ptL2_QCDSS02_WSS60_incl");
  createStringsIsoFakeRate("FakeRate.root", scaleFactTauW,   scaleFactTauWUp,   scaleFactTauWDown,   "ptL2", "FakeRateW",   "W_",  "ElecTau_Tau_ptL2_QCDOS02_WOS60_incl");


  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

//   TString pathToFile = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewTauIDVariables/EleTau/";
  TString pathToFile = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_OlivierTES/EleTau/";
  TString pathToFileDY = pathToFile;
  //TString pathToFileDY = "/data_CMS/cms/htautau/PostMoriond/NTUPLES_NewTauIDVariables/EleTau/DYZeeCorr/"; 
  TString Tanalysis_(analysis_);
  TString fileAnalysis = Tanalysis_;
  if(Tanalysis_=="") fileAnalysis = "nominal";

  cout << "FILE ANALYSIS " << fileAnalysis << endl;

  // DATA //
  TChain *data = new TChain("outTreePtOrd");
  if(RUN.Contains("ABC")) {
    data->Add(pathToFile+"/nTupleRun2012A*Data_ElecTau.root");
    data->Add(pathToFile+"/nTupleRun2012B*Data_ElecTau.root");
    data->Add(pathToFile+"/nTupleRun2012C*Data_ElecTau.root");
  }
  if(RUN.Contains("D")) {
    data->Add(pathToFile+"/nTupleRun2012D*Data_ElecTau.root");
  }

  if(!data) cout << "### DATA NTUPLE NOT FOUND ###" << endl;

  // EMBEDDED //
  TString treeEmbedded,fileAnalysisEmbedded;
  if(Tanalysis_.Contains("TauUp") || Tanalysis_.Contains("TauDown") ) { //|| Tanalysis_.Contains("ElecUp") || Tanalysis_.Contains("ElecDown") )
    treeEmbedded = "outTreePtOrd"+Tanalysis_;
    fileAnalysisEmbedded = fileAnalysis ;
  }
  else {
    treeEmbedded = "outTreePtOrd";
    fileAnalysisEmbedded = "nominal";
  }
  TChain *dataEmbedded = new TChain(treeEmbedded);
  //PFEmbed
  if(RUN.Contains("ABC")) {
    dataEmbedded->Add(pathToFile+"/nTupleRun2012A*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
    dataEmbedded->Add(pathToFile+"/nTupleRun2012B*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
    dataEmbedded->Add(pathToFile+"/nTupleRun2012C*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
  }
  if(RUN.Contains("D")) dataEmbedded->Add(pathToFile+"/nTupleRun2012D*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");

  //RHEmbed
//   if(RUN.Contains("ABC")) {
//     dataEmbedded->Add(pathToFile+"/nTupleRun2012A*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
//     dataEmbedded->Add(pathToFile+"/nTupleRun2012B*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
//     dataEmbedded->Add(pathToFile+"/nTupleRun2012C*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");
//   }
//   if(RUN.Contains("D")) dataEmbedded->Add(pathToFile+"/nTupleRun2012D*Embedded_ElecTau_"+fileAnalysisEmbedded+".root");

  if(!dataEmbedded) cout << "### EMBEDDED NTUPLE NOT FOUND ###" << endl;

  // BACKGROUNDS //
  TString treeMC;
  if(Tanalysis_.Contains("TauUp") || Tanalysis_.Contains("TauDown") || Tanalysis_.Contains("JetUp") || Tanalysis_.Contains("JetDown") )
    treeMC = "outTreePtOrd"+Tanalysis_;
  else treeMC = "outTreePtOrd";
  //
  TChain *backgroundDY         = new TChain(treeMC);
  TChain *backgroundDYTauTau   = new TChain(treeMC);
  TChain *backgroundDYTauTauLL = new TChain(treeMC);
  TChain *backgroundDYTauTauJJ = new TChain(treeMC);
  TChain *backgroundDYElectoTau= new TChain(treeMC);
  TChain *backgroundDYJtoTau   = new TChain(treeMC);
  TChain *backgroundTTbar      = new TChain(treeMC);
  TChain *backgroundTTbarEmb   = new TChain(treeMC);
  TChain *backgroundOthers     = new TChain(treeMC);
  TChain *backgroundWJets      = new TChain(treeMC);
  TChain *backgroundW3Jets     = new TChain(treeMC);
  //
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsTauTau_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsZTTL_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsZTTJ_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsEToTau_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJetsJetToTau_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets1Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets2Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets3Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundDY      ->Add(pathToFileDY+"/nTupleDYJets4Jets*_ElecTau_"+fileAnalysis+".root");

  backgroundTTbar   ->Add(pathToFile+"nTupleTTJets_*_ElecTau_"+fileAnalysis+".root");
  backgroundTTbarEmb->Add(pathToFile+"nTupleTTJets-Embedded_ElecTau_"+fileAnalysis+".root");
  //
  backgroundOthers  ->Add(pathToFile+"nTupleT-tW_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleTbar-tW_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWWJetsTo2L2Nu_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo2L2Q_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleWZJetsTo3LNu_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Nu_ElecTau_"+fileAnalysis+".root");
  backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo2L2Q_ElecTau_"+fileAnalysis+".root");
//   backgroundOthers  ->Add(pathToFile+"nTupleZZJetsTo4L_ElecTau_"+fileAnalysis+".root");
  if(includeWG){
    backgroundOthers  ->Add(pathToFile+"nTupleWGToLNuG_ElecTau_"+fileAnalysis+".root");
    backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2E_ElecTau_"+fileAnalysis+".root");
    backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2Mu_ElecTau_"+fileAnalysis+".root");
    backgroundOthers  ->Add(pathToFile+"nTupleWGstarToLNu2Tau_ElecTau_"+fileAnalysis+".root");
  }
  //
  backgroundWJets   ->Add(pathToFile+"nTupleWJets-p1_ElecTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets-p2_ElecTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets1Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets2Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets3Jets*_ElecTau_"+fileAnalysis+".root");
  backgroundWJets   ->Add(pathToFile+"nTupleWJets4Jets*_ElecTau_"+fileAnalysis+".root");

//   backgroundWJets   ->Add(pathToFile+"nTupleWJets4JetsV19_MuTau_"+fileAnalysis+".root");
//   backgroundW3Jets  ->Add(pathToFile+"nTupleWJets3Jets*_ElecTau_"+fileAnalysis+".root");
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
      signal[iP][iM]->Add(pathToFile+"/nTuple"+nameProd[iP]+nameMasses[iM]+"_ElecTau_"+fileAnalysis+".root");
//       cout<<"Signal entries :"<<signal[iP][iM]->GetEntries()<<endl;
//       cout<<"Signal iP :"<<iP<<endl;
//       cout<<"Signal iM :"<<iM<<endl;
      if(!signal[iP][iM])cout << "###  NTUPLE Signal " << nameProd[iP]+nameMasses[iM] << " NOT FOUND ###" << endl;  
    }
  }
  TChain *signalWW[nProdWW][nMassesWW];
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      signalWW[iP][iM] = new TChain(treeMC);
      signalWW[iP][iM]->Add(pathToFile+"/nTuple"+nameProdWW[iP]+nameMassesWW[iM]+"_ElecTau_"+fileAnalysis+".root");
      if(!signalWW[iP][iM])cout << "###  NTUPLE Signal " << nameProdWW[iP]+nameMassesWW[iM] << " NOT FOUND ###" << endl;
    }
  }
  TChain *signalSusy[nProdS][nMassesS];
  for(int iP=0 ; iP<nProdS ; iP++) {
    for(int iM=0 ; iM<nMassesS ; iM++) {
      signalSusy[iP][iM] = new TChain(treeMC);
      signalSusy[iP][iM]->Add(pathToFile+"/nTupleSUSY"+nameProdS[iP]+nameMassesS[iM]+"_ElecTau_"+fileAnalysis+".root");
    }
  }

  // Split DY into 3 sub-samples (TauTau, ElecToTau, JetToTau)
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
    backgroundDYElectoTau = (TChain*)backgroundDY->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
    //
    cout << "Now copying g/Z -> mu+mu- jet->tau" << endl;
    backgroundDYJtoTau  = (TChain*)backgroundDY->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau
  }
  else {
    cout << "USE DY SEPARATE SUB-SAMPLES" << endl;
    cout << "FILE ANALYSIS " << fileAnalysis << endl;
    //
    //backgroundDYTauTau    ->Add(pathToFile+"/nTupleDYJetsTauTau_ElecTau_"+fileAnalysis+".root");
    //backgroundDYTauTauLL  ->Add(pathToFile+"/nTupleDYJetsZTTL_ElecTau_"+fileAnalysis+".root");
    //backgroundDYElectoTau ->Add(pathToFile+"/nTupleDYJetsEToTau_ElecTau_"+fileAnalysis+".root");
    //backgroundDYJtoTau    ->Add(pathToFile+"/nTupleDYJetsJetToTau_ElecTau_"+fileAnalysis+".root");
    backgroundDYTauTau    ->Add(pathToFileDY+"/nTupleDYJets*TauTau*ElecTau_"+fileAnalysis+".root");
    backgroundDYElectoTau ->Add(pathToFileDY+"/nTupleDYJets*EToTau*ElecTau_"+fileAnalysis+".root");
    backgroundDYJtoTau    ->Add(pathToFileDY+"/nTupleDYJets*JetToTau*ElecTau_"+fileAnalysis+".root");
    backgroundDYTauTauLL  ->Add(pathToFileDY+"/nTupleDYJets*ZTTL*ElecTau_"+fileAnalysis+".root");
    backgroundDYTauTauJJ  ->Add(pathToFileDY+"/nTupleDYJets*ZTTJ*ElecTau_"+fileAnalysis+".root");
  }

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"           << endl;
  cout << backgroundDYTauTauLL->GetEntries()  << " come from DY->tautau LL"      << endl;
  cout << backgroundDYTauTauJJ->GetEntries()  << " come from DY->tautau JJ"      << endl;
  cout << backgroundDYElectoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau"   << endl;

  // MAPPING //
  if(VERBOSE) cout << "-- gather the trees" << endl;

  const int nVarious = 13;
  const int nChainsSM1= nVarious  + nProd*nMasses;
  const int nChainsSM = nChainsSM1 + nProdWW*nMassesWW;
  const int nChains  = nChainsSM + nProdS*nMassesS;
  TString treeNamesVarious[nVarious]={"SS","WJets","Data","W3Jets","TTbar","Others","DYToTauTau","DYToTauTauLL","DYToTauTauJJ","DYElectoTau","DYJtoTau","Embedded","TTbarEmbedded"};
  TChain* chainsVarious[nVarious]   ={data,backgroundWJets,data,backgroundW3Jets,backgroundTTbar,backgroundOthers,
				      backgroundDYTauTau,backgroundDYTauTauLL,backgroundDYTauTauJJ,backgroundDYElectoTau,backgroundDYJtoTau,dataEmbedded,backgroundTTbarEmb};
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

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  ///// ELE PT+ID+ISO ///////
  TCut lpt("ptL1>24");
  TCut lptMax("ptL1>-999");
  TCut lID("((TMath::Abs(scEtaL1)<0.80 && mvaPOGNonTrig>0.925) || (TMath::Abs(scEtaL1)<1.479 && TMath::Abs(scEtaL1)>0.80 && mvaPOGNonTrig>0.975) || (TMath::Abs(scEtaL1)>1.479 && mvaPOGNonTrig>0.985)) && nHits<0.5 && TMath::Abs(etaL1)<2.1");

  TCut EleIDSFEmbedded("1");
  //TCut EleIDSFEmbedded("((0.8*(TMath::Abs(etaL1)>1.479))+(TMath::Abs(etaL1)<=1.479))");
  //TCut EleIDSFEmbedded("elecEffSF(ptL1,scEtaL1)");

  if(     version_.Contains("SoftD"))    {lpt="ptL1>14"; lptMax="ptL1<=24"; }
  else if(version_.Contains("SoftLTau")) {lpt="ptL1>14"; lptMax="ptL1<=24"; }
  else if(!version_.Contains("Soft"))    {lpt="ptL1>24"; lptMax="ptL1>-999";}
  if(     version_.Contains("NoMaxPt"))  {lptMax="ptL1>-999";}
  lpt = lpt && lptMax ;

  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

  ////// TAU PT+ID+ISO //////
  TCut tpt("ptL2>30");
  TCut antimu("tightestAntiMuWP>0");
  TCut antiele("tightestAntiEMVA5WP > 0");
  TCut tiso("hpsDB3H<1.5");
  TCut tdecaymode("decayModeFindingOldDM>0");
  TCut ltiso("hpsDB3H<10.0");
  TCut mtiso("hpsDB3H<5.0");
  TCut lveto("elecFlag==0 && vetoEventOld==0"); //elecFlag==0
  TCut selection("diTauNSVfitMass>-999"); 

  chooseSelection(variable_,version_, selection_, tiso, tdecaymode, ltiso, mtiso, antimu, antiele, lID, lveto,selection,tpt);

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

  tpt= tpt&&TCut("TMath::Abs(etaL2)<2.3 && (ZimpactTau<-1.5 || ZimpactTau>0.5)");
  if(selection_.find("novbfLow")!=string::npos)
    tpt= "ptL2>20 && TMath::Abs(etaL2)<2.3 && (ZimpactTau<-1.5 || ZimpactTau>0.5)";


   ////// EVENT WISE //////
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  //TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));
  TCut apZ2(Form("((%s)>60 && (%s)<120)",antiWcut.c_str(),antiWcut.c_str()));

  bool removeMtCut     = bool(selection_.find("NoMt")!=string::npos);
  bool invertDiTauSign = bool(selection_.find("SS")!=string::npos);
  TCut MtCut       = removeMtCut     ? "(etaL1<999)" : pZ;
  if(selection_.find("HighMt")!=string::npos) {
    MtCut="MtLeg1MVA>70";
    pZ= "MtLeg1MVA>70";
    apZ= "MtLeg1MVA>70";
  }
  TCut diTauCharge = invertDiTauSign ? SS : OS; 

  // HLT matching //
  TCut hltevent("HLTx==1 && HLTmatch==1");
  if(     version_.Contains("SoftD"))    hltevent = "HLTxSoft==1         && HLTmatchSoft==1" ;
  else if(version_.Contains("SoftLTau")) hltevent = "HLTxIsoEle13Tau20==1  && HLTmatchIsoEle13Tau20==1" ;
  else if(!version_.Contains("Soft"))    hltevent = "HLTx==1         && HLTmatch==1" ;
  else {
    cout << "ERROR : Soft analyses need RUN set to ABC or D, nothing else is allowed." << endl;
    return;
  }

  ////// CATEGORIES ///
  TCut zeroJet("nJets30<1");
  TCut oneJet("nJets30>=1 && MEtMVA>30");
  TCut twoJets("nJets30>=2");
//   TCut vbf("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
  TCut vbfTight("nJets30>=2 && pt1>30 && pt2>30 && Mjj>700 && Deta>4.0 && isVetoInJets!=1 && diTauRecoPt>100 && nJets20BTagged<1");
  TCut vbfRelaxedTight("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");
  TCut vbfRelaxedTightQCD("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");
  TCut vbf;
  if (OldCat)
    vbf = TCut("nJets30>=2 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
  else{ 
    vbf = TCut("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1 && nJets20BTagged<1"); 
    vbf = vbf && !vbfTight; 
  }
  //   TCut vbfLoose("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfLoose("nJets30>=2 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfLooseQCD("nJets20>=2 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfRelaxedTightZL("nJets20>=2 && pt1>20 && pt2>20 && isVetoInJets!=1 && Mjj>200 && Deta>2 && diTauRecoPt>100");
  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");

  TCut boostNoMet("nJets30>0 && pt1>30 && nJets20BTagged<1");
  boostNoMet = boostNoMet && !vbf && !vbfTight /*&& !vh*/;

  if(OldCat){ 
    if(selection_.find("High")!=string::npos) { 
      boostNoMet = boostNoMet&&TCut("ptL2>40");  
    } 
    else if(selection_.find("Low")!=string::npos){ 
      boostNoMet = boostNoMet&&TCut("ptL2<40");  
    } 
  }
  else{ 
    if(selection_.find("High")!=string::npos) { 
      boostNoMet = boostNoMet&&TCut("ptL2>45"); 
    } 
    else if(selection_.find("Medium")!=string::npos){ 
      boostNoMet = boostNoMet&&TCut("ptL2>30 && ptL2<45"); 
    } 
    else if(selection_.find("Low")!=string::npos){ 
      boostNoMet = boostNoMet&&TCut("ptL2<30"); 
    } 
  }
  if(selection_.find("highhiggs")!=string::npos) { 
    boostNoMet = boostNoMet&&TCut("diTauRecoPt>100"); 
  } 
  else if(selection_.find("lowhiggs")!=string::npos) { 
    boostNoMet = boostNoMet&&TCut("diTauRecoPt<100"); 
  }
  
  TCut boost      = boostNoMet && TCut("MEtMVA>30");
  TCut boostMet0  = boostNoMet && TCut("MEtMVA>0");
  TCut boostMet5  = boostNoMet && TCut("MEtMVA>5");
  TCut boostMet10 = boostNoMet && TCut("MEtMVA>10");
  TCut boostMet15  = boostNoMet && TCut("MEtMVA>15");
  TCut boostMet20 = boostNoMet && TCut("MEtMVA>20");
  TCut boostMet25  = boostNoMet && TCut("MEtMVA>25");
  TCut boostMet30  = boostNoMet && TCut("MEtMVA>30");
  TCut boostMet35  = boostNoMet && TCut("MEtMVA>35");
  TCut boostMet40  = boostNoMet && TCut("MEtMVA>40");
  TCut boostMet45  = boostNoMet && TCut("MEtMVA>45");

  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut bTagLoose("nJets30<2 && nJets20BTaggedLoose>0"); //for W shape in b-Category
//   TCut nobTag("nJets30<2 && nJets20BTagged==0");
  TCut nobTag("nJets20BTagged==0");
  TCut novbf("nJets30<1 && nJets20BTagged==0");
  TCut ZeeSel("MEtMVA<30 && nJets30<2 && ptL2>30");

  if(OldCat){ 
    if(selection_.find("High")!=string::npos) { 
      novbf = novbf&&TCut("ptL2>40"); 
    } 
    else if(selection_.find("Low")!=string::npos){ 
      novbf = novbf&&TCut("ptL2<40");  
    } 
  }
  else{ 
    if(selection_.find("High")!=string::npos) { 
      novbf = novbf&&TCut("ptL2>45"); 
    } 
    else if(selection_.find("Medium")!=string::npos){ 
      novbf = novbf&&TCut("ptL2>30 && ptL2<45"); 
    } 
    else if(selection_.find("Low")!=string::npos){ 
      novbf = novbf&&TCut("ptL2<30"); 
    } 
  }

  TCut sbinCat("");
  if(     selection_.find("inclusive")!=string::npos)  sbinCat = "etaL1<999";
  else if(selection_.find("oneJet")!=string::npos)     sbinCat = oneJet;
  else if(selection_.find("twoJets")!=string::npos)    sbinCat = twoJets;
  else if(selection_.find("vh")!=string::npos)         sbinCat = vh;
  else if(selection_.find("novbf")!=string::npos)      sbinCat = novbf;
  else if(selection_.find("ZeeSel")!=string::npos)     sbinCat = ZeeSel;
  else if(selection_.find("boostMet0")!=string::npos)  sbinCat = boostMet0;
  else if(selection_.find("boostMet5")!=string::npos)  sbinCat = boostMet5;
  else if(selection_.find("boostMet10")!=string::npos) sbinCat = boostMet10;
  else if(selection_.find("boostMet15")!=string::npos) sbinCat = boostMet15;
  else if(selection_.find("boostMet20")!=string::npos) sbinCat = boostMet20;
  else if(selection_.find("boostMet25")!=string::npos) sbinCat = boostMet25;
  else if(selection_.find("boostMet30")!=string::npos) sbinCat = boostMet30;
  else if(selection_.find("boostMet35")!=string::npos) sbinCat = boostMet35;
  else if(selection_.find("boostMet40")!=string::npos) sbinCat = boostMet40;
  else if(selection_.find("boostMet45")!=string::npos) sbinCat = boostMet45;

  else if(selection_.find("nobTag")!=string::npos)     sbinCat = nobTag;
  else if(selection_.find("vbfTight")!=string::npos)   sbinCat = vbfTight;
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)      sbinCat = vbf;
  else if(selection_.find("boost")!=string::npos && selection_.find("boostMet")==string::npos) sbinCat = boost;
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)    sbinCat = bTag;

  // Global TCuts = category && object selection && event selection //

  TCut sbinPresel           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && hltevent   ;
  TCut sbinEmbeddingPresel  = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection               ;
  TCut sbinaIsoPresel       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && hltevent   ;
  TCut sbinlIsoPresel       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && lliso && lveto && selection && hltevent   ;
  TCut sbinaIsoLtisoPresel  = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && hltevent  ;
  TCut sbinLtisoPresel      = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso && lveto && selection && hltevent  ;

  TCut sbinInclusive                     = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinChargeRelInclusive            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection &&                MtCut  && hltevent ;
  TCut sbinChargeRelLtisoInclusive       = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection &&               MtCut  && hltevent ;
  TCut sbinChargeRelPZetaRelInclusive    = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection &&                hltevent ;
  TCut sbinEmbeddingInclusive            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut              ;
  TCut sbinPZetaRelEmbeddingInclusive    = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge                        && EleIDSFEmbedded;
  TCut sbinPZetaRelSSInclusive           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS                    && hltevent  && EleIDSFEmbedded;
  TCut sbinPZetaRelInclusive             = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge           && hltevent ;
  TCut sbinSSInclusive                   = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSaIsoInclusive               = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinAisoInclusive                 = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinPZetaRelSSaIsoInclusive       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                    && hltevent ;
  TCut sbinPZetaRelSSaIsoMtisoInclusive  = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                    && hltevent ;
  TCut sbinSSaIsoLtisoInclusive          = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSaIsoMtisoInclusive          = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinPZetaRelaIsoInclusive         = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge           && hltevent ;
  TCut sbinSSltisoInclusive              = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinLtisoInclusive                = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinMtisoInclusive                = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinPZetaRelLtisoInclusive        = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge           && hltevent ;
  
  TCut sbin                   = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinEmbedding          = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut               && EleIDSFEmbedded;
  TCut sbinEmbeddingPZetaRel  = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge                        && EleIDSFEmbedded;
  TCut sbinPZetaRel           = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge           && hltevent ;
  TCut sbinPZetaRelaIso       = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && diTauCharge           && hltevent ;
  TCut sbinPZetaRelSSaIso     = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                    && hltevent ;
  TCut sbinSS                 = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinPZetaRelSS         = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && liso  && lveto && selection && SS                    && hltevent ;
  TCut sbinSSaIso             = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSlIso1            = lpt && lID && tpt && tiso  && tdecaymode && antimu && antiele && lliso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSlIso2            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSlIso3            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && lliso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSaIsoLtiso        = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSaIsoMtiso        = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSltiso            = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinSSmtiso            = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && SS          && MtCut  && hltevent ;
  TCut sbinLtiso              = lpt && lID && tpt && ltiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinMtiso              = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge && MtCut  && hltevent ;
  TCut sbinPZetaRelMtiso      = lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && liso  && lveto && selection && diTauCharge           && hltevent ;
  TCut sbinPZetaRelSSaIsoMtiso= lpt && lID && tpt && mtiso && tdecaymode && antimu && antiele && laiso && lveto && selection && SS                    && hltevent ;

  TCut sbinCatIncl = "etaL1<999";

  cout << sbin << endl;
  cout << "sbinCat :" << endl;
  cout << sbinCat << endl;
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << endl;
  cout << "#############################################################" << endl;
  cout << ">>>>>>>>>>> BEGIN Compute inclusive informations <<<<<<<<<<<<" << endl;
  cout << "#############################################################" << endl;
  cout << endl;

  cout << "******** Extrapolation factors for Z->tautau normalization ********" << endl;
  // inclusive DY->tautau:
  cout<<"Number of bins "<<nBins<<endl;
  TH1F* hExtrap = new TH1F("hExtrap","",nBins , bins.GetArray());
  float Error = 0.;

  float ExtrapDYInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl, "MC",version_, RUN, mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusive,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->tautau             = " << ExtrapDYInclusive << " +/- " <<  Error << endl; 

  float ExtrapDYInclusivePZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCatIncl, "MC",version_, RUN, mapAllTrees["DYToTauTau"], variable, ExtrapDYInclusivePZetaRel,   Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRelInclusive);
  cout << "All Z->tautau (pZeta Rel) = " << ExtrapDYInclusivePZetaRel << " +/- " <<  Error << endl; 

  float ExtrapLFakeInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable,ExtrapLFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->ee, e->tau      = " << ExtrapLFakeInclusive << " +/- " <<  Error << endl;

  float ExtrapJFakeInclusive = 0.;
  drawHistogram(sbinPresel,sbinCatIncl, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, ExtrapJFakeInclusive,Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinInclusive);
  cout << "All Z->ee, j->tau       = " << ExtrapJFakeInclusive << " +/- " <<  Error << endl;
  cout << endl;

  float ExtrapDYNum = 0.;
  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"], variable, ExtrapDYNum,                  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbin);
  float ExtrapDYNuminSidebandRegion = 0.;
  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"], variable, ExtrapDYNuminSidebandRegion,  Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel&&apZ);
  float ExtrapDYNumPZetaRel = 0.;
  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTau"], variable, ExtrapDYNumPZetaRel,          Error,   Lumi*lumiCorrFactor*hltEff_/1000., hExtrap, sbinPZetaRel);


  float ExtrapolationFactorMadGraph      = ExtrapDYNum/ExtrapDYInclusive;
  float ErrorExtrapolationFactorMadGraph = TMath::Sqrt(ExtrapolationFactorMadGraph*(1-ExtrapolationFactorMadGraph)/ExtrapDYInclusive);
  cout << "Extrap. factor using MadGraph            = " << ExtrapolationFactorMadGraph << " +/- " << ErrorExtrapolationFactorMadGraph << endl;

  float ExtrapEmbedNum = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, ExtrapEmbedNum,                 Error, 1.0, hExtrap, sbinEmbedding);
  float ExtrapEmbedNuminSidebandRegion = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, ExtrapEmbedNuminSidebandRegion, Error, 1.0, hExtrap, sbinEmbeddingPZetaRel&&apZ);
  float ExtrapEmbedNumPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, ExtrapEmbedNumPZetaRel,         Error, 1.0, hExtrap, sbinEmbeddingPZetaRel);
  float ExtrapEmbedDen = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, ExtrapEmbedDen,                 Error, 1.0, hExtrap, sbinEmbeddingInclusive);
  float ExtrapEmbedDenPZetaRel = 0.;
  drawHistogram(sbinEmbeddingPresel,sbinCatIncl, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, ExtrapEmbedDenPZetaRel,         Error, 1.0, hExtrap, sbinPZetaRelEmbeddingInclusive);

  ExtrapolationFactorZ             = ExtrapEmbedNum/ExtrapEmbedDen; 
  ErrorExtrapolationFactorZ        = TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen);
  ExtrapolationFactorZDataMC       = ExtrapolationFactorZ/ExtrapolationFactorMadGraph;
  ExtrapolationFactorZFromSideband = ExtrapEmbedDen/ExtrapEmbedDenPZetaRel;

  float sidebandRatioMadgraph = ExtrapDYNuminSidebandRegion/ExtrapDYNumPZetaRel;
  float sidebandRatioEmbedded = ExtrapEmbedNuminSidebandRegion/ExtrapEmbedNumPZetaRel;
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

  evaluateQCD(mapAllTrees, version_, "MC_WJet",RUN, 0, 0, true, "SS", false, removeMtCut, "inclusive", 
	      SSQCDinSignalRegionDATAIncl , SSIsoToSSAIsoRatioQCD, scaleFactorTTSSIncl,
	      extrapFactorWSSIncl, 
	      SSWinSignalRegionDATAIncl, SSWinSignalRegionMCIncl,
	      SSWinSidebandRegionDATAIncl, SSWinSidebandRegionMCIncl,
 	      hExtrap, variable,
 	      Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
	      ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
 	      ElectoTauCorrectionFactor, JtoTauCorrectionFactor, 
	      OStoSSRatioQCD,
 	      antiWsdb, antiWsgn, useMt,
	      sbinSSInclusive,
	      sbinCatIncl,
 	      sbinPZetaRelSSInclusive, pZ, apZ, sbinPZetaRelSSInclusive, 
//  	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoMtisoInclusive, 
 	      sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIsoInclusive, 
	      vbf, oneJet, zeroJet, sbinCatIncl, sbinCatIncl, sbinPresel, sbinaIsoPresel,
	      true,true,true);

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
      
      currentTree = (it->second);

      cout << "************** BEGIN QCD evaluation using SS events *******************" << endl;

      TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
      float dummyfloat = 0.;      

//       TCut sbinPZetaRelForWextrapolation = sbinPZetaRel;
      TCut sbinCatForWextrapolation = sbinCat;
      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
	sbinCatForWextrapolation = vbfLoose;

      evaluateQCD(mapAllTrees, version_, "MC_WJet",RUN, h1, hCleaner, true, "SS", false, removeMtCut, selection_, 
		  SSQCDinSignalRegionDATA , dummyfloat , scaleFactorTTSS,
		  extrapFactorWSS, 
		  SSWinSignalRegionDATA, SSWinSignalRegionMC,
		  SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
		  hExtrapSS, variable,
		  Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
		  ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
		  ElectoTauCorrectionFactor, JtoTauCorrectionFactor, 
		  OStoSSRatioQCD,
		  antiWsdb, antiWsgn, useMt,
		  sbinSS,
// 		  sbinPZetaRelForWextrapolation,
		  sbinCatForWextrapolation,
		  sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
// 		  sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
		  sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, 
		  vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel,
		  true,true,true);

      cout << "************** END QCD evaluation using SS events *******************" << endl;

      //delete hExtrapSS;
      hExtrapSS->Reset();

      hQCD->Add(h1, 1.0);
      hSS->Add(hCleaner, OStoSSRatioQCD);
      hCleaner->Reset();

      //fine binning for MSSM
      if(selection_.find("nobTag")!=string::npos){
        TH1F* h1fb = new TH1F("h1fb","",400, 0., 2000.); //fine binning histogram for MSSM
	if ( !h1fb->GetSumw2N() ) h1fb->Sumw2();
	TH1F* hExtrapSSfb = new TH1F("hExtrapSSfb","",400, 0., 2000.);
	if ( !hExtrapSSfb->GetSumw2N() ) hExtrapSSfb->Sumw2();
	
	evaluateQCD(mapAllTrees, version_, "MC_WJet", RUN, h1fb, hCleanerfb, true, "SS", false, removeMtCut, selection_,
		    SSQCDinSignalRegionDATA , dummyfloat , scaleFactorTTSS,
		    extrapFactorWSS,
		    SSWinSignalRegionDATA, SSWinSignalRegionMC,
		    SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
		    hExtrapSSfb, variable,
		    Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
		    ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
		    ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
		    OStoSSRatioQCD,
		    antiWsdb, antiWsgn, useMt,
		    sbinSS,
		    //                sbinPZetaRelForWextrapolation,
		    sbinCatForWextrapolation,
		    sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive,
		    //                sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso,
		    sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso,
		    vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel,
		    true,true,true);

	cout << "************** END QCD evaluation using SS events *******************" << endl;
        hQCD_fb->Add(h1fb, h1->Integral()/h1fb->Integral());
        hCleanerfb->Reset();
	delete hExtrapSSfb;
      }
      delete hExtrapSS;
	
    }
    else{

      currentTree = (it->second);

      if(!currentName.Contains("Embed")){

	float Error = 0.;

	if(currentName.Contains("DYToTauTau")){
	  float NormDYToTauTau = 0.;
	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYToTauTau, Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbin, 1);
	  hZtt->Add(h1, ExtrapolationFactorZFromSideband);
	  float NormDYToTauTauLL = 0.;
	  drawHistogram(sbinPresel,sbinCat,"MC",version_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
          hZtt->Add(hCleaner, 1.0);

	  //fine binning for MSSM
          if(selection_.find("bTag")!=string::npos){
            hCleanerfb->Reset(); float NormDYToTauTau_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYToTauTau_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
            hZtt_fb->Add(hCleanerfb, hZtt->Integral()/hCleanerfb->Integral());
            hCleanerfb->Reset();
	    float NormDYToTauTauLL_fb = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZtt_fb->Add(hCleanerfb, 1.0);
	    hZtt_fb->Scale(hZtt->Integral()/hZtt_fb->Integral());
	    hCleanerfb->Reset();
          }
	}
	if(currentName.Contains("TTbar")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){ 
	    float NormTTjets = 0.; 
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight, "MC",version_, RUN, currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose, "MC",version_, RUN, currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleaner, sbin, 1);
	    hTTb->Add(hCleaner, 1.0);
	    NormTTjets = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., h1, sbin, 1);
	    hTTb->Scale(h1->Integral()/hTTb->Integral()); 
	  } 
	  else{
	    float NormTTjets = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormTTjets,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., h1, sbin, 1);
	    hTTb->Add(h1, 1.0);

	    //fine binning for MSSM
            if(selection_.find("bTag")!=string::npos){
              hCleanerfb->Reset(); float NormTTjets_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormTTjets_fb,     Error,   Lumi*TTxsectionRatio/**scaleFactorTTOSWJets*/*hltEff_/1000., hCleanerfb, sbin, 1);
              hTTb_fb->Add(hCleanerfb, hTTb->Integral()/hCleanerfb->Integral());
              hCleanerfb->Reset();
            }
	  }
	}
	else if(currentName.Contains("W3Jets")){

	  TH1F* hExtrapW3Jets = new TH1F("hExtrapW3Jets","",nBins , bins.GetArray());
	 
	  cout << "************** BEGIN W+3jets normalization using high-Mt sideband *******************" << endl;

	  TCut sbinCatForWextrapolation = sbinCat;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    sbinCatForWextrapolation = vbfLoose;
	    if(selection_.find("vbfTight")!=string::npos)
	      sbinCatForWextrapolation = vbfRelaxedTight;
	  }
	  
	  evaluateWextrapolation(mapAllTrees, version_, "MC", RUN, "OS", false, selection_, 
				 extrapFactorWOSW3Jets, 
				 OSWinSignalRegionDATAW3Jets,   OSWinSignalRegionMCW3Jets,
				 OSWinSidebandRegionDATAW3Jets, OSWinSidebandRegionMCW3Jets,
				 scaleFactorTTOSW3Jets,
				 hExtrapW3Jets, variable,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 sbinCatForWextrapolation,
				 sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				 pZ, apZ2, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, oneJet, zeroJet, 
				 sbinCat, sbinPresel, sbinaIsoPresel);

	  cout << "************** END W+3jets normalization using high-Mt sideband *******************" << endl;
	  delete hExtrapW3Jets;

	  float NormW3Jets = 0.;
	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAW3Jets/OSWinSidebandRegionMCW3Jets);
	  else h1->Scale(OSWinSignalRegionDATAW3Jets/h1->Integral());
	  hW3Jets->Add(h1, 1.0);

// 	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinMtiso, 1);
	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);
	  hW3JetsMediumTauIso->Add(hCleaner, hW3Jets->Integral()/hCleaner->Integral());

	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtiso, 1);
	  hW3JetsLooseTauIso->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());

	  if(selection_.find("vbfTight")!=string::npos)
	    drawHistogram(sbinLtisoPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelLtisoInclusive, 1);
	  else
	    drawHistogram(sbinPresel,vbfLoose, "MC",version_, RUN, currentTree, variable, NormW3Jets, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
	  hW3JetsMediumTauIsoRelVBF->Add(hCleaner,  hW3Jets->Integral()/hCleaner->Integral());
	  
	  hW3JetsMediumTauIsoRelVBFMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAW3Jets));

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
	  
	  evaluateWextrapolation(mapAllTrees, version_, "MC_WJet",RUN, "OS", false, selection_, 
				 extrapFactorWOSWJets, 
				 OSWinSignalRegionDATAWJets,   OSWinSignalRegionMCWJets,
				 OSWinSidebandRegionDATAWJets, OSWinSidebandRegionMCWJets,
				 scaleFactorTTOSWJets,
				 hExtrapW, variable,
				 Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				 ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				 ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
				 antiWsdb, antiWsgn, useMt,
				 sbinCatForWextrapolation,
				 sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				 pZ, apZ, sbinPZetaRelInclusive, 
				 sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet, 
				 sbinCat, sbinPresel, sbinaIsoPresel);
	  delete hExtrapW;

	  cout << "************** END W+jets normalization using high-Mt sideband *******************" << endl;

	  float NormSSWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormSSWJets, Error,   Lumi*hltEff_/1000., h1, sbinSS, 1);
	  if(removeMtCut) h1->Scale(SSWinSidebandRegionDATA/SSWinSidebandRegionMC);
	  else h1->Scale(SSWinSignalRegionDATA/h1->Integral());
	  hWSS->Add(h1, 1.0);

	  float NormWJets = 0.;
	  drawHistogram(sbinPresel,sbinCat, "MC_WJet",version_, RUN, currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	  if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	  else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());
	  //hW->Add(h1, 1.0);
	  //hWMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));

	  //fine binning for MSSM
          if(selection_.find("nobTag")!=string::npos){
	    NormWJets = 0.;
	    //drawHistogram(sbinPresel,sbinCat, "MC_WJet",version_, RUN, currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbin, 1);
	    drawHistogram(sbinLtisoPresel,sbinCat, "MC_WJet",version_, RUN, currentTree, variable, NormWJets, Error,   Lumi*hltEff_/1000., h1, sbinLtiso, 1);
	    if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	    else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());

            hCleanerfb->Reset(); float NormWJets_fb = 0.;
            hW_fb->Reset();
	    //drawHistogram(sbinPresel,sbinCat, "MC_WJet",version_, RUN, currentTree, variable, NormWJets_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	    drawHistogram(sbinLtisoPresel,sbinCat, "MC_WJet",version_, RUN, currentTree, variable, NormWJets_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtiso, 1);
            hW_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());
          }
	  if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
            float NormWJetsBTag = 0.;
	    //drawHistogram(sbinPresel,bTagLoose,"MC_WJet",version_, RUN, currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    drawHistogram(sbinLtisoPresel,bTagLoose,"MC_WJet",version_, RUN, currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinLtisoInclusive, 1);
	    hWLooseBTag->Add(hCleaner,  h1->Integral()/hCleaner->Integral());
            hEWK->Add(hWLooseBTag,1.0);

	    //fine binning for MSSM
            hW_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fb = 0.;
	    //drawHistogram(sbinPresel,bTagLoose, "MC_WJet",version_, RUN, currentTree, variable, NormWJetsBTag_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	    drawHistogram(sbinLtisoPresel,bTagLoose, "MC_WJet",version_, RUN, currentTree, variable, NormWJetsBTag_fb, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinLtisoInclusive, 1);
            hW_fb->Add(hCleanerfb,  h1->Integral()/hCleanerfb->Integral());
	  }
	  else if(selection_.find("boostMedium")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormWJets_NoOS, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
	    hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(selection_.find("boostHighlowhiggs")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormWJets_NoOS, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelInclusive, 1);
	    hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(selection_.find("boostHighhighhiggs")!=string::npos){
            float NormWJets_NoOS = 0; hCleaner->Reset();
	    drawHistogram(sbinLtisoPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormWJets_NoOS, Error,   Lumi*hltEff_/1000., hCleaner, sbinChargeRelLtisoInclusive, 1);
            hW->Add(hCleaner, h1->Integral()/hCleaner->Integral());
            hEWK->Add(hCleaner, h1->Integral()/hCleaner->Integral());
          }
	  else if(!((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
	       selection_.find("twoJets")!=string::npos) && !version_.Contains("JetTauFR")) {
	    hW->Reset(); hWMinusSS->Reset();
            hW->Add(h1, 1.0);
	    hWMinusSS->Add(h1, (1-OStoSSRatioQCD*SSWinSidebandRegionDATA/OSWinSidebandRegionDATAWJets));
	    if(!USESSBKG) hEWK->Add(h1,1.0);
	    else hEWK->Add(hWMinusSS,1.0);
	  }
	  //Jet->Tau FR measurement
	  else if(version_.Contains("JetTauFR")){
	    cout<<"WJets from MC"<<endl;
	    float NormWJets_NoOS = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat,"MC_WJet", version_, RUN,currentTree, variable, NormWJets_NoOS,Error,   Lumi*hltEff_/1000., hCleaner, sbin, 1);
	    //             hW->Add(hCleaner, OSWinSignalRegionDATAWJets/hCleaner->Integral());
	    // 	    hEWK->Add(hCleaner, OSWinSignalRegionDATAWJets/hCleaner->Integral());
            hW->Add(hCleaner,h1->Integral()/hCleaner->Integral());
	    hEWK->Add(hCleaner,h1->Integral()/hCleaner->Integral());
	    cout<<"Scale WJets "<<h1->Integral()/hCleaner->Integral()<<endl;
	  }
	  
	  //Get W Up/Down histograms as well
	  if(selection_.find("bTag")!=string::npos && MSSM){

	    TH1F* hExtrapWUp = new TH1F("hExtrapUp","",nBins , bins.GetArray());

	    cout << "************** BEGIN W+jets (Up) normalization using high-Mt sideband *******************" << endl;

	    float OSWinSignalRegionDATAWJetsUp = 0.; float OSWinSignalRegionMCWJetsUp = 0.;
	    float OSWinSidebandRegionDATAWJetsUp = 0.; float OSWinSidebandRegionMCWJetsUp = 0.;
	    float scaleFactorTTOSWJetsUp = 0.; float extrapFactorWOSWJetsUp = 0.;
	    
	    evaluateWextrapolation(mapAllTrees, version_, "MC_WJetUp",RUN, "OS", false, selection_,
				   extrapFactorWOSWJetsUp,
				   OSWinSignalRegionDATAWJetsUp,   OSWinSignalRegionMCWJetsUp,
				   OSWinSidebandRegionDATAWJetsUp, OSWinSidebandRegionMCWJetsUp,
				   scaleFactorTTOSWJetsUp,
				   hExtrapWUp, variable,
				   Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				   ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				   ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
				   antiWsdb, antiWsgn, useMt,
				   sbinCatForWextrapolation,
				   sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				   pZ, apZ, sbinPZetaRelInclusive,
				   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet,
				   sbinCat, sbinPresel, sbinaIsoPresel);
	    delete hExtrapWUp;

	    cout << "************** END W+jets normalization using high-Mt sideband *******************" << endl;
	    
	    float NormWJetsUp = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC_WJetUp",version_, RUN, currentTree, variable, NormWJetsUp, Error, Lumi*hltEff_/1000., h1, sbin, 1);
	    if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	    else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());

	    hW_TFUp->Reset();
	    hW_TFUp->Add(h1, 1.0);
	    cout<<"hW_TFUp->Integral()"<<hW_TFUp->Integral()<<endl;

	    if(selection_.find("nobTag")!=string::npos){
	      hCleanerfb->Reset(); float NormWJets_fbUp = 0.;
	      hW_TFUp_fb->Reset();
	      drawHistogram(sbinPresel,sbinCat, "MC_WJetUp",version_, RUN, currentTree, variable, NormWJets_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hW_TFUp_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());
	      cout<<"hW_TFUp_fb->Integral()"<<hW_TFUp_fb->Integral()<<endl;
	    }
	    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	      float NormWJetsBTag = 0.; hWLooseBTag_TFUp->Reset();
	      drawHistogram(sbinPresel,bTagLoose, "MC_WJetUp",version_, RUN, currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      hWLooseBTag_TFUp->Add(hCleaner,  h1->Integral()/hCleaner->Integral());
	      cout<<"hWLooseBTag_TFUp->Integral()"<<hWLooseBTag_TFUp->Integral()<<endl;

	      //fine binning for MSSM
	      hW_TFUp_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fbUp = 0.;
	      drawHistogram(sbinPresel,bTagLoose, "MC_WJetUp",version_, RUN, currentTree, variable, NormWJetsBTag_fbUp, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	      hW_TFUp_fb->Add(hCleanerfb,  h1->Integral()/hCleanerfb->Integral());
	    cout<<"hW_TFUp_fb->Integral()"<<hW_TFUp_fb->Integral()<<endl;

	    }

	    TH1F* hExtrapWDown = new TH1F("hExtrapDown","",nBins , bins.GetArray());
	    cout << "************** BEGIN W+jets (Down) normalization using high-Mt sideband *******************" << endl;

	    float OSWinSignalRegionDATAWJetsDown = 0.; float OSWinSignalRegionMCWJetsDown = 0.;
	    float OSWinSidebandRegionDATAWJetsDown = 0.; float OSWinSidebandRegionMCWJetsDown = 0.;
	    float scaleFactorTTOSWJetsDown = 0.; float extrapFactorWOSWJetsDown = 0.;

	    evaluateWextrapolation(mapAllTrees, version_, "MC_WJetDown",RUN, "OS", false, selection_,
				   extrapFactorWOSWJetsDown,
				   OSWinSignalRegionDATAWJetsDown,   OSWinSignalRegionMCWJetsDown,
				   OSWinSidebandRegionDATAWJetsDown, OSWinSidebandRegionMCWJetsDown,
				   scaleFactorTTOSWJetsDown,
				   hExtrapWDown, variable,
				   Lumi*hltEff_/1000., TTxsectionRatio, lumiCorrFactor,
				   ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
				   ElectoTauCorrectionFactor, JtoTauCorrectionFactor,
				   antiWsdb, antiWsgn, useMt,
				   sbinCatForWextrapolation,
				   sbinPZetaRel, sbinChargeRelPZetaRelInclusive,
				   pZ, apZ, sbinPZetaRelInclusive,
				   sbinPZetaRelaIsoInclusive, sbinPZetaRelaIso, vbfLoose, boost, zeroJet,
				   sbinCat, sbinPresel, sbinaIsoPresel);
	    delete hExtrapWDown;
	    cout << "************** END W+jets normalization using high-Mt sideband *******************" << endl;

	    float NormWJetsDown = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC_WJetDown",version_, RUN, currentTree, variable, NormWJetsDown, Error, Lumi*hltEff_/1000., h1, sbin, 1);
	    if(removeMtCut) h1->Scale(OSWinSidebandRegionDATAWJets/OSWinSidebandRegionMCWJets);
	    else h1->Scale(OSWinSignalRegionDATAWJets/h1->Integral());

	    hW_TFDown->Reset();
	    hW_TFDown->Add(h1, 1.0);
	    cout<<"hW_TFDown->Integral()"<<hW_TFDown->Integral()<<endl;

	    if(selection_.find("nobTag")!=string::npos){
	      hCleanerfb->Reset(); float NormWJets_fbDown = 0.;
	      hW_TFDown_fb->Reset();
	      drawHistogram(sbinPresel,sbinCat, "MC_WJetDown",version_, RUN, currentTree, variable, NormWJets_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hW_TFDown_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());
	      cout<<"hW_TFDown_fb->Integral()"<<hW_TFDown_fb->Integral()<<endl;
	    }
	    else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	      float NormWJetsBTag = 0.; hWLooseBTag_TFDown->Reset();
	      drawHistogram(sbinPresel,bTagLoose, "MC_WJetDown",version_, RUN, currentTree, variable, NormWJetsBTag, Error,   Lumi*hltEff_/1000., hCleaner, sbinInclusive, 1);
	      hWLooseBTag_TFDown->Add(hCleaner,  h1->Integral()/hCleaner->Integral());
	      cout<<"hWLooseBTag_TFDown->Integral()"<<hWLooseBTag_TFDown->Integral()<<endl;

	      //fine binning for MSSM
	      hW_TFDown_fb->Reset();hCleanerfb->Reset(); float NormWJetsBTag_fbDown = 0.;
	      drawHistogram(sbinPresel,bTagLoose, "MC_WJetDown",version_, RUN, currentTree, variable, NormWJetsBTag_fbDown, Error,   Lumi*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	      hW_TFDown_fb->Add(hCleanerfb,  h1->Integral()/hCleanerfb->Integral());
	      cout<<"hW_TFDown_fb->Integral()"<<hW_TFDown_fb->Integral()<<endl;
	    }

	  }//Get W Up/Down histograms as well
	}	

	else if(currentName.Contains("DYElectoTau")){
	  if(selection_.find("vbfTight")!=string::npos){
	    float NormDYElectoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    //Take the shape from relaxed VBF cuts with jet Pt 20 GeV (update for summer)
	    float NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale( h1->Integral()/hCleaner->Integral() );
	    hZmm->Add(hCleaner, 1.0);
	    hZfakes->Add(hCleaner,1.0);
	    //hEWK->Add(h1,1.0);

	    //2% Up/Down
	    NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC_ZLUp", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedTightZL,"MC_ZLDown", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	  }
	  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){  

	    // Shape from MC (h1) with the twoJets selection
	    float NormDYElectoTau = 0.;
	    /* //Not used this method with embed anymore
	       if(useZDataMC)
	       drawHistogram(sbinPresel,twoJets, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	       else
	       drawHistogram(sbinPresel,twoJets, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);

	       // Norm from MC with the inclusive selection
	       //float NormDYElectoTauIncl = 0.;
	       //drawHistogram(sbinCatIncl, "MC",version_, RUN, currentTree, variable, NormDYElectoTauIncl, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbin, 1);

	       // Compute efficiency of the full vbf category in the embedded sample
	       float NormDYElectoTauEmbedLoose = 0.; 
	       drawHistogram(sbinEmbeddingPresel,twoJets, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbedLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1);
	       //drawHistogram(sbinCatIncl, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbedLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1);
	       hCleaner->Reset();
	       float NormDYElectoTauEmbed = 0.;
	       drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbed,  Error, 1.0 , hCleaner,  sbinEmbedding  ,1);

	       // Apply eff from 2jet to category sel.
	       NormDYElectoTau = (NormDYElectoTauEmbedLoose>0) ? (NormDYElectoTau * NormDYElectoTauEmbed/NormDYElectoTauEmbedLoose) : 0.; 
	       h1->Scale(NormDYElectoTau/h1->Integral()); // Norm = DYMC(sbinIncl && twoJets)*( Emb(sbin) / Emb(sbinIncl && twoJets) )
	    */
	    //Take yield from full selection
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    //Take the shape from relaxed VBF cuts with jet Pt 20 GeV (update for summer)
	    float NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedZL,"MC", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale( NormDYElectoTau/hCleaner->Integral() );
	    hZmm->Add(hCleaner, 1.0); 
	    hZfakes->Add(hCleaner,1.0); 
	    //hEWK->Add(h1,1.0);
	    //2% Up/Down
	    NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedZL,"MC_ZLUp", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYElectoTauShape = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,vbfRelaxedZL,"MC_ZLDown", version_, RUN,currentTree, variable, NormDYElectoTauShape, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	  }  
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos) {
	    /* //this method not used anymore, keep it for backup
	    // Shape from MC (h1) with the oneJet selection
	    float NormDYElectoTau = 0.;
	    drawHistogram(oneJet, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);

	    // Norm from MC with the inclusive selection
	    //float NormDYElectoTauIncl = 0.;
	    //drawHistogram(sbinCatIncl, "MC",version_, RUN, currentTree, variable, NormDYElectoTauIncl, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbin, 1);

	    // Compute efficiency of the full category in the embedded sample
	    float NormDYElectoTauEmbedLoose = 0.; 
	    drawHistogram(oneJet, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbedLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1);
	    //drawHistogram(sbinCatIncl, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbedLoose,  Error, 1.0 , hCleaner,  sbinEmbeddingInclusive  ,1);
	    hCleaner->Reset();
	    float NormDYElectoTauEmbed = 0.;
	    drawHistogram(sbinCat, "Embed", version_, RUN, mapAllTrees["Embedded"], variable, NormDYElectoTauEmbed,  Error, 1.0 , hCleaner,  sbinEmbedding  ,1);

	    // Apply eff from 2jet to category sel.
	    NormDYElectoTau = (NormDYElectoTauEmbedLoose>0) ? (NormDYElectoTau * NormDYElectoTauEmbed/NormDYElectoTauEmbedLoose) : 0.; 
	    h1->Scale(NormDYElectoTau/h1->Integral()); // Norm = DYMC(sbinIncl && twoJets)*( Emb(sbin) / Emb(sbinIncl && twoJets) )
	    */
	    float NormDYElectoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    //shape from loose bTag selection
	    drawHistogram(sbinPresel,bTagLoose, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
	    hZmm->Add(hCleaner, 1.0); 
	    hZfakes->Add(hCleaner,1.0); 
	    //hEWK->Add(h1,1.0);

	    //% ZLUp/Down
	    NormDYElectoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,bTagLoose, "MC_ZLUp",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYElectoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,bTagLoose, "MC_ZLDown",version_, RUN, currentTree, variable, NormDYElectoTau, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinInclusive, 1);
	    hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());

	    //fine binning for MSSM
	    hCleanerfb->Reset();hZmm_fb->Reset(); float NormDYElectoTau_fb = 0;
	    drawHistogram(sbinPresel,bTagLoose, "MC",version_, RUN, currentTree, variable, NormDYElectoTau_fb, Error,Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinInclusive, 1);
	    hZmm_fb->Add(hCleanerfb, hZmm->Integral()/hCleanerfb->Integral());
	  }
	  else{
	    float NormDYElectoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmm->Add(h1, 1.0);
	    hZfakes->Add(h1,1.0);
	    //hEWK->Add(h1,1.0);

	    //2% ZLUp/Down
	    NormDYElectoTau = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC_ZLUp",version_, RUN, currentTree, variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmm_Up->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());
	    NormDYElectoTau = 0.; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC_ZLDown",version_, RUN, currentTree, variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmm_Down->Add(hCleaner, hZmm->Integral()/hCleaner->Integral());

	    //fine binning for MSSM
	    if(selection_.find("nobTag")!=string::npos){
	      hCleanerfb->Reset();hZmm_fb->Reset(); float NormDYElectoTau_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYElectoTau_fb, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hZmm_fb->Add(hCleanerfb, hZmm->Integral()/hCleanerfb->Integral());
	    }
	  }
	}
	else if(currentName.Contains("DYJtoTau")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){   
	    float NormDYJtoTau = 0.; 
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    NormDYJtoTau = 0.; 
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1); 
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
	    hZmj->Add(hCleaner, 1.0); 
	    hZfakes->Add(hCleaner,1.0); 
	    //hEWK->Add(hCleaner,1.0); 

	    //Add ZTTJ component to ZJ
	    float NormDYTTJtoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmj->Add(hCleaner, 1.0);
	    hZfakes->Add(hCleaner,1.0);
	  }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){   
	    float NormDYJtoTau = 0.; 
	    drawHistogram(sbinPresel,oneJet, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbin, 1);
	    NormDYJtoTau = 0.; 
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1); 
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral());
	    hZmj->Add(hCleaner, 1.0); 
	    hZfakes->Add(hCleaner,1.0); 
	    //hEWK->Add(hCleaner,1.0); 

	    //Add ZTTJ component to ZJ
	    float NormDYTTJtoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmj->Add(hCleaner, 1.0);
	    hZfakes->Add(hCleaner,1.0);

	    //fine binning for MSSM
	    hCleanerfb->Reset();hZmj_fb->Reset(); float NormDYJtoTau_fb = 0.;
	    drawHistogram(sbinPresel,oneJet, "MC",version_, RUN, currentTree, variable, NormDYJtoTau_fb, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZmj_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());

	    //fine binning for MSSM
	    float NormDYTTJtoTau_fb = 0; hCleanerfb->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau_fb, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	    hZmj_fb->Add(hCleanerfb, 1.0);

	  } 
	  else{
	    float NormDYJtoTau = 0.;
	    if(useZDataMC)
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., h1, sbin, 1);
	    else
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., h1, sbin, 1);
	    hZmj->Add(h1, 1.0);
	    hZfakes->Add(h1,1.0);
	    hEWK->Add(h1,1.0);
	    
	    //Add ZTTJ component to ZJ
	    float NormDYTTJtoTau = 0; hCleaner->Reset();
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hZmj->Add(hCleaner, 1.0);
	    hZfakes->Add(hCleaner,1.0);

	    //fine binning for MSSM
	    if(selection_.find("nobTag")!=string::npos){
	      hCleanerfb->Reset();hZmj_fb->Reset(); float NormDYJtoTau_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormDYJtoTau_fb, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hZmj_fb->Add(hCleanerfb, h1->Integral()/hCleanerfb->Integral());

	      //Add ZTTJ component to ZJ
	      float NormDYTTJtoTau_fb = 0; hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYToTauTauJJ"], variable, NormDYTTJtoTau_fb, Error,    Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hZmj_fb->Add(hCleanerfb, 1.0);
	    }

	  }
	}
	else if(currentName.Contains("Others")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){    
	    float NormOthers = 0.;  
	    if(selection_.find("vbfTight")!=string::npos)
	      drawHistogram(sbinPresel,vbfRelaxedTight, "MC",version_, RUN, currentTree, variable, NormOthers, Error,     Lumi*hltEff_/1000., hCleaner, sbin, 1);
	    else
	      drawHistogram(sbinPresel,vbfLoose, "MC",version_, RUN, currentTree, variable, NormOthers, Error,     Lumi*hltEff_/1000., hCleaner, sbin, 1);
	    NormOthers = 0.; 
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
	    hCleaner->Scale(h1->Integral()/hCleaner->Integral()); 
	    hVV->Add(hCleaner, 1.0);  
	    hEWK->Add(hCleaner,1.0);  
	  } 
	  else{
	    float NormOthers = 0.;
	    drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormOthers , Error,     Lumi*hltEff_/1000., h1, sbin, 1);
	    hVV->Add(h1, 1.0);
	    hEWK->Add(h1,1.0);

	    //fine binning for MSSM
	    if(selection_.find("bTag")!=string::npos){
	      hCleanerfb->Reset();hVV_fb->Reset(); float NormOthers_fb = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormOthers_fb, Error,     Lumi*hltEff_/1000., hCleanerfb, sbin, 1);
	      hVV_fb->Add(hCleanerfb, hVV->Integral()/hCleanerfb->Integral());
	    }
	  }
	}
	else if(currentName.Contains("Data")){

	  float NormData = 0.;
	  drawHistogram(sbinPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , h1, sbin, 1);
	  hData->Add(h1, 1.0);
	  if ( !hData->GetSumw2N() ) hData->Sumw2();
	  
	  if(selection_.find("vbfTight")!=string::npos && selection_.find("novbf")==string::npos){
	    //drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    //float tmpNorm = hCleaner->Integral();
	    drawHistogram(sbinaIsoPresel,vbfRelaxedTightQCD, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    //hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral()));
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    
	    //get efficiency of events passing QCD selection to pass the category selection
	    drawHistogram(sbinaIsoLtisoPresel,sbinCatIncl, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1);
	    float tmpNormQCDSel = hCleaner->Integral();
	    drawHistogram(sbinaIsoLtisoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso,1);
	    float tmpNormCatSel = hCleaner->Integral();
	    float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
	    //Normalize to Inclusive measured QCD times the above efficiency
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    // 	    drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtiso ,1);
	    //drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    //float tmpNorm = hCleaner->Integral();
	    // 	    drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtisoInclusive&&vbfLooseQCD ,1);
	    drawHistogram(sbinaIsoPresel,vbfLooseQCD, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    //hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD*(tmpNorm/hCleaner->Integral()));
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    cout<<"vbfLoose shape integral :"<< hDataAntiIsoLooseTauIso->Integral()<<endl;

	    //get efficiency of events passing QCD selection to pass the category selection 
	    drawHistogram(sbinaIsoPresel,sbinCatIncl, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive ,1);
	    float tmpNormQCDSel = hCleaner->Integral();
	    drawHistogram(sbinaIsoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1); 
	    float tmpNormCatSel = hCleaner->Integral();
	    float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
	    //Normalize to Inclusive measured QCD times the above efficiency
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	    cout<<"effQCDToCatSel*SSQCDinSignalRegionDATAIncl :"<<effQCDToCatSel*SSQCDinSignalRegionDATAIncl<<endl;
	    cout<<"hDataAntiIsoLooseTauIsoQCD->Integral() :"<<hDataAntiIsoLooseTauIsoQCD->Integral()<<endl;
	  }
	  else if(selection_.find("boostHighhighhiggs")!=string::npos || selection_.find("boostHighmediumhiggs")!=string::npos){
	    drawHistogram(sbinaIsoLtisoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner);

	    //get efficiency of events passing QCD selection to pass the category selection
	    drawHistogram(sbinaIsoLtisoPresel,sbinCatIncl, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtisoInclusive ,1);
	    float tmpNormQCDSel = hCleaner->Integral();
	    drawHistogram(sbinaIsoLtisoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso,1);
	    float tmpNormCatSel = hCleaner->Integral();
	    float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel;
	    //Normalize to Inclusive measured QCD times the above efficiency
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral());
	  }
	  //else if(selection_.find("novbfHigh")!=string::npos || selection_.find("novbfMedium")!=string::npos || selection_.find("boostHigh")!=string::npos){
	  else if(selection_.find("novbfHigh")!=string::npos || selection_.find("boostHigh")!=string::npos){
	    drawHistogram(sbinaIsoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner);
	    /* //No need to subtract anyother background contributions, summer recommendation
	       float NormDYElectoTau = 0;
	       //drawHistogram(sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	       drawHistogram(sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	       hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	       float NormDYJtoTau = 0.; 
	       //drawHistogram(sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	       drawHistogram(sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	       hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	       float NormTTjets = 0.; 
	       drawHistogram(sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"], variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTSS*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	       hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0);
	    */
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("novbfLow")!=string::npos || selection_.find("novbfMedium")!=string::npos || selection_.find("boostMedium")!=string::npos) {
	    TH1F* hExtrapSS = new TH1F("hExtrapSS","",nBins , bins.GetArray());
	    float dummy1 = 0.;      
	    evaluateQCD(mapAllTrees, version_, "MC_WJet",RUN, hDataAntiIsoLooseTauIso, hCleaner, true, "SS", false, removeMtCut, selection_, 
			SSQCDinSignalRegionDATA , dummy1 , scaleFactorTTSS,
			extrapFactorWSS, 
			SSWinSignalRegionDATA, SSWinSignalRegionMC,
			SSWinSidebandRegionDATA, SSWinSidebandRegionMC,
			hExtrapSS, variable,
			Lumi/1000*hltEff_,  TTxsectionRatio, lumiCorrFactor,
			ExtrapolationFactorSidebandZDataMC, ExtrapolationFactorZDataMC,
			ElectoTauCorrectionFactor, JtoTauCorrectionFactor, 
			OStoSSRatioQCD,
			antiWsdb, antiWsgn, useMt,
			sbinSS,
			sbinCat,
			sbinPZetaRelSS, pZ, apZ, sbinPZetaRelSSInclusive, 
			// 			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIsoMtiso, 
			sbinPZetaRelSSaIsoInclusive, sbinPZetaRelSSaIso, sbinPZetaRelSSaIso, 
			vbfLoose, oneJet, zeroJet, sbinCat, sbinCat, sbinPresel, sbinaIsoPresel,
			true, true, true);
	    
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    delete hExtrapSS;
	    hCleaner->Reset();
	  }
	  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
	    //drawHistogram(sbinaIsoPresel,bTagLoose/*sbinCat*/, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1); 
	    drawHistogram(sbinaIsoLtisoPresel,bTagLoose/*sbinCat*/, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso ,1); 
	    cout<<"hCleaner->Integral() : "<<hCleaner->Integral()<<endl;
	    hDataAntiIsoLooseTauIso->Add(hCleaner); 
	    //fine binning histogram for MSSM
	    hCleanerfb->Reset(); hQCD_fb->Reset(); float NormData_fb = 0.;
	    //drawHistogram(sbinaIsoPresel,bTagLoose/*sbinCat*/, "Data", version_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, sbinSSaIso ,1);
	    drawHistogram(sbinaIsoLtisoPresel,bTagLoose/*sbinCat*/, "Data", version_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, sbinSSaIsoLtiso ,1);
	    hQCD_fb->Add(hCleanerfb);

	    //Background substraction
	    //             float NormDYElectoTau = 0; 
	    // 	    if(useZDataMC)
	    // 	      drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    // 	    else
	    // 	      drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormDYElectoTau, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    // 	    cout<<" - hCleaner->Integral() : "<<hCleaner->Integral()<<endl;
	    //             hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    // 	    hCleanerfb->Reset(); float NormDYElectoTau_fb = 0.;
	    // 	    drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYElectoTau"], variable, NormDYElectoTau_fb, Error,   Lumi*lumiCorrFactor*ElectoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    //             hQCD_fb->Add(hCleanerfb, -1.0);

	    //             float NormDYJtoTau = 0.;  
	    // 	    if(useZDataMC)
	    // 	      drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*ExtrapolationFactorZDataMC*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    // 	    else
	    // 	      drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, NormDYJtoTau, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleaner, sbinSSaIso, 1);
	    // 	    cout<<" - hCleaner->Integral() : "<<hCleaner->Integral()<<endl;
	    //             hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    // 	    hCleanerfb->Reset(); float NormDYJtoTau_fb = 0.;
	    // 	    drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["DYJtoTau"], variable, NormDYJtoTau_fb, Error,    Lumi*lumiCorrFactor*JtoTauCorrectionFactor*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    //             hQCD_fb->Add(hCleanerfb, -1.0);

	    //             float NormTTjets = 0.;  
	    //             drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"], variable, NormTTjets,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleaner, sbinSSaIso, 1); 
	    // 	    cout<<" - hCleaner->Integral() : "<<hCleaner->Integral()<<endl;
	    //             hDataAntiIsoLooseTauIso->Add(hCleaner, -1.0); 
	    // 	    hCleanerfb->Reset(); float NormTTjets_fb = 0.;
	    // 	    drawHistogram(sbinaIsoPresel,sbinCat, "MC",version_, RUN, mapAllTrees["TTbar"], variable, NormTTjets_fb,     Error,   Lumi*TTxsectionRatio*scaleFactorTTOSWJets*hltEff_/1000., hCleanerfb, sbinSSaIso, 1);
	    //             hQCD_fb->Add(hCleanerfb, -1.0);

	    if(selection_.find("High")!=string::npos){
	      //get efficiency of events passing QCD selection to pass the category selection  
	      drawHistogram(sbinaIsoPresel,sbinCatIncl, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoInclusive,1); 
	      float tmpNormQCDSel = hCleaner->Integral(); 
	      drawHistogram(sbinaIsoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso, 1);  
	      float tmpNormCatSel = hCleaner->Integral(); 
	      float effQCDToCatSel = tmpNormCatSel/tmpNormQCDSel; 
	      //Normalize to Inclusive measured QCD times the above efficiency 
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, (effQCDToCatSel*SSQCDinSignalRegionDATAIncl)/hDataAntiIsoLooseTauIso->Integral()); 
	    }
	    else{
	      cout<<"hDataAntiIsoLooseTauIso->Integral() =  "<<hDataAntiIsoLooseTauIso->Integral()<<endl;
	      hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	      hQCD_fb->Scale(hQCD->Integral()/hQCD_fb->Integral());
	    }
	  }
	  else{
	    // 	    drawHistogram(sbinaIsoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoMtiso ,1);
	    //drawHistogram(sbinaIsoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIso ,1);
	    drawHistogram(sbinaIsoLtisoPresel,sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner, sbinSSaIsoLtiso ,1);
	    hDataAntiIsoLooseTauIso->Add(hCleaner, SSIsoToSSAIsoRatioQCD);
	    hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	    //Finebins QCD
	    hCleanerfb->Reset(); hQCD_fb->Reset(); float NormData_fb = 0.; 
	    //drawHistogram(sbinaIsoPresel,bTagLoose,"Data", version_,analysis_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, sbinSSaIso ,1);
	    drawHistogram(sbinaIsoLtisoPresel,bTagLoose,"Data", version_, RUN, currentTree, variable, NormData_fb,  Error, 1.0 , hCleanerfb, sbinSSaIsoLtiso ,1);
	    hQCD_fb->Add(hCleanerfb);
	    hQCD_fb->Scale(hQCD->Integral()/hQCD_fb->Integral());
	  }

	  //hDataAntiIsoLooseTauIsoQCD->Add(hDataAntiIsoLooseTauIso, hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
	  /* //Not necessary for the analysis at the moment
	     drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso1 ,1);
	     hLooseIso1->Add(hCleaner, 1.0);
	     drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso2 ,1);
	     hLooseIso2->Add(hCleaner, 1.0);
	     drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSlIso3 ,1);
	     hLooseIso3->Add(hCleaner, 1.0);
	     drawHistogram(sbinCat, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso  ,1);
	     hAntiIso->Add(hCleaner, 1.0);
	     drawHistogram(sbinCat, "Data_FR", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSaIso  ,1);
	     hAntiIsoFR->Add(hCleaner, 1.0);
	  
	     cleanQCDHisto(mapAllTrees, version_, RUN, true,hCleaner, hLooseIso1, variable, 
	     Lumi*hltEff_/1000., (SSWinSidebandRegionDATA/SSWinSidebandRegionMC), 
	     TTxsectionRatio*scaleFactorTTSS, ElectoTauCorrectionFactor*lumiCorrFactor, 
	     JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso1);
	     cleanQCDHisto(mapAllTrees, version_, RUN, true,hCleaner, hLooseIso2, variable, 
	     Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
	     TTxsectionRatio*scaleFactorTTSS, ElectoTauCorrectionFactor*lumiCorrFactor, 
	     JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso2);
	     cleanQCDHisto(mapAllTrees, version_, RUN, true,hCleaner, hLooseIso3, variable, 
	     Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
	     TTxsectionRatio*scaleFactorTTSS, ElectoTauCorrectionFactor*lumiCorrFactor, 
	     JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSlIso3);


	     drawHistogram(vbfLoose, "Data", version_, RUN, currentTree, variable, NormData,  Error, 1.0 , hCleaner,  sbinSSInclusive , 1);
	     hSSLooseVBF->Add(hCleaner, 1.0);
	     cleanQCDHisto(mapAllTrees, version_, RUN, false,hCleaner, hSSLooseVBF, variable, 
	     Lumi*hltEff_/1000., SSWinSidebandRegionDATA/SSWinSidebandRegionMC, 
	     TTxsectionRatio*scaleFactorTTSS, ElectoTauCorrectionFactor*lumiCorrFactor, 
	     JtoTauCorrectionFactor*lumiCorrFactor, lumiCorrFactor*ExtrapolationFactorZDataMC,sbinSSInclusive,vbfLoose);
	     hSSLooseVBF->Scale(hSS->Integral()/hSSLooseVBF->Integral());
	  */
	}

	else if(currentName.Contains("VBFH")  || 
		currentName.Contains("GGFH")  ||
		currentName.Contains("VH")    ||
		currentName.Contains("SUSY")){

	  float NormSign = 0.;
	  drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, sbin, 1);

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

	  for(int iM=0 ; iM<nMasses ; iM++){
	    if(currentName.Contains("GGFH"+nameMasses[iM])){
	      hCleaner->Reset(); float NormSignUp = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MCGGFHUp",version_, RUN, currentTree, variable, NormSignUp, Error,    Lumi*hltEff_/1000., hCleaner, sbin, 1);
	      hGGFHUp[iM]->Add(hCleaner,1.0);
	      hCleaner->Reset(); float NormSignDown = 0.;
	      drawHistogram(sbinPresel,sbinCat, "MCGGFHDown",version_, RUN, currentTree, variable, NormSignDown, Error,    Lumi*hltEff_/1000., hCleaner, sbin, 1);
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
	      drawHistogram(sbinPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormSign, Error,    Lumi*hltEff_/1000., h1, (sbin&&HWidth), 1);

	      float crossSection = 1.0;
	      for(int iP=0 ; iP<nProdS ; iP++)
		for(int iM=0 ; iM<nMassesS ; iM++){
		  if(nameMassesS[iM]=="130"){
		    cout<<"Modifying xsec : m=130 tanBeta=5"<<endl;
		    crossSection = 1.76395701575004349;
		  }
		  else if(nameMassesS[iM]=="300"){
		    cout<<"Modifying xsec : m=300 tanBeta=12"<<endl;
		    crossSection = 0.188078490754561067;
		  }
		  else if(nameMassesS[iM]=="600"){
		    cout<<"Modifying xsec : m=600 tanBeta=35"<<endl;
		    crossSection = 0.0519465144122727707;
		  }
		  //if(currentName.Contains(nameProdS[iP]+nameMassesS[iM]))
		  TString ProcessName("SUSY"+nameProdS[iP]+nameMassesS[iM]);
		  if(currentName==ProcessName)
		    hSusy[iP][iM]->Add(h1,1.0*crossSection);
		}
	      // 	    TH1F* histoSusy =  (mapSUSYhistos.find( (it->first) ))->second;
	      // 	    histoSusy->Add(h1,1.0);
	      // 	    histoSusy->SetLineWidth(2);
	    }
	  }//SUSY
	}
      }
      else{
	if(!currentName.Contains("TTbarEmbedded")){
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    float NormEmbed = 0.;
	    // 	  TCut sbinEmbeddingLoose = sbinEmbeddingInclusive && vbfLoose;
	    // 	  drawHistogram(sbinCat, "Embed", version_, RUN, currentTree, variable, NormEmbed,  Error, 1.0 , hCleaner,  sbinEmbeddingLoose  ,1);
	    // 	  hDataEmb->Add(hCleaner, 1.0);
	    // 	  NormEmbed = 0.; 
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1); 
	    h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral()); 
	    hDataEmb->Add(h1, 1.0);
	    //remove TTbar embedded contamination
	    h1->Reset();
	    float NormTTjetsEmb = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MC",version_, RUN, backgroundTTbarEmb, variable, NormTTjetsEmb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
	    hTTbEmb->Add(h1, 1.0);
	    cout<<"TTbarEmbedded : "<<hTTbEmb->Integral()<<endl;
	    hDataEmb->Add(hTTbEmb, -1.0);
	    // 	  hDataEmb->Scale(h1->Integral()/hDataEmb->Integral());

	    //Add ZTTLL
	    float NormDYToTauTauLL = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hDataEmb->Add(hCleaner, 1.0);
	  }
	  else{
	    float NormEmbed = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, currentTree, variable, NormEmbed,  Error, 1.0 , h1,  sbinEmbedding  ,1);
	    h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusivePZetaRel*ExtrapolationFactorZFromSideband)/h1->Integral());
	    hDataEmb->Add(h1, 1.0);
	    //remove TTbar embedded contamination
	    h1->Reset();
	    float NormTTjetsEmb = 0.;
	    drawHistogram(sbinEmbeddingPresel,sbinCat, "MC",version_, RUN, backgroundTTbarEmb, variable, NormTTjetsEmb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
	    hTTbEmb->Add(h1, 1.0);
	    cout<<"TTbarEmbedded : "<<hTTbEmb->Integral()<<endl;
	    hDataEmb->Add(hTTbEmb, -1.0);

	    //Add ZTTLL
	    float NormDYToTauTauLL = 0.;
	    drawHistogram(sbinPresel,sbinCat,"MC", version_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleaner, sbin, 1);
	    hDataEmb->Add(hCleaner, 1.0);

	    //fine binning for MSSM
	    if(selection_.find("bTag")!=string::npos){
	      hCleanerfb->Reset(); float NormEmbed_fb = 0.;
	      drawHistogram(sbinEmbeddingPresel,sbinCat, "Embed", version_, RUN, currentTree, variable, NormEmbed_fb,  Error, 1.0 , hCleanerfb,  sbinEmbedding  ,1);
	      hDataEmb_fb->Add(hCleanerfb, hDataEmb->Integral()/hCleanerfb->Integral());
	      float NormTTjetsEmb_fb = 0.;
	      //remove TTbar embedded contamination
	      hCleanerfb->Reset();
	      drawHistogram(sbinEmbeddingPresel,sbinCat, "MC",version_, RUN, backgroundTTbarEmb, variable, NormTTjetsEmb_fb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbinEmbedding, 1);
	      hTTbEmb_fb->Add(hCleanerfb, 1.0);
	      cout<<"TTbarEmbedded : "<<hTTbEmb_fb->Integral()<<endl;
	      hDataEmb_fb->Add(hTTbEmb_fb, -1.0);

	      //Add ZTTLL
	      float NormDYToTauTauLL_fb = 0.; 
	      hCleanerfb->Reset();
	      drawHistogram(sbinPresel,sbinCat,"MC", version_, RUN, backgroundDYTauTauLL, variable, NormDYToTauTauLL_fb, Error,   Lumi*lumiCorrFactor*hltEff_/1000., hCleanerfb, sbin, 1);
	      hDataEmb_fb->Add(hCleanerfb, 1.0);
	      hDataEmb_fb->Scale(hDataEmb->Integral()/hDataEmb_fb->Integral());
	    }
	  }
	}
	else {//TTbarEmbedded
	  float NormTTjetsEmb = 0.;
	  drawHistogram(sbinEmbeddingPresel,sbinCat, "MC",version_, RUN, currentTree, variable, NormTTjetsEmb,     Error,   Lumi*lumiCorrFactor*hltEff_/1000., h1, sbinEmbedding, 1);
	  hTTbEmb->Add(h1, 1.0);
	  cout<<"TTbarEmbedded : "<<hTTbEmb->Integral()<<endl;
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

  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass") && selection_!="inclusive"){ 
    hData->Scale(1.0, "width");
    hTTb->Scale(1.0, "width");
    hDataEmb->Scale(1.0, "width");
    hZmm->Scale(1.0, "width");
    hZmj->Scale(1.0, "width");
    hZfakes->Scale(1.0, "width");
    hZtt->Scale(1.0, "width");
    hDataAntiIsoLooseTauIsoQCD->Scale(1.0, "width");
    hQCD->Scale(1.0, "width");
    hSSLooseVBF->Scale(1.0, "width");
    hSS->Scale(1.0, "width");
    hEWK->Scale(1.0, "width");
    hVV->Scale(1.0, "width");
    hSgn->Scale(1.0, "width");
  }

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

  hSiml->Add(hZmm,1.0);
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
    hSiml->Add(hZmj,1.0);

  hSiml->Add(hTTb,1.0);

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
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
      aStack->Add(hSSLooseVBF);
    else
      aStack->Add(hSS);
  }
  aStack->Add(hTTb);
  aStack->Add(hEWK);
  aStack->Add(hZmm);
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
    aStack->Add(hZmj);

  if(useEmbedding_)
    aStack->Add(hDataEmb);
  else
    aStack->Add(hZtt);

  TH1F* hSgnSUSY ;
  TH1F* hSgnSUSY1 ;
  TH1F* hSgnSUSY2 ;
  if(MSSM) {
    hSgnSUSY = (TH1F*)hSusy[0][5]->Clone("hSgnSUSY");
    hSgnSUSY->Add(hSusy[1][5]);
    hSgnSUSY->Scale(magnifySgn_);
    hSgnSUSY->SetLineColor(kBlue);
    hSgnSUSY1 = (TH1F*)hSusy[0][11]->Clone("hSgnSUSY1");
    hSgnSUSY1->Add(hSusy[1][11]);
    hSgnSUSY1->Scale(magnifySgn_);
    hSgnSUSY1->SetLineColor(kRed);
    hSgnSUSY2 = (TH1F*)hSusy[0][16]->Clone("hSgnSUSY2");
    hSgnSUSY2->Add(hSusy[1][16]);
    hSgnSUSY2->Scale(magnifySgn_);
    hSgnSUSY2->SetLineColor(kGreen+2);
  }
  if(!logy_ && !MSSM)
    aStack->Add(hSgn);

  leg->AddEntry(hData,"Observed","P");
  if(MSSM) {
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
  leg->AddEntry(hTTb,"t#bar{t}","F");
  leg->AddEntry(hEWK,"Electroweak","F");
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
    leg->AddEntry(hZmj,"Zee","F");
  else
    leg->AddEntry(hZmm,"Zee","F");
  leg->AddEntry(hQCD,"QCD","F");
  
  if(MSSM && version_.Contains("TTbarEmb")){
    leg->AddEntry(hTTbEmb,"t#bar{t} embedded","F");
  }

  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");
  

  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle(XTitle_+" ("+Unities_+")");
  if(scaleByBinWidth && variable_.Contains("diTauNSVfitMass")){
    hStack->SetYTitle(Form(" dN/dm_{#tau#tau}(1/%s)", Unities_.Data() ) );
    hData->SetYTitle(Form(" dN/dm_{#tau#tau}(1/%s)", Unities_.Data() ) );
  }
  else 
    hStack->SetYTitle(Form(" Events/(%.0f %s)", hStack->GetBinWidth(1), Unities_.Data() ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.95,"Y");
  if(!logy_)
    hData->SetAxisRange(0.0, TMath::Max( hData->GetMaximum() + hSgn->GetMaximum(), hSiml->GetMaximum() + hSgn->GetMaximum() )*maxY_ ,"Y");
  else
    hData->SetAxisRange(0.1, TMath::Max( hData->GetMaximum() + hSgn->GetMaximum(), hSiml->GetMaximum() + hSgn->GetMaximum() )*maxY_ ,"Y");
//   if(selection_.find("nobTag")!=string::npos && variable_.Contains("diTauNSVfitMass")) hData->SetAxisRange(0.1, 6000 ,"Y");
//   if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos && variable_.Contains("diTauNSVfitMass"))hData->SetAxisRange(0.1, 80 ,"Y");
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
//   for(int k = 1 ; k <= hRatio->GetNbinsX(); k++){
//     float pull = hData->GetBinContent(k) - hSiml->GetBinContent(k);
//     if(selection_.find("HighMt")!=string::npos) pull = hData->GetBinContent(k);
//     if(hSiml->GetBinContent(k)>0)
//       pull /= hSiml->GetBinContent(k);
//     hRatio->SetBinContent(k, pull);
//     if(TMath::Abs(pull) > maxPull)
//       maxPull = TMath::Abs(pull);
//   }
//   //   hRatio->SetAxisRange(TMath::Min(-1.2*maxPull,-1.),TMath::Max(1.2*maxPull,1.),"Y");
//   hRatio->SetAxisRange(-0.5,0.5,"Y");
//   if(selection_.find("HighMt")!=string::npos) hRatio->SetAxisRange(0.5,1.5,"Y");

//   hRatio->Draw("P");

  TH1F * hRatio = (TH1F*)hData->Clone("hRatio");
  hRatio->Sumw2();
  if(selection_.find("HighMt")==string::npos) hRatio->Add(hSiml,-1);
  hRatio->Divide(hSiml);
  hRatio->SetXTitle("");
  hRatio->SetYTitle("#frac{DATA-MC}{MC}");
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
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_log.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
  }
  else{
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
  }

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
    hSiml_Scaled->SetYTitle(Form("a.u.", hSiml->GetBinWidth(1), Unities_.Data() ) );
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
    c1->SaveAs(Form(location+"/%s/plots/plotControl_eTau_mH%d_%s_%s_%s_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plotControl_eTau_mH%d_%s_%s_%s_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
  }
  else{
    c1->SaveAs(Form(location+"/%s/plots/plotControl_eTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    c1->SaveAs(Form(location+"/%s/plots/plotControl_eTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
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

    if(logy_){
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind_log.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind_log.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind_log.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    }
    else{
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
      c1->SaveAs(Form(location+"/%s/plots/plot_eTau_mH%d_%s_%s_%s_blind.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
    }
  }

  // templates for fitting
  TFile* fout = new TFile(Form(location+"/%s/histograms/eTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");  
  fout->cd();

  hSiml->Write();
  hQCD->Write();
  hSS->Write();
  hSSLooseVBF->Write();
  hZmm->Write();
  hZmm_Up->Write();
  hZmm_Down->Write();
  hZmmLoose->Write();
  hZmj->Write();
  hZmjLoose->Write();
  hZfakes->Write();
  hTTb->Write();
  hTTbEmb->Write();
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
  if(MSSM) 
    hSgnSUSY->Write();
  hW3JetsLooseTauIso->Write();
  hW3JetsMediumTauIso->Write();
  hW3JetsMediumTauIsoRelVBF->Write();
  hW3JetsMediumTauIsoRelVBFMinusSS->Write();
  hWLooseBTag->Write();
  hDataAntiIsoLooseTauIso->Write();
  hDataAntiIsoLooseTauIsoQCD->Write();
  hData->Write();
  hParameters->Write();

  //fine binning histograms for MSSM
  hDataEmb_fb->Write(); hW_fb->Write(); hZtt_fb->Write();
  hZmm_fb->Write(); hZmj_fb->Write(); hTTb_fb->Write();
  hQCD_fb->Write(); hVV_fb->Write();
  
  //hW up/Down
  hW_TFUp->Write(); hW_TFDown->Write();
  hWLooseBTag_TFUp->Write(); hWLooseBTag_TFDown->Write();
  hW_TFUp_fb->Write(); hW_TFDown_fb->Write();
  
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

  if(variable_.Contains("Mass")) hDataBlind->Write();

  fout->Write();
  fout->Close();

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

  delete hQCD; delete hSS; delete hSSLooseVBF; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hTTbEmb; delete hZtt; delete hZmm_Up; delete hZmm_Down;
  delete hW; delete hWSS; delete hWMinusSS; delete hW3Jets; delete hAntiIso; delete hAntiIsoFR;
  delete hZmmLoose; delete hZmjLoose; delete hLooseIso1; delete hLooseIso2; delete hLooseIso3;
  delete hVV; delete hSgn; delete hSgn1; delete hSgn2; delete hSgn3; delete hData; delete hParameters;
  delete hW3JetsLooseTauIso; delete hW3JetsMediumTauIso; delete hW3JetsMediumTauIsoRelVBF; delete hW3JetsMediumTauIsoRelVBFMinusSS; 
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoQCD;
  
  delete hDataEmb_fb; delete hZtt_fb; delete hW_fb; delete hZmm_fb; delete hZmj_fb; delete hTTb_fb;
  delete hQCD_fb; delete hVV_fb;
  
  delete hW_TFUp; delete hW_TFDown; delete hWLooseBTag_TFUp; delete hWLooseBTag_TFDown;
  delete hW_TFUp_fb; delete hW_TFDown_fb;
  
  //for(unsigned int i = 0; i < SUSYhistos.size() ; i++) delete mapSUSYhistos.find( SUSYhistos[i] )->second ;
  
  delete aStack;  delete hEWK; delete hSiml; delete hDataEmb;  delete hRatio; delete line;
  delete fout;

  for(int iP=0 ; iP<nProd ; iP++) {
    for(int iM=0 ; iM<nMasses ; iM++) {
      //fSignal[iP][iM]->Close();
      delete signal[iP][iM];
    }
  }
  for(int iP=0 ; iP<nProdWW ; iP++) {
    for(int iM=0 ; iM<nMassesWW ; iM++) {
      delete signalWW[iP][iM];
    }
  }
  for(int iP=0 ; iP<nProdS ; iP++) {
    for(int iM=0 ; iM<nMassesS ; iM++) {
      delete signalSusy[iP][iM];
    }
  }

}


///////////////////////////////////////////////////////////////////////////////////////////////



void plotElecTauAll( Int_t useEmbedded = 1, TString outputDir = ""){
      
  plotElecTau(125,1,"inclusive",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,60,0,360,5.0,1.0,0,1.2);

  return;

  vector<string> variables;
  vector<int> mH;
  
  //variables.push_back("diTauVisMass");
  variables.push_back("diTauNSVfitMass");
  
  mH.push_back(90);
  mH.push_back(95);
  mH.push_back(100);
  mH.push_back(105);
  mH.push_back(110);
  mH.push_back(115);
  mH.push_back(120);
  mH.push_back(125);
  mH.push_back(130);
  mH.push_back(135);
  mH.push_back(140);
  mH.push_back(145);
  mH.push_back(150);
  mH.push_back(155);
  mH.push_back(160);
  
//   plotElecTau(125,1,"inclusive",""   ,"decayMode",     "#tau_{h} decay mode","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
//   plotElecTau(125,1,"inclusive",""   ,"visibleTauMass","visible #tau_{h} mass","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  
//   plotElecTau(125,1,"inclusive",""   ,"MEtMVA","MET","GeV"                        ,outputDir,40,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"inclusive",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,32,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"inclusiveNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,40,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"inclusive",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,50,0,200,5.0,1.0,0,1.2); 
  plotElecTau(125,1,"inclusive",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,60,0,360,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"inclusive",""   ,"etaL1","#e #eta", "units"              ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"inclusive",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,27,11, 92,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"inclusive",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"inclusive",""   ,"etaL2","#tau #eta","units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"inclusive",""   ,"numPV","reconstructed vertexes","units"             ,outputDir,30,0,30,5.0,1.0,0,1.5);
//   plotElecTau(125,1,"inclusive",""   ,"nJets30","jet multiplicity","units"                 ,outputDir,10,0, 10,5.0,1.0,1,10);
//   plotElecTau(125,1,"bTag",""        ,"ptB1", "leading b-tagged jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
//   plotElecTau(125,1,"bTag",""        ,"etaB1","leading b-tagged jet #eta","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  
//   plotElecTau(125,1,"novbfLow",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0,   1.2);
//   plotElecTau(125,1,"novbfLow",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,20,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"novbfLowNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfLow",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,20,0,200,5.0,1.0,0,1.2);  
//   //plotElecTau(125,1,"novbfLow",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfLow",""   ,"etaL1","#e #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"novbfLow",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfLow",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfLow",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
//   plotElecTau(125,1,"novbfHigh",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
//   plotElecTau(125,1,"novbfHigh",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"novbfHighNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfHigh",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2); 
//   //plotElecTau(125,1,"novbfHigh",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfHigh",""   ,"etaL1","#e #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"novbfHigh",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfHigh",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"novbfHigh",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
  
//   plotElecTau(125,1,"boostLow",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
//   plotElecTau(125,1,"boostLow",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"boostLowNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostLow",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,20,0,200,5.0,1.0,0,1.2);  
//   //plotElecTau(125,1,"boostLow",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostLow",""   ,"etaL1","#e #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"boostLow",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostLow",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostLow",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
  
//   plotElecTau(125,1,"boostHigh",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
//   plotElecTau(125,1,"boostHigh",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,10,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"boostHighNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostHigh",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2); 
//   //plotElecTau(125,1,"boostHigh",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostHigh",""   ,"etaL1","#e #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
//   plotElecTau(125,1,"boostHigh",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostHigh",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"boostHigh",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
//   plotElecTau(125,1,"vbf",""   ,"MEtMVA","MET","GeV"                        ,outputDir,10,0,100,5.0,1.0,0, 1.2);
//   plotElecTau(125,1,"vbf",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,16,-3.2,3.2,   5.0,1.0,0,1.5);
//   plotElecTau(125,1,"vbfNoMt",""   ,"MtLeg1MVA","M_{T}(#e#nu)","GeV" ,                  outputDir,16,0,160,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"vbf",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,10,0,200,5.0,1.0,0,1.2);  
//   //plotElecTau(125,1,"vbf",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"vbf",""   ,"etaL1","#e #eta", "units"              ,outputDir,10,-2.5, 2.5,5.0,1.0,0, 2.);
//   plotElecTau(125,1,"vbf",""   ,"ptL1","#e p_{T}", "GeV"                ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"vbf",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,16, 15, 95,5.0,1.0,0,1.2);
//   plotElecTau(125,1,"vbf",""   ,"etaL2","#tau #eta","units"         ,outputDir,10,-2.5, 2.5,5.0,1.0,0,2.);
  
  return;

  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){

      plotElecTau(mH[j],useEmbedded,"novbfLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

      plotElecTau(mH[j],useEmbedded,"novbfHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"novbfHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

      plotElecTau(mH[j],useEmbedded,"boostLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      
      plotElecTau(mH[j],useEmbedded,"boostHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotElecTau(mH[j],useEmbedded,"boostHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

      plotElecTau(mH[j],useEmbedded,"vbf",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
      plotElecTau(mH[j],useEmbedded,"vbf","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
      plotElecTau(mH[j],useEmbedded,"vbf","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
      plotElecTau(mH[j],useEmbedded,"vbf","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 
      plotElecTau(mH[j],useEmbedded,"vbf","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2); 

    }
  }
  
  return;

}

int main(int argc, const char* argv[])
{

  std::cout << "plotElecTau()" << std::endl;
  gROOT->SetBatch(true);

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  int mH, nBins, logy, useEmb; 
  float magnify, hltEff, xMin, xMax, maxY;
  string category, analysis, variable, xtitle, unity, outputDir, RUN, version;

  if(argc==1) plotElecTauAll();
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

    cout << endl << "RUN : " << RUN << " | VERSION : " << version << " | ANALYSIS : " << analysis << endl << endl;

    plotElecTau(mH,useEmb,category,analysis,variable,xtitle,unity,outputDir,nBins,xMin,xMax,magnify,hltEff,logy,maxY, RUN, version);
  }
  else { cout << "Please put at least 16 arguments" << endl; return 1;}

  cout << "DONE" << endl;
  return 0;
}
