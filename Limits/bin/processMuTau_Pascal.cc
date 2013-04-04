#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <cstdlib>
#include <iostream> 
#include <fstream>
#include <sstream>
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

#define USESSBKG         false
#define scaleByBinWidth  false
#define DOSPLIT          false
#define studyQCDshape    false
#define useZDataMC       false

TArrayF createBins(int nBins_ = 80 ,
		   float xMin_ = 0.,
		   float xMax_ = 400.,
		   int& nBins = *(new int()),
		   string selection_   = "inclusive",
		   TString variable_   = "diTauVisMass",
		   //TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_Sep12/src/Bianchi/Limits/bin/results/bins/"
		   TString location    = "/data_CMS/cms/htautau/HCP12/binning/"
		   ){

  // input txt file with bins
  ifstream is;

  TArrayF dummy(2);
  dummy[0] = xMin_; dummy[1] = xMax_;
  
  char* c = new char[10];
  is.open(Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
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
  is.open(Form(location+"/bins_muTau_%s_%s.txt",variable_.Data(), selection_.c_str())); 
  
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

void drawHistogramMC(TString RUN = "ABCD",
		     TTree* tree = 0, 
		     TString variable = "", 
		     float& normalization      = *(new float()), 
		     float& normalizationError = *(new float()), 
		     float scaleFactor = 0., 
		     TH1F* h = 0, 
		     TCut cut = TCut(""),
		     int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();

    if(RUN=="ABC")
      tree->Draw(variable+">>"+TString(h->GetName()),"(sampleWeight*puWeightHCP*HLTweightTauABC*HLTweightMuABC*SFTau*SFMu_ABC*HqTWeight*weightHepNup)"*cut);

    else if(RUN=="D")
      tree->Draw(variable+">>"+TString(h->GetName()),"(sampleWeight*puWeightD*HLTweightTauD*HLTweightMuD*SFTau*SFMu_D*HqTWeight*weightHepNup)"*cut);

    else
      tree->Draw(variable+">>"+TString(h->GetName()),"(sampleWeight*puWeight*HLTweightTau*HLTweightMu*SFTau*SFMu*HqTWeight*weightHepNup)"*cut);

    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
    if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramMC has raised an error" << endl;
    return;
  }
}

void drawHistogramData(TTree* tree = 0, 
		       TString variable = "", 
		       float& normalization      = *(new float()), 
		       float& normalizationError = *(new float()), 
		       float scaleFactor = 0., 
		       TH1F* h = 0, 
		       TCut cut = TCut(""),
		       int verbose = 0 ){
  if(tree!=0 && h!=0){
    h->Reset();
    tree->Draw(variable+">>"+TString(h->GetName()),cut);
    h->Scale(scaleFactor);
    normalization      = h->Integral();
    normalizationError = TMath::Sqrt(h->GetEntries())*(normalization/h->GetEntries());
    if(verbose==0) h->Reset();
    if(verbose){
      //cout << "Tree " << tree->GetTitle() << ":" << endl;
      //cout << "Cut " << cut.GetTitle() << endl;
      //cout << "====> N = " << normalization << " +/- " << normalizationError;
    }
  }
  else{
    cout << "Function drawHistogramData has raised an error" << endl;
    return;
  }
}

void plotMuTau( Int_t mH_           = 120,
		Int_t useEmbedding_ = 0,
		string selection_   = "inclusive",
		string analysis_    = "",		  
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
		TString version_    = "Moriond",
		//TString location    = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_3_4_p2_topup/src/Bianchi/Limits/bin/results/"
		//TString location    = "/home/llr/cms/ivo/HTauTauAnalysis/CMSSW_5_3_4_p2_Trees/src/LLRAnalysis/Limits/bin/results/"
		TString location    = "/home/llr/cms/ndaci/WorkArea/HTauTau/Analysis/CMSSW_534p2_Spring13_Trees/src/LLRAnalysis/Limits/bin/results/"
		) 
{   
  
  cout << endl;
  cout << "@@@@@@@@@@@@@@@@@@ Category  = " << selection_     <<  endl;
  cout << "@@@@@@@@@@@@@@@@@@ Variable  = " << string(variable_.Data()) <<  endl;
  cout << endl;
  
  
  //////////////////////////////////
  // BINNING, LUMINOSITY, FACTORS //
  //////////////////////////////////
  
  int nBins = nBins_;
  TArrayF bins = createBins(nBins_, xMin_, xMax_, nBins, selection_, variable_);
  
  // LUMINOSITY //
  float Lumi = 0.; 
  
  if(RUN=="ABC")    Lumi = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 ;       // 2012ABC 
  else if(RUN=="D") Lumi = 7274;                                               // 2012D 
  else              Lumi = 791.872 + 4434.0 + 495.003 + 6174 + 206.196 + 7274; // 2012ABCD
  /////////////////

  float lumiCorrFactor                     = 1 ;    //= (1-0.056);
  float TTxsectionRatio                    = 0.98; //lumiCorrFactor*(165.8/157.5) ;
  float OStoSSRatioQCD                     = 1.06;
  float SSIsoToSSAIsoRatioQCD              = 1.0;
  float MutoTauCorrectionFactor            = 0.976;
  float JtoTauCorrectionFactor             = 0.976;
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


  ////////////////////
  // DEFINE TCANVAS //
  ////////////////////

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

  const int nSignal=2;
  TString nameSignal[nSignal]={"VBFH125","GGH125"};
  
  TH1F* hSignal[2];
  for(int iS=0 ; iS<nSignal ; iS++) {
    hSignal[iS] = new TH1F("h_"+nameSignal[iS], nameSignal[iS] , nBins , bins.GetArray());
    hSignal[iS]->SetLineWidth(2);
  }
  

  /////////////////////
  // GET THE NTUPLES //
  /////////////////////
  TString Tanalysis_(analysis_);
  TString fileAnalysis = Tanalysis_;
  if(Tanalysis_=="") fileAnalysis = "nominal";

  TString pathToFile = "/data_CMS/cms/htautau/PostMoriond/NTUPLES/MuTau/temp/";

  TString treeMC;
  if(Tanalysis_.Contains("TauUp") || Tanalysis_.Contains("TauDown") || Tanalysis_.Contains("JetUp") || Tanalysis_.Contains("JetDown") )
    treeMC = "outTreePtOrd"+Tanalysis_;
  else treeMC = "outTreePtOrd";

  TChain *signal[2];
  for(int iS=0 ; iS<nSignal ; iS++) {
    signal[iS] = new TChain(treeMC);
    signal[iS]->Add(pathToFile+"/nTuple"+nameSignal[iS]+"_MuTau_"+fileAnalysis+".root");    
  }


  //////////////////////////
  // DEFINE ANALYSIS CUTS //
  //////////////////////////

  bool useMt      = true;
  string antiWcut = useMt ? "MtLeg1MVA" : "-(pZetaMVA-1.5*pZetaVisMVA)" ; 
  float antiWsgn  = useMt ? 20. :  20. ;
  float antiWsdb  = useMt ? 70. :  40. ; 

  bool use2Dcut   = false;
  if( use2Dcut ){
    antiWcut = "!(MtLeg1MVA<40 && (pZetaMVA-1.5*pZetaVisMVA)>-20)";
    antiWsgn = 0.5;
    antiWsdb = 0.5;
  }

  ///// LEPT PT ///////
  TCut lpt("ptL1>20 && isPFMuon && isTightMuon");
  TCut tpt("ptL2>20");

  ////// TAU ISO //////
  TCut tiso("tightestHPSMVAWP>=0  && tightestAntiMuWP>2");
  TCut ltiso("tightestHPSMVAWP>-99 && tightestAntiMuWP>2");
  TCut mtiso("hpsMVA>0.7 && tightestAntiMuWP>2");
  TCut pairIndex("pairIndexMoriond<1");

  if(version_=="Moriond") {
    tiso  = "tightestHPSMVAWP>=0  && tightestAntiMuWP>2" ;
    ltiso = "tightestHPSMVAWP>-99 && tightestAntiMuWP>2";
    mtiso = "hpsMVA>0.7 && tightestAntiMuWP>2";
    pairIndex = "pairIndexMoriond<1";
  }
  else if(version_=="MoriondHPSDB3H") {
    tiso  = "tightestHPSDB3HWP>=0  && tightestAntiMuWP>2" ;
    ltiso = "tightestHPSDB3HWP>-99 && tightestAntiMuWP>2";
    mtiso = "hpsDB3H>0.7 && tightestAntiMuWP>2";
    pairIndex = "pairIndexHPSDB3H<1"; 
    //pairIndex = "pairIndexMoriond<1"; // old ntuples --> to be changed with next iteration
  }
  else if(version_=="TauIso2") {
    tiso  = "tightestHPSMVA2WP>=0 && tightestAntiMuWP>2" ;
    ltiso = "tightestHPSMVA2WP>-99 && tightestAntiMuWP>2";
    mtiso = "hpsMVA2>0.7 && tightestAntiMuWP>2";
    pairIndex = "pairIndexHPSMVA2<1";
  }
  else if(version_=="AntiMu2") {
    tiso  = "tightestHPSMVAWP>=0 && tightestAntiMu2WP>2" ;
    ltiso = "tightestHPSMVAWP>-99 && tightestAntiMu2WP>2";
    mtiso = "hpsMVA>0.7 && tightestAntiMu2WP>2";
    pairIndex = "pairIndexAntiMu2<1";
  }
  else if(version_=="AntiMu2TauIso2") {
    tiso  = "tightestHPSMVA2WP>=0 && tightestAntiMu2WP>2" ;
    ltiso = "tightestHPSMVA2WP>-99 && tightestAntiMu2WP>2";
    mtiso = "hpsMVA2>0.7 && tightestAntiMu2WP>2";
    pairIndex = "pairIndexAntiMu2HPSMVA2<1";
    //pairIndex = "pairIndexSpring13<1"; // old ntuples --> to be changed with next iteration
  }
  else if(version_=="AntiMu2HPSDB3H") {
    tiso  = "tightestHPSDB3HWP>=0 && tightestAntiMu2WP>2" ;
    ltiso = "tightestHPSDB3HWP>-99 && tightestAntiMu2WP>2";
    mtiso = "hpsDB3H2>0.7 && tightestAntiMu2WP>2";
    pairIndex = "pairIndexAntiMu2HPSDB3H<1";
    //pairIndex = "pairIndexSpring13<1"; // old ntuples --> to be changed with next iteration
  }
  else if(version_=="AntiMu2TauIso2Soft") {
    tiso  = "tightestHPSMVA2WP>=0 && tightestAntiMu2WP>2" ;
    ltiso = "tightestHPSMVA2WP>-99 && tightestAntiMu2WP>2";
    mtiso = "hpsMVA2>0.7 && tightestAntiMu2WP>2";
    pairIndex = "pairIndexSoft<1";
  }
  if(VERBOSE) cout << "USING PAIR INDEX : " << pairIndex << endl;

  ////// MU ISO ///////
  TCut liso("combRelIsoLeg1DBetav2<0.10");
  TCut laiso("combRelIsoLeg1DBetav2>0.20 && combRelIsoLeg1DBetav2<0.50");
  TCut lliso("combRelIsoLeg1DBetav2<0.30");

 
  ////// EVENT WISE //////
  TCut lveto("muFlag!=1 && vetoEvent==0"); //muFlag==0
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut pZ( Form("((%s)<%f)",antiWcut.c_str(),antiWsgn));
  TCut apZ(Form("((%s)>%f)",antiWcut.c_str(),antiWsdb));
  //TCut apZ2(Form("((%s)>%f && (%s)<120)",antiWcut.c_str(),antiWsdb,antiWcut.c_str()));
  TCut apZ2(Form("((%s)>60 && (%s)<120)",antiWcut.c_str(),antiWcut.c_str()));
  TCut hltevent("HLTx==1 && ( run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");


  ////// CATEGORIES ///
  TCut zeroJet("nJets30<1");
  TCut oneJet("nJets30>=1");
  TCut twoJets("nJets30>=2");
  TCut vbf("nJets30>=2 && pt1>30 && pt2>30 && Mjj>500 && Deta>3.5 && isVetoInJets!=1");
  TCut vbfLoose("nJets30>=2 && pt1>30 && pt2>30 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vbfLooseQCD("nJets20>=2 && isVetoInJets!=1 && Mjj>200 && Deta>2");
  TCut vh("pt1>30 && pt2>30 && Mjj>70 && Mjj<120 && diJetPt>150 && MVAvbf<0.80 && nJets20BTagged<1");
  TCut boost("nJets30>0 && pt1>30 && nJets20BTagged<1");
  boost = boost && !vbf /*&& !vh*/;
  TCut bTag("nJets30<2 && nJets20BTagged>0");
  TCut bTagLoose("nJets30<2 && nJets20BTaggedLoose>0"); //for W shape in b-Category
  //TCut bTagLoose("nJets30<2 && nJets20>=1");
  TCut nobTag("nJets30<2 && nJets20BTagged==0");
  TCut novbf("nJets30<1 && nJets20BTagged==0");

  if(selection_.find("High")!=string::npos) {
    boost = boost&&TCut("ptL2>40"); 
    novbf = novbf&&TCut("ptL2>40");
  }
  else if(selection_.find("Low")!=string::npos){
    boost = boost&&TCut("ptL2<40"); 
    novbf = novbf&&TCut("ptL2<40"); 
  }

  bool removeMtCut     = bool(selection_.find("NoMt")!=string::npos);
  bool invertDiTauSign = bool(selection_.find("SS")!=string::npos);

  TCut MtCut       = removeMtCut     ? "(etaL1<999)" : pZ;
  TCut diTauCharge = invertDiTauSign ? SS : OS; 

  TCut sbin; TCut sbinEmbedding; TCut sbinEmbeddingPZetaRel; TCut sbinPZetaRel; TCut sbinSS; 
  TCut sbinPZetaRelSS; TCut sbinSSaIso; TCut sbinAiso;
  TCut sbinSSlIso1; TCut sbinSSlIso2; TCut sbinSSlIso3;
  TCut sbinPZetaRelaIso; TCut sbinPZetaRelSSaIso;  TCut sbinPZetaRelSSaIsoMtiso; 
  TCut sbinSSaIsoLtiso; TCut sbinSSaIsoMtiso;
  TCut sbinSSltiso; TCut sbinSSmtiso; TCut sbinLtiso; TCut sbinMtiso; TCut sbinPZetaRelMtiso;

  TCut sbinInclusive;
  sbinInclusive                     = lpt && tpt && tiso && liso && lveto && diTauCharge && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinEmbeddingInclusive;
  sbinEmbeddingInclusive            = lpt && tpt && tiso && liso && lveto && diTauCharge && MtCut              && pairIndex           ;
  TCut sbinPZetaRelEmbeddingInclusive;
  sbinPZetaRelEmbeddingInclusive    = lpt && tpt && tiso && liso && lveto && diTauCharge                       && pairIndex           ;
  TCut sbinPZetaRelSSInclusive;
  sbinPZetaRelSSInclusive           = lpt && tpt && tiso && liso && lveto && SS                    && hltevent && pairIndex && hltmatch;
  TCut sbinPZetaRelInclusive;
  sbinPZetaRelInclusive             = lpt && tpt && tiso && liso && lveto && diTauCharge           && hltevent && pairIndex && hltmatch;
  TCut sbinSSInclusive;
  sbinSSInclusive                   = lpt && tpt && tiso && liso && lveto && SS          && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinSSaIsoInclusive;
  sbinSSaIsoInclusive               = lpt && tpt && tiso && laiso&& lveto && SS          && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinAisoInclusive;
  sbinAisoInclusive                 = lpt && tpt && tiso && laiso&& lveto && diTauCharge && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinPZetaRelSSaIsoInclusive;
  sbinPZetaRelSSaIsoInclusive       = lpt && tpt && tiso && laiso&& lveto && SS                    && hltevent && pairIndex && hltmatch;
  TCut sbinPZetaRelSSaIsoMtisoInclusive;
  sbinPZetaRelSSaIsoMtisoInclusive  = lpt && tpt && mtiso&& laiso&& lveto && SS                    && hltevent && pairIndex && hltmatch;

  TCut sbinSSaIsoLtisoInclusive;
  sbinSSaIsoLtisoInclusive          = lpt && tpt && mtiso&& laiso&& lveto && SS && MtCut           && hltevent && pairIndex && hltmatch;
  TCut sbinSSaIsoMtisoInclusive;
  sbinSSaIsoMtisoInclusive          = lpt && tpt && mtiso&& laiso&& lveto && SS && MtCut           && hltevent && pairIndex && hltmatch;
  TCut sbinPZetaRelaIsoInclusive;
  sbinPZetaRelaIsoInclusive         = lpt && tpt && tiso && laiso&& lveto && diTauCharge           && hltevent && pairIndex && hltmatch;

  TCut sbinSSltisoInclusive;
  sbinSSltisoInclusive              = lpt && tpt && ltiso&& liso && lveto && SS && MtCut           && hltevent && pairIndex && hltmatch;
  TCut sbinLtisoInclusive;
  sbinLtisoInclusive                = lpt && tpt && ltiso&& liso && lveto && diTauCharge && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinMtisoInclusive;
  sbinMtisoInclusive                = lpt && tpt && mtiso&& liso && lveto && diTauCharge && MtCut  && hltevent && pairIndex && hltmatch;
  TCut sbinPZetaRelLtisoInclusive;
  sbinPZetaRelLtisoInclusive        = lpt && tpt && ltiso&& liso && lveto && diTauCharge           && hltevent && pairIndex && hltmatch;


  TCut sbinTmp("");
  if(selection_.find("inclusive")!=string::npos) 
    sbinTmp = "etaL1<999";
  else if(selection_.find("oneJet")!=string::npos)
    sbinTmp = oneJet;
  else if(selection_.find("twoJets")!=string::npos)
    sbinTmp = twoJets;
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos)
    sbinTmp = vbf;
  else if(selection_.find("vh")!=string::npos)
    sbinTmp = vh;
  else if(selection_.find("novbf")!=string::npos)
    sbinTmp = novbf;
  else if(selection_.find("boost")!=string::npos)
    sbinTmp = boost;
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos)
    sbinTmp = bTag;
  else if(selection_.find("nobTag")!=string::npos)
    sbinTmp = nobTag;

  sbin                   =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge  && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinEmbedding          =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge  && MtCut              && pairIndex            ;
  sbinEmbeddingPZetaRel  =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge                        && pairIndex            ;
  sbinPZetaRel           =  sbinTmp && lpt && tpt && tiso && liso && lveto && diTauCharge            && hltevent && pairIndex && hltmatch ;
  sbinPZetaRelaIso       =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && diTauCharge            && hltevent && pairIndex && hltmatch ;
  sbinPZetaRelSSaIso     =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && SS                     && hltevent && pairIndex && hltmatch ;
  sbinSS                 =  sbinTmp && lpt && tpt && tiso && liso && lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinPZetaRelSS         =  sbinTmp && lpt && tpt && tiso && liso && lveto && SS                     && hltevent && pairIndex && hltmatch ;
  sbinAiso               =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && diTauCharge  && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSaIso             =  sbinTmp && lpt && tpt && tiso && laiso&& lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSlIso1            =  sbinTmp && lpt && tpt && tiso && lliso&& lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSlIso2            =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSlIso3            =  sbinTmp && lpt && tpt && mtiso&& lliso&& lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSaIsoLtiso        =  sbinTmp && lpt && tpt && ltiso&& laiso&& lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSaIsoMtiso        =  sbinTmp && lpt && tpt && mtiso&& laiso&& lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSltiso            =  sbinTmp && lpt && tpt && ltiso&& liso && lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinSSmtiso            =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && SS           && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinLtiso              =  sbinTmp && lpt && tpt && ltiso&& liso && lveto && diTauCharge  && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinMtiso              =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && diTauCharge  && MtCut  && hltevent && pairIndex && hltmatch ;
  sbinPZetaRelMtiso      =  sbinTmp && lpt && tpt && mtiso&& liso && lveto && diTauCharge            && hltevent && pairIndex && hltmatch ;
  sbinPZetaRelSSaIsoMtiso=  sbinTmp && lpt && tpt && mtiso&& laiso&& lveto && SS                     && hltevent && pairIndex && hltmatch ;


  /////////////////////
  // ANALYZE NTUPLES //
  /////////////////////

  float NormSign = 0.;
  float Error = 0.;

  for(int iS=0 ; iS<nSignal ; iS++) {
    
    NormSign = 0.;
    Error = 0.;

    drawHistogramMC(RUN,signal[iS], variable_, NormSign, Error,    Lumi*hltEff_/1000., hSignal[iS], sbin, 1);
    hSignal[iS]->Scale(magnifySgn_);

  }


  //////////////////////////////
  // PLOT AND SAVE HISTOGRAMS //
  //////////////////////////////

  // templates for fitting
  TFile* fout = new TFile(Form(location+"/%s/histograms/muTau_mH%d_%s_%s_%s.root",outputDir.Data(), mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");
  fout->cd();
  for(int iS=0 ; iS<nSignal ; iS++) hSignal[iS]->Write();
  
  fout->Write();
  fout->Close();

  for(int iS=0 ; iS<nSignal ; iS++) {
    delete hSignal[iS];
    //signal[iS]->Close();
    delete signal[iS];
  }

}


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

  plotMuTau(125,1,"boostHigh",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);
  plotMuTau(125,1,"vbf",""   ,"diTauNSVfitMass","SVfit mass","GeV"     ,outputDir,-1,0,100,5.0,1.0,0,1.2);

}


int main(int argc, const char* argv[])
{

  std::cout << "plotMuTau()" << std::endl;
  gROOT->SetBatch(true);

  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  plotMuTauAll();

  return 0;
}
