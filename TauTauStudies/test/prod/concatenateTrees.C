#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TChain.h"
#include "TList.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TObject.h"
#include "THStack.h"
#include "TLegend.h"
#include "TBranch.h"
#include <vector>
#include <string>
#include <utility>
#include <iostream>
//#include <sstream> 
//#include <strstream> 
#include <algorithm>

using namespace std;


void makeElecTauStreamTree(string sample = "Zjets", float Lumi = 1000){

  vector<pair<string,float> > files;
  

  vector<string> directories;
  
  if(sample.find("Zjets")!=string::npos){
    files.push_back( make_pair("Z2Jets-ptZ-0to100-alpgen-PUS1.root",        (1.035e+02 *Lumi)  ) );
    files.push_back( make_pair("Z2Jets-ptZ-100to300-alpgen-PUS1.root",      (8.534e+00 	*Lumi)  ) );
    files.push_back( make_pair("Z2Jets-ptZ-300to800-alpgen-PUS1.root",      (1.151e-01*Lumi)  ) );
    files.push_back( make_pair("Z2Jets-ptZ-800to1600-alpgen-PUS1.root",     (3.023e-04 *Lumi)  ) );
    files.push_back( make_pair("Z3Jets-ptZ-0to100-alpgen-PUS1.root",        (2.289e+01 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-ptZ-100to300-alpgen-PUS1.root",      (3.951e+00 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-ptZ-300to800-alpgen-PUS1.root",      (8.344e-02 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-ptZ-800to1600-alpgen-PUS1.root",     (2.480e-04 *Lumi)  ) );
  }

  if(sample.find("DiBoson")!=string::npos){
    files.push_back( make_pair("WW-pythia-PUS1.root",        ( 4.4   *Lumi)  ) );
    files.push_back( make_pair("WZ-pythia-PUS1.root",        (18.2   *Lumi)  ) );
    files.push_back( make_pair("ZZ-pythia-PUS1.root",        ( 5.9   *Lumi)  ) );
  }

  if(sample.find("SingleTop")!=string::npos){
    files.push_back( make_pair("TToBLNu-s-PUS1.root",        ( 1.4   *Lumi)  ) ); 
    files.push_back( make_pair("TToBLNu-t-PUS1.root",        ( 20.9   *Lumi)  ) ); 
    files.push_back( make_pair("TToBLNu-tW-PUS1.root",       ( 10.6  *Lumi)  ) );
  }
  
  if(sample.find("Wjets")!=string::npos){
    files.push_back( make_pair("W1Jets_ptW-0to100.root",       (3.693e+03*0.44*Lumi)  ) );
    files.push_back( make_pair("W1Jets_ptW-100to300.root",     (7.197e+01*0.58*Lumi)  ) );
    files.push_back( make_pair("W1Jets_ptW-300to800.root",     (5.658e-01*0.73*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-0to100.root",       (9.434e+02*0.46*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-100to300.root",     (6.718e+01*0.59*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-300to800.root",     (8.553e-01*0.75*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-0to100.root",       (2.087e+02*0.48*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-100to300.root",     (3.243e+01*0.60*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-300to800.root",     (6.229e-01*0.76*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-0to100.root",       (4.446e+01*0.50*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-100to300.root",     (1.138e+01*0.61*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-300to800.root",     (2.950e-01*0.77*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-0to100.root",       (1.111e+01*0.53*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-100to300.root",     (3.789e+00*0.65*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-300to800.root",     (1.565e-01*0.79*Lumi)  ) );
  }

  if(sample.find("QCD")!=string::npos){
    files.push_back( make_pair("QCDbc-pythia-2030-PUS1.root",             (236000000*0.00056*Lumi)) );
    files.push_back( make_pair("QCDbc-pythia-3080-PUS1.root",             (59480000*0.00230 *Lumi)) );
    files.push_back( make_pair("QCDbc-pythia-80170-PUS1.root",            (900000*0.0104 *Lumi)) );
    files.push_back( make_pair("QCDem-pythia-2030-PUS1.root",             (236000000*0.0104*Lumi)) );
    files.push_back( make_pair("QCDem-pythia-3080-PUS1-v2.root",          (59480000*0.065*Lumi)) );
    files.push_back( make_pair("QCDem-pythia-80170-PUS1.root",            (900000*0.155*Lumi)) );
  }
  
  if(sample.find("G1Jet")!=string::npos){
    files.push_back( make_pair("G1Jet-20to60-alpgen-PUS1.root",           (2.270e+04 *Lumi)) );
    files.push_back( make_pair("G1Jet-60to120-alpgen-PUS1.root",          (7.993e+02 *Lumi)) );
    files.push_back( make_pair("G1Jet-120to180-alpgen-PUS1.root",         (4.011e+01 *Lumi)) );
    files.push_back( make_pair("G1Jet-180to240-alpgen-PUS1.root",         (5.612e+00 *Lumi)) );
    files.push_back( make_pair("G1Jet-240to300-alpgen-PUS1.root",         (1.224e+00 *Lumi)) );
    files.push_back( make_pair("G1Jet-300to5000-alpgen-PUS1.root",        (5.450e-01 *Lumi)) );
  }


  directories.push_back("allEventsFilter");
  directories.push_back("vertexScrapingFilter");
  directories.push_back("atLeastOneElecTauFilter");
  directories.push_back("elecPtEtaFilter");
  directories.push_back("elecPtEtaIDFilter");
  directories.push_back("tauPtEtaFilter");
  directories.push_back("tauPtEtaIDFilter");
  directories.push_back("tauPtEtaIDAgMuFilter");
  directories.push_back("tauPtEtaIDAgMuAgElecFilter");
  //directories.push_back("atLeast1selectedDiTauFilter");
  

  TFile *outFile = new TFile(("treeElecTauStream_"+sample+".root").c_str(),"RECREATE");
  TChain* outChain = new TChain("elecTauStreamAnalyzer/tree");

  float fraction = 1.0;

  for(unsigned int i = 0; i<files.size();i++){

    TFile *currentFile = new TFile(("/data_CMS/cms/lbianchini/ElecTauStream2011_iter2/treeElecTauStream_"+files[i].first).c_str(),"READ");
    
    // protection
    if(currentFile->IsZombie()){
      cout << "No file " << files[i].first << " is found" << endl;
      continue;
    }

    currentFile->cd("allEventsFilter");
    TH1F* totalEvents = (TH1F*)gDirectory->Get("totalEvents");
    float readEvents = totalEvents->GetBinContent(1);
    float currentWeight = (files[i].second/readEvents);
    
    currentFile->cd("elecTauStreamAnalyzer");
    TTree* currentTree =  (TTree*)gDirectory->Get("tree");

    TFile *newFile = new TFile( ("treeElecTauStream_"+files[i].first+"new").c_str(), "RECREATE");
    TDirectory* dir = (TDirectory*) newFile->mkdir("elecTauStreamAnalyzer");
    int entries =  currentTree->GetEntries(); 
    TTree *tree = currentTree->CloneTree( (int)(fraction*entries) );
    float weight;
    TBranch *newBranch = tree->Branch("weight",&weight,"weight/F");
    int Entries = tree->GetEntries();
    for(int n=0; n< Entries ; n++ ){
      tree->GetEntry(n);
      if(n%5000==0) cout << "Processing event " << n << " with weight " << currentWeight/fraction << endl;
      weight=currentWeight/fraction;
      newBranch->Fill();
    }

    dir->cd();
    tree->Write();
    newFile->Write();

    outChain->AddFile( ("treeElecTauStream_"+files[i].first+"new").c_str() );
    cout << " outChain has " << outChain->GetEntries() 
	 << endl;

    currentFile->Close();
    newFile->Close();
    delete currentFile;
    delete newFile;

  }

  
  TDirectory* dir = (TDirectory*) outFile->mkdir("elecTauStreamAnalyzer");  
  TTree* outTree = (TTree*) outChain->CopyTree("");
    
  dir->cd();
  outTree->Write("", TObject::kOverwrite);



  for(unsigned int j = 0; j<directories.size(); j++){

    cout << "Directory " <<  directories[j] << endl;
    outFile->mkdir(directories[j].c_str());

    // CASE 1: a filter directory
    if( directories[j].find("Filter")!=string::npos){
      TH1F* filterHisto = new TH1F();

      // loop over the files
      for(unsigned int i = 0; i<files.size();i++){
	TFile *currentFile = new TFile(("/data_CMS/cms/lbianchini/ElecTauStream2011_iter2/treeElecTauStream_"+files[i].first).c_str(),"READ");

	// protection
	if(currentFile->IsZombie()){
	  cout << "No file " << files[i].first << " is found" << endl;
	  continue;
	}

	currentFile->cd("allEventsFilter");
	TH1F* totalEvents = (TH1F*)gDirectory->Get("totalEvents");
	float readEvents = totalEvents->GetBinContent(1);
	float currentWeight;
	currentWeight = (files[i].second/readEvents);
	currentWeight/=fraction;
	currentFile->cd(directories[j].c_str());
	TH1F* currentHisto =  (TH1F*)gDirectory->Get("totalEvents");
	if(i==0){
	  filterHisto = (TH1F*)currentHisto->Clone("totalEvents_v2");
	  filterHisto->Scale(currentWeight);
	  filterHisto->SetName("totalEvents");
	}
	else{
	  filterHisto->Add(currentHisto,currentWeight);
	}
	cout<<"Adding "<<currentHisto->GetEntries() << endl;
	cout<<"SubTot "<<filterHisto->GetEntries() << endl;
      } // file

      outFile->cd(directories[j].c_str());
      filterHisto->Write();  

    }
   

  }//directories

  
  outFile->Write();
  outFile->Close();
  delete outFile;

}


void makeMuTauStreamTree(string sample = "Zjets", float Lumi = 1000){

  vector<pair<string,float> > files;
  

  vector<string> directories;
  
  if(sample.find("Zjets")!=string::npos){
    files.push_back( make_pair("Z2Jets-Mu-ptZ-0to100-alpgen-PUS1.root",        (1.035e+02 *Lumi)  ) );
    files.push_back( make_pair("Z2Jets-Mu-ptZ-100to300-alpgen-PUS1.root",      (8.534e+00 	*Lumi)  ) );
    files.push_back( make_pair("Z2Jets-Mu-ptZ-300to800-alpgen-PUS1.root",      (1.151e-01*Lumi)  ) );
    files.push_back( make_pair("Z2Jets-Mu-ptZ-800to1600-alpgen-PUS1.root",     (3.023e-04 *Lumi)  ) );
    files.push_back( make_pair("Z3Jets-Mu-ptZ-0to100-alpgen-PUS1.root",        (2.289e+01 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-Mu-ptZ-100to300-alpgen-PUS1.root",      (3.951e+00 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-Mu-ptZ-300to800-alpgen-PUS1.root",      (8.344e-02 	*Lumi)  ) );
    files.push_back( make_pair("Z3Jets-Mu-ptZ-800to1600-alpgen-PUS1.root",     (2.480e-04 *Lumi)  ) );
  }

  if(sample.find("DiBoson")!=string::npos){
    files.push_back( make_pair("WW-Mu-pythia-PUS1.root",        ( 4.4   *Lumi)  ) );
    files.push_back( make_pair("WZ-Mu-pythia-PUS1.root",        (18.2   *Lumi)  ) );
    files.push_back( make_pair("ZZ-Mu-pythia-PUS1.root",        ( 5.9   *Lumi)  ) );
  }

  if(sample.find("SingleTop")!=string::npos){
    files.push_back( make_pair("TToBLNu-Mu-s-PUS1.root",        ( 1.4   *Lumi)  ) ); 
    files.push_back( make_pair("TToBLNu-Mu-t-PUS1.root",        ( 20.9   *Lumi)  ) ); 
    files.push_back( make_pair("TToBLNu-Mu-tW-PUS1.root",       ( 10.6  *Lumi)  ) );
  }
  
  if(sample.find("Wjets")!=string::npos){
    files.push_back( make_pair("W1Jets_ptW-0to100.root",       (3.693e+03*0.44*Lumi)  ) );
    files.push_back( make_pair("W1Jets_ptW-100to300.root",     (7.197e+01*0.58*Lumi)  ) );
    files.push_back( make_pair("W1Jets_ptW-300to800.root",     (5.658e-01*0.73*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-0to100.root",       (9.434e+02*0.46*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-100to300.root",     (6.718e+01*0.59*Lumi)  ) );
    files.push_back( make_pair("W2Jets_ptW-300to800.root",     (8.553e-01*0.75*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-0to100.root",       (2.087e+02*0.48*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-100to300.root",     (3.243e+01*0.60*Lumi)  ) );
    files.push_back( make_pair("W3Jets_ptW-300to800.root",     (6.229e-01*0.76*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-0to100.root",       (4.446e+01*0.50*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-100to300.root",     (1.138e+01*0.61*Lumi)  ) );
    files.push_back( make_pair("W4Jets_ptW-300to800.root",     (2.950e-01*0.77*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-0to100.root",       (1.111e+01*0.53*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-100to300.root",     (3.789e+00*0.65*Lumi)  ) );
    files.push_back( make_pair("W5Jets_ptW-300to800.root",     (1.565e-01*0.79*Lumi)  ) );
  }

  
  directories.push_back("allEventsFilter");
  directories.push_back("vertexScrapingFilter");
  directories.push_back("atLeastOneMuTauFilter");
  directories.push_back("muPtEtaFilter");
  directories.push_back("muPtEtaIDFilter");
  directories.push_back("tauPtEtaFilter");
  directories.push_back("tauPtEtaIDFilter");
  directories.push_back("tauPtEtaIDAgMuFilter");
  directories.push_back("tauPtEtaIDAgMuAgElecFilter");
  directories.push_back("atLeast1selectedDiTauFilter");


  TFile *outFile = new TFile(("treeMuTauStream_"+sample+".root").c_str(),"RECREATE");
  TChain* outChain = new TChain("muTauStreamAnalyzer/tree");

  float fraction = 1.0;

  for(unsigned int i = 0; i<files.size();i++){

    TFile *currentFile = new TFile(("/data_CMS/cms/lbianchini/MuTauStream2011_iter2/treeMuTauStream_"+files[i].first).c_str(),"READ");
    
    // protection
    if(currentFile->IsZombie()){
      cout << "No file " << files[i].first << " is found" << endl;
      continue;
    }

    currentFile->cd("allEventsFilter");
    TH1F* totalEvents = (TH1F*)gDirectory->Get("totalEvents");
    float readEvents = totalEvents->GetBinContent(1);
    float currentWeight = (files[i].second/readEvents);
    
    currentFile->cd("muTauStreamAnalyzer");
    TTree* currentTree =  (TTree*)gDirectory->Get("tree");

    TFile *newFile = new TFile( ("treeMuTauStream_"+files[i].first+"new").c_str(), "RECREATE");
    TDirectory* dir = (TDirectory*) newFile->mkdir("muTauStreamAnalyzer");
    int entries =  currentTree->GetEntries(); 
    TTree *tree = currentTree->CloneTree( (int)(fraction*entries) );
    float weight;
    TBranch *newBranch = tree->Branch("weight",&weight,"weight/F");
    int Entries = tree->GetEntries();
    for(int n=0; n< Entries ; n++ ){
      tree->GetEntry(n);
      if(n%5000==0) cout << "Processing event " << n << " with weight " << currentWeight/fraction << endl;
      weight=currentWeight/fraction;
      newBranch->Fill();
    }

    dir->cd();
    tree->Write();
    newFile->Write();

    outChain->AddFile( ("treeMuTauStream_"+files[i].first+"new").c_str() );
    cout << " outChain has " << outChain->GetEntries() 
	 << endl;

    currentFile->Close();
    newFile->Close();
    delete currentFile;
    delete newFile;

 }

  
  TDirectory* dir = (TDirectory*) outFile->mkdir("muTauStreamAnalyzer");  
  TTree* outTree = (TTree*) outChain->CopyTree("");
    
  dir->cd();
  outTree->Write("", TObject::kOverwrite);



  for(unsigned int j = 0; j<directories.size(); j++){

    cout << "Directory " <<  directories[j] << endl;
    outFile->mkdir(directories[j].c_str());

    // CASE 1: a filter directory
    if( directories[j].find("Filter")!=string::npos){
      TH1F* filterHisto = new TH1F();

      // loop over the files
      for(unsigned int i = 0; i<files.size();i++){
	TFile *currentFile = new TFile(("/data_CMS/cms/lbianchini/MuTauStream2011_iter2/treeMuTauStream_"+files[i].first).c_str(),"READ");

	// protection
	if(currentFile->IsZombie()){
	  cout << "No file " << files[i].first << " is found" << endl;
	  continue;
	}

	currentFile->cd("allEventsFilter");
	TH1F* totalEvents = (TH1F*)gDirectory->Get("totalEvents");
	float readEvents = totalEvents->GetBinContent(1);
	float currentWeight;
	currentWeight = (files[i].second/readEvents);
	currentWeight/=fraction;
	currentFile->cd(directories[j].c_str());
	TH1F* currentHisto =  (TH1F*)gDirectory->Get("totalEvents");
	if(i==0){
	  filterHisto = (TH1F*)currentHisto->Clone("totalEvents_v2");
	  filterHisto->Scale(currentWeight);
	  filterHisto->SetName("totalEvents");
	}
	else{
	  filterHisto->Add(currentHisto,currentWeight);
	}
	cout<<"Adding "<<currentHisto->GetEntries() << endl;
	cout<<"SubTot "<<filterHisto->GetEntries() << endl;
      } // file

      outFile->cd(directories[j].c_str());
      filterHisto->Write();  

    }
   

  }//directories

  
  outFile->Write();
  outFile->Close();
  delete outFile;

}



