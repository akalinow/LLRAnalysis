
/** \executable prepareTauTauDatacards2b2tau
 *
 * Prepare datacards for 2 Higgs -> 2 b-jet 2 tau_h analysis
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
#include <TArrayD.h>
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
  TArrayD getBinning(const TH1* histogram)
  {
    TAxis* xAxis = histogram->GetXaxis();
    int numBins = xAxis->GetNbins();
    TArrayD binning(numBins + 1);
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      binning[iBin - 1] = xAxis->GetBinLowEdge(iBin);
      binning[iBin] = xAxis->GetBinUpEdge(iBin);
    }
    return binning;
  }

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
    // CV: save output histogram as TH1F in order to keep size of datacard.root files small;
    //     and also because the tailfit macro HiggsAnalysis/HiggsToTauTau/macros/addAllNuisance.C
    //     saves its output as TH1F, which causes error message
    //         File "/afs/cern.ch/user/v/veelken/scratch0/mssmHtautauLimits/CMSSW_6_1_1/python/HiggsAnalysis/CombinedLimit/ShapeTools.py", line 386, in getPdf
    //           if shapeUp.ClassName()   != shapeNominal.ClassName(): raise RuntimeError, "Mismatched shape types for channel %s, process %s, syst %s" % (channel,process,syst)
    //       RuntimeError: Mismatched shape types for channel htt_tt_14_8TeV, process QCD, syst CMS_htt_QCDfrNorm_tautau_8TeV
    //     in case one mixes TH1D and TH1F
    //histogram_output = (TH1*)histogram_input->Clone(histogramName_output_full.data());
    TArrayD histogramBinning = getBinning(histogram_input);
    int numBins = histogramBinning.GetSize() - 1;
    TH1* histogram_output = new TH1F(histogramName_output_full.data(), histogramName_output_full.data(), numBins, histogramBinning.GetArray());
    if ( !histogram_output->GetSumw2N() ) histogram_output->Sumw2();
    for ( int iBin = 0; iBin <= (numBins + 1); ++iBin ) {
      double binContent = histogram_input->GetBinContent(iBin);
      histogram_output->SetBinContent(iBin, binContent);
      double binError = histogram_input->GetBinError(iBin);
      histogram_output->SetBinError(iBin, binError);
      // CV: set underflow, overflow and all bins below 50 GeV to zero
      double binCenter = histogram_input->GetBinCenter(iBin);
      if ( iBin == 0 || binCenter < 50. || iBin == (numBins + 1) ) {
	histogram_output->SetBinContent(iBin, 0.);
	histogram_output->SetBinError(iBin, 0.);
      }
    }
  }
    
  std::string getSubdirNameOutput(const std::string& category, const std::string& tauPtBin) 
  {
    std::string subdirName_output = Form("tauTau_%s", category.data());
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

  std::cout << "<prepareTauTauDatacards2b2tau>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("prepareTauTauDatacards2b2tau");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("prepareTauTauDatacards2b2tau") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgPrepareTauTauDatacards2b2tau = cfg.getParameter<edm::ParameterSet>("prepareTauTauDatacards2b2tau");
  
  std::string signalRegion = cfgPrepareTauTauDatacards2b2tau.getParameter<std::string>("signalRegion");

  vstring processesToCopy_string = cfgPrepareTauTauDatacards2b2tau.getParameter<vstring>("processesToCopy");
  std::vector<TPRegexp*> processesToCopy;
  for ( vstring::const_iterator processToCopy_string = processesToCopy_string.begin();
	processToCopy_string != processesToCopy_string.end(); ++processToCopy_string ) {
    TPRegexp* processToCopy = new TPRegexp(processToCopy_string->data());
    processesToCopy.push_back(processToCopy);
  }

  vstring categories = cfgPrepareTauTauDatacards2b2tau.getParameter<vstring>("categories");

  vstring tauPtBins = cfgPrepareTauTauDatacards2b2tau.getParameter<vstring>("tauPtBins");
  if ( tauPtBins.size() != 1 )
    throw cms::Exception("prepareTauTauDatacards2b2tau") 
      << "Exactly one tau Pt bin expected !!\n";
  
  std::string histogramToFit = cfgPrepareTauTauDatacards2b2tau.getParameter<std::string>("histogramToFit");
  std::string histogramToFit_fine_binning = cfgPrepareTauTauDatacards2b2tau.getParameter<std::string>("histogramToFit_fine_binning");

  std::map<std::string, std::string> histogramMapping; // key = inputHistogramName, value = outputHistogramName
  histogramMapping[histogramToFit] = "";
  if ( histogramToFit_fine_binning != histogramToFit ) histogramMapping[histogramToFit_fine_binning] = "fine_binning";

  vstring central_or_shifts = cfgPrepareTauTauDatacards2b2tau.getParameter<vstring>("sysShifts");
  central_or_shifts.push_back(""); // CV: add central value

  std::vector<TH1*> histogramsForDatacards;

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("prepareTauTauDatacards2b2tau") 
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
	      
	      std::string category_and_discriminator;
	      //if ( category->find('_') != std::string::npos ) {
	      //  size_t idx = category->find_last_of('_');
	      //  category_and_discriminator = Form("%s_%s%s", std::string(*category, 0, idx).data(), discriminator->data(), std::string(*category, idx).data());
	      //} else {
	      //  category_and_discriminator = Form("%s_%s", category->data(), discriminator->data());
	      //}
	      category_and_discriminator = (*category);
	      std::string subdirName_output = getSubdirNameOutput(category_and_discriminator, *tauPtBin);
	      if ( subdirName_output == "" ) continue; // CV: skip tau Pt bins that are not used in datacards, in order to keep size of datacard.root files small
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

  clock.Show("prepareTauTauDatacards2b2tau");

  return 0;
}
