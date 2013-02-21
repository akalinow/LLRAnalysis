#ifndef LLRAnalysis_Utilities_UserIsolatedTau_h
#define LLRAnalysis_Utilities_UserIsolatedTau_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/TauReco/interface/PFTauDiscriminator.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

class UserIsolatedTau : public edm::EDProducer{

 public: 

  explicit UserIsolatedTau(const edm::ParameterSet&);
  virtual ~UserIsolatedTau();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag tauTag_;
  edm::InputTag patTauTag_;
  bool isMC_;
};


#endif
