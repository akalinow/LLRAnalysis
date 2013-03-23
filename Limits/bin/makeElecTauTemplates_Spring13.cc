#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

//#include "TauAnalysis/FittingTools/interface/TH2DynamicRebinner.h"

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
#define DO2DFIT false

using namespace std;

float higgsXsection(int mH = 125, string process = "ggH"){
  float xsection = 1.0;
  if(process.find("ggH")!=string::npos){
    if(mH == 110) xsection = 25.04                   * 7.95E-02;
    if(mH == 115) xsection = 22.96                   * 7.58E-02;
    if(mH == 120) xsection = 21.13                   * 7.04E-02;
    if(mH == 125) xsection = 19.52                   * 6.32E-02;
    if(mH == 130) xsection = 18.07                   * 5.45E-02;
    if(mH == 135) xsection = 16.79                   * 4.49E-02;
    if(mH == 140) xsection = 15.63                   * 3.52E-02;
    if(mH == 145) xsection = 14.59                   * 2.61E-02;
  }
  if(process.find("qqH")!=string::npos){
    if(mH == 110) xsection = 1.809                   * 7.95E-02;
    if(mH == 115) xsection = 1.729                   * 7.58E-02;
    if(mH == 120) xsection = 1.649                   * 7.04E-02;
    if(mH == 125) xsection = 1.578                   * 6.32E-02;
    if(mH == 130) xsection = 1.511                   * 5.45E-02;
    if(mH == 135) xsection = 1.448                   * 4.49E-02;
    if(mH == 140) xsection = 1.389                   * 3.52E-02;
    if(mH == 145) xsection = 1.333                   * 2.61E-02;
  }
  if(process.find("VH")!=string::npos){
    if(mH == 110) xsection = (1.060+0.5869+0.1887)   * 7.95E-02;
    if(mH == 115) xsection = (0.9165+0.5117+0.1663)  * 7.58E-02;
    if(mH == 120) xsection = (0.7966+0.4483+0.1470)  * 7.04E-02;
    if(mH == 125) xsection = (0.6966+0.3943+0.1302)  * 6.32E-02;
    if(mH == 130) xsection = (0.6095+0.3473+0.1157)  * 5.45E-02;
    if(mH == 135) xsection = (0.5351+0.3074+0.1031)  * 4.49E-02;
    if(mH == 140) xsection = (0.4713+0.2728+0.09207) * 3.52E-02;
    if(mH == 145) xsection = (0.4164+0.2424+0.08246) * 2.61E-02;
  }
  return xsection;
}

