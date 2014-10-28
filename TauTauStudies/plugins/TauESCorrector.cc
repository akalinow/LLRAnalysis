#ifndef LLRAnalysis_TauTauStudies_TauESCorrector_h
#define LLRAnalysis_TauTauStudies_TauESCorrector_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"
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

      double shiftP = 1.;
      double shiftMass = 1.;
      if ( aTau.genJet() && deltaR(aTau.p4(), aTau.genJet()->p4()) < 0.5 && aTau.genJet()->pt() > 8. ) {
	//Olivier TES
	if((aTau.signalPFChargedHadrCands()).size()==1 && (aTau.signalPFGammaCands()).size()>0){
	  shiftP = 1.01; //New correction for Winter2013
	  shiftMass = 1.01;
	}
	else if((aTau.signalPFChargedHadrCands()).size()==1 && (aTau.signalPFGammaCands()).size()==0){
	  shiftP = 1.01; //New correction for Winter2013
	  shiftMass = 1.;
	}
	else if((aTau.signalPFChargedHadrCands()).size()==3) {
	  shiftP = 1.01; //New correction for Winter2013
	  shiftMass = 1.01;
	}
      }

//       std::cout<<"shiftP = "<<shiftP<<std::endl;
      
      double pxS = aTau.px()*shiftP;
      double pyS = aTau.py()*shiftP;
      double pzS = aTau.pz()*shiftP;
      double massS = aTau.mass()*shiftMass;
      double enS = TMath::Sqrt(pxS*pxS + pyS*pyS + pzS*pzS + massS*massS);
      math::XYZTLorentzVectorD p4S( pxS, pyS, pzS, enS );

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
