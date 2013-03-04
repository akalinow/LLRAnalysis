#ifndef LLRAnalysis_Utilities_GetTauMatchedCaloJet_h
#define LLRAnalysis_Utilities_GetTauMatchedCaloJet_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class GetTauMatchedCaloJet : public edm::EDProducer{

 public: 

  explicit GetTauMatchedCaloJet(const edm::ParameterSet&);
  virtual ~GetTauMatchedCaloJet();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag jetTag_;
  edm::InputTag genTauJetTag_;
  float deltaRMax_;
  bool verbose_;
};


#endif