void produce(   
	     int mH_=125,
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "ABC",
	     TString location  = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_p2_Trees/src/LLRAnalysis/Limits/bin/results/"
	     //TString location  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534_TopUp/src/LLRAnalysis/Limits/bin/results/"
	     )
{
  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ 
       << " analysis " << analysis_ << " and bin " << bin_ << endl;
  TFile* fin = 0;
  string analysisFile = analysis_;

  fin = new TFile(Form(location+"/%s/histograms/eTau_mH%d_%s_%s_%s.root", outputDir.Data(), 125, bin_.c_str() , analysisFile.c_str(), variable_.c_str()), "READ");

  ///////////////////////////////////////////////
  TFile* fin_jUp     = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s_JetUp_%s.root",   outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_jDown   = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s_JetDown_%s.root", outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tUp     = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s_TauUp_%s.root",   outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tDown   = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s_TauDown_%s.root", outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_nominal = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s__%s.root",        outputDir.Data(), 125, bin_.c_str() , variable_.c_str()), "READ");
  ///////////////////////////////////////////////

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

  string theory =  !DOSUSY ? "SM" : "MSSM" ;

  TFile* fTemplOut = new TFile(Form(location+"%s/datacards/eTau%s.root",outputDir.Data(), theory.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "_CMS_scale_t_etau_8TeVUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "_CMS_scale_t_etau_8TeVDown";
  else if(analysis_.find("ElecUp")!=string::npos)
    suffix = "_CMS_scale_eUp";
  else if(analysis_.find("ElecDown")!=string::npos)
    suffix = "_CMS_scale_eDown";
  else if(analysis_.find("JetUp")!=string::npos)
    suffix = "_CMS_scale_jUp";
  else if(analysis_.find("JetDown")!=string::npos)
    suffix = "_CMS_scale_jDown";
  else if(analysis_.find("FakeUp")!=string::npos)
    suffix = "_CMS_scale_fakeUp";
  else if(analysis_.find("FakeDown")!=string::npos)
    suffix = "_CMS_scale_fakeDown";
  
  cout << "Adding histos with suffix " << suffix << endl;
  TString dirName( Form("eleTau_%s",binNameSpace.c_str()) );
  
  if(! (fTemplOut->cd( dirName.Data()  ) ) ){
    cout << "Editing the directory for bin and variable " << binNameSpace << ", " << variable_ << endl;
    TDirectory* dir = fTemplOut->mkdir( dirName.Data() );
    dir->cd();
    
    TH1F* hData = ((TH1F*)fin->Get("hData")); 
    hData->SetName("data_obs");
    hData->Write("data_obs");

    if(!DOSUSY){
      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hggH%d",mH_));
      hSgn2->Scale(1./rescaleggH);
      hSgn2->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("ggH%d%s"   ,mH_,suffix.c_str()));

      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hqqH%d",mH_));
      hSgn1->Scale(1./rescaleqqH);
      hSgn1->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("qqH%d%s"   ,mH_,suffix.c_str()));

      TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
      hSgn3->Scale(1./rescaleVH);
      hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
      // in VBF, take the di-tau mass shape from GGF sample
      if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	float VHyield = hSgn3->Integral();
	hSgn3->Reset();
	hSgn3->Add(hSgn2,1.0);
	hSgn3->Scale(VHyield/hSgn3->Integral());
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      }
      else
	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
    }
    else{//SUSY
      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
      hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
      hSgn1->Write(Form("ggH%d%s"   ,mH_,suffix.c_str()));

      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
      hSgn2->SetName(Form("bbH%d%s" ,mH_,suffix.c_str()));
      hSgn2->Write(Form("bbH%d%s"   ,mH_,suffix.c_str()));
    }

    if(bin_.find("novbf")!=string::npos){
      // ZTT: embedded sample
      TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));

      // ----- QCD ------
      if( bin_.find("High")!=string::npos ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"  ,suffix.c_str()));
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      // ----- W ------
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"        ,suffix.c_str()));
      hW->Write(Form("W%s"          ,suffix.c_str()));
      // ----- Fakes ------
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
      // ----- TTb ------
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
      hTTb->Write(Form("TT%s"       ,suffix.c_str()));
      // ----- VV ------
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"         ,suffix.c_str()));
      hVV->Write(Form("VV%s"         ,suffix.c_str()));
    }//novbf

    else if(bin_.find("boost")!=string::npos){
      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
      // ----- QCD ------ 
      if( bin_.find("High")!=string::npos ) {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
        hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else {
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
        hQCD->SetName(Form("QCD%s"  ,suffix.c_str()));
        hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"        ,suffix.c_str()));
      hW->Write(Form("W%s"          ,suffix.c_str()));
      
      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
      // ----- TTb ------
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
      hTTb->Write(Form("TT%s"       ,suffix.c_str()));
      // ----- VV ------
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"      ,suffix.c_str()));
      hVV->Write(Form("VV%s"        ,suffix.c_str()));
    }//boost

    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
      // ZTT: embedded sample 
      TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
      // ----- QCD ------ 
      TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
      hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
      hQCD->Write(Form("QCD%s"      ,suffix.c_str()));
      // ----- W ------ 
      TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
      hW->SetName(Form("W%s"        ,suffix.c_str()));
      hW->Write(Form("W%s"          ,suffix.c_str()));
      // ----- Fakes ------ 
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
      // ----- TTb ------
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
      hTTb->Write(Form("TT%s"       ,suffix.c_str()));
      // ----- VV ------
      TH1F* hVV = ((TH1F*)fin->Get("hVV"));
      hVV->SetName(Form("VV%s"      ,suffix.c_str()));
      hVV->Write(Form("VV%s"        ,suffix.c_str()));
    }//vbf
    else{//inclusive,
      // ZTT: embedded sample
      TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
      hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
      hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
      // ----- QCD ------
      TH1F *hQCD = ((TH1F*)fin->Get("hQCD"));
      hQCD->SetName(Form("QCD%s"  ,suffix.c_str()));
      hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      // ----- W ------
      TH1F* hW = ((TH1F*)fin->Get("hW"));
      hW->SetName(Form("W%s"        ,suffix.c_str()));
      hW->Write(Form("W%s"          ,suffix.c_str()));
      // ----- Fakes ------
      TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
      hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
      hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
      TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
      hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
      hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
      TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
      hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
      hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
      // ----- TTb ------
      TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
      hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
      hTTb->Write(Form("TT%s"       ,suffix.c_str()));
      // ----- VV ------
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
	TH1F* hSgn2 = (TH1F*)fin->Get(Form("hqqH%d",mH_));
	hSgn2->Scale(1./rescaleqqH);
	hSgn2->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
	hSgn2->Write(Form("qqH%d%s" ,mH_  ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ggH%d%s"         , mH_,suffix.c_str()))==0 ){
	TH1F* hSgn1 = (TH1F*)fin->Get(Form("hggH%d",mH_));
	hSgn1->Scale(1./rescaleggH);
	hSgn1->SetName(Form("ggH%d%s" , mH_,suffix.c_str())); 
	hSgn1->Write(Form("ggH%d%s" , mH_  ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VH%d%s"          , mH_,suffix.c_str()))==0 ){
	TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
	hSgn3->Scale(1./rescaleVH);
	hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
	if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	  TH1F* hSgn2 = (TH1F*)fin->Get(Form("hqqH%d",mH_));
	  float VHyield = hSgn3->Integral();
	  hSgn3->Reset();
	  hSgn3->Add(hSgn2,1.0);
	  hSgn3->Scale(VHyield/hSgn3->Integral());
	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
	}
	else
	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
      }
    }
    else{//SUSY
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
        TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
	hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
        hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
        TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
	hSgn2->SetName(Form("bbH%d%s" , mH_,suffix.c_str()));
        hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
      }
    }

    if(bin_.find("novbf")!=string::npos){
      // ZTT: embedded sample 
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
      }
      // ----- QCD ------ 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("High")!=string::npos ){
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
      }
      // ----- W ------ 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      // ----- Fakes ------ 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      // ----- TTb ------
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      // ----- VV ------
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }//novbf

    else if(bin_.find("boost")!=string::npos){
      // ZTT: embedded sample 
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      // ----- QCD ------ 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	if( bin_.find("High")!=string::npos ) {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else {
	  TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	  //for 1Jet_low, need to scale by 15% below 60 GeV and add a shape systematic for QCD  
	  // if(suffix == ""){ 
// 	    TH1F* hQCDUp=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_etau_boost_low_8TeVUp"); 
// 	    TH1F* hQCDDown=(TH1F*)hQCD->Clone("QCD_CMS_htt_QCDShape_etau_boost_low_8TeVDown"); 
// 	    for(Int_t b=1;b<=hQCD->GetNbinsX();b++){ 
// 	      if(hQCD->GetBinCenter(b)<=60.){ 
// 		hQCDUp->SetBinContent(b,1.15*hQCDUp->GetBinContent(b)); 
// 		hQCDDown->SetBinContent(b,0.85*hQCDDown->GetBinContent(b)); 
// 	      } 
// 	    } 
// 	    hQCDUp->Write("QCD_CMS_htt_QCDShape_etau_boost_low_8TeVUp"); 
// 	    hQCDDown->Write("QCD_CMS_htt_QCDShape_etau_boost_low_8TeVDown");
// 	  }
	  //suffix	  
	}//low
      }
      // ----- W ------ 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      // ----- Fakes ------ 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      // ----- TTb ------
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      // ----- VV ------
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }//boost

    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
      // ZTT: embedded sample 
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
      }
      // ----- QCD ------ 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	TH1F *hQCD = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      // ----- W ------ 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      // ----- Fakes ------ 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      // ----- TTb ------
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      // ----- VV ------
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }//vbf

    else{//inclusive
      // ZTT: embedded sample 
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
	TH1F* hDataEmb = ((TH1F*)fin->Get("hDataEmb"));
	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
	hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
      }
      // ----- QCD ------ 
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	TH1F *hQCD = ((TH1F*)fin->Get("hQCD"));
	hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      // ----- W ------ 
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	TH1F* hW = ((TH1F*)fin->Get("hW"));
	hW->SetName(Form("W%s"           ,suffix.c_str()));
	hW->Write(Form("W%s"           ,suffix.c_str()));
      }
      // ----- Fakes ------ 
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
	TH1F* hZmj = ((TH1F*)fin->Get("hZmj"));
	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZmm = ((TH1F*)fin->Get("hZmm"));
	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) {
	TH1F* hZfakes = ((TH1F*)fin->Get("hZfakes"));
	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
      }
      // ----- TTb ------
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
	TH1F* hTTb = ((TH1F*)fin->Get("hTTb"));
	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
      }
      // ----- VV ------
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
	TH1F* hVV = ((TH1F*)fin->Get("hVV"));
	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
	hVV->Write(Form("VV%s"         ,suffix.c_str()));
      }
    }
  }//directory already there

  fTemplOut->Close();
  
  // edit the datacards only for the nominal analysis
  if(analysis_.find("Up")!=string::npos || analysis_.find("Down")!=string::npos){
    fin->Close();          delete fin;
    fin_tUp->Close();      delete fin_tUp;
    fin_tDown->Close();    delete fin_tDown;
    fin_jUp->Close();      delete fin_jUp;
    fin_jDown->Close();    delete fin_jDown;
    fin_nominal->Close();  delete fin_nominal;
    return;
  }

  //////////////////////////////////
  //////////////////////////////////
  //////////////////////////////////
  //////////////////////////////////
  // ----- EDIT DATACARDS ------ 
  //////////////////////////////////
  //////////////////////////////////
  //////////////////////////////////
  //////////////////////////////////

  ifstream in;

  char* c = new char[1000];
  in.open(Form(location+"templates/eTau_%s_template_v3.txt",binNameSpace.c_str()));
  string resonance = !DOSUSY ? "H" : "A";

  ofstream out(Form(location+"%s/datacards/eTau_%s_m%s%d.txt", outputDir.Data(), binNameSpace.c_str(), resonance.c_str(), mH_ ));
  out.precision(8);

  float QCDyield = 0;
  float Wyield   = 0.;

  while (in.good())
    {
      in.getline(c,1000,'\n');
      if (in.good()){
	string line(c);
	if(line.find("observation")!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f", ((TH1F*)fin->Get("hData"))->Integral()) )  );
	  out << line << endl;
	}
	else if(line.find("shapes")!=string::npos){
	  if(!DOSUSY) 
	    line.replace( line.find("XXX") , 3 , string("eTauSM")  );
	  else 
	    line.replace( line.find("XXX") , 3 , string("eTauMSSM")  );
	  out << line << endl;
	}
	else if(line.find("process")!=string::npos && line.find("ggH")!=string::npos){

	  if(!DOSUSY){
	    if(!RESCALETO1PB){
	      line.replace( line.find("XXX") , 3 , string(Form("%d",mH_))  );
	      line.replace( line.find("YYY") , 3 , string(Form("%d",mH_))  );
	      line.replace( line.find("KKK") , 3 , string(Form("%d",mH_))  );
	    }
	    else{
	      line.replace( line.find("XXX") , 3 , string(Form("   "))  );
	      line.replace( line.find("YYY") , 3 , string(Form("   "))  );
	      line.replace( line.find("KKK") , 3 , string(Form("   "))  );
	    }
	  }
	  else{
	    line.replace( line.find("YYY") , 3 , string(Form("   "))  );
	    line.replace( line.find("KKK") , 3 , string(Form("   "))  );
	  }
          out << line << endl;
        }
	else if(line.find("rate")!=string::npos){
	  // ----- QCD ------ 
	  if((bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) ||  bin_.find("High")!=string::npos  ){
	    QCDyield = ((TH1F*)fin->Get("hDataAntiIsoLooseTauIsoQCD"))->Integral();
	  }//vbf or 0jetHigh or boostHigh
	  else{
	    QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 
	  }//inclusive or 0jetLow or boostLow
	  // ----- W ------ 
	  if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
	    Wyield = ((TH1F*)fin->Get("hW3JetsMediumTauIsoRelVBF"))->Integral();
	  }
	  else{
	    Wyield = ((TH1F*)fin->Get("hW"))->Integral();
	  }

	  /////////////////////////////////////////////
	  /////////////////////////////////////////////
	  
	  TH1F* hSgn2 = !DOSUSY ? (TH1F*)fin->Get(Form("hggH%d",mH_)) : (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
	  //hSgn2->Scale(1./rescaleggH);
	  
	  TH1F* hSgn1 = !DOSUSY ? (TH1F*)fin->Get(Form("hqqH%d",mH_)) : (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
	  //hSgn1->Scale(1./rescaleqqH);
	  
	  TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
	  //hSgn3->Scale(1./rescaleVH);

	  // ----- Rates ------
	  string rate = "rate                                           ";
	  string space = "              ";
	  out << rate ;
	  if(!DOSUSY) 
	    out <<        hSgn3->Integral();
	  out << space << hSgn2->Integral()
	      << space << hSgn1->Integral()
	      << space << ((TH1F*)fin->Get("hDataEmb"))->Integral()
	      << space << QCDyield
	      << space << Wyield;
	  if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("boost")!=string::npos){
	    out << space << ((TH1F*)fin->Get("hZmj"))->Integral()
		<< space << ((TH1F*)fin->Get("hZmm"))->Integral();
	  }
	  else{//vbf
	    out << space << ((TH1F*)fin->Get("hZfakes"))->Integral();
	  }
	  out << space << ((TH1F*)fin->Get("hTTb"))->Integral()
	      << space << ((TH1F*)fin->Get("hVV"))->Integral()
	      << endl;
	}
	// ----- Extrapolation factor ------
	else if(line.find("CMS_htt_ztt_extrap")!=string::npos){
	  float extrapFactor = ((TH1F*)fin->Get("hParameters"))->GetBinContent(8);
	  float extrapError  = ((TH1F*)fin->Get("hParameters"))->GetBinContent(9);
	  float relUnc = 1.+extrapError/extrapFactor;
	  line.replace( line.find("XXX") , 3 , string(Form("%.5f",relUnc))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.5f",relUnc))  );
	  if( line.find("KKK") != string::npos )
	    line.replace( line.find("KKK") , 3 , string(Form("%.5f",relUnc))  );
	  out << line << endl;
	}
	// ----- Jet energy scale ------
	else if(line.find("CMS_scale_j")!=string::npos){
	  float VBFrel = 0; float VBFrelUp = 0;  float VBFrelDown = 0;
	  float SMrel  = 0; float SMrelUp  = 0;  float SMrelDown  = 0;
	  float VHrel  = 0; float VHrelUp  = 0;  float VHrelDown  = 0;
	  if(!DOSUSY){

	    //////////////////////////////// FIX HERE!!!
	    VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral()))
				);
	    if( ((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral()>0 ){
	      VBFrelUp   = (((TH1F*)fin_jUp->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral());
	      VBFrelDown = (((TH1F*)fin_jDown->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral());
	    }
	    else{
	      VBFrelUp   = 1.1; // conservative
	      VBFrelDown = 1.1; // conservative
	    }
	    
	    VBFrel     *= 1./rescaleqqH;
	    VBFrelUp   *= 1./rescaleqqH;
	    VBFrelDown *= 1./rescaleqqH;

	    SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral()))
				);
	    if( ((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral()>0 ){
	      SMrelUp   = (((TH1F*)fin_jUp->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral());
	      SMrelDown = (((TH1F*)fin_jDown->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral());
	    }
	    else{
	      SMrelUp   = 1.1; // conservative
	      SMrelDown = 1.1; // conservative
	    }

	    SMrel     *= 1./rescaleggH;
	    SMrelUp   *= 1./rescaleggH;
	    SMrelDown *= 1./rescaleggH;

	    VHrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral()))
				);
	    
	    if( ((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral()>0 ){
	      VHrelUp   = (((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());
	      VHrelDown = (((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());
	    }
	    else{
	      VHrelUp   = 1.1; // conservative
	      VHrelDown = 1.1; // conservative
	    }


	    VHrel     *= 1./rescaleVH;
	    VHrelUp   *= 1./rescaleVH;
	    VHrelDown *= 1./rescaleVH;

	  }
	  else{//SUSY
	    VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral())),
                                TMath::Abs((((TH1F*)fin_jDown->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral()))
                                );
            VBFrelUp   = (((TH1F*)fin_jUp->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral());
            VBFrelDown = (((TH1F*)fin_jDown->Get(Form("hSUSYGG%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYGG%d",mH_)))->Integral());

            SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral())),
                                TMath::Abs((((TH1F*)fin_jDown->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral()))
                                );

            SMrelUp   = (((TH1F*)fin_jUp->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral());
            SMrelDown = (((TH1F*)fin_jDown->Get(Form("hSUSYBB%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hSUSYBB%d",mH_)))->Integral());
	  }
	  float ZTTrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral()))
				    );
	  float ZTTrelUp   = (((TH1F*)fin_jUp->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral());
	  float ZTTrelDown = (((TH1F*)fin_jDown->Get("hZtt"))->Integral()/((TH1F*)fin->Get("hZtt"))->Integral());
	  float TTrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral()))
				    );
	  float TTrelUp   = (((TH1F*)fin_jUp->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral());
	  float TTrelDown = (((TH1F*)fin_jDown->Get("hTTb"))->Integral()/((TH1F*)fin->Get("hTTb"))->Integral());


	  float VVrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral())),
				    TMath::Abs((((TH1F*)fin_jDown->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral()))
				    );
	  float VVrelUp   = (((TH1F*)fin_jUp->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral());
	  float VVrelDown = (((TH1F*)fin_jDown->Get("hVV"))->Integral()/((TH1F*)fin->Get("hVV"))->Integral());
	  string space      = "                   ";
	  string longspace  = "                      ";
	  string shortspace = "     ";
	  out << "CMS_scale_j";
	  out << longspace << "lnN" << shortspace;
	  if(!DOSUSY){
	    if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("2jet")!=string::npos) 
	      out <<          string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+VHrelDown), TMath::Abs(-1+VHrelUp) ) * TMath::Abs(-1+VHrelUp)/(-1+VHrelUp)   )); // VH
	    else
	      out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+SMrelDown), TMath::Abs(-1+SMrelUp))  * TMath::Abs(-1+SMrelUp)/(-1+SMrelUp)   )); // VH=GGF
	  }
	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+SMrelDown), TMath::Abs(-1+SMrelUp))  * TMath::Abs(-1+SMrelUp)/(-1+SMrelUp)   ));  // GGF/BB
	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+VBFrelDown),TMath::Abs(-1+VBFrelUp))* TMath::Abs(-1+VBFrelUp)/(-1+VBFrelUp) ));   // VBF/GG
	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // ZTT
	  out << space << "-" ;                                                // QCD
	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // W
	  out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));  // Z+fakes

	  if(binNameSpace.find("0jet_")!=string::npos || binNameSpace.find("boost")!=string::npos) 
	    out << space << "-" ; //string(Form("%.2f",1+(-1+ZTTrelDown)*1.0));

	  out << space << string(Form("%.2f",1+TMath::Max(  TMath::Abs(-1+TTrelDown),TMath::Abs(-1+TTrelUp))  * TMath::Abs(-1+TTrelUp)/(-1+TTrelUp) ));  // TT

	  //if(binNameSpace.find("0jet_")!=string::npos) 
	  out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+VVrelDown),TMath::Abs(-1+VVrelUp) ) * TMath::Abs(-1+VVrelUp)/(-1+VVrelUp)));   // VV
	  //else
	  //out << space << string(Form("%.2f",1+TMath::Max(TMath::Abs(-1+TTrelDown),TMath::Abs(-1+TTrelUp))  * TMath::Abs(-1+TTrelUp)/(-1+TTrelUp) ));  // TT

	  out << space << "JEC uncertainty";
	  out << endl;
	}
	// ----- QCD normalization ------
	else if(line.find( Form("CMS_htt_eTau_%s_QCDNorm",binNameSpace.c_str()) )!=string::npos){
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(5)))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(6)))  );
	  out << line << endl;
	}
	// ----- W normalization ------
	else if(line.find( Form("CMS_htt_eTau_%s_WNorm",binNameSpace.c_str()) )!=string::npos){
	  // in VBF use extrapolation factor from W+3jets
	  if(binNameSpace.find("vbf")!=string::npos){
	    float DataVBFSdb  = ((TH1F*)fin->Get("hParameters"))->GetBinContent(14);
	    float TTbarVBFSdb = ((TH1F*)fin->Get("hParameters"))->GetBinContent(15);
	    float QCDVBFSdb   = ((TH1F*)fin->Get("hParameters"))->GetBinContent(16);
	    float ExtrapFactor = Wyield/(DataVBFSdb+TTbarVBFSdb+QCDVBFSdb);
	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",  (DataVBFSdb+TTbarVBFSdb+QCDVBFSdb))));
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",  ExtrapFactor)));
	  }//vbf
	  else{
	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(2)))  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(1)))  );
	  }
	  out << line << endl;
	}
	else{
	  out << c << endl;
	}
      }// if in.good
    }//while in.good

  fin->Close();          delete fin;
  fin_tUp->Close();      delete fin_tUp;
  fin_tDown->Close();    delete fin_tDown;
  fin_jUp->Close();      delete fin_jUp;
  fin_jDown->Close();    delete fin_jDown;

  return;
}//produce


