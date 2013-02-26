#ifndef LLRAnalysis_Utilities_UserIsolatedPatElectron_h
#define LLRAnalysis_Utilities_UserIsolatedPatElectron_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Electron.h"

class UserIsolatedPatElectron : public edm::EDProducer{

 public: 

  explicit UserIsolatedPatElectron(const edm::ParameterSet&);
  virtual ~UserIsolatedPatElectron();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag ElectronTag_;
  bool isMC_;
  bool verbose_;
  std::string userIso_;
};


#endif
