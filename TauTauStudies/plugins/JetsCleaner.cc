#include "LLRAnalysis/TauTauStudies/interface/JetsCleaner.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/ValueMap.h"


JetsCleaner::JetsCleaner(const edm::ParameterSet & iConfig){

  jetsTag_ = iConfig.getParameter<edm::InputTag>("jets");
  valueMapTag_ = iConfig.getParameter<edm::InputTag>("valueMap");
  minDeltaR_= iConfig.getParameter<double>("minDeltaR");

  produces<pat::JetCollection>("");

}

JetsCleaner::~JetsCleaner(){
}

void JetsCleaner::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::JetCollection> jetsHandle;
  iEvent.getByLabel(jetsTag_,jetsHandle);
  const pat::JetCollection* jets = jetsHandle.product();

  edm::Handle<edm::ValueMap<float> > valueMapHandle;
  iEvent.getByLabel(valueMapTag_,valueMapHandle);
  const edm::ValueMap<float>*  aValueMap = valueMapHandle.product();

  std::auto_ptr< pat::JetCollection > cleanedJetCollection( new pat::JetCollection() ) ;

  for(unsigned int i = 0; i < jetsHandle->size(); i++){
  
    pat::JetRef aJet( jetsHandle , i);
    if(aJet.isNull()){
      std::cout << "null reference!!!!" << std::endl;
      continue;
    }

    float minDeltaR = (*aValueMap)[aJet];
    
    if(minDeltaR > minDeltaR_){
      cleanedJetCollection->push_back(*aJet);
      //std::cout << "pushed back a new jet !!!" << std::endl;
    }

  }


  iEvent.put( cleanedJetCollection );
  return;
}


#include "FWCore/Framework/interface/MakerMacros.h"
 

DEFINE_FWK_MODULE(JetsCleaner);


