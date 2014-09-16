#ifndef LLRAnalysis_HHKinFit_DiJetKinFitMaster_H
#define LLRAnalysis_HHKinFit_DiJetKinFitMaster_H

#include <Rtypes.h>
#include <stdio.h>
#include <TMatrixD.h>
#include <map>
#include <utility>
#include <vector>

#include "TLorentzVector.h"

class DiJetKinFitMaster
{
public:
  DiJetKinFitMaster( TLorentzVector* bjet1, TLorentzVector* bjet2 );

  void doFullFit();
  
  //Getters for fit results
  Double_t getBestChi2FullFit();
  std::pair< Int_t, Int_t > getBestHypoFullFit();
  std::map< std::pair< Int_t, Int_t >, Double_t > getChi2FullFit();
  std::map< std::pair< Int_t, Int_t >, Double_t > getFitProbFullFit();
  std::map< std::pair< Int_t, Int_t >, Double_t > getPullB1FullFit();
  std::map< std::pair< Int_t, Int_t >, Double_t > getPullB2FullFit();
  std::map< std::pair< Int_t, Int_t >, Int_t > getConvergenceFullFit();

  TLorentzVector getFitJet1() { return m_bjet1_fitted; }
  TLorentzVector getFitJet2() { return m_bjet2_fitted; }

  //Hypotheses
  void addMhHypothesis(std::vector<Int_t> v);
  void addMhHypothesis(Double_t m1, Double_t m2=0, Double_t m3=0, Double_t m4=0, Double_t m5=0, Double_t m6=0, Double_t m7=0, Double_t m8=0, Double_t m9=0, Double_t m10=0);

  //Resolution  
  Double_t GetBjetResolution(Double_t eta, Double_t et);

private:
  //hypotheses
  std::vector< Int_t > m_mh;

  //input vectors
  TLorentzVector* m_bjet1;
  TLorentzVector m_bjet1_fitted;
  TLorentzVector* m_bjet2;
  TLorentzVector m_bjet2_fitted;

  //full event fit
  std::map< std::pair< Int_t, Int_t >, Double_t > m_fullFitResultChi2;
  std::map< std::pair< Int_t, Int_t >, Double_t > m_fullFitResultFitProb;
  std::map< std::pair< Int_t, Int_t >, Double_t > m_fullFitPullB1;
  std::map< std::pair< Int_t, Int_t >, Double_t > m_fullFitPullB2;
  std::map< std::pair< Int_t, Int_t >, Int_t > m_fullFitConvergence;

  Double_t m_bestChi2FullFit;
  std::pair< Int_t, Int_t > m_bestHypoFullFit;
};

#endif
