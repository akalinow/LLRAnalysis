#include "LLRAnalysis/TauTauStudies/interface/VbfJetAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include <DataFormats/Candidate/interface/Candidate.h>

#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "DataFormats/JetReco/interface/Jet.h"


#include <vector>
#include <utility>
#include <map>

using namespace std;
using namespace reco;

typedef std::map<double, math::XYZTLorentzVectorD ,VbfJetAnalyzer::more>::iterator CImap;

VbfJetAnalyzer::VbfJetAnalyzer(const edm::ParameterSet & iConfig){
  verbose_ =  iConfig.getUntrackedParameter<bool>("verbose",false);
}

void VbfJetAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","vbf jets tree");

  jetsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  tagjetsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  tree_->Branch("jetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsP4_);
  tree_->Branch("tagjetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&tagjetsP4_);
  tree_->Branch("event",&event_,"event/F");
  tree_->Branch("run",&run_,"run/F");
 
}


VbfJetAnalyzer::~VbfJetAnalyzer(){
  delete jetsP4_;
  delete tagjetsP4_;
}

void VbfJetAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){

  std::map<double, math::XYZTLorentzVectorD ,VbfJetAnalyzer::more> sortedJets;
  std::map<double, math::XYZTLorentzVectorD ,VbfJetAnalyzer::more> sortedTagJets;

  jetsP4_->clear();
  tagjetsP4_->clear();

  const reco::PFJetCollection* pfJets = 0;
  edm::Handle<reco::PFJetCollection> pfJetHandle;
  iEvent.getByLabel(edm::InputTag("ak5PFJets"),pfJetHandle);
  pfJets = pfJetHandle.product();

  const reco::GenJetCollection* pfGenJets = 0;
  edm::Handle<reco::GenJetCollection> genJetHandle;
  iEvent.getByLabel(edm::InputTag("ak5GenJets"),genJetHandle);
  pfGenJets = genJetHandle.product();

  const reco::GenParticleCollection* genParticles = 0;
  edm::Handle<reco::GenParticleCollection> genHandle;
  iEvent.getByLabel(edm::InputTag("genParticles"),genHandle);
  genParticles = genHandle.product();

 if( !pfJetHandle.isValid() || !genHandle.isValid() || !genJetHandle.isValid())  
    edm::LogError("DataNotAvailable")
      << "No jets/gen particles available \n";

 
 vector<const reco::Candidate*> tagParticles;

 if(verbose_) cout<< "***************** Event " << iEvent.id() << endl;

 for(reco::GenParticleCollection::const_iterator ci = genParticles->begin(); ci!=genParticles->end(); ci++){

   if( !(ci->pdgId() == 25 &&  ci->status()==3) ) continue;

   // selected the Higgs at the production

   if((ci->motherRefVector()).size()!=2 ){
     if(verbose_) cout << "h0 does not have exactly 2 mothers" << endl;
     continue;
   }

   if(verbose_){
     cout << "h0  with pdgid " << ci->pdgId() << " has mothers:  " << endl;
     for(unsigned int i = 0 ; i < ci->numberOfMothers(); i ++){
       cout << ci->mother(i)->pdgId() << " --> pt=" <<ci->mother(i)->pt() << endl;
     }
   }

   // get the two mothers of the H
   const reco::Candidate* mH1=ci->mother(0);
   //const reco::Candidate* mH2=ci->mother(1);
   for(unsigned int i = 0 ; i < mH1->numberOfDaughters(); i ++){
     tagParticles.push_back(mH1->daughter(i));
   }


   if(verbose_){
     cout << "Mother 1 with pdgid " << mH1->pdgId() << " has dausghters:  " << endl;
     for(unsigned int i = 0 ; i < mH1->numberOfDaughters(); i ++){
       cout << mH1->daughter(i)->pdgId() << " --> pt=" << mH1->daughter(i)->pt() << endl;
     }
   }

   break;
   
 }

 const JetCorrector* corrector0 = JetCorrector::getJetCorrector("ak5PFL2L3", iSetup);
 const JetCorrector* corrector1 = JetCorrector::getJetCorrector("ak5PFL2L3Residual", iSetup);
 const JetCorrector* corrector2 = JetCorrector::getJetCorrector("ak5PFL1FastL2L3", iSetup);
 const JetCorrector* corrector3 = JetCorrector::getJetCorrector("ak5PFL1FastL2L3Residual", iSetup);


 for(reco::PFJetCollection::const_iterator ci = pfJets->begin(); ci!=pfJets->end(); ci++){

   int index = ci - pfJets->begin();
   edm::RefToBase<reco::Jet> jetRef(edm::Ref< PFJetCollection >(pfJets,index));

   double scale0 =  corrector0->correction( *ci,jetRef,iEvent,iSetup ) ;
   double scale1 =  corrector1->correction( *ci,jetRef,iEvent,iSetup ) ;
   double scale2 =  corrector2->correction( *ci,jetRef,iEvent,iSetup ) ;
   double scale3 =  corrector3->correction( *ci,jetRef,iEvent,iSetup ) ;
   double scale=scale2;

   if(verbose_){
     cout << "ak5PFL2L3 ==> " << scale1 << endl; 
     cout << "ak5PFL2L3Residual ==> " << scale2 << endl; 
     cout << "ak5PFL1FastL2L3 ==> " << scale0 << endl; 
     cout << "ak5PFL1FastL2L3Residual ==> " << scale3 << endl; 
   }
   //double scale =  corrector->correction( ci->p4() ) ;

   if( jetID(&(*ci), scale )<0.5 ){
     if(verbose_) cout << "jet does not pass loose id" << endl;
     continue;
   }
   bool isMatchedToTau = false;
   for(reco::GenParticleCollection::const_iterator cj = genParticles->begin(); cj!=genParticles->end(); cj++){
     if( abs(cj->pdgId())==15 && cj->status()==2 &&  Geom::deltaR(ci->p4(),cj->p4())<0.3 ){
       isMatchedToTau=true;
       if(verbose_) cout << "Matched to tau" << endl;
       break;
     }
   }
   if(isMatchedToTau) continue; 

   sortedJets.insert( make_pair(ci->pt()*scale,ci->p4()*scale) );
 
   int counter = 0;
   for(unsigned j = 1 ; j <tagParticles.size(); j++){
     if( (tagParticles[j]!=0 &&  Geom::deltaR(tagParticles[j]->p4(), ci->p4()) < 0.5)  ){
       if(verbose_) cout << "tag particle with pdgId " << tagParticles[j]->pdgId() 
			 << " and pT,eta " << tagParticles[j]->pt() 
			 << ", " << tagParticles[j]->eta() <<  "  is matched to a reco jet pT,eta " 
			 << ci->pt()  << ", " << ci->eta()<< endl;
       counter++;
     }
   }
   if(counter>0)  sortedTagJets.insert( make_pair(ci->pt()*scale,ci->p4()*scale) );
 }

 for(CImap it = sortedJets.begin(); it != sortedJets.end() ; it++){
   jetsP4_->push_back( it->second );
 }
 for(CImap it = sortedTagJets.begin(); it != sortedTagJets.end() ; it++){
    tagjetsP4_->push_back( it->second );
  }

 run_   = iEvent.run();
 event_ = (iEvent.eventAuxiliary()).event();
 
 tree_->Fill();
 
}


unsigned int VbfJetAnalyzer::jetID( const reco::PFJet* jet, const double scale){

  if( (jet->pt()*scale)<10 ) return 99; // always pass jet ID

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

  for(unsigned i=0; i<pfCandPtrs.size(); ++i) {
    const reco::PFCandidate& cand = *(pfCandPtrs[i]);

    totalEnergyFromConst +=  cand.energy();
    nConst += 1;

    switch( cand.particleId() ) {
    case reco::PFCandidate::h: 
      nCharged++;
      energyCharged += cand.energy(); 
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



void VbfJetAnalyzer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(VbfJetAnalyzer);


