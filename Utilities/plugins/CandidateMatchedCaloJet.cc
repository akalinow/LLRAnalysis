#include "LLRAnalysis/Utilities/interface/CandidateMatchedCaloJet.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
//#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <vector>
#include <string>

CandidateMatchedCaloJet::CandidateMatchedCaloJet(const edm::ParameterSet & iConfig){

  jetTag_       = iConfig.getParameter<edm::InputTag>("jetTag"); 
  candidateTag_ = iConfig.getParameter<edm::InputTag>("candidateTag"); 
  deltaRMax_    = iConfig.getParameter<double>("deltaRMax");
  verbose_      = iConfig.getUntrackedParameter<bool>("verbose",false);
  
  produces<reco::CaloJetCollection>("");
}

CandidateMatchedCaloJet::~CandidateMatchedCaloJet(){
}

void CandidateMatchedCaloJet::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  if(verbose_)
    std::cout<<"[CandidateMatchedCaloJet]: New Event "<<std::endl;

  edm::Handle<reco::CaloJetCollection> caloJetsHandle;
  iEvent.getByLabel(jetTag_, caloJetsHandle);
//   if( !caloJetsHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//      << "No caloJets '"<<jetTag_<<"' available \n";
  const reco::CaloJetCollection *caloJets = caloJetsHandle.product();

  edm::Handle<reco::CandidateView> candidatesHandle;
  iEvent.getByLabel(candidateTag_, candidatesHandle);
//   if( !candidatesHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//       << "No candidate collection '"<<candidateTag_<<"' available \n";
  const reco::CandidateView *candidates;

  std::auto_ptr<reco::CaloJetCollection> matchedJets( new reco::CaloJetCollection() ) ;

  if( candidatesHandle.isValid() ){
    candidates = candidatesHandle.product();
    for(reco::CaloJetCollection::const_iterator caloJet = caloJets->begin(); 
	caloJet != caloJets->end(); ++caloJet) { 
      for(reco::CandidateView::const_iterator cand = candidates->begin(); 
	  cand != candidates->end(); ++cand) { 
	float deltaR=reco::deltaR(cand->eta(),  cand->phi(),
				  caloJet->eta(), caloJet->phi() );
	if(deltaR<deltaRMax_){
	  matchedJets->push_back(*caloJet);
	  if(verbose_)
	    std::cout<<"\t candidate(Pt,eta,phi): "
		     <<cand->pt()<<", "<<cand->eta()<<", "<<cand->phi()<<std::endl
		     <<"\t caloJet(Pt,eta,phi):   "
		     <<caloJet->pt()<<", "<<caloJet->eta()<<", "<<caloJet->phi()<<std::endl
		     <<"deltaR(jet,tau) = "<<deltaR<<std::endl
		     <<std::endl;
	}
      }
    }
  }
  if(verbose_ && matchedJets->size()==0)
    std::cout<<"[CandidateMatchedCaloJet]: There are not caloJets matched to candidates!"<<std::endl; 
  

  iEvent.put(matchedJets);
return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CandidateMatchedCaloJet);
