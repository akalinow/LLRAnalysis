
/** \executable FWLiteTauTauAnalyzer2b2tau
 *
 * Run 2 Higgs -> 2 b-jet 2 tau_h analysis.
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/normalizedPhi.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "LLRAnalysis/HadTauStudies2b2tau/interface/TauTauHistManager2b2tau.h"
#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"
#include "LLRAnalysis/HadTauStudies/interface/triggerTurnOnCurves.h"
#include "LLRAnalysis/HadTauStudies/interface/RunLumiSectionEventNumberSelector.h"
#include "LLRAnalysis/HadTauStudies/interface/particleIDlooseToTightWeightEntryType.h"
#include "LLRAnalysis/HadTauStudies/interface/mt2Interface.h"
#include "LLRAnalysis/HHKinFit/interface/DiJetKinFitMaster.h"
#include "LLRAnalysis/HHKinFit/interface/HHKinFitMaster.h"
#include "LLRAnalysis/Utilities/interface/BtagSF.hh"
#include "TauAnalysis/CandidateTools/interface/candidateAuxFunctions.h"

#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>
#include <TFormula.h>
#include <TLorentzVector.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;
typedef std::vector<double> vdouble;

enum { kData, kSignal, kSM_Higgs, kZTTmc, kZL, kZJ, kZTT_Embedded, kW, kTT, kTT_Embedded, kVV };

enum { kOS, kSS };

enum { kTree, kHPScombIso3HitsLoose, kHPScombIso3HitsMedium, kMVAwLToldDMsLoose, kMVAwLToldDMsMedium, kMVAwLToldDMsTight, kMVAwLToldDMsVTight };

struct weightEntryType
{
  weightEntryType(TTree* inputTree, const std::string& branchName)
    : branchName_(branchName)
  {
    inputTree->SetBranchAddress(branchName_.data(), &value_);
  }
  ~weightEntryType() {}
  std::string branchName_;
  Float_t value_;
};

edm::ParameterSet makeHistManagerConfig(const std::string& process, const std::string& category, const std::string& tauPtBin, 
					const vdouble& tau1EtaBins, const vdouble& tau2EtaBins, 
					const std::string& central_or_shift)
{
  edm::ParameterSet cfg;
  cfg.addParameter<std::string>("process", process);
  cfg.addParameter<std::string>("category", category);
  cfg.addParameter<std::string>("tauPtBin", tauPtBin);
  cfg.addParameter<vdouble>("tau1EtaBins", tau1EtaBins);
  cfg.addParameter<vdouble>("tau2EtaBins", tau2EtaBins);
  cfg.addParameter<std::string>("central_or_shift", central_or_shift);
  return cfg;
}

bool contains(const std::string& fullstring, const std::string& substring)
{
  return (fullstring.find(substring) != std::string::npos);
}

int getGenMatch(int isGenHadTau, int isGenMuon, int isGenElectron, int isGenJet)
{
  int genMatch = 0;
  if      ( isGenHadTau   ) genMatch = 1;
  else if ( isGenMuon     ) genMatch = 2;
  else if ( isGenElectron ) genMatch = 3;
  else if ( isGenJet      ) genMatch = 4;
  return genMatch;
}

double compTauDecayModeWeight(double tauEta, int tauDecayMode)
{
  // CV: reweighting factors to correct data/MC differences observed in distribution of tau decay modes reconstructed by boosted tau ID,
  //     cf. https://indico.cern.ch/event/308081/session/6/contribution/16/material/slides/0.pdf
  double weight = 1.0;
  if ( TMath::Abs(tauEta) < 1.47 ) {
    if      ( tauDecayMode ==  0 ) weight = 0.87;
    else if ( tauDecayMode ==  1 ) weight = 1.06;
    else if ( tauDecayMode == 10 ) weight = 1.02;
  } else {
    if      ( tauDecayMode ==  0 ) weight = 0.96;
    else if ( tauDecayMode ==  1 ) weight = 1.00;
    else if ( tauDecayMode == 10 ) weight = 1.06;
  }
  //std::cout << "tauDecayMode = " << tauDecayMode << ": weight = " << weight << std::endl;
  return weight;
}

struct jetType
{
  jetType(double px, double py, double pz, double energy, double enRegFactor, double btagDiscr, int genPartonFlavour)
    : p4_(px, py, pz, energy),
      p4Reg_(px*enRegFactor, py*enRegFactor, pz*enRegFactor, energy*enRegFactor),
      enRegFactor_(enRegFactor),
      btagDiscr_(btagDiscr),
      genPartonFlavour_(genPartonFlavour),
      passesLooseWP_(false),
      passesMediumWP_(false),
      passesTightWP_(false)
  {}
  ~jetType() {}
  reco::Candidate::LorentzVector p4_;
  reco::Candidate::LorentzVector p4Reg_;
  double enRegFactor_;
  double btagDiscr_;
  int genPartonFlavour_;
  bool passesLooseWP_;
  bool passesMediumWP_;
  bool passesTightWP_;
};

void addJet(std::vector<jetType>& jets, double minJetPt, double minJetBtagDiscr,
	    double jetPt, double jetEta, JetCorrectionUncertainty* jecUncertainty, bool applyJetEnergyScaleShapeUp, bool applyJetEnergyScaleShapeDown,
	    double jetPx, double jetPy, double jetPz, double jetE, double jetEnRegFactor, double jetBtagDiscr, double jetGenPartonFlavour)
{
  assert(!(applyJetEnergyScaleShapeUp && applyJetEnergyScaleShapeDown));
  double sf = 1.;
  if ( applyJetEnergyScaleShapeUp || applyJetEnergyScaleShapeDown ) {
    assert(jecUncertainty);
    jecUncertainty->setJetEta(jetEta);
    jecUncertainty->setJetPt(jetPt);
    double shift = jecUncertainty->getUncertainty(true);
    if      ( applyJetEnergyScaleShapeUp   ) sf = (1. + shift);
    else if ( applyJetEnergyScaleShapeDown ) sf = (1. - shift);
    else assert(0);
  }
  if ( (sf*jetPt) > minJetPt && (jetBtagDiscr > minJetBtagDiscr || minJetBtagDiscr == -1.) ) {
    jets.push_back(jetType(sf*jetPx, sf*jetPy, sf*jetPz, sf*jetE, jetEnRegFactor, jetBtagDiscr, jetGenPartonFlavour));
  }
}

bool isHigherBtagDiscriminator(const jetType& bjet1, const jetType& bjet2)
{
  if (  bjet1.passesTightWP_  && !bjet2.passesTightWP_  ) return true;
  if ( !bjet1.passesTightWP_  &&  bjet2.passesTightWP_  ) return false;
  if (  bjet1.passesMediumWP_ && !bjet2.passesMediumWP_ ) return true;
  if ( !bjet1.passesMediumWP_ &&  bjet2.passesMediumWP_ ) return false;
  if (  bjet1.passesLooseWP_  && !bjet2.passesLooseWP_  ) return true;
  if ( !bjet1.passesLooseWP_  &&  bjet2.passesLooseWP_  ) return false;
  return (bjet1.btagDiscr_ > bjet2.btagDiscr_);
}

bool isHigherPt(const jetType& jet1, const jetType& jet2)
{
  return (jet1.btagDiscr_ > jet2.btagDiscr_);
}

//-------------------------------------------------------------------------------
// CV: kinematic fit developed by the University of Hamburg group,
//     presented in the H -> tautau meeting on August 21st 2014:
//       https://indico.cern.ch/event/333569/session/8/contribution/17/material/slides/0.pdf
//    (I adapted the code to make it work only on the h -> bb decay)

struct kinFitResult
{
  reco::Candidate::LorentzVector bJet1P4_fitted_; // filled in case of 2-body fit only
  reco::Candidate::LorentzVector bJet2P4_fitted_;
  double mhh_;
  double chi2_;
};

kinFitResult compKinFit2bdy(const reco::Candidate::LorentzVector& bJet1P4, 
			    const reco::Candidate::LorentzVector& bJet2P4, 
			    const reco::Candidate::LorentzVector& HttP4,
			    double mh = 125.)
{
  //std::cout << "<compKinFit2bdy>:" << std::endl;
  //std::cout << " bJet1: Pt = " << bJet1P4.pt() << ", eta = " << bJet1P4.eta() << ", phi = " << bJet1P4.phi() << ", mass = " << bJet1P4.mass() << std::endl;
  //std::cout << " bJet2: Pt = " << bJet2P4.pt() << ", eta = " << bJet2P4.eta() << ", phi = " << bJet2P4.phi() << ", mass = " << bJet2P4.mass() << std::endl;
  //std::cout << " Htt: Pt = " << HttP4.pt() << ", eta = " << HttP4.eta() << ", phi = " << HttP4.phi() << ", mass = " << HttP4.mass() << std::endl;
  //std::cout << " mh = " << mh << std::endl;
  TLorentzVector* bJet1P4_lorentzvector = new TLorentzVector(bJet1P4.px(), bJet1P4.py(), bJet1P4.pz(), bJet1P4.E());
  TLorentzVector* bJet2P4_lorentzvector = new TLorentzVector(bJet2P4.px(), bJet2P4.py(), bJet2P4.pz(), bJet2P4.E());
  DiJetKinFitMaster kinFit = DiJetKinFitMaster(bJet1P4_lorentzvector, bJet2P4_lorentzvector);  
  kinFit.addMhHypothesis(mh);
  kinFit.doFullFit();
  TLorentzVector bJet1P4_fitted_lorentzvector = kinFit.getFitJet1();
  reco::Candidate::LorentzVector bJet1P4_fitted(
    bJet1P4_fitted_lorentzvector.Px(), 
    bJet1P4_fitted_lorentzvector.Py(), 
    bJet1P4_fitted_lorentzvector.Pz(), 
    bJet1P4_fitted_lorentzvector.E());
  TLorentzVector bJet2P4_fitted_lorentzvector = kinFit.getFitJet2();
  reco::Candidate::LorentzVector bJet2P4_fitted(
    bJet2P4_fitted_lorentzvector.Px(), 
    bJet2P4_fitted_lorentzvector.Py(), 
    bJet2P4_fitted_lorentzvector.Pz(), 
    bJet2P4_fitted_lorentzvector.E());

  reco::Candidate::LorentzVector hhP4_fitted = (bJet1P4_fitted + bJet2P4_fitted + HttP4);

  kinFitResult fitResult;
  fitResult.bJet1P4_fitted_ = bJet1P4_fitted;
  fitResult.bJet2P4_fitted_ = bJet2P4_fitted;
  fitResult.mhh_ = hhP4_fitted.mass();
  fitResult.chi2_ = kinFit.getBestChi2FullFit();

  delete bJet1P4_lorentzvector;
  delete bJet2P4_lorentzvector;

  return fitResult;
}

kinFitResult compKinFit4bdy(const reco::Candidate::LorentzVector& bJet1P4, 
			    const reco::Candidate::LorentzVector& bJet2P4,
			    const reco::Candidate::LorentzVector& tau1P4,
			    const reco::Candidate::LorentzVector& tau2P4,
			    const reco::Candidate::LorentzVector& metP4, const TMatrixD& metCov,
			    double mh = 125.)
{
  //std::cout << "<compKinFit4bdy>:" << std::endl;
  //std::cout << " bJet1: Pt = " << bJet1P4.pt() << ", eta = " << bJet1P4.eta() << ", phi = " << bJet1P4.phi() << ", mass = " << bJet1P4.mass() << std::endl;
  //std::cout << " bJet2: Pt = " << bJet2P4.pt() << ", eta = " << bJet2P4.eta() << ", phi = " << bJet2P4.phi() << ", mass = " << bJet2P4.mass() << std::endl;
  //std::cout << " tau1: Pt = " << tau1P4.pt() << ", eta = " << tau1P4.eta() << ", phi = " << tau1P4.phi() << ", mass = " << tau1P4.mass() << std::endl;
  //std::cout << " tau2: Pt = " << tau2P4.pt() << ", eta = " << tau2P4.eta() << ", phi = " << tau2P4.phi() << ", mass = " << tau2P4.mass() << std::endl;
  //std::cout << " met: Pt = " << metP4.pt() << ", phi = " << metP4.phi() << std::endl;
  //std::cout << " metCov:" << std::endl;
  //metCov.Print();
  //std::cout << " mh = " << mh << std::endl;
  TLorentzVector* bJet1P4_lorentzvector = new TLorentzVector(bJet1P4.px(), bJet1P4.py(), bJet1P4.pz(), bJet1P4.E());
  TLorentzVector* bJet2P4_lorentzvector = new TLorentzVector(bJet2P4.px(), bJet2P4.py(), bJet2P4.pz(), bJet2P4.E());
  TLorentzVector* tau1P4_lorentzvector  = new TLorentzVector(tau1P4.px(), tau1P4.py(), tau1P4.pz(), tau1P4.E());
  TLorentzVector* tau2P4_lorentzvector  = new TLorentzVector(tau2P4.px(), tau2P4.py(), tau2P4.pz(), tau2P4.E());
  TLorentzVector* metP4_lorentzvector   = new TLorentzVector(metP4.px(), metP4.py(), metP4.pz(), metP4.E());
  HHKinFitMaster kinFit = HHKinFitMaster(bJet1P4_lorentzvector, bJet2P4_lorentzvector, tau1P4_lorentzvector, tau2P4_lorentzvector);
  kinFit.setAdvancedBalance(metP4_lorentzvector, metCov);
  kinFit.addMh1Hypothesis(mh);
  kinFit.addMh2Hypothesis(mh);
  kinFit.doFullFit();

  kinFitResult fitResult;
  fitResult.mhh_ = kinFit.getBestMHFullFit();
  fitResult.chi2_ = kinFit.getBestChi2FullFit();

  delete bJet1P4_lorentzvector;
  delete bJet2P4_lorentzvector;
  delete tau1P4_lorentzvector;
  delete tau2P4_lorentzvector;
  delete metP4_lorentzvector; 

  return fitResult;
}
//-------------------------------------------------------------------------------

namespace
{
  TFile* openFile(const edm::FileInPath& inputFileName)
  {
    if ( !inputFileName.isLocal() ) 
      throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
	<< " Failed to find file = " << inputFileName << " !!\n";
    TFile* inputFile = new TFile(inputFileName.fullPath().data());
    return inputFile;
  }

  TH1* loadLUT(TFile* inputFile, const std::string& lutName)
  {
    TH1* lut = dynamic_cast<TH1*>(inputFile->Get(lutName.data()));
    if ( !lut ) 
      throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
	<< " Failed to load LUT = " << lutName.data() << " from file = " << inputFile->GetName() << " !!\n";
    return lut;
  }

  double square(double x)
  {
    return x*x;
  }

  std::ostream* openSelEventsFile_output(const std::string& selEventsFileName_output, const std::string& category)
  {
    size_t idx = selEventsFileName_output.find_last_of('.');
    std::string selEventsFileName_part1 = std::string(selEventsFileName_output, 0, idx);
    std::string selEventsFileName_part2 = ( idx != std::string::npos ) ? std::string(selEventsFileName_output, idx, std::string::npos) : "";
    std::string selEventsFileName = Form("%s_%s%s", selEventsFileName_part1.data(), category.data(), selEventsFileName_part2.data());
    std::cout << "writing events selected in '" << category << "' category to file = " << selEventsFileName << std::endl;  
    std::ostream* selEventsFile = new std::ofstream(selEventsFileName.data(), std::ios::out);
    return selEventsFile;
  }
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<FWLiteTauTauAnalyzer2b2tau>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("FWLiteTauTauAnalyzer2b2tau");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("FWLiteTauTauAnalyzer") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgFWLiteTauTauAnalyzer2b2tau = cfg.getParameter<edm::ParameterSet>("FWLiteTauTauAnalyzer2b2tau");
  
  std::string treeName = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("treeName");

  std::string process_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("process");
  int process = -1;
  if      ( process_string == "data_obs"     ) process = kData;
  else if ( contains(process_string, "data2012run")   ) process = kData; // CV: for checking event yield in individual data-taking periods
  else if ( contains(process_string, "hhTo2b2tau")    || 
	    contains(process_string, "mssmH")         || 
	    contains(process_string, "abelianZprime") ||
	    contains(process_string, "radion")        ||
	    contains(process_string, "graviton")      ) process = kSignal;
  else if ( contains(process_string, "ggH_SM125")     || 
	    contains(process_string, "qqH_SM125")     || 
	    contains(process_string, "VH_SM125")      || 
	    contains(process_string, "singleH_SM125") ) process = kSM_Higgs;
  else if ( process_string == "ZTTmc"        ) process = kZTTmc;
  else if ( process_string == "ZL"           ) process = kZL;
  else if ( process_string == "ZJ"           ) process = kZJ;
  else if ( process_string == "ZTT_Embedded" ) process = kZTT_Embedded;
  else if ( process_string == "Wtmp"         ) process = kW;
  else if ( process_string == "TT"           ) process = kTT;
  else if ( process_string == "TT_Embedded"  ) process = kTT_Embedded;
  else if ( process_string == "VV"           ) process = kVV;
  else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
    << "Invalid Configuration parameter 'process' = " << process_string << " !!\n";

  bool isMC = !(process == kData || process == kZTT_Embedded);
  bool isEmbedded = (process == kZTT_Embedded || process == kTT_Embedded);

  double lumiScale = ( process != kData ) ?
    cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("lumiScale") : 1.;

  vdouble stitchingWeights;
  if ( process == kZTTmc || process == kZL || process == kZJ || process == kW ) {
    stitchingWeights = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<vdouble>("stitchingWeights");
    if ( stitchingWeights.size() != 5 ) throw cms::Exception("FWLiteTauTauAnalyzer") 
      << "Configuration parameter 'stitchingWeights' expected to be of size = 5 !!\n";      
  }

  //double tau1PtMin = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("tau1PtMin");
  //double tau1PtMax = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("tau1PtMax");
  std::string tau1Selection_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("tau1Selection");

  //double tau2PtMin = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("tau2PtMin");
  //double tau2PtMax = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("tau2PtMax");
  std::string tau2Selection_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("tau2Selection");

  std::string tauPtBin = "tau1PtGtXXtau2PtGtXX";
  
  vdouble tau1EtaBins = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<vdouble>("tau1EtaBins"); // CV: eta bins in which jet -> tau fake-rates are determined
  vdouble tau2EtaBins = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<vdouble>("tau2EtaBins");

  std::string discriminator = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("discriminator");

  std::string region = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("region");
  int diTauChargeSel = -1;
  if      ( region.find("OS") != std::string::npos ) diTauChargeSel = kOS;
  else if ( region.find("SS") != std::string::npos ) diTauChargeSel = kSS;
  else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
    << "Invalid Configuration parameter 'region' = " << region << " !!\n";

  std::string trigger_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("trigger");
  int trigger = -1;
  enum { kTau, kJet, kTauPlusJet };
  if      ( trigger_string == "Tau"        ) trigger = kTau;
  else if ( trigger_string == "Jet"        ) trigger = kJet;
  else if ( trigger_string == "TauPlusJet" ) trigger = kTauPlusJet;
  else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
    << "Invalid Configuration parameter 'trigger' = " << trigger_string << " !!\n";
  double tauPtForSwitchingTriggers = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<double>("tauPtForSwitchingTriggers");
  
  std::string applyTauTriggerTurnOn_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("applyTauTriggerTurnOn");
  int applyTauTriggerTurnOn = -1;
  // CV: Trigger turn-on curves for HPS combined isolation 3-hit Loose,
  //     MVA isolation Loose and MVA isolation Medium working-points not measured yet.
  //     Take turn-on curves for most similar measured working-point (does not make much difference).
  if      ( applyTauTriggerTurnOn_string == "tree"                  ) applyTauTriggerTurnOn = kTree;
  else if ( applyTauTriggerTurnOn_string == "HPScombIso3HitsLoose"  ) applyTauTriggerTurnOn = kHPScombIso3HitsMedium; 
  else if ( applyTauTriggerTurnOn_string == "HPScombIso3HitsMedium" ) applyTauTriggerTurnOn = kHPScombIso3HitsMedium;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsLoose"     ) applyTauTriggerTurnOn = kMVAwLToldDMsLoose;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsMedium"    ) applyTauTriggerTurnOn = kMVAwLToldDMsMedium;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsTight"     ) applyTauTriggerTurnOn = kMVAwLToldDMsTight;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsVTight"    ) applyTauTriggerTurnOn = kMVAwLToldDMsVTight;
  else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
    << "Invalid Configuration parameter 'applyTauTriggerTurnOn' = " << applyTauTriggerTurnOn_string << " !!\n";

  BtagSF* bTagSF = new BtagSF(12345);
  
  std::vector<particleIDlooseToTightWeightEntryType*> jetToTauFakeRateLooseToTightWeights;
  bool applyJetToTauFakeRateLooseToTightWeight = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<bool>("applyJetToTauFakeRateLooseToTightWeight");
  if ( applyJetToTauFakeRateLooseToTightWeight ) {
    edm::ParameterSet cfgJetToTauFakeRateLooseToTightWeight = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<edm::ParameterSet>("jetToTauFakeRateLooseToTightWeight");
    
    std::string fitFunctionNormName = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionNormName");
    std::string fitFunctionShapeName_tau1_central = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau1_central");
    std::string graphShapeName_tau1, fitFunctionShapeName_tau1_shift;    
    int applyFitFunction_or_graph_tau1 = particleIDlooseToTightWeightEntryType::kFitFunction;
    if ( cfgJetToTauFakeRateLooseToTightWeight.exists("graphShapeName_tau1") ) {
      graphShapeName_tau1 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("graphShapeName_tau1");
      fitFunctionShapeName_tau1_shift = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau1_shift");
      std::string applyFitFunction_or_graph_tau1_string = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("applyFitFunction_or_graph_tau1");
      if      ( applyFitFunction_or_graph_tau1_string == "fitFunction" ) applyFitFunction_or_graph_tau1 = particleIDlooseToTightWeightEntryType::kFitFunction;
      else if ( applyFitFunction_or_graph_tau1_string == "graph"       ) applyFitFunction_or_graph_tau1 = particleIDlooseToTightWeightEntryType::kGraph;
      else if ( applyFitFunction_or_graph_tau1_string == "notApplied"  ) applyFitFunction_or_graph_tau1 = particleIDlooseToTightWeightEntryType::kNotApplied;
      else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
	<< "Invalid Configuration parameter 'applyFitFunction_or_graph_tau1' = " << applyFitFunction_or_graph_tau1_string << " !!\n";
    }
    double fitFunctionShapePower_tau1 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<double>("fitFunctionShapePower_tau1");
    std::string fitFunctionShapeName_tau2_central = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau2_central");
    std::string graphShapeName_tau2, fitFunctionShapeName_tau2_shift;
    int applyFitFunction_or_graph_tau2 = particleIDlooseToTightWeightEntryType::kFitFunction;
    if ( cfgJetToTauFakeRateLooseToTightWeight.exists("graphShapeName_tau2") ) {
      graphShapeName_tau2 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("graphShapeName_tau2");
      fitFunctionShapeName_tau2_shift = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau2_shift");
      std::string applyFitFunction_or_graph_tau2_string = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("applyFitFunction_or_graph_tau2");
      if      ( applyFitFunction_or_graph_tau2_string == "fitFunction" ) applyFitFunction_or_graph_tau2 = particleIDlooseToTightWeightEntryType::kFitFunction;
      else if ( applyFitFunction_or_graph_tau2_string == "graph"       ) applyFitFunction_or_graph_tau2 = particleIDlooseToTightWeightEntryType::kGraph;
      else if ( applyFitFunction_or_graph_tau2_string == "notApplied"  ) applyFitFunction_or_graph_tau2 = particleIDlooseToTightWeightEntryType::kNotApplied;
      else throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
	<< "Invalid Configuration parameter 'applyFitFunction_or_graph_tau2' = " << applyFitFunction_or_graph_tau2_string << " !!\n";
    }
    double fitFunctionShapePower_tau2 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<double>("fitFunctionShapePower_tau2");

    std::string inputFileName = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("inputFileName");
    TFile* inputFile = new TFile(inputFileName.data());

    int numTau1EtaBins = tau1EtaBins.size() - 1;
    for ( int idxTau1EtaBin = 0; idxTau1EtaBin < numTau1EtaBins; ++idxTau1EtaBin ) {
      double tau1EtaMin = tau1EtaBins[idxTau1EtaBin];
      double tau1EtaMax = tau1EtaBins[idxTau1EtaBin + 1];

      int numTau2EtaBins = tau2EtaBins.size() - 1;
      for ( int idxTau2EtaBin = 0; idxTau2EtaBin < numTau2EtaBins; ++idxTau2EtaBin ) {
	double tau2EtaMin = tau2EtaBins[idxTau2EtaBin];
	double tau2EtaMax = tau2EtaBins[idxTau2EtaBin + 1];

        particleIDlooseToTightWeightEntryType* jetToTauFakeRateLooseToTightWeight = new particleIDlooseToTightWeightEntryType(
          inputFile,
	  "tau", tau1EtaMin, tau1EtaMax, tau2EtaMin, tau2EtaMax,
	  fitFunctionNormName, 
	  graphShapeName_tau1, fitFunctionShapeName_tau1_central, fitFunctionShapeName_tau1_shift, applyFitFunction_or_graph_tau1, fitFunctionShapePower_tau1, 
	  graphShapeName_tau2, fitFunctionShapeName_tau2_central, fitFunctionShapeName_tau2_shift, applyFitFunction_or_graph_tau2, fitFunctionShapePower_tau2);
	jetToTauFakeRateLooseToTightWeights.push_back(jetToTauFakeRateLooseToTightWeight);
      }
    }
    delete inputFile;
  }

  TFormula* jetToTauFakeRateCorrection = 0;
  bool applyJetToTauFakeRateCorrection = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<bool>("applyJetToTauFakeRateCorrection");
  if ( applyJetToTauFakeRateCorrection ) {
    std::string jetToTauFakeRateCorrection_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("jetToTauFakeRateCorrection");
    jetToTauFakeRateCorrection = new TFormula("jetToTauFakeRateCorrection", jetToTauFakeRateCorrection_string.data());
  }

  vstring addWeights_string = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<vstring>("addWeights");

  std::string central_or_shift = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("central_or_shift");
  bool applyHighPtTauIDeffUncertaintyUp        = (central_or_shift == "CMS_eff_t_mssmHigh_tautau_8TeVUp");
  bool applyHighPtTauIDeffUncertaintyDown      = (central_or_shift == "CMS_eff_t_mssmHigh_tautau_8TeVDown");
  bool applyHighPtTauTriggerEffUncertaintyUp   = (central_or_shift == "CMS_htt_eff_trig_mssmHigh_tautau_8TeVUp");
  bool applyHighPtTauTriggerEffUncertaintyDown = (central_or_shift == "CMS_htt_eff_trig_mssmHigh_tautau_8TeVDown");
  bool applyJetEnergyScaleShapeUp              = (central_or_shift == "CMS_scale_j_shape_8TeVUp");
  bool applyJetEnergyScaleShapeDown            = (central_or_shift == "CMS_scale_j_shape_8TeVDown");
  UInt_t bJetEff_shift = BtagSF::kNo;
  if ( central_or_shift == "CMS_eff_b_8TeVUp"    ) bJetEff_shift    = BtagSF::kUp;
  if ( central_or_shift == "CMS_eff_b_8TeVDown"  ) bJetEff_shift    = BtagSF::kDown;
  UInt_t bJetMistag_shift = BtagSF::kNo;
  if ( central_or_shift == "CMS_fake_b_8TeVUp"   ) bJetMistag_shift = BtagSF::kUp;
  if ( central_or_shift == "CMS_fake_b_8TeVDown" ) bJetMistag_shift = BtagSF::kDown;

  edm::FileInPath jetCorrInputFileName = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<edm::FileInPath>("jetCorrInputFileName");
  if ( jetCorrInputFileName.location() == edm::FileInPath::Unknown ) 
    throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
      << " Failed to find JEC parameter file = " << jetCorrInputFileName << " !!\n";
  std::string jetCorrUncertaintyTag = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("jetCorrUncertaintyTag");
  std::cout << "Reading JEC parameters = " << jetCorrUncertaintyTag
	    << " from file = " << jetCorrInputFileName.fullPath() << "." << std::endl;
  JetCorrectorParameters* jetCorrParameters = new JetCorrectorParameters(jetCorrInputFileName.fullPath().data(), jetCorrUncertaintyTag);
  JetCorrectionUncertainty* jecUncertainty = new JetCorrectionUncertainty(*jetCorrParameters);

  std::vector<TFile*> inputFilesToDelete;

  std::string selEventsFileName_input = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("selEventsFileName_input");
  std::cout << "selEventsFileName_input = " << selEventsFileName_input << std::endl;
  RunLumiSectionEventNumberSelector* runLumiSectionEventNumberSelector = 0;
  if ( selEventsFileName_input != "" ) {
    edm::ParameterSet cfgRunLumiSectionEventNumberSelector;
    cfgRunLumiSectionEventNumberSelector.addParameter<std::string>("inputFileName", selEventsFileName_input);
    cfgRunLumiSectionEventNumberSelector.addParameter<std::string>("separator", ":");
    runLumiSectionEventNumberSelector = new RunLumiSectionEventNumberSelector(cfgRunLumiSectionEventNumberSelector);
  }

  std::string selEventsFileName_output = cfgFWLiteTauTauAnalyzer2b2tau.getParameter<std::string>("selEventsFileName_output");
  std::ostream* selEventsFile_inclusive = 0;
  std::ostream* selEventsFile_2bM       = 0;
  std::ostream* selEventsFile_2bL       = 0;
  std::ostream* selEventsFile_1b1j      = 0;
  std::ostream* selEventsFile_2j        = 0;
  if ( selEventsFileName_output != "" ) {
    selEventsFile_inclusive = openSelEventsFile_output(selEventsFileName_output, Form("inclusive_%s", discriminator.data()));
    selEventsFile_2bM       = openSelEventsFile_output(selEventsFileName_output, Form("2bM_%s", discriminator.data()));
    selEventsFile_2bL       = openSelEventsFile_output(selEventsFileName_output, Form("2bL_%s", discriminator.data()));
    selEventsFile_1b1j      = openSelEventsFile_output(selEventsFileName_output, Form("1b1j_%s", discriminator.data()));
    selEventsFile_2j        = openSelEventsFile_output(selEventsFileName_output, Form("2j_%s", discriminator.data()));
  }  

  fwlite::InputSource inputFiles(cfg); 
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  edm::ParameterSet cfgHistManager_inclusive = makeHistManagerConfig(
    process_string, Form("tauTau_%s_inclusive_%s", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_inclusive(cfgHistManager_inclusive);
  histManager_inclusive.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_2bM = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bM_%s", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bM(cfgHistManager_2bM);
  histManager_2bM.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2bM_nonresonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bM_%s_nonresonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bM_nonresonant(cfgHistManager_2bM_nonresonant);
  histManager_2bM_nonresonant.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2bM_resonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bM_%s_resonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bM_resonant(cfgHistManager_2bM_resonant);
  histManager_2bM_resonant.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_2bL = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bL_%s", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bL(cfgHistManager_2bL);
  histManager_2bL.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2bL_nonresonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bL_%s_nonresonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bL_nonresonant(cfgHistManager_2bL_nonresonant);
  histManager_2bL_nonresonant.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2bL_resonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2bL_%s_resonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2bL_resonant(cfgHistManager_2bL_resonant);
  histManager_2bL_resonant.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_1b1j = makeHistManagerConfig(
    process_string, Form("tauTau_%s_1b1j_%s", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_1b1j(cfgHistManager_1b1j);
  histManager_1b1j.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_1b1j_nonresonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_1b1j_%s_nonresonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_1b1j_nonresonant(cfgHistManager_1b1j_nonresonant);
  histManager_1b1j_nonresonant.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_1b1j_resonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_1b1j_%s_resonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_1b1j_resonant(cfgHistManager_1b1j_resonant);
  histManager_1b1j_resonant.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_2j = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2j_%s", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2j(cfgHistManager_2j);
  histManager_2j.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2j_nonresonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2j_%s_nonresonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2j_nonresonant(cfgHistManager_2j_nonresonant);
  histManager_2j_nonresonant.bookHistograms(fs);
  edm::ParameterSet cfgHistManager_2j_resonant = makeHistManagerConfig(
    process_string, Form("tauTau_%s_2j_%s_resonant", region.data(), discriminator.data()), tauPtBin, tau1EtaBins, tau2EtaBins, central_or_shift);
  TauTauHistManager2b2tau histManager_2j_resonant(cfgHistManager_2j_resonant);
  histManager_2j_resonant.bookHistograms(fs);

  TChain* inputTree = new TChain(treeName.data());
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end(); ++inputFileName ) {
    std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
    inputTree->AddFile(inputFileName->data());
  }
  
  if ( !(inputTree->GetListOfFiles()->GetEntries() >= 1) ) {
    throw cms::Exception("FWLiteTauTauAnalyzer2b2tau") 
      << "Failed to identify input Tree !!\n";
  }

  // CV: need to call TChain::LoadTree before processing first event 
  //     in order to prevent ROOT causing a segmentation violation,
  //     cf. http://root.cern.ch/phpBB3/viewtopic.php?t=10062
  inputTree->LoadTree(0);

  std::cout << "input Tree contains " << inputTree->GetEntries() << " Entries in " << inputTree->GetListOfFiles()->GetEntries() << " files." << std::endl;

  ULong_t run, event, lumi;
  inputTree->SetBranchAddress("run", &run);
  inputTree->SetBranchAddress("event", &event);
  inputTree->SetBranchAddress("lumi", &lumi);

  Float_t tau1Pt, tau1Eta, tau1Phi, tau1Px, tau1Py, tau1Pz, tau1E;
  inputTree->SetBranchAddress("l1Pt", &tau1Pt);
  inputTree->SetBranchAddress("l1Eta", &tau1Eta);
  inputTree->SetBranchAddress("l1Phi", &tau1Phi);
  inputTree->SetBranchAddress("l1Px", &tau1Px);
  inputTree->SetBranchAddress("l1Py", &tau1Py);
  inputTree->SetBranchAddress("l1Pz", &tau1Pz);
  inputTree->SetBranchAddress("l1E", &tau1E);
  Float_t tau1DecayMode;
  inputTree->SetBranchAddress("l1DecayMode", &tau1DecayMode);
  Float_t tau1rawMVA, tau1IsoPtSum;
  inputTree->SetBranchAddress("l1RawMVAwLT", &tau1rawMVA);
  inputTree->SetBranchAddress("l1RawDB3HIso", &tau1IsoPtSum);
  Float_t tau1JetPt;
  inputTree->SetBranchAddress("l1CorrJetPt", &tau1JetPt);
  Int_t tau1IsTriggerMatched_diTau, tau1IsTriggerMatched_singleJet;
  inputTree->SetBranchAddress("l1TrigMatched_diTau", &tau1IsTriggerMatched_diTau);
  inputTree->SetBranchAddress("l1TrigMatched_singleJet", &tau1IsTriggerMatched_singleJet);

  TTreeFormula* tau1Selection = 0;
  if ( tau1Selection_string != "" ) {
    tau1Selection = new TTreeFormula("tau1Selection", tau1Selection_string.data(), inputTree);
  }

  Float_t tau2Pt, tau2Eta, tau2Phi, tau2Px, tau2Py, tau2Pz, tau2E;
  inputTree->SetBranchAddress("l2Pt", &tau2Pt);
  inputTree->SetBranchAddress("l2Eta", &tau2Eta);
  inputTree->SetBranchAddress("l2Phi", &tau2Phi);
  inputTree->SetBranchAddress("l2Px", &tau2Px);
  inputTree->SetBranchAddress("l2Py", &tau2Py);
  inputTree->SetBranchAddress("l2Pz", &tau2Pz);
  inputTree->SetBranchAddress("l2E", &tau2E);
  Float_t tau2DecayMode;
  inputTree->SetBranchAddress("l2DecayMode", &tau2DecayMode);
  Float_t tau2rawMVA, tau2IsoPtSum;
  inputTree->SetBranchAddress("l2RawMVAwLT", &tau2rawMVA);
  inputTree->SetBranchAddress("l2RawDB3HIso", &tau2IsoPtSum);
  Float_t tau2JetPt;
  inputTree->SetBranchAddress("l2CorrJetPt", &tau2JetPt);
  Int_t tau2IsTriggerMatched_diTau, tau2IsTriggerMatched_singleJet;
  inputTree->SetBranchAddress("l2TrigMatched_diTau", &tau2IsTriggerMatched_diTau);
  inputTree->SetBranchAddress("l2TrigMatched_singleJet", &tau2IsTriggerMatched_singleJet);

  TTreeFormula* tau2Selection = 0;
  if ( tau2Selection_string != "" ) {
    tau2Selection = new TTreeFormula("tau2Selection", tau2Selection_string.data(), inputTree);
  }

  Float_t diTauCharge;
  inputTree->SetBranchAddress("diTauCharge", &diTauCharge);
  Float_t diTauGenMass;
  if ( process == kSignal ) inputTree->SetBranchAddress("genMass", &diTauGenMass);
  Float_t dPhitt, dEtatt, dRtt;
  inputTree->SetBranchAddress("dPhitt", &dPhitt);
  inputTree->SetBranchAddress("dEtatt", &dEtatt);
  inputTree->SetBranchAddress("dRtt", &dRtt);
  Float_t visMass, svFitMass, mTtotal;
  inputTree->SetBranchAddress("visMass", &visMass);
  inputTree->SetBranchAddress("svfitMass", &svFitMass);
  inputTree->SetBranchAddress("mTtotal", &mTtotal);

  Float_t electron1Pt, electron1Eta, electron1Phi;
  inputTree->SetBranchAddress("electron1Pt", &electron1Pt);
  inputTree->SetBranchAddress("electron1Eta", &electron1Eta);
  inputTree->SetBranchAddress("electron1Phi", &electron1Phi);

  Float_t muon1Pt, muon1Eta, muon1Phi;
  inputTree->SetBranchAddress("muon1Pt", &muon1Pt);
  inputTree->SetBranchAddress("muon1Eta", &muon1Eta);
  inputTree->SetBranchAddress("muon1Phi", &muon1Phi);

  Float_t jet1Pt, jet1Eta, jet1Phi, jet1Px, jet1Py, jet1Pz, jet1E;
  inputTree->SetBranchAddress("jet1Pt", &jet1Pt);
  inputTree->SetBranchAddress("jet1Eta", &jet1Eta);
  inputTree->SetBranchAddress("jet1Phi", &jet1Phi);
  inputTree->SetBranchAddress("jet1Px", &jet1Px);
  inputTree->SetBranchAddress("jet1Py", &jet1Py);
  inputTree->SetBranchAddress("jet1Pz", &jet1Pz);
  inputTree->SetBranchAddress("jet1E", &jet1E);
  Float_t jet1BtagDiscr;
  inputTree->SetBranchAddress("jet1Btag", &jet1BtagDiscr);

  Float_t jet2Pt, jet2Eta, jet2Phi, jet2Px, jet2Py, jet2Pz, jet2E;
  inputTree->SetBranchAddress("jet2Pt", &jet2Pt);
  inputTree->SetBranchAddress("jet2Eta", &jet2Eta);
  inputTree->SetBranchAddress("jet2Phi", &jet2Phi);
  inputTree->SetBranchAddress("jet2Px", &jet2Px);
  inputTree->SetBranchAddress("jet2Py", &jet2Py);
  inputTree->SetBranchAddress("jet2Pz", &jet2Pz);
  inputTree->SetBranchAddress("jet2E", &jet2E);
  Float_t jet2BtagDiscr;
  inputTree->SetBranchAddress("jet2Btag", &jet2BtagDiscr);

  Float_t jet3Pt, jet3Eta, jet3Phi, jet3Px, jet3Py, jet3Pz, jet3E;
  inputTree->SetBranchAddress("jet3Pt", &jet3Pt);
  inputTree->SetBranchAddress("jet3Eta", &jet3Eta);
  inputTree->SetBranchAddress("jet3Phi", &jet3Phi);
  inputTree->SetBranchAddress("jet3Px", &jet3Px);
  inputTree->SetBranchAddress("jet3Py", &jet3Py);
  inputTree->SetBranchAddress("jet3Pz", &jet3Pz);
  inputTree->SetBranchAddress("jet3E", &jet3E);
  Float_t jet3BtagDiscr;
  inputTree->SetBranchAddress("jet3Btag", &jet3BtagDiscr);

  Float_t jet4Pt, jet4Eta, jet4Phi, jet4Px, jet4Py, jet4Pz, jet4E;
  inputTree->SetBranchAddress("jet4Pt", &jet4Pt);
  inputTree->SetBranchAddress("jet4Eta", &jet4Eta);
  inputTree->SetBranchAddress("jet4Phi", &jet4Phi);
  inputTree->SetBranchAddress("jet4Px", &jet4Px);
  inputTree->SetBranchAddress("jet4Py", &jet4Py);
  inputTree->SetBranchAddress("jet4Pz", &jet4Pz);
  inputTree->SetBranchAddress("jet4E", &jet4E);
  Float_t jet4BtagDiscr;
  inputTree->SetBranchAddress("jet4Btag", &jet4BtagDiscr);

  Int_t nJets;
  inputTree->SetBranchAddress("nJets", &nJets);

  Float_t bjet1Pt, bjet1Eta, bjet1Phi, bjet1Px, bjet1Py, bjet1Pz, bjet1E;
  inputTree->SetBranchAddress("bjet1Pt", &bjet1Pt);
  inputTree->SetBranchAddress("bjet1Eta", &bjet1Eta);
  inputTree->SetBranchAddress("bjet1Phi", &bjet1Phi);
  inputTree->SetBranchAddress("bjet1Px", &bjet1Px);
  inputTree->SetBranchAddress("bjet1Py", &bjet1Py);
  inputTree->SetBranchAddress("bjet1Pz", &bjet1Pz);
  inputTree->SetBranchAddress("bjet1E", &bjet1E);
  Float_t bjet1EnRegFactor;
  inputTree->SetBranchAddress("bjet1EnRegFactor", &bjet1EnRegFactor);
  Float_t bjet1BtagDiscr;
  inputTree->SetBranchAddress("bjet1Btag", &bjet1BtagDiscr);
  Int_t bjet1GenPartonFlavour;
  inputTree->SetBranchAddress("bjet1GenPartonFlavour", &bjet1GenPartonFlavour);

  Float_t bjet2Pt, bjet2Eta, bjet2Phi, bjet2Px, bjet2Py, bjet2Pz, bjet2E;
  inputTree->SetBranchAddress("bjet2Pt", &bjet2Pt);
  inputTree->SetBranchAddress("bjet2Eta", &bjet2Eta);
  inputTree->SetBranchAddress("bjet2Phi", &bjet2Phi);
  inputTree->SetBranchAddress("bjet2Px", &bjet2Px);
  inputTree->SetBranchAddress("bjet2Py", &bjet2Py);
  inputTree->SetBranchAddress("bjet2Pz", &bjet2Pz);
  inputTree->SetBranchAddress("bjet2E", &bjet2E);
  Float_t bjet2EnRegFactor;
  inputTree->SetBranchAddress("bjet2EnRegFactor", &bjet2EnRegFactor);
  Float_t bjet2BtagDiscr;
  inputTree->SetBranchAddress("bjet2Btag", &bjet2BtagDiscr);
  Int_t bjet2GenPartonFlavour;
  inputTree->SetBranchAddress("bjet2GenPartonFlavour", &bjet2GenPartonFlavour);

  Float_t bjet3Pt, bjet3Eta, bjet3Phi, bjet3Px, bjet3Py, bjet3Pz, bjet3E;
  inputTree->SetBranchAddress("bjet3Pt", &bjet3Pt);
  inputTree->SetBranchAddress("bjet3Eta", &bjet3Eta);
  inputTree->SetBranchAddress("bjet3Phi", &bjet3Phi);
  inputTree->SetBranchAddress("bjet3Px", &bjet3Px);
  inputTree->SetBranchAddress("bjet3Py", &bjet3Py);
  inputTree->SetBranchAddress("bjet3Pz", &bjet3Pz);
  inputTree->SetBranchAddress("bjet3E", &bjet3E);
  Float_t bjet3EnRegFactor;
  inputTree->SetBranchAddress("bjet3EnRegFactor", &bjet3EnRegFactor);
  Float_t bjet3BtagDiscr;
  inputTree->SetBranchAddress("bjet3Btag", &bjet3BtagDiscr);
  Int_t bjet3GenPartonFlavour;
  inputTree->SetBranchAddress("bjet3GenPartonFlavour", &bjet3GenPartonFlavour);

  Float_t bjet4Pt, bjet4Eta, bjet4Phi, bjet4Px, bjet4Py, bjet4Pz, bjet4E;
  inputTree->SetBranchAddress("bjet4Pt", &bjet4Pt);
  inputTree->SetBranchAddress("bjet4Eta", &bjet4Eta);
  inputTree->SetBranchAddress("bjet4Phi", &bjet4Phi);
  inputTree->SetBranchAddress("bjet4Px", &bjet4Px);
  inputTree->SetBranchAddress("bjet4Py", &bjet4Py);
  inputTree->SetBranchAddress("bjet4Pz", &bjet4Pz);
  inputTree->SetBranchAddress("bjet4E", &bjet4E);
  Float_t bjet4EnRegFactor;
  inputTree->SetBranchAddress("bjet4EnRegFactor", &bjet4EnRegFactor);
  Float_t bjet4BtagDiscr;
  inputTree->SetBranchAddress("bjet4Btag", &bjet4BtagDiscr);
  Int_t bjet4GenPartonFlavour;
  inputTree->SetBranchAddress("bjet4GenPartonFlavour", &bjet4GenPartonFlavour);

  Float_t met, mex, mey;
  inputTree->SetBranchAddress("met", &met);
  inputTree->SetBranchAddress("mex", &mex);
  inputTree->SetBranchAddress("mey", &mey);
  Float_t metCov00, metCov01, metCov10, metCov11;
  inputTree->SetBranchAddress("mvacov00", &metCov00);
  inputTree->SetBranchAddress("mvacov01", &metCov01);
  inputTree->SetBranchAddress("mvacov10", &metCov10);
  inputTree->SetBranchAddress("mvacov11", &metCov11);

  Float_t genMEx, genMEy;  
  inputTree->SetBranchAddress("genMEx", &genMEx);
  inputTree->SetBranchAddress("genMEy", &genMEy);

  Float_t HttPx, HttPy, HttPz, HttE;
  inputTree->SetBranchAddress("svfitMarkovChainPx", &HttPx);
  inputTree->SetBranchAddress("svfitMarkovChainPy", &HttPy);
  inputTree->SetBranchAddress("svfitMarkovChainPz", &HttPz);
  inputTree->SetBranchAddress("svfitMarkovChainE", &HttE);

  //Float_t augMT2ed;
  //inputTree->SetBranchAddress("augMT2ed", &augMT2ed);

  Int_t numVertices;
  inputTree->SetBranchAddress("nVert", &numVertices);

  Int_t isZtt, isZttj, isZttl, isZj, isZee, isZmm;
  inputTree->SetBranchAddress("isZtt", &isZtt);
  inputTree->SetBranchAddress("isZttj", &isZttj);
  inputTree->SetBranchAddress("isZttl", &isZttl);
  inputTree->SetBranchAddress("isZj", &isZj);
  inputTree->SetBranchAddress("isZee", &isZee);
  inputTree->SetBranchAddress("isZmm", &isZmm);
  Int_t l1isGenHadTau, l1isGenMuon, l1isGenElectron, l1isGenJet;
  inputTree->SetBranchAddress("l1isGenHadTau", &l1isGenHadTau);
  inputTree->SetBranchAddress("l1isGenMuon", &l1isGenMuon);
  inputTree->SetBranchAddress("l1isGenElectron", &l1isGenElectron);
  inputTree->SetBranchAddress("l1isGenJet", &l1isGenJet);
  Int_t l2isGenHadTau, l2isGenMuon, l2isGenElectron, l2isGenJet;
  inputTree->SetBranchAddress("l2isGenHadTau", &l2isGenHadTau);
  inputTree->SetBranchAddress("l2isGenMuon", &l2isGenMuon);
  inputTree->SetBranchAddress("l2isGenElectron", &l2isGenElectron);
  inputTree->SetBranchAddress("l2isGenJet", &l2isGenJet);
  
  Float_t NUP;
  inputTree->SetBranchAddress("NUP", &NUP);

  Float_t triggerEffData_diTau, triggerWeight_diTau;
  if ( applyTauTriggerTurnOn == kTree ) {
    inputTree->SetBranchAddress("triggerEffData_diTau", &triggerEffData_diTau);
    inputTree->SetBranchAddress("triggerWeight_diTau", &triggerWeight_diTau);
  }
  Float_t pileupWeight = 1.0;
  if ( isMC ) inputTree->SetBranchAddress("vertexWeight", &pileupWeight);
  Float_t embeddingWeight = 1.0;
  if ( isEmbedded ) inputTree->SetBranchAddress("genFilter", &embeddingWeight);

  std::vector<weightEntryType*> addWeights;
  for ( vstring::const_iterator addWeight_string = addWeights_string.begin();
	addWeight_string != addWeights_string.end(); ++addWeight_string ) {
    weightEntryType* addWeight = new weightEntryType(inputTree, *addWeight_string);
    addWeights.push_back(addWeight);
  }

  mt2Interface* augMT2edAlgo = new mt2Interface();
    
  int currentTreeNumber = inputTree->GetTreeNumber();
  
  int numEntries = inputTree->GetEntries();
  int analyzedEntries = 0;
  int selectedEntries_inclusive = 0;
  double selectedEntriesWeighted_inclusive = 0.;
  int selectedEntries_2bM = 0;
  double selectedEntriesWeighted_2bM = 0.;
  int selectedEntries_2bL = 0;
  double selectedEntriesWeighted_2bL = 0.;  
  int selectedEntries_1b1j = 0;
  double selectedEntriesWeighted_1b1j = 0.;
  int selectedEntries_2j = 0;
  double selectedEntriesWeighted_2j = 0.;
  for ( int iEntry = 0; iEntry < numEntries && (maxEvents == -1 || iEntry < maxEvents); ++iEntry ) {
    if ( iEntry > 0 && (iEntry % reportEvery) == 0 ) {
      std::cout << "processing Entry " << iEntry << " (" << selectedEntries_inclusive << " Entries selected in inclusive category)" << std::endl;
    }
    ++analyzedEntries;
    
    inputTree->GetEntry(iEntry);

    //if ( runLumiSectionEventNumberSelector && !(*runLumiSectionEventNumberSelector)(run, lumi, event) ) continue;

    // CV: general phase-space cuts (driven by trigger requirements)
    if ( !(TMath::Abs(tau1Eta) < 2.1 && tau1Pt > 45. && 
	   TMath::Abs(tau2Eta) < 2.1 && tau2Pt > 45.) ) continue;

    if ( tau1Selection || tau2Selection ) {
      // CV: need to call TTreeFormula::UpdateFormulaLeaves whenever input files changes in TChain
      //     in order to prevent ROOT causing a segmentation violation,
      //     cf. http://root.cern.ch/phpBB3/viewtopic.php?t=481
      if ( inputTree->GetTreeNumber() != currentTreeNumber ) {
	if ( tau1Selection ) tau1Selection->UpdateFormulaLeaves();
	if ( tau2Selection ) tau2Selection->UpdateFormulaLeaves();
	currentTreeNumber = inputTree->GetTreeNumber();
      }   

      if ( tau1Selection && !(tau1Selection->EvalInstance() > 0.5) ) continue;
      if ( tau2Selection && !(tau2Selection->EvalInstance() > 0.5) ) continue;
    }

    if ( !isEmbedded ) {
      bool isTriggerMatched = false;
      if ( (trigger == kTau || trigger == kTauPlusJet) &&   
	   tau1Pt < tauPtForSwitchingTriggers && tau1IsTriggerMatched_diTau &&  
	   tau2Pt < tauPtForSwitchingTriggers && tau2IsTriggerMatched_diTau ) isTriggerMatched = true;
      if ( (trigger == kJet || trigger == kTauPlusJet) && 
	   ((tau1Pt > tauPtForSwitchingTriggers && tau1IsTriggerMatched_singleJet) || 
	    (tau2Pt > tauPtForSwitchingTriggers && tau2IsTriggerMatched_singleJet)) ) isTriggerMatched = true;
      if ( !isTriggerMatched ) continue;
    }
 
    if ( diTauChargeSel == kOS && TMath::Abs(diTauCharge) > 0.5 ) continue;
    if ( diTauChargeSel == kSS && TMath::Abs(diTauCharge) < 0.5 ) continue;
   
    int nElectrons = 0;
    if ( electron1Pt > 10. && TMath::Abs(electron1Eta) < 2.5 ) ++nElectrons;

    int nMuons = 0;
    if ( muon1Pt > 10. && TMath::Abs(muon1Eta) < 2.4 ) ++nMuons;

    if ( nElectrons > 0 || nMuons > 0 ) continue;

    //---------------------------------------------------------------------------
    // CV: process dependent selection criteria

    if ( (process == kZTTmc || process == kZTT_Embedded) && !isZtt ) continue;    
    bool isZL = (l1isGenElectron || l1isGenMuon) && (l2isGenElectron || l2isGenMuon);
    if ( process == kZL && !isZL ) continue;
    bool is2RealTaus = l1isGenHadTau && l2isGenHadTau;
    bool isZJ = !(is2RealTaus || isZL);
    if ( process == kZJ && !isZJ ) continue;

    if ( process == kTT_Embedded && !is2RealTaus ) continue;
    //---------------------------------------------------------------------------

    //---------------------------------------------------------------------------
    // CV: apply tau trigger efficiency turn-on curves measured by Arun for different tau ID discriminators
    
    typedef double (*triggerEffPtr)(double, double, bool);
    triggerEffPtr tauTriggerEffData = 0;
    triggerEffPtr tauTriggerEffMC   = 0;
    typedef double (*triggerEffDropPtr)(double, double);
    triggerEffDropPtr tauTriggerEffDrop = 0;
    if ( applyTauTriggerTurnOn == kHPScombIso3HitsMedium ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_cutMedium;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_cutMedium;
      tauTriggerEffDrop = effDrop2012IsoParkedTau_Arun_cutMedium;
    } else if ( applyTauTriggerTurnOn == kMVAwLToldDMsLoose || applyTauTriggerTurnOn == kMVAwLToldDMsMedium || applyTauTriggerTurnOn == kMVAwLToldDMsTight ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaTight;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaTight;      
      tauTriggerEffDrop = effDrop2012IsoParkedTau_Arun_mvaTight;    
    } else if ( applyTauTriggerTurnOn == kMVAwLToldDMsVTight ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaVTight;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaVTight; 
      tauTriggerEffDrop = effDrop2012IsoParkedTau_Arun_mvaVTight;          
    } 
    if ( applyTauTriggerTurnOn != kTree ) {
      assert(tauTriggerEffData);
      assert(tauTriggerEffMC);
      triggerEffData_diTau = (*tauTriggerEffData)(tau1Pt, tau1Eta, false)*(*tauTriggerEffData)(tau2Pt, tau2Eta, false);
      double triggerEffDropData_diTau = ((7.3 + 11.0*(*tauTriggerEffDrop)(tau1Pt, tau1Eta))/18.3)*((7.3 + 11.0*(*tauTriggerEffDrop)(tau2Pt, tau2Eta))/18.3);
      double triggerEffMC_diTau = (*tauTriggerEffMC)(tau1Pt, tau1Eta, false)*(*tauTriggerEffMC)(tau2Pt, tau2Eta, false);
      double triggerEffDropMC_diTau = (*tauTriggerEffDrop)(tau1Pt, tau1Eta)*(*tauTriggerEffDrop)(tau2Pt, tau2Eta);
      triggerWeight_diTau = ( triggerEffMC_diTau > 0. ) ? (triggerEffData_diTau/triggerEffMC_diTau) : 1.;
      double triggerEffDrop_power = 1.;
      if      ( applyHighPtTauTriggerEffUncertaintyUp   ) triggerEffDrop_power = 2.;
      else if ( applyHighPtTauTriggerEffUncertaintyDown ) triggerEffDrop_power = 0.;
      if ( triggerEffDropMC_diTau > 0. ) triggerWeight_diTau *= TMath::Power(triggerEffDropData_diTau/triggerEffDropMC_diTau, triggerEffDrop_power);
    }
    //---------------------------------------------------------------------------

    Float_t evtWeight = 1.0;
    Float_t evtWeightErr2_relative = 0.;
    if ( isMC || isEmbedded ) {      
      if ( isEmbedded ) {
	if ( tau1Pt > tauPtForSwitchingTriggers || tau2Pt > tauPtForSwitchingTriggers ) {
	  double triggerEffData_singleJet = (1. - (1. - effPFJet320(tau1Pt, tau1JetPt, tau1Eta))*(1. - effPFJet320(tau2Pt, tau2JetPt, tau2Eta)));
	  evtWeight *= triggerEffData_singleJet;
	} else evtWeight *= triggerEffData_diTau;
	evtWeight *= embeddingWeight;
      } else if ( isMC ) {
	if ( tau1Pt > tauPtForSwitchingTriggers || tau2Pt > tauPtForSwitchingTriggers ) {
	  double triggerEffData_singleJet = (1. - (1. - effPFJet320(tau1Pt, tau1JetPt, tau1Eta))*(1. - effPFJet320(tau2Pt, tau2JetPt, tau2Eta)));
	  double triggerEffMC_singleJet   = (1. - (1. - effPFJet320MC(tau1Pt, tau1JetPt, tau1Eta))*(1. - effPFJet320MC(tau2Pt, tau2JetPt, tau2Eta)));
	  double triggerWeight_singleJet  = ( triggerEffMC_singleJet > 0. ) ? (triggerEffData_singleJet/triggerEffMC_singleJet) : 1.;
	  evtWeight *= triggerWeight_singleJet;
	} else evtWeight *= triggerWeight_diTau;
      }
      if ( l1isGenHadTau ) evtWeight *= compTauDecayModeWeight(tau1Eta, TMath::Nint(tau1DecayMode));
      if ( l2isGenHadTau ) evtWeight *= compTauDecayModeWeight(tau2Eta, TMath::Nint(tau2DecayMode));
      if ( process == kZTTmc || process == kZL || process == kZJ || process == kW ) {
	Float_t stitchingWeight = 1.0;
	int idxNUP = TMath::Nint(NUP) - 5;
	if ( TMath::Nint(NUP) == 0 ) {	  
	  std::string inputFileName = inputTree->GetFile()->GetName();
	  if      ( inputFileName.find("/W1JetsExt/") != std::string::npos ) idxNUP = 1;
	  else if ( inputFileName.find("/W2JetsExt/") != std::string::npos ) idxNUP = 2;
	  else if ( inputFileName.find("/W3JetsExt/") != std::string::npos ) idxNUP = 3;
	  if ( idxNUP >= 0 ) {
	    std::cerr << "Warning: NUP not filled in Ntuple, setting idxNUP = " << idxNUP << ", using name of input file = " << inputFileName << " !!" << std::endl;
	  } 
	}
	if ( idxNUP >= 0 && idxNUP < (int)stitchingWeights.size() ) {
	  stitchingWeight *= stitchingWeights[idxNUP];
	} else {
	  std::cerr << "Warning: NUP = " << NUP << " outside range !!" << std::endl;	  
	}
	evtWeight *= stitchingWeight;
      }
      if ( applyHighPtTauIDeffUncertaintyUp ) {
	if ( l1isGenHadTau ) evtWeight *= (1.0 + 0.20*tau1Pt/1000.);
	if ( l2isGenHadTau ) evtWeight *= (1.0 + 0.20*tau2Pt/1000.);
      } else if ( applyHighPtTauIDeffUncertaintyDown ) {
	if ( l1isGenHadTau ) evtWeight *= (1.0 - 0.20*tau1Pt/1000.);
	if ( l2isGenHadTau ) evtWeight *= (1.0 - 0.20*tau2Pt/1000.);
      }
      if ( isMC ) {
	evtWeight *= lumiScale;
	evtWeight *= pileupWeight;
	if ( applyJetToTauFakeRateCorrection ) {
	  if ( l1isGenJet ) evtWeight *= jetToTauFakeRateCorrection->Eval(tau1Pt);
	  if ( l2isGenJet ) evtWeight *= jetToTauFakeRateCorrection->Eval(tau2Pt);
	}
      }
      for ( std::vector<weightEntryType*>::iterator addWeight = addWeights.begin();
	    addWeight != addWeights.end(); ++addWeight ) {
	evtWeight *= (*addWeight)->value_;
      }
    }
    if ( applyJetToTauFakeRateLooseToTightWeight ) {
      double absTau1Eta = TMath::Abs(tau1Eta);
      double absTau2Eta = TMath::Abs(tau2Eta);      
      particleIDlooseToTightWeightEntryType* jetToTauFakeRateLooseToTightWeight_tauEtaBin = 0;
      for ( std::vector<particleIDlooseToTightWeightEntryType*>::const_iterator jetToTauFakeRateLooseToTightWeight = jetToTauFakeRateLooseToTightWeights.begin();
	    jetToTauFakeRateLooseToTightWeight != jetToTauFakeRateLooseToTightWeights.end(); ++jetToTauFakeRateLooseToTightWeight ) {
	if ( ((*jetToTauFakeRateLooseToTightWeight)->particle1EtaMin_ < 0. || absTau1Eta > (*jetToTauFakeRateLooseToTightWeight)->particle1EtaMin_) &&
	     ((*jetToTauFakeRateLooseToTightWeight)->particle1EtaMax_ > 5. || absTau1Eta < (*jetToTauFakeRateLooseToTightWeight)->particle1EtaMax_) &&
	     ((*jetToTauFakeRateLooseToTightWeight)->particle2EtaMin_ < 0. || absTau2Eta > (*jetToTauFakeRateLooseToTightWeight)->particle2EtaMin_) &&
	     ((*jetToTauFakeRateLooseToTightWeight)->particle2EtaMax_ > 5. || absTau2Eta < (*jetToTauFakeRateLooseToTightWeight)->particle2EtaMax_) ) {
	  jetToTauFakeRateLooseToTightWeight_tauEtaBin = (*jetToTauFakeRateLooseToTightWeight);
	  break;
	}
      }
      if ( jetToTauFakeRateLooseToTightWeight_tauEtaBin ) {
	//std::cout << "tau1: Pt = " << tau1Pt << ", eta = " << tau1Eta << std::endl;
	//std::cout << "tau2: Pt = " << tau2Pt << ", eta = " << tau2Eta << std::endl;
	//std::cout << " --> jetToTauFakeRateLooseToTightWeight = " << (*jetToTauFakeRateLooseToTightWeight_tauEtaBin)(tau1Pt, tau2Pt) << std::endl;
	evtWeight *= jetToTauFakeRateLooseToTightWeight_tauEtaBin->weight(tau1Pt, tau2Pt);
        evtWeightErr2_relative += square(jetToTauFakeRateLooseToTightWeight_tauEtaBin->weightErr_relative(tau1Pt, tau2Pt));
      } else {
	std::cerr << "Warning: tau1Eta = " << tau1Eta << ", tau2Eta = " << tau2Eta << " outside range !!" << std::endl;
      }
    }
    Float_t evtWeightErr = evtWeight*TMath::Sqrt(evtWeightErr2_relative);

    //---------------------------------------------------------------------------
    // CV: process dependent  additional scale-factors
    double processSF_inclusive = 1.0;
    double processSF_nobtag    = 1.0;
    double processSF_btag      = 1.0;
    if ( process == kTT || process == kTT_Embedded ) {
      processSF_inclusive = 0.96; // CV: TTbar scale-factor obtained from emu sideband, provided by Valentina
      processSF_nobtag    = 0.96;
      processSF_btag      = 0.96;
    } 
    if ( process == kW ) {
      processSF_inclusive = 1.00; // CV: W+jets scale-factor determined by Olivier (compatible with 1.0 within uncertainties)
      processSF_nobtag    = 1.00;
      processSF_btag      = 1.00;
    }
    //---------------------------------------------------------------------------

    reco::Candidate::LorentzVector tau1P4(tau1Px, tau1Py, tau1Pz, tau1E);
    int tau1GenMatch = getGenMatch(l1isGenHadTau, l1isGenMuon, l1isGenElectron, l1isGenJet);
    reco::Candidate::LorentzVector tau2P4(tau2Px, tau2Py, tau2Pz, tau2E);
    int tau2GenMatch = getGenMatch(l2isGenHadTau, l2isGenMuon, l2isGenElectron, l2isGenJet);
 
    std::vector<jetType> bjets;
    addJet(bjets, 20., 0.244, bjet1Pt, bjet1Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      bjet1Px, bjet1Py, bjet1Pz, bjet1E, bjet1EnRegFactor, bjet1BtagDiscr, bjet1GenPartonFlavour);
    addJet(bjets, 20., 0.244, bjet2Pt, bjet2Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      bjet2Px, bjet2Py, bjet2Pz, bjet2E, bjet2EnRegFactor, bjet2BtagDiscr, bjet2GenPartonFlavour);
    addJet(bjets, 20., 0.244, bjet3Pt, bjet3Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      bjet3Px, bjet3Py, bjet3Pz, bjet3E, bjet3EnRegFactor, bjet3BtagDiscr, bjet3GenPartonFlavour);
    addJet(bjets, 20., 0.244, bjet4Pt, bjet4Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      bjet4Px, bjet4Py, bjet4Pz, bjet4E, bjet4EnRegFactor, bjet4BtagDiscr, bjet4GenPartonFlavour);
    for ( std::vector<jetType>::iterator bjet = bjets.begin();
	  bjet != bjets.end(); ++bjet ) {
      bjet->passesLooseWP_  = bjet->btagDiscr_ > 0.244;
      bjet->passesMediumWP_ = bTagSF->isbtagged(bjet->p4_.pt(), bjet->p4_.eta(), bjet->btagDiscr_, bjet->genPartonFlavour_, !isMC, bJetEff_shift, bJetMistag_shift, true);
      bjet->passesTightWP_  = bjet->passesMediumWP_ && bjet->btagDiscr_ > 0.898;
    }
    std::sort(bjets.begin(), bjets.end(), isHigherBtagDiscriminator);

    std::vector<jetType> jets;
    addJet(jets, 20., -1., jet1Pt, jet1Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      jet1Px, jet1Py, jet1Pz, jet1E, 1., jet1BtagDiscr, -1);
    addJet(jets, 20., -1., jet2Pt, jet2Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      jet2Px, jet2Py, jet2Pz, jet2E, 1., jet2BtagDiscr, -1);
    addJet(jets, 20., -1., jet3Pt, jet3Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      jet3Px, jet3Py, jet3Pz, jet3E, 1., jet3BtagDiscr, -1);
    addJet(jets, 20., -1., jet4Pt, jet4Eta, jecUncertainty, applyJetEnergyScaleShapeUp, applyJetEnergyScaleShapeDown, 
      jet4Px, jet4Py, jet4Pz, jet4E, 1., jet4BtagDiscr, -1);
    std::sort(jets.begin(), jets.end(), isHigherPt);

    int nbJetsLoose  = 0;
    int nbJetsMedium = 0;
    int nbJetsTight  = 0;
    for ( std::vector<jetType>::iterator bjet = bjets.begin();
	  bjet != bjets.end(); ++bjet ) {
      if ( bjet->passesLooseWP_  ) ++nbJetsLoose;
      if ( bjet->passesMediumWP_ ) ++nbJetsMedium;
      if ( bjet->passesTightWP_  ) ++nbJetsTight;
    }      
    reco::Candidate::LorentzVector bjet1P4;
    reco::Candidate::LorentzVector bjet1RegP4;
    bool bjet1_found = false;
    reco::Candidate::LorentzVector bjet2P4;    
    reco::Candidate::LorentzVector bjet2RegP4;
    bool bjet2_found = false;
    if ( bjets.size() >= 1 ) {
      bjet1P4 = bjets[0].p4_;
      bjet1RegP4 = bjets[0].p4Reg_;
      bjet1_found = true;
    }
    if ( bjets.size() >= 2 ) {
      bjet2P4 = bjets[1].p4_;
      bjet2RegP4 = bjets[1].p4Reg_;
      bjet2_found = true;
    }
    if ( !(bjet1_found && bjet2_found) ) {
      for ( std::vector<jetType>::iterator jet = jets.begin();
	    jet != jets.end(); ++jet ) {
	if ( !bjet1_found ) {
	  bjet1P4 = jet->p4_;
	  bjet1RegP4 = jet->p4Reg_;
	  bjet1_found = true;
	  continue;
	} 
	if ( bjet1_found && deltaR(bjet1P4, jet->p4_) < 0.3 ) continue;
	if ( !bjet2_found ) {
	  bjet2P4 = jet->p4_;
	  bjet2RegP4 = jet->p4Reg_;
	  bjet2_found = true;
	}
      }
    }
    if ( !(bjet1_found && bjet2_found) ) continue;

    reco::Candidate::LorentzVector HttP4(HttPx, HttPy, HttPz, HttE);
    reco::Candidate::LorentzVector HbbP4 = bjet1P4 + bjet2P4; 
    reco::Candidate::LorentzVector HbbRegP4 = bjet1RegP4 + bjet2RegP4;
    reco::Candidate::LorentzVector metP4(mex, mey, met, met);
    TMatrixD metCov(2,2);
    metCov(0,0) = metCov00;
    metCov(0,1) = metCov01;
    metCov(1,0) = metCov10;
    metCov(1,1) = metCov11;
    reco::Candidate::LorentzVector HHP4 = HttP4 + HbbP4;
    reco::Candidate::LorentzVector HHbRegP4 = HttP4 + HbbRegP4;

    double augMT2ed = (*augMT2edAlgo)(bjet1P4, bjet2P4, tau1P4, tau2P4, metP4);
    //std::cout << "augMT2ed = " << augMT2ed << std::endl;

    double zetaX = tau1P4.px() + tau2P4.px();
    double zetaY = tau1P4.py() + tau2P4.py();
    double zetaR = TMath::Sqrt(zetaX*zetaX + zetaY*zetaY);
    if ( zetaR > 0. ) {
      zetaX /= zetaR;
      zetaY /= zetaR;
    }
    double visPx = tau1P4.px() + tau2P4.px();
    double visPy = tau1P4.py() + tau2P4.py();
    double pZetaVis = visPx*zetaX + visPy*zetaY;
    double px = visPx + mex;
    double py = visPy + mey;
    double pZeta = px*zetaX + py*zetaY;
    double pZetaComb = pZeta - pZetaVis;

    reco::Candidate::LorentzVector zetaP4(zetaX, zetaY, 0., zetaR);
    int errorFlag = 0;
    std::pair<double, double> deltaMEt = compMEtProjU(zetaP4, mex - genMEx, mey - genMEy, errorFlag, false);
    double deltaMEtParl = deltaMEt.first;
    double deltaMEtPerp = deltaMEt.second;

    double dPhibb     = normalizedPhi(bjet1P4.phi() - bjet2P4.phi());
    double dEtabb     = TMath::Abs(bjet1P4.eta() - bjet2P4.eta());
    double dRbb       = deltaR(bjet1P4, bjet2P4);
    double mbb        = HbbP4.mass(); 
    double mbbReg     = HbbRegP4.mass(); 
    double dPhiHbbHtt = normalizedPhi(HbbP4.phi() - HttP4.phi());
    double dPhiHbbMEt = normalizedPhi(HbbP4.phi() - metP4.phi());
    double dPhiHttMEt = normalizedPhi(HttP4.phi() - metP4.phi());
    double minDRbt = 9.9;
    minDRbt = TMath::Min(minDRbt, deltaR(tau1P4, bjet1P4));
    minDRbt = TMath::Min(minDRbt, deltaR(tau1P4, bjet2P4));
    minDRbt = TMath::Min(minDRbt, deltaR(tau2P4, bjet1P4));
    minDRbt = TMath::Min(minDRbt, deltaR(tau2P4, bjet2P4));
    double HbbPt      = HbbP4.pt();
    double HbbRegPt   = HbbRegP4.pt();
    double HttPt      = HttP4.pt();
    double HHPt       = HHP4.pt();
    double HHMass     = HHP4.mass();
    double HHbRegPt   = HHbRegP4.pt();
    double HHbRegMass = HHbRegP4.mass();
    
    //std::cout << "calling compKinFit2bdy without b-jet energy regression" << std::endl;
    kinFitResult kinFitResult_2bdy = compKinFit2bdy(bjet1P4, bjet2P4, HttP4);
    double HH2bdyKinFitMass = kinFitResult_2bdy.mhh_;
    double HH2bdyKinFitChi2 = kinFitResult_2bdy.chi2_;
    //std::cout << "calling compKinFit2bdy with b-jet energy regression" << std::endl;
    kinFitResult kinFitResult_bReg_2bdy = compKinFit2bdy(bjet1RegP4, bjet2RegP4, HttP4);
    double HHbReg2bdyKinFitMass = kinFitResult_bReg_2bdy.mhh_;
    double HHbReg2bdyKinFitChi2 = kinFitResult_bReg_2bdy.chi2_;
    
    //std::cout << "calling compKinFit4bdy without b-jet energy regression" << std::endl;
    kinFitResult kinFitResult_4bdy = compKinFit4bdy(bjet1P4, bjet2P4, tau1P4, tau2P4, metP4, metCov);
    double HH4bdyKinFitMass = kinFitResult_4bdy.mhh_;
    double HH4bdyKinFitChi2 = kinFitResult_4bdy.chi2_;
    //std::cout << "calling compKinFit4bdy with b-jet energy regression" << std::endl;
    kinFitResult kinFitResult_bReg_4bdy = compKinFit4bdy(bjet1RegP4, bjet2RegP4, tau1P4, tau2P4, metP4, metCov);
    double HHbReg4bdyKinFitMass = kinFitResult_bReg_4bdy.mhh_;
    double HHbReg4bdyKinFitChi2 = kinFitResult_bReg_4bdy.chi2_;

    histManager_inclusive.fillHistograms(
      tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
      tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
      dPhitt, dEtatt, dRtt,       
      jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
      jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
      bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
      bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
      dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
      visMass, svFitMass, mTtotal, mbb, mbbReg, 
      dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
      HbbPt, HbbRegPt, HttPt,
      augMT2ed,
      pZetaVis, pZeta, pZetaComb,
      HHPt, HHbRegPt, HHMass, HHbRegMass,
      HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
      HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
      met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
      evtWeight*processSF_inclusive, evtWeightErr*processSF_inclusive);

    if ( selEventsFile_inclusive ) {
      (*selEventsFile_inclusive) << run << ":" << lumi << ":" << event << std::endl;
    }

    ++selectedEntries_inclusive;
    selectedEntriesWeighted_inclusive += (evtWeight*processSF_inclusive);

    if ( nbJetsMedium >= 2 ) {
      histManager_2bM.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	dPhitt, dEtatt, dRtt,       
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	visMass, svFitMass, mTtotal, mbb, mbbReg, 
	dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	HbbPt, HbbRegPt, HttPt,
	augMT2ed,
	pZetaVis, pZeta, pZetaComb,
	HHPt, HHbRegPt, HHMass, HHbRegMass,
	HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      if ( dRtt < 2.0 && svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
        histManager_2bM_nonresonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }
      if ( svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) { 
        histManager_2bM_resonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }

      if ( selEventsFile_2bM ) {
	(*selEventsFile_2bM) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_2bM;
      selectedEntriesWeighted_2bM += (evtWeight*processSF_btag);
    } else if ( nbJetsLoose >= 2 ) {
      histManager_2bL.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	dPhitt, dEtatt, dRtt,       
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	visMass, svFitMass, mTtotal, mbb, mbbReg, 
	dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	HbbPt, HbbRegPt, HttPt,
	augMT2ed,
	pZetaVis, pZeta, pZetaComb,
	HHPt, HHbRegPt, HHMass, HHbRegMass,
	HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      if ( dRtt < 2.0 && svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) { 
	histManager_2bL_nonresonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }
      if ( svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
	histManager_2bL_resonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }

      if ( selEventsFile_2bL ) {
	(*selEventsFile_2bL) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_2bL;
      selectedEntriesWeighted_2bL += (evtWeight*processSF_btag);
    } else if ( nbJetsTight >= 1 ) {
      histManager_1b1j.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	dPhitt, dEtatt, dRtt,       
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	visMass, svFitMass, mTtotal, mbb, mbbReg, 
	dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	HbbPt, HbbRegPt, HttPt,
	augMT2ed,
	pZetaVis, pZeta, pZetaComb,
	HHPt, HHbRegPt, HHMass, HHbRegMass,
	HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      if ( dRtt < 2.0 && svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
	histManager_1b1j_nonresonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }
      if ( svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
	histManager_1b1j_resonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }

      if ( selEventsFile_1b1j ) {
	(*selEventsFile_1b1j) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_1b1j;
      selectedEntriesWeighted_1b1j += (evtWeight*processSF_btag);
    } else {
      histManager_2j.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	dPhitt, dEtatt, dRtt,       
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	visMass, svFitMass, mTtotal, mbb, mbbReg, 
	dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	HbbPt, HbbRegPt, HttPt,
	augMT2ed,
	pZetaVis, pZeta, pZetaComb,
	HHPt, HHbRegPt, HHMass, HHbRegMass,
	HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      if ( dRtt < 2.0 && svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
	histManager_2j_nonresonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }
      if ( svFitMass > 80. && svFitMass < 140. && mbbReg > 80. && mbbReg < 170. && HHbReg2bdyKinFitChi2 < 20. ) {
	histManager_2j_resonant.fillHistograms(
          tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	  tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	  dPhitt, dEtatt, dRtt,       
	  jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	  jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	  bjet1P4.pt(), bjet1P4.eta(), bjet1P4.phi(),
	  bjet2P4.pt(), bjet2P4.eta(), bjet2P4.phi(),
	  dPhibb, dEtabb, dRbb, nbJetsLoose, nbJetsMedium, 
	  visMass, svFitMass, mTtotal, mbb, mbbReg, 
	  dPhiHbbHtt, dPhiHbbMEt, dPhiHttMEt, minDRbt,
	  HbbPt, HbbRegPt, HttPt,
	  augMT2ed,
	  pZetaVis, pZeta, pZetaComb,
	  HHPt, HHbRegPt, HHMass, HHbRegMass,
	  HH2bdyKinFitMass, HH2bdyKinFitChi2, HHbReg2bdyKinFitMass, HHbReg2bdyKinFitChi2,
	  HH4bdyKinFitMass, HH4bdyKinFitChi2, HHbReg4bdyKinFitMass, HHbReg4bdyKinFitChi2,
	  met, deltaMEtParl, deltaMEtPerp, numVertices, NUP,
	  evtWeight*processSF_btag, evtWeightErr*processSF_btag);
      }

      if ( selEventsFile_2j ) {
	(*selEventsFile_2j) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_2j;
      selectedEntriesWeighted_2j += (evtWeight*processSF_nobtag);
    }
  }

  std::cout << "num. Entries = " << numEntries << std::endl;
  std::cout << " analyzed = " << analyzedEntries << std::endl;
  std::cout << " selected in category:" << std::endl;
  std::cout << "  inclusive = " << selectedEntries_inclusive << " (weighted = " << selectedEntriesWeighted_inclusive << ")" << std::endl;
  std::cout << "  2bM = " << selectedEntries_2bM << " (weighted = " << selectedEntriesWeighted_2bM << ")" << std::endl;
  std::cout << "  2bL = " << selectedEntries_2bL << " (weighted = " << selectedEntriesWeighted_2bL << ")" << std::endl;
  std::cout << "  1b1j = " << selectedEntries_1b1j << " (weighted = " << selectedEntriesWeighted_1b1j << ")" << std::endl;
  std::cout << "  2j = " << selectedEntries_2j << " (weighted = " << selectedEntriesWeighted_2j << ")" << std::endl;
  
  delete runLumiSectionEventNumberSelector;

  delete selEventsFile_inclusive;
  delete selEventsFile_2bM;
  delete selEventsFile_2bL;
  delete selEventsFile_1b1j;

  delete tau1Selection;
  delete tau2Selection;
  
  for ( std::vector<particleIDlooseToTightWeightEntryType*>::iterator it = jetToTauFakeRateLooseToTightWeights.begin();
	it != jetToTauFakeRateLooseToTightWeights.end(); ++it ) {
    delete (*it);
  }

  delete bTagSF;
  
  for ( std::vector<weightEntryType*>::iterator it = addWeights.begin();
	it != addWeights.end(); ++it ) {
    delete (*it);
  }

  delete augMT2edAlgo;

  for ( std::vector<TFile*>::iterator it = inputFilesToDelete.begin();
	it != inputFilesToDelete.end(); ++it ) {
    delete (*it);
  }
  
  delete inputTree;
  
  clock.Show("FWLiteTauTauAnalyzer2b2tau");

  return 0;
}
