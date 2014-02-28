
/** \executable prepareTauTauDatacards
 *
 * Prepare datacards for MSSM -> tau tau -> tau_h tau_h analysis.
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"

#include <TFile.h>
#include <TH1.h>
#include <TBenchmark.h>
#include <TMath.h>
#include "TPRegexp.h"
#include "TDirectory.h"
#include "TList.h"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;

namespace
{
  TDirectory* getDirectory(TFile* inputFile, const std::string& region, const std::string& category, const std::string& tauPtBin, bool enableException)
  {
    std::string dirName = Form("tauTau_%s_%s/%s", region.data(), category.data(), tauPtBin.data());
    TDirectory* dir = dynamic_cast<TDirectory*>(inputFile->Get(dirName.data()));
    if ( enableException && !dir )
      throw cms::Exception("getDirectory") 
	<< "Failed to find directory for category = " << category << ", tauPtBin = " << tauPtBin << " !!\n";
    return dir;
  }
     
  TDirectory* createSubdirectory(TDirectory* dir, const std::string& subdirName)
  {
    dir->cd();
    if ( !dir->Get(subdirName.data()) ) {
      dir->mkdir(subdirName.data());
    }
    TDirectory* subdir = dynamic_cast<TDirectory*>(dir->Get(subdirName.data()));
    assert(subdir);
    return subdir;
  }
  
  TDirectory* createSubdirectory_recursively(TFileDirectory& dir, const std::string& fullSubdirName)
  {
    TString fullSubdirName_tstring = fullSubdirName.data();
    TObjArray* subdirNames = fullSubdirName_tstring.Tokenize("/");
    int numSubdirectories = subdirNames->GetEntries();
    TDirectory* parent = dir.getBareDirectory();
    for ( int iSubdirectory = 0; iSubdirectory < numSubdirectories; ++iSubdirectory ) {
      const TObjString* subdirName = dynamic_cast<TObjString*>(subdirNames->At(iSubdirectory));
      assert(subdirName);
      TDirectory* subdir = createSubdirectory(parent, subdirName->GetString().Data());
      parent = subdir;
    }
    return parent;
  }

  void copyHistogram(TDirectory* dir_input, const std::string& process, const std::string& histogramName_input, 
		     const std::string& histogramName_output, const std::string& central_or_shift, bool enableException)
  {
    std::string histogramName_input_full = histogramName_input;
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_input_full.append("_").append(central_or_shift);
    histogramName_input_full.append("_").append(histogramName_input);
    TH1* histogram_input = dynamic_cast<TH1*>(dir_input->Get(histogramName_input_full.data()));
    if ( !histogram_input ) {
      if ( enableException ) 
	throw cms::Exception("copyHistogram")
	  << "Failed to find histogram = " << histogramName_input_full << " in directory = " << dir_input->GetName() << " !!\n";
      return;
    }
    std::string histogramName_output_full = process;
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_output_full.append("_").append(central_or_shift);
    if ( histogramName_output != "" ) histogramName_output_full.append("_").append(histogramName_output);
    TH1* histogram_output = (TH1*)histogram_input->Clone(histogramName_output_full.data());
  }
  
  TH1* getHistogram(TDirectory* dir, const std::string& process, const std::string& histogramName, const std::string& central_or_shift, bool enableException)
  {
    std::string histogramName_full = Form("%s/%s", process.data(), process.data());
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_full.append("_").append(central_or_shift);
    histogramName_full.append("_").append(histogramName);
    TH1* histogram = dynamic_cast<TH1*>(dir->Get(histogramName_full.data()));
    if ( enableException && !histogram ) 
      throw cms::Exception("getHistogram") 
	<< "Failed to find histogram = " << histogramName_full << " in directory = " << dir->GetName() << " !!\n";    
    return histogram;
  }
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<prepareTauTauDatacards>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("prepareTauTauDatacards");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("prepareTauTauDatacards") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgPrepareTauTauDatacards = cfg.getParameter<edm::ParameterSet>("prepareTauTauDatacards");
  
  std::string signalRegion = cfgPrepareTauTauDatacards.getParameter<std::string>("signalRegion");

  vstring processesToCopy_string = cfgPrepareTauTauDatacards.getParameter<vstring>("processesToCopy");
  std::vector<TPRegexp*> processesToCopy;
  for ( vstring::const_iterator processToCopy_string = processesToCopy_string.begin();
	processToCopy_string != processesToCopy_string.end(); ++processToCopy_string ) {
    TPRegexp* processToCopy = new TPRegexp(processToCopy_string->data());
    processesToCopy.push_back(processToCopy);
  }

  std::string processData = cfgPrepareTauTauDatacards.getParameter<std::string>("processData");
  std::string processZTT = cfgPrepareTauTauDatacards.getParameter<std::string>("processZTT");
  std::string processZTT_Embedded = cfgPrepareTauTauDatacards.getParameter<std::string>("");
  std::string processTT_Embedded = cfgPrepareTauTauDatacards.getParameter<std::string>("");
    
  std::string processQCD = cfgPrepareTauTauDatacards.getParameter<std::string>("processQCD");
  vstring processesToSubtract = cfgPrepareTauTauDatacards.getParameter<vstring>("processesToSubtract");

  vstring categoriesToCopy = cfgPrepareTauTauDatacards.getParameter<vstring>("categoriesToCopy");
  std::string category_inclusive = cfgPrepareTauTauDatacards.getParameter<std::string>("category_inclusive");

  vstring tauPtBins = cfgPrepareTauTauDatacards.getParameter<vstring>("tauPtBins");

  std::string histogramToFit = cfgPrepareTauTauDatacards.getParameter<std::string>("histogramToFit");
  std::string histogramToFit_fine_binning = cfgPrepareTauTauDatacards.getParameter<std::string>("histogramToFit_fine_binning");

  std::map<std::string, std::string> histogramMapping; // key = inputHistogramName, value = outputHistogramName
  histogramMapping[histogramToFit] = "";
  histogramMapping[histogramToFit_fine_binning] = "fine_binning";

  vstring central_or_shifts = cfgPrepareTauTauDatacards.getParameter<vstring>("sysShifts");
  central_or_shifts.push_back(""); // CV: add central value

  std::vector<TH1*> histogramsForDatacards;

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("prepareTauTauDatacards") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  for ( vstring::const_iterator category = categoriesToCopy.begin();
	category != categoriesToCopy.end(); ++category ) {
    for ( vstring::const_iterator tauPtBin = tauPtBins.begin();
	  tauPtBin != tauPtBins.end(); ++tauPtBin ) {
      TDirectory* dir = getDirectory(inputFile, signalRegion, *category, *tauPtBin, true);
      assert(dir);

      // copy histograms that do not require modifications
      TList* list = dir->GetList();
      TIter next(list);
      TObject* subdir_object = 0;
      while ( (subdir_object = dynamic_cast<TObject*>(next())) ) {
	TDirectory* subdir = dynamic_cast<TDirectory*>(subdir_object);
	if ( !subdir ) continue;
	bool isToCopy = false;
	for ( std::vector<TPRegexp*>::iterator processToCopy = processesToCopy.begin();
	      processToCopy != processesToCopy.end(); ++processToCopy ) {
	  if ( (*processToCopy)->Match(subdir->GetName()) ) isToCopy = true;
	}
	if ( isToCopy ) {
	  for ( std::map<std::string, std::string>::const_iterator histogramToCopy = histogramMapping.begin();
		histogramToCopy != histogramMapping.end(); ++histogramToCopy ) {
	    for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		  central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	      std::string subdirName_output = Form("tauTau_%s_%s", category->data(), tauPtBin->data());
	      TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	      subdir_output->cd();
	      copyHistogram(subdir, subdir->GetName(), histogramToCopy->first, histogramToCopy->second, *central_or_shift, (*central_or_shift) == "" || (*central_or_shift) == "central");
	    }
	  }
	}
      }
      
      // compute shape templates for Z -> tautau
      TDirectory* dir_inclusive = getDirectory(inputFile, signalRegion, category_inclusive, *tauPtBin, true);
      assert(dir_inclusive);
      for ( std::map<std::string, std::string>::const_iterator histogramToCopy = histogramMapping.begin();
	    histogramToCopy != histogramMapping.end(); ++histogramToCopy ) {
	TH1* histogramZTT_inclusive = getHistogram(dir_inclusive, processZTT, "EventCounter", "central", true);
	assert(histogramZTT_inclusive);
	TH1* histogramZTT_Embedded_inclusive = getHistogram(dir_inclusive, processZTT_Embedded, "EventCounter", "central", true);
	assert(histogramZTT_Embedded_inclusive);
	TH1* histogramTT_Embedded_inclusive = getHistogram(dir_inclusive, processTT_Embedded, "EventCounter", "central", true);
	assert(histogramTT_Embedded_inclusive);

	double sfZTTfromEmbedded = (histogramZTT_inclusive->Integral() - histogramTT_Embedded_inclusive->Integral())/histogramZTT_inclusive->Integral();
	
	for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
	      central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	  TH1* histogramZTT_Embedded = getHistogram(dir, processZTT_Embedded, histogramToCopy->first, *central_or_shift, false);
	  if ( !histogramZTT_Embedded ) continue;
	  TH1* histogramTT_Embedded = getHistogram(dir, processTT_Embedded, histogramToCopy->first, *central_or_shift, false);
	  if ( !histogramTT_Embedded ) continue;
	  
	  std::string subdirName_output = Form("tauTau_%s_%s", category->data(), tauPtBin->data());
	  TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	  subdir_output->cd();

	  std::string histogramNameZTTfromEmbedded = processZTT;
	  if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) histogramNameZTTfromEmbedded.append("_").append(*central_or_shift);
	  if ( histogramToCopy->second != "" ) histogramNameZTTfromEmbedded.append("_").append(histogramToCopy->second);
	  TH1* histogramZTTfromEmbedded = subtractHistograms(histogramNameZTTfromEmbedded, histogramZTT_Embedded, histogramTT_Embedded);
	  histogramZTTfromEmbedded->Scale(sfZTTfromEmbedded);
	  makeBinContentsPositive(histogramZTTfromEmbedded);
	}
      }

      // compute shape templates for QCD
      for ( std::map<std::string, std::string>::const_iterator histogramToCopy = histogramMapping.begin();
	    histogramToCopy != histogramMapping.end(); ++histogramToCopy ) {
	for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
	      central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	  TH1* histogramData = getHistogram(dir, processData, histogramToCopy->first, *central_or_shift, true);
	  assert(histogramData);

	  std::vector<TH1*> histogramsToSubtract;
	  for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
		processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
	    TH1* histogramToSubtract = getHistogram(dir, *processToSubtract, histogramToCopy->first, *central_or_shift, false);
	    if ( histogramToSubtract ) histogramsToSubtract.push_back(histogramToSubtract);
	  }

	  std::string subdirName_output = Form("tauTau_%s_%s", category->data(), tauPtBin->data());
	  TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	  subdir_output->cd();

	  std::string histogramNameQCD = processQCD;
	  if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) histogramNameQCD.append("_").append(*central_or_shift);
	  if ( histogramToCopy->second != "" ) histogramNameQCD.append("_").append(histogramToCopy->second);
	  TH1* histogramQCD = subtractHistograms(histogramNameQCD, histogramData, histogramsToSubtract);
	  makeBinContentsPositive(histogramQCD);	  
	}
      }
    }
  }
  
  delete inputFile;

  clock.Show("prepareTauTauDatacards");

  return 0;
}
