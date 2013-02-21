#include "LLRAnalysis/TauTauStudies/interface/ElectronsUserEmbedded.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

using namespace edm;
using namespace std;
using namespace reco;


ElectronsUserEmbedded::ElectronsUserEmbedded(const edm::ParameterSet & iConfig){

  electronTag_ = iConfig.getParameter<edm::InputTag>("electronTag");
  vertexTag_   = iConfig.getParameter<edm::InputTag>("vertexTag");
  isMC_        = iConfig.getParameter<bool>("isMC");
  doMVAMIT_    = iConfig.getParameter<bool>("doMVAMIT");
  doMVADaniele_= iConfig.getParameter<bool>("doMVADaniele");

  edm::FileInPath inputFileName0 = iConfig.getParameter<edm::FileInPath>("inputFileName0");
  edm::FileInPath inputFileName1 = iConfig.getParameter<edm::FileInPath>("inputFileName1");
  edm::FileInPath inputFileName2 = iConfig.getParameter<edm::FileInPath>("inputFileName2");
  edm::FileInPath inputFileName3 = iConfig.getParameter<edm::FileInPath>("inputFileName3");
  edm::FileInPath inputFileName4 = iConfig.getParameter<edm::FileInPath>("inputFileName4");
  edm::FileInPath inputFileName5 = iConfig.getParameter<edm::FileInPath>("inputFileName5");

  edm::FileInPath inputFileName0v2 = iConfig.getParameter<edm::FileInPath>("inputFileName0v2");
  edm::FileInPath inputFileName1v2 = iConfig.getParameter<edm::FileInPath>("inputFileName1v2");
  edm::FileInPath inputFileName2v2 = iConfig.getParameter<edm::FileInPath>("inputFileName2v2");
  edm::FileInPath inputFileName3v2 = iConfig.getParameter<edm::FileInPath>("inputFileName3v2");
  edm::FileInPath inputFileName4v2 = iConfig.getParameter<edm::FileInPath>("inputFileName4v2");
  edm::FileInPath inputFileName5v2 = iConfig.getParameter<edm::FileInPath>("inputFileName5v2");

  edm::FileInPath inputFileName0v3 = iConfig.getParameter<edm::FileInPath>("inputFileName0v3");
  edm::FileInPath inputFileName1v3 = iConfig.getParameter<edm::FileInPath>("inputFileName1v3");
  edm::FileInPath inputFileName2v3 = iConfig.getParameter<edm::FileInPath>("inputFileName2v3");
  edm::FileInPath inputFileName3v3 = iConfig.getParameter<edm::FileInPath>("inputFileName3v3");
  edm::FileInPath inputFileName4v3 = iConfig.getParameter<edm::FileInPath>("inputFileName4v3");
  edm::FileInPath inputFileName5v3 = iConfig.getParameter<edm::FileInPath>("inputFileName5v3");

  //edm::FileInPath inputFileNameMVADaniele 
  //= iConfig.getParameter<edm::FileInPath>("inputFileNameMVADaniele");
  
  if(doMVAMIT_){
    fMVA_ = new ElectronIDMVA();
    fMVA_->Initialize("BDTG method",
		      inputFileName0.fullPath().data(),
		      inputFileName1.fullPath().data(),
		      inputFileName2.fullPath().data(),
		      inputFileName3.fullPath().data(),
		      inputFileName4.fullPath().data(),
		      inputFileName5.fullPath().data(),                
		      ElectronIDMVA::kNoIPInfo);
  }
  if(doMVADaniele_){

    //fMVADaniele_ = new ElectronMVAEstimator(inputFileNameMVADaniele.fullPath().data());

    Bool_t manualCat = true;

    std::vector<string> myManualCatWeigthsTrig;
    myManualCatWeigthsTrig.push_back(inputFileName0v2.fullPath().data());
    myManualCatWeigthsTrig.push_back(inputFileName1v2.fullPath().data());
    myManualCatWeigthsTrig.push_back(inputFileName2v2.fullPath().data());
    myManualCatWeigthsTrig.push_back(inputFileName3v2.fullPath().data());
    myManualCatWeigthsTrig.push_back(inputFileName4v2.fullPath().data());
    myManualCatWeigthsTrig.push_back(inputFileName5v2.fullPath().data());
    
    myMVATrig_ = new EGammaMvaEleEstimator();
    myMVATrig_->initialize("BDT",
			   EGammaMvaEleEstimator::kTrig,
			   manualCat,
			   myManualCatWeigthsTrig); 

    std::vector<string> myManualCatWeigthsNonTrig;
    myManualCatWeigthsNonTrig.push_back(inputFileName0v3.fullPath().data());
    myManualCatWeigthsNonTrig.push_back(inputFileName1v3.fullPath().data());
    myManualCatWeigthsNonTrig.push_back(inputFileName2v3.fullPath().data());
    myManualCatWeigthsNonTrig.push_back(inputFileName3v3.fullPath().data());
    myManualCatWeigthsNonTrig.push_back(inputFileName4v3.fullPath().data());
    myManualCatWeigthsNonTrig.push_back(inputFileName5v3.fullPath().data());
    
    myMVANonTrig_ = new EGammaMvaEleEstimator();
    myMVANonTrig_->initialize("BDT",
			      EGammaMvaEleEstimator::kNonTrig,
			      manualCat,
			      myManualCatWeigthsNonTrig); 
    
  }



 
  produces<pat::ElectronCollection>("");

}

