#include "LLRAnalysis/HadTauStudies/plugins/TauTauNtupleProducer.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"

// reconstructed electrons, muons, hadronic taus,
// jets and MET
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/TauReco/interface/PFTau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

// trigger matching
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

// generator level information
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "TauAnalysis/CandidateTools/interface/candidateAuxFunctions.h"

// jet IDs
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h" 
#include "DataFormats/Common/interface/ValueMap.h"

// jet energy uncertainties
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Type1MET/interface/JetCorrExtractorT.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "FWCore/Framework/interface/ESHandle.h"

// reconstructed vertices
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// generator level in-time and out-of-time pile-up
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenFilterInfo.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/normalizedPhi.h"

// SVfit
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEtFwd.h"
#include "TauAnalysis/SVfitStandalone/interface/SVfitStandaloneAlgorithm.h"

// Trigger turn-on curves
#include "LLRAnalysis/HadTauStudies/interface/triggerTurnOnCurves.h"

#include <TMath.h>

#include <iostream>

TauTauNtupleProducer::TauTauNtupleProducer(const edm::ParameterSet& cfg) 
  : moduleLabel_(cfg.getParameter<std::string>("@module_label")),
    jec_(0),
    jecUncertainty_(0),
    inputFileHiggsPtWeight_(0),
    lutHiggsPtWeightNom_(0),
    lutHiggsPtWeightUp_(0),
    lutHiggsPtWeightDown_(0),
    loosePFJetIdAlgo_(0),
    augMT2edAlgo_(0),
    ntuple_(0)
{
  srcDiTau_ = cfg.getParameter<edm::InputTag>("srcDiTau");
  bestDiTauPreselection_ = cfg.getParameter<vstring>("bestDiTauPreselection");
  bestDiTauRanking_ = cfg.getParameter<vstring>("bestDiTauRanking");
  invertRanking_ = cfg.getParameter<bool>("invertRanking");
  
  tauIdDiscrinatorsToStore_ = readStringEntries(cfg, "tauIdDiscriminators");
  
  srcTriggerResults_ = cfg.getParameter<edm::InputTag>("srcTriggerResults");
  vstring hltPaths_diTau_vstring = cfg.getParameter<vstring>("hltPaths_diTau");
  for ( vstring::const_iterator hltPath = hltPaths_diTau_vstring.begin();
	hltPath != hltPaths_diTau_vstring.end(); ++hltPath ) {
    hltPaths_diTau_.push_back(StringEntryType(*hltPath, *hltPath));
  }
  hltPathsToStore_.insert(hltPathsToStore_.end(), hltPaths_diTau_.begin(), hltPaths_diTau_.end());
  vstring hltPaths_diTauJet_vstring = cfg.getParameter<vstring>("hltPaths_diTauJet");
  for ( vstring::const_iterator hltPath = hltPaths_diTauJet_vstring.begin();
	hltPath != hltPaths_diTauJet_vstring.end(); ++hltPath ) {
    hltPaths_diTauJet_.push_back(StringEntryType(*hltPath, *hltPath));
  }
  hltPathsToStore_.insert(hltPathsToStore_.end(), hltPaths_diTauJet_.begin(), hltPaths_diTauJet_.end());
  vstring hltPaths_singleJet_vstring = cfg.getParameter<vstring>("hltPaths_singleJet");
  for ( vstring::const_iterator hltPath = hltPaths_singleJet_vstring.begin();
	hltPath != hltPaths_singleJet_vstring.end(); ++hltPath ) {
    hltPaths_singleJet_.push_back(StringEntryType(*hltPath, *hltPath));
  }
  hltPathsToStore_.insert(hltPathsToStore_.end(), hltPaths_singleJet_.begin(), hltPaths_singleJet_.end());
  srcTriggerObjects_ = cfg.getParameter<edm::InputTag>("srcTriggerObjects");
  hltTauFilters_diTau_ = cfg.getParameter<vstring>("hltTauFilters_diTau");
  hltTauFilters_diTauJet_ = cfg.getParameter<vstring>("hltTauFilters_diTauJet");
  hltJetFilters_diTauJet_ = cfg.getParameter<vstring>("hltJetFilters_diTauJet");
  hltJetFilters_singleJet_ = cfg.getParameter<vstring>("hltJetFilters_singleJet");

  srcL1Taus_ = cfg.getParameter<edm::InputTag>("srcL1Taus");
  srcL1Jets_ = cfg.getParameter<edm::InputTag>("srcL1Jets");

  srcElectrons_ = cfg.getParameter<edm::InputTag>("srcElectrons");
  srcMuons_ = cfg.getParameter<edm::InputTag>("srcMuons");

  srcJets_ = cfg.getParameter<edm::InputTag>("srcJets");
  srcPileupJetId_ = cfg.getParameter<edm::InputTag>("srcPileupJetId");
  std::string wpPileupJetId_string = cfg.getParameter<std::string>("wpPileupJetId");
  if      ( wpPileupJetId_string == "Loose"  ) wpPileupJetId_ = PileupJetIdentifier::kLoose;
  else if ( wpPileupJetId_string == "Medium" ) wpPileupJetId_ = PileupJetIdentifier::kMedium;
  else if ( wpPileupJetId_string == "Tight"  ) wpPileupJetId_ = PileupJetIdentifier::kTight;
  else throw cms::Exception("TauTauNtupleProducer")
    << "Invalid Configuration Parameter 'wpPileupJetId' = " << wpPileupJetId_string << " !!\n";
  srcPileupJetIdMVA_ = cfg.getParameter<edm::InputTag>("srcPileupJetIdMVA");
  bJetDiscriminator_ = cfg.getParameter<std::string>("bJetDiscriminator");  
  jetCorrLabel_ = cfg.getParameter<std::string>("jetCorrLabel");
  jecUncertaintyTag_ = cfg.getParameter<std::string>("jecUncertaintyTag");
  if ( cfg.exists("jecUncertaintyFile") ) {
    edm::FileInPath jecUncertaintyFile = cfg.getParameter<edm::FileInPath>("jecUncertaintyFile");
    if ( jecUncertaintyFile.location() == edm::FileInPath::Unknown ) 
      throw cms::Exception("TauTauNtupleProducer") 
	<< " Failed to find JEC parameter file = " << jecUncertaintyFile << " !!\n";
    std::cout << "Reading JEC parameters = " << jecUncertaintyTag_  
	      << " from file = " << jecUncertaintyFile.fullPath() << "." << std::endl;
    JetCorrectorParameters jecUncertaintyParameters(jecUncertaintyFile.fullPath().data(), jecUncertaintyTag_);
    jecUncertainty_ = new JetCorrectionUncertainty(jecUncertaintyParameters);
  } else {
    std::cout << "Reading JEC parameters = " << jecUncertaintyTag_
	      << " from DB/SQLlite file." << std::endl;
    jetCorrPayload_ = cfg.getParameter<std::string>("jetCorrPayload");
  }
  
  minJetPt_ = cfg.getParameter<double>("minJetPt");
  maxJetAbsEta_ = cfg.getParameter<double>("maxJetAbsEta");
  minBJetPt_ = cfg.getParameter<double>("minBJetPt");
  maxBJetAbsEta_ = cfg.getParameter<double>("maxBJetAbsEta");
  wpBJetDiscriminator_ = cfg.getParameter<double>("wpBJetDiscriminator");

  srcRawPFMEt_ = cfg.getParameter<edm::InputTag>("srcRawPFMEt");
  srcGenMEt_ = cfg.getParameter<edm::InputTag>("srcGenMEt");

  srcVertices_ = cfg.getParameter<edm::InputTag>("srcVertices");
  srcRho_ = cfg.getParameter<edm::InputTag>("srcRho");
  
  edm::ParameterSet evtWeights = cfg.getParameter<edm::ParameterSet>("evtWeights");
  typedef std::vector<std::string> vstring;
  vstring evtWeightNames = evtWeights.getParameterNamesForType<edm::InputTag>();
  for ( vstring::const_iterator name = evtWeightNames.begin();
	name != evtWeightNames.end(); ++name ) {    
    edm::InputTag src = evtWeights.getParameter<edm::InputTag>(*name);
    evtWeightsToStore_.push_back(InputTagEntryType(*name, src));
  }
  
  isMC_ = cfg.getParameter<bool>("isMC");
  if ( isMC_ ) {
    srcGenPileUpSummary_ = cfg.getParameter<edm::InputTag>("srcGenPileUpSummary");
    srcLHE_ = cfg.getParameter<edm::InputTag>("srcLHE");
  }
  
  isEmbedded_ = cfg.getParameter<bool>("isEmbedded");
  if ( isEmbedded_ ) {
    srcEmbeddingWeight_ = cfg.getParameter<edm::InputTag>("srcEmbeddingWeight");
  }

  if ( isMC_ || isEmbedded_ ) {
    srcGenParticles_ = cfg.getParameter<edm::InputTag>("srcGenParticles");
    srcGenParticlesForTopPtReweighting_ = cfg.getParameter<edm::InputTag>("srcGenParticlesForTopPtReweighting");
  } 

  edm::ParameterSet cfgPFJetIdAlgo;
  cfgPFJetIdAlgo.addParameter<std::string>("version", "FIRSTDATA");
  cfgPFJetIdAlgo.addParameter<std::string>("quality", "LOOSE");
  loosePFJetIdAlgo_ = new PFJetIDSelectionFunctor(cfgPFJetIdAlgo);

  augMT2edAlgo_ = new mt2Interface();

  verbosity_ = ( cfg.exists("verbosity") ) ?
    cfg.getParameter<int>("verbosity") : 0;
}

TauTauNtupleProducer::~TauTauNtupleProducer()
{
  delete jecUncertainty_;

  delete inputFileHiggsPtWeight_;

  delete loosePFJetIdAlgo_;

  delete augMT2edAlgo_;
}

std::vector<TauTauNtupleProducer::StringEntryType> TauTauNtupleProducer::readStringEntries(const edm::ParameterSet& cfg, const std::string& psetName)
{
  std::vector<TauTauNtupleProducer::StringEntryType> retVal;
  edm::ParameterSet pset = cfg.getParameter<edm::ParameterSet>(psetName);
  vstring psetNames = pset.getParameterNamesForType<std::string>();
  for ( vstring::const_iterator name = psetNames.begin();
	name != psetNames.end(); ++name ) {
    std::string src = pset.getParameter<std::string>(*name);
    retVal.push_back(TauTauNtupleProducer::StringEntryType(*name, src));
  }
  return retVal;
}

