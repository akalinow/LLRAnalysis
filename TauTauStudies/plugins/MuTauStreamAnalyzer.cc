#include "LLRAnalysis/TauTauStudies/interface/MuTauStreamAnalyzer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "TauAnalysis/CandidateTools/interface/CompositePtrCandidateT1T2MEtProducer.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEt.h"
#include "AnalysisDataFormats/TauAnalysis/interface/CompositePtrCandidateT1T2MEtFwd.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositDirection.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include <DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenFilterInfo.h"

#include "PhysicsTools/JetMCUtils/interface/JetMCTag.h"

////// for DCA ///////////////////////////////

#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajectoryMeasurement.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"

/////// for jet-ID ///////////////////////////

//#include "CMGTools/External/interface/PileupJetIdentifier.h"
#include "RecoJets/JetProducers/interface/PileupJetIdentifier.h"

///METCovMatrix/////
#include "AnalysisDataFormats/TauAnalysis/interface/PFMEtSignCovMatrix.h"

#include <vector>
#include <utility>
#include <map>
#include "TMatrixTBase.h"

using namespace std;
using namespace reco;

typedef std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more>::iterator CImap;

MuTauStreamAnalyzer::MuTauStreamAnalyzer(const edm::ParameterSet & iConfig){

  diTauTag_          = iConfig.getParameter<edm::InputTag>("diTaus");
  jetsTag_           = iConfig.getParameter<edm::InputTag>("jets");
  newJetsTag_        = iConfig.getParameter<edm::InputTag>("newJets");
  metTag_            = iConfig.getParameter<edm::InputTag>("met");
  rawMetTag_         = iConfig.getParameter<edm::InputTag>("rawMet");
  mvaMetTag_         = iConfig.getParameter<edm::InputTag>("mvaMet");
  metCovTag_         = iConfig.getParameter<edm::InputTag>("metCov");
  muonsTag_          = iConfig.getParameter<edm::InputTag>("muons");
  muonsRelTag_       = iConfig.getParameter<edm::InputTag>("muonsRel");
  verticesTag_       = iConfig.getParameter<edm::InputTag>("vertices");
  triggerResultsTag_ = iConfig.getParameter<edm::InputTag>("triggerResults"); 
  isMC_              = iConfig.getParameter<bool>("isMC");
  genParticlesTag_   = iConfig.getParameter<edm::InputTag>("genParticles"); 
  genTausTag_        = iConfig.getParameter<edm::InputTag>("genTaus"); 
  deltaRLegJet_      = iConfig.getUntrackedParameter<double>("deltaRLegJet",0.3);
  minCorrPt_         = iConfig.getUntrackedParameter<double>("minCorrPt",10.);
  minJetID_          = iConfig.getUntrackedParameter<double>("minJetID",0.5);
  verbose_           = iConfig.getUntrackedParameter<bool>("verbose",false);
  

  doMuIsoMVA_        = iConfig.getParameter<bool>("doMuIsoMVA");
  if( doMuIsoMVA_ ){
//     fMuonIsoMVA_ = new MuonMVAEstimator();
//     edm::FileInPath inputFileName0 = iConfig.getParameter<edm::FileInPath>("inputFileName0");
//     edm::FileInPath inputFileName1 = iConfig.getParameter<edm::FileInPath>("inputFileName1");
//     edm::FileInPath inputFileName2 = iConfig.getParameter<edm::FileInPath>("inputFileName2");
//     edm::FileInPath inputFileName3 = iConfig.getParameter<edm::FileInPath>("inputFileName3");
//     edm::FileInPath inputFileName4 = iConfig.getParameter<edm::FileInPath>("inputFileName4");
//     edm::FileInPath inputFileName5 = iConfig.getParameter<edm::FileInPath>("inputFileName5");   
//     vector<string> muoniso_weightfiles;
//     muoniso_weightfiles.push_back(inputFileName0.fullPath().data());
//     muoniso_weightfiles.push_back(inputFileName1.fullPath().data());
//     muoniso_weightfiles.push_back(inputFileName2.fullPath().data());
//     muoniso_weightfiles.push_back(inputFileName3.fullPath().data());
//     muoniso_weightfiles.push_back(inputFileName4.fullPath().data());
//     muoniso_weightfiles.push_back(inputFileName5.fullPath().data());   
//     fMuonIsoMVA_->initialize("MuonIso_BDTG_IsoRings",
// 			     MuonMVAEstimator::kIsoRings,
// 			     kTRUE,
// 			     muoniso_weightfiles);
  }
  
}

void MuTauStreamAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("tree","qqH tree");
 
  jetsBtagHE_  = new std::vector< double >();
  jetsBtagHP_  = new std::vector< double >();
  jetsBtagCSV_ = new std::vector< double >();
  bQuark_      = new std::vector< double >();

  sigDCA_      = new std::vector< double >();

  jetsChEfraction_  = new std::vector< float >();
  jetsChNfraction_  = new std::vector< float >();
  jetMoments_       = new std::vector< float >();
  jetPUMVA_         = new std::vector< float >();
  jetPUWP_          = new std::vector< float >();

  gammadR_       = new std::vector< float >();
  gammadEta_     = new std::vector< float >();
  gammadPhi_     = new std::vector< float >();
  gammaPt_       = new std::vector< float >();
  metSgnMatrix_  = new std::vector< float >();

  tauXTriggers_= new std::vector< int >();
  triggerBits_ = new std::vector< int >();

  jetsP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDUpP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDDownP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDL1OffsetP4_= new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genJetsIDP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauVisP4_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauCAP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauICAP4_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauSVfitP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauLegsP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauLegsAltP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genDiTauLegsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genTausP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  METP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genMETP4_       = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genVP4_         = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  leptonJets_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  extraMuons_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  pfMuons_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoMuonsP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoElectronsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoTausP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  // Summer12: truth (from twiki)
  std::vector< float > Summer12Lumi;
  Double_t Summer12Lumi_f[60] = {
    2.344E-05,
    2.344E-05,
    2.344E-05,
    2.344E-05,
    4.687E-04,
    4.687E-04,
    7.032E-04,
    9.414E-04,
    1.234E-03,
    1.603E-03,
    2.464E-03,
    3.250E-03,
    5.021E-03,
    6.644E-03,
    8.502E-03,
    1.121E-02,
    1.518E-02,
    2.033E-02,
    2.608E-02,
    3.171E-02,
    3.667E-02,
    4.060E-02,
    4.338E-02,
    4.520E-02,
    4.641E-02,
    4.735E-02,
    4.816E-02,
    4.881E-02,
    4.917E-02,
    4.909E-02,
    4.842E-02,
    4.707E-02,
    4.501E-02,
    4.228E-02,
    3.896E-02,
    3.521E-02,
    3.118E-02,
    2.702E-02,
    2.287E-02,
    1.885E-02,
    1.508E-02,
    1.166E-02,
    8.673E-03,
    6.190E-03,
    4.222E-03,
    2.746E-03,
    1.698E-03,
    9.971E-04,
    5.549E-04,
    2.924E-04,
    1.457E-04,
    6.864E-05,
    3.054E-05,
    1.282E-05,
    5.081E-06,
    1.898E-06,
    6.688E-07,
    2.221E-07,
    6.947E-08,
    2.047E-08
  };  


  // sigma 69.4 pb
  std::vector< float > Data2012LumiExt;
  Double_t Data2012LumiExt_f[60] = {
    1.76427e-06, 2.84466e-06, 1.88915e-05, 0.000796165, 0.00110891, 0.000439459, 0.00216878, 0.00562928, 0.0114731, 0.0199515, 0.0317062, 0.0457626, 0.0587994, 0.0700723, 0.0799624, 0.0850344, 0.0802347, 0.0690109, 0.0585324, 0.051109, 0.0457747, 0.0413901, 0.0373793, 0.0335591, 0.0298473, 0.0262066, 0.0226609, 0.0192698, 0.0160998, 0.0132078, 0.0106338, 0.00839923, 0.00650684, 0.00494315, 0.00368197, 0.00268875, 0.00192473, 0.00135048, 0.000928655, 0.000625768, 0.000413144, 0.00026721, 0.000169276, 0.000105016, 6.37928e-05, 3.79375e-05, 2.20845e-05, 1.25825e-05, 7.0155e-06, 3.8275e-06, 2.04314e-06, 1.06702e-06, 5.45147e-07, 2.72455e-07, 1.33199e-07, 6.36963e-08, 2.97937e-08, 1.36308e-08, 6.09958e-09, 2.66964e-09
  };
  

  for( int i=0; i<60; i++) {
    Data2012LumiExt.push_back(Data2012LumiExt_f[i]);
    Summer12Lumi.push_back(Summer12Lumi_f[i]);
  }

  cout << "MC = " << Summer12Lumi.size() << ", DATA = " << Data2012LumiExt.size() << endl;
  LumiWeights_ = edm::LumiReWeighting(Summer12Lumi, Data2012LumiExt);

  tree_->Branch("jetsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsP4_);
  tree_->Branch("jetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDP4_);
  tree_->Branch("jetsIDUpP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDUpP4_);
  tree_->Branch("jetsIDDownP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDDownP4_);
  tree_->Branch("jetsIDL1OffsetP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&jetsIDL1OffsetP4_);
  tree_->Branch("genJetsIDP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genJetsIDP4_);
  
  tree_->Branch("jetsBtagHE","std::vector<double>",&jetsBtagHE_);
  tree_->Branch("jetsBtagHP","std::vector<double>",&jetsBtagHP_);
  tree_->Branch("jetsBtagCSV","std::vector<double>",&jetsBtagCSV_);
  tree_->Branch("bQuark",    "std::vector<double>",&bQuark_);
  
  tree_->Branch("sigDCA","std::vector<double>",&sigDCA_);

  tree_->Branch("jetMoments","std::vector<float> ",&jetMoments_);
  tree_->Branch("jetPUMVA","std::vector<float> ",&jetPUMVA_);
  tree_->Branch("jetPUWP","std::vector<float> ", &jetPUWP_);

  tree_->Branch("gammadR","std::vector<float> ",&gammadR_);
  tree_->Branch("gammadEta","std::vector<float> ",&gammadEta_);
  tree_->Branch("gammadPhi","std::vector<float> ",&gammadPhi_);
  tree_->Branch("gammaPt","std::vector<float> ",&gammaPt_);

  tree_->Branch("metSgnMatrix","std::vector<float>",&metSgnMatrix_);

  tree_->Branch("jetsChEfraction","std::vector<float>",&jetsChEfraction_);
  tree_->Branch("jetsChNfraction","std::vector<float>",&jetsChNfraction_);

  tree_->Branch("tauXTriggers","std::vector<int>",&tauXTriggers_);
  tree_->Branch("triggerBits","std::vector<int>",&triggerBits_);

  tree_->Branch("diTauVisP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",  &diTauVisP4_);
  tree_->Branch("diTauCAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauCAP4_);
  tree_->Branch("diTauICAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauICAP4_);
  tree_->Branch("diTauSVfitP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauSVfitP4_);

  tree_->Branch("validityCA", &validityCA_,"validityCA/I");
  tree_->Branch("validityICA",&validityICA_,"validityICA/I");
 
  tree_->Branch("x1CollApprox",&x1CollApprox_,"x1CollApprox/F");
  tree_->Branch("x2CollApprox",&x2CollApprox_,"x2CollApprox/F");

  tree_->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,"diTauNSVfitMass/F");
  tree_->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,"diTauNSVfitMassErrUp/F");
  tree_->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  tree_->Branch("diTauSVfitMassErrUp",   &diTauSVfitMassErrUp_,"diTauSVfitMassErrUp/F");
  tree_->Branch("diTauSVfitMassErrDown", &diTauSVfitMassErrDown_,"diTauSVfitMassErrDown/F");

  tree_->Branch("diTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauLegsP4_);
  tree_->Branch("diTauLegsAltP4_","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauLegsAltP4_);
  tree_->Branch("genDiTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genDiTauLegsP4_);
  tree_->Branch("genTausP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genTausP4_);

  tree_->Branch("METP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&METP4_);
  tree_->Branch("genMETP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genMETP4_);
  tree_->Branch("genVP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genVP4_);
  tree_->Branch("genDecay",&genDecay_,"genDecay/I");
  tree_->Branch("parton",&parton_,"parton/I");
  tree_->Branch("genPartMult",&genPartMult_,"genPartMult/I");
  tree_->Branch("leadGenPartPdg",&leadGenPartPdg_,"leadGenPartPdg/I");
  tree_->Branch("leadGenPartPt",&leadGenPartPt_,"leadGenPartPt/F");
  tree_->Branch("hepNUP",&hepNUP_,"hepNUP/I");

  tree_->Branch("leptonJets","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&leptonJets_);
  tree_->Branch("extraMuons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&extraMuons_);
  tree_->Branch("pfMuons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&pfMuons_);
  tree_->Branch("vetoMuonsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&vetoMuonsP4_);
  tree_->Branch("vetoElectronsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&vetoElectronsP4_);
  tree_->Branch("vetoTausP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&vetoTausP4_);

  tree_->Branch("sumEt",&sumEt_,"sumEt/F");
  tree_->Branch("mTauTauMin",&mTauTauMin_,"mTauTauMin/F");
  tree_->Branch("MtLeg1",&MtLeg1_,"MtLeg1/F");
  tree_->Branch("pZeta",&pZeta_,"pZeta/F");
  tree_->Branch("pZetaVis",&pZetaVis_,"pZetaVis/F");
  tree_->Branch("pZetaSig",&pZetaSig_,"pZetaSig/F");

  //tree_->Branch("chIsoLeg1v1",&chIsoLeg1v1_,"chIsoLeg1v1/F");
  //tree_->Branch("nhIsoLeg1v1",&nhIsoLeg1v1_,"nhIsoLeg1v1/F");
  //tree_->Branch("phIsoLeg1v1",&phIsoLeg1v1_,"phIsoLeg1v1/F");
  //tree_->Branch("elecIsoLeg1v1",&elecIsoLeg1v1_,"elecIsoLeg1v1/F");
  //tree_->Branch("muIsoLeg1v1",&muIsoLeg1v1_,"muIsoLeg1v1/F");
  //tree_->Branch("chIsoPULeg1v1",&chIsoPULeg1v1_,"chIsoPULeg1v1/F");
  //tree_->Branch("nhIsoPULeg1v1",&nhIsoPULeg1v1_,"nhIsoPULeg1v1/F");
  //tree_->Branch("phIsoPULeg1v1",&phIsoPULeg1v1_,"phIsoPULeg1v1/F");

  tree_->Branch("chIsoLeg1v2",&chIsoLeg1v2_,"chIsoLeg1v2/F");
  tree_->Branch("nhIsoLeg1v2",&nhIsoLeg1v2_,"nhIsoLeg1v2/F");
  tree_->Branch("phIsoLeg1v2",&phIsoLeg1v2_,"phIsoLeg1v2/F");
  tree_->Branch("elecIsoLeg1v2",&elecIsoLeg1v2_,"elecIsoLeg1v2/F");
  tree_->Branch("muIsoLeg1v2",  &muIsoLeg1v2_ ,"muIsoLeg1v2/F");
  tree_->Branch("chIsoPULeg1v2",&chIsoPULeg1v2_,"chIsoPULeg1v2/F");
  tree_->Branch("nhIsoPULeg1v2",&nhIsoPULeg1v2_,"nhIsoPULeg1v2/F");
  tree_->Branch("phIsoPULeg1v2",&phIsoPULeg1v2_,"phIsoPULeg1v2/F");

  tree_->Branch("isoLeg1MVA",&isoLeg1MVA_,"isoLeg1MVA/F");


  tree_->Branch("chIsoLeg2",&chIsoLeg2_,"chIsoLeg2/F");
  tree_->Branch("nhIsoLeg2",&nhIsoLeg2_,"nhIsoLeg2/F");
  tree_->Branch("phIsoLeg2",&phIsoLeg2_,"phIsoLeg2/F");
  tree_->Branch("dxy1",&dxy1_,"dxy1/F");
  tree_->Branch("dxy2",&dxy2_,"dxy2/F");
  tree_->Branch("dxyE1",&dxyE1_,"dxyE1/F");
  tree_->Branch("dxyE2",&dxyE2_,"dxyE2/F");
  tree_->Branch("dz1",&dz1_,"dz1/F");
  tree_->Branch("dz2",&dz2_,"dz2/F");
  tree_->Branch("dzE1",&dzE1_,"dzE1/F");
  tree_->Branch("dzE2",&dzE2_,"dzE2/F");
  tree_->Branch("pfJetPt",&pfJetPt_,"pfJetPt/F");

  tree_->Branch("run",&run_,"run/l");
  tree_->Branch("event",&event_,"event/l");
  tree_->Branch("lumi",&lumi_,"lumi/l");
  tree_->Branch("numPV",&numPV_,"numPV/F");
  tree_->Branch("numOfDiTaus",&numOfDiTaus_,"numOfDiTaus/I");
  tree_->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");
  tree_->Branch("decayMode",&decayMode_,"decayMode/I");
  tree_->Branch("genDecayMode",&genDecayMode_,"genDecayMode/I");
  tree_->Branch("genPolarization",&genPolarization_,"genPolarization/I");
  tree_->Branch("tightestHPSWP",&tightestHPSWP_,"tightestHPSWP/I");
  tree_->Branch("tightestHPSDBWP",&tightestHPSDBWP_,"tightestHPSDBWP/I");
  tree_->Branch("tightestHPSMVAWP",&tightestHPSMVAWP_,"tightestHPSMVAWP/I");
  tree_->Branch("hpsMVA",&hpsMVA_,"hpsMVA/F");
  tree_->Branch("visibleTauMass",&visibleTauMass_,"visibleTauMass/F");
  tree_->Branch("visibleGenTauMass",&visibleGenTauMass_,"visibleGenTauMass/F");

  tree_->Branch("leadPFChargedHadrTrackPt",&leadPFChargedHadrTrackPt_,"leadPFChargedHadrTrackPt/F");
  tree_->Branch("leadPFChargedHadrTrackP", &leadPFChargedHadrTrackP_,"leadPFChargedHadrTrackP/F");
  tree_->Branch("leadPFChargedHadrPt",&leadPFChargedHadrPt_,"leadPFChargedHadrPt/F");
  tree_->Branch("leadPFChargedHadrP", &leadPFChargedHadrP_,"leadPFChargedHadrP/F");
  tree_->Branch("leadPFChargedHadrMva",&leadPFChargedHadrMva_,"leadPFChargedHadrMva/F");
  tree_->Branch("leadPFChargedHadrHcalEnergy",&leadPFChargedHadrHcalEnergy_,"leadPFChargedHadrHcalEnergy/F");
  tree_->Branch("leadPFChargedHadrEcalEnergy",&leadPFChargedHadrEcalEnergy_,"leadPFChargedHadrEcalEnergy/F");
  tree_->Branch("leadPFCandMva",&leadPFCandMva_,"leadPFCandMva/F");
  tree_->Branch("leadPFCandHcalEnergy",&leadPFCandHcalEnergy_,"leadPFCandHcalEnergy/F");
  tree_->Branch("leadPFCandEcalEnergy",&leadPFCandEcalEnergy_,"leadPFCandEcalEnergy/F");
  tree_->Branch("leadPFCandPt",&leadPFCandPt_,"leadPFCandPt/F");
  tree_->Branch("leadPFCandP",&leadPFCandP_,"leadPFCandP/F");
  tree_->Branch("emFraction",&emFraction_,"emFraction/F");
  tree_->Branch("hasGsf",&hasGsf_,"hasGsf/F");
  tree_->Branch("signalPFChargedHadrCands",&signalPFChargedHadrCands_,"signalPFChargedHadrCands/I");
  tree_->Branch("signalPFGammaCands",&signalPFGammaCands_,"signalPFGammaCands/I");


  tree_->Branch("isTauLegMatched",&isTauLegMatched_,"isTauLegMatched/I");
  tree_->Branch("isMuLegMatched",&isMuLegMatched_,"isMuLegMatched/I");
  tree_->Branch("muFlag",&muFlag_,"muFlag/I");
  tree_->Branch("vetoEvent",&vetoEvent_,"vetoEvent/I");
  tree_->Branch("isPFMuon",&isPFMuon_,"isPFMuon/I");
  tree_->Branch("isTightMuon",&isTightMuon_,"isTightMuon/I");
  tree_->Branch("muVetoRelIso",&muVetoRelIso_,"muVetoRelIso/F");
  tree_->Branch("hasKft",&hasKft_,"hasKft/I");

  tree_->Branch("diTauCharge",&diTauCharge_,"diTauCharge/F");
  tree_->Branch("chargeL1",&chargeL1_,"chargeL1/F");
  tree_->Branch("rhoFastJet",&rhoFastJet_,"rhoFastJet/F");
  tree_->Branch("rhoNeutralFastJet",&rhoNeutralFastJet_,"rhoNeutralFastJet/F");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/F");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/F");
  tree_->Branch("nPUVerticesM1",&nPUVerticesM1_,"nPUVerticesM1/F");
  tree_->Branch("nPUVerticesP1",&nPUVerticesP1_,"nPUVerticesP1/F");

  tree_->Branch("embeddingWeight",&embeddingWeight_,"embeddingWeight/F");
  tree_->Branch("nPUtruth",  &nPUtruth_,  "nPUtruth/F");

  tree_->Branch("index",&index_,"index/I");

}


MuTauStreamAnalyzer::~MuTauStreamAnalyzer(){
  delete jetsP4_; delete jetsIDP4_; delete jetsIDUpP4_; delete jetsIDDownP4_; 
  delete METP4_; delete diTauVisP4_; delete diTauCAP4_; delete diTauICAP4_; 
  delete diTauSVfitP4_; delete genVP4_;
  delete diTauLegsP4_; delete jetsBtagHE_; delete jetsBtagHP_; delete jetsBtagCSV_;
  delete bQuark_; delete diTauLegsAltP4_;
  delete tauXTriggers_; delete triggerBits_; delete sigDCA_;
  delete genJetsIDP4_; delete genDiTauLegsP4_; delete genMETP4_; delete extraMuons_; 
  delete vetoMuonsP4_; delete vetoElectronsP4_; delete vetoTausP4_;
  delete pfMuons_; delete jetsIDL1OffsetP4_;
  delete leptonJets_;
  delete genTausP4_;
  delete jetsChNfraction_; delete jetsChEfraction_;delete jetMoments_;
  delete jetPUMVA_; delete jetPUWP_;
  delete gammadR_ ; delete gammadPhi_; delete gammadEta_; delete gammaPt_;
  delete metSgnMatrix_;
  if( doMuIsoMVA_ && fMuonIsoMVA_!=0) delete fMuonIsoMVA_;
}

void MuTauStreamAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){


  //METP4_->clear();
  genVP4_->clear();
  genMETP4_->clear();
  genTausP4_->clear();
  pfMuons_->clear();
  triggerBits_->clear();
  vetoTausP4_->clear();
  vetoElectronsP4_->clear();
  vetoMuonsP4_->clear();

  edm::ESHandle<TransientTrackBuilder> builder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",builder);
  transientTrackBuilder_ = builder.product();

  
  edm::Handle<PATMuTauPairCollection> diTauHandle;
  iEvent.getByLabel(diTauTag_,diTauHandle);
  if( !diTauHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No diTau label available \n";
  const PATMuTauPairCollection* diTaus = diTauHandle.product();

  edm::Handle<pat::JetCollection> jetsHandle;
  iEvent.getByLabel(jetsTag_,jetsHandle);
  if( !jetsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No jets label available \n";
  const pat::JetCollection* jets = jetsHandle.product();

  edm::Handle<edm::View<pat::Jet> > jetsHandleForMVA;
  iEvent.getByLabel(jetsTag_,jetsHandleForMVA);

  edm::Handle<pat::JetCollection> newJetsHandle;
  const pat::JetCollection* newJets = 0;
  if(newJetsTag_.label()!=""){
    iEvent.getByLabel(newJetsTag_,newJetsHandle);
    if( !newJetsHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No newJets label available \n";
    newJets = newJetsHandle.product();
  }

  edm::Handle<edm::ValueMap<float> > puJetIdMVA;
  iEvent.getByLabel("puJetMva","fullDiscriminant",puJetIdMVA);
  
  edm::Handle<edm::ValueMap<int> > puJetIdFlag;
  iEvent.getByLabel("puJetMva","fullId",puJetIdFlag);


  edm::Handle<reco::PFCandidateCollection> pfHandle;
  iEvent.getByLabel("particleFlow",pfHandle);
  if( !pfHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No pf particles label available \n";
  const reco::PFCandidateCollection* pfCandidates = pfHandle.product();

  edm::Handle<reco::VertexCollection> pvHandle;
  iEvent.getByLabel( verticesTag_ ,pvHandle);
  if( !pvHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No PV label available \n";
  const reco::VertexCollection* vertexes = pvHandle.product();

  std::vector<float> vtxZ;
  for(unsigned int k = 0; k<vertexes->size(); k++){
    vtxZ.push_back(((*vertexes)[k].position()).z());
  }


  if(verbose_){
    cout <<  "Run " << iEvent.run() << ", event " << (iEvent.eventAuxiliary()).event() 
	 << ", lumi " << iEvent.luminosityBlock() << endl;
    cout << "List of vertexes " << endl;
    for(unsigned int k = 0; k<vertexes->size(); k++){
      cout << "Vtx[" << k << "] (x,y,z) = (" << ((*vertexes)[k].position()).x()
	   << "," << ((*vertexes)[k].position()).y() << "," << ((*vertexes)[k].position()).z() << ")"
	   << endl;
    }
  }

  numPV_ = vertexes->size();

  edm::Handle<pat::METCollection> metHandle;
  iEvent.getByLabel( metTag_, metHandle);
  if( !metHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No MET label available \n";
  const pat::METCollection* met = metHandle.product();

  edm::Handle<pat::METCollection> rawMetHandle;
  iEvent.getByLabel( rawMetTag_, rawMetHandle);
  if( !rawMetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No raw MET label available \n";
  const pat::METCollection* rawMet = rawMetHandle.product();

  edm::Handle<pat::METCollection> mvaMetHandle;
  iEvent.getByLabel( mvaMetTag_, mvaMetHandle);
  if( !mvaMetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No mva MET label available \n";
  const pat::METCollection* mvaMet = mvaMetHandle.product();

  edm::Handle<PFMEtSignCovMatrix> metCovHandle;
  iEvent.getByLabel( metCovTag_, metCovHandle);
  if( !metCovHandle.isValid() )
    edm::LogError("DataNotAvailable")
      << "No met Cov label available \n";

  edm::Handle<pat::TriggerEvent> triggerHandle;
  iEvent.getByLabel(triggerResultsTag_, triggerHandle);
  if( !triggerHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger label available \n";
  const pat::TriggerEvent* trigger = triggerHandle.product();

  edm::Handle<pat::TriggerObjectStandAloneCollection > triggerObjsHandle;
  iEvent.getByLabel(edm::InputTag("patTrigger"),triggerObjsHandle);
  if( !triggerObjsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No Trigger Objects label available \n";
  const pat::TriggerObjectStandAloneCollection* triggerObjs = triggerObjsHandle.product();


  genDecay_ = -99;
  hepNUP_   = -99;

  edm::Handle<reco::GenParticleCollection> genHandle;
  const reco::GenParticleCollection* genParticles = 0;
  iEvent.getByLabel(genParticlesTag_,genHandle); 

  if(genHandle.isValid()){
    genParticles = genHandle.product();
    int index1 = -99;
    int index2 = -99;
    for(unsigned int k = 0; k < genParticles->size(); k ++){
      if( fabs((*genParticles)[k].pdgId()) != 15 || (*genParticles)[k].status()!=2) continue;
      if(index1<0) 
	index1 = k;
      else 
	index2 = k;
    }
    if(index1>=0 && index2>=0){
      if((*genParticles)[index1].pt()<(*genParticles)[index2].pt()){
	int indexBkp = index1;
	index1 = index2;
	index2 = indexBkp;
      }
      genTausP4_->push_back((*genParticles)[index1].p4());
      genTausP4_->push_back((*genParticles)[index2].p4());
    }

  }

  if(isMC_){

    edm::Handle<LHEEventProduct > LHEHandle;
    const LHEEventProduct* LHE = 0;
    iEvent.getByLabel("source",LHEHandle);
    if(LHEHandle.isValid()){
      LHE = LHEHandle.product();
      hepNUP_ = (LHE->hepeup()).NUP;
      //cout << hepNUP_ << endl;
    }
    

    if( !genHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen particles label available \n";
    genParticles = genHandle.product();
    for(unsigned int k = 0; k < genParticles->size(); k ++){
      if( !( (*genParticles)[k].pdgId() == 23 || 
	     abs((*genParticles)[k].pdgId()) == 24 || 
	     (*genParticles)[k].pdgId() == 25 ||
	     (*genParticles)[k].pdgId() == 35 ||
             (*genParticles)[k].pdgId() == 36
	     ) || 
	  (*genParticles)[k].status()!=3)
	continue;
      if(verbose_) cout << "Boson status, pt,phi " << (*genParticles)[k].status() 
			<< "," << (*genParticles)[k].pt() << "," 
			<< (*genParticles)[k].phi() << endl;

      genVP4_->push_back( (*genParticles)[k].p4() );
      genDecay_ = (*genParticles)[k].pdgId();

      int breakLoop = 0;
      for(unsigned j = 0; j< ((*genParticles)[k].daughterRefVector()).size() && breakLoop!=1; j++){
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 11 ){
	  genDecay_ *= 11;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 13 ){
	  genDecay_ *= 13;
	  breakLoop = 1;
	}
	if( abs(((*genParticles)[k].daughterRef(j))->pdgId()) == 15 ){
	  genDecay_ *= 15;
	  breakLoop = 1;
	}
      }
      if(verbose_) cout << "Decays to pdgId " << genDecay_/(*genParticles)[k].pdgId()  << endl;
      break;
    }
  }


  edm::Handle<reco::GenJetCollection> tauGenJetsHandle;
  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  nPUVertices_       = -99.;
  nPUVerticesP1_     = -99.;
  nPUVerticesM1_     = -99.;

  nPUtruth_          = -99.;

  const reco::GenJetCollection* tauGenJets = 0;
  if(isMC_){
    // tag gen jets
    iEvent.getByLabel(genTausTag_,tauGenJetsHandle);
    if( !tauGenJetsHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No gen jet label available \n";
    tauGenJets = tauGenJetsHandle.product();

    // PU infos
    iEvent.getByType(puInfoH);
    if(puInfoH.isValid()){
      for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){

	//cout << "Bunc crossing " << it->getBunchCrossing() << endl;
	if(it->getBunchCrossing() == 0 ) 
	  //nPUVertices_  = it->getPU_NumInteractions();  /old ICHEP
	  nPUVertices_  = it->getTrueNumInteractions();
	else if(it->getBunchCrossing() == -1)  
	  //nPUVerticesM1_= it->getPU_NumInteractions();  
	  nPUVerticesM1_= it->getTrueNumInteractions();
	else if(it->getBunchCrossing() == +1)  
	  //nPUVerticesP1_= it->getPU_NumInteractions();
	  nPUVerticesP1_= it->getTrueNumInteractions();
	//nPUtruth_ = it->getTrueNumInteractions();	
      }
    }
  }

  if(verbose_){
    cout << "Num of PU = "          << nPUVertices_ << endl;
    cout << "Num of OOT PU = "      << nPUVerticesM1_+nPUVerticesP1_<< endl;
    cout << "Num of true interactions = " << nPUtruth_ << endl;
  }

  mcPUweight_ = LumiWeights_.weight( nPUVertices_ );

  //edm::Handle<double> rhoFastJetHandle;
  //iEvent.getByLabel(edm::InputTag("kt6PFJetsCentral","rho",""), rhoFastJetHandle);
  //rhoFastJet_ = rhoFastJetHandle.isValid() ? (*rhoFastJetHandle) : -99;

  edm::Handle<double> rhoFastJetHandle;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsForRhoComputationVoronoi","rho"), rhoFastJetHandle);
  rhoFastJet_ = rhoFastJetHandle.isValid() ? (*rhoFastJetHandle) : -99;

  edm::Handle<GenFilterInfo> embeddingWeightHandle;
  iEvent.getByLabel(edm::InputTag("generator","minVisPtFilter",""), embeddingWeightHandle);
  embeddingWeight_ = embeddingWeightHandle.isValid() ? embeddingWeightHandle->filterEfficiency() : 1.0;

  //cout << "--- EMBEDDING WEIGHT : " << embeddingWeight_ << endl;

  edm::Handle<double> rhoNeutralFastJetHandle;
  iEvent.getByLabel(edm::InputTag("kt6PFJetsNeutral","rho", ""), rhoNeutralFastJetHandle);
  rhoNeutralFastJet_ = rhoNeutralFastJetHandle.isValid() ? (*rhoNeutralFastJetHandle) : -99;
  
  edm::Handle<pat::MuonCollection> muonsHandle;
  iEvent.getByLabel( muonsTag_ ,muonsHandle);
  if( !muonsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muons label available \n";
  const pat::MuonCollection* muons = muonsHandle.product();
  if(muons->size()<1){
    cout << " No muons !!! " << endl;
    return;
  } else if(muons->size()>1 && verbose_){
    cout << "WARNING: "<< muons->size() << "  muons found in the event !!! We will select only one" << endl;
  }
  edm::Handle<pat::MuonCollection> muonsRelHandle;
  iEvent.getByLabel(muonsRelTag_, muonsRelHandle);
  if( !muonsRelHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No muonsRel label available \n";
  const pat::MuonCollection* muonsRel = muonsRelHandle.product();
  if(muonsRel->size()<1){
    cout << " No muonsRel !!! " << endl;
    return;
  } else if(muonsRel->size()>1 && verbose_){
    cout << "WARNING: "<< muonsRel->size() << "  muonsRel found in the event !!! We will select only one" << endl;
  }
  

  // Loose leptons for veto //
  vetoEvent_ = 0;

  edm::Handle<pat::MuonCollection> muonsForVetoHandle;
  edm::Handle<pat::ElectronCollection> electronsForVetoHandle;
  edm::Handle<pat::TauCollection> tausForVetoHandle;

  iEvent.getByLabel( "muonsForVeto" ,     muonsForVetoHandle);
  iEvent.getByLabel( "electronsForVeto" , electronsForVetoHandle);
  iEvent.getByLabel( "tausForVeto" ,      tausForVetoHandle);

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > buffer;
  bool alreadyThere = false;
  if( muonsForVetoHandle.isValid() ) {
    const pat::MuonCollection* muonsForVeto         = muonsForVetoHandle.product();
    for(unsigned int l = 0; l < muonsForVeto->size() ; l++)
      vetoMuonsP4_->push_back((*muonsForVeto)[l].p4());
  }
  
  if( electronsForVetoHandle.isValid() ) {
    const pat::ElectronCollection* electronsForVeto = electronsForVetoHandle.product();
    for( unsigned int l = 0; l < electronsForVeto->size() ; l++ ){
      alreadyThere = false;
      for(unsigned int m = 0; m < vetoMuonsP4_->size() && !alreadyThere; m++){
        if( Geom::deltaR( (*electronsForVeto)[l].p4(), (*vetoMuonsP4_)[m] ) < 0.3 ) alreadyThere = true;
      }
      if(!alreadyThere){
        //cout << "new electron eta " <<  (*electronsForVeto)[l].eta() << endl;
        buffer.push_back((*electronsForVeto)[l].p4());
      }
    }
    for(unsigned int m = 0; m < buffer.size() ; m++){
      vetoElectronsP4_->push_back( buffer[m] );
    }
    
  }
  buffer.clear();
  alreadyThere = false;

  if( tausForVetoHandle.isValid() ) {
    const pat::TauCollection* tausForVeto           = tausForVetoHandle.product();
    for(unsigned int l = 0; l < tausForVeto->size() ; l++){
      alreadyThere = false; 
      for(unsigned int m = 0; m < vetoMuonsP4_->size() ; m++){
        if( Geom::deltaR( (*tausForVeto)[l].p4(), (*vetoMuonsP4_)[m] ) < 0.3 ) alreadyThere = true;
      }
      for(unsigned int m = 0; m < vetoElectronsP4_->size() ; m++){ 
        if( Geom::deltaR( (*tausForVeto)[l].p4(), (*vetoElectronsP4_)[m] ) < 0.3 ) alreadyThere = true; 
      }
      if(!alreadyThere){
        //cout << "new taus eta " <<  (*tausForVeto)[l].eta() << endl;
        buffer.push_back((*tausForVeto)[l].p4());
      }
    }
    for(unsigned int m = 0; m < buffer.size() ; m++){
      vetoTausP4_->push_back( buffer[m] );
    }
  }

  /////////////////////////


  const PATMuTauPair *theDiTau = 0;
  if(diTaus->size()<1){
    cout << " No diTau !!! " << endl;
    return;
  } else if(diTaus->size()>1 && verbose_){
    cout << "WARNING: "<< diTaus->size() << "  diTaus found in the event !!! We will select only one" << endl;
  }
  numOfDiTaus_ = diTaus->size();

  muFlag_       = 0;
  muVetoRelIso_ = 0.;

  bool found = false;
  for(unsigned int i=0; i<muonsRel->size(); i++){
    for(unsigned int j=i+1; j<muonsRel->size(); j++){
      
      if(found) continue;
      
      if( Geom::deltaR( (*muonsRel)[i].p4(),(*muonsRel)[j].p4())>0.3
	  && (*muonsRel)[i].charge()*(*muonsRel)[j].charge()<0
	  && (*muonsRel)[j].userFloat("PFRelIsoDB04v2")<0.3 
	  && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	muFlag_       = 1;
	muVetoRelIso_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	if(verbose_) cout<< "Two muons failing diMu veto: flag= " << muFlag_ << endl;
	found=true;
      }
      else if( Geom::deltaR( (*muonsRel)[i].p4(),(*muonsRel)[j].p4())>0.3
	       && (*muonsRel)[i].charge()*(*muonsRel)[j].charge()>0
	       && (*muonsRel)[j].userFloat("PFRelIsoDB04v2")<0.3 
	       && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3 ){
	muFlag_       = 2;
	muVetoRelIso_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	if(verbose_) cout<< "Two muons with SS: flag= " << muFlag_ << endl;
	//found=true;
      }
    }
  }
  

  vector<string> triggerPaths;
  vector<string> XtriggerPaths;
  vector<string> HLTfiltersMu;
  vector<string> HLTfiltersTau;

  if(isMC_){
    
    // X-triggers 
    XtriggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*");

    // for Summer12-53X
    //HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v1

    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2");
    
    HLTfiltersMu.push_back("hltL1sMu14erORMu16er");
    HLTfiltersMu.push_back("hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15");
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    
  }
  else{
    
    XtriggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v*");
    XtriggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*");
    XtriggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v*");
    XtriggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v*");

    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4");
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v5");
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v6");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v3");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v4");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v5");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v6");
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v7");
    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v4");
    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v5");
    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v6");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v2");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v3");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v4");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v5");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v6");
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v7");

    HLTfiltersMu.push_back("hltSingleMuIsoL1s14L3IsoFiltered15eta2p1");
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10");
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    HLTfiltersMu.push_back("hltOverlapFilterMu18LooseIsoPFTau20");
    HLTfiltersMu.push_back("hltOverlapFilterMu17LooseIsoPFTau20");

    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterMu18LooseIsoPFTau20");
    HLTfiltersTau.push_back("hltOverlapFilterMu17LooseIsoPFTau20");

  }

  for(unsigned int i=0;i<triggerPaths.size();i++){
    if(!trigger){
      continue;
      cout << "Invalid triggerEvent !" << endl;
    }
    const pat::TriggerPath *triggerPath =  trigger->path(triggerPaths[i]);

    if(verbose_){
      cout<<  "Testing " << triggerPaths[i] << endl;
      if(triggerPath) cout << "Is there..." << endl;
      if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
      if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
    }

    if(triggerPath && triggerPath->wasRun() && 
       triggerPath->wasAccept() && 
       triggerPath->prescale()==1) triggerBits_->push_back(1);
    else if (triggerPath && triggerPath->wasRun() && 
	     triggerPath->wasAccept() && 
	     triggerPath->prescale()!=1) triggerBits_->push_back(2);
    else triggerBits_->push_back(0);
  }
  
  
  unsigned int index = 0;

  //////////////// pair-ordering by sumPt ////////////////////////
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTaus;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausOS;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausSS;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausLooseIso;
  for(unsigned int i=0; i< diTaus->size(); i++){
    float sumPt    = (((*diTaus)[i].leg1())->pt() + ((*diTaus)[i].leg2())->pt());
    int pairCharge = (((*diTaus)[i].leg1())->charge()*((*diTaus)[i].leg2())->charge());

    const pat::Tau*  tau_i = dynamic_cast<const pat::Tau*>(  ((*diTaus)[i].leg2()).get() );
    if(tau_i->tauID("byLooseCombinedIsolationDeltaBetaCorr")>0.5 ||
       tau_i->tauID("byLooseIsolationMVA")>0.5)
      sortedDiTausLooseIso.insert( make_pair( sumPt, i ) );
    sortedDiTaus.insert( make_pair( sumPt, i ) );
    if(pairCharge<0) 
      sortedDiTausOS.insert( make_pair( sumPt, i ) );
    else
      sortedDiTausSS.insert( make_pair( sumPt, i ) );
  }
  if( sortedDiTausOS.size()>0 ) 
    index = (sortedDiTausOS.begin())->second ;
  else
    index = (sortedDiTausSS.begin())->second ;
    
  numOfLooseIsoDiTaus_ = sortedDiTausLooseIso.size();

 

  ////////////////////////////////////////////////////////

  if(isMC_) 
    genMETP4_->push_back( (*rawMet)[0].genMET()->p4() );
  sumEt_  = (*met)[0].sumEt();
  run_   = iEvent.run();
  event_ = (iEvent.eventAuxiliary()).event();
  lumi_  = iEvent.luminosityBlock();

  // loop over pfMuon to make sure we don't have low-mass DY events
  for(unsigned int i = 0 ; i < pfCandidates->size() ; i++){
    const reco::PFCandidate cand = (*pfCandidates)[i];
    if(! (cand.particleId()== PFCandidate::mu && cand.pt()>0.5) ) continue;
    float dz = (cand.trackRef()).isNonnull() ? (cand.trackRef())->dz( ((*vertexes)[0]).position() ) : 999.;
    if(dz>0.2) continue;
    pfMuons_->push_back(cand.p4());
  }


  ///////////////// The loop ////////////////////////////////
  //theDiTau = &(*diTaus)[index];

  int diTauCounter = -1;
  for(std::map<double, unsigned int , MuTauStreamAnalyzer::more>::iterator iter = sortedDiTaus.begin(); 
      iter != sortedDiTaus.end() ; iter++){
    diTauCounter++;

    index_   = diTauCounter;
    theDiTau = &(*diTaus)[ iter->second ];
    
    jetsP4_->clear();
    jetsIDP4_->clear();
    jetsIDUpP4_->clear();
    jetsIDDownP4_->clear();
    jetsIDL1OffsetP4_->clear();
    diTauVisP4_->clear();
    diTauCAP4_->clear();
    diTauICAP4_->clear();
    diTauSVfitP4_->clear();
    diTauLegsP4_->clear();
    diTauLegsAltP4_->clear();
    jetsChNfraction_->clear();
    jetsChEfraction_->clear();
    jetMoments_->clear();
    jetPUMVA_->clear();
    jetPUWP_->clear();
    gammadR_->clear();
    gammadEta_->clear();
    gammadPhi_->clear();
    gammaPt_->clear();
    metSgnMatrix_->clear();
    genJetsIDP4_->clear();
    genDiTauLegsP4_->clear();
    jetsBtagHE_->clear();
    jetsBtagHP_->clear();
    jetsBtagCSV_->clear();
    bQuark_->clear();
    sigDCA_->clear();
    tauXTriggers_->clear();
    extraMuons_->clear();
    leptonJets_->clear();
    METP4_->clear();
    
    diTauCharge_ =  theDiTau->charge();
    chargeL1_    =  theDiTau->leg1()->charge();
    MtLeg1_      =  theDiTau->mt1MET();
    pZeta_       =  theDiTau->pZeta();
    pZetaVis_    =  theDiTau->pZetaVis();
    pZetaSig_    =  theDiTau->pZetaSig();
    mTauTauMin_  =  theDiTau->mTauTauMin();
    /*if(theDiTau->hasMEtSignMatrix()){
      const TMatrixD cov = theDiTau->metSignMatrix();
      const double* elements; 
      elements = cov.GetMatrixArray();
      cout<<" met cov default "<<elements[0]<<endl;
      metSgnMatrix_->push_back( elements[0] ); //sigma_E    
      metSgnMatrix_->push_back( elements[1] ); //sigma_Ephi   
      metSgnMatrix_->push_back( elements[3] ); //sigma_phi   
    }
    */

    if(metCovHandle.isValid()){
      const TMatrixD cov = (*metCovHandle);
      const double* elements;
      elements = cov.GetMatrixArray();
      //cout<<" met cov modified "<<elements[0]<<endl;
      metSgnMatrix_->push_back( elements[0] ); //sigma_E
      metSgnMatrix_->push_back( elements[1] ); //sigma_Ephi
      metSgnMatrix_->push_back( elements[3] ); //sigma_phi
    }
    
    METP4_->push_back((*rawMet)[0].p4()); 
    METP4_->push_back((*met)[0].p4());
    METP4_->push_back(theDiTau->met()->p4());
    METP4_->push_back((*mvaMet)[0].p4()); 

    isMuLegMatched_  = 0;
    isTauLegMatched_ = 0;
    
    const pat::Muon* leg1 = dynamic_cast<const pat::Muon*>( (theDiTau->leg1()).get() );
    const pat::Tau*  leg2 = dynamic_cast<const pat::Tau*>(  (theDiTau->leg2()).get() );

    for(unsigned int i=0; i<muonsRel->size(); i++){
      if( Geom::deltaR( (*muonsRel)[i].p4(),leg1->p4())>0.3) 
	extraMuons_->push_back( (*muonsRel)[i].p4() );
    }
    

    double iDCA3D,iDCA3DE,iDCA2D,iDCA2DE,
      iDCARPhi3D,iDCARPhi3DE,iDCARPhi2D,iDCARPhi2DE;

    if( (leg1->innerTrack()).isNonnull() && 
	(leg2->leadPFChargedHadrCand()->trackRef()).isNonnull())
      computeDCASig(iDCA3D,iDCA3DE,iDCA2D,iDCA2DE,
		    iDCARPhi3D,iDCARPhi3DE,iDCARPhi2D,iDCARPhi2DE,
		    (const reco::Track*)((leg1->innerTrack()).get()),
		    (const reco::Track*)((leg2->leadPFChargedHadrCand()->trackRef()).get())
		    );
    sigDCA_->push_back(iDCA3D);
    sigDCA_->push_back(iDCA3DE);
    sigDCA_->push_back(iDCA2D);
    sigDCA_->push_back(iDCA2DE);
    sigDCA_->push_back(iDCARPhi3D);
    sigDCA_->push_back(iDCARPhi3DE);
    sigDCA_->push_back(iDCARPhi2D);
    sigDCA_->push_back(iDCARPhi2DE);
    if(verbose_){
      cout << "DCA3D: " << iDCA3D << endl;
      cout << "DCA3DE: " << iDCA3DE << endl;
    }


    for(unsigned int i=0 ; i< HLTfiltersMu.size() ; i++){
      bool matched = false;
      for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
	pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));
	if(verbose_) {
	  if( Geom::deltaR( aObj->triggerObject().p4(), leg1->p4() )<0.5 ){
	    for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
	      cout << "Object passing " << (aObj->filterLabels())[k] << " within 0.5 of muon" << endl;
	    }
	  }
	}
	if( Geom::deltaR( aObj->triggerObject().p4(), leg1->p4() )<0.5  && aObj->hasFilterLabel(HLTfiltersMu[i]) && aObj->hasTriggerObjectType(trigger::TriggerMuon)){
	  matched = true;
	}
      }
      if(matched) 
	tauXTriggers_->push_back(1);
      else 
	tauXTriggers_->push_back(0);
      if(verbose_){
	if(matched) cout << "Muon matched within dR=0.5 with trigger object passing filter " << HLTfiltersMu[i] << endl;
	else cout << "!!! Muon is not trigger matched within dR=0.5 !!!" << endl;
      }
    }
    for(unsigned int i=0 ; i< HLTfiltersTau.size() ; i++){
      bool matched = false;
      for(pat::TriggerObjectStandAloneCollection::const_iterator it = triggerObjs->begin() ; it !=triggerObjs->end() ; it++){
	pat::TriggerObjectStandAlone *aObj = const_cast<pat::TriggerObjectStandAlone*>(&(*it));
	if(verbose_) {
	  if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5 ){
	    for(unsigned int k =0; k < (aObj->filterLabels()).size() ; k++){
	      cout << "Object passing " << (aObj->filterLabels())[k] << " within 0.5 of tau" << endl;
	    }
	  }
	}
	if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(HLTfiltersTau[i]) && aObj->hasTriggerObjectType(trigger::TriggerTau) ){
	  matched = true;
	}
      }
      if(matched) 
	tauXTriggers_->push_back(1);
      else 
      tauXTriggers_->push_back(0);
      if(verbose_){
	if(matched) cout << "Tau matched within dR=0.5 with trigger object passing filter " << HLTfiltersTau[i] << endl;
	else cout << "!!! Tau is not trigger matched within dR=0.5 !!!" << endl;
      }
    }
    

    // triggers Mu
    if(verbose_){
      const pat::TriggerObjectStandAloneCollection trColl = leg1->triggerObjectMatchesByType(trigger::TriggerMuon);
      cout << "Mu triggers" << endl;
      for(pat::TriggerObjectStandAloneCollection::const_iterator it = trColl.begin(); it != trColl.end(); it++){
	for(unsigned int k = 0; k < (it->pathNames(false,false)).size(); k++){
	  cout << (it->pathNames(false,false))[k] << " with filters: " << endl;
	  for(unsigned int m = 0; m < (it->filterLabels()).size(); m++){
	    cout << "  - " << (it->filterLabels())[m] << endl;
	  }
	}
      }
    }
    // triggers Tau
    if(verbose_){
      const pat::TriggerObjectStandAloneCollection trColl = leg2->triggerObjectMatchesByType(trigger::TriggerTau);
      cout << "Tau triggers" << endl;
      for(pat::TriggerObjectStandAloneCollection::const_iterator it = trColl.begin(); it != trColl.end(); it++){
	for(unsigned int k = 0; k < (it->pathNames(false,false)).size(); k++){
	  cout << (it->pathNames(false,false))[k]  << " with filters: " << endl;
	  for(unsigned int m = 0; m < (it->filterLabels()).size(); m++){
	    cout << "  - " << (it->filterLabels())[m] << endl;
	  }
	}
      }
    }
    
    diTauLegsP4_->push_back(leg1->p4());
    diTauLegsP4_->push_back(leg2->p4());
    diTauLegsAltP4_->push_back(leg1->p4());
    diTauLegsAltP4_->push_back(leg2->alternatLorentzVect());
    
    genDecayMode_    = -99;
    genPolarization_ = -99;
    parton_          = -99;
    genPartMult_     = -99;
    leadGenPartPdg_  = -99;
    leadGenPartPt_   = -99;

    if(isMC_){
      if( (leg1->genParticleById(13,0,true)).isNonnull() ){
	genDiTauLegsP4_->push_back( leg1->genParticleById(13,0,true)->p4() );
	isMuLegMatched_ = 1;
      }
      else{
	genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
	if(verbose_){
	  for(unsigned int l = 0; l < leg1->genParticlesSize() ; l++){
	    if((leg1->genParticleRefs())[l]->pt() < 0.5 ) continue;
	    cout << "Mu leg matchged to particle " << (leg1->genParticleRefs())[l]->pdgId() 
		 << " with pt " << (leg1->genParticleRefs())[l]->pt()
		 << endl;
	  }
	}
      }
      
      bool leg2IsFromMu = false;
      math::XYZTLorentzVectorD genMuP4(0,0,0,0);
      for(unsigned int k = 0; k < genParticles->size(); k ++){
	if( abs((*genParticles)[k].pdgId()) != 13  || (*genParticles)[k].status()!=3 )
	  continue;
	if(Geom::deltaR( (*genParticles)[k].p4(),leg2->p4())<0.15){
	  leg2IsFromMu = true;
	  genMuP4 = (*genParticles)[k].p4();
	}
      }
      
      if( leg2->genJet() !=0 )
	genDiTauLegsP4_->push_back(leg2->genJet()->p4());
      else if(leg2IsFromMu)
	genDiTauLegsP4_->push_back( genMuP4 );
      else{
	genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
	if(verbose_) cout << "WARNING: no genJet matched to the leg2 with eta,phi " << leg2->eta() << ", " << leg2->phi() << endl;
      }
      
      bool tauHadMatched = false;
      for(unsigned int k = 0; k < tauGenJets->size(); k++){
	if( Geom::deltaR( (*tauGenJets)[k].p4(),leg2->p4() ) < 0.15 ) tauHadMatched = true;
      }
      
      if( ( (leg2->genParticleById( 15,0,true)).isNonnull() || 
	    (leg2->genParticleById(-15,0,true)).isNonnull() ) 
	  && tauHadMatched ){
	isTauLegMatched_ = 1;
	if(leg2->genJet()){

	  //cout << "Tau Mass = " << (leg2->genJet()->p4()).M() << endl;
	  string genTauDecay = JetMCTagUtils::genTauDecayMode( *(leg2->genJet()) );
	  if( genTauDecay.find("oneProng0Pi0")!=string::npos ) 
	    genDecayMode_ = 0;
	  else if( genTauDecay.find("oneProng1Pi0")!=string::npos )
	    genDecayMode_ = 1;
	  else if( genTauDecay.find("oneProng2Pi0")!=string::npos )
	    genDecayMode_ = 2;
	  else if( genTauDecay.find("oneProngOther")!=string::npos )
	    genDecayMode_ = 3;
	  else if( genTauDecay.find("threeProng0Pi0")!=string::npos )
	    genDecayMode_ = 4;
	  else if( genTauDecay.find("threeProng1Pi0")!=string::npos )
	    genDecayMode_ = 5;
	  else if( genTauDecay.find("threeProngOther")!=string::npos )
	    genDecayMode_ = 6;
	  else if( genTauDecay.find("rare")!=string::npos )
	    genDecayMode_ = 7;
	  else
	    genDecayMode_ = 99;
	}
      }
      else if(verbose_){
	for(unsigned int l = 0; l < leg2->genParticlesSize() ; l++){
	  if((leg2->genParticleRefs())[l]->pt() < 0.5 ) continue;
	  cout << "Tau leg matchged to particle " << (leg2->genParticleRefs())[l]->pdgId() 
	       << " with pt " << (leg2->genParticleRefs())[l]->pt()
	       << endl;
	}
      }
    }
    
    
    if((leg2->signalPFChargedHadrCands()).size()==1 && (leg2->signalPFGammaCands()).size()==0) decayMode_ = 0; 
    else if((leg2->signalPFChargedHadrCands()).size()==1 && (leg2->signalPFGammaCands()).size()>0)  decayMode_ = 1; 
    else if((leg2->signalPFChargedHadrCands()).size()==3) decayMode_ = 2; 
    else  decayMode_ = -99;
    
    
    
    for(unsigned int k = 0 ; k < (leg2->signalPFGammaCands()).size() ; k++){
      reco::PFCandidateRef gamma = (leg2->signalPFGammaCands()).at(k);
      if( (leg2->leadPFChargedHadrCand()).isNonnull() ){
	gammadR_->push_back(   Geom::deltaR( gamma->p4(), leg2->leadPFChargedHadrCand()->p4() ) );
	gammadPhi_->push_back( Geom::deltaPhi( gamma->p4(), leg2->leadPFChargedHadrCand()->p4() ) );
	gammadEta_->push_back( gamma->eta() - leg2->leadPFChargedHadrCand()->eta() ) ;
      }
      else{
	gammadR_->push_back(   Geom::deltaR( gamma->p4(), leg2->p4() ) );
	gammadPhi_->push_back( Geom::deltaPhi( gamma->p4(), leg2->p4() ) );
	gammadEta_->push_back(  gamma->eta() - leg2->eta() ) ;
      }
      gammaPt_->push_back(  gamma->pt() );
    }
    
    

    visibleTauMass_    = leg2->mass();
    visibleGenTauMass_ = (leg2->genJet())!=0 ? (leg2->genJet()->p4()).M() : -99 ;
    
    if((leg2->leadPFChargedHadrCand()->trackRef()).isNonnull()){
      leadPFChargedHadrTrackPt_ = leg2->leadPFChargedHadrCand()->trackRef()->pt();
      leadPFChargedHadrTrackP_  = leg2->leadPFChargedHadrCand()->trackRef()->p();
    } else if((leg2->leadPFChargedHadrCand()->gsfTrackRef()).isNonnull()){
      leadPFChargedHadrTrackPt_ = leg2->leadPFChargedHadrCand()->gsfTrackRef()->pt();
      leadPFChargedHadrTrackP_  = leg2->leadPFChargedHadrCand()->gsfTrackRef()->p();
    } else{
      leadPFChargedHadrTrackPt_ = -99;
      leadPFChargedHadrTrackP_  = -99;
    }
    leadPFChargedHadrPt_  = leg2->leadPFChargedHadrCand()->pt();
    leadPFChargedHadrP_  = leg2->leadPFChargedHadrCand()->p();  
    
    leadPFChargedHadrMva_        =   leg2->electronPreIDOutput() ;	
    leadPFChargedHadrHcalEnergy_ =  (leg2->leadPFChargedHadrCand()).isNonnull() ? (leg2->leadPFChargedHadrCand())->hcalEnergy() : -99 ;
    leadPFChargedHadrEcalEnergy_ =  (leg2->leadPFChargedHadrCand()).isNonnull() ? (leg2->leadPFChargedHadrCand())->ecalEnergy() : -99;
    
    if( (leg2->leadPFCand()).isNonnull() ){
      leadPFCandMva_               =  (leg2->leadPFCand())->mva_e_pi() ;	
      leadPFCandHcalEnergy_        =  (leg2->leadPFCand())->hcalEnergy() ;
      leadPFCandEcalEnergy_        =  (leg2->leadPFCand())->ecalEnergy() ;
      leadPFCandPt_                =  (leg2->leadPFCand())->pt();
      leadPFCandP_                 =  (leg2->leadPFCand())->p();
    }
    
    

    signalPFChargedHadrCands_ = (leg2->signalPFChargedHadrCands()).size();
    signalPFGammaCands_       = (leg2->signalPFGammaCands()).size();
    emFraction_ = leg2->emFraction();
    hasGsf_     = ((leg2->leadPFChargedHadrCand())->gsfTrackRef()).isNonnull() ? 1. : 0.;

        
    
    tightestHPSWP_ = -1;
    if(leg2->tauID("byVLooseIsolation")>0.5) tightestHPSWP_++;
    if(leg2->tauID("byLooseIsolation")>0.5)  tightestHPSWP_++;
    if(leg2->tauID("byMediumIsolation")>0.5) tightestHPSWP_++;
    if(leg2->tauID("byTightIsolation")>0.5)  tightestHPSWP_++;
    tightestHPSDBWP_ = -1;
    if(leg2->tauID("byVLooseCombinedIsolationDeltaBetaCorr")>0.5) tightestHPSDBWP_++;
    if(leg2->tauID("byLooseCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_++;
    if(leg2->tauID("byMediumCombinedIsolationDeltaBetaCorr")>0.5) tightestHPSDBWP_++;
    if(leg2->tauID("byTightCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_++;
    tightestHPSMVAWP_ = -1;
    if(leg2->tauID("byLooseIsolationMVA") >0.5) tightestHPSMVAWP_++;
    if(leg2->tauID("byMediumIsolationMVA")>0.5) tightestHPSMVAWP_++;
    if(leg2->tauID("byTightIsolationMVA") >0.5) tightestHPSMVAWP_++;
    hpsMVA_  = leg2->tauID("byIsolationMVAraw");

    pfJetPt_ = (leg2->pfJetRef()).isNonnull() ? leg2->pfJetRef()->pt() : -99;
     
    dxy1_  = vertexes->size()!=0 && (leg1->innerTrack()).isNonnull() ? leg1->innerTrack()->dxy( (*vertexes)[0].position() ) : -99;
    dz1_   = vertexes->size()!=0 && (leg1->innerTrack()).isNonnull() ? leg1->innerTrack()->dz( (*vertexes)[0].position() ) : -99;
    dxyE1_ = vertexes->size()!=0 && (leg1->innerTrack()).isNonnull() ? leg1->innerTrack()->dxyError() : -99;
    dzE1_  = vertexes->size()!=0 && (leg1->innerTrack()).isNonnull() ? leg1->innerTrack()->dzError() : -99;
    
    dxy2_  = -99;
    dz2_   = -99;
    dxyE2_ = -99;
    dzE2_  = -99;
    hasKft_ = 0;
    if( vertexes->size()!=0 && (leg2->leadPFChargedHadrCand()).isNonnull() ){
      if( (leg2->leadPFChargedHadrCand()->trackRef()).isNonnull() ){
	dxy2_   = leg2->leadPFChargedHadrCand()->trackRef()->dxy( (*vertexes)[0].position() );
	dz2_    = leg2->leadPFChargedHadrCand()->trackRef()->dz( (*vertexes)[0].position() );
	dxyE2_  = leg2->leadPFChargedHadrCand()->trackRef()->dxyError();
	dzE2_   = leg2->leadPFChargedHadrCand()->trackRef()->dzError();
	hasKft_ = 1;
      }
      if( (leg2->leadPFChargedHadrCand()->gsfTrackRef()).isNonnull() ){
	dxy2_  = leg2->leadPFChargedHadrCand()->gsfTrackRef()->dxy( (*vertexes)[0].position() );
	dz2_   = leg2->leadPFChargedHadrCand()->gsfTrackRef()->dz( (*vertexes)[0].position() );
	dxyE2_ = leg2->leadPFChargedHadrCand()->gsfTrackRef()->dxyError();
	dzE2_  = leg2->leadPFChargedHadrCand()->gsfTrackRef()->dzError();
      }
    }
    
    isTightMuon_= 
      (leg1->isGlobalMuon() &&
       leg1->globalTrack().isNonnull() &&
       leg1->globalTrack()->normalizedChi2()<10 &&
       (leg1->globalTrack()->hitPattern()).numberOfValidMuonHits()>0 &&
       leg1->numberOfMatchedStations()>1 &&    
       (leg1->innerTrack()->hitPattern()).numberOfValidPixelHits()>0 &&
       (leg1->track()->hitPattern()).trackerLayersWithMeasurement()>5) ? 1 : 0;

    isPFMuon_   = leg1->userInt("isPFMuon");
    isoLeg1MVA_ = -99;
    if(doMuIsoMVA_ && fMuonIsoMVA_!=0 && vertexes->size()){
      
      const reco::GsfElectronCollection dummyGsfColl;
      const reco::MuonCollection dummyRecoMuon;
      
      const reco::Muon* aMuon = static_cast<const reco::Muon*>(leg1); 
      // Mu-Iso MVA
      isoLeg1MVA_ = isMC_ ?
	fMuonIsoMVA_->mvaValue( *aMuon, (*vertexes)[0], 
				*pfCandidates, rhoFastJet_, 
				MuonEffectiveArea::kMuEAFall11MC, 
				dummyGsfColl, dummyRecoMuon) :
	fMuonIsoMVA_->mvaValue( *aMuon, (*vertexes)[0], 
				*pfCandidates, rhoFastJet_, 
				MuonEffectiveArea::kMuEAData2011, 
				dummyGsfColl, dummyRecoMuon) ;
      //cout << "Muon Iso MVA = " << isoLeg1MVA_ << endl;
    }
    
    // isoDeposit definition: 2011
    isodeposit::AbsVetos vetos2011ChargedLeg1; 
    isodeposit::AbsVetos vetos2011AllChargedLeg1; 
    isodeposit::AbsVetos vetos2011NeutralLeg1; 
    isodeposit::AbsVetos vetos2011PhotonLeg1;
     
    vetos2011ChargedLeg1.push_back(new isodeposit::ConeVeto(reco::isodeposit::Direction(leg1->eta(),leg1->phi()),0.0001));
    vetos2011ChargedLeg1.push_back(new isodeposit::ThresholdVeto(0.0));
    vetos2011AllChargedLeg1.push_back(new isodeposit::ConeVeto(reco::isodeposit::Direction(leg1->eta(),leg1->phi()),0.0001));
    vetos2011AllChargedLeg1.push_back(new isodeposit::ThresholdVeto(0.0));
    vetos2011NeutralLeg1.push_back(new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.01));
    vetos2011NeutralLeg1.push_back(new isodeposit::ThresholdVeto(0.5));
    vetos2011PhotonLeg1.push_back( new isodeposit::ConeVeto(isodeposit::Direction(leg1->eta(),leg1->phi()),0.01));
    vetos2011PhotonLeg1.push_back( new isodeposit::ThresholdVeto(0.5));
       
    chIsoLeg1v2_   = 
      leg1->isoDeposit(pat::PfChargedHadronIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg1).first;
    nhIsoLeg1v2_ = 
      leg1->isoDeposit(pat::PfNeutralHadronIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg1).first;
    phIsoLeg1v2_ = 
      leg1->isoDeposit(pat::PfGammaIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg1).first;
    // all charged particles
    elecIsoLeg1v2_ =
      leg1->isoDeposit(pat::User1Iso)->depositAndCountWithin(0.4,vetos2011AllChargedLeg1).first;
    muIsoLeg1v2_   = elecIsoLeg1v2_; 
      //leg1->isoDeposit(pat::User2Iso)->depositAndCountWithin(0.4,vetos2011ChargedLeg1).first;
    chIsoPULeg1v2_ = 
      leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011ChargedLeg1).first;
    nhIsoPULeg1v2_ = 
      leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011NeutralLeg1).first;
    phIsoPULeg1v2_ = 
      leg1->isoDeposit(pat::PfAllParticleIso)->depositAndCountWithin(0.4,vetos2011PhotonLeg1).first;
   
    
    chIsoLeg2_ = leg2->isolationPFChargedHadrCandsPtSum();
    //
    if(verbose_){
      cout << "Tau z position " << (leg2->vertex()).z() << endl;
      for(unsigned int i = 0; i < (leg2->isolationPFChargedHadrCands()).size(); i++){
	if( (((leg2->isolationPFChargedHadrCands()).at(i))->trackRef()).isNonnull() )
	  cout << "Ch # " << i << " has z position " << (((leg2->isolationPFChargedHadrCands()).at(i))->trackRef()->referencePoint()).z() << " and pt " << ((leg2->isolationPFChargedHadrCands()).at(i))->pt() << endl;
      }
    }
    //
    nhIsoLeg2_ = 0.;
    for(unsigned int i = 0; i < (leg2->isolationPFNeutrHadrCands()).size(); i++){
      nhIsoLeg2_ += (leg2->isolationPFNeutrHadrCands()).at(i)->pt();
    }
    phIsoLeg2_ = leg2->isolationPFGammaCandsEtSum();
    
    // cleaning
    for(unsigned int i = 0; i <vetos2011ChargedLeg1.size(); i++){
      delete vetos2011ChargedLeg1[i];
      delete vetos2011AllChargedLeg1[i];
    }
    for(unsigned int i = 0; i <vetos2011NeutralLeg1.size(); i++){
      delete vetos2011NeutralLeg1[i];
      delete vetos2011PhotonLeg1[i];
    }
    //

    diTauVisP4_->push_back( theDiTau->p4Vis() );
    diTauCAP4_->push_back(  theDiTau->p4CollinearApprox() );
    diTauICAP4_->push_back( theDiTau->p4ImprovedCollinearApprox() );
    
    validityCA_   = int(theDiTau->collinearApproxIsValid());
    validityICA_  = int(theDiTau->ImprovedCollinearApproxIsValid());
    x1CollApprox_ = theDiTau->x1CollinearApprox();
    x2CollApprox_ = theDiTau->x2CollinearApprox();

    if(verbose_){
      if(theDiTau->collinearApproxIsValid() && theDiTau->ImprovedCollinearApproxIsValid())
	cout << "Found CA and ICA valid solutions" << endl;
      else if(!theDiTau->collinearApproxIsValid() && theDiTau->ImprovedCollinearApproxIsValid())
	cout << "ICA rescued the event!!!" << endl;
      else if(theDiTau->collinearApproxIsValid() && !theDiTau->ImprovedCollinearApproxIsValid())
	cout << "Strange!!! x1=" << theDiTau->x1CollinearApprox() << ", x2=" << theDiTau->x2CollinearApprox() << endl;
      if((theDiTau->p4CollinearApprox()).M()>0 && (theDiTau->p4ImprovedCollinearApprox()).M()<=0)
	cout << "Watch out! ICA gave mass=0 and CA mass !=0 ..." << endl;
    }
    
    math::XYZTLorentzVectorD nSVfitFitP4(0,0,0,(theDiTau->p4Vis()).M() );
    if( theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)!=0 /*&& theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->isValidSolution()*/ ){
      if(verbose_) cout << "Visible mass ==> " << nSVfitFitP4.E() << endl;
      nSVfitFitP4.SetPxPyPzE( 0,0,0, theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->mass() ) ;
    if(verbose_) cout << "SVFit fit solution ==> " << nSVfitFitP4.E() << endl;
    }
    diTauSVfitP4_->push_back( nSVfitFitP4  );
  
    int errFlag = 0;
    diTauSVfitMassErrUp_    = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",&errFlag)!=0 /*&& theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution()*/ ) 
      ? theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->massErrUp()   : -99; 
    diTauSVfitMassErrDown_  = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",&errFlag)!=0 /*&& theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution()*/ ) 
      ? theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->massErrDown() : -99; 
    
    diTauNSVfitMass_        = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) 
      ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->mass()        : -99; 
    diTauNSVfitMassErrUp_   = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) 
      ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->massErrUp()   : -99; 
    diTauNSVfitMassErrDown_ = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->isValidSolution() ) 
      ? theDiTau->nSVfitSolution("psKine_MEt_logM_int",0)->massErrDown() : -99; 
    
    

    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedJets;
    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedJetsIDL1Offset;
    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedJetsID;
    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedJetsIDUp;
    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedJetsIDDown;
    std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more> sortedGenJetsID;
    std::map<double, std::pair<float,float> ,  MuTauStreamAnalyzer::more> bTaggers;
    std::map<double, double                 ,  MuTauStreamAnalyzer::more> newBTagger;
    std::map<double, double                 ,  MuTauStreamAnalyzer::more> bQuark;
    std::map<double, std::pair<float,float> ,  MuTauStreamAnalyzer::more> jetPVassociation;
    std::map<double, std::pair<float,float> ,  MuTauStreamAnalyzer::more> jetMoments;
    std::map<double, std::vector<float> ,      MuTauStreamAnalyzer::more> jetPUID;


    for(unsigned int it = 0; it < jets->size() ; it++){
      
      pat::Jet* jet = const_cast<pat::Jet*>(&(*jets)[it]);
      
      // newJet redone using possibly different JEC ==> it may not contain infos on bTag
      // so I use it together with jet

      pat::Jet* newJet = 0;
      if(newJets!=0) newJetMatched( jet , newJets );
      if(!newJet){
	if(verbose_) cout << "No jet from the new collection can be matched ==> using old one..." << endl;
	newJet = jet;
      }
      
      math::XYZTLorentzVectorD leg2p4 = ( (leg2->pfJetRef()).isNonnull() ) ? leg2->pfJetRef()->p4() : leg2->p4();
      
      if( Geom::deltaR(jet->p4(), leg1->p4())<deltaRLegJet_ || 
	  Geom::deltaR(jet->p4(), leg2p4 )<deltaRLegJet_ ){
	if(verbose_) cout << "The jet at (" <<jet->pt()<<","<<jet->eta()<<") is closer than "<<deltaRLegJet_ << " from one of the legs" << endl;  

	if( Geom::deltaR(jet->p4(), leg2p4 )<deltaRLegJet_ ){
	  parton_ = jet->partonFlavour();
	  if(jet->genJet()!=0){
	    genPartMult_    = (jet->genJet()->getGenConstituents()).size();
	    leadGenPartPdg_ = (jet->genJet()->getGenConstituents()).size()>0 ?
	      ((jet->genJet()->getGenConstituents())[0])->pdgId() : -99;
	    leadGenPartPt_  = (jet->genJet()->getGenConstituents()).size()>0 ?
	      ((jet->genJet()->getGenConstituents())[0])->pt() : -99;

	    if(verbose_){
	      cout << "parton flavor = " << jet->partonFlavour() << endl;
	      for(unsigned int l = 0; l < (jet->genJet()->getGenConstituents()).size() ; l++){
		cout << "Hadron " << l 
		     << " pdg " << ((jet->genJet()->getGenConstituents())[l])->pdgId() << ", pt " << ((jet->genJet()->getGenConstituents())[l])->pt()
		     << endl;
	      }
	    }
	  }
	}


	if(isMC_) 
	  leptonJets_->push_back( newJet->correctedJet("Uncorrected").p4() );
	else
	  leptonJets_->push_back( newJet->correctedJet("Uncorrected").p4() );
	
	continue;
      }
      
      /////////////////////////////////////////////////////////////////////////
      //// use JES uncertainties
      edm::ESHandle<JetCorrectorParametersCollection> jetCorrParameters;
      // get the jet corrector parameters collection from the global tag
      iSetup.get<JetCorrectionsRecord>().get("AK5PF", jetCorrParameters);
      // get the uncertainty parameters from the collection
      JetCorrectorParameters const & param = (*jetCorrParameters)["Uncertainty"];
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(param);
      deltaJEC->setJetEta(newJet->eta()); deltaJEC->setJetPt(newJet->pt());
      float shift  = deltaJEC->getUncertainty( true );
      delete deltaJEC;
      /////////////////////////////////////////////////////////////////////////
      
      if(verbose_){
	cout << "Analyzing jet with pt " << (newJet->p4()).Pt() 
	   << "and eta " << (newJet->p4()).Eta() << endl;
	cout << " ==> JEC uncertainty is " << shift*100 << " %" << endl;
	for(unsigned int i = 0; i < (newJet->availableJECSets()).size() ; i++ ){
	  std::cout << (newJet->availableJECSets())[i] << std::endl;
	}
	for(unsigned int i = 0; i < (newJet->availableJECLevels()).size() ; i++ ){
	  std::cout << (newJet->availableJECLevels())[i] << std::endl;
	}
	std::cout << "L1FastJet ========> " << std::endl;
	std::cout << "Uncorrected " << newJet->correctedJet("Uncorrected","none","patJetCorrFactors").pt() << std::endl;
	std::cout << "L1FastJet "   << newJet->correctedJet("L1FastJet","none",  "patJetCorrFactors").pt() << std::endl;
	std::cout << "L2Relative "  << newJet->correctedJet("L2Relative","none", "patJetCorrFactors").pt() << std::endl; 
	std::cout << "L3Absolute "  << newJet->correctedJet("L3Absolute","none", "patJetCorrFactors").pt() << std::endl; 
	std::cout << "L1Offset ========> " << std::endl;
	std::cout << "Uncorrected " << newJet->jecFactor("Uncorrected","none","patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	std::cout << "L1Offset"     << newJet->jecFactor("L1Offset","none",   "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	std::cout << "L2Relative "  << newJet->jecFactor("L2Relative","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	std::cout << "L3Absolute"   << newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;  
      }
      
      std::map<string,float> aMap;
      
      if( jetID( jet , &((*vertexes)[0]), vtxZ, aMap ) < minJetID_ ){
	if(verbose_){
	  cout << "@@@@ Jet does not pass the Id" << endl;
	}
	continue;
      }
      
      sortedJets.insert( make_pair( newJet->correctedJet("Uncorrected").p4().Pt() ,newJet->correctedJet("Uncorrected").p4() ) );
      
      if(newJet->p4().Pt() < minCorrPt_) continue;

      // add b-tag info
      bTaggers.insert(         make_pair( newJet->p4().Pt(), make_pair( jet->bDiscriminator("trackCountingHighEffBJetTags"),
									jet->bDiscriminator("trackCountingHighPurBJetTags")  ) ) );
      // add new b-tag info
      newBTagger.insert(       make_pair( newJet->p4().Pt(), jet->bDiscriminator("combinedSecondaryVertexBJetTags") ) );

      // is jet matched to a b-quark?
      bQuark.insert(       make_pair( newJet->p4().Pt(), (jet->genParticleById(5,0,true)).isNonnull()  ) );
      
      // add pu information
      jetPVassociation.insert( make_pair( newJet->p4().Pt(), make_pair(aMap["chFracRawJetE"],
								       aMap["chFracAllChargE"]) ) );
      // add jet moments
      jetMoments.insert(       make_pair( newJet->p4().Pt(), make_pair( jet->etaetaMoment(),
									jet->phiphiMoment()) ) );
      
      if(isMC_) 
	sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
						newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->p4()) );   
      else 
	sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
						newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->p4()) ); 

      if(verbose_) cout << "Components: "
			<< "px=" << (newJet->p4()).Px() << " (" << newJet->px() << "), "
			<< "py=" << (newJet->p4()).Py() << " (" << newJet->py() << "), "
			<< "pz=" << (newJet->p4()).Pz() << " (" << newJet->pz() << "), "
			<< "E="  << (newJet->p4()).E()  << " (" << newJet->energy()  << ")"
			<< endl;
      
      sortedJetsID.insert(     make_pair( newJet->p4().Pt() ,  newJet->p4() ) );
      sortedJetsIDUp.insert(   make_pair( newJet->p4().Pt() ,  newJet->p4()*(1+shift) ) );
      sortedJetsIDDown.insert( make_pair( newJet->p4().Pt() ,  newJet->p4()*(1-shift) ) );
      
      if(isMC_){
	if(jet->genJet() != 0) sortedGenJetsID.insert( make_pair( newJet->p4().Pt() ,jet->genJet()->p4() ) );
	else sortedGenJetsID.insert( make_pair( newJet->p4().Pt() , math::XYZTLorentzVectorD(0,0,0,0) ) );
      }

      /////////////////////////////////////////////////////////////////////
      
      float mva   = (*puJetIdMVA)[ jetsHandleForMVA->refAt(it)->originalObjectRef()];
      int  idflag = (*puJetIdFlag)[jetsHandleForMVA->refAt(it)->originalObjectRef()];
      if( verbose_ ){
	cout << "jet " << it << " pt " << jet->pt() << " eta " << jet->eta() << " PU JetID MVA " << mva << endl; 
	cout << "X-check " << " pt " << jetsHandleForMVA->refAt(it)->pt() << " eta " << jetsHandleForMVA->refAt(it)->eta(); 
	if( PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kLoose ) ) {
	  cout << " pass loose wp";
	}
	if( PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kMedium ) ){
	  cout << " pass medium wp";
	}
	if( PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kTight ) ){
	  cout << " pass tight wp";
	}
	cout << endl;
      }

      vector<float> pileupResults;
      pileupResults.push_back(mva);
      pileupResults.push_back(float(PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kLoose )));
      pileupResults.push_back(float(PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kMedium )));
      pileupResults.push_back(float(PileupJetIdentifier::passJetId( idflag, PileupJetIdentifier::kTight )));

      jetPUID.insert(  make_pair( newJet->p4().Pt() ,  pileupResults ) );
      
      /////////////////////////////////////////////////////////////////////    
    }
    
    for(CImap it = sortedJets.begin(); it != sortedJets.end() ; it++){
      jetsP4_->push_back( it->second );
    }
    for(CImap it = sortedJetsID.begin(); it != sortedJetsID.end() ; it++){
      jetsIDP4_->push_back( it->second );
    }
    for(CImap it = sortedJetsIDUp.begin(); it != sortedJetsIDUp.end() ; it++){
      jetsIDUpP4_->push_back( it->second );
    }
    for(CImap it = sortedJetsIDDown.begin(); it != sortedJetsIDDown.end() ; it++){
      jetsIDDownP4_->push_back( it->second );
    }
    for(CImap it = sortedJetsIDL1Offset.begin(); it != sortedJetsIDL1Offset.end() ; it++){
      jetsIDL1OffsetP4_->push_back( it->second );
    }
    for(CImap it = sortedGenJetsID.begin(); it != sortedGenJetsID.end() ; it++){
      genJetsIDP4_->push_back( it->second );
    }
    for(std::map<double, std::pair<float,float> >::iterator it = bTaggers.begin(); it != bTaggers.end() ; it++){
      jetsBtagHE_->push_back( (it->second).first  );
      jetsBtagHP_->push_back( (it->second).second );
    }
    for(std::map<double, double >::iterator it = bQuark.begin(); it != bQuark.end() ; it++){
      bQuark_->push_back( it->second  );
    }
    for(std::map<double, double >::iterator it = newBTagger.begin(); it != newBTagger.end() ; it++){
      jetsBtagCSV_->push_back( it->second  );
    }
    for(std::map<double, std::pair<float,float> >::iterator it = jetPVassociation.begin(); it != jetPVassociation.end() ; it++){
      jetsChEfraction_->push_back( (it->second).first  );
      jetsChNfraction_->push_back( (it->second).second );
    }
    for(std::map<double, std::pair<float,float> >::iterator it = jetMoments.begin(); it != jetMoments.end() ; it++){
      jetMoments_->push_back( (it->second).first  );
      jetMoments_->push_back( (it->second).second );
    }
    for(std::map<double, std::vector<float> >::iterator it = jetPUID.begin(); it != jetPUID.end() ; it++){
      jetPUMVA_->push_back( (it->second)[0]  );
      jetPUWP_->push_back( (it->second)[1] );
      jetPUWP_->push_back( (it->second)[2] );
      jetPUWP_->push_back( (it->second)[3] );
    }
    
    tree_->Fill();
    
  }
  
}





unsigned int  MuTauStreamAnalyzer::jetID( const pat::Jet* jet, const reco::Vertex* vtx, std::vector<float> vtxZ, std::map<std::string,float>& map_){

  if( (jet->pt())<10 ) return 99; // always pass jet ID

  std::vector<reco::PFCandidatePtr> pfCandPtrs = jet->getPFConstituents();

  int nCharged = 0;
  int nPhotons = 0;
  int nNeutral = 0;
  int nConst = 0;

  float energyCharged = 0;
  float energyPhotons = 0;
  float energyNeutral = 0;
  float energyElectrons = 0;
 
  float totalEnergyFromConst = 0;

  float chEnFractionPV   = 0;
  float chEnFractionChPV = 0;
  float chFractionPV     = 0;

  for(unsigned i=0; i<pfCandPtrs.size(); ++i) {
    const reco::PFCandidate& cand = *(pfCandPtrs[i]);

    totalEnergyFromConst +=  cand.energy();
    nConst += 1;

    switch( cand.particleId() ) {
    case reco::PFCandidate::h: 
      nCharged++;
      energyCharged += cand.energy(); 

      if((cand.trackRef()).isNonnull()){
	bool isMatched = false;
	for(reco::Vertex::trackRef_iterator it = vtx->tracks_begin() ; it!=vtx->tracks_end() && !isMatched; it++){
	  if(  (*it).id() == (cand.trackRef()).id() && (*it).key() == (cand.trackRef()).key() ){
	    isMatched = true;
	    if(verbose_) cout << (*it).id() << ", " << (*it).key() << " is matched!" << endl;
	    chEnFractionPV += cand.energy();
	    chFractionPV+=1;
	  }
	}
	if(!isMatched){
	  float dist = vtxZ.size()>0 ? fabs(vtxZ[0]-((cand.trackRef())->referencePoint()).z()) : 999.;
	  float tmpDist = 999.;
	  for(unsigned k = 1; k < vtxZ.size(); k++){
	    if( fabs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	      tmpDist = fabs(((cand.trackRef())->referencePoint()).z() - vtxZ[k] );
	  }
	  if(tmpDist>dist){
	    isMatched = true;
	    if(verbose_) cout << "Matched by closest vtx in z!!" << endl;
	    chEnFractionPV += cand.energy();
	    chFractionPV+=1;
	  }
	}
	if(!isMatched && verbose_) {
	  cout << "Ch with pt " << cand.pt() << " and eta " << cand.eta() << " is not matched to the PV !!!" << endl;
	  cout << "z position of PV " << (vtx->position()).z() << ", z position of track " << ((cand.trackRef())->referencePoint()).z()  << ", x position of track " << ((cand.trackRef())->referencePoint()).x()   << ", y position of track " << ((cand.trackRef())->referencePoint()).y() << endl;
	}
      }

      break;
    case reco::PFCandidate::gamma:
      nPhotons++;
      energyPhotons += cand.energy();
      break;
    case reco::PFCandidate::h0:
      nNeutral++;
      energyNeutral += cand.energy();
      break;
    case reco::PFCandidate::e: 
      energyElectrons += cand.energy();

      if((cand.gsfTrackRef()).isNonnull()){
	bool isMatched = false;
	float dist = vtxZ.size()>0 ? fabs(vtxZ[0]-((cand.gsfTrackRef())->referencePoint()).z()) : 999.;
	float tmpDist = 999.;
	for(unsigned k = 1; k < vtxZ.size(); k++){
	  if( fabs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] ) < tmpDist )
	    tmpDist = fabs(((cand.gsfTrackRef())->referencePoint()).z() - vtxZ[k] );
	}
	if(tmpDist>dist){
	  isMatched = true;
	  if(verbose_) cout << "Matched by closest vtx in z!!" << endl;
	  chEnFractionPV += cand.energy();
	  chFractionPV+=1;
	}
	if(!isMatched && verbose_) {
	  cout << "Ele with pt " << cand.pt() << " and eta " << cand.eta() << " is not matched to the PV !!!" << endl;
	  cout << "z position of PV " << (vtx->position()).z() << ", z position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).z()  << ", x position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).x()   << ", y position of gsfTrack " << ((cand.gsfTrackRef())->referencePoint()).y() << endl;
	}
      }
      
      break;
    case reco::PFCandidate::h_HF: // fill neutral
      nNeutral++;
      //energyNeutral += cand.energy();
      break;
    case reco::PFCandidate::egamma_HF: // fill e/gamma
      nPhotons++;
      energyPhotons += cand.energy();
      break;
    default:
      break;
    }
  }

  chEnFractionChPV = chEnFractionPV;
  if(energyCharged>0 || energyElectrons>0)
    chEnFractionChPV /= (energyCharged+energyElectrons); 
  chEnFractionPV /= jet->correctedJet("Uncorrected").p4().E();
  chFractionPV   /= nCharged;

  //if(chFractionPV==0) cout << energyCharged << endl;

  map_["chMult"]          =  chFractionPV;
  map_["chFracRawJetE"]   =  chEnFractionPV;
  map_["chFracAllChargE"] =  chEnFractionChPV;
        
  bool loose=false;
  bool medium=false;
  bool tight=false;


  if(verbose_){
    cout << "NeutrFRAC = " << energyNeutral/totalEnergyFromConst << endl;
    cout << "PhotoFRAC = " << energyPhotons/totalEnergyFromConst << endl;
    cout << "ChargFRAC = " << energyCharged/totalEnergyFromConst << endl;
    cout << "nConst = "    << nConst << endl;
    cout << "nCharged = "  << nCharged << endl;
    cout << "ElectFRAC ="  << energyElectrons/totalEnergyFromConst << endl;
  }


  //loose id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.99 && 
       energyPhotons/totalEnergyFromConst<0.99 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<0.99 && 
       energyPhotons/totalEnergyFromConst<0.99 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<0.99
       )
      ) loose = true;
  // medium id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.95 && 
       energyPhotons/totalEnergyFromConst<0.95 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<1 && 
       energyPhotons/totalEnergyFromConst<1 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<1
       )
      ) medium = true;
  // tight id
  if( (TMath::Abs(jet->eta())>2.4 && 
       energyNeutral/totalEnergyFromConst<0.90 && 
       energyPhotons/totalEnergyFromConst<0.90 &&
       nConst > 1) || 
      (TMath::Abs(jet->eta())<2.4 && 
       energyNeutral/totalEnergyFromConst<1 && 
       energyPhotons/totalEnergyFromConst<1 &&
       nConst > 1 &&
       energyCharged/totalEnergyFromConst>0 &&
       nCharged>0 &&
       energyElectrons/totalEnergyFromConst<1
       )
      ) tight = true;
  
  if(loose && !medium && !tight) return 1;
  if(loose && medium && !tight)  return 2;
  if(loose && medium && tight)   return 3; 
  
  return 0;

}

pat::Jet* MuTauStreamAnalyzer::newJetMatched( const pat::Jet* oldJet , const pat::JetCollection* newJets ){

  pat::Jet* matchedJet = 0;

  for(unsigned int it = 0; it < newJets->size() ; it++){
    if( Geom::deltaR( (*newJets)[it].p4() , oldJet->p4() )<0.01 ){
      matchedJet = const_cast<pat::Jet*>(&((*newJets)[it]));
      break;
    }
  }

  return matchedJet;

}



void MuTauStreamAnalyzer::computeDCASig(double &iDCA3D    ,double &iDCA3DE    ,double &iDCA2D    ,double &iDCA2DE,
					double &iDCARPhi3D,double &iDCARPhi3DE,double &iDCARPhi2D,double &iDCARPhi2DE,
					const reco::Track *iTrack1,const reco::Track *iTrack2){
  
  iDCA3D      = -1.0;
  iDCA3DE     = -1.0;
  iDCA2D      = -1.0;
  iDCA2DE     = -1.0;
  iDCARPhi3D  = -1.0;
  iDCARPhi3DE = -1.0;
  iDCARPhi2D  = -1.0;
  iDCARPhi2DE = -1.0;
  
  if (iTrack1==NULL) return;
  if (iTrack2==NULL) return;

  float pion_mass    = 0.139657;
  float pion_sigma   = pion_mass*1.e-6;  
  float muon_mass    = 0.105658;
  float muon_sigma   = muon_mass*1.e-6;
  float mass1        = muon_mass;
  float mass2        = pion_mass;
  float mass_sigma1  = muon_sigma;
  float mass_sigma2  = pion_sigma;

  reco::TransientTrack transientTrack1;
  reco::TransientTrack transientTrack2;
  
  transientTrack1 = transientTrackBuilder_->build(*iTrack1);
  transientTrack2 = transientTrackBuilder_->build(*iTrack2);
  reco::TransientTrack * trk1Ptr = & transientTrack1;
  reco::TransientTrack * trk2Ptr = & transientTrack2;

  FreeTrajectoryState track1State = trk1Ptr->impactPointTSCP().theState();
  FreeTrajectoryState track2State = trk2Ptr->impactPointTSCP().theState();
  
  if (trk1Ptr->impactPointTSCP().isValid() &&  trk2Ptr->impactPointTSCP().isValid()) {
    
    ClosestApproachInRPhi cApp;
    TwoTrackMinimumDistance minDist;
    
    typedef ROOT::Math::SVector<double, 3> SVector3;
    typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;   
    cApp.calculate(track1State,track2State);
    minDist.calculate(track1State,track2State);
    if (minDist.status()) {
      
      minDist.distance();
      std::pair<GlobalPoint,GlobalPoint> pcaLeptons = minDist.points();
      GlobalPoint track1PCA = pcaLeptons.first;
      GlobalPoint track2PCA = pcaLeptons.second;
      
      //Creating a KinematicParticleFactory
      KinematicParticleFactoryFromTransientTrack pFactory;
      
      //initial chi2 and ndf before kinematic fits.
      float chi = 0.;
      float ndf = 0.;
      RefCountedKinematicParticle track1Part = pFactory.particle(transientTrack1,mass1,chi,ndf,mass_sigma1); 
      RefCountedKinematicParticle track2Part = pFactory.particle(transientTrack2,mass2,chi,ndf,mass_sigma2); 

      SVector3 distanceVector(track1PCA.x()-track2PCA.x(),
			      track1PCA.y()-track2PCA.y(),
			      track1PCA.z()-track2PCA.z());
      
      iDCA3D = ROOT::Math::Mag(distanceVector);
      
      std::vector<float> vvv(6);
      
      vvv[0] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,1);   
      vvv[2] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,2);   
      vvv[5] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(2,2);
      
      SMatrixSym3D track1PCACov(vvv.begin(),vvv.end());
      
      vvv[0] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,1);   
      vvv[2] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,2);   
      vvv[5] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(2,2);
      
      SMatrixSym3D track2PCACov(vvv.begin(),vvv.end());
      
      SMatrixSym3D totCov = track1PCACov + track2PCACov;
      
      if(iDCA3D != 0) iDCA3DE   = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCA3D;
      if(iDCA3D == 0) iDCA3DE   = 0.;

      distanceVector(2) = 0.0;
      iDCA2D    = ROOT::Math::Mag(distanceVector);
      if(iDCA2D != 0) iDCA2DE   = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCA2D;
      if(iDCA2D == 0) iDCA2DE   = 0.;
    }
    if (cApp.status()) {
      
      cApp.distance();
      std::pair<GlobalPoint,GlobalPoint> pcaLeptons = cApp.points();
      GlobalPoint track1PCA = pcaLeptons.first;
      GlobalPoint track2PCA = pcaLeptons.second;

      //Creating a KinematicParticleFactory
      KinematicParticleFactoryFromTransientTrack pFactory;
      
      //initial chi2 and ndf before kinematic fits.
      float chi = 0.;
      float ndf = 0.;
      RefCountedKinematicParticle track1Part = pFactory.particle(transientTrack1,mass1,chi,ndf,mass_sigma1); 
      RefCountedKinematicParticle track2Part = pFactory.particle(transientTrack2,mass2,chi,ndf,mass_sigma2); 
 
      SVector3 distanceVector(track1PCA.x()-track2PCA.x(),
			      track1PCA.y()-track2PCA.y(),
			      track1PCA.z()-track2PCA.z());
      iDCARPhi3D = ROOT::Math::Mag(distanceVector);
      
      std::vector<float> vvv(6);
      
      vvv[0] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,1);	   
      vvv[2] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(1,2);	   
      vvv[5] = track1Part->stateAtPoint(track1PCA).kinematicParametersError().matrix()(2,2);
      
      SMatrixSym3D track1PCACov(vvv.begin(),vvv.end());

      vvv[0] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,0);
      vvv[1] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,1);	   
      vvv[2] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,1);
      vvv[3] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(0,2);
      vvv[4] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(1,2);	   
      vvv[5] = track2Part->stateAtPoint(track2PCA).kinematicParametersError().matrix()(2,2);
      
      SMatrixSym3D track2PCACov(vvv.begin(),vvv.end());

      SMatrixSym3D totCov = track1PCACov + track2PCACov;
      
      if(iDCARPhi3D != 0) iDCARPhi3DE = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCARPhi3D;
      if(iDCARPhi3D == 0) iDCARPhi3DE = 0.;

      distanceVector(2) = 0.0;
      iDCARPhi2D  = ROOT::Math::Mag(distanceVector);
      if(iDCARPhi2D != 0) iDCARPhi2DE = sqrt(ROOT::Math::Similarity(totCov, distanceVector))/iDCARPhi2D;
      if(iDCARPhi2D == 0) iDCARPhi2DE = 0.;
    }
  }
 
}


void MuTauStreamAnalyzer::endJob(){}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(MuTauStreamAnalyzer);


