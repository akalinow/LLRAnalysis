#include "LLRAnalysis/HadTauStudies/interface/TauTauHistManager.h"

#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include <TMath.h>

TauTauHistManager::TauTauHistManager(const edm::ParameterSet& cfg)
{
  process_          = cfg.getParameter<std::string>("process");
  category_         = cfg.getParameter<std::string>("category");
  bin_              = cfg.getParameter<std::string>("bin");
  central_or_shift_ = cfg.getParameter<std::string>("central_or_shift");
}

void TauTauHistManager::bookHistograms(TFileDirectory& dir)
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

  histogramTau1PtS_       = book1D(dir, "tau1PtS",       "#tau_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau1PtL_       = book1D(dir, "tau1PtL",       "#tau_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramTau1Eta_       = book1D(dir, "tau1Eta",       "#tau_{1} #eta", 60, -3.0, +3.0);
  histogramTau1Phi_       = book1D(dir, "tau1Phi",       "#tau_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau1DecayMode_ = book1D(dir, "tau1DecayMode", "#tau_{1} Decay Mode", 15, -0.5, 14.5);
  histogramTau1IsoDiscrS_ = book1D(dir, "tau1IsoDiscrS", "#tau_{1} MVA output", 105, -0.005, 1.045);
  histogramTau1IsoDiscrL_ = book1D(dir, "tau1IsoDiscrL", "#tau_{1} Isolation", 101, -0.05, 10.05);

  histogramTau2PtS_       = book1D(dir, "tau2PtS",       "#tau_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramTau2PtL_       = book1D(dir, "tau2PtL",       "#tau_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramTau2Eta_       = book1D(dir, "tau2Eta",       "#tau_{2} #eta", 60, -3.0, +3.0);
  histogramTau2Phi_       = book1D(dir, "tau2Phi",       "#tau_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramTau2DecayMode_ = book1D(dir, "tau2DecayMode", "#tau_{2} Decay Mode", 15, -0.5, 14.5);
  histogramTau2IsoDiscrS_ = book1D(dir, "tau2IsoDiscrS", "#tau_{2} MVA output", 105, -0.005, 1.045);
  histogramTau2IsoDiscrL_ = book1D(dir, "tau2IsoDiscrL", "#tau_{2} Isolation", 101, -0.05, 10.05);

  const int numMassBins = 31;
  float massBinning[numMassBins + 1] = { 
    0., 10., 20., 30., 40., 50., 60., 70., 80., 90., 100., 110., 120., 130., 140., 150., 160., 170., 180., 190., 200., 
    225., 250., 275., 300., 325., 350., 400., 500., 700., 1000., 1500.
  };

  histogramVisMassS_      = book1D(dir, "visMassS",      "M_{vis} / GeV", numMassBins, massBinning);
  histogramVisMassL_      = book1D(dir, "visMassL",      "M_{vis} / GeV", 400, 0., 2000.);
  histogramSVfitMassS_    = book1D(dir, "svFitMassS",    "M_{#tau#tau} / GeV", numMassBins, massBinning);
  histogramSVfitMassL_    = book1D(dir, "svFitMassL",    "M_{#tau#tau} / GeV", 400, 0., 2000.);
  histogramDeltaPhi_      = book1D(dir, "deltaPhi",      "#Delta #phi (#tau_{1},#tau_{2})", 36, -TMath::Pi(), TMath::Pi());
  histogramDeltaEta_      = book1D(dir, "deltaEta",      "#Delta #eta (#tau_{1},#tau_{2})", 50, 0., 5.);
  histogramDeltaR_        = book1D(dir, "deltaR"  ,      "#Delta R (#tau_{1},#tau_{2})", 70, 0., 7.);

  histogramJet1PtS_       = book1D(dir, "jet1PtS",       "Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet1PtL_       = book1D(dir, "jet1PtL",       "Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet1Eta_       = book1D(dir, "jet1Eta",       "Jet_{1} #eta", 60, -3.0, +3.0);
  histogramJet1Phi_       = book1D(dir, "jet1Phi",       "Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet1BtagDiscr_ = book1D(dir, "jet1BtagDiscr", "Jet_{1} b-tag Discriminator", 105, -0.005, 1.045);

  histogramJet2PtS_       = book1D(dir, "jet2PtS",       "Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramJet2PtL_       = book1D(dir, "jet2PtL",       "Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramJet2Eta_       = book1D(dir, "jet2Eta",       "Jet_{2} #eta", 60, -3.0, +3.0);
  histogramJet2Phi_       = book1D(dir, "jet2Phi",       "Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());
  histogramJet2BtagDiscr_ = book1D(dir, "jet2BtagDiscr", "Jet_{2} b-tag Discriminator", 105, -0.005, 1.045);

  histogramBJet1PtS_      = book1D(dir, "bJet1PtS",      "b-Jet_{1} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet1PtL_      = book1D(dir, "bJet1PtL",      "b-Jet_{1} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramBJet1Eta_      = book1D(dir, "bJet1Eta",      "b-Jet_{1} #eta", 60, -3.0, +3.0);
  histogramBJet1Phi_      = book1D(dir, "bJet1Phi",      "b-Jet_{1} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramBJet2PtS_      = book1D(dir, "bJet2PtS",      "b-Jet_{2} P_{T} / GeV", numPtBinsS, ptBinningS);
  histogramBJet2PtL_      = book1D(dir, "bJet2PtL",      "b-Jet_{2} P_{T} / GeV", numPtBinsL, ptBinningL);
  histogramBJet2Eta_      = book1D(dir, "bJet2Eta",      "b-Jet_{2} #eta", 60, -3.0, +3.0);
  histogramBJet2Phi_      = book1D(dir, "bJet2Phi",      "b-Jet_{2} #phi", 36, -TMath::Pi(), TMath::Pi());

  histogramMEtS_          = book1D(dir, "metS",          "E_{T}^{miss} / GeV", numPtBinsS, ptBinningS);
  histogramMEtL_          = book1D(dir, "metL",          "E_{T}^{miss} / GeV", numPtBinsL, ptBinningL);

  histogramNumVertices_   = book1D(dir, "numVertices",   "N_{vtx}", 50, -0.5, 49.5);

  histogramEventCounter_  = book1D(dir, "EventCounter",  "", 1, -0.5, +0.5);
}

void TauTauHistManager::fillHistograms(
       double tau1Pt, double tau1Eta, double tau1Phi, int tau1DecayMode, double tau1IsoDiscr1, double tau1IsoDiscr2,					    
       double tau2Pt, double tau2Eta, double tau2Phi, int tau2DecayMode, double tau2IsoDiscr1, double tau2IsoDiscr2,					    
       double dPhi, double dEta, double dR, 
       double visMass, double svFitMass, 
       double jet1Pt, double jet1Eta, double jet1Phi, double jet1BtagDiscr, 
       double jet2Pt, double jet2Eta, double jet2Phi, double jet2BtagDiscr,
       double bjet1Pt, double bjet1Eta, double bjet1Phi, 
       double bjet2Pt, double bjet2Eta, double bjet2Phi, 
       double met, int numVertices, 
       double evtWeight)
{
  histogramTau1PtS_->Fill(tau1Pt, evtWeight);
  histogramTau1PtL_->Fill(tau1Pt, evtWeight);
  histogramTau1Eta_->Fill(tau1Eta, evtWeight);
  histogramTau1Phi_->Fill(tau1Phi, evtWeight);
  histogramTau1DecayMode_->Fill(tau1DecayMode, evtWeight);
  histogramTau1IsoDiscrS_->Fill(tau1IsoDiscr1, evtWeight);
  histogramTau1IsoDiscrL_->Fill(tau1IsoDiscr2, evtWeight);

  histogramTau2PtS_->Fill(tau2Pt, evtWeight);
  histogramTau2PtL_->Fill(tau2Pt, evtWeight);
  histogramTau2Eta_->Fill(tau2Eta, evtWeight);
  histogramTau2Phi_->Fill(tau2Phi, evtWeight);
  histogramTau2DecayMode_->Fill(tau2DecayMode, evtWeight);
  histogramTau2IsoDiscrS_->Fill(tau2IsoDiscr1, evtWeight);
  histogramTau2IsoDiscrL_->Fill(tau2IsoDiscr2, evtWeight);

  histogramVisMassS_->Fill(visMass, evtWeight);
  histogramVisMassL_->Fill(visMass, evtWeight);
  histogramSVfitMassS_->Fill(svFitMass, evtWeight);
  histogramSVfitMassL_->Fill(svFitMass, evtWeight);
  histogramDeltaPhi_->Fill(dPhi, evtWeight);
  histogramDeltaEta_->Fill(dEta, evtWeight);
  histogramDeltaR_->Fill(dR, evtWeight);
  
  histogramJet1PtS_->Fill(jet1Pt, evtWeight);
  histogramJet1PtL_->Fill(jet1Pt, evtWeight);
  histogramJet1Eta_->Fill(jet1Eta, evtWeight);
  histogramJet1Phi_->Fill(jet1Phi, evtWeight);
  histogramJet1BtagDiscr_->Fill(jet1BtagDiscr, evtWeight);
  
  histogramJet2PtS_->Fill(jet2Pt, evtWeight);
  histogramJet2PtL_->Fill(jet2Pt, evtWeight);
  histogramJet2Eta_->Fill(jet2Eta, evtWeight);
  histogramJet2Phi_->Fill(jet2Phi, evtWeight);
  histogramJet2BtagDiscr_->Fill(jet2BtagDiscr, evtWeight);

  histogramBJet1PtS_->Fill(bjet1Pt, evtWeight);
  histogramBJet1PtL_->Fill(bjet1Pt, evtWeight);
  histogramBJet1Eta_->Fill(bjet1Eta, evtWeight);
  histogramBJet1Phi_->Fill(bjet1Phi, evtWeight);
  
  histogramBJet2PtS_->Fill(bjet2Pt, evtWeight);
  histogramBJet2PtL_->Fill(bjet2Pt, evtWeight);
  histogramBJet2Eta_->Fill(bjet2Eta, evtWeight);
  histogramBJet2Phi_->Fill(bjet2Phi, evtWeight);

  histogramMEtS_->Fill(met, evtWeight);
  histogramMEtL_->Fill(met, evtWeight);

  histogramNumVertices_->Fill(numVertices, evtWeight);

  histogramEventCounter_->Fill(0., evtWeight);
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

namespace
{
  TDirectory* createSubdirectory(TDirectory* dir, const std::string& subdirName)
  {
    dir->cd();
    if ( !dir->Get(subdirName.data()) ) {
      dir->mkdir(subdirName.data());
    }
    TDirectory* subdir = dynamic_cast<TDirectory*>(dir->Get(subdirName.data()));
    assert(subdir);
    return subdir;
  }
  
  TDirectory* createSubdirectory_recursively(TFileDirectory& dir, const std::string& fullSubdirName)
  {
    TString fullSubdirName_tstring = fullSubdirName.data();
    TObjArray* subdirNames = fullSubdirName_tstring.Tokenize("/");
    int numSubdirectories = subdirNames->GetEntries();
    TDirectory* parent = dir.getBareDirectory();
    for ( int iSubdirectory = 0; iSubdirectory < numSubdirectories; ++iSubdirectory ) {
      const TObjString* subdirName = dynamic_cast<TObjString*>(subdirNames->At(iSubdirectory));
      assert(subdirName);
      TDirectory* subdir = createSubdirectory(parent, subdirName->GetString().Data());
      parent = subdir;
    }
    return parent;
  }
}

TDirectory* TauTauHistManager::createHistogramSubdirectory(TFileDirectory& dir)
{
  std::string fullSubdirName = std::string(category_);
  if ( bin_ != "" ) fullSubdirName.append("/").append(bin_);
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
