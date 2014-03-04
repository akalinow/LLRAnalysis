#ifndef LLRAnalysis_HadTauStudies_histogramAuxFunctions_h
#define LLRAnalysis_HadTauStudies_histogramAuxFunctions_h

#include <TH1.h>

#include <vector>
#include <string>

TArrayF getBinning(const TH1*);
void checkCompatibleBinning(const TH1*, const TH1*);

TH1* addHistograms(const std::string&, const TH1*, const TH1*);
TH1* addHistograms(const std::string&, const std::vector<TH1*>&);

TH1* subtractHistograms(const std::string&, const TH1*, const TH1*);
TH1* subtractHistograms(const std::string&, const TH1*, const std::vector<TH1*>&);

void makeBinContentsPositive(TH1*);

#endif