void TauTauNtupleProducer::beginJob()
{
//--- create TTree
  edm::Service<TFileService> fs;
  ntuple_ = fs->make<TTree>("H2TauTauTreeProducerTauTau", "H2TauTauTreeProducerTauTau");

//--- add branches 
  addBranchUL("run");
  addBranchUL("event");
  addBranchUL("lumi");

  // diTau observables
  addBranch_diTau("diTau");
  addBranchF("pZetaVis");
  addBranchF("pZeta");	
  addBranchF("dRttL1");

  // tau observables
  addBranch_Tau("l1");
  addBranch_TauLT_base("l1");
  addBranch_TauLT_extended("l1");
  addBranchI("l1isGenHadTau");
  addBranchI("l1isGenMuon");
  addBranchI("l1isGenElectron");  
  addBranchI("l1isGenJet");
  addBranchF("l1LooseMu");
  addBranchF("l1LooseEle");
  addBranch_EnPxPyPz("l1L1");
  addBranch_PtEtaPhiMass("l1L1");
  addBranchI("l1TrigMatched_diTau");
  addBranchI("l1TrigMatched_diTauJet");
  addBranchI("l1TrigMatched_singleJet");
  addBranchF("mt1");
 
  addBranch_Tau("l2");
  addBranch_TauLT_base("l2");
  addBranch_TauLT_extended("l2");
  addBranchI("l2isGenHadTau");
  addBranchI("l2isGenMuon");
  addBranchI("l2isGenElectron");  
  addBranchI("l2isGenJet");
  addBranchF("l2LooseMu");
  addBranchF("l2LooseEle");
  addBranch_EnPxPyPz("l2L1");
  addBranch_PtEtaPhiMass("l2L1");
  addBranchI("l2TrigMatched_diTau");
  addBranchI("l2TrigMatched_diTauJet");
  addBranchI("l2TrigMatched_singleJet");
  addBranchF("mt2");
  
  // jet observables
  addBranch_Jet("jet1");
  addBranchF("jet1PtErr");
  addBranchF("jet1PUJetId");
  addBranchF("jet1PUJetIdMVA");
  addBranchF("jet1Bmatch");
  addBranchI("jetTrigMatched_diTauJet");
  addBranch_Jet("jet2");
  addBranchF("jet2PtErr");
  addBranchF("jet2PUJetId");
  addBranchF("jet2PUJetIdMVA");
  addBranchF("jet2Bmatch");
  addBranch_Jet("jet3");
  addBranchF("jet3PtErr");
  addBranchF("jet3PUJetId");
  addBranchF("jet3PUJetIdMVA");
  addBranchF("jet3Bmatch");
  addBranch_Jet("jet4");
  addBranchF("jet4PtErr");
  addBranchF("jet4PUJetId");
  addBranchF("jet4PUJetIdMVA");
  addBranchF("jet4Bmatch");
  addBranchI("nCentralJets");
  addBranchI("nJets");
  addBranchF("dPhijj");
  addBranchF("dEtajj");
  addBranchF("mjj");
  addBranchF("mttj");
    
  // b-jet observables
  addBranch_bJet("bjet1");
  addBranch_bJet("bjet2");
  addBranch_bJet("bjet3");
  addBranch_bJet("bjet4");
  addBranchI("nbJets");
  addBranchF("mbb");
  addBranchF("mbbReg");
  addBranchF("ptbb");

  // MET observables
  addBranchF("met");
  addBranchF("mex");
  addBranchF("mey");
  addBranchF("metPhi");
  addBranch_Cov2d("mvacov");
  addBranchF("rawMET");
  addBranchF("rawMEx");
  addBranchF("rawMEy");
  if ( srcGenMEt_.label() != "" ) {
    addBranchF("genMET");
    addBranchF("genMEx");
    addBranchF("genMEy");
  }

  // augMT2ed variable, described in the paper
  //  "Di-Higgs final states augMTed - selecting hh events at the high luminosity LHC",
  //   A.J. Barr, M.J. Dolan, C. Englert and M. Spannowsky
  //  arXiv: 1309.6318
  addBranchF("augMT2ed");

  // (veto) electron observables
  addBranch_Electron("electron1");

  // (veto) muon observables
  addBranch_Muon("muon1");

  // event weights/efficiencies
  for ( std::vector<InputTagEntryType>::const_iterator evtWeight = evtWeightsToStore_.begin();
	evtWeight != evtWeightsToStore_.end(); ++evtWeight ) {
    addBranchF(evtWeight->branchName_);
  }
  addBranchF("genMass");
  addBranchF("genFilter");
  addBranchF("genHiggsPt");
  addBranchF("genHiggsMass");
  addBranchF("higgsPtWeightNom");
  addBranchF("higgsPtWeightUp");
  addBranchF("higgsPtWeightDown");
  addBranchF("genTopPlusPt");
  addBranchF("genTopMinusPt");
  addBranchF("topPtWeightNom");
  addBranchF("topPtWeightUp");
  addBranchF("topPtWeightDown");
  addBranchF("leg1triggerEffMC_diTau");
  addBranchF("leg1triggerEffData_diTau");
  addBranchF("leg2triggerEffMC_diTau");
  addBranchF("leg2triggerEffData_diTau");
  addBranchF("triggerEffMC_diTau");
  addBranchF("triggerEffData_diTau");
  addBranchF("leg1triggerEffMC_diTauJet");
  addBranchF("leg1triggerEffData_diTauJet");
  addBranchF("leg2triggerEffMC_diTauJet");
  addBranchF("leg2triggerEffData_diTauJet");
  addBranchF("jetTriggerEffMC_diTauJet");
  addBranchF("jetTriggerEffData_diTauJet");
  addBranchF("triggerEffMC_diTauJet"); 
  addBranchF("triggerEffData_diTauJet");
  addBranchF("triggerEffMC");
  addBranchF("triggerEffData");
  addBranchF("leg1triggerWeight_diTau");
  addBranchF("leg2triggerWeight_diTau");
  addBranchF("triggerWeight_diTau");
  addBranchF("leg1triggerWeight_diTauJet");
  addBranchF("leg2triggerWeight_diTauJet");
  addBranchF("triggerWeight_diTauJet");
  addBranchF("triggerWeight");
  addBranchF("weight");
  
  // event flags
  for ( std::vector<StringEntryType>::const_iterator hltPathEntry = hltPathsToStore_.begin();
	hltPathEntry != hltPathsToStore_.end(); ++hltPathEntry ) {
    addBranchF(hltPathEntry->branchName_);
  }
  addBranchI("nVert");
  addBranch_XYZ("vertex");
  addBranchF("vertexRho");
  addBranchF("vertexNDoF");
  addBranchF("vertexIsValid");
  addBranchF("rho");
  addBranchI("nPUbx0");
  addBranchI("nPUbxP1");
  addBranchI("nPUbxM1");
  addBranchI("isZtt");
  addBranchI("isZttj");
  addBranchI("isZttl");
  addBranchI("isZj");
  addBranchI("isZee");
  addBranchI("isZmm");
  addBranchI("hasZ");
  addBranchI("hasW");
  addBranchF("NUP");
  addBranchI("is1RealTau");
  addBranchI("is2RealTaus");
  addBranchI("isFake");
  addBranchI("isElectron");
  addBranchI("isMuon");
  addBranchI("isJet");
  addBranchI("isPhoton");
}