// void produce2DFit(   
// 	     int mH_=125,
// 	     string variable_  = "diTauVisMass",
// 	     string analysis_  = "",
// 	     string bin_       = "inclusive",
// 	     TString outputDir = "2DFit",
// 	     //TString location  = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_Oct12/src/LLRAnalysis/Limits/bin/results/"
// 	     TString location  = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534_TopUp/src/LLRAnalysis/Limits/bin/results/"	     
// 	     )
// {
//   cout << "2DFit :" <<endl; 
//   cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ 
//        << " analysis " << analysis_ << " and bin " << bin_ << endl;
//   TFile* fin = 0;
//   string analysisFile = analysis_;

//   fin = new TFile(Form(location+"%s/histograms/eTau_mH%d_%s_%s_%s.root", outputDir.Data(), mH_, bin_.c_str() , analysisFile.c_str(), variable_.c_str()), "READ");

//   float rescaleggH = RESCALETO1PB ? higgsXsection(mH_,"ggH") : 1.0;
//   float rescaleqqH = RESCALETO1PB ? higgsXsection(mH_,"qqH") : 1.0;
//   float rescaleVH  = RESCALETO1PB ? higgsXsection(mH_,"VH")  : 1.0;

//   string binNameSpace = "";
//   if(bin_.find("inclusive")!=string::npos)      
//     binNameSpace =  "inclusive";

