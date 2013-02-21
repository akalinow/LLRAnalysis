#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TTree.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
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
//#include "LLRAnalysis/Utilities/interface/Lumi3DReWeightingForLorenzo.h"
//#include "PhysicsTools/Utilities/interface/LumiReweightingStandAlone.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "TauAnalysis/CandidateTools/interface/NSVfitStandaloneAlgorithm.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include <vector>
#include <utility>
#include <map>
#include <algorithm>

#include "TMVA/Factory.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"

#define SAVE   true

#define MINPt1 20.0 
#define MINPt2 20.0
//#define PtVETO 20.0
#define PtVETO 30.0
#define MAXEta  5.0 
#define MINJetID 0.5
#define ETOTAUEB 0.85
#define ETOTAUEE 0.65

#define USERECOILALGO true
#define USEFAKERATE false
#define DOSVFITSTANDALONE false
#define DOVBFMVA true


using namespace ROOT::Math;
using namespace std;

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;
//reweight::LumiReWeighting LumiWeights_;
edm::LumiReWeighting LumiWeights_("MC_Summer12_PU_S10-600bins.root","Data_Pileup_2012_HCP-600bins.root","pileup","pileup");
bool FIRSTEVENT = true;

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
  TFile* puFile_mc = new TFile("pileUp/pileUpFall11.root", "RECREATE");
  puDist_mc->Write();
  delete puFile_mc;

}

float pileupWeight( float intimepileup_ ){

  /*
  // Summer12: truth (from twiki) 
  std::vector< float > Summer12Lumi; 
  Double_t Summer12Lumi_f[60] = {
    2.344E-05, 2.344E-05, 2.344E-05, 2.344E-05, 4.687E-04, 4.687E-04, 7.032E-04, 9.414E-04, 1.234E-03,
    1.603E-03, 2.464E-03, 3.250E-03, 5.021E-03, 6.644E-03, 8.502E-03, 1.121E-02, 1.518E-02, 2.033E-02,
    2.608E-02, 3.171E-02, 3.667E-02, 4.060E-02, 4.338E-02, 4.520E-02, 4.641E-02, 4.735E-02, 4.816E-02,
    4.881E-02, 4.917E-02, 4.909E-02, 4.842E-02, 4.707E-02, 4.501E-02, 4.228E-02, 3.896E-02, 3.521E-02,
    3.118E-02, 2.702E-02, 2.287E-02, 1.885E-02, 1.508E-02, 1.166E-02, 8.673E-03, 6.190E-03, 4.222E-03,
    2.746E-03, 1.698E-03, 9.971E-04, 5.549E-04, 2.924E-04, 1.457E-04, 6.864E-05, 3.054E-05, 1.282E-05,
    5.081E-06, 1.898E-06, 6.688E-07, 2.221E-07, 6.947E-08, 2.047E-08
  };  
  
  
  // sigma 69.4 pb 
  std::vector< float > Data2012LumiExt; 
  Double_t Data2012LumiExt_f[60] = { 
    1.76427e-06, 2.84466e-06, 1.88915e-05, 0.000796165, 0.00110891, 0.000439459, 0.00216878, 0.00562928, 0.0114731, 0.0199515, 0.0317062, 0.0457626, 0.0587994, 0.0700723, 0.0799624, 0.0850344, 0.0802347, 0.0690109, 0.0585324, 0.051109, 0.0457747, 0.0413901, 0.0373793, 0.0335591, 0.0298473, 0.0262066, 0.0226609, 0.0192698, 0.0160998, 0.0132078, 0.0106338, 0.00839923,  0.00650684, 0.00494315, 0.00368197, 0.00268875, 0.00192473, 0.00135048, 0.000928655, 0.000625768, 0.000413144, 0.00026721, 0.000169276, 0.000105016, 6.37928e-05, 3.79375e-05, 2.20845e-05, 1.25825e-05, 7.0155e-06, 3.8275e-06, 2.04314e-06, 1.06702e-06, 5.45147e-07, 2.72455e-07, 1.33199e-07, 6.36963e-08, 2.97937e-08, 1.36308e-08, 6.09958e-09, 2.66964e-09 
  }; 
  */
  /*
  //From Mike
  std::vector< float > Summer12Lumi; 
  Double_t Summer12Lumi_f[60] = { 
    2.34402e-05, 2.34402e-05, 2.34402e-05, 2.34402e-05, 0.000468703, 0.000468703, 0.000703205, 
    0.000941407, 0.00123401, 0.00160301, 0.00246402, 0.00325002, 0.00502104, 0.00664405,
    0.00850206, 0.0112101, 0.0151801, 0.0203301, 0.0260802, 0.0317102, 0.0366703, 0.0406003,
    0.0433803, 0.0452003, 0.0464103, 0.0473503, 0.0481603, 0.0488104, 0.0491704, 0.0490904, 
    0.0484203, 0.0470703, 0.0450103, 0.0422803, 0.0389603, 0.0352103, 0.0311802, 0.0270202,
    0.0228702, 0.0188501, 0.0150801, 0.0116601, 0.00867306, 0.00619004, 0.00422203, 0.00274602,
    0.00169801, 0.000997107, 0.000554904, 0.000292402, 0.000145701, 6.86405e-05, 3.05402e-05,
    1.28201e-05, 5.08104e-06, 1.89801e-06, 6.68805e-07, 2.22102e-07, 6.94705e-08, 2.04701e-08
  };
 
 
  // sigma 69.4 pb 
  std::vector< float > Data2012LumiExt; 
  Double_t Data2012LumiExt_f[60] = { 
    1.76747e-06, 2.84912e-06, 6.88486e-06, 3.71826e-05, 9.81822e-05, 0.000424844, 0.00217272, 
    0.00563951, 0.0114938, 0.0199867, 0.0317601, 0.0458397, 0.058902, 0.0701984, 0.0801076,
    0.0851889, 0.0803805, 0.0691363, 0.0586388, 0.0512019, 0.0458578, 0.0414654, 0.0374472,
    0.0336201, 0.0299015, 0.0262542, 0.0227021, 0.0193048, 0.0161291, 0.0132318, 0.0106532,
    0.00841449, 0.00651866, 0.00495213, 0.00368866, 0.00269364, 0.00192822, 0.00135293,
    0.000930343, 0.000626905, 0.000413895, 0.000267695, 0.000169583, 0.000105207, 6.39087e-05,
    3.80064e-05, 2.21246e-05, 1.26054e-05, 7.02825e-06, 3.83445e-06, 2.04685e-06, 1.06896e-06,
    5.46137e-07, 2.7295e-07, 1.33441e-07, 6.3812e-08, 2.98478e-08, 1.36556e-08, 6.11067e-09,
    2.67449e-09
  }; 

  for( int i=0; i<60; i++) { 
    Data2012LumiExt.push_back(Data2012LumiExt_f[i]); 
    Summer12Lumi.push_back(Summer12Lumi_f[i]); 
  } 

  //reweight::LumiReWeighting LumiWeights_(Summer12Lumi, Data2012LumiExt);
  if(FIRSTEVENT){
    LumiWeights_ = reweight::LumiReWeighting(Summer12Lumi, Data2012LumiExt); 
    //LumiWeights_ =  reweight::LumiReWeighting("MC_Summer12_PU_S7.root","Data_Pileup_2012.root","pileup","pileup");  //crashing due to some ROOT error
    FIRSTEVENT = false;
  } 
  */

  float intimepileup = intimepileup_; //>59 ? 59 : intimepileup_; 
  
  //return LumiWeights_.ITweight(intimepileup); 
  return LumiWeights_.weight(intimepileup);
  
}


float pileupWeight2( int intimepileup_ ){

  float weights[53];
  weights[0]=1.0;
  weights[1]=0.241725;
  weights[2]=0.497953;
  weights[3]=0.750906;
  weights[4]=0.958778;
  weights[5]=1.15292;
  weights[6]=1.26729;
  weights[7]=1.33763;
  weights[8]=1.39117;
  weights[9]=1.38692;
  weights[10]=1.41177;
  weights[11]=1.37077;
  weights[12]=1.34191;
  weights[13]=1.27619;
  weights[14]=1.20034;
  weights[15]=1.1264;
  weights[16]=1.00513;
  weights[17]=0.898563;
  weights[18]=0.783283;
  weights[19]=0.660026;
  weights[20]=0.545681;
  weights[21]=0.444979;
  weights[22]=0.355539;
  weights[23]=0.278989;
  weights[24]=0.214793;
  weights[25]=0.161305;
  weights[26]=0.12141;
  weights[27]=0.089384;
  weights[28]=0.0655027;
  weights[29]=0.0470954;
  weights[30]=0.033824;
  weights[31]=0.0241277;
  weights[32]=0.0168523;
  weights[33]=0.0118342;
  weights[34]=0.00831188;
  weights[35]=0.00574736;
  weights[36]=0.00395389;
  weights[37]=0.00270099;
  weights[38]=0.00184071;
  weights[39]=0.00126892;
  weights[40]=0.000799038;
  weights[41]=0.000568358;
  weights[42]=0.000366065;
  weights[43]=0.000241041;
  weights[44]=0.000152796;
  weights[45]=5.53181e-05;
  weights[46]=5.53181e-05;
  weights[47]=5.53181e-05;
  weights[48]=5.53181e-05;
  weights[49]=5.53181e-05;
  weights[50]=5.53181e-05;
  weights[51]=5.53181e-05;
  weights[52]=5.53181e-05;

  if(intimepileup_<52)
    return weights[intimepileup_+1];
  else
    return 5.53181e-05;
}