namespace
{
  struct isMoreIsolatedDiTau
  {
    isMoreIsolatedDiTau(const std::vector<std::string>& tauIdDiscrPreselection, const std::vector<std::string>& tauIdDiscrRanking, bool invertRanking)
      : tauIdDiscrPreselection_(tauIdDiscrPreselection),
	tauIdDiscrRanking_(tauIdDiscrRanking),
	invertRanking_(invertRanking)
    {}
    bool operator() (const PATDiTauPair* diTau1, const PATDiTauPair* diTau2)
    {
      //std::cout << "<isMoreIsolatedDiTau::operator()>:" << std::endl;
      //std::cout << "diTau1:" << std::endl;
      //std::cout << " leg1: Pt = " << diTau1->leg1()->pt() << ", eta = " << diTau1->leg1()->eta() << ", phi = " << diTau1->leg1()->phi() << std::endl;
      //std::cout << " leg2: Pt = " << diTau1->leg2()->pt() << ", eta = " << diTau1->leg2()->eta() << ", phi = " << diTau1->leg2()->phi() << std::endl;
      //std::cout << "diTau2:" << std::endl;
      //std::cout << " leg1: Pt = " << diTau2->leg1()->pt() << ", eta = " << diTau2->leg1()->eta() << ", phi = " << diTau2->leg1()->phi() << std::endl;
      //std::cout << " leg2: Pt = " << diTau2->leg2()->pt() << ", eta = " << diTau2->leg2()->eta() << ", phi = " << diTau2->leg2()->phi() << std::endl;
      const pat::Tau* diTau1leg1 = &(*diTau1->leg1());
      const pat::Tau* diTau1leg2 = &(*diTau1->leg2());
      const pat::Tau* diTau2leg1 = &(*diTau2->leg1());
      const pat::Tau* diTau2leg2 = &(*diTau2->leg2());
      for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrPreselection_.begin();
	    tauIdDiscr != tauIdDiscrPreselection_.end(); ++tauIdDiscr ) {
	//std::cout << "tauIdDiscr = " << (*tauIdDiscr) << std::endl;
	int numPassed1 = 0;
	if ( diTau1leg1->tauID(*tauIdDiscr) > 0.5 ) ++numPassed1;
	if ( diTau1leg2->tauID(*tauIdDiscr) > 0.5 ) ++numPassed1;
	int numPassed2 = 0;
	if ( diTau2leg1->tauID(*tauIdDiscr) > 0.5 ) ++numPassed2;
	if ( diTau2leg2->tauID(*tauIdDiscr) > 0.5 ) ++numPassed2;
	//std::cout << "numPassed1 = " << numPassed1 << ", numPassed2 = " << numPassed2 << std::endl;
	if ( numPassed1 > numPassed2 ) return true;
	if ( numPassed2 > numPassed1 ) return false;
      }
      for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrRanking_.begin();
	    tauIdDiscr != tauIdDiscrRanking_.end(); ++tauIdDiscr ) {
	//std::cout << "tauIdDiscr = " << (*tauIdDiscr) << std::endl;
	double rank1 = diTau1leg1->tauID(*tauIdDiscr) + diTau1leg2->tauID(*tauIdDiscr);
	double rank2 = diTau2leg1->tauID(*tauIdDiscr) + diTau2leg2->tauID(*tauIdDiscr);
	//std::cout << "rank1 = " << rank1 << ", rank2 = " << rank2 << std::endl;
	if ( rank1 > rank2 ) return (true^invertRanking_);
	if ( rank2 > rank1 ) return (false^invertRanking_);
      }
      // CV: both diTau objects have the same isolation,
      //     take the higher Pt object to make the sorting order well-defined in all cases
      //std::cout << "diTau1 and diTau2 have equal rank --> returning diTau of higher Pt." << std::endl;
      return ((diTau1leg1->pt()*diTau1leg2->pt()) > (diTau2leg1->pt()*diTau2leg2->pt()));
    }
    std::vector<std::string> tauIdDiscrPreselection_;
    std::vector<std::string> tauIdDiscrRanking_;
    bool invertRanking_;
  };

  struct isHigherPtDiTau 
  {
    bool operator() (const PATDiTauPair* diTau1, const PATDiTauPair* diTau2)
    {
      return ((diTau1->leg1()->pt()*diTau1->leg2()->pt()) > (diTau2->leg1()->pt()*diTau2->leg2()->pt()));
    }
  };

  struct isHigherPtParticle
  {
    bool operator() (const reco::Candidate* particle1, const reco::Candidate* particle2)
    {
      return (particle1->pt() > particle2->pt());
    }
  };

  struct isHigherBtagDiscriminator
  {
    isHigherBtagDiscriminator(const std::string& bJetDiscriminator)
      : bJetDiscriminator_(bJetDiscriminator)
    {}
    ~isHigherBtagDiscriminator() {}
    bool operator() (const pat::Jet* jet1, const pat::Jet* jet2)
    {
      return (jet1->bDiscriminator(bJetDiscriminator_) > jet2->bDiscriminator(bJetDiscriminator_));
    }
    std::string bJetDiscriminator_;
  };

  // functions for matching in dR
  template <typename T>
  double minDeltaR(const reco::Candidate::LorentzVector& p4, const std::vector<T>& particles)
  {
    double dRmin = 99.;
    for ( typename std::vector<T>::const_iterator particle = particles.begin();
	  particle != particles.end(); ++particle ) {
      double dR = deltaR(p4, particle->p4());
      if ( dR < dRmin ) {
	dRmin = dR;
      }
    }
    return dRmin;
  }

  const l1extra::L1JetParticle* matchL1Object(const reco::Candidate::LorentzVector& p4, const l1extra::L1JetParticleCollection& l1Taus_or_Jets)
  {
    const l1extra::L1JetParticle* retVal = 0;
    double dRmin = 1.e+3;
    for ( l1extra::L1JetParticleCollection::const_iterator l1Tau_or_Jet = l1Taus_or_Jets.begin();
	  l1Tau_or_Jet != l1Taus_or_Jets.end(); ++l1Tau_or_Jet ) {
      double dR = deltaR(p4, l1Tau_or_Jet->p4());
      if ( dR < dRmin ) {
	retVal = &(*l1Tau_or_Jet);
	dRmin = dR;
      }
    }    
    return retVal;
  } 

  const l1extra::L1JetParticle* matchL1Object(const reco::Candidate::LorentzVector& p4, const l1extra::L1JetParticleCollection& l1Taus, const l1extra::L1JetParticleCollection& l1Jets)
  {
    const l1extra::L1JetParticle* matchedTau = matchL1Object(p4, l1Taus);
    double dR_Tau = ( matchedTau ) ? deltaR(p4, matchedTau->p4()) : 1.e+3;
    const l1extra::L1JetParticle* matchedJet = matchL1Object(p4, l1Jets);
    double dR_Jet = ( matchedJet ) ? deltaR(p4, matchedJet->p4()) : 1.e+3;
    const l1extra::L1JetParticle* retVal = 0;
    if ( matchedTau && dR_Tau < dR_Jet && dR_Tau < 0.5 && matchedTau->pt() > 44. && TMath::Abs(matchedTau->eta()) < 2.1 ) retVal = matchedTau;
    if ( matchedJet && dR_Jet < dR_Tau && dR_Jet < 0.5 && matchedJet->pt() > 64.                                        ) retVal = matchedJet;
    return retVal;
  }

  bool matchHLTFilter(const reco::Candidate::LorentzVector& p4, const pat::TriggerObjectStandAloneCollection& triggerObjects, const std::vector<std::string>& hltFilters, int triggerType)
  {
    for ( pat::TriggerObjectStandAloneCollection::const_iterator triggerObject = triggerObjects.begin();
	  triggerObject != triggerObjects.end(); ++triggerObject ) {
      if ( triggerObject->hasTriggerObjectType(triggerType) ) {
	double dR = deltaR(p4, (const_cast<pat::TriggerObjectStandAlone*>(&(*triggerObject)))->triggerObject().p4());
	if ( dR < 0.5 ) {
	  bool matchesFilterName = false;
	  for ( std::vector<std::string>::const_iterator hltFilter = hltFilters.begin();
		hltFilter != hltFilters.end(); ++hltFilter ) {
	    if ( triggerObject->hasFilterLabel(*hltFilter) ) {
	      matchesFilterName = true;
	      break;
	    }
	  }
	  if ( matchesFilterName ) return true;
	}
      }
    }
    return false;
  }

  //-----------------------------------------------------------------------------
  // CV: copied from TauAnalysis/CandidateTools/interface/CompositePtrCandidateT1T2MEtAlgorithm.h
  double compMt(const reco::Candidate::LorentzVector& visParticle,
		double metPx, double metPy)
  {
    double px = visParticle.px() + metPx;
    double py = visParticle.py() + metPy;
    double et = TMath::Max(visParticle.Et(), visParticle.pt()) + TMath::Sqrt(metPx*metPx + metPy*metPy);
    double mt2 = et*et - (px*px + py*py);
    if ( mt2 < 0 ) {
      edm::LogWarning ("compMt(2-body)") << " mt2 = " << mt2 << " must not be negative !!";
      return 0.;
    }
    return TMath::Sqrt(mt2);
  }

  std::pair<double, double> compZeta(const reco::Candidate::LorentzVector& leg1,
				     const reco::Candidate::LorentzVector& leg2,
				     double metPx, double metPy)
  {
    double leg1x = cos(leg1.phi());
    double leg1y = sin(leg1.phi());
    double leg2x = cos(leg2.phi());
    double leg2y = sin(leg2.phi());
    double zetaX = leg1x + leg2x;
    double zetaY = leg1y + leg2y;
    double zetaR = TMath::Sqrt(zetaX*zetaX + zetaY*zetaY);
    if ( zetaR > 0. ) {
      zetaX /= zetaR;
      zetaY /= zetaR;
    }

    double visPx = leg1.px() + leg2.px();
    double visPy = leg1.py() + leg2.py();
    double pZetaVis = visPx*zetaX + visPy*zetaY;

    double px = visPx + metPx;
    double py = visPy + metPy;
    double pZeta = px*zetaX + py*zetaY;

    return std::pair<double, double>(pZetaVis, pZeta);
  }
  //-----------------------------------------------------------------------------

  TFile* openFile(const edm::FileInPath& inputFileName)
  {
    if ( !inputFileName.isLocal() ) 
      throw cms::Exception("TauTauNtupleProducer") 
	<< " Failed to find file = " << inputFileName << " !!\n";
    TFile* inputFile = new TFile(inputFileName.fullPath().data());
    return inputFile;
  }

  TH1* loadLUT(TFile* inputFile, const std::string& lutName)
  {
    TH1* lut = dynamic_cast<TH1*>(inputFile->Get(lutName.data()));
    if ( !lut ) 
      throw cms::Exception("TauTauNtupleProducer") 
	<< " Failed to load LUT = " << lutName.data() << " from file = " << inputFile->GetName() << " !!\n";
    return lut;
  }

  double compHiggsPtWeight(const TH1* lut, double higgsPt)
  {
    int idxBin = (const_cast<TH1*>(lut))->FindBin(higgsPt);
    if ( idxBin < 1                ) idxBin = 1;
    if ( idxBin > lut->GetNbinsX() ) idxBin = lut->GetNbinsX();
    return lut->GetBinContent(idxBin);
  }

  //-----------------------------------------------------------------------------
  // CV: recipe for top quark Pt reweighting taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
  double compTopPtWeight(double topPt)
  {
    const double a = 0.156;
    const double b = -0.00137;
    return TMath::Exp(a + b*topPt);
  }

  double compTopPtWeight(double top1Pt, double top2Pt)
  {
    //std::cout << "<compTopPtWeight>:" << std::endl;
    double topPtWeight2 = compTopPtWeight(top1Pt)*compTopPtWeight(top2Pt);
    //std::cout << " top1Pt = " << top1Pt << ", top2Pt = " << top2Pt << ": topPtWeight2 = " << topPtWeight2 << std::endl;
    return ( topPtWeight2 > 0. ) ? TMath::Sqrt(topPtWeight2) : 0.;
  }
  //-----------------------------------------------------------------------------

  bool findGenParticle(const reco::GenParticleCollection& genParticles, int pdgId1, int pdgId2, int pdgId3, reco::Candidate::LorentzVector& p4)
  {
    //std::cout << "<findGenParticle>:" << std::endl;
    //std::cout << " pdgId1 = " << pdgId1 << std::endl;
    //std::cout << " pdgId2 = " << pdgId2 << std::endl;
    //std::cout << " pdgId3 = " << pdgId3 << std::endl;
    for ( reco::GenParticleCollection::const_iterator genParticle = genParticles.begin();
	  genParticle != genParticles.end(); ++genParticle ) {
      int genParticlePdgId = genParticle->pdgId();
      if ( (pdgId1 != 0 && genParticlePdgId == pdgId1) ||
	   (pdgId2 != 0 && genParticlePdgId == pdgId2) ||
	   (pdgId3 != 0 && genParticlePdgId == pdgId3) ) {
	p4 = genParticle->p4();
	//std::cout << "--> returning true." << std::endl;
	return true;
      }
    }
    //std::cout << "--> returning false." << std::endl;
    return false;
  }

  void matchGenParticle(const reco::Candidate::LorentzVector& p4, const std::vector<reco::GenParticle>& genParticles,
			bool& isGenHadTau, bool& isGenMuon, bool& isGenElectron, bool& isGenJet)
  {
    bool matchesGenHadTau   = false;
    bool matchesGenMuon     = false;
    bool matchesGenElectron = false;
    for ( std::vector<reco::GenParticle>::const_iterator genParticle = genParticles.begin();
	  genParticle != genParticles.end(); ++genParticle ) {
      int pdgId = TMath::Abs(genParticle->pdgId());
      if ( !(pdgId == 11 || pdgId == 13 || pdgId == 15) ) continue;
      reco::Candidate::LorentzVector visMomentum = getVisMomentum(&(*genParticle));     
      if ( !(visMomentum.pt() > (0.5*p4.pt())) ) continue;
      if ( deltaR(p4, visMomentum) < 0.3 ) {
	if ( pdgId == 15 ) {
	  std::string genTauDecayMode = getGenTauDecayMode(&(*genParticle));
	  if ( genTauDecayMode == "electron" ) {
	    matchesGenElectron = true;
	  } else if ( genTauDecayMode == "muon" ) {
	    matchesGenMuon = true;
	  } else {
	    matchesGenHadTau = true;
	  }
	} else if ( pdgId == 13 ) {
	  matchesGenMuon = true;
	} else if ( pdgId == 11 ) {
	  matchesGenElectron = true;
	} 
      }
    }
    if      ( matchesGenHadTau   ) isGenHadTau   = true;
    else if ( matchesGenMuon     ) isGenMuon     = true;
    else if ( matchesGenElectron ) isGenElectron = true;
    else                           isGenJet      = true;
  }

  void matchGenParticleFromZdecay(const reco::Candidate::LorentzVector& p4, const reco::GenParticle* genLeptonPlus, const reco::GenParticle* genLeptonMinus,
				  bool& isGenHadTau, bool& isGenMuon, bool& isGenElectron, bool& isGenJet)
  {
    std::vector<int> pdgIds_ranked;
    pdgIds_ranked.push_back(15);
    pdgIds_ranked.push_back(13);
    pdgIds_ranked.push_back(11);
    for ( std::vector<int>::const_iterator pdgId = pdgIds_ranked.begin();
	  pdgId != pdgIds_ranked.end(); ++pdgId ) {
      bool matchesGenLeptonPlus  = (genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == (*pdgId) && deltaR(p4, getVisMomentum(genLeptonPlus))  < 0.3);
      bool matchesGenLeptonMinus = (genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == (*pdgId) && deltaR(p4, getVisMomentum(genLeptonMinus)) < 0.3);
      if ( matchesGenLeptonPlus || matchesGenLeptonMinus ) {
	if ( (*pdgId) == 15 ) {
	  std::string genTauDecayModePlus  = ( matchesGenLeptonPlus )  ? getGenTauDecayMode(genLeptonPlus)  : "undefined";
	  std::string genTauDecayModeMinus = ( matchesGenLeptonMinus ) ? getGenTauDecayMode(genLeptonMinus) : "undefined";
	  if ( (matchesGenLeptonPlus  && genTauDecayModePlus  == "electron") || 
	       (matchesGenLeptonMinus && genTauDecayModeMinus == "electron") ) {
	    isGenElectron = true;
	  } 
	  if ( (matchesGenLeptonPlus  && genTauDecayModePlus  == "muon") || 
	       (matchesGenLeptonMinus && genTauDecayModeMinus == "muon") ) {
	    isGenMuon = true;
	  } 
	  if ( (matchesGenLeptonPlus  && !(genTauDecayModePlus  == "electron" || genTauDecayModePlus  == "muon")) || 
	       (matchesGenLeptonMinus && !(genTauDecayModeMinus == "electron" || genTauDecayModeMinus == "muon")) ) {
	    isGenHadTau = true;
	  } 
	} else if ( (*pdgId) == 13 ) {
	  isGenMuon = true;
	} else if ( (*pdgId) == 11 ) {
	  isGenElectron = true;
	} else assert(0);
	return;
      }
    }
    // CV: reconstructed tau neither matches generator level electron, muon nor tau
    isGenJet = true;
  }

  void printDiTau(const PATDiTauPair& diTau, const std::vector<std::string>& tauIdDiscrPreselection, const std::vector<std::string>& tauIdDiscrRanking)
  {
    std::cout << " leg1: Pt = " << diTau.leg1()->pt() << ", eta = " << diTau.leg1()->eta() << ", phi = " << diTau.leg1()->phi() << ", ";
    for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrPreselection.begin();
	  tauIdDiscr != tauIdDiscrPreselection.end(); ++tauIdDiscr ) {
      std::cout << (*tauIdDiscr) << " = " << diTau.leg1()->tauID(*tauIdDiscr) << ", ";
    }
    for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrRanking.begin();
	  tauIdDiscr != tauIdDiscrRanking.end(); ++tauIdDiscr ) {
      std::cout << (*tauIdDiscr) << " = " << diTau.leg1()->tauID(*tauIdDiscr) << ", ";
    }
    std::cout << std::endl;
    std::cout << " leg2: Pt = " << diTau.leg2()->pt() << ", eta = " << diTau.leg2()->eta() << ", phi = " << diTau.leg2()->phi() << ", ";
    for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrPreselection.begin();
	  tauIdDiscr != tauIdDiscrPreselection.end(); ++tauIdDiscr ) {
      std::cout << (*tauIdDiscr) << " = " << diTau.leg2()->tauID(*tauIdDiscr) << ", ";
    }
    for ( std::vector<std::string>::const_iterator tauIdDiscr = tauIdDiscrRanking.begin();
	  tauIdDiscr != tauIdDiscrRanking.end(); ++tauIdDiscr ) {
      std::cout << (*tauIdDiscr) << " = " << diTau.leg2()->tauID(*tauIdDiscr) << ", ";
    }
    std::cout << std::endl;
  }
}

