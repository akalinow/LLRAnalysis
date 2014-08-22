#ifndef LLRAnalysis_TauTauStudies_ElecTauStreamAnalyzer_h
#define LLRAnalysis_TauTauStudies_ElecTauStreamAnalyzer_h

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

//#include "EGamma/EGammaAnalysisTools/interface/EGammaMvaEleEstimator.h"
#include "EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimator.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

#include <string>


class ElecTauStreamAnalyzer : public edm::EDAnalyzer{


 public:

  struct more {
    bool operator() (const double& lhs, const double& rhs) const
    {return lhs>rhs;}
  };

  explicit ElecTauStreamAnalyzer(const edm::ParameterSet&);
  ~ElecTauStreamAnalyzer();

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
  edm::InputTag electronsTag_;
  edm::InputTag electronsRelTag_;
  edm::InputTag electronsAntiZeeTag_;
  edm::InputTag verticesTag_;
  edm::InputTag triggerResultsTag_;
  edm::InputTag genParticlesTag_;
  edm::InputTag genTausTag_;
  const  TransientTrackBuilder *transientTrackBuilder_;

  std::string lastInputFileHiggsPtWeight_;
  TFile* inputFileHiggsPtWeight_;
  TH1* lutHiggsPtWeightNom_;
  TH1* lutHiggsPtWeightUp_;
  TH1* lutHiggsPtWeightDown_;

  struct InputTagEntryType
  {
    InputTagEntryType(const std::string& branchName, const edm::InputTag& src)
      : branchName_(branchName),
	src_(src)
    {}
    ~InputTagEntryType() {}
    std::string branchName_;
    edm::InputTag src_;
  };
/*   std::vector<InputTagEntryType> evtWeightsToStore_; */

  bool isMC_, isRhEmb_;
  bool verbose_;
  float minCorrPt_;
  float minJetID_;
  float deltaRLegJet_;
  bool doIsoOrdering_;

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

  std::map< std::string,int >* triggerPaths_;
  std::map< std::string,int >* HLTfiltersElec_;  
  std::map< std::string,int >* HLTfiltersTau_;  
  std::map< std::string,int >* HLTfiltersTauJet_;  
/*   std::vector< int >* tauXTriggers_; */
/*   std::vector< int >* triggerBits_; */

  std::vector< double >* sigDCA_;

  std::vector< float >* gammadEta_;
  std::vector< float >* gammadPhi_;
  std::vector< float >* gammadR_;
  std::vector< float >* gammaPt_;

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

  std::vector< int >* genTausCharge_;
  std::vector< int >* genTausDecayLeptonically_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* METP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* caloMETNoHFP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* l1ETMP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genMETP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genVP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* genEleFromVP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >  >* gentopP4_;
  int NumEleFromV_;
  int genDecay_;
  int hepNUP_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* leptonJets_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* extraElectrons_; 
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoElectronsP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoMuonsP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* vetoTausP4_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* pfElectrons_; 
  std::vector<int>* vetoElectronsID_;

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* trgTaus_;
  std::vector<int>* trgTauId_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* l1IsoElectrons_;
  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >* l1NoIsoElectrons_;
  
  unsigned long run_,event_,lumi_;
  int index_;
  float sumEt_, caloNoHFsumEt_, caloNoHFsumEtCorr_;
  float chIsoLeg1v1_,nhIsoLeg1v1_,phIsoLeg1v1_,elecIsoLeg1v1_,muIsoLeg1v1_;
  float chIsoPULeg1v1_,nhIsoPULeg1v1_,phIsoPULeg1v1_;
  float chIsoLeg1v2_,nhIsoLeg1v2_,phIsoLeg1v2_;
  float chIsoPULeg1v2_,nhIsoPULeg1v2_,phIsoPULeg1v2_,elecIsoLeg1v2_,muIsoLeg1v2_;
  float chIsoEELeg1v2_,nhIsoEELeg1v2_,phIsoEELeg1v2_;
  float chIsoEEPULeg1v2_,nhIsoEEPULeg1v2_,phIsoEEPULeg1v2_,elecIsoEELeg1v2_,muIsoEELeg1v2_;
  float chIsoLeg2_,nhIsoLeg2_,phIsoLeg2_;
  float dxy1_,dxy2_;
  float dz1_,dz2_;
  float dxyE1_,dxyE2_;
  float dzE1_,dzE2_;
  float scEta1_;
  float pfJetPt_;
  float pfJetEtaMom2_;
  float pfJetPhiMom2_;
  float MtLeg1_;
  float pZeta_;
  float pZetaVis_;
  float pZetaSig_;
  float mTauTauMin_;
  float numPV_;
  float VtxZ_;
  float VtxX_;
  float VtxY_;
  int numOfDiTaus_;
  int numOfLooseIsoDiTaus_;
  int decayMode_;
  int genDecayMode_;
  int genPolarization_;
  int parton_;
  int genPartMult_;
  int leadGenPartPdg_;
  float leadGenPartPt_;