ElectronsUserEmbedded::~ElectronsUserEmbedded(){
  if(doMVAMIT_) delete fMVA_;
  if(doMVADaniele_){
    delete myMVATrig_; //delete fMVADaniele_;
    delete myMVANonTrig_;
  }
  
}

void ElectronsUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::ElectronCollection> electronsHandle;
  iEvent.getByLabel(electronTag_,electronsHandle);
  const pat::ElectronCollection* electrons = electronsHandle.product();

  //edm::Handle<reco::GsfElectronCollection> gsfElectronsHandle;
  //iEvent.getByLabel("gsfElectrons",gsfElectronsHandle);
  //const reco::GsfElectronCollection* gsfElectrons = gsfElectronsHandle.product();

  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByLabel(vertexTag_,vertexHandle);
  const reco::VertexCollection* vertexes = vertexHandle.product();

  edm::Handle<reco::VertexCollection> stdVertexHandle;
  iEvent.getByLabel("offlinePrimaryVertices",stdVertexHandle);
  const reco::VertexCollection* stdVertexes = stdVertexHandle.product();

  edm::Handle<reco::BeamSpot> bsHandle;
  iEvent.getByLabel("offlineBeamSpot", bsHandle);
  const reco::BeamSpot &thebs = *bsHandle.product();

  edm::Handle<reco::ConversionCollection> hConversions;
  iEvent.getByLabel("allConversions", hConversions);

  // PFCandidateMap
  //edm::Handle<edm::ValueMap<reco::PFCandidatePtr> >ValMapH;
  //iEvent.getByLabel(edm::InputTag("particleFlow","electrons"),ValMapH);    // The InputTag is 'particleFlow:electrons'
  //const edm::ValueMap<reco::PFCandidatePtr> & myValMap(*ValMapH); 

  //Get the CTF tracks
  Handle<reco::TrackCollection> tracks_h;
  //iEvent.getByLabel(edm::InputTag("generalTracks","","RECO"), tracks_h);
  iEvent.getByLabel(edm::InputTag("generalTracks"), tracks_h);
  //iEvent.getByLabel("tmfTracks", tracks_h);

  //get GSF Tracks
  Handle<reco::GsfTrackCollection> gsftracks_h;
  //iEvent.getByLabel(edm::InputTag("electronGsfTracks","","RECO"), gsftracks_h);
  iEvent.getByLabel(edm::InputTag("electronGsfTracks"), gsftracks_h);
  
  edm::ESHandle<TransientTrackBuilder> hTransientTrackBuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",hTransientTrackBuilder);
  const TransientTrackBuilder *transientTrackBuilder = hTransientTrackBuilder.product();

  std::auto_ptr< pat::ElectronCollection > electronsUserEmbeddedColl( new pat::ElectronCollection() ) ;

  for(unsigned int i = 0; i < electrons->size(); i++){

    pat::Electron aElectron( (*electrons)[i] );
    const reco::GsfElectron* aGsf = static_cast<reco::GsfElectron*>(&aElectron); 

    //unsigned int position = 0;
    //bool matchedToGsf = false;
    //for(unsigned int j = 0; j < gsfElectrons->size(); j++){
    //if( Geom::deltaR((*gsfElectrons)[j].p4(), aGsf->p4()) < 1e-03 ){
    //matchedToGsf = true;
    //position = j;
    //}
    //}
    //reco::GsfElectronRef aGsfElectronRef(gsfElectronsHandle, position);
    //const reco::PFCandidatePtr pfElePtr(myValMap[aGsfElectronRef]); 
    int pfId = 1; //pfElePtr.isNonnull();

    //cout << "pfId = " << pfId << endl;

    const reco::Track *el_track = (const reco::Track*)((aElectron).gsfTrack().get());  
    const reco::HitPattern& p_inner = el_track->trackerExpectedHitsInner(); 
    float nHits = p_inner.numberOfHits();

    float dPhi  = aElectron.deltaPhiSuperClusterTrackAtVtx();
    float dEta  = aElectron.deltaEtaSuperClusterTrackAtVtx();
    float sihih = aElectron.sigmaIetaIeta();
    float HoE   = aElectron.hadronicOverEm();

    aElectron.addUserFloat("nHits",nHits);
    aElectron.addUserFloat("dist",-999/*fabs(els_conv_dist)*/);
    aElectron.addUserFloat("dcot",-999/*fabs(els_conv_dcot)*/);
    aElectron.addUserFloat("dPhi",fabs(dPhi));
    aElectron.addUserFloat("dEta",fabs(dEta));
    aElectron.addUserFloat("sihih",sihih);
    aElectron.addUserFloat("HoE",HoE);

    int passconversionveto = 
      //int(!ConversionTools::hasMatchedConversion(*aGsf,hConversions,thebs.position(),true,2.0,1e-06,0));
      int(!ConversionTools::hasMatchedConversion(dynamic_cast<reco::GsfElectron const&>(*(aElectron.originalObjectRef())),hConversions,thebs.position(),true,2.0,1e-06,0));
    aElectron.addUserInt("antiConv",passconversionveto);
  
    double dxyWrtPV =  -99.;
    double dzWrtPV  =  -99.;

    if(vertexes->size()!=0 && (aElectron.gsfTrack()).isNonnull() ){
      dxyWrtPV = (aElectron.gsfTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aElectron.gsfTrack())->dz(  (*vertexes)[0].position() ) ;
    }
    else if (vertexes->size()!=0 && (aElectron.track()).isNonnull() ){
      dxyWrtPV = (aElectron.track())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (aElectron.track())->dz(  (*vertexes)[0].position() ) ;
    }

    aElectron.addUserFloat("dxyWrtPV",dxyWrtPV);
    aElectron.addUserFloat("dzWrtPV",dzWrtPV);

    EcalClusterLazyTools lazyTools(iEvent,iSetup,
				   edm::InputTag("reducedEcalRecHitsEB"),
				   edm::InputTag("reducedEcalRecHitsEE"));


    int myTrigPresel = 0;
    if(fabs(aGsf->superCluster()->eta()) < 1.485) {
      if(aGsf->sigmaIetaIeta() < 0.014 &&
	 aGsf->hadronicOverEm() < 0.15 &&
	 aGsf->dr03TkSumPt()/aGsf->pt() < 0.2 &&
	 aGsf->dr03EcalRecHitSumEt()/aGsf->pt() < 0.2 &&
	 aGsf->dr03HcalTowerSumEt()/aGsf->pt() < 0.2 &&
	 aGsf->gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0)
	myTrigPresel = 1;
    }
    else {
      if(aGsf->sigmaIetaIeta() < 0.035 &&
	 aGsf->hadronicOverEm() < 0.10 &&
	 aGsf->dr03TkSumPt()/aGsf->pt() < 0.2 &&
	 aGsf->dr03EcalRecHitSumEt()/aGsf->pt() < 0.2 &&
	 aGsf->dr03HcalTowerSumEt()/aGsf->pt() < 0.2 &&
	 aGsf->gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0)
	myTrigPresel = 1;
    }



    float mva  = -99;    
    float mva2 = -99; 
    float mva3 = -99; 
    int mvaPreselection = passconversionveto && nHits<=0 && dxyWrtPV<0.045 && dzWrtPV<0.2 &&
      ((aElectron.isEB() && sihih < 0.01 
	&& fabs(dEta) < 0.007
	&& fabs(dPhi) < 0.15
	&& HoE < 0.12
	&& aElectron.dr03TkSumPt()/aElectron.pt() < 0.20
	&& (TMath::Max(aElectron.dr03EcalRecHitSumEt() - 1.0, 0.0))/aElectron.pt() < 0.20
	&& aElectron.dr03HcalTowerSumEt()/aElectron.pt() < 0.20

	) ||
       (aElectron.isEE() && sihih < 0.03 
	&& fabs(dEta) < 0.009
	&& fabs(dPhi) < 0.10
	&& HoE < 0.10
	&& aElectron.dr03TkSumPt()/aElectron.pt() < 0.20
	&& (TMath::Max(aElectron.dr03EcalRecHitSumEt() - 1.0, 0.0))/aElectron.pt() < 0.20
	&& aElectron.dr03HcalTowerSumEt()/aElectron.pt() < 0.20
	));

    if(doMVAMIT_)
      mva = fMVA_->MVAValue(aGsf, lazyTools);
    if(doMVADaniele_){
      //mva2 = fMVADaniele_->mva(*aGsf, vertexes->size());
      //MBmva2 = myMVATrig_->mvaValue( *aGsf , (*stdVertexes)[0], *transientTrackBuilder, lazyTools, false);
      //MBmva3 = myMVANonTrig_->mvaValue( *aGsf , (*stdVertexes)[0], *transientTrackBuilder, lazyTools, false);
      mva2 = myMVATrig_->mvaValue( *aGsf , (*vertexes)[0], *transientTrackBuilder, lazyTools, false);
      mva3 = myMVANonTrig_->mvaValue( *aGsf , (*vertexes)[0], *transientTrackBuilder, lazyTools, false);
      //cout << mva2 << endl; 
    }

    //cout << "Daniele's MVA = " << mva2 << endl;

    aElectron.addUserFloat("mva",mva);
    aElectron.addUserInt("mvaPreselection",mvaPreselection);
    aElectron.addUserInt("isTriggerElectron",myTrigPresel);
    aElectron.addUserFloat("mvaPOGTrig"   ,mva2);
    aElectron.addUserFloat("mvaPOGNonTrig",mva3);


    // iso deposits   
    reco::isodeposit::AbsVetos vetos2012EBPFIdCharged; 
    reco::isodeposit::AbsVetos vetos2012EBPFIdNeutral;  
    reco::isodeposit::AbsVetos vetos2012EBPFIdPhotons;
    reco::isodeposit::AbsVetos vetos2012EEPFIdCharged; 
    reco::isodeposit::AbsVetos vetos2012EEPFIdNeutral;  
    reco::isodeposit::AbsVetos vetos2012EEPFIdPhotons;

    reco::isodeposit::AbsVetos vetos2012EBNoPFIdCharged; 
    reco::isodeposit::AbsVetos vetos2012EBNoPFIdNeutral;  
    reco::isodeposit::AbsVetos vetos2012EBNoPFIdPhotons;
    reco::isodeposit::AbsVetos vetos2012EENoPFIdCharged; 
    reco::isodeposit::AbsVetos vetos2012EENoPFIdNeutral;  
    reco::isodeposit::AbsVetos vetos2012EENoPFIdPhotons;

    // passing ele-ID
    //vetos2012EBPFIdCharged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01)); // check!!
    //vetos2012EBPFIdCharged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    //vetos2012EBPFIdNeutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01));
    //vetos2012EBPFIdNeutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    //vetos2012EBPFIdPhotons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01));
    //vetos2012EBPFIdPhotons.push_back(new reco::isodeposit::ThresholdVeto(0.5));

    vetos2012EEPFIdCharged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.015));
    //vetos2012EEPFIdCharged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    //vetos2012EEPFIdNeutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01));
    //vetos2012EEPFIdNeutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    vetos2012EEPFIdPhotons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.08));
    //vetos2012EEPFIdPhotons.push_back(new reco::isodeposit::ThresholdVeto(0.5));
  
    // failing ele-ID
    //vetos2012EBNoPFIdCharged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.015));
    //vetos2012EBNoPFIdCharged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    //vetos2012EBNoPFIdNeutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01));
    //vetos2012EBNoPFIdNeutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    //vetos2012EBNoPFIdPhotons.push_back(new reco::isodeposit::RectangularEtaPhiVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),-0.02,0.02,-0.5,0.5));
    //vetos2012EBNoPFIdPhotons.push_back(new reco::isodeposit::ThresholdVeto(0.5));

    vetos2012EENoPFIdCharged.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.015));
    //vetos2012EENoPFIdCharged.push_back(new reco::isodeposit::ThresholdVeto(0.0));
    //vetos2012EENoPFIdNeutral.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.01));
    //vetos2012EENoPFIdNeutral.push_back(new reco::isodeposit::ThresholdVeto(0.5));
    vetos2012EENoPFIdPhotons.push_back(new reco::isodeposit::ConeVeto(reco::isodeposit::Direction(aElectron.eta(),aElectron.phi()),0.08));
    //vetos2012EENoPFIdPhotons.push_back(new reco::isodeposit::ThresholdVeto(0.5));
  

    float chIso03EBPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2012EBPFIdCharged).first;
    float nhIso03EBPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2012EBPFIdNeutral).first;
    float phIso03EBPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2012EBPFIdPhotons).first;
    float nhIsoPU03EBPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EBPFIdNeutral).first;
    float phIsoPU03EBPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EBPFIdPhotons).first;

    float chIso03EEPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2012EEPFIdCharged).first;
    float nhIso03EEPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2012EEPFIdNeutral).first;
    float phIso03EEPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2012EEPFIdPhotons).first;
    float nhIsoPU03EEPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EEPFIdNeutral).first;
    float phIsoPU03EEPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EEPFIdPhotons).first;


    float chIso03EBNoPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2012EBNoPFIdCharged).first;
    float nhIso03EBNoPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2012EBNoPFIdNeutral).first;
    float phIso03EBNoPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2012EBNoPFIdPhotons).first;
    float nhIsoPU03EBNoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EBNoPFIdNeutral).first;
    float phIsoPU03EBNoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EBNoPFIdPhotons).first;

    float chIso03EENoPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.3, vetos2012EENoPFIdCharged).first;
    float nhIso03EENoPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.3, vetos2012EENoPFIdNeutral).first;
    float phIso03EENoPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.3, vetos2012EENoPFIdPhotons).first;
    float nhIsoPU03EENoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EENoPFIdNeutral).first;
    float phIsoPU03EENoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.3, vetos2012EENoPFIdPhotons).first;


    //////////////////////////////////////////////////////////////

    float chIso04EBPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2012EBPFIdCharged).first;
    float nhIso04EBPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2012EBPFIdNeutral).first;
    float phIso04EBPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2012EBPFIdPhotons).first;
    float nhIsoPU04EBPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EBPFIdNeutral).first;
    float phIsoPU04EBPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EBPFIdPhotons).first;

    float chIso04EEPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2012EEPFIdCharged).first;
    float nhIso04EEPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2012EEPFIdNeutral).first;
    float phIso04EEPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2012EEPFIdPhotons).first;
    float nhIsoPU04EEPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EEPFIdNeutral).first;
    float phIsoPU04EEPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EEPFIdPhotons).first;


    float chIso04EBNoPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2012EBNoPFIdCharged).first;
    float nhIso04EBNoPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2012EBNoPFIdNeutral).first;
    float phIso04EBNoPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2012EBNoPFIdPhotons).first;
    float nhIsoPU04EBNoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EBNoPFIdNeutral).first;
    float phIsoPU04EBNoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EBNoPFIdPhotons).first;

    float chIso04EENoPFId = 
      aElectron.isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4, vetos2012EENoPFIdCharged).first;
    float nhIso04EENoPFId = 
      aElectron.isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4, vetos2012EENoPFIdNeutral).first;
    float phIso04EENoPFId = 
      aElectron.isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4, vetos2012EENoPFIdPhotons).first;
    float nhIsoPU04EENoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EENoPFIdNeutral).first;
    float phIsoPU04EENoPFId = 
      aElectron.isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4, vetos2012EENoPFIdPhotons).first;


    //////////////////////////////////////////////////////////////////////////////////////////////

    float chIso03 = pfId ?
      (aElectron.isEB())*chIso03EBPFId + (aElectron.isEE())*chIso03EEPFId :
      (aElectron.isEB())*chIso03EBNoPFId + (aElectron.isEE())*chIso03EENoPFId ;
    float nhIso03 = pfId ?
      (aElectron.isEB())*nhIso03EBPFId + (aElectron.isEE())*nhIso03EEPFId :
      (aElectron.isEB())*nhIso03EBNoPFId + (aElectron.isEE())*nhIso03EENoPFId ;
    float phIso03 = pfId ?
      (aElectron.isEB())*phIso03EBPFId + (aElectron.isEE())*phIso03EEPFId :
      (aElectron.isEB())*phIso03EBNoPFId + (aElectron.isEE())*phIso03EENoPFId ;
    float nhIsoPU03 = pfId ?
      (aElectron.isEB())*nhIsoPU03EBPFId + (aElectron.isEE())*nhIsoPU03EEPFId :
      (aElectron.isEB())*nhIsoPU03EBNoPFId + (aElectron.isEE())*nhIsoPU03EENoPFId ;

    float chIso04 = pfId ?
      (aElectron.isEB())*chIso04EBPFId + (aElectron.isEE())*chIso04EEPFId :
      (aElectron.isEB())*chIso04EBNoPFId + (aElectron.isEE())*chIso04EENoPFId ;
    float nhIso04 = pfId ?
      (aElectron.isEB())*nhIso04EBPFId + (aElectron.isEE())*nhIso04EEPFId :
      (aElectron.isEB())*nhIso04EBNoPFId + (aElectron.isEE())*nhIso04EENoPFId ;
    float phIso04 = pfId ?
      (aElectron.isEB())*phIso04EBPFId + (aElectron.isEE())*phIso04EEPFId :
      (aElectron.isEB())*phIso04EBNoPFId + (aElectron.isEE())*phIso04EENoPFId ;
    float nhIsoPU04 = pfId ?
      (aElectron.isEB())*nhIsoPU04EBPFId + (aElectron.isEE())*nhIsoPU04EEPFId :
      (aElectron.isEB())*nhIsoPU04EBNoPFId + (aElectron.isEE())*nhIsoPU04EENoPFId ;

    //cout << "@@@@@@@" << endl;
    //cout << "chIso04 = " << chIso04 << endl;
    //cout << "nhIso04 = " << nhIso04 << endl;
    //cout << "phIso04 = " << phIso04 << endl;
    //cout << "@@@@@@@" << endl;


    aElectron.addUserFloat("PFRelIso03",
			   (chIso03+nhIso03+phIso03)/aElectron.pt());
    aElectron.addUserFloat("PFRelIsoDB03",
			   (chIso03+std::max(nhIso03+phIso03-0.5*(nhIsoPU03),0.0))/aElectron.pt());
  
    aElectron.addUserFloat("PFRelIso04",
			   (chIso04+nhIso04+phIso04)/aElectron.pt());
    aElectron.addUserFloat("PFRelIsoDB04",
			   (chIso04+std::max(nhIso04+phIso04-0.5*(nhIsoPU04),0.0))/aElectron.pt());
   
 
    // cleaning
    for(unsigned int i = 0; i <vetos2012EBPFIdCharged.size(); i++){
      delete vetos2012EBPFIdCharged[i];
    }
    for(unsigned int i = 0; i <vetos2012EBPFIdNeutral.size(); i++){
      delete vetos2012EBPFIdNeutral[i];
    }
    for(unsigned int i = 0; i <vetos2012EBPFIdPhotons.size(); i++){
      delete vetos2012EBPFIdPhotons[i];
    }
    for(unsigned int i = 0; i <vetos2012EEPFIdCharged.size(); i++){
      delete vetos2012EEPFIdCharged[i];
    }
    for(unsigned int i = 0; i <vetos2012EEPFIdNeutral.size(); i++){
      delete vetos2012EEPFIdNeutral[i];
    }
    for(unsigned int i = 0; i <vetos2012EEPFIdPhotons.size(); i++){
      delete vetos2012EEPFIdPhotons[i];
    }
    for(unsigned int i = 0; i <vetos2012EBNoPFIdCharged.size(); i++){
      delete vetos2012EBNoPFIdCharged[i];
    }
    for(unsigned int i = 0; i <vetos2012EBNoPFIdNeutral.size(); i++){
      delete vetos2012EBNoPFIdNeutral[i];
    }
    for(unsigned int i = 0; i <vetos2012EBNoPFIdPhotons.size(); i++){
      delete vetos2012EBNoPFIdPhotons[i];
    }
    for(unsigned int i = 0; i <vetos2012EENoPFIdCharged.size(); i++){
      delete vetos2012EENoPFIdCharged[i];
    }
    for(unsigned int i = 0; i <vetos2012EENoPFIdNeutral.size(); i++){
      delete vetos2012EENoPFIdNeutral[i];
    }
    for(unsigned int i = 0; i <vetos2012EENoPFIdPhotons.size(); i++){
      delete vetos2012EENoPFIdPhotons[i];
    }
   
    aElectron.addUserInt("pfId",pfId);
    aElectron.addUserFloat("isInRun",iEvent.run());

    electronsUserEmbeddedColl->push_back(aElectron);
    
  }


  iEvent.put( electronsUserEmbeddedColl );
  return;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ElectronsUserEmbedded);


