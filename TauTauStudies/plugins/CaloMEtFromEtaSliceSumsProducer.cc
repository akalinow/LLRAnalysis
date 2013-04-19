#include "LLRAnalysis/TauTauStudies/plugins/CaloMEtFromEtaSliceSumsProducer.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"

#include <TMath.h>

CaloMEtFromEtaSliceSumsProducer::CaloMEtFromEtaSliceSumsProducer(const edm::ParameterSet& cfg)
  : moduleLabel_(cfg.getParameter<std::string>("@module_label")),
    residualCorrectorFromFile_(0)
{
  srcMEt_ = cfg.getParameter<edm::InputTag>("srcMEt");
  srcCaloTowerEtaSliceSums_ = cfg.getParameter<edm::InputTag>("srcCaloTowerEtaSliceSums");

  typedef std::vector<edm::ParameterSet> vParameterSet;
  vParameterSet cfgBinning = cfg.getParameter<vParameterSet>("binning");
  for ( vParameterSet::const_iterator cfgBinningEntry = cfgBinning.begin();
	cfgBinningEntry != cfgBinning.end(); ++cfgBinningEntry ) {
    binning_.push_back(new binningEntryType(*cfgBinningEntry));
  }
  
  residualCorrLabel_ = cfg.getParameter<std::string>("residualCorrLabel");
  residualCorrEtaMax_ = cfg.getParameter<double>("residualCorrEtaMax");
  extraCorrFactor_ = cfg.exists("extraCorrFactor") ? 
    cfg.getParameter<double>("extraCorrFactor") : 1.;
  if ( cfg.exists("residualCorrFileName") ) {
    edm::FileInPath residualCorrFileName = cfg.getParameter<edm::FileInPath>("residualCorrFileName");
    if ( !residualCorrFileName.isLocal()) 
      throw cms::Exception("CaloMEtFromEtaSliceSumsProducer") 
	<< " Failed to find File = " << residualCorrFileName << " !!\n";
    JetCorrectorParameters residualCorr(residualCorrFileName.fullPath().data());
    std::vector<JetCorrectorParameters> jetCorrections;
    jetCorrections.push_back(residualCorr);
    residualCorrectorFromFile_ = new FactorizedJetCorrector(jetCorrections);
  }  
  isMC_ = cfg.getParameter<bool>("isMC");

  verbosity_ = ( cfg.exists("verbosity") ) ?
    cfg.getParameter<int>("verbosity") : 0;
  
  produces<reco::CaloMETCollection>();
}

CaloMEtFromEtaSliceSumsProducer::~CaloMEtFromEtaSliceSumsProducer()
{
  delete residualCorrectorFromFile_;
  for ( std::vector<binningEntryType*>::const_iterator it = binning_.begin();
	it != binning_.end(); ++it ) {
    delete (*it);
  }
}

