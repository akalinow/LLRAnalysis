#ifndef LLRAnalysis_TauTauStudies_ObjectRescalerProducer_h
#define LLRAnalysis_TauTauStudies_ObjectRescalerProducer_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"

using namespace reco;

template<typename T> 
class ObjectRescalerProducer : public edm::EDProducer{


public:

  explicit ObjectRescalerProducer(const edm::ParameterSet& cfg){

    inputCollection_ = cfg.getParameter<edm::InputTag>("inputCollection");
    shift_ = cfg.getParameter<double>("shift");

    produces<std::vector<T> >("U");
    produces<std::vector<T> >("D");

  }

  ~ObjectRescalerProducer(){}

  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {


    typedef edm::Ptr<T> TPtr;
    typedef edm::View<T> TView;
    typedef std::vector<T> TCollection;

    std::auto_ptr<TCollection> ObjectRescaledU( new TCollection() );
    std::auto_ptr<TCollection> ObjectRescaledD( new TCollection() );

    edm::Handle<TView> collectionHandle;
    iEvent.getByLabel(inputCollection_,collectionHandle);

    for ( unsigned int i = 0; i < collectionHandle->size() ; i++ ) {
    
      TPtr ptr = collectionHandle->ptrAt(i);

      T objectU( *ptr );
      T objectD( *ptr );

      double scaleU = sqrt( ptr->energy()*(1+shift_)*ptr->energy()*(1+shift_) - ptr->mass()*ptr->mass() )/ptr->p();
      math::XYZTLorentzVectorD p4Up( ptr->px()*scaleU , ptr->py()*scaleU, ptr->pz()*scaleU, ptr->energy()*(1+shift_) );
      objectU.setP4( p4Up );

      double scaleD = sqrt( ptr->energy()*(1-shift_)*ptr->energy()*(1-shift_) - ptr->mass()*ptr->mass() )/ptr->p();
      math::XYZTLorentzVectorD p4Down( ptr->px()*scaleD , ptr->py()*scaleD, ptr->pz()*scaleD, ptr->energy()*(1-shift_) );
      objectD.setP4( p4Down );
 
      ObjectRescaledU->push_back( objectU );
      ObjectRescaledD->push_back( objectD );

    }

    iEvent.put(ObjectRescaledU,"U");
    iEvent.put(ObjectRescaledD,"D");

  }


 private:

  edm::InputTag inputCollection_;
  double shift_;
};


#endif
