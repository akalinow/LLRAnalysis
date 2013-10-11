#include "LLRAnalysis/TauTauStudies/interface/TauNTuplizer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

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

#include "DataFormats/GeometryVector/interface/VectorUtil.h"
#include "Math/VectorUtil.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>


#include <utility>
#include <map>
#include <vector>

using namespace std;
using namespace reco;


TauNTuplizer::TauNTuplizer(const edm::ParameterSet & iConfig){

  matchTo_       = iConfig.getParameter<std::string>("matchTo");
  vertexTag_     = iConfig.getParameter<edm::InputTag>("vertexTag");
  tauTag_        = iConfig.getParameter<edm::InputTag>("tauTag");
  electronTag_   = iConfig.getParameter<edm::InputTag>("electronTag");
  muonTag_       = iConfig.getParameter<edm::InputTag>("muonTag");
  electronIDTag_ = iConfig.getParameter<edm::InputTag>("electronIDTag");
  muonIDTag_     = iConfig.getParameter<edm::InputTag>("muonIDTag");

}

void TauNTuplizer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","tau");

  gammadEta_     = new std::vector< float >();
  gammadPhi_     = new std::vector< float >();
  gammaPt_       = new std::vector< float >();

  tree_->Branch("leadPFChargedHadrMva",&leadPFChargedHadrMva_,"leadPFChargedHadrMva/F");
  tree_->Branch("leadPFChargedHadrHcalEnergy",&leadPFChargedHadrHcalEnergy_,"leadPFChargedHadrHcalEnergy/F");
  tree_->Branch("leadPFChargedHadrEcalEnergy",&leadPFChargedHadrEcalEnergy_,"leadPFChargedHadrEcalEnergy/F");
  tree_->Branch("leadPFChargedHadrTrackPt",&leadPFChargedHadrTrackPt_,"leadPFChargedHadrTrackPt/F");
  tree_->Branch("leadPFChargedHadrTrackP",&leadPFChargedHadrTrackP_,"leadPFChargedHadrTrackP/F");
  tree_->Branch("leadPFCandMva",&leadPFCandMva_,"leadPFCandMva/F");
  tree_->Branch("leadPFCandHcalEnergy",&leadPFCandHcalEnergy_,"leadPFCandHcalEnergy/F");
  tree_->Branch("leadPFCandEcalEnergy",&leadPFCandEcalEnergy_,"leadPFCandEcalEnergy/F");
  tree_->Branch("leadPFCandPt",&leadPFCandPt_,"leadPFCandPt/F");
  tree_->Branch("leadPFCandP",&leadPFCandP_,"leadPFCandP/F");
  tree_->Branch("emFraction",&emFraction_,"emFraction/F");
  tree_->Branch("hasGsf",&hasGsf_,"hasGsf/F");
  tree_->Branch("hasKft",&hasKft_,"hasKft/F");

  tree_->Branch("fbrem",&fbrem_,"fbrem/F");
  tree_->Branch("nbrem",&nbrem_,"nbrem/F");
  tree_->Branch("signalPFChargedHadrCands",&signalPFChargedHadrCands_,"signalPFChargedHadrCands/I");
  tree_->Branch("signalPFGammaCands",&signalPFGammaCands_,"signalPFGammaCands/I");
  tree_->Branch("visMass",&visMass_,"visMass/F"); 
  tree_->Branch("pt",&pt_,"pt/F");
  tree_->Branch("eta",&eta_,"eta/F");
  tree_->Branch("dxy",&dxy_,"dxy/F");
  tree_->Branch("dz",&dz_,"dz/F");

  tree_->Branch("dPhi", &dPhi_, "dPhi/F");
  tree_->Branch("dEta", &dEta_, "dEta/F");
  tree_->Branch("sihih",&sihih_,"sihih/F");
  tree_->Branch("HoE",  &HoE_,  "HoE/F");

  tree_->Branch("gammadEta","std::vector<float> ",&gammadEta_);
  tree_->Branch("gammadPhi","std::vector<float> ",&gammadPhi_);
  tree_->Branch("gammaPt",  "std::vector<float> ",&gammaPt_);

  tree_->Branch("numPV",&numPV_,"numPV/I");
  tree_->Branch("charge",&charge_,"charge/I");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/I");
  tree_->Branch("puWeight",&puWeight_,"puWeight/F");


  tree_->Branch("electronVeto",&electronVeto_,"electronVeto/I");
  tree_->Branch("muonVeto",&muonVeto_,"muonVeto/I");
  tree_->Branch("isTriggerMatched",&isTriggerMatched_,"isTriggerMatched/I");

  tree_->Branch("tightestHPSDBWP",&tightestHPSDBWP_,"tightestHPSDBWP/I");
  tree_->Branch("tightestAntiEWP",&tightestAntiEWP_,"tightestAntiEWP/I");
  tree_->Branch("tightestAntiMWP",&tightestAntiMWP_,"tightestAntiMWP/I");

  std::vector< float > Wlumi ;

  float Wlumi_f[25]= {
    58776.0 ,
    58962.0 ,
    53010.0 ,
    57288.0 ,
    51894.0 ,
    59148.0 ,
    62868.0 ,
    58776.0 ,
    53754.0 ,
    58776.0 ,
    53010.0 ,
    48360.0 ,
    39990.0 ,
    26970.0 ,
    23994.0 ,
    15066.0 ,
    10044.0 ,
    6696.0 ,
    2232.0 ,
    2232.0 ,
    1488.0 ,
    700.0 ,
    300.0 ,
    170.0 ,
    85.0 
  };
  
  
  std::vector< float > TrueDist2011;
  float TrueDist2011_f[25] = {
    0.019091,
    0.0293974,
    0.0667931,
    0.108859,
    0.139533,
    0.149342,
    0.138629,
    0.114582,
    0.0859364,
    0.059324,
    0.0381123,
    0.0229881,
    0.0131129,
    0.00711764,
    0.00369635,
    0.00184543,
    0.000889604,
    0.000415683,
    0.000188921,
    0.000146288,
    0.0,
    0.0,
    0.0,
    0.0,
    0.0
  };
  
  for( int i=0; i<25; ++i) {
    TrueDist2011.push_back(TrueDist2011_f[i]);
    Wlumi.push_back(Wlumi_f[i]);
  }
  
  LumiWeights_ = edm::LumiReWeighting(Wlumi, TrueDist2011);

  
}


