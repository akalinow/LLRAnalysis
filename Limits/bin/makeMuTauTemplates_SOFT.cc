#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TSystem.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#define RESCALETO1PB true
#define DOSUSY       false
#define TESTINCL     false
#define DEBUG        false

// TEMPLATES //
// General
#define RESTRICT     false
#define negbin       -1
#define nullbin      -1
// ZTT 
#define sigmaL1etm   1
// QCD
#define ScaleQCD       true
#define modifyQCDError false
#define kFactor        0.001

using namespace std;

void rescale(TH1F* hRescaled, TH1F* hCentral, TH1F* hModelCentr, TH1F* hModelFluct) {
  
  float central, modCentral, modFluct, ratio;
  central = modCentral = modFluct = ratio = 1;

  int nBins=hCentral->GetNbinsX();
  for(int i=0 ; i<=nBins+1 ; i++) {
    central    = hCentral   ->GetBinContent(i);
    modCentral = hModelCentr->GetBinContent(i);
    modFluct   = hModelFluct->GetBinContent(i);

    if(central<0 || modCentral<0 || modFluct<0) ratio=1.0;
    else ratio = modCentral!=0 ? ( 1 + sigmaL1etm*(modFluct-modCentral)/modCentral) : 1.0;

    hRescaled  ->SetBinContent(i, central*ratio);
  }
}

void checkValidity(TH1F* h, ofstream& f, ofstream& g) {

  int nBins = h->GetNbinsX();
  bool issue=false;
  bool issue2=false;

  f << h->GetName() << endl;
  g << h->GetName() << endl;

  for(int i=0 ; i<=nBins+1 ; i++) {
    f << h->GetBinContent(i) << " | " ;
    if(h->GetBinContent(i) < 0) {
      issue=true;
      //h->SetBinContent(i,0);
      //h->SetBinContent(i,1e-6);
      if(negbin!=-1) h->SetBinContent(i,negbin);
    }
    if(nullbin!=-1 && h->GetBinContent(i) == 0) h->SetBinContent(i,nullbin);
    if(RESTRICT && (h->GetBinCenter(i)<20 || h->GetBinCenter(i)>220) ) h->SetBinContent(i,0);

    g << h->GetBinContent(i) << " | " ;
    if(h->GetBinContent(i) < 0) issue2=true;
  }
  if(issue) f << " NEGATIVE" << endl;
  else f << endl;

  if(issue2) g << " NEGATIVE" << endl;
  else g << endl;

  //return 0;
}

float higgsXsection(int mH = 125, string process = "ggH"){

  float xsection = 1.0;

  if(process.find("ggH")!=string::npos){
    if(mH == 90) xsection = 36.80                   * 8.33E-02;
    if(mH == 95) xsection = 33.19                   * 8.32E-02;
    if(mH == 100) xsection = 30.12                   * 8.28E-02;
    if(mH == 105) xsection = 27.39                   * 8.17E-02;
    if(mH == 110) xsection = 7.95e-02*25.04;
    if(mH == 115) xsection = 7.58e-02*22.96;
    if(mH == 120) xsection = 7.04e-02*21.13;
    if(mH == 125) xsection = 6.32e-02*19.52;
    if(mH == 130) xsection = 5.45e-02*18.07;
    if(mH == 135) xsection = 4.49e-02*16.79;
    if(mH == 140) xsection = 3.52e-02*15.63;
    if(mH == 145) xsection = 2.61e-02*14.59;
    if(mH == 150) xsection = 13.65                   * 1.79E-02;
    if(mH == 155) xsection = 12.79                   * 1.05E-02;
    if(mH == 160) xsection = 11.95                   * 3.96E-03;
  }
  if(process.find("qqH")!=string::npos){
    if(mH == 90) xsection = 2.191                   * 8.33E-02;
    if(mH == 95) xsection = 2.084                   * 8.32E-02;
    if(mH == 100) xsection = 1.988                   * 8.28E-02;
    if(mH == 105) xsection = 1.897                   * 8.17E-02;
    if(mH == 110) xsection = 7.95e-02*1.809;
    if(mH == 115) xsection = 7.58e-02*1.729;
    if(mH == 120) xsection = 7.04e-02*1.649;
    if(mH == 125) xsection = 6.32e-02*1.578;
    if(mH == 130) xsection = 5.45e-02*1.511;
    if(mH == 135) xsection = 4.49e-02*1.448;
    if(mH == 140) xsection = 3.52e-02*1.389;
    if(mH == 145) xsection = 2.61e-02*1.333;
    if(mH == 150) xsection = 1.280                   * 1.79E-02;
    if(mH == 155) xsection = 1.231                   * 1.05E-02;
    if(mH == 160) xsection = 1.185                   * 3.96E-03;
  }
  if(process.find("VH")!=string::npos){
    if(mH == 90) xsection = (1.972 + 1.057 + 0.3233)   * 8.33E-02;
    if(mH == 95) xsection = (1.676 + 0.9060 + 0.2812)   * 8.32E-02;
    if(mH == 100) xsection = (1.432 + 0.7807 + 0.2454)   * 8.28E-02;
    if(mH == 105) xsection = (1.229 + 0.6750 + 0.2149)   * 8.17E-02;
    if(mH == 110) xsection = 7.95e-02*(1.060+0.5869+0.1887  );
    if(mH == 115) xsection = 7.58e-02*(0.9165+0.5117+0.1663  );
    if(mH == 120) xsection = 7.04e-02*(0.7966+0.4483+0.1470 );
    if(mH == 125) xsection = 6.32e-02*(0.6966+0.3943+0.1302 );
    if(mH == 130) xsection = 5.45e-02*(0.6095+0.3473+0.1157 );
    if(mH == 135) xsection = 4.49e-02*(0.5351+0.3074+0.1031 );
    if(mH == 140) xsection = 3.52e-02*(0.4713+0.2728+0.09207 );
    if(mH == 145) xsection = 2.61e-02*(0.4164+0.2424+0.08246 );
    if(mH == 150) xsection = (0.3681 + 0.2159 + 0.07403) * 1.79E-02;
    if(mH == 155) xsection = (0.3252 + 0.1923 + 0.06664) * 1.05E-02;
    if(mH == 160) xsection = (0.2817 + 0.1687 + 0.06013) * 3.96E-03;
  }

  return xsection;
}



