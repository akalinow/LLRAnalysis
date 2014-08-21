// treeSkimmerMuTau
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "TTree.h"
#include "TChain.h"
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
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "TauAnalysis/CandidateTools/interface/candidateAuxFunctions.h"
#include "TauAnalysis/CandidateTools/interface/neuralMtautauAuxFunctions.h"
#include "LLRAnalysis/Utilities/interface/BtagSF.hh" // --> UPDATED FOR MORIOND
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
//MB#define MAXEta  5.0 
#define MAXEta  4.7 //MB
#define MINJetID 0.5
#define USERECOILALGO true
#define USEFAKERATE false
#define DOSVFITSTANDALONE false
#define DOVBFMVA false
#define DEBUG false
#define RERECO true
#define SELTAU true

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

edm::LumiReWeighting LumiWeights_(   "/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root",
				     "/data_CMS/cms/htautau/PostMoriond/tools/Data_Pileup_2012_ReRecoPixel-600bins.root","pileup","pileup");
edm::LumiReWeighting LumiWeightsHCP_("/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root",
				     "/data_CMS/cms/htautau/Moriond/tools/Data_Pileup_2012_HCP-600bins.root","pileup","pileup"); 
edm::LumiReWeighting LumiWeightsD_(  "/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root",
				     "/data_CMS/cms/htautau/PostMoriond/tools/Data_Pileup_2012D_22Jan2013ReReco_full-pixel-600bins-mbiasXS69400.root","pileup","pileup"); 
edm::LumiReWeighting LumiWeightsDHigh_("/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root",
				       "/data_CMS/cms/htautau/PostMoriond/tools/Data_Pileup_2012D_22Jan2013ReReco_high-pixel-600bins-mbiasXS69400.root","pileup","pileup"); 
edm::LumiReWeighting LumiWeightsDLow_( "/data_CMS/cms/htautau/Moriond/tools/MC_Summer12_PU_S10-600bins.root",
				       "/data_CMS/cms/htautau/PostMoriond/tools/Data_Pileup_2012D_22Jan2013ReReco_low-pixel-600bins-mbiasXS69400.root","pileup","pileup"); 

enum BVariation{kNo = 0, kDown = 1, kUp = 2};
BtagSF* btsf = new BtagSF(12345);

float correctL1etm(float L1etm, float caloMEtNoHFUncorr=0, float caloMEtNoHF=0) {

  if(caloMEtNoHFUncorr==0) return L1etm;

  float R = 0.9322;     // VALORE OTTENUTO CON FIT CONSTANTE USANDO 10 BIN
  float K = 0.6693;
  float H = (0.8828 - 1.0) + (0.0499*TMath::Log(caloMEtNoHFUncorr));
  return (L1etm*(caloMEtNoHF/caloMEtNoHFUncorr)*R) + H*(L1etm*(caloMEtNoHF/caloMEtNoHFUncorr)*R - K*caloMEtNoHFUncorr);  
}

double CALIB_PF_JET[6*11] = {
  1.114000,  2.297000, 5.959000, 1.181000,  0.728600,  0.367300, // eta = 0
  0.784200,  4.331000, 2.672000, 0.574300,  0.881100,  0.408500, // eta = 1
  0.961000,  2.941000, 2.400000, 1.248000,  0.666000,  0.104100, 
  0.631800,  6.600000, 3.210000, 0.855100,  0.978600,  0.291000, 
  0.345600,  8.992000, 3.165000, 0.579800,  2.146000,  0.491200, 
  0.850100,  3.892000, 2.466000, 1.236000,  0.832300,  0.180900, 
  0.902700,  2.581000, 1.453000, 1.029000,  0.676700, -0.147600, 
  1.117000,  2.382000, 1.769000, 0.000000, -1.306000, -0.474100, 
  1.634000, -1.010000, 0.718400, 1.639000,  0.672700, -0.212900, 
  0.986200,  3.138000, 4.672000, 2.362000,  1.550000, -0.715400, 
  1.245000,  1.103000, 1.919000, 0.305400,  5.745000,  0.862200, // Eta = 10
};

int calib_pf_jet(double et, unsigned int reta){
  if(et<0.01) return 0;
  //
  // corr_factor = [0]+[1]/(pow(log10(x),2)+[2])+[3]*exp(-[4]*(log10(x)-[5])*(log10(x)-[5]))
  // Et_out = Et_in * corr_factor
  //

  int abseta = reta - 11;
  if (reta < 11) abseta = 10 - reta;
    
  const double * p = &CALIB_PF_JET[6*abseta];
  double cet = et * (p[0]+p[1]/(pow(log10(et),2)+p[2])+p[3]*exp(-p[4]*(log10(et)-p[5])*(log10(et)-p[5])));
  int tet = 4 * ((int) (cet / 4.0));
  if (tet > 252) tet = 252;
  return tet;
}

/* Find rawEt of L1Jet */
double rawEtL1Jet(double et, double eta){

  if(et<0.01) return 0;

  // Digitize Et (needed?? It should be digitized by default)
  int iet = 4 * (int)(et/4.0);
  if(iet>252) iet = 252;
  //std::cout<<"et="<<et<<", iet="<<iet<<std::endl;

  // Convert physical eta into bit code
  double TO_RETA[22] = {
    -4.5,   -4.0,   -3.5,   -3.0, //Forward-
    -2.172, -1.740, -1.392, -1.044, -0.696, -0.348,  0.000, //Central-
     0.348,  0.696,  1.044,  1.392,  1.740,  2.172,  3.000, //Central+
     3.5,    4.0,    4.5,    5.0 //Forward+ 
  };
  unsigned int reta = 12345;
  for(unsigned int i=0; i<22; ++i){
    if(eta < TO_RETA[i]){
      reta=i;
      break;
    }
  }
  if(reta>21)
    reta = 21;
  //std::cout<<"eta="<<eta<<", reta="<<reta<<std::endl;

  // Find rawEt
  double rawEt = et;
  for(unsigned int ietRaw=0; ietRaw<253; ++ietRaw){
    if( iet==calib_pf_jet(ietRaw, reta) ){
      //rawEt = 4.0 * (ietRaw/4); //digitize output. In principle one can use 2GeV binning to better accumulate bin shift (4->2 in the formula)
      //rawEt = 2.0 * (ietRaw/2); //digitize output. In principle one can use 2GeV binning to better accumulate bin shift (4->2 in the formula)
      rawEt = 1.0 * (ietRaw/1); //digitize output. In principle one can use 2GeV binning to better accumulate bin shift (4->2 in the formula)
      //break;
    }
  }

  //std::cout<<"et="<<et<<", rawEt="<<rawEt<<std::endl;
  return rawEt;
  
}

LV unfoldL1JetP4(LV inP4) {

  double et     = inP4.Et();
  double eta    = inP4.Eta();
  double rawEt  = rawEtL1Jet( et , eta );
  double uncorr = et!=0 ? rawEt / et : 1.0 ;

  double Px = inP4.Px();
  double Py = inP4.Py();

  LV outP4(inP4);
  outP4.SetPx(uncorr*Px);
  outP4.SetPy(uncorr*Py);

  return outP4;
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
  //double dphi = v1.Phi() - v2.Phi();
  double dphi = TVector2::Phi_mpi_pi(v1.Phi() - v2.Phi());
  return TMath::Sqrt( TMath::Power(deta,2) + TMath::Power(dphi,2) );

}


float reweightHEPNUPWJets(int hepNUP, int set=0) {

  int nJets = hepNUP-5;
  
  //Winter13
  if(set==0) { // usual set of samples OLD
    if(nJets==0)      return 0.492871535;
    else if(nJets==1) return 0.184565169;
    else if(nJets==2) return 0.056192256;
    else if(nJets==3) return 0.03876607;
    else if(nJets>=4) return 0.018970657;
    else return 1 ;
  }
  else if(set==1) { // adding new high stat samples
    if(nJets==0)      return 0.492871535;
    else if(nJets==1) return 0.100267473;
    else if(nJets==2) return 0.031238278;
    else if(nJets==3) return 0.019961315;
    else if(nJets>=4) return 0.018980202;
    else return 1 ;
  }
  else return 1 ;

}

float reweightHEPNUPDYJets(int hepNUP) {

  int nJets = hepNUP-5;

  if(nJets==0)      return 0.115028141;
  else if(nJets==1) return 0.022330692;
  else if(nJets==2) return 0.009068275;
  else if(nJets==3) return 0.005270592;
  else if(nJets>=4) return 0.004113813;
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
  TFile* puFile_mc = new TFile("../../Utilities/data/pileUp/pileUpFall11.root", "RECREATE");
  puDist_mc->Write();
  delete puFile_mc;

}

float pileupWeight( float intimepileup , TString period=""){

  if(     period=="HCP")   return LumiWeightsHCP_.weight(intimepileup);
  else if(period=="D")     return LumiWeightsD_.weight(intimepileup);
  else if(period=="DHigh") return LumiWeightsDHigh_.weight(intimepileup);
  else if(period=="DLow")  return LumiWeightsDLow_.weight(intimepileup);
  else                     return LumiWeights_.weight(intimepileup);
}

float pileupWeight2( int intimepileup_ ){

  float weights[60];
  weights[0]=0.0754036;
  weights[1]=0.121549;
  weights[2]=0.29372;
  weights[3]=1.58628;
  weights[4]=0.209476;
  weights[5]=0.906423;
  weights[6]=3.08973;
  weights[7]=5.99051;
  weights[8]=9.31421;
  weights[9]=12.4682;
  weights[10]=12.8895;
  weights[11]=14.1044;
  weights[12]=11.731;
  weights[13]=10.5656;
  weights[14]=9.42213;
  weights[15]=7.59931;
  weights[16]=5.29512;
  weights[17]=3.40068;
  weights[18]=2.2484;
  weights[19]=1.61468;
  weights[20]=1.25055;
  weights[21]=1.02131;
  weights[22]=0.86323;
  weights[23]=0.743802;
  weights[24]=0.644286;
  weights[25]=0.554468;
  weights[26]=0.471385;
  weights[27]=0.395507;
  weights[28]=0.328025;
  weights[29]=0.26954;
  weights[30]=0.220014;
  weights[31]=0.178764;
  weights[32]=0.144826;
  weights[33]=0.117126;
  weights[34]=0.0946775;
  weights[35]=0.0765016;
  weights[36]=0.0618413;
  weights[37]=0.0500711;
  weights[38]=0.0406793;
  weights[39]=0.0332573;
  weights[40]=0.0274464;
  weights[41]=0.0229582;
  weights[42]=0.0195529;
  weights[43]=0.0169962;
  weights[44]=0.015137;
  weights[45]=0.0138406;
  weights[46]=0.0130297;
  weights[47]=0.0126419;
  weights[48]=0.0126657;
  weights[49]=0.0131136;
  weights[50]=0.0140483;
  weights[51]=0.0155733;
  weights[52]=0.0178826;
  weights[53]=0.0212908;
  weights[54]=0.0262625;
  weights[55]=0.0336204;
  weights[56]=0.0446286;
  weights[57]=0.0614835;
  weights[58]=0.0879606;
  weights[59]=0.130653;

  int intimepileup = intimepileup_>59 ? 59 : intimepileup_; 
  return weights[intimepileup];
  
}

Bool_t isbtagged(Bool_t isBQuark, Double_t btagCSV, Bool_t isdata, UInt_t btageff, UInt_t mistag)
{
  // btageff, mistag: flags, values can be  kNo = central, kUp = variation up, kDown = variation down

  // new scale factors
  // CSVM       btag eff: 0.97 \pm 0.04         mistag rate: 0.0152 \pm 0.0002          mistag scale factor: 1.10 \pm 0.11

  Bool_t btagged;
  Double_t demoteProb=0; // probability to demote from tagged
  if(btageff==kNo)        demoteProb = 1-0.97; // SF = 0.97 -> 0.03 = (prob to demote from tagged status)
  else if(btageff==kDown) demoteProb = fabs(1-0.97+0.04); // 0.07
  else if(btageff==kUp)   demoteProb = fabs(1-0.97-0.04);  // 0.01
  Double_t promoteProb=0; // probability to promote to tagged
  if(mistag==kNo)         promoteProb = fabs(1.10-1)*0.0152/(1-0.0152);  // |(1-SF)|*mistag = (prob. to promote to tagged status)*(1-mistag)
  else if(mistag==kDown)  promoteProb = fabs(1.10+0.11-1)*0.0152/(1-0.0152);
  else if(mistag==kUp)    promoteProb = fabs(1.10-0.11-1)*0.0152/(1-0.0152);
  
  if(isdata) {
    if(btagCSV>0.679) btagged = kTRUE;
    else               btagged = kFALSE;
  } else { // MC
    //jetflavor = abs(jet->matchedId);
    if(isBQuark) {
      if(btagCSV>0.679) {
        if(gRandom->Uniform()>demoteProb) btagged = kTRUE;  // leave it tagged
        else                           btagged = kFALSE; // demote it
      } else                           btagged = kFALSE; // leave it untagged
    } else { // not bjet 
      if(btagCSV>0.679)                    btagged = kTRUE;  // leave it tagged
      else if(gRandom->Uniform()<promoteProb) btagged = kTRUE;  // promote to tagged
      else                                 btagged = kFALSE; // leave it untagged
    }
  } 
  
  return btagged;
}  

float getTauFakeCorrection(double pt)
{
  //new corrections (Mar14, new T-ES correction)
  float correction = 0;
  float p0 =  0.787452;
  float p1 = -0.146412;
  float p2 = -0.0276803;
  float p3 = -0.0824184;
  float X = (pt-149.832)/100;//(x-meanPt)/100
  correction = p0+p1*X+p2*X*X+p3*X*X*X;

  //old corrections prior to Mar14 (old T-ES correction)
//   float correction = 0;
//   float p0 = 0.718127;
//   float p1 = -0.143612;
//   float p2 = -0.0431415;
//   float p3 = -0.0981383;
//   float X = (pt-163.7)/100;//(x-meanPt)/100
//   correction = p0+p1*X+p2*X*X+p3*X*X*X;

  return correction;
}

