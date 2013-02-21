#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "LLRAnalysis/Utilities/interface/PUWeight.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "TLorentzVector.h"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include <vector>
#include <utility>
#include <map>

using namespace edm;
using namespace std;

class UserDefinedVariablesMuTnP : public edm::EDProducer {
public:
  explicit UserDefinedVariablesMuTnP(const edm::ParameterSet & iConfig);
  virtual ~UserDefinedVariablesMuTnP() ;
  
  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

  virtual void beginJob();

private:

  typedef std::vector<double> vdouble;

  edm::InputTag srcMuons_;
  edm::InputTag triggerResultsTag_;
  edm::InputTag met_;            
  bool isMC_;
  edm::LumiReWeighting LumiWeights2011A_;
  edm::LumiReWeighting LumiWeights2011B_;
  edm::LumiReWeighting LumiWeights2012A_;
  vdouble TrueDist2011A_f_;
  vdouble TrueDist2011B_f_;
  vdouble TrueDist2012A_f_;
  vdouble MCDist_f_;
  bool debug_;
};

UserDefinedVariablesMuTnP::UserDefinedVariablesMuTnP(const edm::ParameterSet & iConfig) :
  srcMuons_(iConfig.getParameter<edm::InputTag>("srcMuons")),
  triggerResultsTag_(iConfig.getParameter<edm::InputTag>("triggerResults")), 
  met_(iConfig.getParameter<edm::InputTag>("met")),
  isMC_(iConfig.getParameter<bool>("isMC")),
  TrueDist2011A_f_(iConfig.getParameter<vdouble>("TrueDist2011A")),
  TrueDist2011B_f_(iConfig.getParameter<vdouble>("TrueDist2011B")),
  TrueDist2012A_f_(iConfig.getParameter<vdouble>("TrueDist2012A")),
  MCDist_f_(iConfig.getParameter<vdouble>("MCDist")),
  debug_(iConfig.getParameter<bool>("debug"))
{
  produces<edm::ValueMap<float> >("puMCWeightRun2011A");
  produces<edm::ValueMap<float> >("puMCWeightRun2011B");
  produces<edm::ValueMap<float> >("puMCWeightRun2012A");
  produces<edm::ValueMap<float> >("Mt");
  produces<edm::ValueMap<float> >("HLTIsoMu24Bit");
  produces<edm::ValueMap<float> >("HLTIsoMu24Match");
  produces<edm::ValueMap<float> >("hltIsoMu20Match");
  produces<edm::ValueMap<float> >("genDecay");
  produces<edm::ValueMap<float> >("numGenPU");
}


UserDefinedVariablesMuTnP::~UserDefinedVariablesMuTnP()
{
}