//   else if(bin_.find("novbf")!=string::npos) 
//     binNameSpace =  "SM0";

//   if(bin_.find("novbfLow")  !=string::npos) 
//     binNameSpace =  "0jet_low";

//   if(bin_.find("novbfHigh") !=string::npos) 
//     binNameSpace =  "0jet_high";

//   else if(bin_.find("boost")!=string::npos) 
//     binNameSpace =  "SM1";

//   if(bin_.find("boostLow")!=string::npos) 
//     binNameSpace =  "boost_low";

//   if(bin_.find("boostHigh")!=string::npos) 
//     binNameSpace =  "boost_high";

//   else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) 
//     binNameSpace =  "vbf";

//   string theory =  !DOSUSY ? "SM" : "MSSM" ;

//   TFile* fTemplOut = new TFile(Form(location+"datacards/%s/eTau2DFit%s.root",outputDir.Data(), theory.c_str()),"UPDATE");
  
//   string suffix = "";
//   if(analysis_.find("TauUp")!=string::npos)
//     suffix = "_CMS_scale_t_etau_8TeVUp";
//   else if(analysis_.find("TauDown")!=string::npos)
//     suffix = "_CMS_scale_t_etau_8TeVDown";
//   else if(analysis_.find("ElecUp")!=string::npos)
//     suffix = "_CMS_scale_eUp";
//   else if(analysis_.find("ElecDown")!=string::npos)
//     suffix = "_CMS_scale_eDown";
//   else if(analysis_.find("JetUp")!=string::npos)
//     suffix = "_CMS_scale_jUp";
//   else if(analysis_.find("JetDown")!=string::npos)
//     suffix = "_CMS_scale_jDown";
//   else if(analysis_.find("FakeUp")!=string::npos)
//     suffix = "_CMS_scale_fakeUp";
//   else if(analysis_.find("FakeDown")!=string::npos)
//     suffix = "_CMS_scale_fakeDown";
  
