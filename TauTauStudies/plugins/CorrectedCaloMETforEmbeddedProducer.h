#ifndef LLRAnalysis_TauTauStudies_CorrectedCaloMETforEmbeddedProducer_h
#define LLRAnalysis_TauTauStudies_CorrectedCaloMETforEmbeddedProducer_h

/** \class CorrectedCaloMETforEmbeddedProducer
 *
 * Correct CaloMET for Data/MC difference in response for CaloJet associated to hadronic tau decay
 *
 * NOTE: This module is to be used for Embedded samples only!
 *
 * \authors Christian Veelken, LLR
 *
 * \version $Revision: 1.5 $
 *
 * $Id: CorrectedCaloMETforEmbeddedProducerT.h,v 1.5 2013/02/22 15:33:15 veelken Exp $
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"

#include <string>
#include <vector>

class CorrectedCaloMETforEmbeddedProducer : public edm::EDProducer  
{
 public:

  explicit CorrectedCaloMETforEmbeddedProducer(const edm::ParameterSet&);
  ~CorrectedCaloMETforEmbeddedProducer();
    
 private:

  void produce(edm::Event&, const edm::EventSetup&);

  std::string moduleLabel_;

  edm::InputTag srcMEt_; // input collection
  typedef std::vector<edm::InputTag> vInputTag;
  vInputTag srcTauJets_; 

  std::string residualCorrLabel_;
  double residualCorrEtaMax_;
  double extraCorrFactor_;
  FactorizedJetCorrector* residualCorrectorFromFile_;
  bool isMC_;

  int verbosity_;
};

#endif

 

