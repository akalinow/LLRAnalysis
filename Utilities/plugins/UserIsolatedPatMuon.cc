#include "LLRAnalysis/Utilities/interface/UserIsolatedPatMuon.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <vector>
#include <string>

struct PatMuonInfo
{
  PatMuonInfo(){};
  const pat::Muon* mu_;
  bool triggerMatch_;
  double relIso_;
};

struct SortMuonDescendingIsolation
{
  bool operator() (const PatMuonInfo t1, const PatMuonInfo t2)
  {
    // 1st criterion: trigger matching
    if ( t1.triggerMatch_ > t2.triggerMatch_ ) return true;
    if ( t1.triggerMatch_ < t2.triggerMatch_ ) return false;
    // 2nd criterion: Rel Isolation Value
    return (t1.relIso_ < t2.relIso_); 
  }
};

UserIsolatedPatMuon::UserIsolatedPatMuon(const edm::ParameterSet & iConfig){

  MuonTag_ = iConfig.getParameter<edm::InputTag>("MuonTag"); 
  isMC_ = iConfig.getParameter<bool>("isMC");
  verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);
  userIso_ = iConfig.getUntrackedParameter<std::string>("userIso");
  minPt_ = iConfig.getParameter<double>("pt");  
  maxEta_ = iConfig.getParameter<double>("eta"); 

  produces<pat::MuonCollection>("");
}

UserIsolatedPatMuon::~UserIsolatedPatMuon(){
}

void UserIsolatedPatMuon::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::MuonCollection> patMuonsHandle;
  iEvent.getByLabel(MuonTag_,patMuonsHandle);
  const pat::MuonCollection* patMuons = patMuonsHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();

  std::auto_ptr< pat::MuonCollection > UserIsolatedMuonsColl( new pat::MuonCollection() ) ;

  //HLT Filter for match
  std::vector<std::string> HLTfiltersMuon;
  if(isMC_){
    HLTfiltersMuon.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
  }
  else {
    HLTfiltersMuon.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");
    HLTfiltersMuon.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    //HLTfiltersMuon.push_back("hltOverlapFilterIsoMu8LooseIsoPFTau20"); //not in MC, so don't match in data as well 
  }
  //select the most isolated tau
  
  const pat::Muon* mostIsolatedMuon = 0;
  std::vector<PatMuonInfo> tmpPatMuonInfos;
  if(verbose_)
    std::cout<<"[UserIsolatedPatMuon]: New Event "<<std::endl;
  for ( pat::MuonCollection::const_iterator patMuon = patMuons->begin(); 
	patMuon != patMuons->end(); ++patMuon ) { 
    const pat::Muon* muon = &(*patMuon);

    if(muon->pt() < minPt_ || fabs(muon->eta()) > maxEta_)continue;

    PatMuonInfo tmpPatMuonInfo;
    tmpPatMuonInfo.mu_ = muon;
    tmpPatMuonInfo.relIso_ = patMuon->userFloat(userIso_);

    bool isTriggerMatched = false;
    for ( std::vector<std::string>::const_iterator triggerPath = HLTfiltersMuon.begin();
	  triggerPath != HLTfiltersMuon.end(); ++triggerPath ) {
      for ( pat::TriggerObjectStandAloneCollection::const_iterator triggerMuon = triggerObjs->begin(); 
	    triggerMuon !=triggerObjs->end(); ++triggerMuon ) {
	pat::TriggerObjectStandAlone* aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*triggerMuon));
	if( Geom::deltaR( aObj->triggerObject().p4(), muon->p4() ) <0.5 && aObj->hasFilterLabel(*triggerPath) && aObj->hasTriggerObjectType(trigger::TriggerMuon) ){
	  isTriggerMatched = true;
	}
      }
    }
    tmpPatMuonInfo.triggerMatch_ = isTriggerMatched;
    
    tmpPatMuonInfos.push_back(tmpPatMuonInfo);
    if(verbose_)
      std::cout<<"\tmu relIso = "<< tmpPatMuonInfo.relIso_ 
	       <<", trig match "<<tmpPatMuonInfo.triggerMatch_
	       <<std::endl;
  }
  
  std::sort(tmpPatMuonInfos.begin(), tmpPatMuonInfos.end(), SortMuonDescendingIsolation());
  mostIsolatedMuon = tmpPatMuonInfos.size() > 0 ? tmpPatMuonInfos.front().mu_ : 0;
  if ( mostIsolatedMuon ) {
    UserIsolatedMuonsColl->push_back(*mostIsolatedMuon);
  }
  //if(verbose_) std::cout<<" size of output muon coll "<<UserIsolatedMuonsColl->size()<<std::endl;
  if(verbose_){
    if(mostIsolatedMuon)
      std::cout<<"Selected muon relIso "<< mostIsolatedMuon->userFloat(userIso_)
	       <<" trig match "<<tmpPatMuonInfos.front().triggerMatch_
	       <<std::endl;
    else
      std::cout<<"Muon collection empty!"<<std::endl; 
  }
  iEvent.put( UserIsolatedMuonsColl );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserIsolatedPatMuon);
