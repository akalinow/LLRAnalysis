#ifndef LLRAnalysis_Utilities_GenTauMatchedCaloJet_h
#define LLRAnalysis_Utilities_GenTauMatchedCaloJet_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class GenTauMatchedCaloJet : public edm::EDProducer{

 public: 

  explicit GenTauMatchedCaloJet(const edm::ParameterSet&);
  virtual ~GenTauMatchedCaloJet();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag jetTag_;
  edm::InputTag genTauJetTag_;
  float deltaRMax_;
  bool verbose_;
};


#endif
