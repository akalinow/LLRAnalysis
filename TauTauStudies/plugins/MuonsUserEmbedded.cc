#include "LLRAnalysis/TauTauStudies/interface/MuonsUserEmbedded.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "RecoVertex/VertexTools/interface/LinearizedTrackStateFactory.h"
#include "RecoVertex/VertexTools/interface/VertexTrackFactory.h"
#include "RecoVertex/VertexPrimitives/interface/VertexTrack.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "LLRAnalysis/Utilities/interface/VertexReProducer.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

MuonsUserEmbedded::MuonsUserEmbedded(const edm::ParameterSet & iConfig){

  muonTag_           = iConfig.getParameter<edm::InputTag>("muonTag");
  vertexTag_         = iConfig.getParameter<edm::InputTag>("vertexTag");
  fitUnbiasedVertex_ = iConfig.getParameter<bool>("fitUnbiasedVertex");

  //doMuIsoMVA_        = iConfig.getParameter<bool>("doMuIsoMVA");
  // if( doMuIsoMVA_ ){
 //   fMuonIsoMVA_ = new MuonMVAEstimator();
 //   edm::FileInPath inputFileName0 = iConfig.getParameter<edm::FileInPath>("inputFileName0");
 //   edm::FileInPath inputFileName1 = iConfig.getParameter<edm::FileInPath>("inputFileName1");
 //   edm::FileInPath inputFileName2 = iConfig.getParameter<edm::FileInPath>("inputFileName2");
 //   edm::FileInPath inputFileName3 = iConfig.getParameter<edm::FileInPath>("inputFileName3");
 //   edm::FileInPath inputFileName4 = iConfig.getParameter<edm::FileInPath>("inputFileName4");
 //   edm::FileInPath inputFileName5 = iConfig.getParameter<edm::FileInPath>("inputFileName5");   
 //   vector<string> muoniso_weightfiles;
 //   muoniso_weightfiles.push_back(inputFileName0.fullPath().data());
 //   muoniso_weightfiles.push_back(inputFileName1.fullPath().data());
 //   muoniso_weightfiles.push_back(inputFileName2.fullPath().data());
 //   muoniso_weightfiles.push_back(inputFileName3.fullPath().data());
 //   muoniso_weightfiles.push_back(inputFileName4.fullPath().data());
 //   muoniso_weightfiles.push_back(inputFileName5.fullPath().data());   
 //   fMuonIsoMVA->initialize("MuonIso_BDTG_IsoRings",
 // 	    MuonMVAEstimator::kIsoRings,
 // 	    kTRUE,
 // 	    muoniso_weightfiles);
 // }
  
  produces<pat::MuonCollection>("");

}

MuonsUserEmbedded::~MuonsUserEmbedded(){
  //if(doMuIsoMVA_) delete fMuonIsoMVA_;
}

void MuonsUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::MuonCollection> muonsHandle;
  iEvent.getByLabel(muonTag_,muonsHandle);
  const pat::MuonCollection* muons = muonsHandle.product();

  edm::Handle<reco::MuonCollection> recoMuonsHandle;
  iEvent.getByLabel("muons",recoMuonsHandle);
  const reco::MuonCollection* recoMuons = recoMuonsHandle.product();

  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByLabel(vertexTag_,vertexHandle);
  const reco::VertexCollection* vertexes = vertexHandle.product();

  edm::Handle<reco::VertexCollection> allVertexHandle;
  iEvent.getByLabel("offlinePrimaryVertices",allVertexHandle);
  const reco::VertexCollection* allVertexes = allVertexHandle.product();

  edm::Handle<reco::PFCandidateCollection> pfHandle;
  iEvent.getByLabel("particleFlow",pfHandle);
  if( !pfHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No pf particles label available \n";
  const reco::PFCandidateCollection* pfCandidates = pfHandle.product();
  
  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();

  std::auto_ptr< pat::MuonCollection > muonsUserEmbeddedColl( new pat::MuonCollection() ) ;

  for(unsigned int i = 0; i < muons->size(); i++){

    pat::Muon aMuon( (*muons)[i] );

    const reco::Muon* aRecoMuon = 0;
    for(unsigned int j = 0; j < recoMuons->size(); j++){
      if( Geom::deltaR( (*recoMuons)[j].p4() , aMuon.p4()) < 1e-03 ) { 
	aRecoMuon = &((*recoMuons)[j]);
	//std::cout << "Match to recoMuon" << std::endl;
      }
    }

    //std::cout << "@@@@@@@@ recoMuon: " << aMuon.px() << ", " << aMuon.py() << ", " << aMuon.pz() << std::endl;
    int isPFMuon = 0;
    for(unsigned int j = 0; j < pfCandidates->size(); j++){
      if( (*pfCandidates)[j].particleId() == reco::PFCandidate::mu ) { 
	reco::MuonRef muonRefToPFMuon = (*pfCandidates)[j].muonRef();
	//if( muonRefToPFMuon.isNonnull() ){
	//std::cout << j << ": muonRefToPFMuon: " << muonRefToPFMuon->px() << ", " 
	//    << muonRefToPFMuon->py() << ", " << muonRefToPFMuon->pz() << std::endl;
	//}
	if( muonRefToPFMuon.isNonnull() && 
	    Geom::deltaR( muonRefToPFMuon->p4() , aMuon.p4()) < 1e-04 &&
	    (muonRefToPFMuon->isGlobalMuon() || muonRefToPFMuon->isTrackerMuon() ) )
	  isPFMuon = 1;
      }
    }
    
    double dxyWrtPV    =  -99.;
    double dzWrtPV     =  -99.;
    double d0PV        =  -99.;
    double d0PVErr     =  -99.;
    double ip3dPV      =  -99.;
    double ip3dPVErr   =  -99.;
    double d0PVub      =  -99.;
    double d0PVubErr   =  -99.;
    double ip3dPVub    =  -99.;
    double ip3dPVubErr =  -99.;

    if(vertexes->size()!=0 && aMuon.isGlobalMuon()){
      dxyWrtPV = (aMuon.globalTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aMuon.globalTrack())->dz( (*vertexes)[0].position() ) ;
    }
    // if it is at least tracker, than use the inner track
    if (vertexes->size()!=0 && aMuon.isTrackerMuon()){
      dxyWrtPV = (aMuon.innerTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aMuon.innerTrack())->dz( (*vertexes)[0].position() ) ;
    }

    aMuon.addUserFloat("dxyWrtPV",dxyWrtPV);
    aMuon.addUserFloat("dzWrtPV",dzWrtPV);
    aMuon.addUserInt("isPFMuon",isPFMuon);

    /////////////////////////////////////////////////////

    if (fitUnbiasedVertex_ && (aMuon.track()).isNonnull()) {

      const reco::TransientTrack &tt = 
	transientTrackBuilder->build(aMuon.track()); 

      reco::Vertex thevtx   = vertexes->at(0);
      reco::Vertex thevtxub = vertexes->at(0);

      //reco::Vertex* thevtxFromFullList = 0;
      // find the same vertex in the offlinePrimaryVertices collection
      //for(reco::VertexCollection::const_iterator vit = allVertexes.begin(); vit != allVertexes.end(); vit++){
      //if( TMath::Sqrt((vit->position()-thevtx.position()).Mag2()) < 1e-06 )
      //  thevtxFromFullList = &(*vit);
      //}

      reco::TrackCollection newTkCollection;
      bool foundMatch = false;

      for(reco::Vertex::trackRef_iterator itk = thevtx.tracks_begin(); itk!=thevtx.tracks_end(); itk++) {
        //if(itk->get() == &*(aMuon.innerTrack())) { 
	//std::cout << "ID: " << itk->id() << ", KEY " << itk->key() << std::endl;

	if( aRecoMuon && 
	    itk->id() == (aRecoMuon->innerTrack()).id() && 
	    itk->key() == (aRecoMuon->innerTrack()).key() ){
          foundMatch = true;
        }
	else
	  newTkCollection.push_back(*itk->get());
      }

      if(foundMatch) { 

	//std::cout << "Found match..." << std::endl;    
        edm::Handle<reco::BeamSpot> bs;
        iEvent.getByLabel(edm::InputTag("offlineBeamSpot"),bs);
      
        VertexReProducer revertex( allVertexHandle , iEvent);
        edm::Handle<reco::BeamSpot> pvbeamspot;
        iEvent.getByLabel(revertex.inputBeamSpot(),pvbeamspot);
	//std::cout << "Creating new vertex..." << std::endl;    
	std::vector<TransientVertex> pvs = revertex.makeVertices(newTkCollection,*pvbeamspot,iSetup);
        if(pvs.size()>0) {
          thevtxub = pvs.front();
	  //std::cout << "Created unbiased vertex" << std::endl;
        }
      }

      //preserve sign of transverse impact parameter (cross-product definition from track, not lifetime-signing)
      const double thesign   = ( (-(aMuon.track())->dxy( thevtx.position() )) >=0 ) ? 1. : -1.;

      const std::pair<bool,Measurement1D> &d0pv =  
	IPTools::absoluteTransverseImpactParameter(tt, thevtx);
      if (d0pv.first) {
        d0PV    = (thesign*d0pv.second.value()) ;
        d0PVErr = (d0pv.second.error());
      }
      else {
	d0PV    = -99.;
	d0PVErr = -99.;
      }

      const std::pair<bool,Measurement1D> &ip3dpv =  
	IPTools::absoluteImpactParameter3D(tt,thevtx);
      if (ip3dpv.first) {
       ip3dPV    = (thesign*ip3dpv.second.value());
       ip3dPVErr = (ip3dpv.second.error());
      }
      else {
        ip3dPV    =  -99.;
	ip3dPVErr =  -99.;
      }

      const std::pair<bool,Measurement1D> &d0pvub =  
	IPTools::absoluteTransverseImpactParameter(tt, thevtxub);
      if (d0pvub.first) {
        d0PVub    = (thesign*d0pvub.second.value()) ;
        d0PVubErr = (d0pvub.second.error());
      }
      else {
	d0PVub    = -99.;
	d0PVubErr = -99.;
      }

      const std::pair<bool,Measurement1D> &ip3dpvub =  
	IPTools::absoluteImpactParameter3D(tt,thevtxub);
      if (ip3dpvub.first) {
       ip3dPVub    = (thesign*ip3dpvub.second.value());
       ip3dPVubErr = (ip3dpvub.second.error());
      }
      else {
        ip3dPVub    =  -99.;
	ip3dPVubErr =  -99.;
      }


    }

    //std::cout << d0PV << ", " << ip3dPV << ", " << d0PVub << ", " << ip3dPVub << std::endl;
    if( fabs(d0PV-d0PVub) > 0) std::cout << "Difference!!" << std::endl;
    aMuon.addUserFloat("d0PV",       d0PV);
    aMuon.addUserFloat("d0PVErr",    d0PVErr);
    aMuon.addUserFloat("ip3dPV",     ip3dPV);
    aMuon.addUserFloat("ip3dPVErr",  ip3dPVErr);
    aMuon.addUserFloat("d0PVub",     d0PVub);
    aMuon.addUserFloat("d0PVubErr",  d0PVubErr);
    aMuon.addUserFloat("ip3dPVub",   ip3dPVub);
    aMuon.addUserFloat("ip3dPVubErr",ip3dPVubErr);

    ////////////////////////////////////////////////////


    // iso deposits
    reco::isodeposit::AbsVetos vetos2011Charged; 
    reco::isodeposit::AbsVetos vetos2011Neutral;  
    reco::isodeposit::AbsVetos vetos2011Photons;
   
    vetos2011Charged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.0001));
    vetos2011Charged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    vetos2011Neutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2011Neutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    vetos2011Photons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aMuon.eta(),aMuon.phi()),0.01));
    vetos2011Photons.push_back(new reco::isodeposit::ThresholdVeto(0.5));

    float chIso03 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2011Charged).first;
    float nhIso03 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2011Neutral).first;
    float phIso03 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2011Photons).first;
    float nhIsoPU03 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2011Neutral).first;
    float phIsoPU03 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2011Photons).first;

    float chIso04 = 
      aMuon.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2011Charged).first;
    float nhIso04 = 
      aMuon.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2011Neutral).first;
    float phIso04 = 
      aMuon.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2011Photons).first;
    float nhIsoPU04 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2011Neutral).first;
    float phIsoPU04 = 
      aMuon.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2011Photons).first;
    float allChIso04 =  
      aMuon.isoDeposit(pat::User1Iso)->depositAndCountWithin(0.4, vetos2011Charged).first;
    
    aMuon.addUserFloat("PFRelIso04",(chIso04+nhIso04+phIso04)/aMuon.pt());
    aMuon.addUserFloat("PFRelIso03",(chIso03+nhIso03+phIso03)/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB04",(chIso04+std::max(nhIso04+phIso04-0.5*(nhIsoPU04),0.0))/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB03",(chIso03+std::max(nhIso03+phIso03-0.5*(nhIsoPU03),0.0))/aMuon.pt());
    aMuon.addUserFloat("PFRelIsoDB04v2",(allChIso04+std::max(nhIso04+phIso04-0.5*(nhIsoPU04),0.0))/aMuon.pt());
    
    //add user isolation with custom configuration for x-check
    float allChIso04v3 = aMuon.userIsolation(pat::User1Iso);
    float nhIso04v3 = aMuon.userIsolation(pat::PfNeutralHadronIso);
    float phIso04v3 = aMuon.userIsolation(pat::PfGammaIso);
    float allIsoPU04v3 = aMuon.userIsolation(pat::PfAllParticleIso);
    aMuon.addUserFloat("PFRelIsoDB04v3",(allChIso04v3+std::max(nhIso04v3+phIso04v3-0.5*(allIsoPU04v3),0.0))/aMuon.pt());

    //std::cout <<" Muon Pt : "<<aMuon.pt() << " => "<<" isoV2 "<<aMuon.userFloat("PFRelIsoDB04v2")
    //          <<" isoV3 "<<aMuon.userFloat("PFRelIsoDB04v3")<<std::endl;
    //std::cout << aMuon.pt() << " => "
    //      << chIso04v2 << ", "
    //      << nhIso04v2 << ", "
    //      << phIso04v2 << ", "
    //      << nhIsoPU04v2 << ", "
    //      << phIsoPU04v2
    //      << std::endl;
    //std::cout << aMuon.userFloat("PFRelIso04v2")
    //      << std::endl;


    // cleaning
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