//   cout << "========== Defining mapping for 1D histos from 2D histos =========="<< endl;
//   cout << "For Signal "<< endl;
//   TH2F *hSgn ;
//   if(!DOSUSY){
//     TH2F* hSgn12D     =  (TH2F*)fin->Get(Form("hggH%d2D",mH_));
//     TH2F* hSgn22D     =  (TH2F*)fin->Get(Form("hqqH%d2D",mH_));
//     TH2F* hSgn32D     =  (TH2F*)fin->Get(Form("hVH%d2D",mH_));
//     hSgn = (TH2F*)hSgn12D->Clone("hSgn");
//     hSgn->Add(hSgn22D);
//     hSgn->Add(hSgn32D);
//   }
//   else{//SUSY
//     TH2F* hSgn12D     =  (TH2F*)fin->Get(Form("hSUSYGGH%d2D",mH_));
//     TH2F* hSgn22D     =  (TH2F*)fin->Get(Form("hSUSYBBH%d2D",mH_));
//     hSgn = (TH2F*)hSgn12D->Clone("hSgn");
//     hSgn->Add(hSgn22D);
//   }
//   cout << "For Background "<< endl;

//   TH2F* hDataEmb2D  =  (TH2F*)fin->Get("hDataEmb2D");
//   TH2F* hQCD2D ;
//   if((bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos) ||  bin_.find("High")!=string::npos  ){
//     hQCD2D = (TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D");
//   }//vbf or 0jetHigh or boostHigh
//   else{
//     hQCD2D = (TH2F*)fin->Get("hQCD2D");
//   }//inclusive or 0jetLow or boostLow
//   TH2F* hW2D;
//   if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
//     hW2D = (TH2F*)fin->Get("hW3JetsMediumTauIsoRelVBF2D");
//   }
//   else{
//     hW2D = (TH2F*)fin->Get("hW2D");
//   }
//   TH2F* hZmj2D      =  (TH2F*)fin->Get("hZmj2D");
//   TH2F* hZmm2D      =  (TH2F*)fin->Get("hZmm2D");
//   TH2F* hTTb2D      =  (TH2F*)fin->Get("hTTb2D");
//   TH2F* hVV2D       =  (TH2F*)fin->Get("hVV2D");

//   TH2F* hBkg = (TH2F*)hDataEmb2D->Clone("hBgk");
//   hBkg->Add(hQCD2D);
//   hBkg->Add(hW2D);
//   hBkg->Add(hZmj2D);
//   hBkg->Add(hZmm2D);
//   hBkg->Add(hTTb2D);
//   hBkg->Add(hVV2D);

