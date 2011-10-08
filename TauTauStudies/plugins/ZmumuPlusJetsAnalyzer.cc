#include "LLRAnalysis/TauTauStudies/interface/ZmumuPlusJetsAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositDirection.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include <vector>
#include <utility>
#include <map>

using namespace std;
using namespace reco;

typedef std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more>::iterator CImap;

ZmumuPlusJetsAnalyzer::ZmumuPlusJetsAnalyzer(const edm::ParameterSet & iConfig){

  diMuonTag_ = iConfig.getParameter<edm::InputTag>("diMuons");
  jetsTag_ = iConfig.getParameter<edm::InputTag>("jets");
  triggerResultsTag_ = iConfig.getParameter<edm::InputTag>("triggerResults"); 
  deltaRLegJet_  =  iConfig.getUntrackedParameter<double>("deltaRLegJet",0.3);
  isMC_ =  iConfig.getParameter<bool>("isMC");
  minCorrPt_  =  iConfig.getUntrackedParameter<double>("minCorrPt",10.);
  minJetID_   =  iConfig.getUntrackedParameter<double>("minJetID",0.5);
  verbose_ =  iConfig.getUntrackedParameter<bool>("verbose",false);
}

void ZmumuPlusJetsAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","Z mumu plus jets tree");

  jetsBtagHE_  = new std::vector< double >();
  jetsBtagHP_  = new std::vector< double >();

  jetsChEfraction_  = new std::vector< float >();
  jetsChNfraction_  = new std::vector< float >();
  jetMoments_       = new std::vector< float >();

  triggerBits_ = new std::vector< int >();

  fpuweight_ = 0;// new PUWeight();

  weights2011_.push_back(0.0809561); 
  weights2011_.push_back(0.483674); 
  weights2011_.push_back(1.04601); 
  weights2011_.push_back(1.6949); 
  weights2011_.push_back(2.1009); 
  weights2011_.push_back(2.24013); 
  weights2011_.push_back(2.15938); 
  weights2011_.push_back(1.73219); 
  weights2011_.push_back(1.32508); 
  weights2011_.push_back(0.950115);
  weights2011_.push_back(0.6599); 
  weights2011_.push_back(0.438093); 
  weights2011_.push_back(0.284885); 
  weights2011_.push_back(0.180615); 
  weights2011_.push_back(0.112314); 
  weights2011_.push_back(0.0687845);
  weights2011_.push_back(0.0415087); 
  weights2011_.push_back(0.0247498); 
  weights2011_.push_back(0.0147688); 
  weights2011_.push_back(0.00865185); 
  weights2011_.push_back(0.00512891); 
  weights2011_.push_back(0.00291561); 
  weights2011_.push_back(0.00169447); 
  weights2011_.push_back(0.000998808);
  weights2011_.push_back(0.00087273); 
  weights2011_.push_back(0); 
  weights2011_.push_back(0); 

  jetsP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDL1OffsetP4_= new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDUpP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDDownP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
 
  genJetsIDP4_  = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  extraMuons_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diMuonLegsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  METP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genVP4_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  tree_->Branch("genDecay",&genDecay_,"genDecay/I");
  
  tree_->Branch("jetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsP4_);
  tree_->Branch("jetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDP4_);
  tree_->Branch("jetsIDUpP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDUpP4_);
  tree_->Branch("jetsIDDownP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDDownP4_);
  tree_->Branch("jetsIDL1OffsetP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDL1OffsetP4_);
   
  tree_->Branch("genJetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genJetsIDP4_);
  tree_->Branch("triggerBits","std::vector<int>",&triggerBits_);
  tree_->Branch("jetsBtagHE","std::vector<double> ",&jetsBtagHE_);
  tree_->Branch("jetsBtagHP","std::vector<double> ",&jetsBtagHP_);
 
  tree_->Branch("jetMoments","std::vector<float> ",&jetMoments_);
  tree_->Branch("jetsChEfraction","std::vector<float>",&jetsChEfraction_);
  tree_->Branch("jetsChNfraction","std::vector<float>",&jetsChNfraction_);

  tree_->Branch("diMuonLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diMuonLegsP4_);
  tree_->Branch("extraMuons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&extraMuons_);
  
  tree_->Branch("METP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&METP4_);
  tree_->Branch("sumEt",&sumEt_,"sumEt/F");
  tree_->Branch("MtLeg1",&MtLeg1_,"MtLeg1/F");
  tree_->Branch("MtLeg2",&MtLeg2_,"MtLeg2/F");
  tree_->Branch("Zmass",&Zmass_,"Zmass/F");
  tree_->Branch("isLegFromTau",&isLegFromTau_,"isLegFromTau/I");

  tree_->Branch("chIsoLeg1v1",&chIsoLeg1v1_,"chIsoLeg1v1/F");
  tree_->Branch("nhIsoLeg1v1",&nhIsoLeg1v1_,"nhIsoLeg1v1/F");
  tree_->Branch("phIsoLeg1v1",&phIsoLeg1v1_,"phIsoLeg1v1/F");
  tree_->Branch("chIsoPULeg1v1",&chIsoPULeg1v1_,"chIsoPULeg1v1/F");
  tree_->Branch("nhIsoPULeg1v1",&nhIsoPULeg1v1_,"nhIsoPULeg1v1/F");
  tree_->Branch("phIsoPULeg1v1",&phIsoPULeg1v1_,"phIsoPULeg1v1/F");

  tree_->Branch("chIsoLeg1v2",&chIsoLeg1v2_,"chIsoLeg1v2/F");
  tree_->Branch("nhIsoLeg1v2",&nhIsoLeg1v2_,"nhIsoLeg1v2/F");
  tree_->Branch("phIsoLeg1v2",&phIsoLeg1v2_,"phIsoLeg1v2/F");
  tree_->Branch("chIsoPULeg1v2",&chIsoPULeg1v2_,"chIsoPULeg1v2/F");
  tree_->Branch("nhIsoPULeg1v2",&nhIsoPULeg1v2_,"nhIsoPULeg1v2/F");
  tree_->Branch("phIsoPULeg1v2",&phIsoPULeg1v2_,"phIsoPULeg1v2/F");

  tree_->Branch("chIsoLeg2v1",&chIsoLeg2v1_,"chIsoLeg2v1/F");
  tree_->Branch("nhIsoLeg2v1",&nhIsoLeg2v1_,"nhIsoLeg2v1/F");
  tree_->Branch("phIsoLeg2v1",&phIsoLeg2v1_,"phIsoLeg2v1/F");
  tree_->Branch("chIsoPULeg2v1",&chIsoPULeg2v1_,"chIsoPULeg2v1/F");
  tree_->Branch("nhIsoPULeg2v1",&nhIsoPULeg2v1_,"nhIsoPULeg2v1/F");
  tree_->Branch("phIsoPULeg2v1",&phIsoPULeg2v1_,"phIsoPULeg2v1/F");

  tree_->Branch("chIsoLeg2v2",&chIsoLeg2v2_,"chIsoLeg2v2/F");
  tree_->Branch("nhIsoLeg2v2",&nhIsoLeg2v2_,"nhIsoLeg2v2/F");
  tree_->Branch("phIsoLeg2v2",&phIsoLeg2v2_,"phIsoLeg2v2/F");
  tree_->Branch("chIsoPULeg2v2",&chIsoPULeg2v2_,"chIsoPULeg2v2/F");
  tree_->Branch("nhIsoPULeg2v2",&nhIsoPULeg2v2_,"nhIsoPULeg2v2/F");
  tree_->Branch("phIsoPULeg2v2",&phIsoPULeg2v2_,"phIsoPULeg2v2/F");

  tree_->Branch("dxy1",&dxy1_,"dxy1/F");
  tree_->Branch("dxy2",&dxy2_,"dxy2/F");

  tree_->Branch("dz1",&dz1_,"dz1/F");
  tree_->Branch("dz2",&dz2_,"dz2/F");

  tree_->Branch("run",&run_,"run/l");
  tree_->Branch("event",&event_,"event/l");
  tree_->Branch("lumi",&lumi_,"lumi/l");

  tree_->Branch("numPV",&numPV_,"numPV/F");
  tree_->Branch("rhoFastJet",&rhoFastJet_,"rhoFastJet/F");
  tree_->Branch("rhoNeutralFastJet",&rhoNeutralFastJet_,"rhoNeutralFastJet/F");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/F");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/I");
}


ZmumuPlusJetsAnalyzer::~ZmumuPlusJetsAnalyzer(){
  delete jetsP4_; delete jetsIDP4_; delete jetsIDUpP4_ ; delete jetsIDDownP4_ ; delete jetsIDL1OffsetP4_; delete jetsBtagHE_; delete jetsBtagHP_;
  delete diMuonLegsP4_ ; delete genJetsIDP4_;delete extraMuons_;
  delete METP4_;  delete triggerBits_; 
  /*delete fpuweight_;*/  
  delete jetsChNfraction_; delete jetsChEfraction_;delete jetMoments_;
}

void ZmumuPlusJetsAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){


  jetsP4_->clear();
  jetsIDP4_->clear();
  jetsIDUpP4_->clear();
  jetsIDDownP4_->clear();
  jetsIDL1OffsetP4_->clear();
  jetsBtagHE_->clear();
  jetsBtagHP_->clear();
  diMuonLegsP4_->clear();
  METP4_->clear();
  extraMuons_->clear();
  triggerBits_->clear();
  jetsBtagHE_->clear();
  jetsBtagHP_->clear();
  jetsChNfraction_->clear();
  jetsChEfraction_->clear();
  jetMoments_->clear();

  
  edm::Handle<CompositeCandidateCollection> diMuonHandle;
  iEvent.getByLabel(diMuonTag_,diMuonHandle);
  if( !diMuonHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No diMuon label available \n";
  const CompositeCandidateCollection* diMuons = diMuonHandle.product();

  edm::Handle<pat::JetCollection> jetsHandle;
  iEvent.getByLabel(jetsTag_,jetsHandle);
  if( !jetsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No jets label available \n";
  const pat::JetCollection* jets = jetsHandle.product();

  edm::Handle<reco::VertexCollection> pvHandle;
  edm::InputTag pvTag("offlinePrimaryVertices");
  iEvent.getByLabel(pvTag,pvHandle);
  if( !pvHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No PV label available \n";
  const reco::VertexCollection* vertexes = pvHandle.product();

  std::vector<float> vtxZ;
  for(unsigned int k = 0; k<vertexes->size(); k++){
    vtxZ.push_back(((*vertexes)[k].position()).z());
  }
  
  numPV_ = vertexes->size();

  edm::Handle<pat::METCollection> metHandle;
  iEvent.getByLabel(edm::InputTag("patMETsPFlow"),metHandle);
  if( !metHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No MET label available \n";
  const pat::METCollection* met = metHandle.product();

  edm::Handle<pat::TriggerEvent> triggerHandle;
  iEvent.getByLabel(triggerResultsTag_, triggerHandle);
  if( !triggerHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger label available \n";
  const pat::TriggerEvent* trigger = triggerHandle.product();

  genDecay_ = -99;
  edm::Handle<reco::GenParticleCollection> genHandle;
  if(isMC_){
    iEvent.getByLabel(edm::InputTag("genParticles"),genHandle);
    if( !genHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen particles label available \n";
    const reco::GenParticleCollection* genParticles = genHandle.product();
    for(unsigned int k = 0; k < genParticles->size(); k ++){
      if( !( (*genParticles)[k].pdgId() == 23 || abs((*genParticles)[k].pdgId()) == 24 || (*genParticles)[k].pdgId() == 25) || (*genParticles)[k].status()!=3)
	continue;
      if(verbose_) cout << "Boson status, pt,phi " << (*genParticles)[k].status() << "," << (*genParticles)[k].pt() << "," << (*genParticles)[k].phi() << endl;
      
      genVP4_->push_back( (*genParticles)[k].p4() );
      genDecay_ = (*genParticles)[k].pdgId();
      
      int breakLoop = 0;
      for(unsigned j = 0; j< ((*genParticles)[k].daughterRefVector()).size() && breakLoop!=1; j++){
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 11 ){
	  genDecay_ *= 11;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 13 ){
	  genDecay_ *= 13;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 15 ){
	  genDecay_ *= 15;
	  breakLoop = 1;
	}
      }
      if(verbose_) cout << "Decays to pdgId " << genDecay_/(*genParticles)[k].pdgId()  << endl;
      break;
    }
  }

  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  nPUVertices_= -99;
  nOOTPUVertices_=-99;
  float sum_nvtx = 0;
  if(isMC_){
    // PU infos
    iEvent.getByType(puInfoH);
    if(puInfoH.isValid()){
      for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){
	//cout << "Bunc crossing " << it->getBunchCrossing() << endl;
	if(it->getBunchCrossing() ==0) nPUVertices_ = it->getPU_NumInteractions();
	else  nOOTPUVertices_ = it->getPU_NumInteractions();
	sum_nvtx += float(it->getPU_NumInteractions());
      }
    }
  }
  if(verbose_){
    cout << "Average num of int = " << sum_nvtx/3. << endl;
    cout << "Num of PU = " << nPUVertices_ << endl;
    cout << "Num of OOT PU = " << nOOTPUVertices_ << endl;
  }
  if( fpuweight_ ) mcPUweight_ = fpuweight_->GetWeight(nPUVertices_);
  else mcPUweight_ =  int(nPUVertices_+0.5) < int(weights2011_.size()) ? weights2011_[int(nPUVertices_+0.5)] : weights2011_[(unsigned int)(weights2011_.size()-1)];

  edm::Handle<double> rhoFastJetHandle;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsCentral","rho", ""), rhoFastJetHandle);
  if( !rhoFastJetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No rho label available \n";
  rhoFastJet_ = (*rhoFastJetHandle);

  edm::Handle<double> rhoNeutralFastJetHandle;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsNeutral","rho", ""), rhoNeutralFastJetHandle);
  if( !rhoNeutralFastJetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No rho neutral label available \n";
  rhoNeutralFastJet_ = (*rhoNeutralFastJetHandle);

  std::map< double, const CompositeCandidate*, less<double> > visMassMap;
  const CompositeCandidate *theDiMuon = 0;
  for(unsigned int it = 0; it < diMuons->size() ; it++){
    visMassMap.insert( make_pair(TMath::Abs(((*diMuons)[it]).mass()-91.2),
				 &(*diMuons)[it]) );
  }

  theDiMuon = visMassMap.size()>0 ?  &(*(visMassMap.begin()->second)) : 0;


  for(unsigned int it = 0; it < diMuons->size() ; it++){

    bool leg1IsThere = false;
    bool leg2IsThere = false;

    for(unsigned int jt = 0; jt < extraMuons_->size() ; jt++){
      if( Geom::deltaR( (*extraMuons_)[jt], ((*diMuons)[it].daughter(0))->p4() )<0.01 ) leg1IsThere = true; 
      if( Geom::deltaR( (*extraMuons_)[jt], ((*diMuons)[it].daughter(1))->p4() )<0.01 ) leg2IsThere = true; 
    }

    if( Geom::deltaR( ((*diMuons)[it].daughter(0))->p4(),(theDiMuon->daughter(0))->p4() )>0.01 &&
	Geom::deltaR( ((*diMuons)[it].daughter(0))->p4(),(theDiMuon->daughter(1))->p4() )>0.01 && !leg1IsThere
	) extraMuons_->push_back( ((*diMuons)[it].daughter(0))->p4() );
    if(	Geom::deltaR( ((*diMuons)[it].daughter(1))->p4(),(theDiMuon->daughter(0))->p4() )>0.01 &&
	Geom::deltaR( ((*diMuons)[it].daughter(1))->p4(),(theDiMuon->daughter(1))->p4() )>0.01 && !leg2IsThere
	) extraMuons_->push_back( ((*diMuons)[it].daughter(1))->p4() ); 
  }


  if(theDiMuon==0 || theDiMuon->numberOfDaughters()<2){
    cout << " No valid diMuon !!! " << endl;
    return;
  }
  
  Zmass_ = theDiMuon->mass();
  METP4_->push_back((*met)[0].p4());
  sumEt_  = (*met)[0].sumEt();
  MtLeg1_ = TMath::Sqrt( (theDiMuon->daughter(0)->pt() + (*met)[0].pt() )*
			 (theDiMuon->daughter(0)->pt() + (*met)[0].pt() )- 
			 (theDiMuon->daughter(0)->p4() + (*met)[0].p4()).pt()*
			 (theDiMuon->daughter(0)->p4() + (*met)[0].p4()).pt()  );
  MtLeg2_ = TMath::Sqrt( (theDiMuon->daughter(1)->pt() + (*met)[0].pt() )*
			 (theDiMuon->daughter(1)->pt() + (*met)[0].pt() )- 
			 (theDiMuon->daughter(1)->p4() + (*met)[0].p4()).pt()*
			 (theDiMuon->daughter(1)->p4() + (*met)[0].p4()).pt()  );

  const pat::Muon* leg1 = dynamic_cast<const pat::Muon*>( (theDiMuon->daughter(0)->masterClone()).get() );
  const pat::Muon* leg2 = dynamic_cast<const pat::Muon*>( (theDiMuon->daughter(1)->masterClone()).get() );
  
  
  isLegFromTau_ = ( (leg1->genParticleById(15,0,true)).isNonnull() ||
		    (leg1->genParticleById(-15,0,true)).isNonnull() ||
		    (leg2->genParticleById(15,0,true)).isNonnull() ||
		    (leg2->genParticleById(-15,0,true)).isNonnull()
		    ) ? 1 : 0;
  
  dxy1_ = vertexes->size()!=0 ? leg1->globalTrack()->dxy( (*vertexes)[0].position() ) : -999;
  dxy2_ = vertexes->size()!=0 ? leg2->globalTrack()->dxy( (*vertexes)[0].position() ) : -999;
  dz1_  = vertexes->size()!=0 ? leg1->globalTrack()->dz( (*vertexes)[0].position() ) : -999;
  dz2_  = vertexes->size()!=0 ? leg2->globalTrack()->dz( (*vertexes)[0].position() ) : -999;


  // isoDeposit definition: 2010
  isodeposit::AbsVetos vetos2010ChargedLeg1; 
  isodeposit::AbsVetos vetos2010NeutralLeg1; 
  isodeposit::AbsVetos vetos2010PhotonLeg1;
  // isoDeposit definition: 2011
  isodeposit::AbsVetos vetos2011ChargedLeg1; 
  isodeposit::AbsVetos vetos2011NeutralLeg1; 
  isodeposit::AbsVetos vetos2011PhotonLeg1;
 
  vetos2010ChargedLeg1.push_back(new isodeposit::ThresholdVeto(0.5));
  vetos2010NeutralLeg1.push_back(new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.08));
  vetos2010NeutralLeg1.push_back(new isodeposit::ThresholdVeto(1.0));
  vetos2010PhotonLeg1.push_back( new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.05));
  vetos2010PhotonLeg1.push_back( new isodeposit::ThresholdVeto(1.0));

  vetos2011ChargedLeg1.push_back(new isodeposit::ThresholdVeto(0.0));
  vetos2011NeutralLeg1.push_back(new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.01));
  vetos2011NeutralLeg1.push_back(new isodeposit::ThresholdVeto(0.5));
  vetos2011PhotonLeg1.push_back( new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.01));
  vetos2011PhotonLeg1.push_back( new isodeposit::ThresholdVeto(0.5));

  chIsoLeg1v1_   = 
    leg1->isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4,vetos2010ChargedLeg1).first;
  nhIsoLeg1v1_ = 
    leg1->isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4,vetos2010NeutralLeg1).first;
  phIsoLeg1v1_ = 
    leg1->isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4,vetos2010PhotonLeg1).first;
  chIsoPULeg1v1_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010ChargedLeg1).first;
  nhIsoPULeg1v1_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010NeutralLeg1).first;
  phIsoPULeg1v1_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010PhotonLeg1).first;

  chIsoLeg1v2_   = 
    leg1->isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg1).first;
  nhIsoLeg1v2_ = 
    leg1->isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg1).first;
  phIsoLeg1v2_ = 
    leg1->isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg1).first;
  chIsoPULeg1v2_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg1).first;
  nhIsoPULeg1v2_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg1).first;
  phIsoPULeg1v2_ = 
    leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg1).first;

  // cleaning
  for(unsigned int i = 0; i <vetos2010ChargedLeg1.size(); i++){
    delete vetos2010ChargedLeg1[i];
  }
  for(unsigned int i = 0; i <vetos2010NeutralLeg1.size(); i++){
    delete vetos2010NeutralLeg1[i];
    delete vetos2010PhotonLeg1[i];
  }
  for(unsigned int i = 0; i <vetos2011ChargedLeg1.size(); i++){
    delete vetos2011ChargedLeg1[i];
  }
  for(unsigned int i = 0; i <vetos2011NeutralLeg1.size(); i++){
    delete vetos2011NeutralLeg1[i];
    delete vetos2011PhotonLeg1[i];
  }
  //
 

  // isoDeposit definition: 2010
  isodeposit::AbsVetos vetos2010ChargedLeg2; 
  isodeposit::AbsVetos vetos2010NeutralLeg2; 
  isodeposit::AbsVetos vetos2010PhotonLeg2;
  // isoDeposit definition: 2011
  isodeposit::AbsVetos vetos2011ChargedLeg2; 
  isodeposit::AbsVetos vetos2011NeutralLeg2; 
  isodeposit::AbsVetos vetos2011PhotonLeg2;
 
  vetos2010ChargedLeg2.push_back(new isodeposit::ThresholdVeto(0.5));
  vetos2010NeutralLeg2.push_back(new isodeposit::ConeVeto(isodeposit::Direction(leg2->eta(),leg2->phi()),0.08));
  vetos2010NeutralLeg2.push_back(new isodeposit::ThresholdVeto(1.0));
  vetos2010PhotonLeg2.push_back( new isodeposit::ConeVeto(isodeposit::Direction(leg2->eta(),leg2->phi()),0.05));
  vetos2010PhotonLeg2.push_back( new isodeposit::ThresholdVeto(1.0));

  vetos2011ChargedLeg2.push_back(new isodeposit::ThresholdVeto(0.0));
  vetos2011NeutralLeg2.push_back(new isodeposit::ConeVeto(isodeposit::Direction(leg2->eta(),leg2->phi()),0.01));
  vetos2011NeutralLeg2.push_back(new isodeposit::ThresholdVeto(0.5));
  vetos2011PhotonLeg2.push_back( new isodeposit::ConeVeto(isodeposit::Direction(leg2->eta(),leg2->phi()),0.01));
  vetos2011PhotonLeg2.push_back( new isodeposit::ThresholdVeto(0.5));

  chIsoLeg2v1_   = 
    leg2->isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4,vetos2010ChargedLeg2).first;
  nhIsoLeg2v1_ = 
    leg2->isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4,vetos2010NeutralLeg2).first;
  phIsoLeg2v1_ = 
    leg2->isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4,vetos2010PhotonLeg2).first;
  chIsoPULeg2v1_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010ChargedLeg2).first;
  nhIsoPULeg2v1_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010NeutralLeg2).first;
  phIsoPULeg2v1_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2010PhotonLeg2).first;

  chIsoLeg2v2_   = 
    leg2->isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg2).first;
  nhIsoLeg2v2_ = 
    leg2->isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg2).first;
  phIsoLeg2v2_ = 
    leg2->isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg2).first;
  chIsoPULeg2v2_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg2).first;
  nhIsoPULeg2v2_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg2).first;
  phIsoPULeg2v2_ = 
    leg2->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg2).first;

  // cleaning
  for(unsigned int i = 0; i <vetos2010ChargedLeg2.size(); i++){
    delete vetos2010ChargedLeg2[i];
  }
  for(unsigned int i = 0; i <vetos2010NeutralLeg2.size(); i++){
    delete vetos2010NeutralLeg2[i];
    delete vetos2010PhotonLeg2[i];
  }
  for(unsigned int i = 0; i <vetos2011ChargedLeg2.size(); i++){
    delete vetos2011ChargedLeg2[i];
  }
  for(unsigned int i = 0; i <vetos2011NeutralLeg2.size(); i++){
    delete vetos2011NeutralLeg2[i];
    delete vetos2011PhotonLeg2[i];
  }
  //
 

  vector<string> triggerPaths;
  if(isMC_){
    triggerPaths.push_back("HLT_IsoMu17_v5");
    triggerPaths.push_back("HLT_IsoMu17_v6");
    triggerPaths.push_back("HLT_IsoMu24_v1");
  }
  else{
    triggerPaths.push_back("HLT_IsoMu24_v1");
    triggerPaths.push_back("HLT_IsoMu24_v2");
    triggerPaths.push_back("HLT_IsoMu24_v3");
    triggerPaths.push_back("HLT_IsoMu24_v4");
    triggerPaths.push_back("HLT_IsoMu24_v5");
    triggerPaths.push_back("HLT_IsoMu24_v6");
    triggerPaths.push_back("HLT_IsoMu24_v7");
    triggerPaths.push_back("HLT_IsoMu24_v8");
    triggerPaths.push_back("HLT_Mu17_Mu8_v1");
    triggerPaths.push_back("HLT_Mu17_Mu8_v2");
    triggerPaths.push_back("HLT_Mu17_Mu8_v3");
    triggerPaths.push_back("HLT_Mu17_Mu8_v4");
    triggerPaths.push_back("HLT_Mu17_Mu8_v5");
    triggerPaths.push_back("HLT_Mu17_Mu8_v6");
    triggerPaths.push_back("HLT_Mu17_Mu8_v7");
    triggerPaths.push_back("HLT_Mu17_Mu8_v8");
  }

  for(unsigned int i=0;i<triggerPaths.size();i++){
    if(!trigger){
      continue;
      cout << "Invalid triggerEvent !" << endl;
    }
    const pat::TriggerPath *triggerPath =  trigger->path(triggerPaths[i]);
    
    if(verbose_){
      cout<<  "Testing " << triggerPaths[i] << endl;
      if(triggerPath) cout << "Is there..." << endl;
      if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
      if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
    }

    if(triggerPath && triggerPath->wasRun() && 
       triggerPath->wasAccept() && 
       triggerPath->prescale()==1) triggerBits_->push_back(1);
    else if (triggerPath && triggerPath->wasRun() && 
	     triggerPath->wasAccept() && 
	     triggerPath->prescale()!=1) triggerBits_->push_back(2);
    else triggerBits_->push_back(0);
  }
  

  diMuonLegsP4_->push_back(leg1->p4());
  diMuonLegsP4_->push_back(leg2->p4());

  run_   = iEvent.run();
  event_ = (iEvent.eventAuxiliary()).event();
  lumi_ = iEvent.luminosityBlock();

  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedJets;
  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedJetsID;
  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedJetsIDL1Offset;
  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedJetsIDUp;
  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedJetsIDDown;
  std::map<double, math::XYZTLorentzVectorD ,ZmumuPlusJetsAnalyzer::more> sortedGenJetsID;
  std::map<double, std::pair<float,float> ,  ZmumuPlusJetsAnalyzer::more> bTaggers;
  std::map<double, std::pair<float,float> ,  ZmumuPlusJetsAnalyzer::more> jetPVassociation;
  std::map<double, std::pair<float,float> ,  ZmumuPlusJetsAnalyzer::more> jetMoments;

  for(unsigned int it = 0; it < jets->size() ; it++){

    if( Geom::deltaR((*jets)[it].p4(),leg1->p4())<deltaRLegJet_ || 
	Geom::deltaR((*jets)[it].p4(),leg2->p4())<deltaRLegJet_ ){
      if(verbose_) cout << "The jet at (" <<(*jets)[it].pt()<<","<<(*jets)[it].eta()<<") is closer than "<<deltaRLegJet_ << " from one of the legs" << endl;  
      continue;
    }

    if((*jets)[it].p4().Pt() < minCorrPt_) continue;
    sortedJets.insert( make_pair( (*jets)[it].correctedJet("Uncorrected").p4().Pt() ,(*jets)[it].correctedJet("Uncorrected").p4() ) );

    /////////////////////////////////////////////////////////////////////////
    //// try to use JES uncertainties
    edm::ESHandle<JetCorrectorParametersCollection> jetCorrParameters;
    // get the jet corrector parameters collection from the global tag
    iSetup.get<JetCorrectionsRecord>().get("AK5PF", jetCorrParameters);
    // get the uncertainty parameters from the collection
    JetCorrectorParameters const & param = (*jetCorrParameters)["Uncertainty"];
    // instantiate the jec uncertainty object
    JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(param);
    deltaJEC->setJetEta((*jets)[it].eta()); deltaJEC->setJetPt((*jets)[it].pt());
    float shift  = deltaJEC->getUncertainty( true );
    /////////////////////////////////////////////////////////////////////////
                                        
    std::map<string,float> aMap;
    if( jetID( &(*jets)[it] , &((*vertexes)[0]), vtxZ, aMap ) < minJetID_ )  continue;

    //add b-tag info
    bTaggers.insert( make_pair((*jets)[it].p4().Pt(), make_pair( (*jets)[it].bDiscriminator("trackCountingHighEffBJetTags"),(*jets)[it].bDiscriminator("trackCountingHighPurBJetTags")  ) ) );
    jetPVassociation.insert( make_pair( (*jets)[it].p4().Pt(), make_pair(aMap["chFracRawJetE"],aMap["chFracAllChargE"]) ) );
    jetMoments.insert( make_pair( (*jets)[it].p4().Pt(), make_pair( (*jets)[it].etaetaMoment(),(*jets)[it].phiphiMoment()) ) );
  
    if(isMC_) sortedJetsIDL1Offset.insert( make_pair( (*jets)[it].jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*(*jets)[it].pt() , (*jets)[it].jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*(*jets)[it].p4()) );   
    else sortedJetsIDL1Offset.insert( make_pair( (*jets)[it].jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*(*jets)[it].pt() , (*jets)[it].jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*(*jets)[it].p4()) ); 
  

    sortedJetsID.insert( make_pair( (*jets)[it].p4().Pt() ,(*jets)[it].p4() ) );
    sortedJetsIDUp.insert( make_pair( (*jets)[it].p4().Pt() ,  (*jets)[it].p4()*(1+shift) ) );
    sortedJetsIDDown.insert( make_pair( (*jets)[it].p4().Pt() ,(*jets)[it].p4()*(1-shift) ) );

    if(isMC_){
      if((*jets)[it].genJet() != 0) sortedGenJetsID.insert( make_pair( (*jets)[it].p4().Pt() ,(*jets)[it].genJet()->p4() ) );
      else sortedGenJetsID.insert( make_pair( (*jets)[it].p4().Pt() , math::XYZTLorentzVectorD(0,0,0,0) ) );
    }
  }
  
  for(CImap it = sortedJets.begin(); it != sortedJets.end() ; it++){
    jetsP4_->push_back( it->second );
  }
  for(CImap it = sortedJetsID.begin(); it != sortedJetsID.end() ; it++){
    jetsIDP4_->push_back( it->second );
  }
  for(CImap it = sortedJetsIDUp.begin(); it != sortedJetsIDUp.end() ; it++){
    jetsIDUpP4_->push_back( it->second );
  }
  for(CImap it = sortedJetsIDDown.begin(); it != sortedJetsIDDown.end() ; it++){
    jetsIDDownP4_->push_back( it->second );
  }
  for(CImap it = sortedJetsIDL1Offset.begin(); it != sortedJetsIDL1Offset.end() ; it++){
    jetsIDL1OffsetP4_->push_back( it->second );
  }
  for(CImap it = sortedGenJetsID.begin(); it != sortedGenJetsID.end() ; it++){
    genJetsIDP4_->push_back( it->second );
  }
  for(std::map<double, std::pair<float,float> >::iterator it = bTaggers.begin(); it != bTaggers.end() ; it++){
    jetsBtagHE_->push_back( (it->second).first  );
    jetsBtagHP_->push_back( (it->second).second );
  }
  for(std::map<double, std::pair<float,float> >::iterator it = jetPVassociation.begin(); it != jetPVassociation.end() ; it++){
    jetsChEfraction_->push_back( (it->second).first  );
    jetsChNfraction_->push_back( (it->second).second );
  }
  for(std::map<double, std::pair<float,float> >::iterator it = jetMoments.begin(); it != jetMoments.end() ; it++){
    jetMoments_->push_back( (it->second).first  );
    jetMoments_->push_back( (it->second).second );
  }

  tree_->Fill();

}


