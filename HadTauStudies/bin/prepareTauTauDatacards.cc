
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
#include "TKey.h"
#include "TObject.h"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;

namespace
{
  void copyHistogram(TDirectory* dir_input, const std::string& process, const std::string& histogramName_input, 
		     const std::string& histogramName_output, const std::string& central_or_shift, bool enableException)
  {
    //std::cout << "<copyHistogram>:" << std::endl;
    //std::cout << " dir_input = " << dir_input->GetName() << std::endl;
    //std::cout << " process = " << process << std::endl;
    //std::cout << " histogramName_input = " << histogramName_input << std::endl;
    //std::cout << " histogramName_output = " << histogramName_output << std::endl;
    //std::cout << " central_or_shift = " << central_or_shift << std::endl;
    //std::cout << " enableException = " << enableException << std::endl;
    std::string histogramName_input_full = process;
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_input_full.append("_").append(central_or_shift);
    histogramName_input_full.append("_").append(histogramName_input);
    TH1* histogram_input = dynamic_cast<TH1*>(dir_input->Get(histogramName_input_full.data()));
    if ( !histogram_input ) {
      if ( enableException ) 
	throw cms::Exception("copyHistogram")
	  << "Failed to find histogram = " << histogramName_input_full << " in directory = " << dir_input->GetName() << " !!\n";
      return;
    }   
    std::cout << " integral(" << process << ") = " << histogram_input->Integral() << std::endl;
    std::string histogramName_output_full = process;
    if ( !(central_or_shift == "" || central_or_shift == "central") ) histogramName_output_full.append("_").append(central_or_shift);
    if ( histogramName_output != "" ) histogramName_output_full.append("_").append(histogramName_output);
    TH1* histogram_output = (TH1*)histogram_input->Clone(histogramName_output_full.data());
  }
    
  std::string getSubdirNameOutput(const std::string& category, const std::string& tauPtBin) 
  {
    // CV: workaround for compatibility with HIG-13-021 datacards
    //     that do not use tauPt bins
    std::string subdirName_output = "";
    if ( tauPtBin == "tau1PtGt45tau2PtGt45" ) subdirName_output = Form("tauTau_%s", category.data());
    else if ( (category == "inclusive" || category == "nobtag") && tauPtBin == "tau1PtGt45tau2Pt45to60" ) subdirName_output = Form("tauTau_%s_lowtau", category.data());
    else if ( (category == "inclusive" || category == "nobtag") && tauPtBin == "tau1PtGt45tau2Pt60to80" ) subdirName_output = Form("tauTau_%s_mediumtau", category.data());
    else if ( (category == "inclusive" || category == "nobtag") && tauPtBin == "tau1PtGt45tau2PtGt80"   ) subdirName_output = Form("tauTau_%s_hightau", category.data());
    else if (  category == "btag"                               && tauPtBin == "tau1PtGt45tau2Pt45to60" ) subdirName_output = Form("tauTau_%s_lowtau", category.data());
    else if (  category == "btag"                               && tauPtBin == "tau1PtGt45tau2PtGt60"   ) subdirName_output = Form("tauTau_%s_hightau", category.data());
    else subdirName_output = Form("tauTau_%s_%s", category.data(), tauPtBin.data());
    return subdirName_output;
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

  vstring categories = cfgPrepareTauTauDatacards.getParameter<vstring>("categories");

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

  for ( vstring::const_iterator category = categories.begin();
	category != categories.end(); ++category ) {
    for ( vstring::const_iterator tauPtBin = tauPtBins.begin();
	  tauPtBin != tauPtBins.end(); ++tauPtBin ) {
      std::cout << "processing category = " << (*category) << ", tauPtBin = " << (*tauPtBin) << std::endl;

      TDirectory* dir_signalRegion = getDirectory(inputFile, signalRegion, *category, *tauPtBin, true);
      assert(dir_signalRegion);

      // copy histograms that do not require modifications
      std::cout << "copying histograms that do not require modifications" << std::endl;
      TList* list = dir_signalRegion->GetListOfKeys();
      TIter next(list);
      TKey* key = 0;
      while ( (key = dynamic_cast<TKey*>(next())) ) {
        TObject* object = key->ReadObj();
	TDirectory* subdir = dynamic_cast<TDirectory*>(object);
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
	      std::cout << "histogramToCopy = " << histogramToCopy->first << ", central_or_shift = " << (*central_or_shift) << std::endl;

	      std::string subdirName_output = getSubdirNameOutput(*category, *tauPtBin);
	      TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	      subdir_output->cd();
	      copyHistogram(subdir, subdir->GetName(), histogramToCopy->first, histogramToCopy->second, *central_or_shift, (*central_or_shift) == "" || (*central_or_shift) == "central");	      
	    }
	  }
	}
      }
    }
  }
  
  delete inputFile;

  clock.Show("prepareTauTauDatacards");

  return 0;
}
