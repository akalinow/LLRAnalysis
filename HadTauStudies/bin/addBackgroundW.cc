
/** \executable addBackgroundW
 *
 * Take shape template for W+jets in b-tag category from relaxed b-tag selection
 * and scale it according to normalization obtained for tight b-tag selection.
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
#include <set>
#include <assert.h>

typedef std::vector<std::string> vstring;

namespace
{
  struct regionEntryType
  {
    regionEntryType(const edm::ParameterSet& cfg, const vstring& categories)
    {
      name_ = cfg.getParameter<std::string>("name");
      edm::ParameterSet cfgRegion_norm = cfg.getParameter<edm::ParameterSet>("region_norm");
      edm::ParameterSet cfgRegion_shape = cfg.getParameter<edm::ParameterSet>("region_shape");
      for ( vstring::const_iterator category = categories.begin();
	    category != categories.end(); ++category ) {
	region_norm_[*category] = cfgRegion_norm.getParameter<std::string>(Form("category_%s", category->data()));
	region_shape_[*category] = cfgRegion_shape.getParameter<std::string>(Form("category_%s", category->data()));
      }
    }
    ~regionEntryType() {}
    std::string name_;
    std::map<std::string, std::string> region_norm_;  // key = category
    std::map<std::string, std::string> region_shape_; // key = category
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

  std::cout << "<addBackgroundW>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("addBackgroundW");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("addBackgroundW") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";
  
  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgAddBackgroundW = cfg.getParameter<edm::ParameterSet>("addBackgroundW");
  
  vstring categories = cfgAddBackgroundW.getParameter<vstring>("categories");

  std::vector<regionEntryType*> regions;
  edm::VParameterSet cfgRegions = cfgAddBackgroundW.getParameter<edm::VParameterSet>("regions");
  for ( edm::VParameterSet::const_iterator cfgRegion = cfgRegions.begin();
	cfgRegion != cfgRegions.end(); ++cfgRegion ) {
    regionEntryType* region = new regionEntryType(*cfgRegion, categories);
    regions.push_back(region);
  }

  std::string processW_tmp = cfgAddBackgroundW.getParameter<std::string>("processW_tmp");
  std::string processW = cfgAddBackgroundW.getParameter<std::string>("processW");

  vstring tauPtBins = cfgAddBackgroundW.getParameter<vstring>("tauPtBins");

  vstring central_or_shifts = cfgAddBackgroundW.getParameter<vstring>("sysShifts");
  central_or_shifts.push_back(""); // CV: add central value
  
  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("addBackgroundW") 
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
	
	std::set<std::string> histograms;
	TDirectory* dirW_tmp = dynamic_cast<TDirectory*>(dir->Get(processW_tmp.data()));
	if ( !dirW_tmp ) throw cms::Exception("addBackgroundW")  
	  << "Failed to find directory for process = " << processW_tmp << " !!\n";
	TList* list = dirW_tmp->GetListOfKeys();
	TIter next(list);
	TKey* key = 0;
	while ( (key = dynamic_cast<TKey*>(next())) ) {
	  TObject* object = key->ReadObj();
	  TH1* histogram = dynamic_cast<TH1*>(object);
	  if ( !histogram ) continue;
	  TString histogramName = TString(histogram->GetName()).ReplaceAll(Form("%s_", processW_tmp.data()), "");
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) {
	      histogramName = histogramName.ReplaceAll(Form("%s_", central_or_shift->data()), "");
	    }
	  }
	  if ( histogramName.Contains("CMS_") ) continue;
	  if ( histograms.find(histogramName.Data()) == histograms.end() ) {
	    std::cout << "adding histogram = " << histogramName.Data() << std::endl;
	    histograms.insert(histogramName.Data());
	  }
	}

	TDirectory* dir_region_norm = getDirectory(inputFile, (*region)->region_norm_[*category], *category, *tauPtBin, true);
	assert(dir_region_norm);
	std::cout << "dir_region_norm = " << dir_region_norm << ": name = " << dir_region_norm->GetName() << std::endl;
	TDirectory* dir_region_shape = getDirectory(inputFile, (*region)->region_shape_[*category], *category, *tauPtBin, true);
	assert(dir_region_shape);
	std::cout << "dir_region_shape = " << dir_region_norm << ": name = " << dir_region_shape->GetName() << std::endl;
	
	for ( std::set<std::string>::const_iterator histogram = histograms.begin();
	      histogram != histograms.end(); ++histogram ) {
	  std::cout << "histogram = " << (*histogram) << std::endl;
	  for ( vstring::const_iterator central_or_shift = central_or_shifts.begin();
		central_or_shift != central_or_shifts.end(); ++central_or_shift ) {
	    TH1* histogramW_tmp_norm = getHistogram(dir_region_norm, processW_tmp, *histogram, "central", true);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(W_tmp_norm) = " << histogramW_tmp_norm->Integral() << std::endl;
	    }
	    
	    TH1* histogramW_tmp_shape = getHistogram(dir_region_shape, processW_tmp, *histogram, "central", true);
	    if ( (*central_or_shift) == "" || (*central_or_shift) == "central" ) {
	      std::cout << " integral(W_tmp_shape) = " << histogramW_tmp_shape->Integral() << std::endl;
	    }

	    std::string subdirName_output = getSubdirNameOutput((*region)->name_, *category, *tauPtBin, processW);
	    TDirectory* subdir_output = createSubdirectory_recursively(fs, subdirName_output);
	    subdir_output->cd();

	    std::string histogramNameW = processW;
	    if ( !((*central_or_shift) == "" || (*central_or_shift) == "central") ) histogramNameW.append("_").append(*central_or_shift);
	    histogramNameW.append("_").append(*histogram);
	    TH1* histogramW = (TH1*)histogramW_tmp_shape->Clone(histogramNameW.data());
	    double sfW = ( histogramW->Integral() > 0. ) ? (histogramW_tmp_norm->Integral()/histogramW_tmp_shape->Integral()) : 1.;
	    histogramW->Scale(sfW);
	  }
	}
      }
    }
  }
  
  delete inputFile;

  clock.Show("addBackgroundW");

  return 0;
}
