#include "LLRAnalysis/TauTauStudies/interface/TauFakeRateAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"

#include "DataFormats/Math/interface/LorentzVector.h"

#include "Math/VectorUtil.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <utility>
#include <map>

using namespace std;
using namespace reco;


TauFakeRateAnalyzer::TauFakeRateAnalyzer(const edm::ParameterSet & iConfig){

  matchTo_ =  iConfig.existsAs<string>("matchTo") ? 
    iConfig.getParameter<string>("matchTo") : "default";
  tauTag_ = iConfig.getParameter<edm::InputTag>("tauTag");
  electronsTag_ = iConfig.getParameter<edm::InputTag>("electronsTag");

  fpuweight_ = new PUWeight();

}

void TauFakeRateAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","tau fake rate");
  tree_->Branch("leadPFChargedHadrMva",&leadPFChargedHadrMva_,"leadPFChargedHadrMva/D");
  tree_->Branch("leadPFChargedHadrHcalEnergy",&leadPFChargedHadrHcalEnergy_,"leadPFChargedHadrHcalEnergy/D");
  tree_->Branch("leadPFChargedHadrEcalEnergy",&leadPFChargedHadrEcalEnergy_,"leadPFChargedHadrEcalEnergy/D");
  tree_->Branch("leadPFChargedHadrTrackPt",&leadPFChargedHadrTrackPt_,"leadPFChargedHadrTrackPt/D");
  tree_->Branch("leadPFChargedHadrTrackP",&leadPFChargedHadrTrackP_,"leadPFChargedHadrTrackP/D");
  tree_->Branch("leadPFCandMva",&leadPFCandMva_,"leadPFCandMva/D");
  tree_->Branch("leadPFCandHcalEnergy",&leadPFCandHcalEnergy_,"leadPFCandHcalEnergy/D");
  tree_->Branch("leadPFCandEcalEnergy",&leadPFCandEcalEnergy_,"leadPFCandEcalEnergy/D");
  tree_->Branch("leadPFCandPt",&leadPFCandPt_,"leadPFCandPt/D");
  tree_->Branch("leadPFCandP",&leadPFCandP_,"leadPFCandP/D");
  tree_->Branch("emFraction",&emFraction_,"emFraction/D");
  tree_->Branch("hasGsf",&hasGsf_,"hasGsf/D");
  tree_->Branch("hasKft",&hasKft_,"hasKft/D");
  tree_->Branch("matchedID",&matchedID_,"matchedID/D");
  //tree_->Branch("fbrem",&fbrem_,"fbrem/D");
  tree_->Branch("signalPFChargedHadrCands",&signalPFChargedHadrCands_,"signalPFChargedHadrCands/I");
  tree_->Branch("signalPFGammaCands",&signalPFGammaCands_,"signalPFGammaCands/I");
  tree_->Branch("visMass",&visMass_,"visMass/D"); 
  tree_->Branch("pt",&pt_,"pt/D");
  tree_->Branch("eta",&eta_,"eta/D");

  tree_->Branch("numPV",&numPV_,"numPV/I");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/D");

  tree_->Branch("tightestHPSWP",&tightestHPSWP_,"tightestHPSWP/I");
  tree_->Branch("tightestAntiEWP",&tightestAntiEWP_,"tightestAntiEWP/I");
  tree_->Branch("tightestAntiMWP",&tightestAntiMWP_,"tightestAntiMWP/I");
  
}


TauFakeRateAnalyzer::~TauFakeRateAnalyzer(){
  delete fpuweight_;
}

void TauFakeRateAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){

  vector<math::XYZTLorentzVector> genMatchP4s;
  int ids[] = {95,90,85,80,70,60};

  edm::Handle<pat::TauCollection> tausHandle;
  iEvent.getByLabel(tauTag_,tausHandle);
  if( !tausHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No taus label available \n";
  const pat::TauCollection* taus = tausHandle.product();

  edm::Handle<pat::ElectronCollection> electronsHandle;
  iEvent.getByLabel(electronsTag_,electronsHandle);
  if( !electronsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No electrons label available \n";
  const pat::ElectronCollection* electrons = electronsHandle.product();

  int nPUVertices = -99;
  int nOOTPUVertices = -99;
  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfoH);
  if(puInfoH.isValid()){
    for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){
      if(it->getBunchCrossing() ==0) nPUVertices = it->getPU_NumInteractions();
      else  nOOTPUVertices = it->getPU_NumInteractions();
    }
  }
  mcPUweight_ = fpuweight_->GetWeight(nPUVertices);

  edm::Handle<reco::VertexCollection> pvHandle;
  edm::InputTag pvTag("offlinePrimaryVertices");
  iEvent.getByLabel(pvTag,pvHandle);
  if( !pvHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No PV label available \n";
  const reco::VertexCollection* vertexes = pvHandle.product();
  numPV_ = vertexes->size();

  edm::Handle<reco::GenParticleCollection> genParticlesHandle;
  const reco::GenParticleCollection* genParticles = 0;

  if(matchTo_.find("default")!=string::npos || matchTo_.find("tau")!=string::npos){
    edm::Handle<reco::GenJetCollection> tauGenJetsHandle;
    iEvent.getByLabel(edm::InputTag("genTauDecaysToHadrons"),tauGenJetsHandle);
    if( !tauGenJetsHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen jet label available \n";
    const reco::GenJetCollection* tauGenJets = tauGenJetsHandle.product();
    for(unsigned int j = 0; j<tauGenJets->size() ; j++){
      genMatchP4s.push_back( (*tauGenJets)[j].p4() );
    }
  } 
  else if( matchTo_.find("electron")!=string::npos ){
    iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesHandle);
    if( !genParticlesHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No genparticles label available \n";
    genParticles = genParticlesHandle.product();
    for(unsigned int j = 0; j<genParticles->size() ; j++){
      if( TMath::Abs((*genParticles)[j].pdgId())!=11  || (*genParticles)[j].pt()<10) continue;
      genMatchP4s.push_back( (*genParticles)[j].p4() );
    }
  }
  else if( matchTo_.find("muon")!=string::npos ){
    iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesHandle);
    if( !genParticlesHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No genparticles label available \n";
    genParticles = genParticlesHandle.product();
    for(unsigned int j = 0; j<genParticles->size() ; j++){
      if( TMath::Abs((*genParticles)[j].pdgId())!=13 || (*genParticles)[j].pt()<10) continue;
      genMatchP4s.push_back( (*genParticles)[j].p4() );
    }
  } 
  else edm::LogError("DataNotAvailable")
      << "not a valid matcher";

  for(unsigned int i = 0; i< taus->size(); i++){

    if( ((*taus)[i].leadPFChargedHadrCand()).isNull() ){
      cout << "Null ref to charged" << endl;
      continue;
    }


    bool matched = false;
    for(unsigned int j = 0; j<genMatchP4s.size() ; j++){
      if( !matched && ROOT::Math::VectorUtil::DeltaR( (*taus)[i].p4(), genMatchP4s[j] )<0.15){

	matched = true;
	matchedID_ = 1.0;

	// check if any gsfElectron is matched to the leadPFCahrgHadr by gsfTrack
	// if so, save the tightest cut-based WP, else 1.0
	for(unsigned int k = 0; k<electrons->size() ; k++){
	  if( ROOT::Math::VectorUtil::DeltaR( (*taus)[i].p4(), (*electrons)[k].p4() )<0.15  ){
	    for(int id = 0; id<6; id++){
	      int match = checkVBTFWP(&(*electrons)[k], ids[id]);
	      //cout << match << endl;
	      if( match > 0.5) matchedID_ = 0.01*ids[id];
	    }
	  }
	}
	
	tightestHPSWP_   = 0;
	tightestAntiEWP_ = 0;
	tightestAntiMWP_ = 0;

	if((*taus)[i].tauID("byLooseIsolation")>0.5)  tightestHPSWP_++;
	if((*taus)[i].tauID("byMediumIsolation")>0.5) tightestHPSWP_++;
	if((*taus)[i].tauID("byTightIsolation")>0.5)  tightestHPSWP_++;

	if((*taus)[i].tauID("againstElectronLoose")>0.5)  tightestAntiEWP_++;
	if((*taus)[i].tauID("againstElectronMedium")>0.5) tightestAntiEWP_++;
	if((*taus)[i].tauID("againstElectronTight")>0.5)  tightestAntiEWP_++;

	if((*taus)[i].tauID("againstMuonLoose")>0.5)  tightestAntiMWP_++;
	if((*taus)[i].tauID("againstMuonTight")>0.5)  tightestAntiMWP_++;
	
	/*
	if((*taus)[i].tauID("againstElectronMedium")>0.5){
	  cout << "Electron passes anti-e medium !!!" << endl;
	  cout << (*taus)[i].pt() << " --- eta " << (*taus)[i].eta() << " --- mva " <<
	    (*taus)[i].electronPreIDOutput() << " --- lead pt " << ((*taus)[i].leadPFChargedHadrCand())->pt() << " --- lead cand " << ((*taus)[i].leadPFCand())->pt() << " --- lead charge " <<  ((*taus)[i].leadPFChargedHadrCand())->charge() << " --- lead cand charge " <<  ((*taus)[i].leadPFCand())->charge() <<  endl;
	  for(unsigned int j = 0; j<genParticles->size() ; j++){
	    if( TMath::Abs((*genParticles)[j].pdgId())==11 && ROOT::Math::VectorUtil::DeltaR( (*taus)[i].p4(),(*genParticles)[j].p4() )<0.15 ) cout << "Match with electron with charge " <<  (*genParticles)[j].charge() << endl;
	  }
	}
	*/

	leadPFChargedHadrMva_        =   (*taus)[i].electronPreIDOutput() ;	
	leadPFChargedHadrHcalEnergy_ =  ((*taus)[i].leadPFChargedHadrCand())->hcalEnergy() ;
	leadPFChargedHadrEcalEnergy_ =  ((*taus)[i].leadPFChargedHadrCand())->ecalEnergy() ;
	leadPFChargedHadrTrackPt_    =  -99;
	if( (((*taus)[i].leadPFChargedHadrCand())->trackRef()).isNonnull() ) 
	  leadPFChargedHadrTrackPt_ = ((*taus)[i].leadPFChargedHadrCand())->trackRef()->pt();
	else if( (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() ) 
	  leadPFChargedHadrTrackPt_ = ((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()->pt();
	else 
	  leadPFChargedHadrTrackPt_ = ((*taus)[i].leadPFChargedHadrCand())->pt();
	leadPFChargedHadrTrackP_     =  ((*taus)[i].leadPFChargedHadrCand())->p();
	if( ((*taus)[i].leadPFCand()).isNonnull() ){
	  leadPFCandMva_               =  ((*taus)[i].leadPFCand())->mva_e_pi() ;	
	  leadPFCandHcalEnergy_        =  ((*taus)[i].leadPFCand())->hcalEnergy() ;
	  leadPFCandEcalEnergy_        =  ((*taus)[i].leadPFCand())->ecalEnergy() ;
	  leadPFCandPt_                =  ((*taus)[i].leadPFCand())->pt();
	  leadPFCandP_                 =  ((*taus)[i].leadPFCand())->p();
	}
	else{
	  leadPFCandMva_ =-99;
	  leadPFCandHcalEnergy_ = -99;
	  leadPFCandEcalEnergy_= -99;
	  leadPFCandPt_= -99;
	  leadPFCandP_= -99;
	}
	emFraction_ = (*taus)[i].emFraction();
	hasGsf_ = (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() ?
	  1. : 0.;
	hasKft_ = (((*taus)[i].leadPFChargedHadrCand())->trackRef()).isNonnull() ?
	  1. : 0.;
	//fbrem_ = (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() ?
	//  ( ((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()->p() - 
	//    ((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()->outerP() ) :
	//  ( ((*taus)[i].leadPFChargedHadrCand())->trackRef()->p() - 
	//    ((*taus)[i].leadPFChargedHadrCand())->trackRef()->outerP() ) ;

	pt_  = (*taus)[i].pt();
	eta_ = (*taus)[i].eta();
	visMass_ = (*taus)[i].mass();

	signalPFChargedHadrCands_ = (*taus)[i].signalPFChargedHadrCands().size();
	signalPFGammaCands_       = (*taus)[i].signalPFGammaCands().size();

	tree_->Fill();
      }

    }
  }


}


int TauFakeRateAnalyzer::checkVBTFWP(const pat::Electron* ele, int id){

  int output = -99;

  switch( id ){
  case 95:    
    output = ( ele->userFloat("nHits")<=1 && ele->userFloat("dist")>-999 && ele->userFloat("dcot")>-999 && 
	       ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.8 && ele->userFloat("dEta")<0.007 && ele->userFloat("HoE")<0.15) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.7 && ele->userFloat("dEta")<0.01 && ele->userFloat("HoE")<0.15) ) && 
	       (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  case 90:
    output = ( ele->userFloat("nHits")<=1 && ele->userFloat("dist")>0.02  && ele->userFloat("dcot")>0.02 && 
	       ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.8 && ele->userFloat("dEta")<0.007 && ele->userFloat("HoE")<0.12) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.7 && ele->userFloat("dEta")<0.009 && ele->userFloat("HoE")<0.15) ) && 
	       (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  case 85:
    output = ( ele->userFloat("nHits")<=1 && ele->userFloat("dist")>0.02 && ele->userFloat("dcot")>0.02 && 
	       ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.06 && ele->userFloat("dEta")<0.006 && ele->userFloat("HoE")<0.04) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.04 && ele->userFloat("dEta")<0.007 && ele->userFloat("HoE")<0.15) ) && 
	       (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  case 80:
    output = (ele->userFloat("nHits")==0 && ele->userFloat("dist")>0.02 && ele->userFloat("dcot")>0.02 && 
	      ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.06 && ele->userFloat("dEta")<0.004 && ele->userFloat("HoE")<0.04) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.03 && ele->userFloat("dEta")<0.007 && ele->userFloat("HoE")<0.15) )  
	      && (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  case 70:
    output = (ele->userFloat("nHits")==0 && ele->userFloat("dist")>0.02 && ele->userFloat("dcot")>0.02 && 
	      ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.03 && ele->userFloat("dEta")<0.004 && ele->userFloat("HoE")<0.025) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.02 && ele->userFloat("dEta")<0.005 && ele->userFloat("HoE")<0.15) )  && 
	      (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  case 60:
    output = (ele->userFloat("nHits")==0 && ele->userFloat("dist")>0.02 && ele->userFloat("dcot")>0.02 && 
	      ( (ele->isEB() && ele->userFloat("sihih")<0.01 && ele->userFloat("dPhi")<0.025 && ele->userFloat("dEta")<0.004 && ele->userFloat("HoE")<0.025) || (ele->isEE() && ele->userFloat("sihih")<0.03 && ele->userFloat("dPhi")<0.02 && ele->userFloat("dEta")<0.005 && ele->userFloat("HoE")<0.15) )  && 
	      (ele->pt()>=20 || (ele->pt()<20 && (ele->fbrem()>0.15 || (abs((ele->superClusterPosition()).Eta())<1. && ele->eSuperClusterOverP()>0.95) ) )) ) ? 1 : 0 ;
    break;
  }


  return output;

}


void TauFakeRateAnalyzer::endJob(){
}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(TauFakeRateAnalyzer);


