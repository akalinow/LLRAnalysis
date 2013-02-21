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


#define RESCALETO1PB false
#define DOSUSY false


using namespace std;



float higgsXsection(int mH = 120, string process = "ggH"){

  float xsection = 1.0;

  if(process.find("ggH")!=string::npos){
    if(mH == 110) xsection = 8.02e-02*19.84;
    if(mH == 115) xsection = 7.65e-02*18.13;
    if(mH == 120) xsection = 7.10e-02*16.63;
    if(mH == 125) xsection = 6.37e-02*15.31;
    if(mH == 130) xsection = 5.48e-02*14.12;
    if(mH == 135) xsection = 4.52e-02*13.08;
    if(mH == 140) xsection = 3.54e-02*12.13;
    if(mH == 145) xsection = 2.61e-02*11.27;
  }
  if(process.find("qqH")!=string::npos){
    if(mH == 110) xsection = 8.02e-02*1.398;
    if(mH == 115) xsection = 7.65e-02*1.332;
    if(mH == 120) xsection = 7.10e-02*1.269;
    if(mH == 125) xsection = 6.37e-02*1.211;
    if(mH == 130) xsection = 5.48e-02*1.154;
    if(mH == 135) xsection = 4.52e-02*1.100;
    if(mH == 140) xsection = 3.54e-02*1.052;
    if(mH == 145) xsection = 2.61e-02*1.004;
  }
  if(process.find("VH")!=string::npos){
    if(mH == 110) xsection = 8.02e-02*(0.8754+0.4721+0.1257  );
    if(mH == 115) xsection = 7.65e-02*(0.7546+0.4107+0.1106  );
    if(mH == 120) xsection = 7.10e-02*(0.6561+0.3598+0.09756 );
    if(mH == 125) xsection = 6.37e-02*(0.5729+0.3158+0.08634 );
    if(mH == 130) xsection = 5.48e-02*(0.4942+0.2778+0.07658 );
    if(mH == 135) xsection = 4.52e-02*(0.4390+0.2453+0.06810 );
    if(mH == 140) xsection = 3.54e-02*(0.3857+0.2172+0.06072 );
    if(mH == 145) xsection = 2.61e-02*(0.3406+0.1930+0.05435 );
  }

  return xsection;
}



