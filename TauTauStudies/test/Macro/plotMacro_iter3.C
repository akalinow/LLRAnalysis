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
#include "TCut.h"

#define VERBOSE true
#define SAVE true

void fakeStudyElec( TCut Cuts_ = "tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && elecFlag==0 && (pZeta-1.5*pZetaVis)>-20",
		    Bool_t doSS_ = true,
		    Bool_t doVBFPreRegion_ = false,
		    TString variable_ = "diTauVisMass",
		    TString XTitle_ = "mass",
		    TString Unities_ = "GeV",
		    Int_t nBins_ = 40, Float_t xMin_=0, Float_t xMax_=200,
		    Float_t magnifySgn_ = 1,
		    Float_t hltEff_ = 0.90*0.994*0.970,
		    Int_t enableHLTmatching_ = 0,
		    Int_t logy_ = 0 ) 
{   
  
  

  //float Lumi = 191;
  float Lumi = 185;

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

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi ));
  TH1F* hSiml = new TH1F();
  TH1F* hSgn   = new TH1F();
  TH1F* hSgn1  = new TH1F();
  TH1F* hSgn2  = new TH1F();
  TH1F* hData  = new TH1F();

 

  // define histos
  // OpenNTuples
  TString fDataName                = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleRun2011-Elec_Open_ElecTauStream.root";
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleVBFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleGGFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleDYJets-50-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYEleEle  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleDYJets-50-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root";
 TString fBackgroundNameEWK       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleElectroWeak_Open_ElecTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleQCD_Open_ElecTauStream.root";
  TString fBackgroundNameG1J       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleG1Jet_Open_ElecTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleTTJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameTop       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleTop_Open_ElecTauStream.root";
  TString fBackgroundNameSingleTop = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleSingleTop_Open_ElecTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011_iter3/Inclusive/nTupleDiBoson_Open_ElecTauStream.root";



  TFile *fData(0); 
  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYEleEle(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundEWK(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundG1J(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundTop(0);
  TFile *fBackgroundSingleTop(0);
  TFile *fBackgroundDiBoson(0);
 
  fData               = TFile::Open( fDataName ); 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYEleEle = TFile::Open( fBackgroundNameDYEleEle ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundEWK      = TFile::Open( fBackgroundNameEWK ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundG1J      = TFile::Open( fBackgroundNameG1J ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar );
  fBackgroundSingleTop= TFile::Open( fBackgroundNameSingleTop ); 
  fBackgroundTop      = TFile::Open( fBackgroundNameTop ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar || !fBackgroundSingleTop ||
     !fSignalGGH || !fBackgroundDYEleEle || !fBackgroundG1J|| !fBackgroundTop || !fBackgroundEWK  || !fBackgroundDiBoson  || !fData){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *data                = (TTree*)fData->Get(tree);
  TTree* dataSS              = 0;
  TTree* WJetsSS             = 0;
  TTree* WJetsSSFullMt       = 0;
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);

  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDYTauTau->Get(tree))->CopyTree("isTauLegMatched>0.5");
  TTree *backgroundDYEleEle    = ((TTree*)fBackgroundDYEleEle->Get(tree))->CopyTree("isTauLegMatched<0.5");
  cout <<backgroundDYTauTau->GetEntries() << " -- " << backgroundDYEleEle->GetEntries() << endl;
  if(doSS_){
    dataSS  = doVBFPreRegion_ ? 
      ((TTree*)fData->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && HLTx==1)") : 
      ((TTree*)fData->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && HLTx==1)");
    WJetsSS = doVBFPreRegion_ ? 
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5)") :
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0)"); 
    WJetsSSFullMt = doVBFPreRegion_ ? 
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5)") :
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && diTauCharge!=0)");
  }
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundEWK       = (TTree*)fBackgroundEWK->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundG1J       = (TTree*)fBackgroundG1J->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundSingleTop = (TTree*)fBackgroundSingleTop->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);
  TTree *backgroundTop       = (TTree*)fBackgroundTop->Get(tree);
 
  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  //samples.push_back("DiBoson");
  //samples.push_back("TTbar");
  //samples.push_back("SingleTop");
  //samples.push_back("Wjets");
  samples.push_back("EWK");
  samples.push_back("Top");
  if(!doSS_){
    samples.push_back("G1J");
    samples.push_back("QCD");
  }
  else samples.push_back("SS");
  samples.push_back("Zeleele");
  samples.push_back("Ztautau");
  samples.push_back("Data");
  

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]=data;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["Zeleele"]=backgroundDYEleEle;
  tMap["Wjets"]=backgroundWJets;
  tMap["EWK"]=backgroundEWK;
  tMap["G1J"]=backgroundG1J;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["Top"]=backgroundTop;
  tMap["SingleTop"]=backgroundSingleTop;
  tMap["DiBoson"]=backgroundDiBoson;
  tMap["SS"]=dataSS;


  Float_t pt1, pt2, Deta, Mjj, eta1, eta2;
  Float_t diTauSVFitPt,diTauVisPt,diTauVisEta,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1DBeta,combRelIsoLeg1,pZeta,pZetaVis, MEt, ptVeto, leadTrackPt, jetsBtagHE1, jetsBtagHE2;
  Float_t sampleWeight,puWeight, HLTweight;

  std::map<TString,Float_t> vMap;

  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << samples[iter] << endl;

    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi) , nBins_ ,xMin_ , xMax_);

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TCut Cuts = Cuts_;
    TCut hlt = enableHLTmatching_ ? "(HLTmatch==1 && HLTx==1)" : "(HLTx==1)";
    if((it->first).find("Data")!=string::npos) Cuts = Cuts_ && hlt;

    TTree* currentTree = 0;

    if((it->first).find("SS")!=string::npos){

      cout << "Dealing with SS" << endl;

      currentTree = (it->second);
      
      //cout << currentTree->GetEntries() << endl;
      TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
      hHelp->Reset();
      TString variable = ( (std::string(variable_.Data())).find("pZeta")!=string::npos ) ? "pZeta-1.5*pZetaVis" : variable_;
      currentTree->Draw(variable+">>hHelp");
      //cout << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);
      //cout << h1->Integral() << endl;
      TH1F* hWMt = new TH1F("hWMt","",200,-200,200);
      WJetsSSFullMt->Draw("(pZeta-1.5*pZetaVis)>>hWMt");
      float scaleFactor = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
      cout << "Scale factor = " << scaleFactor << endl;
      TH1F* hW = (TH1F*)h1->Clone("hW");
      hW->Reset();
      float SSWinSignalRegion = doVBFPreRegion_ ?
	data->GetEntries("tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)<-40 && diTauCharge!=0  && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5") :
	data->GetEntries("tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && elecFlag==0 && (pZeta-1.5*pZetaVis)<-40 && diTauCharge!=0");
      SSWinSignalRegion *= (1./scaleFactor);
      WJetsSS->Draw(variable+">>hW");
      hW->Scale(SSWinSignalRegion/hW->Integral());
      h1->Add(hW, -1 );
      h1->Scale(1.06);
      //cout << h1->Integral() << endl;
    }
    else{

      currentTree =  (TTree*)(it->second)->CopyTree(Cuts);
      
      Int_t counter = 0;
      
      currentTree->SetBranchAddress( "pt1", &pt1 );
      currentTree->SetBranchAddress( "pt2", &pt2 );
      currentTree->SetBranchAddress( "Deta",&Deta );
      currentTree->SetBranchAddress( "Mjj", &Mjj );
      currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
      currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
      currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
      currentTree->SetBranchAddress( "diTauVisPt",&diTauVisPt);
      currentTree->SetBranchAddress( "diTauVisEta",&diTauVisEta);
      currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
      currentTree->SetBranchAddress( "ptL1",  &ptL1 );
      currentTree->SetBranchAddress( "ptL2",  &ptL2 );
      currentTree->SetBranchAddress( "etaL1", &etaL1 );
      currentTree->SetBranchAddress( "etaL2", &etaL2 );
      currentTree->SetBranchAddress( "combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta);
      //currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
      currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
      currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
      currentTree->SetBranchAddress( "pZeta",&pZeta);
      currentTree->SetBranchAddress( "pZetaVis",&pZetaVis);
      currentTree->SetBranchAddress( "numPV",&numPV);
      currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
      currentTree->SetBranchAddress( "puWeight",&puWeight);
      currentTree->SetBranchAddress( "HLTweight",&HLTweight);
      currentTree->SetBranchAddress( "ptVeto",&ptVeto);
      currentTree->SetBranchAddress( "MEt",&MEt);
      currentTree->SetBranchAddress( "leadTrackPt",&leadTrackPt);
      currentTree->SetBranchAddress( "jetsBtagHE1",&jetsBtagHE1);
      currentTree->SetBranchAddress( "jetsBtagHE2",&jetsBtagHE2);
      
      cout << "starting loop..." << endl;

      for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {
	
	currentTree->GetEntry(ievt);
	
	if (ievt%10000 == 0){
	  std::cout << (it->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
	}
	
	
	vMap["diTauSVFitPt"]  = diTauSVFitPt;
	vMap["diTauSVFitEta"] = diTauSVFitEta;
	vMap["diTauSVFitMass"]= diTauSVFitMass;
	vMap["diTauVisPt"]    = diTauVisPt;
	vMap["diTauVisEta"]   = diTauVisEta;
	vMap["diTauVisMass"]  = diTauVisMass;
	vMap["ptL1"]  = ptL1;
	vMap["ptL2"]  = ptL2;
	vMap["etaL1"] = etaL1;
	vMap["etaL2"] = etaL2;
	vMap["diTauCharge"]= Float_t(diTauCharge);
	vMap["MtLeg1"]= MtLeg1;
	vMap["pZeta"] = pZeta;
	vMap["pZetaVis"] = pZetaVis;
	vMap["pZetaCutVar"] = pZetaVis-1.5*pZetaVis;
	vMap["numPV"]= numPV;
	vMap["combRelIsoLeg1"]= combRelIsoLeg1;
	vMap["combRelIsoLeg1DBeta"]= combRelIsoLeg1DBeta;
	vMap["sampleWeight"]= sampleWeight;
	vMap["puWeight"]= puWeight;
	vMap["puWeight"]= puWeight;
	vMap["jetsBtagHE1"]= jetsBtagHE1;
	vMap["jetsBtagHE2"]= jetsBtagHE2;
	vMap["leadTrackPt"]= leadTrackPt;
	
	vMap["pt1"]= pt1;
	vMap["pt2"]= pt2;
	vMap["Deta"]= Deta;
	vMap["Mjj"]= Mjj;
	vMap["ptVeto"]= ptVeto;
	
	if((it->first).find("Data")==string::npos) sampleWeight*=(Lumi/1000*hltEff_*puWeight*HLTweight);
	
	counter++;
	h1->Fill( vMap[variable_], sampleWeight);
	
      }// end loop
    }

    if( (it->first).find("Zeleele")!=string::npos ) {
      h1->SetFillColor(7);
      leg->AddEntry(h1,"Z+jets, fake-#tau","F");
    }
    if( (it->first).find("Ztautau")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"Z+jets, genuine-#tau","F");
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (it->first).find("Top")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"Top","F");
    }
    if( (it->first).find("SingleTop")!=string::npos ) {
      h1->SetFillColor(29);
      leg->AddEntry(h1,"single-t","F");
    }
    if( (it->first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
    if( (it->first).find("EWK")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"EWK","F");
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
    if( (it->first).find("SS")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"SS \\ W","F");
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1 = (TH1F*)h1->Clone("hSgn1");
      hSgn1->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("VBF H(115) X %.0f",magnifySgn_),"F");
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2 = (TH1F*)h1->Clone("hSgn2");
      hSgn2->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
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
      hData->SetXTitle(XTitle_+" ("+Unities_+")");
      hData->SetYTitle(Form(" Events/(%.0f %s)", hData->GetBinWidth(1), Unities_.Data() ) );
      hData->SetTitleSize(0.04,"X");
      hData->SetTitleSize(0.05,"Y");
      hData->SetTitleOffset(0.95,"Y");
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

  c1->SaveAs("tmp.png");

}