void TauTauNtupleProducer::analyze(const edm::Event& evt, const edm::EventSetup& es) 
{
  //if ( verbosity_ >= 1 ) {
  //  std::cout << "<TauTauNtupleProducer::analyze>:" << std::endl;
  //  std::cout << " invertRanking = " << invertRanking_ << std::endl;
  //}

  assert(ntuple_);

  resetBranches();

  setValueUL("run" ,evt.run());
  setValueUL("event", evt.eventAuxiliary().event());
  setValueUL("lumi", evt.luminosityBlock());
  
  edm::Handle<PATDiTauPairCollection> diTaus;
  evt.getByLabel(srcDiTau_, diTaus);
  //if ( verbosity_ >= 1 ) {  
  //  std::cout << "#diTaus = " << diTaus->size() << std::endl;
  //}
  std::vector<const PATDiTauPair*> diTaus_sorted;
  int idx = 0;
  for ( PATDiTauPairCollection::const_iterator diTau = diTaus->begin();
	diTau != diTaus->end(); ++diTau ) {
    //if ( verbosity_ >= 1 ) {      
    //  std::cout << "diTau #" << idx << ":" << std::endl;
    //  printDiTau(*diTau, bestDiTauPreselection_, bestDiTauRanking_);
    //}
    diTaus_sorted.push_back(&(*diTau));
    ++idx;
  }
  std::sort(diTaus_sorted.begin(), diTaus_sorted.end(), isMoreIsolatedDiTau(bestDiTauPreselection_, bestDiTauRanking_, invertRanking_));
  if ( !(diTaus_sorted.size() >= 1) ) return;
  const PATDiTauPair* bestDiTau = diTaus_sorted.front();
  //if ( verbosity_ >= 1 ) {      
  //  std::cout << "best diTau:" << std::endl;
  //  printDiTau(*bestDiTau, bestDiTauPreselection_, bestDiTauRanking_);
  //}

  const pat::Tau* leg1 = &(*bestDiTau->leg1());
  const pat::Tau* leg2 = &(*bestDiTau->leg2());
  const reco::MET* met = &(*bestDiTau->met());

  edm::Handle<pat::MuonCollection> looseMuons;
  evt.getByLabel(srcMuons_, looseMuons);
  std::vector<const pat::Muon*> looseMuons_sorted;
  for ( pat::MuonCollection::const_iterator muon = looseMuons->begin();
	muon != looseMuons->end(); ++muon ) {
    looseMuons_sorted.push_back(&(*muon));
  }
  std::sort(looseMuons_sorted.begin(), looseMuons_sorted.end(), isHigherPtParticle());
  const pat::Muon* looseMuon1 = ( looseMuons_sorted.size() >= 1 ) ? looseMuons_sorted.at(0) : 0;

  edm::Handle<pat::ElectronCollection> looseElectrons;
  evt.getByLabel(srcElectrons_, looseElectrons);
  std::vector<const pat::Electron*> looseElectrons_sorted;
  for ( pat::ElectronCollection::const_iterator electron = looseElectrons->begin();
	electron != looseElectrons->end(); ++electron ) {
    looseElectrons_sorted.push_back(&(*electron));
  }
  std::sort(looseElectrons_sorted.begin(), looseElectrons_sorted.end(), isHigherPtParticle());
  const pat::Electron* looseElectron1 = ( looseElectrons_sorted.size() >= 1 ) ? looseElectrons_sorted.at(0) : 0;

  edm::Handle<l1extra::L1JetParticleCollection> l1Taus;
  evt.getByLabel(srcL1Taus_, l1Taus);
  edm::Handle<l1extra::L1JetParticleCollection> l1Jets;
  evt.getByLabel(srcL1Jets_, l1Jets);
  const l1extra::L1JetParticle* leg1L1 = matchL1Object(leg1->p4(), *l1Taus, *l1Jets);
  const l1extra::L1JetParticle* leg2L1 = matchL1Object(leg2->p4(), *l1Taus, *l1Jets);

  edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
  evt.getByLabel(srcTriggerObjects_, triggerObjects);

  bool leg1HLT_diTau_matched     = matchHLTFilter(leg1->p4(), *triggerObjects, hltTauFilters_diTau_, trigger::TriggerTau);
  bool leg1HLT_diTauJet_matched  = matchHLTFilter(leg1->p4(), *triggerObjects, hltTauFilters_diTauJet_, trigger::TriggerTau);
  bool leg1HLT_singleJet_matched = matchHLTFilter(leg1->p4(), *triggerObjects, hltJetFilters_singleJet_, trigger::TriggerJet);
  bool leg2HLT_diTau_matched     = matchHLTFilter(leg2->p4(), *triggerObjects, hltTauFilters_diTau_, trigger::TriggerTau);
  bool leg2HLT_diTauJet_matched  = matchHLTFilter(leg2->p4(), *triggerObjects, hltTauFilters_diTauJet_, trigger::TriggerTau);
  bool leg2HLT_singleJet_matched = matchHLTFilter(leg2->p4(), *triggerObjects, hltJetFilters_singleJet_, trigger::TriggerJet);

  setValue_diTau("diTau", *bestDiTau);

  std::pair<double, double> pZetaVis_and_pZeta = compZeta(leg1->p4(), leg2->p4(), met->px(), met->py());
  setValueF("pZetaVis", pZetaVis_and_pZeta.first);
  setValueF("pZeta", pZetaVis_and_pZeta.second);	

  if ( leg1L1 && leg2L1 ) {
    setValueF("dRttL1", deltaR(leg1L1->p4(), leg2L1->p4()));
  }

  if ( jetCorrLabel_ != "" ) {
    jec_ = JetCorrector::getJetCorrector(jetCorrLabel_, es);
  }

  setValue_Tau("l1", *leg1, evt, es);
  setValue_TauLT_base("l1", *leg1);
  setValue_TauLT_extended("l1", *leg1);    
  bool leg1isGenHadTau   = false;
  bool leg1isGenMuon     = false;
  bool leg1isGenElectron = false;
  bool leg1isGenJet      = false;    
  if ( isMC_ || isEmbedded_ ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(srcGenParticles_, genParticles);
    matchGenParticle(leg1->p4(), *genParticles, leg1isGenHadTau, leg1isGenMuon, leg1isGenElectron, leg1isGenJet);
  }
  setValueI("l1isGenHadTau", leg1isGenHadTau);
  setValueI("l1isGenElectron", leg1isGenElectron);
  setValueI("l1isGenMuon", leg1isGenMuon);
  setValueI("l1isGenJet", leg1isGenJet);
  double dRl1toLooseMuon = minDeltaR(leg1->p4(), *looseMuons);
  double l1LooseMu = ( dRl1toLooseMuon < 0.5 ) ? 0. : 1.;
  double dRl1toLooseElectron = minDeltaR(leg1->p4(), *looseElectrons);
  double l1LooseEle = ( dRl1toLooseElectron < 0.5 ) ? 0. : 1.;
  setValueF("l1LooseMu", l1LooseMu);
  setValueF("l1LooseEle", l1LooseEle);
  if ( leg1L1 ) {
    setValue_EnPxPyPz("l1L1", leg1L1->p4());
    setValue_PtEtaPhiMass("l1L1", leg1L1->p4());
  }
  setValueI("l1TrigMatched_diTau", leg1HLT_diTau_matched);
  setValueI("l1TrigMatched_diTauJet", leg1HLT_diTauJet_matched);
  setValueI("l1TrigMatched_singleJet", leg1HLT_singleJet_matched);
  double mt1 = compMt(leg1->p4(), met->px(), met->py());
  setValueF("mt1", mt1);

  setValue_Tau("l2", *leg2, evt, es);
  setValue_TauLT_base("l2", *leg2);
  setValue_TauLT_extended("l2", *leg2);
  bool leg2isGenHadTau   = false;
  bool leg2isGenMuon     = false;
  bool leg2isGenElectron = false;
  bool leg2isGenJet      = false;    
  if ( isMC_ || isEmbedded_ ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(srcGenParticles_, genParticles);
    matchGenParticle(leg2->p4(), *genParticles, leg2isGenHadTau, leg2isGenMuon, leg2isGenElectron, leg2isGenJet);
  }
  setValueI("l2isGenHadTau", leg2isGenHadTau);
  setValueI("l2isGenMuon", leg2isGenMuon);
  setValueI("l2isGenElectron", leg2isGenElectron);
  setValueI("l2isGenJet", leg2isGenJet);
  double dRl2toLooseMuon = minDeltaR(leg2->p4(), *looseMuons);
  double l2LooseMu = ( dRl2toLooseMuon < 0.5 ) ? 0. : 1.;
  double dRl2toLooseElectron = minDeltaR(leg2->p4(), *looseElectrons);
  double l2LooseEle = ( dRl2toLooseElectron < 0.5 ) ? 0. : 1.;
  setValueF("l2LooseMu", l2LooseMu);
  setValueF("l2LooseEle", l2LooseEle);
  if ( leg2L1 ) {
    setValue_EnPxPyPz("l2L1", leg2L1->p4());
    setValue_PtEtaPhiMass("l2L1", leg2L1->p4());
  }
  setValueI("l2TrigMatched_diTau", leg2HLT_diTau_matched);
  setValueI("l2TrigMatched_diTauJet", leg2HLT_diTauJet_matched); 
  setValueI("l2TrigMatched_singleJet", leg2HLT_singleJet_matched); 
  double mt2 = compMt(leg2->p4(), met->px(), met->py());
  setValueF("mt2", mt2);

  edm::Handle<pat::JetCollection> jets;
  evt.getByLabel(srcJets_, jets);
  typedef edm::ValueMap<int> puJetIdMap;
  edm::Handle<puJetIdMap> puJetIdFlags;
  evt.getByLabel(srcPileupJetId_, puJetIdFlags);
  typedef edm::ValueMap<float> puJetIdMap_mva;
  edm::Handle<puJetIdMap_mva> puJetIdMVAs;
  evt.getByLabel(srcPileupJetIdMVA_, puJetIdMVAs);
  std::vector<const pat::Jet*> jets_sorted;
  std::vector<const pat::Jet*> bJets_sorted;
  for ( pat::JetCollection::const_iterator jet = jets->begin();
	jet != jets->end(); ++jet ) {
    bool passesLoosePFJetId = (*loosePFJetIdAlgo_)(*jet);
    if ( !passesLoosePFJetId ) continue;

    int puJetIdFlag = (*puJetIdFlags)[jet->originalObjectRef()];
    bool passesPileupJetId = PileupJetIdentifier::passJetId(puJetIdFlag, wpPileupJetId_);
    if ( !passesPileupJetId ) continue;

    // CV: remove jets overlapping with tau decay products
    double dRleg1Jet = deltaR(leg1->p4(), jet->p4());
    double dRleg2Jet = deltaR(leg2->p4(), jet->p4());
    if ( dRleg1Jet < 0.5 || dRleg2Jet < 0.5 ) continue;

    double jetPt = jet->pt();
    double jetAbsEta = TMath::Abs(jet->eta());
    if ( jetPt > minJetPt_ && jetAbsEta < maxJetAbsEta_ ) {
      jets_sorted.push_back(&(*jet));
    }
    if ( jetPt > minBJetPt_ && jetAbsEta < maxBJetAbsEta_ && jet->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_ ) {      
      bJets_sorted.push_back(&(*jet));
    }
  }
  std::sort(jets_sorted.begin(), jets_sorted.end(), isHigherPtParticle());
  //std::sort(bJets_sorted.begin(), bJets_sorted.end(), isHigherPtParticle());
  std::sort(bJets_sorted.begin(), bJets_sorted.end(), isHigherBtagDiscriminator(bJetDiscriminator_));

  if ( jetCorrPayload_ != "" ) {
    edm::ESHandle<JetCorrectorParametersCollection> jetCorrParameters;
    es.get<JetCorrectionsRecord>().get(jetCorrPayload_, jetCorrParameters); 
    const JetCorrectorParameters& jecUncertaintyParameters = (*jetCorrParameters)[jecUncertaintyTag_];
    delete jecUncertainty_;
    jecUncertainty_ = new JetCorrectionUncertainty(jecUncertaintyParameters);
  }

  const pat::Jet* jet1 = ( jets_sorted.size() >= 1 ) ? jets_sorted.at(0) : 0;
  const pat::Jet* jet2 = ( jets_sorted.size() >= 2 ) ? jets_sorted.at(1) : 0;
  const pat::Jet* jet3 = ( jets_sorted.size() >= 3 ) ? jets_sorted.at(2) : 0;
  const pat::Jet* jet4 = ( jets_sorted.size() >= 4 ) ? jets_sorted.at(3) : 0;
  if ( jet1 ) {
    setValue_Jet("jet1", *jet1);  
    int puJetIdFlag = (*puJetIdFlags)[jet1->originalObjectRef()];
    setValueF("jet1PUJetId", puJetIdFlag);
    double puJetIdMVA = (*puJetIdMVAs)[jet1->originalObjectRef()];
    setValueF("jet1PUJetIdMVA", puJetIdMVA);
    jecUncertainty_->setJetEta(jet1->eta());
    jecUncertainty_->setJetPt(jet1->pt());
    double shift = jecUncertainty_->getUncertainty(true);
    setValueF("jet1PtErr", shift*jet1->pt());  
    setValueF("jet1Bmatch", jet1->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_);
  }
  if ( jet2 ) {
    setValue_Jet("jet2", *jet2);
    int puJetIdFlag = (*puJetIdFlags)[jet2->originalObjectRef()];
    setValueF("jet2PUJetId", puJetIdFlag);
    double puJetIdMVA = (*puJetIdMVAs)[jet2->originalObjectRef()];
    setValueF("jet2PUJetIdMVA", puJetIdMVA);
    jecUncertainty_->setJetEta(jet2->eta());
    jecUncertainty_->setJetPt(jet2->pt());
    double shift = jecUncertainty_->getUncertainty(true);
    setValueF("jet2PtErr", shift*jet2->pt());  
    setValueF("jet2Bmatch", jet2->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_);
  }
  if ( jet3 ) {
    setValue_Jet("jet3", *jet3);  
    int puJetIdFlag = (*puJetIdFlags)[jet3->originalObjectRef()];
    setValueF("jet3PUJetId", puJetIdFlag);
    double puJetIdMVA = (*puJetIdMVAs)[jet3->originalObjectRef()];
    setValueF("jet3PUJetIdMVA", puJetIdMVA);
    jecUncertainty_->setJetEta(jet3->eta());
    jecUncertainty_->setJetPt(jet3->pt());
    double shift = jecUncertainty_->getUncertainty(true);
    setValueF("jet3PtErr", shift*jet3->pt());  
    setValueF("jet3Bmatch", jet3->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_);
  }
  if ( jet4 ) {
    setValue_Jet("jet4", *jet4);
    int puJetIdFlag = (*puJetIdFlags)[jet4->originalObjectRef()];
    setValueF("jet4PUJetId", puJetIdFlag);
    double puJetIdMVA = (*puJetIdMVAs)[jet4->originalObjectRef()];
    setValueF("jet4PUJetIdMVA", puJetIdMVA);
    jecUncertainty_->setJetEta(jet4->eta());
    jecUncertainty_->setJetPt(jet4->pt());
    double shift = jecUncertainty_->getUncertainty(true);
    setValueF("jet4PtErr", shift*jet4->pt());  
    setValueF("jet4Bmatch", jet4->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_);
  }
  int nCentralJets = 0;
  if ( jet1 && jet2 ) {
    double etaMin = TMath::Min(jet1->eta(), jet2->eta());
    double etaMax = TMath::Max(jet1->eta(), jet2->eta());
    for ( std::vector<const pat::Jet*>::const_iterator jet = jets_sorted.begin();
	  jet != jets_sorted.end(); ++jet ) {
      double jetEta = (*jet)->eta();
      const double etaGap = 1.e-3;
      if ( jetEta > (etaMin + etaGap) && jetEta < (etaMax - etaGap) ) {
	++nCentralJets;
      }
    }
  }
  setValueI("nCentralJets", nCentralJets);
  setValueI("nJets", jets_sorted.size());
  if ( jet1 && jet2 ) {
    setValueF("dPhijj", reco::deltaPhi(jet1->phi(), jet2->phi()));
    setValueF("dEtajj", TMath::Abs(jet1->eta() - jet2->eta()));
    setValueF("mjj", (jet1->p4() + jet2->p4()).mass());
    setValueF("mttj", (leg1->p4() + leg2->p4() + jet1->p4()).mass());
  }

  bool jet1HLT_diTauJet_matched = ( jet1 ) ? matchHLTFilter(jet1->p4(), *triggerObjects, hltJetFilters_diTauJet_, trigger::TriggerJet) : false;
  setValueI("jetTrigMatched_diTauJet", jet1HLT_diTauJet_matched);

  const pat::Jet* bJet1 = ( bJets_sorted.size() >= 1 ) ? bJets_sorted.at(0) : 0;
  const pat::Jet* bJet2 = ( bJets_sorted.size() >= 2 ) ? bJets_sorted.at(1) : 0;
  const pat::Jet* bJet3 = ( bJets_sorted.size() >= 3 ) ? bJets_sorted.at(2) : 0;
  const pat::Jet* bJet4 = ( bJets_sorted.size() >= 4 ) ? bJets_sorted.at(3) : 0;
  if ( bJet1 ) {
    setValue_bJet("bjet1", *bJet1);
  }
  if ( bJet2 ) {
    setValue_bJet("bjet2", *bJet2);
  }
  if ( bJet3 ) {
    setValue_bJet("bjet3", *bJet3);
  }
  if ( bJet4 ) {
    setValue_bJet("bjet4", *bJet4);
  }
  setValueI("nbJets", bJets_sorted.size());
  if ( bJet1 && bJet2 ) {
    reco::Candidate::LorentzVector bbP4 = bJet1->p4() + bJet2->p4(); // CV: consider doing something more sophisticated like b-jet energy regression in the future...
    double mbb = bbP4.mass();
    double mbbReg2 = mbb*mbb*bJet1->userFloat("bJetEnReg")*bJet2->userFloat("bJetEnReg");
    if ( mbbReg2 < 0. ) mbbReg2 = 0.;
    double mbbReg = TMath::Sqrt(mbbReg2);
    //std::cout << "mbb: without energy regression = " << mbb << ", with regression = " << mbbReg << std::endl;
    setValueF("mbb", mbb);
    setValueF("mbbReg", mbbReg);

    double ptbb = bbP4.pt();
    //std::cout << "ptbb = " << ptbb << std::endl;
    setValueF("ptbb", ptbb);
  }

  setValueF("met", met->pt());
  setValueF("mex", met->px());
  setValueF("mey", met->py());
  setValueF("metPhi", met->phi());
  setValue_Cov2d("mvacov", met->getSignificanceMatrix());

  typedef edm::View<reco::MET> METView;
  edm::Handle<METView> rawMEt;
  evt.getByLabel(srcRawPFMEt_, rawMEt);  
  setValueF("rawMET", rawMEt->front().pt());
  setValueF("rawMEx", rawMEt->front().px());
  setValueF("rawMEy", rawMEt->front().py());

  if ( srcGenMEt_.label() != "" ) {
    edm::Handle<METView> genMEt;
    evt.getByLabel(srcGenMEt_, genMEt);
    setValueF("genMET", genMEt->front().pt());
    setValueF("genMEx", genMEt->front().px());
    setValueF("genMEy", genMEt->front().py());
  }

  if ( leg1 && leg2 && met && bJet1 && bJet2 ) {
    double augMT2ed = (*augMT2edAlgo_)(bJet1->p4(), bJet2->p4(), leg1->p4(), leg2->p4(), met->p4());
    //std::cout << "augMT2ed = " << augMT2ed << std::endl;
    setValueF("augMT2ed", augMT2ed);
  }

  if ( looseElectron1 ) {
    setValue_Electron("electron1", *looseElectron1);
  }

  if ( looseMuon1 ) {
    setValue_Muon("muon1", *looseMuon1);
  }

  edm::Handle<reco::VertexCollection> vertices;
  evt.getByLabel(srcVertices_, vertices);
  setValueI("nVert", vertices->size());
  if ( vertices->size() >= 1 ) {
    setValue_XYZ("vertex", vertices->front().position());
    setValueF("vertexRho", vertices->front().position().Rho());
    setValueF("vertexNDoF", vertices->front().ndof());
    setValueI("vertexIsValid", vertices->front().isValid());
  }

  edm::Handle<double> rho;
  evt.getByLabel(srcRho_, rho);
  setValueF("rho", *rho);

  if ( isMC_ ) {
    typedef std::vector<PileupSummaryInfo> PileupSummaryInfoCollection;
    edm::Handle<PileupSummaryInfoCollection> genPileUpInfos;
    evt.getByLabel(srcGenPileUpSummary_, genPileUpInfos);
    int nPUbxM1 = -1;
    int nPUbx0  = -1;
    int nPUbxP1 = -1;
    for ( PileupSummaryInfoCollection::const_iterator genPileUpInfo = genPileUpInfos->begin();
	  genPileUpInfo != genPileUpInfos->end(); ++genPileUpInfo ) {
      // CV: in-time PU is stored in getBunchCrossing = 0, 
      //    cf. https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupInformation
      int bx = genPileUpInfo->getBunchCrossing();
      int nPU = genPileUpInfo->getPU_NumInteractions();
      if      ( bx == -1 ) nPUbxM1 = nPU;
      else if ( bx ==  0 ) nPUbx0  = nPU;
      else if ( bx == +1 ) nPUbxP1 = nPU;
    }    
    setValueI("nPUbxM1", nPUbxM1);
    setValueI("nPUbx0",  nPUbx0);
    setValueI("nPUbxP1", nPUbxP1);

    edm::Handle<LHEEventProduct> LHE;
    evt.getByLabel(srcLHE_, LHE);
    if ( LHE.isValid() ) {
      setValueF("NUP", LHE->hepeup().NUP);
    } 
  }
  
  if ( isMC_ || isEmbedded_ ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(srcGenParticles_, genParticles);

    bool isZtt   = false;
    bool isZttj  = false;
    bool isZttl  = false;
    bool isZj    = false;
    bool isZee   = false;
    bool isZmm   = false;
    const reco::GenParticle* genZ_or_Gammastar = 0;
    for ( reco::GenParticleCollection::const_iterator genParticle = genParticles->begin();
	  genParticle != genParticles->end(); ++genParticle ) {
      if ( (genParticle->pdgId() == 22 || genParticle->pdgId() == 23) && genParticle->mass() > 50. ) { // CV: Z/gamma* samples generated with MadGraph start @ M(ll) > 50 GeV
	genZ_or_Gammastar = &(*genParticle);
	break;
      }
    }
    //std::cout << "genZ_or_Gammastar = " << genZ_or_Gammastar << ": pdgId = ";
    //if ( genZ_or_Gammastar ) std::cout << genZ_or_Gammastar->pdgId();
    //else std::cout << "N/A";
    //std::cout << std::endl;
    std::vector<const reco::GenParticle*> allDaughters;  
    bool isZdecay = false;
    if ( genZ_or_Gammastar ) {
      findDaughters(genZ_or_Gammastar, allDaughters, -1);      
      isZdecay = true;
    } else {
      for ( reco::GenParticleCollection::const_iterator genParticle = genParticles->begin();
	    genParticle != genParticles->end(); ++genParticle ) {
	allDaughters.push_back(&(*genParticle));
      }
    }
    //std::cout << "isZdecay = " << isZdecay << std::endl;
    const reco::GenParticle* genLeptonPlus  = 0;
    const reco::GenParticle* genLeptonMinus = 0;
    for ( std::vector<const reco::GenParticle*>::const_iterator daughter = allDaughters.begin();
	  daughter != allDaughters.end(); ++daughter ) {
      int absPdgId = TMath::Abs((*daughter)->pdgId());
      // CV: give preference to taus, since electrons and muons may either come from decay of Z-boson or from tau decay
      if ( absPdgId == 15 && (*daughter)->charge() > +0.5 && (isZdecay || !genLeptonPlus)  ) genLeptonPlus  = (*daughter);
      if ( absPdgId == 15 && (*daughter)->charge() < -0.5 && (isZdecay || !genLeptonMinus) ) genLeptonMinus = (*daughter);
      if ( (absPdgId == 11 || absPdgId == 13) && (*daughter)->charge() > +0.5 && !genLeptonPlus  ) genLeptonPlus  = (*daughter);
      if ( (absPdgId == 11 || absPdgId == 13) && (*daughter)->charge() < -0.5 && !genLeptonMinus ) genLeptonMinus = (*daughter);
    }      
    //std::cout << "genLeptonPlus = " << genLeptonPlus << ": pdgId = ";
    //if ( genLeptonPlus ) std::cout << genLeptonPlus->pdgId();
    //else std::cout << "N/A";
    //std::cout << std::endl;
    //std::cout << "genLeptonMinus = " << genLeptonMinus << ": pdgId = ";
    //if ( genLeptonMinus ) std::cout << genLeptonMinus->pdgId();
    //else std::cout << "N/A";
    //std::cout << std::endl;
    double genMass = -1.;
    if ( genLeptonPlus && genLeptonMinus ) {
      genMass = (genLeptonPlus->p4() + genLeptonMinus->p4()).mass();
    }
    setValueF("genMass", genMass);
    bool leg1isGenHadTau   = false; 
    bool leg1isGenMuon     = false;
    bool leg1isGenElectron = false;
    bool leg1isGenJet      = false;    
    matchGenParticleFromZdecay(leg1->p4(), genLeptonPlus, genLeptonMinus, leg1isGenHadTau, leg1isGenMuon, leg1isGenElectron, leg1isGenJet);
    bool leg2isGenHadTau   = false;
    bool leg2isGenMuon     = false;
    bool leg2isGenElectron = false;
    bool leg2isGenJet      = false;    
    matchGenParticleFromZdecay(leg2->p4(), genLeptonPlus, genLeptonMinus, leg2isGenHadTau, leg2isGenMuon, leg2isGenElectron, leg2isGenJet);
    if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 15 && 
	 genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 15 ) {
      if      ( leg1isGenHadTau && leg2isGenHadTau ) isZtt = true;
      else if ( leg1isGenElectron || leg1isGenMuon || leg2isGenElectron || leg2isGenMuon ) isZttl = true;
      else isZttj = true;
    } else if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 11 && leg1isGenElectron &&
		genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 11 && leg2isGenElectron ) {
      isZee = true;
    } else if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 13 && leg1isGenMuon &&
		genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 13 && leg2isGenMuon ) {
      isZmm = true;
    } else {
      isZj = true;
    }
    setValueI("isZtt", isZtt);
    setValueI("isZttj", isZttj);
    setValueI("isZttl", isZttl);
    setValueI("isZj", isZj);
    setValueI("isZee", isZee);
    setValueI("isZmm", isZmm);    

    reco::Candidate::LorentzVector zP4;
    bool hasZ = findGenParticle(*genParticles, 22, 23, 0, zP4);
    setValueI("hasZ", hasZ && zP4.mass() > 50.);
    reco::Candidate::LorentzVector wP4;
    bool hasW = findGenParticle(*genParticles, 24,  0, 0, wP4);
    setValueI("hasW", hasW && wP4.mass() > 50.);

    bool is1RealTau  = leg1->genJet() || leg2->genJet();
    bool is2RealTaus = leg1->genJet() && leg2->genJet();
    bool isFake      = !is2RealTaus;
    bool isElectron  = (leg1->genLepton() && TMath::Abs(leg1->genLepton()->pdgId()) == 11) || (leg2->genLepton() && TMath::Abs(leg2->genLepton()->pdgId()) == 11);
    bool isMuon      = (leg1->genLepton() && TMath::Abs(leg1->genLepton()->pdgId()) == 13) || (leg2->genLepton() && TMath::Abs(leg2->genLepton()->pdgId()) == 13);
    bool isJet       = !(is2RealTaus || isElectron || isMuon);
    bool isPhoton    = genZ_or_Gammastar && (genZ_or_Gammastar->pdgId() == 22 || genZ_or_Gammastar->mass() < 75. || genZ_or_Gammastar->mass() > 105.);
    setValueI("is1RealTau", is1RealTau);
    setValueI("is2RealTaus", is2RealTaus);
    setValueI("isFake", isFake);
    setValueI("isElectron", isElectron);
    setValueI("isMuon", isMuon);
    setValueI("isJet", isJet);
    setValueI("isPhoton", isPhoton);
  }

  for ( std::vector<InputTagEntryType>::const_iterator evtWeight = evtWeightsToStore_.begin();
	evtWeight != evtWeightsToStore_.end(); ++evtWeight ) {
    edm::Handle<double> evtWeightValue;
    evt.getByLabel(evtWeight->src_, evtWeightValue);
    setValueF(evtWeight->branchName_, *evtWeightValue);
  }

  double genHiggsPt        = -1.;
  double genHiggsMass      = -1.;  
  double higgsPtWeightNom  =  1.;
  double higgsPtWeightUp   =  1.;
  double higgsPtWeightDown =  1.;
  bool isHiggs = false;
  if ( isMC_ ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(srcGenParticles_, genParticles);

    reco::Candidate::LorentzVector higgsP4;
    isHiggs = findGenParticle(*genParticles, 25, 35, 36, higgsP4);
    if ( isHiggs ) {
      genHiggsPt = higgsP4.pt();
      genHiggsMass = higgsP4.mass();
      //std::cout << "genHiggsMass = " << genHiggsMass << std::endl;
      std::string inputFileName;
      if      ( genHiggsMass >=  90. && genHiggsMass <= 110. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH100_8TeV.root";
      else if ( genHiggsMass >= 115. && genHiggsMass <= 135. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH125_8TeV.root";
      else if ( genHiggsMass >= 140. && genHiggsMass <= 150. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH150_8TeV.root";
      //std::cout << "inputFileName = " << inputFileName << std::endl;
      if ( inputFileName != "" ) {
	if ( inputFileName != lastInputFileHiggsPtWeight_ || !inputFileHiggsPtWeight_ ) {
	  inputFileHiggsPtWeight_ = openFile(edm::FileInPath(inputFileName));
	  lutHiggsPtWeightNom_  = loadLUT(inputFileHiggsPtWeight_, "Nominal");
	  lutHiggsPtWeightUp_   = loadLUT(inputFileHiggsPtWeight_, "Up");
	  lutHiggsPtWeightDown_ = loadLUT(inputFileHiggsPtWeight_, "Down");
	  lastInputFileHiggsPtWeight_ = inputFileName;
	}
	higgsPtWeightNom = compHiggsPtWeight(lutHiggsPtWeightNom_, genHiggsPt);
	higgsPtWeightUp = compHiggsPtWeight(lutHiggsPtWeightUp_, genHiggsPt);
	higgsPtWeightDown = compHiggsPtWeight(lutHiggsPtWeightDown_, genHiggsPt);
	//std::cout << "higgsPtWeight: Nom = " << higgsPtWeightNom << ", Up = " << higgsPtWeightUp << ", Down = " << higgsPtWeightDown << std::endl;
      }     
    }
  }
  setValueF("genHiggsPt", genHiggsPt);
  setValueF("genHiggsMass", genHiggsMass);
  setValueF("higgsPtWeightNom", higgsPtWeightNom);
  setValueF("higgsPtWeightUp", higgsPtWeightUp);
  setValueF("higgsPtWeightDown", higgsPtWeightDown);

  double genTopPlusPt = -1.;
  double genTopMinusPt = -1.;
  double topPtWeightNom  = 1.;
  double topPtWeightUp   = 1.;
  double topPtWeightDown = 1.;
  if ( isMC_ ) {
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(srcGenParticlesForTopPtReweighting_, genParticles);

    reco::Candidate::LorentzVector topPlusP4;
    bool topPlusFound = findGenParticle(*genParticles, +6, 0, 0, topPlusP4);
    reco::Candidate::LorentzVector topMinusP4;
    bool topMinusFound = findGenParticle(*genParticles, -6, 0, 0, topMinusP4);
    if ( topPlusFound && topMinusFound ) {
      genTopPlusPt = topPlusP4.pt();
      genTopMinusPt = topMinusP4.pt();
      topPtWeightNom = compTopPtWeight(genTopPlusPt, genTopMinusPt);
      topPtWeightUp = topPtWeightNom*topPtWeightNom; // CV: assign 100% systematic uncertainty to top quark Pt reweighting,
      topPtWeightDown = 1.0;                         //     following https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
    }
  }
  setValueF("genTopPlusPt", genTopPlusPt);
  setValueF("genTopMinusPt", genTopMinusPt);
  setValueF("topPtWeightNom", topPtWeightNom);
  setValueF("topPtWeightUp", topPtWeightUp);
  setValueF("topPtWeightDown", topPtWeightDown);

  double genFilter = 1.0; // CV: weight associated to visible Pt cuts applied on gen. tau decay products in Embedded sample production
  if ( isEmbedded_ ) {
    edm::Handle<GenFilterInfo> embeddingWeight;
    evt.getByLabel(srcEmbeddingWeight_, embeddingWeight);
    genFilter = embeddingWeight->filterEfficiency();
  }
  setValueF("genFilter", genFilter);
  
  edm::Handle<pat::TriggerEvent> triggerResults;
  evt.getByLabel(srcTriggerResults_, triggerResults);

  double leg1triggerEffMC_diTau      = eff2012IsoParkedTau19fbMC_Simone(leg1->pt(), leg1->eta());
  double leg1triggerEffData_diTau    = eff2012IsoParkedTau19fb_Simone(leg1->pt(), leg1->eta());
  double leg2triggerEffMC_diTau      = eff2012IsoParkedTau19fbMC_Simone(leg2->pt(), leg2->eta());
  double leg2triggerEffData_diTau    = eff2012IsoParkedTau19fb_Simone(leg2->pt(), leg2->eta());
  double triggerEffMC_diTau          = leg1triggerEffMC_diTau*leg2triggerEffMC_diTau;
  double triggerEffData_diTau        = leg1triggerEffData_diTau*leg2triggerEffData_diTau;

  double leg1triggerEffMC_diTauJet   = eff2012IsoTau19fbMC_Simone(leg1->pt(), leg1->eta());
  double leg1triggerEffData_diTauJet = eff2012IsoTau19fb_Simone(leg1->pt(), leg1->eta());
  double leg2triggerEffMC_diTauJet   = eff2012IsoTau19fbMC_Simone(leg2->pt(), leg2->eta());
  double leg2triggerEffData_diTauJet = eff2012IsoTau19fb_Simone(leg2->pt(), leg2->eta());
  double jetTriggerEffMC_diTauJet    = ( jet1 ) ? eff2012Jet19fb(jet1->pt(), jet1->eta()) : 0.;
  double jetTriggerEffData_diTauJet  = jetTriggerEffMC_diTauJet; // CV: same jet trigger efficiency turn-on used for data and MC
  double triggerEffMC_diTauJet       = leg1triggerEffMC_diTauJet*leg2triggerEffMC_diTauJet*jetTriggerEffMC_diTauJet;
  double triggerEffData_diTauJet     = leg1triggerEffData_diTauJet*leg2triggerEffData_diTauJet*jetTriggerEffData_diTauJet;

  bool isTriggered_diTau = false;
  for ( std::vector<StringEntryType>::const_iterator hltPathName = hltPaths_diTau_.begin();
	hltPathName != hltPaths_diTau_.end(); ++hltPathName ) {
    const pat::TriggerPath* hltPath = triggerResults->path(hltPathName->src_);
    if ( hltPath && hltPath->wasRun() && hltPath->wasAccept() ) {
      isTriggered_diTau = true;
      break;
    }
  }
  bool isTriggered_diTauJet = false;
  for ( std::vector<StringEntryType>::const_iterator hltPathName = hltPaths_diTauJet_.begin();
	hltPathName != hltPaths_diTauJet_.end(); ++hltPathName ) {
    const pat::TriggerPath* hltPath = triggerResults->path(hltPathName->src_);
    if ( hltPath && hltPath->wasRun() && hltPath->wasAccept() ) {
      isTriggered_diTauJet = true;
      break;
    }
  }
  setValueF("leg1triggerEffMC_diTau", leg1triggerEffMC_diTau);
  setValueF("leg1triggerEffData_diTau", leg1triggerEffData_diTau);
  setValueF("leg2triggerEffMC_diTau", leg2triggerEffMC_diTau);
  setValueF("leg2triggerEffData_diTau", leg2triggerEffData_diTau);
  setValueF("triggerEffMC_diTau", triggerEffMC_diTau);
  setValueF("triggerEffData_diTau", triggerEffData_diTau);
  setValueF("leg1triggerEffMC_diTauJet", leg1triggerEffMC_diTauJet);
  setValueF("leg1triggerEffData_diTauJet", leg1triggerEffData_diTauJet);
  setValueF("leg2triggerEffMC_diTauJet", leg2triggerEffMC_diTauJet);
  setValueF("leg2triggerEffData_diTauJet", leg2triggerEffData_diTauJet);
  setValueF("jetTriggerEffMC_diTauJet", jetTriggerEffMC_diTauJet);
  setValueF("jetTriggerEffData_diTauJet", jetTriggerEffData_diTauJet);
  setValueF("triggerEffMC_diTauJet", triggerEffMC_diTauJet); 
  setValueF("triggerEffData_diTauJet", triggerEffData_diTauJet);
  double triggerEffMC   = 0.;
  double triggerEffData = 0.;
  if ( isTriggered_diTau ) {
    triggerEffMC   = triggerEffMC_diTau;
    triggerEffData = triggerEffData_diTau;
  } else if ( isTriggered_diTauJet ) {
    triggerEffMC   = triggerEffMC_diTauJet;
    triggerEffData = triggerEffData_diTauJet;
  }
  setValueF("triggerEffMC", triggerEffMC);
  setValueF("triggerEffData", triggerEffData);
  double leg1triggerWeight_diTau = ( leg1triggerEffMC_diTau > 0. ) ? (leg1triggerEffData_diTau/leg1triggerEffMC_diTau) : 1.;
  double leg2triggerWeight_diTau = ( leg2triggerEffMC_diTau > 0. ) ? (leg2triggerEffData_diTau/leg2triggerEffMC_diTau) : 1.;
  double triggerWeight_diTau = leg1triggerWeight_diTau*leg2triggerWeight_diTau;
  double leg1triggerWeight_diTauJet = ( leg1triggerEffMC_diTauJet > 0. ) ? (leg1triggerEffData_diTauJet/leg1triggerEffMC_diTauJet) : 1.;
  double leg2triggerWeight_diTauJet = ( leg2triggerEffMC_diTauJet > 0. ) ? (leg2triggerEffData_diTauJet/leg2triggerEffMC_diTauJet) : 1.;
  double jetTriggerWeight_diTauJet  = ( jetTriggerEffMC_diTauJet  > 0. ) ? (jetTriggerEffData_diTauJet/jetTriggerEffMC_diTauJet)   : 1.;
  double triggerWeight_diTauJet = leg1triggerWeight_diTauJet*leg2triggerWeight_diTauJet*jetTriggerWeight_diTauJet;
  setValueF("leg1triggerWeight_diTau", leg1triggerWeight_diTau);
  setValueF("leg2triggerWeight_diTau", leg2triggerWeight_diTau);
  setValueF("triggerWeight_diTau", triggerWeight_diTau);
  setValueF("leg1triggerWeight_diTauJet", leg1triggerWeight_diTauJet);
  setValueF("leg2triggerWeight_diTauJet", leg2triggerWeight_diTauJet);
  setValueF("triggerWeight_diTauJet", triggerWeight_diTauJet);
  double triggerWeight = ( triggerEffMC > 0. ) ? (triggerEffData/triggerEffMC) : 1.;
  setValueF("triggerWeight", triggerWeight);
  
  for ( std::vector<StringEntryType>::const_iterator hltPathEntry = hltPathsToStore_.begin();
	hltPathEntry != hltPathsToStore_.end(); ++hltPathEntry ) {
    const pat::TriggerPath* hltPath = triggerResults->path(hltPathEntry->src_);
    bool hltPath_passed = (hltPath && hltPath->wasRun() && hltPath->wasAccept());
    setValueF(hltPathEntry->branchName_, hltPath_passed);
  }

  double weight = 1.0; // CV: not implemented yet; compute weight on analysis level
  setValueF("weight", weight);

//--- fill all computed quantities into TTree
  ntuple_->Fill();
}

void TauTauNtupleProducer::addBranchF(const std::string& name) 
{
  if ( branches_.count(name) != 0 )
    throw cms::Exception("TauTauNtupleProducer") 
      << " Branch with name = " << name << " already exists !!\n";
  std::string name_and_format = name + "/F";
  ntuple_->Branch(name.c_str(), &branches_[name].valueF_, name_and_format.c_str());
}

void TauTauNtupleProducer::addBranchI(const std::string& name) 
{
  if ( branches_.count(name) != 0 )
    throw cms::Exception("TauTauNtupleProducer") 
      << " Branch with name = " << name << " already exists !!\n";
  std::string name_and_format = name + "/I";
  ntuple_->Branch(name.c_str(), &branches_[name].valueI_, name_and_format.c_str());
}

void TauTauNtupleProducer::addBranchUL(const std::string& name) 
{
  if ( branches_.count(name) != 0 ) 
    throw cms::Exception("TauTauNtupleProducer") 
      << " Branch with name = " << name << " already exists !!\n";
  std::string name_and_format = name + "/l";
  ntuple_->Branch(name.c_str(), &branches_[name].valueUL_, name_and_format.c_str());
}

void TauTauNtupleProducer::resetBranches()
{
  for ( branchMap::iterator branch = branches_.begin();
	branch != branches_.end(); ++branch ) {
    branch->second.valueF_  = 0.;
    branch->second.valueI_  = 0;
    branch->second.valueUL_ = 0;
  }
}

void TauTauNtupleProducer::printBranches(std::ostream& stream)
{
  stream << "<TauTauNtupleProducer::printBranches>:" << std::endl;
  stream << " registered branches for module = " << moduleLabel_ << std::endl;
  for ( branchMap::const_iterator branch = branches_.begin();
	branch != branches_.end(); ++branch ) {
    stream << " " << branch->first << std::endl;
  }
  stream << std::endl;
}

void TauTauNtupleProducer::setValueF(const std::string& name, double value) 
{
  if ( verbosity_ >= 1 ) std::cout << "branch = " << name << ": value = " << value << std::endl;
  branchMap::iterator branch = branches_.find(name);
  if ( branch != branches_.end() ) {
    branch->second.valueF_ = value;
  } else {
    throw cms::Exception("InvalidParameter") 
      << "No branch with name = " << name << " defined !!\n";
  }
}

void TauTauNtupleProducer::setValueI(const std::string& name, int value) 
{
  if ( verbosity_ >= 1 ) std::cout << "branch = " << name << ": value = " << value << std::endl;
  branchMap::iterator branch = branches_.find(name);
  if ( branch != branches_.end() ) {
    branch->second.valueI_ = value;
  } else {
    throw cms::Exception("InvalidParameter") 
      << "No branch with name = " << name << " defined !!\n";
  }
}

void TauTauNtupleProducer::setValueUL(const std::string& name, unsigned long value) 
{
  if ( verbosity_ >= 1 ) std::cout << "branch = " << name << ": value = " << value << std::endl;
  branchMap::iterator branch = branches_.find(name);
  if ( branch != branches_.end() ) {
    branch->second.valueUL_ = value;
  } else {
    throw cms::Exception("InvalidParameter") 
      << "No branch with name = " << name << " defined !!\n";
  }
}

//
//-------------------------------------------------------------------------------
//

void TauTauNtupleProducer::addBranch_diTau(const std::string& name) 
{
  addBranch_EnPxPyPz(name);
  addBranch_PtEtaPhiMass(name);
  addBranchF(name + "Charge");
  addBranchF("diTauGenMass");
  addBranchF("visMass");
  addBranchF("svfitMass");
  addBranchF("svfitMassErr");
  addBranch_EnPxPyPz("svfitMarkovChain");
  addBranch_PtEtaPhiMass("svfitMarkovChain");
  addBranchF("mTtotal");
  addBranchF("dEtatt");
  addBranchF("dPhitt");
  addBranchF("dRtt");
  addBranchF("pThiggs");
}

void TauTauNtupleProducer::addBranch_Tau(const std::string& name) 
{
  addBranch_EnPxPyPz(name);
  addBranch_PtEtaPhiMass(name);
  addBranchF(name + "Charge");
  addBranch_PtEtaPhiMass(name + "Gen");
  addBranchF(name + "DecayMode");
  addBranchF(name + "Radius");
  addBranchF(name + "VertexZ");
  for ( std::vector<StringEntryType>::const_iterator tauIdDiscrEntry = tauIdDiscrinatorsToStore_.begin();
	tauIdDiscrEntry != tauIdDiscrinatorsToStore_.end(); ++tauIdDiscrEntry ) {
    addBranchF(name + tauIdDiscrEntry->branchName_);
  }
  addBranchF(name + "RawJetPt");
  addBranchF(name + "RawJetEta");
  addBranchF(name + "RawJetPhi");  
  addBranchF(name + "CorrJetPt");
}

void TauTauNtupleProducer::addBranch_TauLT_base(const std::string& name)
{
  addBranchF(name + "chargedIsoPtSum");
  addBranchF(name + "neutralIsoPtSum");
  addBranchF(name + "puCorrPtSum");
  addBranchF(name + "dxy");
  addBranchF(name + "dxyErr");
  addBranchF(name + "dxySig");
  addBranchI(name + "hasSecVtx");
  addBranchF(name + "flightLength2");
  addBranchF(name + "flightLengthSig");
}

void TauTauNtupleProducer::addBranch_TauLT_extended(const std::string& name)
{
  addBranch_PtEtaPhi(name + "leadPFChargedHadrTrack");
  addBranchF(name + "leadPFChargedHadrTrackCharge");
  addBranch_XYZ(name + "dxyPCA");
  addBranch_XYZ(name + "flightLength");
  addBranch_XYZ(name + "pv");
  addBranch_Cov3d(name + "pvCov");
  addBranch_XYZ(name + "sv");
  addBranch_Cov3d(name + "svCov");
}

void TauTauNtupleProducer::addBranch_Jet(const std::string& name) 
{
  addBranch_EnPxPyPz(name);
  addBranch_PtEtaPhiMass(name);
  addBranch_EnPxPyPz(name + "raw");
  addBranch_PtEtaPhiMass(name + "raw");
  addBranchF(name + "Charge");
  addBranchF(name + "Btag");
  addBranchI(name + "GenPartonFlavour");
}

void TauTauNtupleProducer::addBranch_bJet(const std::string& name) 
{
  addBranch_EnPxPyPz(name);
  addBranch_PtEtaPhiMass(name);
  addBranch_EnPxPyPz(name + "raw");
  addBranch_PtEtaPhiMass(name + "raw");
  addBranchF(name + "Charge");
  addBranchF(name + "Btag");
  addBranchF(name + "EnRegFactor");
  addBranchI(name + "GenPartonFlavour");
}

void TauTauNtupleProducer::addBranch_Electron(const std::string& name)
{
  addBranch_PtEtaPhi(name);
  addBranchF(name + "Charge");
}
 
void TauTauNtupleProducer::addBranch_Muon(const std::string& name)
{
  addBranch_PtEtaPhi(name);
  addBranchF(name + "Charge");
}

void TauTauNtupleProducer::addBranch_EnPxPyPz(const std::string& name) 
{
  addBranchF(name + "E");
  addBranch_PxPyPz(name);
}

void TauTauNtupleProducer::addBranch_PxPyPz(const std::string& name) 
{  
  addBranchF(name + "Px");
  addBranchF(name + "Py");
  addBranchF(name + "Pz");
}

void TauTauNtupleProducer::addBranch_PtEtaPhi(const std::string& name) 
{
  addBranchF(name + "Pt");
  addBranchF(name + "Eta");
  addBranchF(name + "Phi");
}

void TauTauNtupleProducer::addBranch_PtEtaPhiMass(const std::string& name) 
{
  addBranch_PtEtaPhi(name);
  addBranchF(name + "Mass");
}

void TauTauNtupleProducer::addBranch_XYZ(const std::string& name)
{
  addBranchF(name + "X");
  addBranchF(name + "Y");
  addBranchF(name + "Z");
}

void TauTauNtupleProducer::addBranch_Cov2d(const std::string& name)
{
  addBranchF(name + "00");
  addBranchF(name + "01");
  addBranchF(name + "10");
  addBranchF(name + "11");
}

void TauTauNtupleProducer::addBranch_Cov3d(const std::string& name)
{
  addBranch_Cov2d(name);
  addBranchF(name + "02");
  addBranchF(name + "12");
  addBranchF(name + "20");
  addBranchF(name + "21");
  addBranchF(name + "22");
}

//
//-------------------------------------------------------------------------------
//

namespace
{
  double compMt2(const reco::Candidate::LorentzVector& leg1P4, const reco::Candidate::LorentzVector& leg2P4)
  {
    double dphi = normalizedPhi(leg1P4.phi() - leg2P4.phi());
    double mt2 = 2.*leg1P4.pt()*leg2P4.pt()*(1. - TMath::Cos(dphi));
    // CV: protection against rounding errors
    if ( mt2 < 0. ) mt2 = 0.; 
    return mt2;
  }
}

void TauTauNtupleProducer::setValue_diTau(const std::string& name, const PATDiTauPair& diTau) 
{
  const pat::Tau* leg1 = &(*diTau.leg1());
  const pat::Tau* leg2 = &(*diTau.leg2());
  std::vector<svFitStandalone::MeasuredTauLepton> measuredTauLeptons;
  if ( leg1->pt() > leg2->pt() ) {
    measuredTauLeptons.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg1->pt(), leg1->eta(), leg1->phi(), leg1->mass()));
    measuredTauLeptons.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg2->pt(), leg2->eta(), leg2->phi(), leg2->mass()));
  } else {
    measuredTauLeptons.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg2->pt(), leg2->eta(), leg2->phi(), leg2->mass()));
    measuredTauLeptons.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg1->pt(), leg1->eta(), leg1->phi(), leg1->mass()));
  }
  const reco::MET* met = &(*diTau.met());
  SVfitStandaloneAlgorithm svFitAlgorithm(measuredTauLeptons, met->px(), met->py(), met->getSignificanceMatrix(), verbosity_);
  //svFitAlgorithm.addLogM(false);
  svFitAlgorithm.addLogM(true, 2.);
  svFitAlgorithm.integrateVEGAS();
  if ( svFitAlgorithm.isValidSolution() ) {
    setValueF("svfitMass", svFitAlgorithm.getMass());
    setValueF("svfitMassErr", svFitAlgorithm.massUncert());
  }
  //std::cout << "svfitMass = " << svFitAlgorithm.getMass() << " +/- " << svFitAlgorithm.massUncert() << std::endl;
  svFitAlgorithm.addLogM(false);
  svFitAlgorithm.integrateMarkovChain();
  if ( svFitAlgorithm.isValidSolution() ) {
    math::PtEtaPhiMLorentzVectorD svFitP4(svFitAlgorithm.pt(), svFitAlgorithm.eta(), svFitAlgorithm.phi(), svFitAlgorithm.mass());
    setValue_EnPxPyPz("svfitMarkovChain", svFitP4);
    setValue_PtEtaPhiMass("svfitMarkovChain", svFitP4);
  }
  setValueF(name + "Charge", leg1->charge() + leg2->charge());
  if ( leg1->genLepton() && leg2->genLepton() ) {
    setValueF("diTauGenMass", (leg1->genLepton()->p4() + leg2->genLepton()->p4()).mass());
  }
  setValueF("visMass", (leg1->p4() + leg2->p4()).mass());
  // compute "total transverse mass" used in ATLAS MSSM Higgs -> tautau analysis,
  // cf. https://cds.cern.ch/record/1744694/files/ATLAS-CONF-2014-049.pdf
  double mTtotal = TMath::Sqrt(compMt2(leg1->p4(), leg2->p4()) + compMt2(leg1->p4(), met->p4()) + compMt2(leg2->p4(), met->p4()));
  setValueF("mTtotal", mTtotal);
  setValueF("dEtatt", TMath::Abs(leg1->eta() - leg2->eta()));
  setValueF("dPhitt", reco::deltaPhi(leg1->phi(), leg2->phi()));
  setValueF("dRtt", deltaR(leg1->p4(), leg2->p4()));
  double pXhiggs = leg1->px() + leg2->px() + met->px();
  double pYhiggs = leg1->py() + leg2->py() + met->py();
  setValueF("pThiggs", TMath::Sqrt(pXhiggs*pXhiggs + pYhiggs*pYhiggs));
}

