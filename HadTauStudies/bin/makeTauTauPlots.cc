
/** \executable makeTauTauPlots
 *
 * Make control plots (prefit and postfit) for MSSM -> tau tau -> tau_h tau_h analysis.
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
#include <THStack.h>
#include <TBenchmark.h>
#include <TMath.h>
#include "TPRegexp.h"
#include <TString.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TF1.h>
#include <TStyle.h>
#include <TROOT.h>

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;
typedef std::pair<double, double> pdouble; 

namespace
{
  struct regionEntryType
  {
    regionEntryType(const edm::ParameterSet& cfg)
    {
      name_ = cfg.getParameter<std::string>("name");
      label_ = cfg.getParameter<std::string>("label");
    }
    ~regionEntryType() {}
    std::string name_;
    std::string label_;
  };

  struct signalEntryType
  {
    signalEntryType(const edm::ParameterSet& cfg)
    {
      process_ggH_ = cfg.getParameter<std::string>("process_ggH");
      sf_ggH_ = cfg.getParameter<double>("sf_ggH");
      process_bbH_ = cfg.getParameter<std::string>("process_bbH");
      sf_bbH_ = cfg.getParameter<double>("sf_bbH");
      legendEntry_ = cfg.getParameter<std::string>("legendEntry");
    }
    ~signalEntryType() {}
    std::string process_ggH_;
    double sf_ggH_;
    std::string process_bbH_;
    double sf_bbH_;
    std::string legendEntry_;
  };

  struct categoryEntryType
  {
    categoryEntryType(const edm::ParameterSet& cfg)
    {
      name_ = cfg.getParameter<std::string>("name");
      label_ = cfg.getParameter<std::string>("label");
    }
    ~categoryEntryType() {}
    std::string name_;
    std::string label_;
  };

  struct tauPtBinEntryType
  {
    tauPtBinEntryType(const edm::ParameterSet& cfg)
    {
      name_ = cfg.getParameter<std::string>("name");
      label_ = cfg.getParameter<std::string>("label");
    }
    ~tauPtBinEntryType() {}
    std::string name_;
    std::string label_;
  };

  struct distributionEntryType
  {
    distributionEntryType(const edm::ParameterSet& cfg)
      : legendTextSize_(0.03),
	legendPosX_(0.585),
	legendPosY_(0.610),
	legendSizeX_(0.355),
	legendSizeY_(0.330)
    {
      histogramName_ = cfg.getParameter<std::string>("histogramName");
      xAxisTitle_ = cfg.getParameter<std::string>("xAxisTitle");
      yAxisTitle_ = cfg.getParameter<std::string>("yAxisTitle");
      if ( cfg.exists("keepBlinded") ) {
	vstring keepBlinded_string = cfg.getParameter<vstring>("keepBlinded");
	TPRegexp regexpParser_range("([0-9.e+/-]+):([0-9.e+/-]+)");
	for ( vstring::const_iterator keepBlinded_it = keepBlinded_string.begin();
	      keepBlinded_it != keepBlinded_string.end(); ++keepBlinded_it ) {
	  TObjArray* subStrings = regexpParser_range.MatchS(keepBlinded_it->data());
	  if ( subStrings->GetEntries() == 3 ) {
	    //std::cout << "substrings(1) = " << ((TObjString*)subStrings->At(1))->GetString() << std::endl;
	    double range_begin = ((TObjString*)subStrings->At(1))->GetString().Atof();
	    //std::cout << "substrings(2) = " << ((TObjString*)subStrings->At(2))->GetString() << std::endl;
	    double range_end = ((TObjString*)subStrings->At(2))->GetString().Atof();
	    keepBlinded_.push_back(pdouble(range_begin, range_end));
	  }
	}
      }      
    }
    ~distributionEntryType() {}
    std::string histogramName_;
    std::string xAxisTitle_;
    std::string yAxisTitle_;
    std::vector<pdouble> keepBlinded_;
    double legendTextSize_;
    double legendPosX_;
    double legendPosY_;
    double legendSizeX_;
    double legendSizeY_; 
  };
  
  TH1* getHistogramSignal(TDirectory* dir, 
			  const signalEntryType& signal, 
			  const std::string& histogramName, const std::string& central_or_shift, bool enableException)
  {
    TH1* histogramSignal_ggH = getHistogram(dir, signal.process_ggH_, histogramName, central_or_shift, enableException);
    TH1* histogramSignal_bbH = getHistogram(dir, signal.process_bbH_, histogramName, central_or_shift, enableException);
    if ( !(histogramSignal_ggH || histogramSignal_bbH) ) {
      if ( enableException ) {
	throw cms::Exception("getHistogramSignal") 
	  << "Failed to find histogram = " << histogramName << " in directory = " << dir->GetName() << " !!\n";    
      }
      return 0;
    }

    TH1* histogramSignal_ref = histogramSignal_ggH;
    if ( !histogramSignal_ref ) histogramSignal_ref = histogramSignal_bbH;

    std::string histogramNameSignal = Form("%s_plus_bbH", histogramSignal_ref->GetName());
    TH1* histogramSignal = (TH1*)histogramSignal_ref->Clone(histogramNameSignal.data());
    histogramSignal->Reset();
    if ( !histogramSignal->GetSumw2N() ) histogramSignal->Sumw2();
    if ( histogramSignal_ggH ) histogramSignal->Add(histogramSignal_ggH, signal.sf_ggH_);
    if ( histogramSignal_bbH ) histogramSignal->Add(histogramSignal_bbH, signal.sf_bbH_);
    return histogramSignal;
  }

  //-----------------------------------------------------------------------------
  void parseValue_and_Uncertainty(const std::string& valueErr_string, double& value, double& err)
  {
    //std::cout << "<parseValue_and_Uncertainty>:" << std::endl;
    //std::cout << " valueErr = " << valueErr_string << std::endl;
    TPRegexp regexpParser("([0-9.eE+-]+)\\s*\\+/-\\s*([0-9.eE+-]+)");
    TObjArray* subStrings = regexpParser.MatchS(valueErr_string.data());
    if ( subStrings->GetEntries() == 3 ) {
      value = ((TObjString*)subStrings->At(1))->GetString().Atof();
      //std::cout << " value = " << value << std::endl;
      err = ((TObjString*)subStrings->At(2))->GetString().Atof();
      //std::cout << " err = " << err << std::endl;
    } else throw cms::Exception("parseValue_and_Uncertainty") 
	<< "Failed to parse string = '" << valueErr_string << "' !!\n";    
  }
  void printHistogram(const TH1* histogram)
  {
    std::cout << "<printHistogram>:" << std::endl;
    std::cout << "name = " << histogram->GetName() << std::endl;
    int numBins = histogram->GetNbinsX();
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      std::cout << " bin #" << iBin << ": " << histogram->GetBinContent(iBin) << " +/- " << histogram->GetBinError(iBin) << std::endl;
    }
    std::cout << "integral = " << histogram->Integral() << std::endl;
  }
  double square(double x)
  {
    return (x*x);
  }
  struct HistogramManager
  {
    HistogramManager(const std::string& processZTT, const std::string& processZL, const std::string& processZJ, 
		     const std::string& processTT, 
		     const std::string& processW, const std::string& processVV, 
		     const std::string& processQCD,
		     const std::vector<categoryEntryType*>& categories,
		     const edm::ParameterSet& cfg)
      : currentDir_(0),	
	isUpToDate_(false),
	histogram_bgrUncertainty_(0)
    {
      processes_.push_back(processZTT);
      processes_.push_back(processZL);
      processes_.push_back(processZJ);
      processes_.push_back(processTT);
      processes_.push_back(processW);
      processes_.push_back(processVV);
      processes_.push_back(processQCD);

      edm::ParameterSet cfgNormalization = cfg.getParameter<edm::ParameterSet>("normalization");
      for ( vstring::const_iterator process = processes_.begin();
	    process != processes_.end(); ++process ) {
	edm::ParameterSet cfgNormalization_process = cfgNormalization.getParameter<edm::ParameterSet>(*process);
	for ( std::vector<categoryEntryType*>::const_iterator category = categories.begin();
	      category != categories.end(); ++category ) {
	  normEntryType processEntry;
	  processEntry.process_ = (*process);
	  processEntry.category_ = ((*category)->name_);
	  parseValue_and_Uncertainty(cfgNormalization_process.getParameter<std::string>(Form("category_%s", (*category)->name_.data())), processEntry.sf_, processEntry.sfErr_);
	  normalizationValues_and_Uncertainties_[*process][(*category)->name_] = processEntry;
	}
      }

      edm::ParameterSet cfgShape = cfg.getParameter<edm::ParameterSet>("shape");
      sysShifts_ = cfgShape.getParameterNamesForType<std::string>();
      for ( vstring::const_iterator sysShift = sysShifts_.begin();
	    sysShift != sysShifts_.end(); ++sysShift ) {
	shapeEntryType sysShiftEntry;
	sysShiftEntry.sysShift_ = (*sysShift);
	parseValue_and_Uncertainty(cfgShape.getParameter<std::string>(*sysShift), sysShiftEntry.value_, sysShiftEntry.err_);
	shapeValues_and_Uncertainties_[*sysShift] = sysShiftEntry;
      }
    }
    ~HistogramManager()
    {
      //for ( map_string_to_TH1::iterator it = histograms_postfit_.begin();
      //      it != histograms_postfit_.end(); ++it ) {
      //  delete it->second;
      //}
      //delete histogram_bgrUncertainty_;
    }

    void setDirectory(TDirectory* dir)
    {
      currentDir_ = dir;
      isUpToDate_ = false;
    }
    void setCategory(const std::string& category)
    {
      currentCategory_ = category;
      isUpToDate_ = false;
    }
    void setHistogram(const std::string& histogramName)
    {
      currentHistogramName_ = histogramName;
      isUpToDate_ = false;
    }

    TH1* getHistogramPrefit(const std::string& process, bool enableException)
    {
      if ( !isUpToDate_ ) update();
      TH1* histogram = 0;
      if ( histograms_prefit_.find(process) != histograms_prefit_.end() ) {
	histogram = histograms_prefit_[process];
      }
      if ( enableException && !histogram ) 
	throw cms::Exception("HistogramManager") 
	  << "Failed to find prefit histogram for process = " << process << " !!\n";    
      return histogram;
    }
    TH1* getHistogramPostfit(const std::string& process, bool enableException)
    {
      if ( !isUpToDate_ ) update();
      TH1* histogram = 0;
      if ( histograms_postfit_.find(process) != histograms_postfit_.end() ) {
	histogram = histograms_postfit_[process];
      }
      if ( enableException && !histogram ) 
	throw cms::Exception("HistogramManager") 
	  << "Failed to find postfit histogram for process = " << process << " !!\n";    
      return histogram;
    }
    TH1* getHistogramBgrUncertainty()
    {
      if ( !isUpToDate_ ) update();
      return histogram_bgrUncertainty_;
    }

    void getSF(const std::string& process, double& sf, double& sfErr)
    {
      assert(normalizationValues_and_Uncertainties_.find(process) != normalizationValues_and_Uncertainties_.end());
      assert(normalizationValues_and_Uncertainties_[process].find(currentCategory_) != normalizationValues_and_Uncertainties_[process].end());
      sf = normalizationValues_and_Uncertainties_[process][currentCategory_].sf_;
      sfErr = normalizationValues_and_Uncertainties_[process][currentCategory_].sfErr_;
    } 

    void update()
    {
      for ( vstring::const_iterator process = processes_.begin();
	    process != processes_.end(); ++process ) {
	std::cout << "process = " << (*process) << std::endl;

	TH1* histogram_prefit = getHistogram(currentDir_, *process, currentHistogramName_, "central", true);
	histograms_prefit_[*process] = histogram_prefit;
	//printHistogram(histograms_prefit_[*process]);
	
	for ( vstring::const_iterator sysShift = sysShifts_.begin();
	      sysShift != sysShifts_.end(); ++sysShift ) {
	  std::string sysShiftUp = Form("%sUp", sysShift->data());
	  TH1* histogram_sysShiftUp = getHistogram(currentDir_, *process, currentHistogramName_, sysShiftUp, false);
	  if ( histogram_sysShiftUp ) {
	    histograms_sysShifts_[*process][sysShiftUp] = histogram_sysShiftUp;
	  }
	  std::string sysShiftDown = Form("%sDown", sysShift->data());
	  TH1* histogram_sysShiftDown = getHistogram(currentDir_, *process, currentHistogramName_, sysShiftDown, false);
	  if ( histogram_sysShiftDown ) {
	    histograms_sysShifts_[*process][sysShiftDown] = histogram_sysShiftDown;
	  }
	}
	
	std::string histogramName_postfit = Form("%s_postfit", histogram_prefit->GetName());
	TH1* histogram_postfit = (TH1*)histogram_prefit->Clone(histogramName_postfit.data());
	if ( !histogram_postfit->GetSumw2N() ) histogram_postfit->Sumw2();
	if ( normalizationValues_and_Uncertainties_.find(*process) != normalizationValues_and_Uncertainties_.end() ) {
	  double sf, sfErr;
	  getSF(*process, sf, sfErr);
	  std::cout << "--> scaling histogram = " << histogram_postfit->GetName() << " by factor = " << sf << std::endl;
	  histogram_postfit->Scale(sf);
	}
	for ( vstring::const_iterator sysShift = sysShifts_.begin();
	      sysShift != sysShifts_.end(); ++sysShift ) {
	  std::string sysShiftUp = Form("%sUp", sysShift->data());
	  std::string sysShiftDown = Form("%sDown", sysShift->data());
	  if ( histograms_sysShifts_[*process].find(sysShiftUp)   != histograms_sysShifts_[*process].end() &&
	       histograms_sysShifts_[*process].find(sysShiftDown) != histograms_sysShifts_[*process].end() ) {
	    double sysShift_value = shapeValues_and_Uncertainties_[*sysShift].value_;
	    TH1* histogram_sysShift = 0;
	    if ( sysShift_value > 0. ) histogram_sysShift = histograms_sysShifts_[*process][sysShiftUp];
	    else if ( sysShift_value < 0. ) histogram_sysShift = histograms_sysShifts_[*process][sysShiftDown];
	    if ( histogram_sysShift ) {
	      double absSysShift_value = TMath::Abs(sysShift_value);
	      TH1* histogram_prefit = histograms_prefit_[*process];
	      assert(histogram_prefit);
	      int numBins = histogram_postfit->GetNbinsX();
	      for ( int iBin = 1; iBin <= numBins; ++iBin ) {
		double binContent_central = histogram_prefit->GetBinContent(iBin);
		double binContent_sysShift = histogram_sysShift->GetBinContent(iBin);
		double binContent_postfit = histogram_postfit->GetBinContent(iBin);
		binContent_postfit *= (binContent_central + absSysShift_value*(binContent_sysShift - binContent_central));
		if ( binContent_postfit < 0. ) binContent_postfit = 0.;
		histogram_postfit->SetBinContent(iBin, binContent_postfit);
	      }
	    }
	  }
	}
	delete histograms_postfit_[*process];
	histograms_postfit_[*process] = histogram_postfit;
	//printHistogram(histograms_postfit_[*process]);
      }
	
      delete histogram_bgrUncertainty_;
      std::string histogramName_bgrUncertainty = Form("%s_bgrUncertainty", currentHistogramName_.data());
      histogram_bgrUncertainty_ = (TH1*)histograms_prefit_.begin()->second->Clone(histogramName_bgrUncertainty.data());
      histogram_bgrUncertainty_->Reset();
      if ( !histogram_bgrUncertainty_->GetSumw2N() ) histogram_bgrUncertainty_->Sumw2();
      int numBins = histogram_bgrUncertainty_->GetNbinsX();
      for ( int iBin = 1; iBin <= numBins; ++iBin ) {
	double binContent_bgrUncertainty = 0.;
	double binErr2_bgrUncertainty = 0.;
	for ( vstring::const_iterator process = processes_.begin();
	      process != processes_.end(); ++process ) {
	  TH1* histogram_prefit = histograms_prefit_[*process];
	  assert(histogram_prefit);
	  double binContent_prefit = histogram_prefit->GetBinContent(iBin);
	  TH1* histogram_postfit = histograms_postfit_[*process];
	  assert(histogram_postfit);
	  double binContent_postfit = histogram_postfit->GetBinContent(iBin);
	  double binErr_postfit = histogram_postfit->GetBinError(iBin);
	  binContent_bgrUncertainty += binContent_postfit;
	  binErr2_bgrUncertainty += square(binErr_postfit); // statistical uncertainty of background process in given bin
	  double sf, sfErr;
	  getSF(*process, sf, sfErr);
	  if ( sf > 0. ) {
	    binErr2_bgrUncertainty += square(binContent_postfit*(sfErr/sf)); // systematic uncertainty on background normalization
	  }
	  for ( vstring::const_iterator sysShift = sysShifts_.begin();
		sysShift != sysShifts_.end(); ++sysShift ) {
	    double sysShift_value = shapeValues_and_Uncertainties_[*sysShift].value_;	    
	    TH1* histogram_sysShift = 0;
	    if ( sysShift_value > 0. ) {
	      std::string sysShiftUp = Form("%sUp", sysShift->data());
	      if ( histograms_sysShifts_[*process].find(sysShiftUp) != histograms_sysShifts_[*process].end() ) {
		histogram_sysShift = histograms_sysShifts_[*process][sysShiftUp];
	      }
	    } else if ( sysShift_value < 0. ) {
	      std::string sysShiftDown = Form("%sDown", sysShift->data());
	      if ( histograms_sysShifts_[*process].find(sysShiftDown) != histograms_sysShifts_[*process].end() ) {
		histogram_sysShift = histograms_sysShifts_[*process][sysShiftDown];
	      }
	    } 	    
	    if ( histogram_sysShift ) {
	      double sysShift_err = shapeValues_and_Uncertainties_[*sysShift].err_;
	      double binContent_sysShift = histogram_sysShift->GetBinContent(iBin);
	      if ( binContent_prefit > 0. ) {
		binErr2_bgrUncertainty += square(binContent_postfit*sysShift_err*(binContent_sysShift - binContent_prefit)/binContent_prefit); // systematic uncertainty on background shape
	      } else if ( binContent_postfit > 0. ) {
		binErr2_bgrUncertainty += square(sysShift_err*(binContent_sysShift - binContent_prefit));
	      }
	    }
	  }
	}
	histogram_bgrUncertainty_->SetBinContent(iBin, binContent_bgrUncertainty);
	assert(binErr2_bgrUncertainty >= 0.);
	histogram_bgrUncertainty_->SetBinError(iBin, TMath::Sqrt(binErr2_bgrUncertainty));
      }
      
      isUpToDate_ = true;
    }

    vstring processes_;
    struct normEntryType
    {     
      std::string process_;
      std::string category_;
      double sf_;
      double sfErr_;
    };
    typedef std::map<std::string, normEntryType> stringToNormEntryTypeMap;
    std::map<std::string, stringToNormEntryTypeMap> normalizationValues_and_Uncertainties_; // keys = process, category

    vstring sysShifts_;
    struct shapeEntryType
    {     
      std::string sysShift_;
      double value_;
      double err_;
    };
    std::map<std::string, shapeEntryType> shapeValues_and_Uncertainties_; // key = sysShift

    TDirectory* currentDir_;

    std::string currentCategory_;

    std::string currentHistogramName_;

    bool isUpToDate_;

    typedef std::map<std::string, TH1*> map_string_to_TH1;
    map_string_to_TH1 histograms_prefit_;  // key = process
    map_string_to_TH1 histograms_postfit_; // key = process
    std::map<std::string, map_string_to_TH1> histograms_sysShifts_; // key = process, sysShift
    TH1* histogram_bgrUncertainty_;
  };

  //-----------------------------------------------------------------------------

  TH1* blindHistogram(TH1* histogram, const std::vector<pdouble>& keepBlinded)
  {
    //std::cout << "<blindHistogram>:" << std::endl;
    std::string blindedHistogramName = Form("%s_blinded", histogram->GetName());
    TH1* blindedHistogram = (TH1*)histogram->Clone(blindedHistogramName.data());
    if ( !blindedHistogram->GetSumw2N() ) blindedHistogram->Sumw2();
    int numBins = histogram->GetNbinsX();
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      double x = histogram->GetBinCenter(iBin);
      bool isBlinded = false;
      for ( std::vector<pdouble>::const_iterator keepBlinded_it = keepBlinded.begin();
	    keepBlinded_it != keepBlinded.end(); ++keepBlinded_it ) {
	if ( x >= keepBlinded_it->first && x <= keepBlinded_it->second ) {
	  isBlinded = true;
	  break;
	}
      }
      //std::cout << "x = " << x << ": isBlinded = " << isBlinded << std::endl;
      if ( isBlinded ) {
	blindedHistogram->SetBinContent(iBin, -10.);
	blindedHistogram->SetBinError(iBin, 0.);
      }
    }
    //std::cout << "histogram: name = " << histogram->GetName() << ", integral = " << histogram->Integral() << std::endl;
    //std::cout << "histogram(blinded): name = " << blindedHistogram->GetName() << ", integral = " << blindedHistogram->Integral() << std::endl;
    return blindedHistogram;
  }

  TH1* divideHistogramByBinWidth(TH1* histogram)
  {
    std::string histogramDensityName = Form("%s_density", histogram->GetName());
    TH1* histogramDensity = (TH1*)histogram->Clone(histogramDensityName.data());
    TAxis* xAxis = histogram->GetXaxis();
    int numBins = xAxis->GetNbins();
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      double binContent = histogram->GetBinContent(iBin);
      double binError = histogram->GetBinError(iBin);
      double binWidth = xAxis->GetBinWidth(iBin);
      histogramDensity->SetBinContent(iBin, binContent/binWidth);
      histogramDensity->SetBinError(iBin, binError/binWidth);
    }
    return histogramDensity;
  }

  double compYmaxForClearance(TH1* histogram, double legendPosX, double legendPosY, double labelPosY, bool useLogScale, double numOrdersOfMagnitude)
  {
    TAxis* xAxis = histogram->GetXaxis();
    double xMin = xAxis->GetXmin();
    double xMax = xAxis->GetXmax();
    const double dX = 0.05;
    const double dY = 0.05;
    double x12 = xMin + (xMax - xMin)*legendPosX;
    //std::cout << "x12 = " << x12 << std::endl;
    double maxBinContent1 = 0.;
    double maxBinContent2 = 0.;
    int numBins = xAxis->GetNbins();
    for ( int iBin = 1; iBin <= numBins; ++iBin ) {
      double binContent = histogram->GetBinContent(iBin);
      if ( xAxis->GetBinLowEdge(iBin) < (x12 + dX) ) {
	if ( binContent > maxBinContent1 ) maxBinContent1 = binContent;
      }
      if ( xAxis->GetBinUpEdge(iBin) > (x12 - dX) ) {
	if ( binContent > maxBinContent2 ) maxBinContent2 = binContent;
      }
    }
    double yMaxForClearance = 0.;
    if ( useLogScale ) {
      double maxBinContent = TMath::Max(maxBinContent1, maxBinContent2);
      double logYmaxForClearance1 = 0.;      
      if ( maxBinContent1 > 0. && maxBinContent > 0. ) {
	logYmaxForClearance1 = (TMath::Log(maxBinContent1) - (TMath::Log(maxBinContent) - numOrdersOfMagnitude*TMath::Log(10.)))/(labelPosY - dY);
	logYmaxForClearance1 += (TMath::Log(maxBinContent) - numOrdersOfMagnitude*TMath::Log(10.));
      }
      double logYmaxForClearance2 = 0.;    
      if ( maxBinContent2 > 0. && maxBinContent > 0. ) {
	logYmaxForClearance2 = (TMath::Log(maxBinContent2) - (TMath::Log(maxBinContent) - numOrdersOfMagnitude*TMath::Log(10.)))/(legendPosY - dY);
	logYmaxForClearance2 += (TMath::Log(maxBinContent) - numOrdersOfMagnitude*TMath::Log(10.));
      }
      double logYmaxForClearance = TMath::Max(logYmaxForClearance1, logYmaxForClearance2);
      yMaxForClearance = TMath::Exp(logYmaxForClearance);
      //std::cout << "log-scale: maxBinContent1 = " << maxBinContent1 << ", maxBinContent2 = " << maxBinContent2 << " --> setting yMaxForClearance = " << yMaxForClearance << std::endl;
    } else {
      yMaxForClearance = TMath::Max(maxBinContent1/(labelPosY - dY), maxBinContent2/(legendPosY - dY));
      //std::cout << "linear scale: maxBinContent1 = " << maxBinContent1 << ", maxBinContent2 = " << maxBinContent2 << " --> setting yMaxForClearance = " << yMaxForClearance << std::endl;
    }
    return yMaxForClearance;
  }

  void makePlot(double canvasSizeX, double canvasSizeY,
		TH1* histogramSignal1, const std::string& legendEntrySignal1,
		TH1* histogramSignal2, const std::string& legendEntrySignal2,
		TH1* histogramSignal3, const std::string& legendEntrySignal3,
		TH1* histogramData, TH1* histogramData_blinded,
		TH1* histogramZTT,
		TH1* histogramTT,
		TH1* histogramEWK,
		TH1* histogramQCD,
		TH1* histogramBgrUncertainty,		
		double legendTextSize, double legendPosX, double legendPosY, double legendSizeX, double legendSizeY, 
		const std::string& labelOnTop,
		std::vector<std::string>& extraLabels, double labelTextSize,
		double labelPosX, double labelPosY, double labelSizeX, double labelSizeY,
	        const std::string& xAxisTitle, double xAxisOffset,
		bool useLogScale, const std::string& yAxisTitle, double yAxisOffset,
		const std::string& outputFileName)
  {
    TH1* histogramSignal1_density = 0;
    if ( histogramSignal1 ) {
      checkCompatibleBinning(histogramSignal1, histogramData);
      histogramSignal1_density = divideHistogramByBinWidth(histogramSignal1);
    }
    TH1* histogramSignal2_density = 0;
    if ( histogramSignal2 ) {
      checkCompatibleBinning(histogramSignal2, histogramData);
      histogramSignal2_density = divideHistogramByBinWidth(histogramSignal2);
    }
    TH1* histogramSignal3_density = 0;
    if ( histogramSignal3 ) {
      checkCompatibleBinning(histogramSignal3, histogramData);
      histogramSignal3_density = divideHistogramByBinWidth(histogramSignal3);
    }
    TH1* histogramData_density = divideHistogramByBinWidth(histogramData);
    checkCompatibleBinning(histogramData_blinded, histogramData);
    TH1* histogramData_blinded_density = divideHistogramByBinWidth(histogramData_blinded);
    checkCompatibleBinning(histogramZTT, histogramData);
    TH1* histogramZTT_density = divideHistogramByBinWidth(histogramZTT);
    checkCompatibleBinning(histogramTT, histogramData);
    TH1* histogramTT_density = divideHistogramByBinWidth(histogramTT);
    checkCompatibleBinning(histogramEWK, histogramData);
    TH1* histogramEWK_density = divideHistogramByBinWidth(histogramEWK);
    checkCompatibleBinning(histogramQCD, histogramData);
    TH1* histogramQCD_density = divideHistogramByBinWidth(histogramQCD);
    TH1* histogramBgrSum_density = (TH1*)histogramZTT_density->Clone("histogramBgrSum_density"); // CV: used for y-axis normalization only
    histogramBgrSum_density->Add(histogramTT_density);
    histogramBgrSum_density->Add(histogramEWK_density);
    histogramBgrSum_density->Add(histogramQCD_density);
    TH1* histogramBgrUncertainty_density = 0;
    if ( histogramBgrUncertainty ) {
      checkCompatibleBinning(histogramBgrUncertainty, histogramData);
      histogramBgrUncertainty_density = divideHistogramByBinWidth(histogramBgrUncertainty);
    }

    TCanvas* canvas = new TCanvas("canvas", "", canvasSizeX, canvasSizeY);
    canvas->SetFillColor(10);
    canvas->SetFillStyle(4000);
    canvas->SetFillColor(10);
    canvas->SetTicky();
    canvas->SetBorderSize(2);
    canvas->SetLeftMargin(0.12);
    canvas->SetBottomMargin(0.12);

    TPad* topPad = new TPad("topPad", "topPad", 0.00, 0.35, 1.00, 1.00);
    topPad->SetFillColor(10);
    topPad->SetTopMargin(0.04);
    topPad->SetLeftMargin(0.15);
    topPad->SetBottomMargin(0.03);
    topPad->SetRightMargin(0.05);
    topPad->SetLogy(useLogScale);

    TPad* bottomPad = new TPad("bottomPad", "bottomPad", 0.00, 0.00, 1.00, 0.35);
    bottomPad->SetFillColor(10);
    bottomPad->SetTopMargin(0.02);
    bottomPad->SetLeftMargin(0.15);
    bottomPad->SetBottomMargin(0.24);
    bottomPad->SetRightMargin(0.05);
    bottomPad->SetLogy(false);

    canvas->cd();
    topPad->Draw();
    topPad->cd();

    TAxis* xAxis_top = histogramData_blinded_density->GetXaxis();
    xAxis_top->SetTitle(xAxisTitle.data());
    xAxis_top->SetTitleOffset(xAxisOffset);
    xAxis_top->SetLabelColor(10);
    xAxis_top->SetTitleColor(10);
    
    TAxis* yAxis_top = histogramData_blinded_density->GetYaxis();
    yAxis_top->SetTitle(yAxisTitle.data());
    yAxis_top->SetTitleOffset(yAxisOffset);
    yAxis_top->SetTitleSize(0.05);
    yAxis_top->SetLabelSize(0.05);
    yAxis_top->SetTickLength(0.04);  

    TLegend* legend = new TLegend(legendPosX, legendPosY, legendPosX + legendSizeX, legendPosY + legendSizeY, NULL, "brNDC");
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->SetFillColor(10);
    legend->SetTextSize(legendTextSize);

    double yMax, yMin;
    if ( useLogScale ) {
      const double numOrdersOfMagnitude = 4.5;
      yMax = compYmaxForClearance(histogramData_density, legendPosX, legendPosY, labelPosY, true, numOrdersOfMagnitude);
      yMax = TMath::Max(yMax, compYmaxForClearance(histogramBgrSum_density, legendPosX, legendPosY, labelPosY, true, numOrdersOfMagnitude));
      if ( histogramSignal1_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal1_density, legendPosX, legendPosY, labelPosY, true, numOrdersOfMagnitude));
      if ( histogramSignal2_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal2_density, legendPosX, legendPosY, labelPosY, true, numOrdersOfMagnitude));
      if ( histogramSignal3_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal3_density, legendPosX, legendPosY, labelPosY, true, numOrdersOfMagnitude));
      yMin = TMath::Power(10., -numOrdersOfMagnitude)*TMath::Max(1., histogramData_density->GetBinContent(histogramData_density->GetMaximumBin()));
    } else {
      yMax = compYmaxForClearance(histogramData_density, legendPosX, legendPosY, labelPosY, false, -1.);
      yMax = TMath::Max(yMax, compYmaxForClearance(histogramBgrSum_density, legendPosX, legendPosY, labelPosY, false, -1.));
      if ( histogramSignal1_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal1_density, legendPosX, legendPosY, labelPosY, false, -1.));
      if ( histogramSignal2_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal2_density, legendPosX, legendPosY, labelPosY, false, -1.));
      if ( histogramSignal3_density ) yMax = TMath::Max(yMax, compYmaxForClearance(histogramSignal3_density, legendPosX, legendPosY, labelPosY, false, -1.));
      yMin = 0.;
    }

    histogramData_blinded_density->SetTitle("");
    histogramData_blinded_density->SetStats(false);
    histogramData_blinded_density->SetMaximum(yMax);
    histogramData_blinded_density->SetMinimum(yMin);
    histogramData_blinded_density->SetMarkerStyle(20);
    histogramData_blinded_density->SetMarkerSize(1.2);
    histogramData_blinded_density->SetMarkerColor(kBlack);
    histogramData_blinded_density->SetLineColor(kBlack);
    legend->AddEntry(histogramData_blinded_density, "observed", "p");

    histogramData_blinded_density->Draw("ep");

    histogramZTT_density->SetFillColor(kOrange - 4);
    legend->AddEntry(histogramZTT_density, "Z #rightarrow #tau#tau", "f");
    
    histogramTT_density->SetFillColor(kBlue - 8); 
    legend->AddEntry(histogramTT_density, "t#bar{t}", "f");
    
    histogramEWK_density->SetFillColor(kRed + 2);
    legend->AddEntry(histogramEWK_density, "Electroweak", "f");

    histogramQCD_density->SetFillColor(kMagenta - 10);
    legend->AddEntry(histogramQCD_density, "QCD", "f");

    THStack* histogramStack_density = new THStack("stack", "");
    histogramStack_density->Add(histogramQCD_density);
    histogramStack_density->Add(histogramTT_density);
    histogramStack_density->Add(histogramEWK_density);
    histogramStack_density->Add(histogramZTT_density);
    histogramStack_density->Draw("histsame");
        
    if ( histogramBgrUncertainty_density ) {
      int numBins_top = histogramBgrUncertainty_density->GetNbinsX();
      for ( int iBin = 1; iBin <= numBins_top; ++iBin ) {
	double sumBinContents = 0.;
	sumBinContents += histogramZTT_density->GetBinContent(iBin);
	sumBinContents += histogramTT_density->GetBinContent(iBin);
	sumBinContents += histogramEWK_density->GetBinContent(iBin);
	sumBinContents += histogramQCD_density->GetBinContent(iBin);
	histogramBgrUncertainty_density->SetBinContent(iBin, sumBinContents);
      }
      histogramBgrUncertainty_density->SetFillColor(kBlack);
      histogramBgrUncertainty_density->SetFillStyle(3344);    
      histogramBgrUncertainty_density->Draw("e2same");
      legend->AddEntry(histogramBgrUncertainty_density, "bkg. uncertainty", "f");
    }

    if ( histogramSignal1_density ) {      
      histogramSignal1_density->SetLineWidth(2);
      histogramSignal1_density->SetLineStyle(kDashed);
      histogramSignal1_density->SetLineColor(kBlue);
      histogramSignal1_density->Draw("histsame");
      legend->AddEntry(histogramSignal1_density, legendEntrySignal1.data(), "l");
    }
    if ( histogramSignal2_density ) {      
      histogramSignal2_density->SetLineWidth(2);
      histogramSignal2_density->SetLineStyle(kDashed);
      histogramSignal2_density->SetLineColor(kRed);
      histogramSignal2_density->Draw("histsame");
      legend->AddEntry(histogramSignal2_density, legendEntrySignal2.data(), "l");
    }
    if ( histogramSignal3_density ) {    
      histogramSignal3_density->SetLineWidth(2);
      histogramSignal3_density->SetLineStyle(kDashed);
      histogramSignal3_density->SetLineColor(kGreen + 2);
      histogramSignal3_density->Draw("histsame");
      legend->AddEntry(histogramSignal3_density, legendEntrySignal3.data(), "l");
    }

    histogramData_blinded_density->Draw("epsame");
    histogramData_blinded_density->Draw("axissame");

    legend->Draw();

    TPaveText* labelOnTop_pave = 0;
    if ( labelOnTop != "" ) {
      labelOnTop_pave = new TPaveText(0.135, 0.965, 0.71, 0.995, "brNDC");
      labelOnTop_pave->AddText(labelOnTop.data());
      labelOnTop_pave->SetFillColor(10);
      labelOnTop_pave->SetBorderSize(0);
      labelOnTop_pave->SetTextColor(1);
      labelOnTop_pave->SetTextAlign(12);
      labelOnTop_pave->SetTextSize(0.03);
      labelOnTop_pave->Draw();
    }
    TPaveText* extraLabels_pave = 0;
    if ( extraLabels.size() > 0 ) {
      extraLabels_pave = new TPaveText(labelPosX, labelPosY, labelPosX + labelSizeX, labelPosY + labelSizeY, "brNDC");
      for ( std::vector<std::string>::const_iterator extraLabel = extraLabels.begin();
	    extraLabel != extraLabels.end(); ++extraLabel ) {
	extraLabels_pave->AddText(extraLabel->data());
      }
      extraLabels_pave->SetFillColor(10);
      extraLabels_pave->SetBorderSize(0);
      extraLabels_pave->SetTextColor(1);
      extraLabels_pave->SetTextAlign(12);
      extraLabels_pave->SetTextSize(labelTextSize);
      extraLabels_pave->Draw();
    }

    canvas->cd();
    bottomPad->Draw();
    bottomPad->cd();
        
    TH1* histogramBgr = (TH1*)histogramData->Clone("histogramBgr");
    histogramBgr->Reset();
    if ( !histogramBgr->GetSumw2N() ) histogramBgr->Sumw2();
    histogramBgr->Add(histogramZTT);
    histogramBgr->Add(histogramTT);
    histogramBgr->Add(histogramEWK);
    histogramBgr->Add(histogramQCD);
    TH1* histogramRatio = (TH1*)histogramData_blinded->Clone("histogramRatio");
    if ( !histogramRatio->GetSumw2N() ) histogramRatio->Sumw2();
    checkCompatibleBinning(histogramRatio, histogramBgr);
    histogramRatio->Divide(histogramBgr);
    int numBins_bottom = histogramRatio->GetNbinsX();
    for ( int iBin = 1; iBin <= numBins_bottom; ++iBin ) {
      double binContent = histogramRatio->GetBinContent(iBin);
      if ( histogramData_blinded->GetBinContent(iBin) > 0. ) histogramRatio->SetBinContent(iBin, binContent - 1.0);
      else histogramRatio->SetBinContent(iBin, -10.);
    }

    histogramRatio->SetTitle("");
    histogramRatio->SetStats(false);
    histogramRatio->SetMinimum(-0.50);
    histogramRatio->SetMaximum(+0.50);
    histogramRatio->SetMarkerStyle(histogramData_blinded_density->GetMarkerStyle());
    histogramRatio->SetMarkerSize(histogramData_blinded_density->GetMarkerSize());
    histogramRatio->SetMarkerColor(histogramData_blinded_density->GetMarkerColor());
    histogramRatio->SetLineColor(histogramData_blinded_density->GetLineColor());

    TAxis* xAxis_bottom = histogramRatio->GetXaxis();
    xAxis_bottom->SetTitle(xAxis_top->GetTitle());
    xAxis_bottom->SetLabelColor(1);
    xAxis_bottom->SetTitleColor(1);
    xAxis_bottom->SetTitleOffset(1.20);
    xAxis_bottom->SetTitleSize(0.08);
    xAxis_bottom->SetLabelOffset(0.02);
    xAxis_bottom->SetLabelSize(0.08);
    xAxis_bottom->SetTickLength(0.055);
    
    TAxis* yAxis_bottom = histogramRatio->GetYaxis();
    yAxis_bottom->SetTitle("#frac{Data - Simulation}{Simulation}");
    yAxis_bottom->SetTitleOffset(0.80);
    yAxis_bottom->SetNdivisions(505);
    yAxis_bottom->CenterTitle();
    yAxis_bottom->SetTitleSize(0.08);
    yAxis_bottom->SetLabelSize(0.08);
    yAxis_bottom->SetTickLength(0.04);  

    TH1* histogramRatioUncertainty = 0;
    if ( histogramBgrUncertainty ) {
      histogramRatioUncertainty = (TH1*)histogramBgrUncertainty->Clone("histogramBgrUncertainty_bottom");
      if ( !histogramRatioUncertainty->GetSumw2N() ) histogramRatioUncertainty->Sumw2();
      checkCompatibleBinning(histogramRatioUncertainty, histogramBgr);
      histogramRatioUncertainty->Divide(histogramBgr);
      int numBins = histogramRatioUncertainty->GetNbinsX();
      for ( int iBin = 1; iBin <= numBins; ++iBin ) {
	double binContent = histogramRatioUncertainty->GetBinContent(iBin);
	histogramRatioUncertainty->SetBinContent(iBin, binContent - 1.0);
      }
      histogramRatioUncertainty->SetFillColor(histogramBgrUncertainty_density->GetFillColor());
      //histogramRatioUncertainty->SetFillStyle(histogramBgrUncertainty_density->GetFillStyle());    
      histogramRatioUncertainty->SetFillStyle(3644);    
    }
    histogramRatio->Draw("ep");

    TF1* line = new TF1("line","0", xAxis_bottom->GetXmin(), xAxis_bottom->GetXmax());
    line->SetLineStyle(3);
    line->SetLineWidth(1.5);
    line->SetLineColor(kBlack);
    line->Draw("same");

    if ( histogramRatioUncertainty ) {
      //printHistogram(histogramRatioUncertainty);
      histogramRatioUncertainty->Draw("e2same");
    }

    histogramRatio->Draw("epsame");

    canvas->Update();
    size_t idx = outputFileName.find(".");
    std::string outputFileName_plot(outputFileName, 0, idx);
    if ( useLogScale ) outputFileName_plot.append("_log");
    else outputFileName_plot.append("_linear");
    if ( idx != std::string::npos ) canvas->Print(std::string(outputFileName_plot).append(std::string(outputFileName, idx)).data());
    canvas->Print(std::string(outputFileName_plot).append(".png").data());
    canvas->Print(std::string(outputFileName_plot).append(".pdf").data());
    canvas->Print(std::string(outputFileName_plot).append(".root").data());
    
    delete histogramSignal1_density;
    delete histogramSignal2_density;
    delete histogramSignal3_density;
    delete histogramData_density;
    delete histogramData_blinded_density;
    delete histogramZTT_density;
    delete histogramTT_density;
    delete histogramEWK_density;
    delete histogramQCD_density;
    delete histogramBgrSum_density;
    delete histogramBgrUncertainty_density;
    delete legend;
    delete labelOnTop_pave;
    delete extraLabels_pave;
    delete topPad;
    delete histogramRatio;
    delete histogramBgr;
    delete line;
    delete bottomPad;    
    delete canvas;
  }
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<makeTauTauPlots>:" << std::endl;

  gROOT->SetBatch(true);

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("makeTauTauPlots");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("makeTauTauPlots") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgMakeTauTauPlots = cfg.getParameter<edm::ParameterSet>("makeTauTauPlots");

  std::vector<categoryEntryType*> categories;
  edm::VParameterSet cfgCategories = cfgMakeTauTauPlots.getParameter<edm::VParameterSet>("categories");
  for ( edm::VParameterSet::const_iterator cfgCategory = cfgCategories.begin();
	cfgCategory != cfgCategories.end(); ++cfgCategory ) {
    categoryEntryType* category = new categoryEntryType(*cfgCategory);
    categories.push_back(category);
  }

  std::vector<regionEntryType*> regions;
  edm::VParameterSet cfgRegions = cfgMakeTauTauPlots.getParameter<edm::VParameterSet>("regions");
  for ( edm::VParameterSet::const_iterator cfgRegion = cfgRegions.begin();
	cfgRegion != cfgRegions.end(); ++cfgRegion ) {
    regionEntryType* region = new regionEntryType(*cfgRegion);
    regions.push_back(region);
  }

  std::string processData = cfgMakeTauTauPlots.getParameter<std::string>("processData");
  std::string processZTT  = cfgMakeTauTauPlots.getParameter<std::string>("processZTT");
  std::string processZL   = cfgMakeTauTauPlots.getParameter<std::string>("processZL");
  std::string processZJ   = cfgMakeTauTauPlots.getParameter<std::string>("processZJ");
  std::string processTT   = cfgMakeTauTauPlots.getParameter<std::string>("processTT");
  std::string processW    = cfgMakeTauTauPlots.getParameter<std::string>("processW");
  std::string processVV   = cfgMakeTauTauPlots.getParameter<std::string>("processVV");
  std::string processQCD  = cfgMakeTauTauPlots.getParameter<std::string>("processQCD");

  signalEntryType* signal1 = ( cfgMakeTauTauPlots.exists("signal1") ) ? new signalEntryType(cfgMakeTauTauPlots.getParameter<edm::ParameterSet>("signal1")) : 0;
  signalEntryType* signal2 = ( cfgMakeTauTauPlots.exists("signal2") ) ? new signalEntryType(cfgMakeTauTauPlots.getParameter<edm::ParameterSet>("signal2")) : 0;
  signalEntryType* signal3 = ( cfgMakeTauTauPlots.exists("signal3") ) ? new signalEntryType(cfgMakeTauTauPlots.getParameter<edm::ParameterSet>("signal3")) : 0;
  
  std::vector<tauPtBinEntryType*> tauPtBins;
  edm::VParameterSet cfgTauPtBins = cfgMakeTauTauPlots.getParameter<edm::VParameterSet>("tauPtBins");
  for ( edm::VParameterSet::const_iterator cfgTauPtBin = cfgTauPtBins.begin();
	cfgTauPtBin != cfgTauPtBins.end(); ++cfgTauPtBin ) {
    tauPtBinEntryType* tauPtBin = new tauPtBinEntryType(*cfgTauPtBin);
    tauPtBins.push_back(tauPtBin);
  }
  vstring tauPtBinsToPlot = cfgMakeTauTauPlots.getParameter<vstring>("tauPtBinsToPlot");

  std::vector<distributionEntryType*> distributions;
  edm::VParameterSet cfgDistributions = cfgMakeTauTauPlots.getParameter<edm::VParameterSet>("distributions");
  for ( edm::VParameterSet::const_iterator cfgDistribution = cfgDistributions.begin();
	cfgDistribution != cfgDistributions.end(); ++cfgDistribution ) {
    distributionEntryType* distribution = new distributionEntryType(*cfgDistribution);
    distributions.push_back(distribution);
  }

  edm::ParameterSet cfgNuisanceParameters = cfgMakeTauTauPlots.getParameter<edm::ParameterSet>("nuisanceParameters");
  HistogramManager histogramManager(
    processZTT, processZL, processZJ, 
    processTT, 
    processW, processVV, 
    processQCD, 
    categories,
    cfgNuisanceParameters);
  bool showBgrUncertainty = cfgMakeTauTauPlots.getParameter<bool>("showBgrUncertainty");

  std::string labelOnTop_string = cfgMakeTauTauPlots.getParameter<std::string>("labelOnTop");
  double intLumiData = cfgMakeTauTauPlots.getParameter<double>("intLumiData");
  std::string labelOnTop = Form(labelOnTop_string.data(), intLumiData);
  
  std::string outputFileName = cfgMakeTauTauPlots.getParameter<std::string>("outputFileName");

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("makeTauTauPlots") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());
  
  for ( std::vector<regionEntryType*>::iterator region = regions.begin();
	region != regions.end(); ++region ) {
    for ( std::vector<tauPtBinEntryType*>::iterator tauPtBin = tauPtBins.begin();
	  tauPtBin != tauPtBins.end(); ++tauPtBin ) {
      bool isToPlot = false;
      for ( vstring::const_iterator tauPtBinToPlot = tauPtBinsToPlot.begin();
	    tauPtBinToPlot != tauPtBinsToPlot.end(); ++tauPtBinToPlot ) {
	if ( (*tauPtBinToPlot) == (*tauPtBin)->name_ ) {
	  isToPlot = true;
	  break;
	}
      }
      if ( !isToPlot ) continue;
      for ( std::vector<categoryEntryType*>::iterator category = categories.begin();
	    category != categories.end(); ++category ) {
	std::cout << "processing region = " << (*region)->name_ << ", category = " << (*category)->name_ << ", tauPtBin = " << (*tauPtBin)->name_ << std::endl;

	TDirectory* dir = getDirectory(inputFile, (*region)->name_, (*category)->name_, (*tauPtBin)->name_, true);
	assert(dir);

	for ( std::vector<distributionEntryType*>::iterator distribution = distributions.begin();
	      distribution != distributions.end(); ++distribution ) {
	  TH1* histogramSignal1 = ( signal1 ) ? getHistogramSignal(dir, *signal1, (*distribution)->histogramName_, "central", false) : 0;  
	  std::string legendEntrySignal1 = ( signal1 ) ? signal1->legendEntry_ : "";
	  TH1* histogramSignal2 = ( signal2 ) ? getHistogramSignal(dir, *signal2, (*distribution)->histogramName_, "central", false) : 0;  
	  std::string legendEntrySignal2 = ( signal2 ) ? signal2->legendEntry_ : "";
	  TH1* histogramSignal3 = ( signal3 ) ? getHistogramSignal(dir, *signal3, (*distribution)->histogramName_, "central", false) : 0;  
	  std::string legendEntrySignal3 = ( signal3 ) ? signal3->legendEntry_ : "";

	  TH1* histogramData = getHistogram(dir, processData, (*distribution)->histogramName_, "central", true);

	  histogramManager.setDirectory(dir);
	  histogramManager.setCategory((*category)->name_);
	  histogramManager.setHistogram((*distribution)->histogramName_);
	  histogramManager.update();

	  //TH1* histogramZTT = histogramManager.getHistogramPrefit(processZTT, true);
	  //TH1* histogramTT  = histogramManager.getHistogramPrefit(processTT,  true); 
	  //TH1* histogramW   = histogramManager.getHistogramPrefit(processW,   true);
	  //TH1* histogramZL  = histogramManager.getHistogramPrefit(processZL,  true);
	  //TH1* histogramZJ  = histogramManager.getHistogramPrefit(processZJ,  true);
	  //TH1* histogramVV  = histogramManager.getHistogramPrefit(processVV,  true);
	  //TH1* histogramQCD = histogramManager.getHistogramPrefit(processQCD, true);
	  TH1* histogramZTT = histogramManager.getHistogramPostfit(processZTT, true);
	  TH1* histogramTT  = histogramManager.getHistogramPostfit(processTT,  true); 
	  TH1* histogramW   = histogramManager.getHistogramPostfit(processW,   true);
	  TH1* histogramZL  = histogramManager.getHistogramPostfit(processZL,  true);
	  TH1* histogramZJ  = histogramManager.getHistogramPostfit(processZJ,  true);
	  TH1* histogramVV  = histogramManager.getHistogramPostfit(processVV,  true);
	  TH1* histogramQCD = histogramManager.getHistogramPostfit(processQCD, true);

	  std::string histogramNameEWK = Form("%s_plus_ZL_plus_ZJ_plus_VV", histogramW->GetName());
	  TH1* histogramEWK = (TH1*)histogramW->Clone(histogramNameEWK.data());
	  histogramEWK->Add(histogramZL);
	  histogramEWK->Add(histogramZJ);
	  histogramEWK->Add(histogramVV);

	  TH1* histogramData_blinded = ( (*distribution)->keepBlinded_.size() >= 1 ) ? blindHistogram(histogramData, (*distribution)->keepBlinded_) : histogramData;	  

	  TH1* histogramBgrUncertainty = 0;
	  if ( showBgrUncertainty ) {
	    histogramBgrUncertainty = histogramManager.getHistogramBgrUncertainty();
	  }
	  
	  vstring extraLabels;
	  if ( (*region)->label_   != "" ) extraLabels.push_back(Form("#tau_{h}#tau_{h} (%s)", (*region)->label_.data()));
	  else extraLabels.push_back("#tau_{h}#tau_{h}");
	  if ( (*category)->label_ != "" ) extraLabels.push_back((*category)->label_);
	  if ( (*tauPtBin)->label_ != "" ) extraLabels.push_back((*tauPtBin)->label_);
	  double extraLabelsSizeX = 0.;
	  if ( (*tauPtBin)->label_ != "" ) extraLabelsSizeX = 0.20;
	  else extraLabelsSizeX = 0.12;

	  size_t idx = outputFileName.find(".");
	  std::string outputFileName_plot(outputFileName, 0, idx);
	  outputFileName_plot.append(Form("_%s_%s_%s", (*region)->name_.data(), (*category)->name_.data(), (*tauPtBin)->name_.data()));
	  outputFileName_plot.append(Form("_%s", (*distribution)->histogramName_.data()));
	  if ( idx != std::string::npos ) outputFileName_plot.append(std::string(outputFileName, idx));
	  
	  makePlot(800, 900,
		   histogramSignal1, legendEntrySignal1,
		   histogramSignal2, legendEntrySignal2,
		   histogramSignal3, legendEntrySignal3,
		   histogramData, histogramData_blinded,
		   histogramZTT,
		   histogramTT,
		   histogramEWK,
		   histogramQCD,
		   histogramBgrUncertainty,
		   (*distribution)->legendTextSize_, (*distribution)->legendPosX_, (*distribution)->legendPosY_, (*distribution)->legendSizeX_, (*distribution)->legendSizeY_, 
		   labelOnTop,
		   extraLabels, 0.040, 0.175, 0.94 - 0.055*extraLabels.size(), extraLabelsSizeX, 0.055*extraLabels.size(),
		   (*distribution)->xAxisTitle_, 1.30, 
		   true, (*distribution)->yAxisTitle_, 1.25,
		   outputFileName_plot);
	  makePlot(800, 900,
		   histogramSignal1, legendEntrySignal1,
		   histogramSignal2, legendEntrySignal2,
		   histogramSignal3, legendEntrySignal3,
		   histogramData, histogramData_blinded,
		   histogramZTT,
		   histogramTT,
		   histogramEWK,
		   histogramQCD,
		   histogramBgrUncertainty,
		   (*distribution)->legendTextSize_, (*distribution)->legendPosX_, (*distribution)->legendPosY_, (*distribution)->legendSizeX_, (*distribution)->legendSizeY_, 
		   labelOnTop,
		   extraLabels, 0.040, 0.175, 0.94 - 0.055*extraLabels.size(), extraLabelsSizeX, 0.055*extraLabels.size(),
		   (*distribution)->xAxisTitle_, 1.30, 
		   false, (*distribution)->yAxisTitle_, 1.25,
		   outputFileName_plot);

	  if ( histogramData_blinded != histogramData ) delete histogramData_blinded;
	  delete histogramSignal1;
	  delete histogramSignal2;
	  delete histogramSignal3;	  
	}
      }
    }
  }

  delete inputFile;
  
  for ( std::vector<categoryEntryType*>::iterator it = categories.begin();
	it != categories.end(); ++it ) {
    delete (*it);
  }
  for ( std::vector<regionEntryType*>::iterator it = regions.begin();
	it != regions.end(); ++it ) {
    delete (*it);
  }
  for ( std::vector<tauPtBinEntryType*>::iterator it = tauPtBins.begin();
	it != tauPtBins.end(); ++it ) {
    delete (*it);
  }
  for ( std::vector<distributionEntryType*>::iterator it = distributions.begin();
	it != distributions.end(); ++it ) {
    delete (*it);
  }

  clock.Show("makeTauTauPlots");

  return 0;
}

