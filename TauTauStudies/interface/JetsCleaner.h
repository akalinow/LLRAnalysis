#ifndef LLRAnalysis_TauTauStudies_JetsCleaner_h
#define LLRAnalysis_TauTauStudies_JetsCleaner_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class JetsCleaner : public edm::EDProducer{

 public: 

  explicit JetsCleaner(const edm::ParameterSet&);
  virtual ~JetsCleaner();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag jetsTag_;
  edm::InputTag valueMapTag_;
  double minDeltaR_;

};


#endif
