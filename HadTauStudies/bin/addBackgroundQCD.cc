
/** \executable addBackgroundQCD
 *
 * Compute shape templates and normalization for QCD background.
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
  struct regionEntryType
  {
    regionEntryType(const edm::ParameterSet& cfg, const vstring& categories)
    {
      name_ = cfg.getParameter<std::string>("name");
      edm::ParameterSet cfgQCDregion_norm = cfg.getParameter<edm::ParameterSet>("qcdRegion_norm");
      edm::ParameterSet cfgQCDregion_shape = cfg.getParameter<edm::ParameterSet>("qcdRegion_shape");
      for ( vstring::const_iterator category = categories.begin();
	    category != categories.end(); ++category ) {
	qcdRegion_norm_[*category] = cfgQCDregion_norm.getParameter<std::string>(*category);
	qcdRegion_shape_[*category] = cfgQCDregion_shape.getParameter<std::string>(*category);
      }
    }
    ~regionEntryType() {}
    std::string name_;
    std::map<std::string, std::string> qcdRegion_norm_;  // key = category
    std::map<std::string, std::string> qcdRegion_shape_; // key = category
  };

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

  std::cout << "<addBackgroundQCD>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("addBackgroundQCD");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("addBackgroundQCD") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgAddBackgroundQCD = cfg.getParameter<edm::ParameterSet>("addBackgroundQCD");
  
  vstring categories = cfgAddBackgroundQCD.getParameter<vstring>("categories");

  std::vector<regionEntryType*> regions;
  edm::VParameterSet cfgRegions = cfgAddBackgroundQCD.getParameter<edm::VParameterSet>("regions");
  for ( edm::VParameterSet::const_iterator cfgRegion = cfgRegions.begin();
	cfgRegion != cfgRegions.end(); ++cfgRegion ) {
    regionEntryType* region = new regionEntryType(*cfgRegion, categories);
    regions.push_back(region);
  }

  std::string processData = cfgAddBackgroundQCD.getParameter<std::string>("processData");
  std::string processQCD = cfgAddBackgroundQCD.getParameter<std::string>("processQCD");
  vstring processesToSubtract = cfgAddBackgroundQCD.getParameter<vstring>("processesToSubtract");

  vstring tauPtBins = cfgAddBackgroundQCD.getParameter<vstring>("tauPtBins");

  vstring central_or_shifts = cfgAddBackgroundQCD.getParameter<vstring>("sysShifts");
  central_or_shifts.push_back(""); // CV: add central value

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("addBackgroundQCD") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());
  
  for ( std::vector<regionEntryType*>::const_iterator region = regions.begin();
	region != regions.end(); ++region ) {
    for ( vstring::const_iterator category = categories.begin();
	  category != categories.end(); ++category ) {
      for ( vstring::const_iterator tauPtBin = tauPtBins.begin();
	    tauPtBin != tauPtBins.end(); ++tauPtBin ) {
	std::cout << "processing region = " << (*region)->name_ << ", category = " << (*category) << ", tauPtBin = " << (*tauPtBin) << std::endl;
	
	TDirectory* dir = getDirectory(inputFile, (*region)->name_, *category, *tauPtBin, true);
	assert(dir);
	
	vstring histograms;
	TDirectory* dirData = dynamic_cast<TDirectory*>(dir->Get(processData.data()));
	if ( !dirData ) throw cms::Exception("addBackgroundQCD")  
	  << "Failed to find directory for process = " << processData << " !!\n";
	TList* list = dirData->GetListOfKeys();
	TIter next(list);
	TKey* key = 0;
	while ( (key = dynamic_cast<TKey*>(next())) ) {
	  TObject* object = key->ReadObj();
	  TH1* histogram = dynamic_cast<TH1*>(object);
	  if ( !histogram ) continue;
	  TString histogramName = TString(histogram->GetName()).ReplaceAll(Form("%s_", processData.data()), "");
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) {
	      histogramName = histogramName.ReplaceAll(Form("%s_", central_or_shift->data()), "");
	    }
	  }
	  std::cout << "adding histogram = " << histogramName.Data() << std::endl;
	  histograms.push_back(histogramName.Data());
	}

	TDirectory* dir_qcdRegion_norm = getDirectory(inputFile, (*region)->qcdRegion_norm_[*category], *category, *tauPtBin, true);
	assert(dir_qcdRegion_norm);
	std::cout << "dir_qcdRegion_norm = " << dir_qcdRegion_norm << ": name = " << dir_qcdRegion_norm->GetName() << std::endl;
	TDirectory* dir_qcdRegion_shape = getDirectory(inputFile, (*region)->qcdRegion_shape_[*category], *category, *tauPtBin, true);
	assert(dir_qcdRegion_shape);
	std::cout << "dir_qcdRegion_shape = " << dir_qcdRegion_norm << ": name = " << dir_qcdRegion_shape->GetName() << std::endl;
	
	for ( vstring::const_iterator histogram = histograms.begin();
	      histogram != histograms.end(); ++histogram ) {
	  std::cout << "histogram = " << (*histogram) << std::endl;
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    TH1* histogramData_norm = getHistogram(dir_qcdRegion_norm, processData, *histogram, "central", true);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(Data_norm) = " << histogramData_norm->Integral() << std::endl;
	    }
	    
	    std::vector<TH1*> histogramsToSubtract_norm;
	    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
		  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
	      TH1* histogramToSubtract = getHistogram(dir_qcdRegion_norm, *processToSubtract, *histogram, *central_or_shift, false);
	      if ( !histogramToSubtract ) histogramToSubtract = getHistogram(dir_qcdRegion_norm, *processToSubtract, *histogram, "central", true);
	      if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
		std::cout << " integral(" << (*processToSubtract) << ") = " << histogramToSubtract->Integral() << std::endl;
	      }
	      histogramsToSubtract_norm.push_back(histogramToSubtract);
	    }
	    
	    TH1* histogramData_shape = getHistogram(dir_qcdRegion_shape, processData, *histogram, "central", true);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(Data_shape) = " << histogramData_shape->Integral() << std::endl;
	    }
	    
	    std::vector<TH1*> histogramsToSubtract_shape;
	    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
		  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
	      TH1* histogramToSubtract = getHistogram(dir_qcdRegion_shape, *processToSubtract, *histogram, *central_or_shift, false);
	      if ( !histogramToSubtract ) histogramToSubtract = getHistogram(dir_qcdRegion_shape, *processToSubtract, *histogram, "central", true);
	      if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
		std::cout << "integral(" << (*processToSubtract) << ") = " << histogramToSubtract->Integral() << std::endl;
	      }
	      histogramsToSubtract_shape.push_back(histogramToSubtract);
	    }

	    std::string subdirName_output = getSubdirNameOutput((*region)->name_, *category, *tauPtBin, processQCD);
	    TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	    subdir_output->cd();

	    std::string histogramNameQCD = processQCD;
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) histogramNameQCD.append("_").append(*central_or_shift);
	    histogramNameQCD.append("_").append(*histogram);
	    TH1* histogramQCD_norm = subtractHistograms(Form("%s_norm", histogramNameQCD.data()), histogramData_norm, histogramsToSubtract_norm);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(QCD_norm) = " << histogramQCD_norm->Integral() << std::endl;
	    }
	    TH1* histogramQCD_shape = subtractHistograms(histogramNameQCD.data(), histogramData_shape, histogramsToSubtract_shape);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(QCD_shape) = " << histogramQCD_shape->Integral() << std::endl;
	    }
	    double sfQCD_shape = ( histogramQCD_shape->Integral() > 0. ) ? (histogramQCD_norm->Integral()/histogramQCD_shape->Integral()) : 1.;
	    histogramQCD_shape->Scale(sfQCD_shape);
	    makeBinContentsPositive(histogramQCD_shape);	  
	  }
	}
      }
    }
  }
  
  delete inputFile;

  clock.Show("addBackgroundQCD");

  return 0;
}