void UserDefinedVariablesMuTnP::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {

  ////////////////////////////////////////////////////////////////
  //////// Handle
  ////////////////////////////////////////////////////////////////
  Handle<View<pat::Muon> > muonsHandle;
  iEvent.getByLabel(srcMuons_, muonsHandle);
  
  edm::Handle<pat::METCollection> metHandle;
  iEvent.getByLabel(met_,metHandle);
  const pat::METCollection* met = metHandle.product();
  
  edm::Handle<pat::TriggerEvent> triggerHandle;
  iEvent.getByLabel(triggerResultsTag_, triggerHandle);
  const pat::TriggerEvent* trigger = triggerHandle.product();
  
  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();
  
  
  int genDecay = -99;
  
  edm::Handle<reco::GenParticleCollection> genHandle;
  if(isMC_){
    iEvent.getByLabel(edm::InputTag("genParticles"),genHandle);
    const reco::GenParticleCollection* genParticles = genHandle.product();
    for(unsigned int k = 0; k < genParticles->size(); k ++){
      if( !( (*genParticles)[k].pdgId() == 23 || abs((*genParticles)[k].pdgId()) == 24 ) || (*genParticles)[k].status()!=3)
	continue;
	
      genDecay = (*genParticles)[k].pdgId();
	
      int breakLoop = 0;
      for(unsigned j = 0; j< ((*genParticles)[k].daughterRefVector()).size() && breakLoop!=1; j++){
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 11 ){
	  genDecay *= 11;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 13 ){
	  genDecay *= 13;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 15 ){
	  genDecay *= 15;
	  breakLoop = 1;
	}
      }
      break;
    }
  }


  ////////////////////////////////////////////////////////////////
  //////// Prepare vectors for output
  ////////////////////////////////////////////////////////////////
  std::vector<float> valuesPUweight2011A;
  std::vector<float> valuesPUweight2011B;
  std::vector<float> valuesPUweight2012A;
  std::vector<float> valuesMt;
  std::vector<float> valuesGenDecay;
  std::vector<float> valuesNumGenPU;
  std::vector<float> HLTIsoMu24Bits_;
  std::vector<float> HLTIsoMu24Match_;
  std::vector<float> hltIsoMu20Match_;

  ////////////////////////////////////////////////////////////////
  //////// PU weights
  ////////////////////////////////////////////////////////////////
  int nPUVertices = -99;
  float mcPUweight2011A = 1;
  float mcPUweight2011B = 1;
  float mcPUweight2012A = 1;
  if(isMC_){
    edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
    iEvent.getByLabel(edm::InputTag("addPileupInfo"), PupInfo);
      
    std::vector<PileupSummaryInfo>::const_iterator PVI;
      
    if(PupInfo.isValid()){

      for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {
	nPUVertices = PVI->getTrueNumInteractions();
	// 	nPUVertices = PVI->getPU_NumInteractions();
	if (debug_){
	  cout<<"PU info valid"<<endl;
	  cout<<"Num int :"<<nPUVertices<<endl;
	}
      }
    }
    else cout<<"PU info not valid !"<<endl;
      
    mcPUweight2011A = LumiWeights2011A_.weight(nPUVertices);
    mcPUweight2011B = LumiWeights2011B_.weight(nPUVertices);
    mcPUweight2012A = LumiWeights2012A_.weight(nPUVertices);
  }
  if(debug_){
    cout << "PU Weights  --  NumPV"<<endl;
    cout << mcPUweight2011A << "       -- " << nPUVertices << endl;
    cout << mcPUweight2011B << "       -- " << nPUVertices << endl;
    cout << mcPUweight2012A << "       -- " << nPUVertices << endl;
    cout << " ------------------------------------------------------------"<< endl;
  }

  float numGenPU   = nPUVertices;

  string triggerPathName;
  string triggerFilterLabel;
  if(isMC_){
    triggerPathName = "HLT_IsoMu24_eta2p1_v";
    triggerFilterLabel = "hltL3crIsoL1sMu18erORMu20erL1f0L2f18QL3f20QL3crIsoFiltered10";
  }
  else{//Data
    triggerPathName = "HLT_IsoMu24_eta2p1_v";
    triggerFilterLabel = "hltL3crIsoL1sMu18erORMu20erL1f0L2f18QL3f20QL3crIsoFiltered10";
  }


  View<pat::Muon>::const_iterator myMuon; 
  for (myMuon = muonsHandle->begin(); myMuon != muonsHandle->end(); ++myMuon) {
    
    float scalarSumPt = (myMuon->p4()).Pt() + ((*met)[0].p4()).Pt();
    float vectorSumPt = (myMuon->p4() + (*met)[0].p4()).Pt() ;
    float Mt = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;

    valuesPUweight2011A.push_back(mcPUweight2011A);
    valuesPUweight2011B.push_back(mcPUweight2011B);
    valuesPUweight2012A.push_back(mcPUweight2012A);
    valuesMt.push_back(Mt);
    valuesGenDecay.push_back(fabs(genDecay));
    valuesNumGenPU.push_back(numGenPU);


    ////////////////////////////////////////////////////////////////
    //////// Triggers
    ////////////////////////////////////////////////////////////////
    bool matchedPath = false;
    bool matchedFilter = false;
    string MatchedtriggerPathName = "";
    string MatchedtriggerFilterLabel = "";
    if(trigger){
      for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
	pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));

	if(debug_) {
	  if( Geom::deltaR( aObj->triggerObject().p4(), myMuon->p4() )<0.3 ){
	    for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
	      cout << "Object passing " << (aObj->filterLabels())[k] << " within 0.3 of Muon" << endl;
	    }
	    for(unsigned int k =0; k < (aObj->pathNames()).size() ; k++){
	      cout << "Object passing " << (aObj->pathNames())[k] << " within 0.3 of Muon" << endl;
	    }
	  }
	}

	for(unsigned int k =0; k < (aObj->pathNames()).size() ; k++){
	  if (Geom::deltaR( aObj->triggerObject().p4(), myMuon->p4() ) < 0.3 && (aObj->pathNames())[k].find(triggerPathName) != std::string::npos) {
	    MatchedtriggerPathName = (aObj->pathNames())[k];
	    matchedPath = true;
	    if (debug_)cout<<"Trigger Path Matched !"<<endl;
	  }
	}

	for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
	  if (Geom::deltaR( aObj->triggerObject().p4(), myMuon->p4() ) < 0.3 && (aObj->filterLabels())[k].find(triggerFilterLabel) != std::string::npos) {
	    MatchedtriggerFilterLabel = (aObj->filterLabels())[k];
	    matchedFilter = true;
	    if (debug_)cout<<"Trigger Filter Matched !"<<endl;
	  }
	}

      }// loop trigger objects

      const pat::TriggerPath *triggerPath =  trigger->path(MatchedtriggerPathName);
      if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept() && triggerPath->prescale() ==1 ) HLTIsoMu24Bits_.push_back(1);
      else if (triggerPath && triggerPath->wasRun() && triggerPath->wasAccept() && triggerPath->prescale()!=1) HLTIsoMu24Bits_.push_back(2);
      else HLTIsoMu24Bits_.push_back(0);
      if (debug_){
	cout<<  "============================================================" << endl;
	cout<<  "Testing " << MatchedtriggerPathName << endl;
	if(triggerPath) cout << "Is there..." << endl;
	if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
	if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
      }
      
    }// if trigger

    if(matchedPath) HLTIsoMu24Match_.push_back(1);
    else HLTIsoMu24Match_.push_back(0);
    
    if(matchedFilter) hltIsoMu20Match_.push_back(1);
    else hltIsoMu20Match_.push_back(0);

    if(debug_){
      if(matchedPath) cout << "Muon matched within dR=0.3 with trigger object passing path " << MatchedtriggerPathName << endl;
      if(matchedFilter) cout << "Muon matched within dR=0.3 with trigger object passing filter " << MatchedtriggerFilterLabel << endl;
      else cout << "!!! Muon is not trigger matched within dR=0.3 !!!" << endl;
    }


  }//loop on patMuons


  ////////////////////////////////////////////////////////////////
  //////// convert into ValueMap and store
  ////////////////////////////////////////////////////////////////
  std::auto_ptr<ValueMap<float> > valMapPUweight2011A(new ValueMap<float>());
  ValueMap<float>::Filler fillerPUweight2011A(*valMapPUweight2011A);
  fillerPUweight2011A.insert(muonsHandle, valuesPUweight2011A.begin(), valuesPUweight2011A.end());
  fillerPUweight2011A.fill();
  iEvent.put(valMapPUweight2011A, "puMCWeightRun2011A");

  std::auto_ptr<ValueMap<float> > valMapPUweight2011B(new ValueMap<float>());
  ValueMap<float>::Filler fillerPUweight2011B(*valMapPUweight2011B);
  fillerPUweight2011B.insert(muonsHandle, valuesPUweight2011B.begin(), valuesPUweight2011B.end());
  fillerPUweight2011B.fill();
  iEvent.put(valMapPUweight2011B, "puMCWeightRun2011B");

  std::auto_ptr<ValueMap<float> > valMapPUweight2012A(new ValueMap<float>());
  ValueMap<float>::Filler fillerPUweight2012A(*valMapPUweight2012A);
  fillerPUweight2012A.insert(muonsHandle, valuesPUweight2012A.begin(), valuesPUweight2012A.end());
  fillerPUweight2012A.fill();
  iEvent.put(valMapPUweight2012A, "puMCWeightRun2012A");

  std::auto_ptr<ValueMap<float> > valMapMt(new ValueMap<float>());
  ValueMap<float>::Filler fillerMt(*valMapMt);
  fillerMt.insert(muonsHandle, valuesMt.begin(), valuesMt.end());
  fillerMt.fill();
  iEvent.put(valMapMt, "Mt");

  std::auto_ptr<ValueMap<float> > valMapHLTIsoMu24Bit(new ValueMap<float>());
  ValueMap<float>::Filler fillerHLTIsoMu24Bit(*valMapHLTIsoMu24Bit);
  fillerHLTIsoMu24Bit.insert(muonsHandle, HLTIsoMu24Bits_.begin(), HLTIsoMu24Bits_.end());
  fillerHLTIsoMu24Bit.fill();
  iEvent.put(valMapHLTIsoMu24Bit, "HLTIsoMu24Bit"); 

  std::auto_ptr<ValueMap<float> > valMapHLTIsoMu24Match(new ValueMap<float>());
  ValueMap<float>::Filler fillerHLTIsoMu24Match(*valMapHLTIsoMu24Match);
  fillerHLTIsoMu24Match.insert(muonsHandle, HLTIsoMu24Match_.begin(), HLTIsoMu24Match_.end());
  fillerHLTIsoMu24Match.fill();
  iEvent.put(valMapHLTIsoMu24Match, "HLTIsoMu24Match");

  std::auto_ptr<ValueMap<float> > valMaphltIsoMu20Match(new ValueMap<float>());
  ValueMap<float>::Filler fillerhltIsoMu20Match(*valMaphltIsoMu20Match);
  fillerhltIsoMu20Match.insert(muonsHandle, hltIsoMu20Match_.begin(), hltIsoMu20Match_.end());
  fillerhltIsoMu20Match.fill();
  iEvent.put(valMaphltIsoMu20Match, "hltIsoMu20Match");

  std::auto_ptr<ValueMap<float> > valMapGenDecay(new ValueMap<float>());
  ValueMap<float>::Filler fillerGenDecay(*valMapGenDecay);
  fillerGenDecay.insert(muonsHandle, valuesGenDecay.begin(), valuesGenDecay.end());
  fillerGenDecay.fill();
  iEvent.put(valMapGenDecay, "genDecay");

  std::auto_ptr<ValueMap<float> > valMapNumGenPU(new ValueMap<float>());
  ValueMap<float>::Filler fillerNumGenPU(*valMapNumGenPU);
  fillerNumGenPU.insert(muonsHandle, valuesNumGenPU.begin(), valuesNumGenPU.end());
  fillerNumGenPU.fill();
  iEvent.put(valMapNumGenPU, "numGenPU");

}

