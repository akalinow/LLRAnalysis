#include "TTree.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "THStack.h"
#include "TBenchmark.h"
#include "TGraph.h"
#include "TVectorT.h"
#include "TMultiGraph.h"
#include "TBranch.h"
#include "TSystem.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

#include "AntiElectronIDMVA.h"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>

#define SAVE   true
#define MINPt1 20.0 
#define MINPt2 20.0
#define PtVETO 30.0
#define MAXEta  4.5 
#define USERECOILALGO true

using namespace ROOT::Math;
using namespace std;

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;


float* computeZeta(LV leg1, LV leg2, LV MEt){

  float* result = new float[2];

  float cos1 = leg1.Px()/leg1.Pt();
  float cos2 = leg2.Px()/leg2.Pt();
  float sen1 = leg1.Py()/leg1.Pt();
  float sen2 = leg2.Py()/leg2.Pt();

  float bisecX = cos1 + cos2;
  float bisecY = sen1 + sen2;

  float norm = TMath::Sqrt(bisecX*bisecX + bisecY*bisecY);

  if(norm>0.){
    bisecX /= norm;
    bisecY /= norm;
  }

  float pZetaVis = (leg1+leg2).Px()*bisecX + (leg1+leg2).Py()*bisecY;
  float pZeta    = (leg1+leg2+MEt).Px()*bisecX + (leg1+leg2+MEt).Py()*bisecY; 

  result[0] = pZetaVis;
  result[1] = pZeta;

  return result;
}


float pileupWeight( int intimepileup_ ){

  std::vector< float > Summer11Lumi ;
  Double_t Summer11Lumi_f[35] = {
    1.45346E-01,
    6.42802E-02,
    6.95255E-02,
    6.96747E-02,
    6.92955E-02,
    6.84997E-02,
    6.69528E-02,
    6.45515E-02,
    6.09865E-02,
    5.63323E-02,
    5.07322E-02,
    4.44681E-02,
    3.79205E-02,
    3.15131E-02,
    2.54220E-02,
    2.00184E-02,
    1.53776E-02,
    1.15387E-02,
    8.47608E-03,
    6.08715E-03,
    4.28255E-03,
    2.97185E-03,
    2.01918E-03,
    1.34490E-03,
    8.81587E-04,
    5.69954E-04,
    3.61493E-04,
    2.28692E-04,
    1.40791E-04,
    8.44606E-05,
    5.10204E-05,
    3.07802E-05,
    1.81401E-05,
    1.00201E-05,
    5.80004E-06
  };

  std::vector< float > Data2011Lumi;
  Double_t Data2011Lumi_f[35] = {
    0.00290212,
    0.0123985,
    0.0294783,
    0.0504491,
    0.0698525,
    0.0836611,
    0.0905799,
    0.0914388,
    0.0879379,
    0.0817086,
    0.073937,
    0.0653785,
    0.0565162,
    0.047707,
    0.0392591,
    0.0314457,
    0.0244864,
    0.018523,
    0.013608,
    0.00970977,
    0.00673162,
    0.00453714,
    0.00297524,
    0.00189981,
    0.00118234,
    0.000717854,
    0.00042561,
    0.000246653,
    0.000139853,
    7.76535E-05,
    4.22607E-05,
    2.25608E-05,
    1.18236E-05,
    6.0874E-06,
    6.04852E-06
  };

  for( int i=0; i<35; i++) {
    Data2011Lumi.push_back(Data2011Lumi_f[i]);
    Summer11Lumi.push_back(Summer11Lumi_f[i]);
  }
  
  int intimepileup = intimepileup_>34 ? 35 : intimepileup_;

  return Data2011Lumi[intimepileup]/Summer11Lumi[intimepileup];

}