namespace
{
  double square(double x)
  {
    return (x*x);
  }
}

void TauTauNtupleProducer::setValue_Tau(const std::string& name, const pat::Tau& tau, const edm::Event& evt, const edm::EventSetup& es) 
{
  if ( verbosity_ >= 1 ) {
    std::cout << "<TauTauNtupleProducer::setValue_Tau>:" << std::endl;
    std::cout << " tau: Pt = " << tau.pt() << ", eta = " << tau.eta() << ", phi = " << tau.phi() << std::endl;
    std::cout << " genLepton: ";
    if ( tau.genLepton() != 0 ) {
      std::cout << "Pt = " << tau.genLepton()->pt() << ", eta = " << tau.genLepton()->eta() << ", phi = " << tau.genLepton()->phi();
    } else {
      std::cout << "N/A";
    }
    std::cout << std::endl;
    std::cout << " genJet: ";
    if ( tau.genJet() != 0 ) {
      std::cout << "Pt = " << tau.genJet()->pt() << ", eta = " << tau.genJet()->eta() << ", phi = " << tau.genJet()->phi();
    } else {
      std::cout << "N/A";
    }
    std::cout << std::endl;
  }
  setValue_EnPxPyPz(name, tau.p4());
  setValue_PtEtaPhiMass(name, tau.p4());
  setValueF(name + "Charge", tau.charge());
  if ( tau.genJet() != 0 ) {
    setValue_PtEtaPhiMass(name + "Gen", tau.genJet()->p4());
  }
  setValueF(name + "DecayMode", tau.decayMode());
  double jetRadius2 = square(tau.etaetaMoment()) + square(tau.phiphiMoment());
  setValueF(name + "Radius", TMath::Sqrt(jetRadius2));
  setValueF(name + "VertexZ", tau.vertex().z());
  for ( std::vector<StringEntryType>::const_iterator tauIdDiscrEntry = tauIdDiscrinatorsToStore_.begin();
	tauIdDiscrEntry != tauIdDiscrinatorsToStore_.end(); ++tauIdDiscrEntry ) {
    setValueF(name + tauIdDiscrEntry->branchName_, tau.tauID(tauIdDiscrEntry->src_));
  }
  double rawJetPt = tau.p4Jet().pt();
  setValueF(name + "RawJetPt", rawJetPt);
  setValueF(name + "RawJetEta", tau.p4Jet().eta());
  setValueF(name + "RawJetPhi", tau.p4Jet().phi());
  double corrJetPt = -1.;
  if ( jec_ && tau.pfJetRef().isNonnull() && tau.pfJetRef().isAvailable() ) {
    double jec_value = jec_->correction(*tau.pfJetRef(), evt, es);
    corrJetPt = rawJetPt*jec_value;
  }
  setValueF(name + "CorrJetPt", corrJetPt);
}

