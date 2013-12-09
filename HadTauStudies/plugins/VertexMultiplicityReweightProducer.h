#ifndef LLRAnalysis_HadTauStudies_VertexMultiplicityReweightProducer_h
#define LLRAnalysis_HadTauStudies_VertexMultiplicityReweightProducer_h

/** \class VertexMultiplicityReweightProducer
 *
 * Reweight Monte Carlo events simulated with pile-up to match the
 * vertex multiplicity distribution observed in the analyzed data sample
 *
 * NOTE:
 *      o weight > 1: fraction of events which given vertex multiplicity higher in data than in (pile-up) MC
 *      o weight = 1: fraction of events which given vertex multiplicity same   in data than in (pile-up) MC
 *      o weight < 1: fraction of events which given vertex multiplicity lower  in data than in (pile-up) MC
 *
 * \authors Christian Veelken
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "LLRAnalysis/HadTauStudies/interface/ObjValExtractorBase.h"

class VertexMultiplicityReweightProducer : public edm::EDProducer 
{
 public:
  explicit VertexMultiplicityReweightProducer(const edm::ParameterSet&);
  ~VertexMultiplicityReweightProducer();

  void produce(edm::Event&, const edm::EventSetup&);

 private:
  ObjValExtractorBase* extractor_;
};

#endif

