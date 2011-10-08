#ifndef LLRAnalysis_TauTauStudies_MuonsUserEmbedded_h
#define LLRAnalysis_TauTauStudies_MuonsUserEmbedded_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/PatCandidates/interface/Isolation.h"

class MuonsUserEmbedded : public edm::EDProducer{


 public: 

  explicit MuonsUserEmbedded(const edm::ParameterSet&);
  virtual ~MuonsUserEmbedded();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag muonTag_;
  edm::InputTag vertexTag_;

  reco::isodeposit::AbsVetos vetos2010Charged_;
  reco::isodeposit::AbsVetos vetos2010Neutral_;  
  reco::isodeposit::AbsVetos vetos2010Photons_;

  reco::isodeposit::AbsVetos vetos2011Charged_; 
  reco::isodeposit::AbsVetos vetos2011Neutral_;  
  reco::isodeposit::AbsVetos vetos2011Photons_;

};


#endif
