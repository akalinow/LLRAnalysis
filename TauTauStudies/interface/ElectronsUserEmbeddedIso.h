#ifndef LLRAnalysis_TauTauStudies_ElectronsUserEmbeddedIso_h
#define LLRAnalysis_TauTauStudies_ElectronsUserEmbeddedIso_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/PatCandidates/interface/Isolation.h"


class ElectronsUserEmbeddedIso : public edm::EDProducer{


 public: 

  explicit ElectronsUserEmbeddedIso(const edm::ParameterSet&);
  virtual ~ElectronsUserEmbeddedIso();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);


  edm::InputTag electronTag_;


};


#endif
