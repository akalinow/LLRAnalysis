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

void plotElecTau( Int_t mH_           = 120,
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

  string postfix_ = "Raw";

  ofstream out(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/yieldsElecTau_mH%d_%s_%s.txt",outputDir.Data(),mH_,selection_.c_str(), analysis_.c_str() ),ios_base::out); 
  out.precision(5);
  out<< " => " << selection_ << endl;

  // input txt file with bins
  ifstream is;

  char* c = new char[10];
  is.open(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/bins/bins_eTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
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
  is.open(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/bins/bins_eTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  
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

  // Luminosity analyzed & parameters
  //float Lumi = (-47.4 + 215.3 + 930.7 + 410.6 + (450.6+212.7) + (735.2+254.8+778.2+682.0) )*1.00;
  //float Lumi = (-47.4 + 220.9 + 965.3 + 390.6 + 706.4 + 2735)*1.00;

  // from lumiPixel
  float Lumi   = (-47.4 + 215.6 + 955.3 + 389.9 + 706.7 + 2714);
  float lumiCorrFactor = (1-0.056);
  //Lumi *= lumiCorrFactor;

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  float TTxsectionRatio           = lumiCorrFactor*(165.8/157.5) ;

  float OStoSSRatioQCD            = 1.07;
  float SSIsoToSSAIsoRatioQCD     = 1.00;

  float EtoTauCorrectionFactor    = 1.00;
  float JtoTauCorrectionFactor    = 1.00;

  float embeddedMEtCutEff         = 1.00;
  float madgraphMEtCutEff         = 1.00;

  // Fall11_06Dec2011
  float WcorrectionFactorOS        = 1.04;  
  float WcorrectionFactorSS        = 1.39; 
  float ExtrapolationFactorZ       = 1.0;
  float ExtrapolationFactorZFakes  = 1.0;
  float ExtrapolationFactorZDataMC = 1.0;
  float ErrorExtrapolationFactorZ  = 1.0;

  //float NoVbfExtrapolationFactorZ = 0.996;
  //float VbfExtrapolationFactorZ   = 1.23;
  //float BoostExtrapolationFactorZ = 1.11;

  float VbfExtrapolationFactorW   = 1.0;
  float BoostExtrapolationFactorW = 1.0;
  float scaleFactorTTinVBF        = 1.00;
  float scaleFactorTTOS           = 1.0;

  /////////////////  change SVfit mass here ///////////////////

  //string variableStr = "";
  //TString variable(variableStr.c_str());
  TString variable = variable_;

  //////////////////////////////////////////////////////////////

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1Corr" : "-(pZetaCorr-1.5*pZetaVisCorr)" ;
  float antiWsgn  = useMt ? 40. :  20. ; 
  float antiWsdb  = useMt ? 70. :  40. ; 

  bool use2Dcut   = false;
  if( use2Dcut ){
    antiWcut = "!(MtLeg1Corr<40 && (pZetaCorr-1.5*pZetaVisCorr)>-20)";
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
  leg->SetHeader(Form("#splitline{CMS Preliminary #sqrt{s}=7 TeV}{%.1f fb^{-1} #tau_{e}#tau_{had}}", Lumi/1000. ));

  THStack* aStack = new THStack("aStack","");

  TH1F* hSiml    = new TH1F( "hSiml"   ,"all"               , nBins , bins.GetArray());
  TH1F* hSgn     = new TH1F( "hSgn "   ,"vbf+ggf+vh"        , nBins , bins.GetArray());
  TH1F* hSgn1    = new TH1F( "hSgn1"   ,"vbf"               , nBins , bins.GetArray());
  TH1F* hSgn2    = new TH1F( "hSgn2"   ,"ggf"               , nBins , bins.GetArray());
  TH1F* hSgn3    = new TH1F( "hSgn3"   ,"vh"                , nBins , bins.GetArray());
  TH1F* hData    = new TH1F( "hData"   ,"        "          , nBins , bins.GetArray());
  TH1F* hDataEmb = new TH1F( "hDataEmb","Embedded"          , nBins , bins.GetArray());
  TH1F* hW       = new TH1F( "hW"      ,"W+jets"            , nBins , bins.GetArray());
  TH1F* hWLooseIso2 = new TH1F( "hWLooseIso2","W+jets(miso)", nBins , bins.GetArray());
  TH1F* hW3Jets  = new TH1F( "hW3Jets" ,"W+3jets"           , nBins , bins.GetArray());
  TH1F* hEWK     = new TH1F( "hEWK"    ,"EWK"               , nBins , bins.GetArray());
  TH1F* hZtt     = new TH1F( "hZtt"    ,"Ztautau"           , nBins , bins.GetArray());
  TH1F* hZmm     = new TH1F( "hZmm"    ,"Z+jets, e to tau"  , nBins , bins.GetArray());
  TH1F* hZmmLoose= new TH1F( "hZmmLoose","Z+jets, mu to tau (loose)", nBins , bins.GetArray());
  TH1F* hZmj     = new TH1F( "hZmj"    ,"Z+jets, jet to tau", nBins , bins.GetArray());
  TH1F* hZmjLoose= new TH1F( "hZmjLoose","Z+jets, jet to tau (loose)",nBins , bins.GetArray());
  TH1F* hZfakes  = new TH1F( "hZfakes" ,"Z+jets, jet to tau", nBins , bins.GetArray());
  TH1F* hTTb     = new TH1F( "hTTb"    ,"ttbar"             , nBins , bins.GetArray());
  TH1F* hQCD     = new TH1F( "hQCD"    ,"QCD"               , nBins , bins.GetArray());
  TH1F* hLooseIso= new TH1F( "hLooseIso","Loose Iso"        , nBins , bins.GetArray());
  TH1F* hLooseIso2= new TH1F( "hLooseIso2","Loose Iso"      , nBins , bins.GetArray());
  TH1F* hLooseIso3= new TH1F( "hLooseIso3","Loose Iso"      , nBins , bins.GetArray());
  TH1F* hAntiIso = new TH1F( "hAntiIso","Anti Iso"          , nBins , bins.GetArray());
  TH1F* hVV      = new TH1F( "hVV"     ,"Diboson"           , nBins , bins.GetArray());


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

  // get the FR-file

  TFile FakeRate("FakeRate.root","READ");
  if(FakeRate.IsZombie()){
    cout << "Missing FR histos... exit" << endl;
    return;
  }
 
  TF1*  frElec     = (TF1*)FakeRate.Get("fitElecTau_Elec_ptL1_incl");
  TH1F* frElecUp   = (TH1F*)FakeRate.Get("hFakeRateErrUpElecTau_Elec_ptL1_incl");
  TH1F* frElecDown = (TH1F*)FakeRate.Get("hFakeRateErrDownElecTau_Elec_ptL1_incl");

  TF1*  frTauQCD     = (TF1*)FakeRate.Get("fitQCD_ElecTau_Tau_ptL2_QCDSS02_WSS60_incl");
  TH1F* frTauQCDUp   = (TH1F*)FakeRate.Get("hFakeRateQCDErrUpElecTau_Tau_ptL2_QCDSS02_WSS60_incl");
  TH1F* frTauQCDDown = (TH1F*)FakeRate.Get("hFakeRateQCDErrDownElecTau_Tau_ptL2_QCDSS02_WSS60_incl");
  
  TF1*  frTauW     = (TF1*)FakeRate.Get("fitW_ElecTau_Tau_ptL2_QCDOS02_WOS60_incl");
  TH1F* frTauWUp   = (TH1F*)FakeRate.Get("hFakeRateWErrUpElecTau_Tau_ptL2_QCDOS02_WOS60_incl");
  TH1F* frTauWDown = (TH1F*)FakeRate.Get("hFakeRateWErrDownElecTau_Tau_ptL2_QCDOS02_WOS60_incl");

  if(!frElec || !frElecUp || !frElecDown || !frTauQCD || !frTauQCDUp || !frTauQCDDown || !frTauW || !frTauWUp || !frTauWDown){
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

  string scaleFactElec = "( ";
  string scaleFactElecUp   = "( ";
  string scaleFactElecDown = "( ";

  string scaleFactTauQCD = "( ";
  string scaleFactTauQCDUp   = "( ";
  string scaleFactTauQCDDown = "( ";

  string scaleFactTauW = "( ";
  string scaleFactTauWUp   = "( ";
  string scaleFactTauWDown = "( ";

  for(unsigned int i = 0; i < binsFR.size()-1; i++){
    
    float min = binsFR[i];
    float max = binsFR[i+1];

    float bin = frElecUp->FindBin((max+min)/2.);
    if( bin == frElecUp->GetNbinsX() + 1) bin--;

    float weightBinElec_i     =  frElec->Eval( (max+min)/2.);
    float weightBinElec_iUp   =  frElecUp->GetBinContent( bin );
    float weightBinElec_iDown =  frElecDown->GetBinContent( bin );
    
    if( weightBinElec_i <0)     weightBinElec_i     = 1.0;
    if( weightBinElec_iUp <0)   weightBinElec_iUp   = 1.0;
    if( weightBinElec_iDown <0) weightBinElec_iDown = 1.0;

    scaleFactElec     += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinElec_i ) );
    scaleFactElecUp   += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinElec_iUp   ) );
    scaleFactElecDown += string( Form("(ptL1>=%f && ptL1<%f)*%f", min , max, 1./weightBinElec_iDown ) );

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
      scaleFactElec     += " + ";
      scaleFactElecUp   += " + ";
      scaleFactElecDown += " + ";
      
      scaleFactTauQCD     += " + ";
      scaleFactTauQCDUp   += " + ";
      scaleFactTauQCDDown += " + ";
      
      scaleFactTauW     += " + ";
      scaleFactTauWUp   += " + ";
      scaleFactTauWDown += " + ";
    }
 }
 
 scaleFactElec     += " )";
 scaleFactElecUp   += " )";
 scaleFactElecDown += " )";
 
 scaleFactTauQCD     += " )";
 scaleFactTauQCDUp   += " )";
 scaleFactTauQCDDown += " )";
 
 scaleFactTauW     += " )";
 scaleFactTauWUp   += " )";
 scaleFactTauWDown += " )";
 
 cout << scaleFactElec << endl;
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
  TH1F* hParameters   = new TH1F( "hParameters", "" , 27, 0,27);

  // Open the files
  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fDataLooseIso              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleRun2011-ElecTau-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fDataEmbedded              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleRun2011-ElecTau-Embedded-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fSignalVBF         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",mH_) ,"READ");  
  TFile *fSignalGGH         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",mH_),"READ"); 
  TFile *fSignalVH         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",mH_),"READ");  
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleDYJets-ElecTau-50-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleWJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundW3Jets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleW3Jets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleTTJets-ElecTau-madgraph-PUS6_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis/nTupleOthers-ElecTau-PUS6_run_Open_ElecTauStream.root","READ"); 



  vector<int> hMasses;
  hMasses.push_back(110);
  hMasses.push_back(115);
  hMasses.push_back(120);
  hMasses.push_back(125);
  hMasses.push_back(130);
  hMasses.push_back(135);
  hMasses.push_back(140);
  hMasses.push_back(145);


  TFile *fSignalggH110 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",110) ,"READ");  
  TFile *fSignalggH115 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",115) ,"READ");  
  TFile *fSignalggH120 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",120) ,"READ");  
  TFile *fSignalggH125 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",125) ,"READ");  
  TFile *fSignalggH130 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",130) ,"READ");  
  TFile *fSignalggH135 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",135) ,"READ");  
  TFile *fSignalggH140 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",140) ,"READ");  
  TFile *fSignalggH145 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleGGFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",145) ,"READ");  

  TFile *fSignalqqH110 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",110) ,"READ");  
  TFile *fSignalqqH115 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",115) ,"READ");  
  TFile *fSignalqqH120 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",120) ,"READ");  
  TFile *fSignalqqH125 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",125) ,"READ");  
  TFile *fSignalqqH130 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",130) ,"READ");  
  TFile *fSignalqqH135 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",135) ,"READ");  
  TFile *fSignalqqH140 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",140) ,"READ");  
  TFile *fSignalqqH145 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVBFH%d-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",145) ,"READ");  
 
  TFile *fSignalVH110 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",110) ,"READ");  
  TFile *fSignalVH115 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",115) ,"READ");  
  TFile *fSignalVH120 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",120) ,"READ");  
  TFile *fSignalVH125 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",125) ,"READ");  
  TFile *fSignalVH130 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",130) ,"READ");  
  TFile *fSignalVH135 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",135) ,"READ");  
  TFile *fSignalVH140 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",140) ,"READ");  
  TFile *fSignalVH145 =          
    new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTupleVH%d-ElecTau-pythia-PUS6_run_Open_ElecTauStream.root",145) ,"READ");  
 
  std::map<string,TFile*> mapSUSYfiles;
  for(unsigned int i = 0; i < SUSYhistos.size() ; i++){
    mapSUSYfiles.insert( make_pair(SUSYhistos[i], new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamFall11_04May2012_Approval_thesis//nTuple%s-ElecTau-powheg-PUS6_run_Open_ElecTauStream.root",SUSYhistos[i].c_str()) ,"READ")  )  );
  }
  


  // choose the analysis: Nominal "", jet up/Down "JetUp/Down" , elec up/down "ElecUp/Down" , tau up/down "TauUp/Down"
  TString tree         = "outTreePtOrd"+postfix_+analysis_;
  TString treeEmbedded = "outTreePtOrd"+postfix_;
  if(analysis_.find("TauUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("TauDown")!=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("ElecUp")  !=string::npos) 
    treeEmbedded = tree;
  if(analysis_.find("ElecDown")!=string::npos) 
    treeEmbedded = tree;

  TTree *data                = (TTree*)fData->Get(("outTreePtOrd"+postfix_).c_str());
  TTree *dataLooseIso        = LOOSEISO  ? (TTree*)fDataLooseIso->Get(("outTreePtOrd"+postfix_).c_str()) : 0;
  TTree *dataEmbedded        = EMBEDDEDSAMPLES ? (TTree*)fDataEmbedded->Get(treeEmbedded) : 0;
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *signalVH            = addVH ? (TTree*)fSignalVH->Get(tree) : 0;

  // split the DY->ll into l=e/mu and l=tau (MC level) ===> temporary, need fix !!!
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> e+e- e->tau" << endl;
  TTree *backgroundDYEtoTau = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> mu+mu- mu->tau
  cout << "Now copying g/Z -> e+e- jet->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"         << endl;
  cout << backgroundDYEtoTau->GetEntries() << " come from DY->ee, e->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->ee, jet->tau" << endl;

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundW3Jets    = W3JETS ? (TTree*)fBackgroundW3Jets->Get(tree) : 0;
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 

  TTree *signalGGF110        = (TTree*)fSignalggH110->Get(tree);
  TTree *signalGGF115        = (TTree*)fSignalggH115->Get(tree);
  TTree *signalGGF120        = (TTree*)fSignalggH120->Get(tree);
  TTree *signalGGF125        = (TTree*)fSignalggH125->Get(tree);
  TTree *signalGGF130        = (TTree*)fSignalggH130->Get(tree);
  TTree *signalGGF135        = (TTree*)fSignalggH135->Get(tree);
  TTree *signalGGF140        = (TTree*)fSignalggH140->Get(tree);
  TTree *signalGGF145        = (TTree*)fSignalggH145->Get(tree);

  TTree *signalVBF110        = (TTree*)fSignalqqH110->Get(tree);
  TTree *signalVBF115        = (TTree*)fSignalqqH115->Get(tree);
  TTree *signalVBF120        = (TTree*)fSignalqqH120->Get(tree);
  TTree *signalVBF125        = (TTree*)fSignalqqH125->Get(tree);
  TTree *signalVBF130        = (TTree*)fSignalqqH130->Get(tree);
  TTree *signalVBF135        = (TTree*)fSignalqqH135->Get(tree);
  TTree *signalVBF140        = (TTree*)fSignalqqH140->Get(tree);
  TTree *signalVBF145        = (TTree*)fSignalqqH145->Get(tree);

  TTree *signalVH110        = (TTree*)fSignalVH110->Get(tree);
  TTree *signalVH115        = (TTree*)fSignalVH115->Get(tree);
  TTree *signalVH120        = (TTree*)fSignalVH120->Get(tree);
  TTree *signalVH125        = (TTree*)fSignalVH125->Get(tree);
  TTree *signalVH130        = (TTree*)fSignalVH130->Get(tree);
  TTree *signalVH135        = (TTree*)fSignalVH135->Get(tree);
  TTree *signalVH140        = (TTree*)fSignalVH140->Get(tree);
  TTree *signalVH145        = (TTree*)fSignalVH145->Get(tree);

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
  TCut lpt("ptL1>20 && TMath::Abs(etaL1)<2.1");
  
  TCut lID("((TMath::Abs(etaL1)<0.80 && mvaPOGNonTrig>0.925) || (TMath::Abs(etaL1)<1.479 && TMath::Abs(etaL1)>0.80 && mvaPOGNonTrig>0.975) || (TMath::Abs(etaL1)>1.479 && mvaPOGNonTrig>0.985))");

  lpt = lpt && lID;
  TCut tpt("ptL2>20 && TMath::Abs(etaL2)<2.3");

  if(selection_.find("High")!=string::npos)
    tpt = tpt&&TCut("ptL2>40");
  else if(selection_.find("Low")!=string::npos)
    tpt = tpt&&TCut("ptL2<40");
  
 
  ////// TAU ISO //////
  TCut tiso("tightestHPSMVAWP>=0 && tightestAntiECutWP>1"); 
  TCut tAiso("tightestHPSMVAWP>=0 && tightestAntiECutWP<1"); 
  TCut ltiso("tightestHPSMVAWP>-99 && tightestAntiECutWP>1"); 
  TCut mtiso("hpsMVA>0.0");

  ////// ELEC ISO ///////
  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

  ////// EVENT WISE //////
  TCut lveto("elecFlag==0");
  TCut lAveto("elecFlag==1");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut hltevent("pairIndex<1 && HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));


  ////// CATEGORIES ///
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  
  TCut vbf("pt1>30 && pt2>30 && (ptVeto<30 || isVetoInJets!=1) && MVAvbf>0.50");
  TCut vbfLoose("pt1>30 && pt2>30 && (ptVeto<30 || isVetoInJets!=1 && MVAvbf>-0.30)");

  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");

  TCut boost("pt1>30 && nJets20BTagged<1"); // <--- NEW
  boost = boost && !vbf /*&& !vh*/;
  if(selection_.find("novbf")!=string::npos || selection_.find("boost")!=string::npos )
    boost = boost&&TCut("MEtCorr>30");

  TCut boost2("pt1>100 && pt1<150 && !(pt2>30 && eta1*eta2<0 && Mjj>400 && Deta>4.0 && isVetoInJets!=1)");

  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut nobTag("nJets30<2 && nJets20BTagged==0");

  TCut novbf = !vbf /*&& !vh*/ && !boost && !bTag;
  if(selection_.find("novbf")!=string::npos || selection_.find("boost")!=string::npos )
    novbf = novbf&&TCut("MEtCorr>30");


  TCut sbin; TCut sbinEmbedding; TCut sbinEmbeddingPZetaRel; TCut sbinPZetaRel; TCut sbinSS; TCut sbinPZetaRelSS; TCut sbinPZetaRev; TCut sbinPZetaRevSS; TCut sbinSSaIso; TCut sbinSSlIso;TCut sbinSSlIso2; TCut sbinSSlIso3;
  TCut sbinSSaIsoLtiso;
  TCut sbinSSltiso;
  TCut sbinLtiso;
  TCut sbinMtiso;
  TCut sbinALept;


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
  TCut sbinALeptInclusive;
  sbinALeptInclusive       =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ && hltevent && hltmatch;

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
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch;
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
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && oneJet;
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
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && twoJets;

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
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && vbf;
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
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSlIso2            =  lpt && tpt && mtiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSlIso3            =  lpt && tpt && mtiso&& lliso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && vh;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && vh;
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && vh;
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
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && novbf;
  }
  else if(selection_.find("boost")!=string::npos ){
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
    sbinSSaIsoLtiso        =  lpt && tpt && ltiso&& laiso&& lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinSSltiso            =  lpt && tpt && ltiso&& liso && lveto && SS && pZ  && hltevent && hltmatch && boost;
    sbinLtiso              =  lpt && tpt && ltiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinMtiso              =  lpt && tpt && mtiso&& liso && lveto && OS && pZ  && hltevent && hltmatch && boost;
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && boost;
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
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && bTag;

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
    sbinALept              =  lpt && tpt && tAiso&& liso && lAveto&& OS && pZ  && hltevent && hltmatch && nobTag;

  }


  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << "******** Extrapolation factors for Z->tautau normalization ********" << endl;
  // inclusive DY->tautau:
  TH1F* hExtrap = new TH1F("hExtrap","",nBins , bins.GetArray());
  backgroundDYTauTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinInclusive);
  float ExtrapDYInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->tautau = " << ExtrapDYInclusive << endl; 

  backgroundDYEtoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinInclusive);
  float ExtrapLFakeInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->ee, e->tau = " << ExtrapLFakeInclusive << endl;

  backgroundDYJtoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinInclusive);
  float ExtrapJFakeInclusive = hExtrap->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  hExtrap->Reset();
  cout << "All Z->ee, j->tau = " << ExtrapJFakeInclusive << endl;



  TCut sbinInclusiveEmbeddedCut = sbinEmbeddingInclusive;
  TCut sbinEmbeddedCut          = sbinEmbedding;

  // if VBF, minimize ttbar contamination asking for 0 btag jet:
  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    sbinInclusiveEmbeddedCut = sbinInclusiveEmbeddedCut && TCut("nJets20BTagged<999");
    sbinEmbeddedCut          = sbinEmbeddedCut          && TCut("nJets20BTagged<999");
  }

  dataEmbedded->Draw(variable+">>hExtrap", "(HLTTau*HLTElec*embeddingWeight)"*sbinInclusiveEmbeddedCut);
  float ExtrapEmbedDen =  hExtrap->Integral();
  hExtrap->Reset();
  dataEmbedded->Draw(variable+">>hExtrap", "(HLTTau*HLTElec*embeddingWeight)"*sbinEmbeddedCut);
  float ExtrapEmbedNum =  hExtrap->Integral();
  hExtrap->Reset();

  ExtrapolationFactorZ = ExtrapEmbedNum/ExtrapEmbedDen;

  ErrorExtrapolationFactorZ = TMath::Sqrt(ExtrapolationFactorZ*(1-ExtrapolationFactorZ)/ExtrapEmbedDen);
  cout << "Extrap. factor using embedded sample: " << ExtrapolationFactorZ << " +/- " << ErrorExtrapolationFactorZ << endl;
  backgroundDYTauTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbin);

  float ExtrapolationFactorMadGraph = hExtrap->Integral()*lumiCorrFactor*Lumi*hltEff_/1000./ExtrapDYInclusive;
  cout << "MadGraph prediction = " << ExtrapolationFactorMadGraph << endl;
  ExtrapolationFactorZDataMC  = ExtrapolationFactorZ/ExtrapolationFactorMadGraph;
  cout << " ==> data/MC = " << ExtrapolationFactorZDataMC << endl;


  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////

  cout << "******** Extrapolation factors for QCD normalization ********" << endl;
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&pZ));
  float ExtrapSSWinSignalRegionMC   = hExtrap->Integral();
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapSSWinSidebandRegionMC = hExtrap->Integral();
  float ExtrapscaleFactorSS         = ExtrapSSWinSignalRegionMC>0 ? ExtrapSSWinSidebandRegionMC/ExtrapSSWinSignalRegionMC : 1.0;
  cout << " Extrapolation factor W SS (inclusive) " << ExtrapscaleFactorSS << endl;

  hExtrap->Reset();
  backgroundTTbar->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapttbarExtrSS    = hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;

  hExtrap->Reset();
  backgroundTTbar->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float ExtrapttbarExtrSSBtag = hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
  hExtrap->Reset();
  backgroundWJets->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
  float ExtrapwjetsExtrSSBtag = hExtrap->Integral()*Lumi/1000*hltEff_;
  hExtrap->Reset();
  backgroundOthers->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")));
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


  hExtrap->Reset();
  backgroundOthers->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
  float ExtrapothersExtrSS   = hExtrap->Integral()*Lumi/1000*hltEff_;
  hExtrap->Reset();
  backgroundDYJtoTau->Draw(variable+">>hExtrap","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSSInclusive&&apZ));
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
  backgroundTTbar->Draw(variable+">>hExtrap", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi/1000*hltEff_*TTxsectionRatio*scaleFactorTTSS;
  hExtrap->Reset();
  backgroundDYEtoTau->Draw(variable+">>hExtrap", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi*lumiCorrFactor/1000*hltEff_*EtoTauCorrectionFactor;

  hExtrap->Reset();
  backgroundDYJtoTau->Draw(variable+">>hExtrap", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSSInclusive);
  SSeventsExtrap  -= hExtrap->Integral()*Lumi*lumiCorrFactor/1000*hltEff_*JtoTauCorrectionFactor;
  hExtrap->Reset();

  SSeventsExtrap *= OStoSSRatioQCD;

  dataLooseIso->Draw(variable+">>hExtrap", sbinSSaIsoInclusive);
  float SSeventsExtrapAiso = hExtrap->GetEntries();
  SSIsoToSSAIsoRatioQCD = SSeventsExtrap/SSeventsExtrapAiso ;
  cout << "The extrapolation factor Iso>0.3 / Iso<0.1 is " << SSIsoToSSAIsoRatioQCD << endl;

  // Extrapolation for Z->ee

  hExtrap->Reset();
  data->Draw(variable+">>hExtrap",   sbinALeptInclusive&&TCut("diTauVisMass>80 && diTauVisMass<100"));
  float ExtrapolationLeptFakeTauDen = hExtrap->Integral();
  hExtrap->Reset();
  data->Draw(variable+">>hExtrap",   sbinALept&&TCut("diTauVisMass>80 && diTauVisMass<100"));
  float ExtrapolationLeptFakeTauNum = hExtrap->Integral();
  hExtrap->Reset();

  ExtrapolationFactorZFakes = ExtrapolationLeptFakeTauNum/ExtrapolationLeptFakeTauDen ;
  cout << "Using inverted 2nd lepton veto: " << ExtrapolationLeptFakeTauDen << " events in inclusive selection, "
       << ExtrapolationLeptFakeTauNum << " events in category" << endl;
  cout << " ==> extrapolation factor is " << ExtrapolationFactorZFakes << endl;

  cout << "************** END extrapolation *******************" << endl;
  delete hExtrap;
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////


  // estimate the W+jets in the selection bin using pZeta extrapolation

  //TH1F* hWMt = new TH1F("hWMt","",1,-10,10);
  TH1F* hWMt = new TH1F("hWMt","",nBins , bins.GetArray());

  ///////////////////////////////////////// Doing OS first...
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the W+jets OS sample" << endl;
  float OSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float OSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorOS = OSWinSignalRegionMC>0 ? OSWinSidebandRegionMC/OSWinSignalRegionMC : 1.0 ;

  if(useMt)
    cout << "Extrapolation factor for W OS : P(MtCorr>" << antiWsdb << ")/P(MtCorr<" << antiWsgn << ") ==> " << scaleFactorOS << endl;
  else
    cout << "Extrapolation factor for W OS : P(pZetaCorr<- "<< antiWsdb << ")/P(pZetaCorr>"<< antiWsgn << ") ==> " << scaleFactorOS << endl;    
 
  hWMt->Reset();
  cout << "Estimating cobtribution from Ztt, ttbar and others in OS low pZeta tail..." << endl;
  backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float ttbarExtrOS  = hWMt->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio;
  cout << "Contribution from ttbar in OS is " << ttbarExtrOS << endl;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRel&&apZ&&TCut("nJets20BTagged>1")));
  float ttbarExtrOSBtag = hWMt->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRel&&apZ&&TCut("nJets20BTagged>1")));
  float wjetsExtrOSBtag = hWMt->Integral()*Lumi/1000*hltEff_;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRel&&apZ&&TCut("nJets20BTagged>1")));
  float othersExtrOSBtag = hWMt->Integral()*Lumi/1000*hltEff_;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    data->Draw(variable+">>hWMt",(sbinPZetaRelInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    data->Draw(variable+">>hWMt",(sbinPZetaRel&&apZ&&TCut("nJets20BTagged>1")));

  float dataOSBtag = hWMt->Integral();
  scaleFactorTTOS = ttbarExtrOSBtag>0 ? dataOSBtag/ttbarExtrOSBtag : 1.0;
  cout << "Normalizing TTbar from sideband: " << ttbarExtrOSBtag << " events expected from TTbar, " 
       << ", from WJets" << wjetsExtrOSBtag <<  ", expected from others " << othersExtrOSBtag << ", observed " << dataOSBtag << endl;
  cout << "====> scale factor for OS ttbar is " << scaleFactorTTOS << endl;
  //if( (dataOSBtag-ttbarExtrOSBtag)/TMath::Sqrt(dataOSBtag) > 1)
  ttbarExtrOS *= scaleFactorTTOS;
  cout << "Contribution from ttbar in OS is " << ttbarExtrOS << endl;

  backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float othersExtrOS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in OS is " << othersExtrOS << endl;
  hWMt->Reset();
  backgroundDYTauTau->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float dytautauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->tautau in OS is " << dytautauExtrOS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float dyjtotauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->ee, jet->tau in OS is " << dyjtotauExtrOS << endl;
  hWMt->Reset();
  backgroundDYEtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRel&&apZ));
  float dyetotauExtrOS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->ee, e->tau in OS is " << dyetotauExtrOS << endl;

  float OSWinSignalRegionDATA = data->GetEntries(sbinPZetaRev);
  cout << "Selected events in data in low pZeta/low Mt tail " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= ttbarExtrOS;
  OSWinSignalRegionDATA -= othersExtrOS;
  OSWinSignalRegionDATA -= dytautauExtrOS;
  OSWinSignalRegionDATA -= dyjtotauExtrOS;
  OSWinSignalRegionDATA -= dyetotauExtrOS;
  OSWinSignalRegionDATA /= scaleFactorOS;
  cout << "W+jets in signal region is estimated to be "  
       << OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS << " = " 
       << OSWinSignalRegionDATA <<  " +/- " << sqrt(OSWinSignalRegionDATA/scaleFactorOS)/scaleFactorOS << endl;
  cout << "  ===> the MC prediction was " << OSWinSignalRegionMC << endl;

  hParameters->SetBinContent(1, 1./scaleFactorOS );
  hParameters->SetBinContent(2, OSWinSignalRegionDATA*scaleFactorOS );

  ///////////////////////////////////////// Doing SS last...
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSS&&pZ));
  cout << "Using  " << hWMt->GetEntries() << " entries from the SS W+jets sample" << endl;
  float SSWinSignalRegionMC   = hWMt->Integral()*Lumi*hltEff_/1000.;
  hWMt->Reset();
  backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSS&&apZ));
  float SSWinSidebandRegionMC = hWMt->Integral()*Lumi*hltEff_/1000.;
  float scaleFactorSS = SSWinSignalRegionMC>0 ? SSWinSidebandRegionMC/SSWinSignalRegionMC : 1.0;
 
  if(useMt)
    cout << "Extrapolation factor for W SS : P(MtCorr>" << antiWsdb << ")/P(MtCorr<" << antiWsgn << ") ==> " << scaleFactorSS << endl;
  else
    cout << "Extrapolation factor for W SS : P(pZetaCorr<- "<< antiWsdb << ")/P(pZetaCorr>"<< antiWsgn << ") ==> " << scaleFactorSS << endl;    

  hWMt->Reset();
  cout << "Estimating cobtribution Ztt,from ttbar and others in SS low pZeta tail..." << endl;
  backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSS&&apZ));
  float ttbarExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.*TTxsectionRatio;
  cout << "Contribution from ttbar in SS is " << ttbarExtrSS << endl;


  if(selection_.find("boost")!=string::npos)
    backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundTTbar->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSS&&apZ&&TCut("nJets20BTagged>1")));
  float ttbarExtrSSBtag = hWMt->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundWJets->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSS&&apZ&&TCut("nJets20BTagged>1")));
  float wjetsExtrSSBtag = hWMt->Integral()*Lumi/1000*hltEff_;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSS&&apZ&&TCut("nJets20BTagged>1")));
  float othersExtrSSBtag = hWMt->Integral()*Lumi/1000*hltEff_;
  hWMt->Reset();

  if(selection_.find("boost")!=string::npos)
    data->Draw(variable+">>hWMt",(sbinPZetaRelSSInclusive&&apZ&&TCut("nJets20BTagged>1")&&TCut("pt1>30")&&(!vbf)));
  else
    data->Draw(variable+">>hWMt",(sbinPZetaRelSS&&apZ&&TCut("nJets20BTagged>1")));
  float dataSSBtag2 = hWMt->Integral();
  float scaleFactorTTSS2 = ttbarExtrSSBtag>0 ? dataSSBtag2/ttbarExtrSSBtag : 1.0;
  cout << "Normalizing TTbar from sideband: " << ttbarExtrSSBtag << " events expected from TTbar, " 
       << ", from WJets " << wjetsExtrSSBtag << ", expected from others " << othersExtrSSBtag << ", observed " << dataSSBtag2 << endl;
  cout << "====> scale factor for SS ttbar is " << scaleFactorTTSS2 << endl;
  //if( (dataSSBtag2-ttbarExtrSSBtag)/TMath::Sqrt(dataSSBtag2) > 1) 
  ttbarExtrSS *= scaleFactorTTSS2;
  cout << "Contribution from ttbar in SS is " << ttbarExtrSS << endl;


  hWMt->Reset();
  backgroundOthers->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSS&&apZ));
  float othersExtrSS = hWMt->Integral()*Lumi*hltEff_/1000.;
  cout << "Contribution from single-t and di-boson in SS is " << othersExtrSS << endl;
  hWMt->Reset();
  backgroundDYJtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*(sbinPZetaRelSS&&apZ));
  float dyjtotauExtrSS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->ee, jet->tau in SS is " << dyjtotauExtrSS << endl;
  hWMt->Reset();
  backgroundDYEtoTau->Draw(variable+">>hWMt","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSS&&apZ));
  float dyetotauExtrSS = hWMt->Integral()*Lumi*lumiCorrFactor*hltEff_/1000.;
  cout << "Contribution from DY->ee, e->tau in SS is " << dyetotauExtrSS << endl;
  
  float SSWinSignalRegionDATA = data->GetEntries(sbinPZetaRevSS);
  cout << "Selected events in data in low pZeta/low Mt tail " << SSWinSignalRegionDATA << endl;
  SSWinSignalRegionDATA -= ttbarExtrSS;
  SSWinSignalRegionDATA -= othersExtrSS;
  SSWinSignalRegionDATA -= dyjtotauExtrSS;
  SSWinSignalRegionDATA -= dyetotauExtrSS;
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
  samples.push_back("DYEtoTau");
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
  tMap["ggH115"]       = signalGGH;
  tMap["qqH115"]       = signalVBF;
  tMap["VH115"]        = signalVH;
  tMap["DYToTauTau"]   = backgroundDYTauTau;
  tMap["DYEtoTau"]    = backgroundDYEtoTau;
  tMap["DYJtoTau"]     = backgroundDYJtoTau;
  tMap["WJets"]        = backgroundWJets;
  tMap["W3Jets"]       = backgroundW3Jets;
  tMap["Others"]       = backgroundOthers;
  tMap["TTbar"]        = backgroundTTbar;
  tMap["SS"]           = data;

  tMap["ggH110"]       = signalGGF110;
  tMap["ggH115"]       = signalGGF115;
  tMap["ggH120"]       = signalGGF120;
  tMap["ggH125"]       = signalGGF125;
  tMap["ggH130"]       = signalGGF130;
  tMap["ggH135"]       = signalGGF135;
  tMap["ggH140"]       = signalGGF140;
  tMap["ggH145"]       = signalGGF145;

  tMap["qqH110"]       = signalVBF110;
  tMap["qqH115"]       = signalVBF115;
  tMap["qqH120"]       = signalVBF120;
  tMap["qqH125"]       = signalVBF125;
  tMap["qqH130"]       = signalVBF130;
  tMap["qqH135"]       = signalVBF135;
  tMap["qqH140"]       = signalVBF140;
  tMap["qqH145"]       = signalVBF145;

  tMap["VH110"]       = signalVH110;
  tMap["VH115"]       = signalVH115;
  tMap["VH120"]       = signalVH120;
  tMap["VH125"]       = signalVH125;
  tMap["VH130"]       = signalVH130;
  tMap["VH135"]       = signalVH135;
  tMap["VH140"]       = signalVH140;
  tMap["VH145"]       = signalVH145;
  

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
      int SSevents = hHelp->GetEntries(); // <----------------------------------  SHOULD BE Integral()
      cout << "Selected SS events in data " << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);

      hHelp->Reset();
      backgroundWJets->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from W+jets (from " << hHelp->GetEntries() << " entries)" << endl;

      float sFWSS = ( selection_.find("novbf")!=string::npos  || 
		      selection_.find("bTag") !=string::npos  || 
		      selection_.find("boost")!=string::npos  ||
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
      backgroundTTbar->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio << " SS events from TTbar (from " << hHelp->GetEntries() << " entries)" << endl;
      hHelp->Scale(1.0*Lumi/1000*hltEff_*TTxsectionRatio);
      cout << "We estimate " << hHelp->Integral() << " SS events from TTbar" << endl;
      cout << " ==> removing TTbar from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC TTbar SS events
      cout << sqrt(error2OnQCD) << " <== W + TTb" << endl;      

      hHelp->Reset();
      backgroundDYEtoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi*lumiCorrFactor/1000*hltEff_ << " SS events from DY->ee, e->jet" << endl;
      hHelp->Scale(EtoTauCorrectionFactor*Lumi*lumiCorrFactor/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->ee, e->tau" << endl;
      cout << " ==> removing DY->ee, e->tau from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->mumu, mu->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1)" << endl;      

      hHelp->Reset();
      backgroundDYJtoTau->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFTau*SFElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi*lumiCorrFactor/1000*hltEff_ << " SS events from DY->ee, jet->tau" << endl;
      hHelp->Scale(JtoTauCorrectionFactor*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->ee, jet->tau" << endl;
      cout << " ==> removing DY->ee, e->jet from SS...." << endl;
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
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*sbinPZetaRelInclusive);  
	  float madgraphNoMEtCut = h1->Integral();
	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*sbinInclusive);
	  madgraphMEtCutEff = h1->Integral()/madgraphNoMEtCut;
	  cout << "Efficiency of antiW cut on madgraph " << madgraphMEtCutEff << endl;
	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*sbin);
	}
	else if((it->first).find("W3Jets")!=string::npos){
	  
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  hW3JetsKeys = new TH1Keys("hW3JetsKeys","W+3jets smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hW3JetsKeys", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  cout << "Keys for W3Jets filled with integral " << hW3JetsKeys->Integral() << " and entries " << hW3JetsKeys->GetEntries() << endl;


	  cout << "Filling histo with loose taus... if vbf, apply loose vbf cut" << endl;
	  h1->Reset();
	  if((selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) || selection_.find("twoJets")!=string::npos ){

	    if( selection_.find("twoJets")!=string::npos ){
              vbfLoose = twoJets;
              vbf      = twoJets;
            }

	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinMtisoInclusive&&vbfLoose));
	    hW3JetsLooseTauIso->Add(h1,1.0);

	    
	    // NORMALIZE W3JETS ON DATA
	  
	    //TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());

	    // SIDEBAND OS: get extrap factor
	    hHelp->Reset();
	    currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ&&vbfLoose));
	    cout << hHelp->Integral() << endl;
	    float W3JetsVBFSdb    = hHelp->Integral();
	    float W3JetsVBFSdbInt = hHelp->GetEntries();
	    hHelp->Reset();
	    currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&pZ&&vbfLoose));
	    float W3JetsVBFSgn    = hHelp->Integral();
	    float W3JetsVBFSgnInt = hHelp->GetEntries();
	    cout << "W3jets MC in VBF(rel): using " << W3JetsVBFSdbInt << " events from sideband and " << W3JetsVBFSgnInt << " from signal region" << endl;
	    float ExtrapFactorW3Jets = W3JetsVBFSdb/W3JetsVBFSgn;
	    cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorW3Jets << endl;
	    hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&vbfLoose));
            cout << hHelp->Integral() << endl;
            float W3JetsVBFAll    = hHelp->Integral();
            float W3JetsVBFAlllInt = hHelp->GetEntries();

	    // SIDEBAND2 OS: get extrap factor (2)
	    hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbfLoose));
            cout << hHelp->Integral() << endl;
            float W3JetsVBFSdb2    = hHelp->Integral();
            float W3JetsVBFSdbInt2 = hHelp->GetEntries();
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&pZ&&vbfLoose));
            float W3JetsVBFSgn2    = hHelp->Integral();
            float W3JetsVBFSgnInt2 = hHelp->GetEntries();
            cout << "W3jets MC in VBF(rel): using " << W3JetsVBFSdbInt2 << " events from sideband (2) and " << W3JetsVBFSgnInt2 << " from signal region" << endl;
            float ExtrapFactorW3Jets2 = W3JetsVBFSdb2/W3JetsVBFSgn2;
            cout << " ==> ratio sdb2/sgn = " <<  ExtrapFactorW3Jets2 << endl;


	    // SIDEBAND SS: get extrap fact SS
	    hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbfLoose));
	    cout << hHelp->Integral() << endl;
            float W3JetsVBFSdbSS    = hHelp->Integral();
            float W3JetsVBFSdbIntSS = hHelp->GetEntries();
            hHelp->Reset();
            currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbfLoose));
	    float W3JetsVBFSgnSS    = hHelp->Integral();
            float W3JetsVBFSgnIntSS = hHelp->GetEntries();
            cout << "W3jets MC (SS) in VBF(rel): using " << W3JetsVBFSdbIntSS << " events from sideband SS (2) and " << W3JetsVBFSgnIntSS << " from signal region" << endl;
            float ExtrapFactorW3JetsSS = W3JetsVBFSdbSS/W3JetsVBFSgnSS;
	    cout << " ==> ratio sdb/sgn = " <<  ExtrapFactorW3JetsSS << endl;



	    // now, normalize to sidebands
	    cout << "#######  Start normalization OS from sidebands #######" << endl;
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&vbf);
	    float DataVBFSdb = hHelp->Integral();
	    cout << "In VBF region, I get " << DataVBFSdb << " events in the high Mt sdb" << endl;

	    hHelp->Reset();
	    backgroundDYTauTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dytotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dytotauVBFSdb << " events in the high Mt sdb from DYtotautau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYEtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dymutotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dymutotauVBFSdb << " events in the high Mt sdb from DYmutotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float dyjtotauVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSdb << " events in the high Mt sdb from DYjtotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float othersVBFSdb = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSdb << " events in the high Mt sdb from others" << endl;
	    hHelp->Reset();

	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
	    float TTbarVBFSdb = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    cout << "In VBF region, I expect " << TTbarVBFSdb << " events in the high Mt sdb from TTbar" << endl;
	    hHelp->Reset();

	    //////////////////  Here, normalize TTbar from sideband 

	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float TTbarVBFSdbBtag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    hHelp->Reset();
	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>-999")));
	    float TTbarVBFSdbAnyBtag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    float ratiobTagToAny = TTbarVBFSdbBtag/TTbarVBFSdbAnyBtag;
	    cout << "The ratio between events with >0 bTagged jets to any in TTJets MC is " << ratiobTagToAny << endl;
	    hHelp->Reset();

	    data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0"));
	    float DataVBFSdbBtag = hHelp->Integral();
	    cout << "In data, I measure " << DataVBFSdbBtag << " events b-tagged" << endl;

	    hHelp->Reset();
	    backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float WJetsVBFSdbBtag = hHelp->Integral()*Lumi/1000*hltEff_;
	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float OthersVBFSdbBtag = hHelp->Integral()*Lumi/1000*hltEff_;
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float QCDVBFSdbBtag = hHelp->Integral()*OStoSSRatioQCD;
	    cout << "I subtract " << WJetsVBFSdbBtag << " and " << OthersVBFSdbBtag << " and " << QCDVBFSdbBtag << " events from W=jets and others" << endl;

	    scaleFactorTTinVBF = (DataVBFSdbBtag-WJetsVBFSdbBtag-OthersVBFSdbBtag-QCDVBFSdbBtag)/TTbarVBFSdbBtag;
	    cout << "The SF Data/MC ratio is therefore " << scaleFactorTTinVBF << endl;
	    cout << " ==> TTbar prediction in sideband goes from " << TTbarVBFSdbAnyBtag << " to " <<  (DataVBFSdbBtag-WJetsVBFSdbBtag-OthersVBFSdbBtag-QCDVBFSdbBtag)/ratiobTagToAny << endl;

	    hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbfLoose));
            cout << hHelp->Integral() << endl;
            float TTJetsVBFSdb2    = hHelp->Integral();
            float TTJetsVBFSdbInt2 = hHelp->GetEntries();
            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&pZ&&vbfLoose));
            float TTJetsVBFSgn2    = hHelp->Integral();
            float TTJetsVBFSgnInt2 = hHelp->GetEntries();
            cout << "TT MC in VBF(rel): using " << TTJetsVBFSdbInt2 << " events from sideband (2) and " << TTJetsVBFSgnInt2 << " from signal region" << endl;
            float ExtrapFactorTTJets2 = TTJetsVBFSdb2/TTJetsVBFSgn2;
            cout << " ==> ratio sdb2/sgn = " <<  ExtrapFactorTTJets2 << endl;

	    ///////////////////////////////////////////////////////////////////////////////////////
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ2&&vbf));

	    float QCDVBFSdb = hHelp->Integral()*OStoSSRatioQCD;
	    cout << "In VBF region, I measure " <<  hHelp->GetEntries() << " events ==> X FR = " << QCDVBFSdb << " events in the high Mt sdb from anti-isolated events" << endl;

	    cout << "Subtracting the backgrounds from the sideband..." << endl;
	    //DataVBFSdb -= TTbarVBFSdb;
	    DataVBFSdb -= (DataVBFSdbBtag-WJetsVBFSdbBtag-OthersVBFSdbBtag/ratiobTagToAny);
	    DataVBFSdb -= QCDVBFSdb;
	    DataVBFSdb -= dytotauVBFSdb;
	    DataVBFSdb -= dymutotauVBFSdb;
	    DataVBFSdb -= dyjtotauVBFSdb;
	    DataVBFSdb -= othersVBFSdb;
	    float normalization = ExtrapFactorW3Jets>0 ? DataVBFSdb/ExtrapFactorW3Jets : -99;
	    cout << "Estimation of W+jets in the VBF category is " << normalization << endl;

	    hW3JetsLooseTauIso->Scale(normalization/hW3JetsLooseTauIso->Integral());

	    //hW3JetsLooseTauIso->Scale(  W3JetsVBFAll/W3JetsVBFSdb*DataVBFSdb / hW3JetsLooseTauIso->Integral());  //<----------------------------------
