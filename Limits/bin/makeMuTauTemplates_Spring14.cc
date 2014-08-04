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
#define OldCat false
#define HiggsPtReweighting true

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
	     bool DOSUSY = false,
	     int useEmb        = 1,
// 	     TString location  = "~/TAU_ID/Release_OlivierTES/CMSSW_5_3_11_p6_analysis/src/LLRAnalysis/Limits/bin/results/MuTau/"
// 	     TString location  = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_11_p6_NewTauID/src/LLRAnalysis/Limits/bin/results/MuTau/"
	     TString location  = "/data_CMS/cms/davignon/NtuplesProduction_NewTrees_NewTriggers/CMSSW_5_3_11_p6_NewPAT/src/LLRAnalysis/Limits/bin/results/MuTau/"
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
    {
      binNameSpace =  "btag";
      if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	{
// 	  binNameSpace = "btaghigh" ;
// 	  binNameSpace = "btag_hightau" ;	
	  binNameSpace = "btag_high" ;
}
      else if(bin_.find("bTagLow")!=string::npos)
	{
// 	  binNameSpace = "btaglow" ;
// 	  binNameSpace = "btag_lowtau" ;
	  binNameSpace = "btag_low" ;
	}
    }
  else if(bin_.find("nobTag")!=string::npos)
    {
      binNameSpace =  "nobtag";
      if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	{
// 	  binNameSpace = "nobtaghigh" ;
// 	  binNameSpace = "nobtag_hightau" ;
	  binNameSpace = "nobtag_high" ;
	}
      else if(bin_.find("nobTagMedium")!=string::npos)
	{
// 	  binNameSpace = "nobtagmedium" ;
// 	  binNameSpace = "nobtag_mediumtau" ;
	  binNameSpace = "nobtag_medium" ;
	}
      else if(bin_.find("nobTagLow")!=string::npos)
	{
// 	  binNameSpace = "nobtaglow" ;
// 	  binNameSpace = "nobtag_lowtau" ;
	  binNameSpace = "nobtag_low" ;
	}
    }
  
  else if(bin_.find("twoJets")!=string::npos) 
    binNameSpace =  "SMpre2";

  else if(bin_.find("oneJet")!=string::npos) 
    binNameSpace =  "SMpre2a";

  //TFile* fTemplOut = new TFile(Form(location+"/%s/datacards/muTauSM_%s.root",outputDir.Data(), variable_.c_str()),"UPDATE");
  string theory =  !DOSUSY ? "SM" : "MSSM" ;
			     
  TString WeightNoWeight = "PtWeight" ;
  if(HiggsPtReweighting) WeightNoWeight = "PtWeight" ;
  else WeightNoWeight = "NoPtWeight" ;
  TFile* fTemplOut = new TFile(Form(location+"%s/datacards/muTau%s_%s_newNames4_%s.root",outputDir.Data(), theory.c_str(),variable_.c_str(), WeightNoWeight.Data()),"UPDATE");

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
      TH1F* hSgn1 ;
      if(HiggsPtReweighting) hSgn1 = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
      else hSgn1 = (TH1F*)fin->Get(Form("hSUSYNoWeightGGH%d",mH_));
      hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn1_Up = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
      hSgn1_Up->SetName(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
      hSgn1_Up->Write(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));

      TH1F* hSgn1_Down = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
      hSgn1_Down->SetName(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
      hSgn1_Down->Write(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));

      TH1F* hSgn1_PtUp = (TH1F*)fin->Get(Form("hSUSYGGH%dUp",mH_));
//       TH1F* hSgn1_PtUp = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
//       //IN test
//       hSgn1_PtUp->Scale(1.2*hSgn1->Integral()/hSgn1_PtUp->Integral()); 
//       //
      hSgn1_PtUp->SetName(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
      hSgn1_PtUp->Write(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));

      TH1F* hSgn1_PtDown = (TH1F*)fin->Get(Form("hSUSYGGH%dDown",mH_));
