#include "LLRAnalysis/TauTauStudies/interface/DiTauMassProducer.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

typedef DiTauMassProducer<pat::Electron,pat::Tau> ElecTauMassProducer;
typedef DiTauMassProducer<pat::Muon,pat::Tau>     MuTauMassProducer;

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(ElecTauMassProducer);
DEFINE_FWK_MODULE(MuTauMassProducer);
