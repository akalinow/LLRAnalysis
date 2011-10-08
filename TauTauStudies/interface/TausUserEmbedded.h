#ifndef LLRAnalysis_TauTauStudies_TausUserEmbedded_h
#define LLRAnalysis_TauTauStudies_TausUserEmbedded_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

class TausUserEmbedded : public edm::EDProducer{


 public: 

  explicit TausUserEmbedded(const edm::ParameterSet&);
  virtual ~TausUserEmbedded();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag tauTag_;
  edm::InputTag vertexTag_;

};


#endif
