#ifndef LLRAnalysis_TauTauStudies_MEtRescalerProducer_h
#define LLRAnalysis_TauTauStudies_MEtRescalerProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"


class MEtRescalerProducer : public edm::EDProducer{


 public: 

  explicit MEtRescalerProducer(const edm::ParameterSet&);
  virtual ~MEtRescalerProducer();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag metTag_;
  edm::InputTag jetTag_;
  edm::InputTag tauTag_;
  edm::InputTag electronTag_;
  edm::InputTag muonTag_;
  float unClusterShift_;
  float muonShift_;
  float tauShift_;
  float electronShift_;
  float jetThreshold_;
  bool verbose_;

};


#endif
