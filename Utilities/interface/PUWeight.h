///////////////////////////////////////////////////////////////////////
//
//    FILE: PUWeight.h
//   CLASS: PUWeight
// AUTHORS: I. Gonzalez Caballero
//    DATE: 09/03/2011
//
// CONTENT: An utility class to weight the events according to the real
//          data pile up with respect to the generated pileup.
//
///////////////////////////////////////////////////////////////////////

#ifndef PUWEIGHT_H
#define PUWEIGHT_H 1


#include "TH1F.h"
#include "TNamed.h"

class PUWeight {
 public:
  PUWeight(float luminosity = -1, const char* year="2011A");
  PUWeight(const char* mcfolder, const char* mcproccess,
	   float luminosity = -1, const char* year="2011A");
  ~PUWeight() {delete fWeight; delete fMC; delete fData;}


  // Tells if this object has loaded correctly the histograms
  bool IsValid() const { return fWeight;}


  // Returns the weight for a given PU value
  float GetWeight(unsigned int pu) const {
    return (fWeight? fWeight->GetBinContent(pu+1):0);
  }

  // Returns the MC only weight for a given PU value
  float GetPUMC(unsigned int pu) const {
    return (fMC? fMC->GetBinContent(pu+1):0);
  }
  // Returns the Data only weight for a given PU value
  float GetPUData(unsigned int pu) const {
    return (fData? fData->GetBinContent(pu+1):0);
  }

  // Get the histogram with the weights
  TH1F* GetWeightsHisto() const {return (TH1F*)fWeight->Clone();}

  // Get the histogram with the profile for Data
  TH1F* GetDataHisto() const {return (TH1F*)fData->Clone();}

  // Get the histogram with the profile for MC
  TH1F* GetMCHisto() const {return (TH1F*)fMC->Clone();}

 protected:
  // Build the PU ideal profile for MC
  TH1F* IdealMCHistogram();

  // Load the PU profile for MC
  TH1F* LoadMCHistogram(const char* mcfolder, const char* mcproccess);
  // Load the PU profile for Data
  TH1F* LoadDataHistogram(float luminosity, const char* year="2011A");

  // Divide the Data profile by the MC profile
  TH1F* CalculateWeight();

 protected:
  TH1F* fData;   //PU profile for data
  TH1F* fMC;     //PU profile for MC
  TH1F* fWeight; //Histogram with the weight content

};


#endif
