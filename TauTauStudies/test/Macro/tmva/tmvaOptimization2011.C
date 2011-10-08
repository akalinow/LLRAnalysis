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


void TMVAClassificationElecTau(std::string ordering_ = "Pt", std::string bkg_ = "qqH115vsWZttQCD"){

  TMVA::Tools::Instance();

  TString outfileName( "TMVAElecTau"+ordering_+"Ord_"+bkg_+".root" );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassificationElecTau"+ordering_+"Ord_"+bkg_, outputFile, 
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );
  factory->AddVariable( "pt1", "pT-tag1", "GeV/c"         , 'F'  );
  factory->AddVariable( "pt2", "pT-tag2", "GeV/c"         , 'F'  );
  factory->AddVariable( "Deta","|y-tag1 - y-tag2|",""     , 'F'  );
  //factory->AddVariable( "opposite:=abs(eta1*eta2)/eta1/eta2","sign1*sign2",""             , 'F'  );
  //factory->AddVariable( "Dphi", "#Delta#phi" ,""             , 'F'  );
  factory->AddVariable( "Mjj", "M(tag1,tag2)", "GeV/c^{2}"  , 'F'  );

  factory->AddSpectator( "eta1",  "#eta_{tag1}" , 'F' );
  factory->AddSpectator( "eta2",  "#eta_{tag2}" , 'F' );

  factory->SetWeightExpression( "sampleWeight" );

  TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/nTupleVBFH115-powheg-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/nTupleZjets-alpgen-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/nTupleWJets-madgraph-PUS1_Open_ElecTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/nTupleQCD_Open_ElecTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/ElecTauStream2011/nTupleTTJets-madgraph-PUS1_Open_ElecTauStream.root";


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

  TCut mycuts = ""; 
  TCut mycutb = "";
 
  TCut cutA  = "pt1>0 && tightestHPSWP>0";
  TCut cutB  = "pt1>0 && combRelIsoLeg1<0.1";
  TCut cutBl = "pt1>0 && combRelIsoLeg1<0.3";
  TCut cutC  = "pt1>0 && diTauCharge==0";
  TCut cutD  = "pt1>0 && MtLeg1<40";

  // select events for training
  TFile* dummy = new TFile("dummy.root","RECREATE");
  TH1F* allEvents = new TH1F("allEvents","",1,-10,10);
  float totalEvents, cutEvents;

  // signal: all
  TTree *signal           = ((TTree*)(fSignal->Get(tree)))->CopyTree(cutA&&cutB&&cutC&&cutD);
  cout << "Copied signal tree with full selection: " << ((TTree*)(fSignal->Get(tree)))->GetEntries() << " --> "  << signal->GetEntries()  << endl;
  allEvents->Reset();
  signal->Draw("eta1>>allEvents","sampleWeight");
  cutEvents  = allEvents->Integral();
  Double_t signalWeight =   1.0;
  cout << "Signal: expected yield " << cutEvents << " -- weight " << signalWeight << endl;

  // Z+jets: all
  TTree *backgroundDYJets = ((TTree*)(fBackgroundDYJets->Get(tree)))->CopyTree(cutA&&cutB&&cutC&&cutD);
  cout << "Copied DYJets tree with full selection: " << ((TTree*)(fBackgroundDYJets->Get(tree)))->GetEntries() << " --> "  << backgroundDYJets->GetEntries()  << endl;
  allEvents->Reset();
  backgroundDYJets->Draw("eta1>>allEvents","sampleWeight");
  cutEvents  = allEvents->Integral();
  Double_t backgroundDYJetsWeight = 1.0;
  cout << "ZJets: expected yield " << cutEvents << " -- weight " << backgroundDYJetsWeight << endl;

  // W+jets: iso+Mt
  TTree *backgroundWJets  = ((TTree*)(fBackgroundWJets->Get(tree)))->CopyTree(cutB&&cutD);
  cout << "Copied WJets tree with iso+Mt selection: " << ((TTree*)(fBackgroundWJets->Get(tree)))->GetEntries() << " --> "  << backgroundWJets->GetEntries()  << endl;
  allEvents->Reset();
  backgroundWJets->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundWJets->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundWJetsWeight  =  cutEvents / totalEvents;
  cout << "WJets: expected yield " << cutEvents  << " -- weight " << backgroundWJetsWeight << endl;

  // QCD: Mt+loose iso
  TTree *backgroundQCD    = ((TTree*)(fBackgroundQCD->Get(tree)))->CopyTree(cutD&&cutBl);
  cout << "Copied QCD tree with Mt selection: " << ((TTree*)(fBackgroundQCD->Get(tree)))->GetEntries() << " --> "  << backgroundQCD->GetEntries()  << endl;
  allEvents->Reset();
  backgroundQCD->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundQCD->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0 && combRelIsoLeg1<0.1)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundQCDWeight  =  cutEvents / totalEvents;
  cout << "QCD: expected yield " << cutEvents  << " -- weight "  << backgroundQCDWeight << endl;


  // TTbar: iso+Mt
  TTree *backgroundTTbar  = ((TTree*)(fBackgroundTTbar->Get(tree)))->CopyTree(cutB&&cutD);
  cout << "Copied TTbar tree with iso+Mt selection: " << ((TTree*)(fBackgroundTTbar->Get(tree)))->GetEntries() << " --> "  << backgroundTTbar->GetEntries()  << endl;
  allEvents->Reset();
  backgroundTTbar->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundTTbar->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundTTbarWeight  =  cutEvents / totalEvents;
  cout << "TTbar: expected yield "  << cutEvents  << " -- weight " << backgroundTTbarWeight << endl;

 
  delete allEvents;


  factory->AddSignalTree    ( signal,           signalWeight           );
  //factory->AddBackgroundTree( backgroundDYJets, backgroundDYJetsWeight );
  //factory->AddBackgroundTree( backgroundWJets,  backgroundWJetsWeight  );
  factory->AddBackgroundTree( backgroundQCD,    backgroundQCDWeight    );
  //factory->AddBackgroundTree( backgroundTTbar,  backgroundTTbarWeight  );


  factory->PrepareTrainingAndTestTree( mycuts, mycutb,
				       "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=1:nTest_Background=1:SplitMode=Random:NormMode=NumEvents:!V" );
  
  factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
		       "!H:!V:FitMethod=GA:EffSel:CutRangeMin[0]=25.:CutRangeMax[0]=999:CutRangeMin[1]=25.:CutRangeMax[1]=999.:CutRangeMin[2]=1.0:CutRangeMax[2]=9.:CutRangeMin[3]=100:CutRangeMax[3]=7000:VarProp=FSmart" );
  
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



