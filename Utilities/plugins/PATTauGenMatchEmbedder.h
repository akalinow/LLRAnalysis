#ifndef LLRAnalysis_Utilities_PATTauGenMatchEmbedder_h
#define LLRAnalysis_Utilities_PATTauGenMatchEmbedder_h

/** \class PATTauGenMatchEmbedder
 *
 * Embed matches to GenParticles and GenJets into pat::Tau objects
 * (hot-fix that needs to be applied to Embedded sample, due to bug in PAT-tuple production)
 *
 * \author Christian Veelken, LLR
 *
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Tau.h"

#include <string>

class PATTauGenMatchEmbedder : public edm::EDProducer 
{
 public:
  /// constructor
  explicit PATTauGenMatchEmbedder(const edm::ParameterSet&);
  
  /// destructor
  ~PATTauGenMatchEmbedder();
   
 private:
  /// produce collection of pat::Jets.
  /// The output jet collection equals the input jet collection,
  /// except that an additional userFloat('bJetEnReg') is added.
  void produce(edm::Event&, const edm::EventSetup&);

  std::string moduleLabel_;

  /// input tau collection
  edm::InputTag src_;

  /// collections of GenParticle and GenJet matches
  edm::InputTag srcGenParticleMatch_;
  edm::InputTag srcGenJetMatch_;

  int verbosity_;
};

#endif
