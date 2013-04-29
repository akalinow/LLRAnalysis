#ifndef LLRAnalysis_TauTauStudies_L1ExtraMEtToMEtConverterForTauTauStudiesT_h
#define LLRAnalysis_TauTauStudies_L1ExtraMEtToMEtConverterForTauTauStudiesT_h

/** \class L1ExtraMEtToMEtConverterForTauTauStudies
 *
 * Convert L1Extra MET objects to reco::MET/pat::MET data-format,
 * in order to make response and resolution plots
 * 
 * \author Christian Veelken, LLR
 *
 * \version $Revision: 1.8 $
 *
 * $Id: L1ExtraMEtToMEtConverterForTauTauStudiesT.h,v 1.8 2012/02/13 17:33:04 veelken Exp $
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/METReco/interface/MET.h"

template <typename T>
class L1ExtraMEtToMEtConverterForTauTauStudiesT : public edm::EDProducer 
{
  typedef std::vector<T> MEtCollection;

 public:
  explicit L1ExtraMEtToMEtConverterForTauTauStudiesT(const edm::ParameterSet& cfg)
  {
    src_ = cfg.getParameter<edm::InputTag>("src");

    produces<MEtCollection>(); 
  }
  ~L1ExtraMEtToMEtConverterForTauTauStudiesT() {}

 private:
  void produce(edm::Event& evt, const edm::EventSetup& es)
  {
    edm::Handle<l1extra::L1EtMissParticleCollection> met_input;
    evt.getByLabel(src_, met_input);
  
    std::auto_ptr<MEtCollection> met_output(new MEtCollection());

    for ( l1extra::L1EtMissParticleCollection::const_iterator met_input_i = met_input->begin();
	  met_input_i != met_input->end(); ++met_input_i ) {
      T met_output_i = createMEt(met_input_i->etTotal(), met_input_i->p4(), met_input_i->vertex());
      met_output->push_back(met_output_i);
    }
    
    evt.put(met_output);
  }

  T createMEt(double sumEt, const reco::Candidate::LorentzVector& p4, const reco::Candidate::Point& vertex)
  {    
    assert(0);
  }

  edm::InputTag src_;
};

#endif