unsigned int  ZmumuPlusJetsAnalyzer::jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_){
  
  if( (jet->pt())<10 ) return 99; // always pass jet ID

  std::vector<reco::PFCandidatePtr> pfCandPtrs = jet->getPFConstituents();

  int nCharged = 0;
  int nPhotons = 0;
  int nNeutral = 0;
  int nConst = 0;

  float energyCharged = 0;
  float energyPhotons = 0;
  float energyNeutral = 0;
  float energyElectrons = 0;
 
  float totalEnergyFromConst = 0;

  float chEnFractionPV   = 0;
  float chEnFractionChPV = 0;
  float chFractionPV     = 0;

  for(unsigned i=0; i<pfCandPtrs.size(); ++i) {
    const reco::PFCandidate& cand = *(pfCandPtrs[i]);

    totalEnergyFromConst +=  cand.energy();
    nConst += 1;

    switch( cand.particleId() ) {
    case reco::PFCandidate::h: 
      nCharged++;
      energyCharged += cand.energy(); 

      if((cand.trackRef()).isNonnull()){
	bool isMatched = false;
	for(reco::Vertex::trackRef_iterator it = vtx->tracks_begin() ; it!=vtx->tracks_end() && !isMatched; it++){
	  if(  (*it).id() == (cand.trackRef()).id() && (*it).key() == (cand.trackRef()).key() ){
	    isMatched = true;
	    if(verbose_) cout << (*it).id() << ", " << (*it).key() << " is matched!" << endl;
	    chEnFractionPV += cand.energy();
	    chFractionPV+=1;
	  }
	}
	if(!isMatched){
	  float dist = vtxZ.size()>0 ? abs(vtxZ[0]-((cand.trackRef())->referencePoint()).z()) : 999.;
	  float tmpDist = 999.;
	  for(unsigned k = 1; k < vtxZ.size(); k++){
	    if( abs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	      tmpDist = abs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] );
	  }
	  if(tmpDist>dist){
	    isMatched = true;
	    if(verbose_) cout << "Matched by closest vtx in z!!" << endl;
	    chEnFractionPV += cand.energy();
	    chFractionPV+=1;
	  }
	}
	if(!isMatched && verbose_) {
	  cout << "Ch with pt " << cand.pt() << " and eta " << cand.eta() << " is not matched to the PV !!!" << endl;
	  cout << "z position of PV " << (vtx->position()).z() << ", z position of track " << ((cand.trackRef())->referencePoint()).z()  << ", x position of track " << ((cand.trackRef())->referencePoint()).x()   << ", y position of track " << ((cand.trackRef())->referencePoint()).y() << endl;
	}
      }

      break;
    case reco::PFCandidate::gamma:
      nPhotons++;
      energyPhotons += cand.energy();
      break;
    case reco::PFCandidate::h0:
      nNeutral++;
      energyNeutral += cand.energy();
      break;
    case reco::PFCandidate::e: 
      energyElectrons += cand.energy();

      if((cand.gsfTrackRef()).isNonnull()){
	bool isMatched = false;
	float dist = vtxZ.size()>0 ? abs(vtxZ[0]-((cand.gsfTrackRef())->referencePoint()).z()) : 999.;
	float tmpDist = 999.;
	for(unsigned k = 1; k < vtxZ.size(); k++){
	  if( abs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	    tmpDist = abs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] );
	}
	if(tmpDist>dist){
	  isMatched = true;
	  if(verbose_) cout << "Matched by closest vtx in z!!" << endl;
	  chEnFractionPV += cand.energy();
	  chFractionPV+=1;
	}
	if(!isMatched && verbose_) {
	  cout << "Ele with pt " << cand.pt() << " and eta " << cand.eta() << " is not matched to the PV !!!" << endl;
	  cout << "z position of PV " << (vtx->position()).z() << ", z position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).z()  << ", x position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).x()   << ", y position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).y() << endl;
	}
      }
      
      break;
    case reco::PFCandidate::h_HF: // fill neutral
      nNeutral++;
      energyNeutral += cand.energy();
      break;
    case reco::PFCandidate::egamma_HF: // fill e/gamma
      nPhotons++;
      energyPhotons += cand.energy();
      break;
    default:
      break;
    }
  }

  chEnFractionChPV = chEnFractionPV;
  if(energyCharged>0 || energyElectrons>0)
    chEnFractionChPV /= (energyCharged+energyElectrons); 
  chEnFractionPV /= jet->correctedJet("Uncorrected").p4().E();
  chFractionPV   /= nCharged;

  //if(chFractionPV==0) cout << energyCharged << endl;

  map_["chMult"]          =  chFractionPV;
  map_["chFracRawJetE"]   =  chEnFractionPV;
  map_["chFracAllChargE"] =  chEnFractionChPV;

  bool loose=false;
  bool medium=false;
  bool tight=false;

  //loose id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.99 && 
       energyPhotons/totalEnergyFromConst<0.99 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<0.99 && 
       energyPhotons/totalEnergyFromConst<0.99 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<0.99
       )
      ) loose = true;
  // medium id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.95 && 
       energyPhotons/totalEnergyFromConst<0.95 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<1 && 
       energyPhotons/totalEnergyFromConst<1 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<1
       )
      ) medium = true;
  // tight id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.90 && 
       energyPhotons/totalEnergyFromConst<0.90 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<1 && 
       energyPhotons/totalEnergyFromConst<1 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<1
       )
      ) tight = true;
  
  if(loose && !medium && !tight) return 1;
  if(loose && medium && !tight)  return 2;
  if(loose && medium && tight)   return 3; 
  
  return 0;

}




void ZmumuPlusJetsAnalyzer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(ZmumuPlusJetsAnalyzer);


