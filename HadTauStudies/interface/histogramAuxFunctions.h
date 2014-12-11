#ifndef LLRAnalysis_HadTauStudies_histogramAuxFunctions_h
#define LLRAnalysis_HadTauStudies_histogramAuxFunctions_h

#include "CommonTools/Utils/interface/TFileDirectory.h"

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TDirectory.h>

#include <vector>
#include <string>

void fill(TH1*, double, double, double);
void fillWithOverFlow(TH1*, double, double, double);
void fill2d(TH2*, double, double, double, double);
void fillWithOverFlow2d(TH2*, double, double, double, double);

void checkCompatibleBinning(const TH1*, const TH1*);

TH1* addHistograms(const std::string&, const TH1*, const TH1*, int = 0);
TH1* addHistograms(const std::string&, const std::vector<TH1*>&, int = 0);

TH1* subtractHistograms(const std::string&, const TH1*, const TH1*, int = 0);
TH1* subtractHistograms(const std::string&, const TH1*, const std::vector<TH1*>&, int = 0);

void makeBinContentsPositive(TH1*, int = 0);

std::string getParticleEtaLabel(const std::string&, double, double, double, double);

TDirectory* getDirectory(TFile*, const std::string&, const std::string&, const std::string&, bool);

TH1* getHistogram(TDirectory*, const std::string&, const std::string&, const std::string&, bool);

TDirectory* createSubdirectory(TDirectory*, const std::string&);
TDirectory* createSubdirectory_recursively(TFileDirectory&, const std::string&);

#endif
