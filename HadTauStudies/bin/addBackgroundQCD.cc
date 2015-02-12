
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

  struct addBinByBinUncertaintyEntryType
  {
    addBinByBinUncertaintyEntryType(const edm::ParameterSet& cfg)
      : add_(0.),
	addDerr_(0.),
	min_(0.)
    {
      vstring tauPtBins_vector = cfg.getParameter<vstring>("tauPtBins");
      for ( vstring::const_iterator tauPtBin = tauPtBins_vector.begin();
	    tauPtBin != tauPtBins_vector.end(); ++tauPtBin ) {
	tauPtBins_.insert(*tauPtBin);
      }
      if ( cfg.exists("add")     ) add_     = cfg.getParameter<double>("add");
      if ( cfg.exists("addDerr") ) addDerr_ = cfg.getParameter<double>("addDerr");
      if ( cfg.exists("min")     ) min_     = cfg.getParameter<double>("min");      
      std::string range_string = cfg.getParameter<std::string>("range");
      TPRegexp regexpParser_range("([0-9.e+/-]+):([0-9.e+/-]+)");
      TObjArray* subStrings = regexpParser_range.MatchS(range_string.data());
      if ( subStrings->GetEntries() == 3 ) {
	//std::cout << "substrings(1) = " << ((TObjString*)subStrings->At(1))->GetString() << std::endl;
	range_begin_ = ((TObjString*)subStrings->At(1))->GetString().Atof();
	//std::cout << "substrings(2) = " << ((TObjString*)subStrings->At(2))->GetString() << std::endl;
	range_end_ = ((TObjString*)subStrings->At(2))->GetString().Atof();
      } else throw cms::Exception("addBinByBinUncertaintyEntryType") 
	  << "Failed to parse string = '" << range_string << "' !!\n";   
    }
    ~addBinByBinUncertaintyEntryType() {}
    std::set<std::string> tauPtBins_;
    double add_;
    double addDerr_;
    double min_;
    double range_begin_;
    double range_end_;
  };

  double square(double x)
  {
    return (x*x);
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

  std::vector<addBinByBinUncertaintyEntryType*> addBinByBinUncertainties;
  if ( cfgAddBackgroundQCD.exists("addBinByBinUncertainties") ) {
    edm::VParameterSet cfgAddBinByBinUncertainties = cfgAddBackgroundQCD.getParameter<edm::VParameterSet>("addBinByBinUncertainties");
    for ( edm::VParameterSet::const_iterator cfgAddBinByBinUncertainty = cfgAddBinByBinUncertainties.begin();
	  cfgAddBinByBinUncertainty != cfgAddBinByBinUncertainties.end(); ++cfgAddBinByBinUncertainty ) {
      vstring ranges = cfgAddBinByBinUncertainty->getParameter<vstring>("ranges");
      for ( vstring::const_iterator range = ranges.begin();
	    range != ranges.end(); ++range ) {
	edm::ParameterSet cfgAddBinByBinUncertainty_range(*cfgAddBinByBinUncertainty);
	cfgAddBinByBinUncertainty_range.addParameter<std::string>("range", *range);
	addBinByBinUncertaintyEntryType* addBinByBinUncertainty = new addBinByBinUncertaintyEntryType(cfgAddBinByBinUncertainty_range);
	addBinByBinUncertainties.push_back(addBinByBinUncertainty);
      }
    }
  }
  
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
	
	std::set<std::string> histograms;
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

	    int verbosity = ( histogram->find("svFitMass") != std::string::npos && ((*central_or_shift) == "" || (*central_or_shift) == "central") ) ? 1 : 0;

	    std::string central_or_shiftData = "central";
	    if ( central_or_shift->find("CMS_htt_QCDfr") != std::string::npos ) central_or_shiftData = (*central_or_shift);
	    TH1* histogramData_norm = getHistogram(dir_region_norm, processData, *histogram, central_or_shiftData, true);
	    if ( verbosity ) {
	      std::cout << " integral(Data_norm) = " << histogramData_norm->Integral() << std::endl;
	    }
	    
	    std::vector<TH1*> histogramsToSubtract_norm;
	    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
		  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
	      TH1* histogramToSubtract = getHistogram(dir_region_norm, *processToSubtract, *histogram, *central_or_shift, false);
	      if ( !histogramToSubtract ) histogramToSubtract = getHistogram(dir_region_norm, *processToSubtract, *histogram, "central", true);
	      if ( verbosity ) {
		std::cout << " integral(" << (*processToSubtract) << ") = " << histogramToSubtract->Integral() << std::endl;
	      }
	      histogramsToSubtract_norm.push_back(histogramToSubtract);
	    }
	    
	    TH1* histogramData_shape = getHistogram(dir_region_shape, processData, *histogram, "central", true);
	    if ( verbosity ) {
	      std::cout << " integral(Data_shape) = " << histogramData_shape->Integral() << std::endl;
	    }
	    
	    std::vector<TH1*> histogramsToSubtract_shape;
	    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
		  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
	      TH1* histogramToSubtract = getHistogram(dir_region_shape, *processToSubtract, *histogram, *central_or_shift, false);
	      if ( !histogramToSubtract ) histogramToSubtract = getHistogram(dir_region_shape, *processToSubtract, *histogram, "central", true);
	      if ( verbosity ) {
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
	    TH1* histogramQCD_norm = subtractHistograms(Form("%s_norm", histogramNameQCD.data()), histogramData_norm, histogramsToSubtract_norm, verbosity);
	    if ( verbosity ) {
	      std::cout << " integral(QCD_norm) = " << histogramQCD_norm->Integral() << std::endl;
	    }
	    TH1* histogramQCD_shape = subtractHistograms(histogramNameQCD.data(), histogramData_shape, histogramsToSubtract_shape, verbosity);
	    if ( verbosity ) {
	      std::cout << " integral(QCD_shape) = " << histogramQCD_shape->Integral() << std::endl;
	    }
	    double sfQCD_shape = ( histogramQCD_shape->Integral() > 0. ) ? (histogramQCD_norm->Integral()/histogramQCD_shape->Integral()) : 1.;
	    histogramQCD_shape->Scale(sfQCD_shape);
	    makeBinContentsPositive(histogramQCD_shape, verbosity);	  

	    // CV: add 10% extra bin-by-bin uncertainty for SVfit mass range 100-150 GeV,
	    //     where QCD shape template is subject to "turn-on" effects which are difficult to model;
	    //     cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#TAU_TAU_channel
	    if ( std::string(histogramQCD_shape->GetName()).find("svFitMass") != std::string::npos ) {
	      int numBins = histogramQCD_shape->GetNbinsX();
	      for ( int iBin = 1; iBin <= numBins; ++iBin ) {
		double binCenter = histogramQCD_shape->GetBinCenter(iBin);
		double binContent = histogramQCD_shape->GetBinContent(iBin);
		double binError = histogramQCD_shape->GetBinError(iBin);
		double binErr2_add = square(binError);
		double binError_min = binError;
		for ( std::vector<addBinByBinUncertaintyEntryType*>::const_iterator addBinByBinUncertainty = addBinByBinUncertainties.begin();
		      addBinByBinUncertainty != addBinByBinUncertainties.end(); ++addBinByBinUncertainty ) {
		  if ( (*addBinByBinUncertainty)->tauPtBins_.find(*tauPtBin) == (*addBinByBinUncertainty)->tauPtBins_.end() ) continue;
		  if ( binCenter > (*addBinByBinUncertainty)->range_begin_ && binCenter < (*addBinByBinUncertainty)->range_end_ ) {
		    if ( (*addBinByBinUncertainty)->add_ > 0. ) {
		      binErr2_add += square((*addBinByBinUncertainty)->add_*binContent);
		    }
		    std::cout << "bin #" << iBin << " (binCenter = " << binCenter << "): binContent = " << binContent << ", sqrt(binErr2_add) = " << TMath::Sqrt(binErr2_add) << std::endl;
		    if ( (*addBinByBinUncertainty)->addDerr_ > 0. ) {
		      double binErr2_addDerr = 0.;
		      int numBins_addDerr = 0;
		      if ( iBin >= 2 ) {
			double binCenter_left = histogramQCD_shape->GetBinCenter(iBin - 1);
			double binContent_left = histogramQCD_shape->GetBinContent(iBin - 1);
			binErr2_addDerr += square((*addBinByBinUncertainty)->addDerr_*TMath::Abs(binContent - binContent_left)/TMath::Max(1., TMath::Abs(binCenter - binCenter_left)));
			++numBins_addDerr;
		      }
		      if ( iBin <= (numBins - 1) ) {
			double binCenter_right = histogramQCD_shape->GetBinCenter(iBin + 1);
			double binContent_right = histogramQCD_shape->GetBinContent(iBin + 1);
			binErr2_addDerr += square((*addBinByBinUncertainty)->addDerr_*TMath::Abs(binContent - binContent_right)/TMath::Max(1., TMath::Abs(binCenter - binCenter_right)));
			++numBins_addDerr;
		      }
		      std::cout << " sqrt(binErr2_addDerr) = " << TMath::Sqrt(binErr2_addDerr) << std::endl;
		      if ( numBins_addDerr > 0 ) {
			binErr2_add += (binErr2_addDerr/numBins_addDerr);
			std::cout << "--> setting sqrt(binErr2_add) = " << TMath::Sqrt(binErr2_add) << std::endl;
		      }
		    }
		    if ( binError_min < ((*addBinByBinUncertainty)->min_*binContent) ) {
		      binError_min = (*addBinByBinUncertainty)->min_*binContent;
		    }
		  }
		}
		assert(binErr2_add >= 0.);
		double binError_modified = TMath::Max(TMath::Sqrt(binErr2_add), binError_min);
		histogramQCD_shape->SetBinError(iBin, binError_modified);
	      }
	    }	    
	  }
	}
      }
    }
  }
  
  delete inputFile;

  for ( std::vector<addBinByBinUncertaintyEntryType*>::iterator it = addBinByBinUncertainties.begin();
	it != addBinByBinUncertainties.end(); ++it ) {
    delete (*it);
  }

  clock.Show("addBackgroundQCD");

  return 0;
}