void makeTrees_ElecTauStream(string analysis_ = "", string sample_ = "", float xsec_ = 0., string inputDir_ = "./"){

  cout << "Now skimming analysis " << analysis_ << endl;

  TMVA::Tools::Instance();
  
  vector<int> mHMVAs;
  mHMVAs.push_back(120);
  
  vector<string> bkgMVAs;
  bkgMVAs.push_back("VBF");
  
  
  std::map< string , TMVA::Reader*> readers;
  Float_t MJJ, DETA,DPHI,DITAUPT,DIJETPT,DPHIHJ,C1,C2;
  
  for(unsigned int k = 0 ; k < mHMVAs.size(); k++){
    for(unsigned int j = 0 ; j < bkgMVAs.size(); j++){
      
      TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );
      
      if(DOVBFMVA && bkgMVAs[j].find("VBF")!=string::npos){
	reader->AddVariable("mjj", &MJJ);
	reader->AddVariable("dEta", &DETA);
	reader->AddVariable("dPhi", &DPHI);
	reader->AddVariable("ditau_pt", &DITAUPT);
	reader->AddVariable("dijet_pt", &DIJETPT);
	reader->AddVariable("dPhi_hj", &DPHIHJ);
	reader->AddVariable("C1", &C1);
	reader->AddVariable("C2", &C2);
	reader->BookMVA("BDTG", "../../../UserCode/MitHtt/data/VBFMVA/MuTau/VBFMVA_BDTG.weights.xml");
	readers.insert(  make_pair(Form("%d%s",mHMVAs[k],bkgMVAs[j].c_str()), reader));
      }
     
    }
  }

  //////////////////////////////////////////////////////////
  //edm::Lumi3DReWeightingForLorenzo* Lumi3DReWeighting = 
  //new edm::Lumi3DReWeightingForLorenzo("../../Utilities/data/pileUp/pileUpFall11.root",
  //				 "../../Utilities/data/pileUp/Run2011PileUpTruth.root",
  //				 "MC_distr",
  //				 "pileup");
  //
  //Lumi3DReWeighting->weight3D_init(1.00);
  //cout << "Lumi3D rewieghting initialized" << endl;
  //////////////////////////////////////////////////////////
  
  cout << "Creating random engine..." << endl;
  TRandom3* ran = new TRandom3();
  ran->SetSeed(12345);  

  //////////////////////////////////////////////////////////

  RecoilCorrector* recoilCorr = 0;

  //////////////////////////////////////////////////////////

  cout << "Using corrections from llrCorrections.root" << endl;
  TFile corrections("../../Utilities/data/corrections/llrCorrections.root");

  TF1 *ratioElecIDBL      = (TF1*)corrections.Get("ratioElecIDBL");
  TF1 *ratioElecIsoBL     = (TF1*)corrections.Get("ratioElecIsoBL");
  TF1 *ratioElecIDEC      = (TF1*)corrections.Get("ratioElecIDEC");
  TF1 *ratioElecIsoEC     = (TF1*)corrections.Get("ratioElecIsoEC");

  //TF1 *ratioElecIDIsoBL   = (TF1*)corrections.Get("ratioElecIDIsoBL");
  //TF1 *ratioElecIDIsoEC   = (TF1*)corrections.Get("ratioElecIDIsoEC");

  TF1 *ratioTauElecTauAll = (TF1*)corrections.Get("ratioTauElecTauAll");
  //TF1 *ratioTauElecTauAllBL = (TF1*)corrections.Get("ratioTauElecTauAllBL");
  //TF1 *ratioTauElecTauAllEC = (TF1*)corrections.Get("ratioTauElecTauAllEC");
  TF1 *ratioElecAllBL     = (TF1*)corrections.Get("ratioElecAllBL");
  TF1 *ratioElecAllEC     = (TF1*)corrections.Get("ratioElecAllEC");
  TF1 *turnOnTauElecTauAll= (TF1*)corrections.Get("turnOnTauElecTauAll");
  //TF1 *turnOnTauElecTauAllBL = (TF1*)corrections.Get("turnOnTauElecTauAllBL");
  //TF1 *turnOnTauElecTauAllEC = (TF1*)corrections.Get("turnOnTauElecTauAllEC");
  TF1 *turnOnElecAllBL    = (TF1*)corrections.Get("turnOnElecAllBL");
  TF1 *turnOnElecAllEC    = (TF1*)corrections.Get("turnOnElecAllEC");

  //if(!ratioElecIDIsoBL)   cout << "Missing corrections for ElecID+Iso (BL)" << endl;
  //if(!ratioElecIDIsoEC)   cout << "Missing corrections for ElecID+Iso (EC)" << endl;
  if(!ratioElecIDBL)      cout << "Missing corrections for ElecID (BL)" << endl;
  if(!ratioElecIDEC)      cout << "Missing corrections for ElecID (EC)" << endl;
  if(!ratioElecIsoBL)     cout << "Missing corrections for ElecIso (BL)" << endl;
  if(!ratioElecIsoEC)     cout << "Missing corrections for ElecIso (EC)" << endl;
  if(!ratioElecAllBL)     cout << "Missing corrections for Elec HLT (BL)" << endl;
  if(!ratioElecAllEC)     cout << "Missing corrections for Elec HLT (EC)" << endl;
  if(!ratioTauElecTauAll) cout << "Missing corrections for tau HLT" << endl;
  //if(!ratioTauElecTauAllBL) cout << "Missing corrections for tau HLT (BL)" << endl;
  //if(!ratioTauElecTauAllEC) cout << "Missing corrections for tau HLT (EC)" << endl;
  if(!turnOnElecAllBL)    cout << "Missing turnOn for mu (BL)" << endl;
  if(!turnOnElecAllEC)    cout << "Missing turnOn for mu (EC)" << endl;
  if(!turnOnTauElecTauAll)cout << "Missing turnOn for tau" << endl;

  //////////////////////////////////////////////////////////
 
  //TFile embeddingUnfoldingLead("../../Utilities/data/unfolding/Unfold2D_1.root");
  //TFile embeddingUnfoldingSubLead("../../Utilities/data/unfolding/Unfold2D_2.root");
  //TH2F* UnfoldDen1 = embeddingUnfoldingLead.IsZombie()    ? 0 : (TH2F*)embeddingUnfoldingLead.Get("UnfoldDen1");
  //TH2F* UnfoldDen2 = embeddingUnfoldingSubLead.IsZombie() ? 0 : (TH2F*)embeddingUnfoldingSubLead.Get("UnfoldDen2");

  TFile embeddingUnfolding("../../Utilities/data/unfolding/Unfold2DEta.root");
  TH2F* UnfoldDen1 = embeddingUnfolding.IsZombie()    ? 0 : (TH2F*)embeddingUnfolding.Get("UnfoldDen1");
  if(UnfoldDen1) cout << "Unfolding for embedded sample open!!" << endl;


  //////////////////////////////////////////////////////////

  cout << "Using fake-rate method from fakeRate.root" << endl;
  TFile fakeRate_DYJets ("../../Utilities/data/fakeRate/fakeRate_DYJetsToTauTau_rebinned.root","READ");
  TFile fakeRate_Run2011("../../Utilities/data/fakeRate/fakeRate_Run2011AB_rebinned.root","READ");
  TFile fakeRate_WJets  ("../../Utilities/data/fakeRate/fakeRate_WJetsToMuNu_rebinned.root","READ");
  
  
  TH3F* hfakeRateDYJets  = 0;
  TH3F* hfakeRateRun2011 = 0;
  TH3F* hfakeRateWJets   = 0;
  
  
  if(USEFAKERATE){
    if(!fakeRate_DYJets.IsZombie()){
      cout << "Tau Efficiency from DY->tautau avalilable" << endl;
      hfakeRateDYJets  = (TH3F*)fakeRate_DYJets.Get("hRatioRebin");
    }
    //fakeRate_DYJets.Close();

    if(!fakeRate_Run2011.IsZombie()){
      cout << "Fake-rate from Run2011 avalilable" << endl;
      hfakeRateRun2011 = (TH3F*)fakeRate_Run2011.Get("hRatioRebin");
    }
    //fakeRate_Run2011.Close();

    if(!fakeRate_WJets.IsZombie()){
      cout << "Fake Rate from W+jets available" << endl;
      hfakeRateWJets   = (TH3F*)fakeRate_WJets.Get("hRatioRebin");
    }
    //fakeRate_WJets.Close();
  }
  
  //////////////////////////////////////////////////////////
 
  // normalization Lumi
  Float_t Lumi=1000;

  string currentSample = sample_ ;
  TString sample(currentSample.c_str());

  string analysisFileName = analysis_;
  if( !(analysis_.find("Up")!=string::npos || analysis_.find("Down")!=string::npos) &&  analysis_.find("Raw")==string::npos)
    analysisFileName = "Nominal";
  if( !(analysis_.find("Up")!=string::npos || analysis_.find("Down")!=string::npos) &&  analysis_.find("Raw")!=string::npos)
    analysisFileName = "RawNominal";

  TString outName = "nTuple"+sample+"_Open_ElecTauStream_"+analysisFileName+".root";
  cout << "Output file name called " << string(outName.Data()) << endl;

  //TFile *outFile = new TFile(outName,"UPDATE");
  //TTree* outTreePtOrd     = new TTree(TString(("outTreePtOrd"+analysis_).c_str()),"tree jets pT-ord");

  fwlite::TFileService fs = fwlite::TFileService(outName.Data());
  TTree* outTreePtOrd = fs.make<TTree>(TString(("outTreePtOrd"+analysis_).c_str()),"tree jets pT-ord");

  //////////////////////////////////////////////////////////

  // kinematical variables of first 2 jets  
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi,phi1,phi2;
  float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2,phi1_v2,phi2_v2;
  float diJetPt, diJetPhi, dPhiHjet, c1, c2;
  float ptB1, etaB1, phiB1;
  float MVAvbf;
  float jet1PUMVA, jet2PUMVA, jetVetoPUMVA;
  float jet1PUWP, jet2PUWP, jetVetoPUWP;
  int nJets30, nJets20;

  // quality cuts of the first 2 jets
  float jetsBtagHE1,jetsBtagHE2,jetsBtagHP1,jetsBtagHP2, jetsBtagCSV1, jetsBtagCSV2;
  int nJets20BTagged;
  float chFracPV1, chFracPV2;

  // kinematical variables of the veto jet
  float ptVeto, etaVeto, phiVeto;
  int isVetoInJets; float chFracPVVeto;

  // diTau related variables
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_, 
    diTauSVFitMass, diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float diTauSVFitMassSA, diTauSVFitMassErrSA; 
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauRecoPt,diTauRecoPhi;
  float diTauMinMass;

  float etaTau1Fit, etaTau2Fit, phiTau1Fit, phiTau2Fit, ptTau1Fit, ptTau2Fit;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2,dxy1_, dz1_, scEtaL1;
  float diTauCharge_,
    MtLeg1_,MtLeg1Corr_,MtLeg1MVA_,
    MtLeg2_,MtLeg2Corr_,MtLeg2MVA_,
    pZeta_,pZetaCorr_,pZetaMVA_,
    pZetaVis_,pZetaVisCorr_,pZetaVisMVA_,pZetaSig_;
  float MEt,MEtPhi,MEtCorr,MEtCorrPhi, MEtMVA, MEtMVAPhi;
  float MEtCov00,MEtCov01,MEtCov10,MEtCov11;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1DBetav2,
    combRelIsoLeg1Rho, combIsoLeg2;
  float rhoFastJet_;
  float isoLeg1MVA_;
  int tightestHPSDBWP_, tightestHPSMVAWP_, decayMode_;
  int tightestAntiEMVAWP_, tightestAntiECutWP_;
  float hpsMVA_;
  float pfJetPt_;

  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;
  float fakeRateRun2011, fakeRateWMC, effDYMC, CDFWeight;

  // electron related variables
  int tightestCutBasedWP_, tightestMVAWP_;
  int tightestMVAPOGNonTrigWP_;
  float mvaPOGTrig_, mvaPOGNonTrig_, mitMVA_;
  int isTriggerElectron_;

  float sihih_, dEta_, dPhi_, HoE_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, puWeight2, puWeight3D, embeddingWeight_,HqTWeight;
  int numOfLooseIsoDiTaus_;
  int nPUVertices_;

  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightElec,HLTweightTau, SFElec, SFTau, HLTElec, HLTTau, SFEtoTau;
  float HLTxQCD,HLTmatchQCD;
  float SFElecID, SFElecIso;
  int isTauLegMatched_,isElecLegMatched_,elecFlag_,genDecay_, leptFakeTau;

  // event id
  ULong64_t event_,run_,lumi_;
  int index_;
  int pairIndex_;


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

  outTreePtOrd->Branch("diJetPt",  &diJetPt , "diJetPt/F");
  outTreePtOrd->Branch("diJetPhi", &diJetPhi , "diJetPhi/F");
  outTreePtOrd->Branch("dPhiHjet", &dPhiHjet ,"dPhiHjet/F");
  outTreePtOrd->Branch("c1",       &c1 , "c1/F");
  outTreePtOrd->Branch("c2",       &c2 , "c2/F");

  outTreePtOrd->Branch("ptB1",  &ptB1, "ptB1/F");
  outTreePtOrd->Branch("etaB1", &etaB1,"etaB1/F");
  outTreePtOrd->Branch("phiB1", &phiB1,"phiB1/F");

  outTreePtOrd->Branch("dxy1",  &dxy1_ , "dxy1/F");
  outTreePtOrd->Branch("dz1",   &dz1_  , "dz1/F");
  

  outTreePtOrd->Branch("MVAvbf",        &MVAvbf,         "MVAvbf/F");
  outTreePtOrd->Branch("jet1PUMVA",     &jet1PUMVA,      "jet1PUMVA/F");
  outTreePtOrd->Branch("jet2PUMVA",     &jet2PUMVA,      "jet2PUMVA/F");
  outTreePtOrd->Branch("jetVetoPUMVA",  &jetVetoPUMVA,   "jetVetoPUMVA/F");
  outTreePtOrd->Branch("jet1PUWP",      &jet1PUWP,       "jet1PUWP/F");
  outTreePtOrd->Branch("jet2PUWP",      &jet2PUWP,       "jet2PUWP/F");
  outTreePtOrd->Branch("jetVetoPUWP",   &jetVetoPUWP,    "jetVetoPUWP/F");

  outTreePtOrd->Branch("jetsBtagHE1",  &jetsBtagHE1,"jetsBtagHE1/F");
  outTreePtOrd->Branch("jetsBtagHE2",  &jetsBtagHE2,"jetsBtagHE2/F");
  outTreePtOrd->Branch("jetsBtagHP1",  &jetsBtagHP1,"jetsBtagHP1/F");
  outTreePtOrd->Branch("jetsBtagHP2",  &jetsBtagHP2,"jetsBtagHP2/F");
  outTreePtOrd->Branch("jetsBtagCSV1", &jetsBtagCSV1,"jetsBtagCSV1/F");
  outTreePtOrd->Branch("jetsBtagCSV2", &jetsBtagCSV2,"jetsBtagCSV2/F");
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
  outTreePtOrd->Branch("diTauRecoPt",   &diTauRecoPt,  "diTauRecoPt/F");
  outTreePtOrd->Branch("diTauRecoPhi",  &diTauRecoPhi,  "diTauRecoPhi/F");
 
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
  outTreePtOrd->Branch("scEtaL1",   &scEtaL1,"scEtaL1/F");

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
  outTreePtOrd->Branch("MtLeg1MVA",   &MtLeg1MVA_,"MtLeg1MVA/F");
  outTreePtOrd->Branch("MtLeg2",      &MtLeg2_,"MtLeg2/F");
  outTreePtOrd->Branch("MtLeg2Corr",  &MtLeg2Corr_,"MtLeg2Corr/F");
  outTreePtOrd->Branch("MtLeg2MVA",   &MtLeg2MVA_,"MtLeg2MVA/F");
  outTreePtOrd->Branch("pZeta",       &pZeta_,"pZeta/F");
  outTreePtOrd->Branch("pZetaCorr",   &pZetaCorr_,"pZetaCorr/F");
  outTreePtOrd->Branch("pZetaMVA",    &pZetaMVA_,"pZetaMVA/F");
  outTreePtOrd->Branch("pZetaVis",    &pZetaVis_,"pZetaVis/F");
  outTreePtOrd->Branch("pZetaVisCorr",&pZetaVisCorr_,"pZetaVisCorr/F");
  outTreePtOrd->Branch("pZetaVisMVA", &pZetaVisMVA_,"pZetaVisMVA/F");
  outTreePtOrd->Branch("pZetaSig",    &pZetaSig_,"pZetaSig/F");

  outTreePtOrd->Branch("MEt",         &MEt,        "MEt/F");
  outTreePtOrd->Branch("MEtPhi",      &MEtPhi,     "MEtPhi/F");
  outTreePtOrd->Branch("MEtCorr",     &MEtCorr,    "MEtCorr/F");
  outTreePtOrd->Branch("MEtCorrPhi",  &MEtCorrPhi, "MEtCorrPhi/F");
  outTreePtOrd->Branch("MEtMVA",      &MEtMVA,     "MEtMVA/F");
  outTreePtOrd->Branch("MEtMVAPhi",   &MEtMVAPhi,  "MEtMVAPhi/F");

  outTreePtOrd->Branch("MEtCov00",    &MEtCov00,   "MEtCov00/F");
  outTreePtOrd->Branch("MEtCov01",    &MEtCov01,   "MEtCov01/F");
  outTreePtOrd->Branch("MEtCov10",    &MEtCov10,   "MEtCov10/F");
  outTreePtOrd->Branch("MEtCov11",    &MEtCov11,   "MEtCov11/F");

  outTreePtOrd->Branch("combRelIsoLeg1",     &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("combRelIsoLeg1Beta", &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho,"combRelIsoLeg1Rho/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBetav2",&combRelIsoLeg1DBetav2,"combRelIsoLeg1DBetav2/F");
  outTreePtOrd->Branch("isoLeg1MVA",         &isoLeg1MVA_,"isoLeg1MVA/F");
  outTreePtOrd->Branch("combIsoLeg2",        &combIsoLeg2,"combIsoLeg2/F");
  outTreePtOrd->Branch("rhoFastJet",         &rhoFastJet_,"rhoFastJet/F");

  outTreePtOrd->Branch("tightestCutBasedWP", &tightestCutBasedWP_,"tightestCutBasedWP/I");
  outTreePtOrd->Branch("tightestMVAWP",      &tightestMVAWP_,"tightestMVAWP/I");
  outTreePtOrd->Branch("tightestMVAPOGNonTrigWP", &tightestMVAPOGNonTrigWP_,"tightestMVAPOGNonTrigWP/I");
  outTreePtOrd->Branch("mvaPOGTrig",         &mvaPOGTrig_,   "mvaPOGTrig/F");
  outTreePtOrd->Branch("mvaPOGNonTrig",      &mvaPOGNonTrig_,"mvaPOGNonTrig/F");
  outTreePtOrd->Branch("mitMVA",             &mitMVA_,"mitMVA/F");
  outTreePtOrd->Branch("isTriggerElectron",  &isTriggerElectron_,"isTriggerElectron/I");

  outTreePtOrd->Branch("tightestAntiEMVAWP", &tightestAntiEMVAWP_,"tightestAntiEMVAWP/I");
  outTreePtOrd->Branch("tightestAntiECutWP", &tightestAntiECutWP_,"tightestAntiECutWP/I");

  outTreePtOrd->Branch("sihih",              &sihih_,"sihih/F");
  outTreePtOrd->Branch("dEta",               &dEta_,"dEta/F");
  outTreePtOrd->Branch("dPhi",               &dPhi_,"dPhi/F");
  outTreePtOrd->Branch("HoE",                &HoE_,"HoE/F");


  outTreePtOrd->Branch("tightestHPSDBWP",    &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("tightestHPSMVAWP",   &tightestHPSMVAWP_,"tightestHPSMVAWP/I");
  outTreePtOrd->Branch("hpsMVA",             &hpsMVA_,   "hpsMVA/F");
  outTreePtOrd->Branch("decayMode",          &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("puWeight2",          &puWeight2,"puWeight2/F");
  outTreePtOrd->Branch("puWeight3D",         &puWeight3D,"puWeight3D/F");
  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("HqTWeight",          &HqTWeight,"HqTWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");
  outTreePtOrd->Branch("nPUVertices",        &nPUVertices_, "nPUVertices/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightElec",&HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTElec",      &HLTElec,"HLTElec/F");
  outTreePtOrd->Branch("HLTTau",       &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("SFElec",       &SFElec,"SFElec/F");
  outTreePtOrd->Branch("SFEtoTau",     &SFEtoTau,"SFEtoTau/F");
  outTreePtOrd->Branch("SFElecID",     &SFElecID,"SFElecID/F");
  outTreePtOrd->Branch("SFElecIso",    &SFElecIso,"SFElecIso/F");
  outTreePtOrd->Branch("HLTxQCD",      &HLTxQCD,"HLTxQCD/F"); 
  outTreePtOrd->Branch("HLTmatchQCD",  &HLTmatchQCD,"HLTmatchQCD/F");
  
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("isElecLegMatched",&isElecLegMatched_,"isElecLegMatched/I");
  outTreePtOrd->Branch("elecFlag",        &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
  outTreePtOrd->Branch("index", &index_, "index/I");
  outTreePtOrd->Branch("pairIndex", &pairIndex_, "pairIndex/I");


  //string currentInName = "/data_CMS/cms/lbianchini/"+inputDir_+"//treeElecTauStream_"+sample_+".root" ;
  string currentInName = "/data_CMS/cms/anayak/"+inputDir_+"//treeElecTauStream_"+sample_+".root" ;
  //string currentInName = "/home/llr/cms/veelken/ArunAnalysis/CMSSW_5_2_6_July12/src/LLRAnalysis/TauTauStudies/test//treeElecTauStream-debug.root" ;

  TString inName(currentInName.c_str());
  TFile* file   = new TFile(inName,"READ");
  if(file->IsZombie()){
    cout << "No such file!" << endl;
    return;
  }

  string anlyzerName = analysis_;
  if( analysis_.find("Jet")!=string::npos && analysis_.find("Raw")==string::npos)
    anlyzerName = "";
  if( analysis_.find("Jet")!=string::npos && analysis_.find("Raw")!=string::npos)
    anlyzerName = "Raw";

  TString treeName(("elecTauStreamAnalyzer"+anlyzerName+"/tree").c_str());

  TTree* currentTree = (TTree*)file->Get(treeName);
  int nEntries    = currentTree->GetEntries() ;
  int nEventsRead = ((TH1F*)file->Get("allEventsFilter/totalEvents"))->GetBinContent(1) ;
  float crossSection = xsec_;
  float scaleFactor = (crossSection != 0) ? Lumi / (  float(nEventsRead)/crossSection )  : 1.0;

  cout << "Processing sample " << sample.Data() << endl;
  cout<< "nEventsRead " << nEventsRead << endl;
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
  currentTree->SetBranchStatus("jetsBtagCSV"           ,1);
  currentTree->SetBranchStatus("jetMoments"            ,0);
  currentTree->SetBranchStatus("jetsChNfraction"       ,1);
  currentTree->SetBranchStatus("jetsChEfraction"       ,0);
  currentTree->SetBranchStatus("jetPUMVA"              ,1);
  currentTree->SetBranchStatus("jetPUWP"               ,1);

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
  currentTree->SetBranchStatus("genTausP4"             ,1);
  //currentTree->SetBranchStatus("chIsoLeg1v1"           ,0);
  //currentTree->SetBranchStatus("nhIsoLeg1v1"           ,0);
  //currentTree->SetBranchStatus("phIsoLeg1v1"           ,0);
  //currentTree->SetBranchStatus("chIsoPULeg1v1"         ,0);
  //currentTree->SetBranchStatus("nhIsoPULeg1v1"         ,0);
  //currentTree->SetBranchStatus("phIsoPULeg1v1"         ,0);
  currentTree->SetBranchStatus("chIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("nhIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("phIsoLeg1v2"           ,1);
  currentTree->SetBranchStatus("elecIsoLeg1v2"         ,1);
  currentTree->SetBranchStatus("muIsoLeg1v2"           ,0);
  currentTree->SetBranchStatus("chIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("nhIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("phIsoPULeg1v2"         ,1);
  currentTree->SetBranchStatus("isoLeg1MVA"            ,1);

  currentTree->SetBranchStatus("chIsoLeg2"             ,1);
  currentTree->SetBranchStatus("nhIsoLeg2"             ,0);
  currentTree->SetBranchStatus("phIsoLeg2"             ,1);

  currentTree->SetBranchStatus("dxy1"                  ,0);
  currentTree->SetBranchStatus("dxy2"                  ,0);
  currentTree->SetBranchStatus("dz1"                   ,0);
  currentTree->SetBranchStatus("dz2"                   ,0);
  currentTree->SetBranchStatus("scEta1"                ,1);
  currentTree->SetBranchStatus("decayMode"             ,1);
  currentTree->SetBranchStatus("tightestHPSWP"         ,0);
  currentTree->SetBranchStatus("tightestHPSDBWP"       ,1);
  currentTree->SetBranchStatus("tightestHPSMVAWP"      ,1);
  currentTree->SetBranchStatus("hpsMVA"                ,1);
  currentTree->SetBranchStatus("tightestAntiEWP"       ,1);
  currentTree->SetBranchStatus("tightestAntiEMVAWP"    ,1);

  currentTree->SetBranchStatus("visibleTauMass"        ,1);
  currentTree->SetBranchStatus("visibleGenTauMass"     ,0);
  currentTree->SetBranchStatus("isTauLegMatched"       ,1);
  currentTree->SetBranchStatus("isElecLegMatched"      ,1);
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
  currentTree->SetBranchStatus("tightestCiCWP"         ,0);
  currentTree->SetBranchStatus("tightestMVAWP"         ,1);
  currentTree->SetBranchStatus("tightestMVAPOGNonTrigWP", 1);
  currentTree->SetBranchStatus("mvaPOGTrig"            ,1);
  currentTree->SetBranchStatus("mvaPOGNonTrig"         ,1);
  currentTree->SetBranchStatus("mitMVA"                ,1);
  currentTree->SetBranchStatus("antiConv"              ,0);
  currentTree->SetBranchStatus("isTriggerElectron"     ,1);

  // MET
  currentTree->SetBranchStatus("METP4"                 ,1);
  currentTree->SetBranchStatus("genMETP4"              ,1);
  currentTree->SetBranchStatus("sumEt"                 ,0);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,0);
  currentTree->SetBranchStatus("metSgnMatrix"          ,1);

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

  std::vector< float >* jetPUMVA =  new std::vector< float >();
  currentTree->SetBranchAddress("jetPUMVA", &jetPUMVA);

  std::vector< float >* jetPUWP =  new std::vector< float >();
  currentTree->SetBranchAddress("jetPUWP", &jetPUWP);

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

  std::vector< LV >*  genTausP4= new std::vector< LV >();
  currentTree->SetBranchAddress("genTausP4",    &genTausP4);


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
  std::vector< double >* jetsBtagCSV = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagCSV",     &jetsBtagCSV);

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
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,mTauTauMin;
  float diTauCharge;
  int tightestHPSDBWP, tightestHPSMVAWP, decayMode;
  int tightestAntiEMVAWP, tightestAntiECutWP;
  float hpsMVA;
  int tightestCutBasedWP, tightestMVAWP;
  int tightestMVAPOGNonTrigWP;
  float mvaPOGTrig, mvaPOGNonTrig, mitMVA;
  int isTriggerElectron;
  float sihih, dEta, dPhi, HoE;
  float numPV;
  float mcPUweight,embeddingWeight;
  int numOfLooseIsoDiTaus;
  int isTauLegMatched,isElecLegMatched,elecFlag,genDecay;
  float nPUVertices, nPUVerticesM1,nPUVerticesP1;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float pfJetPt;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float dxy1, dxy2, dz1, scEta1;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1;
  float allChIsoLeg1;
  float isoLeg1MVA;
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
  currentTree->SetBranchAddress("elecIsoLeg1v2" ,       &allChIsoLeg1);
  currentTree->SetBranchAddress("isoLeg1MVA",           &isoLeg1MVA);
 
  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);
  currentTree->SetBranchAddress("rhoNeutralFastJet",    &rhoNeutralFastJet);
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("tightestHPSMVAWP",     &tightestHPSMVAWP);
  currentTree->SetBranchAddress("hpsMVA",               &hpsMVA);
  currentTree->SetBranchAddress("diTauCharge",          &diTauCharge);

  currentTree->SetBranchAddress("tightestCutBasedWP",   &tightestCutBasedWP);
  currentTree->SetBranchAddress("tightestMVAWP",        &tightestMVAWP);
  currentTree->SetBranchAddress("tightestMVAPOGNonTrigWP", &tightestMVAPOGNonTrigWP);
  currentTree->SetBranchAddress("mvaPOGTrig",           &mvaPOGTrig);
  currentTree->SetBranchAddress("mvaPOGNonTrig",        &mvaPOGNonTrig);
  currentTree->SetBranchAddress("isTriggerElectron",    &isTriggerElectron);
  currentTree->SetBranchAddress("mitMVA",               &mitMVA);
  currentTree->SetBranchAddress("tightestAntiEMVAWP",   &tightestAntiEMVAWP);
  currentTree->SetBranchAddress("tightestAntiEWP",      &tightestAntiECutWP);

  currentTree->SetBranchAddress("dxy1",                 &dxy1);
  currentTree->SetBranchAddress("dz1",                  &dz1);
  currentTree->SetBranchAddress("dxy2",                 &dxy2);
  currentTree->SetBranchAddress("scEta1",               &scEta1);
  
  currentTree->SetBranchAddress("sihih",                &sihih);
  currentTree->SetBranchAddress("dEta",                 &dEta);
  currentTree->SetBranchAddress("dPhi",                 &dPhi);
  currentTree->SetBranchAddress("HoE",                  &HoE);

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
  currentTree->SetBranchAddress("elecFlag",             &elecFlag);
  currentTree->SetBranchAddress("isTauLegMatched",      &isTauLegMatched);
  currentTree->SetBranchAddress("isElecLegMatched",     &isElecLegMatched);
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

  int counter         = 0;
  
  for (int n = 0; n < nEntries ; n++) {

    currentTree->GetEntry(n);
    if(n%5000==0) cout << n << endl;

    // initialize variables filled only in the two jet case
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;phi1=-99;phi2=-99;
    pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;phi1_v2=-99;phi2_v2=-99;
    ptVeto = -99; phiVeto= -99; etaVeto= -99;isVetoInJets=-99;
    ptB1   = -99; phiB1  = -99; etaB1 = -99;
    diJetPt = -99; diJetPhi = -99; dPhiHjet = -99; c1 = -99; c2 = -99;
    chFracPV1=-99;chFracPV2=-99;chFracPVVeto=-99; jetsBtagHE1 = -99;jetsBtagHE2 = -99;jetsBtagHP1 = -99;jetsBtagHP2 = -99;
    jetsBtagCSV1 = -99; jetsBtagCSV2 = -99; 
    jet1PUMVA = -99; jet2PUMVA=-99; jetVetoPUMVA=-99; 
    jet1PUWP = -99; jet2PUWP = -99; jetVetoPUWP = -99;
    MVAvbf = -99;

    // define the relevant jet collection
    nJets20BTagged = 0;
    nJets30        = 0; nJets20 = 0;
    int lead  = -99;
    int trail = -99;
    int veto  = -99;
    vector<int> indexes;
    for(unsigned int l = 0 ; l < jets->size() ; l++){
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta && (*jetPUWP)[l*3]>MINJetID)
	indexes.push_back(l);
    }
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];

    for(unsigned int v = 0 ; v < indexes.size() ; v++){
      if( (*jets)[indexes[v]].Pt() > 30 ) nJets30++;
      if( (*jets)[indexes[v]].Pt() > 20 ) nJets20++;
      if( (*jets)[indexes[v]].Pt() > 20 && TMath::Abs((*jets)[indexes[v]].Eta())<2.4 && (*jetsBtagCSV)[indexes[v]] > 0.679 ){
	nJets20BTagged++;
	if(nJets20BTagged<2){
	  ptB1  = (*jets)[indexes[v]].Pt();
	  phiB1 = (*jets)[indexes[v]].Phi();
	  etaB1 = (*jets)[indexes[v]].Eta();
	}
      }  
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
      jetsBtagCSV1= (*jetsBtagCSV)[lead];

      chFracPV1 = (*jetsChNfraction)[lead]; 

      jet1PUMVA = (*jetPUMVA)[lead];
      jet1PUWP  = (*jetPUWP)[lead*3]; // WP loose

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
	jetsBtagCSV2= (*jetsBtagCSV)[trail];

	chFracPV2 = (*jetsChNfraction)[trail]; 

	jet2PUMVA = (*jetPUMVA)[trail];
	jet2PUWP  = (*jetPUWP)[trail*3]; // WP loose

	diJetPt  = ((*jets)[lead] + (*jets)[trail]).Pt();
	diJetPhi =  //((*jets)[lead]-(*jets)[trail]).Phi();
	  TMath::Abs(  ((*jets)[lead]-(*jets)[trail]).Phi() ) > TMath::Pi() ? 
	  -TMath::Abs( ((*jets)[lead] - (*jets)[trail]).Phi() ) + 2*TMath::Pi()  :
	   TMath::Abs( ((*jets)[lead] - (*jets)[trail]).Phi() ) ;
	dPhiHjet =  //((*jets)[lead]+(*jets)[trail]).Phi() - ((*diTauVisP4)[0] + METP4[1]).Phi() ;
	  TMath::Abs(  ((*jets)[lead]+(*jets)[trail]).Phi() - ((*diTauVisP4)[0] + (*METP4)[1]).Phi() ) > TMath::Pi() ? 
	  -TMath::Abs( ((*jets)[lead]+(*jets)[trail]).Phi() - ((*diTauVisP4)[0] + (*METP4)[1]).Phi() ) + 2*TMath::Pi()  :
	  TMath::Abs( ((*jets)[lead]+(*jets)[trail]).Phi() - ((*diTauVisP4)[0]  + (*METP4)[1]).Phi() ) ;
	c1 = TMath::Min(TMath::Abs( (*diTauVisP4)[0].Eta() - eta1), TMath::Abs((*diTauVisP4)[0].Eta() - eta2));
	c2 = (*diTauVisP4)[0].Pt() ;
	
	//MVA vbf
	MJJ     = Mjj; 
	DETA    = Deta;
	DPHI    = Dphi;
	DITAUPT = ((*diTauVisP4)[0]+(*METP4)[1]).Pt();
	DIJETPT = diJetPt;
	DPHIHJ  = dPhiHjet;
	C1      = c1 ;
	C2      = c2 ;
	
	MVAvbf    = readers["120VBF"]!=0 ? readers["120VBF"]->EvaluateMVA( "BDTG" ) : -99;
    	
      }

    }

    ptVeto       = (veto>=0) ? (*jets)[veto].Pt() : -99;
    etaVeto      = (veto>=0) ? (*jets)[veto].Eta(): -99;
    phiVeto      = (veto>=0) ? (*jets)[veto].Phi(): -99;
    chFracPVVeto = (veto>=0) ? (*jetsChNfraction)[veto] : -99;
    jetVetoPUMVA = (veto>=0) ? (*jetPUMVA)[veto]  : -99;
    jetVetoPUWP  = (veto>=0) ? (*jetPUWP)[veto*3] : -99; // WP loose

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

    diTauCAMass = (*diTauCAP4)[0].M();
    diTauCAPt   = (*diTauCAP4)[0].Pt();
    diTauCAEta  = (*diTauCAP4)[0].Eta();
    diTauCAPhi  = (*diTauCAP4)[0].Phi();

    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt    = (*diTauVisP4)[0].Pt();
    diTauVisEta   = (*diTauVisP4)[0].Eta();
    diTauVisPhi   = (*diTauVisP4)[0].Phi();

    diTauRecoPt  = ((*diTauVisP4)[0]+(*METP4)[0]).Pt();
    diTauRecoPhi = ((*diTauVisP4)[0]+(*METP4)[0]).Phi();

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
    scEtaL1 = scEta1;

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
	recoilCorr->CorrectType1(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi(), u1, u2 , err1,err2, TMath::Min(nJets30,2) );
      else if(sample_.find("DYJets")!=string::npos || sample_.find("H1")!=string::npos)  
	recoilCorr->CorrectType1(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Pt(),((*diTauLegsP4)[0]+(*diTauLegsP4)[1]).Phi(), u1, u2, err1,err2, TMath::Min(nJets30,2)  );
    }
    corrMET_tmp.SetPtEtaPhiM(corrPt,0,corrPhi,0);
    corrMET.SetPx(corrMET_tmp.Px());
    corrMET.SetPy(corrMET_tmp.Py());
    corrMET.SetPz(corrMET_tmp.Pz());
    corrMET.SetE(corrMET_tmp.E());

    ////////////////////////////////////////////////////////////////////

    LV recoilCorrecMET = corrMET;

    float scalarSumPtLeg1     = ( *diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPtLeg1     = ((*diTauLegsP4)[0] +      (*METP4)[0]).Pt() ;
    float scalarSumPtLeg1Corr = ( *diTauLegsP4)[0].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtLeg1Corr = ((*diTauLegsP4)[0] +      recoilCorrecMET).Pt() ;
    float scalarSumPtLeg1MVA  = ( *diTauLegsP4)[0].Pt() + (*METP4)[3].Pt();
    float vectorSumPtLeg1MVA  = ((*diTauLegsP4)[0] +      (*METP4)[3]).Pt() ;

    float scalarSumPtLeg2     = ( *diTauLegsP4)[1].Pt() + (*METP4)[0].Pt();
    float vectorSumPtLeg2     = ((*diTauLegsP4)[1] +      (*METP4)[0]).Pt() ;
    float scalarSumPtLeg2Corr = ( *diTauLegsP4)[1].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtLeg2Corr = ((*diTauLegsP4)[1] +      recoilCorrecMET).Pt() ;
    float scalarSumPtLeg2MVA  = ( *diTauLegsP4)[1].Pt() + (*METP4)[3].Pt();
    float vectorSumPtLeg2MVA  = ((*diTauLegsP4)[1] +      (*METP4)[3]).Pt() ;

    MtLeg1_     = TMath::Sqrt( scalarSumPtLeg1*scalarSumPtLeg1 - vectorSumPtLeg1*vectorSumPtLeg1 ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtLeg1Corr*scalarSumPtLeg1Corr - vectorSumPtLeg1Corr*vectorSumPtLeg1Corr ) ;
    MtLeg1MVA_  = TMath::Sqrt( scalarSumPtLeg1MVA*scalarSumPtLeg1MVA - vectorSumPtLeg1MVA*vectorSumPtLeg1MVA ) ;

    MtLeg2_     = TMath::Sqrt( scalarSumPtLeg2*scalarSumPtLeg2 - vectorSumPtLeg2*vectorSumPtLeg2 ) ;
    MtLeg2Corr_ = TMath::Sqrt( scalarSumPtLeg2Corr*scalarSumPtLeg2Corr - vectorSumPtLeg2Corr*vectorSumPtLeg2Corr ) ;
    MtLeg2MVA_  = TMath::Sqrt( scalarSumPtLeg2MVA*scalarSumPtLeg2MVA - vectorSumPtLeg2MVA*vectorSumPtLeg2MVA ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[1];
    pZetaMVA_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[3]))[1];
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[0];
    pZetaVisMVA_  = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[3]))[0];

    pZetaSig_     = pZetaSig;

    MEt        = (*METP4)[0].Et();
    MEtPhi     = (*METP4)[0].Phi();
    MEtCorr    = recoilCorrecMET.Et();
    MEtCorrPhi = recoilCorrecMET.Phi();
    MEtMVA     = (*METP4)[1].Et();
    MEtMVAPhi  = (*METP4)[1].Phi();

    ////////////////////////////////////////////////
    
    MEtCov00   = (*metSgnMatrix)[0]; 
    MEtCov01   = (*metSgnMatrix)[1]; 
    MEtCov10   = (*metSgnMatrix)[1]; 
    MEtCov11   = (*metSgnMatrix)[2]; 

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
    unsigned int flagSA = 0;
    NSVfitStandaloneAlgorithm algo(measuredTauLeptons,measuredMET,*metsig,flagSA);
    //algo.maxObjFunctionCalls(5000);
    algo.addLogM(false);
    if(DOSVFITSTANDALONE) {
      //algo.fit();
      algo.integrate();
    }
    if(DOSVFITSTANDALONE){
      diTauSVFitMassSA    =  algo.getMass();//algo.fittedDiTauSystem().mass();
      diTauSVFitMassErrSA = -99;//algo.massUncert();
      etaTau1Fit          = -99;//((algo.fittedTauLeptons())[1]).Eta();
      etaTau2Fit          = -99;//((algo.fittedTauLeptons())[0]).Eta();
      phiTau1Fit          = -99;//((algo.fittedTauLeptons())[1]).Phi();
      phiTau2Fit          = -99;//((algo.fittedTauLeptons())[0]).Phi();
      ptTau1Fit           = -99;//((algo.fittedTauLeptons())[1]).Pt();
      ptTau2Fit           = -99;//((algo.fittedTauLeptons())[0]).Pt();
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

    combRelIsoLeg1         =  (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt() ;
    float PUtoPVratio      = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta     =  (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta    =  (chIsoLeg1      + std::max( nhIsoLeg1+phIsoLeg1-0.5*(nhIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBetav2  =  (allChIsoLeg1   + std::max( nhIsoLeg1+phIsoLeg1-0.5*(nhIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();
    float EffArea          = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho      = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2            =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    
    rhoFastJet_            = rhoFastJet;
    isoLeg1MVA_            = isoLeg1MVA;

    tightestCutBasedWP_ = tightestCutBasedWP;
    tightestMVAWP_      = tightestMVAWP;
    tightestMVAPOGNonTrigWP_ = tightestMVAPOGNonTrigWP;
    mvaPOGTrig_         = mvaPOGTrig;
    mvaPOGNonTrig_      = mvaPOGNonTrig;
    mitMVA_             = mitMVA;
    isTriggerElectron_  = isTriggerElectron;

    tightestAntiECutWP_ = tightestAntiECutWP;
    tightestAntiEMVAWP_ = tightestAntiEMVAWP;

    sihih_ = sihih; 
    dEta_  = dEta; 
    dPhi_  = dPhi; 
    HoE_   = HoE;


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

    tightestHPSDBWP_    = tightestHPSDBWP;
    tightestHPSMVAWP_   = tightestHPSMVAWP;
    hpsMVA_           = hpsMVA;
    
    pfJetPt_            = pfJetPt;

    decayMode_          = decayMode;
    numPV_              = numPV;
    sampleWeight        = scaleFactor; 
    puWeight            = (std::string(sample.Data())).find("Run2012")!=string::npos ? 1.0 : pileupWeight(nPUVertices);
    puWeight2           = (std::string(sample.Data())).find("Run2012")!=string::npos ? 1.0 : pileupWeight2(int(nPUVertices));   
    puWeight3D          = -99;
    nPUVertices_        = nPUVertices;
    embeddingWeight_    = embeddingWeight;

    if((std::string(sample.Data())).find("Embed")!=string::npos && UnfoldDen1 && genTausP4->size()>1){
      float corrFactorEmbed = (UnfoldDen1->GetBinContent( UnfoldDen1->GetXaxis()->FindBin( (*genTausP4)[0].Eta() ) ,  UnfoldDen1->GetYaxis()->FindBin( (*genTausP4)[1].Eta() ) )); 
      embeddingWeight_ *=  corrFactorEmbed;
      //cout << "Correcting with " << corrFactorEmbed << endl;
    }


    HqTWeight = histo!=0 ? histo->GetBinContent( histo->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    if((std::string(sample.Data())).find("Run2012")!=string::npos){

      HLTx =  float((*triggerBits)[0] || (*triggerBits)[1] ||
		    (*triggerBits)[2] || (*triggerBits)[3]); 
      //HLT_Ele20_CaloIdVT_CaloIsoRhoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v4, HLT_Ele20_CaloIdVT_CaloIsoRhoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v5, HLT_Ele20_CaloIdVT_CaloIsoRhoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v6, HLT_Ele22_eta2p1_WP90Rho_LooseIsoPFTau20_v2
      HLTxQCD =  float((*triggerBits)[4] || (*triggerBits)[5] || 
		       (*triggerBits)[6] || (*triggerBits)[7]);
      //HLT_Ele20_CaloIdVT_TrkIdT_LooseIsoPFTau20_v4, HLT_Ele20_CaloIdVT_TrkIdT_LooseIsoPFTau20_v5, HLT_Ele20_CaloIdVT_TrkIdT_LooseIsoPFTau20_v6, HLT_Ele22_eta2p1_WP90NoIso_LooseIsoPFTau20_v2

      bool isTriggMatched = (((*tauXTriggers)[0] && (*tauXTriggers)[4])  ||
			     ((*tauXTriggers)[1] && (*tauXTriggers)[5]));
      HLTmatch = isTriggMatched ? 1.0 : 0.0 ;
      //hltOverlapFilterIsoEle20LooseIsoPFTau20 || hltOverlapFilterIsoEle20WP90LooseIsoPFTau20
      bool isTriggMatchedQCD = (((*tauXTriggers)[2] && (*tauXTriggers)[6])  || 
				((*tauXTriggers)[3] && (*tauXTriggers)[7])); 
      HLTmatchQCD = isTriggMatchedQCD ? 1.0 : 0.0 ;


      HLTweightTau  = 1.0;
      HLTweightElec = 1.0;
      if( (std::string(sample.Data())).find("Embed")!=string::npos ){

	HLTElec = TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?
	  turnOnElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	  turnOnElecAllEC->Eval((*diTauLegsP4)[0].Pt());

	HLTTau  =  turnOnTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() ) ; 
	//((*diTauLegsP4)[1].Eta()<1.5) ? 
	//  turnOnTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
	//  turnOnTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;

	SFTau         = 1.0;
	SFElec        =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ? 
	  //ratioElecIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :
	  //ratioElecIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;
	  ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
	  ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;

	SFElecID =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?  
	  ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() ) :   
	  ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() );  
 
	SFElecIso =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?  
	  ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :   
	  ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() ); 
	
	SFEtoTau      = 1.0;

      }
      else{
	HLTElec       = 1.0;
	HLTTau        = 1.0;
	SFTau         = 1.0;
	SFElec        = 1.0;
	SFEtoTau      = 1.0;
      }

     

    } else{

      HLTx     = float((*triggerBits)[0]);
      //HLTmatch = ((*tauXTriggers)[0] && (*tauXTriggers)[2]) ? 1.0 : 0.0;
      HLTmatch = ((*tauXTriggers)[1] && (*tauXTriggers)[2]) ? 1.0 : 0.0;

      HLTxQCD = 1.0;
      HLTmatchQCD = 1.0;

      HLTweightTau  = ratioTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      //HLTweightTau  = ((*diTauLegsP4)[1].Eta()<1.5) ? 
      //ratioTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
      //ratioTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;
      
      if((*diTauLegsP4)[0].Pt()>24){
	HLTweightElec = TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?
	  ratioElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	  ratioElecAllEC->Eval((*diTauLegsP4)[0].Pt());
      }
      else HLTweightElec = 1.0;

      HLTTau  = turnOnTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      //HLTTau  = ((*diTauLegsP4)[1].Eta()<1.5) ? 
      //  turnOnTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
      //  turnOnTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;

      HLTElec = TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ? 
	turnOnElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	turnOnElecAllEC->Eval((*diTauLegsP4)[0].Pt());

      SFTau  = 1.0;
      SFElec =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?
	//ratioElecIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :
	//ratioElecIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;
	ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
	ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() );

      SFElecID =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ? 
        ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() ) :  
        ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() ); 

      SFElecIso =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ? 
        ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :  
        ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() );

      if((*diTauLegsP4)[0].Pt() >= 30.0 && (*diTauLegsP4)[0].Pt() < 31.0){
	SFElec =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ? 
	  ratioElecIDBL->Eval( 31.5 )*ratioElecIsoBL->Eval( 31.5 ) :  
	  ratioElecIDEC->Eval( 31.5 )*ratioElecIsoEC->Eval( 31.5 ); 
 
	SFElecID =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?  
	  ratioElecIDBL->Eval( 31.5 ) :   
	  ratioElecIDEC->Eval( 31.5 );  
 
	SFElecIso =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.479 ?  
	  ratioElecIsoBL->Eval( 31.5 ) :   
	  ratioElecIsoEC->Eval( 31.5 ); 
      }

      SFEtoTau = 1.0;

    }
 
     isTauLegMatched_ = isTauLegMatched;
     isElecLegMatched_ = isElecLegMatched;

    if((std::string(sample.Data())).find("Run2012")==string::npos)
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

    if(leptFakeTau==1){
      //float extraSmearing = ran->Gaus( -0.373, 1.18 );
      //diTauVisMass += extraSmearing;
      SFEtoTau      = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ? ETOTAUEB : ETOTAUEE;
    }

    elecFlag_        = elecFlag;
    genDecay_        = genDecay ;
    event_           = event;
    run_             = run;
    lumi_            = lumi;
    index_           = index;


    int pairIndex = -1;
    bool passQualityCuts = tightestHPSMVAWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(etaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch
      && ((mvaPOGNonTrig>0.925 && TMath::Abs(etaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(etaL1)>0.8 && TMath::Abs(etaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(etaL1)>1.479) );
    if( !(run==lastRun && lumi==lastLumi && event==lastEvent) ){

      lastEvent = event;
      lastLumi  = lumi;
      lastRun   = run;
      counter   = 0;
      if( passQualityCuts ){
	pairIndex = counter;
	counter++;
      }
      else
	pairIndex = -1;
    }
    else{
      if( passQualityCuts ){
	pairIndex = counter;
	counter++;
      }
      else
	pairIndex = -1;
    }
    pairIndex_ = pairIndex;


    outTreePtOrd->Fill();
  }


 file->Close();

 //if(SAVE) outFile->Write();
 //outFile->Close();

 delete jets; delete jets_v2; delete diTauLegsP4; delete diTauVisP4; delete diTauSVfitP4; delete diTauCAP4; delete genDiTauLegsP4; delete genTausP4;
 delete tauXTriggers; delete triggerBits;
 delete METP4; delete jetsBtagHE; delete jetsBtagHP; delete jetsBtagCSV; delete jetsChNfraction; delete genVP4; delete genMETP4;
 delete gammadEta; delete gammadPhi; delete gammaPt;  delete HqT; 
 delete jetPUMVA; delete jetPUWP;
 delete ran;

 for(std::map<string , TMVA::Reader*>::iterator read = readers.begin() ; read!=readers.end(); read++)
   delete (*read).second;
 
 return;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



void doAllSamplesElec(string inputDir_ = "ElecTauStreamFall11_04May2012"){

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  ////////////// samples & x-sections & skim1 & skim2 /////////////

  samples.push_back("Run2011-ElecTau-All_run");             crossSec.push_back( 0  );      
  //samples.push_back("Run2011-ElecTau-LooseIso-All_run");    crossSec.push_back( 0  );                          
  samples.push_back("Run2011-ElecTau-Embedded-All_run");    crossSec.push_back( 0  );                          
  samples.push_back("DYJets-ElecTau-50-madgraph-PUS6_run"); crossSec.push_back( 3048           * 0.0537207         * 0.2718375); 
  samples.push_back("TTJets-ElecTau-madgraph-PUS6_run");    crossSec.push_back( 157.5          * 0.0149329         * 0.7827282);  
  samples.push_back("WJets-ElecTau-madgraph-PUS6_run");     crossSec.push_back(31314.0         * 0.0011910         * 0.5374372);   
  samples.push_back("W3Jets-ElecTau-madgraph-PUS6_run");    crossSec.push_back( 304.0          * 1.0               * 0.1175129);   
  samples.push_back("WZ-ElecTau-pythia-PUS6_run");          crossSec.push_back( 18.2           * 0.0111572         * 0.5470729);         
  samples.push_back("ZZ-ElecTau-pythia-PUS6_run");          crossSec.push_back(  5.9           * 0.0160517         * 0.5203837);    
  samples.push_back("WW-ElecTau-pythia-PUS6_run");          crossSec.push_back( 43.0           * 0.0092734         * 0.5797184);        
  samples.push_back("VBFH100-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.36e-02*1.546 * 0.0499282         * 0.7341110); 
  samples.push_back("GGFH100-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.36e-02*24.02 * 1.0               * 0.0593094); 
  samples.push_back("VBFH105-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.25e-02*1.472 * 1.0               * 0.1153245); 
  samples.push_back("GGFH105-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.25e-02*21.78 * 1.0               * 0.0645510); 
  samples.push_back("VBFH110-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.02e-02*1.398 * 0.0558070         * 0.7335979);
  samples.push_back("GGFH110-ElecTau-powheg-PUS6_run");     crossSec.push_back( 8.02e-02*19.84 * 0.0441026         * 0.6997732);    
  samples.push_back("VBFH115-ElecTau-powheg-PUS6_run");     crossSec.push_back( 7.65e-02*1.332 * 0.0586927         * 0.7382737);
  samples.push_back("GGFH115-ElecTau-powheg-PUS6_run");     crossSec.push_back( 7.65e-02*18.13 * 0.0463465         * 0.7136729);  
  samples.push_back("VBFH120-ElecTau-powheg-PUS6_run");     crossSec.push_back( 7.10e-02*1.269 * 1.0               * 0.1274666);   
  samples.push_back("GGFH120-ElecTau-powheg-PUS6_run");     crossSec.push_back( 7.10e-02*16.63 * 1.0         * 0.0781068);  
  samples.push_back("VBFH125-ElecTau-powheg-PUS6_run");     crossSec.push_back( 6.37e-02*1.211 * 1.0         * 0.1309841);   
  samples.push_back("GGFH125-ElecTau-powheg-PUS6_run");     crossSec.push_back( 6.37e-02*15.31 * 1.0         * 0.0822008); 
  samples.push_back("VBFH130-ElecTau-powheg-PUS6_run");     crossSec.push_back( 5.48e-02*1.154 * 1.0         * 0.1345095); 
  samples.push_back("GGFH130-ElecTau-powheg-PUS6_run");     crossSec.push_back( 5.48e-02*14.12 * 1.0         * 0.0853114);
  samples.push_back("VBFH135-ElecTau-powheg-PUS6_run");     crossSec.push_back( 4.52e-02*1.100 * 1.0         * 0.1395949); 
  samples.push_back("GGFH135-ElecTau-powheg-PUS6_run");     crossSec.push_back( 4.52e-02*13.08 * 0.0573246   * 0.7114996);    
  samples.push_back("VBFH140-ElecTau-powheg-PUS6_run");     crossSec.push_back( 3.54e-02*1.052 * 1.0         * 0.1416138);  
  samples.push_back("GGFH140-ElecTau-powheg-PUS6_run");     crossSec.push_back( 3.54e-02*12.13 * 1.0         * 0.0951278);  
  samples.push_back("VBFH145-ElecTau-powheg-PUS6_run");     crossSec.push_back( 2.61e-02*1.004 * 1.0         * 0.1451686);  
  samples.push_back("GGFH145-ElecTau-powheg-PUS6_run");     crossSec.push_back( 2.61e-02*11.27 * 1.0         * 0.0983843);  
  samples.push_back("VBFH160-ElecTau-powheg-PUS6_run");     crossSec.push_back( 5.32e-04*0.8787* 1.0         * 0.1537070);  
  samples.push_back("GGFH160-ElecTau-powheg-PUS6_run");     crossSec.push_back( 5.32e-04*9.080 * 1.0         * 0.1087073);  
  samples.push_back("VH100-ElecTau-pythia-PUS6_run");  crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 1.0 * 0.1490132);  
  samples.push_back("VH110-ElecTau-pythia-PUS6_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 0.084386 * 0.76245288);
  samples.push_back("VH115-ElecTau-pythia-PUS6_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 1.0 * 0.1668136);
  samples.push_back("VH120-ElecTau-pythia-PUS6_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0 * 0.1732820);   
  samples.push_back("VH125-ElecTau-pythia-PUS6_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 1.0 * 0.1787773);
  samples.push_back("VH130-ElecTau-pythia-PUS6_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 0.10051 * 0.7730161);
  samples.push_back("VH135-ElecTau-pythia-PUS6_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 0.10133 * 0.7768092);
  samples.push_back("VH140-ElecTau-pythia-PUS6_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 1.0 * 0.1919186);  
  samples.push_back("VH145-ElecTau-pythia-PUS6_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 0.10708 * 0.774027);  
  samples.push_back("VH160-ElecTau-pythia-PUS6_run");  crossSec.push_back( 5.32e-04*(0.2291+0.1334+0.03942 ) * 1.0 * 0.2120560);  
  
  makeTrees_ElecTauStream("",             samples[2], crossSec[2], inputDir_);

  return;


  for( unsigned int k = 0; k < samples.size() ; k++) {

    makeTrees_ElecTauStream("",        samples[k], crossSec[k], inputDir_);

    if( samples[k].find("Run2011-ElecTau-All")!=string::npos )
      continue;
    makeTrees_ElecTauStream("TauUp",   samples[k], crossSec[k], inputDir_);
    makeTrees_ElecTauStream("TauDown", samples[k], crossSec[k], inputDir_);
    if( samples[k].find("Embedded")!=string::npos)
      continue;
    makeTrees_ElecTauStream("JetUp",   samples[k], crossSec[k], inputDir_);
    makeTrees_ElecTauStream("JetDown", samples[k], crossSec[k], inputDir_);
  }
  
 
  
  return;
  
}




int main(int argc, const char* argv[])
{
  
  std::cout << "doAllSamplesElec()" << std::endl;
  gROOT->SetBatch(true);
  
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();
  
  if ( argc < 3 ) {
    std::cout << "Usage: " << argv[0] << " fileName xsection" << std::endl;
    return 0;
  }
  
  //doAllSamplesElec( "ElecTauStreamFall11_04May2012");
  //return 1;  

  //string inputDir = "HTauTauSynchronization/FullSample/8TeV";
  //string inputDir = "HTauTauSynchronization/8TeV";
  string inputDir = "HTauTauSynchronization/SyncDataCard-53X-v1/ElecTauStream/";

  makeTrees_ElecTauStream("",           argv[1], atof(argv[2]), inputDir);
  //makeTrees_ElecTauStream("Raw",        argv[1], atof(argv[2]), inputDir);
  return 0;

  if( string(argv[1]).find("Run2012-ElecTau-All")!=string::npos )
    return 0;
  makeTrees_ElecTauStream("TauUp",      argv[1], atof(argv[2]), inputDir);
  makeTrees_ElecTauStream("TauDown",    argv[1], atof(argv[2]), inputDir);
  //makeTrees_ElecTauStream("RawTauUp",   argv[1], atof(argv[2]), inputDir);
  //makeTrees_ElecTauStream("RawTauDown", argv[1], atof(argv[2]), inputDir);
  if( string(argv[1]).find("Embedded")!=string::npos)
    return 0;
  makeTrees_ElecTauStream("JetUp",      argv[1], atof(argv[2]), inputDir);
  makeTrees_ElecTauStream("JetDown",    argv[1], atof(argv[2]), inputDir);
  //makeTrees_ElecTauStream("RawJetUp",   argv[1], atof(argv[2]), inputDir);
  //makeTrees_ElecTauStream("RawJetDown", argv[1], atof(argv[2]), inputDir);

}