  int diTauNSVfitIsValid_; 
  float diTauNSVfitMass_;
  float diTauNSVfitMassErrUp_;
  float diTauNSVfitMassErrDown_;
  float diTauSVfitMassErrUp_;
  float diTauSVfitMassErrDown_;
  float diTauNSVfitPt_;
  float diTauNSVfitPtErrUp_;
  float diTauNSVfitPtErrDown_;
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

  //EleID
  int tightestCutBasedWP_;
  int tightestCiCWP_;
  int tightestMVAWP_;
  float mvaPOGTrig_;
  float mvaPOGTrigNoIP_;
  float mvaPOGNonTrig_;
  int tightestMVAPOGNonTrigWP_;
  int tightestMVAPOGTrigNoIPWP_;
  float mitMVA_;
  int antiConv_;
  int isTriggerElectron_;

  //TauID
  int decayModeFinding_;
  int decayModeFindingNewDM_;
  int decayModeFindingOldDM_;
  int AntiEDeadEcal_;
  int tightestAntiECutWP_;
  int tightestAntiEMVA5WP_;
  float AntiEMVA5raw_;
  int AntiEMVA5category_;
  int tightestAntiMuWP_;
  int tightestAntiMu2WP_;
  int tightestAntiMu3WP_;
  int tightestAntiMuMVAWP_;
  float AntiMuMVAraw_;
  int tightestHPSDBWP_;
  int tightestHPSDB3HWP_;
  float hpsDB3H_;
  int tightestHPSMVA3newDMwLTWP_;
  float hpsMVA3newDMwLT_;
  int tightestHPSMVA3newDMwoLTWP_;
  float hpsMVA3newDMwoLT_;
  int tightestHPSMVA3oldDMwLTWP_;
  float hpsMVA3oldDMwLT_;
  int tightestHPSMVA3oldDMwoLTWP_;
  float hpsMVA3oldDMwoLT_;

  /////NewTauID input variables
  float chargedIsoPtSum_;
  float neutralIsoPtSum_;
  float puCorrPtSum_; 
  int patDecayMode_;
  float dxyTau_;   
  float dxyErrTau_;    
  float dxySigTau_;     
  int hasSecVtx_;  
  float flightLength_;  
  float flightLengthError_;    
  float flightLength2_;  
  float flightLengthSig_;
  ///NewTauID useful variables
  float dxyPCAX_;        
  float dxyPCAY_;        
  float dxyPCAZ_;        
  float flightLengthX_;
  float flightLengthY_; 
  float flightLengthZ_;  
  float pvX_;   
  float pvY_;   
  float pvZ_;   
  float pvCov00_;    
  float pvCov01_;     
  float pvCov02_;      
  float pvCov11_;       
  float pvCov12_;        
  float pvCov22_;
  float svX_;          
  float svY_;
  float svZ_;
  float svCov00_; 
  float svCov01_;  
  float svCov02_;   
  float svCov11_;    
  float svCov12_;     
  float svCov22_;    

  int isTauLegMatched_;
  int isTauLegMatchedToLep_;
  int isElecLegMatched_;
  int elecFlag_, elecFlagSoft_;
  int vetoEvent_;
  float elecVetoRelIso_, elecVetoRelIsoSoft_;
  int hasKft_;

  //Electrons for antiZee
  float elecAntiZeePt_[4];
  float elecAntiZeeEta_[4];
  float elecAntiZeeSCEta_[4];
  float elecAntiZeePhi_[4];
  float elecAntiZeeRelIso_[4];
  float elecAntiZeeIdNonTrig_[4];
  float elecAntiZeeIdTrigNoIP_[4];

  // ele specific variables
  float nBrehm_;
  float likelihood_;
  float nHits_;
  float sihih_;
  float dPhi_;
  float dEta_;
  float HoE_;
  float EoP_;
  float fbrem_;
  int pfId_;
  //int isEleLikelihoodID_;
  //int isEleCutBasedID_;

  float diTauCharge_;
  float chargeL1_;
  float rhoFastJet_;
  float rho_;
  float rhoiso_;
  float rhoNeutralFastJet_;
  float embeddingWeight_;
  std::vector< double >* embeddingWeights_;
  float TauSpinnerWeight_;
  float genDiTauMass_;
  float nPUVertices_;
  float nPUaverage_;
  float nPUVerticesM1_;
  float nPUVerticesP1_;
  float nPUtruth_;

  float higgsPtWeightNom_ ;
  float higgsPtWeightUp_ ;
  float higgsPtWeightDown_ ;

  //IN top pt reweighting
  float topPtWeightNom_ ;
  float topPtWeightUp_ ;
  float topPtWeightDown_ ;

  float mcPUweight_;

  std::map<std::string,float> calcLikelihoodVariables(const pat::Jet *jet, edm::Handle<reco::VertexCollection> vC, std::string type);

  bool doElecIsoMVA_;
  float isoLeg1MVA_;
  EGammaMvaEleEstimator* fElectronIsoMVA_;


};


#endif
