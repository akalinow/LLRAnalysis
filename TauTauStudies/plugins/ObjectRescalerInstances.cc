#include "LLRAnalysis/TauTauStudies/interface/ObjectRescalerProducer.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

typedef ObjectRescalerProducer<pat::Electron> ElectronRescalerProducer;
typedef ObjectRescalerProducer<pat::Muon>     MuonRescalerProducer;
typedef ObjectRescalerProducer<pat::Tau>      TauRescalerProducer;


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ElectronRescalerProducer);
DEFINE_FWK_MODULE(MuonRescalerProducer);
DEFINE_FWK_MODULE(TauRescalerProducer);
