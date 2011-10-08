#include "LLRAnalysis/TauTauStudies/interface/TausUserEmbedded.h"


TausUserEmbedded::TausUserEmbedded(const edm::ParameterSet & iConfig){

  tauTag_    = iConfig.getParameter<edm::InputTag>("tauTag");
  vertexTag_ = iConfig.getParameter<edm::InputTag>("vertexTag");
 
  produces<pat::TauCollection>("");

}

TausUserEmbedded::~TausUserEmbedded(){
}

void TausUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::TauCollection> tausHandle;
  iEvent.getByLabel(tauTag_,tausHandle);
  const pat::TauCollection* taus = tausHandle.product();

  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByLabel(vertexTag_,vertexHandle);
  const reco::VertexCollection* vertexes = vertexHandle.product();

  std::auto_ptr< pat::TauCollection > tausUserEmbeddedColl( new pat::TauCollection() ) ;

  for(unsigned int i = 0; i < taus->size(); i++){
    pat::Tau aTau( (*taus)[i] );

    float dZPV = vertexes->size()>0 ?
      fabs( aTau.vertex().z() - (*vertexes)[0].position().z() ) : -99; 
    aTau.addUserFloat("dzWrtPV", dZPV );

    tausUserEmbeddedColl->push_back(aTau);

  }


  iEvent.put( tausUserEmbeddedColl );
  return;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TausUserEmbedded);


