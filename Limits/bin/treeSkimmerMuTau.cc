#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TTree.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMatrixT.h"
#include "TMatrixTBase.h"
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
#include "TROOT.h"
#include "LLRAnalysis/Utilities/interface/RecoilCorrector.hh"
#include "LLRAnalysis/Utilities/interface/Lumi3DReWeightingForLorenzo.h"
#include "TauAnalysis/SVFitStandAlone/interface/NSVfitStandaloneAlgorithm.h"

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
#define USEFAKERATE true
#define DOSVFITSTANDALONE false

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


void smear( float& reco, float gen = float(0.), float dM = float(0.), float dRMS = float(0.) , int debug = -1){
  
  double recoOld           = reco;
  reco  = gen + (reco - gen)*(1+dRMS);
  double recoSmeared       = reco;
  reco *= (1+dM);
  double recoSmearedScaled = reco;

  if(debug==0){
    cout << "Reco elec-pt = " << recoOld << ", smeared = " << recoSmeared << ", scaled = " << recoSmearedScaled << endl;
  }

}


void createReWeighting3D(){

  // truth
  std::vector< float > Fall11Lumi ;
  Double_t Fall11Lumi_f[50] = {
    0.003388501,
    0.010357558,
    0.024724258,
    0.042348605,
    0.058279812,
    0.068851751,
    0.072914824,
    0.071579609,
    0.066811668,
    0.060672356,
    0.054528356,
    0.04919354,
    0.044886042,
    0.041341896,
    0.0384679,
    0.035871463,
    0.03341952,
    0.030915649,
    0.028395374,
    0.025798107,
    0.023237445,
    0.020602754,
    0.0180688,
    0.015559693,
    0.013211063,
    0.010964293,
    0.008920993,
    0.007080504,
    0.005499239,
    0.004187022,
    0.003096474,
    0.002237361,
    0.001566428,
    0.001074149,
    0.000721755,
    0.000470838,
    0.00030268,
    0.000184665,
    0.000112883,
    6.74043E-05,
    3.82178E-05,
    2.22847E-05,
    1.20933E-05,
    6.96173E-06,
    3.4689E-06,
    1.96172E-06,
    8.49283E-07,
    5.02393E-07,
    2.15311E-07,
    9.56938E-08
   };

  for( int i=0; i<50; i++) {
    Fall11Lumi.push_back(Fall11Lumi_f[i]);
  }

  TH1* puDist_mc = new TH1D("MC_distr", "MC_distr", 50 , 0., 50.);
  int numBins = puDist_mc->GetNbinsX();
  for ( int iBin = 1; iBin <= numBins; ++iBin ) {
    double binCenter = puDist_mc->GetBinCenter(iBin);
    int idx = TMath::FloorNint(binCenter);
    double binContent = ( idx >= 0 && idx < (int)Fall11Lumi.size() ) ?
      Fall11Lumi[idx] : 0.;
    puDist_mc->SetBinContent(iBin, binContent);
    puDist_mc->SetBinError(iBin, 0.);
  }
  TFile* puFile_mc = new TFile("../../Utilities/data/pileUp/pileUpFall11.root", "RECREATE");
  puDist_mc->Write();
  delete puFile_mc;

}

