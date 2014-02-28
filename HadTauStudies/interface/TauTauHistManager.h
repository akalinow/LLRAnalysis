#ifndef LLRAnalysis_HadTauStudies_TauTauHistManager_h
#define LLRAnalysis_HadTauStudies_TauTauHistManager_h

/** \class TauTauHistManager
 *
 * Fill histograms for MSSM Higgs -> tau tau -> tau_h tau_h analysis
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CommonTools/Utils/interface/TFileDirectory.h"

#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TMath.h>

#include <string>
#include <vector>

namespace
{
  std::string getPt_string(double ptMin, double ptMax)
  {
    std::string retVal = "";
    if      ( ptMin > 0. && ptMax > 0. ) retVal = Form("pt%1.0fto%1.0f", ptMin, ptMax);
    else if ( ptMin > 0.               ) retVal = Form("ptGt%1.0f", ptMin);
    else if (               ptMax > 0. ) retVal = Form("ptLt%1.0f", ptMax);
    return retVal;
  }

  std::string getAbsEta_string(double absEtaMin, double absEtaMax)
  {
    TString retVal = "";
    if      ( absEtaMin > 0. && absEtaMax < +5. ) retVal = Form("absEta%1.0fto%1.1f", absEtaMin, absEtaMax);
    else if ( absEtaMin > 0.                    ) retVal = Form("absEtaGt%1.1f", absEtaMin);
    else if (                   absEtaMax < +5. ) retVal = Form("absEtaLt%1.1f", absEtaMax);
    retVal.ReplaceAll(".", "_");
    return retVal.Data();
  }
}

class TauTauHistManager
{
 public:
  /// constructor
  TauTauHistManager(edm::ParameterSet const&);

  /// destructor
  ~TauTauHistManager() {}

  /// book and fill histograms
  void bookHistograms(TFileDirectory&);
  void fillHistograms(double, double, double, int, double, double, 
		      double, double, double, int, double, double, 
		      double, double, double, 
		      double, double, 
		      double, double, double, double, 
		      double, double, double, double,
		      double, double, double, 
		      double, double, double, 
		      double, int, 
		      double);
  
 protected:

  TH1* book1D(TFileDirectory&, const std::string&, const std::string&, int, double, double);
  TH1* book1D(TFileDirectory&, const std::string&, const std::string&, int, float*);

  TDirectory* createHistogramSubdirectory(TFileDirectory&);

  std::string getHistogramName(const std::string&) const;

 private:

  /// specify process, category, bin, central_or_shift
  /// used to uniquely identify histograms
  std::string process_; 
  std::string category_;
  std::string bin_;
  std::string central_or_shift_;
 
  TH1* histogramTau1PtS_;
  TH1* histogramTau1PtL_;
  TH1* histogramTau1Eta_;
  TH1* histogramTau1Phi_;
  TH1* histogramTau1DecayMode_;
  TH1* histogramTau1IsoDiscrS_;
  TH1* histogramTau1IsoDiscrL_;

  TH1* histogramTau2PtS_;
  TH1* histogramTau2PtL_;
  TH1* histogramTau2Eta_;
  TH1* histogramTau2Phi_;
  TH1* histogramTau2DecayMode_;
  TH1* histogramTau2IsoDiscrS_;
  TH1* histogramTau2IsoDiscrL_;

  TH1* histogramVisMassS_;
  TH1* histogramVisMassL_;
  TH1* histogramSVfitMassS_;
  TH1* histogramSVfitMassL_;
  TH1* histogramDeltaPhi_;
  TH1* histogramDeltaEta_;
  TH1* histogramDeltaR_;
  
  TH1* histogramJet1PtS_;
  TH1* histogramJet1PtL_;
  TH1* histogramJet1Eta_;
  TH1* histogramJet1Phi_;
  TH1* histogramJet1BtagDiscr_;

  TH1* histogramJet2PtS_;
  TH1* histogramJet2PtL_;
  TH1* histogramJet2Eta_;
  TH1* histogramJet2Phi_;
  TH1* histogramJet2BtagDiscr_;

  TH1* histogramBJet1PtS_;
  TH1* histogramBJet1PtL_;
  TH1* histogramBJet1Eta_;
  TH1* histogramBJet1Phi_;

  TH1* histogramBJet2PtS_;
  TH1* histogramBJet2PtL_;
  TH1* histogramBJet2Eta_;
  TH1* histogramBJet2Phi_;

  TH1* histogramMEtS_;
  TH1* histogramMEtL_;

  TH1* histogramNumVertices_;

  TH1* histogramEventCounter_;

  std::vector<TH1*> histograms_;
};

#endif
