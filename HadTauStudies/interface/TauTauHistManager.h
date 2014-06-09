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

class TauTauHistManager
{
 public:
  /// constructor
  TauTauHistManager(edm::ParameterSet const&);

  /// destructor
  ~TauTauHistManager() {}

  /// book and fill histograms
  void bookHistograms(TFileDirectory&);
  void fillHistograms(double, double, double, int, int, double, double, 
		      double, double, double, int, int, double, double, 
		      double, double, double, 
		      double, double, 
		      double, double, double, double, 
		      double, double, double, double, int,
		      double, double, double, 
		      double, double, double, int,
		      double, int, double,
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
  std::string tauPtBin_;
  typedef std::vector<double> vdouble;
  vdouble tau1EtaBins_;
  int numTau1EtaBins_;
  vdouble tau2EtaBins_;
  int numTau2EtaBins_;
  vdouble bJet1EtaBins_;
  int numBJet1EtaBins_;
  vdouble bJet2EtaBins_;
  int numBJet2EtaBins_;
  std::string central_or_shift_;
 
  struct histogramIn1dParticleEtaBinsEntryType
  {
    histogramIn1dParticleEtaBinsEntryType(double particleEtaMin, double particleEtaMax, TH1* histogram)
      : particleEtaMin_(particleEtaMin),
	particleEtaMax_(particleEtaMax),
	histogram_(histogram)
    {}
    ~histogramIn1dParticleEtaBinsEntryType() {}
    void Fill(double absParticleEta, double x, double weight)
    {
      if ( (particleEtaMin_ < 0. || absParticleEta > particleEtaMin_) && (particleEtaMax_ > 5. || absParticleEta < particleEtaMax_) ) {
	histogram_->Fill(x, weight);
      }
    }
    double particleEtaMin_;
    double particleEtaMax_;
    TH1* histogram_;
  };

  TH1* histogramTau1PtS_;
  TH1* histogramTau1PtL_;
  std::vector<histogramIn1dParticleEtaBinsEntryType*> histogramTau1Pt_inTauEtaBins_;
  TH1* histogramTau1Eta_;
  TH1* histogramTau1Phi_;
  TH1* histogramTau1DecayMode_;
  TH1* histogramTau1GenMatch_;
  TH1* histogramTau1IsoPtSum_;
  TH1* histogramTau1MVA_;

  TH1* histogramTau2PtS_;
  TH1* histogramTau2PtL_;
  std::vector<histogramIn1dParticleEtaBinsEntryType*> histogramTau2Pt_inTauEtaBins_;
  TH1* histogramTau2Eta_;
  TH1* histogramTau2Phi_;
  TH1* histogramTau2DecayMode_;
  TH1* histogramTau2GenMatch_;
  TH1* histogramTau2IsoPtSum_;
  TH1* histogramTau2MVA_;

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

  TH1* histogramNumJets_;

  TH1* histogramBJet1PtS_;
  TH1* histogramBJet1PtL_;
  std::vector<histogramIn1dParticleEtaBinsEntryType*> histogramBJet1Pt_inBJetEtaBins_;
  TH1* histogramBJet1Eta_;
  TH1* histogramBJet1Phi_;

  TH1* histogramBJet2PtS_;
  TH1* histogramBJet2PtL_;
  std::vector<histogramIn1dParticleEtaBinsEntryType*> histogramBJet2Pt_inBJetEtaBins_;
  TH1* histogramBJet2Eta_;
  TH1* histogramBJet2Phi_;

  TH1* histogramNumBJets_;

  TH1* histogramMEtS_;
  TH1* histogramMEtL_;

  TH1* histogramNumVertices_;

  TH1* histogramNUP_;

  struct histogramIn2dParticleEtaBinsEntryType
  {
    histogramIn2dParticleEtaBinsEntryType(double particle1EtaMin, double particle1EtaMax, double particle2EtaMin, double particle2EtaMax, TH1* histogram)
      : particle1EtaMin_(particle1EtaMin),
	particle1EtaMax_(particle1EtaMax),
	particle2EtaMin_(particle2EtaMin),
	particle2EtaMax_(particle2EtaMax),
	histogram_(histogram)
    {}
    ~histogramIn2dParticleEtaBinsEntryType() {}
    void Fill(double absParticle1Eta, double absParticle2Eta, double x, double weight)
    {
      if ( (particle1EtaMin_ < 0. || absParticle1Eta > particle1EtaMin_) && (particle1EtaMax_ > 5. || absParticle1Eta < particle1EtaMax_) &&
	   (particle2EtaMin_ < 0. || absParticle2Eta > particle2EtaMin_) && (particle2EtaMax_ > 5. || absParticle2Eta < particle2EtaMax_) ) {
	histogram_->Fill(x, weight);
      }
    }
    double particle1EtaMin_;
    double particle1EtaMax_;
    double particle2EtaMin_;
    double particle2EtaMax_;
    TH1* histogram_;
  };

  TH1* histogramEventCounter_;
  std::vector<histogramIn2dParticleEtaBinsEntryType*> histogramEventCounter_inTauEtaBins_;
  std::vector<histogramIn2dParticleEtaBinsEntryType*> histogramEventCounter_inBJetEtaBins_;

  std::vector<TH1*> histograms_;
};

#endif