//   TH2DynamicRebinner th2Rebinner(hSgn, hBkg);
//   th2Rebinner.print(std::cout);
//   cout << "=========End defining mapping for 1D histos from 2D histos ========== "<< endl;

//   cout << "Adding histos with suffix " << suffix << endl;
//   TString dirName( Form("eTau_%s",binNameSpace.c_str()) );
  
//   if(! (fTemplOut->cd( dirName.Data()  ) ) ){
//     cout << "Editing the directory for bin and variable " << binNameSpace << ", " << variable_ << endl;
//     TDirectory* dir = fTemplOut->mkdir( dirName.Data() );
//     dir->cd();
    
//     TH1* hData = th2Rebinner((TH2F*)fin->Get("hData2D")); 
//     hData->SetName("data_obs");
//     hData->Write("data_obs");

//     if(!DOSUSY){
//       TH1* hSgn2 = th2Rebinner((TH2F*)fin->Get(Form("hggH%d2D",mH_)));
//       hSgn2->Scale(1./rescaleggH);
//       hSgn2->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
//       hSgn2->Write(Form("ggH%d%s"   ,mH_,suffix.c_str()));

//       TH1* hSgn1 = th2Rebinner((TH2F*)fin->Get(Form("hqqH%d2D",mH_)));
//       hSgn1->Scale(1./rescaleqqH);
//       hSgn1->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
//       hSgn1->Write(Form("qqH%d%s"   ,mH_,suffix.c_str()));

//       TH1* hSgn3 = th2Rebinner((TH2F*)fin->Get(Form("hVH%d2D",mH_)));
//       hSgn3->Scale(1./rescaleVH);
//       hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
//       // in VBF, take the di-tau mass shape from GGF sample
//       if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
// 	float VHyield = hSgn3->Integral();
// 	hSgn3->Reset();
// 	hSgn3->Add(hSgn2,1.0);
// 	hSgn3->Scale(VHyield/hSgn3->Integral());
// 	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
//       }
//       else
// 	hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
//     }
//     else{//SUSY
//       TH1* hSgn1 = th2Rebinner((TH2F*)fin->Get(Form("hSUSYGG%d2D",mH_)));
//       hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
//       hSgn1->Write(Form("ggH%d%s"   ,mH_,suffix.c_str()));

//       TH1* hSgn2 = th2Rebinner((TH2F*)fin->Get(Form("hSUSYBB%d2D",mH_)));
//       hSgn2->SetName(Form("bbH%d%s" ,mH_,suffix.c_str()));
//       hSgn2->Write(Form("bbH%d%s"   ,mH_,suffix.c_str()));
//     }

//     if(bin_.find("novbf")!=string::npos){
//       // ZTT: embedded sample
//       TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
//       hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
//       hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));

//       // ----- QCD ------
//       if( bin_.find("High")!=string::npos ){
// 	TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
// 	hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
// 	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
//       }
//       else {
// 	TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hQCD2D"));
// 	hQCD->SetName(Form("QCD%s"  ,suffix.c_str()));
// 	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
//       }
//       // ----- W ------
//       TH1* hW = th2Rebinner((TH2F*)fin->Get("hW2D"));
//       hW->SetName(Form("W%s"        ,suffix.c_str()));
//       hW->Write(Form("W%s"          ,suffix.c_str()));
//       // ----- Fakes ------
//       TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
//       hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
//       hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
//       TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
//       hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
//       hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
// //       TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// //       hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
// //       hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
//       // ----- TTb ------
//       TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
//       hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
//       hTTb->Write(Form("TT%s"       ,suffix.c_str()));
//       // ----- VV ------
//       TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
//       hVV->SetName(Form("VV%s"         ,suffix.c_str()));
//       hVV->Write(Form("VV%s"         ,suffix.c_str()));
//     }//novbf

//     else if(bin_.find("boost")!=string::npos){
//       // ZTT: embedded sample 
//       TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
//       hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
//       hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
//       // ----- QCD ------ 
//       if( bin_.find("High")!=string::npos ) {
// 	TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
//         hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
//         hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
//       }
//       else {
// 	TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hQCD2D"));
//         hQCD->SetName(Form("QCD%s"  ,suffix.c_str()));
//         hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
//       }
//       // ----- W ------ 
//       TH1* hW = th2Rebinner((TH2F*)fin->Get("hW2D"));
//       hW->SetName(Form("W%s"        ,suffix.c_str()));
//       hW->Write(Form("W%s"          ,suffix.c_str()));
      
//       // ----- Fakes ------ 
//       TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
//       hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
//       hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
//       TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
//       hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
//       hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
// //       TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// //       hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
// //       hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
//       // ----- TTb ------
//       TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
//       hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
//       hTTb->Write(Form("TT%s"       ,suffix.c_str()));
//       // ----- VV ------
//       TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
//       hVV->SetName(Form("VV%s"      ,suffix.c_str()));
//       hVV->Write(Form("VV%s"        ,suffix.c_str()));
//     }//boost

//     else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
//       // ZTT: embedded sample 
//       TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
//       hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
//       hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
//       // ----- QCD ------ 
//       TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
//       hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
//       hQCD->Write(Form("QCD%s"      ,suffix.c_str()));
//       // ----- W ------ 
//       TH1* hW = th2Rebinner((TH2F*)fin->Get("hW3JetsMediumTauIsoRelVBF2D"));
//       hW->SetName(Form("W%s"        ,suffix.c_str()));
//       hW->Write(Form("W%s"          ,suffix.c_str()));
//       // ----- Fakes ------ 
//       TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
//       hZmj->SetName(Form("ZJ%s"     ,suffix.c_str()));
//       hZmj->Write(Form("ZJ%s"       ,suffix.c_str()));
//       TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
//       hZmm->SetName(Form("ZL%s"     ,suffix.c_str()));
//       hZmm->Write(Form("ZL%s"       ,suffix.c_str()));
// //       TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// //       hZfakes->SetName(Form("ZLL%s" ,suffix.c_str())); 
// //       hZfakes->Write(Form("ZLL%s"   ,suffix.c_str()));
//       // ----- TTb ------
//       TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
//       hTTb->SetName(Form("TT%s"     ,suffix.c_str()));
//       hTTb->Write(Form("TT%s"       ,suffix.c_str()));
//       // ----- VV ------
//       TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
//       hVV->SetName(Form("VV%s"      ,suffix.c_str()));
//       hVV->Write(Form("VV%s"        ,suffix.c_str()));
//     }//vbf
//     else{//inclusive,
//       th2Rebinner((TH2F*)fin->Get("hDataEmb2D"))->Write(Form("ZTT%s"  ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hQCD2D"))->Write(Form("QCD%s"      ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hW2D"))->Write(Form("W%s"          ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hZmj2D"))->Write(Form("ZJ%s"       ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hZmm2D"))->Write(Form("ZL%s"       ,suffix.c_str()));
// //       th2Rebinner((TH2F*)fin->Get("hZfakes2D"))->Write(Form("ZLL%s"   ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hTTb2D"))->Write(Form("TT%s"       ,suffix.c_str()));
//       th2Rebinner((TH2F*)fin->Get("hVV2D"))->Write(Form("VV%s"        ,suffix.c_str()));
//     }
           
