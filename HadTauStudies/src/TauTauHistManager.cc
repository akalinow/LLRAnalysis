#include "LLRAnalysis/HadTauStudies/interface/TauTauHistManager.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"

#include <TMath.h>

TauTauHistManager::TauTauHistManager(const edm::ParameterSet& cfg)
{
  process_          = cfg.getParameter<std::string>("process");
  category_         = cfg.getParameter<std::string>("category");
  tauPtBin_         = cfg.getParameter<std::string>("tauPtBin");
  tau1EtaBins_      = cfg.getParameter<vdouble>("tau1EtaBins");
  numTau1EtaBins_   = tau1EtaBins_.size() - 1;
  tau2EtaBins_      = cfg.getParameter<vdouble>("tau2EtaBins");
  numTau2EtaBins_   = tau2EtaBins_.size() - 1;
  bJet1EtaBins_     = cfg.getParameter<vdouble>("bJet1EtaBins");
  numBJet1EtaBins_  = bJet1EtaBins_.size() - 1;
  bJet2EtaBins_     = cfg.getParameter<vdouble>("bJet2EtaBins");
  numBJet2EtaBins_  = bJet2EtaBins_.size() - 1;
  central_or_shift_ = cfg.getParameter<std::string>("central_or_shift");
}

void TauTauHistManager::bookHistograms(TFileDirectory& dir)
{
  const int numPtBinsS = 20;
  float ptBinningS[numPtBinsS + 1] = { 
    0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 160., 180., 200.
  };

  const int numPtBinsM = 24;
  float ptBinningM[numPtBinsM + 1] = { 
    0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 160., 180., 200.,
    250., 300., 400., 500.
  };

  const int numPtBinsL = 26;
  float ptBinningL[numPtBinsL + 1] = { 
    0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 160., 180., 200.,
    250., 300., 400., 500., 700., 1000.
  };

  histogramTau1PtS_       = book1D(dir, "tau1PtS",       "#tau_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau1PtM_       = book1D(dir, "tau1PtM",       "#tau_{1} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramTau1PtL_       = book1D(dir, "tau1PtL",       "#tau_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxTau1EtaBin = 0; idxTau1EtaBin < numTau1EtaBins_; ++idxTau1EtaBin ) {
    double tau1EtaMin = tau1EtaBins_[idxTau1EtaBin];
    double tau1EtaMax = tau1EtaBins_[idxTau1EtaBin + 1];
    std::string tauEtaBin_label = getParticleEtaLabel("tau", tau1EtaMin, tau1EtaMax, -1., 9.9);
    histogramTau1Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau1EtaMin, tau1EtaMax, book1D(dir, Form("tau1PtS_%s", tauEtaBin_label.data()), "#tau_{1} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramTau1Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau1EtaMin, tau1EtaMax, book1D(dir, Form("tau1PtM_%s", tauEtaBin_label.data()), "#tau_{1} P_{T} / GeV", numPtBinsM, ptBinningM)));
    histogramTau1Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau1EtaMin, tau1EtaMax, book1D(dir, Form("tau1PtL_%s", tauEtaBin_label.data()), "#tau_{1} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramTau1Eta_       = book1D(dir, "tau1Eta",       "#tau_{1} #eta", 60, -3.0, +3.0);
  histogramTau1Phi_       = book1D(dir, "tau1Phi",       "#tau_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau1DecayMode_ = book1D(dir, "tau1DecayMode", "#tau_{1} Decay Mode", 15, -0.5, 14.5);
  histogramTau1GenMatch_  = book1D(dir, "tau1GenMatch_", "#tau_{1} gen. Match", 5, -0.5, 4.5);
  histogramTau1IsoPtSum_  = book1D(dir, "tau1IsoDiscrL", "#tau_{1} Isolation", 101, -0.05, 10.05);
  histogramTau1MVA_       = book1D(dir, "tau1IsoDiscrS", "#tau_{1} MVA output", 105, -0.005, 1.045);

  histogramTau2PtS_       = book1D(dir, "tau2PtS",       "#tau_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau2PtM_       = book1D(dir, "tau2PtM",       "#tau_{2} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramTau2PtL_       = book1D(dir, "tau2PtL",       "#tau_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxTau2EtaBin = 0; idxTau2EtaBin < numTau2EtaBins_; ++idxTau2EtaBin ) {
    double tau2EtaMin = tau2EtaBins_[idxTau2EtaBin];
    double tau2EtaMax = tau2EtaBins_[idxTau2EtaBin + 1];
    std::string tauEtaBin_label = getParticleEtaLabel("tau", -1., 9.9, tau2EtaMin, tau2EtaMax);
    histogramTau2Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau2EtaMin, tau2EtaMax, book1D(dir, Form("tau2PtS_%s", tauEtaBin_label.data()), "#tau_{2} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramTau2Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau2EtaMin, tau2EtaMax, book1D(dir, Form("tau2PtM_%s", tauEtaBin_label.data()), "#tau_{2} P_{T} / GeV", numPtBinsM, ptBinningM)));
    histogramTau2Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau2EtaMin, tau2EtaMax, book1D(dir, Form("tau2PtL_%s", tauEtaBin_label.data()), "#tau_{2} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramTau2Eta_       = book1D(dir, "tau2Eta",       "#tau_{2} #eta", 60, -3.0, +3.0);
  histogramTau2Phi_       = book1D(dir, "tau2Phi",       "#tau_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau2DecayMode_ = book1D(dir, "tau2DecayMode", "#tau_{2} Decay Mode", 15, -0.5, 14.5);
  histogramTau2GenMatch_  = book1D(dir, "tau2GenMatch_", "#tau_{2} gen. Match", 5, -0.5, 4.5);
  histogramTau2IsoPtSum_  = book1D(dir, "tau2IsoDiscrL", "#tau_{2} Isolation", 101, -0.05, 10.05);
  histogramTau2MVA_       = book1D(dir, "tau2IsoDiscrS", "#tau_{2} MVA output", 105, -0.005, 1.045);

  const int numMassBins_inclusive_and_nobtag = 31;
  float massBinning_inclusive_and_nobtag[numMassBins_inclusive_and_nobtag + 1] = { 
    0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 
    225., 250., 275., 300., 325., 350., 400., 500., 700., 1000., 1500.
  };
  const int numMassBins_inclusive_and_nobtag_wOverflow = 32;
  float massBinning_inclusive_and_nobtag_wOverflow[numMassBins_inclusive_and_nobtag_wOverflow + 1] = { 
    0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 
    225., 250., 275., 300., 325., 350., 400., 500., 700., 1000., 1500., 2000.
  };
  const int numMassBins_btag = 18;
  float massBinning_btag[numMassBins_btag + 1] = { 
    0., 20., 40., 60., 80., 100., 120., 140., 160., 180., 200.,
    250., 300., 350., 400., 500., 700., 1000., 1500.
  };
  const int numMassBins_btag_wOverflow = 19;
  float massBinning_btag_wOverflow[numMassBins_btag_wOverflow + 1] = { 
    0., 20., 40., 60., 80., 100., 120., 140., 160., 180., 200.,
    250., 300., 350., 400., 500., 700., 1000., 1500., 2000.
  };
  int numMassBins = -1;
  float* massBinning = 0;
  int numMassBins_wOverflow = -1;
  float* massBinning_wOverflow = 0;
  if ( category_.find("_inclusive") != std::string::npos || category_.find("_nobtag") != std::string::npos ) {
    numMassBins = numMassBins_inclusive_and_nobtag;
    massBinning = massBinning_inclusive_and_nobtag;
    numMassBins_wOverflow = numMassBins_inclusive_and_nobtag_wOverflow;
    massBinning_wOverflow = massBinning_inclusive_and_nobtag_wOverflow;
  } else if ( category_.find("_btag") != std::string::npos ) {
    numMassBins = numMassBins_btag;
    massBinning = massBinning_btag;
    numMassBins_wOverflow = numMassBins_btag_wOverflow;
    massBinning_wOverflow = massBinning_btag_wOverflow;
  } else throw cms::Exception("TauTauHistManager") 
      << "Invalid Configuration parameter 'category' = " << category_ << " !!\n";
  
  histogramVisMassS_            = book1D(dir, "visMassS",            "M_{vis} / GeV", numMassBins, massBinning);
  histogramVisMassSwOverflow_   = book1D(dir, "visMassSwOverflow",   "M_{vis} / GeV", numMassBins_wOverflow, massBinning_wOverflow);
  histogramVisMassL_            = book1D(dir, "visMassL",            "M_{vis} / GeV", 400, 0., 2000.);  
  histogramSVfitMassS_          = book1D(dir, "svFitMassS",          "M_{#tau#tau} / GeV", numMassBins, massBinning);
  histogramSVfitMassSwOverflow_ = book1D(dir, "svFitMassSwOverflow", "M_{#tau#tau} / GeV", numMassBins_wOverflow, massBinning_wOverflow);
  histogramSVfitMassL_          = book1D(dir, "svFitMassL",          "M_{#tau#tau} / GeV", 400, 0., 2000.);
  histogramMtTotalS_            = book1D(dir, "mTtotalS",            "m_{T}^{total} / GeV", numMassBins, massBinning);
  histogramMtTotalSwOverflow_   = book1D(dir, "mTtotalSwOverflow",   "m_{T}^{total} / GeV", numMassBins, massBinning);
  histogramMtTotalL_            = book1D(dir, "mTtotalL",            "m_{T}^{total} / GeV", 400, 0., 2000.);
  histogramDeltaPhi_            = book1D(dir, "deltaPhi",            "#Delta #phi (#tau_{1},#tau_{2})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaEta_            = book1D(dir, "deltaEta",            "#Delta #eta (#tau_{1},#tau_{2})", 50, 0., 5.);
  histogramDeltaR_              = book1D(dir, "deltaR"  ,            "#Delta R (#tau_{1},#tau_{2})", 70, 0., 7.);

  histogramJet1PtS_             = book1D(dir, "jet1PtS",             "Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet1PtM_             = book1D(dir, "jet1PtM",             "Jet_{1} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramJet1PtL_             = book1D(dir, "jet1PtL",             "Jet_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramJet1Eta_             = book1D(dir, "jet1Eta",             "Jet_{1} #eta", 60, -3.0, +3.0);
  histogramJet1Phi_             = book1D(dir, "jet1Phi",             "Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet1BtagDiscr_       = book1D(dir, "jet1BtagDiscr",       "Jet_{1} b-tag Discriminator", 105, -0.005, 1.045);

  histogramJet2PtS_             = book1D(dir, "jet2PtS",             "Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet2PtM_             = book1D(dir, "jet2PtM",             "Jet_{2} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramJet2PtL_             = book1D(dir, "jet2PtL",             "Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramJet2Eta_             = book1D(dir, "jet2Eta",             "Jet_{2} #eta", 60, -3.0, +3.0);
  histogramJet2Phi_             = book1D(dir, "jet2Phi",             "Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet2BtagDiscr_       = book1D(dir, "jet2BtagDiscr",       "Jet_{2} b-tag Discriminator", 105, -0.005, 1.045);

  histogramNumJets_             = book1D(dir, "numJets",             "N_{Jet}", 10, -0.5, 9.5);

  histogramBJet1PtS_            = book1D(dir, "bJet1PtS",            "b-Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet1PtM_            = book1D(dir, "bJet1PtM",            "b-Jet_{1} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramBJet1PtL_            = book1D(dir, "bJet1PtL",            "b-Jet_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxBJet1EtaBin = 0; idxBJet1EtaBin < numBJet1EtaBins_; ++idxBJet1EtaBin ) {
    double bJet1EtaMin = bJet1EtaBins_[idxBJet1EtaBin];
    double bJet1EtaMax = bJet1EtaBins_[idxBJet1EtaBin + 1];
    std::string bJetEtaBin_label = getParticleEtaLabel("bJet", bJet1EtaMin, bJet1EtaMax, -1., 9.9);
    histogramBJet1Pt_inBJetEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      bJet1EtaMin, bJet1EtaMax, book1D(dir, Form("bJet1PtS_%s", bJetEtaBin_label.data()), "#bJet_{1} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramBJet1Pt_inBJetEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      bJet1EtaMin, bJet1EtaMax, book1D(dir, Form("bJet1PtL_%s", bJetEtaBin_label.data()), "#bJet_{1} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramBJet1Eta_            = book1D(dir, "bJet1Eta",            "b-Jet_{1} #eta", 60, -3.0, +3.0);
  histogramBJet1Phi_            = book1D(dir, "bJet1Phi",            "b-Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramBJet2PtS_            = book1D(dir, "bJet2PtS",            "b-Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet2PtM_            = book1D(dir, "bJet2PtM",            "b-Jet_{2} P_{T} / GeV", numPtBinsM, ptBinningM);
  histogramBJet2PtL_            = book1D(dir, "bJet2PtL",            "b-Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxBJet2EtaBin = 0; idxBJet2EtaBin < numBJet2EtaBins_; ++idxBJet2EtaBin ) {
    double bJet2EtaMin = bJet2EtaBins_[idxBJet2EtaBin];
    double bJet2EtaMax = bJet2EtaBins_[idxBJet2EtaBin + 1];
    std::string bJetEtaBin_label = getParticleEtaLabel("bJet", bJet2EtaMin, bJet2EtaMax, -1., 9.9);
    histogramBJet2Pt_inBJetEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      bJet2EtaMin, bJet2EtaMax, book1D(dir, Form("bJet2PtS_%s", bJetEtaBin_label.data()), "#bJet_{2} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramBJet2Pt_inBJetEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      bJet2EtaMin, bJet2EtaMax, book1D(dir, Form("bJet2PtL_%s", bJetEtaBin_label.data()), "#bJet_{2} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramBJet2Eta_            = book1D(dir, "bJet2Eta",            "b-Jet_{2} #eta", 60, -3.0, +3.0);
  histogramBJet2Phi_            = book1D(dir, "bJet2Phi",            "b-Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramNumBJets_            = book1D(dir, "numBJets",            "N_{b-Jet}", 6, -0.5, 5.5);

  histogramMEtS_                = book1D(dir, "metS",                "E_{T}^{miss} / GeV", numPtBinsS, ptBinningS);
  histogramMEtM_                = book1D(dir, "metM",                "E_{T}^{miss} / GeV", numPtBinsM, ptBinningM);
  histogramMEtL_                = book1D(dir, "metL",                "E_{T}^{miss} / GeV", numPtBinsL, ptBinningL);

  histogramNumVertices_         = book1D(dir, "numVertices",         "N_{vtx}", 50, -0.5, 49.5);
  histogramEvtVertexZ_          = book1D(dir, "evtVertexZ",          "vtx_{z}", 100, -50., +50.);
  histogramEvtVertexRho_        = book1D(dir, "evtVertexRho",        "vtx_{#rho}", 100, 0., 10.);
  histogramEvtVertexNDoF_       = book1D(dir, "evtVertexNDoF",       "vtx_{nDoF}", 100, 0., 100.);
  histogramEvtVertex_isValid_   = book1D(dir, "evtVertexisValid",    "vtx_{z}", 2, -0.5, +1.5);

  histogramGenHiggsPt_          = book1D(dir, "genHiggsPt",          "gen. P_{T}^{#Phi} / GeV", 100, 0., 500.);
  histogramNUP_                 = book1D(dir, "NUP",                 "NUP", 15, -0.5, 14.5);

  histogramEventCounter_  = book1D(dir, "EventCounter",  "", 1, -0.5, +0.5);
  for ( int idxTau1EtaBin = 0; idxTau1EtaBin < numTau1EtaBins_; ++idxTau1EtaBin ) {
    double tau1EtaMin = tau1EtaBins_[idxTau1EtaBin];
    double tau1EtaMax = tau1EtaBins_[idxTau1EtaBin + 1];
    for ( int idxTau2EtaBin = 0; idxTau2EtaBin < numTau2EtaBins_; ++idxTau2EtaBin ) {
      double tau2EtaMin = tau2EtaBins_[idxTau2EtaBin];
      double tau2EtaMax = tau2EtaBins_[idxTau2EtaBin + 1];
      std::string tauEtaBin_label = getParticleEtaLabel("tau", tau1EtaMin, tau1EtaMax, tau2EtaMin, tau2EtaMax);
      histogramEventCounter_inTauEtaBins_.push_back(new histogramIn2dParticleEtaBinsEntryType(
	tau1EtaMin, tau1EtaMax, tau2EtaMin, tau2EtaMax, book1D(dir, Form("EventCounter_%s", tauEtaBin_label.data()), "", 1, -0.5, +0.5)));
    }
  }
  for ( int idxBJet1EtaBin = 0; idxBJet1EtaBin < numBJet1EtaBins_; ++idxBJet1EtaBin ) {
    double bJet1EtaMin = bJet1EtaBins_[idxBJet1EtaBin];
    double bJet1EtaMax = bJet1EtaBins_[idxBJet1EtaBin + 1];
    for ( int idxBJet2EtaBin = 0; idxBJet2EtaBin < numBJet2EtaBins_; ++idxBJet2EtaBin ) {
      double bJet2EtaMin = bJet2EtaBins_[idxBJet2EtaBin];
      double bJet2EtaMax = bJet2EtaBins_[idxBJet2EtaBin + 1];
      std::string bJetEtaBin_label = getParticleEtaLabel("bJet", bJet1EtaMin, bJet1EtaMax, bJet2EtaMin, bJet2EtaMax);
      histogramEventCounter_inBJetEtaBins_.push_back(new histogramIn2dParticleEtaBinsEntryType(
	bJet1EtaMin, bJet1EtaMax, bJet2EtaMin, bJet2EtaMax, book1D(dir, Form("EventCounter_%s", bJetEtaBin_label.data()), "", 1, -0.5, +0.5)));
    }
  }
}

void TauTauHistManager::fillHistograms(
       double tau1Pt, double tau1Eta, double tau1Phi, int tau1DecayMode, int tau1GenMatch, double tau1IsoPtSum, double tau1MVA,					    
       double tau2Pt, double tau2Eta, double tau2Phi, int tau2DecayMode, int tau2GenMatch, double tau2IsoPtSum, double tau2MVA,					    
       double dPhi, double dEta, double dR, 
       double visMass, double svFitMass, double mTtotal, 
       double jet1Pt, double jet1Eta, double jet1Phi, double jet1BtagDiscr, 
       double jet2Pt, double jet2Eta, double jet2Phi, double jet2BtagDiscr, int numJets,
       double bjet1Pt, double bjet1Eta, double bjet1Phi, 
       double bjet2Pt, double bjet2Eta, double bjet2Phi, int numBJets,
       double met, int numVertices, double evtVertexZ, double evtVertexRho, double evtVertexNDoF, int evtVertex_isValid, double genHiggsPt, double NUP,
       double evtWeight, double evtWeightErr)
{
  double absTau1Eta = TMath::Abs(tau1Eta);
  double absTau2Eta = TMath::Abs(tau2Eta);

  fillWithOverFlow(histogramTau1PtS_, tau1Pt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1PtM_, tau1Pt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1PtL_, tau1Pt, evtWeight, evtWeightErr);
  for ( std::vector<histogramIn1dParticleEtaBinsEntryType*>::iterator histogramTau1Pt = histogramTau1Pt_inTauEtaBins_.begin();
	histogramTau1Pt != histogramTau1Pt_inTauEtaBins_.end(); ++histogramTau1Pt ) {
    (*histogramTau1Pt)->Fill(absTau1Eta, tau1Pt, evtWeight, evtWeightErr); 
  }
  fillWithOverFlow(histogramTau1Eta_, tau1Eta, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1Phi_, tau1Phi, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1DecayMode_, tau1DecayMode, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1GenMatch_, tau1GenMatch, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1IsoPtSum_, tau1IsoPtSum, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau1MVA_, tau1MVA, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramTau2PtS_, tau2Pt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2PtM_, tau2Pt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2PtL_, tau2Pt, evtWeight, evtWeightErr);
  for ( std::vector<histogramIn1dParticleEtaBinsEntryType*>::iterator histogramTau2Pt = histogramTau2Pt_inTauEtaBins_.begin();
	histogramTau2Pt != histogramTau2Pt_inTauEtaBins_.end(); ++histogramTau2Pt ) {
    (*histogramTau2Pt)->Fill(absTau2Eta, tau2Pt, evtWeight, evtWeightErr); 
  }
  fillWithOverFlow(histogramTau2Eta_, tau2Eta, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2Phi_, tau2Phi, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2DecayMode_, tau2DecayMode, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2GenMatch_, tau2GenMatch, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2IsoPtSum_, tau2IsoPtSum, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramTau2MVA_, tau2MVA, evtWeight, evtWeightErr);

  fill(histogramVisMassS_, visMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramVisMassSwOverflow_, visMass, evtWeight, evtWeightErr);
  fill(histogramVisMassL_, visMass, evtWeight, evtWeightErr);
  fill(histogramSVfitMassS_, svFitMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramSVfitMassSwOverflow_, svFitMass, evtWeight, evtWeightErr);
  fill(histogramSVfitMassL_, svFitMass, evtWeight, evtWeightErr);
  fill(histogramMtTotalS_, mTtotal, evtWeight, evtWeightErr); 
  fill(histogramMtTotalL_, mTtotal, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhi_, dPhi, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaEta_, dEta, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaR_, dR, evtWeight, evtWeightErr);
  
  if ( jet1Pt > 1. ) {
    fillWithOverFlow(histogramJet1PtS_, jet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1PtM_, jet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1PtL_, jet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1Eta_, jet1Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1Phi_, jet1Phi, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1BtagDiscr_, jet1BtagDiscr, evtWeight, evtWeightErr);
  }

  if ( jet2Pt > 1. ) {
    fillWithOverFlow(histogramJet2PtS_, jet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2PtM_, jet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2PtL_, jet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2Eta_, jet2Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2Phi_, jet2Phi, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2BtagDiscr_, jet2BtagDiscr, evtWeight, evtWeightErr);
  }

  fillWithOverFlow(histogramNumJets_, numJets, evtWeight, evtWeightErr);

  double absBJet1Eta = TMath::Abs(bjet1Eta);
  double absBJet2Eta = TMath::Abs(bjet2Eta);

  if ( bjet1Pt > 1. ) {
    fillWithOverFlow(histogramBJet1PtS_, bjet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1PtM_, bjet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1PtL_, bjet1Pt, evtWeight, evtWeightErr);
    for ( std::vector<histogramIn1dParticleEtaBinsEntryType*>::iterator histogramBJet1Pt = histogramBJet1Pt_inBJetEtaBins_.begin();
	  histogramBJet1Pt != histogramBJet1Pt_inBJetEtaBins_.end(); ++histogramBJet1Pt ) {
      (*histogramBJet1Pt)->Fill(absBJet1Eta, bjet1Pt, evtWeight, evtWeightErr); 
    }
    fillWithOverFlow(histogramBJet1Eta_, bjet1Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1Phi_, bjet1Phi, evtWeight, evtWeightErr);
  }

  if ( bjet2Pt > 1. ) {
    fillWithOverFlow(histogramBJet2PtS_, bjet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2PtM_, bjet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2PtL_, bjet2Pt, evtWeight, evtWeightErr);
    for ( std::vector<histogramIn1dParticleEtaBinsEntryType*>::iterator histogramBJet2Pt = histogramBJet2Pt_inBJetEtaBins_.begin();
	  histogramBJet2Pt != histogramBJet2Pt_inBJetEtaBins_.end(); ++histogramBJet2Pt ) {
      (*histogramBJet2Pt)->Fill(absBJet2Eta, bjet2Pt, evtWeight, evtWeightErr); 
    }
    fillWithOverFlow(histogramBJet2Eta_, bjet2Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2Phi_, bjet2Phi, evtWeight, evtWeightErr);
  }

  fillWithOverFlow(histogramNumBJets_, numBJets, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramMEtS_, met, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMEtL_, met, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramNumVertices_, numVertices, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramEvtVertexZ_, evtVertexZ, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramEvtVertexRho_, evtVertexRho, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramEvtVertexNDoF_, evtVertexNDoF, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramEvtVertex_isValid_, evtVertex_isValid, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramGenHiggsPt_, genHiggsPt, evtWeight, evtWeightErr); 
  fillWithOverFlow(histogramNUP_, NUP, evtWeight, evtWeightErr); 

  fill(histogramEventCounter_, 0., evtWeight, evtWeightErr);  
  for ( std::vector<histogramIn2dParticleEtaBinsEntryType*>::iterator histogramEventCounter = histogramEventCounter_inTauEtaBins_.begin();
	histogramEventCounter != histogramEventCounter_inTauEtaBins_.end(); ++histogramEventCounter ) {
    (*histogramEventCounter)->Fill(absTau1Eta, absTau2Eta, 0., evtWeight, evtWeightErr); 
  }
  if ( bjet1Pt > 1. ) { // CV: fill histogram for computing probabilities for jets to pass tight/loose b-tag discriminators for events with at least one b-jet
    for ( std::vector<histogramIn2dParticleEtaBinsEntryType*>::iterator histogramEventCounter = histogramEventCounter_inBJetEtaBins_.begin();
	  histogramEventCounter != histogramEventCounter_inBJetEtaBins_.end(); ++histogramEventCounter ) {
      (*histogramEventCounter)->Fill(absBJet1Eta, absBJet2Eta, 0., evtWeight, evtWeightErr); 
    }
  }
}

TH1* TauTauHistManager::book1D(TFileDirectory& dir,
			       const std::string& distribution, const std::string& title, int numBins, double min, double max)
{
  TDirectory* subdir = createHistogramSubdirectory(dir);
  subdir->cd();
  TH1* retVal = new TH1D(getHistogramName(distribution).data(), title.data(), numBins, min, max);
  if ( !retVal->GetSumw2N() ) retVal->Sumw2();
  histograms_.push_back(retVal);
  return retVal;
}
 
TH1* TauTauHistManager::book1D(TFileDirectory& dir,
			       const std::string& distribution, const std::string& title, int numBins, float* binning)
{
  TDirectory* subdir = createHistogramSubdirectory(dir);
  subdir->cd();
  TH1* retVal = new TH1D(getHistogramName(distribution).data(), title.data(), numBins, binning);
  if ( !retVal->GetSumw2N() ) retVal->Sumw2();
  histograms_.push_back(retVal);
  return retVal;
}

TDirectory* TauTauHistManager::createHistogramSubdirectory(TFileDirectory& dir)
{
  std::string fullSubdirName = std::string(category_);
  if ( tauPtBin_ != "" ) fullSubdirName.append("/").append(tauPtBin_);
  fullSubdirName.append("/").append(process_);
  TDirectory* subdir = createSubdirectory_recursively(dir, fullSubdirName);
  return subdir;
}
 
std::string TauTauHistManager::getHistogramName(const std::string& distribution) const
{
  std::string retVal = std::string(process_);
  if ( !(central_or_shift_ == "" || central_or_shift_ == "central") ) retVal.append("_").append(central_or_shift_);
  retVal.append("_").append(distribution);
  return retVal;
}