TauNTuplizer::~TauNTuplizer(){
  delete gammadEta_;
  delete gammadPhi_;
  delete gammaPt_;
}

void TauNTuplizer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){

  std::vector<math::XYZTLorentzVectorD> genMatchP4s;
  std::vector<int> charges;

  edm::Handle<pat::TauCollection> tausHandle;
  iEvent.getByLabel(tauTag_,tausHandle);
  if( !tausHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No taus label available \n";
  const pat::TauCollection* taus = tausHandle.product();

  edm::Handle<pat::ElectronCollection> electronsHandle;
  iEvent.getByLabel(electronTag_,electronsHandle);
  if( !electronsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No electrons label available \n";
  const pat::ElectronCollection* electrons = electronsHandle.product();

  edm::Handle<pat::MuonCollection> muonsHandle;
  iEvent.getByLabel(muonTag_,muonsHandle);
  if( !muonsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muons label available \n";
  const pat::MuonCollection* muons = muonsHandle.product();

  edm::Handle<pat::ElectronCollection> electronsIDHandle;
  iEvent.getByLabel(electronIDTag_,electronsIDHandle);
  if( !electronsIDHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No electrons ID label available \n";
  const pat::ElectronCollection* electronsID = electronsIDHandle.product();

  edm::Handle<pat::MuonCollection> muonsIDHandle;
  iEvent.getByLabel(muonIDTag_,muonsIDHandle);
  if( !muonsIDHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muons ID label available \n";
  const pat::MuonCollection* muonsID = muonsIDHandle.product();

  int nPUVertices    = -99;
  int nOOTPUVertices = -99;
  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  iEvent.getByLabel(edm::InputTag("addPileupInfo"), puInfoH);
  if(puInfoH.isValid()){
    for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){
      if(it->getBunchCrossing() ==0) nPUVertices = it->getPU_NumInteractions();
      else  nOOTPUVertices = it->getPU_NumInteractions();
    }
  }

  nPUVertices_ = nPUVertices;

  puWeight_    = LumiWeights_.weight( nPUVertices );


  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByLabel(vertexTag_,pvHandle);
  if( !pvHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No PV label available \n";
  const reco::VertexCollection* vertexes = pvHandle.product();
  numPV_ = vertexes->size();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();
  
  edm::Handle<reco::GenParticleCollection> genParticlesHandle;
  const reco::GenParticleCollection* genParticles = 0;

  if(  matchTo_.find("tau")!=string::npos){
    edm::Handle<reco::GenJetCollection> tauGenJetsHandle;
    iEvent.getByLabel(edm::InputTag("tauGenJetsSelectorAllHadrons"),tauGenJetsHandle);
    if( !tauGenJetsHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen jet label available \n";
    const reco::GenJetCollection* tauGenJets = tauGenJetsHandle.product();
    for(unsigned int j = 0; j<tauGenJets->size() ; j++){
      genMatchP4s.push_back( (*tauGenJets)[j].p4() );
      charges.push_back((*tauGenJets)[j].charge());
    }
  } 
  else if( matchTo_.find("electron")!=string::npos ){
    iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesHandle);
    if( !genParticlesHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No genparticles label available \n";
    genParticles = genParticlesHandle.product();
    for(unsigned int j = 0; j<genParticles->size() ; j++){
      if( TMath::Abs((*genParticles)[j].pdgId())!=11) continue;
      genMatchP4s.push_back( (*genParticles)[j].p4() );
      charges.push_back( (*genParticles)[j].charge() );
    }
  }
  else if( matchTo_.find("muon")!=string::npos ){
    iEvent.getByLabel(edm::InputTag("genParticles"),genParticlesHandle);
    if( !genParticlesHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No genparticles label available \n";
    genParticles = genParticlesHandle.product();
    for(unsigned int j = 0; j<genParticles->size() ; j++){
      if( TMath::Abs((*genParticles)[j].pdgId())!=13) continue;
      genMatchP4s.push_back( (*genParticles)[j].p4() );
      charges.push_back( (*genParticles)[j].charge() );
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

    for(unsigned int j = 0; j<genMatchP4s.size() && !matched; j++){

      if( Geom::deltaR( (*taus)[i].p4(), genMatchP4s[j] )<0.15){
	
	matched = true;

	tightestHPSDBWP_ = 0;
	tightestAntiEWP_ = 0;
	tightestAntiMWP_ = 0;
	muonVeto_        = 0;
	electronVeto_    = 0;
	fbrem_= -99;
	dPhi_ = -99;
	dEta_ = -99;
	sihih_= -99;
	HoE_  = -99;
	leadPFCandMva_        = -99;
	leadPFCandHcalEnergy_ = -99;
	leadPFCandEcalEnergy_ = -99;
	leadPFCandPt_         = -99;
	leadPFCandP_          = -99;
	gammadEta_->clear();
	gammadPhi_->clear();
	gammaPt_->clear();

	charge_ = (*taus)[i].charge() * charges[j];

	if((*taus)[i].tauID("byVLooseCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_++;
	if((*taus)[i].tauID("byLooseCombinedIsolationDeltaBetaCorr") >0.5)  tightestHPSDBWP_++;
	if((*taus)[i].tauID("byMediumCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_++;

	if((*taus)[i].tauID("againstElectronLoose") >0.5)  tightestAntiEWP_++;
	if((*taus)[i].tauID("againstElectronMedium")>0.5)  tightestAntiEWP_++;
	if((*taus)[i].tauID("againstElectronTight") >0.5)  tightestAntiEWP_++;

	if((*taus)[i].tauID("againstMuonLoose")>0.5)  tightestAntiMWP_++;
	if((*taus)[i].tauID("againstMuonTight")>0.5)  tightestAntiMWP_++;

	for(unsigned int m = 0; m < muonsID->size() ; m++){
	  if( Geom::deltaR( (*muonsID)[m].p4(), (*taus)[i].p4() )<0.15){
	    muonVeto_ = 1;
	  }
	}
	for(unsigned int e = 0; e<electronsID->size() ; e++){
	  if( Geom::deltaR( (*electronsID)[e].p4(), (*taus)[i].p4() )<0.15){
	    electronVeto_ = 1;
	  }
	}

	unsigned int matchMuon = 0;
	bool isMatchedMuon = false;
	for(unsigned int m = 0; m < muons->size() && !isMatchedMuon; m++){
	  if( Geom::deltaR( (*muons)[m].p4(), (*taus)[i].p4() )<0.15){
	    matchMuon = m; 
	    isMatchedMuon = true;
	  }
	}
	unsigned int matchElectron = 0;
	bool isMatchedElectron = false;
	for(unsigned int e = 0; e<electrons->size() && !isMatchedElectron; e++){
	  if( Geom::deltaR( (*electrons)[e].p4(), (*taus)[i].p4() )<0.15){
	    matchElectron = e;
	    isMatchedElectron = true;
	  }
	}

	if(isMatchedElectron){
	  if(  (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() &&
	       (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()->eta()- (*electrons)[matchElectron].eta())<1e-03 && 
	       (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()->phi()- (*electrons)[matchElectron].phi())<1e-03 ){
	    dPhi_  = (*electrons)[matchElectron].deltaPhiSuperClusterTrackAtVtx();
	    dEta_  = (*electrons)[matchElectron].deltaEtaSuperClusterTrackAtVtx();
	    sihih_ = (*electrons)[matchElectron].sigmaIetaIeta();
	    HoE_   = (*electrons)[matchElectron].hadronicOverEm();
	    nbrem_ = (*electrons)[matchElectron].numberOfBrems();
	    fbrem_ = (*electrons)[matchElectron].fbrem();
	  }
	}


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
	
	emFraction_ = (*taus)[i].emFraction();
	hasGsf_ = (((*taus)[i].leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() ?
	  1. : 0.;
	hasKft_ = (((*taus)[i].leadPFChargedHadrCand())->trackRef()).isNonnull() ?
	  1. : 0.;
	  
	pt_      = (*taus)[i].pt();
	dxy_     = vertexes->size()>0 ? TMath::Sqrt(pow((*taus)[i].vertex().x()-(*vertexes)[0].position().x(),2)+
						    pow((*taus)[i].vertex().y()-(*vertexes)[0].position().y(),2)) : -99;
	dz_      = vertexes->size()>0 ? (*taus)[i].vertex().z()-(*vertexes)[0].position().z() : -99;
	eta_     = (*taus)[i].eta();
	visMass_ = (*taus)[i].mass();

	signalPFChargedHadrCands_ = (*taus)[i].signalPFChargedHadrCands().size();
	signalPFGammaCands_       = (*taus)[i].signalPFGammaCands().size();

	for(unsigned int k = 0 ; k < ((*taus)[i].signalPFGammaCands()).size() ; k++){
	  reco::PFCandidatePtr gamma = ((*taus)[i].signalPFGammaCands()).at(k);
	  if( ((*taus)[i].leadPFChargedHadrCand()).isNonnull() ){
	    gammadEta_->push_back( gamma->eta() - (*taus)[i].leadPFChargedHadrCand()->eta() );
	    gammadPhi_->push_back( gamma->phi() - (*taus)[i].leadPFChargedHadrCand()->phi() );
	  }
	  else{
	    gammadEta_->push_back( gamma->eta() - (*taus)[i].eta() );
	    gammadPhi_->push_back( gamma->phi() - (*taus)[i].phi() );
	  }
	  gammaPt_->push_back(  gamma->pt() );
	}
	
	bool HLTmatched = false;
	for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
	  pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));
	  if( Geom::deltaR( aObj->triggerObject().p4(), (*taus)[i].p4() )<0.3 && 
	      aObj->hasFilterLabel("hltPFTau20TrackLooseIso") ){ //
	    HLTmatched = true;
	  }
	}
      
	isTriggerMatched_ = HLTmatched ? 1 : 0;

	tree_->Fill();
      }

    }
  }


}




void TauNTuplizer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(TauNTuplizer);