///////////////////////////////////////////////////////////////////////////////////////////////

void fakeStudyMu( TCut Cuts_ = "tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && diTauCharge==0 && muFlag==0 && (pZeta-1.5*pZetaVis)>-20",
		  Bool_t doSS_ = true,
		  Bool_t doVBFPreRegion_ = false,
		  TString variable_ = "diTauVisMass",
		  TString XTitle_ = "mass",
		  TString Unities_ = "GeV",
		  Int_t nBins_ = 40, Float_t xMin_=0, Float_t xMax_=200,
		  Float_t magnifySgn_ = 1,
		  Float_t hltEff_ = 0.925*0.993*0.975,
		  Int_t enableHLTmatching_ = 0,
		  Int_t logy_ = 0 ) 
{   
  
  
  
  float Lumi = 191;
  
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
  leg->SetHeader("#mu+#tau_{had}");

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi ));
  TH1F* hSiml = new TH1F();
  TH1F* hSgn   = new TH1F();
  TH1F* hSgn1  = new TH1F();
  TH1F* hSgn2  = new TH1F();
  TH1F* hData  = new TH1F();


  // OpenNTuples
  TString fDataName                = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleRun2011-Mu_Open_MuTauStream.root";
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleGGFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYMuMu    = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameEWK       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleElectroWeak_Open_MuTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleQCDmu-pythia-20-15-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameTop       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleTop_Open_MuTauStream.root";
  TString fBackgroundNameSingleTop = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleSingleTop_Open_MuTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter3/Inclusive/nTupleDiBoson_Open_MuTauStream.root";

  TFile *fData(0); 
  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYMuMu(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundEWK(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundTop(0);
  TFile *fBackgroundSingleTop(0);
  TFile *fBackgroundDiBoson(0);
 
  fData               = TFile::Open( fDataName ); 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYMuMu   = TFile::Open( fBackgroundNameDYMuMu ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundEWK      = TFile::Open( fBackgroundNameEWK ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar );
  fBackgroundSingleTop= TFile::Open( fBackgroundNameSingleTop );
  fBackgroundTop      = TFile::Open( fBackgroundNameTop ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar || !fBackgroundSingleTop ||
     !fSignalGGH || !fBackgroundDYMuMu || !fBackgroundDiBoson || !fData || !fBackgroundTop || !fBackgroundEWK ){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *data                = (TTree*)fData->Get(tree);
  TTree* dataSS              = 0;
  TTree* WJetsSS             = 0;
  TTree* WJetsSSFullMt       = 0;
  TTree *signalVBF           = (TTree*)fSignalVBF->Get(tree);
  TTree *signalGGH           = (TTree*)fSignalGGH->Get(tree);

  TFile* dummy1 = new TFile("dummy1.root","RECREATE");
  TTree *backgroundDYTauTau  = ((TTree*)fBackgroundDYTauTau->Get(tree))->CopyTree("isTauLegMatched>0.5");
  TTree *backgroundDYMuMu    = ((TTree*)fBackgroundDYMuMu->Get(tree))->CopyTree("isTauLegMatched<0.5");
  cout <<backgroundDYTauTau->GetEntries() << " -- " << backgroundDYMuMu->GetEntries() << endl;
  if(doSS_){
    dataSS  = doVBFPreRegion_ ? 
      ((TTree*)fData->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && ( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) ))") : 
      ((TTree*)fData->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && ( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) ))");
    WJetsSS = doVBFPreRegion_ ? 
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && HLTmu==1)") :
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)>-20 && diTauCharge!=0 && HLTmu==1)"); 
    WJetsSSFullMt = doVBFPreRegion_ ? 
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && diTauCharge!=0 && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && HLTmu==1)") :
      ((TTree*)fBackgroundWJets->Get(tree))->CopyTree("(tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && diTauCharge!=0 && HLTmu==1)");
  }
  TTree *backgroundWJets     = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundEWK       = (TTree*)fBackgroundEWK->Get(tree);
  TTree *backgroundQCD       = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundTTbar     = (TTree*)fBackgroundTTbar->Get(tree);
  TTree *backgroundSingleTop = (TTree*)fBackgroundSingleTop->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);
  TTree *backgroundTop       = (TTree*)fBackgroundTop->Get(tree);

  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  //samples.push_back("DiBoson");
  //samples.push_back("TTbar");
  //samples.push_back("SingleTop");
  //samples.push_back("Wjets");
  samples.push_back("EWK");
  samples.push_back("Top");
  if(!doSS_){
    samples.push_back("QCD");
  }
  else samples.push_back("SS");
  samples.push_back("Zmumu");
  samples.push_back("Ztautau");
  samples.push_back("Data");
  

  // here I define the map between a sample name and its tree
  std::map<std::string,TTree*> tMap;
  tMap["Data"]=data;
  tMap["ggH115"]=signalGGH;
  tMap["qqH115"]=signalVBF;
  tMap["Ztautau"]=backgroundDYTauTau;
  tMap["Zmumu"]=backgroundDYMuMu;
  tMap["Wjets"]=backgroundWJets;
  tMap["EWK"]=backgroundEWK;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;
  tMap["Top"]=backgroundTop;
  tMap["SingleTop"]=backgroundSingleTop;
  tMap["DiBoson"]=backgroundDiBoson;
  tMap["SS"]=dataSS;


  Float_t pt1, pt2, Deta, Mjj, eta1, eta2;
  Float_t diTauSVFitPt,diTauVisPt,diTauVisEta,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1DBeta,combRelIsoLeg1,pZeta,pZetaVis, MEt, ptVeto, leadTrackPt, jetsBtagHE1, jetsBtagHE2;
  Float_t sampleWeight,puWeight, HLTweight;

  std::map<TString,Float_t> vMap;


  for( unsigned iter=0; iter<samples.size(); iter++){

    cout << samples[iter] << endl;
    
    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi) , nBins_ ,xMin_ , xMax_);
    
    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TCut Cuts = Cuts_;
    TCut hlt = enableHLTmatching_ ? "( ((HLTmatch==2 && run<=163261) || (HLTmatch==1 && run>163261)) && ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )" : "( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )";   
    if((it->first).find("Data")!=string::npos) Cuts = Cuts_ && hlt;
    else Cuts = Cuts_ && TCut("HLTmu==1");
    
    TTree* currentTree = 0;
    
    if((it->first).find("SS")!=string::npos){
      
      cout << "Dealing with SS" << endl;
      
      currentTree = (it->second);
      
      //cout << currentTree->GetEntries() << endl;
      TH1F* hHelp = (TH1F*)h1->Clone("hHelp");
      hHelp->Reset();
      TString variable = ( (std::string(variable_.Data())).find("pZeta")!=string::npos ) ? "pZeta-1.5*pZetaVis" : variable_;
      currentTree->Draw(variable+">>hHelp");
      //cout << hHelp->GetEntries() << endl;
      h1->Add(hHelp,1);
      //cout << h1->Integral() << endl;
      TH1F* hWMt = new TH1F("hWMt","",200,-200,200);
      WJetsSSFullMt->Draw("(pZeta-1.5*pZetaVis)>>hWMt");
      float scaleFactor = (hWMt->Integral(0,80))/(hWMt->Integral(90,200));
      cout << "Scale factor = " << scaleFactor << endl;
      TH1F* hW = (TH1F*)h1->Clone("hW");
      hW->Reset();
      float SSWinSignalRegion = doVBFPreRegion_ ?
	data->GetEntries("tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)<-40 && diTauCharge!=0  && pt1>30 && pt2>30 && eta1*eta2<0 && Mjj>350 && Deta>3.5 && ( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )") :
	data->GetEntries("tightestHPSWP>0 && combRelIsoLeg1DBeta<0.10 && muFlag==0 && (pZeta-1.5*pZetaVis)<-40 && diTauCharge!=0 && ( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )");
      SSWinSignalRegion *= (1./scaleFactor);
      WJetsSS->Draw(variable+">>hW");
      hW->Scale(SSWinSignalRegion/hW->Integral());
      h1->Add(hW, -1 );
      h1->Scale(1.06);
      //cout << h1->Integral() << endl;
    }
    else{

      currentTree =  (TTree*)(it->second)->CopyTree(Cuts);
      
      Int_t counter = 0;
      
      currentTree->SetBranchAddress( "pt1", &pt1 );
      currentTree->SetBranchAddress( "pt2", &pt2 );
      currentTree->SetBranchAddress( "Deta",&Deta );
      currentTree->SetBranchAddress( "Mjj", &Mjj );
      currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
      currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
      currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
      currentTree->SetBranchAddress( "diTauVisPt",&diTauVisPt);
      currentTree->SetBranchAddress( "diTauVisEta",&diTauVisEta);
      currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
      currentTree->SetBranchAddress( "ptL1",  &ptL1 );
      currentTree->SetBranchAddress( "ptL2",  &ptL2 );
      currentTree->SetBranchAddress( "etaL1", &etaL1 );
      currentTree->SetBranchAddress( "etaL2", &etaL2 );
      currentTree->SetBranchAddress( "combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta);
      //currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
      currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
      currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
      currentTree->SetBranchAddress( "pZeta",&pZeta);
      currentTree->SetBranchAddress( "pZetaVis",&pZetaVis);
      currentTree->SetBranchAddress( "numPV",&numPV);
      currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
      currentTree->SetBranchAddress( "puWeight",&puWeight);
      currentTree->SetBranchAddress( "ptVeto",&ptVeto);
      currentTree->SetBranchAddress( "MEt",&MEt);
      //currentTree->SetBranchAddress( "leadTrackPt",&leadTrackPt);
      currentTree->SetBranchAddress( "jetsBtagHE1",&jetsBtagHE1);
      currentTree->SetBranchAddress( "jetsBtagHE2",&jetsBtagHE2);
      
      cout << "starting loop..." << endl;

      for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {
	
	currentTree->GetEntry(ievt);
	
	if (ievt%10000 == 0){
	  std::cout << (it->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
	}
	
	
	vMap["diTauSVFitPt"]  = diTauSVFitPt;
	vMap["diTauSVFitEta"] = diTauSVFitEta;
	vMap["diTauSVFitMass"]= diTauSVFitMass;
	vMap["diTauVisPt"]    = diTauVisPt;
	vMap["diTauVisEta"]   = diTauVisEta;
	vMap["diTauVisMass"]  = diTauVisMass;
	vMap["ptL1"]  = ptL1;
	vMap["ptL2"]  = ptL2;
	vMap["etaL1"] = etaL1;
	vMap["etaL2"] = etaL2;
	vMap["diTauCharge"]= Float_t(diTauCharge);
	vMap["MtLeg1"]= MtLeg1;
	vMap["pZeta"] = pZeta;
	vMap["pZetaVis"] = pZetaVis;
	vMap["pZetaCutVar"] = pZeta-1.5*pZetaVis;
	vMap["numPV"]= numPV;
	vMap["combRelIsoLeg1"]= combRelIsoLeg1;
	vMap["combRelIsoLeg1DBeta"]= combRelIsoLeg1DBeta;
	vMap["sampleWeight"]= sampleWeight;
	vMap["puWeight"]= puWeight;
	vMap["puWeight"]= puWeight;
	vMap["jetsBtagHE1"]= jetsBtagHE1;
	vMap["jetsBtagHE2"]= jetsBtagHE2;
	vMap["leadTrackPt"]= leadTrackPt;
	
	vMap["pt1"]= pt1;
	vMap["pt2"]= pt2;
	vMap["Deta"]= Deta;
	vMap["Mjj"]= Mjj;
	vMap["ptVeto"]= ptVeto;
	
	if((it->first).find("Data")==string::npos) sampleWeight*=(Lumi/1000*hltEff_*puWeight);
	
	counter++;
	h1->Fill( vMap[variable_], sampleWeight);
	
      }// end loop
    }

    if( (it->first).find("Zmumu")!=string::npos ) {
      h1->SetFillColor(7);
      leg->AddEntry(h1,"Z+jets, fake-#tau","F");
    }
    if( (it->first).find("Ztautau")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"Z+jets, genuine-#tau","F");
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (it->first).find("Top")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"Top","F");
    }
    if( (it->first).find("SingleTop")!=string::npos ) {
      h1->SetFillColor(29);
      leg->AddEntry(h1,"single-t","F");
    }
    if( (it->first).find("Wjets")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"W+jets","F");
    }
    if( (it->first).find("EWK")!=string::npos ) {
      h1->SetFillColor(kGreen);
      leg->AddEntry(h1,"EWK","F");
    }
    if( (it->first).find("DiBoson")!=string::npos ) {
      h1->SetFillColor(38);
      leg->AddEntry(h1,"Di-Boson","F");
    }
    if( (it->first).find("QCD")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"QCD-multijets","F");
    }
    if( (it->first).find("SS")!=string::npos ) {
      h1->SetFillColor(42);
      leg->AddEntry(h1,"SS \\ W","F");
    }
    if((it->first).find("qqH115")!=string::npos){
      hSgn1 = (TH1F*)h1->Clone("hSgn1");
      hSgn1->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
      hSgn1->SetLineWidth(2);
      h1->SetFillColor(kBlack);
      h1->SetFillStyle(3004);
      h1->SetLineColor(kBlack);
      leg->AddEntry(h1,Form("VBF H(115) X %.0f",magnifySgn_),"F");
    }
    if((it->first).find("ggH115")!=string::npos){
      hSgn2 = (TH1F*)h1->Clone("hSgn2");
      hSgn2->Scale(magnifySgn_);
      h1->Scale(magnifySgn_);
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
      hData->SetXTitle(XTitle_+" ("+Unities_+")");
      hData->SetYTitle(Form(" Events/(%.0f %s)", hData->GetBinWidth(1), Unities_.Data() ) );
      hData->SetTitleSize(0.04,"X");
      hData->SetTitleSize(0.05,"Y");
      hData->SetTitleOffset(0.95,"Y");
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

  c1->SaveAs("tmp.png");

}

















