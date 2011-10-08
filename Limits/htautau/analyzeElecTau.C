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
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "THStack.h"
#include "TCut.h"

#define VERBOSE true
#define SAVE true




///////////////////////////////////////////////////////////////////////////////////////////////

void plotElecTau( Int_t mH_ = 120,
		  string selection_ = "inclusive",
		  string analysis_  = "",		  
		  TString variable_ = "diTauVisMass",
		  TString XTitle_ = "full mass",
		  TString Unities_ = "GeV",
		  Int_t nBins_ = 100, Float_t xMin_=0, Float_t xMax_=400,
		  Float_t magnifySgn_ = 1.0,
		  Float_t hltEff_ = 1.004*0.986, // elecID*elecIso,
		  Int_t logy_ = 0 ) 
{   

  // input txt file with bins
  ifstream is;

  char* c = new char[10];
  is.open(Form("bins/bins_eTau_mH%d_%s_%s.txt",mH_,selection_.c_str(),variable_.Data())); 
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
  Float_t bins[nBins+1];
  cout << "Making histograms with " << nBins << " bins:" << endl;

  is.close();
  is.open(Form("bins/bins_eTau_mH%d_%s_%s.txt",mH_,selection_.c_str(),variable_.Data())); 
  
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

  // Luminosity analyzed  
  float Lumi                     = (-47.4 + 204.6 + 887 + 361.7 + 531.5)*1.00;
  float WcorrectionFactorOS      = 1.04;  
  float WcorrectionFactorSS      = 0.88;  
  float EtoTauCorrectionFactor   = 0.80;
  float JtoTauCorrectionFactor   = 0.80;
  float VbfExtrapolationFactor   = 1.03;
  
  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TLegend* leg = new TLegend(0.55,0.42,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.03);
  leg->SetHeader(Form("#splitline{CMS Preliminary}{#splitline{%.1f fb^{-1} #sqrt{s}=7 TeV}{#tau_{e}#tau_{had}}}", Lumi/1000. ));

  THStack* aStack = new THStack("aStack","");

  TH1F* hSiml  = new TH1F( "hSiml" ,"all"               , nBins , bins);
  TH1F* hSgn   = new TH1F( "hSgn " ,"vbf+ggf"           , nBins , bins);
  TH1F* hSgn1  = new TH1F( "hSgn1" ,"vbf"               , nBins , bins);
  TH1F* hSgn2  = new TH1F( "hSgn2" ,"ggf"               , nBins , bins);
  TH1F* hData  = new TH1F( "hData" ,"Observed"          , nBins , bins);
  TH1F* hW     = new TH1F( "hW"    ,"W+jets"            , nBins , bins);
  TH1F* hEWK   = new TH1F( "hEWK"  ,"EWK"               , nBins , bins);
  TH1F* hZtt   = new TH1F( "hZtt"  ,"Ztautau"           , nBins , bins);
  TH1F* hZmm   = new TH1F( "hZmm"  ,"Z+jets, e  to tau" , nBins , bins);
  TH1F* hZmj   = new TH1F( "hZmj"  ,"Z+jets, jet to tau", nBins , bins);
  TH1F* hTTb   = new TH1F( "hTTb"  ,"ttbar"             , nBins , bins);
  TH1F* hQCD   = new TH1F( "hQCD"  ,"QCD"               , nBins , bins);
  TH1F* hVV    = new TH1F( "hVV"   ,"Diboson"           , nBins , bins);

  // pZeta OS, N pZ sideband OS, pZeta SS, N sideband SS, N QCD SS, OS/SS
  TH1F* hParameters   = new TH1F( "hParameters", "" , 7, 0,7);

  // Open the files
  TFile *fData              
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleRun2011-Elec-All_run_Open_ElecTauStream.root", "READ");  
  TFile *fSignalVBF         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleVBFH%d-powheg-PUS4_run_Open_ElecTauStream.root",mH_) ,"READ");  
  TFile *fSignalGGH         
    = new TFile(Form("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleGGFH%d-powheg-PUS4_run_Open_ElecTauStream.root",mH_),"READ");  
  TFile *fBackgroundDY
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleDYJets-50-madgraph-PUS4_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundWJets   
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleWJets-madgraph-PUS4_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundTTbar  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleTTJets-madgraph-PUS4_run_Open_ElecTauStream.root","READ"); 
  TFile *fBackgroundOthers  
    = new TFile("/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStreamSummer11_fAna//nTupleOthers-PUS4_run_Open_ElecTauStream.root","READ"); 

  // choose the analysis: Nominal "", jet up/Down "JetUp/Down" , elec up/down "MuUp/Down" , tau up/down "TauUp/Down"
  TString tree = "outTreePtOrd"+analysis_;

  TTree *data                = (TTree*)fData->Get("outTreePtOrd");
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);

  // split the DY->ll into l=e/mu and l=tau (MC level)
  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  cout << "Now copying g/Z -> tau+ tau- " << endl;
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)==(23*15)");                 // g/Z -> tau+ tau-
  cout << "Now copying g/Z -> e+e- e->tau" << endl;
  TTree *backgroundDYEtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) &&  leptFakeTau"); // g/Z -> e+e- e->tau
  cout << "Now copying g/Z -> e+e- j->tau" << endl;
  TTree *backgroundDYJtoTau  = ((TTree*)fBackgroundDY->Get(tree))->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> e+e- jet->tau

  cout << backgroundDYTauTau->GetEntries()  << " come from DY->tautau"      << endl;
  cout << backgroundDYEtoTau->GetEntries() << " come from DY->ee, e->tau"  << endl;
  cout << backgroundDYJtoTau->GetEntries()  << " come from DY->ee, jet->tau"<< endl;

  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundOthers    = (TTree*)fBackgroundOthers->Get(tree);
 
  TCut lpt("ptL1>18");
  TCut tpt("ptL2>21");
  TCut tiso("tightestHPSDBWP>0");
  TCut liso("combRelIsoLeg1DBeta<0.10");
  TCut lveto("elecFlag==0");
  TCut SS("diTauCharge!=0");
  TCut OS("diTauCharge==0");
  TCut oneJet("pt1>30");
  TCut twoJets("pt1>30 && pt2>30");
  TCut vbf("pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && ptVeto<30");
  TCut novbf("pt2<30 || (pt1>30 && pt2>30 && ptVeto<30 && !(eta1*eta2<0 && Mjj>350 && Deta>3.5))");
  TCut hltevent("HLTx==1 && (run>=163269 || run==1)");
  TCut hltmatch("HLTmatch==1");
  TCut bTag("pt2<30 && ((pt1>20 && jetsBtagHE1>3.3) || (pt2>20 && jetsBtagHE2>3.3))");
  TCut nobTag("pt2<30 && jetsBtagHE1<3.3 && jetsBtagHE2<3.3");
  TCut pZ("(pZetaCorr-1.5*pZetaVisCorr)>-20");
  TCut apZ("(pZetaCorr-1.5*pZetaVisCorr)<-40");

  TCut sbin; TCut sbinPZetaRel; TCut sbinSS; TCut sbinPZetaRelSS; TCut sbinPZetaRev; TCut sbinPZetaRevSS;

  if(selection_.find("inclusive")!=string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch;
  }
  else if(selection_.find("oneJet")!=string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && oneJet;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && oneJet;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && oneJet;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && oneJet;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && oneJet;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && oneJet;
  }
  else if(selection_.find("twoJets")!=string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && twoJets;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && twoJets;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && twoJets;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && twoJets;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && twoJets;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && twoJets;
  }
  else if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && vbf;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && vbf;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && vbf;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && vbf;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && vbf;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && vbf;
  }
  else if(selection_.find("novbf")!=string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && novbf;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && novbf;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && novbf;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && novbf;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && novbf;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && novbf;
  }
  else if(selection_.find("bTag")!=string::npos && selection_.find("nobTag")==string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && bTag;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && bTag;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && bTag;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && bTag;    
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && bTag;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && bTag;
  }
  else if(selection_.find("nobTag")!=string::npos){
    sbin           =  lpt && tpt && tiso && liso && lveto && OS && pZ  && hltevent && hltmatch && nobTag;
    sbinPZetaRel   =  lpt && tpt && tiso && liso && lveto && OS        && hltevent && hltmatch && nobTag;
    sbinPZetaRev   =  lpt && tpt && tiso && liso && lveto && OS && apZ && hltevent && hltmatch && nobTag;
    sbinPZetaRevSS =  lpt && tpt && tiso && liso && lveto && SS && apZ && hltevent && hltmatch && nobTag;
    sbinSS         =  lpt && tpt && tiso && liso && lveto && SS && pZ  && hltevent && hltmatch && nobTag;
    sbinPZetaRelSS =  lpt && tpt && tiso && liso && lveto && SS        && hltevent && hltmatch && nobTag;
  }

  // estimate the W+jets in the selection bin using pZeta extrapolation

  TH1F* hWMt = new TH1F("hWMt","",200,-200,200);
  cout << "Histogramming the pZeta variable every " << hWMt->GetBinWidth(1) << " GeV" << endl;

  ///////////////////////////////////////// Doing OS first...
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRel);
  cout << "Using  " << hWMt->Integral() << " entries from the W+jets OS sample" << endl;
  float OSWinSignalRegionMC = hWMt->Integral(91,200)*Lumi*hltEff_/1000.;
  float scaleFactorOS = (hWMt->Integral(0,80))/(hWMt->Integral(91,200));
  cout << "Extrapolation factor for W OS : P(pZetaCorr<-40)/P(pZetaCorr>-20) ==> " << scaleFactorOS << endl;
 
  hWMt->Reset();
  cout << "Estimating cobtribution from ttbar and others in OS low pZeta tail..." << endl;
  backgroundTTbar->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRel);
  hWMt->Scale(Lumi*hltEff_/1000.);
  float ttbarExtrOS = hWMt->Integral(0,80);
  cout << "Contribution from ttbar in OS is " << ttbarExtrOS << endl;
  hWMt->Reset();
  backgroundOthers->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRel);
  hWMt->Scale(Lumi*hltEff_/1000.);
  float othersExtrOS = hWMt->Integral(0,80);
  cout << "Contribution from single-t and di-boson in OS is " << othersExtrOS << endl;
  float OSWinSignalRegionDATA = data->GetEntries(sbinPZetaRev);
  cout << "Selected events in data in low pZeta tail " << OSWinSignalRegionDATA << endl;
  OSWinSignalRegionDATA -= ttbarExtrOS;
  OSWinSignalRegionDATA -= othersExtrOS;
  OSWinSignalRegionDATA /= scaleFactorOS;
  cout << "W+jets in signal region is estimated to be "  
       << OSWinSignalRegionDATA*scaleFactorOS << "/" << scaleFactorOS << " = " 
       << OSWinSignalRegionDATA <<  " +/- " << sqrt(OSWinSignalRegionDATA/scaleFactorOS)/scaleFactorOS << endl;
  cout << "  ===> the MC prediction was " << OSWinSignalRegionMC << endl;

  hParameters->SetBinContent(1, 1./scaleFactorOS );
  hParameters->SetBinContent(2, OSWinSignalRegionDATA*scaleFactorOS );

  ///////////////////////////////////////// Doing SS last...
  hWMt->Reset();
  backgroundWJets->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRelSS);
  cout << "Using  " << hWMt->Integral() << " entries from the SS W+jets sample" << endl;
  float SSWinSignalRegionMC = hWMt->Integral(91,200)*Lumi*hltEff_/1000.;
  float scaleFactorSS = (hWMt->Integral(0,80))/(hWMt->Integral(91,200));
  cout << "Extrapolation factor for W SS : P(pZetaCorr<-40)/P(pZetaCorr>-20) ==> " << scaleFactorSS << endl;

  hWMt->Reset();
  cout << "Estimating cobtribution from ttbar and others in SS low pZeta tail..." << endl;
  backgroundTTbar->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRelSS);
  hWMt->Scale(Lumi*hltEff_/1000.);
  float ttbarExtrSS = hWMt->Integral(0,80);
  cout << "Contribution from ttbar in SS is " << ttbarExtrSS << endl;
  hWMt->Reset();
  backgroundOthers->Draw("(pZetaCorr-1.5*pZetaVisCorr)>>hWMt","(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinPZetaRelSS);
  hWMt->Scale(Lumi*hltEff_/1000.);
  float othersExtrSS = hWMt->Integral(0,80);
  cout << "Contribution from single-t and di-boson in SS is " << othersExtrSS << endl;
  float SSWinSignalRegionDATA = data->GetEntries(sbinPZetaRevSS);
  cout << "Selected events in data in low pZeta tail " << SSWinSignalRegionDATA << endl;
  SSWinSignalRegionDATA -= ttbarExtrSS;
  SSWinSignalRegionDATA -= othersExtrSS;
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
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("Others");
  samples.push_back("TTbar");
  samples.push_back("SS");
  samples.push_back("WJets");
  samples.push_back("DYEtoTau");
  samples.push_back("DYJtoTau");
  samples.push_back("DYToTauTau");

  

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]      = data;
  tMap["ggH115"]    = signalGGH;
  tMap["qqH115"]    = signalVBF;
  tMap["DYToTauTau"]= backgroundDYTauTau;
  tMap["DYEtoTau"]  = backgroundDYEtoTau;
  tMap["DYJtoTau"]  = backgroundDYJtoTau;
  tMap["WJets"]     = backgroundWJets;
  tMap["Others"]    = backgroundOthers;
  tMap["TTbar"]     = backgroundTTbar;
  tMap["SS"]        = data;


  
  std::map<TString,Float_t> vMap;


  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << "Dealing with sample " << samples[iter] << endl;
    
    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,"" , nBins , bins);

    TTree* currentTree = 0;
    
    if((it->first).find("SS")!=string::npos){
      
      cout << "Remove W contamination from SS data sample ... " << endl;
      currentTree = (it->second);
      
      float error2OnQCD = 0.0;

      TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
      hHelp->Reset();
      currentTree->Draw(variable_+">>hHelp", sbinSS);
      int SSevents = hHelp->GetEntries();
      cout << "Selected SS events in data " << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);
      
      hHelp->Reset();
      backgroundWJets->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from W+jets (from " << hHelp->GetEntries() << " entries)" << endl;
      float sFWSS = 
	( selection_.find("novbf")!=string::npos || selection_.find("nobTag")!=string::npos ) ? SSWinSignalRegionDATA/SSWinSignalRegionMC : WcorrectionFactorSS; // from the extrapolation factor DATA/MC
      hHelp->Scale(sFWSS*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from W+jets by extrapolating" << endl;
      cout << " ==> removing W+jets from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow( hHelp->Integral()/hHelp->GetEntries(), 2)*hHelp->GetEntries(); // error on MC W+jets SS events
      error2OnQCD +=  pow(WcorrectionFactorSS*0.06,2)*pow(hHelp->GetEntries(),2);                                   // error on W+jets extrapolation factor ==> 6% according to Artur
      cout << sqrt(error2OnQCD) << " <==  W" << endl;      

      hHelp->Reset();
      backgroundTTbar->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from TTbar (from " << hHelp->GetEntries() << " entries)" << endl;
      hHelp->Scale(1.0*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from TTbar" << endl;
      cout << " ==> removing TTbar from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC TTbar SS events
      cout << sqrt(error2OnQCD) << " <== W + TTb" << endl;          

      hHelp->Reset();
      backgroundDYJtoTau->Draw(variable_+">>hHelp", "(sampleWeight*puWeight*HLTweightTau)"*sbinSS);
      cout << "We expect " << hHelp->Integral()*Lumi/1000*hltEff_ << " SS events from DY->ee, jet->tau" << endl;
      hHelp->Scale(JtoTauCorrectionFactor*Lumi/1000*hltEff_);
      cout << "We estimate " << hHelp->Integral() << " SS events from DY->ee, jet->tau" << endl;
      cout << " ==> removing DY->ee, e->jet from SS...." << endl;
      h1->Add(hHelp, -1 );
      if(hHelp->GetEntries()>0) error2OnQCD +=  pow(hHelp->Integral()/hHelp->GetEntries(),2)*hHelp->GetEntries();   // error on MC DY->ee, e->tau events
      cout << sqrt(error2OnQCD) << " <== W + TTb + DY(1)" << endl;      

      //  OS/SS ratio
      h1->Scale(1.06);
      cout << "After removing the expected contribution from W+jets and rescaling by 1.06 we expect " 
	   << h1->Integral() << " events from QCD processes" << endl;

      hParameters->SetBinContent(5, SSevents);
      hParameters->SetBinContent(6, h1->Integral()/SSevents);

      cout << "Total unceratinty from bkg subtraction in SS region is " << sqrt(error2OnQCD) << endl;
      float totalRelErrorOnQCD = 0.02 + sqrt(error2OnQCD)/h1->Integral(); //0.02 ==> uncertainty on OS/SS ratio
      hParameters->SetBinContent(7,totalRelErrorOnQCD);

    }
    else{

      currentTree = (it->second);
      if((it->first).find("DYJtoTau")==string::npos)
	currentTree->Draw(variable_+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbin);
      else
	currentTree->Draw(variable_+">>"+h1Name, "(sampleWeight*puWeight*HLTweightTau*HLTweightElec)"*sbinSS);

      // scale by correction factors
      if((it->first).find("Data")==string::npos)
	h1->Scale(Lumi/1000*hltEff_);

      // if W+jets, scale by extrapolation
      float sFWOS = 
	( selection_.find("novbf")!=string::npos || selection_.find("nobTag")!=string::npos ) ? OSWinSignalRegionDATA/OSWinSignalRegionMC : WcorrectionFactorOS;
      if((it->first).find("WJets")!=string::npos){
	h1->Scale( sFWOS );
	hW->Add(h1,1.0);
      }

      // if DY->tautau, and vbf scale by ratio data/MC
      if((it->first).find("DYToTauTau")!=string::npos && selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos){
	cout << "DY->tautau will be rescaled by 1.03 according to the Z->mumu+vbf/Z->mumu ratio" << endl;
	h1->Scale(VbfExtrapolationFactor);
      }

      // if DY->ee, e->tau, scale by fake-rate
      if((it->first).find("DYEtoTau")!=string::npos){
	float sF = EtoTauCorrectionFactor;
	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) sF *= VbfExtrapolationFactor;
	h1->Scale( sF );
	hZmm->Add(h1,1.0);
      }

      // if DY->ee, jet->tau, scale by fake-rate
      if((it->first).find("DYJtoTau")!=string::npos){
	float sF = JtoTauCorrectionFactor;
	if(selection_.find("vbf")!=string::npos && selection_.find("novbf")==string::npos) sF *= VbfExtrapolationFactor;
	h1->Scale( sF );
	hZmj->Add(h1,1.0);
      }

    }
  
    /////////////////////////////////////////////////////////////////////////////////////

    // Legend
    if( (it->first).find("DYEtoTau")!=string::npos ||  
	(it->first).find("DYJtoTau")!=string::npos || 
	(it->first).find("WJets")!=string::npos || 
	(it->first).find("Others")!=string::npos ) {
      hEWK->SetFillColor(kBlue);
      hEWK->Add(h1,1.0);

      if( (it->first).find("Others")!=string::npos )
	hVV->Add(h1,1.0);
    }

    if( (it->first).find("DYToTauTau")!=string::npos ) {
      hZtt->Add(h1,1.0);
      hZtt->SetFillColor(kWhite);
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      hTTb->Add(h1,1.0);
      hTTb->SetFillColor(kMagenta);     
    }
    if( (it->first).find("SS")!=string::npos ) {
      hQCD->Add(h1,1.0);
      hQCD->SetFillColor(42);
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1->Add(h1,1.0);
      hSgn1->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2->Add(h1,1.0);
      hSgn2->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn2->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
    }
    if((it->first).find("Data")!=string::npos){
      hData->Add(h1,1.0);
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

    // adding alltogether
    hSiml->Add(h1,1.0);

    if(VERBOSE) cout<<(it->first) << " ==> " 
		    << h1->Integral() << " +/- " 
		    << TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries())
		    << endl;
  }

  // all signal summed together:
  hSgn->Add(hSgn1,hSgn2,1,1);
  hSgn->SetFillColor(kRed);
  hSgn->SetLineWidth(2);

  //Adding to the stack
  aStack->Add(hQCD);
  aStack->Add(hEWK);
  aStack->Add(hTTb);
  aStack->Add(hZtt);
  aStack->Add(hSgn);

  // legend
  leg->AddEntry(hData,"Observed","P");
  leg->AddEntry(hSgn,Form("(%.0fx) H#rightarrow#tau#tau m_{H}=%d",magnifySgn_,mH_),"F");
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

  leg->Draw();

  c1->SaveAs(Form("plots/plot_eTau_mH%d_%s_%s_%s.png",mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()));

  // templates for fitting
  TFile* fout = new TFile(Form("histograms/eTau_mH%d_%s_%s_%s.root",mH_,selection_.c_str(),analysis_.c_str(),variable_.Data()),"RECREATE");
  fout->cd();
  hQCD->Write();
  hZmm->Write();
  hZmj->Write();
  hTTb->Write();
  hZtt->Write();
  hW->Write();
  hVV->Write();
  hSgn1->Write();
  hSgn2->Write();
  hData->Write();
  hParameters->Write();
  fout->Write();
  fout->Close();

}


///////////////////////////////////////////////////////////////////////////////////////////////



void plotElecTauAll(){


  plotElecTau(120,"vbf",""         ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","TauUp"    ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","TauDown"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","JetUp"    ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","JetDown"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","ElecUp"    ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"vbf","ElecDown"  ,"diTauVisMass","mass","GeV",-1);

  plotElecTau(120,"novbf",""       ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","TauUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","TauDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","JetUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","JetDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","ElecUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"novbf","ElecDown","diTauVisMass","mass","GeV",-1);

  plotElecTau(120,"twoJets",""       ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","TauUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","TauDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","JetUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","JetDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","ElecUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"twoJets","ElecDown","diTauVisMass","mass","GeV",-1);

  plotElecTau(120,"oneJet",""       ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","TauUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","TauDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","JetUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","JetDown","diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","ElecUp"  ,"diTauVisMass","mass","GeV",-1);
  plotElecTau(120,"oneJet","ElecDown","diTauVisMass","mass","GeV",-1);


}
