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

#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include <iostream>
#include <fstream>


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
//#include "RecoJets/JetProducers/interface/PileupJetIdentifier.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h"

///METCovMatrix/////
//#include "AnalysisDataFormats/TauAnalysis/interface/PFMEtSignCovMatrix.h"
#include "DataFormats/METReco/interface/PFMEtSignCovMatrix.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETFwd.h"
//Standalone SVFit///
// #include "TauAnalysis/CandidateTools/interface/NSVfitStandaloneAlgorithm.h"//old SVfitMass
#include "TauAnalysis/SVfitStandalone/interface/SVfitStandaloneAlgorithm.h"

#include <vector>
#include <utility>
#include <map>
#include "TMatrixTBase.h"

// #ifdef __MAKECINT__
// #pragma link C++ class map<string,int>;
// #pragma link C++ class map<string,int>::iterator;
// #pragma link C++ class pair<string,int>;
// #endif

using namespace std;
using namespace reco;

namespace
{
  typedef std::map<double, math::XYZTLorentzVectorD ,MuTauStreamAnalyzer::more>::iterator CImap;

  TFile* openFile(const edm::FileInPath& inputFileName)
  {
    if ( !inputFileName.isLocal() ) throw cms::Exception("MuTauStreamAnalyzer") 
      << " Failed to find File = " << inputFileName << " !!\n";
    TFile* inputFile = new TFile(inputFileName.fullPath().data());
    return inputFile;
  }

  TH1* loadLUT(TFile* inputFile, const std::string& lutName)
  {
    TH1* lut = dynamic_cast<TH1*>(inputFile->Get(lutName.data()));
    if ( !lut ) 
      throw cms::Exception("TauTauNtupleProducer") 
	<< " Failed to load LUT = " << lutName.data() << " from file = " << inputFile->GetName() << " !!\n";
    return lut;
  }

  double compHiggsPtWeight(const TH1* lut, double higgsPt)
  {
    int idxBin = (const_cast<TH1*>(lut))->FindBin(higgsPt);
    if ( idxBin < 1                ) idxBin = 1;
    if ( idxBin > lut->GetNbinsX() ) idxBin = lut->GetNbinsX();
    return lut->GetBinContent(idxBin);
  }

//   bool findGenParticle(const reco::GenParticleCollection& genParticles, int absPdgId1, int absPdgId2, int absPdgId3, reco::Candidate::LorentzVector& p4)
//   {
//     for ( reco::GenParticleCollection::const_iterator genParticle = genParticles.begin();
// 	  genParticle != genParticles.end(); ++genParticle ) {
//       int absGenParticlePdgId = TMath::Abs(genParticle->pdgId());
//       if ( (absPdgId1 != 0 && absGenParticlePdgId == absPdgId1) ||
// 	   (absPdgId2 != 0 && absGenParticlePdgId == absPdgId2) ||
// 	   (absPdgId3 != 0 && absGenParticlePdgId == absPdgId3) ) {
// 	p4 = genParticle->p4();
// 	return true;
//       }
//     }
//     return false;
//   }
  bool findGenParticle(const reco::GenParticleCollection& genParticles, int pdgId1, int pdgId2, int pdgId3, reco::Candidate::LorentzVector& p4) 
  { 
    for ( reco::GenParticleCollection::const_iterator genParticle = genParticles.begin(); genParticle != genParticles.end(); ++genParticle ) { 
      int genParticlePdgId = genParticle->pdgId(); 
      if ( (pdgId1 != 0 && genParticlePdgId == pdgId1) || (pdgId2 != 0 && genParticlePdgId == pdgId2) || (pdgId3 != 0 && genParticlePdgId == pdgId3) ) { 
	p4 = genParticle->p4(); 
	return true; 
      } 
    } 
    return false; 
  }

  struct DiTauInfo 
  { 
    DiTauInfo(){}; 
    int diTauCharge_; 
    double sumPt_; 
    int index_; 
  }; 

  struct SortDiTauPairs 
  { 
    bool operator() (const DiTauInfo t1, const DiTauInfo t2) 
    { 
      // 1st criterion: OS 
      if ( t1.diTauCharge_ < t2.diTauCharge_ ) return true; 
      if ( t1.diTauCharge_ > t2.diTauCharge_ ) return false; 
      // 2nd criterion: sumPt of diTau pair 
      return (t1.sumPt_ > t2.sumPt_);  
    } 
  }; 

  //IN
  //-----------------------------------------------------------------------------
  // CV: recipe for top quark Pt reweighting taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
  double compTopPtWeight(double topPt)
  {
    const double a = 0.156;
    const double b = -0.00137;
    return TMath::Exp(a + b*topPt);
  }
  
  double compTopPtWeight(double top1Pt, double top2Pt)
  {
    double topPtWeight2 = compTopPtWeight(top1Pt)*compTopPtWeight(top2Pt);
    return ( topPtWeight2 > 0. ) ? TMath::Sqrt(topPtWeight2) : 0.;
  }
  //-----------------------------------------------------------------------------

  void matchGenParticle(const reco::Candidate::LorentzVector& p4, const std::vector<reco::GenParticle>& genParticles,
			bool& isGenHadTau, bool& isGenMuon, bool& isGenElectron, bool& isGenJet)
  {
    bool matchesGenHadTau   = false;
    bool matchesGenMuon     = false;
    bool matchesGenElectron = false;

    //loop on genParticles
    for ( std::vector<reco::GenParticle>::const_iterator genParticle = genParticles.begin(); genParticle != genParticles.end(); ++genParticle )
      {
	int pdgId = TMath::Abs(genParticle->pdgId());
	//reject genParticle if not tau, muon or electron
	if ( !(pdgId == 11 || pdgId == 13 || pdgId == 15) ) continue;
	reco::Candidate::LorentzVector visMomentum = getVisMomentum(&(*genParticle));  
	//speed the loop --> visible transverse momentum should not be greater than true pt
	if ( !(visMomentum.pt() > (0.5*p4.pt())) ) continue;

	//matching in DR < 0.5
	if ( deltaR(p4, visMomentum) < 0.5 )
	  {
	    //tau decays --> hadronic or electron or muon
	    if ( pdgId == 15 )
	      {
		std::string genTauDecayMode = getGenTauDecayMode(&(*genParticle));
		if ( genTauDecayMode == "electron" )
		  {
		    matchesGenElectron = true;
		  }
		else if ( genTauDecayMode == "muon" )
		  {
		    matchesGenMuon = true;
		  }
		else {
		  matchesGenHadTau = true;
		}
	      }
	    //muon
	    else if ( pdgId == 13 )
	      {
		matchesGenMuon = true;
	      }
	    //electron
	    else if ( pdgId == 11 )
	      {
		matchesGenElectron = true;
	      } 
	  }
      }

    if      ( matchesGenHadTau   ) isGenHadTau   = true;
    else if ( matchesGenMuon     ) isGenMuon     = true;
    else if ( matchesGenElectron ) isGenElectron = true;
    else                           isGenJet      = true;
  }

  void matchGenParticleFromZdecay(const reco::Candidate::LorentzVector& p4, const reco::GenParticle* genLeptonPlus, const reco::GenParticle* genLeptonMinus,
				bool& isGenHadTau, bool& isGenMuon, bool& isGenElectron, bool& isGenJet)
  {
    std::vector<int> pdgIds_ranked;
    pdgIds_ranked.push_back(15);
    pdgIds_ranked.push_back(13);
    pdgIds_ranked.push_back(11);

    cout<<"genLeptonPlus->eta() = "<<genLeptonPlus->eta()<<endl;
    cout<<"genLeptonMinus->eta() = "<<genLeptonMinus->eta()<<endl;
    cout<<"candidate p4.Eta() = "<<p4.Eta()<<endl;

    for ( std::vector<int>::const_iterator pdgId = pdgIds_ranked.begin(); pdgId != pdgIds_ranked.end(); ++pdgId )
      {
	bool matchesGenLeptonPlus  = (genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == (*pdgId) && deltaR(p4, getVisMomentum(genLeptonPlus))  < 0.5);
	bool matchesGenLeptonMinus = (genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == (*pdgId) && deltaR(p4, getVisMomentum(genLeptonMinus)) < 0.5);
	if ( matchesGenLeptonPlus || matchesGenLeptonMinus )
	  {
	    if ( (*pdgId) == 15 ) 
	      {
		std::string genTauDecayModePlus  = ( matchesGenLeptonPlus )  ? getGenTauDecayMode(genLeptonPlus)  : "undefined";
		std::string genTauDecayModeMinus = ( matchesGenLeptonMinus ) ? getGenTauDecayMode(genLeptonMinus) : "undefined";
		if ( (matchesGenLeptonPlus  && genTauDecayModePlus  == "electron") || 
		     (matchesGenLeptonMinus && genTauDecayModeMinus == "electron") ) {
		  isGenElectron = true;
		} 
		if ( (matchesGenLeptonPlus  && genTauDecayModePlus  == "muon") || 
		     (matchesGenLeptonMinus && genTauDecayModeMinus == "muon") ) {
		  isGenMuon = true;
		} 
		if ( (matchesGenLeptonPlus  && !(genTauDecayModePlus  == "electron" || genTauDecayModePlus  == "muon")) || 
		     (matchesGenLeptonMinus && !(genTauDecayModeMinus == "electron" || genTauDecayModeMinus == "muon")) ) {
		  isGenHadTau = true;
		} 
	      }
	    else if ( (*pdgId) == 13 )
	      {
		isGenMuon = true;
	      }
	    else if ( (*pdgId) == 11 )
	      {
		isGenElectron = true;
	      }
	    else assert(0);
	    return;
	  }
      }
    // CV: reconstructed tau neither matches generator level electron, muon nor tau
    isGenJet = true;
  }
}


MuTauStreamAnalyzer::MuTauStreamAnalyzer(const edm::ParameterSet & iConfig)
  : inputFileHiggsPtWeight_(0),
    lutHiggsPtWeightNom_(0),
    lutHiggsPtWeightUp_(0),
    lutHiggsPtWeightDown_(0),
    loosePFJetIdAlgo_(0)
{

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
  isPFEmb_           = iConfig.getParameter<bool>("isPFEmb");
  isRhEmb_           = iConfig.getUntrackedParameter<bool>("isRhEmb",false);
  genParticlesTag_   = iConfig.getParameter<edm::InputTag>("genParticles"); 
  genParticlesForTopPtReweightingTag_ = iConfig.getParameter<edm::InputTag>("genParticlesForTopPtReweighting"); 
  genTausTag_        = iConfig.getParameter<edm::InputTag>("genTaus"); 
  deltaRLegJet_      = iConfig.getUntrackedParameter<double>("deltaRLegJet",0.3);
  minCorrPt_         = iConfig.getUntrackedParameter<double>("minCorrPt",10.);
  minJetID_          = iConfig.getUntrackedParameter<double>("minJetID",0.5);
  verbose_           = iConfig.getUntrackedParameter<bool>("verbose",false);
  doIsoOrdering_     = iConfig.getUntrackedParameter<bool>("doIsoOrdering", false);
  pileupSrc_         = iConfig.getParameter<edm::InputTag>("pileupSrc");

  //#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
  edm::ParameterSet cfgPFJetIdAlgo;
  cfgPFJetIdAlgo.addParameter<std::string>("version", "FIRSTDATA");
  cfgPFJetIdAlgo.addParameter<std::string>("quality", "LOOSE");
  loosePFJetIdAlgo_ = new PFJetIDSelectionFunctor(cfgPFJetIdAlgo);

//   edm::ParameterSet evtWeights = iConfig.getParameter<edm::ParameterSet>("evtWeights");
//   typedef std::vector<std::string> vstring;
//   vstring evtWeightNames = evtWeights.getParameterNamesForType<edm::InputTag>();
//   for ( vstring::const_iterator name = evtWeightNames.begin();
// 	name != evtWeightNames.end(); ++name ) {    
//     edm::InputTag src = evtWeights.getParameter<edm::InputTag>(*name);
//     evtWeightsToStore_.push_back(InputTagEntryType(*name, src));
//   }


}

