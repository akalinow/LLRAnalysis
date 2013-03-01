#include "LLRAnalysis/Utilities/interface/GetTauMatchedCaloJet.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <vector>
#include <string>

GetTauMatchedCaloJet::GetTauMatchedCaloJet(const edm::ParameterSet & iConfig){

  jetTag_       = iConfig.getParameter<edm::InputTag>("jetTag"); 
  genTauJetTag_ = iConfig.getParameter<edm::InputTag>("genTauJetTag"); 
  deltaRMax_    = iConfig.getParameter<double>("deltaRMax");
  verbose_      = iConfig.getUntrackedParameter<bool>("verbose",false);
  
  produces<reco::CaloJetCollection>("");
}

GetTauMatchedCaloJet::~GetTauMatchedCaloJet(){
}

void GetTauMatchedCaloJet::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){

  edm::Handle<reco::CaloJetCollection> caloJetsHandle;
  iEvent.getByLabel(jetTag_, caloJetsHandle);
//   if( !caloJetsHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//      << "No caloJets '"<<jetTag_<<"' available \n";
  const reco::CaloJetCollection *caloJets = caloJetsHandle.product();

  edm::Handle<reco::GenJetCollection> genTauJetsHandle;
  iEvent.getByLabel(genTauJetTag_, genTauJetsHandle);
//   if( !genTauJetsHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//       << "No genTauJets '"<<genTauJetTag_<<"' available \n";
  const reco::GenJetCollection *genTaus = genTauJetsHandle.product();


  std::auto_ptr<reco::CaloJetCollection> genTauMatchedJets( new reco::CaloJetCollection() ) ;

  if(verbose_)
    std::cout<<"[GenTauMatchedCaloJet]: New Event "<<std::endl;
  for(reco::CaloJetCollection::const_iterator caloJet = caloJets->begin(); 
	caloJet != caloJets->end(); ++caloJet) { 
    for(reco::GenJetCollection::const_iterator genTau = genTaus->begin(); 
	genTau != genTaus->end(); ++genTau) { 
      float deltaR=reco::deltaR(genTau->eta(),  genTau->phi(),
				caloJet->eta(), caloJet->phi() );
      if(deltaR<deltaRMax_){
	genTauMatchedJets->push_back(*caloJet);
	if(verbose_)
	  std::cout<<"\t genTauJet(Pt,eta,phi): "
		   <<genTau->pt()<<", "<<genTau->eta()<<", "<<genTau->phi()<<std::endl
		   <<"\t caloJet(Pt,eta,phi):   "
		   <<caloJet->pt()<<", "<<caloJet->eta()<<", "<<caloJet->phi()<<std::endl
		   <<"deltaR(jet,tau) = "<<deltaR<<std::endl
		   <<std::endl;
      }
    }
  }
  if(verbose_ && genTauMatchedJets->size()==0)
    std::cout<<"[GenTauMatchedCaloJet]: There are not caloJets matched to genTauJets!"<<std::endl; 
  

  iEvent.put(genTauMatchedJets);
return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GetTauMatchedCaloJet);