int getJetIDMVALoose(double pt, double eta, double rawMVA)
{
  float eta_bin[] = {0,2.5,2.75,3.0,5.0};
  float Pt010_Loose[]    = {-0.95,-0.96,-0.94,-0.95};
  float Pt1020_Loose[]   = {-0.95,-0.96,-0.94,-0.95};
  float Pt2030_Loose[]   = {-0.63,-0.60,-0.55,-0.45};
  float Pt3050_Loose[]   = {-0.63,-0.60,-0.55,-0.45};

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

void fillTrees_MuTauStream(TChain* currentTree,
			   TTree* outTreePtOrd,
			   double nEventsRead =0.,
			   string analysis_ = "", 
			   string sample_ = "",
			   float xsec_ = 0., 
			   float skimEff_ = 0., 
			   int iJson_=-1,
			   bool doLepVeto=true,
			   int iDiv = 0,
			   int nDiv = 1
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
  //Lumi3DReWeighting->weight3D_init(1.00);
  //cout << "Lumi3D rewieghting initialized" << endl;
  //////////////////////////////////////////////////////////
  
  ////////////////////
  //   CORRECTIONS  //
  ////////////////////

  cout << "Using corrections from llrCorrections_Summer14_v9_MVAIso.root" << endl;
  //cout << "Using corrections from llrCorrections_Winter13_v7_MVAIso.root" << endl;
  TFile corrections("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Summer14_v9_MVAIso.root");
//   TFile corrections("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Winter13_v8_MVAIso.root");
  //TFile corrections("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Winter13_v7_MVAIso.root");
  //TFile corrections("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Summer13_v6.root");
  
  // Muon trigger
  const int nEtaMuT=6;    // ]-inf,-1.2[ [-1.2,-0.8[ [-0.8,0[ [0,0.8[ [0.8,1.2[ [1.2,+inf[
  const int nRunMuT=9; // A, B, C, D, MC-old, ABCD, MC-new, SoftMuD, SoftMuMC
  TString nom_run_mu[nRunMuT]={"A","B","C","D","MCold","ABCD","MCnew","SoftD","SoftMC"};
  TString nom_eta_mu[nEtaMuT]={"0","1","2","3","4","5"};
  TF1 *turnOnMu[nEtaMuT][nRunMuT];

  for(int iR=0 ; iR<nRunMuT ; iR++) {
    for(int iE=0 ; iE<nEtaMuT ; iE++) {
      turnOnMu[iE][iR] = (TF1*)corrections.Get("turnOnMu_"+nom_run_mu[iR]+"_"+nom_eta_mu[iE]);
      if(!turnOnMu[iE][iR]) cout << "Missing turnOnMu_"+nom_run_mu[iR]+"_"+nom_eta_mu[iE] << endl;
    }
  }

  // Muon Id+Iso
  const int nEtaMuI=3; // [0,0.8[ [0.8,1.2[ [1.2,+inf[
  const int nRunMuI=6; // ABCD, MC-ABCD, ABCD, MC-ABC, D, MC-D
  const int nIdIso =2; // id, iso
  TString nom_run_muI[nRunMuI]={"ABCD","MC-ABCD","ABC","MC-ABC","D","MC-D"};
  TString nom_eta_muI[nEtaMuI]={"0","1","2"};
  TString nom_idiso_muI[nIdIso]={"id","iso"};
  TF1 *turnOnMuIdIso[nEtaMuI][nRunMuI][nIdIso];
  
  for(int iR=0 ; iR<nRunMuI ; iR++) {
    for(int iE=0 ; iE<nEtaMuI ; iE++) {
      for(int idiso=0 ; idiso<nIdIso ; idiso++) {
	turnOnMuIdIso[iE][iR][idiso] = 
	  (TF1*)corrections.Get("turnOnMuIdIso_"+nom_run_muI[iR]+"_"+nom_eta_muI[iE]+"_"+nom_idiso_muI[idiso]);
	if(!turnOnMuIdIso[iE][iR][idiso]) 
	  cout << "Missing turnOnMuIdIso_"+nom_run_muI[iR]+"_"+nom_eta_muI[iE]+"_"+nom_idiso_muI[idiso] << endl;
      }
    }
  }

  // Tau trigger (mutau)
  const int nEtaTauT=2; // EB / EE
  const int nRunTauT=6; // ABC, MC-ABC, D, MC-D, ABCD new, MC-ABCD new, ABCD old, MC-ABCD old
  TString nom_run_tau[nRunTauT]={"ABC","MC-ABC","D","MC-D","ABCD","MC-ABCD"};
  TString nom_eta_tau[nEtaTauT]={"EB","EE"};

  TF1 *turnOnTau[nEtaTauT][nRunTauT];

  for(int iR=0 ; iR<nRunTauT ; iR++) {
    for(int iE=0 ; iE<nEtaTauT ; iE++) {
      turnOnTau[iE][iR] = (TF1*)corrections.Get("turnOnTau_mutau_"+nom_run_tau[iR]+"_"+nom_eta_tau[iE]);
      if(!turnOnTau[iE][iR]) cout << "Missing turnOnTau_mutau_"+nom_run_tau[iR]+"_"+nom_eta_tau[iE] << endl;
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


  //////////////////////////////////////////////////////////

  // normalization Lumi
  Float_t Lumi=1000;

  //////////////////////////////////////////////////////////

  // kinematical variables of first 2 jets  
  float pt1,pt2,eta1,eta2,Deta,Mjj,Dphi,phi1,phi2;
  float ptAll[50],etaAll[50],phiAll[50],energyAll[50],csvAll[50];
  //float pt1_v2,pt2_v2,eta1_v2,eta2_v2,Deta_v2,Mjj_v2,Dphi_v2,phi1_v2,phi2_v2;
  float diJetPt, diJetPhi, dPhiHjet, c1, c2;
  float ptB1, etaB1, phiB1;
  float ptAllB[50],etaAllB[50],phiAllB[50],energyAllB[50],csvAllB[50];
  float ptAllBLoose[50],etaAllBLoose[50],phiAllBLoose[50],energyAllBLoose[50],csvAllBLoose[50];
  float MVAvbf;
  float jet1PUMVA, jet2PUMVA, jetVetoPUMVA;
  float jet1PUWP, jet2PUWP, jetVetoPUWP;
  int nJets30, nJets20;
  float dRb1b2,dPhib1b2,dEtab1b2,dRb1L1,dRb1L2,dRb2L1,dRb2L2,dRMaxbJetLept,dRMinbJetLept;
  float dRHbbHtt,dPhiHbbHtt,dEtaHbbHtt, diHiggsVisMass,ptHbb,visMassHbb;
  float dRHbbMETMVA,dPhiHbbMETMVA,dEtaHbbMETMVA,dRHttMETMVA,dPhiHttMETMVA,dEtaHttMETMVA;

  float jet1QGmlp, jet1QGlike, jet1QGrhoIso, jet1QGptD, jet1QGaxis1, jet1QGaxis2, jet1QGmult, jet1QGnChg, jet1QGnNeutral, jet1QGflavor, jet1QGNbConst, jet1QGConstId, jet1QGConstPt ;
  float jet2QGmlp, jet2QGlike, jet2QGrhoIso, jet2QGptD, jet2QGaxis1, jet2QGaxis2, jet2QGmult, jet2QGnChg, jet2QGnNeutral, jet2QGflavor, jet2QGNbConst, jet2QGConstId, jet2QGConstPt ;
  float jetVetoQGmlp, jetVetoQGrhoIso, jetVetoQGptD, jetVetoQGaxis1, jetVetoQGaxis2, jetVetoQGmult, jetVetoQGnChg, jetVetoQGnNeutral, jetVetoQGlike, jetVetoQGflavor, jetVetoQGNbConst, jetVetoQGConstId, jetVetoQGConstPt ;

  // quality cuts of the first 2 jets
  float jetsBtagHE1,jetsBtagHE2,jetsBtagHP1,jetsBtagHP2, jetsBtagCSV1, jetsBtagCSV2;
  int nJets20BTagged, nJets20BTaggedBUp, nJets20BTaggedBDown, nJets20BTaggedLUp, nJets20BTaggedLDown, nJets20BTaggedLoose;
  float chFracPV1, chFracPV2;

  // kinematical variables of the veto jet
  float ptVeto, etaVeto, phiVeto; 
  int isVetoInJets; float chFracPVVeto;

  // 

  // diTau related variables
  float diTauNSVfitMass_,diTauNSVfitMassErrUp_,diTauNSVfitMassErrDown_;
  float diTauNSVfitMassZLUp_,diTauNSVfitMassZLDown_;
  float diTauNSVfitPt_,diTauNSVfitPtErrUp_,diTauNSVfitPtErrDown_;
  float diTauSVFitMass, diTauSVFitMassCal0, diTauSVFitMassCal1, diTauSVFitMassCal2, 
    diTauSVFitPt, diTauSVFitEta , diTauSVFitPhi ;
  float genDiTauMass_;
  float diTauSVFitMassSA, diTauSVFitMassErrSA;
  float diTauCAMass, diTauCAPt, diTauCAEta, diTauCAPhi;
  float diTauVisMass,diTauVisPt,diTauVisEta,diTauVisPhi;
  float diTauRecoPt,diTauRecoPhi;
  float diTauMinMass;
  float diTauVisMassZLUp, diTauVisMassZLDown;

  float etaTau1Fit, etaTau2Fit, phiTau1Fit, phiTau2Fit, ptTau1Fit, ptTau2Fit;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,mL1,dRL1L2,dEtaL1L2,dPhiL1L2,dPhiL1J1,dPhiL1J2,dPhiL2J1,dPhiL2J2,dxy1_, dz1_;
  float diTauCharge_, chargeL1_,
    MtLeg1_,MtLeg1Corr_,MtLeg1MVA_,
    MtLeg2_,MtLeg2Corr_,MtLeg2MVA_,
    pZeta_,pZetaCorr_,pZetaMVA_,
    pZetaVis_,pZetaVisCorr_,pZetaVisMVA_,pZetaSig_;
  float MEt,MEtPhi,MEtCorr,MEtCorrPhi, MEtMVA, MEtMVAPhi; 
  float MEtCov00,MEtCov01,MEtCov10,MEtCov11;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1DBetav2,combRelIsoLeg1Rho, combIsoLeg2;
  float rhoFastJet_;
  //   float isoLeg1MVA_;
  //TauID
  int decayMode_,decayModeFinding_,decayModeFindingNewDM_,decayModeFindingOldDM_;
  int AntiEDeadEcal_,tightestAntiECutWP_,tightestAntiEMVA5WP_,AntiEMVA5category_;
  int tightestAntiMuWP_,tightestAntiMu2WP_,tightestAntiMu3WP_,tightestAntiMuMVAWP_;
  float AntiEMVA5raw_,AntiMuMVAraw_;
  int tightestHPSDBWP_,tightestHPSDB3HWP_,tightestHPSMVA3newDMwLTWP_,tightestHPSMVA3newDMwoLTWP_,tightestHPSMVA3oldDMwLTWP_,tightestHPSMVA3oldDMwoLTWP_;
  float hpsDB3H_,hpsMVA3newDMwLT_,hpsMVA3newDMwoLT_,hpsMVA3oldDMwLT_,hpsMVA3oldDMwoLT_;
  ///NewTauID input variables
  int patDecayMode_,hasSecVtx_;
  float chargedIsoPtSum_,neutralIsoPtSum_,puCorrPtSum_,dxyTau_,dxyErrTau_,dxySigTau_,flightLength2_,flightLengthSig_;
  ///NewTauID useful variables
  float dxyPCAX_,dxyPCAY_,dxyPCAZ_,flightLengthX_,flightLengthY_,flightLengthZ_,
    pvX_,pvY_,pvZ_,pvCov00_,pvCov01_,pvCov02_,pvCov11_,pvCov12_,pvCov22_,
    svX_,svY_,svZ_,svCov00_,svCov01_,svCov02_,svCov11_,svCov12_,svCov22_;

  float pfJetPt_;
  float L1etm_, L1etmPhi_, L1etmCorr_, L1etmNoTau_, L1etmNoTauPhi_, L1etmPhiCorr_, L1etmCorrUp_, L1etmPhiCorrUp_, L1etmCorrDown_, L1etmPhiCorrDown_,
    passL1etmCut_, passL1etmCutABC_, passL1etmCutUp_, passL1etmCutABCUp_, passL1etmCutDown_, passL1etmCutABCDown_; // ND
  float etmCut=20;
  float caloMEtNoHFUncorr_, caloMEtNoHFUncorrPhi_, caloMEtNoHF_,//  caloMEtNoTau_, caloMEtNoTauPhi_,
    // caloMEtUncorrNoTau_, caloMEtUncorrNoTauPhi_,
    caloMEtNoHFPhi_, caloMEtNoHFUp_, caloMEtNoHFUpPhi_, caloMEtNoHFDown_, caloMEtNoHFDownPhi_; // ND
  float sumEt_, caloNoHFsumEt_, caloNoHFsumEtCorr_; // ND

  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;
  float fakeRateRun2011, fakeRateWMC, effDYMC, CDFWeight;
  float visGenTauMass, genTauPt, genTauEta, genTauPhi, genVMass, genVPt, genMass;
  float genTrueTau1Pt, genTrueTau1Eta, genTrueTau1Phi, genTrueTau2Pt, genTrueTau2Eta, genTrueTau2Phi;
  int genTrueTau1DecaysLeptonically, genTrueTau2DecaysLeptonically;
  int genTrueTau1Charge, genTrueTau2Charge;
  float genMuPt, genMuEta, genMuPhi;
  int genDecayMode_;

  // event-related variables
  float numPV_ , sampleWeight, sampleWeightW, sampleWeightDY, puWeight, puWeight2, embeddingWeight_,HqTWeight,HqTWeightUp,HqTWeightDown,ZmmWeight;

  //Top pT weights
  float topPtWeightNom_,topPtWeightUp_,topPtWeightDown_;

  //Higgs pT weight MSSM
  //mhmax
  float mssmHiggsPtReweightGluGlu_mhmax ;
  float                                  mssmHiggsPtReweightGluGlu_mhmax_HqTUp, mssmHiggsPtReweightGluGlu_mhmax_HqTDown ;//HqT
  float                                  mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp, mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown ;//HIGLU
  float                                  mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp, mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown ;//tanBeta
  
  //mhmodplus
  float mssmHiggsPtReweightGluGlu_mhmodplus ;
  float                                  mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp, mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown ;//HqT
  float                                  mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp, mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown ;//HIGLU
  float                                  mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp, mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown ;//tanBeta
  
  //mhmodminus
  float mssmHiggsPtReweightGluGlu_mhmodminus ;
  float                                  mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp, mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown ;//HqT
  float                                  mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp, mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown ;//HIGLU
  float                                  mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp, mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown ;//tanBeta
  
  //lowmH
  float mssmHiggsPtReweightGluGlu_lowmH ;
  float                                  mssmHiggsPtReweightGluGlu_lowmH_HqTUp, mssmHiggsPtReweightGluGlu_lowmH_HqTDown ;//HqT
  float                                  mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp, mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown ;//HIGLU
  float                                  mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp, mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown ;//tanBeta
  
  float  weightHepNup,weightHepNupHighStatW,weightHepNupDY, puWeightHCP, puWeightD, puWeightDLow, puWeightDHigh,highPtWeightUp,highPtWeightDown;
  
  float embeddingFilterEffWeight_,TauSpinnerWeight_,ZmumuEffWeight_,diTauMassVSdiTauPtWeight_,tau2EtaVStau1EtaWeight_,tau2PtVStau1PtWeight_,muonRadiationWeight_,muonRadiationDownWeight_,muonRadiationUpWeight_;//IN
  int numOfLooseIsoDiTaus_;
  float nPUVertices_;
  
  // Event trigger matching HLTmatchPFJet320
  float HLTx, HLTxQCD, HLTxSoft, HLTxQCDSoft, HLTxIsoMu8Tau20, HLTxIsoMu15ETM20, HLTxMu8, HLTxMu17Mu8, HLTxDiTau, HLTPFJet320, HLTIsoMu24, HLTIsoMu24_eta2p1;
  float HLTmatchMu, HLTmatchTau, HLTmatch, HLTmatchQCD, HLTmatchSoft, HLTmatchQCDSoft, HLTmatchIsoMu8Tau20, HLTmatchIsoMu15ETM20, HLTmatchMu8, HLTmatchDiTau, HLTmatchDiTauMu, HLTmatchDiTauTau, HLTmatchDiTauL1, HLTmatchDiTauL2, HLTmatchIsoMu24, HLTmatchIsoMu24_eta2p1, HLTmatchPFJet320;

  // Muon weights
  float HLTMu, HLTMuA, HLTMuB, HLTMuC, HLTMuD, HLTMuABC, HLTMuABCD, HLTMuMCold, HLTMuMCnew, HLTMuSoft, HLTMuSoftMC, HLTMuShift, HLTMuShiftMC, HLTMuABCShift, HLTMuShiftMCold;
  float HLTweightMu, HLTweightMuA, HLTweightMuB, HLTweightMuC, HLTweightMuD, HLTweightMuABC, HLTweightMuABCD, HLTweightMuSoft, HLTweightMuShift, HLTweightMuABCShift;
  float SFMuID, SFMuID_ABC, SFMuID_D, SFMuID_ABCD;
  float SFMuIso, SFMuIso_ABC, SFMuIso_D, SFMuIso_ABCD;
  float SFMu, SFMu_ABC, SFMu_D, SFMu_ABCD;
  float EffMuID_ABC, EffMuID_D, EffMuID_ABCD, EffMuID_ABC_MC, EffMuID_D_MC, EffMuID_ABCD_MC;
  float EffMuIso_ABC, EffMuIso_D, EffMuIso_ABCD, EffMuIso_ABC_MC, EffMuIso_D_MC, EffMuIso_ABCD_MC;
  
  // Tau weights
  float HLTTau, HLTTauD, HLTTauABC;
  float HLTTauMC, HLTTauMCD, HLTTauMCABC;
  float HLTweightTau, HLTweightTauD, HLTweightTauABC, HLTweight2Triggers;
  float SFTau;
  float weightDecayMode_, weightTauFakeWJet_, weightTauFakeWJetUp_, weightTauFakeWJetDown_;
  float weightJetFakeQCD_;

  // Other informations about mu/tau
  int isTauLegMatched_,muFlag_,isPFMuon_,isTightMuon_,genDecay_,leptFakeTau;
  int isTauLegMatchedToLep_;
  int vetoEventOld_;
  int vetoEventNew_;
  
  //Parton Info for W+NJet
  int parton_, genPartMult_, leadGenPartPdg_, hepNUP_;
  float leadGenPartPt_;
  
  // event id
  ULong64_t event_,run_,lumi_;
  int index_, pairIndex, counter;
  bool passQualityCuts;
  
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
  
  /*
    outTreePtOrd->Branch("pt1_v2",  &pt1_v2,"pt1_v2/F");
    outTreePtOrd->Branch("pt2_v2",  &pt2_v2,"pt2_v2/F");
    outTreePtOrd->Branch("eta1_v2", &eta1_v2,"eta1_v2/F");
    outTreePtOrd->Branch("eta2_v2", &eta2_v2,"eta2_v2/F");
    outTreePtOrd->Branch("phi1_v2", &phi1_v2,"phi1_v2/F");
    outTreePtOrd->Branch("phi2_v2", &phi2_v2,"phi2_v2/F");
    outTreePtOrd->Branch("Deta_v2", &Deta_v2,"Deta_v2/F");
    outTreePtOrd->Branch("Dphi_v2", &Dphi_v2,"Dphi_v2/F");
    outTreePtOrd->Branch("Mjj_v2",  &Mjj_v2,"Mjj_v2/F");
  */
  
  outTreePtOrd->Branch("diJetPt",  &diJetPt , "diJetPt/F");
  outTreePtOrd->Branch("diJetPhi", &diJetPhi , "diJetPhi/F");
  outTreePtOrd->Branch("dPhiHjet", &dPhiHjet ,"dPhiHjet/F");
  outTreePtOrd->Branch("c1",       &c1 , "c1/F");
  outTreePtOrd->Branch("c2",       &c2 , "c2/F");
  
  outTreePtOrd->Branch("ptB1",  &ptB1, "ptB1/F");
  outTreePtOrd->Branch("etaB1", &etaB1,"etaB1/F");
  outTreePtOrd->Branch("phiB1", &phiB1,"phiB1/F");
  
  outTreePtOrd->Branch("MVAvbf",        &MVAvbf,         "MVAvbf/F");
  outTreePtOrd->Branch("jet1PUMVA",     &jet1PUMVA,      "jet1PUMVA/F");
  outTreePtOrd->Branch("jet2PUMVA",     &jet2PUMVA,      "jet2PUMVA/F");
  outTreePtOrd->Branch("jetVetoPUMVA",  &jetVetoPUMVA,   "jetVetoPUMVA/F");
  outTreePtOrd->Branch("jet1PUWP",      &jet1PUWP,       "jet1PUWP/F");
  outTreePtOrd->Branch("jet2PUWP",      &jet2PUWP,       "jet2PUWP/F");
  outTreePtOrd->Branch("jetVetoPUWP",   &jetVetoPUWP,    "jetVetoPUWP/F");
  
  outTreePtOrd->Branch("jet1QGmlp",     &jet1QGmlp,      "jet1QGmlp/F");
  outTreePtOrd->Branch("jet1QGlike",    &jet1QGlike,     "jet1QGlike/F");
  outTreePtOrd->Branch("jet1QGrhoIso",  &jet1QGrhoIso,   "jet1QGrhoIso/F");
  outTreePtOrd->Branch("jet1QGptD",     &jet1QGptD,      "jet1QGptD/F");
  outTreePtOrd->Branch("jet1QGaxis1",   &jet1QGaxis1,    "jet1QGaxis1/F");
  outTreePtOrd->Branch("jet1QGaxis2",   &jet1QGaxis2,    "jet1QGaxis2/F");
  outTreePtOrd->Branch("jet1QGmult",    &jet1QGmult,     "jet1QGmult/F");
  outTreePtOrd->Branch("jet1QGnChg",    &jet1QGnChg,     "jet1QGnChg/F");
  outTreePtOrd->Branch("jet1QGnNeutral",&jet1QGnNeutral, "jet1QGnNeutral/F");
  outTreePtOrd->Branch("jet1QGflavor",  &jet1QGflavor,   "jet1QG/F");
  outTreePtOrd->Branch("jet1QGNbConst", &jet1QGNbConst,  "jet1QGNbConst/F");
  outTreePtOrd->Branch("jet1QGConstId", &jet1QGConstId,  "jet1QGConstId/F");
  outTreePtOrd->Branch("jet1QGConstPt", &jet1QGConstPt,  "jet1QGConstPt/F");
  outTreePtOrd->Branch("jet2QGmlp",     &jet2QGmlp,      "jet2QGmlp/F");
  outTreePtOrd->Branch("jet2QGlike",    &jet2QGlike,     "jet2QGlike/F");
  outTreePtOrd->Branch("jet2QGrhoIso",  &jet2QGrhoIso,   "jet2QGrhoIso/F");
  outTreePtOrd->Branch("jet2QGptD",     &jet2QGptD,      "jet2QGptD/F");
  outTreePtOrd->Branch("jet2QGaxis1",   &jet2QGaxis1,    "jet2QGaxis1/F");
  outTreePtOrd->Branch("jet2QGaxis2",   &jet2QGaxis2,    "jet2QGaxis2/F");
  outTreePtOrd->Branch("jet2QGmult",    &jet2QGmult,     "jet2QGmult/F");
  outTreePtOrd->Branch("jet2QGnChg",    &jet2QGnChg,     "jet2QGnChg/F");
  outTreePtOrd->Branch("jet2QGnNeutral",&jet2QGnNeutral, "jet2QGnNeutral/F");
  outTreePtOrd->Branch("jet2QGflavor",  &jet2QGflavor,   "jet2QG/F");
  outTreePtOrd->Branch("jet2QGNbConst", &jet2QGNbConst,  "jet2QGNbConst/F");
  outTreePtOrd->Branch("jet2QGConstId", &jet2QGConstId,  "jet2QGConstId/F");
  outTreePtOrd->Branch("jet2QGConstPt", &jet2QGConstPt,  "jet2QGConstPt/F");
  outTreePtOrd->Branch("jetVetoQGmlp",     &jetVetoQGmlp,      "jetVetoQGmlp/F");
  outTreePtOrd->Branch("jetVetoQGlike",    &jetVetoQGlike,     "jetVetoQGlike/F");
  outTreePtOrd->Branch("jetVetoQGrhoIso",  &jetVetoQGrhoIso,   "jetVetoQGrhoIso/F");
  outTreePtOrd->Branch("jetVetoQGptD",  &jetVetoQGptD,   "jetVetoQGptD/F");
  outTreePtOrd->Branch("jetVetoQGaxis1",&jetVetoQGaxis1, "jetVetoQGaxis1/F");
  outTreePtOrd->Branch("jetVetoQGaxis2",&jetVetoQGaxis2, "jetVetoQGaxis2/F");
  outTreePtOrd->Branch("jetVetoQGmult", &jetVetoQGmult,  "jetVetoQGmult/F");
  outTreePtOrd->Branch("jetVetoQGnChg", &jetVetoQGnChg,  "jetVetoQGnChg/F");
  outTreePtOrd->Branch("jetVetoQGnNeutral", &jetVetoQGnNeutral, "jetVetoQGnNeutral/F");
  outTreePtOrd->Branch("jetVetoQGflavor",  &jetVetoQGflavor,   "jetVetoQG/F");
  outTreePtOrd->Branch("jetVetoQGNbConst", &jetVetoQGNbConst,  "jetVetoQGNbConst/F");
  outTreePtOrd->Branch("jetVetoQGConstId", &jetVetoQGConstId,  "jetVetoQGConstId/F");
  outTreePtOrd->Branch("jetVetoQGConstPt", &jetVetoQGConstPt,  "jetVetoQGConstPt/F");

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
  
  outTreePtOrd->Branch("ptAll",  &ptAll,  "ptAll[nJets30]/F");
  outTreePtOrd->Branch("etaAll", &etaAll, "etaAll[nJets30]/F");
  outTreePtOrd->Branch("phiAll", &phiAll, "phiAll[nJets30]/F");
  outTreePtOrd->Branch("energyAll", &energyAll, "energyAll[nJets30]/F");
  outTreePtOrd->Branch("csvAll", &csvAll, "csvAll[nJets30]/F");
  outTreePtOrd->Branch("ptAllB", &ptAllB, "ptAllB[nJets20BTagged]/F");
  outTreePtOrd->Branch("etaAllB",&etaAllB,"etaAllB[nJets20BTagged]/F");
  outTreePtOrd->Branch("phiAllB",&phiAllB,"phiAllB[nJets20BTagged]/F");
  outTreePtOrd->Branch("energyAllB", &energyAllB, "energyAllB[nJets20BTagged]/F");
  outTreePtOrd->Branch("csvAllB",&csvAllB,"csvAllB[nJets20BTagged]/F");
  
  outTreePtOrd->Branch("ptAllBLoose", &ptAllBLoose, "ptAllBLoose[nJets20BTaggedLoose]/F");
  outTreePtOrd->Branch("etaAllBLoose",&etaAllBLoose,"etaAllBLoose[nJets20BTaggedLoose]/F");
  outTreePtOrd->Branch("phiAllBLoose",&phiAllBLoose,"phiAllBLoose[nJets20BTaggedLoose]/F");
  outTreePtOrd->Branch("energyAllBLoose",&energyAllBLoose,"energyAllBLoose[nJets20BTaggedLoose]/F");
  outTreePtOrd->Branch("csvAllBLoose",&csvAllBLoose,"csvAllBLoose[nJets20BTaggedLoose]/F");
  //Variables for Luca
  outTreePtOrd->Branch("dRb1b2",    &dRb1b2,   "dRb1b2/F");
  outTreePtOrd->Branch("dPhib1b2",  &dPhib1b2, "dPhib1b2/F");
  outTreePtOrd->Branch("dEtab1b2",  &dEtab1b2, "dEtab1b2/F");
  outTreePtOrd->Branch("dRb1L1",    &dRb1L1,   "dRb1L1/F");
  outTreePtOrd->Branch("dRb1L2",    &dRb1L2,   "dRb1L2/F");
  outTreePtOrd->Branch("dRb2L1",    &dRb2L1,   "dRb2L1/F");
  outTreePtOrd->Branch("dRb2L2",    &dRb2L2,   "dRb2L2/F");
  outTreePtOrd->Branch("dRMaxbJetLept",    &dRMaxbJetLept,   "dRMaxbJetLept/F");
  outTreePtOrd->Branch("dRMinbJetLept",    &dRMinbJetLept,   "dRMinbJetLept/F");
  outTreePtOrd->Branch("ptHbb",     &ptHbb,    "ptHbb/F");
  outTreePtOrd->Branch("visMassHbb",     &visMassHbb,    "visMassHbb/F");
  outTreePtOrd->Branch("dRHbbHtt",     &dRHbbHtt,    "dRHbbHtt/F");
  outTreePtOrd->Branch("dPhiHbbHtt",     &dPhiHbbHtt,    "dPhiHbbHtt/F");
  outTreePtOrd->Branch("dEtaHbbHtt",     &dEtaHbbHtt,    "dEtaHbbHtt/F");
  outTreePtOrd->Branch("diHiggsVisMass",     &diHiggsVisMass,    "diHiggsVisMass/F");
  outTreePtOrd->Branch("dRHbbMETMVA",     &dRHbbMETMVA,    "dRHbbMETMVA/F");
  outTreePtOrd->Branch("dPhiHbbMETMVA",     &dPhiHbbMETMVA,    "dPhiHbbMETMVA/F");
  outTreePtOrd->Branch("dEtaHbbMETMVA",     &dEtaHbbMETMVA,    "dEtaHbbMETMVA/F");
  outTreePtOrd->Branch("dRHttMETMVA",     &dRHttMETMVA,    "dRHttMETMVA/F");
  outTreePtOrd->Branch("dPhiHttMETMVA",     &dPhiHttMETMVA,    "dPhiHttMETMVA/F");
  outTreePtOrd->Branch("dEtaHttMETMVA",     &dEtaHttMETMVA,    "dEtaHttMETMVA/F");

  outTreePtOrd->Branch("ptVeto",  &ptVeto, "ptVeto/F");
  outTreePtOrd->Branch("phiVeto", &phiVeto,"phiVeto/F");
  outTreePtOrd->Branch("etaVeto", &etaVeto,"etaVeto/F");
  outTreePtOrd->Branch("isVetoInJets", &isVetoInJets,"isVetoInJets/I");
  outTreePtOrd->Branch("chFracPVVeto", &chFracPVVeto,"chFracPVVeto/F"); 

  outTreePtOrd->Branch("genDiTauMass", &genDiTauMass_, "genDiTauMass/F");

  outTreePtOrd->Branch("diTauNSVfitMass",       &diTauNSVfitMass_,       "diTauNSVfitMass/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp_,  "diTauNSVfitMassErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown_,"diTauNSVfitMassErrDown/F");
  outTreePtOrd->Branch("diTauNSVfitMassZLUp",  &diTauNSVfitMassZLUp_,  "diTauNSVfitMassZLUp/F");
  outTreePtOrd->Branch("diTauNSVfitMassZLDown",&diTauNSVfitMassZLDown_,"diTauNSVfitMassZLDown/F");
  outTreePtOrd->Branch("diTauNSVfitPt",       &diTauNSVfitPt_,       "diTauNSVfitPt/F");
  outTreePtOrd->Branch("diTauNSVfitPtErrUp",  &diTauNSVfitPtErrUp_,  "diTauNSVfitPtErrUp/F");
  outTreePtOrd->Branch("diTauNSVfitPtErrDown",&diTauNSVfitPtErrDown_,"diTauNSVfitPtErrDown/F");
  
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
  outTreePtOrd->Branch("diTauVisMassZLUp",&diTauVisMassZLUp,"diTauVisMassZLUp/F");
  outTreePtOrd->Branch("diTauVisMassZLDown",&diTauVisMassZLDown,"diTauVisMassZLDown/F");

  outTreePtOrd->Branch("diTauMinMass",&diTauMinMass,"diTauMisMass/F");

  outTreePtOrd->Branch("etaL1",   &etaL1,"etaL1/F");
  outTreePtOrd->Branch("etaL2",   &etaL2,"etaL2/F");
  outTreePtOrd->Branch("ptL1",    &ptL1,"ptL1/F");
  outTreePtOrd->Branch("ptL2",    &ptL2,"ptL2/F");
  outTreePtOrd->Branch("mL1",    &mL1,"mL1/F");
  outTreePtOrd->Branch("phiL1",   &phiL1,"phiL1/F");
  outTreePtOrd->Branch("phiL2",   &phiL2,"phiL2/F");
  outTreePtOrd->Branch("dRL1L2"  ,&dRL1L2,"dRL1L2/F");
  outTreePtOrd->Branch("dEtaL1L2",&dEtaL1L2,"dEtaL1L2/F");
  outTreePtOrd->Branch("dPhiL1L2",&dPhiL1L2,"dPhiL1L2/F");
  outTreePtOrd->Branch("dPhiL1J1",&dPhiL1J1,"dPhiL1J1/F");
  outTreePtOrd->Branch("dPhiL1J2",&dPhiL1J2,"dPhiL1J2/F");
  outTreePtOrd->Branch("dPhiL2J1",&dPhiL2J1,"dPhiL2J1/F");
  outTreePtOrd->Branch("dPhiL2J2",&dPhiL2J2,"dPhiL2J2/F");

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

  outTreePtOrd->Branch("visGenTauMass",           &visGenTauMass, "visGenTauMass/F");
  outTreePtOrd->Branch("genTauPt",                &genTauPt, "genTauPt/F");
  outTreePtOrd->Branch("genTauEta",               &genTauEta, "genTauEta/F");
  outTreePtOrd->Branch("genTauPhi",               &genTauPhi, "genTauPhi/F");
  outTreePtOrd->Branch("genMuPt",                 &genMuPt, "genMuPt/F");
  outTreePtOrd->Branch("genMuEta",                &genMuEta, "genMuEta/F");
  outTreePtOrd->Branch("genMuPhi",                &genMuPhi, "genMuPhi/F");
  outTreePtOrd->Branch("genDecayMode",            &genDecayMode_, "genDecayMode/I");
  outTreePtOrd->Branch("genVMass",                &genVMass,     "genVMass/F");
  outTreePtOrd->Branch("genVPt",                  &genVPt,     "genVPt/F");
  outTreePtOrd->Branch("genMass",                 &genMass,     "genMass/F");

  outTreePtOrd->Branch("genTrueTau1Pt",                 &genTrueTau1Pt,     "genTrueTau1Pt/F");
  outTreePtOrd->Branch("genTrueTau1Eta",                 &genTrueTau1Eta,     "genTrueTau1Eta/F");
  outTreePtOrd->Branch("genTrueTau1Phi",                 &genTrueTau1Phi,     "genTrueTau1Phi/F");

  outTreePtOrd->Branch("genTrueTau2Pt",                 &genTrueTau2Pt,     "genTrueTau2Pt/F");
  outTreePtOrd->Branch("genTrueTau2Eta",                 &genTrueTau2Eta,     "genTrueTau2Eta/F");
  outTreePtOrd->Branch("genTrueTau2Phi",                 &genTrueTau2Phi,     "genTrueTau2Phi/F");

  outTreePtOrd->Branch("genTrueTau1DecaysLeptonically",                 &genTrueTau1DecaysLeptonically,     "genTrueTau1DecaysLeptonically/I");
  outTreePtOrd->Branch("genTrueTau2DecaysLeptonically",                 &genTrueTau2DecaysLeptonically,     "genTrueTau2DecaysLeptonically/I");

  outTreePtOrd->Branch("genTrueTau1Charge",                 &genTrueTau1Charge,     "genTrueTau1Charge/I");
  outTreePtOrd->Branch("genTrueTau2Charge",                 &genTrueTau2Charge,     "genTrueTau2Charge/I");

  outTreePtOrd->Branch("pfJetPt",                 &pfJetPt_,"pfJetPt/F");
  outTreePtOrd->Branch("fakeRateRun2011",         &fakeRateRun2011,"fakeRateRun2011/F");
  outTreePtOrd->Branch("fakeRateWMC",             &fakeRateWMC,"fakeRateWMC/F");
  outTreePtOrd->Branch("effDYMC",                 &effDYMC,"effDYMC/F");
  outTreePtOrd->Branch("CDFWeight",               &CDFWeight,"CDFWeight/F");
  
  outTreePtOrd->Branch("diTauCharge", &diTauCharge_,"diTauCharge/F");
  outTreePtOrd->Branch("chargeL1", &chargeL1_,"chargeL1/F");
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

  // L1 ETM //
  outTreePtOrd->Branch("L1etm",       &L1etm_,       "L1etm/F");//MB
  outTreePtOrd->Branch("L1etmPhi",    &L1etmPhi_,    "L1etmPhi/F");//MB
  outTreePtOrd->Branch("L1etmCut",    &etmCut,       "L1etm/F");//ND
  //
  outTreePtOrd->Branch("L1etmNoTau",      &L1etmNoTau_,      "L1etmNoTau/F");//MB
  outTreePtOrd->Branch("L1etmNoTauPhi",      &L1etmNoTauPhi_,      "L1etmNoTauPhi/F");//MB
  outTreePtOrd->Branch("L1etmCorr",       &L1etmCorr_,       "L1etmCorr/F");//MB
  outTreePtOrd->Branch("L1etmPhiCorr",    &L1etmPhiCorr_,    "L1etmPhiCorr/F");//MB
  outTreePtOrd->Branch("L1etmCorrUp",     &L1etmCorrUp_,     "L1etmCorrUp/F");//MB
  outTreePtOrd->Branch("L1etmPhiCorrUp",  &L1etmPhiCorrUp_,  "L1etmPhiCorrUp/F");//MB
  outTreePtOrd->Branch("L1etmCorrDown",   &L1etmCorrDown_,   "L1etmCorrDown/F");//MB
  outTreePtOrd->Branch("L1etmPhiCorrDown",&L1etmPhiCorrDown_,"L1etmPhiCorrDown/F");//MB
  //
  outTreePtOrd->Branch("passL1etmCut",        &passL1etmCut_,       "passL1etmCut/F");//ND
  outTreePtOrd->Branch("passL1etmCutABC",     &passL1etmCutABC_,    "passL1etmCutABC/F");//ND
  outTreePtOrd->Branch("passL1etmCutUp",      &passL1etmCutUp_,     "passL1etmCutUp/F");//ND
  outTreePtOrd->Branch("passL1etmCutABCUp",   &passL1etmCutABCUp_,  "passL1etmCutABCUp/F");//ND
  outTreePtOrd->Branch("passL1etmCutDown",    &passL1etmCutDown_,   "passL1etmCutDown/F");//ND
  outTreePtOrd->Branch("passL1etmCutABCDown", &passL1etmCutABCDown_,"passL1etmCutABCDown/F");//ND

  // CALO MET //
//   outTreePtOrd->Branch("caloMEtUncorrNoTau",   &caloMEtUncorrNoTau_,  "caloMEtUncorrNoTau/F");//MB
//   outTreePtOrd->Branch("caloMEtNoTau",         &caloMEtNoTau_,        "caloMEtNoTau/F");//MB
//   outTreePtOrd->Branch("caloMEtUncorrNoTauPhi",   &caloMEtUncorrNoTauPhi_,  "caloMEtUncorrNoTauPhi/F");//MB
//   outTreePtOrd->Branch("caloMEtNoTauPhi",         &caloMEtNoTauPhi_,        "caloMEtNoTauPhi/F");//MB
  outTreePtOrd->Branch("caloMEtNoHF",          &caloMEtNoHF_,         "caloMEtNoHF/F");//MB
  outTreePtOrd->Branch("caloMEtNoHFPhi",       &caloMEtNoHFPhi_,      "caloMEtNoHFPhi/F");//MB
  outTreePtOrd->Branch("caloMEtNoHFUncorr",    &caloMEtNoHFUncorr_,   "caloMEtNoHFUncorr/F");//MB
  outTreePtOrd->Branch("caloMEtNoHFUncorrPhi", &caloMEtNoHFUncorrPhi_,"caloMEtNoHFUncorrPhi/F");//MB
  outTreePtOrd->Branch("caloMEtNoHFUp",        &caloMEtNoHFUp_,       "caloMEtNoHFUp/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFUpPhi",     &caloMEtNoHFUpPhi_,    "caloMEtNoHFUpPhi/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFDown",      &caloMEtNoHFDown_,     "caloMEtNoHFDown/F");// ND
  outTreePtOrd->Branch("caloMEtNoHFDownPhi",   &caloMEtNoHFDownPhi_,  "caloMEtNoHFDownPhi/F");// ND

  outTreePtOrd->Branch("sumEt",             &sumEt_,             "sumEt/F");// ND
  outTreePtOrd->Branch("caloNoHFsumEt",     &caloNoHFsumEt_,     "caloNoHFsumEt/F");// ND
  outTreePtOrd->Branch("caloNoHFsumEtCorr", &caloNoHFsumEtCorr_, "caloNoHFsumEtCorr/F");// ND

  outTreePtOrd->Branch("combRelIsoLeg1",     &combRelIsoLeg1,"combRelIsoLeg1/F");
  outTreePtOrd->Branch("combRelIsoLeg1Beta", &combRelIsoLeg1Beta,"combRelIsoLeg1Beta/F");
  outTreePtOrd->Branch("combRelIsoLeg1Rho",  &combRelIsoLeg1Rho,"combRelIsoLeg1Rho/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBeta",&combRelIsoLeg1DBeta,"combRelIsoLeg1DBeta/F");
  outTreePtOrd->Branch("combRelIsoLeg1DBetav2",&combRelIsoLeg1DBetav2,"combRelIsoLeg1DBetav2/F");
//   outTreePtOrd->Branch("isoLeg1MVA",         &isoLeg1MVA_,"isoLeg1MVA/F");
  outTreePtOrd->Branch("combIsoLeg2",        &combIsoLeg2,"combIsoLeg2/F");
  outTreePtOrd->Branch("rhoFastJet",         &rhoFastJet_,"rhoFastJet/F");

  //TauID
  outTreePtOrd->Branch("decayModeFinding",&decayModeFinding_,"decayModeFinding/I");
  outTreePtOrd->Branch("decayModeFindingNewDM",&decayModeFindingNewDM_,"decayModeFindingNewDM/I");
  outTreePtOrd->Branch("decayModeFindingOldDM",&decayModeFindingOldDM_,"decayModeFindingOldDM/I");
  outTreePtOrd->Branch("AntiEDeadEcal",&AntiEDeadEcal_,"AntiEDeadEcal/I");
  outTreePtOrd->Branch("tightestAntiECutWP",&tightestAntiECutWP_,"tightestAntiECutWP/I");
  outTreePtOrd->Branch("tightestAntiEMVA5WP",&tightestAntiEMVA5WP_,"tightestAntiEMVA5WP/I");
  outTreePtOrd->Branch("AntiEMVA5category",&AntiEMVA5category_,"AntiEMVA5category/I");
  outTreePtOrd->Branch("AntiEMVA5raw",&AntiEMVA5raw_,"AntiEMVA5raw/F");
  outTreePtOrd->Branch("tightestAntiMuWP",&tightestAntiMuWP_,"tightestAntiMuWP/I");
  outTreePtOrd->Branch("tightestAntiMu2WP",&tightestAntiMu2WP_,"tightestAntiMu2WP/I");
  outTreePtOrd->Branch("tightestAntiMu3WP",&tightestAntiMu3WP_,"tightestAntiMu3WP/I");
  outTreePtOrd->Branch("tightestAntiMuMVAWP",&tightestAntiMuMVAWP_,"tightestAntiMuMVAWP/I");
  outTreePtOrd->Branch("AntiMuMVAraw",&AntiMuMVAraw_,"AntiMuMVAraw/F");
  outTreePtOrd->Branch("tightestHPSDBWP",&tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("tightestHPSDB3HWP",&tightestHPSDB3HWP_,"tightestHPSDB3HWP/I");
  outTreePtOrd->Branch("hpsDB3H",&hpsDB3H_,"hpsDB3H/F");
  outTreePtOrd->Branch("tightestHPSMVA3newDMwLTWP",&tightestHPSMVA3newDMwLTWP_,"tightestHPSMVA3newDMwLTWP/I");
  outTreePtOrd->Branch("hpsMVA3newDMwLT",&hpsMVA3newDMwLT_,"hpsMVA3newDMwLT/F");
  outTreePtOrd->Branch("tightestHPSMVA3newDMwoLTWP",&tightestHPSMVA3newDMwoLTWP_,"tightestHPSMVA3newDMwoLTWP/I");
  outTreePtOrd->Branch("hpsMVA3newDMwoLT",&hpsMVA3newDMwoLT_,"hpsMVA3newDMwoLT/F");
  outTreePtOrd->Branch("tightestHPSMVA3oldDMwLTWP",&tightestHPSMVA3oldDMwLTWP_,"tightestHPSMVA3oldDMwLTWP/I");
  outTreePtOrd->Branch("hpsMVA3oldDMwLT",&hpsMVA3oldDMwLT_,"hpsMVA3oldDMwLT/F");
  outTreePtOrd->Branch("tightestHPSMVA3oldDMwoLTWP",&tightestHPSMVA3oldDMwoLTWP_,"tightestHPSMVA3oldDMwoLTWP/I");
  outTreePtOrd->Branch("hpsMVA3oldDMwoLT",&hpsMVA3oldDMwoLT_,"hpsMVA3oldDMwoLT/F");
  outTreePtOrd->Branch("decayMode",          &decayMode_,"decayMode/I");
  ///NewTauID input variables
  outTreePtOrd->Branch("chargedIsoPtSum",&chargedIsoPtSum_,"chargedIsoPtSum/F");
  outTreePtOrd->Branch("neutralIsoPtSum",&neutralIsoPtSum_,"neutralIsoPtSum/F");
  outTreePtOrd->Branch("puCorrPtSum",&puCorrPtSum_,"puCorrPtSum/F");
  outTreePtOrd->Branch("patDecayMode",&patDecayMode_,"patDecayMode/I");
  outTreePtOrd->Branch("dxyTau",&dxyTau_,"dxyTau/F");
  outTreePtOrd->Branch("dxyErrTau",&dxyErrTau_,"dxyErrTau/F");
  outTreePtOrd->Branch("dxySigTau",&dxySigTau_,"dxySigTau/F");
  outTreePtOrd->Branch("hasSecVtx",&hasSecVtx_,"hasSecVtx/I");
  outTreePtOrd->Branch("flightLength2",&flightLength2_,"flightLength2/F");
  outTreePtOrd->Branch("flightLengthSig",&flightLengthSig_,"flightLengthSig/F");
  ///NewTauID useful variables
  outTreePtOrd->Branch("dxyPCAX",&dxyPCAX_,"dxyPCAX/F");
  outTreePtOrd->Branch("dxyPCAY",&dxyPCAY_,"dxyPCAY/F");
  outTreePtOrd->Branch("dxyPCAZ",&dxyPCAZ_,"dxyPCAZ/F");
  outTreePtOrd->Branch("flightLengthX",&flightLengthX_,"flightLengthX/F");
  outTreePtOrd->Branch("flightLengthY",&flightLengthY_,"flightLengthY/F");
  outTreePtOrd->Branch("flightLengthZ",&flightLengthZ_,"flightLengthZ/F");
  outTreePtOrd->Branch("pvX",&pvX_,"pvX/F");
  outTreePtOrd->Branch("pvY",&pvY_,"pvY/F");
  outTreePtOrd->Branch("pvZ",&pvZ_,"pvZ/F");
  outTreePtOrd->Branch("pvCov00",&pvCov00_,"pvCov00/F");
  outTreePtOrd->Branch("pvCov01",&pvCov01_,"pvCov01/F");
  outTreePtOrd->Branch("pvCov02",&pvCov02_,"pvCov02/F");
  outTreePtOrd->Branch("pvCov11",&pvCov11_,"pvCov11/F");
  outTreePtOrd->Branch("pvCov12",&pvCov12_,"pvCov12/F");
  outTreePtOrd->Branch("pvCov22",&pvCov22_,"pvCov22/F");
  outTreePtOrd->Branch("svX",&svX_,"svX/F");
  outTreePtOrd->Branch("svY",&svY_,"svY/F");
  outTreePtOrd->Branch("svZ",&svZ_,"svZ/F");
  outTreePtOrd->Branch("svCov00",&svCov00_,"svCov00/F");
  outTreePtOrd->Branch("svCov01",&svCov01_,"svCov01/F");
  outTreePtOrd->Branch("svCov02",&svCov02_,"svCov02/F");
  outTreePtOrd->Branch("svCov11",&svCov11_,"svCov11/F");
  outTreePtOrd->Branch("svCov12",&svCov12_,"svCov12/F");
  outTreePtOrd->Branch("svCov22",&svCov22_,"svCov22/F");

  outTreePtOrd->Branch("numPV",              &numPV_,        "numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,  "sampleWeight/F"); 
  outTreePtOrd->Branch("sampleWeightW",      &sampleWeightW, "sampleWeightW/F"); 
  outTreePtOrd->Branch("sampleWeightDY",     &sampleWeightDY,"sampleWeightDY/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,      "puWeight/F");
  outTreePtOrd->Branch("puWeightHCP",        &puWeightHCP,   "puWeightHCP/F");
  outTreePtOrd->Branch("puWeightD",          &puWeightD,     "puWeightD/F");
  outTreePtOrd->Branch("puWeightDLow",       &puWeightDLow,  "puWeightDLow/F");
  outTreePtOrd->Branch("puWeightDHigh",      &puWeightDHigh, "puWeightDHigh/F");
  outTreePtOrd->Branch("puWeight2",          &puWeight2,     "puWeight2/F");

  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("embeddingFilterEffWeight",&embeddingFilterEffWeight_,"embeddingFilterEffWeight/F");//IN
  outTreePtOrd->Branch("TauSpinnerWeight",   &TauSpinnerWeight_,"TauSpinnerWeight/F");//IN
  outTreePtOrd->Branch("ZmumuEffWeight",     &ZmumuEffWeight_,"ZmumuEffWeight/F");//IN
  outTreePtOrd->Branch("diTauMassVSdiTauPtWeight",&diTauMassVSdiTauPtWeight_,"diTauMassVSdiTauPtWeight/F");//IN
  outTreePtOrd->Branch("tau2EtaVStau1EtaWeight",&tau2EtaVStau1EtaWeight_,"tau2EtaVStau1EtaWeight/F");//IN
  outTreePtOrd->Branch("tau2PtVStau1PtWeight",&tau2PtVStau1PtWeight_,"tau2PtVStau1PtWeight/F");//IN
  outTreePtOrd->Branch("muonRadiationWeight",&muonRadiationWeight_,"muonRadiationWeight/F");//IN
  outTreePtOrd->Branch("muonRadiationDownWeight",&muonRadiationDownWeight_,"muonRadiationDownWeight/F");//IN
  outTreePtOrd->Branch("muonRadiationUpWeight",&muonRadiationUpWeight_,"muonRadiationUpWeight/F");//IN

  outTreePtOrd->Branch("weightHepNup",         &weightHepNup,         "weightHepNup/F");
  outTreePtOrd->Branch("weightHepNupHighStatW",&weightHepNupHighStatW,"weightHepNupHighStatW/F");
  outTreePtOrd->Branch("weightHepNupDY",     &weightHepNupDY,"weightHepNupDY/F");//IN
  outTreePtOrd->Branch("highPtWeightUp",     &highPtWeightUp,"highPtWeightUp/F");//IN
  outTreePtOrd->Branch("highPtWeightDown",   &highPtWeightDown,"highPtWeightDown/F");//IN
  outTreePtOrd->Branch("HqTWeight",          &HqTWeight,"HqTWeight/F");
  outTreePtOrd->Branch("HqTWeightUp",          &HqTWeightUp,"HqTWeightUp/F");
  outTreePtOrd->Branch("HqTWeightDown",          &HqTWeightDown,"HqTWeightDown/F");

  //Top pT weights
  outTreePtOrd->Branch("topPtWeightNom",          &topPtWeightNom_,"topPtWeightNom/F");
  outTreePtOrd->Branch("topPtWeightUp",           &topPtWeightUp_,"topPtWeightUp/F");
  outTreePtOrd->Branch("topPtWeightDown",         &topPtWeightDown_,"topPtWeightDown/F");

  //Higgs pT weight MSSM

  //mhmax - nominal
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax", &mssmHiggsPtReweightGluGlu_mhmax, "mssmHiggsPtReweightGluGlu_mhmax/F");
  //mhmax - HqT up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_HqTUp", &mssmHiggsPtReweightGluGlu_mhmax_HqTUp, "mssmHiggsPtReweightGluGlu_mhmax_HqTUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_HqTDown", &mssmHiggsPtReweightGluGlu_mhmax_HqTDown, "mssmHiggsPtReweightGluGlu_mhmax_HqTDown/F");
  //mhmax - HIGLU up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp", &mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp, "mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown", &mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown, "mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown/F");
  //mhmax - tanBeta up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp", &mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp, "mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown", &mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown, "mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown/F");

  //mhmodplus - nominal
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus", &mssmHiggsPtReweightGluGlu_mhmodplus, "mssmHiggsPtReweightGluGlu_mhmodplus/F");
  //mhmodplus - HqT up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp", &mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp, "mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown", &mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown, "mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown/F");
  //mhmodplus - HIGLU up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp", &mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp, "mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown", &mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown, "mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown/F");
  //mhmodplus - tanBeta up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp", &mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp, "mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown", &mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown, "mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown/F");

  //mhmodminus - nominal
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus", &mssmHiggsPtReweightGluGlu_mhmodminus, "mssmHiggsPtReweightGluGlu_mhmodminus/F");
  //mhmodminus - HqT up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp", &mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp, "mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown", &mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown, "mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown/F");
  //mhmodminus - HIGLU up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp", &mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp, "mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown", &mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown, "mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown/F");
  //mhmodminus - tanBeta up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp", &mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp, "mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown", &mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown, "mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown/F");

  //lowmH - nominal
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH", &mssmHiggsPtReweightGluGlu_lowmH, "mssmHiggsPtReweightGluGlu_lowmH/F");
  //lowmH - HqT up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_HqTUp", &mssmHiggsPtReweightGluGlu_lowmH_HqTUp, "mssmHiggsPtReweightGluGlu_lowmH_HqTUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_HqTDown", &mssmHiggsPtReweightGluGlu_lowmH_HqTDown, "mssmHiggsPtReweightGluGlu_lowmH_HqTDown/F");
  //lowmH - HIGLU up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp", &mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp, "mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown", &mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown, "mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown/F");
  //lowmH - tanBeta up/down
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp", &mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp, "mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp/F");
  outTreePtOrd->Branch("mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown", &mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown, "mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown/F");

  outTreePtOrd->Branch("ZmmWeight",          &ZmmWeight,"ZmmWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");
  outTreePtOrd->Branch("nPUVertices",        &nPUVertices_, "nPUVertices/F");

  outTreePtOrd->Branch("HLTx",         &HLTx, "HLTx/F");
  outTreePtOrd->Branch("HLTIsoMu24",         &HLTIsoMu24, "HLTIsoMu24/F");
  outTreePtOrd->Branch("HLTIsoMu24_eta2p1",         &HLTIsoMu24_eta2p1, "HLTIsoMu24_eta2p1/F");
  outTreePtOrd->Branch("HLTPFJet320",         &HLTPFJet320, "HLTPFJet320/F");
  outTreePtOrd->Branch("HLTmatchMu",   &HLTmatchMu,"HLTmatchMu/F");
  outTreePtOrd->Branch("HLTmatchTau",  &HLTmatchTau,"HLTmatchTau/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTxQCD",      &HLTxQCD, "HLTxQCD/F"); 
  outTreePtOrd->Branch("HLTmatchQCD",  &HLTmatchQCD,"HLTmatchQCD/F"); 
  outTreePtOrd->Branch("HLTxDiTau",    &HLTxDiTau, "HLTxDiTau/F");
  outTreePtOrd->Branch("HLTmatchDiTauMu",   &HLTmatchDiTauMu,"HLTmatchDiTauMu/F"); 
  outTreePtOrd->Branch("HLTmatchDiTauTau",  &HLTmatchDiTauTau,"HLTmatchDiTauTau/F"); 
  outTreePtOrd->Branch("HLTmatchDiTau",     &HLTmatchDiTau,"HLTmatchDiTau/F");
  outTreePtOrd->Branch("HLTmatchDiTauL1",   &HLTmatchDiTauL1,"HLTmatchDiTauL1/F");
  outTreePtOrd->Branch("HLTmatchDiTauL2",   &HLTmatchDiTauL2,"HLTmatchDiTauL2/F");
  outTreePtOrd->Branch("HLTmatchIsoMu24",   &HLTmatchIsoMu24,"HLTmatchIsoMu24/F");
  outTreePtOrd->Branch("HLTmatchIsoMu24_eta2p1",   &HLTmatchIsoMu24_eta2p1,"HLTmatchIsoMu24_eta2p1/F");

  outTreePtOrd->Branch("HLTxSoft",         &HLTxSoft,       "HLTxSoft/F");
  outTreePtOrd->Branch("HLTmatchSoft",     &HLTmatchSoft,   "HLTmatchSoft/F");
  outTreePtOrd->Branch("HLTxQCDSoft",      &HLTxQCDSoft,    "HLTxQCDSoft/F"); 
  outTreePtOrd->Branch("HLTmatchQCDSoft",  &HLTmatchQCDSoft,"HLTmatchQCDSoft/F"); 

  outTreePtOrd->Branch("HLTxIsoMu8Tau20",       &HLTxIsoMu8Tau20,       "HLTxIsoMu8Tau20/F");
  outTreePtOrd->Branch("HLTmatchIsoMu8Tau20",   &HLTmatchIsoMu8Tau20,   "HLTmatchIsoMu8Tau20/F");
  outTreePtOrd->Branch("HLTxIsoMu15ETM20",      &HLTxIsoMu15ETM20,      "HLTxIsoMu15ETM20/F");
  outTreePtOrd->Branch("HLTmatchIsoMu15ETM20",  &HLTmatchIsoMu15ETM20,  "HLTmatchIsoMu15ETM20/F");

  outTreePtOrd->Branch("HLTxMu17Mu8", &HLTxMu17Mu8, "HLTxMu17Mu8/F");

  // muons
  outTreePtOrd->Branch("HLTweightMu",  &HLTweightMu,"HLTweightMu/F");
  outTreePtOrd->Branch("HLTweightMuA", &HLTweightMuA,"HLTweightMuA/F");
  outTreePtOrd->Branch("HLTweightMuB", &HLTweightMuB,"HLTweightMuB/F");
  outTreePtOrd->Branch("HLTweightMuC", &HLTweightMuC,"HLTweightMuC/F");
  outTreePtOrd->Branch("HLTweightMuD", &HLTweightMuD,"HLTweightMuD/F");
  outTreePtOrd->Branch("HLTweightMuABC", &HLTweightMuABC,"HLTweightMuABC/F");
  outTreePtOrd->Branch("HLTweightMuABCD", &HLTweightMuABCD,"HLTweightMuABCD/F");
  outTreePtOrd->Branch("HLTweightMuSoft",&HLTweightMuSoft,"HLTweightMuSoft/F"); //MB
  outTreePtOrd->Branch("HLTweightMuShift",&HLTweightMuShift,"HLTweightMuShift/F"); //MB
  outTreePtOrd->Branch("HLTweightMuABCShift",&HLTweightMuABCShift,"HLTweightMuABCShift/F"); //MB
  //
  outTreePtOrd->Branch("HLTMu",  &HLTMu,"HLTMu/F");
  outTreePtOrd->Branch("HLTMuA", &HLTMuA,"HLTMuA/F");
  outTreePtOrd->Branch("HLTMuB", &HLTMuB,"HLTMuB/F");
  outTreePtOrd->Branch("HLTMuC", &HLTMuC,"HLTMuC/F");
  outTreePtOrd->Branch("HLTMuD", &HLTMuD,"HLTMuD/F");
  outTreePtOrd->Branch("HLTMuABC", &HLTMuABC,"HLTMuABC/F");
  outTreePtOrd->Branch("HLTMuABCD", &HLTMuABCD,"HLTMuABCD/F");
  outTreePtOrd->Branch("HLTMuSoft",&HLTMuSoft,"HLTMuSoft/F"); //MB
  outTreePtOrd->Branch("HLTMuSoftMC",&HLTMuSoftMC,"HLTMuSoftMC/F"); //MB
  outTreePtOrd->Branch("HLTMuShift",&HLTMuShift,"HLTMuShift/F"); //MB
  outTreePtOrd->Branch("HLTMuShiftMC",&HLTMuShiftMC,"HLTMuShiftMC/F"); //MB
  outTreePtOrd->Branch("HLTMuABCShift",&HLTMuABCShift,"HLTMuABCShift/F"); //MB
  outTreePtOrd->Branch("HLTMuShiftMCold",&HLTMuShiftMCold,"HLTMuShiftMCold/F"); //MB
  //
  outTreePtOrd->Branch("SFMu",         &SFMu,     "SFMu/F");
  outTreePtOrd->Branch("SFMu_ABC",     &SFMu_ABC, "SFMu_ABC/F");
  outTreePtOrd->Branch("SFMu_D",       &SFMu_D,   "SFMu_D/F");
  outTreePtOrd->Branch("SFMu_ABCD",    &SFMu_ABCD,"SFMu_ABCD/F");
  outTreePtOrd->Branch("SFMuID",         &SFMuID,     "SFMuID/F");
  outTreePtOrd->Branch("SFMuID_ABC",     &SFMuID_ABC, "SFMuID_ABC/F");
  outTreePtOrd->Branch("SFMuID_D",       &SFMuID_D,   "SFMuID_D/F");
  outTreePtOrd->Branch("SFMuID_ABCD",    &SFMuID_ABCD,"SFMuID_ABCD/F");
  outTreePtOrd->Branch("SFMuIso",         &SFMuIso,     "SFMuIso/F");
  outTreePtOrd->Branch("SFMuIso_ABC",     &SFMuIso_ABC, "SFMuIso_ABC/F");
  outTreePtOrd->Branch("SFMuIso_D",       &SFMuIso_D,   "SFMuIso_D/F");
  outTreePtOrd->Branch("SFMuIso_ABCD",    &SFMuIso_ABCD,"SFMuIso_ABCD/F");

  // taus
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTweight2Triggers", &HLTweight2Triggers,"HLTweight2Triggers/F");
  outTreePtOrd->Branch("HLTweightTauD",&HLTweightTauD,"HLTweightTauD/F");
  outTreePtOrd->Branch("HLTweightTauABC",&HLTweightTauABC,"HLTweightTauABC/F");
  outTreePtOrd->Branch("HLTTau",       &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("HLTTauD",      &HLTTauD,"HLTTauD/F");
  outTreePtOrd->Branch("HLTTauABC",    &HLTTauABC,"HLTTauABC/F");
  outTreePtOrd->Branch("HLTTauMC",       &HLTTauMC,"HLTTauMC/F");
  outTreePtOrd->Branch("HLTTauMCD",      &HLTTauMCD,"HLTTauMCD/F");
  outTreePtOrd->Branch("HLTTauMCABC",    &HLTTauMCABC,"HLTTauMCABC/F");
  //
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("weightDecayMode",  &weightDecayMode_, "weightDecayMode/F");
  outTreePtOrd->Branch("weightTauFakeWJet", &weightTauFakeWJet_, "weightTauFakeWJet/F");
  outTreePtOrd->Branch("weightTauFakeWJetUp", &weightTauFakeWJetUp_, "weightTauFakeWJetUp/F");
  outTreePtOrd->Branch("weightTauFakeWJetDown", &weightTauFakeWJetDown_, "weightTauFakeWJetDown/F");

  //jet->tau fake correction for antiiso events in the QCD estimation
  outTreePtOrd->Branch("weightJetFakeQCD", &weightJetFakeQCD_, "weightJetFakeQCD/F");
  //
  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("isTauLegMatchedToLep", &isTauLegMatchedToLep_,"isTauLegMatchedToLep/I");
  outTreePtOrd->Branch("muFlag",          &muFlag_,"muFlag/I"); 
  outTreePtOrd->Branch("isPFMuon",        &isPFMuon_,"isPFMuon/I"); 
  outTreePtOrd->Branch("isTightMuon",     &isTightMuon_,"isTightMuon/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");
  outTreePtOrd->Branch("vetoEventOld",    &vetoEventOld_, "vetoEventOld/I");
  outTreePtOrd->Branch("vetoEventNew",    &vetoEventNew_, "vetoEventNew/I");

  outTreePtOrd->Branch("parton", &parton_,"parton/I");
  outTreePtOrd->Branch("genPartMult", &genPartMult_,"genPartMult/I");
  outTreePtOrd->Branch("leadGenPartPdg", &leadGenPartPdg_,"leadGenPartPdg/I");
  outTreePtOrd->Branch("hepNUP", &hepNUP_,"hepNUP/I");
  outTreePtOrd->Branch("leadGenPartPt", &leadGenPartPt_,"leadGenPartPt/F");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
  outTreePtOrd->Branch("index", &index_, "index/I");
  outTreePtOrd->Branch("pairIndex", &pairIndex, "pairIndex/I");

  outTreePtOrd->Branch("uParl", &uParl, "uParl/F");
  outTreePtOrd->Branch("uPerp", &uPerp, "uPerp/F");
  outTreePtOrd->Branch("metParl", &metParl, "metParl/F");
  outTreePtOrd->Branch("metPerp", &metPerp, "metPerp/F");
  outTreePtOrd->Branch("metSigmaParl", &metSigmaParl, "metSigmaParl/F");
  outTreePtOrd->Branch("metSigmaPerp", &metSigmaPerp, "metSigmaPerp/F");

  int nEntries    = currentTree->GetEntries() ;
  float crossSection = xsec_;
  float scaleFactor = (crossSection != 0) ? Lumi / (  float(nEventsRead)/(crossSection*skimEff_) )  : 1.0;

  int nProc,n1,n2;
  //cout<<"nDiv = "<<nDiv<<endl;
  //cout<<"nEntries = "<<nEntries<<endl;
  nProc = nEntries/ nDiv ;
  n1 = iDiv * nProc ;
  if( iDiv < (nDiv-1) )
    n2 = (iDiv+1) * nProc ;
  else if( iDiv == nDiv-1 )
    n2 = nEntries;
  
  //cout<<"n1 = "<<n1<<endl;
  //cout<<"n2 = "<<n2<<endl;

  TString sample(sample_.c_str());
  cout << "Processing sample " << sample << endl;
  cout<< "nEventsRead = " << nEventsRead << endl;
  cout<< "nEntries    = " << nEntries << endl;
  cout<< "crossSection " << crossSection << " pb ==> scaleFactor " << scaleFactor << endl;

  // jets
//   currentTree->SetBranchStatus("l1JetsP4"  ,1);
//   currentTree->SetBranchStatus("caloJetsMatchedP4"  ,1);
  currentTree->SetBranchStatus("jetsP4"                ,0);
  currentTree->SetBranchStatus("jetsIDP4"              ,1);
  currentTree->SetBranchStatus("jetsIDUpP4"            ,1);
  currentTree->SetBranchStatus("jetsIDDownP4"          ,1);
  //currentTree->SetBranchStatus("jetsIDL1OffsetP4"      ,1);
  currentTree->SetBranchStatus("genJetsIDP4"           ,0);
  currentTree->SetBranchStatus("jetsBtagHE"            ,1);
  currentTree->SetBranchStatus("jetsBtagHP"            ,1);
  currentTree->SetBranchStatus("jetsBtagCSV"           ,1);
  currentTree->SetBranchStatus("jetMoments"            ,0);
  currentTree->SetBranchStatus("jetsChNfraction"       ,1);
  currentTree->SetBranchStatus("jetsChEfraction"       ,0);
  currentTree->SetBranchStatus("jetPUMVA"              ,1);
  currentTree->SetBranchStatus("jetPUWP"               ,1);
  currentTree->SetBranchStatus("jetQuarkGluon"         ,1);
  currentTree->SetBranchStatus("jetQuarkGluonGen"      ,1);
  currentTree->SetBranchStatus("bQuark"                ,1);
  
  // diTaus
  currentTree->SetBranchStatus("diTauVisP4"            ,1);
  currentTree->SetBranchStatus("diTauCAP4"             ,0);
  currentTree->SetBranchStatus("diTauICAP4"            ,1); 
  currentTree->SetBranchStatus("diTauSVfitP4"          ,1);
  currentTree->SetBranchStatus("diTauNSVfitMass"       ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrUp"  ,1);
  currentTree->SetBranchStatus("diTauNSVfitMassErrDown",1);
  currentTree->SetBranchStatus("diTauNSVfitPt"         ,1);
  currentTree->SetBranchStatus("diTauNSVfitPtErrUp"    ,1);
  currentTree->SetBranchStatus("diTauNSVfitPtErrDown"  ,1);
  currentTree->SetBranchStatus("mTauTauMin"            ,1);
  currentTree->SetBranchStatus("diTauCharge"           ,1);
  currentTree->SetBranchStatus("chargeL1"              ,1);

  // taus
  currentTree->SetBranchStatus("diTauLegsP4"           ,1);
  currentTree->SetBranchStatus("genDiTauLegsP4"        ,1);
  currentTree->SetBranchStatus("genDiTauMass"          ,1);
  currentTree->SetBranchStatus("genTausP4"             ,1);
  currentTree->SetBranchStatus("genTausDecayLeptonically",1);
  currentTree->SetBranchStatus("genTausCharge",1);

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
//   currentTree->SetBranchStatus("isoLeg1MVA"            ,1);

  currentTree->SetBranchStatus("chIsoLeg2"             ,1);
  currentTree->SetBranchStatus("nhIsoLeg2"             ,0);
  currentTree->SetBranchStatus("phIsoLeg2"             ,1);

  currentTree->SetBranchStatus("dxy1"                  ,1);
  currentTree->SetBranchStatus("dxy2"                  ,1);
  currentTree->SetBranchStatus("dz1"                   ,1);
  currentTree->SetBranchStatus("dz2"                   ,0);

  //TauID
  currentTree->SetBranchStatus("decayMode"                 ,1);
  currentTree->SetBranchStatus("decayModeFinding"          ,1);
  currentTree->SetBranchStatus("decayModeFindingNewDM"     ,1);
  currentTree->SetBranchStatus("decayModeFindingOldDM"     ,1);
  currentTree->SetBranchStatus("AntiEDeadEcal"             ,1);
  currentTree->SetBranchStatus("tightestAntiECutWP"        ,1);
  currentTree->SetBranchStatus("tightestAntiEMVA5WP"       ,1);
  currentTree->SetBranchStatus("AntiEMVA5category"         ,1);
  currentTree->SetBranchStatus("AntiEMVA5raw"              ,1);
  currentTree->SetBranchStatus("tightestAntiMuWP"          ,1);
  currentTree->SetBranchStatus("tightestAntiMu2WP"         ,1);
  currentTree->SetBranchStatus("tightestAntiMu3WP"         ,1);
  currentTree->SetBranchStatus("tightestAntiMuMVAWP"       ,1);
  currentTree->SetBranchStatus("AntiMuMVAraw"              ,1);
  currentTree->SetBranchStatus("tightestHPSDBWP"           ,1);
  currentTree->SetBranchStatus("tightestHPSDB3HWP"         ,1);
  currentTree->SetBranchStatus("hpsDB3H"                   ,1);
  currentTree->SetBranchStatus("tightestHPSMVA3newDMwLTWP" ,1);
  currentTree->SetBranchStatus("hpsMVA3newDMwLT"           ,1);
  currentTree->SetBranchStatus("tightestHPSMVA3newDMwoLTWP",1);
  currentTree->SetBranchStatus("hpsMVA3newDMwoLT"          ,1);
  currentTree->SetBranchStatus("tightestHPSMVA3oldDMwLTWP" ,1);
  currentTree->SetBranchStatus("hpsMVA3oldDMwLT"           ,1);
  currentTree->SetBranchStatus("tightestHPSMVA3oldDMwoLTWP",1);
  currentTree->SetBranchStatus("hpsMVA3oldDMwoLT"          ,1);
  ///NewTauID input variables
  currentTree->SetBranchStatus("chargedIsoPtSum"           ,1);
  currentTree->SetBranchStatus("neutralIsoPtSum"           ,1);
  currentTree->SetBranchStatus("puCorrPtSum"               ,1);
  currentTree->SetBranchStatus("patDecayMode"              ,1);
  currentTree->SetBranchStatus("dxyTau"                    ,1);
  currentTree->SetBranchStatus("dxyErrTau"                 ,1);
  currentTree->SetBranchStatus("dxySigTau"                 ,1);
  currentTree->SetBranchStatus("hasSecVtx"                 ,1);
  currentTree->SetBranchStatus("flightLength2"             ,1);
  currentTree->SetBranchStatus("flightLengthSig"           ,1);
  ///NewTauID useful variables
  currentTree->SetBranchStatus("dxyPCAX"                   ,1);
  currentTree->SetBranchStatus("dxyPCAY"                   ,1);
  currentTree->SetBranchStatus("dxyPCAZ"                   ,1);
  currentTree->SetBranchStatus("flightLengthX"             ,1);
  currentTree->SetBranchStatus("flightLengthY"             ,1);
  currentTree->SetBranchStatus("flightLengthZ"             ,1);
  currentTree->SetBranchStatus("pvX"                       ,1);
  currentTree->SetBranchStatus("pvY"                       ,1);
  currentTree->SetBranchStatus("pvZ"                       ,1);
  currentTree->SetBranchStatus("pvCov00"                   ,1);
  currentTree->SetBranchStatus("pvCov01"                   ,1);
  currentTree->SetBranchStatus("pvCov02"                   ,1);
  currentTree->SetBranchStatus("pvCov11"                   ,1);
  currentTree->SetBranchStatus("pvCov12"                   ,1);
  currentTree->SetBranchStatus("pvCov22"                   ,1);
  currentTree->SetBranchStatus("svX"                       ,1);
  currentTree->SetBranchStatus("svY"                       ,1);
  currentTree->SetBranchStatus("svZ"                       ,1);
  currentTree->SetBranchStatus("svCov00"                   ,1);
  currentTree->SetBranchStatus("svCov01"                   ,1);
  currentTree->SetBranchStatus("svCov02"                   ,1);
  currentTree->SetBranchStatus("svCov11"                   ,1);
  currentTree->SetBranchStatus("svCov12"                   ,1);
  currentTree->SetBranchStatus("svCov22"                   ,1);

  currentTree->SetBranchStatus("visibleTauMass"        ,1);
  currentTree->SetBranchStatus("visibleGenTauMass"     ,1);
  currentTree->SetBranchStatus("genDecayMode"          ,1);
  currentTree->SetBranchStatus("isTauLegMatched"       ,1);
  currentTree->SetBranchStatus("isTauLegMatchedToLep"  ,1);
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
  currentTree->SetBranchStatus("MtLeg1"                ,0);
  currentTree->SetBranchStatus("pZeta"                 ,0);
  currentTree->SetBranchStatus("pZetaVis"              ,0);
  currentTree->SetBranchStatus("pZetaSig"              ,1);
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
  currentTree->SetBranchStatus("extraMuons"            ,0);
  currentTree->SetBranchStatus("pfMuons"               ,0);
  currentTree->SetBranchStatus("muFlag"                ,1);
  currentTree->SetBranchStatus("isPFMuon"              ,1);
  currentTree->SetBranchStatus("isTightMuon"           ,1);
  currentTree->SetBranchStatus("vetoEvent"             ,1);

  // leptons for veto
  if(doLepVeto) {
    currentTree->SetBranchStatus("vetoMuonsP4"        ,1);
    currentTree->SetBranchStatus("vetoElectronsP4"    ,1);
    currentTree->SetBranchStatus("vetoElectronsID"    ,1);//NewEleID
  // currentTree->SetBranchStatus("vetoTausP4"         ,1);
  }

  // event-dependent variables
  currentTree->SetBranchStatus("rhoFastJet"            ,1);
  currentTree->SetBranchStatus("rhoNeutralFastJet"     ,1);
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

  if(RERECO) currentTree->SetBranchStatus("embeddingWeights"      ,1);//IN

  currentTree->SetBranchStatus("index"                 ,1);

  //Top pT reweighting SM // IN
  if( sample_.find("TTJets")!=string::npos ) 
    {
      currentTree->SetBranchStatus("topPtWeightNom"       ,1);
      currentTree->SetBranchStatus("topPtWeightUp"        ,1);
      currentTree->SetBranchStatus("topPtWeightDown"      ,1);
    }
  // triggers
//   currentTree->SetBranchStatus("tauXTriggers"          ,1);
//   currentTree->SetBranchStatus("triggerBits"           ,1);
  currentTree->SetBranchStatus("triggerPaths"          ,1);
  currentTree->SetBranchStatus("HLTfiltersMu"          ,1);
  currentTree->SetBranchStatus("HLTfiltersTau"         ,1);
  currentTree->SetBranchStatus("HLTfiltersTauJet"      ,1);
//   currentTree->SetBranchStatus("trgTaus"               ,1);//MB
//   currentTree->SetBranchStatus("trgTauId"              ,1);//MB

  cout << "SetBranchStatus done" << endl;

  ////////////////////////////////////////////////////////////////////
  std::vector< double >* embeddingWeights = new std::vector< double >();//IN
  if(RERECO) {
    currentTree->SetBranchAddress("embeddingWeights",     &embeddingWeights);//IN
  }
  std::vector< LV >* jets           = new std::vector< LV >();

  if(analysis_.find("JetUp")!=string::npos) 
    currentTree->SetBranchAddress("jetsIDUpP4",        &jets);
  else if(analysis_.find("JetDown")!=string::npos)
    currentTree->SetBranchAddress("jetsIDDownP4",      &jets);
  else
    currentTree->SetBranchAddress("jetsIDP4",          &jets);

  /*
  std::vector< LV >* jets_v2        = new std::vector< LV >();
  currentTree->SetBranchAddress("jetsIDL1OffsetP4",&jets_v2);
  */

  std::vector< float >* jetPUMVA =  new std::vector< float >();
  currentTree->SetBranchAddress("jetPUMVA", &jetPUMVA);

  std::vector< float >* jetPUWP =  new std::vector< float >();
  currentTree->SetBranchAddress("jetPUWP", &jetPUWP);

  std::vector< float >* jetQuarkGluon =  new std::vector< float >();
  currentTree->SetBranchAddress("jetQuarkGluon", &jetQuarkGluon);

  std::vector< float >* jetQuarkGluonGen =  new std::vector< float >();
  currentTree->SetBranchAddress("jetQuarkGluonGen", &jetQuarkGluonGen);

//   std::vector< LV >* caloJetsMatchedP4    = new std::vector< LV >();
//   currentTree->SetBranchAddress("caloJetsMatchedP4",     &caloJetsMatchedP4);

//   std::vector< LV >* l1JetsP4    = new std::vector< LV >();
//   currentTree->SetBranchAddress("l1JetsP4",     &l1JetsP4);

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

  std::vector< LV >* genTausP4 = new std::vector< LV >();
  currentTree->SetBranchAddress("genTausP4",    &genTausP4);

  std::vector< int >* genTausDecayLeptonically = new std::vector < int >();
  currentTree->SetBranchAddress("genTausDecayLeptonically",    &genTausDecayLeptonically);

  std::vector< int >* genTausCharge = new std::vector < int >();
  currentTree->SetBranchAddress("genTausCharge",    &genTausCharge);


  std::vector< LV >* genVP4         = new std::vector< LV >();
  currentTree->SetBranchAddress("genVP4",          &genVP4);

  std::vector< LV >* METP4          = new std::vector< LV >();
  currentTree->SetBranchAddress("METP4",           &METP4);

  std::vector< LV >* l1ETMP4        = new std::vector< LV >();//MB
  currentTree->SetBranchAddress("l1ETMP4",         &l1ETMP4); //MB

  std::vector< LV >* caloMETNoHFP4      = new std::vector< LV >();//MB
  currentTree->SetBranchAddress("caloMETNoHFP4",       &caloMETNoHFP4); //MB

  std::vector< LV >* genMETP4          = new std::vector< LV >();
  currentTree->SetBranchAddress("genMETP4",        &genMETP4);

  std::map< string,int >* triggerPaths = new std::map< string, int >();
  currentTree->SetBranchAddress("triggerPaths",    &triggerPaths);
  std::map< string,int >* HLTfiltersMu = new std::map< string, int >();
  currentTree->SetBranchAddress("HLTfiltersMu",    &HLTfiltersMu);
  std::map< string, int >* HLTfiltersTau = new std::map< string, int >();
  currentTree->SetBranchAddress("HLTfiltersTau",    &HLTfiltersTau);
  std::map< string, int >* HLTfiltersTauJet = new std::map< string, int >();
  currentTree->SetBranchAddress("HLTfiltersTauJet",    &HLTfiltersTauJet);

//   std::vector< int >* tauXTriggers  = new std::vector< int >();
//   currentTree->SetBranchAddress("tauXTriggers",    &tauXTriggers);
//   std::vector< int >* triggerBits   = new std::vector< int >();
//   currentTree->SetBranchAddress("triggerBits",     &triggerBits);

//   std::vector< LV >* trgTaus        = new std::vector< LV >();//MB
//   currentTree->SetBranchAddress("trgTaus",         &trgTaus);//MB
//   std::vector< int >* trgTauId   = new std::vector< int >();//MB
//   currentTree->SetBranchAddress("trgTauId",        &trgTauId);//MB

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
  std::vector< int >* vetoElectronsID = new std::vector< int >();//NewEleID 
  std::vector< LV >* vetoMuonsP4 = new std::vector< LV >();
  if(doLepVeto) {
    currentTree->SetBranchAddress("vetoElectronsP4", &vetoElectronsP4);
    currentTree->SetBranchAddress("vetoElectronsID", &vetoElectronsID);//NewEleID
    currentTree->SetBranchAddress("vetoMuonsP4",     &vetoMuonsP4); 
    //std::vector< LV >* vetoTausP4 = new std::vector< LV >();
  }

  // auxiliary float to store branch values
  float diTauNSVfitMass,diTauNSVfitMassErrUp,diTauNSVfitMassErrDown,
    diTauNSVfitPt,diTauNSVfitPtErrUp,diTauNSVfitPtErrDown,mTauTauMin;
  float genDiTauMass;
  float diTauCharge, chargeL1;
  //TauID
  int genDecayMode,decayMode,decayModeFinding,decayModeFindingNewDM,decayModeFindingOldDM;
  int AntiEDeadEcal,tightestAntiECutWP,tightestAntiEMVA5WP,AntiEMVA5category;
  int tightestAntiMuWPtightestAntiMu2WP,tightestAntiMu3WP,tightestAntiMuMVAWP;
  float AntiEMVA5raw,AntiMuMVAraw;
  int tightestHPSDBWP,tightestHPSDB3HWP,tightestHPSMVA3newDMwLTWP,tightestHPSMVA3newDMwoLTWP,tightestHPSMVA3oldDMwLTWP,tightestHPSMVA3oldDMwoLTWP;
  float hpsDB3H,hpsMVA3newDMwLT,hpsMVA3newDMwoLT,hpsMVA3oldDMwLT,hpsMVA3oldDMwoLT;
  ///NewTauID input variables
  int patDecayMode,hasSecVtx;
  float chargedIsoPtSum,neutralIsoPtSum,puCorrPtSum,dxyTau,dxyErrTau,dxySigTau,flightLength2,flightLengthSig;
  ///NewTauID useful variables
  float dxyPCAX,dxyPCAY,dxyPCAZ,flightLengthX,flightLengthY,flightLengthZ,
    pvX,pvY,pvZ,pvCov00,pvCov01,pvCov02,pvCov11,pvCov12,pvCov22,
    svX,svY,svZ,svCov00,svCov01,svCov02,svCov11,svCov12,svCov22;

  float numPV;
  int numOfLooseIsoDiTaus;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float pfJetPt;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float mcPUweight,embeddingWeight;
  int isTauLegMatched,isTauLegMatchedToLep,muFlag,isPFMuon,isTightMuon,genDecay, vetoEvent;
  float nPUVertices, nPUVerticesM1, nPUVerticesP1;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass, visibleGenTauMass;
  float dxy1, dxy2, dz1;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1; 
  float allChIsoLeg1;
//   float isoLeg1MVA;
  int parton, genPartMult, leadGenPartPdg, hepNUP; 
  float leadGenPartPt;
  ULong64_t event,run,lumi;
  int index;
  int tightestAntiMuWP, tightestAntiMu2WP; //ND
  float sumEt, caloNoHFsumEt, caloNoHFsumEtCorr; // ND
  //Top pT weights
  float topPtWeightNom,topPtWeightUp,topPtWeightDown;

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
  currentTree->SetBranchAddress("elecIsoLeg1v2",        &allChIsoLeg1);
//   currentTree->SetBranchAddress("isoLeg1MVA",           &isoLeg1MVA);
  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);

  currentTree->SetBranchAddress("dxy1",                 &dxy1);
  currentTree->SetBranchAddress("dxy2",                 &dxy2);
  currentTree->SetBranchAddress("dz1",                  &dz1);

  currentTree->SetBranchAddress("rhoNeutralFastJet",    &rhoNeutralFastJet);

  //TauID
  currentTree->SetBranchAddress("decayModeFinding"          ,&decayModeFinding);
  currentTree->SetBranchAddress("decayModeFindingNewDM"     ,&decayModeFindingNewDM);
  currentTree->SetBranchAddress("decayModeFindingOldDM"     ,&decayModeFindingOldDM);
  currentTree->SetBranchAddress("AntiEDeadEcal"             ,&AntiEDeadEcal);
  currentTree->SetBranchAddress("tightestAntiECutWP"        ,&tightestAntiECutWP);
  currentTree->SetBranchAddress("tightestAntiEMVA5WP"       ,&tightestAntiEMVA5WP);
  currentTree->SetBranchAddress("AntiEMVA5category"         ,&AntiEMVA5category);
  currentTree->SetBranchAddress("AntiEMVA5raw"              ,&AntiEMVA5raw);
  currentTree->SetBranchAddress("tightestAntiMuWP"          ,&tightestAntiMuWP);
  currentTree->SetBranchAddress("tightestAntiMu2WP"         ,&tightestAntiMu2WP);
  currentTree->SetBranchAddress("tightestAntiMu3WP"         ,&tightestAntiMu3WP);
  currentTree->SetBranchAddress("tightestAntiMuMVAWP"       ,&tightestAntiMuMVAWP);
  currentTree->SetBranchAddress("AntiMuMVAraw"              ,&AntiMuMVAraw);
  currentTree->SetBranchAddress("tightestHPSDBWP"           ,&tightestHPSDBWP);
  currentTree->SetBranchAddress("tightestHPSDB3HWP"         ,&tightestHPSDB3HWP);
  currentTree->SetBranchAddress("hpsDB3H"                   ,&hpsDB3H);
  currentTree->SetBranchAddress("tightestHPSMVA3newDMwLTWP" ,&tightestHPSMVA3newDMwLTWP);
  currentTree->SetBranchAddress("hpsMVA3newDMwLT"           ,&hpsMVA3newDMwLT);
  currentTree->SetBranchAddress("tightestHPSMVA3newDMwoLTWP",&tightestHPSMVA3newDMwoLTWP);
  currentTree->SetBranchAddress("hpsMVA3newDMwoLT"          ,&hpsMVA3newDMwoLT);
  currentTree->SetBranchAddress("tightestHPSMVA3oldDMwLTWP" ,&tightestHPSMVA3oldDMwLTWP);
  currentTree->SetBranchAddress("hpsMVA3oldDMwLT"           ,&hpsMVA3oldDMwLT);
  currentTree->SetBranchAddress("tightestHPSMVA3oldDMwoLTWP",&tightestHPSMVA3oldDMwoLTWP);
  currentTree->SetBranchAddress("hpsMVA3oldDMwoLT"          ,&hpsMVA3oldDMwoLT);
  ///NewTauID input variables
  currentTree->SetBranchAddress("chargedIsoPtSum",&chargedIsoPtSum);
  currentTree->SetBranchAddress("neutralIsoPtSum",&neutralIsoPtSum);
  currentTree->SetBranchAddress("puCorrPtSum"    ,&puCorrPtSum);
  currentTree->SetBranchAddress("patDecayMode"   ,&patDecayMode);
  currentTree->SetBranchAddress("dxyTau"         ,&dxyTau);
  currentTree->SetBranchAddress("dxyErrTau"      ,&dxyErrTau);
  currentTree->SetBranchAddress("dxySigTau"      ,&dxySigTau);
  currentTree->SetBranchAddress("hasSecVtx"      ,&hasSecVtx);
  currentTree->SetBranchAddress("flightLength2"  ,&flightLength2);
  currentTree->SetBranchAddress("flightLengthSig",&flightLengthSig);
  ///NewTauID useful variables
  currentTree->SetBranchAddress("dxyPCAX"        ,&dxyPCAX);
  currentTree->SetBranchAddress("dxyPCAY"        ,&dxyPCAY);
  currentTree->SetBranchAddress("dxyPCAZ"        ,&dxyPCAZ);
  currentTree->SetBranchAddress("flightLengthX"  ,&flightLengthX);
  currentTree->SetBranchAddress("flightLengthY"  ,&flightLengthY);
  currentTree->SetBranchAddress("flightLengthZ"  ,&flightLengthZ);
  currentTree->SetBranchAddress("pvX"            ,&pvX);
  currentTree->SetBranchAddress("pvY"            ,&pvY);
  currentTree->SetBranchAddress("pvZ"            ,&pvZ);
  currentTree->SetBranchAddress("pvCov00"        ,&pvCov00);
  currentTree->SetBranchAddress("pvCov01"        ,&pvCov01);
  currentTree->SetBranchAddress("pvCov02"        ,&pvCov02);
  currentTree->SetBranchAddress("pvCov11"        ,&pvCov11);
  currentTree->SetBranchAddress("pvCov12"        ,&pvCov12);
  currentTree->SetBranchAddress("pvCov22"        ,&pvCov22);
  currentTree->SetBranchAddress("svX"            ,&svX);
  currentTree->SetBranchAddress("svY"            ,&svY);
  currentTree->SetBranchAddress("svZ"            ,&svZ);
  currentTree->SetBranchAddress("svCov00"        ,&svCov00);
  currentTree->SetBranchAddress("svCov01"        ,&svCov01);
  currentTree->SetBranchAddress("svCov02"        ,&svCov02);
  currentTree->SetBranchAddress("svCov11"        ,&svCov11);
  currentTree->SetBranchAddress("svCov12"        ,&svCov12);
  currentTree->SetBranchAddress("svCov22"        ,&svCov22);

  currentTree->SetBranchAddress("diTauCharge",          &diTauCharge);
  currentTree->SetBranchAddress("chargeL1",          &chargeL1);

  currentTree->SetBranchAddress("diTauNSVfitMass",       &diTauNSVfitMass);
  currentTree->SetBranchAddress("diTauNSVfitMassErrUp",  &diTauNSVfitMassErrUp);
  currentTree->SetBranchAddress("diTauNSVfitMassErrDown",&diTauNSVfitMassErrDown);
  currentTree->SetBranchAddress("diTauNSVfitPt",       &diTauNSVfitPt);
  currentTree->SetBranchAddress("diTauNSVfitPtErrUp",  &diTauNSVfitPtErrUp);
  currentTree->SetBranchAddress("diTauNSVfitPtErrDown",&diTauNSVfitPtErrDown);
  currentTree->SetBranchAddress("mTauTauMin",            &mTauTauMin);

  currentTree->SetBranchAddress("genDiTauMass",&genDiTauMass);

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
  currentTree->SetBranchAddress("genDecayMode",         &genDecayMode);
  currentTree->SetBranchAddress("numOfLooseIsoDiTaus",  &numOfLooseIsoDiTaus);
  currentTree->SetBranchAddress("muFlag",               &muFlag);
  currentTree->SetBranchAddress("isPFMuon",             &isPFMuon);
  currentTree->SetBranchAddress("isTightMuon",          &isTightMuon);
  currentTree->SetBranchAddress("vetoEvent",            &vetoEvent);
  currentTree->SetBranchAddress("isTauLegMatched",      &isTauLegMatched);
  currentTree->SetBranchAddress("isTauLegMatchedToLep", &isTauLegMatchedToLep);
  currentTree->SetBranchAddress("visibleTauMass",       &visibleTauMass);
  currentTree->SetBranchAddress("visibleGenTauMass",    &visibleGenTauMass);
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
  //For W+NJet re-weighting
  currentTree->SetBranchAddress("parton",               &parton);
  currentTree->SetBranchAddress("genPartMult",          &genPartMult);
  currentTree->SetBranchAddress("leadGenPartPdg",       &leadGenPartPdg);
  currentTree->SetBranchAddress("hepNUP",               &hepNUP);
  currentTree->SetBranchAddress("leadGenPartPt",        &leadGenPartPt);
  //Top pT weights
  if( sample_.find("TTJets")!=string::npos ) 
    {
      currentTree->SetBranchAddress("topPtWeightNom",       &topPtWeightNom);
      currentTree->SetBranchAddress("topPtWeightUp",        &topPtWeightUp);
      currentTree->SetBranchAddress("topPtWeightDown",      &topPtWeightDown);
    }
  cout << "SetBranchAddress done" << endl;

  RecoilCorrector* recoilCorr = 0;

  if( (sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) || 
      sample_.find("W1Jets")!=string::npos || sample_.find("W2Jets")!=string::npos || 
      sample_.find("W3Jets")!=string::npos || sample_.find("W4Jets")!=string::npos
      ){ 
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
  int mH          = 125;
  TH1F* histo     = 0; TH1F* histoUp     = 0; TH1F* histoDown     = 0;
  if(sample_.find("GGFH")!=string::npos && sample_.find("GGFHWW")==string::npos){
    if(sample_.find("GGFH90")!=string::npos) mH = 100;
    if(sample_.find("GGFH95")!=string::npos) mH = 100;
    if(sample_.find("GGFH100")!=string::npos) mH = 100;
    if(sample_.find("GGFH105")!=string::npos) mH = 100;
    if(sample_.find("GGFH110")!=string::npos) mH = 100;
    if(sample_.find("GGFH115")!=string::npos) mH = 125;
    if(sample_.find("GGFH120")!=string::npos) mH = 125;
    if(sample_.find("GGFH125")!=string::npos) mH = 125;
    if(sample_.find("GGFH130")!=string::npos) mH = 125;
    if(sample_.find("GGFH135")!=string::npos) mH = 125;
    if(sample_.find("GGFH140")!=string::npos) mH = 150;
    if(sample_.find("GGFH145")!=string::npos) mH = 150;
    if(sample_.find("GGFH150")!=string::npos) mH = 150;
    if(sample_.find("GGFH155")!=string::npos) mH = 150;
    if(sample_.find("GGFH160")!=string::npos) mH = 150;
    cout << "Reweighting powheg with HqT mH=" << mH << endl;
    HqT = new TFile(Form("../../Utilities/dataForSkimmer/HqTWeights/HRes_weight_pTH_mH%d_8TeV.root", mH));
    if(!HqT) cout << "Cannot find HqT file..." << endl;
    else{
      histo = (TH1F*)(HqT->Get("Nominal"));
      histoUp = (TH1F*)(HqT->Get("Up"));
      histoDown = (TH1F*)(HqT->Get("Down"));
    }
  }
  cout << "Reweighting powheg with HqT mH=" << mH << endl;

  //Filling pT weights for MSSM
  int MSSMmH = 100 ;
  TString SampleT(sample_);
  TFile f_mssmHiggsPtReweightGluGlu_mhmax("../../HadTauStudies/data/mssmHiggsPtReweightGluGlu_mhmax.root");
  TFile f_mssmHiggsPtReweightGluGlu_mhmodplus("../../HadTauStudies/data/mssmHiggsPtReweightGluGlu_mhmod+.root");
  TFile f_mssmHiggsPtReweightGluGlu_mhmodminus("../../HadTauStudies/data/mssmHiggsPtReweightGluGlu_mhmod-.root");
  TFile f_mssmHiggsPtReweightGluGlu_lowmH("../../HadTauStudies/data/mssmHiggsPtReweightGluGlu_low-mH.root");

  TH1D* h_mhmax ;
  TH1D* h_mhmax_HqTUp ;
  TH1D* h_mhmax_HqTDown ;
  TH1D* h_mhmax_HIGLUUp ;
  TH1D* h_mhmax_HIGLUDown ;
  TH1D* h_mhmax_tanBetaUp ;
  TH1D* h_mhmax_tanBetaDown ;

  TH1D* h_mhmodplus ;
  TH1D* h_mhmodplus_HqTUp ;
  TH1D* h_mhmodplus_HqTDown ;
  TH1D* h_mhmodplus_HIGLUUp ;
  TH1D* h_mhmodplus_HIGLUDown ;
  TH1D* h_mhmodplus_tanBetaUp ;
  TH1D* h_mhmodplus_tanBetaDown ;

  TH1D* h_mhmodminus ;
  TH1D* h_mhmodminus_HqTUp ;
  TH1D* h_mhmodminus_HqTDown ;
  TH1D* h_mhmodminus_HIGLUUp ;
  TH1D* h_mhmodminus_HIGLUDown ;
  TH1D* h_mhmodminus_tanBetaUp ;
  TH1D* h_mhmodminus_tanBetaDown ;

  TH1D* h_lowmH ;
  TH1D* h_lowmH_HqTUp ;
  TH1D* h_lowmH_HqTDown ;
  TH1D* h_lowmH_HIGLUUp ;
  TH1D* h_lowmH_HIGLUDown ;
  TH1D* h_lowmH_tanBetaUp ;
  TH1D* h_lowmH_tanBetaDown ;

  if(SampleT.Contains("GGH") && SampleT.Contains("SUSY"))
    {
      if(SampleT.Contains("80")) MSSMmH = 80;
      if(SampleT.Contains("90")) MSSMmH = 90;
      if(SampleT.Contains("100")) MSSMmH = 100;
      if(SampleT.Contains("110")) MSSMmH = 110;
      if(SampleT.Contains("120")) MSSMmH = 120;
      if(SampleT.Contains("130")) MSSMmH = 130;
      if(SampleT.Contains("140")) MSSMmH = 140;
      if(SampleT.Contains("160")) MSSMmH = 160;
      if(SampleT.Contains("180")) MSSMmH = 180;
      if(SampleT.Contains("200")) MSSMmH = 200;
      if(SampleT.Contains("250")) MSSMmH = 250;
      if(SampleT.Contains("300")) MSSMmH = 300;
      if(SampleT.Contains("350")) MSSMmH = 350;
      if(SampleT.Contains("400")) MSSMmH = 400;
      if(SampleT.Contains("450")) MSSMmH = 450;
      if(SampleT.Contains("500")) MSSMmH = 500;
      if(SampleT.Contains("600")) MSSMmH = 600;
      if(SampleT.Contains("700")) MSSMmH = 700;
      if(SampleT.Contains("800")) MSSMmH = 800;
      if(SampleT.Contains("900")) MSSMmH = 900;    
      if(SampleT.Contains("1000")) MSSMmH = 1000;    

      TString HistoName_mhmax = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_central", MSSMmH, MSSMmH);
      TString HistoName_mhmax_HqTUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmax_HqTDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmax_HIGLUUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmax_HIGLUDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmax_tanBetaUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaHigh", MSSMmH, MSSMmH);
      TString HistoName_mhmax_tanBetaDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaLow", MSSMmH, MSSMmH);

      h_mhmax = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax.Data());
      h_mhmax_HqTUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_HqTUp.Data());
      h_mhmax_HqTDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_HqTDown.Data());
      h_mhmax_HIGLUUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_HIGLUUp.Data());
      h_mhmax_HIGLUDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_HIGLUDown.Data());
      h_mhmax_tanBetaUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_tanBetaUp.Data());
      h_mhmax_tanBetaDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmax.Get(HistoName_mhmax_tanBetaDown.Data());

      TString HistoName_mhmodplus = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_central", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_HqTUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_HqTDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_HIGLUUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_HIGLUDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_tanBetaUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaHigh", MSSMmH, MSSMmH);
      TString HistoName_mhmodplus_tanBetaDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaLow", MSSMmH, MSSMmH);

      h_mhmodplus = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus.Data());
      h_mhmodplus_HqTUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_HqTUp.Data());
      h_mhmodplus_HqTDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_HqTDown.Data());
      h_mhmodplus_HIGLUUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_HIGLUUp.Data());
      h_mhmodplus_HIGLUDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_HIGLUDown.Data());
      h_mhmodplus_tanBetaUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_tanBetaUp.Data());
      h_mhmodplus_tanBetaDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodplus.Get(HistoName_mhmodplus_tanBetaDown.Data());

      TString HistoName_mhmodminus = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_central", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_HqTUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_HqTDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HqTscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_HIGLUUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleUp", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_HIGLUDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_HIGLUscaleDown", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_tanBetaUp = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaHigh", MSSMmH, MSSMmH);
      TString HistoName_mhmodminus_tanBetaDown = Form("A_mA%d_mu200/mssmHiggsPtReweight_A_mA%d_mu200_tanBetaLow", MSSMmH, MSSMmH);

      h_mhmodminus = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus.Data());
      h_mhmodminus_HqTUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_HqTUp.Data());
      h_mhmodminus_HqTDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_HqTDown.Data());
      h_mhmodminus_HIGLUUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_HIGLUUp.Data());
      h_mhmodminus_HIGLUDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_HIGLUDown.Data());
      h_mhmodminus_tanBetaUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_tanBetaUp.Data());
      h_mhmodminus_tanBetaDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_mhmodminus.Get(HistoName_mhmodminus_tanBetaDown.Data());

      //lowmH -> 2500
      TString HistoName_lowmH = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_central", 110, 110);
      TString HistoName_lowmH_HqTUp = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_HqTscaleUp", 110, 110);
      TString HistoName_lowmH_HqTDown = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_HqTscaleDown", 110, 110);
      TString HistoName_lowmH_HIGLUUp = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_HIGLUscaleUp", 110, 110);
      TString HistoName_lowmH_HIGLUDown = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_HIGLUscaleDown", 110, 110);
      TString HistoName_lowmH_tanBetaUp = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_tanBetaHigh", 110, 110);
      TString HistoName_lowmH_tanBetaDown = Form("A_mA%d_mu2500/mssmHiggsPtReweight_A_mA%d_mu2500_tanBetaLow", 110, 110);

      h_lowmH = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH.Data());
      h_lowmH_HqTUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_HqTUp.Data());
      h_lowmH_HqTDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_HqTDown.Data());
      h_lowmH_HIGLUUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_HIGLUUp.Data());
      h_lowmH_HIGLUDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_HIGLUDown.Data());
      h_lowmH_tanBetaUp = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_tanBetaUp.Data());
      h_lowmH_tanBetaDown = (TH1D*)f_mssmHiggsPtReweightGluGlu_lowmH.Get(HistoName_lowmH_tanBetaDown.Data());      

    }
  else
    {
      //mhmax
      mssmHiggsPtReweightGluGlu_mhmax = 1. ;
      mssmHiggsPtReweightGluGlu_mhmax_HqTUp = 1. ; mssmHiggsPtReweightGluGlu_mhmax_HqTDown =1. ;
      mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp = 1. ; mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown = 1 ;
      mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp = 1. ; mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown = 1. ;
      //mhmodplus
      mssmHiggsPtReweightGluGlu_mhmodplus = 1. ;
      mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown =1. ;
      mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown = 1 ;
      mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown = 1. ;
      //mhmodminus
      mssmHiggsPtReweightGluGlu_mhmodminus = 1. ;
      mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown =1. ;
      mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown = 1 ;
      mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp = 1. ; mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown = 1. ;
      //lowmH
      mssmHiggsPtReweightGluGlu_lowmH = 1. ;
      mssmHiggsPtReweightGluGlu_lowmH_HqTUp = 1. ; mssmHiggsPtReweightGluGlu_lowmH_HqTDown =1. ;
      mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp = 1. ; mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown = 1 ;
      mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp = 1. ; mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown = 1. ;
    }


  // protection against multiple pairs per event
  ULong64_t lastEvent = 0;
  ULong64_t lastRun   = 0;
  ULong64_t lastLumi  = 0;

  // define JSON selector //
  cout << "define JSON selection" << endl;
  int nJson=8;
  string jsonFile[nJson];
  string dirJson = "/data_CMS/cms/htautau/JSON/"; 
  jsonFile[0] = dirJson+"/Cert_190456-196531_8TeV_13Jul2012ReReco_Collisions12_JSON_v2.txt"; // ReReco 13Jul 
  jsonFile[1] = dirJson+"/Cert_198022-198523_8TeV_24Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 24 Aug 
  jsonFile[2] = dirJson+"/Cert_190456-203002_8TeV_PromptReco_Collisions12_JSON_v2.txt";      // PromptReco 
  jsonFile[3] = dirJson+"/Cert_190782-190949_8TeV_06Aug2012ReReco_Collisions12_JSON.txt";    // ReReco 06Aug
  jsonFile[4] = dirJson+"/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt";         // PromptReco updated
  //jsonFile[5] = dirJson+"/Cert_203830-208686_8TeV_PromptReco_Collisions12_JSON_lowETM.txt";  // RunD period Low
  //jsonFile[6] = dirJson+"/Cert_203830-208686_8TeV_PromptReco_Collisions12_JSON_highETM.txt"; // RunD period High
  jsonFile[5] = dirJson+"/Cert_203830-208686_8TeV_22Jan2013ReReco_Collisions12_JSON_lowETM.txt";  // RunD period Low
  jsonFile[6] = dirJson+"/Cert_203830-208686_8TeV_22Jan2013ReReco_Collisions12_JSON_highETM.txt"; // RunD period High
  jsonFile[7] = dirJson+"/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"; // ReReco 22Jan2013
  map<int, vector<pair<int, int> > > jsonMap[nJson] ;  
  for(int iJ=0 ; iJ<nJson ; iJ++)
    jsonMap[iJ] = readJSONFile(jsonFile[iJ]);
  bool isGoodRun=false;
  bool isDuplicated=false;
  bool isMatched=false;
  bool isPeriodLow=false;
  bool isPeriodHigh=false;
  //
  bool dyFinalState=false;
  bool isData = sample.Contains("2012");
  //////////////////////////
  
  MAPDITAU_run mapDiTau;

  // P4s for tau removal
