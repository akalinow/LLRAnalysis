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
#include <map>
#include <string>


#define RESCALETO1PB true
#define DOSUSY false
#define OldCat false

using namespace std;

//void checkValidity(TH1F* h, ofstream& f, ofstream& g) {
void checkValidity(TH1F* h){
  int nBins = h->GetNbinsX();
  
  for(int i=1 ; i<=nBins+1 ; i++) {
    //f << h->GetBinContent(i) << " | " ;
    if(h->GetBinContent(i) < 0) {
      //h->SetBinContent(i,0);
      h->SetBinContent(i,1e-9);
    }
    if(h->GetBinContent(i) == 0) h->SetBinContent(i,1e-9);
  }

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
	     TString outputDir = "MuTau/res_ABCD_Moriond_v1",
	     int useEmb        = 1,
	     TString location  = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_10_git/src/LLRAnalysis/Limits/bin/results/"
	     ){


  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ << " analysis " << analysis_ << " and bin " << bin_ << endl;
  TFile* fin = 0;
  string analysisFile = analysis_;
  if(analysis_.find("FakeUp")!=string::npos)   analysisFile = "TauUp";
  if(analysis_.find("FakeDown")!=string::npos) analysisFile = "TauDown";

  //cout << endl <<  "Try to get file : " << Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root", outputDir.Data(), 125, bin_.c_str() , analysisFile.c_str(), variable_.c_str()) << endl << endl;

  fin = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root", outputDir.Data(), 125, bin_.c_str() , analysisFile.c_str(), variable_.c_str()), "READ");

  TString     nameZtt="hDataEmb";
  if(!useEmb) nameZtt="hZtt";

  ///////////////////////////////////////////////
  TFile* fin_jUp     = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_JetUp_%s.root",   outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_jDown   = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_JetDown_%s.root", outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tUp     = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_TauUp_%s.root",   outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tDown   = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_TauDown_%s.root", outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_nominal = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_nominal_%s.root",        outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  ///////////////////////////////////////////////

  float rescaleggH = RESCALETO1PB ? higgsXsection(mH_,"ggH") : 1.0;
  float rescaleqqH = RESCALETO1PB ? higgsXsection(mH_,"qqH") : 1.0;
  float rescaleVH  = RESCALETO1PB ? higgsXsection(mH_,"VH")  : 1.0;


  string binNameSpace = "";
  if(bin_.find("inclusive")!=string::npos)      
    binNameSpace =  "inclusive";

  if(bin_.find("novbfLow")  !=string::npos) 
    binNameSpace =  "0jet_low";

  if(bin_.find("novbfMedium")  !=string::npos)
    binNameSpace =  "0jet_medium";

  if(bin_.find("novbfHigh") !=string::npos) 
    binNameSpace =  "0jet_high";

  if(OldCat){
    if(bin_.find("boostLow")!=string::npos)
      binNameSpace =  "boost_low";

    if(bin_.find("boostHigh")!=string::npos)
      binNameSpace =  "boost_high";
  }
  else{
    if(bin_.find("boostLow")!=string::npos) 
      binNameSpace =  "1jet_low";
    
    if(bin_.find("boostMedium")!=string::npos)
      binNameSpace =  "1jet_medium";
    
    if(bin_.find("boostHighhighhiggs")!=string::npos) 
      binNameSpace =  "1jet_high_mediumhiggs";
    
    if(bin_.find("boostHighlowhiggs")!=string::npos)
      binNameSpace =  "1jet_high_lowhiggs";
  }
  if(OldCat){
    if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos)
      binNameSpace =  "vbf";
  }
  else{
    if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos && bin_.find("vbfTight")==string::npos) 
      binNameSpace =  "vbf_loose";
    
    if(bin_.find("vbfTight")!=string::npos)
      binNameSpace =  "vbf_tight";
  }
  
  if(bin_.find("vh")!=string::npos) 
    binNameSpace =  "2jet";

  if(bin_.find("bTag")!=string::npos && bin_.find("nobTag")==string::npos) 
    binNameSpace =  "btag";

  else if(bin_.find("nobTag")!=string::npos)
    binNameSpace =  "nobtag";
  
  else if(bin_.find("twoJets")!=string::npos) 
    binNameSpace =  "SMpre2";

  else if(bin_.find("oneJet")!=string::npos) 
    binNameSpace =  "SMpre2a";

  //TFile* fTemplOut = new TFile(Form(location+"/%s/datacards/muTauSM_%s.root",outputDir.Data(), variable_.c_str()),"UPDATE");
  string theory =  !DOSUSY ? "SM" : "MSSM" ;

  TFile* fTemplOut = new TFile(Form(location+"/%s/datacards/muTau%s.root",outputDir.Data(), theory.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "_CMS_scale_t_mutau_8TeVUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "_CMS_scale_t_mutau_8TeVDown";
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
    hData->Write("data_obs");

    if(!DOSUSY){
      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hGGFH%d",mH_));
      hSgn2->Scale(1./rescaleggH);
      hSgn2->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
      hSgn1->Scale(1./rescaleqqH);
      hSgn1->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
      hSgn3->Scale(1./rescaleVH);
      hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
      
      // in VBF, take the di-tau mass shape from GGF sample
      /*if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	float VHyield = hSgn3->Integral();
	hSgn3->Reset();
	hSgn3->Add(hSgn2,1.0);
	hSgn3->Scale(VHyield/hSgn3->Integral());
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      }
      else*/
      hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      
      if(analysis_.find("nominal")!=string::npos){
	TH1F* hSgn2_HqTUp = (TH1F*)fin->Get(Form("hGGFH%dUp",mH_)); 
	hSgn2_HqTUp->Scale(1./rescaleggH); 
	hSgn2_HqTUp->SetName(Form("ggH%d_QCDscale_ggH1inUp" ,mH_)); 
	hSgn2_HqTUp->Write(Form("ggH%d_QCDscale_ggH1inUp" ,mH_));

	TH1F* hSgn2_HqTDown = (TH1F*)fin->Get(Form("hGGFH%dDown",mH_));  
        hSgn2_HqTDown->Scale(1./rescaleggH);  
        hSgn2_HqTDown->SetName(Form("ggH%d_QCDscale_ggH1inDown" ,mH_));  
        hSgn2_HqTDown->Write(Form("ggH%d_QCDscale_ggH1inDown" ,mH_));
      }
      //Add also HWW signal
      if(mH_ >= 110){
	TH1F* hSgn4 = (TH1F*)fin->Get(Form("hGGFHWW%d",mH_));
	//hSgn4->Scale(1./rescaleggH); //already norm to 1.
	hSgn4->SetName(Form("ggH_hww%d%s" ,mH_,suffix.c_str()));
	hSgn4->Write(Form("ggH_hww%d%s" ,mH_,suffix.c_str()));
	
	TH1F* hSgn5 = (TH1F*)fin->Get(Form("hVBFHWW%d",mH_));
	//hSgn5->Scale(1./rescaleqqH); //already norm to 1.
	hSgn5->SetName(Form("qqH_hww%d%s" ,mH_,suffix.c_str()));
	hSgn5->Write(Form("qqH_hww%d%s" ,mH_,suffix.c_str()));
      }
    }
    else{
      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hGGH%d",mH_));
      hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hBBH%d",mH_));
      hSgn2->SetName(Form("bbH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("bbH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSMSgn2 = (TH1F*)fin->Get(Form("hGGFH%d",125));
      hSMSgn2->SetName(Form("ggH_SM%d%s" ,125,suffix.c_str()));
      hSMSgn2->Write(Form("ggH_SM%d%s" ,125,suffix.c_str()));

      TH1F* hSMSgn1 = (TH1F*)fin->Get(Form("hVBFH%d",125));
      hSMSgn1->SetName(Form("qqH_SM%d%s" ,125,suffix.c_str()));
      hSMSgn1->Write(Form("qqH_SM%d%s" ,125,suffix.c_str()));
      
      TH1F* hSMSgn3 = (TH1F*)fin->Get(Form("hVH%d",125));
      hSMSgn3->SetName(Form("VH_SM%d%s" ,125,suffix.c_str()));
      hSMSgn3->Write(Form("VH_SM%d%s" ,125,suffix.c_str()));
    }

    if(bin_.find("novbf")!=string::npos){

      // ZTT: embedded sample
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));

      // ----- QCD ------
      if( bin_.find("High")!=string::npos ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	checkValidity(hQCD);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	checkValidity(hQCD);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      // ----- W ------
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      hW->Write(Form("W%s"           ,suffix.c_str()));

      // ----- Fakes ------
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      if(suffix == ""){
	TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
        hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
      }

      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("boost")!=string::npos){

      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
 
      // ----- QCD ------ 
      if( bin_.find("Low")!=string::npos || bin_.find("Medium")!=string::npos) {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
        hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	//for 1Jet_low/medium, need to scale by 10% below 60 GeV and add a 10% shape systematic for QCD 
	for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	  if(hQCD->GetBinCenter(b)<=50.){ 
	    hQCD->SetBinContent(b,1.1*hQCD->GetBinContent(b)); 
	  }
	}
	if(suffix == ""){
	  TString QCDShape("QCD_CMS_htt_QCDShape_mutau_1jet_low");
	  if(bin_.find("Medium")!=string::npos)
	    QCDShape = TString("QCD_CMS_htt_QCDShape_mutau_1jet_medium");
	  
	  TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp");
	  TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown");
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	    if(hQCD->GetBinCenter(b)<=50.){
	      hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b));
	      hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b));
	    }
	  }
	  checkValidity(hQCDUp);
	  checkValidity(hQCDDown);
	  hQCDUp->Write(QCDShape+"_8TeVUp");
	  hQCDDown->Write(QCDShape+"_8TeVDown");
	}
	checkValidity(hQCD);
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
        TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
        hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	/*
          for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	  if(hQCD->GetBinCenter(b)<=50.){
	  hQCD->SetBinContent(b,1.1*hQCD->GetBinContent(b));
	  }
          }
	*/
	if(suffix == ""){
          TString QCDShape("QCD_CMS_htt_QCDShape_mutau");
	  QCDShape = QCDShape+"_"+binNameSpace;
	  
          TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp");
          TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown");
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
            if(hQCD->GetBinCenter(b)<=50.){
              hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b));
              hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b));
            }
          }
          hQCDUp->Write(QCDShape+"_8TeVUp");
          hQCDDown->Write(QCDShape+"_8TeVDown");
	}
	checkValidity(hQCD);
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      hW->Write(Form("W%s"           ,suffix.c_str()));
      
      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      if(suffix == ""){
        TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
        hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
      }
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }

    else if(bin_.find("bTag")!=string::npos){

      // ZTT: embedded sample
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));

      TString nameZtt_fb = nameZtt+"_fb";
      TH1F* hDataEmb_fb = ((TH1F*)fin->Get(nameZtt_fb)); 
      hDataEmb_fb->SetName(Form("ZTT_fine_binning%s",suffix.c_str())); 
      hDataEmb_fb->Write(Form("ZTT_fine_binning%s",suffix.c_str()));

      // ----- QCD ------
      float maxBin = 50.;
      if(bin_.find("bTag")!=string::npos && bin_.find("nobTag")==string::npos)
        maxBin = 40.;
      TH1F *hQCD;
      if(bin_.find("nobTag")!=string::npos){
	hQCD = ((TH1F*)fin->Get("hQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	  if(hQCD->GetBinCenter(b)<=maxBin){
	    hQCD->SetBinContent(b,1.1*hQCD->GetBinContent(b));
	  }
	}
      }
      else{
	hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
      }
      checkValidity(hQCD);
      hQCD->Write(Form("QCD%s"    ,suffix.c_str()));

      //Add a 10% shape uncert. to QCD for mass < 60 GeV
      if(suffix == ""){
	TString QCDShape("QCD_CMS_htt_QCDShape_mutau");
	QCDShape = QCDShape+"_"+binNameSpace;
	
	TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp");
	TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown");
	for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	  if(hQCD->GetBinCenter(b)<=maxBin){
	    hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b));
	    hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b));
	  }
	}
	checkValidity(hQCDUp);
	hQCDUp->Write(QCDShape+"_8TeVUp");
	checkValidity(hQCDDown);
	hQCDDown->Write(QCDShape+"_8TeVDown");
      }
	
      TH1F *hQCD_fb = ((TH1F*)fin->Get("hQCD_fb")); 
      hQCD_fb->SetName(Form("QCD_fine_binning%s"    ,suffix.c_str())); 
      if(bin_.find("nobTag")!=string::npos){
	for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	  if(hQCD_fb->GetBinCenter(b)<=maxBin){
	    hQCD_fb->SetBinContent(b,1.1*hQCD_fb->GetBinContent(b));
	  }
	}
      }
      checkValidity(hQCD_fb);
      hQCD_fb->Write(Form("QCD_fine_binning%s"    ,suffix.c_str()));
      //Add a 10% shape uncert. to QCD for mass < 60 GeV
      if(suffix == ""){
	TString QCDShapeFb("QCD_fine_binning_CMS_htt_QCDShape_mutau");
	QCDShapeFb = QCDShapeFb+"_"+binNameSpace;
	
	TH1F* hQCD_fbUp=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVUp");
	TH1F* hQCD_fbDown=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVDown");
	for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	  if(hQCD_fb->GetBinCenter(b)<=maxBin){
	    hQCD_fbUp->SetBinContent(b,1.1*hQCD_fbUp->GetBinContent(b));
	    hQCD_fbDown->SetBinContent(b,0.9*hQCD_fbDown->GetBinContent(b));
	  }
	}
	checkValidity(hQCD_fbUp);
	hQCD_fbUp->Write(QCDShapeFb+"_8TeVUp");
	checkValidity(hQCD_fbDown);
	hQCD_fbDown->Write(QCDShapeFb+"_8TeVDown");
      }
      
      // ----- W ------
      if(bin_.find("nobTag")!=string::npos){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      else{
	TH1F* hW = ((TH1F*)fin->Get("hWLooseBTag"));
        hW->SetName(Form("W%s"           ,suffix.c_str()));
        hW->Write(Form("W%s"           ,suffix.c_str()));
      }

      TH1F* hW_fb = ((TH1F*)fin->Get("hW_fb")); 
      hW_fb->SetName(Form("W_fine_binning%s"           ,suffix.c_str())); 
      hW_fb->Write(Form("W_fine_binning%s"           ,suffix.c_str()));

      // ----- Fakes ------
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      if(suffix == ""){
        TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
        hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
      }
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));

      TH1F* hZmj_fb = ((TH1F*)fin->Get("hZmj_fb")); 
      hZmj_fb->SetName(Form("ZJ_fine_binning%s"        ,suffix.c_str())); 
      hZmj_fb->Write(Form("ZJ_fine_binning%s"        ,suffix.c_str())); 
      TH1F* hZmm_fb = ((TH1F*)fin->Get("hZmm_fb")); 
      hZmm_fb->SetName(Form("ZL_fine_binning%s"        ,suffix.c_str())); 
      hZmm_fb->Write(Form("ZL_fine_binning%s"        ,suffix.c_str())); 
      TH1F* hZfakes_fb = ((TH1F*)fin->Get("hZmj_fb")); 
      hZfakes_fb->Add(hZmm_fb);
      hZfakes_fb->SetName(Form("ZLL_fine_binning%s"    ,suffix.c_str())); 
      hZfakes_fb->Write(Form("ZLL_fine_binning%s"    ,suffix.c_str())); 


      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));

      TH1F* hTTb_fb = ((TH1F*)fin->Get("hTTb_fb")); 
      hTTb_fb->SetName(Form("TT_fine_binning%s"        ,suffix.c_str())); 
      hTTb_fb->Write(Form("TT_fine_binning%s"        ,suffix.c_str())); 
      TH1F* hVV_fb = ((TH1F*)fin->Get("hVV")); 
      hVV_fb->SetName(Form("VV_fine_binning%s"         ,suffix.c_str())); 
      hVV_fb->Write(Form("VV_fine_binning%s"         ,suffix.c_str()));
    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
 
      // ----- QCD ------ 
      TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
      hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 

      //Add a 10% shape uncert. to QCD for mass < 60 GeV
      if(suffix == ""){
	TString QCDShape("QCD_CMS_htt_QCDShape_mutau");
	QCDShape = QCDShape+"_"+binNameSpace;

	TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp");
	TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown");
	for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	  if(hQCD->GetBinCenter(b)<=40.){
	    hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b));
	    hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b));
	  }
	}
	checkValidity(hQCDUp);
	checkValidity(hQCDDown);
	hQCDUp->Write(QCDShape+"_8TeVUp");
	hQCDDown->Write(QCDShape+"_8TeVDown");
      }

      checkValidity(hQCD);
      hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
      hW->SetName(Form("W%s"           ,suffix.c_str()));
      hW->Write(Form("W%s"           ,suffix.c_str())); 
 
      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      if(suffix == ""){
        TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
        hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
      }
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("vh")!=string::npos){

      ((TH1F*)fin->Get(nameZtt))->Write(Form("ZTT%s",suffix.c_str()));

      //((TH1F*)fin->Get("hAntiIsoKeys"))->Write(Form("QCD%s"    ,suffix.c_str()));
      TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
      TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
      TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
      hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
      hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
      //((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));

      ((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));

      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
      TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
      TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
      TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakesKeys");
      hZfakesKeys->Reset();
      if(hZmjKeys->Integral()>0) hZfakesKeys->Add(hZmjKeys,1.0);
      else hZfakesKeys->Add(hZmj,1.0);
      if(hZmmKeys->Integral()>0) hZfakesKeys->Add(hZmmKeys,1.0);  
      else hZfakesKeys->Add(hZmm,1.0);

      hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hVVKeys"))->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else{

      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt)); 
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));  
      hDataEmb->Write(Form("ZTT%s",suffix.c_str())); 
 
      // ----- QCD ------ 
      TH1F *hQCD = ((TH1F*)fin->Get("hQCD")); 
      hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
      hQCD->Write(Form("QCD%s"    ,suffix.c_str())); 
      
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW")); 
      hW->SetName(Form("W%s"           ,suffix.c_str())); 
      hW->Write(Form("W%s"           ,suffix.c_str())); 

      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj")); 
      hZmj->SetName(Form("ZJ%s"        ,suffix.c_str())); 
      hZmj->Write(Form("ZJ%s"        ,suffix.c_str())); 
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm")); 
      hZmm->SetName(Form("ZL%s"        ,suffix.c_str())); 
      hZmm->Write(Form("ZL%s"        ,suffix.c_str())); 
      if(suffix == ""){
        TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
        hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
      }
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes")); 
      hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));  
      hZfakes->Write(Form("ZLL%s"    ,suffix.c_str())); 
       
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb")); 
      hTTb->SetName(Form("TT%s"        ,suffix.c_str())); 
      hTTb->Write(Form("TT%s"        ,suffix.c_str())); 
      TH1F* hVV = ((TH1F*)fin->Get("hVV")); 
      hVV->SetName(Form("VV%s"         ,suffix.c_str())); 
      hVV->Write(Form("VV%s"         ,suffix.c_str())); 
    }


           
  }else{
    cout << "Directory is there, filling new histos..." << endl;

    TDirectory* dir = (TDirectory*)fTemplOut->Get( dirName.Data() );

    fTemplOut->cd( dirName.Data() );
    
    if(!DOSUSY){
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
	TH1F* hSgn2 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
	hSgn2->Scale(1./rescaleqqH);
	hSgn2->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
	hSgn2->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ggH%d%s"          , mH_,suffix.c_str()))==0 ){
	TH1F* hSgn1 = (TH1F*)fin->Get(Form("hGGFH%d",mH_));
	hSgn1->Scale(1./rescaleggH);
	hSgn1->SetName(Form("ggH%d%s" , mH_,suffix.c_str())); 
	hSgn1->Write(Form("ggH%d%s" , mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VH%d%s"          , mH_,suffix.c_str()))==0 ){

	TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
	hSgn3->Scale(1./rescaleVH);
	hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
	/*if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	  TH1F* hSgn2 = (TH1F*)fin->Get(Form("hVBFH%d",mH_));
	  float VHyield = hSgn3->Integral();
	  hSgn3->Reset();
	  hSgn3->Add(hSgn2,1.0);
	  hSgn3->Scale(VHyield/hSgn3->Integral());
	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
	}
	else*/
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
	
	if(analysis_.find("nominal")!=string::npos){
	  TH1F* hSgn2_HqTUp = (TH1F*)fin->Get(Form("hGGFH%dUp",mH_));
	  hSgn2_HqTUp->Scale(1./rescaleggH);
	  hSgn2_HqTUp->SetName(Form("ggH%d_QCDscale_ggH1inUp" ,mH_));
	  hSgn2_HqTUp->Write(Form("ggH%d_QCDscale_ggH1inUp" ,mH_));
	  
	  TH1F* hSgn2_HqTDown = (TH1F*)fin->Get(Form("hGGFH%dDown",mH_));
	  hSgn2_HqTDown->Scale(1./rescaleggH);
	  hSgn2_HqTDown->SetName(Form("ggH%d_QCDscale_ggH1inDown" ,mH_));
	  hSgn2_HqTDown->Write(Form("ggH%d_QCDscale_ggH1inDown" ,mH_));
	}
	//Add also HWW signal
	if(mH_ >= 110){
	  if(dir->FindObjectAny(Form("ggH_hww%d%s"          , mH_,suffix.c_str()))==0 ){
	    TH1F* hSgn4 = (TH1F*)fin->Get(Form("hGGFHWW%d",mH_));
	    //hSgn4->Scale(1./rescaleggH); //already norm to 1.
	    hSgn4->SetName(Form("ggH_hww%d%s" ,mH_,suffix.c_str()));
	    hSgn4->Write(Form("ggH_hww%d%s" ,mH_,suffix.c_str()));
	  }
	  if(dir->FindObjectAny(Form("qqH_hww%d%s"         ,mH_,suffix.c_str()))==0 ){
	    TH1F* hSgn5 = (TH1F*)fin->Get(Form("hVBFHWW%d",mH_));
	    //hSgn5->Scale(1./rescaleqqH); //already norm to 1.
	    hSgn5->SetName(Form("qqH_hww%d%s" ,mH_,suffix.c_str()));
	    hSgn5->Write(Form("qqH_hww%d%s" ,mH_,suffix.c_str()));
	  }
	}
      }
    }
    else{
      if(dir->FindObjectAny(Form("ggH%d%s"         ,mH_,suffix.c_str()))==0 ){
        TH1F* hSgn1 = (TH1F*)fin->Get(Form("hGGH%d",mH_));
	hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
        hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
        TH1F* hSgn2 = (TH1F*)fin->Get(Form("hBBH%d",mH_));
	hSgn2->SetName(Form("bbH%d%s" , mH_,suffix.c_str()));
        hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ggH_SM%d%s"         ,mH_,suffix.c_str()))==0 ){
	TH1F* hSMSgn2 = (TH1F*)fin->Get(Form("hGGFH%d",125));
	hSMSgn2->SetName(Form("ggH_SM%d%s" ,125,suffix.c_str()));
	hSMSgn2->Write(Form("ggH_SM%d%s" ,125,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("qqH_SM%d%s"         ,mH_,suffix.c_str()))==0 ){
	TH1F* hSMSgn1 = (TH1F*)fin->Get(Form("hVBFH%d",125));
	hSMSgn1->SetName(Form("qqH_SM%d%s" ,125,suffix.c_str()));
	hSMSgn1->Write(Form("qqH_SM%d%s" ,125,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VH_SM%d%s"         ,mH_,suffix.c_str()))==0 ){
	TH1F* hSMSgn3 = (TH1F*)fin->Get(Form("hVH%d",125));
	hSMSgn3->SetName(Form("VH_SM%d%s" ,125,suffix.c_str()));
	hSMSgn3->Write(Form("VH_SM%d%s" ,125,suffix.c_str()));
      }
    }



    if(bin_.find("novbf")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("High")!=string::npos ){
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	  checkValidity(hQCD);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  checkValidity(hQCD);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
	if(suffix == ""){
	  TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	  hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
	  hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	  hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	}
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
    else if(bin_.find("boost")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("Low")!=string::npos || bin_.find("Medium")!=string::npos) {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	  //for 1Jet_low, need to scale by 10% below 60 GeV and add a shape systematic for QCD  
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){  
	    if(hQCD->GetBinCenter(b)<=50.){  
	      hQCD->SetBinContent(b,1.1*hQCD->GetBinContent(b));  
	    } 
	  } 
	  if(suffix == ""){
	    TString QCDShape("QCD_CMS_htt_QCDShape_mutau_1jet_low");
	    if(bin_.find("Medium")!=string::npos)
	      QCDShape = TString("QCD_CMS_htt_QCDShape_mutau_1jet_medium");

	    TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp"); 
	    TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown"); 
	    for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	      if(hQCD->GetBinCenter(b)<=50.){ 
		hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b)); 
		hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b)); 
	      } 
	    } 
	    checkValidity(hQCDUp);
	    checkValidity(hQCDDown);
	    hQCDUp->Write(QCDShape+"_8TeVUp"); 
	    hQCDDown->Write(QCDShape+"_8TeVDown");
	  }
	  checkValidity(hQCD);
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
          TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
          hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));

	  //Add a 10% shape uncert. to QCD for mass < 60 GeV 
	  if(suffix == ""){ 
	    TString QCDShape("QCD_CMS_htt_QCDShape_mutau"); 
	    QCDShape = QCDShape+"_"+binNameSpace; 
	    
	    TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp"); 
	    TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown"); 
	    for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	      if(hQCD->GetBinCenter(b)<=50.){ 
		hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b)); 
		hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b)); 
	      } 
	    } 
	    checkValidity(hQCDUp);
	    checkValidity(hQCDDown);
	    hQCDUp->Write(QCDShape+"_8TeVUp"); 
	    hQCDDown->Write(QCDShape+"_8TeVDown"); 
	  } 
	  
	  checkValidity(hQCD);
          hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
        }
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
	if(suffix == ""){
	  TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	  hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
	  hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	  hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	}
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
    else if(bin_.find("bTag")!=string::npos){
      
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));

	TString nameZtt_fb = nameZtt+"_fb"; 
	TH1F* hDataEmb_fb = ((TH1F*)fin->Get(nameZtt_fb));  
	hDataEmb_fb->SetName(Form("ZTT_fine_binning%s",suffix.c_str()));  
	hDataEmb_fb->Write(Form("ZTT_fine_binning%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	float maxBin = 50.;
        if(bin_.find("bTag")!=string::npos && bin_.find("nobTag")==string::npos)
          maxBin = 40.;
	TH1F *hQCD;
	if(bin_.find("nobTag")!=string::npos){
	  hQCD = ((TH1F*)fin->Get("hQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	    if(hQCD->GetBinCenter(b)<=maxBin){
	      hQCD->SetBinContent(b,1.1*hQCD->GetBinContent(b));
	    }
	  }
	}
	else{
	  hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	}
	checkValidity(hQCD);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	  
	//Add a 10% shape uncert. to QCD for mass < 60 GeV
	if(suffix == ""){
	  TString QCDShape("QCD_CMS_htt_QCDShape_mutau");
	  QCDShape = QCDShape+"_"+binNameSpace;
	  
	  TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp");
	  TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown");
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){
	    if(hQCD->GetBinCenter(b)<=maxBin){
	      hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b));
	      hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b));
	    }
	  }
	  checkValidity(hQCDUp);
	  hQCDUp->Write(QCDShape+"_8TeVUp");
	  checkValidity(hQCDDown);
	  hQCDDown->Write(QCDShape+"_8TeVDown");
	}

	//}

	TH1F *hQCD_fb = ((TH1F*)fin->Get("hQCD_fb"));
	hQCD_fb->SetName(Form("QCD_fine_binning%s"    ,suffix.c_str()));
	if(bin_.find("nobTag")!=string::npos){
	  for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	    if(hQCD_fb->GetBinCenter(b)<=maxBin){
	      hQCD_fb->SetBinContent(b,1.1*hQCD_fb->GetBinContent(b));
	    }
	  }
	}
	checkValidity(hQCD_fb);
	hQCD_fb->Write(Form("QCD_fine_binning%s"    ,suffix.c_str()));
	//Add a 10% shape uncert. to QCD for mass < 60 GeV
	if(suffix == ""){
	  TString QCDShapeFb("QCD_fine_binning_CMS_htt_QCDShape_mutau");
	  QCDShapeFb = QCDShapeFb+"_"+binNameSpace;
	  
	  TH1F* hQCD_fbUp=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVUp");
	  TH1F* hQCD_fbDown=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVDown");
	  for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	    if(hQCD_fb->GetBinCenter(b)<=maxBin){
	      hQCD_fbUp->SetBinContent(b,1.1*hQCD_fbUp->GetBinContent(b));
	      hQCD_fbDown->SetBinContent(b,0.9*hQCD_fbDown->GetBinContent(b));
	    }
	  }
	  checkValidity(hQCD_fbUp);
	  hQCD_fbUp->Write(QCDShapeFb+"_8TeVUp");
	  checkValidity(hQCD_fbDown);
	  hQCD_fbDown->Write(QCDShapeFb+"_8TeVDown");
	}

      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	if(bin_.find("nobTag")!=string::npos){
	  TH1F* hW = ((TH1F*)fin->Get("hW"));
	  hW->SetName(Form("W%s"           ,suffix.c_str()));
	  hW->Write(Form("W%s"           ,suffix.c_str()));
	}
	else{
	  TH1F* hW = ((TH1F*)fin->Get("hWLooseBTag"));
	  hW->SetName(Form("W%s"           ,suffix.c_str()));
	  hW->Write(Form("W%s"           ,suffix.c_str()));
	}

	TH1F* hW_fb = ((TH1F*)fin->Get("hW_fb"));  
	hW_fb->SetName(Form("W_fine_binning%s"           ,suffix.c_str()));  
	hW_fb->Write(Form("W_fine_binning%s"           ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
        TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
        hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
        hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));

	TH1F* hZmj_fb = ((TH1F*)fin->Get("hZmj_fb"));   
	hZmj_fb->SetName(Form("ZJ_fine_binning%s"        ,suffix.c_str()));   
	hZmj_fb->Write(Form("ZJ_fine_binning%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
        TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
        hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
        hZmm->Write(Form("ZL%s"        ,suffix.c_str()));

	if(suffix == ""){
	  TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	  hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
	  hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	  hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	}
	
	TH1F* hZmm_fb = ((TH1F*)fin->Get("hZmm_fb"));   
	hZmm_fb->SetName(Form("ZL_fine_binning%s"        ,suffix.c_str()));   
	hZmm_fb->Write(Form("ZL_fine_binning%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
        TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
        hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));

	TH1F* hZfakes_fb = ((TH1F*)fin->Get("hZmj_fb"));
	TH1F* hZmm_fb = ((TH1F*)fin->Get("hZmm_fb"));
	hZfakes_fb->Add(hZmm_fb);  
	hZfakes_fb->SetName(Form("ZLL_fine_binning%s"    ,suffix.c_str()));   
	hZfakes_fb->Write(Form("ZLL_fine_binning%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
        TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
        hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
        hTTb->Write(Form("TT%s"        ,suffix.c_str()));

	TH1F* hTTb_fb = ((TH1F*)fin->Get("hTTb_fb"));   
	hTTb_fb->SetName(Form("TT_fine_binning%s"        ,suffix.c_str()));   
	hTTb_fb->Write(Form("TT_fine_binning%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
        TH1F* hVV = ((TH1F*)fin->Get("hVV"));
        hVV->SetName(Form("VV%s"         ,suffix.c_str()));
        hVV->Write(Form("VV%s"         ,suffix.c_str()));

	TH1F* hVV_fb = ((TH1F*)fin->Get("hVV"));   
	hVV_fb->SetName(Form("VV_fine_binning%s"         ,suffix.c_str()));   
	hVV_fb->Write(Form("VV_fine_binning%s"         ,suffix.c_str()));
      }
      
    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 

	//Add a 10% shape uncert. to QCD for mass < 60 GeV 
	if(suffix == ""){ 
	  TString QCDShape("QCD_CMS_htt_QCDShape_mutau"); 
	  QCDShape = QCDShape+"_"+binNameSpace; 
	  
	  TH1F* hQCDUp=(TH1F*)hQCD->Clone(QCDShape+"_8TeVUp"); 
	  TH1F* hQCDDown=(TH1F*)hQCD->Clone(QCDShape+"_8TeVDown"); 
	  for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
	    if(hQCD->GetBinCenter(b)<=50.){ 
	      hQCDUp->SetBinContent(b,1.1*hQCDUp->GetBinContent(b)); 
	      hQCDDown->SetBinContent(b,0.9*hQCDDown->GetBinContent(b)); 
	    } 
	  } 
	  checkValidity(hQCDUp);
	  checkValidity(hQCDDown);
	  hQCDUp->Write(QCDShape+"_8TeVUp"); 
	  hQCDDown->Write(QCDShape+"_8TeVDown"); 
	} 
	
	checkValidity(hQCD);
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));

	if(suffix == ""){
	  TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	  hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
	  hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	  hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	}
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }

    }
    else if(bin_.find("vh")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get(nameZtt))->Write(Form("ZTT%s"       ,suffix.c_str()));

      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){

	TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
	TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
	TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
	hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
	hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
    
	hLooseIsoKeys->Write(Form("QCD%s"       ,suffix.c_str()));

      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 )
	//((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));
	((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
	TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
	TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakesKeys");
	hZfakesKeys->Reset();
	if(hZmjKeys->Integral()>0) hZfakesKeys->Add(hZmjKeys,1.0);
	else hZfakesKeys->Add(hZmj,1.0);
	if(hZmmKeys->Integral()>0) hZfakesKeys->Add(hZmmKeys,1.0);  
	else hZfakesKeys->Add(hZmm,1.0);
	hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      }  
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVVKeys"))->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else{

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hDataEmb = ((TH1F*)fin->Get(nameZtt)); 
        hDataEmb->SetName(Form("ZTT%s",suffix.c_str()));  
        hDataEmb->Write(Form("ZTT%s",suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){ 
	TH1F *hQCD = ((TH1F*)fin->Get("hQCD")); 
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	hQCD->Write(Form("QCD%s"    ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hW = ((TH1F*)fin->Get("hW")); 
        hW->SetName(Form("W%s"           ,suffix.c_str())); 
        hW->Write(Form("W%s"           ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hZmj = ((TH1F*)fin->Get("hZmj")); 
        hZmj->SetName(Form("ZJ%s"        ,suffix.c_str())); 
        hZmj->Write(Form("ZJ%s"        ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) { 
        TH1F* hZmm = ((TH1F*)fin->Get("hZmm")); 
        hZmm->SetName(Form("ZL%s"        ,suffix.c_str())); 
        hZmm->Write(Form("ZL%s"        ,suffix.c_str())); 

	if(suffix == ""){
	  TH1F* hZmm_Up = ((TH1F*)fin->Get("hZmm_Up"));
	  hZmm_Up->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  hZmm_Up->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
	  TH1F* hZmm_Down = ((TH1F*)fin->Get("hZmm_Down"));
	  hZmm_Down->SetName(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	  hZmm_Down->Write(Form("ZL%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	}
      } 
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) { 
        TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes")); 
        hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));  
        hZfakes->Write(Form("ZLL%s"    ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hTTb = ((TH1F*)fin->Get("hTTb")); 
        hTTb->SetName(Form("TT%s"        ,suffix.c_str())); 
        hTTb->Write(Form("TT%s"        ,suffix.c_str())); 
      } 
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){ 
        TH1F* hVV = ((TH1F*)fin->Get("hVV")); 
        hVV->SetName(Form("VV%s"         ,suffix.c_str())); 
        hVV->Write(Form("VV%s"         ,suffix.c_str())); 
      } 

      
    }


  }


  
  fTemplOut->Close();
  delete fTemplOut;

  // edit the datacards only for the nominal analysis
  if(analysis_.find("Up")!=string::npos || analysis_.find("Down")!=string::npos){

    fin->Close();          delete fin;
    fin_tUp->Close();      delete fin_tUp;
    fin_tDown->Close();    delete fin_tDown;
    fin_jUp->Close();      delete fin_jUp;
    fin_jDown->Close();    delete fin_jDown;

    return;
  }
  

  ////not needed any more, creates unnecessary txt files
  //ifstream in;
  //
  //char* c = new char[1000];
  //in.open(Form(location+"/templates/%s/muTau_%s_template_v3.txt", theory.c_str(),    binNameSpace.c_str()));
  ////ofstream out(Form(location+"/%s/datacards/muTau_%s_mH%d_%s.txt", outputDir.Data(), binNameSpace.c_str(), mH_, variable_.c_str()));
  //string resonance = !DOSUSY ? "H" : "A";
  //
  //ofstream out(Form(location+"/%s/datacards/muTau_%s_m%s%d.txt", outputDir.Data(), binNameSpace.c_str(), resonance.c_str(), mH_ ));
  //out.precision(8);
  //
  //float QCDyield = 0;
  //float Wyield   = 0.;
  //
  //while (in.good())
  //  {
  //    in.getline(c,1000,'\n');
  //    if (in.good()){
  //
  //	string line(c);
  //	if(line.find("observation")!=string::npos){
  //	  line.replace( line.find("XXX") , 3 , string(Form("%.0f", ((TH1F*)fin->Get("hData"))->Integral()) )  );
  //	  out << line << endl;
  //	}
  //	else if(line.find("shapes")!=string::npos){
  //        //line.replace( line.find("XXX") , 3 , string(Form("muTauSM_%s",variable_.c_str()))  );
  //	  if(!DOSUSY) 
  //	    line.replace( line.find("XXX") , 3 , string("muTauSM")  );
  //	  else 
  //	    line.replace( line.find("XXX") , 3 , string("muTauMSSM")  );
  //	  out << line << endl;
  //	}
  //	else if(line.find("process")!=string::npos && line.find("ggH")!=string::npos){
  //
  //	  if(!DOSUSY){
  //	    if(!RESCALETO1PB){
  //	      line.replace( line.find("XXX") , 3 , string(Form("%d",mH_))  );
  //	      line.replace( line.find("YYY") , 3 , string(Form("%d",mH_))  );
  //	      line.replace( line.find("KKK") , 3 , string(Form("%d",mH_))  );
  //	    }
  //	    else{
  //	      line.replace( line.find("XXX") , 3 , string(Form("   "))  );
  //	      line.replace( line.find("YYY") , 3 , string(Form("   "))  );
  //	      line.replace( line.find("KKK") , 3 , string(Form("   "))  );
  //	    }
  //	  }
  //	  else{
  //	    line.replace( line.find("YYY") , 3 , string(Form("   "))  );
  //	    line.replace( line.find("KKK") , 3 , string(Form("   "))  );
  //	  }
  //        out << line << endl;
  //      }
  //	else if(line.find("rate")!=string::npos){
  //	  
  //	  if((bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) || bin_.find("vh")!=string::npos ){
  //	    QCDyield = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"))->Integral();
  //	  }
  //	  else if( bin_.find("bTag")!=string::npos  ){
  //	    /*
  //	    if(   bin_.find("Low")!=string::npos )
  //	      QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 	   
  //	    else
  //	      QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 	   
  //	    */
  //	    QCDyield = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIso"))->Integral();
  //	  }
  //	  else{
  //	    QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 
  //	  }
  //
  //	  if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
  //	    Wyield = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"))->Integral();
  //	  }
  //	  else if( bin_.find("bTag")!=string::npos  ){
  //	    Wyield = ((TH1F*)fin->Get("hWJetsLooseBTag"))->Integral();
  //	  }
  //	  else{
  //	    Wyield = ((TH1F*)fin->Get("hW"))->Integral();
  //	  }
  //
  //	  /////////////////////////////////////////////
  //	  /////////////////////////////////////////////
  //	  
  //	  TH1F* hSgn2 = !DOSUSY ? (TH1F*)fin->Get(Form("hGGFH%d",mH_)) : (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
  //	  //hSgn2->Scale(1./rescaleggH);
  //	  
  //	  TH1F* hSgn1 = !DOSUSY ? (TH1F*)fin->Get(Form("hVBFH%d",mH_)) : (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
  //	  //hSgn1->Scale(1./rescaleqqH);
  //	  
  //	  TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
  //	  //hSgn3->Scale(1./rescaleVH);
  //
  //
  //	  string rate = "rate                                           ";
  //	  string space = "              ";
  //	  out << rate ;
  //	  if(!DOSUSY) 
  //	    out <<        hSgn3->Integral();
  //	  out << space << hSgn2->Integral()
  //	      << space << hSgn1->Integral()
  //	      << space << ((TH1F*)fin->Get(nameZtt))->Integral()
  //	      << space << QCDyield
  //	      << space << Wyield;
  //	  if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("boost")!=string::npos){
  //	    out << space << ((TH1F*)fin->Get("hZmj"))->Integral()
  //		<< space << ((TH1F*)fin->Get("hZmm"))->Integral();
  //	  }
  //	  else{
  //	    /*
  //	    float scaleFactFakes = 0.37 ; // from Z->ee
  //	    TH1F* hZmj        = (TH1F*)fin->Get("hZmjLoose");
  //	    TH1F* hZmm        = (TH1F*)fin->Get("hZmmLoose");
  //	    float ZFakesyield = hZmj->Integral();
  //	    ZFakesyield += hZmm->Integral();
  //	    out << space << ZFakesyield*scaleFactFakes;
  //	    */
  //	    out << space << ((TH1F*)fin->Get("hZfakes"))->Integral();
  //	  }
  //	  out << space << ((TH1F*)fin->Get("hTTb"))->Integral()
  //	      << space << ((TH1F*)fin->Get("hVV"))->Integral()
  //	      << endl;
  //	}
  //	else if(line.find("CMS_htt_ztt_extrap")!=string::npos){
  //
  //	  float extrapFactor = ((TH1F*)fin->Get("hParameters"))->GetBinContent(8);
  //	  float extrapError  = ((TH1F*)fin->Get("hParameters"))->GetBinContent(9);
  //
  //	  float relUnc = 1.+extrapError/extrapFactor;
  //
  //	  line.replace( line.find("XXX") , 3 , string(Form("%.5f",relUnc))  );
  //	  line.replace( line.find("YYY") , 3 , string(Form("%.5f",relUnc))  );
  //	  if( line.find("KKK") != string::npos )
  //	    line.replace( line.find("KKK") , 3 , string(Form("%.5f",relUnc))  );
  //	  out << line << endl;
  //
  //	}
  //	else if(line.find("CMS_scale_j")!=string::npos){
  //
  //	  float VBFrel = 0; float VBFrelUp = 0;  float VBFrelDown = 0;
  //	  float SMrel  = 0; float SMrelUp  = 0;  float SMrelDown  = 0;
  //	  float VHrel  = 0; float VHrelUp  = 0;  float VHrelDown  = 0;
  //
  //	  if(!DOSUSY){
  //
  //	    //////////////////////////////// FIX HERE!!!
  //
  //	    VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hVBFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVBFH%d",mH_)))->Integral())),
  //				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hVBFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVBFH%d",mH_)))->Integral()))
  //				);
  //	    if( ((TH1F*)fin->Get(Form("hVBFH%d",mH_)))->Integral()>0 ){
  //	      VBFrelUp   = (((TH1F*)fin_jUp->Get(Form("hVBFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVBFH%d",mH_)))->Integral());
  //	      VBFrelDown = (((TH1F*)fin_jDown->Get(Form("hVBFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVBFH%d",mH_)))->Integral());
  //	    }
  //	    else{
  //	      VBFrelUp   = 1.1; // conservative
  //	      VBFrelDown = 1.1; // conservative
  //	    }
  //	    
  //	    VBFrel     *= 1./rescaleqqH;
  //	    VBFrelUp   *= 1./rescaleqqH;
  //	    VBFrelDown *= 1./rescaleqqH;
  //
  //	    SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hGGFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hGGFH%d",mH_)))->Integral())),
  //				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hGGFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hGGFH%d",mH_)))->Integral()))
  //				);
  //	    if( ((TH1F*)fin->Get(Form("hGGFH%d",mH_)))->Integral()>0 ){
  //	      SMrelUp   = (((TH1F*)fin_jUp->Get(Form("hGGFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hGGFH%d",mH_)))->Integral());
  //	      SMrelDown = (((TH1F*)fin_jDown->Get(Form("hGGFH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hGGFH%d",mH_)))->Integral());
  //	    }
  //	    else{
  //	      SMrelUp   = 1.1; // conservative
  //	      SMrelDown = 1.1; // conservative
  //	    }
  //
  //	    SMrel     *= 1./rescaleggH;
  //	    SMrelUp   *= 1./rescaleggH;
  //	    SMrelDown *= 1./rescaleggH;
  //
  //	    VHrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral())),
  //				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral()))
  //				);
  //	    
  //	    if( ((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral()>0 ){
  //	      VHrelUp   = (((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());
  //	      VHrelDown = (((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());
  //	    }
  //	    else{
  //	      VHrelUp   = 1.1; // conservative
  //	      VHrelDown = 1.1; // conservative
  //	    }
  //
  //
  //	    VHrel     *= 1./rescaleVH;
  //	    VHrelUp   *= 1./rescaleVH;
  //	    VHrelDown *= 1./rescaleVH;
  //
  //	  }
  //	  else{
  //	    VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral())),
  //                              TMath::Abs((((TH1F*)fin_jDown->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral()))
  //                              );
  //          VBFrelUp   = (((TH1F*)fin_jUp->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral());
  //          VBFrelDown = (((TH1F*)fin_jDown->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral());
  //
  //          SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral())),
  //                              TMath::Abs((((TH1F*)fin_jDown->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral()))
  //                              );
  //
  //          SMrelUp   = (((TH1F*)fin_jUp->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral());
  //          SMrelDown = (((TH1F*)fin_jDown->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral());
  //	  }
  //
  //
  //	  float ZTTrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral())),
  //				    TMath::Abs((((TH1F*)fin_jDown->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral()))
  //				    );
  //
  //	  float ZTTrelUp   = (((TH1F*)fin_jUp->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral());
  //	  float ZTTrelDown = (((TH1F*)fin_jDown->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral());
  //
  //
  //	  float TTrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral())),
  //				    TMath::Abs((((TH1F*)fin_jDown->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral()))
  //				    );
  //
  //	  float TTrelUp   = (((TH1F*)fin_jUp->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral());
  //	  float TTrelDown = (((TH1F*)fin_jDown->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral());
  //
  //
  //	  float VVrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral())),
  //				    TMath::Abs((((TH1F*)fin_jDown->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral()))
  //				    );
  //
  //	  float VVrelUp   = (((TH1F*)fin_jUp->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral());
  //	  float VVrelDown = (((TH1F*)fin_jDown->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral());
  //
  //
  //	  string space      = "                   ";
  //	  string longspace  = "                      ";
  //	  string shortspace = "     ";
  //	  out << "CMS_scale_j";
  //	  out << longspace << "lnN" << shortspace;
  //	  if(!DOSUSY){
  //	    if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("2jet")!=string::npos) 
  //	      out <<          string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+VHrelDown), TMath::Abs(-1+VHrelUp) ) * TMath::Abs(-1+VHrelUp)/(-1+VHrelUp)   )); // VH
  //	    else
  //	      out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+SMrelDown), TMath::Abs(-1+SMrelUp))  * TMath::Abs(-1+SMrelUp)/(-1+SMrelUp)   )); // VH=GGF
  //	  }
  //	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+SMrelDown), TMath::Abs(-1+SMrelUp))  * TMath::Abs(-1+SMrelUp)/(-1+SMrelUp)   ));  // GGF/BB
  //	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+VBFrelDown),TMath::Abs(-1+VBFrelUp))* TMath::Abs(-1+VBFrelUp)/(-1+VBFrelUp) ));   // VBF/GG
  //	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // ZTT
  //	  out << space << "-" ;                                                // QCD
  //	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // W
  //	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // Z+fakes
  //
  //	  if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("boost")!=string::npos) 
  //	    out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));
  //
  //	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+TTrelDown),TMath::Abs(-1+TTrelUp))  * TMath::Abs(-1+TTrelUp)/(-1+TTrelUp) ));  // TT
  //
  //	  //if(binNameSpace.find("0jet_")!=string::npos) 
  //	  out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+VVrelDown),TMath::Abs(-1+VVrelUp) ) * TMath::Abs(-1+VVrelUp)/(-1+VVrelUp)));   // VV
  //	  //else
  //	  //out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+TTrelDown),TMath::Abs(-1+TTrelUp))  * TMath::Abs(-1+TTrelUp)/(-1+TTrelUp) ));  // TT
  //
  //	  out << space << "JEC uncertainty";
  //	  out << endl;
  //	}
  //	else if(line.find( Form("CMS_htt_muTau_%s_QCDNorm",binNameSpace.c_str()) )!=string::npos){
  //	 
  //	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(5)))  );
  //	  line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(6)))  );
  //	 
  //	  out << line << endl;
  //	}
  //	else if(line.find( Form("CMS_htt_muTau_%s_WNorm",binNameSpace.c_str()) )!=string::npos){
  //
  //	  // in VBF use extrapolation factor from W+3jets
  //	  if(binNameSpace.find("vbf")!=string::npos){
  //
  //	    float DataVBFSdb  = ((TH1F*)fin->Get("hParameters"))->GetBinContent(14);
  //	    float TTbarVBFSdb = ((TH1F*)fin->Get("hParameters"))->GetBinContent(15);
  //	    float QCDVBFSdb   = ((TH1F*)fin->Get("hParameters"))->GetBinContent(16);
  //
  //	    float ExtrapFactor = Wyield/(DataVBFSdb+TTbarVBFSdb+QCDVBFSdb);
  //
  //	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",  (DataVBFSdb+TTbarVBFSdb+QCDVBFSdb))));
  //	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",  ExtrapFactor)));
  //	  }
  //	  else{
  //	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(2)))  );
  //	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(1)))  );
  //	  }
  //	  out << line << endl;
  //	}
  //	else{
  //	  out << c << endl;
  //	}
  //    
  //    }
  //
  // }
  
  fin->Close();          delete fin;
  fin_tUp->Close();      delete fin_tUp;
  fin_tDown->Close();    delete fin_tDown;
  fin_jUp->Close();      delete fin_jUp;
  fin_jDown->Close();    delete fin_jDown;

  return;

}



void produceAll(TString outputDir="MuTau/res_ABCD_Moriond_v1", int useEmb=1){

  const int nVar=1;
  const int nM=15; // 15 for sm, 21 for mssm
  const int nCat=9; //9 sm; //6 before including "inclusive", 3 for MSSM
  const int nAn=5;

  string variables[nVar]={"diTauNSVfitMass"};
  //string variables[nVar]={"diTauVisMass"};
  //string variables[nVar]={"diTauCDFMass"};
  int mH[nM]={90,95,100,105,110,115,120,125,130,135,140,145,150,155,160};
  //int mH[nM]={80,90,100,110,120,130,140,160,180,200,250,300,350,400,450,500,600,700,800,900,1000};

  string analysis[nAn]={"nominal","TauUp","TauDown","JetUp","JetDown"};
  //string category[nCat]={"inclusive","novbfLow","novbfHigh","boostLow","boostHigh","vbf"}; //old
  string category[nCat]={"inclusive","novbfLow","novbfMedium","novbfHigh","boostMedium","boostHighhighhiggs","boostHighlowhiggs","vbf","vbfTight"};
  //string category[nCat]={"inclusive","bTag", "nobTag"}; //for MSSM

  for(int iVar = 0 ; iVar < nVar; iVar++)
    for(int iM = 0; iM < nM; iM++)
      for(int iAn=0 ; iAn<nAn ; iAn++)
	for(int iCat=0 ; iCat<nCat ; iCat++)
	  
	  produce(mH[iM],variables[iVar], analysis[iAn], category[iCat], outputDir, useEmb);

  /*
  void produce(   
	     int mH_=125,
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "MuTau/res_ABCD_Moriond_v1",
	     TString location  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534p2_Spring13_git/src/LLRAnalysis/Limits/bin/results/"
  */

}




int main(int argc, const char* argv[])
{

  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  TString outputDir="MuTau/res_ABCD_Moriond_v1";
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
