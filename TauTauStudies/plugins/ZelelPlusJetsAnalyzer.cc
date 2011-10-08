#include "LLRAnalysis/TauTauStudies/interface/ZelelPlusJetsAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "DataFormats/Candidate/interface/CompositeCandidateFwd.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
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

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <vector>
#include <utility>
#include <map>

using namespace std;
using namespace reco;

typedef std::map<double, math::XYZTLorentzVectorD ,ZelelPlusJetsAnalyzer::more>::iterator CImap;

ZelelPlusJetsAnalyzer::ZelelPlusJetsAnalyzer(const edm::ParameterSet & iConfig){

  diElectronTag_ = iConfig.getParameter<edm::InputTag>("diElectrons");
  jetsTag_ = iConfig.getParameter<edm::InputTag>("jets");
  triggerResultsTag_ = iConfig.getParameter<edm::InputTag>("triggerResults"); 
  deltaRLegJet_  =  iConfig.getUntrackedParameter<double>("deltaRLegJet",0.3);
  isMC_ =  iConfig.getParameter<bool>("isMC");
  minCorrPt_  =  iConfig.getUntrackedParameter<double>("minCorrPt",10.);
  minJetID_   =  iConfig.getUntrackedParameter<double>("minJetID",0.5);
  verbose_ =  iConfig.getUntrackedParameter<bool>("verbose",false);
}

void ZelelPlusJetsAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","Z elel plus jets tree");

  jetsBtagHE_  = new std::vector< double >();
  jetsBtagHP_  = new std::vector< double >();

  jetsChEfraction_  = new std::vector< float >();
  jetsChNfraction_  = new std::vector< float >();
  jetMoments_       = new std::vector< float >();

  triggerBits_ = new std::vector< int >();

  fpuweight_ = new PUWeight();

  jetsP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDL1OffsetP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  genJetsIDP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  extraElectrons_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diElectronLegsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  METP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  
  tree_->Branch("jetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsP4_);
  tree_->Branch("jetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDP4_);
  tree_->Branch("jetsIDL1OffsetP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDL1OffsetP4_);
  tree_->Branch("genJetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genJetsIDP4_);

  tree_->Branch("triggerBits","std::vector<int>",&triggerBits_); 

  tree_->Branch("jetsBtagHE","std::vector<double> ",&jetsBtagHE_);
  tree_->Branch("jetsBtagHP","std::vector<double> ",&jetsBtagHP_);
  tree_->Branch("jetMoments","std::vector<float> ",&jetMoments_);

  tree_->Branch("jetsChEfraction","std::vector<float>",&jetsChEfraction_);
  tree_->Branch("jetsChNfraction","std::vector<float>",&jetsChNfraction_);

  tree_->Branch("diElectronLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diElectronLegsP4_);
  tree_->Branch("extraElectrons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&extraElectrons_);
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

  tree_->Branch("run",&run_,"run/F");
  tree_->Branch("event",&event_,"event/F");
  tree_->Branch("lumi",&lumi_,"lumi/F");

  //tree_->Branch("ZdeltaPhi",&ZdeltaPhi_,"ZdeltaPhi/F");
  tree_->Branch("numPV",&numPV_,"numPV/F");
  tree_->Branch("rhoFastJet",&rhoFastJet_,"rhoFastJet/F");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/F");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/I");
}


ZelelPlusJetsAnalyzer::~ZelelPlusJetsAnalyzer(){
  delete jetsP4_; delete jetsIDP4_; delete jetsIDL1OffsetP4_; delete jetsBtagHE_; delete jetsBtagHP_;
  delete diElectronLegsP4_ ; delete genJetsIDP4_;delete extraElectrons_;
  delete METP4_;  delete triggerBits_; 
  delete fpuweight_;  delete jetsChNfraction_; delete jetsChEfraction_;delete jetMoments_;
}

void ZelelPlusJetsAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){


  jetsP4_->clear();
  jetsIDP4_->clear();
  jetsIDL1OffsetP4_->clear();
  jetsBtagHE_->clear();
  jetsBtagHP_->clear();
  diElectronLegsP4_->clear();
  METP4_->clear();
  extraElectrons_->clear();
  triggerBits_->clear();
  jetsBtagHE_->clear();
  jetsBtagHP_->clear();
  jetsChNfraction_->clear();
  jetsChEfraction_->clear();
  jetMoments_->clear();

  
  edm::Handle<CompositeCandidateCollection> diElectronHandle;
  iEvent.getByLabel(diElectronTag_,diElectronHandle);
  if( !diElectronHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No diElectron label available \n";
  const CompositeCandidateCollection* diElectrons = diElectronHandle.product();

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

  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  nPUVertices_= -99;
  nOOTPUVertices_=-99;

  if(isMC_){
    iEvent.getByType(puInfoH);
    if(puInfoH.isValid()){
      for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){
	if(it->getBunchCrossing() ==0) nPUVertices_ = it->getPU_NumInteractions();
	else  nOOTPUVertices_ = it->getPU_NumInteractions();
      }
    }
  }
  mcPUweight_ = fpuweight_->GetWeight(nPUVertices_);

  edm::Handle<double> rhoFastJetHandle;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsCentral","rho", ""), rhoFastJetHandle);
  if( !rhoFastJetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No rho label available \n";
  rhoFastJet_ = (*rhoFastJetHandle);

  std::map< double, const CompositeCandidate*, less<double> > visMassMap;
  const CompositeCandidate *theDiElectron = 0;
  for(unsigned int it = 0; it < diElectrons->size() ; it++){
    visMassMap.insert( make_pair(TMath::Abs(((*diElectrons)[it]).mass()-91.2),
				 &(*diElectrons)[it]) );
  }

  theDiElectron = visMassMap.size()>0 ?  &(*(visMassMap.begin()->second)) : 0;


  for(unsigned int it = 0; it < diElectrons->size() ; it++){

    bool leg1IsThere = false;
    bool leg2IsThere = false;

    for(unsigned int jt = 0; jt < extraElectrons_->size() ; jt++){
      if( Geom::deltaR( (*extraElectrons_)[jt], ((*diElectrons)[it].daughter(0))->p4() )<0.01 ) leg1IsThere = true; 
      if( Geom::deltaR( (*extraElectrons_)[jt], ((*diElectrons)[it].daughter(1))->p4() )<0.01 ) leg2IsThere = true; 
    }

    if( Geom::deltaR( ((*diElectrons)[it].daughter(0))->p4(),(theDiElectron->daughter(0))->p4() )>0.01 &&
	Geom::deltaR( ((*diElectrons)[it].daughter(0))->p4(),(theDiElectron->daughter(1))->p4() )>0.01 && !leg1IsThere
	) extraElectrons_->push_back( ((*diElectrons)[it].daughter(0))->p4() );
    if(	Geom::deltaR( ((*diElectrons)[it].daughter(1))->p4(),(theDiElectron->daughter(0))->p4() )>0.01 &&
	Geom::deltaR( ((*diElectrons)[it].daughter(1))->p4(),(theDiElectron->daughter(1))->p4() )>0.01 && !leg2IsThere
	) extraElectrons_->push_back( ((*diElectrons)[it].daughter(1))->p4() ); 
  }


  if(theDiElectron==0 || theDiElectron->numberOfDaughters()<2){
    cout << " No valid diElectron !!! " << endl;
    return;
  }
  

  Zmass_ = theDiElectron->mass();
  METP4_->push_back((*met)[0].p4());
  sumEt_  = (*met)[0].sumEt();
  MtLeg1_ = TMath::Sqrt( (theDiElectron->daughter(0)->pt() + (*met)[0].pt() )*
			 (theDiElectron->daughter(0)->pt() + (*met)[0].pt() )- 
			 (theDiElectron->daughter(0)->p4() + (*met)[0].p4()).pt()*
			 (theDiElectron->daughter(0)->p4() + (*met)[0].p4()).pt()  );
  MtLeg2_ = TMath::Sqrt( (theDiElectron->daughter(1)->pt() + (*met)[0].pt() )*
			 (theDiElectron->daughter(1)->pt() + (*met)[0].pt() )- 
			 (theDiElectron->daughter(1)->p4() + (*met)[0].p4()).pt()*
			 (theDiElectron->daughter(1)->p4() + (*met)[0].p4()).pt()  );

  const pat::Electron* leg1 = dynamic_cast<const pat::Electron*>( (theDiElectron->daughter(0)->masterClone()).get() );
  const pat::Electron* leg2 = dynamic_cast<const pat::Electron*>( (theDiElectron->daughter(1)->masterClone()).get() );

  isLegFromTau_ = ( (leg1->genParticleById(15,0,true)).isNonnull() ||
		    (leg1->genParticleById(-15,0,true)).isNonnull() ||
		    (leg2->genParticleById(15,0,true)).isNonnull() ||
		    (leg2->genParticleById(-15,0,true)).isNonnull()
		    ) ? 1 : 0;


  dxy1_ = vertexes->size()!=0 ? leg1->gsfTrack()->dxy( (*vertexes)[0].position() ) : -999;
  dxy2_ = vertexes->size()!=0 ? leg2->gsfTrack()->dxy( (*vertexes)[0].position() ) : -999;
  dz1_  = vertexes->size()!=0 ? leg1->gsfTrack()->dz( (*vertexes)[0].position() ) : -999;
  dz2_  = vertexes->size()!=0 ? leg2->gsfTrack()->dz( (*vertexes)[0].position() ) : -999;

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
    triggerPaths.push_back("HLT_Ele17_SW_TightCaloEleId_Ele8HE_L1R_v2");
    triggerPaths.push_back("HLT_DoubleEle17_SW_L1R_v1");
    triggerPaths.push_back("HLT_Ele17_SW_TighterEleIdIsol_L1R_v3");
    triggerPaths.push_back("HLT_Ele17_SW_Isol_L1R_v2");
    triggerPaths.push_back("HLT_Ele17_SW_L1R_v2");
  }
  else{
    triggerPaths.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v1");
    triggerPaths.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v2");
    triggerPaths.push_back("HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v3");
    triggerPaths.push_back("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC8_Mass30_v1");
    triggerPaths.push_back("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC8_Mass30_v2");
    triggerPaths.push_back("HLT_Ele17_CaloIdVT_CaloIsoVT_TrkIdT_TrkIsoVT_SC8_Mass30_v3");    
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
  

  diElectronLegsP4_->push_back(leg1->p4());
  diElectronLegsP4_->push_back(leg2->p4());

  run_   = iEvent.run();
  event_ = (iEvent.eventAuxiliary()).event();
  lumi_ = iEvent.luminosityBlock();

  std::map<double, math::XYZTLorentzVectorD ,ZelelPlusJetsAnalyzer::more> sortedJets;
  std::map<double, math::XYZTLorentzVectorD ,ZelelPlusJetsAnalyzer::more> sortedJetsID;
  std::map<double, math::XYZTLorentzVectorD ,ZelelPlusJetsAnalyzer::more> sortedJetsIDL1Offset;

  std::map<double, math::XYZTLorentzVectorD ,ZelelPlusJetsAnalyzer::more> sortedGenJetsID;
  std::map<double, std::pair<float,float> ,  ZelelPlusJetsAnalyzer::more> bTaggers;
  std::map<double, std::pair<float,float> ,  ZelelPlusJetsAnalyzer::more> jetPVassociation;
  std::map<double, std::pair<float,float> ,  ZelelPlusJetsAnalyzer::more> jetMoments;

  for(unsigned int it = 0; it < jets->size() ; it++){

    if( Geom::deltaR((*jets)[it].p4(),leg1->p4())<deltaRLegJet_ || 
	Geom::deltaR((*jets)[it].p4(),leg2->p4())<deltaRLegJet_ ){
      if(verbose_) cout << "The jet at (" <<(*jets)[it].et()<<","<<(*jets)[it].eta()<<") is closer than "<<deltaRLegJet_ << " from one of the legs" << endl;  
      continue;
    }


    if((*jets)[it].p4().Pt() < minCorrPt_) continue;
   
    sortedJets.insert( make_pair( (*jets)[it].correctedJet("Uncorrected").p4().Pt() ,(*jets)[it].correctedJet("Uncorrected").p4() ) );
                                        
    std::map<string,float> aMap;
    if( jetID( &(*jets)[it] , &((*vertexes)[0]), vtxZ, aMap ) < minJetID_ )  continue;

    //add b-tag info
    bTaggers.insert( make_pair((*jets)[it].p4().Pt(), make_pair( (*jets)[it].bDiscriminator("trackCountingHighEffBJetTags"),(*jets)[it].bDiscriminator("trackCountingHighPurBJetTags")  ) ) );
    jetPVassociation.insert( make_pair( (*jets)[it].p4().Pt(), make_pair(aMap["chFracRawJetE"],aMap["chFracAllChargE"]) ) );
    jetMoments.insert( make_pair( (*jets)[it].p4().Pt(), make_pair( (*jets)[it].etaetaMoment(),(*jets)[it].phiphiMoment()) ) );

    if(isMC_) sortedJetsIDL1Offset.insert( make_pair( (*jets)[it].jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*(*jets)[it].pt() , (*jets)[it].jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*(*jets)[it].p4()) );   
    else sortedJetsIDL1Offset.insert( make_pair( (*jets)[it].jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*(*jets)[it].pt() , (*jets)[it].jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*(*jets)[it].p4()) ); 
  
    sortedJetsID.insert( make_pair( (*jets)[it].p4().Pt() ,(*jets)[it].p4() ) );
  
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


unsigned int ZelelPlusJetsAnalyzer::jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_){
  
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


void ZelelPlusJetsAnalyzer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(ZelelPlusJetsAnalyzer);


