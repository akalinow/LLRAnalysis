
/** \executable produceTauTauSyncNtuple
 *
 * Produce sync-Ntuple for hadronic channel of H -> tautau analysis.
 *
 * See https://twiki.cern.ch/twiki/bin/view/CMS/HiggsToTauTauWorkingSummer2013 for details.
 *
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "LLRAnalysis/HadTauStudies/interface/triggerTurnOnCurves.h"

#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TBenchmark.h>
#include <TMath.h>

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

enum { kTree, kHPScombIso3HitsMedium, kMVAwLToldDMsTight, kMVAwLToldDMsVTight };

struct branchEntryBaseType
{
  branchEntryBaseType(const std::string& inputBranchName, const std::string& inputBranchType, const std::string& outputBranchName, const std::string& outputBranchType)
    : inputBranchName_(inputBranchName),
      inputBranchType_string_(inputBranchType),
      outputBranchName_(outputBranchName),
      outputBranchType_string_(outputBranchType)
  {
    std::cout << " copying branch " << inputBranchName << " (type = " << inputBranchType << ") --> " << outputBranchName << " (type = " << outputBranchType << ")" << std::endl;
    if      ( inputBranchType == "F" ) inputBranchType_ = kF;
    else if ( inputBranchType == "D" ) inputBranchType_ = kD;
    else if ( inputBranchType == "I" ) inputBranchType_ = kI;
    else if ( inputBranchType == "l" ) inputBranchType_ = kUL;
    else throw cms::Exception("produceTauTauSyncNtuple") 
      << "Invalid branch type = '" << inputBranchType << "' for branch = '" << inputBranchName << "' !!\n";
    if      ( outputBranchType == "F" ) outputBranchType_ = kF;
    else if ( outputBranchType == "D" ) outputBranchType_ = kD;
    else if ( outputBranchType == "I" ) outputBranchType_ = kI;
    else if ( outputBranchType == "l" ) outputBranchType_ = kUL;
    else throw cms::Exception("produceTauTauSyncNtuple") 
      << "Invalid branch type = '" << outputBranchType << "' for branch = '" << outputBranchName << "' !!\n";
  }
  ~branchEntryBaseType() {}
  virtual void setInputTree(TTree*) = 0;
  virtual void setOutputTree(TTree*) = 0;
  virtual void update() {}
  virtual void copyBranch() = 0;
  std::string inputBranchName_;
  std::string inputBranchType_string_;
  enum { kF, kD, kI, kUL };
  int inputBranchType_;
  std::string outputBranchName_;
  std::string outputBranchType_string_;
  int outputBranchType_;
};

template <typename T1, typename T2>
struct branchEntryType : branchEntryBaseType
{
  branchEntryType(const std::string& inputBranchName, const std::string& inputBranchType, const std::string& outputBranchName, const std::string& outputBranchType)
    : branchEntryBaseType(inputBranchName, inputBranchType, outputBranchName, outputBranchType)
  {}
  ~branchEntryType() {}
  void setInputTree(TTree* inputTree)
  {
    inputTree->SetBranchAddress(inputBranchName_.data(), &inputValue_);
  }
  void setOutputTree(TTree* outputTree)
  {
    outputTree->Branch(outputBranchName_.data(), &outputValue_, Form("%s/%s", outputBranchName_.data(), outputBranchType_string_.data()));
  }
  void copyBranch()
  {
    if ( inputBranchType_ == kF && outputBranchType_ == kI ) {
      outputValue_ = TMath::Nint((Float_t)inputValue_);
    } else if ( inputBranchType_ == kD && outputBranchType_ == kI ) {
      outputValue_ = TMath::Nint((Double_t)inputValue_);
    } else {
      outputValue_ = inputValue_;
    }
  }
  T1 inputValue_;
  T2 outputValue_;
};
typedef branchEntryType<Float_t,     Float_t> branchEntryTypeFF;
typedef branchEntryType<Float_t,    Double_t> branchEntryTypeFD;
typedef branchEntryType<Double_t,   Double_t> branchEntryTypeDD;
typedef branchEntryType<Float_t,       Int_t> branchEntryTypeFI;
typedef branchEntryType<Int_t,         Int_t> branchEntryTypeII;
typedef branchEntryType<ULong64_t, ULong64_t> branchEntryTypeULUL;

template <typename T>
struct branchEntryFormulaType : branchEntryBaseType
{
  branchEntryFormulaType(const std::string& inputExpression, const std::string& outputBranchName, const std::string& outputBranchType)
    : branchEntryBaseType(inputExpression, "F", outputBranchName, outputBranchType),
      inputExpression_string_(inputExpression),
      inputExpression_(0)
  {}
  ~branchEntryFormulaType() 
  {
    delete inputExpression_;
  }
  void setInputTree(TTree* inputTree)
  {
    delete inputExpression_;
    inputExpression_ = new TTreeFormula(Form("branchEntryTypeFormula_%s", outputBranchName_.data()), inputExpression_string_.data(), inputTree);
  }
  void setOutputTree(TTree* outputTree)
  {
    outputTree->Branch(outputBranchName_.data(), &outputValue_, Form("%s/%s", outputBranchName_.data(), outputBranchType_string_.data()));
  }
  void update()
  {
    inputExpression_->UpdateFormulaLeaves();
  }
  void copyBranch()
  {
    Double_t inputValue = inputExpression_->EvalInstance();
    if ( outputBranchType_ == kI ) {
      outputValue_ = TMath::Nint(inputValue);
    } else {
      outputValue_ = inputValue;
    }
  }
  std::string inputExpression_string_;
  TTreeFormula* inputExpression_;
  T outputValue_;
};
typedef branchEntryFormulaType<Double_t> branchEntryFormulaTypeD;

typedef std::vector<std::string> vstring;

int main(int argc, char* argv[]) 
{
//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  std::cout << "<produceTauTauSyncNtuple>:" << std::endl;

//--- keep track of time it takes the macro to execute
  TBenchmark clock;
  clock.Start("produceTauTauSyncNtuple");

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("produceTauTauSyncNtuple") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgProduceTauTauSyncNtuple = cfg.getParameter<edm::ParameterSet>("produceTauTauSyncNtuple");

  bool isHiggsMC = cfgProduceTauTauSyncNtuple.getParameter<bool>("isHiggsMC");

  bool isEmbed = cfgProduceTauTauSyncNtuple.getParameter<bool>("isEmbed");
  enum { kPfEmbed, kRhEmbed };
  int embedType = -1;
  if ( isEmbed ) {
    std::string embedType_string = cfgProduceTauTauSyncNtuple.getParameter<std::string>("embedType");
    if      ( embedType_string == "PfEmbed" ) embedType = kPfEmbed;
    else if ( embedType_string == "RhEmbed" ) embedType = kRhEmbed;
    else throw cms::Exception("produceTauTauSyncNtuple")
      << "Invalid Configuration Parameter 'embedType' = " << embedType_string << " !!\n";
  }

  std::string trigger_string = cfgProduceTauTauSyncNtuple.getParameter<std::string>("trigger");
  int trigger = -1;
  enum { kTau, kJet, kTauPlusJet };
  if      ( trigger_string == "Tau"        ) trigger = kTau;
  else if ( trigger_string == "Jet"        ) trigger = kJet;
  else if ( trigger_string == "TauPlusJet" ) trigger = kTauPlusJet;
  else throw cms::Exception("produceTauTauSyncNtuple") 
    << "Invalid Configuration parameter 'trigger' = " << trigger_string << " !!\n";
  double tauPtForSwitchingTriggers = cfgProduceTauTauSyncNtuple.getParameter<double>("tauPtForSwitchingTriggers");
  
  std::string takeTauTriggerTurnOn_string = cfgProduceTauTauSyncNtuple.getParameter<std::string>("takeTauTriggerTurnOn");
  int takeTauTriggerTurnOn = -1;
  if      ( takeTauTriggerTurnOn_string == "tree"                  ) takeTauTriggerTurnOn = kTree;
  else if ( takeTauTriggerTurnOn_string == "HPScombIso3HitsMedium" ) takeTauTriggerTurnOn = kHPScombIso3HitsMedium;
  else if ( takeTauTriggerTurnOn_string == "MVAwLToldDMsTight"     ) takeTauTriggerTurnOn = kMVAwLToldDMsTight;
  else if ( takeTauTriggerTurnOn_string == "MVAwLToldDMsVTight"    ) takeTauTriggerTurnOn = kMVAwLToldDMsVTight;
  else throw cms::Exception("produceTauTauSyncNtuple") 
    << "Invalid Configuration parameter 'takeTauTriggerTurnOn' = " << takeTauTriggerTurnOn_string << " !!\n";
  
  std::string inputTreeName = cfgProduceTauTauSyncNtuple.getParameter<std::string>("inputTreeName");
  std::string outputTreeName = cfgProduceTauTauSyncNtuple.getParameter<std::string>("outputTreeName");

  std::cout << "adding branches:" << std::endl;
  std::vector<branchEntryBaseType*> addBranches;
  edm::ParameterSet cfgAddBranches = cfgProduceTauTauSyncNtuple.getParameter<edm::ParameterSet>("addBranches");
  vstring outputBranchNames = cfgAddBranches.getParameterNamesForType<std::string>();
  for ( vstring::const_iterator outputBranchName = outputBranchNames.begin();
	outputBranchName != outputBranchNames.end(); ++outputBranchName ) {
    std::string inputBranchName_and_Type = cfgAddBranches.getParameter<std::string>(*outputBranchName);
    size_t idx1 = inputBranchName_and_Type.find_last_of("/");
    std::string inputBranchName = "";
    std::string inputBranchType = "";
    std::string outputBranchType = "";
    if ( idx1 != std::string::npos ) {
      size_t idx2 = inputBranchName_and_Type.find_last_of("->");
      if ( idx2 > idx1 && idx2 != std::string::npos ) {
	inputBranchName = std::string(inputBranchName_and_Type, 0, idx1);
	inputBranchType = std::string(inputBranchName_and_Type, idx1 + 1, (idx2 - 2) - idx1);
	outputBranchType = std::string(inputBranchName_and_Type, idx2 + 1);
      } else {
	inputBranchName = std::string(inputBranchName_and_Type, 0, idx1);
	inputBranchType = std::string(inputBranchName_and_Type, idx1 + 1);
	outputBranchType = inputBranchType;
      }
    }
    if      ( inputBranchType == "F"       && outputBranchType == "D" ) addBranches.push_back(new branchEntryTypeFD(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "F"       && outputBranchType == "I" ) addBranches.push_back(new branchEntryTypeFI(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "I"       && outputBranchType == "I" ) addBranches.push_back(new branchEntryTypeII(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "l"       && outputBranchType == "l" ) addBranches.push_back(new branchEntryTypeULUL(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "l"       && outputBranchType == "l" ) addBranches.push_back(new branchEntryTypeULUL(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "l"       && outputBranchType == "l" ) addBranches.push_back(new branchEntryTypeULUL(inputBranchName, inputBranchType, *outputBranchName, outputBranchType));
    else if ( inputBranchType == "Formula" && outputBranchType == "D" ) addBranches.push_back(new branchEntryFormulaTypeD(inputBranchName, *outputBranchName, outputBranchType));
    else throw cms::Exception("produceTauTauSyncNtuple") 
      << "Invalid branch declaration = '" << inputBranchName_and_Type << "' for branch = '" << (*outputBranchName) << "' !!\n";
  }

  fwlite::InputSource inputFiles(cfg); 
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  std::string outputFileName = cfgProduceTauTauSyncNtuple.getParameter<std::string>("outputFileName");
  std::cout << " outputFileName = " << outputFileName << std::endl;

  int verbosity = ( cfgProduceTauTauSyncNtuple.exists("verbosity") ) ?
    cfgProduceTauTauSyncNtuple.getParameter<int>("verbosity") : 0;

  TChain* inputTree = new TChain(inputTreeName.data());
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end(); ++inputFileName ) {
    std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
    inputTree->AddFile(inputFileName->data());
  }

  if ( !(inputTree->GetListOfFiles()->GetEntries() >= 1) ) {
    throw cms::Exception("produceTauTauSyncNtuple") 
      << "Failed to identify input Tree !!\n";
  }

  // CV: need to call TChain::LoadTree before processing first event 
  //     in order to prevent ROOT causing a segmentation violation,
  //     cf. http://root.cern.ch/phpBB3/viewtopic.php?t=10062
  inputTree->LoadTree(0);

  std::cout << "input Tree contains " << inputTree->GetEntries() << " Entries in " << inputTree->GetListOfFiles()->GetEntries() << " files." << std::endl;
  for ( std::vector<branchEntryBaseType*>::iterator branch = addBranches.begin();
	branch != addBranches.end(); ++branch ) {
    (*branch)->setInputTree(inputTree);
  }

  Float_t leg1Px, leg1Py, leg1Pz, leg1En;
  inputTree->SetBranchAddress("l1Px", &leg1Px);
  inputTree->SetBranchAddress("l1Py", &leg1Py);
  inputTree->SetBranchAddress("l1Pz", &leg1Pz);
  inputTree->SetBranchAddress("l1E",  &leg1En);
  Int_t leg1DecayMode;
  inputTree->SetBranchAddress("l1DecayMode", &leg1DecayMode);
  Float_t leg1GenPt;
  inputTree->SetBranchAddress("l1GenPt", &leg1GenPt);
  Float_t leg1decayModeFinding, leg1byIsolationMedium, leg1againstElectronLoose;
  inputTree->SetBranchAddress("l1decayModeFinding", &leg1decayModeFinding);
  inputTree->SetBranchAddress("l1MediumDB3HIso", &leg1byIsolationMedium);
  inputTree->SetBranchAddress("l1againstElectronLooseMVA3", &leg1againstElectronLoose);
  Int_t leg1TrigMatched;
  inputTree->SetBranchAddress("l1TrigMatched_diTau", &leg1TrigMatched);
  Float_t leg1triggerWeight_diTau, leg1triggerEffMC_diTau, leg1triggerEffData_diTau;
  inputTree->SetBranchAddress("leg1triggerWeight_diTau", &leg1triggerWeight_diTau);
  inputTree->SetBranchAddress("leg1triggerEffMC_diTau", &leg1triggerEffMC_diTau);
  inputTree->SetBranchAddress("leg1triggerEffData_diTau", &leg1triggerEffData_diTau);

  Float_t leg2Px, leg2Py, leg2Pz, leg2En;
  inputTree->SetBranchAddress("l2Px", &leg2Px);
  inputTree->SetBranchAddress("l2Py", &leg2Py);
  inputTree->SetBranchAddress("l2Pz", &leg2Pz);
  inputTree->SetBranchAddress("l2E",  &leg2En);
  Int_t leg2DecayMode;
  inputTree->SetBranchAddress("l2DecayMode", &leg2DecayMode);
  Float_t leg2GenPt;
  inputTree->SetBranchAddress("l2GenPt", &leg2GenPt);
  Float_t leg2decayModeFinding, leg2byIsolationMedium, leg2againstElectronLoose;
  inputTree->SetBranchAddress("l2decayModeFinding", &leg2decayModeFinding);
  inputTree->SetBranchAddress("l2MediumDB3HIso", &leg2byIsolationMedium);
  inputTree->SetBranchAddress("l2againstElectronLooseMVA3", &leg2againstElectronLoose);
  Int_t leg2TrigMatched;
  inputTree->SetBranchAddress("l2TrigMatched_diTau", &leg2TrigMatched);
  Float_t leg2triggerWeight_diTau, leg2triggerEffMC_diTau, leg2triggerEffData_diTau;
  inputTree->SetBranchAddress("leg2triggerWeight_diTau", &leg2triggerWeight_diTau);
  inputTree->SetBranchAddress("leg2triggerEffMC_diTau", &leg2triggerEffMC_diTau);
  inputTree->SetBranchAddress("leg2triggerEffData_diTau", &leg2triggerEffData_diTau);

  Float_t jet1Px, jet1Py, jet1Pz, jet1En;
  inputTree->SetBranchAddress("jet1Px", &jet1Px);
  inputTree->SetBranchAddress("jet1Py", &jet1Py);
  inputTree->SetBranchAddress("jet1Pz", &jet1Pz);
  inputTree->SetBranchAddress("jet1E",  &jet1En);

  Float_t jet2Px, jet2Py, jet2Pz, jet2En;
  inputTree->SetBranchAddress("jet2Px", &jet2Px);
  inputTree->SetBranchAddress("jet2Py", &jet2Py);
  inputTree->SetBranchAddress("jet2Pz", &jet2Pz);
  inputTree->SetBranchAddress("jet2E",  &jet2En);

  Float_t vertexWeight;
  inputTree->SetBranchAddress("vertexWeight", &vertexWeight);  
  Float_t higgsPtWeightNom;
  inputTree->SetBranchAddress("higgsPtWeightNom", &higgsPtWeightNom);  
  Float_t genFilter = 1.0;
  Float_t tauSpin = 1.0;
  Float_t zmumusel = 1.0;
  Float_t muradcorr = 1.0;
  Float_t genDiTauMassVsGenDiTauPt = 1.0;
  Float_t genTau2EtaVsGenTau1Eta = 1.0;
  Float_t genTau2PtVsGenTau1Pt = 1.0;
  if ( isEmbed ) {
    if ( embedType == kPfEmbed ) {
      inputTree->SetBranchAddress("genFilter", &genFilter);
    } else if ( embedType == kRhEmbed ) {
      inputTree->SetBranchAddress("genFilter", &genFilter);
      inputTree->SetBranchAddress("tauSpin", &tauSpin);
      inputTree->SetBranchAddress("zmumusel", &zmumusel);
      inputTree->SetBranchAddress("muradcorr", &muradcorr);
      inputTree->SetBranchAddress("genDiTauMassVsGenDiTauPt", &genDiTauMassVsGenDiTauPt);
      inputTree->SetBranchAddress("genTau2EtaVsGenTau1Eta", &genTau2EtaVsGenTau1Eta); 
      inputTree->SetBranchAddress("genTau2PtVsGenTau1Pt", &genTau2PtVsGenTau1Pt); 
    } else assert(0);
  }
  
  TFile* outputFile = new TFile(outputFileName.data(), "RECREATE");
  TTree* outputTree = new TTree(outputTreeName.data(), outputTreeName.data());

  Double_t mcweight, puweight, effweight, decaymodeweight, embedweight, weight;
  outputTree->Branch("mcweight", &mcweight, "mcweight/D");
  outputTree->Branch("puweight", &puweight, "puweight/D");
  outputTree->Branch("effweight", &effweight, "effweight/D");
  outputTree->Branch("decaymodeweight", &decaymodeweight, "decaymodeweight/D");
  outputTree->Branch("embedweight", &embedweight, "embedweight/D");
  outputTree->Branch("weight", &weight, "weight/D");

  Double_t triggerWeight_diTau, triggerEffMC_diTau, triggerEffData_diTau;
  outputTree->Branch("triggerWeight_diTau", &triggerWeight_diTau, "triggerWeight_diTau/D");
  outputTree->Branch("triggerEffMC_diTau", &triggerEffMC_diTau, "triggerEffMC_diTau/D");
  outputTree->Branch("triggerEffData_diTau", &triggerEffData_diTau, "triggerEffData_diTau/D");

  Double_t antiele_1;
  Int_t passid_1, passiso_1;
  outputTree->Branch("antiele_1", &antiele_1, "antiele_1/D");
  outputTree->Branch("passid_1", &passid_1, "passid_1/I");
  outputTree->Branch("passiso_1", &passiso_1, "passiso_1/I");

  Double_t antiele_2;
  Int_t passid_2, passiso_2;
  outputTree->Branch("antiele_2", &antiele_2, "antiele_2/D");
  outputTree->Branch("passid_2", &passid_2, "passid_2/I");
  outputTree->Branch("passiso_2", &passiso_2, "passiso_2/I");

  Double_t mjj, jdeta, jdphi, dijetpt, dijetphi;
  outputTree->Branch("mjj", &mjj, "mjj/D");
  outputTree->Branch("jdeta", &jdeta, "jdeta/D");
  outputTree->Branch("jdphi", &jdphi, "jdphi/D");
  outputTree->Branch("dijetpt", &dijetpt, "dijetpt/D");
  outputTree->Branch("dijetphi", &dijetphi, "dijetphi/D");
  Double_t ptvis;
  outputTree->Branch("ptvis", &ptvis, "ptvis/D");

  Double_t higgsPtWeightNom_double;
  outputTree->Branch("higgsPtWeightNom", &higgsPtWeightNom_double, "higgsPtWeightNom/D");  
  Double_t genFilter_double, tauSpin_double, zmumusel_double, muradcorr_double, genDiTauMassVsGenDiTauPt_double, genTau2EtaVsGenTau1Eta_double, genTau2PtVsGenTau1Pt_double;
  outputTree->Branch("genFilter", &genFilter_double, "genFilter/D");
  outputTree->Branch("tauSpin_double", &tauSpin_double, "tauSpin/D");
  outputTree->Branch("zmumusel", &zmumusel_double, "zmumusel/D");
  outputTree->Branch("muradcorr", &muradcorr_double, "muradcorr/D");
  outputTree->Branch("genDiTauMassVsGenDiTauPt", &genDiTauMassVsGenDiTauPt_double, "genDiTauMassVsGenDiTauPt/D");
  outputTree->Branch("genTau2EtaVsGenTau1Eta", &genTau2EtaVsGenTau1Eta_double, "genTau2EtaVsGenTau1Eta/D");
  outputTree->Branch("genTau2PtVsGenTau1Pt", &genTau2PtVsGenTau1Pt_double, "genTau2PtVsGenTau1Pt/D");

  for ( std::vector<branchEntryBaseType*>::iterator branch = addBranches.begin();
	branch != addBranches.end(); ++branch ) {
    (*branch)->setOutputTree(outputTree);
  }

  int currentTreeNumber = inputTree->GetTreeNumber();

  int selectedEntries = 0;
  int numEntries = inputTree->GetEntries();
  for ( int iEntry = 0; iEntry < numEntries && (maxEvents == -1 || selectedEntries < maxEvents); ++iEntry ) {
    if ( iEntry > 0 && (iEntry % reportEvery) == 0 ) {
      std::cout << "processing Entry " << iEntry << " (" << selectedEntries << " Entries selected)" << std::endl;
    }

    inputTree->GetEntry(iEntry);

    // check if event passes sync-Ntuple selection
    reco::Candidate::LorentzVector leg1P4(leg1Px, leg1Py, leg1Pz, leg1En);
    if ( verbosity >= 2 ) {    
      std::cout << "leg1: Pt = " << leg1P4.pt() << ", eta = " << leg1P4.eta() << ", phi = " << leg1P4.phi() << std::endl
		<< " decayModeFinding = " << leg1decayModeFinding << ", byIsolationMedium = " << leg1byIsolationMedium << ", againstElectronLoose = " << leg1againstElectronLoose << std::endl
		<< " TrigMatched = " << leg1TrigMatched << std::endl;
    }
    reco::Candidate::LorentzVector leg2P4(leg2Px, leg2Py, leg2Pz, leg2En);
    if ( verbosity >= 2 ) {  
      std::cout << "leg2: Pt = " << leg2P4.pt() << ", eta = " << leg2P4.eta() << ", phi = " << leg2P4.phi() << std::endl
		<< " decayModeFinding = " << leg2decayModeFinding << ", byIsolationMedium = " << leg2byIsolationMedium << ", againstElectronLoose = " << leg2againstElectronLoose << std::endl
		<< " TrigMatched = " << leg2TrigMatched << std::endl;
    }

    bool isSelected = false;
    if ( leg1P4.pt() > 45. && TMath::Abs(leg1P4.eta()) < 2.1 &&
	 leg1decayModeFinding > 0.5 && leg1byIsolationMedium > 0.5 && leg1TrigMatched &&
	 leg2P4.pt() > 45. && TMath::Abs(leg2P4.eta()) < 2.1 &&
	 leg2decayModeFinding > 0.5 && leg2byIsolationMedium > 0.5 && leg2TrigMatched &&
	 (leg1againstElectronLoose > 0.5 || leg2againstElectronLoose > 0.5 ) ) isSelected = true;
    if ( !isSelected) continue;
       
    antiele_1    = leg1againstElectronLoose;
    passid_1     = leg1decayModeFinding;
    passiso_1    = leg1byIsolationMedium;
    
    antiele_2    = leg2againstElectronLoose;
    passid_2     = leg2decayModeFinding;
    passiso_2    = leg2byIsolationMedium;

    mcweight = vertexWeight;
    if ( isHiggsMC ) {
      mcweight *= higgsPtWeightNom;
    }

    puweight = vertexWeight;

    decaymodeweight = 1.0;
    if ( leg1GenPt > 1. && leg1DecayMode == 0 ) decaymodeweight *= 0.88; // CV: data/MC difference for taus to be reconstructed in 1-prong 0pi0 decay mode, determined by Phil
    if ( leg2GenPt > 1. && leg2DecayMode == 0 ) decaymodeweight *= 0.88;

    if ( takeTauTriggerTurnOn == kTree ) {
      triggerEffData_diTau = leg2triggerEffData_diTau*leg2triggerEffData_diTau;
      triggerEffMC_diTau   = leg1triggerEffMC_diTau*leg2triggerEffMC_diTau;
      triggerWeight_diTau  = leg1triggerWeight_diTau*leg2triggerWeight_diTau;
    } else {
      typedef double (*triggerEffPtr)(double, double, bool);
      triggerEffPtr tauTriggerEffData = 0;
      triggerEffPtr tauTriggerEffMC   = 0;
      if ( takeTauTriggerTurnOn == kHPScombIso3HitsMedium ) {
	tauTriggerEffData = eff2012IsoParkedTau_Arun_cutMedium;
	tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_cutMedium;
      } else if ( takeTauTriggerTurnOn == kMVAwLToldDMsTight ) {
	tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaTight;
	tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaTight;      
      } else if ( takeTauTriggerTurnOn == kMVAwLToldDMsVTight ) {
	tauTriggerEffData = eff2012IsoParkedTau_Arun_mvaVTight;
	tauTriggerEffMC   = eff2012IsoParkedTauMC_Arun_mvaVTight; 
      } 
      assert(tauTriggerEffData);
      assert(tauTriggerEffMC);
      triggerEffData_diTau = (*tauTriggerEffData)(leg1P4.pt(), leg1P4.eta(), true)*(*tauTriggerEffData)(leg2P4.pt(), leg2P4.eta(), true);
      triggerEffMC_diTau   = (*tauTriggerEffMC)(leg1P4.pt(), leg1P4.eta(), true)*(*tauTriggerEffMC)(leg2P4.pt(), leg2P4.eta(), true);
      triggerWeight_diTau  = ( triggerEffMC_diTau > 0. ) ? (triggerEffData_diTau/triggerEffMC_diTau) : 1.;
    }

    effweight = 1.0;
    if ( isEmbed ) {
      effweight *= (triggerEffData_diTau);
    } else {
      effweight *= triggerWeight_diTau;
    }

    embedweight = 1.0;
    if ( isEmbed ) {
      if ( embedType == kPfEmbed ) {
	embedweight *= genFilter;
      } else if ( embedType == kRhEmbed ) {
	embedweight *= genFilter;
	embedweight *= tauSpin;
	embedweight *= zmumusel;
	embedweight *= muradcorr;
	embedweight *= genDiTauMassVsGenDiTauPt;
	embedweight *= genTau2EtaVsGenTau1Eta;
	embedweight *= genTau2PtVsGenTau1Pt;
      } else assert(0);
    }

    weight = mcweight*decaymodeweight*effweight*embedweight;

    reco::Candidate::LorentzVector jet1P4(jet1Px, jet1Py, jet1Pz, jet1En);
    reco::Candidate::LorentzVector jet2P4(jet2Px, jet2Py, jet2Pz, jet2En);

    if ( jet1P4.pt() > 1. && jet2P4.pt() > 1. ) {
      reco::Candidate::LorentzVector diJetP4 = jet1P4 + jet2P4;
      mjj      = diJetP4.mass();
      jdeta    = TMath::Abs(jet1P4.eta() - jet2P4.eta());
      jdphi    = reco::deltaPhi(jet1P4.phi(), jet2P4.phi());
      dijetpt  = diJetP4.pt();
      dijetphi = diJetP4.phi();
    } else {
      mjj      = 0.;
      jdeta    = 0.;
      jdphi    = 0.;
      dijetpt  = 0.;
      dijetphi = 0.;
    }

    ptvis = (leg1P4 + leg2P4).pt();

    higgsPtWeightNom_double = higgsPtWeightNom;

    genFilter_double = genFilter;
    tauSpin_double = tauSpin;
    zmumusel_double = zmumusel;
    muradcorr_double = muradcorr;
    genDiTauMassVsGenDiTauPt_double = genDiTauMassVsGenDiTauPt;
    genTau2EtaVsGenTau1Eta_double = genTau2EtaVsGenTau1Eta;
    genTau2PtVsGenTau1Pt_double = genTau2PtVsGenTau1Pt;

    if ( inputTree->GetTreeNumber() != currentTreeNumber ) {
      for ( std::vector<branchEntryBaseType*>::iterator branch = addBranches.begin();
	    branch != addBranches.end(); ++branch ) {
	(*branch)->update();
      } 
      currentTreeNumber = inputTree->GetTreeNumber();
    }

    for ( std::vector<branchEntryBaseType*>::iterator branch = addBranches.begin();
	branch != addBranches.end(); ++branch ) {
      (*branch)->copyBranch();
    }    

    outputTree->Fill();
    ++selectedEntries;
  }
  std::cout << "--> " << outputTree->GetEntries() << " Entries pass selection." << std::endl;

  std::cout << "output Tree:" << std::endl;
  //outputTree->Print();
  //outputTree->Scan("*", "", "", 20, 0);

  std::cout << "writing output Tree to file = " << outputFileName << "." << std::endl;
  outputFile->cd();
  outputTree->Write();

  for ( std::vector<branchEntryBaseType*>::iterator it = addBranches.begin();
	it != addBranches.end(); ++it ) {
    delete (*it);
  }

  delete outputFile;

  delete inputTree;
  
  clock.Show("produceTauTauSyncNtuple");

  return 0;
}