void MuTauStreamAnalyzer::beginJob(){

//   myfile.open ("ForAbdollah_MuTau.txt");

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
  jetQuarkGluon_    = new std::vector< float >();   // Quark/gluon id
  jetQuarkGluonGen_ = new std::vector< float >();   // Quark/gluon id

  gammadR_       = new std::vector< float >();
  gammadEta_     = new std::vector< float >();
  gammadPhi_     = new std::vector< float >();
  gammaPt_       = new std::vector< float >();
  metSgnMatrix_  = new std::vector< float >();

//   tauXTriggers_= new std::vector< int >();
//   triggerBits_ = new std::vector< int >();
  triggerPaths_ = new std::map<string, int>();
  HLTfiltersMu_ =  new std::map<string, int>();
  HLTfiltersTau_ =  new std::map<string, int>();
  HLTfiltersTauJet_ =  new std::map<string, int>();

  jetsP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDP4_        = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDUpP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDDownP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  jetsIDL1OffsetP4_= new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genJetsIDP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  //new Olivier from PFJetIDSelectionFunctor
  //jetsP4_PFJetIDSelectionFunctor_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauVisP4_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauCAP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauICAP4_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauSVfitP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  diTauLegsP4_    = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  diTauLegsAltP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genDiTauLegsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genTausP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genTausCharge_  = new std::vector< int >();
  METP4_          = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  caloMETNoHFP4_  = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genMETP4_       = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  genVP4_         = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  gentopP4_       = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  genTausDecayLeptonically_ = new std::vector<int>() ;

  leptonJets_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  extraMuons_   = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  pfMuons_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoMuonsP4_     = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoElectronsP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoTausP4_      = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
  vetoElectronsID_    = new std::vector< int >();

  l1ETMP4_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
//   trgTaus_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();
//   trgTauId_= new std::vector< int >();
  l1Muons_ = new std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >();

  embeddingWeights_ = new std::vector< double >();

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
  tree_->Branch("jetQuarkGluon","std::vector<float> ", &jetQuarkGluon_);   // Quark/gluon id
  tree_->Branch("jetQuarkGluonGen","std::vector<float> ", &jetQuarkGluonGen_);   // Quark/gluon id

  tree_->Branch("gammadR","std::vector<float> ",&gammadR_);
  tree_->Branch("gammadEta","std::vector<float> ",&gammadEta_);
  tree_->Branch("gammadPhi","std::vector<float> ",&gammadPhi_);
  tree_->Branch("gammaPt","std::vector<float> ",&gammaPt_);

  tree_->Branch("metSgnMatrix","std::vector<float>",&metSgnMatrix_);

  tree_->Branch("jetsChEfraction","std::vector<float>",&jetsChEfraction_);
  tree_->Branch("jetsChNfraction","std::vector<float>",&jetsChNfraction_);

//   tree_->Branch("tauXTriggers","std::vector<int>",&tauXTriggers_);
//   tree_->Branch("triggerBits","std::vector<int>",&triggerBits_);
  tree_->Branch("triggerPaths","std::map<string,int>",&triggerPaths_);
  tree_->Branch("HLTfiltersMu","std::map<string,int>",&HLTfiltersMu_);
  tree_->Branch("HLTfiltersTau","std::map<string,int>",&HLTfiltersTau_);
  tree_->Branch("HLTfiltersTauJet","std::map<string,int>",&HLTfiltersTauJet_);

  tree_->Branch("diTauVisP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",  &diTauVisP4_);
  tree_->Branch("diTauCAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauCAP4_);
  tree_->Branch("diTauICAP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",   &diTauICAP4_);
  tree_->Branch("diTauSVfitP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauSVfitP4_);

  tree_->Branch("validityCA", &validityCA_,"validityCA/I");
  tree_->Branch("validityICA",&validityICA_,"validityICA/I");
 
  tree_->Branch("x1CollApprox",&x1CollApprox_,"x1CollApprox/F");
  tree_->Branch("x2CollApprox",&x2CollApprox_,"x2CollApprox/F");

  tree_->Branch("diTauNSVfitIsValid",    &diTauNSVfitIsValid_,"diTauNSVfitIsValid/I");
  tree_->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,"diTauNSVfitMass/F");
  tree_->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,"diTauNSVfitMassErrUp/F");
  tree_->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  tree_->Branch("diTauNSVfitPt",         &diTauNSVfitPt_,"diTauNSVfitPt/F");
  tree_->Branch("diTauNSVfitPtErrUp",    &diTauNSVfitPtErrUp_,"diTauNSVfitPtErrUp/F");
  tree_->Branch("diTauNSVfitPtErrDown",  &diTauNSVfitPtErrDown_,"diTauNSVfitPtErrDown/F");
  tree_->Branch("diTauSVfitMassErrUp",   &diTauSVfitMassErrUp_,"diTauSVfitMassErrUp/F");
  tree_->Branch("diTauSVfitMassErrDown", &diTauSVfitMassErrDown_,"diTauSVfitMassErrDown/F");

  tree_->Branch("diTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauLegsP4_);
  tree_->Branch("diTauLegsAltP4_","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&diTauLegsAltP4_);
  tree_->Branch("genDiTauLegsP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genDiTauLegsP4_);
  tree_->Branch("genTausP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genTausP4_);
  tree_->Branch("genTausCharge","std::vector< int >",&genTausCharge_);
  tree_->Branch("genTausDecayLeptonically","std::vector< int >",&genTausDecayLeptonically_);

  tree_->Branch("METP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&METP4_);
  tree_->Branch("caloMETNoHFP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&caloMETNoHFP4_);
  tree_->Branch("genMETP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genMETP4_);
  tree_->Branch("genVP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&genVP4_);
  tree_->Branch("gentopP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&gentopP4_);
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
  tree_->Branch("vetoElectronsID","std::vector< int >",&vetoElectronsID_);

  tree_->Branch("l1ETMP4","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&l1ETMP4_);
//   tree_->Branch("trgTaus","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&trgTaus_);
//   tree_->Branch("trgTauId","std::vector<int>",&trgTauId_);
  tree_->Branch("l1Muons","std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > >",&l1Muons_);

  tree_->Branch("sumEt",&sumEt_,"sumEt/F");
  tree_->Branch("caloNoHFsumEt",&caloNoHFsumEt_,"caloNoHFsumEt/F");
  tree_->Branch("caloNoHFsumEtCorr",&caloNoHFsumEtCorr_,"caloNoHFsumEtCorr/F");
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
  tree_->Branch("pfJetEtaMom2",&pfJetEtaMom2_,"pfJetEtaMom2/F");
  tree_->Branch("pfJetPhiMom2",&pfJetPhiMom2_,"pfJetPhiMom2/F");

  tree_->Branch("run",&run_,"run/l");
  tree_->Branch("event",&event_,"event/l");
  tree_->Branch("lumi",&lumi_,"lumi/l");
  tree_->Branch("numPV",&numPV_,"numPV/F");
  tree_->Branch("VtxZ",&VtxZ_,"vtxZ/F");
  tree_->Branch("VtxX",&VtxX_,"vtxX/F");
  tree_->Branch("VtxY",&VtxY_,"vtxY/F");
  tree_->Branch("numOfDiTaus",&numOfDiTaus_,"numOfDiTaus/I");
  tree_->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");
  tree_->Branch("decayMode",&decayMode_,"decayMode/I");
  tree_->Branch("genDecayMode",&genDecayMode_,"genDecayMode/I");
  tree_->Branch("genPolarization",&genPolarization_,"genPolarization/I");
 
  //TauID
  tree_->Branch("decayModeFinding",&decayModeFinding_,"decayModeFinding/I");
  tree_->Branch("decayModeFindingNewDM",&decayModeFindingNewDM_,"decayModeFindingNewDM/I");
  tree_->Branch("decayModeFindingOldDM",&decayModeFindingOldDM_,"decayModeFindingOldDM/I");
  tree_->Branch("AntiEDeadEcal",&AntiEDeadEcal_,"AntiEDeadEcal/I");
  tree_->Branch("tightestAntiECutWP",&tightestAntiECutWP_,"tightestAntiECutWP/I");
  tree_->Branch("tightestAntiEMVA5WP",&tightestAntiEMVA5WP_,"tightestAntiEMVA5WP/I");
  tree_->Branch("AntiEMVA5category",&AntiEMVA5category_,"AntiEMVA5category/I");
  tree_->Branch("AntiEMVA5raw",&AntiEMVA5raw_,"AntiEMVA5raw/F");
  tree_->Branch("tightestAntiMuWP",&tightestAntiMuWP_,"tightestAntiMuWP/I");
  tree_->Branch("tightestAntiMu2WP",&tightestAntiMu2WP_,"tightestAntiMu2WP/I");
  tree_->Branch("tightestAntiMu3WP",&tightestAntiMu3WP_,"tightestAntiMu3WP/I");
  tree_->Branch("tightestAntiMuMVAWP",&tightestAntiMuMVAWP_,"tightestAntiMuMVAWP/I");
  tree_->Branch("AntiMuMVAraw",&AntiMuMVAraw_,"AntiMuMVAraw/F");
  tree_->Branch("tightestHPSDBWP",&tightestHPSDBWP_,"tightestHPSDBWP/I");
  tree_->Branch("tightestHPSDB3HWP",&tightestHPSDB3HWP_,"tightestHPSDB3HWP/I");
  tree_->Branch("hpsDB3H",&hpsDB3H_,"hpsDB3H/F");
  tree_->Branch("tightestHPSMVA3newDMwLTWP",&tightestHPSMVA3newDMwLTWP_,"tightestHPSMVA3newDMwLTWP/I");
  tree_->Branch("hpsMVA3newDMwLT",&hpsMVA3newDMwLT_,"hpsMVA3newDMwLT/F");
  tree_->Branch("tightestHPSMVA3newDMwoLTWP",&tightestHPSMVA3newDMwoLTWP_,"tightestHPSMVA3newDMwoLTWP/I");
  tree_->Branch("hpsMVA3newDMwoLT",&hpsMVA3newDMwoLT_,"hpsMVA3newDMwoLT/F");
  tree_->Branch("tightestHPSMVA3oldDMwLTWP",&tightestHPSMVA3oldDMwLTWP_,"tightestHPSMVA3oldDMwLTWP/I");
  tree_->Branch("hpsMVA3oldDMwLT",&hpsMVA3oldDMwLT_,"hpsMVA3oldDMwLT/F");
  tree_->Branch("tightestHPSMVA3oldDMwoLTWP",&tightestHPSMVA3oldDMwoLTWP_,"tightestHPSMVA3oldDMwoLTWP/I");
  tree_->Branch("hpsMVA3oldDMwoLT",&hpsMVA3oldDMwoLT_,"hpsMVA3oldDMwoLT/F");
  tree_->Branch("visibleTauMass",&visibleTauMass_,"visibleTauMass/F");
  tree_->Branch("visibleGenTauMass",&visibleGenTauMass_,"visibleGenTauMass/F");

  ///NewTauID input variables
  tree_->Branch("chargedIsoPtSum",&chargedIsoPtSum_,"chargedIsoPtSum/F");
  tree_->Branch("neutralIsoPtSum",&neutralIsoPtSum_,"neutralIsoPtSum/F");
  tree_->Branch("puCorrPtSum",&puCorrPtSum_,"puCorrPtSum/F");
  tree_->Branch("patDecayMode",&patDecayMode_,"patDecayMode/I");
  tree_->Branch("dxyTau",&dxyTau_,"dxyTau/F");
  tree_->Branch("dxyErrTau",&dxyErrTau_,"dxyErrTau/F");
  tree_->Branch("dxySigTau",&dxySigTau_,"dxySigTau/F");
  tree_->Branch("hasSecVtx",&hasSecVtx_,"hasSecVtx/I");
  tree_->Branch("flightLength",&flightLength_,"flightLength/F");
  tree_->Branch("flightLengthError",&flightLengthError_,"flightLengthError/F");
  tree_->Branch("flightLength2",&flightLength2_,"flightLength2/F");
  tree_->Branch("flightLengthSig",&flightLengthSig_,"flightLengthSig/F");

  ///NewTauID useful variables
  tree_->Branch("dxyPCAX",&dxyPCAX_,"dxyPCAX/F");
  tree_->Branch("dxyPCAY",&dxyPCAY_,"dxyPCAY/F");
  tree_->Branch("dxyPCAZ",&dxyPCAZ_,"dxyPCAZ/F");
  tree_->Branch("flightLengthX",&flightLengthX_,"flightLengthX/F");
  tree_->Branch("flightLengthY",&flightLengthY_,"flightLengthY/F");
  tree_->Branch("flightLengthZ",&flightLengthZ_,"flightLengthZ/F");
  tree_->Branch("pvX",&pvX_,"pvX/F");
  tree_->Branch("pvY",&pvY_,"pvY/F");
  tree_->Branch("pvZ",&pvZ_,"pvZ/F");
  tree_->Branch("pvCov00",&pvCov00_,"pvCov00/F");
  tree_->Branch("pvCov01",&pvCov01_,"pvCov01/F");
  tree_->Branch("pvCov02",&pvCov02_,"pvCov02/F");
  tree_->Branch("pvCov11",&pvCov11_,"pvCov11/F");
  tree_->Branch("pvCov12",&pvCov12_,"pvCov12/F");
  tree_->Branch("pvCov22",&pvCov22_,"pvCov22/F");
  tree_->Branch("svX",&svX_,"svX/F");
  tree_->Branch("svY",&svY_,"svY/F");
  tree_->Branch("svZ",&svZ_,"svZ/F");
  tree_->Branch("svCov00",&svCov00_,"svCov00/F");
  tree_->Branch("svCov01",&svCov01_,"svCov01/F");
  tree_->Branch("svCov02",&svCov02_,"svCov02/F");
  tree_->Branch("svCov11",&svCov11_,"svCov11/F");
  tree_->Branch("svCov12",&svCov12_,"svCov12/F");
  tree_->Branch("svCov22",&svCov22_,"svCov22/F");

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

  //new branches for truth matching (= booleans)
  tree_->Branch("isZdecay",&isZdecay_,"isZdecay/O");

  tree_->Branch("isZthth",&isZthth_,"isZthth/O");
  tree_->Branch("isZtt",&isZtt_,"isZtt/O");
  tree_->Branch("isZttl",&isZttl_,"isZttl/O");
  tree_->Branch("isZttj",&isZttj_,"isZttj/O");
  tree_->Branch("isZj",&isZj_,"isZj/O");
  tree_->Branch("isZee",&isZee_,"isZee/O");
  tree_->Branch("isZmm",&isZmm_,"isZmm/O");
  tree_->Branch("isZll",&isZll_,"isZll/O");

  tree_->Branch("isLeg1MatchedToHadTau",&isLeg1MatchedToHadTau_,"isLeg1MatchedToHadTau/O");
  tree_->Branch("isLeg1MatchedToMuon",&isLeg1MatchedToMuon_,"isLeg1MatchedToMuon/O");
  tree_->Branch("isLeg1MatchedToElectron",&isLeg1MatchedToElectron_,"isLeg1MatchedToElectron/O");
  tree_->Branch("isLeg1MatchedToLepton",&isLeg1MatchedToLepton_,"isLeg1MatchedToLepton/O");
  tree_->Branch("isLeg1MatchedToJet",&isLeg1MatchedToJet_,"isLeg1MatchedToJet/O");

  tree_->Branch("isLeg2MatchedToHadTau",&isLeg2MatchedToHadTau_,"isLeg2MatchedToHadTau/O");
  tree_->Branch("isLeg2MatchedToMuon",&isLeg2MatchedToMuon_,"isLeg2MatchedToMuon/O");
  tree_->Branch("isLeg2MatchedToElectron",&isLeg2MatchedToElectron_,"isLeg2MatchedToElectron/O");
  tree_->Branch("isLeg2MatchedToLepton",&isLeg2MatchedToLepton_,"isLeg2MatchedToLepton/O");
  tree_->Branch("isLeg2MatchedToJet",&isLeg2MatchedToJet_,"isLeg2MatchedToJet/O");

  tree_->Branch("isTauLegMatched",&isTauLegMatched_,"isTauLegMatched/I");
  tree_->Branch("isTauLegMatchedToLep",&isTauLegMatchedToLep_,"isTauLegMatchedToLep/I");
  tree_->Branch("isMuLegMatched",&isMuLegMatched_,"isMuLegMatched/I");
  tree_->Branch("muFlag",&muFlag_,"muFlag/I");
  tree_->Branch("muFlagSoft",&muFlagSoft_,"muFlagSoft/I");
  tree_->Branch("vetoEvent",&vetoEvent_,"vetoEvent/I");
  tree_->Branch("isPFMuon",&isPFMuon_,"isPFMuon/I");
  tree_->Branch("isTightMuon",&isTightMuon_,"isTightMuon/I");
  tree_->Branch("muVetoRelIso",&muVetoRelIso_,"muVetoRelIso/F");
  tree_->Branch("muVetoRelIsoSoft",&muVetoRelIsoSoft_,"muVetoRelIsoSoft/F");
  tree_->Branch("hasKft",&hasKft_,"hasKft/I");

  tree_->Branch("diTauCharge",&diTauCharge_,"diTauCharge/F");
  tree_->Branch("chargeL1",&chargeL1_,"chargeL1/F");
  tree_->Branch("rhoFastJet",&rhoFastJet_,"rhoFastJet/F");
  tree_->Branch("rhoNeutralFastJet",&rhoNeutralFastJet_,"rhoNeutralFastJet/F");
  tree_->Branch("mcPUweight",&mcPUweight_,"mcPUweight/F");
  tree_->Branch("nPUVertices",&nPUVertices_,"nPUVertices/F");
  tree_->Branch("nPUVerticesM1",&nPUVerticesM1_,"nPUVerticesM1/F");
  tree_->Branch("nPUVerticesP1",&nPUVerticesP1_,"nPUVerticesP1/F");

  tree_->Branch("embeddingWeight",&embeddingWeight_,"embeddingWeight/F");//FilterEfficiency()
  tree_->Branch("embeddingWeights",    "std::vector<double>",&embeddingWeights_);//All other embedding weights
  tree_->Branch("TauSpinnerWeight",&TauSpinnerWeight_,"TauSpinnerWeight/F");
  tree_->Branch("nPUtruth",  &nPUtruth_,  "nPUtruth/F");

  //Higgs pT reweighting SM
//   tree_->Branch("higgsPtWeightNom",&higgsPtWeightNom_,"higgsPtWeightNom/F");
//   tree_->Branch("higgsPtWeightUp",&higgsPtWeightUp_,"higgsPtWeightUp/F");
//   tree_->Branch("higgsPtWeightDown",&higgsPtWeightDown_,"higgsPtWeightDown/F");

  //Top pT reweighting SM // IN
  tree_->Branch("topPtWeightNom",&topPtWeightNom_,"topPtWeightNom/F");
  tree_->Branch("topPtWeightUp",&topPtWeightUp_,"topPtWeightUp/F");
  tree_->Branch("topPtWeightDown",&topPtWeightDown_,"topPtWeightDown/F");

  tree_->Branch("index",&index_,"index/I");

  tree_->Branch("genDiTauMass",&genDiTauMass_,"genDiTauMass/F");
}


MuTauStreamAnalyzer::~MuTauStreamAnalyzer(){
  delete inputFileHiggsPtWeight_;
  delete jetsP4_; delete jetsIDP4_; delete jetsIDUpP4_; delete jetsIDDownP4_;
  delete METP4_; delete caloMETNoHFP4_; delete diTauVisP4_; delete diTauCAP4_; delete diTauICAP4_; 
  delete diTauSVfitP4_; delete genVP4_; delete gentopP4_;
  delete diTauLegsP4_; delete jetsBtagHE_; delete jetsBtagHP_; delete jetsBtagCSV_;
  delete bQuark_; delete diTauLegsAltP4_;
  delete triggerPaths_; delete HLTfiltersMu_; delete HLTfiltersTau_; delete HLTfiltersTauJet_; delete sigDCA_;
//   delete tauXTriggers_; delete triggerBits_; delete sigDCA_;
  delete genJetsIDP4_; delete genDiTauLegsP4_; delete genMETP4_; delete extraMuons_; 
  delete vetoMuonsP4_; delete vetoElectronsP4_; delete vetoTausP4_;
  delete vetoElectronsID_;
  delete l1ETMP4_;//  delete trgTaus_; delete trgTauId_;
  delete l1Muons_;
  delete pfMuons_; delete jetsIDL1OffsetP4_;
  delete leptonJets_;
  delete genTausP4_;
  delete genTausCharge_;
  delete genTausDecayLeptonically_;
  delete jetsChNfraction_; delete jetsChEfraction_;delete jetMoments_;
  delete jetPUMVA_; delete jetPUWP_;
  delete gammadR_ ; delete gammadPhi_; delete gammadEta_; delete gammaPt_;
  delete metSgnMatrix_;
  delete embeddingWeights_;
  delete jetQuarkGluon_; delete jetQuarkGluonGen_;    // Quark/gluon id

}

void MuTauStreamAnalyzer::analyze(const edm::Event & iEvent, const edm::EventSetup & iSetup){

  //METP4_->clear();
  genVP4_->clear();
  gentopP4_->clear();
  caloMETNoHFP4_->clear();
  genMETP4_->clear();
  genTausP4_->clear();
  genTausCharge_->clear();
  genTausDecayLeptonically_->clear();
  pfMuons_->clear();
  triggerPaths_->clear();
//   triggerBits_->clear();
  vetoTausP4_->clear();
  vetoElectronsP4_->clear();
  vetoElectronsID_->clear();
  vetoMuonsP4_->clear();
  l1Muons_->clear();
  l1ETMP4_->clear();
//   trgTaus_->clear();
//   trgTauId_->clear();


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
  VtxZ_ = vertexes->size()!=0 ? (*vertexes)[0].position().z() : -99;
  VtxX_ = vertexes->size()!=0 ? (*vertexes)[0].position().x() : -99;
  VtxY_ = vertexes->size()!=0 ? (*vertexes)[0].position().y() : -99;

//   const pat::METCollection* met = 0;
//   edm::Handle<pat::METCollection> metHandle;
//   iEvent.getByLabel( metTag_, metHandle);
//   if( !metHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//       << "No MET label available \n";
//   if(metHandle.isValid())met = metHandle.product();

  const pat::METCollection* rawMet = 0;
  edm::Handle<pat::METCollection> rawMetHandle;
  iEvent.getByLabel( rawMetTag_, rawMetHandle);
  if( !rawMetHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No raw MET label available \n";
  if(rawMetHandle.isValid() )rawMet = rawMetHandle.product();

//   const pat::METCollection* mvaMet = 0;
//   edm::Handle<pat::METCollection> mvaMetHandle;
//   iEvent.getByLabel( mvaMetTag_, mvaMetHandle);
//   if( !mvaMetHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//       << "No mva MET label available \n";
//   if(mvaMetHandle.isValid())mvaMet = mvaMetHandle.product();

//   edm::Handle<PFMEtSignCovMatrix> metCovHandle;
//   iEvent.getByLabel( metCovTag_, metCovHandle);
//   if( !metCovHandle.isValid() )
//     edm::LogError("DataNotAvailable")
//       << "No met Cov label available \n";

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

  edm::Handle<reco::GenParticleCollection> genHandleForTopPtReweighting;
  const reco::GenParticleCollection* genParticlesForTopPtReweighting = 0;
  iEvent.getByLabel(genParticlesForTopPtReweightingTag_,genHandleForTopPtReweighting); 

  if(genHandle.isValid()){
    cout<<"genHandle is valid"<<endl;

    genParticles = genHandle.product();
    int index1 = -99;
    int index2 = -99;
    for(unsigned int k = 0; k < genParticles->size(); k ++){
      if( fabs((*genParticles)[k].pdgId()) != 15 || (*genParticles)[k].status()!=2) continue;
      if(index1<0) 
	index1 = k;
      else 
	index2 = k;

      //cout<<"tau is found"<<endl;

    }
    if(index1>=0 && index2>=0){
      if((*genParticles)[index1].pt()<(*genParticles)[index2].pt()){
	int indexBkp = index1;
	index1 = index2;
	index2 = indexBkp;
      }
      genTausP4_->push_back((*genParticles)[index1].p4());
      genTausP4_->push_back((*genParticles)[index2].p4());

      int charge_index1 = 0;
      if((*genParticles)[index1].pdgId()==15) charge_index1 = -1;
      else if((*genParticles)[index1].pdgId()==-15) charge_index1 = +1;//tau+
      
      int charge_index2 = 0;
      if((*genParticles)[index2].pdgId()==15) charge_index2 = -1;//tau-
      else if((*genParticles)[index2].pdgId()==-15) charge_index2 = +1;//tau+    
      
      genTausCharge_->push_back(charge_index1);
      genTausCharge_->push_back(charge_index2);

      //checking daughters for index1 tau
//       cout<<"Gen tau k = "<<index1<<endl;
      int n = (*genParticles)[index1].numberOfDaughters();
//       cout<<" --> has "<<n<<" daughters"<<endl;
      int IsDecayingLeptonically = 0 ; 
      for(int j = 0; j < n; ++ j)
	{
	  const Candidate * d = (*genParticles)[index1].daughter(j);
	  int dauId = d->pdgId();
// 	  cout<<"     daughter #"<<j<<" = "<<dauId<<endl;
	  if(dauId==-13 || dauId==+13)
	    {
	      IsDecayingLeptonically = 1 ;
	      break;
	    }
	}
      genTausDecayLeptonically_->push_back(IsDecayingLeptonically);
//       if(IsDecayingLeptonically) cout<<" === decays leptonically"<<endl;
//       else cout<<" === decays hadronically"<<endl;

      //checking daughters for index2 tau
//       cout<<"Gen tau k = "<<index2<<endl;
      n = (*genParticles)[index2].numberOfDaughters();
//       cout<<" --> has "<<n<<" daughters"<<endl;
      IsDecayingLeptonically = 0 ; 
      for(int j = 0; j < n; ++ j)
	{
	  const Candidate * d = (*genParticles)[index2].daughter(j);
	  int dauId = d->pdgId();
// 	  cout<<"     daughter #"<<j<<" = "<<dauId<<endl;

	  if(dauId==-13 || dauId==+13)
	    {
	      IsDecayingLeptonically = 1 ;
	      break;
	    }
	}
      genTausDecayLeptonically_->push_back(IsDecayingLeptonically);
//       if(IsDecayingLeptonically) cout<<" === decays leptonically"<<endl;
//       else cout<<" === decays hadronically"<<endl;

    }

  }

// for(size_t i = 0; i < genParticles->size(); ++ i) {
//      const GenParticle & p = (*genParticles)[i];
//      int id = p.pdgId();
//      int st = p.status();  
//      const Candidate * mom = p.mother();
//      double pt = p.pt(), eta = p.eta(), phi = p.phi(), mass = p.mass();
//      double vx = p.vx(), vy = p.vy(), vz = p.vz();
//      int charge = p.charge();
//      int n = p.numberOfDaughters();
//      for(size_t j = 0; j < n; ++ j) {
//        const Candidate * d = p.daughter( j );
//        int dauId = d->pdgId();
//        // . . . 
//      }

  if(isMC_ || isPFEmb_){

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

      if(isMC_)
	{
	  if( !( (*genParticles)[k].pdgId() == 23 || 
		 abs((*genParticles)[k].pdgId()) == 24 || 
		 (*genParticles)[k].pdgId() == 25 ||
		 (*genParticles)[k].pdgId() == 35 ||
		 (*genParticles)[k].pdgId() == 36
		 ) || 
	      (*genParticles)[k].status()!=3) continue;
	}
      else if(isPFEmb_ && !isMC_)
	{
	  if( !( (*genParticles)[k].pdgId() == 23 || 
		 abs((*genParticles)[k].pdgId()) == 24 || 
		 (*genParticles)[k].pdgId() == 25 ||
		 (*genParticles)[k].pdgId() == 35 ||
		 (*genParticles)[k].pdgId() == 36
		 ) || 
	      (*genParticles)[k].status()!=2) continue;
	}

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

  //edm::InputTag pileupSrc_;
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
    //iEvent.getByType(puInfoH);
    iEvent.getByLabel(pileupSrc_, puInfoH);

    if(puInfoH.isValid()){
      for(std::vector<PileupSummaryInfo>::const_iterator it = puInfoH->begin(); it != puInfoH->end(); it++){

// 	cout << "Bunc crossing " << it->getBunchCrossing() << endl;
	if(it->getBunchCrossing() == 0 ) 
	  {
// 	    cout<<"it->getTrueNumInteractions() = "<<it->getTrueNumInteractions()<<endl;
	    //nPUVertices_  = it->getPU_NumInteractions();  /old ICHEP
	    nPUVertices_  = it->getTrueNumInteractions();
	  }
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
  else if(isPFEmb_ && !isMC_)
    {
      // tag gen jets
      iEvent.getByLabel(genTausTag_,tauGenJetsHandle);
      if( !tauGenJetsHandle.isValid() )  
	edm::LogError("DataNotAvailable")
	  << "No gen jet label available \n";
      tauGenJets = tauGenJetsHandle.product();
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
  /* MB needed?
  if(muonsRel->size()<1){
    cout << " No muonsRel !!! " << endl;
    return;
  } else if(muonsRel->size()>1 && verbose_){
    cout << "WARNING: "<< muonsRel->size() << "  muonsRel found in the event !!! We will select only one" << endl;
  }
  */
  

  // Loose leptons for veto //
  vetoEvent_ = 0;

  edm::Handle<pat::MuonCollection> muonsForVetoHandle;
  edm::Handle<pat::ElectronCollection> electronsForVetoHandle;
  edm::Handle<pat::TauCollection> tausForVetoHandle;

  iEvent.getByLabel( "muonsForVeto" ,     muonsForVetoHandle);
  iEvent.getByLabel( "electronsForVeto" , electronsForVetoHandle);
  iEvent.getByLabel( "tausForVeto" ,      tausForVetoHandle);

  std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > buffer;
  std::vector< int> bufferID;
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

	int passID =-1;
	bool passesMVAPOGNonTrigLoose =   
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.0 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<0.8 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.925) ||  
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.8 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<1.479 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.915) ||  
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=1.479 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.965) || 
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.0 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<0.8 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.905) ||  
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.8 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<1.479 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.955) ||  
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=1.479 && (*electronsForVeto)[l].userFloat("mvaPOGNonTrig")>0.975); 
	
	//Loose MVA ID: new e-Id, same FR as old one
	bool passesMVAPOGTrigNoIPLoose =   
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.0 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<0.8 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>-0.5375) ||  
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.8 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<1.479 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>-0.375) ||  
	  ((*electronsForVeto)[l].pt()<=20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=1.479 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>-0.025) ||
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.0 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<0.8 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>0.325) ||  
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=0.8 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())<1.479 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>0.775) ||  
	  ((*electronsForVeto)[l].pt()>20 && fabs((*electronsForVeto)[l].superClusterPosition().Eta())>=1.479 && (*electronsForVeto)[l].userFloat("mvaPOGTrigNoIP")>0.775); 
	
	if(passesMVAPOGNonTrigLoose && !passesMVAPOGTrigNoIPLoose)  passID= 0;
	if(!passesMVAPOGNonTrigLoose && passesMVAPOGTrigNoIPLoose)  passID= 1;
	if(passesMVAPOGNonTrigLoose && passesMVAPOGTrigNoIPLoose)   passID= 2;	
        bufferID.push_back(passID);
      }
    }
    for(unsigned int m = 0; m < buffer.size() ; m++){
      vetoElectronsP4_->push_back( buffer[m] );
      vetoElectronsID_->push_back( bufferID[m] );
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

  /// l+tau+ETM analysis stuff
  edm::Handle<l1extra::L1MuonParticleCollection> l1muonsHandle; ;
  const l1extra::L1MuonParticleCollection* l1mus = 0;
  edm::Handle<l1extra::L1EtMissParticleCollection> l1etmHandle;
  const l1extra::L1EtMissParticleCollection* l1etm = 0;
  edm::Handle<reco::CaloMETCollection> caloL1etmHandle;
  const reco::CaloMETCollection* caloL1etm = 0;
//   edm::Handle<pat::TauCollection> trgTausHandle;
//   const pat::TauCollection* trgTaus = 0;
  edm::Handle<reco::CaloMETCollection> caloMEtNoHFHandle;
  const reco::CaloMETCollection* caloMEtNoHF = 0;
  edm::Handle<reco::CaloMETCollection> caloMEtNoHFCorrHandle;
  const reco::CaloMETCollection* caloMEtNoHFCorr = 0;

  //diTau trig eff. stuff
  edm::Handle<l1extra::L1JetParticleCollection> l1jetsHandle;
  const l1extra::L1JetParticleCollection* l1jets = 0;
  edm::Handle<l1extra::L1JetParticleCollection> l1tausHandle;
  const l1extra::L1JetParticleCollection* l1taus = 0;

  if(!isRhEmb_)
    iEvent.getByLabel("metNoHF", caloMEtNoHFHandle);
  else
    iEvent.getByLabel("correctedCaloMEtNoHF", caloMEtNoHFHandle);
  if( !caloMEtNoHFHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No metNoHF collection available \n";
  else{
    caloMEtNoHF = caloMEtNoHFHandle.product();      
    caloMETNoHFP4_->push_back( (*caloMEtNoHF)[0].p4() );
    caloNoHFsumEt_ = (*caloMEtNoHF)[0].sumEt();
  }
  if(!isRhEmb_)
    iEvent.getByLabel("metNoHFresidualCorrected", caloMEtNoHFCorrHandle);
  else
    iEvent.getByLabel("metNoHF", caloMEtNoHFCorrHandle);
  if( !caloMEtNoHFCorrHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No metNoHFresidualCorrected collection available \n";
  else{
    caloMEtNoHFCorr = caloMEtNoHFCorrHandle.product();      
    caloMETNoHFP4_->push_back( (*caloMEtNoHFCorr)[0].p4() );
    caloNoHFsumEtCorr_ = (*caloMEtNoHFCorr)[0].sumEt();
  }
  if(isMC_){
    edm::Handle<reco::CaloMETCollection> caloMEtNoHFCorrUpHandle;
    const reco::CaloMETCollection* caloMEtNoHFCorrUp = 0;
    iEvent.getByLabel("metNoHFresidualCorrectedUp", caloMEtNoHFCorrUpHandle);
    if( !caloMEtNoHFCorrUpHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No metNoHFresidualCorrectedUp collection available \n";
    else{
      caloMEtNoHFCorrUp = caloMEtNoHFCorrUpHandle.product();      
      caloMETNoHFP4_->push_back( (*caloMEtNoHFCorrUp)[0].p4() );
    }
    edm::Handle<reco::CaloMETCollection> caloMEtNoHFCorrDownHandle;
    const reco::CaloMETCollection* caloMEtNoHFCorrDown = 0;
    iEvent.getByLabel("metNoHFresidualCorrectedDown", caloMEtNoHFCorrDownHandle);
    if( !caloMEtNoHFCorrDownHandle.isValid() )  
      edm::LogError("DataNotAvailable")
	<< "No metNoHFresidualCorrectedDown collection available \n";
    else{
      caloMEtNoHFCorrDown = caloMEtNoHFCorrDownHandle.product();      
      caloMETNoHFP4_->push_back( (*caloMEtNoHFCorrDown)[0].p4() );
    }
  }
  iEvent.getByLabel("l1extraParticles", l1muonsHandle);
  if( !l1muonsHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No L1Muon collection available \n";
  else{
    l1mus = l1muonsHandle.product();
    for(unsigned int i=0; i<l1mus->size(); ++i)
      l1Muons_->push_back( (*l1mus)[i].p4() );
  }
  if(!isRhEmb_)
    iEvent.getByLabel("uncorrectedL1ETM", caloL1etmHandle);
  else
    iEvent.getByLabel("correctedL1ETM", caloL1etmHandle);
  if( !caloL1etmHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No uncorrectedL1ETM collection available \n";
  else{
    caloL1etm = caloL1etmHandle.product();      
    l1ETMP4_->push_back( (*caloL1etm)[0].p4() );
  }
  iEvent.getByLabel(edm::InputTag("l1extraParticles","MET"), l1etmHandle);
  if( !l1etmHandle.isValid() )  
    edm::LogError("DataNotAvailable")
      << "No L1ETM collection available \n";
  else{
    l1etm = l1etmHandle.product();      
    for(unsigned int i=0; i<l1etm->size(); ++i){
      if( (*l1etm)[i].bx()==0){
	l1ETMP4_->push_back( (*l1etm)[i].p4() );
	break;
      }
    }
  }
  //HLT taus for soft analysis
//   iEvent.getByLabel("selectedHltPatTaus", trgTausHandle);
//   if( !trgTausHandle.isValid() )  
//     edm::LogError("DataNotAvailable")
//       << "No trgTau collection available \n";
//   else{
//     trgTaus = trgTausHandle.product();
//     for(unsigned int i=0; i<trgTaus->size(); ++i){
//       trgTaus_->push_back( (*trgTaus)[i].p4() );
//       int id=0;
//       if( (*trgTaus)[i].tauID("decayModeFinding")>0.5){
// 	id++;
// 	if( (*trgTaus)[i].tauID("byIsolation")>0.5){
// 	  id++;
// 	}
//       }
//       trgTauId_->push_back(id);
//     }
//   }

  iEvent.getByLabel(edm::InputTag("l1extraParticles","Central"), l1jetsHandle);
  if( !l1jetsHandle.isValid() )
    edm::LogError("DataNotAvailable")
      << "No L1CentralJets collection available \n";
  else
    l1jets = l1jetsHandle.product();
  iEvent.getByLabel(edm::InputTag("l1extraParticles","Tau"), l1tausHandle);
  if( !l1jetsHandle.isValid() )
    edm::LogError("DataNotAvailable")
      << "No L1TauJets collection available \n";
  else
    l1taus = l1tausHandle.product(); 

  /// Quark/gluon id stuff
  edm::Handle<edm::ValueMap<float> >  QGTagsHandleMLP;
  edm::Handle<edm::ValueMap<float> >  QGTagsHandleLikelihood;
  iEvent.getByLabel("QGTagger","qgMLP", QGTagsHandleMLP);
  iEvent.getByLabel("QGTagger","qgLikelihood", QGTagsHandleLikelihood);

  /////////////////////////

  const PATMuTauPair *theDiTau = 0;
  if(diTaus->size()<1){
    cout << " No diTau !!! " << endl;
    return;
  } else if(diTaus->size()>1 && verbose_){
    cout << "WARNING: "<< diTaus->size() << "  diTaus found in the event !!! We will select only one" << endl;
  }
  numOfDiTaus_ = diTaus->size();
  /*
  muFlag_       = 0;
  muVetoRelIso_ = 0.;
  muFlagSoft_       = 0;
  muVetoRelIsoSoft_ = 0.;

  //MB incorrect when muonsRel not really looser than diTau leg
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
  */

//   vector<string> triggerPaths;
//   vector<string> XtriggerPaths;
//   vector<string> HLTfiltersMu;
//   vector<string> HLTfiltersTau;
//   vector<string> HLTfiltersTauJet;

//   map<string,int> triggerPaths ;
  map<string,int> HLTfiltersMu ;
  map<string,int> HLTfiltersTau ;
  map<string,int> HLTfiltersTauJet ;
//   map<string,int> HLTfilters ;


  if(isMC_){

    //paths
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_Mu8_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v*"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v*"] = 0 ;

    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2"] = 0 ;
    (*triggerPaths_)["HLT_Mu8_v16"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v5"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_v15"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v13"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v5"] = 0 ;

    (*triggerPaths_)["HLT_Mu17_Mu8_v17"] = 0 ;    

    //mu filters
    HLTfiltersMu["hltL1sMu14erORMu16er"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltL3fL1sMu3L3Filtered8"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"] = 0 ;//Iso Mu24
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"] = 0 ;//Iso Mu24 eta2p1

    //tau filters
    HLTfiltersTau["hltOverlapFilterIsoMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] = 0 ;
    HLTfiltersTau["hltL2Tau25eta2p1"] = 0 ;
    HLTfiltersTau["hltL2TauIsoFilter"] = 0 ;

    //tau jet filters
    HLTfiltersTauJet["hlt1PFJet320"] = 0 ;

    /*
    // X-triggers 
    XtriggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*");//0
    XtriggerPaths.push_back("HLT_Mu8_v*");//1
    XtriggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v*");//2
    XtriggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v*");
    XtriggerPaths.push_back("HLT_PFJet320_v*");
    // for Summer12-53X
    //HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v1

    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2");//0
    triggerPaths.push_back("HLT_Mu8_v16");//1
    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v5");//2
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7");//3
    triggerPaths.push_back("HLT_IsoMu24_v15");//4
    triggerPaths.push_back("HLT_PFJet320_v5");//5
    
    
    HLTfiltersMu.push_back("hltL1sMu14erORMu16er");//0
    HLTfiltersMu.push_back("hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15");//1
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");//2
    HLTfiltersMu.push_back("hltL3fL1sMu3L3Filtered8");//3
    HLTfiltersMu.push_back("hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15");//4
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15"); //5
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"); //6
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"); //7 //the last muon filter
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"); //8 corresponds to HLT_IsoMu24_v15
    
                    
    //L1mu//8
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");//9
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"); //10
    HLTfiltersTau.push_back("hltL2Tau25eta2p1"); //11 //L2Tau
    HLTfiltersTau.push_back("hltL2TauIsoFilter"); //12 //IsoL2Tau
    //trgTau//13, not stored/used 
    //l1tau 44 or l1jet 64//13

    HLTfiltersTauJet.push_back("hlt1PFJet320"); //13
    */
  }
  else{
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v*"] = 0 ;
    (*triggerPaths_)["HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v*"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_v*"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v*"] = 0 ;

    (*triggerPaths_)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v5"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v6"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v3"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v4"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v5"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v6"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v7"] = 0 ;
    (*triggerPaths_)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v4"] = 0 ;
    (*triggerPaths_)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v5"] = 0 ;
    (*triggerPaths_)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v6"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v2"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v3"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v4"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v5"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v6"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v7"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1"] = 0 ;
    (*triggerPaths_)["HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v3"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v4"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v5"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v6"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu15_eta2p1_L1ETM20_v7"] = 0 ;

    (*triggerPaths_)["HLT_Mu17_Mu8_v16"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v17"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v18"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v19"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v20"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v21"] = 0 ;
    (*triggerPaths_)["HLT_Mu17_Mu8_v22"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v5"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v4"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v3"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v3"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v1"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v4"] = 0 ;

    (*triggerPaths_)["HLT_IsoMu24_v15"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_v16"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_v17"] = 0 ;

    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v11"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v12"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v13"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v14"] = 0 ;
    (*triggerPaths_)["HLT_IsoMu24_eta2p1_v15"] = 0 ;

    (*triggerPaths_)["HLT_PFJet320_v3"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v4"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v5"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v6"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v7"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v8"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v9"] = 0 ;
    (*triggerPaths_)["HLT_PFJet320_v10"] = 0 ;

    /*
    XtriggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v*");//0
    XtriggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v*");//1
    XtriggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v*");//2
    XtriggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v*");//3
    XtriggerPaths.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v*");//4
    XtriggerPaths.push_back("HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v*");//5
    XtriggerPaths.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v*");//6
    XtriggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v*");//7
    XtriggerPaths.push_back("HLT_PFJet320_v*");//8
    

    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4");//0
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v5");//1
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v6");//2
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2");//3
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v3");//4
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v4");//5
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v5");//6
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v6");//7
    triggerPaths.push_back("HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v7");//8

    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v4");//9
    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v5");//10
    triggerPaths.push_back("HLT_Mu18_eta2p1_LooseIsoPFTau20_v6");//11
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v2");//12
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v3");//13
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v4");//14
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v5");//15
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v6");//16
    triggerPaths.push_back("HLT_Mu17_eta2p1_LooseIsoPFTau20_v7");//17

    triggerPaths.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1");//18
    triggerPaths.push_back("HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1");//19
    triggerPaths.push_back("HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1");//20

    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v3");//21
    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v4");//22
    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v5");//23
    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v6");//24
    triggerPaths.push_back("HLT_IsoMu15_eta2p1_L1ETM20_v7");//25

    // Double Muon trigger paths for RH Embed
    triggerPaths.push_back("HLT_Mu17_Mu8_v16");//26
    triggerPaths.push_back("HLT_Mu17_Mu8_v17");//27
    triggerPaths.push_back("HLT_Mu17_Mu8_v18");//28
    triggerPaths.push_back("HLT_Mu17_Mu8_v19");//29
    triggerPaths.push_back("HLT_Mu17_Mu8_v20");//30
    triggerPaths.push_back("HLT_Mu17_Mu8_v21");//31
    triggerPaths.push_back("HLT_Mu17_Mu8_v22");//32
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7");// 33
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v5");//34
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v4");//35
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v3");//36
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v3");//37
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v1");//38
    triggerPaths.push_back("HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v4");//39
    triggerPaths.push_back("HLT_PFJet320_v3");//40
    triggerPaths.push_back("HLT_PFJet320_v4");//40
    triggerPaths.push_back("HLT_PFJet320_v5");//40
    triggerPaths.push_back("HLT_PFJet320_v6");//40
    triggerPaths.push_back("HLT_PFJet320_v7");//40
    triggerPaths.push_back("HLT_PFJet320_v8");//40
    triggerPaths.push_back("HLT_PFJet320_v9");//40
    triggerPaths.push_back("HLT_PFJet320_v10");//40
    */

    HLTfiltersMu["hltSingleMuIsoL1s14L3IsoFiltered15eta2p1"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu18LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltOverlapFilterMu18LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltOverlapFilterMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu7Eta2p1L1f0L2f7QL3f8QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sSingleMu7Eta2p1L1f0L2f7QL3f8QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltOverlapFilterSingleIsoMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltL3fL1sMu7Eta2p1L1f0L2f7QL3Filtered8Q"] = 0 ;
    HLTfiltersMu["hltOverlapFilterMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoFiltered10"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] = 0 ;
    HLTfiltersMu["hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"] = 0 ;
    HLTfiltersMu["hltL3crIsoL1sMu16L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"];
    HLTfiltersMu["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"] = 0 ;//Iso Mu24 eta2p1

    /*
    HLTfiltersMu.push_back("hltSingleMuIsoL1s14L3IsoFiltered15eta2p1");//0
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10");//1
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");//2
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");//3
    HLTfiltersMu.push_back("hltOverlapFilterMu18LooseIsoPFTau20");//4
    HLTfiltersMu.push_back("hltOverlapFilterMu17LooseIsoPFTau20");//5
    HLTfiltersMu.push_back("hltL3crIsoL1sMu7Eta2p1L1f0L2f7QL3f8QL3crIsoRhoFiltered0p15");//6
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu8LooseIsoPFTau20");//7
    HLTfiltersMu.push_back("hltL3crIsoL1sSingleMu7Eta2p1L1f0L2f7QL3f8QL3crIsoRhoFiltered0p15");//8
    HLTfiltersMu.push_back("hltOverlapFilterSingleIsoMu8LooseIsoPFTau20");//9
    HLTfiltersMu.push_back("hltL3fL1sMu7Eta2p1L1f0L2f7QL3Filtered8Q");//10
    HLTfiltersMu.push_back("hltOverlapFilterMu8LooseIsoPFTau20");//11
    HLTfiltersMu.push_back("hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoFiltered10");//mu+ETM20 (v3-v4)//12
    HLTfiltersMu.push_back("hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15");//mu+ETM20 (v5-v7)//13
    HLTfiltersMu.push_back("hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15");//14, Missing filter from IsoMu17+LooseTau20 path
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15");//15, muon filter for isoMu18+MediumIsoTau25 path  
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"); //16, muon filter for isoMu18+MediumIsoTau25 path
    HLTfiltersMu.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"); //17, muon filter for isoMu18+MediumIsoTau25 path
    HLTfiltersMu.push_back("hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"); //18 //the last muon filter
    */

    HLTfiltersTau["hltOverlapFilterIsoMu18LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterIsoMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterMu18LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterMu17LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterIsoMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterSingleIsoMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterMu8LooseIsoPFTau20"] = 0 ;
    HLTfiltersTau["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] = 0 ;
    HLTfiltersTau["hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"] = 0 ;
    HLTfiltersTau["hltL2Tau25eta2p1"] = 0 ;
    HLTfiltersTau["hltL2TauIsoFilter"] = 0 ;

    /*
    //L1mu//19 
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18LooseIsoPFTau20");//20
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu17LooseIsoPFTau20");//21
    HLTfiltersTau.push_back("hltOverlapFilterMu18LooseIsoPFTau20");//22
    HLTfiltersTau.push_back("hltOverlapFilterMu17LooseIsoPFTau20");//23
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu8LooseIsoPFTau20");//24
    HLTfiltersTau.push_back("hltOverlapFilterSingleIsoMu8LooseIsoPFTau20");//25
    HLTfiltersTau.push_back("hltOverlapFilterMu8LooseIsoPFTau20");//26
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"); //27, tau filter for isoMu18+MediumIsoTau25 path 
    HLTfiltersTau.push_back("hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"); //28, tau filter for isoMu18+MediumIsoTau25 path
    HLTfiltersTau.push_back("hltL2Tau25eta2p1"); //29 //L2Tau
    HLTfiltersTau.push_back("hltL2TauIsoFilter"); //30 //IsoL2Tau
    //trgTau//31, not stored/used
    //l1tau 44 or l1jet 64//31
    */

    HLTfiltersTauJet["hlt1PFJet320"] = 0 ;

    /*
    HLTfiltersTauJet.push_back("hlt1PFJet320");
    */
  }

  map<string,int>::iterator iter_trigger;

  for(iter_trigger = triggerPaths_->begin(); iter_trigger != triggerPaths_->end(); iter_trigger++)
    {
      if(!trigger)
	{
	  continue;
	  cout << "Invalid triggerEvent !" << endl;
	}

      const pat::TriggerPath *triggerPath =  trigger->path(iter_trigger->first);

//       cout<<"*************"<<endl;
//       cout<<  "Testing " << iter_trigger->first << endl;
//       if(triggerPath) cout << "Is there..." << endl;
//       if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
//       if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
//       cout<<"*************"<<endl;


//       TString local(iter_trigger->first);
//       if(local.Contains("HLT_Mu17_Mu8_v"))
// 	{
// 	  cout<<  "Testing " << iter_trigger->first << endl;
// 	  if(triggerPath) cout << "Is there..." << endl;
// 	  if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
// 	  if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
// 	}
      
      if(verbose_)
	{
	  cout<<  "Testing " << iter_trigger->first << endl;
	  if(triggerPath) cout << "Is there..." << endl;
	  if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
	  if(triggerPath && triggerPath->wasRun() && triggerPath->wasAccept()) cout << "Was accepted..." << endl;
	}

      if(triggerPath && triggerPath->wasRun() && 
	 triggerPath->wasAccept() && 
	 triggerPath->prescale()==1)
	{
// 	  cout<<"trigger matched Olivier!!!"<<endl;
// 	  cout<<"iter_trigger->first = "<<iter_trigger->first<<endl;
	  (*triggerPaths_)[iter_trigger->first] = 1;
// 	  cout<<"value = "<<(*triggerPaths_)[iter_trigger->first]<<endl;
	}
      else if (triggerPath && triggerPath->wasRun() && 
	       triggerPath->wasAccept() && 
	       triggerPath->prescale()!=1) (*triggerPaths_)[iter_trigger->first] = 2;
      else (*triggerPaths_)[iter_trigger->first] = 0;
    }

  /*
  for(unsigned int i=0;i<triggerPaths.size();i++){
    if(!trigger){
      continue;
      cout << "Invalid triggerEvent !" << endl;
    }
    const pat::TriggerPath *triggerPath =  trigger->path(triggerPaths[i]);

//     cout<<  "Testing " << triggerPaths[i] << endl;
//     if(triggerPath) cout << "Is there..." << endl;
//     if(triggerPath && triggerPath->wasRun()) cout << "Was run..." << endl;
//     cout<<"-------------"<<endl;
    
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
  */
  
//   unsigned int index = 0;

  //////////////// pair-ordering by sumPt ////////////////////////
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTaus;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausOS;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausSS;
  std::map<double, unsigned int , MuTauStreamAnalyzer::more> sortedDiTausLooseIso;
  std::vector<DiTauInfo>sortDiTauInfos; sortDiTauInfos.clear();
  for(unsigned int i=0; i< diTaus->size(); i++){
    float sumPt    = (((*diTaus)[i].leg1())->pt() + ((*diTaus)[i].leg2())->pt());
    int pairCharge = (((*diTaus)[i].leg1())->charge()*((*diTaus)[i].leg2())->charge());
    float isoTau = ((*diTaus)[i].leg2())->tauID("byIsolationMVA3oldDMwLTraw");
//     float isoTau = ((*diTaus)[i].leg2())->tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");
    float isoMu = ((*diTaus)[i].leg1())->userFloat("PFRelIsoDB04v2");
    float ptMu = ((*diTaus)[i].leg1())->pt();
    float sortVariable = (doIsoOrdering_) ? isoTau*ptMu : sumPt;
//     float sortVariable = (doIsoOrdering_) ? (isoTau-isoMu) : sumPt;
    const pat::Tau*  tau_i = dynamic_cast<const pat::Tau*>(  ((*diTaus)[i].leg2()).get() );
    if(/*tau_i->tauID("byLooseCombinedIsolationDeltaBetaCorr")>0.5 ||*/
       //tau_i->tauID("byLooseIsolationMVA")>0.5)
      //tau_i->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5 )
       tau_i->tauID("byLooseIsolationMVA3oldDMwLT")>0.5 )
      sortedDiTausLooseIso.insert( make_pair( sortVariable, i ) );
    sortedDiTaus.insert( make_pair( sortVariable, i ) );
    if(pairCharge<0) 
      sortedDiTausOS.insert( make_pair( sortVariable, i ) );
    else
      sortedDiTausSS.insert( make_pair( sortVariable, i ) );
    //Fill all without ordering 
    DiTauInfo sortDiTauInfo; 
    sortDiTauInfo.index_ = i; 
    sortDiTauInfo.sumPt_ = sortVariable; //sumPt; 
    sortDiTauInfo.diTauCharge_ = pairCharge; 
    sortDiTauInfos.push_back(sortDiTauInfo); 
  }


//   if( sortedDiTausOS.size()>0 ) 
//     index = (sortedDiTausOS.begin())->second ;
//   else
//     index = (sortedDiTausSS.begin())->second ;
    
  numOfLooseIsoDiTaus_ = sortedDiTausLooseIso.size();

  //sort diTaus, first OS and then according to sumPt  
  std::sort(sortDiTauInfos.begin(), sortDiTauInfos.end(), SortDiTauPairs()); 

  ////////////////////////////////////////////////////////

  if(isMC_) 
    genMETP4_->push_back( (*rawMet)[0].genMET()->p4() );
  //sumEt_  = (*met)[0].sumEt(); //get it from diTau pair
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
  //for(std::map<double, unsigned int , MuTauStreamAnalyzer::more>::iterator iter = sortedDiTaus.begin(); 
  //iter != sortedDiTaus.end() ; iter++){
  for(std::vector<DiTauInfo>::iterator iter = sortDiTauInfos.begin();  iter != sortDiTauInfos.end() ; iter++){ 
    diTauCounter++;

    index_   = diTauCounter;
    //theDiTau = &(*diTaus)[ iter->second ];
    theDiTau = &(*diTaus)[iter->index_];
    
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
    jetQuarkGluon_->clear(); // Quark/gluon id
    jetQuarkGluonGen_->clear(); // Quark/gluon id
    gammadR_->clear();
    gammadEta_->clear();
    gammadPhi_->clear();
    gammaPt_->clear();
    metSgnMatrix_->clear();
    genJetsIDP4_->clear();
    genDiTauLegsP4_->clear();
    genTausP4_->clear();
    genTausP4_->clear();
    genTausCharge_->clear();
    genTausDecayLeptonically_->clear();  
    jetsBtagHE_->clear();
    jetsBtagHP_->clear();
    jetsBtagCSV_->clear();
    bQuark_->clear();
    sigDCA_->clear();
    HLTfiltersMu_->clear();
    HLTfiltersTau_->clear();
    HLTfiltersTauJet_->clear();
//     tauXTriggers_->clear();
    extraMuons_->clear();
    leptonJets_->clear();
    METP4_->clear();
    embeddingWeights_->clear();

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
    /*
    if(metCovHandle.isValid()){
      const TMatrixD cov = (*metCovHandle);
      const double* elements;
      elements = cov.GetMatrixArray();
      //cout<<" met cov modified "<<elements[0]<<endl;
      metSgnMatrix_->push_back( elements[0] ); //sigma_E
      metSgnMatrix_->push_back( elements[1] ); //sigma_Ephi
      metSgnMatrix_->push_back( elements[3] ); //sigma_phi
    }
    */
    //Use Cov of the MET from diTau 
    const TMatrixD cov = theDiTau->met()->getSignificanceMatrix(); 
    const double* elements; 
    elements = cov.GetMatrixArray(); 
    //cout<<" met cov modified "<<elements[0]<<endl; 
    metSgnMatrix_->push_back( elements[0] ); //sigma_E 
    metSgnMatrix_->push_back( elements[1] ); //sigma_Ephi 
    metSgnMatrix_->push_back( elements[3] ); //sigma_phi 

    METP4_->push_back((*rawMet)[0].p4()); 
    METP4_->push_back(theDiTau->met()->p4());

    
//     if(met)METP4_->push_back((*met)[0].p4());
//     if(mvaMet)METP4_->push_back((*mvaMet)[0].p4()); 
    sumEt_  = theDiTau->met()->sumEt();

    isMuLegMatched_  = 0;
    isTauLegMatched_ = 0;
    isTauLegMatchedToLep_ = 0;

    const pat::Muon* leg1 = dynamic_cast<const pat::Muon*>( (theDiTau->leg1()).get() );
    const pat::Tau*  leg2 = dynamic_cast<const pat::Tau*>(  (theDiTau->leg2()).get() );

    isZdecay_ = false;

    isLeg1MatchedToHadTau_ = false;
    isLeg1MatchedToMuon_ = false;
    isLeg1MatchedToElectron_ = false;
    isLeg1MatchedToJet_ = false;
    
    isLeg2MatchedToHadTau_ = false;
    isLeg2MatchedToMuon_ = false;
    isLeg2MatchedToElectron_ = false;
    isLeg2MatchedToJet_ = false;

    isLeg1MatchedToLepton_ = false;
    isLeg2MatchedToLepton_ = false;

    isZthth_ = false ;
    isZtt_ = false ;
    isZttl_= false ;
    isZttj_= false ;
    isZj_= false ;
    isZee_= false ;
    isZmm_= false ;
    isZll_ = false;
    

    if ( isMC_ || isPFEmb_ )
      {
	//get Z or gamma*
	const reco::GenParticle* genZ_or_Gammastar = 0;


	for ( reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); ++genParticle )
	  {
	    if((genParticle->pdgId() == 11 || genParticle->pdgId() == -11))
	      {
		cout<<"genParticle->pt() = "<<genParticle->pt()<<endl;
		cout<<"genParticle->mother()->pdgId() = "<<genParticle->mother()->pdgId()<<endl;
		cout<<"---"<<endl;
	      }
	  }

	for ( reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); ++genParticle )
	  {
	    if ( (genParticle->pdgId() == 22 || genParticle->pdgId() == 23) && genParticle->mass() > 0. )
// 	    if ( (genParticle->pdgId() == 22 || genParticle->pdgId() == 23) && genParticle->mass() > 50. )#nMSSM --> removed
// 	    if ( (genParticle->pdgId() == 22 || genParticle->pdgId() == 23) && genParticle->mass() > 50. )#nMSSM --> removed
	      {
		genZ_or_Gammastar = &(*genParticle);
		break;
	      }
	  }

	//get Z or gamma* daughters
	std::vector<const reco::GenParticle*> allDaughters;

	//check Z or gamma* decay mode
	if ( genZ_or_Gammastar )
	  {
	    isZdecay_ = true ;
	    findDaughters(genZ_or_Gammastar, allDaughters, -1);      
	  }
	else // in the case a Z or gamma* is not found (inconsistency in DY MC or EmbedPF), still fill the daughters not to lose the events
	  {
	    for ( reco::GenParticleCollection::const_iterator genParticle = genParticles->begin(); genParticle != genParticles->end(); ++genParticle )
	      {
		allDaughters.push_back(&(*genParticle));
	      }
	  }

	//loop on daughters to check decay mode
	const reco::GenParticle* genLeptonPlus  = 0;
	const reco::GenParticle* genLeptonMinus = 0;
	for ( std::vector<const reco::GenParticle*>::const_iterator daughter = allDaughters.begin(); daughter != allDaughters.end(); ++daughter )
	  {
	    int absPdgId = TMath::Abs((*daughter)->pdgId());
// 	    cout<<"absPdgId = "<<absPdgId<<endl;
	    //give preference to taus, since electrons and muons may either come from decay of Z-boson or from tau decay
	    //taus
	    if ( absPdgId == 15 && (*daughter)->charge() > +0.5 && (isZdecay_ || !genLeptonPlus) ) genLeptonPlus  = (*daughter);
	    if ( absPdgId == 15 && (*daughter)->charge() < -0.5 && (isZdecay_ || !genLeptonMinus) ) genLeptonMinus = (*daughter);
	    //electrons or muons
	    if ( (absPdgId == 11 || absPdgId == 13) && (*daughter)->charge() > +0.5 && !genLeptonPlus  ) genLeptonPlus  = (*daughter);
	    if ( (absPdgId == 11 || absPdgId == 13) && (*daughter)->charge() < -0.5 && !genLeptonMinus ) genLeptonMinus = (*daughter);
	  }

	//check if leg1 is matched to a Z decay product
	bool leg1isGenHadTau   = false;
	bool leg1isGenMuon     = false;
	bool leg1isGenElectron = false;
	bool leg1isGenJet      = false;
	if(genLeptonPlus && genLeptonMinus) matchGenParticleFromZdecay(leg1->p4(), genLeptonPlus, genLeptonMinus, leg1isGenHadTau, leg1isGenMuon, leg1isGenElectron, leg1isGenJet);

	//check if leg2 is matched to a Z decay product
	bool leg2isGenHadTau   = false;
	bool leg2isGenMuon     = false;
	bool leg2isGenElectron = false;
	bool leg2isGenJet      = false;
	if(genLeptonPlus && genLeptonMinus) matchGenParticleFromZdecay(leg2->p4(), genLeptonPlus, genLeptonMinus, leg2isGenHadTau, leg2isGenMuon, leg2isGenElectron, leg2isGenJet);

	cout<<"leg1isGenElectron = "<<leg1isGenElectron<<endl;
	cout<<"leg1isGenMuon = "<<leg1isGenMuon<<endl;
	cout<<"leg2isGenHadTau = "<<leg2isGenHadTau<<endl;

	//check decay modes

	//Ztt --> both leg1 and leg2 matched to generator taus
	if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 15 && genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 15 )
	  {
	    //Hadronic decay --> both legs matched to gen had tau
	    if      ( leg1isGenHadTau && leg2isGenHadTau )                                          isZthth_ = true;
	    //Semi-leptonic taus decays --> leg1 matched to lepton, leg2 matched to gen had tau
	    else if ( (leg1isGenElectron || leg1isGenMuon) && leg2isGenHadTau)                      isZtt_   = true;
	    //Fully-leptonic taus decays --> leg1 matched to gen lepton, leg2 matched to gen lepton
	    else if ( (leg1isGenMuon || leg1isGenElectron) && (leg2isGenMuon || leg2isGenElectron)) isZttl_  = true;
	    //Other cases (most are when a jet fakes a had. tau)
	    else                                                                                    isZttj_  = true;
	  }
	//Zee --> both leg1 and leg2 matched to generator electrons
	else if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 11 && leg1isGenElectron && genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 11 && leg2isGenElectron )
	  {
	    isZee_ = true;
	  }
	//Zmm --> both leg1 and leg2 matched to generator muons
	else if ( genLeptonPlus  && TMath::Abs(genLeptonPlus->pdgId())  == 13 && leg1isGenMuon && genLeptonMinus && TMath::Abs(genLeptonMinus->pdgId()) == 13 && leg2isGenMuon )
	  {
	    isZmm_ = true;
	  }
	//Zj --> at least one leg matched to a jet, or legs not matched to same flavor lepton, or a leg is not matched at all
	else
	  {
	    isZj_ = true;
	  }
      

	//check truth matching
	isLeg1MatchedToHadTau_ = false;
	isLeg1MatchedToMuon_ = false;
	isLeg1MatchedToElectron_ = false;
	isLeg1MatchedToJet_ = false;
	matchGenParticle(leg1->p4(), *genParticles, isLeg1MatchedToHadTau_, isLeg1MatchedToMuon_, isLeg1MatchedToMuon_, isLeg1MatchedToJet_);
    
	isLeg2MatchedToHadTau_ = false;
	isLeg2MatchedToMuon_ = false;
	isLeg2MatchedToElectron_ = false;
	isLeg2MatchedToJet_ = false;
	matchGenParticle(leg2->p4(), *genParticles, isLeg2MatchedToHadTau_, isLeg2MatchedToMuon_, isLeg2MatchedToMuon_, isLeg2MatchedToJet_);
    
	isLeg1MatchedToLepton_ = isLeg1MatchedToElectron_ || isLeg1MatchedToMuon_ ;
	isLeg2MatchedToLepton_ = isLeg2MatchedToElectron_ || isLeg2MatchedToMuon_ ;
    
	isZll_ = isZee_ || isZmm_;
    
	TString flag = "";
	if(     isZtt_)   flag = "ZTT";
	else if(isZll_ || isZttl_)   flag = "ZL";
	else if(isZttj_)  flag = "ZTTJ";
	else              flag = "ZJ";

	cout<<"isZdecay_ = "<<isZdecay_<<endl;
	cout<<"flag = "<<flag<<endl;

      }
    

    /*
    myfile<<""<<endl;
    myfile<<""<<endl;
    myfile<<""<<endl;
    myfile<<""<<endl;
    myfile <<  "Run " << iEvent.run() << ", event " << (iEvent.eventAuxiliary()).event() 
	 << ", lumi " << iEvent.luminosityBlock() << endl;

    myfile<<"Raw MET = "<<(*METP4_)[0].Et()<<endl;
    myfile<<"MVA MET = "<<(*METP4_)[1].Et()<<endl;
    myfile<<"   for leptons: - lepton E/p/pt/eta/phi = "<<(leg1->p4()).E()<<"/"<<leg1->p()<<"/"<<leg1->pt()<<"/"<<leg1->eta()<<"/"<<leg1->phi()<<endl;
    myfile<<"                - tau    E/p/pt/eta/phi = "<<(leg2->p4()).E()<<"/"<<leg2->p()<<"/"<<leg2->pt()<<"/"<<leg2->eta()<<"/"<<leg2->phi()<<endl;
    myfile<<"MET covariance matrix: "<<endl;
    myfile<<"            (0,0) = "<<cov(0,0)<<endl;
    myfile<<"            (0,1) = "<<cov(0,1)<<endl;
    myfile<<"            (1,0) = "<<cov(1,0)<<endl;
    myfile<<"            (1,1) = "<<cov(1,1)<<endl;
    myfile<<"***********************"<<endl;
    */

    




    if(leg2->pt()>30.)// && leg1->pt()>20.)
      {
// 	cout<<"event = "<<(iEvent.eventAuxiliary()).event()<<endl;
// 	cout<<"tau, Pt = "<<leg2->pt()<<", eta = "<<leg2->eta()<<", phi = "<<leg2->phi()<<", MVA wLT = "<<leg2->tauID("byIsolationMVA3oldDMwLTraw")<<endl;
// 	cout<<"mu,  Pt = "<<leg1->pt()<<endl;
      }


    muFlag_       = 0;  
    muVetoRelIso_ = 0.;  
    muFlagSoft_       = 0;  
    muVetoRelIsoSoft_ = 0.;  

    for(unsigned int i=0; i<muonsRel->size(); i++){
      if( Geom::deltaR( (*muonsRel)[i].p4(),leg1->p4())>0.3) {
	extraMuons_->push_back( (*muonsRel)[i].p4() );
	//2nd electron veto
	bool found=false, found2=false;
	float vetoPtInclusive = 15.;
	if(!found && (*muonsRel)[i].pt()>vetoPtInclusive && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3){
	  if( (*muonsRel)[i].charge()*leg1->charge()>0){
	    muFlag_=2;
	    muVetoRelIso_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	  }
	  if( (*muonsRel)[i].charge()*leg1->charge()<0){
	    muFlag_=1;
	    muVetoRelIso_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	    found=true;
	  }
	}
	if(!found2 && (*muonsRel)[i].userFloat("PFRelIsoDB04v2")<0.3){
	  if( (*muonsRel)[i].charge()*leg1->charge()>0){
	    muFlagSoft_=2;
	    muVetoRelIsoSoft_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	  }
	  if( (*muonsRel)[i].charge()*leg1->charge()<0){
	    muFlagSoft_=1;
	    muVetoRelIsoSoft_ = (*muonsRel)[i].userFloat("PFRelIsoDB04v2");
	    found2=true;
	  }
	}
      }
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

    for(iter_trigger = HLTfiltersMu.begin(); iter_trigger != HLTfiltersMu.end(); iter_trigger++)
      {
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
	  if( Geom::deltaR( aObj->triggerObject().p4(), leg1->p4() )<0.5  && aObj->hasFilterLabel(iter_trigger->first) && aObj->hasTriggerObjectType(trigger::TriggerMuon)){
	    matched = true;
	  }
	}
	if(matched) 
	  {
	    (*HLTfiltersMu_)[iter_trigger->first] = 1;
	    HLTfiltersMu[iter_trigger->first] = 1;
	  }
	else
	  {
	    (*HLTfiltersMu_)[iter_trigger->first] = 0;
	    HLTfiltersMu[iter_trigger->first] = 0;
	  }
	if(verbose_){
	  if(matched) cout << "Muon matched within dR=0.5 with trigger object passing filter " << iter_trigger->first << endl;
	  else cout << "!!! Muon is not trigger matched within dR=0.5 !!!" << endl;
	}
      }

    int counter_L1_mus = 0 ;
    
    if(l1mus){//check matching with l1mu Pt>7 |eta|<2.1
      bool matched = false;
      for(unsigned int i=0; i<l1mus->size(); ++i){
	if( (*l1mus)[i].pt()<7 || fabs((*l1mus)[i].eta() )>2.1 ) continue;
	if( Geom::deltaR( (*l1mus)[i].p4(), leg1->p4() )<0.5 ){
	  matched = true;
	  break;
	}
      }

      counter_L1_mus++;
      string Result = "l1mu";
      ostringstream convert;
      convert << counter_L1_mus;
      Result.append(convert.str());

      if(matched) 
	{
	  (*HLTfiltersMu_)[Result] = 1;
	}
      else 
	{
	  (*HLTfiltersMu_)[Result] = 0;
	}
      if(verbose_){
	if(matched) cout << "Muon matched within dR=0.5 with trigger L1Mu pt>7, |eta|<2.1" << endl;
	else cout << "!!! Muon is not matched to L1Mu within dR=0.5 !!!" << endl;
      }
    }
    //     else //removed when using maps
    //       tauXTriggers_->push_back(0);

    /*
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
    if(l1mus){//check matching with l1mu Pt>7 |eta|<2.1
      bool matched = false;
      for(unsigned int i=0; i<l1mus->size(); ++i){
	if( (*l1mus)[i].pt()<7 || fabs((*l1mus)[i].eta() )>2.1 ) continue;
	if( Geom::deltaR( (*l1mus)[i].p4(), leg1->p4() )<0.5 ){
	  matched = true;
	  break;
	}
      }
      if(matched) 
	tauXTriggers_->push_back(1);
      else 
	tauXTriggers_->push_back(0);
      if(verbose_){
	if(matched) cout << "Muon matched within dR=0.5 with trigger L1Mu pt>7, |eta|<2.1" << endl;
	else cout << "!!! Muon is not matched to L1Mu within dR=0.5 !!!" << endl;
      }
    }
    else
      tauXTriggers_->push_back(0);
    //
    */


    for(iter_trigger = HLTfiltersTau.begin(); iter_trigger != HLTfiltersTau.end(); iter_trigger++)
      {
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
	  if((iter_trigger->first).find("IsoMu18PFTau25") != string::npos || (iter_trigger->first).find("hltL2Tau") != string::npos){
	    if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(iter_trigger->first) && aObj->hasTriggerObjectType(trigger::TriggerTau) && aObj->triggerObject().p4().pt() > 35){ 
	      matched = true; 
	    }
	  }
	  else if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(iter_trigger->first) && aObj->hasTriggerObjectType(trigger::TriggerTau) ){
	    matched = true;
	  }
	}
	if(matched) 
	  {
	    (*HLTfiltersTau_)[iter_trigger->first] = 1 ;
	    HLTfiltersTau[iter_trigger->first] = 1 ;
	  }
	else 
	  {
	    (*HLTfiltersTau_)[iter_trigger->first] = 0 ;
	    HLTfiltersTau[iter_trigger->first] = 0 ;
	  }
	if(verbose_){
	  if(matched) cout << "Tau matched within dR=0.5 with trigger object passing filter " << iter_trigger->first << endl;
	  else cout << "!!! Tau is not trigger matched within dR=0.5 !!!" << endl;
	}
      }

    /*
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
	if(HLTfiltersTau[i].find("IsoMu18PFTau25") != string::npos || HLTfiltersTau[i].find("hltL2Tau") != string::npos){
	  if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(HLTfiltersTau[i]) && aObj->hasTriggerObjectType(trigger::TriggerTau) && aObj->triggerObject().p4().pt() > 35){ 
	    matched = true; 
	  }
	}
	else if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(HLTfiltersTau[i]) && aObj->hasTriggerObjectType(trigger::TriggerTau) ){
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
    */
//     if(trgTaus){//check matching with HltTau@offline, Pt>20, LooseIso
//       bool matched = false;
//       for(unsigned int i=0; i<trgTaus->size(); ++i){
// 	if( (*trgTaus)[i].pt()<20 ) continue;
// 	if( (*trgTaus)[i].tauID("decayModeFinding")<0.5 || 
// 	    (*trgTaus)[i].tauID("byIsolation")<0.5 ) continue;
// 	if( Geom::deltaR( (*trgTaus)[i].p4(), leg2->p4() )<0.5 ){
// 	  matched = true;
// 	  break;
// 	}
//       }
//       if(matched) 
// 	tauXTriggers_->push_back(1);
//       else 
// 	tauXTriggers_->push_back(0);
//       if(verbose_){
// 	if(matched) cout << "Tau matched within dR=0.5 with HLTPFTau@offline pt>20" << endl;
// 	else cout << "!!! Tau is not matched to HLTPFTau@offline pt>20 !!!" << endl;
//       }
//     }
//     else
//       tauXTriggers_->push_back(0);

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
    
    //check matching to l1tau 44 or l1jet 64
    int counter_L1_taus = 0 ;
    //check matching to l1tau 44 or l1jet 64
    if(l1taus){//check matching with l1tau Pt>44 |eta|<2.127
      bool matched = false;
      for(unsigned int i=0; i<l1taus->size(); ++i){
	if( (*l1taus)[i].pt()<44 || fabs((*l1taus)[i].eta() )>2.172 ) continue;
	if( Geom::deltaR( (*l1taus)[i].p4(), leg2->p4() )<0.5 ){
	  matched = true;
	  break;
	}
      }
      if(!matched){ 
	if(l1jets){//check matching with l1jet Pt>64 |eta|<2.172
	  for(unsigned int i=0; i<l1jets->size(); ++i){
	    if( (*l1jets)[i].pt()<64 || fabs((*l1jets)[i].eta() )>2.127 ) continue;
	    if( Geom::deltaR( (*l1jets)[i].p4(), leg2->p4() )<0.5 ){
	      matched = true;
	      break;
	    }
	  } 
	}
      }
      counter_L1_taus++;
      string Result = "l1tau";
      ostringstream convert;
      convert << counter_L1_taus;
      Result.append(convert.str());

      (*HLTfiltersTau_)[Result] = int(matched) ;
      //tauXTriggers_->push_back( int(matched) );

      if(verbose_){
	if(matched) cout << "Tau matched within dR=0.5 with trigger L1Tau pt>44, |eta|<2.1 or L1Jet pt>64, |eta|<2.1" << endl;
	else cout << "!!! Tau is not matched to L1Tau/Jet within dR=0.5 !!!" << endl;
      }
    }
    //     else // removed when using maps
    //       tauXTriggers_->push_back(0);

    /*
    //check matching to l1tau 44 or l1jet 64
    if(l1taus){//check matching with l1tau Pt>44 |eta|<2.127
      bool matched = false;
      for(unsigned int i=0; i<l1taus->size(); ++i){
	if( (*l1taus)[i].pt()<44 || fabs((*l1taus)[i].eta() )>2.172 ) continue;
	if( Geom::deltaR( (*l1taus)[i].p4(), leg2->p4() )<0.5 ){
	  matched = true;
	  break;
	}
      }
      if(!matched){ 
	if(l1jets){//check matching with l1jet Pt>64 |eta|<2.172
	  for(unsigned int i=0; i<l1jets->size(); ++i){
	    if( (*l1jets)[i].pt()<64 || fabs((*l1jets)[i].eta() )>2.127 ) continue;
	    if( Geom::deltaR( (*l1jets)[i].p4(), leg2->p4() )<0.5 ){
	      matched = true;
	      break;
	    }
	  } 
	}
      }
      tauXTriggers_->push_back( int(matched) );
      if(verbose_){
	if(matched) cout << "Tau matched within dR=0.5 with trigger L1Tau pt>44, |eta|<2.1 or L1Jet pt>64, |eta|<2.1" << endl;
	else cout << "!!! Tau is not matched to L1Tau/Jet within dR=0.5 !!!" << endl;
      }
    }
    else
      tauXTriggers_->push_back(0);
    */
    
    //matching to hlt1PFJet320
    for(iter_trigger = HLTfiltersTauJet.begin(); iter_trigger != HLTfiltersTauJet.end(); iter_trigger++)
      {
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
	  if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(iter_trigger->first) && aObj->hasTriggerObjectType(trigger::TriggerJet) ){
	    matched = true;
	  }
	}
	if(matched)
	  {
	    (*HLTfiltersTauJet_)[iter_trigger->first] = 1 ;
	    HLTfiltersTauJet[iter_trigger->first] = 1 ;
	  }
	else
	  {
	    (*HLTfiltersTauJet_)[iter_trigger->first] = 0 ;
	    HLTfiltersTauJet[iter_trigger->first] = 0 ;
	  }
	if(verbose_){
	  if(matched) cout << "Tau matched within dR=0.5 with trigger object passing filter " << iter_trigger->first << endl;
	  else cout << "!!! Tau is not trigger matched to " << iter_trigger->first << "within dR=0.5 !!!" << endl;
	}
      }

    /*
    //matching to hlt1PFJet320
    for(unsigned int i=0 ; i< HLTfiltersTauJet.size() ; i++){
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
        if( Geom::deltaR( aObj->triggerObject().p4(), leg2->p4() )<0.5  && aObj->hasFilterLabel(HLTfiltersTauJet[i]) && aObj->hasTriggerObjectType(trigger::TriggerJet) ){
          matched = true;
        }
      }
      if(matched)
        tauXTriggers_->push_back(1);
      else
	tauXTriggers_->push_back(0);
      if(verbose_){
        if(matched) cout << "Tau matched within dR=0.5 with trigger object passing filter " << HLTfiltersTauJet[i] << endl;
        else cout << "!!! Tau is not trigger matched to " << HLTfiltersTauJet[i] << "within dR=0.5 !!!" << endl;
      }
    }
    */

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

    if(isMC_ || isPFEmb_){
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
	//if( abs((*genParticles)[k].pdgId()) != 13  || (*genParticles)[k].status()!=3 )
	if( abs((*genParticles)[k].pdgId()) != 13)  //update to new prescription
	  continue;
	//if(Geom::deltaR( (*genParticles)[k].p4(),leg2->p4())<0.15){
	if(Geom::deltaR( (*genParticles)[k].p4(),leg2->p4())<0.5 && (*genParticles)[k].p4().pt() > 8.0){
	  leg2IsFromMu = true;
	  genMuP4 = (*genParticles)[k].p4();
	}
      }
      bool leg2IsFromEle = false;
      math::XYZTLorentzVectorD genEleP4(0,0,0,0);
      for(unsigned int k = 0; k < genParticles->size(); k ++){
        //if( abs((*genParticles)[k].pdgId()) != 11  || (*genParticles)[k].status()!=3 )
        if( abs((*genParticles)[k].pdgId()) != 11)  //update to new prescription
          continue;
        //if(Geom::deltaR( (*genParticles)[k].p4(),leg2->p4())<0.15){
        if(Geom::deltaR( (*genParticles)[k].p4(),leg2->p4())<0.5 && (*genParticles)[k].p4().pt() > 8.0){
          leg2IsFromEle = true;
          genEleP4 = (*genParticles)[k].p4();
        }
      }    

      if( leg2->genJet() !=0 )
	genDiTauLegsP4_->push_back(leg2->genJet()->p4());
      else if(leg2IsFromMu)
	genDiTauLegsP4_->push_back( genMuP4 );
      else if(leg2IsFromEle) 
        genDiTauLegsP4_->push_back( genEleP4 );
      else{
	genDiTauLegsP4_->push_back( math::XYZTLorentzVectorD(0,0,0,0) );
	if(verbose_) cout << "WARNING: no genJet matched to the leg2 with eta,phi " << leg2->eta() << ", " << leg2->phi() << endl;
      }

      if(leg2IsFromMu || leg2IsFromEle) isTauLegMatchedToLep_ = 1;
      else isTauLegMatchedToLep_ = 0;

      bool tauHadMatched = false;
      for(unsigned int k = 0; k < tauGenJets->size(); k++){
	if( Geom::deltaR( (*tauGenJets)[k].p4(),leg2->p4() ) < 0.5 && (*tauGenJets)[k].p4().pt() > 8.0) tauHadMatched = true; //update to new prescription
      }
      
      if( ( (leg2->genParticleById( 15,0,true)).isNonnull() || 
	    (leg2->genParticleById(-15,0,true)).isNonnull() ) 
	  && tauHadMatched ){
	isTauLegMatched_ = 1;
	if(leg2->genJet()){
	  string genTauDecay = JetMCTagUtils::genTauDecayMode( *(leg2->genJet()) );
	  if( genTauDecay.find("oneProng0Pi0")!=string::npos ) 
	    genDecayMode_ = 0;
	  else if( genTauDecay.find("oneProng1Pi0")!=string::npos )
	    genDecayMode_ = 1;
	  else if( genTauDecay.find("oneProng2Pi0")!=string::npos )
	    genDecayMode_ = 2;
	  else if( genTauDecay.find("oneProngOther")!=string::npos )
	    genDecayMode_ = 3;
	  if( genTauDecay.find("twoProng0Pi0")!=string::npos ) 
	    genDecayMode_ = 4;
	  else if( genTauDecay.find("twoProng1Pi0")!=string::npos )
	    genDecayMode_ = 5;
	  else if( genTauDecay.find("twoProng2Pi0")!=string::npos )
	    genDecayMode_ = 6;
	  else if( genTauDecay.find("twoProngOther")!=string::npos )
	    genDecayMode_ = 7;
	  else if( genTauDecay.find("threeProng0Pi0")!=string::npos )
	    genDecayMode_ = 8;
	  else if( genTauDecay.find("threeProng1Pi0")!=string::npos )
	    genDecayMode_ = 9;
	  else if( genTauDecay.find("threeProngOther")!=string::npos )
	    genDecayMode_ = 10;
	  else if( genTauDecay.find("rare")!=string::npos )
	    genDecayMode_ = 11;
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
    else if((leg2->signalPFChargedHadrCands()).size()==2 && (leg2->signalPFGammaCands()).size()==0)  decayMode_ = 2; 
    else if((leg2->signalPFChargedHadrCands()).size()==2 && (leg2->signalPFGammaCands()).size()>0)  decayMode_ = 3; 
    else if((leg2->signalPFChargedHadrCands()).size()==3) decayMode_ = 4; 
    else  decayMode_ = -99;

    for(unsigned int k = 0 ; k < (leg2->signalPFGammaCands()).size() ; k++){
      reco::PFCandidatePtr gamma = (leg2->signalPFGammaCands()).at(k);
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

   //TauID
    decayModeFinding_ = leg2->tauID("decayModeFinding");
    decayModeFindingNewDM_ = leg2->tauID("decayModeFindingNewDMs");
    decayModeFindingOldDM_ = leg2->tauID("decayModeFindingOldDMs");

    tightestHPSDBWP_ = -1;
    if(leg2->tauID("byVLooseCombinedIsolationDeltaBetaCorr")>0.5) tightestHPSDBWP_=0;
    if(leg2->tauID("byLooseCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_=1;
    if(leg2->tauID("byMediumCombinedIsolationDeltaBetaCorr")>0.5) tightestHPSDBWP_=2;
    if(leg2->tauID("byTightCombinedIsolationDeltaBetaCorr")>0.5)  tightestHPSDBWP_=3;
    tightestHPSDB3HWP_ = -1; 
    if(leg2->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5)  tightestHPSDB3HWP_=0; 
    if(leg2->tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits")>0.5) tightestHPSDB3HWP_=1; 
    if(leg2->tauID("byTightCombinedIsolationDeltaBetaCorr3Hits")>0.5)  tightestHPSDB3HWP_=2;
    hpsDB3H_  = leg2->tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits");

    tightestHPSMVA3newDMwLTWP_ = -1;
    if(leg2->tauID("byVLooseIsolationMVA3newDMwLT") >0.5) tightestHPSMVA3newDMwLTWP_=0;
    if(leg2->tauID("byLooseIsolationMVA3newDMwLT")>0.5)   tightestHPSMVA3newDMwLTWP_=1;
    if(leg2->tauID("byMediumIsolationMVA3newDMwLT") >0.5) tightestHPSMVA3newDMwLTWP_=2;
    if(leg2->tauID("byTightIsolationMVA3newDMwLT") >0.5)  tightestHPSMVA3newDMwLTWP_=3;
    if(leg2->tauID("byVTightIsolationMVA3newDMwLT") >0.5) tightestHPSMVA3newDMwLTWP_=4;
    if(leg2->tauID("byVVTightIsolationMVA3newDMwLT") >0.5)tightestHPSMVA3newDMwLTWP_=5;
    hpsMVA3newDMwLT_  = leg2->tauID("byIsolationMVA3newDMwLTraw");

    tightestHPSMVA3newDMwoLTWP_ = -1;
    if(leg2->tauID("byVLooseIsolationMVA3newDMwoLT") >0.5) tightestHPSMVA3newDMwoLTWP_=0;
    if(leg2->tauID("byLooseIsolationMVA3newDMwoLT")>0.5)   tightestHPSMVA3newDMwoLTWP_=1;
    if(leg2->tauID("byMediumIsolationMVA3newDMwoLT") >0.5) tightestHPSMVA3newDMwoLTWP_=2;
    if(leg2->tauID("byTightIsolationMVA3newDMwoLT") >0.5)  tightestHPSMVA3newDMwoLTWP_=3;
    if(leg2->tauID("byVTightIsolationMVA3newDMwoLT") >0.5) tightestHPSMVA3newDMwoLTWP_=4;
    if(leg2->tauID("byVVTightIsolationMVA3newDMwoLT") >0.5)tightestHPSMVA3newDMwoLTWP_=5;
    hpsMVA3newDMwoLT_  = leg2->tauID("byIsolationMVA3newDMwoLTraw");

    tightestHPSMVA3oldDMwLTWP_ = -1;
    if(leg2->tauID("byVLooseIsolationMVA3oldDMwLT") >0.5) tightestHPSMVA3oldDMwLTWP_=0;
    if(leg2->tauID("byLooseIsolationMVA3oldDMwLT")>0.5)   tightestHPSMVA3oldDMwLTWP_=1;
    if(leg2->tauID("byMediumIsolationMVA3oldDMwLT") >0.5) tightestHPSMVA3oldDMwLTWP_=2;
    if(leg2->tauID("byTightIsolationMVA3oldDMwLT") >0.5)  tightestHPSMVA3oldDMwLTWP_=3;
    if(leg2->tauID("byVTightIsolationMVA3oldDMwLT") >0.5) tightestHPSMVA3oldDMwLTWP_=4;
    if(leg2->tauID("byVVTightIsolationMVA3oldDMwLT") >0.5)tightestHPSMVA3oldDMwLTWP_=5;
    hpsMVA3oldDMwLT_  = leg2->tauID("byIsolationMVA3oldDMwLTraw");

    tightestHPSMVA3oldDMwoLTWP_ = -1;
    if(leg2->tauID("byVLooseIsolationMVA3oldDMwoLT") >0.5) tightestHPSMVA3oldDMwoLTWP_=0;
    if(leg2->tauID("byLooseIsolationMVA3oldDMwoLT")>0.5)   tightestHPSMVA3oldDMwoLTWP_=1;
    if(leg2->tauID("byMediumIsolationMVA3oldDMwoLT") >0.5) tightestHPSMVA3oldDMwoLTWP_=2;
    if(leg2->tauID("byTightIsolationMVA3oldDMwoLT") >0.5)  tightestHPSMVA3oldDMwoLTWP_=3;
    if(leg2->tauID("byVTightIsolationMVA3oldDMwoLT") >0.5) tightestHPSMVA3oldDMwoLTWP_=4;
    if(leg2->tauID("byVVTightIsolationMVA3oldDMwoLT") >0.5)tightestHPSMVA3oldDMwoLTWP_=5;
    hpsMVA3oldDMwoLT_  = leg2->tauID("byIsolationMVA3oldDMwoLTraw");

    tightestAntiMuWP_ = 0; 
    if( leg2->tauID("againstMuonLoose")>0.5 )tightestAntiMuWP_ = 1; 
    if( leg2->tauID("againstMuonMedium")>0.5 )tightestAntiMuWP_ = 2; 
    if( leg2->tauID("againstMuonTight")>0.5 )tightestAntiMuWP_ = 3; 

    tightestAntiMu2WP_ = 0;   
    if( leg2->tauID("againstMuonLoose2")>0.5 )tightestAntiMu2WP_ = 1;   
    if( leg2->tauID("againstMuonMedium2")>0.5 )tightestAntiMu2WP_ = 2;   
    if( leg2->tauID("againstMuonTight2")>0.5 )tightestAntiMu2WP_ = 3;   

    tightestAntiMu3WP_ = 0;   
    if( leg2->tauID("againstMuonLoose3")>0.5 )tightestAntiMu3WP_ = 1;   
    if( leg2->tauID("againstMuonTight3")>0.5 )tightestAntiMu3WP_ = 2;   

    tightestAntiMuMVAWP_ = 0;   
    if( leg2->tauID("againstMuonLooseMVA")>0.5 )tightestAntiMuMVAWP_ = 1;   
    if( leg2->tauID("againstMuonMediumMVA")>0.5 )tightestAntiMuMVAWP_ = 2;   
    if( leg2->tauID("againstMuonTightMVA")>0.5 )tightestAntiMuMVAWP_ = 3;   
    AntiMuMVAraw_ = leg2->tauID("againstMuonMVAraw");   

    tightestAntiECutWP_ = 0;
    if( leg2->tauID("againstElectronLoose")>0.5 )tightestAntiECutWP_ = 1;
    if( leg2->tauID("againstElectronMedium")>0.5 )tightestAntiECutWP_ = 2;
    if( leg2->tauID("againstElectronTight")>0.5 )tightestAntiECutWP_ = 3;

    AntiEDeadEcal_ = leg2->tauID("againstElectronDeadECAL");

    tightestAntiEMVA5WP_ = 0;
    if( leg2->tauID("againstElectronVLooseMVA5")>0.5) tightestAntiEMVA5WP_  = 1;
    if( leg2->tauID("againstElectronLooseMVA5")>0.5)  tightestAntiEMVA5WP_  = 2;
    if( leg2->tauID("againstElectronMediumMVA5")>0.5) tightestAntiEMVA5WP_  = 3;
    if( leg2->tauID("againstElectronTightMVA5")>0.5)  tightestAntiEMVA5WP_  = 4;
    if( leg2->tauID("againstElectronVTightMVA5")>0.5) tightestAntiEMVA5WP_  = 5;
    AntiEMVA5raw_ = leg2->tauID("againstElectronMVA5raw");
    AntiEMVA5category_ = leg2->tauID("againstElectronMVA5category");

    /////NewTauID input variables
    chargedIsoPtSum_ = leg2->tauID("chargedIsoPtSum");
    neutralIsoPtSum_ = leg2->tauID("neutralIsoPtSum");
    puCorrPtSum_     = leg2->tauID("puCorrPtSum");
    patDecayMode_    = leg2->decayMode();
    dxyTau_          = leg2->dxy(); 
    dxyErrTau_       = leg2->dxy_error(); 
    dxySigTau_       = leg2->dxy_Sig();
    hasSecVtx_       = leg2->hasSecondaryVertex();
    flightLength_    = TMath::Sqrt(leg2->flightLength().Mag2());
    flightLengthError_= leg2->flightLengthSig()/TMath::Sqrt(leg2->flightLength().Mag2());
    flightLength2_   = leg2->flightLength().Mag2();
    flightLengthSig_ = leg2->flightLengthSig();
    ///NewTauID useful variables
    dxyPCAX_         = leg2->dxy_PCA().X();
    dxyPCAY_         = leg2->dxy_PCA().Y();
    dxyPCAZ_         = leg2->dxy_PCA().Z();
    flightLengthX_   = leg2->flightLength().X();
    flightLengthY_   = leg2->flightLength().Y();
    flightLengthZ_   = leg2->flightLength().Z();
    pvX_             = leg2->primaryVertexPos().X();
    pvY_             = leg2->primaryVertexPos().Y();
    pvZ_             = leg2->primaryVertexPos().Z();
    pvCov00_         = leg2->primaryVertexCov()(0,0);
    pvCov01_         = leg2->primaryVertexCov()(0,1);
    pvCov02_         = leg2->primaryVertexCov()(0,2);
    pvCov11_         = leg2->primaryVertexCov()(1,1);
    pvCov12_         = leg2->primaryVertexCov()(1,2);
    pvCov22_         = leg2->primaryVertexCov()(2,2);
    svX_             = leg2->secondaryVertexPos().X();
    svY_             = leg2->secondaryVertexPos().Y();
    svZ_             = leg2->secondaryVertexPos().Z();
    svCov00_         = leg2->secondaryVertexCov()(0,0);
    svCov01_         = leg2->secondaryVertexCov()(0,1);
    svCov02_         = leg2->secondaryVertexCov()(0,2);
    svCov11_         = leg2->secondaryVertexCov()(1,1);
    svCov12_         = leg2->secondaryVertexCov()(1,2);
    svCov22_         = leg2->secondaryVertexCov()(2,2);

    pfJetPt_ = (leg2->pfJetRef()).isNonnull() ? leg2->pfJetRef()->pt() : -99;
    pfJetEtaMom2_ = (leg2->pfJetRef()).isNonnull() ? leg2->pfJetRef()->etaetaMoment() : -99;
    pfJetPhiMom2_ = (leg2->pfJetRef()).isNonnull() ? leg2->pfJetRef()->phiphiMoment() : -99;
     
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
    
    math::XYZTLorentzVectorD nSVfitIntP4(0,0,0,0 );
    if( theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",0)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ){
      if(verbose_) cout << "Visible mass ==> " << (theDiTau->p4Vis()).M() << endl;
      math::PtEtaPhiMLorentzVectorD ptEtaPhiMTmpP4(theDiTau->nSVfitSolution("psKine_MEt_int",0)->pt(),
						   theDiTau->nSVfitSolution("psKine_MEt_int",0)->eta(),
						   theDiTau->nSVfitSolution("psKine_MEt_int",0)->phi(),
						   theDiTau->nSVfitSolution("psKine_MEt_int",0)->mass() );
      nSVfitIntP4.SetPxPyPzE(ptEtaPhiMTmpP4.Px(),
			     ptEtaPhiMTmpP4.Py(),
			     ptEtaPhiMTmpP4.Pz(),
			     ptEtaPhiMTmpP4.E() );
      if(verbose_) cout << "SVFit int solution ==> " << nSVfitIntP4.M() << endl;
    }
    diTauSVfitP4_->push_back( nSVfitIntP4  );
        
    ///Commented out the plugin version of SVFit for now////
    //int errFlag = 0;
    ////diTauSVfitMassErrUp_    = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",&errFlag)!=0 /*&& theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution()*/ ) ? theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->massErrUp()   : -99; 
    ////diTauSVfitMassErrDown_  = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_logM_fit",&errFlag)!=0 /*&& theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution()*/ ) ? theDiTau->nSVfitSolution("psKine_MEt_logM_fit",0)->massErrDown() : -99; 
    //
    //diTauNSVfitIsValid_     = (int)(theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() );
    //diTauNSVfitMass_        = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->mass()        : -99; 
    //diTauNSVfitMassErrUp_   = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->massErrUp()   : -99; 
    //diTauNSVfitMassErrDown_ = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->massErrDown() : -99; 
    //diTauNSVfitPt_          = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->pt()          : -99;
    //diTauNSVfitPtErrUp_     = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->ptErrUp()     : -99;
    //diTauNSVfitPtErrDown_   = (theDiTau->hasNSVFitSolutions() && theDiTau->nSVfitSolution("psKine_MEt_int",&errFlag)!=0 && theDiTau->nSVfitSolution("psKine_MEt_int",0)->isValidSolution() ) 
    //  ? theDiTau->nSVfitSolution("psKine_MEt_int",0)->ptErrDown()   : -99;
    
    /*
    //Run Standalone SVFit  
    std::vector<NSVfitStandalone::MeasuredTauLepton> measuredTauLeptons; 
    NSVfitStandalone::Vector measuredMET( theDiTau->met()->p4().Px(), theDiTau->met()->p4().Py(), 0); 
    measuredTauLeptons.push_back(NSVfitStandalone::MeasuredTauLepton(NSVfitStandalone::kLepDecay, leg1->p4())); 
    measuredTauLeptons.push_back(NSVfitStandalone::MeasuredTauLepton(NSVfitStandalone::kHadDecay, leg2->p4())); 
    NSVfitStandaloneAlgorithm algo(measuredTauLeptons, measuredMET, cov, 2); 
    algo.addLogM(false); 
    //algo.integrateMarkovChain(); 
    algo.integrateVEGAS(); 
    diTauNSVfitMass_ = algo.getMass(); 
    diTauNSVfitMassErrUp_ = algo.massUncert(); 
    diTauNSVfitPt_ = algo.pt();  
    diTauNSVfitPtErrUp_ = algo.ptUncert();


    // SVmass (leptons, mvaMET, mvaMET cov)
    cout<<"****** OLD SVFIT ********"<<endl;
    cout<<"- mu     E/p/pt/eta/phi/m = "<<(leg1->p4()).E()<<"/"<<leg1->p()<<"/"<<leg1->pt()<<"/"<<leg1->eta()<<"/"<<leg1->phi()<<"/"<<leg1->mass()<<endl;
    cout<<"- tau    E/p/pt/eta/phi/m = "<<(leg2->p4()).E()<<"/"<<leg2->p()<<"/"<<leg2->pt()<<"/"<<leg2->eta()<<"/"<<leg2->phi()<<"/"<<leg2->mass()<<endl;
    cout<<"measuredMET = "<<measuredMET<<endl;
    cout<<"MET covariance matrix: "<<endl;
    cout<<"            (0,0) = "<<cov(0,0)<<endl;
    cout<<"            (0,1) = "<<cov(0,1)<<endl;
    cout<<"            (1,0) = "<<cov(1,0)<<endl;
    cout<<"            (1,1) = "<<cov(1,1)<<endl;
    cout<<"SVfitMass = "<<diTauNSVfitMass_<<endl;
    cout<<"****** END SVFIT ********"<<endl;
    */

    svFitStandalone::LorentzVector leg1P4(leg1->px(), leg1->py(), leg1->pz(), leg1->energy());
    svFitStandalone::LorentzVector leg2P4(leg2->px(), leg2->py(), leg2->pz(), leg2->energy());
    std::vector<svFitStandalone::MeasuredTauLepton> measuredTauLeptons_new;
    measuredTauLeptons_new.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToMuDecay, leg1->pt(), leg1->eta(),  leg1->phi(), svFitStandalone::muonMass)); // tau -> electron decay (Pt, eta, phi, mass)
    measuredTauLeptons_new.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg2->pt(), leg2->eta(),  leg2->phi(), leg2->mass())); // tau -> tau_h decay (Pt, eta, phi, mass)
//     measuredTauLeptons_new.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToMuDecay, leg1P4));
//     measuredTauLeptons_new.push_back(svFitStandalone::MeasuredTauLepton(svFitStandalone::kTauToHadDecay, leg2P4));
    SVfitStandaloneAlgorithm algo(measuredTauLeptons_new, theDiTau->met()->p4().Px(), theDiTau->met()->p4().Py(), cov, 2);
    algo.addLogM(true, 2.);
    algo.integrateVEGAS();

    /*
    //const reco::MET* met = theDiTau->met();
    svFitStandalone::Vector measuredMET( theDiTau->met()->p4().Px(), theDiTau->met()->p4().Py(), 0); 
    SVfitStandaloneAlgorithm svFitAlgorithm(measuredTauLeptons_new, measuredMET, cov, 2);
    svFitAlgorithm.addLogM(true, 5.);
    svFitAlgorithm.integrateVEGAS();
    */
    diTauNSVfitMass_ = algo.getMass(); 
    diTauNSVfitMassErrUp_ = algo.massUncert(); 
    diTauNSVfitPt_ = algo.pt();  
    diTauNSVfitPtErrUp_ = algo.ptUncert();
    
    
    std::cout << "leg1->px() "<< std::setprecision(10) << leg1->px() << endl;
    std::cout << "leg1->py() "<< std::setprecision(10) << leg1->py() << endl;
    std::cout << "leg1->pz() "<< std::setprecision(10) << leg1->pz() << endl;
    std::cout << "leg1->energy() "<< std::setprecision(10) << leg1->energy() << endl;

    std::cout << "leg2->px() "<< std::setprecision(10) << leg2->px() << endl;
    std::cout << "leg2->py() "<< std::setprecision(10) << leg2->py() << endl;
    std::cout << "leg2->pz() "<< std::setprecision(10) << leg2->pz() << endl;
    std::cout << "leg2->energy() "<< std::setprecision(10) << leg2->energy() << endl;
    
    std::cout<< "theDiTau->met()->p4().Px() = " << std::setprecision(10) << theDiTau->met()->p4().Px() << endl;
    std::cout<< "theDiTau->met()->p4().Py() = " << std::setprecision(10) << theDiTau->met()->p4().Py() << endl;
    std::cout<< "theDiTau->met()->p4().Pt() = " << std::setprecision(10) << theDiTau->met()->p4().Pt() << endl;
    std::cout<< " sqrt(Px*Px+Py*Py) = " << std::setprecision(10) << sqrt(theDiTau->met()->p4().Px()*theDiTau->met()->p4().Px()+theDiTau->met()->p4().Py()*theDiTau->met()->p4().Py())<<endl;
    std::cout<< "theDiTau->met()->p4().Eta() = " << std::setprecision(10) << theDiTau->met()->p4().Eta() << endl;
    std::cout<< "theDiTau->met()->p4().Phi() = " << std::setprecision(10) << theDiTau->met()->p4().Phi() << endl;

    cout<<"MET covariance matrix: "<<endl;
    cout<<"            (0,0) = "<<std::setprecision(10) <<cov(0,0)<<endl;
    cout<<"            (0,1) = "<<std::setprecision(10) <<cov(0,1)<<endl;
    cout<<"            (1,0) = "<<std::setprecision(10) <<cov(1,0)<<endl;
    cout<<"            (1,1) = "<<std::setprecision(10) <<cov(1,1)<<endl;

    cout<<"--> New svFitMass = "<<std::setprecision(10) <<algo.getMass()<<endl;
    cout<<"--> New svFitMass error = "<<std::setprecision(10) <<algo.massUncert()<<endl;
    
    /*
    if ( svFitAlgorithm.isValidSolution() )
      {
	cout<<"****** NEW SVFIT ********"<<endl;

	cout<<"- mu     E/p/pt/eta/phi/m = "<<(leg1->p4()).E()<<"/"<<leg1->p()<<"/"<<leg1->pt()<<"/"<<leg1->eta()<<"/"<<leg1->phi()<<"/"<<leg1->mass()<<endl;
	cout<<"- tau    E/p/pt/eta/phi/m = "<<(leg2->p4()).E()<<"/"<<leg2->p()<<"/"<<leg2->pt()<<"/"<<leg2->eta()<<"/"<<leg2->phi()<<"/"<<leg2->mass()<<endl;
	cout<<"measuredMET = "<<measuredMET<<endl;
	cout<<"MET covariance matrix: "<<endl;
	cout<<"            (0,0) = "<<cov(0,0)<<endl;
	cout<<"            (0,1) = "<<cov(0,1)<<endl;
	cout<<"            (1,0) = "<<cov(1,0)<<endl;
	cout<<"            (1,1) = "<<cov(1,1)<<endl;
	cout<<"--> New svFitMass = "<<svFitAlgorithm.getMass()<<endl;
	cout<<"--> New svFitMass error = "<<svFitAlgorithm.massUncert()<<endl;
	cout<<"****** END NEW SVFIT ********"<<endl;

// 	setValueF("svfitMass", svFitAlgorithm.getMass());     // CV: this is how I set the svFit mass branch in my Ntuple; please adapt to your Ntuple filling code
// 	setValueF("svfitMassErr", svFitAlgorithm.massUncert());
      }
    */


  
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
    //////////////////////
    // Quark/gluon id
    std::map<double, std::vector<float> ,      MuTauStreamAnalyzer::more> quarkGluonID;
    std::map<double, std::vector<float> ,      MuTauStreamAnalyzer::more> quarkGluonIDGen;
    //////////////////////  

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
      if(fabs(newJet->eta())>5.2) continue ;
      edm::ESHandle<JetCorrectorParametersCollection> jetCorrParameters;
      // get the jet corrector parameters collection from the global tag
      iSetup.get<JetCorrectionsRecord>().get("AK5PF", jetCorrParameters);
      // get the uncertainty parameters from the collection
      JetCorrectorParameters const & param = (*jetCorrParameters)["Uncertainty"];
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(param);
//       cout<<"JET ETA = "<<newJet->eta()<<endl;
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
	//std::cout << "L1Offset ========> " << std::endl;
	//std::cout << "Uncorrected " << newJet->jecFactor("Uncorrected","none","patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	//std::cout << "L1Offset"     << newJet->jecFactor("L1Offset","none",   "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	//std::cout << "L2Relative "  << newJet->jecFactor("L2Relative","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;
	//std::cout << "L3Absolute"   << newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() << std::endl;  
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
      //bQuark.insert(       make_pair( newJet->p4().Pt(), (jet->genParticleById(5,0,true)).isNonnull()  ) );

      /*
      myfile<<"jet #"<<it<<endl;
      myfile<<"jet pt        = "<<newJet->p4().Pt()<<endl;
      myfile<<"jet eta       = "<<newJet->p4().Eta()<<endl;
      myfile<<"jet phi       = "<<newJet->p4().Phi()<<endl;
      myfile<<"partonFlavour = "<<jet->partonFlavour()<<endl;
      */

      bQuark.insert(       make_pair( newJet->p4().Pt(), (jet->partonFlavour())  ) );

      /*
      myfile<<"-"<<endl;
      */

      // add pu information
      jetPVassociation.insert( make_pair( newJet->p4().Pt(), make_pair(aMap["chFracRawJetE"],
								       aMap["chFracAllChargE"]) ) );
      // add jet moments
      jetMoments.insert(       make_pair( newJet->p4().Pt(), make_pair( jet->etaetaMoment(),
									jet->phiphiMoment()) ) );
      
      /*if(isMC_) 
	sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
						newJet->jecFactor("L3Absolute","none", "patJetCorrFactorsL1Offset")*newJet->p4()) );   
      else 
	sortedJetsIDL1Offset.insert( make_pair( newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->pt() , 
						newJet->jecFactor("L2L3Residual","none", "patJetCorrFactorsL1Offset")*newJet->p4()) ); 
      */
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

      //////////////////////
      // Quark/gluon id
      edm::RefToBase<reco::Jet> jetRef(edm::Ref<pat::JetCollection>(jetsHandle,it));    
      float mlp(-1), likelihood(-1) ;
      if (QGTagsHandleMLP.isValid()) mlp = (*QGTagsHandleMLP)[jetRef] ;
      if (QGTagsHandleLikelihood.isValid()) likelihood = (*QGTagsHandleLikelihood)[jetRef] ;
      if(verbose_) std::cout<<"==> pt ="<<jet->pt()<<" "<<newJet->p4().Pt()<<" mlp="<<mlp<<" likelihood="<<likelihood<<std::endl ;
      vector<float> quarkGluonDiscriminator ;
      quarkGluonDiscriminator.push_back(jet->pt()) ;
      quarkGluonDiscriminator.push_back(mlp);
      quarkGluonDiscriminator.push_back(likelihood);

      ////// Compute input variables of likelihood
      edm::Handle<double> rhoHandle;
      iEvent.getByLabel(edm::InputTag("kt6PFJetsQG","rho"), rhoHandle);
      rho_ = rhoHandle.isValid() ? (*rhoHandle) : -99;

      edm::Handle<double> rhoisoHandle;
      iEvent.getByLabel(edm::InputTag("kt6PFJetsIsoQG","rho"), rhoisoHandle);
      rhoiso_ = rhoisoHandle.isValid() ? (*rhoisoHandle) : -99;

      edm::Handle<reco::VertexCollection> vC_likelihood;
      iEvent.getByLabel("offlinePrimaryVertices", vC_likelihood);
     
      map<string,float> likelihoodvariables;
      if(newJet->isPFJet()){
	likelihoodvariables=calcLikelihoodVariables(&*newJet, vC_likelihood, "Likelihood");
	likelihoodvariables["pt"]=newJet->pt();
	likelihoodvariables["rho"]=rho_;
	likelihoodvariables["rhoiso"]=rhoiso_;
      }
      quarkGluonDiscriminator.push_back(likelihoodvariables["pt"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["eta"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["rho"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["rhoiso"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["ptD"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["axis1"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["axis2"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["mult"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["nChg_QC"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["nChg_ptCut"]);
      quarkGluonDiscriminator.push_back(likelihoodvariables["nNeutral_ptCut"]);

      quarkGluonID.insert(  make_pair( newJet->p4().Pt() ,  quarkGluonDiscriminator ) );

      // generator part
      vector<float> quarkGluonDiscriminatorGen ;
      if(newJet->genJet()==0){
	quarkGluonDiscriminatorGen.push_back(-99);
	quarkGluonDiscriminatorGen.push_back(-99);
	quarkGluonDiscriminatorGen.push_back(-99);
	quarkGluonDiscriminatorGen.push_back(-99);
      }
      if(newJet->genJet()!=0){
	quarkGluonDiscriminatorGen.push_back(newJet->partonFlavour());
	quarkGluonDiscriminatorGen.push_back((newJet->genJet()->getGenConstituents()).size());
	float leadGenPartPdg = (newJet->genJet()->getGenConstituents()).size()>0 ?
	  ((newJet->genJet()->getGenConstituents())[0])->pdgId() : -99;
	quarkGluonDiscriminatorGen.push_back(leadGenPartPdg) ;
	float leadGenPartPt  = (newJet->genJet()->getGenConstituents()).size()>0 ?
	  ((newJet->genJet()->getGenConstituents())[0])->pt() : -99;
	quarkGluonDiscriminatorGen.push_back(leadGenPartPt) ;		
	if(verbose_){
	  cout << "parton flavor = " << newJet->partonFlavour() << endl;
	  for(unsigned int l = 0; l < (newJet->genJet()->getGenConstituents()).size() ; l++){
	    cout << "Hadron " << l 
		 << " pdg " << ((newJet->genJet()->getGenConstituents())[l])->pdgId() << ", pt " << ((newJet->genJet()->getGenConstituents())[l])->pt()
		 << endl;
	  }
	}
      }
      quarkGluonIDGen.insert(  make_pair( newJet->p4().Pt() ,  quarkGluonDiscriminatorGen ) );
      ////////////////////// 


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


    /*
    //Code from Christian using PFJetIDSelectionFunctor
    for(unsigned int it = 0; it < jets->size() ; it++){
      
      pat::Jet* jet = const_cast<pat::Jet*>(&(*jets)[it]);

      bool passesLoosePFJetId = (*loosePFJetIdAlgo_)(*jet);
      if ( !passesLoosePFJetId ) continue;
      
      int puJetIdFlag = (*puJetIdFlags)[jet->originalObjectRef()];
      bool passesPileupJetId = PileupJetIdentifier::passJetId(puJetIdFlag, PileupJetIdentifier::kLoose);
      if ( !passesPileupJetId ) continue;
      
      // CV: remove jets overlapping with tau decay products
      double dRleg1Jet = deltaR(leg1->p4(), jet->p4());
      double dRleg2Jet = deltaR(leg2->p4(), jet->p4());
      if ( dRleg1Jet < 0.5 || dRleg2Jet < 0.5 ) continue;
      
      double jetPt = jet->pt();
      double jetAbsEta = TMath::Abs(jet->eta());

      sortedJets_PFJetIDSelectionFunctor.insert( make_pair( newJet->correctedJet("Uncorrected").p4().Pt() ,newJet->correctedJet("Uncorrected").p4() ) );
      
      if ( jetPt > minJetPt_ && jetAbsEta < 4.7 ) {
	jetsP4_PFJetIDSelectionFunctor_.push_back(&(*jet));
      }
      if ( jetPt > minBJetPt_ && jetAbsEta < 2.4 && jet->bDiscriminator(bJetDiscriminator_) > wpBJetDiscriminator_ ) {      
	bJets_sorted.push_back(&(*jet));
      }
       
    }
    */
  

    //here Olivier create the new jet collections!!!

    

    //////////////////////
    // Quark/gluon id
    for(std::map<double, std::vector<float> >::iterator it = quarkGluonID.begin(); it != quarkGluonID.end() ; it++){
      for(unsigned int j=0; j!=(it->second).size(); j++)
	jetQuarkGluon_->push_back( (it->second)[j] );
      // 0=pt , 1=mlp , 2=likelihood
      //likelihood variables 3=pt , 4=eta , 5=rho , 6=rhoiso , 7=ptd , 8=axis1 , 9=axis2 , 10=mult , 11=nChg_QC , 12=nChg_ptCut , 13=nNeutral_ptCut
    }
    for(std::map<double, std::vector<float> >::iterator it = quarkGluonIDGen.begin(); it != quarkGluonIDGen.end() ; it++){
      if ( !(it->second).empty() ) {
	jetQuarkGluonGen_->push_back( (it->second)[0] ); //flavor
	jetQuarkGluonGen_->push_back( (it->second)[1] ); //nb of constituents
	jetQuarkGluonGen_->push_back( (it->second)[2] ); //leadGenPartPdg
	jetQuarkGluonGen_->push_back( (it->second)[3] ); //leadGenPartPt
      }
    }
//     //////////////////////
//     // Higgs pT Reweighting///IN done at the treeSkimmer level

//     double higgsPtWeightNom  = 1.;
//     double higgsPtWeightUp   = 1.;
//     double higgsPtWeightDown = 1.;
//     bool isHiggs = false;
//     if ( isMC_ ) {
//       //       edm::Handle<reco::GenParticleCollection> genParticles;
//       //       evt.getByLabel(srcGenParticles_, genParticles);

//       reco::Candidate::LorentzVector higgsP4;
//       isHiggs = findGenParticle(*genParticles, 25, 35, 36, higgsP4);
//       if ( isHiggs ) {
// 	double higgsMass = higgsP4.mass();
// 	std::string inputFileName;
// 	if      ( higgsMass >=  90. && higgsMass <= 110. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH100_8TeV.root";
// 	else if ( higgsMass >= 115. && higgsMass <= 135. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH125_8TeV.root";
// 	else if ( higgsMass >= 140. && higgsMass <= 150. ) inputFileName = "LLRAnalysis/HadTauStudies/data/HqTWeights/HRes_weight_pTH_mH150_8TeV.root";
// 	if ( inputFileName != "" ) {
// 	  if ( inputFileName != lastInputFileHiggsPtWeight_ || !inputFileHiggsPtWeight_ ) {
// 	    inputFileHiggsPtWeight_ = openFile(edm::FileInPath(inputFileName));
// 	    lutHiggsPtWeightNom_  = loadLUT(inputFileHiggsPtWeight_, "Nominal");
// 	    lutHiggsPtWeightUp_   = loadLUT(inputFileHiggsPtWeight_, "Up");
// 	    lutHiggsPtWeightDown_ = loadLUT(inputFileHiggsPtWeight_, "Down");
// 	    lastInputFileHiggsPtWeight_ = inputFileName;
// 	  }
// 	  double higgsPt = higgsP4.pt();
// 	  higgsPtWeightNom = compHiggsPtWeight(lutHiggsPtWeightNom_, higgsPt);
// 	  higgsPtWeightUp = compHiggsPtWeight(lutHiggsPtWeightUp_, higgsPt);
// 	  higgsPtWeightDown = compHiggsPtWeight(lutHiggsPtWeightDown_, higgsPt);
// 	}
// // 	std::cout<<"HqtWeight file :" <<inputFileName<<std::endl;
//       }
//     }

//     higgsPtWeightNom_ = higgsPtWeightNom ;
//     higgsPtWeightUp_ = higgsPtWeightUp ;
//     higgsPtWeightDown_ = higgsPtWeightDown ;

// //     std::cout<<"HqtWeight :" <<higgsPtWeightNom_<<std::endl;

    //////////////////////IN
    // Top pT Reweighting
    if ( isMC_ ) {

      if(genHandleForTopPtReweighting.isValid()) genParticlesForTopPtReweighting = genHandleForTopPtReweighting.product();
          
      reco::Candidate::LorentzVector topPlusP4; 
      bool topPlusFound = findGenParticle(*genParticlesForTopPtReweighting, +6, 0, 0, topPlusP4); 
      reco::Candidate::LorentzVector topMinusP4; 
      bool topMinusFound = findGenParticle(*genParticlesForTopPtReweighting, -6, 0, 0, topMinusP4); 

      topPtWeightNom_ = 1.; 
      topPtWeightUp_ = 1.; 
      topPtWeightDown_ = 1.; 
      if ( topPlusFound && topMinusFound ) {
	gentopP4_->push_back( topPlusP4 );
	gentopP4_->push_back( topMinusP4 );
	topPtWeightNom_ = compTopPtWeight(topPlusP4.pt(), topMinusP4.pt()); 
	topPtWeightUp_ = topPtWeightNom_*topPtWeightNom_; // CV: assign 100% systematic uncertainty to top quark Pt reweighting, 
	topPtWeightDown_ = 1.0; // following https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting } 
      } 
      if(verbose_){
	cout<<" topPlusFound: "<<topPlusFound<<endl;
	cout<<" topMinusFound: "<<topMinusFound<<endl;
	cout<<" topPtWeightNom: "<<topPtWeightNom_<<endl;
	cout<<" topPtWeightUp: "<<topPtWeightUp_<<endl;
	cout<<" topPtWeightDown: "<<topPtWeightDown_<<endl;
      }
    }
    //////////////////////
    // Embedding weights

    edm::Handle<double> TauSpinnerHandle;
    double TauSpinnerWeight = 1.0;

    edm::Handle<double> ZmumuEffHandle;
    double ZmumuEffWeight = 1.0;

    edm::Handle<double> diTauMassVSdiTauPtHandle;
    double diTauMassVSdiTauPtWeight = 1.0;

    edm::Handle<double> tau2EtaVStau1EtaHandle;
    double tau2EtaVStau1EtaWeight = 1.0;

    edm::Handle<double> tau2PtVStau1PtHandle;
    double tau2PtVStau1PtWeight = 1.0;

    edm::Handle<double> muonRadiationHandle;
    double muonRadiationWeight = 1.0;

    edm::Handle<double> muonRadiationDownHandle;
    double muonRadiationDownWeight = 1.0;

    edm::Handle<double> muonRadiationUpHandle;
    double muonRadiationUpWeight = 1.0;

    edm::Handle< std::vector<reco::CompositeCandidate> > genDiTauHandle;
    genDiTauMass_ = 9999;

    iEvent.getByLabel(edm::InputTag("TauSpinnerReco","TauSpinnerWT"), TauSpinnerHandle);
    TauSpinnerWeight = TauSpinnerHandle.isValid() ? (*TauSpinnerHandle) : 1.0;
    //cout<<"TauSpinner weight: "<<TauSpinnerWeight<<endl;

    if (isRhEmb_){
      iEvent.getByLabel("genZdecayToTausForEmbeddingKineReweight", genDiTauHandle);      
      iEvent.getByLabel(edm::InputTag("TauSpinnerReco","TauSpinnerWT"), TauSpinnerHandle);
      iEvent.getByLabel(edm::InputTag("ZmumuEvtSelEffCorrWeightProducer","weight"), ZmumuEffHandle);
      if(isMC_){
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightGENembedding","genDiTauMassVsGenDiTauPt"), diTauMassVSdiTauPtHandle);
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightGENembedding","genTau2EtaVsGenTau1Eta"), tau2EtaVStau1EtaHandle);
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightGENembedding","genTau2PtVsGenTau1Pt"), tau2PtVStau1PtHandle);
      }
      else{
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightRECembedding","genDiTauMassVsGenDiTauPt"), diTauMassVSdiTauPtHandle);
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightRECembedding","genTau2EtaVsGenTau1Eta"), tau2EtaVStau1EtaHandle);
	iEvent.getByLabel(edm::InputTag("embeddingKineReweightRECembedding","genTau2PtVsGenTau1Pt"), tau2PtVStau1PtHandle);
      }
      iEvent.getByLabel(edm::InputTag("muonRadiationCorrWeightProducer","weight"), muonRadiationHandle);
      iEvent.getByLabel(edm::InputTag("muonRadiationCorrWeightProducer","weightDown"), muonRadiationDownHandle);
      iEvent.getByLabel(edm::InputTag("muonRadiationCorrWeightProducer","weightUp"), muonRadiationUpHandle);

      TauSpinnerWeight = TauSpinnerHandle.isValid() ? (*TauSpinnerHandle) : 1.0;
      ZmumuEffWeight   = ZmumuEffHandle.isValid() ? (*ZmumuEffHandle) : 1.0;
      diTauMassVSdiTauPtWeight = diTauMassVSdiTauPtHandle.isValid() ? (*diTauMassVSdiTauPtHandle) : 1.0;
      tau2EtaVStau1EtaWeight = tau2EtaVStau1EtaHandle.isValid() ? (*tau2EtaVStau1EtaHandle) : 1.0;
      tau2PtVStau1PtWeight = tau2PtVStau1PtHandle.isValid() ? (*tau2PtVStau1PtHandle) : 1.0;
      muonRadiationWeight = muonRadiationHandle.isValid() ? (*muonRadiationHandle) : 1.0;
      muonRadiationDownWeight = muonRadiationDownHandle.isValid() ? (*muonRadiationDownHandle) : 1.0;
      muonRadiationUpWeight = muonRadiationUpHandle.isValid() ? (*muonRadiationUpHandle) : 1.0;
      genDiTauMass_ = genDiTauHandle.isValid() && genDiTauHandle->size()>0 ? genDiTauHandle->at(0).mass() : 9999;
    }
    if(verbose_){
      cout<<"TauSpinner weight: "<<TauSpinnerWeight<<endl;
      cout<<"ZmumuEff weight: "<<ZmumuEffWeight<<endl;
      cout<<"diTauMassVSdiTauPt weight: "<<diTauMassVSdiTauPtWeight<<endl;
      cout<<"tau2EtaVStau1Eta weight: "<<tau2EtaVStau1EtaWeight<<endl;
      cout<<"tau2PtVStau1Pt weight: "<<tau2PtVStau1PtWeight<<endl;
      cout<<"muonRadiation weight: "<<muonRadiationWeight<<endl;
      cout<<"muonRadiationDown weight: "<<muonRadiationDownWeight<<endl;
      cout<<"muonRadiationUp weight: "<<muonRadiationUpWeight<<endl;
    }
    embeddingWeights_->push_back(TauSpinnerWeight);
    embeddingWeights_->push_back(ZmumuEffWeight);
    embeddingWeights_->push_back(diTauMassVSdiTauPtWeight);
    embeddingWeights_->push_back(tau2EtaVStau1EtaWeight);
    embeddingWeights_->push_back(tau2PtVStau1PtWeight);
    embeddingWeights_->push_back(muonRadiationWeight);
    embeddingWeights_->push_back(muonRadiationDownWeight);
    embeddingWeights_->push_back(muonRadiationUpWeight);

    TauSpinnerWeight_ = TauSpinnerWeight ; 

    tree_->Fill();
    
  }// The loop over ditaus
  
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

map<string,float> MuTauStreamAnalyzer::calcLikelihoodVariables(const pat::Jet *jet, edm::Handle<reco::VertexCollection> vC, string type){
 
  static map<string,float> variables;
  variables["eta"] = jet->eta();
  Bool_t useQC = true;
  if(fabs(jet->eta()) > 2.5 && type == "MLP") useQC = false;		//In MLP: no QC in forward region

  reco::VertexCollection::const_iterator vtxLead = vC->begin();

  Float_t sum_weight = 0., sum_deta = 0., sum_dphi = 0., sum_deta2 = 0., sum_dphi2 = 0., sum_detadphi = 0., sum_pt = 0.;
  Int_t nChg_QC = 0, nChg_ptCut = 0, nNeutral_ptCut = 0;

  //Loop over the jet constituents
  std::vector<reco::PFCandidatePtr> constituents = jet->getPFConstituents();
  for(unsigned i = 0; i < constituents.size(); ++i){
    reco::PFCandidatePtr part = jet->getPFConstituent(i);      
    if(!part.isNonnull()) continue;

    reco::TrackRef itrk = part->trackRef();;

    bool trkForAxis = false;
    if(itrk.isNonnull()){						//Track exists --> charged particle
      if(part->pt() > 1.0) nChg_ptCut++;

      //Search for closest vertex to track
      reco::VertexCollection::const_iterator vtxClose = vC->begin();
      for(reco::VertexCollection::const_iterator vtx = vC->begin(); vtx != vC->end(); ++vtx){
        if(fabs(itrk->dz(vtx->position())) < fabs(itrk->dz(vtxClose->position()))) vtxClose = vtx;
      }

      if(vtxClose == vtxLead){
        Float_t dz = itrk->dz(vtxClose->position());
        Float_t dz_sigma = sqrt(pow(itrk->dzError(),2) + pow(vtxClose->zError(),2));
	      
        if(itrk->quality(reco::TrackBase::qualityByName("highPurity")) && fabs(dz/dz_sigma) < 5.){
          trkForAxis = true;
          Float_t d0 = itrk->dxy(vtxClose->position());
          Float_t d0_sigma = sqrt(pow(itrk->d0Error(),2) + pow(vtxClose->xError(),2) + pow(vtxClose->yError(),2));
          if(fabs(d0/d0_sigma) < 5.) nChg_QC++;
        }
      }
    } else {								//No track --> neutral particle
      if(part->pt() > 1.0) nNeutral_ptCut++;
      trkForAxis = true;
    }
	  
    Float_t deta = part->eta() - jet->eta();
    Float_t dphi = 2*atan(tan(((part->phi()-jet->phi()))/2));           
    Float_t partPt = part->pt(); 
    Float_t weight = partPt*partPt;

    if(!useQC || trkForAxis){					//If quality cuts, only use when trkForAxis
      sum_weight += weight;
      sum_pt += partPt;
      sum_deta += deta*weight;                  
      sum_dphi += dphi*weight;                                                                                             
      sum_deta2 += deta*deta*weight;                    
      sum_detadphi += deta*dphi*weight;                               
      sum_dphi2 += dphi*dphi*weight;
    }	
  }

  //Calculate axis and ptD
  Float_t a = 0., b = 0., c = 0.;
  Float_t ave_deta = 0., ave_dphi = 0., ave_deta2 = 0., ave_dphi2 = 0.;
  if(sum_weight > 0){
    variables["ptD"] = sqrt(sum_weight)/sum_pt;
    ave_deta = sum_deta/sum_weight;
    ave_dphi = sum_dphi/sum_weight;
    ave_deta2 = sum_deta2/sum_weight;
    ave_dphi2 = sum_dphi2/sum_weight;
    a = ave_deta2 - ave_deta*ave_deta;                          
    b = ave_dphi2 - ave_dphi*ave_dphi;                          
    c = -(sum_detadphi/sum_weight - ave_deta*ave_dphi);                
  } else variables["ptD"] = 0;
  Float_t delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
  if(a+b+delta > 0) variables["axis1"] = sqrt(0.5*(a+b+delta));
  else variables["axis1"] = 0.;
  if(a+b-delta > 0) variables["axis2"] = sqrt(0.5*(a+b-delta));
  else variables["axis2"] = 0.;

  if(type == "MLP" && useQC) variables["mult"] = nChg_QC;
  else if(type == "MLP") variables["mult"] = (nChg_ptCut + nNeutral_ptCut);
  else variables["mult"] = (nChg_QC + nNeutral_ptCut);

  variables["nChg_QC"]=nChg_QC;
  variables["nChg_ptCut"]=nChg_ptCut;
  variables["nNeutral_ptCut"]=nNeutral_ptCut;

  return variables;

}

void MuTauStreamAnalyzer::endJob(){
  //myfile.close();
}


#include "FWCore/Framework/interface/MakerMacros.h"
 
DEFINE_FWK_MODULE(MuTauStreamAnalyzer);


