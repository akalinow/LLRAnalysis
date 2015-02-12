
#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TList.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TROOT.h>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <assert.h>

void processRecursively(TDirectory* dir, const std::string& dirName, TFile* inputFile_TIFR, TFile* inputFile_LLR, TDirectory* outputDir)
{
  TList* keys = dir->GetListOfKeys();
  TIter nextObj(keys);
  while ( TObject* obj = nextObj() ) {
    TKey* key = dynamic_cast<TKey*>(obj);
    if ( key ) {
      TObject* key_obj = key->ReadObj();
      //std::cout << key_obj->ClassName() << " " << key->GetName() << std::endl;
      if ( dynamic_cast<TDirectory*>(key_obj) ) {
	TDirectory* subdir = dynamic_cast<TDirectory*>(key_obj);
	std::string subdirName = Form("%s/%s", dirName.data(), key->GetName());
	std::cout << "processing directory = " << subdirName << std::endl;
	processRecursively(subdir, subdirName, inputFile_TIFR, inputFile_LLR, outputDir->mkdir(key->GetName()));
      } 
      if ( dynamic_cast<TH1*>(key_obj) ) {
	TH1* histogram = dynamic_cast<TH1*>(key_obj);
	std::string histogramName = Form("%s/%s", dirName.data(), key->GetName());
	//std::cout << "processing histogram = " << histogramName << std::endl;
	if ( std::string(key->GetName()).find("QCD") == std::string::npos ) {
	  std::cout << " taking histogram = " << histogramName << " from TIFR" << std::endl;
	  TH1* histogram_TIFR = dynamic_cast<TH1*>(histogram->Clone(key->GetName()));
	  assert(histogram_TIFR);
	  outputDir->cd();
	  histogram_TIFR->Write();
	} else {
	  std::cout << " taking histogram = " << histogramName << " from LLR" << std::endl;
	  TH1* histogram_LLR = dynamic_cast<TH1*>(inputFile_LLR->Get(histogramName.data()));
	  if ( !histogram_LLR ) {
	    std::cerr << "Error: failed to find histogram = " << histogramName << " in input file = " << inputFile_LLR->GetName() << " !!" << std::endl;
	    continue;
	  }
	  outputDir->cd();
	  histogram_LLR->Write();
	}
      } 
    }
  }
}

void mergeDatacards()
{
  gROOT->SetBatch(true);

  TH1::AddDirectory(false);

  std::string inputFilePath_LLR = "/data1/veelken/tmp/tauTauAnalysis/v3_13_3/";
  std::string inputFileName_LLR = "htt_tt.inputs-mssm-8TeV-0_MVAwLToldDMsTight.root";
  TFile* inputFile_LLR = new TFile(Form("%s%s", inputFilePath_LLR.data(), inputFileName_LLR.data()));

  std::string inputFilePath_TIFR = "/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/LLRAnalysis/HadTauStudies/macros/";
  std::string inputFileName_TIFR = "fromRam/2014Jul21/DATACARD_MVAWLTTIGHT.root";
  TFile* inputFile_TIFR = new TFile(Form("%s%s", inputFilePath_TIFR.data(), inputFileName_TIFR.data()));

  std::string inputFilePath_merged = "/afs/cern.ch/user/v/veelken/scratch0/CMSSW_5_3_14/src/LLRAnalysis/HadTauStudies/macros/";
  std::string inputFileName_merged = "mergeDatacards.root";
  TFile* outputFile_merged = new TFile(Form("%s%s", inputFilePath_merged.data(), inputFileName_merged.data()), "RECREATE");

  processRecursively(inputFile_TIFR, "", inputFile_TIFR, inputFile_LLR, outputFile_merged);

  delete inputFile_LLR;
  delete inputFile_TIFR;
  delete outputFile_merged;
}