//   LV caloMEtUncorrNoTauP4, caloMEtNoTauP4, L1etmNoTauP4;

  ///////////////////////
  // LOOP OVER ENTRIES //
  ///////////////////////

  for(int n = n1 ; n < n2 ; n++) {
//     cout<<"n/n2 = "<<n<<"/"<<n2<<endl;
//   for (int n = 0; n < nEntries ; n++) {
//   for (int n = 0; n < 10000 ; n++) {

    currentTree->GetEntry(n);
//     cout<<"n = "<<n<<endl;
    if(n%1000==0) cout << n <<"/"<<(n2-n1)<< endl;
//     if(n%1000==0) cout << n <<"/"<<nEntries<< endl;

    // APPLY JSON SELECTION //
    isGoodRun=true;

    if(iJson_>=0 && iJson_<=7)
      isGoodRun = AcceptEventByRunAndLumiSection(run, lumi, jsonMap[iJson_]);
    
    if(!isGoodRun) continue;

    isPeriodLow  = AcceptEventByRunAndLumiSection(run, lumi, jsonMap[5]);  
    isPeriodHigh = AcceptEventByRunAndLumiSection(run, lumi, jsonMap[6]);  
    ///////////////////////////
    // SELECT DY FINAL STATE //
    ///////////////////////////

    // final state informations //
    genDecay_        = genDecay ;
    isTauLegMatched_ = isTauLegMatched;
    isTauLegMatchedToLep_ = isTauLegMatchedToLep;
    if( !isData ) {
      if(DEBUG) cout << "!isData --> leptFakeTau = " ;
      leptFakeTau      = (isTauLegMatched==0 && isTauLegMatchedToLep>0 && (*genDiTauLegsP4)[1].E()>8) ? 1 : 0;
      if(DEBUG) cout << leptFakeTau << endl;
    }
    else leptFakeTau = -99;
    //
    // final state selection //
    if( sample_.find("DYJets")!=string::npos  || 
	sample_.find("DY1Jets")!=string::npos || sample_.find("DY2Jets")!=string::npos || 
	sample_.find("DY3Jets")!=string::npos || sample_.find("DY4Jets")!=string::npos
        ) {
      dyFinalState=false;
      if(       sample_.find("TauTau")  !=string::npos ) { dyFinalState=(abs(genDecay)==(23*15) && isTauLegMatched==1 && isTauLegMatchedToLep==0); }
      else if(  sample_.find("ZTTL")    !=string::npos ) { dyFinalState=(abs(genDecay)==(23*15) && isTauLegMatched==0 && isTauLegMatchedToLep>0); }
      else if ( sample_.find("MuToTau") !=string::npos ) { dyFinalState=(abs(genDecay)!=(23*15) && leptFakeTau); }
      else if ( sample_.find("JetToTau")!=string::npos ) { dyFinalState=(abs(genDecay)!=(23*15) && !leptFakeTau); }
      else if(  sample_.find("ZTTJ")    !=string::npos ) { dyFinalState=(abs(genDecay)==(23*15) && isTauLegMatched==0 && isTauLegMatchedToLep==0); }
      else continue;
      if(!dyFinalState) continue;
    }
    ///////////////////////////

    /////////////////////////    
    ptL1     = (*diTauLegsP4)[0].Pt();
    ptL2     = (*diTauLegsP4)[1].Pt();
    etaL1    = (*diTauLegsP4)[0].Eta();
    etaL2    = (*diTauLegsP4)[1].Eta();
    mL1      = (*diTauLegsP4)[0].M();
    //
    isDuplicated = checkEventIsDuplicated(mapDiTau, run, lumi, event, ptL1, ptL2, etaL1, etaL2);
    if(isDuplicated) continue;
    ///////////////////////

    uParl=-999.; uPerp=-999.; metParl=-999.; metPerp=-999.; metSigmaParl=-999.; metSigmaPerp=-999.;

    // initialize variables filled only in the two jet case
    pt1=-99;pt2=-99;eta1=-99,eta2=-99;Deta=-99;Dphi=-99;Mjj=-99;phi1=-99;phi2=-99;
    //pt1_v2=-99;pt2_v2=-99;eta1_v2=-99,eta2_v2=-99;Deta_v2=-99;Dphi_v2=-99;Mjj_v2=-99;phi1_v2=-99;phi2_v2=-99;
    ptVeto = -99; phiVeto= -99; etaVeto= -99;isVetoInJets=-99;
    ptB1   = -99; phiB1  = -99; etaB1 = -99;
    diJetPt = -99; diJetPhi = -99; dPhiHjet = -99; c1 = -99; c2 = -99;
    chFracPV1=-99;chFracPV2=-99;chFracPVVeto=-99;jetsBtagHE1 = -99;jetsBtagHE2 = -99;jetsBtagHP1 = -99;jetsBtagHP2 = -99;
    jetsBtagCSV1 = -99; jetsBtagCSV2 = -99; 
    jet1PUMVA = -99; jet2PUMVA=-99; jetVetoPUMVA=-99; 
    jet1PUWP = -99; jet2PUWP = -99; jetVetoPUWP = -99;
    jet1QGmlp=-99; jet2QGmlp=-99; jetVetoQGmlp=-99;
    jet1QGlike=-99; jet2QGlike=-99; jetVetoQGlike=-99;
    jet1QGrhoIso=-99; jet2QGrhoIso=-99; jetVetoQGrhoIso=-99;
    jet1QGptD=-99; jet2QGptD=-99; jetVetoQGptD=-99;
    jet1QGaxis1=-99; jet2QGaxis1=-99; jetVetoQGaxis1=-99;
    jet1QGaxis2=-99; jet2QGaxis2=-99; jetVetoQGaxis2=-99;
    jet1QGmult=-99; jet2QGmult=-99; jetVetoQGmult=-99;
    jet1QGnChg=-99; jet2QGnChg=-99; jetVetoQGnChg=-99;
    jet1QGnNeutral=-99; jet2QGnNeutral=-99; jetVetoQGnNeutral=-99;
    jet1QGflavor=-99; jet2QGflavor=-99; jetVetoQGflavor=-99;
    jet1QGNbConst=-99; jet2QGNbConst=-99; jetVetoQGNbConst=-99;
    jet1QGConstId=-99; jet2QGConstId=-99; jetVetoQGConstId=-99;
    jet1QGConstPt=-99; jet2QGConstPt=-99; jetVetoQGConstPt=-99;
    MVAvbf = -99;
    L1etm_=-99; L1etmPhi_=-99; L1etmPhiCorr_=-99; L1etmCorr_=-99; // MB
    caloMEtNoHFUncorr_=-99; caloMEtNoHFUncorrPhi_=-99;//MB 
    caloMEtNoHF_=-99;      caloMEtNoHFPhi_=-99;// MB
    caloMEtNoHFUp_=-99;      caloMEtNoHFUpPhi_=-99;// ND
    caloMEtNoHFDown_=-99;      caloMEtNoHFDownPhi_=-99;// ND
    sumEt_ = caloNoHFsumEt_ = caloNoHFsumEtCorr_ = -99; // ND
    //Variables for Luca
    dRb1b2 = dPhib1b2 = dEtab1b2 = dRb1L1 = dRb1L1 = dRb2L1 = dRb2L2 = dRMaxbJetLept = dRMinbJetLept = -99;
    dPhiHbbHtt = dEtaHbbHtt = dRHbbHtt = ptHbb = visMassHbb = diHiggsVisMass = -99;
    dRHbbMETMVA = dPhiHbbMETMVA = dEtaHbbMETMVA = dRHttMETMVA = dPhiHttMETMVA = dEtaHttMETMVA = -99;

    for(int i=0 ; i<50 ; i++) {
      ptAll[i] = etaAll[i] = phiAll[i] = energyAll[i] = csvAll[i] = ptAllB[i] = etaAllB[i] = phiAllB[i]= energyAllB[i] = csvAllB[i] = -99.;
      ptAllBLoose[i] = etaAllBLoose[i] = phiAllBLoose[i] = energyAllBLoose[i] = csvAllBLoose[i] = -99.;
    }

    // define the relevant jet collection 
    nJets20BTagged = 0; nJets20BTaggedBUp = 0; nJets20BTaggedBDown = 0; nJets20BTaggedLoose = 0;
    nJets20BTaggedLUp = 0; nJets20BTaggedLDown = 0;
    nJets30        = 0; nJets20 = 0;
    int lead  = -99;
    int trail = -99;
    int veto  = -99;
    vector<int> indexes;
    for(int l = 0 ; l < int(jets->size()) ; l++){
      //if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta && (*jetPUWP)[l*3]>MINJetID)
      int passJetID = getJetIDMVALoose((*jets)[l].Pt(), (*jets)[l].Eta(), (*jetPUMVA)[l]);
      if((*jets)[l].Pt()>MINPt1 && TMath::Abs((*jets)[l].Eta())<MAXEta && passJetID>MINJetID)
	indexes.push_back(l);
      // check there was no error while filling Quark/gluon info:
      if (fabs((*jets)[l].Pt()-(*jetQuarkGluon)[l*14])>1e-3) 
	std::cout<<"ERROR in quark/gluon info: inconsistent jets! "<<(*jets)[l].Pt()<<" "
		 <<(*jetQuarkGluon)[l*14]<<" "<<(*jetQuarkGluon)[l*14+1]<<" "<<(*jetQuarkGluon)[l*14+2]<<std::endl ;
    }

    if(indexes.size()>0) lead  = indexes[0];  
    if(indexes.size()>1) trail = indexes[1];  
    if(indexes.size()>2) veto  = indexes[2];  

    for(unsigned int v = 0 ; v < indexes.size() ; v++){
      if( (*jets)[indexes[v]].Pt() > 30 ) {
	ptAll[nJets30]  = (*jets)[indexes[v]].Pt();
	etaAll[nJets30] = (*jets)[indexes[v]].Eta();
	phiAll[nJets30] = (*jets)[indexes[v]].Phi();
	energyAll[nJets30] = (*jets)[indexes[v]].E();
	csvAll[nJets30] = (*jetsBtagCSV)[indexes[v]];
	nJets30++;
      }
      if( (*jets)[indexes[v]].Pt() > 20 ) nJets20++;

      // b-tag loose
      if( (*jets)[indexes[v]].Pt() > 20 && TMath::Abs((*jets)[indexes[v]].Eta())<2.4 && (*jetsBtagCSV)[indexes[v]] > 0.244 )
	{
	  ptAllBLoose[nJets20BTaggedLoose]  = (*jets)[indexes[v]].Pt();
	  etaAllBLoose[nJets20BTaggedLoose] = (*jets)[indexes[v]].Eta();
	  phiAllBLoose[nJets20BTaggedLoose] = (*jets)[indexes[v]].Phi();	  
	  energyAllBLoose[nJets20BTaggedLoose] = (*jets)[indexes[v]].E();	  
	  csvAllBLoose[nJets20BTaggedLoose] = (*jetsBtagCSV)[indexes[v]];
	  nJets20BTaggedLoose++ ;
	}
      if( (*jets)[indexes[v]].Pt() > 20 && TMath::Abs((*jets)[indexes[v]].Eta())<2.4){ 
// 	int jetFlavour = ((*bQuark)[indexes[v]] > 0) ? 5 : 1;
	int jetFlavour = int((*bQuark)[indexes[v]]);
        //bool isBtag = isbtagged(isbQuark, (*jetsBtagCSV)[indexes[v]], isData, kNo, kNo); 
	bool isBtag = btsf->isbtagged((*jets)[indexes[v]].Pt(), (*jets)[indexes[v]].Eta(), (*jetsBtagCSV)[indexes[v]], jetFlavour, isData ,kNo, kNo, true);

	if(isBtag){

	  ptAllB[nJets20BTagged]  = (*jets)[indexes[v]].Pt();
	  etaAllB[nJets20BTagged] = (*jets)[indexes[v]].Eta();
	  phiAllB[nJets20BTagged] = (*jets)[indexes[v]].Phi();	  
	  energyAllB[nJets20BTagged] = (*jets)[indexes[v]].E();	  
	  csvAllB[nJets20BTagged] = (*jetsBtagCSV)[indexes[v]];

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
    TLorentzVector b1_temp,b2_temp;
    TLorentzVector b1b2_temp;
    LV b1(1,0,0,1);
    LV b2(1,0,0,1);
    LV b1b2(1,0,0,1);


    if(nJets20BTagged>1){
      b1_temp.SetPtEtaPhiE(ptAllB[0],etaAllB[0],phiAllB[0],energyAllB[0]);
      b2_temp.SetPtEtaPhiE(ptAllB[1],etaAllB[1],phiAllB[1],energyAllB[1]);
      dRb1b2 = b1_temp.DeltaR(b2_temp);
      dPhib1b2 = b1_temp.DeltaPhi(b2_temp);
      dEtab1b2 = fabs(b1_temp.Eta()-b2_temp.Eta());


      b1b2_temp =  b1_temp+b2_temp;
      b1.SetPx(b1_temp.Px());
      b1.SetPy(b1_temp.Py());
      b1.SetPz(b1_temp.Pz());
      b1.SetE(b1_temp.E());
      b2.SetPx(b2_temp.Px());
      b2.SetPy(b2_temp.Py());
      b2.SetPz(b2_temp.Pz());
      b2.SetE(b2_temp.E());
      b1b2.SetPx(b1b2_temp.Px());
      b1b2.SetPy(b1b2_temp.Py());
      b1b2.SetPz(b1b2_temp.Pz());
      b1b2.SetE(b1b2_temp.E());

      ptHbb= b1b2.pt();
      visMassHbb= b1b2.M();
    }
    if(nJets20BTagged>0){

      dRb1L1 = deltaR(b1,(*diTauLegsP4)[0]);
      dRb1L2 = deltaR(b1,(*diTauLegsP4)[1]);
    }
    if(nJets20BTagged>1){

      dRb2L1 = deltaR(b2,(*diTauLegsP4)[0]);
      dRb2L2 = deltaR(b2,(*diTauLegsP4)[1]);
      dRMaxbJetLept = std::max(std::max(dRb1L1,dRb1L2),std::max(dRb2L1,dRb2L2));
      dRMinbJetLept = std::min(std::min(dRb1L1,dRb1L2),std::min(dRb2L1,dRb2L2));
    }

    LV diHiggs;
    if(nJets20BTagged>1){

      diHiggs = b1b2+(*diTauVisP4)[0];
      dRHbbHtt = deltaR(b1b2,(*diTauVisP4)[0]);
      dPhiHbbHtt =  abs(b1b2.Phi()-(*diTauVisP4)[0].Phi()) > TMath::Pi() ? 
	-abs(b1b2.Phi()-(*diTauVisP4)[0].Phi()) + 2*TMath::Pi()  :
	abs(b1b2.Phi()-(*diTauVisP4)[0].Phi()) ;
      dEtaHbbHtt =  abs(b1b2.Eta()-(*diTauVisP4)[0].Eta());
      diHiggsVisMass = diHiggs.M();
      
      dRHbbMETMVA = deltaR(b1b2,(*METP4)[1]);
      dPhiHbbMETMVA =  abs(b1b2.Phi()-(*METP4)[1].Phi()) > TMath::Pi() ? 
	-abs(b1b2.Phi()-(*METP4)[1].Phi()) + 2*TMath::Pi()  :
	abs(b1b2.Phi()-(*METP4)[1].Phi()) ;
      dEtaHbbMETMVA =  abs(b1b2.Eta()-(*METP4)[1].Eta());
    }


    dRHttMETMVA = deltaR((*diTauVisP4)[0],(*METP4)[1]);
    dPhiHttMETMVA =  abs((*diTauVisP4)[0].Phi()-(*METP4)[1].Phi()) > TMath::Pi() ? 
      -abs((*diTauVisP4)[0].Phi()-(*METP4)[1].Phi()) + 2*TMath::Pi()  :
      abs((*diTauVisP4)[0].Phi()-(*METP4)[1].Phi()) ;
    dEtaHttMETMVA =  abs((*diTauVisP4)[0].Eta()-(*METP4)[1].Eta());
 

    if(DEBUG) cout << "look at first jet" << endl;

    // first jet
    if(lead>=0){


      pt1  = (*jets)[lead].Pt();
      eta1 = (*jets)[lead].Eta();
      phi1 = (*jets)[lead].Phi();
      /*
      eta1_v2 = (*jets_v2)[lead].Eta();
      pt1_v2  = (*jets_v2)[lead].Pt();
      phi1_v2 = (*jets_v2)[lead].Phi();
      */      
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

      jet1QGmlp = (*jetQuarkGluon)[lead*14+1] ;
      jet1QGlike = (*jetQuarkGluon)[lead*14+2] ;
      jet1QGrhoIso = (*jetQuarkGluon)[lead*14+6] ;
      jet1QGptD = (*jetQuarkGluon)[lead*14+7] ;
      jet1QGaxis1 = (*jetQuarkGluon)[lead*14+8] ;
      jet1QGaxis2 = (*jetQuarkGluon)[lead*14+9] ;
      jet1QGmult = (*jetQuarkGluon)[lead*14+10] ;
      jet1QGnChg = (*jetQuarkGluon)[lead*14+11] ;
      jet1QGnNeutral = (*jetQuarkGluon)[lead*14+13] ;
      jet1QGflavor = (run < 100)? (*jetQuarkGluonGen)[lead*4+0] : -99 ;
      jet1QGNbConst = (run < 100)? (*jetQuarkGluonGen)[lead*4+1] : -99 ;
      jet1QGConstId = (run < 100)? (*jetQuarkGluonGen)[lead*4+2] : -99 ;
      jet1QGConstPt = (run < 100)? (*jetQuarkGluonGen)[lead*4+3] : -99 ;


      // second jet
      if(trail>=0){

	pt2  = (*jets)[trail].Pt();
	eta2 = (*jets)[trail].Eta();
	phi2 = (*jets)[trail].Phi();

	/*
	pt2_v2  = (*jets_v2)[trail].Pt();
	eta2_v2 = (*jets_v2)[trail].Eta();
	phi2_v2 = (*jets_v2)[trail].Phi();
	*/
	
	dPhiL1J2 =  abs((*diTauLegsP4)[0].Phi()-phi2) > TMath::Pi() ? 
	  -abs( (*diTauLegsP4)[0].Phi()-phi2 ) + 2*TMath::Pi()  :
	  abs( (*diTauLegsP4)[0].Phi()-phi2 ) ;
	dPhiL2J2 =  abs((*diTauLegsP4)[1].Phi()-phi2) > TMath::Pi() ? 
	  -abs( (*diTauLegsP4)[1].Phi()-phi2 ) + 2*TMath::Pi()  :
	  abs( (*diTauLegsP4)[1].Phi()-phi2 ) ;


	Deta = TMath::Abs(eta1-eta2);
	Dphi = TMath::Abs((*jets)[lead].Phi()-(*jets)[trail].Phi()) > TMath::Pi() ? 
	  -TMath::Abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) + 2*TMath::Pi()  :
	  TMath::Abs( (*jets)[lead].Phi()-(*jets)[trail].Phi() ) ;
	Mjj  = ((*jets)[lead]+(*jets)[trail]).M();

	/*
	Deta_v2 = TMath::Abs(eta1-eta2);
	Dphi_v2 = TMath::Abs((*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi()) > TMath::Pi() ? 
	  -TMath::Abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) + 2*TMath::Pi()  :
	  TMath::Abs( (*jets_v2)[lead].Phi()-(*jets_v2)[trail].Phi() ) ;
	Mjj_v2  = ((*jets_v2)[lead]+(*jets_v2)[trail]).M();
	*/
	
	jetsBtagHE2 = (*jetsBtagHE)[trail];
	jetsBtagHP2 = (*jetsBtagHP)[trail];
	jetsBtagCSV2= (*jetsBtagCSV)[trail];

	chFracPV2 = (*jetsChNfraction)[trail];

	jet2PUMVA = (*jetPUMVA)[trail];
	jet2PUWP  = (*jetPUWP)[trail*3]; // WP loose

	jet2QGmlp = (*jetQuarkGluon)[trail*14+1] ;
	jet2QGlike = (*jetQuarkGluon)[trail*14+2] ;
	jet2QGrhoIso = (*jetQuarkGluon)[trail*14+6] ;
	jet2QGptD = (*jetQuarkGluon)[trail*14+7] ;
	jet2QGaxis1 = (*jetQuarkGluon)[trail*14+8] ;
	jet2QGaxis2 = (*jetQuarkGluon)[trail*14+9] ;
	jet2QGmult = (*jetQuarkGluon)[trail*14+10] ;
	jet2QGnChg = (*jetQuarkGluon)[trail*14+11] ;
	jet2QGnNeutral = (*jetQuarkGluon)[trail*14+13] ;
	jet2QGflavor = (run < 100)? (*jetQuarkGluonGen)[trail*4+0] : -99 ;
	jet2QGNbConst = (run < 100)? (*jetQuarkGluonGen)[trail*4+1] : -99 ;
	jet2QGConstId = (run < 100)? (*jetQuarkGluonGen)[trail*4+2] : -99 ;
	jet2QGConstPt = (run < 100)? (*jetQuarkGluonGen)[trail*4+3] : -99 ;

	diJetPt  = ((*jets)[lead] + (*jets)[trail]).Pt();
	diJetPhi =  //((*jets)[lead]-(*jets)[trail]).Phi();
	  TMath::Abs(  ((*jets)[lead] - (*jets)[trail]).Phi() ) > TMath::Pi() ? 
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

    ptVeto  = (veto>=0) ? (*jets)[veto].Pt() : -99;
    etaVeto = (veto>=0) ? (*jets)[veto].Eta(): -99;
    phiVeto = (veto>=0) ? (*jets)[veto].Phi(): -99;
    chFracPVVeto = (veto>=0) ? (*jetsChNfraction)[veto] : -99; 
    jetVetoPUMVA = (veto>=0) ? (*jetPUMVA)[veto]  : -99;
    jetVetoPUWP  = (veto>=0) ? (*jetPUWP)[veto*3] : -99; // WP loose

    jetVetoQGmlp = (veto>=0) ? (*jetQuarkGluon)[veto*14+1] : -99;
    jetVetoQGlike = (veto>=0) ? (*jetQuarkGluon)[veto*14+2] : -99;
    jetVetoQGrhoIso = (veto>=0) ? (*jetQuarkGluon)[veto*14+6] : -99;
    jetVetoQGptD = (veto>=0) ? (*jetQuarkGluon)[veto*14+7] : -99;
    jetVetoQGaxis1 = (veto>=0) ? (*jetQuarkGluon)[veto*14+8] : -99;
    jetVetoQGaxis2 = (veto>=0) ? (*jetQuarkGluon)[veto*14+9] : -99;
    jetVetoQGmult = (veto>=0) ? (*jetQuarkGluon)[veto*14+10] : -99;
    jetVetoQGnChg = (veto>=0) ? (*jetQuarkGluon)[veto*14+11] : -99;
    jetVetoQGnNeutral = (veto>=0) ? (*jetQuarkGluon)[veto*14+13] : -99;
    jetVetoQGflavor = (veto>=0 && run < 100) ? (*jetQuarkGluonGen)[veto*4+0] : -99;
    jetVetoQGNbConst = (veto>=0 && run < 100) ? (*jetQuarkGluonGen)[veto*4+1] : -99;
    jetVetoQGConstId = (veto>=0 && run < 100) ? (*jetQuarkGluonGen)[veto*4+2] : -99;
    jetVetoQGConstPt = (veto>=0 && run < 100) ? (*jetQuarkGluonGen)[veto*4+3] : -99;

    isVetoInJets = 0;
    for(int l = 0 ; l < int(indexes.size()) ; l++){
      if(lead>=0 && trail>=0 && (l!= lead && l!= trail) &&
	 (*jets)[indexes[l]].Pt()>PtVETO && ((*jets)[indexes[l]].Eta() - eta1)*((*jets)[indexes[l]].Eta() - eta2)<=0 )
	{
	  isVetoInJets = 1;
	}
    }

    diTauNSVfitMass_        = diTauNSVfitMass; // no re-calibration needed with VEGAS
    //diTauNSVfitMass_        = diTauNSVfitMass*0.985; // re-calibration plugin wrt standalone
    diTauNSVfitMassErrUp_   = diTauNSVfitMassErrUp;
    diTauNSVfitMassErrDown_ = diTauNSVfitMassErrDown;
    diTauNSVfitMassZLUp_   = diTauNSVfitMass*1.02;
    diTauNSVfitMassZLDown_ = diTauNSVfitMass*0.98;
    diTauNSVfitPt_        = diTauNSVfitPt;
    diTauNSVfitPtErrUp_   = diTauNSVfitPtErrUp;
    diTauNSVfitPtErrDown_ = diTauNSVfitPtErrDown;

    genDiTauMass_ = genDiTauMass;

    diTauSVFitMass = (*diTauSVfitP4)[0].M();
    diTauSVFitPt   = (*diTauSVfitP4)[0].Pt();
    diTauSVFitEta  = (*diTauSVfitP4)[0].Eta();
    diTauSVFitPhi  = (*diTauSVfitP4)[0].Phi();

    //float c0L = 0.0050;
    //float c1L = 1.14;
    //float c2L = 0.40;
    //float c3L = 25;
    diTauSVFitMassCal0 = -99.;
    diTauSVFitMassCal1 = -99.;
    diTauSVFitMassCal2 = -99.;
    //(*diTauSVfitP4)[0].M() - 
    //(((c0L*120 + c1L)*diTauNSVfitMassErrUp + (c2L*120+c3L)) - 120);
    //float c0 = 9.08725e-01;
    //float c1 = 4.01539e+01;
    //float c2 = 7.48472e-02;
    //float c3 = 3.79035e+01;
    //float c4 = 1.93179e-04;
    //float c5 = 1.35327e-02;
    //float c6 = -1.36772e-02;
    //float c7 = 6.04364e-05;
    //float massH     = 120;
    //float bias120   = (c0*massH+c1) + (c2*massH+c3)*TMath::Erf((c4*massH+c5)*diTauNSVfitMassErrUp + (c6*massH+c7)) - massH;
    //diTauSVFitMassCal1 = (*diTauSVfitP4)[0].M() - bias120; 
    //float massNew = (*diTauSVfitP4)[0].M();
    //float bias    = bias120;
    //float diff    = 999.;
    //int nMax      = 20;
    //bool exit     = false;
    //for(int it=0; it< nMax && !exit; it++){      
    //float bias_it = (c0*massNew+c1) + (c2*massNew+c3)*TMath::Erf((c4*massNew+c5)*diTauNSVfitMassErrUp + (c6*massNew+c7)) - massNew;
    // massNew = (*diTauSVfitP4)[0].M() - bias_it;
    //diff = fabs(bias - bias_it);
    //bias = bias_it;
    //if(it==(nMax-1)) cout << "Reached Max iterartion w/o convergence" << endl;
    //if(it>0 && diff<1){
    //if((*diTauSVfitP4)[0].M()<60 && diTauNSVfitMassErrUp>0) 
    //  bias=0;
    //exit = true;
    //} 
    //}
    //diTauSVFitMassCal2 = -99;
    //(*diTauSVfitP4)[0].M() - bias;


    diTauRecoPt  = ((*diTauVisP4)[0]+(*METP4)[1]).Pt();
    diTauRecoPhi = ((*diTauVisP4)[0]+(*METP4)[1]).Phi();

    diTauCAMass = (*diTauCAP4)[0].M();
    diTauCAPt   = (*diTauCAP4)[0].Pt();
    diTauCAEta  = (*diTauCAP4)[0].Eta();
    diTauCAPhi  = (*diTauCAP4)[0].Phi();

    diTauVisMass  = (*diTauVisP4)[0].M();
    diTauVisPt    = (*diTauVisP4)[0].Pt();
    diTauVisEta   = (*diTauVisP4)[0].Eta();
    diTauVisPhi   = (*diTauVisP4)[0].Phi();
    diTauVisMassZLUp  = (*diTauVisP4)[0].M()*1.02;
    diTauVisMassZLDown  = (*diTauVisP4)[0].M()*0.98;

    diTauMinMass  = mTauTauMin;


    ptL1     = (*diTauLegsP4)[0].Pt();
    ptL2     = (*diTauLegsP4)[1].Pt();
    etaL1    = (*diTauLegsP4)[0].Eta();
    etaL2    = (*diTauLegsP4)[1].Eta();
    phiL1    = (*diTauLegsP4)[0].Phi();
    phiL2    = (*diTauLegsP4)[1].Phi();
    dRL1L2 =  deltaR((*diTauLegsP4)[0],(*diTauLegsP4)[1]);
    dPhiL1L2 =  TMath::Abs((*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi()) > TMath::Pi() ? 
      -TMath::Abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) + 2*TMath::Pi()  :
      TMath::Abs( (*diTauLegsP4)[0].Phi()-(*diTauLegsP4)[1].Phi() ) ;
    dEtaL1L2 =  abs((*diTauLegsP4)[0].Eta()-(*diTauLegsP4)[1].Eta());

    dxy1_    = dxy1;
    dz1_     = dz1;

    visibleTauMass_ = visibleTauMass;
    diTauCharge_    = diTauCharge;
    chargeL1_       = chargeL1;

    // genMass
    genMass = 0;
    genTrueTau1Pt = -99.;
    genTrueTau1Eta = -99.;
    genTrueTau1Phi = -99.;
    genTrueTau2Pt = -99.;
    genTrueTau2Eta = -99.;
    genTrueTau2Phi = -99.;

    genTrueTau2DecaysLeptonically = -1 ;
    genTrueTau1DecaysLeptonically = -1 ;

    genTrueTau1Charge = -99 ;
    genTrueTau2Charge = -99 ;


    if(genTausP4->size()>1) {
      genMass = ( (*genTausP4)[0] + (*genTausP4)[1] ).M();

      genTrueTau1Pt = (*genTausP4)[0].Pt();
      genTrueTau1Eta = (*genTausP4)[0].Eta();
      genTrueTau1Phi = (*genTausP4)[0].Phi();
      genTrueTau1DecaysLeptonically = (*genTausDecayLeptonically)[0];
      genTrueTau1Charge = (*genTausCharge)[0];

      genTrueTau2Pt = (*genTausP4)[1].Pt();
      genTrueTau2Eta = (*genTausP4)[1].Eta();
      genTrueTau2Phi = (*genTausP4)[1].Phi();
      genTrueTau2DecaysLeptonically = (*genTausDecayLeptonically)[1];
      genTrueTau2Charge = (*genTausCharge)[1];



    }

    // genMu info
    if(genDiTauLegsP4->size()>0) {
      genMuPt   = (*genDiTauLegsP4)[0].Pt();
      genMuEta  = (*genDiTauLegsP4)[0].Eta();
      genMuPhi  = (*genDiTauLegsP4)[0].Phi();
    }
    else genMuPt = genMuEta = -99;
    
    //genTau Info
    if(genDiTauLegsP4->size()>1) {
      genTauPt   = (*genDiTauLegsP4)[1].Pt();
      genTauEta  = (*genDiTauLegsP4)[1].Eta();
      genTauPhi  = (*genDiTauLegsP4)[1].Phi();
    }
    else genTauPt = genTauEta = -99;

    visGenTauMass = visibleGenTauMass;
    genDecayMode_ = genDecayMode;
    genVMass     = (genVP4->size() > 0) ? (*genVP4)[0].M() : 0;
    genVPt       = (genVP4->size() > 0) ? (*genVP4)[0].Pt() : 0;
    

    ////////////////////////////////////////////////////////////////////
    if(DEBUG) cout << "Look at MET correct" << endl;
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

    LV recoilCorrecMET = corrMET;

    float scalarSumPtLeg1     = ( *diTauLegsP4)[0].Pt() + (*METP4)[0].Pt();
    float vectorSumPtLeg1     = ((*diTauLegsP4)[0] +      (*METP4)[0]).Pt() ;
    float scalarSumPtLeg1Corr = ( *diTauLegsP4)[0].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtLeg1Corr = ((*diTauLegsP4)[0] +      recoilCorrecMET).Pt() ;
    float scalarSumPtLeg1MVA  = ( *diTauLegsP4)[0].Pt() + (*METP4)[1].Pt();  // index 3->1
    float vectorSumPtLeg1MVA  = ((*diTauLegsP4)[0] +      (*METP4)[1]).Pt() ; //index 3->1

    float scalarSumPtLeg2     = ( *diTauLegsP4)[1].Pt() + (*METP4)[0].Pt();
    float vectorSumPtLeg2     = ((*diTauLegsP4)[1] +      (*METP4)[0]).Pt() ;
    float scalarSumPtLeg2Corr = ( *diTauLegsP4)[1].Pt() + recoilCorrecMET.Pt();
    float vectorSumPtLeg2Corr = ((*diTauLegsP4)[1] +      recoilCorrecMET).Pt() ;
    float scalarSumPtLeg2MVA  = ( *diTauLegsP4)[1].Pt() + (*METP4)[1].Pt(); // index 3->1
    float vectorSumPtLeg2MVA  = ((*diTauLegsP4)[1] +      (*METP4)[1]).Pt() ; // index 3->1

    MtLeg1_     = TMath::Sqrt( scalarSumPtLeg1*scalarSumPtLeg1 - vectorSumPtLeg1*vectorSumPtLeg1 ) ;
    MtLeg1Corr_ = TMath::Sqrt( scalarSumPtLeg1Corr*scalarSumPtLeg1Corr - vectorSumPtLeg1Corr*vectorSumPtLeg1Corr ) ;
    MtLeg1MVA_  = TMath::Sqrt( scalarSumPtLeg1MVA*scalarSumPtLeg1MVA - vectorSumPtLeg1MVA*vectorSumPtLeg1MVA ) ;

    MtLeg2_     = TMath::Sqrt( scalarSumPtLeg2*scalarSumPtLeg2 - vectorSumPtLeg2*vectorSumPtLeg2 ) ;
    MtLeg2Corr_ = TMath::Sqrt( scalarSumPtLeg2Corr*scalarSumPtLeg2Corr - vectorSumPtLeg2Corr*vectorSumPtLeg2Corr ) ;
    MtLeg2MVA_  = TMath::Sqrt( scalarSumPtLeg2MVA*scalarSumPtLeg2MVA - vectorSumPtLeg2MVA*vectorSumPtLeg2MVA ) ;

    pZeta_        = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[1];
    pZetaCorr_    = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[1];
    pZetaMVA_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[1]; // index 3->1
    pZetaVis_     = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[0]))[0];
    pZetaVisCorr_ = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], recoilCorrecMET))[0];
    pZetaVisMVA_  = (computeZeta( (*diTauLegsP4)[0], (*diTauLegsP4)[1], (*METP4)[1]))[0]; // index 3->1

    pZetaSig_     = pZetaSig;

    MEt        = (*METP4)[0].Et();
    MEtPhi     = (*METP4)[0].Phi();
    MEtCorr    = recoilCorrecMET.Et();
    MEtCorrPhi = recoilCorrecMET.Phi();
    MEtMVA     = (*METP4)[1].Et();
    MEtMVAPhi  = (*METP4)[1].Phi();
    
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


