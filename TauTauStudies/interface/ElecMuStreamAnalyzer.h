#ifndef LLRAnalysis_TauTauStudies_ElecMuStreamAnalyzer_h
#define LLRAnalysis_TauTauStudies_ElecMuStreamAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include <TRandom3.h>

#include "LLRAnalysis/Utilities/interface/PUWeight.h"


#include <string>
#include <utility>
#include <map>


class ElecMuStreamAnalyzer : public edm::EDAnalyzer{


 public:

  struct more {
    bool operator() (const double& lhs, const double& rhs) const
    {return lhs>rhs;}
  };

  explicit ElecMuStreamAnalyzer(const edm::ParameterSet&);
  ~ElecMuStreamAnalyzer();

  unsigned int jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_);
  pat::Jet* newJetMatched( const pat::Jet* oldJet , const pat::JetCollection* newJets);

  void beginJob() ;
  void analyze(const edm::Event&  iEvent, const edm::EventSetup& iSetup);
  void endJob() ;

 private:

  TFile* file_;
  TTree* tree_;

  TRandom3* tRandom_;
 
  edm::InputTag diTauTag_;
  edm::InputTag jetsTag_;
  edm::InputTag newJetsTag_;
  edm::InputTag metTag_;
  edm::InputTag rawMetTag_;
  edm::InputTag muonsTag_;
  edm::InputTag muonsRelTag_;
  edm::InputTag electronsTag_;
  edm::InputTag electronsRelTag_;
  edm::InputTag verticesTag_;
  edm::InputTag triggerResultsTag_;

  bool isMC_;
  bool verbose_;
  float minCorrPt_;
  float minJetID_;
  float deltaRLegJet_;

  std::vector< double >* jetsBtagHE_;
  std::vector< double >* jetsBtagHP_;
  std::vector< float >* jetsChNfraction_;
  std::vector< float >* jetsChEfraction_;
  std::vector< float >* jetMoments_;

  std::vector< int >* tauXTriggers_;
  std::vector< int >* triggerBits_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsIDP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsIDUpP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsIDDownP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* jetsIDL1OffsetP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* genJetsIDP4_; 
 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauVisP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauCAP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauICAP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauSVfitP4_; 

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* genDiTauLegsP4_; 

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* METP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genMETP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genVP4_;
  int genDecay_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* extraMuons_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* extraElectrons_; 
  
  unsigned long run_,event_,lumi_;
  float sumEt_;
  float chIsoLeg1v1_,nhIsoLeg1v1_,phIsoLeg1v1_;
  float chIsoPULeg1v1_,nhIsoPULeg1v1_,phIsoPULeg1v1_;
  float chIsoLeg1v2_,nhIsoLeg1v2_,phIsoLeg1v2_;
  float chIsoPULeg1v2_,nhIsoPULeg1v2_,phIsoPULeg1v2_;
  float chIsoLeg2v1_,nhIsoLeg2v1_,phIsoLeg2v1_;
  float chIsoPULeg2v1_,nhIsoPULeg2v1_,phIsoPULeg2v1_;
  float chIsoLeg2v2_,nhIsoLeg2v2_,phIsoLeg2v2_;
  float chIsoPULeg2v2_,nhIsoPULeg2v2_,phIsoPULeg2v2_;

  float dxy1_,dxy2_;
  float dz1_,dz2_;
  float MtLeg1_;
  float MtLeg2_;
  float pZeta_;
  float pZetaVis_;
  float pZetaSig_;
  float mTauTauMin_;
  float numPV_;
  int numOfDiTaus_;
  int numOfLooseIsoDiTaus_;
  float diTauNSVfitMass_;
  float diTauNSVfitMassErrUp_;
  float diTauNSVfitMassErrDown_;

  int isElecLegMatched_;
  int isMuLegMatched_;

  int muFlag_;
  int elecFlag_;

  float diTauCharge_;
  float rhoFastJet_;
  float rhoNeutralFastJet_;
  int nPUVertices_;
  int nOOTPUVertices_;
  std::vector<double> weights2011_;

  PUWeight* fpuweight_;
  float mcPUweight_;
};


#endif
