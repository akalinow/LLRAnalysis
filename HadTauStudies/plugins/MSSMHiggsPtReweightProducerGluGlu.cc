#include "LLRAnalysis/HadTauStudies/plugins/MSSMHiggsPtReweightProducerGluGlu.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include <TString.h>

MSSMHiggsPtReweightProducerGluGlu::MSSMHiggsPtReweightProducerGluGlu(const edm::ParameterSet& cfg)
  : inputFile_(0)
{
  srcGenParticles_ = cfg.getParameter<edm::InputTag>("srcGenParticles");

  edm::FileInPath inputFileName = cfg.getParameter<edm::FileInPath>("inputFileName");
  if ( !inputFileName.isLocal() ) throw cms::Exception("MSSMHiggsPtReweightProducerGluGlu") 
    << " Failed to find File = " << inputFileName << " !!\n";
  inputFile_ = new TFile(inputFileName.fullPath().data());

  std::string lutName = cfg.getParameter<std::string>("lutName");
  
  mA_values_ = cfg.getParameter<vdouble>("mA_values");
  mu_values_ = cfg.getParameter<vdouble>("mu_values");
  higgsTypes_ = cfg.getParameter<vstring>("higgsTypes");
  
  central_or_shifts_ = cfg.getParameter<vstring>("shifts");
  central_or_shifts_.push_back("central");
  
  std::string instanceLabel = cfg.getParameter<std::string>("instanceLabel");

  for ( vdouble::const_iterator mA_value = mA_values_.begin();
	mA_value != mA_values_.end(); ++mA_value ) {
    for ( vdouble::const_iterator mu_value = mu_values_.begin();
	  mu_value != mu_values_.end(); ++mu_value ) {
      for ( vstring::const_iterator higgsType = higgsTypes_.begin();
	    higgsType != higgsTypes_.end(); ++higgsType ) {
	for ( vstring::const_iterator central_or_shift = central_or_shifts_.begin();
	      central_or_shift != central_or_shifts_.end(); ++central_or_shift ) {	  
	  TString lutName_expanded = lutName;
	  lutName_expanded = lutName_expanded.ReplaceAll("$mA", Form("%1.0f", *mA_value));
	  lutName_expanded = lutName_expanded.ReplaceAll("$mu", Form("%1.0f", *mu_value));
	  lutName_expanded = lutName_expanded.ReplaceAll("$higgsType", Form("%s", higgsType->data()));
	  lutName_expanded = lutName_expanded.ReplaceAll("$central_or_shift", Form("%s", central_or_shift->data()));
	  TH1* lut = dynamic_cast<TH1*>(inputFile_->Get(lutName_expanded.Data()));
	  if ( !lut ) throw cms::Exception("MSSMHiggsPtReweightProducerGluGlu") 
	    << " Failed to load LUT = " << lutName_expanded.Data() << " from file = " << inputFileName.fullPath().data() << " !!\n";
	  luts_[*mA_value][*mu_value][*higgsType][*central_or_shift] = lut;
	  
	  TString instanceLabel_expanded = instanceLabel;
	  instanceLabel_expanded = instanceLabel_expanded.ReplaceAll("$mA", Form("%1.0f", *mA_value));
	  instanceLabel_expanded = instanceLabel_expanded.ReplaceAll("$mu", Form("%1.0f", *mu_value));
	  instanceLabel_expanded = instanceLabel_expanded.ReplaceAll("$higgsType", Form("%s", higgsType->data()));
	  instanceLabel_expanded = instanceLabel_expanded.ReplaceAll("$central_or_shift", Form("%s", central_or_shift->data()));
	  instanceLabels_[*mA_value][*mu_value][*higgsType][*central_or_shift] = instanceLabel_expanded.Data();

	  produces<double>(instanceLabel_expanded.Data());
	}
      }
    }
  }

  maxWarnings_ = ( cfg.exists("maxWarnings") ) ?
    cfg.getParameter<int>("maxWarnings") : 1;
  numWarnings_ = 0;
}

MSSMHiggsPtReweightProducerGluGlu::~MSSMHiggsPtReweightProducerGluGlu() 
{
  delete inputFile_;
}

namespace
{
  bool findGenParticle(const reco::GenParticleCollection& genParticles, int absPdgId1, int absPdgId2, int absPdgId3, reco::Candidate::LorentzVector& p4)
  {
    for ( reco::GenParticleCollection::const_iterator genParticle = genParticles.begin();
	  genParticle != genParticles.end(); ++genParticle ) {
      int absGenParticlePdgId = TMath::Abs(genParticle->pdgId());
      if ( (absPdgId1 != 0 && absGenParticlePdgId == absPdgId1) ||
	   (absPdgId2 != 0 && absGenParticlePdgId == absPdgId2) ||
	   (absPdgId3 != 0 && absGenParticlePdgId == absPdgId3) ) {
	p4 = genParticle->p4();
	return true;
      }
    }
    return false;
  }
}

void MSSMHiggsPtReweightProducerGluGlu::produce(edm::Event& evt, const edm::EventSetup& es)
{
  edm::Handle<reco::GenParticleCollection> genParticles;
  evt.getByLabel(srcGenParticles_, genParticles);

  reco::Candidate::LorentzVector higgsP4;
  bool isHiggs = findGenParticle(*genParticles, 25, 35, 36, higgsP4);
  
  for ( vdouble::const_iterator mA_value = mA_values_.begin();
	mA_value != mA_values_.end(); ++mA_value ) {
    for ( vdouble::const_iterator mu_value = mu_values_.begin();
	  mu_value != mu_values_.end(); ++mu_value ) {
      for ( vstring::const_iterator higgsType = higgsTypes_.begin();
	    higgsType != higgsTypes_.end(); ++higgsType ) {
	for ( vstring::const_iterator central_or_shift = central_or_shifts_.begin();
	      central_or_shift != central_or_shifts_.end(); ++central_or_shift ) {
	  std::auto_ptr<double> weightPtr(new double(1.0));
	  if ( isHiggs ) {
	    TH1* lut = luts_[*mA_value][*mu_value][*higgsType][*central_or_shift];
	    assert(lut);
	    int idxBin = lut->FindBin(higgsP4.pt());
	    if ( idxBin < 1                ) idxBin = 1;
	    if ( idxBin > lut->GetNbinsX() ) idxBin = lut->GetNbinsX();
	    (*weightPtr) = lut->GetBinContent(idxBin);
	  } else {
	    if ( numWarnings_ < maxWarnings_ ) {
	      edm::LogWarning ("MSSMHiggsPtReweightProducerGluGlu") 
		<< " Failed to find generator level Higgs boson --> skipping !!";
	      ++numWarnings_;	
	    }
	  }
	  evt.put<double>(weightPtr, instanceLabels_[*mA_value][*mu_value][*higgsType][*central_or_shift]);
	}
      }
    }
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(MSSMHiggsPtReweightProducerGluGlu);


