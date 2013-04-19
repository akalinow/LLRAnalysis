#include "LLRAnalysis/TauTauStudies/plugins/CorrectedCaloMETforEmbeddedProducer.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include <TMath.h>

#include <iostream>
#include <iomanip>

CorrectedCaloMETforEmbeddedProducer::CorrectedCaloMETforEmbeddedProducer(const edm::ParameterSet& cfg)
  : moduleLabel_(cfg.getParameter<std::string>("@module_label")),
    residualCorrectorFromFile_(0)
{
  srcMEt_ = cfg.getParameter<edm::InputTag>("srcMEt");
  srcTauJets_ = cfg.getParameter<vInputTag>("srcTauJets");

  residualCorrLabel_ = cfg.getParameter<std::string>("residualCorrLabel");
  residualCorrEtaMax_ = cfg.getParameter<double>("residualCorrEtaMax");
  extraCorrFactor_ = cfg.exists("extraCorrFactor") ? 
    cfg.getParameter<double>("extraCorrFactor") : 1.;
  if ( cfg.exists("residualCorrFileName") ) {
    edm::FileInPath residualCorrFileName = cfg.getParameter<edm::FileInPath>("residualCorrFileName");
    if ( !residualCorrFileName.isLocal()) 
      throw cms::Exception("CorrectedCaloMETforEmbeddedProducer") 
	<< " Failed to find File = " << residualCorrFileName << " !!\n";
    JetCorrectorParameters residualCorr(residualCorrFileName.fullPath().data());
    std::vector<JetCorrectorParameters> jetCorrections;
    jetCorrections.push_back(residualCorr);
    residualCorrectorFromFile_ = new FactorizedJetCorrector(jetCorrections);
  }  
  isMC_ = cfg.getParameter<bool>("isMC");
  
  verbosity_ = ( cfg.exists("verbosity") ) ?
    cfg.getParameter<int>("verbosity") : 0;
  
  produces<reco::CaloMETCollection>("");
}

CorrectedCaloMETforEmbeddedProducer::~CorrectedCaloMETforEmbeddedProducer()
{
  delete residualCorrectorFromFile_;
}

