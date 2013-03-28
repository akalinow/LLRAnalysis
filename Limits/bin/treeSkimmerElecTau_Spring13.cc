#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TTree.h"
#include "TChain.h"
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
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "LLRAnalysis/Utilities/interface/RecoilCorrector.hh"
//#include "LLRAnalysis/Utilities/interface/Lumi3DReWeightingForLorenzo.h"
//#include "PhysicsTools/Utilities/interface/LumiReweightingStandAlone.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "TauAnalysis/CandidateTools/interface/NSVfitStandaloneAlgorithm.h"
#include "TauAnalysis/CandidateTools/interface/candidateAuxFunctions.h"
#include "TauAnalysis/CandidateTools/interface/neuralMtautauAuxFunctions.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "LLRAnalysis/Utilities/interface/BtagSF.hh"
#include "LLRAnalysis/Utilities/interface/readJSONFile.h"

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
#define tauLowHigh 40.0

#define USERECOILALGO true
#define USEFAKERATE false
#define DOSVFITSTANDALONE false
#define DOVBFMVA true
#define DEBUG false

// Weights of differents periods
#define wA  0.04185 //0.067   L=810.99
#define wB  0.22924 //0.367   L=4442.288743
#define wC  0.35354 //0.566   L=6851.050214
#define wD  0.37537 //0       L=7274
// total L(ABC)= 12104.329
// total L(D)  = 7274
// total ABCD  = 19378.329

using namespace ROOT::Math;
using namespace std;

typedef map< float , int > MAPDITAU_etaL2;
typedef map< float , MAPDITAU_etaL2 > MAPDITAU_etaL1;
typedef map< float , MAPDITAU_etaL1 > MAPDITAU_ptL2;
typedef map< float , MAPDITAU_ptL2 > MAPDITAU_ptL1;
typedef map< int , MAPDITAU_ptL1 > MAPDITAU_event;
typedef map< int , MAPDITAU_event > MAPDITAU_lumi;
typedef map< int , MAPDITAU_lumi > MAPDITAU_run;

typedef std::vector<std::string> vstring;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;
edm::LumiReWeighting LumiWeights_("/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root","/data_CMS/cms/htautau/Moriond/tools/Data_Pileup_2012_Moriond-600bins.root","pileup","pileup");
edm::LumiReWeighting LumiWeightsHCP_("/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root","/data_CMS/cms/htautau/Moriond/tools/Data_Pileup_2012_HCP-600bins.root","pileup","pileup"); 
edm::LumiReWeighting LumiWeightsD_("/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root","/data_CMS/cms/htautau/Moriond/tools/Data_Pileup_2012_DOnly-600bins.root","pileup","pileup"); 

enum BVariation{kNo = 0, kDown = 1, kUp = 2};
BtagSF* btsf = new BtagSF(12345);

float correctL1etm(float L1etm, float caloMEtNoHF=0, float caloMEtNoHFcorr=0, TString method="Luca1") {

  //difference of 'energy scale' found by fittig landau convoluted with gaus (MPV_data/MPV_mc=1.59390e+01/1.82863e+01)
  if(method=="Michal") 
    return L1etm*0.8716; 

  // include caloMEtNoHF corrections and parametrized dependance on caloMEtNoHF (fit to the ratio of the l1etm*corr1 mean values in data/MC)
  else if(method=="Luca1") 
    return L1etm*(caloMEtNoHFcorr/caloMEtNoHF)*(0.01134*TMath::Sqrt(caloMEtNoHF)+0.9422) ;

  // soon : gaussian smearing to be introduced

  else return L1etm;
}

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

double deltaR(LV v1, LV v2) {

  double deta = v1.Eta() - v2.Eta();
  double dphi = v1.Phi() - v2.Phi();
  return TMath::Sqrt( TMath::Power(deta,2) + TMath::Power(dphi,2) );

}


float reweightHEPNUPWJets(int hepNUP) {

  int nJets = hepNUP-5;
  
//   if(nJets==0)      return 1 ;
//   else if(nJets==1) return 0.368748897 ;
//   else if(nJets==2) return 0.114009944 ;
//   else if(nJets==3) return 0.077158785 ;
//   else if(nJets>=4) return 0.038490064 ;
//   else return 1 ;

//   if(nJets==0)      return 0.723706428 ;
//   else if(nJets==1) return 0.210195224 ;
//   else if(nJets==2) return 0.060963485 ;
//   else if(nJets==3) return 0.091456958 ;
//   else if(nJets>=4) return 0.034384641 ;
//   else return 1 ;

  if(nJets==0)      return 0.492871535 ;
  else if(nJets==1) return 0.181745835 ;
  else if(nJets==2) return 0.056192256 ;
  else if(nJets==3) return 0.038029369 ;
  else if(nJets>=4) return 0.018970657 ;
  else return 1 ;
}

float reweightHEPNUPDYJets(int hepNUP) {

  int nJets = hepNUP-5;
  
  if(nJets==0)      return 0.115028141 ;
  else if(nJets==1) return 0.027710126 ;
  else if(nJets==2) return 0.0098376 ;
  else if(nJets==3) return 0.005509647 ;
  else if(nJets>=4) return 0.004266394 ;
  else return 1 ;
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

  float intimepileup = intimepileup_; //>59 ? 59 : intimepileup_; 
  
  //return LumiWeights_.ITweight(intimepileup); 
  return LumiWeights_.weight(intimepileup);
  
}

float pileupWeightHCP( float intimepileup_ ){

  float intimepileup = intimepileup_; //>59 ? 59 : intimepileup_;

  //return LumiWeights_.ITweight(intimepileup);
  return LumiWeightsHCP_.weight(intimepileup);
  
}

