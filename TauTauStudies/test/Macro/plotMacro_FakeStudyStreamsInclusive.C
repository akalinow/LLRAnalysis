#include <cstdlib>
#include <iostream> 
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

//#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#endif

#define VERBOSE true
#define SAVE true

void fakeStudyElec( TString weightFile = "TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",
		    Double_t effS_ = 0.5,
		    TCut Cuts_ = "pt1>0",
		    TString variable_ = "MtLeg1",
		    TString XTitle_ = "M_{T}(#mu,MET)",
		    TString Unities_ = "GeV",
		    Int_t nBins_ = 12, Float_t xMin_=0, Float_t xMax_=120,
		    Float_t magnifySgn_ = 10,
		    Int_t logy_ = 0 ) 
{   
  
  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TLegend* leg = new TLegend(0.55,0.47,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("e+#tau_{had}");

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", 1000.));
  TH1F* hSiml = new TH1F();
  TH1F* hSgn   = new TH1F();
  TH1F* hSgn1  = new TH1F();
  TH1F* hSgn2  = new TH1F();


  TMVA::Tools::Instance();

  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );   

  Float_t pt1, pt2;
  Float_t Deta, Mjj;
  Float_t eta1,eta2;

  reader->AddVariable( "pt1", &pt1);
  reader->AddVariable( "pt2", &pt2);
  reader->AddVariable( "Deta",&Deta);
  reader->AddVariable( "Mjj", &Mjj);

  reader->AddSpectator("eta1",&eta1);
  reader->AddSpectator("eta2",&eta2);

  reader->BookMVA( "Cuts", TString("/home/llr/cms/lbianchini/CMSSW_3_9_9/src/LLRAnalysis/TauTauStudies/test/Macro/weights/")+weightFile ); 

  // define histos

  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleVBFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleGGFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDYToTauTau-20-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYEleEle  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDYToEE-20-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleQCD_Open_ElecTauStream.root";
  TString fBackgroundNameG1J       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleG1Jet_Open_ElecTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleTTJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/v2/nTupleDiBoson_Open_ElecTauStream.root";





  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYEleEle(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundG1J(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundDiBoson(0);
 

  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYEleEle = TFile::Open( fBackgroundNameDYEleEle ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundG1J      = TFile::Open( fBackgroundNameG1J ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar ||
     !fSignalGGH || !fBackgroundDYEleEle || !fBackgroundG1J || !fBackgroundDiBoson ){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);
  TTree *backgroundDYTauTau  = (TTree*)fBackgroundDYTauTau->Get(tree);
  TTree *backgroundDYEleEle  = (TTree*)fBackgroundDYEleEle->Get(tree);
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundG1J       = (TTree*)fBackgroundG1J->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);
 
  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("DiBoson");
  samples.push_back("TTbar");
  samples.push_back("Wjets");
  samples.push_back("Zeleele");
  samples.push_back("Ztautau");
  samples.push_back("G1J");
  samples.push_back("QCD");

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["Zeleele"]=backgroundDYEleEle;
  tMap["Wjets"]=backgroundWJets;
  tMap["G1J"]=backgroundG1J;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["DiBoson"]=backgroundDiBoson;

  Float_t pt1_, pt2_;
  Float_t Deta_, Mjj_;
  Float_t Dphi,diTauSVFitPt,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1,sampleWeight,ptVeto;
  Int_t tightestHPSWP;

  std::map<TString,Float_t> vMap;

  for( unsigned iter=0; iter<samples.size(); iter++){

    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,"" , nBins_ ,xMin_ , xMax_);

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TTree* currentTree = (TTree*)(it->second)->CopyTree(Cuts_);
    Int_t counter = 0;

    currentTree->SetBranchAddress( "pt1", &pt1_ );
    currentTree->SetBranchAddress( "pt2", &pt2_ );
    currentTree->SetBranchAddress( "Deta",&Deta_ );
    currentTree->SetBranchAddress( "Mjj", &Mjj_ );
    currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
    //currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
    currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
    currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
    currentTree->SetBranchAddress( "ptL1", &ptL1 );
    currentTree->SetBranchAddress( "ptL2",  &ptL2 );
    currentTree->SetBranchAddress( "etaL1", &etaL1 );
    currentTree->SetBranchAddress( "etaL2", &etaL2 );
    currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
    currentTree->SetBranchAddress( "tightestHPSWP",&tightestHPSWP);
    currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
    currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
    currentTree->SetBranchAddress( "numPV",&numPV);
    currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
    currentTree->SetBranchAddress( "ptVeto",&ptVeto);

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {

      currentTree->GetEntry(ievt);

      if (ievt%10000 == 0){
	std::cout << (it->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
      }

      pt1  = pt1_;
      pt2  = pt2_;
      Deta = Deta_;
      Mjj  = Mjj_;

      vMap["Dphi"]= Dphi;
      vMap["diTauSVFitPt"]= diTauSVFitPt;
      //vMap["diTauSVFitEta"]= diTauSVFitEta;
      vMap["diTauSVFitMass"]= diTauSVFitMass;
      vMap["diTauVisMass"]= diTauVisMass;
      vMap["ptL1"]= ptL1;
      vMap["ptL2"]= ptL2;
      vMap["etaL1"]= etaL1;
      vMap["etaL2"]= etaL2;
      vMap["diTauCharge"]= Float_t(diTauCharge);
      vMap["MtLeg1"]= MtLeg1;
      vMap["numPV"]= numPV;
      vMap["combRelIsoLeg1"]= combRelIsoLeg1;
      vMap["sampleWeight"]= sampleWeight;
      vMap["pt1_"]= pt1;
      vMap["pt2_"]= pt2;
      vMap["Deta_"]= Deta;
      vMap["Mjj_"]= Mjj;
      vMap["ptVeto"]= ptVeto;

      // if efficiency is more than 0 use the optimized cuts
      // else use VBF  preselection
      bool pass = effS_>0 ? reader->EvaluateMVA( "Cuts", effS_ ) : (pt1>0);

      if(pass){
	counter++;
	h1->Fill( vMap[variable_], sampleWeight);
      }
    
    }// end loop


    if( (it->first).find("Zeleele")!=string::npos ) {
      h1->SetFillColor(7);
      leg->AddEntry(h1,"Z#rightarrow ee","F");
    }
    if( (it->first).find("Ztautau")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"Z#rightarrow #tau#tau","F");
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (it->first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
    if( (it->first).find("DiBoson")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Di-Boson","F");
    }
    if( (it->first).find("QCD")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"QCD-multijets","F");
    }
    if( (it->first).find("G1J")!=string::npos ) {
      h1->SetFillColor(29);
      leg->AddEntry(h1,"#gamma+jet","F");
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1 = (TH1F*)h1->Clone("hSgn1");
      hSgn1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("VBF H(115) X %.0f",magnifySgn_),"F");
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2 = (TH1F*)h1->Clone("hSgn2");
      hSgn2->Scale(magnifySgn_);
      hSgn2->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("GGF H(115) X %.0f",magnifySgn_),"F");
    }

    if(iter==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else hSiml->Add(h1);

    aStack->Add(h1);

    if(VERBOSE) cout<<(it->first) << " ==> " 
		   << h1->Integral() << " +/- " 
		   << TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries())
		   << endl;
  }

  // all signal summed together:
  hSgn = (TH1F*)hSgn1->Clone("hSgn");
  hSgn->Add(hSgn1,hSgn2,1,1);

  if(VERBOSE) cout<< "S/sqrt(B) ==> " 
		  << hSgn->Integral()/ TMath::Sqrt(hSiml->Integral()) << " +/- " 
		  << (1./2)*TMath::Sqrt(hSiml->GetEntries())*(hSiml->GetSumOfWeights())/hSiml->Integral()*( hSgn->Integral()/ TMath::Sqrt(hSiml->Integral())  )
		  << endl;

  aStack->Draw("HIST");
  //hSgn1->Draw("HISTSAME");
  //hSgn2->Draw("HISTSAME");
  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle(XTitle_+" ("+Unities_+")");
  hStack->SetYTitle(Form(" Events/(%.0f %s)", hStack->GetBinWidth(1), Unities_.Data() ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.95,"Y");
  leg->Draw();


  delete reader;

}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


void fakeStudyMu( TCut Cuts_ = "ptL1>0",
		  TString variable_ = "MtLeg1",
		  TString XTitle_ = "M_{T}(#mu,MET)",
		  TString Unities_ = "GeV",
		  Int_t nBins_ = 12, Float_t xMin_=0, Float_t xMax_=120,
		  Float_t magnifySgn_ = 10,
		  Int_t logy_ = 0 ) 
{   
  
  float Lumi = 36.;

  TCanvas *c1 = new TCanvas("c1","",5,30,650,600);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetFillColor(10);
  c1->SetTicky();
  c1->SetObjectStat(0);
  c1->SetLogy(logy_);

  TLegend* leg = new TLegend(0.60,0.50,0.85,0.85,NULL,"brNDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.04);
  leg->SetHeader("#mu+#tau_{had}");

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", 1000.));
  TH1F* hSiml = new TH1F();
  TH1F* hSgn   = new TH1F();
  TH1F* hSgn1  = new TH1F();
  TH1F* hSgn2  = new TH1F();
  TH1F* hData  = new TH1F();
 

  // define histos
  TString fDataName                = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/nTupleRun2011-PDMuon_Open_MuTauStream_Inclusive.root";
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream_Inclusive.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleGGFH115-Mu-powheg-PUS1_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameDYMuMu    = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleQCDmu_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream_Inclusive.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/Inclusive/v2/nTupleDiBoson-Mu_Open_MuTauStream_Inclusive.root";

  TFile *fData(0); 
  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYMuMu(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundDiBoson(0);
 
  fData               = TFile::Open( fDataName ); 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYMuMu   = TFile::Open( fBackgroundNameDYMuMu ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar ||
     !fSignalGGH || !fBackgroundDYMuMu || !fBackgroundDiBoson || !fData){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *data                = (TTree*)fData->Get(tree);
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);

  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDYTauTau->Get(tree))->CopyTree("isTauLegMatched>0.5");
  TTree *backgroundDYMuMu    = ((TTree*)fBackgroundDYMuMu->Get(tree))->CopyTree("isTauLegMatched<0.5");
  cout <<backgroundDYTauTau->GetEntries() << " -- " << backgroundDYMuMu->GetEntries() << endl;

  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);
 
  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("DiBoson");
  samples.push_back("TTbar");
  samples.push_back("Wjets");
  samples.push_back("QCD");
  samples.push_back("ZfakeTau");
  samples.push_back("Ztautau");
  samples.push_back("Data");

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]=data;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["ZfakeTau"]=backgroundDYMuMu;
  tMap["Wjets"]=backgroundWJets;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["DiBoson"]=backgroundDiBoson;

  Float_t pt1_, pt2_;
  Float_t Deta_, Mjj_, HLT;
  Float_t Dphi,diTauSVFitPt,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1,sampleWeight,ptVeto;
  Int_t tightestHPSWP;

  std::map<TString,Float_t> vMap;

  for( unsigned iter=0; iter<samples.size(); iter++){

    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi) , nBins_ ,xMin_ , xMax_);

    TFile* dummy = new TFile("dummy.root","RECREATE");
    TCut Cuts = Cuts_;
    TCut hlt = "(HLT==1 || HLT==0)";
    if((it->first).find("Data")!=string::npos) Cuts = Cuts_ && hlt;
    TTree* currentTree = (TTree*)(it->second)->CopyTree(Cuts);
    Int_t counter = 0;

    //currentTree->SetBranchAddress( "pt1", &pt1_ );
    //currentTree->SetBranchAddress( "pt2", &pt2_ );
    //currentTree->SetBranchAddress( "Deta",&Deta_ );
    //currentTree->SetBranchAddress( "Mjj", &Mjj_ );
    currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
    //currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
    currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
    currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
    currentTree->SetBranchAddress( "ptL1", &ptL1 );
    currentTree->SetBranchAddress( "ptL2",  &ptL2 );
    currentTree->SetBranchAddress( "etaL1", &etaL1 );
    currentTree->SetBranchAddress( "etaL2", &etaL2 );
    currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
    currentTree->SetBranchAddress( "tightestHPSWP",&tightestHPSWP);
    currentTree->SetBranchAddress( "HLT",&HLT);
    currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
    currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
    currentTree->SetBranchAddress( "numPV",&numPV);
    currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
    //currentTree->SetBranchAddress( "ptVeto",&ptVeto);

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {

      currentTree->GetEntry(ievt);

      if (ievt%10000 == 0){
	std::cout << (it->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
      }

      //pt1  = pt1_;
      //pt2  = pt2_;
      //Deta = Deta_;
      //Mjj  = Mjj_;

      //vMap["Dphi"]= Dphi;
      vMap["diTauSVFitPt"]= diTauSVFitPt;
      //vMap["diTauSVFitEta"]= diTauSVFitEta;
      vMap["diTauSVFitMass"]= diTauSVFitMass;
      vMap["diTauVisMass"]= diTauVisMass;
      vMap["ptL1"]= ptL1;
      vMap["ptL2"]= ptL2;
      vMap["etaL1"]= etaL1;
      vMap["etaL2"]= etaL2;
      vMap["diTauCharge"]= Float_t(diTauCharge);
      vMap["MtLeg1"]= MtLeg1;
      vMap["numPV"]= numPV;
      vMap["combRelIsoLeg1"]= combRelIsoLeg1;
      vMap["sampleWeight"]= sampleWeight;
      vMap["HLT"]= HLT;
      //vMap["pt1_"]= pt1;
      //vMap["pt2_"]= pt2;
      //vMap["Deta_"]= Deta;
      //vMap["Mjj_"]= Mjj;
      //vMap["ptVeto"]= ptVeto;

      // if efficiency is more than 0 use the optimized cuts
      // else use VBF  preselection
      //bool pass = effS_>0 ? reader->EvaluateMVA( "Cuts", effS_ ) : (pt1>0);
      bool pass = true;

      if((it->first).find("Data")==string::npos) sampleWeight*=(Lumi/1000);

      if(pass){
	counter++;
	h1->Fill( vMap[variable_], sampleWeight);
      }
    
    }// end loop


    if( (it->first).find("ZfakeTau")!=string::npos ) {
      h1->SetFillColor(7);
      leg->AddEntry(h1,"Z#rightarrow #mu#mu","F");
    }
    if( (it->first).find("Ztautau")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"Z#rightarrow #tau#tau","F");
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (it->first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
    if( (it->first).find("DiBoson")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Di-Boson","F");
    }
    if( (it->first).find("QCD")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"QCD-multijets","F");
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1 = (TH1F*)h1->Clone("hSgn1");
      hSgn1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("VBF H(115) X %.0f",magnifySgn_),"F");
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2 = (TH1F*)h1->Clone("hSgn2");
      hSgn2->Scale(magnifySgn_);
      hSgn2->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3005);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("GGF H(115) X %.0f",magnifySgn_),"F");
    }
    if((it->first).find("Data")!=string::npos){
      hData = (TH1F*)h1->Clone("hData");
      hData->Sumw2();
      hData->SetMarkerStyle(20);
      hData->SetMarkerSize(1.2);
      hData->SetMarkerColor(kBlack);
      hData->SetLineColor(kBlack);
      leg->AddEntry(hData,"DATA","P");
    }

    if(iter==0) hSiml=(TH1F*)h1->Clone("hSiml");
    else if((it->first).find("Data")==string::npos) hSiml->Add(h1);

    if((it->first).find("Data")==string::npos) aStack->Add(h1);

    if(VERBOSE) cout<<(it->first) << " ==> " 
		   << h1->Integral() << " +/- " 
		   << TMath::Sqrt(h1->GetEntries())*(h1->Integral()/h1->GetEntries())
		   << endl;
  }

  // all signal summed together:
  hSgn = (TH1F*)hSgn1->Clone("hSgn");
  hSgn->Add(hSgn1,hSgn2,1,1);

  if(VERBOSE) cout<< "S/sqrt(B) ==> " 
		  << hSgn->Integral()/ TMath::Sqrt(hSiml->Integral()) << " +/- " 
		  << (1./2)*TMath::Sqrt(hSiml->GetEntries())*(hSiml->GetSumOfWeights())/hSiml->Integral()*( hSgn->Integral()/ TMath::Sqrt(hSiml->Integral())  )
		  << endl;

  hData->SetXTitle(XTitle_+" ("+Unities_+")");
  hData->SetYTitle(Form(" Events/(%.1f %s)", hData->GetBinWidth(1), Unities_.Data() ) );
  hData->SetTitleSize(0.04,"X");
  hData->SetTitleSize(0.05,"Y");
  hData->SetTitleOffset(0.95,"Y");

  hData->Draw("P");
  aStack->Draw("HISTSAME");
  hData->Draw("PSAME");
  //hSgn1->Draw("HISTSAME");
  //hSgn2->Draw("HISTSAME");

  TH1F* hStack = (TH1F*)aStack->GetHistogram();
  hStack->SetXTitle(XTitle_+" ("+Unities_+")");
  hStack->SetYTitle(Form(" Events/(%.0f %s)", hStack->GetBinWidth(1), Unities_.Data() ) );
  hStack->SetTitleSize(0.04,"X");
  hStack->SetTitleSize(0.05,"Y");
  hStack->SetTitleOffset(0.95,"Y");
  //aStack->GetYaxis()->SetRangeUser(0.0,std::max(hData->GetMaximum(),hStack->GetMaximum())*1.1);
  leg->Draw();


}


