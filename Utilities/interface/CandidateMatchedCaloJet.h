#ifndef LLRAnalysis_Utilities_CandidateMatchedCaloJet_h
#define LLRAnalysis_Utilities_CandidateMatchedCaloJet_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class CandidateMatchedCaloJet : public edm::EDProducer{

 public: 

  explicit CandidateMatchedCaloJet(const edm::ParameterSet&);
  virtual ~CandidateMatchedCaloJet();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag jetTag_;
  edm::InputTag candidateTag_;
  float deltaRMax_;
  bool verbose_;
};


#endif
