#ifndef LLRAnalysis_TauTauStudies_MuTauStreamAnalyzer_h
#define LLRAnalysis_TauTauStudies_MuTauStreamAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"


#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"


#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"


#include <string>
#include <utility>
#include <map>


class MuTauStreamAnalyzer : public edm::EDAnalyzer{


 public:

  struct more {
    bool operator() (const double& lhs, const double& rhs) const
    {return lhs>rhs;}
  };

  explicit MuTauStreamAnalyzer(const edm::ParameterSet&);
  ~MuTauStreamAnalyzer();

  unsigned int jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_);
  pat::Jet* newJetMatched( const pat::Jet* oldJet , const pat::JetCollection* newJets);

  void computeDCASig(double &iDCA3D    ,double &iDCA3DE    ,double &iDCA2D    ,double &iDCA2DE,
		     double &iDCARPhi3D,double &iDCARPhi3DE,double &iDCARPhi2D,double &iDCARPhi2DE,
		     const reco::Track *iTrack1,const reco::Track *iTrack2);

  void beginJob() ;
  void analyze(const edm::Event&  iEvent, const edm::EventSetup& iSetup);
  void endJob() ;

 private:

  TFile* file_;
  TTree* tree_;

  edm::LumiReWeighting LumiWeights_;

  edm::InputTag diTauTag_;
  edm::InputTag jetsTag_;
  edm::InputTag newJetsTag_;
  edm::InputTag metTag_;
  edm::InputTag rawMetTag_;
  edm::InputTag mvaMetTag_;
  edm::InputTag metCovTag_;
  edm::InputTag muonsTag_;
  edm::InputTag muonsRelTag_;
  edm::InputTag verticesTag_;
  edm::InputTag triggerResultsTag_;
  edm::InputTag genParticlesTag_;
  edm::InputTag genTausTag_;
  const  TransientTrackBuilder *transientTrackBuilder_;


  bool isMC_;
  bool verbose_;
  float minCorrPt_;
  float minJetID_;
  float deltaRLegJet_;
  bool doIsoMVAOrdering_;

  std::vector< double >* jetsBtagHE_;
  std::vector< double >* jetsBtagHP_;
  std::vector< double >* jetsBtagCSV_;
  std::vector< double >* bQuark_;
  std::vector< float >* jetsChNfraction_;
  std::vector< float >* jetsChEfraction_;
  std::vector< float >* jetMoments_;
  std::vector< float >* jetPUMVA_;
  std::vector< float >* jetPUWP_;
  std::vector< float >* jetQuarkGluon_;   // Quark/gluon id
  std::vector< float >* jetQuarkGluonGen_;   // Quark/gluon id
  std::vector< float >* metSgnMatrix_;

  std::vector< float >* gammadEta_;
  std::vector< float >* gammadPhi_;
  std::vector< float >* gammadR_;
  std::vector< float >* gammaPt_;

  std::vector< int >* tauXTriggers_;
  std::vector< int >* triggerBits_;

  std::vector< double >* sigDCA_;

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
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* diTauLegsAltP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* genDiTauLegsP4_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* genTausP4_; 

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* METP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genMETP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genVP4_;
  int genDecay_;
  int hepNUP_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* leptonJets_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* extraMuons_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoMuonsP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoElectronsP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoTausP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* pfMuons_; 
  
  unsigned long run_,event_,lumi_;
  int index_;
  int validityCA_,validityICA_;
  float x1CollApprox_,x2CollApprox_;
  float sumEt_;
  float chIsoLeg1v1_,nhIsoLeg1v1_,phIsoLeg1v1_,elecIsoLeg1v1_,muIsoLeg1v1_;
  float chIsoPULeg1v1_,nhIsoPULeg1v1_,phIsoPULeg1v1_;
  float chIsoLeg1v2_,nhIsoLeg1v2_,phIsoLeg1v2_,elecIsoLeg1v2_,muIsoLeg1v2_;
  float chIsoPULeg1v2_,nhIsoPULeg1v2_,phIsoPULeg1v2_;
  float chIsoLeg2_,nhIsoLeg2_,phIsoLeg2_;
  float dxy1_,dxy2_;
  float dz1_,dz2_;
  float dxyE1_,dxyE2_;
  float dzE1_,dzE2_;
  float pfJetPt_;
  float pfJetEtaMom2_; 
  float pfJetPhiMom2_; 
  float MtLeg1_;
  float pZeta_;
  float pZetaVis_;
  float pZetaSig_;
  float mTauTauMin_;
  float numPV_;
  int numOfDiTaus_;
  int numOfLooseIsoDiTaus_;
  int decayMode_;
  int genDecayMode_;
  int genPolarization_;
  int parton_;
  int genPartMult_;
  int leadGenPartPdg_;
  float leadGenPartPt_;

  float diTauNSVfitMass_;
  float diTauNSVfitMassErrUp_;
  float diTauNSVfitMassErrDown_;
  float diTauSVfitMassErrUp_;
  float diTauSVfitMassErrDown_;
  float visibleTauMass_;
  float visibleGenTauMass_;

  float leadPFChargedHadrMva_;
  float leadPFChargedHadrHcalEnergy_;
  float leadPFChargedHadrEcalEnergy_;
  float leadPFChargedHadrTrackPt_;
  float leadPFChargedHadrTrackP_;
  float leadPFChargedHadrPt_;
  float leadPFChargedHadrP_;
  float leadPFCandMva_;
  float leadPFCandHcalEnergy_;
  float leadPFCandEcalEnergy_;
  float leadPFCandPt_;
  float leadPFCandP_;
  int signalPFChargedHadrCands_;
  int signalPFGammaCands_;
  float emFraction_;
  float hasGsf_;

  int tightestHPSWP_;
  int tightestHPSDBWP_;
  int tightestHPSDB3HWP_;
  int tightestHPSMVAWP_;
  int tightestHPSMVA2WP_;
  int tightestAntiECutWP_; 
  int tightestAntiEMVA3WP_; 
  float AntiEMVA3raw_; 
  int AntiEMVA3category_; 
  int tightestAntiMuWP_; 
  int tightestAntiMu2WP_;
  float hpsMVA_;
  float hpsMVA2_;
  float hpsDB3H_;
  int isTauLegMatched_;
  int isMuLegMatched_;
  int muFlag_;
  int vetoEvent_;
  int isPFMuon_;
  int isTightMuon_;
  float muVetoRelIso_;
  int hasKft_;

  float diTauCharge_;
  float chargeL1_;
  float rhoFastJet_;
  float rhoNeutralFastJet_;
  float embeddingWeight_;
  float nPUVertices_;
  float nPUVerticesM1_;
  float nPUVerticesP1_;
  float nPUtruth_;

  float mcPUweight_;

};


#endif