void doAllPlotsElecTauStream(){

  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04))","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIso.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISo.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMt.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOS.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",0.2,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOSVBF.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",0.2,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0 && ptVeto<20","diTauVisMass","visible mass ","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOSVBFJetVeto.png");

  ////////////////////////////////////////////////////////////////////////////////////////
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04))","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIso_log.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISo_log.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMt_log.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",-1,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOS_log.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",0.2,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOSVBF_log.png");
  fakeStudyElec("TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",0.2,"pt1>0 && ((abs(etaL1)<1.5 && combRelIsoLeg1<0.08) || (abs(etaL1)>1.5 && combRelIsoLeg1<0.04)) && tightestHPSWP>0 && MtLeg1<40 && diTauCharge==0 && ptVeto<20","diTauVisMass","visible mass ","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_ElecTauStream_visibleMassEleIsoTauISoMtOSVBFJetVeto_log.png");

  return;

}



void doAllPlotsMuTauStream(){
  /*
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","ptL1","p_{T}^{#mu}","GeV",22,10,120,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_PtMuAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","ptL2","p_{T}^{#tau}","GeV",22,10,120,1,1); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_PtTauAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","etaL1","#eta^{#mu}","units",20,-3,3,1,0); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_EtaMuAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","etaL2","#eta^{#tau}","units",20,-3,3,1,0); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_EtaTauAllCuts.png");

  fakeStudyMu("tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","combRelIsoLeg1","CombRelIso^{#mu}","units",50,0,1,1,1); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_MuIso.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0","MtLeg1","M_{T}","GeV",32,0,160,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_Mt.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 &&  MtLeg1<40","diTauCharge","pair-charge","",5,-2.5,2.5,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_Charge_Lin.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40","diTauVisMass","visible mass","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_VisMassAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40","diTauVisMass","visible mass","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_VisMassAllCuts_Lin.png");
  */
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40","diTauSVFitMass","full mass","GeV",35,0,350,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_FullMassAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40","diTauSVFitMass","full mass","GeV",35,0,350,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_FullMassAllCuts_Lin.png");


  return;

}