void produce(   
	     int mH_=120,
	     string variable_  = "diTauVisMass",
	     string analysis_  = "",
	     string bin_       = "inclusive",
	     TString outputDir = "Dec2011"
	     ){


  cout << "Now doing mass mH=" << mH_ << ", for variable " << variable_ << " analysis " << analysis_ << " and bin " << bin_ << endl;
  TFile* fin = 0;
  string analysisFile = analysis_;
  if(analysis_.find("FakeUp")!=string::npos)   analysisFile = "TauUp";
  if(analysis_.find("FakeDown")!=string::npos) analysisFile = "TauDown";

  fin = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_%s_%s.root", outputDir.Data(), 120, bin_.c_str() , analysisFile.c_str(), variable_.c_str()), "READ");


  ///////////////////////////////////////////////
  TFile* fin_jUp     = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_JetUp_%s.root",   outputDir.Data(), 120, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_jDown   = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_JetDown_%s.root", outputDir.Data(), 120, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tUp     = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_TauUp_%s.root",   outputDir.Data(), 120, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_tDown   = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_TauDown_%s.root", outputDir.Data(), 120, bin_.c_str() , variable_.c_str()), "READ");
  TFile* fin_nominal = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s__%s.root",        outputDir.Data(), 120, bin_.c_str() , variable_.c_str()), "READ");
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

  //TFile* fTemplOut = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/datacards/%s/eTauSM_%s.root",outputDir.Data(), variable_.c_str()),"UPDATE");
  string theory =  !DOSUSY ? "SM" : "MSSM" ;

  TFile* fTemplOut = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/datacards/%s/eTau%s.root",outputDir.Data(), theory.c_str()),"UPDATE");
  
  string suffix = "";
  if(analysis_.find("TauUp")!=string::npos)
    suffix = "_CMS_scale_tUp";
  else if(analysis_.find("TauDown")!=string::npos)
    suffix = "_CMS_scale_tDown";
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
  TString dirName( Form("eTau_%s",binNameSpace.c_str()) );
  
  if(! (fTemplOut->cd( dirName.Data()  ) ) ){
    
    cout << "Editing the directory for bin and variable " << binNameSpace << ", " << variable_ << endl;

    TDirectory* dir = fTemplOut->mkdir( dirName.Data() );
    dir->cd();


    ((TH1F*)fin->Get("hData"))->Write("data_obs");

    if(!DOSUSY){
      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hggH%d",mH_));
      hSgn2->Scale(1./rescaleggH);
      hSgn2->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hqqH%d",mH_));
      hSgn1->Scale(1./rescaleqqH);
      hSgn1->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
      hSgn3->Scale(1./rescaleVH);

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
    else{
      TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
      hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));

      TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
      hSgn2->Write(Form("bbH%d%s" ,mH_,suffix.c_str()));
    }

    if(bin_.find("novbf")!=string::npos){

      // ZTT: embedded sample
      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));

      // ----- old ------
      //TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
      //TH1F* hAntiIsoKeys = (TH1F*)fin->Get("hAntiIsoKeys") ;
      //hAntiIsoKeys->Scale(hQCD->Integral()/hAntiIsoKeys->Integral());

      // QCD:  low-pt: SS data; high-pt: loose-iso data normalized to SS 
      TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
      TH1F* hLooseIso    = (TH1F*)fin->Get("hLooseIso2") ;

      if( bin_.find("Low")!=string::npos ){
	//hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      else{
	//hDataAntiIso->Scale(hQCD->Integral()/hDataAntiIso->Integral());
	//hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
      }
   
      if( bin_.find("Low")!=string::npos ){
	//((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
	TH1F* hW     = (TH1F*)fin->Get("hW");         // <---------------   hW
	TH1F* hWKeys = (TH1F*)fin->Get("hWLooseIso2");
        hWKeys->Scale(hW->Integral()/hWKeys->Integral());
        hWKeys->Write(Form("W%s"    ,suffix.c_str()));
      }
      else{
	// W: low-pt MC; high-pt MC tau-iso > 0.0
	TH1F* hW     = (TH1F*)fin->Get("hW");         // <---------------   hW
	TH1F* hWKeys = (TH1F*)fin->Get("hWLooseIso2");
	hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	//hW->Write(Form("W%s"    ,suffix.c_str()));
      }
	
      ((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hZfakes"))->Write(Form("ZLL%s"    ,suffix.c_str()));
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));

    }
    else if(bin_.find("boost")!=string::npos){

    
      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));

      // ------- old --------
      //TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
      //TH1F* hAntiIsoKeys = (TH1F*)fin->Get("hAntiIsoKeys");
      //hAntiIsoKeys->Scale(hQCD->Integral()/hAntiIsoKeys->Integral());
      //hAntiIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
      //hQCD->Write(Form("QCD%s"    ,suffix.c_str()));

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
   
      if( bin_.find("Low")!=string::npos )
	((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
      else{
	// W: low-pt MC; high-pt MC tau-iso > 0.0
	TH1F* hW     = (TH1F*)fin->Get("hW");         // <---------------   hW
	//TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	//hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	//hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	hW->Write(Form("W%s"    ,suffix.c_str()));
      }

      //TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
      //TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");
      TH1F* hZfakesKeys = (TH1F*)fin->Get("hZfakes");
      hZmmKeys->Write(Form("ZL%s"        ,suffix.c_str()));
      hZmjKeys->Write(Form("ZJ%s"        ,suffix.c_str()));
      hZfakesKeys->Reset();
      hZfakesKeys->Add(hZmjKeys,1.0);
      hZfakesKeys->Add(hZmmKeys,1.0);
      hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));

      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
    }

    else if(bin_.find("bTag")!=string::npos){

      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));

      TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
      //hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      TH1F* hAntiIsoKeys = (TH1F*)fin->Get("hAntiIsoKeys");
      //TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
      //TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
      //hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
      //hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));

      if( bin_.find("Low")!=string::npos )
	hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
      else
	hAntiIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));

      ((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));
      //((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));

      //TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
      //TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");
      TH1F* hZfakesKeys = (TH1F*)fin->Get("hZfakes");
      hZfakesKeys->Reset();
      hZfakesKeys->Add(hZmjKeys,1.0);
      hZfakesKeys->Add(hZmmKeys,1.0);
      hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));

      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){


      // ZTT from embedded
      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));

      // QCD: take shape from Anti-iso muons && loose tau-ID && loose vbf; norm from mu iso FR
      TH1F* hDataAntiIsoLooseTauIso  = (TH1F*)fin->Get("hDataAntiIsoLooseTauIso");
      TH1F* hDataAntiIsoFR           = (TH1F*)fin->Get("hDataAntiIsoFR");
      float QCDfromSS                = ((TH1F*)fin->Get("hParameters"))->GetBinContent(22);
      QCDfromSS *= 1.07;
      //hDataAntiIsoLooseTauIso->Scale(hDataAntiIsoFR->Integral()/hDataAntiIsoLooseTauIso->Integral());
      hDataAntiIsoLooseTauIso->Scale(QCDfromSS/hDataAntiIsoLooseTauIso->Integral());
      hDataAntiIsoLooseTauIso->Write(Form("QCD%s"    ,suffix.c_str()));


      // ---- old ----  
      //((TH1F*)fin->Get("hAntiIsoKeys"))->Write(Form("QCD%s"    ,suffix.c_str()));
      //TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
      //TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
      //TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
      //hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
      //hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));

      // W+jets: shape from W3jets loose tau-ID; norm from sdb
      // W+jets: shape from W3jets loose tau-ID; norm from sdb2
      TH1F* hW3JetsLooseTauIso  = (TH1F*)fin->Get("hW3JetsLooseTauIso");
      //TH1F* hParameters =  (TH1F*)fin->Get("hParameters");
      //float ExtrapFact  =  hParameters->GetBinContent(23);
      //float DataSdb2    =  hParameters->GetBinContent(24);
      //float TTbarSdb2   =  hParameters->GetBinContent(25);
      //float QCDSdb2     =  hParameters->GetBinContent(26);
      //float Wyield      = (DataSdb2-TTbarSdb2-QCDSdb2)/ExtrapFact;
      //hW3JetsLooseTauIso->Scale(Wyield/hW3JetsLooseTauIso->Integral());
      hW3JetsLooseTauIso->Write(Form("W%s"           ,suffix.c_str()));

      // ---- old ----  
      //((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));
      //((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));


      // ZLL: keys pdf from MC
      TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjLoose");
      TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmLoose");
      TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
      TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
      TH1F* hZfakes     = (TH1F*)fin->Get("hZfakes");
      //hZfakesKeys->Reset();
      //if(hZmjKeys->Integral()>0) hZfakesKeys->Add(hZmjKeys,1.0);
      //hZfakesKeys->Add(hZmj,1.0);
      //if(hZmmKeys->Integral()>0) hZfakesKeys->Add(hZmmKeys,1.0);  
      //hZfakesKeys->Add(hZmm,1.0);
      hZfakes->Reset();
      hZfakes->Add(hZmjKeys);
      hZfakes->Add(hZmmKeys);
      hZfakes->Write(Form("ZLL%s"        ,suffix.c_str()));

      // TT: MV
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));

      // VV: MC keys
      ((TH1F*)fin->Get("hVVKeys"))->Write(Form("VV%s"         ,suffix.c_str()));
    }
    else if(bin_.find("vh")!=string::npos){

      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));

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


      ((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s",suffix.c_str()));
      ((TH1F*)fin->Get("hQcd"))->Write(Form("QCD%s"    ,suffix.c_str()));
      ((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
      ((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hZfakes"))->Write(Form("ZLL%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      ((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
    }


           
  }else{
    cout << "Directory is there, filling new histos..." << endl;

    TDirectory* dir = (TDirectory*)fTemplOut->Get( dirName.Data() );

    fTemplOut->cd( dirName.Data() );
    
    if(!DOSUSY){
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
	TH1F* hSgn2 = (TH1F*)fin->Get(Form("hqqH%d",mH_));
	hSgn2->Scale(1./rescaleqqH);
	hSgn2->Write(Form("qqH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("ggH%d%s"          , mH_,suffix.c_str()))==0 ){
	TH1F* hSgn1 = (TH1F*)fin->Get(Form("hggH%d",mH_));
	hSgn1->Scale(1./rescaleggH);
	hSgn1->Write(Form("ggH%d%s" , mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("VH%d%s"          , mH_,suffix.c_str()))==0 ){

	TH1F* hSgn3 = (TH1F*)fin->Get(Form("hVH%d",mH_));
	hSgn3->Scale(1./rescaleVH);

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
    else{
      if(dir->FindObjectAny(Form("qqH%d%s"         ,mH_,suffix.c_str()))==0 ){
        TH1F* hSgn1 = (TH1F*)fin->Get(Form("hSUSYGG%d",mH_));
        hSgn1->Write(Form("ggH%d%s" ,mH_,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("bbH%d%s"          , mH_,suffix.c_str()))==0 ){
        TH1F* hSgn2 = (TH1F*)fin->Get(Form("hSUSYBB%d",mH_));
        hSgn2->Write(Form("bbH%d%s" , mH_,suffix.c_str()));
      }
    }



    if(bin_.find("novbf")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));

      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){

	// QCD:  low-pt: SS data; high-pt: anti-iso data 
	TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
	TH1F* hLooseIso    = (TH1F*)fin->Get("hLooseIso2") ;
	
	if( bin_.find("Low")!=string::npos ){
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	  hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
          hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
	}
	else{
	  //hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	  hLooseIso->Scale(hQCD->Integral()/hLooseIso->Integral());
	  hLooseIso->Write(Form("QCD%s"    ,suffix.c_str()));
	}

      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){

	if( bin_.find("Low")!=string::npos ){
	  TH1F* hW     = (TH1F*)fin->Get("hW");            //<-------------  hW
	  TH1F* hWKeys = (TH1F*)fin->Get("hWLooseIso2");
          hWKeys->Scale(hW->Integral()/hWKeys->Integral());
          hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	  //((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
	}
	else{
	  // W: low-pt MC; high-pt MC tau-iso > 0.0
	  TH1F* hW     = (TH1F*)fin->Get("hW");            //<-------------  hW
	  TH1F* hWKeys = (TH1F*)fin->Get("hWLooseIso2");
	  hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	  hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	  //hW->Write(Form("W%s"    ,suffix.c_str()));
	}
	
      }
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZfakes"))->Write(Form("ZLL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
     
      

    }
    else if(bin_.find("boost")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){

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

      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){
	
	if( bin_.find("Low")!=string::npos )
	  ((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
	else{
	  // W: low-pt MC; high-pt MC tau-iso > 0.0
	  TH1F* hW     = (TH1F*)fin->Get("hW");         // <---------------   hW
	  //TH1F* hWKeys = (TH1F*)fin->Get("hWKeys");
	  //hWKeys->Scale(hW->Integral()/hWKeys->Integral());
	  //hWKeys->Write(Form("W%s"    ,suffix.c_str()));
	  hW->Write(Form("W%s"    ,suffix.c_str()));
	}
 
      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));

      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	//TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
	//TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakesKeys");
	hZfakesKeys->Reset();
	hZfakesKeys->Add(hZmjKeys,1.0);
	hZfakesKeys->Add(hZmmKeys,1.0);  
	hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));
    
    }
    else if(bin_.find("bTag")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){
	TH1F* hQCD         = (TH1F*)fin->Get("hQCD");
	TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");

	if(  bin_.find("Low")!=string::npos )
	  hQCD->Write(Form("QCD%s"    ,suffix.c_str()));
	else 
	  hAntiIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));
	//((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));

      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){
	//TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjKeys");
	//TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmKeys");
	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmj");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakesKeys = (TH1F*)hZmjKeys->Clone("hZfakes");
	hZfakesKeys->Reset();
	hZfakesKeys->Add(hZmjKeys,1.0);
	hZfakesKeys->Add(hZmmKeys,1.0);  
	hZfakesKeys->Write(Form("ZLL%s"        ,suffix.c_str()));
      }
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));

   

    }
    else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));

      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 ){

	TH1F* hDataAntiIsoLooseTauIso  = (TH1F*)fin->Get("hDataAntiIsoLooseTauIso");
	TH1F* hDataAntiIsoFR           = (TH1F*)fin->Get("hDataAntiIsoFR");
	float QCDfromSS                = ((TH1F*)fin->Get("hParameters"))->GetBinContent(22);
	QCDfromSS *= 1.07;
	//hDataAntiIsoLooseTauIso->Scale(hDataAntiIsoFR->Integral()/hDataAntiIsoLooseTauIso->Integral());
	hDataAntiIsoLooseTauIso->Scale(QCDfromSS/hDataAntiIsoLooseTauIso->Integral());
	hDataAntiIsoLooseTauIso->Write(Form("QCD%s"    ,suffix.c_str()));
	// ---- old ---- 
	//TH1F* hAntiIsoKeys  = (TH1F*)fin->Get("hAntiIsoKeys");
	//TH1F* hAntiIso      = (TH1F*)fin->Get("hAntiIso");
	//TH1F* hLooseIsoKeys = (TH1F*)fin->Get("hLooseIsoKeys");
	//hLooseIsoKeys->Scale(hAntiIso->Integral()/hLooseIsoKeys->Integral());
	//hLooseIsoKeys->Write(Form("QCD%s"    ,suffix.c_str()));

      }

      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 ){

	TH1F* hW3JetsLooseTauIso  = (TH1F*)fin->Get("hW3JetsLooseTauIso");
        //TH1F* hParameters =  (TH1F*)fin->Get("hParameters");
        //float ExtrapFact  =  hParameters->GetBinContent(23);
        //float DataSdb2    =  hParameters->GetBinContent(24);
        //float TTbarSdb2   =  hParameters->GetBinContent(25);
        //float QCDSdb2     =  hParameters->GetBinContent(26);
        //float Wyield      = (DataSdb2-TTbarSdb2-QCDSdb2)/ExtrapFact;
        //hW3JetsLooseTauIso->Scale(Wyield/hW3JetsLooseTauIso->Integral());
        hW3JetsLooseTauIso->Write(Form("W%s"           ,suffix.c_str()));

	// ---- old ---- 
	//((TH1F*)fin->Get("hW3JetsKeys"))->Write(Form("W%s"           ,suffix.c_str()));
	//((TH1F*)fin->Get("hWKeys"))->Write(Form("W%s"           ,suffix.c_str()));

      }

      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ){

	TH1F* hZmjKeys    = (TH1F*)fin->Get("hZmjLoose");
	TH1F* hZmmKeys    = (TH1F*)fin->Get("hZmmLoose");
	TH1F* hZmj        = (TH1F*)fin->Get("hZmj");
	TH1F* hZmm        = (TH1F*)fin->Get("hZmm");
	TH1F* hZfakes     = (TH1F*)fin->Get("hZfakes");
	hZfakes->Reset();
	hZfakes->Add(hZmjKeys);
	hZfakes->Add(hZmmKeys);
	hZfakes->Write(Form("ZLL%s"        ,suffix.c_str()));
      }  
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVVKeys"))->Write(Form("VV%s"         ,suffix.c_str()));

     

    }
    else if(bin_.find("vh")!=string::npos){

      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));

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
      if(dir->FindObjectAny(Form("ZTT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hDataEmb"))->Write(Form("ZTT%s"       ,suffix.c_str()));
      if(dir->FindObjectAny(Form("QCD%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hAntiIso"))->Write(Form("QCD%s"       ,suffix.c_str()));
      if(dir->FindObjectAny(Form("W%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hW"))->Write(Form("W%s"           ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZJ%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hZmj"))->Write(Form("ZJ%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZmm"))->Write(Form("ZL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("ZLL%s"       ,suffix.c_str()))==0 ) 
	((TH1F*)fin->Get("hZfakes"))->Write(Form("ZLL%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("TT%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hTTb"))->Write(Form("TT%s"        ,suffix.c_str()));
      if(dir->FindObjectAny(Form("VV%s"       ,suffix.c_str()))==0 )
	((TH1F*)fin->Get("hVV"))->Write(Form("VV%s"         ,suffix.c_str()));

    }


  }


  
  fTemplOut->Close();
  
  // edit the datacards only for the nominal analysis
  if(analysis_.find("Up")!=string::npos || analysis_.find("Down")!=string::npos){

    fin->Close();          delete fin;
    fin_tUp->Close();      delete fin_tUp;
    fin_tDown->Close();    delete fin_tDown;
    fin_jUp->Close();      delete fin_jUp;
    fin_jDown->Close();    delete fin_jDown;

    return;
  }


  ifstream in;

  char* c = new char[1000];
  in.open(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/templates/%s/eTau_%s_template_v3.txt", theory.c_str(),    binNameSpace.c_str()));
  //ofstream out(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/datacards/%s/eTau_%s_mH%d_%s.txt", outputDir.Data(), binNameSpace.c_str(), mH_, variable_.c_str()));
  string resonance = !DOSUSY ? "H" : "A";

  ofstream out(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/datacards/%s/eTau_%s_m%s%d.txt", outputDir.Data(), binNameSpace.c_str(), resonance.c_str(), mH_ ));
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
          //line.replace( line.find("XXX") , 3 , string(Form("eTauSM_%s",variable_.c_str()))  );
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
	  
	  if(bin_.find("boost")!=string::npos){
	    QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 
	  }
	  else if(bin_.find("novbf")!=string::npos ){
	    if(   bin_.find("Low")!=string::npos )
	      QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 	   
	    else
	      QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral();
	  }
	  else if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos ){
	    float QCDfromSS = ((TH1F*)fin->Get("hParameters"))->GetBinContent(22);
	    QCDfromSS *= 1.07;
	    //QCDyield = ((TH1F*)fin->Get("hDataAntiIsoFR"))->Integral(); 	   
	    QCDyield = QCDfromSS;
	  }
	  else if( bin_.find("bTag")!=string::npos  ){
	    if(   bin_.find("Low")!=string::npos )
	      QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 	   
	    else
	      QCDyield = ((TH1F*)fin->Get("hAntiIso"))->Integral(); 	   
	  }
	  else{
	    QCDyield = ((TH1F*)fin->Get("hQCD"))->Integral(); 
	  }

	  if(bin_.find("vbf")!=string::npos && bin_.find("novbf")==string::npos){

	    //TH1F* hParameters =  (TH1F*)fin->Get("hParameters");
            //float ExtrapFact  =  hParameters->GetBinContent(23);
            //float DataSdb2    =  hParameters->GetBinContent(24);
            //float TTbarSdb2   =  hParameters->GetBinContent(25);
            //float QCDSdb2     =  hParameters->GetBinContent(26);
            //Wyield      = (DataSdb2-TTbarSdb2-QCDSdb2)/ExtrapFact;
            Wyield = ((TH1F*)fin->Get("hW3JetsLooseTauIso"))->Integral();

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
	  else{
	    TH1F* hZmj        = (TH1F*)fin->Get("hZmjLoose");
            TH1F* hZmm        = (TH1F*)fin->Get("hZmmLoose");
            float ZFakesyield = hZmj->Integral();
            ZFakesyield += hZmm->Integral();
            out << space << ZFakesyield;
	  }
	  out << space << ((TH1F*)fin->Get("hTTb"))->Integral()
	      << space << ((TH1F*)fin->Get("hVV"))->Integral()
	      << endl;
	}
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
	else if(line.find("CMS_scale_j")!=string::npos){

	  float VBFrel = 0; float VBFrelUp = 0;  float VBFrelDown = 0;
	  float SMrel  = 0; float SMrelUp  = 0;  float SMrelDown  = 0;
	  float VHrel  = 0; float VHrelUp  = 0;  float VHrelDown  = 0;

	  if(!DOSUSY){

	    //////////////////////////////// FIX HERE!!!

	    VBFrel = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral()))
				);
	    VBFrelUp   = (((TH1F*)fin_jUp->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral());
	    VBFrelDown = (((TH1F*)fin_jDown->Get(Form("hqqH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hqqH%d",mH_)))->Integral());
	    
	    VBFrel     *= 1./rescaleqqH;
	    VBFrelUp   *= 1./rescaleqqH;
	    VBFrelDown *= 1./rescaleqqH;

	    SMrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral()))
				);
	    
	    SMrelUp   = (((TH1F*)fin_jUp->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral());
	    SMrelDown = (((TH1F*)fin_jDown->Get(Form("hggH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hggH%d",mH_)))->Integral());
	    
	    SMrel     *= 1./rescaleggH;
	    SMrelUp   *= 1./rescaleggH;
	    SMrelDown *= 1./rescaleggH;

	    VHrel  = TMath::Max(TMath::Abs((((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral())),
				TMath::Abs((((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral()))
				);
	    
	    VHrelUp   = (((TH1F*)fin_jUp->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());
	    VHrelDown = (((TH1F*)fin_jDown->Get(Form("hVH%d",mH_)))->Integral()/((TH1F*)fin->Get(Form("hVH%d",mH_)))->Integral());

	    VHrel     *= 1./rescaleVH;
	    VHrelUp   *= 1./rescaleVH;
	    VHrelDown *= 1./rescaleVH;

	  }
	  else{
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
	else if(line.find( Form("CMS_htt_eTau_%s_QCDNorm",binNameSpace.c_str()) )!=string::npos){
	 
	  line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(5)))  );
	  line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(6)))  );
	 
	  out << line << endl;
	}
	else if(line.find( Form("CMS_htt_eTau_%s_WNorm",binNameSpace.c_str()) )!=string::npos){

	  // in VBF use extrapolation factor from W+3jets
	  if(binNameSpace.find("vbf")!=string::npos){

	    float DataVBFSdb  = ((TH1F*)fin->Get("hParameters"))->GetBinContent(14);
	    float TTbarVBFSdb = ((TH1F*)fin->Get("hParameters"))->GetBinContent(15);
	    float QCDVBFSdb   = ((TH1F*)fin->Get("hParameters"))->GetBinContent(16);

	    float ExtrapFactor = Wyield/(DataVBFSdb+TTbarVBFSdb+QCDVBFSdb);

	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",  (DataVBFSdb+TTbarVBFSdb+QCDVBFSdb))));
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",  ExtrapFactor)));
	  }
	  else{
	    line.replace( line.find("XXX") , 3 , string(Form("%.0f",((TH1F*)fin->Get("hParameters"))->GetBinContent(2)))  );
	    line.replace( line.find("YYY") , 3 , string(Form("%.4f",((TH1F*)fin->Get("hParameters"))->GetBinContent(1)))  );
	  }
	  out << line << endl;
	}
	else{
	  out << c << endl;
	}
      
      }
 
   }

  fin->Close();          delete fin;
  fin_tUp->Close();      delete fin_tUp;
  fin_tDown->Close();    delete fin_tDown;
  fin_jUp->Close();      delete fin_jUp;
  fin_jDown->Close();    delete fin_jDown;

  return;

}



