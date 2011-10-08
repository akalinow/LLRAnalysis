#include "LLRAnalysis/TauTauStudies/interface/MuonsUserEmbedded.h"


MuonsUserEmbedded::MuonsUserEmbedded(const edm::ParameterSet & iConfig){

  muonTag_ = iConfig.getParameter<edm::InputTag>("muonTag");
  vertexTag_ = iConfig.getParameter<edm::InputTag>("vertexTag");
 
  produces<pat::MuonCollection>("");

}

MuonsUserEmbedded::~MuonsUserEmbedded(){
}

void MuonsUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::MuonCollection> muonsHandle;
  iEvent.getByLabel(muonTag_,muonsHandle);
  const pat::MuonCollection* muons = muonsHandle.product();

  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByLabel(vertexTag_,vertexHandle);
  const reco::VertexCollection* vertexes = vertexHandle.product();

  std::auto_ptr< pat::MuonCollection > muonsUserEmbeddedColl( new pat::MuonCollection() ) ;

  for(unsigned int i = 0; i < muons->size(); i++){
    pat::Muon aMuon( (*muons)[i] );
   
    double dxyWrtPV =  -99.;
    double dzWrtPV  =  -99.;

    if(vertexes->size()!=0 && aMuon.isGlobalMuon()){
      dxyWrtPV = (aMuon.globalTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aMuon.globalTrack())->dxy( (*vertexes)[0].position() ) ;
    }
    else if (vertexes->size()!=0 && aMuon.isTrackerMuon()){
      dxyWrtPV = (aMuon.innerTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aMuon.innerTrack())->dxy( (*vertexes)[0].position() ) ;
    }

    aMuon.addUserFloat("dxyWrtPV",dxyWrtPV);
    aMuon.addUserFloat("dzWrtPV",dzWrtPV);


    // iso deposits
    reco::isodeposit::AbsVetos vetos2010Charged;
    reco::isodeposit::AbsVetos vetos2010Neutral;  
    reco::isodeposit::AbsVetos vetos2010Photons;
    reco::isodeposit::AbsVetos vetos2011Charged; 
    reco::isodeposit::AbsVetos vetos2011Neutral;  
    reco::isodeposit::AbsVetos vetos2011Photons;

    vetos2010Charged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2010Charged.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    vetos2010Neutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.08));
    vetos2010Neutral.push_back(new reco::isodeposit::ThresholdVeto(1.0));
    vetos2010Photons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.05));
    vetos2010Photons.push_back(new reco::isodeposit::ThresholdVeto(1.0));
    
    vetos2011Charged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2011Charged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    vetos2011Neutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2011Neutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    vetos2011Photons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2011Photons.push_back(new reco::isodeposit::ThresholdVeto(0.5));

    float chIso03v1 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2010Charged).first;
    float nhIso03v1 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2010Neutral).first;
    float phIso03v1 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2010Photons).first;
    float nhIsoPU03v1 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2010Neutral).first;
    float phIsoPU03v1 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2010Photons).first;

    float chIso04v1 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2010Charged).first;
    float nhIso04v1 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2010Neutral).first;
    float phIso04v1 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2010Photons).first;
    float nhIsoPU04v1 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2010Neutral).first;
    float phIsoPU04v1 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2010Photons).first;

    float chIso03v2 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2011Charged).first;
    float nhIso03v2 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2011Neutral).first;
    float phIso03v2 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2011Photons).first;
    float nhIsoPU03v2 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2011Neutral).first;
    float phIsoPU03v2 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2011Photons).first;

    float chIso04v2 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2011Charged).first;
    float nhIso04v2 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2011Neutral).first;
    float phIso04v2 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2011Photons).first;
    float nhIsoPU04v2 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2011Neutral).first;
    float phIsoPU04v2 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2011Photons).first;


    aMuon.addUserFloat("PFRelIso04v1",(chIso04v1+nhIso04v1+phIso04v1)/aMuon.pt());
    aMuon.addUserFloat("PFRelIso03v1",(chIso03v1+nhIso03v1+phIso03v1)/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB04v1",(chIso04v1+std::max(nhIso04v1+phIso04v1-0.5*0.5*(nhIsoPU04v1+phIsoPU04v1),0.0))/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB03v1",(chIso03v1+std::max(nhIso03v1+phIso03v1-0.5*0.5*(nhIsoPU03v1+phIsoPU03v1),0.0))/aMuon.pt());

    aMuon.addUserFloat("PFRelIso04v2",(chIso04v2+nhIso04v2+phIso04v2)/aMuon.pt());
    aMuon.addUserFloat("PFRelIso03v2",(chIso03v2+nhIso03v2+phIso03v2)/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB04v2",(chIso04v2+std::max(nhIso04v2+phIso04v2-0.5*0.5*(nhIsoPU04v2+phIsoPU04v2),0.0))/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB03v2",(chIso03v2+std::max(nhIso03v2+phIso03v2-0.5*0.5*(nhIsoPU03v2+phIsoPU03v2),0.0))/aMuon.pt());

    // cleaning
    for(unsigned int i = 0; i <vetos2010Charged.size(); i++){
      delete vetos2010Charged[i];
    }
    for(unsigned int i = 0; i <vetos2010Neutral.size(); i++){
      delete vetos2010Neutral[i];
      delete vetos2010Photons[i];
    }
    for(unsigned int i = 0; i <vetos2011Charged.size(); i++){
      delete vetos2011Charged[i];
    }
    for(unsigned int i = 0; i <vetos2011Neutral.size(); i++){
      delete vetos2011Neutral[i];
      delete vetos2011Photons[i];
    }

    aMuon.addUserFloat("isInRun",iEvent.run());

    muonsUserEmbeddedColl->push_back(aMuon);

  }


  iEvent.put( muonsUserEmbeddedColl );
  return;
}


#include "FWCore/Framework/interface/MakerMacros.h"
 

DEFINE_FWK_MODULE(MuonsUserEmbedded);