float pileupWeightD( float intimepileup_ ){

  float intimepileup = intimepileup_; //>59 ? 59 : intimepileup_;

  //return LumiWeights_.ITweight(intimepileup);
  return LumiWeightsD_.weight(intimepileup);
  
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



int getJetIDMVALoose(double pt, double eta, double rawMVA)
{
  float eta_bin[] = {0,2.5,2.75,3.0,5.0};
  float Pt010_Loose[]    = {-0.95,-0.97,-0.97,-0.97};
  float Pt1020_Loose[]   = {-0.95,-0.97,-0.97,-0.97};
  float Pt2030_Loose[]   = {-0.80,-0.85,-0.84,-0.85};
  float Pt3050_Loose[]   = {-0.80,-0.74,-0.68,-0.77};

  int passId = 0;
  for(int i = 0; i < 4; i++){
    if(TMath::Abs(eta) >= eta_bin[i] && TMath::Abs(eta) < eta_bin[i+1]){
      if(pt < 10){
	if(rawMVA > Pt010_Loose[i])passId = 1;
      }
      else if(pt >= 10 && pt < 20){
	if(rawMVA > Pt1020_Loose[i])passId = 1;
      }
      else if(pt >= 20 && pt < 30){ 
        if(rawMVA > Pt2030_Loose[i])passId = 1; 
      }
      else if(pt >= 30){  
        if(rawMVA > Pt3050_Loose[i])passId = 1;  
      }
    }
  }

  return passId;
}

bool checkEventIsDuplicated(MAPDITAU_run &mapDiTau, int run, int lumi, int event, float ptL1, float ptL2, float etaL1, float etaL2)
{
  
  MAPDITAU_run::const_iterator iter_run = mapDiTau.find(run) ;

  if( iter_run != mapDiTau.end() ) {
    MAPDITAU_lumi::const_iterator iter_lumi = iter_run->second.find(lumi) ;
    
    if( iter_lumi != iter_run->second.end() ) {
      MAPDITAU_event::const_iterator iter_event = iter_lumi->second.find(event) ;
      
      if( iter_event != iter_lumi->second.end() ) {
	MAPDITAU_ptL1::const_iterator iter_ptL1 = iter_event->second.find(ptL1) ;
	
	if( iter_ptL1 != iter_event->second.end() ) {
	  MAPDITAU_ptL2::const_iterator iter_ptL2 = iter_ptL1->second.find(ptL2) ;
	  
	  if( iter_ptL2 != iter_ptL1->second.end() ) {
	    MAPDITAU_etaL1::const_iterator iter_etaL1 = iter_ptL2->second.find(etaL1) ;
	    
	    if( iter_etaL1 != iter_ptL2->second.end() ) {
	      MAPDITAU_etaL2::const_iterator iter_etaL2 = iter_etaL1->second.find(etaL2) ;

	      if( iter_etaL2 != iter_etaL1->second.end() ) {
		return true;
	      }
	    }
	  }
	}
      }
    }
  }

  mapDiTau[run][lumi][event][ptL1][ptL2][etaL1][etaL2] = 1;

  return false;
}

void fillTrees_ElecTauStream( TChain* currentTree,
			      TTree* outTreePtOrd,
			      double nEventsRead =0.,
			      string analysis_ = "", 
			      string sample_ = "",
			      float xsec_ = 0., 
			      float skimEff_ = 0., 
			      int iJson_=-1,
			      bool doLepVeto=true
			      )
{
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
	//reader->AddVariable("dPhi", &DPHI);
	//reader->AddVariable("ditau_pt", &DITAUPT);
	//reader->AddVariable("dijet_pt", &DIJETPT);
	//reader->AddVariable("dPhi_hj", &DPHIHJ);
	reader->AddVariable("C1", &C1);
	reader->AddVariable("C2", &C2);
	//reader->BookMVA("BDTG", "../../../UserCode/MitHtt/data/VBFMVA/MuTau/VBFMVA_BDTG.weights.xml");
	reader->BookMVA("BDTG", "../../../UserCode/MitHtt/data/VBFMVA/MuTau/VBFMVA_BDTG_HCP_52X.weights.xml");
	//reader->BookMVA("BDTG", "../VBFMVA/MuTau/VBFMVA_BDTG_HCP_52X.weights.xml");
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

  ////////////////////
  //   CORRECTIONS  //
  ////////////////////

  cout << "Using corrections from llrCorrections_Moriond.root" << endl;

  TFile corrections("/data_CMS/cms/htautau/Moriond/tools/llrCorrections_Moriond_v2.root");
  
  // Ele trigger
  const int nEtaEle=2;
  const int nRunEle=7;
  TString nom_run_ele[nRunEle]={"A","B","C","D","MCold","ABCD","MCnew"};
  TString nom_eta_ele[nEtaEle]={"EB","EE"};
  TF1 *turnOnEle[nEtaEle][nRunEle];

  for(int iR=0 ; iR<nRunEle ; iR++) {
    for(int iE=0 ; iE<nEtaEle ; iE++) {
      turnOnEle[iE][iR] = 
	(TF1*)corrections.Get("turnOnEle_"+nom_run_ele[iR]+"_"+nom_eta_ele[iE]);
      if(!turnOnEle[iE][iR]) cout << "Missing turnOnEle_"+nom_run_ele[iR]+"_"+nom_eta_ele[iE] << endl;
    }
  }
  
  // Ele Id+Iso
  const int nEtaEleI=2; // EB / EE (1.479)
  const int nRunEleI=6; // 
  const int nIdIso=2;   // id / iso
  TString nom_run_eleI[nRunEleI]={"ABCD","MC-ABCD","ABC","MC-ABC","D","MC-D"};
  TString nom_eta_eleI[nEtaEleI]={"EB","EE"};
  TString nom_idiso_ele[nIdIso]={"id","iso"};
  TF1 *turnOnEleIdIso[nEtaEle][nRunEle][nIdIso];

  for(int iR=0 ; iR<nRunEleI ; iR++) {
    for(int iE=0 ; iE<nEtaEleI ; iE++) {
      for(int idiso=0 ; idiso<nIdIso ; idiso++) {
	turnOnEleIdIso[iE][iR][idiso] = 
	  (TF1*)corrections.Get("turnOnEle_"+nom_run_eleI[iR]+"_"+nom_eta_eleI[iE]+"_"+nom_idiso_ele[idiso]);
	if(!turnOnEleIdIso[iE][iR][idiso]) 
	  cout << "Missing turnOnEle_"+nom_run_eleI[iR]+"_"+nom_eta_eleI[iE]+"_"+nom_idiso_ele[idiso] << endl;
      }
    }
  }

  // Tau trigger (mutau)
  const int nEtaTauT=2; // EB / EE
  const int nRunTauT=6; // ABC, MC-ABC, D, MC-D, ABCD, MC-ABCD
  TString nom_run_tau[nRunTauT]={"ABC","MC-ABC","D","MC-D","ABCD","MC-ABCD"};
  TString nom_eta_tau[nEtaTauT]={"EB","EE"};

  TF1 *turnOnTau[nEtaTauT][nRunTauT];

  for(int iR=0 ; iR<nRunTauT ; iR++) {
    for(int iE=0 ; iE<nEtaTauT ; iE++) {
      turnOnTau[iE][iR] = (TF1*)corrections.Get("turnOnTau_etau_"+nom_run_tau[iR]+"_"+nom_eta_tau[iE]);
      if(!turnOnTau[iE][iR]) cout << "Missing turnOnTau_etau_"+nom_run_tau[iR]+"_"+nom_eta_tau[iE] << endl;
    }
  }

  //////////////////////////////////////////////////////////
 
  //TFile embeddingUnfoldingLead("../../Utilities/data/unfolding/Unfold2D_1.root");
  //TFile embeddingUnfoldingSubLead("../../Utilities/data/unfolding/Unfold2D_2.root");
  //TH2F* UnfoldDen1 = embeddingUnfoldingLead.IsZombie()    ? 0 : (TH2F*)embeddingUnfoldingLead.Get("UnfoldDen1");
  //TH2F* UnfoldDen2 = embeddingUnfoldingSubLead.IsZombie() ? 0 : (TH2F*)embeddingUnfoldingSubLead.Get("UnfoldDen2");

  TFile embeddingUnfolding("../../Utilities/dataForSkimmer/unfolding/Unfold2DEta.root");
  TH2F* UnfoldDen1 = embeddingUnfolding.IsZombie()    ? 0 : (TH2F*)embeddingUnfolding.Get("UnfoldDen1");
  if(UnfoldDen1) cout << "Unfolding for embedded sample open!!" << endl;


  //////////////////////////////////////////////////////////

  cout << "Using fake-rate method from fakeRate.root" << endl;
  TFile fakeRate_DYJets ("../../Utilities/dataForSkimmer/fakeRate/fakeRate_DYJetsToTauTau_rebinned.root","READ");
  TFile fakeRate_Run2011("../../Utilities/dataForSkimmer/fakeRate/fakeRate_Run2011AB_rebinned.root","READ");
  TFile fakeRate_WJets  ("../../Utilities/dataForSkimmer/fakeRate/fakeRate_WJetsToMuNu_rebinned.root","READ");
  
  
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
  int nJets20BTagged, nJets20BTaggedBUp, nJets20BTaggedBDown, nJets20BTaggedLUp, nJets20BTaggedLDown, nJets20BTaggedLoose;
  float chFracPV1, chFracPV2;

  // kinematical variables of the veto jet
  float ptVeto, etaVeto, phiVeto;
  int isVetoInJets; float chFracPVVeto;

  // diTau related variables
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_;
  float diTauNSVfitPt_,diTauNSVfitPtErrUp_,diTauNSVfitPtErrDown_;
  float diTauSVFitMass, diTauSVFitMassCal0, diTauSVFitMassCal1, diTauSVFitMassCal2, 
    diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float diTauNSVfitMassCorr_;
  float diTauSVFitMassSA, diTauSVFitMassErrSA; 
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauVisMassCorr;
  float diTauRecoPt,diTauRecoPhi;
  float diTauMinMass;

  float etaTau1Fit, etaTau2Fit, phiTau1Fit, phiTau2Fit, ptTau1Fit, ptTau2Fit;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2,dPhiL1J1,dPhiL1J2,dPhiL2J1,dPhiL2J2,dxy1_, dz1_, scEtaL1;
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
  int tightestHPSWP_,tightestHPSDBWP_,tightestHPSDB3HWP_,tightestHPSMVAWP_,tightestHPSMVA2WP_, tightestAntiMuWP_, tightestAntiMu2WP_,decayMode_;//IN
  int tightestAntiEMVAWP_, tightestAntiECutWP_,tightestAntiEMVA3WP_,AntiEMVA3category_;//IN
  float hpsMVA_,hpsMVA2_,hpsDB3H_,AntiEMVA3raw_;//IN
  float pfJetPt_;
  float L1etm_, L1etmPhi_, L1etmCorr_, L1etmWeight_; // ND
  float caloMEtNoHFUncorr_, caloMEtNoHFUncorrPhi_, caloMEtNoHF_, caloMEtNoHFPhi_, caloMEtNoHFUp_, caloMEtNoHFUpPhi_, caloMEtNoHFDown_, caloMEtNoHFDownPhi_; // ND
  float sumEt_, caloNoHFsumEt_, caloNoHFsumEtCorr_; // ND


  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;
  float fakeRateRun2011, fakeRateWMC, effDYMC, CDFWeight;
  float visGenTauMass, genTauPt, genTauEta, genVMass;
  int genDecayMode_;
  float leadPFChHadTrackPt_, leadPFChHadTrackEta_,leadPFChHadPt_, leadPFChHadEta_;
  float leadPFCandPt_, leadPFCandEta_;

  // electron related variables
  int tightestCutBasedWP_, tightestMVAWP_;
  int tightestMVAPOGNonTrigWP_;
  float mvaPOGTrig_, mvaPOGNonTrig_, mitMVA_;
  int isTriggerElectron_;

  float sihih_, dEta_, dPhi_, HoE_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, puWeightHCP, puWeightD, puWeight2, puWeight3D, embeddingWeight_,HqTWeight,ZeeWeight,ZeeWeightHCP,weightHepNup,weightHepNupDY;
  float nHits;
  int numOfLooseIsoDiTaus_;
  int nPUVertices_;

  // object-related weights and triggers
  float HLTx, HLTxQCD, HLTxSoft, HLTxQCDSoft, HLTxIsoEle13Tau20, HLTxEle8;
  float HLTmatch, HLTmatchQCD, HLTmatchSoft, HLTmatchQCDSoft, HLTmatchIsoEle13Tau20, HLTmatchEle8;

  float HLTEleA, HLTEleB, HLTEleC, HLTEleD, HLTEleMCold, HLTEleABCD, HLTEleMCnew, HLTEleABC, HLTElec;
  float HLTweightElec, HLTweightEleA, HLTweightEleB, HLTweightEleC, HLTweightEleD, HLTweightEleABCD, HLTweightEleABC;
  float SFElecID, SFEleID_ABC,  SFEleID_D,  SFEleID_ABCD,  SFEleID_MC_ABC,  SFEleID_MC_D;
  float SFElecIso, SFEleIso_ABC,  SFEleIso_D,  SFEleIso_ABCD,  SFEleIso_MC_ABC,  SFEleIso_MC_D;
  float SFElec, SFEle_ABC,  SFEle_D,  SFEle_ABCD,  SFEle_MC_ABC,  SFEle_MC_D;
  float EffElecID, EffEleID_ABC,  EffEleID_D,  EffEleID_ABCD,  EffEleID_ABCD_MC,  EffEleID_ABC_MC,  EffEleID_D_MC;
  float EffElecIso, EffEleIso_ABC,  EffEleIso_D,  EffEleIso_ABCD,  EffEleIso_ABCD_MC,  EffEleIso_ABC_MC,  EffEleIso_D_MC;
  //float EffElec, EffEle_ABC,  EffEle_D,  EffEle_ABCD,  EffEle_MC_ABC,  EffEle_MC_D;

  float HLTTau, HLTTauD, HLTTauABC;
  float HLTTauMC, HLTTauMCD, HLTTauMCABC;
  float HLTweightTau, HLTweightTauD, HLTweightTauABC;
  float SFTau, SFEtoTau;

  int isTauLegMatched_,isElecLegMatched_,elecFlag_,genDecay_, leptFakeTau;
  int vetoEvent_;
  
  //Parton Info for W+NJet
  int parton_, genPartMult_, leadGenPartPdg_, hepNUP_;
  float leadGenPartPt_;
  
  // event id
  ULong64_t event_,run_,lumi_;
  int index_;
  int pairIndexMoriond_,pairIndexAntiETightMVA3_,pairIndexAntiEVTightMVA3_,pairIndexHPSDB3H_,pairIndexHPSMVA2_,pairIndexAntiMu2_,pairIndexSpring13Tight_,pairIndexSpring13VTight_,  pairIndexSoft_;

  float uParl, uPerp, metParl, metPerp, metSigmaParl, metSigmaPerp;

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

  outTreePtOrd->Branch("nHits",       &nHits, "nHits/F");

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
  outTreePtOrd->Branch("nJets20BTaggedLoose",&nJets20BTaggedLoose,  "nJets20BTaggedLoose/I");  
  outTreePtOrd->Branch("nJets20BTaggedBUp",&nJets20BTaggedBUp,  "nJets20BTaggedBUp/I");
  outTreePtOrd->Branch("nJets20BTaggedBDown",&nJets20BTaggedBDown,  "nJets20BTaggedBDown/I");
  outTreePtOrd->Branch("nJets20BTaggedLUp",&nJets20BTaggedLUp,  "nJets20BTaggedLUp/I");
  outTreePtOrd->Branch("nJets20BTaggedLDown",&nJets20BTaggedLDown,  "nJets20BTaggedLDown/I");

  outTreePtOrd->Branch("ptVeto",  &ptVeto, "ptVeto/F");
  outTreePtOrd->Branch("phiVeto", &phiVeto,"phiVeto/F");
  outTreePtOrd->Branch("etaVeto", &etaVeto,"etaVeto/F");
  outTreePtOrd->Branch("isVetoInJets", &isVetoInJets,"isVetoInJets/I");
  outTreePtOrd->Branch("chFracPVVeto", &chFracPVVeto,"chFracPVVeto/F"); 

  outTreePtOrd->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,       "diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,  "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  outTreePtOrd->Branch("diTauNSVfitPt",       &diTauNSVfitPt_,       "diTauNSVfitPt/F");
  outTreePtOrd->Branch("diTauNSVfitPtErrUp",  &diTauNSVfitPtErrUp_,  "diTauNSVfitPtErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitPtErrDown",&diTauNSVfitPtErrDown_,"diTauNSVfitPtErrDown/F");

  outTreePtOrd->Branch("diTauNSVfitMassCorr",   &diTauNSVfitMassCorr_,   "diTauNSVfitMassCorr/F");

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
  outTreePtOrd->Branch("diTauVisMassCorr",&diTauVisMassCorr,"diTauVisMassCorr/F");

  outTreePtOrd->Branch("diTauMinMass",&diTauMinMass,"diTauMisMass/F");

  outTreePtOrd->Branch("etaL1",   &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2",   &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",    &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",    &ptL2,"ptL2/F");
  outTreePtOrd->Branch("phiL1",   &phiL1,"phiL1/F");
  outTreePtOrd->Branch("phiL2",   &phiL2,"phiL2/F");
  outTreePtOrd->Branch("dPhiL1L2",&dPhiL1L2,"dPhiL1L2/F");
  outTreePtOrd->Branch("dPhiL1J1",&dPhiL1J1,"dPhiL1J1/F");
  outTreePtOrd->Branch("dPhiL1J2",&dPhiL1J2,"dPhiL1J2/F");
  outTreePtOrd->Branch("dPhiL2J1",&dPhiL2J1,"dPhiL2J1/F");
  outTreePtOrd->Branch("dPhiL2J2",&dPhiL2J2,"dPhiL2J2/F");
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

  outTreePtOrd->Branch("visGenTauMass",           &visGenTauMass, "visGenTauMass/F");
  outTreePtOrd->Branch("genTauPt",                &genTauPt, "genTauPt/F");
  outTreePtOrd->Branch("genTauEta",               &genTauEta, "genTauEta/F");
  outTreePtOrd->Branch("genDecayMode",            &genDecayMode_, "genDecayMode/I");
  outTreePtOrd->Branch("genVMass",                &genVMass,     "genVMass/F");

  outTreePtOrd->Branch("leadPFChHadTrackPt",      &leadPFChHadTrackPt_,"leadPFChHadPt/F");
  outTreePtOrd->Branch("leadPFChHadTrackEta",     &leadPFChHadTrackEta_,"leadPFChHadEta/F");
  outTreePtOrd->Branch("leadPFChHadPt",           &leadPFChHadPt_,"leadPFChHadPt/F");
  outTreePtOrd->Branch("leadPFChHadEta",          &leadPFChHadEta_,"leadPFChHadEta/F");
  outTreePtOrd->Branch("leadPFCandPt",            &leadPFCandPt_,"leadPFCandPt/F");
  outTreePtOrd->Branch("leadPFCandEta",           &leadPFCandEta_,"leadPFCandEta/F");

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

  outTreePtOrd->Branch("L1etm",       &L1etm_,     "L1etm/F");//ND
  outTreePtOrd->Branch("L1etmPhi",    &L1etmPhi_,  "L1etmPhi/F");//ND
  outTreePtOrd->Branch("L1etmCorr",   &L1etmCorr_, "L1etmCorr/F");//ND
  outTreePtOrd->Branch("L1etmWeight", &L1etmWeight_,"L1etmWeight/F");//ND

  outTreePtOrd->Branch("caloMEtNoHF",         &caloMEtNoHF_,         "caloMEtNoHF/F");//ND
  outTreePtOrd->Branch("caloMEtNoHFPhi",      &caloMEtNoHFPhi_,      "caloMEtNoHFPhi/F");//ND
  outTreePtOrd->Branch("caloMEtNoHFUncorr",    &caloMEtNoHFUncorr_,    "caloMEtNoHFUncorr/F");//ND
  outTreePtOrd->Branch("caloMEtNoHFUncorrPhi", &caloMEtNoHFUncorrPhi_, "caloMEtNoHFUncorrPhi/F");//ND
  outTreePtOrd->Branch("caloMEtNoHFUp",       &caloMEtNoHFUp_,       "caloMEtNoHFUp/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFUpPhi",    &caloMEtNoHFUpPhi_,    "caloMEtNoHFUpPhi/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFDown",     &caloMEtNoHFDown_,     "caloMEtNoHFDown/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFDownPhi",  &caloMEtNoHFDownPhi_,  "caloMEtNoHFDownPhi/F");// ND

  outTreePtOrd->Branch("sumEt",             &sumEt_,             "sumEt/F");// ND
  outTreePtOrd->Branch("caloNoHFsumEt",     &caloNoHFsumEt_,     "caloNoHFsumEt/F");// ND
  outTreePtOrd->Branch("caloNoHFsumEtCorr", &caloNoHFsumEtCorr_, "caloNoHFsumEtCorr/F");// ND

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
  outTreePtOrd->Branch("tightestAntiEMVA3WP", &tightestAntiEMVA3WP_,"tightestAntiEMVA3WP/I");
  outTreePtOrd->Branch("AntiEMVA3category", &AntiEMVA3category_,"AntiEMVA3category/I");
  outTreePtOrd->Branch("AntiEMVA3raw", &AntiEMVA3raw_,"AntiEMVA3raw/F");

  outTreePtOrd->Branch("sihih",              &sihih_,"sihih/F");
  outTreePtOrd->Branch("dEta",               &dEta_,"dEta/F");
  outTreePtOrd->Branch("dPhi",               &dPhi_,"dPhi/F");
  outTreePtOrd->Branch("HoE",                &HoE_,"HoE/F");

  outTreePtOrd->Branch("tightestHPSWP",      &tightestHPSWP_,"tightestHPSWP/I");//IN
  outTreePtOrd->Branch("tightestHPSDBWP",    &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("tightestHPSDB3HWP",  &tightestHPSDB3HWP_,"tightestHPSDB3HWP/I");//IN
  outTreePtOrd->Branch("tightestHPSMVAWP",   &tightestHPSMVAWP_,"tightestHPSMVAWP/I");
  outTreePtOrd->Branch("tightestHPSMVA2WP",  &tightestHPSMVA2WP_,"tightestHPSMVA2WP/I");//IN
  outTreePtOrd->Branch("tightestAntiMuWP",   &tightestAntiMuWP_,"tightestAntiMuWP/I");//ND
  outTreePtOrd->Branch("tightestAntiMu2WP",  &tightestAntiMu2WP_,"tightestAntiMu2WP/I");//ND
  outTreePtOrd->Branch("hpsDB3H",            &hpsDB3H_,   "hpsDB3H/F");//IN
  outTreePtOrd->Branch("hpsMVA",             &hpsMVA_,    "hpsMVA/F");
  outTreePtOrd->Branch("hpsMVA2",            &hpsMVA2_,   "hpsMVA2/F");//IN
  outTreePtOrd->Branch("decayMode",          &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("puWeight2",          &puWeight2,"puWeight2/F");
  outTreePtOrd->Branch("puWeightHCP",        &puWeightHCP,"puWeightHCP/F");
  outTreePtOrd->Branch("puWeightD",          &puWeightD,"puWeightD/F");
  outTreePtOrd->Branch("puWeight3D",         &puWeight3D,"puWeight3D/F");
  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("weightHepNup",       &weightHepNup,"weightHepNup/F");
  outTreePtOrd->Branch("weightHepNupDY",     &weightHepNupDY,"weightHepNupDY/F");//IN
  outTreePtOrd->Branch("HqTWeight",          &HqTWeight,"HqTWeight/F");
  outTreePtOrd->Branch("ZeeWeight",          &ZeeWeight,"ZeeWeight/F");
  outTreePtOrd->Branch("ZeeWeightHCP",          &ZeeWeightHCP,"ZeeWeightHCP/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");
  outTreePtOrd->Branch("nPUVertices",        &nPUVertices_, "nPUVertices/I");

  // Trigger matching
  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTxQCD",      &HLTxQCD,"HLTxQCD/F"); 
  outTreePtOrd->Branch("HLTmatchQCD",  &HLTmatchQCD,"HLTmatchQCD/F");

  outTreePtOrd->Branch("HLTxSoft",     &HLTxSoft,"HLTxSoft/F"); // ND
  outTreePtOrd->Branch("HLTmatchSoft", &HLTmatchSoft,"HLTmatchSoft/F");// ND
  outTreePtOrd->Branch("HLTxQCDSoft",  &HLTxQCDSoft,"HLTxQCDSoft/F"); // ND
  outTreePtOrd->Branch("HLTmatchQCDSoft",&HLTmatchQCDSoft,"HLTmatchQCDSoft/F");// ND

  // Electron
  outTreePtOrd->Branch("HLTweightElec", &HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightEleA", &HLTweightEleA,"HLTweightEleA/F");
  outTreePtOrd->Branch("HLTweightEleB", &HLTweightEleB,"HLTweightEleB/F");
  outTreePtOrd->Branch("HLTweightEleC", &HLTweightEleC,"HLTweightEleC/F");
  outTreePtOrd->Branch("HLTweightEleD", &HLTweightEleD,"HLTweightEleD/F");
  outTreePtOrd->Branch("HLTweightEleABC", &HLTweightEleABC,"HLTweightEleABC/F");
  outTreePtOrd->Branch("HLTweightEleABCD", &HLTweightEleABCD,"HLTweightEleABCD/F");
  //
  outTreePtOrd->Branch("HLTElec", &HLTElec,"HLTElec/F");
  outTreePtOrd->Branch("HLTEleA", &HLTEleA,"HLTEleA/F");
  outTreePtOrd->Branch("HLTEleB", &HLTEleB,"HLTEleB/F");
  outTreePtOrd->Branch("HLTEleC", &HLTEleC,"HLTEleC/F");
  outTreePtOrd->Branch("HLTEleD", &HLTEleD,"HLTEleD/F");
  outTreePtOrd->Branch("HLTEleABC", &HLTEleABC,"HLTEleABC/F");
  outTreePtOrd->Branch("HLTEleABCD", &HLTEleABCD,"HLTEleABCD/F");
  //
  outTreePtOrd->Branch("SFEtoTau",     &SFEtoTau,"SFEtoTau/F");
  outTreePtOrd->Branch("SFElec",       &SFElec,"SFElec/F");
  outTreePtOrd->Branch("SFElecID",     &SFElecID,"SFElecID/F");
  outTreePtOrd->Branch("SFElecIso",    &SFElecIso,"SFElecIso/F");
  outTreePtOrd->Branch("SFEle_ABC",    &SFEle_ABC, "SFEle_ABC/F");
  outTreePtOrd->Branch("SFEle_D",      &SFEle_D,   "SFEle_D/F");
  outTreePtOrd->Branch("SFEle_ABCD",   &SFEle_ABCD,"SFEle_ABCD/F");
  outTreePtOrd->Branch("SFEleID_ABC",  &SFEleID_ABC, "SFEleID_ABC/F");
  outTreePtOrd->Branch("SFEleID_D",    &SFEleID_D,   "SFEleID_D/F");
  outTreePtOrd->Branch("SFEleID_ABCD", &SFEleID_ABCD,"SFEleID_ABCD/F");
  outTreePtOrd->Branch("SFEleIso_ABC", &SFEleIso_ABC, "SFEleIso_ABC/F");
  outTreePtOrd->Branch("SFEleIso_D",   &SFEleIso_D,   "SFEleIso_D/F");
  outTreePtOrd->Branch("SFEleIso_ABCD",&SFEleIso_ABCD,"SFEleIso_ABCD/F");

  // Tau
  outTreePtOrd->Branch("HLTweightTau",   &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTweightTauD",  &HLTweightTauD,"HLTweightTauD/F");
  outTreePtOrd->Branch("HLTweightTauABC",&HLTweightTauABC,"HLTweightTauABC/F");
  outTreePtOrd->Branch("HLTTau",         &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("HLTTauD",        &HLTTauD,"HLTTauD/F");
  outTreePtOrd->Branch("HLTTauABC",      &HLTTauABC,"HLTTauABC/F");
  outTreePtOrd->Branch("HLTTauMC",       &HLTTauMC,"HLTTauMC/F");
  outTreePtOrd->Branch("HLTTauMCD",      &HLTTauMCD,"HLTTauMCD/F");
  outTreePtOrd->Branch("HLTTauMCABC",    &HLTTauMCABC,"HLTTauMCABC/F");
  //
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("SFEtoTau",     &SFEtoTau,"SFEtoTau/F");

  //
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("isElecLegMatched",&isElecLegMatched_,"isElecLegMatched/I");
  outTreePtOrd->Branch("elecFlag",        &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");
  outTreePtOrd->Branch("vetoEvent",       &vetoEvent_, "vetoEvent/I");

  outTreePtOrd->Branch("parton", &parton_,"parton/I");
  outTreePtOrd->Branch("genPartMult", &genPartMult_,"genPartMult/I");
  outTreePtOrd->Branch("leadGenPartPdg", &leadGenPartPdg_,"leadGenPartPdg/I");
  outTreePtOrd->Branch("hepNUP", &hepNUP_,"hepNUP/I");
  outTreePtOrd->Branch("leadGenPartPt", &leadGenPartPt_,"leadGenPartPt/F");
  
  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
  outTreePtOrd->Branch("index", &index_, "index/I");
  outTreePtOrd->Branch("pairIndexMoriond", &pairIndexMoriond_, "pairIndexMoriond/I");
  outTreePtOrd->Branch("pairIndexAntiETightMVA3", &pairIndexAntiETightMVA3_, "pairIndexAntiETightMVA3/I");
  outTreePtOrd->Branch("pairIndexAntiEVTightMVA3", &pairIndexAntiEVTightMVA3_, "pairIndexAntiEVTightMVA3/I");
  outTreePtOrd->Branch("pairIndexHPSDB3H", &pairIndexHPSDB3H_, "pairIndexHPSDB3H/I");
  outTreePtOrd->Branch("pairIndexHPSMVA2", &pairIndexHPSMVA2_, "pairIndexHPSMVA2/I");
  outTreePtOrd->Branch("pairIndexAntiMu2", &pairIndexAntiMu2_, "pairIndexAntiMu2/I");
  outTreePtOrd->Branch("pairIndexSpring13Tight", &pairIndexSpring13Tight_, "pairIndexSpring13Tight/I");
  outTreePtOrd->Branch("pairIndexSpring13VTight", &pairIndexSpring13VTight_, "pairIndexSpring13VTight/I");
  outTreePtOrd->Branch("pairIndexSoft", &pairIndexSoft_, "pairIndexSoft/I");

  outTreePtOrd->Branch("uParl", &uParl, "uParl/F");
  outTreePtOrd->Branch("uPerp", &uPerp, "uPerp/F");
  outTreePtOrd->Branch("metParl", &metParl, "metParl/F");
  outTreePtOrd->Branch("metPerp", &metPerp, "metPerp/F");
  outTreePtOrd->Branch("metSigmaParl", &metSigmaParl, "metSigmaParl/F");
  outTreePtOrd->Branch("metSigmaPerp", &metSigmaPerp, "metSigmaPerp/F");

  int nEntries    = currentTree->GetEntries() ;
  float crossSection = xsec_;
  float scaleFactor = (crossSection != 0) ? Lumi / (  float(nEventsRead)/(crossSection*skimEff_) )  : 1.0;
  
  TString sample(sample_.c_str());
  cout << "Processing sample " << sample << endl;
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
  currentTree->SetBranchStatus("bQuark"                ,1);

  // diTaus
  currentTree->SetBranchStatus("diTauVisP4"            ,1);
  currentTree->SetBranchStatus("diTauCAP4"             ,0);
  currentTree->SetBranchStatus("diTauICAP4"            ,1); 
  currentTree->SetBranchStatus("diTauSVfitP4"          ,1);
  currentTree->SetBranchStatus("diTauNSVfitMass"       ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp"  ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);
  currentTree->SetBranchStatus("diTauNSVfitPt"       ,1);
  currentTree->SetBranchStatus("diTauNSVfitPtErrUp"  ,1);
  currentTree->SetBranchStatus("diTauNSVfitPtErrDown",1);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("diTauCharge"           ,1);

  // taus
  currentTree->SetBranchStatus("diTauLegsP4"           ,1);
  currentTree->SetBranchStatus("genDiTauLegsP4"        ,1); // ND
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
  currentTree->SetBranchStatus("tightestHPSWP"         ,1);
  currentTree->SetBranchStatus("tightestHPSDBWP"       ,1);
  currentTree->SetBranchStatus("tightestHPSDB3HWP"     ,1);
  currentTree->SetBranchStatus("tightestHPSMVAWP"      ,1);
  currentTree->SetBranchStatus("tightestHPSMVA2WP"     ,1);
  currentTree->SetBranchStatus("hpsDB3H"               ,1);
  currentTree->SetBranchStatus("hpsMVA"                ,1);
  currentTree->SetBranchStatus("hpsMVA2"                ,1);
  currentTree->SetBranchStatus("tightestAntiEWP"       ,1);
  currentTree->SetBranchStatus("tightestAntiEMVAWP"    ,1);
  currentTree->SetBranchStatus("tightestAntiEMVA3WP"   ,1);
  currentTree->SetBranchStatus("AntiEMVA3category"     ,1);
  currentTree->SetBranchStatus("AntiEMVA3raw"          ,1);
  currentTree->SetBranchStatus("tightestAntiMuWP",      1);   // ND
  currentTree->SetBranchStatus("tightestAntiMu2WP",     1);   // ND

  currentTree->SetBranchStatus("visibleTauMass"        ,1);
  currentTree->SetBranchStatus("visibleGenTauMass"     ,1); //ND
  currentTree->SetBranchStatus("genDecayMode"          ,1); //ND
  currentTree->SetBranchStatus("isTauLegMatched"       ,1);
  currentTree->SetBranchStatus("isElecLegMatched"      ,1);
  currentTree->SetBranchStatus("hasKft"                ,0);
  currentTree->SetBranchStatus("leadPFCandPt"          ,1);
  currentTree->SetBranchStatus("leadPFCandP"           ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrPt"   ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrP"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackPt"    ,1);
  currentTree->SetBranchStatus("leadPFChargedHadrTrackP"     ,1);
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
  currentTree->SetBranchStatus("nHits"                 ,1);
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
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,0);
  currentTree->SetBranchStatus("metSgnMatrix"          ,1);

  // Extra variables for soft analysis
  currentTree->SetBranchStatus("caloMETNoHFP4"         ,1);// ND
  currentTree->SetBranchStatus("l1ETMP4"               ,1);// ND
  currentTree->SetBranchStatus("sumEt"                 ,1);// ND
  currentTree->SetBranchStatus("caloNoHFsumEt"         ,1);// ND
  currentTree->SetBranchStatus("caloNoHFsumEtCorr"     ,1);// ND

  // generator-level boson
  currentTree->SetBranchStatus("genVP4"                ,1);
  currentTree->SetBranchStatus("genDecay"              ,1);

  // extra-electrons
  currentTree->SetBranchStatus("extraElectrons"        ,0);
  currentTree->SetBranchStatus("pfElectrons"           ,0);
  currentTree->SetBranchStatus("elecFlag"              ,1);
  currentTree->SetBranchStatus("vetoEvent"             ,1);
  
  // leptons for veto
  if(doLepVeto) {
    currentTree->SetBranchStatus("vetoMuonsP4"        ,1);
    currentTree->SetBranchStatus("vetoElectronsP4"    ,1);
  // currentTree->SetBranchStatus("vetoTausP4"         ,1);
  }

  // event-dependent variables
  currentTree->SetBranchStatus("rhoFastJet"            ,1);
  currentTree->SetBranchStatus("rhoNeutralFastJet"     ,0);
  currentTree->SetBranchStatus("numPV"                 ,1);
  currentTree->SetBranchStatus("nPUVertices"           ,1);
  currentTree->SetBranchStatus("nPUVerticesP1"         ,1);
  currentTree->SetBranchStatus("nPUVerticesM1"         ,1);
  currentTree->SetBranchStatus("numOfDiTaus"           ,0);
  currentTree->SetBranchStatus("numOfLooseIsoDiTaus"   ,1);

  //variables for W+Njet re-weighting
  currentTree->SetBranchStatus("parton"                ,1);
  currentTree->SetBranchStatus("genPartMult"           ,1); 
  currentTree->SetBranchStatus("leadGenPartPdg"        ,1); 
  currentTree->SetBranchStatus("hepNUP"                ,1); 
  currentTree->SetBranchStatus("leadGenPartPt"         ,1); 
  
  currentTree->SetBranchStatus("run"                   ,1);
  currentTree->SetBranchStatus("event"                 ,1);
  currentTree->SetBranchStatus("lumi"                  ,1);
  currentTree->SetBranchStatus("mcPUweight"            ,1);
  currentTree->SetBranchStatus("embeddingWeight"       ,1);
  currentTree->SetBranchStatus("index"                 ,1);

  // triggers
  currentTree->SetBranchStatus("tauXTriggers"          ,1);
  currentTree->SetBranchStatus("triggerBits"           ,1);
  currentTree->SetBranchStatus("trgTaus"               ,1);//MB
  currentTree->SetBranchStatus("trgTauId"              ,1);//MB

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

  std::vector< LV >* l1ETMP4        = new std::vector< LV >();// ND
  currentTree->SetBranchAddress("l1ETMP4",         &l1ETMP4); // ND

  std::vector< LV >* caloMETNoHFP4  = new std::vector< LV >();
  currentTree->SetBranchAddress("caloMETNoHFP4", &caloMETNoHFP4);

  std::vector< LV >* genMETP4       = new std::vector< LV >();
  currentTree->SetBranchAddress("genMETP4",        &genMETP4);

  std::vector< int >* tauXTriggers  = new std::vector< int >();
  currentTree->SetBranchAddress("tauXTriggers",    &tauXTriggers);
  std::vector< int >* triggerBits   = new std::vector< int >();
  currentTree->SetBranchAddress("triggerBits",     &triggerBits);

  std::vector< LV >* trgTaus        = new std::vector< LV >();//MB
  currentTree->SetBranchAddress("trgTaus",         &trgTaus);//MB
  std::vector< int >* trgTauId   = new std::vector< int >();//MB
  currentTree->SetBranchAddress("trgTauId",        &trgTauId);//MB

  std::vector< double >* jetsBtagHE = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHE",      &jetsBtagHE);
  std::vector< double >* jetsBtagHP = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagHP",      &jetsBtagHP);
  std::vector< double >* jetsBtagCSV = new std::vector< double >();
  currentTree->SetBranchAddress("jetsBtagCSV",     &jetsBtagCSV);

  std::vector< float >* jetsChNfraction =  new std::vector< float >();
  currentTree->SetBranchAddress("jetsChNfraction", &jetsChNfraction);

  std::vector< double >* bQuark = new std::vector< double >(); 
  currentTree->SetBranchAddress("bQuark",      &bQuark);

  std::vector< float >* gammadEta =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadEta", &gammadEta);

  std::vector< float >* gammadPhi =  new std::vector< float >();
  currentTree->SetBranchAddress("gammadPhi", &gammadPhi);

  std::vector< float >* gammaPt =  new std::vector< float >();
  currentTree->SetBranchAddress("gammaPt", &gammaPt);

  std::vector< float >* metSgnMatrix = new std::vector< float >();
  currentTree->SetBranchAddress("metSgnMatrix", &metSgnMatrix);

  // leptons for veto
  std::vector< LV >* vetoElectronsP4 = new std::vector< LV >();
  std::vector< LV >* vetoMuonsP4 = new std::vector< LV >();
  if(doLepVeto) {
    currentTree->SetBranchAddress("vetoElectronsP4", &vetoElectronsP4);
    currentTree->SetBranchAddress("vetoMuonsP4",     &vetoMuonsP4); 
    //std::vector< LV >* vetoTausP4 = new std::vector< LV >();
  }

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,
    diTauNSVfitPt,diTauNSVfitPtErrUp,diTauNSVfitPtErrDown,mTauTauMin;
  float diTauCharge;
  int tightestHPSDBWP,tightestHPSWP,tightestHPSDB3HWP,tightestHPSMVAWP,tightestHPSMVA2WP, decayMode, genDecayMode;//IN
  int tightestAntiEMVAWP, tightestAntiECutWP,tightestAntiEMVA3WP,AntiEMVA3category;//IN
  float hpsDB3H,hpsMVA,hpsMVA2,AntiEMVA3raw;//IN
  int tightestCutBasedWP, tightestMVAWP;
  int tightestMVAPOGNonTrigWP;
  float mvaPOGTrig, mvaPOGNonTrig, mitMVA;
  int isTriggerElectron;
  float sihih, dEta, dPhi, HoE;
  float numPV;
  float mcPUweight,embeddingWeight;
  int numOfLooseIsoDiTaus;
  int isTauLegMatched,isElecLegMatched,elecFlag,genDecay, vetoEvent;
  float nPUVertices, nPUVerticesM1,nPUVerticesP1;
  float leadPFChargedHadrCandTrackPt;
  float leadPFChargedHadrCandTrackP;
  float leadPFChargedHadrCandPt;
  float leadPFChargedHadrCandP;
  float leadPFCandPt;
  float leadPFCandP;
  float leadPFChargedHadrMva;
  float pfJetPt;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass, visibleGenTauMass;
  float dxy1, dxy2, dz1, scEta1;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1;
  float allChIsoLeg1;
  float isoLeg1MVA;
  int parton, genPartMult, leadGenPartPdg, hepNUP; 
  float leadGenPartPt;
  ULong64_t event,run,lumi;
  int index;
  int tightestAntiMuWP, tightestAntiMu2WP; //ND
  float sumEt, caloNoHFsumEt, caloNoHFsumEtCorr; // ND

  // additional variables for soft analysis ND
  currentTree->SetBranchAddress("sumEt",            &sumEt);
  currentTree->SetBranchAddress("caloNoHFsumEt",    &caloNoHFsumEt);
  currentTree->SetBranchAddress("caloNoHFsumEtCorr",&caloNoHFsumEtCorr);

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
  currentTree->SetBranchAddress("tightestHPSWP",        &tightestHPSWP);
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("tightestHPSDB3HWP",    &tightestHPSDB3HWP);
  currentTree->SetBranchAddress("tightestHPSMVAWP",     &tightestHPSMVAWP);
  currentTree->SetBranchAddress("tightestHPSMVA2WP",    &tightestHPSMVA2WP);
  currentTree->SetBranchAddress("hpsDB3H",              &hpsDB3H);
  currentTree->SetBranchAddress("hpsMVA",               &hpsMVA);
  currentTree->SetBranchAddress("hpsMVA2",               &hpsMVA2);
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
  currentTree->SetBranchAddress("tightestAntiEMVA3WP",  &tightestAntiEMVA3WP);
  currentTree->SetBranchAddress("AntiEMVA3category",    &AntiEMVA3category);
  currentTree->SetBranchAddress("AntiEMVA3raw",         &AntiEMVA3raw);
  currentTree->SetBranchAddress("tightestAntiMuWP",     &tightestAntiMuWP);    // ND
  currentTree->SetBranchAddress("tightestAntiMu2WP",    &tightestAntiMu2WP);   // ND

  currentTree->SetBranchAddress("dxy1",                 &dxy1);
  currentTree->SetBranchAddress("dz1",                  &dz1);
  currentTree->SetBranchAddress("dxy2",                 &dxy2);
  currentTree->SetBranchAddress("scEta1",               &scEta1);
  
  currentTree->SetBranchAddress("nHits", &nHits);

  currentTree->SetBranchAddress("sihih",                &sihih);
  currentTree->SetBranchAddress("dEta",                 &dEta);
  currentTree->SetBranchAddress("dPhi",                 &dPhi);
  currentTree->SetBranchAddress("HoE",                  &HoE);

  currentTree->SetBranchAddress("diTauNSVfitMass",       &diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);
  currentTree->SetBranchAddress("diTauNSVfitPt",       &diTauNSVfitPt);
  currentTree->SetBranchAddress("diTauNSVfitPtErrUp",  &diTauNSVfitPtErrUp);
  currentTree->SetBranchAddress("diTauNSVfitPtErrDown",&diTauNSVfitPtErrDown);
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
  currentTree->SetBranchAddress("genDecayMode",         &genDecayMode); //ND
  currentTree->SetBranchAddress("numOfLooseIsoDiTaus",  &numOfLooseIsoDiTaus);
  currentTree->SetBranchAddress("elecFlag",             &elecFlag);
  currentTree->SetBranchAddress("isTauLegMatched",      &isTauLegMatched);
  currentTree->SetBranchAddress("isElecLegMatched",     &isElecLegMatched);
  currentTree->SetBranchAddress("vetoEvent",            &vetoEvent);
  currentTree->SetBranchAddress("visibleTauMass",       &visibleTauMass);
  currentTree->SetBranchAddress("visibleGenTauMass",    &visibleGenTauMass);//ND
  currentTree->SetBranchAddress("leadPFChargedHadrTrackPt",  &leadPFChargedHadrCandTrackPt);
  currentTree->SetBranchAddress("leadPFChargedHadrTrackP",   &leadPFChargedHadrCandTrackP);
  currentTree->SetBranchAddress("leadPFChargedHadrPt",  &leadPFChargedHadrCandPt);
  currentTree->SetBranchAddress("leadPFChargedHadrP",   &leadPFChargedHadrCandP);
  currentTree->SetBranchAddress("leadPFCandPt",         &leadPFCandPt);
  currentTree->SetBranchAddress("leadPFCandP",          &leadPFCandP);
  currentTree->SetBranchAddress("leadPFChargedHadrMva" ,&leadPFChargedHadrMva);
  currentTree->SetBranchAddress("emFraction"           ,&emFraction);
  currentTree->SetBranchAddress("hasGsf"               ,&hasGsf);
  currentTree->SetBranchAddress("signalPFChargedHadrCands"    ,&signalPFChargedHadrCands);
  currentTree->SetBranchAddress("signalPFGammaCands"   ,&signalPFGammaCands);
  currentTree->SetBranchAddress("leadPFChargedHadrHcalEnergy" ,&leadPFChargedHadrHcalEnergy);
  currentTree->SetBranchAddress("leadPFChargedHadrEcalEnergy" ,&leadPFChargedHadrEcalEnergy);
  currentTree->SetBranchAddress("pfJetPt"              ,&pfJetPt);
  currentTree->SetBranchAddress("index",                &index); 
  //For W+NJet re-weighting
  currentTree->SetBranchAddress("parton",               &parton);
  currentTree->SetBranchAddress("genPartMult",          &genPartMult);
  currentTree->SetBranchAddress("leadGenPartPdg",       &leadGenPartPdg);
  currentTree->SetBranchAddress("hepNUP",               &hepNUP);
  currentTree->SetBranchAddress("leadGenPartPt",        &leadGenPartPt);
  
  RecoilCorrector* recoilCorr = 0;

  if( (sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) || 
      sample_.find("W1Jets")!=string::npos || sample_.find("W2Jets")!=string::npos || 
      sample_.find("W3Jets")!=string::npos || sample_.find("W4Jets")!=string::npos
      ) { 
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_wjets53X_20pv_njet.root"); 
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"); 
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53X_2012_njet.root"); 
  } 
  else if(sample_.find("DYJets")!=string::npos){ 
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root"); 
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root");  
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53X_2012_njet.root");  
  } 
  else if(sample_.find("HToTauTau")!=string::npos){ 
    recoilCorr = new RecoilCorrector("../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_higgs53X_20pv_njet.root"); 
    recoilCorr->addMCFile(           "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_zmm53X_2012_njet.root");   
    recoilCorr->addDataFile(         "../../Utilities/data/recoilv7/RecoilCorrector_v7/recoilfits/recoilfit_datamm53X_2012_njet.root");   
  } 

  TFile* HqT      = 0;
  int mH          = 120;
  TH1F* histo     = 0;
  if(sample_.find("GGFH")!=string::npos){
    if(sample_.find("GGFH90")!=string::npos ) mH = 90 ;
    if(sample_.find("GGFH95")!=string::npos ) mH = 95 ;
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
    HqT = new TFile(Form("../../Utilities/dataForSkimmer/HqTFeHiPro/weight_ptH_%d.root", mH));
    if(!HqT) cout << "Cannot find HqT file..." << endl;
    else{
      histo = (TH1F*)(HqT->Get(Form("powheg_weight/weight_hqt_fehipro_fit_%d",mH)));
    }
  }

  // protection against multiple pairs per event
  ULong64_t lastEvent = 0;
  ULong64_t lastRun   = 0;
  ULong64_t lastLumi  = 0;

  int counterMoriond  = 0;
  int counterAntiETightMVA3  = 0;
  int counterAntiEVTightMVA3  = 0;
  int counterHPSDB3H  = 0;
  int counterHPSMVA2  = 0;
  int counterAntiMu2  = 0;
  int counterSpring13Tight  = 0;
  int counterSpring13VTight  = 0;
  int counterSoft     = 0;
    
  // define JSON selector //
  int nJson=5;
  string jsonFile[nJson];

  string dirJson = "/data_CMS/cms/htautau/JSON/";
  jsonFile[0] = dirJson+"/Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt"; // ReReco 13Jul 
  jsonFile[1] = dirJson+"/Cert_198022-198523_8TeV_24Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 24 Aug 
  jsonFile[2] = dirJson+"/Cert_190456-203002_8TeV_PromptReco_Collisions12_JSON_v2.txt";      // PromptReco 
  jsonFile[3] = dirJson+"/Cert_190782-190949_8TeV_06Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 06Aug
  jsonFile[4] = dirJson+"/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt";         // PromptReco updated
  map<int, vector<pair<int, int> > > jsonMap[nJson] ;  
  for(int iJ=0 ; iJ<nJson ; iJ++)
    jsonMap[iJ] = readJSONFile(jsonFile[iJ]);
  bool isGoodRun=false;
  bool isDuplicated=false;
  bool isMatched=false;
  //////////////////////////

  MAPDITAU_run mapDiTau;

  for (int n = 0; n <nEntries  ; n++) {

    currentTree->GetEntry(n);
    if(n%1000==0) cout << n <<"/"<<nEntries<< endl;
    
    // APPLY JSON SELECTION //
    isGoodRun=true;
    
    if(iJson_>=0 && iJson_<=4)
      isGoodRun = AcceptEventByRunAndLumiSection(run, lumi, jsonMap[iJson_]);
    
    if(!isGoodRun) continue;

    /////////////////////////    
    ptL1     = (*diTauLegsP4)[0].Pt();
    ptL2     = (*diTauLegsP4)[1].Pt();
    etaL1    = (*diTauLegsP4)[0].Eta();
    etaL2    = (*diTauLegsP4)[1].Eta();
    //
    isDuplicated = checkEventIsDuplicated(mapDiTau, run, lumi, event, ptL1, ptL2, etaL1, etaL2);
    if(isDuplicated) continue;
    ///////////////////////

    uParl=-999.; uPerp=-999.; metParl=-999.; metPerp=-999.; metSigmaParl=-999.; metSigmaPerp=-999.;

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
    L1etm_=-99; L1etmPhi_=-99; L1etmCorr_=-99; L1etmWeight_=1;//MB
    caloMEtNoHFUncorr_=-99; caloMEtNoHFUncorrPhi_=-99;//MB 
    caloMEtNoHF_=-99;      caloMEtNoHFPhi_=-99;//MB
    caloMEtNoHFUp_=-99;      caloMEtNoHFUpPhi_=-99;// ND
    caloMEtNoHFDown_=-99;      caloMEtNoHFDownPhi_=-99;// ND
    sumEt_ = caloNoHFsumEt_ = caloNoHFsumEtCorr_ = -99; // ND

    // define the relevant jet collection
    nJets20BTagged = 0; nJets20BTaggedBUp = 0; nJets20BTaggedBDown = 0; nJets20BTaggedLoose = 0;
    nJets20BTaggedLUp = 0; nJets20BTaggedLDown = 0;
    nJets30        = 0; nJets20 = 0;
    int lead  = -99;
    int trail = -99;
    int veto  = -99;
    vector<int> indexes;
    for(unsigned int l = 0 ; l < jets->size() ; l++){
//       if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta && (*jetPUWP)[l*3]>MINJetID)
      int passJetID = getJetIDMVALoose((*jets)[l].Pt(), (*jets)[l].Eta(), (*jetPUMVA)[l]);
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta && passJetID>MINJetID)
	indexes.push_back(l);
    }
    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];

    bool isData = sample.Contains("Run201");

    for(unsigned int v = 0 ; v < indexes.size() ; v++){
      if( (*jets)[indexes[v]].Pt() > 30 ) nJets30++;
      if( (*jets)[indexes[v]].Pt() > 20 ) nJets20++;

      // b-tag loose
      if( (*jets)[indexes[v]].Pt() > 20 && TMath::Abs((*jets)[indexes[v]].Eta())<2.4 && (*jetsBtagCSV)[indexes[v]] > 0.244 )
	nJets20BTaggedLoose++ ;

      if( (*jets)[indexes[v]].Pt() > 20 && TMath::Abs((*jets)[indexes[v]].Eta())<2.4){ 
        int jetFlavour = ((*bQuark)[indexes[v]] > 0) ? 5 : 1;
	bool isBtag = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kNo, kNo, true);
        if(isBtag){
	  nJets20BTagged++;
	  if(nJets20BTagged<2){
	    ptB1  = (*jets)[indexes[v]].Pt();
	    phiB1 = (*jets)[indexes[v]].Phi();
	    etaB1 = (*jets)[indexes[v]].Eta();
	  }
	}
	bool isBtagUp = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kUp, kNo, true);
        if(isBtagUp)nJets20BTaggedBUp++;
	bool isBtagDown = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kDown, kNo, true);
        if(isBtagDown)nJets20BTaggedBDown++;
        bool isMistagUp = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kNo, kUp, true);
        if(isMistagUp)nJets20BTaggedLUp++;
        bool isMistagDown = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kNo, kDown, true);
        if(isMistagDown)nJets20BTaggedLDown++;
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

      dPhiL1J1 =  abs((*diTauLegsP4)[0].Phi()-phi1) > TMath::Pi() ? 
	-abs( (*diTauLegsP4)[0].Phi()-phi1 ) + 2*TMath::Pi()  :
	abs( (*diTauLegsP4)[0].Phi()-phi1 ) ;
      dPhiL2J1 =  abs((*diTauLegsP4)[1].Phi()-phi1) > TMath::Pi() ? 
	-abs( (*diTauLegsP4)[1].Phi()-phi1 ) + 2*TMath::Pi()  :
	abs( (*diTauLegsP4)[1].Phi()-phi1 ) ;

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
	
	dPhiL1J2 =  abs((*diTauLegsP4)[0].Phi()-phi2) > TMath::Pi() ? 
	  -abs( (*diTauLegsP4)[0].Phi()-phi2 ) + 2*TMath::Pi()  :
	  abs( (*diTauLegsP4)[0].Phi()-phi2 ) ;
	dPhiL2J2 =  abs((*diTauLegsP4)[1].Phi()-phi2) > TMath::Pi() ? 
	  -abs( (*diTauLegsP4)[1].Phi()-phi2 ) + 2*TMath::Pi()  :
	  abs( (*diTauLegsP4)[1].Phi()-phi2 ) ;

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
	//DPHI    = Dphi;
	//DITAUPT = ((*diTauVisP4)[0]+(*METP4)[1]).Pt();
	//DIJETPT = diJetPt;
	//DPHIHJ  = dPhiHjet;
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
    diTauNSVfitPt_        = diTauNSVfitPt;
    diTauNSVfitPtErrUp_   = diTauNSVfitPtErrUp;
    diTauNSVfitPtErrDown_ = diTauNSVfitPtErrDown;

    diTauNSVfitMassCorr_    = diTauNSVfitMass;

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
    diTauVisMassCorr  = (*diTauVisP4)[0].M();

    diTauRecoPt  = ((*diTauVisP4)[0]+(*METP4)[0]).Pt();
    diTauRecoPhi = ((*diTauVisP4)[0]+(*METP4)[0]).Phi();

    diTauMinMass  = mTauTauMin;
  
//     ptL1     = (*diTauLegsP4)[0].Pt();
//     ptL2     = (*diTauLegsP4)[1].Pt();
//     etaL1    = (*diTauLegsP4)[0].Eta();
//     etaL2    = (*diTauLegsP4)[1].Eta();
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
      
    // genTau Info
    if(genDiTauLegsP4->size()>1) {
      genTauPt   = (*genDiTauLegsP4)[1].Pt();
      genTauEta  = (*genDiTauLegsP4)[1].Eta();
    }
    else genTauPt = genTauEta = -99;

    visGenTauMass = visibleGenTauMass;
    genDecayMode_ = genDecayMode;
    genVMass     = (genVP4->size() > 0) ? (*genVP4)[0].M() : 0;
    
    ////////////////////////////////////////////////////////////////////

    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    double u1 = 0.; double u2 = 0.;
    double err1 = 0; double err2 = 0;


    if(genVP4->size() && recoilCorr!=0){
      if( (sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) || 
	  sample_.find("W1Jets")!=string::npos || sample_.find("W2Jets")!=string::npos || 
	  sample_.find("W3Jets")!=string::npos || sample_.find("W4Jets")!=string::npos
	  ) 
	recoilCorr->CorrectType2(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi(), u1, u2 , err1,err2, TMath::Min(nJets30,2) );
      else if((sample_.find("DYJets")!=string::npos && abs(genDecay)==(23*15)) //only Z->tautau
	      || (sample_.find("DYJets")!=string::npos && abs(genDecay)!=(23*15) && isTauLegMatched==0 && (*genDiTauLegsP4)[1].E()>0) //Zmm, m->tau
	      || sample_.find("HToTauTau")!=string::npos)
	recoilCorr->CorrectType2(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
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
    float scalarSumPtLeg1MVA  = ( *diTauLegsP4)[0].Pt() + (*METP4)[1].Pt(); // index 3->1
    float vectorSumPtLeg1MVA  = ((*diTauLegsP4)[0] +      (*METP4)[1]).Pt() ; // index 3->1

    float scalarSumPtLeg2     = ( *diTauLegsP4)[1].Pt() + (*METP4)[0].Pt();
    float vectorSumPtLeg2     = ((*diTauLegsP4)[1] +      (*METP4)[0]).Pt() ;
    float scalarSumPtLeg2Corr = ( *diTauLegsP4)[1].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtLeg2Corr = ((*diTauLegsP4)[1] +      recoilCorrecMET).Pt() ;
    float scalarSumPtLeg2MVA  = ( *diTauLegsP4)[1].Pt() + (*METP4)[1].Pt(); // index 3->1
    float vectorSumPtLeg2MVA  = ((*diTauLegsP4)[1] +      (*METP4)[1]).Pt() ; // index 3->1

    MtLeg1_       = TMath::Sqrt( scalarSumPtLeg1*scalarSumPtLeg1 - vectorSumPtLeg1*vectorSumPtLeg1 ) ;
    MtLeg1Corr_   = TMath::Sqrt( scalarSumPtLeg1Corr*scalarSumPtLeg1Corr - vectorSumPtLeg1Corr*vectorSumPtLeg1Corr ) ;
    MtLeg1MVA_    = TMath::Sqrt( scalarSumPtLeg1MVA*scalarSumPtLeg1MVA - vectorSumPtLeg1MVA*vectorSumPtLeg1MVA ) ;

    MtLeg2_       = TMath::Sqrt( scalarSumPtLeg2*scalarSumPtLeg2 - vectorSumPtLeg2*vectorSumPtLeg2 ) ;
    MtLeg2Corr_   = TMath::Sqrt( scalarSumPtLeg2Corr*scalarSumPtLeg2Corr - vectorSumPtLeg2Corr*vectorSumPtLeg2Corr ) ;
    MtLeg2MVA_    = TMath::Sqrt( scalarSumPtLeg2MVA*scalarSumPtLeg2MVA - vectorSumPtLeg2MVA*vectorSumPtLeg2MVA ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[1];
    pZetaMVA_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[1]; // index 3->1

    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[0];
    pZetaVisMVA_  = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[0]; // index 3->1

    pZetaSig_     = pZetaSig;

    MEt          = (*METP4)[0].Et();
    MEtPhi       = (*METP4)[0].Phi();
    MEtCorr      = recoilCorrecMET.Et();
    MEtCorrPhi   = recoilCorrecMET.Phi();
    MEtMVA       = (*METP4)[1].Et();
    MEtMVAPhi    = (*METP4)[1].Phi();

    sumEt_             = sumEt;
    caloNoHFsumEt_     = caloNoHFsumEt;
    caloNoHFsumEtCorr_ = caloNoHFsumEtCorr;

    if(l1ETMP4->size()>0){
      L1etm_     = (*l1ETMP4)[0].Et();//MB etMiss()
      L1etmPhi_  = (*l1ETMP4)[0].Phi();//MB
    }
    else{
      L1etm_     = -99;
      L1etmPhi_  = -99;
    }
 
    if(caloMETNoHFP4->size()>0){
      caloMEtNoHFUncorr_    = (*caloMETNoHFP4)[0].Et();
      caloMEtNoHFUncorrPhi_ = (*caloMETNoHFP4)[0].Phi();
      caloMEtNoHF_         = (*caloMETNoHFP4)[1].Et();
      caloMEtNoHFPhi_      = (*caloMETNoHFP4)[1].Phi();

      if(!isData) {
	caloMEtNoHFUp_      = (*caloMETNoHFP4)[2].Et();
	caloMEtNoHFUpPhi_   = (*caloMETNoHFP4)[2].Phi();
	caloMEtNoHFDown_    = (*caloMETNoHFP4)[3].Et();
	caloMEtNoHFDownPhi_ = (*caloMETNoHFP4)[3].Phi();
      }
      else {
	caloMEtNoHFUp_ = caloMEtNoHFUpPhi_ = caloMEtNoHFDown_ = caloMEtNoHFDownPhi_ = -99;
      }
    }
    else{
      caloMEtNoHFUncorr_=-99; caloMEtNoHFUncorrPhi_=-99;
      caloMEtNoHF_=-99;      caloMEtNoHFPhi_=-99;
      caloMEtNoHFUp_ = caloMEtNoHFUpPhi_ = caloMEtNoHFDown_ = caloMEtNoHFDownPhi_ = -99;
    }

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

    //add additional variables to test MVA Met (from Christian)
    if( !isData && genVP4->size() > 0){
      int errorFlag = 0;
      std::pair<double, double> uT = compMEtProjU((*genVP4)[0], (*METP4)[1].Px() - (*genMETP4)[0].px(), (*METP4)[1].py() - (*genMETP4)[0].py(), errorFlag, false);
      if ( !errorFlag ) {
	uParl = uT.first;
	uPerp = uT.second;
      }
      reco::Candidate::LorentzVector met_rotated = compP4inZetaFrame((*METP4)[1] - (*genMETP4)[0], (*genVP4)[0].Phi());
      metParl = met_rotated.px();
      metPerp = met_rotated.py();
      TMatrixD metCov_rotated = compCovMatrixInZetaFrame(*metsig, (*genVP4)[0].Phi());
      metSigmaParl = TMath::Sqrt(TMath::Abs(metCov_rotated(0, 0)));
      metSigmaPerp = TMath::Sqrt(TMath::Abs(metCov_rotated(1, 1)));
    }

    //NSVfitStandalone::Vector measuredMET( (*METP4)[1].Px(), (*METP4)[1].Py(), 0);
    NSVfitStandalone::Vector measuredMET( recoilCorrecMET.Px(), recoilCorrecMET.Py(), 0);

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
      //algo.integrateMarkovChain();
    }
    if(DOSVFITSTANDALONE){
      diTauSVFitMassSA    =  algo.getMass();//algo.fittedDiTauSystem().mass();
      diTauNSVfitMass_    =  algo.getMass(); //new Recoil correction with MVA MET
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
    tightestAntiEMVA3WP_= tightestAntiEMVA3WP;
    AntiEMVA3category_  = AntiEMVA3category;
    AntiEMVA3raw_       = AntiEMVA3raw;
    tightestAntiMuWP_  = tightestAntiMuWP ;
    tightestAntiMu2WP_ = tightestAntiMu2WP;

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
    leadPFChHadTrackPt_       = leadPFChargedHadrCandTrackPt;
    leadPFChHadTrackEta_      = TMath::ACosH(leadPFChargedHadrCandTrackP/leadPFChargedHadrCandTrackPt);
    leadPFChHadPt_            = leadPFChargedHadrCandPt;
    leadPFChHadEta_           = TMath::ACosH(leadPFChargedHadrCandP/leadPFChargedHadrCandPt);
    leadPFCandPt_             = leadPFCandPt;
    leadPFCandEta_            = TMath::ACosH(leadPFCandP/leadPFCandPt);
    visibleTauMass_           = visibleTauMass;
    hasGsf_                   = hasGsf; 
    signalPFGammaCands_       = signalPFGammaCands; 
    signalPFChargedHadrCands_ = signalPFChargedHadrCands;

    tightestHPSWP_      = tightestHPSWP;
    tightestHPSDBWP_    = tightestHPSDBWP;
    tightestHPSDB3HWP_  = tightestHPSDB3HWP;
    tightestHPSMVAWP_   = tightestHPSMVAWP;
    tightestHPSMVA2WP_  = tightestHPSMVA2WP;
    hpsDB3H_            = hpsDB3H;
    hpsMVA_             = hpsMVA;
    hpsMVA2_            = hpsMVA2;
    
    pfJetPt_            = pfJetPt;

    decayMode_          = decayMode;
    numPV_              = numPV;
    sampleWeight        = scaleFactor; 
    if( sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) sampleWeight        = 1;
    puWeight            = isData ? 1.0 : pileupWeight(nPUVertices);
    puWeightHCP      = isData ? 1.0 : pileupWeightHCP( nPUVertices);   
    puWeightD        = isData ? 1.0 : pileupWeightD( nPUVertices);   
    puWeight2           = isData ? 1.0 : pileupWeight2(int(nPUVertices));   
    puWeight3D          = -99;
    nPUVertices_        = nPUVertices;
    embeddingWeight_    = embeddingWeight;

    if(sample.Contains("Emb") && UnfoldDen1 && genTausP4->size()>1){
      float corrFactorEmbed = (UnfoldDen1->GetBinContent( UnfoldDen1->GetXaxis()->FindBin( (*genTausP4)[0].Eta() ) ,  UnfoldDen1->GetYaxis()->FindBin( (*genTausP4)[1].Eta() ) )); 
      embeddingWeight_ *=  corrFactorEmbed;
      //cout << "Correcting with " << corrFactorEmbed << endl;
    }

    // Reweight W+Jets
    weightHepNup=1;
    if( (sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) || 
	sample_.find("W1Jets")!=string::npos || sample_.find("W2Jets")!=string::npos || 
	sample_.find("W3Jets")!=string::npos || sample_.find("W4Jets")!=string::npos
        ) 
      weightHepNup = reweightHEPNUPWJets( hepNUP_ );

    // Reweight DY+Jets 
    weightHepNupDY=1;
    if( sample_.find("DYJets")!=string::npos  || 
	sample_.find("DY1Jets")!=string::npos || sample_.find("DY2Jets")!=string::npos || 
	sample_.find("DY3Jets")!=string::npos || sample_.find("DY4Jets")!=string::npos
        ) 
      weightHepNupDY = reweightHEPNUPDYJets( hepNUP_ );

    HqTWeight = histo!=0 ? histo->GetBinContent( histo->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
 
    numOfLooseIsoDiTaus_= numOfLooseIsoDiTaus;

    ////////////////////////////////
    // CORRECTIONS : TRIGGER & SF //
    ////////////////////////////////

    if(DEBUG) cout << "Start checking trigger information" << endl;

    if( isData && !sample.Contains("Emb") ){

      L1etmWeight_ = 1;    //no correction for data
      L1etmCorr_ = L1etm_; //no correction for data

      // HLT Paths matching

      HLTxEle8 = 1.0;

      isMatched = false;
      for(int i=0 ; i<9 ; i++)
	isMatched = isMatched || (*triggerBits)[i]; // HLT_Ele20_CaloIdVT_CaloIsoRhoT_TrkIdT_TrkIsoT_LooseIsoPFTau20_v4-6 , HLT_Ele22_eta2p1_WP90Rho_LooseIsoPFTau20_v2-7
      HLTx = isMatched ? 1.0 : 0.0 ;
      
      isMatched = false;
      for(int i=9 ; i<18 ; i++)
	isMatched = isMatched || (*triggerBits)[i]; // HLT_Ele20_CaloIdVT_TrkIdT_LooseIsoPFTau20_v4-6 , HLT_Ele22_eta2p1_WP90NoIso_LooseIsoPFTau20_v2-7
      HLTxQCD = isMatched ? 1.0 : 0.0 ;
      
      HLTxSoft    = float((*triggerBits)[18]); // HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_L1ETM36_v1
      HLTxQCDSoft = float((*triggerBits)[19]); // HLT_Ele13_eta2p1_WP90NoIso_LooseIsoPFTau20_L1ETM36_v1
      
      HLTxIsoEle13Tau20 = float((*triggerBits)[20]); // HLT_Ele13_eta2p1_WP90Rho_LooseIsoPFTau20_v1
      // no path (ele+L1etm) :-(
      
      // HLT filters matching

      isMatched = (((*tauXTriggers)[0] && (*tauXTriggers)[15])  || // hltOverlapFilterIsoEle20LooseIsoPFTau20 (Elec && Tau)
		   ((*tauXTriggers)[1] && (*tauXTriggers)[16]));   // hltOverlapFilterIsoEle20WP90LooseIsoPFTau20 (Elec && Tau)
      HLTmatch = isMatched ? 1.0 : 0.0 ;
      
      isMatched = (((*tauXTriggers)[2] && (*tauXTriggers)[17])  || // hltOverlapFilterEle20LooseIsoPFTau20 (e && t)
		   ((*tauXTriggers)[3] && (*tauXTriggers)[18]));   // hltOverlapFilterEle20WP90LooseIsoPFTau20 (e && t)
      HLTmatchQCD = isMatched ? 1.0 : 0.0 ;
      
      isMatched = (((*tauXTriggers)[8] && (*tauXTriggers)[19])); // hltOverlapFilterIsoEle13WP90LooseIsoPFTau20 (Elec && Tau)
      //isMatched &= (L1etm_>36); // MB is this x-check needed?
      HLTmatchSoft = isMatched ? 1.0 : 0.0 ;
      
      isMatched = (((*tauXTriggers)[9] && (*tauXTriggers)[20])); // hltOverlapFilterEle13WP90LooseIsoPFTau20 (Elec && Tau)
      //isMatched &= (L1etm_>36); // MB is this x-check needed?
      HLTmatchQCDSoft = isMatched ? 1.0 : 0.0 ;

      HLTmatchIsoEle13Tau20 = float((*tauXTriggers)[8] && (*tauXTriggers)[19]); // hltOverlapFilterIsoEle13WP90LooseIsoPFTau20

      SFTau         = 1.0;
      SFEtoTau      = 1.0;
      
      HLTTauABC   = 1.0;
      HLTTauMCABC = 1.0;
      HLTTauD     = 1.0;
      HLTTauMCD   = 1.0;
      HLTTau      = 1.0;
      HLTTauMC    = 1.0;
      HLTweightTau    = 1.0;
      HLTweightTauD   = 1.0;
      HLTweightTauABC = 1.0;
      HLTEleA     = 1.0;
      HLTEleB     = 1.0;
      HLTEleC     = 1.0;
      HLTEleD     = 1.0;
      HLTEleMCold = 1.0;
      HLTEleABCD  = 1.0;
      HLTEleMCnew = 1.0;
      HLTEleABC       = 1.0;
      HLTweightEleA    = 1.0;
      HLTweightEleB    = 1.0;
      HLTweightEleC    = 1.0;
      HLTweightEleD    = 1.0;
      HLTweightEleABC  = 1.0;
      HLTweightEleABCD = 1.0;
      HLTElec = 1.0;
      HLTweightElec = 1.0;
      SFEleID_ABCD  = 1.0;
      SFEleIso_ABCD = 1.0;
      SFEle_ABCD    = 1.0;
      SFEleID_ABC  = 1.0;
      SFEleIso_ABC = 1.0;
      SFEle_ABC    = 1.0;
      SFEleID_D  = 1.0;
      SFEleIso_D = 1.0;
      SFEle_D    = 1.0;
      SFElec    = 1.0;
      SFElecID  = 1.0;
      SFElecIso = 1.0;
    }
    else { // MC or embedded

      // float correctL1etm(float L1etm, float caloMEtNoHF=0, float caloMEtNoHFcorr=0, TString method="Luca")
      L1etmCorr_  = correctL1etm(L1etm_, caloMEtNoHFUncorr_, caloMEtNoHF_, "Luca1");
      L1etmWeight_= 1;            

      HLTxQCD     = 1.0;
      HLTxSoft    = 1.0;
      HLTxQCDSoft = 1.0;

      HLTmatchQCD         = 1.0;
      HLTmatchIsoEle13Tau20 = 1.0;
      
      if( !sample.Contains("Emb") ) { // Check trigger matching only for MC
	
	// HLT Paths matching
	HLTx     = float((*triggerBits)[0]); // HLT_Ele22_eta2p1_WP90Rho_LooseIsoPFTau20_v2
	HLTxEle8 = float((*triggerBits)[1]); // HLT_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v14
	// NO SoftEle+ETM trigger :-(

	// HLT filters matching	
	HLTmatch    = float((*tauXTriggers)[1] && (*tauXTriggers)[4]); // hltOverlapFilterIsoEle20WP90LooseIsoPFTau20 (e && t)
	// NO SoftEle+ETM trigger :-(
	HLTmatchEle8 = float((*tauXTriggers)[3]);

	// emulate matching to SoftEle+L1ETM+Tau
	isMatched = ( (*tauXTriggers)[2] && // HLT_Ele8 (hltEle8TightIdLooseIsoTrackIsoFilter)
		      (*tauXTriggers)[3] && // L1IsoEG
		      (*tauXTriggers)[5] ); // trgTau

	float etmCut=30; // sometimes L1Seed has actual cut at 30 GeV (even if path name is still L1ETM36)
	if(gRandom->Uniform()>(1.0-4.806/7.274) ) // luminosity ratio : different cuts were applied in ABC and D
	  etmCut=36;

	HLTmatchIsoEle13Tau20 = float(isMatched);
	HLTmatchSoft          = float(isMatched && L1etmCorr_>etmCut);
	HLTmatchQCDSoft       = float(isMatched && L1etmCorr_>etmCut);

      }
      else {
	L1etmCorr_ = L1etm_ ;
	HLTx = HLTxEle8 = HLTmatch = HLTmatchEle8 = HLTmatchSoft = HLTmatchQCDSoft = HLTmatchIsoEle13Tau20 = 1.0;
      }
      
      // Weights for both MC and embedded
      // Tau
      SFTau  = 1.0;
      SFEtoTau = 1.0;

      int iEtaTau;
      if( TMath::Abs(etaL2)<1.5 ) iEtaTau=0;
      else iEtaTau=1;

      HLTTauABC   = turnOnTau[iEtaTau][0]->Eval( ptL2 ); // ABC
      HLTTauMCABC = turnOnTau[iEtaTau][1]->Eval( ptL2 ); // MC ABC
      HLTTauD     = turnOnTau[iEtaTau][2]->Eval( ptL2 ); // D
      HLTTauMCD   = turnOnTau[iEtaTau][3]->Eval( ptL2 ); // MC D
      HLTTau      = turnOnTau[iEtaTau][4]->Eval( ptL2 ); // ABCD
      HLTTauMC    = turnOnTau[iEtaTau][5]->Eval( ptL2 ); // MC ABCD

      HLTweightTau    = HLTTauMC    != 0 ? HLTTau    / HLTTauMC    : 0;
      HLTweightTauD   = HLTTauMCD   != 0 ? HLTTauD   / HLTTauMCD   : 0;
      HLTweightTauABC = HLTTauMCABC != 0 ? HLTTauABC / HLTTauMCABC : 0;      

      // Electron
      // trigger
      //
      //choose iEta
      int iEta;
      if(abs(etaL1) < 1.479) iEta = 0;
      else                   iEta = 1;

      // compute efficiency
      HLTEleA     = turnOnEle[iEta][0]->Eval(ptL1);
      HLTEleB     = turnOnEle[iEta][1]->Eval(ptL1);
      HLTEleC     = turnOnEle[iEta][2]->Eval(ptL1);
      HLTEleD     = turnOnEle[iEta][3]->Eval(ptL1);
      HLTEleMCold = turnOnEle[iEta][4]->Eval(ptL1);
      HLTEleABCD  = turnOnEle[iEta][5]->Eval(ptL1);
      HLTEleMCnew = turnOnEle[iEta][6]->Eval(ptL1);

      HLTEleABC       = (HLTEleA*wA + HLTEleB*wB + HLTEleC*wC)/(wA+wB+wC) ;
      
      // Compute weight D/MC
      HLTweightEleA    = HLTEleMCold!=0 ?  HLTEleA / HLTEleMCold : 0;
      HLTweightEleB    = HLTEleMCold!=0 ?  HLTEleB / HLTEleMCold : 0;
      HLTweightEleC    = HLTEleMCold!=0 ?  HLTEleC / HLTEleMCold : 0;
      HLTweightEleD    = HLTEleMCnew!=0 ?  HLTEleD / HLTEleMCnew : 0;
      HLTweightEleABC  = HLTEleMCold!=0 ?  HLTEleABC / HLTEleMCold : 0;
      HLTweightEleABCD = HLTEleMCnew!=0 ?  HLTEleABCD / HLTEleMCnew : 0;

      HLTElec = HLTEleABCD; 
      HLTweightElec = HLTweightEleABCD;

      // compute ID and Iso efficiency
      EffEleID_ABCD     = turnOnEleIdIso[iEta][0][0]->Eval(ptL1);
      EffEleIso_ABCD    = turnOnEleIdIso[iEta][0][1]->Eval(ptL1);
      EffEleID_ABCD_MC  = turnOnEleIdIso[iEta][1][0]->Eval(ptL1);
      EffEleIso_ABCD_MC = turnOnEleIdIso[iEta][1][1]->Eval(ptL1);

      EffEleID_ABC     = turnOnEleIdIso[iEta][2][0]->Eval(ptL1);
      EffEleIso_ABC    = turnOnEleIdIso[iEta][2][1]->Eval(ptL1);
      EffEleID_ABC_MC  = turnOnEleIdIso[iEta][3][0]->Eval(ptL1);
      EffEleIso_ABC_MC = turnOnEleIdIso[iEta][3][1]->Eval(ptL1);

      EffEleID_D     = turnOnEleIdIso[iEta][4][0]->Eval(ptL1);
      EffEleIso_D    = turnOnEleIdIso[iEta][4][1]->Eval(ptL1);
      EffEleID_D_MC  = turnOnEleIdIso[iEta][5][0]->Eval(ptL1);
      EffEleIso_D_MC = turnOnEleIdIso[iEta][5][1]->Eval(ptL1);
      
      // compute D/MC weights
      SFEleID_ABCD  = EffEleID_ABCD_MC !=0 ? EffEleID_ABCD  / EffEleID_ABCD_MC  : 0;
      SFEleIso_ABCD = EffEleIso_ABCD_MC!=0 ? EffEleIso_ABCD / EffEleIso_ABCD_MC : 0;
      SFEle_ABCD    = SFEleID_ABCD*SFEleIso_ABCD;

      SFEleID_ABC  = EffEleID_ABC_MC !=0 ? EffEleID_ABC  / EffEleID_ABC_MC  : 0;
      SFEleIso_ABC = EffEleIso_ABC_MC!=0 ? EffEleIso_ABC / EffEleIso_ABC_MC : 0;
      SFEle_ABC    = SFEleID_ABC*SFEleIso_ABC;

      SFEleID_D  = EffEleID_D_MC !=0 ? EffEleID_D  / EffEleID_D_MC  : 0;
      SFEleIso_D = EffEleIso_D_MC!=0 ? EffEleIso_D / EffEleIso_D_MC : 0;
      SFEle_D    = SFEleID_D*SFEleIso_D;
      
      SFElec    = SFEle_ABCD;
      SFElecID  = SFEleID_ABCD;
      SFElecIso = SFEleIso_ABCD;
 

    }

    //////////////////////////////////////////////////////////////////
 
    isTauLegMatched_ = isTauLegMatched;
    isElecLegMatched_ = isElecLegMatched;

    if(!isData)
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

    // Third lepton veto
    if(DEBUG) cout << "Third lepton veto" << endl;
    int nVetoLepton = 0;
    for(size_t imu = 0; imu < vetoMuonsP4->size(); imu++){
      //if(deltaR((*diTauLegsP4)[0], (*vetoMuonsP4)[imu]) > 0.3 && 
      // deltaR((*diTauLegsP4)[1], (*vetoMuonsP4)[imu]) > 0.3 )
      if((*vetoMuonsP4)[imu].Pt()>10)
	nVetoLepton++;
    }
    for(size_t imu = 0; imu < vetoElectronsP4->size(); imu++){ 
      if(deltaR((*diTauLegsP4)[0], (*vetoElectronsP4)[imu]) > 0.3 &&  
	 //deltaR((*diTauLegsP4)[1], (*vetoElectronsP4)[imu]) > 0.3 && 
	 (*vetoElectronsP4)[imu].Pt()>10) 
        nVetoLepton++; 
    }    
    if(DEBUG) cout << "End 3rd lepton veto" << endl;

    if(leptFakeTau==1){
      //float extraSmearing = ran->Gaus( -0.373, 1.18 );
      //diTauVisMass += extraSmearing;
      SFEtoTau      = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ? ETOTAUEB : ETOTAUEE;
    }

    // Reweight for Zee
    ZeeWeight = 1;
    if( sample_.find("DYJets")!=string::npos ){

//       if(ptL2<tauLowHigh) { // Low pT Tau
// 	if(decayMode==0 && leptFakeTau){
// 	  ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?
// 	    0.85 : 
// 	    0.94;
// 	}
// 	else if (decayMode==1 && leptFakeTau){
// 	  ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?
// 	    1.52 : 
// 	    0.32;
// 	}
//       }
//       else { // High pT Tau
// 	if(decayMode==0 && leptFakeTau){
// 	  ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?
// 	    1.02 : 
// 	    0.70;
	  
// 	  // shift mass
// 	  diTauNSVfitMassCorr_ *= 1.007;
// 	  diTauVisMassCorr     *= 1.004;

// 	  // smear mass
// 	  diTauNSVfitMassCorr_ = ran->Gaus( diTauNSVfitMassCorr_ , 3.5 );
// 	  diTauVisMassCorr     = ran->Gaus( diTauVisMassCorr     , 1.9 );
// 	}
// 	else if (decayMode==1 && leptFakeTau){
// 	  ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ?
// 	    1.85 : 
// 	    0.28;

// 	  // shift mass
// 	  diTauNSVfitMassCorr_ *= 1.002;
// 	  diTauVisMassCorr     *= 1.002;

// 	  // smear mass
// 	  diTauNSVfitMassCorr_ = ran->Gaus( diTauNSVfitMassCorr_ , 3.2 );
// 	  diTauVisMassCorr     = ran->Gaus( diTauVisMassCorr     , 3.2 );
// 	}
//       }
//     }

      if(decayMode==0 && leptFakeTau){
	ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  0.85 : 
	  1.85;
      }
      else if (decayMode==1 && leptFakeTau){
	ZeeWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  1.64 : 
	  0.28;
	diTauNSVfitMass_ *= 1.015;
	diTauVisMass *= 1.015;
      }
    }

    ZeeWeightHCP = 1;
    if( sample_.find("DYJets")!=string::npos ){
      if(decayMode==0 && leptFakeTau){
	ZeeWeightHCP = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  0.82 : 
	  0.76;
      }
      else if (decayMode==1 && leptFakeTau){
	ZeeWeightHCP = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  1.65 : 
	  0.24;
      }
    }


    elecFlag_        = elecFlag;
    genDecay_        = genDecay ;
    //vetoEvent_       = vetoEvent; 
    vetoEvent_       = (nVetoLepton > 0) ? 1 : 0; //vetoEvent; 
    parton_          = parton;
    genPartMult_     = genPartMult;
    leadGenPartPdg_  = leadGenPartPdg;
    hepNUP_          = hepNUP;
    leadGenPartPt_   = leadGenPartPt;
    event_           = event;
    run_             = run;
    lumi_            = lumi;
    index_           = index;

    int pairIndexMoriond = -1; 
    int pairIndexSoft = -1;
    int pairIndexAntiETightMVA3 = -1;
    int pairIndexAntiEVTightMVA3 = -1;
    int pairIndexHPSDB3H = -1;
    int pairIndexHPSMVA2 = -1;
    int pairIndexAntiMu2 = -1; 
    int pairIndexSpring13Tight = -1;   
    int pairIndexSpring13VTight = -1;   

    bool passQualityCutsMoriond = tightestHPSMVAWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch && tightestAntiMuWP>0  &&(tightestAntiEMVAWP == 3 || tightestAntiEMVAWP > 4) && tightestAntiECutWP > 1 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) ); 
    
    bool passQualityCutsAntiETightMVA3 = tightestHPSMVAWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch && tightestAntiMuWP>0 && tightestAntiEMVA3WP > 2 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) ); 
  
    bool passQualityCutsAntiEVTightMVA3 = tightestHPSMVAWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch 
      && tightestAntiMuWP>0 && tightestAntiEMVA3WP > 3  && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479));

    bool passQualityCutsHPSDB3H = tightestHPSDB3HWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch 
      && tightestAntiMuWP>0 && (tightestAntiEMVAWP == 3 || tightestAntiEMVAWP > 4) && tightestAntiECutWP > 1 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) );

    bool passQualityCutsHPSMVA2 = tightestHPSMVA2WP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch 
      && tightestAntiMuWP>0 && (tightestAntiEMVAWP == 3 || tightestAntiEMVAWP > 4) && tightestAntiECutWP > 1 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) );

    bool passQualityCutsAntiMu2 = tightestHPSMVAWP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch 
      && tightestAntiMu2WP>0 && (tightestAntiEMVAWP == 3 || tightestAntiEMVAWP > 4) && tightestAntiECutWP > 1 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) );
    
    bool passQualityCutsSpring13Tight = tightestHPSMVA2WP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch && tightestAntiMu2WP>0 && tightestAntiEMVA3WP >2 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) );

    bool passQualityCutsSpring13VTight = tightestHPSMVA2WP>=0 && ptL1>24 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatch && tightestAntiMu2WP>0 && tightestAntiEMVA3WP >3 && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) );

    bool passQualityCutsSoft = tightestHPSMVAWP>=0 && ptL1>14 && ptL2>20 && TMath::Abs(scEtaL1)<2.1 && combRelIsoLeg1DBetav2<0.1 && HLTmatchSoft 
      && tightestAntiMuWP>0 && (tightestAntiEMVAWP == 3 || tightestAntiEMVAWP > 4) && tightestAntiECutWP > 1 
      && ((mvaPOGNonTrig>0.925 && TMath::Abs(scEtaL1)<0.8) || (mvaPOGNonTrig>0.975 && TMath::Abs(scEtaL1)>0.8 && TMath::Abs(scEtaL1)<1.479) ||  (mvaPOGNonTrig>0.985 &&  TMath::Abs(scEtaL1)>1.479) ); 
    
    if( !(run==lastRun && lumi==lastLumi && event==lastEvent) ){

      lastEvent = event;
      lastLumi  = lumi;
      lastRun   = run;
      counterMoriond   = 0;
      counterAntiETightMVA3   = 0;
      counterAntiEVTightMVA3   = 0;
      counterHPSDB3H   = 0;
      counterHPSMVA2   = 0;
      counterAntiMu2   = 0;
      counterSpring13Tight   = 0;
      counterSpring13VTight   = 0;

      if( passQualityCutsMoriond ){
	pairIndexMoriond = counterMoriond;
	counterMoriond++;
      }
      else
	pairIndexMoriond = -1;

      if( passQualityCutsAntiETightMVA3 ){
	pairIndexAntiETightMVA3 = counterAntiETightMVA3;
	counterAntiETightMVA3++;
      }
      else
	pairIndexAntiETightMVA3 = -1;

      if( passQualityCutsAntiEVTightMVA3 ){
	pairIndexAntiEVTightMVA3 = counterAntiEVTightMVA3;
	counterAntiEVTightMVA3++;
      }
      else
	pairIndexAntiEVTightMVA3 = -1;

      if( passQualityCutsHPSDB3H ){
	pairIndexHPSDB3H = counterHPSDB3H;
	counterHPSDB3H++;
      }
      else
	pairIndexHPSDB3H = -1;

      if( passQualityCutsHPSMVA2 ){
	pairIndexHPSMVA2 = counterHPSMVA2;
	counterHPSMVA2++;
      }
      else
	pairIndexHPSMVA2 = -1;

      if( passQualityCutsAntiMu2 ){
	pairIndexAntiMu2 = counterAntiMu2;
	counterAntiMu2++;
      }
      else
	pairIndexAntiMu2 = -1;

      if( passQualityCutsSpring13Tight ){
	pairIndexSpring13Tight = counterSpring13Tight;
	counterSpring13Tight++;
      }
      else
	pairIndexSpring13Tight = -1;

      if( passQualityCutsSpring13VTight ){
	pairIndexSpring13VTight = counterSpring13VTight;
	counterSpring13VTight++;
      }
      else
	pairIndexSpring13VTight = -1;

      if( passQualityCutsSoft ){
	pairIndexSoft = counterSoft;
	counterSoft++;
      }
      else
	pairIndexSoft = -1;
    }
    else{
      if( passQualityCutsMoriond ){
	pairIndexMoriond = counterMoriond;
	counterMoriond++;
      }
      else
	pairIndexMoriond = -1;

      if( passQualityCutsAntiETightMVA3 ){
	pairIndexAntiETightMVA3 = counterAntiETightMVA3;
	counterAntiETightMVA3++;
      }
      else
	pairIndexAntiETightMVA3 = -1;

      if( passQualityCutsAntiEVTightMVA3 ){
	pairIndexAntiEVTightMVA3 = counterAntiEVTightMVA3;
	counterAntiEVTightMVA3++;
      }
      else
	pairIndexAntiEVTightMVA3 = -1;

      if( passQualityCutsHPSDB3H ){
	pairIndexHPSDB3H = counterHPSDB3H;
	counterHPSDB3H++;
      }
      else
	pairIndexHPSDB3H = -1;

      if( passQualityCutsHPSMVA2 ){
	pairIndexHPSMVA2 = counterHPSMVA2;
	counterHPSMVA2++;
      }
      else
	pairIndexHPSMVA2 = -1;

      if( passQualityCutsAntiMu2 ){
	pairIndexAntiMu2 = counterAntiMu2;
	counterAntiMu2++;
      }
      else
	pairIndexAntiMu2 = -1;

      if( passQualityCutsSpring13Tight ){
	pairIndexSpring13Tight = counterSpring13Tight;
	counterSpring13Tight++;
      }
      else
	pairIndexSpring13Tight = -1;

      if( passQualityCutsSpring13VTight ){
	pairIndexSpring13VTight = counterSpring13VTight;
	counterSpring13VTight++;
      }
      else
	pairIndexSpring13VTight = -1;

      if( passQualityCutsSoft ){
	pairIndexSoft = counterSoft;
	counterSoft++;
      }
      else
	pairIndexSoft = -1;
    }

    pairIndexMoriond_     = pairIndexMoriond;
    pairIndexSoft_ = pairIndexSoft;
    pairIndexAntiETightMVA3_ = pairIndexAntiETightMVA3;
    pairIndexAntiEVTightMVA3_ = pairIndexAntiEVTightMVA3;
    pairIndexHPSDB3H_ = pairIndexHPSDB3H;
    pairIndexHPSMVA2_ = pairIndexHPSMVA2;
    pairIndexAntiMu2_ = pairIndexAntiMu2;
    pairIndexSpring13Tight_ = pairIndexSpring13Tight;
    pairIndexSpring13VTight_ = pairIndexSpring13VTight;

    outTreePtOrd->Fill();
  }
  
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

int main(int argc, const char* argv[])
{
  
  std::cout << "treeSkimmerElecTau_Spring13" << std::endl;
  gROOT->SetBatch(true);
  
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();
  

//--- parse command-line arguments
  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

//--- read python configuration parameters
  if ( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ) 
    throw cms::Exception("TreeSkimmerElecTauAnalyzer") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgTreeSkimmerElecTauAnalyzer = cfg.getParameter<edm::ParameterSet>("treeSkimmerElecTau");

  std::string sample = cfgTreeSkimmerElecTauAnalyzer.getParameter<std::string>("sample");
  std::string analysis = cfgTreeSkimmerElecTauAnalyzer.getParameter<std::string>("analysis");
  double xSection = cfgTreeSkimmerElecTauAnalyzer.getParameter<double>("xSection");
  double skimEff = cfgTreeSkimmerElecTauAnalyzer.getParameter<double>("skimEff");
  int iJson = cfgTreeSkimmerElecTauAnalyzer.getParameter<int>("iJson");

  fwlite::InputSource inputFiles(cfg); 
  int maxEvents = inputFiles.maxEvents();

  fwlite::OutputFiles outputFile(cfg);
  fwlite::TFileService fs = fwlite::TFileService(outputFile.file().data());

  string analysisFileName = analysis;
  if( !(analysis.find("Up")!=string::npos || analysis.find("Down")!=string::npos) &&  analysis.find("Raw")==string::npos)
    analysisFileName = "Nominal";
  if( !(analysis.find("Up")!=string::npos || analysis.find("Down")!=string::npos) &&  analysis.find("Raw")!=string::npos)
    analysisFileName = "RawNominal";

  cout << "Now skimming analysis " << analysis << endl;
  if(analysis=="nominal") analysis="";

  TTree* outTreePtOrd = fs.make<TTree>(TString(("outTreePtOrd"+analysis).c_str()),"tree jets pT-ord");

  double nEventsRead = 0;
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end(); ++inputFileName ) {
//--- open input file
    TFile* inputFile = TFile::Open(inputFileName->data());
    if ( !inputFile ) 
      throw cms::Exception("TreeSkimmerElecTauAnalyzer") 
	<< "Failed to open inputFile = " << (*inputFileName) << " !!\n";

    TString histoName("allEventsFilter/totalEvents");
    TH1D* histo =(TH1D*)inputFile->Get(histoName);
//     cout<<"histo "<<histo->GetEntries();
    if(histo)nEventsRead += histo->GetBinContent(1) ;
    else throw cms::Exception("TreeSkimmerElecTauAnalyzer") 
      << "Failed to read histogram "<<histoName<<" from inputFile = " << (*inputFileName) << " !!\n";
//--- close input file
    delete inputFile;
  }
  cout<< "nEventsRead " << nEventsRead << endl;

  string anlyzerName = analysis;
  if( analysis.find("Jet")!=string::npos && analysis.find("Raw")==string::npos)
    anlyzerName = "";
  if( analysis.find("Jet")!=string::npos && analysis.find("Raw")!=string::npos)
    anlyzerName = "Raw";
  TString treeName(("elecTauStreamAnalyzer"+anlyzerName+"/tree").c_str());
  TChain* currentTree = new TChain (treeName);
  bool maxEvents_processed = false;
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end() && !maxEvents_processed; ++inputFileName ) {
    currentTree->Add(inputFileName->data());
  }
  fillTrees_ElecTauStream(currentTree,outTreePtOrd,nEventsRead,analysis,sample,xSection,skimEff,iJson);

  TString dirOut_ = "/data_CMS/cms/htautau/PostMoriond/NTUPLES/EleTau/temp/";
  TTree* backgroundDYTauTau, *backgroundDYEtoTau, *backgroundDYJtoTau;
  TString outName ="";
  TFile *outFile;// = new TFile(outName,"UPDATE");

  if(sample=="DYJets") {
    backgroundDYTauTau  = outTreePtOrd->CopyTree("abs(genDecay)==(23*15)"); // g/Z -> tau+ tau-
    backgroundDYEtoTau  = outTreePtOrd->CopyTree("abs(genDecay)!=(23*15) && leptFakeTau"); // g/Z -> mu+mu- mu->tau
    backgroundDYJtoTau  = outTreePtOrd->CopyTree("abs(genDecay)!=(23*15) && !leptFakeTau"); // g/Z -> mu+mu- jet->tau

    cout << "-- copy tree" << endl;
    if(backgroundDYTauTau) { 
      outName = dirOut_+"nTupleDYJ_TauTau_"+analysis+".root" ;
      outFile = new TFile(outName,"RECREATE");
      backgroundDYTauTau->Write();
      outFile->Close();
    }
    
    if(backgroundDYEtoTau) {
      outName = dirOut_+"nTupleDYJ_EToTau_"+analysis+".root";
      outFile = new TFile(outName,"RECREATE");
      backgroundDYEtoTau->Write();
      outFile->Close();
    }
    
    if(backgroundDYJtoTau) {
      outName = dirOut_+"nTupleDYJ_JetToTau_"+analysis+".root";
      outFile = new TFile(outName,"RECREATE");
      backgroundDYJtoTau->Write();
      outFile->Close();
    }
  }


  return 0;
}