void TauTauNtupleProducer::setValue_TauLT_base(const std::string& name, const pat::Tau& tau)
{
  setValueF(name + "chargedIsoPtSum", tau.tauID("chargedIsoPtSum"));
  setValueF(name + "neutralIsoPtSum", tau.tauID("neutralIsoPtSum"));
  setValueF(name + "puCorrPtSum", tau.tauID("puCorrPtSum"));
  setValueF(name + "dxy", tau.dxy());
  setValueF(name + "dxyErr", tau.dxy_error());
  setValueF(name + "dxySig", tau.dxy_Sig());
  setValueI(name + "hasSecVtx", tau.hasSecondaryVertex());
  if ( tau.hasSecondaryVertex() ) {
    setValueF(name + "flightLength2", tau.flightLength().Mag2());
    setValueF(name + "flightLengthSig", tau.flightLengthSig());
  }
}

void TauTauNtupleProducer::setValue_TauLT_extended(const std::string& name, const pat::Tau& tau)
{
  if ( tau.leadPFChargedHadrCand().isNonnull() ) {
    setValue_PtEtaPhi(name + "leadPFChargedHadrTrack", tau.leadPFChargedHadrCand()->pt(), tau.leadPFChargedHadrCand()->eta(), tau.leadPFChargedHadrCand()->phi());
    setValueF(name + "leadPFChargedHadrTrackCharge", tau.leadPFChargedHadrCand()->charge());
  }
  setValue_XYZ(name + "dxyPCA", tau.dxy_PCA());
  setValue_XYZ(name + "pv", tau.primaryVertexPos());
  setValue_Cov3d(name + "pvCov", tau.primaryVertexCov());
  if ( tau.hasSecondaryVertex() ) {
    setValue_XYZ(name + "flightLength", tau.flightLength());
    setValue_XYZ(name + "sv", tau.secondaryVertexPos());
    setValue_Cov3d(name + "svCov", tau.secondaryVertexCov());
  }
}

