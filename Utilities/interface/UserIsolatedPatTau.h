#ifndef LLRAnalysis_Utilities_UserIsolatedPatTau_h
#define LLRAnalysis_Utilities_UserIsolatedPatTau_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

class UserIsolatedPatTau : public edm::EDProducer{

 public: 

  explicit UserIsolatedPatTau(const edm::ParameterSet&);
  virtual ~UserIsolatedPatTau();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag patTauTag_;
  bool isMC_;
  bool verbose_;
  bool useIsoMVA_;
  double minPt_;
  double maxEta_;
};


#endif
