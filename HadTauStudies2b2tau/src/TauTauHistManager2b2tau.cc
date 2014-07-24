#include "LLRAnalysis/HadTauStudies2b2tau/interface/TauTauHistManager2b2tau.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "LLRAnalysis/HadTauStudies/interface/histogramAuxFunctions.h"

#include <TMath.h>

TauTauHistManager2b2tau::TauTauHistManager2b2tau(const edm::ParameterSet& cfg)
{
  process_          = cfg.getParameter<std::string>("process");
  category_         = cfg.getParameter<std::string>("category");
  tauPtBin_         = cfg.getParameter<std::string>("tauPtBin");
  tau1EtaBins_      = cfg.getParameter<vdouble>("tau1EtaBins");
  numTau1EtaBins_   = tau1EtaBins_.size() - 1;
  tau2EtaBins_      = cfg.getParameter<vdouble>("tau2EtaBins");
  numTau2EtaBins_   = tau2EtaBins_.size() - 1;
  central_or_shift_ = cfg.getParameter<std::string>("central_or_shift");
}

void TauTauHistManager2b2tau::bookHistograms(TFileDirectory& dir)
{
  const int numPtBinsS = 20;
  float ptBinningS[numPtBinsS + 1] = { 
    0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 160., 180., 200.
  };

  const int numPtBinsL = 26;
  float ptBinningL[numPtBinsL + 1] = { 
    0., 5., 10., 15., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 160., 180., 200.,
    250., 300., 400., 500., 700., 1000.
  };

  histogramTau1PtS_        = book1D(dir, "tau1PtS",        "#tau_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau1PtL_        = book1D(dir, "tau1PtL",        "#tau_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxTau1EtaBin = 0; idxTau1EtaBin < numTau1EtaBins_; ++idxTau1EtaBin ) {
    double tau1EtaMin = tau1EtaBins_[idxTau1EtaBin];
    double tau1EtaMax = tau1EtaBins_[idxTau1EtaBin + 1];
    std::string tauEtaBin_label = getParticleEtaLabel("tau", tau1EtaMin, tau1EtaMax, -1., 9.9);
    histogramTau1Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau1EtaMin, tau1EtaMax, book1D(dir, Form("tau1PtS_%s", tauEtaBin_label.data()), "#tau_{1} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramTau1Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau1EtaMin, tau1EtaMax, book1D(dir, Form("tau1PtL_%s", tauEtaBin_label.data()), "#tau_{1} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramTau1Eta_        = book1D(dir, "tau1Eta",        "#tau_{1} #eta", 60, -3.0, +3.0);
  histogramTau1Phi_        = book1D(dir, "tau1Phi",        "#tau_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau1DecayMode_  = book1D(dir, "tau1DecayMode",  "#tau_{1} Decay Mode", 15, -0.5, 14.5);
  histogramTau1GenMatch_   = book1D(dir, "tau1GenMatch_",  "#tau_{1} gen. Match", 5, -0.5, 4.5);
  histogramTau1IsoPtSum_   = book1D(dir, "tau1IsoDiscrL",  "#tau_{1} Isolation", 101, -0.05, 10.05);
  histogramTau1MVA_        = book1D(dir, "tau1IsoDiscrS",  "#tau_{1} MVA output", 105, -0.005, 1.045);

  histogramTau2PtS_        = book1D(dir, "tau2PtS",        "#tau_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau2PtL_        = book1D(dir, "tau2PtL",        "#tau_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  for ( int idxTau2EtaBin = 0; idxTau2EtaBin < numTau2EtaBins_; ++idxTau2EtaBin ) {
    double tau2EtaMin = tau2EtaBins_[idxTau2EtaBin];
    double tau2EtaMax = tau2EtaBins_[idxTau2EtaBin + 1];
    std::string tauEtaBin_label = getParticleEtaLabel("tau", -1., 9.9, tau2EtaMin, tau2EtaMax);
    histogramTau2Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau2EtaMin, tau2EtaMax, book1D(dir, Form("tau2PtS_%s", tauEtaBin_label.data()), "#tau_{2} P_{T} / GeV", numPtBinsS, ptBinningS)));
    histogramTau2Pt_inTauEtaBins_.push_back(new histogramIn1dParticleEtaBinsEntryType(
      tau2EtaMin, tau2EtaMax, book1D(dir, Form("tau2PtL_%s", tauEtaBin_label.data()), "#tau_{2} P_{T} / GeV", numPtBinsL, ptBinningL)));
  }
  histogramTau2Eta_        = book1D(dir, "tau2Eta",        "#tau_{2} #eta", 60, -3.0, +3.0);
  histogramTau2Phi_        = book1D(dir, "tau2Phi",        "#tau_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau2DecayMode_  = book1D(dir, "tau2DecayMode",  "#tau_{2} Decay Mode", 15, -0.5, 14.5);
  histogramTau2GenMatch_   = book1D(dir, "tau2GenMatch_",  "#tau_{2} gen. Match", 5, -0.5, 4.5);
  histogramTau2IsoPtSum_   = book1D(dir, "tau2IsoDiscrL",  "#tau_{2} Isolation", 101, -0.05, 10.05);
  histogramTau2MVA_        = book1D(dir, "tau2IsoDiscrS",  "#tau_{2} MVA output", 105, -0.005, 1.045);

  const int numMassBins_inclusive_and_nobtag = 31;
  float massBinning_inclusive_and_nobtag[numMassBins_inclusive_and_nobtag + 1] = { 
    0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 
    225., 250., 275., 300., 325., 350., 400., 500., 700., 1000., 1500.
  };
  const int numMassBins_btag = 18;
  float massBinning_btag[numMassBins_btag + 1] = { 
    0., 20., 40., 60., 80., 100., 120., 140., 160., 180., 200.,
    250., 300., 350., 400., 500., 700., 1000., 1500.
  };
  int numMassBins = -1;
  float* massBinning = 0;
  if ( category_.find("_inclusive") != std::string::npos || category_.find("_nobtag") != std::string::npos ) {
    numMassBins = numMassBins_inclusive_and_nobtag;
    massBinning = massBinning_inclusive_and_nobtag;
  } else if ( category_.find("_btag") != std::string::npos ) {
    numMassBins = numMassBins_btag;
    massBinning = massBinning_btag;
  } else throw cms::Exception("TauTauHistManager") 
      << "Invalid Configuration parameter 'category' = " << category_ << " !!\n";
  
  histogramJet1PtS_        = book1D(dir, "jet1PtS",        "Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet1PtL_        = book1D(dir, "jet1PtL",        "Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet1Eta_        = book1D(dir, "jet1Eta",        "Jet_{1} #eta", 60, -3.0, +3.0);
  histogramJet1Phi_        = book1D(dir, "jet1Phi",        "Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet1BtagDiscr_  = book1D(dir, "jet1BtagDiscr",  "Jet_{1} b-tag Discriminator", 105, -0.005, 1.045);

  histogramJet2PtS_        = book1D(dir, "jet2PtS",        "Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet2PtL_        = book1D(dir, "jet2PtL",        "Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramJet2Eta_        = book1D(dir, "jet2Eta",        "Jet_{2} #eta", 60, -3.0, +3.0);
  histogramJet2Phi_        = book1D(dir, "jet2Phi",        "Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet2BtagDiscr_  = book1D(dir, "jet2BtagDiscr",  "Jet_{2} b-tag Discriminator", 105, -0.005, 1.045);

  histogramNumJets_        = book1D(dir, "numJets",        "N_{Jet}", 10, -0.5, 9.5);

  histogramBJet1PtS_       = book1D(dir, "bJet1PtS",       "b-Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet1PtL_       = book1D(dir, "bJet1PtL",       "b-Jet_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramBJet1Eta_       = book1D(dir, "bJet1Eta",       "b-Jet_{1} #eta", 60, -3.0, +3.0);
  histogramBJet1Phi_       = book1D(dir, "bJet1Phi",       "b-Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramBJet2PtS_       = book1D(dir, "bJet2PtS",       "b-Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet2PtL_       = book1D(dir, "bJet2PtL",       "b-Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramBJet2Eta_       = book1D(dir, "bJet2Eta",       "b-Jet_{2} #eta", 60, -3.0, +3.0);
  histogramBJet2Phi_       = book1D(dir, "bJet2Phi",       "b-Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramNumBJetsLoose_  = book1D(dir, "numBJetsLoose",  "N_{b-Jet} (Loose)", 6, -0.5, 5.5);
  histogramNumBJetsMedium_ = book1D(dir, "numBJetsMedium", "N_{b-Jet} (Medium)", 6, -0.5, 5.5);

  histogramVisMassS_       = book1D(dir, "visMassS",       "M_{vis} / GeV", numMassBins, massBinning);
  histogramVisMassL_       = book1D(dir, "visMassL",       "M_{vis} / GeV", 400, 0., 2000.);
  histogramSVfitMassS_     = book1D(dir, "svFitMassS",     "M_{#tau#tau} / GeV", numMassBins, massBinning);
  histogramSVfitMassL_     = book1D(dir, "svFitMassL",     "M_{#tau#tau} / GeV", 400, 0., 2000.);
  histogramMtTotalS_       = book1D(dir, "mTtotalS",       "m_{T}^{total} / GeV", numMassBins, massBinning);
  histogramMtTotalL_       = book1D(dir, "mTtotalL",       "m_{T}^{total} / GeV", 400, 0., 2000.);
  histogramDeltaPhitt_     = book1D(dir, "deltaPhitt",     "#Delta #phi (#tau_{1},#tau_{2})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaEtatt_     = book1D(dir, "deltaEtatt",     "#Delta #eta (#tau_{1},#tau_{2})", 50, 0., 5.);
  histogramDeltaRtt_       = book1D(dir, "deltaRtt"  ,     "#Delta R (#tau_{1},#tau_{2})", 70, 0., 7.);
  histogramMbbS_           = book1D(dir, "mbbS",           "M_{bb} / GeV", numMassBins, massBinning);
  histogramMbbL_           = book1D(dir, "mbbL",           "M_{bb} / GeV", 400, 0., 2000.);
  histogramDeltaPhibb_     = book1D(dir, "deltaPhibb",     "#Delta #phi (b-Jet_{1},b-Jet_{2})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaEtabb_     = book1D(dir, "deltaEtabb",     "#Delta #eta (b-Jet_{1},b-Jet_{2})", 50, 0., 5.);
  histogramDeltaRbb_       = book1D(dir, "deltaRbb"  ,     "#Delta R (b-Jet_{1},b-Jet_{2})", 70, 0., 7.);
  histogramDeltaPhiHbbHtt_ = book1D(dir, "deltaPhiHbbHtt", "#Delta #phi (H_{bb},H_{#tau#tau})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaPhiHbbMEt_ = book1D(dir, "deltaPhiHbbMEt", "#Delta #phi (H_{bb},E_{T}^{miss})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaPhiHttMEt_ = book1D(dir, "deltaPhiHttMEt", "#Delta #phi (H_{#tau#tau},E_{T}^{miss})", 36, -TMath::Pi(), TMath::Pi());
  histogramMinDeltaRbt_    = book1D(dir, "minDeltaRbt"  ,  "min. #Delta R (b-Jet,#tau)", 70, 0., 7.);
  histogramHbbPtS_         = book1D(dir, "HbbPtS",         "h_{bb} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramHbbPtL_         = book1D(dir, "HbbPtL",         "h_{bb} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramHttPtS_         = book1D(dir, "HttPtS",         "h_{#tau#tau} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramHttPtL_         = book1D(dir, "HttPtL",         "h_{#tau#tau} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramMaxBJetPtS_     = book1D(dir, "maxBJetPtS",     "max. b-Jet P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramMaxBJetPtL_     = book1D(dir, "maxBJetPtL",     "max. b-Jet P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramAugMT2ed_       = book1D(dir, "augMT2ed",       "augMT2ed", 100, 0., 1000.);
  histogramDiHiggsPtS_     = book1D(dir, "diHiggsPtS",     "hh P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramDiHiggsPtL_     = book1D(dir, "diHiggsPtL",     "hh P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramDiHiggsMassS_   = book1D(dir, "diHiggsMassS",   "M_{hh} / GeV", numMassBins, massBinning);
  histogramDiHiggsMassL_   = book1D(dir, "diHiggsMassL",   "M_{hh} / GeV", 400, 0., 2000.);

  histogramMEtS_           = book1D(dir, "metS",           "E_{T}^{miss} / GeV", numPtBinsS, ptBinningS);
  histogramMEtL_           = book1D(dir, "metL",           "E_{T}^{miss} / GeV", numPtBinsL, ptBinningL);

  histogramNumVertices_    = book1D(dir, "numVertices",    "N_{vtx}", 50, -0.5, 49.5);

  histogramNUP_            = book1D(dir, "NUP",            "NUP", 15, -0.5, 14.5);

  histogramEventCounter_   = book1D(dir, "EventCounter",  "", 1, -0.5, +0.5);
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
}

void TauTauHistManager2b2tau::fillHistograms(
       double tau1Pt, double tau1Eta, double tau1Phi, int tau1DecayMode, int tau1GenMatch, double tau1IsoPtSum, double tau1MVA,					    
       double tau2Pt, double tau2Eta, double tau2Phi, int tau2DecayMode, int tau2GenMatch, double tau2IsoPtSum, double tau2MVA,					    
       double dPhitt, double dEtatt, double dRtt,       
       double jet1Pt, double jet1Eta, double jet1Phi, double jet1BtagDiscr, 
       double jet2Pt, double jet2Eta, double jet2Phi, double jet2BtagDiscr, int numJets,
       double bjet1Pt, double bjet1Eta, double bjet1Phi, 
       double bjet2Pt, double bjet2Eta, double bjet2Phi, 
       double dPhibb, double dEtabb, double dRbb, int numBJetsLoose, int numBJetsMedium, 
       double visMass, double svFitMass, double mTtotal, double mbb, 
       double dPhiHbbHtt, double dPhiHbbMEt, double dPhiHttMEt, double minDRbt,
       double HbbPt, double HttPt,
       double augMT2ed,
       double diHiggsPt, double diHiggsMass,
       double met, int numVertices, double NUP,
       double evtWeight, double evtWeightErr)
{
  double absTau1Eta = TMath::Abs(tau1Eta);
  double absTau2Eta = TMath::Abs(tau2Eta);

  fillWithOverFlow(histogramTau1PtS_, tau1Pt, evtWeight, evtWeightErr);
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
  
  if ( jet1Pt > 1. ) {
    fillWithOverFlow(histogramJet1PtS_, jet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1PtL_, jet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1Eta_, jet1Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1Phi_, jet1Phi, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet1BtagDiscr_, jet1BtagDiscr, evtWeight, evtWeightErr);
  }

  if ( jet2Pt > 1. ) {
    fillWithOverFlow(histogramJet2PtS_, jet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2PtL_, jet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2Eta_, jet2Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2Phi_, jet2Phi, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramJet2BtagDiscr_, jet2BtagDiscr, evtWeight, evtWeightErr);
  }

  fillWithOverFlow(histogramNumJets_, numJets, evtWeight, evtWeightErr);

  if ( bjet1Pt > 1. ) {
    fillWithOverFlow(histogramBJet1PtS_, bjet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1PtL_, bjet1Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1Eta_, bjet1Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet1Phi_, bjet1Phi, evtWeight, evtWeightErr);
  }

  if ( bjet2Pt > 1. ) {
    fillWithOverFlow(histogramBJet2PtS_, bjet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2PtL_, bjet2Pt, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2Eta_, bjet2Eta, evtWeight, evtWeightErr);
    fillWithOverFlow(histogramBJet2Phi_, bjet2Phi, evtWeight, evtWeightErr);
  }

  fillWithOverFlow(histogramNumBJetsLoose_, numBJetsLoose, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramNumBJetsMedium_, numBJetsMedium, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramVisMassS_, visMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramVisMassL_, visMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramSVfitMassS_, svFitMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramSVfitMassL_, svFitMass, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMtTotalS_, mTtotal, evtWeight, evtWeightErr); 
  fillWithOverFlow(histogramMtTotalL_, mTtotal, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhitt_, dPhitt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaEtatt_, dEtatt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaRtt_, dRtt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMbbS_, mbb, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMbbL_, mbb, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhibb_, dPhibb, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaEtabb_, dEtabb, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaRbb_, dRbb, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhiHbbHtt_, dPhiHbbHtt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhiHbbMEt_, dPhiHbbMEt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramDeltaPhiHttMEt_, dPhiHttMEt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMinDeltaRbt_, minDRbt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramHbbPtS_, HbbPt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramHbbPtL_, HbbPt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramHttPtS_, HttPt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramHttPtL_, HttPt, evtWeight, evtWeightErr);
  double maxBJetPt = TMath::Max(bjet1Pt, bjet2Pt);
  fillWithOverFlow(histogramMaxBJetPtS_, maxBJetPt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMaxBJetPtL_, maxBJetPt, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramAugMT2ed_, augMT2ed, evtWeight, evtWeightErr);
  fill(histogramDiHiggsPtS_, diHiggsPt, evtWeight, evtWeightErr);
  fill(histogramDiHiggsPtL_, diHiggsPt, evtWeight, evtWeightErr);
  fill(histogramDiHiggsMassS_, diHiggsMass, evtWeight, evtWeightErr);
  fill(histogramDiHiggsMassL_, diHiggsMass, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramMEtS_, met, evtWeight, evtWeightErr);
  fillWithOverFlow(histogramMEtL_, met, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramNumVertices_, numVertices, evtWeight, evtWeightErr);

  fillWithOverFlow(histogramNUP_, NUP, evtWeight, evtWeightErr); 

  fill(histogramEventCounter_, 0., evtWeight, evtWeightErr);  
  for ( std::vector<histogramIn2dParticleEtaBinsEntryType*>::iterator histogramEventCounter = histogramEventCounter_inTauEtaBins_.begin();
	histogramEventCounter != histogramEventCounter_inTauEtaBins_.end(); ++histogramEventCounter ) {
    (*histogramEventCounter)->Fill(absTau1Eta, absTau2Eta, 0., evtWeight, evtWeightErr); 
  }
}

TH1* TauTauHistManager2b2tau::book1D(TFileDirectory& dir,
				     const std::string& distribution, const std::string& title, int numBins, double min, double max)
{
  TDirectory* subdir = createHistogramSubdirectory(dir);
  subdir->cd();
  TH1* retVal = new TH1D(getHistogramName(distribution).data(), title.data(), numBins, min, max);
  if ( !retVal->GetSumw2N() ) retVal->Sumw2();
  histograms_.push_back(retVal);
  return retVal;
}
 
TH1* TauTauHistManager2b2tau::book1D(TFileDirectory& dir,
				     const std::string& distribution, const std::string& title, int numBins, float* binning)
{
  TDirectory* subdir = createHistogramSubdirectory(dir);
  subdir->cd();
  TH1* retVal = new TH1D(getHistogramName(distribution).data(), title.data(), numBins, binning);
  if ( !retVal->GetSumw2N() ) retVal->Sumw2();
  histograms_.push_back(retVal);
  return retVal;
}

TDirectory* TauTauHistManager2b2tau::createHistogramSubdirectory(TFileDirectory& dir)
{
  std::string fullSubdirName = std::string(category_);
  if ( tauPtBin_ != "" ) fullSubdirName.append("/").append(tauPtBin_);
  fullSubdirName.append("/").append(process_);
  TDirectory* subdir = createSubdirectory_recursively(dir, fullSubdirName);
  return subdir;
}
 
std::string TauTauHistManager2b2tau::getHistogramName(const std::string& distribution) const
{
  std::string retVal = std::string(process_);
  if ( !(central_or_shift_ == "" || central_or_shift_ == "central") ) retVal.append("_").append(central_or_shift_);
  retVal.append("_").append(distribution);
  return retVal;
}