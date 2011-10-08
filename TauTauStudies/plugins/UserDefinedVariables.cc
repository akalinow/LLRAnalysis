#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "LLRAnalysis/Utilities/interface/PUWeight.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "TLorentzVector.h"

using namespace edm;
using namespace std;

class UserDefinedVariables : public edm::EDProducer {
public:
  explicit UserDefinedVariables(const edm::ParameterSet & iConfig);
  virtual ~UserDefinedVariables() ;
  
  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

private:
  edm::InputTag objects_;
  edm::InputTag met_;            
  PUWeight* fpuweight_;
  bool isMC_;
};

UserDefinedVariables::UserDefinedVariables(const edm::ParameterSet & iConfig) :
  objects_(iConfig.getParameter<edm::InputTag>("objects")),
  met_(iConfig.getParameter<edm::InputTag>("met")),
  isMC_(iConfig.getParameter<bool>("isMC"))
{
  fpuweight_ = new PUWeight();
  produces<edm::ValueMap<float> >("Mt");
  produces<edm::ValueMap<float> >("puMCWeight");
  produces<edm::ValueMap<float> >("numGenPU");
  produces<edm::ValueMap<float> >("genDecay");
}


UserDefinedVariables::~UserDefinedVariables()
{
  delete fpuweight_;
}

void UserDefinedVariables::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {

    // read input
    Handle<View<reco::Candidate> > objects;
    iEvent.getByLabel(objects_, objects);

    edm::Handle<pat::METCollection> metHandle;
    iEvent.getByLabel(met_,metHandle);
    const pat::METCollection* met = metHandle.product();

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

    int nPUVertices = -99;
    int nOOTPUVertices = -99;
    edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
    if(isMC_){
      iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfoH);

      if(puInfoH.isValid()){
	for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){
	  if(it->getBunchCrossing() ==0) nPUVertices = it->getPU_NumInteractions();
	  else  nOOTPUVertices = it->getPU_NumInteractions();
	}
      }
    }
    float mcPUweight = fpuweight_->GetWeight(nPUVertices);
    float numGenPU   = nPUVertices;


    // prepare vector for output   
    std::vector<float> values;
    std::vector<float> values2;
    std::vector<float> values3;
    std::vector<float> values4;

    View<reco::Candidate>::const_iterator object; 
    for (object = objects->begin(); object != objects->end(); ++object) {

      float scalarSumPt = (object->p4()).Pt() + ((*met)[0].p4()).Pt();
      float vectorSumPt = (object->p4() + (*met)[0].p4()).Pt() ;
      float Mt = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;

      values.push_back(Mt);
      values2.push_back(mcPUweight);
      values3.push_back(fabs(genDecay));
      values4.push_back(numGenPU);

    }

    // convert into ValueMap and store
    std::auto_ptr<ValueMap<float> > valMap(new ValueMap<float>());
    ValueMap<float>::Filler filler(*valMap);
    filler.insert(objects, values.begin(), values.end());
    filler.fill();
    iEvent.put(valMap, "Mt");

    std::auto_ptr<ValueMap<float> > valMap2(new ValueMap<float>());
    ValueMap<float>::Filler filler2(*valMap2);
    filler2.insert(objects, values2.begin(), values2.end());
    filler2.fill();
    iEvent.put(valMap2, "puMCWeight");

    std::auto_ptr<ValueMap<float> > valMap3(new ValueMap<float>());
    ValueMap<float>::Filler filler3(*valMap3);
    filler3.insert(objects, values3.begin(), values3.end());
    filler3.fill();
    iEvent.put(valMap3, "genDecay");

    std::auto_ptr<ValueMap<float> > valMap4(new ValueMap<float>());
    ValueMap<float>::Filler filler4(*valMap4);
    filler4.insert(objects, values4.begin(), values4.end());
    filler4.fill();
    iEvent.put(valMap4, "numGenPU");

}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UserDefinedVariables);