//   }else{
//     cout << "Directory is there, filling new histos..." << endl;
//     TDirectory* dir = (TDirectory*)fTemplOut->Get( dirName.Data() );
//     fTemplOut->cd( dirName.Data() );
    
//     if(!DOSUSY){
//       if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
// 	TH1* hSgn2 = th2Rebinner((TH2F*)fin->Get(Form("hqqH%d2D",mH_)));
// 	hSgn2->Scale(1./rescaleqqH);
// 	hSgn2->SetName(Form("qqH%d%s" ,mH_,suffix.c_str()));
// 	hSgn2->Write(Form("qqH%d%s" ,mH_  ,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("ggH%d%s"         , mH_,suffix.c_str()))==0 ){
// 	TH1* hSgn1 = th2Rebinner((TH2F*)fin->Get(Form("hggH%d2D",mH_)));
// 	hSgn1->Scale(1./rescaleggH);
// 	hSgn1->SetName(Form("ggH%d%s" , mH_,suffix.c_str())); 
// 	hSgn1->Write(Form("ggH%d%s" , mH_  ,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("VH%d%s"          , mH_,suffix.c_str()))==0 ){
// 	TH1* hSgn3 = th2Rebinner((TH2F*)fin->Get(Form("hVH%d2D",mH_)));
// 	hSgn3->Scale(1./rescaleVH);
// 	hSgn3->SetName(Form("VH%d%s" ,mH_,suffix.c_str()));
// 	if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
// 	  TH1* hSgn2 = th2Rebinner((TH2F*)fin->Get(Form("hqqH%d2D",mH_)));
// 	  float VHyield = hSgn3->Integral();
// 	  hSgn3->Reset();
// 	  hSgn3->Add(hSgn2,1.0);
// 	  hSgn3->Scale(VHyield/hSgn3->Integral());
// 	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
// 	}
// 	else
// 	  hSgn3->Write(Form("VH%d%s" ,mH_,suffix.c_str()));
//       }
//     }
//     else{//SUSY
//       if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
//         TH1* hSgn1 = th2Rebinner((TH2F*)fin->Get(Form("hSUSYGG%d2D",mH_)));
// 	hSgn1->SetName(Form("ggH%d%s" ,mH_,suffix.c_str()));
//         hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
//         TH1* hSgn2 = th2Rebinner((TH2F*)fin->Get(Form("hSUSYBB%d2D",mH_)));
// 	hSgn2->SetName(Form("bbH%d%s" , mH_,suffix.c_str()));
//         hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
//       }
//     }

//     if(bin_.find("novbf")!=string::npos){
//       // ZTT: embedded sample 
//       if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
// 	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
// 	hDataEmb->Write(Form("ZTT%s"  ,suffix.c_str()));
//       }
//       // ----- QCD ------ 
//       if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
// 	if( bin_.find("High")!=string::npos ){
// 	  TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
// 	  hQCD->SetName(Form("QCD%s"  ,suffix.c_str())); 
// 	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
// 	}
// 	else {
// 	  TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hQCD2D"));
// 	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
// 	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
// 	}
//       }
//       // ----- W ------ 
//       if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
// 	TH1* hW = th2Rebinner((TH2F*)fin->Get("hW2D"));
// 	hW->SetName(Form("W%s"           ,suffix.c_str()));
// 	hW->Write(Form("W%s"           ,suffix.c_str()));
//       }
//       // ----- Fakes ------ 
//       if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
// 	TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
// 	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
// 	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) {
// 	TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
// 	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
// 	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
//       }
// //       if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) {
// // 	TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// // 	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
// // 	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
// //       }
//       // ----- TTb ------
//       if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
// 	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
// 	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
//       }
//       // ----- VV ------
//       if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
// 	TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
// 	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
// 	hVV->Write(Form("VV%s"         ,suffix.c_str()));
//       }
//     }//novbf

//     else if(bin_.find("boost")!=string::npos){
//       // ZTT: embedded sample 
//       if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
// 	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
// 	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
//       }
//       // ----- QCD ------ 
//       if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
// 	if( bin_.find("High")!=string::npos ) {
// 	  TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
// 	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
// 	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
// 	}
// 	else {
// 	  TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hQCD2D"));
// 	  hQCD->SetName(Form("QCD%s"    ,suffix.c_str()));
// 	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
// 	}
//       }
//       // ----- W ------ 
//       if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
// 	TH1* hW = th2Rebinner((TH2F*)fin->Get("hW2D"));
// 	hW->SetName(Form("W%s"           ,suffix.c_str()));
// 	hW->Write(Form("W%s"           ,suffix.c_str()));
//       }
//       // ----- Fakes ------ 
//       if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
// 	TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
// 	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
// 	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
// 	TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
// 	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
// 	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
//       }
// //       if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
// // 	TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// // 	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
// // 	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
// //       }
//       // ----- TTb ------
//       if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
//        	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
// 	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
//       }
//       // ----- VV ------
//       if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
// 	TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
// 	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
// 	hVV->Write(Form("VV%s"         ,suffix.c_str()));
//       }
//     }//boost

