#ifndef LLRAnalysis_TauTauStudies_TauNTuplizer_h
#define LLRAnalysis_TauTauStudies_TauNTuplizer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"



#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <string>


class TauNTuplizer : public edm::EDAnalyzer{


 public:

  explicit TauNTuplizer(const edm::ParameterSet&);
  ~TauNTuplizer();

  void beginJob() ;
  void analyze(const edm::Event&  iEvent, const edm::EventSetup& iSetup);
  void endJob() ;

 private:

  TTree* tree_;
  edm::InputTag vertexTag_;
  edm::InputTag tauTag_;
  edm::InputTag electronTag_;
  edm::InputTag muonTag_;
  edm::InputTag electronIDTag_;
  edm::InputTag muonIDTag_;
  std::string matchTo_;

  edm::LumiReWeighting LumiWeights_;

  float leadPFChargedHadrMva_;
  float leadPFChargedHadrHcalEnergy_;
  float leadPFChargedHadrEcalEnergy_;
  float leadPFChargedHadrTrackPt_;
  float leadPFChargedHadrTrackP_;
  float leadPFCandMva_;
  float leadPFCandHcalEnergy_;
  float leadPFCandEcalEnergy_;
  float leadPFCandPt_;
  float leadPFCandP_;
  int signalPFChargedHadrCands_;
  int signalPFGammaCands_;
  int isTriggerMatched_;
  int numPV_;
  int nPUVertices_;
  int charge_;
  float puWeight_;

  float mcPUweight_;
  
  float emFraction_;
  float visMass_;
  float fbrem_;
  float nbrem_;
  float hasGsf_;
  float hasKft_;
  float pt_;
  float eta_;
  float dxy_;
  float dz_;

  float dPhi_;
  float dEta_;
  float sihih_;
  float HoE_ ;

  std::vector<float>* gammadEta_;
  std::vector<float>* gammadPhi_;
  std::vector<float>* gammaPt_;

  int tightestHPSDBWP_;
  int tightestAntiEWP_;
  int tightestAntiMWP_;
  int electronVeto_;
  int muonVeto_;

};


#endif
