#ifndef LLRAnalysis_TauTauStudies_TauFakeRateAnalyzer_h
#define LLRAnalysis_TauTauStudies_TauFakeRateAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "LLRAnalysis/Utilities/interface/PUWeight.h"


#include "TFile.h"
#include "TTree.h"

#include <string>


class TauFakeRateAnalyzer : public edm::EDAnalyzer{


 public:

  explicit TauFakeRateAnalyzer(const edm::ParameterSet&);
  ~TauFakeRateAnalyzer();

  int checkVBTFWP(const pat::Electron* ele, int id);

  void beginJob() ;
  void analyze(const edm::Event&  iEvent, const edm::EventSetup& iSetup);
  void endJob() ;

 private:

  TTree* tree_;
  edm::InputTag tauTag_;
  edm::InputTag electronsTag_;
  std::string matchTo_;
  double leadPFChargedHadrMva_;
  double leadPFChargedHadrHcalEnergy_;
  double leadPFChargedHadrEcalEnergy_;
  double leadPFChargedHadrTrackPt_;
  double leadPFChargedHadrTrackP_;
  double leadPFCandMva_;
  double leadPFCandHcalEnergy_;
  double leadPFCandEcalEnergy_;
  double leadPFCandPt_;
  double leadPFCandP_;
  int signalPFChargedHadrCands_;
  int signalPFGammaCands_;
  int numPV_;

  double mcPUweight_;
  
  double emFraction_;
  double visMass_;
  double fbrem_;
  double hasGsf_;
  double hasKft_;
  double pt_;
  double eta_;

  int tightestHPSWP_;
  int tightestAntiEWP_;
  int tightestAntiMWP_;
  double matchedID_;

  PUWeight* fpuweight_;

};


#endif