void TMVAClassificationMuTau(std::string ordering_ = "Pt", std::string bkg_ = "qqH115vsWZttQCD"){

  TMVA::Tools::Instance();

  TString outfileName( "TMVAMuTau"+ordering_+"Ord_"+bkg_+".root" );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassificationMuTau"+ordering_+"Ord_"+bkg_, outputFile, 
					      "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );
  factory->AddVariable( "pt1", "pT-tag1", "GeV/c"         , 'F'  );
  factory->AddVariable( "pt2", "pT-tag2", "GeV/c"         , 'F'  );
  factory->AddVariable( "Deta","|y-tag1 - y-tag2|",""     , 'F'  );
  //factory->AddVariable( "opposite:=abs(eta1*eta2)/eta1/eta2","sign1*sign2",""             , 'F'  );
  //factory->AddVariable( "Dphi", "#Delta#phi" ,""             , 'F'  );
  factory->AddVariable( "Mjj", "M(tag1,tag2)", "GeV/c^{2}"  , 'F'  );

  factory->AddSpectator( "eta1",  "#eta_{tag1}" , 'F' );
  factory->AddSpectator( "eta2",  "#eta_{tag2}" , 'F' );

  factory->SetWeightExpression( "sampleWeight" );

  TString fSignalName              = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/nTupleVBFH115-Mu-powheg-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameDYJets    = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/nTupleZjets-Mu-alpgen-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameWJets     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/nTupleWJets-Mu-madgraph-PUS1_Open_MuTauStream.root";
  TString fBackgroundNameQCD       = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/nTupleQCDmu_Open_MuTauStream.root";
  TString fBackgroundNameTTbar     = "/data_CMS/cms/lbianchini/VbfJetsStudy/OpenNtuples/MuTauStream2011/nTupleTTJets-Mu-madgraph-PUS1_Open_MuTauStream.root";


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

  TCut mycuts = ""; 
  TCut mycutb = "";
 
  TCut cutA  = "pt1>0 && tightestHPSWP>0";
  TCut cutB  = "pt1>0 && combRelIsoLeg1<0.1";
  TCut cutBl = "pt1>0 && combRelIsoLeg1<0.3";
  TCut cutC  = "pt1>0 && diTauCharge==0";
  TCut cutD  = "pt1>0 && MtLeg1<40";

  // select events for training
  TFile* dummy = new TFile("dummy.root","RECREATE");
  TH1F* allEvents = new TH1F("allEvents","",1,-10,10);
  float totalEvents, cutEvents;

  // signal: all
  TTree *signal           = ((TTree*)(fSignal->Get(tree)))->CopyTree(cutA&&cutB&&cutC&&cutD);
  cout << "Copied signal tree with full selection: " << ((TTree*)(fSignal->Get(tree)))->GetEntries() << " --> "  << signal->GetEntries()  << endl;
  allEvents->Reset();
  signal->Draw("eta1>>allEvents","sampleWeight");
  cutEvents  = allEvents->Integral();
  Double_t signalWeight =   1.0;
  cout << "Signal: expected yield " << cutEvents << " -- weight " << signalWeight << endl;

  // Z+jets: all
  TTree *backgroundDYJets = ((TTree*)(fBackgroundDYJets->Get(tree)))->CopyTree(cutA&&cutB&&cutC&&cutD);
  cout << "Copied DYJets tree with full selection: " << ((TTree*)(fBackgroundDYJets->Get(tree)))->GetEntries() << " --> "  << backgroundDYJets->GetEntries()  << endl;
  allEvents->Reset();
  backgroundDYJets->Draw("eta1>>allEvents","sampleWeight");
  cutEvents  = allEvents->Integral();
  Double_t backgroundDYJetsWeight = 1.0;
  // 17.1 = correction factor to account for BR bug in Alpgen sample 
  backgroundDYJetsWeight*=17.1;
  cutEvents*=17.1;
  cout << "ZJets: expected yield " << cutEvents << " -- weight " << backgroundDYJetsWeight << endl;

  // W+jets: iso+Mt
  TTree *backgroundWJets  = ((TTree*)(fBackgroundWJets->Get(tree)))->CopyTree(cutB&&cutD);
  cout << "Copied WJets tree with iso+Mt selection: " << ((TTree*)(fBackgroundWJets->Get(tree)))->GetEntries() << " --> "  << backgroundWJets->GetEntries()  << endl;
  allEvents->Reset();
  backgroundWJets->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundWJets->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundWJetsWeight  =  cutEvents / totalEvents;
  cout << "WJets: expected yield " << cutEvents  << " -- weight " << backgroundWJetsWeight << endl;

  // QCD: Mt+loose iso
  TTree *backgroundQCD    = ((TTree*)(fBackgroundQCD->Get(tree)))->CopyTree(cutD&&cutBl);
  cout << "Copied QCD tree with Mt selection: " << ((TTree*)(fBackgroundQCD->Get(tree)))->GetEntries() << " --> "  << backgroundQCD->GetEntries()  << endl;
  allEvents->Reset();
  backgroundQCD->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundQCD->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0 && combRelIsoLeg1<0.1)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundQCDWeight  =  cutEvents / totalEvents;
  cout << "QCD: expected yield " << cutEvents  << " -- weight "  << backgroundQCDWeight << endl;


  // TTbar: iso+Mt
  TTree *backgroundTTbar  = ((TTree*)(fBackgroundTTbar->Get(tree)))->CopyTree(cutB&&cutD);
  cout << "Copied TTbar tree with iso+Mt selection: " << ((TTree*)(fBackgroundTTbar->Get(tree)))->GetEntries() << " --> "  << backgroundTTbar->GetEntries()  << endl;
  allEvents->Reset();
  backgroundTTbar->Draw("eta1>>allEvents","sampleWeight");
  totalEvents  = allEvents->Integral();
  allEvents->Reset();
  backgroundTTbar->Draw("eta1>>allEvents","sampleWeight*(tightestHPSWP>0 && diTauCharge==0)");
  cutEvents  = allEvents->Integral();
  Double_t backgroundTTbarWeight  =  cutEvents / totalEvents;
  cout << "TTbar: expected yield "  << cutEvents  << " -- weight " << backgroundTTbarWeight << endl;

 
  delete allEvents;


  factory->AddSignalTree( signal,signalWeight);
  if(bkg_.find("Z")!=string::npos)       factory->AddBackgroundTree( backgroundDYJets, backgroundDYJetsWeight );
  if(bkg_.find("W")!=string::npos)       factory->AddBackgroundTree( backgroundWJets,  backgroundWJetsWeight  );
  if(bkg_.find("QCD")!=string::npos)     factory->AddBackgroundTree( backgroundQCD,    backgroundQCDWeight    );
  if(bkg_.find("TTbar")!=string::npos)   factory->AddBackgroundTree( backgroundTTbar,  backgroundTTbarWeight  );


  factory->PrepareTrainingAndTestTree( mycuts, mycutb,
				       "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=1:nTest_Background=1:SplitMode=Random:NormMode=NumEvents:!V" );
  
  factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
		       "!H:!V:FitMethod=GA:EffSel:CutRangeMin[0]=25.:CutRangeMax[0]=999:CutRangeMin[1]=25.:CutRangeMax[1]=999.:CutRangeMin[2]=1.0:CutRangeMax[2]=9.:CutRangeMin[3]=100:CutRangeMax[3]=7000:VarProp=FSmart" );
  
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



