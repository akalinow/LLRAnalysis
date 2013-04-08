#include "LLRAnalysis/Utilities/interface/UserIsolatedPatTau.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <vector>
#include <string>

struct PatTauInfo
{
  PatTauInfo(){};
  const pat::Tau* tau_;
  bool triggerMatch_;
  bool passIsoDiscr_;
  double rawIso_;
};

struct SortTauDescendingIsolation
{
  bool operator() (const PatTauInfo t1, const PatTauInfo t2)
  {
    // 1st criterion: trigger matching
    if ( t1.triggerMatch_ > t2.triggerMatch_ ) return true;
    if ( t1.triggerMatch_ < t2.triggerMatch_ ) return false;
    // 2nd criterion: passing Loose Isolation
    if ( t1.passIsoDiscr_ > t2.passIsoDiscr_ ) return true;
    if ( t1.passIsoDiscr_ < t2.passIsoDiscr_ ) return false;
    // 3rd criterion: Raw Isolation Value
    return (t1.rawIso_ < t2.rawIso_); 
  }
};

UserIsolatedPatTau::UserIsolatedPatTau(const edm::ParameterSet & iConfig){

  patTauTag_ = iConfig.getParameter<edm::InputTag>("patTauTag"); 
  isMC_ = iConfig.getParameter<bool>("isMC");
  verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);
  useIsoMVA_ = iConfig.getUntrackedParameter<bool>("useIsoMVA",true);
  minPt_ = iConfig.getParameter<double>("pt"); 
  maxEta_ = iConfig.getParameter<double>("eta"); 

  produces<pat::TauCollection>("");
}

UserIsolatedPatTau::~UserIsolatedPatTau(){
}

void UserIsolatedPatTau::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<pat::TauCollection> patTausHandle;
  iEvent.getByLabel(patTauTag_,patTausHandle);
  const pat::TauCollection* patTaus = patTausHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();

  std::auto_ptr< pat::TauCollection > UserIsolatedTausColl( new pat::TauCollection() ) ;

  //HLT Filter for match
  std::vector<std::string> HLTfiltersTau;
  if(isMC_){
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoEle20WP90LooseIsoPFTau20");
  }
  else {
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    //HLTfiltersTau.push_back("hltOverlapFilterIsoMu8LooseIsoPFTau20"); //not in MC, so don't match indata as well
    HLTfiltersTau.push_back("hltOverlapFilterIsoEle20LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoEle20WP90LooseIsoPFTau20");
    //HLTfiltersTau.push_back("hltOverlapFilterIsoEle13WP90LooseIsoPFTau20"); //not in MC, so don't match indata as well
  }
  //select the most isolated tau
  
  const pat::Tau* mostIsolatedTau = 0;
  std::vector<PatTauInfo> tmpPatTauInfos;
  if(verbose_)
    std::cout<<"[UserIsolatedPatTau]: New Event "<<std::endl;
  for ( pat::TauCollection::const_iterator patTau = patTaus->begin(); 
	patTau != patTaus->end(); ++patTau ) { 
    const pat::Tau* tau = &(*patTau);

    if(tau->pt() < minPt_ || fabs(tau->eta()) > maxEta_)continue;
    
    PatTauInfo tmpPatTauInfo;
    tmpPatTauInfo.tau_ = tau;
    if(useIsoMVA_){
      //tmpPatTauInfo.passIsoDiscr_ = ( patTau->tauID("byIsolationMVA2raw")>0.7 );
      tmpPatTauInfo.passIsoDiscr_ = ( patTau->tauID("byLooseIsolationMVA2")>0.5 );
      tmpPatTauInfo.rawIso_ = -patTau->tauID("byIsolationMVA2raw");
    }
    else{
      tmpPatTauInfo.passIsoDiscr_ = ( patTau->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5 );
      tmpPatTauInfo.rawIso_ = patTau->tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");
    }
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
    tmpPatTauInfo.triggerMatch_ = isTriggerMatched;
    
    tmpPatTauInfos.push_back(tmpPatTauInfo);
    if(verbose_)
      std::cout<<"\ttau rawIso="<< (useIsoMVA_ ? -tmpPatTauInfo.rawIso_ : tmpPatTauInfo.rawIso_) 
	       <<", iso "<<tmpPatTauInfo.passIsoDiscr_
	       <<", trig match "<<tmpPatTauInfo.triggerMatch_
	       <<std::endl;
  }
  
  std::sort(tmpPatTauInfos.begin(), tmpPatTauInfos.end(), SortTauDescendingIsolation());
  mostIsolatedTau = tmpPatTauInfos.size() > 0 ? tmpPatTauInfos.front().tau_ : 0;
  if ( mostIsolatedTau ) {
    UserIsolatedTausColl->push_back(*mostIsolatedTau);
  }
  //if(verbose_) std::cout<<" size of output tau coll "<<UserIsolatedTausColl->size()<<std::endl;
  if(verbose_){
    if(mostIsolatedTau)
      std::cout<<"Selected tau rawIso "<< ( useIsoMVA_ ? mostIsolatedTau->tauID("byIsolationMVA2raw") : mostIsolatedTau->tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") )
	//<<" Iso "<< ( useIsoMVA_ ? mostIsolatedTau->tauID("byIsolationMVA2raw")>0.7 : mostIsolatedTau->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5 ) 
	       <<" Iso "<< ( useIsoMVA_ ? mostIsolatedTau->tauID("byLooseIsolationMVA2")>0.5 : mostIsolatedTau->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5 ) 
	       <<" trig match "<<tmpPatTauInfos.front().triggerMatch_
	       <<std::endl;
    else
      std::cout<<"Tau collection empty!"<<std::endl; 
  }
  iEvent.put( UserIsolatedTausColl );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserIsolatedPatTau);


