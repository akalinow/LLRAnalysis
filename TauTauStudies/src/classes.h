
#include "LLRAnalysis/TauTauStudies/interface/ObjectRescalerProducer.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/Common/interface/Wrapper.h"

namespace {

  struct dictionary{

    //ObjectRescalerProducer<pat::Electron> dummyElectronRescalerProducer;
    //edm::Wrapper<ObjectRescalerProducer<pat::Electron> > dummyElectronRescalerProducerWrapper;
    //std::vector<ObjectRescalerProducer<pat::Electron> > dummyElectronRescalerProducerVector;
    //edm::Wrapper<std::vector<ObjectRescalerProducer<pat::Electron> > > dummyElectronRescalerProducerVectorWrapper;

    //ObjectRescalerProducer<pat::Muon> dummyMuonRescalerProducer;
    //edm::Wrapper<ObjectRescalerProducer<pat::Muon> > dummyMuonRescalerProducerWrapper;
    //std::vector<ObjectRescalerProducer<pat::Muon> > dummyMuonRescalerProducerVector;
    //edm::Wrapper<std::vector<ObjectRescalerProducer<pat::Muon> > > dummyMuonRescalerProducerVectorWrapper;
    
    //ObjectRescalerProducer<pat::Tau> dummyTauRescalerProducer;
    //edm::Wrapper<ObjectRescalerProducer<pat::Tau> > dummyTauRescalerProducerWrapper;
    //std::vector<ObjectRescalerProducer<pat::Tau> > dummyTauRescalerProducerVector;
    //edm::Wrapper<std::vector<ObjectRescalerProducer<pat::Tau> > > dummyTauRescalerProducerVectorWrapper;

    edm::Wrapper<pat::Muon> dummyMuonWrapper;
    edm::Wrapper<pat::Electron> dummyElectronWrapper;
    edm::Wrapper<pat::Tau> dummyTauWrapper;

  };


}
