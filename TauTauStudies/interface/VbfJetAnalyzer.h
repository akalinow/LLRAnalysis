#ifndef LLRAnalysis_TauTauStudies_VbfJetAnalyzer_h
#define LLRAnalysis_TauTauStudies_VbfJetAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "DataFormats/JetReco/interface/PFJet.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include <string>


class VbfJetAnalyzer : public edm::EDAnalyzer{


 public:

  struct more {
    bool operator() (const double& lhs, const double& rhs) const
    {return lhs>rhs;}
  };

  explicit VbfJetAnalyzer(const edm::ParameterSet&);
  ~VbfJetAnalyzer();

  unsigned int jetID( const reco::PFJet* jet, const double scale);

  void beginJob() ;
  void analyze(const edm::Event&  iEvent, const edm::EventSetup& iSetup);
  void endJob() ;

 private:

  TTree* tree_;
   
  bool verbose_; 
  float run_,event_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* tagjetsP4_; 
};


#endif
