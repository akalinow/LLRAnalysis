#include "LLRAnalysis/Utilities/plugins/PATTauGenMatchEmbedder.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include <TMath.h>

#include <assert.h>

PATTauGenMatchEmbedder::PATTauGenMatchEmbedder(const edm::ParameterSet& cfg)
  : moduleLabel_(cfg.getParameter<std::string>("@module_label"))
{
  src_ = cfg.getParameter<edm::InputTag>("src");

  srcGenParticleMatch_ = cfg.getParameter<edm::InputTag>("srcGenParticleMatch");
  srcGenJetMatch_ = cfg.getParameter<edm::InputTag>("srcGenJetMatch");

  verbosity_ = ( cfg.exists("verbosity") ) ?
    cfg.getParameter<int>("verbosity") : 0;

  produces<pat::TauCollection>();
}
  
PATTauGenMatchEmbedder::~PATTauGenMatchEmbedder() 
{
  // nothing to be done yet
}

void PATTauGenMatchEmbedder::produce(edm::Event& evt, const edm::EventSetup& es)
{
  if ( verbosity_ ) {
    std::cout << "<PATTauGenMatchEmbedder::produce (moduleLabel = " << moduleLabel_ << ")>:" << std::endl;
  }

  edm::Handle<pat::TauCollection> inputTaus;
  evt.getByLabel(src_, inputTaus);

  edm::Handle<edm::Association<reco::GenParticleCollection> > genParticleMatch;
  evt.getByLabel(srcGenParticleMatch_, genParticleMatch);

  edm::Handle<edm::Association<reco::GenJetCollection> > genJetMatch;
  evt.getByLabel(srcGenJetMatch_, genJetMatch);

  std::auto_ptr<pat::TauCollection> outputTaus(new pat::TauCollection());

  size_t numTaus = inputTaus->size();
  for ( size_t idx = 0; idx < numTaus; ++idx ) {
    pat::TauRef inputTauRef(inputTaus, idx);
    
    pat::Tau outputTau(*inputTauRef);

    reco::GenParticleRef genParticle_matched = (*genParticleMatch)[inputTauRef];
    if ( genParticle_matched.isNonnull() && genParticle_matched.isAvailable() ) {
      outputTau.setGenParticle(*genParticle_matched); 
    }

    reco::GenJetRef genJet_matched = (*genJetMatch)[inputTauRef];
    if ( genJet_matched.isNonnull() && genJet_matched.isAvailable() ) {
      outputTau.setGenJet(genJet_matched);
    }

    outputTaus->push_back(outputTau);
  }
  
  evt.put(outputTaus);
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PATTauGenMatchEmbedder);