void
UserDefinedVariablesMuTnP::beginJob()
{

  if(isMC_){

    std::vector< float > MCDist ;
    std::vector< float > TrueDist2011A;
    std::vector< float > TrueDist2011B;
    std::vector< float > TrueDist2012A;

    int sizeMCDist_f_ = MCDist_f_.size();

    for( int i=0; i<sizeMCDist_f_; ++i) {
      TrueDist2011A.push_back(TrueDist2011A_f_[i]);
      MCDist.push_back(MCDist_f_[i]);
    }

    for( int i=0; i<sizeMCDist_f_; ++i) {
      TrueDist2011B.push_back(TrueDist2011B_f_[i]);
    }

    for( int i=0; i<sizeMCDist_f_; ++i) {
      TrueDist2012A.push_back(TrueDist2012A_f_[i]);
    }

    //     std::cout<<"Sizes"<<endl;
    //     std::cout<<MCDist_f_.size()<<" "<<TrueDist2011A_f_.size()<<" "<<TrueDist2011B_f_.size()<<" "<<TrueDist2012A_f_.size()<<std::endl;
    //     std::cout<<MCDist.size()<<" "<<TrueDist2011A.size()<<" "<<TrueDist2011B.size()<<" "<<TrueDist2012A.size()<<std::endl;

    LumiWeights2011A_ = edm::LumiReWeighting(MCDist, TrueDist2011A);
    LumiWeights2011B_ = edm::LumiReWeighting(MCDist, TrueDist2011B);
    LumiWeights2012A_ = edm::LumiReWeighting(MCDist, TrueDist2012A);

  }

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserDefinedVariablesMuTnP);
