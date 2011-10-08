#ifndef LLRAnalysis_TauTauStudies_DiTauMassProducer_h
#define LLRAnalysis_TauTauStudies_DiTauMassProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TauAnalysis/CandidateTools/interface/CompositePtrCandidateT1T2MEtProducer.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEtFwd.h"


template<typename T1, typename T2> 
class DiTauMassProducer : public edm::EDProducer{


public:

  explicit DiTauMassProducer(const edm::ParameterSet& cfg){

    diTauTag_ = cfg.getParameter<edm::InputTag>("diTauTag");

    produces<std::vector<double> >("");

  }

  ~DiTauMassProducer(){}

  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {


    std::auto_ptr<std::vector<double> > diTauMass( new std::vector<double>() );

    edm::Handle<edm::View<CompositePtrCandidateT1T2MEt<T1,T2> > > collectionHandle;
    iEvent.getByLabel(diTauTag_,collectionHandle);

    for ( unsigned int i = 0; i < collectionHandle->size() ; i++ ) {
    
      edm::Ptr<CompositePtrCandidateT1T2MEt<T1,T2> > 
	theDiTau = collectionHandle->ptrAt(i);

      double diTauNSVfitMass = (theDiTau->hasNSVFitSolutions() && 
				theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) ? 
	theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->mass() : -99; 

      diTauMass->push_back( diTauNSVfitMass );

    }

    iEvent.put(diTauMass,"");

  }


 private:

  edm::InputTag diTauTag_;

};


#endif
