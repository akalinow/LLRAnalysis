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
    shift_           = cfg.getParameter<std::vector<double> >("shift");
    numOfSigmas_     = cfg.getParameter<double>("numOfSigmas");
    verbose_         = cfg.existsAs<bool>("verbose") ? cfg.getParameter<bool>("verbose") : false;

    produces<std::vector<T> >("U");
    produces<std::vector<T> >("D");

  }

  ~ObjectRescalerProducer(){}
  math::XYZTLorentzVectorD CompScale(const T& object, double shift){
    double scale = sqrt( object.energy()*(1+shift)*object.energy()*(1+shift) - object.mass()*object.mass() )/object.p();
    math::XYZTLorentzVectorD p4( object.px()*scale , object.py()*scale, object.pz()*scale, object.energy()*(1+shift) );
    
    return p4;
  }

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

      float shift = fabs(ptr->eta())<1.44 ? shift_[0] : shift_[1];
      shift *= numOfSigmas_;

      math::XYZTLorentzVectorD p4Up = CompScale(*ptr, shift);
      objectU.setP4( p4Up );

      math::XYZTLorentzVectorD p4Down = CompScale(*ptr, -shift);
      objectD.setP4( p4Down );
 
      if(verbose_) 
	std::cout << iEvent.id() 
		  << " : visited object with pt = " << ptr->pt() << " , up = " << objectU.pt() << ", down = " << objectD.pt() 
		  << std::endl;

      ObjectRescaledU->push_back( objectU );
      ObjectRescaledD->push_back( objectD );

    }

    iEvent.put(ObjectRescaledU,"U");
    iEvent.put(ObjectRescaledD,"D");

  }


 private:

  edm::InputTag inputCollection_;
  std::vector<double> shift_;
  float numOfSigmas_;
  bool verbose_;
};


#endif
