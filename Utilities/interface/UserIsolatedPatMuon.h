#ifndef LLRAnalysis_Utilities_UserIsolatedPatMuon_h
#define LLRAnalysis_Utilities_UserIsolatedPatMuon_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Muon.h"

class UserIsolatedPatMuon : public edm::EDProducer{

 public: 

  explicit UserIsolatedPatMuon(const edm::ParameterSet&);
  virtual ~UserIsolatedPatMuon();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag MuonTag_;
  bool isMC_;
  bool verbose_;
  std::string userIso_;
  double minPt_; 
  double maxEta_;
};


#endif
