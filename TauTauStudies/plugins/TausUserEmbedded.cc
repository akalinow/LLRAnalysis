#include "LLRAnalysis/TauTauStudies/interface/TausUserEmbedded.h"


TausUserEmbedded::TausUserEmbedded(const edm::ParameterSet & iConfig){

  tauTag_    = iConfig.getParameter<edm::InputTag>("tauTag");
  vertexTag_ = iConfig.getParameter<edm::InputTag>("vertexTag");
 
  verbosity_ = ( iConfig.exists("verbosity") ) ?
    iConfig.getParameter<int>("verbosity") : 0;

  produces<pat::TauCollection>("");

}

TausUserEmbedded::~TausUserEmbedded(){
}

void TausUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  if ( verbosity_ ) {
    std::cout << "<TausUserEmbedded::produce>:" << std::endl;
  }

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

    if ( verbosity_ ) {
      std::cout << "tau #" << i << ": Pt = " << aTau.pt() << ", eta = " << aTau.eta() << ", phi = " << aTau.phi() << ", zVertex = " << aTau.vertex().z() << std::endl;
      std::cout << " " << vertexTag_.label() << " #0: z = " << (*vertexes)[0].position().z() << ", rho = " << (*vertexes)[0].position().Rho() << "," 
		<< " ndof = " << (*vertexes)[0].ndof() << ", isValid = " << (*vertexes)[0].isValid() << " --> dZPV = " << dZPV << std::endl;
    }

    tausUserEmbeddedColl->push_back(aTau);

  }


  iEvent.put( tausUserEmbeddedColl );
  return;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TausUserEmbedded);


