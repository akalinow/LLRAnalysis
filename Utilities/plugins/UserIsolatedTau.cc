#include "LLRAnalysis/Utilities/interface/UserIsolatedTau.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <vector>
#include <string>

UserIsolatedTau::UserIsolatedTau(const edm::ParameterSet & iConfig){

  tauTag_    = iConfig.getParameter<edm::InputTag>("tauTag");
  patTauTag_ = iConfig.getParameter<edm::InputTag>("patTauTag"); 
  isMC_ = iConfig.getParameter<bool>("isMC");
  
  produces<reco::PFTauCollection>("");
}

UserIsolatedTau::~UserIsolatedTau(){
}

void UserIsolatedTau::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<reco::PFTauCollection> tausHandle;
  iEvent.getByLabel(tauTag_,tausHandle);
  const reco::PFTauCollection* taus = tausHandle.product();

  edm::Handle<pat::TauCollection> patTausHandle;
  iEvent.getByLabel(patTauTag_,patTausHandle);
  const pat::TauCollection* patTaus = patTausHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();

  std::auto_ptr< reco::PFTauCollection > UserIsolatedTausColl( new reco::PFTauCollection() ) ;

  //HLT Filter for match
  std::vector<std::string> HLTfiltersTau;
  if(isMC_){
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
  }
  else {
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
  }
  //select the most isolated tau
  const reco::PFTau* mostIsolatedTau = 0;
  double mostIsolatedTau_mvaValue = -9999.;
  for ( unsigned int i = 0; i < taus->size(); i++ ) {
    const reco::PFTau* tau = &(taus->at(i));
    bool isSelected = false;
    double mvaValue = -9999.;
    for ( pat::TauCollection::const_iterator patTau = patTaus->begin();
	  patTau != patTaus->end(); ++patTau ) {
      double dR = Geom::deltaR( patTau->p4(), tau->p4() );
      if ( dR < 0.1 ) {
	isSelected = true;
	if ( patTau->tauID("byIsolationMVAraw") > mvaValue ) mvaValue = patTau->tauID("byIsolationMVAraw");
      }
    }
    if ( !isSelected ) continue;

    bool isTriggerMatched = false;
    for ( std::vector<std::string>::const_iterator triggerPath = HLTfiltersTau.begin();
	  triggerPath != HLTfiltersTau.end(); ++triggerPath ) {
      for ( pat::TriggerObjectStandAloneCollection::const_iterator triggerTau = triggerObjs->begin(); 
	    triggerTau !=triggerObjs->end(); ++triggerTau ) {
	pat::TriggerObjectStandAlone* aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*triggerTau));
	if( Geom::deltaR( aObj->triggerObject().p4(), tau->p4() ) <0.5 && aObj->hasFilterLabel(*triggerPath) && aObj->hasTriggerObjectType(trigger::TriggerTau) ){
	  isTriggerMatched = true;
	}
      }
    }
    if ( !isTriggerMatched ) continue;

    if ( mvaValue > mostIsolatedTau_mvaValue ) {
      mostIsolatedTau = tau;
      mostIsolatedTau_mvaValue = mvaValue;
    }
  }
  
  if ( mostIsolatedTau ) {
    UserIsolatedTausColl->push_back(*mostIsolatedTau);
  }
  //std::cout<<" size of output tau coll "<<UserIsolatedTausColl->size()<<std::endl;

  iEvent.put( UserIsolatedTausColl );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserIsolatedTau);


