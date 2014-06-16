#ifndef LLRAnalysis_HadTauStudies_TauTauNtupleProducer_h  
#define LLRAnalysis_HadTauStudies_TauTauNtupleProducer_h 

/** \class TauTauNtupleProducer
 *
 * Produce Ntuple for tau_had tau_had channel of SM/MSSM Higgs -> tau tau analysis
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h" 
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"

#include <TTree.h>
#include <TMatrixD.h>
#include <TString.h>

#include <map>
#include <string>
#include <vector>
#include <ostream>

class TauTauNtupleProducer : public edm::EDAnalyzer 
{
 public:
  
  TauTauNtupleProducer(const edm::ParameterSet&);
  ~TauTauNtupleProducer();

  void analyze(const edm::Event&, const edm::EventSetup&);
  void beginJob();

 private:

  void addBranchF(const std::string&);
  void addBranchI(const std::string&);
  void addBranchUL(const std::string&);

  void addBranch_diTau(const std::string&);
  void addBranch_Tau(const std::string&);
  void addBranch_TauLT_base(const std::string&);
  void addBranch_TauLT_extended(const std::string&);
  void addBranch_Jet(const std::string&);
  void addBranch_bJet(const std::string&);
  void addBranch_Electron(const std::string&);
  void addBranch_Muon(const std::string&);

  void addBranch_EnPxPyPz(const std::string&);
  void addBranch_PxPyPz(const std::string&);
  void addBranch_PtEtaPhi(const std::string&);
  void addBranch_PtEtaPhiMass(const std::string&);
  void addBranch_XYZ(const std::string&);
  void addBranch_Cov3d(const std::string&);
  void addBranch_Cov2d(const std::string&);
    
  void resetBranches();

  void printBranches(std::ostream&);

  void setValueF(const std::string&, double);
  void setValueI(const std::string&, int);
  void setValueUL(const std::string&, unsigned long);

  void setValue_diTau(const std::string&, const PATDiTauPair&);
  void setValue_Tau(const std::string&, const pat::Tau&, const edm::Event&, const edm::EventSetup&);
  void setValue_TauLT_base(const std::string&, const pat::Tau&);
  void setValue_TauLT_extended(const std::string&, const pat::Tau&);
  void setValue_Jet(const std::string&, const pat::Jet&);
  void setValue_bJet(const std::string&, const pat::Jet&);
  void setValue_Electron(const std::string&, const pat::Electron&);
  void setValue_Muon(const std::string&, const pat::Muon&);

  template <typename T>
  void setValue_EnPxPyPz(const std::string&, const T&);
  void setValue_PxPyPz(const std::string&, double, double, double);
  void setValue_PtEtaPhi(const std::string&, double, double, double);
  template <typename T>
  void setValue_PtEtaPhiMass(const std::string&, const T&);
  template <typename T>
  void setValue_XYZ(const std::string&, const T&);
  template <typename T>
  void setValue_Cov3d(const std::string&, const T&);
  template <typename T>
  void setValue_Cov2d(const std::string&, const T&);

  std::string moduleLabel_;

  edm::InputTag srcDiTau_;
  typedef std::vector<std::string> vstring;
  vstring bestDiTauPreselection_;
  vstring bestDiTauRanking_;
  bool invertRanking_; 
  enum { kVEGAS, kMarkovChain };
  int svFitMode_;

  struct StringEntryType
  {
    StringEntryType(const std::string& branchName, const std::string& src)
      : branchName_(branchName),
	src_(src)
    {}
    ~StringEntryType() {}
    std::string branchName_;
    std::string src_;    
  };
  std::vector<StringEntryType> tauIdDiscrinatorsToStore_;
  
  std::vector<StringEntryType> readStringEntries(const edm::ParameterSet&, const std::string&);

  edm::InputTag srcTriggerResults_;
  std::vector<StringEntryType> hltPaths_diTau_;
  std::vector<StringEntryType> hltPaths_diTauJet_;
  std::vector<StringEntryType> hltPaths_singleJet_;
  std::vector<StringEntryType> hltPathsToStore_;
  edm::InputTag srcTriggerObjects_;
  vstring hltTauFilters_diTau_;
  vstring hltTauFilters_diTauJet_;
  vstring hltJetFilters_diTauJet_;
  vstring hltJetFilters_singleJet_;

  edm::InputTag srcL1Taus_;
  edm::InputTag srcL1Jets_;

  edm::InputTag srcElectrons_;
  edm::InputTag srcMuons_;

  edm::InputTag srcJets_;
  edm::InputTag srcPileupJetId_;
  PileupJetIdentifier::Id wpPileupJetId_;
  edm::InputTag srcPileupJetIdMVA_;
  std::string bJetDiscriminator_;
  std::string jetCorrLabel_;
  const JetCorrector* jec_;
  std::string jetCorrPayload_;
  std::string jecUncertaintyTag_;
  JetCorrectionUncertainty* jecUncertainty_;
  double minJetPt_;
  double maxJetAbsEta_;
  double minBJetPt_;
  double maxBJetAbsEta_;
  double wpBJetDiscriminator_;

  edm::InputTag srcRawPFMEt_;

  edm::InputTag srcVertices_;
  edm::InputTag srcRho_;

  struct InputTagEntryType
  {
    InputTagEntryType(const std::string& branchName, const edm::InputTag& src)
      : branchName_(branchName),
	src_(src)
    {}
    ~InputTagEntryType() {}
    std::string branchName_;
    edm::InputTag src_;
  };
  std::vector<InputTagEntryType> evtWeightsToStore_;

  bool isMC_;
  edm::InputTag srcGenPileUpSummary_;
  edm::InputTag srcLHE_;
  edm::InputTag srcGenParticles_;
  std::string lastInputFileHiggsPtWeight_;
  TFile* inputFileHiggsPtWeight_;
  TH1* lutHiggsPtWeightNom_;
  TH1* lutHiggsPtWeightUp_;
  TH1* lutHiggsPtWeightDown_;

  bool isEmbedded_;
  edm::InputTag srcEmbeddingWeight_;

  PFJetIDSelectionFunctor* loosePFJetIdAlgo_;

  struct branchEntryType
  {
    branchEntryType()
      : valueF_(0.),
        valueI_(0),
	valueUL_(0)
    {}
    ~branchEntryType() {}
    Float_t valueF_;
    Int_t valueI_;
    ULong_t valueUL_;
  };
  typedef std::map<std::string, branchEntryType> branchMap; // key = branch name
  branchMap branches_;

  TTree* ntuple_;

  int verbosity_;
};

#endif