void CorrectedCaloMETforEmbeddedProducer::produce(edm::Event& evt, const edm::EventSetup& es)
{
  if ( verbosity_ ) {
    std::cout << "<CorrectedCaloMETforEmbeddedProducer::produce>:" << std::endl;
    std::cout << " moduleLabel = " << moduleLabel_ << std::endl;
  }
  
  std::auto_ptr<reco::CaloMETCollection> correctedMEtCollection(new reco::CaloMETCollection);
  
  edm::Handle<reco::CaloMETCollection> uncorrectedMEtCollection;
  evt.getByLabel(srcMEt_, uncorrectedMEtCollection);
  
  const JetCorrector* residualCorrectorFromDB = 0;
  if ( !residualCorrectorFromFile_ && residualCorrLabel_ != "" ) {
    residualCorrectorFromDB = JetCorrector::getJetCorrector(residualCorrLabel_, es);
    if ( !residualCorrectorFromDB )  
      throw cms::Exception("CorrectedCaloMETforEmbeddedProducer")
	<< "Failed to access Residual corrections = " << residualCorrLabel_ << " !!\n";
  }
  
  double correctionPx    = 0.;
  double correctionPy    = 0.;
  double correctionSumEt = 0.;
  for ( vInputTag::const_iterator srcTauJets_i = srcTauJets_.begin();
	srcTauJets_i != srcTauJets_.end(); ++srcTauJets_i ) {
    typedef edm::View<reco::Candidate> CandidateView;
    edm::Handle<CandidateView> tauJets;
    evt.getByLabel(*srcTauJets_i, tauJets);
  
    int idxTauJet = 0;
    for ( edm::View<reco::Candidate>::const_iterator tauJet = tauJets->begin();
	  tauJet != tauJets->end(); ++tauJet ) {
      if ( verbosity_ ) {
	std::cout << "tauJet (" << srcTauJets_i->label() << ") #" << idxTauJet << ": Pt = " << tauJet->pt() << ", eta = " << tauJet->eta() << ", phi = " << tauJet->phi() 	
		  << " (Px = " << tauJet->px() << ", Py = " << tauJet->py() << ")" << std::endl;
      }
      double residualCorrFactor = 1.;
      if ( fabs(tauJet->eta()) < residualCorrEtaMax_ ) {
	reco::Candidate::PolarLorentzVector binCenterP4(15., tauJet->eta(), 0., 0.);
	if ( residualCorrectorFromFile_ ) {
	  residualCorrectorFromFile_->setJetEta(tauJet->eta());
	  residualCorrectorFromFile_->setJetPt(10.);
	  residualCorrectorFromFile_->setJetA(0.25);
	  residualCorrectorFromFile_->setRho(10.); 
	  residualCorrFactor = residualCorrectorFromFile_->getCorrection();
	} else if ( residualCorrectorFromDB ) {	
	  reco::Candidate::PolarLorentzVector binCenterP4(15., tauJet->eta(), 0., 0.);
	  residualCorrFactor = residualCorrectorFromDB->correction(reco::Candidate::LorentzVector(binCenterP4.px(), binCenterP4.py(), binCenterP4.pz(), binCenterP4.E()));
	}
	if ( verbosity_ ) std::cout << " residualCorrFactor = " << residualCorrFactor << " (extraCorrFactor = " << extraCorrFactor_ << ")" << std::endl;
      }
      residualCorrFactor *= extraCorrFactor_;
      if ( isMC_ && residualCorrFactor != 0. ) residualCorrFactor = 1./residualCorrFactor;
      correctionPx    += (residualCorrFactor - 1.0)*tauJet->px();
      correctionPy    += (residualCorrFactor - 1.0)*tauJet->py();
      correctionSumEt += (residualCorrFactor - 1.0)*tauJet->et();
      ++idxTauJet;
    }
  }
  
  if ( verbosity_ ) {
    std::cout << "correction: px = " << correctionPx << ", py = " << correctionPy << ", sumEt = " << correctionSumEt << std::endl;
  }
  
  int idxMEt = 0;
  for ( reco::CaloMETCollection::const_iterator uncorrectedMEt = uncorrectedMEtCollection->begin();
	uncorrectedMEt != uncorrectedMEtCollection->end(); ++uncorrectedMEt ) {
    if ( verbosity_ ) {
      std::cout << "uncorrectedMET #" << idxMEt << ": Pt = " << uncorrectedMEt->pt() << ", phi = " << uncorrectedMEt->phi() 
		<< " (Px = " << uncorrectedMEt->px() << ", Py = " << uncorrectedMEt->py() << "), sumEt = " << uncorrectedMEt->sumEt() << std::endl;
    }      
    double correctedMEtPx = uncorrectedMEt->px()    - correctionPx;
    double correctedMEtPy = uncorrectedMEt->py()    - correctionPy;
    double correctedMEtPt = TMath::Sqrt(correctedMEtPx*correctedMEtPx + correctedMEtPy*correctedMEtPy);
    double correctedSumEt = uncorrectedMEt->sumEt() + correctionSumEt;
    reco::Candidate::LorentzVector correctedMEtP4(correctedMEtPx, correctedMEtPy, 0., correctedMEtPt);
    reco::CaloMET correctedMEt(
      uncorrectedMEt->getSpecific(),
      correctedSumEt,
      uncorrectedMEt->mEtCorr(),
      correctedMEtP4,
      uncorrectedMEt->vertex());
    if ( verbosity_ ) {
      std::cout << "correctedMEt #" << idxMEt << ": Pt = " << correctedMEt.pt() << ", phi = " << correctedMEt.phi() 
		<< " (Px = " << correctedMEt.px() << ", Py = " << correctedMEt.py() << "), sumEt = " << correctedMEt.sumEt() << std::endl;
    }
    
    correctedMEtCollection->push_back(correctedMEt);
    
    ++idxMEt;
  }
	  
//--- add collection of corrected CaloMET objects
  evt.put(correctedMEtCollection);
}
 
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(CorrectedCaloMETforEmbeddedProducer);


