#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <cmath>


#include <TH1.h>

#include <HepMC/GenEvent.h>
#include <HepMC/SimpleVector.h>

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/Flow.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "GeneratorInterface/LHEInterface/interface/LHEEvent.h"

using namespace lhef;

class LHEAnalyzer : public edm::EDAnalyzer {
public:
  explicit LHEAnalyzer(const edm::ParameterSet &params);
  virtual ~LHEAnalyzer();
  
protected:
  virtual void analyze(const edm::Event &event,
		       const edm::EventSetup &es);
  
  
private:

  edm::InputTag	sourceLabel;
  int nBins_;
  TH1 *h_Pt_b;


};

LHEAnalyzer::LHEAnalyzer(const edm::ParameterSet &params) :
	sourceLabel(params.getParameter<edm::InputTag>("src")),
	nBins_(params.getParameter<unsigned int>("nBins"))
{
  
  edm::Service<TFileService> fs;
  h_Pt_b   = fs->make<TH1F>("h_Pt_b","histoPt_b",nBins_, 0, 100.);

}

LHEAnalyzer::~LHEAnalyzer(){}


void LHEAnalyzer::analyze(const edm::Event &event, const edm::EventSetup &es)
{
  
  edm::Handle<edm::HepMCProduct> hepmc;
  event.getByLabel(sourceLabel, hepmc);
  
  std::auto_ptr<HepMC::GenEvent> clonedEvent;
  const HepMC::GenEvent *genEvent = hepmc->GetEvent();
  if (!genEvent->signal_process_vertex()) {
    clonedEvent.reset(new HepMC::GenEvent(*genEvent));
    const HepMC::GenVertex *signalVertex =
      LHEEvent::findSignalVertex(clonedEvent.get());
    clonedEvent->set_signal_process_vertex(const_cast<HepMC::GenVertex*>(signalVertex));
    genEvent = clonedEvent.get();
  }
  
  int code_b = 0;
  
  for ( HepMC::GenEvent::particle_const_iterator p = genEvent->particles_begin(); p != genEvent->particles_end(); ++p ) {
    
    //(*p)->print();
    
    // leptons from direct b decay
    if ( abs((*p)->pdg_id()==5) && (*p)->status()>-99 ) {
      h_Pt_b->Fill((*p)->momentum().perp());
      code_b=(*p)->barcode();
      std::cout << "Found b= " << (*p)->barcode() << " " << (*p)->production_vertex()->barcode() <<std::endl;	    
    }

  }
  
  
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(LHEAnalyzer);
