#include "LLRAnalysis/Utilities/interface/UserIsolatedPatElectron.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <vector>
#include <string>

struct PatElectronInfo
{
  PatElectronInfo(){};
  const pat::Electron* ele_;
  bool triggerMatch_;
  double relIso_;
};

struct SortElectronDescendingIsolation
{
  bool operator() (const PatElectronInfo t1, const PatElectronInfo t2)
  {
    // 1st criterion: trigger matching
    if ( t1.triggerMatch_ > t2.triggerMatch_ ) return true;
    if ( t1.triggerMatch_ < t2.triggerMatch_ ) return false;
    // 2nd criterion: Rel Isolation Value
    return (t1.relIso_ < t2.relIso_); 
  }
};

UserIsolatedPatElectron::UserIsolatedPatElectron(const edm::ParameterSet & iConfig){

  ElectronTag_ = iConfig.getParameter<edm::InputTag>("ElectronTag"); 
  isMC_ = iConfig.getParameter<bool>("isMC");
  verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);
  userIso_ = iConfig.getUntrackedParameter<std::string>("userIso");
  minPt_ = iConfig.getParameter<double>("pt");  
  maxEta_ = iConfig.getParameter<double>("eta"); 
  
  produces<pat::ElectronCollection>("");
}

UserIsolatedPatElectron::~UserIsolatedPatElectron(){
}

void UserIsolatedPatElectron::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::ElectronCollection> patElectronsHandle;
  iEvent.getByLabel(ElectronTag_,patElectronsHandle);
  const pat::ElectronCollection* patElectrons = patElectronsHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();

  std::auto_ptr< pat::ElectronCollection > UserIsolatedElectronsColl( new pat::ElectronCollection() ) ;

  //HLT Filter for match
  std::vector<std::string> HLTfiltersElectron;
  if(isMC_){ 
    HLTfiltersElectron.push_back("hltOverlapFilterIsoEle20WP90LooseIsoPFTau20"); 
  } 
  else { 
    HLTfiltersElectron.push_back("hltOverlapFilterIsoEle20LooseIsoPFTau20"); 
    HLTfiltersElectron.push_back("hltOverlapFilterIsoEle20WP90LooseIsoPFTau20"); 
    HLTfiltersElectron.push_back("hltOverlapFilterIsoEle13WP90LooseIsoPFTau20"); 
  } 
  //select the most isolated tau
  
  const pat::Electron* mostIsolatedElectron = 0;
  std::vector<PatElectronInfo> tmpPatElectronInfos;
  if(verbose_)
    std::cout<<"[UserIsolatedPatElectron]: New Event "<<std::endl;
  for ( pat::ElectronCollection::const_iterator patElectron = patElectrons->begin(); 
	patElectron != patElectrons->end(); ++patElectron ) { 
    const pat::Electron* electron = &(*patElectron);

    if(electron->pt() < minPt_ || fabs(electron->eta()) > maxEta_)continue;

    PatElectronInfo tmpPatElectronInfo;
    tmpPatElectronInfo.ele_ = electron;
    tmpPatElectronInfo.relIso_ = patElectron->userFloat(userIso_);

    bool isTriggerMatched = false;
    for ( std::vector<std::string>::const_iterator triggerPath = HLTfiltersElectron.begin();
	  triggerPath != HLTfiltersElectron.end(); ++triggerPath ) {
      for ( pat::TriggerObjectStandAloneCollection::const_iterator triggerElectron = triggerObjs->begin(); 
	    triggerElectron !=triggerObjs->end(); ++triggerElectron ) {
	pat::TriggerObjectStandAlone* aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*triggerElectron));
	if( Geom::deltaR( aObj->triggerObject().p4(), electron->p4() ) <0.5 && aObj->hasFilterLabel(*triggerPath) && aObj->hasTriggerObjectType(trigger::TriggerElectron) ){
	  isTriggerMatched = true;
	}
      }
    }
    tmpPatElectronInfo.triggerMatch_ = isTriggerMatched;
    
    tmpPatElectronInfos.push_back(tmpPatElectronInfo);
    if(verbose_)
      std::cout<<"\tmu relIso = "<< tmpPatElectronInfo.relIso_ 
	       <<", trig match "<<tmpPatElectronInfo.triggerMatch_
	       <<std::endl;
  }
  
  std::sort(tmpPatElectronInfos.begin(), tmpPatElectronInfos.end(), SortElectronDescendingIsolation());
  mostIsolatedElectron = tmpPatElectronInfos.size() > 0 ? tmpPatElectronInfos.front().ele_ : 0;
  if ( mostIsolatedElectron ) {
    UserIsolatedElectronsColl->push_back(*mostIsolatedElectron);
  }
  //if(verbose_) std::cout<<" size of output electron coll "<<UserIsolatedElectronsColl->size()<<std::endl;
  if(verbose_){
    if(mostIsolatedElectron)
      std::cout<<"Selected electron relIso "<< mostIsolatedElectron->userFloat(userIso_)
	       <<" trig match "<<tmpPatElectronInfos.front().triggerMatch_
	       <<std::endl;
    else
      std::cout<<"Electron collection empty!"<<std::endl; 
  }
  iEvent.put( UserIsolatedElectronsColl );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserIsolatedPatElectron);
