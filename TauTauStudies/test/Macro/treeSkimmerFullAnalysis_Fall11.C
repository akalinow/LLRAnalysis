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
#include "RecoilCorrector.hh"

//#include "AntiElectronIDMVA.h"

#include "Lumi3DReWeightingForLorenzo.h"

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

float pileupWeight( int intimepileup_ ){

  
  // Fall11: intime
  //Twiki
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
  // Fall11: intime
  Chrsitian
  std::vector< float > Fall11Lumi ;
  Double_t Fall11Lumi_f[50] = {
    0.00905444, 
    0.019661,
    0.0321446,
    0.0435141,
    0.0527906,
    0.0590329,
    0.061598, 
    0.061749, 
    0.0592039,
    0.0569448,
    0.0535766,
    0.0494074,
    0.0458052,
    0.0422751,
    0.0389909,
    0.0356777,
    0.0330786,
    0.0304045,
    0.0275734,
    0.0249792,
    0.0226811,
    0.020367, 
    0.0181379,
    0.0161678,
    0.0141187,
    0.0120066,
    0.0104015,
    0.00893944,
    0.00755337,
    0.00628831,
    0.00529026,
    0.00435321,
    0.00364318,
    0.00287814, 
    0.00224511, 
    0.00174809, 
    0.00143807, 
    0.00111205, 
    0.000795039,
    0.000643032,
    0.000463023,
    0.000363018,
    0.000269013,
    0.00020101, 
    0.000135007,
    0.000103005,
    6.80033e-05,
    5.40026e-05,
    4.90024e-05,
    2.50012e-05
  };
  */

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
  
  /*
  // sigma 73.5 pb
  std::vector< float > Data2011LumiExt;
  Double_t Data2011LumiExt_f[50] = {
    0.00268287, 
    0.0111378, 
    0.0253649, 
    0.0426735, 
    0.0595703, 
    0.0727177, 
    0.0810381, 
    0.0842113, 
    0.0833151, 
    0.0795788, 
    0.0738552, 
    0.0672576, 
    0.0599305, 
    0.0522761, 
    0.0442358, 
    0.0370232, 
    0.030315, 
    0.0243174, 
    0.0189967, 
    0.0145095, 
    0.0107956, 
    0.00779958,
    0.00555043,
    0.00381755,
    0.00257767,
    0.00169383,
    0.00108528,
    0.000661581, 
    0.000409454, 
    0.000254036, 
    0.000151171, 
    8.96485e-05, 
    5.26011e-05, 
    2.82925e-05, 
    1.61978e-05, 
    9.68528e-06, 
    1.01147e-05,
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






void makeTrees_ElecTauStream(string analysis = "", int index = -1 /*, AntiElectronIDMVA* antiE=0*/){

  /*
    if(!antiE){
    cout << "Anti electron mva not initialized!" << endl;
    return;
    }
  */

  gSystem->Load("Lumi3DReWeightingForLorenzo_cc.so");
  edm::Lumi3DReWeightingForLorenzo* Lumi3DReWeighting = 
    new edm::Lumi3DReWeightingForLorenzo("pileUp/pileUpFall11.root",
					 "pileUp/Run2011PileUpTruth.root",
					 "MC_distr",
					 "pileup");
  //Lumi3DReWeighting->weight3D_init(73.5/68.);
  Lumi3DReWeighting->weight3D_init(1.00);
  cout << "Lumi3D rewieghting initialized" << endl;
  
  cout << "Creating random engine..." << endl;
  TRandom3* ran = new TRandom3();
  ran->SetSeed(12345);  

  cout << "Now skimming analysys " << analysis << endl;

  gSystem->Load("RecoilCorrector_hh.so");
  RecoilCorrector* recoilCorr = 0;

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  TFile corrections("llrCorrections.root");

  TF1 *ratioElecIDBL      = (TF1*)corrections.Get("ratioElecIDBL");
  TF1 *ratioElecIsoBL     = (TF1*)corrections.Get("ratioElecIsoBL");
  TF1 *ratioElecIDEC      = (TF1*)corrections.Get("ratioElecIDEC");
  TF1 *ratioElecIsoEC     = (TF1*)corrections.Get("ratioElecIsoEC");

  TF1 *ratioElecIDIsoBL   = (TF1*)corrections.Get("ratioElecIDIsoBL");
  TF1 *ratioElecIDIsoEC   = (TF1*)corrections.Get("ratioElecIDIsoEC");

  TF1 *ratioTauElecTauAll = (TF1*)corrections.Get("ratioTauElecTauAll");
  TF1 *ratioTauElecTauAllBL = (TF1*)corrections.Get("ratioTauElecTauAllBL");
  TF1 *ratioTauElecTauAllEC = (TF1*)corrections.Get("ratioTauElecTauAllEC");
  TF1 *ratioElecAllBL     = (TF1*)corrections.Get("ratioElecAllBL");
  TF1 *ratioElecAllEC     = (TF1*)corrections.Get("ratioElecAllEC");
  TF1 *turnOnTauElecTauAll= (TF1*)corrections.Get("turnOnTauElecTauAll");
  TF1 *turnOnTauElecTauAllBL = (TF1*)corrections.Get("turnOnTauElecTauAllBL");
  TF1 *turnOnTauElecTauAllEC = (TF1*)corrections.Get("turnOnTauElecTauAllEC");
  TF1 *turnOnElecAllBL    = (TF1*)corrections.Get("turnOnElecAllBL");
  TF1 *turnOnElecAllEC    = (TF1*)corrections.Get("turnOnElecAllEC");

 

  std::vector<std::string> samples;
  std::vector<float> crossSec;
  std::vector<int>   readEvents;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011-ElecTau-All_run");             crossSec.push_back( 0  );      
  samples.push_back("Run2011-ElecTau-LooseIso-All_run");    crossSec.push_back( 0  );                          
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
//samples.push_back("VH105-ElecTau-pythia-PUS6_run");  crossSec.push_back( 8.25e-02*(1.018+ 0.5449+0.1433  ) * 1.0 * 0.); 
  samples.push_back("VH110-ElecTau-pythia-PUS6_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 0.084386 * 0.76245288);
  samples.push_back("VH115-ElecTau-pythia-PUS6_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 1.0 * 0.1668136);
  samples.push_back("VH120-ElecTau-pythia-PUS6_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0 * 0.1732820);   
  samples.push_back("VH125-ElecTau-pythia-PUS6_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 1.0 * 0.1787773);
  samples.push_back("VH130-ElecTau-pythia-PUS6_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 0.10051 * 0.7730161);
  samples.push_back("VH135-ElecTau-pythia-PUS6_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 0.10133 * 0.7768092);
  samples.push_back("VH140-ElecTau-pythia-PUS6_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 1.0 * 0.1919186);  
  samples.push_back("VH145-ElecTau-pythia-PUS6_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 0.10708 * 0.774027);  
  samples.push_back("VH160-ElecTau-pythia-PUS6_run");  crossSec.push_back( 5.32e-04*(0.2291+0.1334+0.03942 ) * 1.0 * 0.2120560);  
  

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
  int tightestCutBasedWP_, tightestCiCWP_,tightestMVAWP_,tightestAntiEMVAWP_;
  float sihih_, dEta_, dPhi_, HoE_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, puWeight3D, embeddingWeight_,HqTWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmatch,HLTweightElec,HLTweightTau, SFElec, SFTau, HLTElec, HLTTau, SFEtoTau;
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
  outTreePtOrd->Branch("tightestAntiEMVAWP", &tightestAntiEMVAWP_,"tightestAntiEMVAWP/I");
  outTreePtOrd->Branch("sihih",              &sihih_,"sihih/F");
  outTreePtOrd->Branch("dEta",               &dEta_,"dEta/F");
  outTreePtOrd->Branch("dPhi",               &dPhi_,"dPhi/F");
  outTreePtOrd->Branch("HoE",                &HoE_,"HoE/F");


  outTreePtOrd->Branch("tightestHPSDBWP",    &tightestHPSDBWP_,"tightestHPSDBWP/I");
  outTreePtOrd->Branch("decayMode",          &decayMode_,"decayMode/I");

  outTreePtOrd->Branch("numPV",              &numPV_,"numPV/F");
  outTreePtOrd->Branch("sampleWeight",       &sampleWeight,"sampleWeight/F"); 
  outTreePtOrd->Branch("puWeight",           &puWeight,"puWeight/F");
  outTreePtOrd->Branch("puWeight3D",         &puWeight3D,"puWeight3D/F");
  outTreePtOrd->Branch("embeddingWeight",    &embeddingWeight_,"embeddingWeight/F");
  outTreePtOrd->Branch("HqTWeight",          &HqTWeight,"HqTWeight/F");
  outTreePtOrd->Branch("numOfLooseIsoDiTaus",&numOfLooseIsoDiTaus_,"numOfLooseIsoDiTaus/I");

  outTreePtOrd->Branch("HLTx",         &HLTx,"HLTx/F");
  outTreePtOrd->Branch("HLTmatch",     &HLTmatch,"HLTmatch/F");
  outTreePtOrd->Branch("HLTweightElec",&HLTweightElec,"HLTweightElec/F");
  outTreePtOrd->Branch("HLTweightTau", &HLTweightTau,"HLTweightTau/F");
  outTreePtOrd->Branch("HLTElec",      &HLTElec,"HLTElec/F");
  outTreePtOrd->Branch("HLTTau",       &HLTTau,"HLTTau/F");
  outTreePtOrd->Branch("SFTau",        &SFTau,"SFTau/F");
  outTreePtOrd->Branch("SFElec",       &SFElec,"SFElec/F");
  outTreePtOrd->Branch("SFEtoTau",     &SFEtoTau,"SFEtoTau/F");

  outTreePtOrd->Branch("isTauLegMatched", &isTauLegMatched_,"isTauLegMatched/I");
  outTreePtOrd->Branch("elecFlag",        &elecFlag_,"elecFlag/I"); 
  outTreePtOrd->Branch("genDecay",        &genDecay_,"genDecay/I");
  outTreePtOrd->Branch("leptFakeTau",     &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/ElecTauStreamFall11_06Dec2011//treeElecTauStream_"+samples[index]+".root" : "../treeElecTauStream.root";


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
  currentTree->SetBranchStatus("chIsoEELeg1v2"         ,1);
  currentTree->SetBranchStatus("nhIsoEELeg1v2"         ,1);
  currentTree->SetBranchStatus("phIsoEELeg1v2"         ,1);
  currentTree->SetBranchStatus("nhIsoEEPULeg1v2"       ,1);
  currentTree->SetBranchStatus("phIsoEEPULeg1v2"       ,1);
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
  currentTree->SetBranchStatus("mvaAntiE"              ,1);
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
  currentTree->SetBranchStatus("nPUVerticesP1"         ,1);
  currentTree->SetBranchStatus("nPUVerticesM1"         ,1);
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
  int isTauLegMatched,isElecLegMatched,elecFlag,genDecay, nPUVertices, nPUVerticesM1,nPUVerticesP1;
  float leadPFChargedHadrCandP;
  float leadPFChargedHadrMva;
  float emFraction, hasGsf, leadPFChargedHadrHcalEnergy, leadPFChargedHadrEcalEnergy;
  int signalPFChargedHadrCands, signalPFGammaCands;
  float mvaAntiE;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float pZetaSig;
  float chIsoLeg2, phIsoLeg2;
  float chIsoLeg1,nhIsoLeg1,phIsoLeg1; 
  float chIsoPULeg1,nhIsoPULeg1,phIsoPULeg1;
  float chIsoEELeg1,nhIsoEELeg1,phIsoEELeg1; 
  float nhIsoEEPULeg1,phIsoEEPULeg1; 
  ULong64_t event,run,lumi;

  currentTree->SetBranchAddress("chIsoLeg2",            &chIsoLeg2);
  currentTree->SetBranchAddress("phIsoLeg2",            &phIsoLeg2);
  currentTree->SetBranchAddress("chIsoLeg1v2",          &chIsoLeg1);
  currentTree->SetBranchAddress("nhIsoLeg1v2",          &nhIsoLeg1);    
  currentTree->SetBranchAddress("phIsoLeg1v2",          &phIsoLeg1);
  currentTree->SetBranchAddress("chIsoPULeg1v2",        &chIsoPULeg1);  
  currentTree->SetBranchAddress("nhIsoPULeg1v2",        &nhIsoPULeg1);
  currentTree->SetBranchAddress("phIsoPULeg1v2",        &phIsoPULeg1);

  currentTree->SetBranchAddress("chIsoEELeg1v2",        &chIsoEELeg1);
  currentTree->SetBranchAddress("nhIsoEELeg1v2",        &nhIsoEELeg1);    
  currentTree->SetBranchAddress("phIsoEELeg1v2",        &phIsoEELeg1);
  currentTree->SetBranchAddress("nhIsoEEPULeg1v2",      &nhIsoEEPULeg1);
  currentTree->SetBranchAddress("phIsoEEPULeg1v2",      &phIsoEEPULeg1);

  currentTree->SetBranchAddress("rhoFastJet",           &rhoFastJet);
  currentTree->SetBranchAddress("rhoNeutralFastJet",    &rhoNeutralFastJet);
  currentTree->SetBranchAddress("tightestHPSDBWP",      &tightestHPSDBWP);
  currentTree->SetBranchAddress("tightestCutBasedWP",   &tightestCutBasedWP);
  currentTree->SetBranchAddress("tightestCiCWP",        &tightestCiCWP);
  currentTree->SetBranchAddress("tightestMVAWP",        &tightestMVAWP);
  currentTree->SetBranchAddress("sihih",                &sihih);
  currentTree->SetBranchAddress("dEta",                 &dEta);
  currentTree->SetBranchAddress("dPhi",                 &dPhi);
  currentTree->SetBranchAddress("HoE",                  &HoE);
  currentTree->SetBranchAddress("mvaAntiE",             &mvaAntiE);

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

  unsigned int indexHelp = index >=0 ? index : 0;

  if(samples[indexHelp].find("WJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_wjets_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(samples[indexHelp].find("DYJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zjets_ltau_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(samples[indexHelp].find("H1")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_higgs_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
  

  TFile* HqT      = 0;
  int mH          = 120;
  TH1F* histo     = 0;
  if(samples[indexHelp].find("GGFH")!=string::npos){
    if(samples[indexHelp].find("GGFH100")!=string::npos) mH = 100;
    if(samples[indexHelp].find("GGFH105")!=string::npos) mH = 105;
    if(samples[indexHelp].find("GGFH110")!=string::npos) mH = 110;
    if(samples[indexHelp].find("GGFH115")!=string::npos) mH = 115;
    if(samples[indexHelp].find("GGFH120")!=string::npos) mH = 120;
    if(samples[indexHelp].find("GGFH125")!=string::npos) mH = 125;
    if(samples[indexHelp].find("GGFH130")!=string::npos) mH = 130;
    if(samples[indexHelp].find("GGFH135")!=string::npos) mH = 135;
    if(samples[indexHelp].find("GGFH140")!=string::npos) mH = 140;
    if(samples[indexHelp].find("GGFH145")!=string::npos) mH = 145;
    if(samples[indexHelp].find("GGFH160")!=string::npos) mH = 160;
    cout << "Reweighting powheg with HqT mH=" << mH << endl;
    HqT = new TFile(Form("../../../Utilities/data/HqTFeHiPro/weight_ptH_%d.root", mH));
    if(!HqT) cout << "Cannot find HqT file..." << endl;
    else{
      histo = (TH1F*)(HqT->Get(Form("powheg_weight/weight_hqt_fehipro_fit_%d",mH)));
    }
  }

  
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
      
    ////////////////////////////////////////////////////////////////////

    TLorentzVector corrMET_tmp;
    LV corrMET(1,0,0,1);
    double corrPt = (*METP4)[0].Et(); double corrPhi = (*METP4)[0].Phi();
    double u1 = 0.; double u2 = 0.;
    double err1 = 0; double err2 = 0;

    if(genVP4->size() && recoilCorr!=0){
      if(samples[indexHelp].find("WJets")  !=string::npos)      
	recoilCorr->CorrectType1(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi(), u1, u2 , err1,err2, TMath::Min(nJets30,2) );
      else if(samples[indexHelp].find("DYJets")!=string::npos || samples[indexHelp].find("H1")!=string::npos)  
	recoilCorr->CorrectType1(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
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

    combRelIsoLeg1         = TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ?
      (chIsoLeg1+nhIsoLeg1+phIsoLeg1)/(*diTauLegsP4)[0].Pt() :
      (chIsoEELeg1+nhIsoEELeg1+phIsoEELeg1)/(*diTauLegsP4)[0].Pt() ;
    float PUtoPVratio      = (chIsoLeg1+chIsoPULeg1)>0 ? chIsoPULeg1/(chIsoLeg1+chIsoPULeg1) : 0.0;
    float scaled_nhIsoLeg1 = std::max( nhIsoLeg1*(1-PUtoPVratio), float(0.0));
    float scaled_phIsoLeg1 = std::max( phIsoLeg1*(1-PUtoPVratio), float(0.0));
    combRelIsoLeg1Beta     =  (chIsoLeg1+scaled_nhIsoLeg1+scaled_phIsoLeg1)/(*diTauLegsP4)[0].Pt();
    combRelIsoLeg1DBeta    = TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ?
      (chIsoLeg1   + std::max( nhIsoLeg1+phIsoLeg1-0.5*0.5*(nhIsoPULeg1+phIsoPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt() :
      (chIsoEELeg1 + std::max( nhIsoEELeg1+phIsoEELeg1-0.5*0.5*(nhIsoEEPULeg1+phIsoEEPULeg1),double(0.0)))/(*diTauLegsP4)[0].Pt() ;
    float EffArea          = TMath::Pi()*0.4*0.4;
    combRelIsoLeg1Rho      = std::max(((chIsoLeg1+nhIsoLeg1+phIsoLeg1) - rhoNeutralFastJet*EffArea),float(0.0))/(*diTauLegsP4)[0].Pt();
    combIsoLeg2            =  ( chIsoLeg2 + std::max( phIsoLeg2 - rhoFastJet*TMath::Pi()*0.5*0.5, 0.0) ) ;    

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
    double mva = antiE->MVAValue(etaL2,ptL2,
    signalPFChargedHadrCands,signalPFGammaCands,
    leadPFChargedHadrMva,
    leadPFChargedHadrHcalEnergy/leadPFChargedHadrCandP, leadPFChargedHadrEcalEnergy/leadPFChargedHadrCandP,
    hasGsf,visibleTauMass,emFraction,
    etaMom2,phiMom2,gammaFrac);
    */    
    antiEMVA   = mvaAntiE;//mva

    tightestAntiEMVAWP_ = 
      ((abs(etaL2)<1.5 && signalPFGammaCands==0 && antiEMVA>0.054) ||
       (abs(etaL2)<1.5 && signalPFGammaCands>0  && hasGsf>0.5 && antiEMVA>0.060) ||
       (abs(etaL2)<1.5 && signalPFGammaCands>0  && hasGsf<0.5 && antiEMVA>0.054) ||
       (abs(etaL2)>1.5 && signalPFGammaCands==0 && antiEMVA>0.060) ||
       (abs(etaL2)>1.5 && signalPFGammaCands>0  && hasGsf>0.5 && antiEMVA>0.053) ||
       (abs(etaL2)>1.5 && signalPFGammaCands>0  && hasGsf<0.5 && antiEMVA>0.049)) ? 1.0 : 0.0;

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
    puWeight            = (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : pileupWeight(nPUVertices);
    puWeight3D          = (std::string(sample.Data())).find("Run2011")!=string::npos ? 
      1.0 : Lumi3DReWeighting->weight3D(nPUVerticesM1, nPUVertices, nPUVerticesP1) ;
    embeddingWeight_    = embeddingWeight;

    HqTWeight = histo!=0 ? histo->GetBinContent( histo->FindBin( (*genVP4)[0].Pt() ) ) : 1.0;

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
	bool isTriggMatched = ((*tauXTriggers)[0] || (*tauXTriggers)[1]) && (*tauXTriggers)[6] ; 
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

	HLTTau  = ((*diTauLegsP4)[1].Eta()<1.5) ? 
	  turnOnTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
	  turnOnTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;

	SFTau         = 1.0;
	SFElec        =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ? 
	  ratioElecIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :
	  ratioElecIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;

	  //ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
	  //ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;

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
      HLTmatch = ((*tauXTriggers)[0] && (*tauXTriggers)[2]) ? 1.0 : 0.0;

      //HLTweightTau  = ratioTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTweightTau  = ((*diTauLegsP4)[1].Eta()<1.5) ? 
	ratioTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
	ratioTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;
     
      if((*diTauLegsP4)[0].Pt()>18){
	HLTweightElec = ((*diTauLegsP4)[0].Eta()<1.5) ? 
	  ratioElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	  ratioElecAllEC->Eval((*diTauLegsP4)[0].Pt());
      }
      else HLTweightElec = 1.0;

      //HLTTau  = turnOnTauElecTauAll->Eval( (*diTauLegsP4)[1].Pt() );
      HLTTau  = ((*diTauLegsP4)[1].Eta()<1.5) ? 
	turnOnTauElecTauAllBL->Eval( (*diTauLegsP4)[1].Pt() ) :
	turnOnTauElecTauAllEC->Eval( (*diTauLegsP4)[1].Pt() ) ;

      HLTElec = TMath::Abs((*diTauLegsP4)[0].Eta()<1.5) ? 
	turnOnElecAllBL->Eval((*diTauLegsP4)[0].Pt()) : 
	turnOnElecAllEC->Eval((*diTauLegsP4)[0].Pt());

      SFTau  = 1.0;
      SFElec =  TMath::Abs((*diTauLegsP4)[0].Eta())<1.5 ?
	ratioElecIDIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) :
	ratioElecIDIsoEC->Eval( (*diTauLegsP4)[0].Pt() ) ;
      //ratioElecIDBL->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoBL->Eval( (*diTauLegsP4)[0].Pt() ) : 
      //ratioElecIDEC->Eval( (*diTauLegsP4)[0].Pt() )*ratioElecIsoEC->Eval( (*diTauLegsP4)[0].Pt() );

      SFEtoTau = 1.0;

    }
 
     isTauLegMatched_ = isTauLegMatched;
    if((std::string(sample.Data())).find("Run2011")==string::npos)
      leptFakeTau      = (isTauLegMatched==0 && (*genDiTauLegsP4)[1].E()>0) ? 1 : 0;
    else 
      leptFakeTau = -99;


    if(leptFakeTau==1){
      float extraSmearing = ran->Gaus( -0.373, 1.18 );
      diTauVisMass += extraSmearing;
      SFEtoTau      = TMath::Abs((*diTauLegsP4)[1].Eta())<1.5 ? 1.152 : 1.01;
    }


    /*
    if(isElecLegMatched && genDiTauLegsP4->size()>0 ){
    if(run <=175860 ){ // RunA
    if(TMath::Abs(etaL1)<1.5) 
    smear( ptL1, (*genDiTauLegsP4)[0].Pt(), -0.398e-02 , 1.513e-02, n) ;
    else
    smear( ptL1, (*genDiTauLegsP4)[0].Pt(), -1.096e-02 , 4.464e-02, n) ; 
    }
    else{              // RunB
    if(TMath::Abs(etaL1)<1.5) 
    smear( ptL1, (*genDiTauLegsP4)[0].Pt(), -0.884e-02 , 1.586e-02, n) ;
    else
    smear( ptL1, (*genDiTauLegsP4)[0].Pt(),  1.499e-02 , 4.321e-02, n) ; 
    }
    }
    */

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
 delete gammadEta; delete gammadPhi; delete gammaPt; delete Lumi3DReWeighting;
 delete ran;

 return;
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



void makeTrees_MuTauStream(string analysis = "", int index = -1 ){
  
  cout << "Now skimming analysis " << analysis << endl;
  
  gSystem->Load("Lumi3DReWeightingForLorenzo_cc.so");
  edm::Lumi3DReWeightingForLorenzo* Lumi3DReWeighting = 
    new edm::Lumi3DReWeightingForLorenzo("pileUp/pileUpFall11.root",
					 "pileUp/Run2011PileUpTruth.root",
					 "MC_distr",
					 "pileup");
  //Lumi3DReWeighting->weight3D_init(73.5/68.);
  Lumi3DReWeighting->weight3D_init(1.00);
  cout << "Lumi3D rewieghting initialized" << endl;


  gSystem->Load("RecoilCorrector_hh.so");
  RecoilCorrector* recoilCorr = 0;

  TFile corrections("llrCorrections.root");
  
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

  if(!ratioMuIDIsoBL)   cout << "Missing corrections for MuID+Iso (BL)" << endl;
  if(!ratioMuIDIsoEC)   cout << "Missing corrections for MuID+Iso (EC)" << endl;
 
  if(!ratioMuIdBL)      cout << "Missing corrections for MuID (BL)" << endl;
  if(!ratioMuIdEC)      cout << "Missing corrections for MuID (EC)" << endl;
  if(!ratioMuIsoBL)     cout << "Missing corrections for MuIso (BL)" << endl;
  if(!ratioMuIsoEC)     cout << "Missing corrections for MuIso (EC)" << endl;
  if(!ratioMuAllBL)     cout << "Missing corrections for Mu HLT (BL)" << endl;
  if(!ratioMuAllEC)     cout << "Missing corrections for Mu HLT (EC)" << endl;
  if(!ratioTauMuTauAll) cout << "Missing corrections for tau HLT" << endl;
  if(!ratioTauMuTauAllBL) cout << "Missing corrections for tau HLT (BL)" << endl;
  if(!ratioTauMuTauAllEC) cout << "Missing corrections for tau HLT (EC)" << endl;
  if(!turnOnMuAllBL)    cout << "Missing turnOn for mu (BL)" << endl;
  if(!turnOnMuAllEC)    cout << "Missing turnOn for mu (EC)" << endl;
  if(!turnOnTauMuTauAll)cout << "Missing turnOn for tau" << endl;

  typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LV;

  std::vector<std::string> samples;
  std::vector<float> crossSec;

  // samples & x-sections & skim1 & skim2
  samples.push_back("Run2011-MuTau-All_run");             crossSec.push_back( 0  );                          
  samples.push_back("Run2011-MuTau-LooseIso-All_run");    crossSec.push_back( 0  );                          
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
  samples.push_back("VH100-MuTau-pythia-PUS6_run");  crossSec.push_back( 2.61e-02*(1.186+ 0.6313+0.1638  ) * 0.082128 * 0.834434);  
  samples.push_back("VH110-MuTau-pythia-PUS6_run");  crossSec.push_back( 8.02e-02*(0.8754+0.4721+0.1257  ) * 0.088174 * 0.839732);
  samples.push_back("VH115-MuTau-pythia-PUS6_run");  crossSec.push_back( 7.65e-02*(0.7546+0.4107+0.1106  ) * 0.092032 * 0.841206);
  samples.push_back("VH120-MuTau-pythia-PUS6_run");  crossSec.push_back( 7.10e-02*(0.6561+0.3598+0.09756 ) * 1.0      * 0.1654);   
  samples.push_back("VH125-MuTau-pythia-PUS6_run");  crossSec.push_back( 6.37e-02*(0.5729+0.3158+0.08634 ) * 0.09885  * 0.842507);
  samples.push_back("VH130-MuTau-pythia-PUS6_run");  crossSec.push_back( 5.48e-02*(0.4942+0.2778+0.07658 ) * 1.0      * 0.177053);
  samples.push_back("VH135-MuTau-pythia-PUS6_run");  crossSec.push_back( 4.52e-02*(0.4390+0.2453+0.06810 ) * 0.104662 * 0.841589);
  samples.push_back("VH140-MuTau-pythia-PUS6_run");  crossSec.push_back( 3.54e-02*(0.3857+0.2172+0.06072 ) * 0.108278 * 0.842954);  
  samples.push_back("VH145-MuTau-pythia-PUS6_run");  crossSec.push_back( 2.61e-02*(0.3406+0.1930+0.05435 ) * 1.0      * 0.191843);  
  samples.push_back("VH160-MuTau-pythia-PUS6_run");  crossSec.push_back( 5.32e-04*(0.2291+0.1334+0.03942 ) * 1.0      * 0.203857);  
  


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
  float diTauRecoPt;
  float diTauMinMass;

  // taus/MET related variables
  float ptL1,ptL2,etaL1,etaL2,phiL1,phiL2,dPhiL1L2, dxy1_, dz1_;
  float diTauCharge_,MtLeg1_,MtLeg1Corr_,pZeta_,pZetaCorr_,pZetaVis_,pZetaVisCorr_,MEt,MEtCorr,MEtPhi,pZetaSig_;
  float combRelIsoLeg1,combRelIsoLeg1Beta,combRelIsoLeg1DBeta,combRelIsoLeg1Rho, combIsoLeg2;
  int tightestHPSDBWP_, decayMode_;

  //tau related variables
  float HoP,EoP, emFraction_, leadPFChargedHadrMva_;
  float hasGsf_, signalPFGammaCands_, signalPFChargedHadrCands_;
  float etaMom2,phiMom2,gammaFrac,visibleTauMass_;

  // event-related variables
  float numPV_ , sampleWeight, puWeight, embeddingWeight_,HqTWeight;
  int numOfLooseIsoDiTaus_;
 
  // object-related weights and triggers
  float HLTx,HLTmu,HLTmatch,HLTweightTau, HLTweightMu, SFMu, SFTau, HLTMu, HLTTau;
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
  outTreePtOrd->Branch("diTauRecoPt",   &diTauRecoPt,  "diTauRecoPt/F");
  
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
  outTreePtOrd->Branch("leptFakeTau",       &leptFakeTau,"leptFakeTau/I");

  outTreePtOrd->Branch("event",&event_,"event/l");
  outTreePtOrd->Branch("run",  &run_,  "run/l");
  outTreePtOrd->Branch("lumi", &lumi_, "lumi/l");
 
  string currentInName = index >= 0 ?  "/data_CMS/cms/lbianchini/MuTauStreamFall11_06Dec2011///treeMuTauStream_"+samples[index]+".root" : "../treeMuTauStream.root";

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
  currentTree->SetBranchStatus("nPUVerticesP1"         ,1);
  currentTree->SetBranchStatus("nPUVerticesM1"         ,1);
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
  int isTauLegMatched,muFlag,genDecay, nPUVertices, nPUVerticesM1, nPUVerticesP1;
  float rhoFastJet,rhoNeutralFastJet;
  float visibleTauMass;
  float dxy1, dz1;
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

  unsigned int indexHelp = index >=0 ? index : 0;

  if(samples[indexHelp].find("WJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_wjets_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(samples[indexHelp].find("DYJets")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zjets_ltau_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
  else if(samples[indexHelp].find("H1")!=string::npos){
    recoilCorr = new RecoilCorrector("../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_higgs_njet.root");
    recoilCorr->addMCFile(           "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_zmm42X_njet.root");
    recoilCorr->addDataFile(         "../../../Utilities/data/recoilv3/RecoilCorrector_v2/recoilfits/recoilfit_datamm_njet.root");
  }
    

  TFile* HqT      = 0;
  int mH          = 120;
  TH1F* histo     = 0;
  if(samples[indexHelp].find("GGFH")!=string::npos){
    if(samples[indexHelp].find("GGFH100")!=string::npos) mH = 100;
    if(samples[indexHelp].find("GGFH105")!=string::npos) mH = 105;
    if(samples[indexHelp].find("GGFH110")!=string::npos) mH = 110;
    if(samples[indexHelp].find("GGFH115")!=string::npos) mH = 115;
    if(samples[indexHelp].find("GGFH120")!=string::npos) mH = 120;
    if(samples[indexHelp].find("GGFH125")!=string::npos) mH = 125;
    if(samples[indexHelp].find("GGFH130")!=string::npos) mH = 130;
    if(samples[indexHelp].find("GGFH135")!=string::npos) mH = 135;
    if(samples[indexHelp].find("GGFH140")!=string::npos) mH = 140;
    if(samples[indexHelp].find("GGFH145")!=string::npos) mH = 145;
    if(samples[indexHelp].find("GGFH160")!=string::npos) mH = 160;
    cout << "Reweighting powheg with HqT mH=" << mH << endl;
    HqT = new TFile(Form("../../../Utilities/data/HqTFeHiPro/weight_ptH_%d.root", mH));
    if(!HqT) cout << "Cannot find HqT file..." << endl;
    else{
      histo = (TH1F*)(HqT->Get(Form("powheg_weight/weight_hqt_fehipro_fit_%d",mH)));
    }
  }


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

    diTauRecoPt = ((*diTauVisP4)[0]+(*METP4)[0]).Pt();

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
      if(samples[indexHelp].find("WJets")  !=string::npos)      
	recoilCorr->CorrectType2(corrPt,corrPhi,(*genVP4)[0].Pt() ,(*genVP4)[0].Phi() , 
				 ((*diTauLegsP4)[0]).Pt(),((*diTauLegsP4)[0]).Phi(), u1, u2 , err1,err2, TMath::Min(nJets30,2) );
      else if(samples[indexHelp].find("DYJets")!=string::npos || samples[indexHelp].find("H1")!=string::npos)  
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
  delete gammadEta; delete gammadPhi; delete gammaPt; delete HqT;
  
  return;

}


void doAllSamplesElec(){

  /*
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
  */
  
  for( unsigned int k = 1; k < 2/*42*/ ; k++) {

    makeTrees_ElecTauStream("",                  k/*,antiE*/);
    if( k < 2) continue;
    makeTrees_ElecTauStream("TauUp",             k/*,antiE*/);
    makeTrees_ElecTauStream("TauDown",           k/*,antiE*/);
    if( k < 3) continue;
    makeTrees_ElecTauStream("JetUp",             k/*,antiE*/);
    makeTrees_ElecTauStream("JetDown",           k/*,antiE*/);
    makeTrees_ElecTauStream("MEtResolutionUp",   k/*,antiE*/);
    makeTrees_ElecTauStream("MEtResolutionUp",   k/*,antiE*/);
    makeTrees_ElecTauStream("MEtResponseUp",     k/*,antiE*/);
    makeTrees_ElecTauStream("MEtResponseUp",     k/*,antiE*/);
    makeTrees_ElecTauStream("ElecUp",            k/*,antiE*/);
    makeTrees_ElecTauStream("ElecDown",          k/*,antiE*/);
    
  }
  
  return;
  
}


void doAllSamplesMu(){
 
  for( unsigned int k = 0; k < 42 ; k++) {

    makeTrees_MuTauStream("",        k);

    //continue;

    if( k<2) continue;
    makeTrees_MuTauStream("TauUp",   k);
    makeTrees_MuTauStream("TauDown", k);
    if( k<3) continue;
    makeTrees_MuTauStream("JetUp",   k);
    makeTrees_MuTauStream("JetDown", k);
    //makeTrees_MuTauStream("MEtResolutionUp",   k);
    //makeTrees_MuTauStream("MEtResolutionDown", k);
    //makeTrees_MuTauStream("MEtResponseUp",     k);
    //makeTrees_MuTauStream("MEtResponseDown",   k);
   
    //makeTrees_MuTauStream("MuUp",    k);
    //makeTrees_MuTauStream("MuDown",  k);
  }

  return;

}