//     ////////////////////////////////////////////////
//     // Remove Tau contributions in L1/Calo MET using L1Jets/CaloJets matched to taus

//     // use rawEtL1Jet(et,eta)

//     // Initialization
//     caloMEtUncorrNoTauP4 = (*caloMETNoHFP4)[0];
//     caloMEtNoTauP4       = (*caloMETNoHFP4)[1];

//     if(!sample.Contains("Emb")) L1etmNoTauP4 = (*l1ETMP4)[0];
//     else                        L1etmNoTauP4 = (*l1ETMP4)[1];

//     if( SELTAU && tightestAntiMuWP>2 && hpsDB3H<1.5 ) {

//       // L1Jets matching
//       if(l1JetsP4->size()>0) {
// 	for(uint iJ=0 ; iJ<l1JetsP4->size() ; iJ++) {
// 	  if( deltaR( (*l1JetsP4)[iJ] , (*diTauLegsP4)[1] ) < 0.3 )
// 	    L1etmNoTauP4 += unfoldL1JetP4( (*l1JetsP4)[iJ] );
// 	}
//       }

//       // CaloJets matching
//       if(caloJetsMatchedP4->size()>0) {
// 	for(uint iJ=0 ; iJ<caloJetsMatchedP4->size() ; iJ++) {
// 	  if( deltaR( (*caloJetsMatchedP4)[iJ] , (*diTauLegsP4)[1] ) < 0.3 ) {
// 	    caloMEtNoTauP4       += (*caloJetsMatchedP4)[iJ];
// 	    caloMEtUncorrNoTauP4 += (*caloJetsMatchedP4)[iJ];
// 	  }
// 	}
//       }
//     }

