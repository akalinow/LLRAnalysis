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

//#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#endif


void TMVAClassification(std::string ordering_ = "Pt", std::string bkg_ = "qqH115vsWZttQCD"){

  TMVA::Tools::Instance();

  TString outfileName( "TMVA"+ordering_+"Ord_"+bkg_+".root" );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification"+ordering_+"Ord_"+bkg_, outputFile, 
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );
  factory->AddVariable( "pt1", "pT-tag1", "GeV/c"         , 'F'  );
  factory->AddVariable( "pt2", "pT-tag2", "GeV/c"         , 'F'  );
  factory->AddVariable( "Deta","|y-tag1 - y-tag2|",""     , 'F'  );
  //factory->AddVariable( "opposite:=abs(eta1*eta2)/eta1/eta2","sign1*sign2",""             , 'F'  );
  //factory->AddVariable( "Dphi", "#Delta#phi" ,""             , 'F'  );
  factory->AddVariable( "Mjj", "M(tag1,tag2)", "GeV/c^{2}"  , 'F'  );

  factory->AddSpectator( "eta1",  "#eta_{tag1}" , 'F' );
  factory->AddSpectator( "eta2",  "#eta_{tag2}" , 'F' );

  // mu+tau iso, OS, Mt<40
  TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleVBFH115-PU-L.root";
  //TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleVbf.root";
  // mu+tau iso, OS, Mt<40
  //TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleDYJets-madgraph-50-PU-L.root";
  TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets.root";
  // Mt<40
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleWJets-madgraph-PU-L.root";
  // Mt<40
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleQCD-pythia-PU-L.root";
  // Mt<40
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleTT-madgraph-PU-L.root";


  TFile *fSignal(0); 
  TFile *fBackgroundDYJets(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);

  fSignal           = TFile::Open( fSignalName ); 
  fBackgroundDYJets = TFile::Open( fBackgroundNameDYJets ); 
  fBackgroundWJets  = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD    = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar  = TFile::Open( fBackgroundNameTTbar ); 

  if(!fSignal || !fBackgroundDYJets || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTree"+ordering_+"Ord";

  TTree *signal           = (TTree*)fSignal->Get(tree);
  TTree *backgroundDYJets = (TTree*)fBackgroundDYJets->Get(tree);
  TTree *backgroundWJets  = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD    = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundTTbar  = (TTree*)fBackgroundTTbar->Get(tree);

  TCut mycuts = "pt1>0 && abs(eta1*eta2)/eta1/eta2<0"; 
  //TCut mycuts = ""; 
  TCut mycutb = "pt1>0 && abs(eta1*eta2)/eta1/eta2<0"; 
  //TCut mycutb = ""; 

  ////////////////// compute the weights

  std::map<string, float>  weights;

  std::vector<string> samples;
  samples.push_back( "DYJets-madgraph-50-PU-L");
  samples.push_back( "QCD-pythia-PU-L" );
  samples.push_back( "TT-madgraph-PU-L" );
  samples.push_back( "WJets-madgraph-PU-L" );
  samples.push_back( "VBFH115-PU-L" );
  samples.push_back( "VBFH135-PU-L" );

  std::vector<float> crossSec;
  crossSec.push_back( 3048);
  crossSec.push_back( 349988);
  crossSec.push_back( 157.5);
  crossSec.push_back( 31314.0);
  crossSec.push_back( 0.1012);
  crossSec.push_back( 0.05049);

  TCut cutA = "tightestHPSWP>0";
  TCut cutB = "(chIsoLeg1+nhIsoLeg1+phIsoLeg1)/diTauLegsP4[0].Pt()<0.1";
  TCut cutC = "diTauCharge==0";
  TCut cutD = "MtLeg1<40";
  
  for(int i = 0; i < samples.size() ; i++){
    TFile* f = new TFile(("/data_CMS/cms/lbianchini/MuTauStream/NoMuIsoNoTauIsoNo2Tcuts/treeMuTauStream_"+samples[i]+".root").c_str());    
    TTree* tree = (TTree*)f->Get("muTauStreamAnalyzer/tree");
    TH1F* allEvents = (TH1F*)f->Get("allEventsFilter/totalEvents");
    float totalEvents = allEvents->GetBinContent(1);
    float scaleFactor = ((crossSec[i])) / totalEvents ;
    float total = tree->GetEntries();
    float cut   = tree-> GetEntries(cutA&&cutB&&cutC&&cutD);
    weights[samples[i]] = cut*scaleFactor;
    //cout << samples[i] << ": total proc. " << totalEvents << " # 1 fb:  before VBF = " << cut*scaleFactor << "/" << total*scaleFactor << " = " << cut/total << endl;
    //cout << samples[i] << ": after  VBF  ---> " << cutTot << "/" << total << " = " << cutTot/total << endl;
  }

  ///////////////////
  
  Double_t signalWeight           =   1.0;
  Double_t backgroundDYJetsWeight =   weights["DYJets-madgraph-50-PU-L"]*backgroundDYJets->GetEntries(mycutb)/backgroundDYJets->GetEntries() ;
  Double_t backgroundWJetsWeight  =   weights["WJets-madgraph-PU-L"]*backgroundWJets->GetEntries(mycutb)/backgroundWJets->GetEntries();
  Double_t backgroundQCDWeight    =   weights["QCD-pythia-PU-L"]*backgroundQCD->GetEntries(mycutb)/backgroundQCD->GetEntries();
  Double_t backgroundTTbarWeight  =   weights["TT-madgraph-PU-L"]*backgroundTTbar->GetEntries(mycutb)/backgroundTTbar->GetEntries();
  
  cout << "ZJets " << backgroundDYJetsWeight << endl;
  cout << "WJets " << backgroundWJetsWeight << endl;
  cout << "QCDmu " << backgroundQCDWeight << endl;
  cout << "TTbar " << backgroundTTbarWeight << endl;


  factory->AddSignalTree    ( signal,          signalWeight           );
  factory->AddBackgroundTree( backgroundDYJets,1.0/*backgroundDYJetsWeight*/ );
  //factory->AddBackgroundTree( backgroundWJets, backgroundWJetsWeight  );
  //factory->AddBackgroundTree( backgroundQCD,   backgroundQCDWeight    );
  //factory->AddBackgroundTree( backgroundTTbar, backgroundTTbarWeight  );


  factory->PrepareTrainingAndTestTree( mycuts, mycutb,
				       "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=1:nTest_Background=1:SplitMode=Random:NormMode=NumEvents:!V" );
  
  factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
		       "!H:!V:FitMethod=GA:EffSel:CutRangeMin[0]=20.:CutRangeMax[0]=999:CutRangeMin[1]=15.:CutRangeMax[1]=999.:CutRangeMin[2]=1.0:CutRangeMax[2]=9.:CutRangeMin[3]=100:CutRangeMax[3]=7000:VarProp=FSmart" );
  
  /*
  factory->BookMethod( TMVA::Types::kBDT, "BDT", 
		       "!H:!V:NTrees=200:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
  */

  factory->TrainAllMethods();
  
  factory->TestAllMethods();

  factory->EvaluateAllMethods();  

  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVAClassification is done!" << std::endl;      
  
  delete factory;

  //if (!gROOT->IsBatch()) TMVAGui( outfileName );

}