float pileupWeight( int intimepileup_ ){

  
  // Fall11: intime
  std::vector< float > Fall11Lumi ;
  Double_t Fall11Lumi_f[50] = {
    0.014583699,
    0.025682975,
    0.038460562,
    0.049414536,
    0.056931087,
    0.061182816,
    0.062534625,
    0.061476918,
    0.058677499,
    0.055449877,
    0.051549051,
    0.047621024,
    0.043923799,
    0.040569076,
    0.037414654,
    0.034227033,
    0.031437714,
    0.028825596,
    0.026218978,
    0.023727061,
    0.021365645,
    0.01918743,
    0.016972815,
    0.014920601,
    0.013038989,
    0.011293777,
    0.009612465,
    0.008193556,
    0.006888047,
    0.005715239,
    0.004711232,
    0.003869926,
    0.003154521,
    0.002547417,
    0.002024714,
    0.001574411,
    0.001245808,
    0.000955206,
    0.000735305,
    0.000557304,
    0.000412503,
    0.000305502,
    0.000231002,
    0.000165701,
    0.000121201,
    9.30006E-05,
    6.40004E-05,
    4.22003E-05,
    2.85002E-05,
    1.96001E-05
  };

  /*
  // Fall11: truth
  std::vector< float > Fall11Lumi ;
  Double_t Fall11Lumi_f[50] = {
   0.003388501,
   0.010357558,
   0.024724258,
   0.042348605,
   0.058279812,
   0.068851751,
   0.072914824,
   0.071579609,
   0.066811668,
   0.060672356,
   0.054528356,
   0.04919354,
   0.044886042,
   0.041341896,
   0.0384679,
   0.035871463,
   0.03341952,
   0.030915649,
   0.028395374,
   0.025798107,
   0.023237445,
   0.020602754,
   0.0180688,
   0.015559693,
   0.013211063,
   0.010964293,
   0.008920993,
   0.007080504,
   0.005499239,
   0.004187022,
   0.003096474,
   0.002237361,
   0.001566428,
   0.001074149,
   0.000721755,
   0.000470838,
   0.00030268,
   0.000184665,
   0.000112883,
   6.74043E-05,
   3.82178E-05,
   2.22847E-05,
   1.20933E-05,
   6.96173E-06,
   3.4689E-06,
   1.96172E-06,
   8.49283E-07,
   5.02393E-07,
   2.15311E-07,
   9.56938E-08
   };
  */
   
  // sigma 68 pb
  std::vector< float > Data2011LumiExt;
  Double_t Data2011LumiExt_f[50] = {
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
    6.04852E-06,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
  

  for( int i=0; i<50; i++) {
    Data2011LumiExt.push_back(Data2011LumiExt_f[i]);
    Fall11Lumi.push_back(Fall11Lumi_f[i]);
  }
  
  int intimepileup = intimepileup_>49 ? 49 : intimepileup_;

  return Data2011LumiExt[intimepileup]/Fall11Lumi[intimepileup];

}


void makeTrees_MuTauStream(string analysis_ = "", string sample_ = "", float xsec_ = 0., string inputDir_ = "./"){
  

  cout << "Now skimming analysis " << analysis_ << endl;

  
  //////////////////////////////////////////////////////////
  //edm::Lumi3DReWeightingForLorenzo* Lumi3DReWeighting = 
  //new edm::Lumi3DReWeightingForLorenzo("../../Utilities/data/pileUp/pileUpFall11.root",
  //				 "../../Utilities/data/pileUp/Run2011PileUpTruth.root",
  //				 "MC_distr",
  //				 "pileup");
  //Lumi3DReWeighting->weight3D_init(1.00);
  //cout << "Lumi3D rewieghting initialized" << endl;
  //////////////////////////////////////////////////////////
  
  RecoilCorrector* recoilCorr = 0;

  //////////////////////////////////////////////////////////

  cout << "Using corrections from llrCorrections.root" << endl;
  TFile corrections("../../Utilities/data/corrections/llrCorrections.root");
  
  TF1 *ratioMuIDIsoBL    = (TF1*)corrections.Get("ratioMuIDIsoBL");
  TF1 *ratioMuIDIsoEC    = (TF1*)corrections.Get("ratioMuIDIsoEC");

  TF1 *ratioMuIdBL       = (TF1*)corrections.Get("ratioMuIdBL");
  TF1 *ratioMuIdEC       = (TF1*)corrections.Get("ratioMuIdEC");

  TF1 *ratioMuIsoBL      = (TF1*)corrections.Get("ratioMuIsoBL");
  TF1 *ratioMuIsoEC      = (TF1*)corrections.Get("ratioMuIsoEC");

  TF1 *ratioMuAllBL       = (TF1*)corrections.Get("ratioMuAllBL");
  TF1 *ratioMuAllEC       = (TF1*)corrections.Get("ratioMuAllEC");
  TF1 *ratioTauMuTauAll   = (TF1*)corrections.Get("ratioTauMuTauAll");
  TF1 *ratioTauMuTauAllBL = (TF1*)corrections.Get("ratioTauMuTauAllBL");
  TF1 *ratioTauMuTauAllEC = (TF1*)corrections.Get("ratioTauMuTauAllEC");

  TF1 *turnOnMuAllBL       = (TF1*)corrections.Get("turnOnMuAllBL");
  TF1 *turnOnMuAllEC       = (TF1*)corrections.Get("turnOnMuAllEC");
  TF1 *turnOnTauMuTauAll   = (TF1*)corrections.Get("turnOnTauMuTauAll");  
  TF1 *turnOnTauMuTauAllBL = (TF1*)corrections.Get("turnOnTauMuTauAllBL");  
  TF1 *turnOnTauMuTauAllEC = (TF1*)corrections.Get("turnOnTauMuTauAllEC");  

  if(!ratioMuIDIsoBL)     cout << "Missing corrections for MuID+Iso (BL)" << endl;
  if(!ratioMuIDIsoEC)     cout << "Missing corrections for MuID+Iso (EC)" << endl;
  if(!ratioMuIdBL)        cout << "Missing corrections for MuID (BL)" << endl;
  if(!ratioMuIdEC)        cout << "Missing corrections for MuID (EC)" << endl;
  if(!ratioMuIsoBL)       cout << "Missing corrections for MuIso (BL)" << endl;
  if(!ratioMuIsoEC)       cout << "Missing corrections for MuIso (EC)" << endl;
  if(!ratioMuAllBL)       cout << "Missing corrections for Mu HLT (BL)" << endl;
  if(!ratioMuAllEC)       cout << "Missing corrections for Mu HLT (EC)" << endl;
  if(!ratioTauMuTauAll)   cout << "Missing corrections for tau HLT" << endl;
  if(!ratioTauMuTauAllBL) cout << "Missing corrections for tau HLT (BL)" << endl;
  if(!ratioTauMuTauAllEC) cout << "Missing corrections for tau HLT (EC)" << endl;
  if(!turnOnMuAllBL)      cout << "Missing turnOn for mu (BL)" << endl;
  if(!turnOnMuAllEC)      cout << "Missing turnOn for mu (EC)" << endl;
  if(!turnOnTauMuTauAll)  cout << "Missing turnOn for tau" << endl;

  //////////////////////////////////////////////////////////

  
  cout << "Using fake-rate method from fakeRate.root" << endl;
  TFile fakeRate_DYJets ("../../Utilities/data/fakeRate/fakeRate_DYJetsToTauTau.root","READ");
  TFile fakeRate_Run2011("../../Utilities/data/fakeRate/fakeRate_Run2011AB.root","READ");
  TFile fakeRate_WJets  ("../../Utilities/data/fakeRate/fakeRate_WJetsToMuNu.root","READ");

 
  TH3F* hfakeRateDYJets  = 0;
  TH3F* hfakeRateRun2011 = 0;
  TH3F* hfakeRateWJets   = 0;


  if(USEFAKERATE){
    if(!fakeRate_DYJets.IsZombie()){
      cout << "Tau Efficiency from DY->tautau avalilable" << endl;
      hfakeRateDYJets  = (TH3F*)fakeRate_DYJets.Get("hRatio");
    }
    //fakeRate_DYJets.Close();

    if(!fakeRate_Run2011.IsZombie()){
      cout << "Fake-rate from Run2011 avalilable" << endl;
      hfakeRateRun2011 = (TH3F*)fakeRate_Run2011.Get("hRatio");
    }
    //fakeRate_Run2011.Close();

    if(!fakeRate_WJets.IsZombie()){
      cout << "Fake Rate from W+jets available" << endl;
      hfakeRateWJets   = (TH3F*)fakeRate_WJets.Get("hRatio");
    }
    //fakeRate_WJets.Close();
  }
  
  //////////////////////////////////////////////////////////


  //////////////////////////////////////////////////////////

  // normalization Lumi
  Float_t Lumi=1000;

  string currentSample = sample_ ;
  TString sample(currentSample.c_str());

  TString outName = "nTuple"+sample+"_Open_MuTauStream.root";
  TFile *outFile = new TFile(outName,"UPDATE");

  TTree* outTreePtOrd     = new TTree(TString(("outTreePtOrd"+analysis_).c_str()),"tree jets pT-ord");

  //////////////////////////////////////////////////////////

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
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, diTauSVFitMass, diTauSVFitMassCal0, diTauSVFitMassCal1, diTauSVFitMassCal2, diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float diTauSVFitMassSA, diTauSVFitMassErrSA;
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauRecoPt,diTauRecoPhi;
  float diTauMinMass;

  float etaTau1Fit, etaTau2Fit, phiTau1Fit, phiTau2Fit, ptTau1Fit, ptTau2Fit;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2, dxy1_, dz1_;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,MEtPhi,pZetaSig_;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;
  float pfJetPt_;

  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;
  float fakeRateRun2011, fakeRateWMC, effDYMC, CDFWeight;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, embeddingWeight_,HqTWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmu,HLTmatch,HLTweightTau, HLTweightMu, SFMu, SFTau, HLTMu, HLTTau;
  int isTauLegMatched_,muFlag_,genDecay_,leptFakeTau;

  // event id
  ULong64_t event_,run_,lumi_;
  int index_;

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
  outTreePtOrd->Branch("diTauSVFitMassCal0",&diTauSVFitMassCal0,"diTauSVFitMassCal0/F");
  outTreePtOrd->Branch("diTauSVFitMassCal1",&diTauSVFitMassCal1,"diTauSVFitMassCal1/F");
  outTreePtOrd->Branch("diTauSVFitMassCal2",&diTauSVFitMassCal2,"diTauSVFitMassCal2/F");
  outTreePtOrd->Branch("diTauSVFitPt",  &diTauSVFitPt,  "diTauSVFitPt/F");
  outTreePtOrd->Branch("diTauSVFitEta", &diTauSVFitEta, "diTauSVFitEta/F");
  outTreePtOrd->Branch("diTauSVFitPhi", &diTauSVFitPhi, "diTauSVFitPhi/F");
  outTreePtOrd->Branch("diTauRecoPt",   &diTauRecoPt,  "diTauRecoPt/F");
  outTreePtOrd->Branch("diTauRecoPhi",  &diTauRecoPhi,  "diTauRecoPhi/F");
  
  outTreePtOrd->Branch("diTauSVFitMassSA",     &diTauSVFitMassSA,     "diTauSVFitMassSA/F");
  outTreePtOrd->Branch("diTauSVFitMassErrSA",  &diTauSVFitMassErrSA,  "diTauSVFitMassErrSA/F");
  outTreePtOrd->Branch("etaTau1Fit",           &etaTau1Fit,  "etaTau1Fit/F");
  outTreePtOrd->Branch("etaTau2Fit",           &etaTau2Fit,  "etaTau2Fit/F");
  outTreePtOrd->Branch("phiTau1Fit",           &phiTau1Fit,  "phiTau1Fit/F");
  outTreePtOrd->Branch("phiTau2Fit",           &phiTau2Fit,  "phiTau2Fit/F");
  outTreePtOrd->Branch("ptTau1Fit",            &ptTau1Fit,   "ptTau1Fit/F");
  outTreePtOrd->Branch("ptTau2Fit",            &ptTau2Fit,   "ptTau2Fit/F");

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

  outTreePtOrd->Branch("dxy1",    &dxy1_,"dxy1/F");
  outTreePtOrd->Branch("dz1",     &dz1_,"dz1/F");

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

  outTreePtOrd->Branch("pfJetPt",                 &pfJetPt_,"pfJetPt/F");
  outTreePtOrd->Branch("fakeRateRun2011",         &fakeRateRun2011,"fakeRateRun2011/F");
  outTreePtOrd->Branch("fakeRateWMC",             &fakeRateWMC,"fakeRateWMC/F");
  outTreePtOrd->Branch("effDYMC",                 &effDYMC,"effDYMC/F");
  outTreePtOrd->Branch("CDFWeight",               &CDFWeight,"CDFWeight/F");
  
  outTreePtOrd->Branch("diTauCharge", &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("MtLeg1",      &MtLeg1_,"MtLeg1/F");
  outTreePtOrd->Branch("MtLeg1Corr",  &MtLeg1Corr_,"MtLeg1Corr/F");
  outTreePtOrd->Branch("pZeta",       &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaCorr",   &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaVis",    &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("pZetaVisCorr",&pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("MEt",         &MEt,"MEt/F");
  outTreePtOrd->Branch("MEtCorr",     &MEtCorr,"MEtCorr/F");
  outTreePtOrd->Branch("MEtPhi",      &MEtPhi,"MEtPhi/F");
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
  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("HqTWeight",          &HqTWeight,"HqTWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx, "HLTx/F");
  outTreePtOrd->Branch("HLTmu",        &HLTmu,"HLTmu/F");
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
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");

  outTreePtOrd->Branch("index", &index_, "index/I");
 
  //string currentInName = "/data_CMS/cms/lbianchini/"+inputDir_+"//treeMuTauStream_"+sample_+".root" ;
  string currentInName = inputDir_+"//treeMuTauStream_"+sample_+".root" ;

  TString inName(currentInName.c_str());
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }

  TString treeName(("muTauStreamAnalyzer"+analysis_+"/tree").c_str());

  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries    = currentTree->GetEntries() ;
  int nEventsRead = ((TH1F*)file->Get("allEventsFilter/totalEvents"))->GetBinContent(1) ;
  float crossSection = xsec_ ;
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
  currentTree->SetBranchStatus("dxy1"                  ,1);
  currentTree->SetBranchStatus("dxy2"                  ,0);
  currentTree->SetBranchStatus("dz1"                   ,1);
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
  currentTree->SetBranchStatus("pfJetPt"               ,1);

  // MET
  currentTree->SetBranchStatus("METP4"                 ,1);
  currentTree->SetBranchStatus("genMETP4"              ,1);
  currentTree->SetBranchStatus("sumEt"                 ,0);
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,1);
  currentTree->SetBranchStatus("metSgnMatrix"          ,1);

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
  currentTree->SetBranchStatus("nPUVerticesP1"         ,1);
  currentTree->SetBranchStatus("nPUVerticesM1"         ,1);
  currentTree->SetBranchStatus("numOfDiTaus"           ,0);
  currentTree->SetBranchStatus("numOfLooseIsoDiTaus"   ,1);
 
  currentTree->SetBranchStatus("run"                   ,1);
  currentTree->SetBranchStatus("event"                 ,1);
  currentTree->SetBranchStatus("lumi"                  ,1);
  currentTree->SetBranchStatus("mcPUweight"            ,1);
  currentTree->SetBranchStatus("embeddingWeight"       ,1);

  currentTree->SetBranchStatus("index"                 ,1);

  // triggers
  currentTree->SetBranchStatus("tauXTriggers"          ,1);
  currentTree->SetBranchStatus("triggerBits"           ,1);

  ////////////////////////////////////////////////////////////////////

  std::vector< LV >* jets           = new std::vector< LV >();

  if(analysis_.find("JetUp")!=string::npos) 
    currentTree->SetBranchAddress("jetsIDUpP4",        &jets);
  else if(analysis_.find("JetDown")!=string::npos)
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

  std::vector< float >* metSgnMatrix = new std::vector< float >();
  currentTree->SetBranchAddress("metSgnMatrix", &metSgnMatrix);

  

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;;
  float diTauCharge;
  int tightestHPSDBWP, decayMode;
  float numPV;
  int numOfLooseIsoDiTaus;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float pfJetPt;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float mcPUweight,embeddingWeight;
  int isTauLegMatched,muFlag,genDecay, nPUVertices, nPUVerticesM1, nPUVerticesP1;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float dxy1, dz1;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  ULong64_t event,run,lumi;
  int index;

  currentTree->SetBranchAddress("chIsoLeg2",            &chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",            &phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",          &chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",          &nhIsoLeg1);
  currentTree->SetBranchAddress("phIsoLeg1v2",          &phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",        &chIsoPULeg1);
  currentTree->SetBranchAddress("nhIsoPULeg1v2",        &nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",        &phIsoPULeg1);
  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);

  currentTree->SetBranchAddress("dxy1",                 &dxy1);
  currentTree->SetBranchAddress("dz1",                  &dz1);

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
  currentTree->SetBranchAddress("nPUVerticesP1",        &nPUVerticesP1);
  currentTree->SetBranchAddress("nPUVerticesM1",        &nPUVerticesM1);
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
  currentTree->SetBranchAddress("pfJetPt"              ,&pfJetPt);
  currentTree->SetBranchAddress("index",                &index);

  if(sample_.find("WJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_wjets_njet.root");
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(sample_.find("DYJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_zjets_ltau_njet.root");
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(sample_.find("H1")!=string::npos){
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_higgs_njet.root");
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv4/RecoilCorrector_v4/recoilfits/recoilfit_datamm_njet.root");
  }
    

  TFile* HqT      = 0;
  int mH          = 120;
  TH1F* histo     = 0;
  if(sample_.find("GGFH")!=string::npos){
    if(sample_.find("GGFH100")!=string::npos) mH = 100;
    if(sample_.find("GGFH105")!=string::npos) mH = 105;
    if(sample_.find("GGFH110")!=string::npos) mH = 110;
    if(sample_.find("GGFH115")!=string::npos) mH = 115;
    if(sample_.find("GGFH120")!=string::npos) mH = 120;
    if(sample_.find("GGFH125")!=string::npos) mH = 125;
    if(sample_.find("GGFH130")!=string::npos) mH = 130;
    if(sample_.find("GGFH135")!=string::npos) mH = 135;
    if(sample_.find("GGFH140")!=string::npos) mH = 140;
    if(sample_.find("GGFH145")!=string::npos) mH = 145;
    if(sample_.find("GGFH160")!=string::npos) mH = 160;
    cout << "Reweighting powheg with HqT mH=" << mH << endl;
    HqT = new TFile(Form("../../Utilities/data/HqTFeHiPro/weight_ptH_%d.root", mH));
    if(!HqT) cout << "Cannot find HqT file..." << endl;
    else{
      histo = (TH1F*)(HqT->Get(Form("powheg_weight/weight_hqt_fehipro_fit_%d",mH)));
    }
  }

  // protection against multiple pairs per event
  ULong64_t lastEvent = 0;
  ULong64_t lastRun   = 0;
  ULong64_t lastLumi  = 0;
  int eventFlag       = 0;

  for (int n = 0; n < nEntries ; n++) {
    
    currentTree->GetEntry(n);
    if(n%5000==0) cout << n << endl;
    
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
    for(int l = 0 ; l < int(jets->size()) ; l++){
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta)
	indexes.push_back(l);
      if((*jetsBtagHE)[l] > 3.3) nJets20BTagged++;
    }
    nJets20 = indexes.size();
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];  

    for(int v = 0 ; v < int(indexes.size()) ; v++){
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
    for(int l = 0 ; l < int(indexes.size()) ; l++){
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

    float c0L = 0.0050;
    float c1L = 1.14;
    float c2L = 0.40;
    float c3L = 25;
    diTauSVFitMassCal0 = (*diTauSVfitP4)[0].M() - 
      (((c0L*120 + c1L)*diTauNSVfitMassErrUp + (c2L*120+c3L)) - 120);

    float c0 = 9.08725e-01;
    float c1 = 4.01539e+01;
    float c2 = 7.48472e-02;
    float c3 = 3.79035e+01;
    float c4 = 1.93179e-04;
    float c5 = 1.35327e-02;
    float c6 = -1.36772e-02;
    float c7 = 6.04364e-05;
    float massH     = 120;
    float bias120   = (c0*massH+c1) + (c2*massH+c3)*TMath::Erf((c4*massH+c5)*diTauNSVfitMassErrUp + (c6*massH+c7)) - massH;
    diTauSVFitMassCal1 = (*diTauSVfitP4)[0].M() - bias120; 

    float massNew = (*diTauSVfitP4)[0].M();
    float bias    = bias120;
    float diff    = 999.;
    int nMax      = 20;
    bool exit     = false;
    for(int it=0; it< nMax && !exit; it++){      
      float bias_it = (c0*massNew+c1) + (c2*massNew+c3)*TMath::Erf((c4*massNew+c5)*diTauNSVfitMassErrUp + (c6*massNew+c7)) - massNew;
      massNew = (*diTauSVfitP4)[0].M() - bias_it;
      diff = fabs(bias - bias_it);
      bias = bias_it;
      if(it==(nMax-1)) cout << "Reached Max iterartion w/o convergence" << endl;
      if(it>0 && diff<1){
	if((*diTauSVfitP4)[0].M()<60 && diTauNSVfitMassErrUp>0) 
	  bias=0;
	exit = true;
      } 
    }
    diTauSVFitMassCal2 = (*diTauSVfitP4)[0].M() - bias;


    diTauRecoPt  = ((*diTauVisP4)[0]+(*METP4)[0]).Pt();
    diTauRecoPhi = ((*diTauVisP4)[0]+(*METP4)[0]).Phi();

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

    dxy1_    = dxy1;
    dz1_     = dz1;

    visibleTauMass_ = visibleTauMass;
    diTauCharge_    = diTauCharge;
    
    ////////////////////////////////////////////////////////////////////

    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    double u1 = 0.; double u2 = 0.;
    double err1 = 0; double err2 = 0;

    if(genVP4->size() && recoilCorr!=0){
      if(sample_.find("WJets")  !=string::npos)      
	recoilCorr->CorrectType2(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi(), u1, u2 , err1,err2, TMath::Min(nJets30,2) );
      else if(sample_.find("DYJets")!=string::npos || sample_.find("H1")!=string::npos)  
	recoilCorr->CorrectType2(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi(), u1, u2, err1,err2, TMath::Min(nJets30,2)  );
    }
    corrMET_tmp.SetPtEtaPhiM(corrPt,0,corrPhi,0);
    corrMET.SetPx(corrMET_tmp.Px());
    corrMET.SetPy(corrMET_tmp.Py());
    corrMET.SetPz(corrMET_tmp.Pz());
    corrMET.SetE(corrMET_tmp.E());



    ////////////////////////////////////////////////////////////////////

    LV recoilCorrecMET = (*METP4)[1]; //corrMET;

    float scalarSumPt     = ( *diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPt     = ((*diTauLegsP4)[0] +      (*METP4)[0]).Pt() ;
    float scalarSumPtCorr = ( *diTauLegsP4)[0].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtCorr = ((*diTauLegsP4)[0] +      recoilCorrecMET).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPt*scalarSumPt - vectorSumPt*vectorSumPt ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtCorr*scalarSumPtCorr - vectorSumPtCorr*vectorSumPtCorr ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[0];

    pZetaSig_     = pZetaSig;

    MEt     = (*METP4)[0].Et();
    MEtCorr = recoilCorrecMET.Et();
    MEtPhi  = (*METP4)[0].Phi();

    ////////////////////////////////////////////////
    TMatrixD* metsig = new TMatrixD(2,2);
    (*metsig)[0][0] = (*metSgnMatrix)[0]; 
    (*metsig)[0][1] = (*metSgnMatrix)[1]; 
    (*metsig)[1][0] = (*metSgnMatrix)[1]; 
    (*metsig)[1][1] = (*metSgnMatrix)[2]; 
    NSVfitStandalone::Vector measuredMET( (*METP4)[1].Px(), (*METP4)[1].Py(), 0);
    std::vector<NSVfitStandalone::MeasuredTauLepton> measuredTauLeptons;
    NSVfitStandalone::LorentzVector p1( (*diTauLegsP4)[1] );
    measuredTauLeptons.push_back(NSVfitStandalone::MeasuredTauLepton(NSVfitStandalone::kHadDecay,p1));    
    NSVfitStandalone::LorentzVector p2( (*diTauLegsP4)[0] );
    measuredTauLeptons.push_back(NSVfitStandalone::MeasuredTauLepton(NSVfitStandalone::kLepDecay,p2));    
    NSVfitStandaloneAlgorithm algo(measuredTauLeptons,measuredMET,metsig,0);
    algo.maxObjFunctionCalls(5000);
    if(DOSVFITSTANDALONE) algo.fit();
    if(algo.isValidSolution()){
      diTauSVFitMassSA    =  algo.fittedDiTauSystem().mass();
      diTauSVFitMassErrSA =  algo.massUncert();
      etaTau1Fit          = ((algo.fittedTauLeptons())[1]).Eta();
      etaTau2Fit          = ((algo.fittedTauLeptons())[0]).Eta();
      phiTau1Fit          = ((algo.fittedTauLeptons())[1]).Phi();
      phiTau2Fit          = ((algo.fittedTauLeptons())[0]).Phi();
      ptTau1Fit           = ((algo.fittedTauLeptons())[1]).Pt();
      ptTau2Fit           = ((algo.fittedTauLeptons())[0]).Pt();
    }
    else{
      diTauSVFitMassSA    = -99; 
      diTauSVFitMassErrSA = -99;
      etaTau1Fit          = -99;
      etaTau2Fit          = -99;
      phiTau1Fit          = -99;
      phiTau2Fit          = -99;
      ptTau1Fit           = -99;
      ptTau2Fit           = -99;
    }
    ////////////////////////////////////////////////

    combRelIsoLeg1         = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    float PUtoPVratio      = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta     = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta    = (chIsoLeg1+ std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    float EffArea          = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho      = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2            =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    

    tightestHPSDBWP_ = tightestHPSDBWP;
    decayMode_       = decayMode;
    numPV_           = numPV;
    sampleWeight     = scaleFactor; 
    //puWeight         = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : mcPUweight ;
    puWeight         = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : pileupWeight(nPUVertices);   
    //puWeight         = (std::string(sample.Data())).find("Run2011")!=string::npos ? 1.0 : Lumi3DReWeighting->weight3D(nPUVerticesM1, nPUVertices, nPUVerticesP1) ;
    embeddingWeight_ = embeddingWeight;

    HqTWeight = histo!=0 ? histo->GetBinContent( histo->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

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
    pfJetPt_                  = pfJetPt;

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
	//HLTx =  float((*triggerBits)[9] || (*triggerBits)[8]);  //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v1
	HLTx =  float((*triggerBits)[9]);  //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v1
      else if(run>=178420 && run<=179889)
	HLTx =  float((*triggerBits)[10]); //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v5
      else if(run>=179959 && run<=180252)
	HLTx =  float((*triggerBits)[11]); //HLT_IsoMu15_eta2p1_LooseIsoPFTau20_v6
     

      if(run>=160404 && run<=163869){
	bool isTriggMatched = (*tauXTriggers)[0]  && (*tauXTriggers)[3]  ; //hltSingleMuIsoL3IsoFiltered12 && hltOverlapFilterIsoMu12IsoPFTau10
	HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      }
      else if(run>=165088 && run<=173692){
	bool isTriggMatched = (*tauXTriggers)[1] && (*tauXTriggers)[4] ; //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau15
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }
      else if(run>=175860 && run<=180252){
	//bool isTriggMatched = ((*tauXTriggers)[2] && (*tauXTriggers)[5]) ||
	//((*tauXTriggers)[1] && (*tauXTriggers)[4]); //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau20
	bool isTriggMatched =  ((*tauXTriggers)[2] && (*tauXTriggers)[5]);
	HLTmatch = isTriggMatched ? 1.0 : 0.0;
      }

      HLTweightTau = 1.0;
      HLTweightMu  = 1.0;
      HLTmu        = 1.0;
   
      if( (std::string(sample.Data())).find("Embed")!=string::npos ){

	HLTMu  = TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ? 
	  turnOnMuAllBL->Eval( (*diTauLegsP4)[0].Pt() ) : turnOnMuAllEC->Eval( (*diTauLegsP4)[0].Pt() );

	//HLTTau = turnOnTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() );
	HLTTau = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?  
	  turnOnTauMuTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) : turnOnTauMuTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;

	SFTau = 1.0;
	SFMu  =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ? 
	  //ratioMuIdBL->Eval( (*diTauLegsP4)[0].Pt() ) * ratioMuIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
	  //ratioMuIdEC->Eval( (*diTauLegsP4)[0].Pt() ) * ratioMuIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ; 
	  ratioMuIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ): 
	  ratioMuIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() );

      }
      else{
	HLTMu  = 1.0;
	HLTTau = 1.0;
	SFTau  = 1.0;
	SFMu   = 1.0;
      }
     
    } else{


      HLTx  =  float((*triggerBits)[0]); //HLT_IsoMu15_LooseIsoPFTau15_v9
      HLTmu =  triggerBits->size()>2 ? 
	float((*triggerBits)[2]) : float(1.0); //HLT_IsoMu15_v14
      bool isTriggMatched = (*tauXTriggers)[0] && (*tauXTriggers)[2] ; //hltSingleMuIsoL3IsoFiltered15 && hltOverlapFilterIsoMu15IsoPFTau15
      HLTmatch = isTriggMatched ? 1.0 : 0.0;

      //HLTweightTau = ratioTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTweightTau  = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?  
	ratioTauMuTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) : ratioTauMuTauAllEC->Eval( (*diTauLegsP4)[1].Pt() );
      HLTweightMu  = TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ?  
	ratioMuAllBL->Eval( (*diTauLegsP4)[0].Pt() ) : ratioMuAllEC->Eval( (*diTauLegsP4)[0].Pt() );
      HLTMu  = TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ? 
	turnOnMuAllBL->Eval( (*diTauLegsP4)[0].Pt() ) : turnOnMuAllEC->Eval( (*diTauLegsP4)[0].Pt() );
      HLTTau = turnOnTauMuTauAll->Eval( (*diTauLegsP4)[1].Pt() ); 
      SFTau  = 1.0;
      SFMu   =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ?
	//ratioMuIdBL->Eval( (*diTauLegsP4)[0].Pt() ) * ratioMuIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
	//ratioMuIdEC->Eval( (*diTauLegsP4)[0].Pt() ) * ratioMuIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;  
	ratioMuIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ): 
	ratioMuIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() );


    }
   
    isTauLegMatched_ = isTauLegMatched;
    if((std::string(sample.Data())).find("Run2011")==string::npos)
      leptFakeTau      = (isTauLegMatched==0 && (*genDiTauLegsP4)[1].E()>0) ? 1 : 0;
    else 
      leptFakeTau = -99;
    
    
    if(hfakeRateDYJets==0 || hfakeRateRun2011==0){
      CDFWeight       = -99;
    }
    else{
      float f = hfakeRateRun2011->GetBinContent(hfakeRateRun2011->FindBin( pfJetPt, ptL2, 0.));
      float e = hfakeRateDYJets->GetBinContent( hfakeRateDYJets->FindBin(  pfJetPt, ptL2, 0.));

      if(fabs(e-f)>0)
	CDFWeight = ((tightestHPSDBWP>0)*(f*(e-1)/(e-f)) + (tightestHPSDBWP<=0)*(f*e/(e-f)));
      else
	CDFWeight = f;
    }
    fakeRateRun2011 = (hfakeRateRun2011!=0) ? hfakeRateRun2011->GetBinContent( hfakeRateRun2011->FindBin(pfJetPt, ptL2, 0) ) : -99;
    fakeRateWMC     = (hfakeRateWJets!=0)   ? hfakeRateWJets->GetBinContent(   hfakeRateWJets->FindBin(  pfJetPt, ptL2, 0) ) : -99;
    effDYMC         = (hfakeRateDYJets!=0)  ? hfakeRateDYJets->GetBinContent(  hfakeRateDYJets->FindBin( pfJetPt, ptL2, 0) ) : -99;


    muFlag_          = muFlag;
    genDecay_        = genDecay ;
    event_           = event;
    run_             = run;
    lumi_            = lumi;

    index_           = index;
   
    /*
    if( !(run==lastRun && lumi==lastLumi && event==lastEvent)){
      lastEvent = event;
      lastRun   = run;
      lastLumi  = lumi;
      eventFlag = tightestHPSDBWP>0 ? 1 : 0;
    }
    else if(run==lastRun && lumi==lastLumi && event==lastEvent && eventFlag==1 && tightestHPSDBWP>0.5 ){
      cout << "Skipping pair " << run << ":" << lumi << ":" << event << " index " << index << endl;
      // skip subleading pairs of isolated taus in events with more than 1 isolated taus
      continue; 
    }
    else if(run==lastRun && lumi==lastLumi && event==lastEvent){
      eventFlag = int(bool(eventFlag) || tightestHPSDBWP>0.5);
    }
    */
    
    outTreePtOrd->Fill();
  }

  return;

  file->Close();
  
  if(SAVE) outFile->Write();
  outFile->Close();

  delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfitP4; delete diTauCAP4; delete genDiTauLegsP4;
  delete tauXTriggers; delete triggerBits;
  delete METP4; delete jetsBtagHE; delete jetsBtagHP; delete jetsChNfraction; delete genVP4; delete genMETP4;
  delete gammadEta; delete gammadPhi; delete gammaPt; delete HqT;

  return;

}


void doAllSamplesMu(string inputDir_ =// "MuTauStreamFall11_06Dec2011"
		    "../../TauTauStudies/test/prod/MuTauStream/"
		    ){


  std::vector<std::string> samples;
  std::vector<float> crossSec;

  ////////////// samples & x-sections & skim1 & skim2 /////////////

  samples.push_back("Run2011-MuTau-All_run");             crossSec.push_back( 0  );                          
//samples.push_back("Run2011-MuTau-LooseIso-All_run");    crossSec.push_back( 0  );                          
  samples.push_back("Run2011-MuTau-NoTauIso-All_run");    crossSec.push_back( 0  );                          
  samples.push_back("Run2011-MuTau-Embedded-All_run");    crossSec.push_back( 0  );                          
  samples.push_back("DYJets-MuTau-50-madgraph-PUS6_run"); crossSec.push_back( 3048           * 0.009631    * 0.641987); 
  samples.push_back("TTJets-MuTau-madgraph-PUS6_run");    crossSec.push_back( 157.5          * 0.020998    * 0.823613);  
  samples.push_back("WJets-MuTau-madgraph-PUS6_run");     crossSec.push_back( 31314.0        * 0.001261    * 0.572776);   
  samples.push_back("W3Jets-MuTau-madgraph-PUS6_run");    crossSec.push_back( 304.0          * 1.0         * 0.119569);   
  samples.push_back("WZ-MuTau-pythia-PUS6_run");          crossSec.push_back( 18.2           * 0.0068962   * 0.740770);         
  samples.push_back("ZZ-MuTau-pythia-PUS6_run");          crossSec.push_back(  5.9           * 0.0060357   * 0.753123);    
  samples.push_back("WW-MuTau-pythia-PUS6_run");          crossSec.push_back( 43.0           * 1.0         * 0.065573);        
  samples.push_back("VBFH100-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.36e-02*1.546 * 0.04953     * 0.809326); 
  samples.push_back("GGFH100-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.36e-02*24.02 * 0.03816     * 0.818372); 
  samples.push_back("VBFH105-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.25e-02*1.472 * 0.05278     * 0.804249); 
  samples.push_back("GGFH105-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.25e-02*21.78 * 0.04137     * 0.814457); 
  samples.push_back("VBFH110-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.02e-02*1.398 * 1.0         * 0.110006);
  samples.push_back("GGFH110-MuTau-powheg-PUS6_run");     crossSec.push_back( 8.02e-02*19.84 * 1.0         * 0.066984);    
  samples.push_back("VBFH115-MuTau-powheg-PUS6_run");     crossSec.push_back( 7.65e-02*1.332 * 0.05825     * 0.808179);
  samples.push_back("GGFH115-MuTau-powheg-PUS6_run");     crossSec.push_back( 7.65e-02*18.13 * 1.0         * 0.072958);  
  samples.push_back("VBFH120-MuTau-powheg-PUS6_run");     crossSec.push_back( 7.10e-02*1.269 * 1.0         * 0.118150);   
  samples.push_back("GGFH120-MuTau-powheg-PUS6_run");     crossSec.push_back( 7.10e-02*16.63 * 1.0         * 0.076531);  
  samples.push_back("VBFH125-MuTau-powheg-PUS6_run");     crossSec.push_back( 6.37e-02*1.211 * 0.06337     * 0.813953);   
  samples.push_back("GGFH125-MuTau-powheg-PUS6_run");     crossSec.push_back( 6.37e-02*15.31 * 1.0         * 0.079825); 
  samples.push_back("VBFH130-MuTau-powheg-PUS6_run");     crossSec.push_back( 5.48e-02*1.154 * 0.06515     * 0.811363); 
  samples.push_back("GGFH130-MuTau-powheg-PUS6_run");     crossSec.push_back( 5.48e-02*14.12 * 0.05479     * 0.828420);
  samples.push_back("VBFH135-MuTau-powheg-PUS6_run");     crossSec.push_back( 4.52e-02*1.100 * 0.06740     * 0.816084); 
  samples.push_back("GGFH135-MuTau-powheg-PUS6_run");     crossSec.push_back( 4.52e-02*13.08 * 1.0         * 0.088014);    
  samples.push_back("VBFH140-MuTau-powheg-PUS6_run");     crossSec.push_back( 3.54e-02*1.052 * 1.0         * 0.128525);  
  samples.push_back("GGFH140-MuTau-powheg-PUS6_run");     crossSec.push_back( 3.54e-02*12.13 * 0.05808     * 0.822400);  
  samples.push_back("VBFH145-MuTau-powheg-PUS6_run");     crossSec.push_back( 2.61e-02*1.004 * 0.07349     * 0.817493);  
  samples.push_back("GGFH145-MuTau-powheg-PUS6_run");     crossSec.push_back( 2.61e-02*11.27 * 1.0         * 0.095074); 
  samples.push_back("VBFH160-MuTau-powheg-PUS6_run");     crossSec.push_back( 5.32e-04*0.8787* 1.0         * 0.137951);  
  samples.push_back("GGFH160-MuTau-powheg-PUS6_run");     crossSec.push_back( 5.32e-04*9.080 * 1.0         * 0.104164);  
  samples.push_back("VH100-MuTau-pythia-PUS6_run");       crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 0.082128 * 0.834434);  
  samples.push_back("VH110-MuTau-pythia-PUS6_run");       crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 0.088174 * 0.839732);
  samples.push_back("VH115-MuTau-pythia-PUS6_run");       crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 0.092032 * 0.841206);
  samples.push_back("VH120-MuTau-pythia-PUS6_run");       crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0      * 0.1654);   
  samples.push_back("VH125-MuTau-pythia-PUS6_run");       crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 0.09885  * 0.842507);
  samples.push_back("VH130-MuTau-pythia-PUS6_run");       crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 1.0      * 0.177053);
  samples.push_back("VH135-MuTau-pythia-PUS6_run");       crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 0.104662 * 0.841589);
  samples.push_back("VH140-MuTau-pythia-PUS6_run");       crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 0.108278 * 0.842954);  
  samples.push_back("VH145-MuTau-pythia-PUS6_run");       crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 1.0      * 0.191843);  
  samples.push_back("VH160-MuTau-pythia-PUS6_run");       crossSec.push_back( 5.32e-04*(0.2291+0.1334+0.03942 ) * 1.0      * 0.203857);  
 
  makeTrees_MuTauStream("",        samples[1], crossSec[1], inputDir_);
  return;

  for( unsigned int k = 0; k < samples.size() ; k++) {
    
    makeTrees_MuTauStream("",        samples[k], crossSec[k], inputDir_);

    if( samples[k].find("Run2011-MuTau-All")!=string::npos )
      continue;
    makeTrees_MuTauStream("TauUp",   samples[k], crossSec[k], inputDir_);
    makeTrees_MuTauStream("TauDown", samples[k], crossSec[k], inputDir_);
    if( samples[k].find("Embedded")!=string::npos)
      continue;
    makeTrees_MuTauStream("JetUp",   samples[k], crossSec[k], inputDir_);
    makeTrees_MuTauStream("JetDown", samples[k], crossSec[k], inputDir_);
  }
  
  return;

}


int main(int argc, const char* argv[])
{
  
  std::cout << "doAllSamplesMu()" << std::endl;
  gROOT->SetBatch(true);
 
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  doAllSamplesMu();

}