//     caloMEtNoTau_       = caloMEtNoTauP4.Et() ;
//     caloMEtUncorrNoTau_ = caloMEtUncorrNoTauP4.Et() ;
//     L1etmNoTau_         = L1etmNoTauP4.Et() ;

//     caloMEtNoTauPhi_       = caloMEtNoTauP4.Phi() ;
//     caloMEtUncorrNoTauPhi_ = caloMEtUncorrNoTauP4.Phi() ;
//     L1etmNoTauPhi_         = L1etmNoTauP4.Phi() ;

//     ////////////////////////////////////////////////  
    

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
    //==========================================

      /*
    TMatrixD* metsig = new TMatrixD(2,2);
    (*metsig)[0][0] = (*metSgnMatrix)[0]; 
    (*metsig)[0][1] = (*metSgnMatrix)[1]; 
    (*metsig)[1][0] = (*metSgnMatrix)[1]; 
    (*metsig)[1][1] = (*metSgnMatrix)[2]; */

    // NO NEED FOR STANDALONE
    /* 
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
    */
    diTauSVFitMassSA    = -99; 
    diTauSVFitMassErrSA = -99;
    etaTau1Fit          = -99;
    etaTau2Fit          = -99;
    phiTau1Fit          = -99;
    phiTau2Fit          = -99;
    ptTau1Fit           = -99;
    ptTau2Fit           = -99;
    ////////////////////////////////////////////////

    combRelIsoLeg1         = (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    float PUtoPVratio      = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta     = (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta    = (chIsoLeg1    + std::max( nhIsoLeg1+phIsoLeg1-0.5*(nhIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();



    combRelIsoLeg1DBetav2  = (allChIsoLeg1 + std::max( nhIsoLeg1+phIsoLeg1-0.5*(nhIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt();

    if(event==355631)
      {
	cout<<"ISO FOR ABDOLLAH = "<<endl;
	cout<<" combRelIsoLeg1DBetav2 = "<<combRelIsoLeg1DBetav2<<endl;
	cout<<"      allChIsoLeg1 = "<<allChIsoLeg1<<endl;
	cout<<"      nhIsoLeg1 = "<<nhIsoLeg1<<endl;
	cout<<"      phIsoLeg1 = "<<phIsoLeg1<<endl;
	cout<<"      nhIsoPULeg1 = "<<nhIsoPULeg1<<endl;
	cout<<"END ISO FOR ABDOLLAH = "<<endl;
      }

//     isoLeg1MVA_            = isoLeg1MVA;
    float EffArea          = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho      = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2            =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    
    rhoFastJet_            = rhoFastJet;

    decayMode_       = decayMode;
    numPV_           = numPV;

    nPUVertices_     = nPUVertices;

    // Embedding Weights
    embeddingFilterEffWeight_ = embeddingWeight;
    if(RERECO) {
      TauSpinnerWeight_ = (*embeddingWeights)[0];
      ZmumuEffWeight_ = (*embeddingWeights)[1];
      diTauMassVSdiTauPtWeight_ = (*embeddingWeights)[2];
      tau2EtaVStau1EtaWeight_ = (*embeddingWeights)[3];
      tau2PtVStau1PtWeight_ = (*embeddingWeights)[4];
      muonRadiationWeight_ = (*embeddingWeights)[5];
      muonRadiationDownWeight_ = (*embeddingWeights)[6];
      muonRadiationUpWeight_ = (*embeddingWeights)[7];
    }      
    else {
      TauSpinnerWeight_ = 1;
      ZmumuEffWeight_ = 1;
      diTauMassVSdiTauPtWeight_ = 1;
      tau2EtaVStau1EtaWeight_ = 1;
      tau2PtVStau1PtWeight_ = 1;
      muonRadiationWeight_ = 1;
      muonRadiationDownWeight_ = 1;
      muonRadiationUpWeight_ = 1;
    }
    embeddingWeight_ = embeddingFilterEffWeight_;
    embeddingWeight_ *=  TauSpinnerWeight_;
    embeddingWeight_ *=  ZmumuEffWeight_;
    embeddingWeight_ *=  diTauMassVSdiTauPtWeight_;
    embeddingWeight_ *=  tau2EtaVStau1EtaWeight_;
    embeddingWeight_ *=  tau2PtVStau1PtWeight_;
    embeddingWeight_ *=  muonRadiationWeight_;

    if(sample.Contains("Emb") && UnfoldDen1 && genTausP4->size()>1){
      float corrFactorEmbed = (UnfoldDen1->GetBinContent( UnfoldDen1->GetXaxis()->FindBin( (*genTausP4)[0].Eta() ) ,  UnfoldDen1->GetYaxis()->FindBin( (*genTausP4)[1].Eta() ) )); 
      embeddingWeight_ *=  corrFactorEmbed;
      //cout << "Correcting with " << corrFactorEmbed << endl;
    }

    // PROTECTION AGAINST PATHOLOGIC CASES //
    if(embeddingWeight_>10) embeddingWeight_=10;

    // SWITCH BACK TO SIMPLE WEIGHT FOR PF EMBEDDED //
    if(sample.Contains("Emb") && sample.Contains("PF")) embeddingWeight_ = embeddingWeight ;

    // SAMPLE WEIGHT //
    sampleWeight   = scaleFactor; 
    sampleWeightW  = 1;
    sampleWeightDY = 1;
    weightHepNup   = 1;
    weightHepNupHighStatW =1;
    weightHepNupDY = 1;
    weightTauFakeWJet_ = 1; weightTauFakeWJetUp_ = 1; weightTauFakeWJetDown_ = 1;

    //jet->tau fake correction for antiiso events in the QCD estimation (taken from mutau measurements, see createCorrections_Summer14.C)
    //new implementation by Olivier - July 14

    TFile f_JetFakeCorrection("/data_CMS/cms/htautau/PostMoriond/tools/llrCorrections_Summer14_v9_MVAIso.root");

    //Get the functions
    TF1* QCDWeight_mutau_central = (TF1*)f_JetFakeCorrection.Get("QCDWeight_mutau_central");
    TF1* QCDWeight_mutau_medium = (TF1*)f_JetFakeCorrection.Get("QCDWeight_mutau_medium");
    TF1* QCDWeight_mutau_forward = (TF1*)f_JetFakeCorrection.Get("QCDWeight_mutau_forward");

    //init
    weightJetFakeQCD_=1.;

    //compute the weights
    if(TMath::Abs(etaL2)<=1.2 )
      {
	weightJetFakeQCD_=QCDWeight_mutau_central->Eval(ptL2);
      }
    else if(TMath::Abs(etaL2)>1.2 && TMath::Abs(etaL2)<=1.7)
      {
	weightJetFakeQCD_=QCDWeight_mutau_medium->Eval(ptL2);
      }
    else if(TMath::Abs(etaL2)>1.7)
      {
	weightJetFakeQCD_=QCDWeight_mutau_forward->Eval(ptL2);
      } 

    if(weightJetFakeQCD_<0.) weightJetFakeQCD_=0.;
    
//     //jet->tau fake correction for antiiso events in the QCD estimation (taken from thth measurements
//     TFile f_JetFakeCorrection("/data_CMS/cms/htautau/PostMoriond/tools/QCDShapeCorrections/determineJetToTauFakeRate_MVAwLToldDMsTight.root");
//     if(f_JetFakeCorrection.IsZombie())
//       cout << "Jet Fake correction file not available" << endl;   
//     TF1 *JetFakeCorrectionEtaGt17 = (TF1*)f_JetFakeCorrection.Get("jetToTauFakeRate/inclusive/tau1EtaGt17/fitFunctionShape_tau1PtL_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag");
//     TF1 *JetFakeCorrectionEta12to17 = (TF1*)f_JetFakeCorrection.Get("jetToTauFakeRate/inclusive/tau1Eta12to17/fitFunctionShape_tau1PtL_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag");
//     TF1 *JetFakeCorrectionEtaLt12 = (TF1*)f_JetFakeCorrection.Get("jetToTauFakeRate/inclusive/tau1EtaLt12/fitFunctionShape_tau1PtL_SSiso1_iso2_LooseBtag_div_SSrelaxed1_iso2_LooseBtag");
//     weightJetFakeQCD_=1;
//     if( TMath::Abs(etaL2)<=1.2 )
//       {
// 	weightJetFakeQCD_=JetFakeCorrectionEtaLt12->Eval(ptL2);
//       }
//     else if( TMath::Abs(etaL2)>1.2 && TMath::Abs(etaL2)<=1.7)
//       {
// 	weightJetFakeQCD_=JetFakeCorrectionEta12to17->Eval(ptL2);
//       }
//     else if( TMath::Abs(etaL2)>1.7)
//       {
// 	weightJetFakeQCD_=JetFakeCorrectionEtaGt17->Eval(ptL2);
//       }

    // Reweight W+Jets
    //cout << "SAMPLE : " << sample_ << endl;
    int localNup=0;
    if( (sample_.find("WJets")!=string::npos && sample_.find("WWJets")==string::npos ) || 
	sample_.find("W1Jets")!=string::npos || sample_.find("W2Jets")!=string::npos || 
	sample_.find("W3Jets")!=string::npos || sample_.find("W4Jets")!=string::npos
        ) {
      //cout << "=> computing weight : hepNUP=" << hepNUP ;
      if(     sample_.find("1Jets")!=string::npos) localNup=6;
      else if(sample_.find("2Jets")!=string::npos) localNup=7;
      else if(sample_.find("3Jets")!=string::npos) localNup=8;
      else if(sample_.find("4Jets")!=string::npos) localNup=9;
      if(hepNUP>=5 && hepNUP<=9) localNup=hepNUP;

      weightHepNup          = reweightHEPNUPWJets( localNup, 0 );
      weightHepNupHighStatW = reweightHEPNUPWJets( localNup, 1 );
      sampleWeight = 1;
      sampleWeightW= scaleFactor; 
      //cout << " => weightHepNup=" << weightHepNup << " ; weightHepNupHighStatW=" << weightHepNupHighStatW << endl;
      //Add a weight for tauPt reweighting , coefficiencts are from Andrew
      float ptTau_ = ptL2;
      if(ptTau_ > 200.)ptTau_ = 200.;
//       weightTauFakeWJet_ = 1.15743 - 0.00736136*ptTau_ + 0.000043699*ptTau_*ptTau_ - 0.0000001188*ptTau_*ptTau_*ptTau_;
      weightTauFakeWJet_ = getTauFakeCorrection(ptTau_);
      weightTauFakeWJetUp_ = weightTauFakeWJet_ + 0.50*(1.0 - weightTauFakeWJet_);
      weightTauFakeWJetDown_ = weightTauFakeWJet_ - 0.50*(1.0 - weightTauFakeWJet_);
    }

    // Reweight DY+Jets
    if( sample_.find("DYJets")!=string::npos  || 
	sample_.find("DY1Jets")!=string::npos || sample_.find("DY2Jets")!=string::npos || 
	sample_.find("DY3Jets")!=string::npos || sample_.find("DY4Jets")!=string::npos
        ) {
      weightHepNupDY = reweightHEPNUPDYJets( hepNUP );
      sampleWeight   = 1;
      sampleWeightDY = scaleFactor; 
    }
    ///////////////

    highPtWeightUp =1;
    highPtWeightDown =1;
    if( sample_.find("SUSY")!=string::npos){
      highPtWeightUp =1 + 0.20*(*genDiTauLegsP4)[1].Pt();
      highPtWeightDown =1 - 0.20*(*genDiTauLegsP4)[1].Pt();
    }

    //Top pT weights
    topPtWeightNom_=1;
    topPtWeightUp_=1;
    topPtWeightDown_=1;
    if( sample_.find("TTJets")!=string::npos ) 
      {
	topPtWeightNom_=topPtWeightNom;
	topPtWeightUp_=topPtWeightUp;
	topPtWeightDown_=topPtWeightDown;
      }

    HqTWeight = histo!=0 ? histo->GetBinContent( histo->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
    HqTWeightUp = histoUp!=0 ? histoUp->GetBinContent( histoUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
    HqTWeightDown = histoDown!=0 ? histoDown->GetBinContent( histoDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

    if(SampleT.Contains("SUSY") && SampleT.Contains("GGH"))
      {
	mssmHiggsPtReweightGluGlu_mhmax = h_mhmax!=0 ? h_mhmax->GetBinContent( h_mhmax->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_HqTUp = h_mhmax_HqTUp!=0 ? h_mhmax_HqTUp->GetBinContent( h_mhmax_HqTUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_HqTDown = h_mhmax_HqTDown!=0 ? h_mhmax_HqTDown->GetBinContent( h_mhmax_HqTDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_HIGLUUp = h_mhmax_HIGLUUp!=0 ? h_mhmax_HIGLUUp->GetBinContent( h_mhmax_HIGLUUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_HIGLUDown = h_mhmax_HIGLUDown!=0 ? h_mhmax_HIGLUDown->GetBinContent( h_mhmax_HIGLUDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_tanBetaUp = h_mhmax_tanBetaUp!=0 ? h_mhmax_tanBetaUp->GetBinContent( h_mhmax_tanBetaUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmax_tanBetaDown = h_mhmax_tanBetaDown!=0 ? h_mhmax_tanBetaDown->GetBinContent( h_mhmax_tanBetaDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

	mssmHiggsPtReweightGluGlu_mhmodplus = h_mhmodplus!=0 ? h_mhmodplus->GetBinContent( h_mhmodplus->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_HqTUp = h_mhmodplus_HqTUp!=0 ? h_mhmodplus_HqTUp->GetBinContent( h_mhmodplus_HqTUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_HqTDown = h_mhmodplus_HqTDown!=0 ? h_mhmodplus_HqTDown->GetBinContent( h_mhmodplus_HqTDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUUp = h_mhmodplus_HIGLUUp!=0 ? h_mhmodplus_HIGLUUp->GetBinContent( h_mhmodplus_HIGLUUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_HIGLUDown = h_mhmodplus_HIGLUDown!=0 ? h_mhmodplus_HIGLUDown->GetBinContent( h_mhmodplus_HIGLUDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaUp = h_mhmodplus_tanBetaUp!=0 ? h_mhmodplus_tanBetaUp->GetBinContent( h_mhmodplus_tanBetaUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodplus_tanBetaDown = h_mhmodplus_tanBetaDown!=0 ? h_mhmodplus_tanBetaDown->GetBinContent( h_mhmodplus_tanBetaDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

	mssmHiggsPtReweightGluGlu_mhmodminus = h_mhmodminus!=0 ? h_mhmodminus->GetBinContent( h_mhmodminus->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_HqTUp = h_mhmodminus_HqTUp!=0 ? h_mhmodminus_HqTUp->GetBinContent( h_mhmodminus_HqTUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_HqTDown = h_mhmodminus_HqTDown!=0 ? h_mhmodminus_HqTDown->GetBinContent( h_mhmodminus_HqTDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUUp = h_mhmodminus_HIGLUUp!=0 ? h_mhmodminus_HIGLUUp->GetBinContent( h_mhmodminus_HIGLUUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_HIGLUDown = h_mhmodminus_HIGLUDown!=0 ? h_mhmodminus_HIGLUDown->GetBinContent( h_mhmodminus_HIGLUDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaUp = h_mhmodminus_tanBetaUp!=0 ? h_mhmodminus_tanBetaUp->GetBinContent( h_mhmodminus_tanBetaUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_mhmodminus_tanBetaDown = h_mhmodminus_tanBetaDown!=0 ? h_mhmodminus_tanBetaDown->GetBinContent( h_mhmodminus_tanBetaDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

	mssmHiggsPtReweightGluGlu_lowmH = h_lowmH!=0 ? h_lowmH->GetBinContent( h_lowmH->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_HqTUp = h_lowmH_HqTUp!=0 ? h_lowmH_HqTUp->GetBinContent( h_lowmH_HqTUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_HqTDown = h_lowmH_HqTDown!=0 ? h_lowmH_HqTDown->GetBinContent( h_lowmH_HqTDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_HIGLUUp = h_lowmH_HIGLUUp!=0 ? h_lowmH_HIGLUUp->GetBinContent( h_lowmH_HIGLUUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_HIGLUDown = h_lowmH_HIGLUDown!=0 ? h_lowmH_HIGLUDown->GetBinContent( h_lowmH_HIGLUDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_tanBetaUp = h_lowmH_tanBetaUp!=0 ? h_lowmH_tanBetaUp->GetBinContent( h_lowmH_tanBetaUp->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
	mssmHiggsPtReweightGluGlu_lowmH_tanBetaDown = h_lowmH_tanBetaDown!=0 ? h_lowmH_tanBetaDown->GetBinContent( h_lowmH_tanBetaDown->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;
      }
    
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

    //TauID
    decayModeFinding_           = decayModeFinding;
    decayModeFindingNewDM_      = decayModeFindingNewDM;
    decayModeFindingOldDM_      = decayModeFindingOldDM;
    AntiEDeadEcal_              = AntiEDeadEcal;
    tightestAntiECutWP_         = tightestAntiECutWP;
    tightestAntiEMVA5WP_        = tightestAntiEMVA5WP;
    AntiEMVA5category_          = AntiEMVA5category;
    AntiEMVA5raw_               = AntiEMVA5raw;
    tightestAntiMuWP_           = tightestAntiMuWP;
    tightestAntiMu2WP_          = tightestAntiMu2WP;
    tightestAntiMu3WP_          = tightestAntiMu3WP;
    tightestAntiMuMVAWP_        = tightestAntiMuMVAWP;
    AntiMuMVAraw_               = AntiMuMVAraw;
    tightestHPSDBWP_            = tightestHPSDBWP;
    tightestHPSDB3HWP_          = tightestHPSDB3HWP;
    hpsDB3H_                    = hpsDB3H;
    tightestHPSMVA3newDMwLTWP_  = tightestHPSMVA3newDMwLTWP;
    hpsMVA3newDMwLT_            = hpsMVA3newDMwLT;
    tightestHPSMVA3newDMwoLTWP_ = tightestHPSMVA3newDMwoLTWP;
    hpsMVA3newDMwoLT_           = hpsMVA3newDMwoLT;
    tightestHPSMVA3oldDMwLTWP_  = tightestHPSMVA3oldDMwLTWP;
    hpsMVA3oldDMwLT_            = hpsMVA3oldDMwLT;
    tightestHPSMVA3oldDMwoLTWP_ = tightestHPSMVA3oldDMwoLTWP;
    hpsMVA3oldDMwoLT_           = hpsMVA3oldDMwoLT;
 
    ///NewTauID input variables
    chargedIsoPtSum_      = chargedIsoPtSum;
    neutralIsoPtSum_      = neutralIsoPtSum;
    puCorrPtSum_          = puCorrPtSum;
    patDecayMode_         = patDecayMode;
    dxyTau_               = dxyTau;
    dxyErrTau_            = dxyErrTau;
    dxySigTau_            = dxySigTau;
    hasSecVtx_            = hasSecVtx;
    flightLength2_        = flightLength2;
    flightLengthSig_      = flightLengthSig;
    ///NewTauID useful variables
    dxyPCAX_              = dxyPCAX;
    dxyPCAY_              = dxyPCAY;
    dxyPCAZ_              = dxyPCAZ;
    flightLengthX_        = flightLengthX;
    flightLengthY_        = flightLengthY;
    flightLengthZ_        = flightLengthZ;
    pvX_                  = pvX;
    pvY_                  = pvY;
    pvZ_                  = pvZ;
    pvCov00_              = pvCov00;
    pvCov01_              = pvCov01;
    pvCov02_              = pvCov02;
    pvCov11_              = pvCov11;
    pvCov12_              = pvCov12;
    pvCov22_              = pvCov22;
    svX_                  = svX;
    svY_                  = svY;
    svZ_                  = svZ;
    svCov00_              = svCov00;
    svCov01_              = svCov01;
    svCov02_              = svCov02;
    svCov11_              = svCov11;
    svCov12_              = svCov12;
    svCov22_              = svCov22;
   
    ////////////////////////////////
    // CORRECTIONS : TRIGGER & SF //
    ////////////////////////////////

    if(DEBUG) cout << "Start checking trigger information" << endl;

    if( isData && !sample.Contains("Emb") ){

      L1etmCorrDown_    = L1etmCorrUp_    = L1etmCorr_    = L1etm_;     
      L1etmPhiCorrDown_ = L1etmPhiCorrUp_ = L1etmPhiCorr_ = L1etmPhi_; 

      if(isPeriodLow)       etmCut=20;
      else if(isPeriodHigh) etmCut=26;
      else etmCut=20;

      passL1etmCutUp_    = passL1etmCutDown_    = passL1etmCut_    = float(L1etm_>etmCut);
      passL1etmCutABCUp_ = passL1etmCutABCDown_ = passL1etmCutABC_ = float(L1etm_>20);

      puWeight         = 1.0;
      puWeightHCP      = 1.0;
      puWeightD        = 1.0;
      puWeightDLow     = 1.0;
      puWeightDHigh    = 1.0;
      puWeight2        = 1.0;

      // HLT Paths matching

      HLTxMu8     = 1.0; // ND correct ? trigger not present in data !
      HLTmatchMu8 = 1.0; // ND
      HLTxMu17Mu8 = 1.0; // required only in embedded

      //checking triggers: 

      isMatched = false;
      isMatched = (*triggerPaths)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4"] 
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v5"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v6"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v3"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v4"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v5"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v6"]
	|| (*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v7"]; 
      // HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4-6 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2-7
      HLTx = isMatched ? 1.0 : 0.0 ;

      isMatched = false;
      isMatched = (*triggerPaths)["HLT_PFJet320_v3"]
	|| (*triggerPaths)["HLT_PFJet320_v4"]
	|| (*triggerPaths)["HLT_PFJet320_v5"]
	|| (*triggerPaths)["HLT_PFJet320_v6"]
	|| (*triggerPaths)["HLT_PFJet320_v7"]
	|| (*triggerPaths)["HLT_PFJet320_v8"]
	|| (*triggerPaths)["HLT_PFJet320_v9"]
	|| (*triggerPaths)["HLT_PFJet320_v10"];
      HLTPFJet320 = isMatched ? 1.0 : 0.0 ;

      isMatched = false;
      isMatched = (*triggerPaths)["HLT_IsoMu24_v15"]
	|| (*triggerPaths)["HLT_IsoMu24_v16"]
	|| (*triggerPaths)["HLT_IsoMu24_v17"];
      HLTIsoMu24 = isMatched ? 1. : 0. ;

      isMatched = false;
      isMatched = (*triggerPaths)["HLT_IsoMu24_eta2p1_v11"]
	|| (*triggerPaths)["HLT_IsoMu24_eta2p1_v12"]
	|| (*triggerPaths)["HLT_IsoMu24_eta2p1_v13"]
	|| (*triggerPaths)["HLT_IsoMu24_eta2p1_v14"]
	|| (*triggerPaths)["HLT_IsoMu24_eta2p1_v15"];
      HLTIsoMu24_eta2p1 = isMatched ? 1. : 0. ;

      /*
      isMatched = false;
      for(int i=0 ; i<9 ; i++)
	isMatched = isMatched || (*triggerBits)[i]; // HLT_IsoMu18_eta2p1_LooseIsoPFTau20_v4-6 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2-7
      HLTx = isMatched ? 1.0 : 0.0 ;
      */

      isMatched = false;
      isMatched = (*triggerPaths)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v4"]
	|| (*triggerPaths)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v5"]
	|| (*triggerPaths)["HLT_Mu18_eta2p1_LooseIsoPFTau20_v6"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v2"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v3"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v4"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v5"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v6"]
	|| (*triggerPaths)["HLT_Mu17_eta2p1_LooseIsoPFTau20_v7"];
      // HLT_Mu18_eta2p1_LooseIsoPFTau20_v4-6 , HLT_Mu17_eta2p1_LooseIsoPFTau20_v2-7
      HLTxQCD = isMatched ? 1.0 : 0.0 ;

      /*
      isMatched = false;
      for(int i=9 ; i<18 ; i++)
	isMatched = isMatched || (*triggerBits)[i]; // HLT_Mu18_eta2p1_LooseIsoPFTau20_v4-6 , HLT_Mu17_eta2p1_LooseIsoPFTau20_v2-7
      HLTxQCD = isMatched ? 1.0 : 0.0 ;
      */

      HLTxSoft    = float((*triggerPaths)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1"]); // HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1
      HLTxQCDSoft = float((*triggerPaths)["HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1"]); // HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1

      HLTxIsoMu8Tau20 = float((*triggerPaths)["HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1"]); // HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1

      /*
      HLTxSoft    = float((*triggerBits)[18]); // HLT_IsoMu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1
      HLTxQCDSoft = float((*triggerBits)[19]); // HLT_Mu8_eta2p1_LooseIsoPFTau20_L1ETM26_v1

      HLTxIsoMu8Tau20 = float((*triggerBits)[20]); // HLT_IsoMu8_eta2p1_LooseIsoPFTau20_v1
      */

      isMatched=false;
      isMatched = (*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v3"]
	|| (*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v4"]
	|| (*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v5"]
	|| (*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v6"]
	|| (*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v7"];
      HLTxIsoMu15ETM20 = isMatched ? 1.0 : 0.0 ;  // HLT_IsoMu15_eta2p1_L1ETM20_v3-7

      /*
      isMatched=false;
      for(int i=21 ; i<26 ; i++)
	isMatched = isMatched || (*triggerBits)[i];
      HLTxIsoMu15ETM20 = isMatched ? 1.0 : 0.0 ;  // HLT_IsoMu15_eta2p1_L1ETM20_v3-7
      */

      isMatched = false; 
      isMatched = (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v5"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v4"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v3"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v3"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v1"]
	|| (*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v4"];
      // HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v*, HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v*
      HLTxDiTau = isMatched ? 1.0 : 0.0 ;

      /*
      isMatched = false; 
      for(int i=33 ; i<40 ; i++) 
        isMatched = isMatched || (*triggerBits)[i]; // HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v*, HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk1_eta2p1_v*
      HLTxDiTau = isMatched ? 1.0 : 0.0 ;
      */

      // HLT filters matching

      isMatched = (((*HLTfiltersMu)["hltOverlapFilterIsoMu18LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterIsoMu18LooseIsoPFTau20"]) || // hltOverlapFilterIsoMu18LooseIsoPFTau20 (mu && tau)
		   ((*HLTfiltersMu)["hltOverlapFilterIsoMu17LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterIsoMu17LooseIsoPFTau20"]));  // hltOverlapFilterIsoMu17LooseIsoPFTau20 (mu && tau)
      HLTmatch = isMatched ? 1.0 : 0.0 ;

      /*
      isMatched = (((*tauXTriggers)[2]  && (*tauXTriggers)[20]) || // hltOverlapFilterIsoMu18LooseIsoPFTau20 (mu && tau)
		   ((*tauXTriggers)[3]  && (*tauXTriggers)[21]));  // hltOverlapFilterIsoMu17LooseIsoPFTau20 (mu && tau)
      HLTmatch = isMatched ? 1.0 : 0.0 ;
      */
      
      //HLTmatchMu  = ((*tauXTriggers)[2] || (*tauXTriggers)[3]);

      HLTmatchMu  = ((*HLTfiltersMu)["hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15"] || (*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"]); //hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
      HLTmatchTau = ((*HLTfiltersTau)["hltOverlapFilterIsoMu18LooseIsoPFTau20"] || (*HLTfiltersTau)["hltOverlapFilterIsoMu17LooseIsoPFTau20"]);

      /*
      HLTmatchMu  = ((*tauXTriggers)[14] || (*tauXTriggers)[18]); //hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
      HLTmatchTau = ((*tauXTriggers)[20] || (*tauXTriggers)[21]);
      */

      isMatched = (((*HLTfiltersMu)["hltOverlapFilterMu18LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterMu18LooseIsoPFTau20"]) || // hltOverlapFilterMu18LooseIsoPFTau20
		   ((*HLTfiltersMu)["hltOverlapFilterMu17LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterMu17LooseIsoPFTau20"]));  // hltOverlapFilterMu17LooseIsoPFTau20
      HLTmatchQCD = isMatched ? 1.0 : 0.0 ;       

      /*
      isMatched = (((*tauXTriggers)[4]  && (*tauXTriggers)[22]) || // hltOverlapFilterMu18LooseIsoPFTau20
		   ((*tauXTriggers)[5]  && (*tauXTriggers)[23]));  // hltOverlapFilterMu17LooseIsoPFTau20
      HLTmatchQCD = isMatched ? 1.0 : 0.0 ;   
      */

      isMatched = (((*HLTfiltersMu)["hltOverlapFilterIsoMu8LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterIsoMu8LooseIsoPFTau20"]));  // hltOverlapFilterIsoMu8LooseIsoPFTau20
      //isMatched &= (L1etm_>26); // MB is this x-check needed?
      HLTmatchSoft = isMatched ? 1.0 : 0.0 ;

      /*
      isMatched = (((*tauXTriggers)[7]  && (*tauXTriggers)[24]));  // hltOverlapFilterIsoMu8LooseIsoPFTau20
      //isMatched &= (L1etm_>26); // MB is this x-check needed?
      HLTmatchSoft = isMatched ? 1.0 : 0.0 ;
      */

      isMatched = (((*HLTfiltersMu)["hltOverlapFilterMu8LooseIsoPFTau20"]  && (*HLTfiltersTau)["hltOverlapFilterMu8LooseIsoPFTau20"]));  // hltOverlapFilterMu8LooseIsoPFTau20
      //isMatched &= (L1etm_>26); // MB is this x-check needed?
      HLTmatchQCDSoft = isMatched ? 1.0 : 0.0 ;      

      /*
      isMatched = (((*tauXTriggers)[11]  && (*tauXTriggers)[26]));  // hltOverlapFilterMu8LooseIsoPFTau20
      //isMatched &= (L1etm_>26); // MB is this x-check needed?
      HLTmatchQCDSoft = isMatched ? 1.0 : 0.0 ;      
      */

      HLTmatchIsoMu8Tau20  = float((*HLTfiltersMu)["hltOverlapFilterSingleIsoMu8LooseIsoPFTau20"] && (*HLTfiltersTau)["hltOverlapFilterSingleIsoMu8LooseIsoPFTau20"]); // hltOverlapFilterSingleIsoMu8LooseIsoPFTau20
      HLTmatchIsoMu15ETM20 = float( (*HLTfiltersMu)["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoFiltered10"] || // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoFiltered10
				    (*HLTfiltersMu)["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15"] ); // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15

      /*
      HLTmatchIsoMu8Tau20  = float((*tauXTriggers)[9] && (*tauXTriggers)[25]); // hltOverlapFilterSingleIsoMu8LooseIsoPFTau20
      HLTmatchIsoMu15ETM20 = float( (*tauXTriggers)[12] || // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoFiltered10
				    (*tauXTriggers)[13] ); // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15
      */

      //HLT matching for DiTau trigger measurement 
      HLTmatchDiTau       = float( ((*HLTfiltersMu)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] && (*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"]) || // hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4 
				   ((*HLTfiltersMu)["hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"] && (*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"])); //hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4
      HLTmatchDiTauMu     = float((*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15"] || (*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"]); //hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10 
      HLTmatchDiTauTau    = float((*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] || (*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4"]); //hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4 or hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4  
      HLTmatchDiTauL2     = float((*HLTfiltersTau)["hltL2Tau25eta2p1"] && (*HLTfiltersTau)["hltL2TauIsoFilter"]); //hltL2Tau25eta2p1 and hltL2TauIsoFilter 
      HLTmatchDiTauL1     = float((*HLTfiltersTau)["l1tau1"]); //l1extra cenJet or TauJet

      HLTmatchIsoMu24      = float((*HLTfiltersMu)["hltL3crIsoL1sMu16L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"]);
      HLTmatchIsoMu24_eta2p1 = float((*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"]);
      HLTmatchPFJet320     = float((*HLTfiltersTauJet)["hlt1PFJet320"]);

      /*
      //HLT matching for DiTau trigger measurement 
      HLTmatchDiTau       = float( ((*tauXTriggers)[16] && (*tauXTriggers)[27]) || // hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4 
				   ((*tauXTriggers)[17] && (*tauXTriggers)[28])); //hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4
      HLTmatchDiTauMu     = float((*tauXTriggers)[15] || (*tauXTriggers)[18]); //hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10 
      HLTmatchDiTauTau    = float((*tauXTriggers)[27] || (*tauXTriggers)[28]); //hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4 or hltOverlapFilterIsoMu18PFTau25TrackPt1Prong4  
      HLTmatchDiTauL2     = float((*tauXTriggers)[29] && (*tauXTriggers)[30]); //hltL2Tau25eta2p1 and hltL2TauIsoFilter 
      HLTmatchDiTauL1     = float((*tauXTriggers)[31]); //l1extra cenJet or TauJet
      */

      SFTau  = 1.0;

      HLTTauABC   = 1.0;
      HLTTauMCABC = 1.0;
      HLTTauD     = 1.0;
      HLTTauMCD   = 1.0;
      HLTTau      = 1.0;
      HLTTauMC    = 1.0;
      HLTweightTau    = 1.0;
      HLTweight2Triggers = 1.0;
      HLTweightTauD   = 1.0;
      HLTweightTauABC = 1.0;
      HLTMuA     = 1.0;
      HLTMuB     = 1.0;
      HLTMuC     = 1.0;
      HLTMuD     = 1.0;
      HLTMuMCold = 1.0;
      HLTMuABCD  = 1.0;
      HLTMuMCnew = 1.0;
      HLTMuSoft   = 1.0;
      HLTMuSoftMC = 1.0;
      HLTMuShift   = 1.0;
      HLTMuShiftMC = 1.0;
      HLTMuABCShift   = 1.0;
      HLTMuShiftMCold = 1.0;
      HLTMuABC       = 1.0;
      HLTweightMuA    = 1.0;
      HLTweightMuB    = 1.0;
      HLTweightMuC    = 1.0;
      HLTweightMuD    = 1.0;
      HLTweightMuABC  = 1.0;
      HLTweightMuABCD = 1.0;
      HLTweightMuSoft = 1.0;
      HLTweightMuShift = 1.0;
      HLTweightMuABCShift = 1.0;
      HLTMu = 1.0;
      HLTweightMu = 1.0;
      SFMuID_ABCD  = 1.0;
      SFMuIso_ABCD = 1.0;
      SFMu_ABCD    = 1.0;
      SFMuID_ABC  = 1.0;
      SFMuIso_ABC = 1.0;
      SFMu_ABC    = 1.0;
      SFMuID_D  = 1.0;
      SFMuIso_D = 1.0;
      SFMu_D    = 1.0;
      SFMu    = 1.0;
      SFMuID  = 1.0;
      SFMuIso = 1.0;

    } // end if Data

    else { // MC or embedded
      if(DEBUG) cout << "-- MC or embedded" << endl;

      HLTxQCD         = 1.0;
      HLTmatchQCD     = 1.0;
      HLTxSoft        = 1.0;
      HLTxQCDSoft     = 1.0;
      HLTxIsoMu8Tau20 = 1.0;

      if( !sample.Contains("Emb") || sample.Contains("TTJets-Embedded")  ) { // Check trigger matching only for MC
	
	// L1 ETM
	L1etmCorr_     = correctL1etm(L1etm_, caloMEtNoHFUncorr_, caloMEtNoHF_);
	L1etmCorrUp_   = correctL1etm(L1etm_, caloMEtNoHFUncorr_, caloMEtNoHFUp_);
	L1etmCorrDown_ = correctL1etm(L1etm_, caloMEtNoHFUncorr_, caloMEtNoHFDown_);

	L1etmPhiCorr_ = L1etmPhiCorrUp_ = L1etmPhiCorrDown_ = L1etmPhi_; 
	
	//if(gRandom->Uniform()>(1.0-4.806/7.274) ) etmCut=26;
	if(gRandom->Uniform()>(1.0-4.848/7.317) ) etmCut=26;
	else                                      etmCut=20;

	// Pile-Up
	puWeight2        = pileupWeight2(int(nPUVertices));  
	puWeight         = pileupWeight(nPUVertices, "");
	puWeightHCP      = pileupWeight(nPUVertices, "HCP");   
	puWeightD        = pileupWeight(nPUVertices, "D" );
	if(etmCut==20) {
	  puWeightDLow  = pileupWeight(nPUVertices, "DLow" );
	  puWeightDHigh = 1.0;
	} else {
	  puWeightDLow  = 1.0;
	  puWeightDHigh = pileupWeight(nPUVertices, "DHigh" );
	}

	// HLT Paths matching
	HLTx = float((*triggerPaths)["HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2"]);
	HLTxMu8 = float((*triggerPaths)["HLT_Mu8_v16"]);
	HLTxIsoMu15ETM20 = float((*triggerPaths)["HLT_IsoMu15_eta2p1_L1ETM20_v5"]);
	HLTxDiTau = float((*triggerPaths)["HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7"]);
	HLTIsoMu24 = float((*triggerPaths)["HLT_IsoMu24_v15"]);
	HLTIsoMu24_eta2p1 = float((*triggerPaths)["HLT_IsoMu24_eta2p1_v13"]);
	HLTPFJet320 = float((*triggerPaths)["HLT_PFJet320_v5"]);
	/*
	  HLTx             = float((*triggerBits)[0]); // HLT_IsoMu17_eta2p1_LooseIsoPFTau20_v2
	  HLTxMu8          = float((*triggerBits)[1]); // HLT_Mu8_v16 //MB
	  HLTxIsoMu15ETM20 = float((*triggerBits)[2]); // HLT_IsoMu15_eta2p1_L1ETM20_v5
	  HLTxDiTau        = float((*triggerBits)[3]); // HLT_IsoMu18_eta2p1_MediumIsoPFTau25_Trk5_eta2p1_v7
	*/

	// HLT filters matching
	HLTmatch             = float((*HLTfiltersMu)["hltOverlapFilterIsoMu17LooseIsoPFTau20"] && (*HLTfiltersTau)["hltOverlapFilterIsoMu17LooseIsoPFTau20"]) ; // hltOverlapFilterIsoMu17LooseIsoPFTau20
	//HLTmatchMu           = float((*HLTfiltersMu)[2]);
	HLTmatchMu           = float((*HLTfiltersMu)["hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15"] || (*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"]); //hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15 || hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
	HLTmatchTau          = float((*HLTfiltersTau)["hltOverlapFilterIsoMu17LooseIsoPFTau20"]);
	HLTmatchIsoMu15ETM20 = float((*HLTfiltersMu)["hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15"] && (L1etmCorr_>20));     // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15
	HLTmatchMu8          = float((*HLTfiltersMu)["hltL3fL1sMu3L3Filtered8"]); // hltL3fL1sMu3L3Filtered8

	HLTxMu17Mu8     = 1.0; // required only in embedded

	HLTmatchIsoMu24      = float((*HLTfiltersMu)["hltL3crIsoL1sMu16L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"]);
	HLTmatchIsoMu24_eta2p1  = float((*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f24QL3crIsoRhoFiltered0p15"]);
	HLTmatchPFJet320     = float((*HLTfiltersTauJet)["hlt1PFJet320"]);

	// emulate matching to SoftMu+L1ETM+Tau
	isMatched = ( (*HLTfiltersMu)["hltL3fL1sMu3L3Filtered8"] &&  // HLT_Mu8 (hltL3fL1sMu3L3Filtered8)
		      (*HLTfiltersMu)["l1mu1"] &&  // L1ExtraMu Pt>7, |eta|<2.1 (hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15)
		      (*HLTfiltersTau)["l1tau1"] );  // offline trgTau

	HLTmatchIsoMu8Tau20 = float(isMatched);
	HLTmatchSoft        = float(isMatched && L1etmCorr_>etmCut);
	//HLTmatchSoft        = float(isMatched && L1etmCorr_>etmCut);
	HLTmatchQCDSoft     = float(isMatched && L1etmCorr_>etmCut);


	//HLT matching for DiTau trigger measurement
	HLTmatchDiTau       = float((*HLTfiltersMu)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"] && (*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"]) ; // hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4
        HLTmatchDiTauMu     = float((*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15"] || (*HLTfiltersMu)["hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10"]); //hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
        HLTmatchDiTauTau    = float((*HLTfiltersTau)["hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4"]); //hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4
	HLTmatchDiTauL2     = float((*HLTfiltersTau)["hltL2Tau25eta2p1"] && (*HLTfiltersTau)["hltL2TauIsoFilter"]); //hltL2Tau25eta2p1 and hltL2TauIsoFilter
	HLTmatchDiTauL1     = float((*HLTfiltersTau)["l1tau1"]); //l1extra cenJet or TauJet

	/*
	HLTmatch             = float((*tauXTriggers)[2] && (*tauXTriggers)[9]) ; // hltOverlapFilterIsoMu17LooseIsoPFTau20
	//HLTmatchMu           = float((*tauXTriggers)[2]);
	HLTmatchMu           = float((*tauXTriggers)[1] || (*tauXTriggers)[7]); //hltL3crIsoL1sMu14erORMu16erL1f0L2f14QL3f17QL3crIsoRhoFiltered0p15 || hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
	HLTmatchTau          = float((*tauXTriggers)[9]);
	HLTmatchIsoMu15ETM20 = float((*tauXTriggers)[4] && (L1etmCorr_>20));     // hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15
	HLTmatchMu8          = float((*tauXTriggers)[3]); // hltL3fL1sMu3L3Filtered8

	HLTxMu17Mu8     = 1.0; // required only in embedded

	// emulate matching to SoftMu+L1ETM+Tau
	isMatched = ( (*tauXTriggers)[3] &&  // HLT_Mu8 (hltL3fL1sMu3L3Filtered8)
		      (*tauXTriggers)[8] &&  // L1ExtraMu Pt>7, |eta|<2.1 (hltL3crIsoL1sMu12Eta2p1L1f0L2f12QL3f15QL3crIsoRhoFiltered0p15)
		      (*tauXTriggers)[13] );  // offline trgTau

	HLTmatchIsoMu8Tau20 = float(isMatched);
	HLTmatchSoft        = float(isMatched && L1etmCorr_>etmCut);
	//HLTmatchSoft        = float(isMatched && L1etmCorr_>etmCut);
	HLTmatchQCDSoft     = float(isMatched && L1etmCorr_>etmCut);


	//HLT matching for DiTau trigger measurement
	HLTmatchDiTau       = float((*tauXTriggers)[6] && (*tauXTriggers)[10]) ; // hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4
        HLTmatchDiTauMu     = float((*tauXTriggers)[5] || (*tauXTriggers)[7]); //hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoRhoFiltered0p15 or hltL3crIsoL1sMu16Eta2p1L1f0L2f16QL3f18QL3crIsoFiltered10
        HLTmatchDiTauTau    = float((*tauXTriggers)[10]); //hltOverlapFilterIsoMu18PFTau25TrackPt5Prong4
	HLTmatchDiTauL2     = float((*tauXTriggers)[11] && (*tauXTriggers)[12]); //hltL2Tau25eta2p1 and hltL2TauIsoFilter
	HLTmatchDiTauL1     = float((*tauXTriggers)[13]); //l1extra cenJet or TauJet
	*/

	passL1etmCut_       = float(L1etmCorr_>etmCut);
	passL1etmCutABC_    = float(L1etmCorr_>20);

	passL1etmCutUp_     = float(L1etmCorrUp_>etmCut);
	passL1etmCutABCUp_  = float(L1etmCorrUp_>20);

	passL1etmCutDown_   = float(L1etmCorrDown_>etmCut);
	passL1etmCutABCDown_= float(L1etmCorrDown_>20);

      }
      else { // embedded
	HLTx = HLTxMu8 = HLTxIsoMu15ETM20 = HLTmatchMu = HLTmatchTau = HLTmatch = HLTmatchMu8 = HLTmatchSoft = HLTmatchQCDSoft = HLTmatchIsoMu15ETM20 = HLTmatchIsoMu8Tau20 = HLTmatchDiTau = HLTmatchDiTauL1 = HLTmatchDiTauL2 = HLTmatchDiTauMu = HLTmatchDiTauTau = 1.0;

	HLTPFJet320 = 1. ;
	HLTmatchPFJet320 = 1. ;

	HLTIsoMu24 = 1. ;
	HLTmatchIsoMu24 = 1. ;

	L1etmCorrDown_    = L1etmCorrUp_    = L1etmCorr_    = L1etm_ ;
	L1etmPhiCorrDown_ = L1etmPhiCorrUp_ = L1etmPhiCorr_ = L1etmPhi_ ;

	// Get L1ETM without tau correction (only for embedded)
	if(l1ETMP4->size()>1){
	  L1etm_     = (*l1ETMP4)[1].Et(); // ND
	  L1etmPhi_  = (*l1ETMP4)[1].Phi();// ND
	}

	// NEW CORRECTION FOR EMBEDDED FROM LUCA
	if(ptL2>30) L1etmCorrDown_    = L1etmCorrUp_    = L1etmCorr_    = L1etm_ * 0.973 ;
	else        L1etmCorrDown_    = L1etmCorrUp_    = L1etmCorr_    = L1etm_ ;

	if(isPeriodLow)       etmCut=20;
	else if(isPeriodHigh) etmCut=26;
	else etmCut=20;

	passL1etmCutUp_    = passL1etmCutDown_    = passL1etmCut_    = float(L1etmCorr_>etmCut);
	passL1etmCutABCUp_ = passL1etmCutABCDown_ = passL1etmCutABC_ = float(L1etmCorr_>20);

	// DoubleMu matching
	isMatched = false;
	isMatched = (*triggerPaths)["HLT_Mu17_Mu8_v16"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v17"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v18"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v19"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v20"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v21"]
	  || (*triggerPaths)["HLT_Mu17_Mu8_v22"];
	HLTxMu17Mu8 = isMatched ? 1.0 : 0.0 ;

	/*
	// DoubleMu matching
	isMatched = false;
	for(int i=26 ; i<33 ; i++)
	  isMatched = isMatched || (*triggerBits)[i]; // 
	HLTxMu17Mu8 = isMatched ? 1.0 : 0.0 ;
	*/
      }
      
      // Weights for both MC and embedded
      // Tau
      SFTau  = 1.0;
      
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
 
      HLTweight2Triggers = 1.;
      if(ptL1 < 27.) HLTweight2Triggers = HLTweightTau ;
      else if(ptL1 >= 27.) HLTweight2Triggers = 1. ;

      //Decay Modes Weights
      weightDecayMode_ = 1.0;

      //Old Decay Mode correction (prior to Mar14)
//       if( sample.Contains("Emb")  && !sample.Contains("TTJets-Embedded")){
// 	if(decayMode == 0) weightDecayMode_ = 0.88;
//       }
//       else{
// 	if(isTauLegMatched>0 && decayMode == 0)
// 	  weightDecayMode_ = 0.88; 
//       }

      //Weight to correct for mis-modeling of the decay mode distribution between MC and data
      if( sample.Contains("Emb")  && !sample.Contains("TTJets-Embedded"))
	{
	  if(TMath::Abs((*diTauLegsP4)[1].Eta())<=1.5)//Barrel
	    {
	      if(decayMode == 0) weightDecayMode_ = 0.87;//1-prong
	      if(decayMode == 1) weightDecayMode_ = 1.06;//1-prong + pi0s
	      if(decayMode == 4) weightDecayMode_ = 1.02;//3-prong
	    }
	  else if(TMath::Abs((*diTauLegsP4)[1].Eta())>1.5)//EndCaps
	    {
	      if(decayMode == 0) weightDecayMode_ = 0.96;//1-prong
	      if(decayMode == 1) weightDecayMode_ = 1.00;//1-prong + pi0s
	      if(decayMode == 4) weightDecayMode_ = 1.06;//3-prong
	    }
	}
      else
	{
	  if(isTauLegMatched>0)//check for MC that the Tau Leg is matched to a true tau
	    {
	      if(TMath::Abs((*diTauLegsP4)[1].Eta())<=1.5)//Barrel
		{
		  if(decayMode == 0) weightDecayMode_ = 0.87;//1-prong
		  if(decayMode == 1) weightDecayMode_ = 1.06;//1-prong + pi0s
		  if(decayMode == 4) weightDecayMode_ = 1.02;//3-prong
		}
	      else if(TMath::Abs((*diTauLegsP4)[1].Eta())>1.5)//EndCaps
		{
		  if(decayMode == 0) weightDecayMode_ = 0.96;//1-prong
		  if(decayMode == 1) weightDecayMode_ = 1.00;//1-prong + pi0s
		  if(decayMode == 4) weightDecayMode_ = 1.06;//3-prong
		}
	    }
	}
      
      // Muon
      // trigger
      //
      //choose iEta
      int iEta;
      if(etaL1 < -1.2)    iEta = 0;
      else if(etaL1 < -0.8) iEta = 1;
      else if(etaL1 < 0)    iEta = 2;
      else if(etaL1 < 0.8)  iEta = 3;
      else if(etaL1 < 1.2)  iEta = 4;
      else                  iEta = 5;
      //
      // compute efficiency

      // ABC with pT shift to evaluate ABC IsoMu15 efficiency
      HLTMuA        = turnOnMu[iEta][0]->Eval(ptL1+17-15);
      HLTMuB        = turnOnMu[iEta][1]->Eval(ptL1+17-15);
      HLTMuC        = turnOnMu[iEta][2]->Eval(ptL1+17-15);
      HLTMuABCShift = (HLTMuA*wA + HLTMuB*wB + HLTMuC*wC)/(wA+wB+wC) ;
      // Retrieve normal ABC
      HLTMuA   = turnOnMu[iEta][0]->Eval(ptL1);
      HLTMuB   = turnOnMu[iEta][1]->Eval(ptL1);
      HLTMuC   = turnOnMu[iEta][2]->Eval(ptL1);
      HLTMuABC = (HLTMuA*wA + HLTMuB*wB + HLTMuC*wC)/(wA+wB+wC) ;
      // D and ABCD(Moriond)
      HLTMuD     = turnOnMu[iEta][3]->Eval(ptL1);
      HLTMuABCD  = turnOnMu[iEta][5]->Eval(ptL1);
      // MC : HCP and Moriond
      HLTMuMCold      = turnOnMu[iEta][4]->Eval(ptL1); // for normal ABC
      HLTMuMCnew      = turnOnMu[iEta][6]->Eval(ptL1); // for ABCD and D
      HLTMuShiftMCold = turnOnMu[iEta][6]->Eval(ptL1+17.-15.); // for soft ABC
      // Soft leptons : IsoMu8(soft) and runD/newMC with shift
      HLTMuSoft    = turnOnMu[iEta][7]->Eval(ptL1);
      HLTMuSoftMC  = turnOnMu[iEta][8]->Eval(ptL1);
      HLTMuShift   = turnOnMu[iEta][3]->Eval(ptL1+17.-8.);
      HLTMuShiftMC = turnOnMu[iEta][6]->Eval(ptL1+17.-8.);

      // Compute weight D/MC
      HLTweightMuA    = HLTMuMCold!=0 ?  HLTMuA / HLTMuMCold : 0;
      HLTweightMuB    = HLTMuMCold!=0 ?  HLTMuB / HLTMuMCold : 0;
      HLTweightMuC    = HLTMuMCold!=0 ?  HLTMuC / HLTMuMCold : 0;
      HLTweightMuD    = HLTMuMCnew!=0 ?  HLTMuD / HLTMuMCnew : 0;
      HLTweightMuABC  = HLTMuMCold!=0 ?  HLTMuABC / HLTMuMCold : 0;
      HLTweightMuABCD = HLTMuMCnew!=0 ?  HLTMuABCD / HLTMuMCnew : 0;
      HLTweightMuSoft = HLTMuSoftMC!=0 ? HLTMuSoft / HLTMuSoftMC : 0;
      HLTweightMuShift= HLTMuShiftMC!=0? HLTMuShift / HLTMuShiftMC : 0;
      HLTweightMuABCShift= HLTMuShiftMCold!=0? HLTMuABCShift / HLTMuShiftMCold : 0;

      HLTMu = HLTMuABCD; 
      HLTweightMu = HLTweightMuABCD;

      // id+iso scale factors
      float ptMaxMu = 14.0;
      Double_t ptMuSF  = std::max(ptL1,ptMaxMu);

      // choose iEta
      if(abs(etaL1) < 0.8)      iEta = 0; // [0;0.8[
      else if(abs(etaL1) < 1.2) iEta = 1; // [0.8;1.2[
      else                      iEta = 2; // [1.2;2.1[

      // compute ID and Iso efficiency
      EffMuID_ABCD     = turnOnMuIdIso[iEta][0][0]->Eval(ptMuSF);
      EffMuIso_ABCD    = turnOnMuIdIso[iEta][0][1]->Eval(ptMuSF);
      EffMuID_ABCD_MC  = turnOnMuIdIso[iEta][1][0]->Eval(ptMuSF);
      EffMuIso_ABCD_MC = turnOnMuIdIso[iEta][1][1]->Eval(ptMuSF);

      EffMuID_ABC     = turnOnMuIdIso[iEta][2][0]->Eval(ptMuSF);
      EffMuIso_ABC    = turnOnMuIdIso[iEta][2][1]->Eval(ptMuSF);
      EffMuID_ABC_MC  = turnOnMuIdIso[iEta][3][0]->Eval(ptMuSF);
      EffMuIso_ABC_MC = turnOnMuIdIso[iEta][3][1]->Eval(ptMuSF);

      EffMuID_D     = turnOnMuIdIso[iEta][4][0]->Eval(ptMuSF);
      EffMuIso_D    = turnOnMuIdIso[iEta][4][1]->Eval(ptMuSF);
      EffMuID_D_MC  = turnOnMuIdIso[iEta][5][0]->Eval(ptMuSF);
      EffMuIso_D_MC = turnOnMuIdIso[iEta][5][1]->Eval(ptMuSF);
      
      // compute D/MC weights
      SFMuID_ABCD  = EffMuID_ABCD_MC !=0 ? EffMuID_ABCD  / EffMuID_ABCD_MC  : 0;
      SFMuIso_ABCD = EffMuIso_ABCD_MC!=0 ? EffMuIso_ABCD / EffMuIso_ABCD_MC : 0;
      SFMu_ABCD    = SFMuID_ABCD*SFMuIso_ABCD;

      SFMuID_ABC  = EffMuID_ABC_MC !=0 ? EffMuID_ABC  / EffMuID_ABC_MC  : 0;
      SFMuIso_ABC = EffMuIso_ABC_MC!=0 ? EffMuIso_ABC / EffMuIso_ABC_MC : 0;
      SFMu_ABC    = SFMuID_ABC*SFMuIso_ABC;

      // Run 2012D SFs
      SFMuID_D  = EffMuID_D_MC !=0 ? EffMuID_D  / EffMuID_D_MC  : 0;
      SFMuIso_D = EffMuIso_D_MC!=0 ? EffMuIso_D / EffMuIso_D_MC : 0;
      //
      // SF 2012D Low Pt from Andrew
      if (ptL1 > 8.0  && ptL1 <= 15.0 && etaL1 < 0.8)                    { SFMuID_D = 0.9790; SFMuIso_D = 0.9963; }
      if (ptL1 > 8.0  && ptL1 <= 15.0 && etaL1 >= 0.8 && etaL1 < 1.2)    { SFMuID_D = 0.9809; SFMuIso_D = 0.9769; }
      if (ptL1 > 8.0  && ptL1 <= 15.0 && etaL1 >= 1.2)                   { SFMuID_D = 0.9967; SFMuIso_D = 0.9870; }
      if (ptL1 > 15.0 && ptL1 <= 20.0 && etaL1 < 0.8)                    { SFMuID_D = 0.9746; SFMuIso_D = 0.9842; }
      if (ptL1 > 15.0 && ptL1 <= 20.0 && etaL1 >= 0.8 && etaL1 < 1.2)    { SFMuID_D = 0.9796; SFMuIso_D = 0.9664; }
      if (ptL1 > 15.0 && ptL1 <= 20.0 && etaL1 >= 1.2)                   { SFMuID_D = 0.9864; SFMuIso_D = 0.9795; }      
      //
      SFMu_D    = SFMuID_D*SFMuIso_D;
      
      SFMu    = SFMu_ABCD;
      SFMuID  = SFMuID_ABCD;
      SFMuIso = SFMuIso_ABCD;
      
      if(DEBUG) cout << "HLTxQCD=" << HLTxQCD << endl;

    }// end MC/embedded case
   
    if(DEBUG) cout << "End of corrections trigger+SF" << endl;
    
    if(DEBUG) cout << "CDF weight" << endl;    
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
    int nVetoLeptonOld = 0;
    int nVetoLeptonNew = 0;
    for(size_t imu = 0; imu < vetoMuonsP4->size(); imu++){
      if(deltaR((*diTauLegsP4)[0], (*vetoMuonsP4)[imu]) > 0.3 && 
// 	 deltaR((*diTauLegsP4)[1], (*vetoMuonsP4)[imu]) > 0.3 &&
	 (*vetoMuonsP4)[imu].Pt()>10){
	nVetoLeptonOld++;
	nVetoLeptonNew++;
      }
    }

    for(size_t imu = 0; imu < vetoElectronsP4->size(); imu++){ 
      // if(deltaR((*diTauLegsP4)[0], (*vetoElectronsP4)[imu]) > 0.3 &&  
//          deltaR((*diTauLegsP4)[1], (*vetoElectronsP4)[imu]) > 0.3 )
      if((*vetoElectronsP4)[imu].Pt()>10&&
	 ((*vetoElectronsID)[imu]==0 || (*vetoElectronsID)[imu]==2))
        nVetoLeptonOld++;
      if((*vetoElectronsP4)[imu].Pt()>10&&
	 ((*vetoElectronsID)[imu]==1 || (*vetoElectronsID)[imu]==2))
        nVetoLeptonNew++;  
    } 
    
    //nVetoLepton += vetoElectronsP4->size();
    if(DEBUG) cout << "End 3rd lepton veto" << endl;

    // Reweight for Zmm
    ZmmWeight = 1;
    if( sample_.find("DYJets")!=string::npos ){
      //New Zmm SF 06May2014
      if(decayMode==0 && leptFakeTau){
	ZmmWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  0.760006 : 
	  0.737036;
      }
      else if (decayMode==1 && leptFakeTau){
	ZmmWeight = TMath::Abs((*diTauLegsP4)[1].Eta())<1.479 ?
	  1.20777 : 
	  0.752396;
      }
    }

    isPFMuon_        = isPFMuon;
    isTightMuon_     = isTightMuon;
    muFlag_          = muFlag;
//     vetoEvent_       = (nVetoLepton > 0) ? 1 : 0; //vetoEvent; 
    vetoEventOld_    = (nVetoLeptonOld > 0) ? 1 : 0; //vetoEvent; 
    vetoEventNew_    = (nVetoLeptonNew > 0) ? 1 : 0; //vetoEvent; //NewEleID
    parton_          = parton;
    genPartMult_     = genPartMult;
    leadGenPartPdg_  = leadGenPartPdg;
    hepNUP_          = hepNUP;
    leadGenPartPt_   = leadGenPartPt;
    event_           = event;
    run_             = run;
    lumi_            = lumi;
    index_           = index;   

    // pairIndex
    //counter=0;
    //passQualityCuts=(muFlag!=1 && vetoEventOld_==0 && isPFMuon && isTightMuon && ptL1>20 && ptL2>20 && tightestAntiMu2WP>2  && hpsDB3H<1.5 && combRelIsoLeg1DBetav2<0.1 && HLTmatch);
    passQualityCuts=(isPFMuon && isTightMuon && ptL1>20 && ptL2>20 && tightestAntiMuWP>2  && hpsDB3H<1.5 && combRelIsoLeg1DBetav2<0.1 && HLTmatch);
    
    // Arrived in a new event
    if( !(run==lastRun && lumi==lastLumi && event==lastEvent) ){
      
      // change reference
      lastEvent = event;
      lastLumi  = lumi;
      lastRun   = run;
      
      // set back counters to 0
      counter=0;
      
      if( passQualityCuts ){
	pairIndex = counter;
	counter++;
      }
      else pairIndex = -1;
    }
    
    // This entry is in the same event
    else{
      if( passQualityCuts ){
	pairIndex = counter;
	counter++;
      }
      else
	pairIndex = -1;
    }

    //displaying Abdollah's info
    //run=1   lumi=353   event=187672   l1Pt=55.7683   l1eta=0.223352   l1Phi=1.14464   l2Pt=46.7634   l2Eta=0.108624   l2Phi=-1.29245   MVAMet=21.6821   numJet30=1   numBJet20=1   PU_Weight=1   npu=15   erightLepton_id_iso=0.973673   lepton_trg_Weight=0.985811   tau_Trg_Weight=0.985262
    cout<<"run="<<run
	<<"\t event="<<event
	<<"\t l1Pt="<<ptL1
	<<"\t l1eta="<<etaL1
	<<"\t l1Phi="<<phiL1
	<<"\t l2Pt="<<ptL2
	<<"\t l2eta="<<etaL2
	<<"\t l2Phi="<<phiL2
 	<<"\t MVAMet="<<MEtMVA
 	<<"\t numJet30="<<nJets30
 	<<"\t numBJet20="<<nJets20BTagged
 	<<"\t PU_Weight="<<puWeight//OK
 	<<"\t npu="<<nPUVertices_//OK
 	<<"\t weightLepton_id_iso="<<SFMu//OK
 	<<"\t lepton_trg_Weight="<<HLTweightMu
 	<<"\t tau_Trg_Weight="<<HLTweightTau<<endl;


    outTreePtOrd->Fill();
  }

  if(SampleT.Contains("SUSY") && SampleT.Contains("GGH"))
    {
      delete h_mhmax ;
      delete h_mhmax_HqTUp ;
      delete h_mhmax_HqTDown ;
      delete h_mhmax_HIGLUUp ;
      delete h_mhmax_HIGLUDown ;
      delete h_mhmax_tanBetaUp ;
      delete h_mhmax_tanBetaDown ;

      delete h_mhmodplus ;
      delete h_mhmodplus_HqTUp ;
      delete h_mhmodplus_HqTDown ;
      delete h_mhmodplus_HIGLUUp ;
      delete h_mhmodplus_HIGLUDown ;
      delete h_mhmodplus_tanBetaUp ;
      delete h_mhmodplus_tanBetaDown ;

      delete h_mhmodminus ;
      delete h_mhmodminus_HqTUp ;
      delete h_mhmodminus_HqTDown ;
      delete h_mhmodminus_HIGLUUp ;
      delete h_mhmodminus_HIGLUDown ;
      delete h_mhmodminus_tanBetaUp ;
      delete h_mhmodminus_tanBetaDown ;

      delete h_lowmH ;
      delete h_lowmH_HqTUp ;
      delete h_lowmH_HqTDown ;
      delete h_lowmH_HIGLUUp ;
      delete h_lowmH_HIGLUDown ;
      delete h_lowmH_tanBetaUp ;
      delete h_lowmH_tanBetaDown ;
    }
  
  delete jets; /*delete jets_v2;*/ delete diTauLegsP4; delete diTauVisP4; delete diTauSVfitP4; delete diTauCAP4; delete genDiTauLegsP4; delete genTausP4;
  delete genTausCharge; delete genTausDecayLeptonically;
  delete HLTfiltersMu; delete HLTfiltersTau, delete HLTfiltersTauJet ; delete triggerPaths;
//   delete tauXTriggers; delete triggerBits;
  delete METP4; delete jetsBtagHE; delete jetsBtagHP; delete jetsBtagCSV; delete jetsChNfraction; delete genVP4; delete genMETP4;
  delete gammadEta; delete gammadPhi; delete gammaPt; delete HqT;
  delete jetPUMVA; delete jetPUWP;
  delete metSgnMatrix;

  for(std::map<string , TMVA::Reader*>::iterator read = readers.begin() ; read!=readers.end(); read++)
    delete (*read).second;
  
  return;
  
}


// void doAllSamplesMu(string inputDir_ = "/data_CMS/cms/anayak/H2TauTauHCP/MuTauStream/")
// {


//   std::vector<std::string> samples;
//   std::vector<float> crossSec;

//   ////////////// samples & x-sections & skim1 & skim2 /////////////
  

  //samples.push_back("Run2011-MuTau-All_run");             crossSec.push_back( 0  );                          
  //samples.push_back("Run2011-MuTau-LooseIso-All_run");    crossSec.push_back( 0  );                          
  //samples.push_back("VBFH125-MuTau-8TeV-powheg-DR53X-PUS10_run"); crossSec.push_back(1.578 * 0.0632 * 1.0 * 0.0780138080726);
  //samples.push_back("DYJets-MuTau-50-madgraph-PUS10_run"); crossSec.push_back(1.578 * 0.0632 * 1.0 * 0.0780138080726);

//   makeTrees_MuTau("",             samples[0], crossSec[0], 
// 		  "/data_CMS/cms/htautau/PostMoriond/TREES/MuTau/HiggsSM/VBF125/", 
// 		  "/data_CMS/cms/htautau/PostMoriond/NTUPLES/MuTau/temp/");
 
//   return;

//   for( unsigned int k = 0; k < samples.size(); k++) {
    
//     makeTrees_MuTau("",        samples[k], crossSec[k], inputDir_);

//     if( samples[k].find("Run2012-MuTau-All")!=string::npos )
//       continue;
//     makeTrees_MuTau("TauUp",   samples[k], crossSec[k], inputDir_);
//     makeTrees_MuTau("TauDown", samples[k], crossSec[k], inputDir_);
//     if( samples[k].find("Embedded")!=string::npos)
//       continue;
//     makeTrees_MuTau("JetUp",   samples[k], crossSec[k], inputDir_);
//     makeTrees_MuTau("JetDown", samples[k], crossSec[k], inputDir_);
//   }
  
//   return;

// }


// int main(int argc, const char* argv[])
// {
  
//   std::cout << "doAllSamplesMu()" << std::endl;
//   gROOT->SetBatch(true);
 
//   gSystem->Load("libFWCoreFWLite");
//   AutoLibraryLoader::enable();

//   string inputDir = "./";
//   string dirOut   = "./";

//   if ( argc==1 ) {
//     doAllSamplesMu(inputDir);
//   }
//   else if( argc==3 ) {
    
//     makeTrees_MuTau("",           argv[1], atof(argv[2]), inputDir, dirOut, -1);
    
//     if( string(argv[1]).find("Run2012-MuTau-All")!=string::npos )
//       return 0;
//     makeTrees_MuTau("TauUp",      argv[1], atof(argv[2]), inputDir, dirOut, -1);
//     makeTrees_MuTau("TauDown",    argv[1], atof(argv[2]), inputDir, dirOut, -1);
    
//     if( string(argv[1]).find("Embedded")!=string::npos)
//       return 0;
//     makeTrees_MuTau("JetUp",      argv[1], atof(argv[2]), inputDir, dirOut, -1);
//     makeTrees_MuTau("JetDown",    argv[1], atof(argv[2]), inputDir, dirOut, -1);
//   }
//   else if( argc==7 ){
//     makeTrees_MuTau(argv[1], argv[2], atof(argv[3]), argv[4], argv[5], (int)atof(argv[6]));
//   }
//   else if( argc==8 ){
//     makeTrees_MuTau(argv[1], argv[2], atof(argv[3]), argv[4], argv[5], (int)atof(argv[6]), (int)atof(argv[7]));
//   }
//   else {
//     cout << "Wrong number of arguments. Specify either 0, 2, or 6 arguments" << endl;
//     return 1;
//   }

//   cout << "finished running treeSkimmer" << endl;
//   return 0;
//   //makeTrees_MuTau(string analysis_ = "", string sample_ = "", float xsec_ = 0., string inputDir_ = "./", string dirOut_ = "./", int iJson_=-1, bool doLepVeto=false){

// }


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  
  std::cout << "treeSkimmerMuTau_Summer14" << std::endl;
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
    throw cms::Exception("TreeSkimmerMuTauAnalyzer") 
      << "No ParameterSet 'process' found in configuration file = " << argv[1] << " !!\n";

  edm::ParameterSet cfg = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");

  edm::ParameterSet cfgTreeSkimmerMuTauAnalyzer = cfg.getParameter<edm::ParameterSet>("treeSkimmerMuTau");

  std::string sample = cfgTreeSkimmerMuTauAnalyzer.getParameter<std::string>("sample");
  std::string analysis = cfgTreeSkimmerMuTauAnalyzer.getParameter<std::string>("analysis");
  double xSection = cfgTreeSkimmerMuTauAnalyzer.getParameter<double>("xSection");
  double skimEff = cfgTreeSkimmerMuTauAnalyzer.getParameter<double>("skimEff");
  int iJson = cfgTreeSkimmerMuTauAnalyzer.getParameter<int>("iJson");
  int iDiv = cfgTreeSkimmerMuTauAnalyzer.getParameter<int>("iDiv");
  int nDiv = cfgTreeSkimmerMuTauAnalyzer.getParameter<int>("nDiv");

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
      throw cms::Exception("TreeSkimmerMuTauAnalyzer") 
	<< "Failed to open inputFile = " << (*inputFileName) << " !!\n";

    TString histoName("allEventsFilter/totalEvents");
    TH1D* histo =(TH1D*)inputFile->Get(histoName);
//     cout<<"histo "<<histo->GetEntries();
    if(histo)nEventsRead += histo->GetBinContent(1) ;
    else throw cms::Exception("TreeSkimmerMuTauAnalyzer") 
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
  TString treeName(("muTauStreamAnalyzer"+anlyzerName+"/tree").c_str());
  TChain* currentTree = new TChain (treeName);
  bool maxEvents_processed = false;
  for ( vstring::const_iterator inputFileName = inputFiles.files().begin();
	inputFileName != inputFiles.files().end() && !maxEvents_processed; ++inputFileName ) {
    currentTree->Add(inputFileName->data());
  }

  cout<<"iDiv = "<<iDiv<<endl;
  cout<<"nDiv = "<<nDiv<<endl;


  fillTrees_MuTauStream(currentTree,outTreePtOrd,nEventsRead,analysis,sample,xSection,skimEff,iJson,true,iDiv,nDiv);
//   fillTrees_MuTauStream(currentTree,outTreePtOrd,nEventsRead,analysis,sample,xSection,skimEff,iJson,antiElecMVAcuts,iDiv,nDiv);

  return 0;
}