//     else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){
//       // ZTT: embedded sample 
//       if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hDataEmb = th2Rebinner((TH2F*)fin->Get("hDataEmb2D"));
// 	hDataEmb->SetName(Form("ZTT%s",suffix.c_str())); 
// 	hDataEmb->Write(Form("ZTT%s",suffix.c_str()));
//       }
//       // ----- QCD ------ 
//       if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
// 	TH1* hQCD = th2Rebinner((TH2F*)fin->Get("hDataAntiIsoLooseTauIsoQCD2D"));
// 	hQCD->SetName(Form("QCD%s"    ,suffix.c_str())); 
// 	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
//       }
//       // ----- W ------ 
//       if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
// 	TH1* hW = th2Rebinner((TH2F*)fin->Get("hW3JetsMediumTauIsoRelVBF2D"));
// 	hW->SetName(Form("W%s"           ,suffix.c_str()));
// 	hW->Write(Form("W%s"           ,suffix.c_str()));
//       }
//       // ----- Fakes ------ 
//       if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 ){
// 	TH1* hZmj = th2Rebinner((TH2F*)fin->Get("hZmj2D"));
// 	hZmj->SetName(Form("ZJ%s"        ,suffix.c_str()));
// 	hZmj->Write(Form("ZJ%s"        ,suffix.c_str()));
//       }
//       if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ){
// 	TH1* hZmm = th2Rebinner((TH2F*)fin->Get("hZmm2D"));
// 	hZmm->SetName(Form("ZL%s"        ,suffix.c_str()));
// 	hZmm->Write(Form("ZL%s"        ,suffix.c_str()));
//       }
// //       if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
// // 	TH1* hZfakes = th2Rebinner((TH2F*)fin->Get("hZfakes2D"));
// // 	hZfakes->SetName(Form("ZLL%s"    ,suffix.c_str())); 
// // 	hZfakes->Write(Form("ZLL%s"    ,suffix.c_str()));
// //       }
//       // ----- TTb ------
//       if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 ){
// 	TH1* hTTb = th2Rebinner((TH2F*)fin->Get("hTTb2D"));
// 	hTTb->SetName(Form("TT%s"        ,suffix.c_str()));
// 	hTTb->Write(Form("TT%s"        ,suffix.c_str()));
//       }
//       // ----- VV ------
//       if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 ){
// 	TH1* hVV = th2Rebinner((TH2F*)fin->Get("hVV2D"));
// 	hVV->SetName(Form("VV%s"         ,suffix.c_str()));
// 	hVV->Write(Form("VV%s"         ,suffix.c_str()));
//       }
//     }//vbf

//     else{//inclusive
//       if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hDataEmb2D"))->Write(Form("ZTT%s"       ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hQCD2D"))->Write(Form("QCD%s"       ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hW2D"))->Write(Form("W%s"           ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hZmj2D"))->Write(Form("ZJ%s"        ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
// 	th2Rebinner((TH2F*)fin->Get("hZmm2D"))->Write(Form("ZL%s"        ,suffix.c_str()));
// //       if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) 
// // 	th2Rebinner((TH2F*)fin->Get("hZfakes2D"))->Write(Form("ZLL%s"        ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hTTb2D"))->Write(Form("TT%s"        ,suffix.c_str()));
//       if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
// 	th2Rebinner((TH2F*)fin->Get("hVV2D"))->Write(Form("VV%s"         ,suffix.c_str()));
//     }
//   }//directory already there

//   fTemplOut->Close();
//   fin->Close();          delete fin;
  
//   return;
// }//produce2DFit




void produceAll(  TString outputDir = "ABCD" ){

  vector<string> variables;
  vector<int> mH;

  //variables.push_back("diTauVisMass");
  variables.push_back("diTauNSVfitMass");

  mH.push_back(110);
  mH.push_back(115);
  mH.push_back(120);
  mH.push_back(125);
  mH.push_back(130);
  mH.push_back(135);
  mH.push_back(140);
  mH.push_back(145);

  //mH.push_back(90);
  //mH.push_back(100);
  //mH.push_back(120);
  //mH.push_back(130);
  //mH.push_back(140);
  //mH.push_back(160);
  //mH.push_back(180);
  //mH.push_back(200);
  //mH.push_back(250);
  //mH.push_back(300);
  //mH.push_back(350);
  //mH.push_back(400);
  //mH.push_back(450);


  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){
//       if(!DO2DFIT){
       produce(mH[j],variables[i], ""        , "inclusiveMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "inclusiveMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "inclusiveMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "inclusiveMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "inclusiveMoriond", outputDir);

       produce(mH[j],variables[i], ""        , "novbfLowMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "novbfLowMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "novbfLowMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "novbfLowMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "novbfLowMoriond", outputDir);

       produce(mH[j],variables[i], ""        , "novbfHighMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "novbfHighMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "novbfHighMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "novbfHighMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "novbfHighMoriond", outputDir);

       produce(mH[j],variables[i], ""        , "boostLowMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "boostLowMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "boostLowMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "boostLowMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "boostLowMoriond", outputDir);
 
       produce(mH[j],variables[i], ""        , "boostHighMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "boostHighMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "boostHighMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "boostHighMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "boostHighMoriond", outputDir);

       produce(mH[j],variables[i], ""        , "vbfMoriond", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "vbfMoriond", outputDir);
       produce(mH[j],variables[i], "TauDown" , "vbfMoriond", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "vbfMoriond", outputDir);
       produce(mH[j],variables[i], "JetDown" , "vbfMoriond", outputDir);
//       }//!DO2DFIT
//       else{
//        produce2DFit(mH[j],variables[i], ""        , "inclusiveMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "inclusiveMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "inclusiveMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "inclusiveMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "inclusiveMoriond", outputDir);

//        produce2DFit(mH[j],variables[i], ""        , "novbfLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "novbfLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "novbfLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "novbfLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "novbfLowMoriond", outputDir);

//        produce2DFit(mH[j],variables[i], ""        , "novbfHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "novbfHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "novbfHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "novbfHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "novbfHighMoriond", outputDir);

//        produce2DFit(mH[j],variables[i], ""        , "boostLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "boostLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "boostLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "boostLowMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "boostLowMoriond", outputDir);
 
//        produce2DFit(mH[j],variables[i], ""        , "boostHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "boostHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "boostHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "boostHighMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "boostHighMoriond", outputDir);

//        produce2DFit(mH[j],variables[i], ""        , "vbfMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauUp"   , "vbfMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "TauDown" , "vbfMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetUp"   , "vbfMoriond", outputDir);
//        produce2DFit(mH[j],variables[i], "JetDown" , "vbfMoriond", outputDir);
//       }//DO2DFIT

    }//mH
  }//variables
}




int main(int argc, const char* argv[])
{
  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  if(argc==1)      produceAll();
  else if(argc==2) produceAll(argv[1]);
  else return -1;
}
