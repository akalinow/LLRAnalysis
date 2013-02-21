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
#include "ratioEfficiencyElec.C"
#include "ratioEfficiencyTau.C"
#include "ratioEfficiencyTauMedium.C"
#include "ratioEfficiencyTauTight.C"
#include "ratioEfficiencyTau10.C"
#include "ratioEfficiencyTau15.C"
#include "recoilCorrectionAlgorithm.C"
#include "RecoilCorrector.C"
#include "TLorentzVector.h"
#include "TRandom3.h"

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


void makeTrees_ElecTauStream(string analysis = "", int index = -1 ){

  cout << "Now skimming analysys " << analysis << endl;

  gSystem->Load("ratioEfficiencyElec_C.so");
  ratioEfficiencyElec* ratioEffElec = new ratioEfficiencyElec();

  // scale factors for PFTau20
  gSystem->Load("ratioEfficiencyTau_C.so");
  ratioEfficiencyTau* ratioEffTau = new ratioEfficiencyTau();

  // scale factors for PFTau20Medium
  gSystem->Load("ratioEfficiencyTauMedium_C.so");
  ratioEfficiencyTauMedium* ratioEffTauMedium = new ratioEfficiencyTauMedium();

  // scale factors for PFTau20Tight
  gSystem->Load("ratioEfficiencyTauTight_C.so");
  ratioEfficiencyTauTight* ratioEffTauTight   = new ratioEfficiencyTauTight();

  // scale factors for PFTau10
  gSystem->Load("ratioEfficiencyTau10_C.so");
  ratioEfficiencyTau10* ratioEffTau10 = new ratioEfficiencyTau10();

  // scale factors for PFTau15
  gSystem->Load("ratioEfficiencyTau15_C.so");
  ratioEfficiencyTau15* ratioEffTau15 = new ratioEfficiencyTau15();

  gSystem->Load("RecoilCorrector_C.so");
  RecoilCorrector* recoilCorr = new RecoilCorrector();

  gSystem->Load("recoilCorrectionAlgorithm_C.so");
  recoilCorrectionAlgorithm* recoilCorr2 = new recoilCorrectionAlgorithm(0);

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  // pu weights
  TFile fgen("/data_CMS/cms/lbianchini/VbfJetsStudy/Utilities/pileUp_EPS2011.root");
  TFile fdat("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/PileUp/Pileup_2011_to_173692_LPLumiScale_68mb.root");
  
  TH1F *hgen = (TH1F*)fgen.Get("hMCOneBX");
  TH1F *hdat = (TH1F*)fdat.Get("pileup");

  TH1F *hgen2 = (TH1F*)hdat->Clone("hgen2");
  for(int k=1; k <= hgen2->GetNbinsX(); k++){
    if(k==hgen2->GetNbinsX()){
      hgen2->SetBinContent(k, 0);
      continue;
    }
    hgen2->SetBinContent(k, hgen->GetBinContent(k));
  }
  
  TH1F *hRatio = (TH1F*)hdat->Clone("hRatio");
  hRatio->Reset();

  hdat->Scale(1./hdat->Integral());
  hgen2->Scale(1./hgen2->Integral());

  hRatio->Divide(hdat,hgen2,1,1) ;

  // rho weights
  TFile frho("/data_CMS/cms/lbianchini/VbfJetsStudy/Utilities/vertexMultiplicityVsRhoPFNeutralReweight.root");
  TH2F* hRho = (TH2F*)frho.Get("histoReweight_fitted");

  TRandom3* tRandom = new TRandom3();

  std::vector<std::string> samples;
  std::vector<float> crossSec;
  std::vector<int> readEvents;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011A-ElecTau-All_run");            crossSec.push_back( 0  );                          
  samples.push_back("DYJets-ElecTau-50-madgraph-PUS4_run"); crossSec.push_back( 3048           * 0.0524629   * 0.2063810); 
  samples.push_back("TTJets-ElecTau-madgraph-PUS4_run");    crossSec.push_back( 157.5          * 0.0141612   * 0.7417971);  
  samples.push_back("WJets-ElecTau-madgraph-PUS4_run");     crossSec.push_back( 31314.0        * 0.0010836   * 0.5023370);   
  samples.push_back("WZ-ElecTau-pythia-PUS4_run");          crossSec.push_back( 18.2           * 0.0108603   * 0.4871293);         
  samples.push_back("ZZ-ElecTau-pythia-PUS4_run");          crossSec.push_back(  5.9           * 0.0155201   * 0.4611936);    
  samples.push_back("WW-ElecTau-pythia-PUS4_run");          crossSec.push_back( 43.0           * 0.0089326   * 0.5191052);        
  samples.push_back("VBFH100-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*1.546 * 0.0501993   * 0.6937387); 
  samples.push_back("GGFH100-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.36e-02*24.02 * 0.0375728   * 0.6495874); 
  samples.push_back("VBFH105-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*1.472 * 1.0         * 0.1034317); 
  samples.push_back("GGFH105-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.25e-02*21.78 * 0.0400086   * 0.6581677); 
  samples.push_back("VBFH110-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*1.398 * 0.0564719   * 0.6898090);
  samples.push_back("GGFH110-ElecTau-powheg-PUS4_run");     crossSec.push_back( 8.02e-02*19.84 * 0.0444077   * 0.6515032);    
  samples.push_back("VBFH115-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*1.332 * 0.0593790   * 0.6879252);
  samples.push_back("GGFH115-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.65e-02*18.13 * 0.0470898   * 0.6560061);  
  samples.push_back("VBFH120-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*1.269 * 0.0618780   * 0.6848913);   
  samples.push_back("GGFH120-ElecTau-powheg-PUS4_run");     crossSec.push_back( 7.10e-02*16.63 * 0.0491247   * 0.6530944);  
  samples.push_back("VBFH125-ElecTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*1.211 * 0.0651673   * 0.6914162);   
  samples.push_back("GGFH125-ElecTau-powheg-PUS4_run");     crossSec.push_back( 6.37e-02*15.31 * 0.0527237   * 0.6582891); 
  samples.push_back("VBFH130-ElecTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*1.154 * 1.0         * 0.1210558); 
  samples.push_back("GGFH130-ElecTau-powheg-PUS4_run");     crossSec.push_back( 5.48e-02*14.12 * 0.055105    * 0.6489805);
  samples.push_back("VBFH135-ElecTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*1.100 * 0.0716975   * 0.6897157); 
  samples.push_back("GGFH135-ElecTau-powheg-PUS4_run");     crossSec.push_back( 4.52e-02*13.08 * 1.0         * 0.0831602);    
  samples.push_back("VBFH140-ElecTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*1.052 * 0.0730735   * 0.6933992);  
  samples.push_back("GGFH140-ElecTau-powheg-PUS4_run");     crossSec.push_back( 3.54e-02*12.13 * 0.0608360   * 0.6521164);  
  samples.push_back("VBFH145-ElecTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*1.004 * 0.0753342   * 0.6952451);  
  samples.push_back("GGFH145-ElecTau-powheg-PUS4_run");     crossSec.push_back( 2.61e-02*11.27 * 0.0624890   * 0.6485596);  

  samples.push_back("VH100-ElecTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 1.0 * 0.1359530);  
//samples.push_back("VH105-ElecTau-pythia-PUS4_run");  crossSec.push_back( 8.25e-02*(1.018+ 0.5449+0.1433  ) * 1.0 * 0.); 
  samples.push_back("VH110-ElecTau-pythia-PUS4_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 1.0 * 0.1476877);
  samples.push_back("VH115-ElecTau-pythia-PUS4_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 1.0 * 0.1530700);
  samples.push_back("VH120-ElecTau-pythia-PUS4_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0 * 0.1582757);   
  samples.push_back("VH125-ElecTau-pythia-PUS4_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 1.0 * 0.1630762);
  samples.push_back("VH130-ElecTau-pythia-PUS4_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 1.0 * 0.1694439);
  samples.push_back("VH135-ElecTau-pythia-PUS4_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 1.0 * 0.1733857);
  samples.push_back("VH140-ElecTau-pythia-PUS4_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 1.0 * 0.1752277);  
  samples.push_back("VH145-ElecTau-pythia-PUS4_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 1.0 * 0.1793439);  
  

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
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2,visibleTauMass_;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,pZetaSig_;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;

  // electron related variables
  int tightestCutBasedWP_, tightestCiCWP_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, rhoNeutralWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightElec,HLTweightTau;
  int isTauLegMatched_,elecFlag_,genDecay_, leptFakeTau;

  // event id
  ULong64_t event_,run_,lumi_;

  // random number
  float ran;

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

  outTreePtOrd->Branch("tightestCutBasedWP",  &tightestCutBasedWP_,"tightestCutBasedWP/I");
  outTreePtOrd->Branch("tightestCiCWP",       &tightestCiCWP_,"tightestCiCWP/I");

  outTreePtOrd->Branch("tightestHPSDBWP",  &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("decayMode",        &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("rhoNeutralWeight",   &rhoNeutralWeight,"rhoNeutralWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightElec",&HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");

  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("elecFlag",        &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",       &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");

  outTreePtOrd->Branch("ran", &ran, "ran/F");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/ElecTauStream_13Oct2011//treeElecTauStream_"+samples[index]+".root" : "../treeElecTauStream.root";

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
  currentTree->SetBranchStatus("leadPFChargedHadrCandTrackPt",0);

  //electron specific
  currentTree->SetBranchStatus("nBrehm"                ,0);
  currentTree->SetBranchStatus("likelihood"            ,0);
  currentTree->SetBranchStatus("nHits"                 ,0);
  currentTree->SetBranchStatus("sihih"                 ,0);
  currentTree->SetBranchStatus("dPhi"                  ,0);
  currentTree->SetBranchStatus("dEta"                  ,0);
  currentTree->SetBranchStatus("HoE"                   ,0);
  currentTree->SetBranchStatus("EoP"                   ,0);
  currentTree->SetBranchStatus("fbrem"                 ,0);
  currentTree->SetBranchStatus("tightestCutBasedWP"    ,1);
  currentTree->SetBranchStatus("tightestCiCWP"         ,1);

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
  currentTree->SetBranchStatus("rhoNeutralFastJet"     ,1);
  currentTree->SetBranchStatus("numPV"                 ,1);
  currentTree->SetBranchStatus("nPUVertices"           ,1);
  currentTree->SetBranchStatus("numOfDiTaus"           ,0);
  currentTree->SetBranchStatus("numOfLooseIsoDiTaus"   ,1);
 
  currentTree->SetBranchStatus("run"                   ,1);
  currentTree->SetBranchStatus("event"                 ,1);
  currentTree->SetBranchStatus("lumi"                  ,1);
  currentTree->SetBranchStatus("mcPUweight"            ,1);

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

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;
  float diTauCharge;
  int tightestHPSDBWP, decayMode;
  int tightestCutBasedWP, tightestCiCWP;
  float numPV;
  int numOfLooseIsoDiTaus;
  int isTauLegMatched,elecFlag,genDecay, nPUVertices;
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
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("diTauCharge",          &diTauCharge);

  currentTree->SetBranchAddress("diTauNSVfitMass",       &diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);
  currentTree->SetBranchAddress("mTauTauMin",            &mTauTauMin);

  currentTree->SetBranchAddress("pZetaSig",             &pZetaSig);

  currentTree->SetBranchAddress("numPV",                &numPV);
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
    for(int l = 0 ; l < jets->size() ; l++){
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta){
	indexes.push_back(l);
	if((*jetsBtagHE)[l] > 3.3) nJets20BTagged++;
      }
    }
    nJets20 = indexes.size();
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];

    for(int v = 0 ; v < indexes.size() ; v++){
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
  
    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    unsigned int indexHelp = index >=0 ? index : 0;
    if(genVP4->size()){
      if(samples[indexHelp].find("WJets")  !=string::npos)      
	recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi()  );
      else if(samples[indexHelp].find("DYJets")!=string::npos || samples[indexHelp].find("VBF")  !=string::npos || samples[indexHelp].find("GGF")  !=string::npos)  
	recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi()  );
    }
    corrMET_tmp.SetPtEtaPhiM(corrPt,0,corrPhi,0);
    corrMET.SetPx(corrMET_tmp.Px());
    corrMET.SetPy(corrMET_tmp.Py());
    corrMET.SetPz(corrMET_tmp.Pz());
    corrMET.SetE(corrMET_tmp.E());

    if(USERECOILALGO && genVP4->size() && genMETP4->size() && (samples[indexHelp].find("WJets")  !=string::npos 
							       || samples[indexHelp].find("DYJets")  !=string::npos
							       || samples[indexHelp].find("VBF")  !=string::npos
							       || samples[indexHelp].find("GGF")  !=string::npos) )
      corrMET = recoilCorr2->buildZllCorrectedMEt( (*METP4)[0] , (*genMETP4)[0] , (*genVP4)[0]);
    
    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    float scalarSumPtCorr = (*diTauLegsP4)[0].Pt() + corrMET.Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] + corrMET).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[0];

    pZetaSig_     = pZetaSig;

    MEt     = (*METP4)[0].Et();
    MEtCorr = corrMET.Et();

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

    tightestHPSDBWP_ = tightestHPSDBWP;
    decayMode_       = decayMode;

    numPV_              = numPV;
    sampleWeight        = scaleFactor; 
    puWeight            = (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : hRatio->GetBinContent( hRatio->FindBin(nPUVertices) ) ;
    rhoNeutralWeight    =  (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : hRho->GetBinContent( hRho->FindBin(numPV,rhoNeutralFastJet) ); // to be implemented;
    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){

      ran=1.0;
      
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
	HLTx = 	float((*triggerBits)[7]); //HLT_Ele18_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_MediumIsoPFTau20_v1

      if(run>=160404 && run<=163869){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1])  && (*tauXTriggers)[4] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau15
	HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      }
      else if(run>=165088 && run<=167913){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1]) && (*tauXTriggers)[5] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      else if(run>=170249 && run<=173198){
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1]) && (*tauXTriggers)[6] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15TightIsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      else if(run>=173236){
	bool isTriggMatched = ((*tauXTriggers)[2] || (*tauXTriggers)[3]) && (*tauXTriggers)[7] ; //hltEle18CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15TightIsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }

      HLTweightTau = 1.0;

    } else{

      float LumiData = 2220.;
      ran = (float)tRandom->Uniform(0,1) ;
      // LoosePFTau15
      if( ran<= 168.6/LumiData ){ 
	HLTx = float((*triggerBits)[0]);
	bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[2] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau15
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
	HLTweightTau = ratioEffTau15->ratio( (*diTauLegsP4)[1].Pt() ) ;
      }
      // LoosePFTau20
      else if( ran>168.6/LumiData && ran<=(168.6+934)/LumiData ){
	HLTx = float((*triggerBits)[1]);
	bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[3] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
	HLTweightTau =  ratioEffTau->ratio( (*diTauLegsP4)[1].Pt() ) ;
      }
      // TightPFTau20
      else if( ran>(168.6+934)/LumiData && ran<=(168.6+934+785.7)/LumiData ){
	HLTx = float((*triggerBits)[1]);
	bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[3] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
	HLTweightTau =  ratioEffTauTight->ratio( (*diTauLegsP4)[1].Pt() );
      }
      // MediumPFTau20
      else{
	HLTx = float((*triggerBits)[1]);
	bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[3] ; //hltEle15CaloIdVTTrkIdTCaloIsoTTrkIsoTTrackIsolFilter && hltOverlapFilterIsoEle15IsoPFTau20
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
	HLTweightTau =  ratioEffTauMedium->ratio( (*diTauLegsP4)[1].Pt() );
      }

    }
 
    // attention to runs triggered by IsoEle18 !!!
    HLTweightElec = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEffElec->ratio((*diTauLegsP4)[0].Pt(), bool(fabs((*diTauLegsP4)[0].Eta())<1.5) ) : 1.0;

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
 delete ratioEffElec; delete ratioEffTau; delete ratioEffTau10; delete ratioEffTau15; delete recoilCorr; 
 delete tRandom;

 return;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void makeTrees_MuTauStream(string analysis = "", int index = -1 ){
  
  cout << "Now skimming analysis " << analysis << endl;
  
  // scale factors for PFTau20
  gSystem->Load("ratioEfficiencyTau_C.so");
  ratioEfficiencyTau* ratioEffTau = new ratioEfficiencyTau();

  // scale factors for PFTau10
  gSystem->Load("ratioEfficiencyTau10_C.so");
  ratioEfficiencyTau10* ratioEffTau10 = new ratioEfficiencyTau10();

  // scale factors for PFTau15
  gSystem->Load("ratioEfficiencyTau15_C.so");
  ratioEfficiencyTau15* ratioEffTau15 = new ratioEfficiencyTau15();

  gSystem->Load("RecoilCorrector_C.so");
  RecoilCorrector* recoilCorr = new RecoilCorrector();

  gSystem->Load("recoilCorrectionAlgorithm_C.so");
  recoilCorrectionAlgorithm* recoilCorr2 = new recoilCorrectionAlgorithm(0);

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  // pu weights
  TFile fgen("/data_CMS/cms/lbianchini/VbfJetsStudy/Utilities/pileUp_EPS2011.root");
  TFile fdat("/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions11/7TeV/PileUp/Pileup_2011_to_173692_LPLumiScale_68mb.root");

  TH1F *hgen = (TH1F*)fgen.Get("hMCOneBX");
  TH1F *hdat = (TH1F*)fdat.Get("pileup");

  TH1F *hgen2 = (TH1F*)hdat->Clone("hgen2");
  for(int k=1; k <= hgen2->GetNbinsX(); k++){
    if(k==hgen2->GetNbinsX()){
      hgen2->SetBinContent(k, 0);
      continue;
    }
    hgen2->SetBinContent(k, hgen->GetBinContent(k));
  }

  TH1F *hRatio = (TH1F*)hdat->Clone("hRatio");
  hRatio->Reset();

  hdat->Scale(1./hdat->Integral());
  hgen2->Scale(1./hgen2->Integral());
  hRatio->Divide(hdat,hgen2,1,1) ;

  TFile frho("/data_CMS/cms/lbianchini/VbfJetsStudy/Utilities/vertexMultiplicityVsRhoPFNeutralReweight.root");
  TH2F* hRho = (TH2F*)frho.Get("histoReweight_fitted");

  TFile fiso("/data_CMS/cms/lbianchini/VbfJetsStudy/Utilities/makeMuonIsolationPlots.root");
  TH2F* hiso = (TH2F*)fiso.Get("muonIsoEffPtVsEta");

  TRandom3* tRandom = new TRandom3();

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011A-MuTau-All_run");             crossSec.push_back( 0  );                          
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
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2,visibleTauMass_;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,pZetaSig_;;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, rhoNeutralWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightTau;
  int isTauLegMatched_,muFlag_,genDecay_,leptFakeTau;
  float qcdIsoWeight;

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
  outTreePtOrd->Branch("rhoNeutralWeight",   &rhoNeutralWeight,"rhoNeutralWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("qcdIsoWeight", &qcdIsoWeight,"qcdIsoWeight/F");

  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("muFlag",          &muFlag_,"muFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",       &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/MuTauStream_13Oct2011//treeMuTauStream_"+samples[index]+".root" : "../treeMuTauStream.root";

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
  currentTree->SetBranchStatus("leadPFChargedHadrCandTrackPt",0);

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

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;;
  float diTauCharge;
  int tightestHPSDBWP, decayMode;
  float numPV;
  int numOfLooseIsoDiTaus;
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
  
    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    unsigned int indexHelp = index >=0 ? index : 0;
    if(genVP4->size()){
      if(samples[indexHelp].find("WJets")  !=string::npos)      
	recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi()  );
      else if(samples[indexHelp].find("DYJets")!=string::npos || samples[indexHelp].find("VBF")  !=string::npos || samples[indexHelp].find("GGF")  !=string::npos)  
	recoilCorr->Correct(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi()  );
    }
    corrMET_tmp.SetPtEtaPhiM(corrPt,0,corrPhi,0);
    corrMET.SetPx(corrMET_tmp.Px());
    corrMET.SetPy(corrMET_tmp.Py());
    corrMET.SetPz(corrMET_tmp.Pz());
    corrMET.SetE(corrMET_tmp.E());

    if(USERECOILALGO && genVP4->size() && genMETP4->size() && (samples[indexHelp].find("WJets")  !=string::npos 
							       || samples[indexHelp].find("DYJets")  !=string::npos
							       || samples[indexHelp].find("VBF")  !=string::npos
							       || samples[indexHelp].find("GGF")  !=string::npos) ) 
      corrMET = recoilCorr2->buildZllCorrectedMEt( (*METP4)[0] , (*genMETP4)[0] , (*genVP4)[0]);
    
    float scalarSumPt = (*diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt = ((*diTauLegsP4)[0] + (*METP4)[0]).Pt() ;
    float scalarSumPtCorr = (*diTauLegsP4)[0].Pt() + corrMET.Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] + corrMET).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], corrMET))[0];

    pZetaSig_     = pZetaSig;

    MEt     = (*METP4)[0].Et();
    MEtCorr = corrMET.Et();

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
    numPV_              = numPV;
    sampleWeight        = scaleFactor; 
    puWeight            = (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : hRatio->GetBinContent( hRatio->FindBin(nPUVertices) ) ;
    rhoNeutralWeight    = (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : hRho->GetBinContent( hRho->FindBin(numPV,rhoNeutralFastJet) );
    qcdIsoWeight        = (std::string(sample.Data())).find("QCD")==string::npos ? 
      1.0 : hiso->GetBinContent( hiso->FindBin( (*diTauLegsP4)[0].Eta() ,(*diTauLegsP4)[0].Pt() ) );

    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    if((std::string(sample.Data())).find("Run2011")!=string::npos){
      
      if(run>=160404 && run<=161176)
	HLTx = 	float((*triggerBits)[0]); //HLT_IsoMu12_LooseIsoPFTau10_v1
      else if(run>=161216 && run<=163261)
	HLTx = 	float((*triggerBits)[1]); //HLT_IsoMu12_LooseIsoPFTau10_v2
      else if(run>=163269 && run<=163869)
	HLTx = 	float((*triggerBits)[2]); //HLT_IsoMu12_LooseIsoPFTau10_v4
      else if(run>=165088 && run<=165633)
	HLTx = 	float((*triggerBits)[3]); //HLT_IsoMu15_LooseIsoPFTau15_v2
      else if(run>=165970 && run<=167043 && run!=166346)
	HLTx = 	float((*triggerBits)[4]); //HLT_IsoMu15_LooseIsoPFTau15_v4
      else if(run==166346)
	HLTx =  float((*triggerBits)[5]); //HLT_IsoMu15_LooseIsoPFTau15_v5      
      else if(run>=167078 && run<=167913)
	HLTx =  float((*triggerBits)[6]); //HLT_IsoMu15_LooseIsoPFTau15_v6      
      else if(run>=170249 && run<=173198)
	HLTx =  float((*triggerBits)[7]); //HLT_IsoMu15_LooseIsoPFTau15_v8 
      else if(run>=173243 && run<=178380)
	HLTx =  float((*triggerBits)[8]); //HLT_IsoMu15_LooseIsoPFTau15_v9

      if(run>=160404 && run<=163869){
	bool isTriggMatched = (*tauXTriggers)[0]  && (*tauXTriggers)[2] ; //hltSingleMuIsoL3IsoFiltered12 && hltOverlapFilterIsoMu12IsoPFTau10
	HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      }
      else if(run>=165088){
	bool isTriggMatched = (*tauXTriggers)[1] && (*tauXTriggers)[3] ; //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau15
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
     
    } else{

      //HLTx =  float((*triggerBits)[1]); //HLT_IsoMu12_LooseIsoPFTau10_v2
      HLTx =  float((*triggerBits)[3]); //HLT_IsoMu12_v1
      bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[2] ; //hltSingleMuIsoL3IsoFiltered12 && hltOverlapFilterIsoMu12IsoPFTau10
      isTriggMatched = (*tauXTriggers)[0]; //hltSingleMuIsoL3IsoFiltered12
      HLTmatch = isTriggMatched ? 1.0 : 0.0;
    }

    // ratioEffTau15 does not reach plateau...force same plateau of IsoPFTau20
    if((*diTauLegsP4)[1].Pt() < 30){
      HLTweightTau = (std::string(sample.Data())).find("Run2011")==string::npos ? 
	ratioEffTau15->dataEfficiency( (*diTauLegsP4)[1].Pt() )/ratioEffTau10->mcEfficiency( (*diTauLegsP4)[1].Pt() ) : 1.0;
    }
    else{
      HLTweightTau = (std::string(sample.Data())).find("Run2011")==string::npos ? 
	ratioEffTau->dataEfficiency( (*diTauLegsP4)[1].Pt() )/ratioEffTau10->mcEfficiency( (*diTauLegsP4)[1].Pt() ) : 1.0;
    }
 
    // apply data-measured efficiency
    HLTweightTau = (std::string(sample.Data())).find("Run2011")==string::npos ? 
      ratioEffTau15->dataEfficiency( (*diTauLegsP4)[1].Pt() ) : 1.0;


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
  delete ratioEffTau; delete ratioEffTau10; delete ratioEffTau15; delete recoilCorr; 
  delete tRandom; 
  
  return;

}



void doAllSamplesElec(){
 
 for( unsigned int k = 0; k < 36 ; k++) {
   makeTrees_ElecTauStream("",        k);
   if( k==0 ) continue;
   makeTrees_ElecTauStream("JetUp",   k);
   makeTrees_ElecTauStream("JetDown", k);
   makeTrees_ElecTauStream("TauUp",   k);
   makeTrees_ElecTauStream("TauDown", k);
   makeTrees_ElecTauStream("ElecUp",   k);
   makeTrees_ElecTauStream("ElecDown", k);
  }

  return;
  
}


void doAllSamplesMu(){
 
  for( unsigned int k = 0; k < 37 ; k++) {
    makeTrees_MuTauStream("",        k);
    if( k==0 ) continue;
    makeTrees_MuTauStream("JetUp",   k);
    makeTrees_MuTauStream("JetDown", k);
    makeTrees_MuTauStream("TauUp",   k);
    makeTrees_MuTauStream("TauDown", k);
    makeTrees_MuTauStream("MuUp",   k);
    makeTrees_MuTauStream("MuDown", k);
  }

  return;

}

