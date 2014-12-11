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

  typedef std::vector<unsigned int> vuint;

  explicit SingleCandidatePicker(const edm::ParameterSet& cfg){

    src_           = cfg.getParameter<edm::InputTag>("src");
    itemNumber_    = cfg.getParameter<vuint>("itemNumber");
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
    
    for ( vuint::const_iterator itemNumber_it = itemNumber_.begin();
	  itemNumber_it != itemNumber_.end(); ++itemNumber_it ) {
      
      if( (*itemNumber_it) < collection->size() ){
	T item = collection->at(*itemNumber_it);
	newCollection->push_back(item);
	if ( verbose_ ) {
	  std::cout << "[SingleCandidatePicker:produce]: Item no. " << (*itemNumber_it) << " picked:"
		    << " Pt = " << item.pt() << ", eta = " << item.eta() << ", phi = " << item.phi() << std::endl;
	} else if ( verbose_ ) {
	  std::cout << "[SingleCandidatePicker:produce]: Item no. "<< (*itemNumber_it) <<" does not exist, empty collection produced"
		    << std::endl;
	}
      }
    }
  
    evt.put(newCollection);
  }

  edm::InputTag src_;
  vuint itemNumber_;
  bool verbose_;

};

#endif