void produceAll(  TString outputDir = "June2012/Approval_checks2" ){

  vector<string> variables;
  vector<int> mH;

  //variables.push_back("diTauVisMass");
  variables.push_back("diTauNSVfitMass");
  //variables.push_back("diTauSVFitMassCal0");
  //variables.push_back("diTauSVFitMassCal1");
  //variables.push_back("diTauSVFitMassCal2");


  //mH.push_back(105);
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

//        produce(mH[j],variables[i], ""        , "novbfLow", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "novbfLow", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "novbfLow", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "novbfLow", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "novbfLow", outputDir);
      
//        produce(mH[j],variables[i], ""        , "novbfHigh", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "novbfHigh", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "novbfHigh", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "novbfHigh", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "novbfHigh", outputDir);
      
//        produce(mH[j],variables[i], ""        , "boostLow", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "boostLow", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "boostLow", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "boostLow", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "boostLow", outputDir);
      
//        produce(mH[j],variables[i], ""        , "boostHigh", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "boostHigh", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "boostHigh", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "boostHigh", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "boostHigh", outputDir);
      
//        produce(mH[j],variables[i], ""        , "bTagLow", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "bTagLow", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "bTagLow", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "bTagLow", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "bTagLow", outputDir);
      
//        produce(mH[j],variables[i], ""        , "bTagHigh", outputDir);
//        produce(mH[j],variables[i], "TauUp"   , "bTagHigh", outputDir);
//        produce(mH[j],variables[i], "TauDown" , "bTagHigh", outputDir);
//        produce(mH[j],variables[i], "JetUp"   , "bTagHigh", outputDir);
//        produce(mH[j],variables[i], "JetDown" , "bTagHigh", outputDir);
      
       produce(mH[j],variables[i], ""        , "vbf", outputDir);
       produce(mH[j],variables[i], "TauUp"   , "vbf", outputDir);
       produce(mH[j],variables[i], "TauDown" , "vbf", outputDir);
       produce(mH[j],variables[i], "JetUp"   , "vbf", outputDir);
       produce(mH[j],variables[i], "JetDown" , "vbf", outputDir);
       produce(mH[j],variables[i], "FakeUp"   , "vbf", outputDir);
       produce(mH[j],variables[i], "FakeDown" , "vbf", outputDir);

//       produce(mH[j],variables[i], ""        , "vh", outputDir);
//       produce(mH[j],variables[i], "TauUp"   , "vh", outputDir);
//       produce(mH[j],variables[i], "TauDown" , "vh", outputDir);
//       produce(mH[j],variables[i], "JetUp"   , "vh", outputDir);
//       produce(mH[j],variables[i], "JetDown" , "vh", outputDir);

     
  
      //produce(mH[j],variables[i], ""        , "novbf", outputDir);
      //produce(mH[j],variables[i], "TauUp"   , "novbf", outputDir);
      //produce(mH[j],variables[i], "TauDown" , "novbf", outputDir);
      //produce(mH[j],variables[i], "JetUp"   , "novbf", outputDir);
      //produce(mH[j],variables[i], "JetDown" , "novbf", outputDir);
  
    }

  }

}




int main(int argc, const char* argv[])
{

  std::cout << "produce()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  produceAll();

}