void CaloMEtFromEtaSliceSumsProducer::produce(edm::Event& evt, const edm::EventSetup& es)
{
  if ( verbosity_ ) {
    std::cout << "<CaloMEtFromEtaSliceSumsProducer::produce>:" << std::endl;
  }

  std::auto_ptr<reco::CaloMETCollection> correctedMEtCollection(new reco::CaloMETCollection());

  edm::Handle<reco::CaloMETCollection> uncorrectedMEtCollection;
  evt.getByLabel(srcMEt_, uncorrectedMEtCollection);

  const JetCorrector* residualCorrectorFromDB = 0;
  if ( !residualCorrectorFromFile_ && residualCorrLabel_ != "" ) {
    residualCorrectorFromDB = JetCorrector::getJetCorrector(residualCorrLabel_, es);
    if ( !residualCorrectorFromDB )  
      throw cms::Exception("CaloMEtFromEtaSliceSumsProducer")
	<< "Failed to access Residual corrections = " << residualCorrLabel_ << " !!\n";
  }
  
  int idx = 0;
  for ( reco::CaloMETCollection::const_iterator uncorrectedMEt = uncorrectedMEtCollection->begin();
	uncorrectedMEt != uncorrectedMEtCollection->end(); ++uncorrectedMEt ) {    
    if ( verbosity_ ) {
      std::cout << "uncorrectedMEt #" << idx << ": Pt = " << uncorrectedMEt->pt() << ", phi = " << uncorrectedMEt->phi() 
		<< " (Px = " << uncorrectedMEt->px() << ", Py = " << uncorrectedMEt->py() << ")" << std::endl;
    }
    
    double sumPx = 0.;
    double sumPy = 0.;
    double sumEt = 0.;
    
    for ( std::vector<binningEntryType*>::iterator binningEntry = binning_.begin();
	  binningEntry != binning_.end(); ++binningEntry ) {      
      edm::Handle<CorrMETData> binEnergySum;
      evt.getByLabel(edm::InputTag(srcCaloTowerEtaSliceSums_.label(), (*binningEntry)->binLabel_), binEnergySum);
      if ( verbosity_ ) {
	std::cout << "eta = " << (*binningEntry)->binCenter_ << ": sumPx = " << binEnergySum->mex << ", sumPy = " << binEnergySum->mey << ", sumEt = " << binEnergySum->sumet << std::endl;
      }
      double residualCorrFactor = 1.;
      if ( fabs((*binningEntry)->binCenter_) < residualCorrEtaMax_ ) {
	reco::Candidate::PolarLorentzVector binCenterP4(15., (*binningEntry)->binCenter_, 0., 0.);
	if ( residualCorrectorFromFile_ ) {
	  residualCorrectorFromFile_->setJetEta((*binningEntry)->binCenter_);
	  residualCorrectorFromFile_->setJetPt(10.);
	  residualCorrectorFromFile_->setJetA(0.25);
	  residualCorrectorFromFile_->setRho(10.); 
	  residualCorrFactor = residualCorrectorFromFile_->getCorrection();
	} else if ( residualCorrectorFromDB ) {	
	  reco::Candidate::PolarLorentzVector binCenterP4(15., (*binningEntry)->binCenter_, 0., 0.);
	  residualCorrFactor = residualCorrectorFromDB->correction(reco::Candidate::LorentzVector(binCenterP4.px(), binCenterP4.py(), binCenterP4.pz(), binCenterP4.E()));
	}
      }
      residualCorrFactor *= extraCorrFactor_;
      if ( isMC_ && residualCorrFactor != 0. ) residualCorrFactor = 1./residualCorrFactor;
      if ( verbosity_ ) {
	std::cout << "eta = " << (*binningEntry)->binCenter_ << ": residualCorrFactor = " << residualCorrFactor << std::endl;
      }
      sumPx += (residualCorrFactor*binEnergySum->mex);
      sumPy += (residualCorrFactor*binEnergySum->mey);
      sumEt += (residualCorrFactor*binEnergySum->sumet);
    }
    
    double correctedMEtPx = -sumPx;
    double correctedMEtPy = -sumPy;
    double correctedMEtPt = TMath::Sqrt(correctedMEtPx*correctedMEtPx + correctedMEtPy*correctedMEtPy);
    reco::Candidate::LorentzVector correctedMEtP4(correctedMEtPx, correctedMEtPy, 0., correctedMEtPt);
    reco::CaloMET correctedMEt(
      uncorrectedMEt->getSpecific(),
      sumEt,
      uncorrectedMEt->mEtCorr(),
      correctedMEtP4,
      uncorrectedMEt->vertex());
    if ( verbosity_ ) {
      std::cout << "correctedMEt #" << idx << ": Pt = " << correctedMEt.pt() << ", phi = " << correctedMEt.phi() 
		<< " (Px = " << correctedMEt.px() << ", Py = " << correctedMEt.py() << ")" << std::endl;
    }
    
    correctedMEtCollection->push_back(correctedMEt);

    ++idx;
  }

  evt.put(correctedMEtCollection);
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(CaloMEtFromEtaSliceSumsProducer);
