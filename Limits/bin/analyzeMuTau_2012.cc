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

void plotMuTau( Int_t mH_           = 120,
		Int_t useEmbedding_ = 0,
		string selection_   = "inclusive",
		string analysis_    = "",		  
		TString variable_   = "diTauVisMass",
		TString XTitle_     = "full mass",
		TString Unities_    = "GeV",
		TString outputDir   = "./",
		Int_t nBins_ = 80, Float_t xMin_=0, Float_t xMax_=400,
		Float_t magnifySgn_ = 1.0,
		Float_t hltEff_     = 1.0,
		Int_t logy_         = 0,
		Float_t maxY_       = 1.2
		) 
{   

  //string postfix_ = "Raw";
  string postfix_ = "";

  ofstream out(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/histograms/%s/yieldsMuTau_mH%d_%s_%s.txt",outputDir.Data(),mH_,selection_.c_str(), analysis_.c_str() ),ios_base::out); 
  out.precision(5);
  out<< " => " << selection_ << endl;

  
  // input txt file with bins
  ifstream is;

  char* c = new char[10];
  is.open(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/bins/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
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
  TArrayF bins(nBins+1);
  cout << "Making histograms with " << nBins << " bins:" << endl;

  is.close();
  is.open(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/bins/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  
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


  float Lumi           = 5098.0;
  float lumiCorrFactor = 1.0;

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  float TTxsectionRatio           = 0.92; // using e-mu SF from Valentina 
  //lumiCorrFactor*(165.8/157.5) ;

  float OStoSSRatioQCD            = 1.11;
  float SSIsoToSSAIsoRatioQCD     = 1.00;

  float MutoTauCorrectionFactor   = 0.976; //from Valentina Z->mm data/mc
  float JtoTauCorrectionFactor    = 0.976; //from Valentina Z->mm data/mc

  float embeddedMEtCutEff         = 1.00;
  float madgraphMEtCutEff         = 1.00;

  // Fall11_06Dec2011
  float WcorrectionFactorOS        = 0.92;  
  float WcorrectionFactorSS        = 1.08; 
  float ExtrapolationFactorZ       = 1.0;
  float ExtrapolationFactorZDataMC = 1.0;
  float ErrorExtrapolationFactorZ  = 1.0;

  //float NoVbfExtrapolationFactorZ = 0.997;
  //float VbfExtrapolationFactorZ   = 1.37;
  //float BoostExtrapolationFactorZ = 0.98;

  float VbfExtrapolationFactorW   = 1.00;
  float BoostExtrapolationFactorW = 1.00;
  float scaleFactorTTinVBF        = 1.00;
  float scaleFactorTTOS           = 1.0;
  /////////////////  change SVfit mass here ///////////////////

  //string variableStr = "";
  //TString variable(variableStr.c_str());
  TString variable = variable_;

  //////////////////////////////////////////////////////////////

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
  float antiWsgn  = useMt ? 40. :  20. ;
  float antiWsdb  = useMt ? 60. :  40. ; 

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
  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=7 TeV}{%.1f fb^{-1} #tau_{#mu}#tau_{had}}", Lumi/1000. ));

  THStack* aStack = new THStack("aStack","");

  TH1F* hSiml    = new TH1F( "hSiml"   ,"all"               , nBins , bins.GetArray());
  TH1F* hSgn     = new TH1F( "hSgn "   ,"vbf+ggf"           , nBins , bins.GetArray());
  TH1F* hSgn1    = new TH1F( "hSgn1"   ,"vbf"               , nBins , bins.GetArray());
  TH1F* hSgn2    = new TH1F( "hSgn2"   ,"ggf"               , nBins , bins.GetArray());
  TH1F* hSgn3    = new TH1F( "hSgn3"   ,"vh"                , nBins , bins.GetArray());
  TH1F* hData    = new TH1F( "hData"   ,"        "          , nBins , bins.GetArray());
  TH1F* hDataEmb = new TH1F( "hDataEmb","Embedded"          , nBins , bins.GetArray());
  TH1F* hDataEmbLooseVBF = new TH1F( "hDataEmbLooseVBF","Embedded for VBF" , nBins , bins.GetArray());
  TH1F* hW       = new TH1F( "hW"      ,"W+jets"            , nBins , bins.GetArray());
  TH1F* hWLooseIso2 = new TH1F( "hWLooseIso2","W+jets(miso)", nBins , bins.GetArray());
  TH1F* hW3Jets  = new TH1F( "hW3Jets" ,"W+3jets"           , nBins , bins.GetArray());
  TH1F* hEWK     = new TH1F( "hEWK"    ,"EWK"               , nBins , bins.GetArray());
  TH1F* hZtt     = new TH1F( "hZtt"    ,"Ztautau"           , nBins , bins.GetArray());
  TH1F* hZmm     = new TH1F( "hZmm"    ,"Z+jets, mu to tau" , nBins , bins.GetArray());
  TH1F* hZmmLoose= new TH1F( "hZmmLoose","Z+jets, mu to tau (loose)", nBins , bins.GetArray());
  TH1F* hZmj     = new TH1F( "hZmj"    ,"Z+jets, jet to tau", nBins , bins.GetArray());
  TH1F* hZmjLoose= new TH1F( "hZmjLoose","Z+jets, jet to tau (loose)",nBins , bins.GetArray());
  TH1F* hZfakes  = new TH1F( "hZfakes" ,"Z+jets, jet to tau", nBins , bins.GetArray());
  TH1F* hTTb     = new TH1F( "hTTb"    ,"ttbar"             , nBins , bins.GetArray());
  TH1F* hTTbLoose = new TH1F( "hTTbLoose"    ,"ttbar"       , nBins , bins.GetArray());
  TH1F* hQCD     = new TH1F( "hQCD"    ,"QCD"               , nBins , bins.GetArray());
  TH1F* hLooseIso= new TH1F( "hLooseIso","Loose Iso"        , nBins , bins.GetArray());
  TH1F* hLooseIso2= new TH1F( "hLooseIso2","Loose Iso"      , nBins , bins.GetArray());
  TH1F* hLooseIso3= new TH1F( "hLooseIso3","Loose Iso"      , nBins , bins.GetArray());

  TH1F* hAntiIso = new TH1F( "hAntiIso","Anti Iso"          , nBins , bins.GetArray());
  TH1F* hVV      = new TH1F( "hVV"     ,"Diboson"           , nBins , bins.GetArray());
  TH1F* hVVLoose = new TH1F( "hVVLoose" ,"Diboson"          , nBins , bins.GetArray());

  ///////////////////////// for bkg estimation //////////////////////////

  TH1F* hW3JetsLooseTauIso         = new TH1F( "hW3JetsLooseTauIso" ,  "W+3jets (loose tau-iso)" ,                      nBins , bins.GetArray());
  TH1F* hW3JetsLooseTauIsoFR       = new TH1F( "hW3JetsLooseTauIsoFR" ,"W+3jets (loose tau-iso X fake-rate)"          , nBins , bins.GetArray());

  TH1F* hDataAntiIsoLooseTauIso      = new TH1F( "hDataAntiIsoLooseTauIso"   ,"data anti-iso, loose tau-iso"            , nBins , bins.GetArray());
  TH1F* hDataAntiIsoLooseTauIsoFR    = new TH1F( "hDataAntiIsoLooseTauIsoFR" ,"data anti-iso, loose tau-iso X fake-rate", nBins , bins.GetArray());
  TH1F* hDataAntiIsoLooseTauIsoFRUp  = new TH1F( "hDataAntiIsoLooseTauIsoFRUp"   ,"data anti-iso, loose tau-iso X fake-rate (Up)", nBins , bins.GetArray());
  TH1F* hDataAntiIsoLooseTauIsoFRDown= new TH1F( "hDataAntiIsoLooseTauIsoFRDown" ,"data anti-iso, loose tau-iso X fake-rate (Down)", nBins , bins.GetArray());

  TH1F* hDataAntiIso               = new TH1F( "hDataAntiIso"   ,"data anti-iso"                                 , nBins , bins.GetArray());
  TH1F* hDataAntiIsoFR             = new TH1F( "hDataAntiIsoFR" ,"data anti-iso X fake-rate"                     , nBins , bins.GetArray());
  TH1F* hDataAntiIsoFRUp           = new TH1F( "hDataAntiIsoFRUp" , "data anti-iso X fake-rate (Up)"             , nBins , bins.GetArray());
  TH1F* hDataAntiIsoFRDown         = new TH1F( "hDataAntiIsoFRDown" ,"data anti-iso X fake-rate (Down)"          , nBins , bins.GetArray());
  TH1F* hWJetsLooseBTag         = new TH1F( "hWJetsLooseBTag" ,  "W+jets (loose bTag)" ,  nBins ,bins.GetArray());

  // get the FR-file

  TFile FakeRate("FakeRate.root","READ");
  if(FakeRate.IsZombie()){
    cout << "Missing FR histos... exit" << endl;
    return;
  }
 
  TF1*  frMu     = (TF1*)FakeRate.Get("fit_MuTau_Mu_ptL1_incl");
  TH1F* frMuUp   = (TH1F*)FakeRate.Get("hFakeRateErrUpMuTau_Mu_ptL1_incl");
  TH1F* frMuDown = (TH1F*)FakeRate.Get("hFakeRateErrDownMuTau_Mu_ptL1_incl");

  TF1*  frTauQCD     = (TF1*)FakeRate.Get("fitQCD_MuTau_Tau_ptL2_QCDSS02_WSS60_incl");
  TH1F* frTauQCDUp   = (TH1F*)FakeRate.Get("hFakeRateQCDErrUpMuTau_Tau_ptL2_QCDSS02_WSS60_incl");
  TH1F* frTauQCDDown = (TH1F*)FakeRate.Get("hFakeRateQCDErrDownMuTau_Tau_ptL2_QCDSS02_WSS60_incl");
  
  TF1*  frTauW     = (TF1*)FakeRate.Get("fitW_MuTau_Tau_ptL2_QCDOS02_WOS60_incl");
  TH1F* frTauWUp   = (TH1F*)FakeRate.Get("hFakeRateWErrUpMuTau_Tau_ptL2_QCDOS02_WOS60_incl");
  TH1F* frTauWDown = (TH1F*)FakeRate.Get("hFakeRateWErrDownMuTau_Tau_ptL2_QCDOS02_WOS60_incl");

  if(!frMu || !frMuUp || !frMuDown || !frTauQCD || !frTauQCDUp || !frTauQCDDown || !frTauW || !frTauWUp || !frTauWDown){
    cout << "Missing FR histos... exit" << endl;
    return;
  }

  vector<int> binsFR;
  binsFR.push_back(17);
  binsFR.push_back(20);
  binsFR.push_back(22);
  binsFR.push_back(24);
  binsFR.push_back(26);
  binsFR.push_back(28);
  binsFR.push_back(30);
  binsFR.push_back(32);
  binsFR.push_back(34);
  binsFR.push_back(36);
  binsFR.push_back(40);
  binsFR.push_back(45);
  binsFR.push_back(50);
  binsFR.push_back(60); 
  binsFR.push_back(80); 
  binsFR.push_back(100);
  binsFR.push_back(9999);

  string scaleFactMu = "( ";
  string scaleFactMuUp   = "( ";
  string scaleFactMuDown = "( ";

  string scaleFactTauQCD = "( ";
  string scaleFactTauQCDUp   = "( ";
  string scaleFactTauQCDDown = "( ";

  string scaleFactTauW = "( ";
  string scaleFactTauWUp   = "( ";
  string scaleFactTauWDown = "( ";

 for(unsigned int i = 0; i < binsFR.size()-1; i++){
    
    float min = binsFR[i];
    float max = binsFR[i+1];

    float bin = frMuUp->FindBin((max+min)/2.);
    if( bin == frMuUp->GetNbinsX() + 1) bin--;

    float weightBinMu_i     =  frMu->Eval( (max+min)/2.);
    float weightBinMu_iUp   =  frMuUp->GetBinContent( bin );
    float weightBinMu_iDown =  frMuDown->GetBinContent( bin );
    
    scaleFactMu     += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinMu_i ) );
    scaleFactMuUp   += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinMu_iUp   ) );
    scaleFactMuDown += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinMu_iDown ) );

    float weightBinTauQCD_i     =  frTauQCD->Eval( (max+min)/2.);
    float weightBinTauQCD_iUp   =  frTauQCDUp->GetBinContent( bin );
    float weightBinTauQCD_iDown =  frTauQCDDown->GetBinContent( bin );
    
    scaleFactTauQCD     += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauQCD_i ) );
    scaleFactTauQCDUp   += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauQCD_iUp   ) );
    scaleFactTauQCDDown += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauQCD_iDown ) );

    float weightBinTauW_i     =  frTauW->Eval( (max+min)/2.);
    float weightBinTauW_iUp   =  frTauWUp->GetBinContent( bin );
    float weightBinTauW_iDown =  frTauWDown->GetBinContent( bin );
    
    scaleFactTauW     += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauW_i ) );
    scaleFactTauWUp   += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauW_iUp   ) );
    scaleFactTauWDown += string( Form("(ptL2>=%f && ptL2<%f)*%f", min , max, weightBinTauW_iDown ) );

    if(i < binsFR.size() - 2 ){
      scaleFactMu     += " + ";
      scaleFactMuUp   += " + ";
      scaleFactMuDown += " + ";
      
      scaleFactTauQCD     += " + ";
      scaleFactTauQCDUp   += " + ";
      scaleFactTauQCDDown += " + ";
      
      scaleFactTauW     += " + ";
      scaleFactTauWUp   += " + ";
      scaleFactTauWDown += " + ";
    }
 }
 
 scaleFactMu     += " )";
 scaleFactMuUp   += " )";
 scaleFactMuDown += " )";
 
 scaleFactTauQCD     += " )";
 scaleFactTauQCDUp   += " )";
 scaleFactTauQCDDown += " )";
 
 scaleFactTauW     += " )";
 scaleFactTauWUp   += " )";
 scaleFactTauWDown += " )";
 
 cout << scaleFactMu << endl;
 cout << scaleFactTauQCD << endl;
 cout << scaleFactTauQCDUp << endl;
 cout << scaleFactTauW << endl;

  /////////////////////////////////////////////////////////////////////////


  TH1*  hW3JetsKeys   = 0;
  TH1*  hWKeys        = 0;
  TH1*  hLooseIsoKeys = 0;
  TH1*  hAntiIsoKeys  = 0;
  TH1*  hZmmKeys      = 0;
  TH1*  hZmjKeys      = 0;
  TH1*  hVVKeys       = 0;


  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////

  TH1F* hggH110    = new TH1F( "hggH110"   ,"ggH110"               , nBins , bins.GetArray());
  TH1F* hggH115    = new TH1F( "hggH115"   ,"ggH115"               , nBins , bins.GetArray());
  TH1F* hggH120    = new TH1F( "hggH120"   ,"ggH120"               , nBins , bins.GetArray());
  TH1F* hggH125    = new TH1F( "hggH125"   ,"ggH125"               , nBins , bins.GetArray());
  TH1F* hggH130    = new TH1F( "hggH130"   ,"ggH130"               , nBins , bins.GetArray());
  TH1F* hggH135    = new TH1F( "hggH135"   ,"ggH135"               , nBins , bins.GetArray());
  TH1F* hggH140    = new TH1F( "hggH140"   ,"ggH140"               , nBins , bins.GetArray());
  TH1F* hggH145    = new TH1F( "hggH145"   ,"ggH145"               , nBins , bins.GetArray());

  TH1F* hqqH110    = new TH1F( "hqqH110"   ,"qqH110"               , nBins , bins.GetArray());
  TH1F* hqqH115    = new TH1F( "hqqH115"   ,"qqH115"               , nBins , bins.GetArray());
  TH1F* hqqH120    = new TH1F( "hqqH120"   ,"qqH120"               , nBins , bins.GetArray());
  TH1F* hqqH125    = new TH1F( "hqqH125"   ,"qqH125"               , nBins , bins.GetArray());
  TH1F* hqqH130    = new TH1F( "hqqH130"   ,"qqH130"               , nBins , bins.GetArray());
  TH1F* hqqH135    = new TH1F( "hqqH135"   ,"qqH135"               , nBins , bins.GetArray());
  TH1F* hqqH140    = new TH1F( "hqqH140"   ,"qqH140"               , nBins , bins.GetArray());
  TH1F* hqqH145    = new TH1F( "hqqH145"   ,"qqH145"               , nBins , bins.GetArray());

  TH1F* hVH110    = new TH1F( "hVH110"   ,"VH110"               , nBins , bins.GetArray());
  TH1F* hVH115    = new TH1F( "hVH115"   ,"VH115"               , nBins , bins.GetArray());
  TH1F* hVH120    = new TH1F( "hVH120"   ,"VH120"               , nBins , bins.GetArray());
  TH1F* hVH125    = new TH1F( "hVH125"   ,"VH125"               , nBins , bins.GetArray());
  TH1F* hVH130    = new TH1F( "hVH130"   ,"VH130"               , nBins , bins.GetArray());
  TH1F* hVH135    = new TH1F( "hVH135"   ,"VH135"               , nBins , bins.GetArray());
  TH1F* hVH140    = new TH1F( "hVH140"   ,"VH140"               , nBins , bins.GetArray());
  TH1F* hVH145    = new TH1F( "hVH145"   ,"VH145"               , nBins , bins.GetArray());


  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////
  vector<string> SUSYhistos;
  //SUSYhistos.push_back("SUSYGG90");
  //SUSYhistos.push_back("SUSYGG100");
  //SUSYhistos.push_back("SUSYGG120");
  //SUSYhistos.push_back("SUSYGG130");
  //SUSYhistos.push_back("SUSYGG140");
  //SUSYhistos.push_back("SUSYGG160");
  //SUSYhistos.push_back("SUSYGG180");
  //SUSYhistos.push_back("SUSYGG200");
  //SUSYhistos.push_back("SUSYGG250");
  //SUSYhistos.push_back("SUSYGG300");
  //SUSYhistos.push_back("SUSYGG350");
  //SUSYhistos.push_back("SUSYGG400");
  //SUSYhistos.push_back("SUSYGG450");
  ////SUSYhistos.push_back("SUSYGG500");
  ////SUSYhistos.push_back("SUSYGG600");
  ////SUSYhistos.push_back("SUSYGG700");
  ////SUSYhistos.push_back("SUSYGG800");
  ////SUSYhistos.push_back("SUSYGG900");
  //SUSYhistos.push_back("SUSYBB90");
  //SUSYhistos.push_back("SUSYBB100");
  //SUSYhistos.push_back("SUSYBB120");
  //SUSYhistos.push_back("SUSYBB130");
  //SUSYhistos.push_back("SUSYBB140");
  //SUSYhistos.push_back("SUSYBB160");
  //SUSYhistos.push_back("SUSYBB180");
  //SUSYhistos.push_back("SUSYBB200");
  //SUSYhistos.push_back("SUSYBB250");
  //SUSYhistos.push_back("SUSYBB300");
  //SUSYhistos.push_back("SUSYBB350");
  //SUSYhistos.push_back("SUSYBB400");
  //SUSYhistos.push_back("SUSYBB450");
  ////SUSYhistos.push_back("SUSYBB500");
  ////SUSYhistos.push_back("SUSYBB600");
  ////SUSYhistos.push_back("SUSYBB700");
  ////SUSYhistos.push_back("SUSYBB800");
  ////SUSYhistos.push_back("SUSYBB900");

  std::map<string,TH1F*> mapSUSYhistos;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    mapSUSYhistos.insert( make_pair(SUSYhistos[i], 
				    new TH1F(Form("h%s",SUSYhistos[i].c_str()) ,
					     Form("%s", SUSYhistos[i].c_str()), 
					     nBins , bins.GetArray()) ) 
			  );
  }

  ///////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////


  // pZeta OS, N pZ sideband OS, pZeta SS, N sideband SS, N QCD SS, OS/SS
  TH1F* hParameters   = new TH1F( "hParameters", "" ,26, 0, 26);

  // Open the files
  TFile *fData              
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleRun2012-MuTau-All_run_Open_MuTauStream.root", "READ");  
  TFile *fDataLooseIso  ///////////////////            
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleRun2012-MuTau-All_run_Open_MuTauStream.root", "READ");  
  TFile *fDataEmbedded              
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleRun2012-MuTau-Embedded-All_run_Open_MuTauStream.root", "READ");  
  TFile *fSignalVBF         
    = new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",mH_) ,"READ");  
  TFile *fSignalGGH         
    = new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",mH_),"READ"); 
  TFile *fSignalVH         
    = new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",mH_),"READ");  
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleDYJets-MuTau-8TeV-50-madgraph-PUS7_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleWJets-MuTau-8TeV-madgraph-PUS7_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundW3Jets   
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleW3Jets-MuTau-8TeV-madgraph-PUS7_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleTTJets-MuTau-8TeV-madgraph-PUS7_run_Open_MuTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleOthers-MuTau-8TeV-PUS7_run_Open_MuTauStream.root","READ"); 

  vector<int> hMasses;
  //hMasses.push_back(110);
  //hMasses.push_back(115);
  //hMasses.push_back(120);
  hMasses.push_back(125);
  //hMasses.push_back(130);
  //hMasses.push_back(135);
  //hMasses.push_back(140);
  //hMasses.push_back(145);

  /*
  TFile *fSignalggH110 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",110) ,"READ");  
  TFile *fSignalggH115 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",115) ,"READ");  
  TFile *fSignalggH120 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",120) ,"READ");  
  */
  TFile *fSignalggH125 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",125) ,"READ");  
  /*
  TFile *fSignalggH130 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",130) ,"READ");  
  TFile *fSignalggH135 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",135) ,"READ");  
  TFile *fSignalggH140 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",140) ,"READ");  
  TFile *fSignalggH145 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleGGFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",145) ,"READ");  

  TFile *fSignalqqH110 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",110) ,"READ");  
  TFile *fSignalqqH115 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",115) ,"READ");  
  TFile *fSignalqqH120 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",120) ,"READ");  
  */
  TFile *fSignalqqH125 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",125) ,"READ");  
  /*
  TFile *fSignalqqH130 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",130) ,"READ");  
  TFile *fSignalqqH135 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",135) ,"READ");  
  TFile *fSignalqqH140 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",140) ,"READ");  
  TFile *fSignalqqH145 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVBFH%d-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",145) ,"READ");  
 
  TFile *fSignalVH110 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",110) ,"READ");  
  TFile *fSignalVH115 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",115) ,"READ");  
  TFile *fSignalVH120 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",120) ,"READ");  
  */
  TFile *fSignalVH125 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",125) ,"READ");  
  /*
  TFile *fSignalVH130 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",130) ,"READ");  
  TFile *fSignalVH135 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",135) ,"READ");  
  TFile *fSignalVH140 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",140) ,"READ");  
  TFile *fSignalVH145 =          
    new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTupleVH%d-MuTau-8TeV-pythia-PUS7_run_Open_MuTauStream.root",145) ,"READ");  
  */
  std::map<string,TFile*> mapSUSYfiles;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    mapSUSYfiles.insert( make_pair(SUSYhistos[i], new TFile(Form("/data_CMS/cms/anayak/HTauTauSynchronization/SyncDataCard-v1/OpenNtuples/nTuple%s-MuTau-8TeV-powheg-PUS7_run_Open_MuTauStream.root",SUSYhistos[i].c_str()) ,"READ")  )  );
  }
  

  // choose the analysis: Nominal "", jet up/Down "JetUp/Down" , elec up/down "MuUp/Down" , tau up/down "TauUp/Down"
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
  TTree *dataLooseIso        = LOOSEISO  ? (TTree*)fDataLooseIso->Get(("outTreePtOrd"+postfix_).c_str()) : 0;
  TTree *dataEmbedded        = EMBEDDEDSAMPLES ? (TTree*)fDataEmbedded->Get(treeEmbedded) : 0;
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *signalVH            = addVH ? (TTree*)fSignalVH->Get(tree) : 0;

  // split the DY->ll into l=e/mu and l=tau (MC level) ===> temporary, need fix !!!
  TFile* dummy1 = new TFile("dummy2.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> mu+mu- mu->tau" << endl;
  TTree *backgroundDYMutoTau = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
  cout << "Now copying g/Z -> mu+mu- jet->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau


  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
  cout << backgroundDYMutoTau->GetEntries() << " come from DY->mumu, mu->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->mumu, jet->tau" << endl;

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundW3Jets    = W3JETS ? (TTree*)fBackgroundW3Jets->Get(tree) : 0;
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 

  //TTree *signalGGF110        = (TTree*)fSignalggH110->Get(tree);
  //TTree *signalGGF115        = (TTree*)fSignalggH115->Get(tree);
  //TTree *signalGGF120        = (TTree*)fSignalggH120->Get(tree);
  TTree *signalGGF125        = (TTree*)fSignalggH125->Get(tree);
  //TTree *signalGGF130        = (TTree*)fSignalggH130->Get(tree);
  //TTree *signalGGF135        = (TTree*)fSignalggH135->Get(tree);
  //TTree *signalGGF140        = (TTree*)fSignalggH140->Get(tree);
  //TTree *signalGGF145        = (TTree*)fSignalggH145->Get(tree);
  //
  //TTree *signalVBF110        = (TTree*)fSignalqqH110->Get(tree);
  //TTree *signalVBF115        = (TTree*)fSignalqqH115->Get(tree);
  //TTree *signalVBF120        = (TTree*)fSignalqqH120->Get(tree);
  TTree *signalVBF125        = (TTree*)fSignalqqH125->Get(tree);
  //TTree *signalVBF130        = (TTree*)fSignalqqH130->Get(tree);
  //TTree *signalVBF135        = (TTree*)fSignalqqH135->Get(tree);
  //TTree *signalVBF140        = (TTree*)fSignalqqH140->Get(tree);
  //TTree *signalVBF145        = (TTree*)fSignalqqH145->Get(tree);
  //
  //TTree *signalVH110        = (TTree*)fSignalVH110->Get(tree);
  //TTree *signalVH115        = (TTree*)fSignalVH115->Get(tree);
  //TTree *signalVH120        = (TTree*)fSignalVH120->Get(tree);
  TTree *signalVH125        = (TTree*)fSignalVH125->Get(tree);
  //TTree *signalVH130        = (TTree*)fSignalVH130->Get(tree);
  //TTree *signalVH135        = (TTree*)fSignalVH135->Get(tree);
  //TTree *signalVH140        = (TTree*)fSignalVH140->Get(tree);
  //TTree *signalVH145        = (TTree*)fSignalVH145->Get(tree);

  std::map<string,TTree*> mapSUSYtrees;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    TTree* treeSusy = (mapSUSYfiles.find(SUSYhistos[i]))->second ? (TTree*)((mapSUSYfiles.find(SUSYhistos[i]))->second)->Get(tree) : 0;
    mapSUSYtrees.insert( make_pair( SUSYhistos[i], treeSusy )) ;
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////





  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  ///// LEPT PT ///////
  TCut lpt("ptL1>20 && isPFMuon && isTightMuon");
  TCut tpt("ptL2>20");

  if(selection_.find("High")!=string::npos)
    tpt = tpt&&TCut("ptL2>40");
  else if(selection_.find("Low")!=string::npos)
    tpt = tpt&&TCut("ptL2<40");

  ////// TAU ISO //////
  TCut tiso("tightestHPSMVAWP>=0"); 
  //TCut ltiso("tightestHPSMVAWP>-99");
  TCut ltiso("hpsMVA>0.7");  //loose tau iso changed for 2012
  TCut mtiso("hpsMVA>0.7");  //0.0 ---> 0.7 (from Josh)

  ////// MU ISO ///////
  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

 
  ////// EVENT WISE //////
  TCut lveto("muFlag!=1"); //<----------------------------------
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));
  TCut hltevent("pairIndex<1 && HLTx==1");
  TCut hltmatch("HLTmatch==1");


  ////// CATEGORIES ///
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut ExactOneJet20("nJets20==1");
  TCut ExactOneJet("nJets30==1");

  //TCut vbf("pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1");     // <--- BASELINE
  //TCut vbf("pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1 && jet1PUWP>0.5 && jet2PUWP>0.5 && (jetVetoPUWP>0.5 && jetVetoPUWP<0)");
  /* //old
  TCut vbf("pt1>30 && pt2>30 && (ptVeto<30 || isVetoInJets!=1) && MVAvbf>0.50");
  TCut vbfLoose("pt1>30 && pt2>30 && (ptVeto<30 || isVetoInJets!=1) && MVAvbf>-0.30"); // <--- -0.30
  TCut vbfLooseQCD("pt1>20 && pt2>20 && (ptVeto<30 || isVetoInJets!=1) && MVAvbf>0");
  */
  TCut vbf("pt1>30 && pt2>30 && isVetoInJets!=1 && MVAvbf>0.50"); 
  TCut vbfLoose("pt1>30 && pt2>30 && isVetoInJets!=1 && MVAvbf>-0.30"); // <--- -0.30 
  TCut vbfLooseQCD("pt1>20 && pt2>20 && isVetoInJets!=1 && MVAvbf>0"); 

  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");

  TCut boost("pt1>30 && nJets20BTagged<1");
  boost = boost && !vbf /*&& !vh*/;

  TCut boost2("pt1>100 && pt1<150 && !(pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1)");  

  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut nobTag("nJets30<2 && nJets20BTagged==0");
  TCut bTagLoose("nJets20>=2");

  //TCut novbf = !vbf /*&& !vh*/ && !boost && !bTag;
  TCut novbf("nJets30<=0 && nJets20BTagged<=0"); 

  TCut sbin; TCut sbinEmbedding; TCut sbinEmbeddingPZetaRel; TCut sbinPZetaRel; TCut sbinSS; TCut sbinPZetaRelSS; TCut sbinPZetaRev; TCut sbinPZetaRevSS; TCut sbinSSaIso; TCut sbinSSlIso; TCut sbinSSlIso2; TCut sbinSSlIso3;
  TCut sbinSSaIsoLtiso;
  TCut sbinSSltiso;
  TCut sbinLtiso;
  TCut sbinMtiso;
  TCut sbinNSaIsoLtiso;

  TCut sbinInclusive;
  sbinInclusive            = lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch;
  TCut sbinEmbeddingInclusive;
  sbinEmbeddingInclusive   = lpt && tpt && tiso && liso && lveto && OS && pZ                         ;
  TCut sbinPZetaRelEmbeddingInclusive;
  sbinPZetaRelEmbeddingInclusive = lpt && tpt && tiso && liso && lveto && OS                         ;
  TCut sbinPZetaRelSSInclusive;
  sbinPZetaRelSSInclusive  = lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch;
  TCut sbinPZetaRelInclusive;
  sbinPZetaRelInclusive    = lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch;
  TCut sbinSSInclusive;
  sbinSSInclusive          = lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch;
  TCut sbinSSaIsoInclusive;
  sbinSSaIsoInclusive      = lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch;
  TCut sbinPZetaRelSSaIsoInclusive;
  sbinPZetaRelSSaIsoInclusive = lpt && tpt && tiso && laiso&& lveto && SS && hltevent && hltmatch;
  TCut sbinSSaIsoLtisoInclusive;
  sbinSSaIsoLtisoInclusive =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch;
  TCut sbinSSltisoInclusive;
  sbinSSltisoInclusive     =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch;
  TCut sbinLtisoInclusive;
  sbinLtisoInclusive       =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch;
  TCut sbinMtisoInclusive;
  sbinMtisoInclusive       =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch;
  TCut sbinPZetaRelLtisoInclusive;
  sbinPZetaRelLtisoInclusive =lpt && tpt && ltiso&& liso && lveto && OS        && hltevent && hltmatch;
  TCut sbinNSaIsoLtisoInclusive; 
  sbinNSaIsoLtisoInclusive =  lpt && tpt && ltiso&& laiso&& lveto && pZ  && hltevent && hltmatch;

  if(selection_.find("inclusive")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                         ;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                               ;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch;
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && oneJet;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && oneJet;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && oneJet;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && twoJets;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && twoJets;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && twoJets;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && vbf;
  }
  else if(selection_.find("vh")!=string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && vh;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && vh;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && vh;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && vh;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && vh;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && vh;
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && vh;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && novbf;
  }
  else if(selection_.find("boost")!=string::npos && selection_.find("boost2")==string::npos){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && boost;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && boost;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && boost;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && boost;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && boost;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && boost;
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && boost;
  }
  else if(selection_.find("boost2")!=string::npos ){
    sbin                   =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && boost2;
    sbinEmbedding          =  lpt && tpt && tiso && liso && lveto && OS && pZ                          && boost2;
    sbinEmbeddingPZetaRel  =  lpt && tpt && tiso && liso && lveto && OS                                && boost2;
    sbinPZetaRev           =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && boost2;
    sbinPZetaRel           =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && boost2;
    sbinPZetaRevSS         =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && boost2;
    sbinSS                 =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinPZetaRelSS         =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && boost2;
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && boost2;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost2;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost2;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && boost2;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && bTag;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && bTag;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && bTag;
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
    sbinSSaIso             =  lpt && tpt && tiso && laiso&& lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinSSlIso             =  lpt && tpt && tiso && lliso&& lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && nobTag;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && nobTag;
    sbinNSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto &&       pZ  && hltevent && hltmatch && nobTag;
  }


  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << "******** Extrapolation factors for Z->tautau normalization ********" << endl;
  // inclusive DY->tautau:
  TH1F* hExtrap = new TH1F("hExtrap","",nBins , bins.GetArray());
  backgroundDYTauTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinInclusive);
  float ExtrapDYInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->tautau = " << ExtrapDYInclusive << endl; 

  backgroundDYMutoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinInclusive);
  float ExtrapLFakeInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->mumu, mu->tau = " << ExtrapLFakeInclusive << endl;

  backgroundDYJtoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinInclusive);
  float ExtrapJFakeInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->mumu, j->tau = " << ExtrapJFakeInclusive << endl;

  TCut sbinInclusiveEmbeddedCut = sbinEmbeddingInclusive;
  TCut sbinEmbeddedCut          = sbinEmbedding;

  // if VBF, minimize ttbar contamination asking for 0 btag jet:
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    sbinInclusiveEmbeddedCut = sbinInclusiveEmbeddedCut && TCut("nJets20BTagged<999");   /// <--------  1
    sbinEmbeddedCut          = sbinEmbeddedCut          && TCut("nJets20BTagged<999");
  }

  dataEmbedded->Draw(variable+">>hExtrap", "(HLTTau*HLTMu*embeddingWeight)"*sbinInclusiveEmbeddedCut);
  float ExtrapEmbedDen =  hExtrap->Integral();
  hExtrap->Reset();
  dataEmbedded->Draw(variable+">>hExtrap", "(HLTTau*HLTMu*embeddingWeight)"*sbinEmbeddedCut);
  float ExtrapEmbedNum =  hExtrap->Integral();
  hExtrap->Reset();

  ExtrapolationFactorZ = ExtrapEmbedNum/ExtrapEmbedDen;

  ErrorExtrapolationFactorZ = TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen);
  cout << "Extrap. factor using embedded sample: " << ExtrapolationFactorZ << " +/- " << ErrorExtrapolationFactorZ << endl;
  backgroundDYTauTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbin);
  float ExtrapolationFactorMadGraph = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000./ExtrapDYInclusive;
  cout << "MadGraph prediction = " << ExtrapolationFactorMadGraph << endl;
  ExtrapolationFactorZDataMC  = ExtrapolationFactorZ/ExtrapolationFactorMadGraph;
  cout << " ==> data/MC = " << ExtrapolationFactorZDataMC << endl;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << "******** Extrapolation factors for QCD normalization ********" << endl;
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&pZ));
  float ExtrapSSWinSignalRegionMC   = hExtrap->Integral();
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapSSWinSidebandRegionMC = hExtrap->Integral();
  float ExtrapscaleFactorSS         = ExtrapSSWinSignalRegionMC>0 ? ExtrapSSWinSidebandRegionMC/ExtrapSSWinSignalRegionMC : 1.0;
  cout << " Extrapolation factor W SS (inclusive) " << ExtrapscaleFactorSS << endl;

  hExtrap->Reset();
  backgroundTTbar->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapttbarExtrSS    = hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;

  /*
  hExtrap->Reset();
  backgroundTTbar->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float ExtrapttbarExtrSSBtag = hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float ExtrapwjetsExtrSSBtag = hExtrap->Integral()*Lumi/1000*hltEff_;
  hExtrap->Reset();
  backgroundOthers->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float ExtrapothersExtrSSBtag = hExtrap->Integral()*Lumi/1000*hltEff_;
  hExtrap->Reset();
  data->Draw(variable+">>hExtrap",(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float dataSSBtag = hExtrap->Integral();
  float scaleFactorTTSS = ExtrapttbarExtrSSBtag>0 ? dataSSBtag/ExtrapttbarExtrSSBtag : 1.0;
  //if( (dataSSBtag-ExtrapttbarExtrSSBtag)/TMath::Sqrt(dataSSBtag)>1) scaleFactorTTSS = 1.0;
  cout << "Normalizing TTbar from sideband: " << ExtrapttbarExtrSSBtag << " events expected from TTbar, " << ExtrapwjetsExtrSSBtag
       << " expected from WJets, "  << ", expected from others " << ExtrapothersExtrSSBtag << ", observed " << dataSSBtag << endl;
  cout << "====> scale factor for SS ttbar is " << scaleFactorTTSS << endl;
  ExtrapttbarExtrSS *= scaleFactorTTSS;
  */

  hExtrap->Reset();
  backgroundOthers->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapothersExtrSS   = hExtrap->Integral()*Lumi/1000*hltEff_;
  hExtrap->Reset();
  backgroundDYJtoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapdyjtotauExtrSS = hExtrap->Integral()*Lumi*lumiCorrFactor/1000*hltEff_;

  hExtrap->Reset();
  data->Draw(variable+">>hExtrap", sbinPZetaRelSSInclusive&&apZ);
  float ExtrapSSWinSignalRegionDATA = hExtrap->Integral();
  cout << "Extrapolation for QCD (inclusive): total data events in sideband " << ExtrapSSWinSignalRegionDATA << endl;
  ExtrapSSWinSignalRegionDATA -= ExtrapttbarExtrSS;
  ExtrapSSWinSignalRegionDATA -= ExtrapothersExtrSS;
  ExtrapSSWinSignalRegionDATA -= ExtrapdyjtotauExtrSS;
  ExtrapSSWinSignalRegionDATA /= ExtrapscaleFactorSS;
  cout << "Extrapolation for QCD (inclusive): W+jets in SS signal region (inclusive) is estimated to be " << ExtrapSSWinSignalRegionDATA << endl;

  hExtrap->Reset();
  data->Draw(variable+">>hExtrap", sbinSSInclusive);
  float SSeventsExtrap = hExtrap->Integral();
  cout << "Extrapolation for SS events in data (inclusive) " << hExtrap->GetEntries() << endl;
  cout << "Subtracting W+jets (SS)..." << endl;
  SSeventsExtrap  -= ExtrapSSWinSignalRegionDATA;

  hExtrap->Reset();
  backgroundTTbar->Draw(variable+">>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;//*scaleFactorTTSS;
  hExtrap->Reset();
  backgroundDYMutoTau->Draw(variable+">>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi*lumiCorrFactor/1000*hltEff_*MutoTauCorrectionFactor;

  hExtrap->Reset();
  backgroundDYJtoTau->Draw(variable+">>hExtrap", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi*lumiCorrFactor/1000*hltEff_*JtoTauCorrectionFactor;
  hExtrap->Reset();

  SSeventsExtrap *= OStoSSRatioQCD;

  dataLooseIso->Draw(variable+">>hExtrap", sbinSSaIsoInclusive);
  float SSeventsExtrapAiso = hExtrap->GetEntries();
  SSIsoToSSAIsoRatioQCD = SSeventsExtrap/SSeventsExtrapAiso ;
  cout << "The extrapolation factor Iso>0.3 / Iso<0.1 is " << SSIsoToSSAIsoRatioQCD << endl;

  cout << "************** END extrapolation *******************" << endl;
  delete hExtrap;
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////
  // estimate the W+jets in the inclusive bin using pZeta extrapolation
  
  TH1F* hWMtInc = new TH1F("hWMtInc","",nBins , bins.GetArray()); 
 
  ///////////////////////////////////////// Doing OS first... 
  hWMtInc->Reset(); 
  backgroundWJets->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelInclusive&&pZ)); 
  cout << "Using  " << hWMtInc->GetEntries() << " entries from the W+jets Inclusive OS sample" << endl; 
  float OSIncWinSignalRegionMC   = hWMtInc->Integral()*Lumi*hltEff_/1000.; 
  hWMtInc->Reset(); 
  backgroundWJets->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelInclusive&&apZ)); 
  float OSIncWinSidebandRegionMC = hWMtInc->Integral()*Lumi*hltEff_/1000.; 
  float scaleFactorOSInc = OSIncWinSignalRegionMC>0 ? OSIncWinSidebandRegionMC/OSIncWinSignalRegionMC : 1.0; 
  
  hWMtInc->Reset(); 
  cout << "Estimating cobtribution from Ztt, ttbar and others in OS low pZeta tail..." << endl; 
  backgroundTTbar->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelInclusive&&apZ)); 
  float ttbarExtrOSInc  = hWMtInc->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio; 
  hWMtInc->Reset(); 
  cout << "Contribution from inclusive ttbar in OS is " << ttbarExtrOSInc << endl; 
 
  hWMtInc->Reset(); 
  backgroundOthers->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelInclusive&&apZ)); 
  float othersExtrOSInc = hWMtInc->Integral()*Lumi*hltEff_/1000.; 
  cout << "Contribution from inclusive di-boson in OS is " << othersExtrOSInc << endl; 
  hWMtInc->Reset(); 

  data->Draw(variable+">>hWMtInc", (sbinPZetaRelInclusive&&apZ)); 
  float OSIncWinSignalRegionDATA = hWMtInc->Integral(); 
  cout << "Selected incl. events in data in low pZeta/low Mt tail " << OSIncWinSignalRegionDATA << endl; 
  OSIncWinSignalRegionDATA -= ttbarExtrOSInc; 
  OSIncWinSignalRegionDATA -= othersExtrOSInc; 
  OSIncWinSignalRegionDATA /= scaleFactorOSInc; 
  cout << "W+jets in incl. signal region is estimated to be "   
       << OSIncWinSignalRegionDATA*scaleFactorOSInc << "/" << scaleFactorOSInc << " = "  
       << OSIncWinSignalRegionDATA << endl;
  cout << "  ===> the MC prediction was " << OSIncWinSignalRegionMC << endl; 

  ///////////////////////////////////////// Doing SS last...
  hWMtInc->Reset();
  backgroundWJets->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&pZ));
  cout << "Using  " << hWMtInc->GetEntries() << " entries from the SS incl W+jets sample" << endl;
  float SSIncWinSignalRegionMC   = hWMtInc->Integral()*Lumi*hltEff_/1000.;
  hWMtInc->Reset();
  backgroundWJets->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float SSIncWinSidebandRegionMC = hWMtInc->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorSSInc = SSIncWinSignalRegionMC>0 ? SSIncWinSidebandRegionMC/SSIncWinSignalRegionMC : 1.0;
  
  hWMtInc->Reset();
  backgroundTTbar->Draw(variable+">>hWMtInc","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSSInclusive&&apZ));
  float ttbarExtrSSInc = hWMtInc->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio;
  hWMtInc->Reset();
  cout << "Contribution from ttbar in incl SS is " << ttbarExtrSSInc << endl;
  
  data->Draw(variable+">>hWMtInc",(sbinPZetaRelSSInclusive&&apZ));
  float SSIncWinSignalRegionDATA = hWMtInc->Integral();
  cout << "Selected incl events in data in low pZeta/low Mt tail " << SSIncWinSignalRegionDATA << endl;
  SSIncWinSignalRegionDATA -= ttbarExtrSSInc;
  SSIncWinSignalRegionDATA /= scaleFactorSSInc;
  cout << "W+jets in incl SS signal region is estimated to be "  
       << SSIncWinSignalRegionDATA*scaleFactorSSInc << "/" << scaleFactorSSInc << " = " 
       << SSIncWinSignalRegionDATA << endl;
  cout << "  ===> the MC prediction was " << SSIncWinSignalRegionMC << endl;

  //////////////////////////////////////////////////////////////////  
  // estimate the W+jets in the selection bin using pZeta extrapolation

  //TH1F* hWMt = new TH1F("hWMt","",1,-10,10);
  TH1F* hWMt = new TH1F("hWMt","",nBins , bins.GetArray());

  ///////////////////////////////////////// Doing OS first...
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the W+jets OS sample" << endl;
  float OSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float OSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorOS = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;

  if(useMt)
    cout << "Extrapolation factor for W OS : P(MtMVA>" << antiWsdb << ")/P(MtMVA<" << antiWsgn << ") ==> " << scaleFactorOS << endl;
  else
    cout << "Extrapolation factor for W OS : P(pZetaMVA<- "<< antiWsdb << ")/P(pZetaMVA>"<< antiWsgn << ") ==> " << scaleFactorOS << endl;    
 
  hWMt->Reset();
  cout << "Estimating cobtribution from Ztt, ttbar and others in OS low pZeta tail..." << endl;
  backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float ttbarExtrOS  = hWMt->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio;
  hWMt->Reset();
  cout << "Contribution from ttbar in OS is " << ttbarExtrOS << endl;

  hWMt->Reset();
  backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float othersExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in OS is " << othersExtrOS << endl;
  hWMt->Reset();
  /*
  backgroundDYTauTau->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float dytautauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->tautau in OS is " << dytautauExtrOS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float dyjtotauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->mumu, jet->tau in OS is " << dyjtotauExtrOS << endl;
  hWMt->Reset();
  backgroundDYMutoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRel&&apZ));
  float dymutotauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->mumu, mu->tau in OS is " << dymutotauExtrOS << endl;
  hWMt->Reset();
  */
  data->Draw(variable+">>hWMt", sbinPZetaRev);
  float OSWinSignalRegionDATA = hWMt->Integral();
  cout << "Selected events in data in low pZeta/low Mt tail " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= ttbarExtrOS;
  OSWinSignalRegionDATA -= othersExtrOS;
  //OSWinSignalRegionDATA -= dytautauExtrOS;
  //OSWinSignalRegionDATA -= dyjtotauExtrOS;
  //OSWinSignalRegionDATA -= dymutotauExtrOS;
  OSWinSignalRegionDATA /= scaleFactorOS;
  cout << "W+jets in signal region is estimated to be "  
       << OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS << " = " 
       << OSWinSignalRegionDATA <<  " +/- " << sqrt(OSWinSignalRegionDATA/scaleFactorOS)/scaleFactorOS << endl;
  cout << "  ===> the MC prediction was " << OSWinSignalRegionMC << endl;

  hParameters->SetBinContent(1, 1./scaleFactorOS );
  hParameters->SetBinContent(2, OSWinSignalRegionDATA*scaleFactorOS );

  ///////////////////////////////////////// Doing SS last...
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSS&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the SS W+jets sample" << endl;
  float SSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSS&&apZ));
  float SSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorSS = SSWinSignalRegionMC>0 ? SSWinSidebandRegionMC/SSWinSignalRegionMC : 1.0;
  if(selection_.find("boostHigh")!=string::npos)scaleFactorSS = scaleFactorSSInc;
  
  if(useMt)
    cout << "Extrapolation factor for W SS : P(MtMVA>" << antiWsdb << ")/P(MtMVA<" << antiWsgn << ") ==> " << scaleFactorSS << endl;
  else
    cout << "Extrapolation factor for W SS : P(pZetaMVA<- "<< antiWsdb << ")/P(pZetaMVA>"<< antiWsgn << ") ==> " << scaleFactorSS << endl;    

  hWMt->Reset();
  cout << "Estimating cobtribution Ztt,from ttbar and others in SS low pZeta tail..." << endl;
  backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSS&&apZ));
  float ttbarExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio;
  hWMt->Reset();
  cout << "Contribution from ttbar in SS is " << ttbarExtrSS << endl;

  
  hWMt->Reset();
  backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSS&&apZ));
  float othersExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in SS is " << othersExtrSS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*(sbinPZetaRelSS&&apZ));
  float dyjtotauExtrSS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->mumu, jet->tau in SS is " << dyjtotauExtrSS << endl;
  hWMt->Reset();
  

  data->Draw(variable+">>hWMt",sbinPZetaRevSS);
  float SSWinSignalRegionDATA = hWMt->Integral();
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
  if(dataLooseIso){
    samples.push_back("LooseIso");
    samples.push_back("AntiIso");
  }
  samples.push_back("Others");
  samples.push_back("SS");
  samples.push_back("WJets");
  if(backgroundW3Jets)
    samples.push_back("W3Jets");
  samples.push_back("TTbar");
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


  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]         = data;
  tMap["LooseIso"]     = dataLooseIso;
  tMap["AntiIso"]      = dataLooseIso;
  tMap["Embedded"]     = dataEmbedded;
  tMap["DYToTauTau"]   = backgroundDYTauTau;
  tMap["DYMutoTau"]    = backgroundDYMutoTau;
  tMap["DYJtoTau"]     = backgroundDYJtoTau;
  tMap["WJets"]        = backgroundWJets;
  tMap["W3Jets"]       = backgroundW3Jets;
  tMap["Others"]       = backgroundOthers;
  tMap["TTbar"]        = backgroundTTbar;
  tMap["SS"]           = data;

  //tMap["ggH110"]       = signalGGF110;
  //tMap["ggH115"]       = signalGGF115;
  //tMap["ggH120"]       = signalGGF120;
  tMap["ggH125"]       = signalGGF125;
  //tMap["ggH130"]       = signalGGF130;
  //tMap["ggH135"]       = signalGGF135;
  //tMap["ggH140"]       = signalGGF140;
  //tMap["ggH145"]       = signalGGF145;
  //
  //tMap["qqH110"]       = signalVBF110;
  //tMap["qqH115"]       = signalVBF115;
  //tMap["qqH120"]       = signalVBF120;
  tMap["qqH125"]       = signalVBF125;
  //tMap["qqH130"]       = signalVBF130;
  //tMap["qqH135"]       = signalVBF135;
  //tMap["qqH140"]       = signalVBF140;
  //tMap["qqH145"]       = signalVBF145;
  //
  //tMap["VH110"]       = signalVH110;
  //tMap["VH115"]       = signalVH115;
  //tMap["VH120"]       = signalVH120;
  tMap["VH125"]       = signalVH125;
  //tMap["VH130"]       = signalVH130;
  //tMap["VH135"]       = signalVH135;
  //tMap["VH140"]       = signalVH140;
  //tMap["VH145"]       = signalVH145;

  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    TTree* susyTree = (mapSUSYtrees.find( SUSYhistos[i] ))->second ;
    tMap[SUSYhistos[i]] = susyTree ;
  }




  std::map<TString,Float_t> vMap;


  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << "Dealing with sample " << samples[iter] << endl;
    
    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,"" , nBins , bins.GetArray());

    TTree* currentTree = 0;
    
    if((it->first).find("SS")!=string::npos){
      
      cout << "Remove W contamination from SS data sample ... " << endl;
      currentTree = (it->second);

      float error2OnQCD = 0.0;
      
      TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
      hHelp->Reset();
      currentTree->Draw(variable+">>hHelp", sbinSS);
      int SSevents = hHelp->GetEntries();
      cout << "Selected SS events in data " << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);

      hHelp->Reset();
      backgroundWJets->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from W+jets (from " << hHelp->GetEntries() << " entries)" << endl;
      float sFWSS = ( selection_.find("novbf") !=string::npos || 
		      selection_.find("bTag")  !=string::npos || 
		      selection_.find("boost") !=string::npos || 
		      selection_.find("oneJet") !=string::npos || 
		      selection_.find("twoJets") !=string::npos || 
		      selection_.find("inclusive")!=string::npos) ? 
	SSWinSignalRegionDATA/SSWinSignalRegionMC : WcorrectionFactorSS; // from the extrapolation factor DATA/MC

      if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) 
	sFWSS *= VbfExtrapolationFactorW;
      else if(selection_.find("boost")!=string::npos)
	sFWSS *= BoostExtrapolationFactorW;

      hHelp->Scale(sFWSS*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from W+jets by extrapolating" << endl;
      cout << " ==> removing W+jets from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow( hHelp->Integral()/hHelp->GetEntries(), 2)*hHelp->GetEntries(); // error on MC W+jets SS events
      error2OnQCD +=  pow(WcorrectionFactorSS*0.06,2)*pow(hHelp->GetEntries(),2);        // error on W+jets extrapolation factor ==> 6% according to Artur
      cout << sqrt(error2OnQCD) << " <==  W" << endl;      

      hHelp->Reset();
      backgroundTTbar->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio << " SS events from TTbar (from " << hHelp->GetEntries() << " entries)" << endl;
      hHelp->Scale(1.0*Lumi/1000*hltEff_*TTxsectionRatio);
      cout << "We estimate " << hHelp->Integral() << " SS events from TTbar" << endl;
      cout << " ==> removing TTbar from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC TTbar SS events
      cout << sqrt(error2OnQCD) << " <== W + TTb" << endl;      

      hHelp->Reset();
      backgroundDYMutoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from DY->mumu, mu->jet" << endl;
      hHelp->Scale(MutoTauCorrectionFactor*Lumi*lumiCorrFactor/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->mumu, mu->tau" << endl;
      cout << " ==> removing DY->mumu, mu->tau from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->mumu, mu->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1)" << endl;      

      hHelp->Reset();
      backgroundDYJtoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi*lumiCorrFactor/1000*hltEff_ << " SS events from DY->mumu, jet->tau" << endl;
      hHelp->Scale(JtoTauCorrectionFactor*Lumi*lumiCorrFactor/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->mumu, jet->tau" << endl;
      cout << " ==> removing DY->mumu, mu->jet from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->mumu, jet->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1,2)" << endl;      

      //  OS/SS ratio
      h1->Scale(OStoSSRatioQCD);
      cout << "After removing the expected contribution from W+jets and rescaling by " << OStoSSRatioQCD << " we expect " 
	   << h1->Integral() << " events from QCD processes" << endl;

      hParameters->SetBinContent(5, SSevents);
      hParameters->SetBinContent(6, h1->Integral()/SSevents);

      cout << "Total unceratinty from bkg subtraction in SS region is " << sqrt(error2OnQCD) << endl;
      float totalRelErrorOnQCD = 0.02 + sqrt(error2OnQCD)/h1->Integral(); //0.02 ==> uncertainty on OS/SS ratio
      hParameters->SetBinContent(7,totalRelErrorOnQCD);

      ////////////////////////////////////////////////

      hParameters->SetBinContent(8,ExtrapolationFactorZ);
      hParameters->SetBinContent(9,ErrorExtrapolationFactorZ);
      hParameters->SetBinContent(10,ExtrapolationFactorZDataMC);
      hParameters->SetBinContent(11,SSIsoToSSAIsoRatioQCD);


    }
    else{

      currentTree = (it->second);

      if((it->first).find("Embed")==string::npos){

	if((it->first).find("DYToTauTau")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbinPZetaRelInclusive);  
	  float madgraphNoMEtCut = h1->Integral();
	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbinInclusive);
	  madgraphMEtCutEff = h1->Integral()/madgraphNoMEtCut;
	  cout << "Efficiency of antiW cut on madgraph " << madgraphMEtCutEff << endl;
	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	}
	else if((it->first).find("W3Jets")!=string::npos){
	  
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  hW3JetsKeys = new TH1Keys("hW3JetsKeys","W+3jets smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hW3JetsKeys", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  cout << "Keys for W3Jets filled with integral " << hW3JetsKeys->Integral() << " and entries " << hW3JetsKeys->GetEntries() << endl;

	  cout << "Filling histo with loose taus... if vbf, apply loose vbf cut" << endl;
	  h1->Reset();
	  if((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || selection_.find("twoJets")!=string::npos){
	    vbfLoose = twoJets;  //changed accrding to twiki for 2012
	    if( selection_.find("twoJets")!=string::npos ){
	      vbfLoose = twoJets;
	      vbf      = twoJets;
	    }

	    //currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinMtisoInclusive&&vbfLoose)); // <------- sbinLtisoInclusive LOOSE TAU ID?
	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinInclusive&&vbfLoose)); //shape taken from W+3jets (>= 2 jets) without using loose tau isolation
	    hW3JetsLooseTauIso->Add(h1,1.0);
	    
	    // NORMALIZE W3JETS ON DATA

	    //TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());

	    // SIDEBAND OS: get extrap factor
	    hHelp->Reset();
	    backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ&&vbfLoose));
	    cout << hHelp->Integral() << endl;
	    float WJetsVBFSdb    = hHelp->Integral();
	    float WJetsVBFSdbInt = hHelp->GetEntries();
	    hHelp->Reset();
	    backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&pZ&&vbfLoose));
	    float WJetsVBFSgn    = hHelp->Integral();
	    float WJetsVBFSgnInt = hHelp->GetEntries();
	    cout << "Wjets MC in VBF(rel): using " << WJetsVBFSdbInt << " events from sideband and " << WJetsVBFSgnInt << " from signal region" << endl;
	    float ExtrapFactorWJets = WJetsVBFSdb/WJetsVBFSgn;
	    cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorWJets << endl;
	    hHelp->Reset();
            backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&vbfLoose));
            cout << hHelp->Integral() << endl;
            float WJetsVBFAll    = hHelp->Integral();
            float WJetsVBFAlllInt = hHelp->GetEntries();

	    // SIDEBAND2 OS: get extrap factor (2)
	    hHelp->Reset();
            backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbfLoose));
            cout << hHelp->Integral() << endl;
            float WJetsVBFSdb2    = hHelp->Integral();
            float WJetsVBFSdbInt2 = hHelp->GetEntries();
            hHelp->Reset();
            backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&pZ&&vbfLoose));
            float WJetsVBFSgn2    = hHelp->Integral();
            float WJetsVBFSgnInt2 = hHelp->GetEntries();
            cout << "Wjets MC in VBF(rel): using " << WJetsVBFSdbInt2 << " events from sideband (2) and " << WJetsVBFSgnInt2 << " from signal region" << endl;
            float ExtrapFactorWJets2 = WJetsVBFSdb2/WJetsVBFSgn2;
            cout << " ==> ratio sdb2/sgn = " <<  ExtrapFactorWJets2 << endl;


	    // SIDEBAND SS: get extrap fact SS
	    hHelp->Reset();
            backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbfLoose));
	    cout << hHelp->Integral() << endl;
            float WJetsVBFSdbSS    = hHelp->Integral();
            float WJetsVBFSdbIntSS = hHelp->GetEntries();
            hHelp->Reset();
            backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbfLoose));
	    float WJetsVBFSgnSS    = hHelp->Integral();
            float WJetsVBFSgnIntSS = hHelp->GetEntries();
            cout << "Wjets MC (SS) in VBF(rel): using " << WJetsVBFSdbIntSS << " events from sideband SS (2) and " << WJetsVBFSgnIntSS << " from signal region" << endl;
            float ExtrapFactorWJetsSS = WJetsVBFSdbSS/WJetsVBFSgnSS;
	    cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorWJetsSS << endl;



	    // now, normalize to sidebands
	    cout << "#######  Start normalization OS from sidebands #######" << endl;
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&vbf);
	    float DataVBFSdb = hHelp->Integral();
	    cout << "In VBF region, I get " << DataVBFSdb << " events in the high Mt sdb" << endl;
	    /*
	    hHelp->Reset();
	    backgroundDYTauTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dytotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dytotauVBFSdb << " events in the high Mt sdb from DYtotautau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dymutotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dymutotauVBFSdb << " events in the high Mt sdb from DYmutotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dyjtotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSdb << " events in the high Mt sdb from DYjtotau" << endl;
	    hHelp->Reset();
	    */
	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float othersVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSdb << " events in the high Mt sdb from others" << endl;
	    hHelp->Reset();

	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float TTbarVBFSdb = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    cout << "In VBF region, I expect " << TTbarVBFSdb << " events in the high Mt sdb from TTbar" << endl;

	    cout << "Subtracting the backgrounds from the sideband..." << endl;
	    DataVBFSdb -= TTbarVBFSdb;
	    //DataVBFSdb -= (DataVBFSdbBtag-WJetsVBFSdbBtag-OthersVBFSdbBtag/ratiobTagToAny);
	    //DataVBFSdb -= QCDVBFSdb;
	    //DataVBFSdb -= dytotauVBFSdb;
	    //DataVBFSdb -= dymutotauVBFSdb;
	    //DataVBFSdb -= dyjtotauVBFSdb;
	    DataVBFSdb -= othersVBFSdb;
	    float normalization = ExtrapFactorWJets2>0 ? DataVBFSdb/ExtrapFactorWJets2 : -99;
	    cout << "Estimation of W+jets in the VBF category is " << normalization << endl;

	    hW3JetsLooseTauIso->Scale(normalization/hW3JetsLooseTauIso->Integral());

	    cout << "#######  Start normalization SS from sidebands #######" << endl;
	    
	    hHelp->Reset();
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&apZ2&&vbf);
            float DataVBFSdbSS = hHelp->Integral();
            cout << "In VBF SS region, I get " << DataVBFSdbSS << " events in the high Mt sdb" << endl;

	    hHelp->Reset();
	    backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float dymutotauVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dymutotauVBFSdbSS << " events in the high Mt sdb SS from DYmutotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float dyjtotauVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSdbSS << " events in the high Mt sdb SS from DYjtotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float othersVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSdbSS << " events in the high Mt sdb SS from others" << endl;
	    hHelp->Reset();

            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
            float TTbarVBFSdbSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In VBF SS region, I expect " << TTbarVBFSdbSS << " events in the high Mt sdb from TTbar" << endl;
            hHelp->Reset();

	    cout << "Now subtracting the expected bkg from the SS signal region" << endl;
	    hHelp->Reset();
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&pZ&&vbf);
            float DataVBFSgnSS = hHelp->Integral();

	    hHelp->Reset();
	    backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float dymutotauVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dymutotauVBFSgnSS << " events in the sgn SS from DYmutotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float dyjtotauVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSgnSS << " events in the sgn SS from DYjtotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float othersVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSgnSS << " events in the sgn SS from others" << endl;
	    hHelp->Reset();

            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
            float TTbarVBFSgnSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In VBF SS region, I expect " << TTbarVBFSgnSS << " events in the sgn from TTbar" << endl;
            hHelp->Reset();

	    float nonWbackgroundSS = TTbarVBFSgnSS + othersVBFSgnSS + dyjtotauVBFSgnSS + dymutotauVBFSgnSS ;
	    //float WbackgroundSS    = (DataVBFSdbSS - TTbarVBFSdbSS /*-QCDVBFSdbSS*/ -dymutotauVBFSdbSS-dyjtotauVBFSdbSS-othersVBFSdbSS)/ExtrapFactorWJetsSS;
	    float WbackgroundSS    = normalization*(SSIncWinSignalRegionDATA/OSIncWinSignalRegionDATA); //get SS/OS from inclusive
            float QCDSgnSS         = DataVBFSgnSS - nonWbackgroundSS - WbackgroundSS;
            cout << "In VBF SS region, I measure " << DataVBFSgnSS << endl;
            cout << "The bkg estimation for QCD is therefore: " << DataVBFSgnSS << " - " << nonWbackgroundSS << " - " << WbackgroundSS << " = "
                 << QCDSgnSS << endl;

	    hParameters->SetBinContent(13, ExtrapFactorWJets);
	    hParameters->SetBinContent(14, DataVBFSdb);
	    hParameters->SetBinContent(15, TTbarVBFSdb);
	    //hParameters->SetBinContent(16, QCDVBFSdb);
	    hParameters->SetBinContent(17, ExtrapFactorWJetsSS);
	    hParameters->SetBinContent(18, DataVBFSdbSS);
	    hParameters->SetBinContent(19, TTbarVBFSdbSS);
	    //hParameters->SetBinContent(20, QCDVBFSdbSS);
	    hParameters->SetBinContent(21, DataVBFSgnSS);
            hParameters->SetBinContent(22, QCDSgnSS);
	    hParameters->SetBinContent(23, ExtrapFactorWJets2);
            //hParameters->SetBinContent(24, DataVBFSdb);
            //hParameters->SetBinContent(25, TTbarVBFSdb);
            //hParameters->SetBinContent(26, QCDVBFSdb);


	    delete hHelp;
	  }
	  else{
	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbinLtiso);
	    hW3JetsLooseTauIso->Add(h1,1.0);
	  }

	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);	  
	}
	else if((it->first).find("WJets")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  hWKeys = new TH1Keys("hWKeys","W+jets smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hWKeys", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  cout << "Keys for WJets filled with integral " << hWKeys->Integral() << " and entries " << hWKeys->GetEntries() << endl;

	  if(selection_.find("bTag")!=string::npos){ 
            cout << "Filling histo with loose taus... for W+Jets in bTag" << endl;
            
            TH1F* h1Help = new TH1F( "h1Help" ,"" , nBins , bins.GetArray());
            h1Help->Reset();
            currentTree->Draw(variable+">>h1Help","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinInclusive&&ExactOneJet20));
            hWJetsLooseBTag->Add(h1Help,1.0); 
            
            // NORMALIZE WJETS ON DATA 
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray()); 
            
            // SIDEBAND OS
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ&&bTagLoose));
            cout << hHelp->Integral() << endl;
            float WJetsBTAGSdb    = hHelp->Integral();
            float WJetsBTAGSdbInt = hHelp->GetEntries();
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&pZ&&bTagLoose));
            float WJetsBTAGSgn    = hHelp->Integral();
            float WJetsBTAGSgnInt = hHelp->GetEntries();
            cout << "Wjets MC in BTAG: using " << WJetsBTAGSdbInt << " events from sideband and " << WJetsBTAGSgnInt << " from signal region" << endl;
            float ExtrapFactorWJets = WJetsBTAGSdb/WJetsBTAGSgn;
            cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorWJets << endl;
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&bTag));
            cout << hHelp->Integral() << endl;
            float WJetsBTAGAll    = hHelp->Integral();
            float WJetsBTAGAlllInt = hHelp->GetEntries();

	                
            // SIDEBAND2 OS 
            hHelp->Reset(); 
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTagLoose)); 
            cout << hHelp->Integral() << endl; 
            float WJetsBTAGSdb2    = hHelp->Integral(); 
            float WJetsBTAGSdbInt2 = hHelp->GetEntries(); 
            hHelp->Reset(); 
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&pZ&&bTagLoose)); 
            float WJetsBTAGSgn2    = hHelp->Integral(); 
            float WJetsBTAGSgnInt2 = hHelp->GetEntries(); 
            cout << "Wjets MC in BTAG: using " << WJetsBTAGSdbInt2 << " events from sideband (2) and " << WJetsBTAGSgnInt2 << " from signal region" << endl; 
            float ExtrapFactorWJets2 = WJetsBTAGSdb2/WJetsBTAGSgn2; 
            cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorWJets2 << endl; 

	    // SIDEBAND SS
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&bTagLoose));
            cout << hHelp->Integral() << endl;
            float WJetsBTAGSdbSS    = hHelp->Integral();
            float WJetsBTAGSdbIntSS = hHelp->GetEntries();
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&bTagLoose));
            float WJetsBTAGSgnSS    = hHelp->Integral();
            float WJetsBTAGSgnIntSS = hHelp->GetEntries();
            cout << "Wjets MC (SS) in BTAG: using " << WJetsBTAGSdbIntSS << " events from sideband (2) and " << WJetsBTAGSgnIntSS << " from signal region" << endl;
            float ExtrapFactorWJetsSS = WJetsBTAGSdbSS/WJetsBTAGSgnSS;
            cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorWJetsSS << endl;
            
	    // now, normalize to sidebands
            cout << "#######  Start normalization OS from sidebands #######" << endl;
            hHelp->Reset();
            data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&bTag);
            float DataBTAGSdb = hHelp->Integral();
            cout << "In BTAG region, I get " << DataBTAGSdb << " events in the high Mt sdb" << endl;

            hHelp->Reset();
            backgroundDYTauTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTag));
            float dytotauBTAGSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dytotauBTAGSdb << " events in the high Mt sdb from DYtotautau" << endl;
            hHelp->Reset();

            hHelp->Reset();
            backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTag));
            float dymutotauBTAGSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dymutotauBTAGSdb << " events in the high Mt sdb from DYmutotau" << endl;
            hHelp->Reset();
            
            hHelp->Reset();
            backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTag));
            float dyjtotauBTAGSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dyjtotauBTAGSdb << " events in the high Mt sdb from DYjtotau" << endl;
            hHelp->Reset();
            
	    hHelp->Reset();
            backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTag));
            float othersBTAGSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << othersBTAGSdb << " events in the high Mt sdb from others" << endl;
            hHelp->Reset();
            
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&bTag));
            float TTbarBTAGSdb = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In BTAG region, I expect " << TTbarBTAGSdb << " events in the high Mt sdb from TTbar" << endl;
            hHelp->Reset();

	    cout << "Subtracting the backgrounds from the sideband..." << endl;
            DataBTAGSdb -= TTbarBTAGSdb;
	    DataBTAGSdb -= dytotauBTAGSdb;
            DataBTAGSdb -= dymutotauBTAGSdb;
            DataBTAGSdb -= dyjtotauBTAGSdb;
            DataBTAGSdb -= othersBTAGSdb;
            float normalization = ExtrapFactorWJets2>0 ? DataBTAGSdb/ExtrapFactorWJets2 : -99;
            cout << "Estimation of W+jets in the BTAG category is " << normalization << endl;
            
            hWJetsLooseBTag->Scale(normalization/hWJetsLooseBTag->Integral());
            
            cout << "#######  Start normalization SS from sidebands #######" << endl;
            
            hHelp->Reset(); 
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&apZ2&&bTag); 
            float DataBTAGSdbSS = hHelp->Integral(); 
            cout << "In BTAG SS region, I get " << DataBTAGSdbSS << " events in the high Mt sdb" << endl; 

	    hHelp->Reset();
            backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&bTag));
            float dymutotauBTAGSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dymutotauBTAGSdbSS << " events in the high Mt sdb SS from DYmutotau" << endl;
            hHelp->Reset();
            
            hHelp->Reset();
            backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&bTag));
            float dyjtotauBTAGSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dyjtotauBTAGSdbSS << " events in the high Mt sdb SS from DYjtotau" << endl;
            hHelp->Reset();
            
            hHelp->Reset();
            backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&bTag));
            float othersBTAGSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << othersBTAGSdbSS << " events in the high Mt sdb SS from others" << endl;
            hHelp->Reset();
            
	    hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&bTag));
            float TTbarBTAGSdbSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In BTAG SS region, I expect " << TTbarBTAGSdbSS << " events in the high Mt sdb from TTbar" << endl;
            
            /*
            hHelp->Reset(); 
            data->Draw(variable+">>hHelp",(TCut(scaleFactMu.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ&&bTag)); 
            float QCDBTAGSdbSS = hHelp->Integral(); 
            cout << "In BTAG SS region, I measure " << QCDBTAGSdbSS << " events in the high Mt sdb from anti-isolated events"<< endl; 
	    */

            hHelp->Reset(); 
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&pZ&&bTag); 
            float DataBTAGSgnSS = hHelp->Integral(); 

            hHelp->Reset();
            backgroundDYMutoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&bTag));
            float dymutotauBTAGSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dymutotauBTAGSgnSS << " events in the sgn SS from DYmutotau" << endl;
            hHelp->Reset();

	    hHelp->Reset();
            backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&bTag));
            float dyjtotauBTAGSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << dyjtotauBTAGSgnSS << " events in the sgn SS from DYjtotau" << endl;
            hHelp->Reset();
            
            hHelp->Reset();
            backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&bTag));
            float othersBTAGSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
            cout << "In BTAG region, I expect " << othersBTAGSgnSS << " events in the sgn SS from others" << endl;
            hHelp->Reset();
            
            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&bTag));
            float TTbarBTAGSgnSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In BTAG SS region, I expect " << TTbarBTAGSgnSS << " events in the sgn from TTbar" << endl;

	    float nonWbackgroundSS = TTbarBTAGSgnSS + othersBTAGSgnSS + dyjtotauBTAGSgnSS + dymutotauBTAGSgnSS ;
            float WbackgroundSS    = (DataBTAGSdbSS - TTbarBTAGSdbSS /*-QCDBTAGSdbSS*/ -dymutotauBTAGSdbSS-dyjtotauBTAGSdbSS-othersBTAGSdbSS)/ExtrapFactorWJetsSS;
            float QCDSgnSS         = DataBTAGSgnSS - nonWbackgroundSS - WbackgroundSS;
            cout << "In BTAG SS region, I measure " << DataBTAGSgnSS << endl;
            cout << "The bkg estimation for QCD is therefore: " << DataBTAGSgnSS << " - " << nonWbackgroundSS << " - " << WbackgroundSS << " = " << QCDSgnSS << endl;


            hParameters->SetBinContent(13, ExtrapFactorWJets);
            hParameters->SetBinContent(14, DataBTAGSdb);
            hParameters->SetBinContent(15, TTbarBTAGSdb);
            //hParameters->SetBinContent(16, QCDBTAGSdb);
            hParameters->SetBinContent(17, ExtrapFactorWJetsSS);
            hParameters->SetBinContent(18, DataBTAGSdbSS);
            hParameters->SetBinContent(19, TTbarBTAGSdbSS);
            //hParameters->SetBinContent(20, QCDBTAGSdbSS);
            hParameters->SetBinContent(21, DataBTAGSgnSS);
            hParameters->SetBinContent(22, QCDSgnSS);
            hParameters->SetBinContent(23, ExtrapFactorWJets2);
            hParameters->SetBinContent(24, DataBTAGSdb);
            hParameters->SetBinContent(25, TTbarBTAGSdb);
            //hParameters->SetBinContent(26, QCDBTAGSdb);

            delete hHelp; delete h1Help;
          }
	}
	else if((it->first).find("DYMutoTau")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  hZmmKeys = new TH1Keys("hZmmKeys","Z+jets, mu to tau smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hZmmKeys", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  cout << "Keys for Zmm filled with integral " << hZmmKeys->Integral() << " and entries " << hZmmKeys->GetEntries() << endl;
	  if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	    currentTree->Draw(variable+">>hHelp",     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinInclusive&&twoJets));
	    hHelp->Scale(ExtrapLFakeInclusive*ExtrapolationFactorZ/hHelp->Integral());
	    hZmmLoose->Add(hHelp,1.0);
	    delete hHelp;
	  }
	}
	else if((it->first).find("DYJtoTau")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  hZmjKeys = new TH1Keys("hZmjKeys","Z+jets, jet to tau smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hZmjKeys", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  cout << "Keys for Zmj filled with integral " << hZmjKeys->Integral() << " and entries " << hZmjKeys->GetEntries() << endl;
	  if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){
            TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
            currentTree->Draw(variable+">>hHelp",     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinInclusive&&twoJets));
            hHelp->Scale(ExtrapJFakeInclusive*ExtrapolationFactorZ/hHelp->Integral());
            hZmjLoose->Add(hHelp,1.0);
            delete hHelp;
          }
	}
	else if((it->first).find("TTbar")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);

	  //take TT shape from relaxed selection
	  if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray()); 
            hHelp->Reset(); 
            currentTree->Draw(variable+">>+hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinNSaIsoLtisoInclusive&&twoJets)); 
            hTTbLoose->Add(hHelp, 1.0);
	    delete hHelp;
	  }
	  else{
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	    hHelp->Reset();
	    currentTree->Draw(variable+">>+hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbinNSaIsoLtiso);
	    hTTbLoose->Add(hHelp, 1.0);
	    delete hHelp;
	  }
	}
	else if((it->first).find("SUSY")!=string::npos){ 
          TString sampleName = samples[iter];
          if(sampleName.Contains("SUSYGG"))sampleName.ReplaceAll("SUSYGG", "");
          else if(sampleName.Contains("SUSYBB"))sampleName.ReplaceAll("SUSYBB", "");
          float mA = atof(sampleName.Data());
          cout<<" SUSY mass "<<samples[iter]<<" "<<mA<<endl; 
          TCut HWidth(Form("genVMass > 0.7*%f && genVMass < 1.3*%f", mA, mA));  
          cout<<" width cut "<<HWidth<<endl; 
          currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightMu*SFMu*SFTau*HqTWeight)"*(sbin&&HWidth));
        }
	else if((it->first).find("Others")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  hVVKeys = new TH1Keys("hVVKeys","Others smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hVVKeys", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
	  cout << "Keys for VV filled with integral " << hVVKeys->Integral() << " and entries " << hVVKeys->GetEntries() << endl;
	  //take VV shape from relaxed selection 
          if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){ 
            TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());  
            hHelp->Reset();  
            currentTree->Draw(variable+">>+hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinNSaIsoLtisoInclusive&&twoJets));  
            hVVLoose->Add(hHelp, 1.0); 
            delete hHelp; 
          } 
          else{ 
            TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray()); 
            hHelp->Reset(); 
            currentTree->Draw(variable+">>+hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*(sbinNSaIsoLtisoInclusive&&ExactOneJet)); 
            hVVLoose->Add(hHelp, 1.0); 
            delete hHelp; 
          } 
	}
	
	// here, we fill histos for fake-rate
	else if((it->first).find("Data")!=string::npos){

	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, sbin);
	  hData->Add(h1,1.0);

	  cout << "DATA === " << hData->Integral() << endl;

	  cout << "Filling histo with anti-iso muons and loose taus" << endl;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtisoInclusive&&vbfLooseQCD);
	    hDataAntiIsoLooseTauIso->Add(h1,1.0);
	    h1->Reset();
	    //calculate efficiency of the events passing QCD selection to pass the category selection
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtisoInclusive);
	    float QCDSelDataInclusive = h1->Integral();
	    h1->Reset();
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso);
	    float QCDSelDataVBF = h1->Integral();
	    float normalization = SSeventsExtrap*(QCDSelDataVBF/QCDSelDataInclusive);
	    hDataAntiIsoLooseTauIso->Scale(normalization/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else if(selection_.find("bTag")!=string::npos){ 
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso);
            hDataAntiIsoLooseTauIso->Add(h1,1.0); 
            h1->Reset(); 
            //calculate efficiency of the events passing QCD selection to pass the category selection 
            currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtisoInclusive); 
            float QCDSelDataInclusive = h1->Integral(); 
            h1->Reset(); 
            currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso); 
            float QCDSelDataVBF = h1->Integral(); 
            float normalization = SSeventsExtrap*(QCDSelDataVBF/QCDSelDataInclusive); 
            hDataAntiIsoLooseTauIso->Scale(normalization/hDataAntiIsoLooseTauIso->Integral()); 
          } 
	  else{
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso);

	    //subtract ttbar and ZLL with same selection
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	    hHelp->Reset(); 
	    backgroundTTbar->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSaIsoLtiso); 
	    hHelp->Scale(1.0*Lumi/1000*hltEff_*TTxsectionRatio); 
	    h1->Add(hHelp, -1 ); 
	    
	    hHelp->Reset(); 
	    backgroundDYMutoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSaIsoLtiso); 
	    hHelp->Scale(MutoTauCorrectionFactor*Lumi*lumiCorrFactor/1000*hltEff_); 
	    h1->Add(hHelp, -1 ); 
	     
	    hHelp->Reset(); 
	    backgroundDYJtoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu)"*sbinSSaIsoLtiso); 
	    hHelp->Scale(JtoTauCorrectionFactor*Lumi*lumiCorrFactor/1000*hltEff_); 
	    h1->Add(hHelp, -1 ); 

	    hDataAntiIsoLooseTauIso->Add(h1,1.0);
	    delete hHelp;
	  }

	  // template for QCD : anti-iso mu && loose-tau X fake-rate(mu) X fake-rate(tau)
	  cout << "Filling histo with anti-iso muons and loose taus and FR" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMu.c_str()))*(TCut(scaleFactTauQCD.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFR->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMu.c_str()))*(TCut(scaleFactTauQCD.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral();
	    //hDataAntiIsoLooseTauIsoFR->Scale(normalization/hDataAntiIsoLooseTauIsoFR->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMu.c_str()))*(TCut(scaleFactTauQCD.c_str()))*sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIsoFR->Add(h1,1.0);
	  }

	  cout << "Filling histo with anti-iso muons and loose taus and FR (Up)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFRUp->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
 	    //hDataAntiIsoLooseTauIsoFRUp->Scale(normalization/hDataAntiIsoLooseTauIsoFRUp->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIsoFRUp->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and loose taus and FR (Down)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFRDown->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
 	    //hDataAntiIsoLooseTauIsoFRDown->Scale(normalization/hDataAntiIsoLooseTauIsoFRDown->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIsoFRDown->Add(h1,OStoSSRatioQCD);
	  }

	  // template for QCD : anti-iso mu && loose-tau X fake-rate(mu) X fake-rate(tau)
	  cout << "Filling histo with anti-iso muons and tight taus" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name, sbinSSaIsoInclusive&&vbfLoose);
	    hDataAntiIso->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name, sbinSSaIso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIso->Scale(normalization/hDataAntiIso->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name, sbinSSaIso);
	    hDataAntiIso->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and tight taus and FR" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactMu.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFR->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactMu.c_str()))*(sbinSSaIso));
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFR->Scale(normalization/hDataAntiIsoFR->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactMu.c_str()))*sbinSSaIso);
	    hDataAntiIsoFR->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and tight taus and FR (Up)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactMuUp.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFRUp->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactMuUp.c_str()))*sbinSSaIso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFRUp->Scale(normalization/hDataAntiIsoFRUp->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactMuUp.c_str()))*sbinSSaIso);
	    hDataAntiIsoFRUp->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and tight taus and FR (Down)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFRDown->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*sbinSSaIso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFRDown->Scale(normalization/hDataAntiIsoFRDown->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactMuDown.c_str()))*sbinSSaIso);
	    hDataAntiIsoFRDown->Add(h1,OStoSSRatioQCD);
	  }
	}

	else if((it->first).find("LooseIso")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,    sbinSSlIso);

	  TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hHelp",    sbinSSlIso2);
	  hLooseIso2->Add(hHelp,1.0);
	  hHelp->Reset();
	  currentTree->Draw(variable+">>hHelp",    sbinSSlIso3);
	  hLooseIso3->Add(hHelp,1.0);
	  delete hHelp;

	  hLooseIsoKeys = new TH1Keys("hLooseIsoKeys","Loose Iso smoothed", nBins , bins.GetArray());
	  if(  ((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || 
		selection_.find("boost")!=string::npos ||
		selection_.find("vh")!=string::npos) )
	    currentTree->Draw(variable+">>hLooseIsoKeys", sbinSSlIso);
	  cout << "Keys for LooseIso filled with integral " << hLooseIsoKeys->Integral() << " and entries " << hLooseIsoKeys->GetEntries() << endl;

	}
	else if((it->first).find("AntiIso")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,    sbinSSaIso);
	  hAntiIsoKeys = new TH1Keys("hAntiIsoKeys","Anti Iso smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hAntiIsoKeys", sbinSSaIso);
	  cout << "Keys for AntiIso filled with integral " << hAntiIsoKeys->Integral() << " and entries " << hAntiIsoKeys->GetEntries() << endl;
	}
	else
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbin);
      }

      else{
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTMu*embeddingWeight)"*sbinPZetaRelEmbeddingInclusive);
	float embeddedNoMEtCut = h1->Integral();
	h1->Reset();
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTMu*embeddingWeight)"*sbinEmbeddingInclusive);

	embeddedMEtCutEff =  h1->Integral()/embeddedNoMEtCut;
	cout << "Efficiency of antiW cut on embedded " << embeddedMEtCutEff << endl;

	h1->Reset();
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTMu*embeddingWeight)"*sbinEmbedding);
	
	//Fill with only two jets for VBF shape
	TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	hHelp->Reset();
	currentTree->Draw(variable+">>hHelp", "(HLTTau*HLTMu*embeddingWeight)"*(sbinEmbeddingInclusive&&twoJets));
	hDataEmbLooseVBF->Add(hHelp, 1.0);
	delete hHelp;
      }


      // scale by correction factors
      if( ! ((it->first).find("Data")!=string::npos || 
	     (it->first).find("LooseIso")!=string::npos ||
	     (it->first).find("AntiIso")!=string::npos) ) 
	h1->Scale(Lumi/1000*hltEff_);

      // scale by DATA/MC ratio in Zmumu sideband
      if((it->first).find("DY")!=string::npos)
	h1->Scale(lumiCorrFactor);

      // if W+jets, scale by extrapolation
      float sFWOS = ( selection_.find("novbf")!=string::npos  || 
		      selection_.find("boost")!=string::npos  || 
		      selection_.find("bTag")!=string::npos   || 
		      selection_.find("oneJet") !=string::npos || 
		      selection_.find("twoJets") !=string::npos || 
		      selection_.find("inclusive")!=string::npos) ? 
	OSWinSignalRegionDATA/OSWinSignalRegionMC : WcorrectionFactorOS;
  
      if((it->first).find("WJets")!=string::npos){

	TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	currentTree->Draw(variable+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFMu*SFTau)"*sbinMtiso);
	hWLooseIso2->Add(hHelp,1.0);
	delete hHelp;

	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  sFWOS *= VbfExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << VbfExtrapolationFactorW << " according to the Z->mumu+j+vbf/Z->mumu+j ratio" << endl;
	}
	else if(selection_.find("boost")!=string::npos){
	  sFWOS *= BoostExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << BoostExtrapolationFactorW << " according to the Z->mumu+j+vbf/Z->mumu+j ratio" << endl;
	}	else if(selection_.find("boost")!=string::npos){
	  sFWOS *= BoostExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << BoostExtrapolationFactorW << " according to the Z->mumu+j+vbf/Z->mumu+j ratio" << endl;
	}

	h1->Scale( sFWOS );
	hW->Add(h1,1.0);
	if(h1->Integral()>0) hWLooseIso2->Scale(h1->Integral()/hWLooseIso2->Integral());
      }

      // if DY->tautau, and vbf scale by ratio data/MC
      if((it->first).find("DYToTauTau")!=string::npos){


	h1->Scale( ExtrapolationFactorZDataMC );

	//if(selection_.find("novbf")!=string::npos){
	//cout << "DY->tautau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//h1->Scale( NoVbfExtrapolationFactorZ );
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//cout << "DY->tautau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//h1->Scale( VbfExtrapolationFactorZ );
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//h1->Scale( BoostExtrapolationFactorZ );
	//}
      }

      // if DY->mumu, mu->tau, scale by fake-rate
      if((it->first).find("DYMutoTau")!=string::npos){

	float sF = MutoTauCorrectionFactor;

	sF *= ExtrapolationFactorZDataMC;

	//if(selection_.find("novbf")!=string::npos){
	//sF *= NoVbfExtrapolationFactorZ;
	//cout << "DY->tautau, mu->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//sF *= VbfExtrapolationFactorZ;
	//cout << "DY->tautau, mu->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau, mu->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//sF *= BoostExtrapolationFactorZ;
	//}

	h1->Scale(sF);
	hZmm->Add(h1,1.0);
	hZfakes->Add(h1,1.0);
      }

      // if DY->mumu, jet->tau, scale by fake-rate
      if((it->first).find("DYJtoTau")!=string::npos){

	float sF = JtoTauCorrectionFactor;

	sF *= ExtrapolationFactorZDataMC;

	//if(selection_.find("novbf")!=string::npos){
	//sF *= NoVbfExtrapolationFactorZ;
	//cout << "DY->tautau, jet->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//sF *= VbfExtrapolationFactorZ;
	//cout << "DY->tautau, jet->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau, jet->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	//sF *=BoostExtrapolationFactorZ;
	//}

	h1->Scale(sF);
	hZmj->Add(h1,1.0); //<-----------------------
	hZfakes->Add(h1,1.0);
      }

      //if((it->first).find("ggH")!=string::npos && selection_.find("boost")!=string::npos){
      //h1->Scale(kFactorSM);
      //}


    }
  
    /////////////////////////////////////////////////////////////////////////////////////

    if( (it->first).find("DYMutoTau")!=string::npos ||  
	(it->first).find("DYJtoTau")!=string::npos || 
	(it->first).find("WJets")!=string::npos || 
	(it->first).find("Others")!=string::npos ) {
      hEWK->SetFillColor(kRed+2);
      hEWK->Add(h1,1.0);

      if( (it->first).find("Others")!=string::npos ){
	hVV->Add(h1,1.0);
	if(hVVKeys->Integral()>0) hVVKeys->Scale(hVV->Integral()/hVVKeys->Integral());
	hVVKeys->SetFillColor(kRed+2);
      }
    }

    if( (it->first).find("DYToTauTau")!=string::npos ) {
      hZtt->Add(h1,1.0);
      hZtt->SetFillColor(kOrange-4);
    }
    if( (it->first).find("Embedded")!=string::npos ) {
      //if(hZtt->Integral()>0) h1->Scale(hZtt->Integral()/h1->Integral());

      hParameters->SetBinContent(12,embeddedMEtCutEff/madgraphMEtCutEff);

      h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusive)/h1->Integral());
      h1->Scale(embeddedMEtCutEff/madgraphMEtCutEff);

      hDataEmb->Add(h1,1.0);
      hDataEmb->SetFillColor(kOrange-4);
    }
    if( (it->first).find("DYMutoTau")!=string::npos ) {
      if(hZmmKeys->Integral()>0) hZmmKeys->Scale(hZmm->Integral()/hZmmKeys->Integral());
      hZmmKeys->SetFillColor(kRed+2);
    }
    if( (it->first).find("DYJtoTau")!=string::npos ) {
      if(hZmjKeys->Integral()>0) hZmjKeys->Scale(hZmj->Integral()/hZmjKeys->Integral());
      hZmjKeys->SetFillColor(kRed+2);
    }
    if( (it->first).find("WJets")!=string::npos ) {
      if(hWKeys->Integral()>0) hWKeys->Scale(hW->Integral()/hWKeys->Integral());
      hWKeys->SetFillColor(kRed+2);
    }
    if( (it->first).find("W3Jets")!=string::npos ) {

      hW3Jets->Add(h1,1.0);
      if(hW3Jets->Integral()>0) hW3Jets->Scale(hW->Integral()/hW3Jets->Integral());
      hW3Jets->SetFillColor(kRed+2);
      if(hW3JetsKeys->Integral()>0) hW3JetsKeys->Scale(hW->Integral()/hW3JetsKeys->Integral());
      hW3JetsKeys->SetFillColor(kRed+2);
    }
    if( (it->first).find("LooseIso")!=string::npos ) {
      hLooseIso->Add(h1,1.0);
      if(hLooseIsoKeys->Integral()>0) hLooseIsoKeys->Scale(hLooseIso->Integral()/hLooseIsoKeys->Integral());
      hLooseIsoKeys->SetFillColor(0);
    }
    if( (it->first).find("AntiIso")!=string::npos ) {
      float sF = SSIsoToSSAIsoRatioQCD*OStoSSRatioQCD;
      cout << "Anti-isolated SS data scaled by " <<  SSIsoToSSAIsoRatioQCD << " ratio measured in inclusive sample ==>" << endl;
      cout << "   SS anti-isolated events = " << h1->GetEntries() << " ==> " << h1->Integral()*SSIsoToSSAIsoRatioQCD << " predicted in signal region" << endl;
      h1->Scale(sF);
      hAntiIso->Add(h1,1.0);
      if(hAntiIsoKeys->Integral()>0) hAntiIsoKeys->Scale(hAntiIso->Integral()/hAntiIsoKeys->Integral());
      hAntiIsoKeys->SetFillColor(0);
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      hTTb->Add(h1,1.0);
      hTTb->Scale(TTxsectionRatio);
      if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos )
	hTTb->Scale(scaleFactorTTinVBF);
      else
	hTTb->Scale(scaleFactorTTOS);
      hTTb->SetFillColor(kBlue-8);     
    }
    if( (it->first).find("SS")!=string::npos ) {
      hQCD->Add(h1,1.0);
      hQCD->SetFillColor(kMagenta-10);
    }
    if((it->first).find(string(Form("qqH%d",mH_)))!=string::npos){
      hSgn1->Add(h1,1.0);
      hSgn1->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find(string(Form("ggH%d",mH_)))!=string::npos){
      hSgn2->Add(h1,1.0);
      hSgn2->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn2->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find(string(Form("VH%d",mH_)))!=string::npos){
      hSgn3->Add(h1,1.0);
      hSgn3->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn3->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find(string(Form("ggH%d",110)))!=string::npos){
      hggH110->Add(h1,1.0);
      hggH110->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",115)))!=string::npos){
      hggH115->Add(h1,1.0);
      hggH115->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",120)))!=string::npos){
      hggH120->Add(h1,1.0);
      hggH120->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",125)))!=string::npos){
      hggH125->Add(h1,1.0);
      hggH125->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",130)))!=string::npos){
      hggH130->Add(h1,1.0);
      hggH130->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",135)))!=string::npos){
      hggH135->Add(h1,1.0);
      hggH135->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",140)))!=string::npos){
      hggH140->Add(h1,1.0);
      hggH140->SetLineWidth(2);
    }
    if((it->first).find(string(Form("ggH%d",145)))!=string::npos){
      hggH145->Add(h1,1.0);
      hggH145->SetLineWidth(2);
    }

    if((it->first).find(string(Form("qqH%d",110)))!=string::npos){
      hqqH110->Add(h1,1.0);
      hqqH110->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",115)))!=string::npos){
      hqqH115->Add(h1,1.0);
      hqqH115->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",120)))!=string::npos){
      hqqH120->Add(h1,1.0);
      hqqH120->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",125)))!=string::npos){
      hqqH125->Add(h1,1.0);
      hqqH125->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",130)))!=string::npos){
      hqqH130->Add(h1,1.0);
      hqqH130->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",135)))!=string::npos){
      hqqH135->Add(h1,1.0);
      hqqH135->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",140)))!=string::npos){
      hqqH140->Add(h1,1.0);
      hqqH140->SetLineWidth(2);
    }
    if((it->first).find(string(Form("qqH%d",145)))!=string::npos){
      hqqH145->Add(h1,1.0);
      hqqH145->SetLineWidth(2);
    }

    if((it->first).find(string(Form("VH%d",110)))!=string::npos){
      hVH110->Add(h1,1.0);
      hVH110->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",115)))!=string::npos){
      hVH115->Add(h1,1.0);
      hVH115->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",120)))!=string::npos){
      hVH120->Add(h1,1.0);
      hVH120->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",125)))!=string::npos){
      hVH125->Add(h1,1.0);
      hVH125->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",130)))!=string::npos){
      hVH130->Add(h1,1.0);
      hVH130->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",135)))!=string::npos){
      hVH135->Add(h1,1.0);
      hVH135->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",140)))!=string::npos){
      hVH140->Add(h1,1.0);
      hVH140->SetLineWidth(2);
    }
    if((it->first).find(string(Form("VH%d",145)))!=string::npos){
      hVH145->Add(h1,1.0);
      hVH145->SetLineWidth(2);
    }

    ////////////////////////////////////////////////////////////
    if((it->first).find("SUSY")!=string::npos){
      TH1F* histoSusy =  (mapSUSYhistos.find( (it->first) ))->second;
      histoSusy->Add(h1,1.0);
      histoSusy->SetLineWidth(2);
    }
    ////////////////////////////////////////////////////////////

   

    if((it->first).find("Data")!=string::npos){
      //hData->Add(h1,1.0);
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

  }

  out.close();

  // all signal summed together:
  hSgn->Add(hSgn1,hSgn2,1,1);
  hSgn->Add(hSgn3);
  hSgn->SetFillColor(0);
  hSgn->SetLineColor(kBlue);
  hSgn->SetLineWidth(2);
  hSgn->SetLineStyle(kDashed);

  // adding alltogether
  hSiml->Add(hTTb,1.0);
  if(useEmbedding_)
    hSiml->Add(hDataEmb,1.0);
  else
    hSiml->Add(hZtt,1.0);


  //Adding to the stack
  if((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || selection_.find("twoJets")!=string::npos ){
    /*
    float QCDfromSS = hParameters->GetBinContent(22);
    QCDfromSS *= OStoSSRatioQCD;
    hDataAntiIsoLooseTauIso->Scale(QCDfromSS/hDataAntiIsoLooseTauIso->Integral());
    */
    //qcd already normalized 
    hDataAntiIsoLooseTauIso->SetFillColor(kMagenta-10);    
    aStack->Add(hDataAntiIsoLooseTauIso);
    hSiml->Add(hDataAntiIsoLooseTauIso,1.0);
  }
  else if(selection_.find("bTag")!=string::npos){
    //qcd already normalized  
    hDataAntiIsoLooseTauIso->SetFillColor(kMagenta-10);     
    aStack->Add(hDataAntiIsoLooseTauIso); 
    hSiml->Add(hDataAntiIsoLooseTauIso,1.0); 
  } 
  else{
    //take shape from QCD selection
    hDataAntiIsoLooseTauIso->Scale(hQCD->Integral()/hDataAntiIsoLooseTauIso->Integral());
    hDataAntiIsoLooseTauIso->SetFillColor(kMagenta-10);     
    aStack->Add(hDataAntiIsoLooseTauIso); 
    hSiml->Add(hDataAntiIsoLooseTauIso,1.0); 
    //hSiml->Add(hQCD,1.0);
    //aStack->Add(hQCD);
  }

  if((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || selection_.find("twoJets")!=string::npos ){
    hZfakes->SetFillColor(kRed+2);
    hW3JetsLooseTauIso->SetFillColor(kRed+2);
    hW3JetsLooseTauIso->SetLineColor(kRed+2);
    aStack->Add(hW3JetsLooseTauIso);
    aStack->Add(hZfakes);
    hSiml->Add(hW3JetsLooseTauIso,1.0);
    hSiml->Add(hZfakes,1.0);
  }
  else if(selection_.find("bTag")!=string::npos){
    hZfakes->SetFillColor(kRed+2); 
    hWJetsLooseBTag->SetFillColor(kRed+2); 
    hWJetsLooseBTag->SetLineColor(kRed+2);
    aStack->Add(hWJetsLooseBTag);
    aStack->Add(hZfakes); 
    hSiml->Add(hWJetsLooseBTag,1.0); 
    hSiml->Add(hZfakes,1.0);
  }
  else{
    aStack->Add(hEWK);
    hSiml->Add(hEWK,1.0);
  }

  aStack->Add(hTTb);
  if(useEmbedding_){
    if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
      hDataEmbLooseVBF->Scale(hDataEmb->Integral()/hDataEmbLooseVBF->Integral());
      hDataEmbLooseVBF->SetFillColor(kOrange-4);
      aStack->Add(hDataEmbLooseVBF);
    }
    else
      aStack->Add(hDataEmb);
  }
  else
    aStack->Add(hZtt);
  if(!logy_)
    aStack->Add(hSgn);

  // legend
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

  //TH1F* hRatio = new TH1F("hRatio", " ; ; #frac{(DATA-MC)}{#sqrt{DATA}}",
  //		  hStack->GetNbinsX(), 
  //		  hStack->GetXaxis()->GetXmin(), hStack->GetXaxis()->GetXmax());

  TH1F* hRatio = new TH1F( "hRatio" ," ; ; #frac{(DATA-MC)}{#sqrt{DATA}}" , nBins , bins.GetArray());
  hRatio->Reset();
  hRatio->SetXTitle("");
  hRatio->SetYTitle("#frac{(DATA-MC)}{#sqrt{DATA}}");

  hRatio->SetMarkerStyle(kFullCircle);
  hRatio->SetMarkerSize(0.8);
  hRatio->SetLabelSize(0.12,"X");
  hRatio->SetLabelSize(0.10,"Y");
  hRatio->SetTitleSize(0.12,"Y");
  hRatio->SetTitleOffset(0.36,"Y");
  TH1F* hError = (TH1F*)hRatio->Clone("hError");
  hError->Reset();
  hError->SetFillColor(kRed);
  hError->SetFillStyle(3004);
  hError->SetMarkerStyle(kDot);

  float uncertZtt = 0;
  if(selection_.find("novbf")!=string::npos  || selection_.find("inclusive")!=string::npos ){
    uncertZtt += (0.07  * 0.07) ; // Tau-ID 
    uncertZtt += (0.03  * 0.03);  // Tau-ES
  }
  if(selection_.find("oneJet")!=string::npos || selection_.find("twoJets")!=string::npos){
    uncertZtt += (0.07  * 0.07) ; // Tau-ID 
    uncertZtt += (0.03 * 0.03);   // Tau-ES 
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    uncertZtt += (0.07  * 0.07) ; // Tau-ID 
    uncertZtt += (0.03 * 0.03);   // Tau-ES 
    uncertZtt += (0.06 * 0.06);   // Extrap. factor 
  }
  else if(selection_.find("boost")!=string::npos){
    uncertZtt += (0.07  * 0.07) ; // Tau-ID 
    uncertZtt += (0.03  * 0.03);  // Tau-ES 
  }
  uncertZtt = TMath::Sqrt(uncertZtt);


  float uncertTTb = 0;
  if(selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos||
     selection_.find("oneJet")!=string::npos || selection_.find("twoJets")!=string::npos){
    uncertTTb += (0.075 * 0.075) ; // xsection 
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    uncertTTb += (0.075 * 0.075) ; // xsection 
  }
  else if(selection_.find("boost")!=string::npos){
    uncertTTb += (0.075 * 0.075) ; // xsection 
  }
  uncertTTb = TMath::Sqrt(uncertTTb);

  float uncertEWK = 0;
  if(selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos ||
     selection_.find("oneJet")!=string::npos || selection_.find("twoJets")!=string::npos){
    uncertEWK += (0.07 * 0.07) ; // extrapolation 
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    uncertEWK += (0.07 * 0.07) ; // extrapolation 
    uncertEWK += (0.20 * 0.20) ; // extrapolation 
  }
  else if(selection_.find("boost")!=string::npos){
    uncertEWK += (0.07 * 0.07) ; // extrapolation 
    uncertEWK += (0.20 * 0.20) ; // extrapolation  
  }
  uncertEWK = TMath::Sqrt(uncertEWK);

  float uncertQCD = 0;
  if(selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos ||
     selection_.find("oneJet")!=string::npos || selection_.find("twoJets")!=string::npos){
    uncertQCD += (0.10 * 0.10) ; // extrapolation 
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    uncertQCD += (0.10 * 0.10) ; // extrapolation 
    uncertQCD += (0.20 * 0.20) ; // extrapolation 
  }
  else if(selection_.find("boost")!=string::npos){
    uncertQCD += (0.10 * 0.10) ; // extrapolation 
    uncertQCD += (0.20 * 0.20) ; // extrapolation  
  }
  uncertQCD = TMath::Sqrt(uncertQCD);

  float maxPull = 0.;
  for(int k = 0 ; k < hRatio->GetNbinsX(); k++){
    float pull = hData->GetBinContent(k) - hSiml->GetBinContent(k);
    if(hData->GetBinContent(k)>0)
      pull /= TMath::Sqrt(hData->GetBinContent(k));
    hRatio->SetBinContent(k, pull);
    hError->SetBinContent(k, 0.0);
    float error2 = 0.0;
    error2 += pow(hZtt->GetBinContent(k) * uncertZtt, 2);
    error2 += pow(hTTb->GetBinContent(k) * uncertTTb, 2);
    error2 += pow(hEWK->GetBinContent(k) * uncertEWK, 2);
    error2 += pow(hQCD->GetBinContent(k) * uncertQCD, 2);
    if(hData->GetBinContent(k)>0)
      hError->SetBinError(k, TMath::Sqrt(error2/hData->GetBinContent(k)) );
    else
      hError->SetBinError(k,0);
    if(TMath::Abs(pull) > maxPull)
      maxPull = TMath::Abs(pull);
  }
  hRatio->SetAxisRange(-1.2*maxPull,1.2*maxPull,"Y");
  hRatio->Draw("P");
  hError->Draw("E3SAME");

  TF1* line = new TF1("line","0",hRatio->GetXaxis()->GetXmin(),hStack->GetXaxis()->GetXmax());
  line->SetLineStyle(3);
  line->SetLineWidth(1.5);
  line->SetLineColor(kBlack);
  line->Draw("SAME");
  
  //return;

  c1->SaveAs(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/plots/%s/plot_muTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));
  c1->SaveAs(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/plots/%s/plot_muTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));

  // templates for fitting
  TFile* fout = new TFile(Form("/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/Limits/htautau/histograms/%s/muTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");
  fout->cd();

  hSiml->Write();
  hQCD->Write();
  hZmm->Write();
  hZmmLoose->Write();
  hZmj->Write();
  hZmjLoose->Write();
  hZfakes->Write();
  hTTb->Write();
  hTTbLoose->Write();
  hZtt->Write();
  hDataEmb->Write();
  hDataEmbLooseVBF->Write();
  hLooseIso->Write();
  hLooseIso2->Write();
  hLooseIso3->Write();
  hAntiIso->Write();
  hW->Write();
  hWLooseIso2->Write();
  if(hWKeys) hW3Jets->Write();

  TH1* hWKeysHisto = hWKeys ? ((TH1Keys*)hWKeys)->GetHisto() : 0;
  if(hWKeysHisto){
    makeHistoFromDensity(hWKeysHisto,hW);
    hWKeysHisto->SetName("hWKeys");
    hWKeysHisto->Write();
  }
  TH1* hW3JetsKeysHisto = hW3JetsKeys ? ((TH1Keys*)hW3JetsKeys)->GetHisto() : 0;
  if(hW3JetsKeysHisto){
    makeHistoFromDensity(hW3JetsKeysHisto,hW3Jets);
    hW3JetsKeysHisto->SetName("hW3JetsKeys");
    hW3JetsKeysHisto->Write();
  }
  TH1* hZmmKeysHisto = hZmmKeys ? ((TH1Keys*)hZmmKeys)->GetHisto() : 0;
  if(hZmmKeysHisto){
    makeHistoFromDensity(hZmmKeysHisto,hZmm);
    hZmmKeysHisto->SetName("hZmmKeys");
    hZmmKeysHisto->Write();
  }

  TH1* hZmjKeysHisto = hZmjKeys ? ((TH1Keys*)hZmjKeys)->GetHisto() : 0;
  if(hZmjKeysHisto){
    makeHistoFromDensity(hZmjKeysHisto,hZmj);
    hZmjKeysHisto->SetName("hZmjKeys");
    hZmjKeysHisto->Write();
  }
  TH1* hLooseIsoKeysHisto = hLooseIsoKeys ? ((TH1Keys*)hLooseIsoKeys)->GetHisto() : 0;
  if(hLooseIsoKeysHisto){
    makeHistoFromDensity(hLooseIsoKeysHisto,hLooseIso);
    hLooseIsoKeysHisto->SetName("hLooseIsoKeys");
    hLooseIsoKeysHisto->Write();
  }
  TH1* hAntiIsoKeysHisto = hAntiIsoKeys ? ((TH1Keys*)hAntiIsoKeys)->GetHisto() : 0;
  if(hAntiIsoKeysHisto){
    makeHistoFromDensity(hAntiIsoKeysHisto,hAntiIso);
    hAntiIsoKeysHisto->SetName("hAntiIsoKeys");
    hAntiIsoKeysHisto->Write();
  }
  TH1* hVVKeysHisto = hVVKeys ? ((TH1Keys*)hVVKeys)->GetHisto() : 0;
  if(hVVKeysHisto){
    makeHistoFromDensity(hVVKeysHisto, hVV);
    hVVKeysHisto->SetName("hVVKeys");
    hVVKeysHisto->Write();
  }

  hVV->Write();
  hVVLoose->Write();
  hEWK->Write();
  hSgn1->Write();
  hSgn2->Write();
  hSgn3->Write();

  hW3JetsLooseTauIso->Write();
  hW3JetsLooseTauIsoFR->Write();
  hDataAntiIsoLooseTauIso->Write();
  hDataAntiIsoLooseTauIsoFR->Write();
  hDataAntiIsoLooseTauIsoFRUp->Write();
  hDataAntiIsoLooseTauIsoFRDown->Write();
  hDataAntiIso->Write();
  hDataAntiIsoFR->Write();
  hDataAntiIsoFRUp->Write();
  hDataAntiIsoFRDown->Write();
  hWJetsLooseBTag->Write();

  hggH110->Write();  
  hggH115->Write();  
  hggH120->Write();  
  hggH125->Write();  
  hggH130->Write();  
  hggH135->Write();  
  hggH140->Write();  
  hggH145->Write(); 

  hqqH110->Write();  
  hqqH115->Write();  
  hqqH120->Write();  
  hqqH125->Write();  
  hqqH130->Write();  
  hqqH135->Write();  
  hqqH140->Write();  
  hqqH145->Write();  

  hVH110->Write();  
  hVH115->Write();  
  hVH120->Write();  
  hVH125->Write();  
  hVH130->Write();  
  hVH135->Write();  
  hVH140->Write();  
  hVH145->Write(); 

  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    ((mapSUSYhistos.find( SUSYhistos[i] ))->second)->Write();
  }

  hData->Write();
  hParameters->Write();
  fout->Write();
  fout->Close();

  delete hQCD; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hZtt; delete hW; delete hW3Jets; delete hLooseIso; delete hAntiIso; delete hTTbLoose; 
  delete hZmmLoose; delete hZmjLoose;
  delete hWLooseIso2; delete hLooseIso2; delete hLooseIso3;
  delete hW3JetsLooseTauIso; delete hW3JetsLooseTauIsoFR;
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoFR; delete hDataAntiIsoLooseTauIsoFRUp; delete hDataAntiIsoLooseTauIsoFRDown;
  delete hDataAntiIso; delete hDataAntiIsoFR; delete hDataAntiIsoFRUp; delete hDataAntiIsoFRDown;
  delete hWJetsLooseBTag;

  if(hW3JetsKeys)   delete hW3JetsKeys;
  if(hWKeys)        delete hWKeys;
  if(hZmmKeys)      delete hZmmKeys;
  if(hZmjKeys)      delete hZmjKeys;
  if(hVVKeys)       delete hVVKeys;
  if(hLooseIsoKeys) delete hLooseIsoKeys;
  if(hAntiIsoKeys)  delete hAntiIsoKeys;
  delete hggH110; delete hggH115 ; delete hggH120; delete hggH125; delete hggH130; delete hggH135; delete hggH140; delete hggH145;
  delete hqqH110; delete hqqH115 ; delete hqqH120; delete hqqH125; delete hqqH130; delete hqqH135; delete hqqH140; delete hqqH145;
  delete hVH110;  delete hVH115 ;  delete hVH120;  delete hVH125;  delete hVH130;  delete hVH135;  delete hVH140;  delete hVH145;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++) delete mapSUSYhistos.find( SUSYhistos[i] )->second ;

  delete hVV; delete hSgn1; delete hSgn2; delete hSgn3; delete hData; delete hParameters;
  delete hWMt; delete aStack;  delete hEWK; delete hSiml; delete hDataEmb; delete hSgn; delete hRatio; delete line; delete hDataEmbLooseVBF; delete hVVLoose;
  delete fout;


  fSignalGGH->Close();       delete fSignalGGH; 
  fSignalVBF->Close();       delete fSignalVBF;
  fSignalVH->Close();        delete fSignalVH; 
  //fSignalggH110->Close();    delete fSignalggH110;
  //fSignalggH115->Close();    delete fSignalggH115;
  //fSignalggH120->Close();    delete fSignalggH120;
  fSignalggH125->Close();    delete fSignalggH125;
  //fSignalggH130->Close();    delete fSignalggH130;
  //fSignalggH135->Close();    delete fSignalggH135;
  //fSignalggH140->Close();    delete fSignalggH140;
  //fSignalggH145->Close();    delete fSignalggH145;
  //fSignalqqH110->Close();    delete fSignalqqH110;
  //fSignalqqH115->Close();    delete fSignalqqH115;
  //fSignalqqH120->Close();    delete fSignalqqH120;
  fSignalqqH125->Close();    delete fSignalqqH125;
  //fSignalqqH130->Close();    delete fSignalqqH130;
  //fSignalqqH135->Close();    delete fSignalqqH135;
  //fSignalqqH140->Close();    delete fSignalqqH140;
  //fSignalqqH145->Close();    delete fSignalqqH145;
  //fSignalVH110->Close();     delete fSignalVH110;
  //fSignalVH115->Close();     delete fSignalVH115;
  //fSignalVH120->Close();     delete fSignalVH120;
  fSignalVH125->Close();     delete fSignalVH125;
  //fSignalVH130->Close();     delete fSignalVH130;
  //fSignalVH135->Close();     delete fSignalVH135;
  //fSignalVH140->Close();     delete fSignalVH140;
  //fSignalVH145->Close();     delete fSignalVH145;

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