//       TH1F* hSgn1_PtDown = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
//       //IN test
//       hSgn1_PtDown->Scale(0.8*hSgn1->Integral()/hSgn1_PtDown->Integral()); 
//       //
      hSgn1_PtDown->SetName(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      hSgn1_PtDown->Write(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      
      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
      hSgn2->SetName(Form("bbH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("bbH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn2_Up = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
      hSgn2_Up->SetName(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
      hSgn2_Up->Write(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));

      TH1F* hSgn2_Down = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
      hSgn2_Down->SetName(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
      hSgn2_Down->Write(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));

      TH1F* hSgn2_PtUp = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
      hSgn2_PtUp->SetName(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
      hSgn2_PtUp->Write(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));

      TH1F* hSgn2_PtDown = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
      hSgn2_PtDown->SetName(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      hSgn2_PtDown->Write(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      
      //SM signal
      TH1F* hSMGGFH125 = (TH1F*)fin->Get("hGGFH125");
      hSMGGFH125->SetName("ggH_SM125" );
      hSMGGFH125->Write("ggH_SM125");

      TH1F* hSMVBFH125 = (TH1F*)fin->Get("hVBFH125");
      hSMVBFH125->SetName("qqH_SM125" );
      hSMVBFH125->Write("qqH_SM125");

      TH1F* hSMVH125 = (TH1F*)fin->Get("hVH125");
      hSMVH125->SetName("VH_SM125" );
      hSMVH125->Write("VH_SM125");

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
      hDataEmb_fb->SetName(Form("ZTT%s_fine_binning",suffix.c_str())); 
      hDataEmb_fb->Write(Form("ZTT%s_fine_binning",suffix.c_str()));

      // ----- QCD ------
      float maxBin = 50.;
      if(bin_.find("bTag")!=string::npos && bin_.find("nobTag")==string::npos)
        maxBin = 40.;
      TH1F *hQCD;
      if(bin_.find("nobTag")!=string::npos){
	// 	hQCD = ((TH1F*)fin->Get("hQCD"));
	// 	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
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
      hQCD_fb->SetName(Form("QCD%s_fine_binning"    ,suffix.c_str())); 
      if(bin_.find("nobTag")!=string::npos){
	for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	  if(hQCD_fb->GetBinCenter(b)<=maxBin){
	    hQCD_fb->SetBinContent(b,1.1*hQCD_fb->GetBinContent(b));
	  }
	}
      }
      //checkValidity(hQCD_fb);
      hQCD_fb->Write(Form("QCD%s_fine_binning"    ,suffix.c_str()));
      //Add a 10% shape uncert. to QCD for mass < 60 GeV
      if(suffix == ""){
	TString QCDShapeFb("QCD_CMS_htt_QCDShape_mutau");
	QCDShapeFb = QCDShapeFb+"_"+binNameSpace;
	
	TH1F* hQCD_fbUp=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVUp_fine_binning");
	TH1F* hQCD_fbDown=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVDown_fine_binning");
	for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	  if(hQCD_fb->GetBinCenter(b)<=maxBin){
	    hQCD_fbUp->SetBinContent(b,1.1*hQCD_fbUp->GetBinContent(b));
	    hQCD_fbDown->SetBinContent(b,0.9*hQCD_fbDown->GetBinContent(b));
	  }
	}
	checkValidity(hQCD_fbUp);
	hQCD_fbUp->Write(QCDShapeFb+"_8TeVUp_fine_binning");
	checkValidity(hQCD_fbDown);
	hQCD_fbDown->Write(QCDShapeFb+"_8TeVDown_fine_binning");
      }
      
      // ----- W ------
      if(bin_.find("nobTag")!=string::npos){

        TH1F* hW = ((TH1F*)fin->Get("hW"));
        hW->SetName(Form("W%s"           ,suffix.c_str()));
        hW->Write(Form("W%s"           ,suffix.c_str()));

	//Up/Down for TauFake weight
	if(suffix == ""){
          TString WShape("W_CMS_htt_WShape_mutau");
//           WShape = WShape+"_"+binNameSpace;
	  if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");
	    }	    
	  else if(bin_.find("nobTagMedium")!=string::npos)
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }
	  else if(bin_.find("nobTagLow")!=string::npos)
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }
	  else
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }
	}
      }
      else{
        TH1F* hW = ((TH1F*)fin->Get("hWLooseBTag"));
        hW->SetName(Form("W%s"           ,suffix.c_str()));
        hW->Write(Form("W%s"           ,suffix.c_str()));

	//Up/Down for TauFake weight
        if(suffix == ""){
          TString WShape("W_CMS_htt_WShape_mutau");
//           WShape = WShape+"_"+binNameSpace;

	  if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }
	  else if(bin_.find("bTagLow")!=string::npos)
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }	  
	  else
	    {
	      TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 	      hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp");
// 	      hW_TFUp->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
	      TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 	      hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown");
// 	      hW_TFDown->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	    }
        }
      }

      TH1F* hW_fb = ((TH1F*)fin->Get("hW_fb"));
      hW_fb->SetName(Form("W%s_fine_binning"           ,suffix.c_str()));
      hW_fb->Write(Form("W%s_fine_binning"           ,suffix.c_str()));
      
      //Up/Down for TauFake weight
      if(suffix == ""){
	TString WShape("W_CMS_htt_WShape_mutau");
// 	WShape = WShape+"_"+binNameSpace;
	if(bin_.find("nobTag")!=string::npos){

	  if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	  else if(bin_.find("nobTagMedium")!=string::npos)
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	  else if(bin_.find("nobTagLow")!=string::npos)
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	  else
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	}
	else{
	  
	  if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	  else if(bin_.find("bTagLow")!=string::npos)
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	  else
	    {
	      TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 	      hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
// 	      hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
	      hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
	      hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
	      TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 	      hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
// 	      hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
	      hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
	      hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	    }
	}
      }

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
      hZmj_fb->SetName(Form("ZJ%s_fine_binning"        ,suffix.c_str())); 
      hZmj_fb->Write(Form("ZJ%s_fine_binning"        ,suffix.c_str())); 
      TH1F* hZmm_fb = ((TH1F*)fin->Get("hZmm_fb")); 
      hZmm_fb->SetName(Form("ZL%s_fine_binning"        ,suffix.c_str())); 
      hZmm_fb->Write(Form("ZL%s_fine_binning"        ,suffix.c_str())); 
      TH1F* hZfakes_fb = ((TH1F*)fin->Get("hZmj_fb")); 
      hZfakes_fb->Add(hZmm_fb);
      hZfakes_fb->SetName(Form("ZLL%s_fine_binning"    ,suffix.c_str())); 
      hZfakes_fb->Write(Form("ZLL%s_fine_binning"    ,suffix.c_str())); 


      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
      hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      TH1F* hTTbUp = ((TH1F*)fin->Get("hTTbUp"));
      hTTbUp->SetName(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVUp"));
      hTTbUp->Write(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVUp"));
      TH1F* hTTbDown = ((TH1F*)fin->Get("hTTbDown"));
      hTTbDown->SetName(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVDown"));
      hTTbDown->Write(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVDown"));

      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));

      TH1F* hTTb_fb = ((TH1F*)fin->Get("hTTb_fb")); 
      hTTb_fb->SetName(Form("TT%s_fine_binning"        ,suffix.c_str())); 
      hTTb_fb->Write(Form("TT%s_fine_binning"        ,suffix.c_str())); 
      TH1F* hVV_fb = ((TH1F*)fin->Get("hVV")); 
      hVV_fb->SetName(Form("VV%s_fine_binning"         ,suffix.c_str())); 
      hVV_fb->Write(Form("VV%s_fine_binning"         ,suffix.c_str()));
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
      TH1F* hTTbUp = ((TH1F*)fin->Get("hTTbUp"));
      hTTbUp->SetName(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVUp"));
      hTTbUp->Write(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVUp"));
      TH1F* hTTbDown = ((TH1F*)fin->Get("hTTbDown"));
      hTTbDown->SetName(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVDown"));
      hTTbDown->Write(Form("TT%s"        ,"_CMS_htt_ttbarPtReweight_8TeVDown"));
      
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
        TH1F* hSgn1 ;
	if(HiggsPtReweighting) hSgn1 = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
	else hSgn1 = (TH1F*)fin->Get(Form("hSUSYNoWeightGGH%d",mH_));
	hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
        hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_))==0){
	TH1F* hSgn1_Up = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
	hSgn1_Up->SetName(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
	hSgn1_Up->Write(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
      }
      if(dir->FindObjectAny(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_))==0){
	TH1F* hSgn1_Down = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
	hSgn1_Down->SetName(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
	hSgn1_Down->Write(Form("ggH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
      }
      if(dir->FindObjectAny(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_))==0){
	TH1F* hSgn1_PtUp = (TH1F*)fin->Get(Form("hSUSYGGH%dUp",mH_));
// 	TH1F* hSgn1_PtUp = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
// 	//IN test
//         TH1F* hSgn1 ;
// 	if(HiggsPtReweighting) hSgn1 = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
// 	else hSgn1 = (TH1F*)fin->Get(Form("hSUSYNoWeightGGH%d",mH_));
// 	hSgn1_PtUp->Scale(1.2*hSgn1->Integral()/hSgn1_PtUp->Integral()); 
// 	//
	hSgn1_PtUp->SetName(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
	hSgn1_PtUp->Write(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
      }
      if(dir->FindObjectAny(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_))==0){
	TH1F* hSgn1_PtDown = (TH1F*)fin->Get(Form("hSUSYGGH%dDown",mH_));
// 	TH1F* hSgn1_PtDown = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
// 	//IN test
//         TH1F* hSgn1 ;
// 	if(HiggsPtReweighting) hSgn1 = (TH1F*)fin->Get(Form("hSUSYGGH%d",mH_));
// 	else hSgn1 = (TH1F*)fin->Get(Form("hSUSYNoWeightGGH%d",mH_));
// 	hSgn1_PtDown->Scale(0.8*hSgn1->Integral()/hSgn1_PtDown->Integral()); 
// 	//
	hSgn1_PtDown->SetName(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
	hSgn1_PtDown->Write(Form("ggH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      }
      if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
        TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
	hSgn2->SetName(Form("bbH%d%s" , mH_,suffix.c_str()));
        hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_))==0){
	TH1F* hSgn2_Up = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
	hSgn2_Up->SetName(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
	hSgn2_Up->Write(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVUp",mH_));
      }
      if(dir->FindObjectAny(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_))==0){
	TH1F* hSgn2_Down = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
	hSgn2_Down->SetName(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
	hSgn2_Down->Write(Form("bbH%d_CMS_eff_t_mssmHigh_mutau_8TeVDown",mH_));
      }
      if(dir->FindObjectAny(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_))==0){
	TH1F* hSgn2_PtUp = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
	hSgn2_PtUp->SetName(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
	hSgn2_PtUp->Write(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVUp",mH_));
      }
      if(dir->FindObjectAny(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_))==0){
	TH1F* hSgn2_PtDown = (TH1F*)fin->Get(Form("hSUSYBBH%d",mH_));
	hSgn2_PtDown->SetName(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
	hSgn2_PtDown->Write(Form("bbH%d_CMS_htt_higgsPtReweight_8TeVDown",mH_));
      }     //SM signal
      if(dir->FindObjectAny("ggH_SM125")==0 ){
      	TH1F* hSMGGFH125 = (TH1F*)fin->Get("hGGFH125");
      	hSMGGFH125->SetName("ggH_SM125");
      	hSMGGFH125->Write("ggH_SM125");
      }
      if(dir->FindObjectAny("qqH_SM125")==0 ){
      	TH1F* hSMVBFH125 = (TH1F*)fin->Get("hVBFH125");
      	hSMVBFH125->SetName("qqH_SM125");
      	hSMVBFH125->Write("qqH_SM125");
      }
      if(dir->FindObjectAny("VH_SM125")==0 ){
      	TH1F* hSMVH125 = (TH1F*)fin->Get("hVH125");
      	hSMVH125->SetName("VH_SM125");
      	hSMVH125->Write("VH_SM125");
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
	hDataEmb_fb->SetName(Form("ZTT%s_fine_binning",suffix.c_str()));  
	hDataEmb_fb->Write(Form("ZTT%s_fine_binning",suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	float maxBin = 50.;
        if(bin_.find("bTag")!=string::npos && bin_.find("nobTag")==string::npos)
          maxBin = 40.;
	TH1F *hQCD;
	if(bin_.find("nobTag")!=string::npos){
	  // 	  hQCD = ((TH1F*)fin->Get("hQCD"));
	  // 	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
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
	hQCD_fb->SetName(Form("QCD%s_fine_binning"    ,suffix.c_str()));
	if(bin_.find("nobTag")!=string::npos){
	  for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	    if(hQCD_fb->GetBinCenter(b)<=maxBin){
	      hQCD_fb->SetBinContent(b,1.1*hQCD_fb->GetBinContent(b));
	    }
	  }
	}
	//checkValidity(hQCD_fb);
	hQCD_fb->Write(Form("QCD%s_fine_binning"    ,suffix.c_str()));
	//Add a 10% shape uncert. to QCD for mass < 60 GeV
	if(suffix == ""){
	  TString QCDShapeFb("QCD_CMS_htt_QCDShape_mutau");
	  QCDShapeFb = QCDShapeFb+"_"+binNameSpace;
	  
	  TH1F* hQCD_fbUp=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVUp_fine_binning");
	  TH1F* hQCD_fbDown=(TH1F*)hQCD_fb->Clone(QCDShapeFb+"_8TeVDown_fine_binning");
	  for(Int_t b=1;b<=hQCD_fb->GetNbinsX();b++){
	    if(hQCD_fb->GetBinCenter(b)<=maxBin){
	      hQCD_fbUp->SetBinContent(b,1.1*hQCD_fbUp->GetBinContent(b));
	      hQCD_fbDown->SetBinContent(b,0.9*hQCD_fbDown->GetBinContent(b));
	    }
	  }
	  //checkValidity(hQCD_fbUp);
	  hQCD_fbUp->Write(QCDShapeFb+"_8TeVUp_fine_binning");
	  //checkValidity(hQCD_fbDown);
	  hQCD_fbDown->Write(QCDShapeFb+"_8TeVDown_fine_binning");
	}
      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
        if(bin_.find("nobTag")!=string::npos){
          TH1F* hW = ((TH1F*)fin->Get("hW"));
          hW->SetName(Form("W%s"           ,suffix.c_str()));
          hW->Write(Form("W%s"           ,suffix.c_str()));
	  
	  //Up/Down for TauFake weight
	  if(suffix == ""){
	    TString WShape("W_CMS_htt_WShape_mutau");
// 	    WShape = WShape+"_"+binNameSpace;
	    if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	    else if(bin_.find("nobTagMedium")!=string::npos)
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	    else if(bin_.find("nobTagLow")!=string::npos)
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	    else
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
	      hW_TFUp->SetName(WShape+"_8TeVUp");
	      hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
	      hW_TFDown->SetName(WShape+"_8TeVDown");
	      hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	  }
        }
        else{
          TH1F* hW = ((TH1F*)fin->Get("hWLooseBTag"));
          hW->SetName(Form("W%s"           ,suffix.c_str()));
          hW->Write(Form("W%s"           ,suffix.c_str()));

	  //Up/Down for TauFake weight
	  if(suffix == ""){
	    TString WShape("W_CMS_htt_WShape_mutau");
// 	    WShape = WShape+"_"+binNameSpace;
	    if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
		hW_TFUp->SetName(WShape+"_8TeVUp");
		hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
		hW_TFDown->SetName(WShape+"_8TeVDown");
		hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	    else if(bin_.find("bTagLow")!=string::npos)
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
		hW_TFUp->SetName(WShape+"_8TeVUp");
		hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
		hW_TFDown->SetName(WShape+"_8TeVDown");
		hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }	  
	    else
	      {
		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp");
		hW_TFUp->SetName(WShape+"_8TeVUp");
		hW_TFUp->Write(WShape+"_8TeVUp");
		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown");
		hW_TFDown->SetName(WShape+"_8TeVDown");
		hW_TFDown->Write(WShape+"_8TeVDown");	    
	      }
	  }
        }

	TH1F* hW_fb = ((TH1F*)fin->Get("hW_fb"));
        hW_fb->SetName(Form("W%s_fine_binning"           ,suffix.c_str()));
        hW_fb->Write(Form("W%s_fine_binning"           ,suffix.c_str()));

	//Up/Down for TauFake weight
	if(suffix == ""){
	  TString WShape("W_CMS_htt_WShape_mutau");
// 	  WShape = WShape+"_"+binNameSpace;
	  if(bin_.find("nobTag")!=string::npos){
	    if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
	      }
	    else if(bin_.find("nobTagMedium")!=string::npos)
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	    else if(bin_.find("nobTagLow")!=string::npos)
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	    else
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	  }
	  else{
	    if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	    else if(bin_.find("bTagLow")!=string::npos)
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	    else
	      {
		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
		hW_TFUp_fb->SetName(WShape+"_8TeVUp_fine_binning");
		hW_TFUp_fb->Write(WShape+"_8TeVUp_fine_binning");
		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
		hW_TFDown_fb->SetName(WShape+"_8TeVDown_fine_binning");
		hW_TFDown_fb->Write(WShape+"_8TeVDown_fine_binning");
	      }
	  }
	}
      }
 //      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
// 	if(bin_.find("nobTag")!=string::npos){
// 	  TH1F* hW = ((TH1F*)fin->Get("hW"));
// 	  hW->SetName(Form("W%s"           ,suffix.c_str()));
// 	  hW->Write(Form("W%s"           ,suffix.c_str()));

// 	  //Up/Down for TauFake weight
// 	  if(suffix == ""){
// 	    if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown");
// 	      }
// 	    else if(bin_.find("nobTagMedium")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown");
// 	      }
// 	    else if(bin_.find("nobTagLow")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown");
// 	      }
// 	    else
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hW_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hW_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown");
// 	      }
// 	  }
// 	}
// 	else{
// 	  TH1F* hW = ((TH1F*)fin->Get("hWLooseBTag"));
// 	  hW->SetName(Form("W%s"           ,suffix.c_str()));
// 	  hW->Write(Form("W%s"           ,suffix.c_str()));

// 	  //Up/Down for TauFake weight
// 	  if(suffix == ""){
// 	    if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown");
// 	      }
// 	    else if(bin_.find("bTagLow")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown");
// 	      }	  
// 	    else
// 	      {
// 		TH1F* hW_TFUp = ((TH1F*)fin->Get("hWLooseBTag_TFUp"));
// 		hW_TFUp->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp");
// 		hW_TFUp->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp");
// 		TH1F* hW_TFDown = ((TH1F*)fin->Get("hWLooseBTag_TFDown"));
// 		hW_TFDown->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown");
// 		hW_TFDown->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown");
// 	      }
// 	  }
// 	}

// 	TH1F* hW_fb = ((TH1F*)fin->Get("hW_fb"));  
// 	hW_fb->SetName(Form("W%s_fine_binning"           ,suffix.c_str()));  
// 	hW_fb->Write(Form("W%s_fine_binning"           ,suffix.c_str()));

// 	//Up/Down for TauFake weight
// 	if(suffix == ""){
// 	  if(bin_.find("nobTag")!=string::npos){
// 	    if(bin_.find("nobTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaghigh_8TeVDown_fine_binning");
// 	      }
// 	    else if(bin_.find("nobTagMedium")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtagmedium_8TeVDown_fine_binning");
// 	      }
// 	    else if(bin_.find("nobTagLow")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtaglow_8TeVDown_fine_binning");
// 	      }
// 	    else
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_nobtag_8TeVDown_fine_binning");
// 	      }
// 	  }
// 	  else{
// 	    if(bin_.find("bTagHigh")!=string::npos && !bin_.find("HighMt")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaghigh_8TeVDown_fine_binning");
// 	      }
// 	    else if(bin_.find("bTagLow")!=string::npos)
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btaglow_8TeVDown_fine_binning");
// 	      }
// 	    else
// 	      {
// 		TH1F* hW_TFUp_fb = ((TH1F*)fin->Get("hW_TFUp_fb"));
// 		hW_TFUp_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
// 		hW_TFUp_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVUp_fine_binning");
// 		TH1F* hW_TFDown_fb = ((TH1F*)fin->Get("hW_TFDown_fb"));
// 		hW_TFDown_fb->SetName("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
// 		hW_TFDown_fb->Write("W_CMS_htt_WShape_mutau_btag_8TeVDown_fine_binning");
// 	      }
// 	  }
// 	}
//       }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
        TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
        hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
        hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));

	TH1F* hZmj_fb = ((TH1F*)fin->Get("hZmj_fb"));   
	hZmj_fb->SetName(Form("ZJ%s_fine_binning"        ,suffix.c_str()));   
	hZmj_fb->Write(Form("ZJ%s_fine_binning"        ,suffix.c_str()));
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
	hZmm_fb->SetName(Form("ZL%s_fine_binning"        ,suffix.c_str()));   
	hZmm_fb->Write(Form("ZL%s_fine_binning"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
        TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
        hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str()));
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));

	TH1F* hZfakes_fb = ((TH1F*)fin->Get("hZmj_fb"));
	TH1F* hZmm_fb = ((TH1F*)fin->Get("hZmm_fb"));
	hZfakes_fb->Add(hZmm_fb);  
	hZfakes_fb->SetName(Form("ZLL%s_fine_binning"    ,suffix.c_str()));   
	hZfakes_fb->Write(Form("ZLL%s_fine_binning"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
        TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
        hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
        hTTb->Write(Form("TT%s"        ,suffix.c_str()));

        TH1F* hTTbUp = ((TH1F*)fin->Get("hTTbUp"));
        hTTbUp->SetName(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hTTbUp->Write(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
 
	TH1F* hTTbDown = ((TH1F*)fin->Get("hTTbDown"));
        hTTbDown->SetName(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hTTbDown->Write(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
	
	TH1F* hTTb_fb = ((TH1F*)fin->Get("hTTb_fb"));   
	hTTb_fb->SetName(Form("TT%s_fine_binning"        ,suffix.c_str()));   
	hTTb_fb->Write(Form("TT%s_fine_binning"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
        TH1F* hVV = ((TH1F*)fin->Get("hVV"));
        hVV->SetName(Form("VV%s"         ,suffix.c_str()));
        hVV->Write(Form("VV%s"         ,suffix.c_str()));

	TH1F* hVV_fb = ((TH1F*)fin->Get("hVV"));   
	hVV_fb->SetName(Form("VV%s_fine_binning"         ,suffix.c_str()));   
	hVV_fb->Write(Form("VV%s_fine_binning"         ,suffix.c_str()));
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
	TH1F* hTTbUp = ((TH1F*)fin->Get("hTTbUp"));
        hTTbUp->SetName(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
        hTTbUp->Write(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVUp"));
 	TH1F* hTTbDown = ((TH1F*)fin->Get("hTTbDown"));
        hTTbDown->SetName(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));
        hTTbDown->Write(Form("TT%s"        ,"_CMS_htt_ZLScale_mutau_8TeVDown"));   
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


void produceOne(  TString outputDir = "Results_ABCD_AntiMu1_AntiEle1_TauIso1_Datacards",bool DOSUSY = false,TString boostLabel = "" ){

  vector<string> variables;
  vector<int> mH;
  vector<std::string> analysis;

//   variables.push_back("diTauVisMass");
//   variables.push_back("genVPt");
  variables.push_back("diTauNSVfitMass");
//   variables.push_back("ptL2");

  if(!DOSUSY){
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
  }
  else{
    mH.push_back(80);
    mH.push_back(90);
    mH.push_back(100);
    mH.push_back(110);
    mH.push_back(120);
    mH.push_back(130);
    mH.push_back(140);
    mH.push_back(160);
    mH.push_back(180);
    mH.push_back(200);
    mH.push_back(250);
    mH.push_back(300);
    mH.push_back(350);
    mH.push_back(400);
    mH.push_back(450);
    mH.push_back(500);
    mH.push_back(600);
    mH.push_back(700);
    mH.push_back(800);
    mH.push_back(900);
    mH.push_back(1000);
  }

  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){
      //       if(!DO2DFIT){
      if(!DOSUSY){
	produce(mH[j],variables[i], "nominal" , "inclusive", outputDir);
	produce(mH[j],variables[i], "TauUp"   , "inclusive", outputDir);
	produce(mH[j],variables[i], "TauDown" , "inclusive", outputDir);
	produce(mH[j],variables[i], "JetUp"   , "inclusive", outputDir);
	produce(mH[j],variables[i], "JetDown" , "inclusive", outputDir);

	produce(mH[j],variables[i], "nominal" , "novbfLow", outputDir);
	produce(mH[j],variables[i], "TauUp"   , "novbfLow", outputDir);
	produce(mH[j],variables[i], "TauDown" , "novbfLow", outputDir);
	produce(mH[j],variables[i], "JetUp"   , "novbfLow", outputDir);
	produce(mH[j],variables[i], "JetDown" , "novbfLow", outputDir);

	produce(mH[j],variables[i], "nominal" , "novbfMedium", outputDir); 
        produce(mH[j],variables[i], "TauUp"   , "novbfMedium", outputDir); 
        produce(mH[j],variables[i], "TauDown" , "novbfMedium", outputDir); 
        produce(mH[j],variables[i], "JetUp"   , "novbfMedium", outputDir); 
        produce(mH[j],variables[i], "JetDown" , "novbfMedium", outputDir);

	produce(mH[j],variables[i], "nominal" , "novbfHigh", outputDir);
	produce(mH[j],variables[i], "TauUp"   , "novbfHigh", outputDir);
	produce(mH[j],variables[i], "TauDown" , "novbfHigh", outputDir);
	produce(mH[j],variables[i], "JetUp"   , "novbfHigh", outputDir);
	produce(mH[j],variables[i], "JetDown" , "novbfHigh", outputDir);
	/* //old
	   produce(mH[j],variables[i], "nominal"        , Form("boost%sLow",boostLabel.Data()), outputDir);
	   produce(mH[j],variables[i], "TauUp"   , Form("boost%sLow",boostLabel.Data()), outputDir);
	   produce(mH[j],variables[i], "TauDown" , Form("boost%sLow",boostLabel.Data()), outputDir);
	   produce(mH[j],variables[i], "JetUp"   , Form("boost%sLow",boostLabel.Data()), outputDir);
	   produce(mH[j],variables[i], "JetDown" , Form("boost%sLow",boostLabel.Data()), outputDir);
	*/

	produce(mH[j],variables[i], "nominal" , "boostMedium", outputDir); 
        produce(mH[j],variables[i], "TauUp"   , "boostMedium", outputDir); 
        produce(mH[j],variables[i], "TauDown" , "boostMedium", outputDir); 
        produce(mH[j],variables[i], "JetUp"   , "boostMedium", outputDir); 
        produce(mH[j],variables[i], "JetDown" , "boostMedium", outputDir);


	produce(mH[j],variables[i], "nominal" , "boostHighhighhiggs", outputDir);  
        produce(mH[j],variables[i], "TauUp"   , "boostHighhighhiggs", outputDir);  
        produce(mH[j],variables[i], "TauDown" , "boostHighhighhiggs", outputDir);  
        produce(mH[j],variables[i], "JetUp"   , "boostHighhighhiggs", outputDir);  
        produce(mH[j],variables[i], "JetDown" , "boostHighhighhiggs", outputDir);

	produce(mH[j],variables[i], "nominal" , "boostHighlowhiggs", outputDir);   
        produce(mH[j],variables[i], "TauUp"   , "boostHighlowhiggs", outputDir);   
        produce(mH[j],variables[i], "TauDown" , "boostHighlowhiggs", outputDir);   
        produce(mH[j],variables[i], "JetUp"   , "boostHighlowhiggs", outputDir);   
        produce(mH[j],variables[i], "JetDown" , "boostHighlowhiggs", outputDir);

	produce(mH[j],variables[i], "nominal" , "vbf", outputDir);
	produce(mH[j],variables[i], "TauUp"   , "vbf", outputDir);
	produce(mH[j],variables[i], "TauDown" , "vbf", outputDir);
	produce(mH[j],variables[i], "JetUp"   , "vbf", outputDir);
	produce(mH[j],variables[i], "JetDown" , "vbf", outputDir);

	produce(mH[j],variables[i], "nominal" , "vbfTight", outputDir); 
        produce(mH[j],variables[i], "TauUp"   , "vbfTight", outputDir); 
        produce(mH[j],variables[i], "TauDown" , "vbfTight", outputDir); 
        produce(mH[j],variables[i], "JetUp"   , "vbfTight", outputDir); 
        produce(mH[j],variables[i], "JetDown" , "vbfTight", outputDir);
      }
      else {//SUSY
	produce(mH[j],variables[i], "nominal" , "inclusive", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "inclusive", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "inclusive", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "inclusive", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "inclusive", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "bTag", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "bTag", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "bTag", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "bTag", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "bTag", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "nobTag", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "nobTag", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "nobTag", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "nobTag", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "nobTag", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "bTagHigh", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "bTagHigh", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "bTagHigh", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "bTagHigh", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "bTagHigh", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "nobTagHigh", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "nobTagHigh", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "nobTagHigh", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "nobTagHigh", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "nobTagHigh", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "nobTagMedium", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "nobTagMedium", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "nobTagMedium", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "nobTagMedium", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "nobTagMedium", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "bTagLow", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "bTagLow", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "bTagLow", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "bTagLow", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "bTagLow", outputDir,true);

	produce(mH[j],variables[i], "nominal" , "nobTagLow", outputDir,true);
	produce(mH[j],variables[i], "TauUp"   , "nobTagLow", outputDir,true);
	produce(mH[j],variables[i], "TauDown" , "nobTagLow", outputDir,true);
	produce(mH[j],variables[i], "JetUp"   , "nobTagLow", outputDir,true);
	produce(mH[j],variables[i], "JetDown" , "nobTagLow", outputDir,true);
      }     
    }//mH
  }//variables
}



void produceAll(){ 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_Datacards",true); 
  //##AntiEMVA5
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5VLoose_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Medium_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Tight_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5VTight_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   ////##AntiMu
  //   produceOne("Results_ABCD_AntiMu2Tight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   produceOne("Results_ABCD_AntiMuMVATight_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEle5Loose_HPSDB3H_TauOldDM_OldEleID_Control",true);
  //   //##TauID
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTLoose_TauOldDM_OldEleID_Datacards",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTMedium_TauOldDM_OldEleID_Datacards",true);
  // produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_Datacards",true);
  // produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLTVTight_TauOldDM_OldEleID_Datacards",true);


  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwLT_TauOldDM_OldEleID_Datacards",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3oldDMwoLT_TauOldDM_OldEleID_Datacards",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3newDMwLT_TauNewDM_OldEleID_Datacards",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEle5Loose_HPSMVA3newDMwoLT_TauNewDM_OldEleID_Datacards",true);

  //////TauPt bins
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt20_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt30_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt4560_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt60_OldEleID_Datacards",true);
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt6090_OldEleID_Datacards",true);   
  //   produceOne("Results_ABCD_AntiMu3Tight_AntiEleLoose_HPSDB3H_TauOldDM_taupt90_OldEleID_Datacards",true); 

  //////TauPt bins study
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt20_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt30_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt3045_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt45_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt4560_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_taupt60_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt20_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_Datacards",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_Datacards",true);

  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt20_OldEleID_DatacardsRelax",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID_DatacardsRelax",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt3045_OldEleID_DatacardsRelax",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt45_OldEleID_DatacardsRelax",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt4560_OldEleID_DatacardsRelax",true); 
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt60_OldEleID_DatacardsRelax",true);

  //Ivo good July 14
//   TString OutFileName ;
//   if(HiggsPtReweighting) OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DatacardFix/datacards/muTau*_PtWeight.root") ;
//   else OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DatacardFix/datacards/muTau*_NoPtWeight.root") ;
//   TString Command = "rm "+OutFileName ;
//   gSystem->Exec(Command.Data());

//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_PlotsHiggspT_020414",true);
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DatacardsTTWeights",true);
//  produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_Datacards",true);
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DatacardFix",true);

//Zmm SF datacards
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZmmSel_ZmmSFDatacards2",true);
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZmmSel1Prong0Pi0BL_ZmmSFDatacards2",true);
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZmmSel1Prong0Pi0EC_ZmmSFDatacards2",true);
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZmmSel1Prong1Pi0BL_ZmmSFDatacards2",true);//last ones
//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_ZmmSel1Prong1Pi0EC_ZmmSFDatacards2",true);

//   TString OutFileName ;
//   if(HiggsPtReweighting) OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_010414/datacards/muTau*_PtWeight.root") ;
//   else OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_010414/datacards/muTau*_NoPtWeight.root") ;
//   TString Command = "rm "+OutFileName ;
//   gSystem->Exec(Command.Data());

//   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_010414",true);

  
  TString OutFileName ;
  if(HiggsPtReweighting) OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut_Datacards_280714/datacards/muTau*_PtWeight.root") ;
  else OutFileName = Form("results/MuTau/Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut_Datacards_280714/datacards/muTau*_NoPtWeight.root") ;
  TString Command = "rm "+OutFileName ;
  gSystem->Exec(Command.Data());

  produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_SVfitMassCut_Datacards_280714",true);



  ////SM check
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSDB3H_TauOldDM_OldEleID_DatacardsSM");
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_OldEleID_DatacardsSM");
  
  ///TauPt datacards
  //   produceOne("Results_ABCD_AntiMuMVAMedium_AntiEleLoose_HPSMVA3oldDMwLTTight_TauOldDM_taupt30_OldEleID_Datacards",true); 

  //Datacards Olivier 28/07/14
  

}



int main(int argc, const char* argv[])
{
  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  if(argc==1)      produceAll();
  else if(argc==2) produceOne(argv[1]);
  else return -1;

}