///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

/*
void fakeStudyMu( TCut Cuts_ = "ptL1>0",
		  TString variable_ = "MtLeg1",
		  TString XTitle_ = "M_{T}(#mu,MET)",
		  TString Unities_ = "GeV",
		  Int_t nBins_ = 12, Float_t xMin_=0, Float_t xMax_=120,
		  Float_t magnifySgn_ = 10,
		  Float_t hltEff_ = 1.0,
		  Int_t enableHLTmatching_ = 1,
		  Int_t logy_ = 0 ) 
{   
  
  float Lumi = 24.86+159.15;

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

  THStack* aStack = new THStack("aStack",Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi ));
  TH1F* hSiml = new TH1F();
  TH1F* hSgn   = new TH1F();
  TH1F* hSgn1  = new TH1F();
  TH1F* hSgn2  = new TH1F();
  TH1F* hData  = new TH1F();
 

  // OpenNTuples
  TString fDataName                = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleRun2011-Mu_Open_MuTauStream.root";
  TString fSignalNameVBF           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fSignalNameGGH           = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleGGFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYTauTau  = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYMuMu    = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDYJets-Mu-50-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleQCDmu_Open_MuTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameSingleTop = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleSingleTop-Mu_Open_MuTauStream.root";
  TString fBackgroundNameDiBoson   = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011_iter2/Inclusive/nTupleDiBoson-Mu_Open_MuTauStream.root";

  TFile *fData(0); 
  TFile *fSignalVBF(0); 
  TFile *fSignalGGH(0); 
  TFile *fBackgroundDYTauTau(0);
  TFile *fBackgroundDYMuMu(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);
  TFile *fBackgroundSingleTop(0);
  TFile *fBackgroundDiBoson(0);
 
  fData               = TFile::Open( fDataName ); 
  fSignalVBF          = TFile::Open( fSignalNameVBF ); 
  fSignalGGH          = TFile::Open( fSignalNameGGH ); 
  fBackgroundDYTauTau = TFile::Open( fBackgroundNameDYTauTau ); 
  fBackgroundDYMuMu   = TFile::Open( fBackgroundNameDYMuMu ); 
  fBackgroundWJets    = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD      = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar    = TFile::Open( fBackgroundNameTTbar );
  fBackgroundSingleTop= TFile::Open( fBackgroundNameSingleTop ); 
  fBackgroundDiBoson  = TFile::Open( fBackgroundNameDiBoson ); 

  if(!fSignalVBF || !fBackgroundDYTauTau || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar || !fBackgroundSingleTop ||
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
  TTree *backgroundSingleTop = (TTree*)fBackgroundSingleTop->Get(tree);
  TTree *backgroundDiBoson   = (TTree*)fBackgroundDiBoson->Get(tree);
 
  // here I choose the order in the stack
  std::vector<string> samples;
  samples.push_back("ggH115");
  samples.push_back("qqH115");
  samples.push_back("DiBoson");
  samples.push_back("SingleTop");
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
  tMap["SingleTop"]=backgroundSingleTop;
  tMap["DiBoson"]=backgroundDiBoson;

  Float_t pt1, pt2, Deta, Mjj, eta1, eta2;
  Float_t diTauSVFitPt,diTauVisPt,diTauVisEta,diTauSVFitEta,diTauVisMass,diTauSVFitMass,ptL1,ptL2,etaL1,etaL2,diTauCharge,MtLeg1,numPV,combRelIsoLeg1DBeta,combRelIsoLeg1,pZetaCutVar, MEt, ptVeto, leadTrackPt, jetsBtagHE1, jetsBtagHE2;
  Float_t sampleWeight,puWeight;

  std::map<TString,Float_t> vMap;

  for( unsigned iter=0; iter<samples.size(); iter++){

    std::map<std::string,TTree*>::iterator it = tMap.find(samples[iter]);

    TString h1Name = "h1_"+it->first;
    TH1F* h1 = new TH1F( h1Name ,Form("CMS Preliminary 2011    #sqrt{s}=7 TeV L=%.0f pb^{-1}", Lumi) , nBins_ ,xMin_ , xMax_);

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TCut Cuts = Cuts_;
    TCut hlt = enableHLTmatching_ ? "( ((HLTmatch==2 && run<=163261) || (HLTmatch==1 && run>163261)) && ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )" : "( ((HLTmu==1 && run<=163261) || (HLTx==1 && run>163261)) )";

    if((it->first).find("Data")!=string::npos) Cuts = Cuts_ && hlt;
    TTree* currentTree = (TTree*)(it->second)->CopyTree(Cuts);
    Int_t counter = 0;

    currentTree->SetBranchAddress( "pt1", &pt1 );
    currentTree->SetBranchAddress( "pt2", &pt2 );
    currentTree->SetBranchAddress( "Deta",&Deta );
    currentTree->SetBranchAddress( "Mjj", &Mjj );
    currentTree->SetBranchAddress( "diTauSVFitPt",&diTauSVFitPt);
    currentTree->SetBranchAddress( "diTauSVFitEta",&diTauSVFitEta);
    currentTree->SetBranchAddress( "diTauSVFitMass",&diTauSVFitMass);
    currentTree->SetBranchAddress( "diTauVisPt",&diTauVisPt);
    currentTree->SetBranchAddress( "diTauVisEta",&diTauVisEta);
    currentTree->SetBranchAddress( "diTauVisMass",&diTauVisMass);
    currentTree->SetBranchAddress( "ptL1",  &ptL1 );
    currentTree->SetBranchAddress( "ptL2",  &ptL2 );
    currentTree->SetBranchAddress( "etaL1", &etaL1 );
    currentTree->SetBranchAddress( "etaL2", &etaL2 );
    currentTree->SetBranchAddress( "combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta);
    currentTree->SetBranchAddress( "combRelIsoLeg1",&combRelIsoLeg1);
    currentTree->SetBranchAddress( "diTauCharge",&diTauCharge);
    currentTree->SetBranchAddress( "MtLeg1",&MtLeg1);
    currentTree->SetBranchAddress( "pZetaCutVar",&pZetaCutVar);
    currentTree->SetBranchAddress( "numPV",&numPV);
    currentTree->SetBranchAddress( "sampleWeight",&sampleWeight);
    currentTree->SetBranchAddress( "puWeight",&puWeight);
    currentTree->SetBranchAddress( "ptVeto",&ptVeto);
    currentTree->SetBranchAddress( "MEt",&MEt);
    currentTree->SetBranchAddress( "leadTrackPt",&leadTrackPt);
    currentTree->SetBranchAddress( "jetsBtagHE1",&jetsBtagHE1);
    currentTree->SetBranchAddress( "jetsBtagHE2",&jetsBtagHE2);

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {

      currentTree->GetEntry(ievt);

      if (ievt%10000 == 0){
	std::cout << (it->first) << " ---> processing event: " << ievt << " ..." <<std::endl;
      }

     
      vMap["diTauSVFitPt"]  = diTauSVFitPt;
      vMap["diTauSVFitEta"] = diTauSVFitEta;
      vMap["diTauSVFitMass"]= diTauSVFitMass;
      vMap["diTauVisPt"]    = diTauVisPt;
      vMap["diTauVisEta"]   = diTauVisEta;
      vMap["diTauVisMass"]  = diTauVisMass;
      vMap["ptL1"]  = ptL1;
      vMap["ptL2"]  = ptL2;
      vMap["etaL1"] = etaL1;
      vMap["etaL2"] = etaL2;
      vMap["diTauCharge"]= Float_t(diTauCharge);
      vMap["MtLeg1"]= MtLeg1;
      vMap["pZetaCutVar"] = pZetaCutVar;
      vMap["numPV"]= numPV;
      vMap["combRelIsoLeg1"]= combRelIsoLeg1;
      vMap["combRelIsoLeg1DBeta"]= combRelIsoLeg1DBeta;
      vMap["sampleWeight"]= sampleWeight;
      vMap["puWeight"]= puWeight;
      vMap["puWeight"]= puWeight;
      vMap["jetsBtagHE1"]= jetsBtagHE1;
      vMap["jetsBtagHE2"]= jetsBtagHE2;
      vMap["leadTrackPt"]= leadTrackPt;

      vMap["pt1"]= pt1;
      vMap["pt2"]= pt2;
      vMap["Deta"]= Deta;
      vMap["Mjj"]= Mjj;
      vMap["ptVeto"]= ptVeto;

      if((it->first).find("Data")==string::npos) sampleWeight*=(Lumi/1000*hltEff_*puWeight);

      counter++;
      h1->Fill( vMap[variable_], sampleWeight);
    
    }// end loop


    if( (it->first).find("ZfakeTau")!=string::npos ) {
      h1->SetFillColor(7);
      leg->AddEntry(h1,"Z+jets, fake-#tau","F");
    }
    if( (it->first).find("Ztautau")!=string::npos ) {
      h1->SetFillColor(kRed);
      leg->AddEntry(h1,"Z+jets, genuine-#tau","F");
    }
    if( (it->first).find("TTbar")!=string::npos ) {
      h1->SetFillColor(kBlue);
      leg->AddEntry(h1,"t#bar{t}+jets","F");
    }
    if( (it->first).find("SingleTop")!=string::npos ) {
      h1->SetFillColor(29);
      leg->AddEntry(h1,"single-t","F");
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
      hData->SetXTitle(XTitle_+" ("+Unities_+")");
      hData->SetYTitle(Form(" Events/(%.0f %s)", hData->GetBinWidth(1), Unities_.Data() ) );
      hData->SetTitleSize(0.04,"X");
      hData->SetTitleSize(0.05,"Y");
      hData->SetTitleOffset(0.95,"Y");
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
*/
