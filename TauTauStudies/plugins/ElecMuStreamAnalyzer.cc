#include "LLRAnalysis/TauTauStudies/interface/ElecMuStreamAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "TauAnalysis/CandidateTools/interface/CompositePtrCandidateT1T2MEtProducer.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEtFwd.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositDirection.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>

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

typedef std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more>::iterator CImap;

ElecMuStreamAnalyzer::ElecMuStreamAnalyzer(const edm::ParameterSet & iConfig){

  diTauTag_          = iConfig.getParameter<edm::InputTag>("diTaus");
  jetsTag_           = iConfig.getParameter<edm::InputTag>("jets");
  newJetsTag_        = iConfig.getParameter<edm::InputTag>("newJets");
  metTag_            = iConfig.getParameter<edm::InputTag>("met");
  rawMetTag_         = iConfig.getParameter<edm::InputTag>("rawMet");
  muonsTag_          = iConfig.getParameter<edm::InputTag>("muons");
  muonsRelTag_       = iConfig.getParameter<edm::InputTag>("muonsRel");
  electronsTag_      = iConfig.getParameter<edm::InputTag>("electrons");
  electronsRelTag_   = iConfig.getParameter<edm::InputTag>("electronsRel");
  verticesTag_       = iConfig.getParameter<edm::InputTag>("vertices");
  triggerResultsTag_ = iConfig.getParameter<edm::InputTag>("triggerResults"); 
  isMC_              = iConfig.getParameter<bool>("isMC");
  deltaRLegJet_      = iConfig.getUntrackedParameter<double>("deltaRLegJet",0.3);
  minCorrPt_         = iConfig.getUntrackedParameter<double>("minCorrPt",10.);
  minJetID_          = iConfig.getUntrackedParameter<double>("minJetID",0.5);
  verbose_           = iConfig.getUntrackedParameter<bool>("verbose",false);

}

void ElecMuStreamAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","qqH tree");

  tRandom_ = new TRandom3();
 
  jetsBtagHE_  = new std::vector< double >();
  jetsBtagHP_  = new std::vector< double >();

  jetsChEfraction_  = new std::vector< float >();
  jetsChNfraction_  = new std::vector< float >();
  jetMoments_       = new std::vector< float >();

  tauXTriggers_= new std::vector< int >();
  triggerBits_ = new std::vector< int >();

  jetsP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDUpP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDDownP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDL1OffsetP4_= new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genJetsIDP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauVisP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauCAP4_       = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauICAP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauSVfitP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauLegsP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genDiTauLegsP4_  = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  METP4_           = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genMETP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genVP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  extraMuons_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  extraElectrons_  = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  fpuweight_ = 0;//new PUWeight();

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
  

  tree_->Branch("jetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsP4_);
  tree_->Branch("jetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDP4_);
  tree_->Branch("jetsIDUpP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDUpP4_);
  tree_->Branch("jetsIDDownP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDDownP4_);
  tree_->Branch("jetsIDL1OffsetP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDL1OffsetP4_);
  tree_->Branch("genJetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genJetsIDP4_);
  
  tree_->Branch("jetsBtagHE","std::vector<double>",&jetsBtagHE_);
  tree_->Branch("jetsBtagHP","std::vector<double>",&jetsBtagHP_);

  tree_->Branch("jetMoments","std::vector<float> ",&jetMoments_);

  tree_->Branch("jetsChEfraction","std::vector<float>",&jetsChEfraction_);
  tree_->Branch("jetsChNfraction","std::vector<float>",&jetsChNfraction_);

  tree_->Branch("tauXTriggers","std::vector<int>",&tauXTriggers_);
  tree_->Branch("triggerBits","std::vector<int>",&triggerBits_);

  tree_->Branch("diTauVisP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",  &diTauVisP4_);
  tree_->Branch("diTauCAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauCAP4_);
  tree_->Branch("diTauICAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauICAP4_);
  tree_->Branch("diTauSVfitP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauSVfitP4_);

  tree_->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,"diTauNSVfitMass/F");
  tree_->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,"diTauNSVfitMassErrUp/F");
  tree_->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");

  tree_->Branch("diTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauLegsP4_);
  tree_->Branch("genDiTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genDiTauLegsP4_);

  tree_->Branch("METP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&METP4_);
  tree_->Branch("genMETP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genMETP4_);
  tree_->Branch("genVP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genVP4_);
  tree_->Branch("genDecay",&genDecay_,"genDecay/I");

  tree_->Branch("extraMuons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&extraMuons_);
  tree_->Branch("extraElectrons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&extraElectrons_);


  tree_->Branch("sumEt",&sumEt_,"sumEt/F");
  tree_->Branch("mTauTauMin",&mTauTauMin_,"mTauTauMin/F");
  tree_->Branch("MtLeg1",&MtLeg1_,"MtLeg1/F");
  tree_->Branch("MtLeg1",&MtLeg1_,"MtLeg2/F");

  tree_->Branch("pZeta",&pZeta_,"pZeta/F");
  tree_->Branch("pZetaVis",&pZetaVis_,"pZetaVis/F");
  tree_->Branch("pZetaSig",&pZetaSig_,"pZetaSig/F");

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
  tree_->Branch("numOfDiTaus",&numOfDiTaus_,"numOfDiTaus/I");

  tree_->Branch("isElecLegMatched",&isElecLegMatched_,"isTauLegMatched/I");
  tree_->Branch("isMuLegMatched",&isMuLegMatched_,"isMuLegMatched/I");

  tree_->Branch("muFlag",&muFlag_,"muFlag/I");
  tree_->Branch("elecFlag",&elecFlag_,"elecFlag/I");

  tree_->Branch("diTauCharge",&diTauCharge_,"diTauCharge/F");
  tree_->Branch("rhoFastJet",&rhoFastJet_,"rhoFastJet/F");
  tree_->Branch("rhoNeutralFastJet",&rhoNeutralFastJet_,"rhoNeutralFastJet/F");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/F");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/I");


}


ElecMuStreamAnalyzer::~ElecMuStreamAnalyzer(){
  delete jetsP4_; delete jetsIDP4_; delete jetsIDUpP4_; delete jetsIDDownP4_;   delete jetsIDL1OffsetP4_;
  delete METP4_; delete diTauVisP4_; delete diTauCAP4_; delete diTauICAP4_; 
  delete diTauSVfitP4_; delete genVP4_;
  delete diTauLegsP4_; delete jetsBtagHE_; delete jetsBtagHP_; delete tauXTriggers_; delete triggerBits_;
  delete genJetsIDP4_; delete genDiTauLegsP4_; delete genMETP4_; delete extraMuons_; delete extraElectrons_; 
  delete tRandom_ ; delete jetsChNfraction_; delete jetsChEfraction_;delete jetMoments_;
}

void ElecMuStreamAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){



  jetsP4_->clear();
  jetsIDP4_->clear();
  jetsIDUpP4_->clear();
  jetsIDDownP4_->clear();
  jetsIDL1OffsetP4_->clear();
  diTauVisP4_->clear();
  diTauCAP4_->clear();
  diTauICAP4_->clear();
  diTauSVfitP4_->clear();
  diTauLegsP4_->clear();
  METP4_->clear();
  genVP4_->clear();
  jetsChNfraction_->clear();
  jetsChEfraction_->clear();
  jetMoments_->clear();
  
  genJetsIDP4_->clear();
  genDiTauLegsP4_->clear();
  genMETP4_->clear();
  extraMuons_->clear();
  extraElectrons_->clear();
  jetsBtagHE_->clear();
  jetsBtagHP_->clear();
  tauXTriggers_->clear();
  triggerBits_->clear();
  
  edm::Handle<PATElecMuPairCollection> diTauHandle;
  iEvent.getByLabel(diTauTag_,diTauHandle);
  if( !diTauHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No diTau label available \n";
  const PATElecMuPairCollection* diTaus = diTauHandle.product();

  edm::Handle<pat::JetCollection> jetsHandle;
  iEvent.getByLabel(jetsTag_,jetsHandle);
  if( !jetsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No jets label available \n";
  const pat::JetCollection* jets = jetsHandle.product();

  edm::Handle<pat::JetCollection> newJetsHandle;
  iEvent.getByLabel(newJetsTag_,newJetsHandle);
  if( !newJetsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No newJets label available \n";
  const pat::JetCollection* newJets = newJetsHandle.product();

  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByLabel( verticesTag_ ,pvHandle);
  if( !pvHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No PV label available \n";
  const reco::VertexCollection* vertexes = pvHandle.product();

  std::vector<float> vtxZ;
  for(unsigned int k = 0; k<vertexes->size(); k++){
    vtxZ.push_back(((*vertexes)[k].position()).z());
  }


  if(verbose_){
    cout <<  "Run " << iEvent.run() << ", event " << (iEvent.eventAuxiliary()).event() 
	 << ", lumi " << iEvent.luminosityBlock() << endl;
    cout << "List of vertexes " << endl;
    for(unsigned int k = 0; k<vertexes->size(); k++){
      cout << "Vtx[" << k << "] (x,y,z) = (" << ((*vertexes)[k].position()).x()
	   << "," << ((*vertexes)[k].position()).y() << "," << ((*vertexes)[k].position()).z() << ")"
	   << endl;
    }
  }

  numPV_ = vertexes->size();

  edm::Handle<pat::METCollection> metHandle;
  iEvent.getByLabel( metTag_ ,metHandle);
  if( !metHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No MET label available \n";
  const pat::METCollection* met = metHandle.product();

  edm::Handle<pat::METCollection> rawMetHandle;
  iEvent.getByLabel( rawMetTag_, rawMetHandle);
  if( !rawMetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No raw MET label available \n";
  const pat::METCollection* rawMet = rawMetHandle.product();

  edm::Handle<pat::TriggerEvent> triggerHandle;
  iEvent.getByLabel(triggerResultsTag_, triggerHandle);
  if( !triggerHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger label available \n";
  const pat::TriggerEvent* trigger = triggerHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();


  genDecay_ = -99;
  edm::Handle<reco::GenParticleCollection> genHandle;
  const reco::GenParticleCollection* genParticles = 0;
  if(isMC_){
    iEvent.getByLabel(edm::InputTag("genParticles"),genHandle);
    if( !genHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen particles label available \n";
    genParticles = genHandle.product();
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
  //else mcPUweight_ =  int(sum_nvtx/3.+0.5) < int(weights2011_.size()) ? weights2011_[int(sum_nvtx/3.+0.5)] : weights2011_[(unsigned int)(weights2011_.size()-1)];
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
  
  edm::Handle<pat::ElectronCollection> electronsHandle;
  iEvent.getByLabel(electronsTag_,electronsHandle);
  if( !electronsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No electrons label available \n";
  const pat::ElectronCollection* electrons = electronsHandle.product();
  if(electrons->size()<1){
    cout << " No electrons !!! " << endl;
    return;
  } else if(electrons->size()>1 && verbose_){
    cout << "WARNING: "<< electrons->size() << "  electrons found in the event !!! We will select only one" << endl;
  }
  edm::Handle<pat::ElectronCollection> electronsRelHandle;
  iEvent.getByLabel(electronsRelTag_,electronsRelHandle);
  if( !electronsRelHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No electronsRel label available \n";
  const pat::ElectronCollection* electronsRel = electronsRelHandle.product();
  if(electronsRel->size()<1){
    cout << " No electronsRel !!! " << endl;
    return;
  } else if(electronsRel->size()>1 && verbose_){
    cout << "WARNING: "<< electronsRel->size() << "  electronsRel found in the event !!!" << endl;
  }

  edm::Handle<pat::MuonCollection> muonsHandle;
  iEvent.getByLabel(muonsTag_,muonsHandle);
  if( !muonsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muons label available \n";
  const pat::MuonCollection* muons = muonsHandle.product();
  if(muons->size()<1){
    cout << " No muons !!! " << endl;
    return;
  } else if(muons->size()>1 && verbose_){
    cout << "WARNING: "<< muons->size() << "  muons found in the event !!! We will select only one" << endl;
  }
  edm::Handle<pat::MuonCollection> muonsRelHandle;
  iEvent.getByLabel(muonsRelTag_,muonsRelHandle);
  if( !muonsRelHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muonsRel label available \n";
  const pat::MuonCollection* muonsRel = muonsRelHandle.product();
  if(muonsRel->size()<1){
    cout << " No muonsRel !!! " << endl;
    return;
  } else if(muonsRel->size()>1 && verbose_){
    cout << "WARNING: "<< muonsRel->size() << "  muonsRel found in the event !!!" << endl;
  }

  const PATElecMuPair *theDiTau = 0;
  if(diTaus->size()<1){
    cout << " No diTau !!! " << endl;
    return;
  } else if(diTaus->size()>1 && verbose_){
    cout << "WARNING: "<< diTaus->size() << "  diTaus found in the event !!! We will select only one" << endl;
  }
  numOfDiTaus_ = diTaus->size();


  unsigned int muIndex = 0;
  muFlag_=-99;

  if(  muons->size()==1 &&  muonsRel->size()==1 ){
    muFlag_ = 0;
    if(verbose_) cout<< "Just one selected muon: flag= " << muFlag_ << endl;
  }
  else if(  muonsRel->size()>1 ){
    muFlag_ = 3;
    bool found = false;
    for(unsigned int i=0; i<muonsRel->size(); i++){
      for(unsigned int j=0; j<muons->size(); j++){

	if(found) continue;

	if( Geom::deltaR( (*muonsRel)[i].p4(),(*muons)[j].p4())>0.3 
	    && (*muonsRel)[i].charge()*(*muons)[j].charge()<0
	    && (*muons)[j].userFloat("PFRelIsoDB04v2")<0.3 && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	  muFlag_ = 1;
	  if(verbose_) cout<< "Two muons failing diMu veto: flag= " << muFlag_ << endl;
	  found=true; 
	}
	else if( Geom::deltaR( (*muonsRel)[i].p4(),(*muons)[j].p4())>0.3 
		 && (*muonsRel)[i].charge()*(*muons)[j].charge()>0
		 && (*muons)[j].userFloat("PFRelIsoDB04v2")<0.3 && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	  muFlag_ = 2;
	  if(verbose_) cout<< "Two muons with SS: flag= " << muFlag_ << endl;
	  found=true;  
	}
      }
    }
  } 
  else{
    muFlag_ = -1;
  }

  // select highest-pt electron that forms at least one di-tau 
  bool found2 = false;
  for(unsigned int j=0; j<muons->size(); j++){
    for(unsigned int i=0; i< diTaus->size(); i++){

      if(found2) continue;

      if( Geom::deltaR(((*diTaus)[i].leg2())->p4(),((*muons)[j]).p4())<0.01 ){
	muIndex=j;
	found2 = true;
      }
    }
  }
  if(verbose_) cout<< "Selected muon with index " << muIndex << endl;

  for(unsigned int i=0; i<muonsRel->size(); i++){
    if( Geom::deltaR( (*muonsRel)[i].p4(),(*muons)[muIndex].p4())>0.3) 
      extraMuons_->push_back( (*muonsRel)[i].p4() );
  }

  std::vector<unsigned int> selectedDiTausFromMu;
  for(unsigned int i=0; i< diTaus->size(); i++){
    if( Geom::deltaR(((*diTaus)[i].leg2())->p4(),((*muons)[muIndex]).p4())<0.01 ) 
      selectedDiTausFromMu.push_back(i);
  }
  if(verbose_) cout<< "Selection on Muons has selected " << selectedDiTausFromMu.size() << " diTaus...check tau leg now..." << endl;
    
  unsigned int elecIndex = 0;
  elecFlag_=-99;

  if(  electrons->size()==1 &&  electronsRel->size()==1 ){
    elecFlag_ = 0;
    if(verbose_) cout<< "Just one selected electron: flag= " << elecFlag_ << endl;
  }
  else if(  electronsRel->size()>1 ){
    elecFlag_ = 3;
    bool found = false;
    for(unsigned int i=0; i<electronsRel->size(); i++){
      for(unsigned int j=0; j<electrons->size(); j++){

	if(found) continue;

	if( Geom::deltaR( (*electronsRel)[i].p4(),(*electrons)[j].p4())>0.3 
	    && (*electronsRel)[i].charge()*(*electrons)[j].charge()<0
	    && (*electrons)[j].userFloat("PFRelIsoDB04v2")<0.3 && (*electronsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	  elecFlag_ = 1;
	  if(verbose_) cout<< "Two electron failing diElec veto: flag= " << elecFlag_ << endl;
	  found=true; 
	}
	else if( Geom::deltaR( (*electronsRel)[i].p4(),(*electrons)[j].p4())>0.3 
		 && (*electronsRel)[i].charge()*(*electrons)[j].charge()>0
		 && (*electrons)[j].userFloat("PFRelIsoDB04v2")<0.3 && (*electronsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	  elecFlag_ = 2;
	  if(verbose_) cout<< "Two electron with SS: flag= " << elecFlag_ << endl;
	  found=true;  
	}
      }
    }
  } 
  else{
    elecFlag_ = -1;
  }

  // select highest-pt electron that forms at least one di-tau 
  bool found3 = false;
  for(unsigned int j=0; j<electrons->size(); j++){
    for(unsigned int i=0; i< diTaus->size(); i++){

      if(found3) continue;

      if( Geom::deltaR(((*diTaus)[i].leg1())->p4(),((*electrons)[j]).p4())<0.01 ){
	elecIndex=j;
	found3 = true;
      }
    }
  }
  if(verbose_) cout<< "Selected electron with index " << elecIndex << endl;

  for(unsigned int i=0; i<electronsRel->size(); i++){
    if( Geom::deltaR( (*electronsRel)[i].p4(),(*electrons)[elecIndex].p4())>0.3) 
      extraElectrons_->push_back( (*electronsRel)[i].p4() );
  }

  std::vector<unsigned int> selectedDiTausFromElec;
  for(unsigned int i=0; i< diTaus->size(); i++){
    if( Geom::deltaR(((*diTaus)[i].leg1())->p4(),((*electrons)[elecIndex]).p4())<0.01 ) 
      selectedDiTausFromElec.push_back(i);
  }
  if(verbose_) cout<< "Selection on Electrons has selected " << selectedDiTausFromElec.size() << " diTaus...check tau leg now..." << endl;
    

  unsigned int index = 0;

  for(unsigned int i = 0; i < selectedDiTausFromMu.size() ; i++){
    for(unsigned int j = 0; j < selectedDiTausFromElec.size() ; j++){
      if(selectedDiTausFromMu[i]==selectedDiTausFromElec[j]){
	index = selectedDiTausFromMu[i];
	if(verbose_) cout << "DiTau with index " << index << " contains both selected leptons" << endl;
	break;
      }
    }
  }

  theDiTau = &(*diTaus)[index];

  
  diTauCharge_ = theDiTau->charge();
  METP4_->push_back((*rawMet)[0].p4());
  METP4_->push_back((*met)[0].p4());
  if(isMC_) genMETP4_->push_back( (*rawMet)[0].genMET()->p4() );
  sumEt_  = (*met)[0].sumEt();
  MtLeg1_ =  theDiTau->mt1MET();
  MtLeg2_ =  theDiTau->mt2MET();
  pZeta_     =  theDiTau->pZeta();
  pZetaVis_  =  theDiTau->pZetaVis();
  pZetaSig_  =  theDiTau->pZetaSig();
  mTauTauMin_=  theDiTau->mTauTauMin();

  isMuLegMatched_   = 0;
  isElecLegMatched_ = 0;

  const pat::Electron* leg1 = dynamic_cast<const pat::Electron*>(  (theDiTau->leg1()).get() );
  const pat::Muon*     leg2 = dynamic_cast<const pat::Muon*>( (theDiTau->leg2()).get() );

  vector<string> triggerPaths;
  vector<string> HLTfiltersMu;
  vector<string> HLTfiltersElec;

  if(isMC_){

    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v2");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v2");

    HLTfiltersMu.push_back("hltSingleMu8EG5L3Filtered8");
    HLTfiltersMu.push_back("hltL3Muon17");

    HLTfiltersElec.push_back("hltL1NonIsoHLTNonIsoMu8Ele17PixelMatchFilter");
    HLTfiltersElec.push_back("hltL1NonIsoHLTNonIsoMu17Ele8PixelMatchFilter");
  }
  else{

    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v1");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v2");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v3");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v4");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v5");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v6");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v7");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v8");
    triggerPaths.push_back("HLT_Mu8_Ele17_CaloIdL_v9");

    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v1");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v2");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v3");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v4");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v5");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v6");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v7");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v8");
    triggerPaths.push_back("HLT_Mu17_Ele8_CaloIdL_v9");
    
    HLTfiltersMu.push_back("hltSingleMu8EG5L3Filtered8");
    HLTfiltersMu.push_back("hltL3Muon17");

    HLTfiltersElec.push_back("hltL1NonIsoHLTNonIsoMu8Ele17PixelMatchFilter");
    HLTfiltersElec.push_back("hltL1NonIsoHLTNonIsoMu17Ele8PixelMatchFilter");

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

   for(unsigned int i=0 ; i< HLTfiltersMu.size() ; i++){
    bool matched = false;
    for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
      pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));
      if(verbose_) {
	if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.3 ){
	  for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
	    cout << "Object passing " << (aObj->filterLabels())[k] << " within 0.3 of muon" << endl;
	  }
	}
      }
      if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.3  && aObj->hasFilterLabel(HLTfiltersMu[i]) ){
	matched = true;
      }
    }
    if(matched) 
      tauXTriggers_->push_back(1);
    else 
      tauXTriggers_->push_back(0);
    if(verbose_){
      if(matched) cout << "Muon matched within dR=0.3 with trigger object passing filter " << HLTfiltersMu[i] << endl;
      else cout << "!!! Muon is not trigger matched within dR=0.3 !!!" << endl;
    }
  }
  for(unsigned int i=0 ; i< HLTfiltersElec.size() ; i++){
    bool matched = false;
    for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
      pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));
      if(verbose_) {
        if( Geom::deltaR( aObj->triggerObject().p4(), leg1->p4() )<0.3 ){
          for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
            cout << "Object passing " << (aObj->filterLabels())[k] << " within 0.3 of electron" << endl;
          }
        }
      }
      if( Geom::deltaR( aObj->triggerObject().p4(), leg1->p4() )<0.3  && aObj->hasFilterLabel(HLTfiltersElec[i]) ){
	matched = true;
      }
    }
    if(matched) 
      tauXTriggers_->push_back(1);
    else 
      tauXTriggers_->push_back(0);
    if(verbose_){
      if(matched) cout << "Electron matched within dR=0.3 with trigger object passing filter " << HLTfiltersElec[i] << endl;
      else cout << "!!! Electron is not trigger matched within dR=0.3 !!!" << endl;
    }
  }


  // triggers Elec
  if(verbose_){
    const pat::TriggerObjectStandAloneCollection trColl = leg1->triggerObjectMatchesByType(trigger::TriggerElectron);
    cout << "Mu triggers" << endl;
    for(pat::TriggerObjectStandAloneCollection::const_iterator it = trColl.begin(); it != trColl.end(); it++){
      for(unsigned int k = 0; k < (it->pathNames(false,false)).size(); k++){
	cout << (it->pathNames(false,false))[k] << " with filters: " << endl;
	for(unsigned int m = 0; m < (it->filterLabels()).size(); m++){
	  cout << "  - " << (it->filterLabels())[m] << endl;
	}
      }
    }
  }
  // triggers Mu
  if(verbose_){
    const pat::TriggerObjectStandAloneCollection trColl = leg2->triggerObjectMatchesByType(trigger::TriggerMuon);
    cout << "Electron triggers" << endl;
    for(pat::TriggerObjectStandAloneCollection::const_iterator it = trColl.begin(); it != trColl.end(); it++){
      for(unsigned int k = 0; k < (it->pathNames(false,false)).size(); k++){
	cout << (it->pathNames(false,false))[k]  << " with filters: " << endl;
	for(unsigned int m = 0; m < (it->filterLabels()).size(); m++){
	  cout << "  - " << (it->filterLabels())[m] << endl;
	}
      }
    }
  }

  diTauLegsP4_->push_back(leg1->p4());
  diTauLegsP4_->push_back(leg2->p4());
  
  if(isMC_){
    if( (leg1->genParticleById(11,0,true)).isNonnull()  ){
      genDiTauLegsP4_->push_back( leg1->genParticleById(11,0,true)->p4() );
      isElecLegMatched_ = 1;
    }
    else if( (leg1->genParticleById(-11,0,true)).isNonnull()  ){
      genDiTauLegsP4_->push_back( leg1->genParticleById(-11,0,true)->p4() );
      isElecLegMatched_ = 2;
    }
    else{
      genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
      for(unsigned int k = 0; k < genParticles->size(); k ++){
	if( (*genParticles)[k].status()==3 && abs((*genParticles)[k].pdgId())==15 )
	  isElecLegMatched_ = 3;
	if(  verbose_ && (*genParticles)[k].status()==1 && Geom::deltaR((*genParticles)[k].p4() ,leg1->p4())<0.15 )
	  cout << "Elec leg matchged to particle " << (*genParticles)[k].pdgId() 
	       << " with pt " << (*genParticles)[k].pt()
	       << endl;
      }
    }
    
    if( (leg2->genParticleById(13,0,true)).isNonnull()  ){
      genDiTauLegsP4_->push_back( leg2->genParticleById(13,0,true)->p4() );
      isMuLegMatched_ = 1;
    }
    else if( (leg2->genParticleById(-13,0,true)).isNonnull()  ){
      genDiTauLegsP4_->push_back( leg2->genParticleById(-13,0,true)->p4() );
      isMuLegMatched_ = 2;
    }
    else{
      genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
      for(unsigned int k = 0; k < genParticles->size(); k ++){
	if( (*genParticles)[k].status()==3 && abs((*genParticles)[k].pdgId())==15 )
	  isMuLegMatched_ = 3;
	if( verbose_ && (*genParticles)[k].status()==1 && Geom::deltaR((*genParticles)[k].p4() ,leg2->p4())<0.15 )
	  cout << "Mu leg matchged to particle " << (*genParticles)[k].pdgId() 
	       << " with pt " << (*genParticles)[k].pt()
	       << endl;
      }
    }

  }
  else{
    genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
    genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
  }



  
  dxy1_ = vertexes->size()!=0 ? leg1->gsfTrack()->dxy( (*vertexes)[0].position() ) : -99;
  dz1_  = vertexes->size()!=0 ? leg1->gsfTrack()->dz( (*vertexes)[0].position() ) : -99;
  dxy2_ = vertexes->size()!=0 ? leg2->globalTrack()->dxy( (*vertexes)[0].position() ) : -99;
  dz2_  = vertexes->size()!=0 ? leg2->globalTrack()->dz( (*vertexes)[0].position() ) : -99;
 


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




  diTauVisP4_->push_back( theDiTau->p4Vis() );
  diTauCAP4_->push_back(  theDiTau->p4CollinearApprox() );
  diTauICAP4_->push_back( theDiTau->p4ImprovedCollinearApprox() );

  if(verbose_){
    if(theDiTau->collinearApproxIsValid() && theDiTau->ImprovedCollinearApproxIsValid())
      cout << "Found CA and ICA valid solutions" << endl;
    else if(!theDiTau->collinearApproxIsValid() && theDiTau->ImprovedCollinearApproxIsValid())
      cout << "ICA rescued the event!!!" << endl;
    else if(theDiTau->collinearApproxIsValid() && !theDiTau->ImprovedCollinearApproxIsValid())
      cout << "Strange!!! x1=" << theDiTau->x1CollinearApprox() << ", x2=" << theDiTau->x2CollinearApprox() << endl;
    if((theDiTau->p4CollinearApprox()).M()>0 && (theDiTau->p4ImprovedCollinearApprox()).M()<=0)
      cout << "Watch out! ICA gave mass=0 and CA mass !=0 ..." << endl;
  }

  math::XYZTLorentzVectorD nSVfitFitP4(0,0,0,(theDiTau->p4Vis()).M() );
  if( theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)!=0 /* &&theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->isValidSolution()*/ ){
    if(verbose_) cout << "Visible mass ==> " << nSVfitFitP4.E() << endl;
    nSVfitFitP4.SetPxPyPzE( 0,0,0, theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->mass() ) ;
    if(verbose_) cout << "SVFit fit solution ==> " << nSVfitFitP4.E() << endl;
  }
  diTauSVfitP4_->push_back( nSVfitFitP4  );

  diTauNSVfitMass_        = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->mass()        : -99; 
  diTauNSVfitMassErrUp_   = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->massErrUp()   : -99; 
  diTauNSVfitMassErrDown_ = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->massErrDown() : -99; 

  run_   = iEvent.run();
  event_ = (iEvent.eventAuxiliary()).event();
  lumi_  = iEvent.luminosityBlock();

  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedJets;
  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedJetsIDL1Offset;
  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedJetsID;
  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedJetsIDUp;
  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedJetsIDDown;
  std::map<double, math::XYZTLorentzVectorD ,ElecMuStreamAnalyzer::more> sortedGenJetsID;
  std::map<double, std::pair<float,float> ,  ElecMuStreamAnalyzer::more> bTaggers;
  std::map<double, std::pair<float,float> ,  ElecMuStreamAnalyzer::more> jetPVassociation;
  std::map<double, std::pair<float,float> ,  ElecMuStreamAnalyzer::more> jetMoments;
  

  for(unsigned int it = 0; it < jets->size() ; it++){

    pat::Jet* jet = const_cast<pat::Jet*>(&(*jets)[it]);

    // newJet redone using possibly different JEC ==> it may not contain infos on bTag
    // so I use it together with jet
    pat::Jet* newJet = newJetMatched( jet , newJets );
    if(!newJet){
      cout << "No jet from the new collection can be matched ==> using old one..." << endl;
      newJet = jet;
    }


    if( Geom::deltaR(jet->p4(), leg1->p4())<deltaRLegJet_ || 
	Geom::deltaR(jet->p4(), leg2->p4())<deltaRLegJet_ ){
      if(verbose_) cout << "The jet at (" <<jet->pt()<<","<<jet->eta()<<") is closer than "<<deltaRLegJet_ << " from one of the legs" << endl;  
      continue;
    }

    /////////////////////////////////////////////////////////////////////////
    //// use JES uncertainties
    edm::ESHandle<JetCorrectorParametersCollection> jetCorrParameters;
    // get the jet corrector parameters collection from the global tag
    iSetup.get<JetCorrectionsRecord>().get("AK5PF", jetCorrParameters);
    // get the uncertainty parameters from the collection
    JetCorrectorParameters const & param = (*jetCorrParameters)["Uncertainty"];
    // instantiate the jec uncertainty object
    JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(param);
    deltaJEC->setJetEta(newJet->eta()); deltaJEC->setJetPt(newJet->pt());
    float shift  = deltaJEC->getUncertainty( true );
    /////////////////////////////////////////////////////////////////////////

    if(verbose_){
      cout << "Analyzing jet with pt " << (newJet->p4()).Pt() 
	   << "and eta " << (newJet->p4()).Eta() << endl;
      cout << " ==> JEC uncertainty is " << shift*100 << " %" << endl;
      for(unsigned int i = 0; i < (newJet->availableJECSets()).size() ; i++ ){
	std::cout << (newJet->availableJECSets())[i] << std::endl;
      }
      for(unsigned int i = 0; i < (newJet->availableJECLevels()).size() ; i++ ){
	std::cout << (newJet->availableJECLevels())[i] << std::endl;
      }
      std::cout << "L1FastJet ========> " << std::endl;
      std::cout << "Uncorrected " << newJet->correctedJet("Uncorrected","none","patJetCorrFactors").pt() << std::endl;
      std::cout << "L1FastJet "   << newJet->correctedJet("L1FastJet","none",  "patJetCorrFactors").pt() << std::endl;
      std::cout << "L2Relative "  << newJet->correctedJet("L2Relative","none", "patJetCorrFactors").pt() << std::endl; 
      std::cout << "L3Absolute "  << newJet->correctedJet("L3Absolute","none", "patJetCorrFactors").pt() << std::endl; 
      std::cout << "L1Offset ========> " << std::endl;
      std::cout << "Uncorrected " << newJet->jecFactor("Uncorrected","none","patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
      std::cout << "L1Offset"     << newJet->jecFactor("L1Offset","none",   "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
      std::cout << "L2Relative "  << newJet->jecFactor("L2Relative","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
      std::cout << "L3Absolute"   << newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;  
    }

    std::map<string,float> aMap;

    if( jetID( jet , &((*vertexes)[0]), vtxZ, aMap ) < minJetID_ )  continue;

    sortedJets.insert( make_pair( newJet->correctedJet("Uncorrected").p4().Pt() ,newJet->correctedJet("Uncorrected").p4() ) );

    if(newJet->p4().Pt() < minCorrPt_) continue;

    // add b-tag info
    bTaggers.insert(         make_pair( newJet->p4().Pt(), make_pair( jet->bDiscriminator("trackCountingHighEffBJetTags"),
								      jet->bDiscriminator("trackCountingHighPurBJetTags")  ) ) );
    // add pu information
    jetPVassociation.insert( make_pair( newJet->p4().Pt(), make_pair(aMap["chFracRawJetE"],
								  aMap["chFracAllChargE"]) ) );
    // add jet moments
    jetMoments.insert(       make_pair( newJet->p4().Pt(), make_pair( jet->etaetaMoment(),
								   jet->phiphiMoment()) ) );
   
    if(isMC_) 
      sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
					      newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->p4()) );   
    else 
      sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
					      newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->p4()) ); 

    if(verbose_) cout << "Components: "
		      << "px=" << (newJet->p4()).Px() << " (" << newJet->px() << "), "
		      << "py=" << (newJet->p4()).Py() << " (" << newJet->py() << "), "
		      << "pz=" << (newJet->p4()).Pz() << " (" << newJet->pz() << "), "
		      << "E="  << (newJet->p4()).E()  << " (" << newJet->energy()  << ")"
		      << endl;

    sortedJetsID.insert(     make_pair( newJet->p4().Pt() ,  newJet->p4() ) );
    sortedJetsIDUp.insert(   make_pair( newJet->p4().Pt() ,  newJet->p4()*(1+shift) ) );
    sortedJetsIDDown.insert( make_pair( newJet->p4().Pt() ,  newJet->p4()*(1-shift) ) );

    if(isMC_){
      if(jet->genJet() != 0) sortedGenJetsID.insert( make_pair( newJet->p4().Pt() ,jet->genJet()->p4() ) );
      else sortedGenJetsID.insert( make_pair( newJet->p4().Pt() , math::XYZTLorentzVectorD(0,0,0,0) ) );
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


unsigned int  ElecMuStreamAnalyzer::jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_){

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
	  float dist = vtxZ.size()>0 ? fabs(vtxZ[0]-((cand.trackRef())->referencePoint()).z()) : 999.;
	  float tmpDist = 999.;
	  for(unsigned k = 1; k < vtxZ.size(); k++){
	    if( fabs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	      tmpDist = fabs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] );
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
	float dist = vtxZ.size()>0 ? fabs(vtxZ[0]-((cand.gsfTrackRef())->referencePoint()).z()) : 999.;
	float tmpDist = 999.;
	for(unsigned k = 1; k < vtxZ.size(); k++){
	  if( fabs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	    tmpDist = fabs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] );
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

pat::Jet* ElecMuStreamAnalyzer::newJetMatched( const pat::Jet* oldJet , const pat::JetCollection* newJets ){

  pat::Jet* matchedJet = 0;

  for(unsigned int it = 0; it < newJets->size() ; it++){
    if( Geom::deltaR( (*newJets)[it].p4() , oldJet->p4() )<0.01 ){
      matchedJet = const_cast<pat::Jet*>(&((*newJets)[it]));
      break;
    }
  }

  return matchedJet;

}


void ElecMuStreamAnalyzer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(ElecMuStreamAnalyzer);


