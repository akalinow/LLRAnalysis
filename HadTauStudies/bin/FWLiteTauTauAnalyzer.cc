
/** \executable FWLiteTauTauAnalyzer
 *
 * Run MSSM -> tau tau -> tau_h tau_h analysis.
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

#include "LLRAnalysis/HadTauStudies/interface/TauTauHistManager.h"
#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"
#include "LLRAnalysis/HadTauStudies/interface/triggerTurnOnCurves.h"
#include "LLRAnalysis/HadTauStudies/interface/RunLumiSectionEventNumberSelector.h"
#include "LLRAnalysis/HadTauStudies/interface/particleIDlooseToTightWeightEntryType.h"
#include "LLRAnalysis/Utilities/interface/BtagSF.hh"

#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TBenchmark.h>
#include <TMath.h>
#include <TF1.h>
#include <TGraphAsymmErrors.h>
#include <TFormula.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>

typedef std::vector<std::string> vstring;
typedef std::vector<double> vdouble;

enum { kData, kSignal, kSM_Higgs, kZTTmc, kZL, kZJ, kZTT_Embedded, kW, kTT, kTT_Embedded, kVV };

enum { kOS, kSS };

enum { kTree, kHPScombIso3HitsMedium, kMVAwLToldDMsTight, kMVAwLToldDMsVTight };

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
					const vdouble& bJet1EtaBins, const vdouble& bJet2EtaBins, 
					const std::string& central_or_shift)
{
  edm::ParameterSet cfg;
  cfg.addParameter<std::string>("process", process);
  cfg.addParameter<std::string>("category", category);
  cfg.addParameter<std::string>("tauPtBin", tauPtBin);
  cfg.addParameter<vdouble>("tau1EtaBins", tau1EtaBins);
  cfg.addParameter<vdouble>("tau2EtaBins", tau2EtaBins);
  cfg.addParameter<vdouble>("bJet1EtaBins", bJet1EtaBins);
  cfg.addParameter<vdouble>("bJet2EtaBins", bJet2EtaBins);
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
  return weight;
}

namespace
{
  TFile* openFile(const edm::FileInPath& inputFileName)
  {
    if ( !inputFileName.isLocal() ) 
      throw cms::Exception("FWLiteTauTauAnalyzer") 
	<< " Failed to find file = " << inputFileName << " !!\n";
    TFile* inputFile = new TFile(inputFileName.fullPath().data());
    return inputFile;
  }

  TH1* loadLUT(TFile* inputFile, const std::string& lutName)
  {
    TH1* lut = dynamic_cast<TH1*>(inputFile->Get(lutName.data()));
    if ( !lut ) 
      throw cms::Exception("FWLiteTauTauAnalyzer") 
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
  
  double square(double x)
  {
    return x*x;
  }
}

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<FWLiteTauTauAnalyzer>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("FWLiteTauTauAnalyzer");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("FWLiteTauTauAnalyzer") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgFWLiteTauTauAnalyzer = cfg.getParameter<edm::ParameterSet>("FWLiteTauTauAnalyzer");
  
  std::string treeName = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("treeName");

  std::string process_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("process");
  int process = -1;
  if      ( process_string == "data_obs"     ) process = kData;
  else if ( contains(process_string, "data2012run") ) process = kData; // CV: for checking event yield in individual data-taking periods
  else if ( (contains(process_string, "ggH") || contains(process_string, "bbH")) && !contains(process_string, "_SM125") ) process = kSignal;
  else if ( contains(process_string, "ggH_SM125") || contains(process_string, "qqH_SM125") || contains(process_string, "VH_SM125") ) process = kSM_Higgs;
  else if ( process_string == "ZTTmc"        ) process = kZTTmc;
  else if ( process_string == "ZL"           ) process = kZL;
  else if ( process_string == "ZJ"           ) process = kZJ;
  else if ( process_string == "ZTT_Embedded" ) process = kZTT_Embedded;
  else if ( process_string == "W"            ) process = kW;
  else if ( process_string == "TT"           ) process = kTT;
  else if ( process_string == "TT_Embedded"  ) process = kTT_Embedded;
  else if ( process_string == "VV"           ) process = kVV;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'process' = " << process_string << " !!\n";

  bool isMC = !(process == kData || process == kZTT_Embedded);
  bool isEmbedded = (process == kZTT_Embedded || process == kTT_Embedded);

  double massPoint = ( process == kSignal ) ?
    cfgFWLiteTauTauAnalyzer.getParameter<double>("massPoint") : -1.;

  double lumiScale = ( process != kData ) ?
    cfgFWLiteTauTauAnalyzer.getParameter<double>("lumiScale") : 1.;

  vdouble stitchingWeights;
  if ( process == kZTTmc || process == kZL || process == kZJ || process == kW ) {
    stitchingWeights = cfgFWLiteTauTauAnalyzer.getParameter<vdouble>("stitchingWeights");
    if ( stitchingWeights.size() != 5 ) throw cms::Exception("FWLiteTauTauAnalyzer") 
      << "Configuration parameter 'stitchingWeights' expected to be of size = 5 !!\n";      
  }

  double tau1PtMin = cfgFWLiteTauTauAnalyzer.getParameter<double>("tau1PtMin");
  double tau1PtMax = cfgFWLiteTauTauAnalyzer.getParameter<double>("tau1PtMax");
  std::string tau1Selection_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("tau1Selection");

  double tau2PtMin = cfgFWLiteTauTauAnalyzer.getParameter<double>("tau2PtMin");
  double tau2PtMax = cfgFWLiteTauTauAnalyzer.getParameter<double>("tau2PtMax");
  std::string tau2Selection_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("tau2Selection");

  std::string tauPtBin = "";
  if      ( tau1PtMin > 0. && tau1PtMax > 0. ) tauPtBin.append(Form("tau1Pt%1.0fto%1.0f", tau1PtMin, tau1PtMax));
  else if ( tau1PtMin > 0.                   ) tauPtBin.append(Form("tau1PtGt%1.0f", tau1PtMin));
  else if (                   tau1PtMax > 0. ) tauPtBin.append(Form("tau1PtLt%1.0f", tau1PtMax));
  if      ( tau2PtMin > 0. && tau2PtMax > 0. ) tauPtBin.append(Form("tau2Pt%1.0fto%1.0f", tau2PtMin, tau2PtMax));
  else if ( tau2PtMin > 0.                   ) tauPtBin.append(Form("tau2PtGt%1.0f", tau2PtMin));
  else if (                   tau2PtMax > 0. ) tauPtBin.append(Form("tau2PtLt%1.0f", tau2PtMax));
  
  vdouble tau1EtaBins = cfgFWLiteTauTauAnalyzer.getParameter<vdouble>("tau1EtaBins"); // CV: eta bins in which jet -> tau fake-rates are determined
  vdouble tau2EtaBins = cfgFWLiteTauTauAnalyzer.getParameter<vdouble>("tau2EtaBins");

  std::string region = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("region");
  int diTauChargeSel = -1;
  if      ( region.find("OS") != std::string::npos ) diTauChargeSel = kOS;
  else if ( region.find("SS") != std::string::npos ) diTauChargeSel = kSS;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'region' = " << region << " !!\n";

  std::string trigger_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("trigger");
  int trigger = -1;
  enum { kTau, kJet, kTauPlusJet };
  if      ( trigger_string == "Tau"        ) trigger = kTau;
  else if ( trigger_string == "Jet"        ) trigger = kJet;
  else if ( trigger_string == "TauPlusJet" ) trigger = kTauPlusJet;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'trigger' = " << trigger_string << " !!\n";
  double tauPtForSwitchingTriggers = cfgFWLiteTauTauAnalyzer.getParameter<double>("tauPtForSwitchingTriggers");
  
  std::string applyTauTriggerTurnOn_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("applyTauTriggerTurnOn");
  int applyTauTriggerTurnOn = -1;
  if      ( applyTauTriggerTurnOn_string == "tree"                  ) applyTauTriggerTurnOn = kTree;
  else if ( applyTauTriggerTurnOn_string == "HPScombIso3HitsMedium" ) applyTauTriggerTurnOn = kHPScombIso3HitsMedium;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsTight"     ) applyTauTriggerTurnOn = kMVAwLToldDMsTight;
  else if ( applyTauTriggerTurnOn_string == "MVAwLToldDMsVTight"    ) applyTauTriggerTurnOn = kMVAwLToldDMsVTight;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'applyTauTriggerTurnOn' = " << applyTauTriggerTurnOn_string << " !!\n";

  bool applyTightBtag = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyTightBtag");

  vdouble bJet1EtaBins = cfgFWLiteTauTauAnalyzer.getParameter<vdouble>("bJet1EtaBins"); // CV: eta bins in which probabilities for jets to pass tight/loose b-tag discriminators are determined
  vdouble bJet2EtaBins = cfgFWLiteTauTauAnalyzer.getParameter<vdouble>("bJet2EtaBins");

  std::vector<particleIDlooseToTightWeightEntryType*> bJetLooseToTightWeights;
  bool applyBJetLooseToTightWeight = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyBJetLooseToTightWeight");
  if ( applyBJetLooseToTightWeight ) {
    edm::ParameterSet cfgBJetLooseToTightWeight = cfgFWLiteTauTauAnalyzer.getParameter<edm::ParameterSet>("bJetLooseToTightWeight");
    
    std::string fitFunctionNormName = cfgBJetLooseToTightWeight.getParameter<std::string>("fitFunctionNormName");
    std::string fitFunctionShapeName_bJet1 = cfgBJetLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_bJet1");
    double fitFunctionShapePower_bJet1 = cfgBJetLooseToTightWeight.getParameter<double>("fitFunctionShapePower_bJet1");
    std::string fitFunctionShapeName_bJet2 = cfgBJetLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_bJet2");
    double fitFunctionShapePower_bJet2 = cfgBJetLooseToTightWeight .getParameter<double>("fitFunctionShapePower_bJet2");

    std::string inputFileName = cfgBJetLooseToTightWeight.getParameter<std::string>("inputFileName");
    TFile* inputFile = new TFile(inputFileName.data());
    
    int numBJet1EtaBins = bJet1EtaBins.size() - 1;
    for ( int idxBJet1EtaBin = 0; idxBJet1EtaBin < numBJet1EtaBins; ++idxBJet1EtaBin ) {
      double bJet1EtaMin = bJet1EtaBins[idxBJet1EtaBin];
      double bJet1EtaMax = bJet1EtaBins[idxBJet1EtaBin + 1];

      int numBJet2EtaBins = bJet2EtaBins.size() - 1;
      for ( int idxBJet2EtaBin = 0; idxBJet2EtaBin < numBJet2EtaBins; ++idxBJet2EtaBin ) {
	double bJet2EtaMin = bJet2EtaBins[idxBJet2EtaBin];
	double bJet2EtaMax = bJet2EtaBins[idxBJet2EtaBin + 1];

        particleIDlooseToTightWeightEntryType* bJetLooseToTightWeight = new particleIDlooseToTightWeightEntryType(
          inputFile,
	  "bJet", bJet1EtaMin, bJet1EtaMax, bJet2EtaMin, bJet2EtaMax,
	  fitFunctionNormName, 
	  "", fitFunctionShapeName_bJet1, "", fitFunctionShapePower_bJet1, 
	  "", fitFunctionShapeName_bJet2, "", fitFunctionShapePower_bJet2);
	bJetLooseToTightWeights.push_back(bJetLooseToTightWeight);
      }
    }
    delete inputFile;
  }

  BtagSF* bTagSF = new BtagSF(12345);
  
  std::vector<particleIDlooseToTightWeightEntryType*> jetToTauFakeRateLooseToTightWeights;
  bool applyJetToTauFakeRateLooseToTightWeight = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyJetToTauFakeRateLooseToTightWeight");
  if ( applyJetToTauFakeRateLooseToTightWeight ) {
    edm::ParameterSet cfgJetToTauFakeRateLooseToTightWeight = cfgFWLiteTauTauAnalyzer.getParameter<edm::ParameterSet>("jetToTauFakeRateLooseToTightWeight");
    
    std::string fitFunctionNormName = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionNormName");
    std::string fitFunctionShapeName_tau1_central = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau1_central");
    std::string graphShapeName_tau1, fitFunctionShapeName_tau1_shift;
    if ( cfgJetToTauFakeRateLooseToTightWeight.exists("graphShapeName_tau1") ) {
      graphShapeName_tau1 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("graphShapeName_tau1");
      fitFunctionShapeName_tau1_shift = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau1_shift");
    }
    double fitFunctionShapePower_tau1 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<double>("fitFunctionShapePower_tau1");
    std::string fitFunctionShapeName_tau2_central = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau2_central");
    std::string graphShapeName_tau2, fitFunctionShapeName_tau2_shift;
    if ( cfgJetToTauFakeRateLooseToTightWeight.exists("graphShapeName_tau2") ) {
      graphShapeName_tau2 = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("graphShapeName_tau2");
      fitFunctionShapeName_tau2_shift = cfgJetToTauFakeRateLooseToTightWeight.getParameter<std::string>("fitFunctionShapeName_tau2_shift");
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
	  graphShapeName_tau1, fitFunctionShapeName_tau1_central, fitFunctionShapeName_tau1_shift, fitFunctionShapePower_tau1, 
	  graphShapeName_tau2, fitFunctionShapeName_tau2_central, fitFunctionShapeName_tau2_shift, fitFunctionShapePower_tau2);
	jetToTauFakeRateLooseToTightWeights.push_back(jetToTauFakeRateLooseToTightWeight);
      }
    }
    delete inputFile;
  }

  TFormula* jetToTauFakeRateCorrection = 0;
  bool applyJetToTauFakeRateCorrection = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyJetToTauFakeRateCorrection");
  if ( applyJetToTauFakeRateCorrection ) {
    std::string jetToTauFakeRateCorrection_string = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("jetToTauFakeRateCorrection");
    jetToTauFakeRateCorrection = new TFormula("jetToTauFakeRateCorrection", jetToTauFakeRateCorrection_string.data());
  }

  vstring addWeights_string = cfgFWLiteTauTauAnalyzer.getParameter<vstring>("addWeights");

  std::string central_or_shift = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("central_or_shift");
  bool applyHighPtTauIDeffUncertaintyUp   = (central_or_shift == "CMS_eff_t_mssmHigh_tautau_8TeVUp");
  bool applyHighPtTauIDeffUncertaintyDown = (central_or_shift == "CMS_eff_t_mssmHigh_tautau_8TeVDown");
  UInt_t bJetEff_shift = BtagSF::kNo;
  if ( central_or_shift == "CMS_eff_b_8TeVUp"    ) bJetEff_shift    = BtagSF::kUp;
  if ( central_or_shift == "CMS_eff_b_8TeVDown"  ) bJetEff_shift    = BtagSF::kDown;
  UInt_t bJetMistag_shift = BtagSF::kNo;
  if ( central_or_shift == "CMS_fake_b_8TeVUp"   ) bJetMistag_shift = BtagSF::kUp;
  if ( central_or_shift == "CMS_fake_b_8TeVDown" ) bJetMistag_shift = BtagSF::kDown;

  std::vector<TFile*> inputFilesToDelete;

  bool applyHiggsPtReweighting = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyHiggsPtReweighting");
  TH1* lutHiggsPtReweighting = 0;
  if ( applyHiggsPtReweighting ) {
    edm::ParameterSet cfgHiggsPtReweighting = cfgFWLiteTauTauAnalyzer.getParameter<edm::ParameterSet>("higgsPtReweighting");
    edm::FileInPath inputFileName = cfgHiggsPtReweighting.getParameter<edm::FileInPath>("inputFileName");
    if ( inputFileName.location() == edm::FileInPath::Unknown ) 
      throw cms::Exception("FWLiteTauTauAnalyzer") 
	<< " Failed to find Higgs Pt reweighting file = " << inputFileName << " !!\n";
    std::string lutName = cfgHiggsPtReweighting.getParameter<std::string>("lutName");
    TFile* inputFile = openFile(edm::FileInPath(inputFileName));
    lutHiggsPtReweighting = loadLUT(inputFile, lutName.data());
    inputFilesToDelete.push_back(inputFile);
  }

  std::string selEventsFileName_input = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("selEventsFileName_input");
  RunLumiSectionEventNumberSelector* runLumiSectionEventNumberSelector = 0;
  if ( selEventsFileName_input != "" ) {
    edm::ParameterSet cfgRunLumiSectionEventNumberSelector;
    cfgRunLumiSectionEventNumberSelector.addParameter<std::string>("inputFileName", selEventsFileName_input);
    cfgRunLumiSectionEventNumberSelector.addParameter<std::string>("separator", ":");
    runLumiSectionEventNumberSelector = new RunLumiSectionEventNumberSelector(cfgRunLumiSectionEventNumberSelector);
  }

  std::string selEventsFileName_output = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("selEventsFileName_output");
  std::ostream* selEventsFile_inclusive = 0;
  std::ostream* selEventsFile_nobtag    = 0;
  std::ostream* selEventsFile_btag      = 0;
  if ( selEventsFileName_output != "" ) {
    size_t idx = selEventsFileName_output.find_last_of('.');
    std::string selEventsFileName_part1 = std::string(selEventsFileName_output , 0, idx);
    std::string selEventsFileName_part2 = ( idx != std::string::npos ) ?
      std::string(selEventsFileName_output , idx, std::string::npos) : "";
    std::string selEventsFileName_inclusive = Form("%s_inclusive%s", selEventsFileName_part1.data(), selEventsFileName_part2.data());
    std::cout << "writing events selected in 'inclusive' category to file = " << selEventsFileName_inclusive << std::endl;  
    selEventsFile_inclusive = new std::ofstream(selEventsFileName_inclusive.data(), std::ios::out);
    std::string selEventsFileName_nobtag = Form("%s_nobtag%s", selEventsFileName_part1.data(), selEventsFileName_part2.data());
    std::cout << "writing events selected in 'nobtag' category to file = " << selEventsFileName_nobtag << std::endl;  
    selEventsFile_nobtag = new std::ofstream(selEventsFileName_nobtag.data(), std::ios::out);
    std::string selEventsFileName_btag = Form("%s_btag%s", selEventsFileName_part1.data(), selEventsFileName_part2.data());
    std::cout << "writing events selected in 'btag' category to file = " << selEventsFileName_btag << std::endl;  
    selEventsFile_btag = new std::ofstream(selEventsFileName_btag.data(), std::ios::out);
  }  

  fwlite::InputSource inputFiles(cfg); 
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  edm::ParameterSet cfgHistManager_inclusive = makeHistManagerConfig(
    process_string, Form("tauTau_%s_inclusive", region.data()), tauPtBin, tau1EtaBins, tau2EtaBins, bJet1EtaBins, bJet2EtaBins, central_or_shift);
  TauTauHistManager histManager_inclusive(cfgHistManager_inclusive);
  histManager_inclusive.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_nobtag = makeHistManagerConfig(
    process_string, Form("tauTau_%s_nobtag", region.data()), tauPtBin, tau1EtaBins, tau2EtaBins, bJet1EtaBins, bJet2EtaBins, central_or_shift);
  TauTauHistManager histManager_nobtag(cfgHistManager_nobtag);
  histManager_nobtag.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_btag = makeHistManagerConfig(
    process_string, Form("tauTau_%s_btag", region.data()), tauPtBin, tau1EtaBins, tau2EtaBins, bJet1EtaBins, bJet2EtaBins, central_or_shift);
  TauTauHistManager histManager_btag(cfgHistManager_btag);
  histManager_btag.bookHistograms(fs);

  TChain* inputTree = new TChain(treeName.data());
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end(); ++inputFileName ) {
    std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
    inputTree->AddFile(inputFileName->data());
  }
  
  if ( !(inputTree->GetListOfFiles()->GetEntries() >= 1) ) {
    throw cms::Exception("FWLiteTauTauAnalyzer") 
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

  Float_t tau1Pt, tau1Eta, tau1Phi;
  inputTree->SetBranchAddress("l1Pt", &tau1Pt);
  inputTree->SetBranchAddress("l1Eta", &tau1Eta);
  inputTree->SetBranchAddress("l1Phi", &tau1Phi);
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
  Float_t tau1GenPt;
  inputTree->SetBranchAddress("l1GenPt", &tau1GenPt);

  TTreeFormula* tau1Selection = 0;
  if ( tau1Selection_string != "" ) {
    tau1Selection = new TTreeFormula("tau1Selection", tau1Selection_string.data(), inputTree);
  }

  Float_t tau2Pt, tau2Eta, tau2Phi;
  inputTree->SetBranchAddress("l2Pt", &tau2Pt);
  inputTree->SetBranchAddress("l2Eta", &tau2Eta);
  inputTree->SetBranchAddress("l2Phi", &tau2Phi);
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
  Float_t tau2GenPt; 
  inputTree->SetBranchAddress("l2GenPt", &tau2GenPt);

  TTreeFormula* tau2Selection = 0;
  if ( tau2Selection_string != "" ) {
    tau2Selection = new TTreeFormula("tau2Selection", tau2Selection_string.data(), inputTree);
  }

  Float_t diTauCharge;
  inputTree->SetBranchAddress("diTauCharge", &diTauCharge);
  Float_t diTauGenMass;
  if ( process == kSignal ) inputTree->SetBranchAddress("genMass", &diTauGenMass);
  Float_t dPhi, dEta, dR;
  inputTree->SetBranchAddress("dPhitt", &dPhi);
  inputTree->SetBranchAddress("dEtatt", &dEta);
  inputTree->SetBranchAddress("dRtt", &dR);
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

  Float_t jet1Pt, jet1Eta, jet1Phi;
  inputTree->SetBranchAddress("jet1Pt", &jet1Pt);
  inputTree->SetBranchAddress("jet1Eta", &jet1Eta);
  inputTree->SetBranchAddress("jet1Phi", &jet1Phi);
  Float_t jet1BtagDiscr;
  inputTree->SetBranchAddress("jet1Btag", &jet1BtagDiscr);

  Float_t jet2Pt, jet2Eta, jet2Phi;
  inputTree->SetBranchAddress("jet2Pt", &jet2Pt);
  inputTree->SetBranchAddress("jet2Eta", &jet2Eta);
  inputTree->SetBranchAddress("jet2Phi", &jet2Phi);
  Float_t jet2BtagDiscr;
  inputTree->SetBranchAddress("jet2Btag", &jet2BtagDiscr);

  Int_t nJets;
  inputTree->SetBranchAddress("nJets", &nJets);

  Float_t bjet1Pt, bjet1Eta, bjet1Phi;
  inputTree->SetBranchAddress("bjet1Pt", &bjet1Pt);
  inputTree->SetBranchAddress("bjet1Eta", &bjet1Eta);
  inputTree->SetBranchAddress("bjet1Phi", &bjet1Phi);
  Float_t bjet1BtagDiscr;
  inputTree->SetBranchAddress("bjet1Btag", &bjet1BtagDiscr);
  Int_t bjet1GenPartonFlavour;
  inputTree->SetBranchAddress("bjet1GenPartonFlavour", &bjet1GenPartonFlavour);

  Float_t bjet2Pt, bjet2Eta, bjet2Phi;
  inputTree->SetBranchAddress("bjet2Pt", &bjet2Pt);
  inputTree->SetBranchAddress("bjet2Eta", &bjet2Eta);
  inputTree->SetBranchAddress("bjet2Phi", &bjet2Phi);
  Float_t bjet2BtagDiscr;
  inputTree->SetBranchAddress("bjet2Btag", &bjet2BtagDiscr);
  Int_t bjet2GenPartonFlavour;
  inputTree->SetBranchAddress("bjet2GenPartonFlavour", &bjet2GenPartonFlavour);

  Float_t met;
  inputTree->SetBranchAddress("met", &met);
  
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
  
  Float_t genHiggsPt;
  inputTree->SetBranchAddress("genHiggsPt", &genHiggsPt);

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
    
  int currentTreeNumber = inputTree->GetTreeNumber();
  
  int numEntries = inputTree->GetEntries();
  int analyzedEntries = 0;
  int selectedEntries_inclusive = 0;
  double selectedEntriesWeighted_inclusive = 0.;
  int selectedEntries_nobtag = 0;
  double selectedEntriesWeighted_nobtag = 0.;
  int selectedEntries_btag = 0;
  double selectedEntriesWeighted_btag = 0.;
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
    // CV: binning of events in tau Pt
    if ( !((tau1PtMin == -1. || tau1Pt > tau1PtMin) && (tau1PtMax == -1. || tau1Pt < tau1PtMax) &&
	   (tau2PtMin == -1. || tau2Pt > tau2PtMin) && (tau2PtMax == -1. || tau2Pt < tau2PtMax)) ) continue;

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

    int nbJets = 0;
    if ( bjet1Pt > 20. ) {
      if ( bjet1BtagDiscr > 0.244 && !applyTightBtag ) { 
	++nbJets;
      } else {
	if ( bTagSF->isbtagged(bjet1Pt, bjet1Eta, bjet1BtagDiscr, bjet1GenPartonFlavour, !isMC, bJetEff_shift, bJetMistag_shift, true) ) ++nbJets;
      }
    }
    // CV: relax cut on b-jet discriminator of first b-jet only, to make tight/loose weights well defined
    if ( bjet2Pt > 20. && bTagSF->isbtagged(bjet2Pt, bjet2Eta, bjet2BtagDiscr, bjet2GenPartonFlavour, !isMC, bJetEff_shift, bJetMistag_shift, true) ) ++nbJets;

    int nElectrons = 0;
    if ( electron1Pt > 10. && TMath::Abs(electron1Eta) < 2.5 ) ++nElectrons;

    int nMuons = 0;
    if ( muon1Pt > 10. && TMath::Abs(muon1Eta) < 2.4 ) ++nMuons;

    if ( nElectrons > 0 || nMuons > 0 ) continue;

    //---------------------------------------------------------------------------
    // CV: process dependent selection criteria
    if ( process == kSignal && !(diTauGenMass > 0.7*massPoint && diTauGenMass < 1.3*massPoint) ) continue;

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
    
    typedef double (*triggerTurnOnCurvePtr)(double, double);
    triggerTurnOnCurvePtr tauTriggerEffData = 0;
    triggerTurnOnCurvePtr tauTriggerEffMC   = 0;
    if ( applyTauTriggerTurnOn == kHPScombIso3HitsMedium ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_cutMedium;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_cutMedium;
    } else if ( applyTauTriggerTurnOn == kMVAwLToldDMsTight ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaTight;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaTight;      
    } else if ( applyTauTriggerTurnOn == kMVAwLToldDMsVTight ) {
      tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaVTight;
      tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaVTight;  
    } 
    if ( applyTauTriggerTurnOn != kTree ) {
      assert(tauTriggerEffData);
      assert(tauTriggerEffMC);
      triggerEffData_diTau = (*tauTriggerEffData)(tau1Pt, tau1Eta)*(*tauTriggerEffData)(tau2Pt, tau2Eta);
      Float_t triggerEffMC_diTau = (*tauTriggerEffMC)(tau1Pt, tau1Eta)*(*tauTriggerEffMC)(tau2Pt, tau2Eta);
      triggerWeight_diTau = ( triggerEffMC_diTau > 0. ) ? (triggerEffData_diTau/triggerEffMC_diTau) : 1.;
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
      if ( tau1GenPt > 1.0 ) evtWeight *= compTauDecayModeWeight(tau1Eta, TMath::Nint(tau1DecayMode));
      if ( tau2GenPt > 1.0 ) evtWeight *= compTauDecayModeWeight(tau2Eta, TMath::Nint(tau2DecayMode));
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
    if ( applyBJetLooseToTightWeight ) {
      double absBJet1Eta = TMath::Abs(bjet1Eta);
      particleIDlooseToTightWeightEntryType* bJetLooseToTightWeight_bJetEtaBin = 0;
      for ( std::vector<particleIDlooseToTightWeightEntryType*>::const_iterator bJetLooseToTightWeight = bJetLooseToTightWeights.begin();
	    bJetLooseToTightWeight != bJetLooseToTightWeights.end(); ++bJetLooseToTightWeight ) {
	if ( ((*bJetLooseToTightWeight)->particle1EtaMin_ < 0. || absBJet1Eta > (*bJetLooseToTightWeight)->particle1EtaMin_) &&
	     ((*bJetLooseToTightWeight)->particle1EtaMax_ > 5. || absBJet1Eta < (*bJetLooseToTightWeight)->particle1EtaMax_) ) {
	  bJetLooseToTightWeight_bJetEtaBin = (*bJetLooseToTightWeight);
	  break;
	}
      }
      if ( bJetLooseToTightWeight_bJetEtaBin ) {
	evtWeight *= bJetLooseToTightWeight_bJetEtaBin->weight(bjet1Pt, 0.);
      } else {
	std::cerr << "Warning: bjet1Eta = " << bjet1Eta << " outside range !!" << std::endl;
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
    if ( applyHiggsPtReweighting ) {
      evtWeight *= compHiggsPtWeight(lutHiggsPtReweighting, genHiggsPt);
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
      //processSF_inclusive = 0.73; // CV: W+jets scale-factor provided by Aram
      //processSF_nobtag    = 0.73;
      //processSF_btag      = 1.50;
      processSF_inclusive = 1.00; // CV: new W+jets scale-factor determined by Olivier (compatible with 1.0 within uncertainties)
      processSF_nobtag    = 1.00;
      processSF_btag      = 1.00;
    }
    //---------------------------------------------------------------------------

    int tau1GenMatch = getGenMatch(l1isGenHadTau, l1isGenMuon, l1isGenElectron, l1isGenJet);
    int tau2GenMatch = getGenMatch(l2isGenHadTau, l2isGenMuon, l2isGenElectron, l2isGenJet);
    
    histManager_inclusive.fillHistograms(
      tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
      tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
      dPhi, dEta, dR, 
      visMass, svFitMass, mTtotal, 
      jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
      jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
      bjet1Pt, bjet1Eta, bjet1Phi, 
      bjet2Pt, bjet2Eta, bjet2Phi, nbJets, 
      met, numVertices, genHiggsPt, NUP,
      evtWeight*processSF_inclusive, evtWeightErr*processSF_inclusive);

    if ( selEventsFile_inclusive ) {
      (*selEventsFile_inclusive) << run << ":" << lumi << ":" << event << std::endl;
    }

    ++selectedEntries_inclusive;
    selectedEntriesWeighted_inclusive += (evtWeight*processSF_inclusive);

    if ( nbJets == 0 ) {
      histManager_nobtag.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
        tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
        dPhi, dEta, dR, 
        visMass, svFitMass, mTtotal,
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1Pt, bjet1Eta, bjet1Phi, 
	bjet2Pt, bjet2Eta, bjet2Phi, nbJets,
	met, numVertices, genHiggsPt, NUP, 
	evtWeight*processSF_nobtag, evtWeightErr*processSF_nobtag);

      if ( selEventsFile_nobtag ) {
	(*selEventsFile_nobtag) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_nobtag;
      selectedEntriesWeighted_nobtag += (evtWeight*processSF_nobtag);
    } else if ( nbJets >= 1 && nJets <= 1 ) {
      histManager_btag.fillHistograms(
        tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1GenMatch, tau1IsoPtSum, tau1rawMVA, 
	tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2GenMatch, tau2IsoPtSum, tau2rawMVA, 
	dPhi, dEta, dR, 
	visMass, svFitMass, mTtotal,
	jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
	jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr, nJets,
	bjet1Pt, bjet1Eta, bjet1Phi, 
	bjet2Pt, bjet2Eta, bjet2Phi, nbJets, 
	met, numVertices, genHiggsPt, NUP,
	evtWeight*processSF_btag, evtWeightErr*processSF_btag);

      if ( selEventsFile_btag ) {
	(*selEventsFile_btag) << run << ":" << lumi << ":" << event << std::endl;
      }
      
      ++selectedEntries_btag;
      selectedEntriesWeighted_btag += (evtWeight*processSF_btag);
    }
  }

  std::cout << "num. Entries = " << numEntries << std::endl;
  std::cout << " analyzed = " << analyzedEntries << std::endl;
  std::cout << " selected in category:" << std::endl;
  std::cout << "  inclusive = " << selectedEntries_inclusive << " (weighted = " << selectedEntriesWeighted_inclusive << ")" << std::endl;
  std::cout << "  nobtag = " << selectedEntries_nobtag << " (weighted = " << selectedEntriesWeighted_nobtag << ")" << std::endl;
  std::cout << "  btag = " << selectedEntries_btag << " (weighted = " << selectedEntriesWeighted_btag << ")" << std::endl;
  
  delete runLumiSectionEventNumberSelector;

  delete selEventsFile_inclusive;
  delete selEventsFile_nobtag;
  delete selEventsFile_btag;

  delete tau1Selection;
  delete tau2Selection;
  
  for ( std::vector<particleIDlooseToTightWeightEntryType*>::const_iterator it = bJetLooseToTightWeights.begin();
	it != bJetLooseToTightWeights.end(); ++it ) {
    delete (*it);
  }
  for ( std::vector<particleIDlooseToTightWeightEntryType*>::iterator it = jetToTauFakeRateLooseToTightWeights.begin();
	it != jetToTauFakeRateLooseToTightWeights.end(); ++it ) {
    delete (*it);
  }

  delete bTagSF;
  
  for ( std::vector<weightEntryType*>::iterator it = addWeights.begin();
	it != addWeights.end(); ++it ) {
    delete (*it);
  }

  for ( std::vector<TFile*>::iterator it = inputFilesToDelete.begin();
	it != inputFilesToDelete.end(); ++it ) {
    delete (*it);
  }
  
  delete inputTree;
  
  clock.Show("FWLiteTauTauAnalyzer");

  return 0;
}