// 	    hHelp->Reset();
//          data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&vbf);
//          float DataVBFSdb2 = hHelp->Integral();
//          cout << "In VBF region, I get " << DataVBFSdb2 << " events in the high Mt sdb 2" << endl;
//          hHelp->Reset();
//          backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf));
//          float TTbarVBFSdb2 = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
//          cout << "In VBF region, I expect " << TTbarVBFSdb2 << " events in the high Mt sdb from TTbar" << endl;
//          hHelp->Reset();
//          data->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ2&&vbf));
//          float QCDVBFSdb2 = hHelp->Integral()*OStoSSRatioQCD;
//          cout << "In VBF region, I measure " << QCDVBFSdb2 << " events in the high Mt sdb from anti-isolated events" << endl;
// 	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
// 	    float TTbarVBFSdb2Btag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
// 	    hHelp->Reset();
// 	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>-999")));
// 	    float TTbarVBFSdb2AnyBtag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
// 	    float ratiobTagToAny2 = TTbarVBFSdb2Btag/TTbarVBFSdb2AnyBtag;
// 	    cout << "The ratio between events with >0 bTagged jets to any in TTJets MC is (2)" << ratiobTagToAny2 << endl;
// 	    hHelp->Reset();
// 	    data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0"));
// 	    float DataVBFSdb2Btag = hHelp->Integral();
// 	    cout << "In data, I measure " << DataVBFSdb2Btag << " events b-tagged" << endl;
// 	    cout << "The SF Data/MC ratio is therefore " << DataVBFSdb2Btag/TTbarVBFSdb2Btag << endl;
// 	    cout << " ==> TTbar prediction in sideband 2 goes from " << TTbarVBFSdb2AnyBtag << " to " <<  DataVBFSdb2Btag/ratiobTagToAny2 << endl;
//          cout << "Subtracting the backgrounds from the sideband..." << endl;
//          //DataVBFSdb2-= TTbarVBFSdb2;
// 	    DataVBFSdb2 -= DataVBFSdb2Btag/ratiobTagToAny2;
//          DataVBFSdb2 -= QCDVBFSdb2;
//          float normalization2 = ExtrapFactorW3Jets2>0 ? DataVBFSdb2/ExtrapFactorW3Jets2 : -99;
//          cout << "Estimation of W+jets (2) in the VBF category is " << normalization2 << endl;


	    cout << "#######  Start normalization SS from sidebands #######" << endl;

	    hHelp->Reset();
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&apZ2&&vbf);
            float DataVBFSdbSS = hHelp->Integral();
            cout << "In VBF SS region, I get " << DataVBFSdbSS << " events in the high Mt sdb" << endl;

	    hHelp->Reset();
	    backgroundDYEtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float dyelectotauVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyelectotauVBFSdbSS << " events in the high Mt sdb SS from DYetotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float dyjtotauVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSdbSS << " events in the high Mt sdb SS from DYjtotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
	    float othersVBFSdbSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSdbSS << " events in the high Mt sdb SS from others" << endl;
	    hHelp->Reset();

            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf));
            float TTbarVBFSdbSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
            cout << "In VBF SS region, I expect " << TTbarVBFSdbSS << " events in the high Mt sdb from TTbar" << endl;
            hHelp->Reset();

	    //////////////////  Here, normalize TTbar from sideband 

	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float TTbarVBFSdbSSBtag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    hHelp->Reset();
	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>-999")));
	    float TTbarVBFSdbSSAnyBtag = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    float ratiobTagToAnySS = TTbarVBFSdbSSBtag/TTbarVBFSdbSSAnyBtag;
	    cout << "The ratio between events with >0 bTagged jets to any in TTJets MC in SS is " << ratiobTagToAnySS << endl;
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0"));
	    float DataVBFSdbSSBtag = hHelp->Integral();
	    cout << "In data, I measure " << DataVBFSdbSSBtag << " SS events b-tagged" << endl;

	    hHelp->Reset();
	    backgroundWJets->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float WJetsVBFSdbSSBtag = hHelp->Integral()*Lumi/1000*hltEff_;
	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float OthersVBFSdbSSBtag = hHelp->Integral()*Lumi/1000*hltEff_;
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ2&&vbf&&TCut("nJets20BTagged>0")));
	    float QCDVBFSdbSSBtag = hHelp->Integral();
	    cout << "I subtract " << WJetsVBFSdbSSBtag << " and " << OthersVBFSdbSSBtag << " and " << QCDVBFSdbSSBtag << " events from W=jets and others" << endl;

	    cout << "The SF Data/MC ratio is therefore " << (DataVBFSdbSSBtag-WJetsVBFSdbSSBtag-OthersVBFSdbSSBtag-QCDVBFSdbSSBtag)/TTbarVBFSdbSSBtag << endl;
	    cout << " ==> TTbar prediction in sideband SS goes from " << TTbarVBFSdbSSAnyBtag << " to " <<  (DataVBFSdbSSBtag-WJetsVBFSdbSSBtag-OthersVBFSdbSSBtag-QCDVBFSdbSSBtag)/ratiobTagToAnySS << endl;

	    ///////////////////////////////////////////////////////////////////////////////////////
	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ2&&vbf));
            float QCDVBFSdbSS = hHelp->Integral();
            cout << "In VBF SS region, I measure " <<  hHelp->GetEntries()  << " events ==> X FR = " << QCDVBFSdbSS << " events in the high Mt sdb from anti-isolated events" << endl;

	    cout << "Now subtracting the expected bkg from the SS signal region" << endl;
	    hHelp->Reset();
            data->Draw(variable+">>hHelp",sbinPZetaRelSSInclusive&&pZ&&vbf);
            float DataVBFSgnSS = hHelp->Integral();

	    hHelp->Reset();
	    backgroundDYEtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float dyelectotauVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyelectotauVBFSgnSS << " events in the sgn SS from DYelectotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundDYJtoTau->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float dyjtotauVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << dyjtotauVBFSgnSS << " events in the sgn SS from DYjtotau" << endl;
	    hHelp->Reset();

	    hHelp->Reset();
	    backgroundOthers->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
	    float othersVBFSgnSS = hHelp->Integral()*Lumi/1000.*hltEff_;
	    cout << "In VBF region, I expect " << othersVBFSgnSS << " events in the sgn SS from others" << endl;
	    hHelp->Reset();

            hHelp->Reset();
            backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelSSInclusive&&pZ&&vbf));
            float TTbarVBFSgnSS = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio*(DataVBFSdbSSBtag-WJetsVBFSdbSSBtag-OthersVBFSdbSSBtag)/TTbarVBFSdbSSBtag;
            cout << "In VBF SS region, I expect " << TTbarVBFSgnSS << " events in the sgn from TTbar" << endl;
            hHelp->Reset();

	    float nonWbackgroundSS = TTbarVBFSgnSS + othersVBFSgnSS + dyjtotauVBFSgnSS + dyelectotauVBFSgnSS ;
	    float WbackgroundSS    = (DataVBFSdbSS - (DataVBFSdbSSBtag-WJetsVBFSdbSSBtag-OthersVBFSdbSSBtag-QCDVBFSdbSSBtag)/ratiobTagToAnySS /*TTbarVBFSdbSS*/-QCDVBFSdbSS-dyelectotauVBFSdbSS-dyjtotauVBFSdbSS-othersVBFSdbSS)/ExtrapFactorW3JetsSS;
            float QCDSgnSS         = DataVBFSgnSS - nonWbackgroundSS - WbackgroundSS;
            cout << "In VBF SS region, I measure " << DataVBFSgnSS << endl;
            cout << "The bkg estimation for QCD is therefore: " << DataVBFSgnSS << " - " << nonWbackgroundSS << " - " << WbackgroundSS << " = "
                 << QCDSgnSS << endl;


	    hParameters->SetBinContent(13, ExtrapFactorW3Jets);
	    hParameters->SetBinContent(14, DataVBFSdb);
	    hParameters->SetBinContent(15, TTbarVBFSdb);
	    hParameters->SetBinContent(16, QCDVBFSdb);
	    hParameters->SetBinContent(17, ExtrapFactorW3JetsSS);
            hParameters->SetBinContent(18, DataVBFSdbSS);
            hParameters->SetBinContent(19, TTbarVBFSdbSS);
            hParameters->SetBinContent(20, QCDVBFSdbSS);
            hParameters->SetBinContent(21, DataVBFSgnSS);
            hParameters->SetBinContent(22, QCDSgnSS);
	    hParameters->SetBinContent(23, ExtrapFactorW3Jets);
            hParameters->SetBinContent(24, DataVBFSdb);
            hParameters->SetBinContent(25, TTbarVBFSdb);
            hParameters->SetBinContent(26, QCDVBFSdb);
	    hParameters->SetBinContent(27, ExtrapolationFactorZFakes);

	    delete hHelp;
	  }
	  else{
	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*sbinLtiso);
	    hW3JetsLooseTauIso->Add(h1,1.0);
	  }

	  cout << "Filling histo with loose taus and FR" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*TCut(scaleFactTauW.c_str())*(sbinLtisoInclusive&&vbfLoose));
	    hW3JetsLooseTauIsoFR->Add(h1,1.0);

	    //TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
	    TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());

	    hHelp->Reset();
	    currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*TCut(scaleFactTauW.c_str())*(sbinPZetaRelLtisoInclusive&&apZ&&vbfLoose));
	    float W3JetsVBFSdb    = hHelp->Integral();
	    float W3JetsVBFSdbInt = hHelp->GetEntries();
	    hHelp->Reset();
	    currentTree->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*TCut(scaleFactTauW.c_str())*(sbinPZetaRelLtisoInclusive&&pZ&&vbfLoose));
	    float W3JetsVBFSgn    = hHelp->Integral();
	    float W3JetsVBFSgnInt = hHelp->GetEntries();
	    cout << "W3jets MC in VBF(rel): using " << W3JetsVBFSdbInt << " events from sideband and " << W3JetsVBFSgnInt << " from signal region" << endl;
	    cout << " ==> ratio sdb/sgn = " << W3JetsVBFSdb/W3JetsVBFSgn << endl;

	    hHelp->Reset();
	    data->Draw(variable+">>hHelp",sbinPZetaRelInclusive&&apZ&&vbf);
	    float DataVBFSdb = hHelp->Integral();
	    cout << "In VBF region, I get " << DataVBFSdb << " events in the high Mt sdb" << endl;
	    hHelp->Reset();
	    backgroundTTbar->Draw(variable+">>hHelp","(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*(sbinPZetaRelInclusive&&apZ&&vbf));
	    float TTbarVBFSdb = hHelp->Integral()*Lumi/1000*hltEff_*TTxsectionRatio;
	    cout << "In VBF region, I expect " << TTbarVBFSdb << " events in the high Mt sdb from TTbar" << endl;
	    hHelp->Reset();
	    currentTree->Draw(variable+">>hHelp",(TCut(scaleFactElec.c_str()))*(sbinPZetaRelSSaIsoInclusive&&apZ&&vbf));
	    float QCDVBFSdb = hHelp->Integral()*OStoSSRatioQCD;
	    cout << "In VBF region, I measure " << QCDVBFSdb << " events in the high Mt sdb from anti-isolated events" << endl;

	    cout << "Subtracting the backgrounds from the sideband..." << endl;
	    DataVBFSdb -= TTbarVBFSdb;
	    DataVBFSdb -= QCDVBFSdb;
	    float normalization = W3JetsVBFSdb/W3JetsVBFSgn>0 ? DataVBFSdb/(W3JetsVBFSdb/W3JetsVBFSgn) : -99;
	    cout << "Estimation of W+jets in the VBF category is " << normalization << endl;

	    hW3JetsLooseTauIsoFR->Scale(normalization/hW3JetsLooseTauIso->Integral());

	    delete hHelp;

	  }
	  else{
	    currentTree->Draw(variable+">>"+h1Name,"(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau)"*TCut(scaleFactTauW.c_str())*sbinLtiso);
	    hW3JetsLooseTauIsoFR->Add(h1,1.0);
	  }
	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	}
	else if((it->first).find("WJets")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  hWKeys = new TH1Keys("hWKeys","W+jets smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hWKeys", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  cout << "Keys for WJets filled with integral " << hWKeys->Integral() << " and entries " << hWKeys->GetEntries() << endl;
	}
	else if((it->first).find("DYEtoTau")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  hZmmKeys = new TH1Keys("hZmmKeys","Z+jets, e to tau smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hZmmKeys", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  cout << "Keys for Zmm filled with integral " << hZmmKeys->Integral() << " and entries " << hZmmKeys->GetEntries() << endl;
	  if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){
            TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
            currentTree->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*(sbinInclusive&&twoJets));
            hHelp->Scale(ExtrapLFakeInclusive*ExtrapolationFactorZFakes/hHelp->Integral());
            hZmmLoose->Add(hHelp,1.0);
            delete hHelp;
          }

	}
	else if((it->first).find("DYJtoTau")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  hZmjKeys = new TH1Keys("hZmjKeys","Z+jets, jet to tau smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hZmjKeys", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  cout << "Keys for Zmj filled with integral " << hZmjKeys->Integral() << " and entries " << hZmjKeys->GetEntries() << endl;
	  if( selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos ){
            TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
            currentTree->Draw(variable+">>hHelp",     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*(sbinInclusive&&twoJets));
            hHelp->Scale(ExtrapJFakeInclusive*ExtrapolationFactorZFakes/hHelp->Integral());
            hZmjLoose->Add(hHelp,1.0);
            delete hHelp;
          }
	}
	else if((it->first).find("Others")!=string::npos){
	  currentTree->Draw(variable+">>"+h1Name,     "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  hVVKeys = new TH1Keys("hVVKeys","Others smoothed", nBins , bins.GetArray());
	  currentTree->Draw(variable+">>hVVKeys", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
	  cout << "Keys for VV filled with integral " << hVVKeys->Integral() << " and entries " << hVVKeys->GetEntries() << endl;
	}

	//////////////////////////////////////////////////////
	// here, we fill histos for fake-rate
	else if((it->first).find("Data")!=string::npos){

	  h1->Reset();
	  currentTree->Draw(variable+">>"+h1Name, sbin);
	  hData->Add(h1,1.0);

	  cout << "DATA === " << hData->Integral() << endl;

	  cout << "Filling histo with anti-iso muons and loose taus" << endl;
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtisoInclusive&&vbfLoose);
	    hDataAntiIsoLooseTauIso->Add(h1,1.0);
	    h1->Reset();
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso);
	    float normalization = h1->Integral();
	    hDataAntiIsoLooseTauIso->Scale(normalization/hDataAntiIsoLooseTauIso->Integral());
	  }
	  else{
	    currentTree->Draw(variable+">>"+h1Name,sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIso->Add(h1,1.0);
	  }

	  // template for QCD : anti-iso mu && loose-tau X fake-rate(mu) X fake-rate(tau)
	  cout << "Filling histo with anti-iso muons and loose taus and FR" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElec.c_str()))*(TCut(scaleFactTauQCD.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFR->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElec.c_str()))*(TCut(scaleFactTauQCD.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral();
	    //hDataAntiIsoLooseTauIsoFR->Scale(normalization/hDataAntiIsoLooseTauIsoFR->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElec.c_str()))*(TCut(scaleFactTauQCD.c_str()))*sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIsoFR->Add(h1,1.0);
	  }

	  cout << "Filling histo with anti-iso muons and loose taus and FR (Up)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFRUp->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
 	    //hDataAntiIsoLooseTauIsoFRUp->Scale(normalization/hDataAntiIsoLooseTauIsoFRUp->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecUp.c_str()))*(TCut(scaleFactTauQCDUp.c_str()))*sbinSSaIsoLtiso);
	    hDataAntiIsoLooseTauIsoFRUp->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and loose taus and FR (Down)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*(sbinSSaIsoLtisoInclusive&&vbfLoose));
	    hDataAntiIsoLooseTauIsoFRDown->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*sbinSSaIsoLtiso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
 	    //hDataAntiIsoLooseTauIsoFRDown->Scale(normalization/hDataAntiIsoLooseTauIsoFRDown->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*(TCut(scaleFactTauQCDDown.c_str()))*sbinSSaIsoLtiso);
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
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactElec.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFR->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactElec.c_str()))*(sbinSSaIso));
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFR->Scale(normalization/hDataAntiIsoFR->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,    (TCut(scaleFactElec.c_str()))*sbinSSaIso);
	    hDataAntiIsoFR->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and tight taus and FR (Up)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactElecUp.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFRUp->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactElecUp.c_str()))*sbinSSaIso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFRUp->Scale(normalization/hDataAntiIsoFRUp->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,  (TCut(scaleFactElecUp.c_str()))*sbinSSaIso);
	    hDataAntiIsoFRUp->Add(h1,OStoSSRatioQCD);
	  }

	  cout << "Filling histo with anti-iso muons and tight taus and FR (Down)" << endl;
	  h1->Reset();
	  if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*(sbinSSaIsoInclusive&&vbfLoose));
	    hDataAntiIsoFRDown->Add(h1,1.0);
	    h1->Reset();
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*sbinSSaIso);
	    float normalization = h1->Integral()*OStoSSRatioQCD;
	    //hDataAntiIsoFRDown->Scale(normalization/hDataAntiIsoFRDown->Integral());
	  }
	  else{
	    //currentTree->Draw(variable+">>"+h1Name,(TCut(scaleFactElecDown.c_str()))*sbinSSaIso);
	    hDataAntiIsoFRDown->Add(h1,OStoSSRatioQCD);
	  }

	}

	//////////////////////////////////////////////////////

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
		selection_.find("vh")!=string::npos
		) )
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
	  currentTree->Draw(variable+">>"+h1Name, "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbin);
      }
      else{
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTElec*embeddingWeight)"*sbinPZetaRelEmbeddingInclusive);
	float embeddedNoMEtCut = h1->Integral();
	h1->Reset();
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTElec*embeddingWeight)"*sbinEmbeddingInclusive);

	embeddedMEtCutEff =  h1->Integral()/embeddedNoMEtCut;
	cout << "Efficiency of antiW cut on embedded " << embeddedMEtCutEff << endl;

	h1->Reset();
	currentTree->Draw(variable+">>"+h1Name, "(HLTTau*HLTElec*embeddingWeight)"*sbinEmbedding);
      }


      // scale by correction factors
      // scale by correction factors
      if( ! ((it->first).find("Data")!=string::npos || 
	     (it->first).find("LooseIso")!=string::npos ||
	     (it->first).find("AntiIso")!=string::npos) ) 
	h1->Scale(Lumi/1000*hltEff_);

      // scale by DATA/MC ratio in Zmumu sideband
      if((it->first).find("DY")!=string::npos)
	h1->Scale(lumiCorrFactor);

      // if W+jets, scale by extrapolation
      float sFWOS = ( selection_.find("novbf")!=string::npos   || 
		      selection_.find("boost")!=string::npos  || 
		      selection_.find("bTag")!=string::npos   || 
		      selection_.find("oneJet") !=string::npos || 
		      selection_.find("twoJets") !=string::npos || 		    
		      selection_.find("inclusive")!=string::npos) ? 
	OSWinSignalRegionDATA/OSWinSignalRegionMC : WcorrectionFactorOS;
      
      if((it->first).find("WJets")!=string::npos){

	TH1F* hHelp = new TH1F( "hHelp" ,"" , nBins , bins.GetArray());
	currentTree->Draw(variable+">>hHelp", "(sampleWeight*puWeight2*HLTweightTau*HLTweightElec*SFElec*SFTau*HqTWeight)"*sbinMtiso);
	hWLooseIso2->Add(hHelp,1.0);
	delete hHelp;

	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	  sFWOS *= VbfExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << VbfExtrapolationFactorW << " according to the Z->ee+j+vbf/Z->ee+j ratio" << endl;
	}
	else if(selection_.find("vh")!=string::npos){
	  sFWOS *= BoostExtrapolationFactorW;
	  cout << "Wjets will be rescaled by " << BoostExtrapolationFactorW << " according to the Z->ee+j+vbf/Z->ee+j ratio" << endl;
	}      
	h1->Scale( sFWOS );
	hW->Add(h1,1.0);
	if(h1->Integral()>0) hWLooseIso2->Scale(h1->Integral()/hWLooseIso2->Integral());
      }

      // if DY->tautau, and vbf scale by ratio data/MC
      if((it->first).find("DYToTauTau")!=string::npos){

	h1->Scale( ExtrapolationFactorZDataMC );

	//if(selection_.find("novbf")!=string::npos){
	//cout << "DY->tautau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//h1->Scale( NoVbfExtrapolationFactorZ );
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//cout << "DY->tautau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//h1->Scale( VbfExtrapolationFactorZ );
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//h1->Scale( BoostExtrapolationFactorZ );
	//}
      }

      // if DY->ee, mu->tau, scale by fake-rate
      if((it->first).find("DYEtoTau")!=string::npos){

	float sF = EtoTauCorrectionFactor;

	sF *= ExtrapolationFactorZDataMC;

	//if(selection_.find("novbf")!=string::npos){
	//sF *= NoVbfExtrapolationFactorZ;
	//cout << "DY->tautau, e->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//sF *= VbfExtrapolationFactorZ;
	//cout << "DY->tautau, e->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau, e->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//sF *= BoostExtrapolationFactorZ;
	//}
	h1->Scale(sF);
	hZmm->Add(h1,1.0);
	hZfakes->Add(h1,1.0);
      }

      // if DY->ee, jet->tau, scale by fake-rate
      if((it->first).find("DYJtoTau")!=string::npos){

	float sF = JtoTauCorrectionFactor;

	sF *= ExtrapolationFactorZDataMC;

	//if(selection_.find("novbf")!=string::npos){
	//sF *= NoVbfExtrapolationFactorZ;
	//cout << "DY->tautau, jet->tau will be rescaled by " << NoVbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//}
	//else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	//sF *= VbfExtrapolationFactorZ;
	//cout << "DY->tautau, jet->tau will be rescaled by " << VbfExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//}
	//else if(selection_.find("boost")!=string::npos){
	//cout << "DY->tautau, jet->tau will be rescaled by " << BoostExtrapolationFactorZ << " according to the Z->ee+vbf/Z->ee ratio" << endl;
	//sF *=BoostExtrapolationFactorZ;
	//}

	h1->Scale(sF);
	hZmj->Add(h1,1.0);
	hZfakes->Add(h1,1.0);
      }

      if((it->first).find("ggH115")!=string::npos && selection_.find("boost")!=string::npos){
	h1->Scale(kFactorSM);
      }


    }
  
    /////////////////////////////////////////////////////////////////////////////////////

    if( (it->first).find("DYEtoTau")!=string::npos ||  
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
      h1->Scale( (ExtrapolationFactorZ*ExtrapDYInclusive)/h1->Integral());
      h1->Scale(embeddedMEtCutEff/madgraphMEtCutEff);
      hParameters->SetBinContent(12,embeddedMEtCutEff/madgraphMEtCutEff);
      hDataEmb->Add(h1,1.0);
      hDataEmb->SetFillColor(kOrange-4);
    }
    if( (it->first).find("DYEtoTau")!=string::npos ) {
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
      cout << "   SS annti-isolated events = " << h1->GetEntries() << " ==> " << h1->Integral()*SSIsoToSSAIsoRatioQCD << " predicted in signal region" << endl;
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
    //hDataAntiIsoFR->SetFillColor(kMagenta-10);
    //aStack->Add(hDataAntiIsoFR);
    float QCDfromSS = hParameters->GetBinContent(22);
    QCDfromSS *= OStoSSRatioQCD; //<-------
    hDataAntiIsoLooseTauIso->Scale(QCDfromSS/hDataAntiIsoLooseTauIso->Integral());
    hDataAntiIsoLooseTauIso->SetFillColor(kMagenta-10);    
    aStack->Add(hDataAntiIsoLooseTauIso);
    hSiml->Add(hDataAntiIsoLooseTauIso,1.0);
  }
  else{
    aStack->Add(hQCD);
    hSiml->Add(hQCD,1.0);
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
  else{
    aStack->Add(hEWK);
    hSiml->Add(hEWK,1.0);
  }

  aStack->Add(hTTb);
  if(useEmbedding_)
    aStack->Add(hDataEmb);
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
  if(logy_)
    hSgn->Draw("HISTSAME");
  
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
  if(selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos ||
     selection_.find("oneJet")!=string::npos || selection_.find("twoJets")!=string::npos){
    uncertZtt += (0.06  * 0.06) ; // Tau-ID 
    uncertZtt += (0.035 * 0.035); // Lumi 
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    uncertZtt += (0.06  * 0.06) ; // Tau-ID 
    uncertZtt += (0.035 * 0.035); // Lumi 
    uncertZtt += (0.10  * 0.10);  // Extrap. factor 
  }
  else if(selection_.find("boost")!=string::npos){
    uncertZtt += (0.06  * 0.06) ; // Tau-ID 
    uncertZtt += (0.035 * 0.035); // Lumi 
    uncertZtt += (0.10  * 0.10);  // Extrap. factor 
  }
  uncertZtt = TMath::Sqrt(uncertZtt);
  float uncertTTb = 0;
  if(selection_.find("novbf")!=string::npos || selection_.find("inclusive")!=string::npos ||
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
    if(fabs(pull) > maxPull)
      maxPull = fabs(pull);
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

  c1->SaveAs(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/plots/%s/plot_eTau_mH%d_%s_%s_%s.png",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable.Data()));
  c1->SaveAs(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/plots/%s/plot_eTau_mH%d_%s_%s_%s.pdf",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable.Data()));

  // templates for fitting
  TFile* fout = new TFile(Form("/home/llr/cms/lbianchini/CMSSW_4_2_8_patch7_reload/src/LLRAnalysis/Limits/htautau/histograms/%s/eTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable.Data()),"RECREATE");
  fout->cd();
  hSiml->Write();
  hQCD->Write();
  hZmm->Write();
  hZmmLoose->Write();
  hZmj->Write();
  hZmjLoose->Write();
  hZfakes->Write();
  hTTb->Write();
  hZtt->Write();
  hDataEmb->Write();
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


  delete hQCD; delete hZmm; delete hZmj; delete hZfakes; delete hTTb; delete hZtt; delete hW; delete hW3Jets; delete hLooseIso; delete hAntiIso;
  delete hZmmLoose; delete hZmjLoose;
  delete hW3JetsLooseTauIso; delete hW3JetsLooseTauIsoFR;
  delete hWLooseIso2; delete hLooseIso2; delete hLooseIso3;
  delete hDataAntiIsoLooseTauIso; delete hDataAntiIsoLooseTauIsoFR; delete hDataAntiIsoLooseTauIsoFRUp; delete hDataAntiIsoLooseTauIsoFRDown;
  delete hDataAntiIso; delete hDataAntiIsoFR; delete hDataAntiIsoFRUp; delete hDataAntiIsoFRDown;

  
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
  delete hWMt; delete aStack;  delete hEWK; delete hSiml; delete hDataEmb; delete hSgn; delete hRatio; delete line;
  delete fout;


  fSignalGGH->Close();       delete fSignalGGH; 
  fSignalVBF->Close();       delete fSignalVBF;
  fSignalVH->Close();        delete fSignalVH; 
  fSignalggH110->Close();    delete fSignalggH110;
  fSignalggH115->Close();    delete fSignalggH115;
  fSignalggH120->Close();    delete fSignalggH120;
  fSignalggH125->Close();    delete fSignalggH125;
  fSignalggH130->Close();    delete fSignalggH130;
  fSignalggH135->Close();    delete fSignalggH135;
  fSignalggH140->Close();    delete fSignalggH140;
  fSignalggH145->Close();    delete fSignalggH145;
  fSignalqqH110->Close();    delete fSignalqqH110;
  fSignalqqH115->Close();    delete fSignalqqH115;
  fSignalqqH120->Close();    delete fSignalqqH120;
  fSignalqqH125->Close();    delete fSignalqqH125;
  fSignalqqH130->Close();    delete fSignalqqH130;
  fSignalqqH135->Close();    delete fSignalqqH135;
  fSignalqqH140->Close();    delete fSignalqqH140;
  fSignalqqH145->Close();    delete fSignalqqH145;
  fSignalVH110->Close();     delete fSignalVH110;
  fSignalVH115->Close();     delete fSignalVH115;
  fSignalVH120->Close();     delete fSignalVH120;
  fSignalVH125->Close();     delete fSignalVH125;
  fSignalVH130->Close();     delete fSignalVH130;
  fSignalVH135->Close();     delete fSignalVH135;
  fSignalVH140->Close();     delete fSignalVH140;
  fSignalVH145->Close();     delete fSignalVH145;

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



void plotElecTauAll( Int_t useEmbedded = 1, TString outputDir = "July2012/Thesis"){

  vector<string> variables;
  vector<int> mH;

  //variables.push_back("diTauVisMass");
  //variables.push_back("diTauSVFitMass");
  variables.push_back("diTauNSVfitMass");

  //mH.push_back(105);
  //mH.push_back(110);
  //mH.push_back(115);
  mH.push_back(120);
  //mH.push_back(125);
  //mH.push_back(130);
  //mH.push_back(135);
  //mH.push_back(140);
  //mH.push_back(145);
  //mH.push_back(160);

  //plotElecTau(120,1,"inclusive","" ,"MtLeg1MVA","M_{T}","GeV" ,             outputDir,40,0,160,5.0,1.0,0,1.2);
  //plotElecTau(120,1,"inclusive",""   ,"diTauVisMass","visible mass","GeV" ,  outputDir,50,0,200,5.0,1.0,0,1.2);
  //plotElecTau(120,1,"inclusive",""   ,"etaL1","e #eta", "units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  //return;

  //plotElecTau(120,1,"inclusive",""   ,"MEtCorr","MET","GeV"                        ,outputDir,40,0,100,5.0,1.0,0,1.2);
  //plotElecTau(120,1,"inclusive",""   ,"MEtCorrPhi","MVA #phi","units"              ,outputDir,32,-3.2,3.2,   5.0,1.0,0,1.5);
  //plotElecTau(120,1,"inclusive",""   ,"MtLeg1Corr","M_{T}","GeV" ,                  outputDir,40,0,160,5.0,1.0,0,1.2);


  /*
  plotElecTau(120,1,"inclusive",""   ,"hpsMVA",        "#tau MVA","units"              ,outputDir,50,0.75,1.0, 5.0,1.0,0,1.8);
  plotElecTau(120,1,"inclusive",""   ,"decayMode",     "#tau_{h} decay mode","units"   ,outputDir,3,0,3, 5.0,1.0,0,1.4);
  plotElecTau(120,1,"inclusive",""   ,"visibleTauMass","visible #tau_{h} mass","GeV"   ,outputDir,40,0,2,5.0,1.0,0,1.2);  

  plotElecTau(120,1,"inclusive",""   ,"diTauVisMass","visible mass","GeV"      ,outputDir,50,0,200,5.0,1.0,0,1.2);  
  plotElecTau(120,1,"inclusive",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,60,0,300,5.0,1.0,0,1.2);
  plotElecTau(120,1,"inclusive",""   ,"etaL1","e #eta", "units"              ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  plotElecTau(120,1,"inclusive",""   ,"ptL1","e p_{T}", "GeV"                ,outputDir,27,11, 92,5.0,1.0,0,1.2);

  plotElecTau(120,1,"inclusive",""   ,"ptL2","#tau p_{T}","GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  plotElecTau(120,1,"inclusive",""   ,"ptL1","e p_{T}", "GeV"           ,outputDir,27,11, 92,5.0,1.0,0,1.2);
  plotElecTau(120,0,"inclusive",""   ,"etaL1","e #eta", "units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);
  plotElecTau(120,0,"inclusive",""   ,"etaL2","#tau #eta","units"         ,outputDir,25,-2.5, 2.5,5.0,1.0,0,2.);

  plotElecTau(120,0,"inclusive",""   ,"numPV","reconstructed vertexes","units"             ,outputDir,30,0,30,5.0,1.0,0,1.5);
  plotElecTau(120,1,"inclusive",""   ,"nJets30","jet multiplicity","units"                 ,outputDir,10,0, 10,5.0,1.0,1,10);
  plotElecTau(120,1,"inclusive",""   ,"nJets20BTagged","b-tagged jet multiplicity","units" ,outputDir,5,0, 5,5.0,1.0,1,10);

  //plotElecTau(120,1,"bTag",""        ,"ptB1", "leading b-tagged jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,100);
  //plotElecTau(120,1,"bTag",""        ,"etaB1","leading b-tagged jet #eta","units"      ,outputDir,21,-5, 5,5.0,1.0,0,2.);

  plotElecTau(120,1,"oneJet",""      ,"eta1","leading jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  plotElecTau(120,1,"oneJet",""     , "pt1","leading jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,200);
  */

  //plotElecTau(120,1,"twoJets",""     ,"pt1","leading jet p_{T}","GeV"       ,outputDir,50,30, 330,5.0,1.0,1,200);
  //plotElecTau(120,1,"twoJets",""     ,"pt2","trailing jet p_{T}","GeV"      ,outputDir,50,30, 330,5.0,1.0,1,100);
  //plotElecTau(120,1,"twoJets",""     ,"eta1","leading jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotElecTau(120,1,"twoJets",""     ,"eta2","trailing jet #eta","units"    ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotElecTau(120,1,"twoJets",""     ,"Deta","|#Delta#eta|_{jj}","units"    ,outputDir,20,0, 8,   5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"Mjj","M_{jj}","GeV"                  ,outputDir,20,0, 1000,5.0,1.0,1,100);
  //plotElecTau(120,1,"twoJets",""     ,"MVAvbf","BDT output","units"         ,outputDir,10,-1, 1,5.0,1.0,1,100);
  //plotElecTau(120,1,"twoJets",""     ,"ptVeto","veto jet p_{T}","GeV"       ,outputDir,20,30, 230,5.0,1.0,1,200);
  //plotElecTau(120,1,"twoJets",""     ,"etaVeto","veto jet #eta","units"     ,outputDir,21,-5, 5,5.0,1.0,0,2.);
  //plotElecTau(120,1,"twoJets",""     ,"diJetPt","di-jet p_{T}","GeV"           ,outputDir,30,0, 300,5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"dPhiHjet","(di-jet - H) #phi","units",outputDir,32,0, 3.2,5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"c2","di-tau vis p_{T}","GeV"         ,outputDir,20,0, 100,5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"c1","min#Delta#eta j-H","units"     ,outputDir,20,0,10,5.0,1.0,0,1.5);

  //plotElecTau(120,1,"twoJets",""     ,"c2","di-tau vis p_{T}","GeV"         ,outputDir,20,0, 100,5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"diTauRecoPt","(di-tau+MET) p_{T}","GeV"         ,outputDir,40,0, 400,5.0,1.0,0,1.5);
  //plotElecTau(120,1,"twoJets",""     ,"Dphi","#Delta#phi_{jj}","units"        ,outputDir,32,0, 3.2,5.0,1.0,0,1.5);

  //return;

 
  for(unsigned int i = 0 ; i < variables.size(); i++){
    for(unsigned j = 0; j < mH.size(); j++){

      //plotElecTau(mH[j],useEmbedded,"inclusive",""   ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
      //plotElecTau(mH[j],useEmbedded,"novbf",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);


        plotElecTau(mH[j],useEmbedded,"vbf",""         ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotElecTau(mH[j],useEmbedded,"vbf","TauUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotElecTau(mH[j],useEmbedded,"vbf","TauDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotElecTau(mH[j],useEmbedded,"vbf","JetUp"    ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
        plotElecTau(mH[j],useEmbedded,"vbf","JetDown"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

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

//       plotElecTau(mH[j],useEmbedded,"bTagLow",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagLow","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagLow","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagLow","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagLow","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);

//       plotElecTau(mH[j],useEmbedded,"bTagHigh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagHigh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagHigh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagHigh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"bTagHigh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);


//       plotElecTau(mH[j],useEmbedded,"vh",""       ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"vh","TauUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"vh","TauDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"vh","JetUp"  ,variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);
//       plotElecTau(mH[j],useEmbedded,"vh","JetDown",variables[i],"mass","GeV",outputDir,-1,0,100,1.0,1.0,0,1.2);





    }
  }
  


}




int main(int argc, const char* argv[])
{

  std::cout << "plotElecTau()" << std::endl;
  gROOT->SetBatch(true);
 

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  //plotElecTau();
  plotElecTauAll();
  //plotElecTau(120,1,"inclusive","","diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotElecTau(120,1,"novbf","",    "diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotElecTau(120,1,"boost","",    "diTauVisMass","","","Dec2011/iter3",10,0,7000);
  //plotElecTau(120,1,"vbf","",      "diTauVisMass","","","Dec2011/iter3",10,0,7000);

}