void TauTauNtupleProducer::setValue_Jet(const std::string& name, const pat::Jet& jet) 
{
  setValue_EnPxPyPz(name, jet.p4());
  setValue_PtEtaPhiMass(name, jet.p4());
  reco::Candidate::LorentzVector jetP4_raw = ( jet.jecSetsAvailable() ) ? jet.correctedP4("Uncorrected") : jet.p4();
  setValue_EnPxPyPz(name + "raw", jetP4_raw);
  setValue_PtEtaPhiMass(name + "raw", jetP4_raw);
  setValueF(name + "Charge", jet.charge());
  setValueF(name + "Btag", jet.bDiscriminator(bJetDiscriminator_));  
  setValueI(name + "GenPartonFlavour", jet.partonFlavour());
}

void TauTauNtupleProducer::setValue_bJet(const std::string& name, const pat::Jet& jet) 
{
  setValue_EnPxPyPz(name, jet.p4());
  setValue_PtEtaPhiMass(name, jet.p4());
  reco::Candidate::LorentzVector jetP4_raw = ( jet.jecSetsAvailable() ) ? jet.correctedP4("Uncorrected") : jet.p4();
  setValue_EnPxPyPz(name + "raw", jetP4_raw);
  setValue_PtEtaPhiMass(name + "raw", jetP4_raw);
  setValueF(name + "Charge", jet.charge());
  setValueF(name + "Btag", jet.bDiscriminator(bJetDiscriminator_));
  setValueF(name + "EnRegFactor", jet.userFloat("bJetEnReg"));
  setValueI(name + "GenPartonFlavour", jet.partonFlavour());
}