void produce(   
	     int mH_=125,
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "results/MuTau/res_ABCD_Moriond_v1",
	     int useEmb        = 1,
	     TString location  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_5_3_10_SyncRecoEleIdJEC/src/LLRAnalysis/Limits/bin/"
	     ){


  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ << " analysis " << analysis_ << " and bin " << bin_ << endl;

  string s_outname=Form(location+"/%s/datacards/check_mH%d_%s_%s.txt", outputDir.Data(), mH_, bin_.c_str(),analysis_.c_str());
  ofstream outfile(s_outname,ios::out);

  string s_outname2=Form(location+"/%s/datacards/after_mH%d_%s_%s.txt", outputDir.Data(), mH_, bin_.c_str(),analysis_.c_str());
  ofstream outfile2(s_outname2,ios::out);

  //test(outfile);
  //outfile << "TESTTEST" << endl;

  TString     nameZtt="hDataEmb";
  if(!useEmb) nameZtt="hZtt";

  float rescaleggH = RESCALETO1PB ? higgsXsection(mH_,"ggH") : 1.0;
  float rescaleqqH = RESCALETO1PB ? higgsXsection(mH_,"qqH") : 1.0;
  float rescaleVH  = RESCALETO1PB ? higgsXsection(mH_,"VH")  : 1.0;

  string binNameSpace = "";
  if(bin_.find("inclusive")!=string::npos)      
    binNameSpace =  "inclusive";

  else if(bin_.find("novbf")!=string::npos) 
    binNameSpace =  "SM0";

  if(bin_.find("novbfLow")  !=string::npos) 
    binNameSpace =  "0jet_low";

  if(bin_.find("novbfHigh") !=string::npos) 
    binNameSpace =  "0jet_high";


  else if(bin_.find("boost")!=string::npos) 
    binNameSpace =  "SM1";

  if(bin_.find("boostLow")!=string::npos) 
    binNameSpace =  "boost_low";

  if(bin_.find("boostHigh")!=string::npos) 
    binNameSpace =  "boost_high";


  else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) 
    binNameSpace =  "vbf";

  else if(bin_.find("vh")!=string::npos) 
    binNameSpace =  "2jet";


  else if(bin_.find("bTag")!=string::npos) 
    binNameSpace =  "SM4";

  if(bin_.find("bTagLow")!=string::npos) 
    binNameSpace =  "btag_low";

  if(bin_.find("bTagHigh")!=string::npos) 
    binNameSpace =  "btag_high";


  else if(bin_.find("twoJets")!=string::npos) 
    binNameSpace =  "SMpre2";

  else if(bin_.find("oneJet")!=string::npos) 
    binNameSpace =  "SMpre2a";


  // TESTING PURPOSES : REPLACE EVERYTHING BY INCLUSIVE TEMPLATES //
  string binTest = bin_ ;
  if(TESTINCL) binTest = "inclusive";

  TFile* fin = 0;
  string analysisFile = analysis_;
  if(analysis_.find("FakeUp")!=string::npos)   analysisFile =  "TauUp";
  if(analysis_.find("FakeDown")!=string::npos) analysisFile =  "TauDown";

  //cout << endl <<  "Try to get file : " << Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root", outputDir.Data(), 125, binTest.c_str() , analysisFile.c_str(), variable_.c_str()) << endl << endl;

  fin = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root", outputDir.Data(), 125, binTest.c_str() , analysisFile.c_str(), variable_.c_str()), "READ");

  ///////////////////////////////////////////////
  TFile* fin_jUp     ;//= new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_JetUp_%s.root",    outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  TFile* fin_jDown   ;//= new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_JetDown_%s.root",  outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  TFile* fin_tUp     ;//= new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_TauUp_%s.root",    outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  TFile* fin_tDown   ;//= new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_TauDown_%s.root",  outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  //TFile* fin_etmUp   = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_L1EtmUp_%s.root",  outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  //TFile* fin_etmDown = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_L1EtmDown_%s.root",outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  TFile* fin_nominal = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_nominal_%s.root",  outputDir.Data(), 125, binTest.c_str() , variable_.c_str()), "READ");
  ///////////////////////////////////////////////

  // ZTT NOMINAL HISTOGRAMS TO PRODUCE L1ETM Up/Down DISTRIBUTIONS //
  TH1F* hDataEmbNominal = ((TH1F*)fin_nominal->Get("hDataEmb"));
  TH1F* hZttMCNominal   = ((TH1F*)fin_nominal->Get("hZtt"));
  checkValidity(hDataEmbNominal,outfile,outfile2);
  checkValidity(hZttMCNominal,outfile,outfile2);


  //TFile* fTemplOut = new TFile(Form(location+"/%s/datacards/muTauSM_%s.root",outputDir.Data(), variable_.c_str()),"UPDATE");
  string theory =  !DOSUSY ? "SM" : "MSSM" ;

  TFile* fTemplOut = new TFile(Form(location+"/%s/datacards/muTau%s_ScaleQCDboostlow.root",outputDir.Data(), theory.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "_CMS_scale_t_mutau_8TeVUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "_CMS_scale_t_mutau_8TeVDown";
  if(analysis_.find("L1EtmUp")!=string::npos)
    suffix = "_CMS_L1etm_mutau_8TeVUp";
  else if(analysis_.find("L1EtmDown")!=string::npos)
    suffix = "_CMS_L1etm_mutau_8TeVDown";
  else if(analysis_.find("MuUp")!=string::npos)
    suffix = "_CMS_scale_mUp";
  else if(analysis_.find("MuDown")!=string::npos)
    suffix = "_CMS_scale_mDown";
  else if(analysis_.find("JetUp")!=string::npos)
    suffix = "_CMS_scale_jUp";
  else if(analysis_.find("JetDown")!=string::npos)
    suffix = "_CMS_scale_jDown";
  else if(analysis_.find("FakeUp")!=string::npos)
    suffix = "_CMS_scale_fakeUp";
  else if(analysis_.find("FakeDown")!=string::npos)
    suffix = "_CMS_scale_fakeDown";
  
  cout << "Adding histos with suffix " << suffix << endl;
  TString dirName( Form("muTau_%s",binNameSpace.c_str()) );
  
  if(! (fTemplOut->cd( dirName.Data()  ) ) ){
    
    cout << "Editing the directory for bin and variable " << binNameSpace << ", " << variable_ << endl;

    TDirectory* dir = fTemplOut->mkdir( dirName.Data() );
    dir->cd();

    
    TH1F* hData = ((TH1F*)fin->Get("hData")); 
    hData->SetName("data_obs");
    checkValidity(hData,outfile,outfile2);
    hData->Write("data_obs");
    if(DEBUG) cout << "SUFFIX " << suffix << endl;
    //checkValidity(,outfile);

    if(bin_.find("novbfLow")  !=string::npos) {
      hData->SetBinContent(1,0);
      hData->SetBinContent(2,0);
    }

    if(!DOSUSY){
      if(DEBUG) cout << "Getting : " << Form("hGGFH%d",mH_) << endl;
      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hGGFH%d",mH_));
      hSgn2->Scale(1./rescaleggH);
      hSgn2->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      checkValidity(hSgn2,outfile,outfile2);
      hSgn2->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      if(DEBUG) cout << "Getting : " << Form("hVBFH%d",mH_) << endl;
      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
      hSgn1->Scale(1./rescaleqqH);
      hSgn1->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
      checkValidity(hSgn1,outfile,outfile2);
      hSgn1->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));

      if(DEBUG) cout << "Getting : " << Form("hVH%d",mH_) << endl;
      TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
      hSgn3->Scale(1./rescaleVH);
      hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
      
      // in VBF, take the di-tau mass shape from GGF sample
      if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	float VHyield = hSgn3->Integral();
	hSgn3->Reset();
	hSgn3->Add(hSgn2,1.0);
	hSgn3->Scale(VHyield/hSgn3->Integral());
	checkValidity(hSgn3,outfile,outfile2);
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      }
      else {
	checkValidity(hSgn3,outfile,outfile2);
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      }
    }
    else{
      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
      checkValidity(hSgn1,outfile,outfile2);
      hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
      checkValidity(hSgn2,outfile,outfile2);
      hSgn2->SetName(Form("bbH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("bbH%d%s" ,mH_,suffix.c_str()));
    }

    if(DEBUG) cout << "HERE 1" << endl;

    if(bin_.find("novbf")!=string::npos){

      // ZTT: embedded sample
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hDataEmb,outfile,outfile2);
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));

      // ----- QCD ------
      if( bin_.find("High")!=string::npos ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	checkValidity(hQCD,outfile,outfile2);
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	checkValidity(hQCD,outfile,outfile2);
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
  
      // ----- W ------
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      checkValidity(hW,outfile,outfile2);
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      hW->Write(Form("W%s"           ,suffix.c_str()));

      // ----- Fakes ------
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      checkValidity(hZmj,outfile,outfile2);
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));

      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      checkValidity(hZmm,outfile,outfile2);
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));

      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      checkValidity(hZfakes,outfile,outfile2);
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      checkValidity(hTTb,outfile,outfile2);
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));

      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      checkValidity(hVV,outfile,outfile2);
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("boost")!=string::npos){

      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hDataEmb,outfile,outfile2);
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));

      // ----- QCD ------ 
      if( bin_.find("High")!=string::npos ) {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	checkValidity(hQCD,outfile,outfile2);
        hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
        hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	//for 1Jet_low, need to scale by 15% below 60 GeV and add a shape systematic for QCD 
	if(suffix == ""){

	  if(ScaleQCD) {
	    for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	      if(hQCD->GetBinCenter(b)<=60.){ 
		hQCD->SetBinContent(b,1.15*hQCD->GetBinContent(b)); 
	      }
	    }
	  }
	  TH1F* hQCDUp=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVUp");
	  TH1F* hQCDDown=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVDown");
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	    if(hQCD->GetBinCenter(b)<=60.){
	      hQCDUp->SetBinContent(b,1.15*hQCDUp->GetBinContent(b));
	      hQCDDown->SetBinContent(b,0.85*hQCDDown->GetBinContent(b));
	    }
	  }
	  checkValidity(hQCDUp,outfile,outfile2);
	  checkValidity(hQCDDown,outfile,outfile2);
	  hQCDUp->Write("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVUp");
	  hQCDDown->Write("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVDown");
	}
	checkValidity(hQCD,outfile,outfile2);
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
       
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      checkValidity(hW,outfile,outfile2);
      hW->Write(Form("W%s"           ,suffix.c_str()));

      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      checkValidity(hZmj,outfile,outfile2);
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));

      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      checkValidity(hZmm,outfile,outfile2);
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));

      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      checkValidity(hZfakes,outfile,outfile2);
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      checkValidity(hTTb,outfile,outfile2);
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));

      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      checkValidity(hVV,outfile,outfile2);
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }

    else if(bin_.find("bTag")!=string::npos){

      TH1F* hZtt=(TH1F*)fin->Get(nameZtt);
      if(analysis_.find("L1Etm")!=string::npos) rescale(hZtt, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hZtt,outfile,outfile2);
      hZtt->Write(Form("ZTT%s",suffix.c_str()));
      /*
      // QCD:  low-pt: SS data; high-pt: loose-iso data normalized to SS 
      TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
      TH1F* hLooseIso    = (TH1F*)fin->Get("hLooseIso") ;

      if( bin_.find("Low")!=string::npos ){
	//hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else{
	hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      */
      //2012 (already scaled ) 
      TH1F* hQCD=(TH1F*)fin->Get("hDataAntiIsoLooseTauIso");
      checkValidity(hQCD,outfile,outfile2);
      hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      /*
      if( bin_.find("Low")!=string::npos ){
	TH1F* hW     = (TH1F*)fin->Get("hW");
	TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	//((TH1F*)fin->Get("hW"))->Write(Form("W%s",suffix.c_str()));
      }
      else{	
	TH1F* hW     = (TH1F*)fin->Get("hW");    
	TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	//hW->Write(Form("W%s"    ,suffix.c_str()));
      }
      */

      // W+jets: shape from Wjets exact one jet; norm from sdb 
      TH1F* hWJetsLooseBTag  = (TH1F*)fin->Get("hWJetsLooseBTag"); 
      checkValidity(hWJetsLooseBTag,outfile,outfile2);
      hWJetsLooseBTag->Write(Form("W%s"           ,suffix.c_str())); 
      
      // ------ old ------
      //if( bin_.find("Low")!=string::npos )
      //hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      //else
      //hAntiIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
      //((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));
      //((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));

      //TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
      //TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");

      checkValidity(hZmmKeys,outfile,outfile2);
      hZmmKeys->Write(Form("ZL%s"        ,suffix.c_str()));

      checkValidity(hZmjKeys,outfile,outfile2);
      hZmjKeys->Write(Form("ZJ%s"        ,suffix.c_str()));

      TH1F* hZfakesKeys = (TH1F*)fin->Get("hZfakes");
      hZfakesKeys->Reset();
      hZfakesKeys->Add(hZmjKeys,1.0);
      hZfakesKeys->Add(hZmmKeys,1.0);
      checkValidity(hZfakesKeys,outfile,outfile2);
      hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));

      //((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hTTb     = (TH1F*)fin->Get("hTTb"); 
      TH1F* hTTbLoose= (TH1F*)fin->Get("hTTbLoose"); 
      checkValidity(hTTb,outfile,outfile2);
      checkValidity(hTTbLoose,outfile,outfile2);
      hTTbLoose->Scale(hTTb->Integral()/hTTbLoose->Integral()); 
      hTTbLoose->Write(Form("TT%s"        ,suffix.c_str())); 
      
      //((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
      TH1F* hVV     = (TH1F*)fin->Get("hVV");  
      TH1F* hVVLoose     = (TH1F*)fin->Get("hVVLoose");  
      checkValidity(hVVLoose,outfile,outfile2);
      hVVLoose->Scale(hVV->Integral()/hVVLoose->Integral());  
      hVVLoose->Write(Form("VV%s"        ,suffix.c_str()));  

    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hDataEmb,outfile,outfile2);
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
 
      // ----- QCD ------ 
      TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
      checkValidity(hQCD,outfile,outfile2);
      hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
      hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
      checkValidity(hW,outfile,outfile2);
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      hW->Write(Form("W%s"           ,suffix.c_str())); 
 
      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      checkValidity(hZmj,outfile,outfile2);
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));

      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      checkValidity(hZmm,outfile,outfile2);
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));

      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      checkValidity(hZfakes,outfile,outfile2);
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      checkValidity(hTTb,outfile,outfile2);
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));

      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      checkValidity(hVV,outfile,outfile2);
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("vh")!=string::npos){

      TH1F* hZtt = (TH1F*)fin->Get(nameZtt);
      if(analysis_.find("L1Etm")!=string::npos) rescale(hZtt, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hZtt,outfile,outfile2);
      hZtt->Write(Form("ZTT%s",suffix.c_str()));

      //((TH1F*)fin->Get("hAntiIsoKeys"))->Write(Form("QCD%s"    ,suffix.c_str()));
      TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
      TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
      TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
      checkValidity(hAntiIsoKeys,outfile,outfile2);
      checkValidity(hAntiIso,outfile,outfile2);
      checkValidity(hLooseIsoKeys,outfile,outfile2);

      hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
      hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));

      //((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));

      TH1F* hWKeys=(TH1F*)fin->Get("hWKeys");
      checkValidity(hWKeys,outfile,outfile2);
      hWKeys->Write(Form("W%s"           ,suffix.c_str()));

      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
      TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
      TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
      TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakesKeys");

      checkValidity(hZmjKeys,outfile,outfile2);
      checkValidity(hZmmKeys,outfile,outfile2);
      checkValidity(hZmj,outfile,outfile2);
      checkValidity(hZmm,outfile,outfile2);
      checkValidity(hZfakesKeys,outfile,outfile2);

      hZfakesKeys->Reset();
      if(hZmjKeys->Integral()>0) hZfakesKeys->Add(hZmjKeys,1.0);
      else hZfakesKeys->Add(hZmj,1.0);
      if(hZmmKeys->Integral()>0) hZfakesKeys->Add(hZmmKeys,1.0);  
      else hZfakesKeys->Add(hZmm,1.0);

      checkValidity(hZfakesKeys,outfile,outfile2);
      hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));

      TH1F* hTTb = (TH1F*)fin->Get("hTTb");
      checkValidity(hTTb,outfile,outfile2);
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));

      TH1F* hVVKeys = (TH1F*)fin->Get("hVVKeys");
      checkValidity(hVVKeys,outfile,outfile2);
      hVVKeys->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else{

      // ZTT: embedded sample 
      if(DEBUG) cout << "Getting : " << nameZtt << endl;
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt)); 
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));  
      if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
      checkValidity(hDataEmb,outfile,outfile2);
      hDataEmb->Write(Form("ZTT%s",suffix.c_str())); 
 
      // ----- QCD ------ 
      if(DEBUG) cout << "Getting : hQCD" << endl;
      TH1F *hQCD = ((TH1F*)fin->Get("hQCD")); 
      checkValidity(hQCD,outfile,outfile2);
      hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
      hQCD->Write(Form("QCD%s"    ,suffix.c_str())); 

      // ----- W ------ 
      if(DEBUG) cout << "Getting : hW" << endl;
      TH1F* hW = ((TH1F*)fin->Get("hW")); 
      checkValidity(hW,outfile,outfile2);
      hW->SetName(Form("W%s"           ,suffix.c_str())); 
      hW->Write(Form("W%s"           ,suffix.c_str())); 

      // ----- Fakes ------ 
      if(DEBUG) cout << "Getting : hZmj" << endl;
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj")); 
      checkValidity(hZmj,outfile,outfile2);
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str())); 
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str())); 

      if(DEBUG) cout << "Getting : hZmm" << endl;
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm")); 
      checkValidity(hZmm,outfile,outfile2);
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str())); 
      hZmm->Write(Form("ZL%s"        ,suffix.c_str())); 

      if(DEBUG) cout << "Getting : hZfakes" << endl;
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes")); 
      checkValidity(hZfakes,outfile,outfile2);
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));  
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str())); 
       
      if(DEBUG) cout << "Getting : hTTb" << endl;
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb")); 
      checkValidity(hTTb,outfile,outfile2);
      hTTb->SetName(Form("TT%s"        ,suffix.c_str())); 
      hTTb->Write(Form("TT%s"        ,suffix.c_str())); 

      if(DEBUG) cout << "Getting : hVV" << endl;
      TH1F* hVV = ((TH1F*)fin->Get("hVV")); 
      checkValidity(hVV,outfile,outfile2);
      hVV->SetName(Form("VV%s"         ,suffix.c_str())); 
      hVV->Write(Form("VV%s"         ,suffix.c_str())); 
    }

    if(DEBUG) cout << "HERE 3" << endl;
           
  }else{
    cout << "Directory is there, filling new histos..." << endl;

    TDirectory* dir = (TDirectory*)fTemplOut->Get( dirName.Data() );
    if(DEBUG) cout << "got templout directory" << endl;

    fTemplOut->cd( dirName.Data() );
    if(DEBUG) cout << "cd dirName : " << dirName << endl
		   << "suffix : " << suffix << endl
		   << "dir : " << dir->GetName() << endl;    

    if(!DOSUSY){
      if(DEBUG) cout << "FindObjectAny : " << Form("qqH%d%s"         ,mH_,suffix.c_str()) << endl;
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
	if(DEBUG) cout << "Getting : " << Form("hVBFH%d",mH_) << endl;
	TH1F* hSgn2 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
	hSgn2->Scale(1./rescaleqqH);
	hSgn2->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
	checkValidity(hSgn2,outfile,outfile2);
	hSgn2->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));
      }
      if(DEBUG) cout << "FindObjectAny : " << Form("ggH%d%s"         ,mH_,suffix.c_str()) << endl;
      if(dir->FindObjectAny(Form("ggH%d%s"          , mH_,suffix.c_str()))==0 ){
	if(DEBUG) cout << "Getting : " << Form("hGGFH%d",mH_) << endl;
	TH1F* hSgn1 = (TH1F*)fin->Get(Form("hGGFH%d",mH_));
	hSgn1->Scale(1./rescaleggH);
	hSgn1->SetName(Form("ggH%d%s" , mH_,suffix.c_str())); 
	checkValidity(hSgn1,outfile,outfile2);
	hSgn1->Write(Form("ggH%d%s" , mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VH%d%s"          , mH_,suffix.c_str()))==0 ){

	TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
	if(DEBUG) cout << "Getting : " << Form("hVH%d",mH_) << endl;
	hSgn3->Scale(1./rescaleVH);
	hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
	if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	  TH1F* hSgn2 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
	  float VHyield = hSgn3->Integral();
	  hSgn3->Reset();
	  hSgn3->Add(hSgn2,1.0);
	  hSgn3->Scale(VHyield/hSgn3->Integral());
	  checkValidity(hSgn3,outfile,outfile2);
	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
	}
	else {
	  checkValidity(hSgn3,outfile,outfile2);
	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
	}
      }
    }
    else{
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
        TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
	hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
	checkValidity(hSgn1,outfile,outfile2);
        hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
        TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
	hSgn2->SetName(Form("bbH%d%s" , mH_,suffix.c_str()));
	checkValidity(hSgn2,outfile,outfile2);
        hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
      }
    }



    if(bin_.find("novbf")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hDataEmb,outfile,outfile2);
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
	//cout << "DATA EEEEEEEEEEEEEMB" << endl;
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("High")!=string::npos ){
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	  checkValidity(hQCD,outfile,outfile2);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  checkValidity(hQCD,outfile,outfile2);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	checkValidity(hW,outfile,outfile2);
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	checkValidity(hZmj,outfile,outfile2);
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	checkValidity(hZmm,outfile,outfile2);
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	checkValidity(hZfakes,outfile,outfile2);
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	checkValidity(hTTb,outfile,outfile2);
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	checkValidity(hVV,outfile,outfile2);
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
    else if(bin_.find("boost")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hDataEmb,outfile,outfile2);
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("High")!=string::npos ) {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	  checkValidity(hQCD,outfile,outfile2);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	  //for 1Jet_low, need to scale by 15% below 60 GeV and add a shape systematic for QCD  
	  if(suffix == ""){ 
	    if(ScaleQCD) {
	      for(Int_t b=1;b<=hQCD->GetNbinsX();b++){  
		if(hQCD->GetBinCenter(b)<=60.){  
		  hQCD->SetBinContent(b,1.15*hQCD->GetBinContent(b));  
		} 
	      } 
	    }
	    TH1F* hQCDUp=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVUp"); 
	    TH1F* hQCDDown=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVDown"); 
	    for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	      if(hQCD->GetBinCenter(b)<=60.){ 
		hQCDUp->SetBinContent(b,1.15*hQCDUp->GetBinContent(b)); 
		hQCDDown->SetBinContent(b,0.85*hQCDDown->GetBinContent(b)); 
	      } 
	    } 
	    checkValidity(hQCDUp,outfile,outfile2);
	    checkValidity(hQCDDown,outfile,outfile2);

	    hQCDUp->Write("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVUp"); 
	    hQCDDown->Write("QCD_CMS_htt_QCDShape_mutau_boost_low_8TeVDown");
	  }
	  checkValidity(hQCD,outfile,outfile2);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	checkValidity(hW,outfile,outfile2);
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	checkValidity(hZmj,outfile,outfile2);
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	checkValidity(hZmm,outfile,outfile2);
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	checkValidity(hZfakes,outfile,outfile2);
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	checkValidity(hTTb,outfile,outfile2);
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	checkValidity(hVV,outfile,outfile2);
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
    else if(bin_.find("bTag")!=string::npos){
      
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZtt=(TH1F*)fin->Get(nameZtt);
	if(analysis_.find("L1Etm")!=string::npos) rescale(hZtt, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hZtt,outfile,outfile2);
	hZtt->Write(Form("ZTT%s"       ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	/*
	TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
	TH1F* hLooseIso    = (TH1F*)fin->Get("hLooseIso") ;
	
	if( bin_.find("Low")!=string::npos ){
	  //hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	  hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	  hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else{
	  hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	  hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	*/
	//2012 (already scaled ) 
	TH1F* hQCD=(TH1F*)fin->Get("hDataAntiIsoLooseTauIso");
	checkValidity(hQCD,outfile,outfile2);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));

	// ------  old -------
	//TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
	//TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
	//if(  bin_.find("Low")!=string::npos )
	//hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	//else 
	//hAntiIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	/*
	if( bin_.find("Low")!=string::npos ){
	  TH1F* hW     = (TH1F*)fin->Get("hW"); 
	  TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	  hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	  hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	  //((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
	}
	else{
	  TH1F* hW     = (TH1F*)fin->Get("hW"); 
	  TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	  hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	  hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	  //hW->Write(Form("W%s"    ,suffix.c_str()));
	}
	*/
	// W+jets: shape from Wjets exact one jet; norm from sdb 
	TH1F* hWJetsLooseBTag  = (TH1F*)fin->Get("hWJetsLooseBTag"); 
	checkValidity(hWJetsLooseBTag,outfile,outfile2);
	hWJetsLooseBTag->Write(Form("W%s"           ,suffix.c_str())); 

      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmj=(TH1F*)fin->Get("hZmj");
	checkValidity(hZmj,outfile,outfile2);
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = (TH1F*)fin->Get("hZmm");
	checkValidity(hZmm,outfile,outfile2);
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	//TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
	//TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakes");

	checkValidity(hZmjKeys,outfile,outfile2);
	checkValidity(hZmmKeys,outfile,outfile2);
	checkValidity(hZfakesKeys,outfile,outfile2);

	hZfakesKeys->Reset();
	hZfakesKeys->Add(hZmjKeys,1.0);
	hZfakesKeys->Add(hZmmKeys,1.0);  
	checkValidity(hZfakesKeys,outfile,outfile2);
	hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	//((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
	TH1F* hTTb     = (TH1F*)fin->Get("hTTb"); 
	TH1F* hTTbLoose     = (TH1F*)fin->Get("hTTbLoose"); 
	hTTbLoose->Scale(hTTb->Integral()/hTTbLoose->Integral()); 
	checkValidity(hTTbLoose,outfile,outfile2);
	hTTbLoose->Write(Form("TT%s"        ,suffix.c_str())); 
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	//((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
	TH1F* hVV     = (TH1F*)fin->Get("hVV");   
	TH1F* hVVLoose     = (TH1F*)fin->Get("hVVLoose");   
	hVVLoose->Scale(hVV->Integral()/hVVLoose->Integral());  
	checkValidity(hVVLoose,outfile,outfile2); 
	hVVLoose->Write(Form("VV%s"        ,suffix.c_str()));   
      }

    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hDataEmb,outfile,outfile2);
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	checkValidity(hQCD,outfile,outfile2);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	checkValidity(hW,outfile,outfile2);
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	checkValidity(hZmj,outfile,outfile2);
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	checkValidity(hZmm,outfile,outfile2);
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	checkValidity(hZfakes,outfile,outfile2);
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	checkValidity(hTTb,outfile,outfile2);
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	checkValidity(hVV,outfile,outfile2);
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }

    }
    else if(bin_.find("vh")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZtt = (TH1F*)fin->Get(nameZtt);
	if(analysis_.find("L1Etm")!=string::npos) rescale(hZtt, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hZtt,outfile,outfile2);
	hZtt->Write(Form("ZTT%s"       ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){

	TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
	TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
	TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");

	checkValidity(hAntiIsoKeys,outfile,outfile2);
	checkValidity(hAntiIso,outfile,outfile2);
	checkValidity(hLooseIsoKeys,outfile,outfile2);

	hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
	checkValidity(hLooseIsoKeys,outfile,outfile2);
	hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
	//hLooseIsoKeys->Write(Form("QCD%s"       ,suffix.c_str()));

      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ) {
	//((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));
	TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	checkValidity(hWKeys,outfile,outfile2);
	hWKeys->Write(Form("W%s"           ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmj = (TH1F*)fin->Get("hZmj");
	checkValidity(hZmj,outfile,outfile2);
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = (TH1F*)fin->Get("hZmm");
	checkValidity(hZmm,outfile,outfile2);
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
	TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
	TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakesKeys");

	checkValidity(hZmjKeys,outfile,outfile2);
	checkValidity(hZmmKeys,outfile,outfile2);
	checkValidity(hZmj,outfile,outfile2);
	checkValidity(hZmm,outfile,outfile2);
	checkValidity(hZfakesKeys,outfile,outfile2);

	hZfakesKeys->Reset();
	if(hZmjKeys->Integral()>0) hZfakesKeys->Add(hZmjKeys,1.0);
	else hZfakesKeys->Add(hZmj,1.0);
	if(hZmmKeys->Integral()>0) hZfakesKeys->Add(hZmmKeys,1.0);  
	else hZfakesKeys->Add(hZmm,1.0);

	checkValidity(hZfakesKeys,outfile,outfile2);
	hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      }  
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ) {
	TH1F* hTTb = (TH1F*)fin->Get("hTTb");
	checkValidity(hTTb,outfile,outfile2);
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ) {
	TH1F* hVVKeys = (TH1F*)fin->Get("hVVKeys");
	checkValidity(hVVKeys,outfile,outfile2);
	hVVKeys->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
    else{

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt)); 
        hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));  
	if(analysis_.find("L1Etm")!=string::npos) rescale(hDataEmb, hDataEmbNominal, hZttMCNominal, (TH1F*)fin->Get("hZtt"));
	checkValidity(hDataEmb,outfile,outfile2);
        hDataEmb->Write(Form("ZTT%s",suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){ 
	TH1F *hQCD = ((TH1F*)fin->Get("hQCD")); 
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	checkValidity(hQCD,outfile,outfile2);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hW = ((TH1F*)fin->Get("hW")); 
        hW->SetName(Form("W%s"           ,suffix.c_str())); 
	checkValidity(hW,outfile,outfile2);
        hW->Write(Form("W%s"           ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hZmj = ((TH1F*)fin->Get("hZmj")); 
	checkValidity(hZmj,outfile,outfile2);
        hZmj->SetName(Form("ZJ%s"        ,suffix.c_str())); 
        hZmj->Write(Form("ZJ%s"        ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) { 
        TH1F* hZmm = ((TH1F*)fin->Get("hZmm")); 
	checkValidity(hZmm,outfile,outfile2);
        hZmm->SetName(Form("ZL%s"        ,suffix.c_str())); 
        hZmm->Write(Form("ZL%s"        ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) { 
        TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes")); 
	checkValidity(hZfakes,outfile,outfile2);
        hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));  
        hZfakes->Write(Form("ZLL%s"    ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hTTb = ((TH1F*)fin->Get("hTTb")); 
	checkValidity(hTTb,outfile,outfile2);
        hTTb->SetName(Form("TT%s"        ,suffix.c_str())); 
        hTTb->Write(Form("TT%s"        ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hVV = ((TH1F*)fin->Get("hVV")); 
	checkValidity(hVV,outfile,outfile2);
        hVV->SetName(Form("VV%s"         ,suffix.c_str())); 
        hVV->Write(Form("VV%s"         ,suffix.c_str())); 
      } 

      
    }


  }

  fTemplOut->Close();
  delete fTemplOut;

  fin->Close();          delete fin;
  //fin_tUp->Close();      delete fin_tUp;
  //fin_tDown->Close();    delete fin_tDown;
  //fin_jUp->Close();      delete fin_jUp;
  //fin_jDown->Close();    delete fin_jDown;

  outfile.close();
  outfile2.close();

  return;

}



void produceAll(TString outputDir="results/MuTau/res_ABCD_Moriond_v1", int useEmb=1){

  const int nVar=1;
  const int nM=15;
  const int nCat=6;
  const int nAn=7;

  string variables[nVar]={"diTauNSVfitMass"};
  int mH[nM]={90,95,100,105,110,115,120,125,130,135,140,145,150,155,160};

  string analysis[nAn]={"nominal","TauUp","TauDown","JetUp","JetDown","L1EtmUp","L1EtmDown"};
  string category[nCat]={"inclusive","novbfLow","novbfHigh","boostLow","boostHigh","vbf"};

  for(int iVar = 0 ; iVar < nVar; iVar++)
    for(int iM = 0; iM < nM; iM++)
      for(int iAn=0 ; iAn<nAn ; iAn++)
	//for(int iAn=0 ; iAn<1 ; iAn++)
	for(int iCat=0 ; iCat<nCat ; iCat++)
	  
	  produce(mH[iM],variables[iVar], analysis[iAn], category[iCat], outputDir, useEmb);

  /*
  void produce(   
	     int mH_=125,
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "MuTau/res_ABCD_Moriond_v1",
	     TString location  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534p2_Spring13_Trees/src/LLRAnalysis/Limits/bin/results/"
  */

}




int main(int argc, const char* argv[])
{

  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  TString outputDir="results/MuTau/res_ABCD_Moriond_v1";
  int useEmb=1;

  if(argc>=2) outputDir = argv[1];
  if(argc>=3) useEmb    = (int)atof(argv[2]);
  
  produceAll(outputDir, useEmb);

  /*
  const int nVer=4;
  TString outputDir[nVer];
  TString version[nVer]={"Moriond","TauIso2","AntiMu2","PostMoriond"};

  for(int i=0;i<nVer;i++) produceAll("MuTau/res_ABCD_"+version[i]+"_v1");
  */
}
