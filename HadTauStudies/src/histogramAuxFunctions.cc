#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"

#include "FWCore/Utilities/interface/Exception.h"

#include <TMath.h>
#include <TArrayF.h>
#include <TString.h>

#include <iostream>
#include <assert.h>

TArrayF getBinning(const TH1* histogram)
{
  TAxis* xAxis = histogram->GetXaxis();
  int numBins = xAxis->GetNbins();
  TArrayF binning(numBins + 1);
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    binning[iBin - 1] = xAxis->GetBinLowEdge(iBin);
    binning[iBin] = xAxis->GetBinUpEdge(iBin);
  }
  return binning;
}

void checkCompatibleBinning(const TH1* histogram1, const TH1* histogram2)
{
  if ( !(histogram1->GetNbinsX() == histogram2->GetNbinsX()) )
    throw cms::Exception("checkCompatibleBinning") 
      << "Histograms " << histogram1->GetName() << " and " << histogram2->GetName() << " have incompatible binning !!\n";
  TAxis* xAxis1 = histogram1->GetXaxis();
  TAxis* xAxis2 = histogram2->GetXaxis();
  int numBins = xAxis1->GetNbins();
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    if ( !(xAxis1->GetBinLowEdge(iBin) == xAxis2->GetBinLowEdge(iBin) && xAxis1->GetBinUpEdge(iBin) == xAxis2->GetBinUpEdge(iBin)) )
      throw cms::Exception("checkCompatibleBinning") 
	<< "Histograms " << histogram1->GetName() << " and " << histogram2->GetName() << " have incompatible binning !!\n";
  }
}

TH1* addHistograms(const std::string& newHistogramName, const TH1* histogram1, const TH1* histogram2)
{
  std::vector<TH1*> histogramsToAdd;
  histogramsToAdd.push_back(const_cast<TH1*>(histogram1));
  histogramsToAdd.push_back(const_cast<TH1*>(histogram2));
  return addHistograms(newHistogramName, histogramsToAdd);
}

namespace
{
  double square(double x)
  {
    return x*x;
  }
}

TH1* addHistograms(const std::string& newHistogramName, const std::vector<TH1*>& histogramsToAdd)
{
  //std::cout << "<addHistograms>:" << std::endl;
  //std::cout << " newHistogramName = " << newHistogramName << std::endl;
  if ( histogramsToAdd.size() == 0 )
    throw cms::Exception("addHistograms") 
      << "No histograms given to add !!\n";
  const TH1* histogramRef = histogramsToAdd.front();
  for ( std::vector<TH1*>::const_iterator histogramToAdd = histogramsToAdd.begin();
	histogramToAdd != histogramsToAdd.end(); ++histogramToAdd ) {
    if ( (*histogramToAdd) == histogramRef ) continue;
    checkCompatibleBinning(*histogramToAdd, histogramRef);
  }
  TArrayF histogramRefBinning = getBinning(histogramRef);
  int numBins = histogramRefBinning.GetSize() - 1;
  TH1* newHistogram = new TH1D(newHistogramName.data(), newHistogramName.data(), numBins, histogramRefBinning.GetArray());
  for ( int iBin = 0; iBin <= (numBins + 1); ++iBin ) {
    double sumBinContent = 0.;
    double sumBinError2 = 0.;
    for ( std::vector<TH1*>::const_iterator histogramToAdd = histogramsToAdd.begin();
	  histogramToAdd != histogramsToAdd.end(); ++histogramToAdd ) {
      sumBinContent += (*histogramToAdd)->GetBinContent(iBin);
      sumBinError2 += square((*histogramToAdd)->GetBinError(iBin));
    }
    newHistogram->SetBinContent(iBin, sumBinContent);
    assert(sumBinError2 >= 0.);
    newHistogram->SetBinError(iBin, TMath::Sqrt(sumBinError2));
  }
  return newHistogram;
}