void TMVAClassificationApplication( TString weightFile = "TMVAClassificationPtOrd_qqH115vsWZttQCD_Cuts.weights.xml",
				    Double_t effS_ = 0.5 ) 
{   

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

  reader->BookMVA( "Cuts", TString("weights/")+weightFile ); 



  // mu+tau iso, OS, Mt<40
  TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleVBFH115-PU-L.root";
  //TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleVbf.root";
  // mu+tau iso, OS, Mt<40
  //TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleDYJets-madgraph-50-PU-L.root";
  TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/nTupleZJets.root";
  // Mt<40
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleWJets-madgraph-PU-L.root";
  // Mt<40
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleQCD-pythia-PU-L.root";
  // Mt<40
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/looseSelection/nTupleTT-madgraph-PU-L.root";


  TFile *fSignal(0); 
  TFile *fBackgroundDYJets(0);
  TFile *fBackgroundWJets(0);
  TFile *fBackgroundQCD(0);
  TFile *fBackgroundTTbar(0);

  fSignal           = TFile::Open( fSignalName ); 
  fBackgroundDYJets = TFile::Open( fBackgroundNameDYJets ); 
  fBackgroundWJets  = TFile::Open( fBackgroundNameWJets ); 
  fBackgroundQCD    = TFile::Open( fBackgroundNameQCD ); 
  fBackgroundTTbar  = TFile::Open( fBackgroundNameTTbar ); 

  if(!fSignal || !fBackgroundDYJets || !fBackgroundWJets || !fBackgroundQCD || !fBackgroundTTbar){
    std::cout << "ERROR: could not open files" << std::endl;
    exit(1);
  }

  TString tree = "outTreePtOrd";

  TTree *signal           = (TTree*)fSignal->Get(tree);
  TTree *backgroundDYJets = (TTree*)fBackgroundDYJets->Get(tree);
  TTree *backgroundWJets  = (TTree*)fBackgroundWJets->Get(tree);
  TTree *backgroundQCD    = (TTree*)fBackgroundQCD->Get(tree);
  TTree *backgroundTTbar  = (TTree*)fBackgroundTTbar->Get(tree);

  TCut mycuts = "pt1>0 && abs(eta1*eta2)/eta1/eta2<0"; 
  TCut mycutb = "pt1>0 && abs(eta1*eta2)/eta1/eta2<0"; 


  std::map<std::string,TTree*> tMap;
  tMap["qqH115"]=signal;
  tMap["Zjets"]=backgroundDYJets;
  tMap["Wjets"]=backgroundWJets;
  tMap["QCD"]=backgroundQCD;
  tMap["TTbar"]=backgroundTTbar;

  Double_t pt1_, pt2_;
  Double_t Deta_, Mjj_;

  for(std::map<std::string,TTree*>::iterator it = tMap.begin(); it != tMap.end(); it++){

    TFile* dummy = new TFile("dummy.root","RECREATE");  
    TTree* currentTree = (TTree*)(it->second)->CopyTree(mycuts);

    Int_t counter = 0;

    currentTree->SetBranchAddress( "pt1", &pt1_ );
    currentTree->SetBranchAddress( "pt2", &pt2_ );
    currentTree->SetBranchAddress( "Deta",&Deta_ );
    currentTree->SetBranchAddress( "Mjj", &Mjj_ );

    for (Long64_t ievt=0; ievt<currentTree->GetEntries();ievt++) {
      currentTree->GetEntry(ievt);
      pt1  = pt1_;
      pt2  = pt2_;
      Deta = Deta_;
      Mjj  = Mjj_;
      if (ievt%1000000 == 0){
	std::cout << "--- ... Processing event: " << ievt << std::endl;
	//cout << pt1 << ", " << pt2 << ", " << Deta << ", " << Mjj << endl;
      }
      if(reader->EvaluateMVA( "Cuts", effS_ )) counter++;
    }

    cout<< "Sample " << it->first << " has a rejection factor " << (Float_t)counter/(Float_t)currentTree->GetEntries() << "+/-" <<
      TMath::Sqrt((Float_t)counter/currentTree->GetEntries()*(1-counter/currentTree->GetEntries())/currentTree->GetEntries()) 
	<< " for effS="<< effS_ << endl;
  }

  delete reader;
  
  std::cout << "==> TMVAClassificationApplication is done!" << endl << std::endl;

}



