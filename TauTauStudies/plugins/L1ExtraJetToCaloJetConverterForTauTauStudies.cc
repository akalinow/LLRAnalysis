#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include <vector>

class L1ExtraJetToCaloJetConverterForTauTauStudies : public edm::EDProducer {

  public:
    explicit L1ExtraJetToCaloJetConverterForTauTauStudies(const edm::ParameterSet& cfg){

      srcL1Jets_ = cfg.getParameter<vInputTag>("srcL1Jets");

      produces<reco::CaloJetCollection>(); 
    }

    ~L1ExtraJetToCaloJetConverterForTauTauStudies() {}

  private:

    void produce(edm::Event& evt, const edm::EventSetup& es){
    
      std::auto_ptr<reco::CaloJetCollection> jet_output(new reco::CaloJetCollection() );

      for(vInputTag::const_iterator srcL1Jets_it = srcL1Jets_.begin();
	  srcL1Jets_it != srcL1Jets_.end(); ++srcL1Jets_it){

	edm::Handle<l1extra::L1JetParticleCollection> l1Jets;
	evt.getByLabel(*srcL1Jets_it, l1Jets);
	for(l1extra::L1JetParticleCollection::const_iterator l1Jet = l1Jets->begin();
	    l1Jet != l1Jets->end(); ++l1Jet){

	  reco::CaloJet::Specific dummySpecific;
	  reco::CaloJet caloJet(l1Jet->p4(),l1Jet->vertex(),dummySpecific);
	  jet_output->push_back(caloJet);
	}
      }

      evt.put(jet_output);
    }


    typedef std::vector<edm::InputTag> vInputTag;
    vInputTag srcL1Jets_; 

};

////
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1ExtraJetToCaloJetConverterForTauTauStudies);
