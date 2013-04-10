#ifndef LLRAnalysis_Utilities_SingleCandidatePicker_h
#define LLRAnalysis_Utilities_SingleCandidatePicker_h

/** \class SingleCandidatePicker
 *
 * \authors Michal Bluj,
 *
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"

#include <vector>

template<typename T>
class SingleCandidatePicker : public edm::EDProducer {

 public:

  explicit SingleCandidatePicker(const edm::ParameterSet& cfg){

    src_           = cfg.getParameter<edm::InputTag>("src");
    itemNumber_    = cfg.getParameter<unsigned int>("itemNumber");
    verbose_       = cfg.getUntrackedParameter<bool>("verbose",false);

    produces<std::vector<T> >("");
  }
  ~SingleCandidatePicker(){}
  
 private:

  void produce(edm::Event& evt, const edm::EventSetup& es) 
  {
    edm::Handle<std::vector<T> > collection;
    evt.getByLabel(src_, collection);
    
    std::auto_ptr<std::vector<T> > newCollection(new std::vector<T>() );

    if(itemNumber_ < collection->size() ){
      T item = collection->at(itemNumber_);
      newCollection->push_back(item);
      if(verbose_)
	std::cout<<"[SingleCandidatePicker:produce]: "
		 <<"Item no. "<<itemNumber_<<" picked:"
		 <<" Pt="<<item.pt()
		 <<", eta="<<item.eta()
		 <<", phi="<<item.phi()
		 <<std::endl;
    }
    else if(verbose_)
      std::cout<<"[SingleCandidatePicker:produce]: "
	       <<"Item no. "<<itemNumber_<<" does not exist, empty collection produced"
	       <<std::endl;
  
    evt.put(newCollection);
  }

  edm::InputTag src_;
  unsigned int itemNumber_;
  bool verbose_;

};

#endif

