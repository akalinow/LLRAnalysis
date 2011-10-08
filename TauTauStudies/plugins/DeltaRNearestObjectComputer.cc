#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

template<typename T>
class DeltaRNearestObjectComputer : public edm::EDProducer {
    public:
        explicit DeltaRNearestObjectComputer(const edm::ParameterSet & iConfig);
        virtual ~DeltaRNearestObjectComputer() ;

        virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

    private:
        edm::InputTag probes_;            
        edm::InputTag objects_; 
        StringCutObjectSelector<T,true> objCut_; // lazy parsing, to allow cutting on variables not in reco::Candidate class
};

template<typename T>
DeltaRNearestObjectComputer<T>::DeltaRNearestObjectComputer(const edm::ParameterSet & iConfig) :
    probes_(iConfig.getParameter<edm::InputTag>("probes")),
    objects_(iConfig.getParameter<edm::InputTag>("objects")),
    objCut_(iConfig.existsAs<std::string>("objectSelection") ? iConfig.getParameter<std::string>("objectSelection") : "", true)
{
    produces<edm::ValueMap<float> >();
}


template<typename T>
DeltaRNearestObjectComputer<T>::~DeltaRNearestObjectComputer()
{
}

template<typename T>
void 
DeltaRNearestObjectComputer<T>::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
    using namespace edm;

    // read input
    Handle<View<reco::Candidate> > probes;
    iEvent.getByLabel(probes_,  probes);

    Handle<View<T> > objects;
    iEvent.getByLabel(objects_, objects);

    // prepare vector for output    
    std::vector<float> values;
    
    // fill
    View<reco::Candidate>::const_iterator probe, endprobes = probes->end();
    for (probe = probes->begin(); probe != endprobes; ++probe) {
        double dr2min = 10000;
        for (unsigned int iObj=0; iObj<objects->size(); iObj++) {
	  const T& obj = objects->at(iObj);
	  if (!objCut_(obj)) continue;
            double dr2 = deltaR2(*probe, obj);
            if (dr2 < dr2min) { dr2min = dr2; }
        }
        values.push_back(sqrt(dr2min));
    }

    // convert into ValueMap and store
    std::auto_ptr<ValueMap<float> > valMap(new ValueMap<float>());
    ValueMap<float>::Filler filler(*valMap);
    filler.insert(probes, values.begin(), values.end());
    filler.fill();
    iEvent.put(valMap);
}


////////////////////////////////////////////////////////////////////////////////
// plugin definition
////////////////////////////////////////////////////////////////////////////////

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

typedef DeltaRNearestObjectComputer<reco::Candidate>    DeltaRNearestCandidateComputer;
typedef DeltaRNearestObjectComputer<pat::Muon>          DeltaRNearestMuonComputer;
typedef DeltaRNearestObjectComputer<pat::Electron>      DeltaRNearestElectronComputer;
typedef DeltaRNearestObjectComputer<pat::Jet>           DeltaRNearestJetComputer;
typedef DeltaRNearestObjectComputer<pat::Tau>           DeltaRNearestTauComputer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(DeltaRNearestCandidateComputer);          
DEFINE_FWK_MODULE(DeltaRNearestMuonComputer);          
DEFINE_FWK_MODULE(DeltaRNearestElectronComputer);          
DEFINE_FWK_MODULE(DeltaRNearestJetComputer);          
DEFINE_FWK_MODULE(DeltaRNearestTauComputer);          