void TauTauNtupleProducer::setValue_Electron(const std::string& name, const pat::Electron& electron)
{
  setValue_PtEtaPhi(name, electron.pt(), electron.eta(), electron.phi());
  setValueF(name + "Charge", electron.charge());
}
 
void TauTauNtupleProducer::setValue_Muon(const std::string& name, const pat::Muon& muon)
{
  setValue_PtEtaPhi(name, muon.pt(), muon.eta(), muon.phi());
  setValueF(name + "Charge", muon.charge());
}

template <typename T>
void TauTauNtupleProducer::setValue_EnPxPyPz(const std::string& name, const T& p4)
{
  setValueF(name + "E",  p4.E());
  setValue_PxPyPz(name,  p4.px(), p4.py(), p4.pz());  
}

void TauTauNtupleProducer::setValue_PxPyPz(const std::string& name, double px, double py, double pz)
{
  setValueF(name + "Px", px);
  setValueF(name + "Py", py);
  setValueF(name + "Pz", pz);
}

void TauTauNtupleProducer::setValue_PtEtaPhi(const std::string& name, double pt, double eta, double phi)
{
  setValueF(name + "Pt", pt);
  setValueF(name + "Eta", eta);
  setValueF(name + "Phi", phi);
}

template <typename T>
void TauTauNtupleProducer::setValue_PtEtaPhiMass(const std::string& name, const T& p4)
{
  setValue_PtEtaPhi(name, p4.pt(), p4.eta(), p4.phi());
  setValueF(name + "Mass", p4.M());
}

template <typename T>
void TauTauNtupleProducer::setValue_XYZ(const std::string& name, const T& pos)
{
  setValueF(name + "X", pos.x());
  setValueF(name + "Y", pos.y());
  setValueF(name + "Z", pos.z());
}

template <typename T>
void TauTauNtupleProducer::setValue_Cov2d(const std::string& name, const T& cov)
{
  setValueF(name + "00", cov(0,0));
  setValueF(name + "01", cov(0,1));
  setValueF(name + "10", cov(1,0));
  setValueF(name + "11", cov(1,1));
}

template <typename T>
void TauTauNtupleProducer::setValue_Cov3d(const std::string& name, const T& cov)
{
  setValue_Cov2d(name, cov);
  setValueF(name + "02", cov(0,2));
  setValueF(name + "12", cov(1,2));
  setValueF(name + "20", cov(2,0));
  setValueF(name + "21", cov(2,1));
  setValueF(name + "22", cov(2,2));
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(TauTauNtupleProducer);
