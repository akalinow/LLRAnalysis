
/** \executable addBackgroundZTT
 *
 * Estimate Z -> tau tau background, taking normalization from Drell-Yan Monte Carlo
 * and shape from Embedded sample.
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
  std::string getSubdirNameOutput(const std::string& region, const std::string& category, const std::string& tauPtBin, const std::string& process) 
  {
    std::string subdirName_output = Form("tauTau_%s_%s/%s/%s", region.data(), category.data(), tauPtBin.data(), process.data());
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

  std::cout << "<addBackgroundZTT>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("addBackgroundZTT");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("addBackgroundZTT") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgAddBackgroundZTT = cfg.getParameter<edm::ParameterSet>("addBackgroundZTT");
  
  vstring regions = cfgAddBackgroundZTT.getParameter<vstring>("regions");

  std::string processZTT = cfgAddBackgroundZTT.getParameter<std::string>("processZTT");
  std::string processZTTmc = cfgAddBackgroundZTT.getParameter<std::string>("processZTTmc");
  std::string processZTT_Embedded = cfgAddBackgroundZTT.getParameter<std::string>("processZTT_Embedded");
  std::string processTT_Embedded = cfgAddBackgroundZTT.getParameter<std::string>("processTT_Embedded");

  vstring categories = cfgAddBackgroundZTT.getParameter<vstring>("categories");
  std::string category_inclusive = cfgAddBackgroundZTT.getParameter<std::string>("category_inclusive");

  vstring tauPtBins = cfgAddBackgroundZTT.getParameter<vstring>("tauPtBins");

  vstring central_or_shifts = cfgAddBackgroundZTT.getParameter<vstring>("sysShifts");
  central_or_shifts.push_back(""); // CV: add central value

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("addBackgroundZTT") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  for ( vstring::const_iterator region = regions.begin();
	region != regions.end(); ++region ) {
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      for ( vstring::const_iterator tauPtBin = tauPtBins.begin();
	    tauPtBin != tauPtBins.end(); ++tauPtBin ) {
	std::cout << "processing region = " << (*region) << ", category = " << (*category) << ", tauPtBin = " << (*tauPtBin) << std::endl;
	
	TDirectory* dir_inclusive = getDirectory(inputFile, *region, category_inclusive, *tauPtBin, true);
	assert(dir_inclusive);
	
	TH1* histogramZTTmc_inclusive = getHistogram(dir_inclusive, processZTTmc, "EventCounter", "central", true);
	assert(histogramZTTmc_inclusive);
	std::cout << " integral(ZTTmc_inclusive) = " << histogramZTTmc_inclusive->Integral() << std::endl;
	TH1* histogramZTT_Embedded_inclusive = getHistogram(dir_inclusive, processZTT_Embedded, "EventCounter", "central", true);
	assert(histogramZTT_Embedded_inclusive);
	std::cout << " integral(ZTT_Embedded_inclusive) = " << histogramZTT_Embedded_inclusive->Integral() << std::endl;
	TH1* histogramTT_Embedded_inclusive = getHistogram(dir_inclusive, processTT_Embedded, "EventCounter", "central", true);
	assert(histogramTT_Embedded_inclusive);
	std::cout << " integral(TT_Embedded_inclusive) = " << histogramTT_Embedded_inclusive->Integral() << std::endl;
	
	double sfZTTfromEmbedded = histogramZTTmc_inclusive->Integral()/(histogramZTT_Embedded_inclusive->Integral() - histogramTT_Embedded_inclusive->Integral());
	std::cout << " sfZTTfromEmbedded = " << sfZTTfromEmbedded << std::endl;
	
	TDirectory* dir = getDirectory(inputFile, *region, *category, *tauPtBin, true);
	assert(dir);
	
	vstring histograms;
	TDirectory* dirZTTmc = dynamic_cast<TDirectory*>(dir->Get(processZTTmc.data()));
	if ( !dirZTTmc ) throw cms::Exception("addBackgroundZTT")  
	  << "Failed to find directory for process = " << processZTTmc << " !!\n";
	TList* list = dirZTTmc->GetListOfKeys();
	TIter next(list);
	TKey* key = 0;
	while ( (key = dynamic_cast<TKey*>(next())) ) {
	  TObject* object = key->ReadObj();
	  TH1* histogram = dynamic_cast<TH1*>(object);
	  if ( !histogram ) continue;
	  TString histogramName = TString(histogram->GetName()).ReplaceAll(Form("%s_", processZTTmc.data()), "");
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) {
	      histogramName = histogramName.ReplaceAll(Form("%s_", central_or_shift->data()), "");
	    }
	  }
	  std::cout << "adding histogram = " << histogramName.Data() << std::endl;
	  histograms.push_back(histogramName.Data());
	}
	
	for ( vstring::const_iterator histogram = histograms.begin();
	      histogram != histograms.end(); ++histogram ) {
	  std::cout << "histogram = " << (*histogram) << std::endl;
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    
	    TH1* histogramZTT_Embedded = getHistogram(dir, processZTT_Embedded, *histogram, *central_or_shift, true);
	    TH1* histogramTT_Embedded = getHistogram(dir, processTT_Embedded, *histogram, *central_or_shift, false);
	    if ( !histogramTT_Embedded ) histogramTT_Embedded = getHistogram(dir, processTT_Embedded, *histogram, "central", true);
	    
	    std::string subdirName_output = getSubdirNameOutput(*region, *category, *tauPtBin, processZTT);
	    TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	    subdir_output->cd();
	    
	    std::string histogramNameZTTfromEmbedded = processZTT;
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) histogramNameZTTfromEmbedded.append("_").append(*central_or_shift);
	    histogramNameZTTfromEmbedded.append("_").append(*histogram);
	    TH1* histogramZTTfromEmbedded = subtractHistograms(histogramNameZTTfromEmbedded, histogramZTT_Embedded, histogramTT_Embedded);
	    histogramZTTfromEmbedded->Scale(sfZTTfromEmbedded);
	    makeBinContentsPositive(histogramZTTfromEmbedded);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(ZTTfromEmbedded) = " << histogramZTTfromEmbedded->Integral() << std::endl;
	    }
	  }
	}
      }
    }
  }
  
  delete inputFile;

  clock.Show("addBackgroundZTT");

  return 0;
}
