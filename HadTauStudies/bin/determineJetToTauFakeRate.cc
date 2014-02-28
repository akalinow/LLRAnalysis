
/** \executable determineJetToTauFakeRate
 *
 * Determine ratio of isolated to anti-isolated tau candidates in same-sign (SS) tau_h tau_h events
 * as function of tau Pt and apply ratio to correction to events in OS anti-isolated region,
 * in order to obtain QCD shape template for signal (OS isolated) region of MSSM -> tau tau -> tau_h tau_h analysis.
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
#include <TGraphAsymmErrors.h>

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

  //-------------------------------------------------------------------------------
  void getBinomialBounds(Int_t n, Int_t r, Float_t& rMin, Float_t& rMax)
  {
    rMin = 0.;
    rMax = 0.;
    
    if ( n == 0 ){
      return;
    }
    if ( r < 0 ){
      std::cerr << "Error in <getBinomialBounds>: n = " << n << ", r = " << r << std::endl;
      return;
    }
    
    if ( ((Double_t)r*(n - r)) > (9.*n) ){
      rMin = r - TMath::Sqrt((Double_t)r*(n - r)/((Double_t)n));
      rMax = r + TMath::Sqrt((Double_t)r*(n - r)/((Double_t)n));
      return;
    }
    
    Double_t binomialCoefficient = 1.;
    
    Double_t rMinLeft       = 0.;
    Double_t rMinMiddle     = TMath::Max(0.5*r, n - 1.5*r);
    Double_t rMinRight      = n;
    Double_t rMinLeftProb   = 0.;
    Double_t rMinMiddleProb = 0.5;
    Double_t rMinRightProb  = 1.;
    while ( (rMinRight - rMinLeft) > (0.001*n) ){
      
      rMinMiddleProb = 0;
      for ( Int_t i = r; i <= n; i++ ){
	binomialCoefficient = 1;
	
	for ( Int_t j = n; j > i; j-- ){
	  binomialCoefficient *= j/((Double_t)(j - i));
	}
	
	rMinMiddleProb += binomialCoefficient*TMath::Power(rMinMiddle/((Double_t)(n)), i)
	                 *TMath::Power((n - rMinMiddle)/((Double_t)(n)), n - i);
      }
      
      if ( rMinMiddleProb > 0.16 ){
	rMinRight     = rMinMiddle;
	rMinRightProb = rMinMiddleProb;
      } else if ( rMinMiddleProb < 0.16 ){
	rMinLeft      = rMinMiddle;
	rMinLeftProb  = rMinMiddleProb;
      } else {
	rMinLeft      = rMinRight     = rMinMiddle;
	rMinLeftProb  = rMinRightProb = rMinMiddleProb;
      }
      
      rMinMiddle = 0.5*(rMinLeft + rMinRight);
      
      if ( rMinLeft > r ){
	rMinMiddle = rMinLeft = rMinRight = 0;
      }
    }
    
    Double_t rMaxLeft       = 0.;
    Double_t rMaxMiddle     = TMath::Min(1.5*r, n - 0.5*r);
    Double_t rMaxRight      = n;
    Double_t rMaxLeftProb   = 1.;
    Double_t rMaxMiddleProb = 0.5;
    Double_t rMaxRightProb  = 0.;
    while ( (rMaxRight - rMaxLeft) > (0.001*n) ){
      
      rMaxMiddleProb = 0;
      for ( Int_t i = 0; i <= r; i++ ){
	binomialCoefficient = 1;
	
	for ( Int_t j = n; j > (n - i); j-- ){
	  binomialCoefficient *= j/((Double_t)(i - (n - j)));
	}
	
	rMaxMiddleProb += binomialCoefficient*TMath::Power(rMaxMiddle/((Double_t)(n)), i)
	                 *TMath::Power((n - rMaxMiddle)/((Double_t)(n)), n - i);
      }

      if ( rMaxMiddleProb > 0.16 ){
	rMaxLeft      = rMaxMiddle;
	rMaxLeftProb  = rMaxMiddleProb;
      } else if ( rMaxMiddleProb < 0.16 ){
	rMaxRight     = rMaxMiddle;
	rMaxRightProb = rMaxMiddleProb;
      } else {
	rMaxLeft      = rMaxRight     = rMaxMiddle;
	rMaxLeftProb  = rMaxRightProb = rMaxMiddleProb;
      }
      
      rMaxMiddle = 0.5*(rMaxLeft + rMaxRight);
      
      if ( rMaxRight < r ){
	rMaxMiddle = rMaxLeft = rMaxRight = n;
      }
    }
    
    rMin = rMinMiddle;
    rMax = rMaxMiddle;
  }
  
  TGraphAsymmErrors* getEfficiency(const TH1* histogram_numerator, const TH1* histogram_denominator)
  {
    Int_t error = 0;
    if ( !(histogram_numerator->GetNbinsX()           == histogram_denominator->GetNbinsX())           ) error = 1;
    if ( !(histogram_numerator->GetXaxis()->GetXmin() == histogram_denominator->GetXaxis()->GetXmin()) ) error = 1;
    if ( !(histogram_numerator->GetXaxis()->GetXmax() == histogram_denominator->GetXaxis()->GetXmax()) ) error = 1;
    
    if ( error ){
      std::cerr << "Error in <getEfficiency>: Dimensionality of histograms does not match !!" << std::endl;
      return 0;
    }
    
    TAxis* xAxis = histogram_numerator->GetXaxis();
    
    Int_t nBins = xAxis->GetNbins();
    TArrayF x(nBins);
    TArrayF dxUp(nBins);
    TArrayF dxDown(nBins);
    TArrayF y(nBins);
    TArrayF dyUp(nBins);
    TArrayF dyDown(nBins);
    
    for ( Int_t ibin = 1; ibin <= nBins; ibin++ ){
      Int_t nObs = TMath::Nint(histogram_denominator->GetBinContent(ibin));
      Int_t rObs = TMath::Nint(histogram_numerator->GetBinContent(ibin));
      
      Float_t xCenter = histogram_denominator->GetBinCenter(ibin);
      Float_t xWidth  = histogram_denominator->GetBinWidth(ibin);
      
      x[ibin - 1]      = xCenter;
      dxUp[ibin - 1]   = 0.5*xWidth;
      dxDown[ibin - 1] = 0.5*xWidth;
      
      if ( nObs > 0 ){
	Float_t rMin = 0.;
	Float_t rMax = 0.;
	
	getBinomialBounds(nObs, rObs, rMin, rMax);
	
	y[ibin - 1]      = rObs/((Float_t)nObs);
	dyUp[ibin - 1]   = (rMax - rObs)/((Float_t)nObs);
	dyDown[ibin - 1] = (rObs - rMin)/((Float_t)nObs);
      } else{
	y[ibin - 1]      = 0.;
	dyUp[ibin - 1]   = 0.;
	dyDown[ibin - 1] = 0.;
      }
    }
    
    TString name  = TString(histogram_numerator->GetName()).Append("Graph");
    TString title = histogram_numerator->GetTitle();
    
    TGraphAsymmErrors* graph = 
      new TGraphAsymmErrors(nBins, x.GetArray(), y.GetArray(), 
			    dxDown.GetArray(), dxUp.GetArray(), dyDown.GetArray(), dyUp.GetArray());
    
    graph->SetName(name);
    graph->SetTitle(title);
    
    return graph;
  }
  //-------------------------------------------------------------------------------
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<determineJetToTauFakeRate>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("determineJetToTauFakeRate");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("determineJetToTauFakeRate") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgDetermineJetToTauFakeRate = cfg.getParameter<edm::ParameterSet>("determineJetToTauFakeRate");
  
  std::string looseRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("looseRegion");
  std::string tightRegion = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tightRegion");

  std::string processData = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processData");
  std::string processQCD  = cfgDetermineJetToTauFakeRate.getParameter<std::string>("processQCD");
  vstring processesToSubtract = cfgDetermineJetToTauFakeRate.getParameter<vstring>("processesToSubtract");
  
  std::string category = cfgDetermineJetToTauFakeRate.getParameter<std::string>("category");

  std::string tauPtBin = cfgDetermineJetToTauFakeRate.getParameter<std::string>("tauPtBin");

  vstring histogramsToFit = cfgDetermineJetToTauFakeRate.getParameter<vstring>("histogramsToFit");

  fwlite::InputSource inputFiles(cfg); 
  if ( !(inputFiles.files().size() == 1) )
    throw cms::Exception("determineJetToTauFakeRate") 
      << "Exactly one input file expected !!\n";
  TFile* inputFile = new TFile(inputFiles.files().front().data());

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  TDirectory* inputDir_loose = getDirectory(inputFile, looseRegion, category, tauPtBin, true);
  assert(inputDir_loose);
  TDirectory* inputDir_tight = getDirectory(inputFile, tightRegion, category, tauPtBin, true);
  assert(inputDir_tight);

  for ( vstring::const_iterator histogramToFit = histogramsToFit.begin();
	histogramToFit != histogramsToFit.end(); ++histogramToFit ) {
    TH1* histogramData_loose = getHistogram(inputDir_loose, processData, *histogramToFit, "central", true);
    assert(histogramData_loose);
    TH1* histogramData_tight = getHistogram(inputDir_tight, processData, *histogramToFit, "central", true);
    assert(histogramData_tight);
    checkCompatibleBinning(histogramData_loose, histogramData_tight);

    std::vector<TH1*> histogramsToSubtract_loose;
    std::vector<TH1*> histogramsToSubtract_tight;
    for ( vstring::const_iterator processToSubtract = processesToSubtract.begin();
	  processToSubtract != processesToSubtract.end(); ++processToSubtract ) {
      TH1* histogramToSubtract_loose = getHistogram(inputDir_loose, *processToSubtract, *histogramToFit, "central", true);
      histogramsToSubtract_loose.push_back(histogramToSubtract_loose);
      TH1* histogramToSubtract_tight = getHistogram(inputDir_tight, *processToSubtract, *histogramToFit, "central", true);
      histogramsToSubtract_tight.push_back(histogramToSubtract_tight);
    }
    
    TDirectory* outputDir = createSubdirectory_recursively(fs, "jetToTauFakeRate");
    outputDir->cd();

    std::string histogramNameJetToTauFakeRate_denominator = Form("jetToTauFakeRate_%s_%s_denominator", histogramToFit->data(), looseRegion.data());
    TH1* histogramJetToTauFakeRate_denominator = subtractHistograms(histogramNameJetToTauFakeRate_denominator.data(), histogramData_loose, histogramsToSubtract_loose);
    std::string histogramNameJetToTauFakeRate_numerator = Form("jetToTauFakeRate_%s_%s_numerator", histogramToFit->data(), tightRegion.data());
    TH1* histogramJetToTauFakeRate_numerator = subtractHistograms(histogramNameJetToTauFakeRate_numerator.data(), histogramData_tight, histogramsToSubtract_tight);

    std::string graphNameJetToTauFakeRate = Form("jetToTauFakeRate_%s_%s_div_%s", histogramToFit->data(), tightRegion.data(), looseRegion.data());
    TGraphAsymmErrors* graphJetToTauFakeRate = getEfficiency(histogramJetToTauFakeRate_numerator, histogramJetToTauFakeRate_denominator);
    graphJetToTauFakeRate->SetName(graphNameJetToTauFakeRate.data());
  }

  delete inputFile;

  clock.Show("determineJetToTauFakeRate");

  return 0;
}
