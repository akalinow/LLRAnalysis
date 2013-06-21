#ifndef LLRAnalysis_TauTauStudies_TauESCorrector_h
#define LLRAnalysis_TauTauStudies_TauESCorrector_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Tau.h"
#include "TMath.h"

class TauESCorrector : public edm::EDProducer{


 public: 

  explicit TauESCorrector(const edm::ParameterSet& iConfig){
    tauTag_    = iConfig.getParameter<edm::InputTag>("tauTag");
    verbose_   = iConfig.existsAs<bool>("verbose") ? iConfig.getParameter<bool>("verbose") : false;

    produces<pat::TauCollection>("");
  };
  virtual ~TauESCorrector(){};

  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup){
    
    edm::Handle<pat::TauCollection> tausHandle;
    iEvent.getByLabel(tauTag_,tausHandle);
    const pat::TauCollection* taus = tausHandle.product();

    std::auto_ptr< pat::TauCollection > RescaledTausColl( new pat::TauCollection() ) ;

    for(unsigned int i = 0; i < taus->size(); i++){
      pat::Tau aTau( (*taus)[i] );

      double shift = 1;
      if((aTau.signalPFChargedHadrCands()).size()==1 && (aTau.signalPFGammaCands()).size()>0){
	shift = (1.015 +  0.001  * TMath::Min(TMath::Max(aTau.pt()-45.,0.),10.)); //1prong +pi0
	shift *= 1.012; //New correction for Summer2013
      }
      else if((aTau.signalPFChargedHadrCands()).size()==3)
	shift = (1.012 + 0.001 * TMath::Min(TMath::Max(aTau.pt()-32.,0.),18.)); //3 prongs
      
      //double scale = sqrt( aTau.energy()*shift*aTau.energy()*shift - aTau.mass()*aTau.mass() )/aTau.p();
      //math::XYZTLorentzVectorD p4S( aTau.px()*scale , aTau.py()*scale, aTau.pz()*scale, aTau.energy()*shift );
      math::XYZTLorentzVectorD p4S( aTau.px()*shift , aTau.py()*shift, aTau.pz()*shift, aTau.energy()*shift );

      pat::Tau sTau( (*taus)[i] ); 
      sTau.setP4( p4S );

      if(verbose_) 
	std::cout << iEvent.id() 
                  << " : visited object with pt = " << aTau.pt() << " scaled " << sTau.pt()  
                  << std::endl;
      
      RescaledTausColl->push_back(sTau);
    }
    
    iEvent.put( RescaledTausColl );
    return;
  }

private: 
  edm::InputTag tauTag_;
  bool verbose_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TauESCorrector);

#endif