void plotMuTauAll( Int_t useEmbedded = 1, TString outputDir = "Sync23Sep2012"){

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

 
  
//   plotMuTau(125,1,"inclusive",""   ,"hpsMVA",        "#tau MVA","units"              ,outputDir,50,0.75,1.0, 5.0,1.0,0,1.8);
//   plotMuTau(125,1,"inclusive",""   ,"decayMode",     "#tau_{h} decay mode","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
//   plotMuTau(125,1,"inclusive",""   ,"visibleTauMass","visible #tau_{h} mass","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  
  /*
  plotMuTau(125,1,"inclusive",""   ,"MEtMVA","MET","GeV"                        ,outputDir,40,0,100,5.0,1.0,0,1.2);
  plotMuTau(125,1,"inclusive",""   ,"MEtMVAPhi","MET #phi","units"              ,outputDir,32,-3.2,3.2,   5.0,1.0,0,1.5);
  plotMuTau(125,1,"inclusive",""   ,"MtLeg1MVA","M_{T}(#mu#nu)","GeV" ,                  outputDir,40,0,160,5.0,1.0,0,1.2);

  plotMuTau(125,1,"inclusive",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,50,0,200,5.0,1.0,0,1.2);  
  plotMuTau(125,1,"inclusive",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,60,0,300,5.0,1.0,0,1.2);
  plotMuTau(125,1,"inclusive",""   ,"etaL1","#mu #eta", "units"              ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  plotMuTau(125,1,"inclusive",""   ,"ptL1","#mu p_{T}", "GeV"                ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  
  plotMuTau(125,1,"inclusive",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  plotMuTau(125,1,"inclusive",""   ,"ptL1","#mu p_{T}", "GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  plotMuTau(125,0,"inclusive",""   ,"etaL1","#mu #eta", "units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  plotMuTau(125,0,"inclusive",""   ,"etaL2","#tau #eta","units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  
  plotMuTau(125,0,"inclusive",""   ,"numPV","reconstructed vertexes","units"             ,outputDir,30,0,30,5.0,1.0,0,1.5);
  //   plotMuTau(125,1,"inclusive",""   ,"nJets30","jet multiplicity","units"                 ,outputDir,10,0, 10,5.0,1.0,1,10);
  //   plotMuTau(125,1,"inclusive",""   ,"nJets20BTagged","b-tagged jet multiplicity","units" ,outputDir,5,0, 5,5.0,1.0,1,10);
  */

//   //plotMuTau(125,1,"bTag",""        ,"ptB1", "leading b-tagged jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
//   //plotMuTau(125,1,"bTag",""        ,"etaB1","leading b-tagged jet #eta","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);

//   plotMuTau(125,1,"oneJet",""      ,"eta1","leading jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
//   plotMuTau(125,1,"oneJet",""     , "pt1","leading jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,200);
  
  
  //plotMuTau(125,1,"twoJets",""     ,"pt1","leading jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,200);
  //plotMuTau(125,1,"twoJets",""     ,"pt2","trailing jet p_{T}","GeV"      ,outputDir,50,30, 330,5.0,1.0,1,100);
  //plotMuTau(125,1,"twoJets",""     ,"eta1","leading jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"twoJets",""     ,"eta2","trailing jet #eta","units"    ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"twoJets",""     ,"Deta","|#Delta#eta|_{jj}","units"    ,outputDir,20,0, 8,   5.0,1.0,0,1.5);
  //plotMuTau(125,1,"twoJets",""     ,"Mjj","M_{jj}","GeV"                  ,outputDir,20,0, 1000,5.0,1.0,1,100);
  //plotMuTau(125,1,"twoJets",""     ,"MVAvbf","BDT output","units"         ,outputDir,10,-1, 1,5.0,1.0,1,100);
  //plotMuTau(125,1,"twoJets",""     ,"ptVeto","veto jet p_{T}","GeV"       ,outputDir,20,30, 230,5.0,1.0,1,200);
  //plotMuTau(125,1,"twoJets",""     ,"etaVeto","veto jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotMuTau(125,1,"twoJets",""     ,"diJetPt","di-jet p_{T}","GeV"           ,outputDir,30,0, 300,5.0,1.0,0,1.5);
  //plotMuTau(125,1,"twoJets",""     ,"dPhiHjet","(di-jet - H) #phi","units",outputDir,32,0, 3.2,5.0,1.0,0,1.5);
  //plotMuTau(125,1,"twoJets",""     ,"c2","di-tau vis p_{T}","GeV"         ,outputDir,20,0, 100,5.0,1.0,0,1.5);
  //plotMuTau(125,1,"twoJets",""     ,"c1","min#Delta#eta j-H","units"     ,outputDir,20,0,10,5.0,1.0,0,1.5);

  //plotMuTau(125,1,"twoJets",""     ,"diTauRecoPt","(di-tau+MET) p_{T}","GeV"         ,outputDir,40,0, 400,5.0,1.0,0,1.5);
  //plotMuTau(125,1,"twoJets",""     ,"Dphi","#Delta#phi_{jj}","units"        ,outputDir,32,0, 3.2,5.0,1.0,0,1.5);

  //return;

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
      /*
      plotMuTau(mH[j],useEmbedded,"bTagLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      
      plotMuTau(mH[j],useEmbedded,"bTagHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"bTagHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      */

      plotMuTau(mH[j],useEmbedded,"vbf",""         ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","TauUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","TauDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","JetUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","JetDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      
      //plotMuTau(mH[j],useEmbedded,"vh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"vh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"vh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"vh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"vh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);



      /*
      plotMuTau(mH[j],useEmbedded,"novbf","Raw"       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"novbf","RawTauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"novbf","RawTauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"novbf","RawJetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"novbf","RawJetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      
      plotMuTau(mH[j],useEmbedded,"vbf","Raw"         ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","RawTauUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","RawTauDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","RawJetUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"vbf","RawJetDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

      plotMuTau(mH[j],useEmbedded,"boost","Raw"       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"boost","RawTauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"boost","RawTauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"boost","RawJetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      plotMuTau(mH[j],useEmbedded,"boost","RawJetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      */




      //plotMuTau(mH[j],useEmbedded,"boost2",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"boost2","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"boost2","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"boost2","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"boost2","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

      //plotMuTau(mH[j],useEmbedded,"twoJets",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"twoJets","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"twoJets","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"twoJets","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"twoJets","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      
      //plotMuTau(mH[j],useEmbedded,"oneJet",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"oneJet","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"oneJet","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"oneJet","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      //plotMuTau(mH[j],useEmbedded,"oneJet","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,5.0,1.0,0,1.2);
      
    }
  }
  


}




int main(int argc, const char* argv[])
{

  std::cout << "plotMuTau()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  plotMuTauAll();
  //plotMuTauA();
  //plotMuTau(125,1,"",""   ,"MtLeg1MVA","M_{T}","GeV" ,"./",40,0,160,5.0,1.0,0,1.2);

  //plotMuTau(125,1,"inclusive","","diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotMuTau(125,1,"novbf","","diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotMuTau(125,1,"boost","","diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotMuTau(125,1,"vbf","","diTauVisMass","","","Dec2011/iter3",10,0,7000);

}