void makeTrees_ElecTauStream(string analysis = "", int index = -1 , AntiElectronIDMVA* antiE=0){

  if(!antiE){
    cout << "Anti electron mva not initlaized!" << endl;
    return;
  }

  cout << "Now skimming analysys " << analysis << endl;

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TFile corrections("llrCorrections.root");

  TF1 *ratioElecID        = (TF1*)corrections.Get("ratioElecID");
  TF1 *ratioElecIso       = (TF1*)corrections.Get("ratioElecIso");
  TF1 *ratioTauElecTauAll = (TF1*)corrections.Get("ratioTauElecTauAll");
  TF1 *ratioElecAllBL     = (TF1*)corrections.Get("ratioElecAllBL");
  TF1 *ratioElecAllEC     = (TF1*)corrections.Get("ratioElecAllEC");
  TF1 *turnOnTauElecTauAll= (TF1*)corrections.Get("turnOnTauElecTauAll");
  TF1 *turnOnElecAllBL    = (TF1*)corrections.Get("turnOnElecAllBL");
  TF1 *turnOnElecAllEC    = (TF1*)corrections.Get("turnOnElecAllEC");

 

  std::vector<std::string> samples;
  std::vector<float> crossSec;
  std::vector<int>   readEvents;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011-ElecTau-All_run");             crossSec.push_back( 0  );                          
  samples.push_back("Run2011A-ElecTau-All-Embedded_run");   crossSec.push_back( 0  );                          
  samples.push_back("DYJets-ElecTau-50-madgraph-PUS4_run"); crossSec.push_back( 3048           * 0.0524629   * 0.2615988); 
  samples.push_back("TTJets-ElecTau-madgraph-PUS4_run");    crossSec.push_back( 157.5          * 0.0141612   * 0.7835152);  
  samples.push_back("WJets-ElecTau-madgraph-PUS4_run");     crossSec.push_back(31314.0         * 0.0010836   * 0.5558882);   
  samples.push_back("WZ-ElecTau-pythia-PUS4_run");          crossSec.push_back( 18.2           * 0.0108603   * 0.5423515);         
  samples.push_back("ZZ-ElecTau-pythia-PUS4_run");          crossSec.push_back(  5.9           * 0.0155201   * 0.5160835);    
  samples.push_back("WW-ElecTau-pythia-PUS4_run");          crossSec.push_back( 43.0           * 0.0089326   * 0.5763673);        
  samples.push_back("VBFH100-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*1.546 * 0.0501993   * 0.7323465); 
  samples.push_back("GGFH100-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*24.02 * 0.0375728   * 0.6978973); 
  samples.push_back("VBFH105-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*1.472 * 1.0         * 0.1166750); 
  samples.push_back("GGFH105-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*21.78 * 0.0400086   * 0.7071616); 
  samples.push_back("VBFH110-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*1.398 * 0.0564719   * 0.7356773);
  samples.push_back("GGFH110-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*19.84 * 0.0444077   * 0.7002590);    
  samples.push_back("VBFH115-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*1.332 * 0.0593790   * 0.7374150);
  samples.push_back("GGFH115-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*18.13 * 0.0470898   * 0.7092235);  
  samples.push_back("VBFH120-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*1.269 * 0.0618780   * 0.7267106);   
  samples.push_back("GGFH120-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*16.63 * 0.0491247   * 0.7053135);  
  samples.push_back("VBFH125-ElecTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*1.211 * 0.0651673   * 0.7365371);   
  samples.push_back("GGFH125-ElecTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*15.31 * 0.0527237   * 0.7103566); 
  samples.push_back("VBFH130-ElecTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*1.154 * 1.0         * 0.1365724); 
  samples.push_back("GGFH130-ElecTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*14.12 * 0.055105    * 0.7021458);
  samples.push_back("VBFH135-ElecTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*1.100 * 0.0716975   * 0.7349245); 
  samples.push_back("GGFH135-ElecTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*13.08 * 1.0         * 0.0929491);    
  samples.push_back("VBFH140-ElecTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*1.052 * 0.0730735   * 0.7398515);  
  samples.push_back("GGFH140-ElecTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*12.13 * 0.0608360   * 0.7085120);  
  samples.push_back("VBFH145-ElecTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*1.004 * 0.0753342   * 0.7420528);  
  samples.push_back("GGFH145-ElecTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*11.27 * 0.0624890   * 0.7049520);  
  samples.push_back("VH100-ElecTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 1.0 * 0.763301);  
//samples.push_back("VH105-ElecTau-pythia-PUS4_run");  crossSec.push_back( 8.25e-02*(1.018+ 0.5449+0.1433  ) * 1.0 * 0.); 
  samples.push_back("VH110-ElecTau-pythia-PUS4_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 1.0 * 0.765459);
  samples.push_back("VH115-ElecTau-pythia-PUS4_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 1.0 * 0.7647);
  samples.push_back("VH120-ElecTau-pythia-PUS4_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0 * 0.7723370);   
  samples.push_back("VH125-ElecTau-pythia-PUS4_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 1.0 * 0.77177510);
  samples.push_back("VH130-ElecTau-pythia-PUS4_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 1.0 * 0.76862474);
  samples.push_back("VH135-ElecTau-pythia-PUS4_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 1.0 * 0.77288772);
  samples.push_back("VH140-ElecTau-pythia-PUS4_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 1.0 * 0.77165560);  
  samples.push_back("VH145-ElecTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 1.0 * 0.77817174);  
  

  // normalization Lumi
  Float_t Lumi=1000;

  string currentSample = index >= 0 ? samples[index] : "Debug";
  TString sample(currentSample.c_str());

  TString outName = "nTuple"+sample+"_Open_ElecTauStream.root";
  TFile *outFile = new TFile(outName,"UPDATE");

  TTree* outTreePtOrd     = new TTree(TString(("outTreePtOrd"+analysis).c_str()),"tree jets pT-ord");

  // kinematical variables of first 2 jets  
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi,phi1,phi2;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2,phi1_v2,phi2_v2;
  int nJets30, nJets20;

  // quality cuts of the first 2 jets
  float jetsBtagHE1,jetsBtagHE2,jetsBtagHP1,jetsBtagHP2;
  int nJets20BTagged;
  float chFracPV1, chFracPV2;

  // kinematical variables of the veto jet
  float ptVeto, etaVeto, phiVeto;
  int isVetoInJets; float chFracPVVeto;

  // diTau related variables
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, diTauSVFitMass, diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauMinMass;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,pZetaSig_;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;

  // anti-e related variables
  float antiEMVA;
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_,visibleTauMass_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac;

  // electron related variables
  int tightestCutBasedWP_, tightestCiCWP_,tightestMVAWP_;
  float sihih_, dEta_, dPhi_, HoE_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, embeddingWeight_;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightElec,HLTweightTau, SFElec, SFTau, HLTElec, HLTTau;
  int isTauLegMatched_,elecFlag_,genDecay_, leptFakeTau;

  // event id
  ULong64_t event_,run_,lumi_;


  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("phi1", &phi1,"phi1/F");
  outTreePtOrd->Branch("phi2", &phi2,"phi2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");

  outTreePtOrd->Branch("pt1_v2",  &pt1_v2,"pt1_v2/F");
  outTreePtOrd->Branch("pt2_v2",  &pt2_v2,"pt2_v2/F");
  outTreePtOrd->Branch("eta1_v2", &eta1_v2,"eta1_v2/F");
  outTreePtOrd->Branch("eta2_v2", &eta2_v2,"eta2_v2/F");
  outTreePtOrd->Branch("phi1_v2", &phi1_v2,"phi1_v2/F");
  outTreePtOrd->Branch("phi2_v2", &phi2_v2,"phi2_v2/F");
  outTreePtOrd->Branch("Deta_v2", &Deta_v2,"Deta_v2/F");
  outTreePtOrd->Branch("Dphi_v2", &Dphi_v2,"Dphi_v2/F");
  outTreePtOrd->Branch("Mjj_v2",  &Mjj_v2,"Mjj_v2/F");

  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");
  outTreePtOrd->Branch("jetsBtagHP1",  &jetsBtagHP1,"jetsBtagHP1/F");
  outTreePtOrd->Branch("jetsBtagHP2",  &jetsBtagHP2,"jetsBtagHP2/F");
  outTreePtOrd->Branch("chFracPV1",    &chFracPV1,  "chFracPV1/F");
  outTreePtOrd->Branch("chFracPV2",    &chFracPV2,  "chFracPV2/F");  
 
  outTreePtOrd->Branch("nJets30",       &nJets30,  "nJets30/I");  
  outTreePtOrd->Branch("nJets20",       &nJets20,  "nJets20/I");  
  outTreePtOrd->Branch("nJets20BTagged",&nJets20BTagged,  "nJets20BTagged/I");  

  outTreePtOrd->Branch("ptVeto",  &ptVeto, "ptVeto/F");
  outTreePtOrd->Branch("phiVeto", &phiVeto,"phiVeto/F");
  outTreePtOrd->Branch("etaVeto", &etaVeto,"etaVeto/F");
  outTreePtOrd->Branch("isVetoInJets", &isVetoInJets,"isVetoInJets/I");
  outTreePtOrd->Branch("chFracPVVeto", &chFracPVVeto,"chFracPVVeto/F"); 

  outTreePtOrd->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,       "diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,  "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  
  outTreePtOrd->Branch("diTauSVFitMass",&diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,  "diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta, "diTauSVFitEta/F");
  outTreePtOrd->Branch("diTauSVFitPhi", &diTauSVFitPhi, "diTauSVFitPhi/F");
  
  outTreePtOrd->Branch("diTauCAMass",&diTauCAMass,"diTauCAMass/F");
  outTreePtOrd->Branch("diTauCAPt",  &diTauCAPt,  "diTauCAPt/F");
  outTreePtOrd->Branch("diTauCAEta", &diTauCAEta, "diTauCAEta/F");
  outTreePtOrd->Branch("diTauCAPhi", &diTauCAPhi, "diTauCAPhi/F");
 
  outTreePtOrd->Branch("diTauVisMass",&diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauVisPt",  &diTauVisPt,"diTauVisPt/F");
  outTreePtOrd->Branch("diTauVisEta", &diTauVisEta,"diTauVisEta/F");
  outTreePtOrd->Branch("diTauVisPhi", &diTauVisPhi,"diTauVisPhi/F");

  outTreePtOrd->Branch("diTauMinMass",&diTauMinMass,"diTauMisMass/F");

  outTreePtOrd->Branch("etaL1",   &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2",   &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",    &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",    &ptL2,"ptL2/F");
  outTreePtOrd->Branch("phiL1",   &phiL1,"phiL1/F");
  outTreePtOrd->Branch("phiL2",   &phiL2,"phiL2/F");
  outTreePtOrd->Branch("dPhiL1L2",&dPhiL1L2,"dPhiL1L2/F");

  outTreePtOrd->Branch("visibleTauMass",          &visibleTauMass_,"visibleTauMass/F");
  outTreePtOrd->Branch("HoP",                     &HoP,"HoP/F");
  outTreePtOrd->Branch("EoP",                     &EoP,"EoP/F");
  outTreePtOrd->Branch("emFraction",              &emFraction_,"emFraction/F");
  outTreePtOrd->Branch("leadPFChargedHadrMva",    &leadPFChargedHadrMva_,"leadPFChargedHadrMva/F");
  outTreePtOrd->Branch("hasGsf",                  &hasGsf_,"hasGsf/F");
  outTreePtOrd->Branch("signalPFGammaCands",      &signalPFGammaCands_,"signalPFGammaCands/F");
  outTreePtOrd->Branch("signalPFChargedHadrCands",&signalPFChargedHadrCands_,"signalPFChargedHadrCands/F");
  outTreePtOrd->Branch("etaMom2",                 &etaMom2,"etaMom2/F");
  outTreePtOrd->Branch("phiMom2",                 &phiMom2,"phiMom2/F");
  outTreePtOrd->Branch("gammaFrac",               &gammaFrac,"gammaFrac/F");
  outTreePtOrd->Branch("antiEMVA",                &antiEMVA,"antiEMVA/F");


  outTreePtOrd->Branch("diTauCharge", &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",      &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("MtLeg1Corr",  &MtLeg1Corr_,"MtLeg1Corr/F");
  outTreePtOrd->Branch("pZeta",       &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaCorr",   &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVis",    &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("pZetaVisCorr",&pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEt",         &MEt,"MEt/F");
  outTreePtOrd->Branch("MEtCorr",     &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("pZetaSig",    &pZetaSig_,"pZetaSig/F");

  outTreePtOrd->Branch("combRelIsoLeg1",     &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("combRelIsoLeg1Beta", &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho,"combRelIsoLeg1Rho/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",        &combIsoLeg2,"combIsoLeg2/F");

  outTreePtOrd->Branch("tightestCutBasedWP", &tightestCutBasedWP_,"tightestCutBasedWP/I");
  outTreePtOrd->Branch("tightestCiCWP",      &tightestCiCWP_,"tightestCiCWP/I");
  outTreePtOrd->Branch("tightestMVAWP",      &tightestMVAWP_,"tightestMVAWP/I");
  outTreePtOrd->Branch("sihih",              &sihih_,"sihih/F");
  outTreePtOrd->Branch("dEta",               &dEta_,"dEta/F");
  outTreePtOrd->Branch("dPhi",               &dPhi_,"dPhi/F");
  outTreePtOrd->Branch("HoE",                &HoE_,"HoE/F");


  outTreePtOrd->Branch("tightestHPSDBWP",    &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("decayMode",          &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightElec",&HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTElec",&HLTElec,"HLTElec/F");
  outTreePtOrd->Branch("HLTTau", &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("SFElec",       &SFElec,"SFElec/F");

  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("elecFlag",        &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/ElecTauStream_08Nov2011//treeElecTauStream_"+samples[index]+".root" : "../treeElecTauStream.root";

  TString inName(currentInName.c_str());
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }

  TString treeName(("elecTauStreamAnalyzer"+analysis+"/tree").c_str());

  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries    = currentTree->GetEntries() ;
  int nEventsRead = ((TH1F*)file->Get("allEventsFilter/totalEvents"))->GetBinContent(1) ;
  cout<< "nEventsRead " << nEventsRead << endl;
  float crossSection = index >= 0 ? crossSec[index] : float(nEventsRead)/Lumi ; 
  cout<< "crossSection " << crossSection << endl;
  float scaleFactor = (crossSection != 0) ? Lumi / (  float(nEventsRead)/crossSection )  : 1.0;
  cout<< "scaleFactor " << scaleFactor << endl;

  // jets
  currentTree->SetBranchStatus("jetsP4"                ,0);
  currentTree->SetBranchStatus("jetsIDP4"              ,1);
  currentTree->SetBranchStatus("jetsIDUpP4"            ,1);
  currentTree->SetBranchStatus("jetsIDDownP4"          ,1);
  currentTree->SetBranchStatus("jetsIDL1OffsetP4"      ,1);
  currentTree->SetBranchStatus("genJetsIDP4"           ,0);
  currentTree->SetBranchStatus("jetsBtagHE"            ,1);
  currentTree->SetBranchStatus("jetsBtagHP"            ,1);
  currentTree->SetBranchStatus("jetMoments"            ,0);
  currentTree->SetBranchStatus("jetsChNfraction"       ,1);
  currentTree->SetBranchStatus("jetsChEfraction"       ,0);

  // diTaus
  currentTree->SetBranchStatus("diTauVisP4"            ,1);
  currentTree->SetBranchStatus("diTauCAP4"             ,0);
  currentTree->SetBranchStatus("diTauICAP4"            ,1); 
  currentTree->SetBranchStatus("diTauSVfitP4"          ,1);
  currentTree->SetBranchStatus("diTauNSVfitMass"       ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp"  ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("diTauCharge"           ,1);

  // taus
  currentTree->SetBranchStatus("diTauLegsP4"           ,1);
  currentTree->SetBranchStatus("genDiTauLegsP4"        ,1);
  currentTree->SetBranchStatus("chIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("nhIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("phIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("chIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("nhIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("phIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("chIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("nhIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("phIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("chIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("nhIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("phIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("chIsoLeg2"             ,1);
  currentTree->SetBranchStatus("nhIsoLeg2"             ,0);
  currentTree->SetBranchStatus("phIsoLeg2"             ,1);
  currentTree->SetBranchStatus("dxy1"                  ,0);
  currentTree->SetBranchStatus("dxy2"                  ,0);
  currentTree->SetBranchStatus("dz1"                   ,0);
  currentTree->SetBranchStatus("dz2"                   ,0);
  currentTree->SetBranchStatus("decayMode"             ,1);
  currentTree->SetBranchStatus("tightestHPSWP"         ,0);
  currentTree->SetBranchStatus("tightestHPSDBWP"       ,1);
  currentTree->SetBranchStatus("visibleTauMass"        ,1);
  currentTree->SetBranchStatus("isTauLegMatched"       ,1);
  currentTree->SetBranchStatus("isElecLegMatched"      ,0);
  currentTree->SetBranchStatus("hasKft"                ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrPt"   ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrP"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackPt"    ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackP"     ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrMva"  ,1);
  currentTree->SetBranchStatus("emFraction"            ,1);
  currentTree->SetBranchStatus("hasGsf"                ,1);
  currentTree->SetBranchStatus("signalPFChargedHadrCands"    ,1);
  currentTree->SetBranchStatus("signalPFGammaCands"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrHcalEnergy" ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrEcalEnergy" ,1);
  currentTree->SetBranchStatus("gammadEta"             ,1);
  currentTree->SetBranchStatus("gammadPhi"             ,1);
  currentTree->SetBranchStatus("gammaPt"               ,1);

  //electron specific
  currentTree->SetBranchStatus("nBrehm"                ,0);
  currentTree->SetBranchStatus("likelihood"            ,0);
  currentTree->SetBranchStatus("nHits"                 ,0);
  currentTree->SetBranchStatus("sihih"                 ,1);
  currentTree->SetBranchStatus("dPhi"                  ,1);
  currentTree->SetBranchStatus("dEta"                  ,1);
  currentTree->SetBranchStatus("HoE"                   ,1);
  currentTree->SetBranchStatus("EoP"                   ,0);
  currentTree->SetBranchStatus("fbrem"                 ,0);
  currentTree->SetBranchStatus("tightestCutBasedWP"    ,1);
  currentTree->SetBranchStatus("tightestCiCWP"         ,1);
  currentTree->SetBranchStatus("tightestMVAWP"         ,1);

  // MET
  currentTree->SetBranchStatus("METP4"                 ,1);
  currentTree->SetBranchStatus("genMETP4"              ,1);
  currentTree->SetBranchStatus("sumEt"                 ,0);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,0);

  // generator-level boson
  currentTree->SetBranchStatus("genVP4"                ,1);
  currentTree->SetBranchStatus("genDecay"              ,1);

  // extra-electrons
  currentTree->SetBranchStatus("extraElectrons"        ,0);
  currentTree->SetBranchStatus("pfElectrons"           ,0);
  currentTree->SetBranchStatus("elecFlag"              ,1);

  // event-dependent variables
  currentTree->SetBranchStatus("rhoFastJet"            ,1);
  currentTree->SetBranchStatus("rhoNeutralFastJet"     ,0);
  currentTree->SetBranchStatus("numPV"                 ,1);
  currentTree->SetBranchStatus("nPUVertices"           ,1);
  currentTree->SetBranchStatus("numOfDiTaus"           ,0);
  currentTree->SetBranchStatus("numOfLooseIsoDiTaus"   ,1);
 
  currentTree->SetBranchStatus("run"                   ,1);
  currentTree->SetBranchStatus("event"                 ,1);
  currentTree->SetBranchStatus("lumi"                  ,1);
  currentTree->SetBranchStatus("mcPUweight"            ,1);
  currentTree->SetBranchStatus("embeddingWeight"       ,1);

  // triggers
  currentTree->SetBranchStatus("tauXTriggers"          ,1);
  currentTree->SetBranchStatus("triggerBits"           ,1);

  ////////////////////////////////////////////////////////////////////

  std::vector< LV >* jets           = new std::vector< LV >();

  if(analysis.find("JetUp")!=string::npos) 
    currentTree->SetBranchAddress("jetsIDUpP4",        &jets);
  else if(analysis.find("JetDown")!=string::npos)
    currentTree->SetBranchAddress("jetsIDDownP4",      &jets);
  else
    currentTree->SetBranchAddress("jetsIDP4",          &jets);

  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",&jets_v2);

  std::vector< LV >* diTauLegsP4    = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",     &diTauLegsP4);

  std::vector< LV >* diTauVisP4     = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",      &diTauVisP4);

  std::vector< LV >* diTauSVfitP4     = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauSVfitP4",    &diTauSVfitP4);

  std::vector< LV >* diTauCAP4    = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauICAP4",    &diTauCAP4);

  std::vector< LV >* genDiTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("genDiTauLegsP4",    &genDiTauLegsP4);

  std::vector< LV >* genVP4         = new std::vector< LV >();
  currentTree->SetBranchAddress("genVP4",          &genVP4);

  std::vector< LV >* METP4          = new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",           &METP4);

  std::vector< LV >* genMETP4       = new std::vector< LV >();
  currentTree->SetBranchAddress("genMETP4",        &genMETP4);

  std::vector< int >* tauXTriggers  = new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",    &tauXTriggers);
  std::vector< int >* triggerBits   = new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",     &triggerBits);

  std::vector< double >* jetsBtagHE = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",      &jetsBtagHE);
  std::vector< double >* jetsBtagHP = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHP",      &jetsBtagHP);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChNfraction", &jetsChNfraction);

  std::vector< float >* gammadEta =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadEta", &gammadEta);

  std::vector< float >* gammadPhi =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadPhi", &gammadPhi);

  std::vector< float >* gammaPt =  new std::vector< float >();
  currentTree->SetBranchAddress("gammaPt", &gammaPt);

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;
  float diTauCharge;
  int tightestHPSDBWP, decayMode;
  int tightestCutBasedWP, tightestCiCWP, tightestMVAWP;
  float sihih, dEta, dPhi, HoE;
  float numPV;
  float mcPUweight,embeddingWeight;
  int numOfLooseIsoDiTaus;
  int isTauLegMatched,elecFlag,genDecay, nPUVertices;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  ULong64_t event,run,lumi;

  currentTree->SetBranchAddress("chIsoLeg2",            &chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",            &phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",          &chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",          &nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1v2",          &phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",        &chIsoPULeg1);
  currentTree->SetBranchAddress("nhIsoPULeg1v2",        &nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",        &phIsoPULeg1);
  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);
  currentTree->SetBranchAddress("rhoNeutralFastJet",    &rhoNeutralFastJet);
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("tightestCutBasedWP",   &tightestCutBasedWP);
  currentTree->SetBranchAddress("tightestCiCWP",        &tightestCiCWP);
  currentTree->SetBranchAddress("tightestMVAWP",        &tightestMVAWP);
  currentTree->Branch("sihih",                          &sihih);
  currentTree->Branch("dEta",                           &dEta);
  currentTree->Branch("dPhi",                           &dPhi);
  currentTree->Branch("HoE",                            &HoE);

  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("diTauCharge",          &diTauCharge);

  currentTree->SetBranchAddress("diTauNSVfitMass",       &diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);
  currentTree->SetBranchAddress("mTauTauMin",            &mTauTauMin);

  currentTree->SetBranchAddress("pZetaSig",             &pZetaSig);

  currentTree->SetBranchAddress("numPV",                &numPV);
  currentTree->SetBranchAddress("mcPUweight",           &mcPUweight);
  currentTree->SetBranchAddress("embeddingWeight",      &embeddingWeight);
  currentTree->SetBranchAddress("event",                &event);
  currentTree->SetBranchAddress("run",                  &run);
  currentTree->SetBranchAddress("lumi",                 &lumi);
  currentTree->SetBranchAddress("nPUVertices",          &nPUVertices);
  currentTree->SetBranchAddress("genDecay",             &genDecay);
  currentTree->SetBranchAddress("decayMode",            &decayMode);
  currentTree->SetBranchAddress("numOfLooseIsoDiTaus",  &numOfLooseIsoDiTaus);
  currentTree->SetBranchAddress("elecFlag",             &elecFlag);
  currentTree->SetBranchAddress("isTauLegMatched",      &isTauLegMatched);
  currentTree->SetBranchAddress("visibleTauMass",       &visibleTauMass);
  currentTree->SetBranchAddress("leadPFChargedHadrP",   &leadPFChargedHadrCandP);
  currentTree->SetBranchAddress("leadPFChargedHadrMva" ,&leadPFChargedHadrMva);
  currentTree->SetBranchAddress("emFraction"           ,&emFraction);
  currentTree->SetBranchAddress("hasGsf"               ,&hasGsf);
  currentTree->SetBranchAddress("signalPFChargedHadrCands"    ,&signalPFChargedHadrCands);
  currentTree->SetBranchAddress("signalPFGammaCands"   ,&signalPFGammaCands);
  currentTree->SetBranchAddress("leadPFChargedHadrHcalEnergy" ,&leadPFChargedHadrHcalEnergy);
  currentTree->SetBranchAddress("leadPFChargedHadrEcalEnergy" ,&leadPFChargedHadrEcalEnergy);

  //unsigned int indexHelp = index >=0 ? index : 0;
    
  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);
    if(n%2000==0) cout << n << endl;

    // initialize variables filled only in the two jet case
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;phi1=-99;phi2=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;phi1_v2=-99;phi2_v2=-99;
    ptVeto = -99; phiVeto= -99; etaVeto= -99;isVetoInJets=-99;
    chFracPV1=-99;chFracPV2=-99;chFracPVVeto=-99; jetsBtagHE1 = -99;jetsBtagHE2 = -99;jetsBtagHP1 = -99;jetsBtagHP2 = -99;

    // define the relevant jet collection
    nJets20BTagged = 0;
    nJets30        = 0;
    int lead  = -99;
    int trail = -99;
    int veto  = -99;
    vector<int> indexes;
    for(unsigned int l = 0 ; l < jets->size() ; l++){
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta){
	indexes.push_back(l);
	if((*jetsBtagHE)[l] > 3.3) nJets20BTagged++;
      }
    }
    nJets20 = indexes.size();
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];

    for(unsigned int v = 0 ; v < indexes.size() ; v++){
      if( (*jets)[indexes[v]].Pt() > 30 ) nJets30++;
    }

    //1 or 2 jet preselection
    if(lead>=0){

      pt1  = (*jets)[lead].Pt();
      eta1 = (*jets)[lead].Eta();
      phi1 = (*jets)[lead].Phi();
      eta1_v2 = (*jets_v2)[lead].Eta();
      pt1_v2  = (*jets_v2)[lead].Pt();
      phi1_v2 = (*jets_v2)[lead].Phi();

      jetsBtagHE1 = (*jetsBtagHE)[lead];
      jetsBtagHP1 = (*jetsBtagHP)[lead];

      chFracPV1 = (*jetsChNfraction)[lead]; 

      if(trail>=0){

	pt2  = (*jets)[trail].Pt();
	eta2 = (*jets)[trail].Eta();
	phi2 = (*jets)[trail].Phi();
	
	pt2_v2  = (*jets_v2)[trail].Pt();
	eta2_v2 = (*jets_v2)[trail].Eta();
	phi2_v2 = (*jets_v2)[trail].Phi();
	
	Deta = abs(eta1-eta2);
	Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
	  -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi()  :
	  abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) ;
	Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
	
	Deta_v2 = abs(eta1-eta2);
	Dphi_v2 = abs((*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi()) > TMath::Pi() ? 
	  -abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) + 2*TMath::Pi()  :
	  abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) ;
	Mjj_v2  = ((*jets_v2)[lead]+(*jets_v2)[trail]).M();
	
	jetsBtagHE2 = (*jetsBtagHE)[trail];
	jetsBtagHP2 = (*jetsBtagHP)[trail];

	chFracPV2 = (*jetsChNfraction)[trail]; 
      }

    }

    ptVeto       = (veto>=0) ? (*jets)[veto].Pt() : -99;
    etaVeto      = (veto>=0) ? (*jets)[veto].Eta(): -99;
    phiVeto      = (veto>=0) ? (*jets)[veto].Phi(): -99;
    chFracPVVeto = (veto>=0) ? (*jetsChNfraction)[veto] : -99;

    isVetoInJets = 0;
    for(unsigned int l = 0 ; l < indexes.size() ; l++){
      if(lead>=0 && trail>=0 && (l!= lead && l!= trail) &&
	 (*jets)[indexes[l]].Pt()>PtVETO && ((*jets)[indexes[l]].Eta() - eta1)*((*jets)[indexes[l]].Eta() - eta2)<=0 )
	isVetoInJets = 1;
    }

    diTauNSVfitMass_        = diTauNSVfitMass;
    diTauNSVfitMassErrUp_   = diTauNSVfitMassErrUp;
    diTauNSVfitMassErrDown_ = diTauNSVfitMassErrDown;

    diTauSVFitMass = (*diTauSVfitP4)[0].M();
    diTauSVFitPt   = (*diTauSVfitP4)[0].Pt();
    diTauSVFitEta  = (*diTauSVfitP4)[0].Eta();
    diTauSVFitPhi  = (*diTauSVfitP4)[0].Phi();

    diTauCAMass = (*diTauCAP4)[0].M();
    diTauCAPt   = (*diTauCAP4)[0].Pt();
    diTauCAEta  = (*diTauCAP4)[0].Eta();
    diTauCAPhi  = (*diTauCAP4)[0].Phi();

    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt    = (*diTauVisP4)[0].Pt();
    diTauVisEta   = (*diTauVisP4)[0].Eta();
    diTauVisPhi   = (*diTauVisP4)[0].Phi();

    diTauMinMass  = mTauTauMin;
  
    ptL1     = (*diTauLegsP4)[0].Pt();
    ptL2     = (*diTauLegsP4)[1].Pt();
    etaL1    = (*diTauLegsP4)[0].Eta();
    etaL2    = (*diTauLegsP4)[1].Eta();
    phiL1    = (*diTauLegsP4)[0].Phi();
    phiL2    = (*diTauLegsP4)[1].Phi();
    dPhiL1L2 =  abs((*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi()) > TMath::Pi() ? 
      -abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) + 2*TMath::Pi()  :
      abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) ;

    visibleTauMass_ = visibleTauMass;
    diTauCharge_    = diTauCharge;
      
    float scalarSumPt     = ( *diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt     = ((*diTauLegsP4)[0] +      (*METP4)[0]).Pt() ;
    float scalarSumPtCorr = ( *diTauLegsP4)[0].Pt() + (*METP4)[1].Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] +      (*METP4)[1]).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[0];

    pZetaSig_     = pZetaSig;

    MEt     = (*METP4)[0].Et();
    MEtCorr = (*METP4)[1].Et();

    combRelIsoLeg1      = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    float PUtoPVratio = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta  = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta = (chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    float EffArea = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    

    tightestCutBasedWP_ = tightestCutBasedWP;
    tightestCiCWP_      = tightestCiCWP;
    tightestMVAWP_      = tightestMVAWP;
    sihih_ = sihih; 
    dEta_  = dEta; 
    dPhi_  = dPhi; 
    HoE_   = HoE;

    //////////

    float sumPt  = 0;
    float dEta2  = 0;
    float dPhi2  = 0;
    float sumPt2 = 0;

    for(unsigned int k = 0 ; k < gammaPt->size() ; k++){
      float pt_k  = (*gammaPt)[k];
      float phi_k = (*gammadPhi)[k];
      if ((*gammadPhi)[k] > TMath::Pi()) phi_k = (*gammadPhi)[k] - 2*TMath::Pi();
      else if((*gammadPhi)[k] < -TMath::Pi()) phi_k = (*gammadPhi)[k] + 2*TMath::Pi();

      float eta_k = (*gammadEta)[k];

      sumPt  += pt_k;
      sumPt2 += (pt_k*pt_k);
      dEta2 += (pt_k*eta_k*eta_k);
      dPhi2 += (pt_k*phi_k*phi_k);  
    }

    gammaFrac = 0.;
    if(sumPt>0){
      dEta2 /= sumPt;
      dPhi2 /= sumPt;
      gammaFrac = sumPt/ptL2;
    }

    etaMom2 = TMath::Sqrt(dEta2)*TMath::Sqrt(gammaFrac)*ptL2;
    phiMom2 = TMath::Sqrt(dPhi2)*TMath::Sqrt(gammaFrac)*ptL2;


    //////////
    /*
    double mva = antiE->MVAValue(etaL2,ptL2,
    signalPFChargedHadrCands,signalPFGammaCands,
    leadPFChargedHadrMva,
    leadPFChargedHadrHcalEnergy/leadPFChargedHadrCandP, leadPFChargedHadrEcalEnergy/leadPFChargedHadrCandP,
    hasGsf,visibleTauMass,emFraction,
    gammadEta,gammadPhi,gammaPt);
    */
    double mva = antiE->MVAValue(etaL2,ptL2,
				 signalPFChargedHadrCands,signalPFGammaCands,
				 leadPFChargedHadrMva,
				 leadPFChargedHadrHcalEnergy/leadPFChargedHadrCandP, leadPFChargedHadrEcalEnergy/leadPFChargedHadrCandP,
				 hasGsf,visibleTauMass,emFraction,
				 etaMom2,phiMom2,gammaFrac);
	
    antiEMVA = 
      ((abs(etaL2)<1.5 && signalPFGammaCands==0 && mva>0.054) ||
       (abs(etaL2)<1.5 && signalPFGammaCands>0  && hasGsf>0.5 && mva>0.060) ||
       (abs(etaL2)<1.5 && signalPFGammaCands>0  && hasGsf<0.5 && mva>0.054) ||
       (abs(etaL2)>1.5 && signalPFGammaCands==0 && mva>0.060) ||
       (abs(etaL2)>1.5 && signalPFGammaCands>0  && hasGsf>0.5 && mva>0.053) ||
       (abs(etaL2)>1.5 && signalPFGammaCands>0  && hasGsf<0.5 && mva>0.049)) ? 1.0 : 0.0;

    HoP                       = leadPFChargedHadrHcalEnergy/leadPFChargedHadrCandP;
    EoP                       = leadPFChargedHadrEcalEnergy/leadPFChargedHadrCandP;
    emFraction_               = emFraction;
    leadPFChargedHadrMva_     = leadPFChargedHadrMva;
    visibleTauMass_           = visibleTauMass;
    hasGsf_                   = hasGsf; 
    signalPFGammaCands_       = signalPFGammaCands; 
    signalPFChargedHadrCands_ = signalPFChargedHadrCands;

    tightestHPSDBWP_    = tightestHPSDBWP;
    decayMode_          = decayMode;

    numPV_              = numPV;
    sampleWeight        = scaleFactor; 
    puWeight            = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight ;
    embeddingWeight_    = embeddingWeight;
    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){

      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v2
      else if(run>=163269 && run<=163869)
	HLTx = 	float((*triggerBits)[2]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau15_v4
      else if(run>=165088 && run<=165633)
	HLTx = float((*triggerBits)[3]);  //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v6
      else if(run>=165970 && run<=166967)
	HLTx = 	float((*triggerBits)[4]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v8
      else if(run>=167039 && run<=167913)
	HLTx = 	float((*triggerBits)[5]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v9
      else if(run>=170249 && run<=173198)
	HLTx = 	float((*triggerBits)[6]); //HLT_Ele15_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_TightIsoPFTau20_v2
      else if(run>=173236 && run<=178380)
	HLTx = 	float((*triggerBits)[7] ||//HLT_Ele18_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_MediumIsoPFTau20_v1 ||
		      (*triggerBits)[8]); //HLT_Ele20_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_MediumIsoPFTau20_v1
      else if(run>=178420 && run<=179889)
	HLTx = 	float((*triggerBits)[9]); //HLT_Ele20_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_MediumIsoPFTau20_v5
      else if(run>=179959 && run<=180252)
	HLTx = 	float((*triggerBits)[10]);//HLT_Ele20_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_MediumIsoPFTau20_v6

      //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau15
      if(run>=160404 && run<=163869){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1])  && (*tauXTriggers)[5] ;
	HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      }
      //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau20
      else if(run>=165088 && run<=167913){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1]) && (*tauXTriggers)[7] ; 
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15TightIsoPFTau20
      else if(run>=170249 && run<=173198){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1]) && (*tauXTriggers)[7] ;
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      //hltEle18CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle18MediumIsoPFTau20
      // attention: some runs get prescaled, then take 2020
      else if(run>=173236 && run<=178380){
	bool isTriggMatched = ((*tauXTriggers)[2] || (*tauXTriggers)[3] || (*tauXTriggers)[4]) && ((*tauXTriggers)[8] || (*tauXTriggers)[9]);
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      //hltEle20CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle20MediumIsoPFTau20
      else if(run>=178420 && run<=180252){
	bool isTriggMatched =  (*tauXTriggers)[4] && (*tauXTriggers)[9] ;
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }

      HLTweightTau  = 1.0;
      HLTweightElec = 1.0;
      if( (std::string(sample.Data())).find("Embed")!=string::npos ){
	HLTElec = ((*diTauLegsP4)[0].Eta()<1.5) ? 
	  turnOnElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	  turnOnElecAllEC->Eval((*diTauLegsP4)[0].Pt());
	HLTTau  = turnOnTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      }
      else{
	HLTElec       = 1.0;
	HLTTau        = 1.0;
      }

      SFTau         = 1.0;
      SFElec        = 1.0;

    } else{

      HLTx = float((*triggerBits)[0]);
      HLTmatch = ((*tauXTriggers)[0] && (*tauXTriggers)[2]) ? 1.0 : 0.0;

      HLTweightTau  = ratioTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTweightElec = ((*diTauLegsP4)[0].Eta()<1.5) ? 
	ratioElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	ratioElecAllEC->Eval((*diTauLegsP4)[0].Pt());

      HLTTau  = turnOnTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTElec = ((*diTauLegsP4)[0].Eta()<1.5) ? 
	turnOnElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	turnOnElecAllEC->Eval((*diTauLegsP4)[0].Pt());

      SFTau = 1.0;
      SFElec = (ratioElecID->Eval((*diTauLegsP4)[0].Pt())) * (ratioElecIso->Eval((*diTauLegsP4)[0].Pt()));
    }
 
     isTauLegMatched_ = isTauLegMatched;
    if((std::string(sample.Data())).find("Run2011")==string::npos)
      leptFakeTau      = (isTauLegMatched==0 && (*genDiTauLegsP4)[1].E()>0) ? 1 : 0;
    else 
      leptFakeTau = -99;

    elecFlag_        = elecFlag;
    genDecay_        = genDecay ;
    event_           = event;
    run_             = run;
    lumi_            = lumi;

    outTreePtOrd->Fill();
  }


 file->Close();

 if(SAVE) outFile->Write();
 outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfitP4; delete diTauCAP4; delete genDiTauLegsP4;
 delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsBtagHP; delete jetsChNfraction; delete genVP4; delete genMETP4;
 delete gammadEta; delete gammadPhi; delete gammaPt;

 return;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



void makeTrees_MuTauStream(string analysis = "", int index = -1 ){
  
  cout << "Now skimming analysis " << analysis << endl;
  

  TFile corrections("llrCorrections_backup.root");
  
  TF1 *ratioMuID        = (TF1*)corrections.Get("ratioMuId");
  TF1 *ratioMuIso       = (TF1*)corrections.Get("ratioMuIso");
  TF1 *ratioMuAll       = (TF1*)corrections.Get("ratioMuAll");
  TF1 *ratioTauMuTauAll = (TF1*)corrections.Get("ratioTauMuTauAll");
  TF1 *turnOnMuAll      = (TF1*)corrections.Get("turnOnMuAll");
  TF1 *turnOnTauMuTauAll= (TF1*)corrections.Get("turnOnTauMuTauAll");  

  if(!ratioMuID)        cout << "Missing corrections for MuID" << endl;
  if(!ratioMuIso)       cout << "Missing corrections for MuIso" << endl;
  if(!ratioMuAll)       cout << "Missing corrections for Mu HLT" << endl;
  if(!ratioTauMuTauAll) cout << "Missing corrections for tau HLT" << endl;
  if(!turnOnMuAll)      cout << "Missing turnOn for mu" << endl;
  if(!turnOnTauMuTauAll)cout << "Missing turnOn for tau" << endl;

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011-MuTau-All_run");             crossSec.push_back( 0  );                          
  samples.push_back("QCDmu-MuTau-pythia-20-15-PUS4_run"); crossSec.push_back( 84679          * 0.00756887  * 0.6105025);                  
  samples.push_back("DYJets-MuTau-50-madgraph-PUS4_run"); crossSec.push_back( 3048           * 0.0094875   * 0.6411417); 
  samples.push_back("TTJets-MuTau-madgraph-PUS4_run");    crossSec.push_back( 157.5          * 0.0198369   * 0.8236127);  
  samples.push_back("WJets-MuTau-madgraph-PUS4_run");     crossSec.push_back( 31314.0        * 0.0011318   * 0.5888984);   
  samples.push_back("WZ-MuTau-pythia-PUS4_run");          crossSec.push_back( 18.2           * 0.0065952   * 0.7354066);         
  samples.push_back("ZZ-MuTau-pythia-PUS4_run");          crossSec.push_back(  5.9           * 0.0058205   * 0.7483689);    
  samples.push_back("WW-MuTau-pythia-PUS4_run");          crossSec.push_back( 43.0           * 0.0096241   * 0.7354066);        
  samples.push_back("VBFH100-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*1.546 * 0.04953     * 0.8093258); 
  samples.push_back("GGFH100-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*24.02 * 0.03816     * 0.8183724); 
  samples.push_back("VBFH105-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*1.472 * 0.05278     * 0.8042489); 
  samples.push_back("GGFH105-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*21.78 * 0.04137     * 0.8144567); 
  samples.push_back("VBFH110-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*1.398 * 0.05562     * 0.8098949);
  samples.push_back("GGFH110-MuTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*19.84 * 0.04287     * 0.8169836);    
  samples.push_back("VBFH115-MuTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*1.332 * 0.05825     * 0.8051487);
  samples.push_back("GGFH115-MuTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*18.13 * 0.04636     * 0.8204931);  
  samples.push_back("VBFH120-MuTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*1.269 * 0.05970     * 0.8134593);   
  samples.push_back("GGFH120-MuTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*16.63 * 0.04900     * 0.8140816);  
  samples.push_back("VBFH125-MuTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*1.211 * 0.06337     * 0.8117899);   
  samples.push_back("GGFH125-MuTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*15.31 * 0.05130     * 0.8191832); 
  samples.push_back("VBFH130-MuTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*1.154 * 0.06515     * 0.8113619); 
  samples.push_back("GGFH130-MuTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*14.12 * 0.05479     * 0.8284201);
  samples.push_back("VBFH135-MuTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*1.100 * 0.06740     * 0.8160842); 
  samples.push_back("GGFH135-MuTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*13.08 * 0.05701     * 0.8249117);    
  samples.push_back("VBFH140-MuTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*1.052 * 0.06987     * 0.8082546);  
  samples.push_back("GGFH140-MuTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*12.13 * 0.05808     * 0.8223979);  
  samples.push_back("VBFH145-MuTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*1.004 * 0.07349     * 0.8174933);  
  samples.push_back("GGFH145-MuTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*11.27 * 0.06143     * 0.8229404);  

  samples.push_back("VH100-MuTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 0.082128 *0.834434);  
//samples.push_back("VH105-MuTau-pythia-PUS4_run");  crossSec.push_back( 8.25e-02*(1.018+ 0.5449+0.1433  ) * 1.0      * 1.0); 
  samples.push_back("VH110-MuTau-pythia-PUS4_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 0.0881738* 0.831821);
  samples.push_back("VH115-MuTau-pythia-PUS4_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 0.0920318* 0.837358);
  samples.push_back("VH120-MuTau-pythia-PUS4_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 0.094658 * 0.837034);   
  samples.push_back("VH125-MuTau-pythia-PUS4_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 0.09885  * 0.842507);
  samples.push_back("VH130-MuTau-pythia-PUS4_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 0.10081  * 0.837318);
  samples.push_back("VH135-MuTau-pythia-PUS4_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 0.104662 * 0.836088);
  samples.push_back("VH140-MuTau-pythia-PUS4_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 1.0 * 0.187198);  
  samples.push_back("VH145-MuTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 1.0 * 0.192687);  
  


  // normalization Lumi
  Float_t Lumi=1000;

  string currentSample = index >= 0 ? samples[index] : "Debug";
  TString sample(currentSample.c_str());

  TString outName = "nTuple"+sample+"_Open_MuTauStream.root";
  TFile *outFile = new TFile(outName,"UPDATE");

  TTree* outTreePtOrd     = new TTree(TString(("outTreePtOrd"+analysis).c_str()),"tree jets pT-ord");

  // kinematical variables of first 2 jets  
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi,phi1,phi2;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2,phi1_v2,phi2_v2;
  int nJets30, nJets20;

  // quality cuts of the first 2 jets
  float jetsBtagHE1,jetsBtagHE2,jetsBtagHP1,jetsBtagHP2;
  int nJets20BTagged;
  float chFracPV1, chFracPV2;

  // kinematical variables of the veto jet
  float ptVeto, etaVeto, phiVeto; 
  int isVetoInJets; float chFracPVVeto;

  // diTau related variables
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, diTauSVFitMass, diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauMinMass;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,pZetaSig_;;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;

  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightTau, HLTweightMu, SFMu, SFTau, HLTMu, HLTTau;
  int isTauLegMatched_,muFlag_,genDecay_,leptFakeTau;

  // event id
  ULong64_t event_,run_,lumi_;

  outTreePtOrd->Branch("pt1",  &pt1,"pt1/F");
  outTreePtOrd->Branch("pt2",  &pt2,"pt2/F");
  outTreePtOrd->Branch("eta1", &eta1,"eta1/F");
  outTreePtOrd->Branch("eta2", &eta2,"eta2/F");
  outTreePtOrd->Branch("phi1", &phi1,"phi1/F");
  outTreePtOrd->Branch("phi2", &phi2,"phi2/F");
  outTreePtOrd->Branch("Deta", &Deta,"Deta/F");
  outTreePtOrd->Branch("Dphi", &Dphi,"Dphi/F");
  outTreePtOrd->Branch("Mjj",  &Mjj,"Mjj/F");

  outTreePtOrd->Branch("pt1_v2",  &pt1_v2,"pt1_v2/F");
  outTreePtOrd->Branch("pt2_v2",  &pt2_v2,"pt2_v2/F");
  outTreePtOrd->Branch("eta1_v2", &eta1_v2,"eta1_v2/F");
  outTreePtOrd->Branch("eta2_v2", &eta2_v2,"eta2_v2/F");
  outTreePtOrd->Branch("phi1_v2", &phi1_v2,"phi1_v2/F");
  outTreePtOrd->Branch("phi2_v2", &phi2_v2,"phi2_v2/F");
  outTreePtOrd->Branch("Deta_v2", &Deta_v2,"Deta_v2/F");
  outTreePtOrd->Branch("Dphi_v2", &Dphi_v2,"Dphi_v2/F");
  outTreePtOrd->Branch("Mjj_v2",  &Mjj_v2,"Mjj_v2/F");

  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");
  outTreePtOrd->Branch("jetsBtagHP1",  &jetsBtagHP1,"jetsBtagHP1/F");
  outTreePtOrd->Branch("jetsBtagHP2",  &jetsBtagHP2,"jetsBtagHP2/F");
  outTreePtOrd->Branch("chFracPV1",    &chFracPV1,  "chFracPV1/F");
  outTreePtOrd->Branch("chFracPV2",    &chFracPV2,  "chFracPV2/F"); 

  outTreePtOrd->Branch("nJets30",       &nJets30,  "nJets30/I");  
  outTreePtOrd->Branch("nJets20",       &nJets20,  "nJets20/I");  
  outTreePtOrd->Branch("nJets20BTagged",&nJets20BTagged,  "nJets20BTagged/I");  
 
  outTreePtOrd->Branch("ptVeto",  &ptVeto, "ptVeto/F");
  outTreePtOrd->Branch("phiVeto", &phiVeto,"phiVeto/F");
  outTreePtOrd->Branch("etaVeto", &etaVeto,"etaVeto/F");
  outTreePtOrd->Branch("isVetoInJets", &isVetoInJets,"isVetoInJets/I");
  outTreePtOrd->Branch("chFracPVVeto", &chFracPVVeto,"chFracPVVeto/F"); 

  outTreePtOrd->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,       "diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,  "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  
  outTreePtOrd->Branch("diTauSVFitMass",&diTauSVFitMass,"diTauSVFitMass/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,  "diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta, "diTauSVFitEta/F");
  outTreePtOrd->Branch("diTauSVFitPhi", &diTauSVFitPhi, "diTauSVFitPhi/F");
  
  outTreePtOrd->Branch("diTauCAMass",&diTauCAMass,"diTauCAMass/F");
  outTreePtOrd->Branch("diTauCAPt",  &diTauCAPt,  "diTauCAPt/F");
  outTreePtOrd->Branch("diTauCAEta", &diTauCAEta, "diTauCAEta/F");
  outTreePtOrd->Branch("diTauCAPhi", &diTauCAPhi, "diTauCAPhi/F");
 
  outTreePtOrd->Branch("diTauVisMass",&diTauVisMass,"diTauVisMass/F");
  outTreePtOrd->Branch("diTauVisPt",  &diTauVisPt,"diTauVisPt/F");
  outTreePtOrd->Branch("diTauVisEta", &diTauVisEta,"diTauVisEta/F");
  outTreePtOrd->Branch("diTauVisPhi", &diTauVisPhi,"diTauVisPhi/F");

  outTreePtOrd->Branch("diTauMinMass",&diTauMinMass,"diTauMisMass/F");

  outTreePtOrd->Branch("etaL1",   &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2",   &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",    &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",    &ptL2,"ptL2/F");
  outTreePtOrd->Branch("phiL1",   &phiL1,"phiL1/F");
  outTreePtOrd->Branch("phiL2",   &phiL2,"phiL2/F");
  outTreePtOrd->Branch("dPhiL1L2",&dPhiL1L2,"dPhiL1L2/F");

  outTreePtOrd->Branch("visibleTauMass",&visibleTauMass_,"visibleTauMass/F");
  outTreePtOrd->Branch("HoP",                     &HoP,"HoP/F");
  outTreePtOrd->Branch("EoP",                     &EoP,"EoP/F");
  outTreePtOrd->Branch("emFraction",              &emFraction_,"emFraction/F");
  outTreePtOrd->Branch("leadPFChargedHadrMva",    &leadPFChargedHadrMva_,"leadPFChargedHadrMva/F");
  outTreePtOrd->Branch("hasGsf",                  &hasGsf_,"hasGsf/F");
  outTreePtOrd->Branch("signalPFGammaCands",      &signalPFGammaCands_,"signalPFGammaCands/F");
  outTreePtOrd->Branch("signalPFChargedHadrCands",&signalPFChargedHadrCands_,"signalPFChargedHadrCands/F");
  outTreePtOrd->Branch("etaMom2",                 &etaMom2,"etaMom2/F");
  outTreePtOrd->Branch("phiMom2",                 &phiMom2,"phiMom2/F");
  outTreePtOrd->Branch("gammaFrac",               &gammaFrac,"gammaFrac/F");



  outTreePtOrd->Branch("diTauCharge", &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",      &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("MtLeg1Corr",  &MtLeg1Corr_,"MtLeg1Corr/F");
  outTreePtOrd->Branch("pZeta",       &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaCorr",   &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVis",    &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("pZetaVisCorr",&pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEt",         &MEt,"MEt/F");
  outTreePtOrd->Branch("MEtCorr",     &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("pZetaSig",    &pZetaSig_,"pZetaSig/F");

  outTreePtOrd->Branch("combRelIsoLeg1",     &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("combRelIsoLeg1Beta", &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho,"combRelIsoLeg1Rho/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combIsoLeg2",        &combIsoLeg2,"combIsoLeg2/F");

  outTreePtOrd->Branch("tightestHPSDBWP",  &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("decayMode",        &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightMu",  &HLTweightMu,"HLTweightMu/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTMu",        &HLTMu,"HLTMu/F");
  outTreePtOrd->Branch("HLTTau",       &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("SFMu",         &SFMu,"SFMu/F");

  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("muFlag",          &muFlag_,"muFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",       &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/MuTauStream_08Nov2011///treeMuTauStream_"+samples[index]+".root" : "../treeMuTauStream.root";

  TString inName(currentInName.c_str());
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }

  TString treeName(("muTauStreamAnalyzer"+analysis+"/tree").c_str());

  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries    = currentTree->GetEntries() ;
  int nEventsRead = ((TH1F*)file->Get("allEventsFilter/totalEvents"))->GetBinContent(1) ;
  float crossSection = index >= 0 ? crossSec[index] : float(nEventsRead)/Lumi ; 
  float scaleFactor = (crossSection != 0) ? Lumi / (  float(nEventsRead)/crossSection )  : 1.0;

  cout << "Processing sample " << sample.Data() << endl;
  cout<< "nEventsRead = " << nEventsRead << endl;
  cout<< "nEntries    = " << nEntries << endl;
  cout<< "crossSection " << crossSection << " pb ==> scaleFactor " << scaleFactor << endl;

  // jets
  currentTree->SetBranchStatus("jetsP4"                ,0);
  currentTree->SetBranchStatus("jetsIDP4"              ,1);
  currentTree->SetBranchStatus("jetsIDUpP4"            ,1);
  currentTree->SetBranchStatus("jetsIDDownP4"          ,1);
  currentTree->SetBranchStatus("jetsIDL1OffsetP4"      ,1);
  currentTree->SetBranchStatus("genJetsIDP4"           ,0);
  currentTree->SetBranchStatus("jetsBtagHE"            ,1);
  currentTree->SetBranchStatus("jetsBtagHP"            ,1);
  currentTree->SetBranchStatus("jetMoments"            ,0);
  currentTree->SetBranchStatus("jetsChNfraction"       ,1);
  currentTree->SetBranchStatus("jetsChEfraction"       ,0);

  // diTaus
  currentTree->SetBranchStatus("diTauVisP4"            ,1);
  currentTree->SetBranchStatus("diTauCAP4"             ,0);
  currentTree->SetBranchStatus("diTauICAP4"            ,1); 
  currentTree->SetBranchStatus("diTauSVfitP4"          ,1);
  currentTree->SetBranchStatus("diTauNSVfitMass"       ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp"  ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("diTauCharge"           ,1);

  // taus
  currentTree->SetBranchStatus("diTauLegsP4"           ,1);
  currentTree->SetBranchStatus("genDiTauLegsP4"        ,1);
  currentTree->SetBranchStatus("chIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("nhIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("phIsoLeg1v1"           ,0);
  currentTree->SetBranchStatus("chIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("nhIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("phIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("chIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("nhIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("phIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("chIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("nhIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("phIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("chIsoLeg2"             ,1);
  currentTree->SetBranchStatus("nhIsoLeg2"             ,0);
  currentTree->SetBranchStatus("phIsoLeg2"             ,1);
  currentTree->SetBranchStatus("dxy1"                  ,0);
  currentTree->SetBranchStatus("dxy2"                  ,0);
  currentTree->SetBranchStatus("dz1"                   ,0);
  currentTree->SetBranchStatus("dz2"                   ,0);
  currentTree->SetBranchStatus("decayMode"             ,1);
  currentTree->SetBranchStatus("tightestHPSWP"         ,0);
  currentTree->SetBranchStatus("tightestHPSDBWP"       ,1);
  currentTree->SetBranchStatus("visibleTauMass"        ,1);
  currentTree->SetBranchStatus("isTauLegMatched"       ,1);
  currentTree->SetBranchStatus("isMuLegMatched"        ,0);
  currentTree->SetBranchStatus("hasKft"                ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrPt"   ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrP"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackPt"    ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackP"     ,0);
  currentTree->SetBranchStatus("leadPFChargedHadrMva"  ,1);
  currentTree->SetBranchStatus("emFraction"            ,1);
  currentTree->SetBranchStatus("hasGsf"                ,1);
  currentTree->SetBranchStatus("signalPFChargedHadrCands"    ,1);
  currentTree->SetBranchStatus("signalPFGammaCands"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrHcalEnergy" ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrEcalEnergy" ,1);
  currentTree->SetBranchStatus("gammadEta"             ,1);
  currentTree->SetBranchStatus("gammadPhi"             ,1);
  currentTree->SetBranchStatus("gammaPt"               ,1);

  // MET
  currentTree->SetBranchStatus("METP4"                 ,1);
  currentTree->SetBranchStatus("genMETP4"              ,1);
  currentTree->SetBranchStatus("sumEt"                 ,0);
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,0);


  // generator-level boson
  currentTree->SetBranchStatus("genVP4"                ,1);
  currentTree->SetBranchStatus("genDecay"              ,1);

  // extra-electrons
  currentTree->SetBranchStatus("extraMuons"            ,0);
  currentTree->SetBranchStatus("pfMuons"               ,0);
  currentTree->SetBranchStatus("muFlag"                ,1);

  // event-dependent variables
  currentTree->SetBranchStatus("rhoFastJet"            ,1);
  currentTree->SetBranchStatus("rhoNeutralFastJet"     ,1);
  currentTree->SetBranchStatus("numPV"                 ,1);
  currentTree->SetBranchStatus("nPUVertices"           ,1);
  currentTree->SetBranchStatus("numOfDiTaus"           ,0);
  currentTree->SetBranchStatus("numOfLooseIsoDiTaus"   ,1);
 
  currentTree->SetBranchStatus("run"                   ,1);
  currentTree->SetBranchStatus("event"                 ,1);
  currentTree->SetBranchStatus("lumi"                  ,1);
  currentTree->SetBranchStatus("mcPUweight"            ,1);
  currentTree->SetBranchStatus("embeddingWeight"       ,1);

  // triggers
  currentTree->SetBranchStatus("tauXTriggers"          ,1);
  currentTree->SetBranchStatus("triggerBits"           ,1);

  ////////////////////////////////////////////////////////////////////

  std::vector< LV >* jets           = new std::vector< LV >();

  if(analysis.find("JetUp")!=string::npos) 
    currentTree->SetBranchAddress("jetsIDUpP4",        &jets);
  else if(analysis.find("JetDown")!=string::npos)
    currentTree->SetBranchAddress("jetsIDDownP4",      &jets);
  else
    currentTree->SetBranchAddress("jetsIDP4",          &jets);

  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",&jets_v2);

  std::vector< LV >* diTauLegsP4    = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauLegsP4",     &diTauLegsP4);

  std::vector< LV >* diTauVisP4     = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauVisP4",      &diTauVisP4);

  std::vector< LV >* diTauSVfitP4     = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauSVfitP4",    &diTauSVfitP4);

  std::vector< LV >* diTauCAP4    = new std::vector< LV >();
  currentTree->SetBranchAddress("diTauICAP4",    &diTauCAP4);

  std::vector< LV >* genDiTauLegsP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("genDiTauLegsP4",    &genDiTauLegsP4);

  std::vector< LV >* genVP4         = new std::vector< LV >();
  currentTree->SetBranchAddress("genVP4",          &genVP4);

  std::vector< LV >* METP4          = new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",           &METP4);

  std::vector< LV >* genMETP4          = new std::vector< LV >();
  currentTree->SetBranchAddress("genMETP4",        &genMETP4);

  std::vector< int >* tauXTriggers  = new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",    &tauXTriggers);
  std::vector< int >* triggerBits   = new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",     &triggerBits);

  std::vector< double >* jetsBtagHE = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",      &jetsBtagHE);
  std::vector< double >* jetsBtagHP = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHP",      &jetsBtagHP);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChNfraction", &jetsChNfraction);

  std::vector< float >* gammadEta =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadEta", &gammadEta);

  std::vector< float >* gammadPhi =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadPhi", &gammadPhi);

  std::vector< float >* gammaPt =  new std::vector< float >();
  currentTree->SetBranchAddress("gammaPt", &gammaPt);

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;;
  float diTauCharge;
  int tightestHPSDBWP, decayMode;
  float numPV;
  int numOfLooseIsoDiTaus;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float mcPUweight,embeddingWeight;
  int isTauLegMatched,muFlag,genDecay, nPUVertices;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  ULong64_t event,run,lumi;

  currentTree->SetBranchAddress("chIsoLeg2",            &chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",            &phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",          &chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",          &nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1v2",          &phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",        &chIsoPULeg1);
  currentTree->SetBranchAddress("nhIsoPULeg1v2",        &nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",        &phIsoPULeg1);
  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);
  currentTree->SetBranchAddress("rhoNeutralFastJet",    &rhoNeutralFastJet);
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("diTauCharge",          &diTauCharge);

  currentTree->SetBranchAddress("diTauNSVfitMass",       &diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);
  currentTree->SetBranchAddress("mTauTauMin",            &mTauTauMin);

  currentTree->SetBranchAddress("pZetaSig",             &pZetaSig);

  currentTree->SetBranchAddress("numPV",                &numPV);
  currentTree->SetBranchAddress("mcPUweight",           &mcPUweight);
  currentTree->SetBranchAddress("embeddingWeight",      &embeddingWeight);
  currentTree->SetBranchAddress("event",                &event);
  currentTree->SetBranchAddress("run",                  &run);
  currentTree->SetBranchAddress("lumi",                 &lumi);
  currentTree->SetBranchAddress("nPUVertices",          &nPUVertices);
  currentTree->SetBranchAddress("genDecay",             &genDecay);
  currentTree->SetBranchAddress("decayMode",            &decayMode);
  currentTree->SetBranchAddress("numOfLooseIsoDiTaus",  &numOfLooseIsoDiTaus);
  currentTree->SetBranchAddress("muFlag",               &muFlag);
  currentTree->SetBranchAddress("isTauLegMatched",      &isTauLegMatched);
  currentTree->SetBranchAddress("visibleTauMass",       &visibleTauMass);
  currentTree->SetBranchAddress("leadPFChargedHadrP",   &leadPFChargedHadrCandP);
  currentTree->SetBranchAddress("leadPFChargedHadrMva" ,&leadPFChargedHadrMva);
  currentTree->SetBranchAddress("emFraction"           ,&emFraction);
  currentTree->SetBranchAddress("hasGsf"               ,&hasGsf);
  currentTree->SetBranchAddress("signalPFChargedHadrCands"    ,&signalPFChargedHadrCands);
  currentTree->SetBranchAddress("signalPFGammaCands"   ,&signalPFGammaCands);
  currentTree->SetBranchAddress("leadPFChargedHadrHcalEnergy" ,&leadPFChargedHadrHcalEnergy);
  currentTree->SetBranchAddress("leadPFChargedHadrEcalEnergy" ,&leadPFChargedHadrEcalEnergy);

  unsigned int indexHelp = index >=0 ? index : 0;
  
  for (int n = 0; n < nEntries ; n++) {
    
    currentTree->GetEntry(n);
    if(n%2000==0) cout << n << endl;
    
    // initialize variables filled only in the two jet case
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;phi1=-99;phi2=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;phi1_v2=-99;phi2_v2=-99;
    ptVeto = -99; phiVeto= -99; etaVeto= -99;isVetoInJets=-99;
    chFracPV1=-99;chFracPV2=-99;chFracPVVeto=-99;jetsBtagHE1 = -99;jetsBtagHE2 = -99;jetsBtagHP1 = -99;jetsBtagHP2 = -99;

    // define the relevant jet collection 
    nJets20BTagged = 0;
    nJets30        = 0;
    int lead  = -99;
    int trail = -99;
    int veto  = -99;
    vector<int> indexes;
    for(int l = 0 ; l < jets->size() ; l++){
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta)
	indexes.push_back(l);
      if((*jetsBtagHE)[l] > 3.3) nJets20BTagged++;
    }
    nJets20 = indexes.size();
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];  

    for(int v = 0 ; v < indexes.size() ; v++){
      if( (*jets)[indexes[v]].Pt() > 30 ) nJets30++;
    }

    // first jet
    if(lead>=0){

      pt1  = (*jets)[lead].Pt();
      eta1 = (*jets)[lead].Eta();
      phi1 = (*jets)[lead].Phi();
      eta1_v2 = (*jets_v2)[lead].Eta();
      pt1_v2  = (*jets_v2)[lead].Pt();
      phi1_v2 = (*jets_v2)[lead].Phi();
      
      jetsBtagHE1 = (*jetsBtagHE)[lead];
      jetsBtagHP1 = (*jetsBtagHP)[lead];
      
      chFracPV1 = (*jetsChNfraction)[lead]; 

      // second jet
      if(trail>=0){

	pt2  = (*jets)[trail].Pt();
	eta2 = (*jets)[trail].Eta();
	phi2 = (*jets)[trail].Phi();
	
	pt2_v2  = (*jets_v2)[trail].Pt();
	eta2_v2 = (*jets_v2)[trail].Eta();
	phi2_v2 = (*jets_v2)[trail].Phi();
	
	Deta = abs(eta1-eta2);
	Dphi = abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
	  -abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi()  :
	  abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) ;
	Mjj  = ((*jets)[lead]+(*jets)[trail]).M();
	
	Deta_v2 = abs(eta1-eta2);
	Dphi_v2 = abs((*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi()) > TMath::Pi() ? 
	  -abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) + 2*TMath::Pi()  :
	  abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) ;
	Mjj_v2  = ((*jets_v2)[lead]+(*jets_v2)[trail]).M();
	
	jetsBtagHE2 = (*jetsBtagHE)[trail];
	jetsBtagHP2 = (*jetsBtagHP)[trail];

	chFracPV2 = (*jetsChNfraction)[trail]; 

      }
      
    }

    ptVeto  = (veto>=0) ? (*jets)[veto].Pt() : -99;
    etaVeto = (veto>=0) ? (*jets)[veto].Eta(): -99;
    phiVeto = (veto>=0) ? (*jets)[veto].Phi(): -99;
    chFracPVVeto = (veto>=0) ? (*jetsChNfraction)[veto] : -99; 
 
    isVetoInJets = 0;
    for(int l = 0 ; l < indexes.size() ; l++){
      if(lead>=0 && trail>=0 && (l!= lead && l!= trail) &&
	 (*jets)[indexes[l]].Pt()>PtVETO && ((*jets)[indexes[l]].Eta() - eta1)*((*jets)[indexes[l]].Eta() - eta2)<=0 )
	isVetoInJets = 1;
    }

    diTauNSVfitMass_        = diTauNSVfitMass;
    diTauNSVfitMassErrUp_   = diTauNSVfitMassErrUp;
    diTauNSVfitMassErrDown_ = diTauNSVfitMassErrDown;

    diTauSVFitMass = (*diTauSVfitP4)[0].M();
    diTauSVFitPt   = (*diTauSVfitP4)[0].Pt();
    diTauSVFitEta  = (*diTauSVfitP4)[0].Eta();
    diTauSVFitPhi  = (*diTauSVfitP4)[0].Phi();

    diTauCAMass = (*diTauCAP4)[0].M();
    diTauCAPt   = (*diTauCAP4)[0].Pt();
    diTauCAEta  = (*diTauCAP4)[0].Eta();
    diTauCAPhi  = (*diTauCAP4)[0].Phi();

    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt    = (*diTauVisP4)[0].Pt();
    diTauVisEta   = (*diTauVisP4)[0].Eta();
    diTauVisPhi   = (*diTauVisP4)[0].Phi();

    diTauMinMass  = mTauTauMin;
    
    ptL1     = (*diTauLegsP4)[0].Pt();
    ptL2     = (*diTauLegsP4)[1].Pt();
    etaL1    = (*diTauLegsP4)[0].Eta();
    etaL2    = (*diTauLegsP4)[1].Eta();
    phiL1    = (*diTauLegsP4)[0].Phi();
    phiL2    = (*diTauLegsP4)[1].Phi();
    dPhiL1L2 =  abs((*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi()) > TMath::Pi() ? 
      -abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) + 2*TMath::Pi()  :
      abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) ;

    visibleTauMass_ = visibleTauMass;
    diTauCharge_    = diTauCharge;
    
    float scalarSumPt     = ( *diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt     = ((*diTauLegsP4)[0] +      (*METP4)[0]).Pt() ;
    float scalarSumPtCorr = ( *diTauLegsP4)[0].Pt() + (*METP4)[1].Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] +      (*METP4)[1]).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[0];

    pZetaSig_     = pZetaSig;

    MEt     = (*METP4)[0].Et();
    MEtCorr = (*METP4)[1].Et();

    combRelIsoLeg1      = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    float PUtoPVratio = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta = (chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    float EffArea = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2 =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    

    tightestHPSDBWP_ = tightestHPSDBWP;
    decayMode_       = decayMode;
    numPV_           = numPV;
    sampleWeight     = scaleFactor; 
    //puWeight         = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight ;
    puWeight         = pileupWeight(nPUVertices);   

    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    float sumPt  = 0;
    float dEta2  = 0;
    float dPhi2  = 0;
    float sumPt2 = 0;

    for(unsigned int k = 0 ; k < gammaPt->size() ; k++){
      float pt_k  = (*gammaPt)[k];
      float phi_k = (*gammadPhi)[k];
      if ((*gammadPhi)[k] > TMath::Pi()) phi_k = (*gammadPhi)[k] - 2*TMath::Pi();
      else if((*gammadPhi)[k] < -TMath::Pi()) phi_k = (*gammadPhi)[k] + 2*TMath::Pi();

      float eta_k = (*gammadEta)[k];

      sumPt  += pt_k;
      sumPt2 += (pt_k*pt_k);
      dEta2 += (pt_k*eta_k*eta_k);
      dPhi2 += (pt_k*phi_k*phi_k);  
    }

    gammaFrac = 0.;
    if(sumPt>0){
      dEta2 /= sumPt;
      dPhi2 /= sumPt;
      gammaFrac = sumPt/ptL2;
    }

    etaMom2 = TMath::Sqrt(dEta2)*TMath::Sqrt(gammaFrac)*ptL2;
    phiMom2 = TMath::Sqrt(dPhi2)*TMath::Sqrt(gammaFrac)*ptL2;

    HoP                       = leadPFChargedHadrHcalEnergy/leadPFChargedHadrCandP;
    EoP                       = leadPFChargedHadrEcalEnergy/leadPFChargedHadrCandP;
    emFraction_               = emFraction;
    leadPFChargedHadrMva_     = leadPFChargedHadrMva;
    visibleTauMass_           = visibleTauMass;
    hasGsf_                   = hasGsf; 
    signalPFGammaCands_       = signalPFGammaCands; 
    signalPFChargedHadrCands_ = signalPFChargedHadrCands;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]);  //HLT_IsoMu12_LooseIsoPFTau10_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]);  //HLT_IsoMu12_LooseIsoPFTau10_v2
      else if(run>=163269 && run<=163869)
	HLTx = 	float((*triggerBits)[2]);  //HLT_IsoMu12_LooseIsoPFTau10_v4
      else if(run>=165088 && run<=165633)
	HLTx = 	float((*triggerBits)[3]);  //HLT_IsoMu15_LooseIsoPFTau15_v2
      else if(run>=165970 && run<=167043 && run!=166346)
	HLTx = 	float((*triggerBits)[4]);  //HLT_IsoMu15_LooseIsoPFTau15_v4
      else if(run==166346)
	HLTx =  float((*triggerBits)[5]);  //HLT_IsoMu15_LooseIsoPFTau15_v5      
      else if(run>=167078 && run<=167913)
	HLTx =  float((*triggerBits)[6]);  //HLT_IsoMu15_LooseIsoPFTau15_v6      
      else if(run>=170249 && run<=173198)
	HLTx =  float((*triggerBits)[7]);  //HLT_IsoMu15_LooseIsoPFTau15_v8
      else if(run>=173236 && run<=173692)
	HLTx =  float((*triggerBits)[8]);  //HLT_IsoMu15_LooseIsoPFTau15_v9
      else if(run>=175860 && run<=178380)
	HLTx =  float((*triggerBits)[9]);  //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v1
      else if(run>=178420 && run<=179889)
	HLTx =  float((*triggerBits)[10]); //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v5
      else if(run>=179959 && run<=180252)
	HLTx =  float((*triggerBits)[11]); //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v6
     

      if(run>=160404 && run<=163869){
	bool isTriggMatched = (*tauXTriggers)[0]  && (*tauXTriggers)[2] ; //hltSingleMuIsoL3IsoFiltered12 && hltOverlapFilterIsoMu12IsoPFTau10
	HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      }
      else if(run>=165088 && run<=178380){
	bool isTriggMatched = ((*tauXTriggers)[1] || true ) && ((*tauXTriggers)[3] || (*tauXTriggers)[4]); //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau15
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      else if(run>=178420 && run<=180252){
	bool isTriggMatched = ((*tauXTriggers)[1] || true ) && (*tauXTriggers)[4] ; //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }

      HLTweightTau = 1.0;
      HLTweightMu  = 1.0;
      if( (std::string(sample.Data())).find("Embed")!=string::npos ){
	HLTMu  = turnOnMuAll->Eval( (*diTauLegsP4)[1].Pt() );
	HLTTau = turnOnTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      }
      else{
	HLTMu  = 1.0;
	HLTTau = 1.0;
      }

      SFTau = 1.0;
      SFMu  = 1.0;
     
    } else{


      HLTx =  float((*triggerBits)[1]); //[1]HLT_IsoMu12_LooseIsoPFTau10_v2
      bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[2] ; //hltSingleMuIsoL3IsoFiltered12 && hltOverlapFilterIsoMu12IsoPFTau10
      HLTmatch = isTriggMatched ? 1.0 : 0.0;

      HLTweightTau = ratioTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTweightMu  = ratioMuAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTMu  = turnOnMuAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTTau = turnOnTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() ); 
      SFTau  = 1.0;
      SFMu   = ratioMuID->Eval( (*diTauLegsP4)[1].Pt() )*ratioMuIso->Eval( (*diTauLegsP4)[1].Pt() );

    }
   
    isTauLegMatched_ = isTauLegMatched;
    if((std::string(sample.Data())).find("Run2011")==string::npos)
      leptFakeTau      = (isTauLegMatched==0 && (*genDiTauLegsP4)[1].E()>0) ? 1 : 0;
    else 
      leptFakeTau = -99;
    muFlag_          = muFlag;
    genDecay_        = genDecay ;
    event_           = event;
    run_             = run;
    lumi_            = lumi;
    
    outTreePtOrd->Fill();
  }


  file->Close();
  
  if(SAVE) outFile->Write();
  outFile->Close();

  delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfitP4; delete diTauCAP4; delete genDiTauLegsP4;
  delete tauXTriggers; delete triggerBits;
  delete METP4; delete jetsBtagHE; delete jetsBtagHP; delete jetsChNfraction; delete genVP4; delete genMETP4;
  delete gammadEta; delete gammadPhi; delete gammaPt;
  
  return;

}


void doAllSamplesElec(){

  gSystem->Load("AntiElectronIDMVA_C.so");
  AntiElectronIDMVA* antiE = new AntiElectronIDMVA();
  antiE->Initialize("BDT",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_X_0BL_BDT.weights.xml",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_1_1BL_BDT.weights.xml",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_0_1BL_BDT.weights.xml",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_X_0EC_BDT.weights.xml",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_1_1EC_BDT.weights.xml",
		    "../../../Utilities/data/antiE_v4/TMVAClassification_v2_0_1EC_BDT.weights.xml"
		    );
  
  for( unsigned int k = 1; k < 37 ; k++) {
    makeTrees_ElecTauStream("",                  k,antiE);
    if( true || k==0 || k==1) continue;
    makeTrees_ElecTauStream("JetUp",             k,antiE);
    makeTrees_ElecTauStream("JetDown",           k,antiE);
    makeTrees_ElecTauStream("MEtResolutionUp",   k,antiE);
    makeTrees_ElecTauStream("MEtResolutionUp",   k,antiE);
    makeTrees_ElecTauStream("MEtResponseUp",     k,antiE);
    makeTrees_ElecTauStream("MEtResponseUp",     k,antiE);
    makeTrees_ElecTauStream("TauUp",             k,antiE);
    makeTrees_ElecTauStream("TauDown",           k,antiE);
    makeTrees_ElecTauStream("ElecUp",            k,antiE);
    makeTrees_ElecTauStream("ElecDown",          k,antiE);
  }
  
  return;
  
}


void doAllSamplesMu(){
 
  for( unsigned int k = 0; k < 37 ; k++) {
    makeTrees_MuTauStream("",        k);
    ///if( k==0 ) continue;
    continue;
    makeTrees_MuTauStream("JetUp",   k);
    makeTrees_MuTauStream("JetDown", k);
    makeTrees_MuTauStream("MEtResolutionUp",   k);
    makeTrees_MuTauStream("MEtResolutionUp",   k);
    makeTrees_MuTauStream("MEtResponseUp",     k);
    makeTrees_MuTauStream("MEtResponseUp",     k);
    makeTrees_MuTauStream("TauUp",   k);
    makeTrees_MuTauStream("TauDown", k);
    makeTrees_MuTauStream("MuUp",    k);
    makeTrees_MuTauStream("MuDown",  k);
  }

  return;

}