void doAllPlotsMuTauStreamHLT(){

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40 && (HLT==1 || HLT==2)","ptL1","p_{T}^{#mu}","GeV",21,15,120,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_PtMuAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40 && (HLT==1 || HLT==2)","ptL2","p_{T}^{#tau}","GeV",20,20,120,1,1); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_PtTauAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40 && (HLT==1 || HLT==2)","etaL1","#eta^{#mu}","units",20,-3,3,1,0); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_EtaMuAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40 && (HLT==1 || HLT==2)","etaL2","#eta^{#tau}","units",20,-3,3,1,0); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_EtaTauAllCuts.png");

  fakeStudyMu("tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40 && (HLT==1 || HLT==2)","combRelIsoLeg1","CombRelIso^{#mu}","units",50,0,1,1,1); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_MuIso.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && MtLeg1<40","HLT","HLT: 0=!#mu!#tau, 1=#mu#tau, 2=#mu!#tau, 3=!#mu#tau","",4,0,4,1,0); 
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_HLT.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 && (HLT==1 || HLT==2)","MtLeg1","M_{T}","GeV",32,0,160,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_Mt.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 &&  MtLeg1<40 && (HLT==1 || HLT==2)","diTauCharge","pair-charge","",5,-2.5,2.5,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_NminusOne_Charge_Lin.png");

  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40 && (HLT==1 || HLT==2)","diTauVisMass","visible mass","GeV",20,0,200,1,1);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_VisMassAllCuts.png");
  fakeStudyMu("combRelIsoLeg1<0.1 && tightestHPSWP>0 && diTauCharge==0 &&  MtLeg1<40 && (HLT==1 || HLT==2)","diTauVisMass","visible mass","GeV",20,0,200,1,0);
  if(SAVE) gPad->SaveAs("plot_MuTauStream_Data_Inclusive_VisMassAllCuts_Lin.png");

  return;

}