TH1* subtractHistograms(const std::string& newHistogramName, const TH1* histogramMinuend, const TH1* histogramSubtrahend)
{
  //std::cout << "<subtractHistograms>:" << std::endl;
  //std::cout << " newHistogramName = " << newHistogramName << std::endl;
  checkCompatibleBinning(histogramMinuend, histogramSubtrahend);
  TArrayF histogramBinning = getBinning(histogramMinuend);
  int numBins = histogramBinning.GetSize() - 1;
  TH1* newHistogram = new TH1D(newHistogramName.data(), newHistogramName.data(), numBins, histogramBinning.GetArray());
  for ( int iBin = 0; iBin <= (numBins + 1); ++iBin ) {
    double newBinContent = histogramMinuend->GetBinContent(iBin) - histogramSubtrahend->GetBinContent(iBin);
    double newBinError2 = square(histogramMinuend->GetBinError(iBin)) + square(histogramSubtrahend->GetBinError(iBin));
    newHistogram->SetBinContent(iBin, newBinContent);
    assert(newBinError2 >= 0.);
    newHistogram->SetBinError(iBin, TMath::Sqrt(newBinError2));
  }
  return newHistogram;
}

TH1* subtractHistograms(const std::string& newHistogramName, const TH1* histogram, const std::vector<TH1*>& histogramsToSubtract)
{
  if ( !histogram ) 
    throw cms::Exception("subtractHistograms") 
      << "No histograms given to subtract !!\n";
  TH1* newHistogram = 0;
  if ( histogramsToSubtract.size() == 0 ) {
    newHistogram = (TH1*)histogram->Clone(newHistogramName.data());
  } else {
    TH1* histogramSubtrahend = addHistograms("subtractHistogramsTMP", histogramsToSubtract);
    newHistogram = subtractHistograms(newHistogramName, histogram, histogramSubtrahend);
    delete histogramSubtrahend;
  }
  return newHistogram;
}

double compIntegral(TH1* histogram, bool includeUnderflowBin, bool includeOverflowBin)
{
  double sumBinContent = 0.;
  int numBins = histogram->GetNbinsX();
  int firstBin = ( includeUnderflowBin ) ? 0 : 1;
  int lastBin = ( includeOverflowBin  ) ? (numBins + 1) : numBins;
  for ( int iBin = firstBin; iBin <= lastBin; ++iBin ) {
    sumBinContent += histogram->GetBinContent(iBin);
  }
  return sumBinContent;
}

void makeBinContentsPositive(TH1* histogram)
{
  std::cout << "<makeBinContentsPositive>:" << std::endl;
  std::cout << " integral(" << histogram->GetName() << ") = " << histogram->Integral() << std::endl;
  double integral_original = compIntegral(histogram, true, true);
  std::cout << " integral_original = " << integral_original << std::endl;
  int numBins = histogram->GetNbinsX();
  for ( int iBin = 0; iBin <= (numBins + 1); ++iBin ) {
    double binContent_original = histogram->GetBinContent(iBin);
    double binError2_original = square(histogram->GetBinError(iBin));
    if ( binContent_original < 0. ) {
      double binContent_modified = 0.;
      double binError2_modified = binError2_original + square(binContent_original - binContent_modified);
      assert(binError2_modified >= 0.);
      std::cout << "bin #" << iBin << ": binContent = " << binContent_original << " +/- " << TMath::Sqrt(binError2_original) 
		<< " --> setting it to binContent = " << binContent_modified << " +/- " << TMath::Sqrt(binError2_modified) << std::endl;
      histogram->SetBinContent(iBin, binContent_modified);
      histogram->SetBinError(iBin, TMath::Sqrt(binError2_modified));
    }
  }
  double integral_modified = compIntegral(histogram, true, true);
  std::cout << " integral_modified = " << integral_modified << std::endl;
  if ( integral_modified != 0. ) {
    double sf = integral_original/integral_modified;
    std::cout << "--> scaling histogram by factor = " << sf << std::endl;
    histogram->Scale(sf);
  }
  std::cout << " integral(" << histogram->GetName() << ") = " << histogram->Integral() << std::endl;
}
