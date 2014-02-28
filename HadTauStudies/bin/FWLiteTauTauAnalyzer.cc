
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

typedef std::vector<std::string> vstring;
typedef std::vector<double> vdouble;

enum { kData, kSignal, kSM_Higgs, kZTT, kZLL, kZJ, kZTT_Embedded, kW, kTT, kTT_Embedded, kVV };

enum { kOS, kSS };

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

edm::ParameterSet makeHistManagerConfig(const std::string& process, const std::string& category, const std::string& bin, const std::string& central_or_shift)
{
  edm::ParameterSet cfg;
  cfg.addParameter<std::string>("process", process);
  cfg.addParameter<std::string>("category", category);
  cfg.addParameter<std::string>("bin", bin);
  cfg.addParameter<std::string>("central_or_shift", central_or_shift);
  return cfg;
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
  if      ( process_string == "Data"         ) process = kData;
  else if ( process_string.find("HiggsSUSYGluGlu") != std::string::npos || process_string.find("HiggsSUSYBB") != std::string::npos ) process = kSignal;
  else if ( process_string.find("HiggsGGH") != std::string::npos || process_string.find("HiggsVBF") != std::string::npos ) process = kSM_Higgs;
  else if ( process_string == "ZTT"          ) process = kZTT;
  else if ( process_string == "ZLL"          ) process = kZLL;
  else if ( process_string == "ZJ"           ) process = kZJ;
  else if ( process_string == "ZTT_Embedded" ) process = kZTT_Embedded;
  else if ( process_string == "W"            ) process = kW;
  else if ( process_string == "TT"           ) process = kTT;
  else if ( process_string == "TT_Embedded"  ) process = kTT_Embedded;
  else if ( process_string == "VV"           ) process = kVV;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'process' = " << process_string << " !!\n";

  bool isMC = !(process == kData);
  bool isEmbedded = (process == kZTT_Embedded || process == kTT_Embedded);

  double massPoint = ( process == kSignal ) ?
    cfgFWLiteTauTauAnalyzer.getParameter<double>("massPoint") : -1.;

  double lumiScale = ( process != kData ) ?
    cfgFWLiteTauTauAnalyzer.getParameter<double>("lumiScale") : 1.;

  vdouble stitchingWeights;
  if ( process == kZTT || process == kZLL || process == kZJ || process == kW ) {
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
  
  std::string region = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("region");
  int diTauChargeSel = -1;
  if      ( region.find("OS") != std::string::npos ) diTauChargeSel = kOS;
  else if ( region.find("SS") != std::string::npos ) diTauChargeSel = kSS;
  else throw cms::Exception("FWLiteTauTauAnalyzer") 
    << "Invalid Configuration parameter 'region' = " << region << " !!\n";

  bool applyTightBtag = cfgFWLiteTauTauAnalyzer.getParameter<bool>("applyTightBtag");

  vstring addWeights_string = cfgFWLiteTauTauAnalyzer.getParameter<vstring>("addWeights");

  std::string central_or_shift = cfgFWLiteTauTauAnalyzer.getParameter<std::string>("central_or_shift");

  fwlite::InputSource inputFiles(cfg); 
  int maxEvents = inputFiles.maxEvents();
  std::cout << " maxEvents = " << maxEvents << std::endl;
  unsigned reportEvery = inputFiles.reportAfter();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  edm::ParameterSet cfgHistManager_inclusive = makeHistManagerConfig(process_string, Form("tauTau_%s_inclusive", region.data()), tauPtBin, central_or_shift);
  TauTauHistManager histManager_inclusive(cfgHistManager_inclusive);
  histManager_inclusive.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_nobtag = makeHistManagerConfig(process_string, Form("tauTau_%s_nobtag", region.data()), tauPtBin, central_or_shift);
  TauTauHistManager histManager_nobtag(cfgHistManager_nobtag);
  histManager_nobtag.bookHistograms(fs);

  edm::ParameterSet cfgHistManager_btag = makeHistManagerConfig(process_string, Form("tauTau_%s_btag", region.data()), tauPtBin, central_or_shift);
  TauTauHistManager histManager_btag(cfgHistManager_btag);
  histManager_btag.bookHistograms(fs);

  TChain* inputTree = new TChain(treeName.data());
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end(); ++inputFileName ) {
    std::cout << "input Tree: adding file = " << (*inputFileName) << std::endl;
    inputTree->AddFile(inputFileName->data());
  }
  
  if ( !(inputTree->GetListOfFiles()->GetEntries() >= 1) ) {
    throw cms::Exception("preselectTreeTauIdMVA") 
      << "Failed to identify input Tree !!\n";
  }

  // CV: need to call TChain::LoadTree before processing first event 
  //     in order to prevent ROOT causing a segmentation violation,
  //     cf. http://root.cern.ch/phpBB3/viewtopic.php?t=10062
  inputTree->LoadTree(0);

  std::cout << "input Tree contains " << inputTree->GetEntries() << " Entries in " << inputTree->GetListOfFiles()->GetEntries() << " files." << std::endl;

  Float_t tau1Pt, tau1Eta, tau1Phi;
  inputTree->SetBranchAddress("l1Pt", &tau1Pt);
  inputTree->SetBranchAddress("l1Eta", &tau1Eta);
  inputTree->SetBranchAddress("l1Phi", &tau1Phi);
  Float_t tau1DecayMode;
  inputTree->SetBranchAddress("l1DecayMode", &tau1DecayMode);
  Float_t tau1rawMVA, tau1IsoPtSum;
  inputTree->SetBranchAddress("l1RawMVAwLT", &tau1rawMVA);
  inputTree->SetBranchAddress("l1RawDB3HIso", &tau1IsoPtSum);
  Int_t tau1IsTriggerMatched;
  inputTree->SetBranchAddress("l1TrigMatched_diTau", &tau1IsTriggerMatched);
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
  Int_t tau2IsTriggerMatched;
  inputTree->SetBranchAddress("l2TrigMatched_diTau", &tau2IsTriggerMatched);
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
  Float_t visMass, svFitMass;
  inputTree->SetBranchAddress("visMass", &visMass);
  inputTree->SetBranchAddress("svfitMass", &svFitMass);

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

  Float_t bjet1Pt, bjet1Eta, bjet1Phi;
  inputTree->SetBranchAddress("bjet1Pt", &bjet1Pt);
  inputTree->SetBranchAddress("bjet1Eta", &bjet1Eta);
  inputTree->SetBranchAddress("bjet1Phi", &bjet1Phi);
  Float_t bjet1BtagDiscr;
  inputTree->SetBranchAddress("bjet1Btag", &bjet1BtagDiscr);

  Float_t bjet2Pt, bjet2Eta, bjet2Phi;
  inputTree->SetBranchAddress("bjet2Pt", &bjet2Pt);
  inputTree->SetBranchAddress("bjet2Eta", &bjet2Eta);
  inputTree->SetBranchAddress("bjet2Phi", &bjet2Phi);
  Float_t bjet2BtagDiscr;
  inputTree->SetBranchAddress("bjet2Btag", &bjet2BtagDiscr);

  Float_t met;
  inputTree->SetBranchAddress("met", &met);
  
  Int_t numVertices;
  inputTree->SetBranchAddress("nVert", &numVertices);

  Int_t isZtt, isZttj, isZttll, isZj, isZee, isZmm;
  inputTree->SetBranchAddress("isZtt", &isZtt);
  inputTree->SetBranchAddress("isZttj", &isZttj);
  inputTree->SetBranchAddress("isZttll", &isZttll);
  inputTree->SetBranchAddress("isZj", &isZj);
  inputTree->SetBranchAddress("isZee", &isZee);
  inputTree->SetBranchAddress("isZmm", &isZmm);
  Int_t is2RealTaus;
  inputTree->SetBranchAddress("is2RealTaus", &is2RealTaus);
  
  Float_t NUP;
  inputTree->SetBranchAddress("NUP", &NUP);

  Float_t triggerWeight_diTau;
  inputTree->SetBranchAddress("triggerWeight_diTau", &triggerWeight_diTau);
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
  int selectedEntries = 0;
  for ( int iEntry = 0; iEntry < numEntries && (maxEvents == -1 || selectedEntries < maxEvents); ++iEntry ) {
    if ( iEntry > 0 && (iEntry % reportEvery) == 0 ) {
      std::cout << "processing Entry " << iEntry << " (" << selectedEntries << " Entries selected)" << std::endl;
    }
    
    inputTree->GetEntry(iEntry);

    // CV: general phase-space cuts (driven by trigger requirements)
    if ( !(TMath::Abs(tau1Eta) < 2.1 && tau1Pt > 45. && 
	   TMath::Abs(tau2Eta) < 2.1 && tau2Pt > 45.) ) continue;
    // CV: binning of events in tau Pt binning 
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
      if ( !tau1IsTriggerMatched ) continue;
      if ( !tau2IsTriggerMatched ) continue;
    }
 
    if ( diTauChargeSel == kOS && diTauCharge > +0.5 ) continue;
    if ( diTauChargeSel == kSS && diTauCharge < -0.5 ) continue;

    int nbJets = 0;
    if ( bjet1BtagDiscr > 0.679 || !applyTightBtag ) ++nbJets;
    if ( bjet2BtagDiscr > 0.679 || !applyTightBtag ) ++nbJets;

    int nElectrons = 0;
    if ( electron1Pt > 10. && TMath::Abs(electron1Eta) < 2.5 ) ++nElectrons;

    int nMuons = 0;
    if ( muon1Pt > 10. && TMath::Abs(muon1Eta) < 2.4 ) ++nMuons;

    if ( nElectrons > 0 || nMuons > 0 ) continue;

    //---------------------------------------------------------------------------
    // CV: process dependent selection criteria
    if ( process == kSignal && !(diTauGenMass > 0.7*massPoint && diTauGenMass < 1.3*massPoint) ) continue;

    if ( (process == kZTT || process == kZTT_Embedded) && !isZtt ) continue;
    if ( process == kZLL && !(isZttll || isZee || isZmm) ) continue;
    if ( process == kZJ && !(isZttj || isZj) ) continue;

    if ( process == kTT_Embedded && !is2RealTaus ) continue;
    //---------------------------------------------------------------------------
    
    Float_t evtWeight = 1.0;
    if ( isMC || isEmbedded ) {
      if ( isEmbedded ) {
	evtWeight *= triggerWeight_diTau;
	evtWeight *= embeddingWeight;
      }
      if ( tau1GenPt > 1.0 && TMath::Nint(tau1DecayMode) == 0 ) evtWeight *= 0.88;
      if ( tau2GenPt > 1.0 && TMath::Nint(tau2DecayMode) == 0 ) evtWeight *= 0.88;
      if ( process == kZTT || process == kZLL || process == kZJ || process == kW ) {
	Float_t stitchingWeight = 1.0;
	int idxNUP = TMath::Nint(NUP) - 5;
	if ( idxNUP >= 0 && idxNUP < (int)stitchingWeights.size() ) {
	  stitchingWeight *= stitchingWeights[idxNUP];
	}
	evtWeight *= stitchingWeight;
      }
      if ( isMC ) {
	evtWeight *= lumiScale;
	evtWeight *= pileupWeight;
      }
      for ( std::vector<weightEntryType*>::iterator addWeight = addWeights.begin();
	    addWeight != addWeights.end(); ++addWeight ) {
	evtWeight *= (*addWeight)->value_;
      }
    }

    histManager_inclusive.fillHistograms(
      tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1rawMVA, tau1IsoPtSum,
      tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2rawMVA, tau2IsoPtSum,
      dPhi, dEta, dR, 
      visMass, svFitMass, 
      jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
      jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr,
      bjet1Pt, bjet1Eta, bjet1Phi, 
      bjet2Pt, bjet2Eta, bjet2Phi, 
      met, numVertices, 
      evtWeight);
    if ( nbJets == 0 ) histManager_nobtag.fillHistograms(
      tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1rawMVA, tau1IsoPtSum,
      tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2rawMVA, tau2IsoPtSum,
      dPhi, dEta, dR, 
      visMass, svFitMass, 
      jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
      jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr,
      bjet1Pt, bjet1Eta, bjet1Phi, 
      bjet2Pt, bjet2Eta, bjet2Phi, 
      met, numVertices, 
      evtWeight);
    else if ( nbJets == 1 ) histManager_btag.fillHistograms(
      tau1Pt, tau1Eta, tau1Phi, TMath::Nint(tau1DecayMode), tau1rawMVA, tau1IsoPtSum,
      tau2Pt, tau2Eta, tau2Phi, TMath::Nint(tau2DecayMode), tau2rawMVA, tau2IsoPtSum,
      dPhi, dEta, dR, 
      visMass, svFitMass, 
      jet1Pt, jet1Eta, jet1Phi, jet1BtagDiscr, 
      jet2Pt, jet2Eta, jet2Phi, jet2BtagDiscr,
      bjet1Pt, bjet1Eta, bjet1Phi, 
      bjet2Pt, bjet2Eta, bjet2Phi, 
      met, numVertices, 
      evtWeight);
  }

  delete tau1Selection;
  delete tau2Selection;

  for ( std::vector<weightEntryType*>::iterator it = addWeights.begin();
	it != addWeights.end(); ++it ) {
    delete (*it);
  }

  delete inputTree;
  
  clock.Show("FWLiteTauTauAnalyzer");

  return 0;
}
