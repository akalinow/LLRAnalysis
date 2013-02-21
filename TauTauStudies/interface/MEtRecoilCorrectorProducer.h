#ifndef LLRAnalysis_TauTauStudies_MEtRecoilCorrectorProducer_h
#define LLRAnalysis_TauTauStudies_MEtRecoilCorrectorProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "LLRAnalysis/Utilities/interface/RecoilCorrector.hh"


class MEtRecoilCorrectorProducer : public edm::EDProducer{


 public: 

  explicit MEtRecoilCorrectorProducer(const edm::ParameterSet&);
  virtual ~MEtRecoilCorrectorProducer();

 private:

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  edm::InputTag genParticlesTag_;
  edm::InputTag metTag_;
  edm::InputTag tauTag_;
  edm::InputTag electronTag_;
  edm::InputTag muonTag_;
  edm::InputTag jetTag_;
  edm::FileInPath inputFileNamezmm42X_;
  edm::FileInPath inputFileNamedatamm_;
  edm::FileInPath inputFileNamewjets_;
  edm::FileInPath inputFileNamezjets_;
  edm::FileInPath inputFileNamehiggs_;
  RecoilCorrector* recoilCorr_;
  bool verbose_;
  bool isMC_;
  double minJetPt_;
  double numOfSigmas_;
  int eventCounter_;
  int genDecay_;

};


#endif
